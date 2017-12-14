/**
******************************************************************************
 @file ControlTNM.h

 @brief Declaration of the ISPC::ControlTNM class

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
#ifndef ISPC_CONTROL_TNM_H_
#define ISPC_CONTROL_TNM_H_

#include <string>
#include <ostream>

#include "ispc/Module.h"
#include "ispc/Matrix.h"
#include "ispc/Parameter.h"

namespace ISPC {

#ifdef INFOTM_ISP
typedef enum _GMA_CRV_MODE {
	GMA_CRV_MODE_GAMMA = 0,
	GMA_CRV_MODE_BEZIER_LINEAR_QUADRATIC,
	GMA_CRV_MODE_BEZIER_QUADRATIC,
	GMA_CRV_MODE_MAX
} GMA_CRV_MODE, *PGMA_CRV_MODE;
#endif //INFOTM_ISP

#define TNMC_N_CURVE (65)
#define TNMC_N_HIST (64)

/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief Tone mapping automatic update control module. Deals with the global
 * mapping curve and local tone mapping strength.
 *
 * The ControlTNM control module is in charge of generating and update the
 * global tone mapping curve based on the statistics (HIS) from the previous
 * frames.
 * The algorithm smoothly updates the previously applied tone mapping curve to
 * avoid too abrupt changes in the image look.
 * This class also provides an adaptive tone mapping stregth control.
 * So when large Sensor Gain values are programmed in the sensor the strength
 * of the tone mapping curve and local tone mapping is reduced to avoid
 * enhancing noise.
 *
 * Uses the ModuleHIS statistics to modify the ModuleTNM
 */
class ControlTNM: public ControlModuleBase<CTRL_TNM>
{
protected:
    /**
     * @brief Dynamically adjusted tone mapping strength - should not be
     * accessible from outside
     */
    double adaptiveStrength;

    /**
     * @brief Histogram clipping maximum value for global tone mapping
     * curve generation.
     */
    double histMin;
    /**
     * @brief Histogram clipping min value for global tone mapping
     * curve generation.
     */
    double histMax;
    /**
     * @brief Smoothing value from 0.0 (no smoothing) to 1.0 (max smoothing)
     * for the global mapping curve generation.
     */
    double smoothing;
    /**
     * @brief Tempering value between 0.0 (no tempering) to 1.0
     * (max tempering) for the global mapping curve genration.
     * More tempering produces more gentle mapping curves
     */
    double tempering;
    /**
     * @brief Update speed for application of the newly generated global
     * curves. (0 == no update, 1.0 == instant update).
     */
    double updateSpeed;

    /** @brief Histogram obtained from a previously captured frames */
    Matrix histogram;
    /**
     * @brief Global tone mapping curve generated from previously frame(s)
     * histogram
     */
    Matrix mappingCurve;

    /** @brief Local tone mapping enabled/disabled */
    bool localTNM;
    /** @brief Adaptive tone mapping strength control enabled/disabled */
    bool adaptiveTNM;
    bool adaptiveTNM_FCM;
    /** @brief Local tone mapping strength */
    double localStrength;

    /**
     * @brief To know if we can configure the HIS module or if another
     * control block is responsible to do it
     */
    bool configureHis;

public:
    /** @brief Constructor setting default values */
    explicit ControlTNM(const std::string &logname = "ISPC_CTRL_TNM");

    virtual ~ControlTNM() {}

    /** @copydoc ControlModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ControlModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /** @copydoc ControlModule::update() */
    virtual IMG_RESULT update(const Metadata &metadata);

    /**
     * @name Accessors for parameters
     * @brief To override values loaded in load()
     * @{
     */

#ifdef INFOTM_ISP
    double getAdaptiveStrength(void) const;

#endif //INFOTM_ISP
    /**
     * @brief Set the minimum clipping value for the histogram when
     * generating the global mapping curve
     *
     * @param value in range for TNMC_HISTMIN
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if value is out of range
     *
     * See @ref getHistMin() @ref histMin
     */
    IMG_RESULT setHistMin(double value);

    /**
     * @brief Return the minimum histogram clipping value applied in the
     * global mapping generation.
     *
     * See @ref setHistMin() @ref histMin
     */
    double getHistMin() const;

    /**
     * @brief Set the maximum clipping value for the histogram when
     * generating the global mapping curve
     *
     * @param value in range for TNMC_HISTMAX
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if value is out of range
     *
     * See @ref getHistMax() @ref histMax
     */
    IMG_RESULT setHistMax(double value);

    /**
     * @brief Get the maximum histogram clipping value applied in the global
     * mapping curve generation
     *
     * See @ref setHistMax() @ref histMax
     */
    double getHistMax() const;

    /**
     * @brief Set the tempering value employed in the global tone mapping
     * curve generation
     *
     * @param value in range for TNMC_TEMPERING
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if value is out of range
     *
     * See @ref getTempering() @ref tempering
     */
    IMG_RESULT setTempering(double value);

    /**
     * @brief Get the tempering value employed in the global tone mapping
     * curve generation
     *
     * See @ref setTempering() @ref tempering
     */
    double getTempering() const;

    /**
     * @brief Set the smoothing value applied in the global mapping curve
     * generation
     *
     * @param value in range for TNMC_SMOOTHING
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if value is out of range
     *
     * See @ref getSmoothing() @ref smoothing
     */
    IMG_RESULT setSmoothing(double value);

    /**
     * @brief Get the smoothing value applied in the global mapping curve
     * generation
     *
     * See @ref setSmoothing() @ref smoothing
     */
    double getSmoothing() const;

    /**
     * @brief Set the update speed for the global mapping curve
     *
     * @param value in range for TNMC_UPDATESPEED
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if value is out of range
     *
     * See @ref getUpdateSpeed() @ref updateSpeed
     */
    IMG_RESULT setUpdateSpeed(double value);

    /**
     * @brief Get the update speed for the global mapping curve
     *
     * See @ref setUpdateSpeed() @ref updateSpeed
     */
    double getUpdateSpeed() const;

#ifdef INFOTM_ISP
    IMG_RESULT getHistogram(double *padBuf);

    IMG_RESULT getMappingCurve(double *padBuf);

#endif //INFOTM_ISP
    /**
     * @brief Set the local tone mapping strength
     *
     * @param value in range for TNMC_LOCAL_STRENGTH
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if value is out of range
     *
     * See @ref getLocalStrength() @ref localStrength
     */
    IMG_RESULT setLocalStrength(double value);

    /**
     * @brief Get the local tone mapping strength
     *
     * See @ref setLocalStrength() @ref localStrength
     */
    double getLocalStrength() const;

    /**
     * @brief Enable/disable local tone mapping
     */
    void enableLocalTNM(bool enable = true);

    /**
     * @brief Returns if local tone mapping is enabled or disabled
     *
     * @return Local tone mapping enabled/disabled
     */
    bool getLocalTNM() const;

    /**
     * @brief Enables/disable adaptive tone mapping strength based on sensor
     * set ISO value.
     *
     * Enables the adaptive tone mapping strength (for both local and
     * global tone mapping) based on sensor ISO setting.
     * The larger the ISO setting the smaller the tone mapping strength to
     * avoid noise enhancement
     */
    void enableAdaptiveTNM(bool enable = true);

    /**
     * @brief returns whether the adaptive tone mapping is enabled/disabled
     * @return Adaptive tone mapping enabled/disabled
     */
    bool getAdaptiveTNM() const;

#ifdef INFOTM_ISP
    /**
     * @brief Enables/disable adaptive tone mapping strength based on sensor
     * set ISO value.
     *
     * Enables the adaptive tone mapping strength (for both local and
     * global tone mapping) based on sensor ISO setting.
     * The larger the ISO setting the smaller the tone mapping strength to
     * avoid noise enhancement
     */
    void enableAdaptiveTNM_FCM(bool enable = true);

    /**
     * @brief returns whether the adaptive tone mapping is enabled/disabled
     * @return Adaptive tone mapping enabled/disabled
     */
    bool getAdaptiveTNM_FCM() const;

#endif //INFOTM_ISP
    /**
     * @brief Use to allow the ControlTNM to setup the global HIS in
     * configureStatistics()
     *
     * This is usefull if no other Control algorithms are running.
     *
     * But if using other algorithms (e.g. ControlAE) this should be disabled.
     */
    void setAllowHISConfig(bool enable);

    /**
     * @brief To know if configureStatistics() will modify HIS global
     * histogram configuration
     */
    bool getAllowHISConfig() const;

    /** @note always true */
    virtual bool hasConverged() const;

    virtual std::ostream& printState(std::ostream &os) const;

#ifdef INFOTM_ISP
    int getSmoothHistogramMethod(void);
    IMG_RESULT setSmoothHistogramMethod(int iSmoothHistogramMethod);
    bool isEqualizationEnabled();
    IMG_RESULT enableEqualization(bool bEnable);
    bool isGammaEnabled(void);
    IMG_RESULT enableGamma(bool bEnable);
    double getGammaFCM(void);
    IMG_RESULT setGammaFCM(double gamma);
    double getGammaACM(void);
    IMG_RESULT setGammaACM(double gamma);
    double getEqualMaxBrightSupress(void);
    IMG_RESULT setEqualMaxBrightSupress(double ratio);
    double getEqualMaxDarkSupress(void);
    IMG_RESULT setEqualMaxDarkSupress(double ratio);
    double getEqualBrightSupressRatio(void);
    IMG_RESULT setEqualBrightSupressRatio(double ratio);
    double getEqualDarkSupressRatio(void);
    IMG_RESULT setEqualDarkSupressRatio(double ratio);
    double getEqualFactor(void);
    IMG_RESULT setEqualFactor(double fFactor);
    double getOvershotThreshold(void);
    IMG_RESULT setOvershotThreshold(double fThreshold);
    IMG_RESULT getWdrCeiling(double * pfWdrCeiling);
    IMG_RESULT setWdrCeiling(double * pfWdrCeiling);
    IMG_RESULT getWdrFloor(double * pfWdrFloor);
    IMG_RESULT setWdrFloor(double * pfWdrFloor);
    double getMapCurveUpdateDamp(void);
    IMG_RESULT setMapCurveUpdateDamp(double fUpdateDamp);
    double getMapCurvePowerValue(void);
    IMG_RESULT setMapCurvePowerValue(double fPowerValue);
    int getGammaCurveMode(void);
    IMG_RESULT setGammaCurveMode(int Mode);
    double getBezierCtrlPnt(void);
    IMG_RESULT setBezierCtrlPnt(double point);
    bool isTnmCrvSimBrightnessContrastEnabled(void);
    IMG_RESULT enableTnmCrvSimBrightnessContrast(bool bEnable);
    double getTnmCrvBrightness(void);
    IMG_RESULT setTnmCrvBrightness(double dbBrightness);
    double getTnmCrvContrast(void);
    IMG_RESULT setTnmCrvContrast(double dbContrast);
#endif //INFOTM_ISP

    /**
     * @}
     */

protected:
    /**
     * @copydoc ControlModule::configureStatistics()
     *
     * if configureHis enables the global HIS
     */
    virtual IMG_RESULT configureStatistics();

    /**
     * @copydoc ControlModule::programCorrection()
     *
     * Program the TNM module with the generated curve and parameters
     */
    virtual IMG_RESULT programCorrection();

public:
    /**
     * @name Auxiliary functions
     * @brief Contains the algorithms knowledge
     * @{
     */

    /**
     * @brief Resets an histogram setting it to an evenly distributed one
     *
     * @param[out] hist to be recomputed (1xTNMC_N_HIST elements)
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if hist does not have correct
     * dimentions
     */
    static IMG_RESULT resetHistogram(Matrix &hist);

    /**
     * @brief Resets a mapping curve setting it to an identity curve
     *
     * @param[out] curve to be recomputed (1xTNMC_N_CURVE elements)
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_INVALID_PARAMETERS if curve does not have correct
     * dimentions
     */
    static IMG_RESULT resetCurve(Matrix &curve);

    /**
     * @brief Histogram smoothing with a 7-sized kernel with
     * 0.05 0.1 0.2 0.3 0.2 0.1 0.05 weights.
     * (A 64 element histogram is assumed)
     *
     * @param hist Histogram to be smoothed
     */
    static IMG_RESULT smoothHistogram(Matrix &hist);

    /**
     * @brief Smooth histogram a given amount
     *
     * @param hist Histogram to be smoothed
     * @param amount Strength of the smoothing (0 no smoothing, 1.0 full
     * smoothing)
     *
     * See @ref smoothHistogram
     */
    static IMG_RESULT smoothHistogram(Matrix &hist, double amount);

    /**
     * @brief Generates a tone mapping curve by integrating a given
     * histogram bins.
     *
     * @param hist Histogram to generate the mapping curve ferom
     *
     * @return Mapping curve obtained from the histogram integration
     */
    static Matrix accumulateHistogram(const Matrix &hist);

    /**
     * @brief load HIS global histogram from shot metadata into hist
     *
     * @param metadata Metadata corresponding to a previosuly captured shot
     * @param[out] hist
     */
    static void loadHistogram(const Metadata &metadata, Matrix &hist);

    /**
     * @brief Generate the mapping curve from the image histogram
     *
     * @param[in,out] hist histogram statistics - will be smoothed and
     * normalised
     * @param[in,out] mappingCurve generated curve - will be modified or
     * reset to identity if hist sum is 0
     * @param minHist used for hist smoothing
     * @param maxHist used for hist smoothing
     * @param smoothing factor for the smoothing
     * @param tempering used for histogram flattening
     * @param updateSpeed speed of updates
     *
     * See @ref accumulateHistogram() @ref resetCurve()
     */
    static void generateMappingCurve(Matrix &hist, double minHist,
        double maxHist, double smoothing, double tempering,
        double updateSpeed, Matrix &mappingCurve);

    /**
     * @}
     */

public:  // parameters
    static const ParamDef<float> TNMC_TEMPERING;
    static const ParamDef<float> TNMC_HISTMIN;
    static const ParamDef<float> TNMC_HISTMAX;
    static const ParamDef<float> TNMC_SMOOTHING;
    static const ParamDef<float> TNMC_UPDATESPEED;
    static const ParamDefSingle<bool> TNMC_LOCAL;
    static const ParamDef<float> TNMC_LOCAL_STRENGTH;
    static const ParamDefSingle<bool> TNMC_ADAPTIVE;
    static const ParamDefSingle<bool> TNMC_ADAPTIVE_FCM;
    // not accessible from outside
    // static const ParamDef<float> TNMC_ADAPTIVE_STRENGTH;

#ifdef INFOTM_ISP
    static const ParamDef<int> TNMC_SMOOTH_HISTOGRAM_METHOD;
    static const ParamDefSingle<bool> TNMC_ENABLE_EQUALIZATION;
    static const ParamDefSingle<bool> TNMC_ENABLE_GAMMA;
	static const ParamDef<double> TNMC_EQUAL_MAXDARKSUPRESS;
	static const ParamDef<double> TNMC_EQUAL_MAXBRIGHTSUPRESS;
	static const ParamDef<double> TNMC_EQUAL_DARKSUPRESSRATIO;
	static const ParamDef<double> TNMC_EQUAL_BRIGHTSUPRESSRATIO;
	static const ParamDef<double> TNMC_EQUAL_FACTOR;
	static const ParamDef<double> TNMC_OVERSHOT_THRESHOLD;
	static const ParamDefArray<double> TNMC_WDR_CEILING;
	static const ParamDefArray<double> TNMC_WDR_FLOOR;
	static const ParamDef<double> TNMC_MAPCURVE_UPDATE_DAMP;
	static const ParamDef<double> TNMC_MAPCURVE_POWER_VALUE;
	static const ParamDef<double> TNMC_GAMMA_FCM;
	static const ParamDef<double> TNMC_GAMMA_ACM;
	static const ParamDef<int> TNMC_GAMMA_CRV_MODE;
	static const ParamDef<double> TNMC_BEZIER_CTRL_PNT;
  #if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
    static const ParamDefSingle<bool> TNMC_CRV_SIM_BRIGHTNESS_CONTRAST_ENABLE;
    static const ParamDef<double> TNMC_CRV_BRIGNTNESS;
    static const ParamDef<double> TNMC_CRV_CONTRAST;
  #endif //#if defined(USE_TNM_CRV_SIM_BRIGHTNESS_CONTRAST)
#endif //INFOTM_ISP

    static ParameterGroup getGroup();
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_TNM_H_ */
