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
#include <limits>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/Pipeline.h"
#include "ispc/ModuleHIS.h"
#include "ispc/ModuleFLD.h"
#include "ispc/Sensor.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Save.h"

#include "ispc/PerfTime.h"
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

const double ISPC::ControlAE::WEIGHT_7X7_A[7][7] = {
    { 0.0129, 0.0161, 0.0183, 0.0191, 0.0183, 0.0161, 0.0129 },
    { 0.0161, 0.0200, 0.0227, 0.0237, 0.0227, 0.0200, 0.0161 },
    { 0.0183, 0.0227, 0.0259, 0.0271, 0.0259, 0.0227, 0.0183 },
    { 0.0191, 0.0237, 0.0271, 0.0283, 0.0271, 0.0237, 0.0191 },
    { 0.0183, 0.0227, 0.0259, 0.0271, 0.0259, 0.0227, 0.0183 },
    { 0.0161, 0.0200, 0.0227, 0.0237, 0.0227, 0.0200, 0.0161 },
    { 0.0129, 0.0161, 0.0183, 0.0191, 0.0183, 0.0161, 0.0129 } };

const double ISPC::ControlAE::WEIGHT_7X7_B[7][7] = {
    { 0.0000, 0.0000, 0.0000, 0.0001, 0.0000, 0.0000, 0.0000 },
    { 0.0000, 0.0003, 0.0036, 0.0086, 0.0036, 0.0003, 0.0000 },
    { 0.0000, 0.0036, 0.0487, 0.1160, 0.0487, 0.0036, 0.0000 },
    { 0.0001, 0.0086, 0.1160, 0.2763, 0.1160, 0.0086, 0.0001 },
    { 0.0000, 0.0036, 0.0487, 0.1160, 0.0487, 0.0036, 0.0000 },
    { 0.0000, 0.0003, 0.0036, 0.0086, 0.0036, 0.0003, 0.0000 },
    { 0.0000, 0.0000, 0.0000, 0.0001, 0.0000, 0.0000, 0.0000 } };

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
const ISPC::ParamDefSingle<bool> ISPC::ControlAE::AE_FLICKER_AUTODETECT(
    "AE_FLICKER_AUTODETECT", true);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_FLICKER_FREQ(
    "AE_FLICKER_FREQ", 1.0, 200.0, 50.0);  // in Hz

const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_BRIGHTNESS(
    "AE_TARGET_BRIGHTNESS", -1.0, 1.0, 0.0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_UPDATE_SPEED(
    "AE_UPDATE_SPEED", 0.0, 1.0, 1.0);
const ISPC::ParamDef<double> ISPC::ControlAE::AE_BRACKET_SIZE(
    "AE_BRACKET_SIZE", 0.0, 1.0, 0.05);

// no initial limit by default
// limited to sensor caps at runtime
const ISPC::ParamDef<double> ISPC::ControlAE::AE_TARGET_GAIN(
    "AE_TARGET_GAIN",
    1.0,
    std::numeric_limits<double>::max(),
    std::numeric_limits<double>::max());

// no initial limit by default
// limited to sensor caps at runtime
const ISPC::ParamDef<double> ISPC::ControlAE::AE_MAX_GAIN(
    "AE_MAX_GAIN",
    1.0,
    std::numeric_limits<double>::max(),
    std::numeric_limits<double>::max());

// no initial limit by default
// limited to sensor caps at runtime
const ISPC::ParamDef<ISPC::ControlAE::microseconds_t>
ISPC::ControlAE::AE_MAX_EXPOSURE(
    "AE_MAX_EXPOSURE",
    1,
    std::numeric_limits<ISPC::ControlAE::microseconds_t>::max(),
    std::numeric_limits<ISPC::ControlAE::microseconds_t>::max());


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

/**
 * @brief replacement of floor() which seems to be available in GNU toolchain
 * but is not in Visual Studio below C++11
 * @note Works as round() for positive input values only
 */
static inline
double roundPositiveToNearest(const double val) {
    return floor(val+0.5);
}

double ISPC::ControlAE::getTargetAeGain() const {
    return ispc_min(fTargetAeGain, fMaxSensorGain);
}

void ISPC::ControlAE::setTargetAeGain(double targetAeGain) {
    const Sensor* sensor = getSensor();
    targetAeGain = ISPC::clip(targetAeGain,
            sensor->getMinGain(),
            sensor->getMaxGain());
    fTargetAeGain = targetAeGain;
    settingsUpdated = true;
}

double ISPC::ControlAE::getMaxAeGain() const {
#ifdef INFOTM_ISP
    double fGain = 0.0;

    pthread_mutex_lock((pthread_mutex_t*)&paramLock);
    if (bMaxManualAeGainEnable)
        fGain = ispc_min(fMaxManualAeGain, fMaxSensorGain);
    else
        fGain =  ispc_min(fMaxAeGain, fMaxSensorGain);
    pthread_mutex_unlock((pthread_mutex_t*)&paramLock);
    return fGain;
#else
    return ispc_min(fMaxAeGain, fMaxSensorGain);
#endif
}

void ISPC::ControlAE::setMaxAeGain(double maxAeGain) {
    const Sensor* sensor = getSensor();

#ifdef INFOTM_ISP
    pthread_mutex_lock((pthread_mutex_t*)&paramLock);
#endif
    maxAeGain = ISPC::clip(maxAeGain,
            sensor->getMinGain(),
            sensor->getMaxGain());
    fMaxAeGain = maxAeGain;
    settingsUpdated = true;
#ifdef INFOTM_ISP
    pthread_mutex_unlock((pthread_mutex_t*)&paramLock);
#endif
}

ISPC::ControlAE::microseconds_t
ISPC::ControlAE::getMaxAeExposure() const {
    return ispc_min(uiMaxAeExposure, uiMaxSensorExposure);
}

void ISPC::ControlAE::setMaxAeExposure(
        microseconds_t uiMaxAeExposure) {
    const Sensor* sensor = getSensor();
    uiMaxAeExposure = ISPC::clip(uiMaxAeExposure,
            sensor->getMinExposure(),
            sensor->getMaxExposure());
    this->uiMaxAeExposure = uiMaxAeExposure;
    settingsUpdated = true;
}

double ISPC::ControlAE::getFixedAeGain() const {
    return fFixedAeGain;
}

void ISPC::ControlAE::setFixedAeGain(double fixedAeGain) {
    const Sensor* sensor = getSensor();
    fixedAeGain = ISPC::clip(fixedAeGain,
            sensor->getMinGain(),
            sensor->getMaxGain());
    fFixedAeGain = fixedAeGain;
    settingsUpdated = true;
}

ISPC::ControlAE::microseconds_t
ISPC::ControlAE::getFixedAeExposure() const {
    return uiFixedAeExposure;
}

void ISPC::ControlAE::setFixedAeExposure(
        ISPC::ControlAE::microseconds_t uiFixedAeExposure) {
    const Sensor* sensor = getSensor();
    uiFixedAeExposure = ISPC::clip(uiFixedAeExposure,
            sensor->getMinExposure(),
            sensor->getMaxExposure());
    this->uiFixedAeExposure = uiFixedAeExposure;
    settingsUpdated = true;
}

#ifdef INFOTM_ISP
bool ISPC::ControlAE::isFixedAeGain(void)
{
    return useFixedAeGain;
}

#endif //INFOTM_ISP
void ISPC::ControlAE::enableFixedAeGain(bool enabled) {
    useFixedAeGain = enabled;
    settingsUpdated = true;
}

#ifdef INFOTM_ISP
bool ISPC::ControlAE::isFixedAeExposure(void)
{
    return useFixedAeExposure;
}

#endif //INFOTM_ISP
void ISPC::ControlAE::enableFixedAeExposure(bool enabled) {
    useFixedAeExposure = enabled;
    settingsUpdated = true;
}

std::ostream& ISPC::ControlAE::printState(std::ostream &os) const
{
    os << SAVE_A1 << getLoggingName() << ":" << std::endl;

    os << SAVE_A2 << "config:" << std::endl;
    os << SAVE_A2 << "enabled = " << enabled << std::endl;
    os << SAVE_A3 << "flickerRejection = " <<
        (int)flickerRejection << std::endl;
    os << SAVE_A3 << "autoFlickerRejection = " <<
        (int)autoFlickerRejection << std::endl;
    os << SAVE_A3 << "flickerFreqConfig = " <<
        flickerFreqConfig << std::endl;
    os << SAVE_A3 << "targetBrightness = " <<
        targetBrightness << std::endl;
    os << SAVE_A3 << "updateSpeed = " <<
        updateSpeed << std::endl;
    os << SAVE_A3 << "targetBracket = " << targetBracket << std::endl;
    os << SAVE_A3 << "fTargetAeGain = " << fTargetAeGain << std::endl;
    os << SAVE_A3 << "fMaxAeGain = " << fMaxAeGain << std::endl;
    os << SAVE_A3 << "uiMaxAeExposure = " << uiMaxAeExposure << std::endl;

    os << SAVE_A3 << "uiMaxSensorExposure = " <<
        uiMaxSensorExposure << std::endl;
    os << SAVE_A3 << "fSensorFrameDuration = " <<
        fSensorFrameDuration << std::endl;

    os << SAVE_A3 << "useFixedAeGain = " << (int)useFixedAeGain << std::endl;
    os << SAVE_A3 << "fFixedAeGain = " << fFixedAeGain << std::endl;
    os << SAVE_A3 << "useFixedAeExposure = " <<
        (int)useFixedAeExposure << std::endl;
    os << SAVE_A3 << "uiFixedAeExposure = " <<
        uiFixedAeExposure << std::endl;

    os << SAVE_A2 << "state:" << std::endl;
    os << SAVE_A3 << "hasConverged = " << hasConverged() << std::endl;
    os << SAVE_A3 << "currentBrightness = " << currentBrightness << std::endl;
    os << SAVE_A3 << "fNewGain = " << fNewGain << std::endl;
    os << SAVE_A3 << "uiNewExposure = " << uiNewExposure << std::endl;

    return os;
}

ISPC::ParameterGroup ISPC::ControlAE::getGroup()
{
    ParameterGroup group;

    group.header = "// Auto Exposure parameters";

    group.parameters.insert(AE_FLICKER.name);
    group.parameters.insert(AE_FLICKER_AUTODETECT.name);
    group.parameters.insert(AE_FLICKER_FREQ.name);
    group.parameters.insert(AE_TARGET_BRIGHTNESS.name);
    group.parameters.insert(AE_UPDATE_SPEED.name);
    group.parameters.insert(AE_BRACKET_SIZE.name);

    group.parameters.insert(AE_TARGET_GAIN.name);
    group.parameters.insert(AE_MAX_GAIN.name);
    group.parameters.insert(AE_MAX_EXPOSURE.name);

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
    currentBrightness(0.0),
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
    fMaxManualAeGain(AE_MAX_GAIN.def),
#endif //INFOTM_ISP
    updateSpeed(AE_UPDATE_SPEED.def),
    flickerRejection(AE_FLICKER.def),
    autoFlickerRejection(AE_FLICKER_AUTODETECT.def),
    autoFlickerDetectionSupported(false),
    flickerFreqConfig(AE_FLICKER_FREQ.def),
    flickerFreq(0.0),
    targetBracket(AE_BRACKET_SIZE.def),
    underexposed(false),
    configured(false),
    fMaxAeGain(AE_MAX_GAIN.def),
    // set to numeric max until read from sensor
    fMaxSensorGain(std::numeric_limits<double>::max()),
    fTargetAeGain(AE_TARGET_GAIN.def),
    uiMaxAeExposure(AE_MAX_EXPOSURE.def),
    //set to numeric max until read from sensor
    uiMaxSensorExposure(std::numeric_limits<microseconds_t>::max()),
    fNewGain(1.0),
    uiNewExposure(0),
    fFixedAeGain(0.0),
    useFixedAeGain(false),
    uiFixedAeExposure(0),
    useFixedAeExposure(false),
    doAE(true),
#ifdef INFOTM_SUPPORT_DAUL_SENSOR_AE_PATCH
    useDualAE(false),
    fBrightnessDiffThreshold(0.4),
    fGainDiffThreshold(3.0),
    fCurDaulBrightness({0.0}),
    fNewDaulGain({0.0}),
    fNewDaulExposure({0.0}),
#endif
    settingsUpdated(true)
{
#ifdef INFOTM_ISP
    pthread_mutex_init((pthread_mutex_t*)&paramLock, NULL);
#endif
}

IMG_RESULT ISPC::ControlAE::load(const ParameterList &parameters)
{
    LOG_PERF_IN();

    this->flickerRejection = parameters.getParameter(ControlAE::AE_FLICKER);
    this->autoFlickerRejection =
            parameters.getParameter(ControlAE::AE_FLICKER_AUTODETECT);
    this->flickerFreqConfig = parameters.getParameter(ControlAE::AE_FLICKER_FREQ);
    this->targetBrightness =
        parameters.getParameter(ControlAE::AE_TARGET_BRIGHTNESS);
    this->updateSpeed = parameters.getParameter(ControlAE::AE_UPDATE_SPEED);
    this->targetBracket = parameters.getParameter(ControlAE::AE_BRACKET_SIZE);

    this->fTargetAeGain = parameters.getParameter(ControlAE::AE_TARGET_GAIN);
    this->fMaxAeGain = parameters.getParameter(ControlAE::AE_MAX_GAIN);
    this->uiMaxAeExposure = parameters.getParameter(ControlAE::AE_MAX_EXPOSURE);

#ifdef INFOTM_ISP
    this->OritargetBrightness = this->targetBrightness;
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

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAE::save(ParameterList &parameters, SaveType t) const
{
    LOG_PERF_IN();

    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ControlAE::getGroup();
    }

    parameters.addGroup("ControlAE", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(AE_FLICKER, this->flickerRejection);
        parameters.addParameter(AE_FLICKER_AUTODETECT, this->autoFlickerRejection);
        parameters.addParameter(AE_FLICKER_FREQ, this->flickerFreqConfig);

        parameters.addParameter(AE_TARGET_BRIGHTNESS, this->targetBrightness);
        parameters.addParameter(AE_UPDATE_SPEED, this->updateSpeed);
        parameters.addParameter(AE_BRACKET_SIZE, this->targetBracket);

        parameters.addParameter(AE_TARGET_GAIN, getTargetAeGain());
        parameters.addParameter(AE_MAX_GAIN, getMaxAeGain());
        parameters.addParameter(AE_MAX_EXPOSURE, getMaxAeExposure());

#ifdef INFOTM_ISP
        parameters.addParameter(AE_ENABLE_MOVE_TARGET, this->enMoveAETarget);
        parameters.addParameter(AE_TARGET_MOVE_METHOD, this->ui32AeMoveMethod);
        parameters.addParameter(AE_TARGET_MAX, this->fAeTargetMax);
        parameters.addParameter(AE_TARGET_MIN, this->fAeTargetMin);
        parameters.addParameter(AE_TARGET_GAIN_NRML_MODE_MAX_LMT, this->fAeTargetGainNormalModeMaxLmt);
        parameters.addParameter(AE_TARGRT_MOVE_STEP, this->fAeTargetMoveStep);
        parameters.addParameter(AE_TARGET_MIN_LMT, this->fAeTargetMinLmt);
        parameters.addParameter(AE_TARGET_UP_OVER_THRESHOLD, this->fAeTargetUpOverThreshold);
        parameters.addParameter(AE_TARGET_UP_UNDER_THRESHOLD, this->fAeTargetUpUnderThreshold);
        parameters.addParameter(AE_TARGET_DN_OVER_THRESHOLD, this->fAeTargetDnOverThreshold);
        parameters.addParameter(AE_TARGET_DN_UNDER_THRESHOLD, this->fAeTargetDnUnderThreshold);

        parameters.addParameter(AE_TARGET_EXPOSURE_METHOD, this->ui32AeExposureMethod);
        parameters.addParameter(AE_TARGET_LOWLUX_GAIN_ENTER, this->fAeTargetLowluxGainEnter);
        parameters.addParameter(AE_TARGET_LOWLUX_GAIN_EXIT, this->fAeTargetLowluxGainExit);
        parameters.addParameter(AE_TARGET_LOWLUX_EXPOSURE_ENTER, this->ui32AeTargetLowluxExposureEnter);
        parameters.addParameter(AE_TARGET_LOWLUX_FPS, this->fAeTargetLowluxFPS);
        parameters.addParameter(AE_TARGET_NORMAL_FPS, this->fAeTargetNormalFPS);
        parameters.addParameter(AE_TARGET_MAX_FPS_LOCK_ENABLE, this->bAeTargetMaxFpsLockEnable);

        parameters.addParameter(AE_BRIGHTNESS_METERING_METHOD, this->ui32AeBrightnessMeteringMethod);
        parameters.addParameter(AE_REGION_DUCE, this->fAeRegionDuce);
        parameters.addParameter(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE, this->bAeForceCallSensorDriverEnable);
#endif //INFOTM_ISP
        break;

    case SAVE_MIN:
        parameters.addParameterMin(AE_FLICKER);  // bool does not have min
        parameters.addParameterMin(AE_FLICKER_AUTODETECT);
        parameters.addParameterMin(AE_FLICKER_FREQ);

        parameters.addParameterMin(AE_TARGET_BRIGHTNESS);
        parameters.addParameterMin(AE_UPDATE_SPEED);
        parameters.addParameterMin(AE_BRACKET_SIZE);

        parameters.addParameterMin(AE_TARGET_GAIN);
        parameters.addParameterMin(AE_MAX_GAIN);
        parameters.addParameterMin(AE_MAX_EXPOSURE);

#ifdef INFOTM_ISP
        parameters.addParameterMin(AE_TARGET_MOVE_METHOD);
        parameters.addParameterMin(AE_TARGET_MAX);
        parameters.addParameterMin(AE_TARGET_MIN);
        parameters.addParameterMin(AE_TARGET_GAIN_NRML_MODE_MAX_LMT);
        parameters.addParameterMin(AE_TARGRT_MOVE_STEP);
        parameters.addParameterMin(AE_TARGET_MIN_LMT);
        parameters.addParameterMin(AE_TARGET_UP_OVER_THRESHOLD);
        parameters.addParameterMin(AE_TARGET_UP_UNDER_THRESHOLD);
        parameters.addParameterMin(AE_TARGET_DN_OVER_THRESHOLD);
        parameters.addParameterMin(AE_TARGET_DN_UNDER_THRESHOLD);

        parameters.addParameterMin(AE_TARGET_EXPOSURE_METHOD);
        parameters.addParameterMin(AE_TARGET_LOWLUX_GAIN_ENTER);
        parameters.addParameterMin(AE_TARGET_LOWLUX_GAIN_EXIT);
        parameters.addParameterMin(AE_TARGET_LOWLUX_EXPOSURE_ENTER);
        parameters.addParameterMin(AE_TARGET_LOWLUX_FPS);
        parameters.addParameterMin(AE_TARGET_NORMAL_FPS);
        parameters.addParameterMin(AE_TARGET_MAX_FPS_LOCK_ENABLE);

        parameters.addParameterMin(AE_BRIGHTNESS_METERING_METHOD);
        parameters.addParameterMin(AE_REGION_DUCE);
        parameters.addParameterMin(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE);
#endif //INFOTM_ISP
        break;

    case SAVE_MAX:
        parameters.addParameterMax(AE_FLICKER);  // bool does not have max
        parameters.addParameterMax(AE_FLICKER_AUTODETECT);
        parameters.addParameterMax(AE_FLICKER_FREQ);

        parameters.addParameterMax(AE_TARGET_BRIGHTNESS);
        parameters.addParameterMax(AE_UPDATE_SPEED);
        parameters.addParameterMax(AE_BRACKET_SIZE);

        parameters.addParameterMax(AE_TARGET_GAIN);
        parameters.addParameterMax(AE_MAX_GAIN);
        parameters.addParameterMax(AE_MAX_EXPOSURE);

#ifdef INFOTM_ISP
        parameters.addParameterMax(AE_TARGET_MOVE_METHOD);
        parameters.addParameterMax(AE_TARGET_MAX);
        parameters.addParameterMax(AE_TARGET_MIN);
        parameters.addParameterMax(AE_TARGET_GAIN_NRML_MODE_MAX_LMT);
        parameters.addParameterMax(AE_TARGRT_MOVE_STEP);
        parameters.addParameterMax(AE_TARGET_MIN_LMT);
        parameters.addParameterMax(AE_TARGET_UP_OVER_THRESHOLD);
        parameters.addParameterMax(AE_TARGET_UP_UNDER_THRESHOLD);
        parameters.addParameterMax(AE_TARGET_DN_OVER_THRESHOLD);
        parameters.addParameterMax(AE_TARGET_DN_UNDER_THRESHOLD);

        parameters.addParameterMax(AE_TARGET_EXPOSURE_METHOD);
        parameters.addParameterMax(AE_TARGET_LOWLUX_GAIN_ENTER);
        parameters.addParameterMax(AE_TARGET_LOWLUX_GAIN_EXIT);
        parameters.addParameterMax(AE_TARGET_LOWLUX_EXPOSURE_ENTER);
        parameters.addParameterMax(AE_TARGET_LOWLUX_FPS);
        parameters.addParameterMax(AE_TARGET_NORMAL_FPS);
        parameters.addParameterMax(AE_TARGET_MAX_FPS_LOCK_ENABLE);

        parameters.addParameterMax(AE_BRIGHTNESS_METERING_METHOD);
        parameters.addParameterMax(AE_REGION_DUCE);
        parameters.addParameterMax(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE);
#endif //INFOTM_ISP
        break;

    case SAVE_DEF:
        parameters.addParameterDef(AE_FLICKER);
        parameters.addParameterDef(AE_FLICKER_AUTODETECT);
        parameters.addParameterDef(AE_FLICKER_FREQ);

        parameters.addParameterDef(AE_TARGET_BRIGHTNESS);
        parameters.addParameterDef(AE_UPDATE_SPEED);
        parameters.addParameterDef(AE_BRACKET_SIZE);

        parameters.addParameterDef(AE_TARGET_GAIN);
        parameters.addParameterDef(AE_MAX_GAIN);
        parameters.addParameterDef(AE_MAX_EXPOSURE);

#ifdef INFOTM_ISP
        parameters.addParameterDef(AE_TARGET_MOVE_METHOD);
        parameters.addParameterDef(AE_TARGET_MAX);
        parameters.addParameterDef(AE_TARGET_MIN);
        parameters.addParameterDef(AE_TARGET_GAIN_NRML_MODE_MAX_LMT);
        parameters.addParameterDef(AE_TARGRT_MOVE_STEP);
        parameters.addParameterDef(AE_TARGET_MIN_LMT);
        parameters.addParameterDef(AE_TARGET_UP_OVER_THRESHOLD);
        parameters.addParameterDef(AE_TARGET_UP_UNDER_THRESHOLD);
        parameters.addParameterDef(AE_TARGET_DN_OVER_THRESHOLD);
        parameters.addParameterDef(AE_TARGET_DN_UNDER_THRESHOLD);

        parameters.addParameterDef(AE_TARGET_EXPOSURE_METHOD);
        parameters.addParameterDef(AE_TARGET_LOWLUX_GAIN_ENTER);
        parameters.addParameterDef(AE_TARGET_LOWLUX_GAIN_EXIT);
        parameters.addParameterDef(AE_TARGET_LOWLUX_EXPOSURE_ENTER);
        parameters.addParameterDef(AE_TARGET_LOWLUX_FPS);
        parameters.addParameterDef(AE_TARGET_NORMAL_FPS);
        parameters.addParameterDef(AE_TARGET_MAX_FPS_LOCK_ENABLE);

        parameters.addParameterDef(AE_BRIGHTNESS_METERING_METHOD);
        parameters.addParameterDef(AE_REGION_DUCE);
        parameters.addParameterDef(AE_FORCE_CALL_SENSOR_DRIVER_ENABLE);
#endif //INFOTM_ISP
        break;
    }

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
ISPC::ControlAE::microseconds_t ISPC::ControlAE::getNewExposure(void)
{
	return uiNewExposure;
}

double ISPC::ControlAE::getNewGain(void)
{
	return fNewGain;
}
#endif //INFOTM_ISP

//update configuration based no a previous shot metadata
IMG_RESULT ISPC::ControlAE::update(const Metadata &metadata)
{
    LOG_PERF_IN();
    currentBrightness =
        getBrightnessMetering(metadata.histogramStats, metadata.clippingStats);

#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("Metered/Target brightness: %f/%f. Flicker Rejection:%s\n",
        currentBrightness, targetBrightness,
        (flickerRejection?"Enabled":"Disabled"));
#endif

#ifdef INFOTM_SUPPORT_DAUL_SENSOR_AE_PATCH
    fCurDaulBrightness[0] = getDualBrightnessMetering(metadata.histogramStats, metadata.clippingStats, true);
    fCurDaulBrightness[1] = getDualBrightnessMetering(metadata.histogramStats, metadata.clippingStats, false);

    if (fabs(fCurDaulBrightness[0] - fCurDaulBrightness[1]) >= fBrightnessDiffThreshold)
    {
            for (int m = 0, m < 2; m++)
            {
                getDualAutoExposure(
                    sensor->getExposure()*sensor->getDualGain(m),
                    sensor->getMinExposure(),
                    dbExposureMax,
                    sensor->getMinGain(),
                    getMaxAeGain(),
                    fNewDaulGain[m],
                    fNewDaulExposure[m]);
            }

            if (fabs(fNewDaulGain[0] - fNewDaulGain[1]) >= fGainDiffThreshold)
            {
                useDualAE = true;
   	        if (doAE)
	        {
	            programCorrection();
	        }
                settingsUpdated = false;
            }
	    else
            {
                useDualAE = false;
            }
    }
#endif

    const Sensor *sensor = getSensor();
    if (!sensor)
    {
        MOD_LOG_ERROR("ControlAE owner has no sensors!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

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
					if (hasConverged())
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

				//printf("Ratio_OverExp = %f \n", Ratio_OverExp);
				//printf("getTargetBrightness = %f \n", getTargetBrightness());
	}
#endif //end of INFOTM
    /* If pipeline statistics extraction have not been configured
     * then configure them and wait for the next iteration to update
     * the sensor parameters */
    if (!configured)
    {
        MOD_LOG_WARNING("ControlAE Statistics were not configured! Trying "\
            "to configure them now.\n");
        configureStatistics();
    }
    else
    {

        if (autoFlickerDetectionSupported &&
            autoFlickerRejection )
        {
            if (FLD_50HZ == metadata.flickerStats.flickerStatus)
            {
                flickerFreq = 50.0;
            }
            else if (FLD_60HZ == metadata.flickerStats.flickerStatus)
            {
                flickerFreq = 60.0;
            }
            else
            if (FLD_NO_FLICKER == metadata.flickerStats.flickerStatus ||
                FLD_UNDETERMINED == metadata.flickerStats.flickerStatus)
            {
                flickerFreq = 0.0;
            }
        } else {
            // use configuration value
            flickerFreq = flickerFreqConfig;
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
		if (settingsUpdated || !hasConverged() || AeForceUpdate) //Run only if we are out of the acceptable distance (targetBracket) from target brightness
#else
		// apply settings for upcoming frame
		if (settingsUpdated || !hasConverged())
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
			{
				static double dbFrameRatePrev = 0;

				if (dbFrameRatePrev != flFrameRate) {
					// set the limit of exposure without dropping FPS
					fSensorFrameDuration = 1000000.0 / flFrameRate;
					dbFrameRatePrev = flFrameRate;
				}
			}
#endif //INFOTM_ISP

            /* Calculate the new gain and exposure values based on current
             * & target brightness, sensor configuration, flicker
             * rejection, etc. */
            getAutoExposure(
                sensor->getExposure()*sensor->getGain(),  	//fPrevExposure
                sensor->getMinExposure(),					//fMinExposure
#ifndef INFOTM_ISP
                getMaxAeExposure(),							//fMaxExposure
#else
                (microseconds_t)dbExposureMax,				//fMaxExposure
#endif //INFOTM_ISP
                sensor->getMinGain(),						//minGain
                getMaxAeGain(),								//maxGain
                fNewGain,
                uiNewExposure);

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
                static microseconds_t ui32ExposureBackup = 0;
                static double fGainBackup = 0;

                if ((ui32ExposureBackup != uiNewExposure) || (fGainBackup != fNewGain) || bAeForceCallSensorDriverEnable)
                {
  #if defined(INFOTM_ISP_EXPOSURE_GAIN_DEBUG)
                    printf("curBrightness %f, targetBrightness %f, fps %f, exposure/gain: %d/%f, max_gain:%f\n", currentBrightness, targetBrightness, flFrameRate, uiNewExposure, fNewGain, getMaxAeGain());
  #endif
                    programCorrection();
                    ui32ExposureBackup = uiNewExposure;
                    fGainBackup = fNewGain;
                }
#else
                programCorrection();
#endif //INFOTM_ISP
            }
            settingsUpdated = false;
        }
#if VERBOSE_CONTROL_MODULES
        else
        {
            MOD_LOG_INFO("Sensor exposure/gain: %ld/%f.\n",
                sensor->getExposure(), sensor->getGain());
        }
#endif
    }

    LOG_PERF_OUT();
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAE::configureStatistics()
{
    ModuleHIS *pHIS = NULL;
    ModuleFLD *pFLD = NULL;

    if (getPipelineOwner())
    {
        pHIS = getPipelineOwner()->getModule<ModuleHIS>();
#ifndef INFOTM_ISP
        // FLD is not corrected for AE , so removed not use.
        pFLD = getPipelineOwner()->getModule<ModuleFLD>();
#endif

#ifdef INFOTM_LANCHOU_PROJECT
		pFLD = getPipelineOwner()->getModule<ModuleFLD>();
#endif

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
        Sensor *sensor = getSensor();
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

            tileWidth = IMG_MAX_INT(tileWidth, HIS_MIN_TILE_WIDTH);
            tileHeight = IMG_MAX_INT(tileHeight, HIS_MIN_TILE_HEIGHT);

            pHIS->aGridStartCoord[0] = marginWidth / 2;
            pHIS->aGridStartCoord[1] = marginHeight / 2;
            pHIS->aGridTileSize[0] = tileWidth;
            pHIS->aGridTileSize[1] = tileHeight;

            pHIS->requestUpdate();
        }
        // set the limit of exposure without dropping FPS
        fSensorFrameDuration = 1000000.0 / sensor->flFrameRate;
        fMaxSensorGain = sensor->getMaxGain();
        uiMaxSensorExposure =
            static_cast<microseconds_t>(sensor->getMaxExposure());

        // configure FLD for flicker detection
        if(pFLD) {
            autoFlickerDetectionSupported = true;
            SENSOR_HANDLE handle = sensor->getHandle();
            SENSOR_INFO sensorMode;
            Sensor_GetInfo(handle, &sensorMode);
            // setup flicker detector with sensor config
            pFLD->iVTotal =sensorMode.sMode.ui16VerticalTotal;
            pFLD->fFrameRate = sensorMode.sMode.flFrameRate;
            pFLD->bEnable = true;
            pFLD->setup();
            pFLD->requestUpdate();
        } else {
            LOG_INFO("FLD module not configured. "\
                    "Flicker detection not supported!\n");
        }
        configured = true;
        settingsUpdated = true;
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

#ifdef INFOTM_SUPPORT_DAUL_SENSOR_AE_PATCH
    if (useDualAE)
    {
        sensor->SetDualGainAndExposure(fNewDaulGain, fNewDaulExposure);
        return IMG_SUCCESS;
    }
#endif

#ifdef INFOTM_ISP
    sensor->SetGainAndExposure(fNewGain, uiNewExposure);
#else
	//we changed the set gain and exposure time call function.
    sensor->setExposure(uiNewExposure);
    sensor->setGain(fNewGain);
#endif //INFOTM_ISP

#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("Change exposure/gain: %d/%f - programmed %d/%f\n",
        uiNewExposure, fNewGain, sensor->getExposure(), sensor->getGain());
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
    settingsUpdated = true;
}

double ISPC::ControlAE::getUpdateSpeed() const
{
    return updateSpeed;
}

void ISPC::ControlAE::setTargetBrightness(double value)
{
    targetBrightness = value;
    settingsUpdated = true;
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
        flickerFreqConfig = freq;
        // disable auto if frequency provided
        autoFlickerRejection = false;
    } else {
        autoFlickerRejection = true;
    }
    settingsUpdated = true;
}

void ISPC::ControlAE::enableAutoFlickerRejection(bool enable)
{
    autoFlickerRejection = enable;
    settingsUpdated = true;
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
    if(autoFlickerRejection) {
        // need this logic because the config value is copied in first
        // update(), but the getter may be called earlier than that
        return flickerFreq;
    } else {
        return flickerFreqConfig;
    }
}

double ISPC::ControlAE::getFlickerFreqConfig() const
{
    return flickerFreqConfig;
}

bool ISPC::ControlAE::getAutoFlickerRejection() const
{
    return autoFlickerRejection;
}

void ISPC::ControlAE::setTargetBracket(double value)
{
    targetBracket = value;
    settingsUpdated = true;
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
#ifdef USE_MATH_NEON
        overExposureF +=(double)powf_neon((float)position,3.0f)*nHistogram[i];
        underExposureF +=(double)powf_neon((float)(1.0-position),3.0f)*nHistogram[i];
#else
        overExposureF += pow(position, 3)*nHistogram[i];
        underExposureF += pow(1.0 - position, 3)*nHistogram[i];
#endif
    }
    avgValue = avgValue - 0.5;
}

void ISPC::ControlAE::normalizeHistogram(const IMG_UINT32 sourceH[],
    double destinationH[], int nBins)
{
    double totalHistogram = 0.0;
    int i;

    for (i = 0; i < nBins; i++)
    {
        totalHistogram += sourceH[i];
    }

    if (0.0 != totalHistogram)
    {
        for (i = 0; i < nBins; i++)
        {
            destinationH[i] = double(sourceH[i]) / totalHistogram;
        }
    }
    else
    {
        MOD_LOG_DEBUG("Total histogram is 0!\n");
        for (i = 0; i < nBins; i++)
        {
            destinationH[i] = 0.0;
        }
    }
}

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
#ifdef INFOTM_ISP
    setTargetBrightness(value);
#endif //INFOTM_ISP
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

    pthread_mutex_lock((pthread_mutex_t*)&paramLock);
    fGain = ispc_min(fMaxAeGain, fMaxSensorGain);
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
            normalizeHistogram(histogram.regionHistograms[i][j], nRegHistogram, HIS_REGION_BINS);
            getHistogramStats(nRegHistogram,
                              HIS_REGION_BINS,
                              regionBrightness[i][j],
                              regionUnderExp[i][j],
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
    backLightAccumulation = ISPC::clip(backLightAccumulation, 0, 1);

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

	double finalMetering = overUnderExposureDiff*0.2 + centreWeightedExposure * 0.80;
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

    double finalMetering = overUnderExposureDiff*0.2 + centreWeightedExposure * 0.80;
    finalMetering = (finalMetering * 0.25 + foregroundAvg * 0.75)
        * backLightAccumulation + finalMetering*(1.0 - backLightAccumulation);
#endif //INFOTM_ISP
    return finalMetering;
}

#ifdef INFOTM_SUPPORT_DAUL_SENSOR_AE_PATCH
void ISPC::ControlAE::getDualBackLightMeasure(double gridStats[HIS_REGION_VTILES][HIS_REGION_HTILES], double &foregroundAvg, double &backgroundAvg, bool bLeftRegion)
{
    double nForeground = 0;
    double totalForeground = 0;
    double totalBackground = 0;

    int VMin = 0, VMax = HIS_REGION_VTILES, HMin = 0, HMax = 3;

    if (!bLeftRegion)
    {
         HMin = 4;
         HMax = HIS_REGION_HTILES;
    }

    for (int i = VMin; i < VMax; i++)
    {
        for (int j = HMin; j < HMax; j++)
        {
            totalForeground += gridStats[i][j] * BACKLIGHT_7X7[i][j];
            totalBackground += gridStats[i][j] * (1.0 - BACKLIGHT_7X7[i][j]);
            nForeground += BACKLIGHT_7X7[i][j];
        }
    }

    foregroundAvg = totalForeground / nForeground;
    backgroundAvg = totalBackground / (21 - nForeground);
}

double ISPC::ControlAE::getDualWeightedStatsBlended(double gridStats[HIS_REGION_VTILES][HIS_REGION_HTILES], double combinationWeight, bool bLeftRegion)
{
    double totalWeight = 0;

    int VMin = 0, VMax = HIS_REGION_VTILES, HMin = 0, HMax = 3;

    if (!bLeftRegion)
    {
         HMin = 4;
         HMax = HIS_REGION_HTILES;
    }

    for (int i = VMin; i < VMax; i++)
    {
        for (int j = HMin; j < HMax; j++)
        {
            totalWeight += WEIGHT_7X7_A[i][j] * gridStats[i][j]
                * combinationWeight + WEIGHT_7X7_B[i][j] * gridStats[i][j]
                * (1 - combinationWeight);
        }
    }
    return totalWeight;
}

double ISPC::ControlAE::getDualBrightnessMetering(const MC_STATS_HIS &histogram, const MC_STATS_EXS &expClipping, bool bLeftRegion)
{
    double totalSamples = 0;
    int VMin = 0, VMax = HIS_REGION_VTILES, HMin = 0, HMax = 3;
    double SingleOverUnderExpDiff = 0;
    double SingleRatio_OverExp = 0;
    double SingleRatio_MidOverExp = 0;
    double SingleRatio_UnderExp = 0;

    if (!bLeftRegion)
    {
         HMin = 4;
         HMax = HIS_REGION_HTILES;
    }

    // Compute statistics for each tile in the 7x7 grid
    for (int i = VMin; i < VMax; i++)
    {
        for (int j = HMin; j < HMax; j++)
        {
            double nRegHistogram[HIS_REGION_BINS];
            normalizeHistogram(histogram.regionHistograms[i][j], nRegHistogram, HIS_REGION_BINS);

            for (int k = 13; k<HIS_REGION_BINS; k++)
            {
                SingleRatio_OverExp += nRegHistogram[k];
                SingleRatio_UnderExp += nRegHistogram[HIS_REGION_BINS-1-k];
            }

           for (int k = 7; k<13; k++)
           {
               SingleRatio_MidOverExp += nRegHistogram[k];
           }
        }
    }

    this->Ratio_MidOverExp /= 21;
    this->Ratio_OverExp /= 21;
    this->Ratio_UnderExp /= 21;

    /* Estimate the backlight by comparing background vs foreground
     * average illumination */
    double foregroundAvg, backgroundAvg;
    getDualBackLightMeasure(regionRatio, foregroundAvg, backgroundAvg, bLeftRegion);

    double backlightDifference = ispc_min(1.0, fabs(foregroundAvg - backgroundAvg));

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
    backLightAccumulation = ISPC::clip(backLightAccumulation, 0, 1);

    // Get the maximum values for over and under exposed tiles in the grid
    double overExposure = 0;
    double underExposure = 0;
    for (int i = VMin; i < VMax; i++)
    {
        for (int j = HMin; j < HMax; j++)
        {
            overExposure = ispc_max(overExposure, regionOverExp[i][j]);
            underExposure = ispc_max(underExposure, regionUnderExp[i][j]);
        }
    }

    double overUnderExposureDiff = overExposure - underExposure;
    SingleOverUnderExpDiff = overUnderExposureDiff;

    /* Get weighted exposure value using the grid and the 7x7
     * coefficient matrices */

#ifdef INFOTM_ISP
    //Get weighted exposure value using the grid and the 7x7 coefficient matrices
    double centreWeightedExposure;

    {
        centreWeightedExposure = getDualWeightedStatsBlended(regionRatio, fAeRegionDuce, bLeftRegion);
    }

    //printf("centreWeightedExposure region_duce = %f\n", fAeRegionDuce);

    double finalMetering = overUnderExposureDiff*0.2 + centreWeightedExposure * 0.80;

    finalMetering = overUnderExposureDiff * 0.20 + centreWeightedExposure * 0.80;
#else
    double centreWeightedExposure = getDualWeightedStatsBlended(regionRatio, 0.65, bLeftRegion);
    double finalMetering = overUnderExposureDiff*0.2 + centreWeightedExposure * 0.80;

    finalMetering = (finalMetering * 0.25 + foregroundAvg * 0.75) * backLightAccumulation + finalMetering*(1.0 - backLightAccumulation);
#endif //INFOTM_ISP

    return finalMetering;
}

void ISPC::ControlAE::getDualAutoExposure(double fPrevExposure, microseconds_t fMinExposure, microseconds_t fMaxExposure, double minGain, double maxGain, bool bLeftRegion, double &newGain, microseconds_t &newExposure)
{
    microseconds_t intermediateExposure;
    double intermediateGain;
    int nIndex = 0;

    if (!bLeftRegion)
    {
        nIndex = 1;
    }

    /* First calculate the desired combined exposure to get closer
     * to the target brightness metering */
    double targetExposure = autoExposureCtrl(fCurDaulBrightness[nIndex], targetBrightness, fPrevExposure);

    IMG_ASSERT(targetExposure>=0.0);

    /* semi-automatic exposure/gain control (exposure and/or gain could
     * be manually set, no flicker rejection) */
    if (useFixedAeGain || useFixedAeExposure)
    {
        if (useFixedAeExposure)
        {
            intermediateExposure = uiFixedAeExposure;
            intermediateGain = getGainFixedExposure(targetExposure, uiFixedAeExposure);
        } else if (useFixedAeGain)
        {
            intermediateGain = fFixedAeGain;
            intermediateExposure =  getExposureFixedGain(targetExposure, fFixedAeGain);
        } else /* (fixedAeGain && useFixedAeExposure)*/
        {
            intermediateExposure = uiFixedAeExposure;
            intermediateGain = fFixedAeGain;
        }
    }
    else
    {
        /* Automatic exposure and gain control (with optional flicker
         * rejection
         * Calculate the exposure time and gain values based on
         * anti-flicker detection */

        const bool handleFlicker = (flickerFreq>0.0 && flickerRejection);
        double flFlickerPeriod = 0.0;

        microseconds_t uiTargetAeExposure;
        microseconds_t maxAeExposure;

        // init related variables
        if(handleFlicker)
        {
            IMG_ASSERT(flickerFreq>0);

            flFlickerPeriod = 1000000.0 / (flickerFreq * 2);

            // integer number of flicker periods
            uiTargetAeExposure = static_cast<microseconds_t>(
                floor(fSensorFrameDuration / flFlickerPeriod ) *
                flFlickerPeriod);

            maxAeExposure = static_cast<microseconds_t>(floor(static_cast<double>(uiMaxAeExposure) / flFlickerPeriod) *flFlickerPeriod);
        }
        else
        {
            uiTargetAeExposure = static_cast<microseconds_t>(fSensorFrameDuration);
            maxAeExposure = uiMaxAeExposure;
        }

        /* Zone 0 */
        if (!handleFlicker ||
            targetExposure < flFlickerPeriod)
        {
            /* We are in Zone 0 so we set gain to 1.0 and the exact
             * exposure time we want.*/
            intermediateGain = 1.0;
            intermediateExposure =static_cast<microseconds_t>(roundPositiveToNearest(targetExposure));
        }
        else
        {
            // Zone 1
            // flicker enabled + flicker freq is meaningful
            // apply flicker rejection
            IMG_ASSERT(flFlickerPeriod>0);
            getFlickerExposure(flFlickerPeriod, targetExposure, fMaxExposure, intermediateGain, intermediateExposure);
        }

        if(intermediateExposure > uiTargetAeExposure)
        {
            // try next level (linear gain):
            // - exposure is target flicker periods
            // - gain grows linearly up to fTargetAeGain
            intermediateExposure = uiTargetAeExposure;
            intermediateGain = getGainFixedExposure(targetExposure, intermediateExposure);

            if(intermediateGain > fTargetAeGain)
            {
                // try next level (fps reduction):
                if (handleFlicker)
                {
                    // get base exposure for given target exp and gain
                    const double rebasedTarget = targetExposure/fTargetAeGain;
                    // get next flicker step above ref. exposure
                    // this value will be rounded in getFlickerExposure()
                    const double rebasedTargetFp = rebasedTarget+flFlickerPeriod;
                    // obtain gain difference
                    const double rebaseGain = getGainFixedExposure(rebasedTargetFp, static_cast<microseconds_t>(rebasedTarget));

                    double gainCorrection;
                    getFlickerExposure(flFlickerPeriod,
                        rebasedTargetFp,
                        fMaxExposure,
                        gainCorrection,
                        intermediateExposure);
                    // calc new gain after stepping up with exposure
                    intermediateGain = fTargetAeGain/rebaseGain*gainCorrection;
                }
                else
                {
                    // linear exposure
                    intermediateGain = fTargetAeGain;
                    intermediateExposure =
                        getExposureFixedGain(targetExposure,
                                intermediateGain);
                }

                if(intermediateExposure > maxAeExposure)
                {
                    // try next level :
                    // - max AE exposure
                    // - gain grows linearly up to fMaxAeGain
                    intermediateExposure = maxAeExposure;
                    intermediateGain =
                        getGainFixedExposure(targetExposure,
                            intermediateExposure);
                }
                // no more tracking here
            }
        }
        // clip gain later, but before detecting underexposure
    }
    microseconds_t correctedExposure;
    double correctedGain;

    adjustExposureStep(
        fMinExposure,  // step
        fMinExposure,  // min
        fMaxExposure,  // max
        intermediateExposure,
        intermediateGain,
        correctedExposure,
        correctedGain);

    // check gain only as it is the last limit of AE
    if (correctedGain > maxGain)
    {
        /* if the desired gain lie outside limits
         * set as underexposed (usable for enabling flash light) */
        underexposed = true;
    } else {
        underexposed = false;
    }

    correctedExposure = ISPC::clip(correctedExposure,
        fMinExposure,
        fMaxExposure);

    if(useFixedAeGain) {
        // no correction if fixed gain
        correctedGain = fFixedAeGain;
    } else {
        correctedGain = ISPC::clip(correctedGain,
                minGain,
                maxGain);
    }
    newGain = correctedGain;
    newExposure = correctedExposure;
}
#endif

bool ISPC::ControlAE::hasConverged() const
{
    if (fabs(targetBrightness - currentBrightness) < targetBracket)
    {
        return true;
    }
    return false;
}

bool ISPC::ControlAE::isUnderexposed(void) const
{
    return underexposed;
}

double ISPC::ControlAE::autoExposureCtrl(
    double brightnessMetering,
    double brightnessTarget,
    double prevExposure)
{
    double brightnessError = brightnessTarget - brightnessMetering;
    brightnessError = ISPC::clip(brightnessError, -1.0, 1.0);

    double newExposureValue = prevExposure;

    if (0.0 != brightnessError)
    {
        // the magic happens here
        double errorSign = brightnessError / fabs(brightnessError);
#ifdef USE_MATH_NEON
        newExposureValue = prevExposure * (1 + errorSign
            * (double)powf_neon((float)fabs(brightnessError), 1.25f) * updateSpeed);
#else
        newExposureValue = prevExposure * (1 + errorSign * pow(fabs(brightnessError), 1.25) * updateSpeed);
#endif
    }
    //printf("===>brightnessTarget %f, brightnessMetering %f, newExposureValue %f\n", brightnessTarget, brightnessMetering, newExposureValue);
    return newExposureValue;
}

void ISPC::ControlAE::getFlickerExposure(
    double flFlickerPeriod,
    double flRequested,
    double flMax,
    double &flGain,
    microseconds_t &flickerExposure)
{

//#ifndef INFOTM_ISP
//    flFlickerPeriod = 1000000 / (flFlickerHz);
//#else
//    flFlickerPeriod = 1000000/(flFlickerHz * 2);
//#endif //INFOTM_ISP
#if defined(INFOTM_ENABLE_AE_DEBUG)
    printf("Calculating flicker exposure for requested exposure %f,\n flicker period = %f\n", flRequested, flFlickerPeriod);
#endif  //INFOTM_ENABLE_AE_DEBUG
    // we can compensate for this flicker.
    /* set our exposure to be an integer number of mains cycles,
     * and adjust gain to compensate
     **/
    int exposurecycles =
        static_cast<int>(flRequested / flFlickerPeriod);
    double flexposuretime = exposurecycles*flFlickerPeriod;

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
    flickerExposure = static_cast<microseconds_t>(flexposuretime);
    flGain = flRequested / flexposuretime;
#if VERBOSE_CONTROL_MODULES
    LOG_DEBUG("Flicker correction applied, %d cycles, "\
        "requested=%f new exposure=%f\n",
        exposurecycles, flRequested, flexposuretime);
#endif

#if defined(INFOTM_ENABLE_AE_DEBUG)
        printf("=== flRequested = %f, flFlickerPeriod = %f, exposurecycles = %d, flexposuretime = %f\n",
            flRequested,
            flFlickerPeriod,
            exposurecycles,
            flexposuretime
            );
#endif //INFOTM_ENABLE_AE_DEBUG

   return;
}

void ISPC::ControlAE::adjustExposureStep(
    microseconds_t  exposureStep,
    microseconds_t minExposure,
    microseconds_t maxExposure,
    microseconds_t targetExposure,
    double targetGain,
    microseconds_t &correctedExposure,
    double &correctedGain)
{
    double additionalGain;

    IMG_ASSERT(0 != exposureStep);
    // make exposure an integer number of steps, floor
    correctedExposure = (targetExposure / exposureStep)*exposureStep;
    // adjust to exposureStep if lower
    correctedExposure = ispc_max(exposureStep, correctedExposure);
    // clip to max/min if needed
    correctedExposure = ISPC::clip(correctedExposure, minExposure, maxExposure);

    if (correctedExposure > 0)
    {
        additionalGain = static_cast<double>(targetExposure) / correctedExposure;
    }
    else
    {
        // always false but just in case..
        additionalGain = 1.0;
    }

    correctedGain = targetGain*additionalGain;
}

ISPC::ControlAE::microseconds_t
ISPC::ControlAE::getExposureFixedGain (
        const double targetExposure,
        const double fixedGain) const {
    return static_cast<microseconds_t>(
            roundPositiveToNearest(targetExposure / fixedGain));
}

double ISPC::ControlAE::getGainFixedExposure (
        const double targetExposure,
        const ISPC::ControlAE::microseconds_t fixedExposure)  const {
    return targetExposure / fixedExposure;
}

void ISPC::ControlAE::getAutoExposure(
    double fPrevExposure,
    microseconds_t fMinExposure,
    microseconds_t fMaxExposure,
    double minGain,
    double maxGain,
    double &newGain,
    microseconds_t &newExposure)
{
    microseconds_t intermediateExposure;
    double intermediateGain;

    /* First calculate the desired combined exposure to get closer
     * to the target brightness metering */
    double targetExposure = autoExposureCtrl(currentBrightness,
        targetBrightness, fPrevExposure);
    IMG_ASSERT(targetExposure>=0.0);

    /* semi-automatic exposure/gain control (exposure and/or gain could
     * be manually set, no flicker rejection) */
    if (useFixedAeGain || useFixedAeExposure)
    {
        if (useFixedAeExposure)
        {
            intermediateExposure = uiFixedAeExposure;
            intermediateGain =
                getGainFixedExposure(targetExposure, uiFixedAeExposure);
        } else if (useFixedAeGain)
        {
            intermediateGain = fFixedAeGain;
            intermediateExposure =
                getExposureFixedGain(targetExposure, fFixedAeGain);
        } else /* (fixedAeGain && useFixedAeExposure)*/
        {
            intermediateExposure = uiFixedAeExposure;
            intermediateGain = fFixedAeGain;
        }
    }
    else
    {
        /* Automatic exposure and gain control (with optional flicker
         * rejection
         * Calculate the exposure time and gain values based on
         * anti-flicker detection */

        const bool handleFlicker = (flickerFreq>0.0 && flickerRejection);
        double flFlickerPeriod = 0.0;

        microseconds_t uiTargetAeExposure;
        microseconds_t maxAeExposure;

        // init related variables
        if(handleFlicker)
        {
            IMG_ASSERT(flickerFreq>0);
#ifdef INFOTM_ISP
            flFlickerPeriod = 1000000.0 / (flickerFreq * 2);
            //printf("flickerFreq = %f flFlickerPeriod = %f @@@@@@@@@@@@@@@@@@@@\n", flickerFreq, flFlickerPeriod);
#else
            flFlickerPeriod = 1000000.0 / flickerFreq;
#endif
            // integer number of flicker periods
            uiTargetAeExposure = static_cast<microseconds_t>(
                floor(fSensorFrameDuration / flFlickerPeriod ) *
                flFlickerPeriod);

            maxAeExposure = static_cast<microseconds_t>(
                floor(static_cast<double>(uiMaxAeExposure) / flFlickerPeriod) *
                flFlickerPeriod);
#if defined(INFOTM_ENABLE_AE_DEBUG)
            printf("Flicker exposure = %d, flickerHz = %f\n", flickerExposure, fFlickerHz);
#endif //INFOTM_ENABLE_AE_DEBUG
        }
        else
        {
            uiTargetAeExposure =
                static_cast<microseconds_t>(fSensorFrameDuration);
            maxAeExposure = uiMaxAeExposure;
        }

        /* Zone 0 */
        if (!handleFlicker ||
            targetExposure < flFlickerPeriod)
        {
            /* We are in Zone 0 so we set gain to 1.0 and the exact
             * exposure time we want.*/
            intermediateGain = 1.0;
            intermediateExposure =
                static_cast<microseconds_t>(
                        roundPositiveToNearest(targetExposure));
        }
        else
        {
            // Zone 1
            // flicker enabled + flicker freq is meaningful
            // apply flicker rejection
            IMG_ASSERT(flFlickerPeriod>0);
            getFlickerExposure(flFlickerPeriod, targetExposure, fMaxExposure,
                    intermediateGain, intermediateExposure);
        }

        if(intermediateExposure > uiTargetAeExposure)
        {
            // try next level (linear gain):
            // - exposure is target flicker periods
            // - gain grows linearly up to fTargetAeGain
            intermediateExposure = uiTargetAeExposure;
            intermediateGain =
                getGainFixedExposure(targetExposure,
                        intermediateExposure);

            if(intermediateGain > fTargetAeGain)
            {
                // try next level (fps reduction):
                if (handleFlicker)
                {
                    // get base exposure for given target exp and gain
                    const double rebasedTarget =
                        targetExposure/fTargetAeGain;
                    // get next flicker step above ref. exposure
                    // this value will be rounded in getFlickerExposure()
                    const double rebasedTargetFp =
                        rebasedTarget+flFlickerPeriod;
                    // obtain gain difference
                    const double rebaseGain =
                        getGainFixedExposure(rebasedTargetFp,
                            static_cast<microseconds_t>(rebasedTarget));

                    double gainCorrection;
                    getFlickerExposure(flFlickerPeriod,
                        rebasedTargetFp,
                        fMaxExposure,
                        gainCorrection,
                        intermediateExposure);
                    // calc new gain after stepping up with exposure
                    intermediateGain = fTargetAeGain/rebaseGain*gainCorrection;
                }
                else
                {
                    // linear exposure
                    intermediateGain = fTargetAeGain;
                    intermediateExposure =
                        getExposureFixedGain(targetExposure,
                                intermediateGain);
                }

                if(intermediateExposure > maxAeExposure)
                {
                    // try next level :
                    // - max AE exposure
                    // - gain grows linearly up to fMaxAeGain
                    intermediateExposure = maxAeExposure;
                    intermediateGain =
                        getGainFixedExposure(targetExposure,
                            intermediateExposure);
                }
                // no more tracking here
            }
        }
        // clip gain later, but before detecting underexposure
    }
    microseconds_t correctedExposure;
    double correctedGain;

    adjustExposureStep(
        fMinExposure,  // step
        fMinExposure,  // min
        fMaxExposure,  // max
        intermediateExposure,
        intermediateGain,
        correctedExposure,
        correctedGain);

    // check gain only as it is the last limit of AE
    if (correctedGain > maxGain)
    {
        /* if the desired gain lie outside limits
         * set as underexposed (usable for enabling flash light) */
        underexposed = true;
    } else {
        underexposed = false;
    }

    correctedExposure = ISPC::clip(correctedExposure,
        fMinExposure,
        fMaxExposure);

    if(useFixedAeGain) {
        // no correction if fixed gain
        correctedGain = fFixedAeGain;
    } else {
        correctedGain = ISPC::clip(correctedGain,
                minGain,
                maxGain);
    }
    newGain = correctedGain;
    newExposure = correctedExposure;
}
