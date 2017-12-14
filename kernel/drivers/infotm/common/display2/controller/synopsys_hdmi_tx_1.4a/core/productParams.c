/*
 * @file:productParams.c
 *
 *  Created on: Jul 6, 2010
 *      Author: klabadi & dlopo
 */

#include "productParams.h"
#include "log.h"

void productParams_Reset(productParams_t *params)
{
	unsigned i = 0;
	params->mVendorNameLength = 0;
	params->mProductNameLength = 0;
	params->mSourceType = (u8) (-1);
	params->mOUI = (u8) (-1);
	params->mVendorPayloadLength = 0;
	for (i = 0; i < sizeof(params->mVendorName); i++)
	{
		params->mVendorName[i] = 0;
	}
	for (i = 0; i < sizeof(params->mProductName); i++)
	{
		params->mVendorName[i] = 0;
	}
	for (i = 0; i < sizeof(params->mVendorPayload); i++)
	{
		params->mVendorName[i] = 0;
	}
}

u8 productParams_SetProductName(productParams_t *params, const u8 * data,
		u8 length)
{
	u16 i = 0;
	if ((data == 0) || length > sizeof(params->mProductName))
	{
		LOG_ERROR("invalid parameter");
		return 1;
	}
	params->mProductNameLength = 0;
	for (i = 0; i < sizeof(params->mProductName); i++)
	{
		params->mProductName[i] = (i < length) ? data[i] : 0;
	}
	params->mProductNameLength = length;
	return 0;
}

u8 * productParams_GetProductName(productParams_t *params)
{
	return params->mProductName;
}

u8 productParams_GetProductNameLength(productParams_t *params)
{
	return params->mProductNameLength;
}

u8 productParams_SetVendorName(productParams_t *params, const u8 * data,
		u8 length)
{
	u16 i = 0;
	if (data == 0 || length > sizeof(params->mVendorName))
	{
		LOG_ERROR("invalid parameter");
		return 1;
	}
	params->mVendorNameLength = 0;
	for (i = 0; i < sizeof(params->mVendorName); i++)
	{
		params->mVendorName[i] = (i < length) ? data[i] : 0;
	}
	params->mVendorNameLength = length;
	return 0;
}

u8 * productParams_GetVendorName(productParams_t *params)
{
	return params->mVendorName;
}

u8 productParams_GetVendorNameLength(productParams_t *params)
{
	return params->mVendorNameLength;
}

u8 productParams_SetSourceType(productParams_t *params, u8 value)
{
	params->mSourceType = value;
	return 0;
}

u8 productParams_GetSourceType(productParams_t *params)
{
	return params->mSourceType;
}

u8 productParams_SetOUI(productParams_t *params, u32 value)
{
	params->mOUI = value;
	return 0;
}

u32 productParams_GetOUI(productParams_t *params)
{
	return params->mOUI;
}

u8 productParams_SetVendorPayload(productParams_t *params, const u8 * data,
		u8 length)
{
	u16 i = 0;
	if (data == 0 || length > sizeof(params->mVendorPayload))
	{
		LOG_ERROR("invalid parameter");
		return 1;
	}
	params->mVendorPayloadLength = 0;
	for (i = 0; i < sizeof(params->mVendorPayload); i++)
	{
		params->mVendorPayload[i] = (i < length) ? data[i] : 0;
	}
	params->mVendorPayloadLength = length;
	return 0;
}

u8 * productParams_GetVendorPayload(productParams_t *params)
{
	return params->mVendorPayload;
}

u8 productParams_GetVendorPayloadLength(productParams_t *params)
{
	return params->mVendorPayloadLength;
}

u8 productParams_IsSourceProductValid(productParams_t *params)
{
	return productParams_GetSourceType(params) != (u8) (-1)
			&& productParams_GetVendorNameLength(params) != 0
			&& productParams_GetProductNameLength(params) != 0;
}

u8 productParams_IsVendorSpecificValid(productParams_t *params)
{
	return productParams_GetOUI(params) != (u32) (-1)
			&& productParams_GetVendorPayloadLength(params) != 0;
}
