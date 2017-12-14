/**
******************************************************************************
 @file ar330.h

@brief AR330 functions

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

#ifndef _AR330_
#define _AR330_

#include <img_types.h>

#ifdef __cplusplus
extern "C" {
#endif
struct _Sensor_Functions_; // define in sensorapi/sensorapi.h

#define AR330_SENSOR_INFO_NAME "AR330"
#define AR330_SPECIAL_MODE 0xFF


//#define AR330DVP_SENSOR_INFO_NAME "AR330DVP"
//#define AR330MIPI_SENSOR_INFO_NAME "AR330MIPI"

//IMG_RESULT AR330_Create_DVP(struct _Sensor_Functions_ **hHandle);
//IMG_RESULT AR330_Create_MIPI(struct _Sensor_Functions_ **hHandle);

/**
 * @ingroup SENSOR_API
 */
IMG_RESULT AR330_Create(struct _Sensor_Functions_ **hHandle);

/**
 * @brief Load registers from a file as the special mode AR330_SPECIAL_MODE
 * @ingroup SENSOR_API
 *
 * This allow to specify registers for the AR330 at run time instead of
 * compiling them in. It is not expected to be used on a final product but
 * is there to ease the mode integration.
 *
 * @warning If the provided file is invalid or does not contain all the 
 * needed registers for the getModeInfo() function to get the correct
 * information this function may not realise it
 *
 * File must be formatted to have pairs for hexadecimal values as 
 * 'offset value' as the parser will not cope with other characters.
 *
 * The parser will stop when fscanf cannot find couples of hexadecimal data
 * any more.
 *
 * If file contains more than 1 couple it will override the present values for
 * the special mode.
 * If the loaded data makes getModeInfo() return errors the registers will be
 * discarded.
 *
 * @warning this feature is not robust - SHOULD NOT BE USED in real systems.
 * 
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_INITIALISED if psSensorPhy is NULL (object not 
 * initialised correctly)
 * @return IMG_ERROR_INVALID_PARAMETERS if failed to open filename
 * @return IMG_ERROR_FATAL if failed to read at least 2 registers
 * @return IMG_ERROR_NOT_SUPPORTED if getModeInfo() failed with loaded values
 */
IMG_RESULT AR330_LoadSpecialModeRegisters(struct _Sensor_Functions_ *hHandle,
    const char *filename);

#ifdef __cplusplus
}
#endif

#endif
