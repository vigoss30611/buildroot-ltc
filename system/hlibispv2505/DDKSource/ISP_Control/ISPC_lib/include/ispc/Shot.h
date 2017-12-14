/**
*******************************************************************************
 @file Shot.h

 @brief Declaration of ISPC::Shot

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
#ifndef ISPC_SHOT_H_
#define ISPC_SHOT_H_

#include <img_types.h>
#include <felixcommon/pixel_format.h>
#include <ci/ci_api_structs.h>
#include <mc/mc_structs.h>

namespace ISPC {

class ModuleOUT;  // ispc/ModuleOUT.h
struct Global_Setup;  // ispc/ISPCDefs.h
class Pipeline;  // ispc/Pipeline.h

/**
 * @brief Contains the information for a generic image buffer returned
 * from CI
 *
 * Separates the CI_SHOT information per output (YUV, RGB, etc)
 */
typedef struct Buffer
{
    /**
     * @brief desired output size in pixels
     *
     * Derived from CI_SHOT and CI_PIPELINE information
     */
    IMG_UINT16 width;
    /**
     * @brief desired output size in lines
     *
     * Derived from CI_SHOT and CI_PIPELINE information
     */
    IMG_UINT16 height;
    /**
     * @brief distance to the next line in bytes
     *
     * If pxlFormat is YUV this is the Luma stride and strideCbCr is the
     * chroma stride.
     *
     * Derived from CI_SHOT information
     */
    IMG_UINT16 stride;

    /**
     * @brief actual allocated height in bytes
     *
     * Useful when doing tiling to include the bottom of the image
     */
    IMG_UINT16 vstride;
    /**
     * @brief distance to the next line in Chroma in bytes (only for YUV
     * output)
     *
     * If pxlFormat is YUV this is the chroma stride otherwise it is not
     * used
     */
    IMG_UINT16 strideCbCr;
    /**
     * @brief actual allocated height in bytes (only for YUV output)
     *
     * Useful when doing tiling to include the bottom of the image
     */
    IMG_UINT16 vstrideCbCr;
    /**
     * @brief distance to the start of the data in bytes
     */
    IMG_UINT32 offset;
    /**
     * @brief distance to chroma data in bytes (only for YUV output)
     *
     * If pxlFormat is YUV this is the chroma offset in bytes from the
     * start of the buffer.
     */
    IMG_UINT32 offsetCbCr;
    /**
     * @brief Image pixel format
     *
     * Derived from CI_PIPELINE information
     */
    ePxlFormat pxlFormat;
    /**
     * @brief Pointer to actual buffer data (available data from CI_SHOT)
     */
    const IMG_UINT8 *data;
#ifdef INFOTM_ISP		
	//add for frame memory start physical address		
	void *phyAddr;
#endif //INFOTM_ISP	

    /** @brief ID used when importing/allocating buffer */
    IMG_UINT32 id;
    /** @brief To know if the buffer was imported/allocated as tiled */
    bool isTiled;

    /** @brief uses data and offset to return first entry address */
    const IMG_UINT8* firstData() const;

    /** @brief uses data and offsetCbCr to return first entry address */
    const IMG_UINT8* firstDataCbCr() const;
} Buffer;

/**
 * @brief Contain some statistics output that are not converted
 */
typedef struct Statistics
{
    /**
     * @brief Pointer to actual buffer data (available data from CI_SHOT)
     */
    const IMG_UINT8 *data;

    /** @brief in Bytes of accessible data */
    IMG_UINT32 stride;
    /** @brief in Bytes of useful data */
    IMG_UINT32 size;
    /** @brief in Bytes - element size if relevant */
    IMG_UINT32 elementSize;
} Statistics;

/**
 * @brief Output meta-data used for AAA
 */
typedef struct Metadata
{
    /**
     * @brief Statistics about number of clipped pixels gathered from
     * the imager interface.
     */
    MC_STATS_EXS clippingStats;
    /** @brief associated configuration of @ref clippingStats */
    MC_EXS clippingConfig;

    /** @brief Histograms for global image and 7x7 grid tiles. */
    MC_STATS_HIS histogramStats;
    /** @brief associated configuration of @ref histogramStats */
    MC_HIS histogramConfig;

    /**
     * @brief Sharpness measure for the global image and 7x7 grid tiles.
     */
    MC_STATS_FOS focusStats;
    /** @brief associated configuration of @ref focusStats */
    MC_FOS focusConfig;

    /**
     * @brief White balance statistics. 3 different methods (AC, HLW and
     * WP).
     */
    MC_STATS_WBS whiteBalanceStats;
    /** @brief associated configuration of @ref whiteBalanceStats */
    MC_WBS whiteBalanceConfig;

    /**
     * @brief auto White balance statistics using Planckian locus distance
     * method
     */
    MC_STATS_AWS autoWhiteBalanceStats;
    /** @brief associated configuration of @ref whiteBalanceStats */
    MC_AWS autoWhiteBalanceConfig;

    /** @brief Automatic flicker detection information. */
    MC_STATS_FLD flickerStats;
    /** @brief associated configuration of @ref flickerStats */
    MC_FLD flickerConfig;

    /** @brief Timestamps information */
    MC_STATS_TIMESTAMP timestamps;

    /** @brief Defective pixel statistics (NOT including output map) */
    MC_STATS_DPF defectiveStats;
    /**
     * @brief associated configuration of @ref defectiveStats and
     * @ref Shot::DPF */
    MC_DPF defectiveConfig;

    /** @brief associated configuration of @ref Shot::ENS */
    MC_ENS encodeStatsConfig;

#ifdef INFOTM_HW_AWB_METHOD
	MC_HW_AWB hwAwbConfig;
#endif //INFOTM_HW_AWB_METHOD
} Metadata;

/**
 * @brief Structure containing all the information for a gathered shot
 * (actual image buffer, metadata, timestamp, etc.)
 */
class Shot
{
public:
    /** @brief Image buffer for DISPLAY output */
    Buffer DISPLAY;
    /** @brief Image buffer for ENCODER output */
    Buffer YUV;
    /** @brief Image buffer for Bayer output */
    Buffer BAYER;
    /** @brief Image buffer for HDR Extraction output */
    Buffer HDREXT;
    /** @brief Image buffer for RAW2D Extraction output */
    Buffer RAW2DEXT;

    /** @brief is frame correct? */
    bool bFrameError;
    /**
     * @brief Number of missed frames before that one was acquired
     *
     * @note if negative cannot be trusted
     */
    int iMissedFrames;

    /** @brief Shot metadata */
    Metadata metadata;

    /**
     * @brief Defective pixels output map
     *
     * @note size is computed from Metadata and DPF_MAP_OUTPUT_SIZE
     */
    Statistics DPF;
    /**
     * @brief Encoder statistics output
     */
    Statistics ENS;

    /**
     * @brief Contains all statistics used to compute meta-data
     * information
     *
     * @note size is the size of the HW Save Structure
     */
    Statistics stats;

protected:
    /**
     * @brief Reset all Buffer and metadata to default
     */
    void clear();

    /**
     * @brief Populates the Buffers and Metadata using CI_SHOT
     */
    void configure(CI_SHOT *pCIBuffer, const ModuleOUT &globalConfig,
        const Global_Setup &globalSetup);

    /** @brief Corresponding CI_SHOT */
    CI_SHOT *pCIBuffer;

    friend class ISPC::Pipeline;
};

} /* namespace ISPC */

#endif /* ISPC_SHOT_H_ */
