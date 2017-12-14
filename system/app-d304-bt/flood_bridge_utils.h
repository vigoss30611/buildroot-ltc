/*****************************************************************************
 **
 **  Name:			 flood_bridge_utils.h
 **
 **  Description:	 Bluetooth BLE mesh bridge include file
 **
 **  Copyright (c) 2015, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#ifndef __FLOOD_MESH_UTILS__H__
#define __FLOOD_MESH_UTILS__H__


#include "bt_types.h"

typedef unsigned char byte;


#ifndef boolean
typedef int           boolean;
#endif

#define false       0
#define true        1

int get_random_int(int max);

#define DEVICE_NAME_MAX_LEN 256
#define BT_ADDRESS_LEN      6

extern byte ADV_MANU_DATA;

// Broadcom BLE vendor ID in adv packets: 0x000F
extern byte ADV_VENDOR_ID_BCM[];
extern int ADV_VENDOR_ID_BCM_INT;

// Broadcom flood mesh device ID in adv packets: 0x0302
extern byte ADV_DEVICE_ID_FLOOD_MESH[];

// Broadcom flood mesh device discovery adv types:
extern byte ADV_DEVICE_TYPE_MESH;
extern byte ADV_DEVICE_TYPE_PROXY;

BOOLEAN is_valid_bdaddr(BD_ADDR address);

const char * get_bt_address_string(BD_ADDR bd, char * out);
int   bd_equal(const byte *a, const byte *b);
UINT8 * bd_cpy(byte *  dest, const byte * src);
boolean isMeshAdv(byte * scanRec, int length);

char * str2upper(char * s);

#endif
