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

// deprecated
const ISPC::ParamDef<double> ISPC::ModuleFLD::FLD_FRAMERATE("FLD_FRAME_RATE",
    1.0, 255.0, 30.0);

// deprecated
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_VTOT("FLD_VTOT", 0, 16383, 525);

const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_SCENECHANGE(
    "FLD_SCENE_CHANGE_TH", 0, 1048575, 300000);
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_MINPN("FLD_MIN_PN", 0, 63, 4);
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_PN("FLD_PN", 0, 63, 4);
const ISPC::ParamDef<int> ISPC::ModuleFLD::FLD_NFTH("FLD_NF_TH",
    0, 32767, 1500);
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
        parameters.addParameter(Parameter(FLD_ENABLE.name,
            toString(this->bEnable)));
        parameters.addParameter(Parameter(FLD_FRAMERATE.name,
            toString(this->fFrameRate)));
        parameters.addParameter(Parameter(FLD_VTOT.name,
            toString(this->iVTotal)));
        parameters.addParameter(Parameter(FLD_SCENECHANGE.name,
            toString(this->iScheneChangeTH)));
        parameters.addParameter(Parameter(FLD_MINPN.name,
            toString(this->iMinPN)));
        parameters.addParameter(Parameter(FLD_PN.name,
            toString(this->iPN)));
        parameters.addParameter(Parameter(FLD_NFTH.name,
            toString(this->iNF_TH)));
        parameters.addParameter(Parameter(FLD_COEFDIFF.name,
            toString(this->iCoeffDiffTH)));
        parameters.addParameter(Parameter(FLD_RSHIFT.name,
            toString(this->iRShift)));
        parameters.addParameter(Parameter(FLD_RESET.name,
            toString(this->bReset)));
        break;

    case SAVE_MIN:
        parameters.addParameter(Parameter(FLD_ENABLE.name,
            toString(FLD_ENABLE.def)));  // bool do not have a min
        parameters.addParameter(Parameter(FLD_FRAMERATE.name,
            toString(FLD_FRAMERATE.min)));
        parameters.addParameter(Parameter(FLD_VTOT.name,
            toString(FLD_VTOT.min)));
        parameters.addParameter(Parameter(FLD_SCENECHANGE.name,
            toString(FLD_SCENECHANGE.min)));
        parameters.addParameter(Parameter(FLD_MINPN.name,
            toString(FLD_MINPN.min)));
        parameters.addParameter(Parameter(FLD_PN.name,
            toString(FLD_PN.min)));
        parameters.addParameter(Parameter(FLD_NFTH.name,
            toString(FLD_NFTH.min)));
        parameters.addParameter(Parameter(FLD_COEFDIFF.name,
            toString(FLD_COEFDIFF.min)));
        parameters.addParameter(Parameter(FLD_RSHIFT.name,
            toString(FLD_RSHIFT.min)));
        parameters.addParameter(Parameter(FLD_RESET.name,
            toString(FLD_RESET.def)));  // bool do not have a min
        break;

    case SAVE_MAX:
        parameters.addParameter(Parameter(FLD_ENABLE.name,
            toString(FLD_ENABLE.def)));  // bool do not have a max
        parameters.addParameter(Parameter(FLD_FRAMERATE.name,
            toString(FLD_FRAMERATE.max)));
        parameters.addParameter(Parameter(FLD_VTOT.name,
            toString(FLD_VTOT.max)));
        parameters.addParameter(Parameter(FLD_SCENECHANGE.name,
            toString(FLD_SCENECHANGE.max)));
        parameters.addParameter(Parameter(FLD_MINPN.name,
            toString(FLD_MINPN.max)));
        parameters.addParameter(Parameter(FLD_PN.name,
            toString(FLD_PN.max)));
        parameters.addParameter(Parameter(FLD_NFTH.name,
            toString(FLD_NFTH.max)));
        parameters.addParameter(Parameter(FLD_COEFDIFF.name,
            toString(FLD_COEFDIFF.max)));
        parameters.addParameter(Parameter(FLD_RSHIFT.name,
            toString(FLD_RSHIFT.max)));
        parameters.addParameter(Parameter(FLD_RESET.name,
            toString(FLD_RESET.def)));  // bool do not have a max
        break;

    case SAVE_DEF:
        parameters.addParameter(Parameter(FLD_ENABLE.name,
            toString(FLD_ENABLE.def)
            + " // " + getParameterInfo(FLD_ENABLE)));
        parameters.addParameter(Parameter(FLD_FRAMERATE.name,
            toString(FLD_FRAMERATE.def)
            + " // " + getParameterInfo(FLD_FRAMERATE)));
        parameters.addParameter(Parameter(FLD_VTOT.name,
            toString(FLD_VTOT.def)
            + " // " + getParameterInfo(FLD_VTOT)));
        parameters.addParameter(Parameter(FLD_SCENECHANGE.name,
            toString(FLD_SCENECHANGE.def)
            + " // " + getParameterInfo(FLD_SCENECHANGE)));
        parameters.addParameter(Parameter(FLD_MINPN.name,
            toString(FLD_MINPN.def) + " // " + getParameterInfo(FLD_MINPN)));
        parameters.addParameter(Parameter(FLD_PN.name,
            toString(FLD_PN.def) + " // " + getParameterInfo(FLD_PN)));
        parameters.addParameter(Parameter(FLD_NFTH.name,
            toString(FLD_NFTH.def) + " // " + getParameterInfo(FLD_NFTH)));
        parameters.addParameter(Parameter(FLD_COEFDIFF.name,
            toString(FLD_COEFDIFF.def)
            + " // " + getParameterInfo(FLD_COEFDIFF)));
        parameters.addParameter(Parameter(FLD_RSHIFT.name,
            toString(FLD_RSHIFT.def)
            + " // " + getParameterInfo(FLD_RSHIFT)));
        parameters.addParameter(Parameter(FLD_RESET.name,
            toString(FLD_RESET.def) + " // " + getParameterInfo(FLD_RESET)));
        break;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleFLD::setup()
{
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
    return IMG_SUCCESS;
}
