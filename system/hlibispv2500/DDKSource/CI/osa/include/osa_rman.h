/*!
******************************************************************************
 @file   : rman_api.h

 @brief  The Resource Manager API.

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
         This file contains the header file information for the
         Resource Manager API.

 \n<b>Platform:</b>\n
         Platform Independent

******************************************************************************/

#if !defined (__OSA_RMAN_H_)
#define __OSA_RMAN_H_

#include <img_errors.h>
#include <img_types.h>
#include <img_defs.h>

#if defined(__cplusplus)
extern "C" {
#endif

#include <lst.h>

#define	OSARMAN_ALL_TYPES		(0xFFFFFFFF)	//!< Use to signify all "types" of resource. ( non-sticky ones )
#define	OSARMAN_TYPE_P1		(0xFFFFFFFE)	//!< Use to signify priority 1 "type" resource.
#define	OSARMAN_TYPE_P2		(0xFFFFFFFE)	//!< Use to signify priority 2 "type" resource.
#define	OSARMAN_TYPE_P3		(0xFFFFFFFE)	//!< Use to signify priority 3 "type" resource.
#define OSARMAN_STICKY		(0xFFFFFFFD)	//!< Use to allocate it among the last components.

/*!
******************************************************************************

 @Function				OSARMAN_Initialise

 @Description

 This function is used to initialises the Resource Manager component and should
 be called at start-up.

 @Input		None.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
                            error code.

******************************************************************************/
extern IMG_RESULT OSARMAN_Initialise(IMG_VOID);


/*!
******************************************************************************

 @Function				OSARMAN_Deinitialise

 @Description

 This function is used to deinitialises the Resource Manager component and
 would normally be called at shutdown.

 NOTE: This destroys any active resource buckets using RMAN_DestroyBucket().

 @Input		None.

 @Return	None.

******************************************************************************/
extern IMG_VOID OSARMAN_Deinitialise(IMG_VOID);


/*!
******************************************************************************

 @Function				OSARMAN_CreateBucket

 @Description

 This function is used to create a resource bucket into which resources can be
 placed.

 @Output	phResBHandle :	A pointer used to return the bucket handle.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
                            error code.

******************************************************************************/
extern IMG_RESULT OSARMAN_CreateBucket(
    IMG_HANDLE *		phResBHandle
);

/*!
******************************************************************************

 @Function				OSARMAN_DestroyBucket

 @Description

 This function is used to destroy a resource bucket.

 NOTE: Destroying a resource bucket frees all of the resources within the bucket
 by calling the associated free function #RMAN_pfnFree defined when the resoure
 what registered using RMAN_RegisterResource().

 NOTE: When a bucket is restroyed the resources are freed in the order:
    -	#RMAN_TYPE_P1
    -	#RMAN_TYPE_P2
    -	#RMAN_TYPE_P3
    -	#RMAN_ALL_TYPES

 By using these predefined types it is possible to control the order in which
 resources are freed.

 @Input		hResBHandle :	The handle for the bucket returned by RMAN_CreateBucket().

 @Return	None.

******************************************************************************/
extern IMG_VOID OSARMAN_DestroyBucket(
    IMG_HANDLE			hResBHandle
);


/*!
******************************************************************************

 @Function				OSARMAN_GetGlobalBucket

 @Description

 This function is used to obtain the global resource bucket into which global
 resources can be placed.

 @Input		None.

 @Return	IMG_HANDLE :	This function returns a handle to the resource
                            bucket.  IMG_NULL if the global bucket is not available

******************************************************************************/
extern IMG_HANDLE OSARMAN_GetGlobalBucket(IMG_VOID);



/*!
******************************************************************************

 @Function              OSARMAN_pfnFree

 @Description

 This is the prototype for "free" callback functions.  This function is called
 when a resource registered with the Resource Manager is to be freed.

 @Input		pvParam :		Pointer parameter, defined when the
                            resource was registered using RMAN_RegisterResource().

 @Return	None.

******************************************************************************/
typedef IMG_VOID ( * OSARMAN_pfnFree) (
    IMG_VOID *                  pvParam
);


/*!
******************************************************************************

 @Function				OSARMAN_RegisterResource

 @Description

 This function is used to register a resource with the Resource Manager.

 @Input		hResBHandle :	The handle for the bucket returned by RMAN_CreateBucket().

 @Input		ui32TypeId :	Normally used to identify the "type" of resource.

 @Input		pfnFree :		A pointer to the "free" function - called to free
                            the resource.  IMG_NULL if no free function is
                            to be called - freeing of the resource must be
                            handled outside of the Resource Manager.

 @Input		pvParam :		An IMG_VOID * value passed to the "free" function
                            when freeing this resource.

 @Output	phResHandle :	A pointer used to return a handle to the resource.
                            IMG_NULL if the resource handle is not required.

 @Output	pui32ResId :	A pointer used to return the resource Id.
                            IMG_NULL if the resource Id is not required.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
                            error code.

******************************************************************************/
extern IMG_RESULT OSARMAN_RegisterResource(
    IMG_HANDLE				hResBHandle,
    IMG_UINT32				ui32TypeId,
    OSARMAN_pfnFree			pfnFree,
    IMG_VOID *				pvParam,
    IMG_HANDLE *			phResHandle,
    IMG_UINT32 *			pui32ResId
);



/*!
******************************************************************************

 @Function              OSARMAN_pfnAlloc

 @Description

 This is the prototype for "alloc" callback functions.  This function is called
 when a named resource is to be created.

 @Input		pvAllocInfo :	A pointer to the allcation info passed into the
                            RMAN_GetNamedResource() function.

 @Output	ppvParam :		A pointer used to return the resource.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
                            error code.

******************************************************************************/
typedef IMG_RESULT ( * OSARMAN_pfnAlloc) (
    IMG_VOID *				pvAllocInfo,
    IMG_VOID **				ppvParam
);


/*!
******************************************************************************

 @Function				OSARMAN_GetNamedResource

 @Description

 This function is used to get or allocate a "named" resource.

 If the resource does not exist then a resource is allocated by calling the
 #RMAN_pfnAlloc function and the ppvParam is used by the allocate function
 to return the resource and the resource manage returns a handle to the
 resource via phResHandle.

 If the resource exists then the resource manager returns the ppvParam and
 handle to the resource via phResHandle.

 NOTE: A count of allocates is maintained for named resources which is
 decremented when the resources is freed.  Only on the last free is the
 resource free function called.

 @Input		pszResName :	The resource name.

 @Input		pfnAlloc :		A pointer to the "alloc" function - called to
                            allocate the resource.

 @Input		pvAllocInfo :	A pointer used to pass infomation to the pfnAlloc functions.

 @Input		hResBHandle :	The handle for the bucket into which the named
                            resource will be placed.

 @Input		ui32TypeId :	Normally used to identify the "type" of resource.

 @Input		pfnFree :		A pointer to the "free" function - called to free
                            the resource.  IMG_NULL if no free function is
                            to be called - freeing of the resource must be
                            handled outside of the Resource Manager.

 @Output	ppvParam :		A pointer used to return the allocate resource
                            as returned by the #RMAN_pfnAlloc function.  This
                            is also used passed to the "free" function
                            when freeing this resource.

 @Output	phResHandle :	A pointer used to return a handle to the resource.
                            IMG_NULL if the resource handle is not required.

 @Output	pui32ResId :	A pointer used to return the resource Id.
                            IMG_NULL if the resource Id is not required.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
                            error code.

******************************************************************************/
extern IMG_RESULT OSARMAN_GetNamedResource(
    IMG_CHAR *				pszResName,
    OSARMAN_pfnAlloc		pfnAlloc,
    IMG_VOID *				pvAllocInfo,
    IMG_HANDLE				hResBHandle,
    IMG_UINT32				ui32TypeId,
    OSARMAN_pfnFree			pfnFree,
    IMG_VOID **             ppvParam,
    IMG_HANDLE *			phResHandle,
    IMG_UINT32 *			pui32ResId
);


/*!
******************************************************************************

 @Function				OSARMAN_GetResourceId

 @Description

 This function is used to get the resource ID allocated to the resource by the
 Resource Manager.

 @Input		hResHandle :	The resource handle returned by
                            RMAN_RegisterResource()

 @Return	IMG_UINT32 :	The allocated resource Id.

******************************************************************************/
extern IMG_UINT32 OSARMAN_GetResourceId(
    IMG_HANDLE			hResHandle
);


/*!
******************************************************************************

 @Function				OSARMAN_GetResource

 @Description

 This function is used to get the resource given the bucket in which the resource
 is to be found and the resource Id.

 @Input		ui32ResId :		The resource ID returned by RMAN_GetResourceId();

 @Input		ui32TypeId :	The "type" of resource specified when the resource
                            was registered using RMAN_RegisterResource().
                            NOTE: This parameter MUST match and us used as a
                            cross check that the id matches the type id.

 @Output	ppvParam :		A pointer use to return the pvParam parameter defined
                            when the resource was registered using RMAN_RegisterResource().

 @Output	phResHandle :	A pointer used to return a handle to the resource.
                            IMG_NULL if the resource handle is not required.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
                            error code.

******************************************************************************/
extern IMG_RESULT OSARMAN_GetResource(
    IMG_UINT32				ui32ResId,
    IMG_UINT32				ui32TypeId,
    IMG_VOID **				ppvParam,
    IMG_HANDLE *			phResHandle
);


/*!
******************************************************************************

 @Function				OSARMAN_CloneResourceHandle

 @Description

 This function is used to clone a resource - this creates an additional
 reference to the resource.

 NOTE: A resource is only freed when the last reference to the resource is
 freed.

 @Input		hResHandle :	The resource handle returned by
                            RMAN_RegisterResource()

 @Input		hResBHandle :	The handle for the bucket into which the cloned
                            resource will be placed.

 @Output	phResHandle :	A pointer used to return a handle to the resource.
                            IMG_NULL if the resource handle is not required.

 @Output	pui32ResId :	A pointer used to return the resource Id.
                            IMG_NULL if the resource Id is not required.

 @Return	IMG_RESULT :	This function returns either IMG_SUCCESS or an
                            error code.

******************************************************************************/
extern IMG_RESULT OSARMAN_CloneResourceHandle(
    IMG_HANDLE				hResHandle,
    IMG_HANDLE				hResBHandle,
    IMG_HANDLE *			phResHandle,
    IMG_UINT32 *			pui32ResId
);


/*!
******************************************************************************

 @Function				OSARMAN_FreeResource

 @Description

 This function is used to free resource a individual resource.  This function
 calls the "free" function for the resource defined in the call to
 RMAN_RegisterResource() and de-registers the resource.

 @Input		hResHandle :	The resource handle returned by
                            RMAN_RegisterResource()

 @Return	None.

******************************************************************************/
extern IMG_VOID OSARMAN_FreeResource(
    IMG_HANDLE				hResHandle
);


/*!
******************************************************************************

 @Function				OSARMAN_LockResource

 @Description

 This function is used to lock a resource.

 @Input		hResHandle :	The resource handle returned by
                            RMAN_RegisterResource()

 @Return	None.

******************************************************************************/
extern IMG_VOID OSARMAN_LockResource(
    IMG_HANDLE				hResHandle
);


/*!
******************************************************************************

 @Function				OSARMAN_UnlockResource

 @Description

 This function is used to unlock a resource.

 @Input		hResHandle :	The resource handle returned by
                            RMAN_RegisterResource()

 @Return	None.

******************************************************************************/
extern IMG_VOID OSARMAN_UnlockResource(
    IMG_HANDLE				hResHandle
);


/*!
******************************************************************************

 @Function				OSARMAN_FreeResources

 @Description

 This function is used to free resources.  The function calls the "free"
 functions for the resources and de-registers the resources.

 @Input		hResBHandle :	The handle for the bucket returned by RMAN_CreateBucket().

 @Input		ui32TypeId :	Normally used to identify the "type" of resource to be
                            freed. #RMAN_ALL_TYPES to free for all "type".

 @Return	None.

******************************************************************************/
extern IMG_VOID OSARMAN_FreeResources(
    IMG_HANDLE				hResBHandle,
    IMG_UINT32				ui32TypeId
);

#if defined(__cplusplus)
}
#endif




#endif // OSA_RMAN_H
