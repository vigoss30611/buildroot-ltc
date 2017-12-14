/**
*******************************************************************************
@file ci_converter.c

@brief Implementation of the frame converter for CI

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
#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <sim_image.h>

#include <felixcommon/ci_alloc_info.h>

#include "ci/ci_converter.h"

#ifdef IMG_KERNEL_MODULE
#error "this should not be in a kernel module"
#endif

#define LOG_TAG "CI_CONVERTER"
#include <felixcommon/userlog.h>

/**
 * @brief just check whether all pixels are in valid range, no real
 * functionality
 */
static void CheckPixelRange(const IMG_UINT16 *inPixels, IMG_UINT32 stride,
    IMG_UINT32 Npixels, IMG_UINT32 bitsPerPixel)
{
#if !defined(NDEBUG) && !defined(FELIX_UNIT_TESTS)
    IMG_UINT32 x;
    IMG_UINT32 maxpix = (1 << bitsPerPixel);
    for (x = 0; x < Npixels; x++)
    {
        IMG_UINT32 pix;
        pix = inPixels[x*stride];
        if (!(pix < maxpix))
        {
            LOG_WARNING("pixel %d value 0x%X out of %d-bit range (is input" \
                " image compatible with selected gasket?)\n",
                x, pix, bitsPerPixel);
        }
    }
#endif
}

/*
 * Parallel part
 */

#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE CI_Convert_Parallel10(void *privateData, const IMG_UINT16 *inPixels,
IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    IMG_UINT32 *wr = NULL;
    IMG_SIZE Nwritten;

    // calculate number of bytes that will/would be written
    // 8 output bytes per 3 input Bayer cells
    Nwritten = (NpixelsPerCh / 3) * 8;
    if ((NpixelsPerCh % 3) > 1) Nwritten += 8;
    else if (NpixelsPerCh % 3) Nwritten += 4;

    // actually convert the pixels
    if (outPixels)
    {
        CheckPixelRange(inPixels, inStride, NpixelsPerCh, 10);
        CheckPixelRange(inPixels + 1, inStride, NpixelsPerCh, 10);

        wr = (IMG_UINT32*)outPixels;

        for (; NpixelsPerCh >= 3; NpixelsPerCh -= 3)
        {
            *wr++ = (inPixels[0 * inStride + 0] & 0x3ff)
                | ((inPixels[0 * inStride + 1] & 0x3ff) << 10)
                | ((inPixels[1 * inStride + 0] & 0x3ff) << 20);
            *wr++ = (inPixels[1 * inStride + 1] & 0x3ff)
                | ((inPixels[2 * inStride + 0] & 0x3ff) << 10)
                | ((inPixels[2 * inStride + 1] & 0x3ff) << 20);
            inPixels += 3 * inStride;
        }
        if (NpixelsPerCh > 0)
        {
            *wr++ = (inPixels[0 * inStride + 0] & 0x3ff)
                | ((inPixels[0 * inStride + 1] & 0x3ff) << 10)
                | ((NpixelsPerCh > 1) ?
                ((inPixels[1 * inStride + 0] & 0x3ff) << 20) : 0);
            if (NpixelsPerCh > 1)
                *wr++ = (inPixels[1 * inStride + 1] & 0x3ff)
                | ((NpixelsPerCh > 2) ?
                ((inPixels[2 * inStride + 0] & 0x3ff) << 10) : 0)
                | ((NpixelsPerCh > 2) ?
                ((inPixels[2 * inStride + 1] & 0x3ff) << 20) : 0);
        }
        IMG_ASSERT(Nwritten == (IMG_UINT8*)wr - outPixels);
    }
    return Nwritten;
}

#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE CI_Convert_Parallel12(void *privateData, const IMG_UINT16 *inPixels,
IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    IMG_UINT8 *wr = NULL;
    IMG_SIZE Nwritten;

    // calculate number of bytes that will/would be written
    Nwritten = NpixelsPerCh * 3;  // 3 output bytes per 1 input Bayer cell

    // actually convert the pixels
    if (outPixels)
    {
        CheckPixelRange(inPixels, inStride, NpixelsPerCh, 12);
        CheckPixelRange(inPixels + 1, inStride, NpixelsPerCh, 12);
        wr = outPixels;
        while (NpixelsPerCh--)
        {
            *wr++ = inPixels[0] & 0xff;
            *wr++ = (inPixels[0] >> 8) | ((inPixels[1] & 0xf) << 4);
            *wr++ = inPixels[1] >> 4;
            inPixels += inStride;
        }
        IMG_ASSERT(Nwritten == wr - outPixels);
    }
    return Nwritten;
}

/**
 * MIPI part
 */

/** @brief The MIPI packet types Identifiers (CIS-2.pfg table 3 p50) */
typedef enum DG_MIPI_ID
{
    DG_MIPI_ID_FS = 0,        /**< @brief short packet - frame start */
    DG_MIPI_ID_FE = 1,        /**< @brief short packet - frame end */
    DG_MIPI_ID_LS = 2,        /**< @brief short packet - line start */
    DG_MIPI_ID_LE = 3,        /**< @brief short packet - line end */
    DG_MIPI_ID_RAW10 = 0x2b,  /**< @brief long packet - RAW10 data */
    DG_MIPI_ID_RAW12 = 0x2c,  /**< @brief long packet - RAW12 data */
    DG_MIPI_ID_RAW14 = 0x2d,  /**< @brief long packet - RAW14 data */
} eDG_MIPI_ID;

/**
 * Compute the ECC byte for MIPI - taken from CSIM
 *
 * Not always static to be accessible in unit-tests
 */
#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_UINT8 MIPI_GetHeaderECC(const IMG_UINT8 *data3)
{
    // CSI-2.pdf (MIPI spec) p52-53
    static const char *ECCBits[] =
    {
        /* for ECC bit 0 - XOR these bits of 24-bit data (LSBit->MSBit) to
        * get ECC bit 0 */
        "111011010011010010001111",
        "110110101010101001001111",  // for ECC bit 1
        "101101100101100100101110",  // for ECC bit 2
        "011100011100011100011101",  // for ECC bit 3
        "000011111100000011111011",  // for ECC bit 4
        "000000000011111111110111",  // for ECC bit 5
        "000000000000000000000000",  // for ECC bit 6
        "000000000000000000000000",  // for ECC bit 7
    };
    IMG_INT eccBit, dataBit, bitVal;
    IMG_UINT8 ecc;
    ecc = 0;  // the ECC byte
    for (eccBit = 0; eccBit < 8; eccBit++)
    {
        bitVal = 0;  // value of the eccBit-th bit of the ECC (0 or 1)
        for (dataBit = 0; dataBit < 24; dataBit++)
            bitVal ^= (data3[dataBit / 8] >> (dataBit % 8))
            & (ECCBits[eccBit][dataBit] - '0');
        ecc |= (bitVal << eccBit);
    }
    return ecc;
}

/**
 * Write the control packets (FS, LS, DATA, LE, FE)
 *
 * Not always static to be accessible in unit-tests
 */
#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE MIPI_WriteControl(IMG_UINT8 *pBuf, IMG_INT8 virtualChannel,
IMG_INT8 dataId, IMG_UINT16 ui16Data)
{
    IMG_UINT8 *wr;
    IMG_SIZE ui32ToWrite = 4;

    if (pBuf != NULL)
    {
        wr = pBuf;
        *wr++ = (virtualChannel << 6) | (dataId & 0x3f);
        *(IMG_UINT16*)wr = ui16Data;
        wr += sizeof(IMG_UINT16);  // data size
        *wr++ = MIPI_GetHeaderECC(pBuf);  // the ECC byte

        IMG_ASSERT(ui32ToWrite == (wr - pBuf));
    }

    return ui32ToWrite;
}

#define RoundUpDiv(x, of) ( ((x) + (of) - 1) / (of) )

/**
 * Convert a set of 10b stored in 16b pixels into MIPI 12b format
 *
 * Not always static to be accessible in unit-tests
 */
#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE MIPI_Write_10(const IMG_UINT16 *inPixels,
IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    IMG_UINT8 *wr;
    IMG_SIZE NpixelGroups, Nwritten;

    // calculate number of bytes that will/would be written
    // #of groups of 4 pixels (2 channels accumulated)
    NpixelGroups = RoundUpDiv((NpixelsPerCh * 2), 4);
    Nwritten = NpixelGroups * 5;  // 5 bytes per group of 4 pixels

    // actually convert the pixels only if output exists
    if (outPixels)
    {
        int oddLine, evenLine;

        CheckPixelRange(inPixels, inStride, NpixelsPerCh, 10);
        CheckPixelRange(inPixels + 1, inStride, NpixelsPerCh, 10);
        wr = outPixels;
        // *2 channels = 4 pixels per loop
        // even line (i + 0), odd line (i + 1)
        // even pixel [stride] + 0, odd pixel [stride] + 1
        evenLine = 0;
        oddLine = inStride;
        for (; NpixelsPerCh >= 2; NpixelsPerCh -= 2)
        {
            *wr++ = (inPixels[evenLine + 0] >> 2);
            *wr++ = (inPixels[evenLine + 1] >> 2);
            *wr++ = (inPixels[oddLine + 0] >> 2);
            *wr++ = (inPixels[oddLine + 1] >> 2);
            *wr++ = ((inPixels[evenLine + 0] & 3) << 0)
                | ((inPixels[evenLine + 1] & 3) << 2)
                | ((inPixels[oddLine + 0] & 3) << 4)
                | ((inPixels[oddLine + 1] & 3) << 6);
            evenLine += 2 * inStride;
            oddLine += 2 * inStride;
        }
        // last one if not a multiple of 2
        if (NpixelsPerCh > 0)
        {
            *wr++ = (inPixels[evenLine + 0] >> 2);
            *wr++ = (inPixels[evenLine + 1] >> 2);
            *wr++ = 0;
            *wr++ = 0;
            *wr++ = ((inPixels[evenLine + 0] & 3) << 0)
                | ((inPixels[evenLine + 1] & 3) << 2);
        }
        IMG_ASSERT(Nwritten == wr - outPixels);
    }
    return Nwritten;
}

/**
 * Convert a set of 12b stored in 16b pixels into MIPI RAW 12b format
 *
 * Not always static to be accessible in unit-tests
 */
#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE MIPI_Write_12(const IMG_UINT16 *inPixels,
IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    IMG_UINT8 *wr;
    IMG_SIZE NpixelGroups, Nwritten;
    // calculate number of bytes that will/would be written
    // #of groups of 2 pixels (2 channels accumulated)
    NpixelGroups = NpixelsPerCh;
    Nwritten = NpixelGroups * 3;  // 3 bytes per group of 2 pixels
    // actually convert the pixels only if output present
    if (outPixels)
    {
        int line = 0;
        CheckPixelRange(inPixels, inStride, NpixelsPerCh, 12);
        CheckPixelRange(inPixels + 1, inStride, NpixelsPerCh, 12);
        wr = outPixels;
        // *2 channels = 2 pixels per loop
        for (; NpixelsPerCh > 0; NpixelsPerCh--)
        {
            *wr++ = (inPixels[line + 0] >> 4);
            *wr++ = (inPixels[line + 1] >> 4);
            *wr++ = ((inPixels[line + 0] & 0xf) << 0)
                | ((inPixels[line + 1] & 0xf) << 4);

            line += inStride;
        }
        IMG_ASSERT(Nwritten == wr - outPixels);
    }
    return Nwritten;
}

/**
 * Convert a set of 14b stored in 16b pixels into MIPI RAW 14b format
 *
 * Not always static to be accessible in unit-tests
 */
#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE MIPI_Write_14(const IMG_UINT16 *inPixels,
IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    IMG_UINT8 *wr;
    IMG_SIZE NpixelGroups, Nwritten, tmp;

    // calculate number of bytes that will/would be written
    // #of groups of 4 pixels (2 channels accumulated)
    NpixelGroups = RoundUpDiv((NpixelsPerCh * 2), 4);
    Nwritten = NpixelGroups * 7;  // 7 bytes per group of 4 pixels

    // actually convert the pixels only if output is present
    if (outPixels)
    {
        int evenLine, oddLine;
        CheckPixelRange(inPixels, inStride, NpixelsPerCh, 14);
        CheckPixelRange(inPixels + 1, inStride, NpixelsPerCh, 14);
        wr = outPixels;
        // *2 channels = 4 pixels per loop
        evenLine = 0;
        oddLine = inStride;
        for (; NpixelsPerCh >= 2; NpixelsPerCh -= 2)
        {
            *wr++ = (inPixels[evenLine + 0] >> 6);
            *wr++ = (inPixels[evenLine + 1] >> 6);
            *wr++ = (inPixels[oddLine + 0] >> 6);
            *wr++ = (inPixels[oddLine + 1] >> 6);
            tmp = ((inPixels[evenLine + 0] & 0x3f) << 0)
                | ((inPixels[evenLine + 1] & 0x3f) << 6)
                | ((inPixels[oddLine + 0] & 0x3f) << 12)
                | ((inPixels[oddLine + 1] & 0x3f) << 18);
            *wr++ = ((IMG_INT8*)&tmp)[0];
            *wr++ = ((IMG_INT8*)&tmp)[1];
            *wr++ = ((IMG_INT8*)&tmp)[2];

            evenLine += 2 * inStride;
            oddLine += 2 * inStride;
        }
        if (NpixelsPerCh > 0)
        {
            *wr++ = (inPixels[evenLine + 0] >> 6);
            *wr++ = (inPixels[evenLine + 1] >> 6);
            *wr++ = 0;
            *wr++ = 0;
            tmp = ((inPixels[evenLine + 0] & 0x3f) << 0)
                | ((inPixels[evenLine + 1] & 0x3f) << 6);
            *wr++ = ((IMG_INT8*)&tmp)[0];
            *wr++ = ((IMG_INT8*)&tmp)[1];
            *wr++ = ((IMG_INT8*)&tmp)[2];
        }
        IMG_ASSERT(Nwritten == wr - outPixels);
    }
    return Nwritten;
}

#undef RoundUpDiv

static IMG_SIZE MIPI_FrameHeader(CI_CONV_MIPI_PRIV *pMipiPriv,
    IMG_UINT16 convBytes, IMG_UINT8 *outPixels, IMG_UINT8 dataId)
{
    IMG_SIZE written = 0;
    IMG_UINT8 *pWrite = outPixels;
    IMG_SIZE uiSize = 0;

    if (pMipiPriv->bLongFormat)
    {
        // valid to have line number of 0 when no output
        IMG_ASSERT(!outPixels || pMipiPriv->i32LineNumber > 0);
        written = MIPI_WriteControl(pWrite, pMipiPriv->ui8VirtualChannel,
            DG_MIPI_ID_LS, pMipiPriv->i32LineNumber);

        uiSize += written;
        if (outPixels)
        {
            pWrite = outPixels + uiSize;
        }
    }

    written = MIPI_WriteControl(pWrite, pMipiPriv->ui8VirtualChannel,
        dataId, convBytes);
    uiSize += written;

    return uiSize;
}

static IMG_SIZE MIPI_FrameFooter(CI_CONV_MIPI_PRIV *pMipiPriv,
    IMG_UINT16 convBytes, IMG_UINT8 *outPixels, IMG_UINT16 ui16Crc)
{
    IMG_SIZE written = 0;
    IMG_UINT8 *pWrite = outPixels;
    IMG_SIZE uiSize = 0;

    // write CRC
    uiSize += 2;
    if (outPixels)
    {
        // write only if data is present
        *(IMG_UINT16*)pWrite = ui16Crc;
        pWrite = outPixels + uiSize;
    }

    if (pMipiPriv->bLongFormat)
    {
        written = MIPI_WriteControl(pWrite, pMipiPriv->ui8VirtualChannel,
            DG_MIPI_ID_LE, pMipiPriv->i32LineNumber);

        uiSize += written;
        if (outPixels)
        {
            pWrite = outPixels + uiSize;
        }
    }

    return uiSize;
}

/**
 * Not always static to be accessible in unit-tests
 */
#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_UINT16 MIPI_ComputeCRC(const IMG_UINT8 *dataBytes,
IMG_UINT32 Nbytes)
{
    IMG_UINT16 crc, poly;
    IMG_UINT32 bit;
    crc = 0xffff;
    poly = 0x8408;

    for (bit = 0; bit < Nbytes * 8; bit++)
    {
        if ((crc & 1) ^ ((dataBytes[bit >> 3] >> (bit & 7)) & 1))
            crc = (crc >> 1) ^ poly;
        else
            crc >>= 1;
    }

    return crc;
}

#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE CI_Convert_MIPI_Line(void *privateData, const IMG_UINT16 *inPixels,
IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
    CI_CONV_MIPI_PRIV *pMipiPriv = (CI_CONV_MIPI_PRIV*)privateData;
    IMG_SIZE written = 0;
    IMG_UINT16 convBytes = 0;
    IMG_UINT8 *pWrite = outPixels;
    IMG_SIZE uiSize = 0;  // size of what was written
    IMG_UINT16 ui16Crc = 0;

    if (!pMipiPriv->pfnMipiWrite)
    {
        return 0;
    }

    // conv bytes computation using stride (in pixels, input is IMG_UINT16)
    convBytes = pMipiPriv->pfnMipiWrite(NULL, inStride, NpixelsPerCh, NULL);

    // write header (optional line start + data start)
    written = MIPI_FrameHeader(pMipiPriv, convBytes, pWrite,
        pMipiPriv->ui8DataId);
    uiSize += written;
    if (outPixels)
    {
        pWrite = outPixels + uiSize;
    }

    // write converted data
    written = pMipiPriv->pfnMipiWrite(inPixels, inStride, NpixelsPerCh,
        pWrite);
    IMG_ASSERT(written == convBytes);

    // move only after CRC computed
    uiSize += written;
    if (outPixels)
    {
        // compute CRC only if we actually have data
        ui16Crc = MIPI_ComputeCRC(pWrite, convBytes);
        pWrite = outPixels + uiSize;
    }

    // write footer (CRC + optional line end)
    written = MIPI_FrameFooter(pMipiPriv, convBytes, pWrite, ui16Crc);
    uiSize += written;
    /*if (outPixels)
    {
    pWrite = outPixels + uiSize;
    }*/

    return uiSize;
}

#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE CI_Convert_MIPI_Header(void *privateData, IMG_UINT8 *out)
{
    CI_CONV_MIPI_PRIV *pMipiPriv = (CI_CONV_MIPI_PRIV*)privateData;
    IMG_SIZE written = MIPI_WriteControl(out, pMipiPriv->ui8VirtualChannel,
        DG_MIPI_ID_FS, pMipiPriv->ui16FrameNumber);
    return written;
}

#ifndef FELIX_UNIT_TESTS
static
#endif
IMG_SIZE CI_Convert_MIPI_Footer(void *privateData, IMG_UINT8 *out)
{
    CI_CONV_MIPI_PRIV *pMipiPriv = (CI_CONV_MIPI_PRIV*)privateData;
    IMG_SIZE written = MIPI_WriteControl(out, pMipiPriv->ui8VirtualChannel,
        DG_MIPI_ID_FE, pMipiPriv->ui16FrameNumber);
    return written;
}

IMG_RESULT CI_Convert_HDRInsertion(const struct _sSimImageIn_ *pImage,
    CI_BUFFER *pBuffer, IMG_BOOL8 bRescale)
{
    if (!pImage || !pBuffer)
    {
        LOG_ERROR("pImage or pBuffer is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (SimImage_RGB64 == pImage->info.eColourModel)
    {
        struct CI_SIZEINFO sizeInfo;
        PIXELTYPE pixelType;
        IMG_INT32 neededSize = 0;
        IMG_UINT16 *buffData = (IMG_UINT16*)pBuffer->data;
        unsigned i = 0, j = 0;
        unsigned inStride = 0, outStride = 0;
        int r = 0, g = 1, b = 2;  // pixel orders in pImage
        IMG_RESULT ret;

        ret = PixelTransformRGB(&pixelType, BGR_161616_64);
        if (ret)
        {
            LOG_ERROR("Failed to get information about %s",
                FormatString(BGR_161616_64));
            return IMG_ERROR_FATAL;
        }

        ret = CI_ALLOC_RGBSizeInfo(&pixelType, pImage->info.ui32Width,
            pImage->info.ui32Height, NULL, &sizeInfo);
        if (ret)
        {
            LOG_ERROR("Failed to get allocation information about %s for "\
                "image size %dx%d",
                FormatString(BGR_161616_64), pImage->info.ui32Width,
                pImage->info.ui32Height);
            return IMG_ERROR_FATAL;
        }

        neededSize = sizeInfo.ui32Stride*sizeInfo.ui32Height;

        if ((IMG_UINT32)neededSize > pBuffer->ui32Size)
        {
            LOG_ERROR("a buffer of %u Bytes is too small - %u needed\n",
                pBuffer->ui32Size, neededSize);
            return IMG_ERROR_INVALID_PARAMETERS;
        }
        if (pImage->info.ui8BitDepth > 16)
        {
            LOG_ERROR("unexpected large bitdepth of %d in input image!\n",
                pImage->info.ui8BitDepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        IMG_ASSERT(sizeInfo.ui32Stride % sizeof(IMG_UINT16) == 0);
        IMG_ASSERT(pImage->info.stride % sizeof(IMG_UINT16) == 0);
        outStride = sizeInfo.ui32Stride / sizeof(IMG_UINT16);
        inStride = pImage->info.stride / sizeof(IMG_UINT16);

        if (!pImage->info.isBGR)
        {
            r = 2;
            b = 0;
        }

        if (bRescale)
        {
            int shift = 16 - pImage->info.ui8BitDepth;
            int mask = (1 << (shift + 1)) - 1;

            for (j = 0; j < pImage->info.ui32Height; j++)
            {
                for (i = 0; i < pImage->info.ui32Width; i++)
                {
                    int out = j*outStride + i * 4,
                        in = j*inStride + i * 4;
                    buffData[out + 0] = (pImage->pBuffer[in + r]) << shift
                        | ((pImage->pBuffer[in + r])
                        >> (pImage->info.ui8BitDepth - shift))&mask;

                    buffData[out + 1] = (pImage->pBuffer[in + g]) << shift
                        | ((pImage->pBuffer[in + g])
                        >> (pImage->info.ui8BitDepth - shift))&mask;

                    buffData[out + 2] = (pImage->pBuffer[in + b]) << shift
                        | ((pImage->pBuffer[in + b])
                        >> (pImage->info.ui8BitDepth - shift))&mask;

                    buffData[out + 3] = 0;  // blank space
                }
            }
        }
        else
        {
            for (j = 0; j < pImage->info.ui32Height; j++)
            {
                for (i = 0; i < pImage->info.ui32Width; i++)
                {
                    int out = j*outStride + i * 4,
                        in = j*inStride + i * 4;
                    buffData[out + 0] = pImage->pBuffer[in + r];

                    buffData[out + 1] = pImage->pBuffer[in + g];

                    buffData[out + 2] = (pImage->pBuffer[in + b]);

                    buffData[out + 3] = 0;  // blank space
                }
            }
        }
    }
    else
    {
        LOG_ERROR("given pImage needs to be RGB\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

/*
 * Converter
 */

IMG_RESULT CI_ConverterConfigure(CI_CONVERTER *pConverter,
    CI_CONV_FMT eFormat, IMG_UINT8 ui8Bitdepth)
{
    if (!pConverter)
    {
        LOG_ERROR("pConverter is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    pConverter->privateData = NULL;
    pConverter->pfnHeaderWriter = NULL;
    pConverter->pfnFooterWriter = NULL;
    pConverter->ui8FormatBitdepth = ui8Bitdepth;

    pConverter->eFormat = eFormat;
    switch (eFormat)
    {
    case CI_DGFMT_PARALLEL:
        if (10 == ui8Bitdepth)
        {
            pConverter->pfnLineConverter = &CI_Convert_Parallel10;
        }
        else if (12 == ui8Bitdepth)
        {
            pConverter->pfnLineConverter = &CI_Convert_Parallel12;
        }
        else
        {
            LOG_ERROR("Unsupported bitpdeth %d for parallel format\n",
                (int)ui8Bitdepth);
            return IMG_ERROR_NOT_SUPPORTED;
        }
        break;

    case CI_DGFMT_MIPI:
    case CI_DGFMT_MIPI_LF:
        pConverter->pfnHeaderWriter = &CI_Convert_MIPI_Header;
        pConverter->pfnLineConverter = &CI_Convert_MIPI_Line;
        pConverter->pfnFooterWriter = &CI_Convert_MIPI_Footer;
        break;

    default:
        LOG_ERROR("Unsupported format %d\n", (int)eFormat);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (CI_DGFMT_MIPI <= eFormat)
    {
        CI_CONV_MIPI_PRIV *pNew = (CI_CONV_MIPI_PRIV*)IMG_CALLOC(1,
            sizeof(CI_CONV_MIPI_PRIV));
        if (!pNew)
        {
            LOG_ERROR("Failed to create MIPI private data\n");
            return IMG_ERROR_MALLOC_FAILED;
        }

        if (CI_DGFMT_MIPI_LF == eFormat)
        {
            pNew->bLongFormat = IMG_TRUE;
        }
        switch (ui8Bitdepth)
        {
        case 10:
            pNew->ui8DataId = DG_MIPI_ID_RAW10;
            pNew->pfnMipiWrite = &MIPI_Write_10;
            break;
        case 12:
            pNew->ui8DataId = DG_MIPI_ID_RAW12;
            pNew->pfnMipiWrite = &MIPI_Write_12;
            break;
        case 14:
            pNew->ui8DataId = DG_MIPI_ID_RAW14;
            pNew->pfnMipiWrite = &MIPI_Write_14;
            break;

        default:
            LOG_ERROR("Unsupported bitpdeth %d for MIPI format\n",
                (int)ui8Bitdepth);
            IMG_FREE(pNew);
            return IMG_ERROR_NOT_SUPPORTED;
        }

        // frame starts at 1
        pNew->ui16FrameNumber = 1;
        pConverter->privateData = pNew;
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_ConverterConvertFrame(CI_CONVERTER *pConverter,
    const struct _sSimImageIn_ *pImage, CI_DG_FRAME *pFrame)
{
    IMG_UINT32 i = 0;
    IMG_UINT32 written = 0, inputStride = 0;
    IMG_UINT8 *pWrite = NULL;
    IMG_UINT32 ui32Stride = 0;
    // if using MIPI format will be casted from private data
    CI_CONV_MIPI_PRIV *pMipiPriv = NULL;

    if (!pConverter || !pImage || !pFrame)
    {
        LOG_ERROR("pConverter or pImage or pFrame is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (!pConverter->pfnLineConverter || !pFrame->data)
    {
        LOG_ERROR("pConverted was not configured properly or pFrame has not" \
            " attached data\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    LOG_PERF("start\n");
    inputStride = pImage->info.ui32Width*sizeof(IMG_UINT16);
    // 1 line means stride
    ui32Stride = CI_ConverterFrameSize(pConverter, pImage->info.ui32Width, 1);
    pWrite = (IMG_UINT8*)pFrame->data;
    pFrame->eFormat = pConverter->eFormat;
    pFrame->ui8FormatBitdepth = pConverter->ui8FormatBitdepth;
    pFrame->ui32Width = pImage->info.ui32Width;
    pFrame->ui32Height = pImage->info.ui32Height;
    // blanking should be configured by the caller
    // pFrame->ui32HorizontalBlanking = 0;
    // pFrame->ui32VerticalBlanking = 0;
    pFrame->ui32Stride = ui32Stride;

    switch (pImage->info.eColourModel)
    {
    case SimImage_RGGB:
        pFrame->eBayerMosaic = MOSAIC_RGGB;
        break;
    case SimImage_GRBG:
        pFrame->eBayerMosaic = MOSAIC_GRBG;
        break;
    case SimImage_GBRG:
        pFrame->eBayerMosaic = MOSAIC_GBRG;
        break;
    case SimImage_BGGR:
        pFrame->eBayerMosaic = MOSAIC_BGGR;
        break;
    default:
        LOG_ERROR("input image needs to be BAYER!\n");
        return IMG_ERROR_FATAL;
    }

    if (ui32Stride*pImage->info.ui32Height > pFrame->ui32AllocSize)
    {
        LOG_ERROR("given pFrame is too small (%d Bytes) to store converted" \
            " data (%dx%d = %d Bytes)\n",
            pFrame->ui32AllocSize, ui32Stride, pImage->info.ui32Height,
            ui32Stride*pImage->info.ui32Height);
        return IMG_ERROR_NOT_SUPPORTED;
    }
    // LOG_PERF("memset frame to 0\n");
    IMG_MEMSET(pFrame->data, 0, pFrame->ui32AllocSize);

    if (CI_DGFMT_MIPI <= pConverter->eFormat)
    {
        pMipiPriv = (CI_CONV_MIPI_PRIV*)pConverter->privateData;

        // LOG_PERF("MIPI compute packet size\n");
        pFrame->ui32PacketWidth =
            pConverter->pfnLineConverter(pConverter->privateData,
            NULL, sizeof(IMG_UINT16),
            pImage->info.ui32Width / 2, NULL);
        /* we need to write the package size WITHOUT the extended size,
         * which adds 8 Bytes when using external data-generator */
        if (pMipiPriv->bLongFormat)
        {
            pFrame->ui32PacketWidth -= 8;
        }
    }

    if (pConverter->pfnHeaderWriter)
    {
        // LOG_PERF("write header\n");
        written = pConverter->pfnHeaderWriter(pConverter->privateData, pWrite);

        pWrite += written;
    }

    // LOG_PERF("write frame\n");
    for (i = 0; i < pImage->info.ui32Height; i++)
    {
        if (pMipiPriv)
        {
            pMipiPriv->i32LineNumber = (IMG_INT32)i + 1;
        }
        written += pConverter->pfnLineConverter(pConverter->privateData,
            &(pImage->pBuffer[i*pImage->info.ui32Width]), sizeof(IMG_UINT16),
            pImage->info.ui32Width / 2, pWrite);

        if (written > ui32Stride)
        {
            LOG_ERROR("incorrect number of bytes written (%d) for line %d" \
                " (stride is %d)\n", written, i, ui32Stride);
            return IMG_ERROR_FATAL;
        }
        written = 0;
        pWrite = (IMG_UINT8*)pFrame->data + (i + 1)*ui32Stride;

#ifdef DEBUG_FIRST_LINE
        if (0 == i)
        {
            FILE *f = fopen("dg_first.dat", "w");
            if ( f != NULL )
            {
                fwrite(pImage->pBuffer, inputStride, sizeof(IMG_UINT8), f);

                fclose(f);
            }
        }
#endif
    }
    // LOG_PERF("frame written\n");

    if (pConverter->pfnFooterWriter)
    {
        // LOG_PERF("write footer\n");
        /* for MIPI pWrite is too far - we need to write the FE packet
         * just after the last line */
        if (CI_DGFMT_MIPI <= pConverter->eFormat)
        {
            written = pConverter->pfnLineConverter(pConverter->privateData,
                NULL, sizeof(IMG_UINT16),
                pImage->info.ui32Width / 2, NULL);
            /* we move of ui32Stride forward while we should have moved by
             * only written */
            pWrite = pWrite - (ui32Stride - written);
        }
        written = pConverter->pfnFooterWriter(pConverter->privateData,
            pWrite);
    }

    LOG_PERF("done\n");
    return IMG_SUCCESS;
}

IMG_UINT32 CI_ConverterFrameSize(const CI_CONVERTER *pConverter,
    IMG_UINT32 ui32Width, IMG_UINT32 ui32Height)
{
    IMG_UINT32 ui32Size = 0;
    IMG_UINT32 ui32Header = 0, ui32Footer = 0;

    if (!pConverter)
    {
        LOG_ERROR("pConverter is NULL\n");
        return 0;
    }

    if (!pConverter->pfnLineConverter)
    {
        LOG_ERROR("pConverted was not configured properly or pFrame has no" \
            " attached data\n");
        return 0;
    }

    // get output stride - /2 because RG GB?
    ui32Size = pConverter->pfnLineConverter(pConverter->privateData,
        NULL, 0, ui32Width / 2, NULL);

    if (pConverter->pfnHeaderWriter)
    {
        ui32Header = pConverter->pfnHeaderWriter(pConverter->privateData,
            NULL);
    }
    if (pConverter->pfnFooterWriter)
    {
        ui32Footer = pConverter->pfnFooterWriter(pConverter->privateData,
            NULL);
    }

    if (1)
    {
        /* we assume the stride will either contain header + line
         * or line + footer therefore not both should be added
         * - this could be an optional behaviour but Parallel has no
         * headers/footers and mipi headers and footers are the same size
         * on different strides (1 line frame is not supported) */
        ui32Size += IMG_MAX_INT(ui32Header, ui32Footer);
    }

    // round up to multiple of memory alignment for stride
    ui32Size =
        (ui32Size + SYSMEM_ALIGNMENT - 1) / SYSMEM_ALIGNMENT *SYSMEM_ALIGNMENT;
    ui32Size *= ui32Height;

    return ui32Size;
}

IMG_RESULT CI_ConverterClear(CI_CONVERTER *pConverter)
{
    if (!pConverter)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (pConverter->privateData && CI_DGFMT_MIPI <= pConverter->eFormat)
    {
        IMG_FREE(pConverter->privateData);
    }

    return IMG_SUCCESS;
}
