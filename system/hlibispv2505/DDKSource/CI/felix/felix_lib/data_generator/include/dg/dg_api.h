/**
*******************************************************************************
 @file dg_api.h

 @brief External Data Generator handling

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
#ifndef DG_API_H_
#define DG_API_H_

#include "dg/dg_api_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _sSimImageIn_;  // sim_image.h
struct CI_CONNECTION;  // ci/ci_api.h

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
 * @param ppConnection will create a connection object
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if ppConnection is NULL or
 * *ppConnection contains an object already
 * @return IMG_ERROR_MALLOC_FAILED if the internal structure allocation failed
 * @return IMG_ERROR_UNEXPECTED_STATE if the connection to the kernel-side
 * failed (open failed)
 * @return IMG_ERROR_FATAL if the communication with the kernel driver failed
 * (get information failed)
 *
 * @see DG_DriverFinalise()
 */
IMG_RESULT DG_DriverInit(DG_CONNECTION **ppConnection);

/**
 * @brief Terminates the connection to the kernel-side
 *
 * @warning The Camera objects that are left are destroyed
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pConnection is NULL
 */
IMG_RESULT DG_DriverFinalise(DG_CONNECTION *pConnection);

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
 * can configure.
 *
 * Buffer can then be added (@ref DG_CameraAllocateFrame).
 * When the capture started (@ref DG_CameraStart) the configuration is
 * updated.
 *
 * Available buffers are accessed with @ref DG_CameraGetAvailableFrame and
 * can be populated using the @ref CI_CONVERTER functions. Once ready the
 * frame is pushed with @ref DG_CameraInsertFrame or the capture is cancelled
 * with @ref DG_CameraReleaseFrame.
 *
 * Once finished the capture should be stopped (@ref DG_CameraStop) and the
 * camera can be destroyed (@ref DG_CameraDestroy).
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Data Generator Camera group
 *---------------------------------------------------------------------------*/

/**
 * @brief Creates a Data Generator camera object and sets it to have default
 * values.
 *
 * Creates a camera object and registers it to the kernel side.
 *
 * @param[out] ppCamera to create
 * @param[out] pConnection attached to this connection
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if ppCamera is NULL or *ppCamera
 * already contains an object or pConnection is NULL
 * @return IMG_ERROR_MALLOC_FAILED if internal allocation failed
 */
IMG_RESULT DG_CameraCreate(DG_CAMERA **ppCamera, DG_CONNECTION *pConnection);

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
 * @brief Allocate a new frame of a given size.
 *
 * @param pCamera the frame will be used on
 * @param ui32Size of allocation (>0)
 * @param[out] pFrameID if non-NULL the Id used to identify the frame
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL or ui32Size is 0
 * @return IMG_ERROR_NOT_INITIALISED if camera was not registered in kernel
 * side (unlikely)
 * @return IMG_ERROR_MALLOC_FAILED if internal buffer allocation failed
 * @return IMG_ERROR_FATAL if mapping to user-space failed (unlikely)
 */
IMG_RESULT DG_CameraAllocateFrame(DG_CAMERA *pCamera, IMG_UINT32 ui32Size,
    IMG_UINT32 *pFrameID);

/**
 * @brief Start the capture - acquire the needed HW for captures.
 *
 * Registers the needed config in kernel-side and allow frames to be
 * triggered.
 *
 * Kernel-side can refuse a camera to be started if the HW is not available.
 *
 * @warning The CI gasket has to be configured and acquired from another
 * function!
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL
 * @return IMG_ERROR_NOT_INITIALISED if the Camera is not registered to 
 * kernel-side (unlikely)
 * @return IMG_ERROR_NOT_SUPPORTED if the Camera is already started
 * @return IMG_ERROR_FATAL if kernel-side refused to start this camera
 */
IMG_RESULT DG_CameraStart(DG_CAMERA *pCamera);

/**
 * @brief Stop the capture - liberate the HW used for capture
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS ifpCamera is NULL
 * @return IMG_ERROR_NOT_INITIALISED if the Camera is not registered
 * to kernel-side
 * @return IMG_ERROR_NOT_SUPPORTED if the Camera is not started
 * @return IMG_ERROR_FATAL if kernel-side did not stop the camera
 */
IMG_RESULT DG_CameraStop(DG_CAMERA *pCamera);

/**
 * @brief Get the first available frame for a capture
 *
 * Will contact kernel module to get a frame ID. This will be blocking until
 * a frame is available.
 *
 * Similar to @ref DG_CameraGetFrame() without frame ID
 *
 * @return pointer to the frame to use
 * @return NULL if no frames are available
 */
DG_FRAME* DG_CameraGetAvailableFrame(DG_CAMERA *pCamera);

/**
 * @brief Similar to @ref DG_CameraGetAvailableFrame() but with a specified
 * framed ID.
 */
DG_FRAME* DG_CameraGetFrame(DG_CAMERA *pCamera, IMG_UINT32 ui32FrameID);

/**
 * @brief Insert a frame for the capture.
 *
 * The frame should have been acquired with @ref DG_CameraGetAvailableFrame()
 * or an equivalent function and convertion should have been made to the data
 * if needed.
 *
 * If triggered with success the completion should be waited on using
 * @ref DG_CameraWaitProcessedFrame() at a later point.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if associated Camera is not registered
 * to kernel-side
 * @return IMG_ERROR_FATAL if failed to insert frame
 */
IMG_RESULT DG_CameraInsertFrame(DG_FRAME *pFrame);

/**
 * @brief Cancels the acquisition of a frame.
 *
 * The frame should have been acquired with @ref DG_CameraGetAvailableFrame()
 * or an equivalent function and will be released
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if associated Camera is not registered
 * to kernel-side
 * @return IMG_ERROR_FATAL if failed to insert frame
 */
IMG_RESULT DG_CameraReleaseFrame(DG_FRAME *pFrame);

/**
 * @brief Wait for the frame to be fully processed by the data-generator.
 *
 * When fully processed the frame is pushed back into the available list.
 *
 * @param pCamera
 * @param bBlocking wait until a frame is processed or just return directly
 * if no frames were processed yet
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pDatagen is NULL
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE when no frames were processed
 * in non-blocking wait
 * @return IMG_ERROR_INTERRUPTED if the blocking wait was interrupted or
 * timed out (see insmod parameters)
 */
IMG_RESULT DG_CameraWaitProcessedFrame(DG_CAMERA *pCamera,
    IMG_BOOL8 bBlocking);

/**
 * @brief Destroy a specific frame
 *
 * The frame should have been acquired with @ref DG_CameraGetAvailableFrame()
 * or an equivalent function.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if associated Camera is not registered
 * to kernel-side or pFrame is NULL
 */
IMG_RESULT DG_CameraDestroyFrame(DG_FRAME *pFrame);

/**
 * @brief Verifies that the Frame's configuration is correct
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if pCamera is NULL
 * @return IMG_ERROR_NOT_SUPPORTED
 * @li if width or height are NULL
 * @li if horizontal or vertical blanking are too small
 * (FELIX_MIN_H_BLANKING, FELIX_MIN_V_BLANKING
 * @return IMG_ERROR_FATAL
 * @li if the given stride*height does not fit in allocated frame
 * @li if the given frame format is different than the camera
 */
IMG_RESULT DG_FrameVerify(DG_FRAME *pFrame);

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

#endif /* DG_API_H_ */
