/**
******************************************************************************
@file Helpers.cpp

@brief Definitions of various helper classes and functions used throughout
v2500 Camera HAL

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

#define LOG_TAG "FelixHelpers"

#include <felixcommon/userlog.h>
#include <utils/Timers.h>
#include "ui/Fence.h"
#include "ui/GraphicBufferMapper.h"
#include "FelixCamera.h"
#include "Helpers.h"

namespace android {

const hwBufferId_t NO_HW_BUFFER=0;

static const uint32_t kFenceTimeoutMs = 2000; // 2 s

ionFd_t getIonFd(const buffer_handle_t handle){
    return static_cast<const private_handle_t *>(handle)->getIonFd();
}

size_t getBufferSize(const buffer_handle_t handle){
    return static_cast<const private_handle_t *>(handle)->getSize();
}

size_t getTiled(const buffer_handle_t handle){
    return static_cast<const private_handle_t *>(handle)->getTiled();
}

size_t getBufferStride(const buffer_handle_t handle){
    return static_cast<const private_handle_t *>(handle)->getStride();
}

PrivateStreamInfo* streamToPriv(const camera3_stream_t* stream)
{
    if(!stream) return NULL;
    return static_cast<PrivateStreamInfo*>(stream->priv);
}

bool acquireFence(const int fence) {
    if (fence != -1) {
        LOG_DEBUG("Waiting on %d...", fence);
        sp<Fence> bufferAcquireFence = new Fence(fence);
        if (!bufferAcquireFence.get()){
            LOG_ERROR("Allocation failed!");
            return true;
        } else if (bufferAcquireFence->wait(kFenceTimeoutMs) == TIMED_OUT) {
            LOG_ERROR("Fence timed out after %d ms", kFenceTimeoutMs);
            return true;
        }
    }
    return false;
}

PrivateStreamInfo::~PrivateStreamInfo() {
    LOG_DEBUG("E");
    deregisterBuffers();
    clear();
}

status_t PrivateStreamInfo::deregisterBuffers()
{
    if(!camera.get())
    {
        return INVALID_OPERATION;
    }

    for (size_t i = 0; i < buffersMap.size(); ) {
        const hwBuffer_t& buf = buffersMap.valueAt(i);
        hwBufferId_t id = buf.id;
        LOG_DEBUG("deregistering buffer handle=%p fd=%d id=%d",
                buf.handle, buffersMap.keyAt(i), id);

        if(id != NO_HW_BUFFER &&
            camera->deregisterBuffer(id) != IMG_SUCCESS) {
            LOG_ERROR("Failed to deregister buffer id = %#x", id);
            return INVALID_OPERATION;
        }
        buffersMap.editValueAt(i).id = NO_HW_BUFFER;
        ++i;
    }
    return OK;
}

void PrivateStreamInfo::removeBuffer(const ionFd_t fd) {
    buffersMap.removeItem(fd);
}

status_t PrivateStreamInfo::deregisterBuffer(
        const hwBufferId_t deregId) {
    if(!camera.get())
    {
        return INVALID_OPERATION;
    }

    // FIXME - this loop is implemented just to be able to do replaceValueAt()
    // think about different approach and remove this loop
    for (size_t i = 0; i < buffersMap.size(); ++i) {
        const hwBuffer_t& buf = buffersMap.valueAt(i);
        if(buf.id != deregId) {
            continue;
        }
        __maybe_unused ionFd_t fd = buffersMap.keyAt(i);
        LOG_DEBUG("Deregistering buffer handle=%p ionFd=%d id=0x%d from pipeline %p",
                buf.handle, fd, buf.id, camera.get());
        if(camera->deregisterBuffer(buf.id) != IMG_SUCCESS) {
            LOG_ERROR("Failed to deregister buffer id = %#x", buf.id);
            return INVALID_OPERATION;
        }
        buffersMap.editValueAt(i).id = NO_HW_BUFFER;
        break;
    }
    return OK;
}

bool PrivateStreamInfo::isBufferRegistered(
            const buffer_handle_t& handle) {
    ionFd_t ionBufferFd = getIonFd(handle);
    int idx = buffersMap.indexOfKey(ionBufferFd);

    if(idx>=0) {
        ALOG_ASSERT(buffersMap.valueAt(idx).id>0, "Inconsistent buffer id for fd=%d handle=%p",
                ionBufferFd, buffersMap.valueAt(idx).handle);
        ALOG_ASSERT(buffersMap.valueAt(idx).handle!=NULL, "Inconsistent handle for fd=%d id=%d",
                ionBufferFd, buffersMap.valueAt(idx).id);
        return true;
    }
    return false;
}

status_t PrivateStreamInfo::registerBuffer(
        const buffer_handle_t& handle) {
    ALOG_ASSERT(handle);
    if(!camera.get())
    {
        LOG_ERROR("Camera object does not exist");
        return INVALID_OPERATION;
    }
    ionFd_t ionBufferFd = getIonFd(handle);
    int idx = buffersMap.indexOfKey(ionBufferFd);
    hwBuffer_t buf = { handle, NO_HW_BUFFER };

    // if new in stream, store for future reimports/deregistering
    if(idx < 0) {
        buffersMap.add(ionBufferFd, buf);
    }
    if (camera->importBuffer(ciBuffType, ionBufferFd,
                getBufferSize(handle), getTiled(handle), &buf.id) != IMG_SUCCESS) {
        LOG_ERROR("Importing buffer failed!");
        return INVALID_OPERATION;
    }
    LOG_DEBUG("Imported %s buffer (handle=%p ionBufferId=%d bufferId=%d) "
            "to pipeline %p",
            ciBuffType == CI_TYPE_DISPLAY ?
            "display" : "encoder",
            handle, ionBufferFd, buf.id, camera.get());
    // map ionFd's with imported buffer id's
    buffersMap.replaceValueFor(ionBufferFd, buf);
    return OK;
}

status_t PrivateStreamInfo::registerBuffers(
        const camera3_stream_buffer_set *bufferSet)
{
    if(!camera.get())
    {
        LOG_ERROR("Camera object does not exist");
        return INVALID_OPERATION;
    }

    buffer_handle_t handle = NULL;
    if(bufferSet) {
        // if (framework has provided the buffers) { init with new ionFD's }
        // else { just import previously registered ionFD's }
        for (unsigned bufferIdx = 0; bufferIdx < bufferSet->num_buffers;
                bufferIdx++) {
            handle = *bufferSet->buffers[bufferIdx];
            registerBuffer(handle);
        }
    } else {
        if(buffersMap.isEmpty()) {
            LOG_ERROR("Invalid state of buffersList");
            return INVALID_OPERATION;
        }
        // register all not imported buffers left
        for(unsigned i=0; i < buffersMap.size(); ++i) {
            hwBufferId_t bufferId = buffersMap.valueAt(i).id;
            if(bufferId != NO_HW_BUFFER) {
                // do not import already registered buffers
                continue;
            }
            if(registerBuffer(buffersMap.valueAt(i).handle)!=NO_ERROR) {
                return INVALID_OPERATION;
            }
        }
    }
    return OK;
}

StreamBuffer::StreamBuffer(camera3_stream_buffer_t& newStream) :
                mmap(),
        mStream(newStream.stream ? *newStream.stream : camera3_stream_t()),
        mBuffer(newStream.buffer ? *newStream.buffer : NULL),
        mHwManager(NULL), mPriv(NULL) {
    stream = &mStream;
    buffer = &mBuffer;
    if(mStream.format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        // for custom usage flags, gralloc needs exact format, not opaque
        mStream.format = HwManager::translateImplementationDefinedFormat(
                mStream.usage);
    }
}

StreamBuffer::~StreamBuffer() {
    LOG_DEBUG("E");
    unlock();
    ALOG_ASSERT(stream);
    if(mHwManager) {
        // detach stream only if it was not provided by framework
        mHwManager->detachStream(stream);
        mHwManager = NULL;
    }
    if(mPriv) {
        delete mPriv;
        mPriv = NULL;
    }
}

status_t StreamBuffer::attachToHw(HwManager& hwMng) {
    ALOG_ASSERT(stream);
    ALOG_ASSERT(mHwManager == NULL, "Stream is already attached to HW");
    mHwManager = &hwMng;
    if(!stream->priv) {
        mPriv = new PrivateStreamInfo(stream);
        mPriv->usage = stream->usage;
        stream->priv = mPriv;
    }
    PrivateStreamInfo* priv = streamToPriv(stream);
    cameraHw_t* cam = mHwManager->attachStream(stream, priv->ciBuffType);
    if(!cam) {
        return NO_INIT;
    }
    priv->setPipeline(cam);
    return NO_ERROR;
}

status_t StreamBuffer::registerBuffer() {
    ALOG_ASSERT(buffer, "%s: buffer handle not set!", __FUNCTION__);
    ALOG_ASSERT(stream, "%s: stream not set!", __FUNCTION__);

    PrivateStreamInfo* priv = streamToPriv(stream);
    ALOG_ASSERT(priv, "priv not allocated");
    LOG_DEBUG("registering local buffer ionFd=0x%x to pipeline=%p",
            getIonFd(mBuffer), priv->getPipeline());
    return priv->registerBuffer(*buffer);
}

status_t StreamBuffer::deregisterBuffer() {
    ALOG_ASSERT(buffer);
    ALOG_ASSERT(stream);
    if(!stream->priv || !mBuffer) {
        // no stream this buffer is attached to
        return NO_ERROR;
    }

    PrivateStreamInfo* priv = streamToPriv(stream);
    hwBufferId_t id = priv->getBufferId(getIonFd(mBuffer));
    if(id == NO_HW_BUFFER) {
        return NO_ERROR;
    }
    LOG_DEBUG("Deregistering local buffer id=0x%x from pipeline %p",
            id, priv->getPipeline());
    if (priv->deregisterBuffer(id) != IMG_SUCCESS) {
        LOG_ERROR("Failed to deregister buffer id = %#x", id);
        return INVALID_OPERATION;
    }
    priv->removeBuffer(getIonFd(mBuffer));
    return NO_ERROR;
}

status_t StreamBuffer::lock(const int usage, const Rect& cropRect) {
    ALOG_ASSERT(stream);
    ALOG_ASSERT(buffer);

    Rect crop(cropRect);

    int format = stream->format;
    // if format is implementation defined, translate to internal type
    if(format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        format = HwManager::translateImplementationDefinedFormat(
                stream->usage);
    }
    switch(format) {
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_YCbCr_420_888:
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
    case HAL_PIXEL_FORMAT_YCbCr_422_I:
        if (GraphicBufferMapper::get().lockYCbCr(*buffer, usage,
                        crop, &mmap.yuvImg) != OK) {
            LOG_ERROR("Unable to lock YUV buffer!");
            return UNKNOWN_ERROR;
        }
        break;
    case HAL_PIXEL_FORMAT_BLOB:
        // BLOB buffers are size bytes width, 1 byte height
        crop = Rect(getBufferSize(*buffer), 1);
        LOG_DEBUG("Locking BLOB buffer %dx%d",
                        crop.getWidth(), crop.getHeight());
        /* no break */
    default:
        if (GraphicBufferMapper::get().lock(*buffer, usage,
                        crop, (void**)&mmap.img) != OK) {
            LOG_ERROR("Unable to lock RGB buffer!");
            return UNKNOWN_ERROR;
        }
        ALOG_ASSERT(mmap.img);
        mmap.stride = getBufferStride(*buffer);
        ALOG_ASSERT(mmap.stride>0);
        break;
    }
    return NO_ERROR;
}

void StreamBuffer::unlock(void) const {
    if(buffer && *buffer) {
        GraphicBufferMapper::get().unlock(*buffer);
    }
    memset((void*)&mmap, 0, sizeof(mmap));
}

status_t StreamBuffer::cropAndLock(const Rect& rect, int usage) {
    // modify the stream parameters to target rectangle
    stream->width = rect.getWidth();
    stream->height = rect.getHeight();
    ALOG_ASSERT(!isLocked(), "The buffer is already locked");
    // clear mmap structure
    memset(&mmap, 0, sizeof(mmap));
    // don't copy pointer to local priv struct
    status_t res = lock(usage, rect);
    if (res != NO_ERROR) {
            return res;
    }

    int subsampling = 2; // default for 420 formats
    switch(stream->format) {
        case HAL_PIXEL_FORMAT_YCbCr_422_SP:
            subsampling = 1;
            /* no break */
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
        {
#define ADD_TO_PVOID(field, off) { \
            uint8_t* ptr=(uint8_t*)field;ptr+=(off);field=ptr; }
            // 8 bit format
            android_ycbcr& yuv = mmap.yuvImg;
            ADD_TO_PVOID(yuv.y, rect.left + yuv.ystride * rect.top);
            int coffset = rect.left*yuv.chroma_step/subsampling +
                    yuv.cstride * rect.top;
            ADD_TO_PVOID(yuv.cr, coffset);
            ADD_TO_PVOID(yuv.cb, coffset);
#undef ADD_TO_PVOID
            break;
        }
        case HAL_PIXEL_FORMAT_RGBA_8888:
        case HAL_PIXEL_FORMAT_RGBX_8888:
            // pixel size is 4 bytes
            mmap.img += rect.left*4 + mmap.stride * rect.top;
            break;
        case HAL_PIXEL_FORMAT_RGB_888:
            // pixel size is 3 bytes
            mmap.img += rect.left*3 + mmap.stride * rect.top;
            break;
        default:
            LOG_ERROR("Cropping supported for %d buffers!",
                stream->format);
            res = BAD_VALUE;
        }
    return res;
}

localStreamBuffer::localStreamBuffer(
        const uint32_t width, const uint32_t height, int format, int usage) :
            StreamBuffer() {
    mStream.width = width;
    mStream.height = height;
    mStream.format = format;
    mStream.usage = usage;
    if(mStream.format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        // for custom usage flags, gralloc needs exact format, not opaque
        mStream.format = HwManager::translateImplementationDefinedFormat(
                mStream.usage);
    }
    mStream.stream_type = CAMERA3_STREAM_OUTPUT;
    mStream.max_buffers = 1;
    alloc();
}

localStreamBuffer::localStreamBuffer(camera3_stream_t& newStream) :
        StreamBuffer() {
    mStream = newStream;
    alloc();
}

localStreamBuffer::~localStreamBuffer() {
    LOG_DEBUG("E");
    free();
}

status_t localStreamBuffer::attachToHw(HwManager& hwMng) {
    ALOG_ASSERT(stream == &mStream);
    return StreamBuffer::attachToHw(hwMng);
}

void localStreamBuffer::alloc() {
    ALOG_ASSERT(buffer, "%s: buffer handle not set!", __FUNCTION__);
    ALOG_ASSERT(stream, "%s: stream not set!", __FUNCTION__);
    int stride;
    // if not allocated then gralloc alloc
    GraphicBufferAllocator::get().alloc(
            stream->width, stream->height,
            stream->format, stream->usage,
            &mBuffer, &stride);
    LOG_DEBUG("allocated localStreamBuffer = %p", mBuffer);
    acquire_fence = -1;
    release_fence = -1;
    status = CAMERA3_BUFFER_STATUS_OK;
}

void localStreamBuffer::free() {
    ALOG_ASSERT(buffer, "%s: buffer handle not set!", __FUNCTION__);
    ALOG_ASSERT(stream, "%s: stream not set!", __FUNCTION__);
    if(!mBuffer) {
        return;
    }
    unlock();
    deregisterBuffer();
    GraphicBufferAllocator::get().free(mBuffer);
    mBuffer = NULL;
}

} // namespace android

