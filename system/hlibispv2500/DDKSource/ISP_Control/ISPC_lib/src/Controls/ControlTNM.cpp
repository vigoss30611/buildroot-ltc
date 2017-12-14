/**
******************************************************************************
@file ControlTNM.cpp

@brief ISPC::ControlTNM implementation

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
#include "ispc/ControlTNM.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_TNM"

#include <string>
#include <list>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/ModuleHIS.h"
#include "ispc/ModuleTNM.h"
#include "ispc/Sensor.h"
#include "ispc/Pipeline.h"
#include "ispc/ISPCDefs.h"
#ifdef INFOTM_ISP
#include <ispc/Camera.h>
#include <cmath>
#endif //INFOTM_ISP

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

#ifdef INFOTM_ISP
typedef enum _TNME_SMOOTH_HISTOGRAM_METHOD
{
	TNMMHD_SH_DEFAULT = 0,
	TNMMHD_SH_SHUOYING
} TNME_SMOOTH_HISTOGRAM_METHOD;
#endif //INFOTM_ISP

const ISPC::ParamDef<float> ISPC::ControlTNM::TNMC_TEMPERING(
    "TNMC_TEMPERING", 0.0f, 1.0f, 0.20f);
const ISPC::ParamDef<float> ISPC::ControlTNM::TNMC_HISTMIN(
    "TNMC_HIST_CLIP_MIN", 0.0f, 1.0f, 0.035f);
const ISPC::ParamDef<float> ISPC::ControlTNM::TNMC_HISTMAX(
    "TNMC_HIST_CLIP_MAX", 0.0f, 1.0f, 0.25f);
const ISPC::ParamDef<float> ISPC::ControlTNM::TNMC_SMOOTHING(
    "TNMC_SMOOTHING", 0.0f, 1.0f, 0.4f);
const ISPC::ParamDef<float> ISPC::ControlTNM::TNMC_UPDATESPEED(
    "TNMC_UPDATE_SPEED", 0.0f, 1.0f, 0.5f);
const ISPC::ParamDefSingle<bool> ISPC::ControlTNM::TNMC_LOCAL(
    "TNMC_LOCAL", false);
const ISPC::ParamDef<float> ISPC::ControlTNM::TNMC_LOCAL_STRENGTH(
    "TNMC_LOCALSTRENGTH", 0.0f, 1.0f, 0.0f);
const ISPC::ParamDefSingle<bool> ISPC::ControlTNM::TNMC_ADAPTIVE(
    "TNMC_ADAPTIVE", false);
const ISPC::ParamDefSingle<bool> ISPC::ControlTNM::TNMC_ADAPTIVE_FCM(
    "TNMC_ADAPTIVE_FCM", false);

#ifdef INFOTM_ISP
static const double TNMC_WDR_CEILING_DEF[TNMC_WDR_SEGMENT_CNT] =
{
    //1.1, 1.3, 1.4, 1.4, 1.3, 1.0, 1.0, 1.0
    //1.2, 1.4, 1.6, 1.6, 1.6, 1.4, 1.0, 1.0
    1.2, 1.7, 2.0, 2.0, 2.0, 1.5, 1.0, 1.0
};
static const double TNMC_WDR_FLOOR_DEF[TNMC_WDR_SEGMENT_CNT] =
{
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
};

const ISPC::ParamDef<int> ISPC::ControlTNM::TNMC_SMOOTH_HISTOGRAM_METHOD(
    "TNMC_SMOOTH_HISTOGRAM_METHOD", 0, 1, 0);
const ISPC::ParamDefSingle<bool> ISPC::ControlTNM::TNMC_ENABLE_EQUALIZATION(
    "TNMC_ENABLE_EQUALIZATION", false);
const ISPC::ParamDefSingle<bool> ISPC::ControlTNM::TNMC_ENABLE_GAMMA(
    "TNMC_ENABLE_GAMMA", false);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_EQUAL_MAXDARKSUPRESS(
    "TNMC_EQUAL_MAXDARKSUPRESS", 0.0, 1.0, 0.15);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_EQUAL_MAXBRIGHTSUPRESS(
    "TNMC_EQUAL_MAXBRIGHTSUPRESS", 0.0, 1.0, 0.15);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_EQUAL_DARKSUPRESSRATIO(
    "TNMC_EQUAL_DARKSUPRESSRATIO", 0.0, 1.0, 0.01);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_EQUAL_BRIGHTSUPRESSRATIO(
    "TNMC_EQUAL_BRIGHTSUPRESSRATIO", 0.0, 1.0, 0.002);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_EQUAL_FACTOR(
    "TNMC_EQUAL_FACTOR", 0.0, 1.0, 0.05);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_OVERSHOT_THRESHOLD(
    "TNMC_OVERSHOT_THRESHOLD", TNMC_WDR_MIN, TNMC_WDR_MAX, 2.0);
const ISPC::ParamDefArray<double> ISPC::ControlTNM::TNMC_WDR_CEILING(
    "TNMC_WDR_CEILING", TNMC_WDR_MIN, TNMC_WDR_MAX, TNMC_WDR_CEILING_DEF, TNMC_WDR_SEGMENT_CNT);
const ISPC::ParamDefArray<double> ISPC::ControlTNM::TNMC_WDR_FLOOR(
    "TNMC_WDR_FLOOR", TNMC_WDR_MIN, TNMC_WDR_MAX, TNMC_WDR_FLOOR_DEF, TNMC_WDR_SEGMENT_CNT);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_MAPCURVE_UPDATE_DAMP(
    "TNMC_MAPCURVE_UPDATE_DAMP", 0.0, 1.0, 0.3);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_MAPCURVE_POWER_VALUE(
    "TNMC_MAPCURVE_POWER_VALUE", 0.0, 8.0, 2.0);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_GAMMA_FCM(
	"TNMC_GAMMA_FCM", 0.0, 2.0, 1.0);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_GAMMA_ACM(
	"TNMC_GAMMA_ACM", 0.0, 2.0, 1.0);
const ISPC::ParamDef<int> ISPC::ControlTNM::TNMC_GAMMA_CRV_MODE(
	"TNMC_GAMMA_CRV_MODE", 0, 2, 0);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_BEZIER_CTRL_PNT(
	"TNMC_BEZIER_CTRL_PNT", 0.0, 1.0, 0.83);
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
const ISPC::ParamDefSingle<bool> ISPC::ControlTNM::TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE(
    "TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE", false);
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_CRV_BRIGNTNESS(
    "TNMC_CRV_BRIGNTNESS", -0.5, 0.5, 0.0);
    #if 0
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_CRV_CONTRAST(
    "TNMC_CRV_CONTRAST", 0.0, 2.0, 1.0);
    #else
const ISPC::ParamDef<double> ISPC::ControlTNM::TNMC_CRV_CONTRAST(
    "TNMC_CRV_CONTRAST", 0.7, 1.25, 1.0);
    #endif
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP


#ifndef INFOTM_ENABLE_COLOR_MODE_CHANGE
#define TNMGAMMA            0.91f
#endif

#ifdef INFOTM_ISP
static int ui32SmoothHistogramMethod;
static bool bEnableEqualization;
static bool bEnableGamma;
static double fEqualMaxDarkSupress;
static double fEqualMaxBrightSupress;
static double fEqualDarkSupressRatio;
static double fEqualBrightSupressRatio;
static double fEqualFactor;
static double fOvershotThreshold;
static double fWdrCeiling[TNMC_WDR_SEGMENT_CNT];
static double fWdrFloor[TNMC_WDR_SEGMENT_CNT];
static double fMapCurveUpdateDamp;
static double fMapCurvePowerValue;
static double fGammaFCM;
static double fGammaACM;
static int ui32GammaCrvMode;
static double fBezierCtrlPnt;
#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
static bool bTnmCrvSimBrightnessContrastEnable;
static double fBrightness;
static double fContrast;
#endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP

ISPC::ParameterGroup ISPC::ControlTNM::getGroup()
{
    ParameterGroup group;

    group.header = "// Tone Mapper Control parameters";

    group.parameters.insert(TNMC_TEMPERING.name);
    group.parameters.insert(TNMC_HISTMIN.name);
    group.parameters.insert(TNMC_HISTMAX.name);
    group.parameters.insert(TNMC_SMOOTHING.name);
    group.parameters.insert(TNMC_UPDATESPEED.name);
    group.parameters.insert(TNMC_LOCAL.name);
    group.parameters.insert(TNMC_LOCAL_STRENGTH.name);
    group.parameters.insert(TNMC_ADAPTIVE.name);
    group.parameters.insert(TNMC_ADAPTIVE_FCM.name);

#ifdef INFOTM_ISP
    group.parameters.insert(TNMC_SMOOTH_HISTOGRAM_METHOD.name);
    group.parameters.insert(TNMC_ENABLE_EQUALIZATION.name);
    group.parameters.insert(TNMC_ENABLE_GAMMA.name);
    group.parameters.insert(TNMC_EQUAL_MAXDARKSUPRESS.name);
    group.parameters.insert(TNMC_EQUAL_MAXBRIGHTSUPRESS.name);
    group.parameters.insert(TNMC_EQUAL_DARKSUPRESSRATIO.name);
    group.parameters.insert(TNMC_EQUAL_BRIGHTSUPRESSRATIO.name);
    group.parameters.insert(TNMC_EQUAL_FACTOR.name);
    group.parameters.insert(TNMC_OVERSHOT_THRESHOLD.name);
    group.parameters.insert(TNMC_WDR_CEILING.name);
    group.parameters.insert(TNMC_WDR_FLOOR.name);
    group.parameters.insert(TNMC_MAPCURVE_UPDATE_DAMP.name);
    group.parameters.insert(TNMC_MAPCURVE_POWER_VALUE.name);
    group.parameters.insert(TNMC_GAMMA_FCM.name);
    group.parameters.insert(TNMC_GAMMA_ACM.name);
    group.parameters.insert(TNMC_GAMMA_CRV_MODE.name);
    group.parameters.insert(TNMC_BEZIER_CTRL_PNT.name);
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
    group.parameters.insert(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.name);
    group.parameters.insert(TNMC_CRV_BRIGNTNESS.name);
    group.parameters.insert(TNMC_CRV_CONTRAST.name);
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP

    return group;
}

ISPC::ControlTNM::ControlTNM(const std::string &logname)
    : ControlModuleBase(logname),
    adaptiveStrength(1.0),
    histMin(TNMC_HISTMIN.def),
    histMax(TNMC_HISTMAX.def),
    smoothing(TNMC_SMOOTHING.def),
    tempering(TNMC_TEMPERING.def),
    updateSpeed(TNMC_UPDATESPEED.def),
    histogram(1, TNMC_N_HIST),
    mappingCurve(1, TNMC_N_CURVE),
    localTNM(TNMC_LOCAL.def),
    adaptiveTNM(TNMC_ADAPTIVE.def),
    adaptiveTNM_FCM(TNMC_ADAPTIVE_FCM.def),
    localStrength(TNMC_LOCAL_STRENGTH.def),
    configureHis(false)
{
    resetHistogram(histogram);
    resetCurve(mappingCurve);

#ifdef INFOTM_ISP
	ui32SmoothHistogramMethod = TNMC_SMOOTH_HISTOGRAM_METHOD.def;
	bEnableEqualization = TNMC_ENABLE_EQUALIZATION.def;
	bEnableGamma = TNMC_ENABLE_GAMMA.def;
	fEqualMaxDarkSupress = TNMC_EQUAL_MAXDARKSUPRESS.def;
	fEqualMaxBrightSupress = TNMC_EQUAL_MAXBRIGHTSUPRESS.def;
	fEqualDarkSupressRatio = TNMC_EQUAL_DARKSUPRESSRATIO.def;
	fEqualBrightSupressRatio = TNMC_EQUAL_BRIGHTSUPRESSRATIO.def;
	fEqualFactor = TNMC_EQUAL_FACTOR.def;
	fOvershotThreshold = TNMC_OVERSHOT_THRESHOLD.def;
	for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
		fWdrCeiling[i] = TNMC_WDR_CEILING_DEF[i];
	}
	for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
		fWdrCeiling[i] = TNMC_WDR_FLOOR_DEF[i];
	}
	fMapCurveUpdateDamp = TNMC_MAPCURVE_UPDATE_DAMP.def;
	fMapCurvePowerValue = TNMC_MAPCURVE_POWER_VALUE.def;
	fGammaFCM = TNMC_GAMMA_FCM.def;
	fGammaACM = TNMC_GAMMA_ACM.def;
	ui32GammaCrvMode = TNMC_GAMMA_CRV_MODE.def;
	fBezierCtrlPnt = TNMC_BEZIER_CTRL_PNT.def;
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
	bTnmCrvSimBrightnessContrastEnable = TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.def;
	fBrightness = TNMC_CRV_BRIGNTNESS.def;
	fContrast = TNMC_CRV_CONTRAST.def;
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP
}

//load the module values from a parameter list
IMG_RESULT ISPC::ControlTNM::load(const ParameterList &parameters)
{
    this->tempering = parameters.getParameter(TNMC_TEMPERING);
    this->histMin = parameters.getParameter(TNMC_HISTMIN);
    this->histMax = parameters.getParameter(TNMC_HISTMAX);
    this->smoothing = parameters.getParameter(TNMC_SMOOTHING);
    this->updateSpeed = parameters.getParameter(TNMC_UPDATESPEED);
    this->localTNM = parameters.getParameter(TNMC_LOCAL);
    this->localStrength = parameters.getParameter(TNMC_LOCAL_STRENGTH);
    this->adaptiveTNM = parameters.getParameter(TNMC_ADAPTIVE);
    this->adaptiveTNM_FCM= parameters.getParameter(TNMC_ADAPTIVE_FCM);
    // adaptive strength not accessible from outside
#ifdef INFOTM_ISP
    ui32SmoothHistogramMethod = parameters.getParameter(TNMC_SMOOTH_HISTOGRAM_METHOD);
    bEnableEqualization = parameters.getParameter(TNMC_ENABLE_EQUALIZATION);
    bEnableGamma = parameters.getParameter(TNMC_ENABLE_GAMMA);
    fEqualMaxDarkSupress = parameters.getParameter(TNMC_EQUAL_MAXDARKSUPRESS);
    fEqualMaxBrightSupress = parameters.getParameter(TNMC_EQUAL_MAXBRIGHTSUPRESS);
    fEqualDarkSupressRatio = parameters.getParameter(TNMC_EQUAL_DARKSUPRESSRATIO);
    fEqualBrightSupressRatio = parameters.getParameter(TNMC_EQUAL_BRIGHTSUPRESSRATIO);
    fEqualFactor = parameters.getParameter(TNMC_EQUAL_FACTOR);
    fOvershotThreshold = parameters.getParameter(TNMC_OVERSHOT_THRESHOLD);
    for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
        fWdrCeiling[i] = parameters.getParameter(TNMC_WDR_CEILING, i);
    }
    for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
        fWdrFloor[i] = parameters.getParameter(TNMC_WDR_FLOOR, i);
    }
#if 0
    printf("===== WDR Ceiling start =====\n");
    for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
        printf("%f ", fWdrCeiling[i]);
    }
    printf("===== WDR Ceiling end =====\n");
#endif
    fMapCurveUpdateDamp = parameters.getParameter(TNMC_MAPCURVE_UPDATE_DAMP);
    fMapCurvePowerValue = parameters.getParameter(TNMC_MAPCURVE_POWER_VALUE);
    fGammaFCM =  parameters.getParameter(TNMC_GAMMA_FCM);
    fGammaACM =  parameters.getParameter(TNMC_GAMMA_ACM);
    ui32GammaCrvMode = parameters.getParameter(TNMC_GAMMA_CRV_MODE);
    fBezierCtrlPnt = parameters.getParameter(TNMC_BEZIER_CTRL_PNT);
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
    bTnmCrvSimBrightnessContrastEnable = parameters.getParameter(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE);
    fBrightness = parameters.getParameter(TNMC_CRV_BRIGNTNESS);
    fContrast = parameters.getParameter(TNMC_CRV_CONTRAST);
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::save(ParameterList &parameters, SaveType t) const
{
#ifdef INFOTM_ISP
    std::vector<std::string> values;
#endif //INFOTM_ISP
    static ParameterGroup group;

    if (group.parameters.size() == 0)
    {
        group = ControlTNM::getGroup();
    }

    parameters.addGroup("ControlTNM", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(Parameter(TNMC_TEMPERING.name,
            toString(this->tempering)));
        parameters.addParameter(Parameter(TNMC_HISTMIN.name,
            toString(this->histMin)));
        parameters.addParameter(Parameter(TNMC_HISTMAX.name,
            toString(this->histMax)));
        parameters.addParameter(Parameter(TNMC_SMOOTHING.name,
            toString(this->smoothing)));
        parameters.addParameter(Parameter(TNMC_UPDATESPEED.name,
            toString(this->updateSpeed)));
        parameters.addParameter(Parameter(TNMC_LOCAL.name,
            toString(this->localTNM)));
        parameters.addParameter(Parameter(TNMC_LOCAL_STRENGTH.name,
            toString(this->localStrength)));
        parameters.addParameter(Parameter(TNMC_ADAPTIVE.name,
            toString(this->adaptiveTNM)));
        parameters.addParameter(Parameter(TNMC_ADAPTIVE_FCM.name,
            toString(this->adaptiveTNM_FCM)));
        // adaptive strength not accessible from outside
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNMC_SMOOTH_HISTOGRAM_METHOD.name,
            toString(ui32SmoothHistogramMethod)));
        parameters.addParameter(Parameter(TNMC_ENABLE_EQUALIZATION.name,
            toString(bEnableEqualization)));
        parameters.addParameter(Parameter(TNMC_ENABLE_GAMMA.name,
            toString(bEnableGamma)));
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXDARKSUPRESS.name,
            toString(fEqualMaxDarkSupress)));
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXBRIGHTSUPRESS.name,
            toString(fEqualMaxBrightSupress)));
        parameters.addParameter(Parameter(TNMC_EQUAL_DARKSUPRESSRATIO.name,
            toString(fEqualDarkSupressRatio)));
        parameters.addParameter(Parameter(TNMC_EQUAL_BRIGHTSUPRESSRATIO.name,
            toString(fEqualBrightSupressRatio)));
        parameters.addParameter(Parameter(TNMC_EQUAL_FACTOR.name,
            toString(fEqualFactor)));
        parameters.addParameter(Parameter(TNMC_OVERSHOT_THRESHOLD.name,
            toString(fOvershotThreshold)));
        values.clear();
        for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
            values.push_back(toString(fWdrCeiling[i]));
        }
        parameters.addParameter(Parameter(TNMC_WDR_CEILING.name, values));
        values.clear();
        for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
            values.push_back(toString(fWdrFloor[i]));
        }
        parameters.addParameter(Parameter(TNMC_WDR_FLOOR.name, values));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_UPDATE_DAMP.name,
            toString(fMapCurveUpdateDamp)));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_POWER_VALUE.name,
            toString(fMapCurvePowerValue)));
        parameters.addParameter(Parameter(TNMC_GAMMA_FCM.name,
            toString(fGammaFCM)));
        parameters.addParameter(Parameter(TNMC_GAMMA_ACM.name,
            toString(fGammaACM)));
        parameters.addParameter(Parameter(TNMC_GAMMA_CRV_MODE.name,
            toString(ui32GammaCrvMode)));
        parameters.addParameter(Parameter(TNMC_BEZIER_CTRL_PNT.name,
            toString(fBezierCtrlPnt)));
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
        parameters.addParameter(Parameter(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.name,
            toString(bTnmCrvSimBrightnessContrastEnable)));
        parameters.addParameter(Parameter(TNMC_CRV_BRIGNTNESS.name,
            toString(fBrightness)));
        parameters.addParameter(Parameter(TNMC_CRV_CONTRAST.name,
            toString(fContrast)));
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP
        break;

    case SAVE_MIN:
        parameters.addParameter(Parameter(TNMC_TEMPERING.name,
            toString(TNMC_TEMPERING.min)));
        parameters.addParameter(Parameter(TNMC_HISTMIN.name,
            toString(TNMC_HISTMIN.min)));
        parameters.addParameter(Parameter(TNMC_HISTMAX.name,
            toString(TNMC_HISTMAX.min)));
        parameters.addParameter(Parameter(TNMC_SMOOTHING.name,
            toString(TNMC_SMOOTHING.min)));
        parameters.addParameter(Parameter(TNMC_UPDATESPEED.name,
            toString(TNMC_UPDATESPEED.min)));
        parameters.addParameter(Parameter(TNMC_LOCAL.name,
            toString(TNMC_LOCAL.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_LOCAL_STRENGTH.name,
            toString(TNMC_LOCAL_STRENGTH.min)));
        parameters.addParameter(Parameter(TNMC_ADAPTIVE.name,
            toString(TNMC_ADAPTIVE.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_ADAPTIVE_FCM.name,
            toString(TNMC_ADAPTIVE_FCM.def)));  // bool do not have min
        // adaptive strength not accessible from outside
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNMC_SMOOTH_HISTOGRAM_METHOD.name,
            toString(TNMC_SMOOTH_HISTOGRAM_METHOD.min)));
        parameters.addParameter(Parameter(TNMC_ENABLE_EQUALIZATION.name,
            toString(TNMC_ENABLE_EQUALIZATION.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_ENABLE_GAMMA.name,
            toString(TNMC_ENABLE_GAMMA.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXDARKSUPRESS.name,
            toString(TNMC_EQUAL_MAXDARKSUPRESS.min)));
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXBRIGHTSUPRESS.name,
            toString(TNMC_EQUAL_MAXBRIGHTSUPRESS.min)));
        parameters.addParameter(Parameter(TNMC_EQUAL_DARKSUPRESSRATIO.name,
            toString(TNMC_EQUAL_DARKSUPRESSRATIO.min)));
        parameters.addParameter(Parameter(TNMC_EQUAL_BRIGHTSUPRESSRATIO.name,
            toString(TNMC_EQUAL_BRIGHTSUPRESSRATIO.min)));
        parameters.addParameter(Parameter(TNMC_EQUAL_FACTOR.name,
            toString(TNMC_EQUAL_FACTOR.min)));
        parameters.addParameter(Parameter(TNMC_OVERSHOT_THRESHOLD.name,
            toString(TNMC_OVERSHOT_THRESHOLD.min)));
        parameters.addParameter(Parameter(TNMC_WDR_CEILING.name,
            toString(TNMC_WDR_CEILING.min)));
        parameters.addParameter(Parameter(TNMC_WDR_FLOOR.name,
            toString(TNMC_WDR_FLOOR.min)));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_UPDATE_DAMP.name,
            toString(TNMC_MAPCURVE_UPDATE_DAMP.min)));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_POWER_VALUE.name,
            toString(TNMC_MAPCURVE_POWER_VALUE.min)));
        parameters.addParameter(Parameter(TNMC_GAMMA_FCM.name,
            toString(TNMC_GAMMA_FCM.min)));
        parameters.addParameter(Parameter(TNMC_GAMMA_ACM.name,
            toString(TNMC_GAMMA_ACM.min)));
        parameters.addParameter(Parameter(TNMC_GAMMA_CRV_MODE.name,
            toString(TNMC_GAMMA_CRV_MODE.min)));
        parameters.addParameter(Parameter(TNMC_BEZIER_CTRL_PNT.name,
            toString(TNMC_BEZIER_CTRL_PNT.min)));
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
        parameters.addParameter(Parameter(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.name,
            toString(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_CRV_BRIGNTNESS.name,
            toString(TNMC_CRV_BRIGNTNESS.min)));
        parameters.addParameter(Parameter(TNMC_CRV_CONTRAST.name,
            toString(TNMC_CRV_CONTRAST.min)));
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP
        break;

    case SAVE_MAX:
        parameters.addParameter(Parameter(TNMC_TEMPERING.name,
            toString(TNMC_TEMPERING.max)));
        parameters.addParameter(Parameter(TNMC_HISTMIN.name,
            toString(TNMC_HISTMIN.max)));
        parameters.addParameter(Parameter(TNMC_HISTMAX.name,
            toString(TNMC_HISTMAX.max)));
        parameters.addParameter(Parameter(TNMC_SMOOTHING.name,
            toString(TNMC_SMOOTHING.max)));
        parameters.addParameter(Parameter(TNMC_UPDATESPEED.name,
            toString(TNMC_UPDATESPEED.max)));
        parameters.addParameter(Parameter(TNMC_LOCAL.name,
            toString(TNMC_LOCAL.def)));  // bool do not have max
        parameters.addParameter(Parameter(TNMC_LOCAL_STRENGTH.name,
            toString(TNMC_LOCAL_STRENGTH.max)));
        parameters.addParameter(Parameter(TNMC_ADAPTIVE.name,
            toString(TNMC_ADAPTIVE.def)));  // bool do not have max
        parameters.addParameter(Parameter(TNMC_ADAPTIVE_FCM.name,
            toString(TNMC_ADAPTIVE_FCM.def)));  // bool do not have max
        // adaptive strength not accessible from outside
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNMC_SMOOTH_HISTOGRAM_METHOD.name,
            toString(TNMC_SMOOTH_HISTOGRAM_METHOD.max)));
        parameters.addParameter(Parameter(TNMC_ENABLE_EQUALIZATION.name,
            toString(TNMC_ENABLE_EQUALIZATION.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_ENABLE_GAMMA.name,
            toString(TNMC_ENABLE_GAMMA.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXDARKSUPRESS.name,
            toString(TNMC_EQUAL_MAXDARKSUPRESS.max)));
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXBRIGHTSUPRESS.name,
            toString(TNMC_EQUAL_MAXBRIGHTSUPRESS.max)));
        parameters.addParameter(Parameter(TNMC_EQUAL_DARKSUPRESSRATIO.name,
            toString(TNMC_EQUAL_DARKSUPRESSRATIO.max)));
        parameters.addParameter(Parameter(TNMC_EQUAL_BRIGHTSUPRESSRATIO.name,
            toString(TNMC_EQUAL_BRIGHTSUPRESSRATIO.max)));
        parameters.addParameter(Parameter(TNMC_EQUAL_FACTOR.name,
            toString(TNMC_EQUAL_FACTOR.max)));
        parameters.addParameter(Parameter(TNMC_OVERSHOT_THRESHOLD.name,
            toString(TNMC_OVERSHOT_THRESHOLD.max)));
        parameters.addParameter(Parameter(TNMC_WDR_CEILING.name,
            toString(TNMC_WDR_CEILING.max)));
        parameters.addParameter(Parameter(TNMC_WDR_FLOOR.name,
            toString(TNMC_WDR_FLOOR.max)));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_UPDATE_DAMP.name,
            toString(TNMC_MAPCURVE_UPDATE_DAMP.max)));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_POWER_VALUE.name,
            toString(TNMC_MAPCURVE_POWER_VALUE.max)));
        parameters.addParameter(Parameter(TNMC_GAMMA_FCM.name,
            toString(TNMC_GAMMA_FCM.max)));
        parameters.addParameter(Parameter(TNMC_GAMMA_ACM.name,
            toString(TNMC_GAMMA_ACM.max)));
        parameters.addParameter(Parameter(TNMC_GAMMA_CRV_MODE.name,
            toString(TNMC_GAMMA_CRV_MODE.max)));
        parameters.addParameter(Parameter(TNMC_BEZIER_CTRL_PNT.name,
            toString(TNMC_BEZIER_CTRL_PNT.max)));
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
        parameters.addParameter(Parameter(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.name,
            toString(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.def)));  // bool do not have min
        parameters.addParameter(Parameter(TNMC_CRV_BRIGNTNESS.name,
            toString(TNMC_CRV_BRIGNTNESS.max)));
        parameters.addParameter(Parameter(TNMC_CRV_CONTRAST.name,
            toString(TNMC_CRV_CONTRAST.max)));
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP
        break;

    case SAVE_DEF:
        parameters.addParameter(Parameter(TNMC_TEMPERING.name,
            toString(TNMC_TEMPERING.def)
            + " // " + getParameterInfo(TNMC_TEMPERING)));
        parameters.addParameter(Parameter(TNMC_HISTMIN.name,
            toString(TNMC_HISTMIN.def)
            + " // " + getParameterInfo(TNMC_HISTMIN)));
        parameters.addParameter(Parameter(TNMC_HISTMAX.name,
            toString(TNMC_HISTMAX.def)
            + " // " + getParameterInfo(TNMC_HISTMAX)));
        parameters.addParameter(Parameter(TNMC_SMOOTHING.name,
            toString(TNMC_SMOOTHING.def)
            + " // " + getParameterInfo(TNMC_SMOOTHING)));
        parameters.addParameter(Parameter(TNMC_UPDATESPEED.name,
            toString(TNMC_UPDATESPEED.def)
            + " // " + getParameterInfo(TNMC_UPDATESPEED)));
        parameters.addParameter(Parameter(TNMC_LOCAL.name,
            toString(TNMC_LOCAL.def)
            + " // " + getParameterInfo(TNMC_LOCAL)));
        parameters.addParameter(Parameter(TNMC_LOCAL_STRENGTH.name,
            toString(TNMC_LOCAL_STRENGTH.def)
            + " // " + getParameterInfo(TNMC_LOCAL_STRENGTH)));
        parameters.addParameter(Parameter(TNMC_ADAPTIVE.name,
            toString(TNMC_ADAPTIVE.def)
            + " // " + getParameterInfo(TNMC_ADAPTIVE)));
        parameters.addParameter(Parameter(TNMC_ADAPTIVE_FCM.name,
            toString(TNMC_ADAPTIVE_FCM.def)
            + " // " + getParameterInfo(TNMC_ADAPTIVE_FCM)));
        // adaptive strength not accessible from outside
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(TNMC_SMOOTH_HISTOGRAM_METHOD.name,
            toString(TNMC_SMOOTH_HISTOGRAM_METHOD.def)
            + " // " + getParameterInfo(TNMC_SMOOTH_HISTOGRAM_METHOD)));
        parameters.addParameter(Parameter(TNMC_ENABLE_EQUALIZATION.name,
            toString(TNMC_ENABLE_EQUALIZATION.def)
            + " // " + getParameterInfo(TNMC_ENABLE_EQUALIZATION)));
        parameters.addParameter(Parameter(TNMC_ENABLE_GAMMA.name,
            toString(TNMC_ENABLE_GAMMA.def)
            + " // " + getParameterInfo(TNMC_ENABLE_GAMMA)));
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXDARKSUPRESS.name,
            toString(TNMC_EQUAL_MAXDARKSUPRESS.def)
            + " // " + getParameterInfo(TNMC_EQUAL_MAXDARKSUPRESS)));
        parameters.addParameter(Parameter(TNMC_EQUAL_MAXBRIGHTSUPRESS.name,
            toString(TNMC_EQUAL_MAXBRIGHTSUPRESS.def)
            + " // " + getParameterInfo(TNMC_EQUAL_MAXBRIGHTSUPRESS)));
        parameters.addParameter(Parameter(TNMC_EQUAL_DARKSUPRESSRATIO.name,
            toString(TNMC_EQUAL_DARKSUPRESSRATIO.def)
            + " // " + getParameterInfo(TNMC_EQUAL_DARKSUPRESSRATIO)));
        parameters.addParameter(Parameter(TNMC_EQUAL_BRIGHTSUPRESSRATIO.name,
            toString(TNMC_EQUAL_BRIGHTSUPRESSRATIO.def)
            + " // " + getParameterInfo(TNMC_EQUAL_BRIGHTSUPRESSRATIO)));
        parameters.addParameter(Parameter(TNMC_EQUAL_FACTOR.name,
            toString(TNMC_EQUAL_FACTOR.def)
            + " // " + getParameterInfo(TNMC_EQUAL_FACTOR)));
        parameters.addParameter(Parameter(TNMC_OVERSHOT_THRESHOLD.name,
            toString(TNMC_OVERSHOT_THRESHOLD.def)
            + " // " + getParameterInfo(TNMC_OVERSHOT_THRESHOLD)));
        parameters.addParameter(Parameter(TNMC_WDR_CEILING.name,
            toString(TNMC_WDR_CEILING.def)
            + " // " + getParameterInfo(TNMC_WDR_CEILING)));
        parameters.addParameter(Parameter(TNMC_WDR_FLOOR.name,
            toString(TNMC_WDR_FLOOR.def)
            + " // " + getParameterInfo(TNMC_WDR_FLOOR)));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_UPDATE_DAMP.name,
            toString(TNMC_MAPCURVE_UPDATE_DAMP.def)
            + " // " + getParameterInfo(TNMC_MAPCURVE_UPDATE_DAMP)));
        parameters.addParameter(Parameter(TNMC_MAPCURVE_POWER_VALUE.name,
            toString(TNMC_MAPCURVE_POWER_VALUE.def)
            + " // " + getParameterInfo(TNMC_MAPCURVE_POWER_VALUE)));
        parameters.addParameter(Parameter(TNMC_GAMMA_FCM.name,
            toString(TNMC_GAMMA_FCM.def)
            + " // " + getParameterInfo(TNMC_GAMMA_FCM)));
        parameters.addParameter(Parameter(TNMC_GAMMA_ACM.name,
            toString(TNMC_GAMMA_ACM.def)
            + " // " + getParameterInfo(TNMC_GAMMA_ACM)));
        parameters.addParameter(Parameter(TNMC_GAMMA_CRV_MODE.name,
            toString(TNMC_GAMMA_CRV_MODE.def)
            + " // " + getParameterInfo(TNMC_GAMMA_CRV_MODE)));
        parameters.addParameter(Parameter(TNMC_BEZIER_CTRL_PNT.name,
            toString(TNMC_BEZIER_CTRL_PNT.def)
            + " // " + getParameterInfo(TNMC_BEZIER_CTRL_PNT)));
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
        parameters.addParameter(Parameter(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.name,
            toString(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE.def)
            + " // " + getParameterInfo(TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE)));
        parameters.addParameter(Parameter(TNMC_CRV_BRIGNTNESS.name,
            toString(TNMC_CRV_BRIGNTNESS.def)
            + " // " + getParameterInfo(TNMC_CRV_BRIGNTNESS)));
        parameters.addParameter(Parameter(TNMC_CRV_CONTRAST.name,
            toString(TNMC_CRV_CONTRAST.def)
            + " // " + getParameterInfo(TNMC_CRV_CONTRAST)));
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP
        break;
    }
    return IMG_SUCCESS;
}

//update configuration based on a previous shot metadata
#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlTNM::update(const Metadata &metadata, const Metadata &metadata2)
#else
IMG_RESULT ISPC::ControlTNM::update(const Metadata &metadata)
#endif //INFOTM_DUAL_SENSOR
{
    // Generate local tone mapping curve
    if (adaptiveTNM)
    {
        double sensorGain = 0;
        const Sensor *sensor = getSensor();
        if (!sensor)
        {
            MOD_LOG_ERROR("ControlTNM has no sensor!\n");
            return IMG_ERROR_NOT_INITIALISED;
        }
        sensorGain = ispc_max(1.0, sensor->getGain());
        adaptiveStrength = 1.0 / sensorGain;

#if VERBOSE_CONTROL_MODULES
        MOD_LOG_INFO("TNM adaptive strength: %f\n", adaptiveStrength);
#endif
    }
    else
    {
        adaptiveStrength = 1.0;
    }

#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("compute TNM curve\n");
#endif
    loadHistogram(metadata, histogram);

	//printf("histMin = %f, histMax = %f\n", this->histMin, this->histMax);
    generateMappingCurve(histogram, this->histMin, this->histMax,
        this->smoothing, this->tempering, this->updateSpeed,
        this->mappingCurve);

    // configureStatistics();

    // program the pipeline
    programCorrection();


    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
double ISPC::ControlTNM::getAdaptiveStrength(void) const
{

    return adaptiveStrength;
}

#endif //INFOTM_ISP
IMG_RESULT ISPC::ControlTNM::setHistMin(double value)
{
    if (value<TNMC_HISTMIN.min || value > TNMC_HISTMIN.max)
    {
        MOD_LOG_ERROR("Programmed value (%f) must be between %f and %f\n",
            value, TNMC_HISTMIN.min, TNMC_HISTMIN.max);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    histMin = value;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getHistMin() const
{
    return histMin;
}

IMG_RESULT ISPC::ControlTNM::setHistMax(double value)
{
    if (value<TNMC_HISTMAX.min || value > TNMC_HISTMAX.max)
    {
        MOD_LOG_ERROR("Programmed value (%f) must be between %f and %f\n",
            value, TNMC_HISTMAX.min, TNMC_HISTMAX.max);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    histMax = value;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getHistMax() const
{
    return histMax;
}

IMG_RESULT ISPC::ControlTNM::setTempering(double value)
{
    if (value<TNMC_TEMPERING.min || value > TNMC_TEMPERING.max)
    {
        MOD_LOG_ERROR("Programmed value (%f) must be between %f and %f\n",
            value, TNMC_TEMPERING.min, TNMC_TEMPERING.max);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    tempering = value;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getTempering() const
{
    return tempering;
}

IMG_RESULT ISPC::ControlTNM::setSmoothing(double value)
{
    if (value<TNMC_SMOOTHING.min || value >TNMC_SMOOTHING.max)
    {
        MOD_LOG_ERROR("Programmed value (%f) must be between %f and %f\n",
            value, TNMC_SMOOTHING.min, TNMC_SMOOTHING.max);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    smoothing = value;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getSmoothing() const
{
    return smoothing;
}

IMG_RESULT ISPC::ControlTNM::setUpdateSpeed(double value)
{
    if (value<TNMC_UPDATESPEED.min || value >TNMC_UPDATESPEED.max)
    {
        MOD_LOG_ERROR("Programmed value (%f) must be between %f and %f\n",
            value, TNMC_UPDATESPEED.min, TNMC_UPDATESPEED.max);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    updateSpeed = value;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getUpdateSpeed() const
{
    return updateSpeed;
}

#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlTNM::getHistogram(double *padBuf)
{

    if (NULL == padBuf) {
        MOD_LOG_ERROR("Parameter padBuff is NULL.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    for (int iIdx = 0; iIdx < TNMC_N_HIST; iIdx++) {
        padBuf[iIdx] = histogram[0][iIdx];
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::getMappingCurve(double *padBuf)
{

    if (NULL == padBuf) {
        MOD_LOG_ERROR("Parameter padBuff is NULL.\n");
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    for (int iIdx = 0; iIdx < TNMC_N_CURVE; iIdx++) {
        padBuf[iIdx] = mappingCurve[0][iIdx];
    }

    return IMG_SUCCESS;
}

#endif //INFOTM_ISP
IMG_RESULT ISPC::ControlTNM::setLocalStrength(double value)
{
    if (value<TNMC_LOCAL_STRENGTH.min || value >TNMC_LOCAL_STRENGTH.max)
    {
        MOD_LOG_ERROR("Programmed value (%f) must be between %f and %f\n",
            value, TNMC_LOCAL_STRENGTH.min, TNMC_LOCAL_STRENGTH.max);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    localStrength = value;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getLocalStrength() const
{
    return localStrength;
}

void ISPC::ControlTNM::enableLocalTNM(bool t)
{
    localTNM = t;
}

bool ISPC::ControlTNM::getLocalTNM() const
{
    return localTNM;
}

void ISPC::ControlTNM::enableAdaptiveTNM(bool t)
{
    adaptiveTNM = t;
}

bool ISPC::ControlTNM::getAdaptiveTNM() const
{
    return adaptiveTNM;
}

#ifdef INFOTM_ISP
void ISPC::ControlTNM::enableAdaptiveTNM_FCM(bool enable)
{
    adaptiveTNM_FCM = enable;
}

bool ISPC::ControlTNM::getAdaptiveTNM_FCM() const
{
    return adaptiveTNM_FCM;
}

#endif //INFOTM_ISP
void ISPC::ControlTNM::setAllowHISConfig(bool enable)
{
    configureHis = enable;
}

bool ISPC::ControlTNM::getAllowHISConfig() const
{
    return configureHis;
}

//Auxiliar histogram manipulation and tnm generation functions
IMG_RESULT ISPC::ControlTNM::resetHistogram(Matrix &hist)
{
    if (hist.numRows() != 1 || hist.numCols() != TNMC_N_HIST)
    {
        LOG_ERROR("Expecting matrix with 1x%d elements\n", TNMC_N_HIST);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    for (int i = 0; i < TNMC_N_HIST; i++)
    {
        hist[0][i] = 1.0 / (TNMC_N_HIST);
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::resetCurve(Matrix &curve)
{
    if (curve.numRows() != 1 || curve.numCols() != TNMC_N_CURVE)
    {
        LOG_ERROR("Expecting matrix with 1x%d elements\n", TNMC_N_CURVE);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    for (int i = 0; i < TNMC_N_CURVE; i++)
    {
        // was 64 for a 65 point curve
        curve[0][i] = static_cast<double>(i) / (TNMC_N_CURVE - 1);
    }

    return IMG_SUCCESS;
}

//histogram smoothing with a 7-sized kernel with 0.05 0.1 0.2 0.3 0.2 0.1 0.05 weights
//We are assuming a 1x64 elements matrix
IMG_RESULT ISPC::ControlTNM::smoothHistogram(Matrix &hist)
{
    if (hist.numRows() != 1 || hist.numCols() != TNMC_N_HIST)
    {
        LOG_ERROR("Expecting matrix with 1x%d elements\n", TNMC_N_HIST);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    Matrix originalHist = hist;

#if defined(INFOTM_ISP)
    switch (ui32SmoothHistogramMethod)
    {
    	case TNMMHD_SH_SHUOYING:
    		{
				#define KERNEL_SIZE 3
				double dmax = 0.02;
				double dover = 0;

				for (int i=0; i<64; i++)
				{
					if (originalHist[0][i] > dmax)
					{
						dover += originalHist[0][i] - dmax;
						originalHist[0][i] = dmax;
					}
				}

				dover /= 64;
				for (int i=0; i<64; i++)
					originalHist[0][i] += dover;

				#if 0
				originalHist[0][0] = 0;
				originalHist[0][63] = 0;
				#endif

				for (int i=0; i<64; i++)
				{
					double dsum = 0;

					for (int j=i-KERNEL_SIZE; j<i+KERNEL_SIZE; j++)
					{
						if (j < 0)
							dsum += originalHist[0][0];
						else if (j > 63)
							dsum += originalHist[0][63];
						else
							dsum += originalHist[0][j];
					}
					hist[0][i] = dsum / (KERNEL_SIZE * 2.0 + 1.0);
				}
    		}
    		break;

    	case TNMMHD_SH_DEFAULT:
    	default:
			{
			    hist[0][0] = originalHist[0][0] * 0.65
			        + originalHist[0][1] * 0.20
			        + originalHist[0][2] * 0.10
			        + originalHist[0][3] * 0.05;
			    hist[0][1] = originalHist[0][0] * 0.35
			        + originalHist[0][1] * 0.30
			        + originalHist[0][2] * 0.20
			        + originalHist[0][3] * 0.10
			        + originalHist[0][4] * 0.05;
			    hist[0][2] = originalHist[0][0] * 0.15
			        + originalHist[0][1] * 0.20
			        + originalHist[0][2] * 0.30
			        + originalHist[0][3] * 0.20
			        + originalHist[0][4] * 0.10
			        + originalHist[0][4] * 0.05;
			    for (int i = 3; i < 61; i++)
			    {
			        hist[0][i] = originalHist[0][i - 3] * 0.05
			            + originalHist[0][i - 2] * 0.10
			            + originalHist[0][i - 1] * 0.20
			            + originalHist[0][i] * 0.30
			            + originalHist[0][i + 1] * 0.20
			            + originalHist[0][i + 2] * 0.10
			            + originalHist[0][i + 3] * 0.05;
			    }
			    hist[0][61] = originalHist[0][58] * 0.05
			        + originalHist[0][59] * 0.10
			        + originalHist[0][60] * 0.20
			        + originalHist[0][61] * 0.30
			        + originalHist[0][62] * 0.20
			        + originalHist[0][63] * 0.15;
			    hist[0][62] = originalHist[0][59] * 0.05
			        + originalHist[0][60] * 0.10
			        + originalHist[0][61] * 0.20
			        + originalHist[0][62] * 0.30
			        + originalHist[0][63] * 0.35;
			    hist[0][63] = originalHist[0][60] * 0.05
			        + originalHist[0][61] * 0.10
			        + originalHist[0][62] * 0.20
			        + originalHist[0][63] * 0.65;
			}
    		break;
    } //end of switch (ui32SmoothHistogramMethod)
#else  //INFOTM_ISP
    hist[0][0] = originalHist[0][0] * 0.65
        + originalHist[0][1] * 0.20
        + originalHist[0][2] * 0.10
        + originalHist[0][3] * 0.05;
    hist[0][1] = originalHist[0][0] * 0.35
        + originalHist[0][1] * 0.30
        + originalHist[0][2] * 0.20
        + originalHist[0][3] * 0.10
        + originalHist[0][4] * 0.05;
    hist[0][2] = originalHist[0][0] * 0.15
        + originalHist[0][1] * 0.20
        + originalHist[0][2] * 0.30
        + originalHist[0][3] * 0.20
        + originalHist[0][4] * 0.10
        + originalHist[0][4] * 0.05;
    for (int i = 3; i < 61; i++)
    {
        hist[0][i] = originalHist[0][i - 3] * 0.05
            + originalHist[0][i - 2] * 0.10
            + originalHist[0][i - 1] * 0.20
            + originalHist[0][i] * 0.30
            + originalHist[0][i + 1] * 0.20
            + originalHist[0][i + 2] * 0.10
            + originalHist[0][i + 3] * 0.05;
    }
    hist[0][61] = originalHist[0][58] * 0.05
        + originalHist[0][59] * 0.10
        + originalHist[0][60] * 0.20
        + originalHist[0][61] * 0.30
        + originalHist[0][62] * 0.20
        + originalHist[0][63] * 0.15;
    hist[0][62] = originalHist[0][59] * 0.05
        + originalHist[0][60] * 0.10
        + originalHist[0][61] * 0.20
        + originalHist[0][62] * 0.30
        + originalHist[0][63] * 0.35;
    hist[0][63] = originalHist[0][60] * 0.05
        + originalHist[0][61] * 0.10
        + originalHist[0][62] * 0.20
        + originalHist[0][63] * 0.65;
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::smoothHistogram(Matrix &hist, double amount)
{
    if (hist.numRows() != 1 || hist.numCols() != TNMC_N_HIST)
    {
        LOG_ERROR("Expecting matrix with 1x%d elements\n", TNMC_N_HIST);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

#if defined(INFOTM_ISP)
    if (bEnableEqualization)
    {
		double maxDarkSupress = fEqualMaxDarkSupress;
		double maxBrightSupress = fEqualMaxBrightSupress;
		double darkSupressRatio = fEqualDarkSupressRatio;
		double brightSupressRatio = fEqualBrightSupressRatio;
		double equalFactor = fEqualFactor;

		#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
		double tnmGamma = fGammaACM;
		#else
		double tnmGamma = TNMGAMMA;
		#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

		double accum;
		int maxDarkIdx;
		int minBrightIdx;
		int darkIdx;
		int brightIdx;
		int i;

		#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
		if (FlatModeStatusGet())
		{
			tnmGamma = fGammaFCM;
		}
		#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

		maxDarkIdx = (int)(maxDarkSupress * (double)TNMC_N_HIST);
		minBrightIdx = TNMC_N_HIST - (int)(maxBrightSupress * (double)TNMC_N_HIST) - 1;

		// Normalize hist so sum = 1.0
		hist = hist.normaliseSum();
	
		#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
		int j;
		static int prtCnt = 0;
	
		if (30 <= ++prtCnt)
		{
			prtCnt = 0;
			printf("=== smoothHistogram : histogram value ===\n");
			for (i=0, j=0; i<TNMC_N_HIST; i++)
			{
				//printf("%lf ", hist[0][i]);
				printf("%03d ", (int)(hist[0][i]*1000));
				if (8 == ++j)
				{
					j = 0;
					printf("\n");
				}
			}
		}
		#endif //INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG

		// Scan for index of dark pixels reach defined ratio
		accum = 0;

		for (i = 0; i < TNMC_N_HIST; i++)
		{
			accum += hist[0][i];
			if (accum >= darkSupressRatio)
				break;
		}

		darkIdx = i;

		#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
		if (0 == prtCnt)
			printf("darkIdx = %d\n", darkIdx);
		#endif //INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG

		// Scan for index of bright pixels reach defined ratio
		accum = 0;

		for (i = TNMC_N_HIST - 1; i >= 0; i--)
		{
			accum += hist[0][i];
			if (accum >= brightSupressRatio)
				break;
		}

		brightIdx = i;
	
		#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
		if (0 == prtCnt)
			printf("brightIdx = %d\n", brightIdx);
		#endif //INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG

		if (darkIdx > maxDarkIdx)
			darkIdx = maxDarkIdx;
	
		if (brightIdx < minBrightIdx)
			brightIdx = minBrightIdx;
	
		#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
		if (0 == prtCnt)
			printf("last darkIdx = %d, last rightIdx = %d\n", darkIdx, brightIdx);
		#endif //INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG
	
		if (bEnableGamma)
		{
			// Remove histogram lower bond.
			for (i = 0; i <= darkIdx; i++)
				hist[0][i] = 0.0;

			// Remove histogram upper bond.
			for (i = brightIdx; i < TNMC_N_HIST; i++)
				hist[0][i] = 1.0;

			switch (ui32GammaCrvMode) {
				case GMA_CRV_MODE_GAMMA:
					{
						for (i = darkIdx; i < brightIdx; i++) {
							#ifdef USE_MATH_NEON
							hist[0][i] = (double)powf_neon((float)(i - darkIdx) / (float)(brightIdx - darkIdx), (float)tnmGamma);
							#else
							hist[0][i] = pow((double)(i - darkIdx) / (double)(brightIdx - darkIdx), tnmGamma);
							#endif
						}
	#if 0
						if (0 < pp) {
							printf("=== newhHistogram : histogram value ===\n");
							for (i=0, j=0; i<TNMC_N_HIST; i++) {
								//printf("%lf ", hist[0][i]);
								printf("%d ", (int)(hist[0][i]*100));
								if (8 == ++j) {
									j = 0;
									printf("\n");
								}
							}
						}
	#endif
					}
					break;

				case GMA_CRV_MODE_BEZIER_LINEAR_QUADRATIC:
					{
						int Idx1;
						double P0;
						double P1;
						double P2;

						Idx1 = ((brightIdx - darkIdx) / 2) + darkIdx;
						for (i = darkIdx; i < Idx1;i++)
							hist[0][i] = (double)(i - darkIdx) / (double)(Idx1 - darkIdx) / 2;

						// Bezier curve
						//{B}(t) = (1 - t)^2*P0 + 2t*(1 - t)*P1 + t^2*P2
						P0 = 0.5;
						//P1 = 0.83;
						P1 = fBezierCtrlPnt;
						P2 = 1.0;

						for (i = Idx1; i < brightIdx; i++)
						{
							double t = (double)(i - Idx1) / (double)(brightIdx - Idx1);
							hist[0][i] = (1-t)*(1-t)*P0 + 2*t*(1-t)*P1 + t*t*P2;
						}

						//printf("brightIdx=%d\n", brightIdx);
						//printf("darkIdx=%d\n", darkIdx);
						//printf("Idx1=%d\n", Idx1);
						//for (i = 0; i < 64; i++)
						//	printf("hist[0][%d]=%f\n", i, hist[0][i]);
					}
					break;

				case GMA_CRV_MODE_BEZIER_QUADRATIC:
					{
						double P0 = 0.0;
						double P1;
						double P2 = 1.0;
						double t;

						//P1 = 0.7;
						P1 = fBezierCtrlPnt;

						for (i = darkIdx; i < brightIdx; i++) {
							t = (double)(i - darkIdx) / (double)(brightIdx - darkIdx);
							hist[0][i] = (1-t)*(1-t)*P0 + 2*t*(1-t)*P1 + t*t*P2;
						}
					}
					break;

				default:
					for(i = darkIdx; i < brightIdx;i++)
						hist[0][i] = pow((double)(i - darkIdx) / (double)(brightIdx - darkIdx), tnmGamma);
					break;

			}
		}
		else //else of if (bEnableGamma)
		{
	#if 0
			// Setup equalization strength.
			hist.applyMax(1.0 / TNMC_N_HIST);
			hist.applyMin((1.0 / TNMC_N_HIST) * (1.0 + equalFactor));

			// Remove histogram lower bond.
			for (i = 0; i <= darkIdx; i++)
				hist[0][i] = 0.0;

			// Remove histogram upper bond.
			for (i = brightIdx; i < TNMC_N_HIST; i++)
				hist[0][i] = 0.0;

			Matrix originalHist = hist;
			
			hist[0][darkIdx + 1] = originalHist[0][darkIdx + 1]*0.65 + originalHist[0][darkIdx + 2]*0.20 + originalHist[0][darkIdx + 3]*0.10 + originalHist[0][darkIdx + 4]*0.05;
			hist[0][darkIdx + 2] = originalHist[0][darkIdx + 1]*0.35 + originalHist[0][darkIdx + 2]*0.30 + originalHist[0][darkIdx + 3]*0.20 + originalHist[0][darkIdx + 4]*0.10 + originalHist[0][darkIdx + 5]*0.05;
			hist[0][darkIdx + 3] = originalHist[0][darkIdx + 1]*0.15 + originalHist[0][darkIdx + 2]*0.20 + originalHist[0][darkIdx + 3]*0.30 + originalHist[0][darkIdx + 4]*0.20 + originalHist[0][darkIdx + 5]*0.10 + originalHist[0][darkIdx + 6]*0.05;
			for(i = darkIdx + 4; i < brightIdx - 3;i++)
			{
				hist[0][i] = originalHist[0][i-3]*0.05 + originalHist[0][i-2]*0.10 + originalHist[0][i-1]*0.20 + originalHist[0][i]*0.30 + originalHist[0][i+1]*0.20 + originalHist[0][i+2]*0.10+ originalHist[0][i+3]*0.05;
			}
			hist[0][brightIdx - 3] = originalHist[0][brightIdx - 6]*0.05+originalHist[0][brightIdx - 5]*0.10+originalHist[0][brightIdx - 4]*0.20+originalHist[0][brightIdx - 3]*0.30+originalHist[0][brightIdx - 2]*0.20+originalHist[0][brightIdx - 1]*0.15;
			hist[0][brightIdx - 2] = originalHist[0][brightIdx - 5]*0.05+originalHist[0][brightIdx - 4]*0.10+originalHist[0][brightIdx - 3]*0.20+originalHist[0][brightIdx - 2]*0.30+originalHist[0][brightIdx - 1]*0.35;
			hist[0][brightIdx - 1] = originalHist[0][brightIdx - 4]*0.05+originalHist[0][brightIdx - 3]*0.10+originalHist[0][brightIdx - 2]*0.20+originalHist[0][brightIdx - 1]*0.65;
	#else
			// Remove histogram lower bond.
			for (i = 0; i <= darkIdx; i++)
				hist[0][i] = 0.0;

			// Remove histogram upper bond.
			for (i = brightIdx; i < TNMC_N_HIST; i++)
				hist[0][i] = 0.0;

//			double dmean = hist.sum() / (brightIdx - darkIdx - 1);
			double dmean = hist.sum() / (brightIdx - darkIdx + 1);
			double segMin;
			double segMax;

			// Clip overshot.
			hist.applyMin(dmean * fOvershotThreshold);

			// Shape enhancement curve
//			dmean = hist.sum() / (brightIdx - darkIdx - 1);
			dmean = hist.sum() / (brightIdx - darkIdx + 1);
//			hist.applyMin((dmean) * glbwdrCeiling);
//			hist.applyMax((dmean) * glbwdrFloor);

			for (int ii = 0; ii < TNMC_WDR_SEGMENT_CNT; ii++) {
				int ss = ((brightIdx - darkIdx) * ii / TNMC_WDR_SEGMENT_CNT) + darkIdx;
				int ee = ((brightIdx - darkIdx) * (ii + 1) / TNMC_WDR_SEGMENT_CNT) + darkIdx;
				segMin = fWdrCeiling[ii] * dmean;
				segMax = fWdrFloor[ii] * dmean;

				for (int jj = ss; jj < ee; jj++) {
					hist[0][jj] = std::min(hist[0][jj], segMin);
					hist[0][jj] = std::max(hist[0][jj], segMax);
				}
			}

	#if 0
            segMin = fWdrCeiling[0] * dmean;
            segMax = fWdrFloor[0] * dmean;

            for (i = 0; i <= darkIdx; i++) {
                hist[0][i] = std::min(hist[0][i], segMin);
                hist[0][i] = std::max(hist[0][i], segMax);
            }

            segMin = fWdrCeiling[TNMC_WDR_SEGMENT_CNT - 1] * dmean;
            segMax = fWdrFloor[TNMC_WDR_SEGMENT_CNT - 1] * dmean;

            for (i = brightIdx; i < TNMC_N_HIST; i++) {
                hist[0][i] = std::min(hist[0][i], segMin);
                hist[0][i] = std::max(hist[0][i], segMax);
            }

            // Remove histogram lower bond.
            for (i = 0; i <= darkIdx; i++)
                hist[0][i] = 0.0;

            // Remove histogram upper bond.
            for (i = brightIdx; i < TNMC_N_HIST; i++)
                hist[0][i] = 0.0;

	#endif //#if 0
			Matrix originalHist = hist;

/*
			// Low pass filter
			hist[0][darkIdx + 1] = originalHist[0][darkIdx + 1]*0.65 + originalHist[0][darkIdx + 2]*0.20 + originalHist[0][darkIdx + 3]*0.10 + originalHist[0][darkIdx + 4]*0.05;
			hist[0][darkIdx + 2] = originalHist[0][darkIdx + 1]*0.35 + originalHist[0][darkIdx + 2]*0.30 + originalHist[0][darkIdx + 3]*0.20 + originalHist[0][darkIdx + 4]*0.10 + originalHist[0][darkIdx + 5]*0.05;
			hist[0][darkIdx + 3] = originalHist[0][darkIdx + 1]*0.15 + originalHist[0][darkIdx + 2]*0.20 + originalHist[0][darkIdx + 3]*0.30 + originalHist[0][darkIdx + 4]*0.20 + originalHist[0][darkIdx + 5]*0.10 + originalHist[0][darkIdx + 6]*0.05;
			for(i = darkIdx + 4; i < brightIdx - 3;i++)
			{
				hist[0][i] = originalHist[0][i-3]*0.05 + originalHist[0][i-2]*0.10 + originalHist[0][i-1]*0.20 + originalHist[0][i]*0.30 + originalHist[0][i+1]*0.20 + originalHist[0][i+2]*0.10+ originalHist[0][i+3]*0.05;
			}
			hist[0][brightIdx - 3] = originalHist[0][brightIdx - 6]*0.05+originalHist[0][brightIdx - 5]*0.10+originalHist[0][brightIdx - 4]*0.20+originalHist[0][brightIdx - 3]*0.30+originalHist[0][brightIdx - 2]*0.20+originalHist[0][brightIdx - 1]*0.15;
			hist[0][brightIdx - 2] = originalHist[0][brightIdx - 5]*0.05+originalHist[0][brightIdx - 4]*0.10+originalHist[0][brightIdx - 3]*0.20+originalHist[0][brightIdx - 2]*0.30+originalHist[0][brightIdx - 1]*0.35;
			hist[0][brightIdx - 1] = originalHist[0][brightIdx - 4]*0.05+originalHist[0][brightIdx - 3]*0.10+originalHist[0][brightIdx - 2]*0.20+originalHist[0][brightIdx - 1]*0.65;

*/
			hist[0][0] = originalHist[0][0] * 0.65
				+ originalHist[0][1] * 0.20
				+ originalHist[0][2] * 0.10
				+ originalHist[0][3] * 0.05;
			hist[0][1] = originalHist[0][0] * 0.35
				+ originalHist[0][1] * 0.30
				+ originalHist[0][2] * 0.20
				+ originalHist[0][3] * 0.10
				+ originalHist[0][4] * 0.05;
			hist[0][2] = originalHist[0][0] * 0.15
				+ originalHist[0][1] * 0.20
				+ originalHist[0][2] * 0.30
				+ originalHist[0][3] * 0.20
				+ originalHist[0][4] * 0.10
				+ originalHist[0][5] * 0.05;
			for (int i = 3; i < 61; i++) {
				hist[0][i] = originalHist[0][i - 3] * 0.05
					+ originalHist[0][i - 2] * 0.10
					+ originalHist[0][i - 1] * 0.20
					+ originalHist[0][i] * 0.30
					+ originalHist[0][i + 1] * 0.20
					+ originalHist[0][i + 2] * 0.10
					+ originalHist[0][i + 3] * 0.05;
			}
			hist[0][61] = originalHist[0][58] * 0.05
				+ originalHist[0][59] * 0.10
				+ originalHist[0][60] * 0.20
				+ originalHist[0][61] * 0.30
				+ originalHist[0][62] * 0.20
				+ originalHist[0][63] * 0.15;
			hist[0][62] = originalHist[0][59] * 0.05
				+ originalHist[0][60] * 0.10
				+ originalHist[0][61] * 0.20
				+ originalHist[0][62] * 0.30
				+ originalHist[0][63] * 0.35;
			hist[0][63] = originalHist[0][60] * 0.05
				+ originalHist[0][61] * 0.10
				+ originalHist[0][62] * 0.20
				+ originalHist[0][63] * 0.65;

		#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
			static int pCnt = 0;

			if (30 <= ++pCnt) {
				pCnt = 0;
				printf("=== hist ===\n");
				
				for (int ii=0; ii < TNMC_N_HIST; ii++) {
					printf("%03d ", (int)(hist[0][ii]*1000));
				}

				printf("\n");
			}
		#endif
	#endif
		} //end of if (bEnableGamma)
    }
    else //else if (bEnableEqualization)
    {
		if (amount <0.0 || amount >1.0)
		{
			LOG_ERROR("Smoothing amount must be between 0.0 and 1.0. "\
				"Setting to 0.0.\n");
			amount = 0.0;
		}
		Matrix smoothedHistogram = hist;
		smoothHistogram(smoothedHistogram);
	
		hist = hist*(1.0 - amount) + smoothedHistogram*amount;
    } // end of if (bEnableEqualization)
#else //INFOTM_ISP
    if (amount <0.0 || amount >1.0)
    {
        LOG_ERROR("Smoothing amount must be between 0.0 and 1.0. "\
            "Setting to 0.0.\n");
        amount = 0.0;
    }
    Matrix smoothedHistogram = hist;
    smoothHistogram(smoothedHistogram);

    hist = hist*(1.0 - amount) + smoothedHistogram*amount;
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

ISPC::Matrix ISPC::ControlTNM::accumulateHistogram(const Matrix &hist)
{
    Matrix accumulatedHistogram(1, TNMC_N_CURVE);

    if (hist.numRows() != 1 || hist.numCols() != TNMC_N_HIST)
    {
        LOG_ERROR("Expecting matrix with 1x%d elements\n", TNMC_N_HIST);
        return accumulatedHistogram;
    }

    accumulatedHistogram[0][0] = 0.0;
    for (int i = 0; i < TNMC_N_HIST; i++)
    {
        accumulatedHistogram[0][i + 1] =
            accumulatedHistogram[0][i] + hist[0][i];
    }
    accumulatedHistogram[0][TNMC_N_CURVE - 1] = 1.0;

    return accumulatedHistogram;
}

IMG_STATIC_ASSERT(TNMC_N_HIST == HIS_GLOBAL_BINS, HIS_GLOBAL_BINS_CHANGED);

void ISPC::ControlTNM::loadHistogram(const Metadata &metadata, Matrix &hist)
{
    for (int i = 0; i < TNMC_N_HIST; i++)
    {
        hist[0][i] = metadata.histogramStats.globalHistogram[i];
    }
#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
	static int prtCnt = 0;
	int x, j;

	if (30 <= ++prtCnt)
	{
		prtCnt = 0;
		printf("=== loadHistogram : histogram value ===\n");
		for (x=0, j=0; x<TNMC_N_HIST; x++)
		{
			printf("%f ", hist[0][x]);
			if (8 == ++j)
			{
				j = 0;
				printf("\n");
			}
		}
	}
#endif
}

void ISPC::ControlTNM::generateMappingCurve(Matrix &hist, double minHist,
    double maxHist, double smoothing, double tempering, double updateSpeed,
    Matrix &mappingCurve)
{
    double accumulated = hist.sum();

#if defined(INFOTM_ISP)
    if (accumulated != 0)
    {
    	if (bEnableEqualization)
    	{
			double updateDamp = fMapCurveUpdateDamp;
	#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
			printf("updateDamp = %lf\n", updateDamp);
	#endif

			smoothHistogram(hist, smoothing);
			if (bEnableGamma)
			{
				Matrix newTNMCurve(1, TNMC_N_CURVE);

				for (int i=0; i<TNMC_N_HIST; i++)
				{
					newTNMCurve[0][i] = hist[0][i];
				}
				newTNMCurve[0][TNMC_N_CURVE-1] = 1.0;
	#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
                if (bTnmCrvSimBrightnessContrastEnable) {
                    // Add brightness and contrast compensation here.
                    double cont = fContrast;

                    if (cont > 1.0) {
                        if (cont >= 2.0)
                            cont = 2.0 - (1.0 / 1024);
                        cont = 1.0 / (2.0 - cont);
                    }

                    for (int i=0; i<TNMC_N_HIST; i++)
                    {
                        double fVal;

                        fVal = newTNMCurve[0][i];
        #if 0
                        fVal -= 0.5;
                        fVal *= cont;
                        fVal += 0.5;
        #else
                        //fVal -= 0.5;
                        fVal *= cont;
                        //fVal += 0.5;
        #endif
                        fVal += (fBrightness * 2);
                        if (fVal <= 0.0) fVal = 0.0;
                        if (fVal > 1.0) fVal = 1.0;
                        newTNMCurve[0][i] = fVal;
                    }
                }
	#endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
				mappingCurve = mappingCurve * (1.0 - updateDamp) + (newTNMCurve * updateDamp);
			}
			else //else of if (bEnableGamma)
			{
				// Normalize hist so sum = 1.0
				hist = hist.normaliseSum();

	#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
			    int i;
			    int j;
			    static int prtCnt = 0;

				if (30 <= ++prtCnt)
				{
					prtCnt = 0;
					printf("=== generateMappingCurve : histogram value ===\n");
					for (i=0, j=0; i<TNMC_N_HIST; i++)
					{
						printf("%lf ", hist[0][i]);
						if (8 == ++j)
						{
							j = 0;
							printf("\n");
						}
					}
				}
	#endif //INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG

	#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
                if (!bTnmCrvSimBrightnessContrastEnable) {
                    mappingCurve = mappingCurve * (1.0 - updateDamp) + accumulateHistogram(hist) * updateDamp;
                } else {
                    // Add brightness and contrast compensation here.
                    // Use accumulateHistogram to make final curve before applying compensation.
                    Matrix newTNMCurve = accumulateHistogram(hist);
                    double cont = fContrast;

                    if (cont > 1.0) {
                        if (cont >= 2.0)
                            cont = 2.0 - (1.0 / 1024);
                        cont = 1.0 / (2.0 - cont);
                    }

                    for (int i=0; i<TNMC_N_HIST; i++)
                    {
                        double fVal;

                        fVal = newTNMCurve[0][i];
        #if 0
                        fVal -= 0.5;
                        fVal *= cont;
                        fVal += 0.5;
        #else
                        //fVal -= 0.5;
                        fVal *= cont;
                        //fVal += 0.5;
        #endif
                        fVal += (fBrightness * 2);
                        if (fVal <= 0.0) fVal = 0.0;
                        if (fVal > 1.0) fVal = 1.0;
                        newTNMCurve[0][i] = fVal;
                    }

                    mappingCurve = mappingCurve * (1.0 - updateDamp) + (newTNMCurve * updateDamp);
                }
	#else
                mappingCurve = mappingCurve * (1.0 - updateDamp) + accumulateHistogram(hist) * updateDamp;
	#endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)

	#if defined(INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG)
				if (0 == prtCnt)
				{
					printf("=== generateMappingCurve : mapping curve value ===\n");
					for (i=0, j=0; i<TNMC_N_HIST; i++)
					{
						printf("%lf ", mappingCurve[0][i]);
						if (8 == ++j)
						{
							j = 0;
							printf("\n");
						}
					}
				}
	#endif //INFOTM_ENABLE_TNM_EQUALIZATION_DBG_MSG
			} //end of if (bEnableGamma)
    	}
    	else //else of if (bEnableEqualization)
    	{
			// normalise between 0.0 to 1.0 using maximum value
			hist.normaliseMax();
			// Matrix::plotAsHistogram(histogram, std::cout, '^');
			hist.applyMin(maxHist);
			hist.applyMax(minHist);
			smoothHistogram(hist, smoothing);
			hist = hist.normaliseSum();

			Matrix powerHistogram = hist.power(fMapCurvePowerValue);
			double flattening = ispc_min(1.0, powerHistogram.sum()*tempering * 100);
			hist.power(1.0 - flattening);
			hist = hist.normaliseSum();
	#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
            if (!bTnmCrvSimBrightnessContrastEnable) {
                mappingCurve = mappingCurve*(1.0 - updateSpeed) + accumulateHistogram(hist) * updateSpeed;
            } else {
                // Add brightness and contrast compensation here.
                // Use accumulateHistogram to make final curve before applying compensation.
                Matrix newTNMCurve = accumulateHistogram(hist);
                double cont = fContrast;

                if (cont > 1.0) {
                    if (cont >= 2.0)
                        cont = 2.0 - (1.0 / 1024);
                    cont = 1.0 / (2.0 - cont);
                }

                for (int i=0; i<TNMC_N_HIST; i++)
                {
                    double fVal;

                    fVal = newTNMCurve[0][i];
        #if 0
                    fVal -= 0.5;
                    fVal *= cont;
                    fVal += 0.5;
        #else
                    //fVal -= 0.5;
                    fVal *= cont;
                    //fVal += 0.5;
        #endif
                    fVal += (fBrightness * 2);
                    if (fVal <= 0.0) fVal = 0.0;
                    if (fVal > 1.0) fVal = 1.0;
                    newTNMCurve[0][i] = fVal;
                }

                mappingCurve = mappingCurve * (1.0 - updateSpeed) + (newTNMCurve * updateSpeed);
            }
	#else
            mappingCurve = mappingCurve*(1.0 - updateSpeed) + accumulateHistogram(hist) * updateSpeed;
	#endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
    	} //end of if (bEnableEqualization)
    }
    else //else of if (accumulated != 0)
    {
  #ifdef INFOTM_ENABLE_WARNING_DEBUG
        LOG_WARNING("The global histograms are 0, the curve cannot be "\
            "computed it is forced to identity!\n");
  #endif //INFOTM_ENABLE_WARNING_DEBUG
        resetCurve(mappingCurve);
    } //end of if (accumulated != 0)
#else //INFOTM_ISP
    if (accumulated != 0)
    {
        // normalise between 0.0 to 1.0 using maximum value
        hist.normaliseMax();
        // Matrix::plotAsHistogram(histogram, std::cout, '^');
        hist.applyMin(maxHist);
        hist.applyMax(minHist);
        smoothHistogram(hist, smoothing);
        hist = hist.normaliseSum();

        Matrix powerHistogram = hist.power(2.0);
        double flattening =
            ispc_min(1.0, powerHistogram.sum()*tempering * 100);
        hist.power(1.0 - flattening);
        hist = hist.normaliseSum();
        mappingCurve = mappingCurve*(1.0 - updateSpeed)
            + accumulateHistogram(hist)*updateSpeed;
    }
    else
    {
        LOG_WARNING("The global histograms are 0, the curve cannot be "\
            "computed it is forced to identity!\n");
        resetCurve(mappingCurve);
    } //end of if (accumulated != 0)
#endif //INFOTM_ISP
}

IMG_RESULT ISPC::ControlTNM::configureStatistics()
{
    ModuleHIS *pHIS = NULL;

    if (getPipelineOwner())
    {
        pHIS = getPipelineOwner()->getModule<ModuleHIS>();
    }
    else
    {
        MOD_LOG_ERROR("ControlTNM has no pipeline owner! Cannot "\
            "configure statistics.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    // configured = false;

    if (pHIS)
    {
        if (!pHIS->bEnableGlobal)
        {
            if (configureHis)
            {
                /* Configure statistics to make the 7x7 grid cover as much
                 * image as possible */
                pHIS->bEnableGlobal = true;

                pHIS->ui32InputOffset = ModuleHIS::HIS_INPUTOFF.def;
                pHIS->ui32InputScale = ModuleHIS::HIS_INPUTSCALE.def;

                pHIS->requestUpdate();
            }
            else
            {
                /* we cannot change the configuration, but we'll check it
                 * to make sure it's available */
                MOD_LOG_WARNING("Global Histograms in HIS are not enabled! "\
                    "The computed TNM curve will be flat.\n");
            }
        }
        // configured = true;

        return IMG_SUCCESS;
    }
    else
    {
        MOD_LOG_ERROR("ControlTNM cannot find HIS\n");
    }

    return IMG_ERROR_UNEXPECTED_STATE;
}

IMG_RESULT ISPC::ControlTNM::programCorrection()
{
    std::list<Pipeline *>::iterator it;

    for (it = pipelineList.begin(); it != pipelineList.end(); it++)
    {
        ModuleTNM *tnm = (*it)->getModule<ModuleTNM>();

        if (tnm)
        {
#ifdef INFOTM_ISP
            if (!tnm->bStaticCurve)
            {
                if (adaptiveTNM)
                {
                    for (int i = 0; i < TNM_CURVE_NPOINTS; i++)
                    {
                        tnm->aCurve[i] = mappingCurve[0][i + 1]
                            * (static_cast<double>(i + 1) / (TNMC_N_CURVE))
                            * (1.0 - adaptiveStrength);
                    }
                }
                else
                {
                    for (int i = 0; i < TNM_CURVE_NPOINTS; i++)
                    {
                        tnm->aCurve[i] = mappingCurve[0][i + 1];
                    }
                }
            }
#else
            if (adaptiveTNM)
            {
                for (int i = 0; i < TNM_CURVE_NPOINTS; i++)
                {
                    tnm->aCurve[i] = mappingCurve[0][i + 1]
                        * (static_cast<double>(i + 1) / (TNMC_N_CURVE))
                        * (1.0 - adaptiveStrength);
                }
            }
            else
            {
                for (int i = 0; i < TNM_CURVE_NPOINTS; i++)
                {
                    tnm->aCurve[i] = mappingCurve[0][i + 1];
                }
            }
#endif

            if (localTNM)
            {
                tnm->fWeightLocal = localStrength*adaptiveStrength;
#if VERBOSE_CONTROL_MODULES
                MOD_LOG_INFO("compute TNM local weight = %f\n",
                    tnm->fWeightLocal);
#endif
            }
            else
            {
                tnm->fWeightLocal = 0.0;
            }

            tnm->requestUpdate();
        }
        else
        {
#if VERBOSE_CONTROL_MODULES
            MOD_LOG_WARNING("one pipeline is missing a TNM module\n");
#endif
        }
    }

    return IMG_ERROR_UNEXPECTED_STATE;
}

#ifdef INFOTM_ISP
int ISPC::ControlTNM::getSmoothHistogramMethod(void)
{

    return ui32SmoothHistogramMethod;
}

IMG_RESULT ISPC::ControlTNM::setSmoothHistogramMethod(int iSmoothHistogramMethod)
{

    ui32SmoothHistogramMethod = iSmoothHistogramMethod;
    return IMG_SUCCESS;
}

bool ISPC::ControlTNM::isEqualizationEnabled(void)
{

    return bEnableEqualization;
}

IMG_RESULT ISPC::ControlTNM::enableEqualization(bool bEnable)
{

    bEnableEqualization = bEnable;
    return IMG_SUCCESS;
}

bool ISPC::ControlTNM::isGammaEnabled(void)
{

    return bEnableGamma;
}

IMG_RESULT ISPC::ControlTNM::enableGamma(bool bEnable)
{

    bEnableGamma = bEnable;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getGammaFCM(void)
{

    return fGammaFCM;
}

IMG_RESULT ISPC::ControlTNM::setGammaFCM(double gamma)
{

    fGammaFCM = gamma;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getGammaACM(void)
{

    return fGammaACM;
}

IMG_RESULT ISPC::ControlTNM::setGammaACM(double gamma)
{

    fGammaACM = gamma;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getEqualMaxBrightSupress(void)
{

    return fEqualMaxBrightSupress;
}

IMG_RESULT ISPC::ControlTNM::setEqualMaxBrightSupress(double ratio)
{

    fEqualMaxBrightSupress = ratio;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getEqualMaxDarkSupress(void)
{

    return fEqualMaxDarkSupress;
}

IMG_RESULT ISPC::ControlTNM::setEqualMaxDarkSupress(double ratio)
{

    fEqualMaxDarkSupress = ratio;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getEqualBrightSupressRatio(void)
{

    return fEqualBrightSupressRatio;
}

IMG_RESULT ISPC::ControlTNM::setEqualBrightSupressRatio(double ratio)
{

	fEqualBrightSupressRatio = ratio;
	return IMG_SUCCESS;
}

double ISPC::ControlTNM::getEqualDarkSupressRatio(void)
{

    return fEqualDarkSupressRatio;
}

IMG_RESULT ISPC::ControlTNM::setEqualDarkSupressRatio(double ratio)
{

	fEqualDarkSupressRatio = ratio;
	return IMG_SUCCESS;
}

double ISPC::ControlTNM::getEqualFactor(void)
{

    return fEqualFactor;
}

IMG_RESULT ISPC::ControlTNM::setEqualFactor(double fFactor)
{

    fEqualFactor = fFactor;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getOvershotThreshold(void)
{

    return fOvershotThreshold;
}

IMG_RESULT  ISPC::ControlTNM::setOvershotThreshold(double fThreshold)
{

	fOvershotThreshold = fThreshold;
	return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::getWdrCeiling(double * pfWdrCeiling)
{
    int i;

    for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
        *pfWdrCeiling = fWdrCeiling[i];
        pfWdrCeiling++;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::setWdrCeiling(double * pfWdrCeiling)
{
	int i;

	for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
		fWdrCeiling[i] = *pfWdrCeiling;
		pfWdrCeiling++;
	}

	return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::getWdrFloor(double * pfWdrFloor)
{
    int i;

    for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
        *pfWdrFloor = fWdrFloor[i];
        pfWdrFloor++;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlTNM::setWdrFloor(double * pfWdrFloor)
{
	int i;

	for (int i = 0 ; i < TNMC_WDR_SEGMENT_CNT ; i++) {
		fWdrFloor[i] = *pfWdrFloor;
		pfWdrFloor++;
	}

	return IMG_SUCCESS;
}

double ISPC::ControlTNM::getMapCurveUpdateDamp(void)
{

    return fMapCurveUpdateDamp;
}

IMG_RESULT ISPC::ControlTNM::setMapCurveUpdateDamp(double fUpdateDamp)
{

    fMapCurveUpdateDamp = fUpdateDamp;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getMapCurvePowerValue(void)
{

    return fMapCurvePowerValue;
}

IMG_RESULT ISPC::ControlTNM::setMapCurvePowerValue(double fPowerValue)
{

    fMapCurvePowerValue = fPowerValue;
    return IMG_SUCCESS;
}

int ISPC::ControlTNM::getGammaCurveMode(void)
{

    return ui32GammaCrvMode;
}

IMG_RESULT ISPC::ControlTNM::setGammaCurveMode(int Mode)
{

	if (0 > Mode || 1 < Mode)
		return IMG_ERROR_INVALID_PARAMETERS;

	ui32GammaCrvMode = Mode;
	return IMG_SUCCESS;
}

double ISPC::ControlTNM::getBezierCtrlPnt(void)
{

    return fBezierCtrlPnt;
}

IMG_RESULT ISPC::ControlTNM::setBezierCtrlPnt(double point)
{

	if (0.0 > point || 1.0 < point)
		return IMG_ERROR_INVALID_PARAMETERS;

	fBezierCtrlPnt = point;
	return IMG_SUCCESS;
}

bool ISPC::ControlTNM::isTnmCrvSimBrightnessContrastEnabled(void)
{

    return bTnmCrvSimBrightnessContrastEnable;
}

IMG_RESULT ISPC::ControlTNM::enableTnmCrvSimBrightnessContrast(bool bEnable)
{

    bTnmCrvSimBrightnessContrastEnable = bEnable;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getTnmCrvBrightness(void)
{

    return fBrightness;
}

IMG_RESULT ISPC::ControlTNM::setTnmCrvBrightness(double dbBrightness)
{

    fBrightness = dbBrightness;
    return IMG_SUCCESS;
}

double ISPC::ControlTNM::getTnmCrvContrast(void)
{

    return fContrast;
}

IMG_RESULT ISPC::ControlTNM::setTnmCrvContrast(double dbContrast)
{

    fContrast = dbContrast;
    return IMG_SUCCESS;
}

#endif //INFOTM_ISP
