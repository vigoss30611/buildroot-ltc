/**
*******************************************************************************
@file ISPC_hdr.cpp

@brief Run a capture with 2 HDR extraction, merge and push back as HDR
 insertion

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

#include <ispc/Camera.h>
#include <ispc/ModuleOUT.h>
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB.h>
#include <ispc/ControlAF.h>
#include <ispc/ModuleCCM.h>
#include <ispc/ModuleWBC.h>
#include <ispc/Save.h>
#include <ispc/ParameterFileParser.h>

#include <ispctest/hdf_helper.h>
#include <ispctest/info_helper.h>

#include <dyncmd/commandline.h>

#include <sensorapi/sensorapi.h>
#include <ci/ci_version.h>

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_hdr"

static void printInfo(ISPC::Camera &camera)
{
    std::cout << "*****" << std::endl;
    PrintGasketInfo(camera, std::cout);
    PrintRTMInfo(camera, std::cout);
    std::cout << "*****" << std::endl;
}

struct HDR_Capture
{
    /*
     * command line loadable parameters
     */
    IMG_BOOL8 bAE;
    IMG_BOOL8 bAWB;
    IMG_FLOAT fAWBTemperature;
    IMG_BOOL8 bAF;
    IMG_BOOL8 bSaveIntermediates;
    
    IMG_UINT uiMaxFrames; // AAA max frames
    IMG_UINT uiMaxShots; // nb of shots for each frame
    
    IMG_INT exposure0; // if AE offset, if not sensor value
    IMG_INT exposure1Ratio; // ratio such as exposure 1 = exposure 0 / ratio
    IMG_INT exposure1; // computed
    IMG_FLOAT gain0; // if AE offset, if not sensor value
    
    ISPC::ParameterList setupFile;
    
    char *pszSensorName;
    IMG_INT uiSensorID; // not really unsigned because on error is negative
    IMG_UINT uiSensorMode;
    IMG_UINT uiSensorFlip;
    IMG_UINT uiSensorFocus; // if not AF
    
    /*
     * objects
     */
    ISPC::Camera *pCamExtraction;
    ISPC::Camera *pCamInsertion;
    
    ISPC::Shot frame0;
    ISPC::Shot frame1;
    CI_BUFFER mergedFrames;
    
    /*
     * methods
     */
    
    HDR_Capture();
    ~HDR_Capture();
    
    // returns -number of erroneous parameters found (1 when help)
    int load(int argc, char *argv[]);
    
    int configureCameras();
    
    int runExtraction();
    int runMerge();
    int runInsertion();
};


HDR_Capture::HDR_Capture():
    bAE(IMG_FALSE),
    bAWB(IMG_FALSE),
    bAF(IMG_FALSE),
    bSaveIntermediates(IMG_FALSE),
    uiMaxFrames(30),
    uiMaxShots(3),
    exposure0(0),
    gain0(1.0f),
    exposure1Ratio(2.0f),
    exposure1(0),
    uiSensorFlip(SENSOR_FLIP_NONE),
    uiSensorFocus(0)
{
    pszSensorName = NULL;
    pCamExtraction = NULL;
    pCamInsertion = NULL;
}

HDR_Capture::~HDR_Capture()
{
    if(pszSensorName)
    {
        IMG_FREE(pszSensorName);
    }
    if(pCamInsertion)
    {
        delete pCamInsertion;
    }
    if(pCamExtraction)
    {
        delete pCamExtraction;
    }
}

int HDR_Capture::load(int argc, char *argv[])
{
    IMG_RESULT ret;
    IMG_BOOL8 flip[2] = {false, false}; // horizontal and vertical
    int missing = 0;
    bool bPrintHelp = false;
    char *pszFelixSetupArgsFile = NULL;
    IMG_UINT32 unregistered = 0;
    
    setupFile.clearList();
    
    ret=DYNCMD_AddCommandLine(argc, argv, "-source");
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("unable to parse command line\n");
        DYNCMD_ReleaseParameters();
        missing--;
    }
    
    /*
     * find help first to avoid printing error messages if parameters are not
     * found
     */
    ret = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND,
        "Usage information",
        NULL);
    if (RET_FOUND == ret)
    {
        bPrintHelp = true;
    }

    ret = DYNCMD_RegisterParameter("-sensor", DYNCMDTYPE_STRING, 
        "Sensor to use",
        &(this->pszSensorName));
    if (RET_FOUND != ret)
    {
        pszSensorName = NULL;
        if (!bPrintHelp)
        {
            LOG_ERROR("-sensor option not found!\n");
            missing--;
        }
    }

    ret = DYNCMD_RegisterParameter("-sensorMode", DYNCMDTYPE_UINT, 
        "[optional] Sensor running mode",
        &(this->uiSensorMode));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -sensorMode\n");
            missing--;
        }
        // parameter is optional
    }
    
    ret=DYNCMD_RegisterArray("-sensorFlip", DYNCMDTYPE_BOOL8, 
        "[optional] Set 1 to flip the sensor Horizontally and Vertically. "\
        "If only 1 option is given it is assumed to be the horizontal one",
        flip, 2);
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_INCOMPLETE == ret)
        {
            LOG_WARNING("incomplete -sensorFlip, flipping is H=%d V=%d\n", 
                (int)flip[0], (int)flip[1]);
        }
        else if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -sensorFlip\n");
            missing--;
        }
    }
    
    ret=DYNCMD_RegisterParameter("-sensorFocus", DYNCMDTYPE_UINT,
        "[optional] set the focus position. May not work on all sensors. "\
        "Can be replaced by enabling AF",
        &(this->uiSensorFocus));
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -sensorFocus\n");
            missing--;
        }
        // parameter is optional
    }
    
    ret=DYNCMD_RegisterParameter("-AE", DYNCMDTYPE_BOOL8, 
        "[optional] enable AE algorithm",
        &(this->bAE));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -AE\n");
            missing--;
        }
    }
    
    ret=DYNCMD_RegisterParameter("-AWB", DYNCMDTYPE_BOOL8, 
        "[optional] enable AWB algorithm",
        &(this->bAWB));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -AWB\n");
            missing--;
        }
    }
    
    ret=DYNCMD_RegisterParameter("-AWB_temperature", DYNCMDTYPE_FLOAT, 
        "[optional] if AWB not enabled loads correction for specified" \
        " temperature",
        &(this->fAWBTemperature));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -AWB_temperature\n");
            missing--;
        }
        else
        {
            fAWBTemperature = -1.0;
        }
    }
    
    //  add AF switch
    //  add max frames switch
    
    ret=DYNCMD_RegisterParameter("-saveIntermediates", DYNCMDTYPE_BOOL8, 
        "[optional] save intermediate HDR extraction image and HDR "\
        "insertion image for future re-use",
        &(this->bSaveIntermediates));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -saveIntermediates\n");
            missing--;
        }
    }
    
    ret=DYNCMD_RegisterParameter("-nShots", DYNCMDTYPE_UINT,
        "[optional] Number of shots to capture for each frame. Should be at least 1, "\
        "may need to be more to allow exposure/gain to settle.",
        &(this->uiMaxShots));
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -nShots\n");
            missing--;
        }
    }
    
    ret=DYNCMD_RegisterParameter("-settleFrames", DYNCMDTYPE_UINT,
        "[optional] Number of frames to run for the auto algorithms to "\
        "converge (AE, AWB, AF).",
        &(this->uiMaxFrames));
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -settleFrames\n");
            missing--;
        }
    }
    
    
    ret=DYNCMD_RegisterParameter("-HDR0_exposure", DYNCMDTYPE_INT,
        "Value in us for 1st frame. "\
        "[optional] If using AE offset of computed exposure. "\
        "[mandatory] If not using AE actual exposure value.",
        &(this->exposure0));
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -HDR0_exposure\n");
            missing--;
        }
        else if (!bAE)
        {
            LOG_ERROR("-HDR0_exposure is mandatory when AE is off\n");
            missing--;
        }
    }
    
    ret=DYNCMD_RegisterParameter("-HDR0_gain", DYNCMDTYPE_FLOAT,
        "[optional] Initial gain for the capture of frames. Also affects "\
        "frame 1",
        &(this->gain0));
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -HDR0_gain\n");
            missing--;
        }
        else
        {
            if (bAE) // assumes it has been loaded beforehand!
            {
                gain0 = 0.0f;
            }
        }
    }
    
    ret=DYNCMD_RegisterParameter("-HDR1_ratio", DYNCMDTYPE_INT,
        "Ratio to compute exposure of second frame such as E1 = E0/ratio.",
        &(this->exposure1Ratio));
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -HDR1_ratio\n");
        }
        else
        {
            LOG_ERROR("parameter -HDR1_ratio is mandatory\n");
        }
        missing--;
    }
    
    /*ret=DYNCMD_RegisterParameter("-HDR1_gain", DYNCMDTYPE_FLOAT,
        "[optional] Value in us for 2nd frame. "\
        "If using AE offset of discovered exposure. "\
        "If not using AE actual gain value.",
        &(this->gain1));
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -HDR1_gain\n");
            missing--;
        }
        else
        {
            if (bAE) // assumes it has been loaded beforehand!
            {
                gain1 = 0.0f;
            }
        }
    }*/
    
    ret=DYNCMD_RegisterParameter("-setupFile", DYNCMDTYPE_STRING, 
            "FelixSetupArgs file to use to configure the Pipeline", 
            &(pszFelixSetupArgsFile));
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
        missing--;
    }
    
    /*
     * now convert some information
     */
    
    this->uiSensorFlip = SENSOR_FLIP_NONE;
    if(flip[0])
    {
        this->uiSensorFlip |= SENSOR_FLIP_HORIZONTAL;
    }
    if(flip[1])
    {
        this->uiSensorFlip |= SENSOR_FLIP_VERTICAL;
    }
    
    if (pszFelixSetupArgsFile && !bPrintHelp)
    {
        this->setupFile =
            ISPC::ParameterFileParser::parseFile(pszFelixSetupArgsFile);
        LOG_INFO("Loading parameters from '%s'\n", pszFelixSetupArgsFile);
        IMG_FREE(pszFelixSetupArgsFile);
        pszFelixSetupArgsFile = NULL;
    }
        
    if (pszSensorName && !bPrintHelp)
    {
        this->uiSensorID = ISPC::Sensor::GetSensorId(pszSensorName);
        if (uiSensorID < 0 && !bPrintHelp)
        {
            LOG_ERROR("Sensor '%s' not found\n", pszSensorName);
            missing--;
        }
    }
    
    if (uiMaxShots < 1)
    {
        LOG_ERROR("Number of shots per frame has to be at least 1\n");
        missing--;
    }
    
    if (bPrintHelp || missing != 0)
    {
        DYNCMD_PrintUsage();
        Sensor_PrintAllModes(stdout);
        if(bPrintHelp)
        {
            missing=1;
        }
    }
    
    DYNCMD_HasUnregisteredElements(&unregistered);

    DYNCMD_ReleaseParameters();
    
    return missing;
}

int HDR_Capture::configureCameras()
{
    IMG_RESULT ret;
    ISPC::ModuleOUT *pOUT = 0;
    ISPC::ControlAE *pAE = 0;
    ISPC::ControlAWB *pAWB = 0;
    ISPC::ControlAF *pAF = 0;
    
    if(pCamExtraction || pCamInsertion)
    {
        LOG_ERROR("pCamExtraction or pCamInsertion already created!\n");
        return EXIT_FAILURE;
    }
    
    /*
     * 1. setup extraction camera
     */
    
    LOG_INFO("Configuring extraction camera object\n");
#ifdef INFOTM_ISP
    this->pCamExtraction = new ISPC::Camera(0, uiSensorID, uiSensorMode,
        uiSensorFlip, 0);
#else
    this->pCamExtraction = new ISPC::Camera(0, uiSensorID, uiSensorMode,
            uiSensorFlip);
#endif
    if (ISPC::Camera::CAM_ERROR == pCamExtraction->state)
    {
        LOG_ERROR("failed to create extraction camera\n");
        return EXIT_FAILURE;
    }
    
    if (bAE)
    {
        pAE = new ISPC::ControlAE();
        
        pCamExtraction->registerControlModule(pAE);
        pAE->enableControl(true);
    }
    pAWB = new ISPC::ControlAWB();
    pCamExtraction->registerControlModule(pAWB);
    pAWB->setCorrectionMode(ISPC::ControlAWB::WB_COMBINED);
    pAWB->enableControl(bAWB);
    
    //if (bAF)
    //{
    //    pAF = new ISPC::ControlAF();
        
    //    pCamExtraction->registerControlModule(pAF);
    //    pAF->enableControl(true);
    //}
    
    ret = ISPC::CameraFactory::populateCameraFromHWVersion(*pCamExtraction,
        pCamExtraction->getSensor());
    if (IMG_SUCCESS != ret || ISPC::Camera::CAM_ERROR == pCamExtraction->state)
    {
        LOG_ERROR("failed to create extraction camera object\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamExtraction->loadParameters(setupFile);
    
    pOUT = pCamExtraction->getModule<ISPC::ModuleOUT>();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to load setup file!\n");
        return EXIT_FAILURE;
    }
    
    if ( !pOUT )
    {
        LOG_ERROR("extraction camera is not initialised with a OUT "\
            "instance!\n");
        return EXIT_FAILURE;
    }
    
    pOUT->encoderType = PXL_NONE;
    pOUT->displayType = PXL_NONE;
    pOUT->dataExtractionType = PXL_NONE;
    pOUT->hdrExtractionType = BGR_101010_32;
    pOUT->hdrInsertionType = PXL_NONE;
    pOUT->raw2DExtractionType = PXL_NONE;
    
    pOUT = 0;
    
    if (!bAWB && fAWBTemperature > 0)
    {
        ISPC::ModuleCCM *pCCM = pCamExtraction->getModule<ISPC::ModuleCCM>();
        ISPC::ModuleWBC *pWBC = pCamExtraction->getModule<ISPC::ModuleWBC>();
        const ISPC::TemperatureCorrection &tempCorr =
            pAWB->getTemperatureCorrections();
        ISPC::ColorCorrection currentCCM =
            tempCorr.getColorCorrection(fAWBTemperature);
        
        LOG_INFO("Applying AWB correction %fK\n", fAWBTemperature);
        
        for(int i=0;i<3;i++)
        {
            for(int j=0;j<3;j++)
            {
                pCCM->aMatrix[i*3+j] = currentCCM.coefficients[i][j];
            }
        }
        
        pCCM->aOffset[0] = currentCCM.offsets[0][0];
        pCCM->aOffset[1] = currentCCM.offsets[0][1];
        pCCM->aOffset[2] = currentCCM.offsets[0][2];
        
        pWBC->aWBGain[0] = currentCCM.gains[0][0];
        pWBC->aWBGain[1] = currentCCM.gains[0][1];
        pWBC->aWBGain[2] = currentCCM.gains[0][2];
        pWBC->aWBGain[3] = currentCCM.gains[0][3];
    }
        
    ret = pCamExtraction->setupModules();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to setup extraction camera\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamExtraction->program();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to program extraction camera\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamExtraction->allocateBufferPool(uiMaxShots+1);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to allocate %d buffers for extraction camera!\n",
            uiMaxShots+1);
        return EXIT_FAILURE;
    }
    
    /*
     * 2. setup insertion camera
     * same HW context but not started at the same time
     */
    
    LOG_INFO("Configuring insertion camera object\n");
    pCamInsertion = new ISPC::Camera(0, pCamExtraction->getSensor());
    // not owning the camera and therefore not having algorithms
    
    if (ISPC::Camera::CAM_ERROR == pCamInsertion->state)
    {
        LOG_ERROR("failed to create extraction camera\n");
        return EXIT_FAILURE;
    }
    
    ret = ISPC::CameraFactory::populateCameraFromHWVersion(*pCamInsertion,
        pCamExtraction->getSensor());
    if (IMG_SUCCESS != ret || ISPC::Camera::CAM_ERROR == pCamInsertion->state)
    {
        LOG_ERROR("failed to create insertion camera object\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamInsertion->loadParameters(setupFile);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to load setup parameters!\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamInsertion->loadControlParameters(setupFile);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to load control parameters!\n");
        return EXIT_FAILURE;
    }
        
    pOUT = pCamInsertion->getModule<ISPC::ModuleOUT>();
    
    if ( !pOUT )
    {
        LOG_ERROR("insertion camera is not initialised with a OUT "\
            "instance!\n");
        return EXIT_FAILURE;
    }
    
    // ensure HDR insertion is enabled
    pOUT->hdrInsertionType = BGR_161616_64;
    
    if (PXL_NONE != pOUT->dataExtractionType)
    {
        LOG_WARNING("disabling Data-Extraction for HDR insertion camera\n");
        pOUT->dataExtractionType = PXL_NONE;
    }
    if (PXL_NONE != pOUT->hdrExtractionType)
    {
        LOG_WARNING("disabling HDR extraction for HDR insertion camera\n");
        pOUT->hdrExtractionType = PXL_NONE;
    }
    if (PXL_NONE != pOUT->raw2DExtractionType)
    {
        LOG_WARNING("disabling Raw2D extraction for HDR insertion camera\n");
        pOUT->raw2DExtractionType = PXL_NONE;
    }
    
    if (PXL_NONE == pOUT->encoderType &&
        PXL_NONE == pOUT->displayType)
    {
        LOG_ERROR("loaded configuration file has no output setup after the "\
            "HDF module!\n");
        return EXIT_FAILURE;
    }
    
    pOUT = 0;
    
    ret = pCamInsertion->setupModules(); 
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to setup extraction camera\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamInsertion->program();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to program extraction camera\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamInsertion->allocateBufferPool(1);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to allocate %d buffers for extraction camera!\n",
            1);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

int HDR_Capture::runExtraction()
{
    ISPC::Sensor *pSensor = 0;
    IMG_RESULT ret;
    float gain1 = 0;
    
    if (!pCamExtraction)
    {
        LOG_ERROR("extraction camera is not created!\n");
        return EXIT_FAILURE;
    }
    
    pSensor = pCamExtraction->getSensor();
    
    
    ret = pCamExtraction->startCapture();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("extraction camera failed to start\n");
        return EXIT_FAILURE;
    }
    
    if ( bAE || bAWB || bAF )
    {
        ISPC::ControlAE *pAE =
            pCamExtraction->getControlModule<ISPC::ControlAE>();
        ISPC::ControlAWB *pAWB =
            pCamExtraction->getControlModule<ISPC::ControlAWB>();
        //ISPC::ControlAF *pAF =
        //    pCamExtraction->getControlModule<ISPC::ControlAF>();
        int s, b;
        ISPC::Shot tmp;
        
        //if (pAF)
        //{
        //    pAF->setCommand(ISPC::AF_TRIGGER);
        //}
        
        LOG_INFO("Running algorithms for %d frames\n", uiMaxFrames);
        
        // dummy code - does not check if converged just run for given frames
        ret = IMG_SUCCESS;
        for (s = 0 ; s < uiMaxFrames && IMG_SUCCESS == ret ; s++)
        {
            ret = pCamExtraction->enqueueShot();
            
            ret = pCamExtraction->acquireShot(tmp);
            pCamExtraction->releaseShot(tmp);
        }
        
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to run frame %d/%d\n", s, uiMaxFrames);
            printInfo(*pCamExtraction);
            return EXIT_FAILURE;
        }
        
        // once converged turn them off
        if (pAE)
        {
            unsigned long exp = pSensor->getExposure();
            double ga = pSensor->getGain();
            pAE->enableControl(false);
            
            exposure0 = exp + exposure0;
            if (gain0 >= 1.0f)
            {
                gain0 = gain0;
            }
            
            //exposure1 = exp + exposure1;
            //gain1 = ga + gain1;
            
            LOG_INFO("AE provided %luus x%lf - capturing with %luus x%lf\n",
                exp, ga, exposure0, gain0);
        }
        if (pAWB && bAWB)
        {
            LOG_INFO("AWB correction temperature: %lf\n",
                pAWB->getCorrectionTemperature());
            pAWB->enableControl(false);
        }
        //if (pAF)
        //{
        //    pAF->enableControl(false);
        //}
    }
    
    exposure1 = exposure0 / exposure1Ratio;
    gain1 = gain0;
    
    LOG_INFO("Frame0: %uus x%f - Frame1 %uus %f (ratio=1/%d) "\
        "- %d shots each\n",
        exposure0, gain0, exposure1, gain1, exposure1Ratio, uiMaxShots);
    
    
    ret = pSensor->setExposure(this->exposure0);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set exposure 0: %u\n", this->exposure0);
        return EXIT_FAILURE;
    }
    
    ret = pSensor->setGain(this->gain0);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set gain 0: %f\n", this->gain0);
        return EXIT_FAILURE;
    }
    
    ISPC::Shot tmp;
    int s;
    for ( s = 0 ; s < uiMaxShots ; s++ )
    {
        // enqueue several shots to be able to choose
        ret = pCamExtraction->enqueueShot();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("extraction camera failed to trigger frame 0 "\
                "shot %d/%d\n", s+1, uiMaxShots);
            printInfo(*pCamExtraction);
            return EXIT_FAILURE;
        }
    }
    
    // dummy algorithm: take the last one
    for ( s = 0 ; s < uiMaxShots-1 ; s++ )
    {
        ret = pCamExtraction->acquireShot(tmp);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("extraction camera failed to acquire frame 0 "\
                "shot %d/%d\n", s+1, uiMaxShots);
            printInfo(*pCamExtraction);
            return EXIT_FAILURE;
        }
        pCamExtraction->releaseShot(tmp);
    }
    
    ret = pCamExtraction->acquireShot(frame0);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("extraction camera failed to acquire frame 0\n");
        printInfo(*pCamExtraction);
        return EXIT_FAILURE;
    }
    // if flicker was avoided or min/max applied
    exposure0 = pSensor->getExposure();
    gain0 = pSensor->getGain();
    
    LOG_DEBUG("frame1 with exposure %u\n", exposure1);
    ret = pSensor->setExposure(exposure1);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set exposure 1: %u\n", exposure1);
        return EXIT_FAILURE;
    }
    
    ret = pSensor->setGain(gain1);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to set gain 1: %f\n", gain1);
        return EXIT_FAILURE;
    }
    
    for ( s = 0 ; s < uiMaxShots ; s++ )
    {
        // enqueue several shots to be able to choose
        ret = pCamExtraction->enqueueShot();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("extraction camera failed to trigger frame 1 "\
                "shot %d/%d\n", s+1, uiMaxShots);
            printInfo(*pCamExtraction);
            return EXIT_FAILURE;
        }
    }
    
    // dummy algorithm: take the last one
    for ( s = 0 ; s < uiMaxShots-1 ; s++ )
    {
        ret = pCamExtraction->acquireShot(tmp);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("extraction camera failed to acquire frame 0 "\
                "shot %d/%d\n", s+1, uiMaxShots);
            printInfo(*pCamExtraction);
            return EXIT_FAILURE;
        }
        pCamExtraction->releaseShot(tmp);
    }
    
    ret = pCamExtraction->acquireShot(frame1);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("extraction camera failed to acquire frame 1\n");
        printInfo(*pCamExtraction);
        return EXIT_FAILURE;
    }
    // if flicker was avoided or min/max applied
    exposure1 = pSensor->getExposure();
    gain1 = pSensor->getGain();
    
    LOG_DEBUG("stopping extraction camera\n");
    ret = pCamExtraction->stopCapture();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("extraction camera failed to stop\n");
        printInfo(*pCamExtraction);
        return EXIT_FAILURE;
    }
    
    if (this->bSaveIntermediates)
    {
        std::ostringstream str;
        
        str.str("");
        str << "frame0_" << this->exposure0 << "us_x" << this->gain0 << ".flx";
        
        ret = ISPC::Save::Single(*(pCamExtraction->getPipeline()), frame0,
            ISPC::Save::RGB_EXT, str.str());
        if (IMG_SUCCESS != ret)
        {
            LOG_WARNING("failed to save frame 0\n");
        }
        else
        {
            LOG_INFO("intermediate file for frame0 saved to '%s'\n",
                str.str().c_str());
        }
        
        str.str("");
        str << "frame1_" << exposure1 << "us_x" << gain1 << ".flx";
        
        ret = ISPC::Save::Single(*(pCamExtraction->getPipeline()), frame1,
            ISPC::Save::RGB_EXT, str.str());
        if (IMG_SUCCESS != ret)
        {
            LOG_WARNING("failed to save frame 1\n");
        }
        else
        {
            LOG_INFO("intermediate file for frame1 saved to '%s'\n",
                str.str().c_str());
        }
    }
    
    return EXIT_SUCCESS;
}



int HDR_Capture::runMerge()
{
    IMG_RESULT ret;
    
    if (!pCamInsertion)
    {
        LOG_ERROR("insertion camera not setup!\n");
        return EXIT_FAILURE;
    }
    
    LOG_INFO("Running frame merging\n");
    
    ret = pCamInsertion->getPipeline()->getAvailableHDRInsertion(mergedFrames);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to get available HDR insertion buffer\n");
        return EXIT_FAILURE;
    }
    
    if (HDRLIBS_Supported())
    {
        HDRMergeInput f0(*pCamInsertion->getPipeline(), frame0.HDREXT),
            f1(*pCamInsertion->getPipeline(), frame1.HDREXT);
        
        f0.exposure = exposure0;
        f0.gain = gain0;
        
        f1.exposure = exposure1;
        f1.gain = gain0;
        
        ret = HDRLIBS_MergeFrames(f0, f1, &mergedFrames);
    }
    else
    {
        ret = AverageHDRMerge(frame0.HDREXT, frame1.HDREXT, &mergedFrames);
    }
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to merge the 2 frames\n");
        return EXIT_FAILURE;
    }
    
    /*
     * frames are not needed - release them
     */
    pCamExtraction->releaseShot(frame0);
    pCamExtraction->releaseShot(frame1);
    
    if (this->bSaveIntermediates)
    {
        std::ostringstream str;
        
        str << "hdr_input.flx";
        
        ret = ISPC::Save::SingleHDF(*pCamInsertion->getPipeline(),
            mergedFrames, str.str());
        if (IMG_SUCCESS != ret)
        {
            LOG_WARNING("failed to save merged frame\n");
        }
        else
        {
            LOG_INFO("merged frame saved to '%s'\n",
                str.str().c_str());
        }
    }
    
    return EXIT_SUCCESS;
}

int HDR_Capture::runInsertion()
{
    IMG_RESULT ret;
    ISPC::Shot hdr;
    
    if (!pCamInsertion)
    {
        LOG_ERROR("insertion camera not setup!\n");
        return EXIT_FAILURE;
    }
    
    LOG_INFO("Starting insertion camera\n");
    ret = pCamInsertion->startCapture();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("insertion camera failed to start\n");
        return EXIT_FAILURE;
    }
    
    CI_BUFFID ids;
    
    ret = pCamInsertion->getPipeline()->getFirstAvailableBuffers(ids);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get first available buffers from "\
            "insertion camera\n");
        return EXIT_FAILURE;
    }
    
    ids.idHDRIns = mergedFrames.id;
    
    ret = pCamInsertion->enqueueSpecifiedShot(ids);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("insertion camera failed to enqueue shot\n");
        printInfo(*pCamInsertion);
        return EXIT_FAILURE;
    }
    
    // start the sensor so that HDR frame is replaced
    
    // assumes that configure is not needed
    LOG_DEBUG("re-starting sensor to trigger the capture\n");
    ret = pCamExtraction->getSensor()->enable();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to restart sensor\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamInsertion->acquireShot(hdr);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("insertion camera failed to acquire shot\n");
        printInfo(*pCamInsertion);
        return EXIT_FAILURE;
    }
    
    ret = pCamExtraction->getSensor()->disable();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("sensor failed to stop\n");
        return EXIT_FAILURE;
    }
    
    ret = pCamInsertion->stopCapture();
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("insertion camera failed to stop\n");
        return EXIT_FAILURE;
    }
    
    int saved = 0;
    std::ostringstream str;
    const ISPC::Pipeline &pipeline = *pCamInsertion->getPipeline();

    if (PXL_NONE != hdr.DISPLAY.pxlFormat)
    {
        str.str("");
        str << "hdr_display.flx";
        
        ret = ISPC::Save::Single(pipeline, hdr,
            ISPC::Save::Display, str.str());
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to save RGB output!\n");
        }
        else
        {
            LOG_INFO("Display output saved to '%s'\n", str.str().c_str());
            saved++;
        }
    }
    if (PXL_NONE != hdr.YUV.pxlFormat)
    {
        str.str("");
        str << "hdr_" << ISPC::Save::getEncoderFilename(pipeline);
        
        ret = ISPC::Save::Single(*pCamInsertion->getPipeline(), hdr,
            ISPC::Save::YUV, str.str());
        if (IMG_SUCCESS != ret)
        {
            LOG_INFO("Encoder output saved to '%s'\n", str.str().c_str());
            LOG_ERROR("failed to save YUV output!\n");
        }
        else
        {
            saved++;
        }
    }
    if(0==saved)
    {
        LOG_WARNING("no output saved (neither display nor encoder output "\
            "enabled\n");
    }
    
    pCamInsertion->releaseShot(hdr);
    
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    LOG_INFO("CHANGELIST %s - DATE %s\n", CI_CHANGELIST_STR, CI_DATE_STR);
    
    {
        HDR_Capture hdr;
        int ret;
        
        ret = hdr.load(argc, argv);
        if (ret != 0)
        {
            if (ret < 0)
                return EXIT_FAILURE;
            return EXIT_SUCCESS; // help was invoked
        }
        
        ret = hdr.configureCameras();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to configure the camera\n");
            return EXIT_FAILURE;
        }
        
        ret = hdr.runExtraction();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to run extraction\n");
            return EXIT_FAILURE;
        }
        
        ret = hdr.runMerge();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to merge frames\n");
            return EXIT_FAILURE;
        }
        
        ret = hdr.runInsertion();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to insert merged frame\n");
            return EXIT_FAILURE;
        }
    }
    // HDR_Capture object deleted here
    
    return EXIT_SUCCESS;
}
