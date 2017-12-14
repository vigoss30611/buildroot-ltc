/**
*******************************************************************************
@file ControlAWB_PID.cpp

@brief ISPC::ControlAWB_PID implementation

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
#include "ispc/ControlAWB_PID.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CTRL_AWB"

#include <cmath>
#include <string>

#include "ispc/ModuleWBS.h"
#include "ispc/ModuleWBC.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ColorCorrection.h"
#include "ispc/Sensor.h"
#include "ispc/Pipeline.h"
#include "ispc/ISPCDefs.h"

#ifdef INFOTM_ISP
#include <ispc/Camera.h>
  #if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) && defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
#include "ispc/ControlAE.h"
#include "ispc/ControlCMC.h"
  #endif //INFOTM_ENABLE_COLOR_MODE_CHANGE && INFOTM_ENABLE_GAIN_LEVEL_IDX
#endif //INFOTM_ISP
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

#ifdef INFOTM_AWB_PATCH
#define CLAMP(x, l, h) (((x) > (h)) ? (h) : (((x) < (l)) ? (l) : (x)))
#define MULTI_PASS_PID 1

const ISPC::ParamDef<float> ISPC::ControlAWB_PID::PID_MARGIN("WB_PID_MARGIN", 0.01f, 2.0f, 0.2f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::PID_KP("WB_PID_KP", 0.01f, 1.0f, 0.1f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::PID_KD("WB_PID_KD", 0.0f, 1.0f, 0.01f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::PID_KI("WB_PID_KI", 0.0f, 1.0f, 0.1f);
#endif //INFOTM_AWB_PATCH
#ifdef INFOTM_ISP
// The value of R, B MIN and MAX is decide by tuning result.
const ISPC::ParamDef<double> ISPC::ControlAWB_PID::PID_RGAIN_MIN("PID_RGAIN_MIN", 0.0f, 4.0f, 1.0f);
const ISPC::ParamDef<double> ISPC::ControlAWB_PID::PID_RGAIN_MAX("PID_RGAIN_MAX", 0.0f, 4.0f, 2.0f);
const ISPC::ParamDef<double> ISPC::ControlAWB_PID::PID_BGAIN_MIN("PID_BGAIN_MIN", 0.0f, 4.0f, 1.0f);
const ISPC::ParamDef<double> ISPC::ControlAWB_PID::PID_BGAIN_MAX("PID_BGAIN_MAX", 0.0f, 4.0f, 3.2f);

const ISPC::ParamDefSingle<bool> ISPC::ControlAWB_PID::DO_AWB_ENABLE("DO_AWB_ENABLE", true);
#endif //INFOTM_ISP
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
#define HW_SW_AWB_R_GAIN_RES_MAX     32     //(4 / 0.125)
#define HW_SW_AWB_B_GAIN_RES_MAX     32     //(4 / 0.125)
#define HW_SW_AWB_WEIGHT_MIN          0
#define HW_SW_AWB_WEIGHT_MAX         15
static const unsigned int HW_SW_AWB_WEIGHT_DEF[32] =
{
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD)
const ISPC::ParamDefSingle<bool> ISPC::ControlAWB_PID::SW_AWB_ENABLE("SW_AWB_ENABLE", false);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT("SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT", 1.0f, 16.0f, 3.0f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT("SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT", -1.0f, 1.0f, -0.3f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT("SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT", 0.0f, 1.0f, 0.4f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT("SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT", 1.0f, 16.0f, 4.0f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_STD_CCM_DARK_GAIN_LMT("SW_AWB_USE_STD_CCM_DARK_GAIN_LMT", 1.0f, 16.0f, 8.0f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT("SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT", -1.0f, 1.0f, -0.4f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT("SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT", 0.0f, 1.0f, 0.5f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT("SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT", -1.0f, 1.0f, -0.5f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT("SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT", 0.0f, 1.0f, 0.7f);
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::HW_SW_AWB_UPDATE_SPEED("HW_SW_AWB_UPDATE_SPEED", 0.0f, 1.0f, 0.08f);
const ISPC::ParamDef<float> ISPC::ControlAWB_PID::HW_SW_AWB_DROP_RATIO("HW_SW_AWB_DROP_RATIO", 0.0f, 1.0f, 0.999f);
const ISPC::ParamDefArray<unsigned int> ISPC::ControlAWB_PID::HW_SW_AWB_WEIGHTING_TBL("HW_SW_AWB_WEIGHTING_TBL", HW_SW_AWB_WEIGHT_MIN, HW_SW_AWB_WEIGHT_MAX, HW_SW_AWB_WEIGHT_DEF, HW_SW_AWB_R_GAIN_RES_MAX);
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)

const ISPC::ParamDef<unsigned> ISPC::ControlAWB_PID::AWB_FRAME_DELAY(
    "WB_FRAME_DELAY", 0, 200, 0);

static const double HWMAXGain = 4.0f;
static const double HWMINGain = 0.1f;

#ifdef INFOTM_OV_DEBUG
int Temperature = 0;
double rGain = 0.0;
double bGain = 0.0;
#endif

double ISPC::ControlAWB_PID::State::HWMaxGain()
{
    return HWMAXGain;
}

double ISPC::ControlAWB_PID::State::HWMinGain()
{
    return HWMINGain;
}

void ISPC::ControlAWB_PID::State::reset()
{
    flTargetRatio = 1.0;
    flLastError = 0;
    bClipped = false;
    bConverged = false;

    /*if (Ki != 0)
    {
    flIntegral = flMinGain / Ki;
    }
    else
    {
    LOG_WARNING("Ki==0!\n");
    flIntegral = 0;
    }*/
}

void ISPC::ControlAWB_PID::State::update(double Kp, double Kd, double Ki,
    double flIQE)
{
#if defined(INFOTM_AWB_PATCH)
	double mingain,maxgain;
#endif

    this->Kp = Kp;  // Proportional tuning parameter
    this->Kd = Kd;  // derivitive tuning parameter
    this->Ki = Ki;  // integral tuning parameter

#if defined(INFOTM_AWB_PATCH)
		this->flMaxGain=HWMAXGain;
		this->flMinGain=HWMINGain;
#else
    // min and max values need to be calculated with respect to the gains
    // already being applied to adjust for the quantum efficiency
    if (flIQE > 1.0)
    {
        this->flMaxGain = HWMAXGain / flIQE;
        this->flMinGain = HWMINGain * flIQE;
    }
    else
    {
        this->flMaxGain = HWMAXGain * flIQE;
        this->flMinGain = HWMINGain / flIQE;
    }
#endif

    if (Ki != 0)
    {
        flIntegral = flMinGain / Ki;
    }
    else
    {
        LOG_WARNING("Ki==0!\n");
        flIntegral = 0;
    }
}

ISPC::ParameterGroup ISPC::ControlAWB_PID::getGroup()
{
    ParameterGroup group;

    group.header = "// Auto White Balance parameters";

    // group.parameters.insert(AWB_TARGET_PIXELRATIO.name);
#ifdef INFOTM_AWB_PATCH
    group.parameters.insert(PID_MARGIN.name);
    group.parameters.insert(PID_KP.name);
    group.parameters.insert(PID_KD.name);
    group.parameters.insert(PID_KI.name);
#endif //INFOTM_AWB_PATCH
#ifdef INFOTM_ISP
    group.parameters.insert(PID_RGAIN_MIN.name);
    group.parameters.insert(PID_RGAIN_MAX.name);
    group.parameters.insert(PID_BGAIN_MIN.name);
    group.parameters.insert(PID_BGAIN_MAX.name);

    group.parameters.insert(DO_AWB_ENABLE.name);
#endif //INFOTM_ISP
#if defined(INFOTM_SW_AWB_METHOD)
    group.parameters.insert(SW_AWB_ENABLE.name);
    group.parameters.insert(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.name);
    group.parameters.insert(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.name);
    group.parameters.insert(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.name);
    group.parameters.insert(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.name);
    group.parameters.insert(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.name);
    group.parameters.insert(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.name);
    group.parameters.insert(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.name);
    group.parameters.insert(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.name);
    group.parameters.insert(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.name);
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
    group.parameters.insert(HW_SW_AWB_UPDATE_SPEED.name);
    group.parameters.insert(HW_SW_AWB_DROP_RATIO.name);
    for (int i = 0; i < HW_SW_AWB_B_GAIN_RES_MAX; i++) {
        group.parameters.insert(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).name);
    }
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
    group.parameters.insert(AWB_FRAME_DELAY.name);

    return group;
}

ISPC::ControlAWB_PID::ControlAWB_PID(const std::string &logname)
    : ControlAWB(logname), uiFramesToSkip(AWB_FRAME_DELAY.def)
{
    targetTemperature = 6500.0;  // always 65kK for AWB-PID

    bResetStates = true;

    sRGain.reset();
    sBGain.reset();

#ifdef INFOTM_ISP
    fRGainMin = fRGainMax = fBGainMin = fBGainMax = 0.0f;

  #if defined(INFOTM_SW_AWB_METHOD)
    bUseOriginalCCM = true;
    StdCCM = colorTempCorrection.getColorCorrection(6500.0);
    StdCCM.coefficients[0][0] = 1.0f;
    StdCCM.coefficients[0][1] = 0.0f;
    StdCCM.coefficients[0][2] = 0.0f;
    StdCCM.coefficients[1][0] = 0.0f;
    StdCCM.coefficients[1][1] = 1.0f;
    StdCCM.coefficients[1][2] = 0.0f;
    StdCCM.coefficients[2][0] = 0.0f;
    StdCCM.coefficients[2][1] = 0.0f;
    StdCCM.coefficients[2][2] = 1.0f;
    StdCCM.gains[0][0] = 1.0f;
    StdCCM.gains[0][1] = 1.0f;
    StdCCM.gains[0][2] = 1.0f;
    StdCCM.gains[0][3] = 1.0f;
    StdCCM.offsets[0][0] = -128;
    StdCCM.offsets[0][1] = -128;
    StdCCM.offsets[0][2] = -128;
  #endif //#if defined(INFOTM_SW_AWB_METHOD)
#endif //INFOTM_ISP
}

void  ISPC::ControlAWB_PID::initialiseAWBPID(double Kp, double Kd, double Ki,
    double flRIQE, double flBIQE)
{
#if defined(INFOTM_AWB_PATCH)
    double mingain = 0, maxgain = 0;
	
    sRGain.Kp=Kp;                // Proportional tuning parameter
    sBGain.Kp=Kp;                // Proportional tuning parameter
    sRGain.Kd=Kd;                // derivitive tuning parameter
    sBGain.Kd=Kd;                // derivitive tuning parameter
    sRGain.Ki=Ki;                // integral tuning parameter
    sBGain.Ki=Ki;                // integral tuning parameter
	
	sBGain.flMaxGain=HWMAXGain;
	sBGain.flMinGain=HWMINGain;
	sRGain.flMaxGain=HWMAXGain;
	sRGain.flMinGain=HWMINGain;
	
    mingain=ispc_max(sRGain.flMinGain,sBGain.flMinGain);
    maxgain=ispc_max(sRGain.flMaxGain,sBGain.flMaxGain);
	
    sRGain.flMinGain=sBGain.flMinGain=mingain;
    sRGain.flMaxGain=sBGain.flMaxGain=maxgain;
	
	if (Ki != 0)
    {
		sRGain.flIntegral = sRGain.flMinGain / Ki;
        sBGain.flIntegral = sBGain.flMinGain / Ki;
    }
    else
    {
        LOG_WARNING("Ki==0!\n");
		sRGain.flIntegral = 0;
        sBGain.flIntegral = 0;
    }
	
#else
    sBGain.update(Kp, Kd, Ki, flBIQE);
    sRGain.update(Kp, Kd, Ki, flRIQE);
#endif //INFOTM_AWB_PATCH

    // to force next frame to be processed
    resetNumberOfSkipped();
}

unsigned int ISPC::ControlAWB_PID::getNumberOfSkipped() const
{
    return uiFramesSkipped;
}

unsigned int ISPC::ControlAWB_PID::getNumberToSkip() const
{
    return uiFramesToSkip;
}

void ISPC::ControlAWB_PID::resetNumberOfSkipped()
{
    uiFramesSkipped = uiFramesToSkip;
}

const ISPC::ControlAWB_PID::State& ISPC::ControlAWB_PID::getRedState() const
{
    return sRGain;
}

const ISPC::ControlAWB_PID::State& ISPC::ControlAWB_PID::getBlueState() const
{
    return sBGain;
}

bool ISPC::ControlAWB_PID::hasConverged() const
{
    return sRGain.bConverged && sBGain.bConverged;
}

bool ISPC::ControlAWB_PID::getResetStates() const
{
    return bResetStates;
}

void ISPC::ControlAWB_PID::setResetStates(bool b)
{
    bResetStates = b;
}

#ifdef INFOTM_AWB_PATCH
void ISPC::ControlAWB_PID::LimitGains(double margin,double targetx,double targety,double &minxval,double &maxxval,double &minyval,double &maxyval)
{
	float m,minc,maxc,A,B,C,recip;
	float c;
	float distance,px,py,deltax,deltay;

	m=(p2y-p1y)/(p2x-p1x);
	recip=1.0/m;
	c=p1y-m*p1x;

	A=-m;
	B=1;
	C=-c;

#ifdef USE_MATH_NEON
	distance=fabs(A*targetx+B*targety+C)/(double)sqrtf_neon((float)(A*A+B*B));
	deltax=(double)sqrtf_neon((float)(margin*margin/(1+recip*recip)));
	deltay=fabs(deltax*recip);
#else
	distance=fabs(A*targetx+B*targety+C)/sqrt(A*A+B*B);
	deltax=sqrt(margin*margin/(1+recip*recip));
	deltay=fabs(deltax*recip);
#endif

	px=(B*(B*targetx-A*targety)-A*C)/(A*A+B*B);
	py=(A*(-B*targetx+A*targety)-B*C)/(A*A+B*B);

	minxval=CLAMP((px-deltax),minxval,maxxval);
	maxxval=CLAMP((px+deltax),minxval,maxxval);
	minyval=CLAMP((py-deltay),minyval,maxyval);
	maxyval=CLAMP((py+deltay),minyval,maxyval);
}

#ifdef INFOTM_ISP
void ISPC::ControlAWB_PID::enableAWB(bool enable)
{
	this->doAwb = enable;
}

bool ISPC::ControlAWB_PID::IsAWBenabled() const
{
	return this->doAwb;
}

#endif
double ISPC::ControlAWB_PID::PIDGain(ISPC::ControlAWB_PID::State* sState, double flCurrentRatio,bool bUpdateInternal,double flNewMinGain,double flNewMaxGain)
{
    double flError;
    double flDerivative;
    double flOutput,flOutputUnclipped;
	double flClampedMinGain,flClampedMaxGain;
	double flTempIntegral,flTempLastError;
	bool bTempCLipped,bTempConverged;
    static const double epsilonsquared = 0.01*0.01;

    flTempIntegral=sState->flIntegral;
	bTempConverged=sState->bConverged;
	flTempLastError=sState->flLastError;
#ifdef IMG_AWB_PATCH_DEBUG
    /******** Dump the state of the variables for the PID function*****/
    printf("flIntegral=%f, sState.bConverged=%s,sState->flLastError=%f\n Ki=%f, Kp=%f,Kd=%f, \nflCurrentRatio=%f,bUpdateInternal=%s,flNewMingGain=%f,flNewMaxGain=%f\n",
        sState->flIntegral,sState->bConverged? "TRUE":"FALSE",sState->flLastError,sState->Ki,sState->Kp,sState->Kd,flCurrentRatio,bUpdateInternal? "TRUE":"FALSE",flNewMinGain,flNewMaxGain);
    /*******************************************************************/
#endif
	flError = sState->flTargetRatio-flCurrentRatio;

	flClampedMinGain=CLAMP(sState->flMinGain,flNewMinGain,flNewMaxGain);
	flClampedMaxGain=CLAMP(sState->flMaxGain,flNewMinGain,flNewMaxGain);
	
    if(flError*flError > epsilonsquared )
    {
        flTempIntegral+=flError;
        if(sState->Ki>0.0)
        {
            flTempIntegral=ispc_max(flTempIntegral,flClampedMinGain/sState->Ki);
            flTempIntegral=ispc_min(flTempIntegral,flClampedMaxGain/sState->Ki);
        }
        bTempConverged=false;
    }
    else
    {
        bTempConverged=true;
    }
    flDerivative= (flError-flTempLastError);
    flOutputUnclipped= (sState->Kp*flError +
            sState->Kd*flDerivative +
            sState->Ki * flTempIntegral);
    flOutput=ispc_max(flOutputUnclipped,flClampedMinGain);
    flOutput=ispc_min(flClampedMaxGain,flOutput);

    MOD_LOG_DEBUG("\nPID: current = %f, error = %f, updated output = %f, "\
            "%s,gainrange %f-%f, unclipped output = %f ",
            flCurrentRatio,flError,flOutput,
            (flOutput==flOutputUnclipped?"no clipping\n":"output clipped\n"),
            flClampedMinGain,flClampedMaxGain,flOutputUnclipped);

    bTempCLipped=(flOutput!=flOutputUnclipped);
    flTempLastError=flError;
	
	if(bUpdateInternal)
	{
		sState->flIntegral=flTempIntegral;
		sState->bConverged=bTempConverged;
		sState->flLastError=flTempLastError;
	}
  return flOutput;
}

#else //INFOTM_AWB_PATCH

double ISPC::ControlAWB_PID::PIDGain(ISPC::ControlAWB_PID::State* sState,
    double flCurrentRatio)
{
    double flError;
    double flDerivative;
    double flOutput, flOutputUnclipped;
    static const double epsilonsquared = 0.01*0.01;

    flError = sState->flTargetRatio - flCurrentRatio;

    if (flError*flError > epsilonsquared)
    {
        sState->flIntegral += flError;
        sState->flIntegral = ispc_max(sState->flIntegral,
            sState->flMinGain / sState->Ki);
        sState->flIntegral = ispc_min(sState->flIntegral,
            sState->flMaxGain / sState->Ki);
        sState->bConverged = false;
    }
    else
    {
        sState->bConverged = true;
    }
    flDerivative = (flError - sState->flLastError);
    flOutputUnclipped = (sState->Kp*flError +
        sState->Kd*flDerivative +
        sState->Ki * sState->flIntegral);
    flOutput = ispc_max(flOutputUnclipped, sState->flMinGain);
    flOutput = ispc_min(sState->flMaxGain, flOutput);

    LOG_DEBUG("\nPID: current = %f, error = %f, updated output = %f, "\
        "%s,gainrange %f-%f, unclipped output = %f ",
        flCurrentRatio, flError, flOutput,
        (flOutput == flOutputUnclipped ? "no clipping\n" : "output clipped\n"),
        sState->flMinGain, sState->flMaxGain, flOutputUnclipped);

    sState->bClipped = (flOutput != flOutputUnclipped);
    sState->flLastError = flError;
    return flOutput;
}
#endif //INFOTM_AWB_PATCH


IMG_RESULT ISPC::ControlAWB_PID::load(const ParameterList &parameters)
{
#ifdef INFOTM_AWB_PATCH
    int numCorrections;
#else
	static const double Kp = 0.1;  //  should be parameters
	static const double Kd = 0.01;	//	should be parameters
	static const double Ki = 0.1;  //  should be parameters
#endif //INFOTM_AWB_PATCH

    // Pass the parameter list to the ColorCorrection auxiliary object so
    // it can load parameters from the parameters as well.
    colorTempCorrection.loadParameters(parameters);
    if (colorTempCorrection.getCorrectionIndex(6500.0) < 0)
    {
        LOG_WARNING("loaded temperature correction does not have 6500K" \
            " entry!\n");
        // return IMG_ERROR_NOT_SUPPORTED;
    }
    // load the D65 parameters
    Uncorrected = colorTempCorrection.getColorCorrection(6500.0);

    this->uiFramesToSkip = parameters.getParameter(AWB_FRAME_DELAY);

#ifdef INFOTM_ENABLE_ISP_DEBUG
    if (uiFramesToSkip != AWB_FRAME_DELAY.def)
    {
        MOD_LOG_WARNING("number of frames to skipped is not expected to " \
            "change from default!\n");
    }
#endif //INFOTM_ENABLE_ISP_DEBUG

#ifdef INFOTM_AWB_PATCH
	numCorrections = colorTempCorrection.size();
    this->pid_KP = parameters.getParameter(PID_KP);
    this->pid_KD = parameters.getParameter(PID_KD);
    this->pid_KI = parameters.getParameter(PID_KI);
#else

    initialiseAWBPID(Kp, Kd, Ki,
        Uncorrected.gains[0][0], Uncorrected.gains[0][3]);
#endif//INFOTM_AWB_PATCH


#ifdef INFOTM_ISP
    this->fRGainMin = parameters.getParameter(PID_RGAIN_MIN);
    this->fRGainMax = parameters.getParameter(PID_RGAIN_MAX);
    this->fBGainMin = parameters.getParameter(PID_BGAIN_MIN);
    this->fBGainMax = parameters.getParameter(PID_BGAIN_MAX);

    this->doAwb = parameters.getParameter(DO_AWB_ENABLE);
#endif //INFOTM_ISP

#if defined(INFOTM_SW_AWB_METHOD)
    // Software AWB R and B gain initialise.
    sw_awb_gain_set(Uncorrected.gains[0][0], Uncorrected.gains[0][3]);
    this->bSwAwbEnable = parameters.getParameter(SW_AWB_ENABLE);
    this->dbSwAwbUseOriCcmNrmlGainLmt = parameters.getParameter(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT);
    this->dbSwAwbUseOriCcmNrmlBrightnessLmt = parameters.getParameter(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT);
    this->dbSwAwbUseOriCcmNrmlUnderExposeLmt = parameters.getParameter(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT);
    this->dbSwAwbUseStdCcmLowLuxGainLmt = parameters.getParameter(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT);
    this->dbSwAwbUseStdCcmDarkGainLmt = parameters.getParameter(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT);
    this->dbSwAwbUseStdCcmLowLuxBrightnessLmt = parameters.getParameter(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT);
    this->dbSwAwbUseStdCcmLowLuxUnderExposeLmt = parameters.getParameter(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT);
    this->dbSwAwbUseStdCcmDarkBrightnessLmt = parameters.getParameter(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT);
    this->dbSwAwbUseStdCcmDarkUnderExposeLmt = parameters.getParameter(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT);
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
    this->dbHwSwAwbUpdateSpeed = parameters.getParameter(HW_SW_AWB_UPDATE_SPEED);
    this->dbHwSwAwbDropRatio = parameters.getParameter(HW_SW_AWB_DROP_RATIO);
    for (int i = 0 ; i < HW_SW_AWB_B_GAIN_RES_MAX ; i++) {
        for (int j = 0 ; j < HW_SW_AWB_R_GAIN_RES_MAX ; j++) {
            this->uiHwSwAwbWeightingTable[i][j] = parameters.getParameter(HW_SW_AWB_WEIGHTING_TBL.indexed2(i), j);
        }
    }
//    printf("===== HW and SW AWB Weighting table start =====\n");
//    for (int i=0; i<32; i++) {
//        for (int j=0; j<32; j++) {
//            printf("%2d", this->uiHwSwAwbWeightingTable[i][j]);
//        }
//        printf("\n");
//    }
//    printf("===== HW and SW AWB Weighting table end =====\n");
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)

#ifdef INFOTM_AWB_PATCH
	p1x=currentCCM.gains[0][3];
	p1y=currentCCM.gains[0][0];

	currentCCM=colorTempCorrection.getCorrection(numCorrections-1);
	p2x=currentCCM.gains[0][3];
	p2y=currentCCM.gains[0][0];

	minxgain=ispc_min(p1x,p2x);
	maxxgain=ispc_max(p1x,p2x);
	minygain=ispc_min(p1y,p2y);
	maxygain=ispc_max(p1y,p2y);
	//printf("XRange = %f,%f\n YRange=%f,%f\n",minxgain,maxxgain,minygain,maxygain);
    initialiseAWBPID(this->pid_KP, this->pid_KD, this->pid_KI,
            Uncorrected.gains[0][0],Uncorrected.gains[0][3]);

    currentCCM=colorTempCorrection.getCorrection(0);
	this->margin = parameters.getParameter(PID_MARGIN);
#endif //INFOTM_AWB_PATCH

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlAWB_PID::save(ParameterList &parameters,
    ModuleBase::SaveType t) const
{
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
    int i, j;
    std::vector<std::string> values;
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
    static ParameterGroup group;

    if (0 == group.parameters.size())
    {
        group = ControlAWB_PID::getGroup();
    }

    parameters.addGroup("ControlAWB_PID", group);

    switch (t)
    {
    case ModuleBase::SAVE_VAL:
#ifdef INFOTM_AWB_PATCH
		parameters.addParameter(Parameter(PID_MARGIN.name, toString(this->margin)));
		parameters.addParameter(Parameter(PID_KP.name, toString(this->pid_KP)));
		parameters.addParameter(Parameter(PID_KD.name, toString(this->pid_KD)));
		parameters.addParameter(Parameter(PID_KI.name, toString(this->pid_KI)));
#endif //INFOTM_AWB_PATCH
#ifdef INFOTM_ISP
		parameters.addParameter(Parameter(PID_RGAIN_MIN.name, toString(this->fRGainMin)));
		parameters.addParameter(Parameter(PID_RGAIN_MAX.name, toString(this->fRGainMax)));
		parameters.addParameter(Parameter(PID_BGAIN_MIN.name, toString(this->fBGainMin)));
		parameters.addParameter(Parameter(PID_BGAIN_MAX.name, toString(this->fBGainMax)));

        parameters.addParameter(Parameter(DO_AWB_ENABLE.name, toString(this->doAwb)));
#endif //INFOTM_ISP
#if defined(INFOTM_SW_AWB_METHOD)
        parameters.addParameter(Parameter(SW_AWB_ENABLE.name, toString(this->bSwAwbEnable)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.name, toString(this->dbSwAwbUseOriCcmNrmlGainLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.name, toString(this->dbSwAwbUseOriCcmNrmlBrightnessLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.name, toString(this->dbSwAwbUseOriCcmNrmlUnderExposeLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.name, toString(this->dbSwAwbUseStdCcmLowLuxGainLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.name, toString(this->dbSwAwbUseStdCcmDarkGainLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.name, toString(this->dbSwAwbUseStdCcmLowLuxBrightnessLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.name, toString(this->dbSwAwbUseStdCcmLowLuxUnderExposeLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.name, toString(this->dbSwAwbUseStdCcmDarkBrightnessLmt)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.name, toString(this->dbSwAwbUseStdCcmDarkUnderExposeLmt)));
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(HW_SW_AWB_UPDATE_SPEED.name, toString(this->dbHwSwAwbUpdateSpeed)));
        parameters.addParameter(Parameter(HW_SW_AWB_DROP_RATIO.name, toString(this->dbHwSwAwbDropRatio)));
        for (i = 0 ; i < HW_SW_AWB_B_GAIN_RES_MAX ; i++) {
            values.clear();
            for (j = 0 ; j < HW_SW_AWB_R_GAIN_RES_MAX ; j++) {
                values.push_back(toString(this->uiHwSwAwbWeightingTable[i][j]));
            }
            parameters.addParameter(Parameter(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).name, values));
        }
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(AWB_FRAME_DELAY.name,
            toString(uiFramesSkipped)));
        break;

    case ModuleBase::SAVE_MIN:
#ifdef INFOTM_AWB_PATCH
		parameters.addParameter(Parameter(PID_MARGIN.name, toString(PID_MARGIN.min)));
		parameters.addParameter(Parameter(PID_KP.name, toString(PID_KP.min)));
		parameters.addParameter(Parameter(PID_KD.name, toString(PID_KD.min)));
		parameters.addParameter(Parameter(PID_KI.name, toString(PID_KI.min)));
#endif //INFOTM_AWB_PATCH
#ifdef INFOTM_ISP
		parameters.addParameter(Parameter(PID_RGAIN_MIN.name, toString(PID_RGAIN_MIN.min)));
		parameters.addParameter(Parameter(PID_RGAIN_MAX.name, toString(PID_RGAIN_MAX.min)));
		parameters.addParameter(Parameter(PID_BGAIN_MIN.name, toString(PID_BGAIN_MIN.min)));
		parameters.addParameter(Parameter(PID_BGAIN_MAX.name, toString(PID_BGAIN_MAX.min)));

        parameters.addParameter(Parameter(DO_AWB_ENABLE.name, toString(DO_AWB_ENABLE.def)));
#endif //INFOTM_ISP
#if defined(INFOTM_SW_AWB_METHOD)
        parameters.addParameter(Parameter(SW_AWB_ENABLE.name, toString(SW_AWB_ENABLE.def)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.min)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.min)));
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(HW_SW_AWB_UPDATE_SPEED.name, toString(HW_SW_AWB_UPDATE_SPEED.min)));
        parameters.addParameter(Parameter(HW_SW_AWB_DROP_RATIO.name, toString(HW_SW_AWB_DROP_RATIO.min)));
        for (i = 0 ; i < HW_SW_AWB_B_GAIN_RES_MAX ; i++) {
            values.clear();
            for (j = 0 ; j < HW_SW_AWB_R_GAIN_RES_MAX ; j++) {
                values.push_back(toString(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).min));
            }
            parameters.addParameter(Parameter(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).name, values));
        }
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(AWB_FRAME_DELAY.name,
            toString(AWB_FRAME_DELAY.min)));
        break;

    case ModuleBase::SAVE_MAX:
#ifdef INFOTM_AWB_PATCH
        parameters.addParameter(Parameter(PID_MARGIN.name, toString(PID_MARGIN.max)));
        parameters.addParameter(Parameter(PID_KP.name, toString(PID_KP.max)));
        parameters.addParameter(Parameter(PID_KD.name, toString(PID_KD.max)));
        parameters.addParameter(Parameter(PID_KI.name, toString(PID_KI.max)));
#endif //INFOTM_AWB_PATCH
#ifdef INFOTM_ISP
		parameters.addParameter(Parameter(PID_RGAIN_MIN.name, toString(PID_RGAIN_MIN.max)));
		parameters.addParameter(Parameter(PID_RGAIN_MAX.name, toString(PID_RGAIN_MAX.max)));
		parameters.addParameter(Parameter(PID_BGAIN_MIN.name, toString(PID_BGAIN_MIN.max)));
		parameters.addParameter(Parameter(PID_BGAIN_MAX.name, toString(PID_BGAIN_MAX.max)));

        parameters.addParameter(Parameter(DO_AWB_ENABLE.name, toString(DO_AWB_ENABLE.def)));
#endif //INFOTM_ISP
#if defined(INFOTM_SW_AWB_METHOD)
        parameters.addParameter(Parameter(SW_AWB_ENABLE.name, toString(SW_AWB_ENABLE.def)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.max)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.max)));
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(HW_SW_AWB_UPDATE_SPEED.name, toString(HW_SW_AWB_UPDATE_SPEED.max)));
        parameters.addParameter(Parameter(HW_SW_AWB_DROP_RATIO.name, toString(HW_SW_AWB_DROP_RATIO.max)));
        for (i = 0 ; i < HW_SW_AWB_B_GAIN_RES_MAX ; i++) {
            values.clear();
            for (j = 0 ; j < HW_SW_AWB_R_GAIN_RES_MAX ; j++) {
                values.push_back(toString(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).max));
            }
            parameters.addParameter(Parameter(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).name, values));
        }
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(AWB_FRAME_DELAY.name,
            toString(AWB_FRAME_DELAY.max)));
        break;

    case ModuleBase::SAVE_DEF:
#ifdef INFOTM_AWB_PATCH
        parameters.addParameter(Parameter(PID_MARGIN.name, toString(PID_MARGIN.def) + " // " + getParameterInfo(PID_MARGIN)));
        parameters.addParameter(Parameter(PID_KP.name, toString(PID_KP.def) + " // " + getParameterInfo(PID_KP)));
        parameters.addParameter(Parameter(PID_KD.name, toString(PID_KD.def) + " // " + getParameterInfo(PID_KD)));
        parameters.addParameter(Parameter(PID_KI.name, toString(PID_KI.def) + " // " + getParameterInfo(PID_KI)));
#endif //INFOTM_AWB_PATCH
#ifdef INFOTM_ISP
        parameters.addParameter(Parameter(PID_RGAIN_MIN.name, toString(PID_RGAIN_MIN.def) + " // " + getParameterInfo(PID_RGAIN_MIN)));
        parameters.addParameter(Parameter(PID_RGAIN_MAX.name, toString(PID_RGAIN_MAX.def) + " // " + getParameterInfo(PID_RGAIN_MAX)));
        parameters.addParameter(Parameter(PID_BGAIN_MIN.name, toString(PID_BGAIN_MIN.def) + " // " + getParameterInfo(PID_BGAIN_MIN)));
        parameters.addParameter(Parameter(PID_BGAIN_MAX.name, toString(PID_BGAIN_MAX.def) + " // " + getParameterInfo(PID_BGAIN_MAX)));

        parameters.addParameter(Parameter(DO_AWB_ENABLE.name, toString(DO_AWB_ENABLE.def) + " // " + getParameterInfo(DO_AWB_ENABLE)));
#endif //INFOTM_ISP
#if defined(INFOTM_SW_AWB_METHOD)
        parameters.addParameter(Parameter(SW_AWB_ENABLE.name, toString(SW_AWB_ENABLE.def) + " // " + getParameterInfo(SW_AWB_ENABLE)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_STD_CCM_DARK_GAIN_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT)));
        parameters.addParameter(Parameter(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.name, toString(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT.def) + " // " + getParameterInfo(SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT)));
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(HW_SW_AWB_UPDATE_SPEED.name, toString(HW_SW_AWB_UPDATE_SPEED.def) + " // " + getParameterInfo(HW_SW_AWB_UPDATE_SPEED)));
        parameters.addParameter(Parameter(HW_SW_AWB_DROP_RATIO.name, toString(HW_SW_AWB_DROP_RATIO.def) + " // " + getParameterInfo(HW_SW_AWB_DROP_RATIO)));
        for (i = 0 ; i < HW_SW_AWB_B_GAIN_RES_MAX ; i++) {
            values.clear();
            for (j = 0 ; j < HW_SW_AWB_R_GAIN_RES_MAX ; j++) {
                values.push_back(toString(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).def[j]));
            }
            values.push_back("// " + getParameterInfo(HW_SW_AWB_WEIGHTING_TBL.indexed2(i)));
            parameters.addParameter(Parameter(HW_SW_AWB_WEIGHTING_TBL.indexed2(i).name, values));
        }
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
        parameters.addParameter(Parameter(AWB_FRAME_DELAY.name,
            toString(AWB_FRAME_DELAY.def)
            + " // " + getParameterInfo(AWB_FRAME_DELAY)));
        break;
    }

    return colorTempCorrection.saveParameters(parameters, t);
}

void ISPC::ControlAWB_PID::enableControl(bool enable)
{
    if (bResetStates)
    {
        sRGain.reset();
        sBGain.reset();
    }

    ControlModule::enableControl(enable);
}

#ifdef USE_SMALL_GAIN_START_AWB
extern double GetCurMinRGain();
extern double GetCurMaxRGain();
extern double GetCurMinBGain();
extern double GetCurMaxBGain();
#endif


#ifdef INFOTM_ISP
  #if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
static const IMG_UINT16 SW_AWB_v[16] = {0, 8, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 10, 6};
static const double SW_AWB_s[16] = {0.5, 0.44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.07, -0.25, -0.25, -0.25};
  #endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_HW_AWB_METHOD)
#endif //INFOTM_ISP

#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlAWB_PID::update(const Metadata &metadata, const Metadata &metadata2)
#else
IMG_RESULT ISPC::ControlAWB_PID::update(const Metadata &metadata)
#endif //INFOTM_DUAL_SENSOR
{
    double imagePixels = static_cast<double>(this->imageTotalPixels);
    double HLW_R, HLW_G, HLW_B, WP_R, WP_G, WP_B, AC_R, AC_G, AC_B;
    double CONTROL_R, CONTROL_G, CONTROL_B;
    double rgain, bgain, ggain;
#if defined(INFOTM_AWB_PATCH)
		double totStats;
		double mixAC;
		double mixWP;
		double mix_HLW;
#endif

    if (uiFramesSkipped < uiFramesToSkip)
    {
        // skip this frame
#ifdef INFOTM_ENABLE_ISP_DEBUG
        MOD_LOG_INFO("skipping frame - skipped %u/%u",
            uiFramesSkipped, uiFramesToSkip);
#endif
        uiFramesSkipped++;
        return IMG_SUCCESS;
    }
    uiFramesSkipped = 0;

    // Estimation of new threshold for the WB statistics
    estimateWPThresholds(metadata, imagePixels, this->targetPixelRatio,
        this->thresholdInfoR, this->thresholdInfoG, this->thresholdInfoB,
        WPThresholdR, WPThresholdG, WPThresholdB);

    this->HLWThreshold = estimateHLWThreshold(metadata, imagePixels,
        this->targetPixelRatio, this->thresholdInfoY);
    // program the calculated statistics in the pipeline
#if defined(INFOTM_AWB_PATCH)
	configureStatistics();
#else    
    if (!configured)
    {
        MOD_LOG_WARNING("ControlAWB Statistics were not configured! Trying "\
            "to configure them now.");
        configureStatistics();
    }
#endif

    switch (correctionMode)
    {
    case WB_NONE:  // no correction
        CONTROL_R = CONTROL_G = CONTROL_B = 1;
        break;

    case WB_AC:  // AC Correction
        getACAverages(metadata, imagePixels, AC_R, AC_G, AC_B);
        CONTROL_R = AC_R;
        CONTROL_G = AC_G;
        CONTROL_B = AC_B;
        break;

    case WB_WP:  // WP Correction
        getWPAverages(metadata, imagePixels, WP_R, WP_G, WP_B);
        CONTROL_R = WP_R;
        CONTROL_G = WP_G;
        CONTROL_B = WP_B;
        break;

    case WB_HLW:  // HLW Correction
        getHLWAverages(metadata, imagePixels, HLW_R, HLW_G, HLW_B);
        CONTROL_R = HLW_R;
        CONTROL_G = HLW_G;
        CONTROL_B = HLW_B;
        break;

    case WB_COMBINED:  // Combined Correction
        // Calculate the RGB illuminant estimation with different methods
        getHLWAverages(metadata, imagePixels, HLW_R, HLW_G, HLW_B);
        getWPAverages(metadata, imagePixels, WP_R, WP_G, WP_B);
        getACAverages(metadata, imagePixels, AC_R, AC_G, AC_B);

#if defined(INFOTM_AWB_PATCH)
    #if 1
        totStats=HLW_G+WP_G+AC_G;
        mixAC=AC_G/totStats;
        mixWP=WP_G/totStats;
        mix_HLW=HLW_G/totStats;

        CONTROL_R=mixAC*AC_R/AC_G + mixWP*WP_R/WP_G + mix_HLW*HLW_R/HLW_G;
        CONTROL_G=mixAC*AC_G/AC_G + mixWP*WP_G/WP_G + mix_HLW*HLW_G/HLW_G;
        CONTROL_B=mixAC*AC_B/AC_G + mixWP*WP_B/WP_G + mix_HLW*HLW_B/HLW_G;
    #else
        CONTROL_R=AC_R*AC_R/(AC_R + WP_R + HLW_R) + WP_R*WP_R/(AC_R + WP_R + HLW_R) + HLW_R*HLW_R/(AC_R + WP_R + HLW_R);
        CONTROL_G=AC_G*AC_G/(AC_G + WP_G + HLW_G) + WP_G*WP_G/(AC_G + WP_G + HLW_G) + HLW_G*HLW_G/(AC_G + WP_G + HLW_G);
        CONTROL_B=AC_B*AC_B/(AC_B + WP_B + HLW_B) + WP_B*WP_B/(AC_B + WP_B + HLW_B) + HLW_B*HLW_B/(AC_B + WP_B + HLW_B);

        ///printf("==>AC_R %f WP_R %f HLW_R %f<==\n", AC_R, WP_R, HLW_R);
        ///printf("==>AC_G %f WP_G %f HLW_G %f<==\n", AC_G, WP_G, HLW_G);
        ///printf("==>AC_B %f WP_B %f HLW_B %f<==\n", AC_B, WP_B, HLW_B);
    #endif //1
#else //INFOTM_AWB_PATCH
        CONTROL_R = 0.25*AC_R + 0.4*WP_R + 0.35*HLW_R;
        CONTROL_G = 0.25*AC_G + 0.4*WP_G + 0.35*HLW_G;
        CONTROL_B = 0.25*AC_B + 0.4*WP_B + 0.35*HLW_B;
#endif //INFOTM_AWB_PATCH
        break;

    default:
        return IMG_ERROR_VALUE_OUT_OF_RANGE;
    }

    MOD_LOG_DEBUG("PID Control averages R=%f G= %f, B=%f\n",
        CONTROL_R, CONTROL_G, CONTROL_B);
    // run the PID loop
#ifdef INFOTM_AWB_PATCH
	double rmin, rmax, bmin, bmax, oldrgain, oldbgain, rcliperror, bcliperror;

	rmin = sRGain.flMinGain;
	rmax = sRGain.flMaxGain;
	bmin = sBGain.flMinGain;
	bmax = sBGain.flMaxGain;
  #ifdef MULTI_PASS_PID
	#ifndef INFOTM_ISP
	rgain = PIDGain(&sRGain, CONTROL_R / CONTROL_G, false, sRGain.flMinGain, sRGain.flMaxGain);
	bgain = PIDGain(&sBGain, CONTROL_B / CONTROL_G, false, sBGain.flMinGain, sBGain.flMaxGain);
	#else
	if (fRGainMin == 0.0f && fRGainMax == 0.0f && fBGainMin == 0.0f && fBGainMax == 0.0f)
	{
		rgain = PIDGain(&sRGain, CONTROL_R / CONTROL_G, false, sRGain.flMinGain, sRGain.flMaxGain);
		bgain = PIDGain(&sBGain, CONTROL_B / CONTROL_G, false, sBGain.flMinGain, sBGain.flMaxGain);
	}
	else
	{
		LimitGains(margin, rgain, bgain, rmin, rmax, bmin, bmax);
		rgain=PIDGain(&sRGain, CONTROL_R / CONTROL_G, true, fRGainMin, fRGainMax); //from setup file
		bgain=PIDGain(&sBGain, CONTROL_B  /CONTROL_G, true, fBGainMin, fBGainMax);
	}
	#endif //INFOTM_ISP
  #else
	rgain = PIDGain(&sRGain, CONTROL_R / CONTROL_G, true, sRGain.flMinGain, sRGain.flMaxGain);
	bgain = PIDGain(&sBGain, CONTROL_B / CONTROL_G, true, sBGain.flMinGain, sBGain.flMaxGain);

	LimitGains(margin, rgain, bgain, rmin, rmax, bmin, bmax);

	oldrgain = rgain;
	oldbgain = bgain;
	rgain = CLAMP(rgain, rmin, rmax);
	bgain = CLAMP(bgain, bmin, bmax);

	rcliperror = rgain-oldrgain;
	bcliperror = bgain-oldbgain;
	if (rcliperror != 0.0)
	{
		sRGain.flIntegral = 1 / sRGain.Ki;
	}
	if (bcliperror != 0.0)
	{
		sBGain.flIntegral = 1 / sBGain.Ki;
	}
  #endif //MULTI_PASS_PID
#else
    rgain = PIDGain(&sRGain, CONTROL_R / CONTROL_G);
    bgain = PIDGain(&sBGain, CONTROL_B / CONTROL_G);
#endif //INFOTM_AWB_PATCH
    ggain = 1;

    MOD_LOG_DEBUG("rgain = %f, bgain=%f\n", rgain, bgain);

    /*
    double minval = ispc_ispc_min(ispc_ispc_min(rgain,ggain),bgain);
    double maxval = ispc_ispc_max(ispc_ispc_max(rgain,ggain),bgain);

    if (maxval*(1/minval)<=4.0)
    {
        rgain=rgain*(1/minval);
        ggain=ggain*(1/minval);
        bgain=bgain*(1/minval);
    }
    */

#ifdef INFOTM_SW_AWB_METHOD
    if (this->bSwAwbEnable) {
        dbImg_rgain = rgain;
        dbImg_bgain = bgain;
        rgain = dbSw_rgain;
        bgain = dbSw_bgain;

  #if defined(ENABLE_SW_AWB_DEBUG_MSG)
        double sw_measuredTemperature = colorTempCorrection.getCCT(1/dbSw_rgain, 1/ggain, 1/dbSw_bgain);
        double sw_measuredTemperature_test = colorTempCorrection.getCCT(
                1/(rgain/Uncorrected.gains[0][0]), 1/ggain, 1/(bgain/Uncorrected.gains[0][3]));

        printf("SW measuredTemperature = %f\n", sw_measuredTemperature);
        printf("SW measuredTemperature_test = %f\n", sw_measuredTemperature_test);
  #endif
    }
#endif //INFOTM_SW_AWB_METHOD

    if (doAwb)
    {
#ifdef INFOTM_AWB_PATCH
	#if defined(INFOTM_AWB_PATCH_GET_CCT)
		measuredTemperature = colorTempCorrection.getCCT(
			1/(rgain/ Uncorrected.gains[0][0]), 1/ggain, 1/(bgain/Uncorrected.gains[0][3]));
	#else
        measuredTemperature = colorTempCorrection.getCorrelatedTemperature(
            1/(rgain/ Uncorrected.gains[0][0]), 1/ggain, 1/(bgain/Uncorrected.gains[0][3]));
	#endif

#ifdef INFOTM_OV_DEBUG
	rGain = rgain;
	bGain = bgain;
	Temperature = (int)measuredTemperature;
#endif //INFOTM_OV_DEBUG

#else	
	#if defined(INFOTM_AWB_PATCH_GET_CCT)
		measuredTemperature = colorTempCorrection.getCCT(
			1/rgain, 1/ggain, 1/bgain);
	#else
		measuredTemperature = colorTempCorrection.getCorrelatedTemperature(
			1/rgain, 1/ggain, 1/bgain);
	#endif
#endif //INFOTM_AWB_PATCH

        //printf("measuredTemperature = %f\n", measuredTemperature);
#if defined(INFOTM_ISP_AWB_DEBUG)
        {
            static IMG_UINT8 ui8Cnt = 0;

            if (10 <= ++ui8Cnt) {
            	ui8Cnt = 0;
                printf("measuredTemperature = %f, r_gain=%f, b_gain=%f\n",
                    measuredTemperature, rgain, bgain);
            }
        }
#endif

#if defined(INFOTM_SW_AWB_METHOD)
        if (this->bSwAwbEnable) {
            if (true == bUseOriginalCCM) {
                currentCCM = colorTempCorrection.getColorCorrection(measuredTemperature);
            } else {
                StdCCM.temperature = measuredTemperature;
                currentCCM = StdCCM;
            }
        } else {
            currentCCM = colorTempCorrection.getColorCorrection(measuredTemperature);
        }
#else
        currentCCM = colorTempCorrection.getColorCorrection(measuredTemperature);
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#ifdef INFOTM_ENABLE_COLOR_MODE_CHANGE
        {
            ControlAE *pAE = NULL;
            ControlCMC *pCMC = NULL;

            if (pCamera) {
                pAE = pCamera->getControlModule<ISPC::ControlAE>();
                pCMC = pCamera->getControlModule<ISPC::ControlCMC>();
            }
            if ((pCamera) && (pAE) && (pCMC)) {
                double Newgain = pAE->getNewGain();
                double dbRatio = 0.0;
                double dbDivisor = 1.0;

                if (pCMC->bCmcEnable) {
                    if (FlatModeStatusGet()) {
                        currentCCM.coefficients[0][0] = 1.0f;
                        currentCCM.coefficients[0][1] = 0.0f;
                        currentCCM.coefficients[0][2] = 0.0f;

                        currentCCM.coefficients[1][0] = 0.0f;
                        currentCCM.coefficients[1][1] = 1.0f;
                        currentCCM.coefficients[1][2] = 0.0f;

                        currentCCM.coefficients[2][0] = 0.0f;
                        currentCCM.coefficients[2][1] = 0.0f;
                        currentCCM.coefficients[2][2] = 1.0f;
                        currentCCM.gains[0][0] = 1.0f;
                        currentCCM.gains[0][1] = 1.0f;
                        currentCCM.gains[0][2] = 1.0f;
                        currentCCM.gains[0][3] = 1.0f;
                        currentCCM.offsets[0][0] = -128.0;
                        currentCCM.offsets[0][1] = -128.0;
                        currentCCM.offsets[0][2] = -128.0;
                    } else {
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
                    }
                }
            }
            if ((NULL == pCamera) ||
                (NULL == pCMC) ||
                ((pCMC) && (false == pCMC->bCmcEnable)) ||
                (false == FlatModeStatusGet())) {
  #ifdef INFOTM_AWB_PATCH
                currentCCM.gains[0][0] = rgain;
  #else
                currentCCM.gains[0][0] = rgain * Uncorrected.gains[0][0];
  #endif //INFOTM_AWB_PATCH
                currentCCM.gains[0][1] = ggain;
                currentCCM.gains[0][2] = ggain;
  #ifdef INFOTM_AWB_PATCH
                currentCCM.gains[0][3] = bgain;
  #else
                currentCCM.gains[0][3] = bgain * Uncorrected.gains[0][3];
  #endif //INFOTM_AWB_PATCH
            }
        }
#else
  #ifdef INFOTM_AWB_PATCH
        currentCCM.gains[0][0] = rgain;
  #else
        currentCCM.gains[0][0] = rgain * Uncorrected.gains[0][0];
  #endif //INFOTM_AWB_PATCH
        currentCCM.gains[0][1] = ggain;
        currentCCM.gains[0][2] = ggain;
  #ifdef INFOTM_AWB_PATCH
        currentCCM.gains[0][3] = bgain;
  #else
        currentCCM.gains[0][3] = bgain * Uncorrected.gains[0][3];
  #endif //INFOTM_AWB_PATCH
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

        dRGain = rgain;
        dBGain = bgain;
        programCorrection();
    }

    return IMG_SUCCESS;
}

void ISPC::ControlAWB_PID::setTargetTemperature(double targetTemperature)
{
#if VERBOSE_CONTROL_MODULES
    MOD_LOG_WARNING("ControlAWB-PID does not converge on target correction\n");
#endif
}

double ISPC::ControlAWB_PID::getCorrectionTemperature() const
{
    return measuredTemperature;
}

#ifdef INFOTM_ISP
double ISPC::ControlAWB_PID::getRGain(void)
{
    return dRGain;
}

double ISPC::ControlAWB_PID::getBGain(void)
{
    return dBGain;
}

double ISPC::ControlAWB_PID::getMargin(void)
{
    return margin;
}

void ISPC::ControlAWB_PID::setMargin(double dMargin)
{
    margin = dMargin;
}

double ISPC::ControlAWB_PID::getPidKP(void)
{
    return pid_KP;
}

void ISPC::ControlAWB_PID::setPidKP(double dPidKP)
{
    pid_KP = dPidKP;
}

double ISPC::ControlAWB_PID::getPidKD(void)
{
    return pid_KD;
}

void ISPC::ControlAWB_PID::setPidKD(double dPidKD)
{
    pid_KD = dPidKD;
}

double ISPC::ControlAWB_PID::getPidKI(void)
{
    return pid_KI;
}

void ISPC::ControlAWB_PID::setPidKI(double dPidKI)
{
    pid_KI = dPidKI;
}

#endif //INFOTM_ISP

#ifdef INFOTM_SW_AWB_METHOD
void ISPC::ControlAWB_PID::sw_awb_gain_set(double rgain, double bgain)
{
	dbSw_rgain = rgain;
	dbSw_bgain = bgain;
}
#endif //INFOTM_SW_AWB_METHOD

