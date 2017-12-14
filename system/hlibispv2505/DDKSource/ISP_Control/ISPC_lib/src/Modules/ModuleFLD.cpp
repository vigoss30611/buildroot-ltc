/**
*******************************************************************************
@file ModuleFLD.cpp

@brief Implementation of ISPC::ModuleFLD

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
#include "ispc/ModuleFLD.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_FLD"

#include "ispc/Pipeline.h"

#include "ispc/PerfTime.h"

// deprecated
const ISPC::ParamDef<double> ISPC::ModuleFLD::FLD_FRAMERATE("FLD_FRAME_RATE",
    1.0, 255.0, 30.0);

// deprecated
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_VTOT("FLD_VTOT", 0, 16383, 525);

const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_SCENECHANGE(
    "FLD_SCENE_CHANGE_TH", 0, 1048575, 300000);
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_MINPN("FLD_MIN_PN", 0, 63, 4);

// that much consecutive frames are used in final decision
// must be multiply of 4, too low causes more uncertainty
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_PN("FLD_PN", 0, 63, 16);

// detection threshold, 15000 seems good value for office conditions
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_NFTH("FLD_NF_TH",
    0, 32767, 15000);
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_COEFDIFF("FLD_COEF_DIFF_TH",
    0, 32767, 50);
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_RSHIFT("FLD_RSHIFT",
    0, 63, 10);
const ISPC::ParamDefSingle<bool> ISPC::ModuleFLD::FLD_RESET("FLD_RESET",
    false);
const ISPC::ParamDefSingle<bool> ISPC::ModuleFLD::FLD_ENABLE("FLD_ENABLE",
    false);

ISPC::ParameterGroup ISPC::ModuleFLD::GetGroup()
{
    ParameterGroup group;

    group.header = "// Flicker Detection parameters";

    group.parameters.insert(FLD_ENABLE.name);
    group.parameters.insert(FLD_FRAMERATE.name);
    group.parameters.insert(FLD_VTOT.name);
    group.parameters.insert(FLD_SCENECHANGE.name);
    group.parameters.insert(FLD_MINPN.name);
    group.parameters.insert(FLD_PN.name);
    group.parameters.insert(FLD_NFTH.name);
    group.parameters.insert(FLD_COEFDIFF.name);
    group.parameters.insert(FLD_RSHIFT.name);
    group.parameters.insert(FLD_RESET.name);

    return group;
}

ISPC::ModuleFLD::ModuleFLD(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

IMG_RESULT ISPC::ModuleFLD::load(const ParameterList &parameters)
{
    const Sensor *sensor = NULL;
    if (pipeline)
    {
        sensor = pipeline->getSensor();
    }

    // Flicker detection:
    bEnable =  parameters.getParameter(FLD_ENABLE);
    if (sensor)
    {
        fFrameRate = sensor->flFrameRate;
        iVTotal = sensor->nVTot;
        // deprecated loading:
        // fFrameRate = parameters.getParameter(FLD_FRAMERATE);
        // iVTotal = parameters.getParameter(FLD_VTOT);
    } else {
        iVTotal = parameters.getParameter(FLD_VTOT);
        fFrameRate = parameters.getParameter(FLD_FRAMERATE);
    }
    iScheneChangeTH =  parameters.getParameter(FLD_SCENECHANGE);
    iMinPN =  parameters.getParameter(FLD_MINPN);
    iPN =  parameters.getParameter(FLD_PN);
    iNF_TH =  parameters.getParameter(FLD_NFTH);
    iCoeffDiffTH =  parameters.getParameter(FLD_COEFDIFF);
    iRShift =  parameters.getParameter(FLD_RSHIFT);
    bReset =  parameters.getParameter(FLD_RESET);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleFLD::save(ParameterList &parameters, SaveType t) const
{
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleFLD::GetGroup();
    }

    parameters.addGroup("ModuleFLD", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(FLD_ENABLE, this->bEnable);
        parameters.addParameter(FLD_FRAMERATE, this->fFrameRate);
        parameters.addParameter(FLD_VTOT, this->iVTotal);
        parameters.addParameter(FLD_SCENECHANGE, this->iScheneChangeTH);
        parameters.addParameter(FLD_MINPN, this->iMinPN);
        parameters.addParameter(FLD_PN, this->iPN);
        parameters.addParameter(FLD_NFTH, this->iNF_TH);
        parameters.addParameter(FLD_COEFDIFF, this->iCoeffDiffTH);
        parameters.addParameter(FLD_RSHIFT, this->iRShift);
        parameters.addParameter(FLD_RESET, this->bReset);
        break;

    case SAVE_MIN:
        parameters.addParameterMin(FLD_ENABLE);  // bool do not have a min
        parameters.addParameterMin(FLD_FRAMERATE);
        parameters.addParameterMin(FLD_VTOT);
        parameters.addParameterMin(FLD_SCENECHANGE);
        parameters.addParameterMin(FLD_MINPN);
        parameters.addParameterMin(FLD_PN);
        parameters.addParameterMin(FLD_NFTH);
        parameters.addParameterMin(FLD_COEFDIFF);
        parameters.addParameterMin(FLD_RSHIFT);
        parameters.addParameterMin(FLD_RESET);  // bool do not have a min
        break;

    case SAVE_MAX:
        parameters.addParameterMax(FLD_ENABLE);  // bool do not have a max
        parameters.addParameterMax(FLD_FRAMERATE);
        parameters.addParameterMax(FLD_VTOT);
        parameters.addParameterMax(FLD_SCENECHANGE);
        parameters.addParameterMax(FLD_MINPN);
        parameters.addParameterMax(FLD_PN);
        parameters.addParameterMax(FLD_NFTH);
        parameters.addParameterMax(FLD_COEFDIFF);
        parameters.addParameterMax(FLD_RSHIFT);
        parameters.addParameterMax(FLD_RESET);  // bool do not have a max
        break;

    case SAVE_DEF:
        parameters.addParameterDef(FLD_ENABLE);
        parameters.addParameterDef(FLD_FRAMERATE);
        parameters.addParameterDef(FLD_VTOT);
        parameters.addParameterDef(FLD_SCENECHANGE);
        parameters.addParameterDef(FLD_MINPN);
        parameters.addParameterDef(FLD_PN);
        parameters.addParameterDef(FLD_NFTH);
        parameters.addParameterDef(FLD_COEFDIFF);
        parameters.addParameterDef(FLD_RSHIFT);
        parameters.addParameterDef(FLD_RESET);
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleFLD::setup()
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

    pMCPipeline->sFLD.bEnable = bEnable?IMG_TRUE:IMG_FALSE;
    pMCPipeline->sFLD.fFrameRate =   fFrameRate;
    pMCPipeline->sFLD.ui16VTot =   iVTotal;
    pMCPipeline->sFLD.ui32SceneChange =   iScheneChangeTH;
    pMCPipeline->sFLD.ui8MinPN =   iMinPN;
    pMCPipeline->sFLD.ui8PN =   iPN;
    pMCPipeline->sFLD.ui16NFThreshold = iNF_TH;
    pMCPipeline->sFLD.ui16CoefDiff = iCoeffDiffTH;
    pMCPipeline->sFLD.ui8RShift = iRShift;
    pMCPipeline->sFLD.bReset = bReset?IMG_TRUE:IMG_FALSE;

    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
