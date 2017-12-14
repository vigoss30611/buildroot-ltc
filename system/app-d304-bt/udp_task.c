/*****************************************************************************
**
**  Name:           udp_task.c
**
**  Description:    Bluetooth BLE mesh bridge application
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "app_utils.h"

#include "bridge_info.h"
#include "udp_task.h"

#ifdef ENABLE_UDP_BROADCAST_ADDR


typedef enum{
    RECV_SM_IDLE = 0,
    RECV_SM_BEGINING,
    RECV_SM_GOING,
    RECV_SM_ENDING,
    RECV_SM_COUNT
}recv_sm_t;

typedef int (*recv_sm_handle_t)(char c);

typedef struct{
    recv_sm_t st;    
    int  cmd_len;       // length of command stored in cmd
    int  next_sync_index;    // it is used in sync head/end
    char cmd[256];
    recv_sm_handle_t sm_handle[RECV_SM_COUNT];
    
}recv_cb_t;

static recv_cb_t rcv_cb;

static const char  sync_start_code[] = { "floodmeshstart" };
static const char  sync_end_code[] = { "floodmeshend" };

static const int sync_start_code_len = sizeof(sync_start_code) - 1;
static const int sync_end_code_len = sizeof(sync_end_code) - 1;

static int bridge_notify(UINT8 *data, int length)
{
    sendto(pBridge_info->udpServ, data, length, 0, &pBridge_info->udpClient, sizeof(pBridge_info->udpClient));
    return 0;
}

static void bridge_notify_no_conn()
{	
    flood_bridge_cmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.CMD = BRIDGE_NOTIFY_NO_CONNECTION;
    bridge_notify((UINT8 *)&cmd, sizeof(cmd));
}

static void bridge_nofity_broadcast()
{
    flood_bridge_cmd_t cmd;

	APP_INFO0("UDP notify its address");
	
    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.CMD = BRIDGE_BROADCUST_INFO;
    cmd.hdr.session = BRIDGE_BRODCUST_SESSION;
    bridge_notify((uint8_t*)&cmd, sizeof(cmd));
}

#if 0
static void bridge_notify_ble_forward(uint8_t *data, int length)
{
    flood_bridge_cmd_t cmd;

		
    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.CMD = BRIDGE_NOTIFY_BLE_FORWARD;
    cmd.floodmesh_data.length = length;
    memcpy(cmd.floodmesh_data.data, data, length);
    bridge_notify((UINT8 *)&cmd, sizeof(cmd));
}
#endif
/*
 * command from phone side, need to forward to floodmesh node
*/
static int forward_LE_command(uint8_t *data, int length)
{
    uint8_t att_cmd_cache[256];

    if (!pBridge_info->proxy_connected)
    {
        bridge_notify_no_conn();
        return 0;
    }

    flood_bridge_write_data(pBridge_info->proxy_conn_id, data,length);
    return 0;
}
static int excute_cmd(const char * data, int len)
{
    flood_bridge_cmd_p p = (flood_bridge_cmd_p)(data + sync_start_code_len);
    flood_bridge_cmd_t cmd;

    memset(&cmd, 0, sizeof(flood_bridge_cmd_t));

    if (p->hdr.CMD == UPDATE_MESH_DEVICE_INFO)
    {
		BD_ADDR add_address;
        cmd.floodmesh_data.length = len - sync_start_code_len - sync_end_code_len - sizeof(flood_bridge_cmd_header_t);
        memcpy(cmd.floodmesh_data.data, data + sync_start_code_len + sizeof(flood_bridge_cmd_header_t), cmd.floodmesh_data.length);
        APP_INFO1("sync up one node %s\r\n", cmd.floodmesh_data.data);
        str2ba(cmd.floodmesh_data.data, add_address);

        APP_INFO1("Add Address : 0x%02x%02x%02x%02x%02x%02x\n", add_address[5], add_address[4],
             add_address[3], add_address[2], add_address[1], add_address[0]);

		flood_bridge_add_node(add_address);
    }
    else if (p->hdr.CMD == BRIDGE_MESSAGE)
    {
		
        cmd.hdr.CMD = BRIDGE_MESSAGE;
        cmd.hdr.session = p->hdr.session;
        cmd.floodmesh_data.length = len - sync_start_code_len - sync_end_code_len - sizeof(flood_bridge_cmd_header_t);
        memcpy(cmd.floodmesh_data.data, data + sync_start_code_len + sizeof(flood_bridge_cmd_header_t), cmd.floodmesh_data.length);
        forward_LE_command(cmd.floodmesh_data.data, cmd.floodmesh_data.length);
    }
	else if (p->hdr.CMD == BRIDGE_BROADCUST_INFO && p->hdr.session == BRIDGE_BRODCUST_SESSION)
    {
		APP_INFO0("received search bridge address cmd \r\n");
        bridge_nofity_broadcast();
    }
	else if (p->hdr.CMD == BRIDGE_CLEAN_NODES_INFO)
    {
        flood_brige_clear_all_nodes();
    }
    
    return 0;
}



static void reset_recv_sm_st(recv_sm_t st)
{
    rcv_cb.st = st;
    rcv_cb.cmd_len = 0;
    rcv_cb.next_sync_index = 0;
}

static int recv_sm_idle(char c)
{
	//APP_INFO1("recv_sm_idle: %c", c);
	
    if (c == sync_start_code[rcv_cb.next_sync_index])
    {
        rcv_cb.st++;
        rcv_cb.next_sync_index = 1;
        rcv_cb.cmd[rcv_cb.cmd_len] = c;
        rcv_cb.cmd_len++;
    }
    return 0;
}

static int recv_sm_sync_begining(char c)
{
	//APP_INFO1("recv_sm_sync_begining: %c", c);
	
    rcv_cb.cmd[rcv_cb.cmd_len] = c;
    rcv_cb.cmd_len++;

    if (c == sync_start_code[rcv_cb.next_sync_index])
    {        
        rcv_cb.next_sync_index++;        
        if (rcv_cb.next_sync_index >= sync_start_code_len)
        {
            rcv_cb.st++;
            rcv_cb.next_sync_index = 0;
        }
    }
    else
    {
        reset_recv_sm_st(RECV_SM_IDLE);
    }
    return 0;
}

static int recv_sm_going(char c)
{
	//APP_INFO1("recv_sm_going: %c", c);
	
    rcv_cb.cmd[rcv_cb.cmd_len] = c;
    rcv_cb.cmd_len++;

    if (c == 'f')
    {
        rcv_cb.next_sync_index = 1; // next ending code char
        rcv_cb.st++;
    }

    return 0;
}

static int recv_sm_ending(char c)
{
	//APP_INFO1("\n recv_sm_ending:%c, %c\n", c, sync_end_code[rcv_cb.next_sync_index]);
	
    rcv_cb.cmd[rcv_cb.cmd_len] = c;
    rcv_cb.cmd_len++;

    if (c == sync_end_code[rcv_cb.next_sync_index])
    {        
        rcv_cb.next_sync_index++;
		APP_INFO1("recv_sm_ending: next_sync_index:%d ,%d", rcv_cb.next_sync_index, sync_end_code_len);
        if (rcv_cb.next_sync_index == sync_end_code_len)
        {
			APP_INFO1("excute command :%d ,%d",rcv_cb.cmd, rcv_cb.cmd_len);
            excute_cmd(rcv_cb.cmd, rcv_cb.cmd_len);
            reset_recv_sm_st(RECV_SM_IDLE);
        }
    }
    else
    {
        rcv_cb.next_sync_index = 0;
        rcv_cb.st = RECV_SM_GOING;
    }
    return 0;
}

static void init_recv_sm()
{
    memset(&rcv_cb, 0, sizeof(rcv_cb));
    rcv_cb.st = RECV_SM_IDLE;

    rcv_cb.sm_handle[RECV_SM_IDLE]      = recv_sm_idle;
    rcv_cb.sm_handle[RECV_SM_BEGINING]  = recv_sm_sync_begining;
    rcv_cb.sm_handle[RECV_SM_GOING]     = recv_sm_going;
    rcv_cb.sm_handle[RECV_SM_ENDING]    = recv_sm_ending;

	//APP_INFO1("++ UDP func: %x, func %x", RECV_SM_IDLE, (int)rcv_cb.sm_handle[RECV_SM_IDLE]);
	//APP_INFO1("++ UDP func: %x, func %x", RECV_SM_BEGINING, (int)rcv_cb.sm_handle[RECV_SM_BEGINING]);
	//APP_INFO1("++ UDP func: %x, func %x", RECV_SM_GOING, (int)rcv_cb.sm_handle[RECV_SM_GOING]);
	//APP_INFO1("++ UDP func: %x, func %x", RECV_SM_ENDING, (int)rcv_cb.sm_handle[RECV_SM_ENDING]);
}

static int parse_incomming_data(char * data, int length)
{
    int i = 0;    
    
    for (i = 0; i < length; i++)
    {
		//APP_INFO1("++ UDP state: %x, func %x", rcv_cb.st, (int)rcv_cb.sm_handle[rcv_cb.st]);
        rcv_cb.sm_handle[rcv_cb.st](data[i]);
		//APP_INFO1("-- UDP state: %x", rcv_cb.st);
    }

    return 0;
}

static int bridge_start_server(void)
{
    struct sockaddr_in si_me, si_other;
    int s, i, slen=sizeof(si_other);
    char recvBuf[UDP_RECV_BUFFER_SIZE];   
    int   len = 0;
	static int so_reuseaddr = 1;
    
    if ((s=socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("cannot create flood mesh UDP socket\n"); 
        return -1; 
    }

	/*  
	  * Allow multiple listeners on the 
        * broadcast address: 
         */ 
    if (setsockopt(s,  SOL_SOCKET,  SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr)) < 0)
    {
		perror("cannot socket to receive broadcast\n"); 
        return -1; 
    }
    
    memset((char *) &si_me, 0, sizeof(si_me));
    memset(recvBuf, 0, UDP_RECV_BUFFER_SIZE);
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(FLOODMESH_UDP_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, &si_me, sizeof(si_me)) < 0)
    {
        perror("cannot flood mesh UDP socket bind failed\n"); 
        return -1; 
    }

    pBridge_info->udpServ = s;
	pBridge_info->upd_thread_run = 1;

    APP_INFO0("udp server started...\r\n");

	
    
    while (pBridge_info->upd_thread_run == 1)
    {
		len = sizeof(pBridge_info->udpClient);
        len = recvfrom(pBridge_info->udpServ, recvBuf, UDP_RECV_BUFFER_SIZE, 0, &pBridge_info->udpClient, &len);

        APP_INFO0("udp server received data ...\r\n");
        
        if (len > 0)
        {            
			APP_DUMP("recv: ", recvBuf, len);
            parse_incomming_data(recvBuf, len);            
        }
        else
        {
            sleep(100);
        }
    }

    //close(pBridge_info->udpServ);

	APP_INFO0("udp server end...\r\n");

    return 0;
}

static void * init_bridge_intf_2_cloud(void *temp)
{
    APP_INFO0("udp server init...\r\n");
    init_recv_sm();
    bridge_start_server();

	return 0;
}

void start_upd_task()
{   
    static char t[512];

    int err = pthread_create(&pBridge_info->udp_server_task, NULL, &init_bridge_intf_2_cloud, NULL);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
    else
        printf("\n Thread created successfully\n");

    
}

void end_udp_task()
{
	pBridge_info->upd_thread_run = 0;
	close(pBridge_info->udpServ);
	//pthread_join(&pBridge_info->udp_server_task, NULL);
}

#endif
	

