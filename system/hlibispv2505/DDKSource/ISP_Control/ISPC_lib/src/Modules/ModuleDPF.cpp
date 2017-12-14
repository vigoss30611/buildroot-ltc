/**
*******************************************************************************
 @file ModuleDPF.cpp

 @brief Implementation of ISPC::ModuleDPF

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
#include "ispc/ModuleDPF.h"

#include <felixcommon/dpfmap.h>
#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_DPF"

#include <string>

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleDPF::DPF_DETECT_ENABLE(
    "DPF_DETECT_ENABLE", false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleDPF::DPF_READ_ENABLE(
    "DPF_READ_MAP_ENABLE", false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleDPF::DPF_WRITE_ENABLE(
    "DPF_WRITE_MAP_ENABLE", false);
const ISPC::ParamDefSingle<std::string> ISPC::ModuleDPF::DPF_READ_MAP_FILE(
    "DPF_READ_MAP_FILE", "");
#define DPF_READ_MAP_FILE_DEF "<HW DPF read file>"
const ISPC::ParamDef<double> ISPC::ModuleDPF::DPF_WEIGHT("DPF_WEIGHT",
    0.0, 255.0, 16.0);
const ISPC::ParamDef<int> ISPC::ModuleDPF::DPF_THRESHOLD("DPF_THRESHOLD",
    0, 63, 0);

ISPC::ParameterGroup ISPC::ModuleDPF::getGroup()
{
    ParameterGroup group;

    group.header = "// Defective Pixels parameters";

    group.parameters.insert(DPF_DETECT_ENABLE.name);
    group.parameters.insert(DPF_READ_ENABLE.name);
    group.parameters.insert(DPF_WRITE_ENABLE.name);
    group.parameters.insert(DPF_THRESHOLD.name);
    group.parameters.insert(DPF_WEIGHT.name);
    group.parameters.insert(DPF_READ_MAP_FILE.name);

    return group;
}

ISPC::ModuleDPF::ModuleDPF(): SetupModuleBase(LOG_TAG)
{
    pDefectMap = NULL;
    ui32NbDefects = 0;

    ParameterList defaults;
    load(defaults);
}

ISPC::ModuleDPF::~ModuleDPF()
{
    if (pipeline)
    {
        MC_PIPELINE *pMCPipeline = pipeline->getMCPipeline();
        if (pDefectMap)
        {
            IMG_FREE(pDefectMap);
            pMCPipeline->sDPF.apDefectInput = NULL;
            pDefectMap = NULL;
        }
    }
}

IMG_RESULT ISPC::ModuleDPF::load(const ParameterList &parameters)
{
    bDetect = parameters.getParameter(DPF_DETECT_ENABLE);
    bRead = parameters.getParameter(DPF_READ_ENABLE);
    bWrite = parameters.getParameter(DPF_WRITE_ENABLE);
    ui32Threshold = parameters.getParameter(DPF_THRESHOLD);
    fWeight = parameters.getParameter(DPF_WEIGHT);

    if (parameters.exists(DPF_READ_MAP_FILE.name) && !hasInputMap())
    {
        readMapFilename =
            parameters.getParameter(DPF_READ_MAP_FILE);
        IMG_RESULT ret;
        MOD_LOG_DEBUG("loading DPF read map from %s\n",
            readMapFilename.c_str());
        ui32NbDefects = 0;
        ret = DPF_Load_bin(&pDefectMap, &ui32NbDefects,
            readMapFilename.c_str());
        if (ret)
        {
            MOD_LOG_WARNING("failed to load the DPF read map "\
                "- no map will be loaded\n");
        }
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDPF::save(ParameterList &parameters, SaveType t) const
{
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleDPF::getGroup();
    }

    parameters.addGroup("ModuleDPF", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(DPF_DETECT_ENABLE, this->bDetect);
        parameters.addParameter(DPF_READ_ENABLE, this->bRead);
        parameters.addParameter(DPF_WRITE_ENABLE, this->bWrite);
        parameters.addParameter(DPF_THRESHOLD, (int)this->ui32Threshold);
        parameters.addParameter(DPF_WEIGHT, this->fWeight);
        parameters.addParameter(DPF_READ_MAP_FILE, this->readMapFilename);

        /** @ save the DPF read map and add it to the parameter file
         * in another function */
    break;

    case SAVE_MIN:
        parameters.addParameterMin(DPF_DETECT_ENABLE);  // bool do not have min
        parameters.addParameterMin(DPF_READ_ENABLE);  // bool do not have min
        parameters.addParameterMin(DPF_WRITE_ENABLE);  // bool do not have min
        parameters.addParameterMin(DPF_THRESHOLD);
        parameters.addParameterMin(DPF_WEIGHT);
        parameters.addParameter(DPF_READ_MAP_FILE,
                std::string(DPF_READ_MAP_FILE_DEF));  // DPF_READ_MAP_FILE does not have a min
        break;

    case SAVE_MAX:
        parameters.addParameterMax(DPF_DETECT_ENABLE);  // bool do not have max
        parameters.addParameterMax(DPF_READ_ENABLE);  // bool do not have max
        parameters.addParameterMax(DPF_WRITE_ENABLE);  // bool do not have max
        parameters.addParameterMax(DPF_THRESHOLD);
        parameters.addParameterMax(DPF_WEIGHT);
        parameters.addParameter(DPF_READ_MAP_FILE,
                std::string(DPF_READ_MAP_FILE_DEF));  // DPF_READ_MAP_FILE does not have a max
        break;

    case SAVE_DEF:
        parameters.addParameterDef(DPF_DETECT_ENABLE);
        parameters.addParameterDef(DPF_READ_ENABLE);
        parameters.addParameterDef(DPF_WRITE_ENABLE);
        parameters.addParameterDef(DPF_THRESHOLD);
        parameters.addParameterDef(DPF_WEIGHT);
        parameters.addParameter(DPF_READ_MAP_FILE,
            std::string(DPF_READ_MAP_FILE_DEF));
        parameters.getParameter(DPF_READ_MAP_FILE.name)->setInfo(
            getParameterInfo(DPF_READ_MAP_FILE));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleDPF::setup()
{
    LOG_PERF_IN();
    int i;
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

    pMCPipeline->sDPF.eDPFEnable = 0;
    if (bDetect)
    {
        pMCPipeline->sDPF.eDPFEnable |= CI_DPF_DETECT_ENABLED;
    }
    if (bWrite)
    {
        pMCPipeline->sDPF.eDPFEnable |= CI_DPF_DETECT_ENABLED
            |CI_DPF_WRITE_MAP_ENABLED;
    }
    if (bRead)
    {
        pMCPipeline->sDPF.eDPFEnable |= CI_DPF_READ_MAP_ENABLED;
    }
    pMCPipeline->sDPF.fWeight = fWeight;
    pMCPipeline->sDPF.ui8Threshold = static_cast<IMG_UINT8>(ui32Threshold);

    for (i = 0 ; i < 2 ; i++)
    {
        pMCPipeline->sDPF.aOffset[i] = pMCPipeline->sIIF.ui16ImagerOffset[i];
        pMCPipeline->sDPF.aSkip[i] =
            static_cast<IMG_UINT8>(pMCPipeline->sIIF.ui16ImagerDecimation[i]);
    }

    if (pDefectMap != NULL)
    {
        // the memory is given to MC
        // it will be freed in MC and here at the same time!
        pMCPipeline->sDPF.apDefectInput = pDefectMap;
        pMCPipeline->sDPF.ui32NDefect = ui32NbDefects;
    }
    else
    {
        pMCPipeline->sDPF.apDefectInput = NULL;
        pMCPipeline->sDPF.ui32NDefect = 0;
    }

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

bool ISPC::ModuleDPF::hasInputMap() const
{
    return pDefectMap && ui32NbDefects > 0;
}
