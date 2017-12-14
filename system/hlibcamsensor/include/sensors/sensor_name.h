/*
 * sensor_name.h
 *
 *  Created on: Jan 19, 2017
 *      Author: zhouc
 */

#ifndef SENSOR_NAME_H_
#define SENSOR_NAME_H_
#define AR330DVP_SENSOR_INFO_NAME "ar330dvp"
#define AR330MIPI_SENSOR_INFO_NAME "ar330mipi"
#define IMX179MIPI_SENSOR_INFO_NAME "imx179mipi"
#define IMX322DVP_SENSOR_INFO_NAME "imx322dvp"
#define OV2710_SENSOR_INFO_NAME "ov2710"
#define OV4688_SENSOR_INFO_NAME "ov4688"
#define OV4689_SENSOR_INFO_NAME "ov4689"
#define OV683Dual_SENSOR_INFO_NAME "ov683"
#define P401_SENSOR_INFO_NAME "p401"
#define SC1135DVP_SENSOR_INFO_NAME "sc1135dvp"
#define SC2135DVP_SENSOR_INFO_NAME "sc2135dvp"
#define SC2235DVP_SENSOR_INFO_NAME "sc2235dvp"
#define SC3035DVP_SENSOR_INFO_NAME "sc3035dvp"
#define STK3855DUAL720DVP_SENSOR_INFO_NAME "stk3855dvp"
#define DUAL_BRIDGE_INFO_NAME "dual-bridge"
#define NT99142DVP_SENSOR_INFO_NAME "nt99142dvp"
#define HM2131Dual_SENSOR_INFO_NAME "hm2131mipi"


#define delete_space(pstr) do { \
    char *ptmp = pstr; \
    while(*pstr != '\0') { \
        if ((*pstr != ' ')) { \
            *ptmp++ = *pstr; \
        } \
        ++pstr; \
    } \
    *ptmp = '\0'; \
}while(0)

#endif /* SENSOR_NAME_H_ */
