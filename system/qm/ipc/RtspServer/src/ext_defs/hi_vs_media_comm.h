/***********************************************************************************
*              Copyright 2006 - , Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_vs_media_comm.h
* Description:
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.0       2008-01-28   w54723     NULL         Create this file.
***********************************************************************************/


#ifndef __HI_VS_MEDIA_COMM_H__
#define __HI_VS_MEDIA_COMM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"

typedef HI_S32 VS_CHN;       /*监控通道类型*/ 

#define VS_CHN_ALL     0x0000FFFF
#define VS_CHN_DEFAULT 0
#define VS_CHN_INVALID -1

typedef HI_S32 ENC_VE_CHN;

typedef HI_S32 ENC_AE_CHN;

typedef HI_S32 ENC_VGRP_ID;

typedef HI_S32 ENC_SYNC_CHN;

typedef HI_S32 ENC_VI_CHN;

typedef HI_S32 ENC_AI_CHN;

typedef HI_S32 VI_VIDEV_ID;

typedef HI_S32 AI_AIDEV_ID;

#define ENC_MAX_VS_CHN_NUM  16
#define ENC_MAX_VGRP_NUM    16

/*mhua add 下面的2个宏未使用*/
#define ENC_MAX_VE_CHN_NUM  8   /*最大视频编码通道(ve)数*/
#define ENC_MAX_AE_CHN_NUM  8   /*最大音频编码通道(ae)数*/

#define VI_MAX_VI_CHN_NUM   8
//#define AI_MAX_AI_CHN_NUM   4
#define MEDIA_MAX_VI_DEV_NUM 4  /*最大的vi设备号*/

//mhua add for vs_chn num
#define MEDIA_MAX_VSCHN_NUM   169   /*最大码流号*/

/*format of video encode*/
typedef enum hiMT_VIDEO_FORMAT_E
{
    MT_VIDEO_FORMAT_H261  = 0,  /*H261  */
    MT_VIDEO_FORMAT_H263  = 1,  /*H263  */
    MT_VIDEO_FORMAT_MPEG2 = 2,  /*MPEG2 */
    MT_VIDEO_FORMAT_MPEG4 = 3,  /*MPEG4 */
    MT_VIDEO_FORMAT_H264  = 4,  /*H264  */
    MT_VIDEO_FORMAT_MJPEG = 5,  /*MOTION_JPEG*/
    MT_VIDEO_FORMAT_H265  = 6,  /*H265*/
    MT_VIDEO_FORMAT_BUTT
}MT_VIDEO_FORMAT_E;

/*format of audio encode*/
typedef enum hiMT_AUDIO_FORMAT_E
{
    MT_AUDIO_FORMAT_G711A   = 1,   /* G.711 A            */
    MT_AUDIO_FORMAT_G711Mu  = 2,   /* G.711 Mu           */
    MT_AUDIO_FORMAT_ADPCM   = 3,   /* ADPCM              */
    MT_AUDIO_FORMAT_G726    = 4,   /* G.726              */
    MT_AUDIO_FORMAT_AMR     = 5,   /* AMR encoder format */
    MT_AUDIO_FORMAT_AMRDTX  = 6,   /* AMR encoder formant and VAD1 enable */
    MT_AUDIO_FORMAT_AAC     = 7,   /* AAC encoder        */
    MT_AUDIO_FORMAT_BUTT
}MT_AUDIO_FORMAT_E;

typedef enum hiMT_AUDIO_BITWIDTH_E
{
    MT_AUDIO_BITWIDTH_8   = 0,   /* Bit width is 8 bits   */
    MT_AUDIO_BITWIDTH_16  = 1,   /* Bit width is 16 bits  */
    MT_AUDIO_BITWIDTH_32  = 2,   /* Bit width is 32 bits */ 
    MT_AUDIO_BITWIDTH_BUTT
}MT_AUDIO_BITWIDTH_E;

typedef enum hiMT_AUDIO_SAMPLE_RATE_E
{
    MT_AUDIO_SAMPLE_RATE_8     = 0,   /* 8K Sample rate     */
    MT_AUDIO_SAMPLE_RATE_11025 = 1,   /* 11.025K Sample rate*/
    MT_AUDIO_SAMPLE_RATE_16    = 2,   /* 16K Sample rate    */
    MT_AUDIO_SAMPLE_RATE_22050 = 3,   /* 22.050K Sample rate*/
    MT_AUDIO_SAMPLE_RATE_24    = 4,   /* 24K Sample rate    */
    MT_AUDIO_SAMPLE_RATE_32    = 5,   /* 32K Sample rate    */
    MT_AUDIO_SAMPLE_RATE_441   = 6,   /* 44.1K Sample rate  */
    MT_AUDIO_SAMPLE_RATE_48    = 7,   /* 48K Sample rate    */
    MT_AUDIO_SAMPLE_RATE_64    = 8,   /* 64K Sample rate    */
    MT_AUDIO_SAMPLE_RATE_882   = 9,   /* 88.2K Sample rate  */
    MT_AUDIO_SAMPLE_RATE_96    = 10,  /* 96K Sample rate    */
    MT_AUDIO_SAMPLE_RATE_1764  = 11,  /* 176.4K Sample rate */
    MT_AUDIO_SAMPLE_RATE_192   = 12,  /* 192K Sample rate   */
    MT_AUDIO_SAMPLE_RATE_BUTT
}MT_AUDIO_SAMPLE_RATE_E;


typedef enum hiMT_SOUND_MODE_E
{
    MT_SOUND_MODE_MOMO =0,          /*单声道*/
    MT_SOUND_MODE_STEREO =1,        /*双声道*/
    MT_SOUND_MODE_BUTT    
}MT_SOUND_MODE_E;


typedef struct hiVS_AUDIO_ATTR_S{
    MT_AUDIO_FORMAT_E enAFormat;              /*音频编码格式*/
    HI_U32          u32TargetBps;             /*目标码率*/
    MT_AUDIO_SAMPLE_RATE_E  enSampleRate;     /*音频输入采样率，单位赫兹*/
    MT_AUDIO_BITWIDTH_E  enBitwidth;          /*音频输入位宽*/
    MT_SOUND_MODE_E      enSoundMode;         /*音频输入声道数*/
}VS_AUDIO_ATTR_S;



typedef struct hiMT_MEDIAINFO_S
{
    /*include "hi_vs_media_comm.h"*/
    MT_AUDIO_FORMAT_E enAudioType;    //音频格式
    HI_U32  u32AudioSampleRate;       //音频采样率，单位赫兹
    HI_U32  u32AudioChannelNum;       // 1单声道 2双声道
    MT_VIDEO_FORMAT_E enVideoType;    //视频格式
    HI_U32  u32VideoPicWidth;         //视频宽
    HI_U32  u32VideoPicHeight;        //视频高
    HI_U32  u32VideoSampleRate;       //视频采样频率，单位赫兹，恒定为90000
    HI_U32  u32Framerate;
    HI_U32  u32Bitrate;
    HI_U32  u16EncodeVideo;       //是否编码视频
    HI_U32  u16EncodeAudio;       //是否编码音频

    unsigned long u32TotalTime;         //回放文件长度
    //HI_CHAR auVideoDataInfo[128];
    HI_CHAR auVideoDataInfo[256];		//兼容TI项目
    HI_U32  u32VideoDataLen;
    int SPS_LEN;
    int PPS_LEN;
    int SEL_LEN;
    unsigned char profile_level_id[8];
}MT_MEDIAINFO_S;

//mhua add to check vs_chn
#define HI_CHECK_VSCHN(s32VsChn) \
    (((s32VsChn) < 0 || (s32VsChn) >= MEDIA_MAX_VSCHN_NUM)?HI_FALSE:HI_TRUE)


const HI_CHAR* HI_VS_GetVFmtCap(MT_VIDEO_FORMAT_E fmt);
const HI_CHAR* HI_VS_GetAFmtCap(MT_AUDIO_FORMAT_E fmt);

const MT_VIDEO_FORMAT_E HI_VS_GetVFmt_byCap(const HI_CHAR* cap);
const MT_AUDIO_FORMAT_E HI_VS_GetAFmt_byCap(const HI_CHAR* cap);



#define ENC_VI_CH_STARTX    0
#define ENC_VI_CH_STARTY    0

#define ENC_VGA_WIDTH       640 /*图像为vga时的标准宽*/
#define ENC_VGA_HEIGHT      480 /*图像为vga时的标准高*/
#define ENC_QVGA_WIDTH      320 /*图像为qvga时的标准宽*/
#define ENC_QVGA_HEIGHT     240 /*图像为qvga时的标准高*/
#define ENC_QQVGA_WIDTH      160 /*图像为qvga时的标准宽*/
#define ENC_QQVGA_HEIGHT     128 /*图像为qvga时的标准高*/

#define ENC_CIF_WIDTH_PAL       352 /*图像为cif时的标准宽(P制)*/
#define ENC_CIF_HEIGHT_PAL      288 /*图像为cif时的标准高(P制)*/
#define ENC_CIF_WIDTH_NTSC       352 /*图像为cif时的标准宽(N制)*/
#define ENC_CIF_HEIGHT_NTSC      240 /*图像为cif时的标准高(N制)*/

#define ENC_QCIF_WIDTH_PAL       176 /*图像为cif时的标准宽(P制)*/
#define ENC_QCIF_HEIGHT_PAL      144 /*图像为cif时的标准高(P制)*/
#define ENC_QCIF_WIDTH_NTSC       176 /*图像为cif时的标准宽(N制)*/
#define ENC_QCIF_HEIGHT_NTSC      112 /*图像为cif时的标准高(N制)*/


#define ENC_D1_WIDTH_PAL       704 /*图像为cif时的标准宽(P制)*/
#define ENC_D1_HEIGHT_PAL      576 /*图像为cif时的标准高(P制)*/
#define ENC_D1_WIDTH_NTSC       704 /*图像为cif时的标准宽(N制)*/
#define ENC_D1_HEIGHT_NTSC      480 /*图像为cif时的标准高(N制)*/

#define ENC_XGA_WIDTH           1280    /*图像为高清的标准宽*/
#define ENC_XGA_HEIGHT          720    /*图像为高清的标准高*/



/*VSCHN号到数组下标的转换*/
#define VS_CHN2IDX(vschn) ((vschn/10 - 1)*VS_STREAM_PER_CAM + (vschn%10 -1 ))


/*检查vschn号是否合法*/
//#define VS_CHECK_VSCHN(vschn) ( ( (vschn) > 0) && ( (vschn) <=(VS_MAX_CAM_CNT*10+VS_STREAM_PER_CAM)))

#define VS_CHECK_VSCHN(vschn) (( (vschn) > 0) && ( (vschn) <=(VS_MAX_CAM_CNT*10+VS_STREAM_PER_CAM))&&((vschn/10 >= 1)&&(vschn/10 <= VS_MAX_CAM_CNT))&&((vschn%10 >= 1)&&(vschn%10 <= VS_STREAM_PER_CAM)))


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /*__HI_VS_MEDIA_COMM_H__*/

