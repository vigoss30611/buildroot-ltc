/*
 * shortVideoDesc.c
 *
 *  Created on: Jul 22, 2010
 *      Author: klabadi & dlopo
 */

#include "shortVideoDesc.h"
#include "bitOperation.h"
#include "log.h"

void shortVideoDesc_Reset(shortVideoDesc_t *svd)
{
	svd->mNative = FALSE;
	svd->mCode = 0;
}

int shortVideoDesc_Parse(shortVideoDesc_t *svd, u8 data)
{
	shortVideoDesc_Reset(svd);
	svd->mNative = (bitOperation_BitField(data, 7, 1) == 1) ? TRUE : FALSE;
	svd->mCode = bitOperation_BitField(data, 0, 7);
	if (svd->mNative == TRUE)
		LOG_DEBUG2("mNative", svd->mNative);
	LOG_DEBUG2("mCode", svd->mCode);
	return TRUE;
}

unsigned shortVideoDesc_GetCode(shortVideoDesc_t *svd)
{
	return svd->mCode;
}

int shortVideoDesc_GetNative(shortVideoDesc_t *svd)
{
	return svd->mNative;
}
