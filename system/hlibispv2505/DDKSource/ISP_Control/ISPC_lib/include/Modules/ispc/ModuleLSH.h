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
#include <vector>

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
     * @brief Enable/disable use of lens shading matrix
     *
     * Loaded using @ref LSH_MATRIX
     *
     * @warning This is not affecting the loading of matrices. But
     * applications can check that value to know if the high level parameters
     * want LSH to be enabled.
     *
     * To know if LSH is currently enabled check
     * @ref getCurrentMatrixId() != 0
     */
    bool bEnableMatrix;
    /**
     * @brief [delta per CFA] Horizontal linear shading gradient per channel
     * (always RGGB order)
     *
     * Loaded using @ref LSH_GRADIENT_X
     */
    double aGradientX[4];
    /**
     * @brief [delta per CFA] Vertical linear shading gradient per channel
     * (always RGGB order)
     *
     * Loaded using @ref LSH_GRADIENT_Y
     */
    double aGradientY[4];

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
	bool enable_infotm_method;
	int infotm_method;
	bool infotm_lsh_exchange_direction;
	bool infotm_lsh_flat_mode;

	float awb_convert_gain;
	OTP_CALIBRATION_DATA calibration_data;
#endif

protected:
    /**
     * @brief Current grid - 0 means none
     */
    IMG_UINT32 currentMatId;

    struct lsh_mat
    {
        LSH_GRID sGrid;
        std::string filename;
        IMG_UINT32 matrixId;
        double wbScale;

        lsh_mat();
        ~lsh_mat();
    };

    /* this could be storing a pointer to avoid copy constructing LSH_GRID
     * if we find out that it takes too long */
    std::list <lsh_mat*> list_grid;

    typedef std::list <lsh_mat*>::const_iterator const_iterator;
    typedef std::list <lsh_mat*>::iterator iterator;

    const_iterator findMatrix(IMG_UINT32 matrixId) const;
    iterator findMatrix(IMG_UINT32 matrixId);

    /**
     * Allocates a CI LSH buffer and converts the sGrid into it. If
     * successful information is added to the list_grid, list_filename and
     * list_matrixId.
     *
     * @warning CI_PIPELINE has to be registered when calling this function
     * (see @ref Pipeline::programPipeline() or @ref Camera::program())
     *
     * @param[in] sGrid containing a populated matrix - copied by value to the
     * list if successful
     * @param[out] out_matId CI LSH matrix Id
     * @param[in] filename where the LSH was loaded from - can be empty
     * @param[in] ui8BitsPerDiff number of bits to use when encoding the
     * difference (if 0 computes the minimum one).
     * @param[in] wbScale multiply current White Balance gains by this factor.
     * @note If the matrix has been scaled down to support the HW limitations
     * the wbScale can be used to compensate for the scaling when applying the
     * WBGains of the HW LSH module.
     *
     * Delegates to @ref updateCIMatrix().
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if CI functions failed
     * @return IMG_ERROR_UNEXPECTED_STATE if cannot access CI
     */
    IMG_RESULT loadMatrix(const LSH_GRID &sGrid, IMG_UINT32 &out_matId,
        const std::string &filename, double wbScale, IMG_UINT8 ui8BitsPerDiff);

    /**
     * @warning Assumes that pipeline and pipeline->getCIPipeline() are
     * non NULL and that IIF is setup in the MC_Pipeline
     *
     * @param pGrid
     * @param ui8BitsPerDiff if 0 is given will reuse the same number than
     * currently stored in CI LSH matrix
     * @param matId CI LSH matrix Id allocated
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if CI functions failed
     */
    IMG_RESULT updateCIMatrix(const LSH_GRID *pGrid,
        IMG_UINT8 ui8BitsPerDiff, IMG_UINT32 matId);

public:  // methods
    ModuleLSH();
    /** @brief Destroys the grids in the list */
    virtual ~ModuleLSH();

    /** @brief To know if the grid has been loaded */
    bool hasGrid() const;

    /** @brief Access to the number of loaded grid */
    IMG_UINT32 countGrid() const;

    /**
     * @brief get the matrixID at this offset in the list
     *
     * Can be used to crawl the list of matrices (e.g. to save all matrices).
     *
     * The matrixId should be stored when allocating the matrix instead of
     * crawled using that function.
     *
     * @return 0 if the index does not fit in the list or return the matrixId
     */
    IMG_UINT32 getMatrixId(const unsigned int m) const;

    /**
     * @brief Return currently used matrix ID. If 0 none are in used.
     */
    IMG_UINT32 getCurrentMatrixId() const;

    /**
     * @brief if enable returns the current matrix's scaleWB.
     * Otherwise returns 1.0
     */
    double getCurrentScaleWB() const;

    /**
     * @copydoc ISPC::SetupModule::load()
     *
     * @note Matrices are not loaded when calling this function. Use
     * @ref loadMatrices() to allocate LSH matrices and load them from the
     * parameters or load them manually using @ref addMatrix()
     */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /**
     * @copydoc ISPC::SetupModule::save()
     *
     * @note Matrices are not saved when calling this function. Use
     * @ref saveMatrix() to save the desired matrices.
     *
     * @warning Saving the current matrix file using
     * @ref ControlLSH::LSH_FILE_S without the numbering system
     */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ISPC::SetupModule::setup() */
    virtual IMG_RESULT setup();

    /**
     * @brief Load all matrices present in the configuration file using
     * @ref ControlLSH parameters
     *
     * Delegates to @ref ControlLSH::loadMatrices()
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_CANCELLED if ControlLSH::loadMatrices() did not load
     * any matrix
     * @return IMG_ERROR_FATAL if ControlLSH::loadMatrices() failed or if
     * loading the matrix in the module failed.
     * @return IMG_ERROR_NOT_SUPPORTED if failed to compute a valid bits per
     * diff value
     */
    IMG_RESULT loadMatrices(const ParameterList &parameters,
        std::vector<IMG_UINT32> &matrixIdList);

    /**
     * @brief Load a grid from a file and add it to the list
     *
     * @warning this does not call setup() nor requestUpdate()
     *
     * @param[in] filename
     * @param[out] matrixId
     * @param[in] wbScale
     * @param[in] ui8BitsPerDiff optional bits per difference to use. See
     * @ref loadMatrix() parameters.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if loading of the file failed
     * Delegates to @ref loadMatrix().
     */
    IMG_RESULT addMatrix(const std::string &filename, IMG_UINT32 &matrixId,
        double wbScale = 1.0, IMG_UINT8 ui8BitsPerDiff = 0);

    /**
     * @brief Add a populated grid to the list.
     *
     * @warning this does not call setup() nor requestUpdate()
     *
     * @param[in] sGrid loaded matrix
     * @param[out] matrixId
     * @param[in] wbScale 
     * @param[in] ui8BitsPerDiff optional bits per difference to use. See
     * @ref loadMatrix() parameters.
     *
     * @return IMG_SUCCESS
     * Delegates to @ref loadMatrix().
     */
    IMG_RESULT addMatrix(const LSH_GRID &sGrid, IMG_UINT32 &matrixId,
        double wbScale= 1.0, IMG_UINT8 ui8BitsPerDiff = 0);

    /**
     * @brief Remove a matrix from all the lists
     *
     * @warning Remove the currently used matrix is possible (CI will ensure
     * that the pipeline is not started). In that case currentMatId becomes 0.
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if matrixId is not found
     * @return IMG_ERROR_FATAL if deregistering from CI failed
     * @return IMG_ERROR_UNEXPECTED_STATE if cannot access CI
     * - the list are cleared anyway.
     */
    IMG_RESULT removeMatrix(IMG_UINT32 matrixId);

    /**
     * @brief Setup the pipeline to use this matrix
     *
     * Does not update the content of the matrix
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if matrixId is not found
     * @return IMG_ERROR_FATAL if update LSH from CI failed
     * @return IMG_ERROR_UNEXPECTED_STATE if cannot access CI
     */
    IMG_RESULT configureMatrix(IMG_UINT32 matrixId);

    /**
     * @brief Get the grid associated with matrix ID
     *
     * @note This does not reserve the CI matrix associated with matrixId
     *
     * If sizes are changed as well as content the associated CI matrix may
     * not be big enough and the convertion could fail.
     *
     * If changes are made they can be applied to the CI matrix using
     * @ref updateGrid()
     *
     * @warning This is not const because LSH_GRID is a C struct
     * so addresses are copied not reallocated. Therefore changing the
     * grid content will change what is stored!
     *
     * @return the found grid or an empty structure
     */
    LSH_GRID* getGrid(IMG_UINT32 matrixId);

    /**
     * @brief Update the CI matrix from the stored LSH_GRID
     *
     * If convertion fail or grid does not fit in the matrix anymore
     * the CI matrix may not be updated
     *
     * @param matrixId to update
     * @param ui8BitsPerDiff to use - if 0 is given does not change
     *
     * Delegates to @ref updateCIMatrix()
     */
    IMG_RESULT updateGrid(IMG_UINT32 matrixId, IMG_UINT8 ui8BitsPerDiff = 0);

    /**
     * @brief Access to the filename associated with a grid - may be an empty
     * string if was added with a LSH_GRID structure directly
     */
    std::string getFilename(IMG_UINT32 matrixId) const;

    /** @brief Save the selected matrix to a file */
    IMG_RESULT saveMatrix(IMG_UINT32 matrixId,
        const std::string &filename) const;

public:  // parameter
    static const ParamDefArray<double> LSH_GRADIENT_X;
    static const ParamDefArray<double> LSH_GRADIENT_Y;
    static const ParamDefSingle<bool> LSH_MATRIX;
#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
	static const ParamDefSingle<bool> LSH_ENABLE_INFOTM_METHOD;
	static const ParamDef<int> LSH_INFOTM_METHOD;
	static const ParamDefSingle<bool> INFOTM_LSH_EXCHANGE_DIRECTION;
	static const ParamDefSingle<bool> INFOTM_LSH_FLAT_MODE;

	static const ParamDef<IMG_INT32> LSH_BIG_RING_RADIUS;
	static const ParamDef<IMG_INT32> LSH_SMALL_RING_RADIUS;
	static const ParamDef<IMG_INT32> LSH_MAX_RADIUS;
	static const ParamDef<IMG_FLOAT> AWB_CORRECTION_DATA_PULL_UP_GAIN;
	static const ParamDef<IMG_FLOAT> LSH_L_CORRECTION_DATA_PULLUP_GAIN;
	static const ParamDef<IMG_FLOAT> LSH_R_CORRECTION_DATA_PULLUP_GAIN;
	static const ParamDefArray<int> LSH_RADIUS_CENTER_OFFSET_L;
	static const ParamDefArray<int> LSH_RADIUS_CENTER_OFFSET_R;

    static const ParamDef<IMG_FLOAT> LSH_CURVE_BASE_VAL_L;
    static const ParamDef<IMG_FLOAT> LSH_CURVE_BASE_VAL_R;
#endif

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_MODULE_LSH_H_ */
