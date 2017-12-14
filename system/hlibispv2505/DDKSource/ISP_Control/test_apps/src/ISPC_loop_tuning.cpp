/**
******************************************************************************
@file ISPC_loop_tuning.cpp

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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#endif //INFOTM_ISP


#include <ispc/Pipeline.h>
#include <ci/ci_api.h>
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

#include <sensorapi/sensorapi.h> // to get sensor modes

#ifdef EXT_DATAGEN
#include <sensors/dgsensor.h>
#endif
#include <sensors/iifdatagen.h>


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

// auto white balance controls
#define AWB_RESET '0'
#define AWB_FIRST_DIGIT 1
#define AWB_LAST_DIGIT 9
// from 1 to 9 used to select one CCM

#ifdef INFOTM_ISP
//for off line IQ tuning.
#define CONTROL_EXPOSURE_BY_KEY
#ifdef CONTROL_EXPOSURE_BY_KEY
#define EXPOSURE_UP_KEY '9'
#define EXPOSURE_DOWN_KEY '3'
#define AUTO_CAPTURE_KEY 'c'

#define UP_DOWN_STEP	100
#endif //CONTROL_EXPOSURE_BY_KEY
#endif //INFOTM_ISP

#define EXIT_KEY 'x'

#define FPS_COUNT_MULT 1.5 ///< @brief used to compute the number of frames to use for the FPS count (sensor FPS*multiplier) so that the time is more than a second

//#define debug printf
#define debug
//#define MAINSCREEN_FB_NAME "/dev/fb0"

#define IQ_TUNING

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
    printf("%c/%c Target brightness up/down\n", BRIGHTNESSUP_KEY, BRIGHTNESSDOWN_KEY);
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
    printf("%c Exit\n", EXIT_KEY);

    printf("\n\nAvailable sensors in sensor API:\n");

    const char **pszSensors = Sensor_ListAll();
    int s = 0;
    while(pszSensors[s])
    {
        int strn = strlen(pszSensors[s]);
        SENSOR_HANDLE hSensorHandle = NULL;

        printf("\t%d: %s \n", s, pszSensors[s]);
#ifdef INFOTM_ISP
        if ( Sensor_Initialise(s, &hSensorHandle, 0) == IMG_SUCCESS )
#else
        if ( Sensor_Initialise(s, &hSensorHandle) == IMG_SUCCESS )
#endif
        {
            SENSOR_MODE mode;
            int m = 0;
            while ( Sensor_GetMode(hSensorHandle, m, &mode) == IMG_SUCCESS )
            {
                printf("\t\tmode %d: %5dx%5d @%.2f (vTot=%d) %s\n",
                    m, mode.ui16Width, mode.ui16Height, mode.flFrameRate, mode.ui16VerticalTotal, 
                    mode.ui8SupportFlipping==SENSOR_FLIP_NONE?"flipping=none":
                    mode.ui8SupportFlipping==SENSOR_FLIP_BOTH?"flipping=horizontal|vertical":
                    mode.ui8SupportFlipping==SENSOR_FLIP_HORIZONTAL?"flipping=horizontal":"flipping=vertical");
                m++;
            }
        }
        else
        {
            printf("\tfailed to init sensor API - no modes display availble\n");
        }

        s++;
    }
}

/**
 * @brief Represents the configuration for demo configurable parameters in running time
 */
struct DemoConfiguration
{
    // Auto Exposure
    bool controlAE;             ///< @brief Create a control AE object or not
    double targetBrightness;    ///< @brief target value for the ControlAE brightness
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
	bool commandAWB;

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
#ifdef IQ_TUNING
        // AE
        controlAE = false;
        targetBrightness = 0.0f;
        flickerRejection = false;

        // TNM
        controlTNM = false;
        localToneMapping = false;
        autoToneMapping = false;

        // DNS
        controlDNS = false;
        autoDenoiserISO = false;

        // LBC
        controlLBC = false;
        autoLBC = false;

        // AF
        controlAF = false;

        // AWB
        controlAWB = false;
        enableAWB = false;
        AWBAlgorithm = ISPC::ControlAWB::WB_COMBINED;
        targetAWB = -1;
		commandAWB = false;

        // savable
        bSaveRGB = false;
        bSaveYUV = false;
        bSaveDE = false;
        bSaveHDR = false;
        bSaveRaw2D = false;

        bEnableDPF = false;
        bEnableLSH = false;
#else
		// AE
		controlAE = true;
		targetBrightness = -0.20f;
		flickerRejection = true;

		// TNM
		controlTNM = true;
		localToneMapping = true;
		autoToneMapping = true;

		// DNS
		controlDNS = false;
		autoDenoiserISO = false;

		// LBC
		controlLBC = true;
		autoLBC = true;

		// AF
		controlAF = false;

		// AWB
		controlAWB = true;
		enableAWB = true;
		AWBAlgorithm = ISPC::ControlAWB::WB_COMBINED;
		targetAWB = 0;
		commandAWB = false;

		// savable
		bSaveRGB = false;
		bSaveYUV = false;
		bSaveDE = false;
		bSaveHDR = false;
		bSaveRaw2D = false;

		bEnableDPF = true;
		bEnableLSH = false;
#endif

        bResetCapture = false;

        originalYUV = PXL_NONE;
        originalRGB = PXL_NONE;
        originalBayer = PXL_NONE;
        originalDEPoint = CI_INOUT_NONE;
        originalHDR = PXL_NONE;
        originalTiff = PXL_NONE;

        // other information
        nBuffers = 2;
        sensorMode = 0;
        sensorFlip[0] = false;
        sensorFlip[1] = false;
        uiSensorInitExposure = 0;
        flSensorInitGain = 0.0f;

        pszFelixSetupArgsFile = NULL;
        pszSensor = NULL;
        pszInputFLX = NULL; // for data generators only
        gasket = 0; // for data generator only
        aBlanking[0] = FELIX_MIN_H_BLANKING; // for data generator only
        aBlanking[1] = FELIX_MIN_V_BLANKING;
    }

    // generateDefault if format is PXL_NONE
    void loadFormats(const ISPC::ParameterList &parameters, bool generateDefault=true)
    {
        originalYUV = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::ENCODER_TYPE));
        debug("loaded YVU=%s\n", FormatString(originalYUV));
        if ( generateDefault && originalYUV == PXL_NONE )
            originalYUV = YUV_420_PL12_8;

        originalRGB = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DISPLAY_TYPE));
        debug("loaded RGB=%s (forced)\n", FormatString(RGB_888_32));
        if ( generateDefault && originalRGB == PXL_NONE )
            originalRGB = RGB_888_32;


        originalBayer = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_TYPE));
        debug("loaded DE=%s\n", FormatString(originalBayer));
        if ( generateDefault && originalBayer == PXL_NONE )
            originalBayer = BAYER_RGGB_12;

        originalDEPoint = (CI_INOUT_POINTS)(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_POINT)-1);
        debug("loaded DE point=%d\n", (int)originalDEPoint);
        if ( generateDefault && originalDEPoint != CI_INOUT_BLACK_LEVEL && originalDEPoint != CI_INOUT_FILTER_LINESTORE )
            originalDEPoint = CI_INOUT_BLACK_LEVEL;

        originalHDR = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::HDREXTRA_TYPE));
        debug("loaded HDR=%s (forced)\n", FormatString(BGR_101010_32));
        if ( generateDefault && originalHDR == PXL_NONE )
            originalHDR = BGR_101010_32;


        originalTiff = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::RAW2DEXTRA_TYPE));
        debug("loaded TIFF=%s\n", FormatString(originalTiff));
        if ( generateDefault && originalTiff == PXL_NONE )
            originalTiff = BAYER_TIFF_12;
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
#if defined(ENABLE_AE_DEBUG_MSG)
        	printf("felix::applyConfiguration targetBrightness = %6.3f\n", this->targetBrightness);
#endif
            pAE->setTargetBrightness(this->targetBrightness);

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
#if defined(ENABLE_AWB_DEBUG_MSG)
                printf("targetAWB = %d\n", this->targetAWB);
#endif
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
            glb->displayType = this->originalRGB;
        }
        else
        {
            glb->dataExtractionType = PXL_NONE;
            glb->dataExtractionPoint = CI_INOUT_NONE;
            glb->displayType = PXL_NONE;
        }
#if defined(IQ_TUNING)
        //for off line IQ tuning.
        glb->dataExtractionType = BAYER_RGGB_10;
        glb->dataExtractionPoint = CI_INOUT_BLACK_LEVEL;
        //glb->dataExtractionPoint = CI_INOUT_FILTER_LINESTORE;
#endif
        glb->displayType = PXL_NONE;//this->originalRGB;
		
        this->bResetCapture |= glb->dataExtractionType != prev;
        debug("Configure DE output %s at %d (prev=%s)\n", FormatString(glb->dataExtractionType), (int)glb->dataExtractionPoint, FormatString(prev));
        debug("Configure RGB output %s (prev=%s)\n", FormatString(glb->displayType), FormatString(prev==PXL_NONE?RGB_888_32:PXL_NONE));

        prev = glb->hdrExtractionType;
        if ( this->bSaveHDR ) // assumes the possibility has been checked
        {
            glb->hdrExtractionType = this->originalHDR;
            this->bResetCapture = true;
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
            this->bResetCapture = true;
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
#if defined(ENABLE_LSH_DEBUG_MSG)
                if ( pLSH->bEnableMatrix )
                {
                    printf("LSH matrix from %s\n", pLSH->lshFilename.c_str());
                }
#endif
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
        }
    }

    int loadParameters(int argc, char *argv[])
    {
        int ret;
        int missing = 0;

        //command line parameter loading:
        ret = DYNCMD_AddCommandLine(argc, argv, "-source");
        if (IMG_SUCCESS != ret)
        {
            fprintf(stderr, "ERROR unable to parse command line\n");
            DYNCMD_ReleaseParameters();
            return EXIT_FAILURE;
        }

        ret=DYNCMD_RegisterParameter("-setupFile", DYNCMDTYPE_STRING, 
                "FelixSetupArgs file to use to configure the Pipeline", 
                &(this->pszFelixSetupArgsFile));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                fprintf(stderr, "Error while parsing parameter -setupFile\n");
            }
            else
            {
                fprintf(stderr, "Error: No setup file provided\n");
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
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                fprintf(stderr, "Error while parsing parameter -sensor\n");
            }
            else
            {
                fprintf(stderr, "Error: no sensor provided\n");
            }
            missing++;
        }

        ret=DYNCMD_RegisterParameter("-sensorMode", DYNCMDTYPE_INT, 
                "Sensor mode to use",
                &(this->sensorMode));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -modenum\n");
            }
        }

        ret=DYNCMD_RegisterParameter("-sensorExposure", DYNCMDTYPE_UINT, 
                "[optional] initial exposure to use in us", 
                &(this->uiSensorInitExposure));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret) // if not found use default
            {
                fprintf(stderr, "Error while parsing parameter -sensorExposure\n");
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
                fprintf(stderr, "Error while parsing parameter -sensorGain\n");
                missing++;
            }
            // parameter is optional
        }

        ret=DYNCMD_RegisterArray("-sensorFlip", DYNCMDTYPE_BOOL8, 
                "[optional] Set 1 to flip the sensor Horizontally and Vertically. If only 1 option is given it is assumed to be the horizontal one\n",
                this->sensorFlip, 2);
        if (RET_FOUND != ret)
        {
            if (RET_INCOMPLETE == ret)
            {
                fprintf(stderr, "Warning: incomplete -sensorFlip, flipping is H=%d V=%d\n", 
                        (int)sensorFlip[0], (int)sensorFlip[1]);
            }
            else if (RET_NOT_FOUND != ret)
            {
                fprintf(stderr, "Error: while parsing parameter -sensorFlip\n");
                missing++;
            }
        }

        int algo = 0;
        ret=DYNCMD_RegisterParameter("-chooseAWB", DYNCMDTYPE_UINT, 
                "Enable usage of White Balance Control algorithm and loading of the multi-ccm parameters - values are mapped ISPC::Correction_Types (1=AC, 2=WP, 3=HLW, 4=COMBINED)", 
                &(algo));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret) // if not found use default
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -chooseAWB\n");
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
                    missing++;
                    fprintf(stderr, "Unsuported AWB algorithm %d given\n", algo);
            }
			this->commandAWB = true;
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
        DYNCMD_RegisterParameter("-disableLBC", DYNCMDTYPE_COMMAND, 
                "Disable construction of the Light Base Control object", 
                NULL);
        if(RET_FOUND == ret)
        {
            this->controlLBC = false;
        }

        ret=DYNCMD_RegisterParameter("-nBuffers", DYNCMDTYPE_UINT, 
                "Number of buffers to run with",
                &(this->nBuffers));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -nBuffers\n");
            }
        }

        ret=DYNCMD_RegisterParameter("-DG_inputFLX", DYNCMDTYPE_STRING, 
                "FLX file to use as an input of the data-generator (only if specified sensor is a data generator)",
                &(this->pszInputFLX));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -DG_inputFLX\n");
            }
        }

        ret=DYNCMD_RegisterParameter("-DG_gasket", DYNCMDTYPE_UINT, 
                "Gasket to use for data-generator (only if specified sensor is a data generator)", 
                &(this->gasket));
        if (RET_FOUND != ret)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -DG_gasket\n");
            }
        }

        ret=DYNCMD_RegisterArray("-DG_blanking", DYNCMDTYPE_UINT,
                "Horizontal and vertical blanking in pixels (only if specified sensor is data generator)", 
                &(this->aBlanking), 2);
        if (RET_FOUND != ret)
        {
            if (RET_INCOMPLETE == ret)
            {
                fprintf(stderr, "-DG_blanking incomplete, second value for blanking is assumed to be %u\n", this->aBlanking[1]);
            }
            else if (RET_NOT_FOUND != ret)
            {
                missing++;
                fprintf(stderr, "Error while parsing parameter -DG_blanking\n");
            }
        }

        /*
         * must be last action
         */
        ret = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND, "Usage information", NULL);
        if(RET_FOUND == ret || missing > 0)
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

    LoopCamera(DemoConfiguration &_config): config(_config)
    {
        ISPC::Sensor *sensor = 0;
        if(strncmp(config.pszSensor, IIFDG_SENSOR_INFO_NAME, strlen(config.pszSensor))==0)
        {
            isDatagen = INT_DG;

            if (!ISPC::DGCamera::supportIntDG())
            {
                std::cerr << "ERROR trying to use internal data generator but it is not supported by HW!" << std::endl;
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
                std::cerr << "ERROR trying to use external data generator but it is not supported by HW!" << std::endl;
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

        if(camera->state==ISPC::Camera::CAM_ERROR)
        {
            std::cerr << "ERROR failed to create camera correctly" << std::endl;
            delete camera;
            camera = 0;
            return;
        }

        ISPC::CameraFactory::populateCameraFromHWVersion(*camera, sensor);

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

    int startCapture()
    {
        if ( buffersIds.size() > 0 )
        {
            fprintf(stderr, "ERROR: Buffers still exists - camera was not stopped beforehand\n");
            return EXIT_FAILURE;
        }

        // allocate all types of buffers needed
        printf("### allocateBufferPool %d\n", config.nBuffers);

	if(camera->allocateBufferPool(config.nBuffers, buffersIds)!= IMG_SUCCESS)
        {
            fprintf(stderr, "ERROR: allocating buffers.\n");
            return EXIT_FAILURE;
        }

        missedFrames = 0;

        if(camera->startCapture() !=IMG_SUCCESS)
        {
            fprintf(stderr, "ERROR: starting capture.\n");
            return EXIT_FAILURE;
        }

        //
        // pre-enqueue shots to have multi-buffering
        //
        for ( int i = 0 ; i < config.nBuffers-1 ; i++ )
        {
            ISPC::ModuleOUT *glb = camera->getModule<ISPC::ModuleOUT>();
            //printf("enc %d disp %d de %d\n",
            //        (int)glb->encoderType, (int)glb->displayType, (int)glb->dataExtractionType);
            if(camera->enqueueShot() !=IMG_SUCCESS)
            {
                fprintf(stderr, "ERROR: pre-enqueuing shot %d/%d.", i+1, config.nBuffers-1);
                return EXIT_FAILURE;
            }
        }

        if(config.uiSensorInitExposure>0)
        {
            if(camera->getSensor()->setExposure(config.uiSensorInitExposure) != IMG_SUCCESS)
            {
                fprintf(stderr, "WARNING: failed to set sensor to initial exposure of %u us\n", config.uiSensorInitExposure);
            }
        }
        if(config.flSensorInitGain>1.0)
        {
            if (camera->getSensor()->setGain(config.flSensorInitGain) != IMG_SUCCESS)
            {
                fprintf(stderr, "WARNING: failed to set sensor to initial gain of %lf\n", config.flSensorInitGain);
            }
        }

        return EXIT_SUCCESS;
    }

    int stopCapture()
    {
        // we need to acquire the buffers we already pushed otherwise the HW hangs...
        /// @ investigate that
        for ( int i = 0 ; i < config.nBuffers-1 ; i++ )
        {
            ISPC::Shot shot;
            if(camera->acquireShot(shot) !=IMG_SUCCESS)
            {
                fprintf(stderr, "ERROR: acquire shot %d/%d before stopping.", i+1, config.nBuffers-1);
                return EXIT_FAILURE;
            }
            camera->releaseShot(shot);
        }

        if ( camera->stopCapture() != IMG_SUCCESS )
        {
            fprintf(stderr, "Failed to Stop the capture!\n");
            return EXIT_FAILURE;
        }

        std::list<IMG_UINT32>::iterator it;
        for ( it = buffersIds.begin() ; it != buffersIds.end() ; it++ )
        {
            debug("deregister buffer %d\n", *it);
            if ( camera->deregisterBuffer(*it) != IMG_SUCCESS )
            {
                fprintf(stderr, "Failed to deregister buffer ID=%d!\n", *it);
                return EXIT_FAILURE;
            }
        }
        buffersIds.clear();

        printf("total number of missed frames: %d\n", missedFrames);

        return EXIT_SUCCESS;
    }

#if defined(INFOTM_ISP) && defined(CONTROL_EXPOSURE_BY_KEY)
    void saveResults(const ISPC::Shot &shot, DemoConfiguration &config, const long lConut, const unsigned long ulExposureTime)
#else
    void saveResults(const ISPC::Shot &shot, DemoConfiguration &config)
#endif //INFOTM_ISP && CONTROL_EXPOSURE_BY_KEY
    {
        std::stringstream outputName;
        bool saved = false;
        static int savedFrames = 0;
        debug("Shot RGB=%s YUV=%s DE=%s HDR=%s TIFF=%s\n",
                FormatString(shot.DISPLAY.pxlFormat),
                FormatString(shot.YUV.pxlFormat),
                FormatString(shot.BAYER.pxlFormat),
                FormatString(shot.HDREXT.pxlFormat),
                FormatString(shot.RAW2DEXT.pxlFormat)
        );

        if ( shot.DISPLAY.pxlFormat != PXL_NONE && config.bSaveRGB )
        {
            outputName.str("");
            outputName << "display"<<savedFrames<<".flx";

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::Display, outputName.str().c_str()) != IMG_SUCCESS )
            {
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving RGB output to " << outputName.str() << std::endl;
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
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving YUV output to " << outputName.str() << std::endl;
                saved = true;
            }
            config.bSaveYUV = false;
        }

        if ( shot.BAYER.pxlFormat != PXL_NONE && config.bSaveDE )
        {
            outputName.str("");
#if defined(INFOTM_ISP) && defined(CONTROL_EXPOSURE_BY_KEY)
            char filename[100];
#endif //INFOTM_ISP && CONTROL_EXPOSURE_BY_KEY

#if defined(INFOTM_ISP) && defined(CONTROL_EXPOSURE_BY_KEY)
            //printf("Capture image = %03ld, Exposure Time = %05lu\n", lConut, ulExposureTime);
            sprintf(filename, "_%03ld_%05lu", lConut, ulExposureTime);
            outputName << "dataExtraction"<<filename<<".flx";
#else
            outputName << "dataExtraction"<<savedFrames<<".flx";
#endif //INFOTM_ISP && CONTROL_EXPOSURE_BY_KEY

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot, ISPC::Save::Bayer, outputName.str().c_str()) != IMG_SUCCESS )
            {
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving Bayer output to " << outputName.str() << std::endl;
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
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving HDR Extraction output to " << outputName.str() << std::endl;
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
                std::cerr << "WARNING: failed to saved to " << outputName.str() << std::endl;
            }
            else
            {
                std::cout << "INFO: saving RAW 2D Extraction output to " << outputName.str() << std::endl;
                saved = true;
            }
            config.bSaveRaw2D = false;
        }

        if ( saved )
        {
            // we saved a file so we save the parameters it used too
            ISPC::ParameterList parameters;

            outputName.str("");
            outputName << "FelixSetupArgs_"<<savedFrames<<".txt";

            if ( camera->saveParameters(parameters) == IMG_SUCCESS )
            {
                ISPC::ParameterFileParser::saveGrouped(parameters, outputName.str());
                std::cout << "INFO: saving parameters to " << outputName.str() << std::endl;
            }
            else
            {
                std::cerr << "WARNING: failed to save parameters" << std::endl;
            }

            savedFrames++;
        }
    }

    void printInfo(const ISPC::Shot &shot, const DemoConfiguration &config) // cannot be const because modifies missedFrames
    {
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
            printf("LSH: bmatrix %d 0 from '%s'\n",
                    pLSH->bEnableMatrix,
                    pLSH->getFilename(0).c_str()
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

#ifdef INFOTM_ISP
char getch(void)
{
    static char line[2];
    if(read(0, line, 1))
    {
        return line[0];
    }
    return -1;
}
#endif //INFOTM_ISP

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
#if defined(INFOTM_ISP) && defined(CONTROL_EXPOSURE_BY_KEY)
    long lAutoCaptureCount = 0;
    int iAutoCaptureMode = 0;
    int iAutoCaptureStage = 0;
    double dbAutoStartCaptureTime;
    double dbAutoCurrentCaptureTime;
#endif //INFOTM_ISP && CONTROL_EXPOSURE_BY_KEY

    //Default configuration, everything disabled but control objects are created
    DemoConfiguration config;

    if ( config.loadParameters(argc, argv) != EXIT_SUCCESS )
    {
        return EXIT_FAILURE;
    }

    printf("Using sensor %s\n", config.pszSensor);
    printf("Using config file %s\n", config.pszFelixSetupArgsFile);

    //load parameter file
    ISPC::ParameterFileParser pFileParser;
    ISPC::ParameterList parameters = pFileParser.parseFile(config.pszFelixSetupArgsFile);
    if(parameters.validFlag == false) //Something went wrong parsing the parameters
    {
        std::cout <<"ERROR parsing parameter file \'"<<config.pszFelixSetupArgsFile<<"\'"<<std::endl;
        return EXIT_FAILURE;
    }

    LoopCamera loopCam(config);

    if(!loopCam.camera)
    {
        std::cerr << "ERROR when creating camera" << std::endl;
        return EXIT_FAILURE;
    }

    if(loopCam.camera->loadParameters(parameters) != IMG_SUCCESS)
    {
        std::cerr << "ERROR loading pipeline setup parameters from file: "<< config.pszFelixSetupArgsFile<<std::endl;
        return EXIT_FAILURE;
    }

    config.loadFormats(parameters);

    if(loopCam.camera->program() != IMG_SUCCESS)
    {
        std::cerr <<"ERROR programming pipeline."<<std::endl;
        return EXIT_FAILURE;
    }

    /*if(loopCam.camera->allocateBufferPool(config.nBuffers, buffersIds)!= IMG_SUCCESS) //enqueue one shot in the pipeline with the current configuration
      {
      std::cerr <<"ERROR allocating buffers." <<std::endl;
      return EXIT_FAILURE;
      }

      if(loopCam.camera->startCapture() !=IMG_SUCCESS)
      {
      std::cerr<<"ERROR starting capture."<<std::endl;
      return EXIT_FAILURE;
      }*/

    // Create, setup and register control modules-----------------
    ISPC::ControlAE *pAE = 0;
    ISPC::ControlAWB_PID *pAWB = 0;
    ISPC::ControlTNM *pTNM = 0;
    ISPC::ControlDNS *pDNS = 0;
    ISPC::ControlLBC *pLBC = 0;
    ISPC::ControlAF *pAF = 0;
    ISPC::Sensor *sensor = loopCam.camera->getSensor();
#if defined(DISABLE_AE)
    config.controlAE = false;
#endif
#if defined(DISABLE_AWB)
    config.controlAWB = false;
    config.enableAWB = false;
#endif
#if defined(DISABLE_AF)
    config.controlAF = false;
#endif

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

    if ( pAE )
    {
    	//use setting file as initial targetBrightness value. 20150819.Fred
		config.targetBrightness = pAE->getTargetBrightness();
		
        pAE->setTargetBrightness(config.targetBrightness);
    }

    if ( pAWB )
    {
		if (!config.commandAWB)
		{
			//use setting file as initial AWBAlgorithm value. 20150819.Fred
			config.AWBAlgorithm = pAWB->getCorrectionMode();
		}

        pAWB->setCorrectionMode(config.AWBAlgorithm);
    }
    //--------------------------------------------------------------

    bool loopActive=true;
    bool bSaveOriginals=false; // when saving all saves original formats from config file

    int frameCount = 0;
    double estimatedFPS = 0.0;

    double FPSstartTime = currTime();
    int neededFPSFrames = (int)floor(sensor->flFrameRate*FPS_COUNT_MULT);

    config.applyConfiguration(*(loopCam.camera));
    config.bResetCapture = true; // first time we force the reset

#if defined(ENABLE_GPIO_TOGGLE_FRAME_COUNT)
    // Initial GPIO 147 for the frame rate check.
    static int index=0;

	system ("echo 147 > /sys/class/gpio/export");
	system ("echo out > /sys/class/gpio/gpio147/direction");

#endif
    while(loopActive)   //Capture loop
    {
        if (frameCount%neededFPSFrames == (neededFPSFrames-1))
        {
            double cT = currTime();

            estimatedFPS = (double(neededFPSFrames)/(cT-FPSstartTime));
            FPSstartTime = cT;
        }

#if defined(ENABLE_GPIO_TOGGLE_FRAME_COUNT)
		// Set this GPIO pin as high to count frame rate.
		system ("echo 1 > /sys/class/gpio/gpio147/value");

#endif
        time_t t;
        struct tm tm;

        t = time(NULL); // to print info
        tm = *localtime(&t);
        if (frameCount % 200 == 0) {
            printf("[%d/%d %02d:%02d.%02d]\n",
                    tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, tm.tm_sec
                  );

            printf("Frame %d - sensor %dx%d @%3.2f (mode %d - mosaic %d) - estimated FPS %3.2f (%d frames avg. with %d buffers)\n",
                    frameCount,
                    sensor->uiWidth, sensor->uiHeight, sensor->flFrameRate, config.sensorMode,
                    (int)sensor->eBayerFormat,
                    estimatedFPS, neededFPSFrames, config.nBuffers
                  );
        }

        if ( config.bResetCapture )
        {
            printf("INFO: re-starting capture to enable alternative output\n");

            loopCam.stopCapture();

            if ( loopCam.camera->setupModules() != IMG_SUCCESS )
            {
                fprintf(stderr, "Failed to re-setup the pipeline!\n");
                return EXIT_FAILURE;
            }

            if( loopCam.camera->program() != IMG_SUCCESS )
            {
                fprintf(stderr, "Failed to re-program the pipeline!\n");
                return EXIT_FAILURE;
            }

            loopCam.startCapture();

            // reset flags
            config.bResetCapture = false;
        }

        if ((ret = loopCam.camera->enqueueShot()) != IMG_SUCCESS)
        {

            std::cerr<<"ERROR enqueuing shot."<<std::endl;
            return EXIT_FAILURE;
        }

        ISPC::Shot shot;
        if ((ret = loopCam.camera->acquireShot(shot)) != IMG_SUCCESS)
        {
            std::cerr<<"ERROR retrieving processed shot from pipeline."<<std::endl;
            return EXIT_FAILURE;

        }

        if (frameCount % 200 == 0)
        {
            printf("shot.YUV.phy:0x%x, shot.YUV.data:0x%x, stride:%d, height:%d\n", shot.YUV.phyAddr, shot.YUV.data, shot.YUV.stride, shot.YUV.height);
        }
#if defined(ENABLE_GPIO_TOGGLE_FRAME_COUNT)
		// Set this GPIO pin as low to count frame rate.
		system ("echo 0 > /sys/class/gpio/gpio147/value");

#endif
#if 0
//        printf("stride = %d, width = %d, height = %d\n",
//           shot.RGB.stride, shot.RGB.width, shot.RGB.height);
        int screensize = shot.RGB.width * shot.RGB.height * 4;
        int fbfd = open("/dev/fb0", O_RDWR);
        char *fbp=(char*)mmap(0, screensize, PROT_READ|PROT_WRITE,  MAP_SHARED, fbfd, 0);

        memcpy(fbp, shot.RGB.data, screensize);
        munmap(fbp, screensize);
        close(fbfd);

#endif
        if (ret == IMG_SUCCESS)
        {
#ifdef MAINSCREEN_FB_NAME 
			int fbfd = open(MAINSCREEN_FB_NAME, O_RDWR);
			struct fb_var_screeninfo vinfo;
			ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);


			vinfo.reserved[0] = (unsigned int )shot.YUV.phyAddr;

			ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo);
			close(fbfd);
#endif
        }
        //Deal with the keyboard input
        //char read = fgetc(stdin);
        char read = getch();
#if defined(INFOTM_ISP) && defined(CONTROL_EXPOSURE_BY_KEY)
        if (0xff == read) {
            if (iAutoCaptureMode) {
                //double dbAutoStartCaptureTime;
                //double dbAutoCurrentCaptureTime;
            	dbAutoCurrentCaptureTime = currTime();
                if (100 > lAutoCaptureCount) {
/*
                    if (((30 * 2) - 1) == iFrameCount) {
                        read = EXPOSURE_UP_KEY;
                    } else if (((30 * 4) - 1) <= iFrameCount) {
                        iFrameCount = 0;
                        read = SAVE_DE_KEY;
                    }
*/
                    switch (iAutoCaptureStage) {
                        case 0:
                            if (1.0 < (dbAutoCurrentCaptureTime - dbAutoStartCaptureTime)) {
                                iAutoCaptureStage = 1;
                                read = EXPOSURE_UP_KEY;
                            }
                            //if (((30 * 4) - 1) < iFrameCount) {
                            //    ++iAutoCaptureStage;
                            //    iFrameCount = 0;
                            //    config.bSaveDE = true;
                            //}
                            break;

                        case 1:
                            if (5.0 < (dbAutoCurrentCaptureTime - dbAutoStartCaptureTime)) {
                                iAutoCaptureStage = 0;
                                dbAutoStartCaptureTime = dbAutoCurrentCaptureTime;
                                read = SAVE_DE_KEY;
                            }
                            //if (((30 * 1) - 1) < iFrameCount) {
                            //    iAutoCaptureStage = 0;
                            //    ++lAutoCaptureCount;
                            //    current_expo = sensor->getExposure();
                            //    current_expo += UP_DOWN_STEP;
                            //    sensor->setExposure(current_expo);
                            //}
                            break;
                    }
                } else {
                    iAutoCaptureMode = 0;
                    iAutoCaptureStage = 0;
                    lAutoCaptureCount = 0;
                    printf("--->Exit auto capture raw data mode!!!\n");
                }
            }
        }
#endif //INFOTM_ISP && CONTROL_EXPOSURE_BY_KEY
        switch(read)
        {
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
                config.targetBrightness+=0.05;
                break;

            case BRIGHTNESSDOWN_KEY:    //Decrease target brightness
                config.targetBrightness-=0.05;
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
                printf("INFO: save RGB key pressed...\n");
                break;

            case SAVE_YUV_KEY:
                config.bSaveYUV = true;
                printf("INFO: save YUV key pressed...\n");
                break;

            case SAVE_DE_KEY:
                config.bSaveDE=true;
                printf("INFO: save DE key pressed...\n");
                break;

            case SAVE_HDR_KEY:
                if ( loopCam.camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT )
                {
                    config.bSaveHDR=true;
                    printf("INFO: save HDR key pressed...\n");
                }
                else
                {
                    printf("WARNING: cannot enable HDR extraction, HW does not support it!\n");
                }
                break;

            case SAVE_RAW2D_KEY:
                if ( loopCam.camera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT )
                {
                    config.bSaveRaw2D=true;
                    printf("INFO: save RAW2D key pressed...\n");
                }
                else
                {
                    printf("WARNING: cannot enable RAW 2D extraction, HW does not support it!\n");
                }
                break;

            case SAVE_ORIGINAL_KEY:
                bSaveOriginals=true;
                printf("INFO: save ORIGINAL key pressed...\n");
                break;

#if defined(INFOTM_ISP) && defined(CONTROL_EXPOSURE_BY_KEY)
            case EXPOSURE_UP_KEY:
                {
                    unsigned long current_expo;
                    current_expo = sensor->getExposure();
                    //printf("bef current_expo=%lu\n", current_expo);

                    current_expo += UP_DOWN_STEP;

                    sensor->setExposure(current_expo);
            	    printf("--->set current_expo = %lu\n", current_expo);
                }
               break;

            case EXPOSURE_DOWN_KEY:
                {
                    unsigned long current_expo;
                    current_expo = sensor->getExposure();
                    //printf("bef current_expo=%lu\n", current_expo);

                    current_expo -= UP_DOWN_STEP;

                    sensor->setExposure(current_expo);
                    printf("--->set current_expo = %lu\n", current_expo);
                }
                break;

            case AUTO_CAPTURE_KEY:
                {
                    unsigned long ulExposureTime;

                    ulExposureTime = sensor->getExposure();
                    ulExposureTime -= UP_DOWN_STEP;
                    sensor->setExposure(ulExposureTime);
                    iAutoCaptureMode = 1;
                    iAutoCaptureStage = 0;
                    lAutoCaptureCount = 0;
                    dbAutoStartCaptureTime = currTime();
                    printf("--->Enter auto capture raw data mode!!!\n");
                }
                break;
#endif //INFOTM_ISP && CONTROL_EXPOSURE_BY_KEY

            case EXIT_KEY:  //Exit application
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
        debug("bSaveResults=%d frameError=%D\n", bSaveResults, shot.bFrameError);

        if ((ret == IMG_SUCCESS) && bSaveResults && !shot.bFrameError)
        {
#if defined(INFOTM_ISP) && defined(CONTROL_EXPOSURE_BY_KEY)
        	unsigned long ulExposureTime;

            ulExposureTime = sensor->getExposure();
            loopCam.saveResults(shot, config, lAutoCaptureCount, ulExposureTime);
            ++lAutoCaptureCount;
#else
            loopCam.saveResults(shot, config);
#endif //INFOTM_ISP && CONTROL_EXPOSURE_BY_KEY
        }

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

        //loopCam.printInfo(shot, config);

        //Update Camera loop controls configuration
        // before the release because may use the statistics
        config.applyConfiguration(*(loopCam.camera));

        // done after apply configuration as we already did all changes we needed
        if (bSaveOriginals)
        {
            config.loadFormats(parameters, true); // reload with defaults
            bSaveOriginals=false;
        }


        if (ret == IMG_SUCCESS)
        {
 			if(loopCam.camera->releaseShot(shot) != IMG_SUCCESS)
			{
                //std::cerr<<"Error releasing shot." <<std::endl;
                printf("ERR: Error releasing shot.\n");
				return EXIT_FAILURE;
			}
        }

        frameCount++;
    }

    debug("Exiting loop\n");

    loopCam.stopCapture();

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    int ret;
    struct termios orig_term_attr;
    enableRawMode(orig_term_attr);

#ifdef MAINSCREEN_FB_NAME 
    int fbfd = open(MAINSCREEN_FB_NAME, O_RDWR);
    struct fb_var_screeninfo vinfo; 
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    vinfo.nonstd = 2;
    //vinfo.xres = 1920; //the dss driver has already hack the res to 1920x1088;
    //vinfo.yres = 1088;
    ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo);
    close(fbfd);
#endif

    ret = run(argc, argv);

    disableRawMode(orig_term_attr);

    return EXIT_SUCCESS;
}
