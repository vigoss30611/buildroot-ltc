/**
******************************************************************************
@file Scaler.h

@brief interface of image scaler used in v2500 HAL module

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

#ifndef IMG_SCALER_H
#define IMG_SCALER_H

#include <utils/Errors.h>
#include <utils/Singleton.h>
#include <system/window.h>

namespace android {

class StreamBuffer;

/**
 * @brief pure virtual Base class representing generic image scaler
 * @note Represents the ImgScaler factory interface with method get()
 */
class ImgScaler {
public:
    ImgScaler(){};
    virtual ~ImgScaler(){};

    /**
     * @brief Scale image
     * @param src handle to source image buffer
     * @param dst handle to scaled destination buffer
     *
     */
    virtual status_t scale(StreamBuffer& src, StreamBuffer& dst) = 0;

    /**
     * @brief Factory getter of the ScalerXX object instance
     */
    static ImgScaler& get();

protected:
    Mutex mLock; ///<@brief to be used to scale() serialization
};

/**
 * @brief Example software implementation of YUV image scaler
 *
 * @note Not optimized against performance and memory usage
 * @note Inherits from ImgScaler base class
 * @note Does not support different pixel formats of source and destination
 */
class ScalerSw : public ImgScaler, public Singleton<ScalerSw> {
public:
    virtual ~ScalerSw();

    /**
     * @brief Scale single 8-bit image plane
     *
     * @param pSrc pointer to source data
     * @param pDst pointer to destination data
     * @param pTmp pointer to intermediate (dst_width x src_height) buffer
     * @param srcWidth width of source image
     * @param srcHeight height of source image
     * @param srcStride stride of source buffer (bytes)
     * @param dstWidth width of scaled image
     * @param dstHeight height of scaled image
     * @param dstStride stride of destination buffer (bytes)
     * @param step pixel step, for example for interleaved VU buffers = 2
     *
     * @note can be used to scale single R,G,B,Y,U,V planes
     * @note Uses raw buffer pointers as src and dst parameters
     * @note not protected against multithreaded access
     */
    virtual status_t scalePlane(uint8_t* pSrc, uint8_t* pDst, uint8_t* pTmp,
            int srcWidth, int srcHeight, int srcStride,
            int dstWidth, int dstHeight, int dstStride, int step = 1);

    /**
     * @brief Scale image
     * @param src handle to source image buffer
     * @param dst handle to scaled destination buffer
     *
     * @note internally manages own temporary image buffer
     * of size (dst_width x src_height)
     */
    virtual status_t scale(StreamBuffer& src, StreamBuffer& dst);
};

class ScalerHw : public ImgScaler, public Singleton<ScalerHw> {
private:
    ScalerHw(){}; // private constructor until implemented
};

} // namespace android

#endif /* IMG_SCALER_H */
