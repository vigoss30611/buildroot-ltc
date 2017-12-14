#ifndef APP_QIWO_H
#define APP_QIWO_H

#include "bsa_api.h"
#include "app_qiwo_ag.h"
#include "app_qiwo_ble.h"
#include "flood_bridge.h"
#include "trans_comm.h"
#include "tcp_task.h"

//#ifndef QIWO_ONLY_COMMAND_RSP
//#define QIWO_ONLY_COMMAND_RSP    TRUE
//#endif

#define QIWO_CMD_DATA_MAX_LEN     512
#define QIWO_RSP_DATA_MAX_LEN     (QIWO_CMD_DATA_MAX_LEN + 3)
#define QIWO_RSP_PACKAGE_MAX      54
#define QIWO_GATT_CHAR_LENGTH     20

#define QIWO_Q6M_DEVICE_MAX_NUM   2
#define QIWO_Q6M_DATA_MAX_LEN     10

#define QIWO_SENSOR_NUM 2 //mfas and ch2o

//ble address type
#define BLE_MESH_NODE 1
#define BLE_SENSOR_NODE 2

#define DIRECT_CONN_NODE_ID_BYTE_INDEX 14

typedef struct{
    UINT8 type;
    UINT16 data_len;
    UINT8 data[QIWO_CMD_DATA_MAX_LEN];
}tQIWO_CMD;

typedef struct{
    BOOLEAN is_use;
    BOOLEAN is_conn;
    BD_ADDR addr;
    UINT16  conn_id;
}tQIWO_SENSOR_INFO;

typedef struct{
    tQIWO_SENSOR_INFO sensor[QIWO_SENSOR_NUM];
}tQIWO_SENSOR_LIST;

typedef struct{
    UINT16 data_len;
    UINT8 data[QIWO_Q6M_DATA_MAX_LEN];
}tQIWO_Q6M_DATA;

typedef struct{
    UINT8 num;
    BOOLEAN is_conn[QIWO_Q6M_DEVICE_MAX_NUM];
    UINT16  conn_id[QIWO_Q6M_DEVICE_MAX_NUM];
    BD_ADDR addr[QIWO_Q6M_DEVICE_MAX_NUM];
}tQIWO_Q6M_DEVICE;

typedef struct{    
    BOOLEAN is_scaning;
    tQIWO_Q6M_DATA freq;
    tQIWO_Q6M_DATA read;
    tQIWO_Q6M_DATA reprot;
    tQIWO_Q6M_DEVICE device;
}tQIWO_Q6M;

extern void app_qiwo_cmd_parser(UINT8* p_data, UINT16 data_len);

#endif
