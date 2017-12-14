/*****************************************************************************
**
**  Name:           bridge_info.h
**
**  Description:    Bluetooth BLE mesh bridge include file
**
**  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef __BRIDGE_INFO_H__
#define __BRIDGE_INFO_H__


#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "bt_types.h"
#include "app_qiwo.h"

// this the ATT handle, if don't want to discovery ATT handle
#define FLOOD_MESH_HANDLE_MESH_SERVICE_CHAR_CMD_VAL     ((uint16_t)0x2A)
#define FLOOD_MESH_HANDLE_MESH_SERVICE_CHAR_DATA_VAL    ((uint16_t)0x2C)
#define FLOOD_MESH_HANDLE_MESH_SERVICE_CHAR_NOTIFY_VAL  ((uint16_t)0x2E)

#define MAX_FLOOD_MESH_NODES           (255 + QIWO_SENSOR_NUM)


#define INVALID_NODE_ID           -1
#define MIN_RSSI                  -128


typedef struct {
    INT16    node_id;
    UINT8    addr_type;
    BD_ADDR    address;
}tFloodMeshNode;

typedef int (*conn_evt_handle)(UINT8 addr_type, BD_ADDR connected_addr);
typedef void (*scan_result_evt_handle)(BD_ADDR address, INT8 rssi, UINT8 *p_adv_data);

typedef struct{
	BOOLEAN             mesh_enabled;

    pthread_t           udp_server_task;
    int                 udpServ;     // udp server, local
    struct sockaddr     udpClient;   // udp client, remote
    int                 upd_thread_run;

	pthread_t           tcp_server_task;
	int                 tcpServ;
	int                 tcpChildFd;
	struct sockaddr     tcpClient;
	int                 tcp_thread_run;

	pthread_t           tutk_tcp_server_task;
	int                 tutk_tcpServ;
	int                 tutk_tcpChildFd;
	struct sockaddr     tutk_tcpClient;
	int                 tutk_tcp_thread_run;
    
    BOOLEAN             is_scaning;
    
    BD_ADDR             proxy_address;
    BOOLEAN             proxy_connected;
    UINT16              proxy_conn_id;
    UINT16              proxy_node_id;
    
    BD_ADDR             sensor_address[QIWO_SENSOR_NUM];
    BOOLEAN             sensor_connected[QIWO_SENSOR_NUM];
    UINT16              sensor_conn_id[QIWO_SENSOR_NUM];
    UINT16              sensor_node_id[QIWO_SENSOR_NUM];    
    UINT8               sensor_num;

    BD_ADDR             max_rssi_addr;
    INT8                max_rssi;

	int                 nodes_num;
    tFloodMeshNode     devices[MAX_FLOOD_MESH_NODES];  // all the devices node in network.s

}bridge_info_t, *bridge_info_p;

extern bridge_info_p pBridge_info;

void flood_bridge_init();

void flood_bridge_load_nodes_info();

void flood_bridge_store_nodes_info();

void flood_bridge_add_node(UINT8 addr_type, INT16 node_id, BD_ADDR address);

void flood_bridge_delete_node(BD_ADDR address);

void flood_brige_clear_all_nodes();

int flood_bridge_get_nodes_num();

BOOLEAN is_flood_mesh_nodes(UINT8 addr_type, BD_ADDR address);

int get_flood_mesh_nodes_index(BD_ADDR address);


#endif
