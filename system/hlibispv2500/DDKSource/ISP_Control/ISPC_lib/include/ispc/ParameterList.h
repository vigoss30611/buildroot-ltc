/**
*******************************************************************************
 @file ParameterList.h

 @brief Declaration of ISPC::ParameterList and helper functions

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
#ifndef ISPC_PARAMETER_LIST_H_
#define ISPC_PARAMETER_LIST_H_

#include <img_types.h>
#include <img_errors.h>

#include <map>
#include <list>
#include <set>
#include <string>
#include <iostream>

#include "ispc/Parameter.h"

namespace ISPC {

/**
 * @brief Group of parameters by name, with a header
 */
struct ParameterGroup
{
    /** @brief Group header text */
    std::string header;

    /**
     * @brief list of parameters in that group - use std::set to have
     * logarithm find
     */
    std::set<std::string> parameters;
};

/**
 * Represents a set of tags and associated values parsed from any source.
 *
 * This class doesn't make any assumptions about the format of the values
 * associated to each tag as all of them are stored in Parameter class
 * instances which can keep an arbitrary number of values per tag as strings.
 * There is a number of methods implemented for querying for particular tags
 * and values. Is in the query functions where it can be requested to
 * retrieved any associated value of a particular type (as an integer, float
 * or string).
 * If a value can't be converted to the requested type an error will be
 * returned.
 *
 * See @ref Parameter as storage for parameters.
 *
 * See @ref ParamDef ParamDefArray and ParamDefSingle as a way to declare
 * parameters.
 *
 * See @ref ParameterFileParser to generate this object from a file.
 */
class ParameterList
{
private:
    /**
     * @brief map to store a collection of Parameter objects (which contain a
     * tag + arbitrary number of values)
     */
    std::map<std::string, Parameter> parameters;

    /** @brief map of groups - first is name, second is list of parameters */
    std::map<std::string, ParameterGroup> groups;

public:
    /**
     * @brief Flag to indicate if the parameter list if valid or not (for
     * example if failed to parse from a file)
     */
    bool validFlag;

public:
    /** @brief class ParameterList::const_iterator */
    typedef std::map<std::string, Parameter>::const_iterator const_iterator;

    /** @brief class ParameterList::iterator */
    typedef std::map<std::string, Parameter>::iterator iterator;

    /** @brief class ParameterList::const_iterator */
    typedef std::map<std::string, ParameterGroup>::const_iterator
            const_group_iterator;

    /** @brief class ParameterList::iterator */
    typedef std::map<std::string, ParameterGroup>::iterator
            group_iterator;

    /** @brief Constructor initializing defaults */
    ParameterList(): validFlag(true) {}

    /** @brief Access the parameters std::map */
    const_iterator begin() const;
    iterator begin() { return parameters.begin(); }

    /** @brief Access to the parameters std::map */
    const_iterator end() const;
    iterator end() { return parameters.end(); }

    /**
     * @brief Add a new parameter to the collection
     *
     * If it already existed (a parameter with the same tag) it will be
     * overwritten if the overwrite flag is true (default).
     *
     * @param newParam new Parameter to be added
     * @param overwrite Overwrite if the collection already contains a
     * Parameter with the same tags
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_ALREADY_INITIALISED if overwrite=false and parameter
     * is already present
     */
    IMG_RESULT addParameter(const Parameter &newParam, bool overwrite = true);

    /**
     * @brief Add a group to the list
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_ALREADY_INITIALISED if the group already exists
     */
    IMG_RESULT addGroup(const std::string &name, const ParameterGroup &group);

    /**
     * @brief Is group already registered?
     */
    bool hasGroup(const std::string &name) const;

    /**
     * @brief Access to a copy of the group definition
     */
    ParameterGroup getGroup(const std::string &name) const;

    /**
     * @brief get start of the group map
     */
    const_group_iterator groupsBegin() const;

    /**
     * @brief get end of the group map
     */
    const_group_iterator groupsEnd() const;

    /**
     * @brief Remove a group from the group map
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if the group was not found
     */
    IMG_RESULT removeGroup(const std::string &name);

    /**
     * @brief Is a tag registered in a group?
     */
    bool isInGroup(const std::string &tag) const;

    /**
     * @brief Empty the parameter collection
     */
    void clearList() { parameters.clear();}

    /**
     * @brief Check if a given parameter already exists (a parameter with
     * the same tag)
     *
     * @param tag Parameter name/tag to look for
     *
     * @return true if a parameter with the same tag already existed.
     * False otherwise.
     */
    bool exists(const std::string &tag) const;

    /**
     * @brief Returns a pointer to a Parameter object stored in the
     * collection with a given tag
     *
     * @param tag Parameter tag to look for.
     *
     * @return Pointer to a Parameter instance if one was found in the
     * collection with the requested tag. NULL otherwise.
     */
    Parameter *getParameter(const std::string &tag);

    /** @copydoc getParameter(const std::string&) */
    const Parameter *getParameter(const std::string &tag) const;

    /** @brief add 2 parameter list together */
    ParameterList& operator+=(const ParameterList &list);

    /**
     * @brief get the nth parameter as a string
     *
     * Strings are treated differently to have simpler templates
     *
     * @return found parameter value or given default string if not found
     */
    std::string getParameterString(const std::string &tag, int n,
        const std::string &tDef = std::string()) const;

    /**
     * @brief Get the nth parameter as T
     *
     * @return found value or given tDef
     *
     * Delegates to @ref Parameter::get
     */
    template <typename T> T getParameter(const std::string &str, int n,
        T tDef) const
    {
        const Parameter *param = getParameter(str);
        if (!param)
        {
            return tDef;
        }
        return param->get<T>(n, tDef);
    }

    /**
     * @brief Get the nth parameter as T
     *
     * @return found value or given tDef
     *
     * Delegates to @ref Parameter::get
     */
    template <typename T> T getParameter(const std::string &str, int n,
        T tMin, T tMax, T tDef) const
    {
        const Parameter *param = getParameter(str);
        if (!param)
        {
            return tDef;
        }
        return param->get<T>(n, tMin, tMax, tDef);
    }

    /** @brief Use parameter definition to access a parameter */
    template <typename T> T getParameter(const ParamDefSingle<T> &def) const
    {
        return getParameter<T>(def.name, 0, def.def);
    }

    /** @brief Use parameter definition to access a parameter */
    template <typename T> T getParameter(const ParamDef<T> &def) const
    {
        return getParameter<T>(def.name, 0, def.min, def.max, def.def);
    }

    /** @brief Use parameter definition to access a parameter */
    template <typename T> T getParameter(const ParamDefArray<T> &def,
        unsigned int n) const
    {
        return getParameter<T>(def.name, n, def.min, def.max,
            def.def[n%def.n]);
    }

    int removeParameter(const std::string &tag);
};

} /* namespace ISPC */

#endif /* ISPC_PARAMETER_LIST_H_ */
