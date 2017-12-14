/**
*******************************************************************************
@file ModuleLSH.cpp

@brief Implementation of ISPC::ModuleLSH

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
#include "ispc/ModuleLSH.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_LSH"

#include <string>
#include <vector>

#include "ispc/Pipeline.h"
#include "ispc/ControlLSH.h"
#include "ispc/ModuleWBC.h"
#include "ispc/PerfTime.h"

#include <ci/ci_api.h>

#define LSH_GRADIENT_MIN -4.0f
#define LSH_GRADIENT_MAX 4.0f

static const double LSH_GRADIENT_DEF[LSH_GRADS_NO] = {0.0, 0.0, 0.0, 0.0};
IMG_STATIC_ASSERT(LSH_GRADS_NO == 4, LSH_GRADS_NO_CHANGED)
const ISPC::ParamDefArray<double> ISPC::ModuleLSH::LSH_GRADIENT_X(
    "LSH_GRADIENTX", LSH_GRADIENT_MIN, LSH_GRADIENT_MAX, LSH_GRADIENT_DEF,
    LSH_GRADS_NO);
const ISPC::ParamDefArray<double> ISPC::ModuleLSH::LSH_GRADIENT_Y(
    "LSH_GRADIENTY", LSH_GRADIENT_MIN, LSH_GRADIENT_MAX, LSH_GRADIENT_DEF,
    LSH_GRADS_NO);

const ISPC::ParamDefSingle<bool> ISPC::ModuleLSH::LSH_MATRIX(
    "LSH_MATRIX_ENABLE", false);

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
const ISPC::ParamDefSingle<bool> ISPC::ModuleLSH::INFOTM_LSH_EXCHANGE_DIRECTION("INFOTM_LSH_EXCHANGE_DIRECTION", false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleLSH::INFOTM_LSH_FLAT_MODE("INFOTM_LSH_FLAT_MODE", false);

const ISPC::ParamDefSingle<bool> ISPC::ModuleLSH::LSH_ENABLE_INFOTM_METHOD("LSH_ENABLE_INFOTM_METHOD", false);
const ISPC::ParamDef<int> ISPC::ModuleLSH::LSH_INFOTM_METHOD("LSH_INFOTM_METHOD", 0, 6, 0);

const ISPC::ParamDef<IMG_INT32> ISPC::ModuleLSH::LSH_BIG_RING_RADIUS("LSH_BIG_RING_RADIUS", 0, 1000, 760);
const ISPC::ParamDef<IMG_INT32> ISPC::ModuleLSH::LSH_SMALL_RING_RADIUS("LSH_SMALL_RING_RADIUS", 0, 1000, 420);
const ISPC::ParamDef<IMG_INT32> ISPC::ModuleLSH::LSH_MAX_RADIUS("LSH_MAX_RADIUS", 0, 1000, 760);
const ISPC::ParamDef<IMG_FLOAT> ISPC::ModuleLSH::AWB_CORRECTION_DATA_PULL_UP_GAIN("AWB_CORRECTION_DATA_PULL_UP_GAIN", 0.0, 10.0, 1.0);
const ISPC::ParamDef<IMG_FLOAT> ISPC::ModuleLSH::LSH_L_CORRECTION_DATA_PULLUP_GAIN("LSH_L_CORRECTION_DATA_PULLUP_GAIN", 0.0, 3.0, 1.0);
const ISPC::ParamDef<IMG_FLOAT> ISPC::ModuleLSH::LSH_R_CORRECTION_DATA_PULLUP_GAIN("LSH_R_CORRECTION_DATA_PULLUP_GAIN", 0.0, 3.0, 1.0);

const ISPC::ParamDef<IMG_FLOAT> ISPC::ModuleLSH::LSH_CURVE_BASE_VAL_L("LSH_CURVE_BASE_VAL_L", -10.0, 10.0, 0.0);
const ISPC::ParamDef<IMG_FLOAT> ISPC::ModuleLSH::LSH_CURVE_BASE_VAL_R("LSH_CURVE_BASE_VAL_R", -10.0, 10.0, 0.0);

static const int LSH_RADIUS_CENTER_OFFSET_L_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleLSH::LSH_RADIUS_CENTER_OFFSET_L("LSH_RADIUS_CENTER_OFFSET_L", 0, 100, LSH_RADIUS_CENTER_OFFSET_L_DEF,2);

static const int LSH_RADIUS_CENTER_OFFSET_R_DEF[2] = {0, 0};
const ISPC::ParamDefArray<int> ISPC::ModuleLSH::LSH_RADIUS_CENTER_OFFSET_R("LSH_RADIUS_CENTER_OFFSET_R", 0, 100, LSH_RADIUS_CENTER_OFFSET_R_DEF,2);
#endif

#define LSH_FILE_DEPRECATED "LSH_MATRIX_FILE"

ISPC::ParameterGroup ISPC::ModuleLSH::getGroup()
{
    ParameterGroup group;

    group.header = "// Lens Shading parameters";

    group.parameters.insert(LSH_GRADIENT_X.name);
    group.parameters.insert(LSH_GRADIENT_Y.name);
    group.parameters.insert(LSH_MATRIX.name);

    return group;
}

/*
 * protected
 */
ISPC::ModuleLSH::lsh_mat::lsh_mat()
{
    sGrid = LSH_GRID();
    matrixId = 0;
}

ISPC::ModuleLSH::lsh_mat::~lsh_mat()
{
    LSH_Free(&sGrid);
}

ISPC::ModuleLSH::const_iterator ISPC::ModuleLSH::findMatrix(
    IMG_UINT32 matrixId) const
{
    const_iterator it = list_grid.begin();
    for (it = list_grid.begin(); it != list_grid.end(); it++)
    {
        if ((*it)->matrixId == matrixId)
        {
            return it;
        }
    }
    return it;
}

ISPC::ModuleLSH::iterator ISPC::ModuleLSH::findMatrix(IMG_UINT32 matrixId)
{
    iterator it = list_grid.begin();
    for (it = list_grid.begin(); it != list_grid.end(); it++)
    {
        if ((*it)->matrixId == matrixId)
        {
            return it;
        }
    }
    return it;
}

IMG_RESULT ISPC::ModuleLSH::updateCIMatrix(const LSH_GRID *pGrid,
    IMG_UINT8 ui8BitsPerDiff, IMG_UINT32 matId)
{
    IMG_RESULT ret;
    CI_LSHMAT sMatrix;
    CI_PIPELINE *pCIPipeline = pipeline->getCIPipeline();
    MC_PIPELINE *pMCPipeline = pipeline->getMCPipeline();
    const Global_Setup &globalSetup = pipeline->getGlobalSetup();

    ret = CI_PipelineAcquireLSHMatrix(pCIPipeline, matId, &sMatrix);
    if (ret)
    {
        MOD_LOG_ERROR("Failed to acquire the LSH matrix %d\n", matId);
        return IMG_ERROR_FATAL;
    }

    ret = MC_LSHConvertGrid(pGrid, ui8BitsPerDiff, &sMatrix);
    if (ret)
    {
        MOD_LOG_ERROR("Failed to convert the LSH matrix %d\n", matId);

#ifdef INFOTM_LSH_AUTO_RETRY
		if (calibration_data.pullup_gain_L > 0.05)
		{
			calibration_data.pullup_gain_L -= 0.05;
		}
		
		if (calibration_data.pullup_gain_R > 0.05)
		{
			calibration_data.pullup_gain_R -= 0.05;
		}
		return IMG_ERROR_MC_CONVERT_FAULT;
#else
        return IMG_ERROR_FATAL;
#endif
    }

    // in CFAs
    sMatrix.config.ui16OffsetX =
        pMCPipeline->sIIF.ui16ImagerOffset[0] / (globalSetup.CFA_WIDTH);
    sMatrix.config.ui16OffsetY =
        pMCPipeline->sIIF.ui16ImagerOffset[1] / (globalSetup.CFA_HEIGHT);

    sMatrix.config.ui16SkipX = pMCPipeline->sIIF.ui16ImagerDecimation[0];
    sMatrix.config.ui16SkipY = pMCPipeline->sIIF.ui16ImagerDecimation[1];

    ret = CI_PipelineReleaseLSHMatrix(pCIPipeline, &sMatrix);
    if (ret)
    {
        MOD_LOG_ERROR("Failed to release the LSH matrix %d\n", matId);
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::loadMatrix(const LSH_GRID &sGrid,
    IMG_UINT32 &out_matId, const std::string &filename,
    double wbScale, IMG_UINT8 bitsPerDif)
{
    CI_PIPELINE *pCIPipeline = NULL;

    if (pipeline)
    {
        pCIPipeline = pipeline->getCIPipeline();
    }
    if (pCIPipeline)
    {
        IMG_RESULT ret;
        IMG_UINT32 matId = 0;
        IMG_UINT32 uiAllocation, uiLineSize, uiStride;
        IMG_UINT8 ui8BitsPerDiff = bitsPerDif;

        if (0 == bitsPerDif)
        {
            ui8BitsPerDiff = MC_LSHComputeMinBitdiff(&sGrid, NULL);
        }

        uiAllocation = MC_LSHGetSizes(&sGrid, ui8BitsPerDiff,
            &uiLineSize, &uiStride);

        ret = CI_PipelineAllocateLSHMatrix(pCIPipeline, uiAllocation,
            &matId);
        if (ret)
        {
            MOD_LOG_ERROR("Failed to allocate LSH matrix buffer\n");
            return IMG_ERROR_FATAL;
        }

        // then convert the matrix
        ret = updateCIMatrix(&sGrid, ui8BitsPerDiff, matId);
        if (ret)
        {
            MOD_LOG_ERROR("Failed to update CI LSH matrix\n");
            CI_PipelineDeregisterLSHMatrix(pCIPipeline, matId);
            return ret;
        }

        MOD_LOG_DEBUG("Loading '%s' into LSH matrix %d done using %d bits "\
            "per diff (index %d)\n",
            filename.c_str(), matId, (int)ui8BitsPerDiff, list_grid.size());
        lsh_mat *pNew = new lsh_mat;
        if (!pNew)
        {
            MOD_LOG_ERROR("failed to allocate internal matrix variable\n");
            // clean even though we ran out of memory...
            CI_PipelineDeregisterLSHMatrix(pCIPipeline, matId);
            return IMG_ERROR_MALLOC_FAILED;
        }

        pNew->sGrid = sGrid;  // copy of pointers
        pNew->filename = filename;
        pNew->matrixId = matId;
        pNew->wbScale = wbScale;
        list_grid.push_back(pNew);

        out_matId = matId;

        return IMG_SUCCESS;
    }
    MOD_LOG_ERROR("Pipeline pointer is NULL\n");
    return IMG_ERROR_UNEXPECTED_STATE;
}

/*
 * public
 */

ISPC::ModuleLSH::ModuleLSH() : SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);

    currentMatId = 0;
}

ISPC::ModuleLSH::~ModuleLSH()
{
    CI_PIPELINE *pCIPipeline = NULL;
    IMG_RESULT ret;
    if (pipeline)
    {
        pCIPipeline = pipeline->getCIPipeline();
    }

    iterator it;
    for (it = list_grid.begin(); it != list_grid.end(); it++)
    {
        lsh_mat *pMat = *it;
        if (pMat)
        {
            if (pCIPipeline)
            {
                ret = CI_PipelineDeregisterLSHMatrix(pCIPipeline,
                    pMat->matrixId);
                if (ret)
                {
                    MOD_LOG_WARNING("failed to free matrixId %d\n",
                        pMat->matrixId);
                }
            }

            *it = NULL;
            delete pMat;
        }
    }
    list_grid.clear();
}

bool ISPC::ModuleLSH::hasGrid() const
{
    return currentMatId != 0;
}

IMG_UINT32 ISPC::ModuleLSH::countGrid() const
{
    return list_grid.size();
}

IMG_UINT32 ISPC::ModuleLSH::getMatrixId(const unsigned int m) const
{
    const_iterator it = list_grid.begin();
    unsigned int i;
    for (i = 0; i < m && it != list_grid.end(); i++)
    {
        it++;
    }
    if (it != list_grid.end())
    {
        return (*it)->matrixId;
    }
    return 0;
}

IMG_UINT32 ISPC::ModuleLSH::getCurrentMatrixId() const
{
    return currentMatId;
}

double ISPC::ModuleLSH::getCurrentScaleWB() const
{
    if (currentMatId != 0)
    {
        const_iterator it = findMatrix(currentMatId);
        if (list_grid.end() != it)
        {
            return (*it)->wbScale;
        }
    }
    return 1.0;
}

IMG_RESULT ISPC::ModuleLSH::load(const ParameterList &parameters)
{
    int i;

    bEnableMatrix = parameters.getParameter(LSH_MATRIX);
    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        aGradientX[i] = parameters.getParameter(LSH_GRADIENT_X, i);
    }
    for ( i = 0 ; i < LSH_GRADS_NO ; i++ )
    {
        aGradientY[i] = parameters.getParameter(LSH_GRADIENT_Y, i);
    }

    if (parameters.exists(LSH_FILE_DEPRECATED))
    {
        MOD_LOG_WARNING("Deprecated %s parameter found - use %s_X\n",
            LSH_FILE_DEPRECATED, ControlLSH::LSH_FILE_S.name.c_str());
    }

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE

	enable_infotm_method = parameters.getParameter(LSH_ENABLE_INFOTM_METHOD);
	infotm_method = parameters.getParameter(LSH_INFOTM_METHOD);

	infotm_lsh_exchange_direction = parameters.getParameter(INFOTM_LSH_EXCHANGE_DIRECTION);
	infotm_lsh_flat_mode = parameters.getParameter(INFOTM_LSH_FLAT_MODE);


	if (enable_infotm_method)
	{
		awb_convert_gain = parameters.getParameter(AWB_CORRECTION_DATA_PULL_UP_GAIN);

		calibration_data.big_ring_radius = parameters.getParameter(LSH_BIG_RING_RADIUS);
		calibration_data.small_ring_radius = parameters.getParameter(LSH_SMALL_RING_RADIUS);
		calibration_data.max_radius = parameters.getParameter(LSH_MAX_RADIUS);	
		calibration_data.pullup_gain_L = parameters.getParameter(LSH_L_CORRECTION_DATA_PULLUP_GAIN);
		calibration_data.pullup_gain_R = parameters.getParameter(LSH_R_CORRECTION_DATA_PULLUP_GAIN);
		calibration_data.radius_center_offset_L[0] = parameters.getParameter(LSH_RADIUS_CENTER_OFFSET_L, 0);
		calibration_data.radius_center_offset_L[1] = parameters.getParameter(LSH_RADIUS_CENTER_OFFSET_L, 1);
		calibration_data.radius_center_offset_R[0] = parameters.getParameter(LSH_RADIUS_CENTER_OFFSET_R, 0);
		calibration_data.radius_center_offset_R[1] = parameters.getParameter(LSH_RADIUS_CENTER_OFFSET_R, 1);

		calibration_data.curve_base_val[0] = parameters.getParameter(LSH_CURVE_BASE_VAL_L);
		calibration_data.curve_base_val[1] = parameters.getParameter(LSH_CURVE_BASE_VAL_R);

        calibration_data.flat_mode = infotm_lsh_flat_mode;
		
		printf("==>big radius %d, small radius %d, max_radius %d, awb convert gain %f, left up %f, right up %f\n", calibration_data.big_ring_radius, calibration_data.small_ring_radius, calibration_data.max_radius, awb_convert_gain, calibration_data.pullup_gain_L, calibration_data.pullup_gain_R);
	}
#endif

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::save(ParameterList &parameters, SaveType t) const
{
    int i;
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleLSH::getGroup();
    }

    parameters.addGroup("ModuleLSH", group);

    switch (t)
    {
    case SAVE_VAL:
    {
        parameters.addParameter(LSH_MATRIX, this->bEnableMatrix);
        //  would it be better to save current state?

        values.clear();
        for (i = 0; i < LSH_GRADS_NO; i++)
        {
            values.push_back(toString(this->aGradientX[i]));
        }
        parameters.addParameter(LSH_GRADIENT_X, values);

        values.clear();
        for (i = 0; i < LSH_GRADS_NO; i++)
        {
            values.push_back(toString(this->aGradientY[i]));
        }
        parameters.addParameter(LSH_GRADIENT_Y, values);

        const_iterator it = findMatrix(currentMatId);
        if (it != list_grid.end() && !((*it)->filename.empty()))
        {
            parameters.addParameter(ControlLSH::LSH_FILE_S,
                (*it)->filename);
        }
    }
    break;

    case SAVE_MIN:
        parameters.addParameterMin(LSH_MATRIX);
        parameters.addParameterMin(LSH_GRADIENT_X);
        parameters.addParameterMin(LSH_GRADIENT_Y);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(LSH_MATRIX);
        parameters.addParameterMax(LSH_GRADIENT_X);
        parameters.addParameterMax(LSH_GRADIENT_Y);
        break;

    case SAVE_DEF:
        parameters.addParameterDef(LSH_MATRIX);
        parameters.addParameterDef(LSH_GRADIENT_X);
        parameters.addParameterDef(LSH_GRADIENT_Y);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::setup()
{
    LOG_PERF_IN();
    MC_PIPELINE *pMCPipeline = NULL;
    if (!pipeline)
    {
        MOD_LOG_ERROR("pipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    pMCPipeline = pipeline->getMCPipeline();
    if (!pMCPipeline)
    {
        MOD_LOG_ERROR("pMCPipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    const Global_Setup &globalSetup = pipeline->getGlobalSetup();

    // configuration of the lens-shading should be done using udpateGrid()

    for (int i = 0; i < 2; i++)
    {
        pMCPipeline->sLSH.aGradients[0][i] = aGradientX[i];
        pMCPipeline->sLSH.aGradients[1][i] = aGradientY[i];
    }

    this->setupFlag = true;
    LOG_PERF_OUT();

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::loadMatrices(const ParameterList &parameters,
    std::vector<IMG_UINT32> &matrixIdList)
{
    std::map<ControlLSH::IMG_TEMPERATURE_TYPE, ControlLSH::GridInfo> grids;
    IMG_UINT8 maxBitsPerDiff = 0;
    IMG_RESULT ret;
	int i, nLoaded;

    // Go through grids container and load grids
    std::map<ControlLSH::IMG_TEMPERATURE_TYPE, ControlLSH::GridInfo>::const_iterator it;

	i = 0;
	nLoaded = 0;

    ret = ControlLSH::loadMatrices(parameters, grids, maxBitsPerDiff);
    /* loadMatrices() returns IMG_ERROR_CANCELLED when no matrix
     * are present in parameter list */
    if (grids.size() == 0)
    {
        return IMG_ERROR_CANCELLED;
    }
    if (IMG_SUCCESS != ret)
    {
        MOD_LOG_ERROR("failed to load matrices\n");
        return IMG_ERROR_FATAL;
    }
    if (maxBitsPerDiff < LSH_DELTA_BITS_MIN)
    {
        maxBitsPerDiff = ControlLSH::findBiggestBitsPerDiff(grids);
        MOD_LOG_WARNING("Calculated %s %d. Consider to use this value in "
            "configuration file.\n",
            (ControlLSH::LSH_CTRL_BITS_DIFF.name).c_str(),
            maxBitsPerDiff);
    }
    if (maxBitsPerDiff < LSH_DELTA_BITS_MIN
        || maxBitsPerDiff > LSH_DELTA_BITS_MAX)
    {
        MOD_LOG_ERROR("invalid bits per diff %d selected (min %d, max %d)\n",
            (int)maxBitsPerDiff, LSH_DELTA_BITS_MIN, LSH_DELTA_BITS_MAX);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    //int i = 0, nLoaded = 0;
    it = grids.begin();

    while (it != grids.end())
    {
        IMG_UINT32 matId;
        ret = addMatrix(it->second.filename, matId, it->second.scaledWB,
            maxBitsPerDiff);
        if (ret)
        {
            MOD_LOG_WARNING("failed to load matrix %d from file %s\n",
                i, it->second.filename.c_str());
            return IMG_ERROR_FATAL;
        }
        else
        {
            matrixIdList.push_back(matId);
            nLoaded++;
        }
        it++;
        i++;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleLSH::addMatrix(const std::string &filename,
    IMG_UINT32 &out_matId, double wbScale, IMG_UINT8 ui8BitsPerDiff)
{
    IMG_RESULT ret;
    LSH_GRID sGrid = LSH_GRID();

    MOD_LOG_DEBUG("loading LSH matrix from %s\n", filename.c_str());

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
	if (enable_infotm_method)
	{
		int direction[2] = {0, 1};
        if (infotm_lsh_exchange_direction)
        {
    		direction[0] = 1;
    		direction[1] = 0;
            pipeline->getSensor()->ExchangeCalibDirection(1);
        }
        else
        {
            pipeline->getSensor()->ExchangeCalibDirection(0);
        }
        
		calibration_data.sensor_calibration_data_L = pipeline->getSensor()->ReadSensorCalibrationData(infotm_method, direction[0], awb_convert_gain, &calibration_data.version);
		
		if (calibration_data.sensor_calibration_data_L == NULL)
		{
			calibration_data.sensor_calibration_data_R = NULL;
		}
		else
		{
			calibration_data.sensor_calibration_data_R = pipeline->getSensor()->ReadSensorCalibrationData(infotm_method, direction[1], awb_convert_gain, NULL);
		}
        
        if (infotm_method == 0 || infotm_method == 1) // lipin & baikangxing otp
        {
    		if (calibration_data.version&0xFF00 == 0x1500)
    		{
    			calibration_data.sensor_calibration_data_L = NULL;
    			calibration_data.sensor_calibration_data_R = NULL;
    		}		
    		else if (calibration_data.version&0xFF00 == 0x1400)
    		{
    			LIPIN_OTP_VERSION_1_4* v1_4_cali_struct = (LIPIN_OTP_VERSION_1_4*)calibration_data.sensor_calibration_data_L;
    			calibration_data.radius_center_offset_L[0] = (0x7F&v1_4_cali_struct->radius_center_offset[0])*((0x80&v1_4_cali_struct->radius_center_offset[0]) ? 1:-1);
    			calibration_data.radius_center_offset_L[1] = (0x7F&v1_4_cali_struct->radius_center_offset[1])*((0x80&v1_4_cali_struct->radius_center_offset[1]) ? 1:-1);
    			v1_4_cali_struct = (LIPIN_OTP_VERSION_1_4*)calibration_data.sensor_calibration_data_R;
    			calibration_data.radius_center_offset_R[0] = (0x7F&v1_4_cali_struct->radius_center_offset[0])*((0x80&v1_4_cali_struct->radius_center_offset[0]) ? 1:-1);
    			calibration_data.radius_center_offset_R[1] = (0x7F&v1_4_cali_struct->radius_center_offset[1])*((0x80&v1_4_cali_struct->radius_center_offset[1]) ? 1:-1);
    		}
        }
#ifdef INFOTM_E2PROM_METHOD
	    else if (infotm_method == 2) // e2prom
	    {
	        printf("===>e2prom data load done and cala center offset\n");
			E2PROM_DATA* e2prom_data = (E2PROM_DATA*)calibration_data.sensor_calibration_data_L;
			calibration_data.radius_center_offset_L[0] = 0;//((e2prom_data->center_offset[0]&0x00FF) == 0 ? -1:1)*(e2prom_data->center_offset[0]>>8);
			calibration_data.radius_center_offset_L[1] = 0;//((e2prom_data->center_offset[1]&0x00FF) == 0 ? -1:1)*(e2prom_data->center_offset[1]>>8);
			e2prom_data = (E2PROM_DATA*)calibration_data.sensor_calibration_data_R;
			calibration_data.radius_center_offset_R[0] = 0;//((e2prom_data->center_offset[0]&0x00FF) == 0 ? -1:1)*(e2prom_data->center_offset[0]>>8);
			calibration_data.radius_center_offset_R[1] = 0;//((e2prom_data->center_offset[1]&0x00FF) == 0 ? -1:1)*(e2prom_data->center_offset[1]>>8); 
	        printf("===>calibration_data.sensor_calibration_data_L %x, calibration_data.sensor_calibration_data_R %x, offsetL[%d, %d], offsetR[%d, %d]\n", 
	        calibration_data.sensor_calibration_data_L, 
	        calibration_data.sensor_calibration_data_R,
	        calibration_data.radius_center_offset_L[0], 
	        calibration_data.radius_center_offset_L[1],
	        calibration_data.radius_center_offset_R[0],
	        calibration_data.radius_center_offset_R[1]
	        );
	    }
#endif
	}
	else
	{
		calibration_data.sensor_calibration_data_L = NULL;
		calibration_data.sensor_calibration_data_R = NULL;
		infotm_method = -1;
	}
lsh_retry:
    ret = LSH_Load_bin(&(sGrid), filename.c_str(), infotm_method, &calibration_data);
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

    ret = loadMatrix(sGrid, out_matId, filename, wbScale, ui8BitsPerDiff);
    if (ret)
    {
        LSH_Free(&sGrid);
#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
    	if (IMG_ERROR_MC_CONVERT_FAULT == ret)
		{
		    printf("===>LSH retry!\n");
			sGrid = LSH_GRID();
			goto lsh_retry;
		}
#endif
    }
    return ret;
}

IMG_RESULT ISPC::ModuleLSH::addMatrix(const LSH_GRID &sGrid,
    IMG_UINT32 &out_matId, double wbScale, IMG_UINT8 ui8BitsPerDiff)
{
    return loadMatrix(sGrid, out_matId, std::string(), wbScale,
        ui8BitsPerDiff);
}

IMG_RESULT ISPC::ModuleLSH::removeMatrix(IMG_UINT32 matrixId)
{
    iterator it = findMatrix(matrixId);
    if (it != list_grid.end())
    {
        CI_PIPELINE *pCIPipeline = NULL;
        IMG_RESULT ret;
        lsh_mat *pMat = (*it);

        if (pipeline)
        {
            pCIPipeline = pipeline->getCIPipeline();
        }
        if (pCIPipeline)
        {
            ret = CI_PipelineDeregisterLSHMatrix(pCIPipeline, matrixId);
            if (ret)
            {
                MOD_LOG_ERROR("Failed to deregister CI LSH matrix %d\n",
                    matrixId);
                return IMG_ERROR_FATAL;
            }
        }

        if (matrixId == currentMatId)
        {
            currentMatId = 0;
        }

        LSH_Free(&(pMat->sGrid));
        list_grid.erase(it);
        delete pMat;

        if (!pCIPipeline)
        {
            MOD_LOG_ERROR("Pipeline pointer is NULL\n");
            return IMG_ERROR_UNEXPECTED_STATE;
        }

        return IMG_SUCCESS;
    }
    return IMG_ERROR_INVALID_PARAMETERS;
}

IMG_RESULT ISPC::ModuleLSH::configureMatrix(IMG_UINT32 matrixId)
{
    iterator it = list_grid.end();

    // matrixId of 0 to disable the use of the matrix
    if (matrixId > 0)
    {
        it = findMatrix(matrixId);
    }
    if (it != list_grid.end() || 0 == matrixId)
    {
        CI_PIPELINE *pCIPipeline = NULL;
        IMG_RESULT ret;

        if (pipeline)
        {
            pCIPipeline = pipeline->getCIPipeline();
        }
        if (pCIPipeline)
        {
            ModuleWBC *pWBC = pipeline->getModule<ModuleWBC>();
            ret = CI_PipelineUpdateLSHMatrix(pCIPipeline, matrixId);
            if (ret)
            {
                MOD_LOG_ERROR("Failed to change config to use LSH matrix %d\n",
                    matrixId);
                return IMG_ERROR_FATAL;
            }
            if (pWBC)
            {
                // to support the potential new WBGAIN
                pWBC->requestUpdate();
            }
            currentMatId = matrixId;
            return IMG_SUCCESS;
        }
        MOD_LOG_ERROR("Pipeline pointer is NULL\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    MOD_LOG_ERROR("matrix %d not found\n", matrixId);
    return IMG_ERROR_INVALID_PARAMETERS;
}

LSH_GRID* ISPC::ModuleLSH::getGrid(IMG_UINT32 matrixId)
{
    iterator it = findMatrix(matrixId);

    if (it != list_grid.end())
    {
        return &(*it)->sGrid;  // copy the pointers
    }
    return NULL;
}

IMG_RESULT ISPC::ModuleLSH::updateGrid(IMG_UINT32 matrixId,
    IMG_UINT8 ui8BitsPerDiff)
{
    iterator it = findMatrix(matrixId);

    if (it != list_grid.end())
    {
        CI_PIPELINE *pCIPipeline = NULL;
        lsh_mat *pMat = (*it);

        if (pipeline)
        {
            pCIPipeline = pipeline->getCIPipeline();
        }
        if (pCIPipeline)
        {
            IMG_RESULT ret = updateCIMatrix(&(pMat->sGrid),
                ui8BitsPerDiff, matrixId);
            if (ret)
            {
                MOD_LOG_ERROR("Failed to update CI LSH matrix\n");
                return ret;
            }
        }
        else
        {
            MOD_LOG_ERROR("Pipeline pointer is NULL\n");
            return IMG_ERROR_UNEXPECTED_STATE;
        }
    }
    return IMG_ERROR_INVALID_PARAMETERS;
}

std::string ISPC::ModuleLSH::getFilename(IMG_UINT32 matrixId) const
{
    const_iterator it = findMatrix(matrixId);

    if (it != list_grid.end())
    {
        return (*it)->filename;
    }
    return std::string();
}

IMG_RESULT ISPC::ModuleLSH::saveMatrix(IMG_UINT32 matrixId,
    const std::string &filename) const
{
    IMG_RESULT ret = IMG_ERROR_INVALID_PARAMETERS;
    const_iterator it = findMatrix(matrixId);

    if (it != list_grid.end())
    {
        lsh_mat *pMat = (*it);
        LOG_PERF_IN();
        ret = LSH_Save_bin(&(pMat->sGrid), filename.c_str());
        LOG_PERF_OUT();
    }
    return ret;
}
