/**
******************************************************************************
@file JpegEncoder.h

@brief Base JpegEncoder interface for v2500 HAL module

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

#ifndef JPEG_ENCODER_H
#define JPEG_ENCODER_H

#include <utils/Errors.h>
#include <utils/Thread.h>
#include <utils/RefBase.h>
#include <utils/Singleton.h>
#include <cutils/properties.h>
#include <system/window.h>
#include <log/log.h>

#include <hardware/camera3.h>
#include <system/graphics.h>

#include "Helpers.h"

namespace android {

// forward declarations
class FelixMetadata;

/**
 * @brief adapter class providing basic Io operations which can be done
 * on current jpeg buffer position
 *
 * @note used in ExifWriter class
 */
class JpegBasicIo {
public:
    virtual ~JpegBasicIo(){};
    virtual void writeHeader(
            const uint8_t marker, const uint16_t len) = 0;
    virtual void write1byte(const uint8_t value) = 0;
    virtual void write2bytes(const uint16_t value) = 0;
};

/**
 * @brief pure class providing general interface for Jpeg encoder
 * @note the data preprocessing and compression is done asynchronously
 * in a separate thread
 */
class JpegEncoder : protected JpegBasicIo,
                    protected Thread,
                    public virtual RefBase {

public:
    /**
     * @brief listener class for signaling end of compression
     * @note FelixCamera::ProcessingThread implements this
     */
    struct JpegListener {
        // Called when JPEG compression has finished, or encountered an error
        virtual void onJpegDone(const StreamBuffer &outputBuffer,
                const int32_t jpegSize, bool success) = 0;
        virtual ~JpegListener();
    };

    JpegEncoder();
    virtual ~JpegEncoder();

    /**
     * @brief Factory getter of the JpegEncoderXX object instance
     */
    static JpegEncoder& get();

    /**
     * @brief provide main input image data buffer for compressor
     *
     * @note setting of this field is required prior to calling start()
     */
    status_t setInputBuffer(StreamBuffer& inputBuffer);

    /**
     * @brief provide output jpeg data buffer for compressor
     * @note setting of this field is required prior to calling start()
     */
    status_t setOutputBuffer(StreamBuffer& outputBuffer);

    /**
     * @brief provide jpeg listener object for compressor
     * @note setting of this field is required prior to calling start()
     */
    status_t setJpegListener(JpegListener *listener);

    /**
     * @brief provide request metadata for compressor
     * @note setting of this field is required prior to calling start()
     * @note needed for reading quality level and exif metadata
     */
    status_t setRequestMetadata(FelixMetadata& metadata);

    /**
     * @brief start of compression
     * @note prior to calling this function, setting internal configuration
     * is required
     * @note non blocking call
     */
    virtual status_t start();

    virtual status_t cancel(); ///<@brief Cancel compression, exit the thread

    virtual bool isBusy(); ///<@brief check if thread is busy

    /**
     * @brief wait for finishing compression
     */
    virtual bool waitForDone(nsecs_t timeout = kTimeoutNs);

    /**
     * @brief returns maximum supported buffer size for jpeg binary
     * @note includes JPEG headers, EXIF and thumbnail
     */
    static int32_t getMaxBufferSize() { return kJpegMaxSize; }

    /**
     * @brief returns list of supported thumbnail sizes
     */
    static void getThumbnailCapabilities(
            int32_t** sizes, int32_t& numElements);

protected:

    /**
     * @brief purposed for init compressor parameters for main image
     *
     * @note set up of quality, buffers, dimensions
     */
    virtual void initCompressorMain(const uint8_t quality) = 0;

    /**
     * @brief purposed for init compressor parameters for thumbnail image
     *
     * @note set up of quality, buffers, dimensions
     */
    virtual void initCompressorThumbnail(const uint8_t quality) = 0;

    /**
     * @brief execute compression, spawn the thread
     */
    virtual status_t compress(StreamBuffer& source) = 0;

    virtual void cleanUp(); ///<@brief cleanup after compression

    virtual status_t readyToRun(); ///<@brief thread is always ready to run

    /**
     * @brief overloaded threadLoop, implements compression
     */
    virtual bool threadLoop();

    /**
     * @brief max supported size of jpeg output buffer
     */
    static const int32_t kJpegMaxSize;

    /**
     * @brief max size of compressed thumbnail
     */
    static const int32_t kJpegThumbnailMaxSize;
    static const nsecs_t kTimeoutNs;

    /**
     * @brief static configuration of supported thumbnail sizes
     */
    static const int32_t kJpegNumSupportedThumbnailSizes[];

    Mutex mBusyMutex; ///<@brief thread lock for locking mDone
    bool mIsBusy;     ///<@brief thread busy flag
    Condition mDone;  ///<@brief compression done signaling conditional
    Mutex mMutex;     ///<@brief internal data access serialization mutex

    /**
     * @brief contains total size of main compressed image
     */
    size_t mOutputBytes;

    sp<StreamBuffer> mInputBuffer; ///<@brief main input image data buffer
    sp<StreamBuffer> mOutputBuffer; ///<@brief final jpeg output buffer

    /**
     * @brief source buffer for thumbnail compression
     *
     * @note allocated 'in place' by threadLoop()
     */
    localStreamBuffer mThumbnailInputBuffer;

    /**
     * @brief BLOB destination buffer for thumbnail compression
     *
     * @note allocated 'in place' by threadLoop()
     */
    localStreamBuffer mThumbnailOutputBuffer;

    /**
     * @brief after thumbnail compression finish,
     * contains size of thumbnail compressed image
     *
     * @note should be set by the implementation of JpegEncoder derived class
     */
    size_t mThumbnailOutputSize;

    /**
     * @brief pointer to listener object
     * @note onJpegDone() is called asynchronously after finishing compression
     */
    JpegListener  *mListener;
    sp<FelixMetadata> mRequestMetadata; ///<@brief request metadata

    /**
     * @brief path for saving debug jpeg images
     * @note initialized from img.felix.jpeg.savepath property
     */
    char mJpegSavePath[PROP_VALUE_MAX];
    bool mJpegDebugSaveImages;

    /**
     * @brief hold itself, otherwise Thread would deallocate itself after
     * each run()
     */
    static sp<JpegEncoder> mInstance;
};

} // namespace android

#endif /* JPEG_ENCODER_H */
