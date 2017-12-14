/*
 * ddk_sensor_driver.h
 *
 *  Created on: Apr 28, 2016
 *      Author: zhouc
 */

#ifndef DDK_SENSOR_DRIVER_H_
#define DDK_SENSOR_DRIVER_H_

#define CTRL_MAGIC	('O')

#define GETI2CADDR	_IOW(CTRL_MAGIC, 1, unsigned long)
#define BYPASSMODE	_IOW(CTRL_MAGIC, 2, unsigned long)

#define XC9080_BYPASS_ON	1
#define XC9080_BYPASS_OFF	0

void isp_dvp_wrapper_probe(void);
void isp_dvp_wrapper_init(unsigned int hmode, unsigned int width, unsigned int height,
					unsigned int hdly, unsigned int vdly);
#endif /* DDK_SENSOR_DRIVER_H_ */
