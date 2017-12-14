

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "hi_type.h"
#include "hi_mbuf.h"
#include "hi_live_sendbuf_ext.h"
#include "hi_live_sendbuf_define.h"
#include "hi_buffer_abstract_define.h"
#include "hi_buffer_abstract_ext.h"
#include "QMAPI.h"
#include "QMAPIErrno.h"

#include "QMAPIAV.h"
#include "QMAPIRecord.h"

#ifdef SUPPORT_STUN
#include "libstun.h"
#endif

typedef struct tagPLAYBACKFILE_INFO
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
}PLAYBACKFILE_INFO;


typedef struct
{
    unsigned char *pFrameBuffer;
    //unsigned int frameBufferLen;
    MBUF_VIDEO_STATUS_E enVideoState;
    HI_BOOL audioStat;
    HI_BOOL dataStat;
    HI_S32 s32Chn;
    int rtspReader;
    HI_BOOL bFirstRead;
    //回放所需参数
    HI_BOOL bPlayback;
    unsigned long totaltime;
    unsigned long firststamp;
    HI_BOOL bSeeking;  //是否刚刚执行过定位操作
    //上一次读取的帧数据中是否有音频数据-兼容音视频混在一起存放
    QMAPI_NET_FRAME_HEADER stHeader;
}CBufferReadMng;
typedef struct
{
    int maxUserNum;
    int maxFrameLen;
    CBufferReadMng **ppBufReadMng;
}CBufferAbstrac;
//#define RTSP_HANDLE 0x99
#define RTSP_HANDLE 0

static CBufferAbstrac *g_pBufferAbstrac = NULL;

static unsigned char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char g_stunserver[32]={0};
static unsigned short g_stunport = 0;

static int rtsp_base64encode(unsigned char *input, int input_length, unsigned char *output, int output_length)
{
	int	i = 0, j = 0;
	int	pad;

	//assert(output_length >= (input_length * 4 / 3));

	while (i < input_length) 
    {
		pad = 3 - (input_length - i);
		if (pad == 2) 
        {
			output[j  ] = basis_64[input[i]>>2];
			output[j+1] = basis_64[(input[i] & 0x03) << 4];
			output[j+2] = '=';
			output[j+3] = '=';
		} 
        else if (pad == 1) 
	    {
			output[j  ] = basis_64[input[i]>>2];
			output[j+1] = basis_64[((input[i] & 0x03) << 4) | ((input[i+1] & 0xf0) >> 4)];
			output[j+2] = basis_64[(input[i+1] & 0x0f) << 2];
			output[j+3] = '=';
		} 
        else
        {
			output[j  ] = basis_64[input[i]>>2];
			output[j+1] = basis_64[((input[i] & 0x03) << 4) | ((input[i+1] & 0xf0) >> 4)];
			output[j+2] = basis_64[((input[i+1] & 0x0f) << 2) | ((input[i+2] & 0xc0) >> 6)];
			output[j+3] = basis_64[input[i+2] & 0x3f];
		}
		i += 3;
		j += 4;
	}
	return j;
}
/*
这个参数用于指示 H.264 流的 profile 类型和级别. 由 Base16(十六进制) 
表示的 3 个字节. 第一个字节表示 H.264 的 Profile 类型, 第
三个字节表示 H.264 的 Profile 级别
*/
static void rtsp_sprintf_hexa(char *s, unsigned char *p_data, int i_data)
{
    static const char hex[32] = "0123456789abcdef";
    int i;

    for( i = 0; i < i_data; i++ )
    {
        s[2*i+0] = hex[(p_data[i]>>4)&0xf];
        s[2*i+1] = hex[(p_data[i]   )&0xf];
    }

    s[2*i_data] = '\0';
}


HI_S32 HI_BufAbstract_Init(int enLiveMode,HI_VOID* pBufInfo)
{
    HI_S32 s32Ret = 0;
	if(enLiveMode == LIVE_MODE_1CHN1USER)    
    {
        LIVE_1CHN1USER_BUFINFO_S* pstruBufInfo = (LIVE_1CHN1USER_BUFINFO_S*)pBufInfo;
        int i = 0;
        if(NULL != g_pBufferAbstrac)
        {
            HI_BufAbstract_Deinit(enLiveMode);
        }
        g_pBufferAbstrac = (CBufferAbstrac*)malloc(sizeof(CBufferAbstrac));
        if(NULL == g_pBufferAbstrac)
        {
            return HI_FAILURE;
        }
        g_pBufferAbstrac->maxUserNum = pstruBufInfo->maxUserNum;
        g_pBufferAbstrac->maxFrameLen = pstruBufInfo->maxFrameLen;
        g_pBufferAbstrac->ppBufReadMng = (CBufferReadMng**)malloc(g_pBufferAbstrac->maxUserNum*sizeof(CBufferReadMng));
        if(NULL == g_pBufferAbstrac->ppBufReadMng)
        {
            free(g_pBufferAbstrac);
            g_pBufferAbstrac = NULL;
            return HI_FAILURE;
        }
        for(i = 0; i < g_pBufferAbstrac->maxUserNum; i++)
        {
            g_pBufferAbstrac->ppBufReadMng[i] = NULL;
        }

        printf("%s:%s(%d)-LIVE_MODE_1CHN1USER-success\n", __FILE__,__FUNCTION__, __LINE__);
        return HI_SUCCESS;
    }
    else if(enLiveMode == LIVE_MODE_1CHNnUSER)    
    {
//        LIVE_1CHNnUSER_BUFINFO_S* pstruBufInfo = (LIVE_1CHNnUSER_BUFINFO_S*)pBufInfo;
//        s32Ret = MBUF_Init(pstruBufInfo->chnnum, pstruBufInfo->pMbufCfg);
        if(s32Ret != HI_SUCCESS)
        {
            printf("init the mbuf failed\n");
            return s32Ret;
        }
        else
        {
            printf("init the mbuf success\n");
            return HI_SUCCESS;
        }
    }
    printf("not support the mode:%d\n",enLiveMode);
    return HI_FAILURE;
}    

/*MBUF销毁，负责释放内存以及清空数据*/
HI_S32 HI_BufAbstract_Deinit(int enLiveMode)
{
//    HI_S32 s32Ret = 0;
	if(enLiveMode == LIVE_MODE_1CHN1USER)    
    {
        int i = 0;
        CBufferReadMng *pTmp = NULL;
        if(NULL != g_pBufferAbstrac)
        {
            for(i = 0; i < g_pBufferAbstrac->maxUserNum; i++)
            {
                if(NULL != g_pBufferAbstrac->ppBufReadMng[i])
                {
                    pTmp = g_pBufferAbstrac->ppBufReadMng[i];
					#if 0
                    if(NULL != pTmp->pFrameBuffer)
                    {
                        free(pTmp->pFrameBuffer);
                        pTmp->pFrameBuffer = NULL;
                    }
					#endif
                    free(pTmp);
                    pTmp = NULL;
                }
            }
            free(g_pBufferAbstrac);
            g_pBufferAbstrac = NULL;
        }
        printf("%s:%s(%d)-LIVE_MODE_1CHN1USER-success\n", __FILE__,__FUNCTION__, __LINE__);
        return HI_SUCCESS;
    }
    else if(enLiveMode == LIVE_MODE_1CHNnUSER)    
    {
  
     //   MBUF_DEInit();
        
        return HI_SUCCESS;

    }
    printf("not support the mode:%d\n",enLiveMode);
    return HI_FAILURE;
}

/*
申请一块mbuf
ChanID 输入参数，MBUF通道号
pphandle 输出参数，返回指向buf handle的指针
*/
HI_S32 HI_BufAbstract_Alloc(int enLiveMode,HI_S32 ChanID, HI_VOID **pphandle)
{
    HI_S32 s32Ret = HI_SUCCESS;

	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
    	MBUF_HEADER_S *pMbufhandle;
    	pMbufhandle = (MBUF_HEADER_S *)(*pphandle);
//    	s32Ret = MBUF_Alloc(ChanID, &pMbufhandle);
        if(s32Ret == HI_SUCCESS)
        {
            *pphandle = pMbufhandle;
            return HI_SUCCESS;
        }
        else
        {
            return HI_FAILURE;
        }
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
    	HI_VOID* phandle = NULL;
        int i = 0;

        *pphandle = phandle;
        if(NULL == g_pBufferAbstrac)
        {
            return HI_FAILURE;
        }
        for(i = 0; i < g_pBufferAbstrac->maxUserNum; i++)
        {
            if(NULL == g_pBufferAbstrac->ppBufReadMng[i])
            {
                g_pBufferAbstrac->ppBufReadMng[i] = (CBufferReadMng*)malloc(sizeof(CBufferReadMng));
                if(NULL == g_pBufferAbstrac->ppBufReadMng[i])
                {
                    return HI_FAILURE;
                }
                memset(g_pBufferAbstrac->ppBufReadMng[i], 0, sizeof(CBufferReadMng));
                //g_pBufferAbstrac->ppBufReadMng[i]->frameBufferLen = g_pBufferAbstrac->maxFrameLen;
                g_pBufferAbstrac->ppBufReadMng[i]->enVideoState = VIDEO_DISABLE;
                g_pBufferAbstrac->ppBufReadMng[i]->audioStat = HI_FALSE;
                g_pBufferAbstrac->ppBufReadMng[i]->dataStat = HI_FALSE;
                g_pBufferAbstrac->ppBufReadMng[i]->s32Chn = ChanID;
                g_pBufferAbstrac->ppBufReadMng[i]->bFirstRead = HI_TRUE;
                s32Ret = QMapi_buf_add_reader(RTSP_HANDLE, ChanID/10 - 1,ChanID%10 - 1,
                    &g_pBufferAbstrac->ppBufReadMng[i]->rtspReader);
                if(s32Ret != QMAPI_SUCCESS)
                {
                    free(g_pBufferAbstrac->ppBufReadMng[i]);
                    g_pBufferAbstrac->ppBufReadMng[i] = NULL;
                    return HI_FAILURE;
                }

				g_pBufferAbstrac->ppBufReadMng[i]->pFrameBuffer = NULL;
				#if 0
                g_pBufferAbstrac->ppBufReadMng[i]->pFrameBuffer = (unsigned char*)malloc(g_pBufferAbstrac->maxFrameLen);
                if(NULL == g_pBufferAbstrac->ppBufReadMng[i]->pFrameBuffer)
                {
                    free(g_pBufferAbstrac->ppBufReadMng[i]);
                    g_pBufferAbstrac->ppBufReadMng[i] = NULL;
                    return HI_FAILURE;
                }
				#endif
				
                *pphandle = g_pBufferAbstrac->ppBufReadMng[i];
                printf("%s:%s(%d)-LIVE_MODE_1CHN1USER-success\n", __FILE__,__FUNCTION__, __LINE__);
                return HI_SUCCESS;
            }
        }
        return HI_FAILURE;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
}

/*
释放一块mbuf
pphandle 输入输出参数，指向待释放mbuf handle的指针，
                成功释放后*pphandle = NULL
*/
HI_S32 HI_BufAbstract_Free(int enLiveMode,HI_VOID **pphandle)
{

	HI_S32 s32Ret = HI_SUCCESS;
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
//    	MBUF_HEADER_S *pMbufhandle;
  //  	pMbufhandle = (MBUF_HEADER_S *)(*pphandle);
 //   	s32Ret = MBUF_Free(&pMbufhandle);
        if(s32Ret == HI_SUCCESS)
        {
            return HI_SUCCESS;
        }
        else
        {
            return HI_FAILURE;
        }
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        int i = 0;
        CBufferReadMng *pTmp = (CBufferReadMng*)*pphandle;

        if(NULL == g_pBufferAbstrac || NULL == pTmp)
        {
            return HI_FAILURE;
        }
        for(i = 0; i < g_pBufferAbstrac->maxUserNum; i++)
        {
            if(pTmp == g_pBufferAbstrac->ppBufReadMng[i])
            {
                #if 0
                if(NULL != pTmp->pFrameBuffer)
                {
                    free(pTmp->pFrameBuffer);
                    pTmp->pFrameBuffer = NULL;
                }
                #endif
				
                if(pTmp->bPlayback == HI_FALSE)
                    QMapi_buf_del_reader(RTSP_HANDLE, pTmp->rtspReader);
#if 0
                else
                    QMAPI_Record_PlayBackStop(RTSP_HANDLE,pTmp->rtspReader);
#endif
                free(pTmp);
                pTmp = g_pBufferAbstrac->ppBufReadMng[i] = NULL;
                printf("%s:%s(%d)-LIVE_MODE_1CHN1USER-success\n", __FILE__,__FUNCTION__, __LINE__);
                return HI_SUCCESS;
            }
        }
        return HI_FAILURE;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
 
}

/*MBUF 音频、视频、数据注册函数
HI_TRUE 使能，HI_FALSE 禁止*/
HI_VOID HI_BufAbstract_Register(int enLiveMode,HI_VOID *phandle, MBUF_VIDEO_STATUS_E video_status, HI_BOOL audio_enable, HI_BOOL data_enable)
{
//    HI_S32 s32Ret = HI_SUCCESS;
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
//    	MBUF_HEADER_S *pMbufhandle;
 //   	pMbufhandle = (MBUF_HEADER_S *)(phandle);
        
    //	MBUF_Register(pMbufhandle, video_status, audio_enable, data_enable);
       
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        CBufferReadMng *pTmp = (CBufferReadMng*)phandle;
        if(NULL == pTmp)
        {
            return;
        }
        pTmp->enVideoState = video_status;
        pTmp->audioStat = audio_enable;
        pTmp->dataStat = data_enable;
    }
    else
    {
        printf("invalid live mode\n");
        return;
    }
}

/*MBUF 获取音频、视频、数据注册状态函数
HI_TRUE 使能，HI_FALSE 禁止*/
HI_VOID HI_BufAbstract_GetRegister(int enLiveMode,HI_VOID *phandle, MBUF_VIDEO_STATUS_E* pvideo_status, HI_BOOL* paudio_enable, HI_BOOL* pdata_enable)
{
   // HI_S32 s32Ret;
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
//    	MBUF_HEADER_S *pMbufhandle;
  //  	pMbufhandle = (MBUF_HEADER_S *)(phandle);
       // MBUF_VIDEO_STATUS_E video_status;
        //HI_BOOL paudio_enable;
        //HI_BOOL pdata_enable;
   // 	 MBUF_GetRegister(pMbufhandle, pvideo_status, paudio_enable, pdata_enable);
       
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        CBufferReadMng *pTmp = (CBufferReadMng*)phandle;
        if(NULL == pTmp)
        {
            return;
        }
        *pvideo_status = pTmp->enVideoState;
        *paudio_enable = pTmp->audioStat;
        *pdata_enable = pTmp->dataStat; 
    }
    else
    {
        printf("invalid live mode\n");
        return;
    }
}

/*提供给媒体处理模块的接口函数，用于向连接buf写入数据
该函数会依据ChanID遍历对应的busy list*/

                   
HI_S32 HI_BufAbstract_Read(int enLiveMode,HI_VOID *phandle, HI_ADDR*paddr, HI_U32 *plen, 
            HI_U32 *ppts, MBUF_PT_E *ppayload_type,HI_U8 *pslice_type, 
            HI_U32 *pslicenum, HI_U32 *pframe_len)
{
    HI_S32 s32Ret = HI_SUCCESS;
	
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
//    	MBUF_HEADER_S *pMbufhandle;
  //  	pMbufhandle = (MBUF_HEADER_S *)(phandle);
       // MBUF_VIDEO_STATUS_E video_status;
        //HI_BOOL paudio_enable;
        //HI_BOOL pdata_enable;
   // 	s32Ret = MBUF_Read(pMbufhandle, paddr, plen, ppts, ppayload_type, pslice_type, pslicenum, pframe_len);
        if(s32Ret == HI_SUCCESS)
        {
            return HI_SUCCESS;
        }
        else
        {
            return HI_FAILURE;
        }
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        CBufferReadMng *pTmp = (CBufferReadMng*)phandle;
        QMAPI_MBUF_POS_POLICY_E enPolicy = QMAPI_MBUF_POS_CUR_READ;
        //unsigned int bufferSize = 0;
        //QMAPI_NET_FRAME_HEADER stFrameHeader;
        QMAPI_NET_DATA_PACKET stDataPacket;
        memset(&stDataPacket, 0, sizeof(stDataPacket));
        if(NULL == pTmp)
        {
            return HI_FAILURE;
        }
        if(0 < pTmp->stHeader.wAudioSize && HI_TRUE == pTmp->audioStat)
        {
            *paddr = pTmp->pFrameBuffer+pTmp->stHeader.dwVideoSize;
            *pframe_len = *plen = pTmp->stHeader.wAudioSize;
            *ppts = pTmp->stHeader.dwTimeTick;
            *pslicenum = 1;
            *ppayload_type = AUDIO_FRAME;
            SET_VIDEO_START_SLICE(*pslice_type);
            SET_VIDEO_END_SLICE(*pslice_type);
        
            memset(&pTmp->stHeader, 0, sizeof(pTmp->stHeader));
            printf("%s:%s(%d)-LIVE_MODE_1CHN1USER-read audio frame success\n", __FILE__,__FUNCTION__, __LINE__);
            return HI_SUCCESS;
        }
        int failcount = 0;
        do
        {
            if(HI_TRUE == pTmp->bFirstRead)
            {
                enPolicy = QMAPI_MBUF_POS_LAST_WRITE_nIFRAME;
            }
            //bufferSize = pTmp->frameBufferLen;
            //s32Ret = QMapi_buf_get_frame(RTSP_HANDLE,pTmp->rtspReader,enPolicy,0,
            //    (char*)pTmp->pFrameBuffer,&bufferSize,&stFrameHeader);
            s32Ret = QMapi_buf_get_readptrpos(RTSP_HANDLE, pTmp->rtspReader, enPolicy, 0, &stDataPacket);
            if(QMAPI_SUCCESS != s32Ret)
            {
                failcount++;
                usleep(1000*10);
                printf("[%s-%d]: QMapi_buf_get_readptrpos failure!, failcount=[%d]\n",__FUNCTION__,__LINE__, failcount);

                /*if(failcount ==5)
                    enPolicy = QMAPI_MBUF_POS_LAST_WRITE_nIFRAME;
                else if(failcount>=6)
                    return HI_FAILURE;*/
                return HI_FAILURE; /* RM#4011: return FAIL if can not get readptrpos.    henry.li    2017/05/23 */
            }
            else
            {
                pTmp->bFirstRead = HI_FALSE;
                enPolicy = QMAPI_MBUF_POS_CUR_READ;
            }

          //  printf("pTmp->audioStat=%d,stFrameHeader.dwVideoSize=%lu,stFrameHeader.wAudioSize=%u!\n",
            //    pTmp->audioStat,stDataPacket.stFrameHeader.dwVideoSize,stDataPacket.stFrameHeader.wAudioSize);
        }
        while(HI_FALSE == pTmp->audioStat && stDataPacket.stFrameHeader.dwVideoSize == 0 && stDataPacket.stFrameHeader.wAudioSize > 0);
        //未申请音频，但取得的帧是音频帧--丢弃掉

        pTmp->pFrameBuffer = stDataPacket.pData;
        *paddr = stDataPacket.pData;
        *pframe_len = *plen = stDataPacket.stFrameHeader.dwVideoSize;
        *ppts = stDataPacket.stFrameHeader.dwTimeTick;
        *pslicenum = 1;
        *ppayload_type = VIDEO_NORMAL_FRAME;

      //  struct timespec ts;
      //  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
      //  printf("#### sec=%ld. %ld, Tick=%u, audio:%u. video:%lu. \n", ts.tv_sec, ts.tv_nsec / 1000000, 
	  //		stDataPacket.stFrameHeader.dwTimeTick, stDataPacket.stFrameHeader.wAudioSize, stDataPacket.stFrameHeader.dwVideoSize);

        if(0 < stDataPacket.stFrameHeader.dwVideoSize 
			&& 0 == stDataPacket.stFrameHeader.byFrameType)
        {
            *ppayload_type = VIDEO_KEY_FRAME;
        }
        if(0 == stDataPacket.stFrameHeader.dwVideoSize 
			&& 0 < stDataPacket.stFrameHeader.wAudioSize)
        {
            *ppayload_type = AUDIO_FRAME;
            *pframe_len = *plen = stDataPacket.stFrameHeader.wAudioSize;
        }
        if(0 < stDataPacket.stFrameHeader.dwVideoSize 
			&& 0 < stDataPacket.stFrameHeader.wAudioSize)
        {
            memcpy(&pTmp->stHeader,&stDataPacket.stFrameHeader, sizeof(QMAPI_NET_FRAME_HEADER));
        }
        SET_VIDEO_START_SLICE(*pslice_type);
        SET_VIDEO_END_SLICE(*pslice_type);
/*
        if(*ppayload_type != AUDIO_FRAME)
        {
            printf("VideoSize=%lu,index = %lu!\n",
                stFrameHeader.dwVideoSize,stFrameHeader.dwFrameIndex);
        }*/
        return HI_SUCCESS;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
}

HI_VOID HI_BufAbstract_Write(int enMode,HI_S32 ChanID, MBUF_PT_E payload_type, 
                   const HI_ADDR addr, HI_U32 len, HI_U64 pts, 
                   HI_U8 *pslice_type, HI_U32 slicenum, HI_U32 frame_len)
{
    if(enMode == LIVE_MODE_1CHNnUSER)
    {
//    	return MBUF_Write(ChanID, payload_type, addr, len,pts,pslice_type, slicenum,frame_len);
    }
    else
    {
    }
}

HI_VOID HI_BufAbstract_Set(int enLiveMode,HI_VOID *phandle)
{
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
    }
    else
    {
        printf("invalid live mode\n");
    }
}

HI_S32 HI_BufAbstract_GetCurPts(int enLiveMode,HI_VOID *phandle,HI_U64* pPts)
{
    HI_S32 s32Ret = HI_FAILURE;
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
 //   	MBUF_HEADER_S *pMbufhandle;
 //   	pMbufhandle = (MBUF_HEADER_S *)(phandle);
 //       HI_ADDR   addr;
//		HI_U32  len;
		HI_U64  pts;
//		MBUF_PT_E   payload_type;

//	    HI_U8  slice_type = 0;
//	    HI_U32  slicenum = 0;
//	    HI_U32  frame_len = 0;
	    
	//	while( HI_SUCCESS != s32Ret )
		{
		//	s32Ret = MBUF_Read(pMbufhandle, &addr, &len,   &pts,  &payload_type,&slice_type,&slicenum,&frame_len);
		}
        *pPts = pts;
        return HI_SUCCESS;
    
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        CBufferReadMng *pTmp = (CBufferReadMng*)phandle;
        unsigned int /*dwLeaveCount = 0,*/ dwTimeTick = 0;
        if(NULL == pTmp)
        {
            return HI_FAILURE;
        }
        //s32Ret = QMAPI_Record_MBUF_GetReadPtrLeftNum(RTSP_HANDLE, pTmp->rtspReader, &dwLeaveCount, (unsigned long*)&dwTimeTick);
        if(QMAPI_SUCCESS != s32Ret)
        {
            //return;
        }
        *pPts = (HI_U64)dwTimeTick;
        return HI_SUCCESS;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
}

HI_S32 HI_BufAbstract_GetChn(int enLiveMode,HI_VOID *phandle,HI_S32* ps32Chn)
{
//    HI_S32 s32Ret;
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
    	MBUF_HEADER_S *pMbufhandle;
    	pMbufhandle = (MBUF_HEADER_S *)(phandle);
        *ps32Chn = pMbufhandle->ChanID;
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
    	CBufferReadMng *pTmp = (CBufferReadMng*)phandle;
        if(NULL == pTmp)
        {
            return HI_FAILURE;
        }
        *ps32Chn = pTmp->s32Chn;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_VOID HI_BufAbstract_RequestIFrame(HI_S32 ChanID)
{
    int nCh = 0;
    int streamType = 0;

    nCh = (ChanID/10) - 1;
    streamType = (ChanID%10) - 1;

    QMapi_sys_ioctrl(RTSP_HANDLE, QMAPI_NET_STA_IFRAME_REQUEST, nCh, &streamType, sizeof(streamType));
}

HI_S32 HI_BufAbstract_GetMediaInfo(HI_S32 ChanID, MT_MEDIAINFO_S *pstMtMediaoInfo)
{
    HI_S32 s32SrcChn = 0,s32Stream = 0,s32Ret = 0;
    QMAPI_NET_COMPRESSION_INFO *pstrStreamPara = NULL;
    QMAPI_NET_CHANNEL_PIC_INFO stChnPicInfo;
    QMAPI_NET_SPSPPS_BASE64ENCODE_DATA stVData;
    
    s32SrcChn = ChanID/10 - 1;
    s32Stream = ChanID%10 - 1;

    printf("%s(%d) Param s32Chn:%d, s32SrcChn:%d, s32Stream:%d\n", __FUNCTION__, __LINE__, ChanID, s32SrcChn, s32Stream);

    memset(&stChnPicInfo, 0, sizeof(stChnPicInfo));
    stChnPicInfo.dwSize = sizeof(stChnPicInfo);
    stChnPicInfo.dwChannel = s32SrcChn;

	printf("%s,%d s32SrcChn=%d \n",__FILE__,__LINE__,s32SrcChn);
    s32Ret = QMapi_sys_ioctrl(RTSP_HANDLE,QMAPI_SYSCFG_GET_PICCFG,s32SrcChn,&stChnPicInfo,sizeof(stChnPicInfo));
	printf("%s,%d stRecordPara width=%u, format=%lu\n",__FILE__,__LINE__,stChnPicInfo.stRecordPara.wWidth,stChnPicInfo.stRecordPara.dwStreamFormat);
	printf("%s,%d stNetPara with=%u,format=%lu\n",__FILE__,__LINE__,stChnPicInfo.stNetPara.wWidth,stChnPicInfo.stNetPara.dwStreamFormat);
	printf("%s,%d stPhonePara with=%u format=%lu\n",__FILE__,__LINE__,stChnPicInfo.stPhonePara.wWidth,stChnPicInfo.stPhonePara.dwStreamFormat);
	
    if(QMAPI_SUCCESS != s32Ret)
    {
        printf("%s(%d): QMAPI_SYSCFG_GET_PICCFG failure!\n",__FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    switch(s32Stream )
    {
    case QMAPI_MAIN_STREAM:
        {
            pstrStreamPara = &stChnPicInfo.stRecordPara;
        }
        break;	
    case QMAPI_SECOND_STREAM:
        {
            pstrStreamPara = &stChnPicInfo.stNetPara;
        }
        break;
    case QMAPI_MOBILE_STREAM:
        {
            pstrStreamPara = &stChnPicInfo.stPhonePara;
        }
        break;
    default:
        {
            printf("%s(%d) Param s32Chn:%d, s32SrcChn:%d, s32Stream:%d\n", __FUNCTION__, __LINE__, ChanID, s32SrcChn, s32Stream);
            return -1;
        }
        break;
    }
    pstMtMediaoInfo->u32VideoPicWidth = pstrStreamPara->wWidth;
    pstMtMediaoInfo->u32VideoPicHeight = pstrStreamPara->wHeight;
    pstMtMediaoInfo->u16EncodeVideo = pstrStreamPara->wEncodeVideo;
    pstMtMediaoInfo->u16EncodeAudio = pstrStreamPara->wEncodeAudio;
    
    if(pstrStreamPara->dwCompressionType == QMAPI_PT_H264_HIGHPROFILE
        || pstrStreamPara->dwCompressionType == QMAPI_PT_H264_MAINPROFILE
        || pstrStreamPara->dwCompressionType == QMAPI_PT_H264_BASELINE)
    {
        pstrStreamPara->dwCompressionType = QMAPI_PT_H264;
    }
    
    switch(pstrStreamPara->dwCompressionType)
    {
    case QMAPI_PT_H264:
        pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_H264;
        break;
    case QMAPI_PT_MJPEG:
        pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_MJPEG;
        break;
    case QMAPI_PT_MP4VIDEO:
        pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_MPEG4;
        break;
	case QMAPI_PT_H265:
		pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_H265;
		break;
    default:
        break;
    }
    printf("%s(%d) Width:%u, Height:%u, Type:%d\n", __FUNCTION__, __LINE__, 
        pstMtMediaoInfo->u32VideoPicWidth, pstMtMediaoInfo->u32VideoPicHeight, pstMtMediaoInfo->enVideoType);

    pstMtMediaoInfo->u32Bitrate = pstrStreamPara->dwBitRate/1000;
    pstMtMediaoInfo->u32Framerate = pstrStreamPara->dwFrameRate;
    pstMtMediaoInfo->u32VideoSampleRate = 9000; 


	
		
	if (QMAPI_PT_AAC == pstrStreamPara->wFormatTag )
	{
		pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_AAC;
		if (pstrStreamPara->wBitsPerSample !=0)
			pstMtMediaoInfo->u32AudioSampleRate = pstrStreamPara->wBitsPerSample;
		else
			pstMtMediaoInfo->u32AudioSampleRate = 48000;//8000 
		pstMtMediaoInfo->u32AudioChannelNum = 1;//1
	}
	else
	{
		pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_G711A;
		if (pstrStreamPara->wBitsPerSample !=0)
			pstMtMediaoInfo->u32AudioSampleRate = pstrStreamPara->wBitsPerSample;
		else
		  	pstMtMediaoInfo->u32AudioSampleRate = 8000;
   	 	pstMtMediaoInfo->u32AudioChannelNum = 1;			
	}
	
	if(stChnPicInfo.stRecordPara.wFormatTag == QMAPI_PT_ADPCMA)
    {
        pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_ADPCM;
    }
	
#if 0	
    pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_G711A;
    if(stChnPicInfo.stRecordPara.wFormatTag == QMAPI_PT_ADPCMA)
    {
        pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_ADPCM;
    }
    /*to do:获取AI属性*/


//	pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_AAC;
	/*to do:获取AI属性*/
//	pstMtMediaoInfo->u32AudioSampleRate = 44100;
//	pstMtMediaoInfo->u32AudioChannelNum = 2;

#endif

    memset(&stVData, 0, sizeof(stVData));
    stVData.dwSize = sizeof(stVData);
    stVData.chn = s32SrcChn;
    stVData.streamtype = s32Stream;
	
    if(QMAPI_PT_H264 == pstrStreamPara->dwCompressionType || QMAPI_PT_H265 == pstrStreamPara->dwCompressionType)
    {
        s32Ret = QMapi_sys_ioctrl(RTSP_HANDLE,QMAPI_NET_STA_GET_SPSPPSENCODE_DATA,s32SrcChn,&stVData,sizeof(stVData));
        if(QMAPI_SUCCESS != s32Ret)
        {
            printf("%s(%d): QMAPI_NET_STA_GET_SPSPPSENCODE_DATA failure!\n",__FUNCTION__, __LINE__);
            return HI_FAILURE;
        }

        if(stVData.SPS_PPS_BASE64_LEN<=0 || stVData.PPS_LEN<=0 || stVData.SPS_LEN<=0 || stVData.SEL_LEN<=0)
        {
            printf("%s(%d): QMAPI_NET_STA_GET_SPSPPSENCODE_DATA error!\n",__FUNCTION__, __LINE__);
            return HI_FAILURE;
        }
        
        memcpy(pstMtMediaoInfo->auVideoDataInfo,
    	stVData.SPS_PPS_BASE64, stVData.SPS_PPS_BASE64_LEN);
        pstMtMediaoInfo->u32VideoDataLen = stVData.SPS_PPS_BASE64_LEN;
        pstMtMediaoInfo->SPS_LEN = stVData.SPS_LEN;
        pstMtMediaoInfo->PPS_LEN = stVData.PPS_LEN;
        pstMtMediaoInfo->SEL_LEN = stVData.SEL_LEN;
        memcpy(pstMtMediaoInfo->profile_level_id,stVData.profile_level_id,sizeof(pstMtMediaoInfo->profile_level_id));
        printf("the chn:%d's spspps len;%d string :%s \n",s32SrcChn,pstMtMediaoInfo->u32VideoDataLen,pstMtMediaoInfo->auVideoDataInfo);
        printf("profile_level_id :%s \n",pstMtMediaoInfo->profile_level_id);
    }

    return HI_SUCCESS;
}

HI_U8 HI_BufAbstract_GetAudioEnable(HI_S32 ChanID)
{
    int nCh = 0;
    int streamType = 0;
    QMAPI_NET_CHANNEL_PIC_INFO stChnPicInfo;
    memset(&stChnPicInfo, 0, sizeof(QMAPI_NET_CHANNEL_PIC_INFO));
    stChnPicInfo.dwSize = sizeof(QMAPI_NET_CHANNEL_PIC_INFO);

    nCh = (ChanID/10) - 1;
    streamType = (ChanID%10) - 1;

    QMapi_sys_ioctrl(RTSP_HANDLE,QMAPI_SYSCFG_GET_PICCFG,nCh,&stChnPicInfo,sizeof(QMAPI_NET_CHANNEL_PIC_INFO));

    if(streamType == 0)
        return stChnPicInfo.stRecordPara.wEncodeAudio;
    else if(streamType == 1)
        return stChnPicInfo.stNetPara.wEncodeAudio;
    else if(streamType == 2)
        return stChnPicInfo.stPhonePara.wEncodeAudio;

    return 0;
}
#if 0
HI_S32 HI_BufAbstract_GetNetPt_13_Info(char *pSerialIndex)
{
	int ret;
	QMAPI_NET_PLATFORM_INFO_V2 platInfo;
	memset(&platInfo, 0, sizeof(platInfo));

	if(!pSerialIndex)
		return -1;

	ret = QMapi_sys_ioctrl(0, QMAPI_SYSCFG_GET_PLATFORMCFG, 0, &platInfo, sizeof(platInfo));
	if(ret)
	{
		return -2;
	}
	
	if(platInfo.enPlatManufacture != QMAPI_NETPT_HTZN)
		return -3;

	if(strlen(platInfo.csServerNo) == 0)
		return -4;

	strcpy(pSerialIndex, platInfo.csServerNo);
	
    return 0;
}
#endif
HI_S32 BufAbstract_Playback_Alloc(int enLiveMode, char *csFileName, HI_VOID **pphandle)
{
//    HI_S32 s32Ret = HI_SUCCESS;

    if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
        return HI_FAILURE;
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        HI_VOID* phandle = NULL;
        int i = 0;
        int filehandle = 0;
        PLAYBACKFILE_INFO *ppbfinfo;

        *pphandle = phandle;
        if(NULL == g_pBufferAbstrac)
        {
            return HI_FAILURE;
        }
        for(i = 0; i < g_pBufferAbstrac->maxUserNum; i++)
        {
            if(NULL == g_pBufferAbstrac->ppBufReadMng[i])
            {
                g_pBufferAbstrac->ppBufReadMng[i] = (CBufferReadMng*)malloc(sizeof(CBufferReadMng));
                if(NULL == g_pBufferAbstrac->ppBufReadMng[i])
                {
                    return HI_FAILURE;
                }
                memset(g_pBufferAbstrac->ppBufReadMng[i], 0, sizeof(CBufferReadMng));
                g_pBufferAbstrac->ppBufReadMng[i]->enVideoState = VIDEO_DISABLE;
                g_pBufferAbstrac->ppBufReadMng[i]->audioStat = HI_FALSE;
                g_pBufferAbstrac->ppBufReadMng[i]->dataStat = HI_FALSE;
                g_pBufferAbstrac->ppBufReadMng[i]->s32Chn = 0;
                g_pBufferAbstrac->ppBufReadMng[i]->bFirstRead = HI_TRUE;
                filehandle = QMAPI_Record_PlayBackByName(RTSP_HANDLE, csFileName);
                if(filehandle <= 0)
                {
                    free(g_pBufferAbstrac->ppBufReadMng[i]);
                    g_pBufferAbstrac->ppBufReadMng[i] = NULL;
                    return HI_FAILURE;
                }

                ppbfinfo = (PLAYBACKFILE_INFO *)filehandle;
                g_pBufferAbstrac->ppBufReadMng[i]->totaltime = ppbfinfo->dwTotalTime;
                g_pBufferAbstrac->ppBufReadMng[i]->bPlayback = HI_TRUE;
                g_pBufferAbstrac->ppBufReadMng[i]->rtspReader = filehandle;

                g_pBufferAbstrac->ppBufReadMng[i]->pFrameBuffer = NULL;
                g_pBufferAbstrac->ppBufReadMng[i]->firststamp = ppbfinfo->firstStamp;

                *pphandle = g_pBufferAbstrac->ppBufReadMng[i];
                printf("%s:%s(%d)-LIVE_MODE_1CHN1USER-success\n", __FILE__,__FUNCTION__, __LINE__);
                return HI_SUCCESS;
            }
        }
        return HI_FAILURE;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
}

HI_S32 BufAbstract_Playback_Free(int enLiveMode,HI_VOID **pphandle)
{

	HI_S32 s32Ret = HI_SUCCESS;
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
//    	MBUF_HEADER_S *pMbufhandle;
//    	pMbufhandle = (MBUF_HEADER_S *)(*pphandle);
        if(s32Ret == HI_SUCCESS)
        {
            return HI_SUCCESS;
        }
        else
        {
            return HI_FAILURE;
        }
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        int i = 0;
        CBufferReadMng *pTmp = (CBufferReadMng*)*pphandle;

        if(NULL == g_pBufferAbstrac || NULL == pTmp)
        {
            return HI_FAILURE;
        }
        for(i = 0; i < g_pBufferAbstrac->maxUserNum; i++)
        {
            if(pTmp == g_pBufferAbstrac->ppBufReadMng[i])
            {
                #if 0
                if(NULL != pTmp->pFrameBuffer)
                {
                    free(pTmp->pFrameBuffer);
                    pTmp->pFrameBuffer = NULL;
                }
                #endif
				
                QMAPI_Record_PlayBackStop(RTSP_HANDLE,pTmp->rtspReader);
                free(pTmp);
                pTmp = g_pBufferAbstrac->ppBufReadMng[i] = NULL;
                return HI_SUCCESS;
            }
        }
        return HI_FAILURE;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
 
}


HI_S32 BufAbstract_Playback_GetMediaInfo(char *csFileName, MT_MEDIAINFO_S *pstMtMediaoInfo)
{
    int fhandle/*, ret*/;
    HI_S32 s32Ret = 0;
    QMAPI_TIME stITm;
    QMAPI_NET_DATA_PACKET stReadptr;
    QMAPI_RECFILE_INFO  RecFileInfo;

    memset(&RecFileInfo, 0, sizeof(RecFileInfo));
    
    fhandle = QMAPI_Record_PlayBackByName(RTSP_HANDLE, csFileName);
    if(fhandle <= 0)
    {
        printf("%s,QMAPI_Record_PlayBackByName failed! csFileName:%s\n",__FUNCTION__, csFileName);
        return HI_FAILURE;
    }

    pstMtMediaoInfo->u32TotalTime = ((PLAYBACKFILE_INFO *)fhandle)->dwTotalTime;
    memset(&stReadptr, 0, sizeof(stReadptr));
    s32Ret = QMAPI_Record_PlayBackGetReadPtrPos(RTSP_HANDLE,fhandle,&stReadptr,&stITm);
    if(QMAPI_SUCCESS != s32Ret)
    {
        printf("%s,QMAPI_Record_PlayBackGetReadPtrPos failed! s32Ret:%d\n",__FUNCTION__, s32Ret);
        QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);
        return HI_FAILURE;
    }

    if(stReadptr.stFrameHeader.dwVideoSize == 0 
        || 0 != stReadptr.stFrameHeader.byFrameType)//非I帧
    {
        printf("%s, Got an Invalid frame!\n",__FUNCTION__);
        QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);
        return HI_FAILURE;
    }

    unsigned char *pData = stReadptr.pData;
    if(pData[0] == 0 
        && pData[1] == 0 
        && pData[2] == 0
        && pData[3] == 1)
    {
        int i;
        int flag = 4;
        int sps_base64_len, pps_base64_len/*, spspps_base64_len*/;
        unsigned char spsvalue[256] = {0}, ppsvalue[256] = {0};
        pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_H264;
        char u8NALType = pData[4]&0x1F;
        for(i=4;i<stReadptr.stFrameHeader.dwVideoSize-4;i++)
        {
            if(pData[i] == 0 
                && pData[1+i] == 0 
                && pData[2+i] == 0
                && pData[3+i] == 1)
            {
                if(0x7 == u8NALType)
                {
                    memcpy(spsvalue, pData+flag, i-flag);
                    pstMtMediaoInfo->SPS_LEN = i-flag;
                    rtsp_sprintf_hexa((char*)pstMtMediaoInfo->profile_level_id,pData+flag+1, 3);
                }
                else if(0x8 == u8NALType)
                {
                    memcpy(ppsvalue, pData+flag, i-flag);
                    pstMtMediaoInfo->PPS_LEN = i-flag;
                }
                else if(0x8 == u8NALType)
                {
                    //memcpy(ppsvalue, pData+flag, i-flag);
                    pstMtMediaoInfo->SEL_LEN = i-flag;
                }
                flag = i+4;
                u8NALType = pData[flag]&0x1F;
            }

            if(pstMtMediaoInfo->SPS_LEN>0
                && pstMtMediaoInfo->PPS_LEN>0
                && pstMtMediaoInfo->SEL_LEN>0)
                break;
        }

        if(pstMtMediaoInfo->SPS_LEN>0 && pstMtMediaoInfo->PPS_LEN>0)
        {
            char *pTmp;
            sps_base64_len = rtsp_base64encode(spsvalue, pstMtMediaoInfo->SPS_LEN, 
                (unsigned char *)(pstMtMediaoInfo->auVideoDataInfo), 256);
            pstMtMediaoInfo->auVideoDataInfo[sps_base64_len] = ',';
            pTmp = pstMtMediaoInfo->auVideoDataInfo+sps_base64_len+1;
            pps_base64_len = rtsp_base64encode(ppsvalue, pstMtMediaoInfo->PPS_LEN, 
                (unsigned char *)pTmp, sizeof(pstMtMediaoInfo->auVideoDataInfo)-sps_base64_len-1);
            pstMtMediaoInfo->u32VideoDataLen = sps_base64_len+pps_base64_len+1;
            
        }
        else
        {
            printf("%s, can't get sps and pps info!\n",__FUNCTION__);
            QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);
            return HI_FAILURE;
        }
        
    }
    else
    {
        pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_MJPEG;
    }

    s32Ret = QMAPI_Record_PlayBackGetInfo(RTSP_HANDLE, fhandle, &RecFileInfo);
    
    pstMtMediaoInfo->u32VideoPicWidth = RecFileInfo.u16Height;
    pstMtMediaoInfo->u32VideoPicHeight = RecFileInfo.u16Width;
    pstMtMediaoInfo->u16EncodeVideo = 1;
    pstMtMediaoInfo->u16EncodeAudio = RecFileInfo.u16HaveAudio;

    pstMtMediaoInfo->u32Bitrate = 0;
    pstMtMediaoInfo->u32Framerate = RecFileInfo.u16FrameRate;
    pstMtMediaoInfo->u32VideoSampleRate = 9000;
    pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_G711A;
    /*to do:获取AI属性*/
    pstMtMediaoInfo->u32AudioSampleRate = 8000;
    pstMtMediaoInfo->u32AudioChannelNum = 1;

    QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);

    return HI_SUCCESS;
}

int BufAbstract_ParseDate(char *date, unsigned long *year, unsigned long *month, unsigned long *day)
{
    int ret = 0;
    ret = sscanf(date, "%04d%02d%02d", (int *)year, (int *)month, (int *)day);
    if(ret != 3)
    {
        printf("%s, Invalid date!(%s)\n",__FUNCTION__,date);
        return HI_FAILURE;
    }
    
    if(*year<1970 || *year>2030)
    {
        printf("%s, Invalid year:%lu\n",__FUNCTION__, *year);
        return HI_FAILURE;
    }

    if(*month>12)
    {
        printf("%s, Invalid month:%lu\n",__FUNCTION__, *month);
        return HI_FAILURE;
    }

    if(*day>31)
    {
        printf("%s, Invalid day:%lu\n",__FUNCTION__, *day);
        return HI_FAILURE;
    }

    return 0;
}

int BufAbstract_ParseTime(char *time, unsigned long *hour, unsigned long *minute, unsigned long *sec)
{
    int ret = 0;
    ret = sscanf(time, "%02d%02d%02d", (int *)hour, (int *)minute, (int *)sec);
    if(ret != 3)
    {
        printf("%s, Invalid time format:%s\n",__FUNCTION__,time);
        return HI_FAILURE;
    }
    
    if(*hour>24)
    {
        printf("%s, Invalid hour:%lu\n",__FUNCTION__, *hour);
        return HI_FAILURE;
    }

    if(*minute>60)
    {
        printf("%s, Invalid minute:%lu\n",__FUNCTION__, *minute);
        return HI_FAILURE;
    }

    if(*sec>60)
    {
        printf("%s, Invalid second:%lu\n",__FUNCTION__, *sec);
        return HI_FAILURE;
    }

    return 0;
}


HI_S32 BufAbstract_PlaybackByTime_GetMediaInfo(char *csReqInfo, MT_MEDIAINFO_S *pstMtMediaoInfo, char *csRespFileName, HI_U32*start_offset)
{
    return 0;
#if 0
    int fhandle, ret;
    HI_S32 s32Ret = 0;
    QMAPI_TIME stITm;
    QMAPI_NET_DATA_PACKET stReadptr;
    QMAPI_RECFILE_INFO  RecFileInfo;

    int i;
    char *p = NULL;
    int chn = 0;
    memset(&stITm, 0, sizeof(stITm));
    if(p=strstr(csReqInfo, "date="))
    {
        ret = BufAbstract_ParseDate(p+strlen("date="), &stITm.dwYear,&stITm.dwMonth,&stITm.dwDay);
        if(ret)
        {
            printf("%s %d,Invalid date:%s\n",__FUNCTION__,__LINE__,p);
            return HI_FAILURE;
        }
        
    }
    else
    {
            printf("%s %d, Invalid request date!(%s)\n",__FUNCTION__,__LINE__,csReqInfo);
            return HI_FAILURE;
    }

    if(p=strstr(csReqInfo, "time="))
    {
        ret = BufAbstract_ParseTime(p+strlen("time="), &stITm.dwHour,&stITm.dwMinute,&stITm.dwSecond);
        if(ret)
        {
            printf("%s,Invalid time:%s\n",__FUNCTION__,p);
            return HI_FAILURE;
        }
    }
    else
    {
            printf("%s, Invalid request time!(%s)\n",__FUNCTION__,p);
            return HI_FAILURE;
    }

    if(p=strstr(csReqInfo, "chn="))
    {
        chn = atoi(p+strlen("chn="));
    }

    memset(&RecFileInfo, 0, sizeof(RecFileInfo));

    char sFileName[128] = {0};
    ret = REC_FindFileByTime(chn, stITm, sFileName, sizeof(sFileName)-1);
    if(ret)
    {
        printf("%s, The request file dosn't exist!\n",__FUNCTION__);
    }
    fhandle = QMAPI_Record_PlayBackByName(RTSP_HANDLE, sFileName);
    if(fhandle <= 0)
    {
        printf("%s,QMAPI_Record_PlayBackByName failed! csFileName:%s\n",__FUNCTION__, csReqInfo);
        return HI_FAILURE;
    }

    // /mnt/usb1/20140415/0/151000.asf
    int c;
    QMAPI_TIME tmptime;
    char buf1[16]={0},buf2[16]={0},strdate[16]={0},strtime[16]={0};

    memset(&tmptime, 0, sizeof(tmptime));
    ret = sscanf(sFileName, "/%[^/]/%[^/]/%[^/]/%d/%[^.]", buf1, buf2, strdate, &c, strtime);
    if(ret!=5)
    {
        printf("%s, can not recognize file name:%s\n",__FUNCTION__,sFileName);
        return HI_FAILURE;
    }

    ret = BufAbstract_ParseDate(strdate, &tmptime.dwYear,&tmptime.dwMonth,&tmptime.dwDay);
    if(ret)
    {
        printf("%s %d,Invalid date:%s\n",__FUNCTION__,__LINE__, strdate);
        return HI_FAILURE;
    }

    ret = BufAbstract_ParseTime(strtime, &tmptime.dwHour,&tmptime.dwMinute,&tmptime.dwSecond);
    if(ret)
    {
        printf("%s %d,Invalid time:%s\n",__FUNCTION__,__LINE__, strtime);
        return HI_FAILURE;
    }

    strncpy(csRespFileName, sFileName, 64);

    *start_offset = (stITm.dwDay-tmptime.dwDay)*24*3600
        +(stITm.dwHour-tmptime.dwHour)*3600
        +(stITm.dwMinute-tmptime.dwMinute)*60
        +(stITm.dwSecond-tmptime.dwSecond);

    printf("%s, aaaaaaaaaaaaaaaaaa,start_offset:%d\n",__FUNCTION__,*start_offset);

    pstMtMediaoInfo->u32TotalTime = ((PLAYBACKFILE_INFO *)fhandle)->dwTotalTime;
    memset(&stReadptr, 0, sizeof(stReadptr));
    s32Ret = QMAPI_Record_PlayBackGetReadPtrPos(RTSP_HANDLE,fhandle,&stReadptr,&stITm);
    if(QMAPI_SUCCESS != s32Ret)
    {
        printf("%s,QMAPI_Record_PlayBackGetReadPtrPos failed! s32Ret:%d\n",__FUNCTION__, s32Ret);
        QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);
        return HI_FAILURE;
    }

    if(stReadptr.stFrameHeader.dwVideoSize == 0 
        || 0 != stReadptr.stFrameHeader.byFrameType)//非I帧
    {
        printf("%s, Got an Invalid frame!\n",__FUNCTION__);
        QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);
        return HI_FAILURE;
    }

    unsigned char *pData = stReadptr.pData;
    if(pData[0] == 0 
        && pData[1] == 0 
        && pData[2] == 0
        && pData[3] == 1)
    {
        int flag = 4;
        int sps_base64_len, pps_base64_len, spspps_base64_len;
        unsigned char spsvalue[256] = {0}, ppsvalue[256] = {0};
        pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_H264;
        char u8NALType = pData[4]&0x1F;
        for(i=4;i<stReadptr.stFrameHeader.dwVideoSize-4;i++)
        {
            if(pData[i] == 0 
                && pData[1+i] == 0 
                && pData[2+i] == 0
                && pData[3+i] == 1)
            {
                if(0x7 == u8NALType)
                {
                    memcpy(spsvalue, pData+flag, i-flag);
                    pstMtMediaoInfo->SPS_LEN = i-flag;
                    rtsp_sprintf_hexa((char*)pstMtMediaoInfo->profile_level_id,pData+flag+1, 3);
                }
                else if(0x8 == u8NALType)
                {
                    memcpy(ppsvalue, pData+flag, i-flag);
                    pstMtMediaoInfo->PPS_LEN = i-flag;
                }
                else if(0x8 == u8NALType)
                {
                    //memcpy(ppsvalue, pData+flag, i-flag);
                    pstMtMediaoInfo->SEL_LEN = i-flag;
                }
                flag = i+4;
                u8NALType = pData[flag]&0x1F;
            }

            if(pstMtMediaoInfo->SPS_LEN>0
                && pstMtMediaoInfo->PPS_LEN>0
                && pstMtMediaoInfo->SEL_LEN>0)
                break;
        }

        if(pstMtMediaoInfo->SPS_LEN>0 && pstMtMediaoInfo->PPS_LEN>0)
        {
            char *pTmp;
            sps_base64_len = rtsp_base64encode(spsvalue, pstMtMediaoInfo->SPS_LEN, 
                pstMtMediaoInfo->auVideoDataInfo, 256);
            pstMtMediaoInfo->auVideoDataInfo[sps_base64_len] = ',';
            pTmp = pstMtMediaoInfo->auVideoDataInfo+sps_base64_len+1;
            pps_base64_len = rtsp_base64encode(ppsvalue, pstMtMediaoInfo->PPS_LEN, 
                pTmp, sizeof(pstMtMediaoInfo->auVideoDataInfo)-sps_base64_len-1);
            pstMtMediaoInfo->u32VideoDataLen = sps_base64_len+pps_base64_len+1;
            
        }
        else
        {
            printf("%s, can't get sps and pps info!\n",__FUNCTION__);
            QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);
            return HI_FAILURE;
        }
        
    }
    else
    {
        pstMtMediaoInfo->enVideoType = MT_VIDEO_FORMAT_MJPEG;
    }

    s32Ret = QMAPI_Record_PlayBackGetInfo(RTSP_HANDLE, fhandle, &RecFileInfo);
    
    pstMtMediaoInfo->u32VideoPicWidth = RecFileInfo.u16Height;
    pstMtMediaoInfo->u32VideoPicHeight = RecFileInfo.u16Width;
    pstMtMediaoInfo->u16EncodeVideo = 1;
    pstMtMediaoInfo->u16EncodeAudio = RecFileInfo.u16HaveAudio;

    pstMtMediaoInfo->u32Bitrate = 0;
    pstMtMediaoInfo->u32Framerate = RecFileInfo.u16FrameRate;
    pstMtMediaoInfo->u32VideoSampleRate = 9000;
    pstMtMediaoInfo->enAudioType = MT_AUDIO_FORMAT_G711A;
    /*to do:获取AI属性*/
    pstMtMediaoInfo->u32AudioSampleRate = 8000;
    pstMtMediaoInfo->u32AudioChannelNum = 1;

    QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);
#endif
    return HI_SUCCESS;
}

HI_S32 BufAbstract_Playback_GetAudioEnable(char *csFileName)
{
    int fhandle;
    HI_S32 s32Ret = 0;
  //  QMAPI_TIME stITm;
//    QMAPI_NET_DATA_PACKET stReadptr;
    QMAPI_RECFILE_INFO  RecFileInfo;

    memset(&RecFileInfo, 0, sizeof(RecFileInfo));
    
    fhandle = QMAPI_Record_PlayBackByName(RTSP_HANDLE, csFileName);
    if(fhandle <= 0)
    {
        printf("%s,QMAPI_Record_PlayBackByName failed! csFileName:%s\n",__FUNCTION__, csFileName);
        return 0;
    }

    s32Ret = QMAPI_Record_PlayBackGetInfo(RTSP_HANDLE, fhandle, &RecFileInfo);
    if(s32Ret)
    {
        printf("%s,QMAPI_Record_PlayBackGetInfo failed! csFileName:%s\n",__FUNCTION__, csFileName);
        return 0;
    }
    
    QMAPI_Record_PlayBackStop(RTSP_HANDLE,fhandle);

    return (HI_S32)RecFileInfo.u16HaveAudio;
}



HI_S32 BufAbstract_Playback_Read(int enLiveMode,HI_VOID *phandle, HI_ADDR*paddr, HI_U32 *plen, 
            HI_U32 *ppts, MBUF_PT_E *ppayload_type)
{
    HI_S32 s32Ret = HI_SUCCESS;
	
	if(enLiveMode == LIVE_MODE_1CHNnUSER)
    {
//    	MBUF_HEADER_S *pMbufhandle;
 //   	pMbufhandle = (MBUF_HEADER_S *)(phandle);
        if(s32Ret == HI_SUCCESS)
        {
            return HI_SUCCESS;
        }
        else
        {
            return HI_FAILURE;
        }
    }
    else if(enLiveMode == LIVE_MODE_1CHN1USER)
    {
        CBufferReadMng *pTmp = (CBufferReadMng*)phandle;
        QMAPI_NET_DATA_PACKET stDataPacket;
        memset(&stDataPacket, 0, sizeof(stDataPacket));
        if(NULL == pTmp)
        {
            return HI_FAILURE;
        }
        if(pTmp->bSeeking != HI_TRUE && 0 < pTmp->stHeader.wAudioSize)
        {
            *paddr = pTmp->pFrameBuffer+pTmp->stHeader.dwVideoSize;
            *plen = pTmp->stHeader.wAudioSize;
            *ppts = pTmp->stHeader.dwTimeTick;
            *ppayload_type = AUDIO_FRAME;
        
            memset(&pTmp->stHeader, 0, sizeof(pTmp->stHeader));
            return HI_SUCCESS;
        }

        if(pTmp->bSeeking == HI_FALSE)
        {
            QMAPI_TIME stITm;

        read_again:
            s32Ret = QMAPI_Record_PlayBackGetReadPtrPos(RTSP_HANDLE,pTmp->rtspReader,&stDataPacket,&stITm);
            if(QMAPI_SUCCESS != s32Ret)
            {
                return HI_FAILURE;
            }

            //暂时不回放音频
            if(stDataPacket.stFrameHeader.wAudioSize>0 && stDataPacket.stFrameHeader.dwVideoSize == 0)
                goto read_again;
        }
        else
        {
            stDataPacket.pData = pTmp->pFrameBuffer;
            memcpy(&stDataPacket.stFrameHeader, &pTmp->stHeader, sizeof(QMAPI_NET_FRAME_HEADER));
            pTmp->bSeeking = HI_FALSE;
            pTmp->pFrameBuffer = NULL;
            memset(&pTmp->stHeader, 0, sizeof(QMAPI_NET_FRAME_HEADER));
        }

        if(pTmp->firststamp == 0)
        {
            pTmp->firststamp = ((PLAYBACKFILE_INFO *)pTmp->rtspReader)->firstStamp;
        }

        if(pTmp->totaltime < stDataPacket.stFrameHeader.dwTimeTick)
        {
            //printf("%s, aaaaaaaaaaaaaa,p4:%02x,totaltime:%d,size:%lu,time:%lu\n",__FUNCTION__,stDataPacket.pData[4],pTmp->totaltime,stDataPacket.stFrameHeader.dwVideoSize,stDataPacket.stFrameHeader.dwTimeTick);
            return 1;
        }
        
        pTmp->pFrameBuffer = stDataPacket.pData;
        *paddr = stDataPacket.pData;
        *plen = stDataPacket.stFrameHeader.dwVideoSize;
        *ppts = stDataPacket.stFrameHeader.dwTimeTick;
        *ppayload_type = VIDEO_NORMAL_FRAME;
        if(0 < stDataPacket.stFrameHeader.dwVideoSize 
			&& 0 == stDataPacket.stFrameHeader.byFrameType)
        {
            *ppayload_type = VIDEO_KEY_FRAME;
        }
        if(0 == stDataPacket.stFrameHeader.dwVideoSize 
			&& 0 < stDataPacket.stFrameHeader.wAudioSize)
        {
            *ppayload_type = AUDIO_FRAME;
            *plen = stDataPacket.stFrameHeader.wAudioSize;
        }
        /*
        if(0 < stDataPacket.stFrameHeader.dwVideoSize 
			&& 0 < stDataPacket.stFrameHeader.wAudioSize)
        {
            memcpy(&pTmp->stHeader,&stDataPacket.stFrameHeader, sizeof(QMAPI_NET_FRAME_HEADER));
        }
        */
        
        return HI_SUCCESS;
    }
    else
    {
        printf("invalid live mode\n");
        return HI_FAILURE;
    }
}


HI_S32 BufAbstract_Playback_Seek(HI_VOID *phandle, double seektime, HI_U32 *ppts)
{
    QMAPI_TIME stITm;
    HI_S32 s32Ret = HI_SUCCESS;
    CBufferReadMng *pTmp = (CBufferReadMng*)phandle;
    QMAPI_NET_DATA_PACKET stDataPacket;
    memset(&stDataPacket, 0, sizeof(stDataPacket));

    printf("%s ,seektime:%lf\n",__FUNCTION__,seektime);
    unsigned int seekstamp = seektime*1000+pTmp->firststamp;
    s32Ret = QMAPI_Record_PlayBackControl(RTSP_HANDLE, pTmp->rtspReader, (DWORD)QMAPI_REC_PLAYSETPOS, (char*)(&seekstamp), 4, NULL, NULL);
    if(s32Ret!=QMAPI_SUCCESS)
    {
        return HI_FAILURE;
    }

    while(1)
    {
        printf("Find I frame ....\n");
        if(0 != QMAPI_Record_PlayBackGetReadPtrPos(RTSP_HANDLE, pTmp->rtspReader, &stDataPacket, &stITm))
        {
            printf("%s,############################ get pos ERR \n",__FUNCTION__);
            return HI_FAILURE;
        }
        if(stDataPacket.stFrameHeader.byFrameType==0){
            printf("Find I frame success.\n");
            break;
        }
    }

    memcpy(&pTmp->stHeader,&stDataPacket.stFrameHeader, sizeof(QMAPI_NET_FRAME_HEADER));
    pTmp->pFrameBuffer = stDataPacket.pData;
    *ppts = stDataPacket.stFrameHeader.dwTimeTick;
    pTmp->bSeeking = HI_TRUE;
    
    return HI_SUCCESS;
}

int BufAbstract_Register_StunServer(char *pServerAddr, unsigned short port)
{
    if(pServerAddr == NULL)
        return -1;

    strcpy(g_stunserver, pServerAddr);
    g_stunport = port;
    
    return 0;
}

int BufAbstract_Deregister_StunServer()
{
    g_stunserver[0] = 0;
    g_stunport = 0;
    return 0;
}

int BufAbstract_Get_NatAddr(unsigned short MediaPort, unsigned short AdaptPort, char *pNatAddr, unsigned short *pMapedPortPair, int *pSocketPair)
{
#ifdef SUPPORT_STUN
	int sockfd = -1;
    unsigned short port = 0;
    char NatAddr[32]={0};
    if(g_stunserver[0] == 0)
        return -1;

    sockfd = stun_get_nat_addr(g_stunserver, g_stunport, MediaPort, NatAddr, &port);
    if(sockfd<0)
        return -1;

    pMapedPortPair[0] = port;
    pSocketPair[0] = sockfd;
    strcpy(pNatAddr, NatAddr);
    
    if(AdaptPort>0)
    {
        sockfd = stun_get_nat_addr(g_stunserver, g_stunport, AdaptPort, NatAddr, &port);
        if(sockfd<0)
        {
            close(pSocketPair[0]);
            return -1;
        }
        pMapedPortPair[1] = port;
        pSocketPair[1] = sockfd;
    }

#else
    return -1;
#endif

    return 0;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */




