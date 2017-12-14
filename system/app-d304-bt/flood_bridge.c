/*****************************************************************************
**
**  Name:           flood_bridge.c
**
**  Description:    Bluetooth BLE mesh bridge application
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include <string.h>
#include <stdio.h>

#include "app_ble.h"
#include "app_utils.h"
#include "app_disc.h"
#include "app_mgt.h"
#include "app_dm.h"
#include "app_ble_client.h"


#include "flood_bridge.h"
#include "bridge_info.h"
#include "udp_task.h"
#include "flood_bridge_utils.h"

extern tQIWO_Q6M g_qiwo_q6m;

pthread_cond_t  bleCond =  PTHREAD_COND_INITIALIZER;//init cond   
pthread_mutex_t bleMutex = PTHREAD_MUTEX_INITIALIZER; 

const UINT8 FLOOD_MESH_UUID_SERVICE[16] = {0x27, 0x52, 0xd1, 0x9a, 0x4b, 0x94, 0xf8, 0x9b, 0x0c, 0x4e, 0xb5, 0x20, 0x49, 0x20, 0xc4, 0x36};

// {CE3253F3-9397-4D02-8EEB-13A64A80D0E6}
const UINT8 FLOOD_MESH_UUID_CMD[16]     = {0xe6, 0xd0, 0x80, 0x4a, 0xa6, 0x13, 0xeb, 0x8e, 0x02, 0x4d, 0x97, 0x93, 0xf3, 0x53, 0x32, 0xce};

// {24B119D6-7D42-4392-9748-A66B0375AD7A}
const UINT8 FLOOD_MESH_UUID_DATA[16]    = {0x7a, 0xad, 0x75, 0x03, 0x6b, 0xa6, 0x48, 0x97, 0x92, 0x43, 0x42, 0x73, 0xd6, 0x19, 0xb1, 0x24};

// {8AC32D3F-5CB9-4D44-BEC2-EE689169F626}
//const UINT8 FLOOD_MESH_UUID_NOTIFY[16]  = {0x26, 0xf6, 0x69, 0x91, 0x68, 0xee, 0xc2, 0xbe, 0x44, 0x4d, 0xb9, 0x5c, 0x3f, 0x2d, 0xc3, 0x8a};
const UINT8 FLOOD_MESH_UUID_NOTIFY[16]  = {0xe6, 0xd0, 0x80, 0x4a, 0xa6, 0x13, 0xeb, 0x8e, 0x02, 0x4d, 0x97, 0x93, 0xf3, 0x53, 0x32, 0xce};



/******************************************************
 *               Function Definitions
 ******************************************************/
void flood_bridge_scan_result_callback(tBSA_DISC_EVT event, tBSA_DISC_MSG *p_data)
{
	APP_INFO1("flood_bridge_scan_result_callback =>+++ event %d \n", event);

	switch (event)
	{
		case BSA_DISC_NEW_EVT:
		{
			int i = 0;
			APPL_TRACE_DEBUG6("app_ble_discover_callback, BDA:0x%02X%02X%02X%02X%02X%02X",
						p_data->disc_new.bd_addr[0],p_data->disc_new.bd_addr[1],
						p_data->disc_new.bd_addr[2],p_data->disc_new.bd_addr[3],
						p_data->disc_new.bd_addr[4],p_data->disc_new.bd_addr[5]);

			APPL_TRACE_DEBUG6("app_ble_discover_callback, EIR: 0x%02X%02X%02X%02X%02X%02X",
						p_data->disc_new.eir_data[0],p_data->disc_new.eir_data[1],
						p_data->disc_new.eir_data[2],p_data->disc_new.eir_data[3],
						p_data->disc_new.eir_data[4],p_data->disc_new.eir_data[5]);


		    flood_bridge_scan_callback(p_data->disc_new.bd_addr, p_data->disc_new.rssi, p_data->disc_new.eir_data);

		}
		break;

		case BSA_DISC_CMPL_EVT:
		{
			int result = flood_bridge_scan_complete_callback();

			pBridge_info->is_scaning = FALSE;

			APP_INFO1("on Scan callback return : %d\n", result);
		    if ( -1 == result)
		    {
				APP_INFO0("Singal Scan \n");
		        pthread_cond_signal(&bleCond); 
		    }
		
		}
		break;
	}

	APP_INFO1("flood_bridge_scan_result_callback =>--- event %d \n", event);
}



void flood_bridge_scan_callback(BD_ADDR address, INT8 rssi, UINT8 *p_adv_data)
{
    int i;
    
    if (is_flood_mesh_nodes(BLE_MESH_NODE, address) && isMeshAdv(p_adv_data, 31))
    {
        if (pBridge_info->max_rssi < rssi)
        {
            pBridge_info->max_rssi = rssi;
            memcpy(pBridge_info->max_rssi_addr, address, BD_ADDR_LEN);
        }
    }
    else if(is_flood_mesh_nodes(BLE_SENSOR_NODE, address))
    {
        for(i = 0; i < QIWO_SENSOR_NUM; i++)
        {
            if (!pBridge_info->sensor_connected[i] && !is_valid_bdaddr(pBridge_info->sensor_address[i]))
            {
                memcpy(pBridge_info->sensor_address[i], address, BD_ADDR_LEN);
                //app_qiwo_ble_connect(i + 1, address);
            }
        }
    }
}

int flood_bridge_scan_complete_callback()
{
    int i;
    
    if ((!pBridge_info->proxy_connected) && (pBridge_info->max_rssi > MIN_RSSI) && (is_valid_bdaddr(pBridge_info->max_rssi_addr)))
    {
         memcpy(pBridge_info->proxy_address, pBridge_info->max_rssi_addr, BD_ADDR_LEN);

         memset(pBridge_info->max_rssi_addr, 0, BD_ADDR_LEN);
         pBridge_info->max_rssi = MIN_RSSI;
         pBridge_info->proxy_connected = FALSE;

         flood_bridge_create_connect(BLE_MESH_NODE, pBridge_info->proxy_address);

		 APP_INFO1("Connected to mesh node: 0x%02x%02x%02x%02x%02x%02x\n", pBridge_info->proxy_address[5], pBridge_info->proxy_address[4],
             pBridge_info->proxy_address[3], pBridge_info->proxy_address[2], pBridge_info->proxy_address[1], pBridge_info->proxy_address[0]);
         
         return 0;
    }
    else
    {
        for(i = 0; i < QIWO_SENSOR_NUM; i++)
        {
            if (!pBridge_info->sensor_connected[i] && is_valid_bdaddr(pBridge_info->sensor_address[i]))
            {
                app_qiwo_ble_connect(pBridge_info->sensor_address[i], FALSE);
        		 APP_INFO1("Connected to sensor node: 0x%02x%02x%02x%02x%02x%02x\n", pBridge_info->proxy_address[5], pBridge_info->proxy_address[4],
                     pBridge_info->proxy_address[3], pBridge_info->proxy_address[2], pBridge_info->proxy_address[1], pBridge_info->proxy_address[0]);
                return 0;
            }
        }        
    }
    
    return -1;
}

void flood_bridge_handle_on_conn_up(UINT8 addr_type, BD_ADDR connected_addr, UINT16 conn_id)
{
    int i, index;
    
	APP_INFO1("connecting Proxy Address : 0x%02x%02x%02x%02x%02x%02x\n", pBridge_info->proxy_address[5], pBridge_info->proxy_address[4],
             pBridge_info->proxy_address[3], pBridge_info->proxy_address[2], pBridge_info->proxy_address[1], pBridge_info->proxy_address[0]);

	APP_INFO1("Connected Address : 0x%02x%02x%02x%02x%02x%02x\n", connected_addr[5], connected_addr[4],
             connected_addr[3], connected_addr[2], connected_addr[1], connected_addr[0]);
	
    if (0 == memcmp(connected_addr, pBridge_info->proxy_address, BD_ADDR_LEN))
    {
        pBridge_info->proxy_connected = TRUE;
        pBridge_info->proxy_conn_id = conn_id;
        
        index = get_flood_mesh_nodes_index(connected_addr);
        pBridge_info->proxy_node_id = pBridge_info->devices[index].node_id;
            
        flood_bridge_client_register_notification(app_ble_client_find_index_by_conn_id(conn_id));
        
    }
    else
    {
        for(i = 0; i < QIWO_SENSOR_NUM; i++)
        {
            if (0 == memcmp(connected_addr, pBridge_info->sensor_address[i], BD_ADDR_LEN))
            {
                pBridge_info->sensor_connected[i] = TRUE;
                pBridge_info->sensor_conn_id[i] = conn_id;

                index = get_flood_mesh_nodes_index(connected_addr);
                pBridge_info->sensor_node_id[i] = pBridge_info->devices[index].node_id;
                
                qiwo_client_register_sensor_notification(app_ble_client_find_index_by_conn_id(conn_id));
            }
        }
    }  
    
    GKI_delay(100);
    for(i = 0; i < QIWO_SENSOR_NUM; i++)
    {
        if (!pBridge_info->sensor_connected[i] && is_valid_bdaddr(pBridge_info->sensor_address[i]))
        {
            app_qiwo_ble_connect(pBridge_info->sensor_address[i], FALSE);
            APP_INFO1("flood_bridge_handle_on_conn_up Connected to sensor node: 0x%02x%02x%02x%02x%02x%02x\n", pBridge_info->proxy_address[5], pBridge_info->proxy_address[4],
             pBridge_info->proxy_address[3], pBridge_info->proxy_address[2], pBridge_info->proxy_address[1], pBridge_info->proxy_address[0]);            
            return;
        }
    }

    flood_bridge_search_nodes();
}

 
void flood_bridge_handle_on_conn_down(UINT8 addr_type, BD_ADDR disconnected_addr, UINT16 conn_id)
{
    int i;
    
	APP_INFO1("connected Proxy Address : 0x%02x%02x%02x%02x%02x%02x\n", pBridge_info->proxy_address[5], pBridge_info->proxy_address[4],
             pBridge_info->proxy_address[3], pBridge_info->proxy_address[2], pBridge_info->proxy_address[1], pBridge_info->proxy_address[0]);

	APP_INFO1("Disconnected Address : 0x%02x%02x%02x%02x%02x%02x\n", disconnected_addr[5], disconnected_addr[4],
             disconnected_addr[3], disconnected_addr[2], disconnected_addr[1], disconnected_addr[0]);
	
    if (0 == memcmp(disconnected_addr, pBridge_info->proxy_address, BD_ADDR_LEN))
    {
        pBridge_info->proxy_connected = FALSE;;
        pBridge_info->proxy_conn_id = 0;
        memset(pBridge_info->proxy_address, 0, BD_ADDR_LEN);

        flood_bridge_search_nodes();
    }
    else
    {
        for(i = 0; i < QIWO_SENSOR_NUM; i++)
        {
            if (0 == memcmp(disconnected_addr, pBridge_info->sensor_address[i], BD_ADDR_LEN))
            {
                pBridge_info->sensor_connected[i] = FALSE;;
                pBridge_info->sensor_conn_id[i] = 0;
                memset(pBridge_info->sensor_address[i], 0, BD_ADDR_LEN); 
            }
        }

        flood_bridge_search_nodes();
    }
}



void flood_bridge_handle_notification_data(UINT16 conn_id, tBTA_GATT_ID char_id, UINT8 *data, UINT8 len)
{
    int i;
    
	if (conn_id == pBridge_info->proxy_conn_id)
	{
		if ((char_id.uuid.len == 16) && (0 == memcmp(char_id.uuid.uu.uuid128,  FLOOD_MESH_UUID_NOTIFY, 16)))
		{
		    bridge_notify_ble_forward(data, len);
		    APP_INFO1("Forwared notification data to host: len %d\n", len);
		}
	}
    else
    {

        for(i = 0; i < QIWO_SENSOR_NUM; i++)
        {
             if ((conn_id == pBridge_info->sensor_conn_id[i])
     		  && (char_id.uuid.len == 16) 
     		  && (0 == memcmp(char_id.uuid.uu.uuid128,  FLOOD_MESH_UUID_NOTIFY, 16)))
    		{
    		    bridge_notify_ble_forward(data, len);
    		    APP_INFO1("Forwared sensor notification data to host: len %d\n", len);
    		}  
        }
    }
}


/*******************************************************************************
 **
 ** Function        app_ble_client_register_notification
 **
 ** Description     This is the register function to receive a notification
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int flood_bridge_client_register_notification(int client_num)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_NOTIFREG ble_notireg_param;
    UINT16 service_id, character_id;

    int uuid;
    int ser_inst_id, char_inst_id, is_primary;

    if (!pBridge_info->proxy_connected)
    {
        return -1;
    }

    if((client_num < 0) ||
       (client_num >= BSA_BLE_CLIENT_MAX) ||
       (app_ble_cb.ble_client[client_num].enabled == FALSE))
    {
        APP_ERROR1("Wrong client number! = %d", client_num);
        return -1;
    }
    status = BSA_BleClNotifRegisterInit(&ble_notireg_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClNotifRegisterInit failed status = %d", status);
        return -1;
    }
    
    ble_notireg_param.notification_id.srvc_id.id.uuid.len = 16;
    memcpy(ble_notireg_param.notification_id.srvc_id.id.uuid.uu.uuid128,  FLOOD_MESH_UUID_SERVICE, 16);
    
    ble_notireg_param.notification_id.char_id.uuid.len = 16;
    memcpy(ble_notireg_param.notification_id.char_id.uuid.uu.uuid128,  FLOOD_MESH_UUID_NOTIFY, 16);
    ble_notireg_param.notification_id.srvc_id.id.inst_id = 0;
    ble_notireg_param.notification_id.srvc_id.is_primary = 1;
    ble_notireg_param.notification_id.char_id.inst_id = 0;
    bdcpy(ble_notireg_param.bd_addr, app_ble_cb.ble_client[client_num].server_addr);
    ble_notireg_param.client_if = app_ble_cb.ble_client[client_num].client_if;

    APP_DEBUG1("size of ble_notireg_param:%d", sizeof(ble_notireg_param));
    status = BSA_BleClNotifRegister(&ble_notireg_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClNotifRegister failed status = %d", status);
        return -1;
    }
    return 0;
}


void flood_bridge_start( )
{
    flood_bridge_init();

    flood_bridge_search_nodes();
}


void flood_brige_exit()
{
#ifdef ENABLE_UDP_BROADCAST_ADDR
	end_udp_task();
#endif
	end_tcp_task();
}


BOOLEAN is_need_scan(void)
{
    UINT16 i, connected_sensor_num=0;

    if(pBridge_info->is_scaning || g_qiwo_q6m.is_scaning)
    {
        APP_DEBUG0("not need scan -- is scaning");
        return FALSE;
    }

    if ((!pBridge_info->proxy_connected) && (pBridge_info->nodes_num > 0))
         return TRUE;

    for(i = 0; i < QIWO_SENSOR_NUM; i++)
    {
        if (pBridge_info->sensor_connected[i])
        {
            connected_sensor_num++;
        }
    }
    if(connected_sensor_num < pBridge_info->sensor_num)
        return TRUE;
    else
        return FALSE;
}

void flood_bridge_search_nodes()
{
    
    APP_INFO1("flood_bridge_search_nodes mesh_enabled:%d", pBridge_info->mesh_enabled);
	if (pBridge_info->mesh_enabled)
	{
		if (is_need_scan())
		{
		    if (0 == app_disc_start_ble_regular(flood_bridge_scan_result_callback))
		    {
				pBridge_info->is_scaning = TRUE;
		    }
		}
        else
            APP_INFO0("flood_bridge_search_nodes not need scan");
	}
}

/*******************************************************************************
 **
 ** Function        flood_bridge_create_connect
 **
 ** Description     This is the ble open connection to ble server
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
void flood_bridge_create_connect(UINT8 addr_type, BD_ADDR address)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_OPEN ble_open_param;
    int device_index;
    int client_num = 0;
    int direct;

	if (!pBridge_info->mesh_enabled)
	{
		return;
	}

    APP_INFO0("flood_bridge_create_connect:");

    for(client_num = 0; client_num < BSA_BLE_CLIENT_MAX; client_num++)
    {
        if (app_ble_cb.ble_client[client_num].conn_id != BSA_BLE_INVALID_CONN)
            break;
    }
    
    if((client_num < 0) ||
       (client_num >= BSA_BLE_CLIENT_MAX) ||
       (app_ble_cb.ble_client[client_num].enabled == FALSE))
    {
        APP_ERROR1("Wrong client number! = %d", client_num);
        return -1;
    }
/*
    if (app_ble_cb.ble_client[client_num].conn_id != BSA_BLE_INVALID_CONN)
    {
        APP_ERROR1("Connection already exist, conn_id = %d", app_ble_cb.ble_client[client_num].conn_id );
        return -1;
    }*/

    status = BSA_BleClConnectInit(&ble_open_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClConnectInit failed status = %d", status);
        return -1;
    }

    ble_open_param.client_if = app_ble_cb.ble_client[client_num].client_if;
    
    ble_open_param.is_direct = TRUE;
    
    bdcpy(app_ble_cb.ble_client[client_num].server_addr, address);
    bdcpy(ble_open_param.bd_addr, address);

    status = BSA_BleClConnect(&ble_open_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClConnect failed status = %d", status);
        return -1;
    }

    return 0;

}


/*******************************************************************************
 **
 ** Function        blemesh_bridge_close_conn
 **
 ** Description     This is the ble close connection
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int blemesh_bridge_close_conn(UINT16 conn_id)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_CLOSE ble_close_param;
    int client_num = 0;

    client_num = app_ble_client_find_index_by_conn_id(conn_id);
    if((client_num < 0) ||
       (client_num >= BSA_BLE_CLIENT_MAX) ||
       (app_ble_cb.ble_client[client_num].enabled == FALSE))
    {
        APP_ERROR1("Wrong client number! = %d", client_num);
        return -1;
    }
    status = BSA_BleClCloseInit(&ble_close_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClCLoseInit failed status = %d", status);
        return -1;
    }
    ble_close_param.conn_id = conn_id;
    status = BSA_BleClClose(&ble_close_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClClose failed status = %d", status);
        return -1;
    }

    return 0;
}



/*******************************************************************************
 **
 ** Function        app_ble_client_write
 **
 ** Description     This is the write function to write a characteristic or characteristic discriptor to BLE server.
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int flood_bridge_write_data(UINT16 conn_id, UINT8 *data, UINT8 len)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_WRITE ble_write_param;
    UINT16 char_id, index, service, descr_id;
    int client_num = 0, is_descript;
    int ser_inst_id, char_inst_id, is_primary;
    UINT8 desc_value;
    UINT8 write_type = 0;

    APP_DUMP("flood_bridge_write_data:", data, len);
    
	if (!pBridge_info->proxy_connected || (conn_id != pBridge_info->proxy_conn_id))
	{
		return -1;
	}
    
    if((client_num < 0) ||
       (client_num >= BSA_BLE_CLIENT_MAX) ||
       (app_ble_cb.ble_client[client_num].enabled == FALSE))
    {
        APP_ERROR1("Wrong client number! = %d", client_num);
        return -1;
    }
    status = BSA_BleClWriteInit(&ble_write_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClWriteInit failed status = %d", status);
        return -1;
    }

    //service = app_get_choice("Enter Service UUID to write(eg. x180d)");
    is_primary = 1;
    ser_inst_id = 0;
    //char_id = app_get_choice("Enter Char UUID to write(eg. x2902)");
    char_inst_id = 0;

    
    memcpy(ble_write_param.value, data, len);
    ble_write_param.len = len;
    ble_write_param.descr = FALSE;
    ble_write_param.char_id.srvc_id.id.inst_id= ser_inst_id;
    memcpy(ble_write_param.char_id.srvc_id.id.uuid.uu.uuid128, FLOOD_MESH_UUID_SERVICE, 16);
    ble_write_param.char_id.srvc_id.id.uuid.len = 16;
    ble_write_param.char_id.srvc_id.is_primary = is_primary;

    ble_write_param.char_id.char_id.inst_id = char_inst_id;
    memcpy(ble_write_param.char_id.char_id.uuid.uu.uuid128, FLOOD_MESH_UUID_CMD, 16);
    ble_write_param.char_id.char_id.uuid.len = 16;

    ble_write_param.conn_id = app_ble_cb.ble_client[client_num].conn_id;
    ble_write_param.auth_req = BTA_GATT_AUTH_REQ_NONE;
    write_type= BTA_GATTC_TYPE_WRITE;
    
    if (write_type == BTA_GATTC_TYPE_WRITE_NO_RSP)
    {
        ble_write_param.write_type = BTA_GATTC_TYPE_WRITE_NO_RSP;
    }
    else if (write_type == BTA_GATTC_TYPE_WRITE)
    {
        ble_write_param.write_type = BTA_GATTC_TYPE_WRITE;
    }
    else
    {
        APP_ERROR1("BSA_BleClWrite failed wrong write_type:%d", write_type);
        return -1;
    }

    status = BSA_BleClWrite(&ble_write_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClWrite failed status = %d", status);
        return -1;
    }
    return 0;
}



void blemesh_bridge_switch(BOOLEAN enable_mesh)
{
    UINT8 i;
    
	if (enable_mesh)
	{
		if (!pBridge_info->mesh_enabled)
		{
			pBridge_info->mesh_enabled = TRUE;
			//if ((!pBridge_info->proxy_connected) && (!pBridge_info->is_scaning))
			{
				flood_bridge_search_nodes();
			}
		}
	}
	else
	{
		if (pBridge_info->proxy_connected)
		{
			blemesh_bridge_close_conn(pBridge_info->proxy_conn_id);
		}
        for(i = 0; i < QIWO_SENSOR_NUM; i++)
        {
            if(pBridge_info->sensor_connected[i])
                app_qiwo_ble_close_conn(i+1, pBridge_info->sensor_conn_id[i]);
        }
        
        GKI_delay(200);
        
		pBridge_info->mesh_enabled = FALSE;
        
	}
}


