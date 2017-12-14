/**
******************************************************************************
 @file ov683Dual.h

@brief OV683Dual functions

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

#ifndef _OV683Dual_
#define _OV683Dual_

#include <img_types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#include <sys/ioctl.h> 
#include "sensor_name.h"
#ifdef __cplusplus
extern "C" {
#endif

struct _Sensor_Functions_; // define in sensorapi/sensorapi.h


/**
 * @ingroup SENSOR_API
 */
IMG_RESULT OV683Dual_Create(struct _Sensor_Functions_ **hHandle, int index);

#ifdef __cplusplus
}
#endif

#endif
