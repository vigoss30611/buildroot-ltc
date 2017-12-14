/**
*******************************************************************************
 @file dg_connection.h

 @brief handling interraction with user-space

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
#ifndef DG_CONNECTION_H_
#define DG_CONNECTION_H_

#include "dg_kernel/dg_camera.h"
#include "dg_kernel/dg_ioctl.h"

/**
 * @defgroup DG_KERNEL Data Generator: kernel-side driver
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the DG_KERNEL documentation module
 *---------------------------------------------------------------------------*/

/**
 * @name DEV_DG
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *---------------------------------------------------------------------------*/

#ifdef FELIX_FAKE
#undef __user
/// used to specify that a pointer is from user space and should not be trusted
#define __user

#include "sys/sys_userio_fake.h"  // defines some kernel structures

#else
#include <linux/compiler.h>  // for the __user macro
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#endif /* FELIX_FAKE */

int DEV_DG_Open(struct inode *inode, struct file *flip);
int DEV_DG_Close(struct inode *inode, struct file *flip);
long DEV_DG_Ioctl(struct file *flip, unsigned int ioctl_cmd,
    unsigned long ioctl_param);

int DEV_DG_Mmap(struct file *flip, struct vm_area_struct *vma);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of DEV_DG group
 *---------------------------------------------------------------------------*/

/**
 * @name INT_DG user-kernel interraction
 * @brief Handle user-kernel interraction
 * @{
 */

/**
 * @brief Get the HW information from the kernel
 *
 * @param pConnection
 * @param[out] pUserParam
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_to_user failed
 */
IMG_RESULT INT_DG_DriverGetInfo(KRN_DG_CONNECTION *pConnection,
    DG_HWINFO __user *pUserParam);

/**
 * @brief Create a new camera element on the kernel side
 *
 * @param pConnection associated with the camera
 * @param[out] pGivenId identifier to return to user-space
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_MALLOC_FAILED if creation of kernel side object failed
 * @return IMG_ERROR_FATAL if failed to add new object to connection's list
 */
IMG_RESULT INT_DG_CameraRegister(KRN_DG_CONNECTION *pConnection,
    int *pGivenId);

/**
 * @brief Allocates a buffer using sizes from user-space
 *
 * @param pConnection associated with the camera
 * @param[in,out] pParam from user-space, contains camera Id and size, will
 * contain mmap Id as output
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user or copy_to_user
 * failed
 * @return IMG_ERROR_FATAL if the camera Id is not found
 *
 * Delegates to @ref KRN_DG_FrameCreate()
 */
IMG_RESULT INT_DG_CameraAddBuffers(KRN_DG_CONNECTION *pConnection,
    struct DG_BUFFER_PARAM __user *pParam);

/**
 * @brief Copy the configuration of the camera and reserve the HW to allow
 * running
 *
 * @param pConnection associated with the camera
 * @param[in] pUserCam from user-space, contains the camera Id and
 * configuration
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if the camera Id is not found
 *
 * Delegates to @ref KRN_DG_CameraStart()
 */
IMG_RESULT INT_DG_CameraStart(KRN_DG_CONNECTION *pConnection,
    const struct DG_CAMERA_PARAM __user *pUserCam);

/**
 * @brief Stop the camera and release the associated HW
 *
 * @param pConnection associated with the camera
 * @param cameraId from user-space
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INTERRUPTED if the camera still has pending frames
 * @return IMG_ERROR_FATAL if the camera Id is not found
 *
 * Delegates to @ref KRN_DG_CameraStop()
 */
IMG_RESULT INT_DG_CameraStop(KRN_DG_CONNECTION *pConnection, int cameraId);

/**
 * @brief Get an available buffer to allow user-space to write memory
 *
 * @param pConnection associated with the camera
 * @param[in,out] pParam from user-space containing frame Id as output
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user or copy_to_user
 * failed
 * @return IMG_ERROR_INTERRUPTED if waiting on the acquisition failed
 * @return IMG_ERROR_FATAL if the camera Id is not found
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if the specified frame was not
 * found
 */
IMG_RESULT INT_DG_CameraAcquireBuffer(KRN_DG_CONNECTION *pConnection,
    struct DG_FRAME_PARAM __user *pParam);

/**
 * @brief Trigger a shot or cancel a request
 *
 * @param pConnection associated with the camera
 * @param[in] pParam from user-space containing frame Id
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if the camera Id is not found
 * @return IMG_ERROR_NOT_SUPPORTED if trying to trigger on a stopped DG
 *
 * Delegates to @ref KRN_DG_CameraShoot()
 */
IMG_RESULT INT_DG_CameraShoot(KRN_DG_CONNECTION *pConnection,
    const struct DG_FRAME_PARAM __user *pParam);

/**
 * @brief Wait for a frame to be processed
 *
 * @param pConnection associated with the camera
 * @param[in] pParam from user-space containing Id
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if the camera Id is not found
 *
 * Delegates to @ref KRN_DG_CameraWaitProcessed()
 */
IMG_RESULT INT_DG_CameraWaitProcessed(KRN_DG_CONNECTION *pConnection,
    const struct DG_FRAME_WAIT __user *pParam);

/**
 * @brief Destroy a frame
 *
 * @param pConnection associated with the camera
 * @param[in] pParam from user-space containing frame Id
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if copy_from_user failed
 * @return IMG_ERROR_FATAL if the camera Id is not found or the frame Id is
 * not found in the list of available buffers
 */
IMG_RESULT INT_DG_CameraDeleteBuffer(KRN_DG_CONNECTION *pConnection,
    const struct DG_BUFFER_PARAM __user *pParam);

/**
 * @brief Destroy a camera on the kernel side
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the camera Id is not found
 *
 * Delegates to @ref KRN_DG_CameraDestroy()
 */
IMG_RESULT INT_DG_CameraDeregister(KRN_DG_CONNECTION *pConnection,
    int iCameraId);

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of INT_DG group
 *------------------------------------------------------------------------*/

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the DG_KERNEL documentation module
 *------------------------------------------------------------------------*/

#endif /* DG_CONNECTION_H_ */
