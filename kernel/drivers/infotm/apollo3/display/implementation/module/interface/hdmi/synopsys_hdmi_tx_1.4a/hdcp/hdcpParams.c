/*
 * hdcpParams.c
 *
 *  Created on: Jul 21, 2010
 *      Author: klabadi
 */
#include "hdcpParams.h"

void hdcpParams_Reset(hdcpParams_t *params)
{
	params->mEnable11Feature = 0;
	params->mRiCheck = 0;
	params->mI2cFastMode = 0;
	params->mEnhancedLinkVerification = 0;
}

int hdcpParams_GetEnable11Feature(hdcpParams_t *params)
{
	return params->mEnable11Feature;
}

int hdcpParams_GetRiCheck(hdcpParams_t *params)
{
	return params->mRiCheck;
}

int hdcpParams_GetI2cFastMode(hdcpParams_t *params)
{
	return params->mI2cFastMode;
}

int hdcpParams_GetEnhancedLinkVerification(hdcpParams_t *params)
{
	return params->mEnhancedLinkVerification;
}

void hdcpParams_SetEnable11Feature(hdcpParams_t *params, int value)
{
	params->mEnable11Feature = value;
}

void hdcpParams_SetEnhancedLinkVerification(hdcpParams_t *params, int value)
{
	params->mEnhancedLinkVerification = value;
}

void hdcpParams_SetI2cFastMode(hdcpParams_t *params, int value)
{
	params->mI2cFastMode = value;
}

void hdcpParams_SetRiCheck(hdcpParams_t *params, int value)
{
	params->mRiCheck = value;
}
