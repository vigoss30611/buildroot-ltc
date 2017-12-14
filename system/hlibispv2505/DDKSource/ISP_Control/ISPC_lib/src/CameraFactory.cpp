/**
*******************************************************************************
 @file CameraFactory.cpp

 @brief Implementation of ISPC::CameraFactory

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
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CAM_FACTORY"

#include <list>

// should be first but cpplint complains because it's not CameraFactor.h
#include "ispc/Camera.h"

// Includes for pipeline modules
#include "ispc/ModuleOUT.h"
#include "ispc/ModuleIIF.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ModuleRLT.h"  // only HW >= 2
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleEXS.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ModuleFOS.h"
#include "ispc/ModuleDNS.h"
#include "ispc/ModuleLCA.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleMGM.h"
#include "ispc/ModuleGMA.h"
#include "ispc/ModuleWBS.h"
#include "ispc/ModuleHIS.h"
#include "ispc/ModuleR2Y.h"
#include "ispc/ModuleMIE.h"
#include "ispc/ModuleVIB.h"
#include "ispc/ModuleTNM.h"
#include "ispc/ModuleFLD.h"
#include "ispc/ModuleSHA.h"
#include "ispc/ModuleESC.h"
#include "ispc/ModuleDSC.h"
#include "ispc/ModuleY2R.h"
#include "ispc/ModuleDGM.h"
#include "ispc/ModuleDPF.h"
#include "ispc/ModuleENS.h"
#include "ispc/ModuleAWS.h"

#include "ispc/ControlAE.h"
#include "ispc/ControlAF.h"
#include "ispc/ControlAWB.h"
#include "ispc/ControlAWB_PID.h"
#include "ispc/ControlAWB_Planckian.h"
#include "ispc/ControlDNS.h"
#include "ispc/ControlLBC.h"
#include "ispc/ControlTNM.h"
#include "ispc/ControlLSH.h"

#include "ispc/PerfTime.h"

#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) || defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
#include "ispc/ControlCMC.h"
#endif

// to enable to have verbose use LOG_DEBUG (mostly about state)
#define V_LOG_DEBUG

std::list<ISPC::SetupModule*>
    ISPC::CameraFactory::setupModulesFromHWVersion(unsigned int major,
    unsigned int minor)
{
    std::list<ISPC::SetupModule*> modules;

    if (0 == major)
    {
        LOG_ERROR("major version has to be >0\n");
        return modules;
    }

    // these modules are present since version 1
    modules.push_back(new ModuleOUT());
    modules.push_back(new ModuleIIF());
    modules.push_back(new ModuleEXS());
    modules.push_back(new ModuleBLC());
    modules.push_back(new ModuleLSH());
    if ((2 == major && 6 <= minor) || (2 < major))
    {
        modules.push_back(new ModuleWBC2_6());
    }
    else
    {
        modules.push_back(new ModuleWBC());
    }
    modules.push_back(new ModuleFOS());
    modules.push_back(new ModuleDNS());
    modules.push_back(new ModuleDPF());
    modules.push_back(new ModuleENS());
    modules.push_back(new ModuleLCA());
    modules.push_back(new ModuleCCM());
    modules.push_back(new ModuleMGM());
    modules.push_back(new ModuleGMA());
    modules.push_back(new ModuleWBS());
    modules.push_back(new ModuleHIS());
    modules.push_back(new ModuleR2Y());
    modules.push_back(new ModuleMIE());
    modules.push_back(new ModuleVIB());
    modules.push_back(new ModuleTNM());
    modules.push_back(new ModuleFLD());
    modules.push_back(new ModuleSHA());
    modules.push_back(new ModuleESC());
    modules.push_back(new ModuleDSC());
    modules.push_back(new ModuleY2R());
    modules.push_back(new ModuleDGM());

    if (2 <= major)
    {
        // add HW v2 modules here
        modules.push_back(new ModuleRLT());
    }

    if ((2 == major && 6 <= minor) || (2 < major))
    {
        // AWS statistics block supported since 2.6
        modules.push_back(new ModuleAWS());
    }
    return modules;
}

std::list<ISPC::ControlModule*>
    ISPC::CameraFactory::controlModulesFromHWVersion(unsigned int major,
    unsigned int minor)
{
    std::list<ISPC::ControlModule*> modules;

    if (0 == major)
    {
        LOG_ERROR("major version has to be >0\n");
        return modules;
    }

    // these modules are present since version 1
    modules.push_back(new ControlAE());
    modules.push_back(new ControlAF());
    modules.push_back(new ControlDNS());
    modules.push_back(new ControlLBC());
    modules.push_back(new ControlTNM());
    modules.push_back(new ControlLSH());

    if(major <= 2 && minor < 6) {
        // WBS block used alone up to 2.5
        modules.push_back(new ControlAWB_PID());
    } else {
        // new AWS statistics block is used since 2.6
        // which requires different AWB algorithm
        modules.push_back(new ControlAWB_Planckian());
    }
	
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) || defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    modules.push_back(new ControlCMC());
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE, INFOTM_ENABLE_GAIN_LEVEL_IDX

    return modules;
}

IMG_RESULT ISPC::CameraFactory::populateCameraFromHWVersion(Camera &camera,
    Sensor *sensor, int perf)
{
    LOG_DEBUG("Enter\n");
    Pipeline *pipeline = camera.getPipeline();
    bool bPerf = perf > 0;
    bool bVerbosePerf = perf > 1;

    IMG_RESULT result = IMG_SUCCESS;

    if (!pipeline || 0 == camera.hwInfo.rev_ui8Major)
    {
        LOG_ERROR("Invalid major version in Camera HW info, the connection "\
            "to the driver seems broken\n");
        return IMG_ERROR_FATAL;
    }

    std::list<ISPC::SetupModule*> modules =
        setupModulesFromHWVersion(camera.hwInfo.rev_ui8Major,
        camera.hwInfo.rev_ui8Minor);
    std::list<ISPC::SetupModule*>::iterator it;

    if (0 == modules.size())
    {
        LOG_ERROR("Failed to get module list for HW version %d.%d\n",
            camera.hwInfo.rev_ui8Major, camera.hwInfo.rev_ui8Minor);
        return IMG_ERROR_FATAL;
    }

    for (it = modules.begin() ; it != modules.end() ; it++)
    {
        result = pipeline->registerModule(*it);
        if (result)
        {
            break;
        }
        if (bPerf)
        {
            LOG_Perf_Register(*it, bVerbosePerf);
        }
        *it = 0;  // registered - element is empty
    }

    if (result)
    {
        LOG_DEBUG("Clear remaining modules\n");
        for (it = modules.begin() ; it != modules.end() ; it++)
        {
            if (*it)
            {
                delete *it;
                *it = 0;
            }
        }
        return result;
    }

    ISPC::ModuleIIF *iif = pipeline->getModule<ISPC::ModuleIIF>();
    if (iif && sensor)
    {
        iif->aCropBR[0] = sensor->uiWidth-1;
        iif->aCropBR[1] = sensor->uiHeight-1;
        iif->requestUpdate();

        pipeline->setupAll();  // will configure the defaults for all modules
    }

    camera.state = Camera::CAM_REGISTERED;  // Modules have been registeterd
    V_LOG_DEBUG("state=CAM_REGISTERED\n");

    return IMG_SUCCESS;
}
