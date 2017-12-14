/**
*******************************************************************************
@file ControlLSH.cpp

@brief ISPC::ControlLSH implementation

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
#include "ispc/ControlLSH.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CONTROL_LSH"

#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ostream>

#include "ispc/ISPCDefs.h"
#include "ispc/Module.h"
#include "ispc/ModuleLSH.h"
#include "ispc/Pipeline.h"
#include "ispc/Save.h"
#include "mc/module_config.h"

#define LSH_MAX_TEMP (100000)

// there are no real max, 200 is arbitrary
const ISPC::ParamDef<IMG_INT32> ISPC::ControlLSH::LSH_CORRECTIONS(
    "LSH_CTRL_CORRECTIONS", 0, 200, 0);
const ISPC::ParamDefSingle<std::string> ISPC::ControlLSH::LSH_FILE_S(
    "LSH_CTRL_FILE", "");
// printed in default instead of empty string
static const std::string LSH_CTRL_FILE_DEF = "<matrix file>";

// set minimum and default to 0 Kelvin which is impossible
const ISPC::ParamDef<IMG_UINT32>
ISPC::ControlLSH::LSH_CTRL_TEMPERATURE_S("LSH_CTRL_TEMPERATURE",
0, LSH_MAX_TEMP, 0);

const ISPC::ParamDef<double>
ISPC::ControlLSH::LSH_CTRL_SCALE_WB_S("LSH_CTRL_SCALE_WB",
0, 100.0, 1.0);

/* actually min=4, max=10, but set min to 0 is used as trigger to calculate
 * it by ControlLSH::findBiggestBitsPerDiff()
 * set default to 0 too so that it computes it if not present
 */
const ISPC::ParamDef<IMG_INT32> ISPC::ControlLSH::LSH_CTRL_BITS_DIFF(
    "LSH_CTRL_BITS_DIFF", 0, LSH_DELTA_BITS_MAX, 0);

ISPC::ControlLSH::GridInfo::GridInfo(const std::string &filename,
    double scaledWb)
    : matrixId(0), filename(filename), scaledWB(scaledWb)
{
}

ISPC::ControlLSH::GridInfo::GridInfo(const ISPC::ParameterList &params, int i)
{
    std::ostringstream parameterName;

    if (params.exists(LSH_FILE_S.indexed(i).name))
    {
        filename =
            params.getParameter(LSH_FILE_S.indexed(i));
        LOG_DEBUG("Found LSH matrix: %s\n", filename.c_str());
    }
    if (!filename.empty())
    {
        scaledWB = params.getParameter(LSH_CTRL_SCALE_WB_S.indexed(i));
    }
}

ISPC::ControlLSH::ControlLSH(const std::string &logtag)
    : ControlModuleBase(logtag),
    illuminantTemperature(6500),
    algorithm(LINEAR),
    chosenMatrixId(0),
    pCtrlAWB(NULL),
    maxBitsPerDiff(0)
{
}

ISPC::ControlLSH::~ControlLSH()
{
}

// pass LSH matrices to ModuleLSH as file names
IMG_RESULT ISPC::ControlLSH::load(const ParameterList &parameters)
{
    IMG_RESULT ret = IMG_ERROR_CANCELLED;
    IMG_UINT16 i = 0, num_matricies = 0;
    IMG_TEMPERATURE_TYPE temperature = 0;
    std::stringstream parameterName;
    std::string fileName;

    // clear the object grids
    this->grids.clear();

    ret = loadMatrices(parameters, grids, maxBitsPerDiff);
    /* loadMatrices() returns IMG_ERROR_CANCELLED when no matrix
     * are present in parameter list */
    if (grids.size() == 0)
    {
        return IMG_SUCCESS;
    }
    if (IMG_SUCCESS != ret)
    {
        return ret;
    }

    if (maxBitsPerDiff < LSH_DELTA_BITS_MIN)
    {
        maxBitsPerDiff = findBiggestBitsPerDiff(grids);
        MOD_LOG_WARNING("Calculated %s %d. Consider to use this value in "
                "configuration file.\n",
                (LSH_CTRL_BITS_DIFF.name).c_str(),
                maxBitsPerDiff);
    }
    if (maxBitsPerDiff < LSH_DELTA_BITS_MIN
        || maxBitsPerDiff > LSH_DELTA_BITS_MAX)
    {
        MOD_LOG_ERROR("invalid bits per diff %d selected (min %d, max %d)\n",
            (int)maxBitsPerDiff, LSH_DELTA_BITS_MIN, LSH_DELTA_BITS_MAX);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    // load matricies to ModuleLSH
    {
        ModuleLSH *lsh = NULL;
        Pipeline *pipeline = getPipelineOwner();

        if (pipeline)
        {
            lsh = pipeline->getModule<ModuleLSH>();
        }
        else
        {
            MOD_LOG_ERROR("ControlLSH has no pipeline owner! "\
                "Cannot load deshading matrices.\n");
            return IMG_ERROR_NOT_INITIALISED;
        }

        if (lsh)
        {
            IMG_UINT32 matrixId;
            iterator it;
            for (it = grids.begin(); it != grids.end(); it++)
            {
                ret = lsh->addMatrix(it->second.filename, matrixId,
                    it->second.scaledWB, maxBitsPerDiff);

                if (IMG_SUCCESS != ret)
                {
                    MOD_LOG_ERROR("Failed to load matrix for T=%d\n",
                        it->first);
                    return IMG_ERROR_FATAL;
                }
                else
                {
                    it->second.matrixId = matrixId;
                    MOD_LOG_DEBUG("Loaded matrix from %s for temp %d\n",
                        it->second.filename.c_str(), it->first);
                }
            }
            ret = IMG_SUCCESS;
        }
        else
        {
            MOD_LOG_ERROR("ControlLSH can't find ModuleLSH!\n");
            ret = IMG_ERROR_NOT_INITIALISED;
        }
    }

    return ret;
}

IMG_RESULT ISPC::ControlLSH::addMatrixInfo(IMG_TEMPERATURE_TYPE temp,
    const GridInfo &info)
{
    iterator it;

    it = grids.find(temp);

    if (it == grids.end())
    {
        // check that provided grid info has a valid matrixId
        ModuleLSH *lsh = NULL;
        Pipeline *pipeline = getPipelineOwner();

        if (pipeline)
        {
            lsh = pipeline->getModule<ModuleLSH>();
        }
        else
        {
            MOD_LOG_ERROR("ControlLSH has no pipeline owner! "\
                "Cannot load deshading matrices.\n");
            return IMG_ERROR_NOT_INITIALISED;
        }

        if (lsh->getGrid(info.matrixId) == NULL)
        {
            MOD_LOG_ERROR("cannot find matrix %d in LSH module\n",
                info.matrixId);
            return IMG_ERROR_INVALID_PARAMETERS;
        }

        grids[temp] = info;

        return IMG_SUCCESS;
    }
    MOD_LOG_ERROR("temperatur %d already present\n", temp);
    return IMG_ERROR_ALREADY_INITIALISED;
}

IMG_RESULT ISPC::ControlLSH::addMatrix(IMG_TEMPERATURE_TYPE temp,
    const LSH_GRID &sGrid, IMG_UINT32 &matrixId,
    double wbScale, IMG_UINT8 ui8BitsPerDiff)
{
    ModuleLSH *lsh = NULL;
    Pipeline *pipeline = getPipelineOwner();
    IMG_RESULT ret;
    GridInfo info("", wbScale);
    iterator it;

    if (pipeline)
    {
        lsh = pipeline->getModule<ModuleLSH>();
    }
    else
    {
        MOD_LOG_ERROR("ControlLSH has no pipeline owner! "\
            "Cannot load deshading matrices.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    /* check before calling ModuleLSH::addMatrix() so that later
     * addMatrixInfo() does not fail. Because if addMatrix() succeed ModuleLSH
     * owns the matrix and callilng removeMatrix() will delete it and the
     * user will be unsure if it has to free the matrix or not */
    it = grids.find(temp);
    if (grids.end() != it)
    {
        MOD_LOG_ERROR("Temperature %d is already registered\n", temp);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    ret = lsh->addMatrix(sGrid, info.matrixId, wbScale, ui8BitsPerDiff);
    if (IMG_SUCCESS != ret)
    {
        MOD_LOG_ERROR("failed to add matrix to Module LSH\n");
        return IMG_ERROR_FATAL;
    }
    
    ret = addMatrixInfo(temp, info);
    if (IMG_SUCCESS != ret)
    {
        MOD_LOG_ERROR("failed to add matrix information to Control LSH\n");
        ret = lsh->removeMatrix(info.matrixId);
        if (IMG_SUCCESS != ret)
        {
            MOD_LOG_ERROR("failed to remove matrix %d previously added\n",
                info.matrixId);
        }
        matrixId = 0;
        return IMG_ERROR_CANCELLED;
    }
    matrixId = info.matrixId;
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlLSH::addMatrix(IMG_TEMPERATURE_TYPE temp,
    const std::string &filename, IMG_UINT32 &matrixId,
    double wbScale, IMG_UINT8 ui8BitsPerDiff)
{
    LSH_GRID sGrid = LSH_GRID();
    IMG_RESULT ret;

    MOD_LOG_DEBUG("loading LSH matrix from %s\n", filename.c_str());

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
	Pipeline *pipeline = getPipelineOwner();
	ModuleLSH* pLsh = pipeline->getModule<ModuleLSH>();

	if (pLsh->enable_infotm_method)
	{
		int direction[2] = {0, 1};
        
        if (pLsh->infotm_lsh_exchange_direction)
        {
    		direction[0] = 1;
    		direction[1] = 0;
        }
        
		pLsh->calibration_data.sensor_calibration_data_L = pipeline->getSensor()->ReadSensorCalibrationData(pLsh->infotm_method, direction[0], pLsh->awb_convert_gain, &pLsh->calibration_data.version);
		
		if (pLsh->calibration_data.sensor_calibration_data_L == NULL)
		{
			pLsh->calibration_data.sensor_calibration_data_R = NULL;
		}
		else
		{
			pLsh->calibration_data.sensor_calibration_data_R = pipeline->getSensor()->ReadSensorCalibrationData(pLsh->infotm_method, direction[1], pLsh->awb_convert_gain, NULL);
		}
        
        if (pLsh->infotm_method == 0 || pLsh->infotm_method == 1) // lipin & baikangxing otp
        {
    		if (pLsh->calibration_data.version&0xFF00 == 0x1500)
    		{
    			pLsh->calibration_data.sensor_calibration_data_L = NULL;
    			pLsh->calibration_data.sensor_calibration_data_R = NULL;
    		}	
    		else if (pLsh->calibration_data.version&0xFF00 == 0x1400)
    		{
    			LIPIN_OTP_VERSION_1_4* v1_4_cali_struct = (LIPIN_OTP_VERSION_1_4*)pLsh->calibration_data.sensor_calibration_data_L;
    			pLsh->calibration_data.radius_center_offset_L[0] = (0x7FF&v1_4_cali_struct->radius_center_offset[0])*((0x8000&v1_4_cali_struct->radius_center_offset[0]) ? 1:-1);
    			pLsh->calibration_data.radius_center_offset_L[1] = (0x7FF&v1_4_cali_struct->radius_center_offset[1])*((0x8000&v1_4_cali_struct->radius_center_offset[1]) ? 1:-1);
    			v1_4_cali_struct = (LIPIN_OTP_VERSION_1_4*)pLsh->calibration_data.sensor_calibration_data_R;
    			pLsh->calibration_data.radius_center_offset_R[0] = (0x7FF&v1_4_cali_struct->radius_center_offset[0])*((0x8000&v1_4_cali_struct->radius_center_offset[0]) ? 1:-1);
    			pLsh->calibration_data.radius_center_offset_R[1] = (0x7FF&v1_4_cali_struct->radius_center_offset[1])*((0x8000&v1_4_cali_struct->radius_center_offset[1]) ? 1:-1);
    		}
        }
#ifdef INFOTM_E2PROM_METHOD
	    else if (pLsh->infotm_method == 2) // e2prom
	    {
	        printf("===>e2prom data load done and cala center offset\n");
			E2PROM_DATA* e2prom_data = (E2PROM_DATA*)pLsh->calibration_data.sensor_calibration_data_L;
			pLsh->calibration_data.radius_center_offset_L[0] = ((e2prom_data->center_offset[0]&0xFF00) == 0 ? -1:1)*(e2prom_data->center_offset[0]&0x00FF);
			pLsh->calibration_data.radius_center_offset_L[1] = ((e2prom_data->center_offset[1]&0xFF00) == 0 ? -1:1)*(e2prom_data->center_offset[1]&0x00FF);
            printf("==>L offset<%x, %x>\n", e2prom_data->center_offset[0], e2prom_data->center_offset[1]);
			e2prom_data = (E2PROM_DATA*)pLsh->calibration_data.sensor_calibration_data_R;
			pLsh->calibration_data.radius_center_offset_R[0] = ((e2prom_data->center_offset[0]&0xFF00) == 0 ? -1:1)*(e2prom_data->center_offset[0]&0x00FF);
			pLsh->calibration_data.radius_center_offset_R[1] = ((e2prom_data->center_offset[1]&0xFF00) == 0 ? -1:1)*(e2prom_data->center_offset[1]&0x00FF); 
            printf("==>R offset<%x, %x>\n", e2prom_data->center_offset[0], e2prom_data->center_offset[1]);
            printf("===>calibration_data.sensor_calibration_data_L %x, calibration_data.sensor_calibration_data_R %x, offsetL[%d, %d], offsetR[%d, %d]\n", 
	        pLsh->calibration_data.sensor_calibration_data_L, 
	        pLsh->calibration_data.sensor_calibration_data_R,
	        pLsh->calibration_data.radius_center_offset_L[0], 
	        pLsh->calibration_data.radius_center_offset_L[1],
	        pLsh->calibration_data.radius_center_offset_R[0],
	        pLsh->calibration_data.radius_center_offset_R[1]);
	    }
#endif
        else if (pLsh->infotm_method == 3) // xc9080 + other sensors
        {
            pLsh->calibration_data.sensor_calibration_data_L = NULL;
            pLsh->calibration_data.sensor_calibration_data_R = NULL;
            pLsh->infotm_method = -1;            
        }
        else
        {
            pLsh->calibration_data.sensor_calibration_data_L = NULL;
            pLsh->calibration_data.sensor_calibration_data_R = NULL;
            pLsh->infotm_method = -1;
        }
	}
	else
	{
		pLsh->calibration_data.sensor_calibration_data_L = NULL;
		pLsh->calibration_data.sensor_calibration_data_R = NULL;
		pLsh->infotm_method = -1;
	}
	ret = LSH_Load_bin(&(sGrid), filename.c_str(), pLsh->infotm_method, &pLsh->calibration_data);
#else
	ret = LSH_Load_bin(&(sGrid), filename.c_str());
#endif

    if (ret)
    {
        int c;
        MOD_LOG_WARNING("Failed to load the LSH matrix %s - no matrix "\
            "will be loaded\n", filename.c_str());
        for (c = 0; c < LSH_GRADS_NO; c++)
        {
            if (sGrid.apMatrix[c] != NULL)
            {
                IMG_FREE(sGrid.apMatrix[c]);
                sGrid.apMatrix[c] = NULL;
            }
        }
        return IMG_ERROR_FATAL;
    }

    ret = addMatrix(temp, sGrid, matrixId, wbScale, ui8BitsPerDiff);
    if (IMG_SUCCESS != ret)
    {
        // if IMG_ERROR_CANCELLED then already deleted!
        if (IMG_ERROR_CANCELLED != ret)
        {
            LSH_Free(&sGrid);
        }
    }
    return ret;
}

IMG_RESULT ISPC::ControlLSH::removeMatrix(IMG_UINT32 matrixId)
{
    // check that provided grid info has a valid matrixId
    ModuleLSH *lsh = NULL;
    Pipeline *pipeline = getPipelineOwner();
    iterator it;
    IMG_RESULT ret;

    if (pipeline)
    {
        lsh = pipeline->getModule<ModuleLSH>();
    }
    else
    {
        MOD_LOG_ERROR("ControlLSH has no pipeline owner! "\
            "Cannot load deshading matrices.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (lsh->getGrid(matrixId) == NULL)
    {
        MOD_LOG_ERROR("cannot find matrix %d in LSH module\n",
            matrixId);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    it = findMatrix(matrixId);

    if (grids.end() == it)
    {
        MOD_LOG_ERROR("cannot find matrix %d in LSH control\n", matrixId);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = lsh->removeMatrix(matrixId);
    if (IMG_SUCCESS != ret)
    {
        MOD_LOG_ERROR("failed to remove matrix %d from Module LSH\n",
            matrixId);
        return ret;
    }
    grids.erase(it);

    if (chosenMatrixId == matrixId)
    {
        MOD_LOG_DEBUG("chosenMatrixId is removed matrix: becomes 0\n");
        chosenMatrixId = 0;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlLSH::loadMatrices(const ParameterList &parameters,
    std::map<IMG_TEMPERATURE_TYPE, GridInfo> &grids,
    IMG_UINT8 &maxBitsPerDiff)
{
    unsigned int i;
    IMG_TEMPERATURE_TYPE temperature;

    if (!parameters.exists(LSH_CORRECTIONS.name))
    {
        LOG_WARNING("Unable to load LSH corrections from parameters. "\
            "'%s' not defined\n", LSH_CORRECTIONS.name.c_str());
        return IMG_ERROR_CANCELLED;
    }
    else  // ready to load the parameters
    {
        maxBitsPerDiff = parameters.getParameter(LSH_CTRL_BITS_DIFF);
        IMG_UINT16 numCorrections = parameters.getParameter(LSH_CORRECTIONS);
        // Read every correction
        for (i = 0; i < numCorrections; i++)
        {
            GridInfo info(parameters, i);

            if (info.filename.empty())
            {
                LOG_WARNING("Correction %d does not have a matrix file "\
                    "- ignored\n", i);
            }
            else
            {
                temperature =
                    parameters.getParameter(LSH_CTRL_TEMPERATURE_S.indexed(i));
                if (LSH_CTRL_TEMPERATURE_S.def == temperature)
                {
                    LOG_WARNING("Correction %d doesn't have a "\
                        "matching temperature - ignored\n", i);
                }
                else
                {
                    grids[temperature] = info;
                }
            }
        }
    }

    return IMG_SUCCESS;
}

ISPC::ParameterGroup ISPC::ControlLSH::getGroup()
{
    ParameterGroup group;

    group.header = "// Lens shading correction parameters";

    group.parameters.insert(LSH_CTRL_BITS_DIFF.name);
    group.parameters.insert(LSH_CORRECTIONS.name);

    for (int i = 0; i < LSH_CORRECTIONS.max; i++)
    {
        group.parameters.insert(LSH_CTRL_TEMPERATURE_S.indexed(i).name);
        group.parameters.insert(LSH_FILE_S.indexed(i).name);
        group.parameters.insert(LSH_CTRL_SCALE_WB_S.indexed(i).name);
    }

    return group;
}

IMG_RESULT ISPC::ControlLSH::save(ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    IMG_UINT16 i;

    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ControlLSH::getGroup();
    }

    parameters.addGroup("ControlLSH", group);

    std::map<IMG_TEMPERATURE_TYPE, GridInfo>::const_iterator it;
    switch (t)
    {
    case SAVE_VAL:
        it = grids.begin();
        i = 0;
        while (it != grids.end())
        {
            parameters.addParameter(LSH_CTRL_TEMPERATURE_S.indexed(i), it->first);
            parameters.addParameter(LSH_FILE_S.indexed(i), it->second.filename);
            parameters.addParameter(LSH_CTRL_SCALE_WB_S.indexed(i), it->second.scaledWB);
            ++it;
            ++i;
        }
        parameters.addParameter(LSH_CORRECTIONS, int(i));
        parameters.addParameter(LSH_CTRL_BITS_DIFF, int(maxBitsPerDiff));
        break;
    case SAVE_MIN:
        i = 0;
        // for (i = 0; it != gridFiles.end(); it++, i++)
        {
            parameters.addParameterMin(LSH_CTRL_TEMPERATURE_S.indexed(i));
            parameters.addParameter(LSH_FILE_S.indexed(i), LSH_CTRL_FILE_DEF);
            parameters.addParameterMin(LSH_CTRL_SCALE_WB_S.indexed(i));
        }
        parameters.addParameterMin(LSH_CORRECTIONS);
        parameters.addParameterMin(LSH_CTRL_BITS_DIFF);
        break;
    case SAVE_MAX:
        i = 0;
        // for (i = 0; it != gridFiles.end(); it++, i++)
        {
            parameters.addParameterMax(LSH_CTRL_TEMPERATURE_S.indexed(i));
            parameters.addParameter(LSH_FILE_S.indexed(i), LSH_CTRL_FILE_DEF);
            parameters.addParameterMax(LSH_CTRL_SCALE_WB_S.indexed(i));
        }

        parameters.addParameterMax(LSH_CORRECTIONS);
        parameters.addParameterMax(LSH_CTRL_BITS_DIFF);
        break;
    case SAVE_DEF:
        i = 0;
        // for (i = 0; it != gridFiles.end(); it++, i++)
        {
            parameters.addParameterDef(LSH_CTRL_TEMPERATURE_S.indexed(i));
            parameters.addParameter(LSH_FILE_S.indexed(i), LSH_CTRL_FILE_DEF);
            parameters.getParameter(LSH_FILE_S.indexed(i).name)->setInfo(
                    getParameterInfo(LSH_FILE_S.indexed(i)));
            parameters.addParameterDef(LSH_CTRL_SCALE_WB_S.indexed(i));
        }

        parameters.addParameterDef(LSH_CORRECTIONS);
        parameters.addParameterDef(LSH_CTRL_BITS_DIFF);
        break;
    }

    return IMG_SUCCESS;
}

IMG_INT32 ISPC::ControlLSH::chooseMatrix(IMG_TEMPERATURE_TYPE temp,
    InterpolationAlgorithm algo) const
{
    std::map<IMG_TEMPERATURE_TYPE, GridInfo>::const_iterator it
        = grids.begin();
    IMG_TEMPERATURE_TYPE interpolatedVal = it->first;  // interpolated T value
    IMG_UINT32 useMatrix = it->second.matrixId;

    if (grids.size() == 0)
    {
        MOD_LOG_ERROR("Cannot choose matrix: no grids were loaded\n");
        return (-IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE);
    }
    else if (grids.size() == 1)
    {
        return static_cast<IMG_INT32>(useMatrix);
    }
    if (LINEAR == algo)
    {
        it++; // start from 2nd value
        for (; it != grids.end(); it++)
        {
            // if given temp is closer to the previous T in the map than
            // the actual one, use the previous T as result
            if (temp > (interpolatedVal + (it->first - interpolatedVal) / 2))
            {
                interpolatedVal = it->first;
                useMatrix = it->second.matrixId;
            }
            else
            {
                break;
            }
        }
    }
    else if (EXPONENTIAL == algo)
    {
        MOD_LOG_ERROR("Exponential not implemented\n");
        return (-IMG_ERROR_INVALID_PARAMETERS);
    }
    else
    {
        MOD_LOG_ERROR("Invalid algorithm\n");
        return (-IMG_ERROR_INVALID_PARAMETERS);
    }

    return static_cast<IMG_INT32>(useMatrix);
}

IMG_RESULT ISPC::ControlLSH::update(const Metadata &metadata)
{
    if (isEnabled())
    {
        IMG_INT32 matrixId = 0;

        readTemperatureFromAWB();
        matrixId = chooseMatrix(illuminantTemperature, algorithm);

        if (matrixId > 0)
        {
            MOD_LOG_DEBUG("Given T=%d chose LSH matrix %d for T=%d "\
                "(previously chosen = %d)\n",
                illuminantTemperature, matrixId,
                getTemperature(matrixId), chosenMatrixId);
            if (chosenMatrixId != matrixId)
            {
                chosenMatrixId = matrixId;
                programCorrection();
            }
        }
        else
        {
            MOD_LOG_WARNING("failed to choose matrix!\n");
        }
    }
    else
    {
        if (chosenMatrixId > 0)
        {
            chosenMatrixId = 0;
            MOD_LOG_DEBUG("disabling CtrlLSH\n");
            programCorrection();
        }
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlLSH::programCorrection()
{
    IMG_RESULT ret;

    ModuleLSH *lsh = NULL;
    if (getPipelineOwner())
    {
        lsh = getPipelineOwner()->getModule<ModuleLSH>();
    }
    if (lsh)
    {
        // configureMatrix() works only if one matrix was used before
        // pipeline start
        ret = lsh->configureMatrix(chosenMatrixId);
        if (ret)
        {
            MOD_LOG_WARNING("Cannot use LSH matrix %d\n",
                chosenMatrixId);
        }
        else
        {
            MOD_LOG_DEBUG("Use matrixId %d\n", chosenMatrixId);
        }
    }
    else
    {
        MOD_LOG_ERROR("Could not find LSH module in pipeline\n");
        return IMG_ERROR_NOT_SUPPORTED;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlLSH::configureStatistics()
{
    return IMG_SUCCESS;
}

bool ISPC::ControlLSH::hasConverged() const
{
    return IMG_SUCCESS;
}

IMG_UINT32 ISPC::ControlLSH::getChosenMatrixId(void) const
{
    return chosenMatrixId;
}

IMG_INT32 ISPC::ControlLSH::configureDefaultMatrix(void)
{
    ISPC::ModuleLSH *pLSH = NULL;

    if (getPipelineOwner())
    {
        pLSH = getPipelineOwner()->getModule<ISPC::ModuleLSH>();
    }
    if (pLSH)
    {
        IMG_UINT32 mat = pLSH->getCurrentMatrixId();

        if (mat == 0)
        {
            if (grids.size() > 0)
            {
                iterator it = grids.begin();
                IMG_RESULT ret;

                MOD_LOG_DEBUG("setting default matrix to 1st loaded one\n");

                mat = it->second.matrixId;
                ret = pLSH->configureMatrix(mat);

                if (IMG_SUCCESS != ret)
                {
                    MOD_LOG_ERROR("failed to set default matrix to %d", mat);
                    return -1;
                }
                chosenMatrixId = mat;
                return (IMG_INT32)mat;  // should always be positive
            }
            MOD_LOG_ERROR("cannot set default matrix without loading "\
                "matrices first\n");
            return -1;
        }
        return 0;  // matrix already applied
    }
    MOD_LOG_ERROR("Could not find LSH module in pipeline\n");
    return -1;
}

ISPC::ControlLSH::IMG_TEMPERATURE_TYPE
    ISPC::ControlLSH::getChosenTemperature(void) const
{
    if (chosenMatrixId == 0)
    {
        return 0;
    }
    return getTemperature(chosenMatrixId);
}

void ISPC::ControlLSH::registerCtrlAWB(ISPC::ControlAWB *pAWB)
{
    pCtrlAWB = pAWB;
}

bool ISPC::ControlLSH::isEnabled() const
{
    return enabled && pCtrlAWB != NULL;
}

void ISPC::ControlLSH::readTemperatureFromAWB(void)
{
    //#define CTRL_LSH_DUMMY_TEST
#ifdef CTRL_LSH_DUMMY_TEST
    static unsigned int temp = 1000;
    illuminantTemperature = temp;
    temp += 1000;
    temp %= 7000;
#else
    if (pCtrlAWB)
    {
        ISPC::ControlLSH::IMG_TEMPERATURE_TYPE temp =
            (ISPC::ControlLSH::IMG_TEMPERATURE_TYPE)
            pCtrlAWB->getMeasuredTemperature();
        illuminantTemperature = temp;
    }
#endif
}

ISPC::ControlLSH::iterator ISPC::ControlLSH::findMatrix(IMG_UINT32 matrixId)
{
    iterator it;
    for (it = grids.begin(); it != grids.end(); it++)
    {
        if (matrixId == it->second.matrixId)
        {
            return it;
        }
    }
    return it;
}

void ISPC::ControlLSH::getMatrixIds(std::vector<IMG_UINT32> &ids) const
{
    std::map<IMG_TEMPERATURE_TYPE, GridInfo>::const_iterator it;
    ids.clear();
    for (it = grids.begin(); it != grids.end(); it++)
    {
        ids.push_back(it->second.matrixId);
    }
}

std::ostream& ISPC::ControlLSH::printState(std::ostream &os) const
{
    os << SAVE_A1 << getLoggingName() << ":" << std::endl;

    os << SAVE_A2 << "config:" << std::endl;
    os << SAVE_A3 << "enabled = " << enabled << std::endl;
    if (pCtrlAWB)
    {
        os << SAVE_A3 << "pCtrlAWB = "
            << pCtrlAWB->getLoggingName() << std::endl;
    }
    else
    {
        os << SAVE_A3 << "pCtrlAWB = null" << std::endl;
    }

    os << SAVE_A2 << "state:" << std::endl;
    os << SAVE_A3 << "illuminantTemperature = "
        << illuminantTemperature << std::endl;
    os << SAVE_A3 << "chosenMatrixId = "
        << chosenMatrixId << std::endl;
    os << SAVE_A3 << "getTemperature = "
        << getTemperature(chosenMatrixId) << std::endl;
    return os;
}

ISPC::ControlLSH::IMG_TEMPERATURE_TYPE ISPC::ControlLSH::getTemperature(
    IMG_UINT32 id) const
{
    IMG_TEMPERATURE_TYPE temp = 0;
    std::map<IMG_TEMPERATURE_TYPE, GridInfo>::const_iterator it;

    for (it = grids.begin(); it != grids.end(); it++)
    {
        if (it->second.matrixId == id)
        {
            temp = it->first;
            break;
        }
    }

    if (grids.end() == it)
    {
        MOD_LOG_ERROR("Don't have corresponding temperature for matrix %d\n",
            id);
    }
    return temp;
}

IMG_UINT32 ISPC::ControlLSH::getMatrixId(IMG_TEMPERATURE_TYPE temp) const
{
    std::map<IMG_TEMPERATURE_TYPE, GridInfo>::const_iterator it;
    it = grids.find(temp);
    if (grids.end() != it)
    {
        return it->second.matrixId;
    }
    return 0;
}

double ISPC::ControlLSH::getScaleWB(IMG_TEMPERATURE_TYPE temp) const
{
    std::map<IMG_TEMPERATURE_TYPE, GridInfo>::const_iterator it;
    it = grids.find(temp);
    if (grids.end() != it)
    {
        return it->second.scaledWB;
    }
    return 0.0;
}

ISPC::ControlLSH::InterpolationAlgorithm ISPC::ControlLSH::getAlgorithm() const
{
    return algorithm;
}

IMG_UINT8 ISPC::ControlLSH::findBiggestBitsPerDiff(
    const std::map<IMG_TEMPERATURE_TYPE, GridInfo> &grids)
{
    IMG_RESULT ret = 0;
    IMG_UINT8 maxBitsPerDiff = 0, bitsPerDiff = 0;
    LSH_GRID grid;
#ifndef NDEBUG
    std::string worst;
#endif

    std::map<IMG_TEMPERATURE_TYPE, GridInfo>::const_iterator it;

    for (it = grids.begin(); it != grids.end(); it++)
    {
        grid = LSH_GRID();
#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
		ret = LSH_Load_bin(&(grid), (it->second).filename.c_str(), -1, NULL);
#else
        ret = LSH_Load_bin(&(grid), (it->second).filename.c_str());
#endif
        if (ret == IMG_SUCCESS)
        {
            bitsPerDiff = MC_LSHComputeMinBitdiff(&grid, NULL);
            maxBitsPerDiff = IMG_MAX_INT(bitsPerDiff, maxBitsPerDiff);
#ifndef NDEBUG
            if (bitsPerDiff == maxBitsPerDiff)
            {
                worst = it->second.matrixId;
            }
#endif
        }
        LSH_Free(&grid);
    }
#ifndef NDEBUG
    LOG_DEBUG("biggest bits per diff (%d) found in matrix %s\n",
        maxBitsPerDiff, worst.c_str());
#endif

    return maxBitsPerDiff;
}

IMG_UINT32 ISPC::ControlLSH::getLoadedGrids() const
{
    return grids.size();
}

