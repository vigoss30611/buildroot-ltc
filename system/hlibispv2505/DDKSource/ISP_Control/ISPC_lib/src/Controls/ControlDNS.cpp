/**
*******************************************************************************
 @file ControlDNS.cpp

 @brief ISPC::ControlDNS implementation

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
#include "ispc/ControlDNS.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_DNS"

#include <string>
#include <list>

#include "ispc/ModuleDNS.h"
#include "ispc/Pipeline.h"

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

ISPC::ParameterGroup ISPC::ControlDNS::getGroup()
{
    ParameterGroup group;

    return group;
}

ISPC::ControlDNS::ControlDNS(double defaultGain, const std::string &logname)
    : ControlModuleBase(logname), fSensorGain(defaultGain)
{
}

IMG_RESULT ISPC::ControlDNS::load(const ParameterList &parameters)
{
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlDNS::save(ParameterList &parameters, SaveType t) const
{
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlDNS::update(const Metadata &metadata)
{
    const Sensor *sensor = getSensor();
    if (!sensor)
    {
        MOD_LOG_ERROR("ControlDNS has no sensor!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    // set the iso value to be programmed from the sensor gain
    fSensorGain = sensor->getGain();

    programCorrection();

    return IMG_SUCCESS;
}

bool ISPC::ControlDNS::hasConverged() const
{
    return true;
}

//
// protected
//

IMG_RESULT ISPC::ControlDNS::configureStatistics()
{
    // no stats to configure
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlDNS::programCorrection()
{
    std::list<Pipeline *>::iterator it;

    for (it = pipelineList.begin() ; it != pipelineList.end() ; it++)
    {
        ModuleDNS *dns = (*it)->getModule<ModuleDNS>();

        if ( dns )
        {
            dns->fSensorGain = fSensorGain;

#if VERBOSE_CONTROL_MODULES
            MOD_LOG_INFO("%s set to %f\n", fSensorGain);
#endif

            dns->requestUpdate();
        }
        else
        {
#if VERBOSE_CONTROL_MODULES
            MOD_LOG_WARNING("one pipeline is missing a DNS module\n");
#endif
        }
    }

    return IMG_SUCCESS;
}
