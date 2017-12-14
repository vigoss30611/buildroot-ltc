#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* End of #ifdef __cplusplus */
//btlib
//#include "hi_debug_def.h"
//#include "hi_log_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "ext_defs.h"

#ifdef HI_PTHREAD_MNG
#include "hi_pthread_wait.h"
#else
#include <pthread.h>
#endif

#include "hi_msession_lisnsvr.h"
#include "hi_mt_socket.h"
#include "hi_msession_rtsp.h"
//#include "hi_msession_http.h"
//#include "hi_msession_rtsp_o_http.h"
//#include "hi_msession_owsp.h"
#include "hi_log_def.h"
#include "hi_vod.h"

#define LISNSVR_LISTEN_TIMOUT_SEC   0 /*rtsp 循环监听超时时间 360s*/
#define LISNSVR_LISTEN_TIMOUT_USEC  50000

#define LISNSVR_MAX_PROTOCOL_BUFFER 1024 /*接收buff最大长度*/
#define LISNSVR_MAX_SESS_NUM        32   /*最大连接数*/

typedef enum hiMSESS_LISNSVR_STATE_E
{
    MSESS_LISNSVR_STATE_STOPED    = 0, /*停止状态*/
    MSESS_LISNSVR_STATE_RUNNING   = 1, /*运行状态*/
    MSESS_LISNSVR_SVR_STATE_BUTT
    
}MSESS_LISNSVR_STATE_E;

/*已连接客户的临时信息,从监听端口上接受请求时记入该结构，
  将该连接分发出去时从该结构中清楚*/
typedef struct hiMSESS_ACCEPT_CLI_INFO
{
    /*给信息有效标志*/
    HI_BOOL     bValidFlag;

    /*cli socket*/
    HI_S32   s32AccptCliSock;
    
} MSESS_ACCEPT_CLI_INFO;

typedef struct hiMSESS_LISNSVR_S
{
    /*监听线呈，在监听服务启动时创建*/
	
    LISNSVR_TYPE_E enSvrType;
    //btlib
#ifdef HI_PTHREAD_MNG
    HI_Pthread_T s32LisnThd;
#else
    unsigned long int s32LisnThd;
#endif
    
    /*监听socket*/
    HI_SOCKET    s32LisnSock;
    
    /*监听端口*/
    HI_PORT   s32LisnPort;

    /*监听服务器状态*/
    MSESS_LISNSVR_STATE_E enLisSvrState;

    /*监听svr允许的最大连接数*/
    HI_S32 s32MaxConnNum;

    HI_CHAR  aszFirstMsgBuff[LISNSVR_MAX_PROTOCOL_BUFFER];
    
    /*用户连接临时结构*/
    MSESS_ACCEPT_CLI_INFO astrAccptCliSock[LISNSVR_MAX_SESS_NUM];
	NW_ClientInfo * pNWClientInfo;
}MSESS_LISNSVR_S;

/*add socket into */
HI_S32 LisnSvrAddAccptCli(MSESS_ACCEPT_CLI_INFO* pastrAccptCliSock, HI_SOCKET s32Sock)
{
    int i=0;
    
    /*add the connected sock into a unused member*/
    for(i=0; i<LISNSVR_MAX_SESS_NUM;i++)
    {
        if( HI_FALSE == pastrAccptCliSock[i].bValidFlag)
        {
            pastrAccptCliSock[i].s32AccptCliSock = s32Sock;
            pastrAccptCliSock[i].bValidFlag = HI_TRUE;
            return HI_SUCCESS;
        }
    }

    /*has over max connet number*/
    return HI_ERR_MSESS_LISNSVR_OVER_CONN_NUM;
}

HI_VOID* LisnSvrProcess11(void *args)
{
    HI_S32      s32Ret      = 0;
    int         i           =0 ;
    HI_S32      s32Errno    = 0;                        /*linux system errno*/
    HI_SOCKET   s32TempSock = -1;
    HI_S32      s32RecvBytes = 0;
    MSESS_LISNSVR_S* pstruLisnSvr = NULL;
    struct timeval lisnTimeoutVal;             /* Timeout value */

    fd_set read_fds;                     
    fd_set read_master_fds;                     /* read socket fd set */    
    HI_S32 s32MaxFdNum = 0;                    /*max socket number of read set*/

    HI_SOCKET s32LisnSock = -1;              /*rtsp listen socket*/
    HI_SOCKET s32AccptCliSock = -1;             /*accepted client connect socket */
    struct sockaddr_in strSockAdrAccptCli;      /*socket addr of accepted client connect*/
    socklen_t     s32SockAddrLen  = 0;

    memset(&lisnTimeoutVal,0,sizeof(struct timeval));
    memset(&strSockAdrAccptCli,0,sizeof(struct sockaddr_in));

    pstruLisnSvr = (MSESS_LISNSVR_S*) args;
    s32LisnSock = pstruLisnSvr->s32LisnSock;
    
    /*init read fds and set rtsp lisen sock into read set*/
    FD_ZERO(&read_fds);
    FD_ZERO(&read_master_fds);
    //s32MaxFdNum = s32LisnSock + 1;
    FD_SET(s32LisnSock , &read_master_fds);

    HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_NOTICE,"[Info]rtsp listen svr thread %d start ok, listening on port %d .socket =%d....\r\n",
                getpid(), pstruLisnSvr->s32LisnPort,s32LisnSock);
	
    
            //printf("fine client  =%d.\n",i);
                s32TempSock = pstruLisnSvr->pNWClientInfo->ClientSocket;
				
                /*if client A's connect request has been accpet, and it send first msg,
                  then recv the msg ,and distrubit connect link, remove the link from
                  select readfds*/
                if(1 )  // FD_ISSET(s32TempSock,&read_fds)
                {
                	
                    /*recv the first msg*/
                    memset(pstruLisnSvr->aszFirstMsgBuff,0,LISNSVR_MAX_PROTOCOL_BUFFER);
                    
                    s32RecvBytes = recv(s32TempSock, pstruLisnSvr->aszFirstMsgBuff, 
                                        LISNSVR_MAX_PROTOCOL_BUFFER, 0);
                    if (s32RecvBytes < 0 )
                    {
                        s32Errno = errno;
                        getpeername(s32TempSock,(struct sockaddr*)(&strSockAdrAccptCli),&s32SockAddrLen);
                        HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_ERR, 
                                     "[rtsp lisen thread] read data from socket error= %s. remoteip:%s\n",
                                     strerror(s32Errno), inet_ntoa(strSockAdrAccptCli.sin_addr));
                        close(s32TempSock);
                    }
                    else if (s32RecvBytes == 0)
                    {
                        s32Errno = errno;
                        getpeername(s32TempSock,(struct sockaddr*)(&strSockAdrAccptCli),&s32SockAddrLen);
                        HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_ERR,"[rtsp lisen thread]peer close socket %d.remoteip: %s\n",
                                    s32TempSock,inet_ntoa(strSockAdrAccptCli.sin_addr)); 
                        close(s32TempSock);
                    }
                    /*distrbute the link ,and remove it from select socket and rtsp LisnSvr's struct*/
                    else
                    {
                        /*1 remove the link from select socket and rtsp LisnSvr's struct*/
                        if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_RTSP)
                        {
                        /*1. distrbute the link with first msg*/
							
                        	s32Ret =  HI_RTSP_DistribLink_11(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes, HI_FALSE);
						}
                        else if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_HTTP)
                        {
                        /*1. distrbute the link with first msg*/
                        	//s32Ret =  HI_HTTP_DistribLink(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes);
						}
                        else if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_RTSPoHTTP)
                        {
                        /*1. distrbute the link with first msg*/
                        	//s32Ret =  HI_RTSP_O_HTTP_DistribLink(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes);
						}
                        else if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_OWSP)
                        {
                        /*1. distrbute the link with first msg*/
                        	//s32Ret =  HI_OWSP_DistribLink(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes);
						}
                        if (HI_SUCCESS != s32Ret) 
                        {
                            HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_ERR,
                                        "lisen svr distribute link(sock %d) error=%d\r\n",
                                        s32TempSock,s32Ret);
                        } 
                    }

                  //  FD_CLR(s32TempSock,&read_master_fds);
                  //  pstruLisnSvr->astrAccptCliSock[i].bValidFlag = HI_FALSE;
                } // end of if( (s32TempSock != s32LisnSock) && (FD_ISSET(s32TempSock,&read_fds)) )

            
        

 	
	
	
    return HI_NULL;

}


/*main func of listen thread */
HI_VOID* LisnSvrProcess(void *args)
{
    HI_S32      s32Ret      = 0;
    int         i           =0 ;
    HI_S32      s32Errno    = 0;                        /*linux system errno*/
    HI_SOCKET   s32TempSock = -1;
    HI_S32      s32RecvBytes = 0;
    MSESS_LISNSVR_S* pstruLisnSvr = NULL;
    struct timeval lisnTimeoutVal;             /* Timeout value */

    fd_set read_fds;                     
    fd_set read_master_fds;                     /* read socket fd set */    
    HI_S32 s32MaxFdNum = 0;                    /*max socket number of read set*/

    HI_SOCKET s32LisnSock = -1;              /*rtsp listen socket*/
    HI_SOCKET s32AccptCliSock = -1;             /*accepted client connect socket */
    struct sockaddr_in strSockAdrAccptCli;      /*socket addr of accepted client connect*/
    HI_S32    s32SockAddrLen  = 0;

    memset(&lisnTimeoutVal,0,sizeof(struct timeval));
    memset(&strSockAdrAccptCli,0,sizeof(struct sockaddr_in));

    pstruLisnSvr = (MSESS_LISNSVR_S*) args;
    s32LisnSock = pstruLisnSvr->s32LisnSock;
    
    /*init read fds and set rtsp lisen sock into read set*/
    FD_ZERO(&read_fds);
    FD_ZERO(&read_master_fds);
    s32MaxFdNum = s32LisnSock + 1;
    FD_SET(s32LisnSock , &read_master_fds);

    HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_NOTICE,"[Info]rtsp listen svr thread %d start ok, listening on port %d .socket =%d....\r\n",
                getpid(), pstruLisnSvr->s32LisnPort,s32LisnSock);

    while(MSESS_LISNSVR_STATE_RUNNING == pstruLisnSvr->enLisSvrState)
    {
        lisnTimeoutVal.tv_sec = LISNSVR_LISTEN_TIMOUT_SEC;
        lisnTimeoutVal.tv_usec = LISNSVR_LISTEN_TIMOUT_USEC;
        
        memcpy(&read_fds,&read_master_fds,sizeof(fd_set));
        
        /*jude if there is new connect or first message come through already connected link */
        s32Ret = select(s32MaxFdNum,&read_fds,NULL,NULL,&lisnTimeoutVal);

        /*some wrong with network*/
        if ( s32Ret == -1 ) 
        {
            /*to do : 是否需要关闭socket,关闭哪些*/
            s32Errno = errno;
		if(errno == EINTR)	//TI项目可能会出现这个错误
		{
			errno = 0;
			continue;
		}
            HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_CRIT,"rtsp listen thread: select error=%s\n",
                        strerror(s32Errno));
            MT_SOCKET_CloseSocket(pstruLisnSvr->s32LisnSock);
            return HI_NULL;
        } 
        /*to do :select timeout over preset*/
        else if ( s32Ret == 0 ) 
        {
           	//HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_CRIT,"rtsp listen thread: select error=%s\r\n",
            //            strerror(s32Errno));
            continue;
        }

        /*jude if there is msg comeing from any accepted connect link.
          if it is, recv the msg and distribute the link */
        for(i=0 ; i< LISNSVR_MAX_SESS_NUM; i++)
        {
            //printf("fine client  =%d.\n",i);
            if( HI_TRUE == pstruLisnSvr->astrAccptCliSock[i].bValidFlag)
            {
                s32TempSock = pstruLisnSvr->astrAccptCliSock[i].s32AccptCliSock;

                /*if client A's connect request has been accpet, and it send first msg,
                  then recv the msg ,and distrubit connect link, remove the link from
                  select readfds*/
                if( (s32TempSock != s32LisnSock) && (FD_ISSET(s32TempSock,&read_fds)) )
                {
                    /*recv the first msg*/
                    memset(pstruLisnSvr->aszFirstMsgBuff,0,LISNSVR_MAX_PROTOCOL_BUFFER);
                    
                    s32RecvBytes = recv(s32TempSock, pstruLisnSvr->aszFirstMsgBuff, 
                                        LISNSVR_MAX_PROTOCOL_BUFFER, 0);
                    if (s32RecvBytes < 0 )
                    {
                        s32Errno = errno;
                        getpeername(s32TempSock,(struct sockaddr*)(&strSockAdrAccptCli),&s32SockAddrLen);
                        HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_ERR, 
                                     "[rtsp lisen thread] read data from socket error= %s. remoteip:%s\n",
                                     strerror(s32Errno), inet_ntoa(strSockAdrAccptCli.sin_addr));
                        close(s32TempSock);
                    }
                    else if (s32RecvBytes == 0)
                    {
                        s32Errno = errno;
                        getpeername(s32TempSock,(struct sockaddr*)(&strSockAdrAccptCli),&s32SockAddrLen);
                        HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_ERR,"[rtsp lisen thread]peer close socket %d.remoteip: %s\n",
                                    s32TempSock,inet_ntoa(strSockAdrAccptCli.sin_addr)); 
                        close(s32TempSock);
                    }
                    /*distrbute the link ,and remove it from select socket and rtsp LisnSvr's struct*/
                    else
                    {
                        /*1 remove the link from select socket and rtsp LisnSvr's struct*/
                        if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_RTSP)
                        {
                        /*1. distrbute the link with first msg*/
                        	s32Ret =  HI_RTSP_DistribLink(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes, HI_FALSE);
						}
                        else if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_HTTP)
                        {
                        /*1. distrbute the link with first msg*/
                        	//s32Ret =  HI_HTTP_DistribLink(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes);
						}
                        else if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_RTSPoHTTP)
                        {
                        /*1. distrbute the link with first msg*/
                        	//s32Ret =  HI_RTSP_O_HTTP_DistribLink(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes);
						}
                        else if(pstruLisnSvr->enSvrType == LISNSVR_TYPE_OWSP)
                        {
                        /*1. distrbute the link with first msg*/
                        	//s32Ret =  HI_OWSP_DistribLink(s32TempSock, pstruLisnSvr->aszFirstMsgBuff,s32RecvBytes);
						}
                        if (HI_SUCCESS != s32Ret) 
                        {
                            HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_ERR,
                                        "lisen svr distribute link(sock %d) error=%d\r\n",
                                        s32TempSock,s32Ret);
                        } 
                    }

                    FD_CLR(s32TempSock,&read_master_fds);
                    pstruLisnSvr->astrAccptCliSock[i].bValidFlag = HI_FALSE;
                } // end of if( (s32TempSock != s32LisnSock) && (FD_ISSET(s32TempSock,&read_fds)) )

            }
        }

        /* Check if a new connect has occured  */
        if ( FD_ISSET(s32LisnSock,&read_fds) ) 
        {
            /*accept new connect*/
            s32SockAddrLen = sizeof(strSockAdrAccptCli);
            s32AccptCliSock = accept(s32LisnSock,(struct sockaddr *)&strSockAdrAccptCli,
                                     &s32SockAddrLen);
            /*to do :accept errno??*/
            if ( -1 == s32AccptCliSock  )
            {
                s32Errno = errno;
                HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_ERR,"rtsp lisn svr accept conn error=%s (cliIp:%s,cliPort:%d)\r\n",
                        strerror(s32Errno),inet_ntoa(strSockAdrAccptCli.sin_addr),ntohs(strSockAdrAccptCli.sin_port));
                continue;
            }

			printf("rtsp lisn svr accept conn error=%s (cliIp:%s,cliPort:%d)\r\n",
					strerror(s32Errno),inet_ntoa(strSockAdrAccptCli.sin_addr),ntohs(strSockAdrAccptCli.sin_port));

			struct sockaddr_in me;	
			socklen_t len = sizeof(me);	
			if(getsockname(s32AccptCliSock, (struct sockaddr *)&me, &len) == 0)
			{	
				 char	buf[256];	
				 inet_ntop(AF_INET, &me.sin_addr, buf, 256);   
				 printf("s32AccptCliSock=%d, local address:%s\n", s32AccptCliSock, buf);
			}

        	struct sockaddr_in my_addr;
			my_addr.sin_family		= AF_INET;
			//my_addr.sin_addr 		= me.sin_addr;	//这个IP就是我选择要发送广播包的网卡的IP
			memcpy(&my_addr.sin_addr, &me.sin_addr, sizeof(me.sin_addr));
			s32Ret = bind(s32AccptCliSock, (struct sockaddr *)&my_addr, sizeof(my_addr));
			printf("s32Ret=%d, errno=%d\n", s32Ret, errno);/*add new connection socket num to AcceptCli struct*/


			s32Ret = LisnSvrAddAccptCli(pstruLisnSvr->astrAccptCliSock,s32AccptCliSock);
            if(HI_SUCCESS != s32Ret)
            {
                HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_ERR,
                            "connet cli number which rtspLisThd is processing over preset max number %d\n",
                            LISNSVR_MAX_SESS_NUM);
                //continue;
            }
            else
            {
                /*add new connection link into readfds*/
                if ( s32AccptCliSock + 1 > s32MaxFdNum )
                {
                    s32MaxFdNum = s32AccptCliSock  + 1;
                }
                FD_SET(s32AccptCliSock ,&read_master_fds);
            }
        } 

        /*updat Max read Fds Num*/
        for ( s32TempSock = s32MaxFdNum - 1; 
              s32TempSock >= 0 && !FD_ISSET(s32TempSock,&read_master_fds); 
              s32TempSock = s32MaxFdNum - 1 )
        {
            s32MaxFdNum = s32TempSock;
        }
    }   /*end of while*/

    MT_SOCKET_CloseSocket(pstruLisnSvr->s32LisnSock);
    HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_NOTICE,"[Info]rtsp listen svr stoped: thread %d withdraw\n",
               pstruLisnSvr->s32LisnPort);

    return HI_NULL;
}

/*init rtsp listen svr struct*/
HI_VOID LisnSvrInit(MSESS_LISNSVR_S* pstruLisnSvr)
{
    int i = 0;
    //memset(pstruLisnSvr,0,sizeof(MSESS_LISNSVR_S));

    for(i = 0; i<LISNSVR_MAX_SESS_NUM; i++)
    {
        pstruLisnSvr->astrAccptCliSock[i].bValidFlag = HI_FALSE;
    }
}

HI_S32 HI_MSESS_LISNSVR_StartSvr_11(HI_S32 s32LisnPort,HI_S32 s32MaxConnNum,HI_U32 **ppu32LisnSvrHandle,LISNSVR_TYPE_E enSvrType,NW_ClientInfo * clientinfo)
{

    HI_S32 s32Ret            = 0;   
    
    int socket_opt_value      = 1;
    HI_SOCKET s32TempLisnSock = -1;      /*temp listen socket*/
    MSESS_LISNSVR_S* pstruLisnSvr = NULL;

    /* malloc listen server manage struct*/
    pstruLisnSvr = (MSESS_LISNSVR_S *)malloc(sizeof(MSESS_LISNSVR_S));
    if(NULL == pstruLisnSvr)
    {
        HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_ERR,"malloc for lisn server error.\r\n");
        return HI_ERR_MSESS_LISNSVR_MALLOC;
    }

    /*0. Init listen svr struct */
    LisnSvrInit(pstruLisnSvr);
    pstruLisnSvr->s32LisnPort = s32LisnPort;
    pstruLisnSvr->s32MaxConnNum = s32MaxConnNum;
    pstruLisnSvr->enSvrType = enSvrType;
    /* 1. Create socket as block mode and listen for accept new connection.*/
    s32TempLisnSock = clientinfo->ClientSocket;  //socket(PF_INET,SOCK_STREAM,0);
    pstruLisnSvr->pNWClientInfo = clientinfo;

    /*init rtsp listen svr state as running*/
    pstruLisnSvr->enLisSvrState = MSESS_LISNSVR_STATE_RUNNING;

    /*5. creat a listen thread to accept client connetion*/
    //btlib
/*
#ifdef HI_PTHREAD_MNG
    s32Ret = HI_PthreadMNG_Create("HI_HTTP_LISNSVR_START", &(pstruLisnSvr->s32LisnThd),
		NULL, LisnSvrProcess11, (HI_VOID*)pstruLisnSvr);
#else
    s32Ret = pthread_create(&(pstruLisnSvr->s32LisnThd),
		NULL, LisnSvrProcess11, (HI_VOID*)pstruLisnSvr);
#endif
*/
	
	LisnSvrProcess11(pstruLisnSvr);
	

    /*return listen server handle*/
    *ppu32LisnSvrHandle = (HI_U32*)pstruLisnSvr;

    
    return HI_SUCCESS;
}



/*start listen server*/
HI_S32 HI_MSESS_LISNSVR_StartSvr(HI_S32 s32LisnPort,HI_S32 s32MaxConnNum,HI_U32 **ppu32LisnSvrHandle,LISNSVR_TYPE_E enSvrType)
{

    HI_S32 s32Ret            = 0;   
    struct sockaddr_in svr_addr;
    int socket_opt_value      = 1;
    HI_SOCKET s32TempLisnSock = -1;      /*temp listen socket*/
    MSESS_LISNSVR_S* pstruLisnSvr = NULL;

    /* malloc listen server manage struct*/
    pstruLisnSvr = (MSESS_LISNSVR_S *)malloc(sizeof(MSESS_LISNSVR_S));
    if(NULL == pstruLisnSvr)
    {
        HI_LOG_SYS("RTSP_LIVESVR",LOG_LEVEL_ERR,"malloc for lisn server error.\r\n");
        return HI_ERR_MSESS_LISNSVR_MALLOC;
    }

    /*0. Init listen svr struct */
    LisnSvrInit(pstruLisnSvr);
    pstruLisnSvr->s32LisnPort = s32LisnPort;
    pstruLisnSvr->s32MaxConnNum = s32MaxConnNum;
    pstruLisnSvr->enSvrType = enSvrType;
    /* 1. Create socket as block mode and listen for accept new connection.*/
    s32TempLisnSock = socket(PF_INET,SOCK_STREAM,0);
    if ( s32TempLisnSock  == -1 )
    {
        HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_ERR,"Creat rtsp lisn sock error.\r\n");
        return HI_ERR_MSESS_LISNSVR_SOCKET_ERR;
    }

    /*2. set sock option*/
    if (setsockopt(s32TempLisnSock,SOL_SOCKET,SO_REUSEADDR,&socket_opt_value,sizeof(int)) == -1) 
	{
		HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_ERR,"set socket option error.\r\n");
        return HI_ERR_MSESS_LISNSVR_SOCKET_ERR;
	}

    /*3. bind socket */
    printf("rtsp listen port: %d!\n",pstruLisnSvr->s32LisnPort);
    svr_addr.sin_family = AF_INET; 
    svr_addr.sin_port  =  htons(pstruLisnSvr->s32LisnPort); 
    svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(svr_addr.sin_zero), '\0', 8); 

    if (bind(s32TempLisnSock, (struct sockaddr *)&svr_addr, sizeof(struct sockaddr)) == -1) 
    {
        HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_ERR,"rtsp lisn socket bind error.\n"); 
        return HI_ERR_MSESS_LISNSVR_SOCKET_ERR;
    }

    /*4. listen on port*/
    s32Ret = listen(s32TempLisnSock,pstruLisnSvr->s32MaxConnNum);
    if (s32Ret == -1)
    {
        HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_ERR,"rtsp lisn socket listen error.\n"); 
        return HI_ERR_MSESS_LISNSVR_SOCKET_ERR;
    }
    else
    {
        pstruLisnSvr->s32LisnSock = s32TempLisnSock;
        
        /*init rtsp listen svr state as stoped*/
        pstruLisnSvr->enLisSvrState = MSESS_LISNSVR_STATE_STOPED;
    }

    /*init rtsp listen svr state as running*/
    pstruLisnSvr->enLisSvrState = MSESS_LISNSVR_STATE_RUNNING;

    /*5. creat a listen thread to accept client connetion*/
    //btlib
#ifdef HI_PTHREAD_MNG
    s32Ret = HI_PthreadMNG_Create("HI_HTTP_LISNSVR_START", &(pstruLisnSvr->s32LisnThd),
		NULL, LisnSvrProcess, (HI_VOID*)pstruLisnSvr);
#else
    s32Ret = pthread_create(&(pstruLisnSvr->s32LisnThd),
		NULL, LisnSvrProcess, (HI_VOID*)pstruLisnSvr);
#endif

    if (s32Ret != HI_SUCCESS)
    {
        /*init rtsp listen svr state as running*/
        pstruLisnSvr->enLisSvrState = MSESS_LISNSVR_STATE_STOPED;
        
        HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_NOTICE,"Start rtsp Lisn server error:cann't create thread.\n");
        return  HI_ERR_MSESS_LISNSVR_LISNSVR_INIT;
    }

    /*return listen server handle*/
    *ppu32LisnSvrHandle = (HI_U32*)pstruLisnSvr;

    HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_NOTICE,"**************Start rtsp Lisn server success on port %d,sock %d.\n",
                 pstruLisnSvr->s32LisnPort,s32TempLisnSock);
    
    return HI_SUCCESS;
}

HI_S32 HI_MSESS_LISNSVR_StopSvr_11(HI_U32 *pu32LisnSvrHandle)
{
    MSESS_LISNSVR_S * pstruLisnSvr = NULL;

    if(NULL == pu32LisnSvrHandle)
    {
        return HI_ERR_MSESS_LISNSVR_ILLEGALE_PARAM;
    }
    pstruLisnSvr = (MSESS_LISNSVR_S*)pu32LisnSvrHandle;
    /*set the listen svr stop flag*/
    pstruLisnSvr->enLisSvrState= MSESS_LISNSVR_STATE_STOPED;  //RTSPSESS_THREAD_STATE_RUNNING
   
    //HI_CloseSocket(pstruLisnSvr->s32LisnSock);

    HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_NOTICE,"finished for HI_PthreadJoin of rtsp lisn thread.\n ");
    return HI_SUCCESS;
}


/*stop the lisen svr*/
HI_S32 HI_MSESS_LISNSVR_StopSvr(HI_U32 *pu32LisnSvrHandle)
{
    MSESS_LISNSVR_S * pstruLisnSvr = NULL;

    if(NULL == pu32LisnSvrHandle)
    {
        return HI_ERR_MSESS_LISNSVR_ILLEGALE_PARAM;
    }
    pstruLisnSvr = (MSESS_LISNSVR_S*)pu32LisnSvrHandle;
    /*set the listen svr stop flag*/
    pstruLisnSvr->enLisSvrState= MSESS_LISNSVR_STATE_STOPED;
   
    //HI_CloseSocket(pstruLisnSvr->s32LisnSock);
#ifdef HI_PTHREAD_MNG
    HI_PthreadJoin(pstruLisnSvr->s32LisnThd,0);
#else
    pthread_join(pstruLisnSvr->s32LisnThd,0);
#endif
    HI_LOG_DEBUG("RTSP_LIVESVR",LOG_LEVEL_NOTICE,"finished for HI_PthreadJoin of rtsp lisn thread.\n ");
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
