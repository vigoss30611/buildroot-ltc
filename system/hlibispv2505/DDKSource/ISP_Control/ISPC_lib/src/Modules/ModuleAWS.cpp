/**
*******************************************************************************
 @file ModuleAWS.cpp

 @brief Implementation of ISPC::ModuleAWS

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
#include <limits>
#include "ispc/ModuleAWS.h"

#include <mc/module_config.h>
#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MOD_AWS"

#include "ispc/Pipeline.h"
#include "ispc/PerfTime.h"

const ISPC::ParamDefSingle<bool> ISPC::ModuleAWS::AWS_ENABLED(
        "AWS_ENABLED", false);

const ISPC::ParamDefSingle<bool> ISPC::ModuleAWS::AWS_DEBUG_MODE(
        "AWS_DEBUG_MODE", false);


static const short defaultCoords[] = { 0, 0 };

// to set the real defaults, we need imager resolution
// now we set just some example data
static const short defaultSize[] = {
    AWS_MIN_GRID_TILE_WIDTH, // 1280 / AWS_NUM_GRID_TILES_HORIZ,
    AWS_MIN_GRID_TILE_HEIGHT, // 720 / AWS_NUM_GRID_TILES_VERT
};

const ISPC::ParamDefArray<short> ISPC::ModuleAWS::AWS_TILE_START_COORDS(
        "AWS_TILE_START_COORDS",
        0, std::numeric_limits<short>::max(), defaultCoords, 2 );

const short ISPC::ModuleAWS::AWS_TILE_WIDTH_MIN =
    AWS_MIN_GRID_TILE_WIDTH;

const short ISPC::ModuleAWS::AWS_TILE_HEIGHT_MIN =
    AWS_MIN_GRID_TILE_HEIGHT;

const ISPC::ParamDefArray<short> ISPC::ModuleAWS::AWS_TILE_SIZE(
        "AWS_TILE_SIZE",
        0, std::numeric_limits<short>::max(), defaultSize, 2 );

// ceil() of u'8.5
#define AWS_LOG2_QEFF_MAX (double)(1<<8)
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_LOG2_R_QEFF(
    "AWS_LOG2_R_QEFF", 0.0, AWS_LOG2_QEFF_MAX, 1.0);
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_LOG2_B_QEFF(
    "AWS_LOG2_B_QEFF", 0.0, AWS_LOG2_QEFF_MAX, 1.0);

// ceil() of u'8.8
#define AWS_THRESH_MAX (double)(1<<8)
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_R_DARK_THRESH(
    "AWS_R_DARK_THRESH", 0.0, AWS_THRESH_MAX, 8);
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_G_DARK_THRESH(
    "AWS_G_DARK_THRESH", 0.0, AWS_THRESH_MAX, 8);
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_B_DARK_THRESH(
    "AWS_B_DARK_THRESH", 0.0, AWS_THRESH_MAX, 8);

const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_R_CLIP_THRESH(
    "AWS_R_CLIP_THRESH", 0.0, AWS_THRESH_MAX, AWS_THRESH_MAX/2);
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_G_CLIP_THRESH(
    "AWS_G_CLIP_THRESH", 0.0, AWS_THRESH_MAX, AWS_THRESH_MAX/2);
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_B_CLIP_THRESH(
    "AWS_B_CLIP_THRESH", 0.0, AWS_THRESH_MAX, AWS_THRESH_MAX/2);

// ceil() of u'7.5
#define AWS_BB_DIST_MAX (double)(1<<7)
const ISPC::ParamDef<double> ISPC::ModuleAWS::AWS_BB_DIST(
    "AWS_BB_DIST", 0.0, AWS_BB_DIST_MAX, 0.2);

#define AWS_MAX_LINES 5

const ISPC::ParamDef<int> ISPC::ModuleAWS::AWS_CURVES_NUM(
    "AWS_CURVES_NUM", 0, 5, 0);

/*
 * happily IMG_Fix_Revert() does not rely on any static const variables
 * So we are kind of safe to call it from here...
 */
static const double AWS_CURVE_REG_MIN =
        IMG_Fix_Revert(-(1<<14), 5, 10, true, "");

static const double AWS_CURVE_REG_MAX =
        IMG_Fix_Revert(1<<14, 5, 10, true, "");

static const double awsCurveXCoeffsDef[] =
{
        IMG_Fix_Revert(0x038C, 5, 10, true, ""),
        IMG_Fix_Revert(0x03DE, 5, 10, true, ""),
        IMG_Fix_Revert(0x03FE, 5, 10, true, ""),
        IMG_Fix_Revert(0x0000, 5, 10, true, ""),
        IMG_Fix_Revert(0x0000, 5, 10, true, "")
};

static const double awsCurveYCoeffsDef[] =
{
        IMG_Fix_Revert(0x01d8, 5, 10, true, ""),
        IMG_Fix_Revert(0x0105, 5, 10, true, ""),
        IMG_Fix_Revert(0x0042, 5, 10, true, ""),
        IMG_Fix_Revert(0x0000, 5, 10, true, ""),
        IMG_Fix_Revert(0x0000, 5, 10, true, "")
};

static const double awsCurveOffsetsDef[] =
{
        IMG_Fix_Revert(0x0028, 5, 10, true, ""),
        IMG_Fix_Revert(0x0106, 5, 10, true, ""),
        IMG_Fix_Revert(0x028c, 5, 10, true, ""),
        IMG_Fix_Revert(0x0000, 5, 10, true, ""),
        IMG_Fix_Revert(0x0000, 5, 10, true, "")
};

static const double awsCurveBoundariesDef[] =
{
        IMG_Fix_Revert(0x0100, 5, 10, true, ""),
        IMG_Fix_Revert(0x7ca0, 5, 10, true, ""),
        IMG_Fix_Revert(0x7860, 5, 10, true, ""),
        IMG_Fix_Revert(0x4000, 5, 10, true, ""),
        IMG_Fix_Revert(0x4000, 5, 10, true, "")
};

const ISPC::ParamDefArray<double> ISPC::ModuleAWS::AWS_CURVE_X_COEFFS(
        "AWS_CURVE_X_COEFFS", AWS_CURVE_REG_MIN, AWS_CURVE_REG_MAX,
        awsCurveXCoeffsDef, AWS_MAX_LINES);

const ISPC::ParamDefArray<double> ISPC::ModuleAWS::AWS_CURVE_Y_COEFFS(
        "AWS_CURVE_Y_COEFFS", AWS_CURVE_REG_MIN, AWS_CURVE_REG_MAX,
        awsCurveYCoeffsDef, AWS_MAX_LINES);

const ISPC::ParamDefArray<double> ISPC::ModuleAWS::AWS_CURVE_OFFSETS(
        "AWS_CURVE_OFFSETS", AWS_CURVE_REG_MIN, AWS_CURVE_REG_MAX,
        awsCurveOffsetsDef, AWS_MAX_LINES);

const ISPC::ParamDefArray<double> ISPC::ModuleAWS::AWS_CURVE_BOUNDARIES(
        "AWS_CURVE_BOUNDARIES", AWS_CURVE_REG_MIN, AWS_CURVE_REG_MAX,
        awsCurveBoundariesDef, AWS_MAX_LINES);

bool ISPC::operator<(
        const ModuleAWS::curveLine& l,
        const ModuleAWS::curveLine& r)
{
    return l.fBoundary < r.fBoundary;
}

ISPC::ParameterGroup ISPC::ModuleAWS::getGroup()
{
    ParameterGroup group;

    group.header = "// Auto White Balance Statistics parameters (AWS block)";

    group.parameters.insert(AWS_ENABLED.name);
    group.parameters.insert(AWS_DEBUG_MODE.name);
    group.parameters.insert(AWS_LOG2_R_QEFF.name);
    group.parameters.insert(AWS_LOG2_B_QEFF.name);
    group.parameters.insert(AWS_R_DARK_THRESH.name);
    group.parameters.insert(AWS_G_DARK_THRESH.name);
    group.parameters.insert(AWS_B_DARK_THRESH.name);
    group.parameters.insert(AWS_R_CLIP_THRESH.name);
    group.parameters.insert(AWS_G_CLIP_THRESH.name);
    group.parameters.insert(AWS_B_CLIP_THRESH.name);
    group.parameters.insert(AWS_BB_DIST.name);
    group.parameters.insert(AWS_TILE_START_COORDS.name);
    group.parameters.insert(AWS_TILE_SIZE.name);
    group.parameters.insert(AWS_CURVES_NUM.name);
    group.parameters.insert(AWS_CURVE_X_COEFFS.name);
    group.parameters.insert(AWS_CURVE_Y_COEFFS.name);
    group.parameters.insert(AWS_CURVE_OFFSETS.name);
    group.parameters.insert(AWS_CURVE_BOUNDARIES.name);

    return group;
}

ISPC::ModuleAWS::ModuleAWS(): SetupModuleBase(LOG_TAG)
{
    ParameterList defaults;
    load(defaults);
}

int ISPC::ModuleAWS::loadLinesParams(
        const ParameterList &parameters,
        linesSet& lines)
{
    if(lines.size()>0)
    {
        return -1;
    }
    int curves = parameters.getParameter(AWS_CURVES_NUM);
    int i=0;
    while(i<curves)
    {
        curveLine crv;
        crv.fXcoeff = parameters.getParameter(AWS_CURVE_X_COEFFS, i);
        crv.fYcoeff = parameters.getParameter(AWS_CURVE_Y_COEFFS, i);
        crv.fOffset = parameters.getParameter(AWS_CURVE_OFFSETS, i);
        crv.fBoundary = parameters.getParameter(AWS_CURVE_BOUNDARIES, i);
        lines.push_back(crv);
        ++i;
    }
    return curves;
}

IMG_RESULT ISPC::ModuleAWS::load(const ParameterList &parameters)
{
    const Sensor* sensor = NULL;
    if (pipeline)
    {
        sensor = pipeline->getSensor();
    }
    enabled = parameters.getParameter(AWS_ENABLED);

    debugMode = parameters.getParameter(AWS_DEBUG_MODE);

    fLog2_R_Qeff = parameters.getParameter(AWS_LOG2_R_QEFF);
    fLog2_B_Qeff = parameters.getParameter(AWS_LOG2_B_QEFF);

    fRedDarkThresh = parameters.getParameter(AWS_R_DARK_THRESH);
    fBlueDarkThresh = parameters.getParameter(AWS_B_DARK_THRESH);
    fGreenDarkThresh = parameters.getParameter(AWS_G_DARK_THRESH);

    fRedClipThresh = parameters.getParameter(AWS_R_CLIP_THRESH);
    fBlueClipThresh = parameters.getParameter(AWS_B_CLIP_THRESH);
    fGreenClipThresh = parameters.getParameter(AWS_G_CLIP_THRESH);

    fBbDist = parameters.getParameter(AWS_BB_DIST);

    ui16GridStartColumn = parameters.getParameter(AWS_TILE_START_COORDS, 0);
    ui16GridStartRow = parameters.getParameter(AWS_TILE_START_COORDS, 1);

    if(sensor) {
        int def = IMG_MAX_INT(AWS_TILE_WIDTH_MIN,
            sensor->uiWidth / AWS_NUM_GRID_TILES_HORIZ);
        ui16TileWidth = parameters.getParameter<short>(AWS_TILE_SIZE.name, 0,
            AWS_TILE_WIDTH_MIN, AWS_TILE_SIZE.max,
            def);
        def = IMG_MAX_INT(AWS_TILE_HEIGHT_MIN,
            sensor->uiHeight / AWS_NUM_GRID_TILES_VERT);
        ui16TileHeight = parameters.getParameter<short>(AWS_TILE_SIZE.name, 1,
            AWS_TILE_HEIGHT_MIN, AWS_TILE_SIZE.max,
            def);
    } else {
        // defaults loaded when called from constructor
        ui16TileWidth = parameters.getParameter(AWS_TILE_SIZE, 0);
        ui16TileHeight = parameters.getParameter(AWS_TILE_SIZE, 1);
    }

    fLines.clear();
    loadLinesParams(parameters, fLines);

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleAWS::save(ParameterList &parameters, SaveType t) const
{
    std::vector<std::string> values;
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ModuleAWS::getGroup();
    }

    parameters.addGroup("ModuleAWS", group);

    switch (t)
    {
    case SAVE_VAL:
    {
        parameters.addParameter(AWS_ENABLED, enabled);
        parameters.addParameter(AWS_DEBUG_MODE, debugMode);
        parameters.addParameter(AWS_LOG2_R_QEFF, fLog2_R_Qeff);
        parameters.addParameter(AWS_LOG2_B_QEFF, fLog2_B_Qeff);
        parameters.addParameter(AWS_R_DARK_THRESH, fRedDarkThresh);
        parameters.addParameter(AWS_G_DARK_THRESH, fGreenDarkThresh);
        parameters.addParameter(AWS_B_DARK_THRESH, fBlueDarkThresh);
        parameters.addParameter(AWS_R_CLIP_THRESH, fRedClipThresh);
        parameters.addParameter(AWS_G_CLIP_THRESH, fGreenClipThresh);
        parameters.addParameter(AWS_B_CLIP_THRESH, fBlueClipThresh);
        parameters.addParameter(AWS_BB_DIST, fBbDist);
        values.clear();
        values.push_back(toString(ui16GridStartColumn));
        values.push_back(toString(ui16GridStartRow));
        parameters.addParameter(AWS_TILE_START_COORDS, values);
        values.clear();
        values.push_back(toString(ui16TileWidth));
        values.push_back(toString(ui16TileHeight));
        parameters.addParameter(AWS_TILE_SIZE, values);

        std::vector<std::string> xCoeffs, yCoeffs, offsets, boundaries;

        int entries = fLines.size();
        for(lineIterator i=fLines.begin();i!=fLines.end();++i)
        {
            xCoeffs.push_back(toString(i->fXcoeff));
            yCoeffs.push_back(toString(i->fYcoeff));
            offsets.push_back(toString(i->fOffset));
            boundaries.push_back(toString(i->fBoundary));
        }
        parameters.addParameter(AWS_CURVES_NUM, entries);
        parameters.addParameter(AWS_CURVE_X_COEFFS, xCoeffs);
        parameters.addParameter(AWS_CURVE_Y_COEFFS, yCoeffs);
        parameters.addParameter(AWS_CURVE_OFFSETS, offsets);
        parameters.addParameter(AWS_CURVE_BOUNDARIES, boundaries);
        break;
    }

    case SAVE_MIN:
        parameters.addParameterMin(AWS_ENABLED);
        parameters.addParameterMin(AWS_DEBUG_MODE);
        parameters.addParameterMin(AWS_LOG2_R_QEFF);
        parameters.addParameterMin(AWS_LOG2_B_QEFF);
        parameters.addParameterMin(AWS_R_DARK_THRESH);
        parameters.addParameterMin(AWS_G_DARK_THRESH);
        parameters.addParameterMin(AWS_B_DARK_THRESH);
        parameters.addParameterMin(AWS_R_CLIP_THRESH);
        parameters.addParameterMin(AWS_G_CLIP_THRESH);
        parameters.addParameterMin(AWS_B_CLIP_THRESH);
        parameters.addParameterMin(AWS_BB_DIST);
        parameters.addParameterMin(AWS_TILE_START_COORDS);
        parameters.addParameterMin(AWS_TILE_SIZE);
        parameters.addParameterMin(AWS_CURVES_NUM);
        parameters.addParameterMin(AWS_CURVE_X_COEFFS);
        parameters.addParameterMin(AWS_CURVE_Y_COEFFS);
        parameters.addParameterMin(AWS_CURVE_OFFSETS);
        parameters.addParameterMin(AWS_CURVE_BOUNDARIES);
        break;

    case SAVE_MAX:
        parameters.addParameterMax(AWS_ENABLED);
        parameters.addParameterMax(AWS_DEBUG_MODE);
        parameters.addParameterMax(AWS_LOG2_R_QEFF);
        parameters.addParameterMax(AWS_LOG2_B_QEFF);
        parameters.addParameterMax(AWS_R_DARK_THRESH);
        parameters.addParameterMax(AWS_G_DARK_THRESH);
        parameters.addParameterMax(AWS_B_DARK_THRESH);
        parameters.addParameterMax(AWS_R_CLIP_THRESH);
        parameters.addParameterMax(AWS_G_CLIP_THRESH);
        parameters.addParameterMax(AWS_B_CLIP_THRESH);
        parameters.addParameterMax(AWS_BB_DIST);
        parameters.addParameterMax(AWS_TILE_START_COORDS);
        parameters.addParameterMax(AWS_TILE_SIZE);
        parameters.addParameterMax(AWS_CURVES_NUM);
        parameters.addParameterMax(AWS_CURVE_X_COEFFS);
        parameters.addParameterMax(AWS_CURVE_Y_COEFFS);
        parameters.addParameterMax(AWS_CURVE_OFFSETS);
        parameters.addParameterMax(AWS_CURVE_BOUNDARIES);
        break;

    case SAVE_DEF:
    {
        const Sensor* sensor = NULL;
        if (pipeline)
        {
            sensor = pipeline->getSensor();
        }
        parameters.addParameterDef(AWS_ENABLED);
        parameters.addParameterDef(AWS_DEBUG_MODE);
        parameters.addParameterDef(AWS_LOG2_R_QEFF);
        parameters.addParameterDef(AWS_LOG2_B_QEFF);
        parameters.addParameterDef(AWS_R_DARK_THRESH);
        parameters.addParameterDef(AWS_G_DARK_THRESH);
        parameters.addParameterDef(AWS_B_DARK_THRESH);
        parameters.addParameterDef(AWS_R_CLIP_THRESH);
        parameters.addParameterDef(AWS_G_CLIP_THRESH);
        parameters.addParameterDef(AWS_B_CLIP_THRESH);
        parameters.addParameterDef(AWS_BB_DIST);
        parameters.addParameterDef(AWS_TILE_START_COORDS);
        values.clear();
        if(sensor) {
            int def = IMG_MAX_INT(sensor->uiWidth / AWS_NUM_GRID_TILES_HORIZ,
                AWS_TILE_WIDTH_MIN);
            values.push_back(toString(def));
            def = IMG_MAX_INT(sensor->uiHeight / AWS_NUM_GRID_TILES_HORIZ,
                AWS_TILE_HEIGHT_MIN);
            values.push_back(toString(def));
        } else {
            values.push_back(toString(AWS_TILE_SIZE.def[0]));
            values.push_back(toString(AWS_TILE_SIZE.def[1]));
        }
        parameters.addParameter(AWS_TILE_SIZE, values);
        parameters.getParameter(AWS_TILE_SIZE.name)->setInfo(getParameterInfo(AWS_TILE_SIZE));
        parameters.addParameterDef(AWS_CURVES_NUM);
        parameters.addParameterDef(AWS_CURVE_X_COEFFS);
        parameters.addParameterDef(AWS_CURVE_Y_COEFFS);
        parameters.addParameterDef(AWS_CURVE_OFFSETS);
        parameters.addParameterDef(AWS_CURVE_BOUNDARIES);
        break;
    }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ModuleAWS::setup()
{
    LOG_PERF_IN();
    MC_PIPELINE *pMCPipeline = NULL;
    if (!pipeline)
    {
        MOD_LOG_ERROR("pipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }
    pMCPipeline = pipeline->getMCPipeline();
    if (!pMCPipeline)
    {
        MOD_LOG_ERROR("pMCPipeline not set!\n");
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    MC_AWS& aws = pMCPipeline->sAWS;
    aws.bEnable = static_cast<IMG_BOOL8>(enabled);
    aws.bDebugBitmap = static_cast<IMG_BOOL8>(debugMode);

    aws.fBbDist = fBbDist;

    aws.fLog2_B_Qeff = fLog2_B_Qeff;
    aws.fLog2_R_Qeff = fLog2_R_Qeff;

    aws.fBlueClipThresh = fBlueClipThresh;
    aws.fRedClipThresh = fRedClipThresh;
    aws.fGreenClipThresh = fGreenClipThresh;
    aws.fBlueDarkThresh = fBlueDarkThresh;
    aws.fRedDarkThresh = fRedDarkThresh;
    aws.fGreenDarkThresh = fGreenDarkThresh;

    // so far always setup tiles so they cover the whole sensor space
    aws.ui16GridTileWidth = ui16TileWidth;
    aws.ui16GridTileHeight = ui16TileHeight;

    if(fLines.size()>0)
    {
        // first disable all lines
        for(int i=0;i<AWS_LINE_SEG_N;++i)
        {
            aws.aCurveBoundary[i] = -16.0;
        }
        // sort the segments in ascending order of fBoundary
        fLines.sort();
        fLines.reverse(); // : change to reverse iterator
        int entries = ispc_min(fLines.size(), AWS_LINE_SEG_N);
        int i;
        lineIterator j;
        for (i=0, j=fLines.begin();i<entries;++i,++j)
        {
            aws.aCurveCoeffX[i] = j->fXcoeff;
            aws.aCurveCoeffY[i] = j->fYcoeff;
            aws.aCurveOffset[i] = j->fOffset;
            aws.aCurveBoundary[i] = j->fBoundary;
        }
    }
    this->setupFlag = true;
    LOG_PERF_OUT();
    return IMG_SUCCESS;
}
