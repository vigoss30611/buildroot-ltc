/**
*******************************************************************************
 @file ci_ioctrl.h

 @brief Declaration of the IOCTL enums and structures

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
#ifndef CI_IOCTRL_H_
#define CI_IOCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CI_IOCTL Input/Output Controls (IOCTL)
 * @ingroup INT_USR
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the CI_IOCTL documentation module
 *---------------------------------------------------------------------------*/

/**
 * @name IOCTLs structures
 * @{
 */

#include <linux/ioctl.h>
#include <stddef.h>

#include <img_errors.h>
#include <ci/ci_api_structs.h> /* needed to know the size of structures */

#ifndef __user /* when building fake interface __user is not defined */
#define __user
#endif

/**
 * @brief Map IDs per internal buffer (statistics, dpf output map, ens output 
 * are part of the Shot)
 */
enum CI_POOL_MAP_IDS {
    /**
     * @brief Map id of statistics buffer
     */
    CI_POOL_MAP_ID_STAT = 0,
    /**
     * @brief Map id of DPF buffer
     */
    CI_POOL_MAP_ID_DPF,
    /**
     * @brief Map id of ENS buffer
     */
    CI_POOL_MAP_ID_ENS,
    /** @brief Map id of the HW load structure */
    CI_POOL_MAP_ID_LOAD,

    /**
     * @brief Not a value - number of all ids
     */
    CI_POOL_MAP_ID_COUNT
};

/**
 * @brief Allocator IDs per importable buffer
 */
enum CI_ALLOC_BUFF {
    CI_ALLOC_ENC = 0,
    CI_ALLOC_DISP,
    CI_ALLOC_HDREXT,
    CI_ALLOC_RAW2D,
    CI_ALLOC_HDRINS,
    CI_ALLOC_LSHMAT,
};

/**
 * @brief Configuration IOCTL parameter (used for add a pool of buffers)
 *
 * @see CI_IOCTL_PIPE_ADD
 */
struct CI_POOL_PARAM {
    /**
     * @brief [in] Pipeline's ID
     */
    IMG_UINT32 configId;
    /**
     * @brief [out] all the buffer mmap offsets to use
     */
    IMG_UINT32 aMemMapIds[CI_POOL_MAP_ID_COUNT];
    /**
     * @brief [out] shot object identifier
     */
    IMG_INT32 iShotId;
    /**
     * @brief [out] information about the allocated buffer
     */
    CI_SHOT sample;
};

/**
 * @brief Single buffer import, allocate and de-register IOCTL parameter
 *
 * @see CI_IOCTL_CREATE_BUFF and CI_IOCTL_DEREGISTER_BUFF
 */
struct CI_ALLOC_PARAM {
    /**
     * @brief [in] Pipeline's ID
     */
    IMG_UINT32 configId;
    /**
     * @brief [in] buffer file descriptor when importing
     * @note not used when de-registering
     */
    IMG_UINT32 fd;
    /**
     * @brief [in] type of buffer (one of @ref CI_ALLOC_BUFF)
     * @note not used when de-registering
     */
    int type;
    /**
     * @brief Is the allocation a tiled buffer
     * @note not used when de-registering
     */
    IMG_BOOL8 bTiled;

    /**
     * @brief [in, out] output the buffer mmap offset to use (after import/
     * allocate)
     * @note input the Buffer ID when de-registering
     */
    IMG_UINT32 uiMemMapId;
    /**
     * @brief [out] size of the buffer in bytes when allocating/importing
     * @note not used when de-registering
     */
    IMG_UINT32 uiSize;
};

/**
 * @brief Get access to an LSH matrix buffer
 */
struct CI_LSH_GET_PARAM {
    IMG_UINT32 configId;
    IMG_UINT32 matrixId;
};

/**
 * @brief Update an LSH matrix buffer configuration (device memory should
 * be flushed as user-space may have written its mmap version)
 */
struct CI_LSH_SET_PARAM {
    IMG_UINT32 configId;
    IMG_UINT32 matrixId;
    IMG_UINT16 ui16SkipX;
    IMG_UINT16 ui16SkipY;
    IMG_UINT16 ui16OffsetX;
    IMG_UINT16 ui16OffsetY;
    IMG_UINT8 ui8TileSizeLog2;
    IMG_UINT8 ui8BitsPerDiff;
    IMG_UINT16 ui16Width;
    IMG_UINT16 ui16Height;
    IMG_UINT16 ui16LineSize;
    IMG_UINT32 ui32Stride;
};

/** @brief change the matrix currently in use by a pipeline object */
struct CI_LSH_CHANGE_PARAM {
    IMG_UINT32 configId;
    IMG_UINT32 matrixId;  // if 0 means disabling the matrix
};

struct CI_PIPE_INFO {
    CI_PIPELINE_CONFIG config;
    /** @brief copy of the user-side pipeline configuration */
    CI_MODULE_IIF iifConfig;
    /** @brief copy of the user-side pipeline configuration */
    CI_MODULE_ENS ensConfig;
    /** @brief copy of the user-side pipeline configuration */
    CI_MODULE_LSH lshConfig;
    /** @brief copy of the user-side pipeline configuration */
    CI_MODULE_DPF dpfConfig;
    /** @brief copy of the user-side pipeline configuration */
    CI_AWS_CURVE_COEFFS sAWSCurveCoeffs;
};

/**
 * @brief Capture IOCTL parameter (used for registration and update)
 *
 * And the second level pointers should be changed too (e.g. LSH, DPF)
 *
 * @see CI_IOCTL_PIPE_UPD
 */
struct CI_PIPE_PARAM {
    struct CI_PIPE_INFO info;
    /**
     * @brief [in] user-space memory for the load structure
     * @warning the size is assumed to be CI_HWINFO::uiSizeOfLoadStruct
     */
    void *loadstructure;
    /**
     * @brief [in] Pipeline's ID
     */
    int identifier;
    /**
     * @brief [in] which modules to update (from enum CI_MODULE_UPDATE)
     */
    int eUpdateMask;
    /**
     * @brief [in] When updating try to apply the new configuration on the already 
     * pending buffers
     */
    IMG_BOOL8 bUpdateASAP;
};

/**
 * @brief Used when asking for an available frame
 *
 * @see CI_IOCTL_CAPT_BAQ
 */
struct CI_BUFFER_PARAM {
    /**
     * @brief [in] Associated Pipeline's ID
     */
    int captureId;
    /**
     * @brief [in] To know if the operation can be blocked
     */
    IMG_BOOL8 bBlocking;

    /**
     * @brief [in] Frame buffer identifier 
     */
    int shotId;
    /**
     * @brief [out] strides and height per dynamic buffer and dynamic buffers 
     * IDs
     */
    CI_SHOT shot;
};

/**
 * @brief Used to trigger a shot and release the buffers once used
 *
 * @see CI_IOCTL_CAPT_TRG and CI_IOCTL_CAPT_BRE
 */
struct CI_BUFFER_TRIGG {
    /**
     * @brief [in] Associated Pipeline's ID
     */
    int captureId;
    /**
     * @brief [in] To know if the operation can be blocked
     */
    IMG_BOOL8 bBlocking;

    /**
     * @brief [in] Shot ID to use
     */
    int shotId;
    /**
     * @brief [in] Buffer IDs to use
     */
    CI_BUFFID bufferIds;
};

/**
 * @brief Gasket information
 *
 * @see CI_IOCTL_GASK_NFO
 */
struct CI_GASKET_PARAM {
    /**
     * @brief [in] HW gasket number
     */
    IMG_UINT8 uiGasket;
    /**
     * @brief [out] HW gasket information
     */
    CI_GASKET_INFO sGasketInfo;
};

/**
 * @brief Access number of available buffers and shots
 *
 * @see CI_IOCTL_PIPE_AVL
 */
struct CI_HAS_AVAIL {
    /**
     * @brief [in] Associated Pipeline's ID
     */
    int captureId;
    /**
     * @brief [out] Number of available shots
     */
    unsigned int uiShots;
    /**
     * @brief [out] Number of available buffers (regardless of types)
     */
    unsigned int uiBuffers;
};

/**
 * @brief Internal Data Generator frame information
 *
 * Used when allocating, acquiring and freeing DG frames
 *
 * @see CI_INDG_ALL, CI_IOCTL_INDG_ACQ and CI_IOCTL_INDG_FRE
 */
struct CI_DG_FRAMEINFO {
    /**
     * @brief [in] Associated datagen ID
     */
    int datagenId;
    /**
     * @brief [in] size in bytes (used only when allocating)
     */
    unsigned int uiSize;
    /**
     * @brief [out] mmap offset (used only when allocating)
     */
    IMG_UINT32 mmapId;
    /**
     * @brief [in, out] frame identifier
     *
     * @li [out] when allocating
     * @li [in] when freeing
     * @li [in, out] when acquiring (input == 0 then asks for 1st available, 
     * otherwise search for given ID)
     */
    IMG_UINT32 uiFrameId;
};

/**
 * @brief Frame information when triggering
 *
 * @see CI_IOCTL_INDG_TRG
 */
struct CI_DG_FRAMETRIG {
    /**
     * @brief [in] Associated datagen ID
     */
    int datagenId;
    /**
     * @brief [in] frame identifier
     */
    IMG_UINT32 uiFrameId;

    /**
     * @brief [in] stride to use for capture
     */
    IMG_UINT32 uiStride;
    /**
     * @brief [in] image width in pixels
     */
    IMG_UINT16 uiWidth;
    /**
     * @brief [in] image height in lines
     */
    IMG_UINT16 uiHeight;
    /**
     * @brief [in] horizontal blanking in pixels
     */
    IMG_UINT16 uiHorizontalBlanking;
    /**
     * @brief [in] vertical blanking in lines
     */
    IMG_UINT16 uiVerticalBlanking;

    /**
     * @brief [in] trigger or release boolean (used only when triggering)
     *
     * Trigger starts a capture while release only cancels the acquired frame
     */
    IMG_BOOL8 bTrigger;
};

/**
 * @brief Information when waiting for pending frame
 *
 * @see CI_IOCTL_INDG_WAI
 */
struct CI_DG_FRAMEWAIT {
    /**
     * @brief [in] Associated datagen ID
     */
    int datagenId;
    /**
     * @brief [in] blocking wait or not
     */
    IMG_BOOL8 bBlocking;
};

/**
 * @brief Information to start an Internal DG and reserve its HW
 *
 * @see CI_IOCTL_INDG_STA
 */
struct CI_DG_PARAM {
    /**
     * @brief [in] Associated datagen ID
     */
    int datagenId;
    /**
     * @brief [in] Configuration
     */
    CI_DATAGEN sDG;
};

#define CI_BANK_CTX_MAX (CI_BANK_CTX + CI_N_CONTEXT)
#define CI_BANK_GASKET_MAX (CI_BANK_GASKET + CI_N_IMAGERS)
#define CI_BANK_IIF_MAX (CI_BANK_IIFDG + CI_N_IIF_DATAGEN)
/**
* @brief access to banks in @ref KRN_CI_DRIVER_HANDLE
*
* Only if debug functions are available
*/
enum CI_DEBUG_BANKS {
    CI_BANK_CORE = 0,
    CI_BANK_GMA = 1,
    /** from CI_BANK_CTX to (CI_BANK_CTX + CI_N_CONTEXT -1) */
    CI_BANK_CTX = 2,
    /** from CI_BANK_GASKET to (CI_BANK_GASKET + CI_N_IMAGERS -1) */
    CI_BANK_GASKET = CI_BANK_CTX_MAX,
    /** from CI_BANK_IFF to (CI_BANK_IFF + CI_N_IIF_DATAGEN -1) */
    CI_BANK_IIFDG = CI_BANK_GASKET_MAX,
    CI_BANK_MEM = CI_BANK_IIF_MAX,

    /** not an actual value, if >= then bank enum is invalid */
    CI_BANK_N,
};

/** only if debug fucntions are available */
struct CI_DEBUG_REG_PARAM {
    /** @brief [in] read or write */
    IMG_BOOL8 bRead;
    /** @brief [in] bank to access (from @ref CI_DEBUG_BANKS) */
    IMG_UINT8 eBank;
    /** @brief [in] offset in bank */
    IMG_UINT32 ui32Offset;
    /** @brief [in, out] value */
    IMG_UINT32 ui32Value;
};

/*-----------------------------------------------------------------------------
 * End of IOCTL structures
 *---------------------------------------------------------------------------*/

enum CI_eIOCTL {
    /**
     * @brief Get the HW info strait after opening the connection
     */
    CI_INFO_GET,
    /**
     * @brief Update line-store from kernel-space to user-space
     */
    CI_LINESTORE_GET,
    /**
     * @brief Propose new line-store from user-space to kernel-space
     */
    CI_LINESTORE_SET,
    /**
     * @brief Update GMA LUT from kernel-space to user-space
     */
    CI_GMALUT_GET,
    /**
     * @brief Propose new GMA LUT from user-space to kernel-space
     */
    CI_GMALUT_SET,
    /**
     * @brief Get the current time-stamp value from HW
     */
    CI_TIMEST_GET,
    /**
     * @brief Get the current amount of DPF internal buffer available
     */
    CI_DPFINT_GET,
    /**
     * @brief Read the RTM registers
     */
    CI_RTM_GET,

    /**
     * @brief register the pipeline object to the kernel
     */
    CI_PIPELINE_REGISTER,
    /**
     * @brief de-register the pipeline object from the kernel
     */
    CI_PIPELINE_DEREGISTER,

    /**
     * @brief add N shots to the pipeline configuration
     */
    CI_CONFIG_ADDPOOL,
    /**
     * @brief Remove the whole list of shots from the pipeline configuration
     */
    CI_CONFIG_DELPOOL,
    /**
     * @brief update the pipeline configuration from user space
     */
    CI_CONFIG_UPDATE,

    /**
     * @brief import buffer from user space or allocate it in kernel-space
     */
    CI_CREATE_BUFF,
    /**
     * @brief de-register a buffer
     */
    CI_DEREGISTER_BUFF,
    /**
     * @brief Get access to an LSH buffer
     */
    CI_PIPELINE_GET_LSH,
    /**
     * @brief Release access of an LSH buffer - should flush cache
     */
     CI_PIPELINE_SET_LSH,
     /**
      * @brief Change the used LSH
      */
     CI_PIPELINE_CHANGE_LSH,
    /**
     * @brief acquire the HW for this capture
     */
    CI_CAPTURE_START,
    /**
     * @brief release the HW
     */
    CI_CAPTURE_STOP,
    /**
     * @brief capture a frame - may be blocking or not
     */
    CI_CAPTURE_TRIGGER,
    /**
     * @brief get the shot and its pointers (includes stats) from the capture 
     * @note may be blocking or not
     */
    CI_CAPTURE_ACQUIRE,
    /**
     * @brief release the shot and its pointers
     * @note non-blocking
     */
    CI_CAPTURE_RELEASE,
    /**
     * @brief to update user-side when the state is unexpected
     */
    CI_CAPTURE_ISSTARTED,
    /**
     * @brief to know if the capture has waiting shots available to user-space
     */
    CI_CAPTURE_HASWAITING,
    /**
     * @brief to know if the capture has user available shots for trigger
     */
    CI_CAPTURE_HASAVAILABLE,
    /**
     * @brief to know if the capture has pending shots in the HW
     */
    CI_CAPTURE_HASPENDING,

    /**
     * @brief acquire a Gasket
     */
    CI_GASKET_ACQUIRE,
    /**
     * @brief release a Gasket
     */
    CI_GASKET_RELEASE,
    /**
     * @brief get information about a Gasket
     */
    CI_GASKET_GETINFO,

    /**
     * @brief register the IIF DG object to the kernel side - may be used to 
     * de-register an object too
     */
    CI_IIFDG_REGISTER,
    /**
     * @brief try to acquire the needed HW for IIF DG
     */
    CI_IIFDG_START,
    /**
     * @brief to synchronise if the data generator object is already started
     */
    CI_IIFDG_ISSTARTED,
    /**
     * @brief try to acquire the needed HW for IIF DG
     */
    CI_IIFDG_STOP,
    /**
     * @brief allocate a frame for the IIF DG
     */
    CI_IIFDG_ALLOCATE,
    /**
     * @brief acquire an available frame from kernel side
     */
    CI_IIFDG_ACQUIRE,
    /**
     * @brief trigger a capture on the IIF DG or release an acquired frame
     */
    CI_IIFDG_RELEASE,
    /**
     * @brief wait for frame to be processed by hw
     */
    CI_IIFDG_WAITPROCESSED,
    /**
     * @brief free an allocated frame for the IIF DG
     */
    CI_IIFDG_FREE,
    /**
     * @brief Access reigsters (only if debug functions are available)
     */
    CI_DEBUG_REG,
#ifdef INFOTM_HW_AWB_METHOD
    /**
     * @brief Update hardware AWB boundary from kernel-space to user-space
     */
    CI_HW_AWB_BOUNDARY_GET,
    /**
     * @brief Propose new hardware AWB boundary from user-space to kernel-space
     */
    CI_HW_AWB_BOUNDARY_SET,
#endif // INFOTM_HW_AWB_METHOD

    /** Not an actual IOCTL - just the number of commands */
    CI_IOCTL_N
};

/**
 * @brief IOCTL magic number
 */
#define CI_MAGIC '!'

/*
 * note: reading/writing is relative to user space memory
 */

/**
 * @}
 * @name Driver commands
 * @{
 */

#define CI_IOCTL_INFO _IOW(CI_MAGIC, CI_INFO_GET, CI_CONNECTION*)
#define CI_IOCTL_LINE_GET _IOW(CI_MAGIC, CI_LINESTORE_GET, CI_LINESTORE*)
#define CI_IOCTL_LINE_SET _IOR(CI_MAGIC, CI_LINESTORE_SET, CI_LINESTORE*)
#define CI_IOCTL_GMAL_GET _IOW(CI_MAGIC, CI_GMALUT_GET, CI_MODULE_GMA_LUT*)
#define CI_IOCTL_GMAL_SET _IOR(CI_MAGIC, CI_GMALUT_SET, CI_MODULE_GMA_LUT*)
#define CI_IOCTL_TIME_GET _IOW(CI_MAGIC, CI_TIMEST_GET, IMG_UINT32*)
#define CI_IOCTL_DPFI_GET _IOW(CI_MAGIC, CI_DPFINT_GET, IMG_UINT32*)
#define CI_IOCTL_RTM_GET _IOW(CI_MAGIC, CI_RTM_GET, CI_RTM_INFO*)

#define CI_IOCTL_DBG_REG _IOWR(CI_MAGIC, CI_DEBUG_REG, \
    struct CI_DEBUG_REG_PARAM*)

/**
 * @}
 * @name Configuration commands
 * @{
 */

#define CI_IOCTL_PIPE_REG _IOR(CI_MAGIC, CI_PIPELINE_REGISTER, struct CI_PIPE_INFO*)
#define CI_IOCTL_PIPE_DEL _IOR(CI_MAGIC, CI_PIPELINE_DEREGISTER, int)

#define CI_IOCTL_PIPE_WAI _IOR(CI_MAGIC, CI_CAPTURE_HASWAITING, int)
#define CI_IOCTL_PIPE_AVL _IOWR(CI_MAGIC, CI_CAPTURE_HASAVAILABLE, \
    struct CI_HAS_AVAIL*)
#define CI_IOCTL_PIPE_PEN _IOR(CI_MAGIC, CI_CAPTURE_HASPENDING, int)
#define CI_IOCTL_PIPE_ADD _IOWR(CI_MAGIC, CI_CONFIG_ADDPOOL, \
    struct CI_POOL_PARAM*)
#define CI_IOCTL_PIPE_REM _IOR(CI_MAGIC, CI_CONFIG_DELPOOL, int)
#define CI_IOCTL_PIPE_UPD _IOR(CI_MAGIC, CI_CONFIG_UPDATE, \
    struct CI_PIPE_PARAM*)

#define CI_IOCTL_CREATE_BUFF _IOWR(CI_MAGIC, CI_CREATE_BUFF, \
    struct CI_ALLOC_PARAM*)
#define CI_IOCTL_DEREG_BUFF _IOR(CI_MAGIC, CI_DEREGISTER_BUFF, \
    struct CI_ALLOC_PARAM*)

#define CI_IOCTL_GET_LSH _IOR(CI_MAGIC, CI_PIPELINE_GET_LSH, \
    struct CI_LSH_GET_PARAM*)
#define CI_IOCTL_SET_LSH _IOR(CI_MAGIC, CI_PIPELINE_SET_LSH, \
    struct CI_LSH_SET_PARAM*)
#define CI_IOCTL_CHA_LSH _IOR(CI_MAGIC, CI_PIPELINE_CHANGE_LSH, \
    struct CI_LSH_CHANGE_PARAM*)

/**
 * @}
 * @name Capture commands
 * @{
 */

#define CI_IOCTL_CAPT_STA _IOR(CI_MAGIC, CI_CAPTURE_START, int)
#define CI_IOCTL_CAPT_STP _IOR(CI_MAGIC, CI_CAPTURE_STOP, int)
#define CI_IOCTL_CAPT_TRG _IOR(CI_MAGIC, CI_CAPTURE_TRIGGER, \
    struct CI_BUFFER_TRIGG*)
#define CI_IOCTL_CAPT_BAQ _IOWR(CI_MAGIC, CI_CAPTURE_ACQUIRE, \
    struct CI_BUFFER_PARAM*)
#define CI_IOCTL_CAPT_BRE _IOR(CI_MAGIC, CI_CAPTURE_RELEASE, \
    struct CI_BUFFER_TRIGG*)
#define CI_IOCTL_CAPT_ISS _IOR(CI_MAGIC, CI_CAPTURE_ISSTARTED, int)

/**
 * @}
 * @name Gasket commands
 * @{
 */

#define CI_IOCTL_GASK_ACQ _IOR(CI_MAGIC, CI_GASKET_ACQUIRE, struct CI_GASKET*)
#define CI_IOCTL_GASK_REL _IOR(CI_MAGIC, CI_GASKET_RELEASE, int)
#define CI_IOCTL_GASK_NFO _IOWR(CI_MAGIC, CI_GASKET_GETINFO, \
    struct CI_GASKET_PARAM*)

/**
 * @}
 * @name Internal Data Generator commands (IIF DG)
 * @{
 */

#define CI_IOCTL_INDG_REG _IOR(CI_MAGIC, CI_IIFDG_REGISTER, int)
#define CI_IOCTL_INDG_STA _IOR(CI_MAGIC, CI_IIFDG_START, struct CI_DG_PARAM*)
#define CI_IOCTL_INDG_ISS _IOR(CI_MAGIC, CI_IIFDG_ISSTARTED, int)
#define CI_IOCTL_INDG_STP _IOR(CI_MAGIC, CI_IIFDG_STOP, int)
#define CI_IOCTL_INDG_ALL _IOWR(CI_MAGIC, CI_IIFDG_ALLOCATE, \
    struct CI_DG_FRAMEINFO*)
#define CI_IOCTL_INDG_ACQ _IOWR(CI_MAGIC, CI_IIFDG_ACQUIRE, \
    struct CI_DG_FRAMEINFO*)
#define CI_IOCTL_INDG_TRG _IOR(CI_MAGIC, CI_IIFDG_RELEASE, \
    struct CI_DG_FRAMETRIG*)
#define CI_IOCTL_INDG_WAI _IOR(CI_MAGIC, CI_IIFDG_WAITPROCESSED, \
    struct CI_DG_FRAMEWAIT*)
#define CI_IOCTL_INDG_FRE _IOR(CI_MAGIC, CI_IIFDG_FREE, \
    struct CI_DG_FRAMEINFO*)

#ifdef INFOTM_HW_AWB_METHOD
/**
 * @}
 * @name Infotm hardware AWB commands (HW AWB)
 * @{
 */
#define CI_IOCTL_HW_AWB_BOUNDARY_GET _IOWR(CI_MAGIC, CI_HW_AWB_BOUNDARY_GET, \
    HW_AWB_BOUNDARY*)
#define CI_IOCTL_HW_AWB_BOUNDARY_SET _IOR(CI_MAGIC, CI_HW_AWB_BOUNDARY_SET, \
    HW_AWB_BOUNDARY*)
#endif // INFOTM_HW_AWB_METHOD

/**
 * @}
 */
/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of CI_IOCTL documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_IOCTRL_H_ */
