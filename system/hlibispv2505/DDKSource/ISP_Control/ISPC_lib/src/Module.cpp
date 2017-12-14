/**
*******************************************************************************
 @file Module.cpp

 @brief Implementation of the Module base classes

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

#include "ispc/Module.h"

#include <img_errors.h>

#include <string>
#include <list>
#include <ostream>

#include "ispc/Pipeline.h"
#include "ispc/Save.h"

//
// ModuleBase
//

const char* ISPC::ModuleBase::getLoggingName() const
{
    return logName.c_str();
}

IMG_RESULT ISPC::ModuleBase::save(ParameterList &parameters) const
{
    return save(parameters, SAVE_VAL);
}

std::string ISPC::ModuleBase::setupIDName(SetupID id)
{
    switch (id)
    {
        case STP_IIF:
            return "STP_IIF";
        case STP_EXS:
            return "STP_EXS";
        case STP_BLC:
            return "STP_BLC";
        case STP_RLT:
            return "STP_RLT";
        case STP_LSH:
            return "STP_LSH";
        case STP_WBC:
            return "STP_WBC";
        case STP_FOS:
            return "STP_FOS";
        case STP_DNS:
            return "STP_DNS";
        case STP_DPF:
            return "STP_DPF";
        case STP_ENS:
            return "STP_ENS";
        case STP_LCA:
            return "STP_LCA";
        case STP_CCM:
            return "STP_CCM";
        case STP_MGM:
            return "STP_MGM";
        case STP_GMA:
            return "STP_GMA";
        case STP_WBS:
            return "STP_WBS";
        case STP_AWS:
            return "STP_AWS";
        case STP_HIS:
            return "STP_HIS";
        case STP_R2Y:
            return "STP_R2Y";
        case STP_MIE:
            return "STP_MIE";
        case STP_VIB:
            return "STP_VIB";
        case STP_TNM:
            return "STP_TNM";
        case STP_FLD:
            return "STP_FLD";
        case STP_SHA:
            return "STP_SHA";
        case STP_ESC:
            return "STP_ESC";
        case STP_DSC:
            return "STP_DSC";
        case STP_Y2R:
            return "STP_Y2R";
        case STP_DGM:
            return "STP_DGM";
        case STP_OUT:
            return "STP_OUT";

        default:
            return "unkown-setupID";
    }
}

std::string ISPC::ModuleBase::controlIDName(ControlID id)
{
    switch (id)
    {
        case CTRL_AWB:
            return "CTRL_AWB";
        case CTRL_AE:
            return "CTRL_AE";
        case CTRL_AF:
            return "CTRL_AF";
        case CTRL_TNM:
            return "CTRL_TNM";
        case CTRL_DNS:
            return "CTRL_DNS";
        case CTRL_LBC:
            return "CTRL_LBC";
        case CTRL_AUX1:
            return "CTRL_AUX1";
        case CTRL_AUX2:
            return "CTRL_AUX2";

        default:
            return "unkown-controlID";
    }
}

std::ostream& ISPC::ModuleBase::printState(std::ostream &os) const
{
    os << SAVE_A1 << getLoggingName() << ":" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream &os, const ISPC::ModuleBase &b)
{
    return b.printState(os);
}

//
// SetupModule
//

void ISPC::SetupModule::setPipeline(Pipeline *pipeline)
{
    this->pipeline = pipeline;
}

ISPC::Pipeline* ISPC::SetupModule::getPipeline()
{
    return this->pipeline;
}

const ISPC::Pipeline* ISPC::SetupModule::getPipeline() const
{
    return this->pipeline;
}

ISPC::modulePriority ISPC::SetupModule::getPriority() const
{
    return priority;
}

void ISPC::SetupModule::setPriority(modulePriority newPriority)
{
    this->priority = newPriority;
}

bool ISPC::SetupModule::isSetup() const
{
    return setupFlag;
}

bool ISPC::SetupModule::isUpdateRequested() const
{
    return updateRequested;
}

void ISPC::SetupModule::requestUpdate()
{
    this->updateRequested = true;
}

void ISPC::SetupModule::clearFlags()
{
    setupFlag = false;
    updateRequested = false;
}

//
// ControlModule
//

//
// protected
//

std::list<ISPC::Pipeline*>::iterator ISPC::ControlModule::findPipeline(
    const Pipeline *pipe)
{
    std::list<Pipeline*>::iterator it = pipelineList.begin();

    while (it != pipelineList.end())
    {
        if (*it == pipe)
        {
            return it;
        }
        it++;
    }
    return it;
}

std::list<ISPC::Pipeline*>::const_iterator ISPC::ControlModule::findPipeline(
    const Pipeline *pipe) const
{
    std::list<Pipeline*>::const_iterator it = pipelineList.begin();

    while (it != pipelineList.end())
    {
        if (*it == pipe)
        {
            return it;
        }
        it++;
    }
    return it;
}

//
// public
//

const std::list<ISPC::Pipeline*>& ISPC::ControlModule::getPipelines() const
{
    return pipelineList;
}

void ISPC::ControlModule::clearPipelines()
{
    pipelineList.clear();
    pipelineOwner = pipelineList.end();
}

bool ISPC::ControlModule::hasPipeline(const Pipeline *pipe) const
{
    std::list<Pipeline*>::const_iterator it = findPipeline(pipe);
    return (it != pipelineList.end());
}

IMG_RESULT ISPC::ControlModule::addPipeline(Pipeline *pipe)
{
    if (!pipe)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    if (!hasPipeline(pipe) )
    {
        pipelineList.push_back(pipe);
        return IMG_SUCCESS;
    }
    return IMG_ERROR_ALREADY_INITIALISED;
}

IMG_RESULT ISPC::ControlModule::setPipelineOwner(Pipeline *pipe)
{
    std::list<Pipeline*>::iterator newowner;
    if (!pipe)
    {
        pipelineOwner = pipelineList.end();
        return IMG_SUCCESS;
    }
    newowner = findPipeline(pipe);
    if (newowner != pipelineList.end())
    {
        pipelineOwner = newowner;
        return IMG_SUCCESS;
    }
    return IMG_ERROR_INVALID_PARAMETERS;
}

ISPC::Pipeline* ISPC::ControlModule::getPipelineOwner() const
{
    if (pipelineOwner != pipelineList.end())
    {
        return *pipelineOwner;
    }
    return 0;
}

ISPC::Sensor * ISPC::ControlModule::getSensor() const
{
    if (pipelineOwner != pipelineList.end())
    {
        return (*pipelineOwner)->getSensor();
    }
    return 0;
}

IMG_RESULT ISPC::ControlModule::removePipeline(Pipeline *pipe)
{
    std::list<Pipeline*>::iterator toremove = findPipeline(pipe);
    if (toremove != pipelineList.end())
    {
        if (toremove != pipelineOwner)
        {
            pipelineList.erase(toremove);
            return IMG_SUCCESS;
        }
        return IMG_ERROR_NOT_SUPPORTED;
    }
    return IMG_ERROR_NOT_INITIALISED;
}

void ISPC::ControlModule::enableControl(bool enable)
{
    enabled = enable;
}

bool ISPC::ControlModule::isEnabled() const
{
    return enabled;
}

IMG_RESULT ISPC::ControlModule::setUpdateType(CtrlUpdateMode updateMode)
{
    this->updateMode = updateMode;
    return IMG_SUCCESS;
}

ISPC::CtrlUpdateMode ISPC::ControlModule::getUpdateType() const
{
    return this->updateMode;
}

std::ostream& ISPC::ControlModule::printState(std::ostream &os) const
{
    os << SAVE_A1 << getLoggingName() << ":" << std::endl;
    os << SAVE_A2 << "config:" << std::endl;
    os << SAVE_A3 << "enabled = " << enabled << std::endl;
    return os;
}
