/**
******************************************************************************
@file yuvscale.h

@brief common definitions for software scaler in v2500 HAL module

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

#include "img_types.h"

typedef short int SHORT, *PSHORT;        /* assumed to be 16 bit signed integer */
typedef unsigned short WORD;            /* assumed to be 16 bit unsigned integer */
typedef unsigned long DWORD, *PDWORD;    /* assumed to be 32 bit unsigned integer */
typedef unsigned char BYTE, *PBYTE;        /* assumed to be 8 bit unsigned integer */
typedef long LONG;                        /* assumed to be signed integer at least 16 bits */
typedef char CHAR, *PCHAR;                /* whatever type is used for strings */

typedef struct _YUVSCALE_STATE_
{
#if(0)
    char *srcFilename;
    FILE *srcFile;
    char *dstFilename;
    FILE *dstFile;
#endif
    int     srcHeight;
    int     srcWidth;
    int     dstHeight;
    int     dstWidth;

    unsigned char *tmpScalingBuffer;
    unsigned char *inputFrameBuffer;
    unsigned char *outputFrameBuffer;

    IMG_UINT32    aui32HorizInputCoeffs[33];
    IMG_UINT32    aui32HorizCoeffs[8][4];

    int scaleSrcSize;
    int scaleDstSize;
    int scalePitch;
    int scaleInitial;

    //INTERLACE_MODE eInterlaceMode;
}YUVSCALE_STATE;

extern YUVSCALE_STATE g_YUVScaleState;

#define printf(...) __sc_printf(__VA_ARGS__)
extern int __sc_printf(const char* fmt,...);

void ProgramScaler(IMG_UINT32 ui32SourceSize, IMG_UINT32 ui32DestSize);
void HorizScaleLine(PBYTE pSrc, int srcPixelStride, PBYTE pDst, int dstPixelStride);
void ScaleImagePlane(PBYTE pSrc, PBYTE pDst, int srcWidth, int srcHeight, int srcStride, int dstWidth, int dstHeight, int dstStride, PBYTE tmpBuf);
