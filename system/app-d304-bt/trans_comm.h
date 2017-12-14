
/*****************************************************************************
 **
 **  Name:			 trans_comm.h
 **
 **  Description:	 Bluetooth BLE mesh bridge include file
 **
 **  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/
 #ifndef __TRANS_COMM_H__
#define __TRANS_COMM_H__


#define UPDATE_MESH_DEVICE_INFO  1
#define BRIDGE_MESSAGE           2
#define BRIDGE_CLEAN_ONE_NODES_INFO  3
#define BRIDGE_CLEAN_NODES_INFO  4

#define BRIDGE_BROADCUST_INFO    5

#define BRIDGE_NOTIFY_NO_CONNECTION   1024
#define BRIDGE_NOTIFY_CMD_STATUS      1025

#define BRIDGE_NOTIFY_BLE_FORWARD     0x10

//handsfree
#define AG_SCAN_CMD        0xa1
#define AG_CONNECT_CMD     0xa2
#define AG_DISCONNECT_CMD  0xa3
#define AG_INQUIRY_CMD     0xa4

//switch to ble MESH cmd
#define RUN_BLE_MESH_CMD   0x10
#define RUN_BLE_SERVER_CMD 0x11

//Q6M
#define Q6M_SCAN_CONNECT_CMD  0xb1
#define Q6M_DISCONNECT_CMD    0xb3
#define Q6M_CHANGE_FREQ_CMD   0xb5
#define Q6M_UPLOAD_DATA_CMD   0xb6 //don't need command, only respond
#define Q6M_READ_DATA_CMD     0xb7

#define Q6M_SCAN_CONNECT_RESPOND  Q6M_SCAN_CONNECT_CMD
#define Q6M_DISCONNECT_RESPOND    Q6M_DISCONNECT_CMD
#define Q6M_CHANGE_FREQ_RESPOND   Q6M_CHANGE_FREQ_CMD
#define Q6M_UPLOAD_DATA_RESPOND   Q6M_UPLOAD_DATA_CMD
#define Q6M_READ_DATA_RESPOND     Q6M_READ_DATA_CMD

#pragma pack(push)
#pragma pack(1)


typedef struct {
    char bda[20];  // string
}update_device_payload_t;

typedef struct {
    int length;
    char data[64];  // string
}floodmesh_payload_t;

typedef struct {
    int CMD;
    int session;
}flood_bridge_cmd_header_t, *flood_bridge_cmd_header_p;

typedef struct {
    flood_bridge_cmd_header_t hdr;
    union 
    {
        update_device_payload_t update_dev_payload;
        floodmesh_payload_t     floodmesh_data;
    };
}flood_bridge_cmd_t, *flood_bridge_cmd_p;


#pragma pack(pop)


#endif
