/**
*******************************************************************************
 @file ModuleDPF.h

 @brief Declaration of ISPC::ModuleDPF

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
#ifndef ISPC_MODULE_DPF_H_
#define ISPC_MODULE_DPF_H_

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @ingroup ISPC2_STATSMODULES
 * @brief Defective Pixel Fixing (DPG) HW configuration
 *
 * Setup function configures the @ref MC_DPF elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * ModuleDPF is part of both @ref ISPC2_SETUPMODULES and 
 * @ref ISPC2_STATSMODULES because it corrects defects (by detection or input
 * map) as well as generate an output map of defects which can be considered as
 * statistics.
 *
 * Output map information is generated in @ref Metadata::defectiveStats and
 * content in Shot::DPF.
 */
class ModuleDPF: public SetupModuleBase<STP_DPF>
{
public:  // attributes
    /**
     * @brief enable HW detect of defective pixels
     * 
     * Loaded using @ref DPF_DETECT_ENABLE
     */
    bool bDetect;
    /**
     * @brief enabled HW input map - needs pDefectMap to be populated
     *
     * Loaded using @ref DPF_READ_ENABLE
     */
    bool bRead;
    /**
     * @brief enable HW write (forces detect on)
     *
     * Loaded using @ref DPF_WRITE_ENABLE
     */
    bool bWrite;
    /**
     * @brief [int,u'8.0] Detection threshold
     *
     * Loaded using @ref DPF_THRESHOLD
     */
    IMG_UINT32 ui32Threshold;
    /**
     * @brief [float] Detection MAD weight
     *
     * Loaded using @ref DPF_WEIGHT
     */
    double fWeight;

    /**
     * @brief Defect map to give to HW
     *
     * 2*ui32NbDefects (x,y) coordinates sorted in raster order
     *
     * Can be loaded using @ref DPF_READ_MAP_FILE parameter.
     */
    IMG_UINT16 *pDefectMap;
    /**
     * 
     */
    IMG_UINT32 ui32NbDefects;

public:  // methods
    ModuleDPF();
    virtual ~ModuleDPF();

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /**@copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

    bool hasInputMap() const;

public:  // parameters
    static const ParamDefSingle<bool> DPF_DETECT_ENABLE;
    static const ParamDefSingle<bool> DPF_READ_ENABLE;
    static const ParamDefSingle<bool> DPF_WRITE_ENABLE;
    static const ParamDefSingle<std::string> DPF_READ_MAP_FILE;
    static const ParamDef<double> DPF_WEIGHT;
    static const ParamDef<int> DPF_THRESHOLD;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_DPF_H_ */
