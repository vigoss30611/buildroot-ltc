/**
*******************************************************************************
 @file dg_api_structs.h

 @brief External Data Generator structures

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
#ifndef DG_API_STRUCTS_H_
#define DG_API_STRUCTS_H_

#include <img_types.h>

#include "felixcommon/pixel_format.h"  // bayer format
#include "ci/ci_api_structs.h"  // converter structs

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup DG_API_Conv
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the DG_API documentation module
 *---------------------------------------------------------------------------*/

/** @brief HW does blanking = reg + 40 */
#define DG_H_BLANKING_SUB (40)

typedef struct CI_DG_FRAME DG_FRAME;

/**
 * @}
 * @ingroup DG_API_Camera
 * @{
 */

typedef struct DG_CAMERA
{
    CI_CONV_FMT eFormat;
    IMG_UINT8 ui8FormatBitdepth;
    /** @brief Use to ensure frames do not change bayer order when triggered */
    enum MOSAICType eBayerOrder;
    /**
     * @brief Data Gen to use (0 to CI_N_EXT_DATAGEN-1) - 1 to 1 with gaskets
     */
    IMG_UINT8 ui8Gasket;

    /** @brief Horizontal and Vertical active lines for parallel */
    IMG_BOOL8 bParallelActive[2];

    IMG_BOOL8 bPreload;

    /**
     * @name Mipi protocol configuration (only if using MIPI format)
     * @{
     */
    IMG_UINT16 ui16MipiTLPX;
    IMG_UINT16 ui16MipiTHS_prepare;
    IMG_UINT16 ui16MipiTHS_zero;
    IMG_UINT16 ui16MipiTHS_trail;
    IMG_UINT16 ui16MipiTHS_exit;
    IMG_UINT16 ui16MipiTCLK_prepare;
    IMG_UINT16 ui16MipiTCLK_zero;
    IMG_UINT16 ui16MipiTCLK_post;
    IMG_UINT16 ui16MipiTCLK_trail;
    IMG_UINT8 ui8MipiLanes;
    /**
     * @}
     */
} DG_CAMERA;

/**
 * @}
 * @ingroup DG_API_Driver
 * @{
 */

typedef struct DG_HWINFO
{
    /**
     * @name Version attributes (duplicated from CI driver)
     * @{
     */

    /**
     * @brief imagination technologies identifier
     *
     * @note Read from FELIX_CORE.CORE_ID
     */
    IMG_UINT8 ui8GroupId;
    /**
     * @brief identify the core familly
     *
     * @note Read from FELIX_CORE.CORE_ID
     */
    IMG_UINT8 ui8AllocationId;
    /**
     * @brief Configuration ID
     *
     * @note Read from FELIX_CORE.CORE_ID
     */
    IMG_UINT16 ui16ConfigId;

    /**
     * @brief revision IMG division ID
     *
     * @note Read from FELIX_CORE.CORE_REVISION
     */
    IMG_UINT8 rev_ui8Designer;
    /**
     * @brief revision major
     *
     * @note Read from FELIX_CORE.CORE_REVISION
     */
    IMG_UINT8 rev_ui8Major;
    /**
     * @brief revision minor
     *
     * @note Read from FELIX_CORE.CORE_REVISION
     */
    IMG_UINT8 rev_ui8Minor;
    /**
     * @brief revision maintenance
     *
     * @note Read from FELIX_CORE.CORE_REVISION
     */
    IMG_UINT8 rev_ui8Maint;

    /**
     * @brief Unique identifier for the HW build
     *
     * @note Read from FELIX_CORE.UID_NUM
     */
    IMG_UINT32 rev_uid;

    /**
    * @}
    * @name Felix attributes
    * @{
    */

    /**
     * @brief The maximum number of pixels that the ISP pipeline can process
     * in parallel
     */
    IMG_UINT8 config_ui8Parallelism;

    /**
    * @}
    * @name Gasket attributes
    * @{
    */

    /**
    * @brief Show how many MIPI lanes are output from the imager when the
    * imager is of type mipi.
    *
    * Shows how wide the parallel bus output from the imager when the imager
    * is of type parallel.
    *
    * @note Read from FELIX_CORE.DESIGNER_REV_FIELD5
    */
    IMG_UINT8 gasket_aImagerPortWidth[CI_N_IMAGERS];
    /**
    * @brief Combinaisons of @ref CI_GASKETTYPE
    *
    * @note Read from FELIX_CORE.DESIGNER_REV_FIELD5
    */
    IMG_UINT8 gasket_aType[CI_N_IMAGERS];
    /**
    * @brief One per imager - see @ref CI_HWINFO::config_ui8NImagers
    *
    */
    IMG_UINT16 gasket_aMaxWidth[CI_N_IMAGERS];
    /** @brief One per imager - see @ref CI_HWINFO::config_ui8NImagers */
    IMG_UINT8 gasket_aBitDepth[CI_N_IMAGERS];
    /** @brief Number of MIPI virtual channels supported */
    IMG_UINT8 gasket_aNVChan[CI_N_IMAGERS];

    /**
     * @}
     * @name External DG Information
     * @{
     */

    /** @brief number of available data generators */
    IMG_UINT8 config_ui8NDatagen;

    /**
     * @}
     */
} DG_HWINFO;

typedef struct DG_CONNECTION
{
    DG_HWINFO sHWInfo;
} DG_CONNECTION;

/**
 * @}
 */
/*-------------------------------------------------------------------------
 * end of the DG_API documentation module
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DG_API_STRUCTS_H_ */
