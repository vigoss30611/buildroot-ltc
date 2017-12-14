/*****************************************************************************
**
**  Name:           app_qiwo_ble.h
**
**  Description:    Bluetooth BLE include file
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef APP_QIWO_BLE_H
#define APP_QIWO_BLE_H

#include "bsa_api.h"
#include "app_ble.h"

//qiwo ble command types
/*#define QIWO_BLE_AG_SCAN_CMD        1
#define QIWO_BLE_AG_CONNECT_CMD     2
#define QIWO_BLE_AG_DISCONNECT_CMD  3
#define QIWO_BLE_AG_INQUIRY_CMD     4*/
#define QIWO_BLE_WIFI_CMD           0xb0

#define APP_QIWO_BLE_RESP_SCAN_RSP_VALUE_LEN 6

typedef enum 
{
    QIWO_BLE_STATE_SERVER = 0, //connect to phone
    QIWO_BLE_STATE_MESH, //connect to mesh
}tAPP_QIWO_BLE_STATE;


/*******************************************************************************
 **
 ** Function         app_qiwo_ble_init
 **
 ** Description      qiwo ble service init
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
extern int app_qiwo_ble_init(void);

/*******************************************************************************
**
** Function         app_qiwo_ble_send_rsp
**
** Description      send ble respone(GATT notify) to phone
**
** Parameters
** 
** Returns          void
**
*******************************************************************************/
extern int app_qiwo_ble_send_rsp(UINT8* p_data, UINT16 data_len);

extern void q6m_init(void);

#endif