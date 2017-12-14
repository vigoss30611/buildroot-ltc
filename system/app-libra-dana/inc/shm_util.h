#ifndef _SHM_UTIL_H_
#define _SHM_UTIL_H_

#define LOG_SHM_KEY 0x99123458

#define SHM_KEY      0x99123459
#define CFG_SHM_KEY  0x99123460
#define IPC_SHM_KEY  0x99123461

typedef enum {
    E_PROD_IP_CAMERA,
    E_PROD_SPORT_DV,
    E_PROD_CAR_RECORDER,
    E_PROD_Q3_SPORT_DV,
} T_CFG_PRODUCE_TYPE;

typedef enum {
    E_SENSOR_MIPI,
    E_SENSOR_DVP,
} T_CFG_SENSOR_TYPE;


#endif

