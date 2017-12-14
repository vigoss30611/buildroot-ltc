/**
*******************************************************************************
 @file mc_structs.h

 @brief

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
#ifndef MC_STRUCTS_H_
#define MC_STRUCTS_H_

#include <felixcommon/pixel_format.h>
#include <felixcommon/lshgrid.h>
#include <ci/ci_api_structs.h>
#include <ci/ci_modules_structs.h>
#include "hw_struct/save.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup MC
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the MC documentation module
 *---------------------------------------------------------------------------*/

/**
 * @note Can be changed from double to float to change the precision of
 * computations
 */
#define MC_FLOAT double

/**
 * @name Main modules
 * @brief Modules that are not configuring statistics output
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Main modules group
 *---------------------------------------------------------------------------*/

/** Similar to @ref CI_MODULE_IIF */
typedef struct MC_MODULE_IIF
{
    /** @brief Imager to use (0 to @ref CI_N_IMAGERS -1) */
    IMG_UINT8 ui8Imager;

    enum MOSAICType eBayerFormat;

    /**
     * @brief Left (0) and top (1) displacement from the imager's capture
     * (in pixels)
     */
    IMG_UINT16 ui16ImagerOffset[2];

    /** @brief size of the imager (in CFA) */
    IMG_UINT16 ui16ImagerSize[2];

    /**
     * @brief Horizontal and vertical decimation of the image from the imager
      *(in CFA) - 0 means no decimation
     *
     * Number of columns (0) and rows (1) to skip
     */
    IMG_UINT16 ui16ImagerDecimation[2];
    /** @brief in CFA */
    IMG_UINT8 ui8BlackBorderOffset;
} MC_IIF;

/** Similar to @ref CI_MODULE_BLC */
typedef struct MC_BLC
{
    IMG_BOOL8 bBlackFrame;
    IMG_BOOL8 bRowAverage;
    MC_FLOAT ui16PixelMax;

    MC_FLOAT fRowAverage;
    IMG_INT8 aFixedOffset[BLC_OFFSET_NO];
} MC_BLC;

/** Similar to @ref CI_MODULE_WBC */
typedef struct MC_LSH_WBC
{
    MC_FLOAT aGain[LSH_GRADS_NO];
    MC_FLOAT aClip[LSH_GRADS_NO];

    enum WBC_MODES eRGBThresholdMode;
    /** @brief as a [0.0 .. 1.0] range of register value */
    MC_FLOAT afRGBThreshold[WBC_CHANNEL_NO];
    MC_FLOAT afRGBGain[WBC_CHANNEL_NO];
} MC_WBC;

/** Similar to @ref CI_MODULE_LSH */
typedef struct MC_LSH
{
    MC_FLOAT aGradients[LSH_GRADS_NO][2];
} MC_LSH;

/** Similar to @ref CI_MODULE_LCA */
typedef struct MC_LCA
{
    MC_FLOAT aCoeffRed[LCA_COEFFS_NO][2];
    IMG_UINT16 aOffsetRed[2];

    MC_FLOAT aCoeffBlue[LCA_COEFFS_NO][2];
    IMG_UINT16 aOffsetBlue[2];

    IMG_UINT8 aShift[2];
    IMG_UINT8 aDec[2];
} MC_LCA;

/** Similar to @ref CI_MODULE_DNS */
typedef struct MC_DNS
{
    IMG_BOOL8 bCombineChannels;
    MC_FLOAT aPixThresLUT[DNS_N_LUT];
    /** Necessary to calculate the proper precision conversion for the
     * aPixThresh tables */
    IMG_UINT8 ui8SensorBitdepth;
    MC_FLOAT fGreyscalePixelThreshold;
} MC_DNS;

/** Similar to @ref CI_MODULE_PLANECONV */
struct MC_PLANECONV
{
    enum ePlaneConvType eType;

    MC_FLOAT aCoeff[RGB_COEFF_NO][YCC_COEFF_NO];
    MC_FLOAT aOffset[RGB_COEFF_NO];
};

/** Similar to @ref CI_MODULE_R2Y */
typedef struct MC_PLANECONV MC_R2Y;
/** Similar to @ref CI_MODULE_Y2R */
typedef struct MC_PLANECONV MC_Y2R;

/** Similar to @ref CI_MODULE_CCM */
typedef struct MC_CCM
{
    MC_FLOAT aCoeff[RGB_COEFF_NO][4];
    MC_FLOAT aOffset[RGB_COEFF_NO];
} MC_CCM;

/** @see Related to CI_MODULE_MIE */
typedef struct MC_MIE
{
    /** @note in range of register */
    MC_FLOAT fBlackLevel;

    IMG_BOOL8 bVibrancy;
    MC_FLOAT aVibrancySatMul[MIE_VIBRANCY_N];

    IMG_BOOL8 bMemoryColour;
    /** @brief Output brightness (in range of register) */
    MC_FLOAT aYOffset[MIE_NUM_SLICES];
    /** @brief Output saturation */
    MC_FLOAT aYScale[MIE_NUM_SLICES];

    MC_FLOAT aCOffset[MIE_NUM_SLICES];
    MC_FLOAT aCScale[MIE_NUM_SLICES];
    /**
     * @brief Output hue
     *
     * @note Rot 0 and Rot 1 per slice
     */
    MC_FLOAT aRot[2*MIE_NUM_SLICES];
    /** @note This value is given as 0..1 range of the register */
    MC_FLOAT aGaussMinY[MIE_NUM_SLICES];
    MC_FLOAT aGaussScaleY[MIE_NUM_SLICES];
    /** @note This value is given as 0..1 range of the register */
    MC_FLOAT aGaussCB[MIE_NUM_SLICES];
    /** @note This value is given as 0..1 range of the register */
    MC_FLOAT aGaussCR[MIE_NUM_SLICES];
    /** @note Rot 0 and Rot 1 per slice */
    MC_FLOAT aGaussRot[2*MIE_NUM_SLICES];
    MC_FLOAT aGaussKB[MIE_NUM_SLICES];
    MC_FLOAT aGaussKR[MIE_NUM_SLICES];
    /** MIE_NUM_MEMCOLOURS elements per slice */
    MC_FLOAT aGaussScale[MIE_GAUSS_SC_N];
    MC_FLOAT aGaussGain[MIE_GAUSS_GN_N];
} MC_MIE;

/** Similar to @ref CI_MODULE_TNM */
typedef struct MC_TNM
{
    IMG_BOOL8 bBypassTNM;

    MC_FLOAT aCurve[TNM_CURVE_NPOINTS];

    // histogram
    MC_FLOAT histFlattenMin;
    MC_FLOAT histFlattenThreshold;  // and reciprocal

    // chroma
    MC_FLOAT chromaSaturationScale;
    MC_FLOAT chromaConfigurationScale;
    MC_FLOAT chromaIOScale;

    IMG_UINT16 ui16LocalColumns;
    IMG_UINT16 ui16ColumnsIndex;

    MC_FLOAT localWeights;
    MC_FLOAT updateWeights;

    MC_FLOAT inputLumaOffset;
    MC_FLOAT inputLumaScale;

    MC_FLOAT outputLumaOffset;
    MC_FLOAT outputLumaScale;
} MC_TNM;

/** Similar to @ref CI_MODULE_ESC */
typedef struct MC_ESC
{
    /**
     * @brief Do not filter the image using the scaling taps.
     *
     * If set to TRUE the Vertical pitch and output size can still be used to
     * configure some unfiltered decimation.
     */
    IMG_BOOL8 bBypassESC;
    /** @brief adjust cutoff frequency when computing the taps */
    IMG_BOOL8 bAdjustCutoff;

    /** @brief pixels - width/height */
    IMG_UINT16 aOutputSize[2];
    /** @brief left/top */
    IMG_UINT16 aOffset[2];
    /** @brief horizontal/vertical */
    MC_FLOAT aPitch[2];

    IMG_BOOL8 bChromaInter;
} MC_ESC;

/** Similar to @ref CI_MODULE_DSC */
typedef struct MC_DSC
{
    /**
     * @brief Do not filter the image using the scaling taps.
     *
     * If set to TRUE the Vertical pitch and output size can still be used to
     * configure some unfiltered decimation.
     */
    IMG_BOOL8 bBypassDSC;
    /** @brief adjust cutoff frequency when computing the taps */
    IMG_BOOL8 bAdjustCutoff;

    /** @brief pixels - width/height */
    IMG_UINT16 aOutputSize[2];
    /** @brief left/top */
    IMG_UINT16 aOffset[2];
    /** @brief horizontal/vertical */
    MC_FLOAT aPitch[2];
} MC_DSC;

/** Similar to @ref CI_MODULE_DPF */
typedef struct MC_DPF
{
    /** @brief A combination of @ref DPFEnable values, 0 being none */
    IMG_UINT8 eDPFEnable;

    /**
     * @name input map attributes
     * @{
     */

    /** 2*uiNDefec (X,Y) */
    IMG_UINT16 *apDefectInput;
    IMG_UINT32 ui32NDefect;
    /**
     * nb of lines per window for the DPF module - do not modify (computed
     * when initialised using @ref CI_HWINFO)
     */
    IMG_UINT16 ui16NbLines;

    /**
     * @}
     * @name output map attributes
     * @{
     */

    IMG_UINT32 ui32OutputMapSize;

    /**
     * @}
     * @name input parameters
     * @{
     */

    /** @note affects both read and write map */
    IMG_UINT8 aSkip[2];
    /** @note affects both read and write map */
    IMG_UINT16 aOffset[2];

    /**
     * @}
     * @name Computation attributes
     * @{
     */
    IMG_UINT8 ui8Threshold;
    MC_FLOAT fWeight;

    /**
     * @}
     */
} MC_DPF;

struct MC_GAMUTMAPPER
{
    MC_FLOAT fClipMin;
    MC_FLOAT fClipMax;
    MC_FLOAT fSrcNorm;
    MC_FLOAT aSlope[MGM_N_SLOPE];
    MC_FLOAT aCoeff[MGM_N_COEFF];
};

/** Similar to @ref CI_MODULE_MGM */
typedef struct MC_GAMUTMAPPER MC_MGM;
/** Similar to @ref CI_MODULE_DGM */
typedef struct MC_GAMUTMAPPER MC_DGM;

/** Similar to @ref CI_MODULE_GMA */
typedef struct CI_MODULE_GMA MC_GMA;

/** Similar to @ref CI_MODULE_SHA */
typedef struct MC_SHA
{
    IMG_UINT16 ui16Threshold;
    MC_FLOAT fStrength;
    MC_FLOAT fDetail;
    IMG_BOOL8 bDenoise;
    MC_FLOAT aGainWeigth[SHA_N_WEIGHT];

    MC_FLOAT fELWScale;
    IMG_INT16 i16ELWOffset;

    IMG_UINT16 aDNSimil[SHA_N_COMPARE];
    IMG_UINT16 aDNAvoid[SHA_N_COMPARE];
} MC_SHA;

/** Similar to @ref CI_MODULE_RLT */
typedef struct CI_MODULE_RLT MC_RLT;

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Main modules group
 *---------------------------------------------------------------------------*/

/**
 * @name Statistics configuration
 * @brief Statistics configuration modules
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Statistics modules group
 *---------------------------------------------------------------------------*/

/** Similar to @ref CI_MODULE_EXS */
typedef struct MC_EXS
{
    IMG_BOOL8 bGlobalEnable;
    IMG_BOOL8 bRegionEnable;

    IMG_UINT16 ui16Left;
    IMG_UINT16 ui16Top;
    IMG_UINT16 ui16Width;
    IMG_UINT16 ui16Height;
    MC_FLOAT fPixelMax;
} MC_EXS;

/** Similar to @ref CI_MODULE_FOS */
typedef struct MC_FOS
{
    IMG_BOOL8 bGlobalEnable;
    IMG_BOOL8 bRegionEnable;

    IMG_UINT16 ui16Left;
    IMG_UINT16 ui16Top;
    IMG_UINT16 ui16Width;
    IMG_UINT16 ui16Height;

    IMG_UINT16 ui16ROILeft;
    IMG_UINT16 ui16ROITop;
    IMG_UINT16 ui16ROIWidth;
    IMG_UINT16 ui16ROIHeight;
} MC_FOS;

/** Similar to @ref CI_MODULE_FLD */
typedef struct MC_FLD
{
    IMG_BOOL8 bEnable;
    /**
     * @brief Frame vertical total (frame height + blank border)
     *
     * @note Not written to registers but used with ui8FrameRate to compute
     * some registers (@ref CI_MODULE_FLD::ui16FracStep50 and
     * @ref CI_MODULE_FLD::ui16FracStep60)
     */
    IMG_UINT16 ui16VTot;
    /**
     * @brief Frame rate (in Hz)
     *
     * @note Not written to registers but used with ui16VTot to compute some
     * registers (@ref CI_MODULE_FLD::ui16FracStep50 and
     * @ref CI_MODULE_FLD::ui16FracStep60)
     */
    MC_FLOAT fFrameRate;
    IMG_UINT16 ui16CoefDiff;
    IMG_UINT16 ui16NFThreshold;
    IMG_UINT32 ui32SceneChange;
    IMG_UINT8 ui8RShift;
    IMG_UINT8 ui8MinPN;
    IMG_UINT8 ui8PN;
    IMG_BOOL8 bReset;
} MC_FLD;

/** Similar to @ref CI_MODULE_HIS */
typedef struct MC_HIS
{
    IMG_BOOL8 bGlobalEnable;
    IMG_BOOL8 bRegionEnable;

    MC_FLOAT fInputOffset;
    MC_FLOAT fInputScale;
    IMG_UINT16 ui16Left;
    IMG_UINT16 ui16Top;
    IMG_UINT16 ui16Width;
    IMG_UINT16 ui16Height;
} MC_HIS;

/** Similar to @ref CI_MODULE_WBS */
typedef struct MC_WBS
{
    /** 0 disables WBS module, up to 2 */
    IMG_UINT8 ui8ActiveROI;

    MC_FLOAT fRGBOffset;
    MC_FLOAT fYOffset;

    /**
     * @name region of interest position
     * @{
     */
    IMG_UINT16 aRoiLeft[WBS_NUM_ROI];
    IMG_UINT16 aRoiTop[WBS_NUM_ROI];
    IMG_UINT16 aRoiWidth[WBS_NUM_ROI];
    IMG_UINT16 aRoiHeight[WBS_NUM_ROI];

    /**
     * @}
     * @name thresholds per ROI (RGB and Y)
     * @note this could be %age in a float
     * @{
     */
    IMG_UINT16 aRMax[WBS_NUM_ROI];
    IMG_UINT16 aGMax[WBS_NUM_ROI];
    IMG_UINT16 aBMax[WBS_NUM_ROI];
    IMG_UINT16 aYMax[WBS_NUM_ROI];

    /**
     * @}
     */
} MC_WBS;

/** Similar to @ref CI_MODULE_AWS */
typedef struct MC_AWS
{
    /** @brief Enable usage of AWS if HW supports it */
    IMG_BOOL8 bEnable;
    /** @brief Enable the debug bitmap feature (replaces pixels values) */
    IMG_BOOL8 bDebugBitmap;

    MC_FLOAT fLog2_R_Qeff;
    MC_FLOAT fLog2_B_Qeff;

    MC_FLOAT fRedDarkThresh;
    MC_FLOAT fBlueDarkThresh;
    MC_FLOAT fGreenDarkThresh;

    MC_FLOAT fRedClipThresh;
    MC_FLOAT fBlueClipThresh;
    MC_FLOAT fGreenClipThresh;

    MC_FLOAT fBbDist;

    /**
     * @brief start row for the statistics (use -1 to include the row)
     */
    IMG_UINT16 ui16GridStartRow;
    IMG_UINT16 ui16GridStartColumn;

    /** @brief in CFA */
    IMG_UINT16 ui16GridTileWidth;
    /** @brief in CFA */
    IMG_UINT16 ui16GridTileHeight;

    MC_FLOAT aCurveCoeffX[AWS_LINE_SEG_N];
    MC_FLOAT aCurveCoeffY[AWS_LINE_SEG_N];
    MC_FLOAT aCurveOffset[AWS_LINE_SEG_N];
    MC_FLOAT aCurveBoundary[AWS_LINE_SEG_N];

    /**
     * @}
     */
} MC_AWS;

/** Similar to @ref CI_MODULE_ENS */
typedef struct MC_ENS
{
    IMG_BOOL8 bEnable;

    IMG_UINT32 ui32NLines;
    IMG_UINT32 ui32KernelSubsampling;
} MC_ENS;

#ifdef INFOTM_HW_AWB_METHOD
typedef struct HW_AWB_DATA
{
    IMG_UINT32 ui32LowData;
    IMG_UINT32 ui32HighData;
} HW_AWB_DATA;

typedef struct HW_AWB_ROW
{
    HW_AWB_DATA Sum_G_R[16];
    HW_AWB_DATA Sum_Y_B[16];
    HW_AWB_DATA Sum_RW_W[16];
    HW_AWB_DATA Sum_BW_GW[16];
    HW_AWB_DATA Sum_GBW_GRW[16];
} HW_AWB_ROW;

typedef struct MC_HW_AWB
{
    IMG_UINT32 Reserve[8];
    IMG_UINT32 ScdDoneFlag;
    HW_AWB_ROW Mc_Hw_AWb[16];
} MC_HW_AWB;
#endif //INFOTM_HW_AWB_METHOD

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Statistics modules group
 *---------------------------------------------------------------------------*/

// no name group for pipeline as it's alone

typedef struct MC_PIPELINE
{
    /**
     * given when initialising the structure - represents HW configuration
     */
    const CI_HWINFO *pHWInfo;

    /**
     * @brief can only be a YUV format
     *
     * @warning does not differentiate between YUV and YVU: HW configuration
     * is not changed by MC to match UV/VU order
     */
    ePxlFormat eEncOutput;
    /** @brief can only be a RGB format */
    ePxlFormat eDispOutput;
    /** @brief can only be @ref BGR_101010_32 */
    ePxlFormat eHDRExtOutput;
    /** @brief can only be @ref BGR_161616_64 */
    ePxlFormat eHDRInsertion;
    /** @brief can only be TIFF format */
    ePxlFormat eRaw2DExtOutput;

    enum CI_INOUT_POINTS eDEPoint;
    IMG_UINT16 ui16SystemBlackLevel;
    IMG_BOOL8 bEnableCRC;
    IMG_BOOL8 bEnableTimestamp;

    /**
     * @name Main modules
     * @{
     */

    MC_IIF sIIF;
    MC_BLC sBLC;
    MC_RLT sRLT;
    MC_LSH sLSH;
    MC_WBC sWBC;
    MC_DNS sDNS;
    MC_DPF sDPF;
    MC_LCA sLCA;
    MC_CCM sCCM;
    MC_MGM sMGM;
    MC_GMA sGMA;
    MC_R2Y sR2Y;
    MC_MIE sMIE;
    MC_TNM sTNM;
    MC_SHA sSHA;
    MC_ESC sESC;
    MC_DSC sDSC;
    MC_Y2R sY2R;
    MC_DGM sDGM;

    /**
     * @}
     * @name Statistics modules
     * @{
     */

    MC_EXS sEXS;
    MC_FOS sFOS;
    MC_WBS sWBS;
    MC_AWS sAWS;
    MC_HIS sHIS;
    MC_FLD sFLD;
    MC_ENS sENS;

    /**
     * @}
     */
} MC_PIPELINE;

/**
 * @name Statistics output
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the statistics output group
 *---------------------------------------------------------------------------*/

/** FOS module output histograms configured with @ref MC_FOS */
typedef struct MC_STATS_FOS
{
    /**
     * @brief A single global image statistics set (4 channels for the bayer
     * CFA).
     */
    IMG_UINT32 ROISharpness;
    /**
     * @brief Region clipping statistics ROWSxCOLUMNS regions x4 channels per
     * Bayer CFA
     */
    IMG_UINT32 gridSharpness[FOS_V_TILES][FOS_H_TILES];
} MC_STATS_FOS;

/** EXS module output histograms configured with @ref MC_EXS */
typedef struct MC_STATS_EXS
{
    /**
     * @brief A single global image statistics set (4 channels for the bayer
     * CFA).
     */
    IMG_UINT32 globalClipped[4];
    /**
     * @brief Region clipping statistics ROWSxCOLUMNS regions x4 channels per
     * Bayer CFA
     */
    IMG_UINT32 regionClipped[EXS_V_TILES][EXS_H_TILES][4];
} MC_STATS_EXS;


/** HIS module output histograms configured with @ref MC_HIS */
typedef struct MC_STATS_HIS
{
    /** @brief A single global histogram  */
    IMG_UINT32 globalHistogram[HIS_GLOBAL_BINS];
    /**
     * @brief Region HistogramsAn array of width*height histograms with
     * nbins each
     */
    IMG_UINT32
        regionHistograms[HIS_REGION_VTILES][HIS_REGION_HTILES][HIS_REGION_BINS];
} MC_STATS_HIS;


/** WB White Patch stats as part of @ref MC_STATS_WBS */
typedef struct MC_STATS_WBS_WP
{
    /** @brief Accumulated pixel values for RGB channels */
    IMG_UINT32 channelAccumulated[3];
    /** @brief Number of pixels counted per channel */
    IMG_UINT32 channelCount[3];
} MC_STATS_WBS_WP;

/** WB Average colour stats as part of @ref MC_STATS_WBS */
typedef struct MC_STATS_WBS_AC
{
    IMG_UINT32 channelAccumulated[3];
} MC_STATS_WBS_AC;

/** WB High level luminance stats as part of @ref MC_STATS_WBS */
typedef struct MC_STATS_WBS_HLW
{
    /** @brief Accumulated pixel values for RGB channels */
    IMG_UINT32 channelAccumulated[3];
    /**
     * @brief Number of pixels counted (number of pixels which luminance is
     * above the threshold)
     */
    IMG_UINT32 lumaCount;
} MC_STATS_WBS_HLW;

/** WBS module output stats configured with @ref MC_WBS */
typedef struct MC_STATS_WBS
{
    MC_STATS_WBS_WP whitePatch[WBS_NUM_ROI];
    MC_STATS_WBS_AC averageColor[WBS_NUM_ROI];
    MC_STATS_WBS_HLW highLuminance[WBS_NUM_ROI];
} MC_STATS_WBS;

/**
 * @brief AWS tile statistics
 * @note Field layout as in save structure, do not change
 * @note This layout is different than described in TRM, but same as in vhc
 */
typedef struct MC_TILE_STATS_AWS
{
    MC_FLOAT fCollectedRed;
    MC_FLOAT fCollectedBlue;
    MC_FLOAT fCollectedGreen;
    IMG_UINT32 numCFAs;
} MC_TILE_STATS_AWS;

/**
 * @brief AWS module output statistics configured with @ref MC_AWS
 */
typedef struct MC_STATS_AWS
{
    MC_TILE_STATS_AWS tileStats[AWS_NUM_GRID_TILES_VERT][AWS_NUM_GRID_TILES_HORIZ];
} MC_STATS_AWS;

/** @brief FLD module output possibilities */
typedef enum FLD_STATUS
{
    FLD_WAITING = FELIX_SAVE_FLICKER_WAITING,
    FLD_50HZ = FELIX_SAVE_FLICKER_LIGHT_50HZ,
    FLD_60HZ = FELIX_SAVE_FLICKER_LIGHT_60HZ,
    FLD_NO_FLICKER = FELIX_SAVE_FLICKER_NO_FLICKER,
    FLD_UNDETERMINED = FELIX_SAVE_FLICKER_CANNOT_DETERMINE,
} FLD_STATUS;

/** FLD module output configured with @ref MC_FLD */
typedef struct MC_STATS_FLD
{
    FLD_STATUS flickerStatus;
} MC_STATS_FLD;

/** Timestamps output if enabled in @ref MC_PIPELINE::bEnableTimestamp */
typedef struct MC_STATS_TIMESTAMP
{
    /** @brief Read by drivers when updating the linked list address */
    IMG_UINT32 ui32LinkedListUpdated;
    /**
     * @brief Start frame received at the input of the imager interface,
     * provided by HW
     */
    IMG_UINT32 ui32StartFrameIn;
    /**
     * @brief End of frame received at the input of the imager interface,
     * provided by HW
     */
    IMG_UINT32 ui32EndFrameIn;
    /**
     * @brief Start of frame sent out of the imager interface,
     * provided by HW
     */
    IMG_UINT32 ui32StartFrameOut;
    /**
     * @brief End frame sent out of the encoder memory interface,
     * provided by HW
     */
    IMG_UINT32 ui32EndFrameEncOut;
    /**
     * @brief End frame sent out to the display memory interface,
     * provided by HW
     */
    IMG_UINT32 ui32EndFrameDispOut;
    /**
     * @brief Read by drivers when interrupt is serviced
     */
    IMG_UINT32 ui32InterruptServiced;

#ifdef INFOTM_ISP
    IMG_UINT64 ui64CurSystemTS;
#endif
} MC_STATS_TIMESTAMP;

/** DPF output configured with @ref MC_DPF */
typedef struct MC_STATS_DPF
{
    IMG_UINT32 ui32FixedPixels;
    IMG_UINT32 ui32MapModifications;
    IMG_UINT32 ui32DroppedMapModifications;
    /**
     * @brief Number of corrections in the output map - computed from
     * @ref MC_STATS_DPF::ui32MapModifications and
     * @ref MC_STATS_DPF::ui32DroppedMapModifications
     */
    IMG_UINT32 ui32NOutCorrection;
} MC_STATS_DPF;

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Statistics output group
 *---------------------------------------------------------------------------*/

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the MC documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MC_STRUCTS_H_ */
