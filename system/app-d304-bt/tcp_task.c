/*****************************************************************************
**
**  Name:           tcp_task.c
**
**  Description:    Bluetooth BLE mesh bridge application
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "app_utils.h"

#include "bridge_info.h"
#include "tcp_task.h"

#include "app_qiwo.h"

typedef enum{
    RECV_SM_IDLE,
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

#define USE_TWO_SOCKET

static recv_cb_t rcv_cb;

static const char  sync_start_code[] = { "floodmeshstart" };
static const char  sync_end_code[] = { "floodmeshend" };

static const int sync_start_code_len = sizeof(sync_start_code) - 1;
static const int sync_end_code_len = sizeof(sync_end_code) - 1;

int flood_bridge_write_data(UINT16 conn_id, UINT8 *data, UINT8 len);

int app_qiwo_socket_send_rsp(UINT8* p_data, UINT16 data_len)
{
	int result = -1, len = 0;
	char cmd_data[QIWO_RSP_DATA_MAX_LEN + 128];

    APP_DUMP("response data", p_data, data_len);
    
#ifdef USE_TWO_SOCKET
	memset(cmd_data, 0, QIWO_RSP_DATA_MAX_LEN + 128);
	if (p_data[0] == AG_CONNECT_CMD) {
		sprintf(cmd_data, "pair_state/");
		len = strlen("pair_state/");
		cmd_data[len] = data_len;
		len += 1;
		memcpy(cmd_data + len, p_data, data_len);
		len += data_len;
	} else if (p_data[0] == AG_DISCONNECT_CMD) {
		sprintf(cmd_data, "pair_state/");
		len = strlen("pair_state/");
		cmd_data[len] = data_len;
		len += 1;
		memcpy(cmd_data + len, p_data, data_len);
		cmd_data[len + 3] = 0;
		len += data_len;
	} else if (p_data[0] == AG_SCAN_CMD) {
		sprintf(cmd_data, "scan_device/");
		len = strlen("scan_device/");
		cmd_data[len] = data_len;
		len += 1;
		memcpy(cmd_data + len, p_data, data_len);
		len += data_len;
	} else if (p_data[0] == AG_INQUIRY_CMD) {
		sprintf(cmd_data, "inquiry_result/");
		len = strlen("inquiry_result/");
		cmd_data[len] = data_len;
		len += 1;
		memcpy(cmd_data + len, p_data, data_len);
		len += data_len;
	} else {
		return result;
	}
	printf("ble data event send %d %d \n", data_len, len);
	event_send("qiwo_ble_data", cmd_data, len);
	result = 0;
    //if( -1 != send(pBridge_info->tutk_tcpChildFd , p_data, data_len, 0))
#else
    if( -1 != send(pBridge_info->tcpChildFd , p_data, data_len, 0))
    {
		result = 0;
    }
#endif

    return result;
}

static int bridge_notify(UINT8 *data, int length)
{
	int result = -1;
    if( -1 != send(pBridge_info->tcpChildFd , data, length, 0))
    {
		result = 0;
    }
    return result;
}

static void bridge_notify_no_conn()
{	
    flood_bridge_cmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.CMD = BRIDGE_NOTIFY_NO_CONNECTION;
    bridge_notify((UINT8 *)&cmd, sizeof(cmd));
}


#if 0
int bridge_notify_ble_forward(uint8_t *data, int length)
{
	int result = -1;
	
	if (pBridge_info->tcpChildFd != 0)
	{
	    flood_bridge_cmd_t cmd;
	    memset(&cmd, 0, sizeof(cmd));
	    cmd.hdr.CMD = BRIDGE_NOTIFY_BLE_FORWARD;
	    cmd.floodmesh_data.length = length;
	    memcpy(cmd.floodmesh_data.data, data, length);
	    result = bridge_notify((UINT8 *)&cmd, sizeof(cmd));
	}

	return result;
}
#else
int bridge_notify_ble_forward(uint8_t *data, int length)
{
	int result = -1;
	tQIWO_CMD cmd;
    
	if (pBridge_info->tcpChildFd != 0)
	{
	    memset(&cmd, 0, sizeof(cmd));
	    cmd.type = BRIDGE_NOTIFY_BLE_FORWARD;
	    cmd.data_len = length;
	    memcpy(cmd.data, data, length);
	    result = bridge_notify((UINT8 *)cmd.data, cmd.data_len);
	}

	return result;
}
#endif

int q6m_notify_ble_forward(BD_ADDR addr, UINT8* data, int length)
{
	int result = -1;
	tQIWO_CMD cmd;
    
    memset(&cmd, 0, sizeof(cmd));
    cmd.type = Q6M_UPLOAD_DATA_RESPOND;
    cmd.data_len = length + 9;
    cmd.data[0] = cmd.type;
    qiwo_u16_2_u8(length + 6, cmd.data + 1);
    bd_cpy(cmd.data + 3, addr);
    memcpy(cmd.data + 9, data, length);
    
    result = app_qiwo_socket_send_rsp((UINT8 *)cmd.data, cmd.data_len);

	return result;
}

int q6m_notify_connect_status(BD_ADDR addr, BOOLEAN is_conn)
{
	int result = -1;
	tQIWO_CMD cmd;
    
    memset(&cmd, 0, sizeof(cmd));
    cmd.type = Q6M_SCAN_CONNECT_RESPOND;
    cmd.data_len = 10;
    cmd.data[0] = cmd.type;
    cmd.data[1] = 7;
    bd_cpy(cmd.data + 3, addr);
    cmd.data[9] = (is_conn == TRUE) ? 1 : 0;
    
    result = app_qiwo_socket_send_rsp((UINT8 *)cmd.data, cmd.data_len);

	return result;
}


/*
 * command from phone side, need to forward to floodmesh node
*/
int forward_LE_command(UINT8 *data, int length)
{
    if (!pBridge_info->proxy_connected)
    {
        bridge_notify_no_conn();
        return 0;
    }

    data[DIRECT_CONN_NODE_ID_BYTE_INDEX] = pBridge_info->proxy_node_id;
    flood_bridge_write_data(pBridge_info->proxy_conn_id, data,length);
    return 0;
}

int sensor_forward_LE_command(UINT8* data, int length)
{
    int i;

    for(i = 0; i < QIWO_SENSOR_NUM; i++)
    {
        if (pBridge_info->sensor_connected[i])
        {
            data[DIRECT_CONN_NODE_ID_BYTE_INDEX] = pBridge_info->sensor_node_id[i];
            qiwo_sensor_write_data(pBridge_info->sensor_conn_id[i], data, length);
        }
    }

    return 0;
}

static int excute_cmd(const char * data, int len)
{
/*    flood_bridge_cmd_p p = (flood_bridge_cmd_p)(data + sync_start_code_len);
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
	else if (p->hdr.CMD == BRIDGE_CLEAN_NODES_INFO)
    {
        flood_brige_clear_all_nodes();
    }*/
	
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
    rcv_cb.cmd[rcv_cb.cmd_len] = c;
    rcv_cb.cmd_len++;

    if (c == sync_end_code[rcv_cb.next_sync_index])
    {        
        rcv_cb.next_sync_index++;
        if (rcv_cb.next_sync_index == sync_end_code_len)
        {
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
}

int parse_incomming_data(char * data, int length)
{
    int i = 0;    
    
    for (i = 0; i < length; i++)
    {
        rcv_cb.sm_handle[rcv_cb.st](data[i]);
    }

    return 0;
}

int bridge_start_tcp_server(void)
{
    struct sockaddr_in si_me;
    int s;
    char recvBuf[TCP_RECV_BUFFER_SIZE];   
    int   len = 0;
	static int so_reuseaddr = 1;
	int iOption = 1; //Trun on keep-alive, 0 = diable, 1 = enalbe
	int keep_idle = 60;
	int keep_interval = 5;
	int keep_count =3;
    
    if ((s=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("cannot create flood mesh TCP socket\n"); 
        return -1; 
    }

	/*  
	  * Allow multiple listeners on the 
        * broadcast address: 
         */
    if (setsockopt(s,  SOL_SOCKET,  SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr)) < 0)
    {
		perror("cannot socket to set reused addr\n"); 
        return -1; 
    } 

	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char *)&iOption, sizeof(int)) < 0)
	{
		perror("cannot socket to keep alive\n"); 
        return -1; 
	}

	if (setsockopt(s, SOL_TCP, TCP_KEEPIDLE, (const char *)&keep_idle, sizeof(int)) < 0)
	{
		perror("cannot socket to keep alive\n"); 
        return -1; 
	}

	if (setsockopt(s, SOL_TCP, TCP_KEEPINTVL, (const char *)&keep_interval, sizeof(int)) < 0)
	{
		perror("cannot socket to keep alive\n"); 
        return -1; 
	}

	if (setsockopt(s, SOL_TCP, TCP_KEEPCNT, (const char *)&keep_count, sizeof(int)) < 0)
	{
		perror("cannot socket to keep alive\n"); 
        return -1; 
	}

	

	
    
    memset((char *) &si_me, 0, sizeof(si_me));
    memset(recvBuf, 0, TCP_RECV_BUFFER_SIZE);
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(FLOODMESH_TCP_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, &si_me, sizeof(si_me)) < 0)
    {
        perror("cannot flood mesh TCP socket bind failed\n"); 
        return -1; 
    }

    pBridge_info->tcpServ = s;

   /* 
      * listen: make this socket ready to accept connection requests 
      */
    if (listen(s, 5) < 0) /* allow 1 requests to queue up */ 
    {
		perror("ERROR on listen");
    }

  
	pBridge_info->tcp_thread_run = 1;

    APP_INFO0("TCP server started...\r\n");

 	
    while (pBridge_info->tcp_thread_run == 1) {
		int clientlen = sizeof(pBridge_info->tcpClient);

        /* 
              * accept: wait for a connection request 
              */
        int childfd = accept(pBridge_info->tcpServ, (struct sockaddr *) &pBridge_info->tcpClient, &clientlen);
        if (childfd < 0) 
	    {
	      perror("ERROR on accept");
	    }
		pBridge_info->tcpChildFd = childfd;

		APP_INFO0("TCP server connected...\r\n");
    
    /* 
        * gethostbyaddr: determine who sent the message 
        
	    hostp = gethostbyaddr((const char *)&pBridge_info->tcpClient.sin_addr.s_addr, sizeof(pBridge_info->tcpClient.sin_addr.s_addr), AF_INET);
	    if (hostp == NULL)
	    {
	      perror("ERROR on gethostbyaddr");
	    }
	    hostaddrp = inet_ntoa(clientaddr.sin_addr);
	    if (hostaddrp == NULL)
	    {
	      perror("ERROR on inet_ntoa\n");
	    }
	    printf("server established connection with %s (%s)\n", hostp->h_name, hostaddrp);

	*/
    

  //add by zhifeng for test   


	    while (pBridge_info->tcp_thread_run == 1)
	    {
	        len = recv(pBridge_info->tcpChildFd, recvBuf, TCP_RECV_BUFFER_SIZE, 0);

	        APP_INFO0("tcp server received data ...\r\n");

	        if (len > 0)
	        {            
	            //parse_incomming_data(recvBuf, len);  
	            app_qiwo_cmd_parser(recvBuf, len);
	        }
	        else
	        {
				APP_INFO0("tcp server disconnected ...\r\n");
				close(pBridge_info->tcpChildFd);
				pBridge_info->tcpChildFd = 0;

				if (len == 0)    APP_INFO0("reason: socket closed\r\n");
				else if (len == -1)  APP_INFO0("reason: sokcet error\r\n");
				
	            break;
	        }
	    }

    }

	//close(pBridge_info->tcpServ);

	APP_INFO0("tcp server end...\r\n");

    return 0;
}

int tutk_bridge_start_tcp_server(void)
{
    struct sockaddr_in si_me;
    int s;
    char recvBuf[TCP_RECV_BUFFER_SIZE];   
    int   len = 0;
	static int so_reuseaddr = 1;
	int iOption = 1; //Trun on keep-alive, 0 = diable, 1 = enalbe
	int keep_idle = 60;
	int keep_interval = 5;
	int keep_count =3;
    
    if ((s=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("tutk cannot create flood mesh TCP socket\n"); 
        return -1; 
    }

	/*  
	  * Allow multiple listeners on the 
        * broadcast address: 
         */
    if (setsockopt(s,  SOL_SOCKET,  SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr)) < 0)
    {
		perror("tutk cannot socket to set reused addr\n"); 
        return -1; 
    } 

	if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char *)&iOption, sizeof(int)) < 0)
	{
		perror("tutk cannot socket to keep alive\n"); 
        return -1; 
	}

	if (setsockopt(s, SOL_TCP, TCP_KEEPIDLE, (const char *)&keep_idle, sizeof(int)) < 0)
	{
		perror("tutk cannot socket to keep alive\n"); 
        return -1; 
	}

	if (setsockopt(s, SOL_TCP, TCP_KEEPINTVL, (const char *)&keep_interval, sizeof(int)) < 0)
	{
		perror("tutk cannot socket to keep alive\n"); 
        return -1; 
	}

	if (setsockopt(s, SOL_TCP, TCP_KEEPCNT, (const char *)&keep_count, sizeof(int)) < 0)
	{
		perror("tutk cannot socket to keep alive\n"); 
        return -1; 
	}

	

	
    
    memset((char *) &si_me, 0, sizeof(si_me));
    memset(recvBuf, 0, TCP_RECV_BUFFER_SIZE);
    
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(TUTK_TCP_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, &si_me, sizeof(si_me)) < 0)
    {
        perror("tutk cannot flood mesh TCP socket bind failed\n"); 
        return -1; 
    }

    pBridge_info->tutk_tcpServ = s;

   /* 
      * listen: make this socket ready to accept connection requests 
      */
    if (listen(s, 5) < 0) /* allow 1 requests to queue up */ 
    {
		perror("tutk ERROR on listen");
    }

  
	pBridge_info->tutk_tcp_thread_run = 1;

    APP_INFO0("tutk TCP server started...\r\n");

 	
    while (pBridge_info->tutk_tcp_thread_run == 1) {
		int clientlen = sizeof(pBridge_info->tutk_tcpClient);

        /* 
              * accept: wait for a connection request 
              */
        int childfd = accept(pBridge_info->tutk_tcpServ, (struct sockaddr *) &pBridge_info->tutk_tcpClient, &clientlen);
        if (childfd < 0) 
	    {
	      perror("tutk ERROR on accept");
	    }
		pBridge_info->tutk_tcpChildFd = childfd;

		APP_INFO0("tutk TCP server connected...\r\n");
    
    /* 
        * gethostbyaddr: determine who sent the message 
        
	    hostp = gethostbyaddr((const char *)&pBridge_info->tcpClient.sin_addr.s_addr, sizeof(pBridge_info->tcpClient.sin_addr.s_addr), AF_INET);
	    if (hostp == NULL)
	    {
	      perror("ERROR on gethostbyaddr");
	    }
	    hostaddrp = inet_ntoa(clientaddr.sin_addr);
	    if (hostaddrp == NULL)
	    {
	      perror("ERROR on inet_ntoa\n");
	    }
	    printf("server established connection with %s (%s)\n", hostp->h_name, hostaddrp);

	*/
    
	    while (pBridge_info->tutk_tcp_thread_run == 1)
	    {
	        len = recv(pBridge_info->tutk_tcpChildFd, recvBuf, TCP_RECV_BUFFER_SIZE, 0);

	        APP_INFO0("tutk tcp server received data ...\r\n");

	        if (len > 0)
	        {            
	            //parse_incomming_data(recvBuf, len);  
	            app_qiwo_cmd_parser(recvBuf, len);
	        }
	        else
	        {
				APP_INFO0("tutk tcp server disconnected ...\r\n");
				close(pBridge_info->tutk_tcpChildFd);
				pBridge_info->tutk_tcpChildFd = 0;

				if (len == 0)    APP_INFO0("reason: tutk socket closed\r\n");
				else if (len == -1)  APP_INFO0("reason: tutk sokcet error\r\n");
				
	            break;
	        }
	    }

    }

	//close(pBridge_info->tutk_tcpServ);

	APP_INFO0("tutk tcp server end...\r\n");

    return 0;
}

static void * init_bridge_intf_2_cloud(void *temp)
{
    APP_INFO0("tcp server init...\r\n");
    init_recv_sm();
    bridge_start_tcp_server();

	return 0;
}

static void * tutk_init_bridge_intf_2_cloud(void *temp)
{
    APP_INFO0("tutk tcp server init...\r\n");
    tutk_bridge_start_tcp_server();

	return 0;
}

void start_tcp_task()
{   
    int err = pthread_create(&pBridge_info->tcp_server_task, NULL, &init_bridge_intf_2_cloud, NULL);
    if (err != 0)
        printf("\ncan't create thread :[%s]", strerror(err));
    else
        printf("\n Thread created successfully\n");

	/*while(1){
		if (pBridge_info->tcpChildFd == 0)
		{
                    GKI_delay(100);
			continue;
		}
		int ret = send(pBridge_info->tcpChildFd,"qiwo send to q6 data",21,0);
		printf("app_qiwo send to q6 OK ...ret=%d\n",ret);
		GKI_delay(3000);
		} */ //for socket test zhifeng
#ifdef USE_TWO_SOCKET
    err = pthread_create(&pBridge_info->tutk_tcp_server_task, NULL, &tutk_init_bridge_intf_2_cloud, NULL);
    if (err != 0)
        printf("\ncan't create tutk thread :[%s]", strerror(err));
    else
        printf("\n tutk thread created successfully\n");
#endif
}

void end_tcp_task()
{
	pBridge_info->upd_thread_run = 0;
	close(pBridge_info->tcpServ);
	//pthread_join(&pBridge_info->udp_server_task, NULL);

#ifdef USE_TWO_SOCKET
    close(pBridge_info->tutk_tcpServ);
    pBridge_info->tutk_tcp_thread_run = 0;
#endif
}
	

