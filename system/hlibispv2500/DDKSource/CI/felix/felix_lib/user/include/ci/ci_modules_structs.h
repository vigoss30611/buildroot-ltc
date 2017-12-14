/**
*******************************************************************************
@file ci_modules_structs.h

@brief definitions of the different modules HW configuration available to the
outside world

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
#ifndef CI_MODULES_STRUCTS_H_
#define CI_MODULES_STRUCTS_H_

#include <img_types.h>
#include <felixcommon/pixel_format.h>  // for bayer format

// defines some array sizes and if some blocks are available or not
#include "felix_hw_info.h"  // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup CI_API_MOD
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_API documentation module
 *---------------------------------------------------------------------------*/

/** @brief Fixed size of the CFA */
#define CI_CFA_WIDTH 2
/** @brief Fixed size of the CFA */
#define CI_CFA_HEIGHT 2

typedef struct CI_MODULE_IIF
{
    /**
     * @note Register: IMGR_CTRL:IMGR_SCALER_BOTOM_BORDER
     */
    IMG_UINT16 ui16ScalerBottomBorder;
    /**
     * @brief Imager to use
     *
     * @note Register: IMGR_CTRL:IMG_INPUT_SEL
     */
    IMG_UINT8 ui8Imager;
    /**
     * @note Register: IMGR_CTRL:IMGR_BAYER_FORMAT
     */
    enum MOSAICType eBayerFormat;
    /**
     * @note Register: IMGR_CTRL:IMGR_BUF_THRESH
     */
    IMG_UINT16 ui16BuffThreshold;

    /**
     * @brief Left (0) and top (1) displacement from the imager's capture
     * (in pixels)
     *
     * @note Register: IMGR_CAP_OFFSET:IMGR_CAP_OFFSET_X and
     * IMGR_CAP_OFFSET:IMGR_CAP_OFFSET_Y
     */
    IMG_UINT16 ui16ImagerOffset[2];

    /**
     * @brief size of the imager (in CFA)
     *
     * @note Register: IMGR_OUT_SIZE:IMGR_OUT_ROWS_CFA and IMGR_OUT_COLS_CFA
     */
    IMG_UINT16 ui16ImagerSize[2];

    /**
     * @brief Horizontal and vertical decimation of the image from the
     * imager (in CFA) - 0 means no decimation
     *
     * @note Register: IMGR_IMAGE_DECIMATION:IMGR_COMP_SKIP_X,
     * IMGR_IMAGE_DECIMATION:IMGR_COMP_SKIP_Y
     */
    IMG_UINT16 ui16ImagerDecimation[2];
    /**
     * @brief in CFA
     *
     * @note Register: IMGR_IMAGE_DECIMATION:IMGR_BORDER_OFFSET
     */
    IMG_UINT8 ui8BlackBorderOffset;
} CI_MODULE_IIF;

/** @brief Black level correction configuration */
typedef struct CI_MODULE_BLC
{
    /**
     * @brief If IMG_TRUE apply row average and fixed channel correction.
     * Else apply only fixed correction.
     *
     * @note Load Structure: BLACK_CONTROL:BLACK_MODE
     */
    IMG_BOOL8 bRowAverage;
    /**
     * @brief Indicates that the input frame is optically black.
     *
     * @note Load Structure: BLACK_CONTROL:BLACK_FRAME
     */
    IMG_BOOL8 bBlackFrame;

    /**
     * @brief The reciprocal of the number of CFA units in the black border
     * - used only in row average mode
     *
     * @note Load Structure: BLACK_ANALYSIS:BLACK_ROW_RECIPROCAL
     */
    IMG_UINT16 ui16RowReciprocal;
    /**
     * @note Load Structure: BLACK_ANALYSIS:BLACK_PIXEL_MAX
     */
    IMG_UINT16 ui16PixelMax;

    /**
     * @note Load Structure: BLACK_FIXED_OFFSET:BLACK_FIXED
     */
    IMG_INT8 i8FixedOffset[BLC_OFFSET_NO];
}CI_MODULE_BLC;

/**
 * @brief White Balance Correction configuration (part of LSH module in HW)
 */
typedef struct CI_MODULE_WBC
{
    /**
     * @brief Gains for the white balance
     *
     * @note Load Structure: LSH_GAIN_0:WHITE_BALANCE_GAIN_[0..1]
     * and LSH_GAIN_1:WHITE_BALANCE_GAIN_[2..3]
     */
    IMG_UINT16 aGain[LSH_GRADS_NO];

    /**
     * @note Load Structure: LSH_CLIP_0:WHITE_BALANCE_CLIP[0..1]
     * and LSH_CLIP_1:WHITE_BALANCE_CLIP_[2..3]
     */
    IMG_UINT16 aClip[LSH_GRADS_NO];
}CI_MODULE_WBC;

/**
 * @brief Lens-Shading configuration
 *
 * See @ref LSH_CreateMatrix() @ref LSH_Free()
 * @ref CI_ModuleLSH_verif()
 */
typedef struct CI_MODULE_LSH
{
    /**
     * @note Load Structure: LSH_ALIGNMENT:LSH_SKIP_X
     */
    IMG_UINT16 ui16SkipX;
    /**
     * @note Register: LSH_OFFSET:LSH_SKIP_Y
     */
    IMG_UINT16 ui16SkipY;

    /**
     * @note Load Structure: LSH_ALIGNMENT:LSH_OFFSET_X
     */
    IMG_UINT16 ui16OffsetX;
    /**
     * @note Register: LSH_OFFSET:LSH_OFFSET_Y
     */
    IMG_UINT16 ui16OffsetY;

    /**
     * @note Load Structure: table LSH_GRADIENTS_X:SHADING_GRADIENT_X
     */
    IMG_UINT16 aGradientsX[LSH_GRADS_NO];
    /**
     * @note Register: table LSH_GRADIENTS_Y:SHADING_GRADIENT_Y
     */
    IMG_UINT16 aGradientsY[LSH_GRADS_NO];

    /**
     * @brief Enable the loading of a matrix from device memory
     *
     * @note Register: LSH_CTRL:LSH_ENABLE
     */
    IMG_BOOL8 bUseDeshadingGrid;

    /**
     * @brief The mesh size - a power of 2 in range of
     * [LSH_DELTA_BITS_MIN ; LSH_DELTA_BITS_MAX]
     *
     * @note Register: LSH_GRID_TILE:TILE_SIZE_LOG2
     *
     * @see LSH_DELTA_BITS_MIN LSH_DELTA_BITS_MAX
     */
    IMG_UINT8 ui8TileSizeLog2;

    /**
     * @brief Number of elements in a line of deshading grid
     * (including the 1st full 16b element)
     *
     * @note Not written to registers, @see ui16LineSize
     */
    IMG_UINT16 ui16Width;

    /**
     * @brief Number of lines in the deshading grid.
     *
     * @note Not written to registers, the HW reads as many lines as needed
     * according to the IIF output
     */
    IMG_UINT16 ui16Height;

    /**
     * @brief Number of bits to use to store the differences
     *
     * @note Load Structure: LSH_GRID:LSH_VERTEX_DIFF_BITS
     */
    IMG_UINT8 ui8BitsPerDiff;

    /** ui16Height per channel (1st value of every line) */
    IMG_UINT16 *matrixStart[LSH_GRADS_NO];
    /** (ui16Width-1)*ui16Height per channel - differences computed */
    IMG_INT16 *matrixDiff[LSH_GRADS_NO];

    /**
     * @brief Signals that convertion had clipped values if more than 0
     */
    IMG_UINT32 ui32ClippedValues;
}CI_MODULE_LSH;

/** @brief Chromatic Aberration configuration */
typedef struct CI_MODULE_LCA
{
    /**
     * @brief LCA_COEFFS_NO*[X;Y]
     *
     * @note Load Structure: table LAT_CA_DELTA_COEFF_RED
     */
    IMG_INT16 aCoeffRed[LCA_COEFFS_NO][2];
    /**
     * @brief X-Y
     *
     * @note Load Structure: LAT_CA_OFFSET_RED
     */
    IMG_UINT16 aOffsetRed[2];

    /**
     * @brief LCA_COEEFS_NO*[X;Y]
     *
     * @note Load Structure: table LAT_CA_DELTA_COEFF_BLUE
     */
    IMG_INT16 aCoeffBlue[LCA_COEFFS_NO][2];
    /**
     * @brief X-Y
     *
     * @note Load Structure: LAT_CA_OFFSET_BLUE
     */
    IMG_UINT16 aOffsetBlue[2];

    /**
     * @brief X-Y
     *
     * @note Load Structure: LAT_CA_NORMALIZATION:LAT_CA_SHIFT_Y and
     * LAT_CA_NORMALIZATION:SHIFT_X
     */
    IMG_UINT8 aShift[2];
    /**
     * @brief X-Y
     *
     * @note Load Structure: LAT_CA_NORMALIZATION:LAT_CA_DEC_Y and
     * LAT_CA_NORMALIZATION:LAT_CA_DEC_X
     */
    IMG_UINT8 aDec[2];
}CI_MODULE_LCA;

enum ePlaneConvType
{
    RGB_TO_YCC = 0,
    YCC_TO_RGB,
    YCC_TO_YCC
};

/**
 * @brief Color modification matrix configuration (RGB to YCC, YCC to RGB
 * and Colour Correction)
 */
struct CI_MODULE_PLANECONV
{
    /**
     * @brief To Know which direction is the convertion - set up at creation
     */
    enum ePlaneConvType eType;
    /**
     * @note RGB_TO_YCC: Load Structure: table RGB_TO_YCC_MATRIX_COEFFS
     * @note YCC_TO_RGB: Load Structure: table YCC_TO_RGB_MATRIX_COEFFS
     */
    IMG_INT16 aCoeff[RGB_COEFF_NO][YCC_COEFF_NO];
    /**
     * @note RGB_TO_YCC: Load Structure: table RGB_TO_YCC_MATRIX_COEFF_OFFSET
     * @note YCC_TO_RGB: Load Structure: table YCC_TO_RGB_MATRIX_COEFF_OFFSET
     */
    IMG_INT16 aOffset[RGB_COEFF_NO];
};

/** @brief RGB to YCC colour conversion */
typedef struct CI_MODULE_PLANECONV CI_MODULE_R2Y;
/** @brief YCC to RGB colour conversion */
typedef struct CI_MODULE_PLANECONV CI_MODULE_Y2R;

/** @brief Colour Correction */
typedef struct CI_MODULE_CCM
{
    /**
     * @note Load Structure: table CC_COEFFS
     */
    IMG_INT16 aCoeff[RGB_COEFF_NO][4];
    /**
     * @note Load Structure: CC_OFFSETS_1_0 and CC_OFFSETS_2
     */
    IMG_INT16 aOffset[RGB_COEFF_NO];
} CI_MODULE_CCM;

typedef struct CI_MODULE_MIE
{
    /**
     * @note Load Structure: MIE_VIB_PARAMS_0:MIE_BLACK_LEVEL
     */
    IMG_UINT16 ui16BlackLevel;

    /**
     * @note Load Structure: MIE_VIB_PARAMS_0:MIE_VIB_ON
     */
    IMG_BOOL8 bVibrancy;

    /**
     * @note Load Structure: table MIE_VIB_FUN
     */
    IMG_UINT16 aVibrancySatMul[MIE_VIBRANCY_N];

    /**
     * @note Load Structure: MIE_VIB_PARAMS_0:MIE_MC_ON
     */
    IMG_BOOL8 bMemoryColour;

    /**
     * @note Load Structure: table MIE_MC_PARAMS_Y:MIE_MC_YOFF
     */
    IMG_INT16 aYOffset[3];

    /**
     * @note Load Structure: table MIE_MC_PARAMS_C:MIE_MC_COFF
     */
    IMG_INT16 aCOffset[3];

    /**
     * @note Load Structure: table MIE_MC_PARAMS_Y:MIE_MC_YSCALE
     */
    IMG_UINT8 aYScale[3];

    /**
     * @note Load Structure: table MIE_MC_PARAMS_C:MIE_MC_CSCALE
     */
    IMG_UINT8 aCScale[3];

    /**
     * @note Load Structure: table MIE_MC_ROT_PARAMS
     *
     * @brief Hue rotation matrix, 2 elements per colour channels [r0 r1]
     * transformed in 2x2 matrix [r0 r1 -r1 r0]
     */
    IMG_INT8 aRot[2*3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_0:MIE_MC_GAUSS_MINY
     */
    IMG_UINT16 aGaussMinY[3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_0:MIE_MC_GAUSS_YSCALE
     */
    IMG_UINT8 aGaussScaleY[3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_1:MIE_MC_GAUSS_CB0
     */
    IMG_INT8 aGaussCB[3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_1:MIE_MC_GAUSS_CR0
     */
    IMG_INT8 aGaussCR[3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_2
     *
     * @brief Gaussian likelihood rotation matrix, 2 elements per colour
     * channels [r0 r1] transformed in 2x2 matrix [r0 r1 -r1 r0]
     */
    IMG_INT8 aGaussRot[2*3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_3:MIE_MC_GAUSS_KB
     */
    IMG_UINT8 aGaussKB[3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_3:MIE_MC_GAUSS_KR
     */
    IMG_UINT8 aGaussKR[3];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_4
     */
    IMG_UINT8 aGaussScale[MIE_GAUSS_SC_N];

    /**
     * @note Load Structure: table MIE_MC_GAUSS_PARAMS_5
     */
    IMG_UINT8 aGaussGain[MIE_GAUSS_GN_N];
} CI_MODULE_MIE;

/** @brief Default number of columns in the Tone Mapping processing */
#define TNM_NCOLUMNS 16

/** @brief Tone Mapper configuration */
typedef struct CI_MODULE_TNM
{
    /**
     * @note Load Structure: table TNM_GLOBAL_CURVE
     */
    IMG_UINT16 aCurve[TNM_CURVE_NPOINTS];

    /**
     * @brief bypass some of the functionalities
     * @brief Load Structure: TNM_BYPASS:TNM_BYPASS
     */
    IMG_BOOL8 bBypassTNM;

    //
    // histogram
    //
    /**
     * @note Load Structure: TNM_HIST_FLATTEN_0:TNM_HIST_FLATTEN_MIN
     */
    IMG_UINT16 histFlattenMin;

    /**
     * @brief Threshold and reciprocal
     *
     * @note Load Structure: TNM_HIST_FLATTEN_0:TNM_HIST_FLATTEN_THRES
     * and TNM_HIST_FLATTEN_1:TNM_HIST_FLATTEN_RECIP
     */
    IMG_UINT16 histFlattenThreshold[2];

    /**
     * @note Load Structure: TNM_HIST_NORM:TNM_HIST_NORM_FACTOR
     */
    IMG_UINT16 histNormFactor;
    /**
     * @note Load Structure: TNM_HIST_NORM:TNM_HIST_NORM_FACTOR_LAST
     */
    IMG_UINT16 histNormLastFactor;

    /**
     * @note Load Structure: TNM_CHR_0:TNM_CHR_SAT_SCALE
     */
    IMG_UINT16 chromaSaturationScale;
    /**
     * @note Load Structure: TNM_CHR0:TNM_CHR_CONF_SCALE
     */
    IMG_UINT16 chromaConfigurationScale;
    /**
     * @note Load Structure: TNM_CHR_1:TNM_CHR_IO_SCALE
     */
    IMG_UINT16 chromaIOScale;
    /**
     * @brief width, reciprocal
     *
     * @note Load Structure: TNM_LOCAL_COL:TNM_LOCAL_COL_WIDTH and
     * TNM_LOCAL_COL:TNM_LOCAL_COL_WIDTH_RECIP
     */
    IMG_UINT16 localColWidth[2];
    /**
     * @note Load Structure: TNM_CTX_LOCAL_COL:TNM_CTX_LOCAL_COLS
     */
    IMG_UINT16 localColumns;
    /**
     * @note Load Structure: TNM_CTX_LOCAL_COL:TNM_CTX_LOCAL_COL_IDX
     */
    IMG_UINT16 columnsIdx;
    /**
     * @note Load Structure: TNM_WEIGHTS:TNM_LOCAL_WEIGHT
     */
    IMG_UINT16 localWeights;
    /**
     * @note Load Structure: TNM_WEIGHTS:TNM_CURVE_UPDATE_WEIGHT
     */
    IMG_UINT16 updateWeights;

    /**
     * @note Load Structure: TNM_INPUT_LUMA:TNM_INPUT_LUMA_OFFSET
     */
    IMG_UINT16 inputLumaOffset;
    /**
     * @note Load Structure: TNM_INPUT_LUMA:TNM_INPUT_LUMA_SCALE
     */
    IMG_UINT16 inputLumaScale;
    /**
     * @note Load Structure: TNM_OUTPUT_LUMA:TNM_OUTPUT_LUMA_OFFSET
     */
    IMG_UINT16 outputLumaOffset;
    /**
     * @note Load Structure: TNM_OUTPUT_LUMA:TNM_OUTPUT_LUMA_SCALE
     */
    IMG_UINT16 outputLumaScale;
} CI_MODULE_TNM;

/** @brief Encoder scaler subsampling configuration options */
enum CI_MODULE_SCALER_SUBSAMPLE
{
    /**
     * @brief Used when output format is not subsampled
     */
    EncOut422 = 0,
    /**
     * @brief Used when 422->420 subsampling is done while scaling
     * @note Recommended for scaling ratios <= 4:1
     */
    EncOut420_scaler,
    /**
     * @brief Used when 422->420 subsampling is done after the scaling
     * @note Recommended for scaling ratios > 4:1
     */
    EncOut420_vss
};

/** @brief Scaler configuration (encoder and display) */
struct CI_MODULE_SCALER
{
    /** @brief Encoder or display scaler - set up at creation */
    IMG_BOOL8 bIsDisplay;

    /**
     * @brief If true this disable the scaler
     *
     * @note Load Structure (disp): DISP_SCAL_H_SETUP:DISP_SCAL_BYPASS
     * @note Load Structure (enc): ENC_SCAL_H_SETUP:ENC_SCAL_BYPASS
     */
    IMG_BOOL8 bBypassScaler;
    /**
     * @brief width & height
     *
     * @warning value is stored as width = 2*(aOutputSize[0]+1)
     * and height = aOutputSize[1]+1
     *
     * @note Load Structure (disp): DISP_SCAL_H_SETUP:DISP_SCAL_OUTPUT_COLUMNS
     * and DISP_SCAL_V_SETUP:DISP_SCAL_OUTPUT_ROWS
     * @note Load Structure (enc): ENC_SCAL_H_SETUP:ENC_SCAL_OUTPUT_COLUMNS
     * and ENC_SCAL_V_SETUP:ENC_SCAL_OUTPUT_ROWS
     */
    IMG_UINT16 aOutputSize[2];
    /**
     * @brief H and V - must be converted to have integer-phase-subphase
     *
     * @note Load Structure (disp): DISP_SCAL_H_PITCH and DISP_SCAL_V_PITCH
     * @note Load Structure (enc): ENC_SCAL_H_PITCH and ENC_SCAL_V_PITCH
     */
    IMG_UINT32 aPitch[2];

    /**
     * @brief If true is chroma interstitial, else chroma cosited - not used
     * in display pipeline
     *
     * @note Load Structure (enc): ENC_SCAL_H_SETUP:ENC_SCAL_CHROMA_MODE
     */
    IMG_BOOL8 bChromaInter;
    /**
     * @brief The subsampling method to use
     *
     * @note Load Structure (enc): set ENC_SCAL_V_SETUP:ENC_SCAL_422_NOT_420
     * or ENC_422_TO_420_CTRL:ENC_422_TO_420_ENABLE to 1 according to the value
     */
    enum CI_MODULE_SCALER_SUBSAMPLE eSubsampling;
    /**
     * @brief Cropping at the left/top of the input image of the scaler
     *
     * @note Load Structure (disp): DISP_SCAL_H_SETUP:DISP_SCAL_H_OFFSET
     * and DISP_SCAL_V_SETUP:DISP_SCAL_V_OFFSET
     * @note Load Structure (enc):  ENC_SCAL_H_SETUP:ENC_SCAL_H_OFFSET
     * and ENC_SCAL_V_SETUP:ENC_SCAL_V_OFFSET
     */
    IMG_UINT16 aOffset[2];

    /**
     * @brief SCALER_NUM_PHASExN taps (4x8 in encoder, 4x4 in display)
     *
     * @note Load Structure (disp): table DISP_SCAL_V_LUMA_TAPS_0_TO_3
     * @note Load Structure (enc): tables ENC_SCAL_V_LUMA_TAPS_0_TO_3
     * and ENC_SCAL_V_LUMA_TAPS_4_TO_7
     */
    IMG_INT8 VLuma[(SCALER_PHASES/2)*ESC_V_LUMA_TAPS];
    /**
     * @brief SCALER_NUM_PHASExN taps (4x16 in encoder, 4x8 in display)
     *
     * @note Load Structure (disp): tables DISP_SCAL_H_LUMA_TAPS_0_TO_3
     * and DISP_SCAL_H_LUMA_TAPS_4_TO_7
     * @note Load Structure (enc): tables ENC_SCAL_H_LUMA_TAPS_0_TO_3,
     * ENC_SCAL_H_LUMA_TAPS_4_TO_7, ENC_SCAL_H_LUMA_TAPS_8_TO_11
     * and ENC_SCAL_H_LUMA_TAPS_12_TO_15
     */
    IMG_INT8 HLuma[(SCALER_PHASES/2)*ESC_H_LUMA_TAPS];

    /**
     * @brief SCALER_NUM_PHASExN (4x4 in encoder, 4x2 in display)
     *
     * @note Load Structure (disp): table DISP_SCAL_V_CHROMA_TAPS_0_TO_1
     * @note Load Structure (enc): table ENC_SCAL_V_CHROMA_TAPS_0_TO_3
     */
    IMG_INT8 VChroma[(SCALER_PHASES/2)*ESC_V_CHROMA_TAPS];
    /**
     * @brief SCALER_NUM_PHASExN taps (4x8 in encoder, 4x4 in display)
     *
     * @note Load Structure (disp): table DISP_SCAL_H_CHROMA_TAPS_0_TO_3
     * @note Load Structure (enc): tables ENC_SCAL_H_CHROMA_TAPS_0_TO_3
     * and ENC_SCAL_H_CHROMA_TAPS_4_TO_7
     */
    IMG_INT8 HChroma[(SCALER_PHASES/2)*ESC_H_CHROMA_TAPS];
};

typedef struct CI_MODULE_SCALER CI_MODULE_ESC;
typedef struct CI_MODULE_SCALER CI_MODULE_DSC;

/** @brief Denoiser configuration */
typedef struct CI_MODULE_DNS
{
    /**
     * @brief Combine channel 1 and 2 during denoising process
     *
     * @note Load Structure: DENOISE_CONTROL:DENOISE_COMBINE_1_2
     */
    IMG_BOOL8 bCombineChannels;

    /**
     * @brief Pixel threshold multiplier (sensibility to colour difference)
     *
     * @note Load structure: DENOISER_CONTROL:LOG2_GREYSC_PIXTHRESH_MULT
     *
     * @warning HW use this field only from version 2.4
     */
    IMG_INT8 i8Log2PixThresh;

    /**
     * @brief Denoise Look Up Table
     *
     * @note @li [0;DNS_N_LUT0-1] 1st section\n
     * Load Structure: table PIX_THRESH_LUT_SEC0_POINTS
     * @li [DNS_N_LUT0;DNS_N_LUT0+DNS_N_LUT1-1] 2nd section\n
     * Load Structure: table PIX_THRESH_LUT_SEC1_POINTS
     * @li [DNS_N_LUT1;DNS_N_LUT-2] 3rd section\n
     * Load Structure: table PIX_THRESH_LUT_SEC2_POINTS
     * @li DNS_N_LUT-1 for interpolation\n
     * Load Structure: PIX_THRESH_LUT_SEC3_POINTS:PIX_THRESH_LUT_POINT_20
     */
    IMG_UINT16 aPixThresLUT[DNS_N_LUT];
}CI_MODULE_DNS;

struct CI_MODULE_GAMUTMAPPER
{
    /**
     * @note Load Structure: MGM_CLIP_IN:MGM_CLIP_MIN or
     * DGM_CLIP_IN:DGM_CLIP_MIN
     */
    IMG_INT16 i16ClipMin;
    /**
     * @note Load Structure: MGM_CLIP_IN:MGM_CLIP_MAX or
     * DGM_CLIP_IN:GM_CLIP_MAX
     */
    IMG_UINT16 ui16ClipMax;
    /**
     * @note Load Structure: MGM_CORE_IN_OUT:MGM_SRC_NORM or
     * DGM_CORE_IN_OUT:DGM_SRC_NORM
     */
    IMG_UINT16 ui16SrcNorm;
    /**
     * @note Load Structure: MGM_SLOPE_0_1:MGM_SLOPE_[0..1] and
     * MGM_SLOPE_2_R:MGM_SLOPE_2 or DGM_SLOPE_0_1:DGM_SLOPE_[0..1]
     * and DGM_SLOPE_2_R:DGM_SLOPE_2
     */
    IMG_UINT16 aSlope[MGM_N_SLOPE];
    /**
     * @note Load Structure: MGM_COEFFS_0_3:MGM_COEFF_[0..3] and
     * MGM_COEFFS_4_5:MGM_COEFF_[4..5] or DGM_COEFFS_0_3:DGM_COEFF_[0..3]
     * and DGM_COEFFS_4_5:DGM_COEFF_[4..5]
     */
    IMG_INT16 aCoeff[MGM_N_COEFF];
};

typedef struct CI_MODULE_GAMUTMAPPER CI_MODULE_MGM;
typedef struct CI_MODULE_GAMUTMAPPER CI_MODULE_DGM;

typedef struct CI_MODULE_GMA
{
    /**
     * @brief Use GMA Look up table
     *
     * @note Load Structure: GMA_BYPASS
     */
    IMG_BOOL8 bBypass;
} CI_MODULE_GMA;

/**
 * @brief Gamma Look Up Table - unique for all pipeline, should not be
 *  changed often
 *
 * @see This module is not updated with the Pipeline but with its own
 * function: CI_DriverSetGammaLUT()
 */
typedef struct CI_MODULE_GMA_LUT
{
    /**
     * @note Registers: GMA_RED_POINT table
     */
    IMG_UINT16 aRedPoints[GMA_N_POINTS];

    /**
     * @note Registers: GMA_GREEN_POINT table
     */
    IMG_UINT16 aGreenPoints[GMA_N_POINTS];

    /**
     * @note Registers: GMA_BLUE_POINT table
     */
    IMG_UINT16 aBluePoints[GMA_N_POINTS];
} CI_MODULE_GMA_LUT;

typedef struct CI_MODULE_SHA
{
    /**
     * @note Load Structure: SHA_PARAMS_0:SHA_THRESH
     */
    IMG_UINT8 ui8Threshold;
    /**
     * @note Load Structure: SHA_PARAMS_0:SHA_STRENGTH
     */
    IMG_UINT8 ui8Strength;
    /**
     * @note Load Structure: SHA_PARAMS_0:SHA_DETAIL
     */
    IMG_UINT8 ui8Detail;
    /**
     * @note Load Structure: SHA_PARAMS_0:SHA_DENOISE_BYPASS
     */
    IMG_BOOL8 bDenoiseBypass;
    /**
     * @note Load Structure: SHA_PARAMS_1 repeated field SHA_GWEIGHT_X
     */
    IMG_UINT8 aGainWeight[SHA_N_WEIGHT];
    /**
     * @note Load Structure: SHA_ELW_SCALEOFF:SHA_ELW_SCALE
     */
    IMG_UINT8 ui8ELWScale;
    /**
     * @note Load Structure: SHA_ELW_SCALEOFF:SHA_ELW_OFFS
     */
    IMG_INT8 i8ELWOffset;

    /**
     * @note Load Structure: SHA_DN_EDGE_SIMIL_COMP_PTS_0 and
     * SHA_DN_EDGE_SIMIL_COMP_PTS_1 repeated fields
     * SHA_DN_EDGE_SIMIL_COMP_PTS_X_TO_X
     */
    IMG_UINT8 aDNSimil[SHA_N_COMPARE];
    /**
     * @note Load Structure: SHA_DN_EDGE_AVOID_COMP_PTS_0 and
     * SHA_DN_EDGE_AVOID_COMP_PTS_1 repeated fields
     * SHA_DN_EDGE_AVOID_COMP_PTS_X_TO_X
     */
    IMG_UINT8 aDNAvoid[SHA_N_COMPARE];
} CI_MODULE_SHA;

enum DPFEnable
{
    CI_DPF_DETECT_ENABLED = 1,
    CI_DPF_WRITE_MAP_ENABLED = CI_DPF_DETECT_ENABLED << 1,
    CI_DPF_READ_MAP_ENABLED = CI_DPF_WRITE_MAP_ENABLED << 1,
};

typedef struct CI_MODULE_DPF_INPUT
{
    /** 2*uiNDefec (X,Y) */
    IMG_UINT16 *apDefectInput;

    IMG_UINT32 ui32NDefect;

    /**
     * @note Register: DPF_READ_BUF_SIZE:DPF_READ_BUF_CONTEXT_SIZE
     */
    IMG_UINT16 ui16InternalBufSize;
}CI_MODULE_DPF_INPUT;

/**
 * See @ref CI_PIPELINE::ui32DPFWriteMapSize used for the DPF output map size
 */
typedef struct CI_MODULE_DPF
{
    /**
     * @brief A combinaison of DPFEnable values, 0 being none
     *
     * @note Load structure: DPF_ENABLE (for detect)
     * Linked list: SAVE_CONFIG_FLAGS (for read or write)
     */
    IMG_UINT8 eDPFEnable;

    // output map size is part of the CI_PIPELINE

    //
    // computation parameters
    //
    /**
     * @note Load Structure: DPF_SENSITIVITY:DPF_THRESHOLD
     */
    IMG_UINT8 ui8Threshold;

    /**
     * @note Load Structure: DPF_SENSITIVITY:DPF_WEIGHT
     */
    IMG_UINT8 ui8Weight;

    /**
     * @note Load Structure: DPF_MODIFICATIONS_THRESHOLD:DPF_WRITE_MAP_POS_THR
     */
    IMG_UINT8 ui8PositiveThreshold;

    /**
     * @note Load Structure: DPF_MODIFICATIONS_THRESHOLD:DPF_WRITE_MAP_NEG_THR
     */
    IMG_UINT8 ui8NegativeThreshold;

    /**
     * @brief Combined sensor and imager interface horizontal/vertical skip
     * value in CFAs (same as IIF decimation)
     *
     * @note Register: DPF_SKIP
     *
     * @warning affect both input and output map
     */
    IMG_UINT8 aSkip[2];

    /**
     * @brief Offset of the first pixel of the input row/columns relative
     * to the defect map (full sensor resolution - same as IIF offset).
     *
     * @note Register: DPF_OFFSET
     *
     * @warning affect both input and output map
     */
    IMG_UINT16 aOffset[2];

    /**
     * @brief input map parameters written to registers
     * - modified when CI_UPD_DPF_INPUT is given
     */
    CI_MODULE_DPF_INPUT sInput;
} CI_MODULE_DPF;

/*
 * statistics control
 */

typedef struct CI_MODULE_EXS
{
    /**
     * @note Load Structure: EXS_GRID_START_COORDS:EXS_GRID_COL_START
     */
    IMG_UINT16 ui16Left;

    /**
     * @note Load Structure: EXS_GRID_START_COORDS:EXS_GRID_ROW_START
     */
    IMG_UINT16 ui16Top;

    /**
     * @note Load Structure: EXS_GRID_TILE:EXS_GRID_TILE_WIDTH
     */
    IMG_UINT16 ui16Width;

    /**
     * @note Load Structure: EXS_GRID_TILE:EXS_GRID_TILE_HEIGHT
     */
    IMG_UINT16 ui16Height;

    /**
     * @note Load Structure: EXS_PIXEL_MAX:EXS_PIXEL_MAX
     */
    IMG_UINT16 ui16PixelMax;
} CI_MODULE_EXS;

typedef struct CI_MODULE_FOS
{
    /**
     * @brief in pixels
     * @note Load Structure: FOS_GRID_START_COORDS:FOS_GRID_START_COL
     */
    IMG_UINT16 ui16Left;
    /**
     * @brief in pixels
     * @note Load Structure: FOS_GRID_START_COORDS:FOS_GRID_START_ROW
     */
    IMG_UINT16 ui16Top;
    /**
     * @brief in pixels
     * @note Load Structure: FOS_GRID_TILE:FOS_GRID_TILE_WIDTH
     */
    IMG_UINT16 ui16Width;
    /**
     * @brief in pixels
     * @note Load Structure: FOS_GRID_TILE:FOS_GRID_TILE_HEIGHT
     */
    IMG_UINT16 ui16Height;
    /**
     * @brief in pixels
     * @note Load Structure: FOS_ROI_COORDS_START:FOS_ROI_START_COL
     */
    IMG_UINT16 ui16ROILeft;
    /**
     * @brief in pixels
     * @note Load Structure: FOS_ROI_COORDS_START:FOS_ROI_START_ROW
     */
    IMG_UINT16 ui16ROITop;
    /**
     * @brief in pixels
     * @note Load Structure: FOS_ROI_COORDS_END:FOS_ROI_END_COL
     */
    IMG_UINT16 ui16ROIRight;
    /**
     * @brief in pixels
     * @note Load Structure: FOS_ROI_COORDS_END:FOS_ROI_END_ROW
     */
    IMG_UINT16 ui16ROIBottom;
} CI_MODULE_FOS;

typedef struct CI_MODULE_FLD
{
    /**
     * @note Load Structure: FLD_FRAC_STEP:FLD_FRAC_STEP_50
     */
    IMG_UINT16 ui16FracStep50;
    /**
     * @note Load Structure: FLD_FRAC_STEP:FLD_FRAC_STEP_60
     */
    IMG_UINT16 ui16FracStep60;
    /**
     * @brief Total number of veritcal line (including blanking)
     *
     * @note Not written to registers but needed to be able to recompute
     * the rate from ui16FracStep50 and ui16FracStep60
     */
    IMG_UINT16 ui16VertTotal;

    /**
     * @note Load Structure: FLD_TH:FLD_COEF_DIFF_TH
     */
    IMG_UINT16 ui16CoefDiff;
    /**
     * @note Load Structure: FLD_TH:FLD_NF_TH
     */
    IMG_UINT16 ui16NFThreshold;
    /**
     * @note Load Structure: FLD_SCENE_CHANGE_TH:FLD_SCENE_CHANGE_TH
     */
    IMG_UINT32 ui32SceneChange;

    /**
     * @note Load Structure: FLD_DFT:FLD_RSHIFT
     */
    IMG_UINT8 ui8RShift;
    /**
     * @note Load Structure: FLD_DFT:FLD_MIN_PN
     */
    IMG_UINT8 ui8MinPN;
    /**
     * @note Load Structure: FLD_DFT:FLD_PN
     */
    IMG_UINT8 ui8PN;
    /**
     * @note Load Structure: FLD_RESET:FLD_RESET
     */
    IMG_BOOL8 bReset;
} CI_MODULE_FLD;

typedef struct CI_MODULE_HIS
{
    /**
      * @brief in pixels
      * @note Load Structure: HIS_INPUT_RANGE:HIS_INPUT_OFFSET
      */
    IMG_UINT16 ui16InputOffset;
    /**
      * @brief in pixels
      * @note Load Structure: HIS_INPUT_RANGE:HIS_INPUT_SCALE
      */
    IMG_UINT16 ui16InputScale;
    /**
      * @brief in pixels
      * @note Load Structure:
      * HIS_REGION_GRID_START_COORDS:HIS_REGION_GRID_COL_START
      */
    IMG_UINT16 ui16Left;
    /**
      * @brief in pixels
      * @note Load Structure:
      * HIS_REGION_GRID_START_COORDS:HIS_REGION_GRID_ROW_START
      */
    IMG_UINT16 ui16Top;
    /**
      * @brief in pixels
      * @note Load Structure: HIS_REGION_GRID_TILE:HIS_REGION_GRID_TILE_WIDTH
      */
    IMG_UINT16 ui16Width;
    /**
      * @brief in pixels
      * @note Load Structure: HIS_REGION_GRID_TILE:HIS_REGION_GRID_TILE_HEIGHT
      */
    IMG_UINT16 ui16Height;
} CI_MODULE_HIS;

typedef struct CI_MODULE_WBS
{
    /**
     * @note Load Structure: WBS_ROI_COL_START
     */
    IMG_UINT16 aRoiLeft[WBS_NUM_ROI];
    /**
     * @note Load Structure: WBS_ROI_ROW_START
     */
    IMG_UINT16 aRoiTop[WBS_NUM_ROI];
    /**
     * @note Load Structure: WBS_ROI_COL_END
     */
    IMG_UINT16 aRoiRight[WBS_NUM_ROI];
    /**
     * @note Load Structure: WBS_ROI_ROW_END
     */
    IMG_UINT16 aRoiBottom[WBS_NUM_ROI];

    /**
     * @note Load Structure: WBS_ROI_ACT
     */
    IMG_UINT8 ui8ActiveROI;

    /**
     * @note Load Structure: WBS_MISC:WBS_RGB_OFFSET
     */
    IMG_INT16 ui16RGBOffset;
    /**
     * @note Load Structure: WBS_MISC:WBS_Y_OFFSET
     */
    IMG_INT16 ui16YOffset;

    /**
     * @note Load Structure: WBS_RMAX_TH
     */
    IMG_UINT16 aRMax[WBS_NUM_ROI];
    /**
     * @note Load Structure: WBS_GMAX_TH
     */
    IMG_UINT16 aGMax[WBS_NUM_ROI];
    /**
     * @note Load Structure: WBS_BMAX_TH
     */
    IMG_UINT16 aBMax[WBS_NUM_ROI];

    /**
     * @note Load Structure: WBS_YHLW_TH
     */
    IMG_UINT16 aYMax[WBS_NUM_ROI];
} CI_MODULE_WBS;

typedef struct CI_MODULE_ENS
{
    /**
     * @note Load Structure: ENS_NCOUPLES:ENS_NCOUPLES_EXP
     */
    IMG_UINT8 ui8Log2NCouples;

    /**
     * @note Load Structure: ENS_REGION_REGS:ENS_SUBS_EXP
     */
    IMG_UINT8 ui8SubExp;
} CI_MODULE_ENS;

enum CI_RLT_MODES
{
    CI_RLT_DISABLED = 0,
    CI_RLT_LINEAR,
    CI_RLT_CUBIC,
};

typedef struct CI_MODULE_RLT
{
    /**
     * Choose between linear or cubic interpretation of the points.
     *
     * @li if linear interpolation aPoints should be considered as
     * RLT_SLICE_N segments of points allowing the configuration of a
     * curve with different knee points location.
     * @li if cubic interpolation aPoints should be considered as 1 curve
     * per colour channel
     *
     * @note Load structure: RLT_CONTROL:RLT_ENABLE and
     * RLT_CONTROL:RLT_INTERP_MODE
     */
    enum CI_RLT_MODES eMode;

    /**
     * @warning First point is assumes to 0 and is therefore not asked by HW.
     * The point naming convention in HW registers can be confusing as entry
     * 0 from SW is ODD_1.
     *
     * @note Load structure: RLT_VALUES_X:RLT_LUT_VALUES_X_POINTS entries
     */
    IMG_UINT16 aPoints[RLT_SLICE_N][RLT_SLICE_N_POINTS];
} CI_MODULE_RLT;

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the CI_API documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_MODULES_STRUCTS_H_ */
