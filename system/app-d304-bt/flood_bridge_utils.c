/*****************************************************************************
**
**  Name:           flood_bridge_utils.c
**
**  Description:    Bluetooth BLE mesh bridge application
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ctype.h"
#include "flood_bridge_utils.h"

#include "app_utils.h"


byte ADV_MANU_DATA = (byte)0xFF;

// Broadcom BLE vendor ID in adv packets: 0x000F
byte ADV_VENDOR_ID_BCM[] = { 0x0F, 0x00 };
int ADV_VENDOR_ID_BCM_INT = 0x000F;

// Broadcom flood mesh device ID in adv packets: 0x0302
byte ADV_DEVICE_ID_FLOOD_MESH[] = { 0x02, 0x03 };

// Broadcom flood mesh device discovery adv types:
byte ADV_DEVICE_TYPE_MESH = 0x00;
byte ADV_DEVICE_TYPE_PROXY = 0x01;

char * str2upper(char * s)
{
    int i = 0;
    for (i = 0; s[i] != 0; i++)
    {
        s[i] = toupper(s[i]);
    }

    return s;
}

BOOLEAN is_valid_bdaddr(BD_ADDR address)
{
    BD_ADDR invalid_address = {0, 0, 0, 0, 0, 0};

    if (0 == memcmp(address, invalid_address, BD_ADDR_LEN))
    {
        return FALSE;
    }
    else 
    {
        return TRUE;
    }
}

boolean isMeshAdv(byte * scanRec, int length)
{
    int i, advLen;
    for (i = 0; i < length - 1; i += advLen + 1) {
        advLen = scanRec[i] & 0x0ff;
        if (advLen >= 6 &&
            (scanRec[i + 1] == ADV_MANU_DATA) &&
            (scanRec[i + 2] == ADV_VENDOR_ID_BCM[0]) &&
            (scanRec[i + 3] == ADV_VENDOR_ID_BCM[1]) &&
            (scanRec[i + 4] == ADV_DEVICE_ID_FLOOD_MESH[0]) &&
            (scanRec[i + 5] == ADV_DEVICE_ID_FLOOD_MESH[1]) &&
            (scanRec[i + 6] == ADV_DEVICE_TYPE_PROXY))
        {
            // It is a mesh device.
            APP_INFO0("Match Flood Mesh Adv\n");
            return true;
        }

        if (advLen == 0)  
            break;
    }
    return false;
}

void utils_lib_init()
{
    
}

int get_random_int(int max)
{
    int a;
    time_t t;
    time(&t);
    srand((unsigned int)t);
    a = rand() % max;
    return a;
}

int bachk(const char *str)
{
	if (!str)
		return -1;

	if (strlen(str) != 17)
		return -1;

	while (*str) {
		if (!isxdigit(*str++))
			return -1;

		if (!isxdigit(*str++))
			return -1;

		if (*str == 0)
			break;

		if (*str++ != ':')
			return -1;
	}

	return 0;
}

int ba2str(const BD_ADDR ba, char *str)
{
	return sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
		ba[5], ba[4], ba[3], ba[2], ba[1], ba[0]);
}

int str2ba(const char *str, BD_ADDR ba)
{
	int i;

	if (bachk(str) < 0) {
		memset(ba, 0, sizeof(BD_ADDR));
		return -1;
	}

	for (i = 5; i >= 0; i--, str += 3)
		ba[5-i] = strtol(str, NULL, 16);

	return 0;
}



const char * get_bt_address_string(BD_ADDR bd, char * out)
{
    static char tmp[20];
    sprintf(tmp, "%02x:%02x:%02x:%02x:%02x:%02x", bd[0] & 0x0ff, bd[1] & 0x0ff, bd[2] & 0x0ff
                                                , bd[3] & 0x0ff, bd[4] & 0x0ff, bd[5] & 0x0ff);
    str2upper(tmp);

    if (out)
    {
        strcpy(out, tmp);
    }
    return tmp;
}

int   bd_equal(const byte *a, const byte *b)
{
    return (memcmp(a, b, 6) == 0);
}

UINT8 * bd_cpy(byte *  dest, const byte * src)
{
    memcpy((void *)dest, (void *)src, BD_ADDR_LEN);
    return dest;
}


