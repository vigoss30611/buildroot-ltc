/**
*******************************************************************************
 @file parameters_test.cpp

 @brief Test ISPC Parameter/ParameterList classes and global save()/load()

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
#include <ispc/ISPCDefs.h>
#include <ispc/Parameter.h>
#include <ispc/Camera.h>
#include <ispc/Module.h>
#include <ispc/Sensor.h>
#include <ispc/Pipeline.h>
#include <ispc/LightCorrection.h>
#include <ispc/ControlLSH.h>
#include <ispc/ControlLBC.h>
#include <ispc/ParameterFileParser.h>
#include <img_errors.h>
#include <gtest/gtest.h>
#include <limits>
#include <typeinfo>
#include <algorithm>

/**
 * @brief test type to string using ISPC::toString() conversion
 */
TEST(parameters, toString)
{
    EXPECT_EQ(std::string("0"), ISPC::toString((float)0));
    EXPECT_EQ(std::string("-1.5678"), ISPC::toString((float)-1.5678));
    EXPECT_EQ(std::string("1.5678"), ISPC::toString((float)1.5678));

    EXPECT_EQ(std::string("0"), ISPC::toString((int)0));
    EXPECT_EQ(std::string("-1234"), ISPC::toString((int)-1234));
    EXPECT_EQ(std::string("1234"), ISPC::toString((int)1234));

    EXPECT_EQ(std::string("0"), ISPC::toString((unsigned int)0));
    EXPECT_EQ(std::string("1234"), ISPC::toString((unsigned int)1234));

    EXPECT_EQ(std::string("0"), ISPC::toString((short int)0));
    EXPECT_EQ(std::string("-1234"), ISPC::toString((short int)-1234));
    EXPECT_EQ(std::string("1234"), ISPC::toString((short int)1234));

    EXPECT_EQ(std::string("0"), ISPC::toString((char)0));
    EXPECT_EQ(std::string("-123"), ISPC::toString((char)-123));
    EXPECT_EQ(std::string("123"), ISPC::toString((char)123));

    EXPECT_EQ(std::string("0"), ISPC::toString((IMG_BOOL8)0));
    EXPECT_EQ(std::string("1"), ISPC::toString((IMG_BOOL8)1));
    EXPECT_EQ(std::string("1"), ISPC::toString((IMG_BOOL8)123));
    EXPECT_EQ(std::string("1"), ISPC::toString((IMG_BOOL8)-1));

    EXPECT_EQ(std::string("0"), ISPC::toString(false));
    EXPECT_EQ(std::string("1"), ISPC::toString(true));

    EXPECT_EQ(std::string(""), ISPC::toString(""));
    EXPECT_EQ(std::string("abc"), ISPC::toString("abc"));
}

/**
 * @brief test string to type using ISPC::parse() conversion
 */
TEST(parameters, parse)
{
    bool isOk;
    EXPECT_EQ(0, ISPC::parse<int>(std::string("0"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(-1234, ISPC::parse<int>(std::string("-1234"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(1234, ISPC::parse<int>(std::string("1234"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(2147483647, ISPC::parse<int>(std::string("2147483647"), isOk));
    EXPECT_TRUE(isOk);
    ISPC::parse<int>(std::string("2147483648"), isOk);
    EXPECT_FALSE(isOk);
    ISPC::parse<int>(std::string(""), isOk);
    EXPECT_FALSE(isOk);
    ISPC::parse<int>(std::string("abc"), isOk);
    EXPECT_FALSE(isOk);
    ISPC::parse<int>(std::string("4294967296"), isOk);
    EXPECT_FALSE(isOk);
    ISPC::parse<int>(std::string("1234.5678"), isOk);
    EXPECT_FALSE(isOk);

    EXPECT_EQ(0, ISPC::parse<unsigned int>(std::string("0"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(1234, ISPC::parse<unsigned int>(std::string("1234"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(4294967295, ISPC::parse<unsigned int>(std::string("4294967295"), isOk));
    EXPECT_TRUE(isOk);
    ISPC::parse<unsigned int>(std::string("4294967296"), isOk);
    EXPECT_FALSE(isOk);
    EXPECT_EQ(4294967295, ISPC::parse<unsigned int>(std::string("-1"), isOk));
    EXPECT_TRUE(isOk); /* FIXME incorrect behavior ? */
    ISPC::parse<unsigned int>(std::string(""), isOk);
    EXPECT_FALSE(isOk);
    ISPC::parse<unsigned int>(std::string("abc"), isOk);
    EXPECT_FALSE(isOk);

    EXPECT_EQ(0, ISPC::parse<short int>(std::string("0"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(-1234, ISPC::parse<short int>(std::string("-1234"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(1234, ISPC::parse<short int>(std::string("1234"), isOk));
    EXPECT_TRUE(isOk);

    EXPECT_EQ(0, ISPC::parse<char>(std::string("0"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(-123, ISPC::parse<char>(std::string("-123"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(123, ISPC::parse<char>(std::string("123"), isOk));
    EXPECT_TRUE(isOk);

    EXPECT_EQ(0, ISPC::parse<IMG_BOOL8>(std::string("0"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(1, ISPC::parse<IMG_BOOL8>(std::string("1"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(1, ISPC::parse<IMG_BOOL8>(std::string("123"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(1, ISPC::parse<IMG_BOOL8>(std::string("-1"), isOk));
    EXPECT_TRUE(isOk);

    EXPECT_FALSE(ISPC::parse<bool>("0", isOk));
    EXPECT_TRUE(ISPC::parse<bool>("1", isOk));
    EXPECT_TRUE(ISPC::parse<bool>("123", isOk));
    EXPECT_TRUE(ISPC::parse<bool>("-1", isOk));

    ISPC::parse<std::string>(std::string(""), isOk);
    EXPECT_FALSE(isOk);
    EXPECT_EQ("abc", ISPC::parse<std::string>(std::string("abc"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ("123", ISPC::parse<std::string>(std::string("123"), isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ("-123", ISPC::parse<std::string>(std::string("-123"), isOk));
    EXPECT_TRUE(isOk);
}

/**
 * @brief test Parameter class
 */
TEST(parameters, Parameter)
{
    ISPC::Parameter param1("TAG1");
    EXPECT_EQ("TAG1", param1.getTag());
    EXPECT_EQ(IMG_SUCCESS, param1.addValue("1"));
    EXPECT_EQ(IMG_SUCCESS, param1.addValue("2"));
    EXPECT_EQ(IMG_SUCCESS, param1.addValue("-3"));
    EXPECT_EQ(IMG_SUCCESS, param1.addValue("abc"));
    EXPECT_EQ(4, param1.size());
    EXPECT_EQ("1", param1.getString());
    EXPECT_EQ("2", param1.getString(1));
    EXPECT_EQ("-3", param1.getString(2));
    EXPECT_EQ("abc", param1.getString(3));
    EXPECT_EQ("", param1.getString(4));
    EXPECT_EQ("default", param1.getString(4, "default"));

    const std::vector<std::string>& values = param1.getValues();
    EXPECT_EQ(4, values.size());

    bool isOk;
    EXPECT_EQ(1, param1.get<int>(0, isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(true, param1.get<bool>(0, isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(2, param1.get<char>(1, isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(-3, param1.get<int>(2, isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ("abc", param1.get<std::string>(3, isOk));
    EXPECT_TRUE(isOk);
    EXPECT_EQ(0, param1.get<int>(3, isOk));
    EXPECT_FALSE(isOk);
    EXPECT_EQ("", param1.get<std::string>(4, isOk));
    EXPECT_FALSE(isOk);
    EXPECT_EQ(0, param1.get<int>(4, isOk));
    EXPECT_FALSE(isOk);

    // check default
    EXPECT_EQ(1234, param1.get<int>(3, 1234));
    EXPECT_EQ("default", param1.get<std::string>(4, "default"));
    EXPECT_EQ(1234, param1.get<int>(4, 1234));

    // check min max def
    EXPECT_EQ(3, param1.get<int>(2, 3, 4, 5));
    EXPECT_EQ(-5, param1.get<int>(2, -8, -5, -6));
    EXPECT_EQ(3, param1.get<int>(4, 1, 2, 3));
}

/**
 * @brief test ParamAbstract template class
 */
TEST(parameters, ParamAbstract)
{
    ISPC::ParamAbstract<ISPC::ParamDefSingle<int> > param1("TAG");
    EXPECT_EQ("TAG", param1.name);
    // check that indexed(unsigned int) returns an object of template type
    EXPECT_EQ(typeid(ISPC::ParamDefSingle<int>).hash_code(),
              typeid(param1.indexed(0)).hash_code());
    EXPECT_EQ("TAG_0", param1.indexed(0).name);
    EXPECT_EQ("TAG_1", param1.indexed(1).name);
}

/**
 * @brief test ParamDef template class
 */
TEST(parameters, ParamDef)
{
    ISPC::ParamDef<int> param1("TAG", 1, 3, 2);
    EXPECT_EQ("TAG", param1.name);
    // check that indexed(unsigned int) returns an object of template type
    EXPECT_EQ(typeid(param1).hash_code(),
              typeid(param1.indexed(0)).hash_code());
    EXPECT_EQ("TAG_0", param1.indexed(0).name);
    EXPECT_EQ(1, param1.min);
    EXPECT_EQ(3, param1.max);
    EXPECT_EQ(2, param1.def);
    EXPECT_EQ(param1.min, param1.indexed(0).min);
    EXPECT_EQ(param1.max, param1.indexed(0).max);
    EXPECT_EQ(param1.def, param1.indexed(0).def);
}

/**
 * @brief test ParamDefArray template class
 */
TEST(parameters, ParamDefArray)
{
    const int def[] = { 1, 2, 3, 4, 5};
    const int defsize = sizeof(def)/sizeof(def[0]);
    ISPC::ParamDefArray<int> param1("TAG", 1, 5, def, defsize);

    EXPECT_EQ("TAG", param1.name);
    // check that indexed(unsigned int) returns an object of template type
    EXPECT_EQ(typeid(param1).hash_code(),
              typeid(param1.indexed(0)).hash_code());
    EXPECT_EQ("TAG_0", param1.indexed(0).name);
    EXPECT_EQ(1, param1.min);
    EXPECT_EQ(5, param1.max);
    EXPECT_EQ(defsize, param1.n);
    EXPECT_EQ(param1.min, param1.indexed(0).min);
    EXPECT_EQ(param1.max, param1.indexed(0).max);
    EXPECT_EQ(param1.n, param1.indexed(0).n);

    // check that the def pointer has not been copied
    EXPECT_NE(def, param1.def);
    EXPECT_NE(param1.indexed(0).def, param1.def);

    for(int i=0;i<defsize; ++i)
    {
        EXPECT_EQ(def[i], param1.def[i]);
        EXPECT_EQ(param1.indexed(0).def[i], param1.def[i]);
    }
}

namespace
{

/**
 * @brief functor base class, used in ModuleParameters::checkTags()
 */
struct base_functor
{
    virtual ~base_functor(){}
    virtual const std::string name() const=0;

    virtual bool operator()(int& val, int& min, int& max)=0;
    virtual bool operator()(unsigned int& val, unsigned int& min, unsigned int& max)=0;
    virtual bool operator()(double& val, double& min, double& max)=0;
};

/**
 * @brief child class for checking whether val lies between <min and max>
 */
struct in_range : public base_functor
{
    virtual ~in_range(){}

    virtual const std::string name() const { return "in_range"; }

    template <typename T>
    bool check(T& val, T& min, T& max)
    {
        EXPECT_GE(val, min);
        EXPECT_LE(val, max);
        return (val >= min) && (val <= max);
    }

    virtual bool operator()(int& val, int& min, int& max)
    {
        return check(val, min, max);
    }
    virtual bool operator()(unsigned int& val, unsigned int& min, unsigned int& max)
    {
        return check(val, min, max);
    }
    virtual bool operator()(double& val, double& min, double& max)
    {
        return check(val, min, max);
    }
};

/**
 * @brief child class for checking whether val has the value of first
 */
struct equals_first : public base_functor
{
    virtual ~equals_first(){}
    virtual const std::string name() const { return "equals_first"; }

    template <typename T>
    bool check(T& val, T& first, T& second)
    {
        EXPECT_EQ(first, val);
        return first == val;
    }

    virtual bool operator()(int& val, int& first, int& second)
    {
        return check(val, first, second);
    }
    virtual bool operator()(unsigned int& val, unsigned int& first, unsigned int& second)
    {
        return check(val, first, second);
    }
    virtual bool operator()(double& val, double& first, double& second)
    {
        return check(val, first, second);
    }
};

};

/**
 * @brief Pipeline test harness
 */
class ModuleParameters : public ISPC::Pipeline, public testing::Test {
public:
    ModuleParameters() :
        ISPC::Pipeline(NULL, 0, NULL)
    {
        //setSensor(new ISPC::DGSensor("", 0, true));
        setSensor(new ISPC::Sensor(0));
    }
    ISPC::Control ctrl;

    /**
     * @brief type required for comparing sets of parameters
     */
    typedef std::pair<std::string, ISPC::Parameter> ParamEntry;

    /**
     * @brief `ParamEntry type compare function
     *
     * @note used by set_symmetric_difference()
     */
    static bool cmp(const ParamEntry &a, const ParamEntry &b) {
        return a.first < b.first;
    }

    /**
     * @brief helper array of hwVersions to be tested
     */
    static const struct vers_t {
        int major;
        int minor;
    } hwVersions[];

    /**
     * @brief call checkFunc() on every element of params
     *
     * @note used to validate parameters
     */
    void checkTags(
            ISPC::ParameterList& params,
            ISPC::ParameterList& first,
            ISPC::ParameterList& second,
            base_functor& checkFunc,
            bool skipEmptyValues = false);

    /**
     * @brief set some parameters to values simulating real hw allowing
     * validation
     */
    void fixCurrentPipelineConfig();
};


void ModuleParameters::checkTags(
        ISPC::ParameterList& params,
        ISPC::ParameterList& first,
        ISPC::ParameterList& second,
        base_functor& checkFunc,
        bool skipEmptyValues)
{
    for(auto it = params.begin();
         it!=params.end();
         ++it)
    {
        auto& values = it->second.getValues();
        unsigned int count = values.size();
        if(!first.exists(it->first) || !second.exists(it->first))
        {
            // can't compare default values
            continue;
        }
        ISPC::Parameter* itmin = first.getParameter(it->first);
        ISPC::Parameter* itmax = second.getParameter(it->first);
        for(unsigned int i=0;i < count;++i)
        {
            /*printf("%s: checking param=\"%s\" first=\"%s\" second=\"%s\"\n",
                        it->first.c_str(),
                        it->second.get<std::string>(i).c_str(),
                        itmin->get<std::string>(i).c_str(),
                        itmax->get<std::string>(i).c_str());*/
            {
                bool isOk = true;
                bool valOk;
                int curval;
                // current runtime values may not exist because of specific
                // configuration and this is completely legitimate
                if(skipEmptyValues && it->second.get<std::string>(i).empty())
                {
                    continue;
                } else {
                    curval = it->second.get<int>(i, valOk);
                    isOk = isOk && valOk;
                }
                int minval = itmin->get<int>(i, valOk);
                isOk = isOk && valOk;
                int maxval = itmax->get<int>(i, valOk);
                isOk = isOk && valOk;
                if(isOk)
                {
                    if(!checkFunc(curval, minval, maxval))
                    {
                        printf("tag %s value %s failed check %s(first=%s second=%s)\n",
                                it->first.c_str(),
                                it->second.get<std::string>(i).c_str(),
                                checkFunc.name().c_str(),
                                itmin->get<std::string>(i).c_str(),
                                itmax->get<std::string>(i).c_str());
                    }
                    // all types ok, continue to next value
                    continue;
                }
                // types not ok, try another type
            }
            {
                bool isOk = true;
                bool valOk;
                unsigned int curval;
                curval = it->second.get<unsigned int>(i, valOk);
                isOk = isOk && valOk;
                unsigned int minval = itmin->get<unsigned int>(i, valOk);
                isOk = isOk && valOk;
                unsigned int maxval = itmax->get<unsigned int>(i, valOk);
                isOk = isOk && valOk;
                if(isOk)
                {
                    if(!checkFunc(curval, minval, maxval))
                    {
                        printf("tag %s value %s failed check %s(first=%s second=%s)\n",
                                it->first.c_str(),
                                it->second.get<std::string>(i).c_str(),
                                checkFunc.name().c_str(),
                                itmin->get<std::string>(i).c_str(),
                                itmax->get<std::string>(i).c_str());
                    }
                    // all types ok, continue to next value
                    continue;
                }
                // types not ok, try another type
            }
            {
                bool isOk = true;
                bool valOk;
                double curval;
                curval = it->second.get<double>(i, valOk);
                isOk = isOk && valOk;
                double minval = itmin->get<double>(i, valOk);
                isOk = isOk && valOk;
                double maxval = itmax->get<double>(i, valOk);
                isOk = isOk && valOk;
                if(isOk)
                {
                    if(!checkFunc(curval, minval, maxval))
                    {
                        printf("tag %s value %s failed check %s(first=%s second=%s)\n",
                                it->first.c_str(),
                                it->second.get<std::string>(i).c_str(),
                                checkFunc.name().c_str(),
                                itmin->get<std::string>(i).c_str(),
                                itmax->get<std::string>(i).c_str());
                    }
                    // all types ok, continue to next value
                    continue;
                }
                // types not ok, possibly a string?
            }
        } // values
    } // tags
}

void ModuleParameters::fixCurrentPipelineConfig()
{
    ISPC::ParameterList cur;
    this->saveAll(cur, ISPC::ModuleBase::SAVE_VAL);
    ctrl.saveAll(cur, ISPC::ModuleBase::SAVE_VAL);

    // these modules save min max and default as if there were one
    // configuration
    // so we have to add artificial config here to be able to compare
    // the sets later
    if(ctrl.getControlModule<ISPC::ControlLBC>())
    {
        cur.addParameter(ISPC::Parameter("LBC_CONFIGURATIONS", "1"), true);
    }
    if(ctrl.getControlModule<ISPC::ControlLSH>())
    {
        cur.addParameter(ISPC::Parameter("LSH_CTRL_CORRECTIONS", "1"), true);
        cur.addParameter(ISPC::Parameter("LSH_CTRL_BITS_DIFF",
                ISPC::toString(LSH_DELTA_BITS_MIN)), true);
        cur.addParameter(ISPC::Parameter("LSH_CTRL_TEMPERATURE_0", "6500"), true);
        cur.addParameter(ISPC::Parameter("LSH_CTRL_FILE_0", "abcd"), true);
        cur.addParameter(ISPC::Parameter("LSH_CTRL_SCALE_WB_0",
                ISPC::toString(ISPC::ControlLSH::LSH_CTRL_SCALE_WB_S.def)), true);
    }
    cur.addParameter(ISPC::Parameter("WB_CORRECTIONS", "1"), true);
    // load modified config
    ctxStatus = ISPC::ISPC_Ctx_INIT;
    this->reloadAll(cur);
    ctrl.loadAll(cur);
}

// the set of currently supported hardware versions
const struct ModuleParameters::vers_t
ModuleParameters::hwVersions[] =
        {{ 1, 2 }, { 2, 1 }, { 2, 3 }, { 2, 4 }, { 2, 6 }, { 2, 7 }};

/**
 * @brief validate consistency between defaults, min, max and current values
 *
 * @note checks if these sets contain the same parameters
 */
TEST_F(ModuleParameters, save_tags_consistency)
{
    // check all hw versions
    for(auto hw : hwVersions)
    {
        clearModules();
        ctrl.clearModules();

        // setup all modules for specific hw version
        auto modules =
            ISPC::CameraFactory::setupModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: modules)
        {
            registerModule(module);
        }
        auto controls =
            ISPC::CameraFactory::controlModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: controls)
        {
            ctrl.registerControlModule(module, this);
        }

        ISPC::ParameterList defaults, min, max, cur;
        this->saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);
        this->saveAll(min, ISPC::ModuleBase::SAVE_MIN);
        this->saveAll(max, ISPC::ModuleBase::SAVE_MAX);
        ctrl.saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);
        ctrl.saveAll(min, ISPC::ModuleBase::SAVE_MIN);
        ctrl.saveAll(max, ISPC::ModuleBase::SAVE_MAX);

        fixCurrentPipelineConfig();

        this->saveAll(cur, ISPC::ModuleBase::SAVE_VAL);
        ctrl.saveAll(cur, ISPC::ModuleBase::SAVE_VAL);

        // every case has to have same number of entries
        const size_t def_tags = std::distance(defaults.begin(), defaults.end());
        const size_t min_tags = std::distance(min.begin(), min.end());
        const size_t max_tags = std::distance(max.begin(), max.end());
        const size_t cur_tags = std::distance(cur.begin(), cur.end());
        EXPECT_EQ(def_tags, min_tags);
        EXPECT_EQ(def_tags, max_tags);
        EXPECT_EQ(def_tags, cur_tags);

        // verify that the sets contain exact tags
        std::map<std::string, ISPC::Parameter> diff;

        std::set_symmetric_difference(
                defaults.begin(), defaults.end(),
                min.begin(), min.end(),
                std::inserter(diff, diff.begin()), cmp);
        EXPECT_EQ(0, diff.size());
        if(diff.size())
        {
            printf("Diff(def-min) for HW%d.%d\n", hw.major, hw.minor);
            for (auto& i : diff)
            {
                printf("tag %s\n", i.first.c_str());
            }
        }
        diff.clear();

        std::set_symmetric_difference(
                defaults.begin(), defaults.end(),
                max.begin(), max.end(),
                std::inserter(diff, diff.begin()), cmp);
        EXPECT_EQ(0, diff.size());
        if(diff.size())
        {
            printf("Diff(def-max) for HW%d.%d\n", hw.major, hw.minor);
            for (auto& i : diff)
            {
                printf("tag %s\n", i.first.c_str());
            }
        }
        diff.clear();

        std::set_symmetric_difference(
                defaults.begin(), defaults.end(),
                cur.begin(), cur.end(),
                std::inserter(diff, diff.begin()), cmp);
        EXPECT_EQ(0, diff.size());
        if(diff.size())
        {
            printf("Diff(def-val) for HW%d.%d\n", hw.major, hw.minor);
            for (auto& i : diff)
            {
                printf("tag %s\n", i.first.c_str());
            }
        }
        diff.clear();

        // check the set_difference algorithm gives valid results
        ISPC::ParameterList check(min);
        check.addParameter(ISPC::Parameter("CHECK_TAG", "value"));
        std::set_symmetric_difference(
                check.begin(), check.end(),
                min.begin(), min.end(),
                std::inserter(diff, diff.begin()), cmp);
        EXPECT_EQ(1, diff.size());
        diff.clear();
    }
}

/**
 * @brief validate if the parameter values are valid (lie between <min, max>)
 */
TEST_F(ModuleParameters, save_tags_min_max)
{
    // check all hw versions
    for(auto hw : hwVersions)
    {
        clearModules();
        ctrl.clearModules();

        // setup all modules for specific hw version
        auto modules =
            ISPC::CameraFactory::setupModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: modules)
        {
            registerModule(module);
        }
        auto controls =
            ISPC::CameraFactory::controlModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: controls)
        {
            ctrl.registerControlModule(module, this);
        }

        ISPC::ParameterList defaults, min, max, cur;
        this->saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);
        this->saveAll(min, ISPC::ModuleBase::SAVE_MIN);
        this->saveAll(max, ISPC::ModuleBase::SAVE_MAX);
        ctrl.saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);
        ctrl.saveAll(min, ISPC::ModuleBase::SAVE_MIN);
        ctrl.saveAll(max, ISPC::ModuleBase::SAVE_MAX);

        fixCurrentPipelineConfig();
        this->saveAll(cur, ISPC::ModuleBase::SAVE_VAL);
        ctrl.saveAll(cur, ISPC::ModuleBase::SAVE_VAL);

        // verify that default and current values lie between min and max
        in_range func;
        checkTags(defaults, min, max, func);
        checkTags(cur, min, max, func, true);
    }
}

/**
 * @brief validate if every default parameter contains description string filled
 */
TEST_F(ModuleParameters, save_tags_defaults_info)
{
    // check all hw versions
    for(auto hw : hwVersions)
    {
        clearModules();
        ctrl.clearModules();

        // setup all modules for specific hw version
        auto modules =
            ISPC::CameraFactory::setupModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: modules)
        {
            registerModule(module);
        }
        auto controls =
            ISPC::CameraFactory::controlModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: controls)
        {
            ctrl.registerControlModule(module, this);
        }

        ISPC::ParameterList defaults;
        this->saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);
        ctrl.saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);

        // verify that every default::getInfo() returns non-empty string
        for(auto it = defaults.begin();
             it!=defaults.end();
             ++it)
        {
            if(it->first.find_first_of("//") <=2)
            {
                // do not check deprecated parameters
                continue;
            }
            EXPECT_FALSE(it->second.getInfo().empty());
        }
    }
}

#if(0)
// currently disabled as there are too many tags which need special handling
TEST_F(ModuleParameters, load_save_tags_load_save)
{
    // check all hw versions
    for(auto hw : hwVersions)
    {
        clearModules();
        ctrl.clearModules();

        // setup all modules for specific hw version
        auto modules =
            ISPC::CameraFactory::setupModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: modules)
        {
            registerModule(module);
        }
        auto controls =
            ISPC::CameraFactory::controlModulesFromHWVersion(hw.major, hw.minor);
        for(auto& module: controls)
        {
            ctrl.registerControlModule(module, this);
        }

        ISPC::ParameterList params, defaults, min, max, cur;
        //this->saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);
        this->saveAll(min, ISPC::ModuleBase::SAVE_MIN);
        this->saveAll(max, ISPC::ModuleBase::SAVE_MAX);

        //ctrl.saveAll(defaults, ISPC::ModuleBase::SAVE_DEF);
        ctrl.saveAll(min, ISPC::ModuleBase::SAVE_MIN);
        ctrl.saveAll(max, ISPC::ModuleBase::SAVE_MAX);

        //fixCurrentPipelineConfig();

        std::srand(0x12563478);
        // return random double between min and max
        auto getRandom = [](double min, double max)
        {
            auto ret =
                int64_t(round(floor(min) + (max-min)*double(std::rand())/RAND_MAX));
            ret = ISPC::clip(ret, min, max);
            return ret;
        };

        // set most tags randomly and check if saved values are the same as loaded
        auto itmin = min.begin();
        auto itmax = max.begin();
        for(;itmin!=min.end();
                ++itmin,++itmax)
        {
            // make sure min and max sets are consistent
            ASSERT_EQ(itmin->first, itmax->first);
            // all this code below is needed to decrement the value by one (1)!
            auto& valuesmin = itmin->second.getValues();
            auto& valuesmax = itmax->second.getValues();
            int countmin = valuesmin.size();
            int countmax = valuesmax.size();
            // make sure the same number of values in tag
            ASSERT_EQ(countmin, countmax);

            //double should fit all types except string
            std::vector<std::string> newvalues;
            for(unsigned int i=0;i < countmin;++i)
            {
                bool isOk;
                // parse current values
                double valmin = ISPC::parse<double>(valuesmin[i], isOk);
                if(!isOk)
                {
                    continue;
                }
                double valmax = ISPC::parse<double>(valuesmax[i], isOk);
                if(!isOk)
                {
                    continue;
                }
                int64_t newval = getRandom(valmin, valmax);
                // add new values which are above max min
                newvalues.push_back(ISPC::toString(newval));
            }
            params.addParameter(ISPC::Parameter(itmin->first, newvalues), true);
        }
        // set some parameters to special values to make test much simpler
        fixCurrentPipelineConfig();
        // load parameters, should clip to min
        ctxStatus = ISPC::ISPC_Ctx_INIT;
        this->reloadAll(params);
        ctrl.loadAll(params);
        //fixCurrentPipelineConfig();
        // retrieve clipped values
        this->saveAll(cur, ISPC::ModuleBase::SAVE_VAL);
        ctrl.saveAll(cur, ISPC::ModuleBase::SAVE_VAL);

        equals_first func;
        checkTags(cur, params, min, func, true);
    }
}
#endif
