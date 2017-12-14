#ifndef __QMAPI_AV_H__
#define __QMAPI_AV_H__

#include "QMAPICommon.h"
#include "QMAPINetSdk.h"
#include "QMAPI.h"

/*********************************************************************
    Function:   QMAPI_AVServer_Init
    Description: 初始化音视频服务,初始化音视频buffer
    Calls:
    Called By:
    parameter:
            [in] lpDeafultParam QMAPI_NET_CHANNEL_PIC_INFO
    Return: 模块句柄
********************************************************************/
int QMAPI_AVServer_Init(void *lpDeafultParam, int Resolution);

/*********************************************************************
    Function:   QMAPI_AVServer_Init
    Description: 反初始化音视频服务,反初始化音视频buffer
    Calls:
    Called By:
    parameter:
            [in] Handle 模块句柄
    Return: 
********************************************************************/
int QMAPI_AVServer_UnInit(int Handle);

/*********************************************************************
    Function:   QMAPI_AVServer_Start
    Description: 开启音视频服务
    Calls:
    Called By:
    parameter:
            [in] Handle 模块句柄
            [in] VidoeCount 视频码流打开个数，1<<0代表开启QMAPI_MAIN_STREAM，1<<1代表开启QMAPI_SECOND_STREAM
    Return: QMAPI_SUCCESS/QMAPI_FAILURE
********************************************************************/
int QMAPI_AVServer_Start(int Handle, int VidoeCount);


/*********************************************************************
    Function:   QMAPI_AVServer_Stop
    Description: 关闭音视频服务
    Calls:
    Called By:
    parameter:
            [in] Handle 模块句柄
    Return: 
********************************************************************/
int QMAPI_AVServer_Stop(int Handle);

/*********************************************************************
    Function:   QMAPI_AVServer_IOCtrl
    Description: 配置音视频编码参数
    Calls:
    Called By:
    parameter:
            [in] Handle 控制句柄
            [in] nCmd 控制命令
                    有效值为: QMAPI_SYSCFG_GET_PICCFG/QMAPI_SYSCFG_SET_PICCFG/QMAPI_SYSCFG_GET_DEF_PICCFG
            [in/out] Param QMAPI_NET_CHANNEL_PIC_INFO
            [in/out] nSize 配置结构体大小
    Return: QMAPI_SUCCESS
********************************************************************/
int QMAPI_AVServer_IOCtrl(int Handle, int nCmd, int nChannel, void* Param, int nSize);



/*********************************************************************
    Function:   QMapi_buf_add_reader
    Description:添加读指针,一个Buffer支持多个读指针操作
    Calls:
    Called By:
    parameter:
            [in] s32Chn 源通道号
            [in] enStreamType 码流类型
            [out] int *pnReaderId  返回读指针ID
    Return: QMAPI_SUCCESS
********************************************************************/
int QMapi_buf_add_reader(int Handle,const int nChannel,
	                           const QMAPI_STREAM_TYPE_E enStreamType,
	                           int *pReaderId);

/*********************************************************************
    Function:   QMapi_buf_del_reader
    Description:删除读指针
    Calls:
    Called By:
    parameter:
            [in] int nReaderId  读指针ID
    Return: QMAPI_SUCCESS
********************************************************************/
int QMapi_buf_del_reader(int Handle,int nReaderId);

/*********************************************************************
    Function:   QMapi_buf_get_frame
    Description:获取帧数据
    Calls:      从Buffer中获取帧数据，返回音频或视频原始裸码流帧数据
    Called By:
    parameter:
        [in] int nReaderId  读指针ID
        [in] QMAPI_MBUF_POS_POLICY_E enPolicy  获取帧方式
        [in] int nPosIndex  相对帧数
        [in] char* pStart   外面输入数据缓存起始地址
        [in/out] unsigned int *pdwLen 外面输入缓存长度，返回实际帧数据长度
        [out] QMAPI_NET_FRAME_HEADER *pstFrameHeader 音视频属性
    Return: QMAPI_SUCCESS
            HI_EPAERM   输入参数错误
            HI_ENOTINIT Buffer未创建
            BUF_NO_DATA Buffer中暂时无数据，需要外面再重新获取
********************************************************************/
int QMapi_buf_get_frame(int Handle,int nReaderId,
		                       QMAPI_MBUF_POS_POLICY_E enPolicy,
		                       int nOffset,
		                       char* pStart,
		                       unsigned int *pdwLen,
		                       QMAPI_NET_FRAME_HEADER *pstFrameHeader);

/*********************************************************************
    Function:   QMapi_buf_find_nextframe
    Description:尝试在数据流里找寻下一帧
    Calls:
    Called By:
    parameter:
            [in] int nReaderId  读指针ID
    Return: QMAPI_SUCCESS
********************************************************************/
int QMapi_buf_find_nextframe(int Handle,int nReaderId);

/*********************************************************************
    Function:   QMapi_buf_get_next_frametype
    Description:获取下一帧的类型
    Calls:
    Called By:
    parameter:
			[in] int nReaderId  读指针ID
			[in] QMAPI_MBUF_POS_POLICY_E enPolicy	获取读指针方式
			[in] int nOffset  相对I帧数
			[out] HI_MBUF_READPTR_INFO_S *pstReadptr 返回读指针位置信息
    Return: QMAPI_SUCCESS
********************************************************************/
int QMapi_buf_get_next_frametype(int Handle,int nReaderId,
										QMAPI_MBUF_POS_POLICY_E enPolicy,
										int nOffset,
										QMAPI_FRAME_TYPE_E *penFrameType);

/*********************************************************************
    Function:   QMapi_buf_get_readptrpos
    Description:获取读指针位置
    Calls:
    Called By:
    parameter:
            [in] int nReaderId  读指针ID
            [in] QMAPI_MBUF_POS_POLICY_E enPolicy  获取读指针方式
            [in] int nOffset  相对I帧数
            [out] QMAPI_NET_DATA_PACKET *pstReadptr 返回读指针位置信息
    Return: QMAPI_SUCCESS
********************************************************************/
int QMapi_buf_get_readptrpos(int Handle, int nReaderId,
                                   QMAPI_MBUF_POS_POLICY_E enPolicy,
                                   int nOffset,
                                   QMAPI_NET_DATA_PACKET *pstReadptr);

/*********************************************************************
    Function:QMapi_buf_can_read
    Description:数据是否能读
    Calls:
    Called By:
    parameter:
            [in] int nReaderId  读指针ID
            [in] unsigned int u32Len 长度
    Return: 返回可用数据长度，可能和请求的数据长度不一致
********************************************************************/
int QMapi_buf_can_read(int Handle,int nReaderId, unsigned int dwLen);


/*********************************************************************
    Function:QMapi_buf_set_readptrpos
    Description:设置读指针位置
    Calls:
    Called By:
    parameter:
            [in] int nReaderId  读指针ID
            [in] unsigned int u32Len   长度
    Return: QMAPI_SUCCESS
********************************************************************/
int QMapi_buf_set_readptrpos(int Handle, int nReaderId, unsigned int dwLen);


/*********************************************************************
    Function:   QMapi_buf_get_readptrLeftnum
    Description:获取读指针与写指针之间帧个数、I帧个数与块个数
                以用来协助外部的播放控制或网传控制
    Calls:
    Called By:
    parameter:
            [in] int nReaderId  读指针ID
            [out] unsigned int *pu32IFrameNum  I帧个数
            [out] unsigned int *pu32LeaveCount 剩余数据长度
    Return: QMAPI_SUCCESS
********************************************************************/
int QMapi_buf_get_readptrLeftnum(int Handle, int nReaderId, unsigned int *pdwLeaveCount,unsigned long *dwTimeTick);








#endif


