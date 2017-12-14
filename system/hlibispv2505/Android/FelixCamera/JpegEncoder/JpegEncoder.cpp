/**
******************************************************************************
@file JpegEncoder.cpp

@brief Definition of base JpegEncoder v2500 HAL module

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

//#define LOG_NDEBUG 0
#define LOG_TAG "JpegEncoder"

#include "ui/GraphicBufferMapper.h"

#include "felixcommon/userlog.h"
#include "FelixMetadata.h"
#include "JpegEncoder.h"
#include "JpegEncoderSw.h"
#include "JpegExif.h"
#include "Scaler.h"
#include "Helpers.h"

namespace android {

#define JPEG_DEBUG_SAVE_PROPERTY "img.felix.jpeg.savepath"

// instantiation of statics

/*
 * JPEG encoder capabilities
 */
const nsecs_t JpegEncoder::kTimeoutNs = 2000000000LL; // 2s max, ns
const int32_t JpegEncoder::kJpegMaxSize = 1000000;    // bytes
const int32_t JpegEncoder::kJpegThumbnailMaxSize = 64*1024; // 64kB max
const int32_t JpegEncoder::kJpegNumSupportedThumbnailSizes[] =
{
    0, 0,
    240, 160, // 3:2
    320, 180, // 16:9
    320, 240, // 4:3
    384, 216  // 16:9
};
sp<JpegEncoder> JpegEncoder::mInstance;

JpegEncoder& JpegEncoder::get() {
#ifdef USE_HARDWARE_JPEG_COMPRESSOR
#error Hardware JPEG not configured!
    mInstance = &JpegEncoderHw::getInstance();
#else
    mInstance = &JpegEncoderSw::getInstance();
#endif
    return *mInstance;
}

void JpegEncoder::getThumbnailCapabilities(
        int32_t** sizes, int32_t& numElements) {
    if(!sizes) {
        return;
    }
    *sizes = const_cast<int32_t*>(&kJpegNumSupportedThumbnailSizes[0]);
    numElements = ARRAY_NUM_ELEMENTS(kJpegNumSupportedThumbnailSizes);
}

JpegEncoder::JpegEncoder():
        Thread(false),
        mIsBusy(false),
        mOutputBytes(0),
        mListener(NULL),
        mRequestMetadata(NULL),
        mJpegDebugSaveImages(false) {

    if(property_get(JPEG_DEBUG_SAVE_PROPERTY, mJpegSavePath, "")>0) {
        mJpegDebugSaveImages = true;
    }
}

JpegEncoder::~JpegEncoder() {
}

status_t JpegEncoder::setInputBuffer(
        StreamBuffer& inputBuffer){

    Mutex::Autolock lock(mMutex);
    if(isBusy()){
        LOG_ERROR("Already processing a buffer!");
        return INVALID_OPERATION;
    }
    mInputBuffer = &inputBuffer;
    return OK;
}

status_t JpegEncoder::setOutputBuffer(
        StreamBuffer& outputBuffer){

    Mutex::Autolock lock(mMutex);
    if(isBusy()){
        LOG_ERROR("Already processing a buffer!");
        return INVALID_OPERATION;
    }
    mOutputBuffer = &outputBuffer;
    return OK;
}

status_t JpegEncoder::setJpegListener(
        JpegListener *listener){

    Mutex::Autolock lock(mMutex);
    if(isBusy()){
        LOG_ERROR("Already processing a buffer!");
        return INVALID_OPERATION;
    }
    mListener = listener;
    return OK;
}

status_t JpegEncoder::setRequestMetadata(
        FelixMetadata& metadata){

    Mutex::Autolock lock(mMutex);
    if(isBusy()){
        LOG_ERROR("Already processing a buffer!");
        return INVALID_OPERATION;
    }
    mRequestMetadata = &metadata;
    return OK;
}

status_t JpegEncoder::start() {
    {
        Mutex::Autolock lock(mMutex);
        if (mListener == NULL) {
            ALOGE("%s: NULL listener not allowed!", __FUNCTION__);
            return BAD_VALUE;
        }
    }
    {
        Mutex::Autolock busyLock(mBusyMutex);
        if (mIsBusy) {
            ALOGE("%s: Already processing a buffer!", __FUNCTION__);
            return INVALID_OPERATION;
        }
        mIsBusy = true;
    }

    status_t res = run("JpegCompressor");
    if (res != OK) {
        ALOGE("%s: Unable to start up compression thread: %s (%d)",
                __FUNCTION__, strerror(-res), res);
    }
    return res;
}

status_t JpegEncoder::cancel() {
    requestExitAndWait();
    return OK;
}

status_t JpegEncoder::readyToRun() {
    return OK;
}

bool JpegEncoder::threadLoop() {
    status_t res;
    ALOGV("%s: Starting compression thread", __FUNCTION__);

    Mutex::Autolock lock(mMutex);

    uint8_t quality;
    uint8_t* thumbnail = NULL;
    int thumbnailCompressedSize = 0;

    mOutputBytes = 0;

    ALOG_ASSERT(mRequestMetadata.get());

    uint32_t width, height;
    mRequestMetadata->getJpegThumbnailSize(width, height);
    // if width and height are not set then it means that the thumbnail
    // was not requested
    if(width && height){
        LOG_DEBUG("Alloc thumbnail input buffer %dx%d", width, height);
        camera3_stream_t thumb = camera3_stream_t();
        thumb.format = mInputBuffer->stream->format;
        thumb.width = width;
        thumb.height = height;
        thumb.usage = GRALLOC_USAGE_SW_READ_RARELY |
                GRALLOC_USAGE_SW_WRITE_RARELY |
                GRALLOC_USAGE_HW_CAMERA_READ |
                GRALLOC_USAGE_HW_CAMERA_WRITE;
        mThumbnailInputBuffer = thumb;
        mThumbnailInputBuffer.alloc();

        ImgScaler::get().scale(*mInputBuffer, mThumbnailInputBuffer);

        LOG_DEBUG("Alloc output buffer for compressed thumbnail (%dB)",
                        kJpegThumbnailMaxSize);
        thumb = camera3_stream_t();
        thumb.format = HAL_PIXEL_FORMAT_BLOB;
        // for allocation, define size as buffersize x 1 byte
        thumb.width = kJpegThumbnailMaxSize;
        thumb.height = 1;
        thumb.usage = GRALLOC_USAGE_SW_READ_RARELY |
                GRALLOC_USAGE_SW_WRITE_RARELY |
                GRALLOC_USAGE_HW_CAMERA_READ |
                GRALLOC_USAGE_HW_CAMERA_WRITE;
        mThumbnailOutputBuffer = thumb;
        mThumbnailOutputBuffer.alloc();

        if(!mRequestMetadata->getJpegThumbnailQuality(quality)) {
            quality = 6; // failsafe value
        }
        // configure compressor for thumbnail image
        // (uses mThumbnailOutputBuffer as the destination)
        initCompressorThumbnail(quality);
        res = compress(mThumbnailInputBuffer);
        mThumbnailInputBuffer.free();

        // store result for Exif processing
        thumbnail = mThumbnailOutputBuffer.mmap.img;
        thumbnailCompressedSize = mThumbnailOutputSize;
    }
    // initialize compressor internal config to main image
    // (uses mOutputBuffer as destination buffer)
    if(!mRequestMetadata->getJpegQuality(quality)) {
        quality = 90; // failsafe value
    }
    bool isRgb =
        mInputBuffer->stream->format == (int)HAL_PIXEL_FORMAT_RGB_888;
    // configure compressor for main image 9uses
    initCompressorMain(quality);
    {
        ExifWriter exif(*this, mRequestMetadata);
        // write Exif structure
        exif.writeApp1(mOutputBuffer->stream->width,
                mOutputBuffer->stream->height,
                thumbnail, thumbnailCompressedSize, isRgb);
        mThumbnailOutputBuffer.free();
    }
    res = compress(*mInputBuffer);

    if(mJpegDebugSaveImages) {
        std::stringstream filename;
        filename << mJpegSavePath << "/felix_" << (isRgb ? "rgb" : "yuv");
        filename << "_" << (uint32_t)mRequestMetadata->getTimestamp() << ".jpg";

        LOG_DEBUG("Saving debug %s", filename.str().c_str());
        FILE* f = fopen(filename.str().c_str(), "wb");
        if(f)
        {
            mOutputBuffer->lock(GRALLOC_USAGE_SW_READ_RARELY);
            fwrite(mOutputBuffer->mmap.img,
                    mOutputBytes, 1, f);
            fclose(f);
            mOutputBuffer->unlock();
        } else {
            LOG_WARNING("Couldn't open file %s for writing", filename.str().c_str());
        }
        filename.flush();
    }

    // call listener callback (signal processingThread)
    if(mListener) {
        mListener->onJpegDone(*mOutputBuffer, mOutputBytes, res == OK);
    }

    cleanUp();

    return false;
}

bool JpegEncoder::isBusy() {
    Mutex::Autolock busyLock(mBusyMutex);
    return mIsBusy;
}

bool JpegEncoder::waitForDone(nsecs_t timeout) {
    Mutex::Autolock lock(mBusyMutex);
    status_t res = OK;
    if (mIsBusy) {
        res = mDone.waitRelative(mBusyMutex, timeout);
    }
    return (res == OK);
}

void JpegEncoder::cleanUp() {
    Mutex::Autolock lock(mBusyMutex);
    // unlock and release the external buffers
    mInputBuffer.clear();
    mOutputBuffer.clear();
    mRequestMetadata.clear();
    mIsBusy = false;
    mDone.signal();
}

JpegEncoder::JpegListener::~JpegListener() {
}

} // namespace android
