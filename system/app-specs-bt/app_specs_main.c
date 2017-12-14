/*****************************************************************************
**
**  Name:           app_manager_main.c
**
**  Description:    Bluetooth Manager menu application
**
**  Copyright (c) 2010-2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/prctl.h>

#include "gki.h"
#include "uipc.h"

#include "bsa_api.h"

#include "app_xml_utils.h"

#include "app_disc.h"
#include "app_utils.h"
#include "app_dm.h"

#include "app_services.h"
#include "app_mgt.h"

#include "app_manager.h"
#include "app_ble_server.h"
#include "app_ble_client_db.h"
#include "app_ble_client.h"
#include "app_opc.h"
#include "app_ops.h"

#define APP_TEST_FILE_PATH "./test_file.txt"

#define APP_SPECS_BLE_UUID 0x9999
#define APP_SPECS_BLE_HANDLE_NUM    4 //(1 service x 2 + 5 characteristics x 2)

#define APP_SPECS_BLE_SERVICE_UUID  0x180a
#define APP_SPECS_BLE_CHAR_UUID 0x9999
#define APP_SPECS_BLE_CHAR_DESC_UUID 0x2902

#define APP_BLE_ADV_VALUE_LEN             6  /*This is temporary value, Total Adv data including all fields should be <31bytes*/

/* Menu items */
enum
{
    APP_MGR_MENU_ABORT_DISC         = 1,
    APP_MGR_MENU_DISCOVERY,
    APP_MGR_MENU_DISCOVERY_TEST,
    APP_MGR_MENU_BOND,
    APP_MGR_MENU_BOND_CANCEL,
    APP_MGR_MENU_UNPAIR,
    APP_MGR_MENU_SVC_DISCOVERY,
    APP_MGR_MENU_DI_DISCOVERY,
    APP_MGR_MENU_STOP_BT,
    APP_MGR_MENU_START_BT,
    APP_MGR_MENU_SP_ACCEPT,
    APP_MGR_MENU_SP_REFUSE,
    APP_MGR_MENU_ENTER_BLE_PASSKEY,
    APP_MGR_MENU_ACT_AS_KB,
    APP_MGR_MENU_READ_CONFIG,
    APP_MGR_MENU_READ_LOCAL_OOB,
    APP_MGR_MENU_ENTER_REMOTE_OOB,
    APP_MGR_MENU_SET_VISIBILTITY,
#if (defined(BLE_INCLUDED) && BLE_INCLUDED == TRUE)
    APP_MGR_MENU_SET_BLE_VISIBILTITY,
#endif
    APP_MGR_MENU_SET_AFH_CHANNELS,
    APP_MGR_MENU_CLASS2,
    APP_MGR_MENU_CLASS1_5,
    APP_MGR_MENU_DUAL_STACK,
    APP_MGR_MENU_LINK_POLICY,
    APP_MGR_MENU_ENTER_PIN_CODE,
    APP_MGR_MENU_REMOTE_NAME,
    APP_MGR_MENU_MONITOR_RSSI,
    APP_MGR_MENU_KILL_SERVER        = 96,
    APP_MGR_MENU_MGR_INIT           = 97,
    APP_MGR_MENU_MGR_CLOSE          = 98,
    APP_MGR_MENU_QUIT               = 99
};

/*
 * Extern variables
 */
extern BD_ADDR                 app_sec_db_addr;    /* BdAddr of peer device requesting SP */
extern tAPP_MGR_CB app_mgr_cb;
static tBSA_SEC_IO_CAP g_sp_caps = 0;
extern tAPP_XML_CONFIG         app_xml_config;

BOOLEAN app_mgr_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data);

int app_specs_ble_server_create_service(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_CREATE ble_create_param;
    UINT16 service;
    UINT16  num_handle;
    BOOLEAN  is_primary;
    int server_num, attr_num;

    server_num = 0;
    if ((server_num < 0) || (server_num >= BSA_BLE_SERVER_MAX))
    {
        APP_ERROR1("Wrong server number! = %d", server_num);
        return -1;
    }
    if (app_ble_cb.ble_server[server_num].enabled != TRUE)
    {
        APP_ERROR1("Server was not enabled! = %d", server_num);
        return -1;
    }

    service = APP_SPECS_BLE_SERVICE_UUID;
    if (!service)
    {
        APP_ERROR1("wrong value = %d", service);
        return -1;
    }
  
    num_handle = APP_SPECS_BLE_HANDLE_NUM;
    if (!num_handle)
    {
        APP_ERROR1("wrong value = %d", num_handle);
        return -1;
    }
    is_primary = 1;
    if (!(is_primary == 0) && !(is_primary == 1))
    {
        APP_ERROR1("wrong value = %d", is_primary);
        return -1;
    }
    attr_num = app_ble_server_find_free_attr(server_num);
    if (attr_num < 0)
    {
        APP_ERROR1("Wrong attr number! = %d", attr_num);
        return -1;
    }
    status = BSA_BleSeCreateServiceInit(&ble_create_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeCreateServiceInit failed status = %d", status);
        return -1;
    }

    ble_create_param.service_uuid.uu.uuid16 = service;
    ble_create_param.service_uuid.len = 2;
    ble_create_param.server_if = app_ble_cb.ble_server[server_num].server_if;
    ble_create_param.num_handle = num_handle;
    if (is_primary != 0)
    {
        ble_create_param.is_primary = TRUE;
    }
    else
    {
        ble_create_param.is_primary = FALSE;
    }

    app_ble_cb.ble_server[server_num].attr[attr_num].wait_flag = TRUE;

    status = BSA_BleSeCreateService(&ble_create_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeCreateService failed status = %d", status);
        app_ble_cb.ble_server[server_num].attr[attr_num].wait_flag = FALSE;
        return -1;
    }

    /* store information on control block */
    app_ble_cb.ble_server[server_num].attr[attr_num].attr_UUID.len = 2;
    app_ble_cb.ble_server[server_num].attr[attr_num].attr_UUID.uu.uuid16 = service;
    app_ble_cb.ble_server[server_num].attr[attr_num].is_pri = ble_create_param.is_primary;
    app_ble_cb.ble_server[server_num].attr[attr_num].attr_type = BSA_GATTC_ATTR_TYPE_SRVC;
    return 0;
}

/*******************************************************************************
 **
 ** Function        app_specs_ble_server_add_char
 **
 ** Description     Add character to specs service
 **
 ** Parameters      char_uuid: characteristic uuid
 **                 attribute_permission: Read-0x1, Write-0x10, Read|Write-0x11
 **                 characteristic_property: WRITE-0x08, READ-0x02, Notify-0x10, Indicate-0x20
 **		    is_descript: desc-1, not desc-0
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_specs_ble_server_add_char(UINT16 char_uuid, int attribute_permission, int characteristic_property, int is_descript)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_ADDCHAR ble_addchar_param;
    int server_num, srvc_attr_num, char_attr_num;

    server_num = 0;
    if ((server_num < 0) || (server_num >= BSA_BLE_SERVER_MAX))
    {
        APP_ERROR1("Wrong server number! = %d", server_num);
        return -1;
    }
    if (app_ble_cb.ble_server[server_num].enabled != TRUE)
    {
        APP_ERROR1("Server was not enabled! = %d", server_num);
        return -1;
    }

    srvc_attr_num = 0;
    char_attr_num = app_ble_server_find_free_attr(server_num);

    status = BSA_BleSeAddCharInit(&ble_addchar_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeAddCharInit failed status = %d", status);
        return -1;
    }
    ble_addchar_param.service_id = app_ble_cb.ble_server[server_num].attr[srvc_attr_num].service_id;
    ble_addchar_param.char_uuid.len = 2;
    ble_addchar_param.char_uuid.uu.uuid16 = char_uuid;

    if(is_descript)
    {
        ble_addchar_param.is_descr = TRUE;
        ble_addchar_param.perm = attribute_permission;
    }
    else
    {
        ble_addchar_param.is_descr = FALSE;
        ble_addchar_param.perm = attribute_permission;
        ble_addchar_param.property = characteristic_property;
    }

    status = BSA_BleSeAddChar(&ble_addchar_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeAddChar failed status = %d", status);
        return -1;
    }

    /* save all information */
    app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_UUID.len = 2;
    app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_UUID.uu.uuid16 = char_uuid;
    app_ble_cb.ble_server[server_num].attr[char_attr_num].prop = characteristic_property;
    app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    app_ble_cb.ble_server[server_num].attr[char_attr_num].wait_flag = TRUE;

    return 0;

}

int app_specs_ble_server_start_service(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_START ble_start_param;
    int num, attr_num;

    num = 0;
    if ((num < 0) || (num >= BSA_BLE_SERVER_MAX))
    {
        APP_ERROR1("Wrong server number! = %d", num);
        return -1;
    }
    if (app_ble_cb.ble_server[num].enabled != TRUE)
    {
        APP_ERROR1("Server was not enabled! = %d", num);
        return -1;
    }

    attr_num = 0;
    if(attr_num < 0)
    {
        APP_ERROR0("app_ble_server_start_service : Invalid attr_num entered");
        return -1;
    }

    status = BSA_BleSeStartServiceInit(&ble_start_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeStartServiceInit failed status = %d", status);
        return -1;
    }

    ble_start_param.service_id = app_ble_cb.ble_server[num].attr[attr_num].service_id;
    ble_start_param.sup_transport = BSA_BLE_GATT_TRANSPORT_LE_BR_EDR;

    APP_INFO1("service_id:%d, num:%d", ble_start_param.service_id, num);

    status = BSA_BleSeStartService(&ble_start_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeStartService failed status = %d", status);
        return -1;
    }
    return 0;
}

int app_specs_ble_server_send_indication(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_SENDIND ble_sendind_param;
    int num, length_of_data, index, attr_num;

    num = 0;
    if ((num < 0) || (num >= BSA_BLE_SERVER_MAX))
    {
        APP_ERROR1("Wrong server number! = %d", num);
        return -1;
    }
    if (app_ble_cb.ble_server[num].enabled != TRUE)
    {
        APP_ERROR1("Server was not enabled! = %d", num);
        return -1;
    }

    attr_num = 1;

    status = BSA_BleSeSendIndInit(&ble_sendind_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeSendIndInit failed status = %d", status);
        return -1;
    }

    ble_sendind_param.conn_id = app_ble_cb.ble_server[num].conn_id;
    ble_sendind_param.attr_id = app_ble_cb.ble_server[num].attr[attr_num].attr_id;

    length_of_data = 1;
    ble_sendind_param.data_len = length_of_data;

    for (index = 0; index < length_of_data ; index++)
    {
        ble_sendind_param.value[index] = 0xab;
    }

    ble_sendind_param.need_confirm = FALSE;

    status = BSA_BleSeSendInd(&ble_sendind_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeSendInd failed status = %d", status);
        return -1;
    }

    return 0;
}

void app_specs_ble_server_profile_cback(tBSA_BLE_EVT event, tBSA_BLE_MSG *p_data)
{
    int num, attr_index;
    int status;
    tBSA_BLE_SE_SENDRSP send_server_resp;
    UINT8 attribute_value[BSA_BLE_MAX_ATTR_LEN]={0x11,0x22,0x33,0x44};

    APP_DEBUG1("app_specs_ble_server_profile_cback event = %d ", event);

    switch (event)
    {
    case BSA_BLE_SE_DEREGISTER_EVT:
        APP_INFO1("BSA_BLE_SE_DEREGISTER_EVT server_if:%d status:%d", 
            p_data->ser_deregister.server_if, p_data->ser_deregister.status);
        num = app_ble_server_find_index_by_interface(p_data->ser_deregister.server_if);
        if(num < 0)
        {
            APP_ERROR0("no deregister pending!!");
            break;
        }

        app_ble_cb.ble_server[num].server_if = BSA_BLE_INVALID_IF;
        app_ble_cb.ble_server[num].enabled = FALSE;
        for (attr_index = 0 ; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
        {
            memset(&app_ble_cb.ble_server[num].attr[attr_index], 0, sizeof(tAPP_BLE_ATTRIBUTE));
        }

        break;

    case BSA_BLE_SE_CREATE_EVT:
        APP_INFO1("BSA_BLE_SE_CREATE_EVT server_if:%d status:%d service_id:%d",
            p_data->ser_create.server_if, p_data->ser_create.status, p_data->ser_create.service_id);

        num = app_ble_server_find_index_by_interface(p_data->ser_create.server_if);

        /* search interface number */
        if(num < 0)
        {
            APP_ERROR0("no interface!!");
            break;
        }

        /* search attribute number */
        for (attr_index = 0 ; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
        {
            if (app_ble_cb.ble_server[num].attr[attr_index].wait_flag == TRUE)
            {
                APP_INFO1("BSA_BLE_SE_CREATE_EVT if_num:%d, attr_num:%d", num, attr_index);
                if (p_data->ser_create.status == BSA_SUCCESS)
                {
                    app_ble_cb.ble_server[num].attr[attr_index].service_id = p_data->ser_create.service_id;
                    app_ble_cb.ble_server[num].attr[attr_index].wait_flag = FALSE;
                    break;
                }
                else  /* if CREATE fail */
                {
                    memset(&app_ble_cb.ble_server[num].attr[attr_index], 0, sizeof(tAPP_BLE_ATTRIBUTE));
                    break;
                }
            }
        }
        if (attr_index >= BSA_BLE_ATTRIBUTE_MAX)
        {
            APP_ERROR0("BSA_BLE_SE_CREATE_EVT no waiting!!");
            break;
        }
        break;

    case BSA_BLE_SE_ADDCHAR_EVT:
        APP_INFO1("BSA_BLE_SE_ADDCHAR_EVT status:%d", p_data->ser_addchar.status);
        APP_INFO1("attr_id:0x%x", p_data->ser_addchar.attr_id);
        num = app_ble_server_find_index_by_interface(p_data->ser_addchar.server_if);

        /* search interface number */
        if(num < 0)
        {
            APP_ERROR0("no interface!!");
            break;
        }

        for (attr_index = 0 ; attr_index < BSA_BLE_ATTRIBUTE_MAX ; attr_index++)
        {
            if (app_ble_cb.ble_server[num].attr[attr_index].wait_flag == TRUE)
            {
                APP_INFO1("if_num:%d, attr_num:%d", num, attr_index);
                if (p_data->ser_addchar.status == BSA_SUCCESS)
                {
                    app_ble_cb.ble_server[num].attr[attr_index].service_id = p_data->ser_addchar.service_id;
                    app_ble_cb.ble_server[num].attr[attr_index].attr_id = p_data->ser_addchar.attr_id;
                    app_ble_cb.ble_server[num].attr[attr_index].wait_flag = FALSE;
                    break;
                }
                else  /* if CREATE fail */
                {
                    memset(&app_ble_cb.ble_server[num].attr[attr_index], 0, sizeof(tAPP_BLE_ATTRIBUTE));
                    break;
                }
            }
        }
        if (attr_index >= BSA_BLE_ATTRIBUTE_MAX)
        {
            APP_ERROR0("BSA_BLE_SE_ADDCHAR_EVT no waiting!!");
            break;
        }
        break;

    case BSA_BLE_SE_START_EVT:
        APP_INFO1("BSA_BLE_SE_START_EVT status:%d", p_data->ser_start.status);
        break;

    case BSA_BLE_SE_READ_EVT:
        APP_INFO1("BSA_BLE_SE_READ_EVT status:%d", p_data->ser_read.status);
        BSA_BleSeSendRspInit(&send_server_resp);
        send_server_resp.conn_id = p_data->ser_read.conn_id;
        send_server_resp.trans_id = p_data->ser_read.trans_id;
        send_server_resp.status = p_data->ser_read.status;
        send_server_resp.handle = p_data->ser_read.handle;
        send_server_resp.offset = p_data->ser_read.offset;
        send_server_resp.len = 4;
        send_server_resp.auth_req = GATT_AUTH_REQ_NONE;
        memcpy(send_server_resp.value, attribute_value, BSA_BLE_MAX_ATTR_LEN);
        APP_INFO1("BSA_BLE_SE_READ_EVT: send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
        APP_INFO1("BSA_BLE_SE_READ_EVT: send_server_resp.status:%d,send_server_resp.auth_req:%d", send_server_resp.status,send_server_resp.auth_req);
        APP_INFO1("BSA_BLE_SE_READ_EVT: send_server_resp.handle:%d, send_server_resp.offset:%d, send_server_resp.len:%d", send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
        BSA_BleSeSendRsp(&send_server_resp);

        break;

    case BSA_BLE_SE_WRITE_EVT:
        APP_INFO1("BSA_BLE_SE_WRITE_EVT status:%d", p_data->ser_write.status);
        APP_DUMP("Write value", p_data->ser_write.value, p_data->ser_write.len);
        APP_INFO1("BSA_BLE_SE_WRITE_EVT trans_id:%d, conn_id:%d, handle:%d, is_prep:%d, offset:%d",
            p_data->ser_write.trans_id, p_data->ser_write.conn_id, p_data->ser_write.handle, 
            p_data->ser_write.is_prep, p_data->ser_write.offset);

        if (p_data->ser_write.need_rsp)
        {
            BSA_BleSeSendRspInit(&send_server_resp);
            send_server_resp.conn_id = p_data->ser_write.conn_id;
            send_server_resp.trans_id = p_data->ser_write.trans_id;
            send_server_resp.status = p_data->ser_write.status;
            send_server_resp.handle = p_data->ser_write.handle;
            if(p_data->ser_write.is_prep)
            {
                send_server_resp.offset = p_data->ser_write.offset;
                send_server_resp.len = p_data->ser_write.len;
                memcpy(send_server_resp.value, p_data->ser_write.value, send_server_resp.len);
            }
            else
                send_server_resp.len = 0;
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.status:%d,send_server_resp.auth_req:%d", send_server_resp.status,send_server_resp.auth_req);
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.handle:%d, send_server_resp.offset:%d, send_server_resp.len:%d", send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
            BSA_BleSeSendRsp(&send_server_resp);
        }

	if (p_data->ser_write.value[0] == 1 || p_data->ser_write.value[1] == 2)
		app_specs_ble_server_send_indication();
        break;

    case BSA_BLE_SE_EXEC_WRITE_EVT:
        APP_INFO1("BSA_BLE_SE_EXEC_WRITE_EVT status:%d", p_data->ser_exec_write.status);
        APP_INFO1("BSA_BLE_SE_WRITE_EVT trans_id:%d, conn_id:%d, exec_write:%d",
            p_data->ser_exec_write.trans_id, p_data->ser_exec_write.conn_id, p_data->ser_exec_write.exec_write);

        BSA_BleSeSendRspInit(&send_server_resp);
        send_server_resp.conn_id = p_data->ser_exec_write.conn_id;
        send_server_resp.trans_id = p_data->ser_exec_write.trans_id;
        send_server_resp.status = p_data->ser_exec_write.status;
        send_server_resp.handle = 0;
        send_server_resp.len = 0;
        APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
        APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.status:%d", send_server_resp.status);
        BSA_BleSeSendRsp(&send_server_resp);

        break;

    case BSA_BLE_SE_OPEN_EVT:
        APP_INFO1("BSA_BLE_SE_OPEN_EVT status:%d", p_data->ser_open.reason);
        if (p_data->ser_open.reason == BSA_SUCCESS)
        {
            APP_INFO1("conn_id:0x%x", p_data->ser_open.conn_id);
            num = app_ble_server_find_index_by_interface(p_data->ser_open.server_if);
            /* search interface number */
            if(num < 0)
            {
                APP_ERROR0("no interface!!");
                break;
            }
            app_ble_cb.ble_server[num].conn_id = p_data->ser_open.conn_id;

            /* XML Database update */
            app_read_xml_remote_devices();
            /* Add BLE service for this devices in XML database */
            app_xml_add_trusted_services_db(app_xml_remote_devices_db,
                    APP_NUM_ELEMENTS(app_xml_remote_devices_db), p_data->ser_open.remote_bda,
                    BSA_BLE_SERVICE_MASK);

            status = app_write_xml_remote_devices();
            if (status < 0)
            {
                APP_ERROR1("app_ble_write_remote_devices failed: %d", status);
            }
        }
        break;

    case BSA_BLE_SE_CLOSE_EVT:
        APP_INFO1("BSA_BLE_SE_CLOSE_EVT status:%d", p_data->ser_close.reason);
        APP_INFO1("conn_id:0x%x", p_data->ser_close.conn_id);
        num = app_ble_server_find_index_by_interface(p_data->ser_close.server_if);
        /* search interface number */
        if(num < 0)
        {
            APP_ERROR0("no interface!!");
            break;
        }
        app_ble_cb.ble_server[num].conn_id = BSA_BLE_INVALID_CONN;
        break;

    case BSA_BLE_SE_CONF_EVT:
        APP_INFO1("BSA_BLE_SE_CONF_EVT status:%d", p_data->ser_conf.status);
        APP_INFO1("conn_id:0x%x", p_data->ser_conf.conn_id);
        break;


    default:
        break;
    }

}

int app_specs_ble_server_init(void)
{
    tBSA_DM_BLE_ADV_CONFIG adv_conf, scan_rsp;
    UINT8 app_ble_adv_value[APP_BLE_ADV_VALUE_LEN] = {0x77, 0x11, 0xaa, 0xbb, 0xcc, 0xdd}; /*First 2 byte is Company Identifier Eg: 0x1a2b refers to Bluetooth ORG, followed by 4bytes of data*/

    //app_dm_set_visibility(TRUE, TRUE);
    app_dm_set_ble_visibility(TRUE, TRUE);

    //app_ble_server_register(APP_SPECS_BLE_UUID, NULL);
    app_ble_server_register(APP_SPECS_BLE_UUID, app_specs_ble_server_profile_cback);

    app_specs_ble_server_create_service();
    GKI_delay(50);
    app_specs_ble_server_add_char(APP_SPECS_BLE_CHAR_UUID, 0x11, 0x3a, 0);
    app_specs_ble_server_add_char(APP_SPECS_BLE_CHAR_DESC_UUID, 0x11, 0x3a, 1);
    GKI_delay(50);

    app_specs_ble_server_start_service();
    GKI_delay(500);
    
    /* start advertising */
    memset(&adv_conf, 0, sizeof(tBSA_DM_BLE_ADV_CONFIG));
    adv_conf.len = APP_BLE_ADV_VALUE_LEN;
    adv_conf.flag = BSA_DM_BLE_ADV_FLAG_MASK;
    memcpy(adv_conf.p_val, app_ble_adv_value, APP_BLE_ADV_VALUE_LEN);
    //adv_conf.adv_data_mask = BSA_DM_BLE_AD_BIT_FLAGS|BSA_DM_BLE_AD_BIT_MANU;
    adv_conf.adv_data_mask = BSA_DM_BLE_AD_BIT_FLAGS|BSA_DM_BLE_AD_BIT_SERVICE|BSA_DM_BLE_AD_BIT_APPEARANCE|BSA_DM_BLE_AD_BIT_MANU;
    adv_conf.is_scan_rsp = FALSE;
    app_dm_set_ble_adv_data(&adv_conf);

    return 0;
}

int app_specs_ble_server_disconnect(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_CLOSE ble_close_param;
    int server_num;

    server_num = 0;

    if((server_num < 0) ||
       (server_num >= BSA_BLE_SERVER_MAX) ||
       (app_ble_cb.ble_server[server_num].enabled == FALSE))
    {
        APP_ERROR1("Wrong server number! = %d", server_num);
        return -1;
    }
    status = BSA_BleSeCloseInit(&ble_close_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeCloseInit failed status = %d", status);
        return -1;
    }
    ble_close_param.conn_id = app_ble_cb.ble_server[server_num].conn_id;
    status = BSA_BleSeClose(&ble_close_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeClose failed status = %d", status);
        return -1;
    }

    return 0;
}

void app_specs_ble_server_exit(void)
{   
    app_specs_ble_server_disconnect();

    GKI_delay(200);
    app_ble_server_deregister();
    app_dm_set_ble_visibility(FALSE, FALSE); 
    //app_dm_set_visibility(FALSE, TRUE);
}

tBSA_STATUS app_specs_opc_push_file(char * p_file_name)
{
    tBSA_STATUS status = 0;
    int device_index;
    tBSA_OPC_PUSH param;

    printf("app_opc_push_file\n");
    status = BSA_OpcPushInit(&param);

    /* Devices from XML database */
    /* Read the Remote device XML file to have a fresh view */
    app_read_xml_remote_devices();
    device_index = 0;
    if ((device_index >= APP_NUM_ELEMENTS(app_xml_remote_devices_db)) ||
		    (device_index < 0))
    {
	    printf("Wrong Remote device index\n");
	    return BSA_SUCCESS;
    }

    if (app_xml_remote_devices_db[device_index].in_use == FALSE)
    {
	    printf("Wrong Remote device index\n");
	    return BSA_SUCCESS;
    }
    bdcpy(param.bd_addr, app_xml_remote_devices_db[device_index].bd_addr);

    param.format = BTA_OP_OTHER_FMT;
    strncpy(param.send_path, p_file_name, sizeof(param.send_path) - 1);
    param.send_path[BSA_OP_OBJECT_NAME_LEN_MAX - 1] = 0;

    status = BSA_OpcPush(&param);

    if (status != BSA_SUCCESS)
    {
	    fprintf(stderr, "app_opc_push_file: Error:%d\n", status);
    }
    return status;
}

void app_specs_clean_task(int sig)
{
	APP_DEBUG0("app_specs is killed, clean the task");

	app_specs_ble_server_exit();

	app_ble_exit();

	app_stop_ops();

	app_opc_stop();

	app_mgt_close();

	APP_DEBUG0("app_specs is killed, exit");

	exit(0);
}

static void specs_bt_sigsegv_handle(int sig)
{
	char name[32];

	if (sig == SIGSEGV) {
		prctl(PR_GET_NAME, name);
		printf("specs bt Segmentation Fault %s \n", name);
		exit(1);
	}

	return;
}

/*******************************************************************************
 **
 ** Function         main
 **
 ** Description      This is the main function
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
int main(int argc, char **argv)
{
    int choice;
    int i;
    unsigned int afh_ch1,afh_ch2,passkey;
    BOOLEAN no_init = FALSE;
    int mode;
    BOOLEAN discoverable, connectable;
    tBSA_SEC_PIN_CODE_REPLY pin_code_reply;

    BD_ADDR bd_addr;
    int uarr[16];
    char szInput[64];
    int  x = 0;
    int length = 0;
    tBSA_DM_LP_MASK policy_mask = 0;
    BOOLEAN set = FALSE;
    int i_set = 0;
    int status;

    signal(SIGSEGV, specs_bt_sigsegv_handle);

    /* Check the CLI parameters */
    for (i = 1; i < argc; i++)
    {
        char *arg = argv[i];
        if (*arg != '-')
        {
            APP_ERROR1("Unsupported parameter #%d : %s", i+1, argv[i]);
            exit(-1);
        }
        /* Bypass the first '-' char */
        arg++;
        /* check if the second char is also a '-'char */
        if (*arg == '-') arg++;
        switch (*arg)
        {
        case 'n': /* No init */
            no_init = TRUE;
            break;
        default:
            break;
        }
    }


    /* Init App manager */
    if (!no_init)
    {
        app_mgt_init();
        if (app_mgt_open("/tmp/", app_mgr_mgt_callback) < 0)
        {
            APP_ERROR0("Unable to connect to server");
            return -1;
        }

        /* Init XML state machine */
        app_xml_init();

        if (app_mgr_config())
        {
            APP_ERROR0("Couldn't configure successfully, exiting");
            return -1;
        }
        g_sp_caps = app_xml_config.io_cap;
    }

    /* Display FW versions */
    app_mgr_read_version();

    /* Get the current Stack mode */
    mode = app_dm_get_dual_stack_mode();
    if (mode < 0)
    {
        APP_ERROR0("app_dm_get_dual_stack_mode failed");
        return -1;
    }
    else
    {
        /* Save the current DualStack mode */
        app_mgr_cb.dual_stack_mode = mode;
        APP_INFO1("Current DualStack mode:%s", app_mgr_get_dual_stack_mode_desc());
    }

    /* Example of function to start OPC application */
    status = app_opc_start();
    if (status != BSA_SUCCESS)
    {
        fprintf(stderr, "main: Unable to start OPC\n");
        app_mgt_close();
        return status;
    }

    app_ops_auto_accept();

    /* Example of function to start OPS application */
    status = app_start_ops();
    if (status != BSA_SUCCESS)
    {
        printf("main: Unable to start OPS\n");
        app_mgt_close();
        return status;
    }

    status = app_ble_init();
    if (status < 0)
    {
        APP_ERROR0("Couldn't Initialize BLE app, exiting");
        exit(-1);
    }

    status = app_ble_start();
    if (status < 0)
    {
        APP_ERROR0("Couldn't Start BLE app, exiting");
        return -1;
    }

    app_specs_ble_server_init();

    app_read_xml_remote_devices();
    int device_index = 0;
    while (1) {
	    if ((device_index < APP_NUM_ELEMENTS(app_xml_remote_devices_db)) &&
			    app_xml_remote_devices_db[device_index].in_use == TRUE)
	    {
		    printf("True Remote device index\n");
		    break;
	    }

	    printf("Wrong Remote device index\n");
	    GKI_delay(1000);
    }

    app_specs_opc_push_file(APP_TEST_FILE_PATH);

    signal(SIGINT, app_specs_clean_task);
    do
    {
	    GKI_delay(100);
    } while (1);      /* While user don't exit application */


    app_specs_ble_server_exit();

    app_ble_exit();

    app_stop_ops();

    app_opc_stop();

    app_mgt_close();

    return 0;
}
