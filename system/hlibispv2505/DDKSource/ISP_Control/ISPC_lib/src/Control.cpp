/**
*******************************************************************************
 @file Control.cpp

 @brief Implementation of ISPC::Control

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
#include "ispc/Control.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL"

#include <map>

ISPC::Control::~Control()
{
    // delete all the ControlModule instances registered.
    clearModules();
}

IMG_RESULT ISPC::Control::registerControlModule(ControlModule *module,
    Pipeline *pipeline)
{
    if (!module)
    {
        LOG_ERROR("NULL module provided");
        return IMG_ERROR_FATAL;
    }

    ControlID id = module->getModuleID();
    std::map<ControlID, ControlModule*>::iterator it =
        this->controlRegistry.find(id);

    if (it != this->controlRegistry.end())
    {
        LOG_WARNING("Module with id = %d was previously registered\n", id);
        delete it->second;  // delete the existing control module
    }

    // Assign the current pipeline to the control module
    module->addPipeline(pipeline);
    module->setPipelineOwner(pipeline);
    // must be call after pipline and sensor instantiation
    module->init();

    /* store the module in the registry, indexed by the module ID
     * (so we don't modules with duplicated ID) */
    this->controlRegistry[id] = module;
    return IMG_SUCCESS;
}

ISPC::ControlModule *ISPC::Control::getControlModule(ControlID id)
{
    std::map<ControlID, ControlModule*>::iterator modIt;

    modIt = this->controlRegistry.find(id);
    if (modIt == this->controlRegistry.end())  // module id not found
    {
        return NULL;
    }

    return (modIt->second);
}

IMG_RESULT ISPC::Control::runControlModule(ControlID id,
    const Metadata &metadata)
{
    std::map<ControlID, ControlModule*>::iterator modIt;

    modIt = this->controlRegistry.find(id);
    if (modIt == this->controlRegistry.end())  // module id not found
    {
        LOG_ERROR("Module with id %d not found.\n", id);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return modIt->second->update(metadata);
}

IMG_RESULT ISPC::Control::runControlModules(const Metadata &metadata)
{
    bool errorFlag = false;
    std::map<ControlID, ControlModule*>::iterator modIt;
    IMG_RESULT ret;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        LOG_DEBUG("Run Control module %d\n", modIt->first);
        if (modIt->second->isEnabled())
        {
            ret = modIt->second->update(metadata);
            if (ret)
            {
                LOG_ERROR("Error while updating module with id %d\n",
                    modIt->first);
                errorFlag = true;
            }
        }
    }

    if (errorFlag)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Control::runControlModules(CtrlUpdateMode updateMode,
    const Metadata &metadata)
{
    bool errorFlag = false;
    std::map<ControlID, ControlModule*>::iterator modIt;
    IMG_RESULT ret;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        // only update the control modules with the given type
        if (modIt->second->isEnabled()
            && modIt->second->getUpdateType() == updateMode)
        {
            LOG_DEBUG("Run Control module %d\n", modIt->first);
            ret = modIt->second->update(metadata);
            if (ret)
            {
                LOG_ERROR("Failed to update module with id %d\n",
                    modIt->first);
                errorFlag = true;
            }
        }
    }

    if (errorFlag)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Control::loadAll(const ParameterList &parameters)
{
    IMG_RESULT ret;
    std::map<ControlID, ControlModule*>::iterator modIt;
    bool errorFlag = false;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        ret = modIt->second->load(parameters);
        if (ret)
        {
            LOG_ERROR("Failed to load module with id %d\n", modIt->first);
            errorFlag = true;
        }
    }

    if (errorFlag)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Control::loadControlModule(ControlID id,
    const ParameterList &parameters)
{
    std::map<ControlID, ControlModule*>::const_iterator modIt;

    modIt = this->controlRegistry.find(id);
    if (modIt == this->controlRegistry.end())  // module id not found
    {
        LOG_ERROR("Module with id %d not found.\n", id);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    return modIt->second->load(parameters);
}

IMG_RESULT ISPC::Control::saveAll(ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    bool errorFlag = false;
    std::map<ControlID, ControlModule*>::const_iterator modIt;
    IMG_RESULT ret;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        ret = modIt->second->save(parameters, t);
        if (ret)
        {
            LOG_ERROR("Failed to save module with id %d\n", modIt->first);
            errorFlag = true;
        }
    }

    if (errorFlag)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Control::configureStatistics()
{
    bool errorFlag = false;
    std::map<ControlID, ControlModule*>::const_iterator modIt;
    IMG_RESULT ret;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        ret = modIt->second->configureStatistics();
        if (ret)
        {
            LOG_ERROR("Failed to configure the statistics for module with"\
                "id %d\n", modIt->first);
            errorFlag = true;
        }
    }

    if (errorFlag)
    {
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Control::saveControlModule(ControlID id,
    ParameterList &parameters, ModuleBase::SaveType t) const
{
    std::map<ControlID, ControlModule*>::const_iterator modIt;

    modIt = this->controlRegistry.find(id);
    if (modIt == this->controlRegistry.end())  // module id not found
    {
        LOG_ERROR("Module with id %d not found.\n", id);
        return IMG_ERROR_FATAL;
    }

    return modIt->second->save(parameters, t);
}

IMG_RESULT ISPC::Control::clearModules()
{
    // delete all the elements in the module registre
    std::map<ControlID, ControlModule*>::iterator modIt;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        delete modIt->second;  // delete the existing control module
    }

    // clear the container
    this->controlRegistry.clear();

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Control::addPipelineToAll(Pipeline *pipeline)
{
    std::map<ControlID, ControlModule*>::iterator modIt;
    IMG_RESULT ret = IMG_SUCCESS;
    int failed = 0;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        ret = modIt->second->addPipeline(pipeline);
        if (ret)
        {
            failed++;
        }
    }

    if (0 < failed)
    {
        LOG_ERROR("Failed to add pipeline to %d modules\n", failed);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Control::setPipelineOwnerToAll(Pipeline *pipeline)
{
    std::map<ControlID, ControlModule*>::iterator modIt;
    IMG_RESULT ret = IMG_SUCCESS;
    int failed = 0;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        ret = modIt->second->setPipelineOwner(pipeline);
        if (ret)
        {
            failed++;
        }
    }

    if (0 < failed)
    {
        LOG_ERROR("Failed to set pipeline owner to %d modules\n", failed);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

std::ostream& ISPC::Control::printAllState(std::ostream &os) const
{
    std::map<ControlID, ControlModule*>::const_iterator modIt;
    IMG_RESULT ret = IMG_SUCCESS;
    int failed = 0;
    os << "Controls:" << std::endl;

    for (modIt = controlRegistry.begin(); modIt != controlRegistry.end();
        modIt++)
    {
        os << *(modIt->second) << std::endl;
    }

    return os;
}
