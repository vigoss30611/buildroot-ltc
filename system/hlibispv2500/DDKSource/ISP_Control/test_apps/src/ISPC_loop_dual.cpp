/**
******************************************************************************
@file ISPC_loop_dual.cpp

@brief Example on how to run ISPC library with a forever loop capturing from a
 sensor implemented in Sensor API

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

#include <dyncmd/commandline.h>

#include <iostream>
#include <termios.h>
#include <sstream>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef INFOTM_ISP
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#endif //INFOTM_ISP

#include <ispc/Pipeline.h>
#include <ci/ci_api.h>
#include <ci/ci_version.h>
#include <mc/module_config.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/Camera.h>

// control algorithms
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB_PID.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlDNS.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlAF.h>
#include <ispc/TemperatureCorrection.h>

// modules access
#include <ispc/ModuleOUT.h>
#include <ispc/ModuleCCM.h>
#include <ispc/ModuleDPF.h>
#include <ispc/ModuleWBC.h>
#include <ispc/ModuleLSH.h>

#include <sstream>
#include <ispc/Save.h>
#include <ispctest/info_helper.h>

#include <sensorapi/sensorapi.h> // to get sensor modes

#ifdef EXT_DATAGEN
#include <sensors/dgsensor.h>
#endif
#include <sensors/iifdatagen.h>

#ifdef FELIX_FAKE
#include <ispctest/driver_init.h>
#include <ci_kernel/ci_debug.h>
#endif

#define LOG_TAG "ISPC_loop_dual"
#include <felixcommon/userlog.h>

#define DPF_KEY 'e'
#define LSH_KEY 'r'
#define ADAPTIVETNM_KEY 't'
#define LOCALTNM_KEY 'y'
#define AWB_KEY 'u'
#define DNSISO_KEY 'i'
#define LBC_KEY 'o'
#define AUTOFLICKER_KEY 'p'

#define BRIGHTNESSUP_KEY '+'
#define BRIGHTNESSDOWN_KEY '-'
#define FOCUSCLOSER_KEY ','
#define FOCUSFURTHER_KEY '.'
#define FOCUSTRIGGER_KEY 'm'

#define SAVE_ORIGINAL_KEY 'a'
#define SAVE_RGB_KEY 's'
#define SAVE_YUV_KEY 'd'
#define SAVE_DE_KEY 'f'
#define SAVE_RAW2D_KEY 'g'
#define SAVE_HDR_KEY 'h'

#define TEST_GLITCH_KEY '@'

// auto white balance controls
#define AWB_RESET '0'
#define AWB_FIRST_DIGIT 1
#define AWB_LAST_DIGIT 9
// from 1 to 9 used to select one CCM

#ifdef INFOTM_ISP
//for off line IQ tuning.
//#define ISPC_LOOP_CONTROL_EXPOSURE
#ifdef ISPC_LOOP_CONTROL_EXPOSURE
#define EXPOSUREUP_KEY 'q'
#define EXPOSUREDOWN_KEY 'w'
#define UP_DOWN_STEP	100
#endif //ISPC_LOOP_CONTROL_EXPOSURE
#endif //INFOTM_ISP

#define EXIT_KEY 'x'

/**
 * @brief used to compute the number of frames to use for the FPS count
 * (sensor FPS*multiplier) so that the time is more than a second
 */
#define FPS_COUNT_MULT 1.5

//#define debug printf
#define debug

#ifdef INFOTM_ISP
	#define ISPC_LOOP_DISPLAY_FB_NAME "/dev/fb0"
	#define V2500_MIN_CAM	0
	#define V2500_MAX_CAM	2
#endif //INFOTM_ISP

#ifdef ISPC_LOOP_DISPLAY_FB_NAME

//#include "fb.h"
#define IMAPFB_CONFIG_VIDEO_SIZE   20005

enum Pixel_Format
{
	#define bpp_category(b) (b << 8)
	PFM_NotPixel = 0,
	PFM_NV12 = bpp_category(12),
	PFM_NV21,
	PFM_YUYV422 = bpp_category(16),
	PFM_RGB565,
	PFM_RGBA8888 = bpp_category(32),
};

struct imapfb_info
{
	IMG_UINT32 width;
	IMG_UINT32 height;
	IMG_UINT32 win_width;
	IMG_UINT32 win_height;
	IMG_UINT32 scaler_buff;
	int format;
};

int g_fd_osd0 = -1;

void osd1_disable_color_key(void)
{
	int fd_osd1 = open("/dev/fb1", O_RDWR), size;
	struct fb_var_screeninfo vinfo;

	ioctl(fd_osd1, FBIOGET_VSCREENINFO, &vinfo);
	LOG_INFO("osd1 info: bpp%d, std%d, x%d, y%d, vx%d, vy%d\n",
	 vinfo.bits_per_pixel, vinfo.nonstd, vinfo.xres, vinfo.yres,
	 vinfo.xres_virtual, vinfo.yres_virtual);

	size = vinfo.xres * vinfo.yres * 4 * 2;
	void *buf = malloc(size);
	memset(buf, 0x01, size);
	write(fd_osd1, buf, size);
	free(buf);
	close(fd_osd1);
}

int osd0_open()
{
	int fd = -1;
	fd = open("/dev/fb0", O_RDWR);
	if(fd < 0)
	{
		LOG_INFO("failed to open fb0\n");

	}
	return fd;
}

void osd0_close(int fd)
{
	close(fd);
}

void osd0_set(int fd, IMG_UINT32 addr, int w, int h, Pixel_Format pixf)
{
	struct imapfb_info iinfo;
	if (fd < 0)
		return;

	iinfo.width = w;
	iinfo.height = h;
	iinfo.win_width = w;
	iinfo.win_height = h;
	iinfo.scaler_buff = addr;
	iinfo.format   = (pixf == PFM_NV21)? 2:
					((pixf == PFM_NV12)? 1: 0);

	if (ioctl(fd, IMAPFB_CONFIG_VIDEO_SIZE, &iinfo) == -1)
	{
		LOG_INFO("set osd0 size failed\n");
		return;
	}
}

void osd0_show(int fd, IMG_UINT32 PhyAddr)
{
	struct fb_var_screeninfo vinfo;

	if (fd < 0)
		return;

	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
	vinfo.reserved[0] = PhyAddr;
	ioctl(fd, FBIOPAN_DISPLAY, &vinfo);
}
#endif //ISPC_LOOP_DISPLAY_FB_NAME

/**
 * @return seconds for the FPS calculation
 */
double currTime()
{
    double ret;
    struct timeval tv;

    gettimeofday(&tv, NULL);
    ret = tv.tv_sec + (tv.tv_usec/1000000.0);

    return ret;
}

void printUsage()
{
    DYNCMD_PrintUsage();

    printf("\n\nRuntime key controls:\n");
    printf("\t=== Control Modules toggle ===\n");
    printf("%c defective pixels\n", DPF_KEY);
    printf("%c use LSH matrix if available\n", LSH_KEY);
    printf("%c adaptive TNM\n", ADAPTIVETNM_KEY);
    printf("%c local TNM\n", LOCALTNM_KEY);
    printf("%c enable AWB\n", AWB_KEY);
    printf("%c enable DNS update from sensor\n", DNSISO_KEY);
    printf("%c enable LBC\n", LBC_KEY);
    printf("\t=== Auto Exposure ===\n");
    printf("%c/%c Target brightness up/down (or exposure if AE disabled)\n",
        BRIGHTNESSUP_KEY, BRIGHTNESSDOWN_KEY);
    printf("%c Flicker detection toggle\n", AUTOFLICKER_KEY);
    printf("\t=== Auto Focus ===\n");
    printf("%c/%c Focus closer/further\n", FOCUSCLOSER_KEY, FOCUSFURTHER_KEY);
    printf("%c auto focus trigger\n", FOCUSTRIGGER_KEY);
    printf("\t=== Multi Colour Correction Matrix ===\n");
    printf("%c force identity CCM if AWB disabled\n", AWB_RESET);
    printf("%d-%d load one of the CCM if available and AWB disabled\n", AWB_FIRST_DIGIT, AWB_LAST_DIGIT);
    printf("\t=== Other controls ===\n");
    printf("%c Save all outputs as specified in setup file\n", SAVE_ORIGINAL_KEY);
    printf("%c Save RGB output\n", SAVE_RGB_KEY);
    printf("%c Save YUV output\n", SAVE_YUV_KEY);
    printf("%c Save DE output\n", SAVE_DE_KEY);
    printf("%c Save RAW2D output (may not work on all HW versions)\n", SAVE_RAW2D_KEY);
    printf("%c Save HDR output (may not work on all HW versions)\n", SAVE_HDR_KEY);
    printf("\t=== Test issues ===\n");
    printf("%c Insert stopCapture/StartCapture/Enqueue glitch to test HW restart procedure.\n", TEST_GLITCH_KEY);
    printf("%c Exit\n", EXIT_KEY);

    printf("\n\n");

    Sensor_PrintAllModes(stdout);
}

/**
 * @brief Represents the configuration for demo configurable parameters in running time
 */
struct DemoConfiguration
{
    // Auto Exposure
    bool controlAE;             ///< @brief Create a control AE object or not
    double targetBrightness;    ///< @brief target value for the ControlAE brightness
#ifdef INFOTM_ISP
    double oritargetBrightness;    ///< @brief target value for the ControlAE brightness
#endif
    bool flickerRejection;      ///< @brief Enable/Disable Flicker Rejection

    // Tone Mapper
    bool controlTNM;            ///< @brief Create a control TNM object or not
    bool localToneMapping;      ///< @brief Enable/Disable local tone mapping
    bool autoToneMapping;       ///< @brief Enable/Disable tone mapping auto strength control

    // Denoiser
    bool controlDNS;            ///< @brief Create a control DNS object or not
    bool autoDenoiserISO;       ///< @brief Enable/Disable denoiser ISO control

    // Light Based Controls
    bool controlLBC;            ///< @brief Create a control LBC object or not
    bool autoLBC;               ///< @brief Enable/Disable Light Based Control

    // Auto Focus
    bool controlAF;             ///< @brief Create a control AF object or not

    // White Balance
    bool controlAWB;            ///< @brief Create a control AWB object or not
    bool enableAWB;             ///< @brief Enable/Disable AWB Control
    int targetAWB;              ///< @brief forces one of the AWB as CCM (index from 1) (if 0 forces identity, if <0 does not change)
    ISPC::ControlAWB::Correction_Types AWBAlgorithm; ///< @brief choose which algorithm to use - default is combined
#ifdef INFOTM_ISP	
	bool commandAWB;
#endif //INFOTM_ISP
    // always enable display output unless saving DE
    bool alwaysHasDisplay;
    // enable update ASAP in the ISPC::Camera
    bool updateASAP;

    // to know if we should allow other formats than RGB - changes when a key is pressed, RGB is always savable
    bool bSaveRGB;
    bool bSaveYUV;
    bool bSaveDE;
    bool bSaveHDR;
    bool bSaveRaw2D;

    // other controls
    bool bEnableDPF;
    bool bEnableLSH;

    bool bResetCapture; // to know we need to reset to original output

    bool bTestStopStart; // test stop/start capture glitch after enqueue

    unsigned long ulEnqueueDelay;
    unsigned long ulRandomDelayLow;
    unsigned long ulRandomDelayHigh;

    // original formats loaded from the config file
    ePxlFormat originalYUV;
    ePxlFormat originalRGB; // we force RGB to be RGB_888_32 to be displayable on PDP
    ePxlFormat originalBayer;
    CI_INOUT_POINTS originalDEPoint;
    ePxlFormat originalHDR; // HDR only has 1 format
    ePxlFormat originalTiff;

    // other info from command line
    int nBuffers;
    IMG_BOOL8 sensorFlip[2]; // horizontal and vertical
    unsigned int uiSensorInitExposure; // from parameters and also used when restarting to store current exposure
    // using IMG_FLOAT to be available in DYNCMD otherwise would be double
    IMG_FLOAT flSensorInitGain; // from parameters and also used when restarting to store current gain

    char *pszFelixSetupArgsFile[2];
    char *pszSensor[2];
    int sensorMode[2];

    char *pszInputFLX; // for data generators only
    unsigned int gasket; // for data generator only
    unsigned int aBlanking[2]; // for data generator only

    DemoConfiguration()
    {
		int Idx;

        // AE
        controlAE = true; //default: true 
        targetBrightness = 0.0f;  //default: 0.0f
#ifdef INFOTM_ISP
        oritargetBrightness = targetBrightness;
#endif
        flickerRejection = true; //default: false 

        // TNM
#ifdef INFOTM_ENABLE_GLOBAL_TONE_MAPPER
        controlTNM = true; //default: true 
#else
        controlTNM = false;
#endif //INFOTM_ENABLE_GLOBAL_TONE_MAPPER

#ifdef INFOTM_ENABLE_LOCAL_TONE_MAPPER
        localToneMapping = true;
#else
        localToneMapping = false; //default: false 
#endif //INFOTM_ENABLE_LOCAL_TONE_MAPPER

        autoToneMapping = false; //default: false 

        // DNS
        controlDNS = true; //default: true 
        autoDenoiserISO = true; //default: false

        // LBC
        controlLBC = true; //default: true 
        autoLBC = false; //default: false 

        // AF
        controlAF = false; //default: true 

        // AWB
        controlAWB = true; //default: true 
        enableAWB = true; //default: false 
        AWBAlgorithm = ISPC::ControlAWB::WB_COMBINED; //default: ISPC::ControlAWB::WB_COMBINED
        targetAWB = -1; //default: 0
 #ifdef INFOTM_ISP		
		commandAWB = false;
#endif //INFOTM_ISP	
        alwaysHasDisplay = false; //default: true
        updateASAP = false; //default: false

        // savable
        bSaveRGB = false; //default: false
        bSaveYUV = false; //default: false
        bSaveDE = false; //default: false
        bSaveHDR = false; //default: false
        bSaveRaw2D = false; //default: false

        bEnableDPF = true; //default: false
#ifdef INFOTM_ENABLE_LENS_SHADING_CORRECTION
        bEnableLSH = true; //default: false
#else
        bEnableLSH = false;
#endif //INFOTM_ENABLE_LENS_SHADING_CORRECTION
        bResetCapture = false; //default: false
        bTestStopStart = false; //default: false
        originalYUV = PXL_NONE; //default: PXL_NONE
        originalRGB = PXL_NONE; //default: PXL_NONE
        originalBayer = PXL_NONE; //default: PXL_NONE
        originalDEPoint = CI_INOUT_NONE; //default: CI_INOUT_NONE
        originalHDR = PXL_NONE; //default: PXL_NONE
        originalTiff = PXL_NONE; //default: PXL_NONE

        // other information
        nBuffers = 2; //default: 2
        sensorFlip[0] = false; //default: false
        sensorFlip[1] = false; //default: false
        uiSensorInitExposure = 0; //default: 0
        flSensorInitGain = 0.0f; //default: 0.0f

		for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
		{
	        pszFelixSetupArgsFile[Idx] = NULL; //default: NULL
	        pszSensor[Idx] = NULL; //default: NULL
	        sensorMode[Idx] = 0; //default: 0
		} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

        pszInputFLX = NULL; // for data generators only //default: NULL
        gasket = 0; // for data generator only //default: 0
        aBlanking[0] = FELIX_MIN_H_BLANKING; // for data generator only //default: FELIX_MIN_H_BLANKING
        aBlanking[1] = FELIX_MIN_V_BLANKING; //default: FELIX_MIN_H_BLANKING
        ulEnqueueDelay = 0; //default: 0
        ulRandomDelayLow = 0; //default: 0
        ulRandomDelayHigh = 0; //default: 0
    }

    ~DemoConfiguration()
    {
		int Idx;
		for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
		{
	        if(this->pszSensor[Idx])
	        {
	            IMG_FREE(this->pszSensor[Idx]);
	            this->pszSensor[Idx]=0;
	        }
	        if(this->pszFelixSetupArgsFile[Idx])
	        {
	            IMG_FREE(this->pszFelixSetupArgsFile[Idx]);
	            this->pszFelixSetupArgsFile[Idx]=0;
	        }
		} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
    }

    // generateDefault if format is PXL_NONE
    void loadFormats(const ISPC::ParameterList &parameters, bool generateDefault=true)
    {
        originalYUV = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::ENCODER_TYPE));
        LOG_DEBUG("loaded YVU=%s\n", FormatString(originalYUV));
        if ( generateDefault && originalYUV == PXL_NONE )
        {
            originalYUV = YVU_420_PL12_8;
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("default YUV forced to %s\n", FormatString(originalYUV));
#endif //INFOTM_ENABLE_WARNING_DEBUG
        }

        originalRGB = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DISPLAY_TYPE));
        LOG_DEBUG("loaded RGB=%s\n", FormatString(originalRGB));
        if ( generateDefault && originalRGB == PXL_NONE )
        {
            originalRGB = RGB_888_32;
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("default RGB forced to %s\n", FormatString(originalRGB));
#endif //INFOTM_ENABLE_WARNING_DEBUG
        }


        originalBayer = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_TYPE));
        LOG_DEBUG("loaded DE=%s\n", FormatString(originalBayer));
        if ( generateDefault && originalBayer == PXL_NONE )
        {
            originalBayer = BAYER_RGGB_12;
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("default DE forced to %s\n", FormatString(originalBayer));
#endif //INFOTM_ENABLE_WARNING_DEBUG
        }

        originalDEPoint = (CI_INOUT_POINTS)(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_POINT)-1);
        LOG_DEBUG("loaded DE point=%d\n", (int)originalDEPoint);
        if ( generateDefault && originalDEPoint != CI_INOUT_BLACK_LEVEL && originalDEPoint != CI_INOUT_FILTER_LINESTORE )
        {
            originalDEPoint = CI_INOUT_BLACK_LEVEL;
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("default DE-Point forced to %d\n", originalDEPoint);
#endif //INFOTM_ENABLE_WARNING_DEBUG
        }

        originalHDR = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::HDREXTRA_TYPE));
        LOG_DEBUG("loaded HDR=%s (forced)\n", FormatString(BGR_101010_32));
        if ( generateDefault && originalHDR == PXL_NONE )
        {
            originalHDR = BGR_101010_32;
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("default HDF forced to %s\n", FormatString(originalHDR));
#endif //INFOTM_ENABLE_WARNING_DEBUG
        }


        originalTiff = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::RAW2DEXTRA_TYPE));
        LOG_DEBUG("loaded TIFF=%s\n", FormatString(originalTiff));
        if ( generateDefault && originalTiff == PXL_NONE )
        {
            originalTiff = BAYER_TIFF_12;
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            LOG_INFO("default Raw2D forced to %s\n", FormatString(originalTiff));
#endif //INFOTM_ENABLE_WARNING_DEBUG
    	}
    }

    /**
     * @brief Apply a configuration to the control loop algorithms
     * @param
     */
    void applyConfiguration(ISPC::Camera &cam)
    {
        //
        // control part
        //
        ISPC::ControlAE *pAE = cam.getControlModule<ISPC::ControlAE>();
        ISPC::ControlAWB_PID *pAWB = cam.getControlModule<ISPC::ControlAWB_PID>();
        ISPC::ControlTNM *pTNM = cam.getControlModule<ISPC::ControlTNM>();
        ISPC::ControlDNS *pDNS = cam.getControlModule<ISPC::ControlDNS>();
        ISPC::ControlLBC *pLBC = cam.getControlModule<ISPC::ControlLBC>();

        if ( pAE )
        {
#if defined(INFOTM_ISP) && defined(INFOTM_ENABLE_AE_DEBUG)
        	printf("felix::applyConfiguration targetBrightness = %6.3f\n", this->targetBrightness);
#endif //INFOTM_ISP
#ifndef INFOTM_ISP
            pAE->setTargetBrightness(this->targetBrightness);
#else
			if (this->oritargetBrightness != pAE->getOriTargetBrightness())
			{
				pAE->setTargetBrightness(this->targetBrightness);
				pAE->setOriTargetBrightness(this->oritargetBrightness);
			}
#endif

            //Enable/disable flicker rejection in the auto-exposure algorithm
            pAE->enableFlickerRejection(this->flickerRejection);
        }

        if ( pTNM )
        {
            // parameters loaded from config file
            pTNM->enableControl(this->localToneMapping||this->autoToneMapping);

            //Enable/disable local tone mapping
            pTNM->enableLocalTNM(this->localToneMapping);

            //Enable/disable automatic tone mapping strength control
            pTNM->enableAdaptiveTNM(this->autoToneMapping);

            if (!pAE || !pAE->isEnabled())
            {
                // TNM needs global HIS, normally configured by AE, if AE not present allow TNM to configure it
                pTNM->setAllowHISConfig(true);
            }
        }

        if ( pDNS )
        {
            //Enable disable denoiser ISO parameter automatic update
            pDNS->enableControl(this->autoDenoiserISO);
        }

        if ( pLBC )
        {
            //Enable disable light based control to set up certain pipeline characteristics based on light level
            pLBC->enableControl(this->autoLBC);

            if ((!pTNM || !pTNM->isEnabled()) && (!pAE || !pAE->isEnabled()))
            {
                // LBC needs global HIS to estimate illumination, normally configured by AE or TNM, if both not present allow LBC to configure it
                pLBC->setAllowHISConfig(true);
            }
        }

        if ( pAWB )
        {
            pAWB->enableControl(this->enableAWB);

            if ( this->targetAWB >= 0 )
            {
#if defined(INFOTM_ISP) && defined(INFOTM_ENABLE_AWB_DEBUG)
                printf("targetAWB = %d\n", this->targetAWB);
#endif //INFOTM_ISP
                ISPC::ModuleCCM *pCCM = cam.getModule<ISPC::ModuleCCM>();
                ISPC::ModuleWBC *pWBC = cam.getModule<ISPC::ModuleWBC>();
                const ISPC::TemperatureCorrection &tempCorr = pAWB->getTemperatureCorrections();

                if ( this->targetAWB == 0 )
                {
                    ISPC::ParameterList defaultParams;
                    pCCM->load(defaultParams); // load defaults
                    pCCM->requestUpdate();

                    pWBC->load(defaultParams);
                    pWBC->requestUpdate();
                }
                else if ( this->targetAWB-1 < tempCorr.size() )
                {
                    ISPC::ColorCorrection currentCCM = tempCorr.getCorrection(this->targetAWB-1);

                    for(int i=0;i<3;i++)
                    {
                        for(int j=0;j<3;j++)
                        {
                            pCCM->aMatrix[i*3+j] = currentCCM.coefficients[i][j];
                        }
                    }

                    //apply correction offset
                    pCCM->aOffset[0] = currentCCM.offsets[0][0];
                    pCCM->aOffset[1] = currentCCM.offsets[0][1];
                    pCCM->aOffset[2] = currentCCM.offsets[0][2];

                    pCCM->requestUpdate();

                    pWBC->aWBGain[0] = currentCCM.gains[0][0];
                    pWBC->aWBGain[1] = currentCCM.gains[0][1];
                    pWBC->aWBGain[2] = currentCCM.gains[0][2];
                    pWBC->aWBGain[3] = currentCCM.gains[0][3];

                    pWBC->requestUpdate();
                }
            } // if targetAWB >= 0
            this->targetAWB = -1;
        }

        //
        // module part
        //

        ISPC::ModuleDPF *pDPF = cam.getModule<ISPC::ModuleDPF>();
        ISPC::ModuleLSH *pLSH = cam.getModule<ISPC::ModuleLSH>();
        ISPC::ModuleOUT *glb = cam.getModule<ISPC::ModuleOUT>();

        this->bResetCapture = false; // we assume we don't need to reset

        ePxlFormat prev = glb->encoderType;
        if ( this->bSaveYUV )
        {
            glb->encoderType = this->originalYUV;
        }
        else
        {
            glb->encoderType = PXL_NONE;
        }
#ifdef INFOTM_ISP
		glb->encoderType = this->originalYUV;
#endif //INFOTM_ISP
        this->bResetCapture |= glb->encoderType != prev;
        debug("Configure YUV output %s (prev=%s)\n", FormatString(glb->encoderType), FormatString(prev));

        prev = glb->dataExtractionType;
        if ( this->bSaveDE )
        {
            glb->dataExtractionType = this->originalBayer;
            glb->dataExtractionPoint = this->originalDEPoint;
            glb->displayType = PXL_NONE;
        }
        else
        {
            glb->dataExtractionType = PXL_NONE;
            glb->dataExtractionPoint = CI_INOUT_NONE;
            if (alwaysHasDisplay)
            {
                LOG_INFO("Re-enable display to %s\n", FormatString(this->originalRGB));
                glb->displayType = this->originalRGB;
            }
            else
            {
                glb->displayType = PXL_NONE;
            }
        }
		
#ifdef INFOTM_ISP		
		//for off line IQ tuning.
		//glb->dataExtractionType = BAYER_RGGB_10;
		//glb->dataExtractionPoint = CI_INOUT_BLACK_LEVEL;
		//glb->dataExtractionPoint = CI_INOUT_FILTER_LINESTORE;

        glb->dataExtractionType = PXL_NONE;
        glb->dataExtractionPoint = CI_INOUT_NONE;
        glb->displayType = PXL_NONE;
#endif //INFOTM_ISP		
		
        this->bResetCapture |= glb->dataExtractionType != prev;
        debug("Configure DE output %s at %d (prev=%s)\n", FormatString(glb->dataExtractionType), (int)glb->dataExtractionPoint, FormatString(prev));
        debug("Configure RGB output %s (prev=%s)\n", FormatString(glb->displayType), FormatString(prev==PXL_NONE?RGB_888_32:PXL_NONE));

        prev = glb->hdrExtractionType;
        if ( this->bSaveHDR ) // assumes the possibility has been checked
        {
            glb->hdrExtractionType = this->originalHDR;
            //this->bResetCapture = true;
        }
        else
        {
            glb->hdrExtractionType = PXL_NONE;
        }
        this->bResetCapture |= glb->hdrExtractionType != prev;
        debug("Configure HDR output %s (prev=%s)\n", FormatString(glb->hdrExtractionType), FormatString(prev));

        prev = glb->raw2DExtractionType;
        if ( this->bSaveRaw2D )
        {
            glb->raw2DExtractionType = this->originalTiff;
            //this->bResetCapture = true;
        }
        else
        {
            glb->raw2DExtractionType = PXL_NONE;
        }
        this->bResetCapture |= glb->raw2DExtractionType != prev;
        debug("Configure RAW2D output %s (prev=%s)\n", FormatString(glb->raw2DExtractionType), FormatString(prev));
        debug("Configure needs to reset camera? %d\n", this->bResetCapture);

        if ( pLSH )
        {
            if ( pLSH->hasGrid() )
            {
                pLSH->bEnableMatrix = this->bEnableLSH;
#if defined(INFOTM_ENABLE_LSH_DEBUG)
                if ( pLSH->bEnableMatrix )
                {
                    printf("LSH matrix from %s\n", pLSH->lshFilename.c_str());
                }
#endif //INFOTM_ENABLE_LSH_DEBUG
                pLSH->requestUpdate();
            }
        }

        if ( pDPF )
        {
            pDPF->bDetect = this->bEnableDPF;
            pDPF->bWrite = this->bEnableDPF;

            pDPF->requestUpdate();
        }
        else
        {
            this->bEnableDPF = false; // so that stats are not read
        }

        if(this->bResetCapture && pAE)
        {
            // if we reset and have Auto Exposure we save current values in initial exposure and gains
            // so that the output does not re-run the loop
            this->uiSensorInitExposure = cam.getSensor()->getExposure();
            this->flSensorInitGain = cam.getSensor()->getGain();

            LOG_INFO("current exposure %u gain %lf\n",
                uiSensorInitExposure,
                flSensorInitGain);
        }
    } // End of void applyConfiguration(ISPC::Camera &cam)

    int loadParameters(int argc, char *argv[])
    {
        int ret;
        int missing = 0;
        bool bPrintHelp = false;
        int Idx;
		char szItemName[20];
        
        //command line parameter loading:
        ret = DYNCMD_AddCommandLine(argc, argv, "-source");
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("unable to parse command line\n");
            DYNCMD_ReleaseParameters();
            return EXIT_FAILURE;
        }
        
        ret = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND, "Usage information", NULL);
        if (RET_FOUND == ret)
        {
            bPrintHelp = true;
        }

		for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
		{
            sprintf(szItemName, "-setupFile%d", Idx);
	        ret=DYNCMD_RegisterParameter(szItemName, DYNCMDTYPE_STRING,
	            "FelixSetupArgs file to use to configure the Pipeline",
	            &(this->pszFelixSetupArgsFile[Idx]));
	        if (RET_FOUND != ret && !bPrintHelp)
	        {
	            if (RET_NOT_FOUND != ret)
	            {
	                LOG_ERROR("while parsing parameter %s\n", szItemName);
	            }
	            else
	            {
	                LOG_ERROR("No setup file provided\n");
	            }
	            missing++;
	        }
		} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

        std::ostringstream os;
        os.str("");
        os << "Sensor to use for the run. If '" << IIFDG_SENSOR_INFO_NAME;
#ifdef EXT_DATAGEN
        os << "' or '" << EXTDG_SENSOR_INFO_NAME;
#endif
        os << "' is used then the sensor will be considered a data-generator.";
        static std::string sensorHelp = os.str(); // static otherwise object is lost and c_str() points to invalid memory

		for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
		{
            sprintf(szItemName, "-sensor%d", Idx);
		    ret=DYNCMD_RegisterParameter(szItemName, DYNCMDTYPE_STRING,
	            (sensorHelp.c_str()),
	            &(this->pszSensor[Idx]));
	        if (RET_FOUND != ret && !bPrintHelp)
	        {
	            if (RET_NOT_FOUND != ret)
	            {
	                LOG_ERROR("while parsing parameter -%s\n", szItemName);
	            }
	            else
	            {
	                LOG_ERROR("no sensor provided\n");
	            }
	            missing++;
	        }

            sprintf(szItemName, "-sensorMode%d", Idx);
	        ret=DYNCMD_RegisterParameter(szItemName, DYNCMDTYPE_INT,
	            "Sensor mode to use",
	            &(this->sensorMode[Idx]));
	        if (RET_FOUND != ret && !bPrintHelp)
	        {
	            if (RET_NOT_FOUND != ret)
	            {
	                LOG_ERROR("while parsing parameter %s\n", szItemName);
	                missing++;
	            }
	        }		
		} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

        ret=DYNCMD_RegisterParameter("-sensorExposure", DYNCMDTYPE_UINT,
            "[optional] initial exposure to use in us",
            &(this->uiSensorInitExposure));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret) // if not found use default
            {
                LOG_ERROR("while parsing parameter -sensorExposure\n");
                missing++;
            }
            // parameter is optional
        }

        ret=DYNCMD_RegisterParameter("-sensorGain", DYNCMDTYPE_FLOAT,
            "[optional] initial gain to use",
            &(this->flSensorInitGain));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret) // if not found use default
            {
                LOG_ERROR("while parsing parameter -sensorGain\n");
                missing++;
            }
            // parameter is optional
        }

        ret=DYNCMD_RegisterArray("-sensorFlip", DYNCMDTYPE_BOOL8,
            "[optional] Set 1 to flip the sensor Horizontally and Vertically. If only 1 option is given it is assumed to be the horizontal one\n",
            this->sensorFlip, 2);
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_INCOMPLETE == ret)
            {
                fprintf(stderr, "Warning: incomplete -sensorFlip, flipping is H=%d V=%d\n",
                    (int)sensorFlip[0], (int)sensorFlip[1]);
            }
            else if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -sensorFlip\n");
                missing++;
            }
        }

        int algo = 0;
        ret=DYNCMD_RegisterParameter("-chooseAWB", DYNCMDTYPE_UINT,
            "Enable usage of White Balance Control algorithm and loading of the multi-ccm parameters - values are mapped ISPC::Correction_Types (1=AC, 2=WP, 3=HLW, 4=COMBINED)",
            &(algo));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret && !bPrintHelp) // if not found use default
            {
                LOG_ERROR("while parsing parameter -chooseAWB\n");
                missing++;
            }
        }
        else
        {
            switch(algo)
            {
                case ISPC::ControlAWB::WB_AC:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_AC;
                    break;

                case ISPC::ControlAWB::WB_WP:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_WP;
                    break;

                case ISPC::ControlAWB::WB_HLW:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_HLW;
                    break;

                case ISPC::ControlAWB::WB_COMBINED:
                    this->AWBAlgorithm = ISPC::ControlAWB::WB_COMBINED;
                    break;

                case ISPC::ControlAWB::WB_NONE:
                default:
                    if (!bPrintHelp)
                    {
                        LOG_ERROR("Unsupported AWB algorithm %d given\n", algo);
                    }
                    missing++;
            }
#ifdef INFOTM_ISP			
			this->commandAWB = true;
#endif //INFOTM_ISP			
        }

        ret = DYNCMD_RegisterParameter("-disableAE", DYNCMDTYPE_COMMAND,
            "Disable construction of the Auto Exposure control object",
            NULL);
        if(RET_FOUND == ret)
        {
            this->controlAE = false;
        }
        
        ret = DYNCMD_RegisterParameter("-disableAWB", DYNCMDTYPE_COMMAND,
            "Disable construction of the Auto White Balance control object",
            NULL);
        if(RET_FOUND == ret)
        {
            this->controlAWB = false;
        }
        
        ret = DYNCMD_RegisterParameter("-disableAF", DYNCMDTYPE_COMMAND,
            "Disable construction of the Auto Focus control object",
            NULL);
        if(RET_FOUND == ret)
        {
            this->controlAF = false;
        }
        
        ret = DYNCMD_RegisterParameter("-disableTNMC", DYNCMDTYPE_COMMAND,
            "Disable construction of the Tone Mapper control object",
            NULL);
        if(RET_FOUND == ret)
        {
            this->controlTNM = false;
        }
        
        ret = DYNCMD_RegisterParameter("-disableDNS", DYNCMDTYPE_COMMAND,
            "Disable construction of the Denoiser control object",
            NULL);
        if(RET_FOUND == ret)
        {
            this->controlDNS = false;
        }
        
        ret = DYNCMD_RegisterParameter("-disableLBC", DYNCMDTYPE_COMMAND,
            "Disable construction of the Light Base Control object",
            NULL);
        if(RET_FOUND == ret)
        {
            this->controlLBC = false;
        }

        ret = DYNCMD_RegisterParameter("-disableAutoDisplay", DYNCMDTYPE_COMMAND,
            "Disable the always enabling of display output (unless taking DE)",
            NULL);
        if(RET_FOUND == ret)
        {
            LOG_WARNING("Disabling auto display!!! PDP will not work\n");
            this->alwaysHasDisplay = false;
        }

        ret = DYNCMD_RegisterParameter("-updateASAP", DYNCMDTYPE_COMMAND,
            "Enable the update ASAP in the ISPC::Camera",
            NULL);
        if (RET_FOUND == ret)
        {
            LOG_WARNING("Enabling update ASAP\n");
            this->updateASAP = true;
        }

        ret = DYNCMD_RegisterParameter("-nBuffers", DYNCMDTYPE_UINT,
            "Number of buffers to run with",
            &(this->nBuffers));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -nBuffers\n");
                missing++;
            }
        }
		
        ret = DYNCMD_RegisterParameter("-delay", DYNCMDTYPE_UINT,
            "Enqueue Delay (overrides Random Delay)",
            &(this->ulEnqueueDelay));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -delay\n");
                missing++;
            }
        }

        ret = DYNCMD_RegisterParameter("-randomDelayLow", DYNCMDTYPE_UINT,
            "Random Delay Low bound",
            &(this->ulRandomDelayLow));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -randomDelayLow\n");
                missing++;
            }
        }

        ret = DYNCMD_RegisterParameter("-randomDelayHigh", DYNCMDTYPE_UINT,
            "Random Delay High bound",
            &(this->ulRandomDelayHigh));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -randomDelayHigh\n");
                missing++;
            }
        }

        unsigned int seed = time(NULL);

        ret = DYNCMD_RegisterParameter("-randomSeed", DYNCMDTYPE_UINT,
            "Random seed - if not provided uses time(NULL)",
            &(seed));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -randomSeed\n");
                missing++;
            }
        }
        srandom(seed);

        ret=DYNCMD_RegisterParameter("-DG_inputFLX", DYNCMDTYPE_STRING,
            "FLX file to use as an input of the data-generator n(only if specified sensor is a data generator)",
            &(this->pszInputFLX));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -DG_inputFLX\n");
                missing++;
            }
        }

        ret=DYNCMD_RegisterParameter("-DG_gasket", DYNCMDTYPE_UINT,
            "Gasket to use for data-generator (only if specified sensor is a data generator)",
            &(this->gasket));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -DG_gasket\n");
                missing++;
            }
        }

        ret=DYNCMD_RegisterArray("-DG_blanking", DYNCMDTYPE_UINT,
            "Horizontal and vertical blanking in pixels (only if specified sensor is data generator)",
            &(this->aBlanking), 2);
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_INCOMPLETE == ret)
            {
                LOG_WARNING("-DG_blanking incomplete, second value for blanking is assumed to be %u\n", this->aBlanking[1]);
            }
            else if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -DG_blanking\n");
                missing++;
            }
        }

        /*
         * must be last action
         */
        if(bPrintHelp || missing > 0)
        {
            printUsage();
            ret = EXIT_FAILURE;
        }
        else
        {
            ret = EXIT_SUCCESS;
        }

        DYNCMD_ReleaseParameters();
        return ret;
    } //End of int loadParameters(int argc, char *argv[])

};

struct LoopCamera
{
    ISPC::Camera *camera;
    std::list<IMG_UINT32> buffersIds;
    const DemoConfiguration &config;
    unsigned int missedFrames;

    enum IsDataGenerator { NO_DG = 0, INT_DG, EXT_DG };
    enum IsDataGenerator isDatagen;

#ifdef INFOTM_ISP
    ISPC::ControlAE *pAE;
    ISPC::ControlAWB_PID *pAWB;
    ISPC::ControlTNM *pTNM;
    ISPC::ControlDNS *pDNS;
    ISPC::ControlLBC *pLBC;
    ISPC::ControlAF *pAF;
    ISPC::Sensor *sensor;
    ISPC::Shot shot;
#endif //INFOTM_ISP

    LoopCamera(DemoConfiguration &_config, IMG_UINT32 ContextNum): camera(0), config(_config)
    {
        pAE = 0;
        pAWB = 0;
        pTNM = 0;
        pDNS = 0;
        pLBC = 0;
        pAF = 0;
        sensor = 0;

        if(strncmp(config.pszSensor[ContextNum], IIFDG_SENSOR_INFO_NAME, strlen(config.pszSensor[ContextNum]))==0)
        {
            isDatagen = INT_DG;

            if (!ISPC::DGCamera::supportIntDG())
            {
                LOG_ERROR("trying to use internal data generator but it is not supported by HW!\n");
                return;
            }
            if(!config.pszInputFLX)
            {
                LOG_ERROR("trying to use internal data generator but not input FLX provided!\n");
                return;
            }

            camera = new ISPC::DGCamera(ContextNum, config.pszInputFLX, config.gasket, true);

            sensor = camera->getSensor();

            // apply additional parameters
            //sensor->setExtendedParam(EXTENDED_IIFDG_CONTEXT, &(testparams.aIndexDG[gasket]));
            IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), config.nBuffers);

            IIFDG_ExtendedSetBlanking(sensor->getHandle(), config.aBlanking[0], config.aBlanking[1]);

            sensor = 0;// sensor should be 0 for populateCameraFromHWVersion
        }
#ifdef EXT_DATAGEN
        else if (strncmp(config.pszSensor, EXTDG_SENSOR_INFO_NAME, strlen(EXTDG_SENSOR_INFO_NAME))==0)
        {

            isDatagen = EXT_DG;

            if (!ISPC::DGCamera::supportExtDG())
            {
                LOG_ERROR("trying to use external data generator but it is not supported by HW!\n");
                return;
            }

            camera = new ISPC::DGCamera(ContextNum, config.pszInputFLX, config.gasket, false);

            sensor = camera->getSensor();

            // apply additional parameters
            DGCam_ExtendedSetBlanking(sensor->getHandle(), config.aBlanking[0], config.aBlanking[1]);

            sensor = 0;// sensor should be 0 for populateCameraFromHWVersion
        }
#endif
        else
        {
            int flipping = (_config.sensorFlip[0]?SENSOR_FLIP_HORIZONTAL:SENSOR_FLIP_NONE)
                | (_config.sensorFlip[1]?SENSOR_FLIP_VERTICAL:SENSOR_FLIP_NONE);
            isDatagen = NO_DG;
            camera = new ISPC::Camera(ContextNum, ISPC::Sensor::GetSensorId(_config.pszSensor[ContextNum]), _config.sensorMode[ContextNum], flipping, 0);

            sensor = camera->getSensor();
        }

        if(camera->state==ISPC::CAM_ERROR)
        {
            LOG_ERROR("failed to create camera correctly\n");
            delete camera;
            camera = 0;
            return;
        }
        
        ISPC::CameraFactory::populateCameraFromHWVersion(*camera, sensor);
        camera->bUpdateASAP = _config.updateASAP;

        missedFrames = 0;
    }

    ~LoopCamera()
    {
        if(camera)
        {
            delete camera;
            camera = 0;
        }
    }

    void printInfo()
    {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
        if (camera)
        {
            std::cout << "*****" << std::endl;
            PrintGasketInfo(*camera, std::cout);
            PrintRTMInfo(*camera, std::cout);
            std::cout << "*****" << std::endl;
        }
#endif //INFOTM_ENABLE_WARNING_DEBUG
    }

    int startCapture()
    {
        if ( buffersIds.size() > 0 )
        {
            LOG_ERROR("Buffers still exists - camera was not stopped beforehand\n");
            return EXIT_FAILURE;
        }

        if(camera->allocateBufferPool(config.nBuffers, buffersIds)!= IMG_SUCCESS)
        {
            LOG_ERROR("allocating buffers failed.\n");
            return EXIT_FAILURE;
        }

        missedFrames = 0;

        if(camera->startCapture() !=IMG_SUCCESS)
        {
            LOG_ERROR("starting capture failed.\n");
            return EXIT_FAILURE;
        }

        //
        // pre-enqueue shots to have multi-buffering
        //
        ISPC::ModuleOUT *glb = camera->getModule<ISPC::ModuleOUT>();
        for ( int i = 0 ; i < config.nBuffers-1 ; i++ )
        {
#ifdef INFOTM_ENABLE_WARNING_DEBUG
            debug("enc %d disp %d de %d\n",
                   (int)glb->encoderType, (int)glb->displayType, (int)glb->dataExtractionType);
#endif //INFOTM_ENABLE_WARNING_DEBUG
            if(camera->enqueueShot() !=IMG_SUCCESS)
            {
                LOG_ERROR("pre-enqueuing shot %d/%d failed.\n", i+1, config.nBuffers-1);
                return EXIT_FAILURE;
            }
        }

        if(config.uiSensorInitExposure>0)
        {
            if(camera->getSensor()->setExposure(config.uiSensorInitExposure) != IMG_SUCCESS)
            {
                LOG_WARNING("failed to set sensor to initial exposure of %u us\n", config.uiSensorInitExposure);
            }
        }
        if(config.flSensorInitGain>1.0)
        {
            if (camera->getSensor()->setGain(config.flSensorInitGain) != IMG_SUCCESS)
            {
                LOG_WARNING("failed to set sensor to initial gain of %lf\n", config.flSensorInitGain);
            }
        }

        return EXIT_SUCCESS;
    }

    int stopCapture()
    {
        bool pending = CI_PipelineHasPending(camera->getPipeline()->getCIPipeline());

        // we need to acquire the buffers we already pushed otherwise the HW hangs...
        /// @ investigate that
        //for ( int i = 0 ; i < config.nBuffers-1 ; i++ )
        int i = 0;
        while(pending)
        {
            ISPC::Shot shot;
            if(camera->acquireShot(shot) !=IMG_SUCCESS)
            {
                LOG_ERROR("acquire shot %d/%d before stopping failed.\n", i+1, config.nBuffers-1);
                printInfo();
                return EXIT_FAILURE;
            }
            camera->releaseShot(shot);

            pending = CI_PipelineHasPending(camera->getPipeline()->getCIPipeline());
            i++;
        }
        if ( camera->stopCapture() != IMG_SUCCESS )
        {
            LOG_ERROR("Failed to Stop the capture!\n");
            return EXIT_FAILURE;
        }
        camera->deleteShots();

        std::list<IMG_UINT32>::iterator it;
        for ( it = buffersIds.begin() ; it != buffersIds.end() ; it++ )
        {
            debug("deregister buffer %d\n", *it);
            if ( camera->deregisterBuffer(*it) != IMG_SUCCESS )
            {
                LOG_ERROR("Failed to deregister buffer ID=%d!\n", *it);
                return EXIT_FAILURE;
            }
        }
        buffersIds.clear();

        LOG_INFO("total number of missed frames: %d\n", missedFrames);

        return EXIT_SUCCESS;
    }

    void saveResults(const ISPC::Shot &shot, DemoConfiguration &config)
    {
        std::stringstream outputName;
        bool saved = false;
        static int savedFrames = 0;

        debug("Shot RGB=%s YUV=%s DE=%s HDR=%s TIFF=%s\n",
                FormatString(shot.RGB.pxlFormat),
                FormatString(shot.YUV.pxlFormat),
                FormatString(shot.BAYER.pxlFormat),
                FormatString(shot.HDREXT.pxlFormat),
                FormatString(shot.RAW2DEXT.pxlFormat)
        );

        if ( shot.RGB.pxlFormat != PXL_NONE && config.bSaveRGB )
        {
            outputName.str("");
            outputName << "display"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::RGB, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to saved to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_INFO("saving RGB output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
            config.bSaveRGB=false;
        }

        if ( shot.YUV.pxlFormat != PXL_NONE && config.bSaveYUV )
        {
            ISPC::Global_Setup globalSetup = camera->getPipeline()->getGlobalSetup();
            PIXELTYPE fmt;

            PixelTransformYUV(&fmt, shot.YUV.pxlFormat);

            outputName.str("");
            outputName << "encoder"<<savedFrames
                << "-" << globalSetup.ui32EncWidth << "x" << globalSetup.ui32EncHeight
                << "-" << FormatString(shot.YUV.pxlFormat)
                << "-align" << (unsigned int)fmt.ui8PackedStride
                << ".yuv";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::YUV, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to saved to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_INFO("saving YUV output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
            config.bSaveYUV = false;
        }

        if ( shot.BAYER.pxlFormat != PXL_NONE && config.bSaveDE )
        {
            outputName.str("");
            outputName << "dataExtraction"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::Bayer, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to saved to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_INFO("saving Bayer output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
            config.bSaveDE = false;
        }

        if ( shot.HDREXT.pxlFormat != PXL_NONE && config.bSaveHDR )
        {
            outputName.str("");
            outputName << "hdrExtraction"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::RGB_EXT, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to saved to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_INFO("saving HDR Extraction output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
            config.bSaveHDR = false;
        }

        if ( shot.RAW2DEXT.pxlFormat != PXL_NONE && config.bSaveRaw2D )
        {
            outputName.str("");
            outputName << "raw2DExtraction"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::Bayer_TIFF, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to saved to %s\n",
                    outputName.str().c_str());;
            }
            else
            {
                LOG_INFO("saving RAW 2D Extraction output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
            config.bSaveRaw2D = false;
        }

        if ( shot.ENS.size > 0 )
        {
            outputName.str("");

            outputName << "encoder_stats" << savedFrames << ".dat";

            if ( ISPC::Save::SingleENS(*(camera->getPipeline()), shot, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to save to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_INFO("saving ENS output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
        }

#if 0 //No Save DPF output file
        if ( shot.metadata.defectiveStats.ui32NOutCorrection > 0 )
        {
            outputName.str("");

            outputName << "defective_pixels" << savedFrames << ".dat";

            if ( ISPC::Save::SingleDPF(*(camera->getPipeline()), shot, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to save to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_INFO("saving DPF output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
        }
#endif //INFOTM_ISP
		
        if ( saved )
        {
			#if 0 //No Save FelixSetupArgs.txt file
            // we saved a file so we save the parameters it used too
            ISPC::ParameterList parameters;

            outputName.str("");
            outputName << "FelixSetupArgs_"<<savedFrames<<".txt";

            if ( camera->saveParameters(parameters) == IMG_SUCCESS )
            {
                ISPC::ParameterFileParser::saveGrouped(parameters, outputName.str());
                LOG_INFO("saving parameters to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_WARNING("failed to save parameters\n");
            }

            outputName.str("");
            outputName << "statistics" << savedFrames << ".dat";

            if (ISPC::Save::SingleStats(*(camera->getPipeline()), shot, outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to open statistics %s\n",
                    outputName.str().c_str());
            }
			#endif //0

            savedFrames++;
        }
    }

    void printInfo(const ISPC::Shot &shot, const DemoConfiguration &config, double clockMhz = 40.0) // cannot be const because modifies missedFrames
    {
        static IMG_UINT32 lastTSIn = 0;
        if ( shot.bFrameError )
        {
            printf("=== Frame is erroneous! ===\n");
        }

        if ( shot.iMissedFrames != 0 )
        {
            missedFrames += shot.iMissedFrames;
            printf("=== Missed %d frames before acquiring that one! ===\n",
                shot.iMissedFrames
            );
        }

        printf("TS: LLUp %u - StartIn %u - EndIn %u - StartOut %u - "\
            "EndEncOut %u - EndDispOut %u - Interrupt %u\n",
            shot.metadata.timestamps.ui32LinkedListUpdated,
            shot.metadata.timestamps.ui32StartFrameIn,
            shot.metadata.timestamps.ui32EndFrameIn,
            shot.metadata.timestamps.ui32StartFrameOut,
            shot.metadata.timestamps.ui32EndFrameEncOut,
            shot.metadata.timestamps.ui32EndFrameDispOut,
            shot.metadata.timestamps.ui32InterruptServiced);

        if(lastTSIn!=0)
        {
            printf("Difference from last timestamps is %u tick (FPS ~%.2lf with %.2lfMhz clock)\n",
                shot.metadata.timestamps.ui32StartFrameIn - lastTSIn,
                (clockMhz*1000000.0)/(shot.metadata.timestamps.ui32StartFrameIn - lastTSIn),
                clockMhz);
        }
        lastTSIn = shot.metadata.timestamps.ui32StartFrameIn;

        //
        // control modules information
        //
        const ISPC::ControlAE *pAE = camera->getControlModule<const ISPC::ControlAE>();
        const ISPC::ControlAF *pAF = camera->getControlModule<const ISPC::ControlAF>();
        const ISPC::ControlAWB_PID *pAWB = camera->getControlModule<const ISPC::ControlAWB_PID>();
        const ISPC::ControlTNM *pTNM = camera->getControlModule<const ISPC::ControlTNM>();
        const ISPC::ControlDNS *pDNS = camera->getControlModule<const ISPC::ControlDNS>();
        const ISPC::ControlLBC *pLBC = camera->getControlModule<const ISPC::ControlLBC>();
        const ISPC::ModuleDPF *pDPF = camera->getModule<const ISPC::ModuleDPF>();
        const ISPC::ModuleLSH *pLSH = camera->getModule<const ISPC::ModuleLSH>();

        printf("Sensor: exposure %.4lfms - gain %.2f\n",
            camera->getSensor()->getExposure()/1000.0, camera->getSensor()->getGain()
        );

        if ( pAE )
        {
            printf("AE %d: Metered/target brightness %f/%f (flicker %d)\n",
                pAE->isEnabled(),
                pAE->getCurrentBrightness(),
                pAE->getTargetBrightness(),
                pAE->getFlickerRejection()
            );
        }
        if ( pAF )
        {
            printf("AF %d: state %s current distance %umm\n",
                pAF->isEnabled(),
                ISPC::ControlAF::StateName(pAF->getState()),
                pAF->getBestFocusDistance()
            );
        }
        if ( pAWB )
        {
            printf("AWB_PID %d: mode %s target temp %.0lfK - measured temp %.0lfK - correction temp %.0lfK\n",
                pAWB->isEnabled(),
                ISPC::ControlAWB::CorrectionName(pAWB->getCorrectionMode()),
                pAWB->getTargetTemperature(),
                pAWB->getMeasuredTemperature(),
                pAWB->getCorrectionTemperature()
            );
        }
        if ( pTNM )
        {
            printf("TNMC %d: adaptive %d local %d\n",
                pTNM->isEnabled(),
                pTNM->getAdaptiveTNM(),
                pTNM->getLocalTNM()
            );
        }
        if ( pDNS )
        {
            printf("DNSC %d\n",
                pDNS->isEnabled()
            );
        }
        if ( pLBC )
        {
            printf("LBC %d: correction %.0f measured %.0f\n",
                pLBC->isEnabled(),
                pLBC->getCurrentCorrection().lightLevel,
                pLBC->getLightLevel()
            );
        }
        if ( pDPF )
        {
            printf("DPF: detect %d\n",
                pDPF->bDetect
            );
            if ( config.bEnableDPF )
            {
                const MC_STATS_DPF &dpf = shot.metadata.defectiveStats;

                printf("    fixed=%d written=%d (threshold=%d weight=%f)\n",
                    dpf.ui32FixedPixels, dpf.ui32NOutCorrection,
                    pDPF->ui32Threshold, pDPF->fWeight
                );
            }
        }
        if ( pLSH )
        {
            printf("LSH: matrix %d from '%s'\n",
                pLSH->bEnableMatrix,
                pLSH->lshFilename.c_str()
            );
        }

    }
};

/**
 * @brief Enable mode to allow proper key capture in the application
 */
void enableRawMode(struct termios &orig_term_attr)
{
    struct termios new_term_attr;

    debug("Enable Console RAW mode\n");

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;

    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
}

/**
 * @brief read key
 *
 * Could use fgetc but does not work with some versions of uclibc
 */
char getch(void)
{
    static char line[2];
    if(read(0, line, 1))
    {
        return line[0];
    }
    return -1;
}

/**
 * @brief Disable mode to allow proper key capture in the application
 */
void disableRawMode(struct termios &orig_term_attr)
{
    debug("Disable Console RAW mode\n");
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
}

int run(int argc, char *argv[])
{
    int savedFrames = 0;

    int ret;
    int Idx = 0;
    int ShowIdx = 0;

    //Default configuration, everything disabled but control objects are created
    DemoConfiguration config;

    if ( config.loadParameters(argc, argv) != EXIT_SUCCESS )
    {
        return EXIT_FAILURE;
    }

    LOG_INFO("ISPDDK version: 2.7.0 V2016.11.25-8\n");
    LOG_INFO("Using sensor %s\n", config.pszSensor);
    LOG_INFO("Using config file %s\n", config.pszFelixSetupArgsFile);

    //load parameter file
    ISPC::ParameterFileParser pFileParser;
    ISPC::ParameterList parameters[2];

	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		parameters[Idx] = pFileParser.parseFile(config.pszFelixSetupArgsFile[Idx]);
		if(parameters[Idx].validFlag == false) //Something went wrong parsing the parameters
		{
			LOG_ERROR("parsing parameter file \'%s' failed\n",
				config.pszFelixSetupArgsFile[Idx]);
			return EXIT_FAILURE;
		}
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

    //LoopCamera loopCam(config);
    LoopCamera* pLoopCam[2];
    double clockMhz = 40.0;
    bool loopActive=true;
    bool bSaveOriginals=false; // when saving all saves original formats from config file
    int frameCount = 0;
    double estimatedFPS = 0.0;
    double FPSstartTime = 0.0;
    int neededFPSFrames = 0;


	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		pLoopCam[Idx] = new LoopCamera(config, Idx);
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		if(!pLoopCam[Idx]->camera || pLoopCam[Idx]->camera->state == ISPC::CAM_ERROR)
		{
			LOG_ERROR("Failed when creating camera\n");
			return EXIT_FAILURE;
		}

		if(pLoopCam[Idx]->camera->loadParameters(parameters[Idx]) != IMG_SUCCESS)
		{
			LOG_ERROR("loading pipeline setup parameters[%s] from file: '%s' failed\n", Idx,
				config.pszFelixSetupArgsFile);
			return EXIT_FAILURE;
		}

		if(pLoopCam[Idx]->camera->getConnection())
		{
			clockMhz = (double)pLoopCam[Idx]->camera->getConnection()->sHWInfo.ui32RefClockMhz;
			LOG_INFO("clockMhz: %f\n", clockMhz);
		}
		else
		{
			LOG_ERROR("failed to access camera's connection!\n");
			return EXIT_FAILURE;
		}

		config.loadFormats(parameters[Idx]);

		if(pLoopCam[Idx]->camera->program() != IMG_SUCCESS)
		{
			LOG_ERROR("Failed while programming pipeline.\n");
			return EXIT_FAILURE;
		}
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

	// Create, setup and register control modules
	//ISPC::Sensor *sensor = pLoopCam[Idx]->camera->getSensor();

//	LOG_INFO("Config AE: %d, AWB:%d, TNM: %d, DNS: %d, LBC: %d, AF: %d\n",
//					config.controlAE,
//					config.controlAWB,
//					config.controlTNM,
//					config.controlDNS,
//					config.controlLBC,
//					config.controlAF);

	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		if ( config.controlAE )
		{
			pLoopCam[Idx]->pAE = new ISPC::ControlAE();
			pLoopCam[Idx]->camera->registerControlModule(pLoopCam[Idx]->pAE);
		}
		if ( config.controlAWB )
		{
			pLoopCam[Idx]->pAWB = new ISPC::ControlAWB_PID();
			pLoopCam[Idx]->camera->registerControlModule(pLoopCam[Idx]->pAWB);
		}
		if ( config.controlTNM )
		{
			pLoopCam[Idx]->pTNM = new ISPC::ControlTNM();
			pLoopCam[Idx]->camera->registerControlModule(pLoopCam[Idx]->pTNM);
		}
		if ( config.controlDNS )
		{
			pLoopCam[Idx]->pDNS = new ISPC::ControlDNS();
			pLoopCam[Idx]->camera->registerControlModule(pLoopCam[Idx]->pDNS);
		}
		if ( config.controlLBC )
		{
			pLoopCam[Idx]->pLBC = new ISPC::ControlLBC();
			pLoopCam[Idx]->camera->registerControlModule(pLoopCam[Idx]->pLBC);
		}
		if ( config.controlAF )
		{
			pLoopCam[Idx]->pAF = new ISPC::ControlAF();
			pLoopCam[Idx]->camera->registerControlModule(pLoopCam[Idx]->pAF);
		}

		pLoopCam[Idx]->camera->loadControlParameters(parameters[Idx]); // load all registered controls setup

		// get defaults from the given setup files
		{

			if ( pLoopCam[Idx]->pAE )
			{
				// no need should have been loaded from config file
				//pLoopCam[Idx]->pAE->setTargetBrightness(config.targetBrightness);
				config.targetBrightness = pLoopCam[Idx]->pAE->getTargetBrightness();
				config.flickerRejection = pLoopCam[Idx]->pAE->getFlickerRejection();
			#ifdef INFOTM_ISP
				//use setting file as initial targetBrightness value. 20150819.Fred
				config.oritargetBrightness = pLoopCam[Idx]->pAE->getOriTargetBrightness();
				pLoopCam[Idx]->pAE->setTargetBrightness(config.targetBrightness);
			#endif //INFOTM_ISP
//				LOG_INFO("Config targetBrightness: %f, oritargetBrightness: %f, flickerRejection: %d\n",
//								config.targetBrightness,  config.oritargetBrightness, config.flickerRejection);
			}

			if ( pLoopCam[Idx]->pAWB )
			{
			#ifdef INFOTM_ISP
				if (!config.commandAWB)
				{
					//use setting file as initial AWBAlgorithm value. 20150819.Fred
					config.AWBAlgorithm = pLoopCam[Idx]->pAWB->getCorrectionMode();
				}
			#endif //INFOTM_ISP
				pLoopCam[Idx]->pAWB->setCorrectionMode(config.AWBAlgorithm);
//				LOG_INFO("Config AWBAlgorithm: %d\n", config.AWBAlgorithm);
			}

			ISPC::ModuleLSH *pLSH = pLoopCam[Idx]->camera->getModule<ISPC::ModuleLSH>();
			ISPC::ModuleDPF *pDPF = pLoopCam[Idx]->camera->getModule<ISPC::ModuleDPF>();

			if (pLSH)
			{
				config.bEnableLSH = pLSH->bEnableMatrix;
//				LOG_INFO("Config bEnableLSH: %d\n", config.bEnableLSH);
			}
			if (pDPF)
			{
				config.bEnableDPF = pDPF->bWrite || pDPF->bDetect;
//				LOG_INFO("Config bEnableDPF: %d\n", config.bEnableDPF);
			}

			if (pLoopCam[Idx]->pTNM)
			{
				config.localToneMapping = pLoopCam[Idx]->pTNM->getLocalTNM();
//				LOG_INFO("Config localToneMapping: %d\n", config.localToneMapping);
			}
		}
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

	//--------------------------------------------------------------
	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		FPSstartTime = currTime();
		neededFPSFrames = (int)floor(pLoopCam[Idx]->sensor->flFrameRate*FPS_COUNT_MULT);

		config.applyConfiguration(*(pLoopCam[Idx]->camera));
		config.bResetCapture = false;
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	
	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		// initial setup
		if ( pLoopCam[Idx]->camera->setupModules() != IMG_SUCCESS )
		{
			LOG_ERROR("Failed to re-setup the pipeline!\n");
			return EXIT_FAILURE;
		}

		if( pLoopCam[Idx]->camera->program() != IMG_SUCCESS )
		{
			LOG_ERROR("Failed to re-program the pipeline!\n");
			return EXIT_FAILURE;
		}
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		pLoopCam[Idx]->startCapture();
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

    while(loopActive)   //Capture loop
    {
        if (frameCount%neededFPSFrames == (neededFPSFrames-1))
        {
            double cT = currTime();
            estimatedFPS = (double(neededFPSFrames)/(cT-FPSstartTime));
            FPSstartTime = cT;
        }

        time_t t;
        struct tm tm;
        unsigned long toDelay=0;
        t = time(NULL); // to print info
        tm = *localtime(&t);
		
#ifdef INFOTM_ISP		
        if (frameCount % 1000 == 0)
		{
#endif //INFOTM_ISP
        	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
        	{
				printf("[%d/%d %02d:%02d.%02d] ",
						tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, tm.tm_sec
					  );
				printf("Frame %d - sensor %s: %dx%d @%3.2f (mode %d - mosaic %d) - estimated FPS %3.2f (%d frames avg. with %d buffers)\n",
						frameCount, pLoopCam[Idx]->sensor->pszSensorName,
						pLoopCam[Idx]->sensor->uiWidth, pLoopCam[Idx]->sensor->uiHeight, pLoopCam[Idx]->sensor->flFrameRate, config.sensorMode,
						(int)pLoopCam[Idx]->sensor->eBayerFormat,
						estimatedFPS, neededFPSFrames, config.nBuffers
					  );
        	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
#ifdef INFOTM_ISP
        }
#endif //INFOTM_ISP	


#if 0 // enable to print info about gasket at regular intervals - use same than FPS but could be different
        if (frameCount % neededFPSFrames == (neededFPSFrames-1))
        {
        	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	            pLoopCam[Idx]->printInfo();
        }
#endif

        if ( config.bResetCapture )
        {
            LOG_INFO("re-starting capture to enable alternative output\n");

        	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
        	{
				pLoopCam[Idx]->stopCapture();

				if ( pLoopCam[Idx]->camera->setupModules() != IMG_SUCCESS )
				{
					LOG_ERROR("Failed to re-setup the pipeline!\n");
					return EXIT_FAILURE;
				}

				if( pLoopCam[Idx]->camera->program() != IMG_SUCCESS )
				{
					LOG_ERROR("Failed to re-program the pipeline!\n");
					return EXIT_FAILURE;
				}

				pLoopCam[Idx]->startCapture();
        	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
        }

        if(config.ulEnqueueDelay)
        {

            LOG_INFO("Sleeping %lu\n",config.ulEnqueueDelay);
            usleep(config.ulEnqueueDelay);
        }

        else if(config.ulRandomDelayLow || config.ulRandomDelayHigh)
        {
            toDelay=config.ulRandomDelayLow+(random()%(config.ulRandomDelayHigh-config.ulRandomDelayLow));

            LOG_INFO("Sleeping %lu\n",toDelay);
            usleep(toDelay);
        }
		

    	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
    	{
			if(pLoopCam[Idx]->camera->enqueueShot() !=IMG_SUCCESS)
			{
				LOG_ERROR("Failed to enqueue a shot.\n");
				return EXIT_FAILURE;
			}

			if ( config.bTestStopStart )
			{
				LOG_INFO("Testing stop-start issue\n");

				pLoopCam[Idx]->camera->stopCapture();

				pLoopCam[Idx]->printInfo();

				/*if( pLoopCam[Idx]->camera->program() != IMG_SUCCESS )
				{
					LOG_ERROR("Failed to re-program the pipeline!\n");
					return EXIT_FAILURE;
				}*/

				pLoopCam[Idx]->camera->startCapture();


				for ( int i = 0 ; i < config.nBuffers-1 ; i++ )
				{
					if(pLoopCam[Idx]->camera->enqueueShot() !=IMG_SUCCESS)
					{
						LOG_ERROR("pre-enqueuing shot %d/%d failed.\n", i+1, pLoopCam[Idx]->config.nBuffers-1);
						return EXIT_FAILURE;
					}
				}

				LOG_INFO("X*X*X Stop/start cycle after enqueue, shouldn't hang X*X*X \n");
				config.bTestStopStart = false;
			}
    	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

		//ISPC::Shot shot;
    	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
    	{
			if(pLoopCam[Idx]->camera->acquireShot(pLoopCam[Idx]->shot) != IMG_SUCCESS)
			{
				LOG_ERROR("Failed to retrieve processed shot from pipeline.\n");
				pLoopCam[Idx]->printInfo();
				return EXIT_FAILURE;
			}
    	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

#ifdef ISPC_LOOP_DISPLAY_FB_NAME 
		if ((frameCount % 100) == 0)
		{
			if (ShowIdx == 0)
				ShowIdx = 1;
			else if (ShowIdx == 1)
				ShowIdx = 0;
		}
        if (frameCount == 0)
        {
        	osd0_set(g_fd_osd0, (IMG_UINT32)pLoopCam[ShowIdx]->shot.YUV.phyAddr, pLoopCam[ShowIdx]->sensor->uiWidth, pLoopCam[ShowIdx]->sensor->uiHeight, PFM_NV12);
        }
        osd0_show(g_fd_osd0, (IMG_UINT32)pLoopCam[ShowIdx]->shot.YUV.phyAddr);
        //LOG_INFO("phyAddr[%d] = %p\n", frameCount, (IMG_UINT32)pLoopCam[Idx]->shot.YUV.phyAddr);
#endif //ISPC_LOOP_DISPLAY_FB_NAME

    	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
    	{
		#ifdef INFOTM_ISP
			//Update INFOTM BLC(TNM, R2Y, 3D-denoise)
			if (pLoopCam[Idx]->camera->IsUpdateSelfLBC() != IMG_SUCCESS)
			{
				LOG_INFO("update self LBC fail.\n");
				return EXIT_FAILURE;
			}
		#endif //INFOTM_ISP
    	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)


		//Deal with the keyboard input
		//char read = fgetc(stdin);
		char read = getch();
		switch(read)
		{
			case TEST_GLITCH_KEY:
				// inject glitch in next loop
				config.bTestStopStart = true;
				break;
			case LSH_KEY:
				config.bEnableLSH = !config.bEnableLSH;
			break;

			case DPF_KEY:
				config.bEnableDPF = !config.bEnableDPF;
			break;

			case ADAPTIVETNM_KEY:   //Enable/disable adaptive tone mapping strength control
				config.autoToneMapping = !config.autoToneMapping;
			break;

			case LOCALTNM_KEY:  //Enable/disable local tone mapping
				config.localToneMapping = !config.localToneMapping;
			break;

			case AWB_KEY:
				config.enableAWB = !config.enableAWB;
			break;

			case AWB_RESET:
				// not doing it when AWB is enabled because it forces the CCM and not AWB state
				if ( !config.enableAWB )
				{
					config.targetAWB = 0; // reset
				}
			break;

			case DNSISO_KEY:    //Enable/Disable denoiser automatic ISO control
				config.autoDenoiserISO = !config.autoDenoiserISO;
			break;

			case LBC_KEY:   //Enable/Disable Light based control
				config.autoLBC = !config.autoLBC;
			break;

			case BRIGHTNESSUP_KEY:  //Increase target brightness
				if (config.controlAE)
				{
					config.targetBrightness+=0.05;
				}
				else
				{
					for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
					{
						pLoopCam[Idx]->camera->getSensor()->setExposure(
							pLoopCam[Idx]->camera->getSensor()->getExposure()+100);
					} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				}
			break;

			case BRIGHTNESSDOWN_KEY:    //Decrease target brightness
				if (config.controlAE)
				{
					config.targetBrightness-=0.05;
				}
				else
				{
					for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
					{
						pLoopCam[Idx]->camera->getSensor()->setExposure(
							pLoopCam[Idx]->camera->getSensor()->getExposure()-100);
					} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				}
			break;

			case FOCUSCLOSER_KEY:
				for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				{
					if ( pLoopCam[Idx]->pAF )
						pLoopCam[Idx]->pAF->setCommand(ISPC::ControlAF::AF_FOCUS_CLOSER);
				} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
			break;

			case FOCUSFURTHER_KEY:
				for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				{
					if ( pLoopCam[Idx]->pAF )
						pLoopCam[Idx]->pAF->setCommand(ISPC::ControlAF::AF_FOCUS_FURTHER);
				} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
			break;

			case FOCUSTRIGGER_KEY:
				for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				{
					if ( pLoopCam[Idx]->pAF )
						pLoopCam[Idx]->pAF->setCommand(ISPC::ControlAF::AF_TRIGGER);
				} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
			break;

			case AUTOFLICKER_KEY:   //Enable/disable flicker rejection
				config.flickerRejection = !config.flickerRejection;
			break;

			case SAVE_RGB_KEY:  //Save image to disk
				config.bSaveRGB = true;
				LOG_INFO("save RGB key pressed...\n");
			break;

			case SAVE_YUV_KEY:
				config.bSaveYUV = true;
				LOG_INFO("save YUV key pressed...\n");
				break;

			case SAVE_DE_KEY:
				config.bSaveDE=true;
				LOG_INFO("save DE key pressed...\n");
				break;

			case SAVE_HDR_KEY:
				for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				{
					if ( pLoopCam[Idx]->camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT )
					{
						config.bSaveHDR=true;
						LOG_INFO("save HDR key pressed...\n");
					}
					else
					{
						LOG_WARNING("cannot enable HDR extraction, HW does not support it!\n");
					}
				} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				break;

			case SAVE_RAW2D_KEY:
				for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				{
					if ( pLoopCam[Idx]->camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT )
					{
						config.bSaveRaw2D=true;
						LOG_INFO("save RAW2D key pressed...\n");
					}
					else
					{
						LOG_WARNING("cannot enable RAW 2D extraction, HW does not support it!\n");
					}
				} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				break;

			case SAVE_ORIGINAL_KEY:
				bSaveOriginals=true;
				LOG_INFO("save ORIGINAL key pressed...\n");
				break;

#ifdef ISPC_LOOP_CONTROL_EXPOSURE
			case EXPOSUREUP_KEY:
				for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				{
					unsigned long current_expo;

					current_expo = pLoopCam[Idx]->sensor->getExposure();
					//LOG_INFO("bef current_expo = %lu\n", current_expo);

					current_expo += UP_DOWN_STEP;
					pLoopCam[Idx]->sensor->setExposure(current_expo);
					LOG_INFO("--> set current_expo = %lu\n", current_expo);
				} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				break;

			case EXPOSUREDOWN_KEY:
				for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				{
					unsigned long current_expo;

					current_expo = pLoopCam[Idx]->sensor->getExposure();
					//LOG_INFO("bef current_expo = %lu\n", current_expo);

					current_expo -= UP_DOWN_STEP;
					pLoopCam[Idx]->sensor->setExposure(current_expo);
					LOG_INFO("--> set current_expo = %lu\n", current_expo);
				} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
				break;
#endif //ISPC_LOOP_CONTROL_EXPOSURE

			case EXIT_KEY:  //Exit application
				LOG_INFO("exit key pressed\n");
				loopActive=false;
			break;
		}

		{
			char str[2];
			str[0] = read;
			str[1] = 0;
			int i = atoi(str);
			// not doing it when AWB is enabled because it forces the CCM and not AWB state
			if ( i >= AWB_FIRST_DIGIT && i <= AWB_LAST_DIGIT && !config.enableAWB )
			{
				config.targetAWB = i;
			}
		}

		for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
		{
			bool bSaveResults = config.bSaveRGB||config.bSaveYUV||config.bSaveDE||config.bSaveHDR||config.bSaveRaw2D;

			debug("bSaveResults=%d frameError=%d\n",
				(int)bSaveResults, (int)pLoopCam[Idx]->shot.bFrameError);


			if ( bSaveResults && !pLoopCam[Idx]->shot.bFrameError )
			{
				pLoopCam[Idx]->saveResults(pLoopCam[Idx]->shot, config);
			}

			if(bSaveOriginals)
			{
				config.loadFormats(parameters[Idx], false); // without defaults

				config.bSaveRGB=(config.originalRGB!=PXL_NONE);
				config.bSaveYUV=(config.originalYUV!=PXL_NONE);
				config.bSaveDE=(config.originalBayer!=PXL_NONE);
				if ( pLoopCam[Idx]->camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT )
				{
					config.bSaveHDR=(config.originalHDR!=PXL_NONE);
				}
				if ( pLoopCam[Idx]->camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT )
				{
					config.bSaveRaw2D=(config.originalTiff!=PXL_NONE);
				}
			}
		} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

#ifdef INFOTM_ENABLE_WARNING_DEBUG
    	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
		{
	        pLoopCam[Idx]->printInfo(pLoopCam[Idx]->shot, config, clockMhz);
		} //for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
#endif //INFOTM_ENABLE_WARNING_DEBUG


    	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
    	{
			//Update Camera loop controls configuration
			// before the release because may use the statistics
			config.applyConfiguration(*(pLoopCam[Idx]->camera));

			// done after apply configuration as we already did all changes we needed
			if (bSaveOriginals)
			{
				config.loadFormats(parameters[Idx], true); // reload with defaults
				bSaveOriginals=false;
			}

			if(pLoopCam[Idx]->camera->releaseShot(pLoopCam[Idx]->shot) !=IMG_SUCCESS)
			{
				LOG_ERROR("Failed to release shot.\n");
				return EXIT_FAILURE;
			}
    	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
        frameCount++;
    }

    LOG_INFO("Exiting loop\n");
	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		pLoopCam[Idx]->stopCapture();
	} //End of for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)

    usleep(2000000);
	for (Idx = V2500_MIN_CAM; Idx < V2500_MAX_CAM; Idx++)
	{
		delete pLoopCam[Idx];
		pLoopCam[Idx] = NULL;
	}

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    int ret;
    struct termios orig_term_attr;

#ifdef FELIX_FAKE
    bDisablePdumpRDW = IMG_TRUE; // no read in pdump
    ret = initializeFakeDriver(2, 0, 4096, 256, NULL, NULL);
    if (EXIT_SUCCESS != ret)
    {
        LOG_ERROR("Failed to enable fake driver\n");
        return ret;
    }
#endif

    enableRawMode(orig_term_attr);

#ifdef ISPC_LOOP_DISPLAY_FB_NAME 
    g_fd_osd0 = osd0_open();
#endif //ISPC_LOOP_DISPLAY_FB_NAME	

    LOG_INFO("CHANGELIST %s - DATE %s\n", CI_CHANGELIST_STR, CI_DATE_STR);

    ret = run(argc, argv);

#ifdef ISPC_LOOP_DISPLAY_FB_NAME
    osd0_close(g_fd_osd0);
#endif //ISPC_LOOP_DISPLAY_FB_NAME

    disableRawMode(orig_term_attr);

#ifdef FELIX_FAKE
    ret = finaliseFakeDriver();
    if (EXIT_SUCCESS != ret)
    {
        LOG_ERROR("Failed to disable fake driver\n");
    }
#endif

    return EXIT_SUCCESS;
}
