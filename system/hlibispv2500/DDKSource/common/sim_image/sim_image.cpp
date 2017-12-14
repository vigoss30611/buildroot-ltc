/**
*******************************************************************************
@file sim_image.cpp

@brief CImage C interface for FLX format

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
#include "sim_image.h"

#include "image.h"

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <felixcommon/userlog.h>
#define LOG_TAG "SimImage"

#include <cstdio>
#include <cstdlib>

/*IMG_RESULT SimImageIn_create(sSimImageIn **ppSimImage)
{
    if ( ppSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if ( *ppSimImage != NULL )
    {
        return IMG_ERROR_MEMORY_IN_USE;
    }

    *ppSimImage = (sSimImageIn*)IMG_CALLOC(1, sizeof(sSimImageIn));
    if ( *ppSimImage == NULL ) return IMG_ERROR_MALLOC_FAILED;

    // for the moment init only sets 0 - done in calloc
    //return SimImageIn_init(*ppSimImage);
    return IMG_SUCCESS;
}*/

IMG_RESULT SimImageIn_init(sSimImageIn *pSimImage)
{
    if ( pSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_MEMSET(pSimImage, 0, sizeof(sSimImageIn));
    /*
    pSimImage->ui8BitDepth = 0;
    pSimImage->ui32Width = 0;
    pSimImage->ui32Height = 0;
    pSimImage->stride = 0;
    pSimImage->pBuffer = NULL;
    pSimImage->isBGR = IMG_FALSE;
    */
    return IMG_SUCCESS;
}

IMG_RESULT SimImageIn_clean(sSimImageIn *pSimImage)
{
    if ( pSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pSimImage->pBuffer != NULL )
    {
        IMG_FREE(pSimImage->pBuffer);
        //pSimImage->pBuffer = NULL; // in SimImageIn_init()
    }

    return SimImageIn_init(pSimImage);
}

static IMG_RESULT SimImageIn_ConvertBayer(const CImageBase::COLCHANNEL &c1,
    const CImageBase::COLCHANNEL &c2, const CImageBase::COLCHANNEL &c3,
    const CImageBase::COLCHANNEL &c4, sSimImageIn *pSimImage)
{
    IMG_UINT32 simI = 0;
    IMG_SIZE simImageSize = 0;

    IMG_ASSERT(pSimImage != NULL);
    IMG_ASSERT(pSimImage->pBuffer != NULL);
    
    // 4 channels
    simImageSize = pSimImage->info.ui32Height*pSimImage->info.stride; 
    IMG_ASSERT(c1.chnlWidth==c2.chnlWidth && c1.chnlHeight==c2.chnlHeight);
    IMG_ASSERT(c3.chnlWidth==c4.chnlWidth && c3.chnlHeight==c4.chnlHeight);
    IMG_ASSERT(c3.chnlHeight==c1.chnlHeight);

    for ( int y = 0 ; y < c1.chnlHeight ; y++ )
    {
        IMG_UINT16 a;
        for ( int x = 0 ; x < c1.chnlWidth ; x++ )
        {
            a=c1.data[y*c1.chnlWidth+x];
            pSimImage->pBuffer[simI] = a;
            simI++;
            a=c2.data[y*c2.chnlWidth+x];
            pSimImage->pBuffer[simI] = a;
            simI++;
        }
        for ( int x = 0 ; x < c3.chnlWidth ; x++ )
        {
            a=c3.data[y*c3.chnlWidth+x];
            pSimImage->pBuffer[simI] = a;
            simI++;
            a=c4.data[y*c4.chnlWidth+x];
            pSimImage->pBuffer[simI] = a;
            simI++;
        }
    }
    
    if ( simI*sizeof(pSimImage->pBuffer[0]) != simImageSize ) 
    {
        LOG_ERROR("last offset = %"IMG_SIZEPR"u in %"IMG_SIZEPR"uB "\
            "allocated\n", simI*sizeof(pSimImage->pBuffer[0]), simImageSize);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

static IMG_RESULT SimImageIn_ConvertRGB(const CImageBase::COLCHANNEL &c1,
    const CImageBase::COLCHANNEL &c2, const CImageBase::COLCHANNEL &c3,
    sSimImageIn *pSimImage)
{
    IMG_UINT32 simI = 0;
    IMG_SIZE simImageSize = 0;

    IMG_ASSERT(pSimImage != NULL);
    IMG_ASSERT(pSimImage->pBuffer != NULL);
    
    // 4 channels
    simImageSize = pSimImage->info.ui32Height*pSimImage->info.stride; 
    IMG_ASSERT(c1.chnlWidth == c2.chnlWidth && c1.chnlHeight == c2.chnlHeight);
    IMG_ASSERT(c2.chnlWidth == c3.chnlWidth && c2.chnlHeight == c3.chnlHeight);

    for (int y = 0; y < c1.chnlHeight; y++)
    {
        IMG_UINT16 a;
        for (int x = 0; x < c1.chnlWidth; x++)
        {
            a = c1.data[y*c1.chnlWidth + x];
            pSimImage->pBuffer[simI] = a;
            simI++;

            a = c2.data[y*c2.chnlWidth + x];
            pSimImage->pBuffer[simI] = a;
            simI++;

            a = c3.data[y*c3.chnlWidth + x];
            pSimImage->pBuffer[simI] = a;
            simI++;

            a = 0; // alpha
            pSimImage->pBuffer[simI] = a;
            simI++;
        }
    }

    if (simI*sizeof(pSimImage->pBuffer[0]) != simImageSize)
    {
        LOG_ERROR("last offset = %"IMG_SIZEPR"u in %"IMG_SIZEPR"uB " \
            "allocated\n", simI*sizeof(pSimImage->pBuffer[0]), simImageSize);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT SimImageIn_loadFLX(const char *pszFilename,
    sSimImageIn *pSimImage, IMG_INT32 i32frameIndex)
{
    CImageFlx flxLoader;
    CMetaData *pMetaData = NULL;
    const char *err;
    IMG_RESULT ret = IMG_SUCCESS;

    if ( pszFilename == NULL || pSimImage == NULL || i32frameIndex<0 )
    {
        LOG_ERROR("filename or pSimImage is NULL or i32frameIndex is "\
            "lower than 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if ( pSimImage->pBuffer != NULL )
    {
        SimImageIn_clean(pSimImage);
    }

    if ( (err = flxLoader.LoadFileHeader(pszFilename, NULL)) != NULL )
    {
        LOG_ERROR("failed to load FLX header from '%s': %s\n", pszFilename,
            err);
        return IMG_ERROR_FATAL;
    }

    // for segment 0 because segment is never changed
    pSimImage->nFrames = flxLoader.GetNFrames(0); 

    if ( (err = flxLoader.LoadFileData(i32frameIndex)) != NULL )
    {
        LOG_ERROR("failed to load FLX data from '%s'-frame %d: %s\n",
            pszFilename, i32frameIndex, err);
        return IMG_ERROR_FATAL;
    }

    // now we have data...
    pMetaData = flxLoader.GetMetaForFrame(i32frameIndex);

    if ( pMetaData == NULL )
    {
        LOG_ERROR("could not load meta data for frame %d in file '%s'\n",
            i32frameIndex, pszFilename);
        return IMG_ERROR_FATAL;
    }

    if (flxLoader.colorModel == CImageBase::CM_RGGB
        || flxLoader.colorModel == CImageBase::CM_RGB)
    {
        // it is NULL if it is empty from beginning or was clean before
        if (pSimImage->pBuffer == NULL)
        {
            int n = 4;
            int a = pMetaData->GetMetaInt(FLXMETA_WIDTH);
            int b = pMetaData->GetMetaInt(FLXMETA_SUBS_H, 1);
            pSimImage->info.ui32Width = (IMG_UINT32)a / b;
            if (a%b != 0)
            {
                pSimImage->info.ui32Width++;
            }

            a = pMetaData->GetMetaInt(FLXMETA_HEIGHT);
            b = pMetaData->GetMetaInt(FLXMETA_SUBS_V, 1);
        //pSimImage->info.ui32Height = pMetaData->GetMetaInt(FLXMETA_HEIGHT);
            pSimImage->info.ui32Height = a / b;
            if (a%b != 0)
            {
                pSimImage->info.ui32Height++;
            }
            pSimImage->info.ui8BitDepth = flxLoader.chnl[0].bitDepth;
            
            if (flxLoader.colorModel == CImageBase::CM_RGB)
            {
                n = 3;
            }

            for (int i = 1; i < n; i++)
            {
                if (pSimImage->info.ui8BitDepth != flxLoader.chnl[i].bitDepth)
                {
                    LOG_ERROR("can only load FLX files with same bit depth "\
                        "for all channels\n");
                    return IMG_ERROR_NOT_SUPPORTED;
                }
            }

            // this does not work when image is group-packed 
            /*pSimImage->info.stride = pMetaData->GetMetaInt("FRAME_SIZE") 
             *   / pSimImage->info.ui32Height;*/
            pSimImage->info.stride = pSimImage->info.ui32Width
                * 4 * sizeof(IMG_UINT16);
            pSimImage->pBuffer = (IMG_UINT16*)IMG_MALLOC(
                pSimImage->info.stride*pSimImage->info.ui32Height);

            //pSimImage->info.ui32Width = pMetaData->GetMetaInt(FLXMETA_WIDTH);
        }

        if (flxLoader.colorModel == CImageBase::CM_RGGB)
        {
            // the CImage always has channels as RGGB, reorder for conversion
            switch (flxLoader.subsMode)
            {
            case CImageBase::SUBS_RGGB:
                pSimImage->info.eColourModel = SimImage_RGGB;
                ret = SimImageIn_ConvertBayer(flxLoader.chnl[0],
                    flxLoader.chnl[1], flxLoader.chnl[2],
                    flxLoader.chnl[3], pSimImage);
                break;
            case CImageBase::SUBS_GRBG:
                pSimImage->info.eColourModel = SimImage_GRBG;
                ret = SimImageIn_ConvertBayer(flxLoader.chnl[1],
                    flxLoader.chnl[0], flxLoader.chnl[3], flxLoader.chnl[2],
                    pSimImage);
                break;
            case CImageBase::SUBS_GBRG:
                pSimImage->info.eColourModel = SimImage_GBRG;
                ret = SimImageIn_ConvertBayer(flxLoader.chnl[2],
                    flxLoader.chnl[3], flxLoader.chnl[0], flxLoader.chnl[1],
                    pSimImage);
                break;
            case CImageBase::SUBS_BGGR:
                pSimImage->info.eColourModel = SimImage_BGGR;
                ret = SimImageIn_ConvertBayer(flxLoader.chnl[3],
                    flxLoader.chnl[2], flxLoader.chnl[1], flxLoader.chnl[0],
                    pSimImage);
                break;
            case CImageBase::SUBS_444:
            case CImageBase::SUBS_422:
            case CImageBase::SUBS_420:
            case CImageBase::SUBS_UNDEF:
                LOG_ERROR("Mode not supported!\n");
                return IMG_ERROR_NOT_SUPPORTED;
            }

            pSimImage->info.ui32Width *= 2;
            pSimImage->info.ui32Height *= 2;
        }
        else if (flxLoader.colorModel == CImageBase::CM_RGB)// rgb
        {
            pSimImage->info.eColourModel = SimImage_RGB64;
            pSimImage->info.isBGR = IMG_TRUE;
            ret = SimImageIn_ConvertRGB(flxLoader.chnl[0], flxLoader.chnl[1],
                flxLoader.chnl[2], pSimImage);
        }
        else
        {
            LOG_ERROR("not supported\n");
            return IMG_ERROR_FATAL;
        }

    }
    else
    {
        fprintf(stderr, "can only load Bayer FLX, RGB or RGBA files\n");
        ret = IMG_ERROR_NOT_SUPPORTED;
    }
    return ret;
}

IMG_RESULT SimImageOut_init(sSimImageOut *pSimImage)
{
    if ( pSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_MEMSET(pSimImage, 0, sizeof(sSimImageOut));

    return IMG_SUCCESS;
}

IMG_RESULT SimImageOut_create(sSimImageOut *pSimImage)
{
    CImageFlx *CImage = NULL;
    CImageFlx::FLXSAVEFORMAT *saveFmt = NULL;
    
    CImageBase::CM_xxx colourModel;
    CImageBase::SUBS_xxx subModel;
    IMG_INT8 bitdepth[4] = { 0, 0, 0, 0 };
    IMG_RESULT ret = IMG_SUCCESS;
    const char *err = NULL;

    if ( pSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    
    switch(pSimImage->info.eColourModel)
    {
    case SimImage_RGB24:
    case SimImage_RGB32:
    case SimImage_RGB64:
        colourModel = CImageBase::CM_RGB;
        subModel = CImageBase::SUBS_UNDEF;
        pSimImage->nPlanes = 3;
        break;

    case SimImage_RGGB:
        colourModel = CImageBase::CM_RGGB;
        subModel = CImageBase::SUBS_RGGB;
        pSimImage->nPlanes = 4;
        break;
    case SimImage_GRBG:
        colourModel = CImageBase::CM_RGGB;
        subModel = CImageBase::SUBS_GRBG;
        pSimImage->nPlanes = 4;
        break;
    case SimImage_GBRG:
        colourModel = CImageBase::CM_RGGB;
        subModel = CImageBase::SUBS_GBRG;
        pSimImage->nPlanes = 4;
        break;
    case SimImage_BGGR:
        colourModel = CImageBase::CM_RGGB;
        subModel = CImageBase::SUBS_BGGR;
        pSimImage->nPlanes = 4;
        break;

    default:
        LOG_ERROR("unsupported format\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    for (int i = 0 ; i < pSimImage->nPlanes ; i++ )
    {
        bitdepth[i] = pSimImage->info.ui8BitDepth;
    }

    if ( pSimImage->nPlanes > MAX_COLOR_CHANNELS )
    {
        LOG_ERROR("nPlanes %d not supported(max %d)\n", pSimImage->nPlanes,
            MAX_COLOR_CHANNELS);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    CImage = new CImageFlx();
    if ( CImage == NULL )
    {
        ret = IMG_ERROR_MALLOC_FAILED;
        goto CImageFLXCreate_failure;
    }

    saveFmt = new CImageFlx::FLXSAVEFORMAT();
    if ( saveFmt == NULL )
    {
        ret = IMG_ERROR_MALLOC_FAILED;
        goto CImageFLXCreate_failure;
    }

    if (! CImage->CreateNewImage(pSimImage->info.ui32Width,
        pSimImage->info.ui32Height, colourModel, subModel, bitdepth, NULL) )
    {
        ret = IMG_ERROR_FATAL;
        goto CImageFLXCreate_failure;
    }
    
    // pixelFormatParam is not used
    saveFmt->pixelFormat = CImageFlx::PACKM_UNPACKEDR; 

    for ( int p = 0 ; p < pSimImage->nPlanes ; p++ )
    {
        saveFmt->planeFormat[p] = 1; // 1 plane per FLX plane
    }
    //saveFmt->planeFormat = &(pSimImage->nPlanes);
    // other stuff?

    // call SimImageOut_open() to write the head

    pSimImage->saveToFLX = (void*)CImage;
    pSimImage->saveFormat = (void*)saveFmt;
    pSimImage->saveContext = NULL;
    
    return IMG_SUCCESS;

CImageFLXCreate_failure:
    if ( saveFmt )
    {
        delete saveFmt;
    }
    if ( CImage )
    {
        delete CImage;
    }

    return ret;
}

IMG_RESULT SimImageOut_open(sSimImageOut *pSimImage, const char *pszFilename)
{
    void *saveCtx = NULL;
    CImageFlx::FLXSAVEFORMAT *saveFmt = NULL;
    CImageFlx *CImage = NULL;
    const char *err = NULL;
    IMG_RESULT ret = IMG_SUCCESS;
    
    if ( pSimImage->saveToFLX == NULL || pSimImage->saveFormat == NULL )
    {
        LOG_ERROR("file should have been created beforehand\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    saveFmt = (CImageFlx::FLXSAVEFORMAT*)pSimImage->saveFormat;
    CImage = (CImageFlx*)pSimImage->saveToFLX;

    err = CImage->SaveFileStart(pszFilename, saveFmt, &(saveCtx));
    if ( err != NULL )
    {
        ret = IMG_ERROR_FATAL;
        saveCtx = NULL;
        LOG_ERROR("CImageFlx::SaveFilestart %s\n", err);
        goto CImageFLXOpen_failure;
    }

    err = CImage->StartSegment(saveCtx, true, true);
    if ( err != NULL )
    {
        ret = IMG_ERROR_FATAL;
        LOG_ERROR("CImageFlx::StartSegment %s\n", err);
        goto CImageFLXOpen_failure;
    }
    
    err = CImage->SaveFileHeader(saveCtx);
    if ( err != NULL )
    {
        ret = IMG_ERROR_FATAL;
        LOG_ERROR("CImageFlx::SaveFileHeader %s\n", err);
        goto CImageFLXOpen_failure;
    }

    // call SaveFileData() for each frame
    // call SaveFileEnd() when finished

    pSimImage->saveContext = (void*)saveCtx;
    
    return IMG_SUCCESS;

CImageFLXOpen_failure:
    if ( CImage && saveCtx )
    {
        err = CImage->SaveFileEnd(saveCtx);
        if ( err != NULL )
        {
            LOG_ERROR("CImageFlx::SaveFileEnd %s\n", err);
        }
    }

    return ret;
}

IMG_RESULT SimImageOut_addFrame(sSimImageOut *pSimImage, const void *pData,
    IMG_SIZE size)
{
    CImageFlx *CImage = NULL;
    const char *err = NULL;
    IMG_UINT32 nChannels = 0;
    IMG_SIZE sizeofchannel = 0;
    IMG_SIZE toCopy = 0;
    unsigned int c;

    if ( pSimImage == NULL || pData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = (CImageFlx*)pSimImage->saveToFLX;

    if ( CImage == NULL )
    {
        LOG_ERROR("Image not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    nChannels = CImage->GetNColChannels();
    sizeofchannel = (pSimImage->info.ui8BitDepth)/8;
    if ( pSimImage->info.ui8BitDepth%8 != 0 )
    {
        sizeofchannel++;
    }

    if ( pSimImage->info.eColourModel == SimImage_RGB32
        || pSimImage->info.eColourModel == SimImage_RGB24
        || pSimImage->info.eColourModel == SimImage_RGB64 )
    {
        if ( nChannels != 3 )
        {
            LOG_ERROR("unexpected number of channels\n");
            return IMG_ERROR_FATAL;
        }
    }
    else if ( pSimImage->info.eColourModel == SimImage_RGGB
        || pSimImage->info.eColourModel == SimImage_GRBG
        || pSimImage->info.eColourModel == SimImage_GBRG
        || pSimImage->info.eColourModel == SimImage_BGGR )
    {
        if ( nChannels != 4 )
        {
            LOG_ERROR("unexpected number of channels\n");
            return IMG_ERROR_FATAL;
        }
    }
    else
    {
        LOG_ERROR("unsupported format\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // for 32b RGB8 and RGB10
    if ( pSimImage->info.eColourModel == SimImage_RGB32
        || pSimImage->info.eColourModel == SimImage_RGB64 ) 
    {
        // C bytes per pixel, N channels
        if ( (3*size/4)%(nChannels*sizeofchannel) != 0 )
        {
            LOG_ERROR("unexpected size\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }

        /* there is in fact 4 channels in input for rgb8 in 4 Bytes so we
         * need to copy a bit less*/
        toCopy = (3*size/4)/(nChannels);
        if ( pSimImage->info.ui8BitDepth > 8 )
        {
            toCopy = pSimImage->info.ui32Width*pSimImage->info.ui32Height;
        }
    }
    else if ( pSimImage->info.eColourModel == SimImage_RGB24 )
    {
        if ( size%(nChannels) != 0 )
        {
            LOG_ERROR("unexpected size\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }
        
        toCopy = size/nChannels;
    }
    else // all bayer
    {
        int div = 2; // division of nb bytes per data
        if ( pSimImage->info.ui8BitDepth == 8 )
        {
            // the division does not apply for 8b - it is only 1 byte per data
            div = 1; 
        }
        // C bytes per pixel, N channels
        if ( size%(nChannels*div) != 0 )
        {
            return IMG_ERROR_NOT_SUPPORTED;
        }
        
        toCopy = size/(nChannels*div); // was nChannels*2 for bayer
    }
    
    for ( c = 0 ; c < nChannels ; c++ )
    {
        // ugly cast for VCC
        if ( (IMG_SIZE)(CImage->chnl[c].chnlWidth*CImage->chnl[c].chnlHeight)
            < toCopy )
        {
            LOG_ERROR("trying to copy more (%" IMG_SIZEPR "d) than the "\
                "channel %d can cope with (%d)\n", toCopy, c,
                CImage->chnl[c].chnlWidth*CImage->chnl[c].chnlHeight);
            return IMG_ERROR_FATAL;
        }
    }

    if ( sizeofchannel == 1 ) // RGB 8b per channel or BAYER8
    {
        IMG_UINT8 *data = (IMG_UINT8*)pData;
        IMG_UINT8 nCopy = nChannels; // number of channels to copy

        if ( pSimImage->info.eColourModel == SimImage_RGB32 )
        {
            /* there are in fact 4 channels in input for rgb8 32b,
             * but only 3 are copied */
            nCopy = 4; 
        }

        for ( IMG_SIZE i = 0 ; i < toCopy ; i++ )
        {
            for ( c = 0 ; c < nChannels ; c++ )
            {
                CImage->chnl[c].data[i] = (IMG_UINT16)data[nCopy*i + c];
            }
        }
    }
    // sizeofchannel == 2 && Bayer format
    else if ( pSimImage->info.eColourModel != SimImage_RGB32
        && pSimImage->info.eColourModel != SimImage_RGB64
    ) 
    {
        IMG_UINT16 *data = (IMG_UINT16*)pData;

        for ( IMG_SIZE i = 0 ; i < toCopy ; i++ )
        {
            // could use nChannels because it can be RGB64 not only bayer
            for ( c = 0 ; c < nChannels ; c++ )
            {
                CImage->chnl[c].data[i] = data[nChannels*i + c];
            }
        }
    }
    // sizeofchannel == 2 && SimImage_RGB32
    else if (pSimImage->info.eColourModel == SimImage_RGB32)
    {
        IMG_UINT32 *data = (IMG_UINT32*)pData;
        IMG_UINT32 mask0 = 0x3FF;
        IMG_ASSERT(nChannels == 3);

        for ( IMG_SIZE i = 0 ; i < toCopy ; i++ )
        {
            // 10 = pSimImage->info.ui8BitDepth
            for ( c = 0 ; c < nChannels ; c++ ) 
            {
                CImage->chnl[c].data[i] =
                    (IMG_UINT16)((data[i]>>(c*10)) & mask0);
            }
        }
    }
    else if (pSimImage->info.eColourModel == SimImage_RGB64)
    {
        IMG_UINT16 *data = (IMG_UINT16*)pData;
        IMG_ASSERT(nChannels == 3);

        for ( IMG_SIZE i = 0 ; i < toCopy ; i++ )
        {
            // RGB64 input has 4 channels but only 3 should be saved
            for ( c = 0 ; c < nChannels ; c++ )
            {
                CImage->chnl[c].data[i] = data[4 * i + c];
            }
        }
    }
    else
    {
        LOG_ERROR("unsupported format!\n");
        return IMG_ERROR_FATAL;
    }

    /* if not RGGB the channels need swapping because the CImage class
     * expects R G G B channels */
    PIXEL *tmp;

    switch(pSimImage->info.eColourModel)
    {
    case SimImage_RGB24:
    case SimImage_RGB32:
    case SimImage_RGB64:
        // felix gives RGB but SimImage expects BGR
        if ( !pSimImage->info.isBGR )
        {
            tmp = CImage->chnl[0].data;
            CImage->chnl[0].data = CImage->chnl[2].data;
            CImage->chnl[2].data = tmp;
        }
        break;
    case SimImage_GRBG:
        tmp = CImage->chnl[0].data;
        CImage->chnl[0].data = CImage->chnl[1].data;
        CImage->chnl[1].data = tmp;
        tmp = CImage->chnl[2].data;
        CImage->chnl[2].data = CImage->chnl[3].data;
        CImage->chnl[3].data = tmp;
        break;
    case SimImage_GBRG:
        tmp = CImage->chnl[0].data;
        CImage->chnl[0].data = CImage->chnl[2].data;
        CImage->chnl[2].data = tmp;
        tmp = CImage->chnl[1].data;
        CImage->chnl[1].data = CImage->chnl[3].data;
        CImage->chnl[3].data = tmp;
        break;
    case SimImage_BGGR:
        tmp = CImage->chnl[0].data;
        CImage->chnl[0].data = CImage->chnl[3].data;
        CImage->chnl[3].data = tmp;
        tmp = CImage->chnl[1].data;
        CImage->chnl[1].data = CImage->chnl[2].data;
        CImage->chnl[2].data = tmp;
        break;
    case SimImage_RGGB:
        break; // no need to swap channels for RGGB
        
    default:
        LOG_ERROR("unsupported format!\n");
        return IMG_ERROR_FATAL;
    }
        
    return IMG_SUCCESS;
}

IMG_RESULT SimImageOut_write(sSimImageOut *pSimImage)
{
    const char *err = NULL;
    CImageFlx *CImage = NULL;

    if ( pSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = (CImageFlx*)pSimImage->saveToFLX;

    if ( CImage == NULL || pSimImage->saveContext == NULL )
    {
        return IMG_ERROR_NOT_INITIALISED;
    }
    
    err = CImage->SaveFileData(pSimImage->saveContext);
    if ( err != NULL )
    {
        LOG_ERROR("CImageFlx::SaveFileData %s\n", err);
        return IMG_ERROR_FATAL;
    }
    
    return IMG_SUCCESS;
}

IMG_RESULT SimImageOut_close(sSimImageOut *pSimImage)
{
    CImageFlx *CImage = NULL;
    CImageFlx::FLXSAVEFORMAT *saveFmt = NULL;
    const char *err = NULL;

    if ( pSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = (CImageFlx*)pSimImage->saveToFLX;
    saveFmt = (CImageFlx::FLXSAVEFORMAT*)pSimImage->saveFormat;

    if ( CImage == NULL || pSimImage->saveContext == NULL || saveFmt == NULL )
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    err = CImage->SaveFileEnd(pSimImage->saveContext);
    if ( err != NULL )
    {
        LOG_ERROR("CImageFlx::SaveFileEnd %s\n", err);
        return IMG_ERROR_FATAL;
    }

    delete CImage;
    delete saveFmt;

    pSimImage->saveToFLX = NULL;
    pSimImage->saveContext = NULL;
    pSimImage->saveFormat = NULL;

    return IMG_SUCCESS;
}

IMG_RESULT SimImageOut_clean(sSimImageOut *pSimImage)
{
    CImageFlx *CImage = NULL;
    CImageFlx::FLXSAVEFORMAT *saveFmt = NULL;

    if ( pSimImage == NULL )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = (CImageFlx*)pSimImage->saveToFLX;
    saveFmt = (CImageFlx::FLXSAVEFORMAT*)pSimImage->saveFormat;

    if ( CImage != NULL && pSimImage->saveContext != NULL && saveFmt != NULL )
    {
        // was not closed!
        LOG_ERROR("a sSimImageOut was not closed! closing...\n");
        SimImageOut_close(pSimImage);

        // get them again
        CImage = (CImageFlx*)pSimImage->saveToFLX;
        saveFmt = (CImageFlx::FLXSAVEFORMAT*)pSimImage->saveFormat;
    }

    if ( CImage != NULL )
    {
        // should be NULL! but delete it nonetheless
        delete CImage;
        pSimImage->saveToFLX = NULL;
    }
    if ( saveFmt != NULL )
    {
        // should be NULL! but delete it nonetheless
        delete saveFmt;
        pSimImage->saveFormat = NULL;
    }

    return SimImageOut_init(pSimImage);
}
