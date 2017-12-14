/*!
******************************************************************************
 @file   :  osa_utils.c

 @brief	OS Abstraction Layer Utilities(OSA)

 @Author Imagination Technologies

 @date   30/10/2012

         <b>Copyright 2012 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 <b>Description:</b>\n
         Additional Functionality based on OS Adaption Layer(OSA)

******************************************************************************/
#include "img_errors.h"
#include "osa_utils.h"


/*!
******************************************************************************

 @Struct                 sOSAUtilsMailbox

 @Description

 Combines thread handle and its function and params.

******************************************************************************/
typedef struct
{
    IMG_HANDLE  hEvent;
    IMG_HANDLE  hMutex;
    LST_T       sItemList;
} sOSAUtilsMailbox;

/*!
******************************************************************************

 @Struct                 sOSAUtilsPool

 @Description

 Combines thread handle and its function and params.

******************************************************************************/
typedef struct
{
    IMG_HANDLE  hEvent;
    IMG_HANDLE  hMutex;
    LST_T       sFreeList;
} sOSAUtilsPool;



/*!
******************************************************************************

 @Function				OSAUTILS_MboxInit

 @Description

 Creates a MailBox object.

 @Output	phMbox			: A pointer used to return the mailbox handle.

 @Return	IMG_RESULT		: This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSAUTILS_MboxInit(
    IMG_HANDLE *phMbox
)
{
    IMG_RESULT eResult = IMG_SUCCESS;
    sOSAUtilsMailbox *psMbox = IMG_NULL;
    psMbox = (sOSAUtilsMailbox *)IMG_MALLOC(sizeof(sOSAUtilsMailbox));
    IMG_ASSERT(psMbox != NULL);
    if(psMbox == NULL)
    {
        eResult = IMG_ERROR_MALLOC_FAILED;
        goto ERROR_BAIL;
    }

    eResult = OSA_ThreadSyncCreate(&psMbox->hEvent);
    if(eResult != IMG_SUCCESS)
    {
        IMG_FREE(psMbox);
        goto ERROR_BAIL;

    }
    eResult = OSA_CritSectCreate(&psMbox->hMutex);
    if(eResult != IMG_SUCCESS)
    {
        OSA_ThreadSyncDestroy(psMbox->hEvent);
        IMG_FREE(psMbox);
        goto ERROR_BAIL;

    }

    LST_init(&psMbox->sItemList);
    *phMbox = psMbox;
ERROR_BAIL:
    return eResult;

}

/*!
******************************************************************************

 @Function				OSAUTILS_MboxDeinit

 @Description

 Destroys a mailbox object.

 NOTE: The state of any thread waiting on the mailbox when it is destroyed is
 undefined.

 @Input     hMbox      : Handle to mailbox

 @Return    None.

******************************************************************************/
IMG_VOID OSAUTILS_MboxDeinit(
    IMG_HANDLE			hMbox
)
{
     sOSAUtilsMailbox *psMbox = (sOSAUtilsMailbox *)hMbox;

     OSA_ThreadSyncDestroy(psMbox->hEvent);
     OSA_CritSectDestroy(psMbox->hMutex);

     IMG_FREE(psMbox);
     hMbox= psMbox= IMG_NULL;
}

/*!
******************************************************************************

 @Function				OSAUTILS_MboxGet

 @Description

 If the mbox in not empty, retrieves one item. In case of empty mailbox, waits
 for a specified duration.

 @Input     hMbox      : Handle to mailbox

 @Input     ui32Timeout : The timeout period in ms. OSA_NO_TIMEOUT to wait for indefinite time.

 @Output    IMG_VOID*   : Pointer to the retrieved item

 @Return    None.

******************************************************************************/
IMG_VOID* OSAUTILS_MboxGet(
    IMG_HANDLE			hMbox, IMG_UINT32 ui32Timeout
)
{
    IMG_VOID *pvItem = NULL;
    sOSAUtilsMailbox *psMbox = (sOSAUtilsMailbox *)hMbox;
    if(!OSA_ThreadSyncWait(psMbox->hEvent, ui32Timeout))
    {
        OSA_CritSectLock(psMbox->hMutex);
        pvItem = LST_removeHead(&psMbox->sItemList);
        OSA_CritSectUnlock(psMbox->hMutex);
    }

    return pvItem;

}

/*!
******************************************************************************

 @Function				OSAUTILS_MboxPut

 @Description

 Puts an item into the mailbox.

 @Input    hMbox          : Handle to mailbox

 @Input    pvItem          : Pointer to the mailbox item

 @Return    None.

******************************************************************************/
IMG_VOID OSAUTILS_MboxPut(
    IMG_HANDLE			hMbox, IMG_VOID *Item
)
{
    sOSAUtilsMailbox *psMbox = (sOSAUtilsMailbox *)hMbox;
    OSA_CritSectLock(psMbox->hMutex);
    LST_add(&psMbox->sItemList, Item);
    OSA_ThreadSyncSignal(psMbox->hEvent);
    OSA_CritSectUnlock(psMbox->hMutex);
}

/*!
*******************************************************************************

 @Function				OSAUTILS_PoolInit

 @Description

 Creates a Pool object.

 @Input		pvItems         : Ponter to the pool contents

 @Input     ui32ItemCount   : Number of items in the pool

 @Input     ui32ItemSize    : Size of each item in the pool

 @Output	phPool			: A pointer used to return the pool handle.

 @Return	IMG_RESULT		: This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSAUTILS_PoolInit(
        IMG_HANDLE* phPool,
        IMG_VOID *pvItems,
        IMG_UINT32 ui32ItemCount,
        IMG_UINT32 ui32ItemSize
        )
{
    IMG_RESULT eResult = IMG_SUCCESS;
    IMG_CHAR *pcItem = pvItems;
    sOSAUtilsPool *psPool = IMG_NULL;
    psPool = (sOSAUtilsPool *)IMG_MALLOC(sizeof(sOSAUtilsPool));
    IMG_ASSERT(psPool != NULL);
    if(psPool == NULL)
    {
        eResult = IMG_ERROR_MALLOC_FAILED;
        goto ERROR_BAIL;
    }

    eResult = OSA_ThreadSyncCreate(&psPool->hEvent);
    if(eResult != IMG_SUCCESS)
    {
        IMG_FREE(psPool);
        goto ERROR_BAIL;

    }
    eResult = OSA_CritSectCreate(&psPool->hMutex);
    if(eResult != IMG_SUCCESS)
    {
        OSA_ThreadSyncDestroy(psPool->hEvent);
        IMG_FREE(psPool);
        goto ERROR_BAIL;

    }

    LST_init(&psPool->sFreeList);
    *phPool = psPool;
    while (ui32ItemCount--)
    {
        ((OSAUTILS_PoolLink *)pcItem)->phOwner = phPool;
        LST_add(&psPool->sFreeList, pcItem);
        OSA_ThreadSyncSignal(psPool->hEvent);
        pcItem += ui32ItemSize;
    }

ERROR_BAIL:
    return eResult;

}
/*!
******************************************************************************

 @Function				OSAUTILS_PoolDeinit

 @Description

 Destroys a pool object.

 NOTE: The state of any thread waiting on the pool when it is destroyed is
 undefined.

 @Input     hPool      : Handle to pool

 @Return    None.

******************************************************************************/
IMG_VOID OSAUTILS_PoolDeinit(
    IMG_HANDLE			hPool
)
{
    sOSAUtilsPool *psPool = (sOSAUtilsPool *)hPool;

    OSA_ThreadSyncDestroy(psPool->hEvent);
    OSA_CritSectDestroy(psPool->hMutex);

    IMG_FREE(psPool);
    hPool = psPool = IMG_NULL;
}

/*!
******************************************************************************

 @Function				OSAUTILS_PoolTake

 @Description

 Get an item from the pool

 @Input     hPool      : Handle to pool

 @Input     ui32Timeout : The timeout period in ms. OSA_NO_TIMEOUT to wait for indefinite time.

 @Output    IMG_VOID*   : Pointer to the retrieved item

 @Return    None.

******************************************************************************/
IMG_VOID* OSAUTILS_PoolTake(
    IMG_HANDLE			hPool, IMG_UINT32 ui32Timeout
)
{
    IMG_VOID *pvItem = NULL;
    sOSAUtilsPool *psPool = (sOSAUtilsPool *)hPool;
    if(!OSA_ThreadSyncWait(psPool->hEvent, ui32Timeout))
    {
        OSA_CritSectLock(psPool->hMutex);
        pvItem = LST_removeHead(&psPool->sFreeList);
        OSA_CritSectUnlock(psPool->hMutex);
    }
    return pvItem;
}

/*!
******************************************************************************

 @Function				OSAUTILS_PoolReturn

 @Description

 Release an item back to the pool.


 @Input    pvItem       : Item to be returned to the pool

 @Return    None.

******************************************************************************/
IMG_VOID OSAUTILS_PoolReturn(
        IMG_VOID *pvItem
)
{
    sOSAUtilsPool *psPool = *(((OSAUTILS_PoolLink *)pvItem)->phOwner);
    if(psPool != IMG_NULL)
    {
        OSA_CritSectLock(psPool->hMutex);
        LST_add(&psPool->sFreeList, (OSAUTILS_PoolLink *)pvItem);
        OSA_ThreadSyncSignal(psPool->hEvent);
        OSA_CritSectUnlock(psPool->hMutex);
    }
    else
        IMG_ASSERT(0);
}
