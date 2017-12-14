/**
*******************************************************************************
 @file ci_api_structs.h

 @brief User-side driver structures

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
#ifndef CI_API_STRUCTS_H_
#define CI_API_STRUCTS_H_

#include <img_types.h>
#include <img_defs.h>

#include <felixcommon/pixel_format.h>
#include "ci/ci_modules_structs.h"
// defines CI_N_CONTEXT
#include "felix_hw_info.h"  // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup CI_API
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_API documentation module
 *---------------------------------------------------------------------------*/

/** @brief Informative structre on the state of the line store */
typedef struct CI_LINESTORE
{
    /**
     * @brief Start position of each context - if negative this context should
     * not be used for size computation
     */
    IMG_INT32 aStart[CI_N_CONTEXT];
    /** @brief Size of each context */
    IMG_UINT32 aSize[CI_N_CONTEXT];
    /** @brief State of each context */
    IMG_BOOL8 aActive[CI_N_CONTEXT];
} CI_LINESTORE;

/** @brief Possible supported features */
enum CI_INFO_eSUPPORTED
{
    CI_INFO_SUPPORTED_NONE = 0,
    /** @brief Support tiled output */
    CI_INFO_SUPPORTED_TILING = 1,
    /** @brief Support HDR Extraction formats */
    CI_INFO_SUPPORTED_HDR_EXT = CI_INFO_SUPPORTED_TILING << 1,
    /** @brief Support HDR Insertion formats */
    CI_INFO_SUPPORTED_HDR_INS = CI_INFO_SUPPORTED_HDR_EXT << 1,
    /** @brief Support Raw 2D Extraction formats */
    CI_INFO_SUPPORTED_RAW2D_EXT = CI_INFO_SUPPORTED_HDR_INS << 1,
    /** @brief Support IIF Data generator */
    CI_INFO_SUPPORTED_IIF_DATAGEN = CI_INFO_SUPPORTED_RAW2D_EXT << 1,
    /** @brief Support IIF Data generator with patern generation */
    CI_INFO_SUPPORTED_IIF_PARTERNGEN = CI_INFO_SUPPORTED_IIF_DATAGEN << 1,
};

/**
 * @brief Data Extraction (OUT) and Data Insertion points (IN)
 *
 * This is made to match with HW register value DE_CTRL
 */
enum CI_INOUT_POINTS
{
    CI_INOUT_BLACK_LEVEL = 0,
    CI_INOUT_FILTER_LINESTORE,
    /*CI_INOUT_DEMOISAICER,
    CI_INOUT_GAMA_CORRECTION,
    CI_INOUT_HSBC,
    CI_INOUT_TONE_MAPPER_LINESTORE,*/

    /** @brief Not a valid point - only counts number of points */
    CI_INOUT_NONE
};

enum CI_GASKETTYPE
{
    CI_GASKET_NONE = 0,
    CI_GASKET_MIPI,
    CI_GASKET_PARALLEL
};

/**
 * @brief Contains information about the currently available hardware
 */
typedef struct CI_HWINFO
{
    /**
     * @name Version attributes
     * @{
     */

    /** @brief imagination technologies identifier */
    IMG_UINT8 ui8GroupId;
    /** @brief identify the core family */
    IMG_UINT8 ui8AllocationId;
    /** @brief Configuration ID */
    IMG_UINT16 ui16ConfigId;

    /** @brief revision IMG division ID */
    IMG_UINT8 rev_ui8Designer;
    /** @brief revision major */
    IMG_UINT8 rev_ui8Major;
    /** @brief revision minor */
    IMG_UINT8 rev_ui8Minor;
    /** @brief revision maintenance */
    IMG_UINT8 rev_ui8Maint;

    /** @brief Unique identifier for the HW build */
    IMG_UINT32 rev_uid;

    /**
     * @}
     * @name Configuration attributes
     * @{
     */

    /** @brief Pixels per clock sent from the gasket to the ISP */
    IMG_UINT8 config_ui8PixPerClock;
    /**
     * @brief The maximum number of lines that the ISP pipeline can process
     * in parallel
     */
    IMG_UINT8 config_ui8Parallelism;
    /**
     * @brief Number of bits used at the input of the pipeline after the
     * imager interface block
     */
    IMG_UINT8 config_ui8BitDepth;
    /** @brief The number of context that can be simultaneously used. */
    IMG_UINT8 config_ui8NContexts;
    /** @brief Number of imagers supported. */
    IMG_UINT8 config_ui8NImagers;
    /** @brief Number of IIF Data Generator */
    IMG_UINT8 config_ui8NIIFDataGenerator;
    /** @brief Memory Latency */
    IMG_UINT16 config_ui16MemLatency;

    /** @brief DPF internal FIFO size (shared between all contexts) */
    IMG_UINT32 config_ui32DPFInternalSize;

    /**
     * @brief The number of entries in the RTM registers also known as the
     * number of groups in the pipeline
     */
    IMG_UINT8 config_ui8NRTMRegisters;

    /**
     * @}
     * @name Scaler information
     * @{
     */

    /**
     * @brief The number of horizontal taps used by the encoder scaler for the
     * luma component.
     */
    IMG_UINT8 scaler_ui8EncHLumaTaps;
    /**
     * @brief The number of vertical taps used by the encoder scaler for the
     * luma component.
     */
    IMG_UINT8 scaler_ui8EncVLumaTaps;
    /**
     * @brief The number of vertical taps used by the encoder scaler for the
     * chorma component.
     */
    IMG_UINT8 scaler_ui8EncVChromaTaps;

    /**
     * @brief The number of horizontal taps used by the display scaler for the
     * luma component (scaling is done before YCC to RGB conversion)
     */
    IMG_UINT8 scaler_ui8DispHLumaTaps;
    /**
     * @brief The number of vertical taps used by the display scaler for the
     * luma component (scaling is done before the YCC to RGB conversion)
     */
    IMG_UINT8 scaler_ui8DispVLumaTaps;

    /**
     * @}
     * @name Gasket attributes
     * @{
     */

    /**
     * @brief Show how many MIPI lanes are output from the imager when the
     * imager is of type MIPI.
     *
     * Shows how wide the parallel bus output from the imager when the imager
     * is of type parallel.
     */
    IMG_UINT8 gasket_aImagerPortWidth[CI_N_IMAGERS];
    /** @brief Combinaisons of @ref CI_GASKETTYPE */
    IMG_UINT8 gasket_aType[CI_N_IMAGERS];
    /** @brief One per imager - see @ref CI_HWINFO::config_ui8NImagers */
    IMG_UINT16 gasket_aMaxWidth[CI_N_IMAGERS];
    /** @brief One per imager - see @ref CI_HWINFO::config_ui8NImagers */
    IMG_UINT8 gasket_aBitDepth[CI_N_IMAGERS];

    /**
     * @}
     * @name Context attributes
     * @{
     */

    /**
     * @brief Maximum supported width for the context; one per context
     * - see @ref CI_HWINFO::config_ui8NContexts
     */
    IMG_UINT16 context_aMaxWidthMult[CI_N_CONTEXT];
    /**
     * @brief Maximum supported height for the context; one per context
     * - see @ref CI_HWINFO::config_ui8NContexts
     */
    IMG_UINT16 context_aMaxHeight[CI_N_CONTEXT];
    /**
     * @brief Size of the buffer used to arbitrate the context
     * - see @ref CI_HWINFO::config_ui8NContexts
     */
    IMG_UINT16 context_aMaxWidthSingle[CI_N_CONTEXT];
    /**
     * @brief Bit depth of the context; one per context
     * - see @ref CI_HWINFO::config_ui8NContexts
     */
    IMG_UINT8 context_aBitDepth[CI_N_CONTEXT];
    /**
     * @brief Size of the HW waiting queue; one per context
     * - see @ref CI_HWINFO::config_ui8NContexts
     */
    IMG_UINT8 context_aPendingQueue[CI_N_CONTEXT];

    /**
     * @}
     * @name Other attributes
     * @{
     */

    /**
     * @brief Information on the maximum size of the linestore
     */
    IMG_UINT32 ui32MaxLineStore;

    /**
     * @brief The Lens Shading storage RAM size in bits for 1 line
     */
    IMG_UINT32 ui32LSHRamSizeBits;

    /**
     * @brief Combination of the supported functionalities
     * @ref CI_INFO_eSUPPORTED
     */
    IMG_UINT32 eFunctionalities;

    /**
     * @brief Size of the @ref CI_PIPELINE object the kernel-driver was
     * compiled with - dummy test of driver compatibility
     */
    IMG_UINT32 uiSizeOfPipeline;

    /**
     * @brief Size of the load structure
     */
    IMG_UINT32 uiSizeOfLoadStruct;

    /** @brief Changelist saved into the driver when building it. */
    IMG_UINT32 uiChangelist;
    /** @brief 256B or 512B tiles (256Bx16 or 512Bx8) */
    IMG_UINT32 uiTiledScheme;
    /**
     * @brief 0 for bypass MMU, otherwise bitdepth of the MMU (e.g. 40 = 40b
     * physical addresses)
     */
    IMG_UINT8 mmu_ui8Enabled;
    /** @brief MMU revision major */
    IMG_UINT8 mmu_ui8RevMajor;
    /** @brief MMU revision minor */
    IMG_UINT8 mmu_ui8RevMinor;
    /** @brief MMU revision maintenance */
    IMG_UINT8 mmu_ui8RevMaint;
    /** @brief device mmu page size */
    IMG_UINT32 mmu_ui32PageSize;
    /**
     * @brief Reference clock in Mhz
     *
     * If changes dynamically needs another way to communicate it to
     * user-space.
     */
    IMG_UINT32 ui32RefClockMhz;

    /**
     * @}
     */
} CI_HWINFO;

/** @brief General information about the CI driver */
typedef struct CI_CONNECTION
{
    /**
     * @brief Information on the linestore status of the library - start
     * element used when setting new linestore
     */
    CI_LINESTORE sLinestoreStatus;
    /**
     * @brief Information about the hardware loaded from registers
     */
    CI_HWINFO sHWInfo;
    CI_MODULE_GMA_LUT sGammaLUT;
} CI_CONNECTION;

/**
 * @brief Used to select the Statistics output of the pipeline
 *
 * @note Similar to the available fields of SAVE_CONFIG_FLAGS register but
 * does not include Display or Encoder output.
 * It does not have to be the same than the register offset
 */
enum CI_SAVE_CONFIG_FLAGS
{
    CI_SAVE_NO_STATS = 0,

    CI_SAVE_EXS_GLOBAL = 1,
    CI_SAVE_EXS_REGION = CI_SAVE_EXS_GLOBAL << 1,
    CI_SAVE_FOS_ROI = CI_SAVE_EXS_REGION << 1,
    CI_SAVE_FOS_GRID = CI_SAVE_FOS_ROI << 1,
    CI_SAVE_WHITEBALANCE = CI_SAVE_FOS_GRID << 1,
    // FIXME how to name this flag?
    CI_SAVE_WHITEBALANCE_EXT = CI_SAVE_WHITEBALANCE << 1,
    CI_SAVE_HIST_GLOBAL = CI_SAVE_WHITEBALANCE_EXT << 1,
    CI_SAVE_HIST_REGION = CI_SAVE_HIST_GLOBAL << 1,
    CI_SAVE_FLICKER = CI_SAVE_HIST_REGION << 1,
    CI_SAVE_CRC = CI_SAVE_FLICKER << 1,
    CI_SAVE_TIMESTAMP = CI_SAVE_CRC << 1,
    CI_SAVE_ENS = CI_SAVE_TIMESTAMP << 1,

    CI_SAVE_ALL = (CI_SAVE_ENS << 1) - 1
};

enum CI_MODULE_UPDATE
{
    CI_UPD_NONE = 0,
    CI_UPD_BLC = 1,
    CI_UPD_RLT = (CI_UPD_BLC << 1),
    CI_UPD_LSH = (CI_UPD_RLT << 1),
    CI_UPD_WBC = (CI_UPD_LSH << 1),
    CI_UPD_DNS = (CI_UPD_WBC << 1),
    CI_UPD_DPF = (CI_UPD_DNS << 1),
    CI_UPD_DPF_INPUT = (CI_UPD_DPF << 1),
    CI_UPD_LCA = (CI_UPD_DPF_INPUT << 1),
    // add DMO
    CI_UPD_CCM = (CI_UPD_LCA << 1),
    CI_UPD_MGM = (CI_UPD_CCM << 1),
    CI_UPD_GMA = (CI_UPD_MGM << 1),
    CI_UPD_R2Y = (CI_UPD_GMA << 1),
    CI_UPD_MIE = (CI_UPD_R2Y << 1),
    CI_UPD_TNM = (CI_UPD_MIE << 1),
    CI_UPD_SHA = (CI_UPD_TNM << 1),
    CI_UPD_ESC = (CI_UPD_SHA << 1),
    CI_UPD_DSC = (CI_UPD_ESC << 1),
    CI_UPD_Y2R = (CI_UPD_DSC << 1),
    CI_UPD_DGM = (CI_UPD_Y2R << 1),
    CI_UPD_EXS = (CI_UPD_DGM << 1),
    CI_UPD_FOS = (CI_UPD_EXS << 1),
    CI_UPD_WBS = (CI_UPD_FOS << 1),
    CI_UPD_AWS = (CI_UPD_WBS << 1),
    CI_UPD_HIS = (CI_UPD_AWS << 1),
    CI_UPD_FLD = (CI_UPD_HIS << 1),
    /* should not be updated while running */
    // CI_UPD_ENS = (CI_UPD_FLD << 1),

    CI_UPD_ALL = ((CI_UPD_FLD << 1) - 1)
};

/** @brief The module that need access to registers when updated */
#define CI_UPD_REG (CI_UPD_LSH|CI_UPD_DPF_INPUT)

/** @brief The pipeline configuration */
typedef struct CI_PIPELINE_CONFIG
{
    /** @brief Encoder output format */
    PIXELTYPE eEncType;
    /** @brief Display output format */
    PIXELTYPE eDispType;
    /**
     * @brief HDR Extraction output format - HW supports only
     * @ref BGR_101010_32
     */
    PIXELTYPE eHDRExtType;
    /**
     * @brief HDR Insertion format - HW supports only @ref BGR_161616_64
     */
    PIXELTYPE eHDRInsType;
    /**
     * @brief Raw 2D Extraction output format - HW supports only TIFF format
     * here
     */
    PIXELTYPE eRaw2DExtraction;

    /**
     * @brief If @ref CI_INOUT_NONE then Data Extraction is done into the
     * display buffer
     */
    enum CI_INOUT_POINTS eDataExtraction;
    /**
     * @brief Configure the wanted output (Statistics, histograms, timestamps,
     * CRC) - accumulation of @ref CI_SAVE_CONFIG_FLAGS
     */
    IMG_UINT32 eOutputConfig;

    /**
     * @brief To try to use the Encoder output line in HW with this context
     * (only with supported encoder HW)
     *
     * @note Written to FELIX_CORE:ENC_OUT_CTRL when starting the capture but
     * has to be specified before registration to the kernel-side
     * @warning Will fail to start if another context is using it
     *
     * See @ref CI_PIPELINE::ui8EncOutPulse
     */
    IMG_BOOL8 bUseEncOutLine;
    /**
     * @note written to FELIX_CORE:ENC_OUT_CTRL if using the Encoder output
     * line but has to be specified before registration to the kernel-side
     *
     * See @ref CI_PIPELINE::bUseEncOutLine
     */
    IMG_UINT8 ui8EncOutPulse;

    /** @brief HW context to use */
    IMG_UINT8 ui8Context;

    /**
     * @brief Written to registers - this value is copied to the shot's
     * equivalent when added to the pending list
     *
     * @note E.g. use this value to monitor which capture's setup was used for
     * a shot.
     *
     * See @ref CI_SHOT::ui8PrivateValue
     */
    IMG_UINT8 ui8PrivateValue;

    /**
     * @brief Used to encoder output buffer allocation (in pixels) - further
     * update of ESC output size cannot be bigger than that
     */
    IMG_UINT16 ui16MaxEncOutWidth;
    /**
     * @brief Used to encoder output buffer allocation (in pixels) - further
     * update of ESC output size cannot be bigger than that
     */
    IMG_UINT16 ui16MaxEncOutHeight;

    /**
     * @brief Used to display output buffer allocation - further update of
     * DSC output size cannot be bigger than that
     */
    IMG_UINT16 ui16MaxDispOutWidth;
    /**
     * @brief Used to display output buffer allocation - further update of DSC
     * output size cannot be bigger than that
     */
    IMG_UINT16 ui16MaxDispOutHeight;

    /**
     * @brief Use to enable allocation/importation of tiled output buffers
     */
    IMG_BOOL8 bSupportTiling;

    /**
     * @brief DPF output map size in bytes (if 0 no DPF write map allocated)
     *
     * @note Written to load Structure
     * DPF_MODIFICATIONS_MAP_SIZE_CACHE:DPF_WRITE_MAP_SIZE when triggering
     * shots
     *
     * @warning Once registers to the kernel this option cannot be changed
     */
    IMG_UINT32 ui32DPFWriteMapSize;

    /**
     * Defines the black point value of pixels for modules that make use of
     * unsigned offset pixel values.
     *
     * Black level value is set according to bit depth of Felix pipeline
     * irrespective of source bit depth and module to which it is applied.
     *
     * @note Register: SYS_BLACK_LEVEL:SYS_BLACK_LEVEL
     */
    IMG_UINT16 ui16SysBlackLevel;
} CI_PIPELINE_CONFIG;

/**
 * @brief Statistics configurations
 */
typedef struct CI_PIPELINE_STATS
{
    CI_MODULE_EXS sExposureStats;
    CI_MODULE_FOS sFocusStats;
    CI_MODULE_WBS sWhiteBalanceStats;
    CI_MODULE_AWS sAutoWhiteBalanceStats;
    CI_MODULE_HIS sHistStats;
    CI_MODULE_FLD sFlickerStats;
    /** @note in pipeline cannot be updated after registration */
    CI_MODULE_ENS sEncStats;
} CI_PIPELINE_STATS;

typedef struct CI_PIPELINE
{
    CI_PIPELINE_CONFIG config;

    /**
     * @name Main Modules
     * @{
     */

    /** @brief Imager Interface - cannot be updated after registration! */
    CI_MODULE_IIF sImagerInterface;
    /** @brief Black Level */
    CI_MODULE_BLC sBlackCorrection;
    /** @brief Raw LUT entries */
    CI_MODULE_RLT sRawLUT;
    /** @brief Configuration of the lens shading */
    CI_MODULE_LSH sDeshading;
    /** @brief Configuration of the white balance */
    CI_MODULE_WBC sWhiteBalance;
    /** @brief Denoiser */
    CI_MODULE_DNS sDenoiser;
    /** @brief Defective Pixels Correction */
    CI_MODULE_DPF sDefectivePixels;
    /** @brief Chromatic Aberration */
    CI_MODULE_LCA sChromaAberration;
    /** @brief Colour Correction */
    CI_MODULE_CCM sColourCorrection;
    /** @brief Main Gamut Mapper */
    CI_MODULE_MGM sMainGamutMapper;
    CI_MODULE_GMA sGammaCorrection;
    /** @brief Colour Conversion (RGB to YCC) */
    CI_MODULE_R2Y sRGBToYCC;
    /** @brief Main Image Enhancer */
    CI_MODULE_MIE sImageEnhancer;
    /** @brief Tone Mapper */
    CI_MODULE_TNM sToneMapping;
    /** @brief Sharpening and secondary denoiser */
    CI_MODULE_SHA sSharpening;

    /** @brief Configuration of the scaler for the encoder */
    CI_MODULE_ESC sEncoderScaler;

    /** @brief Configuration of the scaler for the display */
    CI_MODULE_DSC sDisplayScaler;
    /** @brief Colour Conversion (YCC to RGB) */
    CI_MODULE_Y2R sYCCToRGB;
    /** @brief Display Gamut Mapper */
    CI_MODULE_DGM sDisplayGamutMapper;

    /**
     * @}
     */

    /** @brief contains the statistics configuration modules */
    CI_PIPELINE_STATS stats;
} CI_PIPELINE;

/**
 * @brief Buffer ID structure - 1 ID per importable buffer
 *
 * Used to simplify API calls that would change when additional IDs are needed
 *
 * When using ION these are the ION IDs
 *
 * @warning Strides CANNOT be changed if the buffer is tiled (HW limitation).
 * If the buffer is tiled ensure a stride of 0 is given.
 */
typedef struct CI_BUFFID
{
    /** @brief Encoder buffer Identifier */
    IMG_UINT32 encId;
    /**
     * @brief Encoder Y buffer stride in bytes - if 0 ignored
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 encStrideY;
    /**
     * @brief Encoder CbCr buffer stride in bytes - if 0 ignored
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 encStrideC;
    /**
     * @brief start of luma plane offset in bytes
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 encOffsetY;
    /**
     * @brief chroma offset in bytes - if 0 computed by kernel-side to be
     * just after luma plane
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 encOffsetC;

    /** @brief Display buffer Identifier */
    IMG_UINT32 dispId;
    /**
     * @brief Display buffer stride in bytes - if 0 ignored
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 dispStride;
    /**
     * @brief Start of display buffer offset in bytes
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 dispOffset;

    /** @brief HDR Extraction buffer Identifier */
    IMG_UINT32 idHDRExt;
    /**
     * @brief HDR Extraction buffer stride in bytes - if 0 ignored
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 HDRExtStride;
    /**
     * @brief Start of HDR Extraction buffer offset in bytes
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 HDRExtOffset;

    /** @brief HDR Insertion buffer Identifier */
    IMG_UINT32 idHDRIns;
    /**
     * @brief HDR Insertion buffer stride in bytes - if 0 ignored
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 HDRInsStride;
    /**
     * @brief Start of HDR Insertion buffer offset in bytes
     *
     * @note Needs to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 HDRInsOffset;

    /** @brief Raw 2D Extraction buffer Identifier */
    IMG_UINT32 idRaw2D;
    /**
     * @brief Raw2D buffer stride in bytes - if 0 ignored
     *
     * @note Does NOT need to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 raw2DStride;
    /**
     * @brief Start of Raw2D buffer offset in bytes
     *
     * @note Does NOT need to be a multiple of SYSMEM_ALIGNMENT
     */
    IMG_UINT32 raw2DOffset;
} CI_BUFFID;


enum CI_BUFFTYPE
{
    CI_TYPE_NONE = 0,
    /** @brief YUV on the Encoder output */
    CI_TYPE_ENCODER,
    /** @brief RGB on the Display output */
    CI_TYPE_DISPLAY,
    /** @brief Bayer Data Extraction on the Display output */
    CI_TYPE_DATAEXT,
    /** @brief RGB Extraction on HDR Extraction output */
    CI_TYPE_HDREXT,
    /** @brief RGB Insertion on HDR Insertion input */
    CI_TYPE_HDRINS,
    /** @brief TIFF Bayer Extraction on RAW2D Extraction output */
    CI_TYPE_RAW2D,
};

/** @brief Information about a single captured frame */
typedef struct CI_SHOT
{
    /** @brief stride & height */
    IMG_UINT32 aEncYSize[2];
    /** @brief stride & height */
    IMG_UINT32 aEncCbCrSize[2];
    /** @brief Y offset and CbCr offset - assumes chroma is after luma */
    IMG_UINT32 aEncOffset[2];
    /** @brief is the encoder output tiled (using pipeline's tiling scheme) */
    IMG_BOOL8 bEncTiled;

    /** @brief stride & height */
    IMG_UINT32 aDispSize[2];
    /** @brief start of buffer offset in bytes */
    IMG_UINT32 ui32DispOffset;
    /** @brief is the display output tiled (using pipeline's tiling scheme) */
    IMG_BOOL8 bDispTiled;

    /** @brief stride & height */
    IMG_UINT32 aHDRExtSize[2];
    /** @brief start of buffer offset in bytes */
    IMG_UINT32 ui32HDRExtOffset;
    /** @brief is the HDR output tiled (using pipeline's tiling scheme) */
    IMG_BOOL8 bHDRExtTiled;

    /** @brief stride & height */
    IMG_UINT32 aHDRInsSize[2];
    /** @brief start of buffer offset in bytes */
    IMG_UINT32 ui32HDRInsOffset;

    /** @brief stride & height */
    IMG_UINT32 aRaw2DSize[2];
    /** @brief start of buffer offset in bytes */
    IMG_UINT32 ui32Raw2DOffset;

    /** @brief Size of the load structure in bytes */
    IMG_UINT32 loadSize;
    /** @brief Size of the statistics in bytes */
    IMG_UINT32 statsSize;

    /**
     * @brief Size of the DPF output map in bytes
     *
     * @note This is duplicated from the @ref CI_PIPELINE::ui32DPFWriteMapSize
     */
    IMG_UINT32 uiDPFSize;

    /** @brief Size of the ENS output in bytes */
    IMG_UINT32 uiENSSize;

    /**
     * @brief Easy access to the used configuration number
     *
     * See @ref CI_PIPELINE::ui8PrivateValue
     */
    IMG_UINT8 ui8PrivateValue;
    /**
     * @brief Set to IMG_TRUE if an error occurred while capturing the frame
     */
    IMG_BOOL8 bFrameError;
    /**
     * @brief Number of missed frames before acquiring that one
     *
     * @note if the value is negative it means issues with gasket frame counter
     */
    IMG_INT32 i32MissedFrames;

    IMG_UINT32 encId;
    void *pEncoderOutput;
#ifdef INFOTM_ISP
    void *pEncoderOutputPhyAddr;
#endif //INFOTM_ISP

    IMG_UINT32 dispId;
    void *pDisplayOutput;
#ifdef INFOTM_ISP
    void *pDisplayOutputPhyAddr;
#endif //INFOTM_ISP

    IMG_UINT32 HDRExtId;
    void *pHDRExtOutput;
#ifdef INFOTM_ISP
    void *pHDRExtOutputPhyAddr;
#endif //INFOTM_ISP

    IMG_UINT32 raw2DExtId;
    void *pRaw2DExtOutput;
#ifdef INFOTM_ISP
    void *pRaw2DExtOutputPhyAddr;
#endif //INFOTM_ISP
    void *pLoadStruct;
    void *pStatistics;
    void *pDPFMap;
    void *pENSOutput;
#ifdef INFOTM_ISP
    void *pENSOutputPhyAddr;
#endif //INFOTM_ISP

    /** @brief timestamps when pushed into LL - read by drivers */
    IMG_UINT32 ui32LinkedListTS;
    /** @brief timestamps when interrupt was serviced - read by drivers */
    IMG_UINT32 ui32InterruptTS;

    /* statistics configurations */
    /** @brief configuration of the output - copied from pipeline config */
    IMG_UINT32 eOutputConfig;
} CI_SHOT;

/**
 * @brief A buffer information
 *
 * Used for HDR insertion and LSH matrix buffer access
 */
typedef struct CI_BUFFER
{
    IMG_UINT32 id;
    void *data;
    IMG_UINT32 ui32Size;
} CI_BUFFER;

typedef struct CI_LSHMAT
{
    /** @brief matrix identifier - do not modify */
    IMG_UINT32 id;
    /** @brief pointer to user-side accessible data in HW format */
    void *data;
    /** @brief Size in Bytes */
    IMG_UINT32 ui32Size;
    /** @brief Associated configuration */
    CI_MODULE_LSH_MAT config;
} CI_LSHMAT;

/**
 * @brief Information about a buffer
 *
 * Can be used to retrieve information about buffers from CI user space
 */
typedef struct CI_BUFFER_INFO
{
    /** @brief the buffer identifier in CI */
    IMG_UINT32 id;
    /** @brief if imported the buffer import id */
    IMG_UINT32 ionFd;
    /** @brief in bytes */
    IMG_UINT32 ui32Size;
    /** @brief one of @ref CI_BUFFTYPE */
    int type;
    /**
     * @brief is the buffer available for capture
     *
     * If it is an output buffer available if not already given to HW and not
     * already acquired.
     *
     * If it is an HDR input buffer is available if not reserved.
     *
     * If it is an LSH matrix it is available if not currently acquired by
     * the user.
     */
    IMG_BOOL8 bAvailable;
    /** @brief is memory tiled */
    IMG_BOOL8 bIsTiled;
} CI_BUFFER_INFO;

typedef struct CI_GASKET
{
    /** @brief The gasket it refers to */
    IMG_UINT8 uiGasket;

    /** @brief Parallel or Mipi gasket (false=mipi) */
    IMG_BOOL8 bParallel;

    /**
     * @name Parallel configuration only
     * @{
     */
    /**
     * @brief Width-1 of the parallel frame
     *
     * @note Gasket Register: PARALLEL_FRAME_SIZE:PARALLEL_FRAME_WIDTH
     */
    IMG_UINT16 uiWidth;
    /**
     * @brief Height-1 of the parallel frame
     *
     * @note Gasket Register: PARALLEL_FRAME_SIZE:PARALLEL_FRAME_HEIGHT
     */
    IMG_UINT16 uiHeight;
    /**
     * @brief High or Low VSync (true=high)
     *
     * @note Gasket Register: PARALLEL_CTRL:PARALLEL_V_SYNC_POLARITY
     */
    IMG_BOOL8 bVSync;
    /**
     * @brief High or Low HSync (true=high)
     *
     * @note Gasket Register: PARALLEL_CTRL:PARALLEL_H_SYNC_POLARITY
     */
    IMG_BOOL8 bHSync;
    /**
     * @brief Bitdepth of the parallel (10 or 12)
     *
     * @note Gasket Register: PARALLEL_CTRL:PARALLEL_PIXEL_FORMAT
     */
    IMG_UINT8 ui8ParallelBitdepth;

    /**
     * @}
     */
} CI_GASKET;

typedef struct CI_GASKET_INFO
{
    /**
     * @name Information
     * @{
     */

    /**
     * @brief if None then gasket is disabled otherwise combination of
     * @ref CI_GASKETTYPE
     *
     * @note For the moment it is not very useful: this information is
     * available in the @ref CI_CONNECTION.
     * But it may be useful in the future if a gasket can have multiple types
     * and only 1 enabled.
     */
    IMG_UINT8 eType;

    /**
     * @}
     * @name Live information
     * @brief May change while running - need gasket to be enabled to be valid
     * @{
     */

    /** @warning only relevant if gasket is enabled */
    IMG_UINT32 ui32FrameCount;

    /** @warning only relevant if gasket is MIPI and enabled */
    IMG_UINT8 ui8MipiFifoFull;
    /** @warning only relevant if gasket is MIPI and enabled */
    IMG_UINT8 ui8MipiEnabledLanes;
    /** @warning only relevant if gasket is MIPI and enabled */
    IMG_UINT8 ui8MipiCrcError;
    /** @warning only relevant if gasket is MIPI and enabled */
    IMG_UINT8 ui8MipiHdrError;
    /** @warning only relevant if gasket is MIPI and enabled */
    IMG_UINT8 ui8MipiEccError;
    /** @warning only relevant if gasket is MIPI and enabled */
    IMG_UINT8 ui8MipiEccCorrected;

    /**
     * @}
     */
} CI_GASKET_INFO;

/**
 * @brief Access to the Core RTM values
 *
 * Get the actual number of read RTM entries in the
 * @ref CI_HWINFO::config_ui8NRTMRegisters value
 *
 * Get the actual number of context entries in the
 * @ref CI_HWINFO::config_ui8NContexts value
 */
typedef struct CI_RTM_INFO
{
    IMG_UINT32 aRTMEntries[FELIX_MAX_RTM_VALUES];

    IMG_UINT32 context_status[CI_N_CONTEXT];
    IMG_UINT32 context_linkEmptyness[CI_N_CONTEXT];
    IMG_UINT32 context_position[CI_N_CONTEXT][FELIX_MAX_RTM_VALUES];
}CI_RTM_INFO;

/**
 * @brief Converter formats
 * @note >= CI_DGFMT_MIPI is considered MIPI format and only supported by
 * external DG
 * @warning internal DG only supports Parallel formats!
 */
typedef enum CI_CONV_FMT
{
    CI_DGFMT_PARALLEL = 0,

    /**
     * @brief MIPI format - only supported by external DG
     *
     * @note any values >= than CI_DGFMT_MIPI is considered MIPI
     */
    CI_DGFMT_MIPI,
    /** @brief MIPI - Line Flags - only supported by external DG */
    CI_DGFMT_MIPI_LF,

    /** @brief Not a format - used to know how many format are supported */
    CI_DGFMT_N
} CI_CONV_FMT;

typedef struct CI_DATAGEN
{
    /** @brief IIF Data Generator to use */
    IMG_UINT8 ui8IIFDGIndex;
    IMG_BOOL8 bPreload;
    CI_CONV_FMT eFormat;
    IMG_UINT8 ui8FormatBitdepth;

    /** @brief Gasket the IIF DG is attached to */
    IMG_UINT8 ui8Gasket;
    /** @brief Use to ensure frames do not change bayer order when triggered */
    enum MOSAICType eBayerMosaic;
} CI_DATAGEN;

typedef struct CI_DG_FRAME
{
    void *data;
    /** @brief allocation size - info only */
    IMG_UINT32 ui32AllocSize;
    /** @brief unique frame ID - info only */
    IMG_UINT32 ui32FrameID;

    /** @note should be populated when converted */
    CI_CONV_FMT eFormat;
    /** @note should be populated when converted */
    IMG_UINT8 ui8FormatBitdepth;

    /**
     * @brief Used to ensure mosaic does not change from frame to frame
     *
     * @note should be populated when converted
     */
    enum MOSAICType eBayerMosaic;
    /**
     * @note should be populated when converted
     */
    IMG_UINT32 ui32Stride;
    /**
     * @brief Width of the frame in pixels
     *
     * @note should be populated when converted
     */
    IMG_UINT32 ui32Width;
    /**
     * @brief Packet width if using MIPI format
     *
     * @note should be populated when converted
     */
    IMG_UINT32 ui32PacketWidth;
    /**
     * @note should be populated when converted
     */
    IMG_UINT32 ui32Height;
    /**
     * @note should be populated when converted
     */
    IMG_UINT32 ui32HorizontalBlanking;
    /**
     * @note should be populated when converted
     */
    IMG_UINT32 ui32VerticalBlanking;
} CI_DG_FRAME;

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the CI_API documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_API_STRUCTS_H_ */
