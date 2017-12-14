/*
 * @file: error.c
 *
 *  Created on: Aug 10, 2010
 *      Author: klabadi & dlopo
 */

#include "error.h"

static errorType_t errorCode = NO_ERROR;

void error_Set(errorType_t err)
{
	if ((err > NO_ERROR) && (err < ERR_UNDEFINED))
	{
		errorCode = err;
	}
}

errorType_t error_Get()
{
	errorType_t tmpErr = errorCode;
	errorCode = NO_ERROR;
	return tmpErr;
}
