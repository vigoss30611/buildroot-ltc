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
    if (ppSimImage == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (*ppSimImage != NULL)
    {
        return IMG_ERROR_MEMORY_IN_USE;
    }

    *ppSimImage = (sSimImageIn*)IMG_CALLOC(1, sizeof(sSimImageIn));
    if (*ppSimImage == NULL)
    {
        return IMG_ERROR_MALLOC_FAILED;
    }

    // for the moment init only sets 0 - done in calloc
    // return SimImageIn_init(*ppSimImage);
    return IMG_SUCCESS;
}*/

IMG_RESULT SimImageIn_init(sSimImageIn *pSimImageIn)
{
    if (!pSimImageIn)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_MEMSET(pSimImageIn, 0, sizeof(sSimImageIn));
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

static IMG_RESULT SimImageIn_ConvertBayer(const CImageBase::COLCHANNEL &c1,
    const CImageBase::COLCHANNEL &c2, const CImageBase::COLCHANNEL &c3,
    const CImageBase::COLCHANNEL &c4, sSimImageIn *pSimImage)
{
    IMG_UINT32 simI = 0;
    IMG_SIZE simImageSize = 0;
    int y, x;

    IMG_ASSERT(pSimImage != NULL);
    IMG_ASSERT(pSimImage->pBuffer != NULL);

    // 4 channels, but size is multiplied by 2 to display "interpolated" size
    simImageSize = pSimImage->info.ui32Height*pSimImage->info.stride/2;
    IMG_ASSERT(c1.chnlWidth == c2.chnlWidth
        && c1.chnlHeight == c2.chnlHeight);
    IMG_ASSERT(c3.chnlWidth == c4.chnlWidth
        && c3.chnlHeight == c4.chnlHeight);
    IMG_ASSERT(c3.chnlHeight == c1.chnlHeight);

    for (y = 0; y < c1.chnlHeight; y++)
    {
        IMG_UINT16 a;
        for (x = 0; x < c1.chnlWidth; x++)
        {
            a = c1.data[y*c1.chnlWidth + x];
            pSimImage->pBuffer[simI] = a;
            simI++;
            a = c2.data[y*c2.chnlWidth + x];
            pSimImage->pBuffer[simI] = a;
            simI++;
        }
        for (x = 0; x < c3.chnlWidth; x++)
        {
            a = c3.data[y*c3.chnlWidth + x];
            pSimImage->pBuffer[simI] = a;
            simI++;
            a = c4.data[y*c4.chnlWidth + x];
            pSimImage->pBuffer[simI] = a;
            simI++;
        }
    }

    if (simI*sizeof(pSimImage->pBuffer[0]) != simImageSize)
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
    int x, y;

    IMG_ASSERT(pSimImage != NULL);
    IMG_ASSERT(pSimImage->pBuffer != NULL);

    // 4 channels
    simImageSize = pSimImage->info.ui32Height*pSimImage->info.stride;
    IMG_ASSERT(c1.chnlWidth == c2.chnlWidth
        && c1.chnlHeight == c2.chnlHeight);
    IMG_ASSERT(c2.chnlWidth == c3.chnlWidth
        && c2.chnlHeight == c3.chnlHeight);

    for (y = 0; y < c1.chnlHeight; y++)
    {
        IMG_UINT16 a;
        for (x = 0; x < c1.chnlWidth; x++)
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

            a = 0;  // alpha
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

bool operator!=(CMetaData &a, CMetaData &b)
{
    bool res = false;
    int i, nC = 0;

    res |= a.GetMetaInt(FLXMETA_WIDTH) != b.GetMetaInt(FLXMETA_WIDTH);
    res |= a.GetMetaInt(FLXMETA_HEIGHT) != b.GetMetaInt(FLXMETA_HEIGHT);
    res |= a.GetMetaInt(FLXMETA_COLOR_FORMAT)
        != b.GetMetaInt(FLXMETA_COLOR_FORMAT);

    if (0 == strncmp("RGGB", a.GetMetaStr(FLXMETA_COLOR_FORMAT), 4))
    {
        nC = 4;
    }
    else if (0 == strncmp("RGB", a.GetMetaStr(FLXMETA_COLOR_FORMAT), 3))
    {
        nC = 3;
    }

    res |= 0 == nC;

    if (res)
    {
        // no point checking for each plane
        return res;
    }

    for (i = 0; i < nC && !res; i++)
    {
        res |= a.GetMetaInt(FLXMETA_SUBS_H, 1, i)
            != b.GetMetaInt(FLXMETA_SUBS_H, 1, i);
        res |= a.GetMetaInt(FLXMETA_SUBS_V, 1, i)
            != b.GetMetaInt(FLXMETA_SUBS_V, 1, i);
        res |= a.GetMetaInt(FLXMETA_PHASE_OFF_H, 1, i)
            != b.GetMetaInt(FLXMETA_PHASE_OFF_H, 1, i);
        res |= a.GetMetaInt(FLXMETA_PHASE_OFF_V, 1, i)
            != b.GetMetaInt(FLXMETA_PHASE_OFF_V, 1, i);
        res |= a.GetMetaInt(FLXMETA_BITDEPTH, 8, i)
            != b.GetMetaInt(FLXMETA_BITDEPTH, 8, i);
    }

    return res;
}

/**
 * @brief If a file is multi-segment checks that all segments have the same
 * size and subsampling to allow multi-segment loading
 *
 * assumes file header is already loaded
 */
static IMG_RESULT checkMutiSegments(CImageFlx &flxLoader)
{
    CMetaData *pMetaDataOri = NULL;
    CMetaData *pMetaData = NULL;
    const char *err = NULL;
    int i = 0, nFrames = 0, tNFrames = 0;


    /*err = flxLoader.LoadFileHeader(pszFilename, NULL);
    if (err)
    {
        LOG_ERROR("failed to load FLX header from '%s': %s\n", pszFilename,
            err);
        return IMG_ERROR_FATAL;
    }*/

    tNFrames = flxLoader.GetNFrames(-1);

    pMetaDataOri = flxLoader.GetMetaForFrame(nFrames);
    if (!pMetaDataOri)
    {
        LOG_ERROR("could not load meta data for frame %d segment %d\n",
            nFrames, i);
        return IMG_ERROR_FATAL;
    }
    nFrames += flxLoader.GetNFrames(i);
    
    for (i = 1; i < flxLoader.GetNSegments() && nFrames < tNFrames; i++)
    {
        pMetaData = flxLoader.GetMetaForFrame(nFrames);
        if (!pMetaData)
        {
            LOG_ERROR("could not load meta data for frame %d segment %d\n",
                nFrames, i);
            return IMG_ERROR_FATAL;
        }

        if (*pMetaDataOri != *pMetaData)
        {
            LOG_DEBUG("segment %d is different from 1st segment!\n", i);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        nFrames += flxLoader.GetNFrames(i);
    }
    return IMG_SUCCESS;
}

IMG_RESULT SimImageIn_loadFLX(sSimImageIn *pSimImageIn,
    const char *pszFilename)
{
    CImageFlx *pFlxLoader = NULL;
    CMetaData *pMetaData = NULL;
    const char *err;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pszFilename || !pSimImageIn)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (pSimImageIn->pBuffer || pSimImageIn->loadFromFLX)
    {
        LOG_ERROR("object already has open image - close before "\
            "opening again\n");
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    pFlxLoader = new CImageFlx();
    if (!pFlxLoader)
    {
        LOG_ERROR("failed to allocate a new CImageFlx\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    err = pFlxLoader->LoadFileHeader(pszFilename, NULL);
    if (err)
    {
        LOG_ERROR("failed to load FLX header from '%s': %s\n", pszFilename,
            err);
        delete pFlxLoader;
        return IMG_ERROR_FATAL;
    }

    // for segment 0 because segment is never changed
    pSimImageIn->nFrames = pFlxLoader->GetNFrames(-1);
    if (pFlxLoader->GetNSegments() > 1)
    {
        // not very optimal...
        ret = checkMutiSegments(*pFlxLoader);
        if (ret)
        {
            LOG_WARNING("incompatible multi-segment image: load only from "\
                "1st segment\n");
            pSimImageIn->nFrames = pFlxLoader->GetNFrames(0);
        }
    }

    /* now we gather info... if multi-segment it has been verified to be the
     * same for all segments */
    err = pFlxLoader->LoadFileData(0);
    if (err)
    {
        LOG_ERROR("failed to load FLX data from '%s'-frame %d: %s\n",
            pszFilename, 0, err);
        delete pFlxLoader;
        return IMG_ERROR_FATAL;
    }

    pMetaData = pFlxLoader->GetMetaForFrame(0);
    if (!pMetaData)
    {
        LOG_ERROR("could not load meta data for frame %d in file '%s'\n",
            0, pszFilename);
        delete pFlxLoader;
        return IMG_ERROR_FATAL;
    }

    if (CImageBase::CM_RGGB == pFlxLoader->colorModel
        || CImageBase::CM_RGB == pFlxLoader->colorModel)
    {
        int n = 4;
        int a = pMetaData->GetMetaInt(FLXMETA_WIDTH);
        int b = pMetaData->GetMetaInt(FLXMETA_SUBS_H, 1);
        pSimImageIn->info.ui32Width = (IMG_UINT32)a / b;
        if (a%b != 0)
        {
            pSimImageIn->info.ui32Width++;
        }

        a = pMetaData->GetMetaInt(FLXMETA_HEIGHT);
        b = pMetaData->GetMetaInt(FLXMETA_SUBS_V, 1);
        // pSimImage->info.ui32Height =
        //     pMetaData->GetMetaInt(FLXMETA_HEIGHT);
        pSimImageIn->info.ui32Height = a / b;
        if (a%b != 0)
        {
            pSimImageIn->info.ui32Height++;
        }
        pSimImageIn->info.ui8BitDepth = pFlxLoader->chnl[0].bitDepth;

        if (CImageBase::CM_RGB == pFlxLoader->colorModel)
        {
            n = 3;
        }

        for (int i = 1; i < n; i++)
        {
            if (pSimImageIn->info.ui8BitDepth
                != pFlxLoader->chnl[i].bitDepth)
            {
                LOG_ERROR("can only load FLX files with same bit depth "\
                    "for all channels\n");
                delete pFlxLoader;
                return IMG_ERROR_NOT_SUPPORTED;
            }
        }

        // this does not work when image is group-packed
        pSimImageIn->info.stride = pSimImageIn->info.ui32Width
            * 4 * sizeof(IMG_UINT16);
        pSimImageIn->pBuffer = static_cast<IMG_UINT16*>(IMG_MALLOC(
            pSimImageIn->info.stride*pSimImageIn->info.ui32Height));

        if (!pSimImageIn->pBuffer)
        {
            LOG_ERROR("failed to allocate internal buffer\n");
            delete pFlxLoader;
            return IMG_ERROR_MALLOC_FAILED;
        }

        if (CImageBase::CM_RGGB == pFlxLoader->colorModel)
        {
            switch (pFlxLoader->subsMode)
            {
            case CImageBase::SUBS_RGGB:
                pSimImageIn->info.eColourModel = SimImage_RGGB;
                break;
            case CImageBase::SUBS_GRBG:
                pSimImageIn->info.eColourModel = SimImage_GRBG;
                break;
            case CImageBase::SUBS_GBRG:
                pSimImageIn->info.eColourModel = SimImage_GBRG;
                break;
            case CImageBase::SUBS_BGGR:
                pSimImageIn->info.eColourModel = SimImage_BGGR;
                break;
            case CImageBase::SUBS_444:
            case CImageBase::SUBS_422:
            case CImageBase::SUBS_420:
            case CImageBase::SUBS_UNDEF:
                LOG_ERROR("Mode not supported!\n");
                delete pFlxLoader;
                return IMG_ERROR_NOT_SUPPORTED;
            }

            pSimImageIn->info.ui32Width *= 2;
            pSimImageIn->info.ui32Height *= 2;
        }
        else if (CImageBase::CM_RGB == pFlxLoader->colorModel)  // rgb
        {
            pSimImageIn->info.eColourModel = SimImage_RGB64;
            pSimImageIn->info.isBGR = IMG_TRUE;
        }
        else
        {
            LOG_ERROR("not supported\n");
            delete pFlxLoader;
            return IMG_ERROR_FATAL;
        }
    }
    else
    {
        fprintf(stderr, "can only load Bayer FLX, RGB or RGBA files\n");
        delete pFlxLoader;
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pSimImageIn->loadFromFLX = static_cast<void *>(pFlxLoader);
    return IMG_SUCCESS;
}

IMG_RESULT SimImageIn_checkMultiSegment(const char *pszFilename,
    unsigned int *pNSegments, unsigned int *pNTotalFrames)
{
    CImageFlx flxLoader;
    CMetaData *pMetaData = NULL;
    const char *err;
    IMG_RESULT ret = IMG_SUCCESS;
    int nFrames = 0;
    int nSegments = 0;

    if (!pszFilename)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    err = flxLoader.LoadFileHeader(pszFilename, NULL);
    if (err)
    {
        LOG_ERROR("failed to load FLX header from '%s': %s\n", pszFilename,
            err);
        return IMG_ERROR_FATAL;
    }

    // for segment 0 because segment is never changed
    nFrames = flxLoader.GetNFrames(-1);
    nSegments = flxLoader.GetNSegments();
    if (nSegments > 0)
    {
        // not very optimal...
        ret = checkMutiSegments(flxLoader);
        if (ret)
        {
            LOG_WARNING("incompatible multi-segment image: load only from "\
                "1st segment\n");
        }
    }
    else
    {
        ret = IMG_SUCCESS;
    }
    if (pNSegments)
    {
        *pNSegments = nSegments;
    }
    if (pNTotalFrames)
    {
        *pNTotalFrames = nFrames;
    }
    return ret;
}

IMG_RESULT SimImageIn_convertFrame(sSimImageIn *pSimImageIn,
    IMG_INT32 i32frameIndex)
{
    CImageFlx *pFlxLoader = NULL;
    const char *err = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    if (!pSimImageIn || i32frameIndex < 0)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (!pSimImageIn->loadFromFLX || !pSimImageIn->pBuffer)
    {
        LOG_ERROR("file should be open before converting frame\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if ((unsigned int)i32frameIndex > pSimImageIn->nFrames)
    {
        LOG_ERROR("file only has %u frame(s), cannot convert frame %d\n",
            pSimImageIn->nFrames, i32frameIndex);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFlxLoader = static_cast<CImageFlx *>(pSimImageIn->loadFromFLX);

    err = pFlxLoader->LoadFileData(i32frameIndex);
    if (err)
    {
        LOG_ERROR("failed to load FLX data from frame %d: %s\n",
            i32frameIndex, err);
        return IMG_ERROR_FATAL;
    }

    if (CImageBase::CM_RGGB == pFlxLoader->colorModel)
    {
        int r = 0, g1 = 1, g2 = 2, b = 3;
        // the CImage always has channels as RGGB, reorder for conversion
        switch (pFlxLoader->subsMode)
        {
        case CImageBase::SUBS_RGGB:
            break;
        case CImageBase::SUBS_GRBG:
            r = 1; g1 = 0; g2 = 3; b = 2;
            break;
        case CImageBase::SUBS_GBRG:
            r = 2; g1 = 3; g2 = 0; b = 1;
            break;
        case CImageBase::SUBS_BGGR:
            r = 3; g1 = 2; g2 = 1; b = 0;
            break;
        case CImageBase::SUBS_444:
        case CImageBase::SUBS_422:
        case CImageBase::SUBS_420:
        case CImageBase::SUBS_UNDEF:
            // should have failed at loadFLX already!
            return IMG_ERROR_NOT_SUPPORTED;
        }
        ret = SimImageIn_ConvertBayer(pFlxLoader->chnl[r],
            pFlxLoader->chnl[g1], pFlxLoader->chnl[g2],
            pFlxLoader->chnl[b], pSimImageIn);
    }
    else if (CImageBase::CM_RGB == pFlxLoader->colorModel)  // rgb
    {
        ret = SimImageIn_ConvertRGB(pFlxLoader->chnl[0],
            pFlxLoader->chnl[1], pFlxLoader->chnl[2], pSimImageIn);
    }
    else
    {
        // should have failed at loadFLX already!
        ret = IMG_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

IMG_RESULT SimImageIn_close(sSimImageIn *pSimImage)
{
    CImageFlx *pFLXLoader = NULL;
    if (!pSimImage)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pFLXLoader = static_cast<CImageFlx *>(pSimImage->loadFromFLX);
    if (pFLXLoader)
    {
        delete pFLXLoader;
        pFLXLoader = NULL;  // done in init but safer here
    }

    if (pSimImage->pBuffer)
    {
        IMG_FREE(pSimImage->pBuffer);
        pSimImage->pBuffer = NULL;  // done in init but safer here
    }

    return SimImageIn_init(pSimImage);
}

IMG_RESULT SimImageOut_init(sSimImageOut *pSimImage)
{
    if (pSimImage == NULL)
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

    if (pSimImage == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    switch (pSimImage->info.eColourModel)
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

    for (int i = 0; i < pSimImage->nPlanes; i++)
    {
        bitdepth[i] = pSimImage->info.ui8BitDepth;
    }

    if (pSimImage->nPlanes > MAX_COLOR_CHANNELS)
    {
        LOG_ERROR("nPlanes %d not supported(max %d)\n", pSimImage->nPlanes,
            MAX_COLOR_CHANNELS);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    CImage = new CImageFlx();
    if (CImage == NULL)
    {
        ret = IMG_ERROR_MALLOC_FAILED;
        goto CImageFLXCreate_failure;
    }

    saveFmt = new CImageFlx::FLXSAVEFORMAT();
    if (saveFmt == NULL)
    {
        ret = IMG_ERROR_MALLOC_FAILED;
        goto CImageFLXCreate_failure;
    }

    if (!CImage->CreateNewImage(pSimImage->info.ui32Width,
        pSimImage->info.ui32Height, colourModel, subModel, bitdepth, NULL))
    {
        ret = IMG_ERROR_FATAL;
        goto CImageFLXCreate_failure;
    }

    // pixelFormatParam is not used
    saveFmt->pixelFormat = CImageFlx::PACKM_UNPACKEDR;

    for (int p = 0; p < pSimImage->nPlanes; p++)
    {
        saveFmt->planeFormat[p] = 1;  // 1 plane per FLX plane
    }
    // saveFmt->planeFormat = &(pSimImage->nPlanes);
    // other stuff?

    // call SimImageOut_open() to write the head

    pSimImage->saveToFLX = static_cast<void*>(CImage);
    pSimImage->saveFormat = static_cast<void*>(saveFmt);
    pSimImage->saveContext = NULL;

    return IMG_SUCCESS;

CImageFLXCreate_failure:
    if (saveFmt)
    {
        delete saveFmt;
    }
    if (CImage)
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

    if (pSimImage->saveToFLX == NULL || pSimImage->saveFormat == NULL)
    {
        LOG_ERROR("file should have been created beforehand\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    saveFmt = static_cast<CImageFlx::FLXSAVEFORMAT*>(pSimImage->saveFormat);
    CImage = static_cast<CImageFlx*>(pSimImage->saveToFLX);

    err = CImage->SaveFileStart(pszFilename, saveFmt, &(saveCtx));
    if (err)
    {
        ret = IMG_ERROR_FATAL;
        saveCtx = NULL;
        LOG_ERROR("CImageFlx::SaveFilestart %s\n", err);
        goto CImageFLXOpen_failure;
    }

    err = CImage->StartSegment(saveCtx, true, true);
    if (err)
    {
        ret = IMG_ERROR_FATAL;
        LOG_ERROR("CImageFlx::StartSegment %s\n", err);
        goto CImageFLXOpen_failure;
    }

    err = CImage->SaveFileHeader(saveCtx);
    if (err)
    {
        ret = IMG_ERROR_FATAL;
        LOG_ERROR("CImageFlx::SaveFileHeader %s\n", err);
        goto CImageFLXOpen_failure;
    }

    // call SaveFileData() for each frame
    // call SaveFileEnd() when finished

    pSimImage->saveContext = static_cast<void*>(saveCtx);

    return IMG_SUCCESS;

CImageFLXOpen_failure:
    if (CImage && saveCtx)
    {
        err = CImage->SaveFileEnd(saveCtx);
        if (err)
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

    if (pSimImage == NULL || pData == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = static_cast<CImageFlx*>(pSimImage->saveToFLX);

    if (CImage == NULL)
    {
        LOG_ERROR("Image not initialised\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    nChannels = CImage->GetNColChannels();
    sizeofchannel = (pSimImage->info.ui8BitDepth) / 8;
    if (pSimImage->info.ui8BitDepth % 8 != 0)
    {
        sizeofchannel++;
    }

    if (pSimImage->info.eColourModel == SimImage_RGB32
        || pSimImage->info.eColourModel == SimImage_RGB24
        || pSimImage->info.eColourModel == SimImage_RGB64)
    {
        if (3 != nChannels)
        {
            LOG_ERROR("unexpected number of channels\n");
            return IMG_ERROR_FATAL;
        }
    }
    else if (pSimImage->info.eColourModel == SimImage_RGGB
        || pSimImage->info.eColourModel == SimImage_GRBG
        || pSimImage->info.eColourModel == SimImage_GBRG
        || pSimImage->info.eColourModel == SimImage_BGGR)
    {
        if (4 != nChannels)
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
    if (pSimImage->info.eColourModel == SimImage_RGB32
        || pSimImage->info.eColourModel == SimImage_RGB64)
    {
        // C bytes per pixel, N channels
        if (0 != (3 * size / 4) % (nChannels*sizeofchannel))
        {
            LOG_ERROR("unexpected size\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }

        /* there is in fact 4 channels in input for rgb8 in 4 Bytes so we
         * need to copy a bit less*/
        toCopy = (3 * size / 4) / (nChannels);
        if (pSimImage->info.ui8BitDepth > 8)
        {
            toCopy = pSimImage->info.ui32Width*pSimImage->info.ui32Height;
        }
    }
    else if (pSimImage->info.eColourModel == SimImage_RGB24)
    {
        if (0 != size % (nChannels))
        {
            LOG_ERROR("unexpected size\n");
            return IMG_ERROR_NOT_SUPPORTED;
        }

        toCopy = size / nChannels;
    }
    else  // all bayer
    {
        int div = 2;  // division of nb bytes per data
        if (pSimImage->info.ui8BitDepth == 8)
        {
            // the division does not apply for 8b - it is only 1 byte per data
            div = 1;
        }
        // C bytes per pixel, N channels
        if (0 != size % (nChannels*div))
        {
            return IMG_ERROR_NOT_SUPPORTED;
        }

        toCopy = size / (nChannels*div); // was nChannels*2 for bayer
    }

    for (c = 0; c < nChannels; c++)
    {
        // ugly cast for VCC
        if ((IMG_SIZE)(CImage->chnl[c].chnlWidth*CImage->chnl[c].chnlHeight)
            < toCopy)
        {
            LOG_ERROR("trying to copy more (%" IMG_SIZEPR "d) than the "\
                "channel %d can cope with (%d)\n", toCopy, c,
                CImage->chnl[c].chnlWidth*CImage->chnl[c].chnlHeight);
            return IMG_ERROR_FATAL;
        }
    }

    if (1 == sizeofchannel)  // RGB 8b per channel or BAYER8
    {
        const IMG_UINT8 *data = static_cast<const IMG_UINT8*>(pData);
        IMG_UINT8 nCopy = nChannels; // number of channels to copy

        if (pSimImage->info.eColourModel == SimImage_RGB32)
        {
            /* there are in fact 4 channels in input for rgb8 32b,
             * but only 3 are copied */
            nCopy = 4;
        }

        for (IMG_SIZE i = 0; i < toCopy; i++)
        {
            for (c = 0; c < nChannels; c++)
            {
                CImage->chnl[c].data[i] = (IMG_UINT16)data[nCopy*i + c];
            }
        }
    }
    // sizeofchannel == 2 && Bayer format
    else if (pSimImage->info.eColourModel != SimImage_RGB32
        && pSimImage->info.eColourModel != SimImage_RGB64
        )
    {
        const IMG_UINT16 *data = static_cast<const IMG_UINT16*>(pData);

        for (IMG_SIZE i = 0; i < toCopy; i++)
        {
            // could use nChannels because it can be RGB64 not only bayer
            for (c = 0; c < nChannels; c++)
            {
                CImage->chnl[c].data[i] = data[nChannels*i + c];
            }
        }
    }
    // sizeofchannel == 2 && SimImage_RGB32
    else if (pSimImage->info.eColourModel == SimImage_RGB32)
    {
        const IMG_UINT32 *data = static_cast<const IMG_UINT32*>(pData);
        IMG_UINT32 mask0 = 0x3FF;
        IMG_ASSERT(nChannels == 3);

        for (IMG_SIZE i = 0; i < toCopy; i++)
        {
            // 10 = pSimImage->info.ui8BitDepth
            for (c = 0; c < nChannels; c++)
            {
                CImage->chnl[c].data[i] =
                    (IMG_UINT16)((data[i] >> (c * 10)) & mask0);
            }
        }
    }
    else if (pSimImage->info.eColourModel == SimImage_RGB64)
    {
        const IMG_UINT16 *data = static_cast<const IMG_UINT16*>(pData);
        IMG_ASSERT(nChannels == 3);

        for (IMG_SIZE i = 0; i < toCopy; i++)
        {
            // RGB64 input has 4 channels but only 3 should be saved
            for (c = 0; c < nChannels; c++)
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

    switch (pSimImage->info.eColourModel)
    {
    case SimImage_RGB24:
    case SimImage_RGB32:
    case SimImage_RGB64:
        // felix gives RGB but SimImage expects BGR
        if (!pSimImage->info.isBGR)
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
        break;  // no need to swap channels for RGGB

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

    if (pSimImage == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = static_cast<CImageFlx*>(pSimImage->saveToFLX);

    if (CImage == NULL || pSimImage->saveContext == NULL)
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    err = CImage->SaveFileData(pSimImage->saveContext);
    if (err)
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

    if (pSimImage == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = static_cast<CImageFlx*>(pSimImage->saveToFLX);
    saveFmt = static_cast<CImageFlx::FLXSAVEFORMAT*>(pSimImage->saveFormat);

    if (CImage == NULL || pSimImage->saveContext == NULL || saveFmt == NULL)
    {
        return IMG_ERROR_NOT_INITIALISED;
    }

    err = CImage->SaveFileEnd(pSimImage->saveContext);
    if (err)
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

    if (pSimImage == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    CImage = static_cast<CImageFlx*>(pSimImage->saveToFLX);
    saveFmt = static_cast<CImageFlx::FLXSAVEFORMAT*>(pSimImage->saveFormat);

    if (CImage != NULL && pSimImage->saveContext != NULL && saveFmt != NULL)
    {
        // was not closed!
        LOG_ERROR("a sSimImageOut was not closed! closing...\n");
        SimImageOut_close(pSimImage);

        // get them again
        CImage = static_cast<CImageFlx*>(pSimImage->saveToFLX);
        saveFmt =
            static_cast<CImageFlx::FLXSAVEFORMAT*>(pSimImage->saveFormat);
    }

    if (CImage != NULL)
    {
        // should be NULL! but delete it nonetheless
        delete CImage;
        pSimImage->saveToFLX = NULL;
    }
    if (saveFmt != NULL)
    {
        // should be NULL! but delete it nonetheless
        delete saveFmt;
        pSimImage->saveFormat = NULL;
    }

    return SimImageOut_init(pSimImage);
}
