/*
 * @file:videoParams.c
 *
 *  Created on: Jul 2, 2010
 *      Author: klabadi & dlopo
 */
#include "videoParams.h"

void videoParams_Reset(videoParams_t *params)
{
	params->mHdmi = 0;
	params->mEncodingOut = RGB;
	params->mEncodingIn = RGB;
	params->mColorResolution = 8;
	params->mPixelRepetitionFactor = 0;
	params->mRgbQuantizationRange = 0;
	params->mPixelPackingDefaultPhase = 0;
	params->mColorimetry = 0;
	params->mScanInfo = 0;
	params->mActiveFormatAspectRatio = 8;
	params->mNonUniformScaling = 0;
	params->mExtColorimetry = ~0;
	params->mItContent = 0;
	params->mEndTopBar = ~0;
	params->mStartBottomBar = ~0;
	params->mEndLeftBar = ~0;
	params->mStartRightBar = ~0;
	params->mCscFilter = 0;
	params->mHdmiVideoFormat = 0;
	params->m3dStructure = 0;
	params->m3dExtData = 0;
	params->mHdmiVic = 0;
}

u8 videoParams_GetActiveFormatAspectRatio(videoParams_t *params)
{
	return params->mActiveFormatAspectRatio;
}

u8 videoParams_GetColorimetry(videoParams_t *params)
{
	return params->mColorimetry;
}

u8 videoParams_GetColorResolution(videoParams_t *params)
{
	return params->mColorResolution;
}

u8 videoParams_GetCscFilter(videoParams_t *params)
{
	return params->mCscFilter;
}

dtd_t* videoParams_GetDtd(videoParams_t *params)
{
	return &(params->mDtd);
}

encoding_t videoParams_GetEncodingIn(videoParams_t *params)
{
	return params->mEncodingIn;
}

encoding_t videoParams_GetEncodingOut(videoParams_t *params)
{
	return params->mEncodingOut;
}

u16 videoParams_GetEndLeftBar(videoParams_t *params)
{
	return params->mEndLeftBar;
}

u16 videoParams_GetEndTopBar(videoParams_t *params)
{
	return params->mEndTopBar;
}

u8 videoParams_GetExtColorimetry(videoParams_t *params)
{
	return params->mExtColorimetry;
}

u8 videoParams_GetHdmi(videoParams_t *params)
{
	return params->mHdmi;
}

u8 videoParams_GetItContent(videoParams_t *params)
{
	return params->mItContent;
}

u8 videoParams_GetNonUniformScaling(videoParams_t *params)
{
	return params->mNonUniformScaling;
}

u8 videoParams_GetPixelPackingDefaultPhase(videoParams_t *params)
{
	return params->mPixelPackingDefaultPhase;
}

u8 videoParams_GetPixelRepetitionFactor(videoParams_t *params)
{
	return params->mPixelRepetitionFactor;
}

u8 videoParams_GetRgbQuantizationRange(videoParams_t *params)
{
	return params->mRgbQuantizationRange;
}

u8 videoParams_GetScanInfo(videoParams_t *params)
{
	return params->mScanInfo;
}

u16 videoParams_GetStartBottomBar(videoParams_t *params)
{
	return params->mStartBottomBar;
}

u16 videoParams_GetStartRightBar(videoParams_t *params)
{
	return params->mStartRightBar;
}

u16 videoParams_GetCscScale(videoParams_t *params)
{
	return params->mCscScale;
}

void videoParams_SetActiveFormatAspectRatio(videoParams_t *params, u8 value)
{
	params->mActiveFormatAspectRatio = value;
}

void videoParams_SetColorimetry(videoParams_t *params, u8 value)
{
	params->mColorimetry = value;
}

void videoParams_SetColorResolution(videoParams_t *params, u8 value)
{
	params->mColorResolution = value;
}

void videoParams_SetCscFilter(videoParams_t *params, u8 value)
{
	params->mCscFilter = value;
}

void videoParams_SetDtd(videoParams_t *params, dtd_t *dtd)
{
	params->mDtd = *dtd;
}

void videoParams_SetEncodingIn(videoParams_t *params, encoding_t value)
{
	params->mEncodingIn = value;
}

void videoParams_SetEncodingOut(videoParams_t *params, encoding_t value)
{
	params->mEncodingOut = value;
}

void videoParams_SetEndLeftBar(videoParams_t *params, u16 value)
{
	params->mEndLeftBar = value;
}

void videoParams_SetEndTopBar(videoParams_t *params, u16 value)
{
	params->mEndTopBar = value;
}

void videoParams_SetExtColorimetry(videoParams_t *params, u8 value)
{
	params->mExtColorimetry = value;
}

void videoParams_SetHdmi(videoParams_t *params, u8 value)
{
	params->mHdmi = value;
}

void videoParams_SetItContent(videoParams_t *params, u8 value)
{
	params->mItContent = value;
}

void videoParams_SetNonUniformScaling(videoParams_t *params, u8 value)
{
	params->mNonUniformScaling = value;
}

void videoParams_SetPixelPackingDefaultPhase(videoParams_t *params, u8 value)
{
	params->mPixelPackingDefaultPhase = value;
}

void videoParams_SetPixelRepetitionFactor(videoParams_t *params, u8 value)
{
	params->mPixelRepetitionFactor = value;
}

void videoParams_SetRgbQuantizationRange(videoParams_t *params, u8 value)
{
	params->mRgbQuantizationRange = value;
}

void videoParams_SetScanInfo(videoParams_t *params, u8 value)
{
	params->mScanInfo = value;
}

void videoParams_SetStartBottomBar(videoParams_t *params, u16 value)
{
	params->mStartBottomBar = value;
}

void videoParams_SetStartRightBar(videoParams_t *params, u16 value)
{
	params->mStartRightBar = value;
}

u16 * videoParams_GetCscA(videoParams_t *params)
{
	videoParams_UpdateCscCoefficients(params);
	return params->mCscA;
}

void videoParams_SetCscA(videoParams_t *params, u16 value[4])
{
	u16 i = 0;
	for (i = 0; i < sizeof(params->mCscA) / sizeof(params->mCscA[0]); i++)
	{
		params->mCscA[i] = value[i];
	}
}

u16 * videoParams_GetCscB(videoParams_t *params)
{
	videoParams_UpdateCscCoefficients(params);
	return params->mCscB;
}

void videoParams_SetCscB(videoParams_t *params, u16 value[4])
{
	u16 i = 0;
	for (i = 0; i < sizeof(params->mCscB) / sizeof(params->mCscB[0]); i++)
	{
		params->mCscB[i] = value[i];
	}
}

u16 * videoParams_GetCscC(videoParams_t *params)
{
	videoParams_UpdateCscCoefficients(params);
	return params->mCscC;
}

void videoParams_SetCscC(videoParams_t *params, u16 value[4])
{
	u16 i = 0;
	for (i = 0; i < sizeof(params->mCscC) / sizeof(params->mCscC[0]); i++)
	{
		params->mCscC[i] = value[i];
	}
}

void videoParams_SetCscScale(videoParams_t *params, u16 value)
{
	params->mCscScale = value;
}

/* [0.01 MHz] */
u16 videoParams_GetPixelClock(videoParams_t *params)
{
	return dtd_GetPixelClock(&(params->mDtd));
}

/* [0.01 MHz] */
u16 videoParams_GetTmdsClock(videoParams_t *params)
{
	return (u16) ((u32) (dtd_GetPixelClock(&(params->mDtd))
			* (u32) (videoParams_GetRatioClock(params))) / 100);
}

/* 0.01 */
unsigned videoParams_GetRatioClock(videoParams_t *params)
{
	unsigned ratio = 100;

	if (params->mEncodingOut != YCC422)
	{
		if (params->mColorResolution == 8)
		{
			ratio = 100;
		}
		else if (params->mColorResolution == 10)
		{
			ratio = 125;
		}
		else if (params->mColorResolution == 12)
		{
			ratio = 150;
		}
		else if (params->mColorResolution == 16)
		{
			ratio = 200;
		}
	}
	return ratio * (params->mPixelRepetitionFactor + 1);
}

int videoParams_IsColorSpaceConversion(videoParams_t *params)
{
	return params->mEncodingIn != params->mEncodingOut;
}

int videoParams_IsColorSpaceDecimation(videoParams_t *params)
{
	return params->mEncodingOut == YCC422 && (params->mEncodingIn == RGB
			|| params->mEncodingIn == YCC444);
}

int videoParams_IsColorSpaceInterpolation(videoParams_t *params)
{
	return params->mEncodingIn == YCC422 && (params->mEncodingOut == RGB
			|| params->mEncodingOut == YCC444);
}

int videoParams_IsPixelRepetition(videoParams_t *params)
{
	return params->mPixelRepetitionFactor > 0 || dtd_GetPixelRepetitionInput(
			&(params->mDtd)) > 0;
}
u8 videoParams_GetHdmiVideoFormat(videoParams_t *params)
{
	return params->mHdmiVideoFormat;
}

void videoParams_SetHdmiVideoFormat(videoParams_t *params, u8 value)
{
	params->mHdmiVideoFormat = value;
}

u8 videoParams_Get3dStructure(videoParams_t *params)
{
	return params->m3dStructure;
}

void videoParams_Set3dStructure(videoParams_t *params, u8 value)
{
	params->m3dStructure = value;
}

u8 videoParams_Get3dExtData(videoParams_t *params)
{
	return params->m3dExtData;
}

void videoParams_Set3dExtData(videoParams_t *params, u8 value)
{
	params->m3dExtData = value;
}

u8 videoParams_GetHdmiVic(videoParams_t *params)
{
	return params->mHdmiVic;
}

void videoParams_SetHdmiVic(videoParams_t *params, u8 value)
{
	params->mHdmiVic = value;
}

void videoParams_UpdateCscCoefficients(videoParams_t *params)
{
	u16 i = 0;
	if (!videoParams_IsColorSpaceConversion(params))
	{
		for (i = 0; i < 4; i++)
		{
			params->mCscA[i] = 0;
			params->mCscB[i] = 0;
			params->mCscC[i] = 0;
		}
		params->mCscA[0] = 0x2000;
		params->mCscB[1] = 0x2000;
		params->mCscC[2] = 0x2000;
		params->mCscScale = 1;
	}
	else if (videoParams_IsColorSpaceConversion(params) && params->mEncodingOut
			== RGB)
	{
		if (params->mColorimetry == ITU601)
		{
			params->mCscA[0] = 0x2000;
			params->mCscA[1] = 0x6926;
			params->mCscA[2] = 0x74fd;
			params->mCscA[3] = 0x010e;

			params->mCscB[0] = 0x2000;
			params->mCscB[1] = 0x2cdd;
			params->mCscB[2] = 0x0000;
			params->mCscB[3] = 0x7e9a;

			params->mCscC[0] = 0x2000;
			params->mCscC[1] = 0x0000;
			params->mCscC[2] = 0x38b4;
			params->mCscC[3] = 0x7e3b;

			params->mCscScale = 1;
		}
		else if (params->mColorimetry == ITU709)
		{
			params->mCscA[0] = 0x2000;
			params->mCscA[1] = 0x7106;
			params->mCscA[2] = 0x7a02;
			params->mCscA[3] = 0x00a7;

			params->mCscB[0] = 0x2000;
			params->mCscB[1] = 0x3264;
			params->mCscB[2] = 0x0000;
			params->mCscB[3] = 0x7e6d;

			params->mCscC[0] = 0x2000;
			params->mCscC[1] = 0x0000;
			params->mCscC[2] = 0x3b61;
			params->mCscC[3] = 0x7e25;

			params->mCscScale = 1;
		}
	}
	else if (videoParams_IsColorSpaceConversion(params) && params->mEncodingIn
			== RGB)
	{
		if (params->mColorimetry == ITU601)
		{
			params->mCscA[0] = 0x2591;
			params->mCscA[1] = 0x1322;
			params->mCscA[2] = 0x074b;
			params->mCscA[3] = 0x0000;

			params->mCscB[0] = 0x6535;
			params->mCscB[1] = 0x2000;
			params->mCscB[2] = 0x7acc;
			params->mCscB[3] = 0x0200;

			params->mCscC[0] = 0x6acd;
			params->mCscC[1] = 0x7534;
			params->mCscC[2] = 0x2000;
			params->mCscC[3] = 0x0200;

			params->mCscScale = 0;
		}
		else if (params->mColorimetry == ITU709)
		{
			params->mCscA[0] = 0x2dc5;
			params->mCscA[1] = 0x0d9b;
			params->mCscA[2] = 0x049e;
			params->mCscA[3] = 0x0000;

			params->mCscB[0] = 0x62f0;
			params->mCscB[1] = 0x2000;
			params->mCscB[2] = 0x7d11;
			params->mCscB[3] = 0x0200;

			params->mCscC[0] = 0x6756;
			params->mCscC[1] = 0x78ab;
			params->mCscC[2] = 0x2000;
			params->mCscC[3] = 0x0200;

			params->mCscScale = 0;
		}
	}
	/* else use user coefficients */
}

