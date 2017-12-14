/***********************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_rtp.h
* Description: The RTP module.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2006-05-10   T41030     NULL         Create this file.
* 2.0       2006-05-08   W54723     NULL         Modify this file.  
***********************************************************************************/

#ifndef __HI_RTP_H__
#define __HI_RTP_H__

#include "hi_type.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#define HI_ERR_RTP_INVALID_PARA 1
#define HI_ERR_RTP_SOCKET       2
#define HI_ERR_RTP_SEND         3
#define HI_ERR_RTP_REACHMAX     4 /*已经达到能力限制*/
#define HI_ERR_RTP_NOT_ENOUGHMEM 5
#define HI_CHECK_TCPIP_PORT(port) ((port > 0 ) && (port < 35536))

//#define VOD_MAX_CHN             4
#define H264_STARTCODE_LEN      4 /* 00 00 00 01 */

#define SET_BITFIELD(field, val, mask, shift) \
   do { \
     (field) &= ~(mask); \
     (field) |= (((val) << (shift)) & (mask)); \
   } while (0)


#define RTP_VERSION    2

/*RTP Payload type define*/
typedef enum hiRTP_PT_E
{
    RTP_PT_ULAW             = 0,        /* mu-law */
    RTP_PT_GSM              = 3,        /* GSM */
    RTP_PT_G723             = 4,        /* G.723 */
    RTP_PT_ALAW             = 8,        /* a-law */
    RTP_PT_G722             = 9,        /* G.722 */
    RTP_PT_S16BE_STEREO     = 10,       /* linear 16, 44.1khz, 2 channel */
    RTP_PT_S16BE_MONO       = 11,       /* linear 16, 44.1khz, 1 channel */
    RTP_PT_MPEGAUDIO        = 14,       /* mpeg audio */
    RTP_PT_JPEG             = 26,       /* jpeg */
    RTP_PT_H261             = 31,       /* h.261 */
    RTP_PT_MPEGVIDEO        = 32,       /* mpeg video */
    RTP_PT_MPEG2TS          = 33,       /* mpeg2 TS stream */
    RTP_PT_H263             = 34,       /* old H263 encapsulation */
                            
    //RTP_PT_PRIVATE          = 96,       
    RTP_PT_H264             = 96,       /* hisilicon define as h.264 */
    RTP_PT_G726             = 97,       /* hisilicon define as G.726 */
    RTP_PT_ADPCM            = 98,       /* hisilicon define as ADPCM */
    RTP_PT_DATA             = 100,      /* hisilicon define as md alarm data*/
    RTP_PT_ARM              = 101,      /* hisilicon define as AMR*/
    RTP_PT_MJPEG             = 102,       /* hisilicon define as MJPEG */
    RTP_PT_H265				=103,
    RTP_PT_AAC				= 104,    /* dyn sdp set*/
    RTP_PT_INVALID          = 127
}RTP_PT_E;


/* op-codes */
#define RTP_OP_PACKETFLAGS  1               /* opcode datalength = 1 */

#define RTP_OP_CODE_DATA_LENGTH     1

/* flags for opcode RTP_OP_PACKETFLAGS */
#define RTP_FLAG_LASTPACKET 0x00000001      /* last packet in stream */
#define RTP_FLAG_KEYFRAME   0x00000002      /* keyframe packet */

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* total 12Bytes */
typedef struct hiRTP_HDR_S
{
#if (BYTE_ORDER == LITTLE_ENDIAN)
    /* byte 0 */
    HI_U16 cc      :4;   /* CSRC count */
    HI_U16 x       :1;   /* header extension flag */
    HI_U16 p       :1;   /* padding flag */
    HI_U16 version :2;   /* protocol version */

    /* byte 1 */
    HI_U16 pt      :7;   /* payload type */
    HI_U16 marker  :1;   /* marker bit */
#elif (BYTE_ORDER == BIG_ENDIAN)
    /* byte 0 */
    HI_U16 version :2;   /* protocol version */
    HI_U16 p       :1;   /* padding flag */
    HI_U16 x       :1;   /* header extension flag */
    HI_U16 cc      :4;   /* CSRC count */
    /*byte 1*/
    HI_U16 marker  :1;   /* marker bit */
    HI_U16 pt      :7;   /* payload type */
#else
    #error YOU MUST DEFINE BYTE_ORDER == LITTLE_ENDIAN OR BIG_ENDIAN !  
#endif

    /* bytes 2, 3 */
    HI_U16 seqno  :16;   /* sequence number */

    /* bytes 4-7 */
    HI_U32 ts;            /* timestamp in ms */
  
    /* bytes 8-11 */
    HI_U32 ssrc;          /* synchronization source */
} RTP_HDR_S;


/*rtsp interleaved packet*/
typedef struct hiRTSP_ITLEAVED_HDR_S
{
    HI_U8  daollar;      /*8, $:dollar sign(24 decimal)*/
    HI_U8  channelid;    /*8, channel id*/
    HI_U16 payloadLen;   /*16, payload length*/
    
    RTP_HDR_S rtpHead;   /*rtp head*/
    
}RTSP_ITLEAVED_HDR_S;

/*hisi interleaved packet*/
typedef struct hiHISI_ITLEAVED_HDR_S
{
    HI_U8  daollar;      /*8, $:dollar sign(24 decimal)*/
    HI_U8  channelid;    /*8, channel id*/
    HI_U16 resv;         /*16, reseved  0x0080 == I Frame */
    HI_U32 payloadLen;   /*32, payload length*/
    
    RTP_HDR_S rtpHead;   /*rtp head*/
    
}HISI_ITLEAVED_HDR_S;

typedef struct hiRTP_STAP_HDR_S
{
    HI_U8 header;
    HI_U8 nal_size[2]; 
}RTP_STAP_HDR_S; 

/*rtp field packet*/
#define RTP_HDR_SET_VERSION(pHDR, val)  ((pHDR)->version = val)
#define RTP_HDR_SET_P(pHDR, val)        ((pHDR)->p       = val)
#define RTP_HDR_SET_X(pHDR, val)        ((pHDR)->x       = val) 
#define RTP_HDR_SET_CC(pHDR, val)       ((pHDR)->cc      = val)

#define RTP_HDR_SET_M(pHDR, val)        ((pHDR)->marker  = val)
#define RTP_HDR_SET_PT(pHDR, val)       ((pHDR)->pt      = val)

#define RTP_HDR_SET_SEQNO(pHDR, _sn)    ((pHDR)->seqno  = (_sn))
#define RTP_HDR_SET_TS(pHDR, _ts)       ((pHDR)->ts     = (_ts))

#define RTP_HDR_SET_SSRC(pHDR, _ssrc)    ((pHDR)->ssrc  = _ssrc)

#define RTP_HDR_LEN  sizeof(RTP_HDR_S)

#define RTP_DEFAULT_SSRC 41030

#if (BYTE_ORDER == LITTLE_ENDIAN)
typedef struct Frag_unit_indicator
{
	HI_U8 Forbit	:1;
	HI_U8 NRI		:2;
	HI_U8 Type		:5;
}HI_S_FU_INDICATOR;

typedef struct Farg_Unit_Header
{
	HI_U8 Start		:1;
	HI_U8 End		:1;
	HI_U8 Reserve	:1;
	HI_U8 Type		:5;
}HI_S_FU_HEADER;
#else
typedef struct Frag_unit_indicator
{
	HI_U8 Type          :5;
	HI_U8   NRI         :2;
	HI_U8   Forbit      :1;
}HI_S_FU_INDICATOR;

typedef struct Farg_Unit_Header
{
	HI_U8   Type            :5;
	HI_U8   Reserve      :1;
	HI_U8   End              :1;
	HI_U8   Start            :1;
}HI_S_FU_HEADER;
#endif

typedef enum
{
    FU_A = 28,
    FU_B = 29,
}HI_FU_SORT;

typedef enum
{
	SR = 200,
	RR = 201,
	SDES = 202,
	BYE = 203,
	APP = 204
}rtcp_pkt_type;

typedef enum
{
    CNAME = 1,
    NAME = 2,
    EMAIL = 3,
    PHONE = 4,
    LOC = 5,
    TOOL = 6,
    NOTE = 7,
    PRIV = 8
} rtcp_info;

typedef struct RTCP_header
{
#if (BYTE_ORDER == LITTLE_ENDIAN)
    HI_U8 count:5;    //< SC or RC
    HI_U8 padding:1;
    HI_U8 version:2;
#elif (BYTE_ORDER == BIG_ENDIAN)
    HI_U8 version:2;
    HI_U8 padding:1;
    HI_U8 count:5;    //< SC or RC
#else
#error Neither big nor little
#endif
    HI_U8 pt;
    HI_U16 length;
}RTCP_header;

typedef struct RTCP_header_SR
{
    HI_U32 ssrc;
    HI_U32 ntp_timestampH;
    HI_U32 ntp_timestampL;
    HI_U32 rtp_timestamp;
    HI_U32 pkt_count;
    HI_U32 octet_count;
}RTCP_header_SR;

typedef struct RTCP_header_RR
{
    HI_U32 ssrc;
}RTCP_header_RR;

typedef struct RTCP_header_SR_report_block
{
    HI_U32 ssrc;				/* data source being reported */
    HI_U8 fract_lost;			/* fraction lost since last SR/RR */
    HI_U8 pck_lost[3];			/* cumul. no. pkts lost (signed!) */
    HI_U32 h_seq_no;			/* extended last seq. no. received */
    HI_U32 jitter;				/* interarrival jitter */
    HI_U32 last_SR;				/* last SR packet from this source */
    HI_U32 delay_last_SR;		/* delay since last SR packet */
}RTCP_header_SR_report_block;

typedef struct RTCP_header_SDES
{
    HI_U32 ssrc;
    HI_U8 attr_name;
    HI_U8 len;
}RTCP_header_SDES;

typedef struct RTCP_header_BYE
{
    HI_U32 ssrc;
    HI_U8 length;
}RTCP_header_BYE;

typedef struct RTCP_stats
{
    HI_U32 RR_received;
    HI_U32 SR_received;
    HI_U32 dest_SSRC;
    HI_U32 pkt_count;
    HI_U32 octet_count;
    HI_U32 pkt_lost;
    HI_U8 fract_lost;
    HI_U32 highest_seq_no;
    HI_U32 jitter;
    HI_U32 last_SR;
    HI_U32 delay_since_last_SR;
}RTCP_stats;



HI_VOID HI_RTP_Packet(HI_CHAR* pPackAddr,HI_U32 u32TimeStamp, HI_U32 marker,
                     RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn);


HI_VOID HI_RTSP_ITLV_Packet(HI_CHAR * pPackAddr, HI_U32 u32PackLen,
                           HI_U32 u32TimeStamp, HI_U32 marker,
                           RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn,HI_U32  interleavedchid);

/*hisi 方式的潜入式打包格式*/                           
HI_VOID HI_HISI_ITLV_Packet(HI_CHAR * pPackAddr, HI_U32 u32PackLen,
                           HI_U32 u32TimeStamp, HI_U32 marker,
                           RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn);
                           
                           
HI_VOID HI_RTP_STAP_Packet(HI_CHAR * pPackAddr, HI_U32 u32PackLen,
                           HI_U32 u32TimeStamp, HI_U32 marker,
                           RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn);
                           
HI_S32 HI_RTP_FU_PackageHeader(HI_CHAR* pHeader,HI_U8 *pMessage,HI_U8 start,HI_U8 end);                          
                          
#if 0                           
/*send stream*/
HI_S32 HI_RTP_Send(HI_SOCKET s32WritSock,HI_U8* pu8Buff, HI_S32 s32DataLen,
                       struct sockaddr* pstruToAddr);


HI_S32 HI_RTP_AudioPTConvert(HI_U32 aenctype);

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
