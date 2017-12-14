/**
*******************************************************************************
@file ci_gasket.c

@brief Implementation of the Gasket for CI

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h"

#ifdef IMG_KERNEL_MODULE
#error "this should not be in a kernel module"
#endif

#include "ci_internal/ci_errors.h" // toErrno and toImgResult
#include "ci_kernel/ci_ioctrl.h"

#define LOG_TAG "CI_API"
#include <felixcommon/userlog.h>

IMG_RESULT CI_GasketInit(CI_GASKET *pGasket)
{
    if ( !pGasket )
    {
        LOG_ERROR("pGasket is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_MEMSET(pGasket, 0, sizeof(CI_GASKET));

    return IMG_SUCCESS;
}

IMG_RESULT CI_GasketAcquire(CI_GASKET *pGasket, CI_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if ( !pGasket || !pConnection )
    {
        LOG_ERROR("pGasket or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_GASK_ACQ, (long)pGasket); //INT_CI_GasketAcquire()
    if ( ret < 0 )
    {
        LOG_ERROR("Failed to acquire the gasket\n");
        return toImgResult(ret);
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_GasketRelease(CI_GASKET *pGasket, CI_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    INT_CONNECTION *pIntCon = NULL;

    if ( !pGasket || !pConnection )
    {
        LOG_ERROR("pGasket or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_GASK_REL,
        (long)pGasket->uiGasket); //INT_CI_GasketRelease()
    if ( ret < 0 )
    {
        LOG_ERROR("Failed to release the gasket\n");
        return toImgResult(ret);
    }

    return IMG_SUCCESS;
}

IMG_RESULT CI_GasketGetInfo(CI_GASKET_INFO *pGasketInfo, IMG_UINT8 uiGasket,
    CI_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    struct CI_GASKET_PARAM sGasketNfo;
    INT_CONNECTION *pIntCon = NULL;

    if ( !pGasketInfo || !pConnection )
    {
        LOG_ERROR("pGasket or pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pIntCon = container_of(pConnection, INT_CONNECTION, publicConnection);
    IMG_ASSERT(pIntCon);

    sGasketNfo.uiGasket = uiGasket;

    ret = SYS_IO_Control(pIntCon->fileDesc, CI_IOCTL_GASK_NFO,
        (long)&sGasketNfo); //INT_CI_GasketGetInfo()
    if ( ret < 0 )
    {
        LOG_ERROR("Failed to get gasket's information\n");
        return toImgResult(ret);
    }

    IMG_MEMCPY(pGasketInfo, &(sGasketNfo.sGasketInfo), sizeof(CI_GASKET_INFO));

    return IMG_SUCCESS;
}
