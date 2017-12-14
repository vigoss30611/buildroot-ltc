/**
*******************************************************************************
 @file ci_kernel.h

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
#ifndef CI_KERNEL_H_
#define CI_KERNEL_H_

#include <img_types.h>
#include <img_defs.h>

#include "ci/ci_api_structs.h"
#include "ci_kernel/ci_kernel_structs.h"
#include "ci_internal/sys_mem.h"
#include "ci_internal/sys_parallel.h"
#include "felix_hw_info.h"  // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

struct CI_ALLOC_PARAM;  // ci_ioctrl.h
struct CI_DG_FRAMETRIG;  // ci_ioctrl.h
struct CI_DG_FRAMEINFO;  // ci_ioctrl.h

/**
 * @defgroup KRN_CI_API Capture Interface: kernel-side driver documentation
 * @brief Documentation of the internal elements of the V2500 driver.
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the KRN_CI_API documentation module
 *---------------------------------------------------------------------------*/
#ifdef INFOTM_ISP
extern uint ui32ClkRate;
#endif

#ifdef FELIX_FAKE
#define CI_SYNC_FIRST 1
enum CI_PDUMP_SYNC
{
    CI_SYNC_MEM_ALLOC = CI_SYNC_FIRST,
    CI_SYNC_MEM_FREE,
    /** @brief Used when mapping to the HW MMU */
    CI_SYNC_MEM_MAP,
    /** @brief Used when unmapping from the HW MMU */
    CI_SYNC_MEM_UNMAP,
    /**
     * @brief When writing Device Address to register a sync is needed between
     * CTX and SYSMEM as only 1 pdump script can get the register write
     */
    CI_SYNC_MEM_ADDRESS,
    /**
     * @brief used to mark the end - cannot be the number of elements because
     * SYNC id in pdump must start from 1
     */
    CI_SYNC_NOT_USED
};

#else

#ifndef INFOTM_ISP
// comment this to disable time printing some timing as warnings
// use getnstimeofday(struct timespec *) to get a and b
#define W_TIME_PRINT(a, b, name) \
{ \
    CI_WARNING(name" timed %ldmin %.2lds %.6ldus\n", \
        (b.tv_sec - a.tv_sec) / 60, \
        (b.tv_sec - a.tv_sec) % 60, \
        ((b.tv_nsec - a.tv_nsec) / 1000L) % 1000000); \
}

#endif //INFOTM_ISP
#endif //FELIX_FAKE

/**
 * @brief Default register poll time between poll tries (in clock cycles)
 *
 * Fake interface:
 *
 * TAL is using KRN_hibernate, that is using OSA_ThreadSleep().
 *
 * In linux kernel "clock cycle" is 1ms.
 *
 * @see CI_REGPOLL_TRIES
 */
#define CI_REGPOLL_TIMEOUT 20

/**
 * @brief Number of tries for a poll (tries before waiting)
 * @see CI_REGPOLL_TIMEOUT
 */
#define CI_REGPOLL_TRIES 10

/**
 * @brief Maximum time a wait can be when doing a Poll operation
 *
 * Value chosen by HW team
 *
 * @note limit given by HW testbench application is 1,000,000 cycles - a value
 * greater than that will be considered a lockup, use tries to match needed
 * time.
 */
#define CI_REGPOLL_MAXTIME (5000)

/**
 * @brief Default Gamma Lookup curve to use
 *
 * @see KRN_CI_DriverDefaultsGammaLUT() implementation for legitimate values
 */

#define CI_DEF_GMACURVE 1

#define MMU_RO 0x4
#define MMU_WO 0x2
#define MMU_RW 0x0

/**
 * @brief Minimum supported tiling stride for 256x16 scheme (x2 if tiling
 * scheme is 512x8)
 */
#define MMU_MIN_TILING_STR (1 << (9))
/**
 * @brief Maximum supported tiling stride for 256x16 scheme
 * (x2 if tiling scheme is 512x8)
 *
 * @note HW supports very big tiling stride but 
 * @ref CI_ALLOC_GetMaxTilingStride() is used to limit what the maximum
 * is to avoid large bands of virtual addresses to be allocated for alignment
 */
#define MMU_MAX_TILING_STR (1 << (9+9))

/**
 * @brief Used to destroy CI_Shots from lists using the visitor function 
 * (cells are contained in the shot)
 */
IMG_BOOL8 ListDestroy_KRN_CI_Shot(void* elem, void *param);

/**
 * @brief Used to destroy CI_Buffer from lists using the visitor function
 * (cells are contained in the Buffer)
 */
IMG_BOOL8 ListDestroy_KRN_CI_Buffer(void *elem, void *param);

/** 
 * @brief Search a CI_Buffer with a specific ID
 */
IMG_BOOL8 ListSearch_KRN_CI_Buffer(void *elem, void *lookingFor);
#ifdef INFOTM_ISP
IMG_BOOL8 ListSearch_KRN_CI_Buffer_Type(void *elem, void *lookingForType);
#endif //INFOTM_ISP

/**
 * @brief Map a buffer to the device MMU
 *
 * @note HDR insertion buffers are mapped as RO while other as WO
 */
IMG_BOOL8 List_MapBuffer(void *listelem, void *directory);

IMG_BOOL8 ListDestroy_KRN_CI_DGBuffer(void *elem, void *param);

/**
 * @brief Search a DGBuffer with a specific ID
 */
IMG_BOOL8 List_FindDGBuffer(void *elem, void *param);

/**
 * @name KRN_CI_Driver object
 * @brief Internal driver management
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the KRN_CI_DRIVER functions group
 *---------------------------------------------------------------------------*/

/**
 * @param pKrnDriver driver to initialise
 * @param ui8MMUEnabled 0 configures MMU in bypass (1:1 virtual to physical),
 * 1 configures MMU in 32b physical, >1 configures MMU extended address range
 * @param ui32TilingScheme 256 = 256Bx16 or 512 = 512Bx8 tiling scheme 
 * (applied only if tiling is used) - no effect if ui8MMUEnabled is false
 * @param ui32GammaCurveDefault 0 - Gamma curve to use as default, see
 * available curves in @ref KRN_CI_DriverDefaultsGammaLUT()
 * @param pSysDevice to use
 */
IMG_RESULT KRN_CI_DriverCreate(KRN_CI_DRIVER *pKrnDriver,
    IMG_UINT8 ui8MMUEnabled, IMG_UINT32 ui32TilingScheme,
    IMG_UINT32 ui32GammaCurveDefault, SYS_DEVICE *pSysDevice);

/**
 * @brief Setups the default gamma curve
 *
 * @param[out] pGamma structure to populate
 * @param[in] uiCurve curve to use - see implementation for legitimate values
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_INVALID_PARAMETERS if the give uiCurve is not available
 */
IMG_RESULT KRN_CI_DriverDefaultsGammaLUT(CI_MODULE_GMA_LUT *pGamma,
    IMG_UINT32 uiCurve);

void KRN_CI_DriverResetHW(void);
IMG_RESULT KRN_CI_DriverDestroy(KRN_CI_DRIVER *pKrnDriver);

IMG_RESULT KRN_CI_DriverAcquireContext(KRN_CI_PIPELINE *pPipeline);
IMG_RESULT KRN_CI_DriverReleaseContext(KRN_CI_PIPELINE *pPipeline);

/**
 * @brief Checks that the deshading grid covers a size the context can be
 * configured for
 *
 * Uses data from @ref KRN_CI_PIPELINE::userPipeline
 * @ref CI_PIPELINE::sDeshading
 *
 * @warning Uses @ref KRN_CI_PIPELINE::ui16LSHPixelSize
 *
 * @warning If the context is not yet started it assumes the maximum size
 * is for single context - check the size of LSH after acquiring the context
 *
 * @return IMG_TRUE if the context can run with this size
 * @return IMG_FALSE if the context cannot run with this size
 */
IMG_BOOL8 KRN_CI_DriverCheckDeshading(KRN_CI_PIPELINE *pPipeline);

IMG_RESULT KRN_CI_DriverAcquireGasket(IMG_UINT32 uiGasket,
    KRN_CI_CONNECTION *pConnection);
IMG_BOOL8 KRN_CI_DriverCheckGasket(IMG_UINT32 uiGasket,
    KRN_CI_CONNECTION *pConnection);
void KRN_CI_DriverReleaseGasket(IMG_UINT32 uiGasket);

/**
 * @brief Acquire HW resource for the internal data-generator
 *
 * A resource is considered not available if the gasket the internal datagen
 * is supposed to replace is already acquired
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE if the resource is not available
 */
IMG_RESULT KRN_CI_DriverAcquireDatagen(KRN_CI_DATAGEN *pDatagen);
IMG_RESULT KRN_CI_DriverReleaseDatagen(KRN_CI_DATAGEN *pDatagen);

/**
 * @brief Verifies that the wanted DE point is availabled.
 *
 * @warning Do not call if a Capture's lock is locked!
 *
 * TRM: The Data Extraction point cannot be medified whilst a frame is
 * extracted
 *
 * @param eWanted verified Data Extraction point
 *
 * DE point is available if:
 * @li no active capture have DE enabled
 * @li the current DE point is the same than the wanted one
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_OPERATION_PROHIBITED if cannot change DE
 */
IMG_RESULT KRN_CI_DriverEnableDataExtraction(enum CI_INOUT_POINTS eWanted);

/**
 * @brief Change the Gamma Look Up Table currently used
 *
 * @warning Do not call if a Capture's lock is locked!
 *
 * The GMA LUT cannot be changed if a context is running.
 *
 * @param pNewGMA new Gamma LUT table - on success the global driver GMA LUT 
 * is updated as well
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if a context is running
 *
 * @see Delegates to HW_CI_Reg_GMA() to write the registers
 */
IMG_RESULT KRN_CI_DriverChangeGammaLUT(const CI_MODULE_GMA_LUT *pNewGMA);

IMG_RESULT KRN_CI_DriverAcquireDPFBuffer(IMG_UINT32 prev, IMG_UINT32 wanted);

/**
 * @}
 * @name KRN_CI_Pipeline object - configuration functions
 * @brief Configuration part of the Pipeline
 * @{
 */

/**
 * @brief Verify that a user-mode configuration is possible with this HW 
 * version
 *
 * @return IMG_TRUE if possible, IMG_FALSE otherwise
 */
IMG_BOOL8 KRN_CI_PipelineVerify(const CI_PIPELINE *pPipeline,
    const CI_HWINFO *pHWInfo);

/**
 * @param pPipeline Allocated Pipeline object with its userPipeline already 
 * copied from user-space - asserts it is not NULL
 * @param pConnection
 */
IMG_RESULT KRN_CI_PipelineInit(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_CONNECTION *pConnection);
	
/**
 * @brief Destroys the pipeline object (and free it)
 */
IMG_RESULT KRN_CI_PipelineDestroy(KRN_CI_PIPELINE *pPipeline);
/**
 * @brief Update the device memory with the current publicCapture information
 *
 * @param pPipeline
 * @param bRegAccess is true when updating modules that need access to 
 * registers is allowed
 * @param bASAP apply the configuration As Soon As Possible (i.e. on already
 * submitted configurations as well)
 * @note A configuration can only be applied ASAP if there is no register 
 * access
 *
 * @note Currently not efficient: update the whole structure instead of
 * focusing on maybe the only parts that need update
 */
IMG_RESULT KRN_CI_PipelineUpdate(KRN_CI_PIPELINE *pPipeline,
    IMG_BOOL8 bRegAccess, IMG_BOOL8 bASAP);

/**
 * @brief first part of the addition of a frame buffer
 *
 * Buffer is added to the connection's unmap list.
 * When the user space accessed all the shared buffers 
 * @ref KRN_CI_PipelineShotMapped() will be called, making the Shot available
 *
 * @param pPipeline
 * @param[in,out] pShot CI_SHOT part is supposed to have all needed 
 * information about ion FDs and tiling options
 */
IMG_RESULT KRN_CI_PipelineAddShot(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_SHOT *pShot);

/**
 * @brief Will allocate or Import buffer according to given param
 *
 * if pParam->uiSize is 0 will allocate/import the size computed
 * otherwise will allocate/import the provided one
 *
 * @return new buffer or NULL if given size is too small
 */
KRN_CI_BUFFER* KRN_CI_PipelineCreateBuffer(KRN_CI_PIPELINE *pPipeline,
    struct CI_ALLOC_PARAM *pParam, IMG_RESULT *ret);

IMG_RESULT KRN_CI_PipelineAddBuffer(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_BUFFER *pBuffer);

/**
 * @brief Called once every elements of the CI_SHOT has been mapped to 
 * user-space
 */
IMG_RESULT KRN_CI_PipelineShotMapped(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_SHOT *pShot);

/**
 * @brief Called once the CI_BUFFER has been mapped to user-space
 */
IMG_RESULT KRN_CI_PipelineBufferMapped(KRN_CI_PIPELINE *pPipeline,
    KRN_CI_BUFFER *pBuffer);

/**
 * @brief deregister a buffer from the pipeline
 *
 * Buffer is removed from sList_availableBuffers
 *
 * @param pPipeline
 * @param[in] id memMapId of the buffer to remove
 */
IMG_RESULT KRN_CI_PipelineDeregisterBuffer(KRN_CI_PIPELINE *pPipeline,
    IMG_UINT32 id);

/**
 * @}
 * @name KRN_CI_Pipeline object - capture functions
 * @brief The capture management part of the Pipeline
 * @{
 */

/**
 * @note may sleep if bBlocking == IMG_TRUE
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED the capture is not started
 * @return IMG_ERROR_INTERRUPTED if bBlocking == IMG_TRUE and there is no 
 * room in the HW list (i.e. cannot get from the semaphore)
 */
IMG_RESULT KRN_CI_PipelineTriggerShoot(KRN_CI_PIPELINE *pPipeline,
    IMG_BOOL8 bBlocking, CI_BUFFID *pBuffId);

/**
 * @brief Starts the capture
 *
 * In that order:
 * @li if using DE point, try to enable the needed point
 * @li try to acquire the SW context
 * @li checks the size of the LSH grid
 * @li update the configuration
 * @li try to acquire the needed DPF internal buffer
 * @li map the element of available and sent list to the MMU
 * @li map the availableBuffer elements to the MMU
 * @li map the LSH grid to the MMU
 * @li map the DPF read map to the MMU
 * @li set the tiling stride
 * @li flush the MMU cache
 * @li start the HW
 */
IMG_RESULT KRN_CI_PipelineStartCapture(KRN_CI_PIPELINE *pPipeline);

/**
 * @brief Stops the capture
 *
 * In that order:
 * @li stops the HW
 * @li unmap all memory from available and sent list
 * @li clear all elements from pending list back to available list
 * (and unmap from MMU)
 * @li clear all elements from processed list back to available list
 * (and unmap from the MMU)
 * @li unmap all imported buffer from avaiableBuffer list from MMU
 * @li unmap the DPF read map from MMU
 * @li unmap the LSH grid from MMU
 * @li release the DPF internal buffer
 * @li release the SW context
 */
IMG_RESULT KRN_CI_PipelineStopCapture(KRN_CI_PIPELINE *pPipeline);
/**
 * @return IMG_SUCCESS
 * @return IMG_ERROR_NOT_SUPPORTED if waiting on the sProcessedSem failed 
 * or that the semaphore was acquired but not buffer was found
 */
IMG_RESULT KRN_CI_PipelineAcquireShot(KRN_CI_PIPELINE *pPipeline,
    IMG_BOOL8 bBlocking, KRN_CI_SHOT **ppShot);

IMG_RESULT KRN_CI_PipelineRelaseShot(KRN_CI_PIPELINE *pPipeline, int iShotId);

/**
 * @brief This is called when a frame was generated by the HW by the 
 * interrupt handler
 *
 * If a frame was completed successfully it is popped from the pending list
 * and added to the filled list.
 * The callback defined by the user is then called - if not NULL.
 *
 * If a frame error was generated the frame is popped from the pending 
 * list and pushed back into the available list.
 *
 * @param pPipeline the context that generated the interrupt
 *
 * @return the shot that was captured (that is in the processed list)
 * @return NULL if the pending list was empty - the execution is most 
 * likely corrupted if that occurs
 */
KRN_CI_SHOT* KRN_CI_PipelineShotCompleted(KRN_CI_PIPELINE *pPipeline);

/**
 * @}
 * @name KRN_CI_Shot object
 * @brief Kernel shot management
 * @{
 */

KRN_CI_SHOT * KRN_CI_ShotCreate(IMG_RESULT *ret);

/**
 * @brief Allocate internal buffer for a Shot from the configuration in the
 * Pipeline
 *
 * @param pShot asserted not NULL
 * @param pPipeline asserted not NULL
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_ALREADY_INITIALISED if pShot already has a Pipeline
 */
IMG_RESULT KRN_CI_ShotConfigure(KRN_CI_SHOT *pShot,
    KRN_CI_PIPELINE *pPipeline);

IMG_RESULT KRN_CI_ShotMapMemory(KRN_CI_SHOT *pShot,
    struct MMUDirectory *pDirectory);

IMG_RESULT KRN_CI_ShotUnmapMemory(KRN_CI_SHOT *pShot);

/**
 * @warning assumes the shot is not attached to any list (or the list will 
 * become corrupted)
 */
IMG_RESULT KRN_CI_ShotDestroy(KRN_CI_SHOT *pShot);
/**
 * @brief Clean the output memory to avoid remnant from previous captures
 */
IMG_RESULT KRN_CI_ShotClean(KRN_CI_SHOT *pShot);

/**
 * @brief save the current pipeline's configuration to the user shot
 */
IMG_RESULT KRN_CI_ShotGetStatsConfig(KRN_CI_SHOT *pShot);

/**
 * @brief Create a dynamic buffer
 *
 * @param[in] ui32Size needed size (system alignment APPLIED beforehand)
 * @param[in] pHeap virtual heap to use (DATA, IMAGE, TILED_IMAGE)
 * @param[in] buffId if != 0 use import with this ID, otherwise allocates
 * @param[in] uiVirtualAlignment if != 0 used to align start virtual address
 * (has to be != 0 for tiled buffers)
 * @param[out] ret
 */
#ifdef INFOTM_ISP
KRN_CI_BUFFER* KRN_CI_BufferCreate(IMG_UINT32 ui32Size, struct MMUHeap *pHeap,
    IMG_UINT32 uiVirtualAlignment, IMG_UINT32 buffId, IMG_RESULT *ret, char* pszName);
#else
KRN_CI_BUFFER* KRN_CI_BufferCreate(IMG_UINT32 ui32Size, struct MMUHeap *pHeap,
    IMG_UINT32 uiVirtualAlignment, IMG_UINT32 buffId, IMG_RESULT *ret);
#endif //INFOTM_ISP

/**
 * @note if pPipeline->uiTiledStride is != 0 it will be used to replace the
 * tiled stride of a buffer!
 *
 * @param[out] pUserShot
 * @param[in] pPipeline
 * @param bScaleOutput if on will take the scaled output to compute the
 * sizes not the maximum output of the pipeline
 */
IMG_RESULT KRN_CI_BufferFromConfig(CI_SHOT *pUserShot,
    const KRN_CI_PIPELINE *pPipeline, IMG_BOOL8 bScaleOutput);

/** needs to be removed from list before that */
void KRN_CI_BufferDestroy(KRN_CI_BUFFER *pBuff);

/**
 * @}
 * @name KRN_CI_Datagen object
 * @brief Internal Datagenerator functions
 * @{
 */

/**
 * @brief Initialises the object and adds it to the connection's list
 */
IMG_RESULT KRN_CI_DatagenInit(KRN_CI_DATAGEN *pDG,
    KRN_CI_CONNECTION *pConnection);

/**
 * @brief Cleans the object and removes it from the connection's list
 *
 * @note the Datagen is assumed to be stopped
 *
 * @return IMG_SUCCESS
 * @see delegates to IMGMMU_HeapCreate(), SYS_SpinlockInit(), SYS_LockInit(),
 * List_init() and List_pushBack() - which are unlikely to fail
 */
IMG_RESULT KRN_CI_DatagenClear(KRN_CI_DATAGEN *pDG);

/**
 * @brief Allocate a frame for the internal data generator
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_MALLOC_FAILED if internal structure allocation failed
 * @see delegates to SYS_MemAlloc()
 */
IMG_RESULT KRN_CI_DatagenAllocateFrame(KRN_CI_DATAGEN *pDG,
    struct CI_DG_FRAMEINFO *pFrameParam);

IMG_RESULT KRN_CI_DatagenAcquireFrame(KRN_CI_DATAGEN *pDG,
    struct CI_DG_FRAMEINFO *pFrameParam);

/**
 * @note assumes the buffer has already been detached from its list
 */
IMG_RESULT KRN_CI_DatagenFreeFrame(KRN_CI_DGBUFFER *pDGBuffer);

IMG_RESULT KRN_CI_DatagenTriggerFrame(KRN_CI_DATAGEN *pDG,
    struct CI_DG_FRAMETRIG *pFrameInfo);

IMG_RESULT KRN_CI_DatagenReleaseFrame(KRN_CI_DATAGEN *pDG,
    IMG_UINT32 ui32FrameID);

/**
 * @brief requieres HW for capture from driver and starts it
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_UNEXPECTED_STATE if datagen already started
 * @see Delegates to KRN_CI_DriverAcquireDatagen() and HW_CI_DatagenStart()
 */
IMG_RESULT KRN_CI_DatagenStart(KRN_CI_DATAGEN *pDG,
    const CI_DATAGEN *pDGConfig);

/**
 * @brief stops HW and releases it in the driver
 *
 * @note calls KRN_CI_DriverReleaseDatagen() but does propagate its result
 *
 * @return IMG_SUCCESS (returns directly if already stopped)
 */
IMG_RESULT KRN_CI_DatagenStop(KRN_CI_DATAGEN *pDG);

/**
 * @}
 * @name KRN_CI_Connection object
 * @brief Connection to the kernel management
 * @{
 */

IMG_RESULT KRN_CI_ConnectionCreate(KRN_CI_CONNECTION **pConnection);
IMG_RESULT KRN_CI_ConnectionDestroy(KRN_CI_CONNECTION *pConnection);

/**
 * @}
 * @name KRN_CI_MMU object
 * @brief Configure the MMU HW
 * @{
 */

struct MMUDirectory* KRN_CI_MMUDirCreate(KRN_CI_DRIVER *pDriver,
    IMG_RESULT *ret);

void KRN_CI_MMUDirDestroy(struct MMUDirectory *pMMUDir);

/**
 * @brief Configure the MMU HW
 *
 * @param pMMU - asserts it is not NULL
 * @param ui8NDir number of directories to use
 * @param ui8NRequestor number of requestors to the MMU (fixed by HW design)
 * @note each requestor is mapped to a directory
 * @param bBypass to know if the MMU should be in bypass (i.e. direct mapping
 * - no virtual memory)
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT KRN_CI_MMUConfigure(KRN_CI_MMU *pMMU, IMG_UINT8 ui8NDir,
    IMG_UINT8 ui8NRequestor, IMG_BOOL8 bBypass);

/**
 * @brief Flush page table cache or invalidate directory page
 *
 * @param pMMU - asserts it is not NULL
 * @param uiDirectory index to flush
 * @param bFlushAll if true flush both Directory and Page Table cache, if
 * false only Page Table cache
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT KRN_CI_MMUFlushCache(KRN_CI_MMU *pMMU, IMG_UINT8 uiDirectory,
    IMG_BOOL8 bFlushAll);

/**
 * @brief The MMU does not process more memory request if paused, or resume
 * processing them when unpaused
 *
 * @param pMMU - asserts it is not NULL
 * @param bPause
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT KRN_CI_MMUPause(KRN_CI_MMU *pMMU, IMG_BOOL8 bPause);

/**
 * @brief The MMU does not translate requested addresses using page tables 
 * when bypassed
 *
 * @note Uses KRN_CI_MMU::bEnableExtAddress to enable extended address range
 *
 * @param pMMU - asserts it is not NULL
 * @param bBypass
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT KRN_CI_MMUBypass(KRN_CI_MMU *pMMU, IMG_BOOL8 bBypass);

/**
 * @brief Enable Tiling configuration 0 with a given tiling stride
 *
 * @warning Do not change this setting when some tiled memory is mapped.
 *
 * @note Does not the pause the MMU prior to change the settings
 *
 * @param pMMU - asserts it is not NULL
 * @param bEnable
 * @param uiTileStride tiling stride the system should use (>= 512)
 * @param ui32TilingHeap MMU heap used
 *
 * The Tiling stride and MMU heap are system wide setup that are chosen when
 * configuring the MMU HW.
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT KRN_CI_MMUSetTiling(KRN_CI_MMU *pMMU, IMG_BOOL8 bEnable,
    unsigned int uiTileStride, IMG_UINT32 ui32TilingHeap);

/**
 * @brief Computes the virtual address alignment needed according to a 
 * tiling stride
 *
 * HW needs the 1st address tiled memory to be alignment to a certain
 * multiple to avoid going backward when accessing it.
 *
 * It is defined in MMU spec as 1<<(x_tile_stride+8+5+s), x_tile_stride 
 * being the register value and s being 0 for 256x16 scheme and 1 for 
 * 512x8 scheme.
 */
IMG_UINT32 KRN_CI_MMUTilingAlignment(IMG_UINT32 ui32TilingStride);

/**
 * @brief Get the current size in bits to be used in MMU entries (using the
 * enable extended address register to know if extended addresses are 
 * supported).
 *
 * @param pMMU - asserts if is not NULL
 *
 * @return size in bits (32 + extended addresses)
 */
IMG_UINT32 KRN_CI_MMUSize(KRN_CI_MMU *pMMU);

#ifdef INFOTM_ENABLE_ISP_DEBUG
//IMG_RESULT KRN_CI_InfotmDebugRegisterPrint(IMG_CONST FieldDefnListEl *pField, IMG_HANDLE hTalRegBankHandle, IMG_BOOL bDumpFields, IMG_BOOL bIsMemstruct);
//IMG_RESULT KRN_CI_InfotmDebugRegistersDump(FieldDefnListEl *aSortedBlock, IMG_SIZE nElem, IMG_HANDLE hTalRegBankHandle, IMG_BOOL bDumpFields, IMG_BOOL bIsMemstruct);
IMG_RESULT KRN_CI_InfotmDebugDumpRegs(IMG_UINT32 ui32ContextNb, IMG_BOOL bDumpFields);
#endif //INFOTM_ENABLE_ISP_DEBUG

/**
 * @}
 * @name LOG_CI functions
 * Functions to log actions in user or kernel mode
 * @{
 */

/**
 * Implemented in ci_init_km.c when using real driver
 * 
 * Implemented in ci_debug.c when using fake driver
 */
extern IMG_UINT ciLogLevel;

#define CI_LOG_LEVEL_DBG 4  // extra (all + debug messages)
#define CI_LOG_LEVEL_ALL 3  // all (but not debug)
#define CI_LOG_LEVEL_ERRORS 2  // no INFO
#define CI_LOG_LEVEL_FATALS 1  // no WARNING or INFO
#define CI_LOG_LEVEL_QUIET 0  // no FATAL, WARNING or INFO

void LOG_CI_Fatal(const char *function, IMG_UINT32 line,
    const char* format, ...);
void LOG_CI_Warning(const char *function, IMG_UINT32 line,
    const char* format, ...);
void LOG_CI_Info(const char *function, IMG_UINT32 line,
    const char* format, ...);
void LOG_CI_Debug(const char *function, IMG_UINT32 line,
    const char* format, ...);
int LOG_CI_LogLevel(void);  // log level CI was built with
void LOG_CI_PdumpComment(const char *function, IMG_HANDLE talHandle,
    const char* message);

#undef CI_FATAL
// function and line are used
#define CI_FATAL(...) LOG_CI_Fatal(__FUNCTION__, __LINE__, __VA_ARGS__)
#undef CI_WARNING
// line is not used
#define CI_WARNING(...) LOG_CI_Warning(__FUNCTION__, __LINE__, __VA_ARGS__)
#undef CI_INFO
// neither function nor line are used
#define CI_INFO(...) LOG_CI_Info(__FUNCTION__, __LINE__, __VA_ARGS__)
#undef CI_DEBUG
// line is not used
#define CI_DEBUG(...) LOG_CI_Debug(__FUNCTION__, __LINE__, __VA_ARGS__)

#ifdef FELIX_FAKE
#define CI_PDUMP_COMMENT(talHandle, message) \
    LOG_CI_PdumpComment(__FUNCTION__, talHandle, message);
	
void getnstimeofday(struct timespec *ts);
#else
#define CI_PDUMP_COMMENT(talHandle, message)
#endif

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the LOG_CI functions group
 *---------------------------------------------------------------------------*/

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the KRN_CI_API documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_KERNEL_H_ */
