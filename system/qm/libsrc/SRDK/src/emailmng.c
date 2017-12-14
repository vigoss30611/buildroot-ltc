#include "tl_common.h"
#include "smtpc.h"
#include "email.h"

#define MAX_BUFFER_SIZE		256
#define EMAIL_SERVICE_PORT	25

/*BOOL rcvResponse(int _socket,int nresponse)
{
    int recv_bytes = 0;
	int nReturn;
    char response_buffer[MAX_BUFFER_SIZE];

	memset(response_buffer, 0, MAX_BUFFER_SIZE);

    if ( (recv_bytes = recv(_socket, response_buffer, MAX_BUFFER_SIZE, 0)) < 0 )
    {
		return FALSE;
    }
	response_buffer[recv_bytes] = 0;
	//printf("SendMail:%s",response_buffer);
	nReturn = atoi(response_buffer);
	if(nReturn != nresponse)
	{
		printf("SendMail:rcvResponse nresponse %d,nReturn %d\n",nresponse,nReturn);
		return FALSE;
	}
	return TRUE;
}*/


//const char _base64_encode_chars[] = 
//    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*char * mailencode(char *in_str,int len,int *noutlen)
{
    unsigned char c1, c2, c3;
    int i = 0;
	int n = 0;
	char *lpoutstr = malloc(len * 4 / 3 + 8);
	memset(lpoutstr, 0, len * 4 / 3 + 8);

    while ( i<len )
    {
        // read the first byte
        c1 = in_str[i++];
        if ( i==len )       // pad with "="
        {
            lpoutstr[n++] = _base64_encode_chars[ c1>>2 ];
            lpoutstr[n++] = _base64_encode_chars[ (c1&0x3)<<4 ];
            lpoutstr[n++] = '=';
			lpoutstr[n++] = '=';
            break;
        }

        c2 = in_str[i++];
        if ( i==len )       // pad with "="
        {
            lpoutstr[n++] = _base64_encode_chars[ c1>>2 ];
            lpoutstr[n++] = _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
            lpoutstr[n++] = _base64_encode_chars[ (c2&0xF)<<2 ];
            lpoutstr[n++] = '=';
            break;
        }

        // read the third byte
        c3 = in_str[i++];
        // convert into four bytes string
        lpoutstr[n++] = _base64_encode_chars[ c1>>2 ];
        lpoutstr[n++] = _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
        lpoutstr[n++] = _base64_encode_chars[ ((c2&0xF)<<2) | ((c3&0xC0)>>6) ];
        lpoutstr[n++] = _base64_encode_chars[ c3&0x3F ];
    }
	lpoutstr[n++] = 0;
	if(noutlen)
		*noutlen = n - 1;
    return lpoutstr;
}*/

/************************************************************************/
/* EMAIL Function                                                       */
/************************************************************************/

#define BOUNDARY			"***Wangjidh***"
#define SEND_TEXT_HEADER	"Content-Type:text/plain;Charset=gb2312\r\nContent-Transfer-Encoding:8bit\r\n\r\n"
#define MAIL_HEADER1		"X-Mailer: JiaboVideo Shenzhen Write the Software(winnt@qq.com).\r\n"
#define MAIL_HEADER2		"X-Priority: 3\r\nMIME-Version: 1.0\r\nContent-type: multipart/mixed; "
#define MAIL_HEADER3		"boundary=\"***Wangjidh***\"\r\n\r\n"

#define MAIL_HEADER4		"Content-Type:application/octet-stream;name"
#define MAIL_HEADER5		"Content-Disposition:attachment; filename"
#define MAIL_HEADER6		"Content-Transfer-Encoding:base64"

#if 0
#define TLERR_EMAIL_INVALID_SERVER_ADDRESS			0x33000001
#define TLERR_EMAIL_CONNECT_FAILED				0x33000002
#define TLERR_EMAIL_SEND_LOGIN					0x33000003
#define TLERR_EMAIL_SEND_USERNAME				0x33000004
#define TLERR_EMAIL_SEND_PASSWORD				0x33000005
#define TLERR_EMAIL_GET_HOSTNAME					0x33000006
#define TLERR_EMAIL_SERVER_REJECT				0x33000007
#endif

sem_t g_NVMailSem;
pthread_mutex_t pSendEmailMutex;

//int		nEmailBufCount = 0;

typedef struct tagSEND_EMAIL_LIST
{
	DWORD		dwCommand;
	int		nChannel;
	char *		csAttachBuf;
	int		nSize;
	BOOL		bUsed;
}SEND_EMAIL_LIST;

SEND_EMAIL_LIST sendemaillist[10];

static int GetAlarmType(DWORD dwCommand, char *alarmType)
{
	switch (dwCommand)
	{
	case TL_MSG_JPEGSNAP:
		strcpy(alarmType, "User Snapshot");
		break;
	case TL_MSG_VIDEOLOST:
		strcpy(alarmType, "Video Lost Alarm");
		break;
	case TL_MSG_MOVEDETECT: 
		strcpy(alarmType, "Video Move Detect");
		break;
	case TL_MSG_SENSOR_ALARM:
		strcpy(alarmType, "Sensor Alarm");
		break;
	case TL_MSG_VIDEORESUME:
		strcpy(alarmType, "Video Move resume");
		break;	
	case TL_MSG_ITEV_ALART:
		strcpy(alarmType, "Video ITEV Detect");
		break;	
	case TL_MSG_ITEV_RESUME:
		strcpy(alarmType, "Video ITEV resume");
		break;	

	case TL_MSG_ITEV_TRIPWIRE_ALART:
		strcpy(alarmType, "Video ITEV TRIPWIRE Detect");
		break;	
	case TL_MSG_ITEV_TRIPWIRE_RESUME:
		strcpy(alarmType, "Video ITEV TRIPWIRE resume");
		break;

	case TL_MSG_ITEV_ANOMALVIDEO_ALART:
		strcpy(alarmType, "Video ITEV VIDEOFOOL Detect");
		break;	
	case TL_MSG_ITEV_ANOMALVIDEO_RESUME:
		strcpy(alarmType, "Video ITEV VIDEOFOOL resume");
		break;
	
	default:
		strcpy(alarmType, "Unknown error");
		break;
	}
	return 0;
}

int SendEmail(DWORD dwCommand, int channelId, char *attachBuf, int size)
{
	int		i;
	if(g_tl_globel_param.TL_Email_Param.bEnableEmail == FALSE)
		return -1;

	if (g_tl_globel_param.TL_Email_Param.stToAddrList[0].csAddress[0] == 0)
		return -1;

	pthread_mutex_lock(&pSendEmailMutex);
	for(i = 0; i < 10; i ++)
	{
		if(sendemaillist[i].bUsed == FALSE)
		{
			//printf("Insert Message to SendEmail List\n");
			if(attachBuf && size && g_tl_globel_param.TL_Email_Param.byAttachPicture)
			{
				sendemaillist[i].csAttachBuf = malloc(size + 10);
				if(sendemaillist[i].csAttachBuf == 0)
				{
					printf("malloc memory for email send failed.\n");
					pthread_mutex_unlock(&pSendEmailMutex);
					return -1;
				}
				memcpy(sendemaillist[i].csAttachBuf, attachBuf, size);
				sendemaillist[i].nSize = size;
			}
			else
			{
				sendemaillist[i].csAttachBuf = 0;
				sendemaillist[i].nSize = 0;
			}
			sendemaillist[i].dwCommand = dwCommand;
			sendemaillist[i].nChannel = channelId;
			sendemaillist[i].bUsed = TRUE;
			pthread_mutex_unlock(&pSendEmailMutex);
			sem_post(&g_NVMailSem);
			return 0;
		}
	}
	pthread_mutex_unlock(&pSendEmailMutex);
	return -1;
}

void TL_MailSend_Thread()
{
	char	subject[256];
	char	body[256];
	int		ret = -1;
	int		i;
	char	fileName[128];
	char	strAlarmType[128];
	QMAPI_NET_EMAIL_PARAM *lpEmail;
	prctl(PR_SET_NAME, (unsigned long)"MailSend", 0,0,0);
	SEND_EMAIL_LIST sendemailitem;
	memset(sendemaillist,0,sizeof(sendemaillist));
	sem_init(&g_NVMailSem, 0, 0);
	pthread_mutex_init(&pSendEmailMutex , NULL);
	sendemailitem.csAttachBuf = malloc(1024*1024);
	if (sendemailitem.csAttachBuf == NULL)
	{
		printf("alloc for sendemailitem.csAttachBuf error\n");
		return ;
	}

	while(1)
	{
		// need fixme yi.zhang
		//if (g_tl_updateflash.bUpdatestatus)
		//	return ;

		sem_wait(&g_NVMailSem);

		sendemailitem.bUsed = FALSE;
		pthread_mutex_lock(&pSendEmailMutex);
		for(i = 0; i < 10; i ++)
		{
			if(sendemaillist[i].bUsed)
			{
				sendemailitem.nSize = sendemaillist[i].nSize;
				sendemailitem.nChannel = sendemaillist[i].nChannel;
				sendemailitem.bUsed = sendemaillist[i].bUsed;
				sendemailitem.dwCommand = sendemaillist[i].dwCommand;
				if (sendemaillist[i].csAttachBuf)
				{
					memcpy(sendemailitem.csAttachBuf, sendemaillist[i].csAttachBuf, sendemaillist[i].nSize);
					if (sendemaillist[i].csAttachBuf)
					{
						free(sendemaillist[i].csAttachBuf);
						sendemaillist[i].csAttachBuf = NULL;
					}
				}
				sendemaillist[i].bUsed = FALSE;
				break;//
			}
		}
		pthread_mutex_unlock(&pSendEmailMutex);

		if(sendemailitem.bUsed != TRUE)
			continue;
		
		GetAlarmType(sendemailitem.dwCommand, strAlarmType);

		memset(subject, 0, sizeof(subject));
		memset(body, 0, sizeof(body));
		sprintf(subject, "Alarm Type: %s(%s)", strAlarmType, g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4);
		if (TL_MSG_SENSOR_ALARM != sendemailitem.dwCommand)
		{
			sprintf(body, "Server IpAddr: %s;\nServer Name: %s;\nServer Channel Count: %d;\nAlarm Type: %s;\nAlarm ChannelId: %d;\n",
			g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4 ,g_tl_globel_param.stDevicInfo.csDeviceName,
			g_tl_globel_param.TL_Server_Info.wChannelNum, strAlarmType,	sendemailitem.nChannel);
		}
		else
		{
			sprintf(body, "Server IpAddr: %s;\nServer Name: %s;\nServer Channel Count: %d;\nAlarm Type: %s;\nAlarm ProbeId: %d;\n",
			g_tl_globel_param.stNetworkCfg.stEtherNet[0].strIPAddr.csIpV4,g_tl_globel_param.stDevicInfo.csDeviceName,
			g_tl_globel_param.TL_Server_Info.wChannelNum, strAlarmType,
			sendemailitem.nChannel);
		}
		if(sendemailitem.csAttachBuf != NULL && sendemailitem.nSize !=0)
		{
			// need fixme yi.zhang
			//sprintf(fileName, "%04d-%02d-%02d_%02d:%02d:%02d_ch%d.jpg", g_sysYear,g_sysMonth,g_sysDay,g_sysHour,g_sysMinute,g_sysSecond,sendemailitem.nChannel);
		}
		lpEmail = &g_tl_globel_param.TL_Email_Param;
		ret = Hisi_SendEmail(lpEmail, sendemailitem.csAttachBuf,sendemailitem.nSize,fileName,subject,body);

		if (ret)
			printf("SendMail Return %X\n",ret);

#if 0
		if(ret)
		{
			TL_addAlarm(ret, 0);
		}

		goto startsendmail;
#endif
	}
}

