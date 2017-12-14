/**
*******************************************************************************
@file ci_api_internal.h

@brief Internal definition for the user API

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
#ifndef CI_API_INTERNAL_H_
#define CI_API_INTERNAL_H_

#include <img_types.h>
#include <img_defs.h>

#include "ci/ci_api.h"
#include "linkedlist.h"  // NOLINT
#include "sys/sys_userio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup INT_USR Capture Interface: Internal Components
 * @brief Internal components of driver (mostly communication between user
 * and kernel-side driver)
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the INT_API documentation module
 *---------------------------------------------------------------------------*/

/**
 * @defgroup INT_FCT Internal static functions
 * @brief Internal user-side functions that have a scope limited to their
 * file only
 */

/**
 * @brief Internal connection information
 */
typedef struct
{
    /** @brief available to the user */
    CI_CONNECTION publicConnection;

    /** @brief used for open/close/ioctl */
    SYS_FILE *fileDesc;

    /** @brief all the created pipeline configurations (INT_PIPELINE*) */
    sLinkedList_T sList_pipelines;
    /** @brief all the created datagen (INT_DATAGEN*) */
    sLinkedList_T sList_datagen;
    /**
     * @brief all the acquired gasket configurations (CI_GASKET*) 
     *
     * NULL if none, should be populated after the kernel side validated the
     * choice
     */
    struct CI_GASKET *aGaskets[CI_N_IMAGERS];
} INT_CONNECTION;

enum INT_BUFFER_STATUS
{
    /** @brief the buffer is available to trigger a new shot */
    INT_BUFFER_AVAILABLE = 0,
    /** @brief the buffer is currently used in triggered shot */
    INT_BUFFER_PENDING,
    /** @brief the buffer has been captured and is now acquired by user */
    INT_BUFFER_ACQUIRED
};

/**
 * @brief A buffer with ID
 */
typedef struct
{
    /** @brief mapping ID - 0 is invalid */
    IMG_INT32 ID;
    /**
     * @brief File descriptor to ION file buffer 
     */
    IMG_INT32 ionFd;
    /** @brief type of buffer from @ref CI_ALLOC_BUFF */
    int type;

    /** @brief pointer to memory */
    void *memory;
    /** @brief in bytes */
    IMG_UINT32 uiSize;
    IMG_BOOL8 bIsTiled;
    /** @brief in @ref INT_PIPELINE::sList_buffers */
    sCell_T sCell;
    /** @brief main buffer status */
    enum INT_BUFFER_STATUS eStatus;
    /**
     * @brief Is the buffer reserved for HDR insertion?
     *
     * If type is CI_ALLOC_HDRINS then the buffer can be reserved by user to
     * populate the HDR insertion content before triggering a frame.
     *
     * When reserving HDR insertion buffer its status has to be
     * INT_BUFFER_AVAILABLE but if bHDRReserved it cannot be re-reserved
     * until the reservation has been cancelled or the shot captured.
     *
     * This is done by setting it back to IMG_FALSE when triggering but
     * because its eStatus is 'not available' it cannot be acquired before
     * the associated shot has been released too.
     */
    IMG_BOOL8 bHDRReserved;
} INT_BUFFER;

typedef struct
{
    /** @brief mapping ID - 0 is invalid */
    IMG_INT32 ID;
    /** @brief pointer to memory */
    void *memory;
    /** @brief in bytes */
    IMG_UINT32 uiSize;
    /** @brief main buffer status */
    enum INT_BUFFER_STATUS eStatus;
    /** @brief in @ref INT_PIPELINE::sList_lshmat */
    sCell_T sCell;
    CI_MODULE_LSH_MAT config;
} INT_LSHMAT;

typedef struct
{
    int iIdentifier;
    IMG_BOOL8 bAcquired;

    CI_SHOT publicShot;
    /** @brief associated encoder output buffer */
    INT_BUFFER *pEncOutput;
    /** @brief associated display output buffer */
    INT_BUFFER *pDispOutput;
    /** @brief associated HDR extraction buffer */
    INT_BUFFER *pHDRExtOutput;
    /** @brief associated Raw 2D extraction buffer */
    INT_BUFFER *pRaw2DExtOutput;
    /**
     * @brief cell element of @ref INT_PIPELINE::sList_shots
     */
    sCell_T sCell;
} INT_SHOT;

/**
 * @brief Internal configuration object
 */
typedef struct
{
    /** @brief available to the user */
    CI_PIPELINE publicPipeline;
    /**
     * @brief contain all the memory-mapped shots (INT_SHOT*)
     */
    sLinkedList_T sList_shots;
    /** @brief list of memory mapped buffers (INT_BUFFER*) */
    sLinkedList_T sList_buffers;
    /** @brief list of memory mapped lsh matrix (INT_LSHMAT*) */
    sLinkedList_T sList_lshmat;

    /** @brief identifier given by kernel module */
    int ui32Identifier;
    /** @brief store capture started state from kernel side */
    IMG_BOOL bStarted;
    /**
     * @brief stores if the pipeline can allocate/import tiled buffers
     *
     * Changed at registration time.
     */
    IMG_BOOL bSupportTiling;
    /**
     * @brief matrix currently in use - part of sList_lshmat
     */
    INT_LSHMAT *pMatrix;

    /** @brief element in INT_CONNECTION::sList_pipelines */
    sCell_T sCell;
    /** @brief parent connection */
    INT_CONNECTION *pConnection;

    /**
     * @brief contains the load structure, allocated when creating object
     */
    void *pLoadStructure;
} INT_PIPELINE;

typedef struct
{
    /** @brief identifier given by kernel module */
    int ui32DatagenID;
    IMG_BOOL8 bStarted;

    /** INT_DG_FRAME* all frames - status is given by kernel module */
    sLinkedList_T sListFrames;

    /** @brief element in INT_CONNECTION::sList_datagen */
    sCell_T sCell;
    INT_CONNECTION *pConnection;

    CI_DATAGEN publicDatagen;
} INT_INTDATAGEN;

typedef struct
{
    void *data;
    /** @brief in Bytes */
    IMG_UINT32 ui32AllocSize;
    /** @brief unique frame ID */
    IMG_UINT32 ui32FrameID;

    CI_DG_FRAME publicFrame;
    INT_INTDATAGEN *pDatagen;
    sCell_T sCell;
} INT_DGFRAME;

/** @brief size of the load structure in bytes for HW 2.X */
IMG_UINT32 INT_CI_SizeLS_HW2(void);
/** @brief write the load structure from configuration for HW 2.X*/
IMG_RESULT INT_CI_Load_HW2(const INT_PIPELINE *pIntPipeline, char* loadstruct);
/** @brief extract stats config from load structure for HW 2.X */
IMG_RESULT INT_CI_Revert_HW2(const CI_SHOT *pShot, CI_PIPELINE_STATS *pStats);
/** @brief  Write AWS configuration to registers.
 *  Used here because CI unit tests need it. */
void HW_CI_Load_AWS(char *pMemory, const struct CI_MODULE_AWS *pWhiteBalance);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * end of the INT_USR documentation module
 *---------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* CI_API_INTERNAL_H_ */
