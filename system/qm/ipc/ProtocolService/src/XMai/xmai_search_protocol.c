#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "xmai_search_protocol.h"
#include "xmai_common.h"
extern int g_nw_search_run_index;

static int XMaiMakeMsgUseBuf(char *fSendbuf, int buf_size, short messageid, char *jsonMsg, int fSessionInt, int fPeerSeqNum)
{
    int xmaiMsgLen = 0;
	memset(fSendbuf, 0, buf_size);
    XMaiMsgHeader *pXMaiMsgHeader = (XMaiMsgHeader *)fSendbuf;
    
    pXMaiMsgHeader->headFlag = 0xFF;
    pXMaiMsgHeader->version = 0x01;
    pXMaiMsgHeader->reserved1 = 0x00;
    pXMaiMsgHeader->reserved2 = 0x00;
    pXMaiMsgHeader->sessionId = fSessionInt;
    pXMaiMsgHeader->sequenceNum = fPeerSeqNum;
    pXMaiMsgHeader->totalPacket = 0x00;
    pXMaiMsgHeader->curPacket = 0x00;
    pXMaiMsgHeader->messageId = messageid;
    pXMaiMsgHeader->dataLen = strlen(jsonMsg);

	if(pXMaiMsgHeader->dataLen + 20 > buf_size)
	{
		//XMAIERR("send buff too small. send jsonMsg: %s\n", jsonMsg);			
		//XMAIERR("send buf size = %d, send msg len = %d \n", buf_size, (pXMaiMsgHeader->dataLen + 20));
		return -1;
	}

    sprintf(fSendbuf + 20, "%s", jsonMsg);
    xmaiMsgLen = 20 + strlen(jsonMsg);

    return xmaiMsgLen;
}

static int XMaiMsgBoardCastSend(int fSockSearchfd, const char* inData, const int inLength)
{
	int ret = 0;
	int sendlen = 0;
    struct sockaddr_in addr;

    memset(&addr, 0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("255.255.255.255");
    addr.sin_port = htons(XMAI_BROADCAST_SEND_PORT);

	//因为UDP 发送时，没有缓冲区，故需要对发送后的返回值进行判断后，多次发送
	while(sendlen < inLength)
	{
	    ret = sendto(fSockSearchfd, inData + sendlen, inLength - sendlen, 0, (struct sockaddr*)&addr,  sizeof(addr));
		if(ret < 0)
		{
			//perror("Send error");
			//非阻塞才有 EAGAIN
			if (errno != EINTR && errno != EAGAIN )
			{
				XMAIERR("Send() socket %d error :%s", fSockSearchfd, strerror(errno));
				return -1;
			}
			else
				continue;
		}
		sendlen += ret;
	}	
	return sendlen;
}

static int XMaiSearchMsgSend(char *jsonMsg,cJSON *root,short messageid,int socketfd)
{
	//打包	  
	char buf[XMAI_MSG_SEND_BUF_SIZE] = {0};
	int xmaiMsgLen = XMaiMakeMsgUseBuf(buf, sizeof(buf), IPSEARCH_RSP, jsonMsg, 0, 0);
	g_nw_search_run_index = 410;

	free(jsonMsg);
	cJSON_Delete(root);
	if(xmaiMsgLen < 0)
	{
		XMAIERR();
		return -1;
	}
	g_nw_search_run_index = 411;

	int ret = XMaiMsgBoardCastSend(socketfd, buf, xmaiMsgLen);
	if(ret < 0)
	{
		XMAIERR("Stopsession close socket(%d)![send_ret:%d]", socketfd, ret);
		return -1;
	}
	g_nw_search_run_index = 412;

	return 0;
}

static int XMaiIpSearchHandle(int fSockSearchfd, char* msg)
{
	cJSON *root, *NetCommon;
	char csHostIP[30] = {0};
	char csGateWay[30] = {0};
	char csMAC[30] = {0};
	char csSubmask[30] = {0};
	g_nw_search_run_index = 406;

	//获取本机网络参数，
	QMAPI_NET_NETWORK_CFG st_ipc_net_info;
	memset(&st_ipc_net_info,0,sizeof(st_ipc_net_info));
	if(0 != QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &st_ipc_net_info, sizeof(st_ipc_net_info)))
	{
	   XMAIERR("QMAPI_SYSCFG_GET_NETCFG fail");
	   return -1;
	}	
	//获取无线网络参数，
	QMAPI_NET_WIFI_CONFIG st_wifi_net_info;
	memset(&st_wifi_net_info,0,sizeof(st_wifi_net_info));
	if(0 != QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_WIFICFG, 0, &st_wifi_net_info, sizeof(st_wifi_net_info)))
	{
	   XMAIERR("QMAPI_SYSCFG_GET_WIFICFG fail");
	   return -1;
	} 

	//如果无线可用就只传无线的配置
	if(st_wifi_net_info.bWifiEnable && st_wifi_net_info.byStatus == 0)
	{
		sprintf(csHostIP, "0x%08x", inet_addr(st_wifi_net_info.dwNetIpAddr.csIpV4));
		sprintf(csGateWay, "0x%08x", inet_addr(st_wifi_net_info.dwGateway.csIpV4));
		sprintf(csSubmask, "0x%08x", inet_addr(st_wifi_net_info.dwNetMask.csIpV4)); 
	}
	else
	{
		sprintf(csHostIP, "0x%08x", inet_addr(st_ipc_net_info.stEtherNet[0].strIPAddr.csIpV4));
		sprintf(csGateWay, "0x%08x", inet_addr(st_ipc_net_info.stGatewayIpAddr.csIpV4));
		sprintf(csSubmask, "0x%08x", inet_addr(st_ipc_net_info.stEtherNet[0].strIPMask.csIpV4));	
	}
	sprintf(csMAC, "%02x:%02x:%02x:%02x:%02x:%02x", 
						st_ipc_net_info.stEtherNet[0].byMACAddr[0],
						 st_ipc_net_info.stEtherNet[0].byMACAddr[1],
						  st_ipc_net_info.stEtherNet[0].byMACAddr[2],
						   st_ipc_net_info.stEtherNet[0].byMACAddr[3],
							st_ipc_net_info.stEtherNet[0].byMACAddr[4],
							 st_ipc_net_info.stEtherNet[0].byMACAddr[5]);	   
	g_nw_search_run_index = 407;

	root = cJSON_CreateObject();//创建项目
	cJSON_AddStringToObject(root, "Name", "NetWork.NetCommon");
	cJSON_AddNumberToObject(root, "Ret", 100);
	cJSON_AddStringToObject(root, "SessionID", "0x00000000");
	cJSON_AddItemToObject(root,"NetWork.NetCommon", NetCommon = cJSON_CreateObject());
	cJSON_AddStringToObject(NetCommon, "GateWay", csGateWay);//
	cJSON_AddStringToObject(NetCommon, "HostIP", csHostIP);//
	cJSON_AddStringToObject(NetCommon, "HostName", "LocalHost");
	cJSON_AddNumberToObject(NetCommon, "HttpPort", st_ipc_net_info.wHttpPort);
	cJSON_AddStringToObject(NetCommon, "MAC", csMAC);//
	cJSON_AddNumberToObject(NetCommon, "MaxBps", 80);
	cJSON_AddStringToObject(NetCommon, "MonMode", "TCP");
	cJSON_AddNumberToObject(NetCommon, "SSLPort", XMAI_SSL_PORT);
	cJSON_AddStringToObject(NetCommon, "Submask", csSubmask);//
	cJSON_AddNumberToObject(NetCommon, "TCPMaxConn", XMAI_MAX_LINK_NUM);
	cJSON_AddNumberToObject(NetCommon, "TCPPort", XMAI_TCP_LISTEN_PORT);
	cJSON_AddStringToObject(NetCommon, "TransferPlan", "Quality");
	cJSON_AddNumberToObject(NetCommon, "UDPPort", XMAI_UDP_PORT);
	cJSON_AddFalseToObject(NetCommon, "UseHSDownLoad");
	g_nw_search_run_index = 408;
	char *out = cJSON_PrintUnformatted(root);
	g_nw_search_run_index = 409;

	int ret = XMaiSearchMsgSend(out,root,IPSEARCH_RSP,fSockSearchfd);

	return ret;
}

//解析从NVR传递过来的网络信息，如IP地址，子网掩码，网关
static int XMaiGetNetInfo(char *resultBuf, char *sourceBuf)
{	
    struct in_addr in;   
    memset(&in, 0, sizeof(in));
    
    char *p = strstr(sourceBuf, "0x");
    if(p == NULL){
        XMAIERR("error p == NULL \n");
        return -1;
    }
    p += strlen("0x");

    in.s_addr = strtoul(p, NULL, 16); 

    sprintf(resultBuf, "%s", inet_ntoa(in));

	return 0;
}

static int XMaiNetInfoHandle(cJSON *json)
{
    if(json == NULL)
    {
    	XMAIERR();
        return -1;
    }
	cJSON *element;    
    char HostIP[30] = {0};
    char Submask[30] = {0};
	char GateWay[30] = {0};
	char Mac[30] = {0};
	char csMAC[30] = {0};

    element = cJSON_GetObjectItem(json, "HostIP");
    if(element && element->valuestring)
        XMaiGetNetInfo(HostIP, element->valuestring);
	
    element = cJSON_GetObjectItem(json, "Submask");
    if(element && element->valuestring)
        XMaiGetNetInfo(Submask, element->valuestring);

    element = cJSON_GetObjectItem(json, "GateWay");
    if(element && element->valuestring)
        XMaiGetNetInfo(GateWay, element->valuestring);

    element = cJSON_GetObjectItem(json, "MAC");
    if(element && element->valuestring)
    	sprintf(Mac, "%s", element->valuestring);
	
	//获取本机网络参数，
    QMAPI_NET_NETWORK_CFG st_ipc_net_info;
	memset(&st_ipc_net_info,0,sizeof(st_ipc_net_info));
    if(0 != QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &st_ipc_net_info, sizeof(st_ipc_net_info)))
    {
       XMAIERR("QMAPI_SYSCFG_GET_NETCFG fail\n");
       return -1;
    } 

	//通过MAC地址判断是不是发给自己的包
    sprintf(csMAC, "%02x:%02x:%02x:%02x:%02x:%02x", 
						st_ipc_net_info.stEtherNet[0].byMACAddr[0],
                         st_ipc_net_info.stEtherNet[0].byMACAddr[1],
                          st_ipc_net_info.stEtherNet[0].byMACAddr[2],
                           st_ipc_net_info.stEtherNet[0].byMACAddr[3],
                            st_ipc_net_info.stEtherNet[0].byMACAddr[4],
                             st_ipc_net_info.stEtherNet[0].byMACAddr[5]); 

	if(strcmp(Mac, csMAC) != 0)
	{
		XMAINFO("NVR Set IP To Other IPCAM.\n");
		return 1; //没有修改IP，返回1
	}
	else
	{
		XMAINFO("NVR Set IP To Me:\n");
	}

	
	//修改
	
	st_ipc_net_info.byEnableDHCP = 0;	//强制改成固定IP ?
	strncpy(st_ipc_net_info.stEtherNet[0].strIPAddr.csIpV4, HostIP, sizeof(st_ipc_net_info.stEtherNet[0].strIPAddr.csIpV4));
	strncpy(st_ipc_net_info.stEtherNet[0].strIPMask.csIpV4, Submask, sizeof(st_ipc_net_info.stEtherNet[0].strIPMask.csIpV4));
	strncpy(st_ipc_net_info.stGatewayIpAddr.csIpV4, GateWay, sizeof(st_ipc_net_info.stGatewayIpAddr.csIpV4));
	
	//设置
    if(0 != QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_NETCFG, 0, &st_ipc_net_info, sizeof(st_ipc_net_info)))
    {
       XMAIERR("QMAPI_SYSCFG_GET_NETCFG fail");
       return -1;
    } 

	XMAINFO("XMai NVR modify IP OK:%s\n",st_ipc_net_info.stEtherNet[0].strIPAddr.csIpV4);
	return 0; ////修改IP成功，返回0
}

static int XMaiResIPModify(int fSockSearchfd, int status)
{
    cJSON *root = cJSON_CreateObject();//创建项目
    if(status == 0)
    	cJSON_AddNumberToObject(root, "Ret", 100);
	else
		cJSON_AddNumberToObject(root, "Ret", 101);
	
    cJSON_AddStringToObject(root, "SessionID", "0x00000000");
    
    char *out = cJSON_PrintUnformatted(root);

	int ret = XMaiSearchMsgSend(out,root,IP_SET_RSP,fSockSearchfd);
		
    return ret;
}

static int XmaiIpModifHandle(int fSockSearchfd, char* msg)
{
	XMaiMsgHeader *pXMaiMsgHeader = (XMaiMsgHeader *)msg;
    cJSON *json = NULL;
    char *out = NULL;
	int ret = 0;

    if(pXMaiMsgHeader->dataLen > 0)
	{	
	    json = cJSON_Parse(msg + 20);
	    if (!json)
		{
	        XMAIERR("Error before: [%s]", cJSON_GetErrorPtr());
	        return -1;
	    }
	    
	    out = cJSON_PrintUnformatted(json);

		//处理
		//ret 如果为0，则表示成功修改了IP，
		//ret 如果为1，则表示不是修改本机的IP
		//ret 如果为-1，则表示设置的IP不合理,或者出错
		ret = XMaiNetInfoHandle(json);
		
	    cJSON_Delete(json);
	    free(out);
    }
	
    //回应
    XMaiResIPModify(fSockSearchfd, ret);

	if(ret == 0)
	{
	    int ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_SET_SAVECFG_ASYN, 0, 0, 0); 
	    if(0 != ret)
	    {
	        XMAIERR();
	        return -1;        
	    }
	}

	return 0;
}

static int XMaiSearchMsgParse(int fSockSearchfd, char *msg)
{
    XMaiMsgHeader *pXMaiMsgHeader = (XMaiMsgHeader *)msg;

	g_nw_search_run_index = 405;
	switch(pXMaiMsgHeader->messageId)
	{
		case IPSEARCH_REQ:
			XMaiIpSearchHandle(fSockSearchfd, msg);
			break;
		case IPSEARCH_RSP:  // 广播后得到的命令
			break;
		case IP_SET_REQ:  
			XmaiIpModifHandle(fSockSearchfd, msg);
			break;			
		case IP_SET_RSP:  			
			break;				 				  
        default:
            XMAIERR("*** udp, unknown msg id:%d ***\n", pXMaiMsgHeader->messageId);
            break;
    }
    
    return 0;
}

void XMaiSearchSendEndMsg(int sockfd)
{
	if (sockfd > 0)
	{
		// 发送 shutdown
		char send_msg[] = "shutdown";
    	int send_len = strlen(send_msg);
		XMaiMsgBoardCastSend(sockfd, send_msg, send_len);
	}
}

int XMaiSearchRequestProc(int dmsHandle, int sockfd, int sockType)
{
	int packetLen = 0;	  
	XMaiMsgHeader *pXMaiMsgHeader = NULL;
	
	char buf[4096] = {0};
	char bufRecv[4096];

	memset(bufRecv, 0, sizeof(bufRecv));

	struct sockaddr_in from;
	bzero(&from, sizeof(struct sockaddr_in));

	g_nw_search_run_index = 400;
	//接收发来的消息
	int len = sizeof(struct sockaddr_in);
	int recvLen = recvfrom(sockfd, bufRecv, sizeof(bufRecv)-1, 0, (struct sockaddr *)&from, (socklen_t *)&len);
	if(recvLen < 0)
	{
		XMAIERR("recvfrom error:%s", strerror(errno));
		return -1;
	}
	g_nw_search_run_index = 401;

	char *p = bufRecv;

	while(p != (bufRecv + recvLen))
	{
		g_nw_search_run_index = 402;
		if(p[0] == 0xFF)//缺少强壮的纠错
		{
			g_nw_search_run_index = 403;
		    if(p+sizeof(XMaiMsgHeader) > (bufRecv + recvLen))
		    {
		        XMAIERR("recv data error, p:%p  sizeof(XMaiMsgHeader):%d, bufRecv:%p, recvLen:%d", p, sizeof(XMaiMsgHeader), bufRecv, recvLen);
		        return -1;
            }
			pXMaiMsgHeader = (XMaiMsgHeader *)p;
			packetLen = sizeof(XMaiMsgHeader) + pXMaiMsgHeader->dataLen;
            if(p+packetLen > (bufRecv + recvLen))
		    {
		        XMAIERR("recv data error, p:%p  packetLen:%d, bufRecv:%p, recvLen:%d", p, packetLen, bufRecv, recvLen);
		        return -1;
            }
			memset(buf, 0, sizeof(buf));
			memcpy(buf, p, packetLen);

			g_nw_search_run_index = 404;
			//解析消息包
			XMaiSearchMsgParse(sockfd, buf);
			p += packetLen;
		}
		else
		{
			p++;
		}
	}
	
    return 0;  
}

int XMaiCreateSock(int *pSockType)
{
    int sockSvr = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockSvr < 0)
	{
        XMAIERR("socket fail, error:%s", strerror(errno));
        goto cleanup;
    }    

	//设置该套接字为广播类型 
    int opt = 1;
    int ret = setsockopt(sockSvr, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    if(ret < 0)
	{
        XMAIERR("setsockopt SO_BROADCAST fail, error:%s", strerror(errno));
        goto cleanup;
    }

	struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(XMAI_BROADCAST_RCV_PORT); 
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定该套接字
    ret = bind(sockSvr, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0)
	{
        XMAIERR("bind  fail, error:%s", strerror(errno));
        goto cleanup;
    }

	if (pSockType)
	{
		*pSockType = 0;
	}
    return sockSvr;

cleanup:
    if(sockSvr >= 0)
        close(sockSvr);
	
    return -1;	
}
