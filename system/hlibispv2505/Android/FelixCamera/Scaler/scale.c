/**************************************************************************
 * Name         :
 * Title        :
 * Author       : PowerVR
 * Created      : 03/07/03
 *
 * Copyright    : 2003 by Imagination Technologies. All rights reserved.
 *              : No part of this software, either material or conceptual
 *              : may be copied or distributed, transmitted, transcribed,
 *              : stored in a retrieval system or translated into any
 *              : human or computer language in any form by any means,
 *              : electronic, mechanical, manual or other-wise, or
 *              : disclosed to third parties without the express written
 *              : permission of Imagination Technologies Limited, Unit 8,
 *              : HomePark Industrial Estate, King's Langley, Hertfordshire,
 *              : WD4 8LZ, U.K.
 *
 * Description  : Scaler unit
 *
 * Platform     :
 *

 Version         : $Revision: 1.1 $

 Modifications    :

 $Log: scale.c,v $
 Revision 1.1  2006/03/03 15:48:24  ka
 Tool for scaling multiframe 4:2:0 YUV files

 */

#include <stdlib.h>
#include <memory.h>
#include "yuvscale.h"
#include "sccoef.h"

#define INTERMEDIATE_PRECISION 10


/***********************************************************************************
 Function Name      : Read4Coeffs
 Inputs             : none
 Outputs            : none
 Returns            : Error code
 Description        :
************************************************************************************/
static void Read4Coeffs( IMG_UINT32 *puData, IMG_UINT32 uReg )
{
    puData[0] = (uReg & 0xff);
    puData[1] = (uReg >> 8) & 0xff;
    puData[2] = (uReg >> 16) & 0xff;
    puData[3] = (uReg >> 24) & 0xff;
}

/***********************************************************************************
 Function Name      : SignExtend
 Inputs             : none
 Outputs            : none
 Returns            : Error code
 Description        :
************************************************************************************/
static IMG_INT32 SignExtend(IMG_INT32 i32Data, IMG_INT32 i32Size)
{
    IMG_INT32 retval = 0;

    if(i32Data & (1<<(i32Size-1)))
    {
        retval = i32Data | (0xffffffff << (i32Size-1));
    }
    else
    {
        retval = i32Data;
    }

    return retval;
}
/***********************************************************************************
 Function Name      : Read4Coeffs
 Inputs             : none
 Outputs            : none
 Returns            : Error code
 Description        :
************************************************************************************/
static void ConvertScalerCoeffsToInternalFormat()
{
    IMG_UINT32 i, j;

    /* mirror the horiz coeffs */
    for(i = 0; i < 16; i++)
    {
        g_YUVScaleState.aui32HorizInputCoeffs[16 + i] = g_YUVScaleState.aui32HorizInputCoeffs[16 - i];
    }

    /* organise into phases / taps -- note the +2 to fit our 4-tap filter into the 8-tap code */
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 8; j++)
        {
            g_YUVScaleState.aui32HorizCoeffs[j][i] = g_YUVScaleState.aui32HorizInputCoeffs[(3 - i) * 8 + j];
        }
    }
}
/***********************************************************************************
 Function Name      : Read4Coeffs
 Inputs             : none
 Outputs            : none
 Returns            : Error code
 Description        :
************************************************************************************/
static IMG_INT16 FinalFirRound( IMG_INT32 x, IMG_INT32 nRnd )
{
    x = (x + (1 << (nRnd - 1))) >> nRnd;

    if (x > 1023)
    {
        x = 1023;
    }

    if (x < 0)
    {
        x = 0;
    }

    return (IMG_INT16)x;
}

/***********************************************************************************
 Function Name      : Read4Coeffs
 Inputs             : none
 Outputs            : none
 Returns            : Error code
 Description        :
************************************************************************************/
static IMG_INT32 FinalFirRoundUV( IMG_INT32 x, IMG_INT32 nRnd )
{
    x = (x + (1 << (nRnd - 1))) >> nRnd;

    if (x > 511)
    {
        x = 511;
    }

    if (x < -512)
    {
        x = -512;
    }

    return x;
}

/***********************************************************************************
 Function Name      : FirAccumulate3
 Inputs             : none
 Outputs            : none
 Returns            : Error code
 Description        :
************************************************************************************/
static IMG_INT32 FirAccumulate3( IMG_INT32 nAcc, IMG_INT32 nPixData, IMG_INT32 nCoeff )
{
    IMG_INT32 x;

    x = (nPixData * SignExtend( nCoeff, 8 )) >> 3;

    return nAcc + x;
}

/***********************************************************************************
 Function Name      : ClampX
 Inputs             : none
 Outputs            : none
 Returns            : Error code
 Description        :
************************************************************************************/
static IMG_INT32 ClampX( IMG_INT32 i32Value)
{
    if(i32Value<0)
    {
        i32Value = 0;
    }

    if(i32Value >= (IMG_INT32)g_YUVScaleState.scaleSrcSize)
    {
        i32Value = g_YUVScaleState.scaleSrcSize-1;
    }

    return i32Value;
}



void HorizScaleLine(PBYTE pSrc, int srcPixelStride, PBYTE pDst, int dstPixelStride)
{
    IMG_UINT32    ui32XPos, ui32SrcX, ui32ScaledWidth, ui32UnscaledWidth;
    IMG_UINT32    ui32HPitch, ui32HInitial, ui32Phase, ui32IntX, ui32Sum;
    IMG_UINT32    ui32SrcPixel, i, ui32DstIndex = 0;

    ui32ScaledWidth = g_YUVScaleState.scaleDstSize;

    ui32UnscaledWidth = g_YUVScaleState.scaleSrcSize;

    ui32HPitch = g_YUVScaleState.scalePitch;

    ui32HInitial = g_YUVScaleState.scaleInitial;

    for(ui32XPos = 0; ui32XPos < ui32ScaledWidth ; ui32XPos++)
    {
        ui32SrcX = ui32XPos * ui32HPitch + ui32HInitial;
        ui32SrcX -= (2 << 11);            /* This is to match the hware */
        ui32Phase = (ui32SrcX & 0x7ff) >> 8;
        ui32IntX = (ui32SrcX >> 11); // + psEnv->uHorizClip;
        ui32Sum = 0;

#if 1
        if(ui32IntX >= ui32UnscaledWidth)
        {
            printf("Not enough input pixels for the specified scale\n");
        }
#endif

        for(i = 0; i < 4; i++)
        {
            /* read src pixel at ui32IntX */
            ui32SrcPixel = (*(pSrc + ClampX(ui32IntX + i - 1)*srcPixelStride))<<3;
            //psEnv->psUpstream->pfn( psEnv->psUpstream, ClampX( psEnv->uClampWidth, false, nIntX + i - 1 ), nY, &sPixel, pbValid );
            //psEnv->psUpstream->pfn( psEnv->psUpstream, ClampX( psEnv->uClampWidth, false, nIntX + i - 3 ), nY, &sPixel, pbValid );

            ui32Sum = FirAccumulate3( ui32Sum, ui32SrcPixel, g_YUVScaleState.aui32HorizCoeffs[ui32Phase][i] );
        }

        /* pack these output pixels 4 at a time in 64 bit writes to the packer */
        *(pDst + (ui32DstIndex++ * dstPixelStride)) = (BYTE)FinalFirRound( ui32Sum, 6 );
    }

}
void ProgramScaler(IMG_UINT32 ui32SourceSize, IMG_UINT32 ui32DestSize)
{
    /* No decimation supported yet */
    BYTE Coeffs[MAXTAP][MAXINTPT];
    IMG_UINT32 aui32CoefRegs[5];
    int regtaps, t, i, intpoints, hwtaps;
    Beta = 5.0;
    Lanczos = 0.8f;
    intpoints = 8;
    hwtaps = 4;

    g_YUVScaleState.scaleSrcSize = ui32SourceSize;
    g_YUVScaleState.scaleDstSize = ui32DestSize;

    if((ui32DestSize*4)<ui32SourceSize)
    {
        /* Decimation not supported in MVEA1-L scaler */
        printf("Scaler setup failled due to scale factor too low");
        return;
    }

    CalcCoefs(ui32SourceSize,ui32DestSize,Coeffs,8,4,1);

    regtaps = 2;

    memset(aui32CoefRegs,0,sizeof(aui32CoefRegs));

    for(t=0;t<regtaps;t++)
    {
        for(i=(intpoints/2 - 1);i>=0;i--)
        {
            aui32CoefRegs[2*t] |= (Coeffs[t][i]&0xff)<<(i*8);
        }
        for(i=(intpoints - 1);i>=(intpoints/2);i--)
        {
            aui32CoefRegs[2*t+1] |= (Coeffs[t][i]&0xff)<<(i*8);
        }
    }
    aui32CoefRegs[4] = Coeffs[hwtaps/2][0];

    /* write the registers individually at the moment but move to a block write later */
    Read4Coeffs( &g_YUVScaleState.aui32HorizInputCoeffs[0], aui32CoefRegs[0] );
    Read4Coeffs( &g_YUVScaleState.aui32HorizInputCoeffs[4], aui32CoefRegs[1] );
    Read4Coeffs( &g_YUVScaleState.aui32HorizInputCoeffs[8], aui32CoefRegs[2] );
    Read4Coeffs( &g_YUVScaleState.aui32HorizInputCoeffs[12],  aui32CoefRegs[3] );
    g_YUVScaleState.aui32HorizInputCoeffs[16] = aui32CoefRegs[4] & 0xff;

    g_YUVScaleState.scalePitch = ((ui32SourceSize) * 2048)/ui32DestSize;
    g_YUVScaleState.scaleInitial = (2*2048);

#if 0
    ui32ScalerHCTRL |= (ui32Pitch<<MVCSIM_SCALER_HCTRL_PITCH_SHIFT) |
                       ((2*2048)<<MVCSIM_SCALER_HCTRL_INITIAL_SHIFT);

    MVCTestWriteSingleRegister(MVCSIM_REG_SCALER_HCTRL_ADDR, ui32ScalerHCTRL);

    MVCTestWriteSingleRegister(MVCSIM_REG_SCALER_SCLDSIZE_ADDR,
                               ((ui32DestSize-1) << MVCSIM_SCALER_HSSIZE_WIDTH_SHIFT));
#endif
    ConvertScalerCoeffsToInternalFormat();
}

void ScaleImagePlane(PBYTE pSrc, PBYTE pDst, int srcWidth, int srcHeight, int srcStride, int dstWidth, int dstHeight, int dstStride, PBYTE tmpBuf)
{
    int x;

    /* scale horizontal to temp buffer */
    ProgramScaler(srcWidth,dstWidth);

    for(x=0;x<srcHeight;x++)
    {
        HorizScaleLine(pSrc + srcStride*x, 1, tmpBuf + dstWidth*x, 1);
    }

    /* scale vertical to dst buffer */
    ProgramScaler(srcHeight,dstHeight);

    for(x=0;x<dstWidth;x++)
    {
        HorizScaleLine(tmpBuf + x, dstWidth, pDst + x, dstStride);
    }

}
