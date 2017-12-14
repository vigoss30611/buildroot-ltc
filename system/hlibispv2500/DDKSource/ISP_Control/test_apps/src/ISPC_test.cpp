/**
******************************************************************************
 @file ISPC_test.cpp

 @brief ISPControl test application to run with DG

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
#include <savefile.h>
#include <dyncmd/commandline.h>

#include <ci/ci_version.h>

#include <fstream>

#include <ispc/Camera.h>
#include <ispc/ParameterList.h>
#include <ispc/ParameterFileParser.h>

// Modules
#include <ispc/ModuleOUT.h>
#include <ispc/ModuleLSH.h>
#include <ispc/ModuleDPF.h>

// Control modules
#include <ispc/ControlAE.h>
#include <ispc/ControlAWB.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlDNS.h> // not really used

#include <ispc/Save.h>

#ifdef EXT_DATAGEN
#include <sensors/dgsensor.h>
#endif
#include <sensors/iifdatagen.h> // extended params

#ifdef FELIX_FAKE
#include <ci_kernel/ci_debug.h> // access to the simulator IP and Port
#include <ispctest/driver_init.h>
#endif
#include <ispctest/hdf_helper.h>
#include <felix_hw_info.h>


#define LOG_TAG "ISPC_test"
#include "felixcommon/userlog.h"

struct TEST_PARAM;

/**
 * usage:
 * @li TEST_PARAM::populate()
 * @li configureOutput()
 * @li handleShot() for each received shot
 */
struct TestContext
{
    const TEST_PARAM *config;
    int context;

    ISPC::Camera *pCamera;
    std::list<IMG_UINT32> bufferIds;
    const char *lshGridFile; // lsh grid

    ISPC::Save *dispOutput;
    ISPC::Save *encOutput;
    ISPC::Save *hdrExtOutput;
    ISPC::Save *raw2DOutput;

    ISPC::Save *statsOutput;
    bool bIgnoreDPF;
    unsigned int hdrInsMax; // set a configure output

    TestContext()
    {
        pCamera = NULL;
        lshGridFile = NULL;

        dispOutput = 0;
        encOutput = 0;
        hdrExtOutput = 0;
        raw2DOutput = 0;

        statsOutput = 0;
        bIgnoreDPF = false;
        hdrInsMax = 0;
    }
    
    /**
     * @brief Configures output files from Pipeline's configuration
     *
     * @note May disable some of the output formats if the HW does not support it!
     */
    int configureOutput();

    int allocateBuffers();

    void saveRaw(const ISPC::Shot &capturedShot);
    void handleShot(const ISPC::Shot &capturedShot, const ISPC::Camera &camera); // write available images and info to disk if enabled
    void configureAlgorithms(const ISPC::ParameterList &parameters, bool isOnwerOfCamera=true); // handles the update of the configuration using the registered control algorithms

    void close()
    {
        if ( dispOutput ) 
        {
            dispOutput->close();
            delete dispOutput;
            dispOutput = 0;
        }
        if ( encOutput )
        {
            encOutput->close();
            delete encOutput;
            encOutput = 0;
        }
        if ( hdrExtOutput )
        {
            hdrExtOutput->close();
            delete hdrExtOutput;
            hdrExtOutput = 0;
        }
        if ( raw2DOutput )
        {
            raw2DOutput->close();
            delete raw2DOutput;
            raw2DOutput = 0;
        }
        if ( statsOutput )
        {
            statsOutput->close();
            delete statsOutput;
            statsOutput = 0;
        }
    }

    ~TestContext()
    {
        if ( pCamera )
        {
            delete pCamera;
        }

        close();
    }
};

struct TEST_PARAM //struct with parameters for test
{
    bool waitAtFinish; // getchar() at the end to help when running in new console on windows
    IMG_BOOL8 bParallel; // to turn on 2 contexts on single DG (IMG_BOOL8 because given to DYNCMD)
    bool bNoWrite; // do not write output
    IMG_BOOL8 bIgnoreDPF; // to ignore DPF output and override stats (IMG_BOOL8 because given to DYNCMD)
    bool bDumpConfigFiles;
    
    // data generator information stored by gasket
    IMG_CHAR *pszInputFile[CI_N_IMAGERS];   // input file name for the data generator
    IMG_INT aNBInputFrames[CI_N_IMAGERS]; // nb of frames to load from FLX file
    IMG_UINT aBlanking[CI_N_IMAGERS][2]; // blanking period for the DG of that gasket
    bool aIsIntDG[CI_N_IMAGERS]; // is this gasket replaced by an internal dg?
    IMG_UINT aIndexDG[CI_N_IMAGERS]; // which internal dg context to use (if external same as gasket)

    // context parameters
    IMG_CHAR *pszFelixSetupArgsFile[CI_N_CONTEXT]; // FelixSetupArgs file name used to configure the pipeline
    IMG_CHAR *pszHDRInsertionFile[CI_N_CONTEXT]; // HDR insertion file to use if enabled in felix setup args
    IMG_BOOL8 aEnableTimestamps[CI_N_CONTEXT]; // enable the generation of timestamps
    IMG_UINT uiImager[CI_N_CONTEXT];
    IMG_UINT uiNBuffers[CI_N_CONTEXT]; // optional
    IMG_UINT uiNEncTiled[CI_N_CONTEXT]; // nb of tiled encoder output buffers - maxed with uiNBuffers
    IMG_UINT uiNDispTiled[CI_N_CONTEXT]; // nb of tiled display output buffers - maxed with uiNBuffers
    IMG_UINT uiNHDRTiled[CI_N_CONTEXT]; // nb of tiled HDR output buffers - maxed with uiNBuffers

    IMG_UINT uiNFrames[CI_N_CONTEXT]; // nb of frames to run for
    IMG_CHAR *pszLSHGrid[CI_N_CONTEXT];
    IMG_BOOL8 bSaveStatistics[CI_N_CONTEXT];
    IMG_BOOL8 bReUse[CI_N_CONTEXT]; // re-use previous context object
    IMG_BOOL8 bWriteRaw[CI_N_CONTEXT]; // write raw images from HW as well as FLX for the Display, DE, HDR and Raw2D points (YUV is already raw)
    IMG_BOOL8 bRescaleHDRIns[CI_N_CONTEXT];

    //IMG_BOOL8 autoExposure[CI_N_CONTEXT];            ///<@brief Enable/Disable Auto exposure
    //IMG_FLOAT targetBrightness[CI_N_CONTEXT];        ///<@brief target value for the ControlAE brightness
    //IMG_BOOL8 flickerRejection[CI_N_CONTEXT];        ///<@brief Enable/Disable Flicker Rejection

    IMG_BOOL8 autoToneMapping[CI_N_CONTEXT];        ///<@brief Enable/Disable global tone mapping curve automatic calculation
    IMG_BOOL8 localToneMapping[CI_N_CONTEXT];        ///<@brief Enable/Disable local tone mapping
    IMG_BOOL8 adaptiveToneMapping[CI_N_CONTEXT];        ///<@brief Enable/Disable tone mapping auto strength control

    IMG_BOOL8 autoDenoiserISO[CI_N_CONTEXT];        ///<@brief Enable/Disable denoiser ISO control

    IMG_BOOL8 autoLBC[CI_N_CONTEXT];                ///<@brief Enable/Disable Light Based Control

    IMG_UINT autoWhiteBalanceAlgorithm[CI_N_CONTEXT];

    // fake interface only
    IMG_UINT uiRegisterOverride[CI_N_CONTEXT]; // tries to load
    IMG_CHAR *pszTransifLib; // path to simulator dll - only valid when using fake interface
    IMG_CHAR *pszTransifSimParam; // parameter file to give to simulator dll - only valid when using fake interface
    IMG_CHAR *pszSimIP; // simulator IP address when using TCP/IP interface - NULL means localhost
    IMG_INT uiSimPort; // simulator port when using TCP/IP interface - 0 means default
    IMG_BOOL8 bEnableRDW; // when generating pdump should RDW be enabled
    IMG_UINT uiUseMMU; // 0 bypass, 1 32b, 2 extended address range - only valid when using fake interface
    IMG_UINT uiTilingScheme; // 256 or 512 according to tiling scheme to use
    IMG_UINT uiGammaCurve; // gamma curve to use - see KRN_CI_DriverDefaultsGammaLUT() for legitimate values
    IMG_UINT uiCPUPageSize; // CPU page size in kB

    TEST_PARAM()
    {
        IMG_MEMSET(this, 0, sizeof(TEST_PARAM));
        // defaults that are not 0
        uiUseMMU = 2; // use the mmu by default
        uiTilingScheme = 256;
#ifdef FELIX_FAKE // otherwise uiGammaCurve is not used and CI_DEF_GMACURVE is not defined
        uiGammaCurve = CI_DEF_GMACURVE;
#endif
        bEnableRDW = IMG_FALSE;
        uiCPUPageSize = 4; // 4kB by default
        for ( int ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
        {
#ifndef FELIX_FAKE
            aEnableTimestamps[ctx] = IMG_TRUE;
#endif // when running FAKE driver timestamps are disabled by default

            uiNBuffers[ctx] = 1;
            uiNEncTiled[ctx] = 0;
            uiNDispTiled[ctx] = 0;
            uiNHDRTiled[ctx] = 0;
            
            uiNFrames[ctx] = 1;
            uiImager[ctx] = ctx;
            bSaveStatistics[ctx] = IMG_TRUE;
            bRescaleHDRIns[ctx] = IMG_TRUE;
            //uiSocketConfigPort[ctx] = 0;
            //pszSocketConfigIP[ctx] = NULL;
            //targetBrightness[ctx] = 0.0;
            //flickerRejection[ctx] = IMG_FALSE; //needs AE
            autoToneMapping[ctx] = IMG_FALSE;
            localToneMapping[ctx] = IMG_FALSE;
            adaptiveToneMapping[ctx] = IMG_FALSE;
            autoDenoiserISO[ctx] = IMG_FALSE;
            autoLBC[ctx] = IMG_FALSE;
            autoWhiteBalanceAlgorithm[ctx] = (IMG_UINT)ISPC::ControlAWB::WB_NONE;
        }
        for ( int g = 0 ; g < CI_N_IMAGERS ; g++ )
        {
            pszInputFile[g] = NULL;
            aNBInputFrames[g] = -1;
            aBlanking[g][0] = FELIX_MIN_H_BLANKING;
            aBlanking[g][1] = FELIX_MIN_V_BLANKING;
            aIsIntDG[g] = false;
            aIndexDG[g] = g;
        }
    }
    
    ~TEST_PARAM()
    {
        int i;
        
        for (i=0 ; i < CI_N_IMAGERS ; i++)
        {
            if(pszInputFile[i])
            {
                IMG_FREE(pszInputFile[i]);
                pszInputFile[i]=0;
            }
        }
        for (i=0 ; i < CI_N_CONTEXT ; i++)
        {
            if(pszFelixSetupArgsFile[i])
            {
                IMG_FREE(pszFelixSetupArgsFile[i]);
                pszFelixSetupArgsFile[i]=0;
            }
            if (pszHDRInsertionFile[i])
            {
                IMG_FREE(pszHDRInsertionFile[i]);
                pszHDRInsertionFile[i] = 0;
            }
            if(pszLSHGrid[i])
            {
                IMG_FREE(pszLSHGrid[i]);
                pszLSHGrid[i]=0;
            }
        }
        if(pszTransifLib)
        {
            IMG_FREE(pszTransifLib);
            pszTransifLib=0;
        }
        if(pszTransifSimParam)
        {
            IMG_FREE(pszTransifSimParam);
            pszTransifSimParam=0;
        }
        if(pszSimIP)
        {
            IMG_FREE(pszSimIP);
            pszSimIP=0;
        }
    }

    void printTestParameters() const;

    int load_parameters(int argc, char *argv[]); // load options from command line, returns number of vital parameters that were missing

    void populate(TestContext &config, int ctx) const;
};

void TEST_PARAM::printTestParameters() const
{
    printf("===\n\n");
    for ( int i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        int dg = uiImager[i];
        printf("Context %d:\n", i);
        if ( pszInputFile[dg] != NULL && pszFelixSetupArgsFile[i] != NULL )
        {
            printf("  Using imager %d\n", dg);
            printf("  Setup file: %s\n", pszFelixSetupArgsFile[i]);
            printf("  Number of buffers: %u (tiled: enc=%d disp=%d hdr=%d)\n", 
                uiNBuffers[i], uiNEncTiled[i], uiNDispTiled[i], uiNHDRTiled[i]);
            printf("  Number of frames: %u\n", uiNFrames[i]);
#if defined(FELIX_FAKE) && defined(CI_DEBUG_FCT)
            printf("  Override: %d\n", uiRegisterOverride[i]);
#endif
        }
        else
        {
            printf("  (disabled)\n");
        }
    }
    printf("===\n\n");
    for ( int g = 0 ; g < CI_N_IMAGERS ; g++ )
    {
#ifdef EXT_DATAGEN
        if ( pszInputFile[g] != NULL )
#else
        if ( pszInputFile[g] != NULL && aIsIntDG[g] ) // otherwise no other way to input data
#endif
        {
            printf("Gasket %d:\n", g);
            if ( aIsIntDG[g] )
            {
                printf("  Int. Datagen %d\n", aIndexDG[g]);
            }
            else
            {
                printf("  Ext. Datagen %d\n", g);
            }
            if(aNBInputFrames[g]>=0)
            {
                printf("  Input file: %d frames from %s\n", aNBInputFrames[g], pszInputFile[g]);
            }
            else
            {
                printf("  Input file: all frames from %s\n", pszInputFile[g]);
            }
            printf("  Blanking: H=%d V=%d\n", aBlanking[g][0], aBlanking[g][1]);
        }
        else
        {
            printf("Gasket %d disabled\n", g);
        }
    }
    printf("===\n\n");
}

int TEST_PARAM::load_parameters(int argc, char *argv[])
{
    int ret;
    int missing = 0;
    IMG_UINT32 unregistered;
    char paramName[64];
    int ctx;
    // see TEST_PARAM() for defaults

    IMG_CHAR *int_pszInputFile[CI_N_IIF_DATAGEN];
    IMG_INT int_aNBInputFrames[CI_N_IIF_DATAGEN];
    IMG_UINT int_aBlanking[CI_N_IIF_DATAGEN][2];
    IMG_UINT int_aGasket[CI_N_IIF_DATAGEN];

    for ( ctx = 0 ; ctx < CI_N_IIF_DATAGEN ; ctx++ )
    {
        int_pszInputFile[ctx] = this->pszInputFile[ctx];
        int_aNBInputFrames[ctx] = this->aNBInputFrames[ctx];
        int_aBlanking[ctx][0] = this->aBlanking[ctx][0];
        int_aBlanking[ctx][1] = this->aBlanking[ctx][1];
        int_aGasket[ctx] = CI_N_IMAGERS; // disabled
    }

#ifdef EXT_DATAGEN
    // ext
    IMG_CHAR *ext_pszInputFile[CI_N_EXT_DATAGEN];
    IMG_INT ext_aNBInputFrames[CI_N_EXT_DATAGEN];
    IMG_UINT ext_aBlanking[CI_N_EXT_DATAGEN][2];
    IMG_UINT ext_aGasket[CI_N_EXT_DATAGEN];

    for ( ctx = 0 ; ctx < CI_N_EXT_DATAGEN ; ctx++ )
    {
        ext_pszInputFile[ctx] = this->pszInputFile[ctx];
        ext_aNBInputFrames[ctx] = this->aNBInputFrames[ctx];
        ext_aBlanking[ctx][0] = this->aBlanking[ctx][0];
        ext_aBlanking[ctx][1] = this->aBlanking[ctx][1];
        ext_aGasket[ctx] = ctx;
    }
#endif
    
    //command line parameter loading:
    if (DYNCMD_AddCommandLine(argc, argv, "-source") != IMG_SUCCESS)
    {
        LOG_ERROR("unable to parse command line\n");
        DYNCMD_ReleaseParameters();
        return EXIT_FAILURE;
    }

    // internal dg options
    for ( ctx = 0 ; ctx < CI_N_IIF_DATAGEN ; ctx++ )
    {
        sprintf(paramName, "-IntDG%d_inputFLX", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING, 
            "FLX input file to use for Data Generator", 
            &(int_pszInputFile[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-IntDG%d_inputFrames", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_INT, 
            "[optional] Number of frames to load from the FLX input file (<=0 means all)", 
            &(int_aNBInputFrames[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-IntDG%d_blanking", ctx);
        if ( (ret=DYNCMD_RegisterArray(paramName, DYNCMDTYPE_UINT, 
            "Horizontal and vertical blanking in pixels", 
            &(int_aBlanking[ctx]), 2)) != RET_FOUND )
        {
            if ( ret == RET_INCOMPLETE )
            {
                LOG_WARNING("%s incomplete, second value for blanking is assumed to be %u\n", paramName, int_aBlanking[ctx][1]);
            }
            else if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-IntDG%d_gasket", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Gasket to replace data from", 
            &(int_aGasket[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
    }

#ifdef EXT_DATAGEN
    //
    // external dg options options
    //
    for ( ctx = 0 ; ctx < CI_N_EXT_DATAGEN ; ctx++ )
    {
        sprintf(paramName, "-DG%d_inputFLX", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING, 
            "FLX input file to use for Data Generator", 
            &(ext_pszInputFile[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-DG%d_inputFrames", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_INT, 
            "[optional] Number of frames to load from the FLX input file (<=0 means all)", 
            &(ext_aNBInputFrames[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-DG%d_blanking", ctx);
        if ( (ret=DYNCMD_RegisterArray(paramName, DYNCMDTYPE_UINT, 
            "Horizontal and vertical blanking in pixels", 
            &(ext_aBlanking[ctx]), 2)) != RET_FOUND )
        {
            if ( ret == RET_INCOMPLETE )
            {
                LOG_WARNING("Warning: %s incomplete, second value for blanking is assumed to be %u\n", paramName, ext_aBlanking[ctx][1]);
            }
            else if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
    }
#endif

    // then sort dg per gasket
    for ( ctx = 0 ; ctx < CI_N_IMAGERS ; ctx++ )
    {
        int iIntDg = -1; // not internal dg
        int i;
        for ( i = 0 ; i < CI_N_IIF_DATAGEN ; i++ )
        {
            if ( int_aGasket[i] == ctx )
            {
                iIntDg = i;
            }
        }

        if ( iIntDg >= 0 )
        {
            this->aIsIntDG[ctx] = true;
            this->pszInputFile[ctx] = int_pszInputFile[iIntDg];
            this->aNBInputFrames[ctx] = int_aNBInputFrames[iIntDg];
            this->aBlanking[ctx][0] = int_aBlanking[iIntDg][0];
            this->aBlanking[ctx][1] = int_aBlanking[iIntDg][1];
            this->aIndexDG[ctx] = iIntDg;
        }
#ifdef EXT_DATAGEN
        else
        {
            if ( ctx < CI_N_EXT_DATAGEN )
            {
                this->aIsIntDG[ctx] = false;
                this->pszInputFile[ctx] = ext_pszInputFile[ctx];
                this->aNBInputFrames[ctx] = ext_aNBInputFrames[ctx];
                this->aBlanking[ctx][0] = ext_aBlanking[ctx][0];
                this->aBlanking[ctx][1] = ext_aBlanking[ctx][1];
                this->aIndexDG[ctx] = ctx;
            }
            // otherwise leave to defaults
        }
#endif
    }

    //
    // CTX options
    //
    for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
    {
        sprintf(paramName, "-CT%d_setupFile", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING, 
            "FelixSetupArgs file to use to configure the Pipeline", 
            &(this->pszFelixSetupArgsFile[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
        
        sprintf(paramName, "-CT%d_imager", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Imager to use as input for that context", 
            &(this->uiImager[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND )
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_nBuffers", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Number of pending buffers for that context (submitted at once - maxed to the size of the pending list in HW)", 
            &(this->uiNBuffers[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_HDRInsertion", ctx);
        if ((ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING,
            "The filename to use as the input of the HDR Insertion if "\
            "enabled in given setup file - "\
            "if none do not use HDR Insertion, image has to be an FLX "\
            "in RGB of the same size than the one provided to DG but does "\
            "not have to be of the correct bitdepth",
            &(this->pszHDRInsertionFile[ctx]))) != RET_FOUND)
        {
            if (ret != RET_NOT_FOUND)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_HDRInsRescale", ctx);
        if ((ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Rescale provided HDR Insertion file to 16b per pixels",
            &(this->bRescaleHDRIns[ctx]))) != RET_FOUND)
        {
            if (ret != RET_NOT_FOUND) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_nEncTiled", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Number of encoder buffers to have tiled (maxed to corresponding -CTX_nBuffers - if same value than -CTX_nBuffers all outputs are tiled)",
            &(this->uiNEncTiled[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_nDispTiled", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Number of display buffers to have tiled (maxed to corresponding -CTX_nBuffers - if same value than -CTX_nBuffers all outputs are tiled)", 
            &(this->uiNDispTiled[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_nHDRExtTiled", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Number of HDR Extraction buffers to have tiled (maxed to corresponding -CTX_nBuffers - if same value than -CTX_nBuffers all outputs are tiled)", 
            &(this->uiNHDRTiled[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_nFrames", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Number of frames to run for that context", 
            &(this->uiNFrames[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_LSHGrid", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING, 
            "LSH grid file to load instead of the one in -CTX_setupFile", 
            &(this->pszLSHGrid[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_saveStats", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, 
            "Enable saving of the raw statistics from the HW (contains CRCs, timestamps etc) - disabled if -no_write is specified", 
            &(this->bSaveStatistics[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_saveRaw", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, 
            "Enable saving of the raw image data for images output (except YUV which is always RAW image from HW). Stride is removed.", 
            &(this->bWriteRaw[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_TNM", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, 
            "Enable usage Global Tone Mapper control algorithm", 
            &(this->autoToneMapping[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
        sprintf(paramName, "-CT%d_TNM-local", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, 
            "Enable usage Local Tone Mapper control algorithm", 
            &(this->localToneMapping[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
        sprintf(paramName, "-CT%d_TNM-adapt", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, 
            "Enable usage Adaptive Tone Mapper control algorithm", 
            &(this->adaptiveToneMapping[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_LBC", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, 
            "Enable usage Light Based Controls algorithm (and loading of the LBC parameters)", 
            &(this->autoToneMapping[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_WBC", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "Enable usage of White Balance Control algorithm and loading of the multi-ccm parameters - 0 is disabled, other values are mapped ISPC::Correction_Types (1=AC, 2=WP, 3=HLW, 4=COMBINED)", 
            &(this->autoWhiteBalanceAlgorithm[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        sprintf(paramName, "-CT%d_enableTS", ctx);
        if (DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, "Enable generation of timestamps - no effect when running against CSIM", &(this->aEnableTimestamps[ctx])) != RET_FOUND)
        {
            if (ret != RET_NOT_FOUND)
            {
                missing++;
                LOG_ERROR("%s expects a bool but none was found\n", paramName);
            }
        }


        if ( ctx > 0 )
        {
            sprintf(paramName, "-CT%d_re-use", ctx);
            if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8, 
                "Enable the re-use of the previously used Camera object when running in serial mode (the previously used context needs to be configured to use the same DG as the new one!)", 
                &(this->bReUse[ctx]))) != RET_FOUND )
            {
                if ( ret != RET_NOT_FOUND ) // if not found use default
                {
                    missing++;
                    LOG_ERROR("Error while parsing parameter %s\n", paramName);
                }
            }
        }

#if defined(FELIX_FAKE) && defined(CI_DEBUG_FCT)

        sprintf(paramName, "-CT%d_regOverride", ctx);
        if ( (ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT, 
            "[optional] enable register override (0 is disabled, >0 enabled)", 
            &(this->uiRegisterOverride[ctx]))) != RET_FOUND )
        {
            if ( ret != RET_NOT_FOUND ) // if not found use default
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

#endif // FELIX_FAKE && CI_DEBUG_FCT
    } // for each context

#ifdef FELIX_FAKE

    if (DYNCMD_RegisterParameter("-simTransif", DYNCMDTYPE_STRING,
        "Transif library given by the simulator. Also needs -simConfig to be used.",
        &(this->pszTransifLib))==RET_NOT_FOUND)
    {
        LOG_INFO("TCP/IP sim connection\n");
    }
    else
    {
        LOG_INFO("Transif sim connection (%s)\n", this->pszTransifLib);
    }

    // only if transif
    if (DYNCMD_RegisterParameter("-simConfig", DYNCMDTYPE_STRING,
        "Configuration file to give to the simulator when using transif (-argfile file parameter of the sim)", 
        &(this->pszTransifSimParam))==RET_NOT_FOUND)
    {
        if ( this->pszTransifLib != NULL )
        {
            missing++;
            LOG_ERROR("-simConfig parameter file not given\n");
        }
    }

    if ((ret=DYNCMD_RegisterParameter("-simIP", DYNCMDTYPE_STRING,
        "Simulator IP address to use when using TCP/IP interface.", 
        &(this->pszSimIP)))!=RET_FOUND)
    {
        if (ret != RET_NOT_FOUND )
        {
            missing++;
            LOG_ERROR("-simIP expects a string but none was found\n");
        }
    }

    if ((ret=DYNCMD_RegisterParameter("-simPort", DYNCMDTYPE_INT, 
        "Simulator port to use when running TCP/IP interface.", 
        &(this->uiSimPort))) !=RET_FOUND )
    {
        if (ret != RET_NOT_FOUND )
        {
            missing++;
            LOG_ERROR("-simPort expects an int but none was found\n");
        }
    }

    if ((ret=DYNCMD_RegisterParameter("-pdumpEnableRDW", DYNCMDTYPE_BOOL8, 
        "Enable generation of RDW commands in pdump output (not useful for verification)", 
        &(this->bEnableRDW))) !=RET_FOUND )
    {
        if (ret != RET_NOT_FOUND )
        {
            missing++;
            LOG_ERROR("-pdumpEnableRDW expects a bool but none was found\n");
        }
    }

    if (DYNCMD_RegisterParameter("-mmucontrol", DYNCMDTYPE_INT, 
        "Flag to indicate how to use the MMU - 0 = bypass, 1 = 32b MMU, >1 = extended addressing", 
        &(this->uiUseMMU)) == RET_INCORRECT )
    {
        missing++;
        LOG_ERROR("expecting an int for -mmucontrol option\n");
    }

    if (DYNCMD_RegisterParameter("-tilingScheme", DYNCMDTYPE_INT, 
        "Choose tiling scheme (256 = 256Bx16, 512 = 512Bx8)", &(this->uiTilingScheme))==RET_INCORRECT)
    {
        CI_FATAL("-tilingScheme expects an integer and none was found\n");
        return EXIT_FAILURE;
    }

    if (DYNCMD_RegisterParameter("-gammaCurve", DYNCMDTYPE_UINT, 
        "Gamma Look Up Curve to use - 0 = BT709, 1 = sRGB (other is invalid)", 
        &(this->uiGammaCurve)) == RET_INCORRECT )
    {
        missing++;
        LOG_ERROR("expecting an uint for -gammaCurve option\n");
    }
    
    if (DYNCMD_RegisterParameter("-cpuPageSize", DYNCMDTYPE_UINT, 
        "CPU page size in kB (e.g. 4, 16)", 
        &(this->uiCPUPageSize)) == RET_INCORRECT )
    {
        missing++;
        LOG_ERROR("expecting an uint for -gammaCurve option\n");
    }

    if (DYNCMD_RegisterParameter("-no_pdump", DYNCMDTYPE_COMMAND, "Do not write pdumps", NULL)==RET_FOUND)
    {
        // global variable from ci_debug.h
        bEnablePdump = IMG_FALSE;
    }

#endif // FELIX_FAKE

    if((ret=DYNCMD_RegisterParameter("-parallel",DYNCMDTYPE_BOOL8,"run multi-context all at once (1) or in serial (0)",&(this->bParallel)))!=RET_FOUND )
    {
        if (ret != RET_NOT_FOUND )
        {
            missing++;
            LOG_ERROR("-parallel expects a bool but none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-ignoreDPF", DYNCMDTYPE_BOOL8, 
        "Override DPF results in statistics and skip saving of the DPF "\
        "output file (used for verification as CSIM and HW do not match on"\
        "DPF output)", &(this->bIgnoreDPF));
    if (RET_FOUND != ret)
    {
        if (ret != RET_NOT_FOUND)
        {
            missing++;
            LOG_ERROR("-ignoreDPF expects a bool but none was found\n");
        }
    }

    if (DYNCMD_RegisterParameter("-no_write", DYNCMDTYPE_COMMAND, "Do not write output to file (including statistics)", NULL)==RET_FOUND)
    {
        this->bNoWrite = true;
    }

    if (DYNCMD_RegisterParameter("-wait", DYNCMDTYPE_COMMAND, "Wait at the end to allow reading of the console output on windows when starting as batch", NULL)==RET_FOUND)
    {
        this->waitAtFinish = true;
    }

    if (DYNCMD_RegisterParameter("-DumpConfigFiles", DYNCMDTYPE_COMMAND, "Output configuration files for default, minimum and maximum values for all parameters", NULL)==RET_FOUND)
    {
        this->bDumpConfigFiles = true;
    }

    /*
     * must be last action
     */
    if(DYNCMD_RegisterParameter("-h",DYNCMDTYPE_COMMAND,"Usage information",NULL)==RET_FOUND || missing > 0)
    {
        DYNCMD_PrintUsage();
        return EXIT_FAILURE;
    }

    DYNCMD_HasUnregisteredElements(&unregistered);

    this->printTestParameters();
    // print if parameters were not used and clean parameter internal structure - we don't need it anymore
    DYNCMD_ReleaseParameters();

    return missing > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void TEST_PARAM::populate(TestContext &config, int ctx) const
{
    config.context = ctx;
    config.config = this;
    config.bIgnoreDPF = this->bIgnoreDPF?true:false;

    config.lshGridFile = this->pszLSHGrid[ctx];
}

int TestContext::configureOutput()
{
    if (pCamera)
    {
        ISPC::Pipeline &pipeline = *(pCamera->getPipeline());
        ISPC::ModuleOUT *glb = pCamera->getModule<ISPC::ModuleOUT>();
        const CI_CONNECTION *conn = pCamera->getConnection();
        int ctx = pipeline.getCIPipeline()->ui8Context;

#ifdef FELIX_FAKE
        if (config->aEnableTimestamps[ctx])
        {
            LOG_WARNING("Ignoring timestamp option when running against CSIM\n");
        }
#else
        pCamera->getPipeline()->getMCPipeline()->bEnableTimestamp = config->aEnableTimestamps[ctx];
#endif
        LOG_INFO("Ctx %d timestamps are %s\n", ctx, pCamera->getPipeline()->getMCPipeline()->bEnableTimestamp ? "ON" : "OFF");

        
        if (!this->config->bNoWrite)
        {
            char filename[64];
            int nTiled = 0; // increment for each output that needs tiling
            
            if (glb->displayType != PXL_NONE)
            {
                sprintf(filename, "display%d.flx", pipeline.ui8ContextNumber);
                dispOutput = new ISPC::Save();
                if (dispOutput->open(ISPC::Save::RGB, pipeline, filename) != IMG_SUCCESS)
                {
                    LOG_ERROR("failed to open output file '%s'\n", filename);
                    return EXIT_FAILURE;
                }
                if (config->uiNDispTiled[ctx] > 0)
                {
                    nTiled++;
                }
            }
            else if (glb->dataExtractionPoint != CI_INOUT_NONE && glb->dataExtractionType != PXL_NONE)
            {
                sprintf(filename, "dataExtraction%d.flx", pipeline.ui8ContextNumber);
                dispOutput = new ISPC::Save();
                if (dispOutput->open(ISPC::Save::Bayer, pipeline, filename) != IMG_SUCCESS)
                {
                    LOG_ERROR("failed to open output file '%s'\n", filename);
                    return EXIT_FAILURE;
                }
            }

            if (glb->encoderType != PXL_NONE)
            {
                std::string file = ISPC::Save::getEncoderFilename(pipeline);
                encOutput = new ISPC::Save();

                if (encOutput->open(ISPC::Save::YUV, pipeline, file) != IMG_SUCCESS)
                {
                    LOG_ERROR("failed to open output file '%s'\n", file.c_str());
                    return EXIT_FAILURE;
                }
                if (config->uiNEncTiled[ctx] > 0)
                {
                    nTiled++;
                }
            }

            if (glb->hdrExtractionType != PXL_NONE)
            {
                if ((conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT))
                {
                    sprintf(filename, "hdrExtraction%d.flx", pipeline.ui8ContextNumber);
                    hdrExtOutput = new ISPC::Save();
                    if (hdrExtOutput->open(ISPC::Save::RGB_EXT, pipeline, filename) != IMG_SUCCESS)
                    {
                        LOG_ERROR("failed to open output file '%s'\n", filename);
                        return EXIT_FAILURE;
                    }
                    if (config->uiNHDRTiled[ctx] > 0)
                    {
                        nTiled++;
                    }
                }
                else
                {
                    LOG_WARNING("current HW %d.%d does not support HDR Extraction - output forced to NONE\n",
                        (int)conn->sHWInfo.rev_ui8Major, (int)conn->sHWInfo.rev_ui8Minor);
                    glb->hdrExtractionType = PXL_NONE;
                    glb->requestUpdate();
                }
            }

            if (glb->raw2DExtractionType != PXL_NONE)
            {
                if ((conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT))
                {
                    sprintf(filename, "raw2DExtraction%d.flx", pipeline.ui8ContextNumber);
                    raw2DOutput = new ISPC::Save();
                    if (raw2DOutput->open(ISPC::Save::Bayer_TIFF, pipeline, filename) != IMG_SUCCESS)
                    {
                        LOG_ERROR("failed to open output file '%s'\n", filename);
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    LOG_WARNING("current HW %d.%d does not support RAW2D Extraction - output forced to NONE\n",
                        (int)conn->sHWInfo.rev_ui8Major, (int)conn->sHWInfo.rev_ui8Minor);
                    glb->raw2DExtractionType = PXL_NONE;
                    glb->requestUpdate();
                }
            }

            if (this->config->bSaveStatistics[this->context] == IMG_TRUE)
            {
                sprintf(filename, "ctx_%d_stats.dat", pipeline.ui8ContextNumber);
                statsOutput = new ISPC::Save();
                if (statsOutput->open(ISPC::Save::Bytes, pipeline, filename) != IMG_SUCCESS)
                {
                    LOG_ERROR("failed to open output file '%s'\n", filename);
                    return EXIT_FAILURE;
                }
            }

            // write DPF map handled per shot

            // if available in HW enable tiling if needed
            if (conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_TILING)
            {
                pipeline.setTilingEnabled(nTiled > 0);
            }
        }
        
        if (glb->hdrInsertionType != PXL_NONE)
        {
            if ((conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_INS)
                == 0)
            {
                LOG_WARNING("current HW %d.%d does not support HDR Insertion - input forced to NONE\n",
                    (int)conn->sHWInfo.rev_ui8Major, (int)conn->sHWInfo.rev_ui8Minor);

                glb->hdrInsertionType = PXL_NONE;
            }
            else
            {
                if (config->pszHDRInsertionFile[ctx] != NULL)
                {
                    bool isValid;
                    IMG_RESULT ret = IsValidForCapture(*pCamera,
                        config->pszHDRInsertionFile[ctx], isValid, hdrInsMax);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("Failed to verify if provided HDR is not valid for current setup\n");
                        return EXIT_FAILURE;
                    }
                    
                    if (!isValid)
                    {
                        LOG_ERROR("Provided HDR is not valid for current setup\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    LOG_WARNING("disabling HDR insertion because no file was given as input\n");
                    glb->hdrInsertionType = PXL_NONE;
                }
            }

            if (PXL_NONE == glb->hdrInsertionType)
            {
                glb->requestUpdate();
            }
        }

        // check HDR insertion
    }
    return EXIT_SUCCESS;
}

int TestContext::allocateBuffers()
{
    const ISPC::ModuleOUT *pOut = pCamera->getPipeline()->getModule<const ISPC::ModuleOUT>();
    IMG_RESULT ret;
    unsigned int b;
    bool isTiled;
    IMG_UINT32 buffId;
    bool hwSupportsTiling = (pCamera->getConnection()->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_TILING) != 0;
    
    bufferIds.clear();
    ret = pCamera->addShots(config->uiNBuffers[context]);

    if(IMG_SUCCESS != ret)
    {
        LOG_ERROR("Failed to allocate the shots!\n");
        return EXIT_FAILURE;
    }

    if (PXL_NONE != pOut->encoderType)
    {
        for(b=0 ; b < config->uiNBuffers[context] ; b++)
        {
            isTiled = (b < config->uiNEncTiled[context]) && hwSupportsTiling;

            ret = pCamera->allocateBuffer(CI_TYPE_ENCODER, isTiled, &buffId);
            if(IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate encoder buffer %d/%d (tiled=%d)\n",
                    b, config->uiNBuffers[context], (int)isTiled);
                goto allocate_clean;
            }
            bufferIds.push_back(buffId);
        }
    }

    if (PXL_NONE != pOut->displayType)
    {
        for(b=0 ; b < config->uiNBuffers[context] ; b++)
        {
            isTiled = (b < config->uiNDispTiled[context]) && hwSupportsTiling;

            ret = pCamera->allocateBuffer(CI_TYPE_DISPLAY, isTiled, &buffId);
            if(IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate display buffer %d/%d (tiled=%d)\n",
                    b, config->uiNBuffers[context], (int)isTiled);
                goto allocate_clean;
            }
            bufferIds.push_back(buffId);
        }
    }

    if (PXL_NONE != pOut->hdrExtractionType)
    {
        for(b=0 ; b < config->uiNBuffers[context] ; b++)
        {
            isTiled = (b < config->uiNHDRTiled[context]) && hwSupportsTiling;

            ret = pCamera->allocateBuffer(CI_TYPE_HDREXT, isTiled, &buffId);
            if(IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate HDR buffer %d/%d (tiled=%d)\n",
                    b, config->uiNBuffers[context], (int)isTiled);
                goto allocate_clean;
            }
            bufferIds.push_back(buffId);
        }
    }

    if (PXL_NONE != pOut->hdrInsertionType)
    {
        isTiled = false; // HDRIns cannot be tiled
        for (b = 0; b < config->uiNBuffers[context]; b++)
        {
            ret = pCamera->allocateBuffer(CI_TYPE_HDRINS, isTiled, &buffId);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate HDR Ins buffer %d/%d (tiled=%d)\n",
                    b, config->uiNBuffers[context], (int)isTiled);
                goto allocate_clean;
            }
            bufferIds.push_back(buffId);
        }
    }

    if (PXL_NONE != pOut->dataExtractionType)
    {
        isTiled = false; // DE cannot be tiled
        for(b=0 ; b < config->uiNBuffers[context] ; b++)
        {
            ret = pCamera->allocateBuffer(CI_TYPE_DATAEXT, isTiled, &buffId);
            if(IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate data-extraction buffer %d/%d (tiled=%d)\n",
                    b, config->uiNBuffers[context], (int)isTiled);
                goto allocate_clean;
            }
            bufferIds.push_back(buffId);
        }
    }

    if (PXL_NONE != pOut->raw2DExtractionType)
    {
        isTiled = false; // RAW2D cannot be tiled
        for(b=0 ; b < config->uiNBuffers[context] ; b++)
        {
            ret = pCamera->allocateBuffer(CI_TYPE_RAW2D, isTiled, &buffId);
            if(IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to allocate raw2d buffer %d/%d (tiled=%d)\n",
                    b, config->uiNBuffers[context], (int)isTiled);
                goto allocate_clean;
            }
            bufferIds.push_back(buffId);
        }
    }

    /// @ fix the camera so that it realises it has buffers!
    // because we allocated from outside we have to manually change the camera state
    pCamera->state = ISPC::CAM_READY;

    return EXIT_SUCCESS;
allocate_clean:
    std::list<IMG_UINT32>::iterator it = bufferIds.begin();
    while(it != bufferIds.end())
    {
        pCamera->deregisterBuffer(*it);
        it++;
    }
    bufferIds.clear();
    return EXIT_FAILURE;
}

int saveRawBuffer(const ISPC::Buffer &buffer, unsigned int stride, const std::string &filename)
{
    FILE *f = 0;

    if ( stride > buffer.stride || stride == 0 )
    {
        LOG_ERROR("given stride %d is bigger than the buffer's stride (%d) or is 0\n", stride, buffer.stride);
        return EXIT_FAILURE;
    }

    f = fopen(filename.c_str(), "wb");
    if ( f )
    {
        for (int i = 0 ; i < buffer.height ; i++ )
        {
            if ( fwrite(&(buffer.data[i*buffer.stride]), 1, stride, f) != stride )
            {
                LOG_ERROR("failed to save %d bytes for line %d/%d\n", stride, i+1, buffer.height);
                fclose(f);
                return EXIT_FAILURE;
            }
        }
        fclose(f);
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

void TestContext::saveRaw(const ISPC::Shot &capturedShot)
{
    std::ostringstream filename; // for raw saving
    
    int tileW = pCamera->getConnection()->sHWInfo.uiTiledScheme;
    //int tileH = 4096/tileW;
    unsigned int stride;

    static int savedRaw = 0;

    if ( capturedShot.YUV.data != NULL && capturedShot.YUV.isTiled )
    {
        stride = 0;
        filename.str("");
        filename << "encoder" << this->context << "_" << savedRaw
            << "_" << capturedShot.YUV.width << "x" << capturedShot.YUV.height
            << "_tiled" << tileW 
            << "_str" << capturedShot.YUV.stride
            << "-" << FormatString(capturedShot.YUV.pxlFormat)
            << ".yuv";
        stride = capturedShot.YUV.stride;
                
        if ( saveRawBuffer(capturedShot.YUV, stride, filename.str()) != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to open raw (tiled) YUV file: %s\n", filename.str().c_str());
        }
    }

    if ( capturedShot.BAYER.data != NULL )
    {
        stride = 0;
        filename.str("");
        filename << "dataExtraction" << this->context << "_" << savedRaw
            << "_" << capturedShot.BAYER.width << "x" << capturedShot.BAYER.height;
        if ( capturedShot.BAYER.isTiled )
        {
            filename << "_tiled" << tileW;
            stride = capturedShot.BAYER.stride;
        }
        else
        {
            PIXELTYPE stype;

            if ( PixelTransformBayer(&stype, capturedShot.BAYER.pxlFormat, MOSAIC_RGGB) == IMG_SUCCESS )
            {
                stride = (capturedShot.BAYER.width/stype.ui8PackedElements)*stype.ui8PackedStride;
                if ( capturedShot.BAYER.width%stype.ui8PackedElements != 0 )
                {
                    stride += stype.ui8PackedStride;
                }
            }
        }
        filename << "_str" << stride 
            << "_" << FormatString(capturedShot.BAYER.pxlFormat)
            << ".rggb";

        if ( saveRawBuffer(capturedShot.BAYER, stride, filename.str()) != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to open raw DE file: %s\n", filename.str().c_str());
        }
    }
    else if ( capturedShot.RGB.data != NULL )
    {
        stride = 0;
        filename.str("");
        filename << "display" << this->context << "_" << savedRaw
            << "_" << capturedShot.RGB.width << "x" << capturedShot.RGB.height;
        if ( capturedShot.RGB.isTiled )
        {
            filename << "_tiled" << tileW;
            stride = capturedShot.RGB.stride;
        }
        else
        {
            PIXELTYPE stype;

            if ( PixelTransformRGB(&stype, capturedShot.RGB.pxlFormat) == IMG_SUCCESS )
            {
                stride = (capturedShot.RGB.width/stype.ui8PackedElements)*stype.ui8PackedStride;
                if ( capturedShot.RGB.width%stype.ui8PackedElements != 0 )
                {
                    stride += stype.ui8PackedStride;
                }
            }
        }
        filename << "_str" << stride 
            << "_" << FormatString(capturedShot.RGB.pxlFormat)
            << ".rgb";

        if ( saveRawBuffer(capturedShot.RGB, stride, filename.str()) != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to open raw RGB file: %s\n", filename.str().c_str());
        }
    }

    if ( capturedShot.HDREXT.data != NULL )
    {
        stride = 0;
        filename.str("");
        filename << "hdrExtraction" << this->context << "_" << savedRaw
            << "_" << capturedShot.HDREXT.width << "x" << capturedShot.HDREXT.height;
        if ( capturedShot.HDREXT.isTiled )
        {
            filename << "_tiled" << tileW;
            stride = capturedShot.HDREXT.stride;
        }
        else
        {
            PIXELTYPE stype;

            if ( PixelTransformRGB(&stype, capturedShot.HDREXT.pxlFormat) == IMG_SUCCESS )
            {
                stride = (capturedShot.HDREXT.width/stype.ui8PackedElements)*stype.ui8PackedStride;
                if ( capturedShot.HDREXT.width%stype.ui8PackedElements != 0 )
                {
                    stride += stype.ui8PackedStride;
                }
            }
        }
        filename << "_str" << stride 
            << "_" << FormatString(capturedShot.HDREXT.pxlFormat)
            << ".rgb";

        if ( saveRawBuffer(capturedShot.HDREXT, stride, filename.str()) != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to open raw HDR Extraction file: %s\n", filename.str().c_str());
        }
    }

    if ( capturedShot.RAW2DEXT.data != NULL )
    {
        stride = 0;
        filename.str("");
        filename << "raw2dExtraction" << this->context << "_" << savedRaw
            << "_" << capturedShot.RAW2DEXT.width << "x" << capturedShot.RAW2DEXT.height;
        /*if ( capturedShot.RAW2DEXT.isTiled )
        {
            filename << "_tiled" << tileW;
            stride = capturedShot.RGB.stride;
        }
        else*/ // RAW2D cannot be tiled
        {
            PIXELTYPE stype;

            if ( PixelTransformBayer(&stype, capturedShot.RAW2DEXT.pxlFormat, MOSAIC_RGGB) == IMG_SUCCESS )
            {
                stride = (capturedShot.RAW2DEXT.width/stype.ui8PackedElements)*stype.ui8PackedStride;
                if ( capturedShot.RAW2DEXT.width%stype.ui8PackedElements != 0 )
                {
                    stride += stype.ui8PackedStride;
                }
            }
        }
        filename << "_str" << stride 
            << "_" << FormatString(capturedShot.RAW2DEXT.pxlFormat)
            << ".tiff";

        if ( saveRawBuffer(capturedShot.RAW2DEXT, stride, filename.str()) != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to open raw Raw2D file: %s\n", filename.str().c_str());
        }
    }

    savedRaw++;
}

void TestContext::handleShot(const ISPC::Shot &capturedShot, const ISPC::Camera &camera)
{
    char name[128];
    int tileW = pCamera->getConnection()->sHWInfo.uiTiledScheme;
    int tileH = 4096/pCamera->getConnection()->sHWInfo.uiTiledScheme;
    if ( config->bWriteRaw[this->context] )
    {
        saveRaw(capturedShot);
    }

    if ( this->dispOutput )
    {
        if (capturedShot.RGB.isTiled)
        {
            static int nTiledRGB = 0;
            FILE *f = 0;

            sprintf(name, "tiled%d_%dx%d_(%dx%d-%d).rgb", nTiledRGB++,
                capturedShot.RGB.width, capturedShot.RGB.height,
                tileW, tileH,
                capturedShot.RGB.stride);

            f = fopen(name, "wb");
            if(f)
            {
                fwrite(capturedShot.RGB.data,
                    capturedShot.RGB.stride*capturedShot.RGB.height,
                    sizeof(char), f);
                fclose(f);
            }
            else
            {
                LOG_WARNING("failed to open tiled output '%s'\n", name);
            }
        }

        if ( dispOutput->save(capturedShot) != IMG_SUCCESS )
        {
            LOG_ERROR("failed to save display/data extraction output to disk...\n");
        }
    }
    if(  this->encOutput )
    {
        if (capturedShot.YUV.isTiled)
        {
            static int nTiled = 0;
            FILE *f = 0;
            IMG_RESULT ret;
            PIXELTYPE stype;

            ret = PixelTransformYUV(&stype, capturedShot.YUV.pxlFormat);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("the given format '%s' is not YUV\n", 
                    FormatString(capturedShot.YUV.pxlFormat));
                return;
            }
            unsigned chromaHeight = (capturedShot.YUV.height) / stype.ui8VSubsampling;
            
            sprintf(name, "tiled%d_%dx%d_(%dx%d-%d).yuv", nTiled++,
                capturedShot.YUV.width, capturedShot.YUV.height,
                tileW, tileH,
                capturedShot.YUV.stride);

            f = fopen(name, "wb");
            if(f)
            {
                fwrite(capturedShot.YUV.data,
                    capturedShot.YUV.stride*capturedShot.YUV.height +
                    capturedShot.YUV.strideCbCr*chromaHeight,
                    sizeof(char), f);
                fclose(f);
            }
            else
            {
                LOG_WARNING("failed to open tiled output '%s'\n", name);
            }
        }
        if ( encOutput->save(capturedShot) != IMG_SUCCESS )
        {
            LOG_ERROR("failed to save encoder output to disk...\n");
        }
    }
    if (this->hdrExtOutput )
    {
        if (capturedShot.HDREXT.isTiled)
        {
            static int nTiledHDR = 0;
            FILE *f = 0;

            sprintf(name, "tiled%d_%dx%d_(%dx%d-%d)-hdr.rgb", nTiledHDR++,
                capturedShot.HDREXT.width, capturedShot.HDREXT.height,
                tileW, tileH,
                capturedShot.HDREXT.stride);

            f = fopen(name, "wb");
            if(f)
            {
                fwrite(capturedShot.HDREXT.data,
                    capturedShot.HDREXT.stride*capturedShot.HDREXT.height,
                    sizeof(char), f);
                fclose(f);
            }
            else
            {
                LOG_WARNING("failed to open tiled output '%s'\n", name);
            }
        }

        if ( hdrExtOutput->save(capturedShot) != IMG_SUCCESS )
        {
            LOG_ERROR("failed to save HDR Extraction output to disk...\n");
        }
    }
    if ( this->raw2DOutput )
    {
        if ( raw2DOutput->save(capturedShot) != IMG_SUCCESS )
        {
            LOG_ERROR("failed to save Raw2D Extraction output to disk...\n");
        }
    }

    if ( this->statsOutput )
    {
        if (bIgnoreDPF)
        {
            // override statistics
            ISPC::Shot s(capturedShot);

            s.stats.data = (IMG_UINT8*)IMG_MALLOC(s.stats.size);
            if (s.stats.data)
            {
                IMG_UINT32 *copy = (IMG_UINT32 *)s.stats.data;
                const IMG_UINT32 *toWrite = 
                    (const IMG_UINT32 *)capturedShot.stats.data;

                IMG_MEMCPY(copy, toWrite, s.stats.size);

                copy[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET / 4] = 0;
                LOG_WARNING("Override DPF FIXED from 0x%x to 0x%x\n",
                    toWrite[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET / 4],
                    copy[FELIX_SAVE_DPF_FIXED_PIXELS_OFFSET / 4]);

                copy[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET / 4] = 0;
                LOG_WARNING("Override DPF MODIFICATIONS from 0x%x to 0x%x\n",
                    toWrite[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET / 4],
                    copy[FELIX_SAVE_DPF_MAP_MODIFICATIONS_OFFSET / 4]);

                copy[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET / 4] = 0;
                LOG_WARNING("Override DPF DROPPED from 0x%x to 0x%x\n",
                    toWrite[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET / 4],
                    copy[FELIX_SAVE_DPF_DROPPED_MAP_MODIFICATIONS_OFFSET / 4]);
            }
            else
            {
                LOG_WARNING("Failed to allocate stats copy of %u Bytes\n",
                    s.stats.size);
            }

            if (statsOutput->saveStats(s) != IMG_SUCCESS)
            {
                LOG_ERROR("failed to save statistics output to disk...\n");
            }

            IMG_FREE((void*)s.stats.data);
        }
        else
        {
            if (statsOutput->saveStats(capturedShot) != IMG_SUCCESS)
            {
                LOG_ERROR("failed to save statistics output to disk...\n");
            }
        }
    }

    const ISPC::ModuleDPF *pDPF = pCamera->getModule<ISPC::ModuleDPF>();
    if (pDPF && pDPF->bWrite && !bIgnoreDPF)
    {
        static int nDPFSaved = 0;
        char filename[64];
        sprintf(filename, "ctx%d-%d-dpf_write_%d.dat", this->context, nDPFSaved++, capturedShot.metadata.defectiveStats.ui32NOutCorrection);
        if ( ISPC::Save::SingleDPF(*(pCamera->getPipeline()), capturedShot, filename) != IMG_SUCCESS )
        {
            LOG_ERROR("error saving DPF write map.\n");
        }
    }

    if ( capturedShot.ENS.size > 0 )
    {
        static int nENSSaved = 0;
        char filename[64];
        sprintf(filename, "ctx%d-%d-ens_%d.dat", this->context, nENSSaved++, capturedShot.ENS.size/capturedShot.ENS.elementSize);
        if ( ISPC::Save::SingleENS(*(pCamera->getPipeline()), capturedShot, filename) != IMG_SUCCESS )
        {
            LOG_ERROR("error saving ENS output.\n");
        }
    }

    if ( capturedShot.bFrameError )
    {
        LOG_WARNING("frame is erroneous!\n");
    }
    if ( capturedShot.iMissedFrames != 0 )
    {
        LOG_WARNING("missed %d frames!\n", capturedShot.iMissedFrames);
    }

    // also save current parameters
    {
        static int nSetupSaved = 0;
        std::ostringstream filename;
        
        ISPC::ParameterList parameters;
        if ( camera.saveParameters(parameters) == IMG_SUCCESS )
        {
            const ISPC::ModuleLSH *pLSH = camera.getPipeline()->getModule<ISPC::ModuleLSH>();

            if ( pLSH )
            {
                filename << "ctx" << this->context << "-" << nSetupSaved << "-lshgrid.lsh";
                if ( pLSH->saveMatrix(filename.str()) )
                {
                    parameters.addParameter(ISPC::Parameter(ISPC::ModuleLSH::LSH_FILE.name, filename.str()));
                }
            }

            filename.str("");
            filename << "ctx" << this->context << "-" << nSetupSaved << "-FelixSetupArgs.txt";
            ISPC::ParameterFileParser::saveGrouped(parameters, filename.str());

            nSetupSaved++;
        }
    }
}

void TestContext::configureAlgorithms(const ISPC::ParameterList &parameters, bool isOnwerOfCamera)
{
    // Create, setup and register control modules-----------------
    ISPC::ControlAWB *pAWB = 0;
    ISPC::ControlLBC *pLBC = new ISPC::ControlLBC();
    ISPC::ControlAE *pAE = 0;;
    ISPC::ControlTNM *pTNM = new ISPC::ControlTNM();
    ISPC::ControlDNS *pDNS = 0;

    /// @ remove most of the direct set of parameters as setupfile should contain them!

    if(isOnwerOfCamera)
    {
        pAE = new ISPC::ControlAE();
        pDNS = new ISPC::ControlDNS(1.0);

        pCamera->registerControlModule(pAE);
        pCamera->registerControlModule(pDNS);     // not really used because using data generator the gain does not change
    }

    if (this->config->autoWhiteBalanceAlgorithm[this->context] != ISPC::ControlAWB::WB_NONE)
    {
        ISPC::ControlAWB::Correction_Types type = ISPC::ControlAWB::WB_NONE;
        switch(this->config->autoWhiteBalanceAlgorithm[this->context])
        {
        case ISPC::ControlAWB::WB_AC:
            type = ISPC::ControlAWB::WB_AC;
            LOG_INFO("use WB Average Colour\n");
            break;
        case ISPC::ControlAWB::WB_WP:
            type = ISPC::ControlAWB::WB_WP;
            LOG_INFO("use WB White Patch\n");
            break;
        case ISPC::ControlAWB::WB_HLW:
            type = ISPC::ControlAWB::WB_HLW;
            LOG_INFO("use WB High Luminance White\n");
            break;
        case ISPC::ControlAWB::WB_COMBINED:
            type = ISPC::ControlAWB::WB_COMBINED;
            LOG_INFO("use WB Combined\n");
            break;
        default:
            LOG_WARNING("given WBC algorithm %d does not exists - force none\n", this->config->autoWhiteBalanceAlgorithm[this->context]);
        }

        if (type != ISPC::ControlAWB::WB_NONE)
        {
            pAWB = new ISPC::ControlAWB();
            pCamera->registerControlModule(pAWB);

            pAWB->load(parameters);
            pAWB->setCorrectionMode(type);
            pAWB->setTargetTemperature(6000);
        }
    }
    pCamera->registerControlModule(pLBC);
    pCamera->registerControlModule(pTNM);
    
    pCamera->loadControlParameters(parameters);

    if (pAE)
    {
        // DG does not support changement of exposure or gain
        //pAE->setTargetBrightness(this->config->targetBrightness[this->context]);
        pAE->enableControl(false);
    }

    //Enable/disable flicker rejection in the autoexposure algorithm
    //pAE->enableFlickerRejection(this->config->flickerRejection[this->context]);

    if (pTNM)
    {
        //Enable/disable tone mapping options
        pTNM->enableControl(this->config->autoToneMapping[this->context]==IMG_TRUE);
        pTNM->enableLocalTNM(this->config->localToneMapping[this->context]==IMG_TRUE);
        pTNM->enableAdaptiveTNM(this->config->adaptiveToneMapping[this->context]==IMG_TRUE);
        pTNM->setAllowHISConfig(true); // because we don't use AE some module need to setup the HIS block
    }

    if (pDNS)
    {
        // DG has no support of ISO gain so having control class running is not interesting
        pDNS->enableControl(false);
    }

    if (pLBC)
    {
        //Enable disable light based control to set up certain pipeline characteristics based on light level
        pLBC->enableControl(this->config->autoLBC[this->context]==IMG_TRUE);

        if ( !pTNM || !pTNM->isEnabled() )
        {
            pLBC->setAllowHISConfig(true);
        }
    }
    
}

IMG_RESULT saveReST(const ISPC::ParameterList &parameters, const std::string &filename, bool bSaveUngroupped=true)
{
    std::ofstream file;
    file.open(filename.c_str(), std::ios::out);

    std::map<std::string, ISPC::ParameterGroup>::const_iterator groupIt = parameters.groupsBegin();
    std::map<std::string, ISPC::Parameter>::const_iterator paramIt;

    file << ".. generated from CI version " << CI_CHANGELIST_STR << std::endl
        << std::endl;

    while ( groupIt != parameters.groupsEnd() && !file.fail() )
    {
        std::set<std::string>::const_iterator it = groupIt->second.parameters.begin();

        file << ".. //" << std::endl;
        file << ".. " << groupIt->second.header << std::endl;
        file << ".. //" << std::endl << std::endl;

        while (it != groupIt->second.parameters.end() && !file.fail()  )
        {
            const ISPC::Parameter *p = parameters.getParameter(*it);
            if ( p )
            {
                std::string comment = "";
                std::string data = "";
                size_t pos;

                file << ".. |" << p->getTag() << "| replace:: " << p->getTag() << std::endl;
                
                file << ".. |" << p->getTag() << "_DEF| replace::";
                for ( unsigned int i = 0 ; i < p->size() ; i++ )
                {
                    data = p->getString(i);
                    pos = data.find_first_of("//");
                    
                    if ( pos != std::string::npos )
                    {
                        data = p->getString(i).substr(0, pos);
                        comment += p->getString(i).substr(pos+2, p->getString(i).size()-pos);
                    }

                    file << " " << data;
                }
                file << std::endl;

                file << ".. parsing comment:'" << comment << "'" << std::endl;
                pos = comment.find_first_not_of(" "); // to remove leading spaces
                if ( pos != std::string::npos )
                {
                    comment = comment.substr(pos, comment.size()-pos);
                }

                pos = comment.find_first_of(" ", pos+1);
                if ( pos != std::string::npos )
                {
                    /* if it is a string it can be a list of values as
                     * format which would have the 1st space as the middle
                     * of the list and then only the 1st entry would be
                     * saved... */
                    if (comment[pos - 1] != ',' && comment[pos - 1] != '}')
                    {
                        data = comment.substr(0, pos);
                    }
                    else
                    {
                        data = comment;
                    }
                    
                    file << ".. |" << p->getTag() << "_FMT| replace:: " << data << std::endl;

                    comment = comment.substr(pos, comment.size()-pos);

                    pos = comment.find_first_of("["); // search for range=[data]
                    if ( pos != std::string::npos )
                    {
                        size_t end = comment.find_first_of("]", pos);
                        data = comment.substr(pos, end-pos+1);

                        file << ".. |" << p->getTag() << "_RANGE| replace:: " << data << std::endl;
                    }
                    else
                    {
                        pos = comment.find_first_of("{"); // search for {data} (boolean or strings)
                        if ( pos != std::string::npos )
                        {
                            size_t end = comment.find_first_of("}", pos);
                            data = comment.substr(pos, end-pos+1);

                            file << ".. |" << p->getTag() << "_RANGE| replace:: " << data << std::endl;
                        }
                    }
                }

                file << std::endl;
            }
            // else the group created the parameter but it is not in the list (e.g. the list has been cleared since)

            it++; // next parameter
        }
        file << std::endl << std::endl;

        groupIt++; // next group
    }

    if ( bSaveUngroupped )
    {
        bool first=true;
        paramIt = parameters.begin();

        file << std::endl;


        while ( paramIt != parameters.end() && !file.fail() )
        {
            if ( !paramIt->first.empty() && paramIt->second.size() > 0 && !parameters.isInGroup(paramIt->first) )
            {
                std::vector<std::string>::const_iterator jt = paramIt->second.begin();

                if ( first )
                {
                    file << ".. //" << std::endl;
                    file << ".. // Ungroupped parameters" << std::endl;
                    file << ".. //" << std::endl << std::endl;
                    first=false;
                }

                file << ".. |" << paramIt->first << "| replace:: " << paramIt->first << std::endl;
                //file << paramIt->first;

                file << ".. |" << paramIt->first << "_DEF| replace::";
                while ( jt != paramIt->second.end() )
                {
                    file << " " << *jt;
                    jt++;
                }
                file << std::endl;
            }

            paramIt++;
        }
    }

    file.close();

    return IMG_SUCCESS;
}

int dumpConfigFiles()
{
    ISPC::ParameterList minParam, maxParam, defParam;
    
    // could also information from a ISPC::Connection but it would need to connect to the HW while this does not
    std::list<ISPC::SetupModule*> modules = ISPC::CameraFactory::setupModulesFromHWVersion(FELIX_VERSION_MAJ, FELIX_VERSION_MIN);
    std::list<ISPC::SetupModule*>::iterator mod_it;

    LOG_INFO("Generating configuration default, min, max and reST files...\n");

    ISPC::DGSensor cam("");

    cam.save(minParam, ISPC::ModuleBase::SAVE_MIN);
    cam.save(maxParam, ISPC::ModuleBase::SAVE_MAX);
    cam.save(defParam, ISPC::ModuleBase::SAVE_DEF);

    for ( mod_it = modules.begin() ; mod_it != modules.end() ; mod_it++ )
    {
        if ( *mod_it )
        {
            (*mod_it)->save(minParam, ISPC::ModuleBase::SAVE_MIN);
            (*mod_it)->save(maxParam, ISPC::ModuleBase::SAVE_MAX);
            (*mod_it)->save(defParam, ISPC::ModuleBase::SAVE_DEF);

            delete *mod_it;
            *mod_it = 0;
        }
    }

    std::list<ISPC::ControlModule*> controls = ISPC::CameraFactory::controlModulesFromHWVersion(1, 0);
    std::list<ISPC::ControlModule*>::iterator ctr_it;

    for ( ctr_it = controls.begin() ; ctr_it != controls.end() ; ctr_it++ )
    {
        if ( *ctr_it )
        {
            (*ctr_it)->save(minParam, ISPC::ModuleBase::SAVE_MIN);
            (*ctr_it)->save(maxParam, ISPC::ModuleBase::SAVE_MAX);
            (*ctr_it)->save(defParam, ISPC::ModuleBase::SAVE_DEF);

            delete *ctr_it;
            *ctr_it = 0;
        }
    }

    if(  ISPC::ParameterFileParser::saveGrouped(minParam, "FelixSetupArgs2_min.txt") != IMG_SUCCESS )
    {
        LOG_ERROR("failed to save minimum parameters\n");
        return EXIT_FAILURE;
    }
    if ( ISPC::ParameterFileParser::saveGrouped(maxParam, "FelixSetupArgs2_max.txt") != IMG_SUCCESS )
    {
        LOG_ERROR("failed to save maximum parameters\n");
        return EXIT_FAILURE;
    }
    if ( ISPC::ParameterFileParser::saveGrouped(defParam, "FelixSetupArgs2_def.txt") != IMG_SUCCESS )
    {
        LOG_ERROR("failed to save default parameters\n");
        return EXIT_FAILURE;
    }
    
    if ( saveReST(defParam, "FelixSetupArgs_reST.txt") != IMG_SUCCESS )
    {
        LOG_ERROR("failed to save default parameters as reST\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int runSingle(const struct TEST_PARAM& testparams)
{
    int toRet = EXIT_SUCCESS;
    int ctx;
    TestContext testCtx;

    for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
    {
        int gasket = testparams.uiImager[ctx];

        if( testparams.pszFelixSetupArgsFile[ctx] == NULL )
        {
            LOG_INFO("skip context %d: no setup found\n", ctx);
            continue;
        }
        if ( gasket >= CI_N_IMAGERS )
        {
            LOG_ERROR("given gasket %d for context %d is not supported by driver\n",
                gasket, ctx);
            return EXIT_FAILURE;
        }
        if ( testparams.aIsIntDG[gasket] )
        {
            if ( testparams.aIndexDG[gasket] >= CI_N_IIF_DATAGEN )
            {
                LOG_ERROR("given internal data-generator %d for gasket %d is not supported by driver\n",
                    testparams.aIndexDG[gasket], gasket);
                return EXIT_FAILURE;
            }
        }
        else
        {
            if ( gasket > CI_N_EXT_DATAGEN )
            {
                LOG_ERROR("given external data-generator %d for gasket %d is not supported by driver\n",
                    gasket, gasket);
                return EXIT_FAILURE;
            }
        }
        if ( testparams.pszInputFile[gasket] == NULL )
        {
            LOG_INFO("skip context %d because its attached gasket %d has no input file\n",
                ctx, gasket);
            continue;
        }

        if ( testparams.bReUse[ctx] )
        {
            if ( ctx>0 && testparams.uiImager[ctx-1]==gasket )
            {
                LOG_INFO("context %d re-uses object created for context %d\n",
                    ctx, ctx-1);
                
                std::list<IMG_UINT32>::iterator it;
                for ( it = testCtx.bufferIds.begin() ; it != testCtx.bufferIds.end() ; it++ )
                {
                    if ( testCtx.pCamera->deregisterBuffer(*it) != IMG_SUCCESS )
                    {
                        LOG_ERROR("failed to deregister a buffer when trying to reuse previous context object\n");
                        return EXIT_FAILURE;
                    }
                }
                testCtx.bufferIds.clear();
                testCtx.pCamera->deleteShots();
            }
            else
            {
                if ( ctx>0 )
                {
                    LOG_WARNING("context %d cannot re-use previous context object: context %d uses gasket %d but context %d wants gasket %d\n",
                        ctx, ctx-1, testparams.uiImager[ctx-1], ctx, gasket);
                    if ( testCtx.pCamera )
                    {
                        delete testCtx.pCamera; // also deregisters the buffers
                        testCtx.pCamera = 0;
                    }
                }
            }
        }
        else if ( testCtx.pCamera ) // not reuse and we still have a camera
        {
            delete testCtx.pCamera;
            testCtx.pCamera = 0;
        }
        
        if ( testCtx.pCamera == 0 )
        {
            ISPC::CI_Connection sConn;
            ISPC::Sensor *sensor = 0;

            testCtx.pCamera = new ISPC::DGCamera(ctx, testparams.pszInputFile[gasket], gasket, testparams.aIsIntDG[gasket]); // how to choose the DG context
            sensor = testCtx.pCamera->getSensor();

            if ( ISPC::CameraFactory::populateCameraFromHWVersion(*(testCtx.pCamera), sensor) != IMG_SUCCESS )
            {
                LOG_ERROR("failed to setup modules from HW version!\n");
                return EXIT_FAILURE;
            }

            if (testparams.aIsIntDG[gasket])
            {
                IMG_RESULT ret;
                                
                ret = IIFDG_ExtendedSetDatagen(sensor->getHandle(), testparams.aIndexDG[gasket]);
                if(IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG datagen\n");
                    return EXIT_FAILURE;
                }
                ret = IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), testparams.uiNBuffers[ctx]);
                if(IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG nBuffers\n");
                    return EXIT_FAILURE;
                }
                ret = IIFDG_ExtendedSetBlanking(sensor->getHandle(), testparams.aBlanking[gasket][0], testparams.aBlanking[gasket][1]);
                if(IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG blanking\n");
                    return EXIT_FAILURE;
                }
                if(testparams.aNBInputFrames[gasket]>0)
                {
                    IMG_UINT32 cap = testparams.aNBInputFrames[gasket];
                    cap = std::min(cap, IIFDG_ExtendedGetFrameCount(sensor->getHandle()));
                    if(cap!=testparams.aNBInputFrames[gasket])
                    {
                        LOG_WARNING("forcing IntDG%d maximum loaded frame to %d as source image does not have more\n",
                            testparams.aIndexDG[gasket], cap);
                    }
                    ret = IIFDG_ExtendedSetFrameCap(sensor->getHandle(), cap);
                    if(IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to set frame cap\n");
                        return EXIT_FAILURE;
                    }
                }
            }
            else
            {
#ifdef EXT_DATAGEN
                IMG_RESULT ret;
                
                ret = DGCam_ExtendedSetBlanking(sensor->getHandle(), testparams.aBlanking[gasket][0], testparams.aBlanking[gasket][1]);
                if(IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set ExtDG blanking");
                    return EXIT_FAILURE;
                }
                if(testparams.aNBInputFrames[gasket]>0)
                {
                    IMG_UINT32 cap = testparams.aNBInputFrames[gasket];
                    cap = std::min(cap, DGCam_ExtendedGetFrameCount(sensor->getHandle()));
                    if(cap != testparams.aNBInputFrames[gasket])
                    {
                        LOG_WARNING("forcing DG%d maximum loaded frame to %d as source image does not have more\n",
                            gasket, cap);
                    }

                    ret = DGCam_ExtendedSetFrameCap(sensor->getHandle(), cap);
                    if(IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to set frame cap\n");
                        return EXIT_FAILURE;
                    }
                }
#else
                LOG_ERROR("compiled without external data generator support!\n");
                return EXIT_FAILURE;
#endif
            }
        }
        
        
        ISPC::ParameterList parameters = ISPC::ParameterFileParser::parseFile(testparams.pszFelixSetupArgsFile[ctx]);

        if(parameters.validFlag == false)
        {
            LOG_ERROR("error while parsing parameter file '%s'\n",
                testparams.pszFelixSetupArgsFile[ctx]);
            return EXIT_FAILURE;
        }

        if(testCtx.pCamera->loadParameters(parameters) != IMG_SUCCESS)
        {
            LOG_ERROR("error while loading setup parameters from file '%s'\n",
                testparams.pszFelixSetupArgsFile[ctx]);
            return EXIT_FAILURE;
        }

        testparams.populate(testCtx, ctx);
        if ( testCtx.configureOutput() != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to configure output fields\n");
            return EXIT_FAILURE;
        }

        // if using internal DG
        if (testparams.aIsIntDG[gasket])
        {
            //IMG_UINT16 hblank = testCtx.pCamera->sensor->uiWidth;
            //testCtx.pCamera->sensor->setExtendedParam(EXTENDED_IIFDG_H_BLANKING, &hblank);
            LOG_INFO("gasket %d uses Int. Datagen %d\n",
                gasket, testparams.aIndexDG[gasket]);
        }
#ifdef EXT_DATAGEN
        else// using external DG
        {
            IMG_BOOL bUseMipi;
            bUseMipi = DGCam_ExtendedGetUseMIPI(testCtx.pCamera->getSensor()->getHandle());
            LOG_INFO("gasket %d uses Ext. Datagen with mipi=%d\n",
                    gasket, (int)bUseMipi);
        }
#endif

        if ( testCtx.lshGridFile != NULL )
        {
            ISPC::ModuleLSH *lsh = testCtx.pCamera->getModule<ISPC::ModuleLSH>();

            if ( lsh )
            {
                IMG_RESULT ret;
                LOG_INFO("overrides context %d LSH grid '%s' with '%s'\n",
                    testCtx.context, lsh->lshFilename.c_str(), testCtx.lshGridFile);
                ret = lsh->loadMatrix(testCtx.lshGridFile);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to override LSH matrix!\n");
                    return EXIT_FAILURE;
                }
                lsh->setup();
            }
            else
            {
                LOG_WARNING("no LSH module registered! Cannot override grid\n");
            }
        }

        if(testCtx.pCamera->program() != IMG_SUCCESS)
        {
            LOG_ERROR("failed to program the camera\n");
            return EXIT_FAILURE;
        }

        // allocate buffers
        if (EXIT_SUCCESS != testCtx.allocateBuffers())
        {
            LOG_ERROR("failed to allocate buffers\n");
            return EXIT_FAILURE;
        }
        
        // configure algorithms
        testCtx.configureAlgorithms(parameters);

        if(testCtx.pCamera->startCapture() !=IMG_SUCCESS)
        {
            LOG_ERROR("failed to start capture\n");
            return EXIT_FAILURE;
        }

        unsigned int frame = 0;
        const ISPC::ModuleOUT *pOut = testCtx.pCamera->getModule<const ISPC::ModuleOUT>();
        IMG_ASSERT(pOut);
        while ( frame < testparams.uiNFrames[ctx] && toRet == EXIT_SUCCESS )
        {
            unsigned b;
            unsigned pushed = 0;

            for ( b = 0 ; b < testparams.uiNBuffers[ctx] && frame+b < testparams.uiNFrames[ctx] ; b++ )
            {
                LOG_INFO("Enqueue frame: %d\n", frame+b);
                if (pOut->hdrInsertionType == PXL_NONE)
                {
                    if (testCtx.pCamera->enqueueShot() != IMG_SUCCESS)
                    {
                        LOG_ERROR("failed to enque shot %d/%d\n",
                            frame, testparams.uiNFrames[ctx]);
                        toRet = EXIT_FAILURE;
                        break;
                    }
                }
                else
                {
                    int hdrFrame = (frame + b) % testCtx.hdrInsMax;
                    ISPC::DGCamera *pCam = static_cast<ISPC::DGCamera*>(testCtx.pCamera);
                    if (ProgramShotWithHDRIns(*(pCam),
                        testCtx.config->pszHDRInsertionFile[ctx],
                        testCtx.config->bRescaleHDRIns[ctx]?true:false,
                        hdrFrame) != IMG_SUCCESS)
                    {
                        LOG_ERROR("failed to enque shot with HDR insertion %d/%d\n",
                            frame, testparams.uiNFrames[ctx]);
                        toRet = EXIT_FAILURE;
                        break;
                    }
                }
                pushed++;
            }

            while(pushed>0)
            {
                ISPC::Shot captureBuffer;

                LOG_INFO("Acquire frame: %d\n", frame);

                if(testCtx.pCamera->acquireShot(captureBuffer) != IMG_SUCCESS)
                {
                    LOG_ERROR("failed to retrie processed shot %d/%d from pipeline.\n",
                        frame, testparams.uiNFrames[ctx]);
                    toRet = EXIT_FAILURE;
                    break;
                }

                testCtx.handleShot(captureBuffer, *(testCtx.pCamera));

                if(testCtx.pCamera->releaseShot(captureBuffer) !=IMG_SUCCESS)
                {
                    LOG_ERROR("failed to relea shot %d/%d\n",
                        frame, testparams.uiNFrames[ctx]);
                    toRet = EXIT_FAILURE;
                    break;
                }

                frame++;
                pushed--;
            }
        }

        // clean the run

        if(testCtx.pCamera->stopCapture() !=IMG_SUCCESS)
        {
            LOG_ERROR("failed to stop capture.\n");
            toRet = EXIT_FAILURE;
        }

        testCtx.close();
    }

    return toRet;
}

int runParallel(const struct TEST_PARAM& testparams)
{
    TestContext testCtx[CI_N_CONTEXT];
    int ctx;
    int gasket;
    int gasketOwner[CI_N_EXT_DATAGEN]; // owner of DG, last CTX to use it so that the capture is trigger for both at once
    int ret = EXIT_SUCCESS; // return code
    unsigned int nBuffers[CI_N_CONTEXT]; // number of buffers to push for the context - has to be modified from input because CSIM cannot allow 1 context to be empty
    unsigned int nToCompute[CI_N_CONTEXT];
    unsigned int continueRunning = 0; // number of context still needing processing

    for ( ctx = 0 ; ctx < CI_N_EXT_DATAGEN ; ctx++ )
    {
        gasketOwner[ctx] = -1; // no owner
    }

    for ( ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++ )
    {
        gasket = testparams.uiImager[ctx];
        nBuffers[ctx] = testparams.uiNBuffers[ctx]; // may get limited later if context share same DG
        nToCompute[ctx] = testparams.uiNFrames[ctx];

        if ( gasket >= CI_N_IMAGERS )
        {
            LOG_ERROR("given gasket %d for context %d is not supported by driver!\n",
                gasket, ctx);
            return EXIT_FAILURE;
        }
        if ( testparams.aIsIntDG[gasket] )
        {
            if ( testparams.aIndexDG[gasket] >= CI_N_IIF_DATAGEN )
            {
                LOG_ERROR("given internal data-generator %d  for gasket %d is not supported by driver!\n",
                    testparams.aIndexDG[gasket], gasket);
                return EXIT_FAILURE;
            }
        }
        else
        {
            if ( gasket > CI_N_EXT_DATAGEN )
            {
                LOG_ERROR("given external data-generator %d  for gasket %d is not supported by driver!\n",
                    gasket, gasket);
                return EXIT_FAILURE;
            }
        }

        // find which Camera should be a DGCamera and which can be a normal Camera with shared sensor
        // the last context to use the DG is the owner (so that the frame is pushed last and ensure it happens)
        if (testparams.pszFelixSetupArgsFile[ctx] && testparams.pszInputFile[gasket])
        {
            gasketOwner[ testparams.uiImager[ctx] ] = ctx;
        }
        else
        {
            LOG_INFO("Skipping context %d - setup is %s and input FLX is %s\n",
                ctx, testparams.pszFelixSetupArgsFile[ctx], testparams.pszInputFile[gasket]);
        }

        if (testCtx[ctx].pCamera)
        {
            delete testCtx[ctx].pCamera;
            testCtx[ctx].pCamera = 0;
        }
    }

    //
    // from now on only return should go through goto parallel_exit; or be a success!
    //

    // the create the DG cameras
    for ( gasket = 0 ; gasket < CI_N_EXT_DATAGEN ; gasket++ )
    {
        ctx = gasketOwner[gasket];
        if(0 <= ctx)
        {
            ISPC::Sensor *sensor = 0;

            testCtx[ctx].pCamera = new ISPC::DGCamera(ctx, testparams.pszInputFile[gasket], gasket, testparams.aIsIntDG[gasket]);
            if(ISPC::CAM_ERROR==testCtx[ctx].pCamera->state)
            {
                LOG_ERROR("failed to create DGCamera for context %d!\n", ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            sensor = testCtx[ctx].pCamera->getSensor();
            
            if (testparams.aIsIntDG[gasket])
            {
                IMG_RESULT ret;
                
                ret = IIFDG_ExtendedSetDatagen(sensor->getHandle(), testparams.aIndexDG[gasket]);
                if(IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG datagen\n");
                    return EXIT_FAILURE;
                }
                ret = IIFDG_ExtendedSetNbBuffers(sensor->getHandle(), testparams.uiNBuffers[ctx]);
                if(IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG nBuffers\n");
                    return EXIT_FAILURE;
                }
                ret = IIFDG_ExtendedSetBlanking(sensor->getHandle(), testparams.aBlanking[gasket][0], testparams.aBlanking[gasket][1]);
                if(IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG blanking\n");
                    return EXIT_FAILURE;
                }

                LOG_INFO("gasket %d uses Int. Datagen %d\n",
                    gasket, testparams.aIndexDG[gasket]);
            }
            else
            {
#ifdef EXT_DATAGEN
                IMG_BOOL bUseMipi = IMG_FALSE;
                IMG_RESULT ret;

                ret = DGCam_ExtendedSetBlanking(sensor->getHandle(), testparams.aBlanking[gasket][0], testparams.aBlanking[gasket][1]);
                if(IMG_SUCCESS != ret)
                {
                }
                
                bUseMipi = DGCam_ExtendedGetUseMIPI(sensor->getHandle());
                LOG_INFO("gasket %d uses Ext. Datagen with mipi=%d\n",
                    gasket, (int)bUseMipi);
#else
                LOG_ERROR("compiled without external data generator support! Cannot use external data generator.\n");
                ret = EXIT_FAILURE;
                goto parallel_exit;
#endif
            }

            // for every context that has this data generator limit the number of buffers to the same value otherwise CSIM cannot run
            // we enforce number of buffers and number of frames to be the same
            unsigned minBuffers = nBuffers[gasketOwner[gasket]];
            unsigned minFrames = nToCompute[gasketOwner[gasket]];
            
            // once we have the minimums we apply it to all contexts
            for(ctx=0 ; ctx < CI_N_CONTEXT ; ctx++)
            {
                if(testparams.uiImager[ctx]==gasket)
                {
                    nBuffers[ctx] = minBuffers;
                    if (testparams.uiNBuffers[ctx] != minBuffers)
                    {
                        LOG_WARNING("change number of buffers for context %d from %d to %d to allow to run with shared data generator\n",
                            ctx, testparams.uiNBuffers[ctx], minBuffers);
                    }

                    // if camera is not created yet it does not own a sensor
                    if(!testCtx[ctx].pCamera)
                    {
                        nToCompute[ctx] = minFrames;
                        if (nToCompute[ctx] != minFrames)
                        {
                            LOG_WARNING("change number of frames to compute for context %d from %d to %d as context %d owns the Data Generator\n",
                                ctx, testparams.uiNFrames[ctx], nToCompute[ctx], gasketOwner[gasket]);
                        }
                    }
                }
            }
        }
    } // for creation of DGCameras

    // create the normal cameras
    // and populates the setups
    for (ctx = 0 ; ctx < CI_N_CONTEXT ; ctx++)
    {
        gasket = testparams.uiImager[ctx];
        int dgOwner = gasketOwner[gasket];

        if (testparams.pszFelixSetupArgsFile[ctx] && gasket>=0 && dgOwner>=0)
        {
            bool isOwner = false;
            ISPC::Sensor *sensor = 0;
            if (testCtx[ctx].pCamera==0)
            {
                sensor = testCtx[dgOwner].pCamera->getSensor();

                // may need to become DGCamera if we want to be able to transfert ownership
                testCtx[ctx].pCamera = new ISPC::Camera(ctx, sensor);

                if(ISPC::CAM_ERROR==testCtx[ctx].pCamera->state)
                {
                    LOG_ERROR("failed to create DGCamera for context %d!\n", ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                
            }
            else
            {
                isOwner = true;
                sensor = testCtx[ctx].pCamera->getSensor();
            }

            if ( ISPC::CameraFactory::populateCameraFromHWVersion(*(testCtx[ctx].pCamera), sensor) != IMG_SUCCESS )
            {
                LOG_ERROR("failed to setup modules from HW version for context %d!\n", ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            ISPC::ParameterList parameters = ISPC::ParameterFileParser::parseFile(testparams.pszFelixSetupArgsFile[ctx]);

            if(parameters.validFlag == false)
            {
                LOG_ERROR("failed to parse context %d parameter file '%s'\n",
                    ctx, testparams.pszFelixSetupArgsFile[ctx]);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            if(IMG_SUCCESS != testCtx[ctx].pCamera->loadParameters(parameters))
            {
                LOG_ERROR("failed to load context %d setup parameters from file: '%s'\n",
                    ctx, testparams.pszFelixSetupArgsFile[ctx]);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            testparams.populate(testCtx[ctx], ctx);
            if ( testCtx[ctx].configureOutput() != EXIT_SUCCESS )
            {
                LOG_ERROR("failed to configure output fields for context %d\n", ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            if (testCtx[ctx].lshGridFile)
            {
                ISPC::ModuleLSH *lsh = testCtx[ctx].pCamera->getModule<ISPC::ModuleLSH>();

                if (lsh)
                {
                    LOG_INFO("overrides context %d LSH grid '%s' width '%s'\n",
                        ctx, lsh->lshFilename.c_str(), testCtx[ctx].lshGridFile);
                    lsh->lshFilename = testCtx[ctx].lshGridFile;
                    lsh->requestUpdate();
                }
                else
                {
                    LOG_WARNING("No LSH module registered for context %d! Cannot override grid.\n", ctx);
                }
            }

            if(IMG_SUCCESS != testCtx[ctx].pCamera->program())
            {
                LOG_ERROR("failed to program context %d\n", 
                    ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            if (EXIT_SUCCESS != testCtx[ctx].allocateBuffers())
            {
                LOG_ERROR("faield to alloce buffers for context %d\n",
                    ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            // configure algorithms
            testCtx[ctx].configureAlgorithms(parameters, isOwner);

            if(IMG_SUCCESS != testCtx[ctx].pCamera->startCapture())
            {
                LOG_ERROR("failed to start context %d\n",
                    ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            continueRunning++;
        }
    } // for each context: init

    while(continueRunning>0)
    {
        // process frames
        // for the moment: assumes that all contexts that share a DG have the same number of buffers and same number of frames to process
        // assumes that the context that owns the DG is the one with the highest CTX number
        unsigned int nPushed[CI_N_CONTEXT];
        unsigned int f;

        // first push frames
        for(ctx=0 ; ctx < CI_N_CONTEXT ; ctx++)
        {
            if(testCtx[ctx].pCamera &&
               testCtx[ctx].pCamera->state == ISPC::CAM_CAPTURING)
            {
                nPushed[ctx] = IMG_MIN_INT(nBuffers[ctx], nToCompute[ctx]);
                const ISPC::ModuleOUT *pOut = testCtx[ctx].pCamera->getModule<const ISPC::ModuleOUT>();
                
                for(f = 0 ; f < nPushed[ctx] ; f++)
                {
                    if (pOut->hdrInsertionType == PXL_NONE)
                    {
                        if (IMG_SUCCESS != testCtx[ctx].pCamera->enqueueShot())
                        {
                            LOG_ERROR("Failed to trigger frame for context %d\n",
                                ctx);
                            ret = EXIT_FAILURE;
                            goto parallel_exit;
                        }
                    }
                    else
                    {
                        int frame = testparams.uiNFrames[ctx] - nToCompute[ctx];
                        int hdrFrame = (frame - f) % testCtx[ctx].hdrInsMax;
                        
                        // if owning the camera we program shot with DG camera so that it triggers the capture
                        // if not owning the camera the DG should not be triggered!

                        if (testCtx[ctx].pCamera->ownsSensor())
                        {
                            ISPC::DGCamera *pCam = static_cast<ISPC::DGCamera*>(testCtx[ctx].pCamera);
                            // overloaded for DGCam to trigger shot
                            if (ProgramShotWithHDRIns(*(pCam),
                                testCtx[ctx].config->pszHDRInsertionFile[ctx],
                                testCtx[ctx].config->bRescaleHDRIns[ctx] ? true : false,
                                hdrFrame) != IMG_SUCCESS)
                            {
                                LOG_ERROR("failed to enque shot with HDR insertion %d/%d\n",
                                    frame, testparams.uiNFrames[ctx]);
                                ret = EXIT_FAILURE;
                                goto parallel_exit;
                            }
                        }
                        else
                        {
                            if (ProgramShotWithHDRIns(*(testCtx[ctx].pCamera),
                                testCtx[ctx].config->pszHDRInsertionFile[ctx],
                                testCtx[ctx].config->bRescaleHDRIns[ctx] ? true : false,
                                hdrFrame) != IMG_SUCCESS)
                            {
                                LOG_ERROR("failed to enque shot with HDR insertion %d/%d\n",
                                    frame, testparams.uiNFrames[ctx]);
                                ret = EXIT_FAILURE;
                                goto parallel_exit;
                            }
                        }
                    }
                }
            }
            else
            {
                nPushed[ctx] = 0;
            }
        } // for frame trigger

        // now acquire pushed frames
        for (ctx=0 ; ctx < CI_N_CONTEXT ; ctx++)
        {
            for (f=0 ; f < nPushed[ctx] ; f++)
            {
                ISPC::Shot captureBuffer;

                if(IMG_SUCCESS != testCtx[ctx].pCamera->acquireShot(captureBuffer))
                {
                    LOG_ERROR("Failed to acquire a frame for context %d\n", ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                testCtx[ctx].handleShot(captureBuffer, *(testCtx[ctx].pCamera));

                if(IMG_SUCCESS != testCtx[ctx].pCamera->releaseShot(captureBuffer))
                {
                    LOG_ERROR("Failed to release a frame for context %d\n", ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                IMG_ASSERT(nToCompute[ctx]>0);
                nToCompute[ctx]--;
            }

            if(0==nToCompute[ctx])
            {
                LOG_INFO("stopping context %d\n", ctx);
                if(IMG_SUCCESS != testCtx[ctx].pCamera->stopCapture())
                {
                    LOG_ERROR("Failed to stop context %d\n", ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                IMG_ASSERT(continueRunning>0);
                continueRunning--;
            }
        }
    }

    
parallel_exit:
    for(ctx=0 ; ctx < CI_N_CONTEXT ; ctx++)
    {
        if (testCtx[ctx].pCamera)
        {
            delete testCtx[ctx].pCamera;
            testCtx[ctx].pCamera = 0;
        }
    }
    return ret;
}

int main(int argc, char *argv[])
{
    struct TEST_PARAM parameters;
    int ret;

    LOG_INFO("CHANGELIST %s - DATE %s\n", CI_CHANGELIST_STR, CI_DATE_STR);
    LOG_INFO("Compiled for %d contexts, %d internal datagen\n",
        CI_N_CONTEXT, CI_N_IIF_DATAGEN);
#ifdef EXT_DATAGEN
    LOG_INFO("Compiled for %d ext datagen\n", CI_N_EXT_DATAGEN);
#else
    LOG_INFO("No support for ext datagen\n");
#endif

    //Load command line parameters
    if(parameters.load_parameters(argc, argv) > 0)
    {
        return EXIT_FAILURE;
    }

    if ( parameters.bDumpConfigFiles )
    {
        return dumpConfigFiles();
    }

#ifdef FELIX_FAKE
    if ( parameters.pszSimIP )
    {
        pszTCPSimIp = parameters.pszSimIP;
    }
    if ( parameters.uiSimPort != 0 )
    {
        ui32TCPPort = parameters.uiSimPort;
    }
    bDisablePdumpRDW = !parameters.bEnableRDW;
    //FAKE driver insmod when running against the sim
    initializeFakeDriver(parameters.uiUseMMU, parameters.uiGammaCurve, parameters.uiCPUPageSize<<10, parameters.uiTilingScheme, parameters.pszTransifLib, parameters.pszTransifSimParam);
#endif // FELIX_FAKE

#ifndef EXT_DATAGEN
    {
        // check that HW supports internal DG otherwise we simply cannot running
        ISPC::CI_Connection conn;
        const CI_CONNECTION *pConn = conn.getConnection();
        
        if (0 == (pConn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_IIF_DATAGEN))
        {
            LOG_ERROR("compiled without external data generator and HW %d.%d does not support internal data generator. Cannot run.\n",
                (int)pConn->sHWInfo.rev_ui8Major, (int)pConn->sHWInfo.rev_ui8Minor);
            return EXIT_FAILURE;
        }
    }
#endif

    time_t start = time(NULL);

    
    if (parameters.bParallel == IMG_FALSE)
    {
        LOG_INFO("Running context one after the other\n");
        ret = runSingle(parameters);
    }
    else
    {
        LOG_INFO("Running context in parallel\n");
        ret = runParallel(parameters);
    }

    start = time(NULL)-start;
    LOG_INFO("Test ran in %ld seconds\n", start);

#ifdef FELIX_FAKE
    //FAKE driver rmmod
    finaliseFakeDriver();
#endif

    if ( ret == EXIT_SUCCESS )
    {
        LOG_INFO("finished with success\n");
    }
    else
    {
        LOG_INFO("finished with errors\n");
    }

    if ( parameters.waitAtFinish )
    {
        LOG_INFO("Press Enter to terminate\n");
        getchar();
    }

    return ret;
}
