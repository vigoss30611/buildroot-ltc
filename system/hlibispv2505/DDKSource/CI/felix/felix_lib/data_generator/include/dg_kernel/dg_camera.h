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
#ifndef DG_CAMERA_H_
#define DG_CAMERA_H_

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
struct DG_FRAME_PARAM;  // from dg_ioctl.h

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
    sCell_T sCell;
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
    /** @brief element in the connection's list */
    sCell_T sCell;

    /** @brief proctects access to the lists */
    SYS_LOCK sLock;
    /**
     * @brief number of elements in the sList_processed
     *
     * Incremented by interrupt handler when adding elements so that user-side
     * can wait for frames to be processed.
     */
    SYS_SEM sSemProcessed;
    /**
     * List of @ref KRN_DG_FRAME
     * @note protected by sLock
     */
    sLinkedList_T sList_available;
    /**
     * list of @ref KRN_DG_FRAME
     * @note protected by sLock
     */
    sLinkedList_T sList_converting;
    /**
     * list of @ref KRN_DG_FRAME
     * @note protected by sLock
     */
    sLinkedList_T sList_pending;
    /**
     * list of @ref KRN_DG_FRAME
     * @note protected by sLock
     */
    sLinkedList_T sList_processed;

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

    // additional info to write to the HW when triggering
    IMG_UINT32 ui32HoriBlanking;
    IMG_UINT32 ui32VertBlanking;

    IMG_UINT16 ui16Height;
    /** for parallel width in pixels, for MIPI converted width */
    IMG_UINT16 ui16Width;
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
    IMG_UINT8 aEDGPLL[2];  // PLL multiplier and divider
    /**
     * @brief gasket phy for data-generator and sensor used for
     * reclocking DG - assumes there is only 1 PHY regbank
     */
    IMG_HANDLE hRegGasketPhy;

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

    /** @brief protects the list of connections */
    SYS_LOCK sConnectionLock;
    /**
     * @brief list of connection to the driver (KRN_DG_CAMERA*)
     *
     * @note protected by sConnectionLock
     */
    sLinkedList_T sList_connection;
} KRN_DG_DRIVER;

extern KRN_DG_DRIVER *g_pDGDriver;

// functions needed at several places

/**
 * @brief Used to destroy frame elements in lists using the visitor
 * (cells are contained into the frames)
 */
IMG_BOOL8 List_DestroyFrames(void *elem, void *param);

IMG_BOOL8 List_SearchFrame(void *listElem, void *searched);

/**
 * @name Driver control
 * @brief General driver functions to handle resources.
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IOCTL module
 *---------------------------------------------------------------------------*/

/**
 * @brief Create the kernel object of the external data-generator driver
 *
 * @note if either ui8EDGPLL_mult or ui8EDGPLL_div is 0 then the PLL default
 * is computed (i.e. both need to be provided to use the values)
 *
 * @param pDriver
 * @param ui8EDGPLL_mult PLL multiplier writen to test IO registers
 * @param ui8EDGPLL_div PLL divider writen to the test IO registers
 * @param ui8MMUEnabled enable the MMU (2) or disabled it (0)
 */
IMG_RESULT KRN_DG_DriverCreate(KRN_DG_DRIVER *pDriver,
    IMG_UINT8 ui8EDGPLL_mult, IMG_UINT8 ui8EDGPLL_div,
    IMG_UINT8 ui8MMUEnabled);

irqreturn_t KRN_DG_DriverHardHandleInterrupt(int irq, void *dev_id);
irqreturn_t KRN_DG_DriverThreadHandleInterrupt(int irq, void *dev_id);

IMG_RESULT DEV_DG_Suspend(SYS_DEVICE *pDev, pm_message_t state);
IMG_RESULT DEV_DG_Resume(SYS_DEVICE *pDev);

/**
 * @brief Reserves the HW associated with the pCamera
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if the selected gasket is not of a
 * compatible format
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if the selected DG is already
 * in use
 */
IMG_RESULT KRN_DG_DriverAcquireContext(KRN_DG_CAMERA *pCamera);
/**
 * @brief Release the reserved HW associated with the pCamera
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_UNEXPECTED_STATE if selected gasket does not have the
 * given camera as owner.
 */
IMG_RESULT KRN_DG_DriverReleaseContext(KRN_DG_CAMERA *pCamera);

IMG_RESULT KRN_DG_DriverDestroy(KRN_DG_DRIVER *pDriver);

/**
 * @}
 * @name Connection object
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Connection object module
 *---------------------------------------------------------------------------*/

/** @brief Creation of a connection object - returns NULL on failure */
KRN_DG_CONNECTION* KRN_DG_ConnectionCreate(void);

/** @brief Destruction of a connection object */
IMG_RESULT KRN_DG_ConnectionDestroy(KRN_DG_CONNECTION *pConnection);

/**
 * @}
 * @name Camera object
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Camera object module
 *---------------------------------------------------------------------------*/

/** @brief Initialises a new DG_Camera, returns NULL on failure */
KRN_DG_CAMERA* KRN_DG_CameraCreate(void);

IMG_RESULT KRN_DG_CameraVerify(const DG_CAMERA *pCamera);

/**
 * @brief Starts a camera by reserving the needed HW and ensuring associated
 * gasket is of the correct type.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if selected gasket does not have an
 * associated data-generator
 *
 * Delegates to @ref KRN_DG_DriverAcquireContext() and
 * @ref HW_DG_CameraConfigure()
 */
IMG_RESULT KRN_DG_CameraStart(KRN_DG_CAMERA *pCamera);

/**
 * @brief Stops the camera and release the acquired HW
 *
 * @return IMG_SUCCESS
 * @return IMG_
 */
IMG_RESULT KRN_DG_CameraStop(KRN_DG_CAMERA *pCamera);

/**
 * @brief Trigger a capture or cancel the requested buffer
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if trying to trigger a stopped camera or
 * the blanking is too small or the frame format is different from the camera
 * format used when starting
 * @return IMG_ERROR_FATAL if frame is not found
 */
IMG_RESULT KRN_DG_CameraShoot(KRN_DG_CAMERA *pCamera,
    struct DG_FRAME_PARAM *pParam);

/**
 * @brief Called when interrupt Frame end is received
 *
 * @warning Assumes that sPendingLock is locked when called
 *
 * Pops the front of the pending list into the processed list.
 * If the pending list stil has elements submit a new one to the HW.
 *
 * @return IMG_SUCCESS
 * @return deletegates to HW_DG_CameraConfigure()
 */
IMG_RESULT KRN_DG_CameraFrameCaptured(KRN_DG_CAMERA *pCamera);

/**
 * @brief Called by the user to wait for a frame to be fully processed.
 *
 * When fully processed the frame is moved from the processed list to the
 * available list.
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if the camera is not started
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if there is not processed
 * frame and the call is not blocking
 * @return IMG_ERROR_INTERRUPTED if interrupted while waiting for semaphore
 * or waiting timedout
 */
IMG_RESULT KRN_DG_CameraWaitProcessed(KRN_DG_CAMERA *pCamera,
    IMG_BOOL8 bBlocking);

/**
 * @brief Destroy a camera
 *
 * If capture is running will delegate to @ref KRN_DG_CameraStop()
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT KRN_DG_CameraDestroy(KRN_DG_CAMERA *pCamera);

/**
 * @}
 * @name HW access
 * @brief Functions that give access to the registers
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the HW access module
 *---------------------------------------------------------------------------*/

/**
 * @brief reset a DG register bank - no check done!
 */
void HW_DG_CameraReset(IMG_UINT8 ui8Context);

/**
 * @brief Write initial HW configuration
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if failed to reset the HW
 */
IMG_RESULT HW_DG_CameraConfigure(KRN_DG_CAMERA *pCamera);

/**
 * @brief Stop the HW
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the HW did not stop
 */
IMG_RESULT HW_DG_CameraStop(KRN_DG_CAMERA *pCamera);

/**
 * @brief Write the next frame into the HW register
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if data-generator is alread processing a frame
 */
IMG_RESULT HW_DG_CameraSetNextFrame(KRN_DG_CAMERA *pCamera,
    KRN_DG_FRAME *pFrame);

/**
 * @brief Write the PLL register for the external data generator in the PHY
 *
 * @note ui8ClkMult/ui8ClkDiv has to be > 1
 *
 * @param ui8ClkMult multiplier (range 5-64)
 * @param ui8ClkDiv divider (range 1-128)
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if ui8ClkMult < ui8ClkDiv or either is not
 * in range
 * @return IMG_ERROR_FATAL if PLL do not settle
 */
IMG_RESULT HW_DG_WritePLL(IMG_UINT8 ui8ClkMult, IMG_UINT8 ui8ClkDiv);

/**
 * @}
 * @name Frame object
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the Frame object module
 *---------------------------------------------------------------------------*/

/** @brief Allocate a frame of a given size */
IMG_RESULT KRN_DG_FrameCreate(KRN_DG_FRAME **ppFrame, IMG_UINT32 ui32Size,
    KRN_DG_CAMERA *pCam);

/**
 * @brief Map to the device MMU
 *
 * @param pFrame should not be NULL
 * @param pDir can be NULL (no mapping done if NULL)
 */
IMG_RESULT KRN_DG_FrameMap(KRN_DG_FRAME *pFrame, struct MMUDirectory *pDir);

/**
 * @brief Unmap from the device MMU
 *
 * If not mapped this does nothing.
 *
 * @param pFrame should not be NULL
 */
IMG_RESULT KRN_DG_FrameUnmap(KRN_DG_FRAME *pFrame);

/** @brief Deallocate the frame */
IMG_RESULT KRN_DG_FrameDestroy(KRN_DG_FRAME *pFrame);

/*-----------------------------------------------------------------------------
 * End of modules
 *---------------------------------------------------------------------------*/
/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of group
 *---------------------------------------------------------------------------*/
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* DG_CAMERA_H_ */
