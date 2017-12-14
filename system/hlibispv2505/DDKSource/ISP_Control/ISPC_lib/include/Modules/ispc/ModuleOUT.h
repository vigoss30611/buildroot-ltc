/**
*******************************************************************************
 @file ModuleOUT.h

 @brief Declaration of ISPC::ModuleOUT

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
#ifndef ISPC_MODULE_OUT_H_
#define ISPC_MODULE_OUT_H_

#include <ci/ci_api_structs.h>  // CI_INOUT_POINTS
#include <felixcommon/pixel_format.h>

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"


namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Configure output format for HW configuration
 *
 * ModuleOUT doesn't correspond to any particular module in the HW pipeline but
 * it is used to the set of desired output formats in MC_PIPELINE.
 *
 * Also contains the input format for HDR Insertion.
 */
class ModuleOUT: public SetupModuleBase<STP_OUT>
{
public:  // attributes
    /**
     * @brief Encoder pipeline output format
     *
     * Loaded using @ref ENCODER_TYPE
     */
    ePxlFormat encoderType;
    /**
     * @brief Display pipeline output format
     *
     * Cannot be enabled at the same time than @ref dataExtractionType.
     *
     * Loaded using @ref DISPLAY_TYPE
     */
    ePxlFormat displayType;
    /**
     * @brief Data Extraction pipeline output format
     *
     * Will extract image at selected @ref dataExtractionPoint.
     * Needs @ref displayType to be disabled and @ref dataExtractionPoint
     * to be valid.
     *
     * Loaded using @ref DATAEXTRA_TYPE
     */
    ePxlFormat dataExtractionType;
    /**
     * @brief High Dynamic Range Extraction pipeline output format
     *
     * @warning Not supported by all HW versions. Check
     * @ref CI_HWINFO::eFunctionalities
     *
     * Loaded using @ref HDREXTRA_TYPE
     */
    ePxlFormat hdrExtractionType;
    /**
     * @brief High Dynamic Range Insertion pipeline input format
     *
     * @warning Not supported by all HW versions. Check
     * @ref CI_HWINFO::eFunctionalities
     *
     * Loaded using @ref HDRINS_TYPE
     */
    ePxlFormat hdrInsertionType;
    /**
     * @brief Tiff pipeline output format
     *
     * @warning Not supported by all HW versions. Check
     * @ref CI_HWINFO::eFunctionalities
     *
     * Loaded using @ref RAW2DEXTRA_TYPE
     */
    ePxlFormat raw2DExtractionType;

    /**
     * @brief Data Extraction point in pipeline
     *
     * Point where image is extracted when using @ref dataExtractionType
     *
     * Loaded using @ref DATAEXTRA_POINT
     */
    CI_INOUT_POINTS dataExtractionPoint;

public:  // methods
    ModuleOUT();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

    /** @brief Get enum value from string */
    static ePxlFormat getPixelFormat(const std::string &stringFormat);

    /** @brief Get enum value from string
     * return PXL_INVALID if string does not match encoder types
     * */
    static ePxlFormat getPixelFormatEncoder(const std::string &stringFormat, bool failIfInvalid=true);

    /** @brief Get enum value from string
     * return PXL_INVALID if string does not match display types
     * */
    static ePxlFormat getPixelFormatDisplay(const std::string &stringFormat, bool failIfInvalid=true);

    /** @brief Get enum value from string
     * return PXL_INVALID if string does not match bayer types
     * */
    static ePxlFormat getPixelFormatBayer(const std::string &stringFormat, bool failIfInvalid=true);

    /** @brief Get string value from pixel enum */
    static std::string getPixelFormatString(ePxlFormat fmt);

public:  // parameters
    static const ParamDefSingle<std::string> ENCODER_TYPE;
    static const ParamDefSingle<std::string> DISPLAY_TYPE;
    static const ParamDefSingle<std::string> DATAEXTRA_TYPE;
    static const ParamDef<int> DATAEXTRA_POINT;
    static const ParamDefSingle<std::string> HDREXTRA_TYPE;
    static const ParamDefSingle<std::string> HDRINS_TYPE;
    static const ParamDefSingle<std::string> RAW2DEXTRA_TYPE;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_OUT_H_ */
