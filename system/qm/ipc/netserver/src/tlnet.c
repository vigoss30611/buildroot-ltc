/******************************************************************************

******************************************************************************/
#include "tlnetin.h"
#include "TLNetCommon.h"
#include "tlnetinterface.h"
#include "tlsession.h"
#include "tlthrough.h"

int g_modefyVersion = 0;
int getModifyVersion(int *version);
BOOL TL_Video_Net_Command_SendAllMsg(char *pMsgBuf, int nLen , int nCmd, int chn);
int do_OpenFile_Request(int hConnSock);
int XSocket_TCP_Send(int sock, char *buf, int len);
int mega_send_data(int nSock, char *buf, int len);
int XSocket_TCP_Recv(int hSock, char *pBuffer, unsigned long in_size, int *out_size);
extern int TcpReceive(int hSock, char *pBuffer, DWORD nSize);


int dms_ProcessOpenAlarm(int ConnSock);
BOOL SetClientAlarmInfo(int nID, int fdAlarm);
int GenerateMultiAddr(char *pMultiAddr)
{
    unsigned char ipSegment1 = 224;
    unsigned char ipSegment2 = 0;
    unsigned char ipSegment3 = 0;
    unsigned char ipSegment4 = 0;
    srand((unsigned)time(NULL));
    if (pMultiAddr)
    {
        ipSegment1 += (unsigned char)(random()%15);
        ipSegment2 += (unsigned char)(random()%255);
        ipSegment3 += (unsigned char)(random()%255);
        ipSegment4 += (unsigned char)(random()%255);
        sprintf(pMultiAddr, "%d.%d.%d.%d", ipSegment1, ipSegment2, ipSegment3, ipSegment4);
        return 0;
    }
    else
    {
        return -1;
    }
}

BOOL TL_Video_Net_ServerInit(int nChannelNum,int nBasePort)
{
    int i = 0;
    long addr = -1;
    char                multiAddr[64];

    GenerateMultiAddr(multiAddr); // Generate random multi IP address

    if (nChannelNum<1 || nChannelNum>QMAPI_MAX_CHANNUM)
    {
        return FALSE;
    }
    if (nBasePort<1024 || nBasePort>65535)
    {
        return FALSE;
    }
    
    addr = inet_addr(multiAddr);
    if (-1 == addr)
    {
        printf("MultiIP ERROR\n");      
        return FALSE;
    }

    g_serinfo.nChannelNum = nChannelNum;
    g_serinfo.nBasePort = nBasePort;
    strcpy(g_serinfo.szMultiIP, multiAddr);
    g_serinfo.multiAddr = addr;
    for (i=0; i<QMAPI_MAX_CHANNUM; i++)
    {
        g_serinfo.udpStreamSend1.pUdpSendList[i] = NULL;
        g_serinfo.udpStreamSend2.pUdpSendList[i] = NULL;
    }
    
    g_serinfo.pSendBuf = NULL;
    g_serinfo.pRecvBuf = NULL;
    g_serinfo.pUpdateFileBuf = NULL;
    g_serinfo.nSendBufferNum = 0;
    
    g_serinfo.hListenSock = -1;
    g_serinfo.hUdpPortSock = -1;
    g_serinfo.pTcpSendList = NULL;

    g_serinfo.funcServerRecv = NULL;
    g_serinfo.bServerStart = FALSE;
    g_serinfo.bServerExit = FALSE;
    g_serinfo.bListenExit = FALSE;

    
    g_serinfo.pCallbackRequestTalk = NULL;
    g_serinfo.pCallbackStreamTalk = NULL;
    g_serinfo.hTalkbackSocket = 0;
    
    g_clientlist.nTotalNum = 0;
    g_clientlist.pNext = NULL;
    pthread_mutex_init(&g_clientlist.hClientMutex, NULL); 
    pthread_mutex_init(&g_tl_record_login_mutex,NULL);
    g_netKeepAlive = 0;
    
    getModifyVersion(&g_modefyVersion);
    
    return TRUE;
}

BOOL TL_Video_Net_ServerStart()
{
    int i = 0;
    int ret = 0;
    int hSock = -1;
    SEND_NODE   *pNode = NULL;
    g_serinfo.bServerExit = FALSE;
    signal(SIGPIPE, SIG_IGN);
    hSock = TcpSockListen(INADDR_ANY, g_serinfo.nBasePort);
    if (-1 == hSock)
    {
        err();
        return FALSE;
    }
    g_serinfo.hListenSock = hSock;

#if 0 
    hSock = UdpPortListen(INADDR_ANY, g_serinfo.nBasePort);
    if (-1 == hSock)
    {
        err();
        return FALSE;
    }
    g_serinfo.hUdpPortSock = hSock; 
#endif

    for (i=0; i<QMAPI_MAX_CHANNUM; i++)
    {   
        pNode = (SEND_NODE *)malloc(sizeof(SEND_NODE)) ;
        if (NULL == pNode)
        {
            return FALSE;
        }
        bzero(pNode,sizeof(SEND_NODE));
        pNode->status = RUN;
        g_serinfo.udpStreamSend1.pUdpSendList[i] = pNode;
        pthread_mutex_init(&g_serinfo.udpStreamSend1.hUdpSendListMutex[i], NULL);

#if 0
        ret = pthread_create(&g_serinfo.udpStreamSend1.hUdpSendThreadID[i], NULL, (void*)&UdpSendThread,(void*)MakeLong(i,0));
        if (0 != ret)
        {
            err();
            return FALSE;
        }
#endif
    }


    pNode = (SEND_NODE *)malloc(sizeof(SEND_NODE));
    if (NULL == pNode)
    {
        return FALSE;
    }
    bzero(pNode,sizeof(SEND_NODE));
    pNode->status = RUN;
    g_serinfo.pTcpSendList = pNode;
    pthread_mutex_init(&g_serinfo.hTcpSendListMutex, NULL); 
    g_serinfo.pSendBuf = (char *) malloc(NETCMD_MAX_SIZE/8);
    pthread_mutex_init(&g_serinfo.sendMutex, NULL); 
    
    //g_serinfo.pRecvBuf = (char *) malloc(NETCMD_MAX_SIZE);
        
    g_serinfo.msgProcessThreadNum = 0;
    g_serinfo.msgWaitThreadNum = 0;
    g_serinfo.msgQuitThreadNum = 0;
    pthread_mutex_init(&g_serinfo.msgThreadMutex, NULL);
    pthread_cond_init(&g_serinfo.msgThreadCond, NULL);
    for (i=0; i<MAX_THREAD_NUM; i++)
    {
        ret = pthread_create(&g_serinfo.msgThreadID[i], NULL, (void *)&TcpMsgRecvThread, (void *)i);
        if (0 != ret)
        {
            err();
            return FALSE;
        }
    }
    pthread_mutex_init(&g_serinfo.msgQuitMangMutex, NULL);
    pthread_cond_init(&g_serinfo.msgQuitMangCond, NULL);
    ret = pthread_create(&g_serinfo.msgQuitMangThread,NULL, (void *)&TcpMsgQuitThread, NULL);
    if (0 != ret)
    {
        err();
        return FALSE;
    }

    g_serinfo.dataProcessThreadNum = 0;
    g_serinfo.dataWaitThreadNum = 0;
    g_serinfo.dataQuitThreadNum = 0;
    pthread_mutex_init(&g_serinfo.dataThreadMutex,NULL);
    pthread_cond_init(&g_serinfo.dataThreadCond,NULL);

    for (i=0; i<MAX_THREAD_NUM; i++)
    {
        ret = pthread_create(&g_serinfo.dataThreadID[i], NULL, (void *)&NetServerTcpSendThread, (void *)i);
        if (0 != ret)
        {
            err();
            return FALSE;
        }
    }
	
    pthread_mutex_init(&g_serinfo.dataQuitMangMutex, NULL);
    pthread_cond_init(&g_serinfo.dataQuitMangCond, NULL);
    ret = pthread_create(&g_serinfo.dataQuitMangThread, NULL, (void *)&TcpDataQuitThread, NULL);
    if (0 != ret)
    {
        err();
        return FALSE;
    }
    ret = pthread_key_create(&g_serinfo.hReadPosKey, FreeTSD);
    if (0 != ret)
    {
        err();
        return FALSE;
    }

#if 0 
    // UDP
    ret = pthread_create(&g_serinfo.hUdpPortThread, NULL, (void*)&UdpPortListenThread, NULL);
    if (0 != ret)
    {
        err();
        return FALSE;
    }
#endif
    
    // TCP
    ret = pthread_create(&g_serinfo.hListenThread, NULL, (void*)&ListenThread, NULL);
    if (0 != ret)
    {
        err();
        return FALSE;
    }
      
    g_serinfo.nMsgThreadCount = MAX_THREAD_NUM;
    g_serinfo.nDataThreadCount = MAX_THREAD_NUM;
    g_serinfo.bServerStart = TRUE;  
    return TRUE;
}

//
BOOL TL_Video_Net_ServerStop()
{
    //int i = 0;
    if (!g_serinfo.bServerStart)
    {
        return FALSE;
    }
    g_serinfo.bServerStart = FALSE;
    g_serinfo.bServerExit = TRUE;
    g_serinfo.bListenExit = TRUE;
    if (-1 != g_serinfo.hListenSock)
    {
        shutdown(g_serinfo.hListenSock, 2);
        close(g_serinfo.hListenSock);
        g_serinfo.hListenSock = -1;
    }
    if (-1 != g_serinfo.hUdpPortSock)
    {
        shutdown(g_serinfo.hUdpPortSock, 2);
        close(g_serinfo.hUdpPortSock);
        g_serinfo.hUdpPortSock = -1;
    }   
    StopAllTcpNode();
    StopAllClient();
    sleep(SOCK_TIME_OVER);
    pthread_cond_broadcast(&g_serinfo.dataThreadCond);
    pthread_cond_broadcast(&g_serinfo.msgThreadCond);
    usleep(1);
    pthread_cond_signal(&g_serinfo.msgQuitMangCond);
    //pthread_cond_signal(&g_serinfo.dataQuitMangCond);
    usleep(1);
    FreeAllTcpNode();
    FreeAllClient();
    if (g_serinfo.pSendBuf)
    {
        free(g_serinfo.pSendBuf);
        g_serinfo.pSendBuf = NULL;
    }
    pthread_mutex_destroy(&g_serinfo.hTcpSendListMutex); 
    pthread_mutex_destroy(&g_serinfo.sendMutex); 
    
    if(g_serinfo.pRecvBuf)
    {
        free(g_serinfo.pRecvBuf);
        g_serinfo.pRecvBuf = NULL;
    }
    if (g_serinfo.pUpdateFileBuf)
    {
        free(g_serinfo.pUpdateFileBuf);
        g_serinfo.pUpdateFileBuf = NULL;
    }
    pthread_key_delete(g_serinfo.hReadPosKey);
    return TRUE;
}

void TL_Video_Net_SetSendBufNum(int nBufNum)
{
    if ((nBufNum <= 0) || (nBufNum > MAX_POOL_SIZE))
    {
        return;
    }
    if (g_serinfo.bServerStart)
    {
        return;
    }
    g_serinfo.nSendBufferNum = nBufNum;
}




///TL Video Add New Protocol Check User Name Password IP Mac Initial
void    TL_Video_Net_SetUserCheckFunc(TL_CheckUserPsw   pFunc)
{
    g_serinfo.tlcheckUser=pFunc;
}

///TL Video Add New Protocol Server Initial
void    TL_Video_Net_SetServerRecvFunc(ServerRecv   pFunc)
{   
    g_serinfo.funcServerRecv = pFunc;
}

///TL Video Add New Protocol Get Server Information Initial
void    TL_Video_Net_GetServerInfo_Initial(TL_GetServerInfo pFunc)
{   
    g_serinfo.tlGetServerInfo = pFunc;
}

///TL Video Add New Protocol Get Channel Information Initial
void    TL_Video_Net_GetChannelInfo_Initial(TL_GetChannelInfo   pFunc)
{   
    g_serinfo.tlGetChannelInfo = pFunc;
}

///TL Video Add New Protocol Get Sensor Information Initial
void    TL_Video_Net_GetSensorInfo_Initial(TL_GetSensorInfo pFunc)
{   
    g_serinfo.tlGetSensorInfo = pFunc;
}


///TL Video Add New Protocol Cut User Information Initial
void    TL_Video_Net_CutUserInfo_Initial(TL_CutUserInfo pFunc)
{
    g_serinfo.tlcutuserinfo = pFunc;
}

///TL Video Add New Protocol Client Channel Right Check Initial
void    TL_Video_Net_ChannelRightCheck_Initial(TL_DataChannelClientRight pFunc)
{
    g_serinfo.tlchannelrightcheck=pFunc;
}


int TL_Video_Net_Send(MSG_HEAD  *pMsgHead, char *pSendBuf)
{
    int ret = 0;
    COMM_HEAD commHead;
    struct timeval      to;
    fd_set      fset;
    //int   senddatalen=0 , pos=0;
    
    if (!g_serinfo.bServerStart)
    {
        err();
        return -1;
    }
    if (NULL == g_serinfo.pSendBuf)
    {
            err();
        return -1;
    }
    if ((pMsgHead->nBufSize + sizeof(COMM_HEAD)) > NETCMD_MAX_SIZE)
    {
            err();
        return -1;
    }
    FD_ZERO(&fset);
    bzero(&to, sizeof(to));
    to.tv_sec = 20;
    to.tv_usec = 0; 
    commHead.nFlag = TLNET_FLAG;
    commHead.nCommand = pMsgHead->nCommand;
    commHead.nBufSize = pMsgHead->nBufSize;
    commHead.nChannel = pMsgHead->nChannel;
    commHead.nErrorCode = pMsgHead->nErrorCode;

    ConvertHead(&commHead,sizeof(commHead));
    pthread_mutex_lock(&g_serinfo.sendMutex);

    memcpy(g_serinfo.pSendBuf, &commHead, sizeof(commHead));

    if (pMsgHead->nBufSize > 0 && pSendBuf)
    {
        memcpy(g_serinfo.pSendBuf + sizeof(commHead), pSendBuf, pMsgHead->nBufSize);
    }
    else
    {
//      printf(" @@@ Error nBufSize:%d pSendBuf:0x%x\n", __FUNCTION__, __LINE__, pMsgHead->nBufSize, pSendBuf);
//      goto NET_SEND_FAIL;
    }
    if(pMsgHead->nID<=0 || pMsgHead->nID>1024)
    {
        printf(" @@@ %s(%d) nID(%d) Error nCommand:0x%x, nBufSize:%d \n", __FUNCTION__, __LINE__, pMsgHead->nID, pMsgHead->nCommand, pMsgHead->nBufSize);
        goto    NET_SEND_FAIL;
    }
    FD_SET(pMsgHead->nID, &fset);
    ret = select(pMsgHead->nID+1, NULL, &fset, NULL, &to);
    if (ret == 0 )  //timeout
    {
        err();
        goto    NET_SEND_FAIL;
    }
    if (ret < 0)
    {
        err();
        goto NET_SEND_FAIL;
    }
    //printf("Send-----------------------Start \n");
    ret = send(pMsgHead->nID, g_serinfo.pSendBuf, sizeof(COMM_HEAD)+pMsgHead->nBufSize, 0);
    if(ret !=sizeof(COMM_HEAD)+pMsgHead->nBufSize)
    {
        close(pMsgHead->nID);
        err();
    }
    //printf("Complete -----------------------end \n");
NET_SEND_FAIL:
    pthread_mutex_unlock(&g_serinfo.sendMutex);
    return ret;
}


BOOL TL_Video_Net_SendAllMsg(char *pMsgBuf, int nLen)
{
    BOOL bRet = 0;
    MSG_HEAD msgHead;
    CLIENT_INFO *pUser = NULL;
    if (!g_serinfo.bServerStart)
    {
        err();
        return -1;
    }
    
    if (NULL == pMsgBuf)
    {
        err();
        return 0;
    }
    if ((nLen <= 0) || (nLen > NETCMD_MAX_SIZE))
    {
        err();
        return 0;
    }
    bzero(&msgHead,sizeof(msgHead));
    pthread_mutex_lock(&g_clientlist.hClientMutex);
    pUser = g_clientlist.pNext;
    while (pUser)
    {
        //printf("Sending All Uer \n"); 
        msgHead.clientIP = pUser->ip;
        msgHead.clientPort = pUser->port;
		if(pUser->nAlarmFlag==1)	//海康使用hAlarmSocket发报警数据
			msgHead.nID = pUser->hAlarmSocket;
		else
			msgHead.nID = pUser->hMsgSocket;
        msgHead.nLevel = pUser->level;
        memcpy(msgHead.szUserName,pUser->szUserName,QMAPI_NAME_LEN);
        memcpy(msgHead.szUserPsw,pUser->szUserPsw,QMAPI_PASSWD_LEN);
        msgHead.nCommand = TL_STANDARD_ALARM_MSG_HEADER;
        msgHead.nBufSize = nLen;
		if(msgHead.nID>0){// TL_Video_Net_Send通过msgHead.nID发数据
	        bRet = TL_Video_Net_Send(&msgHead, pMsgBuf);
		}
        pUser = pUser->pNext;
    }
    pthread_mutex_unlock(&g_clientlist.hClientMutex);
    
    return bRet;
}

///TL Video All User Msg 
BOOL TL_Video_Net_Command_SendAllMsg(char *pMsgBuf, int nLen , int nCmd, int chn)
{
    BOOL bRet = 0;
    MSG_HEAD msgHead;
    CLIENT_INFO *pUser = NULL;
    if (!g_serinfo.bServerStart)
    {
        return -1;
    }
    
    if (NULL == pMsgBuf)
    {
        return 0;
    }
    if ((nLen <= 0) || (nLen > NETCMD_MAX_SIZE))
    {
        return 0;
    }
    bzero(&msgHead,sizeof(msgHead));
    pthread_mutex_lock(&g_clientlist.hClientMutex);
    msgHead.nChannel = chn;
    pUser = g_clientlist.pNext;
    while (pUser)
    {
        //printf("Sending All Uer \n");   
        msgHead.clientIP = pUser->ip;
        msgHead.clientPort = pUser->port;
        msgHead.nID = pUser->hMsgSocket;
        msgHead.nLevel = pUser->level;
        memcpy(msgHead.szUserName,pUser->szUserName,QMAPI_NAME_LEN);
        memcpy(msgHead.szUserPsw,pUser->szUserPsw,QMAPI_PASSWD_LEN);
        msgHead.nCommand = nCmd;
        msgHead.nBufSize = nLen;
        bRet = TL_Video_Net_Send(&msgHead, pMsgBuf);
        pUser = pUser->pNext;
    }
    pthread_mutex_unlock(&g_clientlist.hClientMutex);
    
    return bRet;
}

BOOL TL_Video_Net_SendUserMsg(int nUserID,char *pMsgBuf, int nLen)
{
    BOOL bRet = 0;
    MSG_HEAD msgHead;
    CLIENT_INFO *pUser = NULL;
    if (!g_serinfo.bServerStart)
    {
        return -1;
    }
    if (NULL == pMsgBuf)
    {
        return 0;
    }
    if ((nLen <= 0) || (nLen > NETCMD_BUF_SIZE))
    {
        return 0;
    }
    bzero(&msgHead,sizeof(msgHead));
    pthread_mutex_lock(&g_clientlist.hClientMutex);
    pUser = g_clientlist.pNext;
    while (pUser)
    {
        
        if (nUserID == pUser->hMsgSocket)
        {
            msgHead.clientIP = pUser->ip;
            msgHead.clientPort = pUser->port;
            msgHead.nID = pUser->hMsgSocket;
            msgHead.nLevel = pUser->level;
            memcpy(msgHead.szUserName,pUser->szUserName,QMAPI_NAME_LEN);
            memcpy(msgHead.szUserPsw,pUser->szUserPsw,QMAPI_PASSWD_LEN);
            msgHead.nCommand = NETCMD_USER_MSG;
            msgHead.nBufSize = nLen;
            bRet = TL_Video_Net_Send(&msgHead,pMsgBuf); 
            break;
        }
        pUser = pUser->pNext;
    }
    pthread_mutex_unlock(&g_clientlist.hClientMutex);
    return bRet;
}

int getUserId(unsigned long ipAddr, unsigned long port)
{
    int socketFd = -1;
    CLIENT_INFO *pUser = NULL;
    if (!g_serinfo.bServerStart)
    {
        return -1;
    }
    pthread_mutex_lock(&g_clientlist.hClientMutex);
    pUser = g_clientlist.pNext;
    while (pUser)
    {
        if (ipAddr == pUser->ip)
        {
            socketFd = pUser->hMsgSocket;
            break;
        }
        pUser = pUser->pNext;
    }
    pthread_mutex_unlock(&g_clientlist.hClientMutex);
    return socketFd;
}
void ListenThread(void *pPar)
{
    int                     hConnSock;
    int                     hListenSock;
    int                     ret;
    int                     len;
    //int                       level;
    //int                   nCurNum;
    ACCEPT_HEAD             acceptHead;
    fd_set                  fset;
    struct timeval          to;
    struct sockaddr_in      addr;
	QMAPI_NET_UPGRADE_RESP uinfo;
    prctl(PR_SET_NAME, (unsigned long)"Listen", 0,0,0);
    listen_ret_t           *plisten_ret;
    pthread_t               thrdID;
    FD_ZERO(&fset);
    bzero(&to, sizeof(to));
    bzero(&addr, sizeof(addr));
    bzero(&acceptHead, sizeof(acceptHead));
	
    len = sizeof(addr);
    hListenSock = g_serinfo.hListenSock;
    
    //while (!g_serinfo.bServerExit)
    while (!g_serinfo.bServerExit && !g_serinfo.bListenExit)
    {
        to.tv_sec = SOCK_TIME_OVER;
        to.tv_usec = 0;
        FD_SET(hListenSock, &fset);
        ret = select(hListenSock+1, &fset, NULL, NULL, &to);

        if (g_serinfo.bServerExit || g_serinfo.bListenExit)
        {
            printf("[ListenThread]: g_serinfo.bServerExit(%d)\n", g_serinfo.bServerExit); 
            break;
        }
        if (0 == ret) // time out
        {
            continue;
        }
        if (-1 == ret) // error
        {       
            err();          
            if (errno == EINTR || errno == EBADF)
            {
                continue;
            }
        }
        if (!FD_ISSET(hListenSock, &fset))
        {
            continue;
        }

        hConnSock = accept(hListenSock, (struct sockaddr*)&addr, (socklen_t *)&len);
        if (hConnSock < 0) 
        {
            err();
            if (errno == 24) 
            {
                //break;
            }
            else
            {
                err();
                continue;
            }
        }
        //升级中不接受新的socket连接
        if (QMapi_sys_ioctrl(0, QMAPI_NET_STA_UPGRADE_RESP, 0, &uinfo, sizeof(QMAPI_NET_UPGRADE_RESP)) == QMAPI_SUCCESS
			&& uinfo.state)
        {
            close(hConnSock);
            err();
            continue;
        }

        SetConnSockAttr(hConnSock, SOCK_TIME_OVER);
        plisten_ret = (listen_ret_t *)malloc(sizeof(listen_ret_t));
        if (plisten_ret == NULL)
        {
            err();
            continue;
        }
        plisten_ret->accept_sock = hConnSock;
        memcpy(&plisten_ret->client_addr, &addr, sizeof(struct sockaddr_in));

        /* create a thread for access the client request */
        ret = pthread_create(&thrdID, NULL, (void*)access_listen_ret_thread, (void *)plisten_ret);
        if (ret != 0)
        {
            err();
            continue;
        }
        usleep(80);

    }//end while
}


int TranslateCommand(MSG_HEAD *pMsgHead, char *pRecvBuf)
{
    OPEN_HEAD               *pOpenHead = NULL;
    CLIENT_INFO                 clientInfo;
    BOOL                    bInner = FALSE;
    char                        *pTmp;
    UPDATE_FILE_PARTITION           *pParHead;

#ifdef TL_NATTCP
    char    strIp[32];
    int hSock2 = 0;
#endif

    //printf("Net pMsgHead->nCommand [0x%x]\n", pMsgHead->nCommand);
    switch (pMsgHead->nCommand)
    {
        case NETCMD_MULTI_OPEN:
        {
            bInner = TRUE;
            pOpenHead = (OPEN_HEAD *)pRecvBuf;
            if (NULL == pOpenHead)
            {
                break;
            }
            ConvertHead(pOpenHead,sizeof(OPEN_HEAD));

            if (TLNET_FLAG != pOpenHead->nFlag)
            {
                err();
                break;
            }

            if ((pOpenHead->nSerChannel < 0) || (pOpenHead->nSerChannel >= QMAPI_MAX_CHANNUM))
            {
                err();
                break;
            }
            if (!GetClient(pOpenHead->nID, &clientInfo))
            {
                err();
                break;
            }

            //if (!ExistMultiUser(*pOpenHead))
            //{
            //    RequestMultiPlay(*pOpenHead);
            //}
        }

        break;

        case NETCMD_PLAY_CLOSE:
        {
            bInner = TRUE;
            pOpenHead = (OPEN_HEAD *)pRecvBuf;
            if (NULL == pOpenHead)
            {
                break;
            }
            ConvertHead(pOpenHead,sizeof(OPEN_HEAD));
            if (TLNET_FLAG != pOpenHead->nFlag)
            {
                err();
                break;
            }

            if ((pOpenHead->nSerChannel < 0) || (pOpenHead->nSerChannel >= QMAPI_MAX_CHANNUM))
            {
                err();
                break;
            }
            if (!GetClient(pOpenHead->nID, &clientInfo))
            {
                err();
                break;
            }
            if (PROTOCOL_TCP == pOpenHead->nProtocolType)
            {       
            }
            else if(PROTOCOL_UDP == pOpenHead->nProtocolType)
            {
                //CloseUdpPlay(*pOpenHead);
            }
            else if(PROTOCOL_MULTI == pOpenHead->nProtocolType)
            {
                //CloseMultiPlay(*pOpenHead);
            } 
            break;
        }

        case NETCMD_UPDATE_FILE:
        {
                bInner = TRUE;
            pParHead = (UPDATE_FILE_PARTITION *) pRecvBuf; 
            pParHead->nFileLength = ArchSwapLE32(pParHead->nFileLength);
            pParHead->nFileOffset = ArchSwapLE32(pParHead->nFileOffset);
            pParHead->nPartitionNo = ArchSwapLE32(pParHead->nPartitionNo);
            pParHead->nPartitionNum = ArchSwapLE32(pParHead->nPartitionNum);
            pParHead->nPartitionSize = ArchSwapLE32(pParHead->nPartitionSize);

            if (0 == pParHead->nPartitionNo)
            {   
                pTmp = pRecvBuf + sizeof(UPDATE_FILE_PARTITION);
                pMsgHead->nCommand = (6000+7);
                pMsgHead->nBufSize = 0;
                bzero(pTmp, 260);
                memcpy(pTmp, pParHead->szFileName, 260);
                if (0 != g_serinfo.funcServerRecv(pMsgHead, pTmp))
                {
                    break;
                }

                //stop all user operation
                g_serinfo.pUpdateFileBuf = malloc(pParHead->nFileLength+260);
                if (NULL == g_serinfo.pUpdateFileBuf)
                {
                    err();
                    break;
                }
                memcpy(g_serinfo.pUpdateFileBuf, pTmp, 260);
            }
            else
            {
                if (NULL == g_serinfo.pUpdateFileBuf)
                {
                    break;
                }

                memcpy(g_serinfo.pUpdateFileBuf + 260 + pParHead->nFileOffset,
                    pRecvBuf+sizeof(UPDATE_FILE_PARTITION), pParHead->nPartitionSize);

                if (pParHead->nPartitionNo == pParHead->nPartitionNum)
                {
                    pMsgHead->nCommand = (6000+7);
                    pMsgHead->nBufSize = pParHead->nFileLength;
                    g_serinfo.funcServerRecv(pMsgHead, g_serinfo.pUpdateFileBuf);   
                }
            }
            break;
        }
        
#ifdef TL_NATTCP
        case NETCMD_OPEN_REVERSE_CH:
        {
            if (!g_sysParam.setSpecifyHostInfo.bUse)
            {
                printf("NETCMD_OPEN_REVERSE_CH: Failed!\n");
                break;
            }
            bInner = TRUE;
            inet_ntop(AF_INET, &REMOTE_HOST_DIR[0].ip, strIp, sizeof(strIp));
            ConnectForCreateCh(pMsgHead->nChannel, strIp, REMOTE_HOST_DIR[0].port, &hSock2, NVReqAVdata);
            break;
        }

        case NETCMD_OPEN_REVERSE_TB:
        {
            if (!g_sysParam.setSpecifyHostInfo.bUse)
            {
                break;
            }
            bInner = TRUE;
            inet_ntop(AF_INET, &REMOTE_HOST_DIR[0].ip, strIp, sizeof(strIp));
            ConnectForCreateCh(pMsgHead->nChannel, strIp, REMOTE_HOST_DIR[0].port, &hSock2, NVReqTalkback2);
            break;
        }
#endif

            default:
                break;
        }

    if (!bInner)
    {
        g_serinfo.funcServerRecv(pMsgHead, pRecvBuf);
        //printf("g_serinfo.funcServerRecv(pMsgHead, pRecvBuf); ok\n");
    }

    return TRUE;
}

int TcpSockListen(int ip, int port)
{
    int hSock = -1;
    int opt = -1;
    int ret = -1;
    socklen_t   len = 0;
    struct sockaddr_in  addr;

    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(ip);
    addr.sin_port = htons(port);
    hSock = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == hSock)
    {
        err();
        return -1;
    }
    do
    {
        opt = 1;
        ret = setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if (0 != ret)
        {
            err();
            break;
        }
        opt = 1;
        ret = setsockopt(hSock,IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
        if (0 != ret)
        {
            err();
            break;
        }
        opt = SOCK_SNDRCV_LEN;
        ret = setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt));
        if (0 != ret)
        {
            err();
            break;
        }
        opt = SOCK_SNDRCV_LEN;
        ret = setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt));
        if (0 != ret)
        {
            err();
            break;
        }
        opt = sizeof(len);
        ret = getsockopt(hSock, SOL_SOCKET, SO_SNDBUF, &len, (socklen_t *)&opt);
        if (0 != ret)
        {
            err();
            break;
        }
        opt = sizeof(len);
        ret = getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, &len, (socklen_t *)&opt);
        if (0 != ret)
        {
            err();
            break;
        }

        ret = bind(hSock, (struct sockaddr *)&addr, sizeof(addr));
        if (0 != ret)
        {
            err();
            break;
        }

        ret = listen(hSock, 10);
        if (0 != ret)
        {
            err();
            break;
        }
    }while(FALSE);

    if (0 != ret)
    {
        shutdown(hSock,2);
        close(hSock);
        return -1;
    }

    return hSock;
}

void    SetConnSockAttr(int hSock, int nTimeOver)
{
    int opt;
    int ret;
    int len;
    struct timeval  to;

    opt = 1;
    ret = setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    opt = 1;
    ret = setsockopt(hSock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
    opt = SOCK_SNDRCV_LEN;
    ret = setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt));
    opt = SOCK_SNDRCV_LEN;
    opt = sizeof(len);
    ret = getsockopt(hSock, SOL_SOCKET, SO_SNDBUF, &len, (socklen_t *)&opt);
    opt = sizeof(len);
    ret = getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, &len, (socklen_t *)&opt);


    to.tv_sec = 3;
    to.tv_usec = 0;
    //ret = setsockopt(hSock,SOL_SOCKET,SO_RCVTIMEO,&to,sizeof(to));
    ret = setsockopt(hSock,SOL_SOCKET,SO_SNDTIMEO,&to,sizeof(to));

	printf("ret=%d\n", ret);
}

void    ConvertHead(void *pConvert,int size)
{
    char *ph = (char *)pConvert;
    if (NULL == ph)
    {
        return;
    }
    
    while (size > 0)
    {
        *(unsigned long*)ph = ArchSwapLE32(*(unsigned long*)ph);
        ph += 4;
        size -= 4;
    }
}

void FreeTSD(void *pPar)
{
    free((READ_POSITION*)pPar);
}

unsigned long MakeLong(unsigned short low,unsigned short hi)
{
    return (unsigned long)(low | (((unsigned long)hi) << 16));
}

unsigned short HeightWord(unsigned long a)
{
    return (unsigned short)((a >> 16) && 0xFFFF);
}

unsigned short LowWord(unsigned long a)
{
    return (unsigned short)a;
}

int GetTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
}

/*void OnTimer(int nSignal)
{
    CheckClientAlive();
    //alarm(2);
}*/

int getModifyVersion(int *version)
{
    FILE     *fp = NULL;
    char    buffer[3];
    int ret = 0;

    fp = fopen("version.ini", "r");
    if (fp == NULL)
    {
        *version = 0;
        return -1;
    }

    ret = fread(buffer, 1, 3, fp);
    if (ret <= 0)
    {
        *version = 0;
        fclose(fp); 
        return -1;
    }

    buffer[2] = '\0';
    *version = atoi(buffer);

    if (*version < 0 || *version >255)
    {
        *version = 0;
        fclose(fp); 
        return -1;
    }
    
    fclose(fp);
    
    return 0;
}

int getNetKeepAliveStatus()
{
    return g_netKeepAlive;
}

int setNetKeepAliveStatus(int value)
{
    g_netKeepAlive = value;
    return 0;
}

int getUserLoginNum()
{
    return g_serinfo.msgProcessThreadNum;
}

int getOpenChannelNum()
{
    return g_serinfo.dataProcessThreadNum;
}


int Send(int hSer,char *pBuf,int nLen)
{
    int ret = 0;
    int sendsize = 0;
    
    while(sendsize < nLen)
    {
        ret = send(hSer,pBuf+sendsize,nLen - sendsize,0);
        if(ret < 1)
        {
            err();
            return ret;
        }
        sendsize = sendsize + ret;
    }
    
    return sendsize;
}




/*10.x.x.x 172.16.x.x 172.31.x.x 192.168.x.x */
BOOL CheckIpIsInternet(unsigned long dwIp)
{
    BOOL    bIsInternet;
    unsigned long bMask;

    bIsInternet = TRUE;
    bMask = dwIp & 0xFF;

    if (dwIp == 1)
        return TRUE;

    if (bMask > 224)
            return FALSE;

    switch(bMask)
    {
        case 10:
            bIsInternet = FALSE;
            break;
        case 192:
            if (((dwIp & 0xFF00) >> 8) == 168)
                bIsInternet = FALSE;
            break;

        case 172:
            bMask = (dwIp & 0xFF00) >> 8;
            if (bMask > 15 && bMask < 32)
                bIsInternet = FALSE;
            break;

        default:
            break;
    }
    
    return bIsInternet;
}

int access_listen_ret_thread(void *param)
{
    fd_set                  fset;
    int                     ret;
//    struct timeval              to;
    struct sockaddr_in                  addr;
    int             hConnSock;
    listen_ret_t            listen_ret;
    //int               i;
    int             level;
    int                         nCurNum;
    ACCEPT_HEAD             acceptHead;

    ///TL New Logon Struct
    //TL_SERVER_USER                TL_User_logon;          
    prctl(PR_SET_NAME, (unsigned long)"Client", 0,0,0);
    memcpy(&listen_ret, param, sizeof(listen_ret_t));

    hConnSock = listen_ret.accept_sock;
    memcpy(&addr, &listen_ret.client_addr, sizeof(struct sockaddr_in));

    if (param)
        free(param);

    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

    FD_ZERO(&fset);
  //  bzero(&to,sizeof(to));

    //while(1)
    {
        ret = TcpReceiveEX2(hConnSock, (char *)&acceptHead, sizeof(acceptHead));
        if(ret <= 0)
        {
            err();
            shutdown(hConnSock,2);
            close(hConnSock);
            hConnSock = -1;
            return -1;
        }

        ConvertHead(&acceptHead, sizeof(acceptHead));
        printf("acceptHead.nSockType [%lu] acceptHead.nFlag [%lu]\n",acceptHead.nSockType , acceptHead.nFlag);


        if(TLNET_FLAG != acceptHead.nFlag)
        {
            shutdown(hConnSock,2);
            close(hConnSock);
            hConnSock = -1;
            err();
            return -1;
        }

        switch(acceptHead.nSockType)
        {
            case SOCK_LOGON:
                {
                int         i = 0;
                int         nBufSize = 0;
                USER_INFO   userInfo;
                COMM_NODE   commNode;
                ///TL Net Portocol
                TLNV_SERVER_INFO    TL_server_info;
                TLNV_CHANNEL_INFO   TL_Channel_info;
                TL_SENSOR_INFO      TL_Sensor_info;
                ///TL Net Portocol Right Manger
                int nUserRight=0x0 , nNetPreviweRight=0x0;
                //int         serverVersion = 0;
                //int         mainVersion = 0;
                //char      deviceID[16];

                bzero(&userInfo,sizeof(userInfo));
                bzero(&commNode,sizeof(commNode));
            
                ret = TcpReceiveEX2(hConnSock,(char *)&userInfo,sizeof(USER_INFO));
                if(ret <= 0)
                {
                    shutdown(hConnSock,2);
                    close(hConnSock);
                    hConnSock = -1;
                    err();
                    return -1;
                }

                level = 0;
				printf("#### %s %d\n", __FUNCTION__, __LINE__);
                if(g_serinfo.tlcheckUser)
                {
                    level=g_serinfo.tlcheckUser(userInfo.szUserName,userInfo.szUserPsw , addr.sin_addr.s_addr , hConnSock , userInfo.MacCheck , &nUserRight , &nNetPreviweRight);

                    if(level!=1)
                    {
                        err();
                        commNode.commHead.nFlag = TLNET_FLAG;
                        commNode.commHead.nCommand = level;
                        commNode.commHead.nBufSize = 0;
                        ConvertHead(&commNode.commHead,sizeof(commNode.commHead));
                        ret=send(hConnSock,&commNode,sizeof(commNode.commHead),0);
                        sleep(1);
                        shutdown(hConnSock,2);
                        close(hConnSock);
                        hConnSock = -1;
                        return -1;
                    }
                }
                else
                {
                    err();
                    commNode.commHead.nFlag = TLNET_FLAG;
                    commNode.commHead.nCommand = TLERR_UNINITOBJ;
                    commNode.commHead.nBufSize = 0;
                    ConvertHead(&commNode.commHead,sizeof(commNode.commHead));
                    ret=send(hConnSock,&commNode,sizeof(commNode.commHead),0);
                    sleep(1);
                    shutdown(hConnSock,2);
                    close(hConnSock);
                    hConnSock = -1;
                    return -1;
                }
    			printf("#### %s %d\n", __FUNCTION__, __LINE__);
                pthread_mutex_lock(&g_serinfo.msgThreadMutex);
                nCurNum = g_serinfo.msgProcessThreadNum;
                pthread_mutex_unlock(&g_serinfo.msgThreadMutex);

                if((nCurNum + 1) >= MAX_THREAD_NUM)
                {
                    err();
                    printf("nCurNum(%d) + 1 >= MAX_THREAD_NUM(%d)\n", nCurNum, MAX_THREAD_NUM);
                                commNode.commHead.nFlag = TLNET_FLAG;
                    commNode.commHead.nCommand = NETCMD_MAX_LINK;
                    commNode.commHead.nBufSize = 0;
                    ConvertHead(&commNode.commHead,sizeof(commNode.commHead));
                    ret = send(hConnSock,&commNode,sizeof(commNode.commHead),0);
                    shutdown(hConnSock,2);
                    close(hConnSock);
                    hConnSock = -1;
                    err();
                    return -1;
                }
                else
                {
                    //g_bMaxMsgThread = False;
                }

                commNode.commHead.nFlag = TLNET_FLAG;
                commNode.commHead.nCommand = NETCMD_CHANNEL_CHECK_OK;
                commNode.commHead.nChannel = g_serinfo.nChannelNum;
                
                
                nBufSize = 0;

                memcpy(commNode.pBuf+nBufSize,&g_serinfo.multiAddr,sizeof(int));
                nBufSize += 4;
                memcpy(commNode.pBuf+nBufSize,&hConnSock,sizeof(int));
                nBufSize += 4;

    			printf("#### %s %d\n", __FUNCTION__, __LINE__);
                ///Write Server information
                if(g_serinfo.tlGetServerInfo)
                {
                	printf("#### %s %d\n", __FUNCTION__, __LINE__);
                    g_serinfo.tlGetServerInfo((char *)&TL_server_info); 
                }
                else
                {
                    err();
                    return -1;
                }
                
                TL_server_info.dwUserRight=nUserRight;
                TL_server_info.dwNetPriviewRight =nNetPreviweRight;

                memcpy(commNode.pBuf+nBufSize, &TL_server_info , sizeof(TLNV_SERVER_INFO));
                nBufSize += sizeof(TLNV_SERVER_INFO);
    			printf("#### %s %d\n", __FUNCTION__, __LINE__);

                ///Write media information
                if(g_serinfo.tlGetChannelInfo)
                {
                	printf("#### %s %d, wChannelNum=%d\n", __FUNCTION__, __LINE__, TL_server_info.wChannelNum);
                    for(i=0;i<TL_server_info.wChannelNum;i++)
                    {
                        ret=g_serinfo.tlGetChannelInfo((char *)&TL_Channel_info , i);
                        if(ret==-1)
                        {
                            err();
                            break;
                        }
                        memcpy(commNode.pBuf+nBufSize, &TL_Channel_info , sizeof(TLNV_CHANNEL_INFO));
                        
                        nBufSize += sizeof(TLNV_CHANNEL_INFO);
                    }
                }
                else
                {
                    err();
                    return -1;
                }

    			printf("#### %s %d\n", __FUNCTION__, __LINE__);
                ///Write Sensor information
                if(g_serinfo.tlGetSensorInfo)
                {
                	printf("#### %s %d\n", __FUNCTION__, __LINE__);
                    for(i=0;i<TL_server_info.dwAlarmInCount;i++)
                    {
                        ret=g_serinfo.tlGetSensorInfo((char *)&TL_Sensor_info , i , 0);
                        if(ret==-1)
                        {
                            err();
                            break;
                        }
                        memcpy(commNode.pBuf+nBufSize, &TL_Sensor_info , sizeof(TL_SENSOR_INFO));
                        nBufSize += sizeof(TL_SENSOR_INFO);
                    }
                    for(i=0;i<TL_server_info.dwAlarmOutCount;i++)
                    {
                        ret=g_serinfo.tlGetSensorInfo((char *)&TL_Sensor_info , i , 1);
                        if(ret==-1)
                        {
                            err();
                            break;
                        }
                        memcpy(commNode.pBuf+nBufSize, &TL_Sensor_info , sizeof(TL_SENSOR_INFO));
                        nBufSize += sizeof(TL_SENSOR_INFO);
                    }
                }   

    			printf("#### %s %d\n", __FUNCTION__, __LINE__);
                commNode.commHead.nBufSize = nBufSize;
                ConvertHead(&commNode.commHead,sizeof(commNode.commHead));
                ConvertHead(commNode.pBuf, nBufSize);
        
                ret = send(hConnSock,&commNode,sizeof(commNode.commHead)+nBufSize, 0);
                printf("#### %s %d\n", __FUNCTION__, __LINE__);
                if (ret <= 0)
                {
                	printf("#### %s %d\n", __FUNCTION__, __LINE__);
                    err();
                    shutdown(hConnSock, 2);
                    close(hConnSock);
                    hConnSock = -1;
                    err();
                    return -1;
                }
    			printf("#### %s %d\n", __FUNCTION__, __LINE__);
             //   printf("### send %d\n", ret);
                QMAPI_AudioPromptPlayFile("/root/qm_app/res/sound/alarm.wav", 0);
                ClientLogon(addr,userInfo,hConnSock,level);
                pthread_mutex_lock(&g_serinfo.msgQuitMangMutex);
                g_serinfo.msgWaitThreadNum ++;
                pthread_mutex_unlock(&g_serinfo.msgQuitMangMutex);
                pthread_cond_signal(&g_serinfo.msgThreadCond);
            }
            break;

        case SOCK_DATA:
        {
            BOOL        bSuccess;
            COMM_HEAD   commHead;
            OPEN_HEAD   openHead;
            CLIENT_INFO clientInfo;
    
            bzero(&clientInfo,sizeof(clientInfo));
            bzero(&openHead,sizeof(openHead));
            bzero(&commHead,sizeof(commHead));
    
            commHead.nFlag = TLNET_FLAG;
            bSuccess = FALSE;

            ret = recv(hConnSock, &openHead, sizeof(openHead), 0);
            do
            {
                if((ret <= 0) || (ret < sizeof(openHead)))
                {
                    err();
                    commHead.nCommand = NETCMD_RECV_ERR;
                    break;
                }

			
                ConvertHead(&openHead,sizeof(openHead));
        
                if(TLNET_FLAG != openHead.nFlag)
                {
                    err();
                    commHead.nCommand = NETCMD_NOT_TLDATA;
                    break;
                }
                if(openHead.nSerChannel >= QMAPI_MAX_CHANNUM)
                {
                    commHead.nCommand = NETCMD_CHANNEL_INVALID;
                    err();
					break;
                }
        
                //printf("openHead.nID=%d, nSerChannel=0x%x, nClientChannel=%d, nStreamType=%d\n" ,
                //        openHead.nID, openHead.nSerChannel, openHead.nClientChannel, openHead.nStreamType);
                if(!GetClient(openHead.nID,&clientInfo))
                {
                    commHead.nCommand = NETCMD_NOT_LOGON;
                    err();
                    break;
                }

                ///TL Video New Protocol Open Channel Right Check
                if(g_serinfo.tlchannelrightcheck)
                {
                    if(!g_serinfo.tlchannelrightcheck(openHead.nSerChannel , openHead.nID))
                    {
                        err();
						break;
                    }
                }           
                
                nCurNum = g_serinfo.dataProcessThreadNum;
                if((nCurNum + 1) >= MAX_THREAD_NUM)
                {
                    commHead.nCommand = NETCMD_MAX_LINK;
                    err();
                    break;
                }
                
                //RequestTcpPlay(openHead, hConnSock, addr.sin_addr.s_addr);
                ret = RequestTcpPlayExt(openHead, hConnSock, addr.sin_addr.s_addr);
                if (ret < 0)
                {
                    break;
                }   
                bSuccess = TRUE;
                /* openHead.nSerChannel */
                //usleep(100000);
                //2013-1-21 10:55:31--去掉这里请求I帧动作，后面已有。
                //g_serinfo.tlIntraencoder(openHead.nSerChannel, openHead.nStreamType);
                
            }while(FALSE);

            if(!bSuccess)
            {
                err();
                ConvertHead(&commHead,sizeof(commHead));
                ret = send(hConnSock, &commHead, sizeof(commHead), 0);
                if (ret == sizeof(commHead))
                {
                    //printf("SOCK_DATA: request fail commHead.nCommand = %ul\n", commHead.nCommand);
                }
                else
                {
                    //printf("SOCK_DATA: error commHead.nCommand = %ul\n", commHead.nCommand);
                }
                usleep(1);
                shutdown(hConnSock,2);
                close(hConnSock);
                hConnSock = -1;
            }
            else
            {
	            pthread_mutex_lock(&g_serinfo.dataQuitMangMutex);
	            g_serinfo.dataWaitThreadNum ++;
	            pthread_mutex_unlock(&g_serinfo.dataQuitMangMutex);
	            pthread_cond_signal(&g_serinfo.dataThreadCond);
            }
        }
        usleep(80);
        break;

        case SOCK_FILE:
        {
            do_OpenFile_Request(hConnSock);      
            break;
        }

        case SOCK_TALKBACK:
        {
            TALK_PAR    par;
            memset(&g_serinfo.talkThrdPar, 0, sizeof(g_serinfo.talkThrdPar));
            // need fixme yi.zhang
            // 语音对讲前设置音频解码参数

            par.nFlag = 931;
            par.nChannel = 1;
            par.nBits = 16;
            par.nSamples = 8000;

            ConvertHead(&par,sizeof(par));
            ret = send(hConnSock, &par, sizeof(par), 0);
            if(ret != sizeof(par))
                return -1;
            g_serinfo.talkThrdPar.hSock = hConnSock;
            g_serinfo.talkThrdPar.ip = addr.sin_addr.s_addr;
            g_serinfo.talkThrdPar.port = addr.sin_port;
            g_serinfo.hTalkbackSocket = g_serinfo.talkThrdPar.hSock; 
            
            pthread_create(&g_serinfo.thrdID, NULL, (void *)&TalkbackThread, &g_serinfo.talkThrdPar);

            sleep(1);
            break;
        }

		case SOCK_ALARM:
			dms_ProcessOpenAlarm(hConnSock);
			break;
        default:
            err();
            break;
        }//end switch
    
        }//end while
    
        return 0;
}

#define MAX_STREAM_BUFF_SIZE 512*1024

typedef struct tagTL_STREAM_BUFFER
{
    DWORD dwSize;
    DWORD dwPosition;
    DWORD dwTimeTick;
    DWORD FrameType;
    BYTE  pBuffer[MAX_STREAM_BUFF_SIZE];
}TL_STREAM_BUFFER,*PTL_STREAM_BUFFER;

typedef struct tagPLAYFILE_INFO
{
    HANDLE hFileHandle;
    QMAPI_TIME BeginTime;//文件起始时间
    QMAPI_TIME EndTime;//文件结束时间
    QMAPI_TIME CurTime;
    QMAPI_TIME StartTime;//搜索开始时间
    QMAPI_TIME StopTime;//搜索结束时间
    int index;
    unsigned int firstReads;//0:未开始读，1:已经开始
    unsigned long lastTimeTick;//上一个I帧的时间戳
    unsigned long firstStamp;//第一个I帧的时间戳
    unsigned long dwTotalTime;//文件总时间,播放时能获取到
    char reserve[16];
    int width;
    int height;
    
    char sFileName[128];
    //TL_STREAM_BUFFER *pStreamBuffer;
}PLAYFILE_INFO;

#define     READ_STATUS      0
#define     WRITE_STATUS     1
#define     EXCPT_STATUS     2

int Net_select(int s, int sec, int usec, short x)
{
    int st = errno;
    struct timeval to;
    fd_set fs;
    to.tv_sec = sec;
    to.tv_usec = usec;
    FD_ZERO(&fs);
    FD_SET(s, &fs);

    switch(x)
    {
    case READ_STATUS:
        st = select(s+1, &fs, 0, 0, &to);
        break;
    case WRITE_STATUS:
        st = select(s+1, 0, &fs, 0, &to);
        break;
    case EXCPT_STATUS:
        st = select(s+1, 0, 0, &fs, &to);
        break;
    }
    return(st);
}
int BlockSendData(int  SenSocket, char *databuffer, int size) 
{
    int buffersize              = size;
    int leave_size              = size;
    int nRet                    = 0;
    int sec                     = 10;
    int usec                    = 0;
    int part_sensize            = 16000;
    
    if(SenSocket <= 0)
    {
        printf("Invailid Socket %d, Net_Protocol::BlockSendData() failed.\n", SenSocket);
        return nRet;
    }
    
    while(leave_size > 0)
    {
        int nneedsend = 0;
        if(leave_size  >= part_sensize)
            nneedsend = part_sensize;
        else
            nneedsend = leave_size;
        nRet = Net_select(SenSocket, sec, usec, WRITE_STATUS);
        if(nRet <= 0)
        {
            printf("%s %d socket %d Error:%d %s\n",__FUNCTION__,__LINE__,SenSocket, errno,strerror(errno));
            return -1;
        }
 
        nRet = send(SenSocket, databuffer + buffersize - leave_size , nneedsend, 0 );
        if(nRet <= 0)
        {
            int nerr = errno;
            if(nerr == EINTR)
                continue;
            if(nerr == EAGAIN)
            {
                usleep(100);
                continue;
            }
            printf("BlockSendData %d socket %d send : %d %s\n",__LINE__,SenSocket,nerr,strerror(nerr));
            break;
        }
        leave_size -= nRet;
        if(leave_size == 0)
        {
            nRet = 1;
            break;
        }
    }
    return nRet;
}

BOOL sendRecData(int socket, QMAPI_NET_DATA_PACKET stReadptr, char *pFrameBuf)
{
    NET_DATA_PACKET         netDataPacket;
    //int                     nPacketDataSize = 0;
    int                     nSendNo = 0;
    //int                     nWritePos = 0;

    int                     j = 0;
    int                     bufSize = 0;
    char                    *pSendBuf = NULL;
    int                     packNum = 0;

    memset(&netDataPacket, 0, sizeof(netDataPacket));
    // 将数据打包发送出去
    //nWritePos = 0;
    nSendNo = 0;
    //nPacketDataSize = 0;

    netDataPacket.stPackHead.nFlag = TLNET_FLAG;

    netDataPacket.stPackData.stFrameHeader.wFrameIndex = stReadptr.stFrameHeader.dwFrameIndex;
    netDataPacket.stPackData.stFrameHeader.dwTimeTick = stReadptr.stFrameHeader.dwTimeTick;
    netDataPacket.stPackData.stFrameHeader.dwVideoSize = stReadptr.stFrameHeader.dwVideoSize;
    netDataPacket.stPackData.stFrameHeader.wAudioSize = stReadptr.stFrameHeader.wAudioSize;
    netDataPacket.stPackData.stFrameHeader.byKeyFrame = (stReadptr.stFrameHeader.byFrameType == 0) ? TRUE : FALSE;

    bufSize = stReadptr.dwPacketSize;
    //printf("### stReadptr.dwPacketNO=%lu, size:%d, type:%d\n", stReadptr.stFrameHeader.dwFrameIndex, bufSize, stReadptr.stFrameHeader.byFrameType);
    /*if(bufSize > DATA_PACK_SIZE)
    {
        netDataPacket.stPackData.wIsSampleHead = TRUE;
    }
    else*/
    {
        netDataPacket.stPackData.wIsSampleHead = (stReadptr.dwPacketNO == 0)  ? TRUE : FALSE;
    }
    //pSendBuf = (char*)pFrameBuf;
    pSendBuf = (char*)stReadptr.pData;
    packNum = 0;
    packNum = stReadptr.dwPacketSize / DATA_PACK_SIZE;
    if((stReadptr.dwPacketSize % DATA_PACK_SIZE) != 0)
    {
        packNum += 1;
    }

    for(j = 0; j < packNum; j++)
    {
        if(j != packNum - 1)
        {
            netDataPacket.stPackData.wBufSize = DATA_PACK_SIZE;
        }
        else
        {
            netDataPacket.stPackData.wBufSize = bufSize - (packNum - 1)*DATA_PACK_SIZE;
        }
        netDataPacket.stPackHead.nSize = sizeof(DATA_PACKET) + netDataPacket.stPackData.wBufSize - sizeof(netDataPacket.stPackData.byPackData);
        netDataPacket.stPackData.stFrameHeader.byPackIndex= nSendNo ++; // packid,bPackId
       
      
        {
            //先发包头
            if(BlockSendData(socket, (char*)(&netDataPacket), sizeof(netDataPacket) - sizeof(netDataPacket.stPackData.byPackData)) <= 0)
            {
                printf("send failed: %s \n", strerror(errno));
                return FALSE; // 失败
            }
            if(BlockSendData(socket, pSendBuf + (DATA_PACK_SIZE * j), netDataPacket.stPackData.wBufSize) <= 0)
            {
                printf("send failed: %s \n", strerror(errno));
                return FALSE; // 失败
            }
        }
        
        netDataPacket.stPackData.wIsSampleHead = FALSE;
    }
    
    return TRUE;
}
static DWORD GetTickCount()
{
    struct timeval stTVal1;
    gettimeofday(&stTVal1, NULL);
    return (stTVal1.tv_sec%1000000)*1000+stTVal1.tv_usec/1000;
}
DWORD GetFileLength(char *sFileName)
{
    struct stat st;
    stat(sFileName, &st);
    return st.st_size;
}

static int dms_handle=0xAAA;//test

int do_OpenFile_Request(int hConnSock)
{
    TLNV_OPEN_FILE requsetFile;     //client send a struct to parse
    bzero(&requsetFile, sizeof(requsetFile));

    fd_set  rset, wset;
    struct timeval tv;
    int bPause = TRUE;  //2009.8.6
    int bEOF = FALSE;

    unsigned int SeekPlayStamp = 0; //when client send a command : seek in file, it is used for timetick, to find I frame's location
    int nReady;             //function select's ret, means whether there is a fd to operate
    int nBytes;             //send and recv's count
    int nRet = 0;
    DWORD      dwFileStamp = 0;
    FilePlay_INFO streamInfo;               /*填充好的发给客户端的结构体*/
    QMAPI_RECFILE_INFO        RecFileInfo;
    QMAPI_NET_DATA_PACKET     stReadptr;
    QMAPI_TIME                FrameTime;
    int playBackHandle=0;
    TLNV_VODACTION_REQ   VCRMsg;        /*接收到的客户端发来的含有命令和操作的结构体*/
    DWORD dwTick=0, dwVideoTick=0, dwStopTick=0;
    DWORD nVideoFrame;//已读视频帧数量
    nRet = TcpReceive(hConnSock, (char *)&requsetFile, sizeof(requsetFile));

    if(nRet != sizeof(requsetFile))
    {
        err();
        printf("file(%s)  line(%d) nRet == %d\n", __FILE__, __LINE__, nRet);
    }
    printf("### FileName:%s, SrmType:%lu\n", requsetFile.csFileName, requsetFile.nStreamType);

    playBackHandle = QMAPI_Record_PlayBackByName(dms_handle, requsetFile.csFileName);
    if(playBackHandle <= 0)
    {
        goto OUT_WORKER;
    }
    nRet = QMAPI_Record_PlayBackGetInfo(dms_handle, playBackHandle,&RecFileInfo);
    streamInfo.dwTotalTime = ((PLAYFILE_INFO *)playBackHandle)->dwTotalTime;
    streamInfo.dwFileSize = RecFileInfo.u32TotalSize;//lpFileData->nFileSize;
    if(nRet == 0)
    {
        streamInfo.dwStream1Width = RecFileInfo.u16Width;
        streamInfo.dwStream1Height = RecFileInfo.u16Height;
        streamInfo.dwAudioSamples = RecFileInfo.u16AudioSampleRate;
    }
    else
    {
        streamInfo.dwStream1Width = 704;
        streamInfo.dwStream1Height = 576;
    }

    QMAPI_NET_CHANNEL_PIC_INFO pinfo;
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, 0, &pinfo, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) != QMAPI_SUCCESS)
	{
		err();
        goto OUT_WORKER;
	}

	if (pinfo.stRecordPara.dwCompressionType == QMAPI_PT_H264)
		streamInfo.dwStream1CodecID = ENCODE_FORMAT_H264;
    else if (pinfo.stRecordPara.dwCompressionType == QMAPI_PT_H265)
        streamInfo.dwStream1CodecID = ENCODE_FORMAT_H265;
    
    streamInfo.dwAudioChannels = 1;
    streamInfo.dwAudioBits = 16;
    streamInfo.dwWaveFormatTag = 0x3e;
    printf("### TotalTime:%lu, %lux%lu, %lu\n", streamInfo.dwTotalTime, streamInfo.dwStream1Width,
        streamInfo.dwStream1Height, streamInfo.dwAudioSamples);

    memset(&stReadptr, 0, sizeof(stReadptr));
    if (0 != QMAPI_Record_PlayBackGetReadPtrPos(dms_handle, playBackHandle, &stReadptr, &FrameTime))
    {
        bEOF = TRUE;
        goto OUT_WORKER;
    }
    nVideoFrame=1;//已读了一帧
    streamInfo.dwPlayStamp = ((PLAYFILE_INFO *)playBackHandle)->firstStamp; //stReadptr.stFrameHeader.dwTimeTick;
    
    //send stream info 
    nRet = XSocket_TCP_Send(hConnSock, (char *)&streamInfo, sizeof(FilePlay_INFO));
    if (nRet != sizeof(FilePlay_INFO))
    {
        err();
        goto OUT_WORKER; 
    }
    sendRecData(hConnSock, stReadptr, NULL);//

    //begin send stream data
    dwFileStamp = streamInfo.dwPlayStamp;

//  printf("Prepare to Send data\n");   
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    dwTick=GetTickCount();
    dwVideoTick=stReadptr.stFrameHeader.dwTimeTick;
    for ( ; ; ) 
    {
        FD_SET(hConnSock, &rset);
        FD_SET(hConnSock, &wset);
        tv.tv_sec = 0;          //0.5 Sec
        tv.tv_usec = 40 * 1000;

        nReady = select(hConnSock + 1, &rset, &wset, NULL, &tv);
        if (nReady < 0)
        {
//          printf("Error select\n");
            goto OUT_WORKER;
        }
        if(FD_ISSET(hConnSock, &rset))
        {   //Receive VCR command from Client
            nRet = XSocket_TCP_Recv(hConnSock, (char *)&VCRMsg, sizeof(VCRMsg), &nBytes);
            if (nRet != QMAPI_SUCCESS || nBytes < sizeof(VCRMsg)) 
            {
                err();
//              printf("Receive VCR command failed ret:%d nBytes:%d strerror:%s\n", nRet, nBytes, strerror(errno));
                if(nRet == QMAPI_ERR_SOCKET_TIMEOUT)
                {
                    goto WRITE_SET;
                }
                goto OUT_WORKER;    //Client Close the socket or something error occur
            }
            printf("### recv:%lu ,%lu---------------\n", VCRMsg.dwCMD, VCRMsg.dwAction);
            if (VCRMsg.dwCMD == TL_VCRACTION_REQUEST) 
            {
                switch(VCRMsg.dwAction) 
                {
                case TL_PLAY_CMD_SEEK:
                    if (bEOF) bEOF = FALSE;
                    SeekPlayStamp = VCRMsg.dwData + dwFileStamp;
                    printf("VCR comamnd = REQUEST_SEEK bHasIndex  :%d dwData:%lu SeekPlayStamp:%d FileStamp:%lu\n", 0 , VCRMsg.dwData, SeekPlayStamp, dwFileStamp);
                    if(playBackHandle == 0){
                        goto OUT_WORKER;
                    }
                    if(QMAPI_Record_PlayBackControl(dms_handle, playBackHandle, (DWORD)QMAPI_REC_PLAYSETPOS, (char*)(&SeekPlayStamp), 4, NULL, NULL)==QMAPI_SUCCESS)
                    {   //定位成功,找I帧
                        while(1)
                        {
                            printf("Find I frame ....\n");
                            memset(&stReadptr, 0, sizeof(stReadptr));
                            if(0 != QMAPI_Record_PlayBackGetReadPtrPos(dms_handle, playBackHandle, &stReadptr, &FrameTime))
                            {
                                bEOF = TRUE;
                                printf("############################ get pos ERR \n");
                                goto OUT_WORKER;;
                            }
                            if(stReadptr.stFrameHeader.byFrameType==0){
                                printf("Find I frame success.\n");
                                break;
                            }
                        }//end of while
                        dwTick=GetTickCount();
                        dwVideoTick=stReadptr.stFrameHeader.dwTimeTick;
                        nVideoFrame=0;
                        //-----------------------
                    }
                    break;
                case TL_PLAY_CMD_PLAY:
                    printf("VCR comamnd = TL_PLAY_CMD_PLAY\n");
                    bPause = FALSE;
                    break;
                case TL_PLAY_CMD_PAUSE:
//                  printf("VCR comamnd = TL_PLAY_CMD_PAUSE\n");
                    bPause = TRUE;
///                 FD_CLR(hConnSock, &wset);
                    break;
                case TL_PLAY_CMD_FASTPLAY:
                    break;
                case TL_PLAY_CMD_FASTBACK:

                    break;
                case TL_PLAY_CMD_FIRST:
//                  printf("VCR comamnd = TL_PLAY_CMD_FIRST\n");
                    if (bEOF) bEOF = FALSE; 
//                  SetFilePosToBegin(hfileRanderAsf);
                    break;
                case TL_PLAY_CMD_LAST:
//                  printf("VCR comamnd = TL_PLAY_CMD_LAST\n");
                    if (bEOF) bEOF = FALSE;
//                  SetFilePosToEnd(hfileRanderAsf);
                    break;
                case TL_PLAY_CMD_STOP:
                    goto OUT_WORKER;
                    break;
                case TL_PLAY_CMD_PLAYPREV:

                    break;
                case TL_PLAY_CMD_SLOWPLAY:

                    break;
                case TL_PLAY_CMD_STEPIN:

                    break;
                case TL_PLAY_CMD_STEPOUT:

                    break;
                case TL_PLAY_CMD_RESUME:
                    
                    break;
                default:
                    break;
                };
            }
        }
WRITE_SET:
        //if(bEOF == TRUE || bPause == TRUE) continue;
        if( bPause == TRUE)
        {
            continue;
        }
        if(bEOF)
        {
           goto OUT_WORKER;
        }
#if 1
        if(stReadptr.stFrameHeader.dwVideoSize>0){
            if(nVideoFrame >0 && nVideoFrame %50==0){// 2秒检测一次
                DWORD nTick=GetTickCount()-dwTick;
                DWORD nSendTick=stReadptr.stFrameHeader.dwTimeTick-dwVideoTick;
//              printf("### Index:%lu(%lu), timetick:%lu, sys:%lu, V Tick:%lu\n", stReadptr.stFrameHeader.dwFrameIndex, nVideoFrame,
//                  stReadptr.stFrameHeader.dwTimeTick, nTick, nSendTick);
                if(nTick<nSendTick)
                {   //发数据太快
                    DWORD duration = nSendTick - nTick;
                    if (duration > 500) {
                        usleep(500*1000);
                        continue;//不读数据
                    }
                }
                dwTick=GetTickCount();
                dwVideoTick=stReadptr.stFrameHeader.dwTimeTick;
            }
            nVideoFrame++;
        }
#endif
        if (FD_ISSET(hConnSock, &wset)) {   //Can send data to Client
    //      printf("Begin Send a Data to Decoder\n");
            memset(&stReadptr, 0, sizeof(stReadptr));
            if(playBackHandle != 0)
            {
                memset(&stReadptr, 0, sizeof(stReadptr));
                if(0 != QMAPI_Record_PlayBackGetReadPtrPos(dms_handle, playBackHandle, &stReadptr, &FrameTime))
                {
                    bEOF = TRUE;
                    printf("############################ get pos ERR \n");
                    goto OUT_WORKER;;
                }
            }
            else
            {
                goto OUT_WORKER;
            }
            
            if (dwStopTick && GetTickCount()-dwStopTick>2000)
            {//发送完数据,最多等2秒没收到停止消息,则自动退出
                goto OUT_WORKER;
            }
            if (!dwStopTick && streamInfo.dwTotalTime<stReadptr.stFrameHeader.dwTimeTick)
            { //超过预算的时间戳且是虚拟的I帧
//              stReadptr.dwPacketSize==1024 && stReadptr.stFrameHeader.byFrameType==0){
                printf("wait stop(streamInfo.dwTotalTime=%lu,%lu)...\n", streamInfo.dwTotalTime, stReadptr.stFrameHeader.dwTimeTick);
                if(!dwStopTick) dwStopTick=GetTickCount();
                //继续发数据且等待结束
            }
            if (!sendRecData(hConnSock, stReadptr, NULL))
            {
                printf("%s[%d] File_ReadAndSend FAILED. GOTO OUT_WORKER\n", __FUNCTION__, __LINE__);
                goto OUT_WORKER;
            }
        } // Send data
    } //end for
OUT_WORKER:
    shutdown(hConnSock, SHUT_RDWR);
    close(hConnSock);

    if(playBackHandle != 0)
    {
         QMAPI_Record_PlayBackStop(dms_handle, playBackHandle);
    }
    printf("Leave do_OpenFile_Request main body sockfd=%d\n", hConnSock);
    return 0;
}

int mega_send_data(int nSock, char *buf, int len)
{
    // if nSock is closed, return bytes is -1
    int bytes = -1;
    
    int length = len;
    
//#ifdef MEGA_WRITE_SEND_XML
    do
    {
ReSend:
        bytes = send(nSock, buf, length, 0);
        if ( bytes > 0 )
           {
        length -= bytes;
        buf += bytes;
        }

        else if (bytes == -1)
            {
        if (errno == EINTR)
            goto ReSend;
    
         }

    }
    while ( length > 0 && bytes > 0 );
    if ( length == 0 )
        bytes = len; // 锟斤拷锟街斤拷
    return bytes;
}


int XSocket_TCP_Send(int sock, char *buf, int len)
{
    int     ret;

    //pthread_mutex_lock(&g_encoder_context[chn].snd_mutex);
    fd_set wfds;
    struct timeval tv;
        
    while(1)
    {
        FD_ZERO(&wfds);
        FD_SET(sock, &wfds);
        tv.tv_sec = 30;
        tv.tv_usec = 0;
        ret = select(sock + 1, NULL, &wfds, NULL, &tv);
        if (ret == 0)  /* timeout */
        {
            continue;
        }
        if(ret < 0 && ret == EINTR)
        {
            err();
            printf("---------------------------------------------------err(%d) 1\n", errno);
            usleep(1);
            continue;
        }
        break;
    }

    if( ret < 0 ) 
    {
        err();
        printf("---------------------------------------------------err(%d) 2\n", errno);
        return -1; //timeout obut the socket is not ready to write; or socket is error
    }

    ret = mega_send_data(sock, buf, len);

    //pthread_mutex_unlock(&g_encoder_context[chn].snd_mutex);

    return ret;
}

int XSocket_TCP_Recv(int hSock, char *pBuffer, unsigned long in_size, int *out_size)
{
    int     ret = 0;
    fd_set  fset;
    struct timeval  to;
    
    int bytes = -1;
    int length = in_size;
    time_t ticks_old, ticks_new;
    int tv_wait_recv_timeout = 15;
    int tv_sec = tv_wait_recv_timeout;
    
    to.tv_sec = 15;
    to.tv_usec = 0; 

    do
    {
ReRecv:
        FD_ZERO(&fset);
        FD_SET(hSock, &fset);

        if ( tv_sec <= 0 )
        {
            *out_size = in_size - length;
            return QMAPI_ERR_SOCKET_TIMEOUT;
        }
        to.tv_sec = tv_sec;
        to.tv_usec = 0;
        ticks_old = time(NULL);
  
        ret = select(hSock+1, &fset, NULL, NULL, &to);  //&to
        if (ret == 0)
        {
            //err();
            *out_size = in_size - length;
            return QMAPI_ERR_SOCKET_TIMEOUT;
        }
        if (ret < 0 && errno == EINTR)
        {
            err();
            goto ReRecv;
        }
        else if (ret < 0) 
        {
            err();
            *out_size = in_size - length;
            return QMAPI_ERR_SOCKET_ERR;
        }

        bytes = recv(hSock, pBuffer, length, 0);

        if ( bytes > 0 ) 
        {
            length -= bytes;
            pBuffer += bytes;
        }   
        
        ticks_new =  time(NULL);
        if(ticks_new - ticks_old > tv_wait_recv_timeout ||
            ticks_new - ticks_old < -tv_wait_recv_timeout )
        {   //预防修改系统时间导致偏差
            tv_sec = to.tv_sec;
        }
        else
        {
            tv_sec -= (ticks_new - ticks_old);
        }

    }while( length>0 && bytes>0 );
    
    *out_size = in_size - length;

    return QMAPI_SUCCESS;
}
int dms_ProcessOpenAlarm(int ConnSock)
{
	printf("### dms_ProcessOpenAlarm..\n");
	int 			ret = 0;
//	unsigned int	nError = 0;
	printf("Alarm 1\n");
	//读OPEN_HEAD头
	if(Net_select(ConnSock, 0, 500000, READ_STATUS)<=0)	
	{
		close(ConnSock);
		return -1;
	}
	OPEN_HEAD hdr;
	ret=recv(ConnSock,&hdr, sizeof(hdr), 0);
	if(ret!=sizeof(hdr) || hdr.nFlag!=TLNET_FLAG){
		close(ConnSock);
		return -1;
	}
	//查找DMS_CLIENT_INFO中的nSocket, 设置DMS_CLIENT_INFO中的nAlarmSocket
	if(SetClientAlarmInfo(hdr.nID, ConnSock))
		return 0;
	close(ConnSock);
	return -1;
}


int TL_net_server_init(int serverMediaPort, int nBufNum)
{
	int iRet;
	iRet = TL_Video_Net_ServerInit(1, serverMediaPort);
	

	TL_Video_Net_SetUserCheckFunc(TL_Video_CheckUser); //TL Video Right Check CallBack Initial 
	
	TL_Video_Net_GetServerInfo_Initial(TL_Server_Info_func);  //TL Video Get Server Information Callback Initial

	TL_Video_Net_GetChannelInfo_Initial(TL_Channel_Info_func); //TL Video Get Channel Information CallBack Initial

	TL_Video_Net_GetSensorInfo_Initial(TL_Sensor_Info_func); //TL Video Get Sensor Information CallBack Initial

	//CreateBackConnectThread(1);

	///TL Video Add New Protocol Cut User Information 
	TL_Video_Net_CutUserInfo_Initial((void *)TL_Client_Login_Right_Cut);

	///TL Video Add New Protocol Client Channel Right Check Initial
	TL_Video_Net_ChannelRightCheck_Initial(TL_User_Open_Data_Channel_Check_Func);
	
	// set up the callback function that be used for processing received information
	TL_Video_Net_SetServerRecvFunc(ReceiveFun);

	TL_Video_Net_SetSendBufNum(nBufNum);  // set send buffer's size

	return iRet;
}

int TL_net_server_start()
{
	return TL_Video_Net_ServerStart();
}

