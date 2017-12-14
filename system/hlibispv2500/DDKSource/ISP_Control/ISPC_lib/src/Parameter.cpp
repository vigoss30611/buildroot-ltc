/**
*******************************************************************************
@file Parameter.cpp

@brief Implementation of Parameter class

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
#include "ispc/Parameter.h"

#include <img_errors.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_PARAMETER"

#include <sstream>
#include <string>
#include <vector>

template <>
std::string ISPC::toString<bool>(const bool &value)
{
    if (value)
    {
        return "1";
    }
    return "0";
}

template <>
std::string ISPC::toString<IMG_BOOL8>(const IMG_BOOL8 &value)
{
    if (value)
    {
        return "1";
    }
    return "0";
}

template <>
bool ISPC::parse<bool>(const std::string &str, bool &isOk)
{
    int r = parse<int>(str, isOk);
    return r ? true : false;
}

template <>
IMG_BOOL8 ISPC::parse<IMG_BOOL8>(const std::string &str, bool &isOk)
{
    int r = parse<int>(str, isOk);
    return r ? IMG_TRUE : IMG_FALSE;
}

ISPC::Parameter::Parameter(const std::string &_tag, const std::string &value)
    : tag(_tag)
{
    // check that we have a tag with at least one character
    if (tag.length() < 1)
    {
        if (value.length() != 0)
        {
            LOG_DEBUG("Invalide Tag '%s'\n", tag.c_str());
        }
        // else == Parameter("", "") is called from ParameterList::addParameter
        //      and after object creation it will copy its members values
        //      from already existing object - so pass constructor in silence
        validParameter = false;
    }
    else
    {
        validParameter = true;
        addValue(value);
    }
}

std::vector<std::string>::const_iterator ISPC::Parameter::begin() const
{
    return values.begin();
}

std::vector<std::string>::const_iterator ISPC::Parameter::end() const
{
    return values.end();
}

unsigned ISPC::Parameter::size() const
{
    return values.size();
}

std::string ISPC::Parameter::getTag() const
{
    return this->tag;
}

ISPC::Parameter::Parameter(const std::string &_tag,
    const std::vector<std::string> &_values)
    : tag(_tag)
{
    // check that we have a tag with at least one character
    if (tag.length() < 1)
    {
        LOG_DEBUG("Invalide Tag '%s'\n", tag.c_str());
        validParameter = false;
    }
    else
    {
        std::vector<std::string>::const_iterator it = _values.begin();
        while (it != _values.end())
        {
            values.push_back(*it);
            it++;
        }

        validParameter = true;
    }
}

IMG_RESULT ISPC::Parameter::addValue(const std::string &newValue)
{
    if (!validParameter)
    {
        LOG_DEBUG("Trying to add value '%s' to invalid parameter\n",
            newValue.c_str());
        return IMG_ERROR_FATAL;
    }

    if (newValue.length() == 0)
    {
        LOG_DEBUG("Trying to add empty value\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    values.push_back(newValue);
    return IMG_SUCCESS;
}

std::string ISPC::Parameter::getString(unsigned int n) const
{
    if (!validParameter)
    {
        LOG_DEBUG("Trying to get value from invalid parameter '%s'\n",
            tag.c_str());
        return std::string();  // return an empty string
    }

    if (values.size() <= n)
    {
        LOG_DEBUG("Trying to get value %d of %d values for parameter '%s'\n",
            n, values.size(), tag.c_str());
        return std::string();  // return an empty string
    }

    return values[n];
}

std::string ISPC::Parameter::getString(unsigned int n,
    const std::string &defVal) const
{
    if (!validParameter)
    {
        LOG_DEBUG("Trying to get value from invalid parameter '%s'\n",
            tag.c_str());
        return std::string();  // return an empty string
    }

    if (values.size() <= n)
    {
        LOG_DEBUG("Trying to get value %d of %d values for parameter '%s'\n",
            n, values.size(), tag.c_str());
        return defVal;
    }

    return values[n];
}

std::string ISPC::getParameterInfo(const ParamDefSingle<bool> &t)
{
    return "bool {0,1}";
}

std::string ISPC::getParameterInfo(const ParamDefSingle<std::string> &t)
{
    return "string";
}

std::string ISPC::getParameterInfo(const ParamDef<int> &t)
{
    std::ostringstream os;
    os << "int range=[" << t.min << "," << t.max << "]";
    return os.str();
}

std::string ISPC::getParameterInfo(const ParamDef<unsigned> &t)
{
    std::ostringstream os;
    os << "unsigned range=[" << t.min << "," << t.max << "]";
    return os.str();
}

std::string ISPC::getParameterInfo(const ParamDef<float> &t)
{
    std::ostringstream os;
    os << "float range=[" << t.min << "," << t.max << "]";
    return os.str();
}

std::string ISPC::getParameterInfo(const ParamDef<double> &t)
{
    std::ostringstream os;
    os << "double range=[" << t.min << "," << t.max << "]";;
    return os.str();
}

std::string ISPC::getParameterInfo(const ParamDefArray<int> &t)
{
    std::ostringstream os;
    os << "int[" << t.n << "] range=[" << t.min << "," << t.max << "]";
    return os.str();
}

std::string ISPC::getParameterInfo(const ParamDefArray<unsigned> &t)
{
    std::ostringstream os;
    os << "unsigned[" << t.n << "] range=[" << t.min << "," << t.max << "]";
    return os.str();
}

std::string ISPC::getParameterInfo(const ParamDefArray<float> &t)
{
    std::ostringstream os;
    os << "float[" << t.n << "] range=[" << t.min << "," << t.max << "]";
    return os.str();
}

std::string ISPC::getParameterInfo(const ParamDefArray<double> &t)
{
    std::ostringstream os;
    os << "double[" << t.n << "] range=[" << t.min << "," << t.max << "]";
    return os.str();
}
