
#include "tlnetin.h"
#include "tlthrough.h"
#include "tlsession.h"

#ifdef WIN32
#define		pthread_create		CreateThread
#define		pthread_mutex_init	CreateMutex
#endif

// define command used in udp socket link
typedef enum
{
    BaseUdpCmd      = 10000,
    BroadcastCmd    = BaseUdpCmd + 1,               /* broadcast */

    NVReqTalkback   = BaseUdpCmd + 2,
    NVReqLogon      = BaseUdpCmd + 10,
    NVReqAVdata     = BaseUdpCmd + 11,
    NVReqTalkback2  = BaseUdpCmd + 12,

}nvsUdp_t;

typedef struct tagTLNV_BROADCAST_PACKET
{
    nvsUdp_t            CommandType;
    DWORD               dwServerType;
    DWORD               dwServerIp;
    WORD                wServerDataPort;
    WORD                wServerWebPort; 
    char                csServerName[32];
    BYTE                bServerMac[8];
    BYTE                bChannelCount;
    BYTE                bAlarmInCount;
    BYTE                bAlarmOutCount;
    BYTE                bChanneNo;
    DWORD               dwReserve[4];
}TLNV_BROADCAST_PACKET;

void InitSendMessageData(TLNV_BROADCAST_PACKET *lpHostMsg,int nCmdType)
{
	QMAPI_NET_NETWORK_CFG ninfo;
	TLNV_SERVER_INFO sinfo;
	QMAPI_NET_DEVICE_INFO dinfo;
	
	memset(lpHostMsg, 0, sizeof(TLNV_BROADCAST_PACKET));
	lpHostMsg->CommandType	= nCmdType;
	lpHostMsg->dwServerType = IPC_FLAG;
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &ninfo, sizeof(QMAPI_NET_NETWORK_CFG)) == QMAPI_SUCCESS)
	{
		lpHostMsg->dwServerIp = TL_IpValueFromString(ninfo.stEtherNet[0].strIPAddr.csIpV4);
		lpHostMsg->wServerDataPort = ninfo.stEtherNet[0].wDataPort;
		lpHostMsg->wServerWebPort  = ninfo.wHttpPort;
		memcpy(lpHostMsg->bServerMac, ninfo.stEtherNet[0].byMACAddr, 6);
	}

   	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_TLSERVER, 0, &sinfo, sizeof(TLNV_SERVER_INFO)) == QMAPI_SUCCESS)
   	{
		lpHostMsg->bChannelCount = sinfo.wChannelNum;
		lpHostMsg->bAlarmInCount = sinfo.dwAlarmInCount;
		lpHostMsg->bAlarmOutCount = sinfo.dwAlarmOutCount;
	}
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, &dinfo, sizeof(QMAPI_NET_DEVICE_INFO)) == QMAPI_SUCCESS)
	{
		memcpy(lpHostMsg->csServerName, dinfo.csDeviceName,32);
	}	
	lpHostMsg->bServerMac[6] = 0;
};


int ProcessRemoteRequest(DWORD dwRemoteIp,WORD wRemotePort,int sockfd)
{
	listen_ret_t		   *plisten_ret;

	if(sockfd < 0 || sockfd > QMAPI_MAX_SYS_FD)
	{
		err();
		return -1;
	}
	plisten_ret = (listen_ret_t *)malloc(sizeof(listen_ret_t));
	plisten_ret->client_addr.sin_port = wRemotePort;
	plisten_ret->client_addr.sin_addr.s_addr = dwRemoteIp;
	plisten_ret->bIsThread = FALSE;
	plisten_ret->accept_sock = sockfd;
	plisten_ret->resverd = 0;
	printf("access_listen_ret_thread \n");
	if(access_listen_ret_thread(plisten_ret) < 0)
		return -1;

	printf("access_listen_ret_thread ret \n");
    return 0;
}

void TcpListenNatThread(void)
{
	COMM_HEAD		commHead;
	int				nIntervalTime;
	prctl(PR_SET_NAME, (unsigned long)"TcpListenNatThread", 0, 0, 0);
	memset(&commHead, 0, sizeof(COMM_HEAD));
	commHead.nFlag = TLNET_FLAG;
	commHead.nCommand = NETCMD_KEEP_ALIVE;
	commHead.nBufSize = 0;
	while(1)
	{
	// need fixme yi.zhang
	// QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_RESP, 0, &upgrade, sizeof(QMAPI_NET_UPGRADE_RESP)) == QMAPI_SUCCESS
		//if (g_tl_updateflash.bUpdatestatus)
		//	return ;

	//	if(g_tl_globel_param.TL_Notify_Servers.bEnable)
		{
	//		if (TcpSendMessageToRemote(&commHead, sizeof(COMM_HEAD)) <= 0)
			{
				sleep(1);				
				continue;
			}			
		}
	//	nIntervalTime = g_tl_globel_param.TL_Notify_Servers.dwTimeDelay;
		if (nIntervalTime < 1 || nIntervalTime > 65535)
			nIntervalTime = 30;
		sleep(nIntervalTime);
	}
}

int CreateBackConnectThread(int EnableTcp)
{
	int ret = 0;
	pthread_t		g_listenNatThreadID;
	REMOTE_HOST_DIR.tcpLocalSock = -1;
	pthread_mutex_init(&REMOTE_HOST_DIR.dirMutex, NULL);
	if(EnableTcp)
		ret = pthread_create(&g_listenNatThreadID, NULL, (void*)&TcpListenNatThread, NULL);
	else
		ret = pthread_create(&g_listenNatThreadID, NULL, (void*)&udpListenNatThread, NULL);
	if (0 != ret)
	{
		err();
		return 0;
	}
	
	return 1;
}

int InitUDPRemoteDir(DWORD ip,WORD port)
{

	int ret = -1;
	int hSock = -1;
	remoteHostDir_t dirData;
	unsigned short	 tmpPort = 0;
	
	if (REMOTE_HOST_DIR.ip == ip && REMOTE_HOST_DIR.port == port)
	{
		printf("InitUDPRemoteDir: the ip has exist\n");
		return 0;
	}
	
	dirData.status = LOGO_READY;
	dirData.ip = ip;
	dirData.port = port;
	
	ret = getUdpMsgSock(&hSock, &tmpPort);
	if (ret <= 0)
	{
		return 0;
	}
	dirData.hSock = hSock;
	dirData.hPort = tmpPort;
	
	printf("InitUDPRemoteDir: hSock: %d, hPort: %d\n",dirData.hSock,dirData.hPort);
	
	if (SetRemoteHostDir(dirData) < 0)
	{
		close(hSock);
		return 0;
	}
	
	return 1;	
}

int SetRemoteHostDir(remoteHostDir_t dirData)
{
	int i = 0;
	
	delRemoteDir(i);
	pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);
	memcpy(&REMOTE_HOST_DIR, &dirData, sizeof(remoteHostDir_t));
	REMOTE_HOST_DIR.hSock = -1;
	REMOTE_HOST_DIR.tcpLocalSock = -1;
	pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);
	return i;
}


int delRemoteDir(int pos)
{
	if (REMOTE_HOST_DIR.status > LOGO_OFF)
	{
		pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);
		if (REMOTE_HOST_DIR.hSock > 0)
		{
			close(REMOTE_HOST_DIR.hSock);
			REMOTE_HOST_DIR.hSock = -1;
		}
		
		REMOTE_HOST_DIR.status =  NO_LOGO;
		pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);
	}
	else
	{
		return 0;
	}
	return 1;
}

int getRemoteDir(int pos, remoteHostDir_t *remoteHostDir)
{
	if (NULL == remoteHostDir)
		return -1;
	memcpy(remoteHostDir, &REMOTE_HOST_DIR, sizeof(remoteHostDir_t));
	return 1;	             
}

int getRemotePos(int hScok, int *pos, remoteHostDir_t *remoteHostDir)
{
	int i = 0;
	int tmpSock = -1;
	
	tmpSock = hScok;
	
	if (NULL == remoteHostDir)
	{
		return 0;
	}
	

		if (REMOTE_HOST_DIR.status > LOGO_OFF)
		{
			if (tmpSock == REMOTE_HOST_DIR.hSock)
			{
				*pos = i;
				memcpy(remoteHostDir,&REMOTE_HOST_DIR,sizeof(remoteHostDir_t));
				return i;
			}
		}

	
	return -1;
}


int getReadyRemoteHost(int pos, remoteHostDir_t *remoteHostDir)
{
	if (NULL == remoteHostDir)
	{
		return 0;
	}
	
	if (REMOTE_HOST_DIR.status == LOGO_READY)
	{
		memcpy(remoteHostDir, &REMOTE_HOST_DIR, sizeof(remoteHostDir_t));
		return 1;
	}
	else	
	{
		return 0;
	}
}

int GetUdpMessagePort(void)
{
	static unsigned int seed = 7000;
	seed += 1;
	if (seed > 65535)
		seed = 7000;
	return seed;
}

int getUdpMsgSock(int *udpSock, unsigned short *port)
{
#ifndef TL_Q3_PLATFORM

	int	udpMsgPort;
	int	hSock;
	
	udpMsgPort = GetUdpMessagePort();
	hSock = UdpPortListen(INADDR_ANY, udpMsgPort);
	if (hSock < 0)
	{
		return 0;
	}
	
	*udpSock = hSock;
	*port = udpMsgPort;
#endif
	return 1;
}


int udpLinkRecv(int hSock)
{
	int					recvLen = -1;
	int					ret;
	int					addrLen ;
	OPEN_HEAD			openHead;
	CLIENT_INFO		clientInfo;
	fd_set				fset;
	//NET_DATA_HEAD	echo;
	struct timeval			to;
	struct sockaddr_in		addr;
	
	FD_ZERO(&fset);
	bzero(&to,sizeof(to));
	
	bzero(&openHead,sizeof(openHead));
	bzero(&clientInfo,sizeof(clientInfo));
	addrLen = sizeof(openHead);
	
	to.tv_sec = SELECT_WAIT_TIME;
	to.tv_usec = 0;
	FD_SET(hSock,&fset);
	ret = select(hSock+1,&fset,NULL,NULL,&to);
	if(g_serinfo.bServerExit)
	{
		err();
		return 0;
	}
	if(0 == ret)//time over
	{
		err();
		return 0;
	}
	if(-1 == ret)//socket error
	{
		err();
		return 0;
	}
	if (!FD_ISSET(hSock,&fset))
	{
		err();
		return 0;
	}
	
	addrLen = sizeof(addr);
	recvLen = recvfrom(hSock, &openHead, sizeof(openHead), MSG_WAITALL, (struct sockaddr *)&addr, (socklen_t *)&addrLen);
	
	if(recvLen <= 0)
	{
		err();
		return 0;
	}
	
	ConvertHead(&openHead,sizeof(openHead));
	
	if (TLNET_FLAG != openHead.nFlag)
	{
		err();
		return 0;
	}

	if ((openHead.nSerChannel < 0) || (openHead.nSerChannel >= QMAPI_MAX_CHANNUM))
	{		
		err();
		return 0;
	}
	if (!GetClient(openHead.nID,&clientInfo))
	{
		err();
		return 0;
	}
	return 1;
}

int udpLinkRemote(unsigned long ip, unsigned short port, void *data, int len)
{
	int ret;
	int localSock = -1;
	unsigned short localPort;
	int loopNum = 0;
	struct sockaddr_in addr;
	
	if (ip <= 0 || port <= 0 || NULL == data || len <= 0)
	{
		return 0;
	}
	
	if (getUdpMsgSock(&localSock, &localPort) <= 0)
	{
		return 0;
	}
	
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);
	
loop:
	ret = sendto(localSock, data, len, 0, (struct sockaddr*)&addr, sizeof(addr));
	if (ret <= 0)
	{
		return 0;
	}
	
	if (udpLinkRecv(localSock) <= 0)
	{
		if(loopNum >2)
		{
			close(localSock);
			return 0;	
		}
		loopNum++;
		goto loop;
	}
	return 1;	
}

int udpSendMsg(int pos, void *data, int len)
{
	int					ret = 0;
	int					hSock;
	struct sockaddr_in		addr;
	remoteHostDir_t		remoteHostDir;
	
	
	if (NULL == data || len <= 0)
	{
		return 0;
	}
	
	if (getRemoteDir(pos, &remoteHostDir) <= 0)
	{
		return 0;
	}
	
	if (remoteHostDir.status != LOGO_READY)
	{
		return 0;
	}
	
	hSock = REMOTE_HOST_DIR.hSock;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = remoteHostDir.ip;
	addr.sin_port = htons(remoteHostDir.port);
	
	ret = sendto(hSock,data,len,0,(struct sockaddr*)&addr,sizeof(addr));
	
#ifdef THRANSNAT_DEBUG
	printf("udpSendMsg: send to %s : %d data size is: %d\n", inet_ntoa(addr.sin_addr), htons(addr.sin_port), ret);
#endif
	
	return ret;
}

int udpSendMsgToAll(void *data, int len)
{

	
	if (NULL == data || len <= 0)
	{
		return 0;
	}
	udpSendMsg(0, data, len);

	return 1;
}


int getReadySock(int pos, int *hSock)//, struct sockaddr_in *addr)
{

	remoteHostDir_t remoteHostDir;
	if (getReadyRemoteHost(0, &remoteHostDir))
	{
		*hSock = remoteHostDir.hSock;
		return 1;
	}
	return 0;
}

int recvUdpMsg(fd_set fset)
{
	int			ret = 0;
	int			hSock, maxfd ,addrLen;
	int			sockfd_set;
	struct timeval          to;
	struct sockaddr_in      addr;
	remoteHostMsg_t		remoteHostMsg;
	int			serchfd = 0;
	
	FD_ZERO(&fset);
	bzero(&to,sizeof(to));
	addrLen = sizeof(struct sockaddr_in);
	maxfd = 0;
	
	sockfd_set = 0;

	
	to.tv_sec = SELECT_WAIT_TIME;
	to.tv_usec = 0;
	
	
	if (getReadySock(0, &hSock))
	{
		serchfd++;
		FD_SET(hSock,&fset);
		sockfd_set = hSock;
		if (hSock > maxfd)
		{
			maxfd = hSock;
		}
	}	

	
	if (serchfd == 0)
		return 0;
	serchfd = 0;
	
	ret = select(maxfd+1,&fset,NULL,NULL,&to);
	if(0 == ret) //time over
	{
		err();
		return 0;
	}
	if(-1 == ret) //socket error
	{
		err();
		return 0;
	}
	

	if(FD_ISSET(sockfd_set,&fset))
	{
		serchfd++;
	}

	
	if (serchfd == 0)
	{
		return 0;
	}
	
	ret = recvfrom(sockfd_set, &remoteHostMsg, sizeof(remoteHostMsg_t), 0, (struct sockaddr*)&addr, (socklen_t *)&addrLen);
	
#ifdef THRANSNAT_DEBUG
	printf("recv from: %s : %d data size is:%d, errno: %d\n", inet_ntoa(addr.sin_addr), htons(addr.sin_port), ret, errno);
#endif
	
	if (ret <= 0)
	{
		err();
		usleep(1000);
		return 0;
	}
	
	if(procRemoteCmd(remoteHostMsg) <= 0)
	{
		return 0;
	}
	
	return 1;
	
}

int connectRemoteHost(int *hSock, unsigned long ip, unsigned short port)
{
	int ret = -1;
	int tmpSock = -1;
	struct sockaddr_in addr;
	
	bzero(&addr, sizeof(addr));
    
	tmpSock = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);
	
	ret = connect(tmpSock, (struct sockaddr*)&addr, sizeof(addr));
	if (-1 == ret)
	{
		close(tmpSock);
		return 0;
	}
	
	printf("connectRemoteHost: connect ok\n");
	
	*hSock = tmpSock;
	
	return 1;
}


int procRemoteCmd(remoteHostMsg_t remoteHostMsg)
{
	int hTcpSock;
	unsigned long ip;
	unsigned short port;
	
	ip = remoteHostMsg.ip;
	port = remoteHostMsg.port;
	
	switch (remoteHostMsg.cmd)
	{
	case tcpLogon:
	case tcpAVdata:
	case tcpTalkback:
		{
			
#ifdef THRANSNAT_DEBUG
			printf("tcpLogon: tcpAVdata: tcpTalkback\n");
#endif
			
			if (port <= 0 || ip <= 0)
			{
				return 0;
			}
			if (connectRemoteHost(&hTcpSock, ip, port) <= 0)
			{
				return 0;
			}
			if (ProcessRemoteRequest(ip,port,hTcpSock) < 0)
			{
				return 0;
			}
		}
		break;
		
	case udpLogon:
		break;
		
	case udpAVdata:
		udpLinkRemote(ip, port, &remoteHostMsg, sizeof(remoteHostMsg_t));
		break;
		
	case udpTalkback:
		break;
		
	case udpLogoff:
		break;
		
	default:
		break;
	}
	
	return 1;
}

void udpListenNatThread(void)
{
	fd_set fset;
	int nIntervalTime;
	TLNV_BROADCAST_PACKET sendMsg;
	prctl(PR_SET_NAME, (unsigned long)"udpListenNatThread", 0,0,0);
	InitSendMessageData(&sendMsg,BroadcastCmd);
	while (1)
	{
		// need fixme yi.zhang
		//if (g_updateSystem)
		//	return ;
		udpSendMsgToAll(&sendMsg, sizeof(TLNV_BROADCAST_PACKET));
		recvUdpMsg(fset);
		//nIntervalTime = g_tl_globel_param.TL_Notify_Servers.dwTimeDelay;
		if (nIntervalTime <= 0 || nIntervalTime > 65535)
			nIntervalTime = 1;
		if (nIntervalTime != 1)
			sleep(nIntervalTime - 1);
		else
			sleep(30);
	}
}

int InitTcpRemoteDir(DWORD ip,unsigned char *strIp, WORD port)
{


	remoteHostDir_t dirData;
	pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);		
	if(REMOTE_HOST_DIR.tcpLocalSock > 0)
	{
		shutdown(REMOTE_HOST_DIR.tcpLocalSock, 2);
		close(REMOTE_HOST_DIR.tcpLocalSock);
		printf("InitTcpRemoteDir: REMOTE_HOST_DIR.tcpLocalSock = %d\n", REMOTE_HOST_DIR.tcpLocalSock);
		REMOTE_HOST_DIR.tcpLocalSock = -1;
		REMOTE_HOST_DIR.status = NO_LOGO;
		memset(REMOTE_HOST_DIR.strIp, 0, sizeof(REMOTE_HOST_DIR.strIp));
	}
	else
		printf("InitTcpRemoteDir: REMOTE_HOST_DIR.tcpLocalSock = %d\n", REMOTE_HOST_DIR.tcpLocalSock);
	
	pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);			
	dirData.status = LOGO_READY;
	dirData.ip = ip;
	dirData.port = port;
	strcpy((char*)dirData.strIp, (char*)strIp);
	if (SetRemoteHostDir(dirData) < 0)
	{
		err();
		//shutdown(dirData.hSock, 2);
		//close(dirData.hSock);
		return 0;
	}
	return 1;	
}

int getTcpMsgSock(int *tcpSock, unsigned short *port)
{
	int tcpMsgPort;
	int hSock;
	
	tcpMsgPort = GetUdpMessagePort();
	hSock = TcpSockListen(INADDR_ANY,tcpMsgPort);
	if(hSock < 0)
	{
		return 0;
	}
	
	*tcpSock = hSock;
	*port = tcpMsgPort;
	
	return 1;
}

int TcpConnectToRemote(const char*pSerIP, int inPort, int *sock)
{
    
	struct sockaddr_in addr;
	int		nRet = 0;
	unsigned long	ulServer;
	int		hSocket = 0;
	int		nOpt = 1;	
	int		error;
	struct hostent	*hostPtr;
	
	unsigned long 	non_blocking = 1;
	unsigned long 	blocking = 0;
	if(NULL == pSerIP)
	{
		err();
		return -1;
	}
	
	if((inPort < 1000) || (inPort > 65535))
	{
		err();
		return -1;
	}

	hostPtr = gethostbyname2(pSerIP,AF_INET);
	if (NULL == hostPtr)
	{
		printf("Get TL_Notify_Servers.szDNS IpAddress error(%d)\n",errno);
		return -1;
	}
	
	memcpy(&ulServer, hostPtr->h_addr_list[0], sizeof(unsigned long));
    REMOTE_HOST_DIR.ip = ulServer;
	hSocket = socket(AF_INET,SOCK_STREAM,0);
	if( hSocket < 0 )
	{
		err();
		return -1;
	}
	nRet = setsockopt(hSocket,IPPROTO_TCP,TCP_NODELAY,(char *)&nOpt,sizeof(nOpt));	
	nOpt = TRUE;
	nRet = setsockopt(hSocket,SOL_SOCKET,SO_REUSEADDR,(char *)&nOpt,sizeof(nOpt));
	nOpt = 2000;  
	nRet = setsockopt(hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&nOpt,sizeof(nOpt));	
	nRet = setsockopt(hSocket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nOpt,sizeof(nOpt));
	nOpt = 1024 * 32;
	nRet = setsockopt(hSocket,SOL_SOCKET,SO_SNDBUF,(char *)&nOpt,sizeof(nOpt));
	nOpt = 1024 * 32;
	nRet = setsockopt(hSocket,SOL_SOCKET,SO_RCVBUF,(char *)&nOpt,sizeof(nOpt));

	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)inPort);
	addr.sin_addr.s_addr = ulServer;
	ioctl(hSocket,FIONBIO,&non_blocking);
	nRet = connect(hSocket, (struct sockaddr*)&addr, sizeof(addr));
	if (nRet == -1 && errno == EINPROGRESS)
	{
		struct timeval tv; 
		fd_set writefds;
		
		tv.tv_sec = 10;
		tv.tv_usec = 0; 
		FD_ZERO(&writefds); 
		FD_SET(hSocket, &writefds); 
		if(select(hSocket+1,NULL,&writefds,NULL,&tv) != 0)
		{ 
			if(FD_ISSET(hSocket,&writefds))
			{
				int len=sizeof(error); 
				if(getsockopt(hSocket, SOL_SOCKET, SO_ERROR, (char *)&error, (socklen_t*)&len) < 0)
				{
					err();
					goto error_ret; 
				}
				if(error != 0) 
				{
					err();
					goto error_ret; 
				}
			}
			else
			{
				err();
				goto error_ret;
			}
		}
		else 
		{
			err();
			goto error_ret;
		}
		ioctl(hSocket, FIONBIO, &blocking);
		nRet = 0;
	}
	if(nRet != 0)
	{
		err();
		goto error_ret;
	}
	printf("sock is %d \n",hSocket);
	*sock = hSocket;
	return 1;
error_ret:
	shutdown(hSocket, 2);
	close(hSocket);
	err();
	return -1;
}

int SynConnect(int hSocket, unsigned long inIp, int inPort)
{
	struct sockaddr_in addr;
	int		nRet = 0;
	//unsigned long	ulServer;
	//SOCKET		hSocket = 0;
	int		nOpt = 0;	
	int		error;
	struct linger   ling;
	
	unsigned long 	non_blocking = 1;
	unsigned long 	blocking = 0;
	
	ling.l_onoff = 0;
	nRet = setsockopt(hSocket, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
	
	nOpt = TRUE;

	nRet = setsockopt(hSocket,SOL_SOCKET,SO_REUSEADDR,(char *)&nOpt,sizeof(nOpt));
	
	memset(&addr,0,sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons((u_short)inPort);
	addr.sin_addr.s_addr = inIp;

	printf("inPort = %d\n", inPort);
	
	ioctl(hSocket,FIONBIO,&non_blocking);
	
	nRet = connect(hSocket, (struct sockaddr*)&addr, sizeof(addr));
	if (nRet == -1 && errno == EINPROGRESS)
	{
		struct timeval tv; 
		fd_set writefds;
		
		tv.tv_sec = 5; //SOCK_CONNECT_OVER
		tv.tv_usec = 0; 
		FD_ZERO(&writefds); 
		FD_SET(hSocket, &writefds); 
		if(select(hSocket+1,NULL,&writefds,NULL,&tv) != 0)
		{ 
			if(FD_ISSET(hSocket,&writefds))
			{
				int len=sizeof(error); 
				if(getsockopt(hSocket, SOL_SOCKET, SO_ERROR
					,  (char *)&error, (socklen_t*)&len) < 0)
				{
					err();
					goto error_ret; 
				}
				
				if(error != 0) 
				{
					err();
					goto error_ret; 
				}
			}
			else
			{
				err();
				goto error_ret; /* timeout or error happend  */
			}
		}
		else 
		{
			err();
			goto error_ret;
		}
		
		ioctl(hSocket, FIONBIO, &blocking);
		nRet = 0;
	}
	
	if( nRet != 0 )
	{
		err();
		goto error_ret;
	}
	
	//*sock = hSocket;
	
	return 0;
	
error_ret:
	shutdown(hSocket, 2);
	close(hSocket);
	err();
	return -3;
}

int TcpReceive2(int hSock, char *pBuffer, unsigned long nSize)
{
	int		ret = 0;
	fd_set           fset;
	unsigned long	dwRecved = 0;
	struct timeval to;
	
	while(dwRecved < nSize)
	{
		FD_ZERO(&fset);
		FD_SET(hSock,&fset);
		to.tv_sec = 3;
		to.tv_usec = 0;	
		
		ret = select(hSock+1, &fset, NULL, NULL, &to);  //&to
		if (ret == 0)
		{
			return -2;
		}
		if (ret < 0 && errno == EINTR)
			continue;
		if (ret < 0)
		{
			err();
			return -1;
		}
		
		if(!FD_ISSET(hSock, &fset))
		{
			err();
			continue;
		}	
		ret = recv(hSock, pBuffer + dwRecved, nSize - dwRecved,0);
		if (ret < 0 && (errno == EINTR || errno == EAGAIN))
		{
			continue;
		}
		
		if (ret < 0)
		{
			err();
			return -1;
		}
		dwRecved += ret;
	}
	
	return dwRecved;
}

int ConnectForCreateCh(int nChannel, const char*pSerIP, int inPort, int *sock, int chType)
{
	TLNV_BROADCAST_PACKET	sendMsg;
	int			tmpSock;
	int			ret;
	
	InitSendMessageData(&sendMsg,chType);
	sendMsg.bChanneNo = nChannel;
	if (TcpConnectToRemote(pSerIP, inPort, &tmpSock) < 0)
	{
		return -1;
	}
	
	ret = tcpSendMsg(tmpSock, &sendMsg, sizeof(TLNV_BROADCAST_PACKET));
	if (ret <= 0 && errno == EPIPE)
	{
		printf("connectRemoteHost: tcpSendMsg error\n");
		shutdown(tmpSock, 2);
		close(tmpSock);
		return -1;
	}
	
	ret = ProcessRemoteRequest(REMOTE_HOST_DIR.ip,inPort,tmpSock);
	if (ret < 0)
	{
		printf("ConnectForCreateCh: ProcessRemoteRequest error\n");
		//shutdown(tmpSock, 2);
		//close(tmpSock);		
		return -1;
	}
	
	*sock = tmpSock;
	
	printf("[test]: ConnectForCreateCh OK!\n");
	
	return 0;
}

int TcpSendMessageToRemote(void *data, int len)
{
	int				ret = 0;
	TLNV_BROADCAST_PACKET	sendMsg;
	int				tmpsock;

	if (NULL == data || len <= 0)
	{
		return 0;
	}
	if (REMOTE_HOST_DIR.status != LOGO_ON)
	{
		if (REMOTE_HOST_DIR.port <= 0 || REMOTE_HOST_DIR.strIp[0] == 0)
		   	{
			printf("REMOTE_HOST_DIR.port == %d\n", REMOTE_HOST_DIR.port);
			printf("REMOTE_HOST_DIR.ip == %lu\n", REMOTE_HOST_DIR.ip);			
			   printf("BackConnect : No Set Remote Server Address\n");
			   return -1;
		   	}
		//inet_ntop(AF_INET, &REMOTE_HOST_DIR.ip, strIp, sizeof(strIp));
	//	ret = TcpConnectToRemote(g_tl_globel_param.TL_Notify_Servers.szDNS, g_tl_globel_param.TL_Notify_Servers.dwPort, &tmpsock);
	//	if(ret < 0)
		{
	//		printf("Connect To Client %s %d Failed\n",g_tl_globel_param.TL_Notify_Servers.szDNS,REMOTE_HOST_DIR.port);
			return -1;
		}
	//	else
	//		printf("Connect To Client %s %d Success\n",g_tl_globel_param.TL_Notify_Servers.szDNS, REMOTE_HOST_DIR.port);
		
		pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);		
		REMOTE_HOST_DIR.status = LOGO_ON;
		REMOTE_HOST_DIR.tcpLocalSock = tmpsock;
		pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);		
		InitSendMessageData(&sendMsg,NVReqLogon);
		ret = tcpSendMsg(REMOTE_HOST_DIR.tcpLocalSock, &sendMsg, sizeof(TLNV_BROADCAST_PACKET));
		if (ret <= 0 && errno == EPIPE)
		{
			printf("Connect To Client Send Message Failed\n");
			pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);		
			shutdown(REMOTE_HOST_DIR.tcpLocalSock, 2);
			close(REMOTE_HOST_DIR.tcpLocalSock);
			REMOTE_HOST_DIR.tcpLocalSock = -1;
			REMOTE_HOST_DIR.status = NO_LOGO;
			pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);		
			return -1;
		}
		if(ProcessRemoteRequest(REMOTE_HOST_DIR.ip,REMOTE_HOST_DIR.port,REMOTE_HOST_DIR.tcpLocalSock) < 0)
		{
			printf("ProcessRemoteRequest Failed tcpLocalSock:%d\n", REMOTE_HOST_DIR.tcpLocalSock);
			pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);		
			//shutdown(REMOTE_HOST_DIR.tcpLocalSock, 2);
			//close(REMOTE_HOST_DIR.tcpLocalSock);
			REMOTE_HOST_DIR.tcpLocalSock = -1;
			REMOTE_HOST_DIR.status = NO_LOGO;
			pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);		
			return -1;
		}
	}
	
	/* check command sock */
	if (REMOTE_HOST_DIR.tcpLocalSock < 0)
	{
		err();
		printf("REMOTE_HOST_DIR.tcpLocalSock < 0 \n");		///2009.8.1 zhang del = and ok
		pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);		
		REMOTE_HOST_DIR.tcpLocalSock = -1;
		REMOTE_HOST_DIR.status = NO_LOGO;
		pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);		
		return -1;
	}
	
	if (REMOTE_HOST_DIR.status == LOGO_OFF)
	{
		printf("REMOTE_HOST_DIR.status == LOGO_OFF \n");	///2009.8.1 zhang del ok
		err();
		pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);		
		REMOTE_HOST_DIR.tcpLocalSock = -1;
		REMOTE_HOST_DIR.status = NO_LOGO;
		pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);		
		return -1;
	}
	
#if 0
	/* send keep alive pack */
	ret = tcpSendMsg(REMOTE_HOST_DIR.tcpLocalSock, data, len);
	if (ret <= 0 && (errno == EPIPE || errno == ENOTSOCK || errno == EBADF))
	{
		printf("send alive package error errno = %d\n", errno);
		REMOTE_HOST_DIR.tcpLocalSock = -1;
		REMOTE_HOST_DIR.status = NO_LOGO;
	}
	else
	{
		printf("send alive package success\n");
	}	
#endif	
	return 1;
}

int tcpSendMsg(int hSock, void *data, int len)
{
	int		ret;
	
	if (NULL == data || len <= 0)
	{
		return 0;
	}	
	ret = Send(hSock, data, len);
	if (ret <= 0)
	{
		return -1;
	}
	return ret;
}

