/******************************************************************************

******************************************************************************/
#include "TLNetCommon.h"
#include "tlnetinterface.h"
#include "tlnetin.h"
#include "Q3Audio.h"

void  tltalk_SetCallback(ClientRequestTalk  fun1,ClientStreamTalk fun2)
{
	g_serinfo.pCallbackRequestTalk = fun1;
	g_serinfo.pCallbackStreamTalk = fun2;
}

//func  :
void    tltalk_SetTalkParam(int nChannel,int nBits,int nSamples)
{
	g_serinfo.nAudioChannels = nChannel;
	g_serinfo.nAudioBits = nBits;
	g_serinfo.nAudioSamples = nSamples;
}

void tltalk_End()
{
	if(g_serinfo.hTalkbackSocket)
	{
		close(g_serinfo.hTalkbackSocket);
		g_serinfo.hTalkbackSocket = 0;
	} 
}

//func  :
BOOL    tltalk_Begin(char *pszIP,unsigned short port)
{
	int                 ret;
	struct sockaddr_in  addr;
	
	bzero(&addr,sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(pszIP);
	addr.sin_port = htons(port);
	if(g_serinfo.hTalkbackSocket)
	{
		err();
		tltalk_End();
		return FALSE;
	}
	
	g_serinfo.hTalkbackSocket = socket(AF_INET,SOCK_STREAM,0);
	
	ret = connect(g_serinfo.hTalkbackSocket,(struct sockaddr*)&addr,sizeof(addr));
	if(-1 == ret)
	{
		err();
		tltalk_End(); 
		return FALSE;    
	}
	
	return TRUE;
}

//func  :
int     tltalk_Send(char *pBuf,int nLen)
{
	if(0 == g_serinfo.hTalkbackSocket)
	{
		printf("g_serinfo.hTalkbackSocket=0    error\n");
		return 0; 
	}    
	return send(g_serinfo.hTalkbackSocket,pBuf,nLen,0);
}

//func  :
int     tltalk_Send1(char *pBuf,int nLen)
{   
	int ret = send(g_serinfo.hTalkbackSocket, pBuf, nLen, 0);
	if( ret != nLen )
	{
		err();
		//stopAudiotalk();
	}
//	printf("............... tltalk_Send1 hTalkbackSocket:%d len:%d ret:%d\n", g_serinfo.hTalkbackSocket, nLen, ret);

	return ret;
}

//func  :
void TalkbackThread(void *par)
{
	TALKTHRD_PAR    thdpar;
	fd_set  fset;
	int     ret;
	struct timeval  to;
	char    acktest = '0';
	char    pBuf[NETCMD_TALKBACK_SIZE];
	int		recv_pack_timeout = 0;
	#ifdef TL_HI3518_PLATFORM
	int frameindex = 0;
	int offset, section1, section2;
	int frame_num;
	char tmpbuf[NETCMD_TALKBACK_SIZE]={0};
	#endif
	prctl(PR_SET_NAME, (unsigned long)"Talkback", 0,0,0);
	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    
Q3_Audio_Info info =  {
        .wFormatTag = QMAPI_PT_G711A,
        .channel = 1,
        .bitwidth = 16,
        .sample_rate = 8000,
        .volume = 256,
    };

    int audioHandle = QMAPI_Audio_Decoder_Init(&info);

    if (!audioHandle) {
        printf("%s audio handle init failed\n", __FUNCTION__);
        return;
    }

	memcpy(&thdpar, par, sizeof(thdpar));
	FD_ZERO(&fset);
	bzero(&to,sizeof(to));

	while(!g_serinfo.bServerExit)
	{
		to.tv_sec = 20;
		to.tv_usec = 0;
		FD_SET(thdpar.hSock, &fset);

		ret = select(thdpar.hSock+1,&fset,NULL,NULL,&to);
		if(g_serinfo.bServerExit)
		{
			err();
			printf("TalkbackThread: audio talk socket is rushed0!\n");
			//stopAudiotalk();
			break;
		}

		if(0 == ret)
		{
			ret = send(thdpar.hSock, &acktest, 1, 0);
			if(ret<=0)
			{
				printf("TalkbackThread: audio talk socket is rushed1!\n");
				//stopAudiotalk();
				break;
			}

			recv_pack_timeout++;
			if (recv_pack_timeout > 10)
			{
				printf("TalkbackThread: audio talk socket is rushed1:2!\n");
				//stopAudiotalk();
				break;
			}
			continue;
		}
		if(-1 == ret)
		{
			err();
			printf("TalkbackThread: audio talk socket is rushed2!\n");
			//stopAudiotalk();
			break;
		}
		if(!FD_ISSET(thdpar.hSock,&fset))
			continue;
		ret = recv(thdpar.hSock, pBuf, NETCMD_TALKBACK_SIZE, 0); //NETCMD_TALKBACK_SIZE
		if(ret < 0)
		{
			if(ECONNRESET == errno)
			{
				printf("TalkbackThread: audio talk socket is rushed3!\n");
				//stopAudiotalk();
				break;
			}
			continue;
		}
		else if(ret == 0) 
		{
			printf("TalkbackThread: audio talk socket is rushed4!\n");
			//stopAudiotalk();
			break;
		}
		QMAPI_Audio_Decoder_Data(audioHandle, pBuf, ret);
		memset(pBuf, 0, NETCMD_TALKBACK_SIZE);
    }

    if (audioHandle) {
        QMAPI_Audio_Decoder_UnInit(audioHandle);
        audioHandle = 0;
    }
}




