/**
*******************************************************************************
 @file dg_api_internal.h

 @brief Data Generator structures

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

#ifndef __DG_API_INTERNAL__
#define __DG_API_INTERNAL__

#include "dg/dg_api_structs.h"

#include "sys/sys_userio.h"
#include <linkedlist.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _sSimImageIn_; // include <simimage.h>

/**
 * @defgroup DG_Conv Internal and converter object (DG_Conv)
 * @ingroup DG_API
 * @brief Converter and internal objects
 *
 * Internal objects are used to store more information than user-side needs.
 *
 * The converter transforms FLX images into data-generator ready memory
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the DG_API documentation module
 *---------------------------------------------------------------------------*/

typedef struct INT_CAMERA
{
	int identifier;
	IMG_BOOL8 bStarted;
	DG_CAMERA publicCamera;
	/**
	 * @brief If not NULL the gasket is acquired when starting the capture 
     * and released when stopping - otherwise gasket has to be acquired and
     * released manually
	 *
	 * @warning This object is not owned by the Pipeline - it is NOT 
     * destroyed when destroying the pipeline
	 *
	 * @see CI_GasketAcquired() CI_GasketRelease()
	 */
	struct CI_GASKET *pGasket;
     /** @brief connection used for the registration of the gasket */
    struct CI_CONNECTION *pCIConnection;
    /** @brief List of memory mapped frames (INT_FRAME*) */
	sLinkedList_T sList_frames;
} INT_CAMERA;

typedef struct INT_FRAME
{
	unsigned int identifier;
	IMG_SIZE uiStride;
	IMG_SIZE uiHeight;

	void *pImage;
} INT_FRAME;

typedef struct INT_DATAGEN
{
	SYS_FILE *fileDesc;
	DG_HWINFO sHWinfo;

	sLinkedList_T sList_cameras;
} INT_DATAGEN;

 /**
 * @brief convert bayer data from individual pixel values to in-memory packed
 * format (as specified in the TRM, i.e. 3 10-bit values per 4 bytes)
 *
 * @note Adapted from simulator code
 *
 * We refer to this as "BT656" format, because BT656 is a parallel bus and
 * we only store raw data in memory (no control signals)
 * @param inPixels0 row of Bayer pixels for channel at even and odd columns
 * (i.e. 1 pointer per R,G or B channel)
 * @param inPixels1 see inPixels0
 * @param stride0 nb of IMG_UINT16s to skip to get to the next pixel value
 * (in inPixels0,1 arrays)
 * @param stride1 see stride0
 * @param NpixelsPerCh number of Bayer values in each channel 
 * (in inPixels0,1 arrays)
 * @param outPixels buffer to write to; if NULL the function only returns
 * number of bytes that would have been written.
 * The output data will be written to outPixels
 *
 * @return nb of bytes written to outPixels[]
 */
IMG_SIZE DG_ConvBayerToBT656_10(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

/**
 * @brief convert bayer data from individual pixel values to in-memory packed
 * format (as specified in the TRM, i.e. 2 12-bit values per 3 bytes)
 *
 * @note Adapted from simulator code
 *
 * otherwise same interface as  ConvPixFmtBayerToMemBT656_10()
 *
 * 2 pixels are packed in 3 bytes as:
 * p1 | p2<<12
 */
IMG_SIZE DG_ConvBayerToBT656_12(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

/**
 * @brief convert bayer data from individual pixel values to in-memory packed
 * format representing RAW10 MIPI packets
 *
 * @note Adapted from simulator code
 *
 * otherwise same interface as  ConvPixFmtBayerToMemBT656_10()
 * based on MIPI spec, each line of pixels is a single packet and in RAW10 it
 * must be a multiple of 4 pixels (5 bytes)
 *
 * 4 pixels are packed in 5 bytes as:
 * (p1>>2),(p2>>2),(p3>>2),(p4>>2),(p1&3)|((p2&3)<<2)|((p3&3)<<4)|((p4&3)<<6)
 */
IMG_SIZE DG_ConvBayerToMIPI_RAW10(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

/**
 * @brief convert bayer data from individual pixel values to in-memory packed
 * format representing RAW12 MIPI packets
 *
 * @note Adapted from simulator code
 *
 * otherwise same interface as  ConvPixFmtBayerToMemBT656_10()
 * based on MIPI spec, each line of pixels is a single packet and in RAW12 it
 * must be a multiple of 2 pixels (3 bytes)
 *
 * 2 pixels are packed in 3 bytes as:
 * (p1>>4),(p2>>4),(p1&0xF)|((p2&0xF)<<4)
 */
IMG_SIZE DG_ConvBayerToMIPI_RAW12(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

/**
 * @brief convert bayer data from individual pixel values to in-memory packed
 * format representing RAW14 MIPI packets
 *
 * @note Adapted from simulator code
 *
 * otherwise same interface as  ConvPixFmtBayerToMemBT656_10()
 *
 * based on MIPI spec, each line of pixels is a single packet and in RAW14 it
 * must be a multiple of 4 pixels (7 bytes).
 * 
 * 4 pixels are packed in 7 bytes as:
 * (p1>>6),(p2>>6),(p3>>6),(p4>>6), and a 24-bit int 
 * (p1&0x3F)|((p2&0x3F)<<6)|((p3&0x3F)<<12)|((p4&0x3F)<<18)
 */
IMG_SIZE DG_ConvBayerToMIPI_RAW14(const IMG_UINT16 *inPixels0,
    const IMG_UINT16 *inPixels1, IMG_UINT32 stride0, IMG_UINT32 stride1,
    IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

/**
 * @}
 */

/**
 * @name Format Convertion: MIPI packet creation
 * @{
 */

/**
 * @brief Get the MIPI packet CRC data
 *
 * @note Adapted from simulator code
 */
IMG_UINT16 DG_ConvMIPIGetDataCRC(const IMG_UINT8 *dataBytes,
    IMG_UINT32 Nbytes);

/**
 * @brief Get the header's data Error Correction Code byte
 *
 * @note Adapted from simulator code
 */
IMG_UINT8 DG_ConvMIPIGetHeaderECC(const IMG_UINT8 *data3);

/**
 * @brief writes a MIPI packet header (=same as short MIPI packet)
 *
 * @note Adapted from simulator code
 * 
 * @return Number of bytes written
 */
IMG_SIZE DG_ConvMIPIWriteHeader(IMG_UINT8 *pMipi, IMG_INT8 virtualChannel,
    IMG_INT8 dataId, IMG_UINT16 ui16Data);

/**
 * @}
 */

/**
 * @name Format Convertion object
 * @{
 */

/**
 * @brief Wrap raw pixel data into a MIPI or BT656 packet - converts bayer
 * into needed format
 *
 * @param pConvInfo
 * @param pInput Bayer formated
 * @param Nbytes
 * @param pOutput
 * @param eFrameType
 * @param fTmp if not NULL will save converted frame into that file
 *
 * @returns final number of bytes written (=Nbytes + any MIPI format overhead)
 */
IMG_SIZE DG_ConvWriteLine(DG_CONV *pConvInfo, const IMG_UINT16 *pInput,
    IMG_UINT32 Nbytes, IMG_UINT8 *pOutput, eDG_MIPI_FRAMETYPE eFrameType,
    FILE *fTmp);

/**
 * @brief Computes the size of a converted frame using MIPI - if NRow is 
 * 1 the frame sizes misses 4 Bytes (FE or FS explanation)
 *
 * @Input pConvInfo
 * @Input ui32NPixels Width in Pixels
 *
 * the actual needed memory is (FS+FE) + NRow *( lineFlag*(LS+LE)+
 * DATA_HEADER+DATA+CRC) but:
 * @li all headers are the same size FS=FE=LS=LE=DATA_HEADER
 * @li the DG in hardware wants a fixed stride, thus the FS and FE are
 * assumed to be always present
 * @li a frame cannot be 1 line long, therefore only FS or FE is present at
 * a time
 *
 * The computation is then: NRow * (H + lineFlag*(2*H)+ H+DATA+CRC)
 */
IMG_SIZE DG_ConvGetConvertedSize(const DG_CONV *pConvInfo,
    IMG_UINT32 ui32NPixels);
IMG_SIZE DG_ConvGetConvertedStride(const DG_CONV *pConvInfo,
    IMG_UINT32 ui32NPixels);

/**
 * @brief Point of entry of the converter
 */
IMG_RESULT DG_ConvSimImage(DG_CONV *pConvInfo,
    const struct _sSimImageIn_ *pImage, IMG_UINT8 *pOutput,
    IMG_UINT32 ui32Stride, const char *pszTmpSave);

/**
 * @}
 */
#define DG_LOG_TAG "DG_UM"

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the DG_INT documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __DG_API_INTERNAL__
