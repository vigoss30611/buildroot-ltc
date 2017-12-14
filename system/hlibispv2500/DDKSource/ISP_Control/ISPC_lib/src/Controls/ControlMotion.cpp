/**
******************************************************************************
 @file ControlAE.cpp

 @brief ISPC::ControlMotion implementation

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

#ifdef INFOTM_MOTION

#define LOG_TAG "ISPC_CTRL_MOTION"

#include "ispc/ControlMotion.h"
#include "ispc/Pipeline.h"
#include "ispc/ModuleHIS.h"
#include "ispc/Sensor.h"
#include <cmath>
#include <felixcommon/userlog.h>

#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#ifndef VERBOSE_CONTROL_MODULES
#define VERBOSE_CONTROL_MODULES 0
#endif

enum{
	MOTION_LEVEL_LOW = 0,
	MOTION_LEVEL_MEDIAN,
	MOTION_LEVEL_HIGH,
	MOTION_LEVEL_NUM,
};

typedef struct _motion_Level_param_{
	double motion_detect_sensitive;
	double motino_detect_update_speed;
	double motion_detect_blockcounter;
}motion_Level_param;

motion_Level_param level_param[MOTION_LEVEL_NUM] = {
	[MOTION_LEVEL_LOW] = {
			.motion_detect_sensitive = 0.95,
			.motino_detect_update_speed = 0.5,
			.motion_detect_blockcounter = 0.2
		},
	[MOTION_LEVEL_MEDIAN] = {
			.motion_detect_sensitive = 0.95,
			.motino_detect_update_speed = 0.5,
			.motion_detect_blockcounter = 0.6
		},
	[MOTION_LEVEL_HIGH] = {
			.motion_detect_sensitive = 0.95,
			.motino_detect_update_speed = 0.5,
			.motion_detect_blockcounter = 1.0
		}
};

const ISPC::ParamDef<double> ISPC::ControlMotion::MOTION_DETECT_SENSITIVE("MOTION_DETECT_SENSITIVE", 0.0, 1.0, 0.5);
const ISPC::ParamDef<double> ISPC::ControlMotion::MOTION_DETECT_UPDATE_SPEED("MOTION_DETECT_UPDATE_SPEED", 0.0, 1.0, 0.1);
const ISPC::ParamDef<int> ISPC::ControlMotion::MOTION_DETECT_MOTIONCOUNT("MOTION_DETECT_MOTIONCOUNT", 0.0, 8.0, 3.0);
const ISPC::ParamDef<double> ISPC::ControlMotion::MOTION_DETECT_BLOCKCOUNTER("MOTION_DETECT_BLOCKCOUNTER", 0.0, 1.0, 0.7);

const ISPC::ParamDef<int> ISPC::ControlMotion::MOTION_DETECT_LEVEL("MOTION_DETECT_LEVEL", 0, 2, 2);


double getcurrTime()
{
    double ret;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    ret = tv.tv_sec + (tv.tv_usec/1000000.0);

    return ret;
}

ISPC::ControlMotion::ControlMotion(const std::string &logTag)
    : ControlModuleBase(logTag)
{
	this->level = 0;
	this->updateSpeed = 0;
	this->sensitive = 0;
	this->motionAlarmTrigger = 0;
	this->motionBlockCounter = 0;
	memset( preNormalizeHist, 0, sizeof(double)*HIS_REGION_VTILES*HIS_REGION_HTILES*HIS_REGION_BINS );
	memset( MotionCounter, 0, sizeof(int)*HIS_REGION_VTILES*HIS_REGION_HTILES );
}

IMG_RESULT ISPC::ControlMotion::load(const ParameterList &parameters)
{
	this->level = parameters.getParameter(ControlMotion::MOTION_DETECT_LEVEL);
	
	motion_Level_param param = level_param[this->level];

	this->BhattacharyyaCoe = param.motion_detect_sensitive;
	
	setUpdateSpeed(param.motino_detect_update_speed);
	
	setMotionCnt(this->skipFrame);

	setMotionBlockTriggerThres(param.motion_detect_blockcounter);
	
	/*this->BhattacharyyaCoe = parameters.getParameter(ControlMotion::MOTION_DETECT_SENSITIVE);
	setUpdateSpeed(parameters.getParameter(ControlMotion::MOTION_DETECT_UPDATE_SPEED));
	//this->motionCnt = parameters.getParameter(ControlMotion::MOTION_DETECT_MOTIONCOUNT);

	setMotionCnt(this->skipFrame);

	setMotionBlockTriggerThres(parameters.getParameter(ControlMotion::MOTION_DETECT_BLOCKCOUNTER));*/

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlMotion::save(ParameterList &parameters, SaveType t) const
{
    return IMG_SUCCESS;
}

#ifdef INFOTM_ISP
IMG_RESULT ISPC::ControlMotion::update(const Metadata &metadata, const Metadata &metadata2)
#else
IMG_RESULT ISPC::ControlMotion::update(const Metadata &metadata)
#endif //INFOTM_DUAL_SENSOR
{
	const MC_STATS_HIS *histogram = &metadata.histogramStats;
	static bool flag = false;
	static int frameCnt = 0;

	if (flag == false)
	{
		recordFirstHistogram(histogram);
		flag = true;
		
		return IMG_SUCCESS;
	}
	
	if (flag == true && (frameCnt % this->skipFrame == 0))
	{
        bool motionTrigger = false;

		for (int i=0;i<HIS_REGION_VTILES;i++)
		{
			for (int j=0;j<HIS_REGION_HTILES;j++)
			{					
				double nRegHistogram[HIS_REGION_BINS];
				normalizeHistogram(histogram->regionHistograms[i][j], nRegHistogram, HIS_REGION_BINS);

				double BC = 0;
				for (int k=0;k<HIS_REGION_BINS;k++)
				{
					BC += sqrt(preNormalizeHist[i][j][k]*nRegHistogram[k]);
				}

				if (BC < this->BhattacharyyaCoe)
				{
					MotionCounter[i][j]++;

					if (MotionCounter[i][j] > this->motionCnt)
					{
						MotionCounter[i][j] = 0;
						
						memcpy(preNormalizeHist[i][j], nRegHistogram, sizeof(double)*HIS_REGION_BINS);
						
						this->motionBlockCounter++;
						
						//this->motionAlarmTrigger = true;
                        //motionTrigger = true;
					}
				}
				else
				{
					MotionCounter[i][j] = 0;

					memcpy(preNormalizeHist[i][j], nRegHistogram, sizeof(double)*HIS_REGION_BINS);
				}
				
			}
		}

        //this->motionAlarmTrigger = motionTrigger;
		
        //if (this->motionAlarmTrigger == true)
        {
			if (this->motionBlockCounter > this->motionBlockCounterThres)
			{
				detectHandler(this->motionBlockCounter);
	                        motionTrigger = true;
				//this->motionAlarmTrigger = false;
			} 
            this->motionAlarmTrigger = motionTrigger;
            this->motionBlockCounter = 0;
        }
	}

	frameCnt++;

    return IMG_SUCCESS;
}

void ISPC::ControlMotion::recordFirstHistogram(const MC_STATS_HIS *histogram)
{
	for (int i=0;i<HIS_REGION_VTILES;i++)
	{
		for (int j=0;j<HIS_REGION_HTILES;j++)
		{
			double nRegHistogram[HIS_REGION_BINS];
			normalizeHistogram(histogram->regionHistograms[i][j], nRegHistogram, HIS_REGION_BINS);

			memcpy(preNormalizeHist[i][j], nRegHistogram, sizeof(double)*HIS_REGION_BINS);
		}
	}
}

void ISPC::ControlMotion::setThresByFramerateVibrate(void)
{
/*
	const Sensor *sensor = getSensor(); 
    	double estimatedFPS = 0.0;
	static double FPSstartTime;

	int neededFPSFrames = (int)floor(sensor->flFrameRate*1.5);

	if (frameCnt%neededFPSFrames == (neededFPSFrames-1))
	{
	    double cT = getcurrTime();

	    estimatedFPS = (double(neededFPSFrames)/(cT-FPSstartTime));
	    FPSstartTime = cT;

		printf("Frame %d - sensor %dx%d @%3.2f - estimated FPS %3.2f (%d frames avg.)\n",
	    frameCnt,
	    sensor->uiWidth, sensor->uiHeight, sensor->flFrameRate,
	    estimatedFPS, neededFPSFrames
	  );
	}
*/
}

void ISPC::ControlMotion::detectHandler(unsigned int BlockCounter)
{
	//printf("Motion detect!! [%d] Block detected\n", BlockCounter);
	//printf("update Speed %f, BC %f, BlockCnt %d\n", this->updateSpeed, this->BhattacharyyaCoe, this->motionBlockCounterThres);
}

 void ISPC::ControlMotion::setMotionBlockTriggerThres(double value)
{
    if(value < 0.0 || value > 1.0)
    {
        MOD_LOG_ERROR("Motion Block Number sensitive must be between 0.0 and 1.0 (received: %f)\n", value);
        return;
    }

	this->ObjectSizeSensitive = value;

	value = (value == 1.0) ? 0 : floor((1.0 - value) * 10.0);
	
    this->motionBlockCounterThres = value;
}

double ISPC::ControlMotion::getMotionBlockTriggerThres() const
{
	return this->ObjectSizeSensitive;
}

void ISPC::ControlMotion::setMotionCnt(unsigned int skipFrame)
{
	if (skipFrame < 10)
	{
		this->motionCnt = 1;
	}
	else if(skipFrame < 20 && skipFrame >= 10)
	{
		this->motionCnt = 1;
	}
	else if(skipFrame <= 30 && skipFrame >= 20)
	{
		this->motionCnt = 1;
	}
}

void ISPC::ControlMotion::setUpdateSpeed(double value)
{
    if(value < 0.0 || value > 1.0)
    {
        MOD_LOG_ERROR("Update speed must be between 0.0 and 1.0 (received: %f)\n", value);
        return;
    }

	this->updateSpeed = value;

	value = floor(value*27) + 3;
	
    this->skipFrame = value;

	setMotionCnt(this->skipFrame);

	//return this->skipFrame;
}

double ISPC::ControlMotion::getUpdateSpeed() const
{
    return this->updateSpeed;
}

void ISPC::ControlMotion::setMotionSensitive(double value)
{
    if(value < 0.0 || value > 1.0)
    {
        MOD_LOG_ERROR("Motion sensitive must be between 0.0 and 1.0 (received: %f)\n", value);
        return;
    }

	this->sensitive = value;

	value = (1.0 - value * 1.0);
	
    this->BhattacharyyaCoe = value;
}

double ISPC::ControlMotion::getMotionSensitive() const
{
    return this->sensitive;
}

void ISPC::ControlMotion::setMotionLevel(int value)
{
    if(value != 0 && value != 1 && value != 2)
    {
        MOD_LOG_ERROR("Motion sensitive must be between 0.0 and 2.0 (received: %d)\n", value);
        return;
    }

	this->level = value;
	
	motion_Level_param param = level_param[this->level];

	this->BhattacharyyaCoe = param.motion_detect_sensitive;
	
	setUpdateSpeed(param.motino_detect_update_speed);
	
	setMotionCnt(this->skipFrame);

	setMotionBlockTriggerThres(param.motion_detect_blockcounter);
}

int ISPC::ControlMotion::getMotionLevel() const
{
    return this->level;
}

bool ISPC::ControlMotion::getMotionTrigger(void) const
{
    return this->motionAlarmTrigger;
}

IMG_RESULT ISPC::ControlMotion::configureStatistics()
{
    //return IMG_ERROR_UNEXPECTED_STATE;
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ControlMotion::programCorrection()
{
    return IMG_SUCCESS;
}

//normalize an histogram so all the valuess add up to 1.0
void ISPC::ControlMotion::normalizeHistogram(const IMG_UINT32 sourceH[], double destinationH[], int nBins)
{
    double totalHistogram = 0;
    int i;

    for(i=0;i<nBins;i++)
    {
        totalHistogram +=sourceH[i];
    }

    if ( totalHistogram != 0 )
    {
        for(i=0;i<nBins;i++)
            destinationH[i] = ((double)sourceH[i])/totalHistogram;
    }
    else
    {
#ifndef INFOTM_ISP	
        MOD_LOG_WARNING("Total histogram is 0!\n");
#endif //INFOTM_ISP		
        for(i=0;i<nBins;i++)
        {
            destinationH[i] = (double)sourceH[i];
        }
    }
}

#endif //INFOTM_MOTION
