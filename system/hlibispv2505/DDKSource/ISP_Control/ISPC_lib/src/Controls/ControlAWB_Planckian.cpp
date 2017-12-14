/**
*******************************************************************************
@file ControlAWB_Planckian.cpp

@brief ISPC::ControlAWB_Planckian_Planckian implementation

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
#include "ispc/ControlAWB_Planckian.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_AWB"

#include <list>
#include <cmath>

#include "ispc/ModuleAWS.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ColorCorrection.h"
#include "ispc/Sensor.h"
#include "ispc/Pipeline.h"
#include "ispc/ISPCDefs.h"

#include "ispc/PerfTime.h"
#include "ispc/Save.h"

#include <ispc/Camera.h>
#include "ispc/ControlAE.h"
#include "ispc/ControlCMC.h"


#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

/**
 * If true then uses numCFA field as tile weight to better handle
 * strength of 'white' in centroid calculation
 *
 */
#define USE_WEIGHTED_CENTROID 1

/**
 * @brief Max temperature in Kelvin for the AWB algorithm
 * - it is an arbitrary choice
 */
#define AWB_MAX_TEMP 10000.0f

const int ISPC::ControlAWB_Planckian::DEFAULT_WEIGHT_BASE = 2;
const int ISPC::ControlAWB_Planckian::DEFAULT_TEMPORAL_STRETCH = 300;
const int ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MIN = 200;
const int ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MAX = 5000;

const ISPC::ParamDefSingle<bool> ISPC::ControlAWB_Planckian::AWB_USE_AWS_CONFIG(
    "AWB_USE_AWS_CONFIG", false);

const ISPC::ParamDef<double> ISPC::ControlAWB_Planckian::AWB_MAX_RATIO_DISTANCE(
    "AWB_MAX_RATIO_DISTANCE", 0.0, 1.0, 0.2);

const ISPC::ParamDefSingle<bool> ISPC::ControlAWB_Planckian::WBT_USE_FLASH_FILTERING(
    "WBT_USE_FLASH_FILTERING", false);

const ISPC::ParamDefSingle<bool> ISPC::ControlAWB_Planckian::WBT_USE_SMOOTHING(
    "WBT_USE_SMOOTHING", false);

const ISPC::ParamDef<int> ISPC::ControlAWB_Planckian::WBTS_TEMPORAL_STRETCH(
    "WBT_TEMPORAL_STRETCH",
    ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MIN,
    ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MAX,
    ISPC::ControlAWB_Planckian::DEFAULT_TEMPORAL_STRETCH);

const ISPC::ParamDef<float> ISPC::ControlAWB_Planckian::WBTS_WEIGHT_BASE(
    "WBT_WEIGHT_BASE",
    ISPC::ControlAWB_Planckian::getMinWeightBase(),
    ISPC::ControlAWB_Planckian::getMaxWeightBase(),
    ISPC::ControlAWB_Planckian::DEFAULT_WEIGHT_BASE);

const double ISPC::ControlAWB_Planckian::AwbTile::RATIO_DEF = 1.0;

bool ISPC::ControlAWB_Planckian::AwbTile::applyBounds(
    const double lowLog2RatioR,
    const double lowLog2RatioB)
{
    if(getLog2RatioR() > lowLog2RatioR &&
       getLog2RatioB() < lowLog2RatioB)
    {
#if VERBOSE_CONTROL_MODULES
        LOG_INFO(
            "Invalidated tile ratioR=%f ratioB=%f\n",
            getRatioR(), getRatioB());
#endif
        numCFAs = 0; // invalidate tile if outside low_end bound
        return false;
    }
    return true;
}

ISPC::ParameterGroup ISPC::ControlAWB_Planckian::getGroup()
{
    ParameterGroup group;

    group.header = "// Auto White Balance parameters (Planckian)";

    group.parameters.insert(ControlAWB::AWB_TARGET_TEMPERATURE.name);
    group.parameters.insert(AWB_MAX_RATIO_DISTANCE.name);
    group.parameters.insert(AWB_USE_AWS_CONFIG.name);

    group.parameters.insert(WBT_USE_FLASH_FILTERING.name);
    group.parameters.insert(WBT_USE_SMOOTHING.name);
    group.parameters.insert(WBTS_TEMPORAL_STRETCH.name);
    group.parameters.insert(WBTS_WEIGHT_BASE.name);

    return group;
}

ISPC::ControlAWB_Planckian::ControlAWB_Planckian(const std::string &logName)
    : ControlAWB(logName),
    mFlashFilteringEnabled(true),
    mUseTemporalSmoothing(false),
    mWeightsBase(float(DEFAULT_WEIGHT_BASE)),
    mTemporalStretch(DEFAULT_TEMPORAL_STRETCH),
    fRatioR(1.0),
    fRatioB(1.0),
    fLog2_R_Qeff(0.0),
    fLog2_B_Qeff(0.0),
    f_R_Qeff(1.0),
    f_B_Qeff(1.0),
    fLog2LowTempRatioR(1.0),
    fLog2LowTempRatioB(1.0),
    fMaxRatioDistance(AWB_MAX_RATIO_DISTANCE.def),
    fMaxRatioDistancePow2(fMaxRatioDistance*fMaxRatioDistance),
    useCustomAwsConfig(AWB_USE_AWS_CONFIG.def)
{
    MOD_LOG_DEBUG("mUseTemporalSmoothing = %d mWeightsBase = %f mTemporalStretch = %d \n", mUseTemporalSmoothing, mWeightsBase, mTemporalStretch);
}

IMG_RESULT ISPC::ControlAWB_Planckian::load(const ParameterList &parameters)
{
    /* Pass the parameter list to the ColorCorrection auxiliary object so it
     *can load parameters from the parameters as well. */
    colorTempCorrection.loadParameters(parameters);

    if (colorTempCorrection.getCorrectionIndex(6500.0) < 0)
    {
        LOG_WARNING("loaded temperature correction does not have 6500K" \
            " entry!\n");
        // return IMG_ERROR_NOT_SUPPORTED;
    }

    targetTemperature =
            parameters.getParameter(ControlAWB::AWB_TARGET_TEMPERATURE);
    useCustomAwsConfig = parameters.getParameter(AWB_USE_AWS_CONFIG);
    fMaxRatioDistance = parameters.getParameter(AWB_MAX_RATIO_DISTANCE);
    fMaxRatioDistancePow2 = fMaxRatioDistance*fMaxRatioDistance;

    mFlashFilteringEnabled =
            parameters.getParameter(WBT_USE_FLASH_FILTERING);
    mUseTemporalSmoothing =
            parameters.getParameter(WBT_USE_SMOOTHING);
    mTemporalStretch =
            parameters.getParameter(WBTS_TEMPORAL_STRETCH);
    mWeightsBase =
            parameters.getParameter(WBTS_WEIGHT_BASE);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAWB_Planckian::save(ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    static ParameterGroup group;

    if (group.parameters.size() == 0)
    {
        group = ControlAWB_Planckian::getGroup();
    }

    parameters.addGroup("ControlAWB_Planckian", group);

    switch (t)
    {
    case ModuleBase::SAVE_VAL:
        parameters.addParameter(ControlAWB::AWB_TARGET_TEMPERATURE,
                targetTemperature);
        parameters.addParameter(AWB_USE_AWS_CONFIG, useCustomAwsConfig);
        parameters.addParameter(AWB_MAX_RATIO_DISTANCE, fMaxRatioDistance);
        parameters.addParameter(WBT_USE_FLASH_FILTERING, mFlashFilteringEnabled);
        parameters.addParameter(WBT_USE_SMOOTHING, mUseTemporalSmoothing);
        parameters.addParameter(WBTS_TEMPORAL_STRETCH, mTemporalStretch);
        parameters.addParameter(WBTS_WEIGHT_BASE, mWeightsBase);
        break;

    case ModuleBase::SAVE_MIN:
        parameters.addParameterMin(ControlAWB::AWB_TARGET_TEMPERATURE);
        parameters.addParameterMin(AWB_USE_AWS_CONFIG);
        parameters.addParameterMin(AWB_MAX_RATIO_DISTANCE);
        parameters.addParameterMin(WBT_USE_FLASH_FILTERING);
        parameters.addParameterMin(WBT_USE_SMOOTHING);
        parameters.addParameterMin(WBTS_TEMPORAL_STRETCH);
        parameters.addParameterMin(WBTS_WEIGHT_BASE);
        break;

    case ModuleBase::SAVE_MAX:
        parameters.addParameterMax(ControlAWB::AWB_TARGET_TEMPERATURE);
        parameters.addParameterMax(AWB_USE_AWS_CONFIG);
        parameters.addParameterMax(AWB_MAX_RATIO_DISTANCE);
        parameters.addParameterMax(WBT_USE_FLASH_FILTERING);
        parameters.addParameterMax(WBT_USE_SMOOTHING);
        parameters.addParameterMax(WBTS_TEMPORAL_STRETCH);
        parameters.addParameterMax(WBTS_WEIGHT_BASE);
        break;

    case ModuleBase::SAVE_DEF:
        parameters.addParameterDef(ControlAWB::AWB_TARGET_TEMPERATURE);
        parameters.addParameterDef(AWB_USE_AWS_CONFIG);
        parameters.addParameterDef(AWB_MAX_RATIO_DISTANCE);
        parameters.addParameterDef(WBT_USE_FLASH_FILTERING);
        parameters.addParameterDef(WBT_USE_SMOOTHING);
        parameters.addParameterDef(WBTS_TEMPORAL_STRETCH);
        parameters.addParameterDef(WBTS_WEIGHT_BASE);

        break;
    }

    return colorTempCorrection.saveParameters(parameters, t);
}

int ISPC::ControlAWB_Planckian::getTemporalStretch()
{
    return mTemporalStretch;
}

void ISPC::ControlAWB_Planckian::setTemporalStretch(int tempStretch_ms)
{
    mTemporalStretch = tempStretch_ms;
}

// hr_TODO: getting fps from sensor is not always reliable, involve AE
double ISPC::ControlAWB_Planckian::getFps()
{
    Sensor *sensor = getSensor();
    SENSOR_HANDLE handle = sensor->getHandle();
    SENSOR_INFO sensorMode;
    Sensor_GetInfo(handle, &sensorMode);
    return sensorMode.sMode.flFrameRate;
}

bool ISPC::ControlAWB_Planckian::potentialFlash(const rbNode_t rbNode)
{
    /* ft - flash trigger - change in ratio value indicating potential flash */
    const float ftLo = 0.95f;
    const float flHi = 1.05f;
    const float rgFtLo = ftLo;
    const float rgFtHi = flHi;
    const float bgFtLo = ftLo;
    const float bgFtHi = flHi;
    return (((rbNode.rg < rbNodes.front().rg*rgFtLo) && (rbNode.bg < rbNodes.front().bg*bgFtLo)) ||
            ((rbNode.rg > rbNodes.front().rg*rgFtHi) && (rbNode.bg > rbNodes.front().bg*bgFtHi)) ||
            ((rbNode.rg < rbNodes.front().rg*rgFtLo) && (rbNode.bg > rbNodes.front().bg*bgFtHi)) ||
            ((rbNode.rg > rbNodes.front().rg*rgFtHi) && (rbNode.bg < rbNodes.front().bg*bgFtLo)));
}

void ISPC::ControlAWB_Planckian::temporalAwbClear(void)
{
    rbNodesCache.clear();
    rbNodes.clear();
    return;
}

bool ISPC::ControlAWB_Planckian::flashFilteringDisabled(void)
{
    return !mFlashFilteringEnabled;
}

void ISPC::ControlAWB_Planckian::flashFiltering(const double in_RG, const double in_BG)
{
    const int maxFlashFrames = 1;
    const int FLASH_FILTERING_START_DELAY = 5;
    rbNode_t rbNode;
    rbNode.rg = in_RG;
    rbNode.bg = in_BG;

    if(flashFilteringDisabled()) {
        rbNodes.push_front(rbNode);
        MOD_LOG_DEBUG("FLASH FILTERING DISABLED\n");
        return;
    }
    MOD_LOG_DEBUG("FLASH FILTERING ENABLED\n");

    if (rbNodes.size()>FLASH_FILTERING_START_DELAY && potentialFlash(rbNode)) {
        MOD_LOG_DEBUG("potential FLASH frame nr %d (curr = %f %f, prev = %f %f)\n", rbNodesCache.size()+1, rbNode.rg, rbNode.bg, rbNodes.front().rg, rbNodes.front().bg);
        // potential flash detected
        if (rbNodesCache.size()<maxFlashFrames) {
            /* cache this entry */
            rbNodesCache.push_front(rbNode);
        } else {
            /* this is not a flash, this is changed scene
             * move cached entries to take them into calculations  */
            for(std::deque<rbNode_t>::reverse_iterator i = rbNodesCache.rbegin(); i != rbNodesCache.rend(); ++i) {
                rbNode.rg = i->rg;
                rbNode.bg = i->bg;
                rbNodes.push_front(rbNode);
            }
            rbNode.rg = in_RG;
            rbNode.bg = in_BG;
            rbNodes.push_front(rbNode);
            rbNodesCache.clear();
        }
    } else {
        while (!rbNodesCache.empty()) {
            MOD_LOG_DEBUG("flash detected, flash frames = %d \n", rbNodesCache.size());
            rbNode.rg = rbNodes.begin()->rg;
            rbNode.bg = rbNodes.begin()->bg;
            rbNodes.push_front(rbNode);
            rbNodesCache.pop_front();
        }
        rbNode.rg = in_RG;
        rbNode.bg = in_BG;
        rbNodes.push_front(rbNode);
    }
    return;

}

void ISPC::ControlAWB_Planckian::setWBTSAlgorithm(
        bool smoothingEnabled, float base, int temporalStretch)
{
    rbNodes.clear();
    mUseTemporalSmoothing = smoothingEnabled;
    mWeightsBase = base;
    mTemporalStretch = temporalStretch;
    if(1.0f >= mWeightsBase) mWeightsBase = 1.0001f;
    if(10.0f < mWeightsBase) mWeightsBase = 10.0f;
    MOD_LOG_INFO("mUseTemporalSmoothing = %d mWeightsBase = %f "
            "mTemporalStretch = %d[ms] \n",
            mUseTemporalSmoothing, mWeightsBase, mTemporalStretch);
}

void ISPC::ControlAWB_Planckian::getWBTSAlgorithm(
        bool &smoothingEnabled, float &base, int &temporalStretch)
{
    rbNodes.clear();
    smoothingEnabled = mUseTemporalSmoothing;
    base = mWeightsBase;
    temporalStretch = mTemporalStretch;
    MOD_LOG_INFO("mUseTemporalSmoothing = %d mWeightsBase = %f "
            "mTemporalStretch = %d[ms] \n",
            mUseTemporalSmoothing, mWeightsBase, mTemporalStretch);
}

void ISPC::ControlAWB_Planckian::setWBTSFeatures(
        unsigned int features)
{
    mFlashFilteringEnabled = (features & WBTS_FEATURES_FF) ? true: false;
//    ...Enabled = (features & WBTS_FEATURES_...) ? true: false;
    MOD_LOG_DEBUG("mFlashFilteringEnabled = %i \n", mFlashFilteringEnabled);
}

void ISPC::ControlAWB_Planckian::getWBTSFeatures(
        unsigned int &features)
{
    features = 0;
    if(mFlashFilteringEnabled) {
        features |= WBTS_FEATURES_FF;
    }
/*    if(...Enabled) {
 *      features |= WBTS_FEATURES_...;
 *    } */
    MOD_LOG_DEBUG("features = %i \n", features);
}

void ISPC::ControlAWB_Planckian::getFlashFiltering(bool &enabled)
{
    enabled = mFlashFilteringEnabled;
}

void ISPC::ControlAWB_Planckian::setFlashFiltering(const bool enabled)
{
    mFlashFilteringEnabled = enabled;
}

void ISPC::ControlAWB_Planckian::setWeightParam(float base)
{
    if(getMinWeightBase() > base)
    {
        mWeightsBase = getMinWeightBase();
        LOG_WARNING("WBTS: Weight generation parameter clipped to min");
    }
    else
    {
        if(getMaxWeightBase() < base)
        {
            /* higher values make no sense as 10 makes it really
             * close to no smoothing
             */
            mWeightsBase = getMaxWeightBase();
            LOG_WARNING("WBTS: Weight generation parameter clipped to max");
        }
        else
        {
            mWeightsBase = base;
        }
    }
    return;
}

float ISPC::ControlAWB_Planckian::getWeightParam(void)
{
    return mWeightsBase;
}

float ISPC::ControlAWB_Planckian::getMinWeightBase(void)
{
    return WEIGHT_BASE_MIN;
}

float ISPC::ControlAWB_Planckian::getMaxWeightBase(void)
{
    return WEIGHT_BASE_MAX;
}

float ISPC::ControlAWB_Planckian::weightSpread(const int nOfElem, const int index)
{
    float base;
    double a=1.0, sum=100.0;

    /* spreading weights as arithmetic sequence of percentage */
//    r = (float)200/(nOfElem*(nOfElem+1));
//    return (index+1)*r;

    /* spreading weights as geometric sequence of percentage */
    base = getWeightParam();
    if(base>1)
    {
#ifdef USE_MATH_NEON
        a = powf_neon(base, (float)index);
        sum = (1-powf_neon(base, (float)nOfElem))/(1-base);
#else
        a = pow(base, index);
        sum = (1-pow(base, nOfElem))/(1-base);
#endif
    }
    return float(100.0f*a/sum);
}

int ISPC::ControlAWB_Planckian::ts_getTemporalStretch() const
{
	return mTemporalStretch;
}

void ISPC::ControlAWB_Planckian::ts_setTemporalStretch(const int stretch)
{
	mTemporalStretch = stretch;
}

float ISPC::ControlAWB_Planckian::ts_getWeightBase() const
{
	return mWeightsBase;
}

void ISPC::ControlAWB_Planckian::ts_setWeightBase(const float weight)
{
	mWeightsBase = weight;
}

bool ISPC::ControlAWB_Planckian::ts_getSmoothing() const
{
	return mUseTemporalSmoothing;
}

void ISPC::ControlAWB_Planckian::ts_setSmoothing(const bool smoothing)
{
	mUseTemporalSmoothing = smoothing;
}

bool ISPC::ControlAWB_Planckian::ts_getFlashFiltering() const
{
	return mFlashFilteringEnabled;
}

void ISPC::ControlAWB_Planckian::generateWeights(const int size)
{
    /* the weight applied to most recent */
    float weight;
    rbWeights.clear();
    for(int x=0; x<size; x++) {
        weight = weightSpread(size, x);
        rbWeights.push_front(weight);
    }
    /* verify weights, they must add to 100% (with some tolerance) */
    IMG_ASSERT(size == rbWeights.size());
    weight = 0;
    for(std::deque<float>::iterator i = rbWeights.begin();
            i != rbWeights.end(); ++i)
    {
        weight += *i;
        MOD_LOG_DEBUG("*i = %f ", *i);
    }
    MOD_LOG_DEBUG("\n");
    IMG_ASSERT(weight > 98);
    IMG_ASSERT(weight < 102);
}

void ISPC::ControlAWB_Planckian::movingAverageWeighted(double &out_RG, double &out_BG)
{
    const int TIME_STRETCH_ms = getTemporalStretch();
    const int FPS = int(getFps()+0.5);
    const unsigned int AWB_FRAME_STRETCH = FPS*TIME_STRETCH_ms/1000;
    rbNode_t rbNode;

   // MOD_LOG_DEBUG("FPS = %d getFps() = %f\n", FPS, getFps());
   // MOD_LOG_DEBUG("TIME_STRETCH_ms = %d, AWB_FRAME_STRETCH = %d\n", TIME_STRETCH_ms, AWB_FRAME_STRETCH);
    while(rbNodes.size() > AWB_FRAME_STRETCH) {
        rbNodes.pop_back();
       // MOD_LOG_DEBUG(">AWB_FRAME_STRETCH\n");
    }
    if(rbNodes.size() != rbWeights.size()) {
        generateWeights(rbNodes.size());
       // MOD_LOG_DEBUG("generateWeights size = %d \n", rbWeights.size());
    }

    rbNode.rg = 0;
    rbNode.bg = 0;

    /* calculate average */
    std::deque<float>::iterator weight = rbWeights.begin();
    std::deque<rbNode_t>::iterator i = rbNodes.begin();
    for(; i != rbNodes.end(); ++i, ++weight)
    {
        // MOD_LOG_DEBUG("i->rg = %f i->bg = %f weight = %f \n", i->rg, i->bg, *weight);
        rbNode.rg = rbNode.rg + i->rg * (*weight/100);
        rbNode.bg = rbNode.bg + i->bg * (*weight/100);
        // MOD_LOG_DEBUG("rbNode = (%f %f) i = (%f %f) \n", rbNode.rg, rbNode.bg, i->rg * (*weight/100), i->bg * (*weight/100));
    }

   // MOD_LOG_DEBUG("last rbNode %f, %f   \n", out_RG, out_BG);
    out_RG = rbNode.rg;
    out_BG = rbNode.bg;
   // MOD_LOG_DEBUG("averaged %f, %f \n", out_RG, out_BG);

    return;
}

void ISPC::ControlAWB_Planckian::smoothingNone(double &out_RG, double &out_BG)
{
    const int TIME_STRETCH_S = getTemporalStretch();
    const int FPS = int(getFps()+0.5);
    const unsigned int AWB_FRAME_STRETCH = TIME_STRETCH_S*FPS;

    out_RG = rbNodes.begin()->rg;
    out_BG = rbNodes.begin()->bg;

    while(rbNodes.size() > AWB_FRAME_STRETCH) {
        rbNodes.pop_back();
    }
    return;
}
#if 0
void ISPC::ControlAWB_Planckian::movingAverage(double &out_RG, double &out_BG)
{
    const int TIME_STRETCH_S = getTemporalStretch();
    const int FPS = int(getFps()+0.5);
    const unsigned int AWB_FRAME_STRETCH = TIME_STRETCH_S*FPS;
    rbNode_t rbNode;

    while(rbNodes.size() > AWB_FRAME_STRETCH) {
        rbNodes.pop_back();
    }

    rbNode.rg = 0;
    rbNode.bg = 0;

    /* calculate average */
    for(std::deque<rbNode_t>::iterator i = rbNodes.begin();
            i != rbNodes.end(); ++i)
    {
        rbNode.rg = rbNode.rg + i->rg;
        rbNode.bg = rbNode.bg + i->bg;
    }

    out_RG = rbNode.rg / rbNodes.size();
    out_BG = rbNode.bg / rbNodes.size();

    return;
}
#endif

//void ISPC::ControlAWB_Planckian::temporalAWBsmoothing(const double in_RG, const double in_BG, double &out_RG, double &out_BG)
void ISPC::ControlAWB_Planckian::temporalAWBsmoothing(double &out_RG, double &out_BG)
{
    if (mUseTemporalSmoothing) {
        movingAverageWeighted(out_RG, out_BG);
    } else {
        smoothingNone(out_RG, out_BG);
    }
    return;
}

IMG_RESULT ISPC::ControlAWB_Planckian::update(const Metadata &metadata)
{
    LOG_PERF_IN();
    double fCRRed_smoothed, fCRBlue_smoothed;
#if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    ControlAE *pAE = NULL;
    ControlCMC *pCMC = NULL;
    
    double Newgain = 1.0;
    double dbRatio = 0.0;
    double dbDivisor = 1.0;

    pAE = pCamera->getControlModule<ISPC::ControlAE>();
    pCMC = pCamera->getControlModule<ISPC::ControlCMC>();
    Newgain = pAE->getNewGain();
#endif

    processStatistics(metadata.autoWhiteBalanceStats);

    if (0 == fRatioR)
    {
        fRatioR = 1.0;
    }
    if (0 == fRatioB)
    {
        fRatioB = 1.0;
    }
    const double fGainR = f_R_Qeff / fRatioR;
    const double fGainB = f_B_Qeff / fRatioB;

    fCRRed_smoothed = fRatioR;
    fCRBlue_smoothed = fRatioB;

    flashFiltering(fRatioR, fRatioB);

    temporalAWBsmoothing(fCRRed_smoothed, fCRBlue_smoothed);

    while(rbNodes.size() > (getTemporalStretch()*getFps())) {
        rbNodes.pop_back();
    }

    //MOD_LOG_DEBUG("PID Control ratio smoothed   R/G=%f B/G=%f\n", fCRRed_smoothed, fCRBlue_smoothed);

    measuredTemperature = colorTempCorrection.getCorrelatedTemperature(
                fCRRed_smoothed, 1.0, fCRBlue_smoothed);

    correctionTemperature = 6500 - (targetTemperature - measuredTemperature);

    if (doAwb)
    {
        const double fGainR = f_R_Qeff / fCRRed_smoothed;
        const double fGainB = f_B_Qeff / fCRBlue_smoothed;

        const ModuleAWS *aws = getPipelineOwner() ?
                getPipelineOwner()->getModule<ModuleAWS>() :
                NULL;
        IMG_ASSERT(aws);
        if (!aws->getDebugMode())
        {
            currentCCM =
                colorTempCorrection.getColorCorrection(correctionTemperature);

            currentCCM.gains[0][0] = fGainR;
            currentCCM.gains[0][1] = 1.0;
            currentCCM.gains[0][2] = 1.0;
            currentCCM.gains[0][3] = fGainB;
////////////////////////////////////////////////////////////
#if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
            pCamera->DynamicChangeRatio(Newgain, dbRatio);
            if ((!pCMC->blSensorGainLevelCtrl) ||
                ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGain_lv1))) {
            } else {
                if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGainLv1Interpolation)) {
                    dbDivisor = (1 + ((pCMC->dbCcmAttenuation_lv1 - 1) * dbRatio));
                } else if (pCMC->blSensorGainLevelCtrl && (Newgain <= pCMC->fSensorGain_lv2)) {
                    dbDivisor = pCMC->dbCcmAttenuation_lv1;
                } else if ((pCMC->blSensorGainLevelCtrl) && (Newgain <= pCMC->fSensorGainLv2Interpolation)) {
                    dbDivisor = (pCMC->dbCcmAttenuation_lv1 + ((pCMC->dbCcmAttenuation_lv2 - pCMC->dbCcmAttenuation_lv1) * dbRatio));
                } else {
                    if (!pCMC->bEnableInterpolationGamma) {
                        dbDivisor = pCMC->dbCcmAttenuation_lv2;
                    } else {
                        dbDivisor = (pCMC->dbCcmAttenuation_lv2 + ((pCMC->dbCcmAttenuation_lv3 - pCMC->dbCcmAttenuation_lv2) * dbRatio));
                    }
                }
                //printf("===>dbDivisor = %f, pCMC->dbCcmAttenuation_lv<%f, %f, %f>\n", dbDivisor, pCMC->dbCcmAttenuation_lv1, pCMC->dbCcmAttenuation_lv2, pCMC->dbCcmAttenuation_lv3);
                
                currentCCM.coefficients[0][0] = ((currentCCM.coefficients[0][0] - 1.0f) / dbDivisor) + 1.0f;
                currentCCM.coefficients[0][1] = ((currentCCM.coefficients[0][1] - 0.0f) / dbDivisor);
                currentCCM.coefficients[0][2] = ((currentCCM.coefficients[0][2] - 0.0f) / dbDivisor);

                currentCCM.coefficients[1][0] = ((currentCCM.coefficients[1][0] - 0.0f) / dbDivisor);
                currentCCM.coefficients[1][1] = ((currentCCM.coefficients[1][1] - 1.0f) / dbDivisor) + 1.0f;
                currentCCM.coefficients[1][2] = ((currentCCM.coefficients[1][2] - 0.0f) / dbDivisor);

                currentCCM.coefficients[2][0] = ((currentCCM.coefficients[2][0] - 0.0f) / dbDivisor);
                currentCCM.coefficients[2][1] = ((currentCCM.coefficients[2][1] - 0.0f) / dbDivisor);
                currentCCM.coefficients[2][2] = ((currentCCM.coefficients[2][2] - 1.0f) / dbDivisor) + 1.0f;
                currentCCM.offsets[0][0] = ((currentCCM.offsets[0][0] + 128.0f) / dbDivisor) - 128.0f;
                currentCCM.offsets[0][1] = ((currentCCM.offsets[0][1] + 128.0f) / dbDivisor) - 128.0f;
                currentCCM.offsets[0][2] = ((currentCCM.offsets[0][2] + 128.0f) / dbDivisor) - 128.0f;
            }
#endif //INFOTM_ENABLE_GAIN_LEVEL_IDX
#ifdef INFOTM_AUTO_CHANGE_CAPTURE_CCM
            if (pCMC->bEnableCaptureIQ && pCamera->GetCaptureIQFlag()) {
                int m = 0, n = 0;

                for (m = 0; m < 3; m++) {
                    for (n = 0; n < 3; n++) {
                        currentCCM.coefficients[m][n] = pCMC->fCmcCaptureModeCCMRatio*currentCCM.coefficients[m][n];
                    }
                    currentCCM.offsets[0][m] = pCMC->fCmcCaptureModeCCMRatio*currentCCM.offsets[0][m];
                }
            }
#endif

////////////////////////////////////////////////////////////
            programCorrection();
        }
#if VERBOSE_CONTROL_MODULES
        else {
            MOD_LOG_INFO("Skipping update due to AWS debug mode enabled!\n");
        }
#endif
    }

#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("gainR=%f gainB=%f ratioR=%f ratioB=%f temperature=%f\n",
            fGainR, fGainB,
            fRatioR, fRatioB,
            measuredTemperature);

    MOD_LOG_INFO("Estimated temp. %.0lf - target "\
                "temp. %.0lf --> correction temp. %.0lf\n",
                measuredTemperature, targetTemperature,
                correctionTemperature);
#endif

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

bool ISPC::ControlAWB_Planckian::hasConverged() const
{
    // not using a convergence state...
    return true;
}

IMG_RESULT ISPC::ControlAWB_Planckian::configureStatistics()
{
    ModuleAWS *aws = NULL;

    if (getPipelineOwner())
    {
        aws = getPipelineOwner()->getModule<ModuleAWS>();
    } else {
        MOD_LOG_ERROR("ControlAWB_Planckian has no pipeline owner! "\
            "Cannot configure statistics.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    configured = false;
    if (aws)
    {
        /* If we haven't programmed the image areas to gather statistics
         * from we do so */
        if (!statisticsRegionProgrammed)
        {
            const Sensor *sensor = getSensor();
            if (!sensor)
            {
                MOD_LOG_ERROR("ControlAE owner has no sensors!\n");
                return IMG_ERROR_NOT_INITIALISED;
            }

            if (!useCustomAwsConfig)
            {
                // use sensor calibration data
                ColorCorrection Uncorrected =
                    colorTempCorrection.getColorCorrection(6500.0);
                if(Uncorrected.isValid())
                {
                    f_B_Qeff = Uncorrected.gains[0][3];
                    f_R_Qeff = Uncorrected.gains[0][0];
                }
                aws->fLog2_B_Qeff = fLog2_B_Qeff = ISPC::log2(f_B_Qeff);
                aws->fLog2_R_Qeff = fLog2_R_Qeff = ISPC::log2(f_R_Qeff);

                // setup tiles covering full sensor area
                aws->ui16GridStartColumn = 0;
                aws->ui16GridStartRow = 0;
                aws->ui16TileWidth =
                        sensor->uiWidth / AWS_NUM_GRID_TILES_HORIZ;
                aws->ui16TileHeight =
                        sensor->uiHeight / AWS_NUM_GRID_TILES_VERT;
            } else {
                // override calibration using use AWS module configuration
                // helpful for validating with data generator
                fLog2_B_Qeff = aws->fLog2_B_Qeff;
                fLog2_R_Qeff = aws->fLog2_R_Qeff;
#ifdef USE_MATH_NEON
                f_R_Qeff = powf_neon((float)2.0, fLog2_R_Qeff);
                f_B_Qeff = powf_neon((float)2.0, fLog2_B_Qeff);
#else
                f_R_Qeff = pow(2, fLog2_R_Qeff);
                f_B_Qeff = pow(2, fLog2_B_Qeff);
#endif
            }
#if VERBOSE_CONTROL_MODULES
            MOD_LOG_INFO("fLog2_B_Qeff=%f fLog2_R_Qeff=%f\n",
                fLog2_B_Qeff, fLog2_R_Qeff);
#endif

            /*
             * determine lowest temperature calibration point
             * initialize bounding box with R and B gains
             * add AWS_BB_DISTANCE margin
             */
            int points = colorTempCorrection.size();
            if(points > 0)
            {
                // number of calibration points
                double minTemp = std::numeric_limits<double>::max();
                ColorCorrection cc;
                ColorCorrection lowPoint;

                for(int i=0;i < points;++i)
                {
                    cc = colorTempCorrection.getCorrection(i);
                    if(cc.temperature < minTemp)
                    {
                        minTemp = cc.temperature;
                        lowPoint = cc;
                    }
                }
                fLog2LowTempRatioR =
                    fLog2_R_Qeff-ISPC::log2(lowPoint.gains[0][0]) +
                    aws->fBbDist;
                fLog2LowTempRatioB =
                    fLog2_B_Qeff-ISPC::log2(lowPoint.gains[0][3]) -
                    aws->fBbDist;
            } else {
                MOD_LOG_INFO("No temperature calibration points defined\n");
            }
            aws->enabled = true;
        }
        aws->requestUpdate();
        configured = true;
        return IMG_SUCCESS;
    } else {
        MOD_LOG_ERROR("ControlAWB_Planckian cannot find WBS module.");
    }

    return IMG_ERROR_UNEXPECTED_STATE;
}

void ISPC::ControlAWB_Planckian::processStatistics(
        const MC_STATS_AWS& awsStats)
{
    double fCentroidRatioR = 0;
    double fCentroidRatioB = 0;
    int count = 0;

    // 1. invalidate tiles below low end bounding box
    // 2. calculate main centroid
    for (int i = 0; i < STATS_TILES_VERT; i++)
    {
        for (int j = 0; j < STATS_TILES_HORIZ; j++)
        {
            AwbTile& tile = awbTiles[i][j];
            tile = awsStats.tileStats[i][j];
            tile.setQeffs(fLog2_R_Qeff, fLog2_B_Qeff);
#if VERBOSE_CONTROL_MODULES
            const double temp =
                colorTempCorrection.getCorrelatedTemperature(
                     tile.getRatioR(), 1.0, tile.getRatioB());
            MOD_LOG_INFO("Tile[%d,%d] considered cr=%f cb=%f cg=%f cfa=%d "
                         "ratioR=%f ratioB=%f temp=%f\n",
                j, i,
                tile.fCollectedRed,
                tile.fCollectedBlue,
                tile.fCollectedGreen,
                tile.numCFAs,
                tile.ratioR, tile.ratioB,
                temp);
#endif
            if (tile.isValid() &&
                tile.applyBounds(fLog2LowTempRatioR, fLog2LowTempRatioB))
            {
#if USE_WEIGHTED_CENTROID
                fCentroidRatioR += tile.getRatioR()*tile.numCFAs;
                fCentroidRatioB += tile.getRatioB()*tile.numCFAs;
                count += tile.numCFAs;
#else
                fCentroidRatioR += tile.getRatioR();
                fCentroidRatioB += tile.getRatioB();
                ++count;
#endif
            }
        }
    }
    if(count)
    {
        fCentroidRatioR /= count;
        fCentroidRatioB /= count;
    } else {
        fCentroidRatioR = 1.0f;
        fCentroidRatioB = 1.0f;
    }
#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("Main centroid ratioR=%f ratioB=%f estimatedTemp=%f\n",
            fCentroidRatioR, fCentroidRatioB,
            colorTempCorrection.getCorrelatedTemperature(
                    fCentroidRatioR, 1.0, fCentroidRatioB));
#endif

    postprocessCentroid(fCentroidRatioR, fCentroidRatioB);

    // calc ratio from filtered tiles
    AwbTile stats;
    stats.setQeffs(fLog2_R_Qeff, fLog2_B_Qeff);
    for (int i = 0; i < STATS_TILES_VERT; i++)
    {
        for (int j = 0; j < STATS_TILES_HORIZ; j++)
        {
            AwbTile& tile = awbTiles[i][j];

            if (tile.isValid() &&
               tile.isCloseToCentroid(
                       fCentroidRatioR,
                       fCentroidRatioB,
                       fMaxRatioDistancePow2))
            {
#if VERBOSE_CONTROL_MODULES
                // tile debug section
                const double temp =
                    colorTempCorrection.getCorrelatedTemperature(
                         tile.ratioR, 1.0, tile.ratioB);

                MOD_LOG_INFO("Tile[%d,%d] cr=%f cb=%f cg=%f cfa=%d "
                             "ratioR=%f ratioB=%f temp=%f\n",
                    j, i,
                    tile.fCollectedRed,
                    tile.fCollectedBlue,
                    tile.fCollectedGreen,
                    tile.numCFAs,
                    tile.ratioR, tile.ratioB,
                    temp);
#endif
                // sum stats for each tile which passed the filtering
                stats += tile;
            }
        }
    }
    if(stats.isValid())
    {
        // calculate global ratio
        fRatioR = stats.getRatioR();
        fRatioB = stats.getRatioB();
    } else {
        fRatioR = fCentroidRatioR;
        fRatioB = fCentroidRatioB;
    }
}

bool ISPC::ControlAWB_Planckian::postprocessCentroid(
        double& fCentroidRatioR, double& fCentroidRatioB)
{
    const LineSegments& lines = colorTempCorrection.getLines();
    if(!lines.empty())
    {
        // 3. detect multiple clouds of 'white' tiles, move the centroid to the
        // strongest area
        // this part below on calibrated planckian locus
        // bypass it if calibration not loaded
        double projCentroidR = fCentroidRatioR;
        double projCentroidB = fCentroidRatioB;

        bool ret = lines.getRbProj(
                fCentroidRatioR, fCentroidRatioB,
                projCentroidR, projCentroidB);
        IMG_ASSERT(ret);
        double centroidAboveR=0.0, centroidAboveB=0.0;
        double centroidBelowR=0.0, centroidBelowB=0.0;
        double weightAbove=0.0, weightBelow=0.0;
        int countAbove=0, countBelow=0;
        for (int i = 0; i < STATS_TILES_VERT; i++)
        {
            for (int j = 0; j < STATS_TILES_HORIZ; j++)
            {
                AwbTile& tile = awbTiles[i][j];
                if (tile.isValid())
                {
                    // get point projected on closest line
                    double projR, projB;
                    lines.getRbProj(tile.ratioR, tile.ratioB, projR, projB);
                    // categorize the tiles according to relative position to
                    // centroid
                    if(projB < projCentroidB)
                    {
#if USE_WEIGHTED_CENTROID
                        centroidBelowR += tile.getRatioR()*tile.numCFAs;
                        centroidBelowB += tile.getRatioB()*tile.numCFAs;
                        countBelow += tile.numCFAs;
#else
                        centroidBelowR += tile.getRatioR();
                        centroidBelowB += tile.getRatioB();
                        ++countBelow;
#endif
                        weightBelow += tile.numCFAs;
                        //tilesBelow.push_back(tile);
                    } else {
#if USE_WEIGHTED_CENTROID
                        centroidAboveR += tile.getRatioR()*tile.numCFAs;
                        centroidAboveB += tile.getRatioB()*tile.numCFAs;
                        countAbove += tile.numCFAs;
#else
                        centroidAboveR += tile.getRatioR();
                        centroidAboveB += tile.getRatioB();
                        ++countAbove;
#endif
                        weightAbove += tile.numCFAs;
                        //tilesAbove.push_back(tile);
                    }
                }
            }
        }
        if(countAbove > 0)
        {
            centroidAboveR /= countAbove;
            centroidAboveB /= countAbove;
        }
        if(countBelow > 0)
        {
            centroidBelowR /= countBelow;
            centroidBelowB /= countBelow;
        }

        // 4. determine the relative position between centroids
        // conditionally modify the resulting centroid
        const double thresholdPow2 = 1.5 * 1.5 * fMaxRatioDistancePow2;
        bool strongAbove =
                LineSegment::getLengthPow2(
                        fCentroidRatioR, fCentroidRatioB,
                        centroidAboveR, centroidAboveB) < thresholdPow2;
        bool strongBelow =
                LineSegment::getLengthPow2(
                        fCentroidRatioR, fCentroidRatioB,
                        centroidBelowR, centroidBelowB) < thresholdPow2;

#if VERBOSE_CONTROL_MODULES
        MOD_LOG_INFO("above=(%f,%f) t1=%f strong=%d "
                     "below=(%f,%f) t2=%f strong=%d\n",
                centroidAboveR, centroidAboveB,
                colorTempCorrection.getCorrelatedTemperature_Calibrated(
                        centroidAboveR, 1.0, centroidAboveB),
                strongAbove,
                centroidBelowR, centroidBelowB,
                colorTempCorrection.getCorrelatedTemperature_Calibrated(
                        centroidBelowR, 1.0, centroidBelowB),
                strongBelow
                );
#endif

        if(strongAbove && !strongBelow)
        {
            fCentroidRatioR = centroidAboveR;
            fCentroidRatioB = centroidAboveB;
        } else if(!strongAbove && strongBelow) {
            fCentroidRatioR = centroidBelowR;
            fCentroidRatioB = centroidBelowB;
        } else {
            if(weightBelow < 0.7 * weightAbove)
            {
                fCentroidRatioR = centroidAboveR;
                fCentroidRatioB = centroidAboveB;
            } else if(weightAbove < 0.7 * weightBelow) {
                fCentroidRatioR = centroidBelowR;
                fCentroidRatioB = centroidBelowB;
            } else {
                // similar mass, take the one which is closer to planckian locus
                // what means the one which is 'whiter'
                double distanceAbove =
                    lines.getClosestLine(
                        fCentroidRatioR, fCentroidRatioB)->getDistancePow2(
                            centroidAboveR, centroidAboveB);
                double distanceBelow =
                    lines.getClosestLine(
                        fCentroidRatioR, fCentroidRatioB)->getDistancePow2(
                            centroidBelowR, centroidBelowB);
                if(distanceAbove < distanceBelow)
                {
                    fCentroidRatioR = centroidAboveR;
                    fCentroidRatioB = centroidAboveB;
                } else {
                    fCentroidRatioR = centroidBelowR;
                    fCentroidRatioB = centroidBelowB;
                }
            }
        }
    } else {
        MOD_LOG_ERROR("No Planckian locus approximation loaded, please provide"
                " at least two different calibration points\n");
        return false;
    }
    return true;
}

IMG_RESULT ISPC::ControlAWB_Planckian::programCorrection()
{
    std::list<Pipeline*>::iterator it;

    for (it = pipelineList.begin(); it != pipelineList.end(); it++)
    {
        ModuleWBC2_6 *wbc = (*it)->getModule<ModuleWBC2_6>();
        ModuleCCM *ccm = (*it)->getModule<ModuleCCM>();

        if (wbc && ccm)
        {
            // apply red and blue channel gain correction
            wbc->aWBGain[0] = currentCCM.gains[0][0];
            wbc->aWBGain[1] = currentCCM.gains[0][1];
            wbc->aWBGain[2] = currentCCM.gains[0][2];
            wbc->aWBGain[3] = currentCCM.gains[0][3];
            /// @ should update the clip as well?
            wbc->requestUpdate();

            // apply correction CCM
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    ccm->aMatrix[i * 3 + j] = currentCCM.coefficients[i][j];
                }
            }

            // apply correction offset
            ccm->aOffset[0] = currentCCM.offsets[0][0];
            ccm->aOffset[1] = currentCCM.offsets[0][1];
            ccm->aOffset[2] = currentCCM.offsets[0][2];

            ccm->requestUpdate();
        } else {
#if VERBOSE_CONTROL_MODULES
            MOD_LOG_WARNING("one pipeline is missing either CCM or "\
                "WBC modules\n");
#endif
        }
    }

    return IMG_SUCCESS;
}

std::ostream& ISPC::ControlAWB_Planckian::printState(
        std::ostream &os) const
{
    os << SAVE_A1 << getLoggingName() << ":" << std::endl;

    os << SAVE_A2 << "Config:" << std::endl;
    os << SAVE_A3 << "enabled = " << enabled << std::endl;
    os << SAVE_A3 << "Qer = " << fLog2_R_Qeff << std::endl;
    os << SAVE_A3 << "Qeb = " << fLog2_B_Qeff << std::endl;
    os << SAVE_A3 << "log2(LowTempRatioR) = " << fLog2LowTempRatioR << std::endl;
    os << SAVE_A3 << "log2(LowTempRatioB) = " << fLog2LowTempRatioB << std::endl;
    os << SAVE_A3 << "MaxDistance = " << fMaxRatioDistance << std::endl;
    os << SAVE_A3 << "useCustomAwsConfig = " << useCustomAwsConfig << std::endl;

    os << SAVE_A2 << "Temporal proc config:" << std::endl;
    os << SAVE_A3 << "weights base = " << mWeightsBase << std::endl;
    os << SAVE_A3 << "flash filter = " << mFlashFilteringEnabled << std::endl;
    os << SAVE_A3 << "mode = " << (int)mUseTemporalSmoothing << std::endl;
    os << SAVE_A3 << "stretch [ms] = " << mTemporalStretch << std::endl;

    os << SAVE_A2 << "Capture state:" << std::endl;
    os << SAVE_A3 << "ratioR = " << fRatioR << std::endl;
    os << SAVE_A3 << "ratioB = " << fRatioB << std::endl;
    os << SAVE_A3 << "estimated temperature = " <<
            colorTempCorrection.getCorrelatedTemperature(
                    fRatioR, 1.0, fRatioB) << std::endl;

    os << SAVE_A2 << "Final valid tiles:" << std::endl;
    for (int i = 0; i < STATS_TILES_VERT; i++)
    {
        for (int j = 0; j < STATS_TILES_HORIZ; j++)
        {
            const AwbTile& tile = awbTiles[i][j];
            if(tile.isValid())
            {
                os << SAVE_A3 << "tile[" << i << "," << j << "] ";
                os << "R=" << tile.ratioR << " B=" << tile.ratioB << std::endl;
            }
        }
    }
    return os;
}

#ifdef INFOTM_ISP
void ISPC::ControlAWB_Planckian::enableAWB(bool enable)
{
	this->doAwb = enable;
}

bool ISPC::ControlAWB_Planckian::IsAWBenabled() const
{
	return this->doAwb;
}

void ISPC::ControlAWB_Planckian::enableControl(bool enable)
{
    this->doAwb = enable;
}
#endif

