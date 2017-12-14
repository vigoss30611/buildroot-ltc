/*****************************************************************************
 **
 **  Name:           app_qiwo_ag.c
 **
 **  Description:    Bluetooth AG application
 **
 **  Copyright (c) 2010-2014, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#include <unistd.h>
#include <fcntl.h>

#include "app_ag.h"
#include "app_utils.h"
#include "app_xml_utils.h"
#include "app_disc.h"
#include "app_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#ifdef PCM_ALSA
#include "alsa/asoundlib.h"
#endif

#include "app_qiwo.h"

#if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
extern BOOLEAN g_qiwo_isCommandRsp;
#endif

tAPP_QIWO_AG_SCAN_RSP ag_scan_rsp;

const key_t k_msgKey = 0x1014a19;
static int s_msgId;
tAPP_QIWO_AG_CONN g_qiwo_ag_conn;
void setBtdevState(bool connected)
{
    struct msg_btdev_state_t state;
    int sendlength;

//    printf("setBtdevState(%d)\n", connected);

    state.mtype = 1;
    state.connected = connected;
    sendlength = sizeof(struct msg_btdev_state_t) - sizeof(long);
    
    if (msgsnd(s_msgId, &state, sendlength, 0/*msgflg*/) == -1) {
        perror("msgsnd() failed");
    }
}

int msgOperateForBtState(bool connected)
{
//    perror("msgOperateForBtState()\n");
    s_msgId = msgget(k_msgKey, 0666 | IPC_CREAT);
    if (s_msgId == -1) 
    {
        perror("msgget() failed");
        return -1;
    }
 
    setBtdevState(connected);
    usleep(3000 * 1000);
 
    /*!< destroy the msg */
//    msgctl(s_msgId, IPC_RMID, NULL);

    return 0;
} 
//zhifeng
#define BT_SND_FILE  "/tmp/ble_pcm_file" 
extern int saveBleData(char* bleFile, char* data);

#if 0
//Big-endian
void qiwo_u16_2_u8(UINT16 u16_data, UINT8* p_u8_data)
{
    *p_u8_data = (UINT8)(u16_data >> 8);
    *++p_u8_data = (UINT8)(u16_data & 0xff);
}
#else
//Small-endian for Qiwo 304
void qiwo_u16_2_u8(UINT16 u16_data, UINT8* p_u8_data)
{
    *p_u8_data = (UINT8)(u16_data & 0xff);
    *++p_u8_data = (UINT8)(u16_data >> 8);
}
#endif

void qiwo_u8_2_u16(UINT8* p_u8_data, UINT16* p_u16_data)
{
    *p_u16_data = ((UINT16)*p_u8_data << 8) + (UINT16)*++p_u8_data;
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_send_scan_rsp
 **
 ** Description      send ag scan result to phone
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_qiwo_ag_send_scan_rsp(void)
{
    UINT8 device_index;
    UINT8 data[QIWO_RSP_DATA_MAX_LEN];
    UINT16 data_len, name_len;
    UINT8* pos;

    APP_DEBUG0(" ");
    
    memset(data, 0, QIWO_RSP_DATA_MAX_LEN);
    pos = data;
    
    ag_scan_rsp.num = 0;
    for(device_index = 0; device_index < APP_DISC_NB_DEVICES; device_index++)
    {
        if(app_discovery_cb.devs[device_index].in_use != FALSE)
        {
            memcpy(&ag_scan_rsp.device[ag_scan_rsp.num], &app_discovery_cb.devs[device_index].device, sizeof(tBSA_DISC_REMOTE_DEV));
            ag_scan_rsp.num++;
        }
    }
    //APP_DEBUG1("scan device num : %d", ag_scan_rsp.num);
    
    data[0] = AG_SCAN_CMD;
    //qiwo_u16_2_u8((UINT16)(ag_scan_rsp.num * 6 + 1), data + 1);
    data[3] = ag_scan_rsp.num;
    pos += 4;
    for(device_index = 0; device_index < ag_scan_rsp.num; device_index++)
    {
        memcpy(pos, ag_scan_rsp.device[device_index].bd_addr, BD_ADDR_LEN);
        pos += BD_ADDR_LEN;
        name_len = strlen(ag_scan_rsp.device[device_index].name);
        *pos = name_len;
        pos++;
        memcpy(pos, ag_scan_rsp.device[device_index].name, name_len);
        pos += name_len;
    }

    data_len = pos - data;
    qiwo_u16_2_u8(data_len - 3, data + 1);

    app_qiwo_socket_send_rsp(data, data_len);
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_disc_cback
 **
 ** Description      Discovery callback
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_qiwo_ag_disc_cback(tBSA_DISC_EVT event, tBSA_DISC_MSG *p_data)
{
    int index;

    switch (event)
    {
    
    case BSA_DISC_CMPL_EVT: /* Discovery complete. */
        APP_INFO0("app_qiwo_ag_disc_cback Discovery complete");
        app_qiwo_ag_send_scan_rsp();      
        break;


    default:
        break;
    }
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_scan
 **
 ** Description      Start Device Class Of Device discovery
 **
 ** Returns          int
 **
 *******************************************************************************/
void app_qiwo_ag_scan(void)
{
    /* Look for audio Devices (Headset/HandsFree) */
    app_disc_start_cod(BTM_COD_SERVICE_AUDIO, BTM_COD_MAJOR_AUDIO, 0, app_qiwo_ag_disc_cback);
    //app_disc_start_regular(app_qiwo_ag_disc_cback);
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_connect
 **
 ** Description      
 **
 ** Parameters       br_addr: address 
 **
 ** Returns          int
 **
 *******************************************************************************/
int app_qiwo_ag_connect(BD_ADDR bd_addr)
{
    tBSA_STATUS status = BSA_SUCCESS;
    tBSA_AG_OPEN param;
   
    if(app_ag_cb.opened == TRUE)
    {
        status = BSA_ERROR_CLI_BUSY;
        //app_qiwo_ag_send_connect_rsp(status);

        g_qiwo_ag_conn.is_need_disconnect = TRUE;
        bdcpy(g_qiwo_ag_conn.conn_addr, bd_addr);
        app_ag_close_audio(); 
        return status;
    } 

#if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
    g_qiwo_isCommandRsp = TRUE;
#endif

    BSA_AgOpenInit(&param);

    bdcpy(param.bd_addr, bd_addr);
    param.hndl = app_ag_cb.hndl[0];
    status = BSA_AgOpen(&param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_AgOpen failed (%d)", status);
        app_qiwo_ag_send_connect_rsp(status);
        return status;
    }
    return status;    
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_send_connect_rsp
 **
 ** Description      send ag connect result to phone
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_qiwo_ag_send_connect_rsp(tBSA_STATUS status)
{
    UINT8 data[64];   
 //zhifeng
    if(status == BSA_SUCCESS)
    {
	//saveBleData(BT_SND_FILE, "is connect");
	msgOperateForBtState(TRUE);
    }
    else
    {
	//saveBleData(BT_SND_FILE, "is not connect");  
 	msgOperateForBtState(FALSE);
    }
	
#if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
    if(!g_qiwo_isCommandRsp)
    {
        APP_DEBUG0("app_qiwo_ag_send_connect_rsp don't need response");
        return;
    }
    else
    {
        g_qiwo_isCommandRsp = FALSE;
    }
#endif
    
    data[0] = AG_CONNECT_CMD;
    qiwo_u16_2_u8(1, data + 1);
    data[3] = (status == BSA_SUCCESS) ? 1 : 0;
	memcpy(&data[4], g_qiwo_ag_conn.curr_conn_addr, sizeof(BD_ADDR));

    app_qiwo_socket_send_rsp(data, 4 + sizeof(BD_ADDR));
}

void app_qiwo_ag_save_connect_addr_for_unpair(BD_ADDR bd_addr)
{
    bdcpy(g_qiwo_ag_conn.curr_conn_addr, bd_addr);
    return;
}

int app_qiwo_ag_unpair(BD_ADDR bd_addr)
{
    int status;
    int dev_index;
    tBSA_SEC_REMOVE_DEV sec_remove;
    tAPP_XML_REM_DEVICE *p_xml_dev;
    
    APP_INFO0("app_qiwo_ag_unpair");

    /* Remove the device from Security database (BSA Server) */
    BSA_SecRemoveDeviceInit(&sec_remove);

    /* Read the XML file which contains all the bonded devices */
    app_read_xml_remote_devices();
    for (dev_index = 0 ; dev_index < APP_NUM_ELEMENTS(app_xml_remote_devices_db) ; dev_index++)
    {
        p_xml_dev = &app_xml_remote_devices_db[dev_index];
        if ((p_xml_dev->in_use) && (bdcmp(p_xml_dev->bd_addr, bd_addr) == 0))
        {
            bdcpy(sec_remove.bd_addr, p_xml_dev->bd_addr);
            status = BSA_SecRemoveDevice(&sec_remove);        

            if (status != BSA_SUCCESS)
            {
                APP_ERROR1("BSA_SecRemoveDevice Operation Failed with status [%d]",status);

                return -1;
            }
            else
            {
                /* delete the device from database */
                p_xml_dev->in_use = FALSE;
                app_write_xml_remote_devices();
            }  
        }
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_disconnect
 **
 ** Description      
 **
 ** Parameters       br_addr: address 
 **
 ** Returns          int
 **
 *******************************************************************************/
int app_qiwo_ag_disconnect(BD_ADDR bd_addr)
{
#if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
    g_qiwo_isCommandRsp = TRUE;
#endif

    app_ag_close_audio(); 
    return 0;    
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_send_disconnect_rsp
 **
 ** Description      send ag disconnect result to phone
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_qiwo_ag_send_disconnect_rsp(tBSA_STATUS status)
{
    UINT8 data[4];   

    if(g_qiwo_ag_conn.is_need_disconnect)
    {
    #if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
        g_qiwo_isCommandRsp = TRUE;
    #endif
        
        app_qiwo_ag_unpair(g_qiwo_ag_conn.curr_conn_addr);

        GKI_delay(1200);

        g_qiwo_ag_conn.is_need_disconnect = FALSE;
        app_qiwo_ag_connect(g_qiwo_ag_conn.conn_addr);
        return;
    }
    app_qiwo_ag_unpair(g_qiwo_ag_conn.curr_conn_addr);

 //zhifeng
    if(status == BSA_SUCCESS)
    {
	//saveBleData(BT_SND_FILE, "is not connect");
        msgOperateForBtState(FALSE);
    }
    else
    {
	//saveBleData(BT_SND_FILE, "is not connect");
        msgOperateForBtState(FALSE);
    }
	
#if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
    if(!g_qiwo_isCommandRsp)
    {
        APP_DEBUG0("app_qiwo_ag_send_connect_rsp don't need response");
        return;
    }
    else
    {
        g_qiwo_isCommandRsp = FALSE;
    }
#endif

    data[0] = AG_DISCONNECT_CMD;
    qiwo_u16_2_u8(1, data + 1);
    data[3] = (status == BSA_SUCCESS) ? 1 : 0;

   
    app_qiwo_socket_send_rsp(data, 4);
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_send_disconnect_rsp
 **
 ** Description      send ag disconnect result to phone
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_qiwo_ag_send_inquiry_rsp(BOOLEAN opened)
{
    UINT8 data[4];   
   
    data[0] = AG_INQUIRY_CMD;
    qiwo_u16_2_u8(1, data + 1);
    data[3] = (opened == TRUE) ? 1 : 0;
    
    app_qiwo_socket_send_rsp(data, 4);
}

/*******************************************************************************
 **
 ** Function         app_qiwo_ag_inquiry
 **
 ** Description      
 **
 ** Returns          int
 **
 *******************************************************************************/
int app_qiwo_ag_inquiry(void)
{
    app_qiwo_ag_send_inquiry_rsp(app_ag_cb.opened);
    return 0;    
}



