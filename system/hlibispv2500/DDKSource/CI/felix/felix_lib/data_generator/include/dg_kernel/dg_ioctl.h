/**
******************************************************************************
 @file dg_ioctl.h

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
#ifndef CI_IOCTRL
#define CI_IOCTRL

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DG_KERNEL Data Generator: kernel-side driver
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the DG_KERNEL documentation module
 *------------------------------------------------------------------------*/

/**
 * @name User/kernel IO controls
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *------------------------------------------------------------------------*/

#include <linux/ioctl.h>
#include <stddef.h>

#include "dg/dg_api_structs.h" 

struct DG_CAMERA_PARAM {
	int cameraId;
	unsigned int nbBuffers;
	int *aMemMapIds; ///< @brief Should be nbBuffers - output
	size_t stride; // output
	//size_t height; // output
};

struct DG_FRAME_PARAM {
	int cameraId;
	int frameId;
	/**
	 * @brief Behaviour choice:
	 * @li when Acquiring a buffer used to know if the call is blocking.
	 * @li when releasing a buffer used to know if capture needs to be triggered
	 */
	IMG_BOOL8 bOption; 
};

enum DG_eIOCTL {
	DG_INFO_GET,
	
	DG_CAMERA_REGISTER,
	DG_CAMERA_ADDBUFFER,
	DG_CAMERA_START,
	DG_CAMERA_STOP,
	DG_CAMERA_ACQBUFFER, // get an available buffer for the shoot
	DG_CAMERA_RELBUFFER,	// releases the available buffer for the shoot and trigger the shoot
	DG_CAMERA_DEREGISTER,
};

/**
 * @brief IOCTL magic number
 */
#define DG_MAGIC '~'

//
// reading/writing is relative to user space memory
//
#define DG_IOCTL_INFO _IOW(DG_MAGIC, DG_INFO_GET, DG_HWINFO*)

#define DG_IOCTL_CAM_REG _IOR(DG_MAGIC, DG_CAMERA_REGISTER, DG_CAMERA*)
#define DG_IOCTL_CAM_ADD _IOWR(DG_MAGIC, DG_CAMERA_ADDBUFFER, struct DG_CAMERA_PARAM*)
#define DG_IOCTL_CAM_STA _IOR(DG_MAGIC, DG_CAMERA_START, int)
#define DG_IOCTL_CAM_STP _IOR(DG_MAGIC, DG_CAMERA_STOP, int)
#define DG_IOCTL_CAM_BAQ _IOW(DG_MAGIC, DG_CAMERA_ACQBUFFER, struct DG_FRAME_PARAM*)
#define DG_IOCTL_CAM_BRE _IOR(DG_MAGIC, DG_CAMERA_RELBUFFER, struct DG_FRAME_PARAM*)
#define DG_IOCTL_CAM_DEL _IOR(DG_MAGIC, DG_CAMERA_DEREGISTER, int)

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the IOCTL group
 *------------------------------------------------------------------------*/

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the DG_KERNEL documentation module
 *------------------------------------------------------------------------*/

#endif // CI_IOCTRL
