/*
 * hdmivsdb.c
 *
 *  Created on: Jul 21, 2010
 *      Author: klabadi & dlopo
 */

#include "hdmivsdb.h"
#include "bitOperation.h"
#include "log.h"

void hdmivsdb_Reset(hdmivsdb_t *vsdb)
{
	vsdb->mPhysicalAddress = 0;
	vsdb->mSupportsAi = FALSE;
	vsdb->mDeepColor30 = FALSE;
	vsdb->mDeepColor36 = FALSE;
	vsdb->mDeepColor48 = FALSE;
	vsdb->mDeepColorY444 = FALSE;
	vsdb->mDviDual = FALSE;
	vsdb->mMaxTmdsClk = 0;
	vsdb->mVideoLatency = 0;
	vsdb->mAudioLatency = 0;
	vsdb->mInterlacedVideoLatency = 0;
	vsdb->mInterlacedAudioLatency = 0;
	vsdb->mId = 0;
	vsdb->mLength = 0;
	vsdb->mValid = FALSE;
}
int hdmivsdb_Parse(hdmivsdb_t *vsdb, u8 * data)
{
	u8 blockLength = 0;
	LOG_TRACE();
	hdmivsdb_Reset(vsdb);
	if (data == 0)
	{
		return FALSE;
	}
	if (bitOperation_BitField(data[0], 5, 3) != 0x3)
	{
		LOG_WARNING("Invalid datablock tag");
		return FALSE;
	}
	blockLength = bitOperation_BitField(data[0], 0, 5);
	hdmivsdb_SetLength(vsdb, blockLength);
	if (blockLength < 5)
	{
		LOG_WARNING("Invalid minimum length");
		return FALSE;
	}
	if (bitOperation_Bytes2Dword(0x00, data[3], data[2], data[1]) != 0x000C03)
	{
		LOG_WARNING("HDMI IEEE registration identifier not valid");
		return FALSE;
	}
	hdmivsdb_Reset(vsdb);
	hdmivsdb_SetId(vsdb, 0x000C03);
	vsdb->mPhysicalAddress = bitOperation_Bytes2Word(data[4], data[5]);
	LOG_DEBUG2("mPhysicalAddress", vsdb->mPhysicalAddress);
	/* parse extension fields if they exist */
	if (blockLength > 5)
	{
		vsdb->mSupportsAi = bitOperation_BitField(data[6], 7, 1) == 1;
		vsdb->mDeepColor48 = bitOperation_BitField(data[6], 6, 1) == 1;
		vsdb->mDeepColor36 = bitOperation_BitField(data[6], 5, 1) == 1;
		vsdb->mDeepColor30 = bitOperation_BitField(data[6], 4, 1) == 1;
		vsdb->mDeepColorY444 = bitOperation_BitField(data[6], 3, 1) == 1;
		vsdb->mDviDual = bitOperation_BitField(data[6], 0, 1) == 1;
	}
	else
	{
		vsdb->mSupportsAi = FALSE;
		vsdb->mDeepColor48 = FALSE;
		vsdb->mDeepColor36 = FALSE;
		vsdb->mDeepColor30 = FALSE;
		vsdb->mDeepColorY444 = FALSE;
		vsdb->mDviDual = FALSE;
	}
	LOG_DEBUG2("mSupportsAi", vsdb->mSupportsAi);
	LOG_DEBUG2("mDeepColor48", vsdb->mDeepColor48);
	LOG_DEBUG2("mDeepColor36", vsdb->mDeepColor36);
	LOG_DEBUG2("mDeepColor30", vsdb->mDeepColor30);
	LOG_DEBUG2("mDeepColorY444", vsdb->mDeepColorY444);
	LOG_DEBUG2("mDviDual", vsdb->mDviDual);
	
	vsdb->mMaxTmdsClk = (blockLength > 6) ? data[7] : 0;
	vsdb->mVideoLatency = 0;
	vsdb->mAudioLatency = 0;
	vsdb->mInterlacedVideoLatency = 0;
	vsdb->mInterlacedAudioLatency = 0;
	LOG_DEBUG2("mMaxTmdsClk", vsdb->mMaxTmdsClk);
	if (blockLength > 7)
	{
		if (bitOperation_BitField(data[8], 7, 1) == 1)
		{
			if (blockLength < 10)
			{
				LOG_WARNING("Invalid length - latencies are not valid");
				return FALSE;
			}
			if (bitOperation_BitField(data[8], 6, 1) == 1)
			{
				if (blockLength < 12)
				{
					LOG_WARNING("Invalid length - Interlaced latencies are not valid");
					return FALSE;
				}
				else
				{
					vsdb->mVideoLatency = data[9];
					vsdb->mAudioLatency = data[10];
					vsdb->mInterlacedVideoLatency = data[11];
					vsdb->mInterlacedAudioLatency = data[12];
				}
			}
			else
			{
				vsdb->mVideoLatency = data[9];
				vsdb->mAudioLatency = data[10];
				vsdb->mInterlacedVideoLatency = data[9];
				vsdb->mInterlacedAudioLatency = data[10];
			}
			LOG_DEBUG2("mVideoLatency", vsdb->mVideoLatency);
			LOG_DEBUG2("mAudioLatency", vsdb->mAudioLatency);
			LOG_DEBUG2("mInterlacedVideoLatency", vsdb->mInterlacedVideoLatency);
			LOG_DEBUG2("mInterlacedAudioLatency", vsdb->mInterlacedAudioLatency);			
		}
	}
	vsdb->mValid = TRUE;
	return TRUE;
}
int hdmivsdb_GetDeepColor30(hdmivsdb_t *vsdb)
{
	return vsdb->mDeepColor30;
}

int hdmivsdb_GetDeepColor36(hdmivsdb_t *vsdb)
{
	return vsdb->mDeepColor36;
}

int hdmivsdb_GetDeepColor48(hdmivsdb_t *vsdb)
{
	return vsdb->mDeepColor48;
}

int hdmivsdb_GetDeepColorY444(hdmivsdb_t *vsdb)
{
	return vsdb->mDeepColorY444;
}

int hdmivsdb_GetSupportsAi(hdmivsdb_t *vsdb)
{
	return vsdb->mSupportsAi;
}

int hdmivsdb_GetDviDual(hdmivsdb_t *vsdb)
{
	return vsdb->mDviDual;
}

u8 hdmivsdb_GetMaxTmdsClk(hdmivsdb_t *vsdb)
{
	return vsdb->mMaxTmdsClk;
}

u16 hdmivsdb_GetPVideoLatency(hdmivsdb_t *vsdb)
{
	return vsdb->mVideoLatency;
}

u16 hdmivsdb_GetPAudioLatency(hdmivsdb_t *vsdb)
{
	return vsdb->mAudioLatency;
}

u16 hdmivsdb_GetIAudioLatency(hdmivsdb_t *vsdb)
{
	return vsdb->mInterlacedAudioLatency;
}

u16 hdmivsdb_GetIVideoLatency(hdmivsdb_t *vsdb)
{
	return vsdb->mInterlacedVideoLatency;
}

u16 hdmivsdb_GetPhysicalAddress(hdmivsdb_t *vsdb)
{
	return vsdb->mPhysicalAddress;
}

u32 hdmivsdb_GetId(hdmivsdb_t *vsdb)
{
	return vsdb->mId;
}

u8 hdmivsdb_GetLength(hdmivsdb_t *vsdb)
{
	return vsdb->mLength;
}

void hdmivsdb_SetId(hdmivsdb_t *vsdb, u32 id)
{
	vsdb->mId = id;
}

void hdmivsdb_SetLength(hdmivsdb_t *vsdb, u8 length)
{
	vsdb->mLength = length;
}
