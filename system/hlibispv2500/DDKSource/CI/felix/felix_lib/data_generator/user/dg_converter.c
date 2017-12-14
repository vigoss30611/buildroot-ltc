/**
*******************************************************************************
 @file dg_converter.c

 @brief User-side conversion of an FLX image to Data Generator ready format

 @note This was partially taken and adapted from the internal driver of the
 simulator

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

#include "dg/dg_api.h"
#include "dg/dg_api_internal.h"
#include <sim_image.h>

#define LOG_TAG DG_LOG_TAG
#include <felixcommon/userlog.h>

/** 
 * @brief #of bytes in a MIPI packet header (also an entire short packet)
 */
#define MIPI_HDR_SIZE 4
/** @brief #of bytes at the end of a long MIPI packet (CRC) */
#define MIPI_FTR_SIZE 2

/** @brief round up 'x' to multiple of 'of' */
#define RoundUp(x,of) ( (((x) + (of) - 1) / (of)) * (of) )
/** @brief divide 'x' by 'of' and round up */
#define RoundUpDiv(x,of) ( ((x) + (of) - 1) / (of) )

/**
 * @brief just check whether all pixels are in valid range, no real
 * functionality
 */
static void CheckPixelRange(const IMG_UINT16 *inPixels, IMG_UINT32 stride,
    IMG_UINT32 Npixels, IMG_UINT32 bitsPerPixel)
{
	IMG_UINT32 x;
	for (x = 0; x < Npixels; x++)
	{
		IMG_UINT32 pix;
		pix = inPixels[x*stride];
		if (!(pix < (IMG_UINT32)(1<<bitsPerPixel)))
		{
		    LOG_WARNING("pixel %d value 0x%X out of %d-bit range (is input "\
                "image compatible with selected gasket?)\n", x, pix,
                bitsPerPixel);
		}
	}
}

IMG_SIZE DG_ConvBayerToBT656_10(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
	IMG_UINT32 *wr;
	IMG_SIZE Nwritten;
	//calculate number of bytes that will/would be written
	Nwritten = (NpixelsPerCh / 3) * 8;	//8 output bytes per 3 input Bayer cells
	if ((NpixelsPerCh % 3) > 1) Nwritten += 8;
	else if (NpixelsPerCh % 3) Nwritten += 4;
	//actually convert the pixels
	if (outPixels != NULL)
	{
		CheckPixelRange(inPixels0,stride0,NpixelsPerCh,10);
		CheckPixelRange(inPixels1,stride1,NpixelsPerCh,10);
		wr = (IMG_UINT32*)outPixels;
		for (;NpixelsPerCh >= 3;NpixelsPerCh -= 3)
		{
			*wr++ = (inPixels0[0*stride0] & 0x3ff) 
                | ((inPixels1[0*stride1] & 0x3ff) << 10)
                | ((inPixels0[1*stride0] & 0x3ff) << 20);
			*wr++ = (inPixels1[1*stride1] & 0x3ff)
                | ((inPixels0[2*stride0] & 0x3ff) << 10)
                | ((inPixels1[2*stride1] & 0x3ff) << 20);
			inPixels0 += 3*stride0;
			inPixels1 += 3*stride1;
		}
		if (NpixelsPerCh > 0)
		{
			*wr++ = (inPixels0[0*stride0] & 0x3ff)
					| ((inPixels1[0*stride1] & 0x3ff) << 10)
					| ((NpixelsPerCh > 1) ?
					((inPixels0[1*stride0] & 0x3ff) << 20) : 0);
			if (NpixelsPerCh > 1)
				*wr++ = (inPixels1[1*stride1] & 0x3ff)
						| ((NpixelsPerCh > 2) ?
						((inPixels0[2*stride0] & 0x3ff) << 10) : 0)
						| ((NpixelsPerCh > 2) ?
						((inPixels1[2*stride1] & 0x3ff) << 20) : 0);
		}
		IMG_ASSERT(Nwritten == (IMG_UINT8*)wr - outPixels);
	}
	return Nwritten;
}

IMG_SIZE DG_ConvBayerToBT656_12(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
	IMG_UINT8 *wr;
	IMG_SIZE Nwritten;
	//calculate number of bytes that will/would be written
	Nwritten = NpixelsPerCh * 3;	//3 output bytes per 1 input Bayer cell
	//actually convert the pixels
	if (outPixels != NULL)
	{
		CheckPixelRange(inPixels0,stride0,NpixelsPerCh,12);
		CheckPixelRange(inPixels1,stride1,NpixelsPerCh,12);
		wr = outPixels;
		while (NpixelsPerCh--)
		{
			*wr++ = inPixels0[0] & 0xff;
			*wr++ = (inPixels0[0] >> 8) | ((inPixels1[0] & 0xf) << 4);
			*wr++ = inPixels1[0] >> 4;
			inPixels0 += stride0;
			inPixels1 += stride1;
		}
		IMG_ASSERT(Nwritten == wr - outPixels);
	}
	return Nwritten;
}

IMG_SIZE DG_ConvBayerToMIPI_RAW10(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
	IMG_UINT8 *wr;
	IMG_SIZE NpixelGroups,Nwritten;
	//calculate number of bytes that will/would be written
    /* #of groups of 4 pixels (2 channels accumulated) */
	NpixelGroups = RoundUpDiv((NpixelsPerCh * 2),4);
	Nwritten = NpixelGroups * 5;	//5 bytes per group of 4 pixels
	//actually convert the pixels
	if (outPixels != NULL)
	{
		CheckPixelRange(inPixels0,stride0,NpixelsPerCh,10);
		CheckPixelRange(inPixels1,stride1,NpixelsPerCh,10);
		wr = outPixels;
        //*2 channels = 4 pixels per loop
		for (;NpixelsPerCh >= 2;NpixelsPerCh -= 2)
		{
			*wr++ = (inPixels0[0*stride0] >> 2);
			*wr++ = (inPixels1[0*stride1] >> 2);
			*wr++ = (inPixels0[1*stride0] >> 2);
			*wr++ = (inPixels1[1*stride1] >> 2);
			*wr++ = ((inPixels0[0*stride0] & 3) << 0) 
                | ((inPixels1[0*stride1] & 3) << 2)
                | ((inPixels0[1*stride0] & 3) << 4)
                | ((inPixels1[1*stride1] & 3) << 6);
			inPixels0 += 2*stride0;
			inPixels1 += 2*stride1;
		}
		if (NpixelsPerCh > 0)
		{
			*wr++ = (inPixels0[0*stride0] >> 2);
			*wr++ = (inPixels1[0*stride1] >> 2);
			*wr++ = 0;
			*wr++ = 0;
			*wr++ = ((inPixels0[0*stride0] & 3) << 0)
                | ((inPixels1[0*stride1] & 3) << 2);
		}
		IMG_ASSERT(Nwritten == wr - outPixels);
	}
	return Nwritten;
}

IMG_SIZE DG_ConvBayerToMIPI_RAW12(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
	IMG_UINT8 *wr;
	IMG_SIZE NpixelGroups,Nwritten;
	//calculate number of bytes that will/would be written
    //#of groups of 2 pixels (2 channels accumulated)
	NpixelGroups = NpixelsPerCh;
	Nwritten = NpixelGroups * 3;	//3 bytes per group of 2 pixels
	//actually convert the pixels
	if (outPixels != NULL)
	{
		CheckPixelRange(inPixels0,stride0,NpixelsPerCh,12);
		CheckPixelRange(inPixels1,stride1,NpixelsPerCh,12);
		wr = outPixels;
        //*2 channels = 2 pixels per loop
		for (;NpixelsPerCh > 0;NpixelsPerCh --)
		{
			*wr++ = (inPixels0[0*stride0] >> 4);
			*wr++ = (inPixels1[0*stride1] >> 4);
			*wr++ = ((inPixels0[0*stride0] & 0xf) << 0)
                | ((inPixels1[0*stride1] & 0xf) << 4);
			inPixels0 += stride0;
			inPixels1 += stride1;
		}
		IMG_ASSERT(Nwritten == wr - outPixels);
	}
	return Nwritten;
}

IMG_SIZE DG_ConvBayerToMIPI_RAW14(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels)
{
	IMG_UINT8 *wr;
	IMG_SIZE NpixelGroups,Nwritten,tmp;

	//calculate number of bytes that will/would be written
    //#of groups of 4 pixels (2 channels accumulated)
	NpixelGroups = RoundUpDiv((NpixelsPerCh * 2),4);
	Nwritten = NpixelGroups * 7;	//7 bytes per group of 4 pixels

	//actually convert the pixels
	if (outPixels != NULL)
	{
		CheckPixelRange(inPixels0,stride0,NpixelsPerCh,14);
		CheckPixelRange(inPixels1,stride1,NpixelsPerCh,14);
		wr = outPixels;
        //*2 channels = 4 pixels per loop
		for (;NpixelsPerCh >= 2;NpixelsPerCh -= 2)
		{
			*wr++ = (inPixels0[0*stride0] >> 6);
			*wr++ = (inPixels1[0*stride1] >> 6);
			*wr++ = (inPixels0[1*stride0] >> 6);
			*wr++ = (inPixels1[1*stride1] >> 6);
			tmp = ((inPixels0[0*stride0] & 0x3f) << 0)
                | ((inPixels1[0*stride1] & 0x3f) << 6)
                | ((inPixels0[1*stride0] & 0x3f) << 12)
                | ((inPixels1[1*stride1] & 0x3f) << 18);
			*wr++ = ((IMG_INT8*)&tmp)[0];
			*wr++ = ((IMG_INT8*)&tmp)[1];
			*wr++ = ((IMG_INT8*)&tmp)[2];
			inPixels0 += 2*stride0;
			inPixels1 += 2*stride1;
		}
		if (NpixelsPerCh > 0)
		{
			*wr++ = (inPixels0[0*stride0] >> 6);
			*wr++ = (inPixels1[0*stride1] >> 6);
			*wr++ = 0;
			*wr++ = 0;
			tmp = ((inPixels0[0*stride0] & 0x3f) << 0)
                | ((inPixels1[0*stride1] & 0x3f) << 6);
			*wr++ = ((IMG_INT8*)&tmp)[0];
			*wr++ = ((IMG_INT8*)&tmp)[1];
			*wr++ = ((IMG_INT8*)&tmp)[2];
		}
		IMG_ASSERT(Nwritten == wr - outPixels);
	}
	return Nwritten;
}

IMG_UINT16 DG_ConvMIPIGetDataCRC(const IMG_UINT8 *dataBytes,
    IMG_UINT32 Nbytes)
{
	IMG_UINT16 crc,poly;
	IMG_UINT32 bit;
	crc = 0xffff;
	poly = 0x8408;

	for (bit = 0; bit < Nbytes*8; bit++)
	{
		if ((crc & 1) ^ ((dataBytes[bit >> 3] >> (bit & 7)) & 1))
			crc = (crc >> 1) ^ poly;
		else
			crc >>= 1;
	}

	return crc;
}

IMG_UINT8 DG_ConvMIPIGetHeaderECC(const IMG_UINT8 *data3)
{
	static const char *ECCBits[] =
	{	//CSI-2.pdf (MIPI spec) p52-53
        /** for ECC bit 0 - XOR these bits of 24-bit data (LSBit->MSBit) to
         * get ECC bit 0 */
		"111011010011010010001111",
		"110110101010101001001111", //for ECC bit 1
		"101101100101100100101110", //for ECC bit 2
		"011100011100011100011101", //for ECC bit 3
		"000011111100000011111011", //for ECC bit 4
		"000000000011111111110111", //for ECC bit 5
		"000000000000000000000000", //for ECC bit 6
		"000000000000000000000000", //for ECC bit 7
	};
	IMG_INT eccBit,dataBit,bitVal;
	IMG_UINT8 ecc;
	ecc = 0;	//the ECC byte
	for (eccBit = 0; eccBit < 8; eccBit++)
	{
		bitVal = 0;	//value of the eccBit-th bit of the ECC (0 or 1)
		for (dataBit = 0; dataBit < 24; dataBit++)
			bitVal ^= (data3[dataBit/8] >> (dataBit%8)) 
                & (ECCBits[eccBit][dataBit] - '0');
		ecc |= (bitVal << eccBit);
	}
	return ecc;
}

IMG_SIZE DG_ConvMIPIWriteHeader(IMG_UINT8 *pBuf, IMG_INT8 virtualChannel,
    IMG_INT8 dataId, IMG_UINT16 ui16Data)
{
	IMG_UINT8 *wr;
	IMG_SIZE ui32ToWrite = 4;

	if ( pBuf != NULL )
	{
		wr = pBuf;
		*wr++ = (virtualChannel<<6) | (dataId & 0x3f);
		*(IMG_UINT16*)wr = ui16Data;
		wr += sizeof(IMG_UINT16); // data size
		*wr++ = DG_ConvMIPIGetHeaderECC(pBuf);	//the ECC byte

		IMG_ASSERT(ui32ToWrite == (wr-pBuf));
	}

	return ui32ToWrite;
}

IMG_SIZE DG_ConvWriteLine(DG_CONV *pConvInfo, const IMG_UINT16 *pInput,
    IMG_UINT32 Nbytes, IMG_UINT8 *pOutput, eDG_MIPI_FRAMETYPE eFrameType,
    FILE *fTmp)
{
	IMG_SIZE uiSize = 0, uiConvNBytes = 0, written = 0;
	IMG_UINT8 ui8VirtualChannel = 0, ui8IBPP; // InputBytesPerPixel
	IMG_UINT8 *pWrite = pOutput;
	IMG_UINT16 ui16Crc;

	if ( pInput == NULL || pOutput == NULL || Nbytes == 0 )
	{
		LOG_ERROR("pInput or pOuput is NULL or NBytes is 0\n");
		return 0;
	}
	
	/*
	 * input is done from an IMG_UINT16 buffer
	 */
	ui8IBPP = sizeof(IMG_UINT16);

	if ( eFrameType == DG_MIPI_FRAMESTART )
	{
		pConvInfo->ui32FrameNumber++;
		if ( pConvInfo->eFormat >= DG_FMT_MIPI )
		{
			written += DG_ConvMIPIWriteHeader(pWrite, ui8VirtualChannel,
                DG_MIPI_ID_FS, pConvInfo->ui32FrameNumber);
            uiSize += written;
            
            if (fTmp)
            {
                fwrite(pWrite, 1, written, fTmp);
            }
            
			pWrite = pOutput+uiSize;
		}
	}

	pConvInfo->ui32LineNumber++;
	if ( pConvInfo->eFormat == DG_FMT_MIPI_LF )
	{
		written = DG_ConvMIPIWriteHeader(pWrite, ui8VirtualChannel,
            DG_MIPI_ID_LS, pConvInfo->ui32LineNumber);
        uiSize += written;
        
        if (fTmp)
        {
            fwrite(pWrite, 1, written, fTmp);
        }
        
		pWrite = pOutput+uiSize;
	}

	if ( pConvInfo->eFormat >= DG_FMT_MIPI )
	{
		// get the number of bytes needed
		uiConvNBytes = pConvInfo->pfnConverter(NULL, NULL, ui8IBPP, ui8IBPP,
            (Nbytes/ui8IBPP)/2, NULL);

		// write packet header
		written = DG_ConvMIPIWriteHeader(pWrite, ui8VirtualChannel,
            pConvInfo->eMIPIFormat, uiConvNBytes);
        uiSize += written;
        
        if (fTmp)
        {
            fwrite(pWrite, 1, written, fTmp);
        }
        
		pWrite = pOutput+uiSize;
	}

	// convert data
	written = pConvInfo->pfnConverter(&pInput[0], &pInput[1], ui8IBPP,
        ui8IBPP, (Nbytes/ui8IBPP)/2, pWrite);
    uiSize += written;
    
    if (fTmp)
    {
        fwrite(pWrite, 1, written, fTmp);
    }
    
	// do not move yet: MIPI CRC needs the data

	if ( pConvInfo->eFormat >= DG_FMT_MIPI )
	{
		// CRC
		ui16Crc = DG_ConvMIPIGetDataCRC(pWrite, uiConvNBytes);

		// move away from written data
		pWrite = pOutput+uiSize;
		// write crc after data
		*(IMG_UINT16*)pWrite = ui16Crc;
        written = DG_MIPI_SIZEOF_CRC;
		uiSize += written;
        
        if (fTmp)
        {
            fwrite(pWrite, 1, written, fTmp);
        }
        
		pWrite = pOutput+uiSize;
	}
	//else BT656
	// move away from written data is useless in BT656 - no more writting
	//pWrite = pOutput+ui32Size;

	if ( pConvInfo->eFormat == DG_FMT_MIPI_LF )
	{
		written = DG_ConvMIPIWriteHeader(pWrite, ui8VirtualChannel,
            DG_MIPI_ID_LE, pConvInfo->ui32LineNumber);
        uiSize += written;
        
        if (fTmp)
        {
            fwrite(pWrite, 1, written, fTmp);
        }
        
		pWrite = pOutput+uiSize;
	}

	if ( pConvInfo->eFormat >= DG_FMT_MIPI && eFrameType == DG_MIPI_FRAMEEND )
	{
		written = DG_ConvMIPIWriteHeader(pWrite, ui8VirtualChannel,
            DG_MIPI_ID_FE, pConvInfo->ui32FrameNumber);
        uiSize += written;
        
        if (fTmp)
        {
            fwrite(pWrite, 1, written, fTmp);
        }
	}
	
	return uiSize;
}

/*
 * compute the converted size
 * for BT656 it is the same than the stride
 * for MIPI it is the size of the DATA packet (the stride is bigger because
 * it has more information)
 */
IMG_SIZE DG_ConvGetConvertedSize(const DG_CONV *pConvInfo,
    IMG_UINT32 ui32NPixels)
{
	IMG_SIZE uiSize = 0;

	if ( pConvInfo == NULL || pConvInfo->pfnConverter == NULL )
	{
		return 0;
	}

	if ( pConvInfo->eFormat == DG_FMT_BT656 )
	{
		uiSize = pConvInfo->pfnConverter(NULL, NULL, 0, 0, ui32NPixels/2,
            NULL);
	}
	else
	{
		uiSize = DG_ConvMIPIWriteHeader(NULL, 0, 0, 0)
		  + pConvInfo->pfnConverter(NULL, NULL, 0, 0, ui32NPixels/2, NULL)
		  + DG_MIPI_SIZEOF_CRC;
	}
	return uiSize;
}

/*
 * stride in MIPI is the biggest possible packet(i.e. a FRAME START or a
 * FRAME END packet
 */
IMG_SIZE DG_ConvGetConvertedStride(const DG_CONV *pConvInfo,
    IMG_UINT32 ui32NPixels)
{
	IMG_SIZE uiSize = 0;

	if ( pConvInfo == NULL || pConvInfo->pfnConverter == NULL )
	{
		return 0;
	}

	if ( pConvInfo->eFormat == DG_FMT_BT656 )
	{
		uiSize = DG_ConvGetConvertedSize(pConvInfo, ui32NPixels);
	}
	else
	{
		IMG_UINT32 ui32HeaderSize = DG_ConvMIPIWriteHeader(NULL, 0, 0, 0);
		IMG_UINT8 bMIPILineFlags = pConvInfo->eFormat == DG_FMT_MIPI_LF ?
            1 : 0;
		/* the biggest possible line in MIPI is a line with:
		 * Frame Start + [line start] + (header + data + crc) + [line end]
		 * or [line start] + (header + data + crc) + [line end] + Frame End
		 * Frame Start, line start, line end and Frame end are the same size
         * than header
		 * line start and line end are optional
		 * (header + data + crc) is the data packet size
         */

		uiSize = ui32HeaderSize + (bMIPILineFlags * 2*ui32HeaderSize)
            + DG_ConvGetConvertedSize(pConvInfo, ui32NPixels);

	}
	return uiSize;
}

IMG_RESULT DG_ConvSimImage(DG_CONV *pConvInfo, const sSimImageIn *pImage,
    IMG_UINT8 *pOutput, IMG_UINT32 ui32Stride, const char *pszTmpSave)
{
	IMG_UINT32 i = 0;
	IMG_UINT32 written, inputStride;
	IMG_UINT8 *pTmp = NULL;
	IMG_UINT8 *pWrite = NULL;
    FILE *fTmp = NULL;

	if ( pConvInfo == NULL || pImage == NULL || pOutput == NULL 
        || ui32Stride == 0)
	{
		LOG_ERROR("pConvInfo or pImage or pOutput is NULL or ui32Stride "\
            "is 0\n");
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	inputStride = pImage->info.ui32Width*sizeof(IMG_UINT16);

    if (pszTmpSave)
    {
        fTmp = fopen(pszTmpSave, "wb");
        if (!fTmp)
        {
            LOG_WARNING("failed to open '%s' to save converted result\n",
                pszTmpSave);
        }
    }

	pTmp = (IMG_UINT8*)IMG_CALLOC(ui32Stride*pImage->info.ui32Height,
        sizeof(IMG_UINT8));
	if ( pTmp == NULL )
	{
        if (fTmp)
        {
            fclose(fTmp);
        }
		return IMG_ERROR_FATAL;
	}
	pWrite = pTmp;

	written = DG_ConvWriteLine(pConvInfo, pImage->pBuffer, inputStride,
        pWrite, DG_MIPI_FRAMESTART, fTmp);
	if (written > ui32Stride || written == 0)
	{
		LOG_ERROR("incorrect number of bytes written (%d) for line %d "\
            "(stride is %d)\n", i, written, ui32Stride);
		IMG_FREE(pTmp);
        if (fTmp)
        {
            fclose(fTmp);
        }
		return IMG_ERROR_FATAL;
	}
	pWrite+=ui32Stride;

	for ( i = 1 ; i < pImage->info.ui32Height-1 ; i++ )
	{
		written = DG_ConvWriteLine(pConvInfo,
            &(pImage->pBuffer[i*pImage->info.ui32Width]), inputStride,
            pWrite, DG_MIPI_FRAME, fTmp);

		if (written > ui32Stride)
		{
			LOG_ERROR("incorrect number of bytes written (%d) for line %d "\
                "(stride is %d)\n", i, written, ui32Stride);
			IMG_FREE(pTmp);
            if (fTmp)
            {
                fclose(fTmp);
            }
			return IMG_ERROR_FATAL;
		}
		pWrite+=ui32Stride;
	}

	written = DG_ConvWriteLine(pConvInfo,
        &(pImage->pBuffer[i*pImage->info.ui32Width]), inputStride, pWrite,
        DG_MIPI_FRAMEEND, fTmp);
	if (written > ui32Stride)
	{
		LOG_ERROR("incorrect number of bytes written (%d) for line %d "\
            "(stride is %d)\n", i, written, ui32Stride);
		IMG_FREE(pTmp);
        if (fTmp)
        {
            fclose(fTmp);
        }
		return IMG_ERROR_FATAL;
	}
	pWrite+=ui32Stride;

	IMG_MEMCPY(pOutput, pTmp, ui32Stride*pImage->info.ui32Height);
	IMG_FREE(pTmp);
    if (fTmp)
    {
        fclose(fTmp);
    }

	return IMG_SUCCESS;
}
