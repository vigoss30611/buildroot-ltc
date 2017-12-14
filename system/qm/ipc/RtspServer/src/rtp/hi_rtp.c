/***********************************************************************************
*              Copyright 2006 - 2006, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_transfer.c
* Description: The transfer model for audio and video data.
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2006-05-10   t41030  NULL         Create this file.
*
* Version   Date         Author     DefectNum    Description
* 1.2       2006-09-12   q46326  NULL        
* use tcp send packet, assemble packet on client 
***********************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include <stdio.h>
#include "hi_rtp.h"

#if HI_OS_TYPE == HI_OS_LINUX
#include <netinet/in.h>
#elif HI_OS_TYPE == HI_OS_WIN32
#include <winsock2.h>
#endif


/*rtp packet function*/
HI_VOID HI_RTP_Packet(HI_CHAR* pPackAddr,HI_U32 u32TimeStamp, HI_U32 marker,
                     RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn)
{
    //printf("marker:%d,enPayload:%d,u32Ssrc:%d, u16LastSn:%d\n",
    //        marker,enPayload,u32Ssrc, u16LastSn);
#if 0
    unsigned short kkk1 = htons(u16LastSn);
    unsigned int kkk2 = htonl(u32TimeStamp);
    unsigned int kkk3 = htonl(u32Ssrc);
    pPackAddr[0] = 0x80;
    pPackAddr[1] = 0x60;
    if(marker == 1)pPackAddr[1] = 0xE0;
    memcpy(pPackAddr+2, &kkk1, 2);
    memcpy(pPackAddr+4, &kkk2, 4);
    memcpy(pPackAddr+8, &kkk3, 4);
#else          
    RTP_HDR_S* pRtpHdr = NULL;
    
    pRtpHdr = (RTP_HDR_S*)pPackAddr;

    RTP_HDR_SET_VERSION(pRtpHdr, RTP_VERSION);
    RTP_HDR_SET_P(pRtpHdr, 0);                  //no padding
    RTP_HDR_SET_X(pRtpHdr, 0);                  //no extension
    RTP_HDR_SET_CC(pRtpHdr, 0);                 //0 CSRC

    RTP_HDR_SET_M(pRtpHdr, marker);             
    RTP_HDR_SET_PT(pRtpHdr, enPayload);

    RTP_HDR_SET_SEQNO(pRtpHdr, htons(u16LastSn));
    
    RTP_HDR_SET_TS(pRtpHdr, htonl(u32TimeStamp));

    RTP_HDR_SET_SSRC(pRtpHdr, htonl(u32Ssrc));
#endif
    return ;
}

HI_VOID HI_RTSP_ITLV_Packet(HI_CHAR * pPackAddr, HI_U32 u32PackLen,
                           HI_U32 u32TimeStamp, HI_U32 marker,
                           RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn,HI_U32  interleavedchid)
{
    RTSP_ITLEAVED_HDR_S* pRtspItlvHdr = NULL;

    pRtspItlvHdr = (RTSP_ITLEAVED_HDR_S*)pPackAddr;

    pRtspItlvHdr->daollar    = '$';

	pRtspItlvHdr->channelid = interleavedchid;
#if 0
	//del by lm 2015.9.1
	if(enPayload == RTP_PT_ALAW ||enPayload == RTP_PT_G726)
	{
		pRtspItlvHdr->channelid  =  2;
	}
	else
	{
    	pRtspItlvHdr->channelid  = 0;  /*to do : channel id???*/
	}
	
#endif

     /*rtsp 的payload len字段表示后续包体长度，包体长=rtp头+数据长*/
    pRtspItlvHdr->payloadLen = htons((HI_U16)(u32PackLen - sizeof(RTSP_ITLEAVED_HDR_S)+sizeof(RTP_HDR_S)));
     /*
    printf(" rtsp payload len = %d %d %u payloadtype:%d \n",u32PackLen,u32PackLen - sizeof(RTSP_ITLEAVED_HDR_S)+sizeof(RTP_HDR_S),
            pRtspItlvHdr->payloadLen,enPayload);
   */
    /*packet rtp packet*/
    HI_RTP_Packet((HI_CHAR*) (&(pRtspItlvHdr->rtpHead)),u32TimeStamp,marker,enPayload,u32Ssrc,u16LastSn);

/*
    int i = 0;
    for (i = 0;i<36;i++)
    {
        printf("0x%X ",pPackAddr[i]);
    }
    printf("\r\n");
*/
   
    return;
}  

HI_VOID HI_HISI_ITLV_Packet(HI_CHAR * pPackAddr, HI_U32 u32PackLen,
                           HI_U32 u32TimeStamp, HI_U32 marker,
                           RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn)
{
    HISI_ITLEAVED_HDR_S* pHisiItlvHdr = NULL;

	//printf("the hisi interleave pack len:%d\r\n",u32PackLen);
    pHisiItlvHdr = (HISI_ITLEAVED_HDR_S*)pPackAddr;

    pHisiItlvHdr->daollar    = '$';
    pHisiItlvHdr->channelid  = 0;  /*to do : channel id???*/
     /*rtsp 的payload len字段表示后续包体长度，包体长=rtp头+数据长*/
    pHisiItlvHdr->payloadLen = htonl((HI_U32)(u32PackLen - sizeof(HISI_ITLEAVED_HDR_S)+sizeof(RTP_HDR_S)));
    
    /* 
    printf(" rtsp payload size:%d len = %d %d %u\n",sizeof(pHisiItlvHdr->payloadLen),u32PackLen,(HI_U32)(u32PackLen - sizeof(RTSP_ITLEAVED_HDR_S)+sizeof(RTP_HDR_S)),
            pHisiItlvHdr->payloadLen);
    
    int i = 0;
    for (i = 0;i<8;i++)
    {
        printf("%X ",pPackAddr[i]);
    }
    printf("\r\n");
    */
    /*packet rtp packet*/
    HI_RTP_Packet((HI_CHAR*) (&(pHisiItlvHdr->rtpHead)),u32TimeStamp,marker,enPayload,u32Ssrc,u16LastSn);

/*
    int i = 0;
    for (i = 0;i<20;i++)
    {
        printf("%X ",pPackAddr[i]);
    }
    printf("\r\n");
*/ 
    return;
}  

/**
*parse packet of interleaved frame paramter
**/
HI_S32 HI_RTSP_Parse_Itlv_Packet(HI_CHAR *pStr)
{
	//pStr[0] Magic:0x24 	'$'标记
	//pStr[1] Channel:0x01(表示RTCP) 0x0(表示RTP通道)
	//pStr[2] pStr[3] Length:后续数据的长度
	//该模式是在RTP,RTCP包上封装一层interleaved frame
#if 0
	HI_S32 s32Len;
	if(0x24 == pStr[0] && 0x1 == pStr[1])
	{
		s32Len = ((pStr[2] << 8) | pStr[3]);
		printf("----> RTCP packet, len:%d\n", s32Len);
	}
	else if(0x24 == pStr[0] && 0x0 == pStr[1])
	{
		s32Len = ((pStr[2] << 8) | pStr[3]);
		printf("----> RTP packet, len:%d\n", s32Len);
	}
	else
		return HI_ERR_RTSP_PARSE_ERROR;

	if(0x24 != pStr[0])
		return HI_ERR_RTSP_PARSE_ERROR;

	
#endif
	return HI_SUCCESS;
}


HI_VOID HI_RTP_STAP_Packet(HI_CHAR * pPackAddr, HI_U32 u32PackLen,
                           HI_U32 u32TimeStamp, HI_U32 marker,
                           RTP_PT_E enPayload,HI_U32 u32Ssrc, HI_U16 u16LastSn)
{

}                     
typedef struct {
	 //byte 0
	unsigned char TYPE:5;
	unsigned char NRI:2; 
	unsigned char F:1;    
} FU_INDICATOR; /**//* 1 BYTES */
typedef struct {
	//byte 0
	unsigned char TYPE:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;    
} FU_HEADER; /**//* 1 BYTES */
HI_S32 HI_RTP_FU_PackageHeader(HI_CHAR* pHeader,HI_U8 *pMessage,HI_U8 start,HI_U8 end)
{
    
    HI_U8                               u8NALType;
    HI_U8                               u8Forbidden_bit;
    HI_U8                               u8Nal_reference_idc;
    FU_INDICATOR                        *pIndicator = NULL;
    if(NULL == pHeader || NULL == pMessage)
    {   
        printf("HI_RTP_FU_PackageHeader erro : pHeader=%s pMessage=%s\n",pHeader,pMessage);fflush(stdout);
        return HI_FAILURE;
    }
    if((1 == start && 1 == end) || start > 1 || end > 1)
    {   
        printf("HI_RTP_FU_PackageHeader erro : start=%d end=%d\n",start,end);fflush(stdout);
        return HI_FAILURE;
    }    
    if(*pMessage != 0 ||*(pMessage+1) != 0 || *(pMessage + 2) != 0 ||  *(pMessage + 3) != 1)
    {
		printf("this package is not have NALU\n");fflush(stdout);
		return HI_FAILURE;
    }

    u8Forbidden_bit = (*(pMessage + 4)) & 0x80;	    //1 bit
    u8Nal_reference_idc = (*(pMessage + 4)) & 0x60; // 2 bit
    u8NALType = (*(pMessage + 4)) & 0x1F;           //pMessage + 4 是NAL单元的字节头

/*
	NAL的头占用了一个字节，可以表示如下：0AAB BBBB
	其中，AA用于表示改NAL是否可以丢弃（有无被其后的NAL参考），
	00b表示没有参考作用，可丢弃，如B slice、SEI等，
	非零——包括01b、10b、11b——表示该NAL不可丢弃，如SPS、PPS、I Slice、P Slice等。常用的NAL头的取值如：
	0x67: SPS 				0110 0111
	0x68: PPS 				0110 1000
	0x65: IDR				0110 0101
	0x61: non-IDR Slice		0110 0001
	0x01: B Slice
	0x06: SEI
	0x09: AU Delimiter

	FU指示字节有以下格式：
	   +---------------+
	   |0|1|2|3|4|5|6|7|
	   +-+-+-+-+-+-+-+-+
	   |F|NRI|	Type   |
	   +---------------+
	FU指示字节的类型域的28，29表示FU-A和FU-B。F的使用在5。3描述。NRI域的值必须根据分片NAL单元的NRI域的值设置。
*/
    pIndicator = (FU_INDICATOR *)pHeader;
    pIndicator->F = u8Forbidden_bit;
    pIndicator->NRI = u8Nal_reference_idc >> 5;
    pIndicator->TYPE = 28;
//    if(0x1 == u8NALType || 0x6 == u8NALType)	//B Slice, SEI
//        *pHeader = 0x3c;						//0x3c == 0011 1100, 此处字节1至2值为 01b,表示该NAL不可丢弃
//        										//此处的字节3至7的值为NAL字节头的type(1 1100)=28,代表rtp的负载格式为FU-A
//    else
//        *pHeader = 0x7c;						//0111 1100

/*
	FU头的格式如下：
	   +---------------+
	   |0|1|2|3|4|5|6|7|
	   +-+-+-+-+-+-+-+-+
	   |S|E|R|	Type   |
	   +---------------+
	S: 1 bit
	   当设置成1,开始位指示分片NAL单元的开始。当跟随的FU荷载不是分片NAL单元荷载的开始，开始位设为0。
	E: 1 bit
	   当设置成1, 结束位指示分片NAL单元的结束，即, 荷载的最后字节也是分片NAL单元的最后一个字节。当跟随的
	   FU荷载不是分片NAL单元的最后分片,结束位设置为0。
	R: 1 bit
	   保留位必须设置为0，接收者必须忽略该位。
*/
    *(pHeader + 1) = 0;
    *(pHeader + 1) = start << 7 | end << 6 | u8NALType;		//此处start<<7为标记S, end<<6为标记E

    return HI_SUCCESS;
}


HI_S32 HI_RTCP_Decode_SR(HI_CHAR *pStr, HI_S32 len, RTCP_stats *rtcp_stats)
{
	HI_S32 ssrc_count;
	HI_U8 tmp[4];
	RTCP_header_SR SR_send_info;
 
    rtcp_stats->SR_received += 1;
    rtcp_stats->pkt_count = *((int *) &(pStr[len + 20]));
    rtcp_stats->octet_count = *((int *) &(pStr[len + 24]));

	SR_send_info.ssrc = ntohl(pStr[len + 4]);
	SR_send_info.ntp_timestampH = *((int *) &(pStr[len + 8]));
	SR_send_info.ntp_timestampL = *((int *) &(pStr[len + 12]));
	SR_send_info.rtp_timestamp = *((int *) &(pStr[len + 16]));
	SR_send_info.pkt_count = *((int *) &(pStr[len + 20]));
	SR_send_info.octet_count = *((int *) &(pStr[len + 24]));

	ssrc_count = pStr[len + 0] & 0x1f;		//reception report count

	//for(i = 0; i < ssrc_count; i++)
	if(ssrc_count > 0)
	{
		rtcp_stats->fract_lost = pStr[len + 32];
		tmp[0] = 0;
		tmp[1] = pStr[len + 33];
		tmp[2] = pStr[len + 34];
		tmp[3] = pStr[len + 35];
		rtcp_stats->pkt_lost = ntohl(*((int *) tmp));		//cumulative number of packets lost
		rtcp_stats->highest_seq_no = ntohl(pStr[len + 36]);
		rtcp_stats->jitter = ntohl(pStr[len + 40]);
		rtcp_stats->last_SR = ntohl(pStr[len + 44]);
		rtcp_stats->delay_since_last_SR = ntohl(pStr[len + 48]);
	}

	printf("SR_received:%d, pkt_count:%d, octet_count:%d, ssrc=0x%X, %u, %u, %u, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", 
		rtcp_stats->SR_received, rtcp_stats->pkt_count, rtcp_stats->octet_count,
		SR_send_info.ssrc, SR_send_info.ntp_timestampH, SR_send_info.ntp_timestampL,
		SR_send_info.rtp_timestamp, SR_send_info.pkt_count, SR_send_info.octet_count, ssrc_count,
		rtcp_stats->fract_lost, rtcp_stats->pkt_lost, rtcp_stats->highest_seq_no,
		rtcp_stats->jitter, rtcp_stats->last_SR, rtcp_stats->delay_since_last_SR);

	return HI_SUCCESS;
}


HI_S32 HI_RTCP_Decode_RR(HI_CHAR *pStr, HI_S32 len, RTCP_stats *rtcp_stats)
{
	int ssrc_count;
    unsigned char tmp[4];

    rtcp_stats->RR_received += 1;
    ssrc_count = pStr[0 + len] & 0x1f;

    //for (i = 0; i < ssrc_count; ++i)
	if(ssrc_count > 0)
	{
        rtcp_stats->fract_lost = pStr[len + 12];
        tmp[0] = 0;
        tmp[1] = pStr[len + 13];
        tmp[2] = pStr[len + 14];
        tmp[3] = pStr[len + 15];
        rtcp_stats->pkt_lost = ntohl(*((int *) tmp));
        rtcp_stats->highest_seq_no = ntohl(pStr[len + 16]);
        rtcp_stats->jitter = ntohl(pStr[len + 20]);
        rtcp_stats->last_SR = ntohl(pStr[len + 24]);
        rtcp_stats->delay_since_last_SR = ntohl(pStr[len + 28]);
    }
	return HI_SUCCESS;
}


HI_S32 HI_RTCP_Decode_SDES(HI_CHAR *pStr, HI_S32 len, RTCP_stats *rtcp_stats)
{
	HI_U32 i, packet_len;
//	RTCP_header_SDES sdes_header;

	//sdes_count = pStr[len + 0] & 0x1f;
	packet_len = (ntohs(*((short *) &(pStr[len + 2]))) + 1) * 4;
	printf("----> packet_len:%d\n", packet_len);

	rtcp_stats->dest_SSRC = ntohs(*((int *)&(pStr[len + 4])));
	printf("\n");
	for(i = 0; i < pStr[len + 9]; i++)
		printf("%c", pStr[len + 9 + i]);
	printf("\n");
	
//	sdes_header.ssrc = ntohs(*((int *)&(pStr[len + 4])));
//	sdes_header.attr_name = pStr[len + 8];
//	sdes_header.len = pStr[len + 9];

    switch (pStr[len + 8])
	{
	    case CNAME:
	        printf("SDES item type CNAME\n");
	        break;
	    case NAME:
			printf("SDES item type NAME\n");
	        break;
	    case EMAIL:
			printf("SDES item type EMAIL\n");
	        break;
	    case PHONE:
			printf("SDES item type PHONE\n");
	        break;
	    case LOC:
			printf("SDES item type LOC\n");
	        break;
	    case TOOL:
			printf("SDES item type TOOL\n");
	        break;
	    case NOTE:
			printf("SDES item type NOTE\n");
	        break;
	    case PRIV:
			printf("SDES item type PRIV\n");
	        break;
    }

	return HI_SUCCESS;
}

HI_S32 HI_RTCP_Decode_BYE(HI_CHAR *pStr, HI_S32 len, RTCP_stats *rtcp_stats)
{
	return HI_SUCCESS;
}

HI_S32 HI_RTCP_Decode_APP(HI_CHAR *pStr, HI_S32 len, RTCP_stats *rtcp_stats)
{
	return HI_SUCCESS;
}


/**
*parse RTCP packet
**/
HI_S32 HI_RTCP_recv_packet(HI_CHAR *buffer)
{
    int len = 0;
	RTCP_stats rtcp_stats;
    //for (len = 0; len < ((buffer[2]<<8 | buffer[3]) + 4); len += (ntohs(*((short *) &(buffer[len + 2]))) + 1) * 4)
	len += 4;
	while(len < ((buffer[2]<<8 | buffer[3]) + 4))
	{
		printf("----> len:%d, len1:%d, len2:%d, len3:%d\n", 
				(buffer[2]<<8 | buffer[3]) + 4, len, buffer[4 + 1 + len],
				(ntohs(*((short *) &(buffer[len + 2]))) + 1) * 4);
        switch (buffer[1 + len])
		{
	        case SR:
				printf("RTCP Decode SR packet received\n");
	            HI_RTCP_Decode_SR(buffer, len, &rtcp_stats);
				len += (ntohs(*((short *) &(buffer[len + 2]))) + 1) * 4;	//len of SR is 8
	            break;
	        case RR:
				printf("RTCP Decode RR packet received\n");
	            HI_RTCP_Decode_RR(buffer, len, &rtcp_stats);
				len += (ntohs(*((short *) &(buffer[len + 2]))) + 1) * 4;	//len of RR is 8
	            break;
	        case SDES:
				printf("RTCP Decode SDES packet received, len:%d, item:%d\n", len + 8, buffer[len + 8]);
	            HI_RTCP_Decode_SDES(buffer, len, &rtcp_stats);
				len += (ntohs(*((short *) &(buffer[len + 2]))) + 1) * 4;	//len of SDES is 8
	            break;
	        case BYE:
	            printf("RTCP BYE packet received\n");
				HI_RTCP_Decode_BYE(buffer, len, &rtcp_stats);
				len += (ntohs(*((short *) &(buffer[len + 2]))) + 1) * 4;	//len of BYE is 8
	            break;
	        case APP:
	            printf("RTCP APP packet received\n");
				HI_RTCP_Decode_APP(buffer, len, &rtcp_stats);
				len += (ntohs(*((short *) &(buffer[len + 2]))) + 1) * 4;	//len of APP is 8
	            break;
	        default:
	            printf("Unknown RTCP received and ignored.\n");
	            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}


#if 0
HI_S32 HI_RTP_Send(HI_SOCKET s32WritSock,HI_U8* pu8Buff, HI_S32 s32DataLen,
                       struct sockaddr* pstruToAddr)
{
    HI_S32 s32RemSize =0;
    HI_S32 s32AddrLen = 0;
    HI_S32 s32Size    = 0;
    const HI_CHAR*  ps8BufferPos = NULL;

    s32RemSize = s32DataLen;
    ps8BufferPos = pu8Buff;
    while(s32RemSize > 0)
    {

        s32AddrLen = (HI_NULL == pstruToAddr) ? 0 : sizeof(struct sockaddr);
        s32Size = sendto(s32WritSock, pu8Buff, s32RemSize, 0, pstruToAddr, 
                            s32AddrLen);
        if(s32Size < 0)
        {
            return HI_ERR_RTP_SEND;
        }
        s32RemSize -= s32Size;
        ps8BufferPos += s32Size;
    }

    return HI_SUCCESS;
}

HI_S32 HI_RTP_AudioPTConvert(HI_U32 aenctype)
{
    HI_S32 pt = RTP_PT_INVALID;
    switch(aenctype)
    {
        case MT_AUDIO_FORMAT_G711A: //   = 1,   /*G.711 A率*/
            pt = RTP_PT_ALAW;
            break;
        case MT_AUDIO_FORMAT_G711Mu: //  = 2,   /*G.711 Mu率*/
            pt = RTP_PT_ULAW;
            break;
        case MT_AUDIO_FORMAT_ADPCM:  //3,   /*ADPCM */
            pt = RTP_PT_ADPCM;
            break;
        case MT_AUDIO_FORMAT_G726:   // = 4,   /*G.726 */
            pt = RTP_PT_G726;
            break;
        default:
            pt = RTP_PT_INVALID;
            break;
    }
    return pt;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


