/******************************************************************************
******************************************************************************/

#include <asm/ioctls.h>
#include <errno.h>
#include "tlnetin.h"

#include "TLNetCommon.h"
#include "tlnetinterface.h"

#include "QMAPIAV.h"

SEND_NODE *WaitReadyProcessNode();

static int g_sendAvDataFlag = 1;
int InsertPlayNodeExt(SEND_NODE *pNodeHead, pthread_mutex_t hMutex, SEND_NODE *pNew);

int pauseSendAvData( void )
{
    g_sendAvDataFlag = 0;

    return 0;
}

int resumeSendAvData( void )
{
    g_sendAvDataFlag = 1;

    return 0;
}

int getSendAvStatus( void )
{
    return g_sendAvDataFlag;
}
int TcpSendOnePacket(int socket , void * pbuff , unsigned int len)
{
	int ret =0 ;
	//printf("TcpSendOnePacket %d \n" , len);
	do
	{
		ret = send(socket, pbuff, len, 0);
		if(ret  <  0 && errno == EAGAIN)
			usleep(10000);
	}while(ret  <  0 && errno == EAGAIN);
	
	return ret;
}
//发送最后一个包
int XSendLastPacket(int socket , NET_DATA_PACKET * pnetDataPack ,void * pAvBuff , int s32AvLen , void *pAudioBuff , int s32AudioLen , unsigned char u8PacketIndex)
{
	int sendlen = 0;
	int sendaudiopacket =0;
	int ret = 0;
	pnetDataPack->stPackData.wBufSize = s32AvLen ;
	pnetDataPack->stPackHead.nSize = sizeof(DATA_PACKET) + pnetDataPack->stPackData.wBufSize - sizeof(pnetDataPack->stPackData.byPackData);
	memcpy(pnetDataPack->stPackData.byPackData , pAvBuff , pnetDataPack->stPackData.wBufSize);
	if((pnetDataPack->stPackData.wBufSize + s32AudioLen )<= sizeof(pnetDataPack->stPackData.byPackData ) ) //如果最后一个包空间足够将音频与视频数据一起打包出去
	{
		if(s32AudioLen > 0)
		{
			memcpy(pnetDataPack->stPackData.byPackData + pnetDataPack->stPackData.wBufSize , pAudioBuff , s32AudioLen);
			pnetDataPack->stPackData.wBufSize += s32AudioLen;
			pnetDataPack->stPackHead.nSize  += s32AudioLen;
		}
	}
	else if(s32AudioLen > 0)  //如果最后一个包空间不够,再单独发音频包
	{
		
		sendlen = sizeof(pnetDataPack->stPackData.byPackData )  - pnetDataPack->stPackData.wBufSize ;
		memcpy(pnetDataPack->stPackData.byPackData + pnetDataPack->stPackData.wBufSize , pAudioBuff , sendlen);
		pnetDataPack->stPackData.wBufSize += sendlen;
		pnetDataPack->stPackHead.nSize  += sendlen;
		s32AudioLen -= sendlen;
		sendaudiopacket =1;
	}
	pnetDataPack->stPackData.stFrameHeader.byPackIndex = u8PacketIndex;
	ret = TcpSendOnePacket(socket, pnetDataPack, sizeof(NET_DATA_PACKET) - sizeof(pnetDataPack->stPackData.byPackData) + pnetDataPack->stPackData.wBufSize  );
	if(sendaudiopacket) //send a audio data alone
	{
		pnetDataPack->stPackData.wIsSampleHead = FALSE;
		sendaudiopacket =0;
		pnetDataPack->stPackData.wBufSize = s32AudioLen;
		memcpy(pnetDataPack->stPackData.byPackData , pAudioBuff + sendlen , s32AudioLen);
		pnetDataPack->stPackHead.nSize = sizeof(DATA_PACKET) + pnetDataPack->stPackData.wBufSize - sizeof(pnetDataPack->stPackData.byPackData);
		pnetDataPack->stPackData.stFrameHeader.byPackIndex = u8PacketIndex + 1;
		ret = TcpSendOnePacket(socket, pnetDataPack, sizeof(NET_DATA_PACKET) - sizeof(pnetDataPack->stPackData.byPackData)  + pnetDataPack->stPackData.wBufSize  );
	}
	return ret ;
}
//
void NetServerTcpSendThread(void *pPar)
{
    SEND_NODE           *pNode = NULL;
    //NET_DATA_PACKET  netDataPacket;
    NET_DATA_PACKET  netDataPack ;          
    QMAPI_MBUF_POS_POLICY_E enBufPolicy = 0;
    QMAPI_NET_DATA_PACKET stAVDataPacket;
    QMAPI_NET_DATA_PACKET stAudioDataPacket;
    unsigned char audiobuff[2048+128];
	unsigned int  audiolen = 0;
    int                 streamNo = 0;
	int avbufSize =0;
	int packNum = 0;

	char * pSendBuf = NULL;
	unsigned int j = 0;
    BOOL                bFirstRead = TRUE;
    int                 ret;
    int                 loop_send = 0;
    int 				s32Reader = 0;
    struct timeval      to;
    //int                   opt;
    fd_set              fset;
    //struct linger     ling;
    //long              sndbuf_data_size = 0;
    //int                   sock_buffer_changed = 0;
    //int                   i;
    static int                  send_timeout_tick = 0;
    int                 is_internet = 0;
    int                 select_err_tick = 0;
    int                 snd_err_tick2 = 0;
    prctl(PR_SET_NAME, (unsigned long)"NetServerTcpSendThread", 0,0,0);
    FD_ZERO(&fset);
    bzero(&to, sizeof(to));

    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    while (!g_serinfo.bServerExit)
    {
        pthread_mutex_lock(&g_serinfo.dataThreadMutex);
        while (g_serinfo.dataWaitThreadNum == 0) //
        {
            pthread_cond_wait(&g_serinfo.dataThreadCond, &g_serinfo.dataThreadMutex);
            if (g_serinfo.bServerExit)
            {
                break;
            }
        }
        if (g_serinfo.bServerExit)
        {
            pthread_mutex_unlock(&g_serinfo.dataThreadMutex);
            break;
        }
        pNode = WaitReadyProcessNode();
        if (NULL == pNode)
        {
            pthread_mutex_unlock(&g_serinfo.dataThreadMutex);
            continue;
        }
        if (CheckIpIsInternet(pNode->nClientTcpIP))
        {
            is_internet = 1;
            //printf("this ip is an internet ip(%s), and we will adjust the send rate\n", inet_ntoa(addr.sin_addr));
        }
        else
        {
            is_internet = 0;
            //printf("this ip is an lan ip(%s)\n", inet_ntoa(addr.sin_addr));
        }

        g_serinfo.dataProcessThreadNum ++;        
        g_serinfo.dataWaitThreadNum --;
        if (g_serinfo.dataWaitThreadNum < 0)
        {
            g_serinfo.dataWaitThreadNum = 0;
        }

        pthread_mutex_unlock(&g_serinfo.dataThreadMutex);
        //use main stream anyway
        streamNo = pNode->nStreamType;

        to.tv_sec = 30;
        to.tv_usec = 0;
        ret = setsockopt(pNode->hSendSock, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));
        //nFlag = fcntl(pNode->hSendSock, F_GETFL, 0);
        //fcntl(pNode->hSendSock,F_SETFL,nFlag|O_NONBLOCK);

        bFirstRead = TRUE;
        while (STOP != pNode->status)
        {
            if (g_serinfo.bServerExit)
            {
                break;
            }
            if (STOP == pNode->status)
            {
                break;
            }
            // [zhb][add][2006-05-20]
            if (!getSendAvStatus())
            {
                break;
            }
						
            if(s32Reader == 0)
            {
                ret = QMapi_buf_add_reader(0, pNode->nSerChannel, streamNo, &s32Reader);
                if (ret)
                {
                    err();
                    goto out;
                }
            }

            if(bFirstRead)
            {
                enBufPolicy = QMAPI_MBUF_POS_LAST_WRITE_nIFRAME;
            }
            else
            {
                enBufPolicy = QMAPI_MBUF_POS_CUR_READ;
            }
            memset( &stAVDataPacket  , 0 , sizeof(stAVDataPacket));

            ret = QMapi_buf_get_readptrpos(0, s32Reader, enBufPolicy, 0, &stAVDataPacket);
            if(ret)
            {	
                bFirstRead = TRUE;
                usleep(20000);
                err();
                continue;
            }
            bFirstRead = FALSE;
            audiolen = 0;
	
getaudio:
            memset(&stAudioDataPacket , 0 , sizeof(stAudioDataPacket));
            //下一帧是音频部分
            QMAPI_FRAME_TYPE_E enNextFrametype = QMAPI_FRAME_I;
            ret = QMapi_buf_get_next_frametype(0, s32Reader, QMAPI_MBUF_POS_CUR_READ, 0, &enNextFrametype);
			if(0 == ret && QMAPI_FRAME_A == enNextFrametype)
			{
				ret = QMapi_buf_get_readptrpos(0, s32Reader, QMAPI_MBUF_POS_CUR_READ, 0, &stAudioDataPacket);
				if(ret)
				{	
					memset(&stAudioDataPacket, 0, sizeof(stAudioDataPacket));
				}
				
				if(audiolen + stAudioDataPacket.stFrameHeader.wAudioSize > sizeof(audiobuff) || QMAPI_FRAME_A !=stAudioDataPacket.stFrameHeader.byFrameType)
				{
					//err();
					//printf("##### audiolen:%u. wAudioSize:%u. byFrameType:%u. \n",audiolen,stAudioDataPacket.stFrameHeader.wAudioSize,stAudioDataPacket.stFrameHeader.byFrameType);
				}
				else
				{
                    memcpy(audiobuff  + audiolen, stAudioDataPacket.pData, stAudioDataPacket.stFrameHeader.wAudioSize);
					audiolen += stAudioDataPacket.stFrameHeader.wAudioSize;
					goto getaudio;
				}
			}
            if (STOP == pNode->status)
            {
                break;
            }
            for (loop_send=0; loop_send<1; loop_send++)
            {
                if (is_internet)
                {
                    to.tv_sec = 5;
                    to.tv_usec = 0;
                    FD_SET(pNode->hSendSock, &fset);
                    ret = select(pNode->hSendSock+1, NULL, &fset, NULL, &to);
                    if (ret == 0 )  //timeout
                    {
                        send_timeout_tick++;
                        if (send_timeout_tick > 10)
                        {
                            printf("send timeout too long, and we will close this session!\n");
                            goto out;
                        }
                        break;
                    }
                    if (ret < 0)
                    {
                        if (errno == EINTR)
                        {
                            err();
                            select_err_tick++;
                            if (select_err_tick > 10)
                            {
                                err();
                                select_err_tick = 0;
                                goto out;
                            }
                            break;
                        }
                        err();
                        goto out;
                    }
                    select_err_tick = 0;
                    send_timeout_tick = 0;
                }

				netDataPack.stPackHead.nFlag = TLNET_FLAG;
                netDataPack.stPackData.stFrameHeader.wMotionDetect = 0;
				netDataPack.stPackData.stFrameHeader.wFrameIndex   = stAVDataPacket.stFrameHeader.dwFrameIndex;
				netDataPack.stPackData.stFrameHeader.dwTimeTick    = stAVDataPacket.stFrameHeader.dwTimeTick;
				netDataPack.stPackData.stFrameHeader.dwVideoSize   = stAVDataPacket.stFrameHeader.dwVideoSize;
				netDataPack.stPackData.stFrameHeader.wAudioSize    = stAVDataPacket.stFrameHeader.wAudioSize + audiolen;
				netDataPack.stPackData.stFrameHeader.byKeyFrame    = (stAVDataPacket.stFrameHeader.byFrameType == 0) ? TRUE : FALSE;

				if(avbufSize > DATA_PACK_SIZE)
				{
					netDataPack.stPackData.wIsSampleHead = TRUE;
				}
				else
				{	
					netDataPack.stPackData.wIsSampleHead = (stAVDataPacket.dwPacketNO == 0)  ? TRUE : FALSE;
				}
				
				avbufSize = stAVDataPacket.dwPacketSize;
				pSendBuf = (char*)stAVDataPacket.pData;
				packNum = 0;
				packNum = (stAVDataPacket.dwPacketSize   )/ DATA_PACK_SIZE;
				if(((stAVDataPacket.dwPacketSize  )% DATA_PACK_SIZE) != 0)
				{
					packNum += 1;
				}
		
				for(j = 0; j < packNum -1 ; j++)
				{
                    netDataPack.stPackData.wBufSize = DATA_PACK_SIZE;
                    netDataPack.stPackHead.nSize    = sizeof(DATA_PACKET) + netDataPack.stPackData.wBufSize - sizeof(netDataPack.stPackData.byPackData);
                    //memcpy(netDataPack.stPackData.byPackData , pSendBuf + (DATA_PACK_SIZE * j) , netDataPack.stPackData.wBufSize);
                    netDataPack.stPackData.stFrameHeader.byPackIndex = j;
                    ret = TcpSendOnePacket(pNode->hSendSock, &netDataPack, sizeof(netDataPack) - sizeof(netDataPack.stPackData.byPackData));
                    if (ret > 0)
                    {
                        ret = TcpSendOnePacket(pNode->hSendSock, pSendBuf + (DATA_PACK_SIZE * j), netDataPack.stPackData.wBufSize  );
                        if (ret < 0)
                        {
                            err();
                        }
                    }
                    else
                    {
                        err();
                    }
                    netDataPack.stPackData.wIsSampleHead = FALSE;
				}
				//最后一个包
				ret = XSendLastPacket(pNode->hSendSock , &netDataPack , pSendBuf + (DATA_PACK_SIZE * j) , avbufSize - (packNum - 1)*DATA_PACK_SIZE , audiobuff , audiolen, j);
				netDataPack.stPackData.wIsSampleHead = FALSE;

				
                if (ret <= 0 && (errno == EPIPE || errno == EBADF))
                {
                    err();
                    goto out;
                }
                if (ret <= 0)
                {
                    snd_err_tick2++;
                    if (snd_err_tick2 > 5)
                    {
                        printf("send thread: snd_err_tick2 > 10 \n");
                        goto out;
                    }
                    err();
                    if (g_serinfo.bServerExit)
                    {
                        goto out;
                    }
                    if (STOP == pNode->status)
                    {
                        goto out;
                    }
                }
                else  /* if(ret <= 0) else */
                {
                    snd_err_tick2 = 0;
                }
            }
            usleep(10);
        }

out:
//      printf("pthread 0x%x (%s_%d)\n", pthread_self(), __FUNCTION__, __LINE__ );
        if (s32Reader != 0)
        {
            ret = QMapi_buf_del_reader(0 , s32Reader);
            if(ret)
            {
                err();
               // return;
            }
            s32Reader = 0;
        }
        pthread_mutex_lock(&g_serinfo.hTcpSendListMutex);
        pNode->status = QUIT;
        pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);
            
        pthread_mutex_lock(&g_serinfo.dataQuitMangMutex);
        g_serinfo.dataQuitThreadNum ++;
        g_serinfo.dataProcessThreadNum --;

        if (g_serinfo.dataProcessThreadNum < 0)
        {
            g_serinfo.dataProcessThreadNum = 0;
        }
        pthread_mutex_unlock(&g_serinfo.dataQuitMangMutex);

        pthread_cond_signal(&g_serinfo.dataQuitMangCond);
//      printf("pthread 0x%x (%s_%d)\n", pthread_self(), __FUNCTION__, __LINE__ );

    }
    g_serinfo.nDataThreadCount--;
}

void    TcpDataQuitThread(void *pPar)
{
    SEND_NODE *pFind = NULL;
    prctl(PR_SET_NAME, (unsigned long)"TcpDataQuit", 0,0,0);
    pthread_detach(pthread_self());

    while (!g_serinfo.bServerExit)
    {
        pthread_mutex_lock(&g_serinfo.dataQuitMangMutex);
        while (g_serinfo.dataQuitThreadNum == 0)
        {
            pthread_cond_wait(&g_serinfo.dataQuitMangCond, &g_serinfo.dataQuitMangMutex);
        }

        if (g_serinfo.bServerExit)
        {
            pthread_mutex_unlock(&g_serinfo.dataQuitMangMutex);
            break;
        }
       // while ((pFind = FindWaitProcessNode(STOP)) != NULL)
         while ((pFind = FindWaitProcessNode(QUIT)) != NULL)
        {
            FreeTcpSendNode(pFind);
        }

        g_serinfo.dataQuitThreadNum = 0;
        pthread_mutex_unlock(&g_serinfo.dataQuitMangMutex);
    }
}

void RequestTcpPlay(OPEN_HEAD openHead, int hDataSock)
{
    SEND_NODE *pNew = NULL;
                 
    pNew = (SEND_NODE *)malloc(sizeof(SEND_NODE));
    if (pNew != NULL) // [zhb][add][2006-04-21]
    {
        memset(pNew, 0, sizeof(SEND_NODE));
        
        pNew->status = READY;
        pNew->pNext = NULL;
        pNew->hSendSock = hDataSock;
        
        pNew->nType = PROTOCOL_TCP;
        pNew->nSerChannel = openHead.nSerChannel;
        pNew->nStreamType = openHead.nStreamType;
        pNew->nClientChannel = openHead.nClientChannel;
        pNew->nClientID = openHead.nID;
        pNew->ncork = 0;
      
        pNew->nClientIP = 0;        //udp set,not use in tcp
        pNew->nClientPort = 0;      //udp set,not use in tcp

        InsertPlayNode(g_serinfo.pTcpSendList, g_serinfo.hTcpSendListMutex, pNew);
    }
}

int RequestTcpPlayExt(OPEN_HEAD openHead, int hDataSock,  unsigned long clientIp)
{
    int ret = -1;
    SEND_NODE *pNew = NULL;
    
    if (hDataSock < 0)
    {
        return -1;
    }
             
    pNew = (SEND_NODE *)malloc(sizeof(SEND_NODE));
    if (pNew != NULL)
    {
        memset(pNew, 0, sizeof(SEND_NODE));
        
        pNew->status = READY;
        pNew->pNext = NULL;
        pNew->hSendSock = hDataSock;
        
        pNew->nType = PROTOCOL_TCP;
        pNew->nSerChannel = openHead.nSerChannel;
        pNew->nStreamType = openHead.nStreamType;
        pNew->nClientChannel = openHead.nClientChannel;
        pNew->nClientID = openHead.nID;
        pNew->nClientTcpIP = clientIp; 
        pNew->ncork = 0;
        pNew->nClientIP = 0;        //udp set,not use in tcp
        pNew->nClientPort = 0;      //udp set,not use in tcp

        ret = InsertPlayNodeExt(g_serinfo.pTcpSendList, g_serinfo.hTcpSendListMutex, pNew);
        if (ret < 0)
        {
            return -1;
        }
        
        return 0;
    }
    else
    {
        //shutdown(hDataSock, 2);
        //close(hDataSock);
        return -1;
    }
}

void InsertPlayNode(SEND_NODE *pNodeHead, pthread_mutex_t hMutex, SEND_NODE *pNew)
{
    if (NULL == pNodeHead)
    {
        return ;
    }
    if (NULL == pNew)
    {
        return ;
    }
   
    pthread_mutex_lock(&hMutex);
    
    while (pNodeHead->pNext)
    {
        pNodeHead = pNodeHead->pNext;
    }
        
    pNodeHead->pNext = pNew;
    
    pthread_mutex_unlock(&hMutex);
}

int InsertPlayNodeExt(SEND_NODE *pNodeHead, pthread_mutex_t hMutex, SEND_NODE *pNew)
{
    if (NULL == pNodeHead)
    {
        return -1;
    }
    if (NULL == pNew)
    {
        return -1;
    }
   
    pthread_mutex_lock(&hMutex);
    
    while (pNodeHead->pNext)
    {
        pNodeHead = pNodeHead->pNext;
    }
        
    pNodeHead->pNext = pNew;
    
    pthread_mutex_unlock(&hMutex);
    
    return 0;
}

SEND_NODE *WaitReadyProcessNode()
{
    SEND_NODE *pHead = g_serinfo.pTcpSendList->pNext;
    
    pthread_mutex_lock(&g_serinfo.hTcpSendListMutex);
    
    while (pHead)
    {
        if (READY == pHead->status)
        {
            pHead->status = RUN;
            pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);
            return pHead;
        }
        pHead = pHead->pNext;
    }
    pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);

    return NULL;
}


SEND_NODE *FindWaitProcessNode(NODE_STATUS status)
{
    SEND_NODE *pHead = g_serinfo.pTcpSendList->pNext;
    
    pthread_mutex_lock(&g_serinfo.hTcpSendListMutex);
    
    while (pHead)
    {
        if (status == pHead->status)
        {
            pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);
            return pHead;
        }
        pHead = pHead->pNext;
    }
    pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);

    return NULL;
}

// [ZHB][2005-11-22]
SEND_NODE *GetWaitProcessHeadNode()
{
    return g_serinfo.pTcpSendList->pNext;
}

void FreeTcpSendNode(SEND_NODE *pNode)
{
    SEND_NODE *pPre = NULL;
    SEND_NODE *pCur = NULL;
    
    pthread_mutex_lock(&g_serinfo.hTcpSendListMutex);

    pPre = g_serinfo.pTcpSendList;
    
    if (NULL == pPre)
    {
        pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex); 
        return;
    }
    
    pCur = pPre->pNext;
    
    while (pCur)
    {
        if (pCur == pNode)
        {
            shutdown(pNode->hSendSock, 2);
            close(pNode->hSendSock);
            pPre->pNext = pNode->pNext;
            free(pNode);
            pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);
          
            return ;
        }
        
        pPre = pCur;
        pCur = pCur->pNext;
    }
    
    pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);
}

int StopTcpNode(int id)
{
    int nNum = 0;
        SEND_NODE *pNode = NULL;
    
    pthread_mutex_lock(&g_serinfo.hTcpSendListMutex);
    
    pNode = g_serinfo.pTcpSendList->pNext;
    
    while (pNode)
    {
        if (pNode->nClientID == id)
        {
            if(pNode->status == RUN)
            pNode->status = STOP;
            else 
                pNode->status = QUIT;
            nNum ++;
        }
        
        pNode = pNode->pNext;
    }
    
    pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);

    return nNum;
}

int StopAllTcpNode()
{
    int         nNum = 0;
    SEND_NODE   *pNode = NULL;

    pthread_mutex_lock(&g_serinfo.hTcpSendListMutex);

    pNode = g_serinfo.pTcpSendList;

    //first node is unuseful
    if (pNode)
    {
        pNode = pNode->pNext;
    }
        
    while (pNode)
    {
    if(pNode->status == RUN)
        pNode->status = STOP;
    else 
        pNode->status = QUIT;       
        nNum ++;

        pNode = pNode->pNext;
    }
    
    pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);

    return nNum;
}

int FreeAllTcpNode()
{
    int         nFreeNum = 0;
    SEND_NODE   *pTmp = NULL;

    pthread_mutex_lock(&g_serinfo.hTcpSendListMutex);

    while (g_serinfo.pTcpSendList)
    {
        pTmp = g_serinfo.pTcpSendList;
        g_serinfo.pTcpSendList = pTmp->pNext;
        free(pTmp);
        nFreeNum ++;
    }

    pthread_mutex_unlock(&g_serinfo.hTcpSendListMutex);
    
    return nFreeNum;
}

//UDP
#if 0
void UdpSendThread(void *pPar)
{
    unsigned long       lPar = (unsigned long)pPar;
    int             channelNo;
    int                 streamNo;
    int                 opt;
    int                 ret = 0;
    //BOOL            bMultiSend = FALSE;
    BOOL            bFirstRead = TRUE;
    int             hSendSock = -1;
    QMAPI_MBUF_POS_POLICY_E enBufPolicy = 0;
    QMAPI_NET_DATA_PACKET stDataPacket;

	//int nSendNo = 0;
	//int nPacketDataSize = 0;
	int bufSize =0;
	int packNum = 0;
	char * pSendBuf = NULL;
	unsigned int j = 0;

    SEND_NODE       *pHeadNode = NULL;
    SEND_NODE       *pSendNode = NULL;

    pthread_mutex_t     hNodeMutex;
    
    NET_DATA_PACKET  netDataPack;

    struct sockaddr_in      addr;

    channelNo = LowWord(lPar);
    streamNo = HeightWord(lPar);
    prctl(PR_SET_NAME, (unsigned long)"UdpSend", 0,0,0);
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);

    bzero(&addr,sizeof(addr));
    bzero(&netDataPack,sizeof(NET_DATA_PACKET));

    addr.sin_family = AF_INET; 
    if (0 == streamNo)
    {
        pHeadNode = g_serinfo.udpStreamSend1.pUdpSendList[channelNo];
        hNodeMutex = g_serinfo.udpStreamSend1.hUdpSendListMutex[channelNo];
    }
    else
    {
        //pHeadNode = g_serinfo.udpStreamSend2.pUdpSendList[channelNo];
        //hNodeMutex = g_serinfo.udpStreamSend2.hUdpSendListMutex[channelNo];
    }
    
    hSendSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == hSendSock)
    {
        err();
        return ;
    }
    
    opt = SOCK_SNDRCV_LEN;
    setsockopt(hSendSock, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt));
    

	 int s32Reader = -1;
	
    while (!g_serinfo.bServerExit && g_tl_updateflash.bUpdatestatus == 0)
    {
        if(0 == GetUdpRunNode(channelNo,streamNo))  
        {
        	if(s32Reader > 0)
        	{
        		ret = QMapi_buf_del_reader( 0 , s32Reader);
				if(ret)
				{
					  err();
					  return;
				}
				s32Reader = -1;
        	}
            sleep(1);
            continue;
        }
        
		if(s32Reader < 0)
		{
	        ret = QMapi_buf_add_reader(0 , channelNo , streamNo, &s32Reader);
			if(ret)
			{
				  err();
				  return;
			}
		}

        if (g_serinfo.bServerExit)
        {
        	if(s32Reader > 0)
        	{
        		ret = QMapi_buf_del_reader( 0 , s32Reader);
				if(ret)
				{
					  err();
					  return;
				}
				s32Reader = -1;
        	}
        	bFirstRead = TRUE;
            break;
        }
        
        if(0 == GetUdpRunNode(channelNo,streamNo))
        {
            //usleep(100000); 
          	if(s32Reader > 0)
        	{
        		ret = QMapi_buf_del_reader( 0 , s32Reader);
				if(ret)
				{
					  err();
					  return;
				}
				s32Reader = -1;
        	}
            bFirstRead = TRUE;
            sleep(1);
            continue;
        }

        //bMultiSend = FALSE;
		if(bFirstRead)
		{
			enBufPolicy = QMAPI_MBUF_POS_LAST_WRITE_nIFRAME;
		}
		else
		{
			enBufPolicy = QMAPI_MBUF_POS_CUR_READ;
		}

		memset( &stDataPacket  , 0 , sizeof(stDataPacket));

		ret = QMapi_buf_get_readptrpos( 0, s32Reader , enBufPolicy , 0 , &stDataPacket);
		if(ret)
		{	
			bFirstRead = TRUE;
			err();
			continue;
		}

        bFirstRead = FALSE;

        //begin send
        pthread_mutex_lock(&hNodeMutex);
        pSendNode = pHeadNode->pNext;

        while (pSendNode)
        {
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = pSendNode->nClientIP;
            addr.sin_port = pSendNode->nClientPort;

            memset(&netDataPack, 0, sizeof(netDataPack));
			// 将数据打包发送出去
			//nSendNo = 0;
			//nPacketDataSize = 0;

			netDataPack.stPackHead.nFlag = TLNET_FLAG;

			netDataPack.stPackData.stFrameHeader.wFrameIndex = stDataPacket.stFrameHeader.dwFrameIndex;
			netDataPack.stPackData.stFrameHeader.dwTimeTick = stDataPacket.stFrameHeader.dwTimeTick;
			netDataPack.stPackData.stFrameHeader.dwVideoSize = stDataPacket.stFrameHeader.dwVideoSize;
			netDataPack.stPackData.stFrameHeader.wAudioSize = stDataPacket.stFrameHeader.wAudioSize;
			netDataPack.stPackData.stFrameHeader.byKeyFrame = (stDataPacket.stFrameHeader.byFrameType == 0) ? TRUE : FALSE;

			if(bufSize > DATA_PACK_SIZE)
			{
				netDataPack.stPackData.wIsSampleHead = TRUE;
			}
			else
			{	
				netDataPack.stPackData.wIsSampleHead = (stDataPacket.dwPacketNO == 0)  ? TRUE : FALSE;
			}
			
			bufSize = stDataPacket.dwPacketSize;
			pSendBuf = (char*)stDataPacket.pData;
			packNum = 0;
			packNum = stDataPacket.dwPacketSize / DATA_PACK_SIZE;
			if((stDataPacket.dwPacketSize % DATA_PACK_SIZE) != 0)
			{
				packNum += 1;
			}
			
			for(j = 0; j < packNum; j++)
			{
				if(j != packNum - 1)
				{
					netDataPack.stPackData.wBufSize = DATA_PACK_SIZE;
				}
				else
				{
					netDataPack.stPackData.wBufSize = bufSize - (packNum - 1)*DATA_PACK_SIZE;
				}
				netDataPack.stPackHead.nSize = sizeof(DATA_PACKET) + netDataPack.stPackData.wBufSize - DATA_PACK_SIZE;
				//send header
				ret = sendto(hSendSock, &netDataPack, sizeof(netDataPack) - DATA_PACK_SIZE,
	                            0, (struct sockaddr *)&addr, sizeof(addr));
			   ret = sendto(hSendSock, pSendBuf + (DATA_PACK_SIZE * j) , netDataPack.stPackData.wBufSize,
	                            0, (struct sockaddr *)&addr, sizeof(addr));
	                      
	            if (ret < 1)
	            {
	                err();
	            }
					
				netDataPack.stPackData.wIsSampleHead = FALSE;
				
			}

            pSendNode = pSendNode->pNext;
        }
        pthread_mutex_unlock(&hNodeMutex);
    }

    if(s32Reader > 0)
    {
		ret = QMapi_buf_del_reader( 0 , s32Reader);
		if(ret)
		{
			  err();
			  return;
		}
		s32Reader = -1;
	}
	
}

void RequestUdpPlay(OPEN_HEAD openHead, unsigned long ip, unsigned long port)
{
    int                 i = 0;
    SEND_NODE       *pHeadNode = NULL;
    SEND_NODE       *pSendNode = NULL;
    SEND_NODE       *pNew = NULL;

    pNew = (SEND_NODE *)malloc(sizeof(SEND_NODE));
    bzero(pNew,sizeof(SEND_NODE));

    pNew->status = RUN;
    pNew->pNext = NULL;
    pNew->hSendSock = 0;        //tcp use,not use in udp

    pNew->nType = PROTOCOL_UDP;
    pNew->nSerChannel = openHead.nSerChannel;
    pNew->nStreamType = openHead.nStreamType;
    pNew->nClientChannel = openHead.nClientChannel;
    pNew->nClientID = openHead.nID;

    pNew->nClientIP = ip;
    pNew->nClientPort = port;
    
    // [ZHB][ADD][2005-12-01]
    for (i=0; i<g_serinfo.nChannelNum; i++)
    {
        pHeadNode = g_serinfo.udpStreamSend1.pUdpSendList[i];
        pSendNode = pHeadNode->pNext;
       
        while (pSendNode)
        {
            if ((pSendNode->nClientIP == pNew->nClientIP) 
                && (pSendNode->nClientPort == pNew->nClientPort) 
                && (pSendNode->nType == pNew->nType) 
                && (pSendNode->nSerChannel == pNew->nSerChannel))
            {
                return ;
            }
            
            pSendNode = pSendNode->pNext;
        }
    }

    if (0 == openHead.nStreamType)
    {
        InsertPlayNode(g_serinfo.udpStreamSend1.pUdpSendList[openHead.nSerChannel],
                    g_serinfo.udpStreamSend1.hUdpSendListMutex[openHead.nSerChannel], pNew);
    }
    else
    {
        /*
        InsertPlayNode(g_serinfo.udpStreamSend2.pUdpSendList[openHead.nSerChannel],
                    g_serinfo.udpStreamSend2.hUdpSendListMutex[openHead.nSerChannel], pNew);
        */
    }
}

BOOL ExistUdpUser(OPEN_HEAD openHead, unsigned long ip, unsigned long port)
{
    SEND_NODE       *pNode = NULL;
    pthread_mutex_t hMutex;
    
    if (0 == openHead.nStreamType)
    {
        pNode = g_serinfo.udpStreamSend1.pUdpSendList[openHead.nSerChannel];
        hMutex = g_serinfo.udpStreamSend1.hUdpSendListMutex[openHead.nSerChannel];
    }
    else
    {
        // pNode = g_serinfo.udpStreamSend2.pUdpSendList[openHead.nSerChannel];
        // hMutex = g_serinfo.udpStreamSend2.hUdpSendListMutex[openHead.nSerChannel];
    }
    
    err();
    
    if (NULL == pNode)
    {
        return FALSE;
    }
        
    pthread_mutex_lock(&hMutex);
    pNode = pNode->pNext;
    
    while (pNode)
    {
        if ((pNode->nClientID == openHead.nID) && (pNode->nClientIP == ip) && (pNode->nClientPort == port))
        {
            pthread_mutex_unlock(&hMutex);
            err();
            return TRUE;
        }
    }
    
    pthread_mutex_unlock(&hMutex);

    err();
    
    return FALSE;
}

BOOL CloseUdpPlay(OPEN_HEAD closeHead)
{
    SEND_NODE       *pNode = NULL;
    SEND_NODE       *pPre = NULL;
    pthread_mutex_t hMutex;

    if (0 == closeHead.nStreamType)
    {
        pNode = g_serinfo.udpStreamSend1.pUdpSendList[closeHead.nSerChannel];
        hMutex = g_serinfo.udpStreamSend1.hUdpSendListMutex[closeHead.nSerChannel];
    }
    else
    {
        //pNode = g_serinfo.udpStreamSend2.pUdpSendList[closeHead.nSerChannel];
        //hMutex = g_serinfo.udpStreamSend2.hUdpSendListMutex[closeHead.nSerChannel];
    }

    if (NULL == pNode)
    {
        return FALSE;
    }

    pthread_mutex_lock(&hMutex);
    pPre = pNode;
    pNode = pNode->pNext;
    
    while (pNode)
    {
        if (pNode->nClientID==closeHead.nID 
            && pNode->nType==closeHead.nProtocolType
            && pNode->nClientChannel==closeHead.nClientChannel)
        {
            pPre->pNext = pNode->pNext;
            free(pNode);
            err();
            break;
        }
        pPre = pNode;
        pNode = pNode->pNext;
    }
    pthread_mutex_unlock(&hMutex);
    return TRUE;
}
#enid


BOOL ExistMultiUser(OPEN_HEAD openHead)
{
    SEND_NODE       *pNode = NULL;
    pthread_mutex_t     hMutex;

    if (0 == openHead.nStreamType)
    {
        pNode = g_serinfo.udpStreamSend1.pUdpSendList[openHead.nSerChannel];
        hMutex = g_serinfo.udpStreamSend1.hUdpSendListMutex[openHead.nSerChannel];
    }
    else
    {
        //pNode = g_serinfo.udpStreamSend2.pUdpSendList[openHead.nSerChannel];
        //hMutex = g_serinfo.udpStreamSend2.hUdpSendListMutex[openHead.nSerChannel];
    }

    if (NULL == pNode)
    {
        return FALSE;
    }

    pthread_mutex_lock(&hMutex);
    pNode = pNode->pNext;
    
    while (pNode)
    {
        if ((pNode->nClientID == openHead.nID) 
            && (pNode->nClientIP == htonl(g_serinfo.multiAddr)) 
            && (pNode->nClientPort == htonl(g_serinfo.nBasePort+openHead.nSerChannel)))
        {
            pthread_mutex_unlock(&hMutex);
            return TRUE;
        }
    }      
    pthread_mutex_unlock(&hMutex);
    
    return FALSE;
}

BOOL CloseMultiPlay(OPEN_HEAD closeHead)
{
    return CloseUdpPlay(closeHead);
}

void RequestMultiPlay(OPEN_HEAD openHead)
{
    SEND_NODE       *pNew = NULL;

    pNew = (SEND_NODE *)malloc(sizeof(SEND_NODE));
    bzero(pNew,sizeof(SEND_NODE));

    pNew->status = RUN;
    pNew->pNext = NULL;
    pNew->hSendSock = 0;//tcp use,not use in udp

    pNew->nType = PROTOCOL_MULTI;
    pNew->nSerChannel = openHead.nSerChannel;
    pNew->nStreamType = openHead.nStreamType;
    pNew->nClientChannel = openHead.nClientChannel;
    pNew->nClientID = openHead.nID;

    pNew->nClientIP = g_serinfo.multiAddr;
    pNew->nClientPort = htons(g_serinfo.nBasePort+openHead.nSerChannel);

    if (0 == openHead.nStreamType)
    {
        InsertPlayNode(g_serinfo.udpStreamSend1.pUdpSendList[openHead.nSerChannel], 
                    g_serinfo.udpStreamSend1.hUdpSendListMutex[openHead.nSerChannel], pNew);
    }
    else
    {
        /*
        InsertPlayNode(g_serinfo.udpStreamSend2.pUdpSendList[openHead.nSerChannel],
                    g_serinfo.udpStreamSend2.hUdpSendListMutex[openHead.nSerChannel], pNew);
        */
    }
}

int GetUdpRunNode(int nChannel,int nStream)
{
    int                 nCount = 0;
    SEND_NODE       *pPre = NULL;
    SEND_NODE       *pNode = NULL;
    
    if (0 == nStream)
    {
        pthread_mutex_lock(&g_serinfo.udpStreamSend1.hUdpSendListMutex[nChannel]);

        pNode = g_serinfo.udpStreamSend1.pUdpSendList[nChannel];
        pPre = pNode;
        pNode = pNode->pNext;
        
        while (pNode)
        {
            if (STOP == pNode->status)
            {
                pPre->pNext = pNode->pNext;
                free(pNode);
                pNode = pPre->pNext;
            }
            else
            {
                nCount++;
                pPre = pNode;
                pNode=pNode->pNext;
            }
        }
        pthread_mutex_unlock(&g_serinfo.udpStreamSend1.hUdpSendListMutex[nChannel]);
    }
    else
    {
        /*
        pthread_mutex_lock(&g_serinfo.udpStreamSend2.hUdpSendListMutex[nChannel]);
        pNode = g_serinfo.udpStreamSend2.pUdpSendList[nChannel];
        pPre = pNode;
        pNode = pNode->pNext;
        while(pNode)
        {
            if(STOP ==pNode->status)
            {
                pPre->pNext = pNode->pNext;
                free(pNode);
                pNode = pPre->pNext;
            }
            else
            {
                nCount++;
                pPre = pNode;
                pNode=pNode->pNext;
            }
        }
        pthread_mutex_unlock(&g_serinfo.udpStreamSend2.hUdpSendListMutex[nChannel]);
        */
    }

    return nCount;
}

void StopUdpNode(int id)
{
    int i = 0;
    SEND_NODE *pNode = NULL;
    
    for (i=0; i<g_serinfo.nChannelNum; i++)
    {
        pthread_mutex_lock(&g_serinfo.udpStreamSend1.hUdpSendListMutex[i]);

        if (g_serinfo.udpStreamSend1.pUdpSendList[i])
        {
            pNode = g_serinfo.udpStreamSend1.pUdpSendList[i]->pNext;
        }
        
        while (pNode)
        {
            if (pNode->nClientID == id)
            {
                pNode->status = STOP;
                printf("STOP  %s:%d \n",__FUNCTION__,__LINE__);
            }
            pNode = pNode->pNext;
        }
        
        pthread_mutex_unlock(&g_serinfo.udpStreamSend1.hUdpSendListMutex[i]);
        
        /*
        pthread_mutex_lock(&g_serinfo.udpStreamSend2.hUdpSendListMutex[i]);
        
        if (g_serinfo.udpStreamSend2.pUdpSendList[i])
        {
            pNode = g_serinfo.udpStreamSend2.pUdpSendList[i]->pNext;
        }
        
        while (pNode)
        {
            if (pNode->nClientID == id)
            {
                pNode->status = STOP;
            }
            pNode = pNode->pNext;
        }
        pthread_mutex_unlock(&g_serinfo.udpStreamSend2.hUdpSendListMutex[i]);
        */
    }
}

void FreeAllUdpNode()
{
    int                 i = 0;
    SEND_NODE       *pTmp = NULL;
    SEND_NODE       *pNode = NULL;

    for (i=0; i<g_serinfo.nChannelNum; i++)
    {
        pthread_mutex_lock(&g_serinfo.udpStreamSend1.hUdpSendListMutex[i]);
        
        if (g_serinfo.udpStreamSend1.pUdpSendList[i])
        {
            pNode = g_serinfo.udpStreamSend1.pUdpSendList[i];
        }
        
        while (pNode)
        {
            pTmp = pNode->pNext;
            free(pNode);
            pNode = pTmp;
        }
        
        pthread_mutex_unlock(&g_serinfo.udpStreamSend1.hUdpSendListMutex[i]);

        /*
        pthread_mutex_lock(&g_serinfo.udpStreamSend2.hUdpSendListMutex[i]);

        if (g_serinfo.udpStreamSend2.pUdpSendList[i])
        {
            pNode = g_serinfo.udpStreamSend2.pUdpSendList[i];
        }
        
        while (pNode)
        {
            pTmp = pNode->pNext;
            free(pNode);
            pNode = pTmp;
        }
        
        pthread_mutex_unlock(&g_serinfo.udpStreamSend2.hUdpSendListMutex[i]);
        */
    }
}
#endif

