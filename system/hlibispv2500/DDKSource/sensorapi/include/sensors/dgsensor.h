/**
******************************************************************************
 @file dgsensor.h

@brief External Data Generator sensor driver

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

#ifndef _DGSENSOR_
#define _DGSENSOR_

#include <img_types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CI_CONNECTION;
struct _Sensor_Functions_;

/**
 * @defgroup EXT_DATAGEN External Data Generator Sensor
 * @ingroup SENSOR_API
 * @brief External Data Generator sensor implementation
 *
 * The external data generator is NOT part of the V2500 IP and is connected to
 * a unique gasket (sharing its number, i.e. ExtDG0 <=> gasket0).
 * It can be used to replace data from a context by loading image information
 * from memory.
 *
 * Extended parameters have to be used to set additional information that a
 * normal sensor cannot provide:
 * @li CI_Connection object to use to register the data generator 
 * @li source file in FLX format to load the image data from 
 * @li optionally gasket data to replace by changing its context
 *
 * Other extended parameter are optional but can use to change the behaviour of
 * the external data generator.
 *
 * @{
 */

#define EXTDG_SENSOR_INFO_NAME "External Datagen"

IMG_RESULT DGCam_Create(struct _Sensor_Functions_ **hHandle);

/**
 * @name Extended Functions
 * @brief All extended functions for external data generator driving
 * @{
 */

IMG_RESULT DGCam_ExtendedSetConnection(struct _Sensor_Functions_ *handle, struct CI_CONNECTION* conn);

IMG_RESULT DGCam_ExtendedSetSourceFile(struct _Sensor_Functions_ *hHandle, const char *filename);
IMG_RESULT DGCam_ExtendedGetSourceFile(struct _Sensor_Functions_ *handle, const char **filename);

/**
 * @brief if DGCam isVideo then this is the maximum number of frames to load from source file
 *
 * @param hHandle
 * @param ui32FrameCap maximum number of frames to load from FLX source file
 * Cannot be bigger than the source file number of frames but can be smaller.
 */
IMG_RESULT DGCam_ExtendedSetFrameCap(struct _Sensor_Functions_ *hHandle, IMG_UINT32 ui32FrameCap);
IMG_RESULT DGCam_ExtendedGetFrameCap(struct _Sensor_Functions_ *hHandle, IMG_UINT32 *pFrameCap);

IMG_UINT32 DGCam_ExtendedGetFrameNo(struct _Sensor_Functions_ *hHandle);

/**
 * @brief Uses configured gasket to get information
 */
IMG_BOOL8 DGCam_ExtendedGetUseMIPI(struct _Sensor_Functions_ *hHandle);

/**
 * @brief Used only if gasket is MIPI
 */
IMG_RESULT DGCam_ExtendedSetUseMIPILF(struct _Sensor_Functions_ *hHandle, IMG_BOOL8 bUseMIPILF);
IMG_RESULT DGCam_ExtendedGetUseMIPILF(struct _Sensor_Functions_ *hHandle, IMG_BOOL8 *pUseMIPILF);

IMG_UINT32 DGCam_ExtendedGetFrameCount(struct _Sensor_Functions_ *hHandle);

IMG_RESULT DGCam_ExtendedSetIsVideo(struct _Sensor_Functions_ *handle, IMG_BOOL8 bIsVideo);
IMG_RESULT DGCam_ExtendedGetIsVideo(struct _Sensor_Functions_ *handle, IMG_BOOL8 *pIsVideo);

IMG_RESULT DGCam_ExtendedSetNbBuffers(struct _Sensor_Functions_ *handle, IMG_UINT32 ui32NBuffers);
IMG_RESULT DGCam_ExtendedGetNbBuffers(struct _Sensor_Functions_ *handle, IMG_UINT32 *pNBuffers);

IMG_RESULT DGCam_ExtendedSetBlanking(struct _Sensor_Functions_ *handle, IMG_UINT32 ui32HBlanking, IMG_UINT32 ui32VBlanking);
IMG_RESULT DGCam_ExtendedGetBlanking(struct _Sensor_Functions_ *handle, IMG_UINT32 *pHBlanking, IMG_UINT32 *pVBlanking);

IMG_RESULT DGCam_ExtendedSetGasket(struct _Sensor_Functions_ *handle, IMG_UINT8 ui8Gasket);
IMG_RESULT DGCam_ExtendedGetGasket(struct _Sensor_Functions_ *handle, IMG_UINT8 *pGasket);

/**
 * @}
 */
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // _DGSENSOR_
