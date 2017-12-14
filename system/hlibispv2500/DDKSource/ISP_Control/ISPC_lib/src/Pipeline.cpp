/**
*******************************************************************************
@file Pipeline.cpp

@brief Implementation of ISPC::Pipeline

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
#include "ispc/Pipeline.h"

#include <ci/ci_api.h>
#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_PIPELINE"

#include <map>

#include "ispc/Module.h"
#include "ispc/Sensor.h"
#include "ispc/ModuleOUT.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ModuleLSH.h"

// enable to have verbose LOG_DEBUG (mostly about state)
// #define V_LOG LOG_DEBUG
#define V_LOG_DEBUG

ISPC::SetupID ISPC::Pipeline::globalModulesID[PIPELINE_N_GLOBALS] = {
    ISPC::STP_OUT,
    ISPC::STP_IIF,
};

ISPC::Pipeline::Pipeline(CI_CONNECTION *pCIConnection, unsigned int ctxNumber,
    Sensor *sensor) : pCIConnection(pCIConnection)
{
    IMG_RESULT ret;

    pCIPipeline = NULL;
    pMCPipeline = NULL;
    this->ctxStatus = ISPC_Ctx_ERROR;

    if (!pCIConnection)
    {
        LOG_ERROR("Invalid CI_CONNETION parameter.\n");
        return;
    }

    if (!sensor)
    {
        LOG_ERROR("Invalid sensor parameter.\n");
        return;
    }

    this->sensor = sensor;

    pMCPipeline = static_cast<MC_PIPELINE *>(IMG_MALLOC(sizeof(MC_PIPELINE)));
    if (!pMCPipeline)
    {
        LOG_ERROR("allocating MC pipeline failed.\n");
        return;
    }

    ret = CI_PipelineCreate(&pCIPipeline, pCIConnection);
    if (ret)
    {
        LOG_ERROR("allocating CI Pipeline.\n");
        return;
    }

    this->ui8ContextNumber = ctxNumber;
    this->pCIPipeline->ui8Context = this->ui8ContextNumber;
    this->pCIPipeline->sImagerInterface.ui8Imager = sensor->uiImager;

    /* IIF must be configured beforehand as there are modules in the pipeline
     * which configuration depends on IIF*/
    MC_IIFInit(&(pMCPipeline->sIIF));
    pMCPipeline->sIIF.ui16ImagerSize[0] = sensor->uiWidth / CI_CFA_WIDTH;
    pMCPipeline->sIIF.ui16ImagerSize[1] = sensor->uiHeight / CI_CFA_HEIGHT;

    // set default buffer dimensions to sensor native W x H
    ui32MaxDisplayWidth = ui32MaxEncoderWidth = sensor->uiWidth;
    ui32MaxDisplayHeight = ui32MaxEncoderHeight = sensor->uiHeight;

    // initialize MC pipeline with bypass values
    MC_PipelineInit(pMCPipeline, &(pCIConnection->sHWInfo));

    if (this->ui8ContextNumber >= pCIConnection->sHWInfo.config_ui8NContexts)
    {
        LOG_ERROR("Invalid context number - max supported is %d\n",
            pCIConnection->sHWInfo.config_ui8NContexts - 1);
        return;
    }

    bEnableTiling = false;

    this->ctxStatus = ISPC_Ctx_INIT;
    V_LOG_DEBUG("ctxStatus=ISPC_Ctx_INIT\n");
}

ISPC::Pipeline::~Pipeline()
{
    this->clearModules();  // delete the modules in the module registry

    if (pCIPipeline)
    {
        // will stop the pipeline if not stopped already
        CI_PipelineDestroy(pCIPipeline);
    }

    if (pMCPipeline)
    {
        free(pMCPipeline);
    }
}

const CI_CONNECTION* ISPC::Pipeline::getConnection() const
{
    return pCIConnection;
}

MC_PIPELINE* ISPC::Pipeline::getMCPipeline()
{
    return pMCPipeline;
}

const MC_PIPELINE* ISPC::Pipeline::getMCPipeline() const
{
    return pMCPipeline;
}

const CI_PIPELINE* ISPC::Pipeline::getCIPipeline() const
{
    return pCIPipeline;
}

CI_PIPELINE* ISPC::Pipeline::getCIPipeline()
{
    return pCIPipeline;
}

ISPC::Global_Setup ISPC::Pipeline::getGlobalSetup(IMG_RESULT *ret) const
{
    Global_Setup globalSetup;

    // from init
    globalSetup.CFA_WIDTH = CI_CFA_WIDTH;
    globalSetup.CFA_HEIGHT = CI_CFA_HEIGHT;

    if (!this->sensor)
    {
        LOG_ERROR("Pipeline does not have a sensor!\n");
        if (ret) *ret = IMG_ERROR_UNEXPECTED_STATE;
        return globalSetup;
    }

    globalSetup.ui32Sensor_imager = this->sensor->uiImager;
    globalSetup.ui32Sensor_width = this->sensor->uiWidth;
    globalSetup.ui32Sensor_height = this->sensor->uiHeight;
    globalSetup.ui32Sensor_bitdepth = this->sensor->uiBitDepth;
    // note: gain can change per frame
    globalSetup.fSensor_gain = this->sensor->getGain();
    globalSetup.ui32Sensor_well_depth = this->sensor->uiWellDepth;
    globalSetup.fSensor_read_noise = this->sensor->flReadNoise;
    globalSetup.eSensor_bayer_format = this->sensor->eBayerFormat;

    globalSetup.ui32ImageWidth = globalSetup.ui32Sensor_width;
    globalSetup.ui32ImageHeight = globalSetup.ui32Sensor_height;

    // Initialize pipeline output image(s) size to invalid values
    globalSetup.ui32DispWidth = -1;
    globalSetup.ui32DispHeight = -1;

    globalSetup.ui32EncWidth = -1;
    globalSetup.ui32EncHeight = -1;

    globalSetup.ui32BlackBorder = 0;
    globalSetup.ui32SystemBlack = 64;

    //
    // from update - needs not to be erroneous
    //
    const ModuleOUT *glb = getModule<ModuleOUT>();
    const ModuleBLC *blc = getModule<ModuleBLC>();

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        if (ret)
        {
            *ret = IMG_ERROR_UNEXPECTED_STATE;
        }
        return globalSetup;
    }

    /* Update image size values after imager decimation, cropping and
     * demosaicing in IIF setup */
    globalSetup.ui32ImageWidth =
        pMCPipeline->sIIF.ui16ImagerSize[0] * CI_CFA_WIDTH;
    globalSetup.ui32ImageHeight =
        pMCPipeline->sIIF.ui16ImagerSize[1] * CI_CFA_HEIGHT;

    // Update output image values according to output scalers configuration
    globalSetup.ui32EncWidth = pMCPipeline->sESC.aOutputSize[0];
    globalSetup.ui32EncHeight = pMCPipeline->sESC.aOutputSize[1];

    if (glb)
    {
        if (glb->dataExtractionPoint != CI_INOUT_NONE &&
            glb->dataExtractionType != PXL_NONE)
        {
            globalSetup.ui32DispWidth = globalSetup.ui32ImageWidth;
            globalSetup.ui32DispHeight = globalSetup.ui32ImageHeight;
        }
        else
        {
            /* Update output image values according to output scalers
             * configuration */
            globalSetup.ui32DispWidth = pMCPipeline->sDSC.aOutputSize[0];
            globalSetup.ui32DispHeight = pMCPipeline->sDSC.aOutputSize[1];
        }
    }
    else
    {
        LOG_ERROR("Pipeline has no registered ModuleOUT!\n");
        if (ret)
        {
            *ret = IMG_ERROR_UNEXPECTED_STATE;
        }
        return globalSetup;
    }

    if (blc)
    {
        globalSetup.ui32BlackBorder = 0;  // warning: setup IIF
        globalSetup.ui32SystemBlack = blc->ui32SystemBlack;
    }
    else
    {
        LOG_WARNING("Pipeline has no registered ModuleBLC\n");
    }

    if (ret)
    {
        *ret = IMG_SUCCESS;
    }
    return globalSetup;
}

ISPC::Sensor* ISPC::Pipeline::getSensor()
{
    return sensor;
}

const ISPC::Sensor* ISPC::Pipeline::getSensor() const
{
    return sensor;
}

void ISPC::Pipeline::setSensor(Sensor *_sensor)
{
    sensor = _sensor;
}

bool ISPC::Pipeline::tilingEnabled() const
{
    return bEnableTiling;
}

void ISPC::Pipeline::setTilingEnabled(bool b)
{
    bEnableTiling = b;
}

//
// REGISTER MODULES IN THE PIPELINE
//

IMG_RESULT ISPC::Pipeline::registerModule(SetupModule *module)
{
    std::map<SetupID, SetupModule*>::iterator it;
    if (!module)
    {
        LOG_ERROR("Given module is NULL\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    SetupID id = module->getModuleID();
    it = this->moduleRegistry.find(id);

    if (it != this->moduleRegistry.end())
    {
        LOG_WARNING("Module with id = %d was previously registered\n", id);
        delete it->second;  // delete the existing module
    }

    module->setPipeline(this);  // Assign the current pipeline to the module

    /* store the module in the registry, indexed by the module ID (so we
     * don't have modules with duplicated ID) */
    this->moduleRegistry[id] = module;

    return IMG_SUCCESS;
}

ISPC::SetupModule* ISPC::Pipeline::getModule(SetupID id)
{
    std::map<SetupID, SetupModule*>::const_iterator modIt;

    modIt = this->moduleRegistry.find(id);
    if (modIt == this->moduleRegistry.end())  // module id not found
    {
        return 0;
    }

    return (modIt->second);
}

const ISPC::SetupModule* ISPC::Pipeline::getModule(SetupID id) const
{
    std::map<SetupID, SetupModule*>::const_iterator modIt;

    modIt = this->moduleRegistry.find(id);
    if (modIt == this->moduleRegistry.end())  // module id not found
    {
        return 0;
    }

    return (modIt->second);
}

void ISPC::Pipeline::clearModules()
{
    std::map<SetupID, SetupModule*>::iterator it;
    // delete all the elements in the module registre
    for (it = this->moduleRegistry.begin();
        it != this->moduleRegistry.end(); it++)
    {
        delete it->second;
    }

    // clear the container
    this->moduleRegistry.clear();
}

IMG_RESULT ISPC::Pipeline::reloadAll(const ParameterList &parameters)
{
    IMG_RESULT ret;
    std::map<SetupID, SetupModule*>::iterator it;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)
    {
        ret = it->second->load(parameters);
        if (ret)
        {
            LOG_ERROR("Failed to load module: %d\n", it->first);
            return ret;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::reloadAllModules(const ParameterList &parameters)
{
    IMG_RESULT ret;
    std::map<SetupID, SetupModule*>::iterator it;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)
    {
        int i;
        for (i = 0; i < PIPELINE_N_GLOBALS; i++)
        {
            if (it->first == Pipeline::globalModulesID[i])
            {
                break;
            }
        }
        if (PIPELINE_N_GLOBALS != i)
        {
            continue;  // skip global modules
        }

        ret = it->second->load(parameters);
        if (ret)
        {
            LOG_ERROR("Failed to load module: %d\n", it->first);
            return ret;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::reloadAllGlobals(const ParameterList &parameters)
{
    IMG_RESULT ret;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (int i = 0; i < PIPELINE_N_GLOBALS; i++)
    {
        ret = this->reloadModule(Pipeline::globalModulesID[i], parameters);
        if (ret)
        {
            LOG_ERROR("Failed to reload global module %d=module%d\n",
                i, Pipeline::globalModulesID[i]);
            return ret;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::reloadModule(SetupID id,
    const ParameterList &parameters)
{
    IMG_RESULT ret;
    std::map<SetupID, SetupModule*>::iterator modIt;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    modIt = this->moduleRegistry.find(id);

    if (modIt == this->moduleRegistry.end())
    {
        LOG_ERROR("Module not found: id=%d\n", id);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = modIt->second->load(parameters);
    if (ret)
    {
        LOG_ERROR("Failed to load module id=%d\n", id);
    }
    return ret;
}

IMG_RESULT ISPC::Pipeline::saveAll(ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    IMG_RESULT ret;
    std::map<SetupID, SetupModule*>::const_iterator it;

    for (it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)
    {
        ret = it->second->save(parameters, t);
        if (ret)
        {
            LOG_ERROR("Failed to save module: %d=%s\n", (int)it->first,
                ModuleBase::setupIDName(it->first).c_str());
            return ret;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::saveModule(SetupID id, ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    std::map<SetupID, SetupModule*>::const_iterator modIt;
    IMG_RESULT ret;
    modIt = this->moduleRegistry.find(id);

    if (modIt == this->moduleRegistry.end())
    {
        LOG_ERROR("Module not found: id=%d\n", id);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = modIt->second->save(parameters, t);
    if (ret)
    {
        LOG_ERROR("Failed to save module: %d\n", id);
    }
    return ret;
}

//
// MODULE SETUP
//

IMG_RESULT ISPC::Pipeline::setupAll()
{
    IMG_RESULT ret;

    ret = setupAllGlobals();
    if (ret)
    {
        LOG_ERROR("Failed to setup global modules\n");
        return ret;
    }

    ret = setupAllModules();
    if (ret)
    {
        LOG_ERROR("Failed to setup modules\n");
        // return ret;
    }

    return ret;
}

IMG_RESULT ISPC::Pipeline::setupAllGlobals()
{
    IMG_RESULT ret;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (int i = 0; i < PIPELINE_N_GLOBALS; i++)
    {
        ret = this->setupModule(Pipeline::globalModulesID[i]);
        if (ret)
        {
            LOG_ERROR("Failed to setup global module %d=module%d\n",
                i, Pipeline::globalModulesID[i]);
            return ret;
        }
    }

    return IMG_SUCCESS;
}


IMG_RESULT ISPC::Pipeline::setupAllModules()
{
    IMG_RESULT ret;
    std::map<SetupID, SetupModule*>::iterator it;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)
    {
        int i;
        for (i = 0; i < PIPELINE_N_GLOBALS; i++)
        {
            if (it->first == Pipeline::globalModulesID[i])
            {
                LOG_DEBUG("skip global module %d=%s\n", (int)it->first,
                    ModuleBase::setupIDName(it->first).c_str());
                break;
            }
        }
        if (PIPELINE_N_GLOBALS != i)
        {
            continue;  // skip global modules
        }

        ret = it->second->setup();

        if (ret)
        {
            LOG_ERROR("Error configuring module: %d=%s\n", (int)it->first,
                ModuleBase::setupIDName(it->first).c_str());
            return ret;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::setupModule(SetupID id)
{
    IMG_RESULT ret;
    std::map<SetupID, SetupModule*>::iterator modIt;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    modIt = this->moduleRegistry.find(id);

    if (modIt == this->moduleRegistry.end())
    {
        LOG_ERROR("Module not found: id=%d\n", id);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = modIt->second->setup();
    if (ret)
    {
        LOG_ERROR("Error configuring module: %d\n", id);
        // return ret;
    }
    return ret;
}

IMG_RESULT ISPC::Pipeline::setupByPriority(modulePriority priority)
{
    IMG_RESULT ret;
    std::map<SetupID, SetupModule*>::iterator it;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)
    {
        if (it->second->getPriority() <= priority)
        {
            ret = it->second->setup();

            if (ret)
            {
                LOG_ERROR("Failed to configure module: %d\n", it->first);
                return ret;
            }
        }
    }

    return ret;
}

IMG_RESULT ISPC::Pipeline::setupPending()
{
    IMG_RESULT ret = IMG_SUCCESS;
    std::map<SetupID, SetupModule*>::iterator it;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)
    {
        if (!it->second->isSetup())
        {
            ret = it->second->setup();

            if (ret)
            {
                LOG_ERROR("Failed to configure module: %d\n", it->first);
                return ret;
            }
        }
    }

    return ret;
}

IMG_RESULT ISPC::Pipeline::setupRequested()
{
    IMG_RESULT ret = IMG_SUCCESS;
    std::map<SetupID, SetupModule*>::iterator it;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    for (it = moduleRegistry.begin(); it != moduleRegistry.end(); it++)
    {
        if (it->second->isUpdateRequested())
        {
            ret = it->second->setup();

            if (ret)
            {
                LOG_ERROR("Failed to configure module: %d\n", it->first);
                return ret;
            }
            it->second->clearFlags();
        }
    }

    return ret;
}

IMG_RESULT ISPC::Pipeline::verifyConfiguration() const
{
    const ModuleOUT *glb = getModule<ModuleOUT>();

    if (0 == (pCIConnection->sHWInfo.\
        eFunctionalities&CI_INFO_SUPPORTED_TILING)
        && bEnableTiling)
    {
        LOG_ERROR("The HW %d.%d does not support tiled output\n",
            pCIConnection->sHWInfo.rev_ui8Major,
            pCIConnection->sHWInfo.rev_ui8Minor);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (0 == (pCIConnection->sHWInfo.\
        eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT)
        && PXL_NONE != glb->hdrExtractionType)
    {
        LOG_ERROR("The HW %d.%d does not support HDR Extraction point\n",
            pCIConnection->sHWInfo.rev_ui8Major,
            pCIConnection->sHWInfo.rev_ui8Minor);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    if (0 == (pCIConnection->sHWInfo.\
        eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT)
        && PXL_NONE != glb->raw2DExtractionType)
    {
        LOG_ERROR("The HW %d.%d does not support RAW2D Extraction point\n",
            pCIConnection->sHWInfo.rev_ui8Major,
            pCIConnection->sHWInfo.rev_ui8Minor);
        return IMG_ERROR_NOT_SUPPORTED;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::programPipeline(bool bUpdateASAP)
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (ISPC_Ctx_UNINIT == this->ctxStatus)
    {
        LOG_ERROR("Pipeline not initialized\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = verifyConfiguration();
    if (ret)
    {
        LOG_ERROR("Configuration not supported\n");
        return ret;
    }

    ret = MC_PipelineConvert(pMCPipeline, pCIPipeline);
    if (ret)
    {
        LOG_ERROR("failed to convert the pipeline (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }

    pCIPipeline->ui16MaxDispOutWidth = ui32MaxDisplayWidth;
    pCIPipeline->ui16MaxDispOutHeight = ui32MaxDisplayHeight;
    pCIPipeline->ui16MaxEncOutWidth = ui32MaxEncoderWidth;
    pCIPipeline->ui16MaxEncOutHeight = ui32MaxEncoderHeight;

    if (pCIConnection->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_TILING)
    {
        pCIPipeline->bSupportTiling = bEnableTiling ? IMG_TRUE : IMG_FALSE;
    }
    else
    {
        if (bEnableTiling)
        {
            LOG_WARNING("Force tiling enabled to false because HW does "\
                "not support it\n");
        }
        pCIPipeline->bSupportTiling = IMG_FALSE;
    }

    if (ISPC_Ctx_SETUP != this->ctxStatus
        && ISPC_Ctx_READY != this->ctxStatus)
    {

        ret = CI_PipelineRegister(pCIPipeline);
        if (ret)
        {
            LOG_ERROR("Failed to register the pipeline configuration "\
                "(returned %d)\n", ret);
            return IMG_ERROR_FATAL;
        }
        this->ctxStatus = ISPC_Ctx_SETUP;
    }
    else
    {
        int moduleFailure = 0;
        int toUpdate = CI_UPD_ALL;
        // update the next submitted frame

        if (CI_PipelineIsStarted(pCIPipeline))
        {
            /* do not update LSH - it needs access to registers and preload
             * matrix */
            /* do not update DPF input map it needs to preload map */
            /* this could be changed to just check if we have pushed frames
            * or not but it is safer to not allow it to change if started */
            toUpdate &= (~CI_UPD_REG);
        }
        if (bUpdateASAP)
        {
            ret = CI_PipelineUpdateASAP(pCIPipeline, toUpdate, &moduleFailure);
        }
        else
        {
            ret = CI_PipelineUpdate(pCIPipeline, toUpdate, &moduleFailure);
        }
        if (ret)
        {
            LOG_ERROR("Failed to update the pipeline configuration for "\
                "module %d (returned %d)\n", moduleFailure, ret);
            return IMG_ERROR_FATAL;
        }
    }

    return IMG_SUCCESS;
}

//
// Buffers and Shots allocation management
//

IMG_RESULT ISPC::Pipeline::addShots(unsigned int num)
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (num < 1)
    {
        LOG_ERROR("At least one buffer must be requested.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    ret = CI_PipelineAddPool(pCIPipeline, num);
    if (ret)
    {
        LOG_ERROR("Failed to add Shots to the pipeline (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }

    /* it is not started we should only have available when both Shot and
     * Buffer are there but it is legitimate to run the pipeline with only
     * statistics as an output therefore we should not check for Buffers
     * (to check both buffers and shot use CI_PipelineHasAvailable()) */
    if (CI_PipelineHasAvailableShots(pCIPipeline))
    {
        this->ctxStatus = ISPC_Ctx_READY;
        V_LOG_DEBUG("ctxStatus=ISC_Ctx_READY\n");
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::deleteShots()
{
    IMG_RESULT ret = IMG_SUCCESS;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CI_PipelineIsStarted(pCIPipeline))
    {
        LOG_ERROR("Pipeline is started - cannot delete shots\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    /* unlikely if pipeline is actually stopped as we checked before but
     * needed for CI call */
    if (CI_PipelineHasPending(pCIPipeline))
    {
        LOG_ERROR("Pipeline has pending Shots - cannot delete shots\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (CI_PipelineHasAcquired(pCIPipeline))
    {
        LOG_ERROR("Pipeline has acquired Shots - cannot delete shots\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineDeleteShots(pCIPipeline);
    if (ret)
    {
        LOG_ERROR("Failed to delete Pipeline's shots\n");
        // return ret;
    }

    return ret;
}

IMG_RESULT ISPC::Pipeline::allocateBuffer(CI_BUFFTYPE eBuffer,
    IMG_UINT32 ui32Size, bool isTiled, IMG_UINT32 *pBufferId)
{
    IMG_RESULT ret = IMG_ERROR_UNEXPECTED_STATE;  // no output configured

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (ISPC_Ctx_SETUP != this->ctxStatus
        && ISPC_Ctx_READY != this->ctxStatus)
    {
        LOG_ERROR("Context is not set up. Unabel to allocate buffer\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineAllocateBuffer(pCIPipeline, eBuffer, ui32Size,
        isTiled ? IMG_TRUE : IMG_FALSE, pBufferId);
    if (ret)
    {
        LOG_ERROR("Failed to allocate buffer (ret=%d)\n", ret);
        this->ctxStatus = ISPC_Ctx_ERROR;
        V_LOG_DEBUG("ctxStatus=ISPC_Ctx_ERROR\n");
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::importBuffer(CI_BUFFTYPE eBuffer,
    IMG_UINT32 ionFd, IMG_UINT32 ui32Size, bool isTiled,
    IMG_UINT32 *pBufferId)
{
    IMG_RESULT ret;
    if (CI_TYPE_NONE == eBuffer || 0 == ionFd)
    {
        LOG_ERROR("eBuffer is NONE or ionFD is 0\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!((ISPC_Ctx_SETUP == this->ctxStatus) ||
        (ISPC_Ctx_READY == this->ctxStatus)))
    {
        LOG_ERROR("Context is not set up. Unable to import buffer\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineImportBuffer(this->pCIPipeline, eBuffer, ui32Size,
        isTiled ? IMG_TRUE : IMG_FALSE, ionFd, pBufferId);
    if (ret)
    {
        LOG_ERROR("Failed to import buffer (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }

    this->ctxStatus = ISPC_Ctx_READY;
    V_LOG_DEBUG("ctxStatus=ISPC_Ctx_READY\n");
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::deregisterBuffer(IMG_UINT32 uiBufferID)
{
    IMG_RESULT ret;

    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    // there is no status to know if the pipeline is running...

    ret = CI_PipelineDeregisterBuffer(this->pCIPipeline, uiBufferID);
    if (ret)
    {
        LOG_ERROR("Failed to deregister buffer %u (returned %d)\n",
            uiBufferID, ret);
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::setDisplayDimensions(unsigned int width,
    unsigned int height)
{
    /// @ check if running here
    LOG_DEBUG("Setting the pipeline display dimensions to %dx%d",
        width, height);
    ui32MaxDisplayWidth = width;
    ui32MaxDisplayHeight = height;
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::setEncoderDimensions(unsigned int width,
    unsigned int height)
{
    /// @ check if running here
    LOG_DEBUG("Setting the pipeline encoder dimensions to %dx%d",
        width, height);
    ui32MaxEncoderWidth = width;
    ui32MaxEncoderHeight = height;
    return IMG_SUCCESS;
}

IMG_UINT32 ISPC::Pipeline::getMaxDisplayWidth() const
{
    return ui32MaxDisplayWidth;
}

IMG_UINT32 ISPC::Pipeline::getMaxDisplayHeight() const
{
    return ui32MaxDisplayHeight;
}

IMG_UINT32 ISPC::Pipeline::getMaxEncoderWidth() const
{
    return ui32MaxEncoderWidth;
}

IMG_UINT32 ISPC::Pipeline::getMaxEncoderHeight() const
{
    return ui32MaxEncoderHeight;
}

#ifdef INFOTM_ISP
IMG_RESULT ISPC::Pipeline::setYUVConst(int flag)
{
	IMG_RESULT ret;

	ret = CI_PipelineSetYUVConst(pCIPipeline, flag);
	if (ret)
	{
		LOG_ERROR("Failed to set YUV const (returned %d)\n", ret);
		return IMG_ERROR_FATAL;
	}


	return IMG_SUCCESS;
}
#endif

//
// PIPELINE CAPTURE CONTROL
//

IMG_RESULT ISPC::Pipeline::startCapture()
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (ISPC_Ctx_READY != this->ctxStatus)
    {
        LOG_ERROR("Pipeline not ready for starting capture.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!CI_PipelineIsStarted(pCIPipeline))
    {
        ret = CI_PipelineComputeLinestore(pCIPipeline);
        if (ret)
        {
            LOG_ERROR("Failed to compute the linestore before starting the "\
                "capture (returned %d)\n", ret);
            return IMG_ERROR_FATAL;
        }

        ret = CI_PipelineStartCapture(pCIPipeline);
        if (ret)
        {
            LOG_ERROR("Failed to start capture (returned %d)\n", ret);
            return IMG_ERROR_FATAL;
        }
    }
    else
    {
        LOG_WARNING("capture already started.\n");
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::stopCapture()
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (ISPC_Ctx_READY != this->ctxStatus)
    {
        LOG_ERROR("Pipeline not ready.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineStopCapture(pCIPipeline);
    if (ret)
    {
        LOG_ERROR("Failed to stop the capture (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::programSpecifiedShot(const CI_BUFFID &buffId)
{
    IMG_RESULT ret;

    if (ISPC_Ctx_READY != this->ctxStatus)
    {
        LOG_ERROR("Pipeline not ready for programming a shot\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!CI_PipelineIsStarted(this->pCIPipeline))
    {
        LOG_ERROR("Capture not started, unable to program shot.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineTriggerSpecifiedShoot(this->pCIPipeline, &buffId);
    if (ret)
    {
        LOG_ERROR("Could not trigger specified shoot (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}



IMG_RESULT ISPC::Pipeline::programShot()
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (ISPC_Ctx_READY != this->ctxStatus)
    {
        LOG_ERROR("Pipeline not ready for programming a shot\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (!CI_PipelineIsStarted(pCIPipeline))
    {
        LOG_ERROR("Capture not started, unable to program shot.\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineTriggerShoot(pCIPipeline);
    if (ret)
    {
        LOG_ERROR("Could not capture a frame (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::getFirstAvailableBuffers(CI_BUFFID &buffId)
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineFindFirstAvailable(pCIPipeline, &buffId);
    if (ret)
    {
        LOG_ERROR("Failed to find first available buffer (returned %d)\n",
            ret);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::getAvailableHDRInsertion(CI_BUFFER &HDRInsertion,
    IMG_UINT32 id)
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    ret = CI_PipelineAcquireHDRBuffer(pCIPipeline, &HDRInsertion, id);
    if (ret)
    {
        LOG_ERROR("Failed to find available HDR buffer (id=%d returned %d)\n",
            id, ret);
        return IMG_ERROR_FATAL;
    }
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::acquireShot(Shot &shot, bool block)
{
    IMG_RESULT ret;
    if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
        LOG_ERROR("Pipeline is in error state\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    CI_SHOT *pCIBuffer = NULL;
    if (block)
    {
        ret = CI_PipelineAcquireShot(pCIPipeline, &pCIBuffer);
        if (ret || !pCIBuffer)
        {
            // blocking call - print error as it timed-out
            LOG_ERROR("Failed to acquire buffer with blocking call "\
                "(returned %d, pCIBuffer=0x%p)\n", ret, pCIBuffer);
            return IMG_ERROR_FATAL;
        }
    }
    else
    {
        ret = CI_PipelineAcquireShotNB(pCIPipeline, &pCIBuffer);
        if (ret || !pCIBuffer)
        {
            if (IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE == ret)
            {
                /* non-blocking call do not print error as did not wait
                 * for frame */
                return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
            }
            LOG_ERROR("Failed to acquire buffer with non-blocking call "\
                "(returned %d, pCIBuffer=0x%p)\n", ret, pCIBuffer);
            return IMG_ERROR_FATAL;
        }
    }

    processShot(shot, pCIBuffer);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::Pipeline::releaseShot(Shot &shot)
{
    IMG_RESULT ret;
    /*if (ISPC_Ctx_ERROR == this->ctxStatus)
    {
    LOG_ERROR("Pipeline is in error state\n");
    return IMG_ERROR_UNEXPECTED_STATE;
    }*/

    // release buffer
    ret = CI_PipelineReleaseShot(pCIPipeline, shot.pCIBuffer);
    if (ret)
    {
        LOG_ERROR("Failed to release the buffer (returned %d)\n", ret);
        return IMG_ERROR_FATAL;
    }

    IMG_MEMSET(&(shot.RGB), 0, sizeof(Buffer));
    shot.RGB.pxlFormat = PXL_NONE;

    IMG_MEMSET(&(shot.YUV), 0, sizeof(Buffer));
    shot.YUV.pxlFormat = PXL_NONE;

    IMG_MEMSET(&(shot.BAYER), 0, sizeof(Buffer));
    shot.BAYER.pxlFormat = PXL_NONE;

    return IMG_SUCCESS;
}

//
// this is private
//

void ISPC::Pipeline::processShot(Shot &shot, CI_SHOT *pCIBuffer)
{
    const ModuleOUT *glb = getModule<const ModuleOUT>();

    IMG_ASSERT(pCIBuffer != NULL);

    shot.clear();

    shot.configure(pCIBuffer, *glb, this->getGlobalSetup());
}
