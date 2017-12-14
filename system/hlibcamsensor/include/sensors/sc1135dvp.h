/**
 ******************************************************************************
 @file dummycam.h

 @brief Dummy camera header file

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

#ifndef _SC1135_
#define _SC1135_

#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#include <sys/ioctl.h>  

#include "sensorapi/sensorapi.h"
#include "img_types.h"
#include "sensorapi/sensor_phy.h"
#include <img_errors.h>
#include <ci/ci_api_structs.h> 
#include <felixcommon/userlog.h>

#include <img_types.h>
#include "sensor_name.h"
#ifdef __cplusplus
extern "C" {
#endif
//struct _Sensor_Functions_; // define in sensorapi/sensorapi.h


/**
 * @ingroup SENSOR_API
 */
IMG_RESULT SC1135DVP_Create(struct _Sensor_Functions_ **hHandle);

/**
 * @brief Load registers from a file as the special mode SC1135_SPECIAL_MODE
 * @ingroup SENSOR_API
 *
 * This allow to specify registers for the SC1135 at run time instead of
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

#ifdef __cplusplus
}
#endif

#endif
