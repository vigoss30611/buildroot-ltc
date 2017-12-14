/**
******************************************************************************
@file ISPC_loop.cpp

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
#ifdef INFOTM_SW_AWB_METHOD
#include <ispc/ModuleDSC.h>
#endif //INFOTM_SW_AWB_METHOD

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

#define LOG_TAG "ISPC_loop"
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
//#define ISPC_LOOP_LOADING_TEST	3   //for loading test, the number is copy times
//#define ISPC_LOOP_AUTO_SAVE_YUV_FILE
//#define ISPC_LOOP_PRINT_FUNCTION_TIME
#endif //INFOTM_ISP

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
    int sensorMode;
    IMG_BOOL8 sensorFlip[2]; // horizontal and vertical
    unsigned int uiSensorInitExposure; // from parameters and also used when restarting to store current exposure
    // using IMG_FLOAT to be available in DYNCMD otherwise would be double
    IMG_FLOAT flSensorInitGain; // from parameters and also used when restarting to store current gain

    char *pszFelixSetupArgsFile;
    char *pszSensor;
    char *pszInputFLX; // for data generators only
    unsigned int gasket; // for data generator only
    unsigned int aBlanking[2]; // for data generator only

    DemoConfiguration()
    {
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
        sensorMode = 0; //default: 0
        sensorFlip[0] = false; //default: false
        sensorFlip[1] = false; //default: false
        uiSensorInitExposure = 0; //default: 0
        flSensorInitGain = 0.0f; //default: 0.0f

        pszFelixSetupArgsFile = NULL; //default: NULL
        pszSensor = NULL; //default: NULL
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
        if(this->pszSensor)
        {
            IMG_FREE(this->pszSensor);
            this->pszSensor=0;
        }
        if(this->pszFelixSetupArgsFile)
        {
            IMG_FREE(this->pszFelixSetupArgsFile);
            this->pszFelixSetupArgsFile=0;
        }
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

#ifdef INFOTM_SW_AWB_METHOD
	//INFOTM_SW_AWB_METHOD need locate disp buffer(160x90)
	#if defined(INFOTM_SW_AWB_USING_RGB_888_24)
		glb->displayType = RGB_888_24;
	#elif defined(INFOTM_SW_AWB_USING_RGB_888_32)
		glb->displayType = RGB_888_32;
	#else
		glb->displayType = RGB_888_24;
	#endif
#endif //INFOTM_SW_AWB_METHOD
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
    }

    int loadParameters(int argc, char *argv[])
    {
        int ret;
        int missing = 0;
        bool bPrintHelp = false;
        
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

        ret=DYNCMD_RegisterParameter("-setupFile", DYNCMDTYPE_STRING,
            "FelixSetupArgs file to use to configure the Pipeline",
            &(this->pszFelixSetupArgsFile));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -setupFile\n");
            }
            else
            {
                LOG_ERROR("No setup file provided\n");
            }
            missing++;
        }

        std::ostringstream os;
        os.str("");
        os << "Sensor to use for the run. If '" << IIFDG_SENSOR_INFO_NAME;
#ifdef EXT_DATAGEN
        os << "' or '" << EXTDG_SENSOR_INFO_NAME;
#endif
        os << "' is used then the sensor will be considered a data-generator.";
        static std::string sensorHelp = os.str(); // static otherwise object is lost and c_str() points to invalid memory

        ret=DYNCMD_RegisterParameter("-sensor", DYNCMDTYPE_STRING,
            (sensorHelp.c_str()),
            &(this->pszSensor));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -sensor\n");
            }
            else
            {
                LOG_ERROR("no sensor provided\n");
            }
            missing++;
        }

        ret=DYNCMD_RegisterParameter("-sensorMode", DYNCMDTYPE_INT,
            "Sensor mode to use",
            &(this->sensorMode));
        if (RET_FOUND != ret && !bPrintHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("while parsing parameter -modenum\n");
                missing++;
            }
        }

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
    }

};

struct LoopCamera
{
    ISPC::Camera *camera;
    std::list<IMG_UINT32> buffersIds;
    const DemoConfiguration &config;
    unsigned int missedFrames;

    enum IsDataGenerator { NO_DG = 0, INT_DG, EXT_DG };
    enum IsDataGenerator isDatagen;

    LoopCamera(DemoConfiguration &_config): camera(0), config(_config)
    {
        ISPC::Sensor *sensor = 0;
        if(strncmp(config.pszSensor, IIFDG_SENSOR_INFO_NAME, strlen(config.pszSensor))==0)
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

            camera = new ISPC::DGCamera(0, config.pszInputFLX, config.gasket, true);

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

            camera = new ISPC::DGCamera(0, config.pszInputFLX, config.gasket, false);

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
#ifdef INFOTM_ISP
        camera = new ISPC::Camera(0, ISPC::Sensor::GetSensorId(_config.pszSensor), _config.sensorMode, flipping, 0);
#else
        camera = new ISPC::Camera(0, ISPC::Sensor::GetSensorId(_config.pszSensor), _config.sensorMode, flipping);
#endif
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

#if defined(INFOTM_SW_AWB_METHOD)
  #if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)
    int IspGetOverlayFontIndex(char* pChar) {
        int Index = 0xFFFFFFFF;

        if (*pChar >= '0' && *pChar <= '9') {
            Index = *pChar - '0' + 1;
        } else if(*pChar >= 65 && *pChar <= 90) {
            Index = *pChar - 52;
        } else if(*pChar >= 128 && *pChar <= 174) {
            Index = *pChar - 89;
        } else if (*pChar == '/') {
            Index = 11;
        } else if (*pChar == ':') {
            Index = 12;
        } else if (*pChar == '.') {
            Index = 89;
        } else if (*pChar == '`') {
            Index = 90;
        } else if (*pChar == ',') {
            Index = 88;
        } else if (*pChar == 'k') {
            Index = 86;
        } else if (*pChar == 'm') {
            Index = 87;
        } else if (*pChar == 'h') {
            Index = 85;
        } else if (*pChar == 32) {
            Index = 0;
        }

        return Index;
    }

    void IspSetOverlayMsg(void* ImgAddr, int ImgWidth, int ImgStride, char overlayString[], int xOffset, int yOffset) {
        static int flag = 1;
        FILE *fp;
        int i, j;
        int count, Index, startx;
        char* pChar;
        static IMG_UINT8 imgOverlayBuf[176640];
        static IMG_UINT32 pBufSize;

        char* imgBuf = (char*)ImgAddr;
        if (flag) {
            fp = fopen("/etc/ovimg_timestamp.bin", "rb");
            if (NULL != fp) {
                pBufSize = fread(imgOverlayBuf, 1, 176640, fp);
                fclose(fp);
                flag = 0;
            }
        }
        if (0 != flag) {
            return;
        }

        count = 0;
        startx = 100;
        pChar = overlayString;
        while (*pChar != '\0') {
            Index = IspGetOverlayFontIndex(pChar);
            for (i = 0; i < 40; i++) {
                for (j = 0; j < 32; j++) {
                    if (imgOverlayBuf[32*60*Index + i*32 + j] > 200) {
                        *(imgBuf + (i + yOffset) * ImgStride + count * 32 + j + xOffset) = 250; //imgOverlayBuf[32*60 + i*32 + j];
                    }
                }
            }

            count++;
            pChar++;
        }
    }
  #endif

    //2016.04.26 test rgb data.
    //
//#define ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE
//#define ENABLE_SEPARATENESS_R_G_B_WEIGHTING_CALCULATE
//#define ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT
  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
#define IMG_WIDTH                   (160)
#define IMG_HEIGHT                  (90)
#define PIXWL_C_R_ARRAY_SIZE        ((((IMG_WIDTH * 6) + 1) * IMG_HEIGHT) + 2)
#define PIXWL_WEIGHTING_ARRAY_SIZE  ((IMG_WIDTH + 1) * IMG_HEIGHT)
  #endif

    void InfotmSwAWB(ISPC::Shot &shot) {
        static const int SW_AWB_DE_GAMMA[256] =
        {
        	//GAMMA_1.2
        	0,	0,	0,	1,	1,	2,	2,	3,	4,	4,	5,	5,	6,	7,	7,	8,
        	9,	9,	10,	11,	12,	12,	13,	14,	14,	15,	16,	17,	18,	18,	19,	20,
        	21,	21,	22,	23,	24,	25,	25,	26,	27,	28,	29,	30,	30,	31,	32,	33,
        	34,	35,	36,	36,	37,	38,	39,	40,	41,	42,	43,	44,	44,	45,	46,	47,
        	48,	49,	50,	51,	52,	53,	54,	54,	55,	56,	57,	58,	59,	60,	61,	62,
        	63,	64,	65,	66,	67,	68,	69,	70,	71,	72,	73,	74,	75,	76,	76,	77,
        	78,	79,	80,	81,	82,	83,	84,	85,	86,	87,	88,	89,	90,	91,	92,	93,
        	95,	96,	97,	98,	99,	100,	101,	102,	103,	104,	105,	106,	107,	108,	109,	110,
        	111,	112,	113,	114,	115,	116,	117,	118,	119,	120,	122,	123,	124,	125,	126,	127,
        	128,	129,	130,	131,	132,	133,	134,	135,	137,	138,	139,	140,	141,	142,	143,	144,
        	145,	146,	147,	149,	150,	151,	152,	153,	154,	155,	156,	157,	158,	160,	161,	162,
        	163,	164,	165,	166,	167,	169,	170,	171,	172,	173,	174,	175,	176,	178,	179,	180,
        	181,	182,	183,	184,	185,	187,	188,	189,	190,	191,	192,	193,	195,	196,	197,	198,
        	199,	200,	202,	203,	204,	205,	206,	207,	208,	210,	211,	212,	213,	214,	215,	217,
        	218,	219,	220,	221,	222,	224,	225,	226,	227,	228,	230,	231,	232,	233,	234,	235,
        	237,	238,	239,	240,	241,	243,	244,	245,	246,	247,	249,	250,	251,	252,	253,	255
        };

        static const IMG_UINT16 SW_AWB_v[16] = {0, 8, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 14, 10, 6};
        static const float SW_AWB_s[16] = {0.5, 0.44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -0.07, -0.25, -0.25, -0.25};

  #if defined(ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE)
        int w_cnt[5][5] = {
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}
            };

        int cr_filter_cnt[5][5] = {
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}
            };

        int total_filter_cnt[5][5] = {
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}
            };

  #endif
        ISPC::ControlAE* pAE = camera->getControlModule<ISPC::ControlAE>();
        ISPC::ModuleWBC *pWBC = camera->getModule<ISPC::ModuleWBC>();
        ISPC::ControlAWB_PID *pAWB = camera->getControlModule<ISPC::ControlAWB_PID>();
        ISPC::Sensor *sensor = camera->getSensor();
  #if defined(ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE)
        IMG_UINT16 ui16ColSize;
        IMG_UINT16 ui16RowSize;
        IMG_UINT16 ui16ColMultiple;
        IMG_UINT16 ui16RowMultiple;
  #endif
        IMG_UINT16 i = 0;	//width
        IMG_UINT16 j = 0;	//height
        double SUM_grw = 0;
        double SUM_gbw = 0;
        IMG_UINT32 SUM_w = 0;
  #if defined(ENABLE_SEPARATENESS_R_G_B_WEIGHTING_CALCULATE)
        float SUM_rw = 0;
        float SUM_gw = 0;
        float SUM_bw = 0;
  #endif
        IMG_UINT16 interval = 8;	// 4/32=0.125
        int drop_pixel = 0;
        bool RGB_filter;
        int R, G, B;
        double gr, gb;
        int c, r;
        float IW;
        IMG_UINT32 CW, W;
        int iLuma, k, d;
        IMG_UINT8 *pRGBData;

  #if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)
        int rGain1, rGain2, bGain1, bGain2, DropRatio1, DropRatio2;
        IMG_UINT8 * virAddr = 0;
        int valid_width, valid_stride;
        char szString[128];
  #endif
  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
        IMG_UINT8 byPixelCR[PIXWL_C_R_ARRAY_SIZE];
        IMG_UINT8 byPixelWeighting[PIXWL_WEIGHTING_ARRAY_SIZE];
        int iPixelCRIdx;
        int iPixelWeightingIdx;
  #endif

        //SW AWB
        //BGR...

  #if defined(ENABLE_SW_AWB_DEBUG_MSG)
        //printf("@@@ loop\n");
        //printf("pWBC->aWBGain[0] r=%f\n", pWBC->aWBGain[0]);	//r
        //printf("pWBC->aWBGain[1] g=%f\n", pWBC->aWBGain[1]);	//g
        //printf("pWBC->aWBGain[2] g=%f\n", pWBC->aWBGain[2]);	//g
        //printf("pWBC->aWBGain[3] b=%f\n", pWBC->aWBGain[3]);	//b

        printf("pAWB->dbImg_rgain=%f\n", pAWB->dbImg_rgain);
        printf("pAWB->dbImg_bgain=%f\n", pAWB->dbImg_bgain);
  #endif

  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
        iPixelCRIdx = 0;
        iPixelWeightingIdx = 0;
  #endif
  #if defined(ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE)
        if (160 == shot.RGB.width) {
            ui16ColSize = 4;
        } else if (120  == shot.RGB.width) {
            ui16ColSize = 4;
        } else {
            ui16ColSize = 4;
        }
        if (90 == shot.RGB.height) {
            ui16RowSize = 3;
        } else if (68  == shot.RGB.height) {
            ui16RowSize = 4;
        } else {
            ui16RowSize = 3;
        }
        ui16ColMultiple = (shot.RGB.width + ui16ColSize - 1) / ui16ColSize;
        ui16RowMultiple = (shot.RGB.height + ui16RowSize - 1) / ui16RowSize;
  #endif

        //160 * 90 or 120 x 68
        for (j=0; j<shot.RGB.height; j++) {
            pRGBData = (IMG_UINT8 *)shot.RGB.data + (shot.RGB.stride * j);
            for (i=0; i<shot.RGB.width; i++) {
                B = *pRGBData++;
                G = *pRGBData++;
                R = *pRGBData++;
  #if defined(INFOTM_SW_AWB_USING_RGB_888_32)
				pRGBData++;
  #endif

  #if defined(ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE)
                int a = i / ui16ColMultiple;	// 0 ~ 3
                int b = j / ui16RowMultiple;	// 0 ~ 2
  #endif

                //01.De GAMMA
                if ((R < 10 || R > 240) ||
                    (G < 10 || G > 240) ||
                    (B < 10 || B > 240)) {
                    RGB_filter = false;
                } else {
                    RGB_filter = true;
                    R = SW_AWB_DE_GAMMA[R];
                    G = SW_AWB_DE_GAMMA[G];
                    B = SW_AWB_DE_GAMMA[B];
                }

                //02.De r, b gain
                if (R!=0 && pWBC->aWBGain[0]!=0) {
                    gr = (float)G / (float)R;
                    gr *= pWBC->aWBGain[0];
                } else {
                    gr = 0;
                }

                if (B!=0 && pWBC->aWBGain[3]!=0) {
                    gb = (float)G / (float)B;
                    gb *= pWBC->aWBGain[3];
                } else {
                    gb = 0;
                }

                //03.gr, gb interval
                c = (int)(gr * interval);
                r = (int)(gb * interval);
  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
                if ((99 - 1) < c) {
                    byPixelCR[iPixelCRIdx++] = '9';
                    byPixelCR[iPixelCRIdx++] = '9';
                } else {
                    byPixelCR[iPixelCRIdx++] = (int)(c / 10) + 0x30;
                    byPixelCR[iPixelCRIdx++] = (int)(c % 10) + 0x30;
                }
                byPixelCR[iPixelCRIdx++] = '/';
                if ((99 - 1) < r) {
                    byPixelCR[iPixelCRIdx++] = '9';
                    byPixelCR[iPixelCRIdx++] = '9';
                } else {
                    byPixelCR[iPixelCRIdx++] = (int)(r / 10) + 0x30;
                    byPixelCR[iPixelCRIdx++] = (int)(r % 10) + 0x30;
                }
                byPixelCR[iPixelCRIdx++] = ' ';
                if ((IMG_WIDTH - 1) == i) {
                    byPixelCR[iPixelCRIdx++] = '\n';
                }
  #endif

                if (0 <= c && interval*4 > c &&
                    0 <= r && interval*4 > r &&
                    RGB_filter) {
                    CW = pAWB->uiHwSwAwbWeightingTable[r][c];	//weighting table
  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
                    byPixelWeighting[iPixelWeightingIdx++] = CW + 0x30;
  #endif
                    if (CW) {
                        //float dxq = interval * gr - (float)c;
                        //float dyq = interval * gb - (float)r;
  #if 1
                        iLuma = (R * 5 + G * 9 + B * 2) / 16;
                        k = iLuma / 16;
                        d = iLuma - (k * 16);
                        IW = SW_AWB_s[k] * (float)d + (float)SW_AWB_v[k];

                        W = (IMG_UINT32)((CW * IW) / 16);
  #else
                        W = CW;
  #endif

                        SUM_grw += (gr * (double)W);
                        SUM_gbw += (gb * (double)W);

                        SUM_w += W;
  #if defined(ENABLE_SEPARATENESS_R_G_B_WEIGHTING_CALCULATE)
                        SUM_rw += (R * W);
                        SUM_gw += (G * W);
                        SUM_bw += (B * W);
  #endif
                    } else {
                        W = 0;
                    }
  #if defined(ENABLE_SW_AWB_DEBUG_MSG)
                    if (i == 80 && j == 45) {
                        //printf("@@@\n");
                        printf("R=%d, G=%d, B=%d\n", R, G, B);
                        printf("gr=%f, gb=%f\n", gr, gb);
                        printf("c=%d, r=%d\n", c, r);

                        //printf("dxq=%f, dyq=%f\n", dxq, dyq);
                        //printf("CW=%d\n", CW);

                        //printf("l=%d, k=%d, d=%d\n", l, k, d);
                        //printf("s[%d]=%f, v[%d]=%d\n", k, s[k], k, v[k]);

                        //printf("IW=%f\n", IW);
                        printf("W=%d\n", W);
                    }
  #endif

  #if defined(ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE)
                    if (0 == W && b < ui16RowSize && a < ui16ColSize) {
                        w_cnt[b][a]++;
                        total_filter_cnt[b][a]++;
                        drop_pixel++;
                    }
  #else
                    if (0 == W) {
                        drop_pixel++;
                    }
  #endif
                } else {
  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
                    byPixelWeighting[iPixelWeightingIdx++] = ' ';
  #endif
  #if defined(ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE)
                    total_filter_cnt[b][a]++;
                    cr_filter_cnt[b][a]++;
                    drop_pixel++;
  #else
                    drop_pixel++;
  #endif

  #if defined(ENABLE_SW_AWB_DEBUG_MSG)
                    if (i == 80 && j == 45) {
                        //printf("@@@\n");
                        printf("R=%d, G=%d, B=%d\n", R, G, B);
                        printf("gr=%f, gb=%f\n", gr, gb);
                        printf("c=%d, r=%d\n", c, r);

                        //printf("dxq=%f, dyq=%f\n", dxq, dyq);
                        //printf("CW=%d\n", CW);

                        //printf("l=%d, k=%d, d=%d\n", l, k, d);
                        //printf("s[%d]=%f, v[%d]=%d\n", k, s[k], k, v[k]);

                        //printf("IW=%f\n", IW);
                        //printf("W=%d\n", SW_AWB_WEIGHT_TABLE[r][c]);
                    }
  #endif
                }
  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
                if ((IMG_WIDTH - 1) == i) {
                    byPixelWeighting[iPixelWeightingIdx++] = '\n';
                }
  #endif
            }
        }
  #if defined(ENABLE_SW_AWB_DEBUG_MSG)
        //printf("SUM_grw=%f\n", SUM_grw);
        //printf("SUM_gbw=%f\n", SUM_gbw);
        //printf("SUM_w=%d\n", SUM_w);
  #endif

        //04. calculate and apply r, b gain
        //default set 6500 r b gain.
        double sw_r_gain = pWBC->aWBGain_def[0];
        double sw_b_gain = pWBC->aWBGain_def[3];
        static double sw_r_gain_reserve = pWBC->aWBGain_def[0];
        static double sw_b_gain_reserve = pWBC->aWBGain_def[3];
        double drop_ratio = 0;

        drop_ratio = (double)drop_pixel / (double)(shot.RGB.width*shot.RGB.height);

  #if defined(ENABLE_SW_AWB_DEBUG_MSG)
        //printf("drop_pixel=%d\n", drop_pixel);
        printf("drop_ratio=%03f\n", drop_ratio);
  #endif
  #if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)
        virAddr = const_cast<IMG_UINT8 *>(shot.YUV.data);
        valid_width = shot.YUV.width;
        valid_stride = shot.YUV.stride;
  #endif

        //if drop_ration large than pAWB->dbHwSwAwbDropRatio, approch to the last awb gain(or img awb value).
        if (drop_ratio < pAWB->dbHwSwAwbDropRatio) {
            sw_r_gain = SUM_grw / SUM_w;
            sw_b_gain = SUM_gbw / SUM_w;
            sw_r_gain_reserve = sw_r_gain;
            sw_b_gain_reserve = sw_b_gain;
  #if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)

            DropRatio1 = (int)drop_ratio;
            DropRatio2 = (int)((drop_ratio - DropRatio1) * 1000000);
            rGain1 = (int)sw_r_gain;
            rGain2 = (int)((sw_r_gain - rGain1) * 1000000);
            bGain1 = (int)sw_b_gain;
            bGain2 = (int)((sw_b_gain - bGain1) * 1000000);
            sprintf(szString, "DROP:%d.%06d R:%d.%06d B:%d.%06d", DropRatio1, DropRatio2, rGain1, rGain2, bGain1, bGain2);
      #if defined(ZT201_PROJECT)
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 550);
      #else
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 700);
      #endif

  #endif

            sw_r_gain = pWBC->aWBGain[0]*(1-pAWB->dbHwSwAwbUpdateSpeed) + sw_r_gain*pAWB->dbHwSwAwbUpdateSpeed;
            sw_b_gain = pWBC->aWBGain[3]*(1-pAWB->dbHwSwAwbUpdateSpeed) + sw_b_gain*pAWB->dbHwSwAwbUpdateSpeed;
        } else {
  #if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)

            DropRatio1 = (int)drop_ratio;
            DropRatio2 = (int)((drop_ratio - DropRatio1) * 1000000);
            rGain1 = (int)sw_r_gain;
            rGain2 = (int)((sw_r_gain - rGain1) * 1000000);
            bGain1 = (int)sw_b_gain;
            bGain2 = (int)((sw_b_gain - bGain1) * 1000000);
            sprintf(szString, "DROP:%d.%06d R:%d.%06d B:%d.%06d", DropRatio1, DropRatio2, rGain1, rGain2, bGain1, bGain2);
      #if defined(ZT201_PROJECT)
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 550);
      #else
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 700);
      #endif

  #endif
            //Use last r,b gain as target.
            //sw_r_gain = pWBC->aWBGain[0]*(1-pAWB->dbHwSwAwbUpdateSpeed) + sw_r_gain_reserve*pAWB->dbHwSwAwbUpdateSpeed;
            //sw_b_gain = pWBC->aWBGain[3]*(1-pAWB->dbHwSwAwbUpdateSpeed) + sw_b_gain_reserve*pAWB->dbHwSwAwbUpdateSpeed;

            //Use last imagination r,b gain as target.
            sw_r_gain = pWBC->aWBGain[0]*(1-pAWB->dbHwSwAwbUpdateSpeed) + pAWB->dbImg_rgain*pAWB->dbHwSwAwbUpdateSpeed;
            sw_b_gain = pWBC->aWBGain[3]*(1-pAWB->dbHwSwAwbUpdateSpeed) + pAWB->dbImg_bgain*pAWB->dbHwSwAwbUpdateSpeed;

            //Use defalut(normally D65 gain) r,b gain as target.
            //sw_r_gain = pWBC->aWBGain[0]*(1-pAWB->dbHwSwAwbUpdateSpeed) + pWBC->aWBGain_def[0]*pAWB->dbHwSwAwbUpdateSpeed;
            //sw_b_gain = pWBC->aWBGain[3]*(1-pAWB->dbHwSwAwbUpdateSpeed) + pWBC->aWBGain_def[3]*pAWB->dbHwSwAwbUpdateSpeed;
        }

        pAWB->sw_awb_gain_set(sw_r_gain, sw_b_gain);

  #if defined(ENABLE_SW_AWB_DEBUG_MSG)
        printf("sw_r_gain=%f\n", sw_r_gain);
        printf("sw_b_gain=%f\n", sw_b_gain);
  #endif
  #if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)

        rGain1 = (int)sw_r_gain;
        rGain2 = (int)((sw_r_gain - rGain1) * 1000000);
        bGain1 = (int)sw_b_gain;
        bGain2 = (int)((sw_b_gain - bGain1) * 1000000);
        sprintf(szString, "              R:%d.%06d B:%d.%06d", rGain1, rGain2, bGain1, bGain2);
      #if defined(ZT201_PROJECT)
        IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 580);
      #else
        IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 730);
      #endif

  #endif
        if (pAE && sensor && pAWB) {
            ISPC::ColorCorrection tmpCCM;
            double dbBrightness, dbGain, dbOverExpos, dbUnderExpos;
            double dbTmp1, dbTmp2, dbTmp3, dbTmp4;
            int iTmp11, iTmp12, iTmp21, iTmp22, iTmp31, iTmp32, iTmp41, iTmp42;

            dbBrightness = pAE->getCurrentBrightness();
            dbGain = sensor->getGain();
            dbOverExpos = pAE->getRatioOverExp();
            dbUnderExpos = pAE->getRatioUnderExp();
            if (true == pAWB->bUseOriginalCCM) {
                if ((pAWB->dbSwAwbUseStdCcmDarkGainLmt < dbGain) ||
                    ((pAWB->dbSwAwbUseStdCcmLowLuxGainLmt < dbGain) && ((pAWB->dbSwAwbUseStdCcmLowLuxBrightnessLmt > dbBrightness) || (pAWB->dbSwAwbUseStdCcmLowLuxUnderExposeLmt < dbUnderExpos))) ||
                    ((pAWB->dbSwAwbUseStdCcmDarkBrightnessLmt > dbBrightness) && (pAWB->dbSwAwbUseStdCcmDarkUnderExposeLmt < dbUnderExpos))
                   ) {
                    pAWB->bUseOriginalCCM = false;
                }
            } else {
                if (((pAWB->dbSwAwbUseOriCcmNrmlGainLmt > dbGain) &&
                     (pAWB->dbSwAwbUseOriCcmNrmlBrightnessLmt < dbBrightness) &&
                     (pAWB->dbSwAwbUseOriCcmNrmlUnderExposeLmt > dbUnderExpos))
                   ) {
                    pAWB->bUseOriginalCCM = true;
                }
            }
  #if defined(ENABLE_SW_AWB_DBG_MSG_OVERLAY_TO_IMG)
            if (0.0f > dbBrightness) {
                dbBrightness = -dbBrightness;

                iTmp11 = (int)dbBrightness;
                iTmp12 = (int)((dbBrightness - iTmp11) * 1000000);
                iTmp21 = (int)dbGain;
                iTmp22 = (int)((dbGain - iTmp21) * 1000000);
                sprintf(szString, "CUR_B:N%d.%06d GAIN:%d.%06d", iTmp11, iTmp12, iTmp21, iTmp22);
      #if defined(ZT201_PROJECT)
                IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 610);
      #else
                IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 760);
      #endif

            } else {

                iTmp11 = (int)dbBrightness;
                iTmp12 = (int)((dbBrightness - iTmp11) * 1000000);
                iTmp21 = (int)dbGain;
                iTmp22 = (int)((dbGain - iTmp21) * 1000000);
                sprintf(szString, "CUR_B: %d.%06d GAIN:%d.%06d", iTmp11, iTmp12, iTmp21, iTmp22);
      #if defined(ZT201_PROJECT)
                IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 610);
      #else
                IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 760);
      #endif

            }

            iTmp31 = (int)dbOverExpos;
            iTmp32 = (int)((dbOverExpos - iTmp31) * 1000000);
            iTmp41 = (int)dbUnderExpos;
            iTmp42 = (int)((dbUnderExpos - iTmp41) * 1000000);
            sprintf(szString, "%s OVER:%d.%06d, UNDER:%d.%06d", ((true == pAWB->bUseOriginalCCM) ? ("ORI") : ("STD")), iTmp31, iTmp32, iTmp41, iTmp42);
      #if defined(ZT201_PROJECT)
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 640);
      #else
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 790);
      #endif

    #if 0
            tmpCCM = pAWB->getCurrentCCM();
            dbTmp1 = tmpCCM.coefficients[0][0];
            if (0.0f > dbTmp1) {
                dbTmp1 = -dbTmp1;
            }
            iTmp11 = (int)dbTmp1;
            iTmp12 = (int)((dbTmp1 - iTmp11) * 1000000);
            dbTmp2 = tmpCCM.coefficients[0][1];
            if (0.0f > dbTmp2) {
                dbTmp2 = -dbTmp2;
            }
            iTmp21 = (int)dbTmp2;
            iTmp22 = (int)((dbTmp2 - iTmp21) * 1000000);
            dbTmp3 = tmpCCM.coefficients[0][2];
            if (0.0f > dbTmp3) {
                dbTmp3 = -dbTmp3;
            }
            iTmp31 = (int)dbTmp3;
            iTmp32 = (int)((dbTmp3 - iTmp31) * 1000000);
            dbTmp4 = tmpCCM.offsets[0][0];
            if (0.0f > dbTmp4) {
                dbTmp4 = -dbTmp4;
            }
            iTmp41 = (int)dbTmp4;
            iTmp42 = (int)((dbTmp4 - iTmp41) * 1000000);
            sprintf(szString, "%c%d.%06d  %c%d.%06d  %c%d.%06d  %c%d.%06d",
                ((0.0f < tmpCCM.coefficients[0][0]) ? (' ') : ('N')), iTmp11, iTmp12,
                ((0.0f < tmpCCM.coefficients[0][1]) ? (' ') : ('N')), iTmp21, iTmp22,
                ((0.0f < tmpCCM.coefficients[0][2]) ? (' ') : ('N')), iTmp31, iTmp32,
                ((0.0f < tmpCCM.offsets[0][0]) ? (' ') : ('N')), iTmp41, iTmp42
                );
      #if defined(ZT201_PROJECT)
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 700);
      #else
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 300, 850);
      #endif
            tmpCCM = pAWB->getCurrentCCM();
            dbTmp1 = tmpCCM.coefficients[1][0];
            if (0.0f > dbTmp1) {
                dbTmp1 = -dbTmp1;
            }
            iTmp11 = (int)dbTmp1;
            iTmp12 = (int)((dbTmp1 - iTmp11) * 1000000);
            dbTmp2 = tmpCCM.coefficients[1][1];
            if (0.0f > dbTmp2) {
                dbTmp2 = -dbTmp2;
            }
            iTmp21 = (int)dbTmp2;
            iTmp22 = (int)((dbTmp2 - iTmp21) * 1000000);
            dbTmp3 = tmpCCM.coefficients[1][2];
            if (0.0f > dbTmp3) {
                dbTmp3 = -dbTmp3;
            }
            iTmp31 = (int)dbTmp3;
            iTmp32 = (int)((dbTmp3 - iTmp31) * 1000000);
            dbTmp4 = tmpCCM.offsets[0][1];
            if (0.0f > dbTmp4) {
                dbTmp4 = -dbTmp4;
            }
            iTmp41 = (int)dbTmp4;
            iTmp42 = (int)((dbTmp4 - iTmp41) * 1000000);
            sprintf(szString, "%c%d.%06d  %c%d.%06d  %c%d.%06d  %c%d.%06d",
                ((0.0f < tmpCCM.coefficients[1][0]) ? (' ') : ('N')), iTmp11, iTmp12,
                ((0.0f < tmpCCM.coefficients[1][1]) ? (' ') : ('N')), iTmp21, iTmp22,
                ((0.0f < tmpCCM.coefficients[1][2]) ? (' ') : ('N')), iTmp31, iTmp32,
                ((0.0f < tmpCCM.offsets[0][1]) ? (' ') : ('N')), iTmp41, iTmp42
                );
      #if defined(ZT201_PROJECT)
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 730);
      #else
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 300, 880);
      #endif
            tmpCCM = pAWB->getCurrentCCM();
            dbTmp1 = tmpCCM.coefficients[2][0];
            if (0.0f > dbTmp1) {
                dbTmp1 = -dbTmp1;
            }
            iTmp11 = (int)dbTmp1;
            iTmp12 = (int)((dbTmp1 - iTmp11) * 1000000);
            dbTmp2 = tmpCCM.coefficients[2][1];
            if (0.0f > dbTmp2) {
                dbTmp2 = -dbTmp2;
            }
            iTmp21 = (int)dbTmp2;
            iTmp22 = (int)((dbTmp2 - iTmp21) * 1000000);
            dbTmp3 = tmpCCM.coefficients[2][2];
            if (0.0f > dbTmp3) {
                dbTmp3 = -dbTmp3;
            }
            iTmp31 = (int)dbTmp3;
            iTmp32 = (int)((dbTmp3 - iTmp31) * 1000000);
            dbTmp4 = tmpCCM.offsets[0][2];
            if (0.0f > dbTmp4) {
                dbTmp4 = -dbTmp4;
            }
            iTmp41 = (int)dbTmp4;
            iTmp42 = (int)((dbTmp4 - iTmp41) * 1000000);
            sprintf(szString, "%c%d.%06d  %c%d.%06d  %c%d.%06d  %c%d.%06d",
                ((0.0f < tmpCCM.coefficients[2][0]) ? (' ') : ('N')), iTmp11, iTmp12,
                ((0.0f < tmpCCM.coefficients[2][1]) ? (' ') : ('N')), iTmp21, iTmp22,
                ((0.0f < tmpCCM.coefficients[2][2]) ? (' ') : ('N')), iTmp31, iTmp32,
                ((0.0f < tmpCCM.offsets[0][2]) ? (' ') : ('N')), iTmp41, iTmp42
                );
      #if defined(ZT201_PROJECT)
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 760);
      #else
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 300, 910);
      #endif
            tmpCCM = pAWB->getCurrentCCM();
            dbTmp1 = tmpCCM.gains[0][0];
            if (0.0f > dbTmp1) {
                dbTmp1 = -dbTmp1;
            }
            iTmp11 = (int)dbTmp1;
            iTmp12 = (int)((dbTmp1 - iTmp11) * 1000000);
            dbTmp2 = tmpCCM.gains[0][1];
            if (0.0f > dbTmp2) {
                dbTmp2 = -dbTmp2;
            }
            iTmp21 = (int)dbTmp2;
            iTmp22 = (int)((dbTmp2 - iTmp21) * 1000000);
            dbTmp3 = tmpCCM.gains[0][2];
            if (0.0f > dbTmp3) {
                dbTmp3 = -dbTmp3;
            }
            iTmp31 = (int)dbTmp3;
            iTmp32 = (int)((dbTmp3 - iTmp31) * 1000000);
            dbTmp4 = tmpCCM.gains[0][3];
            if (0.0f > dbTmp4) {
                dbTmp4 = -dbTmp4;
            }
            iTmp41 = (int)dbTmp4;
            iTmp42 = (int)((dbTmp4 - iTmp41) * 1000000);
            sprintf(szString, "%c%d.%06d %c%d.%06d %c%d.%06d %c%d.%06d",
                ((0.0f < tmpCCM.gains[0][0]) ? (' ') : ('N')), iTmp11, iTmp12,
                ((0.0f < tmpCCM.gains[0][1]) ? (' ') : ('N')), iTmp21, iTmp22,
                ((0.0f < tmpCCM.gains[0][2]) ? (' ') : ('N')), iTmp31, iTmp32,
                ((0.0f < tmpCCM.gains[0][3]) ? (' ') : ('N')), iTmp41, iTmp42
                );
      #if defined(ZT201_PROJECT)
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 400, 790);
      #else
            IspSetOverlayMsg(virAddr, valid_width, valid_stride, szString, 300, 940);
      #endif
    #endif
  #endif
        }

  #if defined(ENABLE_SW_AWB_DEBUG_MSG) && defined(ENABLE_SW_AWB_REGION_WEIGHTING_AND_DROP_RATIO_CALCULATE)
        //printf("w_filter cnt_\n");
        //printf("[%d] [%d] [%d] [%d]\n", w_cnt[0][0], w_cnt[0][1], w_cnt[0][2], w_cnt[0][3]);
        //printf("[%d] [%d] [%d] [%d]\n", w_cnt[1][0], w_cnt[1][1], w_cnt[1][2], w_cnt[1][3]);
        //printf("[%d] [%d] [%d] [%d]\n", w_cnt[2][0], w_cnt[2][1], w_cnt[2][2], w_cnt[2][3]);

        //printf("cr_filter cnt_\n");
        //printf("[%d] [%d] [%d] [%d]\n", cr_filter_cnt[0][0], cr_filter_cnt[0][1], cr_filter_cnt[0][2], cr_filter_cnt[0][3]);
        //printf("[%d] [%d] [%d] [%d]\n", cr_filter_cnt[1][0], cr_filter_cnt[1][1], cr_filter_cnt[1][2], cr_filter_cnt[1][3]);
        //printf("[%d] [%d] [%d] [%d]\n", cr_filter_cnt[2][0], cr_filter_cnt[2][1], cr_filter_cnt[2][2], cr_filter_cnt[2][3]);

        printf("total_filter cnt_\n");
        printf("[%d] [%d] [%d] [%d]\n", total_filter_cnt[0][0], total_filter_cnt[0][1], total_filter_cnt[0][2], total_filter_cnt[0][3]);
        printf("[%d] [%d] [%d] [%d]\n", total_filter_cnt[1][0], total_filter_cnt[1][1], total_filter_cnt[1][2], total_filter_cnt[1][3]);
        printf("[%d] [%d] [%d] [%d]\n\n", total_filter_cnt[2][0], total_filter_cnt[2][1], total_filter_cnt[2][2], total_filter_cnt[2][3]);
  #endif
  #if defined(ENABLE_SW_AWB_PIXEL_C_R_WEIGHTING_OUTPUT)
        if (0 == (frameCount % 150)) {
            struct timeval time;
            struct tm tm;
            char filename[256];
            FILE *fp = NULL;

            if (access("/mnt/mmc/PixelWeight",0) == -1)
                mkdir("/mnt/mmc/PixelWeight", 0777);

            gettimeofday(&time, NULL);
            tm = *localtime(&time.tv_sec);
            sprintf(filename, "/mnt/mmc/PixelWeight/PixelWeight_%04d%02d%02d_%02d%02d%02d",
                    tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
                    tm.tm_hour, tm.tm_min, tm.tm_sec
                );
            fp = fopen(filename, "wb");
            if (fp) {
                byPixelCR[iPixelCRIdx++] = '\n';
                byPixelCR[iPixelCRIdx++] = '\n';
                fwrite(byPixelCR, sizeof(unsigned char), iPixelCRIdx, fp);
                fwrite(byPixelWeighting, sizeof(unsigned char), iPixelWeightingIdx, fp);
                fclose(fp);
            }
        }
  #endif
	}
#endif //#if defined(INFOTM_SW_AWB_METHOD)

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

#ifndef ISPC_LOOP_AUTO_SAVE_YUV_FILE //??? for test
            outputName << "encoder"<<savedFrames
#else
            int cnt = savedFrames % 500;
            outputName << "/mnt/mmc/encoder" << cnt
#endif //ISPC_LOOP_AUTO_SAVE_YUV_FILE
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

    //Default configuration, everything disabled but control objects are created
    DemoConfiguration config;

#ifdef INFOTM_ISP_RETRY
retry_loop:
#endif //INFOTM_ISP_RETRY
    if ( config.loadParameters(argc, argv) != EXIT_SUCCESS )
    {
        return EXIT_FAILURE;
    }

    printf("ISPDDK version: 2.7.0\n");
    printf("Using sensor %s\n", config.pszSensor);
    printf("Using config file %s\n", config.pszFelixSetupArgsFile);

    //load parameter file
    ISPC::ParameterFileParser pFileParser;
    ISPC::ParameterList parameters = pFileParser.parseFile(config.pszFelixSetupArgsFile);
    if(parameters.validFlag == false) //Something went wrong parsing the parameters
    {
        LOG_ERROR("parsing parameter file \'%s' failed\n",
            config.pszFelixSetupArgsFile);
        return EXIT_FAILURE;
    }

    LoopCamera loopCam(config);
    double clockMhz = 40.0;

    if(!loopCam.camera || loopCam.camera->state == ISPC::CAM_ERROR)
    {
        LOG_ERROR("Failed when creating camera\n");
        return EXIT_FAILURE;
    }

    if(loopCam.camera->loadParameters(parameters) != IMG_SUCCESS)
    {
        LOG_ERROR("loading pipeline setup parameters from file: '%s' failed\n",
            config.pszFelixSetupArgsFile);
        return EXIT_FAILURE;
    }

    if(loopCam.camera->getConnection())
    {
        clockMhz = (double)loopCam.camera->getConnection()->sHWInfo.ui32RefClockMhz;
    }
    else
    {
        LOG_ERROR("failed to access camera's connection!\n");
        return EXIT_FAILURE;
    }

    config.loadFormats(parameters);

    if(loopCam.camera->program() != IMG_SUCCESS)
    {
        LOG_ERROR("Failed while programming pipeline.\n");
        return EXIT_FAILURE;
    }

    // Create, setup and register control modules
    ISPC::ControlAE *pAE = 0;
    ISPC::ControlAWB_PID *pAWB = 0;
    ISPC::ControlTNM *pTNM = 0;
    ISPC::ControlDNS *pDNS = 0;
    ISPC::ControlLBC *pLBC = 0;
    ISPC::ControlAF *pAF = 0;
    ISPC::Sensor *sensor = loopCam.camera->getSensor();

#ifdef INFOTM_DISABLE_AE
    config.controlAE = false;
#endif //INFOTM_ENABLE_AE

#ifdef INFOTM_DISABLE_AWB
    config.controlAWB = false;
    config.enableAWB = false;
#endif //INFOTM_ENABLE_AWB

#ifdef INFOTM_DISABLE_AF
    config.controlAF = false;
#endif //INFOTM_ENABLE_AF

    if ( config.controlAE )
    {
        pAE = new ISPC::ControlAE();
        loopCam.camera->registerControlModule(pAE);
    }
    if ( config.controlAWB )
    {
        pAWB = new ISPC::ControlAWB_PID();
        loopCam.camera->registerControlModule(pAWB);
    }
    if ( config.controlTNM )
    {
        pTNM = new ISPC::ControlTNM();
        loopCam.camera->registerControlModule(pTNM);
    }
    if ( config.controlDNS )
    {
        pDNS = new ISPC::ControlDNS();
        loopCam.camera->registerControlModule(pDNS);
    }
    if ( config.controlLBC )
    {
        pLBC = new ISPC::ControlLBC();
        loopCam.camera->registerControlModule(pLBC);
    }
    if ( config.controlAF )
    {
        pAF = new ISPC::ControlAF();
        loopCam.camera->registerControlModule(pAF);
    }

    loopCam.camera->loadControlParameters(parameters); // load all registered controls setup

    // get defaults from the given setup files
    {

        if ( pAE )
        {
            // no need should have been loaded from config file
            //pAE->setTargetBrightness(config.targetBrightness);
            config.targetBrightness = pAE->getTargetBrightness();
            config.flickerRejection = pAE->getFlickerRejection();
#ifdef INFOTM_ISP
			//use setting file as initial targetBrightness value. 20150819.Fred
			config.oritargetBrightness = pAE->getOriTargetBrightness();
	        pAE->setTargetBrightness(config.targetBrightness);
#endif //INFOTM_ISP			
        }

        if ( pAWB )
        {
#ifdef INFOTM_ISP	
			if (!config.commandAWB)
			{
				//use setting file as initial AWBAlgorithm value. 20150819.Fred
				config.AWBAlgorithm = pAWB->getCorrectionMode();
			}
#endif //INFOTM_ISP		
            pAWB->setCorrectionMode(config.AWBAlgorithm);
        }

        ISPC::ModuleLSH *pLSH = loopCam.camera->getModule<ISPC::ModuleLSH>();
        ISPC::ModuleDPF *pDPF = loopCam.camera->getModule<ISPC::ModuleDPF>();

        if (pLSH)
        {
            config.bEnableLSH = pLSH->bEnableMatrix;
        }
        if (pDPF)
        {
            config.bEnableDPF = pDPF->bWrite || pDPF->bDetect;
        }

        if (pTNM)
        {
            config.localToneMapping = pTNM->getLocalTNM();
        }
    }

    //--------------------------------------------------------------

    bool loopActive=true;
    bool bSaveOriginals=false; // when saving all saves original formats from config file

    int frameCount = 0;
    double estimatedFPS = 0.0;

    double FPSstartTime = currTime();
    int neededFPSFrames = (int)floor(sensor->flFrameRate*FPS_COUNT_MULT);

    config.applyConfiguration(*(loopCam.camera));
    config.bResetCapture = false;
	
#if defined(INFOTM_ENABLE_GPIO_TOGGLE_FRAME_COUNT)
    // Initial GPIO 147 for the frame rate check.
    static int index=0;
	system ("echo 147 > /sys/class/gpio/export");
	system ("echo out > /sys/class/gpio/gpio147/direction");
#endif	

    // initial setup
    if ( loopCam.camera->setupModules() != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to re-setup the pipeline!\n");
        return EXIT_FAILURE;
    }

#ifdef INFOTM_SW_AWB_METHOD
    ISPC::ModuleDSC *pDSC = loopCam.camera->getModule<ISPC::ModuleDSC>();
    if (pDSC) {
        loopCam.camera->setDisplayDimensions(pDSC->aRect[2], pDSC->aRect[3]);
        printf("INFOTM_SW_AWB_METHOD set display buffer to %dx%d.\n", pDSC->aRect[2], pDSC->aRect[3]);
    } else {
        //loopCam.camera->setDisplayDimensions(160, 90);
        //printf("INFOTM_SW_AWB_METHOD need set disp buffer to 160x90.\n");
        printf("===== WARNING : INFOTM_SW_AWB_METHOD has to set display buffer to small size!!! =====");
    }
#endif //INFOTM_SW_AWB_METHOD

    if( loopCam.camera->program() != IMG_SUCCESS )
    {
        LOG_ERROR("Failed to re-program the pipeline!\n");
        return EXIT_FAILURE;
    }

    loopCam.startCapture();
    //
#ifdef ISPC_LOOP_LOADING_TEST
    IMG_UINT32 YuvSize = 1920*1088 + 1920*(1088/2);
    printf("YuvSize=%d\n", YuvSize);
    IMG_UINT32 Y, YuvCount = ISPC_LOOP_LOADING_TEST;
    IMG_UINT8* pYuvImage[YuvCount];
    for (Y=0; Y<YuvCount; Y++)
    {
    	pYuvImage[Y] = (IMG_UINT8*)IMG_MALLOC(YuvSize);
    }
#endif //ISPC_LOOP_LOADING_TEST

#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
    double dTloop, dTenqueue, dTacquire, dTdisplay, dTsave, dTapplyConf, dTrelease;
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME

    while(loopActive)   //Capture loop
    {
#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
    	dTloop = currTime();
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME
        if (frameCount%neededFPSFrames == (neededFPSFrames-1))
        {
            double cT = currTime();
            estimatedFPS = (double(neededFPSFrames)/(cT-FPSstartTime));
            FPSstartTime = cT;
        }

#if defined(INFOTM_ENABLE_GPIO_TOGGLE_FRAME_COUNT)
		// Set this GPIO pin as high to count frame rate.
		system ("echo 1 > /sys/class/gpio/gpio147/value");
#endif

        time_t t;
        struct tm tm;
        unsigned long toDelay=0;
        t = time(NULL); // to print info
        tm = *localtime(&t);
		
#ifdef INFOTM_ISP		
        if (frameCount % 200 == 0) 
		{
#endif //INFOTM_ISP

            printf("[%d/%d %02d:%02d.%02d] ",
                    tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, tm.tm_sec
                  );
            printf("Frame %d - sensor %dx%d @%3.2f (mode %d - mosaic %d) - estimated FPS %3.2f (%d frames avg. with %d buffers)\n",
                    frameCount,
                    sensor->uiWidth, sensor->uiHeight, sensor->flFrameRate, config.sensorMode,
                    (int)sensor->eBayerFormat,
                    estimatedFPS, neededFPSFrames, config.nBuffers
                  );

#ifdef INFOTM_ISP
        }
#endif //INFOTM_ISP	


#if 1 // enable to print info about gasket at regular intervals - use same than FPS but could be different
        if(frameCount%neededFPSFrames == (neededFPSFrames-1))
        {
            loopCam.printInfo();
        }
#endif

        if ( config.bResetCapture )
        {
            LOG_INFO("re-starting capture to enable alternative output\n");

            loopCam.stopCapture();

            if ( loopCam.camera->setupModules() != IMG_SUCCESS )
            {
                LOG_ERROR("Failed to re-setup the pipeline!\n");
                return EXIT_FAILURE;
            }

            if( loopCam.camera->program() != IMG_SUCCESS )
            {
                LOG_ERROR("Failed to re-program the pipeline!\n");
                return EXIT_FAILURE;
            }

#ifdef INFOTM_ISP_RETRY
            if (loopCam.startCapture() != IMG_SUCCESS)
            {
                printf("******* retry: startCapture() *******\n");
                loopCam.camera->control.clearModules();
                if (pAE)  	delete pAE;
                if (pAWB)  	delete pAWB;
                if (pTNM)  	delete pTNM;
                if (pDNS)  	delete pDNS;
                if (pLBC)  	delete pLBC;
                if (pAF)   	delete pAF;
                goto retry_loop;
            }
#else
            loopCam.startCapture();
#endif //INFOTM_ISP_RETRY
        }

        if(config.ulEnqueueDelay)
        {

            printf("Sleeping %lu\n",config.ulEnqueueDelay);
            usleep(config.ulEnqueueDelay);
        }

        else if(config.ulRandomDelayLow || config.ulRandomDelayHigh)
        {
            toDelay=config.ulRandomDelayLow+(random()%(config.ulRandomDelayHigh-config.ulRandomDelayLow));

            printf("Sleeping %lu\n",toDelay);
            usleep(toDelay);
        }

#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTenqueue = currTime();
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME
        if(loopCam.camera->enqueueShot() !=IMG_SUCCESS)
        {
            LOG_ERROR("Failed to enqueue a shot.\n");
#ifdef INFOTM_ISP_RETRY
            printf("******* retry: enqueueShot() *******\n");
            loopCam.camera->control.clearModules();
            if (pAE)  	delete pAE;
            if (pAWB)  	delete pAWB;
            if (pTNM)  	delete pTNM;
            if (pDNS)  	delete pDNS;
            if (pLBC)  	delete pLBC;
            if (pAF)   	delete pAF;
            goto retry_loop;
#else
            return EXIT_FAILURE;
#endif //INFOTM_ISP_RETRY

        }
#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTenqueue = (currTime() - dTenqueue) * 1000;
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME

        if ( config.bTestStopStart )
        {
            LOG_INFO("Testing stop-start issue\n");

            loopCam.camera->stopCapture();

            loopCam.printInfo();

            /*if( loopCam.camera->program() != IMG_SUCCESS )
            {
                LOG_ERROR("Failed to re-program the pipeline!\n");
                return EXIT_FAILURE;
            }*/

            loopCam.camera->startCapture();


            for ( int i = 0 ; i < config.nBuffers-1 ; i++ )
            {
                if(loopCam.camera->enqueueShot() !=IMG_SUCCESS)
                {
                    LOG_ERROR("pre-enqueuing shot %d/%d failed.\n", i+1, loopCam.config.nBuffers-1);
                    return EXIT_FAILURE;
                }
            }

            printf("X*X*X Stop/start cycle after enqueue, shouldn't hang X*X*X \n");
            config.bTestStopStart = false;
        }


#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTacquire = currTime();
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME
        ISPC::Shot shot;
        if(loopCam.camera->acquireShot(shot) != IMG_SUCCESS)
        {
            LOG_ERROR("Failed to retrieve processed shot from pipeline.\n");
            loopCam.printInfo();
#ifdef INFOTM_ISP_RETRY
            printf("******* retry: acquireShot() *******\n");
            loopCam.camera->control.clearModules();
            if (pAE)  	delete pAE;
            if (pAWB)  	delete pAWB;
            if (pTNM)  	delete pTNM;
            if (pDNS)  	delete pDNS;
            if (pLBC)  	delete pLBC;
            if (pAF)   	delete pAF;
            goto retry_loop;
#else
            return EXIT_FAILURE;
#endif //INFOTM_ISP_RETRY			
        }
#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTacquire = (currTime() - dTacquire) * 1000;
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME

#ifdef INFOTM_SW_AWB_METHOD
		loopCam.InfotmSwAWB(shot);
#endif //INFOTM_SW_AWB_METHOD

#ifdef ISPC_LOOP_LOADING_TEST
        for (Y=0; Y<YuvCount; Y++)
        {
            if (pYuvImage[Y])
            {
            	IMG_MEMCPY(pYuvImage[Y], shot.YUV.data, YuvSize);
            }
        }
#endif //ISPC_LOOP_LOADING_TEST

//        printf("TS: LLUp %u - StartIn %u - EndIn %u - StartOut %u - "\
//            "EndEncOut %u - EndDispOut %u - Interrupt %u\n",
//            shot.metadata.timestamps.ui32LinkedListUpdated,
//            shot.metadata.timestamps.ui32StartFrameIn,
//            shot.metadata.timestamps.ui32EndFrameIn,
//            shot.metadata.timestamps.ui32StartFrameOut,
//            shot.metadata.timestamps.ui32EndFrameEncOut,
//            shot.metadata.timestamps.ui32EndFrameDispOut,
//            shot.metadata.timestamps.ui32InterruptServiced);

#ifdef INFOTM_ISP
		//Update INFOTM BLC(TNM, R2Y, 3D-denoise)
		if (loopCam.camera->IsUpdateSelfLBC() != IMG_SUCCESS)
		{
			printf("update self LBC fail.\n");
			return EXIT_FAILURE;
		}
		
//        if (frameCount % 200 == 0)
//        {
//            printf("shot.YUV.phy:0x%x, shot.YUV.data:0x%x, stride:%d, height:%d\n", shot.YUV.phyAddr, shot.YUV.data, shot.YUV.stride, shot.YUV.height);
//        }
#endif //INFOTM_ISP

#if defined(INFOTM_ENABLE_GPIO_TOGGLE_FRAME_COUNT)
		// Set this GPIO pin as low to count frame rate.
		system ("echo 0 > /sys/class/gpio/gpio147/value");
#endif

#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTdisplay = currTime();
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME
#ifdef ISPC_LOOP_DISPLAY_FB_NAME 
        int fbfd = open(ISPC_LOOP_DISPLAY_FB_NAME, O_RDWR);
        struct fb_var_screeninfo vinfo;
        ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
#endif //ISPC_LOOP_DISPLAY_FB_NAME


#ifdef ISPC_LOOP_DISPLAY_FB_NAME

        ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo);
        close(fbfd);
#endif //ISPC_LOOP_DISPLAY_FB_NAME
#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTdisplay = (currTime() - dTdisplay) * 1000;
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME

        //Deal with the keyboard input
        //char read = fgetc(stdin);
        char read = getch();

#ifdef ISPC_LOOP_AUTO_SAVE_YUV_FILE
		if (read != EXIT_KEY)
		{
			if (frameCount > 5)
			{
				read = SAVE_YUV_KEY;// ??? for test
			}
		}
#endif //ISPC_LOOP_AUTO_SAVE_YUV_FILE

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
                    loopCam.camera->getSensor()->setExposure(
                        loopCam.camera->getSensor()->getExposure()+100);
                }
            break;

            case BRIGHTNESSDOWN_KEY:    //Decrease target brightness
                if (config.controlAE)
                {
                    config.targetBrightness-=0.05;
                }
                else
                {
                    loopCam.camera->getSensor()->setExposure(
                        loopCam.camera->getSensor()->getExposure()-100);
                }
            break;

            case FOCUSCLOSER_KEY:
                if ( pAF )
                    pAF->setCommand(ISPC::ControlAF::AF_FOCUS_CLOSER);
            break;

            case FOCUSFURTHER_KEY:
                if ( pAF )
                    pAF->setCommand(ISPC::ControlAF::AF_FOCUS_FURTHER);
            break;

            case FOCUSTRIGGER_KEY:
                if ( pAF )
                    pAF->setCommand(ISPC::ControlAF::AF_TRIGGER);
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
                if ( loopCam.camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT )
                {
                    config.bSaveHDR=true;
                    LOG_INFO("save HDR key pressed...\n");
                }
                else
                {
                    LOG_WARNING("cannot enable HDR extraction, HW does not support it!\n");
                }
                break;

            case SAVE_RAW2D_KEY:
                if ( loopCam.camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT )
                {
                    config.bSaveRaw2D=true;
                    LOG_INFO("save RAW2D key pressed...\n");
                }
                else
                {
                    LOG_WARNING("cannot enable RAW 2D extraction, HW does not support it!\n");
                }
                break;

            case SAVE_ORIGINAL_KEY:
                bSaveOriginals=true;
                LOG_INFO("save ORIGINAL key pressed...\n");
                break;

#ifdef ISPC_LOOP_CONTROL_EXPOSURE
			case EXPOSUREUP_KEY:
				{
					unsigned long current_expo;
					
					current_expo = sensor->getExposure();
					//printf("bef current_expo = %lu\n", current_expo);

					current_expo += UP_DOWN_STEP;
					sensor->setExposure(current_expo);				
					printf("--> set current_expo = %lu\n", current_expo);
				}
				break;

			case EXPOSUREDOWN_KEY:
				{
					unsigned long current_expo;
					
					current_expo = sensor->getExposure();
					//printf("bef current_expo = %lu\n", current_expo);

					current_expo -= UP_DOWN_STEP;
					sensor->setExposure(current_expo);				
					printf("--> set current_expo = %lu\n", current_expo);
				}
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

        bool bSaveResults = config.bSaveRGB||config.bSaveYUV||config.bSaveDE||config.bSaveHDR||config.bSaveRaw2D;

        debug("bSaveResults=%d frameError=%d\n",
            (int)bSaveResults, (int)shot.bFrameError);

#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTsave = currTime();
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME
        if ( bSaveResults && !shot.bFrameError )
        {
            loopCam.saveResults(shot, config);
        }
#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTsave = (currTime() - dTsave) * 1000;
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME

        if(bSaveOriginals)
        {
            config.loadFormats(parameters, false); // without defaults

            config.bSaveRGB=(config.originalRGB!=PXL_NONE);
            config.bSaveYUV=(config.originalYUV!=PXL_NONE);
            config.bSaveDE=(config.originalBayer!=PXL_NONE);
            if ( loopCam.camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT )
            {
                config.bSaveHDR=(config.originalHDR!=PXL_NONE);
            }
            if ( loopCam.camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT )
            {
                config.bSaveRaw2D=(config.originalTiff!=PXL_NONE);
            }

        }

#ifdef INFOTM_ENABLE_WARNING_DEBUG
        loopCam.printInfo(shot, config, clockMhz);
#endif //INFOTM_ENABLE_WARNING_DEBUG

#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTapplyConf = currTime();
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME
        //Update Camera loop controls configuration
        // before the release because may use the statistics
        config.applyConfiguration(*(loopCam.camera));
#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTapplyConf = (currTime() - dTapplyConf) * 1000;
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME

        // done after apply configuration as we already did all changes we needed
        if (bSaveOriginals)
        {
            config.loadFormats(parameters, true); // reload with defaults
            bSaveOriginals=false;
        }

#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTrelease = currTime();
#endif //ISPC_LOOP_PRINT_FUNCTION_TIME
       if(loopCam.camera->releaseShot(shot) !=IMG_SUCCESS)
        {
            LOG_ERROR("Failed to release shot.\n");
            return EXIT_FAILURE;
        }
#ifdef ISPC_LOOP_PRINT_FUNCTION_TIME
        dTrelease = (currTime() - dTrelease) * 1000;

        dTloop = (currTime() - dTloop) * 1000;
        if (frameCount % 200 == 0)
        {
        	printf("while(loopActive)-loop=%f, enqueueShot()=%f, acquireShot()=%f, displayFB()=%f, saveResults()=%f, applyConfiguration()=%f, releaseShot()=%f\n", dTloop, dTenqueue, dTacquire, dTdisplay, dTsave, dTapplyConf, dTrelease);
        }
#endif //INFOTM_ENABLE_TICKET_98469_DEBUG
        frameCount++;
    }

#ifdef ISPC_LOOP_LOADING_TEST
    for (Y=0; Y<YuvCount; Y++)
    {
        if (pYuvImage[Y])
        	IMG_FREE(pYuvImage[Y]);
        pYuvImage[Y] = NULL;
    }
#endif //ISPC_LOOP_LOADING_TEST

    LOG_INFO("Exiting loop\n");
    loopCam.stopCapture();

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
    int fbfd = open(ISPC_LOOP_DISPLAY_FB_NAME, O_RDWR);
    struct fb_var_screeninfo vinfo; 
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    vinfo.nonstd = 2;
    //vinfo.xres = 1920; //the dss driver has already hack the res to 1920x1088;
    //vinfo.yres = 1088;
    ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo);
    close(fbfd);
#endif //ISPC_LOOP_DISPLAY_FB_NAME	

    LOG_INFO("CHANGELIST %s - DATE %s\n", CI_CHANGELIST_STR, CI_DATE_STR);

    ret = run(argc, argv);

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
