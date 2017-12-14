/**
*******************************************************************************
 @file ModuleAWS.h

 @brief Declaration of ISPC::ModuleAWS

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
#ifndef ISPC_MODULE_AWS_H_
#define ISPC_MODULE_AWS_H_

#include <felix_hw_info.h>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_STATSMODULES
 * @brief Auto White Balance Statistics (AWS) HW configuration
 *
 * Setup function configures the @ref MC_AWS elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * Output generated in @ref Metadata::autoWhiteBalanceStats
 */
class ModuleAWS: public SetupModuleBase<STP_AWS>
{
public:  // attributes

    /**
     * @brief container defining a single approximation line
     */
    typedef struct curveLine {
        double fBoundary;
        double fXcoeff;
        double fYcoeff;
        double fOffset;
    } curveLine_t;

    /**
     * @brief type of set of lines
     */
    typedef std::list<curveLine> linesSet;

    /**
     * @brief iterator type for curvesSet_t
     */
    typedef linesSet::const_iterator lineIterator;

    bool enabled;

    bool debugMode;

    double fLog2_R_Qeff;
    double fLog2_B_Qeff;

    double fRedDarkThresh;
    double fBlueDarkThresh;
    double fGreenDarkThresh;

    double fRedClipThresh;
    double fBlueClipThresh;
    double fGreenClipThresh;

    double fBbDist;

    IMG_UINT16 ui16GridStartRow;
    IMG_UINT16 ui16GridStartColumn;

    IMG_UINT16 ui16TileWidth;
    IMG_UINT16 ui16TileHeight;

    /**
     * @brief container for lines
     */
    linesSet fLines;

public:  // methods
    ModuleAWS();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

    /**
     * @brief getter of AWS debug mode
     */
    inline bool getDebugMode() const { return debugMode; }

    /**
     * @brief setter of debug mode
     */
    inline void setDebugMode(const bool mode) {
        debugMode = mode;
        requestUpdate();
    }

    /**
     * @brief load approximation lines from parameter file
     *
     * @return number of lines loaded, -1 if lines not empty
     */
    static int loadLinesParams(
            const ParameterList &parameters,
            linesSet& lines);

public:  // parameters
    static const ParamDefSingle<bool> AWS_ENABLED;
    static const ParamDefSingle<bool> AWS_DEBUG_MODE;

    static const ParamDefArray<short> AWS_TILE_START_COORDS;

    /**
     * @brief whole rectangle covering all tiles
     * @note in pixels
     */
    static const ParamDefArray<short> AWS_TILE_SIZE;
    static const short AWS_TILE_WIDTH_MIN;
    static const short AWS_TILE_HEIGHT_MIN;

    static const ParamDef<double> AWS_LOG2_R_QEFF;
    static const ParamDef<double> AWS_LOG2_B_QEFF;

    static const ParamDef<double> AWS_R_DARK_THRESH;
    static const ParamDef<double> AWS_G_DARK_THRESH;
    static const ParamDef<double> AWS_B_DARK_THRESH;

    static const ParamDef<double> AWS_R_CLIP_THRESH;
    static const ParamDef<double> AWS_G_CLIP_THRESH;
    static const ParamDef<double> AWS_B_CLIP_THRESH;

    static const ParamDef<double> AWS_BB_DIST;

    /**
     * @brief Number of approximation curves
     */
    static const ParamDef<int> AWS_CURVES_NUM;

    /**
     * @brief X coefficients for approximation lines
     */
    static const ParamDefArray<double> AWS_CURVE_X_COEFFS;

    /**
     * @brief Y coefficients for approximation lines
     */
    static const ParamDefArray<double> AWS_CURVE_Y_COEFFS;

    /**
     * @brief Offsets for approximation lines
     */
    static const ParamDefArray<double> AWS_CURVE_OFFSETS;

    /**
     * @brief Thresholds to define which line approximation to use
     *
     * @note First entry defines line segment 0 outer threshold, further entries
     * define threshold to choose between line segment n-1 and n
     */
    static const ParamDefArray<double> AWS_CURVE_BOUNDARIES;

    static ParameterGroup getGroup();
};

/**
 * @brief globally visible relational operator used for sorting by line boundary
 */
bool operator<(const ModuleAWS::curveLine& l,
        const ModuleAWS::curveLine& r);

} /* namespace ISPC */

#endif /* ISPC_MODULE_AWS_H_ */
