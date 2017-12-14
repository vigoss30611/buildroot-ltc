/*****************************************************************************
 **
 **  Name:			 udp_task.h
 **
 **  Description:	 Bluetooth BLE mesh bridge include file
 **
 **  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#ifndef __UDP_TASK_H__
#define __UDP_TASK_H__

#include "bt_types.h"
#include "trans_comm.h"


#ifdef ENABLE_UDP_BROADCAST_ADDR

#define FLOODMESH_UDP_PORT   7000
#define UDP_RECV_BUFFER_SIZE   512

#define BRIDGE_BRODCUST_SESSION 0x10203040

void start_upd_task();
void end_udp_task();

#endif
#endif
