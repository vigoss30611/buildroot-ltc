/**
*******************************************************************************
 @file ModuleLSH.h

 @brief Declaration of ISPC::ModuleLSH

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
#ifndef ISPC_MODULE_LSH_H_
#define ISPC_MODULE_LSH_H_

#include <felixcommon/lshgrid.h>

#include <string>

#include "ispc/Parameter.h"
#include "ispc/Module.h"

namespace ISPC {

/**
 * @ingroup ISPC2_SETUPMODULES
 * @brief Lens Shading correction (LSH) HW configuration (excluding white 
 * balance)
 *
 * Setup function configures the @ref MC_LSH elements of the MC_PIPELINE
 * attached to the owner Pipeline.
 *
 * The @ref ModuleWBC configures the white balance correction part of the
 * LSH HW module.
 */
class ModuleLSH: public SetupModuleBase<STP_LSH>
{
public:  // attributes
    /**
     * @brief Enable/disable use of lens shading matrix @ref sGrid
     *
     * Loaded using @ref LSH_MATRIX
     */
    bool    bEnableMatrix;
    /**
     * @brief [delta per CFA] Horizontal linear shading gradient per channel
     * (always RGGB order)
     *
     * Loaded using @ref LSH_GRADIENT_X
     */
    double   aGradientX[4];
    /**
     * @brief [delta per CFA] Vertical linear shading gradient per channel 
     * (always RGGB order)
     *
     * Loaded using @ref LSH_GRADIENT_Y
     */
    double   aGradientY[4];

    /**
     * @brief Structure to fill that contains the LSH information
     *
     * @warning If elements are allocate dynamic they will need to be cleared
     * when cleaning the registry.
     * Change the destructor when registering the modules to avoid leaking
     * memory.
     *
     * @note If the matrix is allocated by another application the memory
     * should not be delete until the setup functions ran!
     * This memory is used by lower layers to convert the matrix!
     * It is safe to assume that it should be destroyed when the pipeline
     * does not update LSH anymore.
     *
     * Loaded using @ref LSH_FILE but could also be loaded with other means
     * (e.g. loading it from socket or compiled binary information)
     */
    LSH_GRID sGrid;
    /**
     * @brief Name of the loaded file if loaded from a file
     *
     * The grid can also be loaded without a filename by directly populating
     * the sGrid attribute.
     */
    std::string lshFilename;

public:  // methods
    ModuleLSH();
    /** @brief Destroys the @ref sGrid if it has allocated elements */
    virtual ~ModuleLSH();

    /** @brief To know if the grid has been loaded */
    bool hasGrid() const;

    /** @copydoc ISPC::SetupModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ISPC::SetupModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

    /**
     * @brief Load a grid from a filename
     *
     * @note updates lshFilename too
     *
     * @warning this does not call setup() nor requestUpdate()
     */
    IMG_RESULT loadMatrix(const std::string &filename);

    /** @brief Save the current matrix to a file */
    IMG_RESULT saveMatrix(const std::string &filename) const;

public:  // parameter
    static const ParamDefArray<double> LSH_GRADIENT_X;
    static const ParamDefArray<double> LSH_GRADIENT_Y;
    static const ParamDefSingle<bool> LSH_MATRIX;
    static const ParamDefSingle<std::string> LSH_FILE;

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_LSH_H_ */
