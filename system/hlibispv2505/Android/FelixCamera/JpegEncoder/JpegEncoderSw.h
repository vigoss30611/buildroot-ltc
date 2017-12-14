/**
******************************************************************************
@file JpegEncoderSw.h

@brief Interface of software jpeg encoder v2500 HAL module

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

#ifndef JPEG_ENCODER_SW_H
#define JPEG_ENCODER_SW_H

#include "JpegEncoder.h"

extern "C" {
#include "jpeglib.h"
}

namespace android {

/**
 * @class JpegEncoderSw
 *
 * @brief Software Jpeg encoder
 * @note implements software compression using libjpeg
  */
class JpegEncoderSw: public JpegEncoder,
                      public Singleton<JpegEncoderSw> {

public:
    JpegEncoderSw();
    virtual ~JpegEncoderSw();

protected:
    /**
     * @brief libjpeg error handler
     */
    static void jpegErrorHandler(j_common_ptr cinfo);

    /**
     * @brief libjpeg init compression callback
     */
    static void jpegInitDestination(j_compress_ptr cinfo);

    /**
     * @brief libjpeg terminate compression callback
     */
    static void jpegTermDestination(j_compress_ptr cinfo);

    static void jpegInitThumbDestination(j_compress_ptr cinfo);
    static void jpegTermThumbDestination(j_compress_ptr cinfo);

    /**
     * @brief libjpeg empty output buffer callback
     */
    static boolean jpegEmptyOutputBuffer(j_compress_ptr cinfo);

    virtual void initCompressorMain(const uint8_t quality);
    virtual void initCompressorThumbnail(const uint8_t quality);

    /**
     * @brief libjpeg implementation of compression
     */
    virtual status_t compress(StreamBuffer& source);

    /**
     * @brief overloaded version of base class implementation, \
     * @note calls jpeg_destroy_compress()
     */
    virtual void cleanUp();

    /**
     * @brief implementation of JpegBasicIo interface
     */
    virtual inline void writeHeader(const uint8_t marker, const uint16_t len);
    virtual inline void write1byte(const uint8_t value);
    virtual inline void write2bytes(const uint16_t value);

private:

    /**
     * @brief libjpeg jpeg_error_mgr interface
     */
    struct JpegError : public jpeg_error_mgr {
        JpegEncoderSw *parent;
    };
    j_common_ptr mJpegErrorInfo;
    /**
     * @brief libjpeg jpeg_destination_mgr interface
     */
    struct JpegDestination : public jpeg_destination_mgr {
        JpegEncoderSw *parent;
    };

    /**
     * @brief deinterleave yuv420SP to planar buffers needed by libjpeg
     */
    void deinterleaveYuv420SP(uint8_t *vuPlane,
            uint8_t *uRows, uint8_t *vRows,
            int rowIndex, int width, int height, int cstride);
    /**
     * @brief deinterleave yuv422SP to planar buffers needed by libjpeg
     */
    void deinterleaveYuv422SP(uint8_t *vuPlane,
            uint8_t *uRows, uint8_t *vRows,
            int rowIndex, int width, int height, int cstride);
    /**
     * @brief deinterleave rgb24 to planar buffers needed by libjpeg
     */
    void deinterleaveRGB24(uint8_t *src,
            uint8_t *rRows, uint8_t *gRows, uint8_t *bRows,
            int numRows, int width, int stride);
    /**
     * @brief configure sampling factors depending on input format (420, 422)
     */
    void configSamplingFactors(int pixelFormat);

    /**
     * @brief initialize libjpeg internal compression structure
     */
    void setJpegCompressStruct(int width, int height,
            int quality, int pixelFormat);

    JpegDestination jpegDestMgr;
    jpeg_compress_struct mCInfo; ///<@brief libjpeg compression structure
    JpegError error;
};

} // namespace android

#endif // JPEG_ENCODER_SW_H
