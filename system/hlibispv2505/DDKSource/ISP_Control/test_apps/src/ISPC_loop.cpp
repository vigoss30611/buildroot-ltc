/**
*******************************************************************************
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
#include "ISPC_loop.h"
#include <dyncmd/commandline.h>

#include <iostream>
#include <termios.h>
#include <sstream>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include <ispc/Pipeline.h>
#include <ci/ci_api.h>
#include <ci/ci_version.h>
#include <mc/module_config.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>
#include <ispc/Camera.h>

// control algorithms
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB.h>
#include <ispc/ControlAWB_PID.h>
#include <ispc/ControlAWB_Planckian.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlDNS.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlAF.h>
#include <ispc/ControlLSH.h>
#include <ispc/TemperatureCorrection.h>

// modules access
#include <ispc/ModuleOUT.h>
#include <ispc/ModuleCCM.h>
#include <ispc/ModuleDPF.h>
#include <ispc/ModuleWBC.h>
#include <ispc/ModuleLSH.h>

// modules access for moitoring only
#include <ispc/ModuleBLC.h>
#include <ispc/ModuleCCM.h>
#include <ispc/ModuleDGM.h>
#include <ispc/ModuleDNS.h>
#include <ispc/ModuleDPF.h>
#include <ispc/ModuleDSC.h>
#include <ispc/ModuleENS.h>
#include <ispc/ModuleESC.h>
#include <ispc/ModuleEXS.h>
#include <ispc/ModuleFLD.h>
#include <ispc/ModuleFOS.h>
#include <ispc/ModuleGMA.h>
#include <ispc/ModuleHIS.h>
#include <ispc/ModuleIIF.h>
#include <ispc/ModuleLCA.h>
#include <ispc/ModuleLSH.h>
#include <ispc/ModuleMGM.h>
#include <ispc/ModuleMIE.h>
#include <ispc/ModuleOUT.h>
#include <ispc/ModuleR2Y.h>
#include <ispc/ModuleRLT.h>
#include <ispc/ModuleSHA.h>
#include <ispc/ModuleTNM.h>
#include <ispc/ModuleVIB.h>
#include <ispc/ModuleWBC.h>
#include <ispc/ModuleWBS.h>
#include <ispc/ModuleY2R.h>

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

#ifdef USE_DMABUF
#include "dmabuf/dmabuf.h"
#include "felixcommon/ci_alloc_info.h"
#endif

#define LOG_TAG "ISPC_loop"
#include <felixcommon/userlog.h>

// performance/efficiency measurement
#include "ispc/PerfTime.h"
#include "ispctest/perf_controls.h"

//#define debug printf
#define debug

#define AWB_KEY 'w'
#define WBTS_KEY 'q'
#define DPF_KEY 'e'
#define LSH_KEY 'r'
#define ADAPTIVETNM_KEY 't'
#define LOCALTNM_KEY 'y'
#define AUTOTNM_KEY 'u'
#define DNSISO_KEY 'i'
#define LBC_KEY 'o'
#define AUTOFLICKER_KEY 'p'

#define BRIGHTNESSUP_KEY '+'
#define BRIGHTNESSDOWN_KEY '-'
#define FOCUSCLOSER_KEY ','
#define FOCUSFURTHER_KEY '.'
#define FOCUSTRIGGER_KEY 'm'

#define SAVE_ORIGINAL_KEY 'a'
// display output is RGB or YUV 444
#define SAVE_DISPLAY_OUTPUT_KEY 's'
// encoder output is YUV (not 444)
#define SAVE_ENCODER_OUTPUT_KEY 'd'
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
#define FPS_KEY 'j'
#endif
#define EXIT_KEY 'x'

/**
 * @brief used to compute the number of frames to use for the FPS count
 * (sensor FPS*multiplier) so that the time is more than a second
 */
#define FPS_COUNT_MULT 1.5

/*
 * DemoConfiguration implementation
 */

DemoConfiguration::DemoConfiguration()
{
    // AE
    controlAE = true;
    targetBrightness = 0.0f;
    flickerRejection = false;
    monitorAE = false;
    autoFlickerRejection = false;
    flickerRejectionFreq = 0.0;
    currentAeFlicker = getFlickerModeFromState();

    // TNM
    controlTNM = true;
    autoToneMapping = false;
    localToneMapping = false;
    adaptiveToneMapping = false;

    // DNS
    controlDNS = true;
    autoDenoiserISO = false;

    // LBC
    controlLBC = true;
    autoLBC = false;

    // AF
    controlAF = true;
    monitorAF = false;

    // AWB
    controlAWB = true;
    enableAWB = false;
    AWBAlgorithm = DemoConfiguration::WB_PLANCKIAN;
    WBTSEnabled = false;
    WBTSWeightsBase = ISPC::ControlAWB_Planckian::DEFAULT_WEIGHT_BASE;
    targetAWB = 0;
    monitorAWB = false;

    // LSH
    controlLSH=true;
    bEnableLSH=false;

    measureVerbose = false;

    alwaysHasDisplay = true;
    updateASAP = false;

    // savable
    bSaveDisplay=false;
    bSaveEncoder=false;
    bSaveDE=false;
    bSaveHDR=false;
    bSaveRaw2D=false;

    bEnableDPF=false;

    bResetCapture=false;
    bTestStopStart = false;
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
    ulEnqueueDelay=0;
    ulRandomDelayLow=0;
    ulRandomDelayHigh=0;

    useLocalBuffers = false;
}

DemoConfiguration::~DemoConfiguration()
{
    if(this->pszSensor)
    {
        IMG_FREE(this->pszSensor);
        this->pszSensor = NULL;
    }
    if(this->pszFelixSetupArgsFile)
    {
        IMG_FREE(this->pszFelixSetupArgsFile);
        this->pszFelixSetupArgsFile = NULL;
    }
    if(this->pszInputFLX)
    {
        IMG_FREE(this->pszInputFLX);
        this->pszInputFLX = NULL;
    }
}

ISPC::ControlAWB * DemoConfiguration::instantiateControlAwb(IMG_UINT8 hwMajor, IMG_UINT8 hwMinor)
{
    ISPC::ControlAWB *pAWB = NULL;
        if((hwMajor<=2 && hwMinor<6) &&
                AWBAlgorithm == WB_PLANCKIAN)
    {
        printf("NOTICE: HW version earlier than 2.6: "
                    "Overriding AWB Planckian with AWB PID/COMBINED\n");
            AWBAlgorithm = WB_COMBINED;
    }
        switch(AWBAlgorithm)
    {
            case WB_PLANCKIAN:
                pAWB = new ISPC::ControlAWB_Planckian();
            break;
            case WB_NONE:
                pAWB = NULL;
            break;
        default:
                pAWB = new ISPC::ControlAWB_PID();
            break;
    }
    return pAWB;
}

ISPC::ControlAWB * DemoConfiguration::getControlAwb(ISPC::Camera* camera) const
{
    ISPC::ControlAWB *pAWB = NULL;
        if(!controlAWB) {
            return NULL;
        }
        switch(AWBAlgorithm)
    {
            case WB_PLANCKIAN:
                pAWB = camera->getControlModule<ISPC::ControlAWB_Planckian>();
            break;
            case WB_NONE:
                IMG_ASSERT(false);
            break;
        default:
                pAWB = camera->getControlModule<ISPC::ControlAWB_PID>();
            break;
    }
    return pAWB;
}

void DemoConfiguration::loadFormats(const ISPC::ParameterList &parameters, bool generateDefault)
{
    originalYUV = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::ENCODER_TYPE));
    LOG_DEBUG("loaded YVU=%s\n", FormatString(originalYUV));
    if ( generateDefault && originalYUV == PXL_NONE )
    {
        originalYUV = YVU_420_PL12_8;
        LOG_INFO("default YUV forced to %s\n", FormatString(originalYUV));
    }

    originalRGB = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DISPLAY_TYPE));
    LOG_DEBUG("loaded RGB=%s\n", FormatString(originalRGB));
    if ( generateDefault && originalRGB == PXL_NONE )
    {
        originalRGB = RGB_888_32;
        LOG_INFO("default RGB forced to %s\n", FormatString(originalRGB));
    }


    originalBayer = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_TYPE));
    LOG_DEBUG("loaded DE=%s\n", FormatString(originalBayer));
    if ( generateDefault && originalBayer == PXL_NONE )
    {
        originalBayer = BAYER_RGGB_12;
        LOG_INFO("default DE forced to %s\n", FormatString(originalBayer));
    }

    originalDEPoint = (CI_INOUT_POINTS)(parameters.getParameter(ISPC::ModuleOUT::DATAEXTRA_POINT)-1);
    LOG_DEBUG("loaded DE point=%d\n", (int)originalDEPoint);
    if ( generateDefault && originalDEPoint != CI_INOUT_BLACK_LEVEL && originalDEPoint != CI_INOUT_FILTER_LINESTORE )
    {
        originalDEPoint = CI_INOUT_BLACK_LEVEL;
        LOG_INFO("default DE-Point forced to %d\n", originalDEPoint);
    }

    originalHDR = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::HDREXTRA_TYPE));
    LOG_DEBUG("loaded HDR=%s (forced)\n", FormatString(BGR_101010_32));
    if ( generateDefault && originalHDR == PXL_NONE )
    {
        originalHDR = BGR_101010_32;
        LOG_INFO("default HDF forced to %s\n", FormatString(originalHDR));
    }


    originalTiff = ISPC::ModuleOUT::getPixelFormat(parameters.getParameter(ISPC::ModuleOUT::RAW2DEXTRA_TYPE));
    LOG_DEBUG("loaded TIFF=%s\n", FormatString(originalTiff));
    if ( generateDefault && originalTiff == PXL_NONE )
    {
        originalTiff = BAYER_TIFF_12;
        LOG_INFO("default Raw2D forced to %s\n", FormatString(originalTiff));
    }
}

void DemoConfiguration::applyConfiguration(ISPC::Camera &cam)
{
    //
    // control part
    //
    ISPC::ControlAE *pAE = cam.getControlModule<ISPC::ControlAE>();
    ISPC::ControlAWB *pAWB = getControlAwb(&cam);
    ISPC::ControlTNM *pTNM = cam.getControlModule<ISPC::ControlTNM>();
    ISPC::ControlDNS *pDNS = cam.getControlModule<ISPC::ControlDNS>();
    ISPC::ControlLBC *pLBC = cam.getControlModule<ISPC::ControlLBC>();
    ISPC::ControlLSH *pLSH = cam.getControlModule<ISPC::ControlLSH>();

    if ( pAE )
    {
        pAE->setTargetBrightness(this->targetBrightness);

        //Enable/disable flicker rejection in the auto-exposure algorithm
        pAE->enableFlickerRejection(
                this->flickerRejection,
                this->flickerRejectionFreq);
        pAE->enableAutoFlickerRejection(this->autoFlickerRejection);
    }

    if ( pTNM )
    {
        // parameters loaded from config file
        pTNM->enableControl(this->autoToneMapping);
        // this->localToneMapping||this->adaptiveToneMapping;

        //Enable/disable local tone mapping
        pTNM->enableLocalTNM(this->localToneMapping);

        //Enable/disable automatic tone mapping strength control
        pTNM->enableAdaptiveTNM(this->adaptiveToneMapping);

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
    ISPC::ModuleOUT *glb = cam.getModule<ISPC::ModuleOUT>();

    this->bResetCapture = false; // we assume we don't need to reset

    ePxlFormat prev = glb->encoderType;
    if ( this->bSaveEncoder )
    {
        glb->encoderType = this->originalYUV;
    }
    else
    {
        glb->encoderType = PXL_NONE;
    }
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
        if(alwaysHasDisplay)
        {
            LOG_INFO("Re-enable display to %s\n", FormatString(this->originalRGB));
            glb->displayType = this->originalRGB;
        }
        else
        {
            glb->displayType = PXL_NONE;
        }
    }
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
        if (pLSH->isEnabled() != this->bEnableLSH)
        {
            ISPC::ModuleLSH *pLSHmod = cam.getModule<ISPC::ModuleLSH>();

            if (pLSHmod)
            {
                pLSH->enableControl(this->bEnableLSH);
                if (!this->bEnableLSH)
                {
                    // ensure we disable the grid
                    pLSHmod->configureMatrix(0);
                }
                else if (pLSHmod->getCurrentMatrixId() == 0)
                {
                    /* if we did not have a grid and now we need one we need
                     * to restart */
                    this->bResetCapture = true;
                }
            }
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

int DemoConfiguration::loadParameters(int argc, char *argv[])
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
            case DemoConfiguration::WB_AC:
                this->AWBAlgorithm = DemoConfiguration::WB_AC;
            break;

            case DemoConfiguration::WB_WP:
                this->AWBAlgorithm = DemoConfiguration::WB_WP;
            break;

            case DemoConfiguration::WB_HLW:
                this->AWBAlgorithm = DemoConfiguration::WB_HLW;
            break;

            case DemoConfiguration::WB_COMBINED:
                this->AWBAlgorithm = DemoConfiguration::WB_COMBINED;
            break;

            case DemoConfiguration::WB_PLANCKIAN:
                this->AWBAlgorithm = DemoConfiguration::WB_PLANCKIAN;
                break;

            case DemoConfiguration::WB_NONE:
        default:
            if (!bPrintHelp)
            {
                LOG_ERROR("Unsupported AWB algorithm %d given\n", algo);
            }
            missing++;
            break;
        }
    }

    algo = -1;
    ret = DYNCMD_RegisterParameter("-useWBTS", DYNCMDTYPE_BOOL8,
        "Enable usage of White Balance Temporal Smoothing algorithm - (1=enabled, 0=disabled)",
        &(this->WBTSEnabled));
    if (RET_FOUND != ret)
    {
        this->WBTSEnabledOverride = false;
        if (RET_NOT_FOUND != ret && !bPrintHelp) // if not found use default
        {
            LOG_ERROR("while parsing parameter -useWBTS\n");
            missing++;
        }
    }
    else
    {
        this->WBTSEnabledOverride = true;
    }

    ret=DYNCMD_RegisterParameter("-wbtsTemporalStretch", DYNCMDTYPE_INT,
        "[optional] time to settle awb after change (milliseconds) 200 < param <= 5000, default 300",
        &(this->WBTSTemporalStretch));
    if (RET_FOUND != ret)
    {
        this->WBTSTemporalStretchOverride = false;
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -wbtsTemporalStretch\n");
            missing++;
        }
        // parameter is optional
    }
    else
    {
        this->WBTSTemporalStretchOverride = true;
        if(ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MIN > this->WBTSTemporalStretch)
        {
            LOG_WARNING("-wbtsTemporalStretch below minimum. Clipping\n");
            this->WBTSTemporalStretch = ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MIN;
        }
        if(ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MAX < this->WBTSTemporalStretch)
        {
            LOG_WARNING("-wbtsTemporalStretch above max. Clipping\n");
            this->WBTSTemporalStretch = ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MAX;
        }
    }

    ret=DYNCMD_RegisterParameter("-wbtsWeightsBase", DYNCMDTYPE_FLOAT,
        "[optional] parameter used for wbts algorithm 1<= param <=10, default 2",
        &(this->WBTSWeightsBase));
    if (RET_FOUND != ret)
    {
        this->WBTSWeightsBaseOverride = false;
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -wbtsWeightsBase\n");
            missing++;
        }
        // parameter is optional
    }
    else
    {
        this->WBTSWeightsBaseOverride = true;
        if(ISPC::ControlAWB_Planckian::getMinWeightBase() >
                                                    this->WBTSWeightsBase)

        {
            this->WBTSWeightsBase = ISPC::ControlAWB_Planckian::getMinWeightBase();
            LOG_WARNING("-wbtsWeightsBase parameter < %f. Clipping\n",
                    this->WBTSWeightsBase);
        }
        if(ISPC::ControlAWB_Planckian::getMaxWeightBase() <
                                                    this->WBTSWeightsBase)

        {
            this->WBTSWeightsBase = ISPC::ControlAWB_Planckian::getMaxWeightBase();
            LOG_WARNING("-wbtsWeightsBase parameter > %f. Clipping\n",
                    this->WBTSWeightsBase);
        }
    }

    algo = 0;
    ret=DYNCMD_RegisterParameter("-featuresWBTS", DYNCMDTYPE_UINT,
        "Enable usage of White Balance Temporal Smoothing algorithm - values are mapped ISPC::Smoothing_Features (0=None, 1=FlashFiltering)",
        &(algo));
    if (RET_FOUND != ret)
    {
        this->WBTSFeaturesOverride = false;
        if (RET_NOT_FOUND != ret && !bPrintHelp) // if not found use default
        {
            LOG_ERROR("while parsing parameter -featuresWBTS\n");
            missing++;
        }
    }
    else
    {
        this->WBTSFeaturesOverride = true;
        this->WBTSFeatures = ISPC::ControlAWB_Planckian::WBTS_FEATURES_NONE;
        if ( algo & ISPC::ControlAWB_Planckian::WBTS_FEATURES_FF)
            this->WBTSFeatures = ISPC::ControlAWB_Planckian::WBTS_FEATURES_FF;
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

    if (ISPC::LOG_Perf_Available())
    {
        // monitoring number of frames needed to get to converged state in controls
        ret = DYNCMD_RegisterParameter("-monitorAAA", DYNCMDTYPE_COMMAND,
            "Monitor n of frames needed to converge for AWB, AE, AF algorithms",

            NULL);
        if(RET_FOUND == ret) {
            if(this->controlAWB) this->monitorAWB = true;
            if(this->controlAF)  this->monitorAF  = true;
            if(this->controlAE)  this->monitorAE  = true;
        }

        ret = DYNCMD_RegisterParameter("-monitorAWB", DYNCMDTYPE_COMMAND,
            "Monitor n of frames needed to converge for AWB algorithm",
            NULL);
        if(RET_FOUND == ret) { if(this->controlAWB) this->monitorAWB = true;    }

        ret = DYNCMD_RegisterParameter("-monitorAF", DYNCMDTYPE_COMMAND,
            "Monitor n of frames needed to converge for AF algorithm",
            NULL);
        if(RET_FOUND == ret) { if(this->controlAF) this->monitorAF = true;      }

        ret = DYNCMD_RegisterParameter("-monitorAE", DYNCMDTYPE_COMMAND,
            "Monitor n of frames needed to converge for AE algorithm",
            NULL);
        if(RET_FOUND == ret) { if(this->controlAE) this->monitorAE = true;      }

        // measuring time of functions marked for measuring in modules code
        ret = DYNCMD_RegisterParameter("-measureVERBOSE", DYNCMDTYPE_COMMAND,
            "Measure performance of all Modules",

            NULL);
        if(RET_FOUND == ret) {
            this->measureVerbose = true;
        }

            // measuring time of functions marked for measuring in controls code
        ret = DYNCMD_RegisterParameter("-measureAAA", DYNCMDTYPE_COMMAND,
            "measure n of frames needed to converge for AWB, AE, AF algorithms",

            NULL);
        if(RET_FOUND == ret) {
            if(this->controlAWB) this->measureAWB = true;
            if(this->controlAF)  this->measureAF  = true;
            if(this->controlAE)  this->measureAE  = true;
        }

        ret = DYNCMD_RegisterParameter("-measureAWB", DYNCMDTYPE_COMMAND,
            "Measure n of frames needed to converge for AWB algorithm",
            NULL);
        if(RET_FOUND == ret) { if(this->controlAWB) this->measureAWB = true;    }

        ret = DYNCMD_RegisterParameter("-measureAF", DYNCMDTYPE_COMMAND,
            "Measure n of frames needed to converge for AF algorithm",

            NULL);
        if(RET_FOUND == ret) { if(this->controlAF) this->measureAF = true;      }

        ret = DYNCMD_RegisterParameter("-measureAE", DYNCMDTYPE_COMMAND,
            "Measure n of frames needed to converge for AE algorithm",
            NULL);
        if(RET_FOUND == ret) { if(this->controlAE) this->measureAE = true;      }

        // measuring time of functions marked for measuring in modules code
        ret = DYNCMD_RegisterParameter("-measureMODULES", DYNCMDTYPE_COMMAND,
            "Measure performance of all Modules",
            NULL);
        if(RET_FOUND == ret) {
#if 0
            this->measureBLC = true;
            this->measureCCM = true;
            this->measureDGM = true;
            this->measureDNS = true;
            this->measureDPF = true;
            this->measureDSC = true;
            this->measureENS = true;
            this->measureESC = true;
            this->measureEXS = true;
            this->measureFLD = true;
            this->measureFOS = true;
            this->measureGMA = true;
            this->measureHIS = true;
            this->measureIIF = true;
            this->measureLCA = true;
            this->measureLSH = true;
            this->measureMGM = true;
            this->measureMIE = true;
            this->measureOUT = true;
            this->measureR2Y = true;
            this->measureRLT = true;
            this->measureSHA = true;
            this->measureTNM = true;
            this->measureVIB = true;
            this->measureWBC = true;
            this->measureWBS = true;
            this->measureY2R = true;
#endif
            this->measureMODULES = true;
        }

#if 0
            // disabled until proven useful
            ret = DYNCMD_RegisterParameter("-measureBLC", DYNCMDTYPE_COMMAND,
                "Measure BLC module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureBLC = true;  }

            ret = DYNCMD_RegisterParameter("-measureCCM", DYNCMDTYPE_COMMAND,
                "Measure CCM module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureCCM = true;  }

            ret = DYNCMD_RegisterParameter("-measureDGM", DYNCMDTYPE_COMMAND,
                "Measure DGM module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureDGM = true;  }

            ret = DYNCMD_RegisterParameter("-measureDNS", DYNCMDTYPE_COMMAND,
                "Measure DNS module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureDNS = true;  }

            ret = DYNCMD_RegisterParameter("-measureDPF", DYNCMDTYPE_COMMAND,
                "Measure DPF module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureDPF = true;  }

            ret = DYNCMD_RegisterParameter("-measureDSC", DYNCMDTYPE_COMMAND,
                "Measure DSC module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureDSC = true;  }

            ret = DYNCMD_RegisterParameter("-measureENS", DYNCMDTYPE_COMMAND,
                "Measure ENS module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureENS = true;  }

            ret = DYNCMD_RegisterParameter("-measureESC", DYNCMDTYPE_COMMAND,
                "Measure ESC module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureESC = true;  }

            ret = DYNCMD_RegisterParameter("-measureEXS", DYNCMDTYPE_COMMAND,
                "Measure EXS module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureEXS = true;  }

            ret = DYNCMD_RegisterParameter("-measureFLD", DYNCMDTYPE_COMMAND,
                "Measure FLD module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureFLD = true;  }

            ret = DYNCMD_RegisterParameter("-measureFOS", DYNCMDTYPE_COMMAND,
                "Measure FOS module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureFOS = true;  }

            ret = DYNCMD_RegisterParameter("-measureGMA", DYNCMDTYPE_COMMAND,
                "Measure GMA module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureGMA = true;  }

            ret = DYNCMD_RegisterParameter("-measureHIS", DYNCMDTYPE_COMMAND,
                "Measure HIS module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureHIS = true;  }

            ret = DYNCMD_RegisterParameter("-measureIIF", DYNCMDTYPE_COMMAND,
                "Measure IIF module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureIIF = true;  }

            ret = DYNCMD_RegisterParameter("-measureLCA", DYNCMDTYPE_COMMAND,
                "Measure LCA module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureLCA = true;  }

            ret = DYNCMD_RegisterParameter("-measureLSH", DYNCMDTYPE_COMMAND,
                "Measure LSH module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureLSH = true;  }

            ret = DYNCMD_RegisterParameter("-measureMGM", DYNCMDTYPE_COMMAND,
                "Measure MGM module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureMGM = true;  }

            ret = DYNCMD_RegisterParameter("-measureMIE", DYNCMDTYPE_COMMAND,
                "Measure MIE module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureMIE = true;  }

            ret = DYNCMD_RegisterParameter("-measureOUT", DYNCMDTYPE_COMMAND,
                "Measure OUT module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureOUT = true;  }

            ret = DYNCMD_RegisterParameter("-measureR2Y", DYNCMDTYPE_COMMAND,
                "Measure R2Y module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureR2Y = true;  }

            ret = DYNCMD_RegisterParameter("-measureRLT", DYNCMDTYPE_COMMAND,
                "Measure RLT module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureRLT = true;  }

            ret = DYNCMD_RegisterParameter("-measureSHA", DYNCMDTYPE_COMMAND,
                "Measure SHA module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureSHA = true;  }

            ret = DYNCMD_RegisterParameter("-measureTNM", DYNCMDTYPE_COMMAND,
                "Measure TNM module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureTNM = true;  }

            ret = DYNCMD_RegisterParameter("-measureVIB", DYNCMDTYPE_COMMAND,
                "Measure VIB module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureVIB = true;  }

            ret = DYNCMD_RegisterParameter("-measureWBC", DYNCMDTYPE_COMMAND,
                "Measure WBC module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureWBC = true;  }

            ret = DYNCMD_RegisterParameter("-measureWBS", DYNCMDTYPE_COMMAND,
                "Measure WBS module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureWBS = true;  }

            ret = DYNCMD_RegisterParameter("-measureY2R", DYNCMDTYPE_COMMAND,
                "Measure Y2R module performance",
                NULL);
            if(RET_FOUND == ret) {  this->measureY2R = true;  }
#endif /* 0 */
    }  // LOG_Perf_Available()

#ifdef USE_DMABUF
    ret=DYNCMD_RegisterParameter("-importBuffers", DYNCMDTYPE_COMMAND,
        "Allocate and import image buffers from user space instead of internal allocation in kernel space",
        NULL);
    if (RET_FOUND == ret)
    {
        this->useLocalBuffers = true;
    }
#endif
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

void DemoConfiguration::nextAeFlickerMode()
{
    currentAeFlicker=static_cast<aeFlickerSetting_t>(currentAeFlicker+1);
    currentAeFlicker=static_cast<aeFlickerSetting_t>(currentAeFlicker%AE_FLICKER_MAX);
    switch (currentAeFlicker)
    {
    case AE_FLICKER_OFF:
        flickerRejection = false;
        autoFlickerRejection = false;
        flickerRejectionFreq = 0.0;
        break;
    case AE_FLICKER_AUTO:
        flickerRejection = true;
        autoFlickerRejection = true;
        flickerRejectionFreq = 0.0;
        break;
    case AE_FLICKER_50HZ:
        flickerRejection = true;
        autoFlickerRejection = false;
        flickerRejectionFreq = 50.0;
        break;
    case AE_FLICKER_60HZ:
        flickerRejection = true;
        autoFlickerRejection = false;
        flickerRejectionFreq = 60.0;
        break;
    case AE_FLICKER_MAX:
    default:
        IMG_ASSERT(false);
        break;
    }
}

std::string DemoConfiguration::getFlickerModeStr() const
{
    switch (currentAeFlicker)
    {
    case AE_FLICKER_OFF:
        return "OFF";
        break;
    case AE_FLICKER_AUTO:
        return "AUTO";
        break;
    case AE_FLICKER_50HZ:
        return "50Hz";
        break;
    case AE_FLICKER_60HZ:
        return "60Hz";
        break;
    case AE_FLICKER_MAX:
    default:
        IMG_ASSERT(false);
        break;
    }
    return "FAILURE";
}

DemoConfiguration::aeFlickerSetting_t DemoConfiguration::getFlickerModeFromState()
{
    if(!flickerRejection)
    {
        return AE_FLICKER_OFF;
    }
    if(autoFlickerRejection)
    {
        return AE_FLICKER_AUTO;
    }
    if(flickerRejectionFreq == 50.0)
    {
        return AE_FLICKER_50HZ;
    }
    if(flickerRejectionFreq == 60.0)
    {
        return AE_FLICKER_60HZ;
    }
    // else ignore settings
    return AE_FLICKER_OFF;
}

/*
 * LoopCamera implementation
 */

LoopCamera::LoopCamera(DemoConfiguration &_config): camera(0), config(_config), hwMajor(0), hwMinor(0)
{
    ISPC::Sensor *sensor = NULL;
    IMG_UINT32 nFrames = 0;  // number of frames in FLX
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
        IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), config.nBuffers);

        IIFDG_ExtendedSetBlanking(sensor->getHandle(), config.aBlanking[0], config.aBlanking[1]);

        // ensure that video will load all frames
        nFrames = IIFDG_ExtendedGetFrameCount(sensor->getHandle());

        IIFDG_ExtendedSetFrameCap(sensor->getHandle(), nFrames);

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
        if(!config.pszInputFLX)
        {
            LOG_ERROR("trying to use external data generator but not input FLX provided!\n");
            return;
        }

        camera = new ISPC::DGCamera(0, config.pszInputFLX, config.gasket, false);

        sensor = camera->getSensor();

        // apply additional parameters
        DGCam_ExtendedSetNbBuffers(sensor->getHandle(), config.nBuffers);

        DGCam_ExtendedSetBlanking(sensor->getHandle(), config.aBlanking[0], config.aBlanking[1]);

        DGCam_ExtendedGetFrameCount(sensor->getHandle(), &nFrames);

        DGCam_ExtendedSetFrameCap(sensor->getHandle(), nFrames);

        sensor = NULL;// sensor should be 0 for populateCameraFromHWVersion
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
        LOG_ERROR("failed to create camera correctly\n");
        delete camera;
        camera = 0;
        return;
    }


    int perf = 0;
    if (config.measureMODULES)
    {
        perf++;
    }
    if (config.measureVerbose)
    {
        perf++;
    }
    ISPC::CameraFactory::populateCameraFromHWVersion(*camera, sensor, perf);
    camera->bUpdateASAP = _config.updateASAP;
    hwMajor = camera->hwInfo.rev_ui8Major;
    hwMinor = camera->hwInfo.rev_ui8Minor;

    missedFrames = 0;
    currMatrixId = 0; // invalid ID means no matrix
}

LoopCamera::~LoopCamera()
{
    if(camera)
    {
        delete camera;
        camera = 0;
    }
}

void LoopCamera::printInfo()
{
    if (camera)
    {
        std::cout << "*****" << std::endl;
        PrintGasketInfo(*camera, std::cout);
        PrintRTMInfo(*camera, std::cout);
        std::cout << "*****" << std::endl;
    }
}

int LoopCamera::startCapture()
{
    ISPC::ControlLSH *pLSH = NULL;
    if (camera == NULL)
    {
        return EXIT_FAILURE;
    }
    pLSH = camera->getControlModule<ISPC::ControlLSH>();
    if ( buffersIds.size() > 0 )
    {
        LOG_ERROR("Buffers still exists - camera was not stopped beforehand\n");
        return EXIT_FAILURE;
    }
    if (pLSH)
    {
        LOG_INFO("number of LSH matrices: %d\n", pLSH->getLoadedGrids());
    }
#ifdef USE_DMABUF
    if(config.useLocalBuffers)
    {
        if(camera->addShots(config.nBuffers)!= IMG_SUCCESS)
        {
            LOG_ERROR("Adding shots failed.\n");
            return EXIT_FAILURE;
        }

        const CI_PIPELINE* pipeline =
                camera->getPipeline()->getCIPipeline();
        if(!pipeline) {
            LOG_ERROR("No CI pipeline\n");
            return EXIT_FAILURE;
        }
        IMG_RESULT res;
        IMG_HANDLE buffer;
        IMG_UINT32 bufferId;
        for(int shot=0;shot<config.nBuffers;++shot) {

            res = allocAndImport(pipeline->eEncType,
                            CI_TYPE_ENCODER,
                    false, &buffer, &bufferId);
            if(IMG_SUCCESS == res) {
                buffersIds.push_back(bufferId);
                buffers.push_back(buffer);
            } else if (EXIT_FAILURE == res) {
                return EXIT_FAILURE;
            }

    CI_BUFFTYPE type = CI_TYPE_DISPLAY;
    if (pipeline->eDispType.eBuffer == TYPE_BAYER) {
                type = CI_TYPE_DATAEXT;
            }
            res = allocAndImport(pipeline->eDispType, type,
                    false, &buffer, &bufferId);
            if(IMG_SUCCESS == res) {
                buffersIds.push_back(bufferId);
                buffers.push_back(buffer);
            } else if (EXIT_FAILURE == res) {
                return EXIT_FAILURE;
            }

            res = allocAndImport(pipeline->eHDRExtType,
                            CI_TYPE_HDREXT,
                    false, &buffer, &bufferId);
            if(IMG_SUCCESS == res) {
                buffersIds.push_back(bufferId);
                buffers.push_back(buffer);
            } else if (EXIT_FAILURE == res) {
                return EXIT_FAILURE;
            }

            res = allocAndImport(pipeline->eRaw2DExtraction,
                            CI_TYPE_RAW2D,
                    false, &buffer, &bufferId);
            if(IMG_SUCCESS == res) {
                buffersIds.push_back(bufferId);
                buffers.push_back(buffer);
            } else if (EXIT_FAILURE == res) {
                return EXIT_FAILURE;
            }
        }
    } else
#endif
{
        // allocate all types of buffers needed
        if(camera->allocateBufferPool(config.nBuffers, buffersIds)!= IMG_SUCCESS)
        {
            LOG_ERROR("allocating buffers failed.\n");
            return EXIT_FAILURE;
        }
    }
    missedFrames = 0;

    if(camera->startCapture() != EXIT_SUCCESS)
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
        debug("enc %d disp %d de %d\n",
                (int)glb->encoderType, (int)glb->displayType, (int)glb->dataExtractionType);
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

int LoopCamera::stopCapture()
{
    if (camera == NULL)
    {
        return EXIT_FAILURE;
    }
    bool pending = CI_PipelineHasPending(camera->getPipeline()->getCIPipeline());
    ISPC::ControlLSH *pLSH = NULL;
    pLSH = camera->getControlModule<ISPC::ControlLSH>();

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

    if (pLSH)
    {
        LOG_INFO("number of LSH matrices: %d", pLSH->getLoadedGrids());
    }

    std::list<IMG_UINT32>::iterator it;
    for ( it = buffersIds.begin() ; it != buffersIds.end() ; it++ )
    {
        IMG_UINT32 id = *it;
        LOG_DEBUG("deregister buffer %d\n", id);
        if ( camera->deregisterBuffer(id) != IMG_SUCCESS )
        {
            LOG_ERROR("Failed to deregister buffer ID=%d!\n", id);
            return EXIT_FAILURE;
        }
    }

    buffersIds.clear();

    // LSH matrix is kept between runs

#ifdef USE_DMABUF
    {
        std::list<IMG_HANDLE>::iterator it;
        for ( it = buffers.begin() ; it != buffers.end() ; it++ )
        {
            IMG_HANDLE handle = *it;
            LOG_DEBUG("Free buffer 0x%lx\n", handle);
            if ( DMABUF_Free(handle) != IMG_SUCCESS )
            {
                LOG_ERROR("Failed to free buffer!\n");
                return EXIT_FAILURE;
            }
        }
        buffers.clear();
    }
#endif

    LOG_INFO("total number of missed frames: %d\n", missedFrames);

    return EXIT_SUCCESS;
}

void LoopCamera::saveResults(const ISPC::Shot &shot, DemoConfiguration &config)
{
    std::stringstream outputName;
    bool saved = false;
    static int savedFrames = 0;

    debug("Shot Display=%s YUV=%s DE=%s HDR=%s TIFF=%s\n",
            FormatString(shot.DISPLAY.pxlFormat),
            FormatString(shot.YUV.pxlFormat),
            FormatString(shot.BAYER.pxlFormat),
            FormatString(shot.HDREXT.pxlFormat),
            FormatString(shot.RAW2DEXT.pxlFormat)
    );

    if ( shot.DISPLAY.pxlFormat != PXL_NONE && config.bSaveDisplay )
    {
            if (IMG_TRUE == PixelFormatIsPackedYcc(shot.DISPLAY.pxlFormat) )
            {
                PIXELTYPE fmt;
                PixelTransformDisplay(&fmt, shot.DISPLAY.pxlFormat);
                outputName << "display"<<savedFrames
                    << "-" << shot.DISPLAY.width << "x" << shot.DISPLAY.height
                    << "-" << FormatStringForFilename(shot.DISPLAY.pxlFormat)
                    << "-align" << (unsigned int)fmt.ui8PackedStride
                    << ".yuv";
            }
            else
            {
                outputName << "display"<<savedFrames<<".flx";
            }

            if ( ISPC::Save::Single(*(camera->getPipeline()), shot,
                        ISPC::Save::Display,
                        outputName.str().c_str()) != IMG_SUCCESS )
            {
                LOG_WARNING("failed to saved to %s\n",
                    outputName.str().c_str());
            }
            else
            {
                LOG_INFO("saving display output to %s\n",
                    outputName.str().c_str());
                saved = true;
            }
            config.bSaveDisplay=false;
    }

    if ( shot.YUV.pxlFormat != PXL_NONE && config.bSaveEncoder )
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
        config.bSaveEncoder = false;
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

    if ( saved )
    {
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

        outputName.str("");
        outputName << "statistics" << savedFrames << ".txt";

        if (ISPC::Save::SingleTxtStats(*camera, shot, outputName.str().c_str()) != IMG_SUCCESS )
        {
            LOG_WARNING("failed to open txt statistics %s\n",
                outputName.str().c_str());
        }

        savedFrames++;
    }
}

void LoopCamera::printInfo(const ISPC::Shot &shot, const DemoConfiguration &config, double clockMhz)
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
    const ISPC::ControlAWB *pAWB = config.getControlAwb(camera);
    const ISPC::ControlTNM *pTNM = camera->getControlModule<const ISPC::ControlTNM>();
    const ISPC::ControlDNS *pDNS = camera->getControlModule<const ISPC::ControlDNS>();
    const ISPC::ControlLBC *pLBC = camera->getControlModule<const ISPC::ControlLBC>();
    const ISPC::ModuleDPF *pDPF = camera->getModule<const ISPC::ModuleDPF>();
    const ISPC::ControlLSH *pLSH = camera->getControlModule<ISPC::ControlLSH>();

    printf("Sensor: exposure %.4lfms - gain %.2f\n",
        camera->getSensor()->getExposure()/1000.0, camera->getSensor()->getGain()
    );

    if ( pAE )
    {
        const bool isAuto = (config.currentAeFlicker == DemoConfiguration::AE_FLICKER_AUTO);
        std::stringstream freq(" detected=");
        freq.seekp(0, std::ios_base::end); // set position to last char
        if(isAuto) {
            // int conversions are faster than double
            freq << static_cast<int>(pAE->getFlickerRejectionFrequency());
        }
        printf("AE %d: Metered/target brightness %f/%f (flicker %s%s) underexposed=%d\n",
            pAE->isEnabled(),
            pAE->getCurrentBrightness(),
            pAE->getTargetBrightness(),
            config.getFlickerModeStr().c_str(),
            (isAuto ? freq.str().c_str() : ""),
            pAE->isUnderexposed()
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
            switch(config.AWBAlgorithm)
            {
                case DemoConfiguration::WB_PLANCKIAN:
                    printf("AWB_Planckian %d: measured temp %.0lfK\n",
                    pAWB->isEnabled(),
                        pAWB->getMeasuredTemperature());
            break;
                case DemoConfiguration::WB_NONE:
                    IMG_ASSERT(false);
                    break;
                default:
                    printf("AWB_PID %d: converged=%d mode %s target temp %.0lfK - "
                            "measured temp %.0lfK - correction temp %.0lfK\n",
                    pAWB->isEnabled(),
                        pAWB->hasConverged(),
                        ISPC::ControlAWB::CorrectionName(
                                pAWB->getCorrectionMode()),
                    pAWB->getTargetTemperature(),
                    pAWB->getMeasuredTemperature(),
                    pAWB->getCorrectionTemperature()
                );
            break;
        }
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
        printf("LSH: matrix %d from matrixId '%u' temp %uK\n", (int)pLSH->isEnabled(),
            pLSH->getChosenMatrixId(), pLSH->getChosenTemperature());
    }
}

#ifdef USE_DMABUF
IMG_SIZE LoopCamera::calcBufferSize(const PIXELTYPE& pixelType, bool bTiling)
{
    IMG_SIZE bufSize = 0;


    struct CI_SIZEINFO sSizeInfo = CI_SIZEINFO();
    struct CI_TILINGINFO sTilingInfo = CI_TILINGINFO(); // not used

    IMG_RESULT ret;
    IMG_UINT32 ui32Width = camera->getSensor()->uiWidth;
    IMG_UINT32 ui32Height = camera->getSensor()->uiHeight;

    IMG_UINT32 devmmu_page_size = camera->getConnection()->sHWInfo.mmu_ui32PageSize;

    switch(pixelType.eBuffer) {

    case TYPE_NONE:
        return 0;
        break;

    case TYPE_YUV:
        ret = CI_ALLOC_YUVSizeInfo(&pixelType,
                ui32Width, ui32Height,
                bTiling ? &sTilingInfo : NULL, &sSizeInfo);
        break;

    case TYPE_RGB:
        ret = CI_ALLOC_RGBSizeInfo(&pixelType,
                ui32Width, ui32Height,
                bTiling ? &sTilingInfo : NULL, &sSizeInfo);
        break;

    case TYPE_BAYER:
        ret = CI_ALLOC_Raw2DSizeInfo(&pixelType,
                ui32Width, ui32Height,
                bTiling ? &sTilingInfo : NULL, &sSizeInfo);
        break;

    default:
        LOG_ERROR("Invalid buffer type : %d", pixelType.eBuffer);
        break;
    }
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to calculate buffer size!\n");
        return 0;
    }

    bufSize = sSizeInfo.ui32Stride*sSizeInfo.ui32Height +
            sSizeInfo.ui32CStride*sSizeInfo.ui32CHeight;

    bufSize = (bufSize + devmmu_page_size - 1) & ~(devmmu_page_size-1);
    return bufSize;
}

IMG_RESULT LoopCamera::allocAndImport(const PIXELTYPE& bufferType,
        const CI_BUFFTYPE eBuffer,
        bool isTiled,
        IMG_HANDLE* buffer,
        IMG_UINT32* bufferId)
{
    int fd;
    IMG_UINT32 id;
    IMG_HANDLE handle = NULL;
    IMG_SIZE sizeBytes = calcBufferSize(bufferType, isTiled);

    if(sizeBytes<=0) {
        // not fatal, just output has been disabled or unsupported type
        return IMG_ERROR_DISABLED;
    }

    LOG_DEBUG("DMABUF type=%d, size 0x%x bytes\n",
            eBuffer, sizeBytes);

    if(DMABUF_Alloc(&handle, sizeBytes) != IMG_SUCCESS) {
        LOG_ERROR("DMABUF allocation failed\n");
        return EXIT_FAILURE;
    }
    if(DMABUF_GetBufferFd(handle, &fd) != IMG_SUCCESS) {
        LOG_ERROR("DMABUF export failed\n");
        DMABUF_Free(handle);
        return EXIT_FAILURE;
    }


    if(camera->importBuffer(eBuffer, fd, (IMG_UINT32)sizeBytes, isTiled,
        &id) != IMG_SUCCESS)
    {
        LOG_ERROR("DMABUF import to CI failed\n");
        DMABUF_Free(handle);
        return EXIT_FAILURE;
    }

    LOG_INFO("DMABUF handle=0x%lx fd=%d id=%d\n", handle, fd, id);

    if(buffer) {
        *buffer = handle;
    }
    if(bufferId) {
        *bufferId = id;
    }

    return IMG_SUCCESS;
}
#endif /* USE_DMABUF */

/*
 * other functions
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
    printf("%c enable AWB\n", AWB_KEY);
    printf("%c enable WBTS\n", WBTS_KEY);
    printf("%c defective pixels\n", DPF_KEY);
    printf("%c use LSH matrix if available\n", LSH_KEY);
    printf("%c adaptive TNM (needs auto TNM)\n", ADAPTIVETNM_KEY);
    printf("%c local TNM (needs auto TNM)\n", LOCALTNM_KEY);
    printf("%c auto TNM\n", AUTOTNM_KEY);
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
    printf("%c Save Display output\n", SAVE_DISPLAY_OUTPUT_KEY);
    printf("%c Save Encoder output\n", SAVE_ENCODER_OUTPUT_KEY);
    printf("%c Save DE output\n", SAVE_DE_KEY);
    printf("%c Save RAW2D output (may not work on all HW versions)\n", SAVE_RAW2D_KEY);
    printf("%c Save HDR output (may not work on all HW versions)\n", SAVE_HDR_KEY);
    printf("\t=== Test issues ===\n");
    printf("%c Insert stopCapture/StartCapture/Enqueue glitch to test HW restart procedure", TEST_GLITCH_KEY);
    printf("%c Exit\n", EXIT_KEY);

    printf("\n\n");

    Sensor_PrintAllModes(stdout);
}

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

char getch(void)
{
    static char line[2];
    if(read(0, line, 1))
    {
        return line[0];
    }
    return -1;
}

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
        LOG_ERROR("parsing parameter file \'%s' failed\n",
            config.pszFelixSetupArgsFile);
        return EXIT_FAILURE;
    }

    LoopCamera loopCam(config);
    double clockMhz = 40.0;

    if(!loopCam.camera || loopCam.camera->state == ISPC::Camera::CAM_ERROR)
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
    ISPC::ControlAE *pAE = NULL;
    ISPC::ControlAWB *pAWB = NULL;
    ISPC::ControlTNM *pTNM = NULL;
    ISPC::ControlDNS *pDNS = NULL;
    ISPC::ControlLBC *pLBC = NULL;
    ISPC::ControlAF *pAF = NULL;
    ISPC::ControlLSH *pLSH = NULL;
    ISPC::Sensor *sensor = loopCam.camera->getSensor();

    PerfControls perf;

    if ( config.controlAE )
    {
        pAE = new ISPC::ControlAE();
        loopCam.camera->registerControlModule(pAE);
        if ( config.monitorAE )
        {
            perf.registerControlForConvergenceMonitoring(pAE);
        }
    }
    if ( config.controlAWB )
    {
        pAWB = config.instantiateControlAwb(loopCam.hwMajor, loopCam.hwMinor);
        loopCam.camera->registerControlModule(pAWB);
        if ( config.monitorAWB )
        {
            perf.registerControlForConvergenceMonitoring(pAWB);
        }
    }
    if ( config.controlLSH )
    {
        if (pAWB)
        {
            pLSH = new ISPC::ControlLSH();
            loopCam.camera->registerControlModule(pLSH);
            pLSH->registerCtrlAWB(pAWB);
            pLSH->enableControl(true); // Needs to be called before load(params)
                                       // Otherwise matrix change won't be possible.
        }
        else
        {
            LOG_WARNING("ignores creation of Control LSH as AWB is not present\n");
        }
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
        if ( config.monitorAF )
        {
            perf.registerControlForConvergenceMonitoring(pAF);
        }
    }

    // load all registered controls setup
    ret = loopCam.camera->loadControlParameters(parameters);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to load control module parameters\n");
        return EXIT_FAILURE;
    }

    // get defaults from the given setup files
    {

        if ( pAE )
        {
            // no need should have been loaded from config file
            config.targetBrightness = pAE->getTargetBrightness();
            config.flickerRejection = pAE->getFlickerRejection();
            config.autoFlickerRejection = pAE->getAutoFlickerRejection();
            config.flickerRejectionFreq = pAE->getFlickerRejectionFrequency();
            config.currentAeFlicker = config.getFlickerModeFromState();
        }

        if ( pAWB )
        {
            if (config.AWBAlgorithm != DemoConfiguration::WB_PLANCKIAN)
            {
                pAWB->setCorrectionMode(
                    static_cast<ISPC::ControlAWB::Correction_Types>(
                        config.AWBAlgorithm));
                config.WBTSEnabled = false;
            }
            else
            {
                ISPC::ControlAWB_Planckian *pAWB_Plackian = static_cast<ISPC::ControlAWB_Planckian*>(pAWB);
                bool enabled;
                int stretch;
                unsigned int features=0;
                float weight;
                bool ff;

                pAWB_Plackian->getWBTSAlgorithm(enabled, weight, stretch);
                pAWB_Plackian->getFlashFiltering(ff);
                features |= ff ?
                        ISPC::ControlAWB_Planckian::WBTS_FEATURES_FF:
                        0;
                if(!config.WBTSEnabledOverride) {
                    config.WBTSEnabled = enabled;
                }
                if(!config.WBTSWeightsBaseOverride) {
                    config.WBTSWeightsBase = weight;
                }
                if(!config.WBTSTemporalStretchOverride) {
                    config.WBTSTemporalStretch = stretch;
                }
                if(!config.WBTSFeaturesOverride) {
                    config.WBTSFeatures = features;
                }

                pAWB_Plackian->setWBTSAlgorithm(config.WBTSEnabled, config.WBTSWeightsBase, config.WBTSTemporalStretch);
                pAWB_Plackian->setWBTSFeatures(config.WBTSFeatures);
            }
        }

        ISPC::ModuleDPF *pDPF = loopCam.camera->getModule<ISPC::ModuleDPF>();

        if ( config.measureAAA ) {
            LOG_Perf_Register(pAWB, config.measureVerbose);
            LOG_Perf_Register(pAF,  config.measureVerbose);
            LOG_Perf_Register(pAE,  config.measureVerbose);
        }
        if ( config.measureAWB )
        {
            LOG_Perf_Register(pAWB, config.measureVerbose);
        }
        if ( config.measureAF  )
        {
            LOG_Perf_Register(pAF , config.measureVerbose);
        }
        if ( config.measureAE  )
        {
            LOG_Perf_Register(pAE , config.measureVerbose);
        }

        if (pDPF)
        {
            config.bEnableDPF = pDPF->bWrite || pDPF->bDetect;
        }

        if (pTNM)
        {
            config.localToneMapping = pTNM->getLocalTNM();
        }

        if (pLSH)
        {
            ISPC::ModuleLSH *pLSHmod = loopCam.camera->getModule<ISPC::ModuleLSH>();

            if (pLSHmod)
            {
                config.bEnableLSH = pLSHmod->bEnableMatrix;
                pLSH->enableControl(config.bEnableLSH);
                if (config.bEnableLSH)
                {
                    // load a default matrix before we start
                    pLSH->configureDefaultMatrix();
                }
            }
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

    // initial setup
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

    if (loopCam.startCapture() != EXIT_SUCCESS)
    {
        LOG_ERROR("Failed to start the capture!\n");
        return EXIT_FAILURE;
    }
    //
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

#ifndef INFOTM_ISP
        printf("[%d/%d %02d:%02d.%02d]\n",
            tm.tm_mday, tm.tm_mon, tm.tm_hour, tm.tm_min, tm.tm_sec
        );

        printf("Frame %d - sensor %dx%d @%3.2f (mode %d - mosaic %d) - estimated FPS %3.2f (%d frames avg. with %d buffers)\n",
               frameCount,
               sensor->uiWidth, sensor->uiHeight, sensor->flFrameRate, config.sensorMode,
               (int)sensor->eBayerFormat,
            estimatedFPS, neededFPSFrames, config.nBuffers
        );

#if 1 // enable to print info about gasket at regular intervals - use same than FPS but could be different
        if(frameCount%neededFPSFrames == (neededFPSFrames-1))
        {
            loopCam.printInfo();
        }
#endif
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

            //actually "re-load"
            loopCam.camera->loadControlParameters(parameters);

            if( loopCam.camera->program() != IMG_SUCCESS )
            {
                LOG_ERROR("Failed to re-program the pipeline!\n");
                return EXIT_FAILURE;
            }

            if (loopCam.startCapture() != EXIT_SUCCESS)
            {
                LOG_ERROR("Failed to restart the capture!\n");
                return EXIT_FAILURE;
            }
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

        if(loopCam.camera->enqueueShot() !=IMG_SUCCESS)
        {
            LOG_ERROR("Failed to enqueue a shot.\n");
            return EXIT_FAILURE;
        }

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

            if (loopCam.camera->startCapture() != EXIT_SUCCESS)
            {
                LOG_ERROR("Failed to restart!\n");
                return EXIT_FAILURE;
            }


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

        ISPC::Shot shot;
        if(loopCam.camera->acquireShot(shot) != IMG_SUCCESS)
        {
            LOG_ERROR("Failed to retrieve processed shot from pipeline.\n");
            loopCam.printInfo();
            return EXIT_FAILURE;
        }

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

            // Enable/disable auto tone mapping
            case AUTOTNM_KEY:
                config.autoToneMapping = !config.autoToneMapping;
                break;

            // Enable/disable adaptive tone mapping strength control
            case ADAPTIVETNM_KEY:
                config.adaptiveToneMapping = !config.adaptiveToneMapping;
            break;

            // Enable/disable local tone mapping
            case LOCALTNM_KEY:
                config.localToneMapping = !config.localToneMapping;
            break;

            case AWB_KEY:
                config.enableAWB = !config.enableAWB;
            break;

            case WBTS_KEY:
                if ( config.AWBAlgorithm == DemoConfiguration::WB_PLANCKIAN)
                {
                    config.WBTSEnabled = config.WBTSEnabled ? false : true;
                    ISPC::ControlAWB_Planckian *pAWB_Plackian = static_cast<ISPC::ControlAWB_Planckian*>(pAWB);
                    if(pAWB_Plackian) {
                        pAWB_Plackian->setWBTSAlgorithm(config.WBTSEnabled, config.WBTSWeightsBase, config.WBTSTemporalStretch);
                    }
                }
            break;

            case AWB_RESET:
                // not doing it when AWB is enabled because it forces the CCM and not AWB state
                if ( !config.enableAWB )
                {
                    config.WBTSEnabled = false;
                    if ( config.AWBAlgorithm == DemoConfiguration::WB_PLANCKIAN)
                    {
                        ISPC::ControlAWB_Planckian *pAWB_Plackian = static_cast<ISPC::ControlAWB_Planckian*>(pAWB);
                        if(pAWB_Plackian) {
                            pAWB_Plackian->setWBTSAlgorithm(config.WBTSEnabled);
                        }
                    }
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
                config.nextAeFlickerMode();
            break;

            case SAVE_DISPLAY_OUTPUT_KEY:  //Save image to disk
                config.bSaveDisplay = true;
                LOG_INFO("save RGB key pressed...\n");
            break;

            case SAVE_ENCODER_OUTPUT_KEY:
                config.bSaveEncoder = true;
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
#ifdef INFOTM_ISP
            case FPS_KEY:
                   LOG_INFO("Frame %d - sensor %dx%d @%3.2f (mode %d - mosaic %d) - "
                       "estimated FPS %3.2f (%d frames avg. with %d buffers)\n",
                       frameCount,
                       sensor->uiWidth, sensor->uiHeight,
                       sensor->flFrameRate, config.sensorMode,
                       (int)sensor->eBayerFormat,
                        estimatedFPS, neededFPSFrames, config.nBuffers);
                break;
#endif
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

        bool bSaveResults = config.bSaveDisplay||config.bSaveEncoder||config.bSaveDE||config.bSaveHDR||config.bSaveRaw2D;
        debug("bSaveResults=%d frameError=%d\n",
            (int)bSaveResults, (int)shot.bFrameError);

        if ( bSaveResults && !shot.bFrameError )
        {
            loopCam.saveResults(shot, config);
        }

        if(bSaveOriginals)
        {
            config.loadFormats(parameters, false); // without defaults

            config.bSaveDisplay=(config.originalRGB!=PXL_NONE);
            config.bSaveEncoder=(config.originalYUV!=PXL_NONE);
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

#ifndef INFOTM_ISP
        loopCam.printInfo(shot, config, clockMhz);
#endif

        perf.updateConv();

        //Update Camera loop controls configuration
        // before the release because may use the statistics
        config.applyConfiguration(*(loopCam.camera));

        // done after apply configuration as we already did all changes we needed
        if (bSaveOriginals)
        {
            config.loadFormats(parameters, true); // reload with defaults
            bSaveOriginals=false;
        }

        if(loopCam.camera->releaseShot(shot) !=IMG_SUCCESS)
        {
            LOG_ERROR("Failed to release shot.\n");
            return EXIT_FAILURE;
        }

        frameCount++;
    }

    LOG_INFO("Exiting loop\n");
    loopCam.stopCapture();

    ISPC::LOG_Perf_Summary();

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    int ret;
    struct termios orig_term_attr;

#ifdef FELIX_FAKE
    bDisablePdumpRDW = IMG_TRUE; // no read in pdump
    ret = initializeFakeDriver(2, 0, 4096, 256, 0, 0, 0, NULL, NULL);
    if (EXIT_SUCCESS != ret)
    {
        LOG_ERROR("Failed to enable fake driver\n");
        return ret;
    }
#endif

    enableRawMode(orig_term_attr);

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
