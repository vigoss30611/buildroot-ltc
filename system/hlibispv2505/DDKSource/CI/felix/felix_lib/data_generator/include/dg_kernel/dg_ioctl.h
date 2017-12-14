/**
*******************************************************************************
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
#ifndef DG_IOCTRL_H_
#define DG_IOCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DG_KERNEL Data Generator: kernel-side driver
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the DG_KERNEL documentation module
 *---------------------------------------------------------------------------*/

/**
 * @name User/kernel IO controls
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *---------------------------------------------------------------------------*/

#include <linux/ioctl.h>
#include <stddef.h>

#include "dg/dg_api_structs.h"

struct DG_CAMERA_PARAM {
    /** @brief [in] associated camera Id */
    int cameraId;

    /** @brief [in] format to setup the HW */
    CI_CONV_FMT eFormat;
    IMG_UINT8 ui8FormatBitdepth;
    /** @brief [in] to ensure it does not change between frames */
    enum MOSAICType eBayerOrder;
    /** @brief [in] used a DG context but is 1 to 1 with gasket in ISP */
    IMG_UINT8 ui8Gasket;

    /** @brief Horizontal and Vertical active lines for parallel */
    IMG_BOOL8 bParallelActive[2];

    // when using MIPI format only
    IMG_UINT16 ui16MipiTLPX;
    IMG_UINT16 ui16MipiTHS_prepare;
    IMG_UINT16 ui16MipiTHS_zero;
    IMG_UINT16 ui16MipiTHS_trail;
    IMG_UINT16 ui16MipiTHS_exit;
    IMG_UINT16 ui16MipiTCLK_prepare;
    IMG_UINT16 ui16MipiTCLK_zero;
    IMG_UINT16 ui16MipiTCLK_post;
    IMG_UINT16 ui16MipiTCLK_trail;
};

struct DG_BUFFER_PARAM {
    /** @brief [in] associated camera Id */
    int cameraId;
    /** @brief [in] size to allocate in bytes */
    unsigned int uiAllocSize;
    /** @brief [out] buffer ID != 0 */
    int bufferId;
};

struct DG_FRAME_PARAM {
    /** @brief [in] associated camera Id */
    int cameraId;
    /**
     * @brief Behaviour choice:
     * @li when Acquiring: [in, out] found buffer (in:0 for 1st available,
     * out:id)
     * @li when Triggering/Releasing: [in] buffer to use
     */
    int frameId;
    /**
     * @brief [in] Behaviour choice:
     * @li when Acquiring not used
     * @li when releasing a buffer used to know if capture needs to be
     * triggered
     */
    IMG_BOOL8 bOption;

    // additional info to write to the HW when triggering
    IMG_UINT32 ui32HoriBlanking;
    IMG_UINT32 ui32VertBlanking;

    IMG_UINT16 ui16Height;
    /** for parallel width in pixels, for MIPI converted width */
    IMG_UINT16 ui16Width;
    IMG_UINT32 ui32Stride;
};

struct DG_FRAME_WAIT {
    /** @brief [in] associated camera Id */
    int cameraId;
    /** @brief blocking wait on the semaphore */
    IMG_BOOL8 bBlocking;
};

enum DG_eIOCTL {
    /**
     * @brief Get the HW information
     *
     * [out] DG_HWINFO
     */
    DG_INFO_GET,

    /**
     * @brief Create a kernel-side camera object
     *
     * [out] camera ID (as return)
     */
    DG_CAMERA_REGISTER,
    /**
     * @brief Allocate a buffer
     *
     * [in, out] struct DG_BUFFER_PARAM
     */
    DG_CAMERA_ADDBUFFER,
    /**
     * @brief Start the processing on a camera
     *
     * [in] DG_CAMERA to get format and datagen to use
     */
    DG_CAMERA_START,
    /**
     * @brief Stop the processing on a camera
     *
     * [in] cameraId
     */
    DG_CAMERA_STOP,
    /**
     * @brief To know if a camera is processing
     *
     * [in] cameraId
     */
    DG_CAMERA_IS_RUNNING,
    /**
     * @brief Get an available buffer for the shoot
     */
    DG_CAMERA_ACQBUFFER,
    /**
     * @brief releases the available buffer for the shoot and trigger the
     * shoot
     *
     * [in] DG_FRAME_PARAM
     */
    DG_CAMERA_RELBUFFER,
    /**
     * @brief wait for a processed buffer
     *
     * [in] DG_FRAME_WAIT
     */
    DG_CAMERA_WAITPROCESSED,
    /**
     * @brief delete a buffer
     *
     * [in] DG_BUFFER_PARAM
     */
    DG_CAMERA_DELBUFFER,
    /**
     * @brief Deregister a camera
     *
     * [in] cameraId
     */
    DG_CAMERA_DEREGISTER,

    /** Not an actual IOCTL - just the number of commands */
    DG_IOCTL_N
};

/**
 * @brief IOCTL magic number
 */
#define DG_MAGIC '~'

//
// reading/writing is relative to user space memory
//
#define DG_IOCTL_INFO _IOW(DG_MAGIC, DG_INFO_GET, DG_HWINFO*)

#define DG_IOCTL_CAM_REG _IO(DG_MAGIC, DG_CAMERA_REGISTER)
#define DG_IOCTL_CAM_ADD _IOWR(DG_MAGIC, DG_CAMERA_ADDBUFFER, \
    struct DG_BUFFER_PARAM*)
#define DG_IOCTL_CAM_STA _IOR(DG_MAGIC, DG_CAMERA_START, DG_CAMERA*)
#define DG_IOCTL_CAM_STP _IOR(DG_MAGIC, DG_CAMERA_STOP, int)
#define DG_IOCTL_CAM_BAQ _IOWR(DG_MAGIC, DG_CAMERA_ACQBUFFER, \
    struct DG_FRAME_PARAM*)
#define DG_IOCTL_CAM_BRE _IOR(DG_MAGIC, DG_CAMERA_RELBUFFER, \
    struct DG_FRAME_PARAM*)
#define DG_IOCTL_CAM_WAI _IOR(DG_MAGIC, DG_CAMERA_WAITPROCESSED, \
    struct DG_FRAME_WAIT*)
#define DG_IOCTL_CAM_BDE _IOR(DG_MAGIC, DG_CAMERA_DELBUFFER, \
    struct DG_BUFFER_PARAM*)
#define DG_IOCTL_CAM_DEL _IOR(DG_MAGIC, DG_CAMERA_DEREGISTER, int)

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the IOCTL group
 *---------------------------------------------------------------------------*/

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the DG_KERNEL documentation module
 *---------------------------------------------------------------------------*/

#endif /* DG_IOCTRL_H_ */
