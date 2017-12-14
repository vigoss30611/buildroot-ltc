/**
******************************************************************************
@file Helpers.h

@brief Declarations of various helper classes and functions used throughout
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
#ifndef HW_HELPERS_H
#define HW_HELPERS_H

#include "HwManager.h"

#include "hardware/camera3.h"
#include "hardware/camera_common.h"

#include <utils/List.h>
#include <utils/Mutex.h>
#include <utils/KeyedVector.h>
#include <ui/Rect.h>
#include <ui/GraphicBufferAllocator.h>
#include "gralloc_priv.h"

#include "ispc/Camera.h"
#include "ispc/Sensor.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Average.h"
#include "sensorapi/sensorapi.h"
#include <felixcommon/userlog.h>

namespace android {

#define ARRAY_NUM_ELEMENTS(array) (sizeof(array)/sizeof(array[0]))

// forward declarations
class FelixCamera;
class PrivateStreamInfo;

typedef int32_t ionFd_t;
typedef uint32_t hwBufferId_t;

extern const hwBufferId_t NO_HW_BUFFER;

/**
 * @brief extract ion fd from android native buffer handle
 */
ionFd_t getIonFd(const buffer_handle_t handle);

/**
 * @brief read buffer size from android native buffer handle
 * @note used in buffer import to CI
 */
size_t getBufferSize(const buffer_handle_t handle);

/**
 * @brief read buffer stride from android native buffer handle
 */
size_t getBufferStride(const buffer_handle_t handle);

/**
 * @brief return true if the buffer is tiled
 * @note used in buffer import to CI
 */
size_t getTiled(const buffer_handle_t handle);

/**
 * @brief extract HAL private info from android camera3 stream structure
 */
PrivateStreamInfo* streamToPriv(const camera3_stream_t* stream);

/**
 * @brief acquire fence
 *
 * @param fence fence file descriptor
 * @return true if error occured
 */
bool acquireFence(const int fence);

/**
 * @brief Private stream information, stored in camera3_stream_t->priv.
 */
class PrivateStreamInfo {
public:
    bool alive;      ///<@brief utility flag for flagging dead streams
    bool registered; ///<@brief is registered by registerStreamBuffers()
    uint32_t usage;  ///<@brief storage for stream usage flags
    CI_BUFFTYPE ciBuffType; ///<@brief storage for internal type of stream

    /**
     * @brief Class constructor
     * @param stream pointer to camera pipeline instance
     */
    PrivateStreamInfo(const camera3_stream_t* stream) :
                        alive(false),
                        registered(false),
                        usage(0),
                        ciBuffType(CI_TYPE_NONE),
                        mStream(stream){}

    /**
     * @brief Stream destructor, deallocates jpeg local buffer,
     * deregisters all buffers
     */
    ~PrivateStreamInfo();

    /**
     * @brief Deregister all buffers from HW pipeline
     * @note Sets buffersMap.value to NO_HW_BUFFER so further call to
     * registerBuffers() would register the same buffers
     */
    status_t deregisterBuffers();

    /**
     * @brief Deregister specific buffer from HW pipeline
     * @param id buffer id to deregister from HW pipeline
     * @note Sets buffersMap.value to NO_HW_BUFFER so further call to
     * registerBuffers() would register the same buffers
     */

    status_t deregisterBuffer(const hwBufferId_t id);

    status_t registerBuffer(
            const buffer_handle_t& handle);

    bool isBufferRegistered(
                const buffer_handle_t& handle);
    /**
     * @brief Register provided buffers into HW pipeline
     * @param bufferSet pointer to buffer set descriptor to be registered
     * @note Processes set of provided and registered buffers in the
     * following sequence:
     * 1. Registers all new buffers provided in bufferSet parameter
     * 2. Re-registers all buffers from bufferSet which have value field
     * set to NO_HW_BUFFER (after deregisterBuffers())
     */
    status_t registerBuffers(
            const camera3_stream_buffer_set *bufferSet = NULL);

    void removeBuffer(const ionFd_t fd);

    /**
     * @brief return HW pipeline associated with current stream
     */
    cameraHw_t* getPipeline(void) { return camera.get(); }
    void setPipeline(cameraHw_t* pipeline) { camera = pipeline; }

    hwBufferId_t getBufferId(const ionFd_t ionFd) const {
        return buffersMap.indexOfKey(ionFd) >= 0 ?
            buffersMap.valueFor(ionFd).id :
            NO_HW_BUFFER;
    }
    /**
     * @brief Clears all internal buffer associations
     * @note This call does not deregister the buffers!
     */
    void clear() { buffersMap.clear(); }

    /**
     * @brief get parent stream
     */
    const camera3_stream_t* getParent() const { return mStream; }

private:
    typedef struct {
        buffer_handle_t handle;
        hwBufferId_t id;
    } hwBuffer_t;
    /**
     * @brief ionFd->bufferId mapping container
     */
    typedef KeyedVector<ionFd_t, hwBuffer_t> BuffersMap;
    BuffersMap buffersMap;

    /**
     * @brief association to specific HW pipeline
     */
    sp<cameraHw_t> camera;

    /**
     * @brief pointer to parent stream
     */
    const camera3_stream_t* mStream;
};

/**
 * @brief utility container class which represents preallocated stream buffer
 */
class StreamBuffer :
    public camera3_stream_buffer_t,
    public virtual RefBase {
public:

    /**
     * @brief pointers to raw mmapped image data
     */
    union {
        struct {
            uint8_t* img;    ///<@brief non yuv img data (rgb, jpeg)
            int      stride; ///<@brief stride in bytes
        };
        android_ycbcr yuvImg; ///<@brief android yuv img descriptor
    } mmap;

    /**
     * @brief default NULL constructor
     */
    StreamBuffer() : camera3_stream_buffer_t(),
                     mmap(), mStream(), mBuffer(NULL), mHwManager(NULL),
                     mPriv(NULL) {
        stream = &mStream;
        buffer = &mBuffer;
    }

    /**
     * @brief construct copy of stream_buffer descriptor
     * using provided pre-allocated data
     */
    StreamBuffer(camera3_stream_buffer_t& newStream);

    virtual ~StreamBuffer();

    /**
     * @brief attach given stream to first available HW output
     * @param hwMng instance of hw manager
     * @return error status
     */
    virtual status_t attachToHw(HwManager& hwMng);

    /**
     * @brief register buffer in hardware
     * @return error status
     */
    status_t registerBuffer();

    /**
     * @brief unregister buffer from hardware
     * @return error status
     */
    status_t deregisterBuffer();

    /**
     * @brief mmap buffer and set the mmap structure depending on the format
     *
     * @param usage usage flags used to mmapping
     * @param rect subrectangle of the mapped buffer.
     * after locking, the internal pointer will represent top-left corner of
     * the subrectangle
     *
     * @return error status
     */
    status_t lock(const int usage, const Rect& rect);

    /**
     * @brief mmap the buffer using default rectangle
     * @param usage usage flags used to mmapping
     *
     * @return error status
     */
    status_t lock(const int usage) {
            Rect rect(stream->width, stream->height);
        return lock(usage, rect);
    }

    /**
     * @brief mmap the buffer using default rectangle and stream
     * usage flags
     *
     * @return error status
     */
    status_t lock(void) {
        return lock(stream->usage);
    }

    /**
     * @brief unmmap buffer
     */
    void unlock(void) const;

    /**
     * @brief returns true if the buffer is already locked
     *
     * @return true if locked
     */
    bool isLocked(void) const {
            return (mmap.img!=NULL || mmap.yuvImg.y!=NULL);
    }

    /**
     * @brief lock the buffer using cropping rectangle
     *
     * @param usage usage flags for mmap
     * @param rect target subrectangle of cropped area
     *
     * @return status
     *
     * @note no buffer data is copied, it is just locked in gralloc with
     * target rectangle provided (new buffer offset is calculated) and the
     * stream descriptor is modified with new width and height
     *
     * @note supports YUV buffers only!
     */
    status_t cropAndLock(const Rect& rect, int usage);

    /**
     * @brief assignment operator from camera3_stream_buffer_t
     *
     * @param src source Android descriptor
     */
    StreamBuffer& operator=(camera3_stream_buffer_t& src) {
        ALOG_ASSERT(src.stream);
        ALOG_ASSERT(src.buffer);
        // check if we assign to cleared object
        ALOG_ASSERT(mBuffer);
        ALOG_ASSERT(!isLocked());
        mStream = *src.stream;
        mBuffer = *src.buffer;
        return *this;
    }

    /**
     * @brief assignment operator from different StreamBuffer instance
     *
     * @param src source object
     */
    StreamBuffer& operator=(StreamBuffer& src) {
        mStream = src.mStream;
        mBuffer = src.mBuffer;
        mHwManager = src.mHwManager;
        mPriv = NULL;
        mmap = src.mmap;
        return *this;
    }

    /**
     * @brief copy constructor using const Streambuffer
     *
     * @param src source object
     */
    StreamBuffer& operator=(const StreamBuffer& src) {
        return operator=(const_cast<StreamBuffer&>(src));
    }

protected:
    camera3_stream_t  mStream; ///<@brief copy of stream descriptor
    buffer_handle_t   mBuffer;
    HwManager*        mHwManager;
    PrivateStreamInfo* mPriv;
};

/**
 * @brief utility class which represents locally allocated buffer
 *
 * @note the non default constructor will allocate the buffer
 * using GraphicBufferAllocator
 */
class localStreamBuffer : public StreamBuffer {
public:
    localStreamBuffer() : StreamBuffer() {}

    /**
     * @brief construct and allocate using provided parameters
     *
     * @param width width of buffer in pixels
     * @param height height of buffer
     *
     * @param format format of pixels
     * @param usage usage flags of stream
     *
     * @note
     */
    localStreamBuffer(const uint32_t width, const uint32_t height,
                    int format, int usage);

    /**
     * @brief allocate buffer using properties of provided encoder stream
     *
     * @param newStream descriptor of stream used to allocate the buffer
     */
    localStreamBuffer(camera3_stream_t& newStream);

    /**
     * @brief destructor
     *
     * @note the destructor calls unlock() and free()
     */
    virtual ~localStreamBuffer();

    /**
     * @brief attach given stream to first available HW output
     * @param hwMng instance of hw manager
     * @return error status
     */
    virtual status_t attachToHw(HwManager& hwMng);

    /**
     * @brief allocate the buffer using GraphicBufferAllocator
     */
    void alloc();

    /**
     * @brief free the alllocated buffer
     */
    void free();

    /**
     * @brief assignment operator from camera3_stream_t
     *
     * @note does not allocate the buffer!
     */
    StreamBuffer& operator=(camera3_stream_t& src) {
        // check if we assign to cleared object
        ALOG_ASSERT(!mBuffer);
        ALOG_ASSERT(!isLocked());
        mStream = src;
        return *this;
    }

};

} // namespace android

#endif // HW_HELPERS_H
