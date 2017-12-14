#ifndef __VS_BUFFER_H__
#define __VS_BUFFER_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/**最大输入块大小*/
#define BUF_MAX_IFRAME_IN_BLOCK 128

#define MAX_CHANNEL_NUM 16

/**最大读指针个数*/    
#define BUF_MAX_READER_NUM 8   


typedef enum tagVS_FRAME_TYPE_E
{
    VS_FRAME_I, /**<I帧*/
    VS_FRAME_P, /**<P帧*/
    VS_FRAME_B, /**<B帧*/
    VS_FRAME_A, /**<音频帧*/
    VS_FRAME_BUTT /**<保留值*/
    
}VS_FRAME_TYPE_E;

typedef enum tagVS_AUDIO_FORMAT_E
{
    VS_AUDIO_FORMAT_G711A   = 1,   /* G.711 A            */
    VS_AUDIO_FORMAT_G711Mu  = 2,   /* G.711 Mu           */
    VS_AUDIO_FORMAT_ADPCM   = 3,   /* ADPCM              */
    VS_AUDIO_FORMAT_G726    = 4,   /* G.726              */
    VS_AUDIO_FORMAT_AMR     = 5,   /* AMR encoder format */
    VS_AUDIO_FORMAT_AMRDTX  = 6,   /* AMR encoder formant and VAD1 enable */
    VS_AUDIO_FORMAT_AAC     = 7,   /* AAC encoder        */
    VS_AUDIO_FORMAT_BUTT
}VS_AUDIO_FORMAT_E;

typedef enum tagVS_STREAM_TYPE_E
{
    VS_STREAM_TYPE_MAIN = 0,    /**< 主码流 */
    VS_STREAM_TYPE_NETSUB,  /**< 网络子码流 */
    VS_STREAM_TYPE_SNAP,    /**< 抓拍码流 */
    VS_STREAM_TYPE_MOBILE,  /**< 手机码流 */
    VS_STREAM_TYPE_IP,      /**< IP码流*/
    VS_STREAM_TYPE_BUTT     /**< 非法值*/
}VS_STREAM_TYPE_E;


/*星期*/
typedef enum tagVS_WEEKDAY_E
{
    VS_WEEKDAY_SUNDAY,   /**<星期天*/
    VS_WEEKDAY_MONDAY,   /**<星期一*/
    VS_WEEKDAY_TUESDAY,  /**<星期二*/
    VS_WEEKDAY_WEDNESDAY,/**<星期三*/
    VS_WEEKDAY_THURSDAY, /**<星期四*/
    VS_WEEKDAY_FRIDAY,   /**<星期五*/
    VS_WEEKDAY_SATURDAY  /**<星期六*/
}VS_WEEKDAY_E;


/**日期时间*/
typedef struct tagVS_DATETIME_S
{
    int s32Year;   /**<年*/
    int s32Month;  /**<月*/
    int s32Day;    /**<日*/
    int s32Hour;   /**<时*/
    int s32Minute; /**<分*/
    int s32Second; /**<秒*/
    VS_WEEKDAY_E enWeek; /**<星期,详见::VS_WEEKDAY_E*/
}VS_DATETIME_S;

typedef enum tagPOSITION_POLICY_E
{
    BUF_POSITION_CUR_READ = 0,/**<当前读指针的位置*/
    BUF_POSITION_LAST_READ_nIFRAME,/**<离当前读指针最近的第n个I帧所在的块的起始位置*/
    BUF_POSITION_CUR_WRITE,/**<当前写指针的位置*/
    BUF_POSITION_LAST_WRITE_nBLOCK,/**<离当前写指针最近的第n个块的起始位置*/
    BUF_POSITION_LAST_WRITE_nIFRAME,/**<离当前写指针最近的第n个I帧所在的块的起始位置, n=0表示最近的I帧*/
    BUF_POSITION_BUTT
}BUF_POSITION_POLICY_E;

typedef enum tagVS_TVSYSTEM_E
{
    VS_TVSYSTEM_PAL,    /**<PAL*/
    VS_TVSYSTEM_NTSC,   /**<NTSC*/
    VS_TVSYSTEM_BUTT /**<非法值*/
}VS_TVSYSTEM_E;

/**码流属性*/
typedef struct tagBUF_STREAM_ATTR_S
{
    VS_TVSYSTEM_E enNorm;/**制式*/
    unsigned int u32Width;
    unsigned int u32Height;
    unsigned int u32Fps; /**帧率*/
    unsigned int u32Bps; /**码率*/
    unsigned int u32Gop; /**编码I帧间隔 */
    VS_AUDIO_FORMAT_E enCodecType;  /**<音频类型*/
}BUF_STREAM_ATTR_S;

typedef struct tagIFRAME_INFO_S
{
    unsigned int u32Offset;/**<I帧的块内偏移 <unit:Byte*/
    BUF_STREAM_ATTR_S stStreamAttr; /**<码流属性*/
}BUF_IFRAME_INFO_S;

/**缓冲区各个块(64K一个块)的相关信息*/
typedef struct tagBLOCK_INFO_S
{
    unsigned int u32Cycle;/**<BLOCK的圈数*/
    unsigned int u32IFrameNum;/**<一个块内的I帧的个数*/
    unsigned int u32FrameNum;/**<一个块内帧个数*/
    BUF_IFRAME_INFO_S astIFrame[BUF_MAX_IFRAME_IN_BLOCK];/**<I帧信息*/
} BUF_BLOCK_INFO_S;

/**读指针信息*/
typedef struct tagREADPTR_INFO_S
{
    unsigned char *pu8ReadPos;/**<读指针*/
    unsigned int u32ReadOffset; //读偏移
    unsigned int u32BufferSize;
    BUF_BLOCK_INFO_S stBlockInfo;/**<读指针所在的块信息*/
}BUF_READPTR_INFO_S;

/*视频头*/
typedef struct hiVIDEO_HEAD_S
{
    unsigned int u32FrameType;    /*I帧为x0dc，x在第一个字节，0为第二个字节，d为第三个字节，c为第四个字节，p 帧为x1dc*/
    unsigned int u32StreamType;   /*流格式：暂时固定为H264，ASCII*/
    unsigned int u32FrameLen;     /*仅视频帧数据长度*/
    unsigned int u32PacketNo;    
    unsigned long long u64Pts;          /*时间戳*/
    unsigned char   u8Time[8];
} VIDEO_HEAD_S;

typedef struct hiDUMMY_HEAD_S  //this struct must tightly align with video header
{
    unsigned int u32FrameType;    /*I帧为x0dc，x在第一个字节，0为第二个字节，d为第三个字节，c为第四个字节，p 帧为x1dc*/
    unsigned int u32StreamType;   /*流格式：暂时固定为H264，dum frame 为dumy ASCII*/
    unsigned int u32FrameLen;     /*仅视频帧数据长度*/
	unsigned int u32PacketNo;    
    unsigned long long  u64Pts;          /*时间戳*/
    unsigned char  u8Align[8];    //align with Video header;
} DUMMY_HEAD_S;

/*音频头*/
typedef struct hiAUDIO_HEAD_S
{
    unsigned int u32FrameType;    /*音频帧标识*/
    unsigned short u16FrameLen;     /*一个音频帧长度*/
    unsigned short u16PacketLen;    /*此包音频数据长度*/
    unsigned int u32PacketNo;
    unsigned int u32Reserved;
    unsigned long long  u64Pts;          /*时间戳*/
} AUDIO_HEAD_S;

/*----------------------------------------------*
* 函数声明
*----------------------------------------------*/

/** @defgroup buffer_API_routine_prototypes buffer_API模块
 *  @ingroup H1
 *  @brief 详细描述模块(buffer_API)的函数定义
 *  @{  */

/**
 \brief 初始化Buf
 \attention \n
无
 \retval ::HI_SUCCESS
 \see \n
    ::VS_BUF_Exit
 */
int VS_BUF_Init();



int VS_BUF_Exit();

/**
 \brief 创建一个码流Buffer
 \attention \n
    一般在创建编码通道、创建网络接收通道时创建
    一个通道号和码流类型对应一个Buffer
 \param[in] s32Chn 源通道号
 \param[in] enStreamType 码流类型
 \param[in] u32BlockSize 块大小
 \param[in] u32BlockNum 块数

 \retval ::HI_SUCCESS

 \see \n
    ::VS_BUF_Destroy
 */
int VS_BUF_Create(const int s32Chn,
                     const VS_STREAM_TYPE_E enStreamType,
                     const unsigned int u32BlockSize,
                     const unsigned int u32BlockNum);

/**
 \brief 销毁一个码流Buffer
 \attention \n
无
 \param[in] s32Chn 源通道号
 \param[in] enStreamType 码流类型
 \retval ::HI_SUCCESS  成功
 \see \n
    ::VS_BUF_Create
 */
int VS_BUF_Destroy(const int s32Chn,
                      const VS_STREAM_TYPE_E enStreamType);

 /**
 \brief 写帧数据进Buffer
 \attention \n
        通常使用VS_BUF_WriteFrame输入原始裸码流帧数据，包括视频帧、音频帧
        用VS_BUF_GetFrame获取音视频原始裸码流帧，使用完毕之后再调用VS_BUF_SetFrame
        其中BUF_Write输入数据为内部编码使用
        BUF_GetReadPtrPos、BUF_CanRead、BUF_SetReadPtrPos读数据为内部使用
 \param[in] s32Chn 源通道号
 \param[in] enStreamType 码流类型
 \param[in] HI_VS_VIDEO_FRAME_TYPE_E enFrameType 帧类型
 \param[in] unsigned char *pu8Start 起始地址
 \param[in] unsigned int u32Len   长度
 \param[in] unsigned long long u64Pts  时间戳
 \param[in] VS_DATETIME_S *pTime  时间
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
    ::VS_BUF_GetFrame
 */
int VS_BUF_WriteFrame(const int s32Chn,
                         const VS_STREAM_TYPE_E enStreamType,
                         VS_FRAME_TYPE_E enFrameType,
                         const unsigned char *pu8Start,
                         unsigned int u32Len,
                         unsigned long long u64Pts,
                         VS_DATETIME_S *pTime);


 /**
 \brief 添加读指针
 \attention \n
       一个Buffer支持多个读指针操作
 \param[in] s32Chn 源通道号
 \param[in] enStreamType 码流类型
 \param[out] int *ps32ReaderId  返回读指针ID
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
    ::VS_BUF_DelReader
 */
int VS_BUF_AddReader(const int s32Chn,
                       const VS_STREAM_TYPE_E enStreamType,
                       int *ps32ReaderId);

 /**
 \brief 删除读指针
 \attention \n
 \param[in] int s32ReaderId  读指针ID
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
    ::VS_BUF_AddReader
 */
int VS_BUF_DelReader(int s32ReaderId);



 /**
 \brief 获取帧数据
 \attention \n
        从Buffer中获取帧数据，返回音频或视频原始裸码流帧数据，
 \param[in] int s32ReaderId  读指针ID
 \param[in] VS_BUF_POSITION_POLICY_E enPolicy  获取帧方式
 \param[in] int s32N  相对帧数
 \param[out] VS_FRAME_TYPE_E *penFrameType 返回帧类型
 \param[in] unsigned char *pu8Start   外面输入数据缓存起始地址
 \param[in/out] unsigned int *pu32Len  外面输入缓存长度，返回实际帧数据长度
 \param[out] unsigned long long *pu64Pts  时间戳
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
         ::BUF_NO_DATA Buffer中暂时无数据，需要外面再重新获取
 \see \n
    ::VS_BUF_SetFrame
 */
int VS_BUF_GetFrame(int s32ReaderId,
                       BUF_POSITION_POLICY_E enPolicy,
                       int s32N,
                       VS_FRAME_TYPE_E *penFrameType,
                       unsigned char *pu8Start,
                       unsigned int *pu32Len,
                       unsigned long long *pu64Pts,
                       unsigned int  *pu32PacketNo , 
                       int * ps32Width , int * ps32Height ,
                       unsigned char u8IOblock);


 /**
 \brief 获取读指针与写指针之间帧个数、I帧个数与块个数
 \attention \n
 获取当前读指针距离写指针剩余I帧个数与块个数，
 以用来协助外部的播放控制或网传控制
 \param[in] int s32ReaderId  读指针ID
 \param[out] unsigned int *pu32FrameNum  帧个数
 \param[out] unsigned int *pu32IFrameNum  I帧个数
 \param[out] unsigned int *pu32BlockNum   块个数
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
    ::VS_BUF_GetFrame
 */
int VS_BUF_GetReadPtrLeftNum(int s32ReaderId,
							 	unsigned int *pu32BlockNum);
/*
int VS_BUF_GetReadPtrLeftNum(int s32ReaderId,
                                unsigned int *pu32FrameNum,
                                unsigned int *pu32IFrameNum,
                                unsigned int *pu32LeaveCount,
                                unsigned long long *pu64Pts);
*/

 /**
 \brief 获取读指针的码流属性
 \attention \n
 \param[in] int s32ReaderId  读指针ID
 \param[out] BUF_STREAM_ATTR_S pstStreamAttr  返回码流属性
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
 */
int VS_BUF_GetReaderStreamAttr(int s32ReaderId,
                                  BUF_STREAM_ATTR_S *pstStreamAttr);
 
 /**
 \brief 设置Buffer码流属性
 \attention \n
 \param[in] s32Chn 源通道号
 \param[in] enStreamType 码流类型
 \param[in] BUF_STREAM_ATTR_S stStreamAttr  码流属性
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
 */
int VS_BUF_SetStreamAttr(const int s32Chn,
                            const VS_STREAM_TYPE_E enStreamType,
                            const BUF_STREAM_ATTR_S *pstStreamAttr);


 /**
 \brief 获取码流属性
 \attention \n
 \param[in] s32Chn 源通道号
 \param[in] enStreamType 码流类型
 \param[out] BUF_STREAM_ATTR_S pstStreamAttr  返回码流属性
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
 */
int VS_BUF_GetStreamAttr(const int s32Chn,
                            const VS_STREAM_TYPE_E enStreamType,
                            BUF_STREAM_ATTR_S *pstStreamAttr);
 
 /**
 \brief 获取读指针位置
 \attention \n
 \param[in] int s32ReaderId  读指针ID
 \param[in] VS_BUF_POSITION_POLICY_E enPolicy  获取读指针方式
 \param[in] int s32N  相对I帧数
 \param[out] VS_BUF_READPTR_INFO_S *pstReadptr 返回读指针位置信息
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
    ::BUF_CanRead
    ::BUF_SetReadPtrPos
 */
int VS_BUF_GetReadPtrPos(int s32ReaderId,
                           BUF_POSITION_POLICY_E enPolicy,
                           int s32N,
                           BUF_READPTR_INFO_S *pstReadptr);

 /**
 \brief 数据是否能读
 \attention \n
 \param[in] int s32ReaderId  读指针ID
 \param[in] unsigned int u32Len   长度
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
    ::BUF_GetReadPtrPos
 */
int VS_BUF_CanRead(int s32ReaderId,
                      unsigned int u32Len);

 /**
 \brief 设置读指针位置
 \attention \n
 \param[in] int s32ReaderId  读指针ID
 \param[in] unsigned int u32Len   长度
 \retval ::HI_SUCCESS  成功
         ::HI_EPAERM   输入参数错误
         ::HI_ENOTINIT Buffer未创建
 \see \n
    ::BUF_GetReadPtrPos
 */
int VS_BUF_SetReadPtrPos(int s32ReaderId,
                            unsigned int u32Len);

 /**
 \brief 获取BUF物理地址
 \attention \n
 \param[in] int s32Chn  通道号
 \param[in] VS_STREAM_TYPE_E enStreamType 码流类型
 \retval 
         ::BUF物理地址   
         ::NULL 失败
 \see \n
    ::VS_BUF_GetBufPhyAddr
 */
unsigned int VS_BUF_GetBufPhyAddr(const int s32Chn,const VS_STREAM_TYPE_E enStreamType);

 /**
 \brief 获取BUF大小
 \attention \n
 \param[in] int s32Chn  通道号
 \param[in] VS_STREAM_TYPE_E enStreamType 码流类型
 \retval 
         ::BUF大小
         ::0 失败
 \see \n
    ::VS_BUF_GetBufSize
 */

unsigned int VS_BUF_GetBufSize(const int s32Chn,const VS_STREAM_TYPE_E enStreamType);

 /**
 \brief 获取一帧数据
 \attention \n
 \返回一帧数据的指针
 \param[in] int s32ReaderId  readerid
\param[in] BUF_POSITION_POLICY_E enPolicy 获取帧方式
\param[in] int  s32N 相对帧数
\param[out] VS_FRAME_TYPE_E *penFrameType 返回帧类型
\param[out] unsigned char **pu8Start 返回帧地址
\param[out] unsigned int * Pu8StartOffset 返回帧相对地址
\param[out] unsigned int *pu32Len 返回帧长度
\param[out] unsigned long long *pu64Pts  返回帧时间戳
\param[out] unsigned int * pu32PacketNo 返回帧序号
\param[out] int * ps32Width 返回帧宽度
\param[out] int * ps32Height  返回帧高度
\param[in] unsigned char u8IOblock 是否阻塞获取
 \retval 
         :: HI_SUCCESS  成功
         ::其他 失败
 \see \n
    ::VS_BUF_GetFramePtr
 */

  int VS_BUF_GetFramePtr(int s32ReaderId, BUF_POSITION_POLICY_E enPolicy,int s32N,VS_FRAME_TYPE_E *penFrameType,unsigned char **pu8Start,
  unsigned int * Pu8StartOffset,unsigned int *pu32Len,unsigned long long *pu64Pts , unsigned int * pu32PacketNo , int * ps32Width , int * ps32Height ,unsigned char u8IOblock);

   /**
 \brief 获取可写内存指针
 \attention \n
 \返可写内存指针，与VS_BUF_ReleaseWriteMemPos成对使用
 \param[in] int s32Chn 通道号
\param[in] VS_STREAM_TYPE_E enStreamType 码流类型
\param[in] unsigned int u32Size 请求内存大小
 \retval 
         :: 可写内存指针
         ::0 失败
 \see \n
    ::VS_BUF_GetWriteMemPos
 */

unsigned char * VS_BUF_GetWriteMemPos(const int s32Chn,const VS_STREAM_TYPE_E enStreamType , unsigned int u32Size);

   /**
 \brief 释放可写内存指针
 \attention \n
 \释放可写内存指针，写指针往前移动一帧并填充帧头信息(但不拷贝帧数据)与VS_BUF_GetWriteMemPos配套使用
 \如果u32Len = 0 则不填充帧头信息
 \param[in] int s32Chn 通道号
\param[in] VS_STREAM_TYPE_E enStreamType 码流类型
\param[in] VS_FRAME_TYPE_E enFrameType 帧类型
\param[in] unsigned int u32Len 帧大小
\param[in] unsigned long long u64Pts 时间戳
\param[in] VS_DATETIME_S *pTime 时间信息

 \retval 
         :: 可写内存指针
         ::0 失败
 \see \n
    ::VS_BUF_GetWriteMemPos
 */
int VS_BUF_ReleaseWriteMemPos(const int s32Chn,const VS_STREAM_TYPE_E enStreamType,
VS_FRAME_TYPE_E enFrameType,unsigned int u32Len,unsigned long long u64Pts, VS_DATETIME_S *pTime);


   /**
 \brief获取下一帧的类型
 \attention \n
 \param[in] int s32ReaderId readerid
\param[in] BUF_POSITION_POLICY_E enPolicy 下一帧读帧方式
\param[in] unsigned int s32N 相对帧数
\param[out] VS_FRAME_TYPE_E *penFrameType 帧类型
\param[in] unsigned char u8IOblock  是否阻塞

 \retval 
         :: 可写内存指针
         ::0 失败
 \see \n
    ::VS_BUF_GetWriteMemPos
 */
 int VS_BUF_GetNextFrameType(int s32ReaderId, BUF_POSITION_POLICY_E enPolicy,
                        int s32N,VS_FRAME_TYPE_E *penFrameType, unsigned char u8IOblock);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__VS_BUFFER_H__*/
