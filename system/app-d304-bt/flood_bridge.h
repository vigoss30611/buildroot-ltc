/*****************************************************************************
 **
 **  Name:			 flood_bridge.h
 **
 **  Description:	 Bluetooth BLE mesh bridge include file
 **
 **  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/
#ifndef __FLOOD_BRIDGE_H__
#define __FLOOD_BRIDGE_H__

#include "bt_types.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Security / Device Management
 ****************************************************************************/
extern pthread_cond_t  bleCond;//init cond   
extern pthread_mutex_t bleMutex; 

/*****************************************************************************
 * Globals
 *****************************************************************************/
 
void flood_bridge_start();

void flood_brige_exit();

void flood_bridge_scan_callback(BD_ADDR address, INT8 rssi, UINT8 *p_adv_data);

int flood_bridge_scan_complete_callback();

void flood_bridge_handle_on_conn_up(UINT8 addr_type, BD_ADDR connected_addr, UINT16 conn_id);

void flood_bridge_handle_on_conn_down(UINT8 addr_type, BD_ADDR disconnected_addr, UINT16 conn_id);

void flood_bridge_handle_notification_data(UINT16 conn_id, tBTA_GATT_ID char_id, UINT8 *data, UINT8 len);

void flood_bridge_search_nodes();

void flood_bridge_create_connect(UINT8 addr_type, BD_ADDR address);

int flood_bridge_write_data(UINT16 conn_id, UINT8 *data, UINT8 len);

int flood_bridge_client_register_notification(int clinet_num);

void flood_bridge_scan_result_callback(tBSA_DISC_EVT event, tBSA_DISC_MSG *p_data);

void blemesh_bridge_switch(BOOLEAN enable_mesh);

#ifdef __cplusplus
}
#endif

#endif
