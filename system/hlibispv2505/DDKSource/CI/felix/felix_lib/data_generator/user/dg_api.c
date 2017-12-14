/**
*******************************************************************************
@file dg_api.c

@brief Data Generator user-side library

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
#include "dg/dg_api.h"
#include "dg/dg_api_internal.h"

#include "dg_kernel/dg_ioctl.h"

#include <img_defs.h>
#include <img_errors.h>

#define LOG_TAG DG_LOG_TAG
#include <felixcommon/userlog.h>

#define DG_DEV "/dev/imgfelixDG0"

#ifdef WIN32
#ifndef MAP_FAILED
#define MAP_FAILED ((void *)-1)
#endif
#endif

#ifdef FELIX_FAKE

static int DEV_DG_MUnmap(void *addr, size_t length)
{
    (void)addr;
    (void)length;
    return 0;
}

#include "dg_kernel/dg_connection.h"
// fake device
#include "sys/sys_userio_fake.h"

#endif /* FELIX_FAKE */

static IMG_BOOL8 List_destroyCamera(void *listElem, void *param)
{
    struct INT_DG_CAMERA *pCam = (struct INT_DG_CAMERA*)listElem;

    DG_CameraDestroy(&(pCam->publicCamera));

    return IMG_TRUE;
}

/*
 * Driver part
 */

IMG_RESULT DG_DriverInit(DG_CONNECTION **ppConnection)
{
    struct INT_DG_CONNECTION *pConn = NULL;
    IMG_RESULT ret;
    void *fakeOps = NULL;  // used in case of Fake device

    if (!ppConnection || *ppConnection)
    {
        LOG_ERROR("ppConnection is NULL or *ppConnection is not NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pConn = (struct INT_DG_CONNECTION*)IMG_CALLOC(1,
        sizeof(struct INT_DG_CONNECTION));
    if (!pConn)
    {
        LOG_ERROR("failed to allocate internal structure (%" IMG_SIZEPR \
            "u Bytes)\n", sizeof(struct INT_DG_CONNECTION));
        return IMG_ERROR_MALLOC_FAILED;
    }

    ret = List_init(&(pConn->sList_cameras));
    if (ret)  // unlikely
    {
        LOG_ERROR("failed to initialise the internal list of cameras\n");
        IMG_FREE(pConn);
        return IMG_ERROR_MALLOC_FAILED;
    }

#ifdef FELIX_FAKE
    if (g_pDGDriver == NULL)
    {
        // list is empty, no need for cleaning
        IMG_FREE(pConn);
        LOG_ERROR("could not find the kernel-side driver: did you call " \
            "KRN_DG_DriverCreate()?\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    // does not exists in the kernel so add it here
    g_pDGDriver->sDevice.sFileOps.munmap = &DEV_DG_MUnmap;
    fakeOps = (void*)&(g_pDGDriver->sDevice.sFileOps);
#endif

    pConn->fileDesc = SYS_IO_Open(DG_DEV, O_RDWR, fakeOps);
    if (pConn->fileDesc)
    {
        ret = SYS_IO_Control(pConn->fileDesc, DG_IOCTL_INFO,
            (long int)&(pConn->publicConnection.sHWInfo));
        if (ret)
        {
            LOG_ERROR("failed to receive informations from kernel-side\n");
            ret = IMG_ERROR_FATAL;
            goto open_failure;
        }
    }
    else
    {
        LOG_ERROR("failed to connect to the kernel-side driver\n");
        ret = IMG_ERROR_UNEXPECTED_STATE;
        goto open_failure;
    }

    *ppConnection = &(pConn->publicConnection);

    return ret;
open_failure:
    // camera list is empty
    if (pConn->fileDesc)
    {
        SYS_IO_Close(pConn->fileDesc);
    }
    IMG_FREE(pConn);
    pConn = NULL;
    return ret;
}

IMG_RESULT DG_DriverFinalise(DG_CONNECTION *pConnection)
{
    IMG_RESULT ret;
    struct INT_DG_CONNECTION *pConn = NULL;

    if (!pConnection)
    {
        LOG_ERROR("pConnection is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    pConn = container_of(pConnection, struct INT_DG_CONNECTION,
        publicConnection);
    IMG_ASSERT(pConn);

    List_visitor(&(pConn->sList_cameras), NULL, &List_destroyCamera);
    // no need to clean the list more, cells are destroyed

    ret = SYS_IO_Close(pConn->fileDesc);
    if (ret)
    {
        LOG_ERROR("failed to close device (returned %d)\n", ret);
    }

    IMG_FREE(pConn);

    return IMG_SUCCESS;
}
