
#ifndef __HI_MBUF_DEF_H__
#define __HI_MBUF_DEF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

typedef void*  HI_ADDR;
typedef HI_S32 MBUF_CHN;

/*MBUF payload type*/
typedef enum hiMBUF_PT_E
{
    VIDEO_KEY_FRAME = 1, //视频关键帧
    VIDEO_NORMAL_FRAME, //视频普通帧
    AUDIO_FRAME, //音频帧
    MD_FRAME, //MD数据
    MBUF_PT_BUTT
}MBUF_PT_E;


/*视频slice的信息特别注意，一帧只有一个slice，则该slice即是起始也是结束slice*/
#define CHECK_VIDEO_START_SLICE(x) ((x) & 0x01)/*一帧的起始slice*/
#define CHECK_VIDEO_NORMAL_SLICE(x) ((x) & 0x02) /*一帧的中间slice*/
#define CHECK_VIDEO_END_SLICE(x) ((x) & 0x04) /*一帧的最后一个slice*/

#define SET_VIDEO_START_SLICE(x) ((x) |= 0x01) /*一帧的起始slice*/
#define SET_VIDEO_NORMAL_SLICE(x) ((x) |= 0x02) /*一帧的中间slice*/
#define SET_VIDEO_END_SLICE(x) ((x) |= 0x04) /*一帧的最后一个slice*/
#define CLEAR_VIDEO_SLICE(x) ((x) = 0) /*清除视频slice标志*/


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
