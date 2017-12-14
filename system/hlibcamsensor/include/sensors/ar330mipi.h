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

#ifndef _AR330MIPI_
#define _AR330MIPI_

#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/param.h>
#include <sys/ioctl.h>  
#include <unistd.h>
#include "sensorapi/sensorapi.h"
#include "img_types.h"
#include "sensorapi/sensor_phy.h"
#include <img_errors.h>
#include <ci/ci_api_structs.h>
#include <felixcommon/userlog.h>
#include <math.h>
#include <img_types.h>
#include "sensor_name.h"

#ifdef INFOTM_ISP
IMG_RESULT AR330_Create_MIPI(struct _Sensor_Functions_ **hHandle, int index);
#else
IMG_RESULT AR330_Create_MIPI(struct _Sensor_Functions_ **hHandle);
#endif
//int AR330MIPI_Create(SENSOR_HANDLE *hHandle);

#endif
