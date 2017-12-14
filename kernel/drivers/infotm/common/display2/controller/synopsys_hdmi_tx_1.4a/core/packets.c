/*
 * @file:packets.c
 *
 *  Created on: Jul 7, 2010
 *      Author: klabadi & dlopo
 */
#include "packets.h"
#include "log.h"
#include "halFrameComposerGcp.h"
#include "halFrameComposerPackets.h"
#include "halFrameComposerGamut.h"
#include "halFrameComposerAcp.h"
#include "halFrameComposerAudioInfo.h"
#include "halFrameComposerAvi.h"
#include "halFrameComposerIsrc.h"
#include "halFrameComposerSpd.h"
#include "halFrameComposerVsd.h"
#include "error.h"

#define ACP_PACKET_SIZE 16
#define ISRC_PACKET_SIZE 16
static const u16 FC_BASE_ADDR = 0x1000;

int packets_Initialize(u16 baseAddr)
{
	LOG_TRACE();
	packets_DisableAllPackets(baseAddr + FC_BASE_ADDR);
	return TRUE;
}

int packets_Configure(u16 baseAddr, videoParams_t * video,
		productParams_t * prod)
{
	u8 send3d = FALSE;
	LOG_TRACE();
	if (videoParams_GetHdmiVideoFormat(video) == 2)
	{
		if (videoParams_Get3dStructure(video) == 6
				|| videoParams_Get3dStructure(video) == 8)
		{
			u8 data[3];
			data[0] = videoParams_GetHdmiVideoFormat(video) << 5;
			data[1] = videoParams_Get3dStructure(video) << 4;
			data[2] = videoParams_Get3dExtData(video) << 4;
			packets_VendorSpecificInfoFrame(baseAddr, 0x000C03,
					data, sizeof(data), 1); /* HDMI Licensing, LLC */
			send3d = TRUE;
		}
		else
		{
			LOG_ERROR2("3D structure not supported",
					videoParams_Get3dStructure(video));
			error_Set(ERR_3D_STRUCT_NOT_SUPPORTED);
			return FALSE;
		}
	}
	if (prod != 0)
	{
		if (productParams_IsSourceProductValid(prod))
		{
			packets_SourceProductInfoFrame(baseAddr,
					productParams_GetVendorName(prod),
					productParams_GetVendorNameLength(prod),
					productParams_GetProductName(prod),
					productParams_GetProductNameLength(prod),
					productParams_GetSourceType(prod), 1);
		}
		if (productParams_IsVendorSpecificValid(prod))
		{
			if (send3d)
			{
				LOG_WARNING("forcing Vendor Specific InfoFrame, 3D configuration will be ignored");
				error_Set(ERR_FORCING_VSD_3D_IGNORED);
			}
			packets_VendorSpecificInfoFrame(baseAddr,
					productParams_GetOUI(prod), productParams_GetVendorPayload(
							prod), productParams_GetVendorPayloadLength(prod),
					1);
		}
	}
	else
	{
		LOG_WARNING("No product info provided: not configured");
	}
	/* set values that shall not change */
	halFrameComposerPackets_MetadataFrameInterpolation(baseAddr + FC_BASE_ADDR,
			1);
	halFrameComposerPackets_MetadataFramesPerPacket(baseAddr + FC_BASE_ADDR, 1);
	halFrameComposerPackets_MetadataLineSpacing(baseAddr + FC_BASE_ADDR, 1);
	halFrameComposerGcp_DefaultPhase(baseAddr + FC_BASE_ADDR,
			videoParams_GetPixelPackingDefaultPhase(video) == 1); /* default phase 1 = true */
	halFrameComposerGamut_Profile(baseAddr + FC_BASE_ADDR + 0x100, 0x0); /* P0 */
	halFrameComposerGamut_PacketsPerFrame(baseAddr + FC_BASE_ADDR + 0x100, 0x1); /* P0 */
	halFrameComposerGamut_PacketLineSpacing(baseAddr + FC_BASE_ADDR + 0x100,
			0x1);
	packets_AuxiliaryVideoInfoFrame(baseAddr, video);
	return TRUE;
}

void packets_AudioContentProtection(u16 baseAddr, u8 type, const u8 * fields,
		u8 length, u8 autoSend)
{
	u8 newFields[ACP_PACKET_SIZE];
	u16 i = 0;
	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, ACP_TX);
	halFrameComposerAcp_Type(baseAddr + FC_BASE_ADDR, type);

	for (i = 0; i < length; i++)
	{
		newFields[i] = fields[i];
	}
	if (length < ACP_PACKET_SIZE)
	{
		for (i = length; i < ACP_PACKET_SIZE; i++)
		{
			newFields[i] = 0; /* Padding */
		}
		length = ACP_PACKET_SIZE;
	}
	halFrameComposerAcp_TypeDependentFields(baseAddr + FC_BASE_ADDR, newFields,
			length);
	if (!autoSend)
	{
		halFrameComposerPackets_ManualSend(baseAddr + FC_BASE_ADDR, ACP_TX);
	}
	else
	{
		halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, autoSend,
				ACP_TX);
	}

}

void packets_IsrcPackets(u16 baseAddr, u8 initStatus, const u8 * codes,
		u8 length, u8 autoSend)
{
	u16 i = 0;
	u8 newCodes[ISRC_PACKET_SIZE * 2];
	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, ISRC1_TX);
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, ISRC2_TX);
	halFrameComposerIsrc_Status(baseAddr + FC_BASE_ADDR, initStatus);

	for (i = 0; i < length; i++)
	{
		newCodes[i] = codes[i];
	}

	if (length > ISRC_PACKET_SIZE)
	{
		for (i = length; i < (ISRC_PACKET_SIZE * 2); i++)
		{
			newCodes[i] = 0; /* Padding */
		}
		length = (ISRC_PACKET_SIZE * 2);
		halFrameComposerIsrc_Isrc2Codes(baseAddr + FC_BASE_ADDR, newCodes
				+ (ISRC_PACKET_SIZE * sizeof(u8)), length - ISRC_PACKET_SIZE);
		halFrameComposerIsrc_Cont(baseAddr + FC_BASE_ADDR, 1);
		halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, autoSend,
				ISRC2_TX);
		if (!autoSend)
		{
			halFrameComposerPackets_ManualSend(baseAddr + FC_BASE_ADDR,
					ISRC2_TX);
		}
	}
	if (length < ISRC_PACKET_SIZE)
	{
		for (i = length; i < ISRC_PACKET_SIZE; i++)
		{
			newCodes[i] = 0; /* Padding */
		}
		length = ISRC_PACKET_SIZE;
		halFrameComposerIsrc_Cont(baseAddr + FC_BASE_ADDR, 0);
	}
	halFrameComposerIsrc_Isrc1Codes(baseAddr + FC_BASE_ADDR, newCodes, length); /* first part only */
	halFrameComposerIsrc_Valid(baseAddr + FC_BASE_ADDR, 1);
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, autoSend,
			ISRC1_TX);
	if (!autoSend)
	{
		halFrameComposerPackets_ManualSend(baseAddr + FC_BASE_ADDR, ISRC1_TX);
	}
}

void packets_AudioInfoFrame(u16 baseAddr, audioParams_t *params)
{
	LOG_TRACE();
	halFrameComposerAudioInfo_ChannelCount(baseAddr + FC_BASE_ADDR, audioParams_ChannelCount(
			params));
	halFrameComposerAudioInfo_AllocateChannels(baseAddr + FC_BASE_ADDR,
			audioParams_GetChannelAllocation(params));
	halFrameComposerAudioInfo_LevelShiftValue(baseAddr + FC_BASE_ADDR,
			audioParams_GetLevelShiftValue(params));
	halFrameComposerAudioInfo_DownMixInhibit(baseAddr + FC_BASE_ADDR,
			audioParams_GetDownMixInhibitFlag(params));
	if ((audioParams_GetCodingType(params) == ONE_BIT_AUDIO)
			|| (audioParams_GetCodingType(params) == DST))
	{
		/* Audio InfoFrame sample frequency when OBA or DST */
		if (audioParams_GetSamplingFrequency(params) == 32000)
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 1);
		}
		else if (audioParams_GetSamplingFrequency(params) == 44100)
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 2);
		}
		else if (audioParams_GetSamplingFrequency(params) == 48000)
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 3);
		}
		else if (audioParams_GetSamplingFrequency(params) == 88200)
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 4);
		}
		else if (audioParams_GetSamplingFrequency(params) == 96000)
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 5);
		}
		else if (audioParams_GetSamplingFrequency(params) == 176400)
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 6);
		}
		else if (audioParams_GetSamplingFrequency(params) == 192000)
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 7);
		}
		else
		{
			halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 0);
		}
	}
	else
	{
		halFrameComposerAudioInfo_SampleFreq(baseAddr + FC_BASE_ADDR, 0); /* otherwise refer to stream header (0) */
	}
	halFrameComposerAudioInfo_CodingType(baseAddr + FC_BASE_ADDR, 0); /* for HDMI refer to stream header  (0) */
	halFrameComposerAudioInfo_SamplingSize(baseAddr + FC_BASE_ADDR, 0); /* for HDMI refer to stream header  (0) */
}

void packets_AvMute(u16 baseAddr, u8 enable)
{
	LOG_TRACE();
	halFrameComposerGcp_AvMute(baseAddr + FC_BASE_ADDR, enable);
}
void packets_IsrcStatus(u16 baseAddr, u8 status)
{
	LOG_TRACE();
	halFrameComposerIsrc_Status(baseAddr + FC_BASE_ADDR, status);
}
void packets_StopSendAcp(u16 baseAddr)
{
	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, ACP_TX);
}

void packets_StopSendIsrc1(u16 baseAddr)
{
	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, ISRC1_TX);
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, ISRC2_TX);
}

void packets_StopSendIsrc2(u16 baseAddr)
{
	LOG_TRACE();
	halFrameComposerIsrc_Cont(baseAddr + FC_BASE_ADDR, 0);
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, ISRC2_TX);
}

void packets_StopSendSpd(u16 baseAddr)
{
	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, SPD_TX);
}

void packets_StopSendVsd(u16 baseAddr)
{
	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, VSD_TX);
}

void packets_GamutMetadataPackets(u16 baseAddr, const u8 * gbdContent,
		u8 length)
{
	LOG_TRACE();
	halFrameComposerGamut_EnableTx(baseAddr + FC_BASE_ADDR + 0x100, 1);
	halFrameComposerGamut_AffectedSeqNo(
			baseAddr + FC_BASE_ADDR + 0x100,
			(halFrameComposerGamut_CurrentSeqNo(baseAddr + FC_BASE_ADDR + 0x100)
					+ 1) % 16); /* sequential */
	halFrameComposerGamut_Content(baseAddr + FC_BASE_ADDR + 0x100, gbdContent,
			length);
	halFrameComposerGamut_UpdatePacket(baseAddr + FC_BASE_ADDR + 0x100); /* set next_field to 1 */
}

void packets_DisableAllPackets(u16 baseAddr)
{
	LOG_TRACE();
	halFrameComposerPackets_DisableAll(baseAddr + FC_BASE_ADDR);
}

int packets_SourceProductInfoFrame(u16 baseAddr, const u8 * vName, u8 vLength,
		const u8 * pName, u8 pLength, u8 code, u8 autoSend)
{
	const unsigned short vSize = 8;
	const unsigned short pSize = 16;

	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, SPD_TX); /* prevent sending half the info. */
	if (vName == 0)
	{
		error_Set(ERR_INVALID_PARAM_VENDOR_NAME);
		LOG_WARNING("invalid parameter");
		return FALSE;
	}
	if (vLength > vSize)
	{
		vLength = vSize;
		LOG_WARNING("vendor name truncated");
	}
	if (pName == 0)
	{
		error_Set(ERR_INVALID_PARAM_PRODUCT_NAME);
		LOG_WARNING("invalid parameter");
		return FALSE;
	}
	if (pLength > pSize)
	{
		pLength = pSize;
		LOG_WARNING("product name truncated");
	}
	halFrameComposerSpd_VendorName(baseAddr + FC_BASE_ADDR, vName, vLength);
	halFrameComposerSpd_ProductName(baseAddr + FC_BASE_ADDR, pName, pLength);

	halFrameComposerSpd_SourceDeviceInfo(baseAddr + FC_BASE_ADDR, code);
	if (autoSend)
	{
		halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, autoSend, SPD_TX);
	}
	else
	{
		halFrameComposerPackets_ManualSend(baseAddr + FC_BASE_ADDR, SPD_TX);
	}
	return TRUE;
}

int packets_VendorSpecificInfoFrame(u16 baseAddr, u32 oui, const u8 * payload,
		u8 length, u8 autoSend)
{
	LOG_TRACE();
	halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, 0, VSD_TX); /* prevent sending half the info. */
	halFrameComposerVsd_VendorOUI(baseAddr + FC_BASE_ADDR, oui);
	if (halFrameComposerVsd_VendorPayload(baseAddr + FC_BASE_ADDR, payload, length))
	{
		return FALSE; /* DEFINE ERROR */
	}
	if (autoSend)
	{
		halFrameComposerPackets_AutoSend(baseAddr + FC_BASE_ADDR, autoSend, VSD_TX);
	}
	else
	{
		halFrameComposerPackets_ManualSend(baseAddr + FC_BASE_ADDR, VSD_TX);
	}
	return TRUE;
}

void packets_AuxiliaryVideoInfoFrame(u16 baseAddr, videoParams_t *videoParams)
{
	u16 endTop = 0;
	u16 startBottom = 0;
	u16 endLeft = 0;
	u16 startRight = 0;
	LOG_TRACE();
	if (videoParams_GetEncodingOut(videoParams) == RGB)
	{
		halFrameComposerAvi_RgbYcc(baseAddr + FC_BASE_ADDR, 0);
	}
	else if (videoParams_GetEncodingOut(videoParams) == YCC422)
	{
		halFrameComposerAvi_RgbYcc(baseAddr + FC_BASE_ADDR, 1);
	}
	else if (videoParams_GetEncodingOut(videoParams) == YCC444)
	{
		halFrameComposerAvi_RgbYcc(baseAddr + FC_BASE_ADDR, 2);
	}
	halFrameComposerAvi_ScanInfo(baseAddr + FC_BASE_ADDR, videoParams_GetScanInfo(videoParams));
	if (dtd_GetHImageSize(videoParams_GetDtd(videoParams)) != 0
			|| dtd_GetVImageSize(videoParams_GetDtd(videoParams)) != 0)
	{
		u8 pic = (dtd_GetHImageSize(videoParams_GetDtd(videoParams)) * 10)
				% dtd_GetVImageSize(videoParams_GetDtd(videoParams));
		halFrameComposerAvi_PicAspectRatio(baseAddr + FC_BASE_ADDR, (pic > 5) ? 2 : 1); /* 16:9 or 4:3 */
	}
	else
	{
		halFrameComposerAvi_PicAspectRatio(baseAddr + FC_BASE_ADDR, 0); /* No Data */
	}
	halFrameComposerAvi_IsItContent(baseAddr + FC_BASE_ADDR, videoParams_GetItContent(
			videoParams));

	halFrameComposerAvi_QuantizationRange(baseAddr + FC_BASE_ADDR,
			videoParams_GetRgbQuantizationRange(videoParams));
	halFrameComposerAvi_NonUniformPicScaling(baseAddr + FC_BASE_ADDR,
			videoParams_GetNonUniformScaling(videoParams));
	if (dtd_GetCode(videoParams_GetDtd(videoParams)) != (u8) (-1))
	{
		halFrameComposerAvi_VideoCode(baseAddr + FC_BASE_ADDR, dtd_GetCode(
		videoParams_GetDtd(videoParams)));
	}
	else
	{
		halFrameComposerAvi_VideoCode(baseAddr + FC_BASE_ADDR, 0);
	}
	if (videoParams_GetColorimetry(videoParams) == EXTENDED_COLORIMETRY)
	{ /* ext colorimetry valid */
		if (videoParams_GetExtColorimetry(videoParams) != (u8) (-1))
		{
			halFrameComposerAvi_ExtendedColorimetry(baseAddr + FC_BASE_ADDR,
					videoParams_GetExtColorimetry(videoParams));
			halFrameComposerAvi_Colorimetry(baseAddr + FC_BASE_ADDR,
					videoParams_GetColorimetry(videoParams)); /* EXT-3 */
		}
		else
		{
			halFrameComposerAvi_Colorimetry(baseAddr + FC_BASE_ADDR, 0); /* No Data */
		}
	}
	else
	{
		halFrameComposerAvi_Colorimetry(baseAddr + FC_BASE_ADDR, videoParams_GetColorimetry(
				videoParams)); /* NODATA-0/ 601-1/ 709-2/ EXT-3 */
	}
	if (videoParams_GetActiveFormatAspectRatio(videoParams) != 0)
	{
		halFrameComposerAvi_ActiveFormatAspectRatio(baseAddr + FC_BASE_ADDR,
				videoParams_GetActiveFormatAspectRatio(videoParams));
		halFrameComposerAvi_ActiveAspectRatioValid(baseAddr + FC_BASE_ADDR, 1);
	}
	else
	{
		halFrameComposerAvi_ActiveAspectRatioValid(baseAddr + FC_BASE_ADDR, 0);
	}
	if (videoParams_GetEndTopBar(videoParams) != (u16) (-1)
			|| videoParams_GetStartBottomBar(videoParams) != (u16) (-1))
	{
		if (videoParams_GetEndTopBar(videoParams) != (u16) (-1))
		{
			endTop = videoParams_GetEndTopBar(videoParams);
		}
		if (videoParams_GetStartBottomBar(videoParams) != (u16) (-1))
		{
			startBottom = videoParams_GetStartBottomBar(videoParams);
		}
		halFrameComposerAvi_HorizontalBars(baseAddr + FC_BASE_ADDR, endTop, 
		startBottom);
		halFrameComposerAvi_HorizontalBarsValid(baseAddr + FC_BASE_ADDR, 1);
	}
	else
	{
		halFrameComposerAvi_HorizontalBarsValid(baseAddr + FC_BASE_ADDR, 0);
	}
	if (videoParams_GetEndLeftBar(videoParams) != (u16) (-1)
			|| videoParams_GetStartRightBar(videoParams) != (u16) (-1))
	{
		if (videoParams_GetEndLeftBar(videoParams) != (u16) (-1))
		{
			endLeft = videoParams_GetEndLeftBar(videoParams);
		}
		if (videoParams_GetStartRightBar(videoParams) != (u16) (-1))
		{
			startRight = videoParams_GetStartRightBar(videoParams);
		}
		halFrameComposerAvi_VerticalBars(baseAddr + FC_BASE_ADDR, endLeft, startRight);
		halFrameComposerAvi_VerticalBarsValid(baseAddr + FC_BASE_ADDR, 1);
	}
	else
	{
		halFrameComposerAvi_VerticalBarsValid(baseAddr + FC_BASE_ADDR, 0);
	}
	halFrameComposerAvi_OutPixelRepetition(baseAddr + FC_BASE_ADDR,
			(dtd_GetPixelRepetitionInput(videoParams_GetDtd(videoParams)) + 1)
					* (videoParams_GetPixelRepetitionFactor(videoParams) + 1)
					- 1);
}
