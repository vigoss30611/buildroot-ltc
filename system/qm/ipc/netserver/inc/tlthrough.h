/******************************************************************************
******************************************************************************/

#ifndef _TLVIDEOTHROUGHNAT_H
#define _TLVIDEOTHROUGHNAT_H

#include "TLNetCommon.h"
//#include "tlnetinterface.h"
#include "tlnetin.h"

#define SELECT_WAIT_TIME		1

typedef enum
{
	NO_LOGO,
	LOGO_OFF,   
	LOGO_READY,
	LOGO_ON,
}remoteStatus_t;

typedef struct
{
	remoteStatus_t   	status;
	int             	hSock;
	unsigned short		hPort;
	pthread_mutex_t 	dirMutex;
	unsigned long    	ip;
	unsigned char		strIp[128];
	unsigned short   	port;
	int 				tcpLocalSock;
	int					intervalTime;
	int					reserve;
}remoteHostDir_t, *premoteHostDir_t;


typedef enum
{
	udpLogon,
	udpAVdata,
	udpTalkback,
	udpLogoff,
	tcpLogon,
	tcpAVdata,
	tcpTalkback,
	tcpLogoff,
	udpLink,	
}remoteCmdType_t;

typedef struct
{
	remoteCmdType_t	cmd;
	int				result;
	int				channelId;
	DWORD			ip;
	WORD			port;
	int				reserved;
}remoteHostMsg_t, *premoteHostMsg_t;


remoteHostDir_t REMOTE_HOST_DIR;

int CreateBackConnectThread(int EnableTcp);
int ProcessRemoteRequest(DWORD dwRemoteIp,WORD wRemotePort,int sockfd);
int SetRemoteHostDir(remoteHostDir_t dirData);
int delRemoteDir(int pos);
int getRemoteDir(int pos, remoteHostDir_t *remoteHostDir);
int getRemotePos(int hScok, int *pos, remoteHostDir_t *remoteHostDir);
int getReadyRemoteHost(int pos, remoteHostDir_t *remoteHostDir);
int GetUdpMessagePort(void);
int getUdpMsgSock(int *udpSock, unsigned short *port);
int udpSendMsg(int pos, void *data, int len);
int udpSendMsgToAll(void *data, int len);
int udpLinkRecv(int hSock);
int udpLinkRemote(unsigned long ip, unsigned short port, void *data, int len);
int getReadySock(int pos, int *hSock);
int recvUdpMsg(fd_set fset);
int tcpSendMsg(int hSock, void *data, int len);
int connectRemoteHost(int *hSock, unsigned long ip, unsigned short port);
int procRemoteCmd(remoteHostMsg_t remoteHostMsg);
void udpListenNatThread(void);

int InitUDPRemoteDir(unsigned long ip, unsigned short port);
int InitTcpRemoteDir(DWORD ip,unsigned char *strIp, WORD port);
int getTcpMsgSock(int *tcpSock, unsigned short *port);
int SynConnect(int hSocket, unsigned long inIp, int inPort);
int TcpSendMessageToRemote(void *data, int len);
int tcpSendMsg(int hSock, void *data, int len);
int TcpReceive2(int hSock, char *pBuffer, unsigned long nSize);
int ConnectForCreateCh(int nChannel, const char*pSerIP, int inPort, int *sock, int chType);

#endif
