/*!
******************************************************************************
 @file   :  osa_utils.h

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
#ifndef OSA_UTILS_H
#define OSA_UTILS_H

#if (__cplusplus)
extern "C" {
#endif

#include "img_defs.h"
#include "osa.h"
#include "lst.h"
#include "dq.h"
#include "tre.h"

#define OSAUTILS_POOLLINK OSAUTILS_PoolLink sPoolLink
/*!

******************************************************************************

 @Struct                 OSAUTILS_PoolLink

 @Description

 Identifies the pool which owns the item. 

******************************************************************************/
typedef struct
{
    union
    {
        LST_LINK;
        DQ_LINK;
        TRE_LINK;
    }uPoolLinkages;
    IMG_HANDLE *phOwner;
} OSAUTILS_PoolLink;

/*!
******************************************************************************

 @Function				OSAUTILS_MboxInit

 @Description

 Creates a MailBox object.

 @Output	phMbox		: A pointer used to return the mailbox handle.

 @Return	IMG_RESULT	: This function returns either IMG_SUCCESS or an error code.

******************************************************************************/
IMG_RESULT OSAUTILS_MboxInit(
    IMG_HANDLE *phMbox
);


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
);


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
);


/*!
******************************************************************************

 @Function				OSAUTILS_MboxPut

 @Description

 Puts an item into the mailbox.

 @Input    hMbox          : Handle to mailbox

 @Input    pvItem         : Pointer to the mailbox item

 @Return    None.

******************************************************************************/
IMG_VOID OSAUTILS_MboxPut(
    IMG_HANDLE			hMbox, IMG_VOID *pvItem
);

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
        IMG_HANDLE *phPool,
        IMG_VOID *pvItems,
        IMG_UINT32 ui32ItemCount,
        IMG_UINT32 ui32ItemSize
        );


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
);


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
);


/*!
******************************************************************************

 @Function				OSAUTILS_PoolReturn

 @Description

 Release an item back to the pool.


 @Input     pvItem       : Pointer to the item which is to be returned back.

 @Return    None.

******************************************************************************/
IMG_VOID OSAUTILS_PoolReturn(
        IMG_VOID *pvItem
);

#if (__cplusplus)
}
#endif

#endif /* __OSAL_H__	*/
