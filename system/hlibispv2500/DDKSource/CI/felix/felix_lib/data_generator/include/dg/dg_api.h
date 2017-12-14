/**
******************************************************************************
 @file dg_api.h

 @brief Data Generator structures

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
#ifndef __DG_API__
#define __DG_API__

#include "dg/dg_api_structs.h"
struct _sSimImageIn_;//#include <sim_image.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CI_CONNECTION; // ci/ci_api.h

/**
 * @defgroup DG_API Data Generator: API
 * @brief Documentation of the Data Generator user-side driver API.
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the DG_API documentation module
 *---------------------------------------------------------------------------*/

/**
 * @defgroup DG_API_driver Driver functions (DG_Driver)
 * @brief The basic communication with kernel-side
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Data Generator Driver group
 *---------------------------------------------------------------------------*/

/**
 * @brief Connection to the kernel-side of the driver.
 *
 * Creates an internal structure to hold the information about the connection
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_ALREADY_INITIALISED if connection was already established
 * @return IMG_ERROR_MALLOC_FAILED if the internal structure allocation failed
 * @return IMG_ERROR_UNEXPECTED_STATE if the connection to the kernel-side
 * failed (open failed)
 * @return IMG_ERROR_FATAL if the communication with the kernel driver failed
 * (get information failed)
 *
 * @see DG_DriverFinalise()
 */
IMG_RESULT DG_DriverInit(void);

/**
 * @brief Access the number of available Data Generators in HW
 */
IMG_UINT8 DG_DriverNDatagen(void);

/**
 * @brief Terminates the connection to the kernel-side
 *
 * @warning The Camera objects that are left are destroyed
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if the connection was not initialised
 * with DG_DriverInit()
 */
IMG_RESULT DG_DriverFinalise(void);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Data Generator Driver group
 *---------------------------------------------------------------------------*/

/**
 * @defgroup DG_API_Camera Camera object (DG_Camera)
 * @brief Converts and submit images to the data generator HW
 *
 * Usage of a camera object is: creation (@ref DG_CameraCreate), then the user
 * can configure it to finally register it to kernel side
 * (@ref DG_CameraRegister).
 *
 * Buffer can then be added (@ref DG_CameraAddPool), the capture started
 * (@ref DG_CameraStart) and frames shot 
 * (@ref DG_CameraShoot DG_CameraShootNB).
 *
 * Once finished the capture should be stopped (@ref DG_CameraStop) and the
 * camera destroyed (@ref DG_CameraDestroy).
 *
 * Cameras configuration cannot be changed after their registration.
 *
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Data Generator Camera group
 *---------------------------------------------------------------------------*/

/**
 * @brief Creates a Data Generator camera object and sets it to have default
 * values.
 *
 * Creates a camera object and a converter object in user side only.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if ppCamera is NULL
 * @return IMG_ERROR_MEMORY_IN_USE if *ppCamera is not NULL
 * @return IMG_ERROR_NOT_INITIALISED if not connected to the kernel-side
 * driver
 * @return IMG_ERROR_MALLOC_FAILED if internal allocation failed
 */
IMG_RESULT DG_CameraCreate(DG_CAMERA **ppCamera);

/**
 * @brief Destroys a Data Generator object and its device memory structure
 *
 * If the Camera is running it tries to stop it first.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL
 * @return IMG_ERROR_FATAL if stopping the camera was a failure
 */
IMG_RESULT DG_CameraDestroy(DG_CAMERA *pCamera);

/**
 * @brief Register the Camera object to the kernel side (copies the
 * configuration)
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL
 * @return IMG_ERROR_ALREADY_INITIALISED if the Camera was already 
 * registered to kernel-side
 * @return IMG_ERROR_FATAL if the format is not supported or registration 
 * to kernel-side failed
 * @return IMG_ERROR_NOT_SUPPORTED if the requested DG is not accessible 
 * (not enough DGs)
 *
 * @see Delegates to DG_CameraVerify() for verification of the configuration
 */
IMG_RESULT DG_CameraRegister(DG_CAMERA *pCamera);

/**
 * @brief Add available frames to a registered camera
 *
 * @warning Frames cannot be added to a started camera
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL or ui32ToAdd is 0
 * @return IMG_ERROR_NOT_INITIALISED if the Camera is not registered to 
 * kernel-side
 * @return IMG_ERROR_UNEXPECTED_STATE if the capture is already started
 * @return IMG_ERROR_MALLOC_FAILED if internal structure allocation failed
 * @return IMG_ERROR_FATAL if the creation of device memory in kernel-side 
 * failed (or memory mapping to user-side failed)
 */
IMG_RESULT DG_CameraAddPool(DG_CAMERA *pCamera, IMG_UINT32 ui32ToAdd);

/**
 * @brief Start the capture - acquire the needed HW for captures.
 *
 * Registers the needed HW in kernel-side and allow frames to be shot.
 *
 * Kernel-side can refuse a camera to be started if the HW is not available.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL
 * @return IMG_ERROR_NOT_INITIALISED if the Camera is not registered to 
 * kernel-side
 * @return IMG_ERROR_NOT_SUPPORTED if the Camera is already started
 * @return IMG_ERROR_FATAL if kernel-side refused to start this camera
 */
IMG_RESULT DG_CameraStart(DG_CAMERA *pCamera,
    struct CI_CONNECTION *pCIConnection);

/**
 * @brief Stop the capture - liberate the HW used for capture
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if the Camera is not registered
 * to kernel-side
 * @return IMG_ERROR_NOT_SUPPORTED if the Camera is not started
 * @return IMG_ERROR_FATAL if kernel-side did not stop the camera
 */
IMG_RESULT DG_CameraStop(DG_CAMERA *pCamera);

/**
 * @brief Verifies that the Camera's configuration is correct
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL
 * @return IMG_ERROR_FATAL
 * @li if width or height are NULL
 * @li if horizontal or vertical blanking are too small
 * (FELIX_MIN_H_BLANKING, FELIX_MIN_V_BLANKING
 * @li if the given stride is not fitting the converted stride
 * (format specific)
 * @li if the given width is not fitting the converted width
 * (format specific)
 *
 * @see Delegates to @ref DG_ConvGetConvertedStride() and
 * @ref DG_ConvGetConvertedSize() to know converted size and stride
 */
IMG_RESULT DG_CameraVerify(DG_CAMERA *pCamera);

/**
 * @brief Shoot a frame - fails if no buffer is available
 *
 * @param pCamera
 * @param pIMG
 * @param pszTmpFile if not NULL will save converted frame to that file
 * 
 * @return a converted errno value when acquiring a frame failed
 *
 * @see DG_CameraShoot for blocking equivalent (and return code values)
 */
IMG_RESULT DG_CameraShootNB(DG_CAMERA *pCamera,
    const struct _sSimImageIn_* pIMG, const char *pszTmpFile);

/**
 * @brief Shoot a frame - blocks until a buffer is available
 * 
 * @param pCamera
 * @param pIMG
 * @param pszTmpFile if not NULL will save converted frame to that file
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera os pIMG is NULL
 * @return IMG_ERROR_NOT_INITIALISED if the Camera is not registered to
 * kernel-side
 * @return IMG_ERROR_NOT_SUPPORTED if Camera is not started
 * @return IMG_ERROR_FATAL if the kernel-side gives a frame that user-side
 * does not know about (internal failure)
 * @return a converted errno value when acquiring a frame failed
 *
 * @see DG_CameraShootNB for non-blocking equivalent
 */
IMG_RESULT DG_CameraShoot(DG_CAMERA *pCamera,
    const struct _sSimImageIn_* pIMG, const char *pszTmpFile);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the Data Generator Camera group
 *---------------------------------------------------------------------------*/

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the DG_API
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __DG_API__
