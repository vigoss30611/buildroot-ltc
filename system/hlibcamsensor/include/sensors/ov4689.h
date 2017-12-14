/**
******************************************************************************
 @file ov4689.h

@brief OV4689 functions

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

#ifndef _OV4689_
#define _OV4689_

#include <img_types.h>
#include "sensor_name.h"
#ifdef __cplusplus
extern "C" {
#endif

struct _Sensor_Functions_; // define in sensorapi/sensorapi.h


/**
 * @ingroup SENSOR_API
 */
IMG_RESULT OV4689_Create(struct _Sensor_Functions_ **phHandle, int index);

#ifdef __cplusplus
}
#endif

#endif
