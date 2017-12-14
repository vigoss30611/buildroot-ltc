/*****************************************************************************
 **
 **  Name:			 tcp_task.h
 **
 **  Description:	 Bluetooth BLE mesh bridge include file
 **
 **  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#ifndef __TCP_TASK_H__
#define __TCP_TASK_H__

#include "bt_types.h"
#include "trans_comm.h"

#define FLOODMESH_TCP_PORT     7000
#define TUTK_TCP_PORT          7001
#define TCP_RECV_BUFFER_SIZE   512

void start_tcp_task();
void end_tcp_task();

int bridge_notify_ble_forward(UINT8 *data, int length);

int app_qiwo_ble_send_rsp(UINT8* p_data, UINT16 data_len);

#endif
