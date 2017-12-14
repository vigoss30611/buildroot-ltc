/**
*******************************************************************************
 @file dg_api_internal.h

 @brief Data Generator structures

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
#ifndef DG_API_INTERNAL_H_
#define DG_API_INTERNAL_H_

#include <linkedlist.h>
#include <stdio.h>

#include "dg/dg_api_structs.h"

#include "sys/sys_userio.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _sSimImageIn_;  // sim_image.h

/**
 * @defgroup DG_Conv Internal and converter object (DG_Conv)
 * @ingroup DG_API
 * @brief Converter and internal objects
 *
 * Internal objects are used to store more information than user-side needs.
 *
 * The converter transforms FLX images into data-generator ready memory
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the DG_API documentation module
 *---------------------------------------------------------------------------*/

struct INT_DG_CONNECTION
{
    SYS_FILE *fileDesc;
    DG_CONNECTION publicConnection;

    sLinkedList_T sList_cameras;
};

struct INT_DG_CAMERA
{
    int identifier;
    /** @brief in connection's list */
    sCell_T sCell;
    IMG_BOOL8 bStarted;
    DG_CAMERA publicCamera;

    /** @brief List of memory mapped frames (struct INT_DG_FRAME*) */
    sLinkedList_T sList_frames;
    struct INT_DG_CONNECTION *pConnection;
};

struct INT_DG_FRAME
{
    unsigned int identifier;
    sCell_T sCell;

    /** @brief allocated size */
    IMG_SIZE ui32Size;

    void *data;
    // may need a public frame
    struct INT_DG_CAMERA *pParent;
    DG_FRAME publicFrame;
};

/**
 * @}
 */
#define DG_LOG_TAG "DG_UM"

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the DG_INT documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DG_API_INTERNAL_H_ */
