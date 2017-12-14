/*
 * xc9080_driver.h
 *
 *  Created on: Mar 11, 2017
 *      Author: xiaohui.wei
 */

#ifndef XC9080_DRIVER_H_
#define XC9080_DRIVER_H_
#include <linux/ioctl.h>
#define CTRL_MAGIC	('O')
#define GETI2CADDR	_IOW(CTRL_MAGIC, 1, unsigned long)
#define BYPASSMODE	_IOW(CTRL_MAGIC, 2, unsigned long)

#define XC9080_BYPASS_ON	1
#define XC9080_BYPASS_OFF	0

#endif /* XC9080_DRIVER_H_ */
