/**
*******************************************************************************
@file ISPC_capture.cpp

@brief Run a capture of RAW format for a given number of frames

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
#include <ispc/Save.h>
#include <ispctest/info_helper.h>
#include <dyncmd/commandline.h>
#include <sensorapi/sensorapi.h>
#include <ci/ci_api.h> // to get gasket info
#include <ci/ci_version.h>

// include this if special AR330 modes need to be supported
#include <sensors/ar330.h>
#ifdef INFOTM_ISP
//#define SPECIAL_AR330
#endif //#INFOTM_ISP

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_capture"

#define CAPTURE_INFO "shot_info.txt"
#define EXP_LBL "%lu"
#define GAIN_LBL "%2.4lf"
#define FOCUS_LBL "%u"
#define CLOCK_LBL "%.2lf"
#define FPS_LBL "%.2lf"

/* name of the sensor from DYNCMD */
static char *pszSensor = NULL;
/* name of the output file from DYNCMD */
static char *pszDEFile = NULL;
/* saves exposure information for every frame only if AE enabled */
static unsigned long *frameExposureInfo = 0; 
/* saves exposure information for every frame only if AE enabled */
static double *frameGainInfo = 0;
/* stores the frames when running */
static ISPC::Shot *frames = 0;

#ifdef SPECIAL_AR330
/* load registers from there */
char *specialAR330Registers = NULL;
#endif

static void cleanHeap()
{
    if (frames) delete [] frames;
    if (frameExposureInfo) delete [] frameExposureInfo;
    if (frameGainInfo) delete [] frameGainInfo;
    if (pszSensor) free(pszSensor);
    if (pszDEFile) free(pszDEFile);
#ifdef SPECIAL_AR330
    if (specialAR330Registers) free(specialAR330Registers);
#endif
}

static void printInfo(ISPC::Camera &camera)
{
    std::cout << "*****" << std::endl;
    PrintGasketInfo(camera, std::cout);
    PrintRTMInfo(camera, std::cout);
    std::cout << "*****" << std::endl;
}

static IMG_RESULT saveShot(const ISPC::Shot &shot, int frame, 
    ISPC::Save &file, bool bEnableAE, bool bSaveErroneous, FILE *stats)
{
    IMG_RESULT ret = IMG_SUCCESS;
    if (bEnableAE)
    {
        printf("      exposure="EXP_LBL" gains="GAIN_LBL"\n",
            frameExposureInfo[frame],
            frameGainInfo[frame]);
    }
    
    if (file.isOpened())
    {
        if (shot.bFrameError)
        {
            LOG_WARNING("frame %d is erroneous - %s\n",
                frame, bSaveErroneous ? "saved anyway" : "skipped"
            );
        }
        
        if ((!shot.bFrameError || bSaveErroneous))
        {
            ret = file.save(shot);
        }
    }
    
    if(stats)
    {
        fprintf(stats, "%d: %u "EXP_LBL"us "GAIN_LBL" %d %d",
            frame, shot.metadata.timestamps.ui32StartFrameIn,
            frameExposureInfo[frame], frameGainInfo[frame],
            (int)shot.bFrameError, shot.iMissedFrames);
        
        fprintf(stats, "\n");
    }
    
    return ret;
}

static int acquireShot(ISPC::Camera &camera, ISPC::Shot &shot, int frame,
    int uiNFrames, unsigned int &nErroneous, unsigned int &nMissed,
    double clockMhz)
{
    IMG_RESULT ret = IMG_SUCCESS;
    static int lastTimestamps = 0;
    int currTimestamps = 0;
    int TSDiff = 0;
 
    ret = camera.acquireShot(shot); 
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to get shot %d/%d\n",
            frame, uiNFrames);

        printInfo(camera);
        
        return -1;
    }
    
    currTimestamps = shot.metadata.timestamps.ui32StartFrameIn;
    printf("Shot %d/%d TS StartIn %u\n", 
            frame+1, uiNFrames, currTimestamps);
    
    if(lastTimestamps!=0)
    {
        TSDiff = currTimestamps - lastTimestamps;
        printf("      Difference from last %u tick (FPS ~"FPS_LBL" "\
            "with "CLOCK_LBL"Mhz clock)\n",
        TSDiff,
        (clockMhz*1000000.0)/(TSDiff),
        clockMhz);
    }
    lastTimestamps = currTimestamps;
        
    if(shot.bFrameError)
    {
        printf("      Frame is erroneous\n");
        nErroneous++;
    }
    if(shot.iMissedFrames>0)
    {
        printf("      Missed %d frames (from gasket counter)\n",
            shot.iMissedFrames);
        nMissed += shot.iMissedFrames;
    }
    
    
    return TSDiff;
}

int main(int argc, char *argv[])
{
    int ret=RET_FOUND;
    /* will become number of frames to capture if not concurrent */
    IMG_UINT uiNBuffers = 2;
    /* number of shots to have ready in the linked list
     * will pre-submit uiNShots-1 */
    IMG_UINT uiNShots = 2;
    IMG_UINT uiNFrames = 0;
    IMG_UINT uiDEBitdepth = 10;
    IMG_UINT uiSensorMode = 0;
    IMG_UINT uiSensorExposure = 0;
    IMG_FLOAT flSensorGain = 1.0;
    int missing = 0; // missing parameters - if >0 stops before running
    IMG_UINT32 unregistered;
    IMG_BOOL bConcurrent = IMG_FALSE;
    IMG_BOOL bEnableAE = IMG_FALSE;
    IMG_BOOL bSaveErroneous = IMG_FALSE;
    IMG_FLOAT fTargetBrightness = 0.0f;
    IMG_UINT uiSensorFocus = 0;
    bool saveOutput = true;
    bool bPrintHelp = false;
    
    IMG_BOOL8 flip[2] = {false, false}; // horizontal and vertical

    LOG_INFO("CHANGELIST %s - DATE %s\n", CI_CHANGELIST_STR, CI_DATE_STR);

    //
    //command line parameter loading:
    //

    ret=DYNCMD_AddCommandLine(argc, argv, "-source");
    if (ret != IMG_SUCCESS)
    {
        LOG_ERROR("unable to parse command line\n");
        DYNCMD_ReleaseParameters();
        missing++;
    }
    
    ret = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND,
        "Usage information",
        NULL);
    if (RET_FOUND == ret)
    {
        bPrintHelp = true;
    }

    ret = DYNCMD_RegisterParameter("-sensor", DYNCMDTYPE_STRING, 
        "Sensor to use",
        &(pszSensor));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        pszSensor = NULL;
        LOG_ERROR("-sensor option not found!\n");
        missing++; // mandatory parameter
    }

    ret = DYNCMD_RegisterParameter("-sensorMode", DYNCMDTYPE_UINT, 
        "Sensor running mode",
        &(uiSensorMode));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -sensorMode\n");
            missing++;
        }
        // parameter is optional
    }

    ret=DYNCMD_RegisterParameter("-sensorExposure", DYNCMDTYPE_UINT,
        "[optional] initial exposure to use in us", 
        &(uiSensorExposure));
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
        &(flSensorGain));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -sensorGain\n");
            missing++;
        }
        // parameter is optional
    }
    
    ret=DYNCMD_RegisterArray("-sensorFlip", DYNCMDTYPE_BOOL8, 
        "[optional] Set 1 to flip the sensor Horizontally and Vertically. "\
        "If only 1 option is given it is assumed to be the horizontal one\n",
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
            missing++;
        }
        // parameter is optional
    }
    
    ret=DYNCMD_RegisterParameter("-sensorFocus", DYNCMDTYPE_UINT,
        "[optional] set the focus position. May not work on all sensors.\n",
        &uiSensorFocus);
    if(RET_FOUND != ret && !bPrintHelp)
    {
        if(RET_NOT_FOUND != ret)
        {
            LOG_ERROR("while parsing parameter -sensorFocus\n");
            missing++;
        }
        // parameter is optional
    }

    ret=DYNCMD_RegisterParameter("-nFrames", DYNCMDTYPE_UINT, 
        "number of frames to capture",
        &(uiNFrames));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -nFrames\n");
        }
        else
        {
            LOG_ERROR("-nFrames option not found!\n");
        }
        missing++; // mandatory parameter
    }
    
    // called -nBuffers like in other apps but populates uiNShots!
    ret=DYNCMD_RegisterParameter("-nBuffers", DYNCMDTYPE_UINT, 
        "[optional] number of buffers to push to the HW - min 1, max 16",
        &(uiNShots));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -nBuffers\n");
            missing++;
        }
        // parameter is optional
    }
    if((uiNShots<=0 || uiNShots>16) && !bPrintHelp)
    {
        LOG_ERROR("-nBuffers %d isn't valid - should be between 1 and %d\n",
            uiNShots, 16);
        missing++;
    }
    
    ret = DYNCMD_RegisterParameter("-DEBitDepth", DYNCMDTYPE_UINT,
        "[optional] data extraction bitdepth to use (8,10,12)",
        &(uiDEBitdepth));
    if (RET_FOUND != ret)
    {
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -DEBitDepth\n");
            missing++;
        }
        // parameter is optional
    }

    ret = DYNCMD_RegisterParameter("-output", DYNCMDTYPE_STRING,
        "[optional] name of the output file",
        &(pszDEFile));
    if (RET_FOUND != ret)
    {
        pszDEFile = strdup("dataExtraction.flx");
    }
    
    ret = DYNCMD_RegisterParameter("-concurrent", DYNCMDTYPE_COMMAND,
        "[optional] saves the frame while they are captured with double "\
        "buffering - WARNING: may be slow on FPGA system with high memory "\
        "latency access",
        NULL);
    if (RET_FOUND == ret)
    {
        bConcurrent = IMG_TRUE;
    }
    
    ret = DYNCMD_RegisterParameter("-enableAE", DYNCMDTYPE_FLOAT,
        "[optional] enable the Auto Exposure algorithm and sets its target "\
        "brightness",
        &fTargetBrightness);
    if (RET_FOUND != ret)
    {
        if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("expecting target brightness as a float after "\
            "-enableAE\n");
            missing++;
        }
        // parameter is optional
    }
    else
    {
        bEnableAE = IMG_TRUE;
    }
    
    ret = DYNCMD_RegisterParameter("-no_write", DYNCMDTYPE_COMMAND,
        "[optional] do not write output to disk (faster) but accumulate "\
        "statistics to disk",
        NULL);
    if (RET_FOUND == ret)
    {
        saveOutput=false;
    }

#ifdef SPECIAL_AR330
    ret = DYNCMD_RegisterParameter("-ar330_registers", DYNCMDTYPE_STRING,
        "[optional] if using AR330 and specified will override the mode to "\
        "be the special mode and load the registers from this file",
        &(specialAR330Registers));
    if (RET_FOUND != ret)
    {
        specialAR330Registers = NULL;
        if (RET_NOT_FOUND != ret && !bPrintHelp)
        {
            LOG_ERROR("expecting a string after -ar330_registers\n");
            missing++;
        }
        // parameter is optional
    }
    else
    {
        if (pszSensor && strncmp(pszSensor, AR330_SENSOR_INFO_NAME,
                strlen(AR330_SENSOR_INFO_NAME)) == 0)
        {
            LOG_INFO("Replacing mode %u to special mode %u\n",
                uiSensorMode, AR330_SPECIAL_MODE);
            uiSensorMode = AR330_SPECIAL_MODE;
        }
        else
        {
            LOG_INFO("wrong sensor - do not use specified ar330 register "\
                "file\n");
            free(specialAR330Registers);
            specialAR330Registers = NULL;
        }
        
    }
#endif
    
    /*
     * must be last action
     */
    if (bPrintHelp || missing > 0)
    {
        DYNCMD_PrintUsage();
        Sensor_PrintAllModes(stdout);
        missing++;
    }

    DYNCMD_HasUnregisteredElements(&unregistered);

    DYNCMD_ReleaseParameters();

    if (missing > 0)
    {
        cleanHeap();
        return EXIT_SUCCESS;
    }
    
    if ( !bConcurrent )
    {
        uiNBuffers = uiNFrames;
    }
    
    uiNShots = IMG_MIN_INT(uiNShots, uiNBuffers);
    LOG_INFO("Using %u shots in the HW linked list\n", uiNShots);

    // create a context for the driver connection
    {
        //
        // configure the pipeline
        //
        int flipping = (flip[0]?SENSOR_FLIP_HORIZONTAL:SENSOR_FLIP_NONE)
            | (flip[1]?SENSOR_FLIP_VERTICAL:SENSOR_FLIP_NONE);
#ifdef SPECIAL_AR330
        // when using special mode we need to create the sensor beforehand
        // so that we can load registers and apply them
        // which is not possible when creating the Camera with a sensor mode
        ISPC::Sensor *sensor = new ISPC::Sensor(
            ISPC::Sensor::GetSensorId(pszSensor));

        if (specialAR330Registers)
        {
            ret = AR330_LoadSpecialModeRegisters(sensor->getHandle(),
                specialAR330Registers);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("Failed to load ar330 registers!\n");
                delete sensor;
                cleanHeap();
                return EXIT_FAILURE;
            }
        }

        ret = sensor->configure(uiSensorMode, flipping);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to configure mode %u\n", uiSensorMode);
            delete sensor;
            cleanHeap();
            return EXIT_FAILURE;
        }

        ISPC::Camera camera(0, sensor);
        // the camera will delete the sensor when deleted
        camera.setOwnSensor(true);
#else
#ifdef INFOTM_ISP
        ISPC::Camera camera(0, ISPC::Sensor::GetSensorId(pszSensor),
            uiSensorMode, flipping, 0);
#else
        ISPC::Camera camera(0, ISPC::Sensor::GetSensorId(pszSensor),
                   uiSensorMode, flipping);
#endif
        ISPC::Sensor *sensor = camera.getSensor();
#endif
        ISPC::ParameterList parameters;
        ISPC::ControlAE *pAE = new ISPC::ControlAE();
        
        
        camera.registerControlModule(pAE);
        
        LOG_DEBUG("Enable AE %d\n", bEnableAE);
        if ( bEnableAE )
        {
            pAE->enableControl();
            pAE->setTargetBrightness(fTargetBrightness);
            // @ set flicker rejection for AE
        }
        else
        {
            pAE->enableControl(false);
        }
        
        ISPC::CameraFactory::populateCameraFromHWVersion(camera, sensor);

        if(ISPC::Camera::CAM_ERROR == camera.state)
        {
            LOG_ERROR("failed to create camera object\n");
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        // loads defaults because the file is empty
        camera.loadParameters(parameters);
        
        ISPC::ModuleOUT *pGLB = camera.getModule<ISPC::ModuleOUT>();

        if ( !pGLB )
        {
            LOG_ERROR("camera is not initialised with a OUT instance!\n");
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        pGLB->dataExtractionPoint = CI_INOUT_BLACK_LEVEL;
        switch (uiDEBitdepth)
        {
        case 8:
            pGLB->dataExtractionType = BAYER_RGGB_8;
            break;
        case 10:
            pGLB->dataExtractionType = BAYER_RGGB_10;
            break;
        case 12:
            pGLB->dataExtractionType = BAYER_RGGB_12;
            break;
        default:
            LOG_ERROR("DE bit depth of %d not supported, use 8, 10 or 12\n",
                uiDEBitdepth);
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        if ( camera.setupModules() != IMG_SUCCESS )
        {
            LOG_ERROR("failed to setup camera\n");
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        if ( camera.program() != IMG_SUCCESS )
        {
            LOG_ERROR("failed to program camera\n");
            cleanHeap();
            return EXIT_FAILURE;
        }

        // should NOT separate buffers from shots because we accumulate them!
        LOG_INFO("allocate %u buffers to run for %u frames\n",
            uiNBuffers, uiNFrames);
        if (camera.allocateBufferPool(uiNBuffers) != IMG_SUCCESS)
        {
            LOG_ERROR("failed to allocate %d buffers!\n", uiNBuffers);
            cleanHeap();
            return EXIT_FAILURE;
        }

        //
        // run the capture
        //
        PIXELTYPE stype;
        const ISPC::ModuleOUT *glb = 
            camera.getPipeline()->getModule<ISPC::ModuleOUT>();
            
        ret =
            PixelTransformBayer(&stype, glb->dataExtractionType, MOSAIC_RGGB);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("the given format '%s' is not Bayer\n",
                FormatString(glb->dataExtractionType));
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        LOG_INFO("Using DE point %d: format %s\n", 
            glb->dataExtractionPoint, FormatString(glb->dataExtractionType));
        
        frames = new ISPC::Shot[uiNBuffers];
        
        frameExposureInfo = new unsigned long[uiNFrames];
        frameGainInfo = new double[uiNFrames];
        
        if ( !frameExposureInfo || !frameGainInfo )
        {
            LOG_ERROR("failed to allocate the frame information "\
                "buffers\n");
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        IMG_UINT pushedFrames = 0;
        IMG_UINT retrievedFrames = 0;
        double clockMhz = 
            (double)camera.getConnection()->sHWInfo.ui32RefClockMhz;
        
        ISPC::Save file;
        IMG_UINT32 frameSize = 0; // populated with last frame information
        
        if(saveOutput)
        {
            LOG_INFO("open output image file '%s'\n", pszDEFile);
            ret = file.open(ISPC::Save::Bayer, *camera.getPipeline(),
                pszDEFile);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to open the output file %s\n", pszDEFile);
                cleanHeap();
                return EXIT_FAILURE;
            }
        }
        else
        {
            LOG_INFO("Skip saving output image\n");
        }
        
        FILE *stats_file = fopen(CAPTURE_INFO, "w");
        LOG_DEBUG("open stats "CAPTURE_INFO"\n");
        if(!stats_file)
        {
            LOG_ERROR("failed to open the statistics file "CAPTURE_INFO"\n");
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        printInfo(camera);
        
        // we should start later but we need to start to set exposure and focus
        LOG_INFO("start capturing...\n");
        if (camera.startCapture() != IMG_SUCCESS)
        {
            LOG_ERROR("failed to start the capture!\n");
            cleanHeap();
            return EXIT_FAILURE;
        }
        
        if ( uiSensorExposure > 0 )
        {
            sensor->setExposure(uiSensorExposure);
        }
        if ( flSensorGain != 1.0 )
        {
            sensor->setGain(flSensorGain);
        }
        
        if(sensor->getFocusSupported() && uiSensorFocus > 0)
        {
            if(uiSensorFocus >= sensor->getMinFocus()
               && uiSensorFocus <= sensor->getMaxFocus())
            {
                sensor->setFocusDistance(uiSensorFocus);
            }
            else
            {
                LOG_ERROR("cannot set focus to "FOCUS_LBL"mm with selected "\
                    "sensor (min %u, max %u)\n",
                    uiSensorFocus, sensor->getMinFocus(),
                    sensor->getMaxFocus());
                cleanHeap();
                return EXIT_FAILURE;
            }
        }
                
        //
        // initial information
        //
        fprintf(stats_file, "%s mode %d\n", pszSensor, uiSensorMode);
        fprintf(stats_file, "exposure_min="EXP_LBL" exposure_max="EXP_LBL\
            " gain_min="GAIN_LBL" gain_max="GAIN_LBL"\n",
            sensor->getMinExposure(), sensor->getMaxExposure(),
            sensor->getMinGain(), sensor->getMaxGain()
        );
        fprintf(stats_file, "Driver reports clock of "CLOCK_LBL"Mhz\n", clockMhz);
        
        if(sensor->getFocusSupported())
        {
            fprintf(stats_file, "focus_min="FOCUS_LBL"mm focus_max="\
                FOCUS_LBL"mm current="FOCUS_LBL"mm\n",
                (unsigned)sensor->getMinFocus(), (unsigned)sensor->getMaxFocus(),
                (unsigned)sensor->getFocusDistance());
        }
        else
        {
            fprintf(stats_file, "focus not supported\n");
        }
            
        if(bEnableAE)
        {
            fprintf(stats_file, "AE target brightness %.2lf\n",
                fTargetBrightness);
        }
        else
        {
            fprintf(stats_file, "AE disabled\n");
            frameExposureInfo[0] = sensor->getExposure();
            frameGainInfo[0] = sensor->getGain();
            for(int i = 1 ; i < uiNFrames ; i++)
            {
                frameExposureInfo[i] = frameExposureInfo[0];
                frameGainInfo[i] = frameGainInfo[0];
            }
        }
        
        fprintf(stats_file, "%d frames (timestamps, exposure, gain, "\
            "erroneous, missed frames)\n",
                uiNFrames);
                
        printInfo(camera);
        
        time_t running = time(NULL);
        
        IMG_UINT32 lastTimestamps = 0;
        IMG_UINT32 currTimestamps = 0;
        unsigned int nErroneous = 0;
        unsigned int nMissed = 0;
        double accTSDiff = 0;
                
        //
        // pre-submit to LL
        //
        while(pushedFrames < (uiNShots-1))
        {
        
            /* pushed a first shots to always have a buffer in the 
             * capture queue */
            if ( bEnableAE )
            {
                frameExposureInfo[pushedFrames] = sensor->getExposure();
                frameGainInfo[pushedFrames] = sensor->getGain();
            }
            
            if ( camera.enqueueShot() != IMG_SUCCESS )
            {
                LOG_ERROR("failed to enqueue shot %d/%d\n",
                    pushedFrames, uiNFrames);
                cleanHeap();
                if(stats_file)
                {
                    fclose(stats_file);
                }
                return EXIT_FAILURE;
            }
            printf("Pre-Trigger frame %d/%d\n", pushedFrames+1,uiNFrames);
            pushedFrames++;
        }

        //
        // submit then acquire
        //
        while (pushedFrames < uiNFrames)
        {
            if ( bEnableAE )
            {
                frameExposureInfo[pushedFrames] = sensor->getExposure();
                frameGainInfo[pushedFrames] = sensor->getGain();
            }
            
            printf("Trigger frame %d/%d...\n", pushedFrames+1,uiNFrames);
            if ( camera.enqueueShot() != IMG_SUCCESS )
            {
                LOG_ERROR("failed to enqueue shot %d/%d\n",
                    pushedFrames, uiNFrames);
                printInfo(camera);
                cleanHeap();
                if(stats_file)
                {
                    fclose(stats_file);
                }
                return EXIT_FAILURE;
            }
            pushedFrames++;
            
            ret = acquireShot(camera, frames[retrievedFrames%uiNBuffers], 
                retrievedFrames, uiNFrames, nErroneous, nMissed, clockMhz);
            if(ret<0)
            {
                cleanHeap();
                if(stats_file)
                {
                    fclose(stats_file);
                }
                return EXIT_FAILURE;
            }
            accTSDiff += ret;
            
            if ( bConcurrent )
            {
                printf("   concurrent saving frame %d/%d...\n",
                    retrievedFrames+1, uiNFrames);
                
                saveShot(frames[retrievedFrames%uiNBuffers], retrievedFrames,
                    file, bEnableAE, bSaveErroneous, stats_file);

                camera.releaseShot(frames[retrievedFrames%uiNBuffers]);
            }
            
            retrievedFrames++;
        }
        
        while (retrievedFrames < pushedFrames)
        {
            ret = acquireShot(camera, frames[retrievedFrames%uiNBuffers],
                retrievedFrames, uiNFrames, nErroneous, nMissed, clockMhz);
            if(ret<0)
            {
                cleanHeap();
                if(stats_file)
                {
                    fclose(stats_file);
                }
                return EXIT_FAILURE;
            }
            accTSDiff += ret;
                    
            if ( bConcurrent )
            {
                printf("   concurrent saving frame %d/%d...\n",
                    retrievedFrames+1, uiNFrames);
                
                saveShot(frames[retrievedFrames%uiNBuffers], retrievedFrames,
                    file, bEnableAE, bSaveErroneous, stats_file);

                camera.releaseShot(frames[retrievedFrames%uiNBuffers]);
            }
                        
            frameSize = frames[retrievedFrames%uiNBuffers].BAYER.stride
                *frames[retrievedFrames%uiNBuffers].BAYER.height;
            
            retrievedFrames++;
        }

        time_t saving = time(NULL);
        running = saving - running;

        printInfo(camera);
        
        LOG_INFO("stop capturing...\n");
        camera.stopCapture(); // stopping should not destroy the buffers!
        
        if ( !bConcurrent )
        {
            pushedFrames=0;
            while (retrievedFrames>0)
            {
                printf("Saving frame %d/%d\n", pushedFrames+1, uiNFrames);

                saveShot(frames[pushedFrames%uiNBuffers], pushedFrames,
                    file, bEnableAE, bSaveErroneous, stats_file);
                
                retrievedFrames--;

                camera.releaseShot(frames[pushedFrames]);
                pushedFrames++;
            }
            
            saving = time(NULL) - saving;
        }
        else
        {
            saving = running; // saving done at same time
        }
        file.close();
        if(stats_file)
        {
            fclose(stats_file);
        }

        printf("===\n");
        double frameSizeMB = frameSize/(1024.0*1024.0);
        ISPC::Global_Setup globalSetup =
            camera.getPipeline()->getGlobalSetup();
        printf("Frames: %u (%.3fMB per frame) using %d buffers\n",
            uiNFrames, frameSizeMB, uiNShots);
        printf("Image sizes: %ux%u\n",
            globalSetup.ui32ImageWidth, globalSetup.ui32ImageHeight);
        if ( !bConcurrent )
        {
            printf("Capture ran in %d sec - saving done in %d sec\n",
                (int)running, (int)saving);
        }
        else
        {
            printf("Capture ran in %d sec - including saving\n", (int)running);
        }
        printf("Estimated FPS "FPS_LBL" - estimated transfer %.3fMB/s\n",
               (float)(uiNFrames)/((float)running),
               (uiNFrames*frameSizeMB/saving)
        );
        printf("Timestamps average difference %.0lf tick ("FPS_LBL"FPS "\
            "with "CLOCK_LBL"MHz Clock)\n",
            accTSDiff/(uiNFrames-1),
            (clockMhz*1000000.0)/(accTSDiff/(uiNFrames-1)),
            clockMhz);
        if ( !bEnableAE )
        {
            printf("Sensor exposure %lu us - gain %2.3f\n",
                sensor->getExposure(), sensor->getGain());
        }
        printf("%u erroneous shots\n", nErroneous);
        printf("%u missed shots\n", nMissed);
        printf("Shots information available in "CAPTURE_INFO"\n");
        printf("===\n");
        printInfo(camera);
    }
    // destroys context that holds the driver

    cleanHeap();

    return EXIT_SUCCESS;
}
