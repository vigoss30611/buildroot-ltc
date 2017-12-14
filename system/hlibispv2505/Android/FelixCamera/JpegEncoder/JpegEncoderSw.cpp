/**
******************************************************************************
@file JpegEncoderSw.cpp

@brief Implementation of software jpeg encoder v2500 HAL module

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
#define LOG_TAG "JpegEncoderSw"

#include "felixcommon/userlog.h"

#include <ui/GraphicBufferMapper.h>

#include "JpegEncoderSw.h"
#include "Helpers.h"

namespace android {

ANDROID_SINGLETON_STATIC_INSTANCE(JpegEncoderSw);

JpegEncoderSw::JpegEncoderSw() : JpegEncoder() {
    LOG_DEBUG("E");
    error.parent = this;
}

JpegEncoderSw::~JpegEncoderSw() {
    LOG_DEBUG("E");
}

void JpegEncoderSw::deinterleaveYuv420SP(uint8_t *vuPlane, uint8_t *uRows,
        uint8_t *vRows, int rowIndex, int width, int height, int cstride) {
    int numRows = (height - rowIndex) / 2;
    if (numRows > 8) numRows = 8;

    for (int row = 0; row < numRows; ++row) {
        int offset = ((rowIndex >> 1) + row) * cstride;
        uint8_t* vu = vuPlane + offset;
        for (int i = 0; i < (width >> 1); ++i) {
            int index = row * (width >> 1) + i;
            uRows[index] = vu[0];
            vRows[index] = vu[1];
            vu += 2;
        }
    }
}

void JpegEncoderSw::deinterleaveYuv422SP(uint8_t *vuPlane, uint8_t *uRows,
        uint8_t *vRows, int rowIndex, int width, int height, int cstride) {
    if(rowIndex>height) {
        LOG_ERROR("Invalid row index!");
        return;
    }
    int numRows = (height - rowIndex);
    if (numRows > 16) numRows = 16;

    for (int row = 0; row < numRows; ++row) {
        int offset = (rowIndex + row) * cstride;
        uint8_t* vu = vuPlane + offset;
        for (int i = 0; i < (width >> 1); ++i) {
            int index = row * (width >> 1) + i;
            uRows[index] = vu[0];
            vRows[index] = vu[1];
            vu += 2;
        }
    }
}

void JpegEncoderSw::deinterleaveRGB24(uint8_t *src,
            uint8_t *rRows, uint8_t *gRows, uint8_t *bRows,
            int numRows, int width, int stride) {

    for (int row = 0; row < numRows; ++row) {
        // RGB_888 pixel size
        uint8_t* rgb = src;
        for (int i = 0; i < width ; i++) {
            *rRows++ = *rgb++;
            *gRows++ = *rgb++;
            *bRows++ = *rgb++;
        }
        src+=stride;
    }
}

void JpegEncoderSw::configSamplingFactors(int pixelFormat) {
    // default for RGB
    mCInfo.comp_info[0].h_samp_factor = 1;
    mCInfo.comp_info[0].v_samp_factor = 1;
    mCInfo.comp_info[1].h_samp_factor = 1;
    mCInfo.comp_info[1].v_samp_factor = 1;
    mCInfo.comp_info[2].h_samp_factor = 1;
    mCInfo.comp_info[2].v_samp_factor = 1;

    switch(pixelFormat) {
    case HAL_PIXEL_FORMAT_YCbCr_422_SP:
        // 422 : only horizontal chroma downsampling
        mCInfo.comp_info[1].v_samp_factor = 2;
        mCInfo.comp_info[2].v_samp_factor = 2;
        /* no break */
    case HAL_PIXEL_FORMAT_YCbCr_420_888:
#ifdef USE_IMG_SPECIFIC_PIXEL_FORMATS
    case HAL_IMG_FormatYUV420PP:
    case HAL_IMG_FormatYUV420SP:
#endif
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
        mCInfo.comp_info[0].h_samp_factor = 2;
        mCInfo.comp_info[0].v_samp_factor = 2;
        // 420: chroma is horizontally and vertically downsampled
        break;
    default:
        break;
    }

}

void JpegEncoderSw::setJpegCompressStruct(
        int width, int height, int quality, int pixelFormat) {

    mCInfo.image_width = width;
    mCInfo.image_height = height;

    // output colorspace set the same as input - no conversion needed
    // but for RGB, output compression ratio worse than JCS_YCbCr
    switch (pixelFormat) {
#if(0)
    // currently not supported
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
        mCInfo.input_components = 4;
        mCInfo.in_color_space = JCS_RGBA_8888;
        break;
#endif
    case HAL_PIXEL_FORMAT_RGB_888:
        mCInfo.input_components = 3;
        mCInfo.in_color_space = JCS_RGB;
        break;
#if(0)
        //not supported
    case HAL_PIXEL_FORMAT_RGB_565:
        mCInfo.input_components = 3;
        mCInfo.in_color_space = JCS_RGB_565;
        break;
#endif
    default:
        // : check for unsupported yuv formats
        mCInfo.input_components = 3;
        mCInfo.in_color_space = JCS_YCbCr;
        break;
    }
    jpeg_set_defaults(&mCInfo);
    if(pixelFormat == HAL_PIXEL_FORMAT_RGB_888) {
        // override output colorspace for RGB mode
        jpeg_set_colorspace(&mCInfo, JCS_RGB);
    }
    jpeg_set_quality(&mCInfo, quality, TRUE);
    mCInfo.raw_data_in = TRUE;
    mCInfo.dct_method = JDCT_IFAST;
    configSamplingFactors(pixelFormat);
}

void JpegEncoderSw::initCompressorMain(const uint8_t quality) {
    JpegError error;
    error.parent = this;

    mCInfo.err = jpeg_std_error(&error);

    jpeg_create_compress(&mCInfo);

    jpegDestMgr = JpegDestination();
    jpegDestMgr.parent = this;
    jpegDestMgr.init_destination = jpegInitDestination;
    jpegDestMgr.empty_output_buffer = jpegEmptyOutputBuffer;
    jpegDestMgr.term_destination = jpegTermDestination;
    mCInfo.dest = &jpegDestMgr;

    setJpegCompressStruct(
            mInputBuffer->stream->width,
            mInputBuffer->stream->height,
            quality,
            mInputBuffer->stream->format);

    // init compression, emit SOI
    jpeg_start_compress(&mCInfo, TRUE);
}

void JpegEncoderSw::initCompressorThumbnail(const uint8_t quality) {

    // : allocate destination for thumbnail
    mCInfo.err = jpeg_std_error(&error);

    jpeg_create_compress(&mCInfo);

    jpegDestMgr = JpegDestination();
    jpegDestMgr.parent = this;
    jpegDestMgr.init_destination = jpegInitThumbDestination;
    jpegDestMgr.empty_output_buffer = jpegEmptyOutputBuffer;
    jpegDestMgr.term_destination = jpegTermThumbDestination;
    mCInfo.dest = &jpegDestMgr;

    setJpegCompressStruct(
            mThumbnailInputBuffer.stream->width,
            mThumbnailInputBuffer.stream->height,
            quality,
            mThumbnailInputBuffer.stream->format);

    // init compression, emit SOI
    jpeg_start_compress(&mCInfo, TRUE);

}

status_t JpegEncoderSw::compress(StreamBuffer& source) {

    int width = mCInfo.image_width;
    int height = mCInfo.image_height;

    // lock for software read access
    source.lock(GRALLOC_USAGE_SW_READ_RARELY);

    // prepare planes, deinterleave and compress image rows
    // supports : YUV420SP, 420P (not really used in Felix HW) and YUV422SP
    // and RGB24
    if(mCInfo.in_color_space == JCS_YCbCr) {
        // YUV only 420 and 422 supported, horizontal subsampling is always 1:
        // for 420: vert subsampling is 1,
        // for 422: vert subsampling is 2
        int vert_downsampling = mCInfo.comp_info[1].v_samp_factor;

        JSAMPROW y[16]; // no downsampling of luma, always
        JSAMPROW cb[8 * vert_downsampling];
        JSAMPROW cr[8 * vert_downsampling];
        JSAMPARRAY planes[3] = {
            y, cb, cr
        };

        uint8_t* yPlane = (uint8_t*)source.mmap.yuvImg.y;
        bool crInterleaved = (source.mmap.yuvImg.chroma_step == 2);
        uint8_t* uRows, *vRows;

        if(crInterleaved) {
            if(vert_downsampling == 1) {
                LOG_DEBUG("Input is YUV420SP");
            } else if(vert_downsampling == 2) {
                LOG_DEBUG("Input is YUV422SP");
            } else {
                LOG_DEBUG("Input is UNKNOWN");
                return NO_INIT;
            }

            // modify buffer sizes dependent of cr downsampling
            uRows = new uint8_t [vert_downsampling * 8 * (width >> 1)];
            vRows = new uint8_t [vert_downsampling * 8 * (width >> 1)];

            // pre-init u and v rows
            for (int i = 0; i < 16; i++) {
                // construct u row and v row
                if(vert_downsampling == 1) {
                    if ((i & 1) == 0) {
                        int offset = (i >> 1) * (width >> 1);
                        // height and width are both halved because of
                        // downsampling
                        cb[i / 2] = uRows + offset;
                        cr[i / 2] = vRows + offset;
                    }
                } else {
                    int offset = i * (width >> 1);
                    cb[i] = uRows + offset;
                    cr[i] = vRows + offset;
                }
            }

            // process 16 lines of Y and 8*vert_downsampling lines of U/V
            // each time.
            while (mCInfo.next_scanline < mCInfo.image_height) {
                if(exitPending()) {
                    break;
                }
                // deinterleave u and v
                switch (source.stream->format) {
                case HAL_PIXEL_FORMAT_YCrCb_420_SP:
                {
                    // YVU
                    // cr is first byte in interleaved chroma, cb = cr+1
                    uint8_t* vuSemiPlane = (uint8_t*)source.mmap.yuvImg.cr;
                    deinterleaveYuv420SP(vuSemiPlane, vRows, uRows,
                        mCInfo.next_scanline,
                        width, height, source.mmap.yuvImg.cstride);
                    break;
                }
                case HAL_PIXEL_FORMAT_YCbCr_420_888:
                {
                    // YUV
                    // cb is first byte in interleaved chroma, cr = cb+1
                    uint8_t* vuSemiPlane = (uint8_t*)source.mmap.yuvImg.cb;
                    deinterleaveYuv420SP(vuSemiPlane, uRows, vRows,
                        mCInfo.next_scanline,
                        width, height, source.mmap.yuvImg.cstride);
                    break;
                }
                case HAL_PIXEL_FORMAT_YCbCr_422_SP:
                {
                    // cb is first byte in interleaved chroma, cb = cr+1
                    uint8_t* vuSemiPlane = (uint8_t*)source.mmap.yuvImg.cb;
                    deinterleaveYuv422SP(vuSemiPlane, uRows, vRows,
                        mCInfo.next_scanline,
                        width, height, source.mmap.yuvImg.cstride);
                    break;
                }
                }

                for (int i = 0; i < 16; i++) {
                    // y row is read straight from y plane
                    y[i] = yPlane + (mCInfo.next_scanline + i) *
                            source.mmap.yuvImg.ystride;

                }
                jpeg_write_raw_data(&mCInfo, planes, 16);
            }
            delete [] uRows;
            delete [] vRows;
        } else {
            // if input yuv is planar, no need to deinterleave
            if(vert_downsampling == 1) {
                LOG_DEBUG("Input is YUV420P");
            } else if(vert_downsampling == 2) {
                LOG_DEBUG("Input is YUV422P - not supported!");
                return NO_INIT;
            }
            // process 16 lines of Y and 8*vert_downsampling lines of U/V
            // each time.
            while (mCInfo.next_scanline < mCInfo.image_height) {
                if(exitPending()) {
                    break;
                }
                // Jpeg library ignores the rows whose indices are greater
                // than height.
                for (int i = 0; i < 16; i++) {
                    // y row
                    y[i] = yPlane + (mCInfo.next_scanline + i) *
                            source.mmap.yuvImg.ystride;
                    if((i & 1) == 0) {
                        // height and width are both halved because of
                                // downsampling
                        int offset = (mCInfo.next_scanline + i) *
                                source.mmap.yuvImg.cstride;

                        cb[i / 2] = (uint8_t*)source.mmap.yuvImg.cb + offset;
                        cr[i / 2] = (uint8_t*)source.mmap.yuvImg.cr + offset;
                    }
                }
                jpeg_write_raw_data(&mCInfo, planes, 16);
            }
        }
    } else if (mCInfo.in_color_space == JCS_RGB) {
        const int PROC_SCANLINES = DCTSIZE;
        JSAMPROW r[PROC_SCANLINES];
        JSAMPROW g[PROC_SCANLINES];
        JSAMPROW b[PROC_SCANLINES];
        JSAMPARRAY planes[3] = { r, g, b };
        uint8_t* rgbPlane = source.mmap.img;
        ALOG_ASSERT(rgbPlane);
        uint8_t* rRows, *gRows, *bRows;

        LOG_DEBUG("Input is RGB");

        rRows = new uint8_t [PROC_SCANLINES * width];
        gRows = new uint8_t [PROC_SCANLINES * width];
        bRows = new uint8_t [PROC_SCANLINES * width];

        if(!rRows || !gRows || !bRows) {
            return NO_MEMORY;
        }
        // Jpeg library ignores the rows whose indices are greater
        // than height.
        for (int i = 0, offset=0; i < PROC_SCANLINES; i++, offset+=width) {
            r[i] = rRows + offset;
            g[i] = gRows + offset;
            b[i] = bRows + offset;
        }
        // process PROC_SCANLINES lines of each plane
        while (mCInfo.next_scanline < mCInfo.image_height) {
            if(exitPending()) {
                break;
            }
            // deinterleave r, g, b
#if(0)
            deinterleaveRGB24_old(rgbPlane, rRows, gRows, bRows,
                mCInfo.next_scanline,
                width, height, source.mmap.stride);
#else
             int numRows = (height - mCInfo.next_scanline);
             if (numRows > PROC_SCANLINES) numRows = PROC_SCANLINES;
             deinterleaveRGB24(rgbPlane,
                         rRows, gRows, bRows,
                         numRows, width, source.mmap.stride);
             rgbPlane+=numRows*source.mmap.stride;
#endif
            jpeg_write_raw_data(&mCInfo, planes, PROC_SCANLINES);
        }
        delete [] rRows;
        delete [] gRows;
        delete [] bRows;
    } else {
        ALOG_ASSERT("Input format different than RGB or YUV!");
    }
    if(exitPending()) {
        // if exit is pending, do not finish compression
        mOutputBytes = 0;
        jpeg_abort_compress(&mCInfo);
    } else {
        jpeg_finish_compress(&mCInfo);
    }
    source.unlock();
    return OK;
}

void JpegEncoderSw::cleanUp() {
    jpeg_destroy_compress(&mCInfo);
    JpegEncoder::cleanUp();
}

void JpegEncoderSw::jpegErrorHandler(j_common_ptr cinfo) {
    JpegError *error = static_cast<JpegError*>(cinfo->err);
    error->parent->mJpegErrorInfo = cinfo;
}

void JpegEncoderSw::jpegInitDestination(j_compress_ptr cinfo) {
    JpegDestination *dest= static_cast<JpegDestination *>(cinfo->dest);

    // lock for software write access
    dest->parent->mOutputBuffer->lock(GRALLOC_USAGE_SW_WRITE_RARELY);
    dest->next_output_byte = (JOCTET *)(dest->parent->mOutputBuffer->mmap.img);
    dest->free_in_buffer = JpegEncoder::kJpegMaxSize;

    LOG_DEBUG("Setting destination to %p, size %d",
            dest->next_output_byte,
            dest->free_in_buffer);
}

boolean JpegEncoderSw::jpegEmptyOutputBuffer(
        __maybe_unused j_compress_ptr cinfo) {
    LOG_ERROR("JPEG destination buffer overflow!");
    return true;
}

// static
void JpegEncoderSw::jpegTermDestination(
        __maybe_unused j_compress_ptr cinfo) {
    JpegDestination *dest= static_cast<JpegDestination *>(cinfo->dest);

    dest->parent->mOutputBytes = dest->parent->kJpegMaxSize -
            (int32_t) cinfo->dest->free_in_buffer;

    LOG_DEBUG("Done writing JPEG data. Image size %d bytes, "\
            "%d bytes left in buffer.",
            dest->parent->mOutputBytes,
            cinfo->dest->free_in_buffer);
}

 // static
void JpegEncoderSw::jpegInitThumbDestination(j_compress_ptr cinfo) {
    JpegDestination *dest= static_cast<JpegDestination *>(cinfo->dest);

    // lock for software write access
    dest->parent->mThumbnailOutputBuffer.lock(GRALLOC_USAGE_SW_WRITE_RARELY);
    dest->next_output_byte =
            (JOCTET *)(dest->parent->mThumbnailOutputBuffer.mmap.img);
    dest->free_in_buffer = JpegEncoder::kJpegThumbnailMaxSize;

    LOG_DEBUG("Setting thumbnail destination to %p, size %d",
            dest->next_output_byte,
            dest->free_in_buffer);
}

// static
void JpegEncoderSw::jpegTermThumbDestination(
        __maybe_unused j_compress_ptr cinfo) {
    JpegDestination *dest= static_cast<JpegDestination *>(cinfo->dest);

    dest->parent->mThumbnailOutputSize = dest->parent->kJpegThumbnailMaxSize -
            (int32_t) cinfo->dest->free_in_buffer;

    LOG_DEBUG("Done writing JPEG thumbnail data. Image size %d bytes, "\
            "%d bytes left in buffer.",
            dest->parent->mThumbnailOutputSize,
            cinfo->dest->free_in_buffer);
}

inline void JpegEncoderSw::writeHeader(
        const uint8_t marker, const uint16_t len){
    jpeg_write_m_header(&mCInfo, marker, len);
}

inline void JpegEncoderSw::write1byte(const uint8_t value) {
    jpeg_write_m_byte(&mCInfo, value);
}

inline void JpegEncoderSw::write2bytes(const uint16_t value) {
    /* Emit a 2-byte integer; these are always MSB first in JPEG files */
    write1byte((value >> 8) & 0xFF);
    write1byte(value & 0xFF);
}

} // namespace android
