/**
******************************************************************************
@file Region.h

@brief Interface of sRegion container class used in region handling code
in all v2500 Camera HAL AAA modules

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

*****************************************************************************/

#ifndef REGION_H
#define REGION_H

#include <ui/Rect.h>
#include "utils/List.h"
#include "img_errors.h"

namespace android {

/**
 * @brief represents pointer in raw region data in CameraMetadata
 */
typedef int32_t* rawRegion_t;

/**
 * @brief represents specific generic region
 */
class sRegion : public Rect {
public:
    double weight;      ///<@brief weight of region

    /**
     * @brief number of uint32_t entries in CameraMetadata region field
     */
    enum {
        rawEntrySize = 5
    };

    sRegion& operator=(const sRegion& rsh) {
        set(rsh);
        weight = rsh.weight;
        return *this;
    }

    /**
     * @brief returns column index for accessing FOS tiles
     * @return column index
     */
    int32_t indexH() const {
        const int32_t width = getWidth();
        return (width>0 ? left/width : 0);
    }

    /**
     * @brief returns row index for accessing FOS tiles
     * @return row index
     */
    int32_t indexV() const {
        const int32_t height = getHeight();
         return (height>0 ? top/height : 0);
    }

    /**
     * @brief Constructs empty region + width & height
     */
    sRegion(const uint32_t x, const uint32_t y,
            const uint32_t w, const uint32_t h) :
        Rect(x, y, x+w, y+h),
        weight(0){
        ALOG_ASSERT(bottom>=top, "(%d,%d,%d,%d) bottom>=top",
            left, top, right, bottom);
        ALOG_ASSERT(right>=left, "(%d,%d,%d,%d) right>=left",
            left, top, right, bottom);
    }

    /**
     * @brief constructs region using raw CameraMetadata field and
     * width & height
     */
    sRegion(const rawRegion_t data) :
        Rect(data[0], data[1], data[2], data[3]),
        weight(data[4]) {
        ALOG_ASSERT(bottom>=top, "(%d,%d,%d,%d) bottom>=top",
            left, top, right, bottom);
        ALOG_ASSERT(right>=left, "(%d,%d,%d,%d) right>=left",
            left, top, right, bottom);
    }

    /**
     * copy constructor
     */
    sRegion(const sRegion& reg) : Rect(), weight(reg.weight) {
        set(reg);
    }

    /**
     * empty region
     */
    sRegion(void) : Rect(), weight(0) {};
};

/**
 * @brief region metering mode
 */
typedef enum {
    CENTER_WEIGHT,    ///<@brief center weighted using default algorithm
    /**
     * @brief sharpest tile from whole image, equal weights
     */
    BEST_TILE,
    WEIGHTED_AVERAGE, ///<@brief uses provided region weights
    SINGLE_ROI        ///<@brief focus on single roi
} metering_mode_t;

/**
 * @class RegionHandler
 * @brief
 *
 * @note
 */
template <class T = sRegion>
class RegionHandler {
public:

    typedef T region_t;
    typedef List<region_t> regions_t;

    RegionHandler() :
        mGridTopX(0),
        mGridTopY(0),
        mTileWidth(0),
        mTileHeight(0),
        mHorizTiles(0),
        mVertTiles(0),
        mMaxRegions(0),
        mCurrentRegions(NULL),
        mAutoRegionMeteringMode(CENTER_WEIGHT),
        mMeteringMode(mAutoRegionMeteringMode){
        mDefaultRegions.clear();
        mBestRegion = mDefaultRegions.end();
    }

    virtual ~RegionHandler(){}

    virtual double weight(int32_t indexH, int32_t indexV) = 0;

    void initInternalMeteringRegionData(
            uint32_t gridTopX, uint32_t gridTopY,
            uint32_t horizTiles, uint32_t vertTiles,
            uint32_t tileWidth, uint32_t tileHeight);
    /**
     * @brief post construction initialization of internal fields
     * @note uses functor to calculate region weights
     */
    status_t initDefaultRegions(void);

    void initNullRegion();

    /*
     * @brief set current metering regions to defaults (mDefaultRegions)
     */
    void updateRegionsWithDefaults();

    /**
     * @brief configure specific region of interest for next programmed capture
     * @note pure virtual as specific to AF/AE and AWB
     */
    virtual status_t configureRoiMeteringMode(const region_t& region) = 0;

    /**
     * @brief reset statistics module to default metering
     * @note pure virtual as specific to AF/AE and AWB
     */
    virtual status_t configureDefaultMeteringMode() = 0;

    /**
     * @brief returns region mode using raw data from CameraMetadata
     */

    metering_mode_t getMeteringMode(const regions_t& regions);

#if not LOG_NDEBUG
    /**
     * @brief converts metering_mode_t to string
     */
    const char* meteringModeStr(const metering_mode_t mode);
#endif

    uint32_t mGridTopX;    ///<@brief stores X offset of grid
    uint32_t mGridTopY;    ///<@brief stores Y offset of grid
    uint32_t mTileWidth;   ///<@brief stores tile width
    uint32_t mTileHeight;  ///<@brief stores tile height
    uint32_t mHorizTiles;  ///<@brief stores number of columns
    uint32_t mVertTiles;   ///<@brief stores number of rows
    uint32_t mMaxRegions;  ///<@brief stores total number of tiles

    /**
     * @brief regions to be measured in current capture
     */
    regions_t*  mCurrentRegions;

    ///<@brief all supported regions, used in CENTER_WEIGHT mode
    regions_t  mDefaultRegions;

    /**
     * @brief best region measured and returned in metadata
     */
    typename regions_t::iterator mBestRegion;

    /**
     * @brief metering method chosen when all weights are 0
     * @note shall be BEST_TILE or CENTER_WEIGHT
     */
    metering_mode_t mAutoRegionMeteringMode;

    /**
     * current metering mode
     */
    metering_mode_t mMeteringMode;

}; //class regionHandler

template <class T>
void RegionHandler<T>::initInternalMeteringRegionData(
        uint32_t gridTopX, uint32_t gridTopY,
        uint32_t horizTiles, uint32_t vertTiles,
        uint32_t tileWidth, uint32_t tileHeight) {

    mGridTopX = gridTopX;
    mGridTopY = gridTopY;
    mTileWidth = tileWidth;
    mTileHeight = tileHeight;
    mHorizTiles = horizTiles;
    mVertTiles = vertTiles;
    mMaxRegions = mHorizTiles*mVertTiles;
}

template <class T>
status_t RegionHandler<T>::initDefaultRegions(void) {

    mDefaultRegions.clear();

    int32_t rowY = mGridTopY;
    for(uint32_t ver=0;ver<mVertTiles;++ver) {
        int32_t tileX = mGridTopX;
        int32_t nextRowY = rowY+mTileHeight;
        for(uint32_t hor=0;hor<mHorizTiles;++hor) {
            region_t cr(tileX, rowY, mTileWidth, mTileHeight);
            tileX += mTileWidth;
            // calculate default weights using external functor
            cr.weight = weight(cr.indexH(), cr.indexV());
            mDefaultRegions.push_front(cr);
#ifdef BE_VERBOSE
            LOG_DEBUG("New region %dx%d : (%d,%d,%d,%d) w=%f",
                    cr.indexH(), cr.indexV(),
                    cr.left, cr.top, cr.right, cr.bottom, cr.weight);
#endif
        }
        rowY = nextRowY;
    }
    return NO_ERROR;
}

template <class T>
void RegionHandler<T>::initNullRegion() {
    mDefaultRegions.clear();
    mDefaultRegions.push_front(region_t());
}

template <class T>
void RegionHandler<T>::updateRegionsWithDefaults() {
    mCurrentRegions = &mDefaultRegions;
}

template <class T>
metering_mode_t RegionHandler<T>::getMeteringMode(
        const regions_t& regions) {

    metering_mode_t mode = CENTER_WEIGHT;
    const uint32_t count = regions.size();
    bool autoRegion = true;
    if(!count) {
        // default value
        return mode;
    }
    // parse raw data
    typename regions_t::const_iterator i = regions.begin();
    while(i!=regions.end()) {
        if(i->weight > 0) {
            autoRegion = false;
            break;
        }
        ++i;
    }

    if(autoRegion) {
        // if all weights are zero then mode choosen by hal
        // BEST_TILE or CENTER_WEIGHT
        mode = mAutoRegionMeteringMode;
    } else if (count == 1){
        // special case because framework can provide single region different
        // than supported by HAL
        mode = SINGLE_ROI;
    } else {
        mode = WEIGHTED_AVERAGE;
    }
#ifdef BE_VERBOSE
    LOG_DEBUG("Metering mode set to %s", meteringModeStr(mode));
#endif
    return mode;
}

#if not LOG_NDEBUG
template <class T>
const char* RegionHandler<T>::meteringModeStr(const metering_mode_t mode) {
    switch (mode) {
    case CENTER_WEIGHT:
        return "CENTER_WEIGHT";
        break;
    case BEST_TILE:
        return "BEST_TILE";
        break;
    case WEIGHTED_AVERAGE:
        return "WEIGHTED_AVERAGE";
        break;
    case SINGLE_ROI:
        return "SINGLE_ROI";
        break;
    default:
        break;
    }
    return "invalid";
}
#endif /* LOG_NDEBUG */

} //namespace android

#endif /*REGION_H*/
