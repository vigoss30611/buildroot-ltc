/**
******************************************************************************
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
#ifndef CONTROL_MOTION_H
#define CONTROL_MOTION_H
#include "ispc/Module.h"
#include "ispc/ISPCDefs.h"
#include "ispc/Parameter.h"

#ifdef INFOTM_MOTION

namespace ISPC {

class Sensor; // "ispc/sensor.h"

class ControlMotion: public ControlModuleBase<CTRL_MOTION>
{
public:
	double BhattacharyyaCoe;
	unsigned int skipFrame;
	unsigned int motionCnt;
	unsigned int motionBlockCounterThres;
	unsigned int level;
		
	double updateSpeed;
	double sensitive;
	double ObjectSizeSensitive;
	
	bool motionAlarmTrigger;
	unsigned int motionBlockCounter;

protected:	
	double preNormalizeHist[HIS_REGION_VTILES][HIS_REGION_HTILES][HIS_REGION_BINS];
	int MotionCounter[HIS_REGION_VTILES][HIS_REGION_HTILES];
    
public:

    /**
    * @brief Default constructor
    */
    ControlMotion(const std::string &logtag = "ISPC_CTRL_MOTION");


    /**
     * @brief Virtual destructor
     */
    virtual ~ControlMotion(){}

	    /**
    * @copydoc ControlModule::load()
    */
    virtual IMG_RESULT load(const ParameterList &parameters);

    /**
    * @copydoc ControlModule::save()
    */
    virtual IMG_RESULT save(ParameterList &parameters, SaveType t) const;

    /**
     * @copydoc ControlModule::update()
     */
#ifdef INFOTM_ISP
    virtual IMG_RESULT update(const Metadata &metadata, const Metadata &metadata2);
#else
    virtual IMG_RESULT update(const Metadata &metadata);
#endif //INFOTM_DUAL_SENSOR

	/**
    * @brief Normalize an histogram so all the valuess add up to 1.0
    * @param[in] sourceH Histogram to be normalized
    * @param[out] destinationH Where the normalized histogram values will be copied
    * @param[in] nBins Number of bins in the histogram
    */
    void normalizeHistogram(const IMG_UINT32 sourceH[], double destinationH[], int nBins);

	void setUpdateSpeed(double value);

    double getUpdateSpeed() const;

	void setMotionSensitive(double value);

    double getMotionSensitive() const;

    void setMotionBlockTriggerThres(double value);

    double getMotionBlockTriggerThres() const;

    bool getMotionTrigger(void) const;

	void setMotionCnt(unsigned int skipFrame);

	void detectHandler(unsigned int BlockCounter);

	void recordFirstHistogram(const MC_STATS_HIS *histogram);

	void setThresByFramerateVibrate(void);

	void setMotionLevel(int value);

	int getMotionLevel() const;
		
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
    const static ParamDef<double> MOTION_DETECT_SENSITIVE;
    const static ParamDef<double> MOTION_DETECT_UPDATE_SPEED;
    const static ParamDef<int> MOTION_DETECT_MOTIONCOUNT;
	const static ParamDef<double> MOTION_DETECT_BLOCKCOUNTER;	
	const static ParamDef<int> MOTION_DETECT_LEVEL;

   
};

} // namespace ISPC

#endif //INFOTM_MOTION

#endif
