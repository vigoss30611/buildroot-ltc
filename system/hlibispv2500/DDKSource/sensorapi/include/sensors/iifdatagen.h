/**
******************************************************************************
 @file iifdatagen.h

@brief IIF Data Generator sensor driver

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

#ifndef SENSORAPI_IIF_DG
#define SENSORAPI_IIF_DG

#include <img_types.h> /* IMG_RESULT */

#ifdef __cplusplus
extern "C" {
#endif

struct _Sensor_Functions_; /* see sensorapi.h, also known as SENSOR_HANDLE */
struct CI_CONNECTION;

/**
 * @defgroup IIF_DATAGEN Internal Data Generator Sensor
 * @ingroup SENSOR_API
 * @brief Internal Data Generator sensor implementation
 *
 * The internal data generator is part of the V2500 IP and is between the ISP
 * contexts and gaskets.
 * It can be used to replace data from a context by loading image information
 * from memory.
 *
 * Extended parameters have to be used to set additional information that a
 * normal sensor cannot provide:
 * @li CI_Connection object to use to register the data generator 
 * @li source file in FLX format to load the image data from 
 * @li gasket data to replace
 *
 * Other extended parameter are optional but can use to change the behaviour of
 * the internal data generator.
 *
 * @{
 */

#define IIFDG_SENSOR_INFO_NAME ("IIF Datagen")

/**
 * @return IMG_SUCCESS
 * @return IMG_ERROR_MALLOC_FAILED if internal allocation failed
 */
IMG_RESULT IIFDG_Create(struct _Sensor_Functions_ **phHandle);

/**
 * @name Extended Functions
 * @brief All extended functions for internal data generator driving
 * @{
 */

/**
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if hHandle is NULL
 * @return IMG_ERROR_INVALID_PARAMETERS if conn is NULL
 * @return IMG_ERROR_ALREADY_INITIALISED if object already has a connection
 */
IMG_RESULT IIFDG_ExtendedSetConnection(struct _Sensor_Functions_ *hHandle, struct CI_CONNECTION* conn);

/**
 * @brief Set the source file to load image data from (HAS to be RGGB FLX file)
 * @note Cannot be changed once state is STATE_RUNNING
 *
 * @warning resets the frameCap to number of available frames
 *
 * @param hHandle
 * @param filename to load from - value is duplicated
 *
 * Get value using IIFDG_ExtendedGetSourceFile()
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if hHandle is NULL
 * @return IMG_ERROR_INVALID_PARAMETERS if filename could not be loaded
 * @return IMG_ERROR_OPERATION_PROHIBITED if sensor already enabled
 * Delegates to IIFDG_GetState()
 */
IMG_RESULT IIFDG_ExtendedSetSourceFile(struct _Sensor_Functions_ *hHandle, const char *filename);
/**
 * @brief Access to the filename to load image data from
 *
 * Modify with IIFDG_ExtendedSetSourceFile()
 *
 * @param hHandle
 * @param[out] filename value is given by copy - DO NOT FREE!
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if hHandle is NULL
 * @return IMG_ERROR_INVALID_PARAMETERS if filename is NULL
 */
IMG_RESULT IIFDG_ExtendedGetSourceFile(struct _Sensor_Functions_ *hHandle, const char **filename);

IMG_RESULT IIFDG_ExtendedSetGasket(struct _Sensor_Functions_ *hHandle, IMG_UINT8 ui8Gasket);
IMG_RESULT IIFDG_ExtendedGetGasket(struct _Sensor_Functions_ *hHandle, IMG_UINT8 *pGasket);

IMG_RESULT IIFDG_ExtendedSetDatagen(struct _Sensor_Functions_ *hHandle, IMG_UINT8 ui8Context);
IMG_RESULT IIFDG_ExtendedGetDatagen(struct _Sensor_Functions_ *hHandle, IMG_UINT8 *pContext);

/**
 * @brief if DGCam isVideo then this is the maximum number of frames to load from source file
 *
 * @warning reseted to number of frames every-time source is changed
 *
 * @param hHandle
 * @param ui32FrameCap maximum number of frames to load from FLX source file
 * Cannot be bigger than the source file number of frames but can be smaller.
 *
 * Get current value with IIFDG_ExtendedGetFrameCap()
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if hHandle is NULL
 * @return IMG_ERROR_NOT_SUPPORTED if proposed ui32FrameCap is bigger than the number of frames from the source file
 */
IMG_RESULT IIFDG_ExtendedSetFrameCap(struct _Sensor_Functions_ *hHandle, IMG_UINT32 ui32FrameCap);
/**
 * @brief Accessor to the maximum number of frames to load from source file.
 *
 * Modify with IIFDG_ExtendedSetFrameCap()
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if hHandle is NULL
 * @return IMG_ERROR_INVALID_PARAMETERS if pFrameCap is NULL
 */
IMG_RESULT IIFDG_ExtendedGetFrameCap(struct _Sensor_Functions_ *hHandle, IMG_UINT32 *pFrameCap);

/**
 * @brief Access to the frame number that will be loaded next [read-only]
 */
IMG_UINT32 IIFDG_ExtendedGetFrameNO(struct _Sensor_Functions_ *hHandle);

/**
 * @brief Access to the number of frames available in the selected source [read-only]
 */
IMG_UINT32 IIFDG_ExtendedGetFrameCount(struct _Sensor_Functions_ *hHandle);

IMG_RESULT IIFDG_ExtendedSetIsVideo(struct _Sensor_Functions_ *hHandle, IMG_BOOL8 bIsVideo);
IMG_RESULT IIFDG_ExtendedGetIsVideo(struct _Sensor_Functions_ *hHandle, IMG_BOOL8 *pIsVideo);

IMG_RESULT IIFDG_ExtendedSetNbBuffers(struct _Sensor_Functions_ *hHandle, IMG_UINT32 ui32NBuffers);
IMG_RESULT IIFDG_ExtendedGetNbBuffers(struct _Sensor_Functions_ *hHandle, IMG_UINT32 *pNBuffers);

IMG_RESULT IIFDG_ExtendedSetBlanking(struct _Sensor_Functions_ *hHandle, IMG_UINT32 ui32HBlanking, IMG_UINT32 ui32VBlanking);
IMG_RESULT IIFDG_ExtendedGetBlanking(struct _Sensor_Functions_ *hHandle, IMG_UINT32 *pHBlanking, IMG_UINT32 *pVBlanking);

/**
 * @}
 */
/**
 * @}
 */
/*----------------------------------------------------------------------------
 * End of the IIF Datagen functions group
 *---------------------------------------------------------------------------*/ 

#ifdef __cplusplus
}
#endif

#endif /* SENSORAPI_IIF_DG */
