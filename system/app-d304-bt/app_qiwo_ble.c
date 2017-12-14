#include <string.h>
#include "app_ble.h"
#include "app_xml_utils.h"
#include "app_utils.h"
#include "app_mgt.h"
#include "app_disc.h"
#include "app_dm.h"

#include "app_ble_server.h"
#include "app_ble_client_db.h"
#include "app_ble_client.h"

#include "app_qiwo.h"
#include "bridge_info.h"

extern tAPP_QIWO_BLE_STATE g_qiwo_ble_state;
extern int app_qiwo_ble_send_notify(UINT8* p_data, UINT16 data_len);

tQIWO_Q6M g_qiwo_q6m;
tQIWO_Q6M_DEVICE g_qiwo_q6m_scan_device;

#define APP_QIWO_BLE_UUID 0x9999
#define APP_QIWO_BLE_HANDLE_NUM    4 //(1 service x 2 + 5 characteristics x 2)

#define APP_QIWO_BLE_SERVICE_UUID  0x3600
#define APP_QIWO_BLE_CHAR_CMD_UUID 0x3601
#define APP_QIWO_BLE_CHAR_RSP_UUID 0x3602

typedef struct{
    UINT8  len;
    UINT8  data[QIWO_GATT_CHAR_LENGTH];
}tAPP_QIWO_PACKAGE;

typedef struct{
    UINT8  num;
    tAPP_QIWO_PACKAGE pack[QIWO_RSP_PACKAGE_MAX];
}tAPP_QIWO_PACKAGE_LIST;

static tAPP_QIWO_PACKAGE_LIST qiwo_rsp_pack_list;
UINT8 app_qiwo_ble_resp_scan_rsp_value[APP_QIWO_BLE_RESP_SCAN_RSP_VALUE_LEN] =
                   {0x77, 0x11, 0x22, 0x33, 0x44, 0x55};

tQIWO_SENSOR_LIST g_qiwo_sensor;

// {36C42049-20B5-4E0C-9BF8-944B9AD15227}
const UINT8 MFAS_UUID_SERVICE[16] = {0x27, 0x52, 0xd1, 0x9a, 0x4b, 0x94, 0xf8, 0x9b, 0x0c, 0x4e, 0xb5, 0x20, 0x49, 0x20, 0xc4, 0x36};

// {CE3253F3-9397-4D02-8EEB-13A64A80D0E6}
const UINT8 MFAS_UUID_CMD[16]     = {0xe6, 0xd0, 0x80, 0x4a, 0xa6, 0x13, 0xeb, 0x8e, 0x02, 0x4d, 0x97, 0x93, 0xf3, 0x53, 0x32, 0xce};

// {24B119D6-7D42-4392-9748-A66B0375AD7A}
const UINT8 MFAS_UUID_DATA[16]    = {0x7a, 0xad, 0x75, 0x03, 0x6b, 0xa6, 0x48, 0x97, 0x92, 0x43, 0x42, 0x73, 0xd6, 0x19, 0xb1, 0x24};

// {8AC32D3F-5CB9-4D44-BEC2-EE689169F626}
const UINT8 MFAS_UUID_NOTIFY[16]  = {0x26, 0xf6, 0x69, 0x91, 0x68, 0xee, 0xc2, 0xbe, 0x44, 0x4d, 0xb9, 0x5c, 0x3f, 0x2d, 0xc3, 0x8a};


//Q6M
UINT16 Q6M_UUID_SERVICE = 0x1892;
UINT16 Q6M_UUID_CHAR = 0x2A92;

int qiwo_sensor_write_data(UINT16 conn_id, UINT8 *data, UINT8 len)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_WRITE ble_write_param;
    UINT16 char_id, index, service, descr_id;
    int client_num = 0, is_descript;
    int ser_inst_id, char_inst_id, is_primary;
    UINT8 desc_value;
    UINT8 write_type = 0;

    APP_INFO0("qiwo_sensor_write_data\n");
    
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
    memcpy(ble_write_param.char_id.srvc_id.id.uuid.uu.uuid128, MFAS_UUID_SERVICE, 16);
    ble_write_param.char_id.srvc_id.id.uuid.len = 16;
    ble_write_param.char_id.srvc_id.is_primary = is_primary;

    ble_write_param.char_id.char_id.inst_id = char_inst_id;
    memcpy(ble_write_param.char_id.char_id.uuid.uu.uuid128, MFAS_UUID_CMD, 16);
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


void qiwo_client_register_sensor_notification(int client_num)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_NOTIFREG ble_notireg_param;
    UINT16 service_id, character_id;

    int  uuid;
    int ser_inst_id, char_inst_id, is_primary;

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
    memcpy(ble_notireg_param.notification_id.srvc_id.id.uuid.uu.uuid128,  MFAS_UUID_SERVICE, 16);
    
    ble_notireg_param.notification_id.char_id.uuid.len = 16;
    memcpy(ble_notireg_param.notification_id.char_id.uuid.uu.uuid128,  MFAS_UUID_NOTIFY, 16);
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

  
int app_qiwo_ble_connect(BD_ADDR bd_addr, BOOLEAN is_direct)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_OPEN ble_open_param;
    UINT8 i, client_num;
    
    APP_INFO0("app_qiwo_ble_connect");

    client_num = BSA_BLE_CLIENT_MAX;
    for(i = 0; i < BSA_BLE_CLIENT_MAX; i++)
    {
        if(app_ble_cb.ble_client[i].conn_id == BSA_BLE_INVALID_CONN)
        {
            client_num = i;
            break;
        }
    }
    
    if((client_num < 0) ||
       (client_num >= BSA_BLE_CLIENT_MAX) ||
       (app_ble_cb.ble_client[client_num].enabled == FALSE))
    {
        APP_ERROR1("Wrong client number! = %d", client_num);
        return -1;
    }

    if (app_ble_cb.ble_client[client_num].conn_id != BSA_BLE_INVALID_CONN)
    {
        APP_ERROR1("Connection already exist, conn_id = %d", app_ble_cb.ble_client[client_num].conn_id );
        return -1;
    }

    status = BSA_BleClConnectInit(&ble_open_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClConnectInit failed status = %d", status);
        return -1;
    }

    ble_open_param.client_if = app_ble_cb.ble_client[client_num].client_if;
    ble_open_param.is_direct = is_direct;

    bdcpy(app_ble_cb.ble_client[client_num].server_addr, bd_addr);
    bdcpy(ble_open_param.bd_addr, bd_addr);

    status = BSA_BleClConnect(&ble_open_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleClConnect failed status = %d", status);
        return -1;
    }

    return 0;    
}

int app_qiwo_ble_close_conn(int client_num, UINT16 conn_id)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_CLOSE ble_close_param;;

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

int app_qiwo_ble_client_unpair(int client_num)
{
    tBSA_STATUS status;
    tBSA_SEC_REMOVE_DEV sec_remove_dev;
    tBSA_BLE_CL_CLOSE ble_close_param;
    
    tAPP_BLE_CLIENT_DB_ELEMENT *p_blecl_db_elmt;

    APP_INFO0("app_qiwo_ble_client_unpair");

    if((client_num < 0) ||
       (client_num >= BSA_BLE_CLIENT_MAX) ||
       (app_ble_cb.ble_client[client_num].enabled == FALSE))
    {
        APP_ERROR1("Wrong client number! = %d", client_num);
        return -1;
    }

    /*First close any active connection if exist*/
    if(app_ble_cb.ble_client[client_num].conn_id != BSA_BLE_INVALID_CONN)
    {
        status = BSA_BleClCloseInit(&ble_close_param);
        if (status != BSA_SUCCESS)
        {
            APP_ERROR1("BSA_BleClCLoseInit failed status = %d", status);
            return -1;
        }
        ble_close_param.conn_id = app_ble_cb.ble_client[client_num].conn_id;
        status = BSA_BleClClose(&ble_close_param);
        if (status != BSA_SUCCESS)
        {
            APP_ERROR1("BSA_BleClClose failed status = %d", status);
            return -1;
        }
    }

    /* Remove the device from Security database (BSA Server) */
    BSA_SecRemoveDeviceInit(&sec_remove_dev);

    /* Read the XML file which contains all the bonded devices */
    app_read_xml_remote_devices();

    if ((client_num >= 0) &&
        (client_num < APP_MAX_NB_REMOTE_STORED_DEVICES) &&
        (app_xml_remote_devices_db[client_num].in_use != FALSE))
    {
        bdcpy(sec_remove_dev.bd_addr, app_xml_remote_devices_db[client_num].bd_addr);
        status = BSA_SecRemoveDevice(&sec_remove_dev);
    }
    else
    {
        APP_ERROR1("Wrong input [%d]",client_num);
        return -1;
    }

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_SecRemoveDevice Operation Failed with status [%d]",status);
        return -1;
    }
    else
    {
        /* delete the device from database */
        app_xml_remote_devices_db[client_num].in_use = FALSE;
        app_write_xml_remote_devices();
    }

    APP_INFO0("Remove device from BLE DB");
    APP_DEBUG1("BDA:%02X:%02X:%02X:%02X:%02X:%02X",
              app_xml_remote_devices_db[client_num].bd_addr[0], app_xml_remote_devices_db[client_num].bd_addr[1],
              app_xml_remote_devices_db[client_num].bd_addr[2], app_xml_remote_devices_db[client_num].bd_addr[3],
              app_xml_remote_devices_db[client_num].bd_addr[4], app_xml_remote_devices_db[client_num].bd_addr[5]);

    /* search BLE DB */
    p_blecl_db_elmt = app_ble_client_db_find_by_bda(app_xml_remote_devices_db[client_num].bd_addr);
    if(p_blecl_db_elmt != NULL)
    {
        APP_DEBUG0("Device present in DB, So clear it!!");
        app_ble_client_db_clear_by_bda(app_xml_remote_devices_db[client_num].bd_addr);
        app_ble_client_db_save();
    }
    else
    {
        APP_DEBUG0("No device present");
    }
    return 0;
}

/**************************************ble server*************************************/
/*******************************************************************************
**
** Function         package_rsp_data
**
** Description      package respone data to qiwo_rsp_pack_list
**
** Parameters
** 
** Returns          void
**
*******************************************************************************/
UINT8 package_rsp_data(UINT8* p_data, UINT16 data_len)
{
    int i = 0;
    UINT8 * p;
    tAPP_QIWO_PACKAGE* p_pack;
    UINT16 remains;
    UINT16 copy_len = 0;

    //init package list
    p = p_data;
    remains = data_len;
    qiwo_rsp_pack_list.num = 0;
    for(i = 0; i < QIWO_RSP_PACKAGE_MAX; i++)
        memset(qiwo_rsp_pack_list.pack[i].data, 0, QIWO_GATT_CHAR_LENGTH);  
    
    //send first package
    p_pack = &qiwo_rsp_pack_list.pack[0];
    memset(p_pack->data, 0, QIWO_GATT_CHAR_LENGTH);
    p_pack->data[0] = 0;  //package sn
    p_pack->data[1] = *p; //command type
    p_pack->data[2] = data_len - 1; //command data len
    p++;
    remains--;
    if(remains > (QIWO_GATT_CHAR_LENGTH - 3))
        copy_len = QIWO_GATT_CHAR_LENGTH - 3;
    else
        copy_len = remains;
    
    memcpy(p_pack->data + 3, p, copy_len);
    p_pack->len = copy_len + 3;
    remains -= copy_len;
    p += copy_len;
    qiwo_rsp_pack_list.num++;

    //send remain packages
    while (remains > 0)
    {
        p_pack = &qiwo_rsp_pack_list.pack[qiwo_rsp_pack_list.num];
        p_pack->data[0] = qiwo_rsp_pack_list.num;
        
        if(remains > (QIWO_GATT_CHAR_LENGTH - 1))
            copy_len = QIWO_GATT_CHAR_LENGTH - 1;
        else
            copy_len = remains;

        memcpy(p_pack->data + 1, p, copy_len);
        p_pack->len = copy_len + 1;
        
        qiwo_rsp_pack_list.num++;
        remains -= copy_len;
        p += copy_len;
    }

    return qiwo_rsp_pack_list.num;
}

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
int app_qiwo_ble_send_rsp(UINT8* p_data, UINT16 data_len)
{
    int i;
    
    if(p_data == NULL)
    {
        APP_ERROR0("no data!!");
        return -1;
    }

    if(data_len < 2 || data_len > QIWO_RSP_DATA_MAX_LEN)
    {
        APP_ERROR1("not correct respone length: %d!!", data_len);
        return -1;
    }

    APP_DUMP("respone data", p_data, data_len);
    package_rsp_data(p_data, data_len);   

    for(i = 0; i < qiwo_rsp_pack_list.num; i++)
    {
        APP_DUMP("notify", qiwo_rsp_pack_list.pack[i].data, qiwo_rsp_pack_list.pack[i].len);
        app_qiwo_ble_send_notify(qiwo_rsp_pack_list.pack[i].data, qiwo_rsp_pack_list.pack[i].len);
    }
    
    return 0;
}

/*******************************************************************************
**
** Function         app_qiwo_ble_send_notify
**
** Description      send ble notify to phone
**
** Parameters
** 
** Returns          status: 0 if success / -1 otherwise
**
*******************************************************************************/
int app_qiwo_ble_send_notify(UINT8* p_data, UINT16 data_len)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_SENDIND ble_sendind_param;
    int num, index, attr_num;

    num = 0;
    if (app_ble_cb.ble_server[num].enabled != TRUE)
    {
        APP_ERROR1("Server was not enabled! = %d", num);
        return -1;
    }

    attr_num = 1; //2;

    status = BSA_BleSeSendIndInit(&ble_sendind_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeSendIndInit failed status = %d", status);
        return -1;
    }

    ble_sendind_param.conn_id = app_ble_cb.ble_server[num].conn_id;
    ble_sendind_param.attr_id = app_ble_cb.ble_server[num].attr[attr_num].attr_id;

    ble_sendind_param.data_len = data_len;
    memcpy(ble_sendind_param.value, p_data, data_len);
    
    ble_sendind_param.need_confirm = FALSE;

    status = BSA_BleSeSendInd(&ble_sendind_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeSendInd failed status = %d", status);
        return -1;
    }

    return 0;    
}
    
/*******************************************************************************
**
** Function         app_qiwo_ble_server_profile_cback
**
** Description      BLE Server Profile callback.
**                  
** Returns          void
**
*******************************************************************************/
void app_qiwo_ble_server_profile_cback(tBSA_BLE_EVT event,  tBSA_BLE_MSG *p_data)
{
    int num, attr_index;
    int status;
    tBSA_BLE_SE_SENDRSP send_server_resp;
    UINT8 attribute_value[BSA_BLE_MAX_ATTR_LEN]={0x11,0x22,0x33,0x44};

    APP_DEBUG1("app_qiwo_ble_server_profile_cback event = %d ", event);

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
        APP_INFO1("BSA_BLE_SE_WRITE_EVT trans_id:%d, conn_id:%d, handle:%d",
            p_data->ser_write.trans_id, p_data->ser_write.conn_id, p_data->ser_write.handle);

        if(p_data->ser_write.status == 0)
            app_qiwo_ble_cmd_parser(p_data->ser_write.value, p_data->ser_write.len);

        if (p_data->ser_write.need_rsp)
        {
            BSA_BleSeSendRspInit(&send_server_resp);
            send_server_resp.conn_id = p_data->ser_write.conn_id;
            send_server_resp.trans_id = p_data->ser_write.trans_id;
            send_server_resp.status = p_data->ser_write.status;
            send_server_resp.handle = p_data->ser_write.handle;
            send_server_resp.len = 0;
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.conn_id:%d, send_server_resp.trans_id:%d", send_server_resp.conn_id, send_server_resp.trans_id, send_server_resp.status);
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.status:%d,send_server_resp.auth_req:%d", send_server_resp.status,send_server_resp.auth_req);
            APP_INFO1("BSA_BLE_SE_WRITE_EVT: send_server_resp.handle:%d, send_server_resp.offset:%d, send_server_resp.len:%d", send_server_resp.handle,send_server_resp.offset,send_server_resp.len );
            BSA_BleSeSendRsp(&send_server_resp);
        }
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


/*******************************************************************************
 **
 ** Function        app_qiwo_ble_server_create_service
 **
 ** Description     create qiwo service
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
 int app_qiwo_ble_server_create_service(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_CREATE ble_create_param;
    UINT16 service;
    UINT16  num_handle;
    BOOLEAN  is_primary;
    int server_num, attr_num;

    APP_INFO0("app_qiwo_ble_server_create_service");
   
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


    service = APP_QIWO_BLE_SERVICE_UUID;
    if (!service)
    {
        APP_ERROR1("wrong value = %d", service);
        return -1;
    }
   
    num_handle = APP_QIWO_BLE_HANDLE_NUM;
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

int app_qiwo_ble_server_create_service_128(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_CREATE ble_create_param;
    UINT16 service;
    UINT8 service_128[16] = {0x27,0x52,0xD1,0x9A,0x4B,0x94,0xF8,0x9B,0x0C,0x4E,0xB5,0x20,0x49,0x20,0xC4,0x36}; 
    UINT16  num_handle;
    BOOLEAN  is_primary;
    int server_num, attr_num;

    APP_INFO0("app_qiwo_ble_server_create_service");
   
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
   
    num_handle = APP_QIWO_BLE_HANDLE_NUM;
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

    memcpy(ble_create_param.service_uuid.uu.uuid128, service_128, 16);
    ble_create_param.service_uuid.len = 16;    
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
    app_ble_cb.ble_server[server_num].attr[attr_num].attr_UUID.len = 16;
    memcpy(app_ble_cb.ble_server[server_num].attr[attr_num].attr_UUID.uu.uuid128, service_128, 16);
    app_ble_cb.ble_server[server_num].attr[attr_num].is_pri = ble_create_param.is_primary;
    app_ble_cb.ble_server[server_num].attr[attr_num].attr_type = BSA_GATTC_ATTR_TYPE_SRVC;
    return 0;
}


/*******************************************************************************
 **
 ** Function        app_qiwo_ble_server_add_char
 **
 ** Description     Add character to qiwo service
 **
 ** Parameters      char_uuid: characteristic uuid
 **                 attribute_permission: Read-0x1, Write-0x10, Read|Write-0x11
 **                 characteristic_property: WRITE-0x08, READ-0x02, Notify-0x10, Indicate-0x20
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
 int app_qiwo_ble_server_add_char(UINT16 char_uuid, int attribute_permission, int characteristic_property)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_ADDCHAR ble_addchar_param;
    int server_num, srvc_attr_num, char_attr_num;
    
    server_num = 0;
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
    
    ble_addchar_param.is_descr = FALSE;
    ble_addchar_param.perm = attribute_permission;
    ble_addchar_param.property = characteristic_property;

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


int app_qiwo_ble_server_add_char_128(UINT16 char_uuid, int attribute_permission, int characteristic_property)
{
    tBSA_STATUS status;
    tBSA_BLE_SE_ADDCHAR ble_addchar_param;
    int server_num, srvc_attr_num, char_attr_num;
    UINT8 char_128[16] = {0xE6,0xD0,0x80,0x4A,0xA6,0x13,0xEB,0x8E,0x02,0x4D,0x97,0x93,0x49,0x20,0xC4,0x36};
    
    server_num = 0;
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
    ble_addchar_param.char_uuid.len = 16;
    memcpy(ble_addchar_param.char_uuid.uu.uuid128, char_128, 16);
    
    ble_addchar_param.is_descr = FALSE;
    ble_addchar_param.perm = attribute_permission;
    ble_addchar_param.property = characteristic_property;

    status = BSA_BleSeAddChar(&ble_addchar_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleSeAddChar failed status = %d", status);
        return -1;
    }

    /* save all information */
    app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_UUID.len = 16;
    memcpy(app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_UUID.uu.uuid128, char_128, 16);   
    app_ble_cb.ble_server[server_num].attr[char_attr_num].prop = characteristic_property;
    app_ble_cb.ble_server[server_num].attr[char_attr_num].attr_type = BSA_GATTC_ATTR_TYPE_CHAR;
    app_ble_cb.ble_server[server_num].attr[char_attr_num].wait_flag = TRUE;

    return 0;
}


/*******************************************************************************
 **
 ** Function        app_qiwo_ble_server_start_service
 **
 ** Description     Start Service
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_qiwo_ble_server_start_service(void)
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

int app_qiwo_ble_server_disconnect(void)
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


/*******************************************************************************
 **
 ** Function         app_qiwo_ble_server_init
 **
 ** Description      qiwo ble service init
 **
 ** Parameters
 **
 ** Returns          positive number(include 0) if successful, error code otherwise
 **
 *******************************************************************************/
int app_qiwo_ble_server_init(void)
{
    tBSA_DM_BLE_ADV_CONFIG adv_conf, scan_rsp;

    app_dm_set_visibility(FALSE, FALSE);
    app_dm_set_ble_visibility(TRUE, TRUE);

    app_ble_server_register(APP_QIWO_BLE_UUID, app_qiwo_ble_server_profile_cback);

#if 1
    app_qiwo_ble_server_create_service_128();
    GKI_delay(500);
    app_qiwo_ble_server_add_char_128(APP_QIWO_BLE_CHAR_CMD_UUID, 0x11, 0x18);
    GKI_delay(500);
#else
    app_qiwo_ble_server_create_service();
    GKI_delay(50);
    app_qiwo_ble_server_add_char(APP_QIWO_BLE_CHAR_CMD_UUID, 0x11, 0x18);
    GKI_delay(50);
#endif
    app_qiwo_ble_server_start_service();
    GKI_delay(500);
    
    /* start advertising */
/*    memset(&adv_conf, 0, sizeof(tBSA_DM_BLE_ADV_CONFIG));
    adv_conf.len = APP_BLE_WIFI_RESP_ADV_VALUE_LEN;
    adv_conf.flag = BSA_DM_BLE_ADV_FLAG_MASK;
    memcpy(adv_conf.p_val, app_ble_wifi_resp_adv_value, APP_BLE_WIFI_RESP_ADV_VALUE_LEN);
    adv_conf.adv_data_mask = BSA_DM_BLE_AD_BIT_FLAGS|BSA_DM_BLE_AD_BIT_MANU;
    adv_conf.is_scan_rsp = FALSE;
    app_dm_set_ble_adv_data(&adv_conf);*/

    /* configure scan response data */
    memset(&scan_rsp, 0, sizeof(tBSA_DM_BLE_ADV_CONFIG));
    scan_rsp.len = APP_QIWO_BLE_RESP_SCAN_RSP_VALUE_LEN;
    scan_rsp.flag = BSA_DM_BLE_ADV_FLAG_MASK;
    memcpy(scan_rsp.p_val, app_qiwo_ble_resp_scan_rsp_value, APP_QIWO_BLE_RESP_SCAN_RSP_VALUE_LEN);
    scan_rsp.adv_data_mask = BSA_DM_BLE_AD_BIT_FLAGS|BSA_DM_BLE_AD_BIT_MANU|BTM_BLE_AD_BIT_DEV_NAME;
    scan_rsp.is_scan_rsp = TRUE;
    app_dm_set_ble_adv_data(&scan_rsp);

    
    return 0;
}

void app_qiwo_ble_server_exit(void)
{   
    app_qiwo_ble_server_disconnect();
    GKI_delay(200);
    app_ble_server_deregister();
    app_dm_set_ble_visibility(FALSE, FALSE); 
    app_dm_set_visibility(FALSE, TRUE);
}

void app_qiwo_ble_switch(UINT8 next_state)
{
    int i;
    
    if(next_state == QIWO_BLE_STATE_SERVER) //switch to QIWO_BLE_STATE_SERVER
    {
        if(g_qiwo_ble_state == QIWO_BLE_STATE_MESH)
        {
            g_qiwo_ble_state = QIWO_BLE_STATE_SERVER;

            blemesh_bridge_switch(FALSE);

            for(i = 0; i < BSA_BLE_CLIENT_MAX; i++)
                app_ble_client_deregister(i);

            app_qiwo_ble_server_init();
        }
    }
    else if(next_state == QIWO_BLE_STATE_MESH) //switch to QIWO_BLE_STATE_MESH
    {
        if(g_qiwo_ble_state == QIWO_BLE_STATE_SERVER)
        {
            g_qiwo_ble_state = QIWO_BLE_STATE_MESH;  
            
            app_qiwo_ble_server_exit();
            //flood_bridge_start();
            blemesh_bridge_switch(TRUE);

            app_ble_client_register(9000);
            app_ble_client_register(9001);
            app_ble_client_register(9002);
            app_ble_client_register(9003);
            app_ble_client_register(9004);
            app_dm_set_ble_bg_conn_type(1);
            
            app_ag_init();
            app_ag_start(BSA_SCO_ROUTE_PCM); 
        }        
    }
}

/******************************************************************************
**Q6M
*******************************************************************************/
void q6m_init(void)
{
    int i;
    
    memset(&g_qiwo_q6m, 0, sizeof(tQIWO_Q6M));

    for(i = 0 ; i < QIWO_Q6M_DEVICE_MAX_NUM; i++)
        g_qiwo_q6m.device.conn_id[0] = BSA_BLE_INVALID_CONN;
}

int q6m_register_notification(UINT8 client_num)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_NOTIFREG ble_notireg_param;
    UINT16 service_id, character_id;

    int uuid;
    int ser_inst_id, char_inst_id, is_primary;

    APP_INFO0("q6m_register_notification");
    
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

    service_id = Q6M_UUID_SERVICE;
    character_id = Q6M_UUID_CHAR;
    
    ble_notireg_param.notification_id.srvc_id.id.uuid.len = 2;
    ble_notireg_param.notification_id.srvc_id.id.uuid.uu.uuid16 = service_id;
    ble_notireg_param.notification_id.char_id.uuid.len = 2;
    ble_notireg_param.notification_id.char_id.uuid.uu.uuid16 = character_id;
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

int q6m_ble_client_read(UINT16 conn_id, UINT8* p_data, UINT16 len)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_READ ble_read_param;
    UINT16 service, char_id, descr_id;
    int client_num;
    int ser_inst_id, char_inst_id, is_primary;

    APP_INFO1("app_ble_client_read conn_id = %d", conn_id);
    
    client_num = app_ble_client_find_index_by_conn_id(conn_id);
    if((client_num < 0) ||
       (client_num >= BSA_BLE_CLIENT_MAX) ||
       (app_ble_cb.ble_client[client_num].enabled == FALSE))
    {
        APP_ERROR1("app_ble_client_read client %d was not enabled yet", client_num);
        return -1;
    }

    status = BSA_BleClReadInit(&ble_read_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_ble_client_read failed status = %d", status);
        return -1;
    }

    ble_read_param.char_id.srvc_id.id.inst_id = 0;
    ble_read_param.char_id.srvc_id.id.uuid.uu.uuid16 = Q6M_UUID_SERVICE;
    ble_read_param.char_id.srvc_id.id.uuid.len = 2;
    ble_read_param.char_id.srvc_id.is_primary = 1;

    ble_read_param.char_id.char_id.inst_id = 0;
    ble_read_param.char_id.char_id.uuid.uu.uuid16 = Q6M_UUID_CHAR;
    ble_read_param.char_id.char_id.uuid.len = 2;
    ble_read_param.descr = FALSE;

    ble_read_param.conn_id = app_ble_cb.ble_client[client_num].conn_id;
    ble_read_param.auth_req = 0x00;
    status = BSA_BleClRead(&ble_read_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_ble_client_read failed status = %d", status);
        return -1;
    }
    return 0;
}

int q6m_ble_client_write(UINT16 conn_id, UINT8 write_type, UINT8* p_data, UINT16 len)
{
    tBSA_STATUS status;
    tBSA_BLE_CL_WRITE ble_write_param;
    UINT16 char_id, index, service, descr_id;
    int client_num;
    UINT8 desc_value;

    APP_INFO1("q6m_ble_client_write conn_id = %d", conn_id);

    if(len > BSA_BLE_CL_WRITE_MAX)
    {
        APP_ERROR1("len error! = %d", len);
        return -1;    
    }
   
    client_num = app_ble_client_find_index_by_conn_id(conn_id);
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

    memcpy(ble_write_param.value, p_data, len);
    ble_write_param.len = len;
    ble_write_param.descr = FALSE;
    ble_write_param.char_id.srvc_id.id.inst_id = 0;
    ble_write_param.char_id.srvc_id.id.uuid.uu.uuid16 = Q6M_UUID_SERVICE;
    ble_write_param.char_id.srvc_id.id.uuid.len = 2;
    ble_write_param.char_id.srvc_id.is_primary = 1;

    ble_write_param.char_id.char_id.inst_id = 0;
    ble_write_param.char_id.char_id.uuid.uu.uuid16 = Q6M_UUID_CHAR;
    ble_write_param.char_id.char_id.uuid.len = 2;
   

    ble_write_param.conn_id = app_ble_cb.ble_client[client_num].conn_id;
    ble_write_param.auth_req = BTA_GATT_AUTH_REQ_NONE;
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

void q6m_handle_notification_data(UINT16 conn_id, UINT16 ser_id, UINT16 char_id, UINT8 *data, UINT8 len)
{
    int i;

    if ((ser_id != Q6M_UUID_SERVICE) || (char_id != Q6M_UUID_CHAR))
    {
        APP_INFO0("q6m_handle_notification_data NOT Q6M notification");
        return;
    }
        
    for(i = 0; i < QIWO_Q6M_DEVICE_MAX_NUM; i++)
    {
    	if ((TRUE == g_qiwo_q6m.device.is_conn[i]) && (conn_id = g_qiwo_q6m.device.is_conn[i]))
    	{
    	    APP_INFO1("q6m_handle_notification_data Forwared notification data to host: len %d", len);
    	    
            q6m_notify_ble_forward(g_qiwo_q6m.device.addr[i], data, len);
            break;
    	}
    }
}

void q6m_handle_on_conn_up(BD_ADDR connected_addr, UINT16 conn_id)
{
    int i;
    UINT8 data = 0xbb;
    
	APP_INFO1("q6m_handle_on_conn_up Connected Address : 0x%02x%02x%02x%02x%02x%02x\n", connected_addr[0], connected_addr[1],
             connected_addr[2], connected_addr[3], connected_addr[4], connected_addr[5]);
	
 
    for(i = 0; i < QIWO_Q6M_DEVICE_MAX_NUM; i++)
    {
        if(!g_qiwo_q6m.device.is_conn[i])
        {   
            bd_cpy(g_qiwo_q6m.device.addr[i], connected_addr);
            g_qiwo_q6m.device.is_conn[i] = TRUE;
            g_qiwo_q6m.device.conn_id[i] = conn_id;
            g_qiwo_q6m.device.num++;
            
            q6m_register_notification(app_ble_client_find_index_by_conn_id(conn_id));

            q6m_ble_client_write(conn_id, BTA_GATTC_TYPE_WRITE, &data, 1);

            q6m_notify_connect_status(g_qiwo_q6m.device.addr[i], TRUE);

            break;
        }
    }   
    
}

void q6m_handle_on_conn_down(BD_ADDR connected_addr)
{
    int i;

    for(i = 0; i < QIWO_Q6M_DEVICE_MAX_NUM; i++)
    {
        if(g_qiwo_q6m.device.is_conn[i] && (memcmp(g_qiwo_q6m.device.addr[i], connected_addr, 6) == 0))
        {
            q6m_notify_connect_status(g_qiwo_q6m.device.addr[i], FALSE);
            
            memset(g_qiwo_q6m.device.addr[i], 0, 6);
            g_qiwo_q6m.device.is_conn[i] = FALSE;
            g_qiwo_q6m.device.conn_id[i] = BSA_BLE_INVALID_CONN;
            g_qiwo_q6m.device.num--;

            break;
        }
    }
}

int q6m_scan_complete_callback(void)
{
    UINT8 i;
    
    for(i = 0; i < g_qiwo_q6m_scan_device.num; i++)
    {
        app_qiwo_ble_connect(g_qiwo_q6m_scan_device.addr[i], FALSE);
        APP_INFO1("Connected to Q6M: 0x%02x%02x%02x%02x%02x%02x\n",
            g_qiwo_q6m_scan_device.addr[i][0], 
            g_qiwo_q6m_scan_device.addr[i][1],
            g_qiwo_q6m_scan_device.addr[i][2], 
            g_qiwo_q6m_scan_device.addr[i][3], 
            g_qiwo_q6m_scan_device.addr[i][4], 
            g_qiwo_q6m_scan_device.addr[i][5]);
            
        return 0;
    }      
}

void q6m_scan_result_callback(tBSA_DISC_EVT event, tBSA_DISC_MSG *p_data)
{
    UINT8 i = 0;
    
	APP_INFO1("q6m_scan_result_callback =>+++ event %d \n", event);

	switch (event)
	{
		case BSA_DISC_NEW_EVT:
		{
            APP_INFO1("\t q6m_scan_result_callback Bdaddr:%02x:%02x:%02x:%02x:%02x:%02x",
                    p_data->disc_new.bd_addr[0],
                    p_data->disc_new.bd_addr[1],
                    p_data->disc_new.bd_addr[2],
                    p_data->disc_new.bd_addr[3],
                    p_data->disc_new.bd_addr[4],
                    p_data->disc_new.bd_addr[5]);
            APP_INFO1("\t q6m_scan_result_callback Name:%s", p_data->disc_new.name);

            if(0 == strcmp(p_data->disc_new.name, "Q6M"))
            {
                if(g_qiwo_q6m_scan_device.num < (QIWO_Q6M_DEVICE_MAX_NUM - g_qiwo_q6m.device.num))
                {
                    bd_cpy(g_qiwo_q6m_scan_device.addr[g_qiwo_q6m_scan_device.num], p_data->disc_new.bd_addr);
                    g_qiwo_q6m_scan_device.num++;
                }   
                if(g_qiwo_q6m_scan_device.num >= (QIWO_Q6M_DEVICE_MAX_NUM - g_qiwo_q6m.device.num))
                    app_disc_abort();
            }
		}
		break;

		case BSA_DISC_CMPL_EVT:
		{
			int result = q6m_scan_complete_callback();
            g_qiwo_q6m.is_scaning = FALSE;
            
			APP_INFO1("q6m_scan_result_callbackon Scan callback return : %d\n", result);
		}
		break;
	};
}


BOOLEAN is_q6m_disc_device(BD_ADDR bda)
{
    int i;
    
    for(i = 0; i < g_qiwo_q6m_scan_device.num; i++)
    {
        if(memcmp(g_qiwo_q6m_scan_device.addr[i], bda, 6) == 0)
            return TRUE;
    }   

    return FALSE;
}

BOOLEAN is_q6m_device_by_attr(tAPP_BLE_CLIENT_DB_ELEMENT *p_blecl_db_elmt)
{
    tAPP_BLE_CLIENT_DB_ATTR *cur_attr;
    BOOLEAN is_service, is_char;
    
    cur_attr = p_blecl_db_elmt->p_attr;

    is_service = FALSE;
    is_char    = FALSE;
    while(cur_attr != NULL)
    {
        if(cur_attr->attr_type == BSA_GATTC_ATTR_TYPE_SRVC)
        {
            is_service = (cur_attr->attr_UUID.uu.uuid16 == Q6M_UUID_SERVICE) ? TRUE : FALSE;
        }
        else if(cur_attr->attr_type == BSA_GATTC_ATTR_TYPE_CHAR)
        {
            is_char = (cur_attr->attr_UUID.uu.uuid16 == Q6M_UUID_CHAR) ? TRUE : FALSE;
        }
        if(is_service && is_char)
            return TRUE;

        cur_attr = cur_attr->next;
    }

    return FALSE;
}

int app_qiwo_q6m_scan_connect(void)
{
    if(g_qiwo_ble_state != QIWO_BLE_STATE_MESH)
    {
        APP_ERROR0("app_qiwo_q6m_scan_connect NOT BLE CLIENT STATE!!!");
        return -1;
    }
    if(g_qiwo_q6m.is_scaning)
    {
        APP_ERROR0("app_qiwo_q6m_scan_connect IS SCANING!!!");
        return -1;       
    }
    if(g_qiwo_q6m.device.num >= 2)
    {
        APP_ERROR0("app_qiwo_q6m_scan_connect NUM MAX!!!");
        return -1;       
    }    

    memset(&g_qiwo_q6m_scan_device, 0, sizeof(tQIWO_Q6M_DEVICE));

    if(pBridge_info->is_scaning || g_qiwo_q6m.is_scaning)
    {
        app_disc_abort();
        GKI_delay(100);
    }
    g_qiwo_q6m.is_scaning = TRUE;
	app_disc_start_ble_regular(q6m_scan_result_callback);

    return 0;
}

void app_qiwo_q6m_disconnect(BD_ADDR addr)
{
    int i;

    for(i = 0; i < QIWO_Q6M_DEVICE_MAX_NUM; i++)
    {    
        if(g_qiwo_q6m.device.is_conn[i] && (memcmp(g_qiwo_q6m.device.addr[i], addr, 6) == 0))
        {
            app_qiwo_ble_client_unpair(app_ble_client_find_index_by_conn_id(g_qiwo_q6m.device.conn_id[i])); 
            return;
        }
    }    
}

void app_qiwo_q6m_change_frequency(UINT8* p_data,  UINT16 len)
{
    int i;

    for(i = 0; i < QIWO_Q6M_DEVICE_MAX_NUM; i++)
    {    
        if(g_qiwo_q6m.device.is_conn[i])
        {
            q6m_ble_client_write(g_qiwo_q6m.device.conn_id[i], BTA_GATTC_TYPE_WRITE_NO_RSP, p_data, len);
        }
    }   
    
}

void app_qiwo_q6m_read_data(UINT8* p_data,  UINT16 len)
{
    int i;
    
    for(i = 0; i < QIWO_Q6M_DEVICE_MAX_NUM; i++)
    {    
        if(g_qiwo_q6m.device.is_conn[i] && (memcmp(g_qiwo_q6m.device.addr[i], p_data, 6) == 0))
        {
            q6m_ble_client_read(g_qiwo_q6m.device.conn_id[i], p_data + 6, len - 6);
            return;
        }
    }       
    
}

