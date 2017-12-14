/**
*******************************************************************************
 @file ci_kernel_structs.h

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
#ifndef CI_KERNEL_STRUCTS_H_
#define CI_KERNEL_STRUCTS_H_

#include <img_types.h>

#include <ci/ci_api_structs.h>

// IMG_HANDLE definition
//#include <tal.h>  // NOLINT
#include "linkedlist.h"  // NOLINT

#include "ci_internal/sys_device.h"
#include "ci_internal/sys_mem.h"
#include "ci_internal/sys_parallel.h"  // locks and semaphores

#ifdef __cplusplus
extern "C" {
#endif

// KRN_CI_API defined in ci_kernel.h
/**
 * @addtogroup KRN_CI_API
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the KRN_CI_API documentation module
 *---------------------------------------------------------------------------*/

struct KRN_CI_DRIVER_HANDLE
{
    IMG_HANDLE hRegFelixCore;
    IMG_HANDLE hRegGammaLut;
    IMG_HANDLE hRegFelixContext[CI_N_CONTEXT];
    // IMG_HANDLE hRegFelixI2C;
    /**
     * @note HW version 1.2 only has 1 reg bank for all gaskets (so uses only
     * element 0 of array, later version have 1 reg bank per gasket
     */
    IMG_HANDLE hRegGaskets[CI_N_IMAGERS];
    /**
     * @note HW version 1.2 does not have these registers banks. 
     * Version 2.1 has only 1 (but could change)
     */
    IMG_HANDLE hRegIIFDataGen[CI_N_IIF_DATAGEN];
    IMG_HANDLE hMemHandle;
};

enum CI_HEAPS
{
    /** @brief Memory containing Data */
    CI_DATA_HEAP = 0,
    /** @brief Internal Data Generator heap (always using directory 0) */
    CI_INTDG_HEAP,
    /** @brief Memory containing untiled Image data */
    CI_IMAGE_HEAP,
    /** @brief Tile memory containing Image data (ctx 0) */
    CI_TILED_IMAGE_HEAP0,
    /** @brief Tile memory containing Image data (ctx 1) */
    CI_TILED_IMAGE_HEAP1,
    /** @brief Tile memory containing Image data (ctx 2)*/
    CI_TILED_IMAGE_HEAP2,
    /** @brief Tile memory containing Image data (ctx 3) */
    CI_TILED_IMAGE_HEAP3,

    /** @warning not a value - counts the number of heaps */
    CI_N_HEAPS
};

/** @brief Maximum number of MMU directories in the HW */
#define CI_MMU_N_DIR 4
/** @brief Maximum number of MMU requestors */
#define CI_MMU_N_REQ 8
#define CI_MMU_N_HEAPS CI_N_HEAPS
#define CI_MMU_HW_TILED_HEAPS 4

/**
 * @brief MMU HW configuration and information
 *
 * Should be populated before calling KRN_CI_MMUConfigure and not changed
 * afterwards
 */
typedef struct KRN_CI_MMU
{
    /**
     * @brief Each context directoy - apDirectory[0] can be used to know if 
     * the MMU is enabled or not
     */
    struct MMUDirectory *apDirectory[CI_MMU_N_DIR];
    /** @brief Mapping requestors to MMU directory entries */
    IMG_UINT8 aRequestors[CI_MMU_N_REQ];
    /** @brief To enable extended addressing for MMU that have more than 32b
     * physical addresses
     */
    IMG_BOOL8 bEnableExtAddress;

    /** @brief register handle */
    IMG_HANDLE hRegFelixMMU;

    /** @brief Tile size (256 = 256Bx16, 512 = 512Bx8) - if 512 write 1 into
     * MMU_CONTROL0:MMU_TILING_SCHEME otherwise writes 0
     */
    IMG_UINT32 uiTiledScheme;

    // heaps info
    IMG_UINT32 aHeapStart[CI_MMU_N_HEAPS];
    IMG_UINT32 aHeapSize[CI_MMU_N_HEAPS];
    IMG_SIZE uiAllocAtom;
} KRN_CI_MMU;

/**
 * @note The imager are not monitored because it is allowed to have 2 HW 
 * context using the same imager (but processing the frame differently)
 */
typedef struct KRN_CI_DRIVER
{
    struct KRN_CI_DRIVER_HANDLE sTalHandles;

    SYS_DEVICE *pDevice;

    /** @brief protects the workqueue between top/bottom halves of interrupt */
    SYS_SPINLOCK sWorkSpinlock;
    /**
     * @brief workqueue for interrupt handlers
     *
     * @note protected by sWorkSpinlock
     */
    sLinkedList_T sWorkqueue;

    /** @brief Active context protection lock */
#ifndef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_LOCK sActiveContextLock;
#else
	SYS_SPINLOCK sActiveContextLock;
#endif
    /**
     * @brief Current active context
     *
     * @note Make sure interrupts for the context are turned on AFTER this has
     * been populated and turned off BEFORE this is removed
     *
     * @note protected by sActiveContextLock
     */
    struct KRN_CI_PIPELINE *aActiveCapture[CI_N_CONTEXT];
    /**
     * @brief number of active captures in aActiveCapture
     *
     * @note protected by sActiveContextLock
     */
    IMG_UINT32 ui32NActiveCapture;

    /**
     * @brief Counts the missed frames per context
     *
     * @li initialised to gasket value when registering a new Pipeline in 
     * aActiveCapture
     *
     * @note only used in interrupt context - and when new Pipeline is 
     * registered
     *
     * @note protected by sActiveContextLock
     */
    IMG_UINT32 aLastGasketCount[CI_N_CONTEXT];
    /**
     * @brief Global data extraction point used
     *
     * @warning value NONE is invalid
     *
     * @note protected by sActiveContextLock
     */
    enum CI_INOUT_POINTS eCurrentDataExtraction;
    /**
     * @brief Context that sends information to the Encoder output line 
     * - if CI_N_CONTEXT it means none
     *
     * @warning only 1 can be enabled at a time!
     *
     * @note protected by sActiveContextLock
     */
    IMG_UINT8 ui8EncOutContext;
    /**
     * @brief Datagen that is currently active
     *
     * Register banks can potentially support several internal DG but they 
     * all use the same MMU requestor so cannot be enabled at the same time.
     * Therefore a HW with several internal DG is very unlikely (TRM even says
     * there is only 1).
     *
     * @note protected by sActiveContextLock
     */
    struct KRN_CI_DATAGEN *pActiveDatagen;

    /**
     * @brief Number of elements available in the HW queue per context
     */
    SYS_SEM aListQueue[CI_N_CONTEXT];

    /**
     * @brief Sharing the MMU directories between all CI_Pipeline that uses 
     * the same context
     */
    KRN_CI_MMU sMMU;

    CI_HWINFO sHWInfo;

    /**
     * @brief maximum timed waited to acquire a frame in ms (can be changed 
     * at insmod time)
     */
    IMG_UINT32 uiSemWait;

    /**
     * @brief Protect access to connection, linestore, gasket and gamma LUT 
     * info
     */
    SYS_LOCK sConnectionLock;
    /**
     * @brief Management of the DPF internal buffer (nb of available bytes) 
     * - protected by sConnectionLock
     *
     * @note The size of the DPF internal buffer should be reserved before 
     * the context is started.
     *
     * @see managed by KRN_CI_DriverAcquireDPFBuffer()
     */
    IMG_UINT32 ui32DPFInternalBuff;
    /**
     * @brief What connection is currently using a gasket
     * 
     * @note protected by sConnectionLock
     */
    struct KRN_CI_CONNECTION* apGasketUser[CI_N_IMAGERS];
    /**
     * @brief Linestore information
     *
     * @note protected by sConnectionLock
     */
    CI_LINESTORE sLinestoreStatus;
    /**
     * @brief list of connections (KRN_CI_CONNECTION*)
     *
     * @note protected by sConnectionLock
     */
    sLinkedList_T sList_connection;
    /**
     * @brief Current Gamma LUT used
     *
     * @note protected by sConnectionLock
     */
    CI_MODULE_GMA_LUT sGammaLUT;
} KRN_CI_DRIVER;

typedef struct KRN_CI_CONNECTION
{
    /**
     * @brief uses a bit more memory but allows a single locking of the driver
     * to get the info instead of locking when copy to user
     */
    CI_CONNECTION publicConnection;

    /**
     * @brief protect elements of the connection
     *
     * @li @ref KRN_CI_CONNECTION::sList_pipeline
     * @li @ref KRN_CI_CONNECTION::sList_iifdg
     * @li 
     */
    SYS_LOCK sLock;

    /**
     * @brief list of pipeline configuration from user-side 
     * (@ref KRN_CI_PIPELINE *) - protected by @ref KRN_CI_CONNECTION::sLock
     */
    sLinkedList_T sList_pipeline;

    /** 
     * @brief list of internal data generator object from user-side
     * (@ref KRN_CI_DATAGEN*) - protected by @ref KRN_CI_CONNECTION::sLock
     */
    sLinkedList_T sList_iifdg;

    /**
     * @brief shots are stored here until the user-space map them - protected
     * by @ref KRN_CI_CONNECTION::sLock
     *
     * Then they go to @ref KRN_CI_PIPELINE::sList_available 
     * (@ref KRN_CI_SHOT *)
     */
    sLinkedList_T sList_unmappedShots;
    /**
     * @brief buffers are stored here until the user-space map them
     * - protected by @ref KRN_CI_CONNECTION::sLock
     *
     * Then they go to @ref KRN_CI_PIPELINE::sList_availableBuffers 
     * (@ref KRN_CI_BUFFER *)
     */
    sLinkedList_T sList_unmappedBuffers;
    /**
     * @brief frames are stored here until the user-space map them - protected
     * by @ref KRN_CI_CONNECTION::sLock
     *
     * Then they go to @ref KRN_CI_DATAGEN::sList_available 
     * (@ref KRN_CI_DGBUFFER)
     */
    sLinkedList_T sList_unmappedDGBuffers;

    /**
     * @brief base ID for pipeline objects - protected by 
     * @ref KRN_CI_CONNECTION::sLock
     *
     * @ could be atomic
     */
    int ui32PipelineIDBase;
    /**
     * @brief base ID for IIF DG objects - protected by 
     * @ref KRN_CI_CONNECTION::sLock
     *
     * @ could be atomic
     */
    int ui32IIFDGIDBase;
    /** 
     * @brief mmap identifier
     *
     * @ could be atomic
     */
    int iMemMapBase;
    /** @ could be atomic */
    int iShotIdBase;
} KRN_CI_CONNECTION;

enum KRN_CI_SHOT_eSTATUS
{
    /**
     * @brief @ref KRN_CI_SHOT is ready to be given to HW for processing
     *
     * The status changes to @ref CI_SHOT_PENDING.
     */
    CI_SHOT_AVAILABLE = 0,
    /**
     * @brief @ref KRN_CI_SHOT is currently used by HW (or waiting to be used
     * by the HW).
     *
     * When HW finished processing the status changes to 
     * @ref CI_SHOT_PROCESSED.
     *
     * If capture failed the buffer is returned as @ref CI_SHOT_AVAILABLE
     */
    CI_SHOT_PENDING,
    /**
     * @brief @ref KRN_CI_SHOT has been processed by HW but not acquired by
     * user-space yet
     *
     * When user acquires it the status changes to @ref CI_SHOT_SENT.
     */
    CI_SHOT_PROCESSED,
    /**
     * @brief @ref KRN_CI_SHOT is acquired by user-space and cannot be used.
     *
     * When user releases it the @ref KRN_CI_SHOT is moved as
     * @ref CI_SHOT_AVAILABLE.
     */
    CI_SHOT_SENT,
};

/**
 * @par Adding a new HW module
 * In order to add a new HW to be configure you need to:
 * @li define a new CI_MODULE_MOD structure in ci_modules_structs.h and its
 * associated functions in ci_modules.h (if it needs special setup)
 * @li add this structure to @ref CI_PIPELINE
 * @li update the @ref CI_MODULE_UPDATE flags
 * @li update @ref INT_CI_PipelineUpdate() to take into account your new flag
 * and the module
 * @li add a new HW_CI_Load_MOD (and HW_CI_Reg_MOD if needed) into
 * ci_hwstruct.h
 * @li update @ref KRN_CI_PipelineUpdate() to use your new functions (be aware
 * that register access are limited)
 * @li update the unit tests CI_Config::update() to test the independant
 * update of your new structure
 */
typedef struct KRN_CI_PIPELINE
{
    /** @brief owner */
    KRN_CI_CONNECTION *pConnection;

    /**
     * @name configuration part of the pipeline
     * @{
     */

    /** @brief copy of the user-side pipeline configuration */
    struct CI_PIPELINE userPipeline;
    /** @brief ID given to user space */
    int ui32Identifier;

    /** @brief Hardware load structure to use for the next submitted buffer */
    char *pLoadStructStamp;

    CI_SHOT sSizes;
    CI_SHOT sTiledSize;

    /**
     * @}
     * @name capture management part of the pipeline
     * @{
     */

    /**
     * @brief secure access to the lists not used in interrupt handler
     *
     * @li @ref KRN_CI_PIPELINE::sList_available
     * @li @ref KRN_CI_PIPELINE::sList_sent
     * @li @ref KRN_CI_PIPELINE::sList_availableBuffers
     * @li @ref KRN_CI_PIPELINE::sList_pending
     * @li @ref KRN_CI_PIPELINE::sList_processed
     */
    SYS_LOCK sListLock;
    /**
     * @brief List of available shots to user-space (@ref KRN_CI_SHOT *)
     * - @ref CI_SHOT_AVAILABLE state
     *
     * @note Access is protected by @ref KRN_CI_PIPELINE::sListLock
     */
    sLinkedList_T sList_available;

    /**
     * @brief List of shots that the user-space acquired (@ref KRN_CI_SHOT *)
     * - @ref CI_SHOT_SENT state
     *
     * @note Access is protected by @ref KRN_CI_PIPELINE::sListLock
     */
    sLinkedList_T sList_sent;
#ifdef INFOTM_USE_SPIN_LOCK // added by linyun.xiong @2016-03-22
    SYS_SPINLOCK sProcessingSpinlock; ///< @brief secure access to sList_pending and sList_processed
#endif
    /**
     * @brief List of shots ready to be acquired by the user-space
     * (@ref KRN_CI_SHOT *) - @ref CI_SHOT_PROCESSED state
     *
     * @note Access is protected by @ref KRN_CI_PIPELINE::sListLock
     *
     * Number of elements should be in sync with
     * @ref KRN_CI_PIPELINE::sProcessedSem
     */
    sLinkedList_T sList_processed;

    /**
     * @brief To wait until a shot is available when requiring a new shot
     *
     * Should be in sync with number of elements in
     * @ref KRN_CI_PIPELINE::sList_processed
     */
    SYS_SEM sProcessedSem;

    /**
     * @brief List of shots submitted to the HW for capture 
     * (@ref KRN_CI_SHOT *) - @ref CI_SHOT_PENDING state
     *
     * @note Access is protected by @ref KRN_CI_PIPELINE::sListLock
     */
    sLinkedList_T sList_pending;


    /**
     * @brief Cumulative @ref CI_MODULE_UPDATE enum used at last update
     *
     * @note the driver sets it back to @ref CI_UPD_NONE after it had been
     * applied to at least 1 new frame
     */
    int eLastUpdated;

    /** 
     * @brief The capture is registered to the Driver and can modify registers
     * (i.e. it is started and ready to capture)
     */
    IMG_BOOL8 bStarted;

    struct MMUHeap *apHeap[CI_N_HEAPS];
    IMG_UINT32 uiTiledStride;
    IMG_UINT32 uiTiledAlignment;

    /**
     * @brief list of available buffers (@ref KRN_CI_BUFFER*) so that user can
     * choose which one to use for a shoot
     *
     * Once triggered the buffers are attached to a @ref KRN_CI_SHOT and
     * re-pushed once the Shot is released
     *
     * @note Access is protected by @ref KRN_CI_PIPELINE::sListLock
     */
    sLinkedList_T sList_availableBuffers;

    /**
     * @}
     * @name Deshading grid attributes
     * @}
     */

    /**
     * @brief The matrix stride (per channel) in bytes
     *
     * @note Register: LSH_GRID_STRIDE:LSH_GRID_STRIDE
     */
    IMG_SIZE uiLSHMatrixStride;
    /**
     * @brief Number of elements in the matrix line in units of 16 bytes
     * - related to ui16TileSize and the image's size
     *
     * @warning IN UNITS OF 16 BYTES
     *
     * @note Register: LSH_GRID_LINE_SIZE:LSH_GRID_LINE_SIZE
     */
    IMG_UINT16 ui16LSHLineSize;
    /**
     * @brief The size the matrix covers in pixels
     *
     * used to know if covering more than the single or multiple context size
     * limitation
     *
     */
    IMG_UINT16 ui16LSHPixelSize;

    SYS_MEM sDeshadingGrid;

    /**
     * @}
     * @name input defective pixels attributes
     * @{
     */

    /**
     * @note Register: DPF_READ_SIZE_CACHE:DPF_READ_MAP_SIZE
     */
    IMG_UINT32 uiInputMapSize;
    IMG_UINT32 uiWantedDPFBuffer;
    SYS_MEM sDPFReadMap;

    /**
     * @{
     * @name encoder statistics attributes
     * @{
     */

    /**
     * @brief size in bytes needed for a defective pixel buffer to be usable 
     * with current configuration
     */
    IMG_UINT32 uiENSSizeNeeded;

    /**
     * @}
     */

#ifdef INFOTM_ISP
    void* pPrevPipeline;
#endif //INFOTM_ISP

} KRN_CI_PIPELINE;

/** so that output buffers can be imported and chosen by ID */
typedef struct KRN_CI_BUFFER
{
    /**
     * @brief Buffer ID (used for mmap and also finding it when triggering/
     * deregistering)
     */
    int ID;
    /** @brief can be used by owner to apply a type to help search */
    int type;
    SYS_MEM sMemory;
    /** in the pipeline's buffer list */
    sCell_T sCell;
    IMG_BOOL bTiled;

    /** @brief pointer to the configuration it belongs to */
    KRN_CI_PIPELINE *pPipeline;

#ifdef INFOTM_ISP
    void* pDualBuffer;
#endif //INFOTM_ISP
} KRN_CI_BUFFER;

enum eKRN_CI_BUFFER_TYPE {
    /** @brief Encoder output */
    CI_BUFF_ENC = 1,
    /** @brief Display output */
    CI_BUFF_DISP,
    /** @brief HDR extraction */
    CI_BUFF_HDR_WR,
    /** @brief HDR insertion */
    CI_BUFF_HDR_RD,
    /** @brief RAW 2D extraction point (TIFF format) */
    CI_BUFF_RAW2D,
    /** @brief statistics - used only when mapping to user-space */
    CI_BUFF_STAT,
    /** 
     * @brief Defective pixels output - used only when mapping to user-space
     */
    CI_BUFF_DPF_WR,
    /**
     * @brief Encoder stats output
     */
    CI_BUFF_ENS_OUT,

};

enum PwrCtrl
{
    PWR_FORCE_ON = 0x1,
    PWR_AUTO = 0x2,
};


typedef struct KRN_CI_SHOT
{


    /**
     * @brief The hardware associated Context Pointers (the linked list 
     * element)
     */
    SYS_MEM hwLinkedList;
    /**
     * @brief The hardware associated Context Config (register configuration)
     */
    SYS_MEM hwLoadStructure;

    /*
     * the type of the buffer is only updated when mapped - when all are 
     * updated the shot is considered ready
     */

    /**
     * @brief The output image of the encoder pipeline - includes Y and CbCr
     */
    KRN_CI_BUFFER *phwEncoderOutput;
    /** @brief The output image of the display pipeline */
    KRN_CI_BUFFER *phwDisplayOutput;
    /** @brief The output image of the HDR extraction point */
    KRN_CI_BUFFER *phwHDRExtOutput;
    /** @brief The input image of the HDR insertion */
    KRN_CI_BUFFER *phwHDRInsertion;
    /** @brief The output image of the Raw 2D extraction point */
    KRN_CI_BUFFER *phwRaw2DExtOutput;

    /** @brief The hardware associated Save structure (statistics) */
    KRN_CI_BUFFER hwSave;
    /** @brief The output of the DPF if generated */
    KRN_CI_BUFFER sDPFWrite;
    /** @brief The output of the ENS if generated */
    KRN_CI_BUFFER sENSOutput;

    /** @brief Identifier in user space */
    int iIdentifier;

    /** @brief The current status of the Shot */
    enum KRN_CI_SHOT_eSTATUS eStatus;

    sCell_T sListCell;

    /** @brief pointer to the configuration it belongs to */
    KRN_CI_PIPELINE *pPipeline;

    /**
     * @brief user-side part of the structure - memory is mapped to user 
     * space! 
     */
    CI_SHOT userShot;
} KRN_CI_SHOT;

typedef struct KRN_CI_DATAGEN
{
    /** @brief owner */
    KRN_CI_CONNECTION *pConnection;
    /** cell in the iifdg list in connection object */
    sCell_T sCell;

    int ui32Identifier;
    /** copie at acquire time */
    CI_DATAGEN userDG;
    IMG_BOOL8 bCapturing;

    /**
     * @brief Needs reset before submitting next frame
     *
     * If HW sends DG_INT_ERROR interrupt the HW needs a reset to be able to
     * capture the next frame
     *
     * This is written during interrupt context, DG start and DG trigger.
     *
     * If written cannot push frames until a reset
     */
    IMG_BOOL8 bNeedsReset;

    /**
     * @brief virtual memory heap when using MMU
     */
    struct MMUHeap *pHeap;

    /**
     * @brief secure access to the lists used
     *
     * @li @ref KRN_CI_DATAGEN::sList_busy
     * @li @ref KRN_CI_DATAGEN::sList_processed
     * @li @ref KRN_CI_DATAGEN::sList_available 
     * @li @ref KRN_CI_DATAGEN::sList_pending
     *
     * @note we need to protect lists when mapping and mapping may need
     * to allocate Device MMU pages.
     */
    SYS_LOCK slistLock;

    /**
     * @brief frames available to give for convertion to user-space 
     * (@ref KRN_CI_DGBUFFER)
     *
     * @note protected by @ref KRN_CI_DATAGEN::slistLock
     */
    sLinkedList_T sList_available;

    /**
     * @brief frames given to user-space for convertion 
     * (@ref KRN_CI_DGBUFFER)
     *
     * @note protected by @ref KRN_CI_DATAGEN::slistLock
     */
    sLinkedList_T sList_pending;

    /**
     * @brief frames currently submitted to HW (@ref KRN_CI_DGBUFFER)
     *
     * @note protected by @ref KRN_CI_DATAGEN::slistLock
     */
    sLinkedList_T sList_busy;

    /**
     * @brief Once processed by HW waits here until pushed back to available
     *
     * Done so that @ref KRN_CI_DATAGEN::sList_available does not have to be 
     * protected by @ref KRN_CI_DATAGEN::slistLock
     *
     * @note protected by @ref KRN_CI_DATAGEN::slistLock
     */
    sLinkedList_T sList_processed;
} KRN_CI_DATAGEN;

typedef struct KRN_CI_DGBUFFER
{
    /**
     * @brief Buffer ID (used for mmap and also finding it when triggering/
     * deregistering)
     */
    int ID;
    SYS_MEM sMemory;
    /** in the datagen buffer list */
    sCell_T sCell;

    IMG_UINT32 ui32Stride;
    IMG_UINT32 ui16Width;
    IMG_UINT32 ui16Height;
    IMG_UINT16 ui16HorizontalBlanking;
    IMG_UINT16 ui16VerticalBlanking;

    /** @brief pointer to the configuration it belongs to */
    KRN_CI_DATAGEN *pDatagen;
} KRN_CI_DGBUFFER;

/** @brief Driver singleton instance */
extern KRN_CI_DRIVER *g_psCIDriver;

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * End of the KRN_CI_API functions group
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_KERNEL_STRUCTS_H_ */
