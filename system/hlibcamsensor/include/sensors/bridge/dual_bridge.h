#ifndef DUAL_DVP_BRIDGE_H
#define DUAL_DVP_BRIDGE_H

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
#include "sensors/sensor_name.h"

#ifdef __cplusplus
extern "C" {
#endif
struct _Sensor_Functions_; // define in sensorapi/sensorapi.h

IMG_RESULT DualBridge_Create(struct _Sensor_Functions_ **hHandle, int index);

#ifdef __cplusplus
}
#endif


#endif
