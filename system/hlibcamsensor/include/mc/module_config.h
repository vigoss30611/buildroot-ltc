/**
*******************************************************************************
 @file module_config.h

 @brief Felix module configuration helper

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
#ifndef MODULES_CONFIG_H_
#define MODULES_CONFIG_H_

#include <img_types.h>
#include <ci/ci_modules_structs.h>
#include <ci/ci_api_structs.h>

#include "mc/mc_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MC Module Configuration (MC)
 * @ingroup CI_API
 * @brief Helping the configuration of the CI pipeline
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the MC documentation module
 *---------------------------------------------------------------------------*/

/**
 * @brief Converts float to clipped fixed point value
 *
 * @param fl input value
 * @param intBits field integer part
 * @param fractBits field fractional part
 * @param isSigned
 * @param dbg_regname @see IMG_clip()
 *
 * @return transformed clipped value
 * @see Delegates to IMG_clip() for clipping
 */
IMG_INT32 IMG_Fix_Clip(MC_FLOAT fl, IMG_INT32 intBits, IMG_INT32 fractBits,
    IMG_BOOL isSigned, const char *dbg_regname);

MC_FLOAT IMG_Fix_Revert(IMG_INT32 reg, IMG_INT32 intBits,
    IMG_INT32 fractBits, IMG_BOOL isSigned, const char *dbg_regname);

/**
 * @brief Clips a value
 *
 * @param val input value
 * @param nBits field total size (integer+fractional)
 * @param isSigned
 * @param dbg_regname used with debug printing if the value was clipped
 */
IMG_INT32 IMG_clip(IMG_INT32 val, IMG_INT32 nBits, IMG_BOOL isSigned,
    const char *dbg_regname);
/*void IMG_range(IMG_INT32 nBits, IMG_BOOL isSigned, IMG_INT32 *pMin,
    IMG_INT32 *pMax);*/

/**
 * @name Configuration functions
 * @brief Initialise the modules to have a default ready to use CI pipeline
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Configuration functions group
 *---------------------------------------------------------------------------*/

/** @warning Only does a memset of 0 */
IMG_RESULT MC_IIFInit(MC_IIF *pIIF);

IMG_RESULT MC_BLCInit(MC_BLC *pBLC, IMG_UINT8 ui8BitDepth);

IMG_RESULT MC_RLTInit(MC_RLT *pRLT);

IMG_RESULT MC_LSHInit(MC_LSH *pLSH);

IMG_RESULT MC_WBCInit(MC_WBC *pWBC);

IMG_RESULT MC_DPFInit(MC_DPF *pDPF, const CI_HWINFO *pHWInfo,
    const MC_IIF *pIIF);
void MC_DPFFree(MC_DPF *pDPF);

IMG_RESULT MC_LCAInit(MC_LCA *pLCA, const MC_IIF *pIIF);

IMG_RESULT MC_MGMInit(MC_MGM *pMGM);

IMG_RESULT MC_GMAInit(MC_GMA *pGMA);

IMG_RESULT MC_DNSInit(MC_DNS *pDNS);

IMG_RESULT MC_CCMInit(MC_CCM *pCCM);

IMG_RESULT MC_R2YInit(MC_R2Y *pR2Y);

IMG_RESULT MC_Y2RInit(MC_Y2R *pY2R);

IMG_RESULT MC_MIEInit(MC_MIE *pMie);

IMG_RESULT MC_TNMInit(MC_TNM *pTNM, const MC_IIF *pIIF,
    const CI_HWINFO *pHWInfo);

IMG_RESULT MC_ESCInit(MC_ESC *pESC, const MC_IIF *pIIF);

IMG_RESULT MC_DSCInit(MC_DSC *pDSC, const MC_IIF *pIIF);

IMG_RESULT MC_DGMInit(MC_DGM *pDGM);

IMG_RESULT MC_SHAInit(MC_SHA *pSHA);

// statistics

IMG_RESULT MC_EXSInit(MC_EXS *pEXS, const MC_IIF *pIIF);

IMG_RESULT MC_FOSInit(MC_FOS *pFOS, const MC_IIF *pIIF);

IMG_RESULT MC_FLDInit(MC_FLD *pFLD, const MC_IIF *pIIF);

IMG_RESULT MC_HISInit(MC_HIS *pHIS, const MC_IIF *pIIF);

IMG_RESULT MC_WBSInit(MC_WBS *pWBS, const MC_IIF *pIIF);

IMG_RESULT MC_AWSInit(MC_AWS *pAWS, const MC_IIF *pIIF);

IMG_RESULT MC_ENSInit(MC_ENS *pENS);

/**
 * @name Extraction functions
 * @brief Convert the statistics blob information into structures (meta-data)
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Extraction functions group
 *---------------------------------------------------------------------------*/

IMG_RESULT MC_FOSExtract(const void *blob, MC_STATS_FOS *pFOSStats);

IMG_RESULT MC_EXSExtract(const void *blob, MC_STATS_EXS *pEXSStats);

IMG_RESULT MC_HISExtract(const void *blob, MC_STATS_HIS *pHISStats);

IMG_RESULT MC_WBSExtract(const void *blob, MC_STATS_WBS *pWBSStats);

IMG_RESULT MC_AWSExtract(const void *blob, MC_STATS_AWS *pAWSStats);

IMG_RESULT MC_FLDExtract(const void *blob, MC_STATS_FLD *pFLDStats);

/** @warning some timestamps info are not from the HW data */
IMG_RESULT MC_TimestampExtract(const void *blob,
    MC_STATS_TIMESTAMP *pTimestamps);

IMG_RESULT MC_DPFExtract(const void *blob, MC_STATS_DPF *pDPFStats);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Configuration functions group
 *---------------------------------------------------------------------------*/

/**
 * @name Pipeline functions
 * @brief Initialise and convert a whole pipeline
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Conversion functions group
 *---------------------------------------------------------------------------*/

/**
 * @brief Initialise the structure and pre-compute some data from IIF setup
 *
 * @warning IIF is not setup! it should be done before calling that function!
 */
IMG_RESULT MC_PipelineInit(MC_PIPELINE *pMCPipeline,
    const CI_HWINFO *pHWInfo);

/**
 * @brief Call the conversion function of all MC elements in MC_PIPELIEN
 * and the global elements too
 */
IMG_RESULT MC_PipelineConvert(const MC_PIPELINE *pMCPipeline,
    CI_PIPELINE *pCIPipeline);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Configuration functions group
 *---------------------------------------------------------------------------*/

/**
 * @name Conversion functions
 * @brief Convert the logical level information into the register information
 * expected by CI
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Conversion functions group
 *---------------------------------------------------------------------------*/

IMG_RESULT MC_IIFConvert(const MC_IIF *pMC_IIF, CI_MODULE_IIF *pCI_IIF);

IMG_RESULT MC_BLCConvert(const MC_BLC *pMC_BLC, CI_MODULE_BLC *pCI_BLC);

IMG_RESULT MC_RLTConvert(const MC_RLT *pMC_RLT, CI_MODULE_RLT *pCI_RLT);

IMG_RESULT MC_LSHConvert(const MC_LSH *pMC_LSH, CI_MODULE_LSH *pCI_LSH);

/**
 * @brief Compute the minimum number of bits per difference the matrix will
 * need.
 *
 * @warning Goes through the whole matrix to compute all differences so may
 * be slow
 *
 * @param[in] pLSH
 * @param[out] pMaxDiff if not NULL contains the maximum difference
 * found
 *
 * @return number of bits per diff to use (minimum supported by HW applied
 * - but maximum not verified).
 * @return 0 if pLSH is NULL (or without matrices) or contains negative values
 */
IMG_UINT8 MC_LSHComputeMinBitdiff(const LSH_GRID *pLSH, int *pMaxDiff);

/**
 * @brief Compute the allocation size needed for a grid
 *
 * @param[in] pLSH matrix
 * @param[in] ui8BitsPerDiff chosen size of difference (not checked to meet
 * HW limits)
 * @param[out] pLineSize optional output of the line size in Bytes meeting HW
 * registers requirements
 * @warning Register expects a multiple of 16 Bytes not the value in Bytes
 * @param[out] pStride optional output of the line stride in bytes meeting HW
 * requirements
 */
IMG_UINT32 MC_LSHGetSizes(const LSH_GRID *pLSH, IMG_UINT8 ui8BitsPerDiff,
    IMG_UINT32 *pLineSize, IMG_UINT32 *pStride);

/**
 * @brief Converts a grid from the LSH_GRID to the CI_LSHMAT format.
 *
 * @warning Not set by this function: CI_LSHMAT::ui16SkipX
 * CI_LSHMAT::ui16SkipY CI_LSHMAT::ui16OffsetX CI_LSHMAT::ui16OffsetY
 */
IMG_RESULT MC_LSHConvertGrid(const LSH_GRID *pLSH, IMG_UINT8 ui8BitsPerDiff,
    CI_LSHMAT *pMatrix);

/**
 * @brief Can be used in test applications that generate matrices to fix
 * the differences to fit in the maximum range
 *
 * Does not check if ui8BitsPerDiff or pLSH->ui16TileSize are valid
 */
IMG_RESULT MC_LSHPreCheckTest(LSH_GRID *pLSH, IMG_UINT8 ui8BitsPerDiff);

IMG_RESULT MC_WBCConvert(const MC_WBC *pMC_WBC, CI_MODULE_WBC *pCI_WBC);

/**
 * @note Output map size copied when converting the pipeline
 * (MC_PipelineConvert())
 */
IMG_RESULT MC_DPFConvert(const MC_DPF *pMC_DPF, const MC_IIF *pIIF,
    CI_MODULE_DPF *pCI_DPF);
IMG_RESULT MC_DPFRevert(const CI_MODULE_DPF *pCI_DPF, IMG_UINT32 eConfig,
    MC_DPF *pMC_DPF);

IMG_RESULT MC_LCAConvert(const MC_LCA *pMC_LCA, CI_MODULE_LCA *pCI_LCA);

IMG_RESULT MC_MGMConvert(const MC_MGM *pMC_MGM, CI_MODULE_MGM *pCI_MGM);

IMG_RESULT MC_GMAConvert(const MC_GMA *pMC_GMA, CI_MODULE_GMA *pCI_GMA);

IMG_RESULT MC_DNSConvert(const MC_DNS *pMC_DNS, CI_MODULE_DNS *pCI_DNS);

IMG_RESULT MC_CCMConvert(const MC_CCM *pMC_CCM, CI_MODULE_CCM *pCI_CCM);

IMG_RESULT MC_R2YConvert(const MC_R2Y *pMC_R2Y, CI_MODULE_R2Y *pCI_R2Y);

IMG_RESULT MC_Y2RConvert(const MC_Y2R *pMC_Y2R, CI_MODULE_Y2R *pCI_Y2R);

IMG_RESULT MC_MIEConvert(const MC_MIE *pMC_MIE, CI_MODULE_MIE *pCI_MIE,
    const CI_HWINFO *pInfo);

IMG_RESULT MC_TNMConvert(const MC_TNM *pMC_TNM, const MC_IIF *pIIF,
    IMG_UINT8 ui8Paralelism, CI_MODULE_TNM *pCI_TNM);

/** choose bOutput422 from pipeline's output format */
IMG_RESULT MC_ESCConvert(const MC_ESC *pMC_ESC, CI_MODULE_ESC *pCI_ESC,
    IMG_BOOL8 bOutput422);

IMG_RESULT MC_DSCConvert(const MC_DSC *pMC_DSC, CI_MODULE_DSC *pCI_DSC);

IMG_RESULT MC_DGMConvert(const MC_DGM *pMC_DGM, CI_MODULE_DGM *pCI_DGM);

IMG_RESULT MC_SHAConvert(const MC_SHA *pMC_SHA, CI_MODULE_SHA *pCI_SHA,
    const CI_HWINFO *pInfo);

// statistics

/** @brief converts MC Exposure stats configuration to CI registers */
IMG_RESULT MC_EXSConvert(const MC_EXS *pMC_EXS, CI_MODULE_EXS *pCI_EXS);
/**
 * @brief extract MC Exposure stats configuration from CI registers
 *
 * @param[in] pCI_EXS
 * @param eConfig flags used as output for the pipeline
 * (see @ref CI_PIPELINE::eOutputConfig)
 * @param[out] pMC_EXS
 */
IMG_RESULT MC_EXSRevert(const CI_MODULE_EXS *pCI_EXS, IMG_UINT32 eConfig,
    MC_EXS *pMC_EXS);

IMG_RESULT MC_FOSConvert(const MC_FOS *pMC_FOS, CI_MODULE_FOS *pCI_FOS);
IMG_RESULT MC_FOSRevert(const CI_MODULE_FOS *pCI_FOS, IMG_UINT32 eConfig,
    MC_FOS *pMC_FOS);

IMG_RESULT MC_FLDConvert(const MC_FLD *pMC_FLD, CI_MODULE_FLD *pCI_FLD);
IMG_RESULT MC_FLDRevert(const CI_MODULE_FLD *pCI_FLD, IMG_UINT32 eConfig,
    MC_FLD *pMC_FLD);

IMG_RESULT MC_HISConvert(const MC_HIS *pMC_HIS, CI_MODULE_HIS *pCI_HIS);
IMG_RESULT MC_HISRevert(const CI_MODULE_HIS *pCI_HIS, IMG_UINT32 eConfig,
    MC_HIS *pMC_HIS);

IMG_RESULT MC_WBSConvert(const MC_WBS *pMC_WBS, CI_MODULE_WBS *pCI_WBS);
IMG_RESULT MC_WBSRevert(const CI_MODULE_WBS *pCI_WBS, IMG_UINT32 eConfig,
    MC_WBS *pMC_WBS);

IMG_RESULT MC_AWSConvert(const MC_AWS *pMC_AWS, CI_MODULE_AWS *pCI_AWS);
IMG_RESULT MC_AWSRevert(const CI_MODULE_AWS *pCI_AWS, IMG_UINT32 eConfig,
    MC_AWS *pMC_AWS);

IMG_RESULT MC_ENSConvert(const MC_ENS *pMC_ENS, CI_MODULE_ENS *pCI_ENS);
IMG_RESULT MC_ENSRevert(const CI_MODULE_ENS *pCI_ENS, IMG_UINT32 eConfig,
    MC_ENS *pMC_ENS);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Conversion functions group
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

#endif /* MODULES_CONFIG_H_ */
