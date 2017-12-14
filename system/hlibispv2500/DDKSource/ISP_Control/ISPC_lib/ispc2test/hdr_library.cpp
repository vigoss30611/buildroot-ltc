/**
*******************************************************************************
@file hdr_library.cpp

@brief Interface with the HDRLibs from ISPC structures

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

******************************************************************************/

#include "ispctest/hdf_helper.h"

#include <img_errors.h>
#include <img_defs.h>
#include <ispc/Save.h>

#include <felixcommon/pixel_format.h>
#include <felixcommon/ci_alloc_info.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_hdrlib"

//#define VERBOSE_DEBUG LOG_DEBUG
//#define SAVE_CONV
#define VERBOSE_DEBUG

#if defined(HDRLIBS_SEQHDR)

#include "seqHDR/SeqHQ.h"
#include "image/image_source.h"
#include "util/timer.h"
#include <vector>

typedef float ISPC_image_T;
#define BLACK_CLIP (0.0f) //(0.06f)//(16/255.0)
#define WHITE_CLIP (0.80f)

class ISPC_image: public images::image_source<ISPC_image_T>
{
protected:
    struct Frame
    {
        float gain;
        float exposureMS;
        ISPC_image_T black;
        ISPC_image_T white;
        
        ISPC::Buffer source;
        images::image<ISPC_image_T> *raster;
        
        Frame(const ISPC::Buffer &src);
        // no destructor of dynamic data because should be cleared by owner
    };

    std::vector<Frame> frames;
    int curr;
    
public:
    
    ISPC_image();
    virtual ~ISPC_image();
    
    /**
     * @param[in] buff source buffer
     * @param[in] exposure time in us
     * @param[in] gain multiplier in sensor
     * @param[in] buff_white light saturation point
     * @param[in] buff_black value that should be considered black
     */
    IMG_RESULT addBuffer(const ISPC::Buffer &buff, unsigned int exposure,
        float gain, unsigned int buff_white, unsigned int buff_black);
    
    static images::image<ISPC_image_T>* convert(const ISPC::Buffer &input);
    
    static IMG_RESULT convert(const images::image<ISPC_image_T> &input,
        CI_BUFFER &sOutput, IMG_UINT32 outputStride);
    
    
// from images::image_source class:
    
    virtual int nframes();
    
    virtual int idx();
    
    virtual int next();
    
    virtual bool get(images::image<ISPC_image_T> &p_image);
    
    virtual const images::image<ISPC_image_T>* get();
    
    virtual ISPC_image_T BlackPoint();
    
    virtual ISPC_image_T WhitePoint();
    
    virtual float shutter();
    
    virtual float iso();
};

static unsigned int GetPixel8_32(const ISPC::Buffer &buff, int x, int y,
    int c)
{
    IMG_UINT32 *pData = (IMG_UINT32*)buff.data;
    IMG_ASSERT(c<=4);
    
    return (pData[y*buff.stride/4 + x]>>(c*8))&0xFF;
}

static unsigned int GetPixel10_32(const ISPC::Buffer &buff, int x, int y,
    int c)
{
    IMG_UINT32 *pData = (IMG_UINT32*)buff.data;
    IMG_ASSERT(c<=3);
    
    return (pData[y*buff.stride/4 + x]>>(c*10))&0x3FF;
}

ISPC_image::Frame::Frame(const ISPC::Buffer &buff): source(buff)
{
    source.data = 0;
    this->raster = ISPC_image::convert(buff);
    if (!raster)
    {
        LOG_ERROR("Failed to convert buffer\n");
        return;
    }
    
    this->white = 1.0f;
    this->black = 0.0f;
}

images::image<ISPC_image_T>* ISPC_image::convert(const ISPC::Buffer &input)
{
    PIXELTYPE pixelType;
    IMG_RESULT ret;
    
    if (input.isTiled)
    {
        LOG_ERROR("Does not support tiled buffers as input!\n");
        return 0;
    }
    
    ret = PixelTransformRGB(&pixelType, input.pxlFormat);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get information about %s, is it an RGB format?",
            FormatString(input.pxlFormat));
        return 0;
    }
    
    if (pixelType.ui8PackedStride != 4)
    {
        LOG_ERROR("Does not support other format than 32b RGB\n");
        return 0;
    }
    
    LOG_INFO("Converting %s buffer to HDRLibs image\n",
        FormatString(input.pxlFormat));
    
    const ISPC_image_T maxPixel = (float)((1<<(pixelType.ui8BitDepth))-1);
    unsigned int (*getPixel)(const ISPC::Buffer &buff, int x, int y, int c)
        = 0;
        
    if (pixelType.ui8BitDepth == 8)
    {
        getPixel = GetPixel8_32;
    }
    else
    {
        getPixel = GetPixel10_32;
    }
    
    images::image<ISPC_image_T> *raster =
        new images::image<ISPC_image_T>(input.width, input.height, 3);
    if (!raster)
    {
        LOG_ERROR("failed to create raster image\n");
        return 0;
    }
    
    ISPC_image_T *data = raster->data();
    
    int i, j;
    const int r = 2, g = 1, b = 0;
    for ( i = 0 ; i < input.height ; i++ )
    {
        for ( j = 0 ; j < input.width ; j++ )
        {
            unsigned int pixel = 0;
            ISPC_image_T pf = 0;
            
            pixel = getPixel(input, j, i, r);
            pf = pixel/maxPixel; 
            data[i*input.width*3 + j*3 + 0] = pf;
                                
            pixel = getPixel(input, j, i, g);
            pf = pixel/maxPixel; 
            data[i*input.width*3 + j*3 + 1] = pf;
                
            pixel = getPixel(input, j, i, b);
            pf = pixel/maxPixel; 
            data[i*input.width*3 + j*3 + 2] = pf;
        }
    }
    
    return raster;
}

IMG_RESULT ISPC_image::convert(const images::image<ISPC_image_T> &input,
    CI_BUFFER &sOutput, IMG_UINT32 outputStride)
{
    unsigned int i, j, c;
    int v = 0;
    const int image_w = input.width(); // same for output than input
    const ISPC_image_T *data = input.data();
    IMG_UINT16 *outData = (IMG_UINT16*)sOutput.data;
    
    if (input.channels() != 3)
    {
        LOG_ERROR("input image has more than 3 channels!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (image_w*4*sizeof(IMG_UINT16) > outputStride)
    {
        LOG_ERROR("cannot fit in given stride %d (need %d)\n",
            outputStride, image_w*4*sizeof(IMG_UINT16));
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (outputStride*input.height() > sOutput.ui32Size)
    {
        LOG_ERROR("does not fit in provided output buffer\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if(outputStride%sizeof(IMG_UINT16) != 0)
    {
        LOG_ERROR("unexpected stride\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    
    LOG_INFO("Converting %dx%d image to HDF input\n",
        input.width(), input.height());
    
    v = 0;
    const unsigned output_s = outputStride/sizeof(IMG_UINT16);
    const unsigned int maxPixel = (1<<16)-1;
    const int r = 2, g = 1, b = 0;
    unsigned pixels[3];
    for ( i = 0 ; i < input.height() ; i++ )
    {
        for ( j = 0 ; j < input.width() ; j++ )
        {
            for ( c = 0 ; c < 3u ; c++)
            {
                pixels[c] = (unsigned)
                    floor(data[i*input.width()*3 + j*3 + c]*maxPixel);
            }
            outData[i*output_s + j*4 + r] = IMG_MIN_INT(pixels[0], maxPixel);
            v++;
            outData[i*output_s + j*4 + g] = IMG_MIN_INT(pixels[1], maxPixel);
            v++;
            outData[i*output_s + j*4 + b] = IMG_MIN_INT(pixels[2], maxPixel);
            v++;
            // additional output
            outData[i*output_s + j*4 + 3] = 0;
            v++;
        }
    }
    
    return IMG_SUCCESS;
}

ISPC_image::ISPC_image()
{
    curr = 0;
}

ISPC_image::~ISPC_image()
{
    std::vector<Frame>::iterator it = frames.begin();
    while(it != frames.end())
    {
        if (it->raster)
        {
            delete it->raster;
            it->raster = 0;
        }
        it++;
    }
    frames.clear();
}

IMG_RESULT ISPC_image::addBuffer(const ISPC::Buffer &buff, unsigned int exposure, float gain,
        unsigned int buff_white, unsigned int buff_black)
{
    PIXELTYPE pixelType;
    IMG_RESULT ret;
    
    if (buff.isTiled)
    {
        LOG_ERROR("Does not support tiled buffers as input!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    
    ret = PixelTransformRGB(&pixelType, buff.pxlFormat);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get information about %s, is it an RGB format?",
            FormatString(buff.pxlFormat));
        return IMG_ERROR_FATAL;
    }
    
    if (pixelType.ui8PackedStride != 4)
    {
        LOG_ERROR("Does not support other format than 32b RGB\n");
        return IMG_ERROR_FATAL;
    }
    
    Frame f(buff);
    const ISPC_image_T maxPixel = (ISPC_image_T)((1<<pixelType.ui8BitDepth)-1);
    
    if (!f.raster)
    {
        LOG_ERROR("Failed to convert buffer to raster\n");
        return IMG_ERROR_FATAL;
    }
    
    f.exposureMS = exposure/1000.0f;
    f.gain = gain;
    f.white = ((ISPC_image_T)buff_white)/maxPixel;
    f.black = ((ISPC_image_T)buff_black)/maxPixel;
    VERBOSE_DEBUG("buff_white=%u maxPixel=%f\n", buff_white, maxPixel);
    VERBOSE_DEBUG("created raster 0x%p exp=%f gain=%f white=%f black=%f\n",
        f.raster, f.exposureMS, f.gain, f.white, f.black);
    
    frames.push_back(f);
    
    return IMG_SUCCESS;
}


int ISPC_image::nframes()
{
    VERBOSE_DEBUG("nFrames %d\n", frames.size());
    return frames.size();
}

int ISPC_image::idx()
{
    VERBOSE_DEBUG("curr %d\n", curr);
    return curr;
}

int ISPC_image::next()
{
    if (frames.size() > 0)
    {
        curr = (curr+1)%(frames.size());
        VERBOSE_DEBUG("curr=%d\n", curr);
        return curr;
    }
    VERBOSE_DEBUG("curr=%d ret -1\n", curr);
    return -1;
}

bool ISPC_image::get(images::image<ISPC_image_T> &p_image)
{
    VERBOSE_DEBUG("affect raster 0x%p (curr %d)\n", frames[curr].raster, curr);
    p_image = *frames[curr].raster;
    return true;
}

const images::image<ISPC_image_T>* ISPC_image::get()
{
    VERBOSE_DEBUG("raster 0x%p (curr %d)\n", frames[curr].raster, curr);
    return frames[curr].raster;
}

ISPC_image_T ISPC_image::BlackPoint()
{
    VERBOSE_DEBUG("black %f (curr %d)\n", frames[curr].black, curr);
    return frames[curr].black;
}

ISPC_image_T ISPC_image::WhitePoint()
{
    VERBOSE_DEBUG("white %f (curr %d)\n", frames[curr].white, curr);
    return frames[curr].white;
}

float ISPC_image::shutter()
{
    VERBOSE_DEBUG("shutter %f (curr %d)\n", frames[curr].exposureMS, curr);
    return frames[curr].exposureMS;
}

float ISPC_image::iso()
{
    VERBOSE_DEBUG("iso %f (curr %d)\n", frames[curr].gain*100, curr);
    return frames[curr].gain*100;
}

#endif

bool HDRLIBS_Supported(void)
{
#if defined(HDRLIBS_SEQHDR)
    return true;
#else
    return false;
#endif
}

IMG_RESULT HDRLIBS_MergeFrames(const HDRMergeInput &frame0,
    const HDRMergeInput &frame1, CI_BUFFER *pHDRBuffer)
{
#if !defined(HDRLIBS_SEQHDR)
    return IMG_ERROR_DISABLED;
#else

    IMG_UINT16 *pBuffData = (IMG_UINT16*)pHDRBuffer->data;
    IMG_INT32 neededSize = 0;
    struct CI_SIZEINFO sizeInfo;
    PIXELTYPE pixelType;
    IMG_RESULT ret;
        
    LOG_DEBUG("using HDRLIBS frame merge\n");
    if (frame0.buffer.width != frame1.buffer.width
        || frame0.buffer.height != frame1.buffer.height
        || frame0.buffer.stride != frame1.buffer.stride 
        || frame0.buffer.pxlFormat != frame1.buffer.pxlFormat)
    {
        LOG_ERROR("frame0 and frame1 are not from the same source\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (frame0.buffer.isTiled || frame1.buffer.isTiled)
    {
        LOG_ERROR("algorithm does not support tiled input\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (frame0.buffer.pxlFormat != BGR_101010_32)
    {
        LOG_ERROR("frame0 and frame1 format is not BGR 10b 32\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (!pHDRBuffer)
    {
        LOG_ERROR("pHDRBuffer is NULL!\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    
    ret = PixelTransformRGB(&pixelType, BGR_161616_64);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get information about %s",
            FormatString(BGR_161616_64));
        return IMG_ERROR_FATAL;
    }

    ret = CI_ALLOC_RGBSizeInfo(&pixelType, frame0.buffer.width,
        frame0.buffer.height, NULL, &sizeInfo);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get allocation information about %s for "\
            "image size %dx%d",
            FormatString(BGR_161616_64), frame0.buffer.width,
            frame0.buffer.height);
        return IMG_ERROR_FATAL;
    }
    
    neededSize = sizeInfo.ui32Stride*sizeInfo.ui32Height;
    
    if ((IMG_UINT32)neededSize > pHDRBuffer->ui32Size)
    {
        LOG_ERROR("a buffer of %u Bytes is too small - %u needed\n",
            pHDRBuffer->ui32Size, neededSize);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    
    
    ISPC_image source;
    util::timer s_timer, s_TotalTimer;
    
    s_timer.reset();
    
    // black and white not used yet
    LOG_INFO("Converting input frames...\n");
    const unsigned int maxPixel = (1<<10)-1;
    const unsigned int white = (unsigned int)floor(WHITE_CLIP*maxPixel),
        black = (unsigned int)floor(BLACK_CLIP*maxPixel);
    //const unsigned int white = 819, black = 64;
    
    VERBOSE_DEBUG("white=%u black=%u maxPixel=%u\n",
        white, black, maxPixel);
        
    ret = source.addBuffer(frame0.buffer, frame0.exposure, (float)frame0.gain,
        white, black);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to convert 1st buffer\n");
        return IMG_ERROR_FATAL;
    }
    
#if defined(SAVE_CONV)
    {
        LOG_INFO("saving tmp frame %d\n", source.idx());
        // debug image conversion
        ret = ISPC_image::convert(*source.get(), *pHDRBuffer, sizeInfo.ui32Stride);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to convert to the HDR input format!\n");
            return IMG_ERROR_FATAL;
        }
        
        ISPC::Save::SingleHDF(frame0.context, *pHDRBuffer, "tmp0.flx");
    }
#endif
    
    ret = source.addBuffer(frame1.buffer, frame1.exposure, (float)frame1.gain,
        white, black);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to convert 2nd buffer\n");
        return IMG_ERROR_FATAL;
    }
    
#if defined(SAVE_CONV)
    {
        source.next();
        LOG_INFO("saving tmp frame %d\n", source.idx());
        // debug image conversion
        ret = ISPC_image::convert(*source.get(), *pHDRBuffer, sizeInfo.ui32Stride);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to convert to the HDR input format!\n");
            return IMG_ERROR_FATAL;
        }
        
        ISPC::Save::SingleHDF(frame0.context, *pHDRBuffer, "tmp1.flx");
        
        source.next(); // move back to 0
    }
#endif
    
    LOG_INFO("Input images converted in %.3f s\n", s_timer.time());
    LOG_INFO("Run frame merging...\n");
    s_timer.reset();
    
    // input of algorithm - provided by writers of HDRLibs
    float f_MotionThresh = 3.f; // manually tuned Canon 500D. (last: 10)
    float a_NoiseDC [3] = {0.00593196f, 0.00432549f, 0.00471119f};
    std::vector <unsigned char> s_FiltProfile;
    s_FiltProfile.push_back (1);                        // ISO < 100.   
    s_FiltProfile.push_back (2);                        // ISO 100.
    s_FiltProfile.insert (s_FiltProfile.end (), 6, 2);  // ISO 200 - 400.
    s_FiltProfile.insert (s_FiltProfile.end (), 25, 2); // ISO 800 - 3200.
    
    // output of algorithm
    float f_black, f_white;
    images::image <float> outputHDR;
    std::ostream *p_verb = 0;
#if !defined(N_DEBUG) || 1
    p_verb = &std::cout;
#endif
    
    source.next(); //cheat to go back to image 0
    
    // run the algorithm
    bool mergeRes =
        seqHDR::HDR_DecreasingExposure_raw(outputHDR, f_black, f_white,
            source, a_NoiseDC, s_FiltProfile, f_MotionThresh, NULL, 3,
            p_verb);
    if (!mergeRes)
    {
        LOG_ERROR("Failed to merge with sequential HDR function\n");
        return IMG_ERROR_FATAL;
    }
    
    LOG_INFO("Merge done in %.3f s (white=%.4f black=%.4f)\n",
        s_timer.time(), f_white, f_black);
    LOG_INFO("Converting output (including memset)...\n");
    s_timer.reset();
    
    IMG_MEMSET(pHDRBuffer->data, 0xAA, pHDRBuffer->ui32Size);
    
    ret = ISPC_image::convert(outputHDR, *pHDRBuffer, sizeInfo.ui32Stride);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to convert to the HDR input format!\n");
        return IMG_ERROR_FATAL;
    }
    
    LOG_INFO("Output conversion done in %.3f s\n", s_timer.time());
    s_timer.reset();
    
    LOG_INFO("Total HDR merging done in %.3f s\n", s_TotalTimer.time());

    return IMG_SUCCESS;
#endif // HDRLIBS_SEQHDR
}
