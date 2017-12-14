/**
*******************************************************************************
@file ControlAWB.cpp

@brief ISPC::ControlAWB implementation

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
#include "ispc/ControlAWB.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_AWB"

#include <cmath>
#include <string>
#include <list>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "ispc/ModuleWBS.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ColorCorrection.h"
#include "ispc/Sensor.h"
#include "ispc/Pipeline.h"
#include "ispc/ISPCDefs.h"

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

const ISPC::ParamDef<float> ISPC::ControlAWB::AWB_ESTIMATION_SCALE(
    "WB_ESTIMATION_SCALE", 0.0f, 20.0f, 1.0f);
const ISPC::ParamDef<float> ISPC::ControlAWB::AWB_ESTIMATION_OFFSET(
    "WB_ESTIMATION_OFFSET", -6500.0f, 6500.0f, 0.0f);
const ISPC::ParamDef<float> ISPC::ControlAWB::AWB_TARGET_TEMPERATURE(
    "WB_TARGET_TEMPERATURE", 0.0f, AWB_MAX_TEMP, 6500.0f);
const ISPC::ParamDef<float> ISPC::ControlAWB::AWB_TARGET_PIXELRATIO(
    "WB_TARGET_PIXEL_RATIO", 0.0f, 1.0f, 0.075f);

const char * ISPC::ControlAWB::CorrectionName(Correction_Types e)
{
    switch (e)
    {
    case WB_NONE:
        return "NONE";
    case WB_AC:
        return "Average Colour";
    case WB_WP:
        return "White Patch";
    case WB_HLW:
        return "High Luminance White";
    case WB_COMBINED:
        return "Combined";
    default:
        return "unknown";
    }
}

ISPC::ParameterGroup ISPC::ControlAWB::getGroup()
{
    ParameterGroup group;

    group.header = "// Auto White Balance parameters";

    group.parameters.insert(AWB_ESTIMATION_SCALE.name);
    group.parameters.insert(AWB_ESTIMATION_OFFSET.name);
    group.parameters.insert(AWB_TARGET_TEMPERATURE.name);
    group.parameters.insert(AWB_TARGET_PIXELRATIO.name);

    return group;
}

ISPC::ControlAWB::ControlAWB(const std::string &logName)
    : ControlModuleBase(logName), imageTotalPixels(-1),
    estimationScale(AWB_ESTIMATION_SCALE.def),
    estimationOffset(AWB_ESTIMATION_OFFSET.def),
    targetTemperature(AWB_TARGET_TEMPERATURE.def),
    targetPixelRatio(AWB_TARGET_PIXELRATIO.def),
    statisticsRegionProgrammed(false),
    measuredTemperature(0.0),
    correctionTemperature(0.0), doAwb(true), configured(false)
{
    currentCCM = colorTempCorrection.getColorCorrection(targetTemperature);
    inverseCCM = currentCCM;
    inverseCCM.inv();

    // Setup initial threshold for WB statistics
    HLWThreshold = 0.75;
    WPThresholdR = 0.4;
    WPThresholdG = 0.4;
    WPThresholdB = 0.4;

    // Initialize the estructures for tracking the threshold adaptive mechanism
    thresholdInfoR.currentState = TS_RESET;
    thresholdInfoG.currentState = TS_RESET;
    thresholdInfoB.currentState = TS_RESET;
    thresholdInfoY.currentState = TS_RESET;

    // defaults for correction mode
    correctionMode = WB_AC;
}

void ISPC::ControlAWB::setPipelineOwner(Pipeline *pipeline)
{
    ControlModule::setPipelineOwner(pipeline);
    const Sensor *sensor = pipeline->getSensor();

    if (sensor)
    {
        this->imageTotalPixels = sensor->uiWidth*sensor->uiHeight;
    }
    else
    {
        MOD_LOG_WARNING("Pipeline set but imageTotalPixels cannot be "\
            "computed because pipeline does not have a sensor!\n");
        this->imageTotalPixels = -1;
    }
}

IMG_RESULT ISPC::ControlAWB::load(const ParameterList &parameters)
{
    /* Pass the parameter list to the ColorCorrection auxiliary object so it
     *can load parameters from the parameters as well. */
    colorTempCorrection.loadParameters(parameters);

    this->estimationScale = parameters.getParameter(AWB_ESTIMATION_SCALE);
    this->estimationOffset = parameters.getParameter(AWB_ESTIMATION_OFFSET);
    this->targetTemperature = parameters.getParameter(AWB_TARGET_TEMPERATURE);
    this->targetPixelRatio = parameters.getParameter(AWB_TARGET_PIXELRATIO);

    MOD_LOG_DEBUG("scale=%f offset=%f target_temp=%f pixelRatio=%f\n",
        estimationScale, estimationOffset, targetTemperature,
        targetPixelRatio);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAWB::save(ParameterList &parameters,
    ModuleBase::SaveType t) const
{
    static ParameterGroup group;

    if (group.parameters.size() == 0)
    {
        group = ControlAWB::getGroup();
    }

    parameters.addGroup("ControlAWB", group);

    switch (t)
    {
    case ModuleBase::SAVE_VAL:
        parameters.addParameter(Parameter(AWB_ESTIMATION_SCALE.name,
            toString(this->estimationScale)));
        parameters.addParameter(Parameter(AWB_ESTIMATION_OFFSET.name,
            toString(this->estimationOffset)));
        parameters.addParameter(Parameter(AWB_TARGET_TEMPERATURE.name,
            toString(this->targetTemperature)));
        parameters.addParameter(Parameter(AWB_TARGET_PIXELRATIO.name,
            toString(this->targetPixelRatio)));
        break;

    case ModuleBase::SAVE_MIN:
        parameters.addParameter(Parameter(AWB_ESTIMATION_SCALE.name,
            toString(AWB_ESTIMATION_SCALE.min)));
        parameters.addParameter(Parameter(AWB_ESTIMATION_OFFSET.name,
            toString(AWB_ESTIMATION_SCALE.min)));
        parameters.addParameter(Parameter(AWB_TARGET_TEMPERATURE.name,
            toString(AWB_TARGET_TEMPERATURE.min)));
        parameters.addParameter(Parameter(AWB_TARGET_PIXELRATIO.name,
            toString(AWB_TARGET_PIXELRATIO.min)));
        break;

    case ModuleBase::SAVE_MAX:
        parameters.addParameter(Parameter(AWB_ESTIMATION_SCALE.name,
            toString(AWB_ESTIMATION_SCALE.max)));
        parameters.addParameter(Parameter(AWB_ESTIMATION_OFFSET.name,
            toString(AWB_ESTIMATION_SCALE.max)));
        parameters.addParameter(Parameter(AWB_TARGET_TEMPERATURE.name,
            toString(AWB_TARGET_TEMPERATURE.max)));
        parameters.addParameter(Parameter(AWB_TARGET_PIXELRATIO.name,
            toString(AWB_TARGET_PIXELRATIO.max)));
        break;

    case ModuleBase::SAVE_DEF:
        parameters.addParameter(Parameter(AWB_ESTIMATION_SCALE.name,
            toString(AWB_ESTIMATION_SCALE.def)
            + " // " + getParameterInfo(AWB_ESTIMATION_SCALE)));
        parameters.addParameter(Parameter(AWB_ESTIMATION_OFFSET.name,
            toString(AWB_ESTIMATION_OFFSET.def)
            + " // " + getParameterInfo(AWB_ESTIMATION_OFFSET)));
        parameters.addParameter(Parameter(AWB_TARGET_TEMPERATURE.name,
            toString(AWB_TARGET_TEMPERATURE.def)
            + " // " + getParameterInfo(AWB_TARGET_TEMPERATURE)));
        parameters.addParameter(Parameter(AWB_TARGET_PIXELRATIO.name,
            toString(AWB_TARGET_PIXELRATIO.def)
            + " // " + getParameterInfo(AWB_TARGET_PIXELRATIO)));
        break;
    }

    return colorTempCorrection.saveParameters(parameters, t);
}

#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlAWB::update(const Metadata &metadata, const Metadata &metadata2)
#else
IMG_RESULT ISPC::ControlAWB::update(const Metadata &metadata)
#endif //INFOTM_DUAL_SENSOR
{
    double imagePixels = static_cast<double>(this->imageTotalPixels);

    // Estimation of new threshold for the WB statistics
    estimateWPThresholds(metadata, imagePixels, this->targetPixelRatio,
        this->thresholdInfoR, this->thresholdInfoG, this->thresholdInfoB,
        WPThresholdR, WPThresholdG, WPThresholdB);

    this->HLWThreshold = estimateHLWThreshold(metadata, imagePixels,
        this->targetPixelRatio, this->thresholdInfoY);

    // program the calculated statistics in the pipeline
    // configureStatistics();

    /* Estimation of the illuminant color with the three different
     * approaches (AC, WP and HLW) */
    double HLWTemperature, WPTemperature, ACTemperature;
    double HLW_R, HLW_G, HLW_B, WP_R, WP_G, WP_B, AC_R, AC_G, AC_B;

    // Calculate the RGB illuminant estimation with different methods
    getHLWAverages(metadata, imagePixels, HLW_R, HLW_G, HLW_B);
    getWPAverages(metadata, imagePixels, WP_R, WP_G, WP_B);
    getACAverages(metadata, imagePixels, AC_R, AC_G, AC_B);

    /* Calculate the inverse of the colour transform in the pipeline to
     * estimate the captured image temperature (we have to undo the
     * previously applied color correction) */
    double iHLW_R, iHLW_G, iHLW_B, iWP_R, iWP_G, iWP_B, iAC_R, iAC_G, iAC_B;
    invertColorCorrection(HLW_R, HLW_G, HLW_B, inverseCCM, iHLW_R, iHLW_G,
        iHLW_B);
    invertColorCorrection(WP_R, WP_G, WP_B, inverseCCM, iWP_R, iWP_G, iWP_B);
    invertColorCorrection(AC_R, AC_G, AC_B, inverseCCM, iAC_R, iAC_G, iAC_B);

    // Calculation of color temperature from the R,G,B gains estimation
    // getTemperatureCorrection(pAC_R, pAC_G, pAC_B, &ACTemperature);
    // getTemperatureCorrection(pWP_R, pWP_G, pWP_B, &WPTemperature);
    // getTemperatureCorrection(pHLW_R,pHLW_G, pHLW_B,  &HLWTemperature);

    /* Calculation of the correction temperatures from the inverse RGB
     * gains estimation */
    ACTemperature =
        colorTempCorrection.getCorrelatedTemperature(iAC_R, iAC_G, iAC_B);
    WPTemperature =
        colorTempCorrection.getCorrelatedTemperature(iWP_R, iWP_G, iWP_B);
    HLWTemperature =
        colorTempCorrection.getCorrelatedTemperature(iHLW_R, iHLW_G, iHLW_B);

    MOD_LOG_DEBUG("Temp before correction AC=%lf WP=%lf HLW=%lf\n",
        ACTemperature, WPTemperature, HLWTemperature);

#if 1
    ACTemperature =
        correctTemperature(ACTemperature, estimationOffset, estimationScale);
    WPTemperature =
        correctTemperature(WPTemperature, estimationOffset, estimationScale);
    HLWTemperature =
        correctTemperature(HLWTemperature, estimationOffset, estimationScale);
#else
    ACTemperature =
        correctTemperature2(ACTemperature, estimationOffset, estimationScale);
    WPTemperature =
        correctTemperature2(WPTemperature, estimationOffset, estimationScale);
    HLWTemperature =
        correctTemperature2(HLWTemperature, estimationOffset, estimationScale);
#endif

    MOD_LOG_DEBUG("Apply corr off=%lf scale=%lf => AC=%lf WP=%lf HLW=%lf\n",
        estimationOffset, estimationScale,
        ACTemperature, WPTemperature, HLWTemperature);

    double mixRate = 0.5;
    double temperatureError = 0;
    static IMG_DOUBLE estimatedTemperature = targetTemperature;

    switch (correctionMode)
    {
    case WB_NONE:  // no correction
        estimatedTemperature = 6500;
        break;

    case WB_AC:  // AC Correction
        temperatureError = ACTemperature - targetTemperature;
        estimatedTemperature = estimatedTemperature*(1.0 - mixRate)
            + mixRate*ACTemperature;
        break;

    case WB_WP:  // WP Correction
        temperatureError = WPTemperature - targetTemperature;
        estimatedTemperature = estimatedTemperature*(1.0 - mixRate)
            + mixRate*WPTemperature;
        break;

    case WB_HLW:  // HLW Correction
        temperatureError = HLWTemperature - targetTemperature;
        estimatedTemperature = estimatedTemperature*(1.0 - mixRate)
            + mixRate*HLWTemperature;
        break;

    case WB_COMBINED:  // Combined Correction
        double mixedTemperature = 0.25*ACTemperature + 0.4*WPTemperature
            + 0.35*HLWTemperature;
        temperatureError = mixedTemperature - targetTemperature;
        estimatedTemperature = estimatedTemperature*(1.0 - mixRate)
            + mixRate*mixedTemperature;
        break;
    }
    measuredTemperature = estimatedTemperature;

    correctionTemperature = 6500 - (targetTemperature - estimatedTemperature);

    if (doAwb)
    {
#if VERBOSE_CONTROL_MODULES
        MOD_LOG_INFO("Algorithm %d Estimated temp. %.0lf - target "\
            "temp. %.0lf --> correction temp. %.0lf\n",
            correctionMode, estimatedTemperature, targetTemperature,
            correctionTemperature);
#endif

        currentCCM =
            colorTempCorrection.getColorCorrection(correctionTemperature);

        programCorrection();

        // Set inverse values for illuminant estimation
        inverseCCM = currentCCM;
        inverseCCM.inv();
    }
    return IMG_SUCCESS;
}

void ISPC::ControlAWB::setTargetTemperature(double targetTemperature)
{
    this->targetTemperature = targetTemperature;
}

double ISPC::ControlAWB::getTargetTemperature() const
{
    return this->targetTemperature;
}

void ISPC::ControlAWB::setCorrectionMode(Correction_Types correctionMode)
{
    this->correctionMode = correctionMode;
}

ISPC::ControlAWB::Correction_Types ISPC::ControlAWB::getCorrectionMode() const
{
    return this->correctionMode;
}

const ISPC::TemperatureCorrection&
ISPC::ControlAWB::getTemperatureCorrections() const
{
    return this->colorTempCorrection;
}

#if defined(INFOTM_ISP)
ISPC::TemperatureCorrection&
ISPC::ControlAWB::getTemperatureCorrections()
{
    return this->colorTempCorrection;
}

#endif //#if defined(INFOTM_ISP)
double ISPC::ControlAWB::getMeasuredTemperature() const
{
    return measuredTemperature;
}

double ISPC::ControlAWB::getCorrectionTemperature() const
{
    return correctionTemperature;
}

IMG_RESULT ISPC::ControlAWB::configureStatistics()
{
    ModuleWBS *wbs = NULL;

    if (getPipelineOwner())
    {
        wbs = getPipelineOwner()->getModule<ModuleWBS>();
    }
    else
    {
        MOD_LOG_ERROR("ControlAWB has no pipeline owner! "\
            "Cannot configure statistics.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    configured = false;
    if (wbs)
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

            // Program region 1
            wbs->aROIStartCoord[0][0] = 0;
            wbs->aROIStartCoord[0][1] = 0;
            wbs->aROIEndCoord[0][0] = sensor->uiWidth - 1;
            wbs->aROIEndCoord[0][1] = sensor->uiHeight - 1;

            // Program region 2
            wbs->aROIStartCoord[1][0] = 0;
            wbs->aROIStartCoord[1][1] = 0;
            wbs->aROIEndCoord[1][0] = sensor->uiWidth - 1;
            wbs->aROIEndCoord[1][1] = sensor->uiHeight - 1;
            statisticsRegionProgrammed = true;

            wbs->ui32NROIEnabled = 2;
        }

        /* program the second set of thresholds to fixed values to measure
         * clipped pixels */
        wbs->aYHLWTH[1] = 0.9f;
        wbs->aRedMaxTH[1] = 0.45f;
        wbs->aGreenMaxTH[1] = 0.45f;
        wbs->aBlueMaxTH[1] = 0.45f;

        /* program the statistics threshold for the next frame based on
         * the calculated thresholds */
        wbs->aYHLWTH[0] = HLWThreshold;
        wbs->aRedMaxTH[0] = WPThresholdR;
        wbs->aGreenMaxTH[0] = WPThresholdG;
        wbs->aBlueMaxTH[0] = WPThresholdB;

        wbs->requestUpdate();
        configured = true;
        return IMG_SUCCESS;
    }
    else
    {
        MOD_LOG_ERROR("ControlAWB cannot find WBS module.");
    }

    return IMG_ERROR_UNEXPECTED_STATE;
}

IMG_RESULT ISPC::ControlAWB::programCorrection()
{
    std::list<Pipeline*>::iterator it;

    for (it = pipelineList.begin(); it != pipelineList.end(); it++)
    {
        ModuleWBC *wbc = (*it)->getModule<ModuleWBC>();
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
        }
        else
        {
#if VERBOSE_CONTROL_MODULES
            MOD_LOG_WARNING("one pipeline is missing either CCM or "\
                "WBC modules\n");
#endif
        }
    }

    return IMG_SUCCESS;
}

//Dynamic threshold estimation for WB statistics (see header file)
double ISPC::ControlAWB::estimateThreshold(Threshold &thresholdInfo,
    double meteredRatio, double targetRatio)
{
    double targetError = meteredRatio - targetRatio;
    thresholdInfo.accumulatedError += targetError;
    thresholdInfo.accumulatedError =
        ispc_max(ispc_min(2.0, thresholdInfo.accumulatedError), -2.0);

    /* Implements different behaviour depending on the state of the
     * threshold estimation */
    switch (thresholdInfo.currentState)
    {
    case TS_RESET:  // resetting to default values
        thresholdInfo.nextThreshold = 0.5;
        thresholdInfo.errorMargin = 0.005;
        thresholdInfo.currentState = TS_SCANNING;
        thresholdInfo.accumulatedError = 0;
        thresholdInfo.scanningFrames = 0;
        break;

    case TS_SCANNING:
        /* Looking for a stable threshold that produces the desired
         * ratio of pixels above the threshold */

        /* The next threshold is calculated in a sort of pid
         * controller manner */
        thresholdInfo.nextThreshold = thresholdInfo.nextThreshold
            + targetError*0.075 + thresholdInfo.accumulatedError*0.01;

        if ((fabs(targetError) < thresholdInfo.errorMargin)
            && (meteredRatio > 0.0001))
        {
            // We've found a stable threshold
            thresholdInfo.currentState = TS_FOUND;
        }

        thresholdInfo.scanningFrames++;

        //  the number of frames should be configurable
        /* If we can't find an stable threshold for certain amount
         * of time we increase the tolerance value */
        if (thresholdInfo.scanningFrames > 50)
        {
            // unable to find an stable threshold
            thresholdInfo.errorMargin *= 1.1;  // increase the tolerance range
        }
        break;

    case TS_FOUND:
        /* This states represents to have found a threshold, we'll keep
         * it unless the error goes to high */
        thresholdInfo.accumulatedError = 0;
        if (fabs(targetError) > thresholdInfo.errorMargin
            || meteredRatio < 0.0001)
        {
            thresholdInfo.currentState = TS_SCANNING;
            thresholdInfo.errorMargin = 0.005;
        }
        thresholdInfo.errorMargin *= 0.98;
        thresholdInfo.errorMargin =
            ispc_max(0.005, thresholdInfo.errorMargin);
        break;
    }

    thresholdInfo.nextThreshold =
        ispc_min(1.0, ispc_max(0.0, thresholdInfo.nextThreshold));

    return  thresholdInfo.nextThreshold;
}

//Process the WP statistics and update the WP thresholds
void ISPC::ControlAWB::estimateWPThresholds(const Metadata &metadata,
    double imagePixels, double targetPixelRatio,
    Threshold &RInfo, Threshold &GInfo, Threshold &BInfo,
    double &RThreshold, double &GThreshold, double &BThreshold)
{
    double RPixels = metadata.whiteBalanceStats.whitePatch[0].channelCount[0];
    double GPixels = metadata.whiteBalanceStats.whitePatch[0].channelCount[1];
    double BPixels = metadata.whiteBalanceStats.whitePatch[0].channelCount[2];

    double RPixelsClipped =
        metadata.whiteBalanceStats.whitePatch[1].channelCount[0];
    double GPixelsClipped =
        metadata.whiteBalanceStats.whitePatch[1].channelCount[1];
    double BPixelsClipped =
        metadata.whiteBalanceStats.whitePatch[1].channelCount[2];

    double pixelRatioR = RPixels / imagePixels;
    double pixelRatioG = GPixels / imagePixels;
    double pixelRatioB = BPixels / imagePixels;

    double pixelRatioRClipped = RPixelsClipped / imagePixels;
    double pixelRatioGClipped = GPixelsClipped / imagePixels;
    double pixelRatioBClipped = BPixelsClipped / imagePixels;

    double clippedRatio = ispc_max(pixelRatioRClipped,
        ispc_max(pixelRatioGClipped, pixelRatioBClipped)) * 1;

    RThreshold =
        estimateThreshold(RInfo, pixelRatioR, targetPixelRatio + clippedRatio);
    GThreshold =
        estimateThreshold(GInfo, pixelRatioG, targetPixelRatio + clippedRatio);
    BThreshold =
        estimateThreshold(BInfo, pixelRatioB, targetPixelRatio + clippedRatio);
}

double ISPC::ControlAWB::estimateHLWThreshold(const Metadata &metadata,
    double imagePixels, double targetPixelRatio, Threshold &YInfo)
{
    double threshold = 0;

    double HLWPixels =
        metadata.whiteBalanceStats.highLuminance[0].lumaCount;
    double HLWPixelsClipped =
        metadata.whiteBalanceStats.highLuminance[1].lumaCount;

    double HLWRatio = HLWPixels / imagePixels;
    double HLWRatioClipped = HLWPixelsClipped / imagePixels;

    threshold = estimateThreshold(YInfo, HLWRatio,
        targetPixelRatio + HLWRatioClipped*1.25);
    return threshold;
}

//Process AWB statistics and get R,G,B averages for the pixels counted in the HLW statistics module
void ISPC::ControlAWB::getHLWAverages(const Metadata &metadata,
    double imagePixels, double &redHLW, double &greenHLW, double &blueHLW)
{
    if (metadata.whiteBalanceStats.highLuminance[0].lumaCount > 0)
    {
        double countDifference = static_cast<double>(
            metadata.whiteBalanceStats.highLuminance[0].lumaCount
            - metadata.whiteBalanceStats.highLuminance[1].lumaCount);
        if (countDifference > 0)
        {
            redHLW = (metadata.whiteBalanceStats.highLuminance[0].\
                channelAccumulated[0]
                - metadata.whiteBalanceStats.highLuminance[1].\
                channelAccumulated[0]) / countDifference;
            greenHLW = (metadata.whiteBalanceStats.highLuminance[0].\
                channelAccumulated[1]
                - metadata.whiteBalanceStats.highLuminance[1].\
                channelAccumulated[1]) / countDifference;
            blueHLW = (metadata.whiteBalanceStats.highLuminance[0].\
                channelAccumulated[2]
                - metadata.whiteBalanceStats.highLuminance[1].\
                channelAccumulated[2]) / countDifference;
        }
        else
        {
            // no data to get a metering, will give the AC stimation instead.
            getACAverages(metadata, imagePixels, redHLW, greenHLW, blueHLW);
        }
    }
    else
    {
        // no data to get a metering, will give the AC stimation instead.
        getACAverages(metadata, imagePixels, redHLW, greenHLW, blueHLW);
    }
}

//Process AWB stastistics and get R,G,B averages for pixels counted in the WP statistics module
void ISPC::ControlAWB::getWPAverages(const Metadata &metadata,
    double imagePixels, double &redWP, double &greenWP, double &blueWP)
{
    if ((metadata.whiteBalanceStats.whitePatch[0].channelCount[0] > 0)
        && (metadata.whiteBalanceStats.whitePatch[0].channelCount[1] > 0)
        && (metadata.whiteBalanceStats.whitePatch[0].channelCount[2] > 0))
    {
        redWP = 0;
        greenWP = 0;
        blueWP = 0;

        double countDifference = static_cast<double>(
            metadata.whiteBalanceStats.whitePatch[0].channelCount[0]
            - metadata.whiteBalanceStats.whitePatch[1].channelCount[0]);

        if (countDifference > 0)
        {
            redWP = static_cast<double>(
                metadata.whiteBalanceStats.whitePatch[0].channelAccumulated[0]
                - metadata.whiteBalanceStats.whitePatch[1].\
                channelAccumulated[0]) / countDifference;
        }

        countDifference = static_cast<double>(
            metadata.whiteBalanceStats.whitePatch[0].channelCount[1]
            - metadata.whiteBalanceStats.whitePatch[1].channelCount[1]);
        if (countDifference > 0)
        {
            greenWP = static_cast<double>(
                metadata.whiteBalanceStats.whitePatch[0].channelAccumulated[1]
                - metadata.whiteBalanceStats.whitePatch[1].\
                channelAccumulated[1]) / countDifference;
        }

        countDifference = static_cast<double>(
            metadata.whiteBalanceStats.whitePatch[0].channelCount[2]
            - metadata.whiteBalanceStats.whitePatch[1].channelCount[2]);
        if (countDifference > 0)
        {
            blueWP = static_cast<double>(
                metadata.whiteBalanceStats.whitePatch[0].channelAccumulated[2]
                - metadata.whiteBalanceStats.whitePatch[1].\
                channelAccumulated[2]) / countDifference;
        }
    }
    else
    {
        // no data to get a metering, will give the AC stimation instead.
        getACAverages(metadata, imagePixels, redWP, greenWP, blueWP);
    }

    if (0.0 == redWP || 0.0 == greenWP || 0.0 == blueWP)
    {
        // no data to get a metering, will give the AC stimation instead.
        getACAverages(metadata, imagePixels, redWP, greenWP, blueWP);
    }
}

void ISPC::ControlAWB::getACAverages(const Metadata &metadata,
    double imagePixels, double &redAvg, double &greenAvg, double &blueAvg)
{
    redAvg = metadata.whiteBalanceStats.averageColor[0].channelAccumulated[0]
        / imagePixels;
    greenAvg = metadata.whiteBalanceStats.averageColor[0].channelAccumulated[1]
        / imagePixels;
    blueAvg = metadata.whiteBalanceStats.averageColor[0].channelAccumulated[2]
        / imagePixels;
}

void ISPC::ControlAWB::colorTransform(double R, double G, double B,
    const ColorCorrection &correction, double &targetR, double &targetG,
    double &targetB)
{
    targetR = R * correction.coefficients[0][0]
        + G * correction.coefficients[1][0]
        + B * correction.coefficients[2][0];
    targetG = R * correction.coefficients[0][1]
        + G * correction.coefficients[1][1]
        + B * correction.coefficients[2][1];
    targetB = R * correction.coefficients[0][2]
        + G * correction.coefficients[1][2]
        + B * correction.coefficients[2][2];
}

void ISPC::ControlAWB::invertColorCorrection(double R, double G, double B,
    const ColorCorrection &invCCM, double &iR, double &iG, double &iB)
{
    IMG_DOUBLE gammaN = PIPELINE_WHITE_LEVEL;

#ifdef USE_MATH_NEON
	iR = (double)powf_neon((float)(R / gammaN), 2.2f)*gammaN;
    iG = (double)powf_neon((float)(G / gammaN), 2.2f)*gammaN;
    iB = (double)powf_neon((float)(B / gammaN), 2.2f)*gammaN;
#else
	iR = pow(R / gammaN, 2.2)*gammaN;
    iG = pow(G / gammaN, 2.2)*gammaN;
    iB = pow(B / gammaN, 2.2)*gammaN;
#endif

    colorTransform(R + invCCM.offsets[0][0], G + invCCM.offsets[0][1],
        B + invCCM.offsets[0][2], invCCM, iR, iG, iB);
    iR *= invCCM.gains[0][0];
    iG *= (invCCM.gains[0][1] + invCCM.gains[0][2]) / 2;
    iB *= invCCM.gains[0][3];

#ifdef USE_MATH_NEON
    iR = (double)powf_neon((float)(iR / gammaN), 1.0 / 2.2f)*gammaN;
    iG = (double)powf_neon((float)(iG / gammaN), 1.0 / 2.2f)*gammaN;
    iB = (double)powf_neon((float)(iB / gammaN), 1.0 / 2.2f)*gammaN;
#else
    iR = pow(iR / gammaN, 1 / 2.2)*gammaN;
    iG = pow(iG / gammaN, 1 / 2.2)*gammaN;
    iB = pow(iB / gammaN, 1 / 2.2)*gammaN;
#endif
}

double ISPC::ControlAWB::correctTemperature(double temperature,
    double offset, double scale)
{
    double correctedT = (temperature + offset)*scale;
    return ispc_max(correctedT, 0.0);
}

double ISPC::ControlAWB::correctTemperature2(double temperature,
    double offset, double scale)
{
    double correctedT = (temperature*scale) + offset;
    return ispc_max(correctedT, 0.0);
}
