
/*
*
*
*
*
*
*
*
*
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <asm/ioctl.h> 
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/prctl.h>
#include <assert.h>

#include "PPCS_Error.h"
#include "PPCS_Type.h"
#include "PPCS_API.h"

#include "libtypes.h"
#include "nsys_cli.h"
#include "nsys_module.h"
#include "vguard_did_authz.h"

#include "QMAPIType.h"
#include "QMAPIDataType.h"
#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPIErrno.h"
#include "QMAPI.h"

#include "QMAPIRecord.h"
#include "QMAPIAV.h"
#include "Q3Audio.h"

#include "sys_fun_interface.h" /* RM#2291: used sys_fun_gethostbyname, henry.li    2017/03/01*/


#define P2P_OK 0
#define P2P_ERR -1

#define VGMAXBUFFLEN 512

#define PPCS_P2P_PARAM_FILE	"/config/ppcs_p2p_conf.txt"

#define MAX_XADDRS_LEN	128
#define MAX_SCOPES_LEN	512
#define MAX_REF_LEN		128

#define CONN_RECV_BUFSZ		128 /* 命令套接字接收缓冲大小 */

#define GEN_SEC_COUNT(x) (x*1000)

#define DEBUG(fmt, args...)	printf("%s..%d:"fmt, __FUNCTION__, __LINE__, ##args)

#define NPC_D_PVM_DP_UMSP_FUNCID_P1_VER_CONSULT_GET_TOKEN			0x0101	
#define NPC_D_PVM_DP_UMSP_PACKET_TYPE_REQUEST						0				
#define NPC_D_PVM_DP_UMSP_PACKET_TYPE_RESPONSION					1

#define MAX_QDATA_PACKET_SZ		32768 /* 数据包大小，必须小于64K */
#define MAX_QDATA_PACKET_NUM	64 /* 数据包数量 */
#define MAX_QDATA_FRAME_TS		240 /* 数据帧时间戳，考虑音频的均衡性，该数值不应太大 */

#define PACK_INDEX_INIT -1 /*数据包索引初始化*/
#define PADDING_BUFSZ 1024

/* 数据主类型 */
#define TYPE_META_DATA			0 /* 元数据,非音视频数据 */
#define TYPE_VIDEO_RAW			1 /* 原始图像 */
#define TYPE_VIDEO_H264			2 /* H.264视频 */
#define TYPE_VIDEO_MPEG4		3 /* MPEG4视频 */
#define TYPE_VIDEO_MJPEG		4 /* MJPEG视频 */
#define TYPE_AUDIO_RAW			8 /* 原始音频 */
#define TYPE_AUDIO_G711			9 /* G711音频 */
#define TYPE_AUDIO_ADPCM		10 /* ADPCM音频 */
#define TYPE_AUDIO_AMR			11 /* AMR音频 */
#define TYPE_AUDIO_VORBIS		12 /* VORBIS音频 */
#define TYPE_AUDIO_AAC			13 /* AAC音频 */
#define TYPE_AUDIO_MP3			14 /* MP3音频 */

/* 数据子类型 */

#define SUB_TYPE_FINISH			0 /* 数据结束 */

/* TYPE_VIDEO_RAW的子类型 */
#define RAW_TYPE_YV12			1 /* YUV12格式 */
#define RAW_TYPE_I420			2 /* YUV420格式,与YV12的区别是UV顺序不同 */
#define RAW_TYPE_YUV422			3 /* YUV422格式 */
#define RAW_TYPE_YUV444			4 /* YUV444格式 */

/* TYPE_VIDEO_H264的子类型,SPS/PPS应当作为I帧的一部分处理 */
#define H264_TYPE_IFRAME		1 /* I帧 */
#define H264_TYPE_BFRAME		2 /* B帧 */
#define H264_TYPE_PFRAME		3 /* P帧 */

/* TYPE_VIDEO_MPEG4的子类型 */
#define MPEG4_TYPE_IFRAME		1 /* I帧 */
#define MPEG4_TYPE_BFRAME		2 /* B帧 */
#define MPEG4_TYPE_PFRAME		3 /* P帧 */

/* TYPE_AUDIO_RAW的子类型 */
#define RAW_SUBTYPE_PCM			1 /* pcm数据 */

/* TYPE_AUDIO_G711的子类型 */
#define G711_SUBTYPE_A			1 /* a律 */
#define G711_SUBTYPE_U			2 /* μ律 */

/* TYPE_AUDIO_ADPCM的子类型 */
#define ADPCM_SUBTYPE_IMA		1 /* IMA ADPCM */
#define ADPCM_SUBTYPE_DVI4		2 /* DVI4 ADPCM */
#define ADPCM_SUBTYPE_G726		3 /* g726 ADPCM */

/* TYPE_AUDIO_AAC的子类型 */
#define AAC_SUBTYPE_LC			1 /* LC AAC */
#define AAC_SUBTYPE_HE			2 /* HE AAC */

/* 数据类型操作定义 */
#define GEN_DATA_TYPE(m, s)		(unsigned char)( (( (m) << 4) & 0xf0) | ( (s) & 0xf) ) /* 数据类型 */
#define DATA_MAIN_TYPE(x)		( ( (x) >> 4) & 0xf ) /* 主类型 */
#define DATA_SUB_TYPE(x)		( (x) & 0xf ) /* 子类型 */

typedef struct{
	unsigned long   dwPacketFlag;	//0xFFFFEEEE				
	unsigned long   dwPacketSize;	//Size			
	unsigned short  usMsgFuncId;	//0x0201:Open, 0x0202:Close			
	unsigned char   ucPacketType:2;	//0			
	unsigned char   ucEndFlag:2;	
	unsigned char   ucReserve1:4;	
	unsigned char   ucResult;		//0
	unsigned long   dwTransId;		//0
	unsigned long   dwReserve2;
	
} NPC_S_PVM_DP_UMSP_MSG_HEAD, *PNPC_S_PVM_DP_UMSP_MSG_HEAD;


typedef struct{
	unsigned short   i_usClientVerList[12];			
	unsigned short    i_usClientVerNum;				
	
	unsigned short    o_usConsultVer;					
	char              o_sLoginToken[64];				
	
} NPC_S_PVM_DP_UMSP_MSG_BODY_P1_VER_CONSULT_GET_TOKEN, *PNPC_S_PVM_DP_UMSP_MSG_BODY_P1_VER_CONSULT_GET_TOKEN;

typedef struct {
	int  p2penable;
	char initstring[256]; /*InitString*/
	char crckey[32]; /*CRCKey*/
	char p2pconfdid[32]; /*DID*/
	char p2pconfapil[32]; /*APILicense*/
	char authzip[16]; /*授权服务器IP*/
	char authzhost[128]; /*授权服务器域名*/
}ppcs_p2p_info;

enum {
	CONN_STATE_ACTIVE = LIB_BIT00, /* 活动连接 */
	CONN_STATE_LOGIN  = LIB_BIT01, /* 已登录的连接 */
	CONN_DATA_CONNECT = LIB_BIT02 /* 数据连接建立 */
};

enum {
	RECV_STATUS_NEED_CMD = 0,
	RECV_STATUS_NEED_FULL_CMD
};

enum{
	CONT_START = 1,
	CONT_STOP,
	CONT_PAUSE,
	CONT_SEEK,
	CONT_AUDIO_START,
	CONT_AUDIO_STOP,
	CONT_REC_GETFRAME
};

typedef struct{
	unsigned short connid;
	unsigned short dataid;
	unsigned short control;
	unsigned short res;
	comm_moment_t comm;
}data_handle_control;

typedef struct {
	comm_data_header_t hdr; /* 数据头 */
	unsigned short size;    /*数据包长度*/
	unsigned short send_len; /*数据包已经发送的长度，包括数据头*/
	unsigned char  *pdata;   /*数据包起始地址*/
}data_send_packet;

typedef struct _ppcs_p2p_data{
	int p2p_handle;             /* P2P 连接句柄*/
	
	unsigned short connid;       /* 连接ID */
	unsigned short dataid;       /* 数据ID */

	unsigned char  index;        /* 当前通道索引号 */
	unsigned char  fps;          /* 帧率 */
	unsigned char  audioch;      /* 音频声道数 */
	unsigned char  audioKBps;    /* 音频码率 */
	unsigned short  audiohz;     /* 音频采样频率 */
	unsigned char  send_key:1;  /* 数据发送关键帧 */
	unsigned char  send_padding:1; /* 数据需要填充 */
	unsigned char  send_audio:1;   /* 发送音频*/
	unsigned char  send_pause:1;    /* 暂停 (只限于本地存储录像) */
	unsigned char  send_count:4;   /* 发送帧数量 (只限于本地存储录像)*/
	unsigned char  res1;

	int StreamReader;          /* 数据读取句柄 */

	int FilesInfoHandle;		/* 回放数据检索句柄 */
	time_t file_start_time;  /* 记录录像开发播放时间，用于拖动*/
	
	unsigned short packetsn; /* 数据包序号 */
	short packetindex; /* 当前数据包索引 */
	data_send_packet packet[MAX_QDATA_PACKET_NUM]; /* 数据包信息 */
	
	struct _ppcs_p2p_data *pnext;
}ppcs_p2p_send;

/* 数据接收信息 */
typedef struct _data_receive_t {
	int fd; /* socket句柄 */
	int audioHandle;
	unsigned short connid;       /* 连接ID */
	unsigned short dataid;       /* 数据ID */
	int conntype;
	unsigned short recvstate; /* 接收状态 */
	unsigned short lastrecvsn; /* 接收序号 */
	size_t datahdrlen; /* 数据头长度 */
	size_t exthdrlen; /* 数据扩展头长度 */
	size_t datalen; /* 接收数据长度 */
	size_t extdatalen; /* 接收扩展数据长度 */
	size_t recvbuflen; /* 接收缓冲区长度 */
	unsigned char *precvbuf; /* 数据接收缓冲区 */
	comm_data_header_t datahdr; /* 数据头 */
	comm_extension_header_t exthdr; /* 数据扩展头 */
	struct _data_receive_t *pnext; /* 下一连接 */
} ppcs_p2p_recv;

typedef struct _ppcs_p2p_connect{
	int p2p_handle;             /* P2P 连接句柄*/
	int countdown;              /* 活动计数 */

	unsigned short connid;       /* 连接ID */
	unsigned char verifytimes;  /* 用户验证次数 */
	unsigned char res;

	unsigned short connstate;   /* 连接状态 */
	unsigned short recv_status; /* 数据接收状态，准备接收、接收命令中、接收负载数据*/

	cmd_header_t cmd;            /* 命令头 */
	int recv_len;               /* 当前接收到的数据长度 */
	char recv_buffer[CONN_RECV_BUFSZ];

	int files_info_handle;	/* 文件查询句柄 */
	
	data_send_packet sendpacket; /* 当退出数据发送时，需要记录当前数据包发送情况，目的是为了保证一个数据包发送完整 */
	
	struct _ppcs_p2p_connect *pnext;
}ppcs_p2p_connect;



static unsigned char ppcs_p2p_run;
static unsigned char ppcs_p2p_pthread_count;

static ppcs_p2p_send *ppcs_p2p_send_head; /* 数据发送 */
static ppcs_p2p_send *ppcs_p2p_send_add;
static ppcs_p2p_send *ppcs_p2p_send_del;

static ppcs_p2p_recv *ppcs_p2p_recv_head; /* 数据接收 */
static ppcs_p2p_recv *ppcs_p2p_recv_add;
static ppcs_p2p_recv *ppcs_p2p_recv_del;


static ppcs_p2p_connect *ppcs_p2p_connect_head;
static ppcs_p2p_connect *ppcs_p2p_connect_add;

static data_handle_control handle_control;
/*
	获取 P2P 信息
*/
	
static bool get_value_string(FILE *pFile, char *pKeyStr, char *pValueStr, int strLen)
{
	char readLineStr[VGMAXBUFFLEN];
	int readLineLen, tmp;

	if (pFile == NULL || pKeyStr == NULL || pValueStr == NULL)
	{
		return FALSE;
	}
	memset(pValueStr, 0, strLen);
	fseek(pFile, SEEK_SET, 0);
	while (feof(pFile) == 0)
	{
		memset(readLineStr, 0, VGMAXBUFFLEN);
		if (fgets(readLineStr, VGMAXBUFFLEN, pFile) != NULL)
		{
			readLineLen = strlen(readLineStr);
			if (readLineStr[readLineLen - 1] == '\n')
			{
				readLineStr[readLineLen - 1] = '\0';
				readLineLen--;
			}
			for (tmp = 0; tmp < readLineLen; tmp++)
			{
				if (readLineStr[tmp] == '=')
				{
					break;
				}
			}
			if (tmp < readLineLen && memcmp(readLineStr, pKeyStr, tmp) == 0)
			{
				//拷贝长度不足
				if (readLineLen - tmp > strLen)
				{
					break;
				}
				strcpy(pValueStr, &readLineStr[tmp + 1]);
				return TRUE;
			}
		}
	}
	return FALSE;
}


static bool update_p2p_info(ppcs_p2p_info *pParam)
{
	FILE *pVgConfigFile;
	char writeBuff[VGMAXBUFFLEN];
	
	if (pParam == NULL)
	{
		printf("vg_update_did_file param input error\n");
		return FALSE;
	}

	pVgConfigFile = fopen(PPCS_P2P_PARAM_FILE, "w+");
	if (pVgConfigFile == NULL)
	{
		printf("vguard_did_authz file open error\n");
		return FALSE;
	}
	memset(writeBuff, 0, VGMAXBUFFLEN);
	sprintf(writeBuff, "p2penable=%d\n", pParam->p2penable);
	fwrite(writeBuff, 1, strlen(writeBuff), pVgConfigFile);

	memset(writeBuff, 0, VGMAXBUFFLEN);
	sprintf(writeBuff, "did=%s\n", pParam->p2pconfdid);
	fwrite(writeBuff, 1, strlen(writeBuff), pVgConfigFile);

	memset(writeBuff, 0, VGMAXBUFFLEN);
	sprintf(writeBuff, "apilicense=%s\n", pParam->p2pconfapil);
	fwrite(writeBuff, 1, strlen(writeBuff), pVgConfigFile);

	memset(writeBuff, 0, VGMAXBUFFLEN);
	sprintf(writeBuff, "crckey=%s\n", pParam->crckey);
	fwrite(writeBuff, 1, strlen(writeBuff), pVgConfigFile);

	memset(writeBuff, 0, VGMAXBUFFLEN);
	sprintf(writeBuff, "authzip=%s\n", pParam->authzip);
	fwrite(writeBuff, 1, strlen(writeBuff), pVgConfigFile);

	memset(writeBuff, 0, VGMAXBUFFLEN);
	sprintf(writeBuff, "authzhost=%s\n", pParam->authzhost);
	fwrite(writeBuff, 1, strlen(writeBuff), pVgConfigFile);

	memset(writeBuff, 0, VGMAXBUFFLEN);
	sprintf(writeBuff, "initstring=%s\n", pParam->initstring);
	fwrite(writeBuff, 1, strlen(writeBuff), pVgConfigFile);
	
	fclose(pVgConfigFile);

	return TRUE;
}

int get_p2p_info(ppcs_p2p_info *info)
{
	int ret = P2P_ERR;
	if (!access(PPCS_P2P_PARAM_FILE, F_OK))
	{
		FILE *pVgParamFile;
		char funcStr[12];
		pVgParamFile = fopen(PPCS_P2P_PARAM_FILE, "r");
		if (pVgParamFile == NULL)
		{
			printf("ppcs_check_p2p_param open file error\n");
			return 0;
		}
		memset(info, 0, sizeof(ppcs_p2p_info));
		get_value_string(pVgParamFile, "p2penable", funcStr, sizeof(funcStr));
		info->p2penable = atoi(funcStr);
		get_value_string(pVgParamFile, "did", info->p2pconfdid, sizeof(info->p2pconfdid));
		get_value_string(pVgParamFile, "apilicense", info->p2pconfapil, sizeof(info->p2pconfapil));
		get_value_string(pVgParamFile, "crckey", info->crckey, sizeof(info->crckey));
		get_value_string(pVgParamFile, "authzip", info->authzip, sizeof(info->authzip));
		get_value_string(pVgParamFile, "authzhost", info->authzhost, sizeof(info->authzhost));
		get_value_string(pVgParamFile, "initstring", info->initstring, sizeof(info->initstring));
		ret = P2P_OK;
	}
	
	return ret;
}

int get_dev_info(QMAPI_NET_DEVICE_INFO *devInfo)
{
	
	if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_DEVICECFG, 0, (void*)devInfo, sizeof(QMAPI_NET_DEVICE_INFO)) != QMAPI_SUCCESS)
	{
		DEBUG("net get devicecfg error.\n");
		return P2P_ERR;
	}
	return P2P_OK;
}

int ppcs_send_add(ppcs_p2p_send *handle)
{
	ppcs_p2p_send_add = handle;
	DEBUG("start，\n");
	while (ppcs_p2p_send_add) usleep(10);
	DEBUG("end，\n");
	return P2P_OK;
}

int ppcs_send_del(ppcs_p2p_send *handle)
{
	ppcs_p2p_send_del = handle;
	DEBUG("start，\n");
	while (ppcs_p2p_send_del) usleep(10);
	DEBUG("end，\n");
	return P2P_OK;
}

int ppcs_recv_add(ppcs_p2p_recv *handle)
{
	ppcs_p2p_recv_add = handle;
	DEBUG("start，\n");
	while (ppcs_p2p_recv_add) usleep(10);
	DEBUG("end，\n");
	return P2P_OK;
}

int ppcs_recv_del(ppcs_p2p_recv *handle)
{
	ppcs_p2p_recv_del = handle;
	DEBUG("start，\n");
	while (ppcs_p2p_recv_del) usleep(10);
	DEBUG("end，\n");
	return P2P_OK;
}

int ppcs_data_handle_control(unsigned short connid, unsigned short dataid, unsigned short control, comm_moment_t *comm)
{
	handle_control.connid = connid;
	handle_control.dataid = dataid;
	handle_control.control = control;
	if (comm) memcpy(&handle_control.comm, comm, sizeof(comm_moment_t));
	DEBUG("start，\n");
	while (handle_control.control) usleep(10);
	DEBUG("end，\n");

	return P2P_OK;
}

int ppcs_connect_handle_add(int handle)
{
	int ret = P2P_ERR;
	ppcs_p2p_connect *connect = malloc(sizeof(ppcs_p2p_connect));
	if (connect)
	{
		memset(connect, 0, sizeof(ppcs_p2p_connect));
		connect->p2p_handle = handle;
		connect->connstate = CONN_STATE_ACTIVE;
		connect->recv_status = RECV_STATUS_NEED_CMD;
		connect->recv_len = 0;
		connect->countdown = GEN_SEC_COUNT(90);
		ppcs_p2p_connect_add = connect;
		DEBUG("start，\n");
		while (ppcs_p2p_connect_add) usleep(10);
		DEBUG("end，\n");
		ret = P2P_OK;
	}
	return ret;
}

int ppcs_data_writev(int p2p_handle, struct iovec *iov, int iov_num)
{
	int wr, i, writelen;
	unsigned int wsize;
	
	i = 0;
	writelen = 0;
	while (i < iov_num)
	{	
		if (PPCS_Check_Buffer(p2p_handle, 1, &wsize, NULL) == ERROR_PPCS_SUCCESSFUL)
		{
			if (wsize + iov[i].iov_len < 128 * 1024)
			{
				wr = PPCS_Write(p2p_handle, 1, (CHAR_8* )(iov[i].iov_base), iov[i].iov_len);
				if (wr > 0)
				{
					i++;
					writelen += wr;
					continue;
				}
			}
		}
		break;
	}

	return writelen;
}


int ppcs_qdata_padding(ppcs_p2p_send *psend)
{
	int i = psend->packetindex;

	if (i!=PACK_INDEX_INIT && psend->packet[i].send_len)
	{
		int wr, num = 0;
		size_t sz  = psend->packet[i].hdr.sz;
		size_t len = 0;
		unsigned char paddingbuf[PADDING_BUFSZ];
		struct iovec iov[MAX_QDATA_PACKET_SZ / PADDING_BUFSZ + 2]; /* +2为除余数以及数据头 */
		if (psend->packet[i].send_len < sizeof(comm_data_header_t))
		{
			/* 剩余数据头发送 */
			len               = sizeof(comm_data_header_t) - psend->packet[i].send_len;
			iov[num].iov_base = (char*)&(psend->packet[i].hdr)+ psend->packet[i].send_len;
			iov[num].iov_len  = len;
			num++;
		}
		else if (sz)
		{
			sz -= psend->packet[i].send_len - sizeof(comm_data_header_t);
		}
		if (sz)
		{
			/* 剩余数据填充 */
			len += sz; /* 发送数据总长度 */
			while (1) {
				iov[num].iov_base = &paddingbuf[0];
				iov[num].iov_len  = (sz > PADDING_BUFSZ) ? PADDING_BUFSZ : sz;
				num++;
				if (sz > PADDING_BUFSZ)
				{
					sz -= PADDING_BUFSZ;
				}
				else
				{
					paddingbuf[sz - 1] = psend->packet[i].hdr.eb + 1; /* 表示填充数据，使接收方校验后丢弃 */
					break;
				}
			}
		}
		if (num)
		{
			wr = ppcs_data_writev(psend->p2p_handle, iov, num);
			if (wr == len)
			{
				psend->packetindex = PACK_INDEX_INIT;
				psend->send_padding = 0;
				memset(&psend->packet[i], 0, sizeof(data_send_packet));
				DEBUG("========= P2P %d.==========\n",psend->p2p_handle);
				return P2P_OK;
			}
			else
			{
				psend->packet[i].send_len += wr;
			}
		}
	}

	return P2P_ERR;
}

/*
	
*/
int ppcs_qdata_format(ppcs_p2p_send *psend)
{
	int enBufPolicy,ret;
	QMAPI_NET_DATA_PACKET packet;
	QMAPI_TIME FrameTime;
	data_send_packet *send;
	
	if (psend->packetindex != PACK_INDEX_INIT) /* 还有数据包没有发送完 */
	{
		return P2P_OK;
	}

	memset( &packet  , 0 , sizeof(packet));
	if (psend->FilesInfoHandle)
	{
		if (psend->send_pause)
		{
			return P2P_ERR;
		}
		if (!psend->send_count)
		{
			return P2P_ERR;
		}
		psend->send_count--;
		
		ret = QMAPI_Record_PlayBackGetReadPtrPos(0, psend->StreamReader, &packet, &FrameTime);
	}
	else
	{
		if (psend->send_key)
		{
			enBufPolicy = QMAPI_MBUF_POS_LAST_WRITE_nIFRAME;
		}
		else
		{
			enBufPolicy = QMAPI_MBUF_POS_CUR_READ;
		}
		ret = QMapi_buf_get_readptrpos( 0, psend->StreamReader, enBufPolicy , 0 , &packet);
	}

	if (ret == QMAPI_SUCCESS)
	{
		int i, pos = 0;
		unsigned char mf = 0;
		unsigned short sz;
		//if (!psend->send_audio) return P2P_ERR; 
		for (i=0; i<MAX_QDATA_PACKET_NUM && !mf; i++)
		{
			if (packet.dwPacketSize > pos+MAX_QDATA_PACKET_SZ)
			{
				sz = MAX_QDATA_PACKET_SZ;
				mf = 0;
			}
			else
			{
				sz = packet.dwPacketSize - pos;
				mf = 1;
			}
			/* 填充数据结构 */
			send = &(psend->packet[i]);
			send->pdata = packet.pData + pos;
			send->send_len = 0;
			send->size = sz;
			send->hdr.cf = 1;
			send->hdr.sn = psend->packetsn++;
			send->hdr.id = psend->dataid;
			send->hdr.mf = mf;
			send->hdr.sz = sz;
			send->hdr.eb = (unsigned char)(*(packet.pData + pos + sz - 1));
			send->hdr.ts = packet.stFrameHeader.dwTimeTick;
			if (packet.stFrameHeader.dwVideoSize)
			{
				if( 0 == packet.stFrameHeader.byFrameType )
				{
					send->hdr.dt = GEN_DATA_TYPE(TYPE_VIDEO_H264, H264_TYPE_IFRAME);
					//DEBUG("w:%u. h:%u. ts:%lu. sz:%lu.\n",
					//	packet.stFrameHeader.wVideoWidth,packet.stFrameHeader.wVideoHeight,packet.stFrameHeader.dwTimeTick,packet.dwPacketSize);
				}
				else if( 1 == packet.stFrameHeader.byFrameType )
					send->hdr.dt = GEN_DATA_TYPE(TYPE_VIDEO_H264, H264_TYPE_PFRAME);
				else
					send->hdr.dt = GEN_DATA_TYPE(TYPE_VIDEO_H264, H264_TYPE_BFRAME);
				send->hdr.vdo.fm = 0;
				send->hdr.vdo.fs = 0;
				send->hdr.vdo.fps = psend->fps;
				send->hdr.pic.w = packet.stFrameHeader.wVideoWidth / 16;
				send->hdr.pic.h = packet.stFrameHeader.wVideoHeight / 8;
				
			}
			else if (packet.stFrameHeader.wAudioSize)
			{
				send->hdr.dt = GEN_DATA_TYPE(TYPE_AUDIO_G711, G711_SUBTYPE_A);
				send->hdr.ado.chs = psend->audioch;
				send->hdr.ado.KBps = psend->audioKBps;
				send->hdr.hz = psend->audiohz;
			}
			pos += sz;
		}
		psend->packetindex = 0;
		psend->send_key = 0;
	}
	else
	{
		DEBUG("get read data error. %d.\n",psend->StreamReader);
		psend->send_key = 1;
	}

	return P2P_OK;
}

int ppcs_qdata_send(ppcs_p2p_send *psend)
{
	int i, sz, j=0;
	struct iovec iov[2 * MAX_QDATA_PACKET_NUM];
	
	for (i=psend->packetindex; i<MAX_QDATA_PACKET_NUM; i++)
	{
		if (!psend->packet[i].pdata)
		{
			break;
		}
		if (psend->packet[i].send_len < sizeof(comm_data_header_t))
		{
			/* 剩余数据头发送 */
			iov[j].iov_base = (char*)&psend->packet[i].hdr + psend->packet[i].send_len;
			iov[j].iov_len  = sizeof(comm_data_header_t) - psend->packet[i].send_len;
			j++;
			sz = 0;
		}
		else
		{
			sz = psend->packet[i].send_len - sizeof(comm_data_header_t);
		}
		iov[j].iov_base = psend->packet[i].pdata + sz;
		iov[j].iov_len  = psend->packet[i].size - sz;
		j++;
	}
	if (j)
	{
		int writelen = ppcs_data_writev(psend->p2p_handle, iov, j);
		if (writelen > 0)/* 有成功发送数据 */
		{
			for (i=psend->packetindex; i<MAX_QDATA_PACKET_NUM; i++)
			{				
				if (!psend->packet[i].pdata)
				{
					psend->packetindex = PACK_INDEX_INIT;/* 数据发送完成重置索引 */
					break;
				}
				if (psend->packet[i].hdr.eb != (unsigned char)*(psend->packet[i].pdata + psend->packet[i].hdr.sz - 1))
				{
					DEBUG("======error==========\n");
				}
				sz  = psend->packet[i].hdr.sz;
				sz += sizeof(comm_data_header_t);
				sz -= psend->packet[i].send_len;
				if (writelen >= sz)
				{
					writelen -= sz;
					memset(&psend->packet[i], 0, sizeof(data_send_packet));
				}
				else
				{
					psend->packetindex = i;
					psend->packet[i].send_len += writelen;
					break;
				}
			}
			return P2P_OK;
		}
		return P2P_ERR;
	}

	return P2P_OK;
}

int ppcs_qdata_contrl(ppcs_p2p_send *data, int cmd, comm_moment_t *comm)
{
#define DEFAULT_SEND_COUNT 4

	int r = P2P_ERR;
	
	switch (cmd)
	{
		case CONT_START:
		{
			if (data->FilesInfoHandle)
			{
				data->send_pause = 0;
			}
			break;
		}
		case CONT_STOP:
		{
			break;
		}
		case CONT_PAUSE:
		{
			if (data->FilesInfoHandle)
			{
				data->send_pause = 1;
			}
			break;
		}
		case CONT_SEEK:
		{
			if (data->FilesInfoHandle)
			{
				struct tm tt;
				unsigned int SeekPlayStamp; /*相对时间，单位毫秒*/

				tt.tm_year  = comm->momdate.year - 1900;
				tt.tm_mon   = comm->momdate.month - 1;
				tt.tm_mday  = comm->momdate.day;
				tt.tm_hour  = comm->momtime.hour;
				tt.tm_min   = comm->momtime.minute;
				tt.tm_sec   = comm->momtime.second;
				tt.tm_isdst = 0;
				SeekPlayStamp  = mktime(&tt);
				SeekPlayStamp  -= data->file_start_time;
				SeekPlayStamp  *= 1000;
				if (QMAPI_Record_PlayBackControl(0, data->StreamReader, (DWORD)QMAPI_REC_PLAYSETPOS, (char*)(&SeekPlayStamp), 4, NULL, NULL) == QMAPI_SUCCESS)
				{
					data->send_pause = 0;
					data->send_key = 1;
				}
			}
			break;
		}
		case CONT_AUDIO_START:
		{
			data->send_audio = 1;
			break;
		}
		case CONT_AUDIO_STOP:
		{
			data->send_audio = 0;
			break;
		}
		case CONT_REC_GETFRAME:
		{
			if (data->FilesInfoHandle)
			{
				data->send_count = DEFAULT_SEND_COUNT;
			}
			break;
		}
		default:
			break;
	}

	return r;
}

static int check_data_recv_buf(ppcs_p2p_recv *precv, size_t len)
{
	if (precv->precvbuf)
	{
		if (precv->recvbuflen < len)
		{
			free(precv->precvbuf);
			precv->precvbuf   = NULL;
			precv->recvbuflen = 0;
		}
	}
	if (precv->precvbuf == NULL)
	{
		printf("####### malloc len:%d. \n",len);
		precv->precvbuf = (unsigned char*)malloc(len);
		if (precv->precvbuf)
		{
			precv->recvbuflen = len;
		}
	}
	return precv->precvbuf ? P2P_OK : P2P_ERR;
}


static void ppcs_data_recv(ppcs_p2p_recv *precv)
{
	size_t len = 0;
	char *pbuf = NULL;
	do {
		/* 计算本次需要读取的数据和长度 */
		if (precv->datahdrlen < sizeof(comm_data_header_t))
		{
			/* 固定头 */
			len  = sizeof(comm_data_header_t) - precv->datahdrlen;
			pbuf = (char*)&precv->datahdr + precv->datahdrlen;
			precv->recvstate |= RECV_STATE_DATA_HEADER;
			break;
		}
		if (precv->datahdr.xf == 1)
		{
			/* 扩展头 */
			if (precv->exthdrlen < sizeof(comm_extension_header_t))
			{
				len  = sizeof(comm_extension_header_t) - precv->exthdrlen;
				pbuf = (char*)&precv->exthdr + precv->exthdrlen;
				precv->recvstate |= RECV_STATE_EXT_HEADER;
				break;
			}
			if (precv->extdatalen < precv->exthdr.sz)
			{
				/* 扩展数据 */
				if (check_data_recv_buf(precv, precv->exthdr.sz) == P2P_OK)
				{
					pbuf = (char*)precv->precvbuf + precv->extdatalen;
					len  = precv->exthdr.sz - precv->extdatalen;
					precv->recvstate |= RECV_STATE_EXT_DATA;
					break;
				}
				/* 无法分配到足够的缓冲区，连接关闭状态 */
				precv->recvstate &= ~RECV_STATE_ACTIVE;
				printf("extension %u out of memory\n", precv->exthdr.sz);
				return;
			}
		}
		/* 负载数据 */
		if (precv->datalen < precv->datahdr.sz)
		{
			if (check_data_recv_buf(precv, precv->datahdr.sz) == P2P_OK)
			{
				pbuf = (char*)precv->precvbuf + precv->datalen;
				len  = precv->datahdr.sz - precv->datalen;
				precv->recvstate |= RECV_STATE_DATA_DATA;
				break;
			}
			/* 无法分配到足够的缓冲区，连接关闭状态 */
			precv->recvstate &= ~RECV_STATE_ACTIVE;
			printf("payload %u out of memory\n", precv->datahdr.sz);
			return;
		}
	} while(0);
	/* 数据接收 */
	if (pbuf)
	{	
		int r = 0;
		
		int readlen = len;
		if (PPCS_Read(precv->fd, 1, pbuf, &readlen, 3 * 1000) == ERROR_PPCS_SUCCESSFUL)
		{
			r = readlen;
		}
		if (r <= 0)
		{
			/* 连接关闭状态 */
			printf("recv:%d. error=%d.fd:%d.conntype:%d.\n", r,errno,precv->fd,precv->conntype);
			precv->recvstate &= ~RECV_STATE_ACTIVE;
		}
		else
		{
			if (precv->recvstate & RECV_STATE_DATA_HEADER)
			{
				precv->datahdrlen += r;
			}
			if (precv->recvstate & RECV_STATE_EXT_HEADER)
			{
				precv->exthdrlen += r;
			}
			if (precv->recvstate & RECV_STATE_EXT_DATA)
			{
				precv->extdatalen += r;
			}
			if (precv->recvstate & RECV_STATE_DATA_DATA)
			{
				precv->datalen += r;
			}
			/* 接收数据处理 */
			if (len == (size_t)r)
			{
				comm_data_header_t *phdr = &precv->datahdr;
				do {
					if (precv->recvstate & RECV_STATE_DATA_HEADER)
					{
						if (++precv->lastrecvsn != phdr->sn)
						{
							/* 重设数据包序号 */
							precv->lastrecvsn = phdr->sn;
							printf("sn init or error\n");
						}
						if (phdr->sz > 0 || phdr->xf == 1)
						{
							/* 继续接收扩展头或负载数据 */
							break;
						}
					}
					if (precv->recvstate & RECV_STATE_EXT_HEADER)
					{
						if (precv->exthdr.sz > 0 || phdr->sz > 0)
						{
							/* 继续接收扩展数据或负载数据 */
							break;
						}
					}
					if (precv->recvstate & RECV_STATE_EXT_DATA)
					{
						/* 扩展数据处理，注意会被负载数据覆盖 */
						if (phdr->sz > 0)
						{
							/* 继续接收负载数据 */
							break;
						}
					}
					do {
						/* 是否有负载数据接收 */
						if (precv->recvstate & RECV_STATE_DATA_DATA)
						{
							/* 接收数据校验 */
							if (phdr->cf == 1)
							{
								if (phdr->eb != precv->precvbuf[precv->datalen - 1])
								{
									printf("recv data check error\n");
									break;
								}
							}
						}
						switch (DATA_MAIN_TYPE(phdr->dt)) {
						case TYPE_META_DATA:
							break;
						case TYPE_VIDEO_H264:
						case TYPE_AUDIO_RAW:
						case TYPE_AUDIO_G711:
						case TYPE_AUDIO_ADPCM:
							if (phdr->mf == 1)
							{
								/* 提交完整数据帧 */
								QMAPI_Audio_Decoder_Data(precv->audioHandle, (char*)precv->precvbuf, precv->datalen);
							}
							break;
						default:
							break;
						}
					} while(0);
					/* 准备接收下一个数据包 */
					precv->datahdrlen = 0;
					precv->datalen    = 0;
					precv->exthdrlen  = 0;
					precv->extdatalen = 0;
				} while(0);
			}
			/* 重置接收状态 */
			precv->recvstate &= ~(RECV_STATE_DATA_HEADER | RECV_STATE_EXT_HEADER | RECV_STATE_EXT_DATA | RECV_STATE_DATA_DATA);
		}
	}
}

void ppcs_p2p_recv_data(void* arg)
{
	int r;
	unsigned int rsize;
	ppcs_p2p_recv *precv;
	unsigned char sleepcount = 0;
	
	prctl(PR_SET_NAME, (unsigned long)__FUNCTION__);
	pthread_detach(pthread_self()); /* 线程结束时自动清除 */

	DEBUG(" create ok.\n");

	while (ppcs_p2p_run)
	{
		if (ppcs_p2p_recv_add) /* 添加节点 */
		{
			ppcs_p2p_recv_add->pnext = ppcs_p2p_recv_head;
			ppcs_p2p_recv_head = ppcs_p2p_recv_add;
			ppcs_p2p_recv_add = NULL;
		}
		if (ppcs_p2p_recv_del) /* 删除节点 */
		{
			ppcs_p2p_recv *pdata = NULL;
			precv = ppcs_p2p_recv_head;
			while (precv)
			{
				if (ppcs_p2p_recv_del == precv)
				{
					if (pdata == NULL)
					{
						ppcs_p2p_recv_head = precv->pnext;
					}
					else
					{
						pdata->pnext = precv->pnext;
					}
					break;
				}
				pdata = precv;
				precv = precv->pnext;
			}
			ppcs_p2p_recv_del = NULL;
		}

		precv = ppcs_p2p_recv_head;
		while (precv) /* 遍历链表 发送数据*/
		{
			rsize = 0;
			r = PPCS_Check_Buffer(precv->fd, 1, NULL , &rsize);
			if (r < 0 || rsize > 0)
			{
				ppcs_data_recv(precv);
			}
			precv = precv->pnext;
		}
		if ((sleepcount++) % 3 == 0)
		{
			usleep(1000);
		}
	}
	ppcs_p2p_pthread_count--;
	
	return ;
}

/*
	P2P 帧数据发送线程
*/
void ppcs_p2p_send_data(void* arg)
{
	ppcs_p2p_send *data;
	int debugcount = 0;
	unsigned char sleepcount = 0;
	prctl(PR_SET_NAME, (unsigned long)__FUNCTION__);
	pthread_detach(pthread_self()); /* 线程结束时自动清除 */
	DEBUG(" create ok.\n");

	while (ppcs_p2p_run)
	{
		if (ppcs_p2p_send_add) /* 添加节点 */
		{
			ppcs_p2p_send_add->pnext = ppcs_p2p_send_head;
			ppcs_p2p_send_head = ppcs_p2p_send_add;
			ppcs_p2p_send_add = NULL;
		}
		if (ppcs_p2p_send_del) /* 删除节点 */
		{
			ppcs_p2p_send *pdata = NULL;
			data = ppcs_p2p_send_head;
			while (data)
			{
				if (ppcs_p2p_send_del == data)
				{
					if (pdata == NULL)
					{
						ppcs_p2p_send_head = data->pnext;
					}
					else
					{
						pdata->pnext = data->pnext;
					}
					break;
				}
				pdata = data;
				data = data->pnext;
			}
			ppcs_p2p_send_del = NULL;
		}
		if (handle_control.control)
		{
			data = ppcs_p2p_send_head;
			while (data)
			{
				if (data->connid == handle_control.connid && data->dataid==handle_control.dataid)
				{
					ppcs_qdata_contrl(data, handle_control.control, &handle_control.comm);
					break;
				}
				data = data->pnext;
			}
			handle_control.control = 0;
		}
		
		data = ppcs_p2p_send_head;
		while (data) /* 遍历链表 发送数据*/
		{
			if (data->send_padding)
			{
				ppcs_qdata_padding(data);
			}
			else
			{
				if (ppcs_qdata_format(data) != P2P_OK
					|| ppcs_qdata_send(data) != P2P_OK)
				{
				//	DEBUG("handle:%d. conn:%u. data:%u. stream:%d. \n",
				//		data->p2p_handle,data->connid,data->dataid,data->StreamReader);
				}
			}
			data = data->pnext;
		}
		if ((sleepcount++) % 7 == 0) /* 根据实际效果可以考虑去掉，因为在获取码流数据的时候有等待 */
		{
			usleep(12);
			if (debugcount++ >= 10*60)
			{
				debugcount = 0;
				//DEBUG(" run ok.\n");
			}
		}
	}
	ppcs_p2p_pthread_count--;

	return ;
}

static int ppcs_data_authenticate(ppcs_p2p_connect *pconnect)
{	
	int r=-1, len;
	comm_data_header_t hdr = {0};
	
	/* 验证连接 */
	len = sizeof(comm_data_header_t);
	r = PPCS_Read(pconnect->p2p_handle, 1, (char *)&hdr, &len, 3 * 1000);
	if (r != ERROR_PPCS_SUCCESSFUL)
	{
		memset(&hdr, 0, sizeof(comm_data_header_t));
	}
	
	if (hdr.dt == DATA_CONN_AUTH && hdr.sz == sizeof(comm_link_id_t))
	{
		comm_link_id_t id;

		len = sizeof(comm_link_id_t);
		r = PPCS_Read(pconnect->p2p_handle, 1, (char *)&id, &len, 3 * 1000);
		if (r == ERROR_PPCS_SUCCESSFUL)
		{	
			if (pconnect->connid == id.connid)/* 连接验证成功 */
			{				
				pconnect->connstate |= CONN_DATA_CONNECT;
			}
		}
	}

	return r;
}

static void keep_alive_control(ppcs_p2p_connect *pconnect)
{
	SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
}

static void sys_device_info(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_GET)
	{
		dev_info_t devinfo;
		QMAPI_NET_DEVICE_INFO dev;
		
		assert(pconnect->cmd.cbsize == 0);
		memset(&dev, 0, sizeof(QMAPI_NET_DEVICE_INFO));
		if (get_dev_info(&dev) == P2P_OK)
		{
			memset(&devinfo, 0, sizeof(dev_info_t));
			devinfo.analog_ch_num  = 1;
			devinfo.digital_ch_num = 0;
			devinfo.total_ch_num   = 1;
			devinfo.disk_num       = 0;
			devinfo.alarmin_num    = 0;
			devinfo.alarmout_num   = 0;
			devinfo.ethernet_num   = 1;

			SET_CMDSZRET(pconnect->cmd, sizeof(dev_info_t), ERR_NO_ERROR);
			if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
			{
				DEBUG("send fail\n");
			}
			
			if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&devinfo, sizeof(dev_info_t)) < 0)
			{
				DEBUG("send fail\n");
			}
			return;
		}
	}
	SET_CMDSZRET(pconnect->cmd, sizeof(dev_info_t), ERR_NO_SUPPORT);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return ;
}

static int conn_check_user(ppcs_p2p_connect *pconnect, comm_user_login_t *pcomm)
{
	int i;
	
	for(i = 0 ; i < 10 ; i ++)
	{
		QMAPI_NET_USER_INFO       stUserInfo;
		memset(&stUserInfo, 0, sizeof(stUserInfo));
		if (QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_USERCFG, i, (void*)&stUserInfo, sizeof(stUserInfo)) != QMAPI_SUCCESS)
		{
			DEBUG("net get usercfg error\n");
			return -1;
		}
		if ( strcmp(stUserInfo.csUserName, pcomm->username) == 0 )
		{
			if ( strcmp(pcomm->password, stUserInfo.csPassword) == 0 )
			{
				DEBUG("user check ok.\n");
				return 0;
			}
		}
	}

	return -2;
}


static void conn_user_login(ppcs_p2p_connect *pconnect)
{
	static unsigned short ConnectId;
	if (pconnect)
	{
		comm_user_login_t *pcomm = (comm_user_login_t*)pconnect->recv_buffer;
		assert(pconnect->cmd.cbsize == sizeof(comm_user_login_t));
	
		if (pconnect->verifytimes >= 5)
		{
			SET_CMDSZRET(pconnect->cmd, 0, ERR_VERIFY_OUT_TIMES);
		}
		else
		{
			if (conn_check_user(pconnect, pcomm) == 0)/* 比对 用户名 & 密码 */
			{
				comm_link_info_t comm;
				pconnect->connid     = ConnectId++;
				pconnect->connstate |= CONN_STATE_LOGIN; /* 进入登录状态 */
				pconnect->countdown  = GEN_SEC_COUNT(90); /* 保活计数开始 */
				pconnect->verifytimes = 0;
				SET_CMDSZRET(pconnect->cmd, sizeof(comm_link_info_t), ERR_NO_ERROR);
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
				{
					DEBUG("send fail\n");
				}
				memset(&comm, 0, sizeof(comm_link_info_t));
				comm.linkid.connid  = pconnect->connid;
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&comm, sizeof(comm_link_info_t)) < 0)
				{
					DEBUG("send fail\n");
				}
				return;
			}
			pconnect->verifytimes++;
		}
		
		SET_CMDSZRET(pconnect->cmd, 0, ERR_USER_PASSWORD);
		PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t));
	}
}

int video_capture_jpeg(ppcs_p2p_connect *pconnect)
{
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}


int video_osd_name(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_GET)
	{
		venc_osd_t osd;
		QMAPI_NET_CHANNEL_OSDINFO osdinfo;
		
		memset(&osd, 0, sizeof(venc_osd_t));
		if (QMapi_sys_ioctrl(1, QMAPI_SYSCFG_GET_OSDCFG, 0, &osdinfo, sizeof(QMAPI_NET_CHANNEL_OSDINFO)) == QMAPI_SUCCESS)
		{
			memcpy(osd.osdname, osdinfo.csChannelName, QMAPI_NAME_LEN);
			DEBUG(" osdname:%s. \n",osd.osdname);
			SET_CMDSZRET(pconnect->cmd, sizeof(venc_osd_t), ERR_NO_ERROR);
			if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
			{
				DEBUG("send fail\n");
			}
			if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&osd, sizeof(venc_osd_t)) < 0)
			{
				DEBUG("send fail\n");
			}
			return P2P_OK;
		}
	}
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}

int video_encode_range(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_GET)
	{
		int index;
		QMAPI_NET_SUPPORT_STREAM_FMT support;
		
		assert(pconnect->cmd.cbsize == sizeof(int));
		memcpy(&index, pconnect->recv_buffer, sizeof(int));
		
		if (QMapi_sys_ioctrl(1, QMAPI_SYSCFG_GET_SUPPORT_STREAM_FMT, 0, &support, sizeof(QMAPI_NET_SUPPORT_STREAM_FMT)) == QMAPI_SUCCESS)
		{
			int i, sz,count = 0;
			vdenc_range_t enc[10];

			for (i=0; i<10; i++)
			{
				if (support.stVideoSize[index][i].nHeight <= 0)
				{
					break;
				}
				count++;
				
				enc[i].minenc.bitrate   = support.stVideoBitRate[index].nMin;
				enc[i].minenc.encformat = VIDEO_PROFILE_H264_MP;
				enc[i].minenc.framerate = support.stMINFrameRate[index];
				enc[i].minenc.gop       = 100;   /* 需要重新确认 */
				enc[i].minenc.quality   = 0;     /* 需要重新确认 */
				enc[i].minenc.encmode   = 0;     /* 需要重新确认 */
				enc[i].minenc.picsz.height = support.stVideoSize[index][i].nHeight;
				enc[i].minenc.picsz.width  = support.stVideoSize[index][i].nWidth;

				enc[i].maxenc.bitrate   = support.stVideoBitRate[index].nMax;
				enc[i].maxenc.encformat = VIDEO_PROFILE_H264_MP;
				enc[i].maxenc.framerate = support.stMAXFrameRate[index];
				enc[i].maxenc.gop       = 100;   /* 需要重新确认 */
				enc[i].maxenc.quality   = 0;     /* 需要重新确认 */
				enc[i].maxenc.encmode   = 0;     /* 需要重新确认 */
				enc[i].maxenc.picsz.height = support.stVideoSize[index][i].nHeight;
				enc[i].maxenc.picsz.width  = support.stVideoSize[index][i].nWidth;
			}

			if (count)
			{
				sz = count * sizeof(vdenc_range_t);
				SET_CMDSZRET(pconnect->cmd, sz, ERR_NO_ERROR);
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0
					|| PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)enc, sz) < 0)
				{
					DEBUG("send fail\n");
				}
				return P2P_OK;
			}
		}
	}
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}

int video_encode_cfg(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_GET)
	{
		int index;
		video_encode_t enc = {0};
		
		assert(pconnect->cmd.cbsize == sizeof(int));
		memcpy(&index, pconnect->recv_buffer, sizeof(int));

		QMAPI_NET_CHANNEL_PIC_INFO  pic_info;
		QMAPI_NET_COMPRESSION_INFO  *info = NULL;
		memset(&pic_info, 0, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
		if (QMapi_sys_ioctrl(1, QMAPI_SYSCFG_GET_PICCFG, pconnect->cmd.cmdch, &pic_info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) == QMAPI_SUCCESS)
		{
			DEBUG("===index:%d====\n",index);
			if (index == 0)
			{
				info = &pic_info.stRecordPara;
			}
			else if (index == 1)
			{
				info = &pic_info.stNetPara;
			}
			if (info)
			{
				enc.bitrate = info->dwBitRate;
				enc.encformat = VIDEO_PROFILE_H264_MP;
				enc.encmode = info->dwRateType ? ENCODE_MODE_VBR : ENCODE_MODE_CBR;
				enc.framerate = info->dwFrameRate;
				enc.gop = info->dwMaxKeyInterval;
				enc.picsz.height = info->wHeight;
				enc.picsz.width = info->wWidth;
				enc.quality = info->dwImageQuality;
				
				SET_CMDSZRET(pconnect->cmd, sizeof(video_encode_t), ERR_NO_ERROR);
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
				{
					DEBUG("send fail\n");
				}
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&enc, sizeof(video_encode_t)) < 0)
				{
					DEBUG("send fail\n");
				}
				return P2P_OK;
			}
		}
	}
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}

int duration_format_convert(QMAPI_TIME *dms, comm_moment_t *comm, int convert)
{
	if (convert)
	{
		dms->dwYear      = comm->momdate.year;
		dms->dwMonth     = comm->momdate.month;
		dms->dwDay       = comm->momdate.day;
		dms->dwHour      = comm->momtime.hour;
		dms->dwMinute    = comm->momtime.minute;
		dms->dwSecond    = comm->momtime.second;
	}
	else
	{
		comm->momdate.year      = dms->dwYear;
		comm->momdate.month     = dms->dwMonth;
		comm->momdate.day       = dms->dwDay;
		comm->momtime.hour      = dms->dwHour;
		comm->momtime.minute    = dms->dwMonth;
		comm->momtime.second    = dms->dwSecond;
	}

	return P2P_OK;
}

int qdata_real_play(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		int StreamReader = 0,Stream;
		comm_link_info_t comm;
		assert(pconnect->cmd.cbsize == sizeof(comm_link_info_t));

		memcpy(&comm, pconnect->recv_buffer, sizeof(comm_link_info_t));
		if (pconnect->connid == comm.linkid.connid)
		{
			/* 打开相应通道的码流 */
			QMAPI_NET_CHANNEL_PIC_INFO  pic_info;
			QMAPI_NET_COMPRESSION_INFO  *info = NULL;
			
			Stream = comm.streamindex;
			memset(&pic_info, 0, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
			if (QMapi_buf_add_reader(0 , 0, (QMAPI_STREAM_TYPE_E)Stream , &StreamReader) == QMAPI_SUCCESS
				&& QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PICCFG, pconnect->cmd.cmdch, &pic_info, sizeof(QMAPI_NET_CHANNEL_PIC_INFO)) == QMAPI_SUCCESS)
			{
				DEBUG("cd:%d. Stream:%d. read:%d. \n",pconnect->cmd.cmdch,Stream,StreamReader);
				if (Stream == QMAPI_MAIN_STREAM)
				{
					info = &pic_info.stRecordPara;
				}
				else if (Stream == QMAPI_SECOND_STREAM)
				{
					info = &pic_info.stNetPara;
				}
				ppcs_p2p_send *data = malloc(sizeof(ppcs_p2p_send));
				if (data && info)
				{
					SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
					if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
					{
						DEBUG("send fail\n");
					}

					memset(data, 0, sizeof(ppcs_p2p_send));
					data->p2p_handle = pconnect->p2p_handle;
					data->connid = pconnect->connid;
					data->dataid = comm.linkid.dataid;
					data->fps = info->dwFrameRate;
					if (info->wEncodeAudio) /* 音频参数需要再确认 */
					{
						data->audioch = 1;
						data->audioKBps = 8;
						data->audiohz = 16000;
					}
					data->index = (unsigned char)(Stream);
					data->send_key = 1;
					data->StreamReader = StreamReader;
					if (pconnect->sendpacket.pdata)
					{
						data->send_padding = 1;
						data->packetindex = 0;
						memcpy(&data->packet[0], &pconnect->sendpacket, sizeof(data_send_packet));
						memset(&pconnect->sendpacket, 0, sizeof(data_send_packet));
					}
					else
					{
						data->packetindex = PACK_INDEX_INIT;
					}
					ppcs_send_add(data);
					return P2P_OK;
				}
				QMapi_buf_del_reader( 0, StreamReader);
			}
		}
		DEBUG("cd:%d. read:%d. \n",pconnect->cmd.cmdch,StreamReader);
	}
	
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}

int qdata_rec_play(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		comm_link_info_t comm;
		assert(pconnect->cmd.cbsize == sizeof(comm_link_info_t));

		memcpy(&comm, pconnect->recv_buffer, sizeof(comm_link_info_t));
		if (pconnect->connid == comm.linkid.connid)
		{
			QMAPI_NET_FILECOND find;
			int files_info_handle;
			
			find.dwChannel = pconnect->cmd.cmdch;
			find.dwFileType = QMAPI_NET_RECORD_TYPE_SCHED|QMAPI_NET_RECORD_TYPE_MOTION|QMAPI_NET_RECORD_TYPE_ALARM|QMAPI_NET_RECORD_TYPE_CMD|QMAPI_NET_RECORD_TYPE_MANU;
			duration_format_convert(&find.stStartTime, &comm.recch.recdur.beginning, 1);
			duration_format_convert(&find.stStopTime, &comm.recch.recdur.ending, 1);
			
			files_info_handle = QMAPI_Record_FindFile(pconnect->cmd.cmdch, &find);
			if (files_info_handle > 0)
			{
				struct tm tt;
				int play_back_handle;
				comm_moment_t moment;
				QMAPI_NET_FINDDATA finddata;
				QMAPI_RECFILE_INFO info;
				
				if (QMAPI_Record_FindNextFile(pconnect->cmd.cmdch, files_info_handle, &finddata) == QMAPI_SEARCH_FILE_SUCCESS)
				{
					play_back_handle = QMAPI_Record_PlayBackByName(pconnect->cmd.cmdch, finddata.csFileName);
					if (play_back_handle>0 && QMAPI_Record_PlayBackGetInfo(pconnect->cmd.cmdch, play_back_handle, &info)==QMAPI_SUCCESS)
					{
						ppcs_p2p_send *data = malloc(sizeof(ppcs_p2p_send));
						if (data)
						{
							SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
							if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
							{
								DEBUG("send fail\n");
							}
							memset(data, 0, sizeof(ppcs_p2p_send));
							data->p2p_handle = pconnect->p2p_handle;
							data->connid = pconnect->connid;
							data->dataid = comm.linkid.dataid;
							data->fps = info.u16FrameRate;
							if (info.u16HaveAudio) /* 音频参数需要再确认 */
							{
								data->audioch = 1;
								data->audioKBps = 8;
								data->audiohz = 16000;
								data->send_audio = 1;
							}
							data->send_key = 1;
							data->StreamReader = play_back_handle;
							data->FilesInfoHandle = files_info_handle;
							duration_format_convert(&finddata.stStartTime, &moment, 0);
							tt.tm_year  = moment.momdate.year - 1900;
							tt.tm_mon   = moment.momdate.month - 1;
							tt.tm_mday  = moment.momdate.day;
							tt.tm_hour  = moment.momtime.hour;
							tt.tm_min   = moment.momtime.minute;
							tt.tm_sec   = moment.momtime.second;
							tt.tm_isdst = 0;
							data->file_start_time   = mktime(&tt);/* 记录文件开始时间 秒*/
							if (pconnect->sendpacket.pdata)
							{
								data->send_padding = 1;
								data->packetindex = 0;
								memcpy(&data->packet[0], &pconnect->sendpacket, sizeof(data_send_packet));
								memset(&pconnect->sendpacket, 0, sizeof(data_send_packet));
							}
							else
							{
								data->packetindex = PACK_INDEX_INIT;
							}
							ppcs_send_add(data);
							return P2P_OK;
						}
						QMAPI_Record_PlayBackStop(pconnect->cmd.cmdch, play_back_handle);
					}
				}
				QMAPI_Record_FindClose(pconnect->cmd.cmdch, files_info_handle);
			}
		}
	}
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}

static int qdata_play_stop(ppcs_p2p_connect *pconnect, comm_play_control_t *comm)
{
	int r = P2P_ERR;
	ppcs_p2p_send *pdata = ppcs_p2p_send_head;
	while (pdata)
	{
		if (pdata->connid == pconnect->connid)
		{
			ppcs_send_del(pdata);
			if (pdata->packetindex!=PACK_INDEX_INIT && pdata->packet[pdata->packetindex].send_len)
			{
				DEBUG("=============%d=============\n",pdata->p2p_handle);
				memcpy(&pconnect->sendpacket, &(pdata->packet[pdata->packetindex]), sizeof(data_send_packet));
			}
			if (pdata->FilesInfoHandle)
			{
				if (pdata->StreamReader) QMAPI_Record_PlayBackStop(0, pdata->StreamReader);
				QMAPI_Record_FindClose(0, pdata->FilesInfoHandle);
			}
			else
			{
				QMapi_buf_del_reader( 0, pdata->StreamReader);
			}
			free(pdata);
			r = P2P_OK;
			break;
		}
		pdata = pdata->pnext;
	}
	return r;
}

static int qdata_play_seek(ppcs_p2p_connect *pconnect, comm_play_control_t *comm)
{
	ppcs_data_handle_control(pconnect->connid,comm->id.dataid, CONT_SEEK, &comm->playmom);
	return P2P_OK;
}

static int qdata_play_start(ppcs_p2p_connect *pconnect, comm_play_control_t *comm)
{
	ppcs_data_handle_control(pconnect->connid,comm->id.dataid, CONT_START, NULL);
	return P2P_OK;
}

static int qdata_play_pause(ppcs_p2p_connect *pconnect, comm_play_control_t *comm)
{
	ppcs_data_handle_control(pconnect->connid,comm->id.dataid, CONT_PAUSE, NULL);
	return P2P_OK;
}

static int qdata_play_sound(ppcs_p2p_connect *pconnect, comm_play_control_t *comm)
{
	ppcs_data_handle_control(pconnect->connid,comm->id.dataid, comm->param?CONT_AUDIO_START:CONT_AUDIO_STOP, NULL);
	return P2P_OK;
}


int qdata_play_control(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		int r = P2P_ERR;
		struct {
			int cmd;
			comm_play_control_t comm;
		} ctrl;
		
		assert(pconnect->cmd.cbsize == sizeof(ctrl));

		memcpy(&ctrl, pconnect->recv_buffer, sizeof(ctrl));
		if (pconnect->connid == ctrl.comm.id.connid)
		{
			DEBUG("ctrl.cmd:%d. \n",ctrl.cmd);
			switch(ctrl.cmd)
			{
				case PLAY_CMD_SOUND:
					r = qdata_play_sound(pconnect, &ctrl.comm);
					break;
				case PLAY_CMD_STOP:
					r = qdata_play_stop(pconnect, &ctrl.comm);
					break;
				case PLAY_CMD_SEEK:
					r = qdata_play_seek(pconnect, &ctrl.comm);
					break;
				case PLAY_CMD_REVERSE:
					break;
				case PLAY_CMD_PAUSE:
					r = qdata_play_pause(pconnect, &ctrl.comm);
					break;
				case PLAY_CMD_START:
					r = qdata_play_start(pconnect, &ctrl.comm);
					break;
				default:
					break;
			}
			if (r == P2P_OK)
			{
				SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
				{
					DEBUG("send fail\n");
				}
				return P2P_OK;
			}
		}
	}
	DEBUG("========\n");
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}

int qdata_rec_tssync(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		comm_link_id_t linkid;

		assert(pconnect->cmd.cbsize == sizeof(comm_link_id_t));
		memcpy(&linkid, pconnect->recv_buffer, sizeof(comm_link_id_t));
	}
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}

/*
	获取本地设备录像数据
	不需要消息返回
*/
int qdata_rec_get_frame(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		comm_link_id_t linkid;

		assert(pconnect->cmd.cbsize == sizeof(comm_link_id_t));
		memcpy(&linkid, pconnect->recv_buffer, sizeof(comm_link_id_t));

		return ppcs_data_handle_control(linkid.connid, linkid.dataid, CONT_REC_GETFRAME, NULL);
	}

	return P2P_ERR;
}

int qdata_audio_back(ppcs_p2p_connect *pconnect)
{
	ppcs_p2p_recv *precv;
	
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		comm_link_info_t comm;
		assert(pconnect->cmd.cbsize == sizeof(comm_link_info_t));

		memcpy(&comm, pconnect->recv_buffer, sizeof(comm_link_info_t));
		if (pconnect->connid == comm.linkid.connid)
		{
			printf("qdata_audio_back. ch:%d. bit:%d. rate:%d. profile:%d. \n", comm.audioch.src.audiochs,
				comm.audioch.src.samplebits,comm.audioch.src.samplerate,comm.audioch.enc.profile);
			Q3_Audio_Info info =  {
					.wFormatTag = QMAPI_PT_G711A,
					.channel = 1,
					.bitwidth = 16,
					.sample_rate = 8000,
					.volume = 256,
				};
			
			int audioHandle = QMAPI_Audio_Decoder_Init(&info);
			precv = malloc(sizeof(ppcs_p2p_recv));
			if (precv)
			{
				memset(precv, 0, sizeof(ppcs_p2p_recv));
				precv->fd          = pconnect->p2p_handle;
				precv->connid      = pconnect->connid;
				precv->recvstate   = RECV_STATE_ACTIVE;
				precv->audioHandle = audioHandle;
				ppcs_recv_add(precv);
				SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
			}
			else
			{
				SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_MEMORY);
			}
		}
	}
	else if (pconnect->cmd.cmdopt == OPERATE_STOP)
	{
		comm_link_id_t linkid;

		assert(pconnect->cmd.cbsize == sizeof(comm_link_id_t));
		memcpy(&linkid, pconnect->recv_buffer, sizeof(comm_link_id_t));
		
		precv = ppcs_p2p_recv_head;
		while (precv)
		{
			if (precv->connid == pconnect->connid)
			{
				ppcs_recv_del(precv);
				if (precv->precvbuf)
				{
					free(precv->precvbuf);
					precv->precvbuf = NULL;
				}
				if (precv->audioHandle)
				{
					QMAPI_Audio_Decoder_UnInit(precv->audioHandle);
					precv->audioHandle = 0;
				}
				free(precv);
				break;
			}
			precv = precv->pnext;
		}
		SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
	}
	else
	{
		SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	}
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_OK;
}


int qdata_voice_com(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		comm_link_info_t comm;
		assert(pconnect->cmd.cbsize == sizeof(comm_link_info_t));

		memcpy(&comm, pconnect->recv_buffer, sizeof(comm_link_info_t));
		if (pconnect->connid == comm.linkid.connid)
		{
			SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
		}
		else
		{
			SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
		}
	}
	else
	{
		SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_CMD);
	}
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_OK;
}

int rec_info_search(ppcs_p2p_connect *pconnect)
{
	int ret = P2P_ERR;
	
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		QMAPI_NET_FILECOND find;
		struct {
			int type;
			comm_duration_t dur;
		} recsearch;
		
		assert(pconnect->cmd.cbsize == sizeof(recsearch));
		memcpy(&recsearch, pconnect->recv_buffer, sizeof(recsearch));
		
		find.dwChannel = pconnect->cmd.cmdch;
		find.dwFileType = QMAPI_NET_RECORD_TYPE_SCHED;
		find.dwFileType |= QMAPI_NET_RECORD_TYPE_MOTION;
		find.dwFileType |= QMAPI_NET_RECORD_TYPE_ALARM;
		find.dwFileType |= QMAPI_NET_RECORD_TYPE_CMD;
		find.dwFileType = QMAPI_NET_RECORD_TYPE_MANU;
		duration_format_convert(&find.stStartTime, &recsearch.dur.beginning, 1);
		duration_format_convert(&find.stStopTime, &recsearch.dur.ending, 1);
		
		pconnect->files_info_handle = QMAPI_Record_FindFile(pconnect->cmd.cmdch, &find);
		if (pconnect->files_info_handle > 0)
		{
			SET_CMDSZRET(pconnect->cmd, 0, ERR_NO_ERROR);
			ret = P2P_OK;
		}
		else
		{
			SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
		}
	}
	else if (pconnect->cmd.cmdopt == OPERATE_STOP)
	{
		if (pconnect->files_info_handle > 0)
		{
			QMAPI_Record_FindClose(pconnect->cmd.cmdch, pconnect->files_info_handle);
			pconnect->files_info_handle = -1;
		}
		ret = P2P_OK;
	}
	else
	{
		SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	}
	
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return ret;
}


int rec_info_read(ppcs_p2p_connect *pconnect)
{
	if (pconnect->cmd.cmdopt == OPERATE_START)
	{
		int num; /* */

		assert(pconnect->cmd.cbsize == sizeof(int));
		memcpy(&num, pconnect->recv_buffer, sizeof(int));

		if (num>0 && pconnect->files_info_handle>0)
		{
			int i=0, sz, ret;
			QMAPI_NET_FINDDATA find;
			comm_rec_info_t rec[num];

			while (i < num)
			{
				ret = QMAPI_Record_FindNextFile(pconnect->cmd.cmdch, pconnect->files_info_handle, &find);
				if (ret == QMAPI_SEARCH_FILE_SUCCESS) /* 提取成功 */
				{
					rec[i].samplelen                           = find.dwFileSize;
					rec[i].recdur.ch                           = find.nChannel;
					rec[i].recdur.type                         = REC_TYPE_NORMAL;
					duration_format_convert(&find.stStartTime, &rec[i].recdur.dur.beginning, 0);
					duration_format_convert(&find.stStopTime, &rec[i].recdur.dur.ending, 0);
					i++;
				}
				else if (ret == QMAPI_SEARCH_NOMOREFILE) /* 所有的文件已经提取完 */
				{
					break;
				}
				else /* 出错 */
				{
					i = 0;
					break;
				}
			}
			if (i)
			{
				sz = i*sizeof(comm_rec_info_t);
				SET_CMDSZRET(pconnect->cmd, sz, ERR_INVALID_PARAM);
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
				{
					DEBUG("send fail\n");
				}
				if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&rec, sz) < 0)
				{
					DEBUG("send fail\n");
				}
				return P2P_OK;
			}
		}
	}
	SET_CMDSZRET(pconnect->cmd, 0, ERR_INVALID_PARAM);
	if (PPCS_Write(pconnect->p2p_handle, 0, (CHAR_8 *)&pconnect->cmd, sizeof(cmd_header_t)) < 0)
	{
		DEBUG("send fail\n");
	}
	return P2P_ERR;
}


void ppcs_p2p_msg_process(ppcs_p2p_connect *pconnect)
{
	int tmp,len,need_len;
	char* buffer = NULL;

	if (pconnect->recv_status == RECV_STATUS_NEED_CMD)
	{
		len = sizeof(cmd_header_t) - pconnect->recv_len;
		buffer = (char*)&pconnect->cmd + pconnect->recv_len;
	}
	else if (pconnect->recv_status == RECV_STATUS_NEED_FULL_CMD)
	{
		len = pconnect->cmd.cbsize - pconnect->recv_len;
		buffer = pconnect->recv_buffer + pconnect->recv_len;
	}
	else
	{
		DEBUG("invalid , recv_status = %d\n", pconnect->recv_status);
	}

	need_len = len;
	tmp = PPCS_Read(pconnect->p2p_handle, 0, buffer, (int *)&len, 2 * 1000);
	if (tmp != ERROR_PPCS_SUCCESSFUL)
	{
		if (tmp == ERROR_PPCS_TIME_OUT)
		{

		}
		pconnect->connstate &= ~CONN_STATE_ACTIVE;
		DEBUG(" PPCS_Read error; %d.\n",tmp);
		return ;
	}
	else 
	{
		if (len < need_len) //wait for more data
		{
			pconnect->recv_len += len;
			DEBUG(" need read len:%d. need_len:%d \n",len,need_len);
			return;
		}
	}

	//需要的命令数据都已接收到，检查合法性并处理
	if(pconnect->recv_status == RECV_STATUS_NEED_CMD)
	{
		if (pconnect->cmd.xorsum != CMD_XORSUM(pconnect->cmd) ||
			(pconnect->cmd.cbsize < 0) ||
			(pconnect->cmd.cbsize > CONN_RECV_BUFSZ))
		{
			/* 非法命令或数据 */
			DEBUG("invalid msg or len, cbsize = %d\n", pconnect->cmd.cbsize);
			pconnect->recv_status = RECV_STATUS_NEED_CMD;
			pconnect->recv_len = 0;
			return;
		}
		if (pconnect->cmd.cbsize > 0)
		{
			pconnect->recv_status = RECV_STATUS_NEED_FULL_CMD;
			pconnect->recv_len = 0;
			return;
		}
	}
	if (pconnect->cmd.cmdtag != SYS_KEEP_ALIVE)
		DEBUG("fd:%d. ch:%u. tag:%x-%d. seq:%u. size:%u.\n",
			pconnect->p2p_handle, pconnect->cmd.cmdch,CMDTAGMODULE(pconnect->cmd.cmdtag)
			,(int)(pconnect->cmd.cmdtag & 0x00FF), pconnect->cmd.cmdseq,pconnect->cmd.cbsize);
	if (pconnect->cmd.cmdtag == SYS_USER_LOGIN)
	{
		/* 用户登录命令 */
		conn_user_login(pconnect);
	}
	else if (pconnect->connstate & CONN_STATE_LOGIN)
	{
		pconnect->countdown  = GEN_SEC_COUNT(90); /* 保活计数开始 */

		switch(pconnect->cmd.cmdtag)
		{
			case SYS_USER_LOGOUT:
				break;
			case SYS_KEEP_ALIVE:
				keep_alive_control(pconnect);
				break;
			case SYS_DEVICE_INFO:
				sys_device_info(pconnect);
				break;
			case SYS_LOG_SEARCH:
				break;
			case SYS_LOG_READ:
				break;
			case JPEG_PICTURE_CAPTURE:
				video_capture_jpeg(pconnect);
				break;
			case NAME_OSD_CONFIG:
				video_osd_name(pconnect);
				break;
			case AUDIO_ENCODE_CONFIG:
				break;
			case VIDEO_ENCODE_RANGE:
				video_encode_range(pconnect);
				break;
			case VIDEO_ENCODE_CONFIG:
				video_encode_cfg(pconnect);
				break;
			case AVC_PTZ_CONFIG:
				break;
			case AVC_PTZ_CONTROL:
				break;
			case QDATA_REAL_PLAY:
				qdata_real_play(pconnect);
				break;
			case QDATA_REC_PLAY:
				qdata_rec_play(pconnect);
				break;
			case QDATA_PLAY_CONTROL:
				qdata_play_control(pconnect);
				break;
			case QDATA_REC_TSSYNC:
				qdata_rec_play(pconnect);
				break;
			case QDATA_REC_GET_FRAME:
				qdata_rec_get_frame(pconnect);
				break;
			case QDATA_VOICE_COM:
				qdata_voice_com(pconnect);
				break;
			case REC_INFO_SEARCH:
				rec_info_search(pconnect);
				break;
			case REC_INFO_READ:
				rec_info_read(pconnect);
				break;
			case REC_MONTH_INFO:
				break;
			case NET_GLOBAL_CONFIG:
				break;
			case AUDIO_BACK_CHANNEL:
				qdata_audio_back(pconnect);
				break;
			default:
				break;
		}
	}

	//本次不再判断recv_status，可更新为下次命令接收做准备
	pconnect->recv_status = RECV_STATUS_NEED_CMD;
	pconnect->recv_len = 0;
	
}

/*
	检查连接的活动性,对于不是活动状态的连接要回收
*/
static void connect_active_check(void)
{
	int flag = 0;
	ppcs_p2p_send *psend;
	ppcs_p2p_recv *precv;
	
	ppcs_p2p_connect *pfront   = NULL;
	ppcs_p2p_connect *pconnect = ppcs_p2p_connect_head;
	while (pconnect) {
		if ((pconnect->connstate & CONN_STATE_ACTIVE) == 0)
		{
			DEBUG(" fd:%d. \n",pconnect->p2p_handle);
			if (pfront)
			{
				pfront->pnext = pconnect->pnext;
			}
			else
			{
				ppcs_p2p_connect_head = pconnect->pnext;
			}
			do {/* 连接处于非活动状态时，需要对连接上的数据连接全部移除 */
				flag = 0;
				psend = ppcs_p2p_send_head;
				while (psend)
				{
					if (psend->connid == pconnect->connid)
					{
						ppcs_send_del(psend);
						QMapi_buf_del_reader( 0, psend->StreamReader);
						free(psend);
						flag = 1;
						break;
					}
					psend = psend->pnext;
				}
				precv = ppcs_p2p_recv_head;
				while (precv)
				{
					if (precv->connid == pconnect->connid)
					{
						ppcs_recv_del(precv);
						if (precv->precvbuf)
						{
							free(precv->precvbuf);
							precv->precvbuf = NULL;
						}
						if (precv->audioHandle)
						{
							QMAPI_Audio_Decoder_UnInit(precv->audioHandle);
							precv->audioHandle = 0;
						}
						free(precv);
						flag = 1;
						break;
					}
					precv = precv->pnext;
				}
			}while(flag);
			PPCS_Close(pconnect->p2p_handle);
			free(pconnect);
			pconnect = pfront ? pfront->pnext : ppcs_p2p_connect_head;
			continue;
		}
		pfront   = pconnect;
		pconnect = pconnect->pnext;
	}
}

/*
	P2P 消息线程
		1. 处理消息
*/
void ppcs_p2p_msg(void* arg)
{
	int r;
	unsigned int rsize;
	ppcs_p2p_connect *pconnect;
	prctl(PR_SET_NAME, (unsigned long)__FUNCTION__);
	pthread_detach(pthread_self()); /* 线程结束时自动清除 */

	DEBUG(" create ok.\n");

	while (ppcs_p2p_run)
	{
		if (ppcs_p2p_connect_add)
		{
			ppcs_p2p_connect_add->pnext = ppcs_p2p_connect_head;
			ppcs_p2p_connect_head = ppcs_p2p_connect_add;
			ppcs_p2p_connect_add = NULL;
		}

		pconnect = ppcs_p2p_connect_head;
		while (pconnect)
		{
			if (pconnect->connstate & CONN_STATE_ACTIVE)
			{
				rsize = 0;
				r     = PPCS_Check_Buffer(pconnect->p2p_handle, 0, NULL , &rsize);
				if (r == ERROR_PPCS_SUCCESSFUL)
				{
					if (rsize > 0)
					{
						ppcs_p2p_msg_process(pconnect);
					}
					
					if ((pconnect->connstate & CONN_DATA_CONNECT) == 0)
					{	
						rsize = 0;
						if (PPCS_Check_Buffer(pconnect->p2p_handle, 1, NULL , &rsize)==ERROR_PPCS_SUCCESSFUL && rsize > 0)
						{
							r = ppcs_data_authenticate(pconnect);
							if (r == ERROR_PPCS_SUCCESSFUL) /*成功*/
							{
								DEBUG("..ppcs_data_authenticate..fd:%d.\n",pconnect->p2p_handle);
							}
							else if (r == ERROR_PPCS_TIME_OUT)
							{/*容忍RECV_TIMEOUT_COUNT 次超时*/

							}
							else
							{/*出错*/

							}
						}
					}
				}
				else
				{
					DEBUG("handle:%d. connid :%u. r:%d. \n",pconnect->p2p_handle,pconnect->connid, r);
					pconnect->connstate &= ~CONN_STATE_ACTIVE;
				}
			}
			pconnect = pconnect->pnext;
		}
		connect_active_check();
		usleep(100);
	}
	ppcs_p2p_pthread_count--;

	return ;
}


/*
	P2P 服务线程
		1. 启动P2P
		2. 监听
*/
void ppcs_p2p_server(void* arg)
{
	int ret;
	int SessionHandle, p2pconfigchange = 0;
	ppcs_p2p_info info;
	st_PPCS_NetInfo NetInfo;
	prctl(PR_SET_NAME, (unsigned long)__FUNCTION__);
	pthread_detach(pthread_self()); /* 线程结束时自动清除 */

	DEBUG(" create ok.\n");
	get_p2p_info(&info);

	if (strlen(info.p2pconfdid) == 0)/* 未获取到ID，*/
	{
		struct hostent *pHostInfo;
		QMAPI_NET_NETWORK_CFG ninfo;
		unsigned int encrypSeq;
		
		QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_NETCFG, 0, &ninfo, sizeof(QMAPI_NET_NETWORK_CFG));

		encrypSeq = (unsigned int)ninfo.stEtherNet[0].byMACAddr[0];
		encrypSeq |= ((unsigned int)ninfo.stEtherNet[0].byMACAddr[1]) << 8;
		encrypSeq |= ((unsigned int)ninfo.stEtherNet[0].byMACAddr[2]) << 16;
		encrypSeq |= ((unsigned int)ninfo.stEtherNet[0].byMACAddr[3]) << 24;
		printf("ppcs_p2p_server. encrypSeq:%x. \n",encrypSeq);
		while (1)
		{
			if (vg_conn_authz_server(info.authzip, 1, encrypSeq, info.crckey, info.p2pconfdid, info.p2pconfapil) != VG_AUTHZ_ERROR_NO_ERROR)
			{
                /* RM#2291: used sys_fun_gethostbyname, henry.li    2017/03/01*/
				//pHostInfo = gethostbyname(info.authzhost);
				pHostInfo = sys_fun_gethostbyname(info.authzhost);
				if (pHostInfo == NULL)
				{
					printf("vg_server_update_thread host name parse error\n");
					continue;
				}
				if (vg_conn_authz_server(inet_ntoa(*((struct in_addr *)pHostInfo->h_addr)), 1, encrypSeq, info.crckey, info.p2pconfdid, info.p2pconfapil) == VG_AUTHZ_ERROR_NO_ERROR)
				{
					break;
				}
			}
			else
			{
				break;
			}
			sleep(5);
		}
		p2pconfigchange = 1;
	}
	ret = PPCS_Initialize(info.initstring);
	if (ret != ERROR_PPCS_SUCCESSFUL && ret != ERROR_PPCS_ALREADY_INITIALIZED)
	{
		DEBUG("ppcs_server_thread PPCS_Initialize error, ret = %d\n", ret);
		goto error_end;
	}
	DEBUG(" ===.\n");
	while (1)
	{
		if (!ppcs_p2p_run)
			goto error_end;
		ret = PPCS_NetworkDetect(&NetInfo, 0);
		if (ret != ERROR_PPCS_SUCCESSFUL || !(NetInfo.bFlagInternet == 1 
			&& NetInfo.bFlagHostResolved == 1 && NetInfo.bFlagServerHello == 1))
		{
			DEBUG(" ==2==.\n");
			sleep(1);
		}
		else
		{
			if (p2pconfigchange)
			{
				update_p2p_info(&info);
				p2pconfigchange = 1;
			}
			printf("------------------- NetInfo: -----------------------\n");
			printf("P2P Server DID         : %s\n", info.p2pconfdid);
			printf("Internet Reachable	   : %s\n", (NetInfo.bFlagInternet == 1) ? "YES":"NO");
			printf("P2P Server IP resolved : %s\n", (NetInfo.bFlagHostResolved == 1) ? "YES":"NO");
			printf("P2P Server Hello Ack   : %s\n", (NetInfo.bFlagServerHello == 1) ? "YES":"NO");
			printf("Local NAT Type		   :");
			switch(NetInfo.NAT_Type)
			{
				case 0:
					printf(" Unknow\n");
					break;
				case 1:
					printf(" IP-Restricted Cone\n");
					break;
				case 2:
					printf(" Port-Restricted Cone\n");
					break;
				case 3:
					printf(" Symmetric\n");
					break;
			}
			printf("My Wan IP : %s\n", NetInfo.MyWanIP);
			printf("My Lan IP : %s\n", NetInfo.MyLanIP);
			break;
		}
	}
	ret = PPCS_Share_Bandwidth(1);
	if (ret != ERROR_PPCS_SUCCESSFUL)
	{
		printf("ppcs_server_thread PPCS_Share_Bandwidth error ret = %d\n", ret);
		PPCS_DeInitialize();
		goto error_end;
	}
	while (ppcs_p2p_run)
	{
		SessionHandle = PPCS_Listen(info.p2pconfdid, 300, 0, 1, info.p2pconfapil);
		printf("PPCS_Listen() SessionHandle = %d\n", SessionHandle);
		if (SessionHandle < 0)
		{/*出错处理*/
			sleep(2);
		}
		else
		{
			struct _PacketData{ 
				NPC_S_PVM_DP_UMSP_MSG_HEAD							tHead; 
				NPC_S_PVM_DP_UMSP_MSG_BODY_P1_VER_CONSULT_GET_TOKEN	tBody; 
				} tPacketData;
			int len = sizeof(tPacketData);
			memset(&tPacketData, 0, len);
			do {
				/*收取检查包，有一分钟等待时间*/
				DEBUG("PPCS_Read fd:%d. start.\n",SessionHandle);
				if (PPCS_Read(SessionHandle, 0, (CHAR *)&tPacketData, &len, 60 * 1000) == ERROR_PPCS_SUCCESSFUL && len == sizeof(tPacketData)) 
				{ 
					if (tPacketData.tHead.usMsgFuncId == NPC_D_PVM_DP_UMSP_FUNCID_P1_VER_CONSULT_GET_TOKEN 
						&& tPacketData.tHead.ucPacketType == NPC_D_PVM_DP_UMSP_PACKET_TYPE_REQUEST) 
					{
						tPacketData.tHead.ucPacketType = NPC_D_PVM_DP_UMSP_PACKET_TYPE_RESPONSION; 
						memset(&tPacketData.tBody, 0xff, sizeof(tPacketData.tBody));
						if (PPCS_Write(SessionHandle, 0, (CHAR_8 *)&tPacketData, len) == len && ppcs_connect_handle_add(SessionHandle)==P2P_OK)
						{
							DEBUG("======%d====ok====\n",SessionHandle);
							break;
						}
					}
				}
				DEBUG("======%d====error====\n",SessionHandle);
				PPCS_Close(SessionHandle);
				sleep(1);
			}while (0);
		}
	}
	
error_end:
	ppcs_p2p_pthread_count--;
	return ;	
}

void discovery_prefix(char *dest, char *type, char *src)
{
	char s[4] = ":";
	char ss[4] = "\r\n";
	
	strncpy(dest, type, strlen(type));
	dest += strlen(type);
	strncpy(dest, s, strlen(s));
	dest += strlen(s);
	strncpy(dest, src, strlen(src));
	dest += strlen(src);
	strncpy(dest, ss, strlen(ss));
}

int discovery_response(char *buf, int buf_len)
{
	char s[32];
	ppcs_p2p_info info;
	QMAPI_NET_DEVICE_INFO dev;

	memset(&dev, 0, sizeof(QMAPI_NET_DEVICE_INFO));
	if (get_dev_info(&dev) == P2P_OK && get_p2p_info(&info) == P2P_OK)
	{
		/* 设备序号 */
		discovery_prefix(buf, DS_SERIAL, (char *)dev.csSerialNumber);
		
		/* 设备名 */
		discovery_prefix(buf+strlen(buf), DS_NAME, dev.csDeviceName);
		
		/* 硬件版本号 */
		sprintf(s, "%u.%u", (unsigned short)(dev.dwHardwareVersion>>16), (unsigned short)(dev.dwHardwareVersion&0xFFFF));
		discovery_prefix(buf+strlen(buf), DS_HARDWARE, s);

		/* 软件版本号 */
		sprintf(s, "%x.%x", (unsigned short)(dev.dwSoftwareVersion>>16), (unsigned short)(dev.dwSoftwareVersion&0xFFFF));
		discovery_prefix(buf+strlen(buf), DS_SOFTWARE, s);

		/* 通道数 */
		sprintf(s, "%u", dev.byVideoInNum);
		discovery_prefix(buf+strlen(buf), DS_CHANNELS, s);

		/* P2P ID */
		discovery_prefix(buf+strlen(buf), DS_P2PID, info.p2pconfdid);

		return P2P_OK;
	}
	return P2P_ERR;
}

void discovery_listen(void)
{
	char buf[DS_BUFLEN];
	fd_set readfd;
	int sock,len,ret;
	struct timeval timeout;
	struct sockaddr_in local_addr;		//本地地址
	struct sockaddr_in from_addr;		//客户端地址

	//socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (0 > sock)
	{
		perror("client socket err");
		return ;
	}

	len = sizeof(struct sockaddr_in);
	memset(&local_addr, 0, sizeof(struct sockaddr_in));
	local_addr.sin_family = AF_INET;					/*协议族*/
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY); 	/*本地地址*/
	local_addr.sin_port = htons(DS_IPPORT);				/*监听端口*/

	//bind
	if (bind(sock, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("server1 bind err");
		goto _out;
	}
	while (ppcs_p2p_run)
	{
		FD_ZERO(&readfd);
		FD_SET(sock, &readfd);

		timeout.tv_sec = 1;	/*超时时间*/
		timeout.tv_usec = 0;	
		ret = select(sock+1, &readfd, NULL, NULL, &timeout);
		switch (ret)
		{
			case -1:
				DEBUG("select error!\n");
				goto _out;
			case 0:
				break;
			default:
			{
				//sock套接口有数据可读
				if( FD_ISSET(sock, &readfd) )
				{
					//recieve
					ret = recvfrom(sock, buf, DS_BUFLEN, 0, (struct sockaddr *)&from_addr, (socklen_t *)&len);
					if (0 > ret)
					{
						perror("server recieve err");
						goto _out;
					}

					if (strstr(buf, DS_TYPES))/* 广播 数据包，可以做加密处理*/
					{
						memset(buf, 0, DS_BUFLEN);
						if (discovery_response(buf, DS_BUFLEN) == P2P_OK)
						{
							ret = sendto(sock, buf, DS_BUFLEN, 0, (struct sockaddr *)&from_addr, len);
							if (0 > ret)
							{
								perror("server send err");
								goto _out;
							}
						}
					}
				
				}

			}
		}
	}
	
_out:
	close(sock);

	return ;
}
/*
	P2P 局域网扫描
		1. UDP 广播包 实现方式，反馈设备信息，云ID 等
*/
void ppcs_p2p_discovery(void)
{
	prctl(PR_SET_NAME, (unsigned long)__FUNCTION__);
	pthread_detach(pthread_self()); /* 线程结束时自动清除 */

	DEBUG(" create ok.\n");
	while (ppcs_p2p_run)
	{
		discovery_listen();
		usleep(10);
	}
	ppcs_p2p_pthread_count--;

	return ;
}


int ppcs_p2p_init(void)
{
	pthread_t thd;
	ppcs_p2p_info info;
	
	DEBUG("===start===.\n");

	if (get_p2p_info(&info) == P2P_ERR || info.p2penable==0)
	{
		DEBUG("no /config/ppcs_p2p_conf.txt, or disable \n");
		return -2;
	}
	
	ppcs_p2p_run = 1;
	ppcs_p2p_pthread_count = 0;
	if (pthread_create(&thd, NULL, (void *)ppcs_p2p_discovery, NULL))
	{
		DEBUG("discovery create error.\n");
		return -1;
	}
	ppcs_p2p_pthread_count++;

	if (pthread_create(&thd, NULL, (void *)ppcs_p2p_server, NULL))
	{
		DEBUG("server create error.\n");
		return -1;
	}
	ppcs_p2p_pthread_count++;
	
	if (pthread_create(&thd, NULL, (void *)ppcs_p2p_msg, NULL))
	{
		DEBUG("msg create error.\n");
		return -1;
	}
	ppcs_p2p_pthread_count++;
	
	if (pthread_create(&thd, NULL, (void *)ppcs_p2p_send_data, NULL))
	{
		DEBUG("send create error.\n");
		return -1;
	}
	ppcs_p2p_pthread_count++;

	if (pthread_create(&thd, NULL, (void *)ppcs_p2p_recv_data, NULL))
	{
		DEBUG("send create error.\n");
		return -1;
	}
	ppcs_p2p_pthread_count++;
	DEBUG("===end===.\n");

	return 0;
}


int ppcs_p2p_exit(void)
{
	ppcs_p2p_run = 0;
	while (ppcs_p2p_pthread_count) usleep(10);

	return 0;
}


