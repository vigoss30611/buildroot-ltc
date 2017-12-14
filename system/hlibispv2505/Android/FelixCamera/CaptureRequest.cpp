/**
******************************************************************************
@file CaptureRequest.cpp

@brief Definition of CaptureRequest class in v2500 HAL module

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

#define LOG_TAG "CaptureRequest"

#include "CaptureRequest.h"
#include "FelixCamera.h"
#include "HwManager.h"
//#include "FelixProcessing.h"
//#include <utils/SystemClock.h>

#include <ui/Rect.h>
#include <ui/GraphicBufferMapper.h>

#include <felixcommon/userlog.h>

// timeout for CaptureRequest::acquireShot() call
#define ACQUIRE_SHOT_TOTAL_TIMEOUT_MS    1000UL // 1 second of timeout

// ---------------------------------------------------------------------------/
// Processing thread
// ---------------------------------------------------------------------------/
namespace android {

status_t CaptureRequest::sendCaptureResult(bool sendMetadata) {
    Mutex::Autolock l(mLock);

    // Gather capture results (metadata + output buffers) and send back to
    // framework.
    // : in cases of nonZSL captures involving
    // JPEG/RAW and other types of streams, we can collect metadata in parts
    // and provide all immediately available fields with current result
    // and jpeg related ones in sendReprocessResult()

    // send only non BLOB/RAW type streams

    LOG_DEBUG("frame = %d", mFrameNumber);

    // local copy buffs, will contain buffers returned in current call
    BufferList_t outBuffers(mBuffers);
    mBuffers.clear();
    // upon returning from this call, mBuffers will contain not returned streams
    // that is 'slow' streams only - BLOB/ or possibly RAW/HDR

    for(size_t i=0;i<outBuffers.size();) {
        camera3_stream_buffer_t& buffer = outBuffers.editItemAt(i);
        ALOG_ASSERT(buffer.stream);
        ALOG_ASSERT(buffer.buffer);

        // : extend with other slow streams
        if(!mRequestError &&
            buffer.stream->format == HAL_PIXEL_FORMAT_BLOB) {
            // store this buffer for sendReprocessResult
            mBuffers.push(buffer);
            i = outBuffers.removeAt(i);
            // do this only for non erroneous results
            continue;
        }
        if(mRequestError) {
            buffer.status = CAMERA3_BUFFER_STATUS_ERROR;
        } else {
            // the buffer status may have changed earlier, but do not
            // notify buffer error if request error has been sent earlier
            if(buffer.status == CAMERA3_BUFFER_STATUS_ERROR) {
                mParent.notifyBufferError(mFrameNumber, buffer.stream);
            }
        }
        buffer.acquire_fence = -1;
        buffer.release_fence = -1;
        LOG_DEBUG("Buffer %dx%d, format=0x%x usage=0x%x fd=0x%x handle=%p",
            buffer.stream->width, buffer.stream->height,
            buffer.stream->format, buffer.stream->usage,
            getIonFd(*buffer.buffer),
            *buffer.buffer);
        ++i;
    }
    if(!outBuffers.size() && !sendMetadata) {
        // nothing to send now
        return OK;
    }

#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    mFrameSettings->update(ANDROID_REQUEST_PIPELINE_DEPTH,
        &mPipelineDepth, 1);
#endif

    camera3_capture_result result = camera3_capture_result();
    result.frame_number = mFrameNumber;
    result.num_output_buffers = outBuffers.size();
    result.output_buffers = outBuffers.array();
    result.result = (mMetadataError || !sendMetadata) ?
            NULL :
            mFrameSettings->getAndLock();
#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    // so far we don't support partial results
    result.partial_result = 1;
#endif
    mParent.sendCaptureResult(&result);

    if(result.result) {
        mFrameSettings->unlock(result.result);
    }
    return OK;
}

status_t CaptureRequest::sendReprocessResult(const bool success) {
    Mutex::Autolock l(mLock);

#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    mFrameSettings->update(ANDROID_REQUEST_PIPELINE_DEPTH,
        &mPipelineDepth, 1);
#endif

    // send partial result with output buffer and metadata
    // input buffer is optional (added since HAL v3.2)
    camera3_stream_buffer_t output;
    camera3_capture_result result = camera3_capture_result();
    result.frame_number = mFrameNumber;
    result.result = success ? mFrameSettings->getAndLock() : NULL;
    result.num_output_buffers = 1;
    result.output_buffers = &output;

    output.status = success ? CAMERA3_BUFFER_STATUS_OK :
        CAMERA3_BUFFER_STATUS_ERROR;
    output.acquire_fence = -1;
    output.release_fence = -1;
    output.stream = mReprocessOutputBuffer.stream;
    output.buffer = mReprocessOutputBuffer.buffer;
#if CAMERA_DEVICE_API_VERSION_CURRENT >= HARDWARE_DEVICE_API_VERSION(3, 2)
    if(mReprocessing) {
        // even if we don't actually reprocess the input buffer but take
        // non ZSL JPEG shot, we call sendReprocessResult()
        result.input_buffer = &mInputBuffer;
    }
    result.partial_result = 1;
#endif
    mParent.sendCaptureResult(&result);
    mFrameSettings->unlock(result.result);
    return OK;
}
#if(0)
extern "C" IMG_RESULT CI_GasketGetInfo(CI_GASKET_INFO *pGasketInfo, IMG_UINT8 uiGasket,
        CI_CONNECTION *pConnection);

static void printGasketInfo(ISPC::Camera &camera)
{
    CI_GASKET_INFO sGasketInfo;
    CI_CONNECTION *pConn = camera.getConnection();
    IMG_RESULT ret;
    IMG_UINT8 uiGasket = camera.getSensor()->uiImager;

    ret = CI_GasketGetInfo(&sGasketInfo, uiGasket, pConn);

    if (IMG_SUCCESS == ret)
    {
        LOG_DEBUG("*****\n");
        LOG_DEBUG("Gasket %u\n", (unsigned)uiGasket);
        if (sGasketInfo.eType&CI_GASKET_PARALLEL)
        {
            LOG_DEBUG("enabled (Parallel) - frame count %u\n",
                sGasketInfo.ui32FrameCount);
        }
        else if (sGasketInfo.eType&CI_GASKET_MIPI)
        {
            LOG_DEBUG("enabled (MIPI) - frame count %u\n",
                sGasketInfo.ui32FrameCount);
            LOG_DEBUG("Gasket MIPI FIFO %u - Enabled lanes %u\n",
                (int)sGasketInfo.ui8MipiFifoFull,
                (int)sGasketInfo.ui8MipiEnabledLanes);
            LOG_DEBUG("Gasket CRC Error 0x%x\n"\
                "       HDR Error 0x%x\n"\
                "       ECC Error 0x%x\n"\
                "       ECC Correted 0x%x\n",
                (int)sGasketInfo.ui8MipiCrcError,
                (int)sGasketInfo.ui8MipiHdrError,
                (int)sGasketInfo.ui8MipiEccError,
                (int)sGasketInfo.ui8MipiEccCorrected);
        }
        else
        {
            LOG_DEBUG("disabled\n");
        }
        LOG_DEBUG("*****\n");
    }
    else
    {
        LOG_ERROR("failed to get information about gasket %u\n",
            (int)uiGasket);
    }
}
#endif

status_t CaptureRequest::acquireShot(ISPC::Shot& acquiredShot) {

    status_t res = OK;
    ISPC::Shot ispc_shot;
    // ------------------------------------------------------------------------
    // Get all shots for current capture request
    // ------------------------------------------------------------------------
    Mutex::Autolock l(mLock);

    newPipelineStep();

    for(size_t index = 0; index<mShotList.size();++index) {
        const Shot_t& shot = mShotList.valueAt(index);
        // check if the pipeline object still exists
        cameraHw_t* pipeline = const_cast<cameraHw_t*>(
                mShotList.keyAt(index).promote().get());
        if(!pipeline) {
            LOG_ERROR("Pipeline %p has been already destroyed!",
                    mShotList.keyAt(index).unsafe_get());
            return NO_INIT;
        }
        LOG_DEBUG("Acquiring shot (encId=%#x dispId=%#x) from pipeline %p, "\
                "frame=%d",
                shot.encId, shot.dispId, pipeline, mFrameNumber);

        IMG_RESULT imgres = IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;

        const uint32_t tryAcquireShotPeriodUs = 5000;
        uint32_t loops =
            ACQUIRE_SHOT_TOTAL_TIMEOUT_MS * 1000 / tryAcquireShotPeriodUs;

        while(imgres != IMG_SUCCESS && --loops) {
            if(mParent.isFlushing()) {
                // take fast request flush path
                mRequestError = true;
                return FAILED_TRANSACTION;
            }
            {
                Mutex::Autolock lhw(mCameraHw.getHwLock());
                imgres = pipeline->tryAcquireShot(ispc_shot);
            }
            if(imgres == IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE) {
                usleep(tryAcquireShotPeriodUs);
                continue;
            }
            if(imgres != IMG_SUCCESS) {
                LOG_ERROR("Getting shot failed! frameError=%d",
                        acquiredShot.bFrameError);
                res = UNKNOWN_ERROR;
                mRequestError = true;
                break;
            }
        }
        if(mRequestError) {
            break;
        }
        if(pipeline == mCameraHw.getMainPipeline()) {
            // get the statistics from the main pipeline only
            acquiredShot = ispc_shot;
        }
        LOG_DEBUG("Acquired buffer (encId=%#x dispId=%#x) error=%d",
                ispc_shot.YUV.id, ispc_shot.RGB.id,
                ispc_shot.bFrameError);

        // Handle encoder buffer...
        if (shot.encId > 0) {
            // Validate file descriptors
            if (shot.encId != ispc_shot.YUV.id) {
                res = UNKNOWN_ERROR;
                LOG_ERROR("ProcessingThread: Buffer descriptors do not match "
                        "up (expected encId=%#x got %#x)!",
                        shot.encId, ispc_shot.YUV.id);
                mRequestError = true;
            }
        }

        // Handle display buffer...
        if (shot.dispId > 0) {
            // Validate file descriptors
            if (shot.dispId != ispc_shot.RGB.id) {
                res = UNKNOWN_ERROR;
                LOG_ERROR("ProcessingThread: Buffer descriptors do not match "
                        "up (expected dispId=%#x got %#x)!",
                        shot.dispId, ispc_shot.RGB.id);
                mRequestError = true;
            }
        }

        Mutex::Autolock lhw(mCameraHw.getHwLock());
        if (pipeline->releaseShot(ispc_shot) != IMG_SUCCESS) {
            LOG_WARNING("Releasing captured shot failed!");
            // only warning now, don't know what to report here
        }
    } // end of buffers handling.
    return res;
}

CaptureRequest::CaptureRequest(
        FelixCamera &parent,
        sp<FelixMetadata>& meta) :
                       mFrameNumber(0),
                       mFrameSettings(meta),
                       mReprocessing(false),
                       mJpegRequested(false),
                       mRequestError(false),
                       mMetadataError(false),
                       mPipelineDepth(0),
                       mParent(parent),
                       mCameraHw(mParent.getHwInstance()){
    LOG_DEBUG("E");
};

CaptureRequest::~CaptureRequest() {
    LOG_DEBUG("frame=%d", mFrameNumber);
    cleanUp();
}

void CaptureRequest::cleanUp(void) {
    LOG_DEBUG("E");
    Mutex::Autolock l(mLock);
    // Unlock all request buffers.
    while (mBuffers.begin() != mBuffers.end()) {
        GraphicBufferMapper::get().unlock(*(*mBuffers.begin()).buffer);
        mBuffers.erase(mBuffers.begin());
    }
}

status_t CaptureRequest::insertStreamBuffer(
        const camera3_stream_buffer_t &streamBuffer) {

    PrivateStreamInfo* priv = streamToPriv(streamBuffer.stream);
    if(!priv) {
        return NO_INIT;
    }

    hwBufferId_t bufferId = priv->getBufferId(
            getIonFd(*streamBuffer.buffer));
    // all buffers in the request shall be imported prior to request
    ALOG_ASSERT(bufferId != NO_HW_BUFFER);
    ALOG_ASSERT(priv->getPipeline());
    int index = mShotList.indexOfKey(priv->getPipeline());
    if(index<0) {
        // new pipeline to program in requested shot, add empty descriptor
        LOG_DEBUG("Adding pipeline %p to shot", priv->getPipeline());
        index = mShotList.add(priv->getPipeline(), Shot_t());
    }
    Shot_t& shot = mShotList.editValueAt(index);

    if (priv->ciBuffType == CI_TYPE_ENCODER) {
        if(shot.encId != NO_HW_BUFFER) {
            LOG_ERROR("Encoder output already requested in specific pipeline");
            return BAD_VALUE;
        }
        shot.encId = bufferId;
        LOG_DEBUG("Adding enc buffer 0x%x to pipeline %p",
                bufferId, priv->getPipeline());
    } else if (priv->ciBuffType == CI_TYPE_DISPLAY) {
        // Display buffers (preview) => RGB
        if(shot.dispId != NO_HW_BUFFER) {
            LOG_ERROR("Encoder output already requested in specific pipeline");
            return BAD_VALUE;
        }
        shot.dispId = bufferId;
        LOG_DEBUG("Adding disp buffer 0x%x to pipeline %p",
                bufferId, priv->getPipeline());
    } else if (priv->ciBuffType == CI_TYPE_DATAEXT) {
        // RAW buffers
        if(shot.idRaw2D != NO_HW_BUFFER) {
            LOG_ERROR("Encoder output already requested in specific pipeline");
            return BAD_VALUE;
        }
        shot.idRaw2D = bufferId;
    } else {
        LOG_ERROR("Unexpected buffer requested, type=%d", priv->ciBuffType);
        return BAD_VALUE;
    }
    return NO_ERROR;
}

status_t CaptureRequest::preprocessRequest(
        camera3_capture_request &captureRequest) {
    Mutex::Autolock l(mLock);

    mFrameNumber = captureRequest.frame_number;
    // if set then it is reprocessing request
    // specifically, if BLOB output buffer was provided
    // then it is jpeg encoding request

    if(captureRequest.input_buffer) {
        mInputBuffer = *captureRequest.input_buffer;
        mReprocessing = true;
        if(acquireFence(captureRequest.input_buffer->acquire_fence)) {
            // error acquiring input buffer -> request error
            mParent.notifyRequestError(captureRequest.frame_number);
            mRequestError = true;
        }
        captureRequest.input_buffer->release_fence = -1;
    }
    mBuffers.clear(); // clear, just to be sure
    mBuffers.appendArray(captureRequest.output_buffers,
            captureRequest.num_output_buffers);

    for (size_t i = 0; i < captureRequest.num_output_buffers; i++) {

        camera3_stream_buffer_t &srcBuf = mBuffers.editItemAt(i);
        ALOG_ASSERT(srcBuf.buffer);

        srcBuf.status = acquireFence(srcBuf.acquire_fence) != true ?
                CAMERA3_BUFFER_STATUS_OK :
                CAMERA3_BUFFER_STATUS_ERROR;

        // Output buffer for JPEG encoder found.
        if (srcBuf.stream->format == HAL_PIXEL_FORMAT_BLOB) {
            LOG_DEBUG("JPEG: requested.");

            mJpegRequested = true;
            mReprocessOutputBuffer = srcBuf;
            // JPEG request may contain >1 output buffers
            if(!mReprocessing) {
                LOG_INFO("non ZSL JPEG requested");
            }
            continue;
        }

        // connect request buffers to specific HW pipeline outputs
        // only if request not erroneous
        if(mRequestError == false &&
           insertStreamBuffer(srcBuf)!=NO_ERROR) {
            LOG_ERROR("Couldn't insert buffer into Shot");
            srcBuf.status = CAMERA3_BUFFER_STATUS_ERROR;
        }
    }
    if(mRequestError == false && mJpegRequested && !mReprocessing) {

        // capture the image directly to nonZsl buffer
        ALOG_ASSERT(mParent.mNonZslOutputBuffer->buffer,
                "%s: nonZsl buffer not prepared!", __FUNCTION__);

        if(insertStreamBuffer(*mParent.mNonZslOutputBuffer)!=NO_ERROR) {
            LOG_ERROR("Couldn't schedule a capture to local buffer!");
            return NO_INIT;
        }

        // nonZsl jpeg captures use local buffer as input for jpeg encoder
        mInputBuffer = *mParent.mNonZslOutputBuffer;
    }
    return OK;
}

status_t CaptureRequest::preprocessMetadata() {

    mFrameSettings->preprocess3aRegions();
    mFrameSettings->preprocessEffectMode();

    FelixAF *afModule =
            mCameraHw.getMainPipeline()->getControlModule<FelixAF>();
    if(afModule) {
        afModule->processUrgentHALMetadata(*mFrameSettings);
    }
    FelixAWB *awbModule =
            mCameraHw.getMainPipeline()->getControlModule<FelixAWB>();
    if(awbModule) {
        awbModule->processUrgentHALMetadata(*mFrameSettings);
    }
    FelixAE *aeModule =
            mCameraHw.getMainPipeline()->getControlModule<FelixAE>();
    if(aeModule) {
        aeModule->processUrgentHALMetadata(*mFrameSettings);
    }
    return OK;
}

status_t CaptureRequest::program() {
    Mutex::Autolock l(mLock);
    // lock the hardware
    // if needed, update the pipeline before programming the shots
    if (mParent.program() != NO_ERROR) {
        cleanUp();
        return NO_INIT;
    }

    if(mParent.startCapture()!=NO_ERROR) {
        cleanUp();
        return NO_INIT;
    }

    newPipelineStep();

    for(size_t index = 0; index<mShotList.size();++index) {
        const Shot_t& shot = mShotList.valueAt(index);
        cameraHw_t* pipeline = const_cast<cameraHw_t*>(
                mShotList.keyAt(index).unsafe_get());
        ALOG_ASSERT(pipeline);

        LOG_DEBUG("Programming specified shot (encId=%#x"
                " dispId=%#x in pipeline %p)...",
                shot.encId, shot.dispId, pipeline);

        Mutex::Autolock lhw(mCameraHw.getHwLock());
        if(pipeline->state == ISPC::Camera::CAM_READY &&
            pipeline->startCapture()!=IMG_SUCCESS) {
            return INVALID_OPERATION;
        }
        if (pipeline->enqueueSpecifiedShot(shot) != IMG_SUCCESS) {
            LOG_ERROR("Failed to program shot!");
            mParent.notifyRequestError(mFrameNumber, NULL);
            mRequestError = true;
        } else {
            LOG_DEBUG("Shot programmed.");
        }
    }
    return OK;
}

}; // namespace android
