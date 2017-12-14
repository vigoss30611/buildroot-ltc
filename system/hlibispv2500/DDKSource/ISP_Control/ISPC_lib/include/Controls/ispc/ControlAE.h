/**
*******************************************************************************
 @file ControlAE.h

 @brief Control module for automatic exposure. Controls the sensor exposure
 and gain as well as the HIS statistics configuration.

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
#ifndef ISPC_CONTROL_AE_H_
#define ISPC_CONTROL_AE_H_

#include <string>

#include "ispc/Module.h"
#include "ispc/Parameter.h"
#include <pthread.h>

namespace ISPC {

class Sensor;  // declared in ispc/sensor.h

/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief ControlAE class implements the automatic exposure.
 *
 * Uses the ModuleHIS statistics to modify the Sensor exposure and gain.
 */
class ControlAE: public ControlModuleBase<CTRL_AE>
{
public:

#ifdef INFOTM_ISP
    typedef enum _AEE_EXPOSURE_METHOD
    {
        AEMHD_EXP_DEFAULT = 0,
        AEMHD_EXP_LOWLUX
    } AEE_EXPOSURE_METHOD;
#endif

protected:  // attributes
    /** @brief Metered brightness information */
    double previousBrightness;
    /** @brief Metered brightness information*/
    double currentBrightness;
    /** @brief desired brightness */
    double targetBrightness;
    /**
     * @brief Update speed for the exposure. Increase if AE works slowly,
     * reduce if AE doesn't converge (oscillates).
     */
    double updateSpeed;
    /** @brief enable/disable flicker rejection */
    bool flickerRejection;
    /** @brief if true then flickerFreq is read from flicker statistics */
    bool autoFlickerRejection;
    /**
     * @brief Frequency to reject - may be acquired from the flicker
     * detection module
     */
    double flickerFreq;
    /**
     * @brief Margin around the target brightness which we will consider
     * as a proper exposure (no changes in the exposure settings will
     * be carried out)
     */
    double targetBracket;

    /**
     * @brief Set to true in update() if both desired exposure and gain
     * are higher than values supported by sensor
     */
    bool underexposed;
    bool configured;

    /**
     * @brief Used to transfert exposure information from update() to
     * programCorrection()
     */
    double fNewGain;
    /**
     * @brief Used to transfert exposure information from update() to
     * programCorrection()
     */
    unsigned int uiNewExposure;
    bool doAE;

    /** @brief Grid weights with higher relevance towards the centre */
    static const double WEIGHT_7X7[7][7];
    /**
     * @brief Grid weights with higher relevance towards the centre but more
     * homogeneously distributed
     */
    static const double WEIGHT_7X7_A[7][7];
    /** @brief Grid weights with much more focused in the centra values */
    static const double WEIGHT_7X7_B[7][7];
    /** @brief Defines a background/foregrund mask for backlight detection */
    static const double BACKLIGHT_7X7[7][7];

    /** @brief temp storage used in getBrightnessMetering() */
    double regionBrightness[HIS_REGION_VTILES][HIS_REGION_HTILES];
    double regionOverExp[HIS_REGION_VTILES][HIS_REGION_HTILES];
    double regionUnderExp[HIS_REGION_VTILES][HIS_REGION_HTILES];
    double regionRatio[HIS_REGION_VTILES][HIS_REGION_HTILES];

#ifdef INFOTM_ISP
    bool settingsUpdated;
	double OverUnderExpDiff;
	double Ratio_OverExp;
	double Ratio_MidOverExp;
	double Ratio_UnderExp;
	unsigned int uiAeMode;
	unsigned int uiAeModeNew;
	double OritargetBrightness;
	bool enMoveAETarget;
	int ui32AeMoveMethod;
	double fAeTargetMax;
	double fAeTargetMin;
	double fAeTargetGainNormalModeMaxLmt;
	double fAeTargetMoveStep;
	double fAeTargetMinLmt;
	double fAeTargetUpOverThreshold;
	double fAeTargetUpUnderThreshold;
	double fAeTargetDnOverThreshold;
	double fAeTargetDnUnderThreshold;

	int ui32AeExposureMethod;
	double fAeTargetLowluxGainEnter;
	double fAeTargetLowluxGainExit;
	int ui32AeTargetLowluxExposureEnter;
	double fAeTargetLowluxFPS;
	double fAeTargetNormalFPS;
    bool bAeTargetMaxFpsLockEnable;

	int ui32AeBrightnessMeteringMethod;
	double fAeRegionDuce;
    bool bAeForceCallSensorDriverEnable; //for software controlling the sensor effective exposure time
    bool bMaxManualAeGainEnable;
    double fMaxManualAeGain;
    pthread_mutex_t paramLock;
#endif

public:  // methods
    explicit ControlAE(const std::string &logtag = "ISPC_CTRL_AE");

#ifdef INFOTM_ISP
    virtual ~ControlAE();
#else
    virtual ~ControlAE() {}
#endif

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

    /**
     * @brief Set the update speed for exposure valuess
     *
     * @param value Update speed, value between 0 (no update)
     * and 1 (full update).
     */
    void setUpdateSpeed(double value);

    /** @brief Get the update speed for exposure values */
    double getUpdateSpeed() const;

    /**
     * @brief Set the target brigthness for the capture
     *
     * @param value Target brightness value. Defined between -1
     * (totally black image) and 1 (totally white image)
     */
    void setTargetBrightness(double value);

    /** @brief Get the target metered brightness */
    double getTargetBrightness() const;

    /**
     * @brief Enable flicker rejection
     *
     * @param enable status to change
     * @param freq frequency in Hz to apply - <= 0 means do not change value
     */
    void enableFlickerRejection(bool enable = true, double freq = -1.0);

    /**
    * @brief Enable auto flicker rejection, read frequency from statistics
    * @param enable status to change
    */
    void enableAutoFlickerRejection(bool enable = true);

    /**
     * @brief Returns whether flicker rejection is activated or not
     * 
     * See @ref getFlickerRejectionFrequency()
     */
    bool getFlickerRejection() const;

    /**
     * @brief Returns the flicker rejection frequency regardless of it
     * being active or not
     * See @ref getFlickerRejection()
     */
    double getFlickerRejectionFrequency() const;

    /**
    * @brief Returns whether auto flicker rejection is activated or not
    */
    bool getAutoFlickerRejection() const;

    /**
    * @brief Set the brightness target bracket value (to set the range
    * around the target brightness which will be considered as a correct
    * exposure)
    *
    * @param value New target bracket value to be set
    *
    * See @ref targetBracket @ref getTargetBracket()
    */
    void setTargetBracket(double value);

    /**
    * @brief Get the brighness target bracket value
    *
    * @return applied brightness target bracket value
    *
    * See @ref targetBracket @ref setTargetBracket
    */
    double getTargetBracket() const;

    double getCurrentBrightness() const;

    bool isConverged(void);

    bool isUnderexposed(void);

#ifdef INFOTM_ISP
    void enableAE(bool enable);
    bool IsAEenabled() const;
    IMG_RESULT setAntiflickerFreq(double freq);
    unsigned int getNewExposure(void);
    double getNewGain();
    void getRegionBrightness(double *fRegionBrightness);
    void getRegionOverExp(double *fRegionOverExp);
    void getRegionUnderExp(double *fRegionUnderExp);
    void getRegionRatio(double *fRegionRatio);
    double getOverUnderExpDiff() const;
	double getRatioOverExp() const;
	double getRatioMidOverExp() const;
	double getRatioUnderExp() const;
	unsigned int getAeMode() const;
    void setOriTargetBrightness(double value);
    double getOriTargetBrightness() const;
    bool IsAeTargetMoveEnable(void);
    void AeTargetMoveEnable(bool bEnable);
    int getAeTargetMoveMethod(void);
    void setAeTargetMoveMethod(int iAeTargetMoveMethod);
    double getAeTargetMax(void);
    void setAeTargetMax(double dAeTargetMax);
    double getAeTargetMin(void);
    void setAeTargetMin(double dAeTargetMin);
    double getAeTargetGainNormalModeMaxLmt(void);
    void setAeTargetGainNormalModeMaxLmt(double dAeTargetGainNormalModeMaxLmt);
    double getAeTargetMoveStep(void);
    void setAeTargetMoveStep(double dAeTargetMoveStep);
    double getAeTargetMinLmt(void);
    void setAeTargetMinLmt(double dAeTargetMinLmt);
    double getAeTargetUpOverThreshold(void);
    void setAeTargetUpOverThreshold(double dAeTargetUpOverThreshold);
    double getAeTargetUpUnderThreshold(void);
    void setAeTargetUpUnderThreshold(double dAeTargetUpUnderThreshold);
    double getAeTargetDnOverThreshold(void);
    void setAeTargetDnOverThreshold(double dAeTargetDnOverThreshold);
    double getAeTargetDnUnderThreshold(void);
    void setAeTargetDnUnderThreshold(double dAeTargetDnUnderThreshold);
    int getAeExposureMethod(void);
    void setAeExposureMethod(int iAeExposureMethod);
    double getAeTargetLowluxGainEnter(void);
    void setAeTargetLowluxGainEnter(double dAeTargetLowluxGainEnter);
    double getAeTargetLowluxGainExit(void);
    void setAeTargetLowluxGainExit(double dAeTargetLowluxGainExit);
    int getAeTargetLowluxExposureEnter(void);
    void setAeTargetLowluxExposureEnter(int iAeTargetLowluxExposureEnter);
    double getAeTargetLowluxFPS(void);
    void setAeTargetLowluxFPS(double dAeTargetLowluxFPS);
    double getAeTargetNormalFPS(void);
    void setAeTargetNormalFPS(double dAeTargetNormalFPS);
    bool getAeTargetMaxFpsLockEnable(void);
    void setAeTargetMaxFpsLockEnable(bool enable);
    int getAeBrightnessMeteringMethod(void);
    void setAeBrightnessMeteringMethod(int iAeBrightnessMeteringMethod);
    double getAeRegionDuce(void);
    void setAeRegionDuce(double dAeRegionDuce);
    void setFpsRange(double fMaxFps, double fMinFps);
    void setMaxManualAeGain(double fMaxAeGain, bool bManualEnable);
    double getMaxManualAeGain(void);
    double getMaxOrgAeGain() const;
    double getMaxAeGain() const;
#endif //INFOTM_ISP

protected:
    /**
     * @copydoc ControlModule::configureStatistics()
     *
     * Configures the HIS statistics
     * @return IMG_ERROR_NOT_INITIALISED if sensor is NULL
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * @note Does not use the pipelineList (apply on sensor)
     *
     * Set the sensor exposure and gain using fNewGain and uiNewExposure
     * @return IMG_ERROR_NOT_INITIALISED if sensor is NULL
     */
    virtual IMG_RESULT programCorrection();

public:
    /**
     * @name Auxiliary functions
     * @brief Contains the algorithms knowledge
     * @{
     */

    /**
     * @brief Calculate a metered brithness value from a shot histogram
     * and clipping statistics
     *
     * In this version the brightnes measure is obtained from the 7x7 grid
     * of histograms from the image, using a center weighted approach.
     * A basic backlight detection system is runing by calculating the
     * difference in brightness between foreground and background (defined
     * by a foreground/background mask).
     * In case of backlight illumination detected more weighted is applied
     * to the foreground.
     *
     * @param histogram Histogram metadata from a previous captured shot
     * @param expClipping Clipping statistics from a previous shot
     */
    double getBrightnessMetering(const MC_STATS_HIS &histogram,
        const MC_STATS_EXS &expClipping);

    /**
     * @brief Calculate different stats from a luma histogram: average
     * brigthness and ratios of under and over exposed values
     *
     * @param nHistogram An array of double values which form an histogram.
     * @param nBins Number of bins in the histogram
     * @param[out] avgValue Average brightness of the histogram
     * @param[out] underExposureF Measure of underexposed pixels in the image.
     * The larger this value the more under exposed values are in the image.
     * @param[out] overExposureF Measure of overexposed pixels in the image.
     * The larger this value the more over exposed values in the image.
     */
    void getHistogramStats(double nHistogram[], int nBins, double &avgValue,
        double &underExposureF, double &overExposureF);

    /**
     * @brief Normalize an histogram so all the valuess add up to 1.0
     *
     * @param[in] sourceH Histogram to be normalized
     * @param[out] destinationH Where the normalized histogram values will
     * be copied
     * @param[in] nBins Number of bins in the histogram
     */
    void normalizeHistogram(const IMG_UINT32 sourceH[],
        double destinationH[], int nBins);

    /**
     * @brief Get a weighted combination of statistics from a
     * HIS_REGION_VTILESxHIS_REGION_HTILES (7x7 on first version).
     *
     * @param gridStats a grid with precalculated statistics
     */
    double getWeightedStats(
        double gridStats[HIS_REGION_VTILES][HIS_REGION_HTILES]);

    /**
     * @brief Get weighted measure based in the WEIGHT_7X7_A and
     * WEIGHT_7X7_B weight matrices using a blending between both of them
     *
     * @param gridStats a 7x7 matrix of doubles representing some measure
     * taken from each tile in the 7x7 grid
     * @param combinationWeight blending amount between the weighted result
     * obtained with WEIGHT_7x7_A and WEIGHT_7X7_B matrices
     */
    double getWeightedStatsBlended(
        double gridStats[HIS_REGION_VTILES][HIS_REGION_HTILES],
        double combinationWeight);

    /**
     * @brief Uses the BACKLIGHT_7X7 double value matrix as a mask to define
     * background and foreground and calculates the average brightnes on
     * each region
     *
     * @param gridStats 7x7 double matrix with in which each value
     * represents a measure obtained from the 7x7 grid on the image
     * @param foregroundAvg average brightness for the foreground region
     * @param backgroundAvg average brigthness for the background region
     */
    void getBackLightMeasure(
        double gridStats[HIS_REGION_VTILES][HIS_REGION_HTILES],
        double &foregroundAvg, double &backgroundAvg);

    /**
     * @brief Calculates the next exposure value to get closer to the desired
     * image brightness
     *
     * @param brightnessMetering A metered brightness value obtained from
     * a previous image
     * @param brightnessTarget The desired brightness value
     * @param prevExposure Previous capture exposure value (combined
     * sensor exposure time and gain)
     * @param minExposure Minimum exposure value
     * @param maxExposure Maximum exposure value
     * @param blending multiplier to smooth the change
     *
     * @return exposure value based on previous meterings and sensor settings
     */
    static double autoExposureCtrl(double brightnessMetering,
        double brightnessTarget, double prevExposure, double minExposure,
        double maxExposure, double blending);

    /**
     * @brief Adjusts the exposure time and gain values to be programmed
     * in the sensor according to the sensor exposure step (that is if the
     * sensor has a discreet set of programnable exposures)
     *
     * @param exposureStep Sensor exposure step
     * @param minExposure Minimum programmable exposure
     * @param maxExposure Maximum programmable exposure
     * @param ulExposure Exposure time to be set in the sensor
     * @param flGain Gain value to be set in the sensor
     * @param[out] correctedExposure Corrected exposure time to be set in
     * the sensor
     * @param[out] correctedGain Corrected gain value to be set in the sensor
     */
    static void adjustExposureStep(unsigned int exposureStep,
        unsigned int minExposure, unsigned int maxExposure,
        unsigned int &ulExposure, double &flGain,
        unsigned int &correctedExposure, double &correctedGain);

    /**
     * @brief Calculates the exposure and gain values for getting
     * closer to a desired image brightness taking into account sensor
     * settings, flicker rejection etc.
     *
     * @param fBrightnessMetering Brightness metered from a previous frame.
     * @param fBrightnessTarget Target brigthness value.
     * @param fixedGain fixed gain value (applied if it is != 0 only)
     * @param fixedExposure fixed exposure time value
     * (applied if it is != 0 only)
     * @param fPrevExposure previous exposure value applied
     * (combined exposure value, i.e. exposure time * gain)
     * @param fMinExposure minimum sensor exposure time
     * @param fMaxExposure maximum sensor exposure time
     * @param minGain minimum sensor exposure time
     * @param maxGain maximum sensor exposure time
     * @param controlFlicker to know if flicker should be controlled
     * @param fFlickerHz light flickering frequency
     * @param fblending blending factor to smooth update of values
     * @param[out] fNewGain New calculated sensor gain
     * (to be programmed in the sensor)
     * @param[out] ulNewExposure New calculated exposure time
     * (to be programmed in the sensor)
     * @param[out] bUnderexposed is the image underexposed 
     */
    static void getAutoExposure(
        double fBrightnessMetering, double fBrightnessTarget,
        double fixedExposure, double fixedGain,
        double fPrevExposure, double fMinExposure, double fMaxExposure,
        double minGain, double maxGain,
        bool controlFlicker, double fFlickerHz,
        double fblending,
        double &fNewGain, unsigned int &ulNewExposure, bool &bUnderexposed);

    /**
     * @brief Calculates the exposure and gain based on the flicker
     * rejection requirements.
     *
     * @param flFlickerHz light flicker frequency.
     * @param flRequested requested exposure value.
     * @param flMax maximum exposure time
     * @param[out] flGain calculated gain value
     * @param[out] ulExposureMicros calculated exposure time
     */
    static void getFlickerExposure(double flFlickerHz, double flRequested,
        double flMax, double *flGain, unsigned int *ulExposureMicros);

    /**
     * @}
     */

public:  // parameters
    static const ParamDefSingle<bool> AE_FLICKER;
    static const ParamDef<double> AE_FLICKER_FREQ;
    static const ParamDef<double> AE_TARGET_BRIGHTNESS;
    static const ParamDef<double> AE_UPDATE_SPEED;
    static const ParamDef<double> AE_BRACKET_SIZE;

#ifdef INFOTM_ISP
    static const ParamDefSingle<bool> AE_ENABLE_MOVE_TARGET;
    static const ParamDef<int> AE_TARGET_MOVE_METHOD;
    static const ParamDef<double> AE_TARGET_MAX;
    static const ParamDef<double> AE_TARGET_MIN;
    static const ParamDef<double> AE_TARGET_GAIN_NRML_MODE_MAX_LMT;
    static const ParamDef<double> AE_TARGRT_MOVE_STEP;
    static const ParamDef<double> AE_TARGET_MIN_LMT;
    static const ParamDef<double> AE_TARGET_UP_OVER_THRESHOLD;
    static const ParamDef<double> AE_TARGET_UP_UNDER_THRESHOLD;
    static const ParamDef<double> AE_TARGET_DN_OVER_THRESHOLD;
    static const ParamDef<double> AE_TARGET_DN_UNDER_THRESHOLD;

    static const ParamDef<int> AE_TARGET_EXPOSURE_METHOD;
    static const ParamDef<double> AE_TARGET_LOWLUX_GAIN_ENTER;
    static const ParamDef<double> AE_TARGET_LOWLUX_GAIN_EXIT;
    static const ParamDef<int> AE_TARGET_LOWLUX_EXPOSURE_ENTER;
    static const ParamDef<double> AE_TARGET_LOWLUX_FPS;
    static const ParamDef<double> AE_TARGET_NORMAL_FPS;
    static const ParamDefSingle<bool> AE_TARGET_MAX_FPS_LOCK_ENABLE;

    static const ParamDef<int> AE_BRIGHTNESS_METERING_METHOD;
    static const ParamDef<double> AE_REGION_DUCE;
    static const ParamDefSingle<bool> AE_FORCE_CALL_SENSOR_DRIVER_ENABLE;
#endif //INFOTM_ISP

    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_AE_H_ */
