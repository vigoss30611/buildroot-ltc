/***********************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_mbuf.h
* Description: The MBUF module.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2007-05-04   h48078     NULL         Create this file.
***********************************************************************************/

#ifndef __HI_MBUF_H__
#define __HI_MBUF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <pthread.h>
#include "hi_type.h"
#include "hi_list.h"
#include "hi_mt_def.h"
#include "hi_mbuf_def.h"
#include "hi_vs_media_comm.h"

/*根据vschn号获取enc内部数组的下标
 vschn编号方式为CCS. CC为camera编号(01~08), S为该camera下的码流号(1~2)
 enc内部数组的下标 = (CC-1)*每camera支持的最大码流数 + (S-1)
 */
#define MBUF_GET_MBUFCHN_BY_VSCHN VS_CHN2IDX

//typedef void*              HI_ADDR;


/*最大通道数，每个视频编码通道对应一个通道*/
//#define MAX_MBUF_CHN_NUM	IDM_MAX_VSCHN_CNT	//BT HI3511	
#define MAX_MBUF_CHN_NUM	VS_MAX_STREAM_CNT

/*支持协议的最大packet header size(4bytes对齐)*/
#define MAX_PROTOCOL_HEADER_SIZE 	100

/*p代表读写指针，len待该帧数据长度
NEXT_POINT(p, len) 指向这帧payload后的下一个字节*/
#define NEXT_POINT(p, len) ((HI_ADDR)p + ALIGN_LENGTH(len, ALIGNTYPE_4BYTE) \
                                    + MAX_PROTOCOL_HEADER_SIZE \
                                    + ALIGN_LENGTH(sizeof(MBUF_PT_HEADER_S), ALIGNTYPE_4BYTE)) 

/*len为数据帧长度，PT_LENGTH返回整个payload的长度*/
#define PT_LENGTH(len) (ALIGN_LENGTH(len, ALIGNTYPE_4BYTE) \
                            + MAX_PROTOCOL_HEADER_SIZE \
                            + ALIGN_LENGTH(sizeof(MBUF_PT_HEADER_S), ALIGNTYPE_4BYTE))
                                    
/*p为指向MBUF_PT_HEADER_S的指针，PAYLOAD_ADDR返回
数据帧的起始地址*/
#define PAYLOAD_ADDR(p) ((HI_ADDR)p + MAX_PROTOCOL_HEADER_SIZE \
                        + ALIGN_LENGTH(sizeof(MBUF_PT_HEADER_S), ALIGNTYPE_4BYTE))

//typedef HI_S32 MBUF_CHN;
#if 0
typedef struct hiMBUF_CHENNEL_INFO_S{
    int  chnnum; /* 011 */
    HI_U32 max_connect_num;     //该通道支持的最大连接数
    HI_U32 buf_size;            //该通道每块buf的大小(4bytes对齐)
}MBUF_CHENNEL_INFO_S;
#endif
/*区分图像大小的枚举变量*/
typedef enum hiMBUF_PIC_FORMAT_E
{
    D1 = 0, 
    CIF,
    QCIF,
    VGA,
    QVGA,
    PIC_FORMAT_NUM
}MBUF_PIC_FORMAT_E; 

typedef enum hiMBUF_VIDEO_STATUS_E
{
    VIDEO_DISABLE = 0,
    VIDEO_REGISTER,       //已注册
    VIDEO_READY           //开始写入
}MBUF_VIDEO_STATUS_E;



#define SET_VIDEO_DISCARD(x)    ((x) |= 0x1)
#define SET_AUDIO_DISCARD(x)    ((x) |= 0x2)
#define SET_DATA_DISCARD(x)     ((x) |= 0x4)
#define SET_ALL_DISCARD(x)      ((x) |= 0x7)

#define CLEAR_VIDEO_DISCARD(x)  ((x) &= (~0x1))
#define CLEAR_AUDIO_DISCARD(x)  ((x) &= (~0x2))
#define CLEAR_DATA_DISCARD(x)   ((x) &= (~0x4))
#define CLEAR_ALL_DISCARD(x)    ((x) = 0)

#define CHECK_VIDEO_DISCARD(x)  ((x) & 0x1)
/*MBUF header*/
typedef struct hiMBUF_HEADER_S{
    HI_ADDR start_addr; //数据区起始地址
    HI_U32 len; //数据区长度
    MBUF_CHN ChanID; //MBUF通道号
    volatile HI_U8 discard_flag; //丢帧状态标志位,最低3bit分别表示是否丢弃视频、音频、DATA
    volatile MBUF_VIDEO_STATUS_E video_status; //该连接是否开启视频
    volatile HI_BOOL audio_enable; //该连接是否开启音频
    volatile HI_BOOL data_enable; //该连接是否开启数据
    /*start <= w/r < start + len*/
    volatile HI_ADDR writepointer; //写指针，指向当前可写字节
    volatile HI_ADDR readpointer; //读指针，指向下一个待读取字节
    volatile HI_ADDR flagpointer; //标明是否发生环回留空的指针
    volatile HI_ADDR tempWriterpointer; //零时的视频写指针
    volatile HI_ADDR last_framestart; //记录上个视频帧起始
    volatile HI_U64 discard_count; //记录该连接丢包
    List_Head_S list;
}MBUF_HEADER_S;

/*T41030 2010-02-11 Add Config of Buffer leftspace*/
#define MBUF_DEFAULT_BUF_DISCARD_THRESHOLD 20480 //16kbps*10s + u32VideoFrameLen

/*MBUF info*/
typedef struct hiMBUF_INFO_S{
    List_Head_S buf_free; //该通道空闲buf链表
    List_Head_S buf_busy; //该通道占用buf链表
    HI_U32 max_connect_num; //该通道支持的最大连接数
    HI_U32 buf_size; //该通道每块buf的大小(4bytes对齐)
    /*T41030 2010-02-11 Add Config of Buffer leftspace*/
    HI_U32 buf_discard_threshold;/*开始丢帧的门限值*/
    
    HI_ADDR start_addr; //该通道内存起始地址(4bytes对齐)
    HI_U32 total_size; //该通道总内存大小(4bytes对齐)
    pthread_mutex_t lock; //用于链表操作时互斥
}MBUF_INFO_S;
#if 0
/*MBUF payload type*/
typedef enum hiMBUF_PT_E
{
    VIDEO_KEY_FRAME = 1, //视频关键帧
    VIDEO_NORMAL_FRAME, //视频普通帧
    AUDIO_FRAME, //音频帧
    MD_FRAME, //MD数据
    MBUF_PT_BUTT
}MBUF_PT_E;
#endif
/*BUF payload header*/
typedef struct hiMBUF_PT_HEADER_S{
    MBUF_PT_E payload_type; //payload类型（视频I，视频，音频）
    HI_U32 payload_len; //payload长度
    HI_U64 pts; //时间戳
    /*视频slice的信息特别注意，一帧只有一个slice，则该slice即是起始也是结束slice*/
    HI_U32 slice_num; //每帧slice个数
    HI_U32 frame_len; //该帧整个数据长度
    HI_U8 slice_type; //slice类型 0x01－开始  0x02 中间slice 0x04 - 一帧的最后一个slice*/
    HI_U8 resv[3];
}MBUF_PT_HEADER_S;

/*MBUF初始化，负责分配内存以及管理初始化*/
HI_S32 MBUF_Init(HI_S32 chnnum, MBUF_CHENNEL_INFO_S *pinfo);

/*MBUF销毁，负责释放内存以及清空数据*/
HI_VOID MBUF_DEInit();

/*
申请一块mbuf
ChanID 输入参数，MBUF通道号
pphandle 输出参数，返回指向buf handle的指针
*/
HI_S32 MBUF_Alloc(MBUF_CHN ChanID, MBUF_HEADER_S **pphandle);

/*
释放一块mbuf
pphandle 输入输出参数，指向待释放mbuf handle的指针，
                成功释放后*pphandle = NULL
*/
HI_S32 MBUF_Free(MBUF_HEADER_S **pphandle);

/*MBUF 音频、视频、数据注册函数
HI_TRUE 使能，HI_FALSE 禁止*/
HI_VOID MBUF_Register(MBUF_HEADER_S *phandle, MBUF_VIDEO_STATUS_E video_status, HI_BOOL audio_enable, HI_BOOL data_enable);

/*MBUF 获取音频、视频、数据注册状态函数
HI_TRUE 使能，HI_FALSE 禁止*/
HI_VOID MBUF_GetRegister(MBUF_HEADER_S *phandle, MBUF_VIDEO_STATUS_E* pvideo_status, HI_BOOL* paudio_enable, HI_BOOL* pdata_enable);

/*提供给媒体处理模块的接口函数，用于向连接buf写入数据
该函数会依据ChanID遍历对应的busy list*/

HI_VOID MBUF_Write(HI_S32 ChanID, MBUF_PT_E payload_type, 
                   const HI_ADDR addr, HI_U32 len, HI_U64 pts, 
                   HI_U8 *pslice_type, HI_U32 slicenum, HI_U32 frame_len);
                   
HI_S32 MBUF_Read(MBUF_HEADER_S *phandle, HI_ADDR*paddr, HI_U32 *plen, 
            HI_U64 *ppts, MBUF_PT_E *ppayload_type,HI_U8 *pslice_type, 
            HI_U32 *pslicenum, HI_U32 *pframe_len);

HI_VOID MBUF_Set(MBUF_HEADER_S *phandle);

#define MBUFF_MOD "MBUFF"

#ifndef DEBUG_MBUF
#define DEBUG_MBUF 8
#if (DEBUG_MBUF == 0) || !defined(DEBUG_ON)
    #define DEBUG_MBUF_PRINT(pszModeName, u32Level, args ...)
#else
#define DEBUG_MBUF_PRINT(pszModeName, u32Level, args ...)   \
    do  \
    {   \
        if (DEBUG_MBUF >= u32Level)    \
        {   \
            printf(args);   \
        }   \
    }   \
    while(0)
#endif
#endif
            
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif

