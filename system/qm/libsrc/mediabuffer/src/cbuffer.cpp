/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <malloc.h>

#include "hi_comm_inc.h"
#include "hi_buf_define.h"
#include "cbuffer.h"
#ifdef HI3518
#include "hi_mem.h"
#endif
#define BUF_MAX_PRE_RECORD_TIME 10
#define BUF_MIN_IFRAME_INTERVAL 1

#ifdef DM36X

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
unsigned char *OSA_cmemGetPhysAddr(unsigned char *virtAddr);
unsigned char  *OSA_cmemAllocCached(unsigned int size, unsigned int  alignment);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
CBuffer::CBuffer()
{
    m_u32BlockSize = 0;
    m_u32BlockNum  = 0;
    m_bBufCreate = HI_FALSE;
    m_u32ReaderCnt = 0;
    (HI_VOID)memset(&m_stBufferInfo, 0, sizeof(BUF_STREAM_INFO_S));
    (HI_VOID)memset(m_au32ReadPos,  0, BUF_MAX_READER_NUM * sizeof(HI_U32));
    (HI_VOID)memset(m_au32ReadCycle,0, BUF_MAX_READER_NUM * sizeof(HI_U32));
    (HI_VOID)memset(m_abReaderUsed, 0, BUF_MAX_READER_NUM * sizeof(HI_BOOL));
    (HI_VOID)memset(&m_stIFrameManager, 0, sizeof(BUF_IFRAME_MNG_S));
    (HI_VOID)memset(&m_stKeyTime, 0, sizeof(VS_DATETIME_S));
    m_stStreamAttr.enCodecType = VS_AUDIO_FORMAT_G711A;
    m_stStreamAttr.enNorm = VS_TVSYSTEM_PAL;
    m_stStreamAttr.u32Width= 704;
    m_stStreamAttr.u32Height= 576;
    m_stStreamAttr.u32Bps = 512;
    m_stStreamAttr.u32Fps = 25;
    m_stStreamAttr.u32Gop = 25;
	m_bLock = HI_FALSE;
    for (HI_S32 i = 0; i < BUF_MAX_READER_NUM; i++)
    {
        m_astReaderStreamAttr[i].enCodecType = VS_AUDIO_FORMAT_G711A;
        m_astReaderStreamAttr[i].enNorm = VS_TVSYSTEM_PAL;
        m_astReaderStreamAttr[i].u32Width= 704;
        m_astReaderStreamAttr[i].u32Height= 576;
        m_astReaderStreamAttr[i].u32Bps = 512;
        m_astReaderStreamAttr[i].u32Fps = 25;
        m_astReaderStreamAttr[i].u32Gop = 25;
    }
    pthread_mutex_init(&m_BufLock, NULL);
}

/*********************************************************************
    Function:~CBuffer
    Description:buffer释构函数
    Calls:
    Called By:
    parameter:
    Return:
    author:
********************************************************************/
CBuffer::~CBuffer()
{
    pthread_mutex_destroy(&m_BufLock);
}


/*********************************************************************
    Function:  InitIFrameManager
    Description:  初始化I帧管理
    Calls:
    Called By:
    parameter:
           [in] HI_U32 u32ExpectIFrameNum  I帧个数
    Return:
    author:
********************************************************************/
HI_S32 CBuffer::InitIFrameManager(HI_U32 u32ExpectIFrameNum)
{
    m_stIFrameManager.u32ExpectNumber = u32ExpectIFrameNum;
    m_stIFrameManager.u32ValidNumber  = 0;
    m_stIFrameManager.pu32Position = new HI_U32[u32ExpectIFrameNum];
    m_stIFrameManager.pu32Cycle    = new HI_U32[u32ExpectIFrameNum];
    m_stIFrameManager.u32OldestIFrameIndex = 0;

    (HI_VOID)memset(m_stIFrameManager.pu32Position, 0, u32ExpectIFrameNum * sizeof(HI_U32));
    (HI_VOID)memset(m_stIFrameManager.pu32Cycle   , 0, u32ExpectIFrameNum * sizeof(HI_U32));

    return HI_SUCCESS;
}


/*********************************************************************
    Function:  DeinitIFrameManager
    Description:  去初始化I帧管理
    Calls:
    Called By:
    parameter:
    Return:
    author:
********************************************************************/
HI_S32 CBuffer::DeinitIFrameManager()
{
    if (m_stIFrameManager.pu32Position)
    {
        delete []m_stIFrameManager.pu32Position;
        m_stIFrameManager.pu32Position = NULL;
    }
    if (m_stIFrameManager.pu32Cycle)
    {
        delete []m_stIFrameManager.pu32Cycle;
        m_stIFrameManager.pu32Cycle = NULL;
    }
    (HI_VOID)memset(&m_stIFrameManager, 0, sizeof(BUF_IFRAME_MNG_S));
    return HI_SUCCESS;
}


/*********************************************************************
    Function:  SetIFrameManager
    Description:  设置I帧管理
    Calls:
    Called By:
    parameter:
    Return:
    author:
********************************************************************/
HI_S32 CBuffer::SetIFrameManager(HI_U32 u32Size, HI_BOOL bIFrameHeader)
{
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    if (0 == m_u32BlockSize)
    {
        return HI_FAILURE;
    }
    /*first clear the overwrited i frame
        [FirstSegStart, FirstSegEnd), [SecondSegStart, SecondSegEnd)*/
    HI_U32 FirstSegStart = m_stBufferInfo.u32WritePos;
    HI_U32 FirstSegEnd = 0, SecondSegStart = 0, SecondSegEnd = 0;
    if ((m_stBufferInfo.u32WritePos + u32Size) <= m_stBufferInfo.u32BufferSize)
    {
        FirstSegEnd = m_stBufferInfo.u32WritePos + u32Size;
        FirstSegEnd = (FirstSegEnd + m_u32BlockSize - 1)
            / m_u32BlockSize * m_u32BlockSize;/*结束位置对齐到块尾*/
    }
    else
    {
        FirstSegEnd  = m_stBufferInfo.u32BufferSize;
        SecondSegEnd = m_stBufferInfo.u32WritePos + u32Size
                     - m_stBufferInfo.u32BufferSize;
        SecondSegEnd = (SecondSegEnd + m_u32BlockSize - 1)
            / m_u32BlockSize * m_u32BlockSize;/*结束位置对齐到块尾*/
    }

    HI_U32 TmpPosition = 0;
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    while (m_stIFrameManager.u32ValidNumber)
    {
        /*头没有被覆盖，数据部分肯定不会被覆盖，老的数据没有被覆盖，新的数据也不可能被覆盖*/
        TmpPosition = m_stIFrameManager.pu32Position[m_stIFrameManager.u32OldestIFrameIndex];
        if ( ( ( TmpPosition >= FirstSegStart) && (TmpPosition < FirstSegEnd))  \
            || (( TmpPosition >= SecondSegStart) && (TmpPosition < SecondSegEnd)))
        {
              HI_U32 block_index = TmpPosition / m_u32BlockSize;

    		/* 2011-1-20  c47559 解决bug: 当cbuffer  只分配一个BLOCK,  会概率性出现不编码, cpu idle 为0 问题*/
    		if(m_stBufferInfo.pstBlockInfo[block_index].u32IFrameNum == 0)
    		{								 
    			m_stIFrameManager.u32ValidNumber = 0;
    			m_stIFrameManager.u32OldestIFrameIndex = 0;
    			break;
    		}
			
            if (m_stIFrameManager.u32ValidNumber
                >= m_stBufferInfo.pstBlockInfo[block_index].u32IFrameNum)
            {
                m_stIFrameManager.u32ValidNumber
                    -= m_stBufferInfo.pstBlockInfo[block_index].u32IFrameNum;
                m_stIFrameManager.u32OldestIFrameIndex
                    += m_stBufferInfo.pstBlockInfo[block_index].u32IFrameNum;
                if (m_stIFrameManager.u32OldestIFrameIndex
                    >= m_stIFrameManager.u32ExpectNumber)
                {
                    m_stIFrameManager.u32OldestIFrameIndex
                        = m_stIFrameManager.u32OldestIFrameIndex
                        % m_stIFrameManager.u32ExpectNumber;
                }
            }
            else
            {
                m_stIFrameManager.u32ValidNumber = 0;
                m_stIFrameManager.u32OldestIFrameIndex = 0;
                for (unsigned int i = 0; i < m_u32BlockNum; i++)
                {
                    m_stBufferInfo.pstBlockInfo[i].u32IFrameNum = 0;
                }
            }
        }
        else
        {
            break;
        }
    }
    if (0 == m_stIFrameManager.u32ValidNumber)
    {
        m_stIFrameManager.u32OldestIFrameIndex = 0;
    }

    /*second set new I-frame*/
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    if (bIFrameHeader)
    {
        if (m_stIFrameManager.u32ValidNumber < m_stIFrameManager.u32ExpectNumber)
        {
            m_stIFrameManager.pu32Position[(m_stIFrameManager.u32OldestIFrameIndex \
                    + m_stIFrameManager.u32ValidNumber) % m_stIFrameManager.u32ExpectNumber] \
                    = m_stBufferInfo.u32WritePos;
            m_stIFrameManager.pu32Cycle[(m_stIFrameManager.u32OldestIFrameIndex \
                    + m_stIFrameManager.u32ValidNumber) % m_stIFrameManager.u32ExpectNumber] \
                    = m_stBufferInfo.u32WriteCycle;
            m_stIFrameManager.u32ValidNumber++;
        }
        else
        {
            m_stIFrameManager.pu32Position[m_stIFrameManager.u32OldestIFrameIndex]
                = m_stBufferInfo.u32WritePos;
            m_stIFrameManager.pu32Cycle[m_stIFrameManager.u32OldestIFrameIndex]
                = m_stBufferInfo.u32WriteCycle;
            m_stIFrameManager.u32OldestIFrameIndex++;
            m_stIFrameManager.u32OldestIFrameIndex = m_stIFrameManager.u32OldestIFrameIndex \
                % m_stIFrameManager.u32ExpectNumber;
        }
    }

    //printf("#### %s %d, u32ValidNumber=%d\n", __FUNCTION__, __LINE__, m_stIFrameManager.u32ValidNumber);
    return HI_SUCCESS;
}


/*********************************************************************
    Function:  SetBlockInfo
    Description:  设置块信息
    Calls:
    Called By:
    parameter:
    Return:
    author:
********************************************************************/
HI_S32 CBuffer::SetBlockInfo(HI_BOOL bIFrame,
                              HI_U32 u32WritePos,
                              HI_U32 u32DataLen,
                              VS_DATETIME_S *pTime)
{
    if (0 == m_u32BlockSize)
    {
        return HI_FAILURE;
    }
    int first = u32WritePos / m_u32BlockSize;
    int end   = (u32WritePos + u32DataLen -1) / m_u32BlockSize;

    if (m_stBufferInfo.pstBlockInfo[first].u32Cycle != m_stBufferInfo.u32WriteCycle)
    {
        m_stBufferInfo.pstBlockInfo[first].u32IFrameNum = 0;
        m_stBufferInfo.pstBlockInfo[first].u32FrameNum = 0;
        m_stBufferInfo.pstBlockInfo[first].u32Cycle = m_stBufferInfo.u32WriteCycle;
    }

    /*填充第一个块*/
    m_stBufferInfo.pstBlockInfo[first].u32FrameNum++;
    if (bIFrame && pTime)/*I FRAME*/
    {
		if (m_stBufferInfo.pstBlockInfo[first].u32IFrameNum >= BUF_MAX_IFRAME_IN_BLOCK)
		{
			return HI_FAILURE;
		}
        m_stBufferInfo.pstBlockInfo[first].u32IFrameNum++;
        m_stBufferInfo.pstBlockInfo[first].astIFrame[m_stBufferInfo.pstBlockInfo[first].u32IFrameNum-1].u32Offset
                                         = u32WritePos % m_u32BlockSize;
        (HI_VOID)memcpy(&m_stBufferInfo.pstBlockInfo[first].astIFrame[m_stBufferInfo.pstBlockInfo[first].u32IFrameNum-1].stStreamAttr,
            &m_stStreamAttr, sizeof(BUF_STREAM_ATTR_S));
    }
    else/*NOT I FRAME*/
    {
        m_stBufferInfo.pstBlockInfo[first].astIFrame[m_stBufferInfo.pstBlockInfo[first].u32IFrameNum].u32Offset
            = 0xffffffff;
    }

    /*填充剩余的块*/
    for (int i = first + 1; i <= end; i++)/*NOT I FRAME*/
    {
        m_stBufferInfo.pstBlockInfo[i].u32Cycle = m_stBufferInfo.u32WriteCycle;
        m_stBufferInfo.pstBlockInfo[i].u32IFrameNum = 0;
        m_stBufferInfo.pstBlockInfo[i].u32FrameNum = 0;
        m_stBufferInfo.pstBlockInfo[i].astIFrame[0].u32Offset = 0xffffffff;
    }
    return HI_SUCCESS;
}


/*********************************************************************
    Function:   GetLastWriteNIFramPos
    Description:获取离当前写指针最近I帧所在块的起始位置
    Calls:
    Called By:
    parameter:
          [in] HI_U32 u32N  I帧数
          [in] BUF_IFRAME_MNG_S stIFramManager  I帧管理结构体
          [out]HI_U32 *pu32ReadPos  返回读指针位置
          [out]HI_U32 *pu32ReadCycle 返回读指针圈数
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::GetLastWriteNIFramPos(HI_U32 u32N,
                                      BUF_IFRAME_MNG_S stIFramManager,
                                      HI_U32 *pu32ReadPos,
                                      HI_U32 *pu32ReadCycle)
{
    if (0 == m_u32BlockSize || 0 == stIFramManager.u32ExpectNumber)
    {
		printf("%s %d BlockSize %d u32ExpectNumber %d\n",__FUNCTION__,__LINE__,m_u32BlockSize,
			stIFramManager.u32ExpectNumber);
        return HI_FAILURE;
    }
    HI_S32 s32Ret = HI_SUCCESS;
    printf("Stream Maneger: Query Ifrmae is %d \n" , stIFramManager.u32ValidNumber);
    if (0 == stIFramManager.u32ValidNumber)
    {
        return HI_FAILURE;
    }

    if (u32N >= stIFramManager.u32ValidNumber)
    {
        u32N = stIFramManager.u32ValidNumber - 1;
        s32Ret = 1;
    }

    HI_S32 s32Index = (stIFramManager.u32ValidNumber - u32N \
              + stIFramManager.u32OldestIFrameIndex -1) \
              % stIFramManager.u32ExpectNumber;
    *pu32ReadPos = stIFramManager.pu32Position[s32Index]
                               / m_u32BlockSize * m_u32BlockSize;
    *pu32ReadCycle = stIFramManager.pu32Cycle[s32Index];
    return s32Ret;
}
                                                    

/*********************************************************************
    Function:BUF_Create
    Description:创建写指针
    Calls:
    Called By:
    parameter:
          [in] HI_U32 u32BlockSize  块大小
          [in] HI_U32 u32BlockNum 块个数
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_Create(HI_U32 u32BlockSize, HI_U32 u32BlockNum)
{
    if (HI_TRUE == m_bBufCreate)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Create Buffer have been created!\n");
        return HI_EEXIST;
    }

    m_u32BlockSize = u32BlockSize;
    m_u32BlockNum  = u32BlockNum;

   

    #ifdef DM36X
    {
    	 m_stBufferInfo.pu8BufferStart = (HI_U8 *)OSA_cmemAllocCached(m_u32BlockSize * m_u32BlockNum , 512);
	    if (NULL == m_stBufferInfo.pu8BufferStart)
	    {
	        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Create Stream buffer create failed(-1)!\n");
	        return HI_FAILURE;
	    }
    	m_stBufferInfo.pu8BufferStartPhy =(HI_U32) OSA_cmemGetPhysAddr(m_stBufferInfo.pu8BufferStart);
    }
    #else
    {
    	 m_stBufferInfo.pu8BufferStart = (HI_U8 *)memalign(512, m_u32BlockSize * m_u32BlockNum);
	    if (NULL == m_stBufferInfo.pu8BufferStart)
	    {
	        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Create Stream buffer create failed(-1)!\n");
	        return HI_FAILURE;
	    }
    	m_stBufferInfo.pu8BufferStartPhy = 0;
    }
    #endif

	m_stBufferInfo.pstBlockInfo = new BUF_BLOCK_INFO_S[m_u32BlockNum];
	if (NULL == m_stBufferInfo.pstBlockInfo)
	{
		delete []m_stBufferInfo.pu8BufferStart;
		m_stBufferInfo.pu8BufferStart = NULL;
		HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Create Stream buffer create failed(-2)!\n");
		return HI_ENORES;
	}
	(HI_VOID)memset(m_stBufferInfo.pu8BufferStart, 0, m_u32BlockSize * m_u32BlockNum * sizeof(HI_U8));
	(HI_VOID)memset(m_stBufferInfo.pstBlockInfo, 0, m_u32BlockNum * sizeof(BUF_BLOCK_INFO_S));
	
	m_stBufferInfo.u32BufferSize = m_u32BlockSize * m_u32BlockNum;
	m_stBufferInfo.u32WriteCycle = 0;
	m_stBufferInfo.u32WritePos   = 0;
	
	(HI_VOID)InitIFrameManager(BUF_MAX_PRE_RECORD_TIME / BUF_MIN_IFRAME_INTERVAL + 1);
	
	m_bBufCreate = HI_TRUE;


    return HI_SUCCESS;
}


/*********************************************************************
    Function:     BUF_Destroy
    Description:  销毁写Buffer
    Calls:
    Called By:
    parameter:
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_Destroy()
{
    if (HI_FALSE == m_bBufCreate)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Destroy Buffer not create!\n");
        return HI_ENOTINIT;
    }

    pthread_mutex_lock(&m_BufLock);
    if (m_u32ReaderCnt > 0)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Destroy Buffer in use\n");
        pthread_mutex_unlock(&m_BufLock);
        return HI_FAILURE;
    }
    pthread_mutex_unlock(&m_BufLock);

    if (NULL != m_stBufferInfo.pstBlockInfo)
    {
        delete []m_stBufferInfo.pstBlockInfo;
        m_stBufferInfo.pstBlockInfo = NULL;
    }
    if (NULL != m_stBufferInfo.pu8BufferStart)
    {
        delete []m_stBufferInfo.pu8BufferStart;
        m_stBufferInfo.pu8BufferStart = NULL;
    }
    (HI_VOID)memset(&m_stBufferInfo, 0, sizeof(BUF_STREAM_INFO_S));

    (HI_VOID)DeinitIFrameManager();
	m_bBufCreate = HI_FALSE;

	return HI_SUCCESS;
}

void CBuffer::BUF_Lock()
{
	//if(m_bLock == HI_FALSE)
	pthread_mutex_lock(&m_BufLock);
	//m_bLock = HI_TRUE;
}

void CBuffer::BUF_Unlock()
{
	//if(m_bLock)
	pthread_mutex_unlock(&m_BufLock);
	//m_bLock = HI_FALSE; 
}

/*********************************************************************
    Function:     BUF_Write
    Description:  写Buffer
    Calls:
    Called By:
    parameter:
           [in] HI_U8 *pu8Start 起始地址
           [in] HI_U32 u32Len   长度
           [in] HI_BOOL bIFrame 是否为I帧
           [in] VS_DATETIME_S *pTime  时间
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_Write(const HI_U8 *pu8Start,
                             HI_U32 u32Len,
                             HI_BOOL bIFrame,
                             VS_DATETIME_S *pTime)
{
	BUF_IFRAME_MNG_S TmpstIFrameManager;
	HI_S32 s32Ret;
    if (HI_FALSE == m_bBufCreate)
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Write Buffer not create\n");
        return HI_ENOTINIT;
    }
    if ( 0 == u32Len)
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Write Invalid para\n");
        return HI_EPAERM;
    }
    if ((HI_TRUE == bIFrame) && (NULL == pTime))
    {
        /*如果指明为I帧却未传入I帧时间信息，则将此I帧作为非I帧处理*/
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        bIFrame = HI_FALSE;
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Write Invalid I-frame time.\n");
    }

    /*受dvrfs限制，如果当前关键帧时间与上一个关键帧时间相同，则需要把此关键帧作为非关键帧来处理*/
    if (HI_TRUE == bIFrame)
    {
		/*
        if (m_stKeyTime.s32Year == pTime->s32Year
            && m_stKeyTime.s32Month == pTime->s32Month
            && m_stKeyTime.s32Day   == pTime->s32Day
            && m_stKeyTime.s32Hour  == pTime->s32Hour
            && m_stKeyTime.s32Minute== pTime->s32Minute
            && m_stKeyTime.s32Second== pTime->s32Second)
        {
           // bIFrame = HI_FALSE;
        }
        else
        {
            (HI_VOID)memcpy(&m_stKeyTime,pTime,sizeof(VS_DATETIME_S));
        }
		*/
    }
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    /*I帧信息管理*/
    (HI_VOID)memcpy(&TmpstIFrameManager, &m_stIFrameManager, sizeof(BUF_IFRAME_MNG_S));
	//if(m_bLock == HI_FALSE)
	//{
	//	printf("%s %d Please Lock Buffer first.",__FUNCTION__,__LINE__);
	//}
    (HI_VOID)SetIFrameManager(u32Len, bIFrame);
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    unsigned long LeftSize=0, CopySize=0;
    while (u32Len)
    {
        LeftSize = m_stBufferInfo.u32BufferSize - m_stBufferInfo.u32WritePos;//缓冲区剩余空间
        if(LeftSize < u32Len)
        {
            CopySize = LeftSize;//需要回头写
        }
        else
        {
            CopySize = u32Len;//一次性写完
        }

        s32Ret = SetBlockInfo(bIFrame, m_stBufferInfo.u32WritePos, CopySize, pTime);
		if (s32Ret != 0)
		{
            HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_Write SetBlockInfo failed.\n");
			(HI_VOID)memcpy(&m_stIFrameManager, &TmpstIFrameManager, sizeof(BUF_IFRAME_MNG_S));
			return -3;
		}
        bIFrame = HI_FALSE;
		if(pu8Start)
		{
	        (HI_VOID)memcpy(m_stBufferInfo.pu8BufferStart + m_stBufferInfo.u32WritePos, pu8Start, CopySize);
	        pu8Start += CopySize;
        }
        u32Len   -= CopySize;

        if ((m_stBufferInfo.u32WritePos + CopySize) < m_stBufferInfo.u32BufferSize)
        {
            m_stBufferInfo.u32WritePos += CopySize;
        }
        else
        {
            /*当要换圈时，必须确保写指针和圈数原子性操作*/
            m_stBufferInfo.u32WritePos = 0;
            m_stBufferInfo.u32WriteCycle++;
        }
    }
    return HI_SUCCESS;
}


/*********************************************************************
    Function:     BUF_WriteVideoData
    Description:  写视频数据进Buffer
    Calls:
    Called By:
    parameter:
            [in] HI_U8 *pu8Start 起始地址
            [in] HI_U32 u32Len   长度
            [in] VS_FRAME_TYPE_E enFrameType 帧类型
            [in] VS_DATETIME_S *pTime  时间
            [in] HI_S32 s32Chn  通道号
            [in] HI_U64 u64Pts  时间戳
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_WriteVideoData(const HI_U8 *pu8Start,
                                      HI_U32 u32Len,
                                      VS_FRAME_TYPE_E enFrameType,
                                      VS_DATETIME_S *pTime,
                                      HI_S32 s32Chn,
                                      HI_U64 u64Pts)
{
    HI_U8 	header[sizeof(VIDEO_HEAD_S) ] = {0};
    HI_BOOL bIFrame = HI_FALSE;
    HI_U8  dumyflag = 0;

WriteDumy:
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
 	if(m_stBufferInfo.u32WritePos + u32Len  + sizeof(header) > m_stBufferInfo.u32BufferSize)  //buffer 剩余空间不足一帧，丢弃buffer内存   --vinsent
 	{
	    memcpy(header, "00dcdumy", 8);
	    dumyflag = 1;
 	}
 	else
 	{
	    memcpy(header, "00dcH264", 8);
	    dumyflag = 0;
	}

	if(m_stBufferInfo.u32WritePos  + sizeof(header) > m_stBufferInfo.u32BufferSize)  //如果剩余空间不足以放帧头信息
	{
		printf("left buffer small then head\n");
		return 1;
	}

	if(dumyflag)
	{
		HI_U32 dumylen = 0;
		*((HI_U32*)&header[8]) = dumylen = m_stBufferInfo.u32BufferSize - m_stBufferInfo.u32WritePos  - sizeof(header) ;
	  	BUF_Write(header, sizeof(header) , HI_FALSE, NULL);
	  	if(dumylen > 0)
			BUF_Write(NULL, dumylen, HI_FALSE, NULL);
		dumyflag = 0;
		goto WriteDumy;

	}
	
    header[0] = '0' + (HI_U8)s32Chn;

	if(VS_FRAME_P == enFrameType)
	{
        header[1] = '1';		
	}
	else if(VS_FRAME_I == enFrameType)
	{
        bIFrame = HI_TRUE;
        header[1] = '0';
	}
	else
	{
        HI_Info("cbuf", LOG_LEVEL_ERR, "Wrong Frame type: %d, must be I or P.\n", (VS_FRAME_TYPE_E)enFrameType);
        return HI_EPAERM;		
	}
	
    *((HI_U32*)&header[8]) = u32Len;
	*((HI_U32*)&header[12]) = m_u32VideoPacketNo++;
    *((HI_U64*)&header[16]) = u64Pts;

	if(bIFrame == HI_TRUE)
	{
		#if 0
		header[24 + 0] = (pTime->s32Year%100);
		header[24 + 1] = pTime->s32Month;
		header[24 + 2] = pTime->s32Day;
		header[24 + 3] = pTime->s32Hour;
		header[24 + 4] = pTime->s32Minute;
		header[24 + 5] = pTime->s32Second;
		header[24 + 6] = (HI_U8)pTime->enWeek;
        /*填充够8个字节*/
		header[24 + 7] = 0;
		#endif
    	BUF_Write(header, sizeof(VIDEO_HEAD_S) , bIFrame, pTime);
	}
	else
	{
		BUF_Write(header, sizeof(VIDEO_HEAD_S) , bIFrame, pTime);
	}
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    /*写码流*/
    BUF_Write(pu8Start, u32Len, HI_FALSE, NULL);

    HI_U8 	tmp[8] = {0};
    HI_U32 	u32SetLen;
	HI_S32 	s32Ret;
	u32SetLen = u32Len%8;
    if(0 != u32SetLen)
    {
        u32SetLen = 8 - u32SetLen;
        s32Ret = BUF_Write(tmp, u32SetLen, HI_FALSE, NULL);
        if(HI_SUCCESS != s32Ret)
        {
    		printf("#### %s %d, err:%d\n", __FUNCTION__, __LINE__, s32Ret);
        }
    }

  	if(m_stBufferInfo.u32WritePos  + sizeof(header) > m_stBufferInfo.u32BufferSize) //尾部已经不足已放帧头信息
  	{
  		HI_U32 u32LeftLen = 0;
  		u32LeftLen  = m_stBufferInfo.u32BufferSize - m_stBufferInfo.u32WritePos;
  		s32Ret = BUF_Write(NULL, u32LeftLen, HI_FALSE, NULL);
        if(HI_SUCCESS != s32Ret)
        {
    		printf("#### %s %d, err:%d\n", __FUNCTION__, __LINE__, s32Ret);
        }
  		//printf("BUF_WriteVideoData clear the left buffer %d\n" , u32LeftLen);
  	}
    //printf("#### %s %d\n", __FUNCTION__, __LINE__);

    return HI_SUCCESS;
}


/*********************************************************************
    Function:     BUF_WriteAudioData
    Description:  写音频数据进Buffer
    Calls:
    Called By:
    parameter:
            [in] HI_U8 *pu8Start 起始地址
            [in] HI_U32 u32Len   长度
            [in] HI_S32 s32Chn  通道号
            [in] HI_U64 u64Pts  时间戳
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_WriteAudioData(const HI_U8 *pu8Start,
                                      HI_U32 u32Len,
                                      HI_S32 s32Chn,
                                      HI_U64 u64Pts)
{
    HI_U8 	header[sizeof(AUDIO_HEAD_S)] = {0};
    HI_U8 	dumHeader[sizeof(DUMMY_HEAD_S)] = {0};
    HI_U8  dumyflag = 0;
    HI_S32  s32Ret= 0;

WriteDumy:

 	if(m_stBufferInfo.u32WritePos + u32Len + sizeof(dumHeader) > m_stBufferInfo.u32BufferSize)  //buffer 剩余空间不足一帧，丢弃buffer内存   --vinsent
 	{
	    memcpy(dumHeader, "00dcdumy", 8);
	    dumyflag = 1;
 	}
 	else
 	{
	    (HI_VOID)memcpy(header, "00wb", 4);
	    dumyflag = 0;
	}

	if(m_stBufferInfo.u32WritePos  + sizeof(dumHeader) > m_stBufferInfo.u32BufferSize)  //如果剩余空间不足以放帧头信息
	{
		printf("left buffer small then head\n");
		return 1;
	}

	if(dumyflag)
	{
		HI_U32 dumylen = 0;
		*((HI_U32*)&dumHeader[8]) = dumylen = m_stBufferInfo.u32BufferSize - m_stBufferInfo.u32WritePos  - sizeof(dumHeader) ;
	  	BUF_Write(dumHeader, sizeof(dumHeader) , HI_FALSE, NULL);
	  	if(dumylen > 0)
			BUF_Write(NULL, dumylen, HI_FALSE, NULL);
		dumyflag = 0;
		goto WriteDumy;

	}

    header[0] = '0' + (HI_U8)s32Chn;

    /* 音频编码类型 */
    header[1] = '0' + (HI_U32)m_stStreamAttr.enCodecType;

    *((HI_U16*)&header[4]) = (HI_U16)u32Len;
    *((HI_U16*)&header[6]) = (HI_U16)u32Len;
    *((HI_U32*)&header[8]) = (HI_U32)m_u32AudioPacketNo++;
    *((HI_U64*)&header[16]) = u64Pts;
    /*写音频头*/

    BUF_Write(header, sizeof(AUDIO_HEAD_S), HI_FALSE, NULL);
	u32Len = ((u32Len + 7) / 8) * 8;
    /*写码流*/
    BUF_Write(pu8Start, u32Len, HI_FALSE, NULL);

    if(m_stBufferInfo.u32WritePos  + sizeof(dumHeader) > m_stBufferInfo.u32BufferSize) //尾部已经不足已放帧头信息
  	{
  		HI_U32 u32LeftLen = 0;
  		u32LeftLen  = m_stBufferInfo.u32BufferSize - m_stBufferInfo.u32WritePos;
  		s32Ret = BUF_Write(NULL, u32LeftLen, HI_FALSE, NULL);
        if(HI_SUCCESS != s32Ret)
        {
    		printf("#### %s %d, err:%d\n", __FUNCTION__, __LINE__, s32Ret);
        }
  		//printf("BUF_WriteVideoData clear the left buffer %d\n" , u32LeftLen);
  	}

    return HI_SUCCESS;
}

unsigned char  * CBuffer::BUF_GetWriteMemPos(HI_U32 u32Size)
{
    HI_U8 	dumHeader[sizeof(DUMMY_HEAD_S)] = {0};
    HI_U8  dumyflag = 0;
WriteDumy:
 	if(m_stBufferInfo.u32WritePos + u32Size  > m_stBufferInfo.u32BufferSize)  
 	{
	    memcpy(dumHeader, "00dcdumy", 8);
	    dumyflag = 1;
 	}

	if(dumyflag)
	{
		HI_U32 dumylen = 0;
		*((HI_U32*)&dumHeader[8]) = dumylen = m_stBufferInfo.u32BufferSize - m_stBufferInfo.u32WritePos  - sizeof(dumHeader) ;
	  	BUF_Write(dumHeader, sizeof(dumHeader) , HI_FALSE, NULL);
	  	if(dumylen > 0)
			BUF_Write(NULL, dumylen, HI_FALSE, NULL);
		dumyflag = 0;
		goto WriteDumy;

	}
	return m_stBufferInfo.pu8BufferStart + m_stBufferInfo.u32WritePos;
}


/*********************************************************************
    Function:     BUF_AddReader
    Description:  添加读指针
    Calls:
    Called By:
    parameter:
           [out]  HI_S32 *ps32ReaderId  返回读指针ID
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_AddReader(HI_S32 *ps32ReaderId)
{
    if (HI_FALSE == m_bBufCreate)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_AddReader Buffer not create!\n");
		*ps32ReaderId = -1;
        return HI_ENOTINIT;
    }
    if (NULL == ps32ReaderId)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_AddReader NULL ptr!\n");
        return HI_EPAERM;
    }

    int id = -1;
    pthread_mutex_lock(&m_BufLock);
    if (m_u32ReaderCnt >= BUF_MAX_READER_NUM)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_AddReader already max readers!\n");
		*ps32ReaderId = -1;
        pthread_mutex_unlock(&m_BufLock);
        return HI_EFULL;
    }
    for (id = 0; id < BUF_MAX_READER_NUM; id++)
    {
        if (HI_FALSE == m_abReaderUsed[id])
        {
            break;
        }
    }
    if (id < BUF_MAX_READER_NUM)
    {
        *ps32ReaderId = id;
        m_abReaderUsed[id] = HI_TRUE;
        m_u32ReaderCnt++;
        pthread_mutex_unlock(&m_BufLock);
        return HI_SUCCESS;
    }
    else
    {
        *ps32ReaderId = -1;
        pthread_mutex_unlock(&m_BufLock);
        return HI_FAILURE;
    }
}


/*********************************************************************
    Function:     BUF_DelReader
    Description:  删除读指针
    Calls:
    Called By:
    parameter:
           [in]  HI_S32 s32ReaderId  读指针ID
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_DelReader(HI_S32 s32ReaderId)
{
    if (HI_FALSE == m_bBufCreate)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_DelReader Buffer not create!\n");
        return HI_ENOTINIT;
    }
    if (s32ReaderId < 0 || s32ReaderId >= BUF_MAX_READER_NUM
        || HI_FALSE == m_abReaderUsed[s32ReaderId])
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_DelReader Invalid reader id!\n");
        return HI_EPAERM;
    }

    pthread_mutex_lock(&m_BufLock);
    if (0 == m_u32ReaderCnt)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_DelReader No readers yet!\n");
        pthread_mutex_unlock(&m_BufLock);
        return HI_FAILURE;
    }
    m_u32ReaderCnt--;
    m_au32ReadPos[s32ReaderId] = 0;
    m_au32ReadCycle[s32ReaderId] = 0;
    m_abReaderUsed[s32ReaderId] = HI_FALSE;
    m_u32ReadAudioPacketNo[s32ReaderId] = 0;
    m_u32ReadVideoPacketNo[s32ReaderId] = 0;
    pthread_mutex_unlock(&m_BufLock);

    return HI_SUCCESS;
}

/*********************************************************************
    Function:     BUF_GetReadPtrPos
    Description:  获取读指针位置
    Calls:
    Called By:
    parameter:
           [in] HI_S32 s32ReaderId  读指针ID
           [in] BUF_POSITION_POLICY_E enPolicy  获取读指针方式
           [in] HI_S32 s32N  相对I帧数
           [out] BUF_READPTR_INFO_S *pstReadptr 返回读指针位置信息
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_GetReadPtrPos(HI_S32 s32ReaderId,
                                     BUF_POSITION_POLICY_E enPolicy,
                                     HI_S32 s32N,
                                     BUF_READPTR_INFO_S *pstReadptr)
{
    HI_U32 u32ReadPos = 0;
    HI_U32 u32ReadCycle = 0;

    if (NULL == pstReadptr || enPolicy >= BUF_POSITION_BUTT
        || 0 == m_u32BlockSize)
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrPos Invalid para!\n");
        return HI_EPAERM;
    }
    if (HI_FALSE == m_bBufCreate)
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrPos Buffer not create!\n");
        return HI_ENOTINIT;
    }
    if (s32ReaderId < 0 || s32ReaderId >= BUF_MAX_READER_NUM
        || HI_FALSE == m_abReaderUsed[s32ReaderId])
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrPos Buffer not create!\n");
        return HI_EPAERM;
    }
    (HI_VOID)memset(pstReadptr, 0, sizeof(BUF_READPTR_INFO_S));

    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    HI_U32 u32WritePos,u32WriteCycle;
    u32WritePos   = m_stBufferInfo.u32WritePos;
    u32WriteCycle = m_stBufferInfo.u32WriteCycle;
    switch (enPolicy)
    {
        case BUF_POSITION_CUR_READ:
            break;

        /*从当前读指针找第N个I帧所在块的起始位置*/
        case BUF_POSITION_LAST_READ_nIFRAME:
        {
            HI_U32 u32ReadCycle = m_au32ReadCycle[s32ReaderId];
            HI_U32 u32ReadPos = m_au32ReadPos[s32ReaderId];
            HI_S32 s32INum = 0;
            HI_S32 i = 0;
            if (0 == s32N)
            {
                break;
            }
            /*找旧数据*/
            else if (s32N > 0)
            {
                /*读写指针在同一圈*/
                if (u32WriteCycle == u32ReadCycle)
                {
                    /*同一圈时,写指针在Buffer中位置比读指针大，否则，异常，数据被覆盖*/
                    if (u32WritePos >= u32ReadPos)
                    {
                        /*往前搜索到Buffer起止位置*/
                        for (i = u32ReadPos/m_u32BlockSize;
                             i >= 0;
                             i--)
                        {
                            s32INum += m_stBufferInfo.pstBlockInfo[i].u32IFrameNum;
                            if (s32INum >= s32N)
                            {
                                m_au32ReadPos[s32ReaderId] = i*m_u32BlockSize;
                                m_au32ReadCycle[s32ReaderId] = u32ReadCycle;
                                break;
                            }
                        }
                        if (i < 0)/*前面没有搜索到结果，继续往下搜索*/
                        {
                            /*从Buffer结束位置搜索到写位置*/
                            for (i = m_stBufferInfo.u32BufferSize/m_u32BlockSize-1;
                                 i > (HI_S32)(u32WritePos/m_u32BlockSize+1);
                                 i--)
                            {
                                s32INum += m_stBufferInfo.pstBlockInfo[i].u32IFrameNum;
                                if (s32INum >= s32N)
                                {
                                    m_au32ReadPos[s32ReaderId] = i*m_u32BlockSize;
                                    m_au32ReadCycle[s32ReaderId] = u32ReadCycle-1;
									break;
                                }
                            }
                            /*没有搜索到s32N个I块，则按照默认当前位置*/
							HI_Debug("Buffer", LOG_LEVEL_ERR,
								 "BUF_GetReadPtrPos nofind N, s32N:%d\n", s32N);
                        }
                    }
                    else
                    {
                        m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
                        m_au32ReadPos[s32ReaderId] = u32WritePos;
                        HI_Debug("Buffer", LOG_LEVEL_ERR,
                                 "BUF_GetReadPtrPos Readptr over writeptr!\n");
                        return HI_FAILURE;
                    }
                }
                /*写指针比读指针多一圈*/
                else if (1 == (u32WriteCycle - u32ReadCycle))
                {
                    /*写指针比读指针多一圈，读指针在Buffer中位置比写指针大，
                      否则，异常，数据被覆盖*/
                    if (u32ReadPos > u32WritePos)
                    {
                        for (i = u32ReadPos/m_u32BlockSize;
                             i > (HI_S32)(u32WritePos/m_u32BlockSize+1);
                             i--)
                        {
                            s32INum += m_stBufferInfo.pstBlockInfo[i].u32IFrameNum;
                            if (s32INum >= s32N)
                            {
                                m_au32ReadPos[s32ReaderId] = i*m_u32BlockSize;
                                m_au32ReadCycle[s32ReaderId] = u32ReadCycle;
                                break;
                            }
                        }
                        /*没有搜索到s32N个I块，则按照默认当前位置*/
                        HI_Debug("Buffer", LOG_LEVEL_ERR,
                             "BUF_GetReadPtrPos2 nofind N, s32N:%d\n", s32N);
                    }
                    else
                    {
                        m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
                        m_au32ReadPos[s32ReaderId] = u32WritePos;
                        HI_Debug("Buffer", LOG_LEVEL_ERR,
                                 "BUF_GetReadPtrPos2 Readptr over writeptr!!!\n");
                        return HI_FAILURE;
                    }
                }
                /*读写指针不在同一圈，或写指针比读指针不是多一圈，
                  则为异常情况，数据被覆盖*/
                else
                {
                    m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
                    m_au32ReadPos[s32ReaderId] = u32WritePos;
                    HI_Debug("Buffer", LOG_LEVEL_ERR,
                             "BUF_GetReadPtrPos Invalid cycle!\n");
                    return HI_FAILURE;
                }
            }
            /*找新数据*/
            else
            {
                s32N = 0 - s32N;
                if (u32WriteCycle == u32ReadCycle)
                {
                    if (u32WritePos >= u32ReadPos)
                    {
                        for (i = u32ReadPos/m_u32BlockSize;
                             i < (HI_S32)(u32WritePos/m_u32BlockSize);
                             i++)
                        {
                            s32INum += m_stBufferInfo.pstBlockInfo[i].u32IFrameNum;
                            if (s32INum >= s32N)
                            {
                                m_au32ReadPos[s32ReaderId] = i*m_u32BlockSize;
                                m_au32ReadCycle[s32ReaderId] = u32ReadCycle;
                                break;
                            }
                        }
                        /*没有搜索到s32N个I块，则按照当前默认位置*/
                        HI_Debug("Buffer", LOG_LEVEL_ERR,
                             "BUF_GetReadPtrPos3 nofind N, s32N:%d\n", s32N);
                    }
                    /*同一圈写指针不在读指针前面，异常，数据被覆盖*/
                    else
                    {
                        m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
                        m_au32ReadPos[s32ReaderId] = u32WritePos;
                        HI_Debug("Buffer", LOG_LEVEL_ERR,
                                 "BUF_GetReadPtrPos3 Readptr NewData over writeptr!\n");
                        return HI_FAILURE;
                    }
                }
                else if (1 == (u32WriteCycle - u32ReadCycle))
                {
                    /*写指针比读指针多一圈，读指针在Buffer中位置比写指针大，
                      否则，异常，数据被覆盖*/
                    if (u32ReadPos > u32WritePos)
                    {
                        for (i = u32ReadPos/m_u32BlockSize;
                             i < (HI_S32)(m_stBufferInfo.u32BufferSize/m_u32BlockSize);
                             i++)
                        {
                            s32INum += m_stBufferInfo.pstBlockInfo[i].u32IFrameNum;
                            if (s32INum >= s32N)
                            {
                                m_au32ReadPos[s32ReaderId] = i*m_u32BlockSize;
                                m_au32ReadCycle[s32ReaderId] = u32ReadCycle;
                                break;
                            }
                        }
                        /*继续搜索直到写指针*/
                        if (i >= (HI_S32)(m_stBufferInfo.u32BufferSize/m_u32BlockSize))
                        {
                            for (i = 0;
                                 i < (HI_S32)(u32WritePos/m_u32BlockSize);
                                 i++)
                            {
                                s32INum += m_stBufferInfo.pstBlockInfo[i].u32IFrameNum;
                                if (s32INum >= s32N)
                                {
                                    m_au32ReadPos[s32ReaderId] = i*m_u32BlockSize;
                                    m_au32ReadCycle[s32ReaderId] = u32ReadCycle+1;
                                    break;
                                }
                            }
                        }
                        /*没有搜索到s32N个I块，则按照默认当前位置*/
                        HI_Debug("Buffer", LOG_LEVEL_ERR,
                             "BUF_GetReadPtrPos5 nofind N, s32N:%d\n", s32N);
                    }
                    else
                    {
                        m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
                        m_au32ReadPos[s32ReaderId] = u32WritePos;
                        HI_Debug("Buffer", LOG_LEVEL_ERR,
                                 "BUF_GetReadPtrPos5 NewData Readptr over writeptr!!!\n");
                        return HI_FAILURE;
                    }
                }
                else
                {
                    m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
                    m_au32ReadPos[s32ReaderId] = u32WritePos;
                    HI_Debug("Buffer", LOG_LEVEL_ERR,
                             "BUF_GetReadPtrPos New Data Invalid cycle!\n");
                    return HI_FAILURE;
                }
            }
            break;
        }
        case BUF_POSITION_CUR_WRITE:
            m_au32ReadPos[s32ReaderId]   = u32WritePos;
            m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
            break;

        case BUF_POSITION_LAST_WRITE_nBLOCK:
            if (s32N != 0)/*暂时只支持当前写指针所在的块的首地址*/
            {
                HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrPos Not support yet!\n");
                pthread_mutex_unlock(&m_BufLock);
                return -1;
            }
            m_au32ReadPos[s32ReaderId]   = u32WritePos / m_u32BlockSize * m_u32BlockSize;
            m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
            break;

        case BUF_POSITION_LAST_WRITE_nIFRAME:
            u32ReadPos = 0;
            u32ReadCycle = 0;
            if (GetLastWriteNIFramPos(s32N, m_stIFrameManager,
                &u32ReadPos, &u32ReadCycle) < 0)
            {
                return HI_FAILURE;
            }
            m_au32ReadPos[s32ReaderId] = u32ReadPos;
            m_au32ReadCycle[s32ReaderId] = u32ReadCycle;
            break;

        case BUF_POSITION_BUTT:
        default:
            HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrPos Not support yet!\n");
            return HI_EPAERM;
            break;
    }

    pstReadptr->pu8ReadPos = (HI_U8*)(m_stBufferInfo.pu8BufferStart	+ m_au32ReadPos[s32ReaderId]);
    pstReadptr->u32ReadOffset = m_au32ReadPos[s32ReaderId];
    pstReadptr->u32BufferSize = m_stBufferInfo.u32BufferSize;
    if(BUF_POSITION_LAST_READ_nIFRAME == enPolicy || BUF_POSITION_LAST_WRITE_nIFRAME == enPolicy)
    {
	    (HI_VOID)memcpy(&pstReadptr->stBlockInfo, &m_stBufferInfo.pstBlockInfo[m_au32ReadPos[s32ReaderId] / m_u32BlockSize], sizeof(BUF_BLOCK_INFO_S));
	    /*更新读指针码流属性*/
	    if (pstReadptr->stBlockInfo.u32IFrameNum > 0)
	    {
	        (HI_VOID)memcpy(&m_astReaderStreamAttr[s32ReaderId], &pstReadptr->stBlockInfo.astIFrame[0].stStreamAttr,sizeof(BUF_STREAM_ATTR_S));    
		}
	}

    return HI_SUCCESS;
}


/*********************************************************************
    Function:     BUF_CanRead
    Description:  数据是否能读
    Calls:
    Called By:
    parameter:
           [in] HI_S32 s32ReaderId  读指针ID
           [in] HI_U32 u32Len 长度
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_CanRead(HI_S32 s32ReaderId, HI_U32 u32Len)
{
    if (s32ReaderId < 0 || s32ReaderId >= BUF_MAX_READER_NUM 
        || HI_FALSE == m_abReaderUsed[s32ReaderId])
    {
        printf("#### %s %d\n", __FUNCTION__, __LINE__);
        HI_Debug("Buffer",
                 LOG_LEVEL_ERR,
                 "BUF_CanRead Invalid reader id:%d,%d!\n", s32ReaderId, m_abReaderUsed[s32ReaderId]);
        return HI_EPAERM;
    }

    HI_S32 s32RetLen = 0;
    HI_U32 u32WritePos,u32WriteCycle;

	//[2011-6-18 14:18:31]  注释掉互斥锁
    u32WritePos   = m_stBufferInfo.u32WritePos;
    u32WriteCycle = m_stBufferInfo.u32WriteCycle;
//	printf("u32WritePos %d , u32WriteCycle %d ,m_au32ReadCycle[%d] %d ,m_au32ReadPos[%d] %d\n" , u32WritePos , 
//	u32WriteCycle  , s32ReaderId , m_au32ReadCycle[s32ReaderId]  ,  s32ReaderId ,m_au32ReadPos[s32ReaderId] );
    if (u32WriteCycle == m_au32ReadCycle[s32ReaderId])
    {
        if (u32WritePos >= m_au32ReadPos[s32ReaderId])
        {
			//返回可用数据长
            s32RetLen = ((u32WritePos - m_au32ReadPos[s32ReaderId]) > u32Len) ? u32Len :(u32WritePos - m_au32ReadPos[s32ReaderId]);
        }
        else
        {
            HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_CanRead Readptr over writeptr!\n");
            s32RetLen =-3;
        }
    }
    else if ((u32WriteCycle - m_au32ReadCycle[s32ReaderId]) == 1)
    {
		s32RetLen = ((m_stBufferInfo.u32BufferSize - m_au32ReadPos[s32ReaderId])  > u32Len) \
					? u32Len :(m_stBufferInfo.u32BufferSize - m_au32ReadPos[s32ReaderId]);
    }
    else
    {
		printf("BUF_CanRead Invalid position 2! s32ReaderId %d u32WritePos:%d, u32WriteCycle:%d, m_au32ReadPos[%d]:%d, m_au32ReadCycle[%d]:%d\n", 
					s32ReaderId , u32WritePos, u32WriteCycle, 
					s32ReaderId, m_au32ReadPos[s32ReaderId], 
					s32ReaderId, m_au32ReadCycle[s32ReaderId]);

        m_au32ReadCycle[s32ReaderId] = u32WriteCycle;
        m_au32ReadPos[s32ReaderId] = u32WritePos;
        //HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_CanRead Invalid position!\n");
        s32RetLen = -4;
    }

    //printf("#### %s %d\n", __FUNCTION__, __LINE__);
    return s32RetLen;
}


/*********************************************************************
    Function:     BUF_SetReadPtrPos
    Description:  设置读指针位置
    Calls:
    Called By:
    parameter:
           [in] HI_S32 s32ReaderId  读指针ID
           [in] [in] HI_U32 u32Len   长度
    Return: HI_SUCCESS
    author:
********************************************************************/
HI_S32 CBuffer::BUF_SetReadPtrPos(HI_S32 s32ReaderId, HI_U32 u32Len)
{
    if (HI_FALSE == m_bBufCreate)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_SetReadPtrPos Buffer not create!\n");
        return HI_ENOTINIT;
    }
    if (s32ReaderId < 0 || s32ReaderId >= BUF_MAX_READER_NUM || HI_FALSE == m_abReaderUsed[s32ReaderId])
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_SetReadPtrPos Invalid reader id!\n");
        return HI_EPAERM;
    }

    if ((m_au32ReadPos[s32ReaderId] + u32Len) >= m_stBufferInfo.u32BufferSize)
    {
        m_au32ReadPos[s32ReaderId] = m_au32ReadPos[s32ReaderId] + u32Len - m_stBufferInfo.u32BufferSize;
        m_au32ReadCycle[s32ReaderId]++;
       
    }
    else
    {
        m_au32ReadPos[s32ReaderId] += u32Len;
    }
    
	//更新读写圈数
	if( m_stBufferInfo.u32WriteCycle - m_au32ReadCycle[s32ReaderId] > 10 )
	{
		if(m_au32ReadPos[s32ReaderId] < m_stBufferInfo.u32WritePos)
			m_au32ReadCycle[s32ReaderId] = m_stBufferInfo.u32WriteCycle;
		else
			m_au32ReadCycle[s32ReaderId] = m_stBufferInfo.u32WriteCycle -1;
	}

    return HI_SUCCESS;
}


/*********************************************************************
    Function:   BUF_GetReadPtrLeftNum
    Description:获取读指针与写指针之间I帧个数与块个数
                以用来协助外部的播放控制或网传控制
    Calls:
    Called By:
    parameter:
            [in] HI_S32 s32ReaderId  读指针ID
            [out] HI_U32 *pu32LeaveCount 剩余数据量
    Return: HI_SUCCESS
    Author:
********************************************************************/
HI_S32 CBuffer::BUF_GetReadPtrLeftNum(HI_S32 s32ReaderId,HI_U32 *pu32LeaveCount)
{
    if (HI_FALSE == m_bBufCreate)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrLeftNum Buffer not create!\n");
        return HI_ENOTINIT;
    }
    if (s32ReaderId < 0 || s32ReaderId >= BUF_MAX_READER_NUM
        || HI_FALSE == m_abReaderUsed[s32ReaderId]
        || NULL == pu32LeaveCount
        || 0 == m_u32BlockSize)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrLeftNum Invalid reader id!\n");
        return HI_EPAERM;
    }

    *pu32LeaveCount = 0;
    if (0 == m_u32BlockSize)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReadPtrLeftNum m_u32BlockSize = 0\n");
        return HI_FAILURE;
    }

    /*找到读指针离写指针有多少个I帧*/
    HI_U32 u32WritePos   = m_stBufferInfo.u32WritePos;
    HI_U32 u32WriteCycle = m_stBufferInfo.u32WriteCycle;
    HI_U32 u32ReadCycle = m_au32ReadCycle[s32ReaderId];
    HI_U32 u32ReadPos	= m_au32ReadPos[s32ReaderId];

    if (u32WriteCycle == u32ReadCycle)
    {
        if (u32WritePos >= u32ReadPos)
        {
            *pu32LeaveCount = u32WritePos - u32ReadPos;
        }
        else
        {
            HI_Debug("Buffer", LOG_LEVEL_ERR,"BUF_GetReadPtrLeftNum Readptr over writeptr!\n");
            return HI_FAILURE;
        }
    }
    else if ((u32WriteCycle - u32ReadCycle) == 1)
    {
        *pu32LeaveCount = m_stBufferInfo.u32BufferSize - u32ReadPos + u32WritePos;
    }
    else
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR,"BUF_GetReadPtrLeftNum Invalid cycle!\n");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}


/*********************************************************************
    Function:   BUF_GetReaderStreamAttr
    Description:获取读指针的码流属性
    Calls:
    Called By:
    parameter:
            [in] s32ReaderId  读指针ID
            [out] pstStreamAttr  返回码流属性
    Return: HI_SUCCESS
    Author:
********************************************************************/
HI_S32 CBuffer::BUF_GetReaderStreamAttr(HI_S32 s32ReaderId,
                                           BUF_STREAM_ATTR_S *pstStreamAttr)
{
    /*输入参数检查*/
    if (HI_FALSE == m_bBufCreate)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReaderStreamAttr Buffer not create!\n");
        return HI_ENOTINIT;
    }
    if (s32ReaderId < 0 || s32ReaderId >= BUF_MAX_READER_NUM
        || HI_FALSE == m_abReaderUsed[s32ReaderId])
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReaderStreamAttr Invalid reader id!\n");
        return HI_EPAERM;
    }
    if (NULL == pstStreamAttr)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetReaderStreamAttr para erro!\n");
        return HI_EPAERM;
    }
    *pstStreamAttr = m_astReaderStreamAttr[s32ReaderId];
    return HI_SUCCESS;
}


/*********************************************************************
    Function:   BUF_SetStreamAttr
    Description:设置Buffer码流属性
    Calls:
    Called By:
    parameter:
            [in] stStreamAttr  码流属性
    Return: HI_SUCCESS
    Author:
********************************************************************/
HI_S32 CBuffer::BUF_SetStreamAttr(const BUF_STREAM_ATTR_S *pstStreamAttr)
{
    /*输入参数检查*/
    if (NULL == pstStreamAttr)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_SetStreamAttr para erro!\n");
        return HI_EPAERM;
    }

    m_stStreamAttr = *pstStreamAttr;
    return HI_SUCCESS;
}


/*********************************************************************
    Function:   BUF_GetStreamAttr
    Description:获取码流属性
    Calls:
    Called By:
    parameter:
            [out] pstStreamAttr  返回码流属性
    Return: HI_SUCCESS
    Author:
********************************************************************/
HI_U32 CBuffer::BUF_GetStreamAttr(BUF_STREAM_ATTR_S *pstStreamAttr)
{
    /*输入参数检查*/
    if (NULL == pstStreamAttr)
    {
        HI_Debug("Buffer", LOG_LEVEL_ERR, "BUF_GetStreamAttr para erro!\n");
        return HI_EPAERM;
    }

    *pstStreamAttr = m_stStreamAttr;
    return HI_SUCCESS;
}


HI_U32 CBuffer::BUF_GetBufPhyAddr()
{
    return m_stBufferInfo.pu8BufferStartPhy;
}

HI_U8 * CBuffer::BUF_GetBufAddr()
{
    return m_stBufferInfo.pu8BufferStart;
}

HI_U32 CBuffer::BUF_GetBufSize()
{
    return m_stBufferInfo.u32BufferSize;
}


HI_S32  CBuffer::BUF_CheckReaderPacketNo(HI_S32 s32ReaderId , HI_BOOL bVideo ,  HI_U32 u32PacketNo)
{
	if(bVideo)
	{
		if( 0 == m_u32ReadVideoPacketNo[s32ReaderId ])
		{
			m_u32ReadVideoPacketNo[s32ReaderId ] = u32PacketNo;
		}
		else
		{	
			if(++m_u32ReadVideoPacketNo[s32ReaderId ] != u32PacketNo)
			{
				printf("VSBUF Lost  Video Packet ReaderId %d CurPacketNo %d Shoud Be %d\n" , 
				s32ReaderId , u32PacketNo , m_u32ReadVideoPacketNo[s32ReaderId]);
				m_u32ReadVideoPacketNo[s32ReaderId ] = u32PacketNo;
			}
		}
	}
	else
	{
		if( 0 == m_u32ReadAudioPacketNo[s32ReaderId ])
		{
			m_u32ReadAudioPacketNo[s32ReaderId ] = u32PacketNo;
		}
		else
		{	
			if(++m_u32ReadAudioPacketNo[s32ReaderId ] != u32PacketNo)
			{
				printf("VSBUF Lost  Audio Packet ReaderId %d CurPacketNo %d Shoud Be %d\n" , 
				s32ReaderId , u32PacketNo , m_u32ReadAudioPacketNo[s32ReaderId]);
				m_u32ReadAudioPacketNo[s32ReaderId ] = u32PacketNo;
			}
		}
	}

	return 0;
		
}



