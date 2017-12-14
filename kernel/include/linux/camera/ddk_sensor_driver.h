/*
 * ddk_sensor_driver.h
 *
 *  Created on: Apr 28, 2016
 *      Author: zhouc
 */

#ifndef DDK_SENSOR_DRIVER_H_
#define DDK_SENSOR_DRIVER_H_

#define CTRL_MAGIC	('O')
#define SEQ_NO (1)
#define SEQ_NO1 (2)
#define SEQ_NO2 (3)
#define SEQ_NO3 (4)
#define SEQ_NO4 (5)
#define SEQ_NO5 (6)
#define SEQ_NO6 (7)

#define GETI2CADDR	_IOW(CTRL_MAGIC, SEQ_NO, unsigned long)
#define GETI2CCHN  _IOW(CTRL_MAGIC, SEQ_NO1, unsigned long)
#define GETNAME   _IOW(CTRL_MAGIC, SEQ_NO2, unsigned long)
#define GETIMAGER  _IOW(CTRL_MAGIC, SEQ_NO3, unsigned long)
#define SETPHYENABLE  _IOW(CTRL_MAGIC, SEQ_NO4, unsigned long)
#define GETSENSORPATH  _IOW(CTRL_MAGIC, SEQ_NO5, unsigned long)
#define GETISPPATH  _IOW(CTRL_MAGIC, SEQ_NO6, unsigned long)

void isp_dvp_wrapper_probe(void);
void isp_dvp_wrapper_init(unsigned int hmode, unsigned int width, unsigned int height,
					unsigned int hdly, unsigned int vdly);
void camif_dvp_wrapper_probe(void);
void camif_dvp_wrapper_init(unsigned int hmode, unsigned int width, unsigned int height,
                    unsigned int hdly, unsigned int vdly);
void dvp_wrapper_debug(void);

#ifdef CONFIG_ARCH_APOLLO3
void isp_dvp_wrapper1_probe(void);
void isp_dvp_wrapper1_init(unsigned int hmode, unsigned int width, unsigned int height,
					unsigned int hdly, unsigned int vdly);
#endif

#endif /* DDK_SENSOR_DRIVER_H_ */
