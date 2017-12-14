/*
 * videoCapabilityDataBlock.c
 *
 *  Created on: Jul 23, 2010
 *      Author: klabadi & dlopo
 */
#include "videoCapabilityDataBlock.h"
#include "bitOperation.h"
#include "log.h"

void videoCapabilityDataBlock_Reset(videoCapabilityDataBlock_t * vcdb)
{
	vcdb->mQuantizationRangeSelectableRGB = FALSE;
	vcdb->mQuantizationRangeSelectableYCC = FALSE;
	vcdb->mPreferredTimingScanInfo = 0;
	vcdb->mItScanInfo = 0;
	vcdb->mCeScanInfo = 0;
	vcdb->mValid = FALSE;
}

int videoCapabilityDataBlock_Parse(videoCapabilityDataBlock_t * vcdb, u8 * data)
{
	LOG_TRACE();
	videoCapabilityDataBlock_Reset(vcdb);
	/* check tag code and extended tag */
	if (data != 0 && bitOperation_BitField(data[0], 5, 3) == 0x7
			&& bitOperation_BitField(data[1], 0, 8) == 0x0
			&& bitOperation_BitField(data[0], 0, 5) == 0x2)
	{ /* so far VCDB is 2 bytes long */
		vcdb->mCeScanInfo = bitOperation_BitField(data[2], 0, 2);
		vcdb->mItScanInfo = bitOperation_BitField(data[2], 2, 2);
		vcdb->mPreferredTimingScanInfo = bitOperation_BitField(data[2], 4, 2);
		vcdb->mQuantizationRangeSelectableRGB = (bitOperation_BitField(data[2], 6,
				1) == 1) ? TRUE : FALSE;
		vcdb->mQuantizationRangeSelectableYCC = (bitOperation_BitField(data[2], 7,
				1) == 1) ? TRUE : FALSE;
		vcdb->mValid = TRUE;
		LOG_DEBUG2("mCeScanInfo", vcdb->mCeScanInfo);
		LOG_DEBUG2("mItScanInfo", vcdb->mItScanInfo);
		LOG_DEBUG2("mPreferredTimingScanInfo", vcdb->mPreferredTimingScanInfo);
		LOG_DEBUG2("mQuantizationRangeSelectableRGB", vcdb->mQuantizationRangeSelectableRGB);
		LOG_DEBUG2("mQuantizationRangeSelectableYCC", vcdb->mQuantizationRangeSelectableYCC);
		return TRUE;
	}
	return FALSE;
}

u8 videoCapabilityDataBlock_GetCeScanInfo(
		videoCapabilityDataBlock_t * vcdb)
{
	return vcdb->mCeScanInfo;
}

u8 videoCapabilityDataBlock_GetItScanInfo(
		videoCapabilityDataBlock_t * vcdb)
{
	return vcdb->mItScanInfo;
}

u8 videoCapabilityDataBlock_GetPreferredTimingScanInfo(
		videoCapabilityDataBlock_t * vcdb)
{
	return vcdb->mPreferredTimingScanInfo;
}

int videoCapabilityDataBlock_GetQuantizationRangeSelectableRGB(videoCapabilityDataBlock_t * vcdb)
{
	return vcdb->mQuantizationRangeSelectableRGB;
}

int videoCapabilityDataBlock_GetQuantizationRangeSelectableYCC(videoCapabilityDataBlock_t * vcdb)
{
	return vcdb->mQuantizationRangeSelectableYCC;
}
