/**
*******************************************************************************
 @file ci_connection.h

 @brief User-kernel communication interface

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

#include "ci/ci_api_structs.h"
#include "ci_kernel/ci_ioctrl.h"
#include "ci_kernel/ci_kernel_structs.h"
#include "ci_internal/sys_device.h"

#ifndef CI_CONNECTION_H_
#define CI_CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DEV_CI Linux kernel registration (DEV_CI)
 * @ingroup KRN_CI_API
 * @brief Functions used when registering driver functionalities to the Linux
 * kernel
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the INT_CI_DRIVER functions group
 *---------------------------------------------------------------------------*/

#ifdef FELIX_FAKE

// contains the definition of some kernel structs and PAGE_SIZE
#include <sys/sys_userio_fake.h>

#else /* FELIX_FAKE */
#include <linux/compiler.h> /* for the __user macro */
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#endif /* FELIX_FAKE */

int DEV_CI_Open(struct inode *inode, struct file *flip);
int DEV_CI_Close(struct inode *inode, struct file *flip);
/**
 * @return -ENODEV for internal corruption
 * @return -EACCES when intended action failed
 * @return -ENOMEM when some memory could not be acquired
 * @return -ENOTTY when IOCTL command is unknown
 */
long DEV_CI_Ioctl(struct file *flip, unsigned int ioctl_cmd,
    unsigned long ioctl_param);

int DEV_CI_Mmap(struct file *flip, struct vm_area_struct *vma);

/**
 * @brief Stops all running captures and turns the HW off
 */
IMG_RESULT DEV_CI_Suspend(SYS_DEVICE *pDev, pm_message_t state);

/**
 * @brief Use to resume the HW in a usable state after a suspend
 *
 * @note Also used when the first connection to the driver is made to setup the
 * HW
 */
IMG_RESULT DEV_CI_Resume(SYS_DEVICE *pDev);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the DEV_CI functions group
 *---------------------------------------------------------------------------*/

/**
 * @defgroup INT_CI Interaction to/from user-space (INT_CI)
 * @ingroup KRN_CI_API
 * @brief Kernel functions accessible 'from' User Space through IOCTL or 
 * standard system call.
 *
 * 'Input' __user parameters means they are "copied from user".
 *
 * 'Output __user parameters means they are "copied to user".
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the INT_CI functions group
 *---------------------------------------------------------------------------*/

/**
 * @name Driver commands
 * @{
 */

/**
 * @brief Connect the kernel-space connection object with the user-space one
 *
 * This copies some HW related information to the user-space structure
 *
 * @param pKrnConnection kernel-side object - asserts it is not NULL
 * @param[out] pConnection user-side pointer to copy to
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if the copy_to_user failed
 */
IMG_RESULT INT_CI_DriverConnect(KRN_CI_CONNECTION *pKrnConnection,
    CI_CONNECTION __user *pConnection);

/**
 * @brief Get the current kernel-space line-store values
 *
 * @param[out] pCopy
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_to_user failed
 */
IMG_RESULT INT_CI_DriverGetLineStore(CI_LINESTORE __user *pCopy);

/**
 * @brief Propose new line-store values from user-space
 *
 * Each context registers its start position and needed size when starting.
 *
 * A change can be refused if, for a running context, it changes:
 * @li the start position of a running context, or
 * @li the new proposed sized becomes smaller
 *
 * @param[in] pNew user pointer - asserts if NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_DEVICE_UNAVAILABLE if the changes fail
 */
IMG_RESULT INT_CI_DriverSetLineStore(const CI_LINESTORE __user *pNew);

/**
 * @brief Get the current Gamma Look Up Table from the kernel-side structure
 *
 * @param[out] pCopy
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_to_user failed
 */
IMG_RESULT INT_CI_DriverGetGammaLUT(CI_MODULE_GMA_LUT __user *pCopy);

/**
 * @brief Propose a new Gamma Look Up Table from the user-side
 *
 * @param[in] pNew
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 *
 * @see Delegates to KRN_CI_DriverChangeGammaLUT()
 */
IMG_RESULT INT_CI_DriverSetGammaLUT(const CI_MODULE_GMA_LUT __user *pNew);

/**
 * @brief Reads the current timestamp and stores it into user-space variable
 *
 * @param[out] puiTimestamp
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_to_user failed
 */
IMG_RESULT INT_CI_DriverGetTimestamp(IMG_UINT32 __user *puiTimestamp);

/**
 * @brief Get the current available DPF internal buffer to a user-space 
 * variable
 *
 * @param[out] puiDPFInt
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_to_user failed
 */
IMG_RESULT INT_CI_DriverGetDPFInternal(IMG_UINT32 __user *puiDPFInt);

/**
 * @brief Get the RTM information
 *
 * @param[out] pRTMInfo
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if @ref CI_RTM_INFO cannot store all RTM
 * information available
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_to_user failed
 */
IMG_RESULT INT_CI_DriverGetRTM(CI_RTM_INFO __user *pRTMInfo);

IMG_RESULT INT_CI_DebugReg(struct CI_DEBUG_REG_PARAM __user *pInfo);

/**
 * @brief Disconnect with user-space and destroy the kernel connection object
 *
 * @param pConnection connection to destroys - asserts it is not NULL
 *
 * @return IMG_SUCCESS
 *
 * @see Delegates to KRN_CI_ConnectionDestroy()
 */
IMG_RESULT INT_CI_DriverDisconnect(KRN_CI_CONNECTION *pConnection);

/**
 * @}
 * @name Configuration commands
 * @{
 */

/**
 * @brief Register a user-space Pipeline structure
 *
 * If the deshading matrix is used it delegates to 
 * INT_CI_PipelineSetDeshading() to load it (but return code is not used).
 *
 * No update of system memory is done for configuration at that time.
 * The writing of the load structure is done when starting the capture.
 *
 * @param pConnection - asserts it is not NULL
 * @param[in] pUserConfig pointer to user space structure
 * @param[out] pGivenId is returned to user throught IOCTL return value (so it
 * is a pointer to kernel value) - asserts it is not NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_to_user failed or loading of 
 * the deshading matrix failed
 *
 * @see Delegates to KRN_CI_PipelineCreate()
 */
IMG_RESULT INT_CI_PipelineRegister(KRN_CI_CONNECTION *pConnection,
    struct CI_PIPE_INFO __user *pUserConfig, int *pGivenId);

/**
 * @brief Deregister a user-space Pipeline structure
 *
 * Stops the capture if it is started.
 *
 * @param pConnection Pipeline associated kernel-side connection object 
 * - asserts it is not NULL
 * @param[in] ui32ConfigId Pipeline configuration ID given at creation time
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline was not found
 *
 * @see Delegates to KRN_CI_PipelineStopCapture()
 */
IMG_RESULT INT_CI_PipelineDeregister(KRN_CI_CONNECTION *pConnection,
    int ui32ConfigId);

/**
 * @brief Update the kernel-side configuration
 *
 * @note if the capture is not started the device memory and registers are not 
 * updated - they will be updated when the capture starts.
 * Unless the desahding matrix is not NULL and it is copied to device memory 
 * (so that user memory can be reused after this system call terminates).
 *
 * In case of failure a copy if the previous configuration is restored.
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in,out] pParam kernel-side structure containing some user pointer 
 * - copied back to user on error to access eUpdateMask
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_NOT_SUPPORTED if a module to update needs access to 
 * registers but a frame is currently being processed - pParam::eUpdateMask is 
 * updated with the module enum
 *
 * @see Delegates to KRN_CI_PipelineUpdate() to update device memory
 */
IMG_RESULT INT_CI_PipelineUpdate(KRN_CI_CONNECTION *pConnection,
    struct CI_PIPE_PARAM __user *pParam);

/**
 * @brief Import a buffer allocated by ION to the Pipeline
 *
 * @param pConnection associated kernel-side connection object
 * @param[in,out] pParam kernel side structure containing buffer file 
 * descriptors (when in) and memMapId's (when out)
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 *
 * @note Does not allocate shot structure which should be created by 
 * INT_CI_PipelineAddShot()
 * 
 * @see Delegates to KRN_CI_ShotCreate(), KRN_CI_PipelineAddShot()
 */
IMG_RESULT INT_CI_PipelineImportBuffer(KRN_CI_CONNECTION *pConnection,
    struct CI_ALLOC_PARAM __user *pParam);

/**
 * @brief Deregisters a imported buffer allocated by ION from the Pipeline
 *
 * @param pConnection associated kernel-side connection object
 * @param[in] pUserParam kernel side structure containing buffer memMapId's
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 *
 * @see Delegates to KRN_CI_ShotCreate(), KRN_CI_PipelineAddShot()
 */
IMG_RESULT INT_CI_PipelineDeregBuffer(KRN_CI_CONNECTION *pConnection,
    struct CI_ALLOC_PARAM __user *pUserParam);

/**
 * @brief Import a buffer allocated by ION to the Pipeline
 *
 * @param pConnection associated kernel-side connection object
 * @param[in,out] pParam strait from iotcl
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 *
 * @note Allocates and initializes the shot structure, no need to 
 * INT_CI_PipelineAddShot()
 * 
 * @see Delegates to KRN_CI_ShotCreate(), KRN_CI_PipelineAddShot()
 */
IMG_RESULT INT_CI_PipelineCreateBuffer(KRN_CI_CONNECTION *pConnection,
    struct CI_ALLOC_PARAM __user *pParam);

/**
 * @brief Add a given number of buffers to the Pipeline
 *
 * @param pConnection associated kernel-side connection object - asserts it is 
 * not NULL
 * @param[in,out] userParam strait from ioctl
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user or copy_to_user 
 * failed
 *
 * @see Delegates to KRN_CI_ShotCreate(), KRN_CI_PipelineAddShot(),
 */
IMG_RESULT INT_CI_PipelineAddShot(KRN_CI_CONNECTION *pConnection,
    struct CI_POOL_PARAM __user *userParam);

IMG_RESULT INT_CI_PipelineDeleteShots(KRN_CI_CONNECTION *pConnection,
    int pipelineID);

/**
 * @brief Get access to a LSH matrix buffer if not used by the pipeline
 *
 * @param pConnection
 * @param[in] userParam user-space parameter containing KRN_CI_PIPELINE id
 * and KRN_CI_LSH_MATRIX id
 *
 * @return IMG_SUCCESS if the object is available
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if could not find the pipeline or matrix using
 * their ID
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if the buffer is not available
 */
IMG_RESULT INT_CI_PipelineGetLSHMatrix(KRN_CI_CONNECTION *pConnection,
    struct CI_LSH_GET_PARAM __user *userParam);

/**
 * @brief Release access to a LSH matrix buffer
 *
 * Device memory should be updated as user-side may have modified the
 * content of the matrix
 *
 * @param pConnection
 * @param[in] userParam user-space parameter containing KRN_CI_PIPELINE id,
 * KRN_CI_LSH_MATRIX id and the configuration for the matrix
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if could not find the pipeline or matrix using
 * their id (or the matrix is now in use which should not be possible as
 * user-space checks that it's not acquire before setting a matrix)
 */
IMG_RESULT INT_CI_PipelineSetLSHMatrix(KRN_CI_CONNECTION *pConnection,
    struct CI_LSH_SET_PARAM __user *userParam);

/**
 * @brief Change the LSH matrix used by the Pipeline
 *
 * @param pConnection
 * @param[in] userParam user-space parameter containing KRN_CI_PIPELINE id
 * and KRN_CI_LSH_MATRIX id (id can be 0 to disable usage of LSH matrix)
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if the pipeline or matrix are not found
 * @return IMG_ERROR_ALREADY_INITIALISED if matrix already in use
 * @return IMG_ERROR_NOT_SUPPORTED if checking for the limitation
 * returned an error
 *
 * @note Note, that due to HW2.x limitation one matrix have to be
 * choosen before pipeline start. If not, the matrix cannot be changed later.
 */
IMG_RESULT INT_CI_PipelineChangeLSHMatrix(KRN_CI_CONNECTION *pConnection,
    struct CI_LSH_CHANGE_PARAM __user *userParam);

/**
 * @brief Update the Defective pixel map
 *
 * Copy the given DPF map to device memory
 *
 * @param pPipeline
 * @param[in] pUserDPF
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy from user failed
 * @return IMG_ERROR_MALLOC_FAILED if temporary allocation failed
 *
 * @see delegates to HW_CI_DPF_ReadMapConvert()
 */
IMG_RESULT INT_CI_PipelineSetDPFRead(KRN_CI_PIPELINE *pPipeline,
    IMG_UINT16 __user *pUserDPF);

/**
 * @}
 * @name Capture commands
 * @{
 */

/**
 * @brief Start the capture - finds the pipeline object in the connection using
 * the ID
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in] ui32CaptureId Pipeline ID given at creation time
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 *
 * @see Delegates to KRN_CI_PipelineStartCapture()
 */
IMG_RESULT INT_CI_PipelineStartCapture(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId);

/**
 * @brief Stop the capture - finds the pipeline object in the connection using
 * the ID
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in] ui32CaptureId Pipeline ID given at creation time
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 *
 * @see Delegates to KRN_CI_PipelineStopCapture()
 */
IMG_RESULT INT_CI_PipelineStopCapture(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId);

/**
 * @brief To know if the capture is started - used to update user-side when
 * unexpected status occurs
 *
 * @param pConnection associated kernel-side connection object - asserts it is 
 * not NULL
 * @param[in] ui32CaptureId Pipeline ID given at creation time
 * @param[out] bIsStarted status - asserts it is not NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 */
IMG_RESULT INT_CI_PipelineIsStarted(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId, IMG_BOOL8 *bIsStarted);

/**
 * @brief Trigger a frame capture - finds the pipeline object in the connection
 * using the ID
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in] param parameters from user-side
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 *
 * @see Delegates to KRN_CI_PipelineTriggerShoot()
 */
IMG_RESULT INT_CI_PipelineTriggerShoot(KRN_CI_CONNECTION *pConnection,
    struct CI_BUFFER_TRIGG __user *param);

/**
 * @brief Acquire a captured shot - finds the pipeline object in the connection
 * using the ID
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in,out] pParam driver-side structure that may contain user-side
 * pointers
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user or copy_to_user
 * failed
 *
 * @see Delegates to KRN_CI_PipelineAcquireShot()
 */
IMG_RESULT INT_CI_PipelineAcquireShot(KRN_CI_CONNECTION *pConnection,
    struct CI_BUFFER_PARAM __user *pParam);

/**
 * @brief Release an acquired shot - finds the pipeline object in the 
 * connection using the ID
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in] pParam driver-side structure that may contain user-side pointers
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user
 *
 * @see Delegates to KRN_CI_PipelineRelaseShot()
 */
IMG_RESULT INT_CI_PipelineReleaseShot(KRN_CI_CONNECTION *pConnection,
    const struct CI_BUFFER_TRIGG __user *pParam);

/**
 * @brief To know if there are available buffers for capture
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in,out] pParam 
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 */
IMG_RESULT INT_CI_PipelineHasAvailable(KRN_CI_CONNECTION *pConnection,
    struct CI_HAS_AVAIL __user *pParam);

/**
 * @brief To know if there are any frame currently being captured
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in] ui32CaptureId Pipeline ID given at creation time
 * @param[out] bResult boolean result - asserts it is not NULL - returned as 
 * ioctl result
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 */
IMG_RESULT INT_CI_PipelineHasPending(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId, IMG_BOOL8 *bResult);

/**
 * @brief To know if there are buffers ready for user-space to acquire
 *
 * @param pConnection associated kernel-side connection object - asserts it is
 * not NULL
 * @param[in] ui32CaptureId Pipeline ID given at creation time
 * @param[out] pNbWaiting number result - asserts it is not NULL - returned as
 * ioctl result
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the Pipeline object was not found
 */
IMG_RESULT INT_CI_PipelineHasWaiting(KRN_CI_CONNECTION *pConnection,
    int ui32CaptureId, int *pNbWaiting);

/**
 * @}
 * @name Gasket commands
 * @{
 */

/**
 * @brief Acquire a given gasktet
 *
 * @param pConnection
 * @param[in] pGasket
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_DEVICE_UNAVAILABLE if the gasket does not exist
 * @return IMG_ERROR_NOT_SUPPORTED if the gasket does not support the required
 * format
 *
 * @see delegates to HW_CI_WriteGasket()
 */
IMG_RESULT INT_CI_GasketAcquire(KRN_CI_CONNECTION *pConnection,
    CI_GASKET __user *pGasket);

/**
 * @brief Release an acquired gasket
 *
 * @param pConnection
 * @param[in] uiGasket
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_DEVICE_UNAVAILABLE if the gasket does not exist
 *
 * @see delegates to HW_CI_WriteGasket()
 */
IMG_RESULT INT_CI_GasketRelease(KRN_CI_CONNECTION *pConnection,
    IMG_UINT8 uiGasket);

/**
 * @brief Get information about the gasket
 *
 * @param[in,out] pUserGasket (in: gasket to use, out: gasket info)
 */
IMG_RESULT INT_CI_GasketGetInfo(struct CI_GASKET_PARAM __user *pUserGasket);

/*
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of gasket commands
 *---------------------------------------------------------------------------*/

/**
 * @}
 * @name Internal DG commands
 * @{
 */

/**
 * if iIIFDGId < 0 then registration, otherwise deregistration
 */
IMG_RESULT INT_CI_DatagenRegister(KRN_CI_CONNECTION *pConnection, int iIIFDGId,
    int *regId);

/**
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if could not find datagen object from ID
 * @see delegates from KRN_CI_DatagenStart()
 */
IMG_RESULT INT_CI_DatagenStart(KRN_CI_CONNECTION *pConnection,
    const struct CI_DG_PARAM __user *pUserParam);

IMG_RESULT INT_CI_DatagenIsStarted(KRN_CI_CONNECTION *pConnection,
    int iIIFDGId, int *pbResult);

/**
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if could not find datagen object from ID
 * @see delegates to KRN_CI_DatagenStop()
 */
IMG_RESULT INT_CI_DatagenStop(KRN_CI_CONNECTION *pConnection, int iIIFDGId);

/**
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user/copy_to_user failed
 * @return IMG_ERROR_FATAL if could not find datagen object from ID
 * @see delegates to KRN_CI_DatagenAllocateFrame()
 */
IMG_RESULT INT_CI_DatagenAllocate(KRN_CI_CONNECTION *pConnection,
    struct CI_DG_FRAMEINFO __user *pUserParam);

IMG_RESULT INT_CI_DatagenAcquireFrame(KRN_CI_CONNECTION *pConnection,
    struct CI_DG_FRAMEINFO __user *pUserParam);

IMG_RESULT INT_CI_DatagenTrigger(KRN_CI_CONNECTION *pConnection,
    const struct CI_DG_FRAMETRIG __user *pUserParam);

IMG_RESULT INT_CI_DatagenWaitProcessed(KRN_CI_CONNECTION *pConnection,
    const struct CI_DG_FRAMEWAIT __user *pUserParam);

IMG_RESULT INT_CI_DatagenFreeFrame(KRN_CI_CONNECTION *pConnection,
    const struct CI_DG_FRAMEINFO __user *pUserParam);

#ifdef INFOTM_HW_AWB_METHOD
IMG_RESULT INT_CI_DriverGetHwAwbBoundary(KRN_CI_CONNECTION *pConnection,
    HW_AWB_BOUNDARY __user *pHwAwbBoundary);

IMG_RESULT INT_CI_DriverSetHwAwbBoundary(KRN_CI_CONNECTION *pConnection,
    HW_AWB_BOUNDARY __user *pHwAwbBoundary);
#endif // INFOTM_HW_AWB_METHOD

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of internal DG commands
 *---------------------------------------------------------------------------*/
/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the INT_CI functions group
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_CONNECTION_H_ */
