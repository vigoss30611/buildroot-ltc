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

#include "app_ag.h"

#include "bsa_api.h"

#include "gki.h"
#include "uipc.h"

#include "app_utils.h"
#include "app_mgt.h"
#include "app_xml_param.h"
#include "app_xml_utils.h"
#include "app_disc.h"
#include "app_dm.h"

#include "app_services.h"
#include "app_mgt.h"

#include "app_manager.h"

#include "app_ble.h"
#include "app_ble_client.h"
#include "app_ble_server.h"
#if defined(APP_BLE_OTA_FW_DL_INCLUDED) && (APP_BLE_OTA_FW_DL_INCLUDED == TRUE)
#include "app_ble_client_otafwdl.h"
#endif

#if defined(USE_IPC_FILE_OP)
#include "file_op.h"
#endif

#include "app_qiwo.h"

extern tQIWO_SENSOR_LIST g_qiwo_sensor;

tAPP_QIWO_BLE_STATE g_qiwo_ble_state;

tQIWO_CMD g_qiwo_cmd;

#if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
BOOLEAN g_qiwo_isCommandRsp;
#endif
extern tAPP_QIWO_AG_CONN g_qiwo_ag_conn;

static UINT8 g_cmd_store_data_len;
static UINT8 ble_store_data_len;
static int ble_package_num = -1;
static tQIWO_CMD ble_package_data;

static void app_qiwo_cmd_handle(char *event, void *arg)
{
	UINT8 *cmd = (UINT8 *)arg;
	int len;

	printf("app qiwo recv cmd %s %x %x %x %x \n", event, cmd[0], cmd[1], cmd[2], cmd[3]);
	if (!strncmp(event, "qiwo_bt_cmd", strlen("qiwo_bt_cmd"))) {
		len = *(int *)cmd;
		app_qiwo_cmd_parser(cmd + sizeof(int), len);
	}

	return;
}

int socket_parser_cmd(UINT8* p_data, UINT16 data_len)
{
    UINT16 cmd_data_len; 
   
    g_qiwo_cmd.type = p_data[0];
    g_qiwo_cmd.data_len = (UINT16)p_data[1] + ((UINT16)p_data[2] << 8);
    if(data_len < (g_qiwo_cmd.data_len + 3))
        g_qiwo_cmd.data_len = data_len - 3;

	if (g_qiwo_cmd.data_len > 0)
    	memcpy(g_qiwo_cmd.data, p_data + 3, g_qiwo_cmd.data_len);
    
    return 0;
}

void cmd_parse_data_init(void)
{
    g_qiwo_cmd.data_len = 0;
    g_cmd_store_data_len = 0;
    memset(g_qiwo_cmd.data, 0, QIWO_CMD_DATA_MAX_LEN);
}
 
//zhifeng  
//#define REG_INFO_FILE  "/mnt/id/reg_info_file" 
#define REG_INFO_FILE  "/config/bt/reg_info_file" 
#define	un_lock( fd, offset, whence, len ) \
	set_lock( fd, F_SETLK, F_UNLCK, offset, whence, len )
#define	write_lock( fd, offset, whence, len ) \
	set_lock( fd, F_SETLK, F_WRLCK, offset, whence, len )

int	set_lock( int fd, int cmd, int type, off_t offset, int whence, off_t len )
{
	struct flock lock;

	lock.l_type = type;
	lock.l_start = offset;
	lock.l_whence = whence;
	lock.l_len = len;

	return ( fcntl( fd, cmd, &lock ) );
}

int saveBleData(char* bleFile, char* data)
{
    if (!bleFile || !data) return 0;

    FILE* file= NULL;
                  
#if defined(USE_IPC_FILE_OP)
    file = ipc_file_fopen(bleFile,"w");
#else
    file = fopen(bleFile,"w");
#endif
    if(file == NULL) 
    {
        APP_ERROR1("error open %s\n",bleFile);
        return 0;
    }
    
    write_lock( file, 0, SEEK_SET, 0 );
    int ret = fputs(data, file);
    printf("fputs ret=%d,Len=%d,prop=%s\n",ret,strlen(data),data);
    un_lock( file, 0, SEEK_SET, 0 );

#if defined(USE_IPC_FILE_OP)
    ipc_file_fclose(file);
#else
    fclose(file);
#endif
    system("sync");

    return 1;
}

/*******************************************************************************
 **
 ** Function        app_qiwo_cmd_parser
 **
 ** Description     parser qiwo command
 **
 ** Parameters    data: data format 
 **                   -- head(1byte)+command_types(1byte)+command_data_len(1byte)+command_data  
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
void app_qiwo_cmd_parser(UINT8* p_data, UINT16 data_len)
{
    UINT8* p_parser;
    UINT8 cmd;
    BD_ADDR bd_addr;
    UINT8 cmd_data[QIWO_CMD_DATA_MAX_LEN];
    UINT8 addr_type, node_id;
    
    APP_DEBUG0("app_qiwo_cmd_parser");

    p_parser = p_data;

    //command must be equal or greater than 2
    if(data_len == 0)
    {
        APP_ERROR1("not command! packet_len = %d ", data_len);
        return;
    }

    if(p_data == NULL)
    {
        APP_ERROR0("command data NULL");
        return;
    }  
    
    socket_parser_cmd(p_data, data_len);

    cmd = g_qiwo_cmd.type; //command types
    APP_DEBUG1("app_qiwo_cmd_parser command type = 0x%x", cmd);
    APP_DUMP("command data", g_qiwo_cmd.data, g_qiwo_cmd.data_len);
    
    switch(cmd)
    {
    case RUN_BLE_MESH_CMD:
        app_qiwo_ble_switch(QIWO_BLE_STATE_MESH);
        break;

    case RUN_BLE_SERVER_CMD:
        app_qiwo_ble_switch(QIWO_BLE_STATE_SERVER);
        break;
        
    case AG_SCAN_CMD:
        app_qiwo_ag_scan();
        break;
        
    case AG_CONNECT_CMD:
        memcpy(bd_addr, g_qiwo_cmd.data,  sizeof(BD_ADDR));
        app_qiwo_ag_connect(bd_addr);
        break;
        
    case AG_DISCONNECT_CMD:
        memcpy(bd_addr, g_qiwo_cmd.data,  sizeof(BD_ADDR));
        app_qiwo_ag_disconnect(bd_addr);
        break;
        
    case AG_INQUIRY_CMD:
        app_qiwo_ag_inquiry();
        break;
        
    case UPDATE_MESH_DEVICE_INFO:       
		if(g_qiwo_cmd.data_len != 8)
        {
            APP_ERROR1("UPDATE_MESH_DEVICE_INFO addr error %d\r\n", g_qiwo_cmd.data_len);
        }      
            
        APP_DUMP("UPDATE_MESH_DEVICE_INFO:", g_qiwo_cmd.data, g_qiwo_cmd.data_len);
        addr_type = g_qiwo_cmd.data[0];
        node_id = g_qiwo_cmd.data[1];
        //str2ba(g_qiwo_cmd.data+2, add_address);
        memcpy(bd_addr, g_qiwo_cmd.data+2,  sizeof(BD_ADDR));

        APP_INFO1("Add Address : 0x%02x%02x%02x%02x%02x%02x\n", bd_addr[5], bd_addr[4],
             bd_addr[3], bd_addr[2], bd_addr[1], bd_addr[0]);

		flood_bridge_add_node(addr_type, node_id, bd_addr);
        break;
        
    case BRIDGE_MESSAGE:
        forward_LE_command(g_qiwo_cmd.data, g_qiwo_cmd.data_len);
        sensor_forward_LE_command(g_qiwo_cmd.data, g_qiwo_cmd.data_len);
        break;

    case BRIDGE_CLEAN_ONE_NODES_INFO:
		if(g_qiwo_cmd.data_len != 6)
        {
            APP_ERROR1("BRIDGE_CLEAN_ONE_NODES_INFO addr error %d\r\n", g_qiwo_cmd.data_len);
        }

        APP_DUMP("BRIDGE_CLEAN_ONE_NODES_INFO:", g_qiwo_cmd.data, g_qiwo_cmd.data_len);
        //str2ba(g_qiwo_cmd.data, bd_addr);
        memcpy(bd_addr, g_qiwo_cmd.data,  sizeof(BD_ADDR));
        flood_bridge_delete_node(bd_addr);
        break;
        
	case BRIDGE_CLEAN_NODES_INFO:
        flood_brige_clear_all_nodes();
        break;

    case Q6M_SCAN_CONNECT_CMD:
        app_qiwo_q6m_scan_connect();
        break;
        
    case Q6M_DISCONNECT_CMD:
        memcpy(bd_addr, g_qiwo_cmd.data, sizeof(BD_ADDR));
        app_qiwo_q6m_disconnect(bd_addr);
        break;
        
    case Q6M_CHANGE_FREQ_CMD:
        app_qiwo_q6m_change_frequency(g_qiwo_cmd.data, g_qiwo_cmd.data_len); 
        break;
        
    case Q6M_READ_DATA_CMD:
        app_qiwo_q6m_read_data(g_qiwo_cmd.data, g_qiwo_cmd.data_len);
        break;

            
    default:
        APP_ERROR1("app_qiwo_cmd_parser couldn't parse this command:%d !", cmd);
        break;
    }

    cmd_parse_data_init();
}


int ble_parser_cmd(UINT8* p_data, UINT16 data_len)
{
    UINT8* p_parser; 
    BOOLEAN is_whole_package;
    UINT8 package_cmd_data_len, remain_len;
    UINT8 offset;
    
    p_parser = p_data;   
    remain_len = data_len;
    while(remain_len)
    {
        if(*p_parser == 0) //package 0
        {
            if (ble_package_num < 0)
            	ble_package_num = 0;
            else
				return FALSE;

		    memset(ble_package_data.data, 0, QIWO_CMD_DATA_MAX_LEN);
            ble_package_data.type = *++p_parser;
            ble_package_data.data_len = *++p_parser;
            ble_store_data_len = 0;

            offset = 3;
        }
        else
        {
        	if (*p_parser != (ble_package_num + 1))
        		return FALSE;
        	else
        		ble_package_num = *p_parser;
            offset = 1;
        }
        p_parser++;
    
        if(remain_len > QIWO_GATT_CHAR_LENGTH)
        {
            package_cmd_data_len = QIWO_GATT_CHAR_LENGTH - offset;
            remain_len -= QIWO_GATT_CHAR_LENGTH;
        }
        else
        {
            package_cmd_data_len = remain_len - offset;  
            remain_len = 0;
        }

        memcpy(ble_package_data.data + ble_store_data_len, p_parser, package_cmd_data_len);
        ble_store_data_len += package_cmd_data_len;
        p_parser += package_cmd_data_len;
    }
    
    if(ble_store_data_len >= ble_package_data.data_len)
        is_whole_package = TRUE;
    else
        is_whole_package = FALSE;

    return is_whole_package;
}

/*******************************************************************************
 **
 ** Function        app_qiwo_ble_cmd_parser
 **
 ** Description     parser qiwo ble command
 **
 ** Parameters    data: data format 
 **                   -- head(1byte)+command_types(1byte)+command_data_len(1byte)+command_data  
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
void app_qiwo_ble_cmd_parser(UINT8* p_data, UINT16 data_len)
{
    UINT8* p_parser;
    UINT8 cmd;
    BD_ADDR bd_addr;
    UINT8 cmd_data[QIWO_CMD_DATA_MAX_LEN];
    BOOLEAN is_whole_package;
        
    APP_DEBUG0("app_qiwo_ble_cmd_parser");

    p_parser = p_data;

    //command must be equal or greater than 2
    if(data_len == 0)
    {
        APP_ERROR1("not command! packet_len = %d ", data_len);
        return;
    }

    if(p_data == NULL)
    {
        APP_ERROR0("command data NULL");
        return;
    }  
   
    if(!(is_whole_package = ble_parser_cmd(p_data, data_len)))
    {
        APP_DEBUG1("continue to get command data %d", data_len);
        return;
    }

    APP_DEBUG1("app_qiwo_cmd_parser command type = %d len %d %d ", ble_package_data.type, ble_package_data.data_len, data_len);
    APP_DUMP("command data", ble_package_data.data, ble_package_data.data_len);
    
    switch(ble_package_data.type)
    {
    case QIWO_BLE_WIFI_CMD:
		sprintf(cmd_data, "%s/%s", "reg_data", ble_package_data.data);
		printf("QIWO_BLE_WIFI_CMD %s %d \n", cmd_data, strlen(cmd_data) + 1);
		event_send("qiwo_ble_data", cmd_data, strlen(cmd_data) + 1);
        break;
        
    default:
        APP_ERROR1("app_qiwo_cmd_parser couldn't parse this command:%d !", ble_package_data.type);
        break;
    }
    ble_store_data_len = 0;
    ble_package_num = -1;
}

/*******************************************************************************
 **
 ** Function         app_qiwo_mgt_callback
 **
 ** Description      This callback function is called in case of server
 **                  disconnection (e.g. server crashes)
 **
 ** Parameters
 **
 ** Returns          void
 **
 *******************************************************************************/
static BOOLEAN app_qiwo_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
    int index;
    
    switch(event)
    {
    case BSA_MGT_STATUS_EVT:
        APP_DEBUG0("BSA_MGT_STATUS_EVT");
        if (p_data->status.enable == FALSE)
        {
            APP_DEBUG0("Bluetooth Stopped");
            if(app_ag_cb.fd_sco_in_file >= 0)
            {
                /* if file was opened in previous test and if still opened, close it now */
                app_ag_close_rec_file();
                app_ag_cb.fd_sco_in_file = -1;
            }

            if(app_ag_cb.fd_sco_out_file >= 0)
            {
                /* if file was opened in previous test and if still opened, close it now */
                close(app_ag_cb.fd_sco_out_file);
                app_ag_cb.fd_sco_out_file = -1;
            }

            for(index=0; index<APP_AG_MAX_NUM_CONN; index++)
            {
                if((app_ag_cb.sco_route == BSA_SCO_ROUTE_HCI) &&
                   (app_ag_cb.uipc_channel[index] != UIPC_CH_ID_BAD))
                {
                    app_ag_cb.uipc_channel[index] = UIPC_CH_ID_BAD;
                }

                app_ag_cb.hndl[index] = BSA_AG_BAD_HANDLE;
            }
            app_ag_cb.voice_opened = FALSE;
            app_ag_cb.opened = FALSE;
        }
        else
        {
            APP_DEBUG0("Bluetooth restarted => re-initialize the application");
            app_mgr_config();
            app_ble_start();
            app_ag_start(app_ag_cb.sco_route);
        }
        break;

    case BSA_MGT_DISCONNECT_EVT:
        APP_DEBUG1("BSA_MGT_DISCONNECT_EVT reason:%d", p_data->disconnect.reason);
        exit(-1);
        break;

    default:
        break;
    }
    return FALSE;
}

void app_qiwo_clean_task(int sig)
{
	APP_DEBUG0("app_qiwo is killed, clean the task");

    app_ble_exit();

    flood_brige_exit();
    
    app_ag_end();
    
    app_mgt_close();  
    
	APP_DEBUG0("app_qiwo is killed, exit");
	
	exit(0);
}

static void qiwo_bt_sigsegv_handle(int sig)
{
	char name[32];

	if (sig == SIGSEGV) {
		prctl(PR_GET_NAME, name);
		printf("qiwo bt Segmentation Fault %s \n", name);
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
extern int msgOperateForBtState(bool connected);

int main(int argc, char **argv)
{
    int i;
    BOOLEAN no_init = FALSE;
    int mode = QIWO_BLE_STATE_MESH;
    int status;

    BD_ADDR bd_addr;
    int uarr[16];
    char szInput[64];
    int  x = 0;
    int length = 0;
    BOOLEAN set = FALSE;
    int i_set = 0;

	signal(SIGSEGV, qiwo_bt_sigsegv_handle);
    /* Check the CLI parameters */
	if	(argc > 1) {
		if (!strcmp(argv[1], "server"))
			mode = QIWO_BLE_STATE_SERVER;
	}
    for (i = 2; i < argc; i++)
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
		msgOperateForBtState(false);
		
        app_mgt_init();
        if (app_mgt_open("/tmp/", app_qiwo_mgt_callback) < 0)
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
    }

	event_register_handler("qiwo_bt_cmd", 0, app_qiwo_cmd_handle);
    /* Display FW versions */
    app_mgr_read_version();

#if defined(QIWO_ONLY_COMMAND_RSP) && (QIWO_ONLY_COMMAND_RSP == TRUE)
    g_qiwo_isCommandRsp = FALSE;
#endif
    g_qiwo_ag_conn.is_need_disconnect = FALSE;

    /* Initialize BLE application */
    status = app_ble_init();
    if (status < 0)
    {
        APP_ERROR0("Couldn't Initialize BLE app, exiting");
        exit(-1);
    }

    /* Start BLE application */
    status = app_ble_start();
    if (status < 0)
    {
        APP_ERROR0("Couldn't Start BLE app, exiting");
        return -1;
    }

//    app_ag_init();
//    app_ag_start(BSA_SCO_ROUTE_PCM);    

	if (mode == QIWO_BLE_STATE_SERVER) {
	    flood_bridge_init();
	    blemesh_bridge_switch(FALSE);

	    g_qiwo_ble_state = QIWO_BLE_STATE_SERVER;
	    app_qiwo_ble_server_init();
		q6m_init();
	} else {
	    flood_bridge_start();
	    g_qiwo_ble_state = QIWO_BLE_STATE_MESH;
	}

	signal(SIGINT, app_qiwo_clean_task);
    
    cmd_parse_data_init();
        
    do
    {
		APP_INFO0("Wait for Signal\n");
        pthread_mutex_lock(&bleMutex); 
        pthread_cond_wait (&bleCond, &bleMutex); 
        pthread_mutex_unlock (&bleMutex);

        GKI_delay(100);
        flood_bridge_search_nodes();
    } while (1);      /* While user don't exit application */

    if(g_qiwo_ble_state == QIWO_BLE_STATE_SERVER)
        app_qiwo_ble_server_exit();
    
    app_ble_exit();

    flood_brige_exit();
    
    app_ag_end();
    
    app_mgt_close();    
    
    return 0;
}

