/**
*******************************************************************************
 @file Parameter.h

 @brief Declaration of ISPC::Parameter and helper functions

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
#ifndef ISPC_PARAMETER_H_
#define ISPC_PARAMETER_H_

#include <img_types.h>

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>  // std::min/std::max

namespace ISPC {

/** @brief Save a type to string */
template <typename T>
std::string toString(const T &value)
{
    std::stringstream ss;
    ss << value;

    return ss.str();
}

/** @brief Overload to save boolean as string of 1 or 0 */
template <>
std::string toString<bool>(const bool &value);

/** @brief Overload to save boolean as string of 1 or 0 */
template <>
std::string toString<IMG_BOOL8>(const IMG_BOOL8 &value);

/** @brief Converts a string to a type */
template <typename T>T parse(const std::string &str, bool &isOk)
{
    std::istringstream iss(str);
    T r = T();
    iss >> std::noskipws >> r;
    // noskipws considers leading white-space invalid
    // Check the entire string was consumed and if either failbit or
    // badbit is set to know if conversion was correct
    isOk = iss.eof() && !iss.fail();
    return r;
}

/** @brief Overload to load boolean from 1 or 0 as int */
template <> bool parse<bool>(const std::string &str, bool &isOk);

/** @brief Overload to load IMG_BOOL8 from 1 or 0 as int */
template <> IMG_BOOL8 parse<IMG_BOOL8>(const std::string &str, bool &isOk);

/**
 * @brief A parameter with a label and a variable number of values associated.
 *
 * Values are stored as strings and dynamically converted when required
 */
class Parameter
{
private:
    /** @brief parameter tag (name of the parameter */
    std::string tag;
    /**
     * @brief vector of strings with all the values associated to a given tag
     */
    std::vector<std::string> values;
    /** @brief flag to indicate if the parameter is valid or not */
    bool validParameter;

public:
    Parameter(const std::string &tag = std::string(),
        const std::string &value = std::string());

    Parameter(const std::string &tag, const std::vector<std::string> &values);

    /**
     * @brief Function for adding a value to the list of values associated to
     * this parameter/tag
     *
     * @param newValue new value to be added to the list
     * @return IMG_SUCCESS
     * @return IMG_ERROR_FATAL if parameter is marked as invalid
     * @return IMG_ERROR_INVALID_PARAMETERS if given new value is empty
     */
    IMG_RESULT addValue(const std::string &newValue);

    std::vector<std::string>::const_iterator begin() const;

    std::vector<std::string>::const_iterator end() const;

    unsigned size() const;

public:
    /** @brief Access the tag (name) of this parameter */
    std::string getTag() const;

    /**
     * @brief Access the nth element in the list of values associated to this
     * tag as a string
     * @param n Position of the value to be retrieved
     */
    std::string getString(unsigned int n = 0) const;

    /**
     * @brief Access the nth element in the list of values associated to this
     * tag as a string
     *
     * @param n index of parameter (0 is first)
     * @param tDef default value to return if nth value is not available
     */
    std::string getString(unsigned int n, const std::string &tDef) const;

    template <typename T>T get(unsigned int n = 0) const
    {
        std::string strValue = getString(n);
        bool isOk = false;
        return parse<T>(strValue, isOk);
    }

    /**
     * @brief Access the nth element in the list of values associated to this
     * tag as T
     *
     * @note Using isstringstream to do the conversions
     *
     * @param n position of the value to be retrieved
     * @param[out] isOk to know if the conversion to T was correct
     *
     * @return delegates to parse()
     */
    template <typename T>T get(unsigned int n, bool &isOk) const
    {
        std::string strValue = getString(n);
        isOk = false;
        return parse<T>(strValue, isOk);
    }

    /**
     * @brief Access the nth element in the list of values associated to this
     * tag as T
     *
     * @param n position of the value to be retrieved
     * @param tDef default value if conversion failed
     *
     * @return delegates to parse() and return tDef if conversion failed
     * @see get(unsigned, bool&) get(unsigned, const T&, const T&, const T&)
     */
    template <typename T>T get(unsigned int n, const T &tDef) const
    {
        std::string strValue = getString(n);
        T ret;
        bool isOk = false;
        ret = parse<T>(strValue, isOk);
        if (isOk)
        {
            return ret;
        }
        return tDef;
    }

    /**
     * @brief Access the nth element in the list of values associated to this
     * tag as T
     *
     * @param n position of the value to be retrieved
     * @param tMin minimum possible value
     * @param tMax maximum possible value
     * @param tDef default if conversion failed
     *
     * @return delegates to parse() and return tDef if conversion failed.
     * Returns std::min(tMax, std::max(r, tMin))
     */
    template <typename T>T get(unsigned int n, const T &tMin, const T &tMax,
        const T &tDef) const
    {
        std::string strValue = getString(n);
        T ret;
        bool isOk = false;
        ret = parse<T>(strValue, isOk);
        if (isOk)
        {
            return std::min(tMax, std::max(ret, tMin));
        }
        return tDef;
    }
};

/** @brief Base class to define parameter definitions */
#ifdef INFOTM_ISP
template <typename T>
#endif //INFOTM_ISP
struct ParamAbstract
{
    std::string name;
    explicit ParamAbstract(const std::string &n)
        : name(n)
    {}
#ifdef INFOTM_ISP

    /**
     * @brief returns copy of current object with name extended by index
     *
     * @note helpful in handling NAME_X tags, such as WB_TEMPERATURE_0
     */
    T indexed(const unsigned int index) const {
        T obj(*static_cast<const T*>(this));
        std::stringstream namei;
        namei << name << "_" << index;
        obj.name = namei.str();
        return obj;
    }

    T indexed2(const unsigned int index) const {
        T obj(*static_cast<const T*>(this));
        std::stringstream namei;
        if (10 > index) {
            namei << name << "_" << 0 << index;
        } else {
            namei << name << "_" << index;
        }
        obj.name = namei.str();
        return obj;
    }
#endif //INFOTM_ISP
};

/**
 * @brief Define a parameter without minimum and maximum values
 *
 * For string and bool
 *
 * @code
ISPC::ParamDefSingle<bool> booleanOption("THE_NAME", false);
@endcode
 */
template <typename T>
#ifdef INFOTM_ISP
struct ParamDefSingle: public ParamAbstract<ParamDefSingle<T> >
#else
struct ParamDefSingle: public ParamAbstract
#endif //INFOTM_ISP
{
    T def;

    ParamDefSingle(const std::string &n, const T&d)
#ifdef INFOTM_ISP
        : ParamAbstract<ParamDefSingle<T> >(n), def(d)
#else
        : ParamAbstract(n), def(d)
#endif //INFOTM_ISP
    {}
};

/**
 * @brief Define a parameter with a minimum and maximum value
 *
 * @code
ISPC::ParamDef<int> intOption("THE_NAME", 0, 256, 64);
@endcode
 */
template <typename T>
#ifdef INFOTM_ISP
struct ParamDef: public ParamAbstract<ParamDef<T> >
#else
struct ParamDef: public ParamAbstract
#endif //INFOTM_ISP
{
    T min;
    T max;
    T def;

    ParamDef(const std::string &n, const T &m, const T &M, const T &d)
#ifdef INFOTM_ISP
        : ParamAbstract<ParamDef<T> >(n), def(d)
#else
        : ParamAbstract(n), def(d)
#endif //INFOTM_ISP
    {
        min = m;  // not using inline init to avoid warnings with min macro
        max = M;
    }
};

/**
 * @brief Define a parameter that has multiple values
 *
 * @code
static const int defvalues[4] = { 0, 0, 0, 0 };
ISPC::ParamDefArray<int> arrayOption("THE_NAME", -128, 127, defvalues, 4);
@endcode
 */
#ifdef INFOTM_ISP
template <typename T>struct ParamDefArray: ParamAbstract<ParamDefArray<T> >
#else
template <typename T>struct ParamDefArray: ParamAbstract
#endif //INFOTM_ISP
{
    T min;
    T max;
    T *def;
    unsigned n;

    ParamDefArray(const std::string &name, const T &m, const T &M,
        const T *d, unsigned tn)
#ifdef INFOTM_ISP
        :ParamAbstract<ParamDefArray<T> >(name), def(0), n(tn)
#else
        :ParamAbstract(name), def(0), n(tn)
#endif //INFOTM_ISP
    {
        // not using inline init to avoid warnings with min macro
        this->min = m;
        this->max = M;
        if (n > 0)
        {
            def = new T[n];
            for (unsigned int i = 0 ; i < n ; i++ )
            {
                def[i] = d[i];
            }
        }
    }

    ParamDefArray(const ParamDefArray<T> &d)
#ifdef INFOTM_ISP
        :ParamAbstract<ParamDefArray<T> >(d.name), def(0), n(d.n)
#else
        :ParamAbstract(d.name), def(0), n(d.n)
#endif //INFOTM_ISP
    {
        // not using inline init to avoid warnings with min macro
        this->min = d.min;
        this->max = d.max;
        if (n > 0)
        {
            def = new T[n];
            for (unsigned int i = 0 ; i < n ; i++ )
            {
                def[i] = d.def[i];
            }
        }
    }

    ParamDefArray<T>& operator=(const ParamDefArray &d)
    {
#ifdef INFOTM_ISP
        ParamAbstract<ParamDefArray<T> >::operator=(d);
#else
        ParamAbstract::operator=(d);
#endif //INFOTM_ISP
        // not using inline init to avoid warnings with min macro
        this->min = d.min;
        this->max = d.max;
        n = d.n;
        if (def) delete [] def;
        if (n > 0)
        {
            def = new T[n];
            for (unsigned int i = 0 ; i < n ; i++ )
            {
                def[i] = d.def[i];
            }
        }
        return *this;
    }

    ~ParamDefArray()
    {
        if (def)
        {
            delete[] def;
        }
    }
};

/**
 * @brief Print information about Parameter as a formatted string containing
 * type and defaults
 */
std::string getParameterInfo(const ParamDefSingle<bool> &t);

/**
 * @brief Print information about Parameter as a formatted string containing
 * type and defaults
 */
std::string getParameterInfo(const ParamDefSingle<std::string> &t);

/**
 * @brief Print information about Parameter as a formatted string containing
 * type, defaults and ranges
 */
std::string getParameterInfo(const ParamDef<int> &t);
/** @copydoc getParameterInfo(const ParamDef<int> &t) */
std::string getParameterInfo(const ParamDef<unsigned> &t);
/** @copydoc getParameterInfo(const ParamDef<int> &t) */
std::string getParameterInfo(const ParamDef<float> &t);
/** @copydoc getParameterInfo(const ParamDef<int> &t) */
std::string getParameterInfo(const ParamDef<double> &t);

/** @copydoc getParameterInfo(const ParamDef<int> &t) */
std::string getParameterInfo(const ParamDefArray<int> &t);
/** @copydoc getParameterInfo(const ParamDef<int> &t) */
std::string getParameterInfo(const ParamDefArray<unsigned> &t);
/** @copydoc getParameterInfo(const ParamDef<int> &t) */
std::string getParameterInfo(const ParamDefArray<float> &t);
/** @copydoc getParameterInfo(const ParamDef<int> &t) */
std::string getParameterInfo(const ParamDefArray<double> &t);

} /* namespace ISPC */

#endif /* ISPC_PARAMETER_H_ */
