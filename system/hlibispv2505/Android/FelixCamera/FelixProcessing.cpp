/**
******************************************************************************
@file FelixProcessing.cpp

@brief Definition of FelixProcessing request processing thread class
in v2500 Camera HAL

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

*****************************************************************************/

#define LOG_TAG "FelixProcessing"

#include "FelixCamera.h"
#include "FelixProcessing.h"
#include "CaptureRequest.h"
#include "Scaler.h"
#include "Helpers.h"

#include <utils/SystemClock.h>
#include <ui/Rect.h>
#include <ui/GraphicBufferMapper.h>

#include "AAA/FelixAWB.h"
#include "AAA/FelixAE.h"
#include "AAA/FelixAF.h"
#include <felixcommon/userlog.h>

#define PRESCALE_IMAGE_FOR_JPEG_COMPRESSOR

// ---------------------------------------------------------------------------/
// Static definitions. Available in this file only.
// ---------------------------------------------------------------------------/
namespace { //init values

__maybe_unused char metadataStr[100] = { 0 };

} //anonymous namespace

// ---------------------------------------------------------------------------/
// Processing thread
// ---------------------------------------------------------------------------/
namespace android {

ProcessingThread::ProcessingThread(FelixCamera &parent) :
        mPrevFrameTimestamp(),
        mParent(parent),
        mCameraHw(parent.getHwInstance()),
        mThreadActive(false),
        mFlushing(false),
        mReprocessWaiting(false){
    LOG_DEBUG("E");
}

ProcessingThread::~ProcessingThread() {
    LOG_DEBUG("E");
    flushQueue();
}

bool ProcessingThread::isIdle() {
    Mutex::Autolock l(mLock);
    return !mThreadActive;
}

void ProcessingThread::tryIdle() {
    bool signal = false;
    {
        Mutex::Autolock l(mLock);
        if (mCaptureRequestsQueue.empty()) {
            mThreadActive = false;
            mPrevFrameTimestamp = 0;
            mCameraHw.initHwNanoTimer();
            signal = true;
        }
    }
    if(signal) {
        mParent.signalProcessingThreadIdle();
    }
}

void ProcessingThread::queueCaptureRequest(
        CaptureRequest &internalRequest) {
    status_t res;
    Mutex::Autolock l(mLock);

    // to mitigate the excessive framework errors in Kitkat
    //
    // E Camera3-IOStreamBase: getBufferPreconditionCheckLocked:
    // Stream 0: Already dequeued maximum number of simultaneous buffers (7)
    //
    // Wait until we can enqueue the request and exit from
    // processCaptureRequest

    // Check if 'more than' just to be sure...
    // why kMaxBufferCount-2 ? because:
    // 1. threadLoop actually removes current 'in-flight'
    // request from the thread queue prior to acquiring and processing
    // 2. another buffer is already pending to be requested in the framework
    while(mCaptureRequestsQueue.size() >= mParent.kMaxBufferCount-2)
    {
        res = mCaptureResultSentSignal.waitRelative(mLock, kWaitPerLoop);
        if(mFlushing) {
            internalRequest.mRequestError = true;
            // if flush() called in the meantime, set the error in request and
            // go on as quickly as possible
            break;
        }
        if (res == TIMED_OUT) {
            continue;
        } else if (res != NO_ERROR) {
            LOG_ERROR("Error waiting for queue: %d", res);
            return;
        }
    }
    mCaptureRequestsQueue.push_back(sp<CaptureRequest>(&internalRequest));

    // may have been stopped by flush()
    if (!isRunning()) {
        run("FelixCamera::processingThread");
    }
    mThreadActive = true;
    mCaptureRequestQueuedSignal.signal();
}

void ProcessingThread::popCaptureRequest(void)
{
    Mutex::Autolock l(mLock);
    // remove capture request from queue.
    mCaptureRequestsQueue.erase(mCaptureRequestsQueue.begin());
}

#ifdef USE_GPU_LIB
status_t ProcessingThread::processISandFD(
        CaptureRequest &captureRequest)
{
    int bufferToUse;
    int bufferToUseHint;
    camera_frame_metadata_t fdMetaData;
    uint8_t maxFaces;

    // print all streams
    for(int i=0; i<mParent.mGPUstream.size(); i++) {
        const GPUstreamInfo_t &d = mParent.mGPUstream.itemAt(i);
        mParent.mGPUstream.editItemAt(i).requested = false;
        LOG_DEBUG("%d requested %d, stream %p, target %d, d.requestedCount %d, d.usedCount %d", __LINE__, d.requested, d.stream, d.target, d.requestedCount, d.usedCount);
    }

    mParent.mGPU->gpuGetMaxFaces( &maxFaces );
    fdMetaData.faces = (camera_face_t*)malloc(maxFaces*sizeof(camera_face_t));

//#define MAX_FD_DATA_SIZE sizeof(camera_face_t.rect)
//#define SIZEOF_CAMERA_RACE_T_RECT   (sizeof(int32_t)*4)
    int32_t *fdData_i32 = (int32_t*)malloc(maxFaces*(sizeof(int32_t)*4));
    uint8_t *fdData_u8 = (uint8_t*)malloc(maxFaces*sizeof(uint8_t));

    /* select buffer to process
     * from received buffers select best candidate for vision processing,
     * the order is encoder, preview,
     * using the same loop find matching string in registered streams (must be there)
     * update usage count
     */
    bufferToUseHint = -1;
    bufferToUse = -1;
    for(int x=0; x<captureRequest.mBuffers.size(); x++)
    {
        const camera3_stream_buffer_t& captureBuf = captureRequest.mBuffers.itemAt(x);
        if( ( HAL_PIXEL_FORMAT_YCbCr_420_888             == captureBuf.stream->format )||
            ( HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED    == captureBuf.stream->format  &&
              isEncoderUsage(captureBuf.stream->usage)))
        {
            bufferToUseHint = x;
            for(int i=0; i<mParent.mGPUstream.size(); i++) {
                if(mParent.mGPUstream.itemAt(i).stream == captureBuf.stream) {
                    mParent.mGPUstream.editItemAt(i).requested = true;
                }
            }
        }
//        LOG_DEBUG("%d index %d size %dx%d captureBuf.stream = %p "
//                "captureBuf.stream->format = %d captureBuf.stream->usage = 0x%x",
//                __LINE__, x, captureBuf.stream->width, captureBuf.stream->height, captureBuf.stream,
//                captureBuf.stream->format, captureBuf.stream->usage);

        if( ( HAL_PIXEL_FORMAT_YCrCb_420_SP             == captureBuf.stream->format ) ||
            ( HAL_PIXEL_FORMAT_RGBX_8888                == captureBuf.stream->format ) ||
            ( HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED   == captureBuf.stream->format   &&
              isDisplayUsage(captureBuf.stream->usage)))
        {
            if(bufferToUseHint < 0) // not selected encoder usage yet
            {
                // preview, second best candidate
                bufferToUseHint = x;
            }
            for(int i=0; i<mParent.mGPUstream.size(); i++) {
                if(mParent.mGPUstream.itemAt(i).stream == captureBuf.stream) {
                    mParent.mGPUstream.editItemAt(i).requested = true;
                }
            }
        }
    }
    // print all streams
//    LOG_DEBUG("%d mParent.mGPUstream.size() = %d", __LINE__, mParent.mGPUstream.size());
//    for(int i=0; i<mParent.mGPUstream.size(); i++) {
//        const GPUstreamInfo_t &d = mParent.mGPUstream.itemAt(i);
//        LOG_DEBUG("%d requested %d, stream %p, target %d, d.requestedCount %d, d.usedCount %d", __LINE__, d.requested, d.stream, d.target, d.requestedCount, d.usedCount);
//    }
    /* update requestedCount count, if buffer in this request matches the stream in our stream list, then the requestedCount
     * of the stream is increased
     */
    for(int i=0; i<mParent.mGPUstream.size(); i++) {
        int requestedCount = mParent.mGPUstream.itemAt(i).requestedCount;
        if(mParent.mGPUstream.itemAt(i).requested) {
            mParent.mGPUstream.editItemAt(i).requestedCount = (requestedCount == MAX_GPU_STREAM_USE_COUNT) ? requestedCount : ++requestedCount;
        } else {
            mParent.mGPUstream.editItemAt(i).requestedCount = ((requestedCount > 0) ? --requestedCount : 0);
        }
    }

    if(-1 == bufferToUseHint) {
        // none of the buffers seem to be for IS nor FD
        LOG_DEBUG("%d return;", __LINE__);
        return OK;
    }

    if(-1 != mParent.mGPUstreamCurrentIndex) {
        if(mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).stream == captureRequest.mBuffers.itemAt(bufferToUseHint).stream) {
            bufferToUse = bufferToUseHint;
            int usedCount = mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).usedCount;
            mParent.mGPUstream.editItemAt(mParent.mGPUstreamCurrentIndex).usedCount = (usedCount >= MAX_GPU_STREAM_USE_COUNT) ? MAX_GPU_STREAM_USE_COUNT: ++usedCount;
        }
    }

    if(-1 == bufferToUse) {
        /* the "best" input buffer does not match current stream
         * search streams to find match to the best buffer
         */
        for(int i=0; i<mParent.mGPUstream.size(); i++) {
            if(mParent.mGPUstream.itemAt(i).stream == captureRequest.mBuffers.itemAt(bufferToUseHint).stream) {
                int usedCount;
                if(mParent.mGPUstreamCurrentIndex >= 0) {
                    if(mParent.mGPUstream.itemAt(i).target >= mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).target) {
                        usedCount = mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).usedCount;
                        mParent.mGPUstream.editItemAt(mParent.mGPUstreamCurrentIndex).usedCount = 0;
                        mParent.mGPUstreamCurrentIndex = i;
                        mParent.reconfigureCameraGPU(mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).stream);
                        usedCount = mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).usedCount;
                        mParent.mGPUstream.editItemAt(mParent.mGPUstreamCurrentIndex).usedCount = (usedCount >= MAX_GPU_STREAM_USE_COUNT) ? MAX_GPU_STREAM_USE_COUNT: ++usedCount;
                        bufferToUse = bufferToUseHint;
                    } else {
                        if(0 == mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).usedCount) {
                            mParent.mGPUstreamCurrentIndex = i;
                            usedCount = mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).usedCount;
                            mParent.mGPUstream.editItemAt(mParent.mGPUstreamCurrentIndex).usedCount = (usedCount >= MAX_GPU_STREAM_USE_COUNT) ? MAX_GPU_STREAM_USE_COUNT: ++usedCount;
                            bufferToUse = bufferToUseHint;
                            mParent.reconfigureCameraGPU(mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).stream);
                        } else {
                            mParent.mGPUstream.editItemAt(mParent.mGPUstreamCurrentIndex).usedCount =
                                    mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).usedCount - 1;
                        }
                    }
                } else  { // if(mParent.mGPUstreamCurrentIndex >= 0) {
                    mParent.mGPUstreamCurrentIndex = i;
                    usedCount = mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).usedCount;
                    mParent.mGPUstream.editItemAt(mParent.mGPUstreamCurrentIndex).usedCount = (usedCount >= MAX_GPU_STREAM_USE_COUNT) ? MAX_GPU_STREAM_USE_COUNT: ++usedCount;
                    bufferToUse = bufferToUseHint;
                    mParent.reconfigureCameraGPU(mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).stream);
                }
            }
        }
    }

    if(-1 == bufferToUse) {
        LOG_DEBUG("%d skip face detection for this buffer", __LINE__);
        // TOVERIFY:        decided to skip this round, should we we set faces found to 0?
        return OK;
    }

    {
        uint8_t *ptr = NULL;
        int64_t timeStamp=0;    // hr_TODO: actual timestamp required

        const camera3_stream_buffer_t& captureBuf = captureRequest.mBuffers.itemAt(bufferToUse);

        // stream to which buffer belongs matches the stream to be processed by vision.
        switch(mParent.mGPUstream.itemAt(mParent.mGPUstreamCurrentIndex).target) {
        case    GPU_STREAM_PREVIEW_RGB:
            LOG_DEBUG("%d GPU_STREAM_PREVIEW_RGB", __LINE__);
            mParent.mGPU->gpuDetectFaces( (uint8_t*)*captureBuf.buffer, /*is YUV*/false, &fdMetaData);
            break;
        case    GPU_STREAM_ENCODER: // YUV
            LOG_DEBUG("%d GPU_STREAM_ENCODER", __LINE__);
            //            mParent.mGPU->gpuGetStabilizedFrame(timeStamp, (uint8_t*)*captureBuf.buffer, (uint8_t*)*captureBuf.buffer, NULL, NULL);
            mParent.mGPU->gpuDetectFaces( (uint8_t*)*captureBuf.buffer, /*is YUV*/true, &fdMetaData);
            break;
        case    GPU_STREAM_PREVIEW_YUV:
            LOG_DEBUG("%d GPU_STREAM_PREVIEW_YUV", __LINE__);
            mParent.mGPU->gpuDetectFaces( (uint8_t*)*captureBuf.buffer, /*is YUV*/true, &fdMetaData);
            break;
        default:
            break;
        }

        {
            // rescale face coordinates to sensor dimensions
            uint32_t sensorWidth, sensorHeight;
            int32_t gpuWidth;
            int32_t gpuHeight;
            mParent.mCameraHw.getSensorResolution(sensorWidth, sensorHeight);
            mParent.mGPU->getFdResolution(gpuWidth, gpuHeight);
            for(int x=0; x<fdMetaData.number_of_faces; x++) {
                fdMetaData.faces[x].rect[0] = (fdMetaData.faces[x].rect[0] * sensorWidth)/gpuWidth;
                fdMetaData.faces[x].rect[1] = (fdMetaData.faces[x].rect[1] * sensorHeight)/gpuHeight;
                fdMetaData.faces[x].rect[2] = (fdMetaData.faces[x].rect[2] * sensorWidth)/gpuWidth;
                fdMetaData.faces[x].rect[3] = (fdMetaData.faces[x].rect[3] * sensorHeight)/gpuHeight;
            }
        }

        // provide faces
        camera_metadata_enum_android_statistics_face_detect_mode_t fdMode;
        camera_metadata_entry_t e =
            captureRequest.mFrameSettings->find(ANDROID_STATISTICS_FACE_DETECT_MODE);

        LOG_DEBUG("ANDROID_STATISTICS_FACE_DETECT_MODE requested is %s", metadataStr);
        if(e.count && e.data.u8)
        {
            fdMode = (camera_metadata_enum_android_statistics_face_detect_mode_t)*e.data.u8;
            camera_metadata_enum_snprint(ANDROID_STATISTICS_FACE_DETECT_MODE, fdMode,
                    metadataStr, sizeof(metadataStr));
        }
        else
        {
            LOG_WARNING("suspicious face detect mode requested = %d (0 - off, 1 - simple, 2 - full)",  (uint8_t)fdMode);
        }

        uint8_t numFaces;
        mParent.mGPU->gpuGetNumFaces( &numFaces );
        // providing artificial faces
#ifdef FACE_DETECT_MODE_FULL_SUPPORTED
        if((ANDROID_STATISTICS_FACE_DETECT_MODE_SIMPLE == fdMode) || (ANDROID_STATISTICS_FACE_DETECT_MODE_FULL == fdMode))
#else
        if(ANDROID_STATISTICS_FACE_DETECT_MODE_SIMPLE == fdMode)
#endif
        {
            uint fdDataIndex, fdDataItemSize;
            // ANDROID_STATISTICS_FACE_RECTANGLES,               // int32[]      | public
            fdDataItemSize = sizeof(fdMetaData.faces[0].rect)/sizeof(fdMetaData.faces[0].rect[0]);
            for(int x=0; x<numFaces; x++) {
                fdDataIndex = x * fdDataItemSize;
                memcpy(&fdData_i32[fdDataIndex], &fdMetaData.faces[x].rect[0], sizeof(fdMetaData.faces[0].rect));
            }
            captureRequest.mFrameSettings->update(ANDROID_STATISTICS_FACE_RECTANGLES, (int32_t *)&fdData_i32[0], numFaces*4);

            //ANDROID_STATISTICS_FACE_SCORES,                   // byte[]       | public
            for(int x=0; x<numFaces; x++) {
                fdData_u8[x] = (uint8_t)fdMetaData.faces[x].score;
            }
            captureRequest.mFrameSettings->update(ANDROID_STATISTICS_FACE_SCORES, &fdData_u8[0], numFaces);
        }

#ifdef FACE_DETECT_MODE_FULL_SUPPORTED
        if(ANDROID_STATISTICS_FACE_DETECT_MODE_FULL == fdMode)
        {
            uint fdDataIndex, fdDataItemSize;
            //        ANDROID_STATISTICS_FACE_IDS,                      // int32[]      | public
            for(int x=0; x<numFaces; x++)
            {
                fdData_i32[x] = fdMetaData.faces[x].id;
            }
            captureRequest->mFrameSettings->update(ANDROID_STATISTICS_FACE_IDS, (int32_t *)&fdData_i32[0], numFaces);

            //        ANDROID_STATISTICS_FACE_LANDMARKS,                // int32[]      | public
            fdDataItemSize =    sizeof(fdMetaData.faces[0].left_eye) *
                                sizeof(fdMetaData.faces[0].right_eye) *
                                sizeof(fdMetaData.faces[0].mouth);
            for(int x=0; x<numFaces; x++)
            {
                fdDataIndex = x * fdDataItemSize;
                memcpy(&fdData_i32[x], &fdMetaData.faces[x].rect[0], fdDataItemSize);
            }
            captureRequest->mFrameSettings->update(ANDROID_STATISTICS_FACE_LANDMARKS, (int32_t *)&fdData_i32[0],
                    numFaces*6);

        }
#endif
    }

    free(fdData_i32);
    free(fdData_u8);
    return OK;
}
#endif

void ProcessingThread::signalCaptureResultSent(void)
{
    mCaptureResultSentSignal.signal();
}

// NOTE: mJpegLock shall be acquired prior to calling
// processJpegEncodingRequest()
status_t ProcessingThread::processJpegEncodingRequest(
        CaptureRequest &captureRequest) {
    LOG_DEBUG("E");

    sp<StreamBuffer> encoderInputBuffer;
    sp<StreamBuffer> encoderOutputBuffer;
    bool prescaleImage = false;

    // set new depth of pipeline processing for current request
    captureRequest.newPipelineStep();

    // ---------------------------------------------------------------------
    // Prepare JPEG encoder input buffer
    // Prescale if needed
    // ---------------------------------------------------------------------
    if((captureRequest.mInputBuffer.stream->width !=
           captureRequest.mReprocessOutputBuffer.stream->width) ||
           (captureRequest.mInputBuffer.stream->height !=
           captureRequest.mReprocessOutputBuffer.stream->height)) {
#ifdef PRESCALE_IMAGE_FOR_JPEG_COMPRESSOR
        prescaleImage = true;
#else
        LOG_WARNING("Dimensions of source buffer for JPEG compressor are "\
                    "different than destination, but scaling not enabled!");
#endif /* PRESCALE_IMAGE_FOR_JPEG_COMPRESSOR */
    }

    if(prescaleImage) {
        // alloc destination buffer for prescaled image,
        // set as encoderInputBuffer
        // dimensions of output buffer, pixel format same as input buffer
        // allocate buffer in the constructor
        captureRequest.mPrescaledInputBuffer =
            static_cast<StreamBuffer*>(new localStreamBuffer(
                            captureRequest.mReprocessOutputBuffer.stream->width,
                            captureRequest.mReprocessOutputBuffer.stream->height,
                            captureRequest.mInputBuffer.stream->format,
                            captureRequest.mInputBuffer.stream->usage |
                            GRALLOC_USAGE_SW_WRITE_RARELY |
                            GRALLOC_USAGE_HW_CAMERA_WRITE));

        // calculate crop rectangle of input buffer to maintain the same aspect
        // ratio of prescaled input buffer and output JPEG image
        StreamBuffer croppedInput(captureRequest.mInputBuffer);
        Rect rect;
        if(captureRequest.mFrameSettings->calculateStreamCrop(
                captureRequest.mReprocessOutputBuffer.stream->width,
                captureRequest.mReprocessOutputBuffer.stream->height,
                rect, false) == NO_ERROR) {
            croppedInput.cropAndLock(rect,
                    GRALLOC_USAGE_HW_CAMERA_READ | // hw processing
                    GRALLOC_USAGE_SW_READ_RARELY); // sw processing
        }
            // if the buffers are not locked prior to scale() -
            // they are locked by it with default rectangles
        ImgScaler::get().scale(
                 croppedInput,
                *captureRequest.mPrescaledInputBuffer);

        // use mInputScaledBuffer buffer as source
        // for JPEG compressed main image
        encoderInputBuffer = captureRequest.mPrescaledInputBuffer;

    } else { // prescaleImage == false
        // use mInputBuffer directly as compressor source
        // WARNING - no cropping and no scaling is done!
        encoderInputBuffer = new StreamBuffer(captureRequest.mInputBuffer);
        ALOG_ASSERT(encoderInputBuffer->buffer);
    }

    // ------------------------------------------------------------------------
    // Prepare JPEG encoder output buffer
    // ------------------------------------------------------------------------
    encoderOutputBuffer =
        new StreamBuffer(captureRequest.mReprocessOutputBuffer);

    LOG_DEBUG("encoding 0x%x (%p) to JPEG (%p).",
            encoderInputBuffer->stream->format,
            *encoderInputBuffer->buffer, *encoderOutputBuffer->buffer);

    // Store data for capture result.
    mReprocessWaiting = true;
    mReprocessingRequest = &captureRequest;
    JpegEncoder& jpeg = JpegEncoder::get();
    jpeg.setInputBuffer(*encoderInputBuffer);
    jpeg.setOutputBuffer(*encoderOutputBuffer);
    jpeg.setRequestMetadata(*captureRequest.mFrameSettings);
    jpeg.setJpegListener(this);
    if (jpeg.start() != OK) {
        LOG_ERROR("JPEG compressor cannot be started!");
        captureRequest.mRequestError = true;
        return UNKNOWN_ERROR;
    }
    LOG_DEBUG("encoding to JPEG started.");

    return OK;
}

status_t ProcessingThread::processRegularRequest(
        CaptureRequest &captureRequest, nsecs_t &frameTimestampNs) {
    LOG_DEBUG("E");
    status_t res = NO_ERROR;
    // process metadata for current request and set the internal 3A
    // state machines accordingly
    FelixAWB *awbModule =
        mCameraHw.getMainPipeline()->getControlModule<FelixAWB>();
    if(awbModule) {
        if(awbModule->processDeferredHALMetadata(
                *captureRequest.mFrameSettings) != NO_ERROR) {
            captureRequest.mMetadataError = true;
        }
    }
    FelixAF *afModule =
            mCameraHw.getMainPipeline()->getControlModule<FelixAF>();
    if(afModule) {
        if(afModule->processDeferredHALMetadata(
                *captureRequest.mFrameSettings) != NO_ERROR) {
            captureRequest.mMetadataError = true;
        }
    }
    FelixAE *aeModule =
            mCameraHw.getMainPipeline()->getControlModule<FelixAE>();
    if(aeModule) {
        if(aeModule->processDeferredHALMetadata(
                *captureRequest.mFrameSettings) != NO_ERROR) {
            captureRequest.mMetadataError = true;
        }
    }

    ISPC::Shot acquiredShot;
    res = captureRequest.acquireShot(acquiredShot);

    if(res == OK) {
        // handle flickering
        captureRequest.mFrameSettings->processFlickering(acquiredShot.metadata);

        // handle histogram ANDROID_STATISTICS_HISTOGRAM from
        // acquiredShot.metadata.histogramStats
        captureRequest.mFrameSettings->processHistogram(acquiredShot.metadata);

        // Notify the framework of the next capture exposure time
        nsecs_t durationNs = 0;
        processTimestamp(acquiredShot.metadata.timestamps.ui32StartFrameIn,
               durationNs, frameTimestampNs);

        captureRequest.mFrameSettings->updateFrameDuration(durationNs);
        // : check if these timestamps can be used instead of
        // actual start of exposures
        captureRequest.mFrameSettings->updateRollingShutterSkew(
            acquiredShot.metadata.timestamps.ui32StartFrameIn,
            acquiredShot.metadata.timestamps.ui32EndFrameIn);

        captureRequest.mFrameSettings->processLensShadingMap();
        captureRequest.mFrameSettings->processHotPixelMap();
    }
    return res;
}

status_t ProcessingThread::processTimestamp(
        const uint32_t hwTimestamp,
        nsecs_t& durationNs, nsecs_t& frameTimestampNs) {
    // Notify the framework of the next capture exposure time

    frameTimestampNs = mCameraHw.processHwTimestamp(hwTimestamp);
    {
        Mutex::Autolock l(mLock);

        if(mPrevFrameTimestamp) {
            durationNs = frameTimestampNs - mPrevFrameTimestamp;
            if(durationNs < 0) {
                durationNs = -durationNs;
            }

            LOG_DEBUG("Estimated FPS=%f frame duration=%lldms timestamp=%lld",
                (double)s2ns(1)/durationNs, ns2ms(durationNs),
                frameTimestampNs);
        }
        mPrevFrameTimestamp = frameTimestampNs; // obtain first frame timestamp
    }
    return OK;
}

bool ProcessingThread::threadLoop() {
    status_t res = NO_ERROR;
    bool sendMetadata = true;
    sp<CaptureRequest> captureRequest;
    {
        Mutex::Autolock l(mLock);
        // allow to wait on next request even if we flush(), because
        // the framework might have enqueued the next request in the meantime
        if (mCaptureRequestsQueue.empty()) {
            res = mCaptureRequestQueuedSignal.waitRelative(mLock,
                                    kWaitPerLoop);
            // sometimes res==NO_ERROR but the queue is still empty...
            if (res == TIMED_OUT ||
                    mCaptureRequestsQueue.empty()) {

                if(mFlushing) {
                    flushQueue();
                    mPrevFrameTimestamp = 0;
                    mCameraHw.initHwNanoTimer();
                    LOG_DEBUG("Now idle...");
                    mThreadActive = false;
                    mParent.mCameraIdleSignal.signal();
                }
                return true;
            } else if (res != NO_ERROR) {
                LOG_ERROR("Error waiting for capture "
                    "requests: %d", res);
                mParent.notifyFatalError();
                return false;
            }
            // now we should have at least one request queued
        }
        // don't process any more if flushing
        if(mFlushing) {
            flushQueue();
            return true;
        }
        LOG_DEBUG("%d capture requests left to handle.",
                mCaptureRequestsQueue.size());
        captureRequest = *mCaptureRequestsQueue.begin();
        mCaptureRequestsQueue.erase(mCaptureRequestsQueue.begin());
        if(!captureRequest.get()) {
            LOG_ERROR("Request pointer is NULL!");
            mParent.notifyFatalError();
            return false;
        }
    }
    ALOG_ASSERT(captureRequest->mFrameSettings.get());

    LOG_DEBUG("Processing frame %d", captureRequest->mFrameNumber);
    nsecs_t frameTimestampNs = 0;
    if(!captureRequest->mRequestError &&
       !captureRequest->mReprocessing) {
        // acquire programmed shots
        res = processRegularRequest(*captureRequest, frameTimestampNs);
    }
    if(frameTimestampNs == 0) {
        // use the timestamp provided in input buffer
        // else fallback to current system time
        camera_metadata_entry_t e =
            captureRequest->mFrameSettings->find(ANDROID_SENSOR_TIMESTAMP);
        if(e.count && e.data.i64) {
            frameTimestampNs = (nsecs_t)*e.data.i64;
            LOG_DEBUG("Timestamp provided=%lld",
                frameTimestampNs);
        } else {
            LOG_WARNING("Using system timestamp");
            frameTimestampNs = elapsedRealtimeNano();
        }
    }
    captureRequest->mFrameSettings->update(ANDROID_SENSOR_TIMESTAMP,
        (int64_t *)&frameTimestampNs, 1);

    if (mParent.process3A(*captureRequest->mFrameSettings) != OK) {
        LOG_ERROR("3A processing failed!");
        captureRequest->mMetadataError = true;
    }

    if(!captureRequest->mRequestError) {
        mParent.notifyShutter(captureRequest->mFrameNumber, frameTimestampNs);

        if (captureRequest->mJpegRequested) {
            LOG_DEBUG("processing JPEG...");

            Mutex::Autolock jl(mReprocessLock);
            if (mReprocessWaiting) {
                // This shouldn't happen, because processCaptureRequest should
                // be stalling until JPEG compressor is free.
                ALOG_ASSERT(false, "%s: Already processing a JPEG!",
                        __FUNCTION__);
            } else {
                res = processJpegEncodingRequest(*captureRequest);
                if(res != OK) {
                    mParent.notifyRequestError(captureRequest->mFrameNumber);
                } else {
                    sendMetadata = false;
                }
            }
            // for jpeg requests
            // output stream result + metadata is sent in onJpegDone()
        }
    } else {
        mParent.notifyRequestError(captureRequest->mFrameNumber);
    }

#ifdef USE_GPU_LIB
#warning Building with GPU support
    LOG_DEBUG("processing IS or FD");
    if (processISandFD(*captureRequest) != OK) {
        LOG_ERROR("3A processing failed!");
        captureRequest->mMetadataError = true;
    }
#endif //USE_GPU_LIB

    // send all processed streams (except blob and other processed in parallel)
    res = captureRequest->sendCaptureResult(sendMetadata);
    if(res != OK) {
        // device error
        mParent.notifyFatalError();
        return false; // break the thread loop
    }
    signalCaptureResultSent();
    tryIdle();
    return true;
}

bool ProcessingThread::isQueueEmpty(void) {
    Mutex::Autolock l(mLock);
    return mCaptureRequestsQueue.empty();
}

// to be called with mLock held
bool ProcessingThread::flushQueue(void) {
    LOG_DEBUG("E");
    if(mCaptureRequestsQueue.empty()) {
        LOG_DEBUG("Pending queue already empty");
        return true;
    }

    while (!mCaptureRequestsQueue.empty()) {
        CaptureRequest& captureRequest = *mCaptureRequestsQueue.begin()->get();
        LOG_DEBUG("Cancelling request %d", captureRequest.mFrameNumber);
        mParent.notifyRequestError(captureRequest.mFrameNumber);
        captureRequest.mRequestError = true;
        captureRequest.sendCaptureResult(true);
        if(captureRequest.mReprocessing) {
            // handle scheduled preprocessing requests
            captureRequest.sendReprocessResult(false);
        }
        mCaptureRequestsQueue.erase(mCaptureRequestsQueue.begin());
    }
    {
        Mutex::Autolock jpegLock(mReprocessLock);
        if(mReprocessWaiting == true) {
            LOG_DEBUG("Canceling ongoing reprocessing process in request %d",
                    mReprocessingRequest->mFrameNumber);
            JpegEncoder::get().cancel();
            if(mReprocessingRequest.get()) {
                mReprocessingRequest->sendReprocessResult(false);
            }
        }
        mReprocessingRequest.clear();
    }
    LOG_DEBUG("Done");

    return true;
}

// ---------------------------------------------------------------------------/
void ProcessingThread::onJpegDone(
        __maybe_unused const StreamBuffer &jpegBuffer,
        const int32_t jpegSize,
        bool success) {
    Mutex::Autolock jpegLock(mReprocessLock);
    mReprocessingRequest->mFrameSettings->update(ANDROID_JPEG_SIZE, &jpegSize, 1);
    mReprocessingRequest->sendReprocessResult(success);
    mReprocessingRequest.clear();
    LOG_DEBUG("JPEG result sent, jpeg size=%d", jpegSize);
    mReprocessWaiting = false;
}

}; // namespace android
