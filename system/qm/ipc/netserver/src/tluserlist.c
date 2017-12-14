

#include "tlnetin.h"

#include "TLNetCommon.h"
#include "tlnetinterface.h"

#include "tlthrough.h"

int TL_Video_Net_GetClientInfo(void *pBuf, int nBufSize)
{
	int ret = -1;
    CLIENT_INFO *pTmp = NULL;
    CLIENT_NODE *pUsrNode = (CLIENT_NODE *)pBuf;
    
    pthread_mutex_lock(&g_clientlist.hClientMutex);
    ret = g_clientlist.nTotalNum;
    pthread_mutex_unlock(&g_clientlist.hClientMutex);
    
    if (NULL == pBuf)
    {
        return ret;
	}
    if (nBufSize < ret * sizeof(CLIENT_NODE))
    {
        return -1;
	}
    
	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	while (pTmp)
	{
        pUsrNode->ip = pTmp->ip;
        pUsrNode->port = pTmp->port;
        pUsrNode->hUserID = pTmp->hMsgSocket;
        pUsrNode->level = pTmp->level;
        strcpy(pUsrNode->szUserName,pTmp->szUserName);
        strcpy(pUsrNode->szUserPsw,pTmp->szUserPsw);
        
        pUsrNode++;
		pTmp = pTmp->pNext;
	}
	pthread_mutex_unlock(&g_clientlist.hClientMutex);
    
    return ret;
}



int TcpReceiveEX2(int hSock, char *pBuffer, DWORD nSize)
{
	int			ret = 0;
	fd_set			fset;
	struct timeval  	to;

	DWORD			dwRecved = 0;

	bzero(&to, sizeof(to));

	while(dwRecved < nSize)
	{
		FD_ZERO(&fset);
		FD_SET(hSock, &fset);
		to.tv_sec = 30;
		to.tv_usec = 0;
	
		if (hSock < 0 || hSock > 65535)		///2009.8.1 zhang del =
		{
			err();
			printf("error hSock = %d\n", hSock);
			return -2;
		}
	
       		ret = select(hSock+1, &fset, NULL, NULL, &to);  //&to
		if ( ret == 0 || (ret == -1 && errno == EINTR))
		{
			if(errno == EINTR)
			{
			   printf("%s:%d  interrupted \n",__FUNCTION__,__LINE__);
			   continue;
			}

			if(ret == 0)
			{
			   printf("%s:%d socket %d  disconnected \n",__FUNCTION__,__LINE__,hSock);
			}
			
			return -1;
		}
	
		if (ret == -1) //&& errno == ECONNREFUSED)
		{
			err();
			return -1;
		}

		if(!FD_ISSET(hSock, &fset))
		{
			err();
			return -2;
		}

		ret = recv(hSock, pBuffer + dwRecved, nSize - dwRecved,0);
		if ( (ret < 0) && (errno == EAGAIN || errno == EINTR))
		{
			err();
			return -2;
		}
		if (ret <= 0)
		{
			return -1;
		}
		
		dwRecved += ret;
	}
	return dwRecved;
}

#if 0
int TcpReceive(int hSock, char *pBuffer, DWORD nSize)
{
	int ret = 0;
	fd_set fset;
	struct timeval to;
	DWORD dwRecved = 0;

	bzero(&to, sizeof(to));
	
	if (nSize <= 0)
	{
		return -2;
	}

	while (dwRecved < nSize)
	{
		FD_ZERO(&fset);
		FD_SET(hSock, &fset);
		to.tv_sec = 15;
		to.tv_usec = 0;

		if (hSock < 0 || hSock > 65535)	///2009.8.1 zhang del =
		{
			err();
			return -2;
		}

		ret = select(hSock+1, &fset, NULL, NULL, &to);  //&to
		if ( ret == 0 || (ret == -1 && errno == EINTR))
		{
			err();
			return -1;
		}

		if (ret == -1) //&& errno == ECONNREFUSED)
		{
			err();
			return -2;
		}

		if(!FD_ISSET(hSock, &fset))
		{
			err();
			return -1;
		}

		ret = recv(hSock, pBuffer + dwRecved, nSize - dwRecved, 0);
		if ( (ret < 0) && (errno == EAGAIN || errno == EINTR))
		{
			//err();
			printf("TcpReceive blocked or interruted \n");
			continue;
		}
		if (ret <= 0)
		{
			printf("TcpReceive error : %s  %d-%d  ret is %d\n",strerror(errno),EINTR,errno,ret);
			return -2;
		}

		dwRecved += ret;
	}
	return dwRecved;
}
#else
/*TcpReceive
  读nSize字节数据到pBuffer,15秒超时
  */
int TcpReceive(int hSock, char *pBuffer, DWORD nSize)
{
	int ret = 0;
	fd_set fset;
	struct timeval to;
	DWORD dwRecved = 0;
	bzero(&to, sizeof(to));
	if (nSize <= 0)
	{
		return -2;
	}

	to.tv_sec = 15;
	to.tv_usec = 0;
	while (dwRecved < nSize)
	{
		FD_ZERO(&fset);
		FD_SET(hSock, &fset);
		if (hSock < 0 || hSock > 65535)	///2009.8.1 zhang del =
		{
			err();
			return -2;
		}

		ret = select(hSock+1, &fset, NULL, NULL, &to);  //&to
		if ( ret == 0 || (ret == -1 && errno == EINTR))
		{
			err();
			//printf("TcpReceive error ret %d error %d \n" , ret , errno);
			return -1;
		}

		if (ret == -1) //&& errno == ECONNREFUSED)
		{
			err();
			return -2;
		}

		if(!FD_ISSET(hSock, &fset))
		{
			printf("TcpReceive other fd, re select(%lu,%lu).... \n", to.tv_sec, to.tv_usec);
			continue;//剩余时间,重新检测
			//err();
//			return -1;
		}
		ret = recv(hSock, pBuffer + dwRecved, nSize - dwRecved, 0);
		if ( (ret < 0) && (errno == EAGAIN || errno == EINTR))
		{
			//err();
			printf("TcpReceive blocked or interruted \n");
			continue;
		}
		if (ret <= 0)
		{
			printf("TcpReceive error : %s  %d-%d  ret is %d\n",strerror(errno),EINTR,errno,ret);
			return -2;
		}

		dwRecved += ret;
	}
	return dwRecved;
}
#endif
int TcpReceiveExt(int hSock, char *pBuffer, DWORD nSize)
{
	int ret = 0;
	fd_set fset;
	struct timeval to;
	DWORD dwRecved = 0;

	bzero(&to, sizeof(to));

	if (nSize <= 0)
	{
		return -2;
	}
	
	while (dwRecved < nSize)
	{
		FD_ZERO(&fset);
		FD_SET(hSock, &fset);
		to.tv_sec = 30;
		to.tv_usec = 0;

		if (hSock < 0 || hSock > 65535)	///2009.8.1 zhang del =
		{
			err();
			return -2;
		}

		ret = select(hSock+1, &fset, NULL, NULL, &to);  //&to
		if ( ret == 0 || (ret == -1 && errno == EINTR))
		{
			err();
			continue;
		}

		if (ret == -1)
		{
			err();
			return -2;
		}

		if (!FD_ISSET(hSock, &fset))
		{
			err();
			continue;
		}

		ret = recv(hSock, pBuffer + dwRecved, nSize - dwRecved, 0);
		if ( (ret < 0) && (errno == EAGAIN || errno == EINTR))
		{
			err();
			continue;
		}
		if (ret <= 0)
		{
			err();
			return -2;
		}

		dwRecved += ret;
	}
	return dwRecved;
}

//
void	TcpMsgRecvThread(void *pPar)
{
	int updateFlag = 0;	
	CLIENT_INFO *pLogonClient = NULL;
	COMM_HEAD *pCommHead = NULL;
	char *pCommData = NULL;
	MSG_HEAD msgHead;
	//COMM_HEAD keepCommHead;
	struct timeval to;
	fd_set fset;
	int ret;
	char	 *buffer = NULL; 

	prctl(PR_SET_NAME, (unsigned long)"TcpMsgRecv", 0,0,0);
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

    //keepCommHead.nFlag = TLNET_FLAG;
    //keepCommHead.nCommand = NETCMD_KEEP_ALIVE;
    //keepCommHead.nBufSize = 0;
       
	buffer = (char *)malloc(NETCMD_MAX_SIZE/2/8);
    if (NULL == buffer)
    {
#ifdef OLD_H_SUPPORT
		printf("file(%s), line(%d) fatal this system will restart !!!\n", __FILE__, __LINE__);
		restartSystem();
#endif
		return ;
    }
    pCommHead = (COMM_HEAD*)buffer;
    pCommData = buffer + sizeof(COMM_HEAD);

	while (!g_serinfo.bServerExit)
	{ 
		pthread_mutex_lock(&g_serinfo.msgThreadMutex);
		while (g_serinfo.msgWaitThreadNum == 0) 
		{
			pthread_cond_wait(&g_serinfo.msgThreadCond, &g_serinfo.msgThreadMutex);
			printf("#### %s %d\n", __FUNCTION__, __LINE__);
			if (g_serinfo.bServerExit)
			{
				break;
			}
		}

		if (g_serinfo.bServerExit)
		{
			pthread_mutex_unlock(&g_serinfo.msgThreadMutex);
			break;
		}
		//printf("#### %s %d\n", __FUNCTION__, __LINE__);

		pLogonClient = FindWaitProccessClient();
		//printf("#### %s %d\n", __FUNCTION__, __LINE__);
		if (NULL == pLogonClient)
		{
			err();
			pthread_mutex_unlock(&g_serinfo.msgThreadMutex);
			continue;
		}
		//printf("#### %s %d\n", __FUNCTION__, __LINE__);
		g_serinfo.msgProcessThreadNum++;
		g_serinfo.msgWaitThreadNum--;
		if (g_serinfo.msgWaitThreadNum < 0)
		{
			g_serinfo.msgWaitThreadNum = 0;
		}

		pthread_mutex_unlock(&g_serinfo.msgThreadMutex);

		//
		FD_ZERO(&fset);
		bzero(&to, sizeof(to));

		//nFlag = fcntl(hMsgSock,F_GETFL,0);
		//fcntl(hMsgSock,F_SETFL,nFlag|O_NONBLOCK);
		pLogonClient->nKeepAlive = 0;
		pLogonClient->status = RUN;

		while (STOP != pLogonClient->status)
		{
			if (!getSendAvStatus())
			{
				if (!updateFlag)
				{
					break;
				}
			}

			//printf("#### %s %d\n", __FUNCTION__, __LINE__);
			ret = TcpReceive(pLogonClient->hMsgSocket, (char *)pCommHead, sizeof(COMM_HEAD));
			//printf("#### %s %d\n", __FUNCTION__, __LINE__);
			pLogonClient->nKeepAlive ++;
			if (pLogonClient->nKeepAlive >= 3) 
			{
				err();
				break;
			}
			if (-2 == ret)
			{
				err();
				break;
			}
			if (-1 == ret)
			{
				err();
				continue;
			}
			pLogonClient->nKeepAlive = 0; 	
			ConvertHead(pCommHead, sizeof(COMM_HEAD));
			if (TLNET_FLAG != pCommHead->nFlag)
			{
				
				continue;
			}
			//printf("#### %s %d, nFlag=%ld, nCommand=%ld--0x%X, nChannel=%ld, nErrorCode=%ld, nBufSize=%ld\n",
            //            __FUNCTION__, __LINE__, pCommHead->nFlag, pCommHead->nCommand, (int)pCommHead->nCommand, pCommHead->nChannel, pCommHead->nErrorCode, pCommHead->nBufSize);

			if (pCommHead->nBufSize > 0)
			{
				
				if (pCommHead->nCommand == NETCMD_UPDATE_FILE)
				{
					//printf("Update file: %d-%d\n", pCommHead->nBufSize, pLogonClient->hMsgSocket);					
					
					if (!updateFlag)
					{
						pauseSendAvData();	
						updateFlag = 1;
					}					
				}

				//printf("#### %s %d\n", __FUNCTION__, __LINE__);
				ret = TcpReceive(pLogonClient->hMsgSocket, pCommData, pCommHead->nBufSize);
				if (-2 == ret)
				{
					err();
					break;
				}
				if (-1 == ret)
				{
					err();
					continue;
				}
			}
			//only translate this command
			if (pCommHead->nCommand == NETCMD_KEEP_ALIVE)
			{
				pLogonClient->nKeepAlive = 0;
				send(pLogonClient->hMsgSocket,pCommHead,sizeof(*pCommHead),0);
				continue;
			}
			pLogonClient->nKeepAlive = 0;
			g_netKeepAlive = 0; 

			msgHead.clientIP = pLogonClient->ip;
			msgHead.clientPort = pLogonClient->port;
			msgHead.nLevel = pLogonClient->level;
			msgHead.nID = pLogonClient->hMsgSocket;
			memcpy(msgHead.szUserName,pLogonClient->szUserName,QMAPI_NAME_LEN);
			memcpy(msgHead.szUserPsw,pLogonClient->szUserPsw,QMAPI_PASSWD_LEN);
			msgHead.nCommand = pCommHead->nCommand;
			msgHead.nChannel = pCommHead->nChannel;
			msgHead.nBufSize = pCommHead->nBufSize;
            //printf("#### %s %d\n", __FUNCTION__, __LINE__);
			TranslateCommand(&msgHead, (char *)pCommHead+sizeof(COMM_HEAD)); 
			//printf("#### %s %d\n", __FUNCTION__, __LINE__);
		}
        
		pLogonClient->status = STOP;

		if (REMOTE_HOST_DIR.tcpLocalSock == pLogonClient->hMsgSocket)
		{
			printf(" %s(%d) - %d , %s, remote client exit ok\n", __FILE__, __LINE__, errno, strerror(errno));
			pthread_mutex_lock(&REMOTE_HOST_DIR.dirMutex);			
			shutdown(REMOTE_HOST_DIR.tcpLocalSock, 2);
			close(REMOTE_HOST_DIR.tcpLocalSock);
			REMOTE_HOST_DIR.tcpLocalSock = -1;
			REMOTE_HOST_DIR.status = LOGO_OFF;
			REMOTE_HOST_DIR.strIp[0] = 0;
			pthread_mutex_unlock(&REMOTE_HOST_DIR.dirMutex);

		}

		//StopUdpNode(pLogonClient->hMsgSocket);
		StopTcpNode(pLogonClient->hMsgSocket);
		//pthread_cond_signal(&g_serinfo.dataQuitMangCond);
		usleep(1);

		pthread_mutex_lock(&g_serinfo.msgQuitMangMutex);
		g_serinfo.msgQuitThreadNum ++;
		g_serinfo.msgProcessThreadNum --;
		if (g_serinfo.msgProcessThreadNum < 0)
		{
			g_serinfo.msgProcessThreadNum = 0;
		}
		pthread_mutex_unlock(&g_serinfo.msgQuitMangMutex);

		pthread_cond_signal(&g_serinfo.msgQuitMangCond);

	} // while (!g_serinfo.bServerExit)
	
	g_serinfo.nMsgThreadCount--;
	if (buffer) 
	{
		free(buffer);
		buffer = NULL;
	}
}

//
void	TcpMsgQuitThread(void *pPar)
{
    int hFind = 0;
 	prctl(PR_SET_NAME, (unsigned long)"TcpMsgQuit", 0,0,0);
    pthread_detach(pthread_self());

    while (!g_serinfo.bServerExit)
    {
        pthread_mutex_lock(&g_serinfo.msgQuitMangMutex);
        while(g_serinfo.msgQuitThreadNum == 0)
        {
            pthread_cond_wait(&g_serinfo.msgQuitMangCond,&g_serinfo.msgQuitMangMutex);
        }
            
        if (g_serinfo.bServerExit)
        {
            pthread_mutex_unlock(&g_serinfo.msgQuitMangMutex);
            break;
        }

        while((hFind = GetExitClient()) > 0)
        {
            printf("call logout \n");
            ClientLogoff(hFind);
        }

        g_serinfo.msgQuitThreadNum = 0;
        pthread_mutex_unlock(&g_serinfo.msgQuitMangMutex);
    }
}

void ClientLogon(struct sockaddr_in addr, USER_INFO userInfo, int nID, int nLevel)
{
	CLIENT_INFO *pClientInfo = NULL;
	CLIENT_INFO *pTmp = NULL;

	pClientInfo =(CLIENT_INFO	*) malloc(sizeof(CLIENT_INFO));
	memset(pClientInfo, 0, sizeof(CLIENT_INFO));

	pClientInfo->ip = addr.sin_addr.s_addr;
	pClientInfo->port = addr.sin_port;
	strcpy(pClientInfo->szUserName,userInfo.szUserName);
	strcpy(pClientInfo->szUserPsw,userInfo.szUserPsw);
	pClientInfo->level = nLevel;
	pClientInfo->hMsgSocket = nID;
	pClientInfo->status = READY;
	pClientInfo->nKeepAlive = 0;
	pClientInfo->pNext = NULL;

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	if (NULL == pTmp)
		g_clientlist.pNext = pClientInfo;
	else
	{
		while(pTmp->pNext)
			pTmp = pTmp->pNext;
		pTmp->pNext = pClientInfo;
	}
	g_clientlist.nTotalNum ++;
	pthread_mutex_unlock(&g_clientlist.hClientMutex);
}

int ClientLogonExt(struct sockaddr_in addr, USER_INFO userInfo, int nID, int nLevel)
{
	CLIENT_INFO *pClientInfo = NULL;
	CLIENT_INFO *pTmp = NULL;

	if (nID < 0)
	{
		return -1;
	}
	
	pClientInfo =(CLIENT_INFO	*) malloc(sizeof(CLIENT_INFO));
	if (pClientInfo == NULL) 
	{
		if (nID >= 0)
		{
			shutdown(nID, 2);
			close(nID);
		}
		return -1;
	}
	memset(pClientInfo, 0, sizeof(CLIENT_INFO));

	pClientInfo->ip = addr.sin_addr.s_addr;
	pClientInfo->port = addr.sin_port;
	strcpy(pClientInfo->szUserName,userInfo.szUserName);
	strcpy(pClientInfo->szUserPsw,userInfo.szUserPsw);
	pClientInfo->level = nLevel;
	pClientInfo->hMsgSocket = nID;
	pClientInfo->status = READY;
	pClientInfo->nKeepAlive = 0;
	pClientInfo->pNext = NULL;

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	if (NULL == pTmp)
	{
		g_clientlist.pNext = pClientInfo;
	}
	else
	{
		while(pTmp->pNext)
			pTmp = pTmp->pNext;
		pTmp->pNext = pClientInfo;
	}
	g_clientlist.nTotalNum ++;
	pthread_mutex_unlock(&g_clientlist.hClientMutex);
	
	return 0;
}

BOOL ClientLogoff(int nID)
{
    printf("ClientLogoff \n");
	BOOL bRet = FALSE;
	CLIENT_INFO *pTmp = NULL;
	CLIENT_INFO *pPre = NULL;

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	if (NULL != pTmp)
	{
		if (nID == pTmp->hMsgSocket)
		{
			g_clientlist.pNext = pTmp->pNext;
			shutdown(pTmp->hMsgSocket, 2);
			close(pTmp->hMsgSocket);
			if(pTmp->hAlarmSocket){
				close(pTmp->hAlarmSocket);
				pTmp->hAlarmSocket=0;
			}
			//g_clientlist.pNext = pTmp->pNext;
			free(pTmp);
			g_clientlist.nTotalNum --;
			bRet = TRUE;
		}
		else
		{
			while (pTmp)
			{
				pPre = pTmp;
				pTmp = pTmp->pNext;
				if (pTmp && (nID == pTmp->hMsgSocket))
				{
					pPre->pNext = pTmp->pNext;
					shutdown(pTmp->hMsgSocket, 2);
					close(pTmp->hMsgSocket);
					if(pTmp->hAlarmSocket){
						close(pTmp->hAlarmSocket);
						pTmp->hAlarmSocket=0;
					}
					free(pTmp);
					g_clientlist.nTotalNum --;
					bRet = TRUE;
					break;
				}
			}
		}
	}
	pthread_mutex_unlock(&g_clientlist.hClientMutex);

	return bRet;
}

CLIENT_INFO *FindWaitProccessClient()
{
	BOOL bFind = FALSE;
	CLIENT_INFO *pFind = NULL;

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pFind = g_clientlist.pNext;
	while(pFind)
	{
		if(READY == pFind->status)
		{
			bFind = TRUE;
			break;
		}
		pFind = pFind->pNext;
	}
	pthread_mutex_unlock(&g_clientlist.hClientMutex);
	if(!bFind)
		pFind = NULL;

	return pFind;
}

int GetExitClient()
{
    int nRet = 0;
	CLIENT_INFO *pTmp = NULL;

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	while (pTmp)
	{
		if (STOP == pTmp->status)
		{
			nRet = pTmp->hMsgSocket;
			break;
		}
		pTmp = pTmp->pNext;
	}
	pthread_mutex_unlock(&g_clientlist.hClientMutex);

	return nRet;
}

int ClientExist(unsigned long ip,unsigned long port)
{
    int nRet = 0;
	CLIENT_INFO *pTmp = NULL;

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	while (pTmp)
	{
		if ((pTmp->ip == ip) && (pTmp->port == port))
        {
            nRet = pTmp->hMsgSocket;
			break;
        }
		pTmp = pTmp->pNext;
	}
	pthread_mutex_unlock(&g_clientlist.hClientMutex);

	return nRet;
}

BOOL GetClient(int nID, CLIENT_INFO *pClientInfo)
{
	CLIENT_INFO *pTmp = NULL;
	BOOL bRet = FALSE;

	if (NULL == pClientInfo)
	{
		return FALSE;
	}

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	while (pTmp)
	{
		if (pTmp->hMsgSocket == nID)
		{
			memcpy(pClientInfo,pTmp,sizeof(CLIENT_INFO));
			bRet = TRUE;
			break;                     
		}
		pTmp = pTmp->pNext;
	}
	pthread_mutex_unlock(&g_clientlist.hClientMutex);

	return bRet;
}
BOOL SetClientAlarmInfo(int nID, int fdAlarm)
{
	CLIENT_INFO *pTmp = NULL;
	BOOL bRet = FALSE;

	pthread_mutex_lock(&g_clientlist.hClientMutex);
	pTmp = g_clientlist.pNext;
	while (pTmp)
	{
		if (pTmp->hMsgSocket == nID)
		{
			if(pTmp->hAlarmSocket)
				close(pTmp->hAlarmSocket);
			pTmp->hAlarmSocket=fdAlarm;
			pTmp->nAlarmFlag=1;//使用alarm socket
			bRet = TRUE;
			break;                     
		}
		pTmp = pTmp->pNext;
	}
	pthread_mutex_unlock(&g_clientlist.hClientMutex);
	return bRet;
}

int StopAllClient()
{
    int nNum = 0;
    CLIENT_INFO *pClient = NULL;

    pthread_mutex_lock(&g_clientlist.hClientMutex);
    
    pClient = g_clientlist.pNext;
    while(pClient)
    {
        pClient->status = STOP;
		printf("STOP  %s:%d \n",__FUNCTION__,__LINE__);
        nNum ++;
        pClient = pClient->pNext;
    }
    
    pthread_mutex_unlock(&g_clientlist.hClientMutex);

    return nNum;
}

int FreeAllClient()
{
    int nFreeNum = 0;
    CLIENT_INFO *pTmp = NULL;

    pthread_mutex_lock(&g_clientlist.hClientMutex);
    while (g_clientlist.pNext)
    {
        pTmp = g_clientlist.pNext;
        shutdown(pTmp->hMsgSocket, 2); 
		close(pTmp->hMsgSocket); 
		if(pTmp->hAlarmSocket){
			close(pTmp->hAlarmSocket);
			pTmp->hAlarmSocket=0;
		}
        g_clientlist.pNext = pTmp->pNext;
        free(pTmp);
        nFreeNum ++;
    }
    if (nFreeNum != g_clientlist.nTotalNum)
    {
        err();
	}

    g_clientlist.nTotalNum = 0;
    pthread_mutex_unlock(&g_clientlist.hClientMutex);
    return 0;
}

/*void CheckClientAlive()
{
    int         ret;
    int         nCount = 0;
    CLIENT_INFO *pTmp = NULL;
    COMM_HEAD   commHead;

    bzero(&commHead,sizeof(COMM_HEAD));
    commHead.nFlag = TLNET_FLAG;
    commHead.nCommand = NETCMD_KEEP_ALIVE;
    ConvertHead(&commHead,sizeof(COMM_HEAD));
    
    pthread_mutex_lock(&g_clientlist.hClientMutex);
    pTmp = g_clientlist.pNext;
    
    while(pTmp)
    {
        if(2 == pTmp->nKeepAlive)
        {
            ret = send(pTmp->hMsgSocket,&commHead,sizeof(COMM_HEAD),0);
            if(ret < 0)
            {
                pTmp->nKeepAlive = 4;
                pTmp->status = STOP;
				printf("STOP  %s:%d \n",__FUNCTION__,__LINE__);
            }
        }
        else if(pTmp->nKeepAlive >= 4)
        {
            pTmp->status = STOP;
			printf("STOP  %s:%d \n",__FUNCTION__,__LINE__);
            nCount++;
        }
        pTmp->nKeepAlive++;
        pTmp = pTmp->pNext;
    }
    pthread_mutex_unlock(&g_clientlist.hClientMutex);
    
    if(nCount > 0)
        pthread_cond_signal(&g_serinfo.msgQuitMangCond);    
}*/

