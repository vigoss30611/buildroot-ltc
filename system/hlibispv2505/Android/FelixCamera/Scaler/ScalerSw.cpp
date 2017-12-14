/**
******************************************************************************
@file ScalerSw.cpp

@brief Definition of software scaler for v2500 HAL module

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
#define LOG_TAG "ScalerSw"

#include <stdarg.h>
#include <ui/Rect.h>
#include <ui/GraphicBufferMapper.h>
#include <ui/GraphicBufferAllocator.h>

#include "Scaler.h"
#include "HwManager.h"
#include "Helpers.h"

extern "C" {
#include "yuvscale.h"
};
#include <log/log.h>
#include <felixcommon/userlog.h>

YUVSCALE_STATE g_YUVScaleState = YUVSCALE_STATE();

extern "C"
int __sc_printf(const char* fmt,...) {
    int res;
    va_list args;
    va_start(args, fmt);
    res = LOG_PRI_VA(ANDROID_LOG_DEBUG, LOG_TAG, fmt, args);
    va_end(args);
    return res;
}

namespace android {

ANDROID_SINGLETON_STATIC_INSTANCE(ScalerSw);

ImgScaler& ImgScaler::get() {
#ifdef USE_HARDWARE_SCALER
#error Hardware scaler not configured!
        return ScalerHw::getInstance();
#else
        return ScalerSw::getInstance();
#endif
    }

ScalerSw::~ScalerSw(){
}

status_t ScalerSw::scalePlane(uint8_t* pSrc, uint8_t* pDst, uint8_t* pTmp,
            int srcWidth, int srcHeight, int srcStride,
            int dstWidth, int dstHeight, int dstStride, int step) {

    /* scale horizontal to temp buffer */
    ProgramScaler(srcWidth,dstWidth);

    for(int x=0;x<srcHeight;x++)
    {
        HorizScaleLine(pSrc + srcStride*x, step, pTmp + dstStride*x, step);
    }

    /* scale vertical to dst buffer */
    ProgramScaler(srcHeight,dstHeight);

    for(int x=0;x<dstWidth;x++)
    {
        HorizScaleLine(pTmp + x*step, dstStride, pDst + x*step, dstStride);
    }
    return OK;
}

status_t ScalerSw::scale(StreamBuffer& src, StreamBuffer& dst){
    Mutex::Autolock l(mLock);

    status_t res = NO_ERROR;

    int src_format = src.stream->format;
    if(src_format!=dst.stream->format) {
        LOG_ERROR("ScalerSw does not support different src and dst formats"
                "%d-%d", src_format, dst.stream->format);
        return BAD_VALUE;
    }
    // convert implementation defined format to something more specific
    if(src_format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        src_format = HwManager::translateImplementationDefinedFormat(
                        src.stream->usage);
    }

    // 0. alloc tmp (dst.width x src.height)
    // 1. scale image horizontally to the tmp
    // 2. scale vertically to destination
    localStreamBuffer tmp(
                    dst.stream->width, src.stream->height,
                    dst.stream->format,
                    GRALLOC_USAGE_SW_WRITE_RARELY |
                    GRALLOC_USAGE_SW_READ_RARELY);
    tmp.lock();

    // lock buffers if not mmapped yet
    if(!src.isLocked()) {
        src.lock(GRALLOC_USAGE_SW_READ_RARELY);
    }
    if(!dst.isLocked()) {
        dst.lock(GRALLOC_USAGE_SW_WRITE_RARELY);
    }

    LOG_DEBUG("Scaling format 0x%x from %dx%d to %dx%d",
            src_format,
            src.stream->width, src.stream->height,
            dst.stream->width, dst.stream->height);

    if(src_format == HAL_PIXEL_FORMAT_YCrCb_420_SP ||
        src_format == HAL_PIXEL_FORMAT_YCbCr_420_888 ||
        src_format == HAL_PIXEL_FORMAT_YCbCr_422_SP) {

        android_ycbcr &ycbcr_src = src.mmap.yuvImg;
        android_ycbcr &ycbcr_tmp = tmp.mmap.yuvImg;
        android_ycbcr &ycbcr_dst = dst.mmap.yuvImg;

        int heightSampling = 2;
        // get plane strides and sizes
        switch(src_format) {
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
            ALOG_ASSERT(ycbcr_src.chroma_step == 2,
                    "No support for planar YUV formats!");
            // treat interleaved U and V the same way so no need to split code here
            break;
        case HAL_PIXEL_FORMAT_YCbCr_422_SP:
            heightSampling = 1;
            break;
        default:
            res = BAD_VALUE;
        }

        // scale image planes
        if(res == NO_ERROR) {
            //LOG_DEBUG("Scale luma");
            scalePlane((uint8_t*)ycbcr_src.y,
                       (uint8_t*)ycbcr_dst.y,
                       (uint8_t*)ycbcr_tmp.y,
                src.stream->width, src.stream->height, ycbcr_src.ystride,
                dst.stream->width, dst.stream->height, ycbcr_dst.ystride);

            //LOG_DEBUG("Scale chroma even bytes");
            scalePlane((uint8_t*)ycbcr_src.cr,
                       (uint8_t*)ycbcr_dst.cr,
                       (uint8_t*)ycbcr_tmp.cr,
                src.stream->width>>1,
                src.stream->height/heightSampling,
                ycbcr_src.cstride,
                dst.stream->width>>1,
                dst.stream->height/heightSampling,
                ycbcr_dst.cstride, 2);

            //LOG_DEBUG("Scale chroma odd bytes");
            scalePlane((uint8_t*)ycbcr_src.cr+1,
                       (uint8_t*)ycbcr_dst.cr+1,
                       (uint8_t*)ycbcr_tmp.cr+1,
                    src.stream->width>>1,
                    src.stream->height/heightSampling,
                    ycbcr_src.cstride,
                    dst.stream->width>>1,
                    dst.stream->height/heightSampling,
                    ycbcr_dst.cstride, 2);
        }
    } else if(src_format == HAL_PIXEL_FORMAT_RGBA_8888 ||
            src_format == HAL_PIXEL_FORMAT_RGBX_8888 ||
            src_format == HAL_PIXEL_FORMAT_RGB_888) {

        int pixelsize = src_format == HAL_PIXEL_FORMAT_RGB_888 ? 3 : 4;
        scalePlane((uint8_t*)src.mmap.img,
                   (uint8_t*)dst.mmap.img,
                   (uint8_t*)tmp.mmap.img,
            src.stream->width, src.stream->height, src.mmap.stride,
            dst.stream->width, dst.stream->height, dst.mmap.stride, pixelsize);

        //LOG_DEBUG("Scale chroma even bytes");
        scalePlane((uint8_t*)src.mmap.img+1,
                   (uint8_t*)dst.mmap.img+1,
                   (uint8_t*)tmp.mmap.img+1,
            src.stream->width, src.stream->height, src.mmap.stride,
            dst.stream->width, dst.stream->height, dst.mmap.stride, pixelsize);

        //LOG_DEBUG("Scale chroma odd bytes");
        scalePlane((uint8_t*)src.mmap.img+2,
                   (uint8_t*)dst.mmap.img+2,
                   (uint8_t*)tmp.mmap.img+2,
            src.stream->width, src.stream->height, src.mmap.stride,
            dst.stream->width, dst.stream->height, dst.mmap.stride, pixelsize);
    } else {
        LOG_ERROR("ScalerSw supports YUV and RGB formats only");
        res = BAD_VALUE;
    }

    src.unlock();
    tmp.unlock();
    dst.unlock();
    tmp.free();

    return res;
}

}; // namespace android
