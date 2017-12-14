/**
 */

#ifndef __CBUFFER_H__
#define __CBUFFER_H__

class CBuffer
{
public:

private:
    /*块大小*/
    HI_U32 m_u32BlockSize;
    /*块个数*/
    HI_U32 m_u32BlockNum;
    /*buffer创建标识*/
    HI_BOOL m_bBufCreate;
    /*读指针计数*/
    HI_U32 m_u32ReaderCnt;

    HI_U32 m_u32VideoPacketNo;

    HI_U32 m_u32AudioPacketNo;

    HI_U32 m_u32ReadVideoPacketNo[BUF_MAX_READER_NUM];

    HI_U32 m_u32ReadAudioPacketNo[BUF_MAX_READER_NUM];

	HI_U32 m_bLock;
    /*buffer信息*/
    BUF_STREAM_INFO_S m_stBufferInfo;
    /*读指针位置*/
    HI_U32 m_au32ReadPos[BUF_MAX_READER_NUM];
    /*读指针圈数*/
    HI_U32 m_au32ReadCycle[BUF_MAX_READER_NUM];
    /*读指针使用情况*/
    HI_BOOL m_abReaderUsed[BUF_MAX_READER_NUM];
    /*I帧信息*/
    BUF_IFRAME_MNG_S m_stIFrameManager;
    /*记录每一个写入的I帧的时间*/
    VS_DATETIME_S m_stKeyTime;
    /*码流属性*/
    BUF_STREAM_ATTR_S m_stStreamAttr;
    /*读指针码流属性*/
    BUF_STREAM_ATTR_S m_astReaderStreamAttr[BUF_MAX_READER_NUM];
    /*buf锁*/
    pthread_mutex_t m_BufLock;
private:
    /*初始化I帧管理*/
    HI_S32 InitIFrameManager(HI_U32 u32ExpectIFrameNum);
    /*去初始化I帧管理*/
    HI_S32 DeinitIFrameManager();
    /*设置I帧管理*/
    HI_S32 SetIFrameManager(HI_U32 u32Size, HI_BOOL bIFrameHeader);
    /*设置块信息*/
    HI_S32 SetBlockInfo(HI_BOOL bIFrame,HI_U32 u32WritePos,HI_U32 u32DataLen,VS_DATETIME_S *pTime);
    /*获取离当前写指针最近I帧所在块的起始位置*/
    HI_S32 GetLastWriteNIFramPos(HI_U32 u32N,BUF_IFRAME_MNG_S stIFramManager,HI_U32 *pu32ReadPos,HI_U32 *pu32ReadCycle);
public:
    CBuffer();
    ~CBuffer();
public:
 /**
 \brief 创建Buffer,供Buffer管理调用
 \attention \n
 \param[in] HI_U32 u32BlockSize  块大小
 \param[in] HI_U32 u32BlockNum   快个数
 \retval ::HI_SUCCESS
 */
    HI_S32 BUF_Create(HI_U32 u32BlockSize, HI_U32 u32BlockNum);

 /**
 \brief 销毁Buffer
 \attention \n
 \param
 \retval ::HI_SUCCESS
 */
    HI_S32 BUF_Destroy();

 /**
 \brief 写Buffer
 \attention \n
 \param[in] HI_U8 *pu8Start 起始地址
 \param[in] HI_U32 u32Len   长度
 \param[in] HI_BOOL bIFrame 是否为I帧
 \param[in] VS_DATETIME_S *pTime  时间
 \retval ::HI_SUCCESS
 \see \n
 */
	void BUF_Lock();
    HI_S32 BUF_Write(const HI_U8 *pu8Start,HI_U32 u32Len,HI_BOOL bIFrame,VS_DATETIME_S *pTime);
	void BUF_Unlock();
 /**
 \brief 写视频数据进Buffer
 \attention \n
 \param[in] HI_U8 *pu8Start 起始地址
 \param[in] HI_U32 u32Len   长度
 \param[in] VS_FRAME_TYPE_E enFrameType 帧类型
 \param[in] VS_DATETIME_S *pTime  时间
 \param[in] HI_S32 s32Chn  通道号
 \param[in] HI_U64 u64Pts  时间戳
 \retval ::HI_SUCCESS
 \see \n
    ::BUF_WriteAudioData
 */
    HI_S32 BUF_WriteVideoData(const HI_U8 *pu8Start,HI_U32 u32Len,VS_FRAME_TYPE_E enFrameType,VS_DATETIME_S *pTime,HI_S32 s32Chn,HI_U64 u64Pts);

 /**
 \brief 写音频数据进Buffer
 \attention \n
 \param[in] HI_U8 *pu8Start 起始地址
 \param[in] HI_U32 u32Len   长度
 \param[in] VS_AUDIO_FORMAT_E enCodecType 音频类型
 \param[in] HI_S32 s32Chn  通道号
 \param[in] HI_U64 u64Pts  时间戳
 \retval ::HI_SUCCESS
 \see \n
    ::BUF_WriteVideoData
 */
    HI_S32 BUF_WriteAudioData(const HI_U8 *pu8Start,HI_U32 u32Len,HI_S32 s32Chn,HI_U64 u64Pts);

  /**
 \brief 添加读指针
 \attention \n
 \param[out] HI_S32 *ps32ReaderId  返回读指针ID
 
 \retval ::HI_SUCCESS
 \see \n
    ::BUF_DelReader
 */
    HI_S32 BUF_AddReader(HI_S32 *ps32ReaderId);

 /**
 \brief 删除读指针
 \attention \n
 \param[in] HI_S32 s32ReaderId  读指针ID
 \retval ::HI_SUCCESS
 \see \n
    ::BUF_AddReader
 */
    HI_S32 BUF_DelReader(HI_S32 s32ReaderId);

 /**
 \brief 获取读指针位置
 \attention \n
 \param[in] HI_S32 s32ReaderId  读指针ID
 \param[in] BUF_POSITION_POLICY_E enPolicy  获取读指针方式
 \param[in] HI_S32 s32N  相对I帧数
 \param[out] BUF_READPTR_INFO_S *pstReadptr 返回读指针位置信息
 \retval ::HI_SUCCESS
 \see \n
    ::BUF_WriteVideoData
 */
    HI_S32 BUF_GetReadPtrPos(HI_S32 s32ReaderId,
                            BUF_POSITION_POLICY_E enPolicy,
                            HI_S32 s32N,
                            BUF_READPTR_INFO_S *pstReadptr);

 /**
 \brief 数据是否能读
 \attention \n
 \param[in] HI_S32 s32ReaderId  读指针ID
 \param[in] HI_U32 u32Len 长度
 \retval ::HI_SUCCESS
 */
    HI_S32 BUF_CanRead(HI_S32 s32ReaderId, HI_U32 u32Len);

 /**
 \brief 设置读指针位置
 \attention \n
 \param[in] HI_S32 s32ReaderId  读指针ID
 \param[in] HI_U32 u32Len   长度
 \retval ::HI_SUCCESS
 */
    HI_S32 BUF_SetReadPtrPos(HI_S32 s32ReaderId, HI_U32 u32Len);

 /**
 \brief 获取读指针与写指针之间I帧个数与块个数
        以用来协助外部的播放控制或网传控制
 \attention \n
 \param[in] HI_S32 s32ReaderId  读指针ID
 \param[out] HI_U32 *pu32IFrameNum  I帧个数
 \param[out] HI_U32 *pu32BlockNum   块个数
 \retval ::HI_SUCCESS
 */
    HI_S32 BUF_GetReadPtrLeftNum(HI_S32 s32ReaderId,
                                 HI_U32 *pu32BlockNum);

 /**
 \brief 获取读指针的码流属性
 \attention \n
 \param[in] HI_S32 s32ReaderId  读指针ID
 \param[out] BUF_STREAM_ATTR_S pstStreamAttr  返回码流属性
 \retval ::HI_SUCCESS
 \see \n
 */
    HI_S32 BUF_GetReaderStreamAttr(HI_S32 s32ReaderId,
                                      BUF_STREAM_ATTR_S *pstStreamAttr);

 /**
 \brief 设置Buffer码流属性
 \attention \n
 \param[out] stStreamAttr  码流属性
 \retval ::HI_SUCCESS
 \see \n
 */
    HI_S32 BUF_SetStreamAttr(const BUF_STREAM_ATTR_S *pstStreamAttr);

 /**
 \brief 获取Buffer的码流属性
 \attention \n
 \param[out] BUF_STREAM_ATTR_S *pstStreamAttr  返回码流属性
 \retval ::HI_SUCCESS
 \see \n
 */
    HI_U32 BUF_GetStreamAttr(BUF_STREAM_ATTR_S *pstStreamAttr);

    HI_U32  BUF_GetBufPhyAddr();

    HI_U32  BUF_GetBufSize();

    HI_U8 * BUF_GetBufAddr();

    HI_S32  BUF_CheckReaderPacketNo(HI_S32 s32ReaderId , HI_BOOL bVideo ,  HI_U32 u32PacketNo);
    
	unsigned char  *   BUF_GetWriteMemPos(HI_U32 u32Size);


/** @} */  /*! <!-- function Definition end */
};


#endif /*__CBUFFER_H__*/


