/**
*******************************************************************************
@file ControlAF.cpp

@brief ISPC::ControlAF implementation

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
#include "ispc/ControlAF.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_AF"

#include <cmath>
#include <string>

#include "ispc/ModuleFOS.h"
#include "ispc/Sensor.h"
#include "ispc/Pipeline.h"

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

const double ISPC::ControlAF::WEIGHT_7X7_SPREAD[7][7] = {
    { 0.0129, 0.0161, 0.0183, 0.0191, 0.0183, 0.0161, 0.0129 },
    { 0.0161, 0.0200, 0.0227, 0.0237, 0.0227, 0.0200, 0.0161 },
    { 0.0183, 0.0227, 0.0259, 0.0271, 0.0259, 0.0227, 0.0183 },
    { 0.0191, 0.0237, 0.0271, 0.0283, 0.0271, 0.0237, 0.0191 },
    { 0.0183, 0.0227, 0.0259, 0.0271, 0.0259, 0.0227, 0.0183 },
    { 0.0161, 0.0200, 0.0227, 0.0237, 0.0227, 0.0200, 0.0161 },
    { 0.0129, 0.0161, 0.0183, 0.0191, 0.0183, 0.0161, 0.0129 } };

const double ISPC::ControlAF::WEIGHT_7X7_CENTRAL[7][7] = {
    { 0.0000, 0.0000, 0.0000, 0.0001, 0.0000, 0.0000, 0.0000 },
    { 0.0000, 0.0003, 0.0036, 0.0086, 0.0036, 0.0003, 0.0000 },
    { 0.0000, 0.0036, 0.0487, 0.1160, 0.0487, 0.0036, 0.0000 },
    { 0.0001, 0.0086, 0.1160, 0.2763, 0.1160, 0.0086, 0.0001 },
    { 0.0000, 0.0036, 0.0487, 0.1160, 0.0487, 0.0036, 0.0000 },
    { 0.0000, 0.0003, 0.0036, 0.0086, 0.0036, 0.0003, 0.0000 },
    { 0.0000, 0.0000, 0.0000, 0.0001, 0.0000, 0.0000, 0.0000 } };

const ISPC::ParamDef<double> ISPC::ControlAF::AF_CENTER_WEIGTH(
    "AF_CENTER_WEIGTH", 0.0, 1.0, 1.0);

const char* ISPC::ControlAF::StateName(ISPC::ControlAF::State e)
{
    switch (e)
    {
    case AF_IDLE:
        return "AF_IDLE";
    case AF_SCANNING:
        return "AF_SCANNING";
    case AF_FOCUSED:
        return "AF_FOCUSED";
    case AF_OUT:
        return "AF_OUT";
    default:
        return "unknown";
    }
}

const char* ISPC::ControlAF::ScanStateName(ISPC::ControlAF::ScanState e)
{
    switch (e)
    {
    case AF_SCAN_STOP:
        return "AF_SCAN_STOP";
    case AF_SCAN_INIT:
        return "AF_SCAN_INIT";
    case AF_SCAN_ROUGH:
        return "AF_SCAN_ROUGH";
    case AF_SCAN_FINE:
        return "AF_SCAN_FINE";
    case AF_SCAN_REFINE:
        return "AF_SCAN_REFINE";
    case AF_SCAN_POSITIONING:
        return "AF_SCAN_POSITIONING";
    case AF_SCAN_FINISHED:
        return "AF_SCAN_FINISHED";
    default:
        return "unknown";
    }
}

const char* ISPC::ControlAF::CommandName(ISPC::ControlAF::Command e)
{
    switch (e)
    {
    case AF_TRIGGER:
        return "AF_TRIGGER";
    case AF_STOP:
        return "AF_STOP";
    case AF_FOCUS_CLOSER:
        return "AF_FOCUS_CLOSER";
    case AF_FOCUS_FURTHER:
        return "AF_FOCUS_FURTHER";
    case AF_NONE:
        return "AF_NONE";
    default:
        return "unknown";
    }
}

ISPC::ParameterGroup ISPC::ControlAF::getGroup()
{
    ParameterGroup group;

    group.header = "// Auto Focus parameters";

    group.parameters.insert(AF_CENTER_WEIGTH.name);

    return group;
}

ISPC::ControlAF::ControlAF(const std::string &logName)
    : ControlModuleBase(logName), flagInitialized(false),
    centerWeigth(AF_CENTER_WEIGTH.def)
{
}

IMG_RESULT ISPC::ControlAF::load(const ParameterList &parameters)
{
    this->centerWeigth = parameters.getParameter(AF_CENTER_WEIGTH);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAF::save(ParameterList &parameters, SaveType t) const
{
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ControlAF::getGroup();
    }

    parameters.addGroup("ControlAF", group);

    switch (t)
    {
    case SAVE_VAL:
        parameters.addParameter(Parameter(AF_CENTER_WEIGTH.name,
            toString(this->centerWeigth)));
        break;

    case SAVE_MIN:
        parameters.addParameter(Parameter(AF_CENTER_WEIGTH.name,
            toString(AF_CENTER_WEIGTH.min)));
        break;

    case SAVE_MAX:
        parameters.addParameter(Parameter(AF_CENTER_WEIGTH.name,
            toString(AF_CENTER_WEIGTH.max)));
        break;

    case SAVE_DEF:
        parameters.addParameter(Parameter(AF_CENTER_WEIGTH.name,
            toString(AF_CENTER_WEIGTH.def)
            + " // " + getParameterInfo(AF_CENTER_WEIGTH)));
        break;
    }

    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlAF::update(const Metadata &metadata, const Metadata &metadata2)
#else
IMG_RESULT ISPC::ControlAF::update(const Metadata &metadata)
#endif //INFOTM_DUAL_SENSOR
{
    const Sensor *sensor = getSensor();
    if (!sensor)
    {
        MOD_LOG_ERROR("ControlAF has no sensor!\n");
        return IMG_ERROR_NOT_INITIALISED;
    }
    if (!flagInitialized)
    {
        MOD_LOG_WARNING("ControlAF is not initialised. Initialising it now\n");
        configureStatistics();  // Configure the module statistics
        bestSharpness = 0.0;
        bestFocusDistance = 0;
        minFocusDistance = sensor->getMinFocus();
        maxFocusDistance = sensor->getMaxFocus();
        targetFocusDistance = minFocusDistance;
        lastSharpness = 0.0;
        currentState = AF_IDLE;
        nextState = AF_IDLE;
        scanState = AF_SCAN_STOP;
        lastCommand = AF_NONE;
        scanInit = 0;
        scanEnd = 0;

        flagInitialized = true;
    }
    else
    {
        double sharpness = sharpnessGridMetering(metadata, this->centerWeigth);
        runAF(sensor->getFocusDistance(), sharpness, lastCommand);
        lastCommand = AF_NONE;
    }

    return IMG_SUCCESS;
}

void ISPC::ControlAF::setCommand(ISPC::ControlAF::Command command)
{
    lastCommand = command;
}

ISPC::ControlAF::State ISPC::ControlAF::getState() const
{
    return this->currentState;
}

ISPC::ControlAF::ScanState ISPC::ControlAF::getScanState() const
{
    return this->scanState;
}

unsigned int ISPC::ControlAF::getBestFocusDistance() const
{
    return this->bestFocusDistance;
}

double ISPC::ControlAF::getCenterWeight() const
{
    return this->centerWeigth;
}

void ISPC::ControlAF::runAF(unsigned int lastFocusDistance,
    double lastSharpness, ISPC::ControlAF::Command command)
{
#if VERBOSE_CONTROL_MODULES
    MOD_LOG_INFO("Command %d - State %d - Scan state %d - Distance %u - "\
        "Sharpness %10.0lfk - Scan init %d - Scan end %d\n",
        (int)command,
        (int)currentState,
        (int)scanState,
        lastFocusDistance,
        lastSharpness/1000.0,
        scanInit,
        scanEnd);
#endif

    currentState = nextState;
    switch (currentState)
    {
    case AF_IDLE:  // Waiting
        if (AF_TRIGGER == command)
        {
            nextState = AF_SCANNING;
            scanState = AF_SCAN_STOP;
        }
        if (AF_FOCUS_CLOSER == command)
        {
            targetFocusDistance =
                static_cast<unsigned int>(targetFocusDistance / 1.1);
        }
        if (AF_FOCUS_FURTHER == command)
        {
            targetFocusDistance =
                static_cast<unsigned int>(targetFocusDistance * 1.1);
        }
        break;

    case AF_SCANNING:  // Scanning for optimal focus
        if (AF_SCAN_STOP == scanState)
        {
            // just initializing the focus scan
            bestSharpness = 0.0;
            scanInit = minFocusDistance;
            scanEnd = maxFocusDistance;
            if (scanInit > scanEnd)
            {
                unsigned int tmp = scanInit;
                scanInit = scanEnd;
                scanEnd = tmp;
            }
            bestFocusDistance = scanInit;
            targetFocusDistance = scanInit;
            scanState = AF_SCAN_INIT;
        }
        else if (AF_SCAN_INIT == scanState)
        {
            // focus scan positioned ready to start scan
            scanState = AF_SCAN_ROUGH;
        }
        else if (AF_SCAN_ROUGH == scanState)
        {
            // rough focus scan
            if (lastSharpness > bestSharpness)
            {
                bestSharpness = lastSharpness;
                bestFocusDistance = lastFocusDistance;
            }
            targetFocusDistance = lastFocusDistance
                + (scanEnd - scanInit) / 10;
            targetFocusDistance = ispc_clip(targetFocusDistance,
                ispc_min(scanInit, scanEnd), ispc_max(scanInit, scanEnd));

            if (lastFocusDistance == scanEnd)  // finished the rough scanning
            {
                unsigned int distanceMargin = (scanEnd - scanInit) / 5;
                if (distanceMargin > bestFocusDistance)
                {
                    distanceMargin = bestFocusDistance / 2;
                }
                scanInit = bestFocusDistance - distanceMargin;
                scanEnd = bestFocusDistance + distanceMargin;
                scanInit = ispc_clip(scanInit,
                    minFocusDistance, maxFocusDistance);
                scanEnd = ispc_clip(scanEnd,
                    minFocusDistance, maxFocusDistance);

                targetFocusDistance = scanInit;

                scanState = AF_SCAN_FINE;
                bestSharpness = 0;
                bestFocusDistance = scanInit;
            }
        }
        else if (AF_SCAN_FINE == scanState)
        {
            // fine focus scan
            if (lastSharpness > bestSharpness)
            {
                bestSharpness = lastSharpness;
                bestFocusDistance = lastFocusDistance;
            }
            targetFocusDistance = lastFocusDistance
                + (scanEnd - scanInit) / 10;
            targetFocusDistance = ispc_clip(targetFocusDistance,
                ispc_min(scanInit, scanEnd), ispc_max(scanInit, scanEnd));

            if (lastFocusDistance == scanEnd)  // finished the fine scanning
            {
                targetFocusDistance = bestFocusDistance;
                scanState = AF_SCAN_POSITIONING;
            }
        }
        else if (AF_SCAN_POSITIONING == scanState)
        {
            nextState = AF_FOCUSED;
            scanState = AF_SCAN_STOP;
        }
        else if (AF_SCAN_REFINE == scanState || AF_SCAN_FINISHED == scanState)
        {
            scanState = AF_SCAN_STOP;
            /* used only in FelixAF */
        }
        break;

    case AF_FOCUSED:  // In-focus found
        nextState = AF_IDLE;  // Let's go and wait for a different status
        break;

    case AF_OUT:  // Unable to get in-focus
        break;
    }

    targetFocusDistance = ispc_clip(targetFocusDistance,
        minFocusDistance, maxFocusDistance);

    Sensor *sensor = getSensor();
    if (sensor)
    {
        sensor->setFocusDistance(targetFocusDistance);
    }
    else
    {
        MOD_LOG_ERROR("ControlAF has no sensor! Did not update "\
            "focus distance\n");
    }
}

double ISPC::ControlAF::sharpnessGridMetering(const Metadata &shotMetadata,
    double centreWeight)
{
    double sharpness = 0;

    for (int tX = 0; tX < 7; tX++)
    {
        for (int tY = 0; tY < 7; tY++)
        {
            sharpness += (shotMetadata.focusStats.gridSharpness[tX][tY])
                * (WEIGHT_7X7_SPREAD[tX][tY] * (1.0 - centreWeight)
                + WEIGHT_7X7_CENTRAL[tX][tY] * centreWeight);
        }
    }
    return sharpness;
}

IMG_RESULT ISPC::ControlAF::programCorrection()
{
    return IMG_ERROR_NOT_SUPPORTED;
}

IMG_RESULT ISPC::ControlAF::configureStatistics()
{
    ModuleFOS *fos = NULL;

    if (getPipelineOwner())
    {
        fos = getPipelineOwner()->getModule<ModuleFOS>();
    }
    else
    {
        MOD_LOG_ERROR("ControlAF has no pipeline owner! Cannot "\
            "configure statistics.\n");
        return IMG_ERROR_NOT_INITIALISED;
    }

    if (fos)
    {
        const Sensor *sensor = getSensor();
        if (!sensor)
        {
            MOD_LOG_ERROR("ControlAF has no sensor!\n");
            return IMG_ERROR_NOT_INITIALISED;
        }
        // Set the ROI coordinates to cover central 1/3 x 1/3 tile of the image
        fos->bEnableROI = true;

        fos->aRoiStartCoord[0] = sensor->uiWidth / 3;
        fos->aRoiStartCoord[1] = sensor->uiHeight / 3;
        fos->aRoiEndCoord[0] = 2 * sensor->uiWidth / 3;
        fos->aRoiEndCoord[1] = 2 * sensor->uiHeight / 3;

        // Set the grid coordinates to cover the whole image
        fos->bEnableGrid = true;

        int tileWidth = static_cast<int>(floor((sensor->uiWidth) / 7.0));
        int tileHeight = static_cast<int>(floor((sensor->uiHeight) / 7.0));
        int marginWidth = sensor->uiWidth
            - static_cast<int>(tileWidth*7.0);
        int marginHeight = sensor->uiHeight
            - static_cast<int>(tileHeight*7.0);

        fos->aGridStartCoord[0] = marginWidth / 2;
        fos->aGridStartCoord[1] = marginHeight / 2;
        fos->aGridTileSize[0] = tileWidth;
        fos->aGridTileSize[1] = tileHeight;

        fos->requestUpdate();

        return IMG_SUCCESS;
    }
    return IMG_ERROR_UNEXPECTED_STATE;
}
