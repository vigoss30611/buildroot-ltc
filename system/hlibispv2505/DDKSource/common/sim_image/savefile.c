/**
*******************************************************************************
@file savefile.c

@brief saving sim_image to file

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
#include "savefile.h"  // NOLINT

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <felixcommon/userlog.h>
#define LOG_TAG "Savefile"

#include <stdio.h>

#include "sim_image.h"  // NOLINT

#define DBG_SaveFile
// #define DBG_SaveFile LOG_DEBUG

// #define DEBUG_SAVEFILE

#ifdef SAVE_LOCKS
#include <pthread.h>
#endif

IMG_BOOL SaveFile_useSaveLocks()
{
#ifdef SAVE_LOCKS
    return IMG_TRUE;
#else
    return IMG_FALSE;
#endif
}

IMG_RESULT SaveFile_init(SaveFile *pFile)
{
    if (pFile == NULL)
    {
        LOG_ERROR("pFile is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    DBG_SaveFile("0x%p\n", pFile);

#ifdef SAVE_LOCKS
    pthread_mutex_init(&(pFile->fileLock), NULL);
    pFile->bInitiliasedLock = IMG_TRUE;
#endif
    pFile->saveTo = NULL;
    pFile->pSimImage = NULL;
    pFile->ui32TimeWritten = 0;

    return IMG_SUCCESS;
}

IMG_RESULT SaveFile_open(SaveFile *pFile, const char* filename)
{
    IMG_RESULT ret = IMG_SUCCESS;

    if (pFile == NULL || filename == NULL)
    {
        LOG_ERROR("pFile or filename is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (pFile->saveTo != NULL || pFile->pSimImage != NULL)
    {
        LOG_ERROR("pFile->saveTo or pFile->pSimImage is NULL\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }

    DBG_SaveFile("0x%p '%s'\n", pFile, filename);

#ifdef SAVE_LOCKS
    if (!pFile->bInitiliasedLock)
    {
        LOG_ERROR("file not initialised!\n");
        return IMG_ERROR_FATAL;
    }
    pthread_mutex_lock(&(pFile->fileLock));
#endif
    {
        pFile->saveTo = fopen(filename, "wb");
        if (pFile->saveTo == NULL)
        {
            LOG_ERROR("failed to open file '%s'\n", filename);
            ret = IMG_ERROR_FATAL;
        }

        pFile->ui32TimeWritten = 0;
    }
#ifdef SAVE_LOCKS
    pthread_mutex_unlock(&(pFile->fileLock));
#endif

    return ret;
}

IMG_RESULT SaveFile_openFLX(SaveFile *pFile, const char* filename,
    const struct SimImageInfo *info)
{
    IMG_RESULT ret = IMG_SUCCESS;

    if (pFile == NULL || filename == NULL || info == NULL)
    {
        LOG_ERROR("pFile or filename or info is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (pFile->saveTo != NULL || pFile->pSimImage != NULL)
    {
        LOG_ERROR("pFile->saveTo or pFile->pSimImage is not NULL (file "\
            "alread opened)\n");
        return IMG_ERROR_MEMORY_IN_USE;
    }

    DBG_SaveFile("0x%p '%s'\n", pFile, filename);

#ifdef SAVE_LOCKS
    if (!pFile->bInitiliasedLock)
    {
        LOG_ERROR("file not initialised!\n");
        return IMG_ERROR_FATAL;
    }
    pthread_mutex_lock(&(pFile->fileLock));
#endif
    {
        pFile->pSimImage = (sSimImageOut*)IMG_CALLOC(1, sizeof(sSimImageOut));

        ret = SimImageOut_init(pFile->pSimImage);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to create Sim Image object (returned %d)\n",
                ret);
            goto openFLX_OUT;
        }

        IMG_MEMCPY(&(pFile->pSimImage->info), info,
            sizeof(struct SimImageInfo));

        ret = SimImageOut_create(pFile->pSimImage);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to create FLX file for saving (returned %d)\n",
                ret);
            goto openFLX_OUT;
        }

        ret = SimImageOut_open(pFile->pSimImage, filename);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to open FLX file for saving (returned %d)\n",
                ret);
            goto openFLX_OUT;
        }

        pFile->ui32TimeWritten = 0;
    }
openFLX_OUT:
#ifdef SAVE_LOCKS
    pthread_mutex_unlock(&(pFile->fileLock));
#endif

    return ret;
}

IMG_RESULT SaveFile_close(SaveFile *pFile)
{
    if (pFile == NULL)
    {
        LOG_ERROR("pFile is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    DBG_SaveFile("0x%p\n", pFile);

#ifdef SAVE_LOCKS
    if (!pFile->bInitiliasedLock)
    {
        LOG_ERROR("file not initialised!\n");
        return IMG_ERROR_FATAL;
    }
    pthread_mutex_lock(&(pFile->fileLock));
#endif
    {
        if (pFile->saveTo)
        {
            fclose(pFile->saveTo);
            pFile->saveTo = NULL;
        }
        if (pFile->pSimImage)
        {
            SimImageOut_close(pFile->pSimImage);
            SimImageOut_clean(pFile->pSimImage);
            IMG_FREE(pFile->pSimImage);
            pFile->pSimImage = NULL;
        }
    }
#ifdef SAVE_LOCKS
    pthread_mutex_unlock(&(pFile->fileLock));
#endif

    return IMG_SUCCESS;
}

IMG_RESULT SaveFile_destroy(SaveFile *pFile)
{
    if (pFile == NULL)
    {
        LOG_ERROR("pFile is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    DBG_SaveFile("0x%p\n", pFile);

    if (pFile->saveTo != NULL || pFile->pSimImage != NULL)
    {
        SaveFile_close(pFile);
    }
#ifdef SAVE_LOCKS
    if (pFile->bInitiliasedLock)
    {
        pthread_mutex_destroy(&(pFile->fileLock));
        pFile->bInitiliasedLock = IMG_FALSE;
    }
#endif

    return IMG_SUCCESS;
}

IMG_RESULT SaveFile_write(SaveFile *pFile, const void* ptr, size_t size)
{
    IMG_RESULT ret = IMG_SUCCESS;

    if (pFile == NULL || ptr == NULL || size == 0)
    {
        LOG_ERROR("pFile or ptr is NULL (or size is 0)\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (pFile->saveTo == NULL && pFile->pSimImage == NULL)
    {
        LOG_ERROR("pFile->saveTo and pFile->pSimImage is NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    DBG_SaveFile("0x%p to=0x%p %ldB\n", pFile, ptr, size);

#ifdef SAVE_LOCKS
    if (!pFile->bInitiliasedLock)
    {
        LOG_ERROR("file not initialised!\n");
        return IMG_ERROR_FATAL;
    }
    pthread_mutex_lock(&(pFile->fileLock));
#endif
    {
        if (pFile->saveTo != NULL)
        {
            IMG_SIZE written;
            written = fwrite(ptr, 1, size, pFile->saveTo);
            if (written != size)
            {
                LOG_ERROR("failed to write to file (written %" IMG_SIZEPR \
                    "d/%" IMG_SIZEPR "dB)\n", written, size);
                ret = IMG_ERROR_FATAL;
            }
        }
        else
        {
            ret = SimImageOut_addFrame(pFile->pSimImage, ptr, size);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to add a frame (returned %d)\n", ret);
            }
            ret = SimImageOut_write(pFile->pSimImage);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to write a frame (returned %d\n", ret);
            }
        }

        if (ret == IMG_SUCCESS)
        {
            pFile->ui32TimeWritten++;
        }
    }
#ifdef SAVE_LOCKS
    pthread_mutex_unlock(&(pFile->fileLock));
#endif

    return ret;
}

IMG_RESULT SaveFile_writeFrame(SaveFile *pFile, const void * ptr,
    size_t stride, size_t size, IMG_UINT32 lines)
{
    IMG_RESULT ret = IMG_SUCCESS;
    IMG_UINT32 i;
    IMG_UINT8 *pSave = (IMG_UINT8*)ptr;

    if (pFile == NULL || ptr == NULL || stride == 0 || size == 0
        || lines == 0)
    {
        LOG_ERROR("pFile(%p) or ptr(%p) or is NULL (or stride, size or "\
            "lines is 0)\n", pFile, ptr);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (size > stride)
    {
        LOG_ERROR("size > stride\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (pFile->saveTo == NULL && pFile->pSimImage == NULL)
    {
        LOG_ERROR("pFile->saveTo and pFile->pSimImage are NULL\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    DBG_SaveFile("0x%p to=0x%p %ldx%ldB (str=%ldB)\n", pFile, ptr, size,
        lines, stride);

#ifdef SAVE_LOCKS
    if (!pFile->bInitiliasedLock)
    {
        LOG_ERROR("file not initialised!\n");
        return IMG_ERROR_FATAL;
    }
    pthread_mutex_lock(&(pFile->fileLock));
#endif
    {
        if (pFile->saveTo != NULL)
        {
            IMG_SIZE written;
            for (i = 0; i < lines; i++)
            {
                written = fwrite(pSave, 1, size, pFile->saveTo);
                if (written != size)
                {
                    ret = IMG_ERROR_FATAL;
                    LOG_ERROR("failed to write to file (%" IMG_SIZEPR "d/%" \
                        IMG_SIZEPR "dB written)\n", written, size);
                    break;
                }
                pSave += stride;
            }
        }
        else
        {
            IMG_UINT8 *tmp = (IMG_UINT8*)IMG_MALLOC(size*lines);

            for (i = 0; i < lines; i++)
            {
                IMG_MEMCPY(&(tmp[i*size]), pSave, size);
                pSave += stride;
            }

            ret = SimImageOut_addFrame(pFile->pSimImage, tmp, size*lines);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to add a frame (returned %d)\n", ret);
                ret = IMG_ERROR_FATAL;
            }
            ret = SimImageOut_write(pFile->pSimImage);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to write a frame (returned %d\n", ret);
                ret = IMG_ERROR_FATAL;
            }

            IMG_FREE(tmp);
        }

        if (i != 0)
        {
            pFile->ui32TimeWritten++;
        }
    }
#ifdef SAVE_LOCKS
    pthread_mutex_unlock(&(pFile->fileLock));
#endif

    return ret;
}

//
// transform: R G into R G G B
//            G B
//

IMG_RESULT convertToPlanarBayer12(IMG_UINT16 imageSize[2],
    const void *pInput, IMG_SIZE stride,
    IMG_UINT16 **pOutput, IMG_SIZE *pOutsize)
{
    IMG_UINT16 *outputBuffer = NULL;

    // width in nb of 32b elements
    IMG_UINT32 width32 = 0, outWidth16 = 0;
    // even and odd lines of the input
    IMG_UINT32 *pEven = NULL, *pOdd = NULL;
    IMG_UINT32 mask = (1 << 12) - 1;

    int p;
    IMG_SIZE i;
    IMG_UINT16 l, o, tmpO;

    // should not happen because memory is aligned to 64B
    if (stride%sizeof(IMG_UINT32) != 0)
    {
        LOG_ERROR("size is not a multiple of 32b\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (imageSize[1] % 2)
    {
        // unlikely to be odd because it's extracted from Bayer domain
        LOG_ERROR("function does not work with odd height\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    width32 = stride / sizeof(IMG_UINT32);  // each bop is 192b, 6x4 bytes
    outWidth16 = imageSize[0] * 2;

    pEven = (IMG_UINT32*)pInput;
    pOdd = pEven + width32;

    /* actual allocation should be
     * imageSize[0]*2 * imageSize[1]/2 * sizeof(IMG_UINT16)
     */
    *pOutsize = imageSize[0] * imageSize[1] * sizeof(IMG_UINT16);
    outputBuffer = (IMG_UINT16*)IMG_CALLOC(*pOutsize, 1);

    if (outputBuffer == NULL)
    {
        LOG_ERROR("outputBuffer is NULL\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    *pOutput = outputBuffer;

    // reading 2 lines at once from input!
    for (l = 0; l < imageSize[1] / 2; l++)
    {
        o = 0;  // start of line

#ifdef DEBUG_SAVEFILE
        // ugly cast for warnings with VCC
        if ( (IMG_SIZE)(pEven-(IMG_UINT32*)pInput) >= stride*imageSize[1] )
        {
            LOG_ERROR("pEven 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pEven, pInput,
                ((IMG_PTRDIFF)pInput+stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ( (IMG_SIZE)(pOdd-(IMG_UINT32*)pInput) >= stride*imageSize[1] )
        {
            LOG_ERROR("pOdd 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pOdd, pInput,
                ((IMG_PTRDIFF)pInput+stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ( (IMG_SIZE)(outputBuffer-*pOutput) >= *pOutsize )
        {
            LOG_ERROR("outputBuffer 0x%p does not fit in pOutput (0x%p to "\
                "0x%" IMG_PTRDPR "x)\n", outputBuffer, *pOutput,
                ((IMG_PTRDIFF)*pOutput+*pOutsize));
            return IMG_ERROR_FATAL;
        }
#endif /* DEBUG_SAVEFILE */

        // read two 32b every time - 8 bop of 2 pixels in each line
        for (i = 0; i < width32 && o < outWidth16;)
        {
            IMG_UINT16 outEven[2 * 8], outOdd[2 * 8];
            IMG_UINT8 leftBits = 32;

            leftBits = 32;
            tmpO = i;  // used to store i for verif
            for (p = 0; p < 2 * 8; p++)
            {
                outEven[p] = (pEven[i] >> (32 - leftBits))&mask;
                outOdd[p] = (pOdd[i] >> (32 - leftBits))&mask;

                if (leftBits > 12)
                {
                    leftBits -= 12;
                }
                else if (i + 1 < width32)
                {
                    i++;  // move to next 32b
                    if (leftBits < 12)
                    {
                        outEven[p] |=
                            (pEven[i] & ((1 << (12 - leftBits)) - 1))
                            << (leftBits);
                        outOdd[p] |=
                            (pOdd[i] & ((1 << (12 - leftBits)) - 1))
                            << (leftBits);
                    }
                    leftBits = 32 - (12 - leftBits);
                }
                else
                {
                    break;  // not enough data left!
                }
            }

            /*
             *
             */
            tmpO = o;
            leftBits = 0;  // nb of written entries
            p = 0;
            while (p < 16 && tmpO < outWidth16)
            {
                outputBuffer[tmpO + 0] = outEven[p + 0];
                outputBuffer[tmpO + 1] = outEven[p + 1];
                outputBuffer[tmpO + 2] = outOdd[p + 0];
                outputBuffer[tmpO + 3] = outOdd[p + 1];

                tmpO += 4;
                p += 2;
                leftBits += 4;
            }

            /* add the number of written entries - otherwise o may be bigger
             * than outWidth16 and it will complain! */
            o += leftBits;
        }  // for width

        if (o != outWidth16)
        {
            LOG_ERROR("did not save the correct number of pixels %d vs "\
                "%d\n", o, outWidth16);
            return IMG_ERROR_FATAL;
        }

        // next line
        outputBuffer += outWidth16;
        pEven += 2 * width32;
        pOdd += 2 * width32;
    }  // for line

    return IMG_SUCCESS;
}

IMG_RESULT convertToPlanarBayer10(IMG_UINT16 imageSize[2],
    const void *pInput, IMG_SIZE stride,
    IMG_UINT16 **pOutput, IMG_SIZE *pOutsize)
{
    IMG_UINT16 *outputBuffer = NULL;

    // width in nb of 32b elements
    IMG_SIZE width32 = 0, outWidth16 = 0;
    // even and odd lines of the input
    IMG_UINT32 *pEven = NULL, *pOdd = NULL;
    IMG_UINT32 mask = (1 << 10) - 1;  // 0x3FF;

    int p;
    IMG_SIZE i;
    IMG_UINT16 l, o;

    // should not happen because memory is aligned to 64B
    if (stride%sizeof(IMG_UINT32) != 0)
    {
        LOG_ERROR("size is not a multiple of 32b\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (imageSize[1] % 2)
    {
        // unlikely to be odd because it's extracted from Bayer domain
        LOG_ERROR("function does not work with odd height\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    width32 = stride / sizeof(IMG_UINT32);
    outWidth16 = imageSize[0] * 2;

    pEven = (IMG_UINT32*)pInput;
    pOdd = pEven + width32;

    /* actual allocation should be
     * imageSize[0]*2 * imageSize[1]/2 * sizeof(IMG_UINT16)
     */
    *pOutsize = imageSize[0] * imageSize[1] * sizeof(IMG_UINT16);
    outputBuffer = (IMG_UINT16*)IMG_CALLOC(*pOutsize, 1);

    if (outputBuffer == NULL)
    {
        LOG_ERROR("outputBuffer is NULL\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    *pOutput = outputBuffer;

    // reading 2 lines at once from input!
    for (l = 0; l < imageSize[1] / 2; l++)
    {
        o = 0;  // start of line

#ifdef DEBUG_SAVEFILE
        // ugly cast for warnings with VCC
        if ( (IMG_SIZE)(pEven-(IMG_UINT32*)pInput) >= stride*imageSize[1] )
        {
            LOG_ERROR("pEven 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pEven, pInput,
                ((IMG_PTRDIFF)pInput+stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ( (IMG_SIZE)(pOdd-(IMG_UINT32*)pInput) >= stride*imageSize[1] )
        {
            LOG_ERROR("pOdd 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pOdd, pInput,
                ((IMG_PTRDIFF)pInput+stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ( (IMG_SIZE)(outputBuffer-*pOutput) >= *pOutsize )
        {
            LOG_ERROR("outputBuffer 0x%p does not fit in pOutput (0x%p to "\
                "0x%" IMG_PTRDPR "x)\n", outputBuffer, *pOutput,
                ((IMG_PTRDIFF)*pOutput+*pOutsize));
            return IMG_ERROR_FATAL;
        }
#endif /* DEBUG_SAVEFILE */

        // read two 32b every time
        for (i = 0; i < width32 && o < outWidth16;)
        {
            IMG_UINT16 outEven[2 * 3], outOdd[2 * 3];

            /*
             * extract the even and odd row information regardless of its
             * usefulness
             */
            for (p = 0; p < 3; p++)
            {
                outEven[p] = (pEven[i] >> (10 * p))&mask;
                outOdd[p] = (pOdd[i] >> (10 * p))&mask;
            }
            i++;

            // in some cases the output will not do the second part
            // depending on the resolution
            for (p = 0; p < 3 && i < width32; p++)
            {
                outEven[3 + p] = (pEven[i] >> (10 * p))&mask;
                outOdd[3 + p] = (pOdd[i] >> (10 * p))&mask;
            }
            i++;

            /*
             *
             */
            p = 0;

            while (p < 6 && o < outWidth16)
            {
                outputBuffer[o + 0] = outEven[p + 0];
                outputBuffer[o + 1] = outEven[p + 1];
                outputBuffer[o + 2] = outOdd[p + 0];
                outputBuffer[o + 3] = outOdd[p + 1];

                o += 4;
                p += 2;
            }

            // o+=12; // 2 lines of 3 bop of 2 pixels
        }  // for width

        // next line
        outputBuffer += outWidth16;
        pEven += 2 * width32;
        pOdd += 2 * width32;
    }  // for line

    return IMG_SUCCESS;
}

IMG_RESULT convertToPlanarBayer8(IMG_UINT16 imageSize[2], const void *pInput,
    IMG_SIZE stride, IMG_UINT8 **pOutput, IMG_SIZE *pOutsize)
{
    IMG_UINT8 *outputBuffer = NULL;

    // width in nb of 32b elements
    IMG_SIZE width32 = 0, outWidth8 = 0;
    // even and odd lines of the input
    IMG_UINT32 *pEven = NULL, *pOdd = NULL;
    IMG_UINT32 mask = 0xFF;

    int p;
    IMG_SIZE i;
    IMG_UINT16 l, o;

    // should not happen because memory is aligned to 64B
    if (stride%sizeof(IMG_UINT32) != 0)
    {
        LOG_ERROR("size is not a multiple of 32b\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    if (imageSize[1] % 2)
    {
        // unlikely to be odd because it's extracted from Bayer domain
        LOG_ERROR("function does not work with odd height\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    width32 = stride / sizeof(IMG_UINT32);
    outWidth8 = imageSize[0] * 2;

    pEven = (IMG_UINT32*)pInput;
    pOdd = pEven + width32;

    /* : the output size should be the same as the input size - but if it
     * is not we stop before the image is over... */
    *pOutsize = imageSize[0] * imageSize[1] * sizeof(IMG_UINT8);
    outputBuffer = (IMG_UINT8*)IMG_CALLOC(*pOutsize, 1);

    if (outputBuffer == NULL)
    {
        LOG_ERROR("outputBuffer is NULL\n");
        return IMG_ERROR_MALLOC_FAILED;
    }

    *pOutput = outputBuffer;

    // reading 2 lines at once from input but each line is contains 2 pixels!
    for (l = 0; l < imageSize[1] / 2; l++)
    {
        o = 0;  // start of line

#ifdef DEBUG_SAVEFILE
        // ugly cast for warnings with VCC
        if ( (IMG_SIZE)(pEven-(IMG_UINT32*)pInput) >= stride*imageSize[1] )
        {
            LOG_ERROR("pEven 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pEven, pInput,
                ((IMG_PTRDIFF)pInput+stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ( (IMG_SIZE)(pOdd-(IMG_UINT32*)pInput) >= stride*imageSize[1] )
        {
            LOG_ERROR("pOdd 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pOdd, pInput,
                ((IMG_PTRDIFF)pInput+stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ( (IMG_SIZE)(outputBuffer-*pOutput) >= *pOutsize )
        {
            LOG_ERROR("outputBuffer 0x%p does not fit in pOutput (0x%p to "\
                "0x%" IMG_PTRDPR "x)\n", outputBuffer, *pOutput,
                ((IMG_PTRDIFF)*pOutput+*pOutsize));
            return IMG_ERROR_FATAL;
        }
#endif /* DEBUG_SAVEFILE */

        // read 1x 32b every time
        for (i = 0; i < width32 && o < outWidth8; i++)
        {
            /*
             * extract the even and odd row information regardless of its
             * usefulness
             */
            for (p = 0; p < 4 && o < outWidth8; p++) // 2 bop of 2 pixels
            {
                outputBuffer[o + 0] = (pEven[i] >> (8 * p))&mask;
                outputBuffer[o + 1] = (pOdd[i] >> (8 * p))&mask;
                o += 2;
            }
        }  // for width

        // next line
        outputBuffer += outWidth8;
        pEven += 2 * width32;
        pOdd += 2 * width32;
    }  // for line

    return IMG_SUCCESS;
}

IMG_RESULT convertToPlanarBayer(IMG_UINT16 imageSize[2], const void *pInput,
    IMG_SIZE stride, IMG_UINT8 uiBayerDepth, void **pOutput,
    IMG_SIZE *pOutsize)
{
    switch (uiBayerDepth)
    {
    case 8:
        return convertToPlanarBayer8(imageSize, pInput, stride,
            (IMG_UINT8**)pOutput, pOutsize);
    case 10:
        return convertToPlanarBayer10(imageSize, pInput, stride,
            (IMG_UINT16**)pOutput, pOutsize);
    case 12:
        return convertToPlanarBayer12(imageSize, pInput, stride,
            (IMG_UINT16**)pOutput, pOutsize);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT convertToPlanarBayerTiff12(IMG_UINT16 imageSize[2],
    const void *pInput, IMG_SIZE stride,
    IMG_UINT16 **pOutput, IMG_SIZE *pOutsize
    )
{
    // even and odd lines of the input
    unsigned char *pEven = NULL, *pOdd = NULL;
    unsigned int i, o;
    IMG_UINT16 l;
    unsigned int outWidth16 = 0;
    unsigned int minStride = 0;
    IMG_UINT16 *pOut = NULL;

    // no need to check as minStride is computed for that
    /*if ( stride%(3*sizeof(unsigned char)) != 0 )
    {
    LOG_ERROR("size is not a multiple of 24b\n");
    return IMG_ERROR_NOT_SUPPORTED;
    }*/
    if (imageSize[1] % 2)
    {
        // unlikely to be odd because it's extracted from Bayer domain
        LOG_ERROR("function does not work with odd height\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pEven = (unsigned char*)pInput;
    pOdd = pEven + stride;

    // bop is 2 elements in 3 Bytes
    // so we round up size and multiply by 3
    // (width should be even because image is extracted from Bayer domain)
    minStride = ((imageSize[0] + 1) / 2) * 3;

    *pOutsize = imageSize[0] * imageSize[1] * sizeof(IMG_UINT16);
    *pOutput = (IMG_UINT16*)IMG_MALLOC(*pOutsize);
    /* output is in R G G B order (instead of separating R G / G B in 2 lines
    * the size is in pixels (so in CFA it's /2) so
    * outWidth16 = width /2 (horiz CFA subsampling) but
    * we also multiply by 4 because we put the 4 elements together instead
    * of using 2 lines
    * therefore the actual operation should be
    * outWidth16 = (imageSize[0]/2) *4*/
    outWidth16 = imageSize[0] * 2;  // in ui16 elements not bytes!

    if (!*pOutput)
    {
        LOG_ERROR("*pOutput is NULL\n");
        return IMG_ERROR_MALLOC_FAILED;
    }
    pOut = *pOutput;

    // reading 2 lines at once from input!
    for (l = 0; l < imageSize[1] / 2; l++)
    {
        o = 0;  // start of line

        // ugly cast for warnings with VCC
        if ((IMG_SIZE)(pEven - (unsigned char*)pInput) >= stride*imageSize[1])
        {
            LOG_ERROR("pEven 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pEven, pInput,
                ((IMG_PTRDIFF)pInput + stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ((IMG_SIZE)(pOdd - (unsigned char*)pInput) >= stride*imageSize[1])
        {
            LOG_ERROR("pOdd 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pOdd, pInput,
                ((IMG_PTRDIFF)pInput + stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }

        // bop is 2 elements in 3 Bytes
        for (i = 0; i < minStride; i += 3)
        {
            // read 8b out of 8 shifted to msb of 12b output
            // read 4b out of 8 to be lsb of 12b output
            pOut[l*outWidth16 + o + 0] = 0
                | pEven[i + 0] << (12 - 8)
                | ((pEven[i + 1] & 0xF0) >> (8 - 4));
            // read 4b out of 8 shifted to be msb of 12b output
            // read 8b out of 8 to be lsb of 12b output
            pOut[l*outWidth16 + o + 1] = 0
                | ((pEven[i + 1] & 0x0F) << (12 - 4))
                | pEven[i + 2];

            // same but for the odd line
            pOut[l*outWidth16 + o + 2] = 0
                | pOdd[i + 0] << (12 - 8)
                | ((pOdd[i + 1] & 0xF0) >> (8 - 4));
            pOut[l*outWidth16 + o + 3] = 0
                | ((pOdd[i + 1] & 0x0F) << (12 - 4))
                | pOdd[i + 2];

            o += 4;
        }

        // move 2 lines down in the image means next even line
        pEven += 2 * stride;
        pOdd += 2 * stride;
    }

    return IMG_SUCCESS;
}

IMG_RESULT convertToPlanarBayerTiff10(IMG_UINT16 imageSize[2],
    const void *pInput, IMG_SIZE stride,
    IMG_UINT16 **pOutput, IMG_SIZE *pOutsize
    )
{
    // even and odd lines of the input
    unsigned char *pEven = NULL, *pOdd = NULL;
    unsigned int i, o, outWidth16 = 0;
    unsigned int minStride = 0;
    IMG_UINT16 l;
    IMG_UINT16 *pOut = NULL;

    // no need to check as minStride is computed for that
    /*if ( stride%(5*sizeof(unsigned char)) != 0 )
    {
    LOG_ERROR("size is not a multiple of 40b\n");
    return IMG_ERROR_NOT_SUPPORTED;
    }*/
    if (imageSize[1] % 2)
    {
        // unlikely to be odd because it's extracted from Bayer domain
        LOG_ERROR("function does not work with odd height\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    pEven = (unsigned char*)pInput;
    pOdd = pEven + stride;

    // bop is 4 elements in 5 bytes
    // so we round up size to 4: ((x+3)/4)*4
    // and need 5 Bytes for 4 elemets (removed the *4 from round up)
    minStride = ((imageSize[0] + 3) / 4) * 5;

    *pOutsize = imageSize[0] * imageSize[1] * sizeof(IMG_UINT16);
    *pOutput = (IMG_UINT16*)IMG_MALLOC(*pOutsize);
    /* output is in R G G B order (instead of separating R G / G B in 2 lines
     * the size is in pixels (so in CFA it's /2) so
     * outWidth16 = width /2 (horiz CFA subsampling) but
     * we also multiply by 4 because we put the 4 elements together instead
     * of using 2 lines
     * therefore the actual operation should be
     * outWidth16 = (imageSize[0]/2) *4*/
    outWidth16 = imageSize[0] * 2;  // in ui16 elements not bytes!

    if (!*pOutput)
    {
        LOG_ERROR("*pOutput is NULL\n");
        return IMG_ERROR_MALLOC_FAILED;
    }
    pOut = *pOutput;

    // reading 2 lines at once from input!
    for (l = 0; l < imageSize[1] / 2; l++)
    {
        o = 0;  // start of line

        // ugly cast for warnings with VCC
        if ((IMG_SIZE)(pEven - (unsigned char*)pInput) >= stride*imageSize[1])
        {
            LOG_ERROR("pEven 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pEven, pInput,
                ((IMG_PTRDIFF)pInput + stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }
        if ((IMG_SIZE)(pOdd - (unsigned char*)pInput) >= stride*imageSize[1])
        {
            LOG_ERROR("pOdd 0x%p does not fit in pInput (0x%p to 0x%" \
                IMG_PTRDPR "x)\n", pOdd, pInput,
                ((IMG_PTRDIFF)pInput + stride*imageSize[1]));
            return IMG_ERROR_FATAL;
        }

        // bop pack stride is 5 Bytes packing 4 elements
        for (i = 0; i < minStride; i += 5)
        {
            // read 2 couples for even lines and 2 couples from odd lines
            // TIFF is MSB packed (see HW TRM for details)
            pOut[l*outWidth16 + o + 0] = 0
                | (pEven[i + 0] << (10 - 8))
                | ((pEven[i + 1] & 0xC0) >> (8 - 2));
            pOut[l*outWidth16 + o + 1] = 0
                | (pEven[i + 1] & 0x3F) << (10 - 6)
                | ((pEven[i + 2] & 0xF0) >> (8 - 4));

            pOut[l*outWidth16 + o + 2] = 0
                | (pOdd[i + 0] << (10 - 8))
                | ((pOdd[i + 1] & 0xC0) >> (8 - 2));
            pOut[l*outWidth16 + o + 3] = 0
                | ((pOdd[i + 1] & 0x3F) << (10 - 6))
                | ((pOdd[i + 2] & 0xF0) >> (8 - 4));

            // because we re-ordered the R G / G B into a single R G G B
            if (o + 7 < outWidth16)
            {
                pOut[l*outWidth16 + o + 4] = 0
                    | ((pEven[i + 2] & 0x0F) << (10 - 4))
                    | ((pEven[i + 3] & 0xFC) >> (8 - 6));
                pOut[l*outWidth16 + o + 5] = 0
                    | ((pEven[i + 3] & 0x03) << (10 - 2))
                    | (pEven[i + 4]);

                pOut[l*outWidth16 + o + 6] = 0
                    | ((pOdd[i + 2] & 0x0F) << (10 - 4))
                    | ((pOdd[i + 3] & 0xFC) >> (8 - 6));
                pOut[l*outWidth16 + o + 7] = 0
                    | ((pOdd[i + 3] & 0x03) << (10 - 2))
                    | (pOdd[i + 4]);
                o += 8;
            }
            else
            {
                o += 4;
            }
        }

        // move 2 lines down in the image means next even line
        pEven += 2 * stride;
        pOdd += 2 * stride;
    }

    return IMG_SUCCESS;
}

IMG_RESULT convertToPlanarBayerTiff(IMG_UINT16 imageSize[2],
    const void *pInput, IMG_SIZE stride, IMG_UINT8 uiBayerDepth,
    void **pOutput, IMG_SIZE *pOutsize)
{
    switch (uiBayerDepth)
    {
    case 10:
        return convertToPlanarBayerTiff10(imageSize, pInput, stride,
            (IMG_UINT16**)pOutput, pOutsize);
    case 12:
        return convertToPlanarBayerTiff12(imageSize, pInput, stride,
            (IMG_UINT16**)pOutput, pOutsize);
    }
    return IMG_ERROR_NOT_SUPPORTED;
}
