/******************************************************************************
******************************************************************************/


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>

#include "QMAPIType.h"
#include "QMAPIDataType.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPIErrno.h"
#include "QMAPI.h"

#include "QMAPIRecord.h"

#include "TLNVSDK.h"
#include "TLNetCommon.h"

#ifndef _TL_VIDEO_INSIDE_
#define _TL_VIDEO_INSIDE_


#define		SOCK_SNDRCV_LEN		(1024*32)

#define     SOCK_TIME_OVER      30

#define     MAX_MEGA_POOL_SIZE         600      //6MB,4s
#define     MAX_SMALL_POOL_SIZE  100
#define     MAX_MOBILE_POOL_SIZE  100  
#define     MAX_POOL_SIZE          100

#define DATA_PACK_SIZE			(16 *1024)

#define     MAX_THREAD_NUM      5

#ifdef  PPC
static  inline unsigned long ArchSwapLE32(unsigned long X)
{
    return  ((X << 24) | ((X << 8)&0x00FF0000) | ((X >> 8)&0x0000FF00) | (X >> 24));
}
static  inline unsigned short ArchSwapLE16(unsigned short X)
{
    return (((X << 8)&0xFF00) | ((X >> 8)&0x00FF));
}
#else
#define		ArchSwapLE32(X)		(X)
#define		ArchSwapLE16(X)		(X)
#endif

//MSG_HEAD-nErrorCode:???????(??λ??)
#define SNAP_TO_FTP             0x0001//FTP???
#define SNAP_TO_EMAIL           0x0002//????EMAIL
#define SNAP_TO_ALL_CLIENT      0x0004//????????????????????????
#define SNAP_TO_SPECIAL_CLIENT  0x0008//??????????????
#define SNAP_TO_LOCAL           0x0010//??????SD??

typedef int  (*ServerRecv)(MSG_HEAD *pMsgHead,char *pRecvBuf);
typedef int  (*CheckUserPsw)(const char *pUserName,const char *pPsw);

///TL Video Add New Protocol Check User Name Password IP Mac
typedef int  (*TL_CheckUserPsw)(const char *pUserName,const char *pPsw , const int pUserIp , const int pSocketID , const char *pUserMac , int *UserRight , int *NetPreviewRight);

///TL Video Add New Protocol Get Server Info 
typedef int  (*TL_GetServerInfo)(char *pServerInfo);

///TL Video Add New Protocol Get Channel Info 
typedef int  (*TL_GetChannelInfo)(char *pChannelInfo , int tl_channel);

///TL Video Add New Protocol Get Sensor Info 
typedef int  (*TL_GetSensorInfo)(char *pSensorInfo , int tl_channel , int SensorType);

///TL Video Add New Protocol Cut User Info
typedef int  (*TL_CutUserInfo)(const int pSocketID);

///TL Video Add New Protocol Check Data Channel Open Right
typedef int  (*TL_DataChannelClientRight)(int nOpenChannel , int SocketID);


typedef void (*OnNetTimer)(int nSignal);

typedef int  (*ClientRequestTalk)(unsigned long ip,unsigned short port);

typedef int  (*ClientStreamTalk)(unsigned long ip, unsigned short port, char *pbuf, int len);


typedef enum _NODE_STATUS
{
    READY = 0,
    RUN = 1,
    STOP = 2,
    QUIT = 3
}NODE_STATUS;

typedef struct _CLIENT_NODE
{
	unsigned long   ip;
	unsigned long 	port;
	char		szUserName[QMAPI_NAME_LEN];
	char		szUserPsw[QMAPI_PASSWD_LEN];
	int		level;
	int		hUserID;
}CLIENT_NODE,*PCLIENT_NODE;

typedef struct _SEND_NODE
{
    PROTOCOL_TYPE       nType;
    int                 nSerChannel;

    int                 nClientID;//msg socket handle
    unsigned long       nClientIP;//just for udp and multicast
    unsigned long       nClientPort;//just for udp and multicast
    int                 nClientChannel;
	unsigned long       nClientTcpIP;

    int                 nStreamType;
    int                 hSendSock;//just for tcp
    int				ncork;			//for tcp cork control

    NODE_STATUS          status;

    struct   _SEND_NODE *pNext;
}SEND_NODE,*PSEND_NODE;

typedef struct _READ_POSITION
{
    int             nBegin;
    int             nEnd;
    BOOL            bLost;
   
}READ_POSITION,*PREAD_POSITION;


//UDPêy?Y·￠?í????
typedef struct _UDP_STREAM_SEND
{
	pthread_t       hUdpSendThreadID[QMAPI_MAX_CHANNUM];
	SEND_NODE       *pUdpSendList[QMAPI_MAX_CHANNUM];
	pthread_mutex_t hUdpSendListMutex[QMAPI_MAX_CHANNUM];
}UDP_STREAM_SEND,*PUDP_STREAM_SEND;

//?í?§??ó??§D??￠
typedef struct _CLIENT_INFO
{
	unsigned long   ip;
	unsigned long 	port;
	char			szUserName[QMAPI_NAME_LEN];
	char			szUserPsw[QMAPI_PASSWD_LEN];

	int				level;
	int				hMsgSocket;//msg socket
	int				hAlarmSocket;	//报警socket
	int				nAlarmFlag;	//是否使用
    int             nKeepAlive;
    NODE_STATUS     status;
	struct _CLIENT_INFO		*pNext;
}CLIENT_INFO,*PCLIENT_INFO;

//ó??§D??￠á?±íí·
typedef struct _CLIENT_HEAD
{
	int			nTotalNum;
	pthread_mutex_t hClientMutex;
	CLIENT_INFO		*pNext;
}CLIENT_HEAD,*PCLIENT_HEAD;

typedef struct _TALKTHRD_PAR
{
    int             hSock;
    unsigned long   ip;
    unsigned long   port;
}TALKTHRD_PAR;


typedef struct _SER_INFO
{

	int				nChannelNum;
	int				nBasePort;// void RequestTcpPlay(OPEN_HEAD openHead,int  hDataSock)
	char			szMultiIP[16];
	unsigned long 	multiAddr;
		
	int             nSendBufferNum;
	
	int				hListenSock;//TCP SOCKET
	int				hUdpPortSock;//UDP SOCKET

	pthread_t		hListenThread;//TCP Thread
	pthread_t		hUdpPortThread;//UDP Thread
	
	SEND_NODE       *pTcpSendList;//TCP·￠?í?úμ?
	pthread_mutex_t hTcpSendListMutex;//TCP·￠?í?úμ??￥3a
	
		//UDP,MULTIêy?Yá÷·￠?í,
	UDP_STREAM_SEND udpStreamSend1;//dual stream ,udp send stream 1
	UDP_STREAM_SEND udpStreamSend2;//dual stream ,udp send stream 2
	
	char            *pSendBuf;
	
	pthread_mutex_t sendMutex; 
	
	char            *pRecvBuf;
	char            *pUpdateFileBuf;
	
		//?á·￠?íêy?Y ????TSD
	pthread_key_t   hReadPosKey;
	
		//???￠??3ì1üàí
	pthread_t       msgThreadID[MAX_THREAD_NUM];
	pthread_mutex_t msgThreadMutex;
	pthread_cond_t  msgThreadCond;
	
	int             msgProcessThreadNum;
	int             msgWaitThreadNum;
	int             msgQuitThreadNum;

	pthread_t       msgQuitMangThread;
	pthread_mutex_t msgQuitMangMutex;
	pthread_cond_t  msgQuitMangCond;
	
		//TCPêy?Y·￠?íêy?Y1üàí
	pthread_t       dataThreadID[MAX_THREAD_NUM];
	pthread_mutex_t dataThreadMutex;
	pthread_cond_t	dataThreadCond;
	
	int             dataProcessThreadNum;
	int             dataWaitThreadNum;
	int             dataQuitThreadNum;
	
	pthread_t       dataQuitMangThread;
	pthread_mutex_t dataQuitMangMutex;
	pthread_cond_t  dataQuitMangCond;
	
	CheckUserPsw	funcCheckUserPsw;

	///TL Video Add New Protocol Check User Name Password IP Mac
	TL_CheckUserPsw		tlcheckUser;
	///TL Video Add New Protocol Get Server Info 
	TL_GetServerInfo	tlGetServerInfo;
	///TL Video Add New Protocol Get Channel Info 
	TL_GetChannelInfo	tlGetChannelInfo;
	///TL Video Add New Protocol Get Sensor Info 
	TL_GetSensorInfo	tlGetSensorInfo;
	///TL Video Add New Protocol Cut User Info
	TL_CutUserInfo		tlcutuserinfo;
	///TL Video Add New Protocol Check Data Channel Open Right
	TL_DataChannelClientRight	tlchannelrightcheck;

	ServerRecv		funcServerRecv;

	BOOL			bServerStart;
	BOOL			bServerExit;
    BOOL			bListenExit;
    
	int             nMsgThreadCount;
	int             nDataThreadCount;
	
	//talkback
	int             nAudioChannels;
	int             nAudioBits;
	int             nAudioSamples;
	//
	ClientRequestTalk   pCallbackRequestTalk;
	ClientStreamTalk    pCallbackStreamTalk;
	int                 		hTalkbackSocket;
	TALKTHRD_PAR      talkThrdPar;
	pthread_t   		thrdID;
}SER_INFO,*PSER_INFO;



typedef struct
{
	struct sockaddr_in			client_addr;
	int					accept_sock;
	int					resverd;
	BOOL				bIsThread;
}listen_ret_t, *plisten_ret_t;

//
//********************oˉêy??ò?

//----------êy?Y???￥

//????TCP?àìySOCKET
int		TcpSockListen(int ip,int port);
// ????UDP?àìy???úSOCKET
int		UdpPortListen(int ip,int port);

//TCP?àìy??3ì
void	ListenThread(void *pPar);
//UDP PORT?àìy??3ì
void	UdpPortListenThread(void *pPar);

//UDP,MULTICASTêy?Y·￠?í??3ì
void	UdpSendThread(void *pPar);

//TCP MSG?óê???3ì
void	TcpMsgRecvThread(void *pPar);
//TCP MSGí?3???3ì1üàí
void	TcpMsgQuitThread(void *pPar);

//TCP êy?Y·￠?í??3ì
void	NetServerTcpSendThread(void *pPar);
//TCP êy?Y·￠?í??3ìí?3?1üàí
void	TcpDataQuitThread(void *pPar);

//??àí?óê?μ?μ?D??￠?üá?
//BOOL    TranslateCommand(MSG_HEAD *pMsgHead,char *pRecvBuf);
int    TranslateCommand(MSG_HEAD *pMsgHead,char *pRecvBuf);

//éè??áa?óSOCKET
void	SetConnSockAttr(int hSock,int nTimeOver);

//-----------ó??§1üàí

//ó??§μ???
void        ClientLogon(struct sockaddr_in addr,USER_INFO userInfo,int nID,int nLevel);
//ó??§????
BOOL		ClientLogoff(int nID);

//μ?μ?ò???í?3?ó??§ID
int         GetExitClient();
//2é?òò????y?úμè?y??àíμ??í?§
CLIENT_INFO	*FindWaitProccessClient();
//í??ùIPoí???úμ??í?§ê?·?ò??-???ú
int         ClientExist(unsigned long ip,unsigned long port);
//?ù?YIDμ?μ?ó??§D??￠
BOOL        GetClient(int nID,CLIENT_INFO *pClientInfo);
//???ùóD?í?§?úμ?éè???aí?3?×?ì?
int         StopAllClient();
//êí·??ùóD?í?§?úμ?
int         FreeAllClient();
//
//void        CheckClientAlive();



/*                    TCP */
void        RequestTcpPlay(OPEN_HEAD openHead,int  hDataSock);
void        InsertPlayNode(SEND_NODE *pNodeHead,pthread_mutex_t hMutex,SEND_NODE *pNew);
SEND_NODE   *FindWaitProcessNode(NODE_STATUS    status);
int         StopTcpNode(int id);
int         StopAllTcpNode();
void        FreeTcpSendNode(SEND_NODE  *pNode);
int         FreeAllTcpNode();

//-----------UDP

int         GetUdpRunNode(int nChannel,int nStream);
void        StopUdpNode(int id);
void        RequestUdpPlay(OPEN_HEAD openHead,unsigned long ip,unsigned long port);
BOOL        ExistUdpUser(OPEN_HEAD openHead,unsigned long ip,unsigned long port);
BOOL        CloseUdpPlay(OPEN_HEAD closeHead);
void        RequestMultiPlay(OPEN_HEAD openHead);
BOOL        ExistMultiUser(OPEN_HEAD openHead);
BOOL        CloseMultiPlay(OPEN_HEAD closeHead);
void        FreeAllUdpNode();


void		FileSendThread(void *pPar);
//-------------

void        TalkbackThread(void *par);

int access_listen_ret_thread(void *param);

void        FreeTSD(void *pPar);
void	    ConvertHead(void  *pConvert,int size);
//
unsigned long   MakeLong(unsigned short low,unsigned short hi);
//
unsigned short  HeightWord(unsigned long a);
//
unsigned short  LowWord(unsigned long a);
//
int             GetTime();
//
void        OnTimer(int nSignal);

SEND_NODE   *GetWaitProcessHeadNode();

int getNetKeepAliveStatus();
int setNetKeepAliveStatus(int value);

BOOL CheckIpIsInternet(unsigned long dwIp);
int Send(int hSer,char *pBuf,int nLen);
int RequestTcpPlayExt(OPEN_HEAD openHead, int hDataSock,  unsigned long clientIp);

int TcpReceiveEX2(int hSock, char *pBuffer, DWORD nSize);

int pauseSendAvData( void );
int resumeSendAvData( void );
int getSendAvStatus( void );


SER_INFO	g_serinfo;
CLIENT_HEAD g_clientlist;

int g_netKeepAlive;

int TL_Video_Net_Send(MSG_HEAD  *pMsgHead, char *pSendBuf);
BOOL TL_Video_Net_SendAllMsg(char *pMsgBuf, int nLen);


#endif


