/**
*******************************************************************************
@file ControlAE.cpp

@brief ISPC::ControlAE implementation

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
#include "ispc/ControlAE.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_AE"

#include <cmath>
#include <string>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/Pipeline.h"
#include "ispc/ModuleHIS.h"
#include "ispc/Sensor.h"
#include "ispc/ISPCDefs.h"

#ifdef INFOTM_ISP
#include <ispc/Camera.h>
#endif //INFOTM_ISP

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

#ifdef INFOTM_ISP
typedef enum _AEE_MODE
{
    AE_MODE_HI_LIGHT = 0,
    AE_MODE_NORMAL,
    AE_MODE_DAY,
	AE_MODE_NOON,
    AE_MODE_NIGHT,
    AE_MODE_LOW_LIGHT,
    AE_MODE_MAX
} AEE_MODE;

typedef enum _AEE_TARGET_MODE
{
	AE_TARGET_MODE_NORMAL = 0,
	AE_TARGET_MODE_LOWLUX
} AEE_TARGET_MODE;

typedef enum AEE_MOVE_METHOD
{
	AEMHD_MOVE_DEFAULT = 0,
	AEMHD_MOVE_SHUOYING,
} AEE_MOVE_METHOD;

typedef enum _AEE_BRIGHTNESS_METERING_METHOD
{
	AEMHD_BM_DEFAULT = 0,
	AEMHD_BM_SHUOYING
} AEE_BRIGHTNESS_METERING_METHOD;
#endif //INFOTM_ISP

/**
 * @brief Grid weights with higher relevance towards the centre
 */
const double ISPC::ControlAE::WEIGHT_7X7[7][7] = {
    { 0.0026, 0.0062, 0.0105, 0.0125, 0.0105, 0.0062, 0.0026 },
    { 0.0062, 0.0148, 0.0250, 0.0297, 0.0260, 0.0148, 0.0062 },
    { 0.0105, 0.0250, 0.0421, 0.0500, 0.0421, 0.0250, 0.0105 },
    { 0.0125, 0.0297, 0.0500, 0.0595, 0.0500, 0.0297, 0.0125 },
    { 0.0105, 0.0250, 0.0421, 0.0500, 0.0421, 0.0250, 0.0105 },
    { 0.0062, 0.0148, 0.0250, 0.0297, 0.0260, 0.0148, 0.0062 },
    { 0.0026, 0.0062, 0.0105, 0.0125, 0.0105, 0.0062, 0.0026 } };

/**
 * @brief Grid weights with higher relevance towards the centre but more homogeneously distributed
 */
const double ISPC::ControlAE::WEIGHT_7X7_A[7][7] = {
    { 0.0129, 0.0161, 0.0183, 0.0191, 0.0183, 0.0161, 0.0129 },
    { 0.0161, 0.0200, 0.0227, 0.0237, 0.0227, 0.0200, 0.0161 },
    { 0.0183, 0.0227, 0.0259, 0.0271, 0.0259, 0.0227, 0.0183 },
    { 0.0191, 0.0237, 0.0271, 0.0283, 0.0271, 0.0237, 0.0191 },
    { 0.0183, 0.0227, 0.0259, 0.0271, 0.0259, 0.0227, 0.0183 },
    { 0.0161, 0.0200, 0.0227, 0.0237, 0.0227, 0.0200, 0.0161 },
    { 0.0129, 0.0161, 0.0183, 0.0191, 0.0183, 0.0161, 0.0129 } };

/**
 * @brief Grid weights with much more focused in the centra values
 */
const double ISPC::ControlAE::WEIGHT_7X7_B[7][7] = {
    { 0.0000, 0.0000, 0.0000, 0.0001, 0.0000, 0.0000, 0.0000 },
    { 0.0000, 0.0003, 0.0036, 0.0086, 0.0036, 0.0003, 0.0000 },
    { 0.0000, 0.0036, 0.0487, 0.1160, 0.0487, 0.0036, 0.0000 },
    { 0.0001, 0.0086, 0.1160, 0.2763, 0.1160, 0.0086, 0.0001 },
    { 0.0000, 0.0036, 0.0487, 0.1160, 0.0487, 0.0036, 0.0000 },
    { 0.0000, 0.0003, 0.0036, 0.0086, 0.0036, 0.0003, 0.0000 },
    { 0.0000, 0.0000, 0.0000, 0.0001, 0.0000, 0.0000, 0.0000 } };

/**
 * @brief Defines a background/foregrund mask for backlight detection
 */
const double ISPC::ControlAE::BACKLIGHT_7X7[7][7] = {
    { 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000, 0.0000 },
    { 0.0000, 0.0000, 0.0000, 0.5000, 0.0000, 0.0000, 0.0000 },
    { 0.0000, 0.0000, 0.5000, 1.0000, 0.5000, 0.0000, 0.0000 },
    { 0.0000, 0.2500, 1.0000, 1.0000, 1.0000, 0.2500, 0.0000 },
    { 0.0000, 0.5000, 1.0000, 1.0000, 1.0000, 0.5000, 0.0000 },
    { 0.0000, 0.7500, 1.0000, 1.0000, 1.0000, 0.7500, 0.0000 },
    { 0.2500, 1.0000, 1.0000, 1.0000, 1.0000, 1.0000, 0.2500 } };

const ISPC::ParamDefSingle<bool> ISPC::ControlAE::AE_FLICKER(
    "AE_FLICKER_REJECTION", false);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_FLICKER_FREQ(
    "AE_FLICKER_FREQ", 1.0, 200.0, 50.0);  // in Hz
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_BRIGHTNESS(
    "AE_TARGET_BRIGHTNESS", -1.0, 1.0, 0.0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_UPDATE_SPEED(
    "AE_UPDATE_SPEED", 0.0, 1.0, 1.0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_BRACKET_SIZE(
    "AE_BRACKET_SIZE", 0.0, 1.0, 0.05);

#ifdef INFOTM_ISP
const ISPC::ParamDefSingle<bool> ISPC::ControlAE::AE_ENABLE_MOVE_TARGET(
    "AE_ENABLE_MOVE_TARGET", true);
const ISPC::ParamDef<int> ISPC::ControlAE::AE_TARGET_MOVE_METHOD(
    "AE_TARGET_MOVE_METHOD", 0, 1, 0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_MAX(
    "AE_TARGET_MAX", -1.0, 1.0, -0.10);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_MIN(
    "AE_TARGET_MIN", -1.0, 1.0, -0.20);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_GAIN_NRML_MODE_MAX_LMT(
    "AE_TARGET_GAIN_NRML_MODE_MAX_LMT", 1.0, 10.0, 4.0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGRT_MOVE_STEP(
    "AE_TARGRT_MOVE_STEP", 0.0, 1.0, 0.002);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_MIN_LMT(
    "AE_TARGET_MIN_LMT", -1.0, 1.0, -0.1);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_UP_OVER_THRESHOLD(
    "AE_TARGET_UP_OVER_THRESHOLD", -1.0, 1.0, 0.05);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_UP_UNDER_THRESHOLD(
    "AE_TARGET_UP_UNDER_THRESHOLD", -1.0, 1.0, 0.450);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_DN_OVER_THRESHOLD(
    "AE_TARGET_DN_OVER_THRESHOLD", -1.0, 1.0, 0.1);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_DN_UNDER_THRESHOLD(
    "AE_TARGET_DN_UNDER_THRESHOLD", -1.0, 1.0, 0.900);

const ISPC::ParamDef<int> ISPC::ControlAE::AE_TARGET_EXPOSURE_METHOD(
    "AE_TARGET_EXPOSURE_METHOD", 0, 1, 0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_LOWLUX_GAIN_ENTER(
    "AE_TARGET_LOWLUX_GAIN_ENTER", 0.0, 32.0, 6.0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_LOWLUX_GAIN_EXIT(
    "AE_TARGET_LOWLUX_GAIN_EXIT", 0.0, 32.0, 4.0);
const ISPC::ParamDef<int> ISPC::ControlAE::AE_TARGET_LOWLUX_EXPOSURE_ENTER(
    "AE_TARGET_LOWLUX_EXPOSURE_ENTER", 0, 1000000, (60000 - 500));
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_LOWLUX_FPS(
    "AE_TARGET_LOWLUX_FPS", 0.0, 60.0, 10.0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_NORMAL_FPS(
    "AE_TARGET_NORMAL_FPS", 0.0, 60.0, 15.0);
const ISPC::ParamDefSingle<bool> ISPC::ControlAE::AE_TARGET_MAX_FPS_LOCK_ENABLE(
    "AE_TARGET_MAX_FPS_LOCK_ENABLE", false);

const ISPC::ParamDef<int> ISPC::ControlAE::AE_BRIGHTNESS_METERING_METHOD(
    "AE_BRIGHTNESS_METERING_METHOD", 0, 1, 0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_REGION_DUCE(
    "AE_REGION_DUCE", 0.0, 1.0, 0.65);
const ISPC::ParamDefSingle<bool> ISPC::ControlAE::AE_FORCE_CALL_SENSOR_DRIVER_ENABLE(
    "AE_FORCE_CALL_SENSOR_DRIVER_ENABLE", false);
#endif //INFOTM_ISP

ISPC::ParameterGroup ISPC::ControlAE::getGroup()
{
    ParameterGroup group;

    group.header = "// Auto Exposure parameters";

    group.parameters.insert(AE_FLICKER.name);
    group.parameters.insert(AE_FLICKER_FREQ.name);
    group.parameters.insert(AE_TARGET_BRIGHTNESS.name);
    group.parameters.insert(AE_UPDATE_SPEED.name);
    group.parameters.insert(AE_BRACKET_SIZE.name);

#ifdef INFOTM_ISP
    group.parameters.insert(AE_ENABLE_MOVE_TARGET.name);
    group.parameters.insert(AE_TARGET_MOVE_METHOD.name);
    group.parameters.insert(AE_TARGET_MAX.name);
    group.parameters.insert(AE_TARGET_MIN.name);
    group.parameters.insert(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.name);
    group.parameters.insert(AE_TARGRT_MOVE_STEP.name);
    group.parameters.insert(AE_TARGET_MIN_LMT.name);
    group.parameters.insert(AE_TARGET_UP_OVER_THRESHOLD.name);
    group.parameters.insert(AE_TARGET_UP_UNDER_THRESHOLD.name);
    group.parameters.insert(AE_TARGET_DN_OVER_THRESHOLD.name);
    group.parameters.insert(AE_TARGET_DN_UNDER_THRESHOLD.name);

    group.parameters.insert(AE_TARGET_EXPOSURE_METHOD.name);
    group.parameters.insert(AE_TARGET_LOWLUX_GAIN_ENTER.name);
    group.parameters.insert(AE_TARGET_LOWLUX_GAIN_EXIT.name);
    group.parameters.insert(AE_TARGET_LOWLUX_EXPOSURE_ENTER.name);
    group.parameters.insert(AE_TARGET_LOWLUX_FPS.name);
    group.parameters.insert(AE_TARGET_NORMAL_FPS.name);
    group.parameters.insert(AE_TARGET_MAX_FPS_LOCK_ENABLE.name);

    group.parameters.insert(AE_BRIGHTNESS_METERING_METHOD.name);
    group.parameters.insert(AE_REGION_DUCE.name);
    group.parameters.insert(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.name);
#endif //INFOTM_ISP

    return group;
}

ISPC::ControlAE::ControlAE(const std::string &logTag)
    : ControlModuleBase(logTag),
    previousBrightness(0),
    currentBrightness(0),
    targetBrightness(AE_TARGET_BRIGHTNESS.def),
#ifdef INFOTM_ISP
    OritargetBrightness(AE_TARGET_BRIGHTNESS.def),
	uiAeMode(AE_MODE_DAY),
	uiAeModeNew(AE_MODE_DAY),
	enMoveAETarget(AE_ENABLE_MOVE_TARGET.def),
	ui32AeMoveMethod(AE_TARGET_MOVE_METHOD.def),
    fAeTargetMax(AE_TARGET_MAX.def),
    fAeTargetMin(AE_TARGET_MIN.def),
    fAeTargetGainNormalModeMaxLmt(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.def),
    fAeTargetMoveStep(AE_TARGRT_MOVE_STEP.def),
    fAeTargetMinLmt(AE_TARGET_MIN_LMT.def),
    fAeTargetUpOverThreshold(AE_TARGET_UP_OVER_THRESHOLD.def),
    fAeTargetUpUnderThreshold(AE_TARGET_UP_UNDER_THRESHOLD.def),
    fAeTargetDnOverThreshold(AE_TARGET_DN_OVER_THRESHOLD.def),
    fAeTargetDnUnderThreshold(AE_TARGET_DN_UNDER_THRESHOLD.def),
	ui32AeExposureMethod(AE_TARGET_EXPOSURE_METHOD.def),
	fAeTargetLowluxGainEnter(AE_TARGET_LOWLUX_GAIN_ENTER.def),
	fAeTargetLowluxGainExit(AE_TARGET_LOWLUX_GAIN_EXIT.def),
	ui32AeTargetLowluxExposureEnter(AE_TARGET_LOWLUX_EXPOSURE_ENTER.def),
	fAeTargetLowluxFPS(AE_TARGET_LOWLUX_FPS.def),
	fAeTargetNormalFPS(AE_TARGET_NORMAL_FPS.def),
    bAeTargetMaxFpsLockEnable(AE_TARGET_MAX_FPS_LOCK_ENABLE.def),
	ui32AeBrightnessMeteringMethod(AE_BRIGHTNESS_METERING_METHOD.def),
	fAeRegionDuce(AE_REGION_DUCE.def),
    bAeForceCallSensorDriverEnable(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.def),
    bMaxManualAeGainEnable(false),
    fMaxManualAeGain(1.0),
#endif //INFOTM_ISP
    updateSpeed(AE_UPDATE_SPEED.def),
    flickerRejection(AE_FLICKER.def),
    autoFlickerRejection(false),
    flickerFreq(AE_FLICKER_FREQ.def),
    targetBracket(AE_BRACKET_SIZE.def),
    configured(false),
    fNewGain(1.0),
    uiNewExposure(0),
    doAE(true)
{
#ifdef INFOTM_ISP
    pthread_mutex_init((pthread_mutex_t*)&paramLock, NULL);
#endif
}

IMG_RESULT ISPC::ControlAE::load(const ParameterList &parameters)
{
    this->flickerRejection = parameters.getParameter(ControlAE::AE_FLICKER);
    this->flickerFreq = parameters.getParameter(ControlAE::AE_FLICKER_FREQ);
    this->targetBrightness = parameters.getParameter(ControlAE::AE_TARGET_BRIGHTNESS);
#ifdef INFOTM_ISP
    this->OritargetBrightness = parameters.getParameter(ControlAE::AE_TARGET_BRIGHTNESS);
#endif //INFOTM_ISP
    this->updateSpeed = parameters.getParameter(ControlAE::AE_UPDATE_SPEED);
    this->targetBracket = parameters.getParameter(ControlAE::AE_BRACKET_SIZE);

#ifdef INFOTM_ISP
	this->enMoveAETarget = parameters.getParameter(ControlAE::AE_ENABLE_MOVE_TARGET);
    this->ui32AeMoveMethod = parameters.getParameter(ControlAE::AE_TARGET_MOVE_METHOD);
    this->fAeTargetMax = parameters.getParameter(ControlAE::AE_TARGET_MAX);
    this->fAeTargetMin = parameters.getParameter(ControlAE::AE_TARGET_MIN);
    this->fAeTargetGainNormalModeMaxLmt = parameters.getParameter(ControlAE::AE_TARGET_GAIN_NRML_MODE_MAX_LMT);
    this->fAeTargetMoveStep = parameters.getParameter(ControlAE::AE_TARGRT_MOVE_STEP);
    this->fAeTargetMinLmt = parameters.getParameter(ControlAE::AE_TARGET_MIN_LMT);
    this->fAeTargetUpOverThreshold = parameters.getParameter(ControlAE::AE_TARGET_UP_OVER_THRESHOLD);
    this->fAeTargetUpUnderThreshold = parameters.getParameter(ControlAE::AE_TARGET_UP_UNDER_THRESHOLD);
    this->fAeTargetDnOverThreshold = parameters.getParameter(ControlAE::AE_TARGET_DN_OVER_THRESHOLD);
    this->fAeTargetDnUnderThreshold = parameters.getParameter(ControlAE::AE_TARGET_DN_UNDER_THRESHOLD);

    this->ui32AeExposureMethod = parameters.getParameter(ControlAE::AE_TARGET_EXPOSURE_METHOD);
    this->fAeTargetLowluxGainEnter = parameters.getParameter(ControlAE::AE_TARGET_LOWLUX_GAIN_ENTER);
    this->fAeTargetLowluxGainExit = parameters.getParameter(ControlAE::AE_TARGET_LOWLUX_GAIN_EXIT);
    this->ui32AeTargetLowluxExposureEnter = parameters.getParameter(ControlAE::AE_TARGET_LOWLUX_EXPOSURE_ENTER);
    this->fAeTargetLowluxFPS = parameters.getParameter(ControlAE::AE_TARGET_LOWLUX_FPS);
    this->fAeTargetNormalFPS = parameters.getParameter(ControlAE::AE_TARGET_NORMAL_FPS);
    this->bAeTargetMaxFpsLockEnable = parameters.getParameter(ControlAE::AE_TARGET_MAX_FPS_LOCK_ENABLE);

    this->ui32AeBrightnessMeteringMethod = parameters.getParameter(ControlAE::AE_BRIGHTNESS_METERING_METHOD);
    this->fAeRegionDuce = parameters.getParameter(ControlAE::AE_REGION_DUCE);
    this->bAeForceCallSensorDriverEnable = parameters.getParameter(ControlAE::AE_FORCE_CALL_SENSOR_DRIVER_ENABLE);
#endif //INFOTM_ISP

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAE::save(ParameterList &parameters, SaveType t) const
{
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ControlAE::getGroup();
    }

    parameters.addGroup("ControlAE", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(Parameter(AE_FLICKER.name,
            toString(this->flickerRejection)));
        parameters.addParameter(Parameter(AE_FLICKER_FREQ.name,
            toString(this->flickerFreq)));
        parameters.addParameter(Parameter(AE_TARGET_BRIGHTNESS.name,
            toString(this->targetBrightness)));
        parameters.addParameter(Parameter(AE_UPDATE_SPEED.name,
            toString(this->updateSpeed)));
        parameters.addParameter(Parameter(AE_BRACKET_SIZE.name,
            toString(this->targetBracket)));

#ifdef INFOTM_ISP

		parameters.addParameter(Parameter(AE_ENABLE_MOVE_TARGET.name,
		            toString(this->enMoveAETarget)));
        parameters.addParameter(Parameter(AE_TARGET_MOVE_METHOD.name,
            toString(this->ui32AeMoveMethod)));
        parameters.addParameter(Parameter(AE_TARGET_MAX.name,
            toString(this->fAeTargetMax)));
        parameters.addParameter(Parameter(AE_TARGET_MIN.name,
            toString(this->fAeTargetMin)));
        parameters.addParameter(Parameter(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.name,
            toString(this->fAeTargetGainNormalModeMaxLmt)));
        parameters.addParameter(Parameter(AE_TARGRT_MOVE_STEP.name,
            toString(this->fAeTargetMoveStep)));
        parameters.addParameter(Parameter(AE_TARGET_MIN_LMT.name,
            toString(this->fAeTargetMinLmt)));
        parameters.addParameter(Parameter(AE_TARGET_UP_OVER_THRESHOLD.name,
            toString(this->fAeTargetUpOverThreshold)));
        parameters.addParameter(Parameter(AE_TARGET_UP_UNDER_THRESHOLD.name,
            toString(this->fAeTargetUpUnderThreshold)));
        parameters.addParameter(Parameter(AE_TARGET_DN_OVER_THRESHOLD.name,
            toString(this->fAeTargetDnOverThreshold)));
        parameters.addParameter(Parameter(AE_TARGET_DN_UNDER_THRESHOLD.name,
            toString(this->fAeTargetDnUnderThreshold)));

        parameters.addParameter(Parameter(AE_TARGET_EXPOSURE_METHOD.name,
            toString(this->ui32AeExposureMethod)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_ENTER.name,
            toString(this->fAeTargetLowluxGainEnter)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_EXIT.name,
            toString(this->fAeTargetLowluxGainExit)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_EXPOSURE_ENTER.name,
            toString(this->ui32AeTargetLowluxExposureEnter)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_FPS.name,
            toString(this->fAeTargetLowluxFPS)));
        parameters.addParameter(Parameter(AE_TARGET_NORMAL_FPS.name,
            toString(this->fAeTargetNormalFPS)));
        parameters.addParameter(Parameter(AE_TARGET_MAX_FPS_LOCK_ENABLE.name,
            toString(this->bAeTargetMaxFpsLockEnable)));

        parameters.addParameter(Parameter(AE_BRIGHTNESS_METERING_METHOD.name,
            toString(this->ui32AeBrightnessMeteringMethod)));
        parameters.addParameter(Parameter(AE_REGION_DUCE.name,
            toString(this->fAeRegionDuce)));
        parameters.addParameter(Parameter(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.name,
            toString(this->bAeForceCallSensorDriverEnable)));
#endif //INFOTM_ISP
        break;

    case SAVE_MIN:
        parameters.addParameter(Parameter(AE_FLICKER.name,
            toString(AE_FLICKER.def)));  // bool does not have min
        parameters.addParameter(Parameter(AE_FLICKER_FREQ.name,
            toString(AE_FLICKER_FREQ.min)));
        parameters.addParameter(Parameter(AE_TARGET_BRIGHTNESS.name,
            toString(AE_TARGET_BRIGHTNESS.min)));
        parameters.addParameter(Parameter(AE_UPDATE_SPEED.name,
            toString(AE_UPDATE_SPEED.min)));
        parameters.addParameter(Parameter(AE_BRACKET_SIZE.name,
            toString(AE_BRACKET_SIZE.min)));
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(AE_TARGET_MOVE_METHOD.name,
            toString(AE_TARGET_MOVE_METHOD.min)));
        parameters.addParameter(Parameter(AE_TARGET_MAX.name,
            toString(AE_TARGET_MAX.min)));
        parameters.addParameter(Parameter(AE_TARGET_MIN.name,
            toString(AE_TARGET_MIN.min)));
        parameters.addParameter(Parameter(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.name,
            toString(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.min)));
        parameters.addParameter(Parameter(AE_TARGRT_MOVE_STEP.name,
            toString(AE_TARGRT_MOVE_STEP.min)));
        parameters.addParameter(Parameter(AE_TARGET_MIN_LMT.name,
            toString(AE_TARGET_MIN_LMT.min)));
        parameters.addParameter(Parameter(AE_TARGET_UP_OVER_THRESHOLD.name,
            toString(AE_TARGET_UP_OVER_THRESHOLD.min)));
        parameters.addParameter(Parameter(AE_TARGET_UP_UNDER_THRESHOLD.name,
            toString(AE_TARGET_UP_UNDER_THRESHOLD.min)));
        parameters.addParameter(Parameter(AE_TARGET_DN_OVER_THRESHOLD.name,
            toString(AE_TARGET_DN_OVER_THRESHOLD.min)));
        parameters.addParameter(Parameter(AE_TARGET_DN_UNDER_THRESHOLD.name,
            toString(AE_TARGET_DN_UNDER_THRESHOLD.min)));

        parameters.addParameter(Parameter(AE_TARGET_EXPOSURE_METHOD.name,
            toString(AE_TARGET_EXPOSURE_METHOD.min)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_ENTER.name,
            toString(AE_TARGET_LOWLUX_GAIN_ENTER.min)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_EXIT.name,
            toString(AE_TARGET_LOWLUX_GAIN_EXIT.min)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_EXPOSURE_ENTER.name,
            toString(AE_TARGET_LOWLUX_EXPOSURE_ENTER.min)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_FPS.name,
            toString(AE_TARGET_LOWLUX_FPS.min)));
        parameters.addParameter(Parameter(AE_TARGET_NORMAL_FPS.name,
            toString(AE_TARGET_NORMAL_FPS.min)));
        parameters.addParameter(Parameter(AE_TARGET_MAX_FPS_LOCK_ENABLE.name,
            toString(AE_TARGET_MAX_FPS_LOCK_ENABLE.def)));

        parameters.addParameter(Parameter(AE_BRIGHTNESS_METERING_METHOD.name,
            toString(AE_BRIGHTNESS_METERING_METHOD.min)));
        parameters.addParameter(Parameter(AE_REGION_DUCE.name,
            toString(AE_REGION_DUCE.min)));
        parameters.addParameter(Parameter(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.name,
            toString(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.def)));
#endif //INFOTM_ISP
        break;

    case SAVE_MAX:
        parameters.addParameter(Parameter(AE_FLICKER.name,
            toString(AE_FLICKER.def)));  // bool does not have max
        parameters.addParameter(Parameter(AE_FLICKER_FREQ.name,
            toString(AE_FLICKER_FREQ.max)));
        parameters.addParameter(Parameter(AE_TARGET_BRIGHTNESS.name,
            toString(AE_TARGET_BRIGHTNESS.max)));
        parameters.addParameter(Parameter(AE_UPDATE_SPEED.name,
            toString(AE_UPDATE_SPEED.max)));
        parameters.addParameter(Parameter(AE_BRACKET_SIZE.name,
            toString(AE_BRACKET_SIZE.max)));
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(AE_TARGET_MOVE_METHOD.name,
            toString(AE_TARGET_MOVE_METHOD.max)));
        parameters.addParameter(Parameter(AE_TARGET_MAX.name,
            toString(AE_TARGET_MAX.max)));
        parameters.addParameter(Parameter(AE_TARGET_MIN.name,
            toString(AE_TARGET_MIN.max)));
        parameters.addParameter(Parameter(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.name,
            toString(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.max)));
        parameters.addParameter(Parameter(AE_TARGRT_MOVE_STEP.name,
            toString(AE_TARGRT_MOVE_STEP.max)));
        parameters.addParameter(Parameter(AE_TARGET_MIN_LMT.name,
            toString(AE_TARGET_MIN_LMT.max)));
        parameters.addParameter(Parameter(AE_TARGET_UP_OVER_THRESHOLD.name,
            toString(AE_TARGET_UP_OVER_THRESHOLD.max)));
        parameters.addParameter(Parameter(AE_TARGET_UP_UNDER_THRESHOLD.name,
            toString(AE_TARGET_UP_UNDER_THRESHOLD.max)));
        parameters.addParameter(Parameter(AE_TARGET_DN_OVER_THRESHOLD.name,
            toString(AE_TARGET_DN_OVER_THRESHOLD.max)));
        parameters.addParameter(Parameter(AE_TARGET_DN_UNDER_THRESHOLD.name,
            toString(AE_TARGET_DN_UNDER_THRESHOLD.max)));

        parameters.addParameter(Parameter(AE_TARGET_EXPOSURE_METHOD.name,
            toString(AE_TARGET_EXPOSURE_METHOD.max)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_ENTER.name,
            toString(AE_TARGET_LOWLUX_GAIN_ENTER.max)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_EXIT.name,
            toString(AE_TARGET_LOWLUX_GAIN_EXIT.max)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_EXPOSURE_ENTER.name,
            toString(AE_TARGET_LOWLUX_EXPOSURE_ENTER.max)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_FPS.name,
            toString(AE_TARGET_LOWLUX_FPS.max)));
        parameters.addParameter(Parameter(AE_TARGET_NORMAL_FPS.name,
            toString(AE_TARGET_NORMAL_FPS.max)));
        parameters.addParameter(Parameter(AE_TARGET_MAX_FPS_LOCK_ENABLE.name,
            toString(AE_TARGET_MAX_FPS_LOCK_ENABLE.def)));

        parameters.addParameter(Parameter(AE_BRIGHTNESS_METERING_METHOD.name,
            toString(AE_BRIGHTNESS_METERING_METHOD.max)));
        parameters.addParameter(Parameter(AE_REGION_DUCE.name,
            toString(AE_REGION_DUCE.max)));
        parameters.addParameter(Parameter(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.name,
            toString(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.def)));
#endif //INFOTM_ISP
        break;

    case SAVE_DEF:
        parameters.addParameter(Parameter(AE_FLICKER.name,
            toString(AE_FLICKER.def)
            + " // " + getParameterInfo(AE_FLICKER)));
        parameters.addParameter(Parameter(AE_FLICKER_FREQ.name,
            toString(AE_FLICKER_FREQ.def)
            + " // " + getParameterInfo(AE_FLICKER_FREQ)));
        parameters.addParameter(Parameter(AE_TARGET_BRIGHTNESS.name,
            toString(AE_TARGET_BRIGHTNESS.def)
            + " // " + getParameterInfo(AE_TARGET_BRIGHTNESS)));
        parameters.addParameter(Parameter(AE_UPDATE_SPEED.name,
            toString(AE_UPDATE_SPEED.def)
            + " // " + getParameterInfo(AE_UPDATE_SPEED)));
        parameters.addParameter(Parameter(AE_BRACKET_SIZE.name,
            toString(AE_BRACKET_SIZE.def)
            + " // " + getParameterInfo(AE_BRACKET_SIZE)));
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(AE_TARGET_MOVE_METHOD.name,
            toString(AE_TARGET_MOVE_METHOD.def)
            + " // " + getParameterInfo(AE_TARGET_MOVE_METHOD)));
        parameters.addParameter(Parameter(AE_TARGET_MAX.name,
            toString(AE_TARGET_MAX.def)
            + " // " + getParameterInfo(AE_TARGET_MAX)));
        parameters.addParameter(Parameter(AE_TARGET_MIN.name,
            toString(AE_TARGET_MIN.def)
            + " // " + getParameterInfo(AE_TARGET_MIN)));
        parameters.addParameter(Parameter(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.name,
            toString(AE_TARGET_GAIN_NRML_MODE_MAX_LMT.def)
            + " // " + getParameterInfo(AE_TARGET_GAIN_NRML_MODE_MAX_LMT)));
        parameters.addParameter(Parameter(AE_TARGRT_MOVE_STEP.name,
            toString(AE_TARGRT_MOVE_STEP.def)
            + " // " + getParameterInfo(AE_TARGRT_MOVE_STEP)));
        parameters.addParameter(Parameter(AE_TARGET_MIN_LMT.name,
            toString(AE_TARGET_MIN_LMT.def)
            + " // " + getParameterInfo(AE_TARGET_MIN_LMT)));
        parameters.addParameter(Parameter(AE_TARGET_UP_OVER_THRESHOLD.name,
            toString(AE_TARGET_UP_OVER_THRESHOLD.def)
            + " // " + getParameterInfo(AE_TARGET_UP_OVER_THRESHOLD)));
        parameters.addParameter(Parameter(AE_TARGET_UP_UNDER_THRESHOLD.name,
            toString(AE_TARGET_UP_UNDER_THRESHOLD.def)
            + " // " + getParameterInfo(AE_TARGET_UP_UNDER_THRESHOLD)));
        parameters.addParameter(Parameter(AE_TARGET_DN_OVER_THRESHOLD.name,
            toString(AE_TARGET_DN_OVER_THRESHOLD.def)
            + " // " + getParameterInfo(AE_TARGET_DN_OVER_THRESHOLD)));
        parameters.addParameter(Parameter(AE_TARGET_DN_UNDER_THRESHOLD.name,
            toString(AE_TARGET_DN_UNDER_THRESHOLD.def)
            + " // " + getParameterInfo(AE_TARGET_DN_UNDER_THRESHOLD)));

        parameters.addParameter(Parameter(AE_TARGET_EXPOSURE_METHOD.name,
            toString(AE_TARGET_EXPOSURE_METHOD.def)
            + " // " + getParameterInfo(AE_TARGET_EXPOSURE_METHOD)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_ENTER.name,
            toString(AE_TARGET_LOWLUX_GAIN_ENTER.def)
            + " // " + getParameterInfo(AE_TARGET_LOWLUX_GAIN_ENTER)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_GAIN_EXIT.name,
            toString(AE_TARGET_LOWLUX_GAIN_EXIT.def)
            + " // " + getParameterInfo(AE_TARGET_LOWLUX_GAIN_EXIT)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_EXPOSURE_ENTER.name,
            toString(AE_TARGET_LOWLUX_EXPOSURE_ENTER.def)
            + " // " + getParameterInfo(AE_TARGET_LOWLUX_EXPOSURE_ENTER)));
        parameters.addParameter(Parameter(AE_TARGET_LOWLUX_FPS.name,
            toString(AE_TARGET_LOWLUX_FPS.def)
            + " // " + getParameterInfo(AE_TARGET_LOWLUX_FPS)));
        parameters.addParameter(Parameter(AE_TARGET_NORMAL_FPS.name,
            toString(AE_TARGET_NORMAL_FPS.def)
            + " // " + getParameterInfo(AE_TARGET_NORMAL_FPS)));
        parameters.addParameter(Parameter(AE_TARGET_MAX_FPS_LOCK_ENABLE.name,
            toString(AE_TARGET_MAX_FPS_LOCK_ENABLE.def)
            + " // " + getParameterInfo(AE_TARGET_MAX_FPS_LOCK_ENABLE)));

        parameters.addParameter(Parameter(AE_BRIGHTNESS_METERING_METHOD.name,
            toString(AE_BRIGHTNESS_METERING_METHOD.def)
            + " // " + getParameterInfo(AE_BRIGHTNESS_METERING_METHOD)));
        parameters.addParameter(Parameter(AE_REGION_DUCE.name,
            toString(AE_REGION_DUCE.def)
            + " // " + getParameterInfo(AE_REGION_DUCE)));
        parameters.addParameter(Parameter(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.name,
            toString(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE.def)));
#endif //INFOTM_ISP
        break;
    }

    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
unsigned int ISPC::ControlAE::getNewExposure(void)
{
	return uiNewExposure;
}

double ISPC::ControlAE::getNewGain(void)
{
	return fNewGain;
}
#endif //INFOTM_ISP


#ifdef AWB_LIMIT_GAIN_PATCH
static double MinRGain = 1.00;
static double MaxRGain = 1.60;
static double MinBGain = 1.39;
static double MaxBGain = 3.24;

static double avgGain = 1.0;

double GetCurMinRGain()
{
    return MinRGain;
}

double GetCurMaxRGain()
{
    return MaxRGain;
}

double GetCurMinBGain()
{
    return MinBGain;
}

double GetCurMaxBGain()
{
    return MaxBGain;
}

void SetCurRBGainLimit(double CurGain)
{
    avgGain = (avgGain + CurGain)/2;
    if (avgGain <= 3.0)
    {
#if defined(INFOTM_Q360_PROJECT)

        MinRGain = 1.06274;//1.00;
        MaxRGain = 1.89492 - 0.15;//1.60;

        MinBGain = 1.59972 + 0.05;//1.63;
        MaxBGain = 2.93059 - 0.32;//0.32;//0.20;//3.04;//3.09;//3.15;     
#elif defined(INFOTM_LANCHOU_PROJECT)
        MinRGain = 1.294;
        MaxRGain = 2.216;

        MinBGain = 1.589;
        MaxBGain = 2.845;// + 0.10;
#else //CAMERA_MODEL_OV2710
        MinRGain = 1.00;
        MaxRGain = 1.52;

        MinBGain = 1.73;
        MaxBGain = 3.05;
#endif
    }
    else
    {
#if defined(INFOTM_Q360_PROJECT)


        MinRGain = 1.02482 + 0.1; // 1.06274 + 0.1;
        MaxRGain = 1.80628 + 0.1; // 1.89492 + 0.1;

        MinBGain = 1.47177 + 0.05; // 1.59972 + 0.05;
        MaxBGain = 2.51695 - 0.32; // 2.93059 - 0.32;
#elif defined(INFOTM_LANCHOU_PROJECT)
        MinRGain = 1.294;
        MaxRGain = 2.216;

        MinBGain = 1.589;
        MaxBGain = 2.845;// + 0.10;
#else //CAMERA_MODEL_OV2710
        MinRGain = 1.20;
        MaxRGain = 1.70;

        MinBGain = 1.73;
        MaxBGain = 3.05;

#endif

    }
//    printf("-->SetCurRBGainLimit: %f, %f, %f, %f, %f\n", CurGain, MinRGain, MaxRGain, MinBGain, MaxBGain);
}
#endif

//update configuration based no a previous shot metadata
#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlAE::update(const Metadata &metadata, const Metadata &metadata2)
#else
IMG_RESULT ISPC::ControlAE::update(const Metadata &metadata)
#endif //INFOTM_DUAL_SENSOR
{
    fNewGain = 0.0;
    uiNewExposure = 0;

#ifdef INFOTM_ISP
	const Sensor *sensor = getSensor();
#endif //INFOTM_ISP

#ifdef INFOTM_ISP
	double curBrightness1, curBrightness2;
	Pipeline* pPipeline;
	curBrightness1 = getBrightnessMetering(metadata.histogramStats, metadata.clippingStats);
	curBrightness2 = getBrightnessMetering(metadata2.histogramStats, metadata2.clippingStats);
    this->currentBrightness = (curBrightness1 + curBrightness2) / 2.0f;

    pPipeline = getPipelineOwner();
    //MOD_LOG_INFO("[%d] curBrightness1: %f, curBrightness2: %f, currentBrightness/2: %f\n", pPipeline->ui8ContextNumber, curBrightness1, curBrightness2, currentBrightness);
#else
    this->currentBrightness = getBrightnessMetering(metadata.histogramStats, metadata.clippingStats);
#endif //INFOTM_DUAL_SENSOR

#if defined(INFOTM_ISP)
        static IMG_UINT32 BrightCnt = 0;
        static IMG_UINT32 DarkCnt = 0;
        bool AeForceUpdate = false;
        double dbFleqExposureMax;
        double dbFleqExposureMin;
        double dbSensorGain;
        double dbFleqExposureTemp;
        double dbFleqExposureFactor;
        double dbFleqExposureCurrent;
        double dbFleqAeTarget;

	if (this->enMoveAETarget)
	{
#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
		if (FlatModeStatusGet())
		{
			BrightCnt = 0;
			DarkCnt = 0;
		}
		else
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE
		{
			if (isConverged())
			{
				dbSensorGain = sensor->getGain();
				dbFleqExposureMax = 1000000 / (this->flickerFreq * 2);	// 1000000 / 100 = 10000
				dbFleqExposureMin = 1000000 / (this->flickerFreq * 8);

				switch (ui32AeMoveMethod)
				{
					case AEMHD_MOVE_SHUOYING:
			            if (fAeTargetUpOverThreshold > Ratio_OverExp)
			            {
			                if (this->targetBrightness < this->OritargetBrightness)
			                {
			                    setTargetBrightness(this->targetBrightness + fAeTargetMoveStep);
			                    DarkCnt = 0;
			                    BrightCnt += fAeTargetMoveStep;
			                }
			            }
			            else if (fAeTargetDnOverThreshold < Ratio_OverExp)
			            {
			                if (fAeTargetMin < this->targetBrightness)
			                {
			                    setTargetBrightness(this->targetBrightness - fAeTargetMoveStep);
			                    BrightCnt = 0;
			                    DarkCnt += fAeTargetMoveStep;
			                }
			            }
			            else
			            {
			                BrightCnt = 0;
			                DarkCnt = 0;
			            }
					break;

					case AEMHD_MOVE_DEFAULT:
					default:
						if (((fAeTargetUpOverThreshold > Ratio_OverExp) && (fAeTargetUpUnderThreshold > Ratio_UnderExp)) ||
							(((fAeTargetUpOverThreshold + 0.01) > Ratio_OverExp) && ((double)sensor->getExposure() < ((50 == this->flickerFreq) ? (dbFleqExposureMax * 3) : (dbFleqExposureMax * 4)))))
						{
							if (this->targetBrightness < this->OritargetBrightness)
							{
								if ((0 == DarkCnt) && (0.002 > Ratio_OverExp))
								{
									setTargetBrightness(this->targetBrightness + ((this->OritargetBrightness - this->targetBrightness) / 2.0f));
								}
								else
								{
									setTargetBrightness(this->targetBrightness + fAeTargetMoveStep);
								}
								DarkCnt = 0;
								BrightCnt += fAeTargetMoveStep;
							}
						}
						//else if ((fAeTargetDnOverThreshold < Ratio_OverExp) || (fAeTargetUpUnderThreshold < Ratio_UnderExp) || (fAeTargetGainNormalModeMaxLmt < dbSensorGain))
						else if ((fAeTargetDnOverThreshold < Ratio_OverExp) || (fAeTargetGainNormalModeMaxLmt < dbSensorGain))
						{
							dbFleqExposureCurrent = (double)sensor->getExposure();

							if (dbFleqExposureCurrent > dbFleqExposureMax)
								dbFleqExposureTemp = dbFleqExposureMax;
							else if (dbFleqExposureCurrent < dbFleqExposureMin)
								dbFleqExposureTemp = dbFleqExposureMin;
							else
								dbFleqExposureTemp = dbFleqExposureCurrent;

							dbFleqExposureFactor = (dbFleqExposureMax - dbFleqExposureTemp) / (dbFleqExposureMax - dbFleqExposureMin);

							dbFleqAeTarget = this->OritargetBrightness + fAeTargetMin + (dbFleqExposureFactor * (fAeTargetMax - fAeTargetMin));
							if (fAeTargetMinLmt > dbFleqAeTarget)
							{
								dbFleqAeTarget = fAeTargetMinLmt;
							}

							if (this->targetBrightness > dbFleqAeTarget)
							{
								setTargetBrightness(this->targetBrightness - fAeTargetMoveStep);
								BrightCnt = 0;
								DarkCnt += fAeTargetMoveStep;
							}
						}
						else
						{
							BrightCnt = 0;
							DarkCnt = 0;
						}
						break;
				} //end of switch (ui32AeUpdMoveMethod)
			} //end of if (isConverged())
		} //end of if (FlatModeStatusGet())
	}
		//printf("Ratio_OverExp = %f \n", Ratio_OverExp);
		//printf("getTargetBrightness = %f \n", getTargetBrightness());
#endif //INFOTM_ISP


#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("Metered/Target brightness: %f/%f. Flicker Rejection:%s\n",
        currentBrightness, targetBrightness,
        (flickerRejection?"Enabled":"Disabled"));
#endif
    /* If pipeline statistics extraction have not been configured
     * then configure them and wait for the next iteration to update
     * the sensor parameters */
    if (!configured)
    {
        MOD_LOG_WARNING("ControlAE Statistics were not configured! Trying "\
            "to configure them now.");
        configureStatistics();
    }
    else
    {
        const Sensor *sensor = getSensor();

        /**
         * @ flicker frequency should come from the flicker
         * detection module (if available)
         */
        if (!sensor)
        {
            MOD_LOG_ERROR("ControlAE owner has no sensors!\n");
            return IMG_ERROR_NOT_INITIALISED;
        }

        if (autoFlickerRejection )
        {
            if (FLD_50HZ == metadata.flickerStats.flickerStatus)
            {
                flickerFreq = 50.0;
            }
            else if (FLD_60HZ == metadata.flickerStats.flickerStatus)
            {
                flickerFreq = 60.0;
            }
            else if (FLD_NO_FLICKER == metadata.flickerStats.flickerStatus)
            {
                flickerFreq = 0.0;
            }
        }

#ifdef INFOTM_ISP
        if (this->enMoveAETarget)
        {
			if (BrightCnt >= (this->targetBracket / 2) || DarkCnt >= (this->targetBracket / 2))
			{
				AeForceUpdate = true;
				BrightCnt = 0;
				DarkCnt = 0;
			}
        }
        if (settingsUpdated || !isConverged() || AeForceUpdate) //Run only if we are out of the acceptable distance (targetBracket) from target brightness
#else
		if(!isConverged())	 //Run only if we are out of the acceptable distance (targetBracket) from target brightness
#endif //INFOTM_ISP
        {

#ifdef INFOTM_ISP
            static int iAeTargetMode = AE_TARGET_MODE_NORMAL;
    		double dbExposureMax;
            double flFrameRate;

            switch (ui32AeExposureMethod)
            {
            	case AEMHD_EXP_LOWLUX:
					#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
					if (FlatModeStatusGet())
					{
						flFrameRate = fAeTargetLowluxFPS;
						iAeTargetMode = AE_TARGET_MODE_NORMAL;
					}
					else
					#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE
					{
						switch (iAeTargetMode)
						{
							case AE_TARGET_MODE_NORMAL:
								if ((ui32AeTargetLowluxExposureEnter < sensor->getExposure()) && (fAeTargetLowluxGainEnter <= sensor->getGain()))
									iAeTargetMode = AE_TARGET_MODE_LOWLUX;
								break;

							case AE_TARGET_MODE_LOWLUX:
								if (fAeTargetLowluxGainExit > sensor->getGain())
									iAeTargetMode = AE_TARGET_MODE_NORMAL;
								break;
						}
						flFrameRate = (iAeTargetMode == AE_TARGET_MODE_LOWLUX) ? fAeTargetLowluxFPS : fAeTargetNormalFPS;
                        {
                            static double fAeTargetNormalFPSBackup = -1;

                            if (true == bAeTargetMaxFpsLockEnable) {
                                if (fAeTargetNormalFPSBackup != fAeTargetNormalFPS) {
                                    Sensor *pSensor = getSensor();
    #if 0
                                    SENSOR_MODE stSeneorMode;

                                    pSensor->getMode(0, stSeneorMode);
                                    pSensor->SetFPS(stSeneorMode.flFrameRate / fAeTargetNormalFPS);
    #else
                                    pSensor->SetFPS(fAeTargetNormalFPS);
    #endif
                                    fAeTargetNormalFPSBackup = fAeTargetNormalFPS;
                                }
                            }
                        }
					}
            		break;

            	case AEMHD_EXP_DEFAULT:
            	default:
#ifndef INFOTM_ISP
                    flFrameRate = sensor->flFrameRate;
#else
                    flFrameRate = sensor->flCurrentFPS;
#endif
            		break;
            }
			if (true == this->flickerRejection)
			{
				if (50.0 == this->flickerFreq)
					dbExposureMax = (IMG_UINT32)(100.0 / flFrameRate) * 10000.0;
				else
					dbExposureMax = 1000000.0 / flFrameRate;
			}
			else
			{
				dbExposureMax = sensor->getMaxExposure();
			}
#endif //INFOTM_ISP

		
            /* Calculate the new gain and exposure values based on current
             * & target brightness, sensor configuration, flicker
             * rejection, etc. */

            getAutoExposure(this->currentBrightness,
                this->targetBrightness,
                0.0f,
                0.0f,
                sensor->getExposure()*sensor->getGain(), //fPrevExposure
                sensor->getMinExposure(),                //fMinExposure
#ifndef INFOTM_ISP
                sensor->getMaxExposure(),                //fMaxExposure
#else
				dbExposureMax,                           //fMaxExposure
#endif //INFOTM_ISP
                sensor->getMinGain(),                    //minGain
#ifdef INFOTM_ISP
                getMaxAeGain(),
#else
                sensor->getMaxGain(),                    //maxGain
#endif
                this->flickerRejection,
                this->flickerFreq,  // in Hz
                this->updateSpeed,
                this->fNewGain,
                this->uiNewExposure,
                this->underexposed);

#ifndef INFOTM_ISP
	#if defined(INFOTM_ENABLE_AE_DEBUG)
				printf("=== currentBrightness = %f, targetBrightness = %f, flickerRejection = %d, flickerFreq = %f ===\n",
					this->currentBrightness,
					this->targetBrightness,
					this->flickerRejection,
					this->flickerFreq
					);
	#endif //INFOTM_ENABLE_AE_DEBUG
//				printf("=== currentBrightness = %f ===\n",
//					this->currentBrightness
//					);	
#endif //INFOTM_ISP

            if (doAE)

            {
#ifdef INFOTM_ISP
                static IMG_UINT32 ui32ExposureBackup = 0;
                static double fGainBackup = 0;

                if ((ui32ExposureBackup != uiNewExposure) || (fGainBackup != fNewGain) || bAeForceCallSensorDriverEnable)
                {
  #if defined(INFOTM_ISP_EXPOSURE_GAIN_DEBUG)
                    printf("fps %f, exposure/gain: %d/%f\n", flFrameRate, uiNewExposure, fNewGain);
  #endif
                    programCorrection();
                    ui32ExposureBackup = uiNewExposure;
                    fGainBackup = fNewGain;
                }
#else
                programCorrection();
#endif //INFOTM_ISP
            }
#ifdef INFOTM_ISP
            settingsUpdated = false;
#endif
        }
#if VERBOSE_CONTROL_MODULES
        else
        {
            MOD_LOG_INFO("Sensor exposure/gain: %ld/%f.\n",
                sensor->getExposure(), sensor->getGain());
        }
#endif

        this->previousBrightness = this->currentBrightness;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAE::configureStatistics()
{
    ModuleHIS *pHIS = NULL;

    if (getPipelineOwner())
    {
        pHIS = getPipelineOwner()->getModule<ModuleHIS>();
    }
    else
    {
        MOD_LOG_ERROR("ControlAE has no pipeline owner! Cannot "\
            "configure statistics.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    configured = false;
    if (pHIS)
    {
        /* Configure statistics to make the 7x7 grid cover as much
         * image as possible */
        const Sensor *sensor = getSensor();
        if (!sensor)
        {
            MOD_LOG_ERROR("ControlAE owner has no sensors!\n");
            return IMG_ERROR_NOT_INITIALISED;
        }

        if ( !pHIS->bEnableGlobal || !pHIS->bEnableROI )
        {
            MOD_LOG_DEBUG("AE RECONFIGURED!\n");
            pHIS->bEnableGlobal = true;
            pHIS->bEnableROI = true;

            pHIS->ui32InputOffset = ModuleHIS::HIS_INPUTOFF.def;
            pHIS->ui32InputScale = ModuleHIS::HIS_INPUTSCALE.def;

            int tileWidth = static_cast<int>(floor((sensor->uiWidth) / 7.0));
            int tileHeight = static_cast<int>(floor((sensor->uiHeight) / 7.0));
            int marginWidth = sensor->uiWidth - (tileWidth * 7);
            int marginHeight = sensor->uiHeight - (tileHeight * 7);

            pHIS->aGridStartCoord[0] = marginWidth / 2;
            pHIS->aGridStartCoord[1] = marginHeight / 2;
            pHIS->aGridTileSize[0] = tileWidth;
            pHIS->aGridTileSize[1] = tileHeight;

            pHIS->requestUpdate();
        }
        configured = true;

        return IMG_SUCCESS;
    }
    else
    {
        MOD_LOG_ERROR("ControlAE cannot find HIS module\n");
    }

    return IMG_ERROR_UNEXPECTED_STATE;
}

IMG_RESULT ISPC::ControlAE::programCorrection()
{
    Sensor *sensor = getSensor();
    if (!sensor)
    {
        MOD_LOG_ERROR("ControlAE owner has no sensors!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
#ifdef INFOTM_ISP
	// set sensor gain and exposure by group method. 
	// if the sensor does not support group, call set exposure and set gain function.
	#ifdef INFOTM_ISP
//		Pipeline* pPipeline;
//		pPipeline = getPipelineOwner();
//		MOD_LOG_INFO("[%d] fNewGain: %f, uiNewExposure: %d\n", pPipeline->ui8ContextNumber, fNewGain, uiNewExposure);

		std::list<Pipeline *>::iterator it;
		for (it = pipelineList.begin(); it != pipelineList.end(); it++)
		{
			//MOD_LOG_INFO("[%d] fNewGain: %f, uiNewExposure: %d\n", (*it)->ui8ContextNumber, fNewGain, uiNewExposure);
			sensor = (*it)->getSensor();
			sensor->SetGainAndExposure(fNewGain, uiNewExposure);
		}
	#else
	    sensor->SetGainAndExposure(fNewGain, uiNewExposure);
	#endif //INFOTM_DUAL_SENSOR
#else
	sensor->setExposure(uiNewExposure);
	sensor->setGain(fNewGain);
#endif //INFOTM_ISP
#ifndef INFOTM_Q360_PROJECT
#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("Change exposure/gain: %d/%f - programmed %d/%f\n",
        uiNewExposure, fNewGain, sensor->getExposure(), sensor->getGain());
#endif
#endif
    return IMG_SUCCESS;
}

void ISPC::ControlAE::setUpdateSpeed(double value)
{
    if (value < 0.0 || value > 1.0)
    {
        MOD_LOG_ERROR("Update speed must be between 0.0 and 1.0 "\
            "(received: %f)\n", value);
        return;
    }
    updateSpeed = value;
}

double ISPC::ControlAE::getUpdateSpeed() const
{
    return updateSpeed;
}

void ISPC::ControlAE::setTargetBrightness(double value)
{
    targetBrightness = value;
}

double ISPC::ControlAE::getTargetBrightness() const
{
    return targetBrightness;
}

void ISPC::ControlAE::enableFlickerRejection(bool enable, double freq)
{
    flickerRejection = enable;
    if (freq > 0.0)
    {
        flickerFreq = freq;
        // disable auto if frequency provided
        autoFlickerRejection = false;
    }
}

void ISPC::ControlAE::enableAutoFlickerRejection(bool enable)
{
    autoFlickerRejection = enable;
}

bool ISPC::ControlAE::getFlickerRejection() const
{
    return flickerRejection;
}

#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlAE::setAntiflickerFreq(double freq)
{
	this->flickerFreq = freq;
	return IMG_SUCCESS;
}
#endif //INFOTM_ISP

double ISPC::ControlAE::getFlickerRejectionFrequency() const
{
    return flickerFreq;
}

bool ISPC::ControlAE::getAutoFlickerRejection() const
{
    return autoFlickerRejection;
}

void ISPC::ControlAE::setTargetBracket(double value)
{
    targetBracket = value;
}

double ISPC::ControlAE::getTargetBracket() const
{
    return targetBracket;
}

double ISPC::ControlAE::getCurrentBrightness() const
{
    return currentBrightness;
}

void ISPC::ControlAE::getHistogramStats(double nHistogram[],
    int nBins, double &avgValue, double &underExposureF,
    double &overExposureF)
{
    avgValue = 0;
    underExposureF = 0;
    overExposureF = 0;
    double posFactor = 63.0 / (nBins - 1);

    for (int i = 0; i < nBins; i++)
    {
        double position = (double) i*posFactor / (63.0);
        avgValue += position*nHistogram[i];
        overExposureF += pow(position, 3)*nHistogram[i];
        underExposureF += pow(1.0 - position, 3)*nHistogram[i];
    }
    avgValue = avgValue - 0.5;
}

//normalize an histogram so all the valuess add up to 1.0
void ISPC::ControlAE::normalizeHistogram(const IMG_UINT32 sourceH[],
    double destinationH[], int nBins)
{
    double totalHistogram = 0;
    int i;

    for (i = 0; i < nBins; i++)
    {
        totalHistogram += sourceH[i];
    }

    if (0 != totalHistogram)
    {
        for (i = 0; i < nBins; i++)
        {
            destinationH[i] = ((double)sourceH[i])/totalHistogram;
        }
    }
    else
    {
#ifndef INFOTM_ISP	
        MOD_LOG_WARNING("Total histogram is 0!\n");
#endif //INFOTM_ISP		
        for (i = 0; i < nBins; i++)
        {
            destinationH[i] = static_cast<double>(sourceH[i]);
        }
    }
}

//Get a weighted combination of statistics from the grid
double ISPC::ControlAE::getWeightedStats(
    double gridStats[HIS_REGION_VTILES][HIS_REGION_HTILES])
{
    double totalWeight = 0;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            totalWeight += WEIGHT_7X7[i][j] * gridStats[i][j];
        }
    }

    return totalWeight;
}

//Get weighted measure based in the A and B 7x7 weight matrices
double ISPC::ControlAE::getWeightedStatsBlended(double gridStats[7][7],
    double combinationWeight)
{
    double totalWeight = 0;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            totalWeight += WEIGHT_7X7_A[i][j] * gridStats[i][j]
                * combinationWeight + WEIGHT_7X7_B[i][j] * gridStats[i][j]
                * (1 - combinationWeight);
        }
    }
    return totalWeight;
}

void ISPC::ControlAE::getBackLightMeasure(double gridStats[7][7],
    double &foregroundAvg, double &backgroundAvg)
{
    double nForeground = 0;
    double totalForeground = 0;
    double totalBackground = 0;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            totalForeground += gridStats[i][j] * BACKLIGHT_7X7[i][j];
            totalBackground += gridStats[i][j] * (1.0 - BACKLIGHT_7X7[i][j]);
            nForeground += BACKLIGHT_7X7[i][j];
        }
    }

    foregroundAvg = totalForeground / nForeground;
    backgroundAvg = totalBackground / (49 - nForeground);
}

#ifdef INFOTM_ISP
void ISPC::ControlAE::enableAE(bool enable)
{
    this->doAE = enable;
}

bool ISPC::ControlAE::IsAEenabled() const
{
    return this->doAE;
}

void ISPC::ControlAE::getRegionBrightness(double *fRegionBrightness)
{

    memcpy(fRegionBrightness, regionBrightness, sizeof(regionBrightness));
}

void ISPC::ControlAE::getRegionOverExp(double *fRegionOverExp)
{

    memcpy(fRegionOverExp, regionOverExp, sizeof(regionOverExp));
}

void ISPC::ControlAE::getRegionUnderExp(double *fRegionUnderExp)
{

    memcpy(fRegionUnderExp, regionUnderExp, sizeof(regionUnderExp));
}

void ISPC::ControlAE::getRegionRatio(double *fRegionRatio)
{

    memcpy(fRegionRatio, regionRatio, sizeof(fRegionRatio));
}

double ISPC::ControlAE::getOverUnderExpDiff() const
{
    return OverUnderExpDiff;
}

double ISPC::ControlAE::getRatioOverExp() const
{
    return Ratio_OverExp;
}

double ISPC::ControlAE::getRatioMidOverExp() const
{
    return Ratio_MidOverExp;
}

double ISPC::ControlAE::getRatioUnderExp() const
{
    return Ratio_UnderExp;
}

unsigned int ISPC::ControlAE::getAeMode() const
{
    return this->uiAeMode;
}

//get set the original target brightness
void ISPC::ControlAE::setOriTargetBrightness(double value)
{
    OritargetBrightness = value;

    setTargetBrightness(value);
}

double ISPC::ControlAE::getOriTargetBrightness() const
{
    return OritargetBrightness;
}

bool ISPC::ControlAE::IsAeTargetMoveEnable(void)
{
    return enMoveAETarget;
}

void ISPC::ControlAE::AeTargetMoveEnable(bool bEnable)
{
    enMoveAETarget = bEnable;
    settingsUpdated = true;
}

int ISPC::ControlAE::getAeTargetMoveMethod(void)
{
    return ui32AeMoveMethod;
}

void ISPC::ControlAE::setAeTargetMoveMethod(int iAeTargetMoveMethod)
{
    ui32AeMoveMethod = iAeTargetMoveMethod;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetMax(void)
{
    return fAeTargetMax;
}

void ISPC::ControlAE::setAeTargetMax(double dAeTargetMax)
{
    fAeTargetMax = dAeTargetMax;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetMin(void)
{
    return fAeTargetMin;
}

void ISPC::ControlAE::setAeTargetMin(double dAeTargetMin)
{
    fAeTargetMin = dAeTargetMin;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetGainNormalModeMaxLmt(void)
{
    return fAeTargetGainNormalModeMaxLmt;
}

void ISPC::ControlAE::setAeTargetGainNormalModeMaxLmt(double dAeTargetGainNormalModeMaxLmt)
{
    fAeTargetGainNormalModeMaxLmt = dAeTargetGainNormalModeMaxLmt;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetMoveStep(void)
{
    return fAeTargetMoveStep;
}

void ISPC::ControlAE::setAeTargetMoveStep(double dAeTargetMoveStep)
{
    fAeTargetMoveStep = dAeTargetMoveStep;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetMinLmt(void)
{
    return fAeTargetMinLmt;
}

void ISPC::ControlAE::setAeTargetMinLmt(double dAeTargetMinLmt)
{
    fAeTargetMinLmt = dAeTargetMinLmt;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetUpOverThreshold(void)
{
    return fAeTargetUpOverThreshold;
}

void ISPC::ControlAE::setAeTargetUpOverThreshold(double dAeTargetUpOverThreshold)
{
    fAeTargetUpOverThreshold= dAeTargetUpOverThreshold;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetUpUnderThreshold(void)
{
    return fAeTargetUpUnderThreshold;
}

void ISPC::ControlAE::setAeTargetUpUnderThreshold(double dAeTargetUpUnderThreshold)
{
    fAeTargetUpUnderThreshold = dAeTargetUpUnderThreshold;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetDnOverThreshold(void)
{
    return fAeTargetDnOverThreshold;
}

void ISPC::ControlAE::setAeTargetDnOverThreshold(double dAeTargetDnOverThreshold)
{
    fAeTargetDnOverThreshold = dAeTargetDnOverThreshold;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetDnUnderThreshold(void)
{
    return fAeTargetDnUnderThreshold;
}

void ISPC::ControlAE::setAeTargetDnUnderThreshold(double dAeTargetDnUnderThreshold)
{
    fAeTargetDnUnderThreshold = dAeTargetDnUnderThreshold;
    settingsUpdated = true;
}

int ISPC::ControlAE::getAeExposureMethod(void)
{
    return ui32AeExposureMethod;
}

void ISPC::ControlAE::setAeExposureMethod(int iAeExposureMethod)
{
    ui32AeExposureMethod = iAeExposureMethod;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetLowluxGainEnter(void)
{
    return fAeTargetLowluxGainEnter;
}

void ISPC::ControlAE::setAeTargetLowluxGainEnter(double dAeTargetLowluxGainEnter)
{
    fAeTargetLowluxGainEnter = dAeTargetLowluxGainEnter;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetLowluxGainExit(void)
{
    return fAeTargetLowluxGainExit;
}

void ISPC::ControlAE::setAeTargetLowluxGainExit(double dAeTargetLowluxGainExit)
{
    fAeTargetLowluxGainExit = dAeTargetLowluxGainExit;
    settingsUpdated = true;
}

int ISPC::ControlAE::getAeTargetLowluxExposureEnter(void)
{
    return ui32AeTargetLowluxExposureEnter;
}

void ISPC::ControlAE::setAeTargetLowluxExposureEnter(int iAeTargetLowluxExposureEnter)
{
    ui32AeTargetLowluxExposureEnter = iAeTargetLowluxExposureEnter;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetLowluxFPS(void)
{
    return fAeTargetLowluxFPS;
}

void ISPC::ControlAE::setAeTargetLowluxFPS(double dAeTargetLowluxFPS)
{
    fAeTargetLowluxFPS = dAeTargetLowluxFPS;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeTargetNormalFPS(void)
{
    return fAeTargetNormalFPS;
}

void ISPC::ControlAE::setAeTargetNormalFPS(double dAeTargetNormalFPS)
{
    fAeTargetNormalFPS = dAeTargetNormalFPS;
    settingsUpdated = true;
}

bool ISPC::ControlAE::getAeTargetMaxFpsLockEnable(void)
{
    return bAeTargetMaxFpsLockEnable;
}

void ISPC::ControlAE::setAeTargetMaxFpsLockEnable(bool enable)
{
    bAeTargetMaxFpsLockEnable = enable;
    settingsUpdated = true;
}

int ISPC::ControlAE::getAeBrightnessMeteringMethod(void)
{
    return ui32AeBrightnessMeteringMethod;
}

void ISPC::ControlAE::setAeBrightnessMeteringMethod(int iAeBrightnessMeteringMethod)
{
    ui32AeBrightnessMeteringMethod = iAeBrightnessMeteringMethod;
    settingsUpdated = true;
}

double ISPC::ControlAE::getAeRegionDuce(void)
{
    return fAeRegionDuce;
}

void ISPC::ControlAE::setAeRegionDuce(double dAeRegionDuce)
{
    fAeRegionDuce = dAeRegionDuce;
    settingsUpdated = true;
}

void ISPC::ControlAE::setFpsRange(double fMaxFps, double fMinFps)
{
    fAeTargetNormalFPS = fMaxFps;
    fAeTargetLowluxFPS = fMinFps;
    settingsUpdated = true;
}

ISPC::ControlAE::~ControlAE()
{
    pthread_mutex_destroy((pthread_mutex_t*)&paramLock);
}

void ISPC::ControlAE::setMaxManualAeGain(double fMaxAeGain, bool bManualEnable)
{
    pthread_mutex_lock((pthread_mutex_t*)&paramLock);
    bMaxManualAeGainEnable = bManualEnable;
    fMaxManualAeGain = fMaxAeGain;
    settingsUpdated = true;
    pthread_mutex_unlock((pthread_mutex_t*)&paramLock);
}

double ISPC::ControlAE::getMaxManualAeGain(void)
{
    return fMaxManualAeGain;
}

double ISPC::ControlAE::getMaxOrgAeGain() const
{
    double fGain = 0.0;
    const Sensor *sensor = getSensor();

    if (sensor)
    {
        pthread_mutex_lock((pthread_mutex_t*)&paramLock);
        fGain = sensor->getMaxGain();
        pthread_mutex_unlock((pthread_mutex_t*)&paramLock);
    }
    return fGain;
}

double ISPC::ControlAE::getMaxAeGain() const
{
    double fGain = 0.0;
    const Sensor *sensor = getSensor();

    pthread_mutex_lock((pthread_mutex_t*)&paramLock);
    if (bMaxManualAeGainEnable)
    {
        fGain = fMaxManualAeGain;
    }
    else if (sensor)
    {
        fGain =  sensor->getMaxGain();
    }
    pthread_mutex_unlock((pthread_mutex_t*)&paramLock);
    return fGain;
}
#endif //INFOTM_ISP

double ISPC::ControlAE::getBrightnessMetering(const MC_STATS_HIS &histogram,
    const MC_STATS_EXS &expClipping)
{
    double totalSamples = 0;

    // Compute statistics for each tile in the 7x7 grid
    for (int i = 0; i < HIS_REGION_VTILES; i++)
    {
        for (int j = 0; j < HIS_REGION_HTILES; j++)
        {
            double nRegHistogram[HIS_REGION_BINS];
            normalizeHistogram(histogram.regionHistograms[i][j],
                nRegHistogram, HIS_REGION_BINS);
            getHistogramStats(nRegHistogram, HIS_REGION_BINS,
                regionBrightness[i][j], regionUnderExp[i][j],
                regionOverExp[i][j]);
				
            #ifdef INFOTM_ISP
			for (int k = 13; k<HIS_REGION_BINS; k++)
            {
                this->Ratio_OverExp += nRegHistogram[k];
                this->Ratio_UnderExp += nRegHistogram[HIS_REGION_BINS-1-k];
            }
            for (int k = 7; k<13; k++) {
                this->Ratio_MidOverExp += nRegHistogram[k];
            }
			#endif //INFOTM_ISP

#ifdef USE_MATH_NEON
			if(regionOverExp[i][j]==0 && regionUnderExp[i][j]==0){
				regionRatio[i][j] = 0;
				continue;
			}
			if(regionOverExp[i][j]==0){
				regionRatio[i][j] = 0 - (double)powf_neon((float)regionUnderExp[i][j], 2.0f);
				continue;
			}
			if(regionUnderExp[i][j]==0){
				regionRatio[i][j] = (double)powf_neon((float)regionOverExp[i][j], 2.0f) - 0;
				continue;
			}

			regionRatio[i][j] =
                (double)powf_neon((float)regionOverExp[i][j], 2.0f) - (double)powf_neon((float)regionUnderExp[i][j], 2.0f);
#else
            if(regionOverExp[i][j]==0 && regionUnderExp[i][j]==0){
                regionRatio[i][j] = 0;
                continue;
            }
            if(regionOverExp[i][j]==0){
                regionRatio[i][j] = 0 - (double)pow((float)regionUnderExp[i][j], 2.0f);
                continue;
            }
            if(regionUnderExp[i][j]==0){
                regionRatio[i][j] = (double)pow((float)regionOverExp[i][j], 2.0f) - 0;
                continue;
            }
			regionRatio[i][j] =
                pow(regionOverExp[i][j], 2) - pow(regionUnderExp[i][j], 2);
#endif
        }
    }

	#ifdef INFOTM_ISP
	this->Ratio_MidOverExp /= 49;
	this->Ratio_OverExp /= 49;
	this->Ratio_UnderExp /= 49;
	#endif //INFOTM_ISP

    /* Estimate the backlight by comparing background vs foreground
     * average illumination */
    double foregroundAvg, backgroundAvg;
    getBackLightMeasure(regionRatio, foregroundAvg, backgroundAvg);
    double backlightDifference =
        ispc_min(1.0, fabs(foregroundAvg - backgroundAvg));

    /* If there is a big difference between background and foreground
     * we increase an accumulated value */
    static double backLightAccumulation = 0;
    if (backlightDifference > 0.5)
    {
        backLightAccumulation += 0.025;
    }
    else
    {
        backLightAccumulation -= 0.025;
    }
    backLightAccumulation = ispc_clip(backLightAccumulation, 0, 1);

    // Get the maximum values for over and under exposed tiles in the grid
    double overExposure = 0;
    double underExposure = 0;
    for (int i = 0; i < HIS_REGION_VTILES; i++)
    {
        for (int j = 0; j < HIS_REGION_HTILES; j++)
        {
            overExposure = ispc_max(overExposure, regionOverExp[i][j]);
            underExposure = ispc_max(underExposure, regionUnderExp[i][j]);
        }
    }
    double overUnderExposureDiff = overExposure - underExposure;
	#ifdef INFOTM_ISP
    this->OverUnderExpDiff = overUnderExposureDiff;
	#endif //INFOTM_ISP
    /* Get weighted exposure value using the grid and the 7x7
     * coefficient matrices */


#ifdef INFOTM_ISP
    //Get weighted exposure value using the grid and the 7x7 coefficient matrices
    double centreWeightedExposure;

#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
    if (FlatModeStatusGet())
    {
        centreWeightedExposure = getWeightedStatsBlended(regionRatio, 1.00);
    }
    else
#endif
    {
        centreWeightedExposure = getWeightedStatsBlended(regionRatio, fAeRegionDuce);
    }
    //printf("centreWeightedExposure region_duce = %f\n", fAeRegionDuce);

	double finalMetering = overUnderExposureDiff*0.2
	        + centreWeightedExposure * 0.80;
	switch (ui32AeBrightnessMeteringMethod)
	{
		case AEMHD_BM_SHUOYING:
			finalMetering = (finalMetering * 0.25 + foregroundAvg * 0.75) * backLightAccumulation + finalMetering * (1 - backLightAccumulation);
			break;

		case AEMHD_BM_DEFAULT:
		default:   	
			// The method is depend on the project. But defult use IMG supply.
			finalMetering = overUnderExposureDiff * 0.20 + centreWeightedExposure * 0.80;
            finalMetering = (finalMetering * 0.25 + foregroundAvg * 0.75) * backLightAccumulation + finalMetering * (1 - backLightAccumulation);
			break;
	}
#else

    double centreWeightedExposure =
        getWeightedStatsBlended(regionRatio, 0.65);

    double finalMetering = overUnderExposureDiff*0.2
        + centreWeightedExposure * 0.80;
    finalMetering = (finalMetering * 0.25 + foregroundAvg * 0.75)
        * backLightAccumulation + finalMetering*(1.0 - backLightAccumulation);
#endif //INFOTM_ISP

    return finalMetering;
}

bool ISPC::ControlAE::isConverged(void)
{
    if (fabs(targetBrightness - currentBrightness) <= targetBracket)
    {
        return true;
    }
    return false;
}

bool ISPC::ControlAE::isUnderexposed(void)
{
    return underexposed;
}

//Automatic exposure control, returns exposure value based on previous meterings and sensor settings
double ISPC::ControlAE::autoExposureCtrl(double brightnessMetering,
    double brightnessTarget, double prevExposure, double minExposure,
    double maxExposure, double blending)
{
    double brightnessError = brightnessTarget - brightnessMetering;
    brightnessError = ispc_clip(brightnessError, -1.0, 1.0);

    double newExposureValue = prevExposure;

    if (0.0 != brightnessError)
    {
    	double errorSign = brightnessError / fabs(brightnessError);
#ifdef USE_MATH_NEON
		newExposureValue = prevExposure * (1 + errorSign
		* (double)powf_neon((float)fabs(brightnessError), 1.25f) * blending);
#else
		newExposureValue = prevExposure * (1 + errorSign
		* pow(fabs(brightnessError), 1.25) * blending);
#endif
    }

    return newExposureValue;
}

//Adjustment of the exposure and gain times based on the flicker frequency, to avoid it
void ISPC::ControlAE::getFlickerExposure(double flFlickerHz,
    double flRequested, double flMax, double *flGain,
    unsigned int *ulExposureMicros)
{
    int exposurecycles;
    double flFlickerPeriod;
    double flexposuretime;

#ifndef INFOTM_ISP
    flFlickerPeriod = 1000000 / (flFlickerHz);
#else
    flFlickerPeriod = 1000000/(flFlickerHz * 2);
#endif //INFOTM_ISP	

#if defined(INFOTM_ENABLE_AE_DEBUG)
    printf("Calculating flicker exposure for requested exposure %f,\n flicker period = %f\n", flRequested, flFlickerPeriod);
#endif  //INFOTM_ENABLE_AE_DEBUG	
    if (flRequested < flFlickerPeriod || 0.0 == flFlickerHz)
    {
#if defined(INFOTM_ENABLE_AE_DEBUG)
        printf("Exposure too short for flicker rejection\n");
#endif //INFOTM_ENABLE_AE_DEBUG	
        *flGain = 1;  // no gain
        // convert the exposure to microseconds
        *ulExposureMicros = static_cast<unsigned int>(flRequested);
    }
    else
    {
        // we can compensate for this flicker.
        /* set our exposure to be an integer number of mains cycles,
         * and adjust gain to compensate */
        exposurecycles = static_cast<int>(flRequested / flFlickerPeriod);
        flexposuretime = exposurecycles*flFlickerPeriod;
#if defined(INFOTM_ENABLE_AE_DEBUG)
        printf("exposure_cycles = %d, flexposureTime=%f\n",exposurecycles,flexposuretime);
#endif  //INFOTM_ENABLE_AE_DEBUG	
        if (flexposuretime > flMax)
        {
            flexposuretime = flMax;
#if defined(INFOTM_ENABLE_AE_DEBUG)
            printf("Warning Requested exposure %f exceeds max frame period %f\n",flexposuretime,flMax);
#endif //INFOTM_ENABLE_AE_DEBUG
        }
        *ulExposureMicros = static_cast<unsigned int>(flexposuretime);
        *flGain = flRequested / flexposuretime;
        LOG_DEBUG("Flicker correction applied, %d cycles, "\
            "requested=%f new exposure=%f",
            exposurecycles, flRequested, flexposuretime);
#if defined(INFOTM_ENABLE_AE_DEBUG)
        printf("=== flRequested = %f, flFlickerPeriod = %f, exposurecycles = %d, flexposuretime = %f\n",
            flRequested,
            flFlickerPeriod,
            exposurecycles,
            flexposuretime
            );
#endif //INFOTM_ENABLE_AE_DEBUG	
    }
    return;
}

//adjust exposure and gain according to the sensor exposure step
void ISPC::ControlAE::adjustExposureStep(unsigned int  exposureStep,
    unsigned int minExposure, unsigned int maxExposure,
    unsigned int &ulExposure, double &flGain,
    unsigned int &correctedExposure, double &correctedGain)
{
    double additionalGain;
    // double minExposure = exposureStep;
    IMG_ASSERT(0 != exposureStep);
    correctedExposure = ispc_max(exposureStep,
        (ulExposure) / (exposureStep)*exposureStep);

    correctedExposure = ispc_clip(correctedExposure, minExposure, maxExposure);

    if (correctedExposure > 0)
    {
        additionalGain = static_cast<double>(ulExposure) / correctedExposure;
    }
    else
    {
        additionalGain = 1.0;
    }

    if (additionalGain < 1.0 && ulExposure > exposureStep)
    {
        additionalGain = 1.0;
    }

    correctedGain = flGain*additionalGain;
}

//This function calculates the new exposure values taking into account the sensor parameters, flicker, etc.
void ISPC::ControlAE::getAutoExposure(
    double fBrightnessMetering, double fBrightnessTarget,
    double fixedExposure, double fixedGain,
    double fPrevExposure, double fMinExposure, double fMaxExposure,
    double minGain, double maxGain,
    bool controlFlicker, double fFlickerHz, double fblending,
    double &newGain, unsigned int &newExposure, bool &bUnderexposed)
{
    unsigned int correctedExposure;
    double correctedGain;
    /* First calculate the desired combined exposure to get closer
     * to the target brightness metering */
    double targetExposure = autoExposureCtrl(fBrightnessMetering,
        fBrightnessTarget, fPrevExposure, fMinExposure, fMaxExposure,
        fblending);

    /* semi-automatic exposure/gain control (exposure and/or gain could
     * be manually set, no flicker rejection) */
    if (0.0 != fixedExposure || 0.0 != fixedGain)
    {
        if (0.0 != fixedExposure && .0 == fixedGain)
        {
            newExposure = static_cast<unsigned int>(fixedExposure);
            newGain = targetExposure / newExposure;
        }

        if (0.0 == fixedExposure && 0.0 != fixedGain)
        {
            newExposure = static_cast<unsigned int>(targetExposure / newGain);
            newGain = fixedGain;
        }

        if (0.0 != fixedExposure && 0.0 != fixedGain)
        {
            newExposure = static_cast<unsigned int>(fixedExposure);
            newGain = fixedGain;
        }
        correctedExposure = newExposure;
        correctedGain = newGain;
    }
    else
    {
        /* Automatic exposure and gain control (with optional flicker
         * rejection
         * Calculate the exposure time and gain values based on
         * anti-flicker detection */
        double flickerGain = 1.0;
        unsigned int flickerExposure =
            static_cast<unsigned int>(targetExposure);
        if (0.0 != fFlickerHz && controlFlicker)
        {
            // we have to apply flicker rejection
            getFlickerExposure(fFlickerHz, targetExposure, fMaxExposure,
                &flickerGain, &flickerExposure);
#if defined(INFOTM_ENABLE_AE_DEBUG)
            printf("Flicker exposure = %d, flickerHz = %f\n", flickerExposure, fFlickerHz);
#endif //INFOTM_ENABLE_AE_DEBUG
        }
        else
        {
            /* no flicker problems so we set gain to 1.0 and the exact
             * exposure time we want.*/
            flickerGain = 1.0;
            flickerExposure = static_cast<unsigned int>(targetExposure);
        }

        adjustExposureStep(
            static_cast<unsigned int>(fMinExposure),  // step
            static_cast<unsigned int>(fMinExposure),  // min
            static_cast<unsigned int>(fMaxExposure),  // max
            flickerExposure, flickerGain, correctedExposure, correctedGain);
    }
    bUnderexposed = false;
    if (correctedExposure > fMaxExposure && correctedGain > maxGain)
    {
        /* if the desired settings lie outside supported values for
         * current sensor
         * set as underexposed (usable for enabling flash light) */
        bUnderexposed = true;
    }
        correctedExposure = ispc_clip(correctedExposure,
            static_cast<unsigned int>(fMinExposure),
            static_cast<unsigned int>(fMaxExposure));
        correctedGain = ispc_clip(correctedGain, minGain, maxGain);

    newGain = correctedGain;
    newExposure = correctedExposure;
}
