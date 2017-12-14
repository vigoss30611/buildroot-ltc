/**
******************************************************************************
 @file dg_connection.h

 @brief Internal declarations

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
#ifndef DG_CONNECTION
#define DG_CONNECTION

#include "dg_kernel/dg_camera.h"
#include "dg_kernel/dg_ioctl.h"
struct DG_CAMERA_PARAM; // defines by dg_ioctl.h but needed to avoid warnings when compiling in the kernel

/**
 * @defgroup DG_KERNEL Data Generator: kernel-side driver
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the DG_KERNEL documentation module
 *------------------------------------------------------------------------*/

/**
 * @name DEV_DG
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *------------------------------------------------------------------------*/

#ifdef FELIX_FAKE
#undef __user
/// used to specify that a pointer is from user space and should not be trusted
#define __user

#include "sys/sys_userio_fake.h" // defines some kernel structures

#else
#include <linux/compiler.h> // for the __user macro
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/mm.h>
#endif // FELIX_FAKE

int DEV_DG_Open(struct inode *inode, struct file *flip);
int DEV_DG_Close(struct inode *inode, struct file *flip);
long DEV_DG_Ioctl(struct file *flip, unsigned int ioctl_cmd, unsigned long ioctl_param);

int DEV_DG_Mmap(struct file *flip, struct vm_area_struct *vma);

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of DEV_DG group
 *------------------------------------------------------------------------*/

/**
 * @name INT_DG
 * @{
 */

IMG_RESULT INT_DG_CameraRegister(KRN_DG_CONNECTION *pConnection, DG_CAMERA __user *pCam, int *pGivenId);

IMG_RESULT INT_DG_CameraAddBuffers(KRN_DG_CONNECTION *pConnection, struct DG_CAMERA_PARAM *pParam);
IMG_RESULT INT_DG_CameraStart(KRN_DG_CONNECTION *pConnection, int cameraId);
IMG_RESULT INT_DG_CameraStop(KRN_DG_CONNECTION *pConnection, int cameraId);
IMG_RESULT INT_DG_CameraAcquireBuffer(KRN_DG_CONNECTION *pConnection, int cameraId, IMG_BOOL8 bBlocking, int *pGivenShot);
IMG_RESULT INT_DG_CameraShoot(KRN_DG_CONNECTION *pConnection, int cameraId, int frameId, IMG_BOOL8 bTrigger);

IMG_RESULT INT_DG_CameraDeregister(KRN_DG_CONNECTION *pConnection, int iCameraId);

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

#endif // DG_CONNECTION
