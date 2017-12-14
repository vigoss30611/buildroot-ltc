/**
*******************************************************************************
 @file dg_camera.h

 @brief Internal declarations

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
#ifndef DG_CAMERA_KRN
#define DG_CAMERA_KRN

#include <img_types.h>
#include <linkedlist.h>

#include <ci_internal/sys_parallel.h>
#include <ci_internal/sys_mem.h>
#include <ci_internal/sys_device.h>

#include "felix_hw_info.h"

#include "ci_kernel/ci_kernel_structs.h"  // KRN_CI_MMU
#include "dg/dg_api_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup DG_KERNEL Data Generator: kernel-side driver
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the DG_KERNEL documentation module
 *---------------------------------------------------------------------------*/

struct MMUDirectory;  // from mmulib/mmu.h
struct MMUHeap;  // from mmulib/heap.h

enum DG_HEAPS
{
    /** @brief Memory containing Data */
    DG_IMAGE_HEAP = 0,
    /** @brief Memory containing untiled Image data */
    DG_DATA_HEAP,
    DG_N_HEAPS
};

typedef struct KRN_DG_CONNECTION
{
    SYS_LOCK sLock;

    sLinkedList_T sList_camera;
    sLinkedList_T sList_unmappedFrames;

    IMG_BOOL8 bStarted;
    int iCameraIDBase;
    int iFrameIDBase;

} KRN_DG_CONNECTION;

typedef struct KRN_DG_CAMERA
{
    int identifier;
    IMG_BOOL8 bStarted;

    /** @brief number of available buffers in sList_available */
    SYS_SEM sSemAvailable;
    SYS_LOCK sPendingLock;
    /**
     * List of (KRN_DG_FRAME*)
     * @note protected by sPendingLock
     */
    sLinkedList_T sList_available;
    /**
     * list of (KRN_DG_FRAME*)
     * @note protected by sPendingLock
     */
    sLinkedList_T sList_converting;
    /**
     * list of (KRN_DG_FRAME*)
     * @note protected by sPendingLock
     */
    sLinkedList_T sList_pending;

    struct MMUHeap *apHeap[DG_N_HEAPS];

    DG_CAMERA publicCamera;
} KRN_DG_CAMERA;

typedef struct KRN_DG_FRAME
{
    KRN_DG_CAMERA *pParent;

    int identifier;
    IMG_SIZE uiStride;
    sCell_T sListCell;

    /** @brief Actual image buffer */
    SYS_MEM sBuffer;
} KRN_DG_FRAME;

/**
 * @note Uses CI's driver to handle interrupts but has its own workqueue
 */
typedef struct KRN_DG_DRIVER
{
    /** @brief Register handle for the several data generators */
    IMG_HANDLE hRegFelixDG[CI_N_EXT_DATAGEN];
    /** @brief Memory handle for buffer's image. Copied from KRN_CI_DRIVER */
    IMG_HANDLE hMemHandle;
    IMG_HANDLE hRegTestIO;  // Debug only

    DG_HWINFO sHWInfo;

    SYS_SPINLOCK sWorkSpinlock;
    sLinkedList_T sWorkqueue;

    /** @brief Locks to change aActiveCamera and use hRegFelixDG */
    SYS_LOCK sCameraLock;
    /**
     * @brief Currently active cameras (i.e. holding HW access)
     *
     * @note protected by sCameraLock
     */
    KRN_DG_CAMERA *aActiveCamera[CI_N_EXT_DATAGEN];
    /** @brief used to know how many frame sent was last read per context */
    IMG_UINT16 aFrameSent[CI_N_EXT_DATAGEN];

    /** @brief the device is not looked for */
    SYS_DEVICE sDevice;

    /** @brief The MMU to use with the data generator */
    KRN_CI_MMU sMMU;

    SYS_LOCK sConnectionLock;
    /** @brief list of connection to the driver (KRN_DG_CAMERA*) */
    sLinkedList_T sList_connection;
} KRN_DG_DRIVER;

extern KRN_DG_DRIVER *g_pDGDriver;

// functions needed at several places

/**
 * @brief Used to destroy frame elements in lists using the visitor
 * (cells are contained into the frames)
 */
IMG_BOOL8 List_DestroyFrames(void *elem, void *param);

/**
 * @name Driver control
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *---------------------------------------------------------------------------*/

IMG_RESULT KRN_DG_DriverCreate(KRN_DG_DRIVER *pDriver,
    IMG_UINT8 ui8MMUEnabled);

irqreturn_t KRN_DG_DriverHardHandleInterrupt(int irq, void *dev_id);
irqreturn_t KRN_DG_DriverThreadHandleInterrupt(int irq, void *dev_id);

IMG_RESULT KRN_DG_DriverAcquireContext(KRN_DG_CAMERA *pCamera);
IMG_RESULT KRN_DG_DriverReleaseContext(KRN_DG_CAMERA *pCamera);

IMG_RESULT KRN_DG_DriverDestroy(KRN_DG_DRIVER *pDriver);

/**
 * @}
 */

/**
 * @name Connection
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *---------------------------------------------------------------------------*/

IMG_RESULT KRN_DG_ConnectionCreate(KRN_DG_CONNECTION **pConnection);

IMG_RESULT KRN_DG_ConnectionDestroy(KRN_DG_CONNECTION *pConnection);

/**
 * @name Camera
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *---------------------------------------------------------------------------*/

IMG_RESULT KRN_DG_CameraCreate(KRN_DG_CAMERA **ppCamera);

IMG_RESULT KRN_DG_CameraStart(KRN_DG_CAMERA *pCamera);
IMG_RESULT KRN_DG_CameraStop(KRN_DG_CAMERA *pCamera);
IMG_RESULT HW_DG_CameraConfigure(KRN_DG_CAMERA *pCamera);
/**
 * @brief Write the next frame into the HW register
 *
 * @warning assumes that pCamera->sPendingLock is already locked
 */
IMG_RESULT HW_DG_CameraSetNextFrame(KRN_DG_CAMERA *pCamera,
    KRN_DG_FRAME *pFrame);

// triggers a capture
IMG_RESULT KRN_DG_CameraShoot(KRN_DG_CAMERA *pCamera, int frameId,
    IMG_BOOL8 bTrigger);
/**
 * @brief Called when interrupt Frame end is received
 *
 * @warning Assumes that sPendingLock is locked when called
 *
 * Pops the front of the pending list into the available list.
 * If the pending list stil has elements submit a new one to the HW.
 *
 * @return IMG_SUCCESS
 * @return deletegates to HW_DG_CameraConfigure()
 */
IMG_RESULT KRN_DG_CameraFrameCaptured(KRN_DG_CAMERA *pCamera);

IMG_RESULT KRN_DG_CameraDestroy(KRN_DG_CAMERA *pCamera);

/**
 * @}
 */

/**
 * @name Frame
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *---------------------------------------------------------------------------*/

IMG_RESULT KRN_DG_FrameCreate(KRN_DG_FRAME **ppFrame, KRN_DG_CAMERA *pCam);
IMG_RESULT KRN_DG_FrameMap(KRN_DG_FRAME *pFrame, struct MMUDirectory *pDir);
IMG_RESULT KRN_DG_FrameUnmap(KRN_DG_FRAME *pFrame);
IMG_RESULT KRN_DG_FrameDestroy(KRN_DG_FRAME *pFrame);

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // DG_CAMERA_KRN
