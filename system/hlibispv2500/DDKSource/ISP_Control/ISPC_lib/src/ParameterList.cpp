/**
*******************************************************************************
 @file ParameterList.cpp

 @brief Implementation of ISPC::ParameterList

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
#include "ispc/ParameterList.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_PARAMLIST"

#include <string>
#include <map>

#ifndef ISPC_PARAM_FOUND_VERBOSE
#define ISPC_PARAM_FOUND_VERBOSE 0
#endif

ISPC::ParameterList::const_iterator
ISPC::ParameterList::begin() const
{
    return parameters.begin();
}

ISPC::ParameterList::const_iterator
ISPC::ParameterList::end() const
{
    return parameters.end();
}

IMG_RESULT ISPC::ParameterList::addParameter(const Parameter &newParam,
    bool overwrite)
{
    iterator it = parameters.find(newParam.getTag());

    if (it == parameters.end())  // parameter didn't exist
    {
        parameters[newParam.getTag()] = newParam;
    }
    else
    {
#if ISPC_PARAM_FOUND_VERBOSE
        LOG_DEBUG("Paramater '%s' already exists (overriding=%d)\n",
            newParam.getTag().c_str(), overwrite?1:0);
#endif
        if (overwrite)
        {
            parameters[newParam.getTag()] = newParam;
        }
        else
        {
            return IMG_ERROR_ALREADY_INITIALISED;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ParameterList::addGroup(const std::string &name,
    const ParameterGroup &group)
{
    if ( group.parameters.size() > 0 && !hasGroup(name) )
    {
        groups[name] = group;
        return IMG_SUCCESS;
    }
    return IMG_ERROR_ALREADY_INITIALISED;
}

bool ISPC::ParameterList::hasGroup(const std::string &name) const
{
    const_group_iterator it = groups.find(name);
    if ( it == groups.end() )
    {
        return false;
    }
    return true;
}

ISPC::ParameterGroup ISPC::ParameterList::getGroup(
    const std::string &name) const
{
    const_group_iterator it = groups.find(name);
    if ( it == groups.end() )
    {
        return ParameterGroup();
    }
    return it->second;
}

ISPC::ParameterList::const_group_iterator
ISPC::ParameterList::groupsBegin() const
{
    return groups.begin();
}

ISPC::ParameterList::const_group_iterator
ISPC::ParameterList::groupsEnd() const
{
    return groups.end();
}

IMG_RESULT ISPC::ParameterList::removeGroup(const std::string &name)
{
    group_iterator it = groups.find(name);
    if ( it == groups.end() )
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    groups.erase(it);
    return IMG_SUCCESS;
}

bool ISPC::ParameterList::isInGroup(const std::string &tag) const
{
    const_group_iterator it;
    for ( it = groups.begin() ; it != groups.end() ; it++ )
    {
        if ( it->second.parameters.find(tag) != it->second.parameters.end() )
        {
            return true;
        }
    }
    return false;
}

ISPC::Parameter *ISPC::ParameterList::getParameter(const std::string &tag)
{
    iterator it = parameters.find(tag);
    if (it != parameters.end())  // parameter exists
    {
        return &(it->second);
    }
    return NULL;  // parameter didn't exist
}

const ISPC::Parameter *ISPC::ParameterList::getParameter(
    const std::string &tag) const
{
    const_iterator it = parameters.find(tag);
    if (it != parameters.end())  // parameter exists
    {
        return &(it->second);
    }
    return NULL;  // parameter didn't exist
}

bool ISPC::ParameterList::exists(const std::string &tag) const
{
    const_iterator it = parameters.find(tag);
    if (it != parameters.end())  // parameter exists
    {
        return true;
    }
    return false;
}

ISPC::ParameterList& ISPC::ParameterList::operator+=(const ParameterList &list)
{
    const_iterator it;
    for (it = list.begin(); it != list.end(); it++)
    {
        this->addParameter((*it).second);
    }
    return *this;
}

std::string ISPC::ParameterList::getParameterString(const std::string &tag,
    int n, const std::string &tDef) const
{
    const Parameter *param = getParameter(tag);

    if (!param)
    {
#if ISPC_PARAM_FOUND_VERBOSE
        LOG_DEBUG("Parameter '%s' not found\n", tag.c_str());
#endif
        return tDef;
    }

    return param->getString(n, tDef);
}

int ISPC::ParameterList::removeParameter(const std::string &tag)
{
    iterator it = parameters.find(tag);
    if (it == parameters.end())
    {
        return IMG_ERROR_FATAL;
    }
    parameters.erase(it);
    return IMG_SUCCESS;
}
