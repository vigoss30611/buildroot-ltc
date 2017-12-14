/**
*******************************************************************************
 @file ci_api.h

 @brief User-side driver functions

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
#ifndef CI_API_H_
#define CI_API_H_

#include <img_types.h>
#include <img_defs.h>

#include "ci/ci_modules.h"
#include "ci/ci_api_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _sSimImageIn_;  /* defined in sim_image.h */

/**
 * @defgroup CI_API Capture Interface: library documentation
 * @brief Documentation of the Capture Interface user-side driver.
 *
 * Other pages that may be useful:
 * @li @link CI_USER_howToUse How to use CI user-side library @endlink
 * @li @ref KRN_CI_API
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_API documentation module
 *---------------------------------------------------------------------------*/

/**
 * @defgroup CI_API_DRIVER Driver functions (CI_Driver)
 * @ingroup CI_API
 * @brief Manage the connection and global interaction with the kernel-side
 * driver
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Connection functions group
 *---------------------------------------------------------------------------*/

/**
 * @brief Initialise the drivers and return a handle to the connection
 *
 * @note Does not fail if the driver is already initialise. It only returns a 
 * handle to the driver.
 *
 * @warning If the CI is compiled as a fake kernel module the internal function 
 * KRN_CI_DriverCreate() should be called prior to that function.
 *
 * @param[in,out] ppConnection returned pointer to a capture interface handle
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if ppConnection is NULL
 * @return IMG_ERROR_ALREADY_INITIALISED if the connection is already 
 * established
 * @return IMG_ERROR_FATAL if size check failed
 */
IMG_RESULT CI_DriverInit(CI_CONNECTION **ppConnection);

/**
 * @brief Free the connection
 *
 * @warning If the CI is compiled as a fake kernel module the internal function 
 * KRN_CI_DriverDestroy() should be called after that function.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pConnection is NULL
 */
IMG_RESULT CI_DriverFinalise(CI_CONNECTION *pConnection);

/**
 * @brief Verify that a linestore respects the basic linestore rules.
 *
 * @note Computes the aSize elements from the aStart elements
 *
 * Linestore rules are:
 * @li the sum of the sizes are lower than the maximum value
 * @li each context's linestore does not overlap with another
 *
 * @warning Does not check if a linestore is active and can be changed.
 *
 * @param pConnection
 * @param[in,out] pLinestore
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_UNEXPECTED_STATE if driver is not initialised
 * @return IMG_ERROR_INVALID_PARAMETERS if pConnection is NULL
 * @return IMG_ERROR_FATAL if context's linestore overlaps
 *
 * @see Delegates to CI_LinestoreComputeSizes
 */
IMG_RESULT CI_DriverVerifLinestore(CI_CONNECTION *pConnection,
    CI_LINESTORE *pLinestore);

/**
 * @brief Updates the connection linstore information using the one from the 
 * kernel-side driver
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_UNEXPECTED_STATE if the driver was not initialised
 * @return error value from the IOCTL call
 */
IMG_RESULT CI_DriverGetLinestore(CI_CONNECTION *pConnection);

/**
 * @brief Use the connection linestore's start information to propose a new 
 * linestore to the kernel-side driver
 *
 * @warning Linestore start position cannot be moved or its size reduced while 
 * active.
 *
 * @note To disable the usage of a context set its start point to be negative
 *
 * @param pConnection
 * @param[in] pLinestore use start information to try to apply a new linestore 
 * to the driver
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pConnection is NULL
 *
 * @see Delegates to CI_DriverVerifLinestore to verify initial linestore rules. 
 * Verify HW availability and updates driver's info with 
 * INT_CI_DriverSetLineStore
 */
IMG_RESULT CI_DriverSetLinestore(CI_CONNECTION *pConnection,
    CI_LINESTORE *pLinestore);

/**
 * @brief Update the connection Gamma Look Up Table using the one from the 
 * kernel-side driver
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT CI_DriverGetGammaLUT(CI_CONNECTION *pConnection);

/**
 * @brief Update the kernel-side Gamma Look Up Table if possible
 *
 * The Gamma LUT cannot be updated if any context is running.
 * Moreover this table should not be updated on normal behaviours.
 *
 * @param pConnection
 * @param pNewGammaLUT new GMA LUT table (will be copied to the CI_CONNECTION 
 * one on success
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_UNEXPECTED_STATE if the driver is not connected
 * @return IMG_ERROR_NOT_SUPPORTED if the GMA LUT was not updated
 */
IMG_RESULT CI_DriverSetGammaLUT(CI_CONNECTION *pConnection,
    const CI_MODULE_GMA_LUT *pNewGammaLUT);

/**
 * @brief Get the current timestamp in the HW
 *
 * @param pConnection
 * @param[out] puiCurrentStamp were to store the timestamp value read from HW
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if puiCurrentStamp is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if the driver is not connected
 */
IMG_RESULT CI_DriverGetTimestamp(CI_CONNECTION *pConnection,
    IMG_UINT32 *puiCurrentStamp);

/**
 * @brief Get the available size for the internal DPF buffer (shared between 
 * contexts)
 *
 * @param pConnection
 * @param[out] puiAvailableDPF
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if puiAvailableDPF is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if the driver is not connected
 */
IMG_RESULT CI_DriverGetAvailableInternalDPFBuffer(CI_CONNECTION *pConnection,
    IMG_UINT32 *puiAvailableDPF);
	
#if defined(INFOTM_ISP)
IMG_RESULT CI_DriverGetReg(CI_CONNECTION *pConnection,
    CI_REG_PARAM *pRegParam);

IMG_RESULT CI_DriverSetReg(CI_CONNECTION *pConnection,
    CI_REG_PARAM *pRegParam);

IMG_RESULT CI_PipelineSetYUVConst(CI_PIPELINE *pPipeline, int flag);
#endif //INFOTM_ISP	

/**
 * @brief Get the Real Time Monitoring information from HW
 *
 * RTM are core registers that can be used to debug the HW.
 * Also gathers information about the status of the contexts.
 *
 * @warning Should not be used to monitor if a context or another could start
 * because it is only monitoring the HW state (a HW context can be acquired
 * by a SW object and still be IDLE until its associated gasket receives data)
 *
 * @param pConnection
 * @param[out] pRTM
 *
 * @note The number of read entries in pRTM is available in
 * @ref CI_HWINFO::config_ui8NRTMRegisters
 */
IMG_RESULT CI_DriverGetRTMInfo(CI_CONNECTION *pConnection, CI_RTM_INFO *pRTM);

/**
 * @brief Read a register or memory at a given offset
 *
 * @note Available only if kernel-side compiled with debug functions
 *
 * @param[in] pConnection
 * @param[in] eBank composed from @ref CI_DEBUG_BANKS (see ci_ioctlr.h)
 * @param[in] ui32Offset in bytes inside the selected bank
 * @warning if using an offset in memory it is PHYSICAL offset, not a virtual
 * address
 * @param[out] pResult where to store the result - should not be NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETER if pConnection or pResult is NULL or
 * eBank is an invalid value.
 * @return IMG_ERROR_NOT_SUPPORTED if kernel module does not support debug
 * access
 * @return IMG_ERROR_FATAL if kernel module returned an error.
 */
IMG_RESULT CI_DriverDebugRegRead(CI_CONNECTION *pConnection,
    IMG_UINT32 eBank, IMG_UINT32 ui32Offset, IMG_UINT32 *pResult);

/**
 * @brief Read a register or memory at a given offset
 *
 * @note Available only if kernel-side compiled with debug functions
 *
 * @param[in] pConnection
 * @param[in] eBank composed from @ref CI_DEBUG_BANKS (see ci_ioctlr.h)
 * @param[in] ui32Offset in bytes inside the selected bank
 * @warning if using an offset in memory it is PHYSICAL offset, not a virtual
 * address
 * @param[in] ui32Value to write
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETER if pConnection or pResult is NULL or
 * eBank is an invalid value.
 * @return IMG_ERROR_NOT_SUPPORTED if kernel module does not support debug
 * access
 */
IMG_RESULT CI_DriverDebugRegWrite(CI_CONNECTION *pConnection,
    IMG_UINT32 eBank, IMG_UINT32 ui32Offset, IMG_UINT32 ui32Value);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Driver functions group
 *---------------------------------------------------------------------------*/
/**
 * @defgroup CI_API_PIPELINE Pipeline functions
 * @ingroup CI_API
 * @brief Pipeline configuration and capture controls
 */
/**
 * @name Pipeline configuration functions (CI_Pipeline)
 * @ingroup CI_API_PIPELINE
 * @brief Functions to manipulate the pipeline configuration
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_Pipeline configuration functions group
 *---------------------------------------------------------------------------*/

/**
 * @brief Allocates and initialise a configuration structure.
 *
 * @param[out] ppPipeline allocate and return the configuration
 * @param pConnection
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if ppPipeline is NULL
 * @return IMG_ERROR_MEMORY_IN_USE if *ppPipeline is not NULL
 */
IMG_RESULT CI_PipelineCreate(CI_PIPELINE **ppPipeline,
    CI_CONNECTION *pConnection);

/**
 * @brief Destroys a configuration
 *
 * Delete the internal configuration after it is de-registered from the driver.
 *
 * @param[in] pPipeline
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 */
IMG_RESULT CI_PipelineDestroy(CI_PIPELINE *pPipeline);

/**
 * @brief Register the configuration to the kernel side
 *
 * Needed before populating the pool.
 * @warning Once registered the configuration should not be changed.
 *
 */
IMG_RESULT CI_PipelineRegister(CI_PIPELINE *pPipeline);

/**
 * @brief Allocate the LSH deshading grid
 *
 * The allocation size is determined by the matrix stride, the number of 
 * channels et the height fo the matrix
 *
 * difference size = (pLSH->ui16Width-1)pLSH->ui16Height per channel
 *
 * start size = pLSH->ui16Height per channel
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if allocation size is 0
 * @return IMG_ERROR_ALREADY_INITIALISED if the matrix is already allocated
 * @return IMG_ERROR_MALLOC_FAILED if allocation failed
 */
IMG_RESULT CI_PipelineAllocLSH(CI_MODULE_LSH *pLSH);

/**
 * @brief Free the matrix if allocated
 *
 * @note This is called when destroying the CI_PIPELINE that ownes the 
 * CI_MODULE_LSH object
 */
IMG_RESULT CI_PipelineFreeLSH(CI_MODULE_LSH *pLSH);

/**
 * @brief Set the config parameters for the next frame added to the pending 
 * list
 *
 * If the capture is not started and no buffers were allocated yet it updates 
 * all modules, regardless of the given eUpdateMask.
 *
 * @note if the capture is not started the device memory and registers are not 
 * really updated - they will be written when the capture starts
 *
 * @param[in,out] pPipeline use the CI_MODULE_* structure to update the HW 
 * configuration
 * @param[in] eUpdateMask from enum CI_MODULE_UPDATE combinaisons (ignored if 
 * Pipeline hasn't started and does not have allocated buffers)
 * @param[out] peFailure if not NULL filled with the module that caused the 
 * failure
 * @li CI_UPD_NONE if some error occured that is not related to a particular 
 * module (e.g. the private number)
 * @li CI_UPD_ALL if the error occured when updating the HW configuration
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL or eUpdateMask is 
 * CI_UPD_NONE
 * @return IMG_ERROR_NOT_INITIALISED if the pipeline configuration is not 
 * registered
 * @return IMG_ERROR_NOT_SUPPORTED or IMG_ERROR_FATAL if the update failed from
 * kernel side
 */
IMG_RESULT CI_PipelineUpdate(CI_PIPELINE *pPipeline, int eUpdateMask,
    int *peFailure);

/**
 * @copydoc CI_PipelineUpdate
 * @brief The configuration is applied As Soon As Possible on the pending 
 * frames (triggered but not yet captured)
 *
 * @note An new configuration cannot be applied ASAP if it requires direct 
 * register changes
 */
IMG_RESULT CI_PipelineUpdateASAP(CI_PIPELINE *pPipeline, int eUpdateMask,
    int *peFailure);

/**
 * @brief Import ION buffer allocated in user space.
 *
 * @param[in,out] pPipeline
 * @param[in] eBuffer buffer type imported
 * @param[in] ui32Size in Bytes - if 0 the kernel side will compute the
 * optimal size of the currently configured pipeline
 * @param[in] bTiled is the buffer to import tiled?
 * @param[in] ionFd identifier for import mechanism (when using ION ionfd)
 * @param[out] pBufferId buffers identifier
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_MALLOC_FAILED if the user space could not allocate the 
 * needed memory to register the new Shots
 * 
 * @note Does not allocate a shot which need to be added using 
 * CI_PipelineAddPool()
 */
IMG_RESULT CI_PipelineImportBuffer(CI_PIPELINE *pPipeline,
    enum CI_BUFFTYPE eBuffer, IMG_UINT32 ui32Size, IMG_BOOL8 bTiled,
    IMG_UINT32 ionFd, IMG_UINT32 *pBufferId);

/**
 * @brief Import ION buffer allocated in user space and allocate the shot 
 * internally.
 *
 * @param[in,out] pPipeline
 * @param[in] eBuffer buffer to allocate, sizes come from given CI_PIPELINE
 * @param[in] ui32Size in Bytes - if 0 the kernel side will compute the
 * optimal size of the currently configured pipeline
 * @param[in] bTiled should the buffer to allocate be tiled?
 * @param[out] pBufferId buffers identifier (can be NULL)
 *
 * @note pBufferId can be NULL if it is not interesting to trigger the captures
 * with specific buffers
 * or that all of the buffers and shots will be liberated with the Pipeline 
 * object destruction
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL or eBuffer is 
 * TYPE_NONE
 * @return IMG_ERROR_MALLOC_FAILED if the user space could not allocate the 
 * needed memory to register the new Shots
 * @return IMG_ERROR_NOT_SUPPORTED when given eBuffer cannot extract sizes from
 * current Pipeline configuration
 */
IMG_RESULT CI_PipelineAllocateBuffer(CI_PIPELINE *pPipeline,
    enum CI_BUFFTYPE eBuffer, IMG_UINT32 ui32Size, IMG_BOOL8 bTiled,
    IMG_UINT32 *pBufferId);

/**
 * @brief Deregister a buffer from kernel space
 *
 * If the buffer was allocated the memory is freed.
 *
 * If the buffer was imported the memory is released.
 *
 * @param[in,out] pPipeline
 * @param[in] bufferId of the buffer to deregister
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if the given buffer is not found or 
 * cannot be deregistered at the moment (triggered or acquired)
 * @return IMG_ERROR_MALLOC_FAILED if the user space could not allocate the 
 * needed memory to register the new Shots
 */
IMG_RESULT CI_PipelineDeregisterBuffer(CI_PIPELINE *pPipeline,
    IMG_UINT32 bufferId);

/**
 * @brief Add N Shots to the Pipeline. Buffers needs to be allocated or 
 * imported separately
 *
 * This determine how many Shots can be triggered and acquired before the Pool 
 * is empty.
 * Note that the HW pool is limited and that the SW may need a few extra Shot 
 * to allow buffer-users to process them.
 *
 * Check CI_HWINFO::context_aPendingQueue[] to know the maximum pending queue 
 * for the HW.
 *
 * @note the Pipeline becomes responsible for the Shots, it will destroy them 
 * at the end of its lifetime.
 * Shots can be destroyed manullay with @ref CI_PipelineDeleteShots()
 *
 * @param[in,out] pPipeline
 * @param[in] ui32NBuffers number of Shot objects to create
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline or ui32NBuffers is NULL
 * @return IMG_ERROR_MALLOC_FAILED if the user space could not allocate the 
 * needed memory to register the new Shots
 * @return IMG_ERROR_NOT_SUPPORTED when the configured sizes are wrong
 *
 * @ return the number of buffers that were successfuly added on error
 */

IMG_RESULT CI_PipelineAddPool(CI_PIPELINE *pPipeline, IMG_UINT32 ui32NBuffers);

/**
 * @brief Delete all Shots associated with a pipeline object - buffers are not
 * affected.
 *
 * @warning The capture has to be stopped for shots to be destroyed and the user
 * should have released all acquired shots as well.
 *
 * @param[in, out] pPipeline
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_NOT_INITIALISED if pPipeline was not registered
 * @return IMG_ERROR_CANCELLED if shots are still acquired by user
 * @return IMG_ERROR_UNEXPECTED_STATE if capture is running
 * @return IMG_ERROR_FATAL if cleaning of the shots failed
 */
IMG_RESULT CI_PipelineDeleteShots(CI_PIPELINE *pPipeline);

/**
 * @brief Verifies that the current configuration structures are sensible
 */
IMG_RESULT CI_PipelineVerify(CI_PIPELINE *pPipeline);

/**
 * @brief Can be use to try to change current CI_CONNECTION linestore 
 * information before starting the capture.
 *
 * Updates the linestore from kernel side and then tries to fit the current 
 * Pipeline configuration into the first available gap.
 * Then tries to update the linestore.
 *
 * @note The linestore needs the IIF to be configured to compute the size!
 *
 * @warning This simple method does not take into account the fact that a 
 * context may have maximum linestore access which is smaller than the maximum 
 * linestore size.
 * But this should this should be verified when checking the linestore before 
 * submission.
 *
 * @warning This could also be improved to try to fit in the smallest gap which
 * is big enough (but currently HW has few contexts so it is not necessary)
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if not connected to the kernel-side 
 * driver or the IIF is not configured (widht>0 is needed)
 * @return IMG_ERROR_NOT_SUPPORTED if the Pipeline's capture is already started
 * or the updated linestore information reveals that the context is not 
 * available anymore
 *
 * @see Delegates to CI_DriverGetLinestore() and CI_DriverSetLinestore()
 */
IMG_RESULT CI_PipelineComputeLinestore(CI_PIPELINE *pPipeline);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the CI_Pipeline configuration functions group
 *---------------------------------------------------------------------------*/
 /**
 * @name Pipeline capture functions (CI_Pipeline)
 * @ingroup CI_API_PIPELINE
 * @brief Functions to control the capture of frames using the pipeline 
 * configuration
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_Pipeline capture functions group
 *---------------------------------------------------------------------------*/

/**
 * @brief Requires the Hardware components needed for a capture to be 
 * performed - needs to be stopped using CI_PipelineStopCapture()
 *
 * Start the capture by requiring the needed HW.
 * The available (and acquired) shots are mapped to the HW MMU.
 *
 * The pipeline configuration of the LSH matrix cannot be changed after that 
 * point.
 *
 * @param[in] pPipeline
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_NOT_INITIALISED if the Capture is not registered or the 
 * number of allocated/imported buffer is 0
 * @return IMG_ERROR_OPERATION_PROHIBITED if the capture failed to start
 * @return IMG_ERROR_MINIMUM_LIMIT_NOT_MET if the linestore is too small to 
 * allow the capture to start
 *
 * @see Delegates to CI_PipelineVerify() to verify integrity
 */
IMG_RESULT CI_PipelineStartCapture(CI_PIPELINE *pPipeline);

/**
 * @brief To know if a capture is started or not - does not communicate with 
 * kernel side
 *
 * @return IMG_FALSE if pPipeline is NULL or it is not connected to driver side
 */
IMG_BOOL8 CI_PipelineIsStarted(CI_PIPELINE *pPipeline);

/**
 * @brief Release the Hardware acquiered with CI_PipelineStartCapture()
 *
 * Stopping the capture and flushes the processed or pending buffers back to 
 * the available list (no more frames can be acquired with 
 * CI_PipelineAcquireShot()).
 * The memory is unmapped from the HW MMU.
 * Shots acquired before stopping are left untouched (they are only unmapped 
 * from MMU).
 *
 * @param[in] pPipeline
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_OPERATION_PROHIBITED if the capture failed to stop
 */
IMG_RESULT CI_PipelineStopCapture(CI_PIPELINE *pPipeline);

/**
 * @brief Adds a frame to the capture list - Pipeline must be registered and 
 * started first
 *
 * @note This call may sleep if the HW waiting list is full.
 *
 * @param[in] pPipeline
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if the Capture is not started (or was 
 * stopped because of a suspend call)
 * @return IMG_ERROR_NOT_SUPPORTED if no available buffer was found
 *
 * @see CI_PipelineTriggerShootNB() for non-blocking equivalent
 */
IMG_RESULT CI_PipelineTriggerShoot(CI_PIPELINE *pPipeline);

/**
 * @brief Adds a frame to the capture list - Pipeline must be registered and 
 * started first
 *
 * @note This call will not sleep but return an error code if the capture was 
 * not triggered
 *
 * @param[in] pPipeline
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if the Capture is not started (or was 
 * stopped because of a suspend call)
 * @return IMG_ERROR_NOT_SUPPORTED if no available buffer was found
 * @return IMG_ERROR_INTERRUPTED if the capture was not submitted
 *
 * @see CI_PipelineTriggerShoot() for blocking equivalent
 */
IMG_RESULT CI_PipelineTriggerShootNB(CI_PIPELINE *pPipeline);

/**
 * @brief Adds a frame to the capture list using specified buffers -Pipeline 
 * must be registered and started first
 *
 * @note This call may sleep if the HW waiting list is full.
 *
 * First available buffers can be found using
 * @ref CI_PipelineFindFirstAvailable()
 *
 * @param[in] pPipeline
 * @param[in] pBuffId structure containing buffer IDs to capture (e.g. ion
 * fd when using ion allocator). The ID value has to be >0 to be searched for.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if the Capture is not started (or was 
 * stopped because of a suspend call)
 * @return IMG_ERROR_NOT_SUPPORTED if no available buffer was found
 */
IMG_RESULT CI_PipelineTriggerSpecifiedShoot(CI_PIPELINE *pPipeline,
    const CI_BUFFID *pBuffId);

/**
 * @brief Adds a frame to the capture list using specified buffers -Pipeline 
 * must be registered and started first
 *
 * @note This call will not sleep but return an error code if the capture was 
 * not triggered
 *
 * First available buffers can be found using 
 * @ref CI_PipelineFindFirstAvailable()
 *
 * @param[in] pPipeline
 * @param[in] pBuffId structure containing internal buffer IDs (0 means none) 
 * - e.g. ion fd when using ion allocator
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if the Capture is not started (or was 
 * stopped because of a suspend call)
 * @return IMG_ERROR_NOT_SUPPORTED if no available buffer was found
 * @return IMG_ERROR_INTERRUPTED if the capture was not submitted
 */
IMG_RESULT CI_PipelineTriggerSpecifiedShootNB(CI_PIPELINE *pPipeline,
    CI_BUFFID *pBuffId);

/**
 * @brief Blocking acquisition of an available buffer from the capture - must 
 * be released using CI_PipelineReleaseBuffer()
 *
 * @warning Acquired buffer MUST NOT be destroyed!
 *
 * @param[in] pPipeline
 * @param[out] ppBuffer populates a pointer to buffer's pointer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline or pBuffer is NULL
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if no buffers are available
 * @return IMG_ERROR_INTERRUPTED if no frame was received (HW or sensor may be 
 * lock-up)
 *
 * @see Non-blocking equivalent: CI_PipelineAcquireBufferNB()
 */
IMG_RESULT CI_PipelineAcquireShot(CI_PIPELINE *pPipeline, CI_SHOT **ppBuffer);

/**
 * @brief Non-blocking equivalent of CI_PipelineAcquireBuffer() - must be 
 * released using CI_PipelineReleaseBuffer()
 *
 * @warning Acquired buffer MUST NOT be destroyed!
 *
 * @param[in] pPipeline
 * @param[out] ppBuffer populates a pointer to buffer's pointer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline or pBuffer is NULL
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if no buffers are available
 *
 * @see Blocking equivalent: KRN_CI_PipelineAcquireShot()
 */
IMG_RESULT CI_PipelineAcquireShotNB(CI_PIPELINE *pPipeline,
    CI_SHOT **ppBuffer);

/**
 * @brief Release a previously acquired with CI_PipelineAcquireBuffer() or 
 * CI_PipelineAcquireBufferNB()
 *
 * @param[in] pPipeline
 * @param[in] pBuffer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline or pBuffer is NULL
 */
IMG_RESULT CI_PipelineReleaseShot(CI_PIPELINE *pPipeline, CI_SHOT *pBuffer);

/**
 * @brief Get a HDR Buffer that has not been acquired yet
 *
 * HDR buffers can be released in two ways:
 * @li calling @ref CI_PipelineReleaseHDRBuffer()
 * @li calling @ref CI_PipelineTriggerSpecifiedShoot() or
 * @ref CI_PipelineTriggerSpecifiedShootNB() with their associated id
 *
 * If released when triggering the HDR buffers cannot be acquired until their
 * associated Shot is released as well.
 *
 * The reservation is only done in user-side.
 *
 * @param[in] pPipeline
 * @param[out] pFrame content output
 * @param[in] id if 0 search first available one, otherwise try to acquire
 * the specified ID. Has to be a HDR Insertion buffer and should be available
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline or pFrame is NULL
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if could not find an available
 * buffer or the specified one is not available
 * 
 */
IMG_RESULT CI_PipelineAcquireHDRBuffer(CI_PIPELINE *pPipeline,
    CI_BUFFER *pFrame, IMG_UINT32 id);
/**
 * @brief Cancel a previous call to @ref CI_PipelineAcquireHDRBuffer()
 *
 * Can only release HDR insertion buffer that have not yet been triggered with
 * a shot.
 *
 * The other way to release an acquired HDR buffer is to trigger a shot 
 * with it.
 * See @ref CI_PipelineAcquireHDRBuffer() for more details.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pPipeline is NULL or id is 0
 * 
 */
IMG_RESULT CI_PipelineReleaseHDRBuffer(CI_PIPELINE *pPipeline, IMG_UINT32 id);

/**
 * @brief Get the IDs of the 1st available buffers for all configured outputs
 *
 * @note will not return any ID for HDRIns, this has to be done using 
 * @ref CI_PipelineAcquireHDRBuffer()
 *
 * @param[in] pPipeline
 * @param[out] pIds
 */
IMG_RESULT CI_PipelineFindFirstAvailable(CI_PIPELINE *pPipeline,
    CI_BUFFID *pIds);

/**
 * @brief The Pipeline still has Shots waiting for their capture
 *
 * @param[in] pPipeline
 *
 * @return IMG_TRUE if the capture has pending Shots, IMG_FALSE otherwise
 * @return IMG_FALSE if pPipeline is NULL
 */
IMG_BOOL8 CI_PipelineHasPending(const CI_PIPELINE *pPipeline);

/**
 * @brief The Pipeline has some Shots that were captured but not acquired
 *
 * @return positive value if no error occurred (number of waiting Shots)
 * @return negative value if an error occurred
 */
IMG_INT32 CI_PipelineHasWaiting(const CI_PIPELINE *pPipeline);

/**
 * @brief The Pipeline has some Shots that can be used to trigger a new capture
 */
IMG_BOOL8 CI_PipelineHasAvailableShots(const CI_PIPELINE *pPipeline);

/**
 * @brief The Pipeline has some Buffers that can be used to trigger a new 
 * capture
 */
IMG_BOOL8 CI_PipelineHasAvailableBuffers(const CI_PIPELINE *pPipeline);

/**
 * @brief Similar to CI_PipelineHasAvailableShots() and 
 * CI_PipelineHasAvailableBuffers() but return TRUE only if both have available
 * elements
 */
IMG_BOOL8 CI_PipelineHasAvailable(const CI_PIPELINE *pPipeline);

/**
 * @brief The Pipeline has some allocated or imported buffers
 *
 * @note does not call an ioctl(), verifies the user-mode driver list only
 *
 * @return positive value if no error occured (number of buffers)
 * @return negative value if an error occured
 */
IMG_INT32 CI_PipelineHasBuffers(const CI_PIPELINE *pPipeline);

/**
 * @brief To know if there are still acquired Shots
 *
 * No communication with kernel module is required.
 *
 * @return IMG_TRUE if it has any acquired Shots
 * @return IMG_FALSE otherwise or in case of errors
 */
IMG_BOOL8 CI_PipelineHasAcquired(const CI_PIPELINE *pPipeline);

/**
 * @brief Searches the user-side buffer list to find the related ionFd in the 
 * buffers
 *
 * @return IMG_SUCCESS if the ionFd was found and pBufferId has the associated 
 * bufferID
 * @return IMG_ERROR_INVALID_PARAMETERS if the given parameters are NULL or 
 * ionFd is invalid (0)
 * @return IMG_ERROR_NOT_INITIALISED if the pipeline was not registered
 * @return IMG_ERROR_FATAL otherwise
 * @see CI_PipelineImportBuffer()
 */
IMG_RESULT CI_PipelineFindBufferFromImportFd(const CI_PIPELINE *pPipeline,
    IMG_UINT32 ionFd, IMG_UINT32 *pBufferId);

/**
 * @brief Find information about a buffer
 *
 * @note this information is from the user-space CI library and could have
 * been stored when allocating the buffer.
 *
 * @param[in] pPipeline - should not be NULL
 * @param[in] buffId - should be different than 0
 * @param[out] pInfo - should not be NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if parameters are NULL or 0
 * @return IMG_ERROR_NOT_INITIALISED if pPipeline was not registered
 * @return IMG_ERROR_FATAL if the given buffer was not found
 */
IMG_RESULT CI_PipelineGetBufferInfo(const CI_PIPELINE *pPipeline,
    IMG_UINT32 buffId, CI_BUFFER_INFO *pInfo);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the CI_Pipeline capture functions group
 *---------------------------------------------------------------------------*/

/**
 * @defgroup CI_API_GASKET Gasket functions (CI_Gasket)
 * @ingroup CI_API
 * @brief Manage the Gasket
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Gasket functions group
 *---------------------------------------------------------------------------*/

IMG_RESULT CI_GasketInit(CI_GASKET *pGasket);

/**
 * @brief Acquire the gasket - release with CI_GasketRelease()
 *
 * If acquired the gasket information is written to registers and started 
 * otherwise nothing happens.
 *
 * @warning This delegates all the checking to the kernel-side
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pGasket is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if not connected to the kernel-side
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if the Gasket is already 
 * acquired
 */
IMG_RESULT CI_GasketAcquire(CI_GASKET *pGasket, CI_CONNECTION *pConnection);

/**
 * @brief Release an acquired gasket acquired with CI_GasketAcquire()
 *
 * If the gasket was acquired with this CI_CONNECTION it is stopped otherwise 
 * nothing happens
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pGasket is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if not connected to the kernel-side
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if the Gasket was not acquired 
 * by this kernel-connection
 *
 */
IMG_RESULT CI_GasketRelease(CI_GASKET *pGasket, CI_CONNECTION *pConnection);

/**
 * @brief Access the specified gasket informations
 */
IMG_RESULT CI_GasketGetInfo(CI_GASKET_INFO *pGasketInfo, IMG_UINT8 uiGasket,
    CI_CONNECTION *pConnection);

/**
 * @}
 */
 /*----------------------------------------------------------------------------
 * End of the Gasket functions group
 *---------------------------------------------------------------------------*/
/**
 * @defgroup CI_API_IFFDG Internal Data Generator
 * @ingroup CI_API
 * @brief Control of the Internal Data Generator and its converter
 */
/**
 * @name Internal Data Generator controls (CI_Datagen)
 * @ingroup CI_API_IIFDG
 * @brief Manages the IIF Data Generator if available
 *
 * To know if the IIF Datagen is available check CI_HWINFO::eFunctionalities 
 * for CI_INFO_SUPPORTED_IIF_DATAGEN
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IIF DG functions group
 *---------------------------------------------------------------------------*/ 

/**
 * @brief Creates a data generator object attached to the given connection
 *
 * Registers the created object to the kernel-side using the given connection
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if ppDatagen or pConnection is NULL
 * @return IMG_ERROR_MEMORY_IN_USE if *ppDatagen is not NULL
 * @return IMG_ERROR_NOT_SUPPORTED if IIF Datagen is not supported by HW
 * @return IMG_ERROR_FATAL if registration to the kernel-side failed
 * @return IMG_ERROR_MALLOC_FAILED if allocation of internal structures failed
 */
IMG_RESULT CI_DatagenCreate(CI_DATAGEN **ppDatagen,
    CI_CONNECTION *pConnection);

/**
 * @brief Destroys a Datagen object
 *
 * Also deregisters a datagen from the kernel-side
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pDatagen is NULL
 * @return IMG_ERROR_FATAL if datagen object was not found in the kernel-side
 * @see Delegates to CI_DatagenStop() if datagen is still running
 */
IMG_RESULT CI_DatagenDestroy(CI_DATAGEN *pDatagen);

/**
 * @brief Acquires needed HW for the capture
 * 
 * Maps elements to device MMU in kernel (all frames must be allocated before 
 * that)
 *
 * @return IMG_SUCCESS (if already started returns directly)
 * @return IMG_ERROR_INVALID_PARAMETERS if pDatagen is NULL
 * @return IMG_ERROR_FATAL if datagen object was not found in the kernel-side
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if chosen datagen resource is 
 * not available
 */
IMG_RESULT CI_DatagenStart(CI_DATAGEN *pDatagen);

/**
 * @brief To know if the capture is already started
 *
 * May be needed when suspend/resume are called by system the user-space may 
 * not realise that the capture was stopped.
 *
 * Can also be used
 */
IMG_BOOL8 CI_DatagenIsStarted(CI_DATAGEN *pDatagen);

/**
 * @brief Releases acquired HW for the capture
 *
 * unmaps elements from device MMU in kernel (frames can be destroyed after 
 * that)
 *
 * @return IMG_SUCCESS (if already stopped returns directly)
 * @return IMG_ERROR_INVALID_PARAMETERS if pDatagen is NULL
 * @return IMG_ERROR_FATAL if datagen object was not found in the kernel-side
 */
IMG_RESULT CI_DatagenStop(CI_DATAGEN *pDatagen);

/**
 * @brief allocate a frame of a given size 
 *
 * In the future the DG may also have an import function
 *
 * @note works only if DG is not started
 *
 * @param pDatagen
 * @param ui32Size in Bytes
 * @param[out] *pFrameID optional pointer to get the given ID to an allocated
 * frame
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pDatagen is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if datagen is already started
 * @return IMG_ERROR_MALLOC_FAILED if internal structure allocation failed
 * @return IMG_ERROR_FATAL if datagen object was not found in the kernel-side
 * or memory mapping to user-space failed
 */
IMG_RESULT CI_DatagenAllocateFrame(CI_DATAGEN *pDatagen, IMG_UINT32 ui32Size,
    IMG_UINT32 *pFrameID);

CI_DG_FRAME* CI_DatagenGetAvailableFrame(CI_DATAGEN *pDatagen);

// get the asked frame only if it is available - does NOT call
// CI_DatagenUpdateFrameStatus()
CI_DG_FRAME* CI_DatagenGetFrame(CI_DATAGEN *pDatagen, IMG_UINT32 ui32FrameID);

// if the frame is available will
IMG_RESULT CI_DatagenInsertFrame(CI_DG_FRAME *pFrame);

// if not inserted can become available again
IMG_RESULT CI_DatagenReleaseFrame(CI_DG_FRAME *pFrame);

// liberate the memory - only if DG is not acquired - frame has to be in
// pending list
IMG_RESULT CI_DatagenDestroyFrame(CI_DG_FRAME *pFrame);

/**
 * @}
 */
/*----------------------------------------------------------------------------
 * End of the IIF Datagen functions group
 *---------------------------------------------------------------------------*/ 
/**
 * @name Internal Data Generator frame converter
 * @ingroup CI_API_IIFDG
 * @brief Functions to help convert images to correct pixel format
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IIF Converter functions group
 *---------------------------------------------------------------------------*/ 

IMG_RESULT CI_ConverterConfigure(CI_DG_CONVERTER *pConverter,
    CI_CONV_FMT eFormat);

IMG_RESULT CI_ConverterConvertFrame(CI_DG_CONVERTER *pConverter,
    const struct _sSimImageIn_ *pImage, CI_DG_FRAME *pFrame);

// used with 1 line to have the stride
IMG_UINT32 CI_ConverterFrameSize(CI_DG_CONVERTER *pConverter,
    IMG_UINT32 ui32Width, IMG_UINT32 ui32Height);

/**
 * @brief Converts input data to Parallel 10b output - used by converter
 *
 * @note do not use directly - defined in header to be useful in external DG 
 * code
 *
 * @see implementation of CI_DATAGEN_CONVERTER::pfnConverter
 */
IMG_SIZE IMG_Convert_Parallel10(const IMG_UINT16 *inPixels,
    IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

/**
 * @brief Converts input data to Parallel 12b output - used by converter
 *
 * @note do not use directly - defined in header to be useful in external DG 
 * code
 *
 * @see implementation of CI_DATAGEN_CONVERTER::pfnConverter
 */
IMG_SIZE IMG_Convert_Parallel12(const IMG_UINT16 *inPixels,
    IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);

/**
 * @brief Converts a FLX image to the HDR insertion format
 *
 * @note Not part of the internal DG but implemented as such to be with the
 * other loading/conversion from sim_image structures.
 *
 * @param[in] pImage input RGB24 or RGB32 image
 * @param[out] pBuffer to write the converted data
 * @param bRescale rescale input data to HDR Insertion bitdepth
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pImage or pBuffer is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if pImage is not RGB32 or RGB24
 */
IMG_RESULT CI_Convert_HDRInsertion(const struct _sSimImageIn_ *pImage,
    CI_BUFFER *pBuffer, IMG_BOOL8 bRescale);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the IIF Converter functions group
 *---------------------------------------------------------------------------*/ 
/*----------------------------------------------------------------------------
 * End of the CI_API documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_API_H_ */
