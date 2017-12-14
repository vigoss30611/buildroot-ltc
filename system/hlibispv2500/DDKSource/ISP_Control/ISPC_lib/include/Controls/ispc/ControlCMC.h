/**
******************************************************************************
 @file ControlCMC.h

 @brief Control module for color mode change. Controls the color mode change parameters.

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

#ifndef CONTROL_CMC_H
#define CONTROL_CMC_H


#include "ispc/Module.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Parameter.h"


#include <cmath>
#include <string>
#include <math.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif


#define GMA_CNT 3
#define GMA_P_CNT 256


typedef enum _CMC_DAY_MODE {
    CMC_DAY_MODE_NIGHT = 0,
    CMC_DAY_MODE_DAY,
    CMC_DAY_MODE_MAX
} CMC_DAY_MODE, *PCMC_DAY_MODE;


typedef struct _CMC_NIGHT_MODE_DETECT_PARAM {
    double fCmcNightModeDetectBrightnessEnter;
    double fCmcNightModeDetectBrightnessExit;
    double fCmcNightModeDetectGainEnter;
    double fCmcNightModeDetectGainExit;
    double fCmcNightModeDetectWeighting;
} CMC_NIGHT_MODE_DETECT_PARAM, *PCMC_NIGHT_MODE_DETECT_PARAM;


#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) || defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)

namespace ISPC {

class Sensor; // "ispc/sensor.h"

class ControlCMC: public ControlModuleBase<CTRL_CMC>
{
public:
    bool bCmcEnable;
    bool bDnTargetIdxChageEnable;
    bool bCmcHasChange;
    bool bEnableCalcAvgGain;

    double fR2YSaturationExpect;

    bool bShaStrengthChange;
    double fShaStrengthExpect;
    double fShaStrengthOffset_acm;
    double fShaStrengthOffset_acm_lv1;
    double fShaStrengthOffset_acm_lv2;
    double fShaStrengthOffset_acm_lv3;
    double fShaStrengthOffset_fcm;
    double fShaStrengthOffset_fcm_lv1;
    double fShaStrengthOffset_fcm_lv2;
    double fShaStrengthOffset_fcm_lv3;

    bool bCmcNightModeDetectEnable;
    double fCmcNightModeDetectBrightnessEnter;
    double fCmcNightModeDetectBrightnessExit;
    double fCmcNightModeDetectGainEnter;
    double fCmcNightModeDetectGainExit;
    double fCmcNightModeDetectWeighting;

    bool bEnableCaptureIQ;
    double fCmcCaptureModeCCMRatio;
    double fCmcCaptureR2YRangeMul[3];
    double iCmcCaptureInY[2];
    double iCmcCaptureOutY[2];
    double iCmcCaptureOutC[2];

    //3D-Denoise target index.
    IMG_UINT32 ui32DnTargetIdx_fcm;
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)
///////////////////////////////////////Flat Color Mode setting
    //Level 0
    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_fcm[4];
    int ui32SystemBlack_fcm;

    //DNS
    double fDnsStrength_fcm;
    double fDnsReadNoise_fcm;
    int iDnsWellDepth_fcm;

    //SHA
    double fRadius_fcm;
    double fStrength_fcm;
    double fThreshold_fcm;
    double fDetail_fcm;
    double fEdgeScale_fcm;
    double fEdgeOffset_fcm;
    bool bBypassDenoise_fcm;
    double fDenoiseTau_fcm;
    double fDenoiseSigma_fcm;

    //R2Y
    double fBrightness_fcm;
    double fContrast_fcm;
    double fSaturation_fcm;
    double aRangeMult_fcm[3];

    // HIS
    int iGridStartCoords_fcm[2];
    int iGridTileDimensions_fcm[2];

    // DPF
    double fDpfWeight_fcm;
    int iDpfThreshold_fcm;

    //TNM
    double fHisMin_fcm;
    double fHisMax_fcm;
    bool bTnmcAdaptive_fcm;

    double iInY_fcm[2];
    double iOutY_fcm[2];
    double iOutC_fcm[2];
    double fFlatFactor_fcm;
    double fWeightLine_fcm;
    double fColourConfidence_fcm;
    double fColourSaturation_fcm;
    bool bEnableGamma_fcm;
    double fEqualBrightSupressRatio_fcm;
    double fEqualDarkSupressRatio_fcm;
    double fOvershotThreshold_fcm;
    double fWdrCeiling_fcm[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_fcm[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_fcm;
    double fTnmGamma_fcm;
    double fBezierCtrlPnt_fcm;

    //AE
    double targetBrightness_fcm;
    double fAeTargetMin_fcm;

    //Sensor Max Gain
    double fSensorMaxGain_fcm;
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) && defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    // For 3D-Denoise parameters in FLAT_COLOR_MODE gain level.
    //Level 1
    IMG_UINT32 ui32DnTargetIdx_fcm_lv1;

    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_fcm_lv1[4];
    int ui32SystemBlack_fcm_lv1;

    //DNS
    double fDnsStrength_fcm_lv1;
    double fDnsReadNoise_fcm_lv1;
    int iDnsWellDepth_fcm_lv1;

    // For SHA parameters in FLAT_COLOR_MODE gain level.
    //Level 1
    double fRadius_fcm_lv1;
    double fStrength_fcm_lv1;
    double fThreshold_fcm_lv1;
    double fDetail_fcm_lv1;
    double fEdgeScale_fcm_lv1;
    double fEdgeOffset_fcm_lv1;
    bool bBypassDenoise_fcm_lv1;
    double fDenoiseTau_fcm_lv1;
    double fDenoiseSigma_fcm_lv1;

    double fContrast_fcm_lv1;
    double fSaturation_fcm_lv1;
    double fBrightness_fcm_lv1;
    double aRangeMult_fcm_lv1[3];

    // HIS
    int iGridStartCoords_fcm_lv1[2];
    int iGridTileDimensions_fcm_lv1[2];

    // DPF
    double fDpfWeight_fcm_lv1;
    int iDpfThreshold_fcm_lv1;

    // For TNM parameters in FLAT_COLOR_MODE gain level.
    //Level 1
    double fHisMin_fcm_lv1;
    double fHisMax_fcm_lv1;
    bool bTnmcAdaptive_fcm_lv1;

    double iInY_fcm_lv1[2];
    double iOutY_fcm_lv1[2];
    double iOutC_fcm_lv1[2];
    double fFlatFactor_fcm_lv1;
    double fWeightLine_fcm_lv1;
    double fColourConfidence_fcm_lv1;
    double fColourSaturation_fcm_lv1;
    double fEqualBrightSupressRatio_fcm_lv1;
    double fEqualDarkSupressRatio_fcm_lv1;
    double fOvershotThreshold_fcm_lv1;
    double fWdrCeiling_fcm_lv1[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_fcm_lv1[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_fcm_lv1;
    double fTnmGamma_fcm_lv1;
    double fBezierCtrlPnt_fcm_lv1;

    // For 3D-Denoise parameters in FLAT_COLOR_MODE gain level.
    //Level 2
    IMG_UINT32 ui32DnTargetIdx_fcm_lv2;

    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_fcm_lv2[4];
    int ui32SystemBlack_fcm_lv2;

    //DNS
    double fDnsStrength_fcm_lv2;
    double fDnsReadNoise_fcm_lv2;
    int iDnsWellDepth_fcm_lv2;

    // For SHA parameters in FLAT_COLOR_MODE gain level.
    //Level 2
    double fRadius_fcm_lv2;
    double fStrength_fcm_lv2;
    double fThreshold_fcm_lv2;
    double fDetail_fcm_lv2;
    double fEdgeScale_fcm_lv2;
    double fEdgeOffset_fcm_lv2;
    bool bBypassDenoise_fcm_lv2;
    double fDenoiseTau_fcm_lv2;
    double fDenoiseSigma_fcm_lv2;

    double fContrast_fcm_lv2;
    double fSaturation_fcm_lv2;
    double fBrightness_fcm_lv2;
    double aRangeMult_fcm_lv2[3];

    // HIS
    int iGridStartCoords_fcm_lv2[2];
    int iGridTileDimensions_fcm_lv2[2];

    // DPF
    double fDpfWeight_fcm_lv2;
    int iDpfThreshold_fcm_lv2;

    // For TNM parameters in FLAT_COLOR_MODE gain level.
    //Level 2
    double fHisMin_fcm_lv2;
    double fHisMax_fcm_lv2;
    bool bTnmcAdaptive_fcm_lv2;

    double iInY_fcm_lv2[2];
    double iOutY_fcm_lv2[2];
    double iOutC_fcm_lv2[2];
    double fFlatFactor_fcm_lv2;
    double fWeightLine_fcm_lv2;
    double fColourConfidence_fcm_lv2;
    double fColourSaturation_fcm_lv2;
    double fEqualBrightSupressRatio_fcm_lv2;
    double fEqualDarkSupressRatio_fcm_lv2;
    double fOvershotThreshold_fcm_lv2;
    double fWdrCeiling_fcm_lv2[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_fcm_lv2[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_fcm_lv2;
    double fTnmGamma_fcm_lv2;
    double fBezierCtrlPnt_fcm_lv2;

    // For 3D-Denoise parameters in FLAT_COLOR_MODE gain level.
    //Level 3
    IMG_UINT32 ui32DnTargetIdx_fcm_lv3;

    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_fcm_lv3[4];
    int ui32SystemBlack_fcm_lv3;

    //DNS
    double fDnsStrength_fcm_lv3;
    double fDnsReadNoise_fcm_lv3;
    int iDnsWellDepth_fcm_lv3;

    //Level 3
    double fRadius_fcm_lv3;
    double fStrength_fcm_lv3;
    double fThreshold_fcm_lv3;
    double fDetail_fcm_lv3;
    double fEdgeScale_fcm_lv3;
    double fEdgeOffset_fcm_lv3;
    bool bBypassDenoise_fcm_lv3;
    double fDenoiseTau_fcm_lv3;
    double fDenoiseSigma_fcm_lv3;

    double fContrast_fcm_lv3;
    double fSaturation_fcm_lv3;
    double fBrightness_fcm_lv3;
    double aRangeMult_fcm_lv3[3];

    // HIS
    int iGridStartCoords_fcm_lv3[2];
    int iGridTileDimensions_fcm_lv3[2];

    // DPF
    double fDpfWeight_fcm_lv3;
    int iDpfThreshold_fcm_lv3;

    // For TNM parameters in FLAT_COLOR_MODE gain level.
    //Level 3
    double fHisMin_fcm_lv3;
    double fHisMax_fcm_lv3;
    bool bTnmcAdaptive_fcm_lv3;

    double iInY_fcm_lv3[2];
    double iOutY_fcm_lv3[2];
    double iOutC_fcm_lv3[2];
    double fFlatFactor_fcm_lv3;
    double fWeightLine_fcm_lv3;
    double fColourConfidence_fcm_lv3;
    double fColourSaturation_fcm_lv3;
    double fEqualBrightSupressRatio_fcm_lv3;
    double fEqualDarkSupressRatio_fcm_lv3;
    double fOvershotThreshold_fcm_lv3;
    double fWdrCeiling_fcm_lv3[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_fcm_lv3[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_fcm_lv3;
    double fTnmGamma_fcm_lv3;
    double fBezierCtrlPnt_fcm_lv3;
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE, INFOTM_ENABLE_GAIN_LEVEL_IDX

    //3D-Denoise target index.
    IMG_UINT32 ui32DnTargetIdx_acm;
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)
///////////////////////////////////////Advanced Color Mode setting
    //Level 0
    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_acm[4];
    int ui32SystemBlack_acm;

    //DNS
    double fDnsStrength_acm;
    double fDnsReadNoise_acm;
    int iDnsWellDepth_acm;

    //SHA
    double fRadius_acm;
    double fStrength_acm;
    double fThreshold_acm;
    double fDetail_acm;
    double fEdgeScale_acm;
    double fEdgeOffset_acm;
    bool bBypassDenoise_acm;
    double fDenoiseTau_acm;
    double fDenoiseSigma_acm;

    //R2Y
    double fBrightness_acm;
    double fContrast_acm;
    double fSaturation_acm;
    double aRangeMult_acm[3];

    // HIS
    int iGridStartCoords_acm[2];
    int iGridTileDimensions_acm[2];

    // DPF
    double fDpfWeight_acm;
    int iDpfThreshold_acm;

    //TNM
    double fHisMin_acm;
    double fHisMax_acm;
    bool bTnmcAdaptive_acm;

    double iInY_acm[2];
    double iOutY_acm[2];
    double iOutC_acm[2];
    double fFlatFactor_acm;
    double fWeightLine_acm;
    double fColourConfidence_acm;
    double fColourSaturation_acm;
    bool bEnableGamma_acm;
    double fEqualBrightSupressRatio_acm;
    double fEqualDarkSupressRatio_acm;
    double fOvershotThreshold_acm;
    double fWdrCeiling_acm[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_acm[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_acm;
    double fTnmGamma_acm;
    double fBezierCtrlPnt_acm;

    //AE
    double targetBrightness_acm;
    double fAeTargetMin_acm;

    //Sensor Max Gain
    double fSensorMaxGain_acm;
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

#if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    // Sensor Gain level.
    bool blSensorGainLevelCtrl;
    double fSensorGain_lv1;
    double fSensorGain_lv2;
    // Sensor Gain Interpolation
    double fSensorGainLv1Interpolation;
    double fSensorGainLv2Interpolation;
    // Sensor Gain Interpolation using Gamma method.
    bool bEnableInterpolationGamma;
    double fCmcInterpolationGamma[3];
    IMG_FLOAT afGammaLUT[GMA_CNT][GMA_P_CNT];

    // CCM attenuation level.
    double dbCcmAttenuation_lv1;
    double dbCcmAttenuation_lv2;
    double dbCcmAttenuation_lv3;

    // For 3D-Denoise parameters in ADVANCED_COLOR_MODE gain level.
    //Level 1
    IMG_UINT32 ui32DnTargetIdx_acm_lv1;

    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_acm_lv1[4];
    int ui32SystemBlack_acm_lv1;

    //DNS
    double fDnsStrength_acm_lv1;
    double fDnsReadNoise_acm_lv1;
    int iDnsWellDepth_acm_lv1;

    // For SHA parameters in ADVANCED_COLOR_MODE gain level.
    //Level 1
    double fRadius_acm_lv1;
    double fStrength_acm_lv1;
    double fThreshold_acm_lv1;
    double fDetail_acm_lv1;
    double fEdgeScale_acm_lv1;
    double fEdgeOffset_acm_lv1;
    bool bBypassDenoise_acm_lv1;
    double fDenoiseTau_acm_lv1;
    double fDenoiseSigma_acm_lv1;

    double fContrast_acm_lv1;
    double fSaturation_acm_lv1;
    double fBrightness_acm_lv1;
    double aRangeMult_acm_lv1[3];

    // HIS
    int iGridStartCoords_acm_lv1[2];
    int iGridTileDimensions_acm_lv1[2];

    // DPF
    double fDpfWeight_acm_lv1;
    int iDpfThreshold_acm_lv1;

    // For TNM parameters in ADVANCED_COLOR_MODE gain level.
    //Level 1
    double fHisMin_acm_lv1;
    double fHisMax_acm_lv1;
    bool bTnmcAdaptive_acm_lv1;

    double iInY_acm_lv1[2];
    double iOutY_acm_lv1[2];
    double iOutC_acm_lv1[2];
    double fFlatFactor_acm_lv1;
    double fWeightLine_acm_lv1;
    double fColourConfidence_acm_lv1;
    double fColourSaturation_acm_lv1;
    double fEqualBrightSupressRatio_acm_lv1;
    double fEqualDarkSupressRatio_acm_lv1;
    double fOvershotThreshold_acm_lv1;
    double fWdrCeiling_acm_lv1[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_acm_lv1[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_acm_lv1;
    double fTnmGamma_acm_lv1;
    double fBezierCtrlPnt_acm_lv1;

    // For 3D-Denoise parameters in ADVANCED_COLOR_MODE gain level.
    //Level 2
    IMG_UINT32 ui32DnTargetIdx_acm_lv2;

    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_acm_lv2[4];
    int ui32SystemBlack_acm_lv2;

    //DNS
    double fDnsStrength_acm_lv2;
    double fDnsReadNoise_acm_lv2;
    int iDnsWellDepth_acm_lv2;

    //Level 2
    double fRadius_acm_lv2;
    double fStrength_acm_lv2;
    double fThreshold_acm_lv2;
    double fDetail_acm_lv2;
    double fEdgeScale_acm_lv2;
    double fEdgeOffset_acm_lv2;
    bool bBypassDenoise_acm_lv2;
    double fDenoiseTau_acm_lv2;
    double fDenoiseSigma_acm_lv2;

    double fContrast_acm_lv2;
    double fSaturation_acm_lv2;
    double fBrightness_acm_lv2;
    double aRangeMult_acm_lv2[3];

    // HIS
    int iGridStartCoords_acm_lv2[2];
    int iGridTileDimensions_acm_lv2[2];

    // DPF
    double fDpfWeight_acm_lv2;
    int iDpfThreshold_acm_lv2;

    // For TNM parameters in ADVANCED_COLOR_MODE gain level.
    //Level 2
    double fHisMin_acm_lv2;
    double fHisMax_acm_lv2;
    bool bTnmcAdaptive_acm_lv2;

    double iInY_acm_lv2[2];
    double iOutY_acm_lv2[2];
    double iOutC_acm_lv2[2];
    double fFlatFactor_acm_lv2;
    double fWeightLine_acm_lv2;
    double fColourConfidence_acm_lv2;
    double fColourSaturation_acm_lv2;
    double fEqualBrightSupressRatio_acm_lv2;
    double fEqualDarkSupressRatio_acm_lv2;
    double fOvershotThreshold_acm_lv2;
    double fWdrCeiling_acm_lv2[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_acm_lv2[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_acm_lv2;
    double fTnmGamma_acm_lv2;
    double fBezierCtrlPnt_acm_lv2;

    // For 3D-Denoise parameters in ADVANCED_COLOR_MODE gain level.
    //Level 3
    IMG_UINT32 ui32DnTargetIdx_acm_lv3;

    //BLC SENSOR BLACK
    int ui32BlcSensorBlack_acm_lv3[4];
    int ui32SystemBlack_acm_lv3;

    //DNS
    double fDnsStrength_acm_lv3;
    double fDnsReadNoise_acm_lv3;
    int iDnsWellDepth_acm_lv3;

    //Level 3
    double fRadius_acm_lv3;
    double fStrength_acm_lv3;
    double fThreshold_acm_lv3;
    double fDetail_acm_lv3;
    double fEdgeScale_acm_lv3;
    double fEdgeOffset_acm_lv3;
    bool bBypassDenoise_acm_lv3;
    double fDenoiseTau_acm_lv3;
    double fDenoiseSigma_acm_lv3;

    double fContrast_acm_lv3;
    double fSaturation_acm_lv3;
    double fBrightness_acm_lv3;
    double aRangeMult_acm_lv3[3];

    // HIS
    int iGridStartCoords_acm_lv3[2];
    int iGridTileDimensions_acm_lv3[2];

    // DPF
    double fDpfWeight_acm_lv3;
    int iDpfThreshold_acm_lv3;

    // For TNM parameters in ADVANCED_COLOR_MODE gain level.
    //Level 3
    double fHisMin_acm_lv3;
    double fHisMax_acm_lv3;
    bool bTnmcAdaptive_acm_lv3;

    double iInY_acm_lv3[2];
    double iOutY_acm_lv3[2];
    double iOutC_acm_lv3[2];
    double fFlatFactor_acm_lv3;
    double fWeightLine_acm_lv3;
    double fColourConfidence_acm_lv3;
    double fColourSaturation_acm_lv3;
    double fEqualBrightSupressRatio_acm_lv3;
    double fEqualDarkSupressRatio_acm_lv3;
    double fOvershotThreshold_acm_lv3;
    double fWdrCeiling_acm_lv3[TNMC_WDR_SEGMENT_CNT];
    double fWdrFloor_acm_lv3[TNMC_WDR_SEGMENT_CNT];
    int ui32GammaCrvMode_acm_lv3;
    double fTnmGamma_acm_lv3;
    double fBezierCtrlPnt_acm_lv3;
#endif //INFOTM_ENABLE_GAIN_LEVEL_IDX

protected:

public:
    /** @brief Constructor setting default values */
    ControlCMC(const std::string &logtag = "ISPC_CTRL_CMC");

    /** @brief Virtual destructor */
    virtual ~ControlCMC() {}

    IMG_RESULT BuildGammaCurve(double* fPrecompensation, IMG_INT cnt);

    /** @copydoc ControlModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ControlModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ControlModule::update() */
#ifdef INFOTM_ISP
    virtual IMG_RESULT update(const Metadata &metadata, const Metadata &metadata2);
#else
    virtual IMG_RESULT update(const Metadata &metadata);
#endif //INFOTM_DUAL_SENSOR

    IMG_RESULT GetR2YSaturationExpect(double &fSaturation);
    IMG_RESULT SetR2YSaturationExpect(double fSaturation);
    double CalcR2YSaturation(double fCmcSaturation);

    IMG_RESULT GetShaStrengthExpect(double &fStrength);
    IMG_RESULT SetShaStrengthExpect(double fStrength);
    IMG_RESULT CalcShaStrength(void);

    IMG_RESULT CmcNightModeDetectEnableGet(bool &bEnable);
    IMG_RESULT CmcNightModeDetectEnableSet(bool bEnable);
    IMG_RESULT CmcNightModeDetectEnableGet(CMC_NIGHT_MODE_DETECT_PARAM &Param);
    IMG_RESULT CmcNightModeDetectEnableSet(CMC_NIGHT_MODE_DETECT_PARAM Param);

protected:
    /**
     * @copydoc ControlModule::configureStatistics()
     *
     * Does nothing, module does not use statistics
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * Does nothing, module does not use program correction
     */
    virtual IMG_RESULT programCorrection();

public:  // parameters
    static const ParamDefSingle<bool> CMC_ENABLE;
    static const ParamDefSingle<bool> CMC_DN_TARGET_IDX_CHANGE_ENABLE;
    static const ParamDefSingle<bool> CMC_ENABLE_AVG_GAIN;

    static const ParamDef<double> CMC_R2Y_SATURATION_EXPECT;

    static const ParamDef<double> CMC_SHA_STRENGTH_EXPECT;

    static const ParamDefSingle<bool> CMC_NIGHT_MODE_DETECT_ENABLE;
    static const ParamDef<double> CMC_NIGHT_MODE_DETECT_BRIGHTNESS_ENTER;
    static const ParamDef<double> CMC_NIGHT_MODE_DETECT_BRIGHTNESS_EXIT;
    static const ParamDef<double> CMC_NIGHT_MODE_DETECT_GAIN_ENTER;
    static const ParamDef<double> CMC_NIGHT_MODE_DETECT_GAIN_EXIT;
    static const ParamDef<double> CMC_NIGHT_MODE_DETECT_WEIGHTING;

    static const ParamDefSingle<bool> CMC_ENABLE_CAPTURE_IQ;
    static const ParamDef<double> CMC_CAPTURE_CCM_RATIO;
    static const ParamDefArray<double> CMC_R2Y_CAPTURE_RANGE_MUL;
    static const ParamDefArray<double> CMC_TNM_CAPTURE_IN_Y;
    static const ParamDefArray<double> CMC_TNM_CAPTURE_OUT_Y;
    static const ParamDefArray<double> CMC_TNM_CAPTURE_OUT_C;

    //3D-Denoise target index.
    static const ParamDef<int> CMC_DN_TARGET_IDX_FCM;
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)
///////////////////////////////////////Flat Color Mode setting
    //BLC SENSOR BLACK
    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_FCM;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_FCM;

    //DNS
    static const ParamDef<double> CMC_DNS_STRENGTH_FCM;
    static const ParamDef<double> CMC_DNS_READ_NOISE_FCM;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_FCM;

    //SHA
    static const ParamDef<double> CMC_SHA_RADIUS_FCM;
    static const ParamDef<double> CMC_SHA_STRENGTH_FCM;
    static const ParamDef<double> CMC_SHA_THRESH_FCM;
    static const ParamDef<double> CMC_SHA_DETAIL_FCM;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_FCM;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_FCM;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_FCM;
    static const ParamDef<double> CMC_SHADN_TAU_FCM;
    static const ParamDef<double> CMC_SHADN_SIGMA_FCM;

    //R2Y
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_FCM;
    static const ParamDef<double> CMC_R2Y_CONTRAST_FCM;
    static const ParamDef<double> CMC_R2Y_SATURATION_FCM;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_FCM;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_FCM;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_FCM;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_FCM;
    static const ParamDef<int> CMC_DPF_THRESHOLD_FCM;

    //TNM
    static const ParamDef<double> CMC_TNMC_HISTMIN_FCM;
    static const ParamDef<double> CMC_TNMC_HISTMAX_FCM;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_FCM;

    static const ParamDefArray<double> CMC_TNM_IN_Y_FCM;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_FCM;
    static const ParamDefArray<double> CMC_TNM_OUT_C_FCM;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_FCM;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_FCM;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_FCM;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_FCM;
    static const ParamDefSingle<bool> CMC_TNMC_ENABLE_GAMMA_FCM;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_FCM;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_FCM;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_FCM;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_FCM;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_FCM;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_FCM;
    static const ParamDef<double> CMC_TNMC_GAMMA_FCM;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_FCM;

    //AE
    static const ParamDef<double> CMC_AE_TARGET_BRIGHTNESS_FCM;
    static const ParamDef<double> CMC_AE_TARGET_MIN_FCM;

    //Sensor Max Gain
    static const ParamDef<double> CMC_SENSOR_MAX_GAIN_FCM;
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE) && defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    static const ParamDef<int> CMC_DN_TARGET_IDX_FCM_LV1;

    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_FCM_LV1;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_FCM_LV1;

    static const ParamDef<double> CMC_DNS_STRENGTH_FCM_LV1;
    static const ParamDef<double> CMC_DNS_READ_NOISE_FCM_LV1;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_FCM_LV1;

    static const ParamDef<double> CMC_SHA_RADIUS_FCM_LV1;
    static const ParamDef<double> CMC_SHA_STRENGTH_FCM_LV1;
    static const ParamDef<double> CMC_SHA_THRESH_FCM_LV1;
    static const ParamDef<double> CMC_SHA_DETAIL_FCM_LV1;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_FCM_LV1;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_FCM_LV1;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_FCM_LV1;
    static const ParamDef<double> CMC_SHADN_TAU_FCM_LV1;
    static const ParamDef<double> CMC_SHADN_SIGMA_FCM_LV1;

    static const ParamDef<double> CMC_R2Y_CONTRAST_FCM_LV1;
    static const ParamDef<double> CMC_R2Y_SATURATION_FCM_LV1;
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_FCM_LV1;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_FCM_LV1;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_FCM_LV1;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_FCM_LV1;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_FCM_LV1;
    static const ParamDef<int> CMC_DPF_THRESHOLD_FCM_LV1;

    static const ParamDef<double> CMC_TNMC_HISTMIN_FCM_LV1;
    static const ParamDef<double> CMC_TNMC_HISTMAX_FCM_LV1;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_FCM_LV1;

    static const ParamDefArray<double> CMC_TNM_IN_Y_FCM_LV1;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_FCM_LV1;
    static const ParamDefArray<double> CMC_TNM_OUT_C_FCM_LV1;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_FCM_LV1;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_FCM_LV1;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_FCM_LV1;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_FCM_LV1;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_FCM_LV1;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_FCM_LV1;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_FCM_LV1;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_FCM_LV1;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_FCM_LV1;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_FCM_LV1;
    static const ParamDef<double> CMC_TNMC_GAMMA_FCM_LV1;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_FCM_LV1;

    static const ParamDef<int> CMC_DN_TARGET_IDX_FCM_LV2;

    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_FCM_LV2;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_FCM_LV2;

    static const ParamDef<double> CMC_DNS_STRENGTH_FCM_LV2;
    static const ParamDef<double> CMC_DNS_READ_NOISE_FCM_LV2;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_FCM_LV2;

    static const ParamDef<double> CMC_SHA_RADIUS_FCM_LV2;
    static const ParamDef<double> CMC_SHA_STRENGTH_FCM_LV2;
    static const ParamDef<double> CMC_SHA_THRESH_FCM_LV2;
    static const ParamDef<double> CMC_SHA_DETAIL_FCM_LV2;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_FCM_LV2;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_FCM_LV2;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_FCM_LV2;
    static const ParamDef<double> CMC_SHADN_TAU_FCM_LV2;
    static const ParamDef<double> CMC_SHADN_SIGMA_FCM_LV2;

    static const ParamDef<double> CMC_R2Y_CONTRAST_FCM_LV2;
    static const ParamDef<double> CMC_R2Y_SATURATION_FCM_LV2;
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_FCM_LV2;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_FCM_LV2;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_FCM_LV2;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_FCM_LV2;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_FCM_LV2;
    static const ParamDef<int> CMC_DPF_THRESHOLD_FCM_LV2;

    static const ParamDef<double> CMC_TNMC_HISTMIN_FCM_LV2;
    static const ParamDef<double> CMC_TNMC_HISTMAX_FCM_LV2;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_FCM_LV2;

    static const ParamDefArray<double> CMC_TNM_IN_Y_FCM_LV2;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_FCM_LV2;
    static const ParamDefArray<double> CMC_TNM_OUT_C_FCM_LV2;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_FCM_LV2;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_FCM_LV2;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_FCM_LV2;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_FCM_LV2;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_FCM_LV2;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_FCM_LV2;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_FCM_LV2;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_FCM_LV2;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_FCM_LV2;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_FCM_LV2;
    static const ParamDef<double> CMC_TNMC_GAMMA_FCM_LV2;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_FCM_LV2;

    static const ParamDef<int> CMC_DN_TARGET_IDX_FCM_LV3;

    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_FCM_LV3;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_FCM_LV3;

    static const ParamDef<double> CMC_DNS_STRENGTH_FCM_LV3;
    static const ParamDef<double> CMC_DNS_READ_NOISE_FCM_LV3;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_FCM_LV3;

    static const ParamDef<double> CMC_SHA_RADIUS_FCM_LV3;
    static const ParamDef<double> CMC_SHA_STRENGTH_FCM_LV3;
    static const ParamDef<double> CMC_SHA_THRESH_FCM_LV3;
    static const ParamDef<double> CMC_SHA_DETAIL_FCM_LV3;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_FCM_LV3;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_FCM_LV3;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_FCM_LV3;
    static const ParamDef<double> CMC_SHADN_TAU_FCM_LV3;
    static const ParamDef<double> CMC_SHADN_SIGMA_FCM_LV3;

    static const ParamDef<double> CMC_R2Y_CONTRAST_FCM_LV3;
    static const ParamDef<double> CMC_R2Y_SATURATION_FCM_LV3;
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_FCM_LV3;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_FCM_LV3;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_FCM_LV3;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_FCM_LV3;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_FCM_LV3;
    static const ParamDef<int> CMC_DPF_THRESHOLD_FCM_LV3;

    static const ParamDef<double> CMC_TNMC_HISTMIN_FCM_LV3;
    static const ParamDef<double> CMC_TNMC_HISTMAX_FCM_LV3;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_FCM_LV3;

    static const ParamDefArray<double> CMC_TNM_IN_Y_FCM_LV3;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_FCM_LV3;
    static const ParamDefArray<double> CMC_TNM_OUT_C_FCM_LV3;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_FCM_LV3;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_FCM_LV3;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_FCM_LV3;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_FCM_LV3;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_FCM_LV3;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_FCM_LV3;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_FCM_LV3;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_FCM_LV3;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_FCM_LV3;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_FCM_LV3;
    static const ParamDef<double> CMC_TNMC_GAMMA_FCM_LV3;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_FCM_LV3;
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE, INFOTM_ENABLE_GAIN_LEVEL_IDX

    //3D-Denoise target index.
    static const ParamDef<int> CMC_DN_TARGET_IDX_ACM;
#if defined(INFOTM_ENABLE_COLOR_MODE_CHANGE)
///////////////////////////////////////Advanced Color Mode setting
    //BLC SENSOR BLACK
    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_ACM;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_ACM;

    //DNS
    static const ParamDef<double> CMC_DNS_STRENGTH_ACM;
    static const ParamDef<double> CMC_DNS_READ_NOISE_ACM;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_ACM;

    //SHA
    static const ParamDef<double> CMC_SHA_RADIUS_ACM;
    static const ParamDef<double> CMC_SHA_STRENGTH_ACM;
    static const ParamDef<double> CMC_SHA_THRESH_ACM;
    static const ParamDef<double> CMC_SHA_DETAIL_ACM;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_ACM;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_ACM;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_ACM;
    static const ParamDef<double> CMC_SHADN_TAU_ACM;
    static const ParamDef<double> CMC_SHADN_SIGMA_ACM;

    //R2Y
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_ACM;
    static const ParamDef<double> CMC_R2Y_CONTRAST_ACM;
    static const ParamDef<double> CMC_R2Y_SATURATION_ACM;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_ACM;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_ACM;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_ACM;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_ACM;
    static const ParamDef<int> CMC_DPF_THRESHOLD_ACM;

    //TNM
    static const ParamDef<double> CMC_TNMC_HISTMIN_ACM;
    static const ParamDef<double> CMC_TNMC_HISTMAX_ACM;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_ACM;  
    
    static const ParamDefArray<double> CMC_TNM_IN_Y_ACM;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_ACM;
    static const ParamDefArray<double> CMC_TNM_OUT_C_ACM;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_ACM;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_ACM;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_ACM;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_ACM;
    static const ParamDefSingle<bool> CMC_TNMC_ENABLE_GAMMA_ACM;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_ACM;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_ACM;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_ACM;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_ACM;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_ACM;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_ACM;
    static const ParamDef<double> CMC_TNMC_GAMMA_ACM;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_ACM;

    //AE
    static const ParamDef<double> CMC_AE_TARGET_BRIGHTNESS_ACM;
    static const ParamDef<double> CMC_AE_TARGET_MIN_ACM;

    //Sensor Max Gain
    static const ParamDef<double> CMC_SENSOR_MAX_GAIN_ACM;
#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE

#if defined(INFOTM_ENABLE_GAIN_LEVEL_IDX)
    static const ParamDefSingle<bool> CMC_SENSOR_GAIN_LEVEL_CTRL;
    static const ParamDef<double> CMC_SENSOR_GAIN_LV1;
    static const ParamDef<double> CMC_SENSOR_GAIN_LV2;
    static const ParamDef<double> CMC_SENSOR_GAIN_LV1_INTERPOLATION;
    static const ParamDef<double> CMC_SENSOR_GAIN_LV2_INTERPOLATION;
    static const ParamDefSingle<bool> CMC_ENABLE_INTERPOLATION_GAMMA;
    static const ParamDef<double> CMC_INTERPOLATION_GAMMA_0;
    static const ParamDef<double> CMC_INTERPOLATION_GAMMA_1;
    static const ParamDef<double> CMC_INTERPOLATION_GAMMA_2;

    static const ParamDef<double> CMC_CCM_ATTENUATION_LV1;
    static const ParamDef<double> CMC_CCM_ATTENUATION_LV2;
    static const ParamDef<double> CMC_CCM_ATTENUATION_LV3;

    static const ParamDef<int> CMC_DN_TARGET_IDX_ACM_LV1;

    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_ACM_LV1;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_ACM_LV1;

    static const ParamDef<double> CMC_DNS_STRENGTH_ACM_LV1;
    static const ParamDef<double> CMC_DNS_READ_NOISE_ACM_LV1;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_ACM_LV1;

    static const ParamDef<double> CMC_SHA_RADIUS_ACM_LV1;
    static const ParamDef<double> CMC_SHA_STRENGTH_ACM_LV1;
    static const ParamDef<double> CMC_SHA_THRESH_ACM_LV1;
    static const ParamDef<double> CMC_SHA_DETAIL_ACM_LV1;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_ACM_LV1;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_ACM_LV1;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_ACM_LV1;
    static const ParamDef<double> CMC_SHADN_TAU_ACM_LV1;
    static const ParamDef<double> CMC_SHADN_SIGMA_ACM_LV1;

    static const ParamDef<double> CMC_R2Y_CONTRAST_ACM_LV1;
    static const ParamDef<double> CMC_R2Y_SATURATION_ACM_LV1;
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_ACM_LV1;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_ACM_LV1;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_ACM_LV1;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_ACM_LV1;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_ACM_LV1;
    static const ParamDef<int> CMC_DPF_THRESHOLD_ACM_LV1;

    static const ParamDef<double> CMC_TNMC_HISTMIN_ACM_LV1;
    static const ParamDef<double> CMC_TNMC_HISTMAX_ACM_LV1;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_ACM_LV1;

    static const ParamDefArray<double> CMC_TNM_IN_Y_ACM_LV1;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_ACM_LV1;
    static const ParamDefArray<double> CMC_TNM_OUT_C_ACM_LV1;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_ACM_LV1;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_ACM_LV1;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_ACM_LV1;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_ACM_LV1;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_ACM_LV1;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_ACM_LV1;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_ACM_LV1;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_ACM_LV1;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_ACM_LV1;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_ACM_LV1;
    static const ParamDef<double> CMC_TNMC_GAMMA_ACM_LV1;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_ACM_LV1;

    static const ParamDef<int> CMC_DN_TARGET_IDX_ACM_LV2;

    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_ACM_LV2;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_ACM_LV2;

    static const ParamDef<double> CMC_DNS_STRENGTH_ACM_LV2;
    static const ParamDef<double> CMC_DNS_READ_NOISE_ACM_LV2;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_ACM_LV2;

    static const ParamDef<double> CMC_SHA_RADIUS_ACM_LV2;
    static const ParamDef<double> CMC_SHA_STRENGTH_ACM_LV2;
    static const ParamDef<double> CMC_SHA_THRESH_ACM_LV2;
    static const ParamDef<double> CMC_SHA_DETAIL_ACM_LV2;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_ACM_LV2;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_ACM_LV2;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_ACM_LV2;
    static const ParamDef<double> CMC_SHADN_TAU_ACM_LV2;
    static const ParamDef<double> CMC_SHADN_SIGMA_ACM_LV2;

    static const ParamDef<double> CMC_R2Y_CONTRAST_ACM_LV2;
    static const ParamDef<double> CMC_R2Y_SATURATION_ACM_LV2;
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_ACM_LV2;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_ACM_LV2;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_ACM_LV2;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_ACM_LV2;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_ACM_LV2;
    static const ParamDef<int> CMC_DPF_THRESHOLD_ACM_LV2;

    static const ParamDef<double> CMC_TNMC_HISTMIN_ACM_LV2;
    static const ParamDef<double> CMC_TNMC_HISTMAX_ACM_LV2;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_ACM_LV2;

    static const ParamDefArray<double> CMC_TNM_IN_Y_ACM_LV2;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_ACM_LV2;
    static const ParamDefArray<double> CMC_TNM_OUT_C_ACM_LV2;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_ACM_LV2;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_ACM_LV2;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_ACM_LV2;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_ACM_LV2;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_ACM_LV2;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_ACM_LV2;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_ACM_LV2;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_ACM_LV2;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_ACM_LV2;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_ACM_LV2;
    static const ParamDef<double> CMC_TNMC_GAMMA_ACM_LV2;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_ACM_LV2;

    static const ParamDef<int> CMC_DN_TARGET_IDX_ACM_LV3;

    static const ParamDefArray<int> CMC_BLC_SENSOR_BLACK_ACM_LV3;
    static const ParamDef<int> CMC_BLC_SYS_BLACK_ACM_LV3;

    static const ParamDef<double> CMC_DNS_STRENGTH_ACM_LV3;
    static const ParamDef<double> CMC_DNS_READ_NOISE_ACM_LV3;
    static const ParamDef<int> CMC_DNS_WELLDEPTH_ACM_LV3;

    static const ParamDef<double> CMC_SHA_RADIUS_ACM_LV3;
    static const ParamDef<double> CMC_SHA_STRENGTH_ACM_LV3;
    static const ParamDef<double> CMC_SHA_THRESH_ACM_LV3;
    static const ParamDef<double> CMC_SHA_DETAIL_ACM_LV3;
    static const ParamDef<double> CMC_SHA_EDGE_SCALE_ACM_LV3;
    static const ParamDef<double> CMC_SHA_EDGE_OFFSET_ACM_LV3;
    static const ParamDefSingle<bool> CMC_SHA_DENOISE_BYPASS_ACM_LV3;
    static const ParamDef<double> CMC_SHADN_TAU_ACM_LV3;
    static const ParamDef<double> CMC_SHADN_SIGMA_ACM_LV3;

    static const ParamDef<double> CMC_R2Y_CONTRAST_ACM_LV3;
    static const ParamDef<double> CMC_R2Y_SATURATION_ACM_LV3;
    static const ParamDef<double> CMC_R2Y_BRIGHTNESS_ACM_LV3;
    static const ParamDefArray<double> CMC_R2Y_RANGE_MUL_ACM_LV3;

    // HIS
    static const ParamDefArray<int> CMC_HIS_GRID_START_COORDS_ACM_LV3;
    static const ParamDefArray<int> CMC_HIS_GRID_TILE_DIMENSIONS_ACM_LV3;

    // DPF
    static const ParamDef<double> CMC_DPF_WEIGHT_ACM_LV3;
    static const ParamDef<int> CMC_DPF_THRESHOLD_ACM_LV3;

    static const ParamDef<double> CMC_TNMC_HISTMIN_ACM_LV3;
    static const ParamDef<double> CMC_TNMC_HISTMAX_ACM_LV3;
    static const ParamDefSingle<bool> CMC_TNMC_ADAPTIVE_ACM_LV3;

    static const ParamDefArray<double> CMC_TNM_IN_Y_ACM_LV3;
    static const ParamDefArray<double> CMC_TNM_OUT_Y_ACM_LV3;
    static const ParamDefArray<double> CMC_TNM_OUT_C_ACM_LV3;
    static const ParamDef<double> CMC_TNM_FLAT_FACTOR_ACM_LV3;
    static const ParamDef<double> CMC_TNM_WEIGHT_LINE_ACM_LV3;
    static const ParamDef<double> CMC_TNM_COLOUR_CONFIDENCE_ACM_LV3;
    static const ParamDef<double> CMC_TNM_COLOUR_SATURATION_ACM_LV3;
    static const ParamDef<double> CMC_TNMC_EQUAL_BRIGHTSUPRESSRATIO_ACM_LV3;
    static const ParamDef<double> CMC_TNMC_EQUAL_DARKSUPRESSRATIO_ACM_LV3;
    static const ParamDef<double> CMC_TNMC_OVERSHOT_THRESHOLD_ACM_LV3;
    static const ParamDefArray<double> CMC_TNMC_WDR_CEILING_ACM_LV3;
    static const ParamDefArray<double> CMC_TNMC_WDR_FLOOR_ACM_LV3;
    static const ParamDef<int> CMC_TNMC_GAMMA_CRV_MODE_ACM_LV3;
    static const ParamDef<double> CMC_TNMC_GAMMA_ACM_LV3;
    static const ParamDef<double> CMC_TNMC_BEZIER_CTRL_PNT_ACM_LV3;
#endif //INFOTM_ENABLE_GAIN_LEVEL_IDX

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif //INFOTM_ENABLE_COLOR_MODE_CHANGE, INFOTM_ENABLE_GAIN_LEVEL_IDX


#endif //ISPC_CONTROL_CMC_H_
