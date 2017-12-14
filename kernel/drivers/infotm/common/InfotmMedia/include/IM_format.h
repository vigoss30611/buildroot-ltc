/***************************************************************************** 
** 
** Copyright (c) 2012~2112 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
**      
** Revision History: 
** ----------------- 
** v1.0.1	leo@2012/03/15: first commit.
**
*****************************************************************************/ 

#ifndef __IM_FORMAT_H__
#define __IM_FORMAT_H__

#include <IM_picformat.h>

typedef enum{
    IM_MEDIA_NONE = 0,

    IM_MEDIA_LOCAL = 10,
    IM_MEDIA_STREAMING = 20
}IM_MEDIA_TYPE;

typedef enum{
    IM_LOCALMEDIA_NONE = 0,

    IM_LOCALMEDIA_AVI = 1,
    IM_LOCALMEDIA_MP4 = 2
}IM_LOCALMEDIA_TYPE;

typedef enum{
    IM_STREAMINGMEDIA_NONE = 0
}IM_STREAMINGMEDIA_TYPE;

typedef enum{
    IM_STRM_TYPE_NONE = 0,

    IM_STRM_TYPE_AUDIO = 10,
    IM_STRM_TYPE_VIDEO = 20,
    IM_STRM_TYPE_TEXT = 30
}IM_STREAM_TYPE;

typedef enum{
    IM_STRM_SUBTYPE_NONE = 0,

    /* various PCM "codecs" */
    IM_STRM_AUDIO_PCM_S16LE= 0x10000,
    IM_STRM_AUDIO_PCM_S16BE,
    IM_STRM_AUDIO_PCM_U16LE,
    IM_STRM_AUDIO_PCM_U16BE,
    IM_STRM_AUDIO_PCM_S8,
    IM_STRM_AUDIO_PCM_U8,
    IM_STRM_AUDIO_PCM_MULAW,
    IM_STRM_AUDIO_PCM_ALAW,
    IM_STRM_AUDIO_PCM_S32LE,
    IM_STRM_AUDIO_PCM_S32BE,
    IM_STRM_AUDIO_PCM_U32LE,
    IM_STRM_AUDIO_PCM_U32BE,
    IM_STRM_AUDIO_PCM_S24LE,
    IM_STRM_AUDIO_PCM_S24BE,
    IM_STRM_AUDIO_PCM_U24LE,
    IM_STRM_AUDIO_PCM_U24BE,
    IM_STRM_AUDIO_PCM_S24DAUD,
    IM_STRM_AUDIO_PCM_ZORK,
    IM_STRM_AUDIO_PCM_S16LE_PLANAR,
    IM_STRM_AUDIO_PCM_DVD,
    IM_STRM_AUDIO_PCM_F32BE,
    IM_STRM_AUDIO_PCM_F32LE,
    IM_STRM_AUDIO_PCM_F64BE,
    IM_STRM_AUDIO_PCM_F64LE,
    IM_STRM_AUDIO_PCM_BLURAY,

    /* various ADPCM codecs */
    IM_STRM_AUDIO_ADPCM_IMA_QT= 0x11000,
    IM_STRM_AUDIO_ADPCM_IMA_WAV,
    IM_STRM_AUDIO_ADPCM_IMA_DK3,
    IM_STRM_AUDIO_ADPCM_IMA_DK4,
    IM_STRM_AUDIO_ADPCM_IMA_WS,
    IM_STRM_AUDIO_ADPCM_IMA_SMJPEG,
    IM_STRM_AUDIO_ADPCM_MS,
    IM_STRM_AUDIO_ADPCM_4XM,
    IM_STRM_AUDIO_ADPCM_XA,
    IM_STRM_AUDIO_ADPCM_ADX,
    IM_STRM_AUDIO_ADPCM_EA,
    IM_STRM_AUDIO_ADPCM_G726,
    IM_STRM_AUDIO_ADPCM_CT,
    IM_STRM_AUDIO_ADPCM_SWF,
    IM_STRM_AUDIO_ADPCM_YAMAHA,
    IM_STRM_AUDIO_ADPCM_SBPRO_4,
    IM_STRM_AUDIO_ADPCM_SBPRO_3,
    IM_STRM_AUDIO_ADPCM_SBPRO_2,
    IM_STRM_AUDIO_ADPCM_THP,
    IM_STRM_AUDIO_ADPCM_IMA_AMV,
    IM_STRM_AUDIO_ADPCM_EA_R1,
    IM_STRM_AUDIO_ADPCM_EA_R3,
    IM_STRM_AUDIO_ADPCM_EA_R2,
    IM_STRM_AUDIO_ADPCM_IMA_EA_SEAD,
    IM_STRM_AUDIO_ADPCM_IMA_EA_EACS,
    IM_STRM_AUDIO_ADPCM_EA_XAS,
    IM_STRM_AUDIO_ADPCM_EA_MAXIS_XA,
    IM_STRM_AUDIO_ADPCM_IMA_ISS,

    /* AMR */
    IM_STRM_AUDIO_AMR_NB= 0x12000,
    IM_STRM_AUDIO_AMR_WB,

    /* RealAudio codecs*/
    IM_STRM_AUDIO_RA_144= 0x13000,
    IM_STRM_AUDIO_RA_288,
    IM_STRM_AUDIO_RALF,

    /* various DPCM codecs */
    IM_STRM_AUDIO_ROQ_DPCM= 0x14000,
    IM_STRM_AUDIO_INTERPLAY_DPCM,
    IM_STRM_AUDIO_XAN_DPCM,
    IM_STRM_AUDIO_SOL_DPCM,

    /* audio codecs */
    IM_STRM_AUDIO_MP2= 0x15000,
    IM_STRM_AUDIO_MP3, ///< preferred ID for decoding MPEG audio layer 1, 2 or 3
    IM_STRM_AUDIO_AAC,
    IM_STRM_AUDIO_AC3,
    IM_STRM_AUDIO_DTS,
    IM_STRM_AUDIO_VORBIS,
    IM_STRM_AUDIO_DVAUDIO,
    IM_STRM_AUDIO_WMA,
    IM_STRM_AUDIO_WMAV1,
    IM_STRM_AUDIO_WMAV2,
    IM_STRM_AUDIO_MACE3,
    IM_STRM_AUDIO_MACE6,
    IM_STRM_AUDIO_VMDAUDIO,
    IM_STRM_AUDIO_SONIC,
    IM_STRM_AUDIO_SONIC_LS,
    IM_STRM_AUDIO_FLAC,
    IM_STRM_AUDIO_MP3ADU,
    IM_STRM_AUDIO_MP3ON4,
    IM_STRM_AUDIO_SHORTEN,
    IM_STRM_AUDIO_ALAC,
    IM_STRM_AUDIO_WESTWOOD_SND1,
    IM_STRM_AUDIO_GSM, ///< as in Berlin toast format
    IM_STRM_AUDIO_QDM2,
    IM_STRM_AUDIO_COOK,
    IM_STRM_AUDIO_TRUESPEECH,
    IM_STRM_AUDIO_TTA,
    IM_STRM_AUDIO_SMACKAUDIO,
    IM_STRM_AUDIO_QCELP,
    IM_STRM_AUDIO_WAVPACK,
    IM_STRM_AUDIO_DSICINAUDIO,
    IM_STRM_AUDIO_IMC,
    IM_STRM_AUDIO_MUSEPACK7,
    IM_STRM_AUDIO_MLP,
    IM_STRM_AUDIO_GSM_MS, /* as found in WAV */
    IM_STRM_AUDIO_ATRAC3,
    IM_STRM_AUDIO_VOXWARE,
    IM_STRM_AUDIO_APE,
    IM_STRM_AUDIO_NELLYMOSER,
    IM_STRM_AUDIO_MUSEPACK8,
    IM_STRM_AUDIO_SPEEX,
    IM_STRM_AUDIO_WMAVOICE,
    IM_STRM_AUDIO_WMAPRO,
    IM_STRM_AUDIO_WMALOSSLESS,
    IM_STRM_AUDIO_ATRAC3P,
    IM_STRM_AUDIO_EAC3,
    IM_STRM_AUDIO_SIPR,
    IM_STRM_AUDIO_MP1,
    IM_STRM_AUDIO_TWINVQ,
    IM_STRM_AUDIO_TRUEHD,
    IM_STRM_AUDIO_MP4ALS,
    IM_STRM_AUDIO_ATRAC1,

    IM_STRM_VIDEO_H263 = 0x20000,
    IM_STRM_VIDEO_H264,
    IM_STRM_VIDEO_MPEG4,
    IM_STRM_VIDEO_RV30,
    IM_STRM_VIDEO_RV40,
    IM_STRM_VIDEO_MSMPEG4V3,
    IM_STRM_VIDEO_MSMPEG4V2,
    IM_STRM_VIDEO_MSMPEG4V1,
    IM_STRM_VIDEO_WMV1,
    IM_STRM_VIDEO_WMV2,
    IM_STRM_VIDEO_WMV3,
    IM_STRM_VIDEO_VC1,
    IM_STRM_VIDEO_VP6,
    IM_STRM_VIDEO_VP6F,
    IM_STRM_VIDEO_VP8,
    IM_STRM_VIDEO_FLV1,
    IM_STRM_VIDEO_MPEG1VIDEO,
    IM_STRM_VIDEO_MPEG2VIDEO,
    IM_STRM_VIDEO_MJPEG,
    IM_STRM_VIDEO_FFMPEG,
    IM_STRM_VIDEO_NOT_SUPPORT,
    IM_STRM_VIDEO_AVS,
    IM_STRM_VIDEO_DIVX4,
    IM_STRM_VIDEO_DIVX5,
    IM_STRM_VIDEO_DIVX6,
    IM_STRM_VIDEO_VP7,
    IM_STRM_VIDEO_RV10,
    IM_STRM_VIDEO_RV20,
    IM_STRM_VIDEO_SVQ3,

    IM_STRM_SUBTITLE_TEXT = 0x30000,
    IM_STRM_SUBTITLE_SRT,
    IM_STRM_SUBTITLE_SUBRIP,
    IM_STRM_SUBTITLE_DVDSUB,
    IM_STRM_SUBTITLE_DVBSUB,
    IM_STRM_SUBTITLE_XSUB,
    IM_STRM_SUBTITLE_SSA,
    IM_STRM_SUBTITLE_MOV_TEXT,
    IM_STRM_SUBTITLE_HDMV_PGS_SUB,
    IM_STRM_SUBTITLE_TELETEXT,
    IM_STRM_SUBTITLE_MICRODVD,
    IM_STRM_SUBTITLE_EIA_608,
    IM_STRM_SUBTITLE_JACOSUB,
    IM_STRM_SUBTITLE_SAMI,
    IM_STRM_SUBTITLE_REALTEXT,
    IM_STRM_SUBTITLE_SUBVIEWER,


}IM_STREAM_SUBTYPE;

typedef struct{
    IM_INT32	type;		// IM_STREAM_TYPE_xxx
    IM_INT32	subtype;	// IM_STRM_AUDIO_xxx
    IM_INT32	strm_id;

    IM_INT32	profile;	/*such as simple profile*/
    IM_INT32	level;		/*such as mp1/2/3*/
    IM_INT32	sample_rate;
    IM_INT32	frame_size;
    IM_INT32	channels;
    IM_INT32	bitspersample;
    IM_INT32 	bit_rate;
    IM_INT32 	frame_number;
    IM_INT32 	block_align;
    IM_TCHAR    language[32];

    IM_INT32 	extradata_size;
    IM_INT32 	privdata_size;
    IM_UINT8 	*extradata;
    void 	 	*privdata;
}IM_AUDIO_FORMAT;

typedef struct{
    IM_UINT32 num_sizes;
    IM_UINT32 size[16];
}IM_RV8_MSGS;

typedef struct{
    IM_INT32	type;		// IM_STREAM_TYPE_xxx
    IM_INT32	subtype;	// IM_STRM_VIDEO_xxx
    IM_INT32	strm_id;

    IM_INT32 	profile;	/*such as simple profile*/
    IM_INT32 	level;
    IM_INT32 	width;
    IM_INT32 	height;
    IM_DOUBLE 	fps;
    IM_INT32 	bit_rate;
    IM_INT32 	frame_number;
    IM_INT32 	block_align;
    IM_UINT32 	frame_duration; // sam : in us
    IM_INT32 	rotate;

    IM_INT32 	extradata_size;
    IM_INT32 	privdata_size;
    IM_UINT8 	*extradata;
    void 	 	*privdata;

    IM_RV8_MSGS 	*rv8msgs;	//rv used	
}IM_VIDEO_FORMAT;

typedef struct{
    IM_INT32	type;		// IM_STREAM_TYPE_xxx
    IM_INT32	subtype;	// IM_STRM_TEXT_xxx
    IM_INT32	strm_id;
    IM_TCHAR    language[32];

    IM_INT32 	extradata_size;
    IM_UINT8 	*extradata;
    IM_INT32 	privdata_size;
    void 	 	*privdata;

    // ...
}IM_TEXT_FORMAT;

typedef struct{
    IM_INT32	type;
    IM_INT32	subtype;
    IM_INT32	strm_id;
}IM_STREAM_FORMAT_HEAD;

//
// seek capabalities.
//
#define IM_CAN_SEEK_FORWARD  1
#define IM_CAN_SEEK_BACKWARD 2

typedef struct IM_MetaDataEntry{
    char *key;
    char *value;
}IM_MetaDataEntry;

typedef struct IM_MetaData {
    int count;
    IM_MetaDataEntry *elems;
}IM_MetaData;    

typedef struct IM_MetaPic {
    IM_TCHAR *meta_pic;
    IM_INT32 meta_pic_size;
}IM_MetaPic;

//
// media info.
//
#define IM_MAX_STREAMS			(32)
typedef struct{	
    IM_TCHAR 				*media_name;
    IM_MEDIA_TYPE			media_type;
    IM_LOCALMEDIA_TYPE		local_type;
    IM_STREAMINGMEDIA_TYPE	streaming_type;

    IM_INT32 nb_streams;
    IM_INT64 start_time;	// unit us
    IM_INT64 duration;	// unit us
    IM_INT64 file_size;
    IM_INT32 bit_rate;
    IM_INT64 offset;   		/* offset of the first packet */
    IM_TCHAR *file_container;
    IM_MetaData *meta;

    IM_INT32    st_index[3];  //0->best audio, 1->best video,2->best subtitle
#if 0
    IM_TCHAR title[512];
    IM_TCHAR author[512];
    IM_TCHAR copyright[512];
    IM_TCHAR comment[512];
    IM_TCHAR album[512];
    IM_TCHAR artist[512];//add new feature, in the future,we should arrange them
    IM_TCHAR date[512];//
    IM_INT32 year;  /**< ID3 year, 0 if none */
    IM_INT32 track; /**< track number, 0 if none */
    IM_TCHAR genre[128]; /**< ID3 genre */
    IM_TCHAR location[32]; /**< latitude and longitude */
#endif
    IM_UINT32 flag;     /*IM_CAN_SEEK_xxx*/
    IM_INT32 meta_pic_num;
    IM_MetaPic  meta_pic[IM_MAX_STREAMS];
    IM_INT32 meta_pic_size;

    IM_STREAM_FORMAT_HEAD *stream_format[IM_MAX_STREAMS];
}IM_MEDIA_INFO;

//
// invalid timestamp.
#define IM_INVALID_TS	(IM_INT64)-1
#define IM_INVALID_DURATION	(IM_INT64)(0x8000000000000000LL)

//
// Endian mode.
//
#define IM_LITTLE_ENDIAN_MODE	// if non-defined, it's BIG endian

//
// =======The image defination is deprecated, please use IM_picformat.h========
//
// image type. 
// [31:20]--reserved, [19:16]--color space(0 is rgb, 1 is yuv, 2 is jpeg), [15:8]--bpp, [7:0]--index
//
#define IM_IMAGE_CS_RGB		(0)
#define IM_IMAGE_CS_YUV		(1)
#define IM_IMAGE_CS_JPEG	(2)

#define IM_GET_IMAGE_CS(format)		(((format)>>16) & 0xf)
#define IM_GET_IMAGE_BPP(format)	(((format)>>8) & 0xff)
typedef enum{
    IM_IMAGE_NONE = 0,

    // rgb-1bit
    IM_IMAGE_BPP1_PAL = 0x00100,

    // rgb-2bit
    IM_IMAGE_BPP2_PAL = 0x00200,

    // rgb-4bit
    IM_IMAGE_BPP4_PAL = 0x00400,

    // rgb-8bit
    IM_IMAGE_BPP8_PAL = 0x00800,
    IM_IMAGE_ARGB1232 = 0x00801,

    // rgb-16bit
    IM_IMAGE_RGB565 = 0x01000,
    IM_IMAGE_BGR565 = 0x01001,
    IM_IMAGE_RGB555 = 0x01002,
    IM_IMAGE_BGR555 = 0x01003,
    IM_IMAGE_RGB444 = 0x01004,
    IM_IMAGE_BGR444 = 0x01005,
    IM_IMAGE_RGB1555 = 0x01006,
    IM_IMAGE_RGB5551 = 0x01007,
    IM_IMAGE_BGR1555 = 0x01008,
    IM_IMAGE_BGR5551 = 0x01009,
    IM_IMAGE_RGBI555 = 0x0100a,
    IM_IMAGE_RGB555I = 0x0100b,
    IM_IMAGE_BGRI555 = 0x0100c,
    IM_IMAGE_BGR555I = 0x0100d,
    IM_IMAGE_ARGB4444 = 0x0100e,
    IM_IMAGE_ABGR4444 = 0x0100f,
    IM_IMAGE_RGBA5551 = 0x01010,
    IM_IMAGE_BGRA5551 = 0x01011,

    // rgb-32bit
    IM_IMAGE_RGB0888 = 0x02000,
    IM_IMAGE_BGR0888 = 0x02001,
    IM_IMAGE_RGB8880 = 0x02002,
    IM_IMAGE_BGR8880 = 0x02003,
    IM_IMAGE_ARGB8888 = 0x02004,
    IM_IMAGE_ABGR8888 = 0x02005,
    IM_IMAGE_RGBA8888 = 0x02006,
    IM_IMAGE_BGRA8888 = 0x02007,
    IM_IMAGE_RGB666 = 0x02008,
    IM_IMAGE_BGR666 = 0x02009,
    IM_IMAGE_ARGB1665 = 0x0200a,
    IM_IMAGE_ABGR1566 = 0x0200b,
    IM_IMAGE_ARGB1666 = 0x0200c,
    IM_IMAGE_ABGR1666 = 0x0200d,
    IM_IMAGE_ARGB1887 = 0x0200e,
    IM_IMAGE_ABGR1788 = 0x0200f,
    IM_IMAGE_ARGB1888 = 0x02010,
    IM_IMAGE_ABGR1888 = 0x02011,
    IM_IMAGE_ARGB4888 = 0x02012,
    IM_IMAGE_ABGR4888 = 0x02013,
    IM_IMAGE_RGB101010 = 0x02014,
    IM_IMAGE_BGR101010 = 0x02015,


    // yuv-1bit
    IM_IMAGE_YUV400 = 0x10800,

    // yuv-12bit
    IM_IMAGE_YUV420P = 0x10c00,
    IM_IMAGE_YUV420SP = 0x10c01,

    // yuv-16bit
    IM_IMAGE_YUV422P = 0x11000,
    IM_IMAGE_YUV422SP = 0x11001,
    IM_IMAGE_YUV422I = 0x11002,
    IM_IMAGE_YUV422_YUYV = IM_IMAGE_YUV422I,
    IM_IMAGE_YUV422_VYUY = 0x11003,
    IM_IMAGE_YUV422_YVYU = 0x11004,
    IM_IMAGE_YUV422_UYVY = 0x11005,

    // yuv-24bit
    IM_IMAGE_YUV444P = 0x12000,

    // compressed format
    IM_IMAGE_JPEG = 0x20000
}IM_IMAGE_TYPE;

typedef struct{
    IM_IMAGE_TYPE type;	// IM_IMAGE_XXX
    IM_INT32 width;		// image width, pixel unit.
    IM_INT32 height;	// image height, pixel unit.
    IM_INT32 xoffset;	// image x-offset with whole buffer window, pixel unit.
    IM_INT32 yoffset;	// image y-offset with whole buffer window, pixel unit.
    IM_INT32 stride;	// y, ycbycr, rgb buffer stride, bytes nuit.
    IM_INT32 strideCb;	// cb, cbcr buffer stride, bytes nuit.
    IM_INT32 strideCr;	// cr buffer stride, bytes nuit.
}IM_IMAGE_FORMAT;
//
// ============================================================================
//

//
//rotation infomation
//

//this is for user defined rotation type
typedef struct{
    void *data;
    IM_INT32 size;
    //xxx
}IM_ROTATE_CUSTOM_CTX;


//rotation type
typedef enum{
    IM_ROTATE_NONE,
    IM_ROTATE_RIGHT90,
    IM_ROTATE_LEFT90,
    IM_ROTATE_180,
    IM_ROTATE_VER_FLIP,
    IM_ROTATE_HOR_FLIP,

    IM_ROTATE_CUSTOM//user defined rotation type
}IM_ROTATE_TYPE;


//
// color space conversion.
//
typedef enum{
    IM_CSC_NONE = 0,

    IM_CSC_BT601,
    IM_CSC_BT709,

    IM_CSC_CUSTOM = 10
}IM_CSC_TYPE;

typedef struct{
    IM_INT32 coeffA;
    IM_INT32 coeffB;
    IM_INT32 coeffC;
    IM_INT32 coeffE;
    IM_INT32 coeffF;
}IM_CSC_PARAMS;

//crop input frame
typedef struct{
    IM_INT32 enable; //0: unsupport; others: support
    /* NOTE: these are coordinates relative to the input picture;
     * if enable is 0, all these parameters are useless*/
    IM_INT32 originX; 
    IM_INT32 originY;
    IM_INT32 height;  //8 multiple
    IM_INT32 width;   //8 multiple
}IM_CROP_PARAMS;

//===========metadata func======
/**
 ** Get a metadata entry with matching key.
 **
 ** @param prev Set to the previous matching element to find the next.
 **             If set to NULL the first matching element is returned.
 ** @param flags Allows case as well as suffix-insensitive comparisons.
 ** @return Found entry or NULL, changing key or value leads to undefined behavior.
 **/
IM_MetaData *
im_meta_get(IM_MetaData *m, const char *key, const IM_MetaDataEntry *prev, int flags);

/**
 * Get number of entries in metadata list.
 *
 * @param m metadata
 * @return  number of entries in metadata list
 */
int im_meta_count(const IM_MetaData *m);

/**
 * Set the given entry in *pm, overwriting an existing entry.
 *
 * @param pm pointer to a pointer to a IM_MetaData struct. If *pm is NULL
 * a IM_MetaData struct is allocated and put in *pm.
 * @param key entry key to add to *pm (will be av_strduped depending on flags)
 * @param value entry value to add to *pm (will be av_strduped depending on flags).
 *        Passing a NULL value will cause an existing entry to be deleted.
 * @return >= 0 on success otherwise an error code <0
 */
int im_meta_set(IM_MetaData **pm, const char *key, const char *value, int flags);

/**
 * Parse the key/value pairs list and add to a IM_MetaData.
 *
 * @param key_val_sep  a 0-terminated list of characters used to separate
 *                     key from value
 * @param pairs_sep    a 0-terminated list of characters used to separate
 *                     two pairs from each other
 * @param flags        flags to use when adding to metadata list.
 * @return             0 on success, negative code on failure
 */
int im_meta_parse_string(IM_MetaData **pm, const char *str,
        const char *key_val_sep, const char *pairs_sep,
        int flags);

/**
 * Copy entries from one IM_MetaData struct into another.
 * @param dst pointer to a pointer to a IM_MetaData struct. If *dst is NULL,
 *            this function will allocate a struct for you and put it in *dst
 * @param src pointer to source IM_MetaData struct
 * @param flags flags to use when setting entries in *dst
 * @note metadata is read using the IM_META_IGNORE_SUFFIX flag
 */
void im_meta_copy(IM_MetaData **dst, IM_MetaData *src, int flags);

/**
 * Free all the memory allocated for an IM_MetaData struct
 * and all keys and values.
 */
void im_meta_free(IM_MetaData **m); 
/**
 * @}
 */

#endif	// __IM_FORMAT_H__
