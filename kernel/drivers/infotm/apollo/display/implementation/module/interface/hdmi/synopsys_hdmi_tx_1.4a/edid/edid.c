/*
 * edid.c
 *
 *  Created on: Jul 23, 2010
 *      Author: klabadi & dlopo
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include "edid.h"
#include "system.h"
#include "bitOperation.h"
#include "log.h"
#include "error.h"

extern verbose_t log_Verbose ;

const u16 I2CM_BASE_ADDR = 0x7E00;

u8 edid_request = 0;

u8 edid_mBlocksNo;
/**
 * Local variable hold the number of the currently read byte in a certain block of the EDID structure
 */
u8 edid_mCurrAddress;
/**
 * Local variable hold the number of the currently read block in the EDID structure.
 */
u8 edid_mCurrBlockNo;
/**
 * The accumulator of the block check sum value.
 */
u8 edid_mBlockSum;
/**
 * Local variable to hold the status when the reading of the whole EDID structure
 */
edid_status_t edid_mStatus = EDID_IDLE;
/**
 * Local variable holding the buffer of 128 read bytes.
 */
u8 edid_mBuffer[0x80];
/**
 * Array to hold all the parsed Detailed Timing Descriptors.
 */
dtd_t edid_mDtd[32];

unsigned int edid_mDtdIndex;
/**
 * array to hold all the parsed Short Video Descriptors.
 */
shortVideoDesc_t edid_mSvd[128];

unsigned int edid_mSvdIndex;
/**
 * array to hold all the parsed Short Audio Descriptors.
 */
shortAudioDesc_t edid_mSad[128];

unsigned int edid_mSadIndex;
/**
 * A string to hold the Monitor Name parsed from EDID.
 */
char edid_mMonitorName[13];

int edid_mYcc444Support;

int edid_mYcc422Support;

int edid_mBasicAudioSupport;

int edid_mUnderscanSupport;

hdmivsdb_t edid_mHdmivsdb;

monitorRangeLimits_t edid_mMonitorRangeLimits;

videoCapabilityDataBlock_t edid_mVideoCapabilityDataBlock;

colorimetryDataBlock_t edid_mColorimetryDataBlock;

speakerAllocationDataBlock_t edid_mSpeakerAllocationDataBlock;

int edid_Initialize(u16 baseAddr, u16 sfrClock)
{
	LOG_TRACE2(baseAddr, sfrClock);
	if (sfrClock != 2500)
	{
		error_Set(ERR_SFR_CLOCK_NOT_SUPPORTED);
		LOG_ERROR("SFR clock is not 25MHz");
		return FALSE;
	}

	halEdid_MaskInterrupts(baseAddr + I2CM_BASE_ADDR, TRUE);
	edid_Reset(baseAddr);
	/* set clock division - SFR is always 25MHz (TODO) */
	halEdid_MasterClockDivision(baseAddr + I2CM_BASE_ADDR, 0x05);
	halEdid_StandardSpeedCounter(baseAddr + I2CM_BASE_ADDR, 0x00790091);
	halEdid_FastSpeedCounter(baseAddr + I2CM_BASE_ADDR, 0x000F0021);

	/* set address to EDID address -see spec */
	halEdid_SlaveAddress(baseAddr + I2CM_BASE_ADDR, 0x50); /* HW deals with LSB (alternating the address between 0xA0 & 0xA1) */
	halEdid_SegmentAddr(baseAddr + I2CM_BASE_ADDR, 0x30); /* HW deals with LSB (making the segment address go to 60) */

	/* read EDID */
	edid_mStatus = EDID_READING;
	LOG_NOTICE("reading EDID");
	halEdid_MaskInterrupts(baseAddr + I2CM_BASE_ADDR, FALSE); /* enable interrupts */
	edid_ReadRequest(baseAddr, 0, 0);
	return TRUE;
}

int edid_Standby(u16 baseAddr)
{
	LOG_TRACE();
	halEdid_MaskInterrupts(baseAddr + I2CM_BASE_ADDR, TRUE); /* disable intterupts */
	//edid_mStatus = EDID_IDLE;
	edid_mStatus = EDID_DONE;		// William
	return TRUE;
}

u8 edid_EventHandler(u16 baseAddr, int hpd, u8 state)
{
	LOG_TRACE2(hpd, state);
	if (edid_mStatus != EDID_READING)
	{
		LOG_DEBUG2("Inappropriate mStatus", edid_mStatus);
		return EDID_IDLE;
	}
	else if (!hpd)
	{ /* hot plug detected without cable disconnection */
		error_Set(ERR_HPD_LOST);
		LOG_WARNING("hpd");
		edid_mStatus = EDID_ERROR;
	}
	else if ((state & BIT(0)) != 0) /* error */
	{ 
		LOG_WARNING("error");
		edid_mStatus = EDID_ERROR;
	}
	else if ((state & BIT(1)) != 0) /* done */
	{ 
		if (edid_mCurrAddress >= sizeof(edid_mBuffer))
		{
			error_Set(ERR_OVERFLOW);
			LOG_WARNING("overflow");
			edid_mStatus = EDID_ERROR;
		}
		else
		{
			edid_mBuffer[edid_mCurrAddress] = halEdid_ReadData(baseAddr
					+ I2CM_BASE_ADDR);
			edid_mBlockSum += edid_mBuffer[edid_mCurrAddress++];
			if (edid_mCurrAddress >= sizeof(edid_mBuffer))
			{
				/*check if checksum is correct (CEA-861 D Spec p108) */
				if (edid_mBlockSum % 0x100 == 0)
				{
					LOG_NOTICE("block checksum correct");
					edid_ParseBlock(baseAddr, edid_mBuffer);
					edid_mCurrAddress = 0;
					edid_mCurrBlockNo++;
					edid_mBlockSum = 0;
					if (edid_mCurrBlockNo >= edid_mBlocksNo)
					{
						edid_mStatus = EDID_DONE; 
					}
				}
				else
				{
					error_Set(ERR_BLOCK_CHECKSUM_INVALID);
					LOG_WARNING("block checksum invalid");
					edid_mStatus = EDID_ERROR; 
				}
			}
		}
	}
	if (edid_mStatus == EDID_READING)
	{
		edid_ReadRequest(baseAddr, edid_mCurrAddress, edid_mCurrBlockNo);
		edid_request = 1;
	}
	else if (edid_mStatus == EDID_DONE)
	{
		edid_mBlocksNo = 1;
		edid_mCurrBlockNo = 0;
		edid_mCurrAddress = 0;
		edid_mBlockSum = 0;
	}
	return edid_mStatus;
}

unsigned int edid_GetDtdCount(u16 baseAddr)
{
	LOG_TRACE();
	return edid_mDtdIndex;
}

int edid_GetDtd(u16 baseAddr, unsigned int n, dtd_t * dtd)
{
	LOG_TRACE1(n);
	if (n < edid_GetDtdCount(baseAddr))
	{
		*dtd = edid_mDtd[n];
		return TRUE;
	}
	return FALSE;
}

int edid_GetHdmivsdb(u16 baseAddr, hdmivsdb_t * vsdb)
{
	LOG_TRACE();
	if (edid_mHdmivsdb.mValid)
	{
		*vsdb = edid_mHdmivsdb;
		return TRUE;
	}
	return FALSE;
}

int edid_GetMonitorName(u16 baseAddr, char * name, unsigned int length)
{
	unsigned int i = 0;
	LOG_TRACE();
	for (i = 0; i < length && i < sizeof(edid_mMonitorName); i++)
	{
		name[i] = edid_mMonitorName[i];
	}
	return i;
}

int edid_GetMonitorRangeLimits(u16 baseAddr, monitorRangeLimits_t * limits)
{
	LOG_TRACE();
	if (edid_mMonitorRangeLimits.mValid)
	{
		*limits = edid_mMonitorRangeLimits;
		return TRUE;
	}
	return FALSE;
}

unsigned int edid_GetSvdCount(u16 baseAddr)
{
	LOG_TRACE();
	return edid_mSvdIndex;
}

int edid_GetSvd(u16 baseAddr, unsigned int n, shortVideoDesc_t * svd)
{
	LOG_TRACE1(n);
	if (n < edid_GetSvdCount(baseAddr))
	{
		*svd = edid_mSvd[n];
		return TRUE;
	}
	return FALSE;
}

unsigned int edid_GetSadCount(u16 baseAddr)
{
	LOG_TRACE();
	return edid_mSadIndex;
}

int edid_GetSad(u16 baseAddr, unsigned int n, shortAudioDesc_t * sad)
{
	LOG_TRACE1(n);
	if (n < edid_GetSadCount(baseAddr))
	{
		*sad = edid_mSad[n];
		return TRUE;
	}
	return FALSE;
}

int edid_GetVideoCapabilityDataBlock(u16 baseAddr,
		videoCapabilityDataBlock_t * capability)
{
	LOG_TRACE();
	if (edid_mVideoCapabilityDataBlock.mValid)
	{
		*capability = edid_mVideoCapabilityDataBlock;
		return TRUE;
	}
	return FALSE;
}

int edid_GetSpeakerAllocationDataBlock(u16 baseAddr,
		speakerAllocationDataBlock_t * allocation)
{
	LOG_TRACE();
	if (edid_mSpeakerAllocationDataBlock.mValid)
	{
		*allocation = edid_mSpeakerAllocationDataBlock;
		return TRUE;
	}
	return FALSE;
}

int edid_GetColorimetryDataBlock(u16 baseAddr,
		colorimetryDataBlock_t * colorimetry)
{
	LOG_TRACE();
	if (edid_mColorimetryDataBlock.mValid)
	{
		*colorimetry = edid_mColorimetryDataBlock;
		return TRUE;
	}
	return FALSE;
}

int edid_SupportsBasicAudio(u16 baseAddr)
{
	return edid_mBasicAudioSupport;
}

int edid_SupportsUnderscan(u16 baseAddr)
{
	return edid_mUnderscanSupport;
}

int edid_SupportsYcc422(u16 baseAddr)
{
	return edid_mYcc422Support;
}

int edid_SupportsYcc444(u16 baseAddr)
{
	return edid_mYcc444Support;
}

int edid_Reset(u16 baseAddr)
{
	unsigned i = 0;
	LOG_TRACE();
	edid_mBlocksNo = 1;
	edid_mCurrBlockNo = 0;
	edid_mCurrAddress = 0;
	edid_mBlockSum = 0;
	edid_mStatus = EDID_READING; 
	for (i = 0; i < sizeof(edid_mBuffer); i++)
	{
		edid_mBuffer[i] = 0;
	}
	for (i = 0; i < sizeof(edid_mMonitorName); i++)
	{
		edid_mMonitorName[i] = 0;
	}
	edid_mBasicAudioSupport = FALSE;
	edid_mUnderscanSupport = FALSE;
	edid_mYcc422Support = FALSE;
	edid_mYcc444Support = FALSE;
	edid_mDtdIndex = 0;
	edid_mSadIndex = 0;
	edid_mSvdIndex = 0;
	hdmivsdb_Reset(&edid_mHdmivsdb);
	monitorRangeLimits_Reset(&edid_mMonitorRangeLimits);
	videoCapabilityDataBlock_Reset(&edid_mVideoCapabilityDataBlock);
	colorimetryDataBlock_Reset(&edid_mColorimetryDataBlock);
	speakerAllocationDataBlock_Reset(&edid_mSpeakerAllocationDataBlock);
	return TRUE;
}

int edid_ReadRequest(u16 baseAddr, u8 address, u8 blockNo)
{
	/*to incorporate extensions we have to include the following - see VESA E-DDC spec. P 11 */
	u8 sPointer = blockNo / 2;
	u8 edidAddress = ((blockNo % 2) * 0x80) + address;

	//LOG_DEBUG3("blockNo-address", blockNo, address);
	//LOG_DEBUG3("sPointer, edidAddress", sPointer, edidAddress);

	//LOG_TRACE2(sPointer, edidAddress);
	halEdid_RequestAddr(baseAddr + I2CM_BASE_ADDR, edidAddress);
	halEdid_SegmentPointer(baseAddr + I2CM_BASE_ADDR, sPointer);
	if (sPointer == 0)
	{
		halEdid_RequestRead(baseAddr + I2CM_BASE_ADDR);
	}
	else
	{
		halEdid_RequestExtRead(baseAddr + I2CM_BASE_ADDR);
	}
	return TRUE;
}

int edid_ParseBlock(u16 baseAddr, u8 * buffer)
{
	const unsigned DTD_SIZE = 0x12;
	const unsigned TIMING_MODES = 0x36;
	dtd_t tmpDtd;
	unsigned c = 0;
	unsigned i = 0;
	unsigned char edids[100] = {0};
	unsigned char * edids_ptr = edids;
	
	LOG_DEBUGS("\n\n__________________________________________________\n\n");
	LOG_TRACE();
	LOG_DEBUG2("edid_mCurrentBlockNo", edid_mCurrBlockNo);
	LOG_DEBUG2("Segment Pointer", edid_mCurrBlockNo/2);

	for (i = 0; i <= 128; i++) {
		if (i % 8 == 0) {
			LOG_DEBUGFSP(edids_ptr, "  ");
			edids_ptr += 1;
		}
		if (i % 16 == 0) {
			LOG_DEBUGFSP(edids_ptr, "\n");
			LOG_DEBUGF("%s", edids);
			memset(edids, 0, 100);
			edids_ptr = edids;
		}
		LOG_DEBUGFSP(edids_ptr, "%02X ", buffer[i]);
		edids_ptr += 3;
	}
	LOG_DEBUGF("\n\n");
	
	if (edid_mCurrBlockNo == 0)
	{
		LOG_DEBUG("Base EDID Block");
		if (buffer[0] == 0x00)
		{ /* parse block zero */
			edid_mBlocksNo = buffer[126] + 1;
			LOG_DEBUG2("Extension flag", buffer[126]);
			/* parse DTD's */
			LOG_DEBUGF("\n");
			for (i = TIMING_MODES; i < (TIMING_MODES + (DTD_SIZE * 4)); i
					+= DTD_SIZE)
			{
				if (bitOperation_Bytes2Word(buffer[i + 1], buffer[i + 0]) > 0)
				{
					if (dtd_Parse(&tmpDtd, buffer + i) == TRUE)
					{
						if (edid_mDtdIndex < (sizeof(edid_mDtd)
								/ sizeof(dtd_t)))
						{
							edid_mDtd[edid_mDtdIndex++] = tmpDtd;
						}
						else
						{
							error_Set(ERR_DTD_BUFFER_FULL);
							LOG_WARNING("buffer full - DTD ignored");
						}
					}
					else
					{
						LOG_WARNING("DTD corrupt");
					}
				}
				else if ((buffer[i + 2] == 0) && (buffer[i + 4] == 0))
				{
					/* it is a Display-monitor Descriptor */
					if (buffer[i + 3] == 0xFC)
					{ /* Monitor Name */
						LOG_DEBUGS("\n");
						LOG_DEBUG("Monitor Name");
						for (c = 0; c < 13; c++)
						{
							edid_mMonitorName[c] = buffer[i + c + 5];
						}
						LOG_DEBUG(edid_mMonitorName);
					}
					else if (buffer[i + 3] == 0xFD)
					{ /* Monitor Range Limits */
						LOG_DEBUG("Monitor Range Limits");
						if (monitorRangeLimits_Parse(&edid_mMonitorRangeLimits,
								buffer + i) != TRUE)
						{
							LOG_WARNING2("Monitor Range Limits corrupt", i);
						}
					}
				}
			}
		}
	}
	else if (buffer[0] == 0xF0)
	{ /* Block Map Extension */
		LOG_DEBUG("Block Map Extension");
		/* last is checksum */
		for (i = 1; i < (sizeof(edid_mBuffer) - 1); i++)
		{
			if (buffer[i] == 0x00)
			{
				break;
			}
		}
		if (edid_mBlocksNo < 128)
		{
			if (i > edid_mBlocksNo)
			{ /* N (no of extensions) does NOT include Block maps */
				edid_mBlocksNo += 2;
			}
			else if (i == edid_mBlocksNo)
			{
				edid_mBlocksNo += 1;
			}
		}
		else
		{
			i += 127;
			if (i > edid_mBlocksNo)
			{ /* N (no of extensions) does NOT include Block maps */
				edid_mBlocksNo += 3;
			}
			else if (i == edid_mBlocksNo)
			{
				edid_mBlocksNo += 1;
			}
		}
	}
	else if (buffer[0] == 0x02)
	{ /* CEA Extension block */
		LOG_DEBUG("CEA Extension block");
		if (buffer[1] == 0x03)
		{ /* revision number (only rev3 is allowed by HDMI spec) */
			u8 offset = buffer[2];
			LOG_DEBUG("revision 3");
			edid_mYcc422Support = bitOperation_BitField(buffer[3], 4, 1) == 1;
			edid_mYcc444Support = bitOperation_BitField(buffer[3], 5, 1) == 1;
			edid_mBasicAudioSupport = bitOperation_BitField(buffer[3], 6, 1)
					== 1;
			edid_mUnderscanSupport = bitOperation_BitField(buffer[3], 7, 1)
					== 1;
			LOG_DEBUG2("edid_mYcc422Support", edid_mYcc422Support);
			LOG_DEBUG2("edid_mYcc444Support", edid_mYcc444Support);
			LOG_DEBUG2("edid_mBasicAudioSupport", edid_mBasicAudioSupport);
			LOG_DEBUG2("edid_mUnderscanSupport", edid_mUnderscanSupport);
			if (offset != 4)
			{
				for (i = 4; i < offset; i += edid_ParseDataBlock(baseAddr,
						buffer + i))
					;
			}
			/* last is checksum */
			/* parse DTD's */
			LOG_DEBUG("\n");
			for (i = offset, c = 0; i < (sizeof(edid_mBuffer) - 1) && c < 6; i
					+= DTD_SIZE, c++)
			{
				if (dtd_Parse(&tmpDtd, buffer + i) == TRUE)
				{
					if (edid_mDtdIndex < (sizeof(edid_mDtd) / sizeof(dtd_t)))
					{
						edid_mDtd[edid_mDtdIndex++] = tmpDtd;
					}
					else
					{
						error_Set(ERR_DTD_BUFFER_FULL);
						LOG_WARNING("buffer full - DTD ignored");
					}
				}
			}
		}
	}
	LOG_DEBUGS("__________________________________________________\n\n\n");
	return TRUE;
}

u8 edid_ParseDataBlock(u16 baseAddr, u8 * data)
{
	u8 tag = bitOperation_BitField(data[0], 5, 3);
	u8 length = bitOperation_BitField(data[0], 0, 5);
	u8 c = 0;
	shortAudioDesc_t tmpSad;
	shortVideoDesc_t tmpSvd;
	//LOG_TRACE3("TAG", tag, length);
	LOG_DEBUGS("\n");
	LOG_DEBUG3("TAG", tag, length);
	switch (tag)
	{
	case 0x1: /* Audio Data Block */
		LOG_DEBUG("Audio Data Block");
		for (c = 1; c < (length + 1); c += 3)
		{
			shortAudioDesc_Parse(&tmpSad, data + c);
			if (edid_mSadIndex < (sizeof(edid_mSad) / sizeof(shortAudioDesc_t)))
			{
				edid_mSad[edid_mSadIndex++] = tmpSad;
			}
			else
			{
				error_Set(ERR_SHORT_AUDIO_DESC_BUFFER_FULL);
				LOG_WARNING("buffer full - SAD ignored");
			}
		}
		break;
	case 0x2: /* Video Data Block */
		LOG_DEBUG("Video Data Block");
		for (c = 1; c < (length + 1); c++)
		{
			shortVideoDesc_Parse(&tmpSvd, data[c]);
			if (edid_mSvdIndex < (sizeof(edid_mSvd) / sizeof(shortVideoDesc_t)))
			{
				edid_mSvd[edid_mSvdIndex++] = tmpSvd;
			}
			else
			{
				error_Set(ERR_SHORT_VIDEO_DESC_BUFFER_FULL);
				LOG_WARNING("buffer full - SVD ignored");
			}
		}
		break;
	case 0x3: /* Vendor Specific Data Block */
	{
		u32 ieeeId = bitOperation_Bytes2Dword(0x00, data[3], data[2], data[1]);
		LOG_DEBUG2("Vendor Specific Data Block", ieeeId);
		if (ieeeId == 0x000C03)
		{ /* HDMI */
			if (hdmivsdb_Parse(&edid_mHdmivsdb, data) != TRUE)
			{
				LOG_WARNING("HDMI Vendor Specific Data Block corrupt");
			}
		}
		else
		{
			LOG_WARNING2("Vendor Specific Data Block not parsed", ieeeId);
		}
		break;
	}
	case 0x4: /* Speaker Allocation Data Block */
		LOG_DEBUG("Speaker Allocation Data Block");
		if (speakerAllocationDataBlock_Parse(&edid_mSpeakerAllocationDataBlock,
				data) != TRUE)
		{
			LOG_WARNING("Speaker Allocation Data Block corrupt");
		}
		break;
	case 0x7:
	{
		u8 extendedTag = data[1];
		LOG_DEBUG("ExtenedTag");
		switch (extendedTag)
		{
		case 0x00: /* Video Capability Data Block */
			LOG_DEBUG("Video Capability Data Block");
			if (videoCapabilityDataBlock_Parse(&edid_mVideoCapabilityDataBlock,
					data) != TRUE)
			{
				LOG_WARNING("Video Capability Data Block corrupt");
			}
			break;
		case 0x05: /* Colorimetry Data Block */
			LOG_DEBUG("Colorimetry Data Block");
			if (colorimetryDataBlock_Parse(&edid_mColorimetryDataBlock, data)
					!= TRUE)
			{
				LOG_WARNING("Colorimetry Data Block corrupt");
			}
			break;
		case 0x04: /* HDMI Video Data Block */
		case 0x12: /* HDMI Audio Data Block */
			break;
		default:
			LOG_WARNING2("Extended Data Block not parsed", extendedTag);
			break;
		}
		break;
	}
	default:
		LOG_WARNING2("Data Block not parsed", tag);
		break;
	}
	return length + 1;
}
