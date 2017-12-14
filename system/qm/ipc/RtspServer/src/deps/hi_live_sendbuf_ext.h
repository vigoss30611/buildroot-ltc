#ifndef __HI_LIVE_SENDBUF_EXT_H__
#define __HI_LIVE_SENDBUF_EXT_H__



#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

 





HI_S32 HI_LiveSendBuf_Init(HI_S32 maxUserNum,HI_S32 maxBufSize);


/*MBUF销毁，负责释放内存以及清空数据*/
HI_S32 HI_LiveSendBuf_Deinit();

/*
申请一块mbuf
ChanID 输入参数，MBUF通道号
pphandle 输出参数，返回指向buf handle的指针
*/
HI_S32 HI_LiveSendBuf_Alloc(HI_S32 ChanID, HI_VOID **pphandle);

/*
释放一块mbuf
pphandle 输入输出参数，指向待释放mbuf handle的指针，
                成功释放后*pphandle = NULL
*/
HI_S32 HI_LiveSendBuf_Free(HI_VOID **pphandle);

/*MBUF 音频、视频、数据注册函数
HI_TRUE 使能，HI_FALSE 禁止*/
HI_S32 HI_LiveSendBuf_Register(HI_VOID *phandle, MBUF_VIDEO_STATUS_E video_status, HI_BOOL audio_enable, HI_BOOL data_enable);

/*MBUF 获取音频、视频、数据注册状态函数
HI_TRUE 使能，HI_FALSE 禁止*/
HI_VOID HI_LiveSendBuf_GetRegister(HI_VOID *phandle, MBUF_VIDEO_STATUS_E* pvideo_status, HI_BOOL* paudio_enable, HI_BOOL* pdata_enable);

/*提供给媒体处理模块的接口函数，用于向连接buf写入数据
该函数会依据ChanID遍历对应的busy list*/

                   
HI_S32 HI_LiveSendBuf_Read(HI_VOID *phandle, HI_ADDR*paddr, HI_U32 *plen, 
            HI_U64 *ppts, MBUF_PT_E *ppayload_type,HI_U8 *pslice_type, 
            HI_U32 *pslicenum, HI_U32 *pframe_len);

HI_VOID HI_LiveSendBuf_Set(HI_VOID *phandle);

HI_S32 HI_LiveSendBuf_GetPts(HI_VOID *phandle,HI_U64* pU64Pts);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */



#endif




