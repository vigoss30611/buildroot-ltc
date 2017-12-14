/**
*******************************************************************************
 @file ControlLSH.h

 @brief Lens shading control class. Derived from ControlModule
 class to implement generic control modules

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
#ifndef ISPC_CONTROL_LSH_H_
#define ISPC_CONTROL_LSH_H_

#include <string>
#include <ostream>

#include "ispc/Module.h"
#include "ispc/Parameter.h"
#include "ispc/ControlAWB.h"

namespace ISPC {

class ControlLSH: public ControlModuleBase<CTRL_LSH>
{
public:

    typedef IMG_UINT32 IMG_TEMPERATURE_TYPE;

    struct GridInfo {
        IMG_UINT32 matrixId;
        /** @brief associated filename, can be empty */
        std::string filename;
        double scaledWB;

        GridInfo(const std::string &filename = std::string(),
            double scaledWb = 1.0);
        GridInfo(const ParameterList &params, int i);
    };

    enum InterpolationAlgorithm{
        LINEAR,
        EXPONENTIAL
    };

    explicit ControlLSH(const std::string &logtag = "ISPC_CTRL_LSH");
    virtual ~ControlLSH();

    /**
     * see @ref ControlModule::load()
     *
     * @return IMG_SUCCESS (even if @ref LSH_CORRECTIONS is 0)
     * @return IMG_ERROR_NOT_INITIALISED if @ref getPipelineOwner() returns
     * NULL or if the owner does not have @ref ModuleLSH
     * @return IMG_ERROR_INVALID_PARAMETERS if @ref LSH_CORRECTIONS is
     * not found
     * @return IMG_ERROR_FATAL if failed to load a matrix filename
     * @return IMG_ERROR_NOT_SUPPORTED if the bits per diff loaded is not
     * valid
     */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /**
     * @brief Add information about a matrix already present in ModuleLSH
     *
     * @param[in] temp associated temperature
     * @param[in] info containing a valid matrixId and relevant scaledWB
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_INITIALISED if @ref getPipelineOwner() returns
     * NULL or if the owner does not have @ref ModuleLSH
     * @return IMG_ERROR_INVALID_PARAMETERS if info.matrixId is not found in
     * ModuleLSH
     * @return IMG_ERROR_ALREADY_INITIALISED if temp already has an entry in
     * map of grids.
     */
    IMG_RESULT addMatrixInfo(IMG_TEMPERATURE_TYPE temp, const GridInfo &info);

    /**
     * @brief Add a matrix data to ModuleLSH and its associated information
     * to the Control module.
     *
     * @param[in] temp associated temperature
     * @param[in] sGrid to give to @ref ModuleLSH::addMatrix()
     * @param[out] matrixId retrieved from @ref ModuleLSH::addMatrix()
     * @param[in] wbScale to give to @ref ModuleLSH::addMatrix()
     * @param[in] ui8BitsPerDiff to give to @ref ModuleLSH::addMatrix()
     *
     * Delegates to @ref ModuleLSH::addMatrix() and then @ref addMatrixInfo().
     *
     * If addMatrixInfo() failed then the matrix is removed using
     * @ref ModuleLSH::removeMatrix().
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_INITIALISED if @ref getPipelineOwner() returns
     * NULL or if the owner does not have @ref ModuleLSH
     * @return IMG_ERROR_NOT_SUPPORTED if the temp is already in the map
     * @return IMG_ERROR_FATAL if @ref ModuleLSH::addMatrix() failed
     * @return IMG_ERROR_CANCELLED if @ref addMatrixInfo() failed
     * @warning If returning IMG_ERROR_CANCELLED then the matrix was added
     * to ModuleLSH and then removed: the given matrix has been deleted!
     */
    IMG_RESULT addMatrix(IMG_TEMPERATURE_TYPE temp,
        const LSH_GRID &sGrid, IMG_UINT32 &matrixId,
        double wbScale = 1.0, IMG_UINT8 ui8BitsPerDiff = 0);

    /**
    * @brief Add a matrix data to ModuleLSH and its associated information
    * to the Control module.
    *
    * @param[in] temp associated temperature
    * @param[in] filename to give to @ref ControlLSH::addMatrix()
    * @param[out] matrixId retrieved from @ref ControlLSH::addMatrix()
    * @param[in] wbScale to give to @ref ControlLSH::addMatrix()
    * @param[in] ui8BitsPerDiff to give to @ref ControlLSH::addMatrix()
    *
    * Delegates to @ref ControlLSH::addMatrix() after opening the the file
    * with @ref LSH_Load_bin().
    *
    * @return IMG_SUCCESS
    * @return IMG_ERROR_FATAL if failed to load the file
    * @return code from @ref ControlLSH::addMatrix()
    */
    IMG_RESULT addMatrix(IMG_TEMPERATURE_TYPE temp,
        const std::string &filename, IMG_UINT32 &matrixId,
        double wbScale = 1.0, IMG_UINT8 ui8BitsPerDiff = 0);

    /**
     * @brief Removes both the matrix from ModuleLSH and the information held
     * by the control module.
     *
     * Delegates the destruction of the matrix to
     * @ref ModuleLSH::removeMatrix()
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_NOT_INITIALISED if @ref getPipelineOwner() returns
     * NULL or if the owner does not have @ref ModuleLSH
     * @return IMG_ERROR_INVALID_PARAMETERS if matrixId is not found in
     * ModuleLSH or in the map of grids
     * @return code from @ref ModuleLSH::removeMatrix()
     */
    IMG_RESULT removeMatrix(IMG_UINT32 matrixId);

    /**
     * @brief Loads several LSH matrices from the parameter list.
     *
     * @param[in] parameters
     * @param[out] grids - the list is NOT cleared before loading. All matrices
     * found will be added to it.
     * @param[out] maxBitsPerDiff found in parameter list. Can return an
     * invalid value that means the bits per diff should be computed using
     * @ref findBiggestBitsPerDiff()
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_CANCELLED if there is no matrix to load
     * (LSH_CORRECTIONS parameter not found).
     */
    static IMG_RESULT loadMatrices(const ParameterList &parameters,
        std::map<IMG_TEMPERATURE_TYPE, GridInfo> &grids,
        IMG_UINT8 &maxBitsPerDiff);

    /**
     * @brief Finds biggest bit/diff ratio needed by HW to encode LSH matrices
     * available in @ref grids
     *
     * @return biggest bits per diff or 0 in case of error
     *
     * @note Use it only to find out a value, then save it to configuration
     * file as LSH_CTRL_BITS_DIFF parameter. If this parameter is already
     * defined (and correct), method should not be called, as it duplicates
     * reads from LSH matrices files.
     */
    static IMG_UINT8 findBiggestBitsPerDiff(
        const std::map<IMG_TEMPERATURE_TYPE, GridInfo> &grids);

    /**
     * See @ref ControlModule::save()
     */
    virtual IMG_RESULT save(ParameterList &parameters,
        ModuleBase::SaveType t) const;

    /**
     * @copydoc ControlModule::update()
     *
     * This function gets the temperature from the registered AWB and then
     * sets in the HW deshading grid which coresponds to
     * illuminant temperature.
     */
    virtual IMG_RESULT update(const Metadata &metadata);

    /**
     * @warning this algorithm does not allow to know if convergence is
     * reached
     */
    virtual bool hasConverged() const;

    /**
     * @brief Get the chosen matrix Id using AWB temperature. 0 means none.
     *
     * The chosen matrix is applied when @ref programCorrection() is called.
     *
     * The currently applied matrix is available using
     * @ref ISPC::ModuleLSH::getCurrentMatrixId()
     */
    IMG_UINT32 getChosenMatrixId(void) const;

    /**
     * @brief If no matrix are applied try to apply a default matrix from the
     * current list.
     *
     * This should be used before starting the capture to ensure that the HW
     * has an enabled grid. On hw 2.x if the capture is started without an
     * enabled grid then the control LSH cannot change the grid.
     *
     * @return 0 if a grid is already applied
     * @return <0 on failure to apply a default (no grid, or failure to apply
     * the grid)
     * @return matrix ID used
     */
    IMG_INT32 configureDefaultMatrix(void);

    /** 
     * @brief Returns the temperature relevant to chosen deshading matrix.
     * If deshading is disabled returned value is 0.
     */
    IMG_TEMPERATURE_TYPE getChosenTemperature(void) const;

    /**
     * @brief Provides Control AWB module pointer, used later for
     * illuminant temperature polling
     *
     * @see readTemperatureFromAWB()
     */
    void registerCtrlAWB(ISPC::ControlAWB *pAWB);

    /**
     * @brief Return true if both: Control is enabled with
     * @ref enableControl() and @ref registerCtrlAWB() was used to provide
     * a non-NULL pointer
     */
    virtual bool isEnabled() const;

    /** @brief Store LSH matrices IDs which have been loaded before */
    void getMatrixIds(std::vector<IMG_UINT32> &ids) const;

    virtual std::ostream& printState(std::ostream &os) const;

    /** @brief get a temperature for a given matrix Id - or 0 */
    IMG_TEMPERATURE_TYPE getTemperature(IMG_UINT32 matrixId) const;

    /* @brief returns matrixId for the temperature - or 0 */
    IMG_UINT32 getMatrixId(IMG_TEMPERATURE_TYPE temp) const;

    /* @brief returns scaleWB for the temperature - or 0.0 */
    double getScaleWB(IMG_TEMPERATURE_TYPE temp) const;

    InterpolationAlgorithm getAlgorithm() const;

    /** @brief returns the number of loaded grids */
    IMG_UINT32 getLoadedGrids() const;

private:
    IMG_TEMPERATURE_TYPE illuminantTemperature;

    /** @brief maps temperature => file_with_LSH_correction_matrix */
    std::map<IMG_TEMPERATURE_TYPE, GridInfo> grids;
    typedef std::map<IMG_TEMPERATURE_TYPE, GridInfo>::iterator iterator;

    InterpolationAlgorithm algorithm;

    /** @brief matrix to be programmed when calling @ref programCorrection() */
    IMG_UINT32 chosenMatrixId;

    /**
     * @brief used to update temperature every frame
     *
     * @see readTemperatureFromAWB()
     */
    ISPC::ControlAWB *pCtrlAWB;
    /**
     * @brief How many bits are necessarry to differentially encode all LSH
     * matrices in memory. 0 means that this class will calculate it itself
     * (although it's not optimal solution regarding IO reads).
     * */
    IMG_UINT8 maxBitsPerDiff;

    /**
     * @brief choose the matrixId to use according to the temperature
     * @return matrixId or negative number in case invalid algorithms is given
     */
    IMG_INT32 chooseMatrix(IMG_TEMPERATURE_TYPE temp,
            InterpolationAlgorithm algo) const;

    /** @brief Uses @ref pCtrlAWB to update @ref illuminantTemperature */
    void readTemperatureFromAWB(void);

    /** @brief find the entry in @ref grids with given matrixId */
    iterator findMatrix(IMG_UINT32 matrixId);

protected:
    /**
     * @see ControlModule::configureStatistics()
     *
     * Not used
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * Programs the @ref chosenMatrixId as the matrix to use in
     * the owner's @ref ModuleLSH
     */
    virtual IMG_RESULT programCorrection();

public:  // parameter
    static const ParamDef<IMG_INT32> LSH_CORRECTIONS;
    // one per found configuration
    static const ParamDefSingle<std::string> LSH_FILE_S;
    // one per found configuration
    static const ParamDef<IMG_UINT32> LSH_CTRL_TEMPERATURE_S;
    // one per found configuration
    static const ParamDef<double> LSH_CTRL_SCALE_WB_S;
    // one for all matrices
    static const ParamDef<IMG_INT32> LSH_CTRL_BITS_DIFF;

    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();
};

}  // namespace ISPC

#endif /*ISPC_CONTROL_LSH_H_*/
