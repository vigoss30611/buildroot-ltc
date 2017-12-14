/**
*******************************************************************************
 @file ControlAWB_PID.h

 @brief Automatic White Balance control class.
 Derived from ControlModule class to implement generic control modules

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
#ifndef ISPC_CONTROL_AWB_PID_H_
#define ISPC_CONTROL_AWB_PID_H_

#include <string>

#include "ispc/Module.h"
#include "ispc/Parameter.h"
#include "ispc/ControlAWB.h"

#ifdef INFOTM_ISP
  #if defined(INFOTM_HW_AWB_METHOD)
typedef enum _HW_AWB_METHOD
{
    HW_AWB_METHOD_PIXEL_SUMMATION,
    HW_AWB_METHOD_WEIGHTED_GRW_GBW_W,
    HW_AWB_METHOD_WEIGHTED_RW_GW_BW,
    HW_AWB_METHOD_MAX
} HW_AWB_METHOD, *PHW_AWB_METHOD;

typedef enum _HW_AWB_FIRST_PIXEL
{
    HW_AWB_FIRST_PIXEL_R,
    HW_AWB_FIRST_PIXEL_GR,
    HW_AWB_FIRST_PIXEL_GB,
    HW_AWB_FIRST_PIXEL_B,
    HW_AWB_FIRST_PIXEL_MAX
} HW_AWB_FIRST_PIXEL, *PHW_AWB_FIRST_PIXEL;

typedef struct _HW_AWB_STAT
{
    double fGR;
    double fGB;
    unsigned int uiQualCnt;
} HW_AWB_STAT, *PHW_AWB_STAT;
  #endif// INFOTM_HW_AWB_METHOD
#endif //INFOTM_ISP

namespace ISPC {

/**
 * @ingroup ISPC2_CONTROLMODULES
 * @brief Another implementation of the automatic white balance control module
 *
 * Uses the ModuleHIS statistics to update the ModuleCCM and ModuleWBC.
 */
class ControlAWB_PID: public ControlAWB
{
public:
    struct State
    {
        double flMinGain;
        double flMaxGain;
        /** this is the setpoint of the PID loop */
        double flTargetRatio;
        /** this is the accumulated integral of the errors */
        double flIntegral;
        double flLastError;
        /** Proportional tuning parameter */
        double Kp;
        /** derivative tuning parameter */
        double Kd;
        /** integral tuning parameter */
        double Ki;
        bool bClipped;
        bool bConverged;

        void reset();
        void update(double Kp, double Kd, double Ki, double flRIQE);

        static double HWMaxGain();
        static double HWMinGain();
    };

#ifdef INFOTM_ISP
    double dRGain;
    double dBGain;
  #if defined(INFOTM_SW_AWB_METHOD)
    bool bSwAwbEnable;
    bool bUseOriginalCCM;
    ColorCorrection StdCCM;
    double dbImg_rgain;
    double dbImg_bgain;
    double dbSw_rgain;
    double dbSw_bgain;
    double dbSwAwbUseOriCcmNrmlGainLmt;
    double dbSwAwbUseOriCcmNrmlBrightnessLmt;
    double dbSwAwbUseOriCcmNrmlUnderExposeLmt;
    double dbSwAwbUseStdCcmLowLuxGainLmt;
    double dbSwAwbUseStdCcmDarkGainLmt;
    double dbSwAwbUseStdCcmLowLuxBrightnessLmt;
    double dbSwAwbUseStdCcmLowLuxUnderExposeLmt;
    double dbSwAwbUseStdCcmDarkBrightnessLmt;
    double dbSwAwbUseStdCcmDarkUnderExposeLmt;
  #endif //#if defined(INFOTM_SW_AWB_METHOD)
  #if defined(INFOTM_HW_AWB_METHOD)
    bool bHwAwbEnable;
    unsigned int ui32HwAwbMethod;
    unsigned int ui32HwAwbFirstPixel;
    HW_AWB_STAT stHwAwbStat[16][16];
  #endif //INFOTM_HW_AWB_METHOD
  #if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_SW_AWB_METHOD)
    double dbHwSwAwbUpdateSpeed;
    double dbHwSwAwbDropRatio;
    unsigned int uiHwSwAwbWeightingTable[32][32];
  #endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_SW_AWB_METHOD)
    double fRGainMin, fRGainMax;
    double fBGainMin, fBGainMax;
#endif //INFOTM_ISP

protected:  // attributes
    /**
     * @brief The CCM and gains for the D65 light source, used for
     * calculating the IQE
     */
    ColorCorrection Uncorrected;
    /** @brief The state of the PID control loops for red gain */
    ControlAWB_PID::State sRGain;
    /** @brief The state of the PID control loops for blue gain */
    ControlAWB_PID::State sBGain;

    /**
     * @brief number of frames to skip using for processing statistics
     *
     * Loaded using @ref AWB_FRAME_DELAY
     *
     * Can be used to avoid using statistics on systems that have several
     * buffers that delay the application of the new statistics threshold to
     * the HW.
     *
     * @warning this feature has not been tested as number of buffers of the
     * system is recomended to be small (e.g. 2) and therefore the lag is
     * negligible
     */
    unsigned int uiFramesToSkip;
    /**
     * @brief Current number of skipped frames - used for
     * @ref uiFramesToSkip feature
     *
     * When reaching uiFramesToSkip will process the statistics
     */
    unsigned int uiFramesSkipped;

    /**
     * @brief To know if sRGain and sBGain should be reset when calling
     * module enabled
     *
     * Disabling this allows to carry the state of AWB even if it is started
     * and stoped.
     */
    bool bResetStates;
	
#ifdef INFOTM_AWB_PATCH
	double margin;
	double minxgain,maxxgain;
	double minygain,maxygain;
	double p1x,p2x,p1y,p2y;
    double pid_KP,pid_KD,pid_KI;
#endif	
public:
    /**
     * @brief Initializes colour correction to 6500K temperature and
     * statistics thresholds to default values.
    */
    explicit ControlAWB_PID(const std::string &logname = "ISPC_CTRL_AWB-PID");

    virtual ~ControlAWB_PID() {}

    /** @copydoc ControlModule::load() */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /** @copydoc ControlModule::save() */
    virtual IMG_RESULT save(ParameterList &parameters,
        ModuleBase::SaveType t) const;

    /**
    * @copydoc ControlModule::enableControl
    *
    * Resets the sRGain and sBGain objects if bResetStates is true
    */
    virtual void enableControl(bool enable = true);

    /**
     * @copydoc ControlModule::update()
     *
     * This function calculates the required colour correction based on the
     * capture white balance statistics and programs the pipeline accordingly.
     *
     * In this function several actions are performed: The WB statistics
     * contained in the captured shot metadata are processed and the
     * statistics thresholds updated accordingly.
     * The gathered statistics are processed try to invert the colour
     * processing carried out in different modules in the pipeline.
     * The purpose is to try to eliminate the effect of the colour
     * correction module, which is applied before the extraction of the 
     * WB statistics.
     * Once that is done the scene illuminant temperature is estimated and
     * a colour correction matrix is interpolated to compensate for that
     * particular colour temperature.
     * Both the WB statistics modules as well as the CCM and white balance
     * gains are reprogrammed during the call to this function.
     */
    virtual IMG_RESULT update(const Metadata &metadata);

    /**
     * @brief Not used for AWB-PID, does not converge on target temperature
     */
    virtual void setTargetTemperature(double targetTemperature);

    /**
     * @brief Correction temperature is the same than measured temperature
     * for AWB-PID
     */
    virtual double getCorrectionTemperature() const;

    /**
     * @brief Initialise the Red and Blue state and reset skipped frames
     *
     * Called when loading parameters - no need to call explicitely
     */
    void initialiseAWBPID(double Kp, double Kd, double Ki, double flRIQE,
        double flBIQE);

    unsigned int getNumberOfSkipped() const;

    unsigned int getNumberToSkip() const;

    /**
     * @brief If number to skip is > 0 then will force the next frame not to
     * be skipped
     */
    void resetNumberOfSkipped();

    const ControlAWB_PID::State& getRedState() const;

    const ControlAWB_PID::State& getBlueState() const;

    /**
     * @brief Check both Red and Blue state to know if algorithm has converged
     */
    virtual bool hasConverged() const;

    /**
     * @brief Set features of White Balance Temporal Smoothing e.g. flash filtering
     */
    virtual void setWBTSFeatures(unsigned int);

    /** @brief Get bResetStates (see @ref enableControl()) */
    bool getResetStates() const;

    /** @brief Change bResetStates (see @ref enableControl()) */
    void setResetStates(bool b);

    static double PIDGain(ControlAWB_PID::State* sState,
        double flCurrentRatio);
#ifdef INFOTM_ISP
	void enableAWB(bool enable);
	bool IsAWBenabled() const;
#endif //INFOTM_ISP

#ifdef INFOTM_AWB_PATCH
    static double PIDGain(ControlAWB_PID::State* sState, double flCurrentRatio, bool bUpdateInternal, double flNewMinGain, double flNewMaxGain);
	void LimitGains(double margin, double targetx, double targety, double &minxval, double &maxxval, double &minyval, double &maxyval);
#else
    static double PIDGain(ControlAWB_PID::State* sState,
        double flCurrentRatio);
#endif

#ifdef INFOTM_ISP
    double getRGain(void);
    double getBGain(void);
    double getMargin(void);
    void setMargin(double dMargin);
    double getPidKP(void);
    void setPidKP(double dPidKP);
    double getPidKD(void);
    void setPidKD(double dPidKD);
    double getPidKI(void);
    void setPidKI(double dPidKI);
  #ifdef INFOTM_SW_AWB_METHOD
	void sw_awb_gain_set(double rgain, double bgain);
	ColorCorrection getCurrentCCM(void) { return currentCCM; }
  #endif //INFOTM_SW_AWB_METHOD
  #if defined(INFOTM_HW_AWB_METHOD)
	IMG_RESULT HwAwbSetSummationBoundary(HW_AWB_BOUNDARY & HwAwbBoundary);
  #endif //#if defined(INFOTM_HW_AWB_METHOD)
#endif //INFOTM_ISP

public:  // parameters
    static const ParamDef<unsigned> AWB_FRAME_DELAY;
    /** @brief Get the group of parameters used by that class */
    static ParameterGroup getGroup();
#ifdef INFOTM_AWB_PATCH
	const static ParamDef<double> PID_MARGIN;
	const static ParamDef<double> PID_KP;
	const static ParamDef<double> PID_KD;
	const static ParamDef<double> PID_KI;
#endif
#ifdef INFOTM_ISP
	const static ParamDef<double> PID_RGAIN_MIN;
	const static ParamDef<double> PID_RGAIN_MAX;
	const static ParamDef<double> PID_BGAIN_MIN;
	const static ParamDef<double> PID_BGAIN_MAX;

    const static ParamDefSingle<bool> DO_AWB_ENABLE;
#endif //INFOTM_ISP
#if defined(INFOTM_SW_AWB_METHOD)
    static const ParamDefSingle<bool> SW_AWB_ENABLE;
    const static ParamDef<double> SW_AWB_USE_ORI_CCM_NRML_GAIN_LMT;
	const static ParamDef<double> SW_AWB_USE_ORI_CCM_NRML_BRIGHTNESS_LMT;
	const static ParamDef<double> SW_AWB_USE_ORI_CCM_NRML_UNDER_EXPOSE_LMT;
	const static ParamDef<double> SW_AWB_USE_STD_CCM_LOW_LUX_GAIN_LMT;
	const static ParamDef<double> SW_AWB_USE_STD_CCM_DARK_GAIN_LMT;
	const static ParamDef<double> SW_AWB_USE_STD_CCM_LOW_LUX_BRIGHTNESS_LMT;
	const static ParamDef<double> SW_AWB_USE_STD_CCM_LOW_LUX_UNDER_EXPOSE_LMT;
	const static ParamDef<double> SW_AWB_USE_STD_CCM_DARK_BRIGHTNESS_LMT;
	const static ParamDef<double> SW_AWB_USE_STD_CCM_DARK_UNDER_EXPOSE_LMT;
#endif //#if defined(INFOTM_SW_AWB_METHOD)
#if defined(INFOTM_HW_AWB_METHOD)
	const static ParamDefSingle<bool> HW_AWB_ENABLE;
	const static ParamDef<unsigned int> HW_AWB_METHOD;
	const static ParamDef<unsigned int> HW_AWB_FIRST_PIXEL;
#endif //INFOTM_HW_AWB_METHOD
#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_SW_AWB_METHOD)
	const static ParamDef<double> HW_SW_AWB_UPDATE_SPEED;
	const static ParamDef<double> HW_SW_AWB_DROP_RATIO;
	const static ParamDefArray<unsigned int> HW_SW_AWB_WEIGHTING_TBL;
#endif //#if defined(INFOTM_SW_AWB_METHOD) || defined(INFOTM_SW_AWB_METHOD)
};

} /* namespace ISPC */

#endif /* ISPC_CONTROL_AWB_PID_H_ */
