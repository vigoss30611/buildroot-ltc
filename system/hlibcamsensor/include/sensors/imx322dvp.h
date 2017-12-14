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

#ifndef _IMX322DVP_
#define _IMX322DVP_

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
#include "sensor_name.h"


IMG_RESULT IMX322_Create_DVP(struct _Sensor_Functions_ **hHandle);

#endif
