/**
*******************************************************************************
 @file ci_hwstruct.h

 @brief Internal hardware structures descriptions

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
#ifndef CI_HWSTRUCT_H_
#define CI_HWSTRUCT_H_

#include <img_types.h>

#include "ci_internal/sys_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "ci/ci_modules_structs.h"
#include "ci_kernel/ci_kernel_structs.h"

/**
 * @defgroup HW_CI Load structure into or from HW registers (HW_CI)
 * @ingroup KRN_CI_API
 * @brief Functions that are used to configure the HW registers/memory
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the HW_CI documentation module
 *---------------------------------------------------------------------------*/

/**
 * @name HW structure size access
 * @brief Access to sizes from structure definitions to avoid including
 * register definition headers in files that only need the size for allocation
 * @{
 */

/**
 * @brief Access the Linked List hw structure size in Bytes from register 
 * definitions
 */
IMG_SIZE HW_CI_LinkedListSize(void);

/**
 * @brief Access the Load Structure hw structure size in Bytes from register 
 * definitions
 */
IMG_SIZE HW_CI_LoadStructureSize(void);

/**
 * @brief Access the Save Structure hw structure size in Bytes from register 
 * definitions
 */
IMG_SIZE HW_CI_SaveStructureSize(void);

/**
 * @}
 */
/**
 * @name Interrupt Handler
 * @{
 */

/**
 * @brief Interrupt handler function! SHOULD NOT BE CALLED DIRECTLY
 * 
 * This function should be registered to the interrupt handling mechanism as
 * the top-half (hard) interrupt handler
 */
irqreturn_t HW_CI_DriverHardHandleInterrupt(int irq, void *dev_id);


/**
 * @brief Interrupt handler function! SHOULD NOT BE CALLED DIRECTLY
 *
 * This function should be registered to the interrupt handling mechanism as
 * the bottom-half (threaded) itnerrupt handler
 */
irqreturn_t HW_CI_DriverThreadHandleInterrupt(int irq, void *dev_id);

/**
 * @}
 */
/**
 * @name Register write
 * @{
 */

/**
 * @brief Write the registers when starting the capture
 *
 * The HW is considered as "fail to start" if:
 * @li the capture mode is not disbaled, or
 * @li the linked list emptyness is 0
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if the capture is not marked as started in 
 * SW (KRN_CI_PIPELINE::bStarted)
 * @return IMG_ERROR_FATAL if the bayer format is unknown, or the HW failed to
 * start
 */
IMG_RESULT HW_CI_PipelineRegStart(KRN_CI_PIPELINE *pCapture);

/**
 * @brief Write the registers to stop the capture
 *
 * Done in that order:
 * @li write interrupt enabled to 0 to stop receiving interrupts for that
 * context
 * @li make the HW in disable mode
 * @li if bPoll waits for context to become idle
 * @li clear the linked list
 */
void HW_CI_PipelineRegStop(KRN_CI_PIPELINE *pCapture, IMG_BOOL8 bPoll);

/**
 * @brief Write the Gamma Look Up Table to registers
 *
 * @param pGamma - asserts it is not NULL
 */
void HW_CI_Reg_GMA(const CI_MODULE_GMA_LUT *pGamma);

/**
 * @brief Write the Deshading part of LSH to registers
 *
 * @warning This should only be called if the pipeline has no pending buffers!
 *
 * @note ASSUMES the Pipeline currently has its context locked!
 *
 * @param pCapture asserts it is not NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if the Pipeline is not marked as started by
 * SW (KRN_CI_PIPELINE::bStarted)
 */
IMG_RESULT HW_CI_Reg_LSH_DS(KRN_CI_PIPELINE *pCapture);

/** @warning do not call that if the capture is already running! */
IMG_RESULT HW_CI_DPF_ReadMapConvert(KRN_CI_PIPELINE *pCapture,
    IMG_UINT16 *apDefectRead, IMG_UINT32 ui32NDefect);

/** expects the DPF read map to be already converted */
void HW_CI_Reg_DPF(KRN_CI_PIPELINE *pCapture);

IMG_RESULT HW_CI_WriteGasket(CI_GASKET *pGasket, int enable);

IMG_UINT32 HW_CI_GasketFrameCount(IMG_UINT32 ui32Gasket);

void HW_CI_DriverPowerManagement(enum PwrCtrl eCtrl);

/**
 * @brief Additional power management to force context clock for BRN48137
 */
void HW_CI_DriverPowerManagement_BRN48137(IMG_UINT8 ui8Context,
    enum PwrCtrl eCtrl);

void HW_CI_DriverTimestampManagement(void);

/**
 * @brief enables the tiling scheme according to the MMU setup in 
 * FELIX_CORE:FELIX_CONTROL
 */
void HW_CI_DriverTilingManagement(void);

void HW_CI_DriverResetHW(void);

/**
 * @brief Enables or disables the Encoder low latency output
 *
 * @param ui8Context if < CI_N_CONTEXT enables that context otherwise disables
 * the output
 * @param ui8EncOutPulse value to write into ENC_OUT_PULSE_WTH_MINUS1 field
 */
void HW_CI_DriverWriteEncOut(IMG_UINT8 ui8Context, IMG_UINT8 ui8EncOutPulse);

void HW_CI_PipelineEnqueueShot(KRN_CI_PIPELINE *pPipeline, KRN_CI_SHOT *pShot);

/**
 * @}
 */
/**
 * @name Register read
 * @{
 */

/**
 * @brief Reads the core registers to fill the info structure
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if values in the core ID are unexpected
 */
IMG_RESULT HW_CI_DriverLoadInfo(KRN_CI_DRIVER *pDriver, CI_HWINFO *pInfo);

void HW_CI_ReadGasket(CI_GASKET_INFO *pInfo, IMG_UINT8 uiGasket);

/**
 * @brief Read the current time-stamp value
 * @param[out] pTimestamps asserts it is not NULL
 */
void HW_CI_ReadTimestamps(IMG_UINT32 *pTimestamps);

void HW_CI_ReadCurrentDEPoint(KRN_CI_DRIVER *pKrnDriver,
    enum CI_INOUT_POINTS *eCurrent);

void HW_CI_UpdateCurrentDEPoint(enum CI_INOUT_POINTS eWanted);

/**
 * @brief Read the Core RTM registers for the given value.
 *
 * @warning asserts that ui8RTM is smaller than
 * @ref CI_HWINFO::config_ui8NRTMRegisters
 */
IMG_UINT32 HW_CI_ReadCoreRTM(IMG_UINT8 ui8RTM);

/**
 * @brief Read the Context RTM registers
 *
 * @warning asserts that ctx is smaller than
 * @ref CI_HWINFO::config_ui8NContexts
 */
void HW_CI_ReadContextRTM(IMG_UINT8 ctx, CI_RTM_INFO *pRTMInfo);

#ifdef INFOTM_ISP
void HW_CI_ReadContextToPrint(IMG_UINT8 ctx);
#endif //INFOTM_ISP

/**
 * @}
 */
/**
 * @name Load Structure write (module configuration)
 * @{
 */

/**
 * @brief Compute the needed size to allocate the ENS
 */
IMG_UINT32 HW_CI_EnsOutputSize(IMG_UINT8 ui8NcoupleExp,
    IMG_UINT32 uiImageHeight);

/**
 * @brief Load Black Level into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pBlack asserts it is not NULL
 */
void HW_CI_Load_BLC(char *pMemory, const CI_MODULE_BLC *pBlack);

/**
 * @brief Load Raw Look Up Table into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pRawLut asserts it is not NULL
 */
void HW_CI_Load_RLT(char *pMemory, const CI_MODULE_RLT *pRawLut);

/**
 * @brief White Balance par of LSH update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pWBC asserts it is not NULL
 */
void HW_CI_Load_LSH_WB(char *pMemory, const CI_MODULE_WBC *pWBC);

/**
 * @brief Deshading part of the LSH update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pLens asserts it is not NULL
 */
void HW_CI_Load_LSH_DS(char *pMemory, const CI_MODULE_LSH *pLens);

void HW_CI_Load_DPF(char *pMemory, const CI_MODULE_DPF *pDefective);

void HW_CI_Update_DPF(char *pMemory, IMG_UINT32 ui32DPFOutputMapSize);

/**
 * @brief YCC to RGB update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pConv asserts it is not NULL
 */
void HW_CI_Load_Y2R(char *pMemory, const CI_MODULE_Y2R *pConv);

/**
 * @brief RGB to YCC update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pConv asserts it is not NULL
 */
void HW_CI_Load_R2Y(char *pMemory, const CI_MODULE_R2Y *pConv);

/**
 * @brief Latteral Chromatic Aberration update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pChroma
 */
void HW_CI_Load_LCA(char *pMemory, const CI_MODULE_LCA *pChroma);

/**
 * @brief Colour Correction update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pCorr asserts it is not NULL
 */
void HW_CI_Load_CCM(char *pMemory, const CI_MODULE_CCM *pCorr);

/**
 * @brief Image Enhancer update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pMie asserts it is not NULL
 */
void HW_CI_Load_MIE(char *pMemory, const CI_MODULE_MIE *pMie);

/**
 * @brief Tone Mapper update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pMap asserts it is not NULL
 */
void HW_CI_Load_TNM(char *pMemory, const CI_MODULE_TNM *pMap);

/**
 * @brief Generic scaler update into Load structure
 *
 * Will update either ESC or DSC according to pScaler->bIsDisplay
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pScaler asserts it is not NULL
 */
void HW_CI_Load_Scaler(char *pMemory, const struct CI_MODULE_SCALER *pScaler);

/**
 * @brief Denoiser update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pDenoiser asserts it is not NULL
 */
void HW_CI_Load_DNS(char *pMemory, const CI_MODULE_DNS *pDenoiser);

/**
 * @brief Main Gamut Mapper update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pGamut asserts it is not NULL
 */
void HW_CI_Load_MGM(char *pMemory, const CI_MODULE_MGM *pGamut);

/**
 * @brief Display Gamut Mapper update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pGamut asserts it is not NULL
 */
void HW_CI_Load_DGM(char *pMemory, const CI_MODULE_DGM *pGamut);

/**
 * @brief Gamma control update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pGamma asserts it is not NULL
 *
 * @return IMG_SUCCESS
 */
void HW_CI_Load_GMA(char *pMemory, const CI_MODULE_GMA *pGamma);

/**
 * @brief Sharpening update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pSharp asserts it is not NULL
 */
void HW_CI_Load_SHA(char *pMemory, const CI_MODULE_SHA *pSharp);

/**
 * @}
 */
/**
 * @name Load Structure write (statistics configuration)
 * @{
 */

/**
 * @brief Exposure statistics control update into Load structure
 *
 * @param pMemory system memory of the load structure - asserts it is not NULL
 * @param pExposure asserts it is not NULL
 */
void HW_CI_Load_EXS(char *pMemory, const CI_MODULE_EXS *pExposure);

/**
 * @brief Focus statistics control update into Load structure
 */
void HW_CI_Load_FOS(char *pMemory, const CI_MODULE_FOS *pFocus);

/**
 * @brief Flicker statistics control update into Load structure
 */
void HW_CI_Load_FLD(char *pMemory, const CI_MODULE_FLD *pFlicker);

/**
 * @brief Histograms statistics control update into Load structure
 */
void HW_CI_Load_HIS(char *pMemory, const CI_MODULE_HIS *pHistograms);

/**
 * @brief White Balance statistics control update into Load structure
 */
void HW_CI_Load_WBS(char *pMemory, const CI_MODULE_WBS *pWhiteBalance);

/**
 * @brief Encoder statistics control update into Load structure
 */
void HW_CI_Load_ENS(char *pMemory, const CI_MODULE_ENS *pEncStats);

/**
 * @brief Encoder statistics control update into Load structure
 */
void HW_CI_Load_ENS(char *pMemory, const CI_MODULE_ENS *pEncStats);

/**
 * @}
 */
/**
 * @name Linked list write
 * @{
 */

/**
 * @brief Configures the HW Linked List from the Shot structure
 *
 * @param[in,out] pBuffer
 * @param[in] eConfigOutput see CI_PIPELINE::eOutputConfig
 * @param[in] ui32LoadConfigReg as a combinaison of CI_MODULE_UPDATE
 *
 * @warning Image buffers should already be allocated.
 * @note call this if updates in the pipeline are needed (e.g. new statistics
 * output)
 */
IMG_RESULT HW_CI_ShotLoadLinkedList(KRN_CI_SHOT *pBuffer,
    unsigned int eConfigOutput, IMG_UINT32 ui32LoadConfigReg);

/**
 * @brief Update the HW linked list addresses of the shot (and strides)
 *
 * @note does not update the output formats
 * @warning does not update device memory
 */
void HW_CI_ShotUpdateAddresses(KRN_CI_SHOT *pShot);

/**
 * @brief Update a submitted shot load structure from its pipeline's 
 * pLoadStructStamp
 */
void HW_CI_ShotUpdateSubmittedLS(KRN_CI_SHOT *pShot);

/**
 * @brief Update a shot load structure from its pipeline's pLoadStructStamp
 */
void HW_CI_ShotUpdateLS(KRN_CI_SHOT *pShot);

/**
 * @}
 */
/**
 * @name Internal Datagen HW controls
 * @{
 */

/**
 * @brief Writes HW registers to start data generator
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if failed to POL on reset
 * @return IMG_ERROR_NOT_SUPPORTED if the chosen format is not supported by HW
 */
IMG_RESULT HW_CI_DatagenStart(KRN_CI_DATAGEN *pDatagen);

// assumes the DG has no more frames to capture
IMG_RESULT HW_CI_DatagenStop(KRN_CI_DATAGEN *pDatagen);

IMG_RESULT HW_CI_DatagenTrigger(KRN_CI_DGBUFFER *pDGBuffer);

IMG_UINT32 HW_CI_DatagenFrameCount(IMG_UINT32 ui32InternalDatagen);

/**
 * @}
 */
/**
 * @}
 */ 
/*-----------------------------------------------------------------------------
 * End of the HW_CI documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_HWSTRUCT_H_ */
