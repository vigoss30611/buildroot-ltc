/*!
******************************************************************************
 @file   : rman_api.c

 @brief

 @Author Imagination Technologies

 @date   04/03/2008

         <b>Copyright 2008 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 \n<b>Description:</b>\n
         This file contains the Resource Manager API.

 \n<b>Platform:</b>\n
         Platform Independent

******************************************************************************/
/*
******************************************************************************
*****************************************************************************/

#include "osa_rman.h"
#include "osa.h"
#include "dq.h"
#include "osa_idgen.h"


/*!
******************************************************************************
 The follwing macros are used to build/decompose the composite resource Id
 made up from the bucket index + 1 and the allocated resource Id.
******************************************************************************/
#define RMAN_CRESID_BUCKET_INDEX_BITS	(8)										//!< Bits in comp res Id allocated to the bucket index
#define RMAN_CRESID_RES_ID_BITS			(32-RMAN_CRESID_BUCKET_INDEX_BITS)		//!< Bits in comp res Id allocated to the resource Id
#define RMAN_CRESID_MAX_RES_ID			((1<<RMAN_CRESID_RES_ID_BITS)-1)		//!< Max valid resource Id
#define RMAN_CRESID_RES_ID_MASK			(RMAN_CRESID_MAX_RES_ID)				//!< Resource Id mask
#define RMAN_CRESID_BUCKET_SHIFT		(RMAN_CRESID_RES_ID_BITS)				//!< Bits to shift bucket index
#define RMAN_CRESID_MAX_BUCKET_INDEX	((1<<RMAN_CRESID_BUCKET_INDEX_BITS)-1)	//!< Max valid bucket index

#define RMAN_MAX_ID 		4096		// max number of ids per bucket
#define RMAN_ID_BLOCKSIZE	256			// size of hash table for ids

/*!
******************************************************************************
 This structure contains the resource details for a resource registered with
 the resource manager.
******************************************************************************/
typedef struct RMAN_tag_sResource OSARMAN_sResource;

/*!
******************************************************************************
 This structure contains the bucket information.
******************************************************************************/
typedef struct
{
    LST_LINK;			/*!< List link (allows the structure to be part of a MeOS list).*/
    DQ_T				sResList;				//!< List of active resource structures
    IMG_UINT32			ui32BucketIndex;		//!< Bucket index
    IMG_HANDLE			hIdGenerator;			//!< Pointer to the resource from which resource ids are to be allocated
    IMG_UINT32			ui32ResCnt;				//!< Count of resources in the bucket

} OSARMAN_sBucket;

/*!
******************************************************************************
 This structure contains the resource details for a resource registered with
 the resource manager.
******************************************************************************/
struct RMAN_tag_sResource
{
    DQ_LINK;			/*!< DQ link (allows the structure to be part of a MeOS DQ list).*/
    OSARMAN_sBucket *	psBucket;				//!< Pointer to the bucket
    IMG_UINT32			ui32TypeId;				//!< "Type" id
    OSARMAN_pfnFree		pfnFree;				//!< Pointer to free callback function
    IMG_VOID *          pvParam;				//!< IMG_VOID * parameter
    IMG_UINT32			ui32ResId;				//!< Resource Id.
    IMG_HANDLE			hMutexHandle;			//!< Mutex handle, IMG_NULL of no mutex allocated
    IMG_CHAR *			pszResName;				//!< Resource name, IMG_NULL if no name
    OSARMAN_sResource *	psSharedResource;		//!< Pointer to shared resource - only valid for shared resources
    IMG_UINT32			ui32ReferenceCnt;		//!< Reference count - only used with the shared resource

};

static IMG_BOOL	gInitialised = IMG_FALSE;			//!< Indicates where the API has been initialised

static OSARMAN_sBucket *	gapsBucket[RMAN_CRESID_MAX_BUCKET_INDEX];	//!< Array of pointers to buckets

static OSARMAN_sBucket *	gpsGlobalResBucket = IMG_NULL;	//!< Global resource bucket

static OSARMAN_sBucket *	gpsSharedResBucket = IMG_NULL;		//!< Shared resource bucket
static IMG_HANDLE		ghSharedResMutexHandle = IMG_NULL;	//!< Shared resource mutex

static IMG_HANDLE       ghGlobalMutexHandle = IMG_NULL;         //!< Global Mutex

/*!
******************************************************************************

 @Function				OSARMAN_Initialise

******************************************************************************/
IMG_RESULT OSARMAN_Initialise(IMG_VOID)
{
    IMG_UINT32			ui32Result;

    /* If not initialised...*/
    if (!gInitialised)
    {
        /* Initialise the active buckets list...*/
        IMG_MEMSET(&gapsBucket[0], 0, sizeof(gapsBucket));
		
		/* Create mutex...*/
        ui32Result = OSA_CritSectCreate(&ghGlobalMutexHandle);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return ui32Result;
        }

        /* Create mutex...*/
        ui32Result = OSA_CritSectCreate(&ghSharedResMutexHandle);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return ui32Result;
        }

        /* Set initialised flag...*/
        gInitialised = IMG_TRUE;

        /* Create the global resource bucket...*/
        ui32Result = OSARMAN_CreateBucket((IMG_HANDLE *)&gpsGlobalResBucket);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return ui32Result;
        }

        /* Create the shared resource bucket...*/
        ui32Result = OSARMAN_CreateBucket((IMG_HANDLE *)&gpsSharedResBucket);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return ui32Result;
        }
    }

    /* Return success...*/
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				OSARMAN_Deinitialise

******************************************************************************/
IMG_VOID OSARMAN_Deinitialise(IMG_VOID)
{
    IMG_UINT32				i;

    /* Make sure no other cpu is using the shared resources.  */
    OSA_CritSectLock(ghGlobalMutexHandle);

    /* If initialised...*/
    if (gInitialised)
    {
        /* Destroy the golbal resource bucket...*/
        OSARMAN_DestroyBucket(gpsGlobalResBucket);

        /* Destroy the shared resource bucket...*/
        OSARMAN_DestroyBucket(gpsSharedResBucket);

        /* Destroy mutex...*/
        OSA_CritSectDestroy(ghSharedResMutexHandle);

        /* Check all buckets destroyed...*/
        for (i=0; i<RMAN_CRESID_MAX_BUCKET_INDEX; i++)
        {
            IMG_ASSERT(gapsBucket[i] == IMG_NULL);
        }

        /* Reset initialised flag...*/
        gInitialised = IMG_FALSE;
    }

    OSA_CritSectUnlock(ghGlobalMutexHandle);
	OSA_CritSectDestroy(ghGlobalMutexHandle);
}


/*!
******************************************************************************

 @Function				OSARMAN_CreateBucket

******************************************************************************/
IMG_RESULT OSARMAN_CreateBucket(
    IMG_HANDLE *		phResBHandle
)
{
    OSARMAN_sBucket *		psBucket;
    IMG_UINT32				i;
    IMG_RESULT              i32Result;

    IMG_ASSERT(gInitialised);

    /* Allocate a bucket structure...*/
    psBucket = IMG_MALLOC(sizeof(*psBucket));
    IMG_ASSERT(psBucket != IMG_NULL);
    if (psBucket == IMG_NULL)
    {
        return IMG_ERROR_OUT_OF_MEMORY;
    }
    IMG_MEMSET(psBucket, 0, sizeof(*psBucket));

    /* Intialise the resource list...*/
    DQ_init(&psBucket->sResList);

    /* The start allocating resource ids at the first...*/
    i32Result = OSAIDGEN_CreateContext(RMAN_MAX_ID, RMAN_ID_BLOCKSIZE, IMG_TRUE, &psBucket->hIdGenerator);
    if(i32Result != IMG_SUCCESS)
    {
        IMG_FREE(psBucket);
        IMG_ASSERT(!"failed to create IDGEN context");
        return i32Result;
    }

    /* Locate free bucket index within the table...*/
    OSA_CritSectLock(ghGlobalMutexHandle);
    for (i=0; i<RMAN_CRESID_MAX_BUCKET_INDEX; i++)
    {
        if (gapsBucket[i] == IMG_NULL)
        {
            break;
        }
    }
    if (i >= RMAN_CRESID_MAX_BUCKET_INDEX) {
        OSA_CritSectLock(ghGlobalMutexHandle);
        IMG_FREE(psBucket);
        IMG_ASSERT(!"No free buckets left");
        return IMG_ERROR_GENERIC_FAILURE;
    }

    /* Allocate bucket index...*/
    psBucket->ui32BucketIndex = i;
    gapsBucket[i] = psBucket;

    OSA_CritSectUnlock(ghGlobalMutexHandle);

    /* Return the bucket handle...*/
    *phResBHandle = psBucket;

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				OSARMAN_DestroyBucket

******************************************************************************/
IMG_VOID OSARMAN_DestroyBucket(
    IMG_HANDLE				hResBHandle
)
{
    OSARMAN_sBucket *		psBucket = (OSARMAN_sBucket *)hResBHandle;

    IMG_ASSERT(gInitialised);

    IMG_ASSERT(psBucket != IMG_NULL);
    if (psBucket== IMG_NULL)
    {
        return;
    }

    IMG_ASSERT(psBucket->ui32BucketIndex < RMAN_CRESID_MAX_BUCKET_INDEX);
    IMG_ASSERT(gapsBucket[psBucket->ui32BucketIndex] != IMG_NULL);

    /* Free all resources from the bucket...*/
    OSARMAN_FreeResources(hResBHandle, OSARMAN_TYPE_P1);
    OSARMAN_FreeResources(hResBHandle, OSARMAN_TYPE_P2);
    OSARMAN_FreeResources(hResBHandle, OSARMAN_TYPE_P3);
    OSARMAN_FreeResources(hResBHandle, OSARMAN_ALL_TYPES);

    /* free sticky resources last: other resources are dependent on them */
    OSARMAN_FreeResources(hResBHandle, OSARMAN_STICKY);
    /* Use proper locking around global buckets.  */
    OSA_CritSectLock(ghGlobalMutexHandle);

    /* Free from array of bucket pointers...*/
    gapsBucket[psBucket->ui32BucketIndex] = IMG_NULL;

    OSA_CritSectUnlock(ghGlobalMutexHandle);

    /* Free the bucket itself...*/
    OSAIDGEN_DestroyContext(psBucket->hIdGenerator);
    IMG_FREE(psBucket);
}


/*!
******************************************************************************

 @Function				OSARMAN_GetGlobalBucket

******************************************************************************/
IMG_HANDLE OSARMAN_GetGlobalBucket(IMG_VOID)
{
    IMG_ASSERT(gInitialised);
    IMG_ASSERT(gpsGlobalResBucket != IMG_NULL);

    /* Return the handle of the global resource bucket...*/
    return gpsGlobalResBucket;
}

/*!
******************************************************************************

 @Function				OSARMAN_RegisterResource

******************************************************************************/
IMG_RESULT OSARMAN_RegisterResource(
    IMG_HANDLE				hResBHandle,
    IMG_UINT32				ui32TypeId,
    OSARMAN_pfnFree			pfnFree,
    IMG_VOID *              pvParam,
    IMG_HANDLE *			phResHandle,
    IMG_UINT32 *			pui32ResId
)
{
    OSARMAN_sBucket *			psBucket = (OSARMAN_sBucket *) hResBHandle;
    OSARMAN_sResource *			psResource;
    IMG_RESULT                  i32Result;

    IMG_ASSERT(gInitialised);
    IMG_ASSERT(ui32TypeId		!= OSARMAN_ALL_TYPES);

    IMG_ASSERT(hResBHandle != IMG_NULL);
    if (hResBHandle == IMG_NULL)
    {
        return IMG_ERROR_GENERIC_FAILURE;
    }

    /* Allocate a resource structure...*/
    psResource = IMG_MALLOC(sizeof(*psResource));
    IMG_ASSERT(psResource != IMG_NULL);
    if (psResource == IMG_NULL)
    {
        return IMG_ERROR_OUT_OF_MEMORY;
    }
    IMG_MEMSET(psResource, 0, sizeof(*psResource));

    /* Fill in the resource structure...*/
    psResource->psBucket		= psBucket;
    psResource->ui32TypeId		= ui32TypeId;
    psResource->pfnFree			= pfnFree;
    psResource->pvParam         = pvParam;

    /* Allocate resource Id...*/
    i32Result = OSAIDGEN_AllocId(psBucket->hIdGenerator, psResource, &psResource->ui32ResId);
    if(i32Result != IMG_SUCCESS)
    {
        IMG_ASSERT(!"failed to allocate RMAN id");
        return i32Result;
    }
    IMG_ASSERT(psResource->ui32ResId <= RMAN_CRESID_MAX_RES_ID);

    // add this resource to the bucket
    OSA_CritSectLock(ghGlobalMutexHandle);
    DQ_addTail(&psBucket->sResList, psResource);

    /* Update count of resources...*/
    psBucket->ui32ResCnt++;
    OSA_CritSectUnlock(ghGlobalMutexHandle);

    /* If resource handle required...*/
    if (phResHandle != IMG_NULL)
    {
        *phResHandle = psResource;
    }

    /* If resource id required...*/
    if (pui32ResId != IMG_NULL)
    {
        *pui32ResId = OSARMAN_GetResourceId(psResource);
    }

    /* Return success...*/
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				OSARMAN_GetResourceId

******************************************************************************/
IMG_UINT32 OSARMAN_GetResourceId(
    IMG_HANDLE			hResHandle
)
{
    OSARMAN_sResource *			psResource = hResHandle;
    IMG_UINT32					ui32ExtResId;

    IMG_ASSERT(hResHandle != IMG_NULL);
    if (hResHandle == IMG_NULL)
    {
        return 0;
    }

    IMG_ASSERT(psResource->ui32ResId <= RMAN_CRESID_MAX_RES_ID);
    IMG_ASSERT(psResource->psBucket->ui32BucketIndex < RMAN_CRESID_MAX_BUCKET_INDEX);
    if (psResource->psBucket->ui32BucketIndex >= RMAN_CRESID_MAX_BUCKET_INDEX) {
        return 0;
    }

    ui32ExtResId = (((psResource->psBucket->ui32BucketIndex + 1)<<RMAN_CRESID_BUCKET_SHIFT) | psResource->ui32ResId);

    return ui32ExtResId;
}


/*!
******************************************************************************

 @Function				rman_GetResource

******************************************************************************/
static IMG_VOID * rman_GetResource(
    IMG_HANDLE				hResBHandle,
    IMG_UINT32				ui32ResId,
    IMG_UINT32				ui32TypeId,
    IMG_HANDLE *			phResHandle
)
{
    OSARMAN_sBucket *			psBucket = (OSARMAN_sBucket *) hResBHandle;
    OSARMAN_sResource *			psResource;
    IMG_RESULT					i32Result;
	(void)ui32TypeId;

    IMG_ASSERT(ui32ResId <= RMAN_CRESID_MAX_RES_ID);

    /* Loop over the resources in this bucket till we find the required id...*/
    OSA_CritSectLock(ghGlobalMutexHandle);
    i32Result = OSAIDGEN_GetHandle(psBucket->hIdGenerator, ui32ResId, (void**)&psResource);
    OSA_CritSectUnlock(ghGlobalMutexHandle);
    if(i32Result != IMG_SUCCESS)
    {
        IMG_ASSERT(!"failed to get RMAN resource");
        return IMG_NULL;
    }

    /* If the resource handle is required...*/
    if (phResHandle != IMG_NULL)
    {
        /* Return it...*/
        *phResHandle = psResource;
    }

    /* If the resource was not found...*/
    IMG_ASSERT(psResource != IMG_NULL);
    IMG_ASSERT((IMG_VOID*)psResource != &psBucket->sResList);
    if ( (psResource == IMG_NULL) || ((IMG_VOID*)psResource == &psBucket->sResList) )
    {
        return IMG_NULL;
    }

    /* Cross check the type...*/
    IMG_ASSERT(ui32TypeId == psResource->ui32TypeId);

    /* Return the resource....*/
    return psResource->pvParam;
}


/*!
******************************************************************************

 @Function				OSARMAN_GetResource

******************************************************************************/
IMG_RESULT OSARMAN_GetResource(
    IMG_UINT32				ui32ResId,
    IMG_UINT32				ui32TypeId,
    IMG_VOID **				ppvParam,
    IMG_HANDLE *			phResHandle
)
{
    IMG_UINT32				ui32BucketIndex = (ui32ResId>>RMAN_CRESID_BUCKET_SHIFT)-1;
    IMG_UINT32				ui32IntResId	= (ui32ResId & RMAN_CRESID_RES_ID_MASK);
    IMG_VOID *				pvParam;

    IMG_ASSERT(ui32BucketIndex < RMAN_CRESID_MAX_BUCKET_INDEX);
    if (ui32BucketIndex >= RMAN_CRESID_MAX_BUCKET_INDEX) {
        /* Happens when ui32BucketIndex == 0 */
        return IMG_ERROR_INVALID_ID;
    }
    IMG_ASSERT(ui32IntResId <= RMAN_CRESID_MAX_RES_ID);
    if (ui32IntResId > RMAN_CRESID_MAX_RES_ID) {
        return IMG_ERROR_INVALID_ID;
    }
    IMG_ASSERT(gapsBucket[ui32BucketIndex] != IMG_NULL);
    if (gapsBucket[ui32BucketIndex] == NULL) {
        return IMG_ERROR_INVALID_ID;
    }

    pvParam = rman_GetResource(gapsBucket[ui32BucketIndex], ui32IntResId, ui32TypeId, phResHandle);

    /* If we didn't find the resource...*/
    if (pvParam == IMG_NULL)
    {
        return IMG_ERROR_INVALID_ID;
    }

    /* Return the resource...*/
    if (ppvParam != IMG_NULL)
    {
        *ppvParam = pvParam;
    }

    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				rman_CloneResourceHandle

******************************************************************************/
static IMG_RESULT rman_CloneResourceHandle(
    IMG_HANDLE				hResHandle,
    IMG_HANDLE				hResBHandle,
    IMG_HANDLE *			phResHandle,
    IMG_UINT32 *			pui32ResId
)
{
    OSARMAN_sResource *			psResource = hResHandle;
    OSARMAN_sResource *			psCloneResourceHandle;
    IMG_UINT32					ui32Result;

    /* If this resource is not already shared...*/
    if (psResource->psSharedResource == IMG_NULL)
    {
        ui32Result = OSARMAN_RegisterResource(gpsSharedResBucket, psResource->ui32TypeId, psResource->pfnFree, psResource->pvParam, (IMG_HANDLE *)&psResource->psSharedResource, IMG_NULL);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return ui32Result;
        }

        /* Update the no. of references to the shared resource...*/
        psResource->psSharedResource->ui32ReferenceCnt++;
    }

    /* Register this resource...*/
    ui32Result = OSARMAN_RegisterResource(hResBHandle, psResource->ui32TypeId, psResource->pfnFree, psResource->pvParam, (IMG_HANDLE *)&psCloneResourceHandle, IMG_NULL);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        return ui32Result;
    }

    /* Update reference to shared resource...*/
    psResource->psSharedResource->ui32ReferenceCnt++;
    psCloneResourceHandle->psSharedResource = psResource->psSharedResource;

    /* If resource handle required...*/
    if (phResHandle != IMG_NULL)
    {
        *phResHandle = psCloneResourceHandle;
    }

    /* If resource id required...*/
    if (pui32ResId != IMG_NULL)
    {
        *pui32ResId = OSARMAN_GetResourceId(psResource);
    }

    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				OSARMAN_CloneResourceHandle

******************************************************************************/
IMG_RESULT OSARMAN_CloneResourceHandle(
    IMG_HANDLE				hResHandle,
    IMG_HANDLE				hResBHandle,
    IMG_HANDLE *			phResHandle,
    IMG_UINT32 *			pui32ResId
)
{
    IMG_UINT32				ui32Result;

    IMG_ASSERT(gInitialised);

    IMG_ASSERT(hResHandle != IMG_NULL);
    if (hResHandle == IMG_NULL)
    {
        return IMG_ERROR_GENERIC_FAILURE;
    }

    IMG_ASSERT(hResBHandle != IMG_NULL);
    if (hResBHandle == IMG_NULL)
    {
        return IMG_ERROR_GENERIC_FAILURE;
    }

    /* Lock the shared resources...*/
    OSA_CritSectLock(ghSharedResMutexHandle);

    /* Create the clone...*/
    ui32Result = rman_CloneResourceHandle(hResHandle, hResBHandle, phResHandle, pui32ResId);
    IMG_ASSERT(ui32Result == IMG_SUCCESS);
    if (ui32Result != IMG_SUCCESS)
    {
        OSA_CritSectUnlock(ghSharedResMutexHandle);
        return ui32Result;
    }

    /* Exit...*/
    OSA_CritSectUnlock(ghSharedResMutexHandle);
    return IMG_SUCCESS;
}


/*!
******************************************************************************

 @Function				RMAN_GetNamedResource

******************************************************************************/
IMG_RESULT OSARMAN_GetNamedResource(
    IMG_CHAR *				pszResName,
    OSARMAN_pfnAlloc		pfnAlloc,
    IMG_VOID *				pvAllocInfo,
    IMG_HANDLE				hResBHandle,
    IMG_UINT32				ui32TypeId,
    OSARMAN_pfnFree			pfnFree,
    IMG_VOID **             ppvParam,
    IMG_HANDLE *			phResHandle,
    IMG_UINT32 *			pui32ResId
)
{
    OSARMAN_sBucket *			psBucket = hResBHandle;
    OSARMAN_sResource *			psResource;
    IMG_UINT32					ui32Result;
    IMG_VOID *					pvParam;
    IMG_BOOL					bFound = IMG_FALSE;
    IMG_CHAR*                   pszResNameDup;

    IMG_ASSERT(gInitialised);

    IMG_ASSERT(hResBHandle != IMG_NULL);
    if (hResBHandle == IMG_NULL)
    {
        return IMG_ERROR_GENERIC_FAILURE;
    }

    /* Lock the shared resources...*/
    OSA_CritSectLock(ghGlobalMutexHandle);
    psResource = (OSARMAN_sResource *)DQ_first(&psBucket->sResList);
    while ( (psResource != IMG_NULL) && ((IMG_VOID *)psResource != &psBucket->sResList) )
    {
        /* If resource already in the shared list...*/
        if ( (psResource->pszResName != IMG_NULL) && (IMG_STRCMP(pszResName, psResource->pszResName) == 0) )
        {
            IMG_ASSERT(psResource->pfnFree == pfnFree);
            bFound = IMG_TRUE;
            break;
        }

        /* Move to next resource...*/
        psResource = (OSARMAN_sResource *)DQ_next(psResource);
    }
    OSA_CritSectUnlock(ghGlobalMutexHandle);

    /* If the named resource was not found...*/
    if (!bFound)
    {
        /* Allocate the resource...*/
        ui32Result = pfnAlloc(pvAllocInfo, &pvParam);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return ui32Result;
        }

        /* Register the named resource...*/
        ui32Result = OSARMAN_RegisterResource(hResBHandle, ui32TypeId, pfnFree, pvParam, (IMG_HANDLE *)&psResource, IMG_NULL);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            OSA_CritSectUnlock(ghGlobalMutexHandle);
            return ui32Result;
        }

        /* Set resource name...*/
        pszResNameDup = IMG_STRDUP(pszResName);
        OSA_CritSectLock(ghGlobalMutexHandle);
        psResource->pszResName = pszResNameDup;
        OSA_CritSectUnlock(ghGlobalMutexHandle);
    }

    /* Return the pvParam value...*/
    *ppvParam = psResource->pvParam;

    /* If resource handle required...*/
    if (phResHandle != IMG_NULL)
    {
        *phResHandle = psResource;
    }

    /* If resource id required...*/
    if (pui32ResId != IMG_NULL)
    {
        *pui32ResId = OSARMAN_GetResourceId(psResource);
    }

    /* Exit...*/
    return IMG_SUCCESS;
}

/*!
******************************************************************************

 @Function				rman_FreeResource

******************************************************************************/
static IMG_VOID rman_FreeResource(
    OSARMAN_sResource *		psResource
)
{
    OSARMAN_sBucket *				psBucket = psResource->psBucket;

    /* Remove the resource from the active list...*/
    OSA_CritSectLock(ghGlobalMutexHandle);

    /* Remove from list...*/
    DQ_remove(psResource);

    /* Update count of resources...*/
    psBucket->ui32ResCnt--;

    OSA_CritSectUnlock(ghGlobalMutexHandle);

    /* If mutex associated with the resource...*/
    if (psResource->hMutexHandle != IMG_NULL)
    {
        /* Destroy mutex...*/
        OSA_CritSectDestroy(psResource->hMutexHandle);
    }

    /* If this resource is not already shared...*/
    if (psResource->psSharedResource != IMG_NULL)
    {
        /* Lock the shared resources...*/
        OSA_CritSectLock(ghSharedResMutexHandle);

        /* Update the reference count...*/
        IMG_ASSERT(psResource->psSharedResource->ui32ReferenceCnt != 0);
        psResource->psSharedResource->ui32ReferenceCnt--;

        /* If this is the last free for the shared resource...*/
        if (psResource->psSharedResource->ui32ReferenceCnt == 0)
        {
            /* Free the shared resource...*/
            rman_FreeResource(psResource->psSharedResource);
        }
        /* UnLock the shared resources...*/
        OSA_CritSectUnlock(ghSharedResMutexHandle);
    }
    else
    {
        /* If there is a free callback function....*/
        if (psResource->pfnFree != IMG_NULL)
        {
            /* Call resource free callback...*/
            psResource->pfnFree(psResource->pvParam);
        }
    }

    /* If the resource has a name then free it...*/
    if (psResource->pszResName != IMG_NULL)
    {
        IMG_FREE(psResource->pszResName);
    }

    /* Free a resource structure...*/
    IMG_FREE(psResource);
}


/*!
******************************************************************************

 @Function				OSARMAN_FreeResource

******************************************************************************/
IMG_VOID OSARMAN_FreeResource(
    IMG_HANDLE				hResHandle
)
{
    OSARMAN_sResource *			psResource;

    IMG_ASSERT(gInitialised);

    IMG_ASSERT(hResHandle != IMG_NULL);
    if (hResHandle == IMG_NULL)
    {
        return;
    }

    /* Get access to the resource structure...*/
    psResource = (OSARMAN_sResource *)hResHandle;


    /* Free resource...*/
    rman_FreeResource(psResource);
}


/*!
******************************************************************************

 @Function				OSARMAN_LockResource

******************************************************************************/
IMG_VOID OSARMAN_LockResource(
    IMG_HANDLE			hResHandle
)
{
    OSARMAN_sResource *			psResource;
    IMG_UINT32					ui32Result;

    IMG_ASSERT(gInitialised);

    IMG_ASSERT(hResHandle != IMG_NULL);
    if (hResHandle == IMG_NULL)
    {
        return;
    }

    /* Get access to the resource structure...*/
    psResource = (OSARMAN_sResource *)hResHandle;

    /* If this is a shared resource...*/
    if (psResource->psSharedResource != IMG_NULL)
    {
        /* We need to lock/unlock the underlying shared resource...*/
        psResource = psResource->psSharedResource;
    }

    /* If no mutex associated with this resource...*/
    if (psResource->hMutexHandle == IMG_NULL)
    {
        /* Create one...*/
        ui32Result = OSA_CritSectCreate(&psResource->hMutexHandle);
        IMG_ASSERT(ui32Result == IMG_SUCCESS);
        if (ui32Result != IMG_SUCCESS)
        {
            return;
        }
    }

    /* lock it...*/
    OSA_CritSectLock(psResource->hMutexHandle);
}

/*!
******************************************************************************

 @Function				OSARMAN_UnlockResource

******************************************************************************/
IMG_VOID OSARMAN_UnlockResource(
    IMG_HANDLE			hResHandle
)
{
    OSARMAN_sResource *			psResource;

    IMG_ASSERT(gInitialised);

    IMG_ASSERT(hResHandle != IMG_NULL);
    if (hResHandle == IMG_NULL)
    {
        return;
    }

    /* Get access to the resource structure...*/
    psResource = (OSARMAN_sResource *)hResHandle;

    /* If this is a shared resource...*/
    if (psResource->psSharedResource != IMG_NULL)
    {
        /* We need to lock/unlock the underlying shared resource...*/
        psResource = psResource->psSharedResource;
    }

    IMG_ASSERT (psResource->hMutexHandle != IMG_NULL);

    /* Unlock mutex...*/
    OSA_CritSectUnlock(psResource->hMutexHandle);
}


/*!
******************************************************************************

 @Function				OSARMAN_FreeResources

******************************************************************************/
IMG_VOID OSARMAN_FreeResources(
    IMG_HANDLE					hResBHandle,
    IMG_UINT32					ui32TypeId
)
{
    OSARMAN_sBucket *				psBucket = (OSARMAN_sBucket *) hResBHandle;
    OSARMAN_sResource *			psResource;

    IMG_ASSERT(gInitialised);

    IMG_ASSERT(hResBHandle != IMG_NULL);
    if (hResBHandle == IMG_NULL)
    {
        return;
    }

    /* Scan the active list looking for the resources to be freed...*/
    OSA_CritSectUnlock(ghGlobalMutexHandle);
    psResource = (OSARMAN_sResource *)DQ_first(&psBucket->sResList);
    while (
            (psResource != IMG_NULL) &&
            ((IMG_VOID*)psResource != &psBucket->sResList)
            )
    {
        /* If this is resource is to be removed...*/
        if ( ((ui32TypeId == OSARMAN_ALL_TYPES)  && (psResource->ui32TypeId != OSARMAN_STICKY)) || (psResource->ui32TypeId == ui32TypeId) )
        {
            /* Yes, remove it...*/
            /* Free current resource...*/
            OSA_CritSectLock(ghGlobalMutexHandle);
            rman_FreeResource(psResource);
            OSA_CritSectUnlock(ghGlobalMutexHandle);

            /* Restart from the begining of the list...*/
            psResource = (OSARMAN_sResource *)DQ_first(&psBucket->sResList);
        }
        else
        {
            /* Move to next resource...*/
            psResource = (OSARMAN_sResource *)LST_next(psResource);
        }
    }
    OSA_CritSectLock(ghGlobalMutexHandle);
}



