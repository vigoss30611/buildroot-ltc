/**
*******************************************************************************
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
#include <ispc/ControlAWB_PID.h>
#include <ispc/ControlAWB_Planckian.h>
#include <ispc/ControlLBC.h>
#include <ispc/ControlTNM.h>
#include <ispc/ControlDNS.h> // not really used
#include <ispc/ControlLSH.h>

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
#include <ispctest/info_helper.h>
#include <felix_hw_info.h>
#include <vector>
#include <hw_struct/save.h>

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
    std::vector<IMG_UINT32> lshMatrixIds;
    const char *lshGridFile; // lsh grid

    ISPC::Save *dispOutput;
    ISPC::Save *encOutput;
    ISPC::Save *hdrExtOutput;
    ISPC::Save *raw2DOutput;

    ISPC::Save *statsOutput;
    bool bStatsTxt;
    bool bIgnoreDPF;
    unsigned int hdrInsMax; // set a configure output
    IMG_UINT32 uiNFrames; // number of frames to run for - see TEST_PARAM::populate()

    TestContext();

    /**
     * @brief Configures output files from Pipeline's configuration
     *
     * @note May disable some of the output formats if the HW does not support it!
     */
    int configureOutput();

    int allocateBuffers();

    int loadLSH(const ISPC::ParameterList &parameters);
    /**
     * @brief change the matrix in use based on the frame number if
     * @ref TEST_PARAM::bChangeLSHPerFrame is ON and there is more than 1 LSH
     * matrix loaded
     */
    int changeLSH(int frame);

    void saveRaw(const ISPC::Shot &capturedShot);
    void handleShot(const ISPC::Shot &capturedShot, const ISPC::Camera &camera); // write available images and info to disk if enabled
    int configureAlgorithms(const ISPC::ParameterList &parameters, bool isOnwerOfCamera=true); // handles the update of the configuration using the registered control algorithms

    void close();

    void printInfo() const;

    ~TestContext();
};

struct TEST_PARAM //struct with parameters for test
{
    typedef enum {
        WB_NONE = ISPC::ControlAWB::WB_NONE,
        WB_AC = ISPC::ControlAWB::WB_AC,  /**< @brief Average colour */
        WB_WP = ISPC::ControlAWB::WB_WP,  /**< @brief White Patch */
        WB_HLW = ISPC::ControlAWB::WB_HLW,  /**< @brief High Luminance White */
        WB_COMBINED = ISPC::ControlAWB::WB_COMBINED, /**< @brief Combined AC + WP + HLW */
        WB_PLANCKIAN /** <@brief Planckian locus method, available on HW 2.6 */
    } awbControlAlgorithm_t;

    bool waitAtFinish; // getchar() at the end to help when running in new console on windows
    IMG_BOOL8 bParallel; // to turn on 2 contexts on single DG (IMG_BOOL8 because given to DYNCMD)
    bool bNoWrite; // do not write output
    IMG_BOOL8 bIgnoreDPF; // to ignore DPF output and override stats (IMG_BOOL8 because given to DYNCMD)
    bool bDumpConfigFiles;
    IMG_UINT aEncOutSelect[2];
    bool bPrintHWInfo;

    // data generator information stored by gasket
    IMG_CHAR *pszInputFile[CI_N_IMAGERS];   // input file name for the data generator
    IMG_INT aNBInputFrames[CI_N_IMAGERS]; // nb of frames to load from FLX file
    IMG_UINT aBlanking[CI_N_IMAGERS][2]; // blanking period for the DG of that gasket
    IMG_BOOL8 bUseMIPILF[CI_N_IMAGERS]; // if external dg can use MIPI with Line Format or not
    IMG_BOOL8 bPreload[CI_N_IMAGERS]; // enable line preloading
    IMG_UINT uiMipiLanes[CI_N_IMAGERS]; // if external dg number of MIPI lanes to use
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

    IMG_INT iNFrames[CI_N_CONTEXT]; // nb of frames to run for - if < 0 get the number from the imager
    IMG_CHAR *pszLSHGrid[CI_N_CONTEXT];
    IMG_BOOL8 bChangeLSHPerFrame[CI_N_CONTEXT];  // if multiple LSH are loaded will try to change the LSH matrix per frame
    IMG_BOOL8 bUseCtrlLSH[CI_N_CONTEXT];  // Instead of direct access to LSH Module use Control LSH
    IMG_BOOL8 bSaveStatistics[CI_N_CONTEXT];
    IMG_BOOL8 bSaveTxtStatistics[CI_N_CONTEXT];
    IMG_BOOL8 bReUse[CI_N_CONTEXT]; // re-use previous context object
    IMG_BOOL8 bWriteRaw[CI_N_CONTEXT]; // write raw images from HW as well as FLX for the Display, DE, HDR and Raw2D points (YUV is already raw)
    IMG_BOOL8 bSaveLSHMatrices[CI_N_CONTEXT]; // save the LSH matrices in the running folder
    IMG_BOOL8 bRescaleHDRIns[CI_N_CONTEXT];
    IMG_BOOL8 bUpdateMaxSize[CI_N_CONTEXT]; // update the maximum context size for the output

    //IMG_BOOL8 autoExposure[CI_N_CONTEXT];            /** @brief Enable/Disable Auto exposure */
    //IMG_FLOAT targetBrightness[CI_N_CONTEXT];        /** @brief target value for the ControlAE brightness */
    //IMG_BOOL8 flickerRejection[CI_N_CONTEXT];        /** @brief Enable/Disable Flicker Rejection */

    IMG_BOOL8 autoToneMapping[CI_N_CONTEXT];        /** @brief Enable/Disable global tone mapping curve automatic calculation */
    IMG_BOOL8 localToneMapping[CI_N_CONTEXT];        /** @brief Enable/Disable local tone mapping */
    IMG_BOOL8 adaptiveToneMapping[CI_N_CONTEXT];        /** @brief Enable/Disable tone mapping auto strength control */

    IMG_BOOL8 autoDenoiserISO[CI_N_CONTEXT];        /** @brief Enable/Disable denoiser ISO control */

    IMG_BOOL8 autoLBC[CI_N_CONTEXT];                /** @brief Enable/Disable Light Based Control */

    IMG_UINT autoWhiteBalanceAlgorithm[CI_N_CONTEXT];

    bool WBTSEnabled;
    IMG_FLOAT WBTSWeightsBase;
    IMG_UINT WBTSTemporalStretch;
    IMG_UINT WBTSFeatures;
    IMG_BOOL WBTSEnabledOverride;
    IMG_BOOL WBTSWeightsBaseOverride;
    IMG_BOOL WBTSTemporalStretchOverride;
    IMG_BOOL WBTSFeaturesOverride;

    // fake interface only
    IMG_UINT uiRegisterOverride[CI_N_CONTEXT]; // tries to load
    IMG_CHAR *pszTransifLib; // path to simulator dll - only valid when using fake interface
    IMG_CHAR *pszTransifSimParam; // parameter file to give to simulator dll - only valid when using fake interface
    IMG_CHAR *pszSimIP; // simulator IP address when using TCP/IP interface - NULL means localhost
    IMG_INT uiSimPort; // simulator port when using TCP/IP interface - 0 means default
    IMG_BOOL8 bEnableRDW; // when generating pdump should RDW be enabled
    IMG_UINT uiUseMMU; // 0 bypass, 1 32b, 2 extended address range - only valid when using fake interface
    IMG_UINT uiTilingScheme; // 256 or 512 according to tiling scheme to use
    IMG_UINT uiTilingStride; // if non-0 used for tiling allocations
    IMG_UINT uiGammaCurve; // gamma curve to use - see KRN_CI_DriverDefaultsGammaLUT() for legitimate values
    IMG_UINT aEDGPLL[2]; // external data generator PLL
    IMG_UINT uiCPUPageSize; // CPU page size in kB

    TEST_PARAM();

    ~TEST_PARAM();

    void printTestParameters() const;

    int load_parameters(int argc, char *argv[]); // load options from command line, returns number of vital parameters that were missing

    int populate(TestContext &config, int ctx) const;

};

TEST_PARAM::TEST_PARAM()
{
    IMG_MEMSET(this, 0, sizeof(TEST_PARAM));
    // defaults that are not 0
    uiUseMMU = 2; // use the mmu by default
    uiTilingScheme = 256;
    // uiTilingStride = 0;
#ifdef FELIX_FAKE // otherwise uiGammaCurve is not used and CI_DEF_GMACURVE is not defined
    uiGammaCurve = CI_DEF_GMACURVE;
    aEDGPLL[0] = 0;
    aEDGPLL[1] = 0;
#endif
    bEnableRDW = IMG_FALSE;
    uiCPUPageSize = 4; // 4kB by default
    aEncOutSelect[0] = CI_N_CONTEXT;
    aEncOutSelect[1] = 1;
    bPrintHWInfo = false;
    for (int ctx = 0; ctx < CI_N_CONTEXT; ctx++)
    {
#ifndef FELIX_FAKE
        aEnableTimestamps[ctx] = IMG_TRUE;
#endif // when running FAKE driver timestamps are disabled by default

        uiNBuffers[ctx] = 1;
        uiNEncTiled[ctx] = 0;
        uiNDispTiled[ctx] = 0;
        uiNHDRTiled[ctx] = 0;

        iNFrames[ctx] = -1;
        uiImager[ctx] = ctx;
        bSaveStatistics[ctx] = IMG_TRUE;
        bSaveTxtStatistics[ctx] = IMG_TRUE;
        bSaveLSHMatrices[ctx] = IMG_TRUE;
        bRescaleHDRIns[ctx] = IMG_TRUE;
        //bUpdateMaxSize[ctx] = IMG_FALSE;
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

    WBTSEnabled = false;
    WBTSWeightsBase = (IMG_FLOAT)ISPC::ControlAWB_Planckian::DEFAULT_WEIGHT_BASE;
    WBTSTemporalStretch = (IMG_UINT)ISPC::ControlAWB_Planckian::DEFAULT_TEMPORAL_STRETCH;
    WBTSFeatures = (IMG_UINT)0;

    for (int g = 0; g < CI_N_IMAGERS; g++)
    {
        pszInputFile[g] = NULL;
        aNBInputFrames[g] = -1;
        aBlanking[g][0] = FELIX_MIN_H_BLANKING;
        aBlanking[g][1] = FELIX_MIN_V_BLANKING;
        bUseMIPILF[g] = IMG_FALSE;
        uiMipiLanes[g] = 1;
        bPreload[g] = IMG_FALSE;
        uiMipiLanes[g] = 1;
        aIsIntDG[g] = false;
        aIndexDG[g] = g;
    }
}

TEST_PARAM::~TEST_PARAM()
{
    int i;

    for (i = 0; i < CI_N_IMAGERS; i++)
    {
        if (pszInputFile[i])
        {
            IMG_FREE(pszInputFile[i]);
            pszInputFile[i] = NULL;
        }
    }
    for (i = 0; i < CI_N_CONTEXT; i++)
    {
        if (pszFelixSetupArgsFile[i])
        {
            IMG_FREE(pszFelixSetupArgsFile[i]);
            pszFelixSetupArgsFile[i] = NULL;
        }
        if (pszHDRInsertionFile[i])
        {
            IMG_FREE(pszHDRInsertionFile[i]);
            pszHDRInsertionFile[i] = NULL;
        }
        if (pszLSHGrid[i])
        {
            IMG_FREE(pszLSHGrid[i]);
            pszLSHGrid[i] = NULL;
        }
    }
    if (pszTransifLib)
    {
        IMG_FREE(pszTransifLib);
        pszTransifLib = NULL;
    }
    if (pszTransifSimParam)
    {
        IMG_FREE(pszTransifSimParam);
        pszTransifSimParam = NULL;
    }
    if (pszSimIP)
    {
        IMG_FREE(pszSimIP);
        pszSimIP = NULL;
    }
}

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
            if (iNFrames[i] > 0)
            {
                printf("  Number of frames: %d\n", iNFrames[i]);
            }
            else
            {
                printf("  Number of frames: all input frames\n");
            }
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
                printf("  Ext. Datagen %d", g);
                if (bUseMIPILF)
                {
                    printf(" (MIPI LF if possible)");
                }
                printf("\n");
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
    bool printHelp = false;
    // see TEST_PARAM() for defaults

    IMG_CHAR *int_pszInputFile[CI_N_IIF_DATAGEN];
    IMG_INT int_aNBInputFrames[CI_N_IIF_DATAGEN];
    IMG_UINT int_aBlanking[CI_N_IIF_DATAGEN][2];
    IMG_BOOL8 int_bPreload[CI_N_IIF_DATAGEN];
    IMG_UINT int_aGasket[CI_N_IIF_DATAGEN];

    for (ctx = 0; ctx < CI_N_IIF_DATAGEN; ctx++)
    {
        int_pszInputFile[ctx] = this->pszInputFile[ctx];
        int_aNBInputFrames[ctx] = this->aNBInputFrames[ctx];
        int_aBlanking[ctx][0] = this->aBlanking[ctx][0];
        int_aBlanking[ctx][1] = this->aBlanking[ctx][1];
        int_bPreload[ctx] = IMG_FALSE;
        int_aGasket[ctx] = CI_N_IMAGERS; // disabled
    }

#ifdef EXT_DATAGEN
    // ext
    IMG_CHAR *ext_pszInputFile[CI_N_EXT_DATAGEN];
    IMG_INT ext_aNBInputFrames[CI_N_EXT_DATAGEN];
    IMG_UINT ext_aBlanking[CI_N_EXT_DATAGEN][2];
    IMG_BOOL8 ext_bUseMIPILF[CI_N_EXT_DATAGEN];
    IMG_BOOL8 ext_bPreload[CI_N_EXT_DATAGEN];
    IMG_UINT ext_mipiLanes[CI_N_EXT_DATAGEN];
    IMG_UINT ext_aGasket[CI_N_EXT_DATAGEN];

    for (ctx = 0; ctx < CI_N_EXT_DATAGEN; ctx++)
    {
        ext_pszInputFile[ctx] = this->pszInputFile[ctx];
        ext_aNBInputFrames[ctx] = this->aNBInputFrames[ctx];
        ext_aBlanking[ctx][0] = this->aBlanking[ctx][0];
        ext_aBlanking[ctx][1] = this->aBlanking[ctx][1];
        ext_bUseMIPILF[ctx] = IMG_FALSE;
        ext_bPreload[ctx] = IMG_FALSE;
        ext_mipiLanes[ctx] = 1;
        ext_aGasket[ctx] = ctx;
    }
#endif

    //command line parameter loading:
    ret = DYNCMD_AddCommandLine(argc, argv, "-source");
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("unable to parse command line\n");
        DYNCMD_ReleaseParameters();
        return EXIT_FAILURE;
    }

    ret = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND,
        "Usage information", NULL);
    if (RET_FOUND == ret)
    {
        // will print help at the end, errors are not printed
        printHelp = true;
    }

    // internal dg options
    for (ctx = 0; ctx < CI_N_IIF_DATAGEN; ctx++)
    {
        snprintf(paramName, sizeof(paramName), "-IntDG%d_inputFLX", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING,
            "FLX input file to use for Data Generator",
            &(int_pszInputFile[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-IntDG%d_inputFrames", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_INT,
            "[optional] Number of frames to load from the FLX input file "\
            "(<=0 means all)",
            &(int_aNBInputFrames[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-IntDG%d_blanking", ctx);
        ret = DYNCMD_RegisterArray(paramName, DYNCMDTYPE_UINT,
            "Horizontal and vertical blanking in pixels",
            &(int_aBlanking[ctx][0]), 2);
        if (RET_FOUND != ret && !printHelp)
        {
            if (ret == RET_INCOMPLETE)
            {
                LOG_WARNING("%s incomplete, second value for blanking is "\
                    "assumed to be %u\n", paramName, int_aBlanking[ctx][1]);
            }
            else if (ret != RET_NOT_FOUND)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-IntDG%d_preload", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable the line preload functionality of the data-generator",
            &(int_bPreload[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-IntDG%d_gasket", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Gasket to replace data from",
            &(int_aGasket[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
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
    for (ctx = 0; ctx < CI_N_EXT_DATAGEN; ctx++)
    {
        snprintf(paramName, sizeof(paramName), "-DG%d_inputFLX", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING,
            "FLX input file to use for Data Generator",
            &(ext_pszInputFile[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-DG%d_inputFrames", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_INT,
            "[optional] Number of frames to load from the FLX input file "\
            "(<=0 means all)",
            &(ext_aNBInputFrames[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-DG%d_blanking", ctx);
        ret = DYNCMD_RegisterArray(paramName, DYNCMDTYPE_UINT,
            "Horizontal and vertical blanking in pixels",
            &(ext_aBlanking[ctx]), 2);
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_INCOMPLETE == ret)
            {
                LOG_WARNING("Warning: %s incomplete, second value for "\
                    "blanking is assumed to be %u\n",
                    paramName, ext_aBlanking[ctx][1]);
            }
            else if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-DG%d_MIPI_LF", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "If using a MIPI gasket uses the MIPI Line Flag format",
            &(ext_bUseMIPILF[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-DG%d_preload", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable the line preload of the data generator",
            &(ext_bPreload[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-DG%d_mipiLanes", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Number of mipi lanes to use (1, 2 or 4) - no effect if using a "\
            "parallel gasket",
            &(ext_mipiLanes[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
    }
#endif

    // then sort dg per gasket
    for (ctx = 0; ctx < CI_N_IMAGERS; ctx++)
    {
        int iIntDg = -1; // not internal dg
        int i;
        for (i = 0; i < CI_N_IIF_DATAGEN; i++)
        {
            if (int_aGasket[i] == ctx)
            {
                iIntDg = i;
            }
        }

        if (iIntDg >= 0 && iIntDg < CI_N_IIF_DATAGEN)
        {
            this->aIsIntDG[ctx] = true;
            this->pszInputFile[ctx] = int_pszInputFile[iIntDg];
            this->aNBInputFrames[ctx] = int_aNBInputFrames[iIntDg];
            this->aBlanking[ctx][0] = int_aBlanking[iIntDg][0];
            this->aBlanking[ctx][1] = int_aBlanking[iIntDg][1];
            this->bUseMIPILF[ctx] = IMG_FALSE;  // for ext dg only
            this->bPreload[ctx] = int_bPreload[iIntDg];
            this->uiMipiLanes[ctx] = 1;  // for ext dg only
            this->aIndexDG[ctx] = iIntDg;
        }
#ifdef EXT_DATAGEN
        else
        {
            if (ctx < CI_N_EXT_DATAGEN)
            {
                this->aIsIntDG[ctx] = false;
                this->pszInputFile[ctx] = ext_pszInputFile[ctx];
                this->aNBInputFrames[ctx] = ext_aNBInputFrames[ctx];
                this->aBlanking[ctx][0] = ext_aBlanking[ctx][0];
                this->aBlanking[ctx][1] = ext_aBlanking[ctx][1];
                this->bUseMIPILF[ctx] = ext_bUseMIPILF[ctx];
                this->bPreload[ctx] = ext_bPreload[ctx];
                this->uiMipiLanes[ctx] = ext_mipiLanes[ctx];
                this->aIndexDG[ctx] = ctx;
            }
            // otherwise leave to defaults
        }
#endif
    }
    // wbts options
    snprintf(paramName, sizeof(paramName), "-useWBTS");
    ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
        "Enable usage of White Balance Temporal Smoothing algorithm - (1=enabled, 0=disabled)",
        &(this->WBTSEnabled));
    if (RET_FOUND != ret)
    {
        this->WBTSEnabledOverride = false;
        if (RET_NOT_FOUND != ret && !printHelp) // if not found use default
        {
                LOG_ERROR("while parsing parameter -useWBTS\n");
            missing++;
        }
    }
    else
    {
        this->WBTSEnabledOverride = true;
    }

    IMG_UINT algo = 0;
    snprintf(paramName, sizeof(paramName), "-wbtsTemporalStretch");
    ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_INT,
        "[optional] time to settle awb after change (milliseconds) 200 < param <= 5000, default 300",
        &this->WBTSTemporalStretch);
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
            this->WBTSTemporalStretch =
                    ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MIN;
        }
        if(ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MAX < this->WBTSTemporalStretch)
        {
            LOG_WARNING("-wbtsTemporalStretch above max. Clipping\n");
            this->WBTSTemporalStretch =
                    ISPC::ControlAWB_Planckian::TEMPORAL_STRETCH_MAX;
        }
    }

    snprintf(paramName, sizeof(paramName), "-wbtsWeightsBase");
    ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_FLOAT,
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
            LOG_WARNING("-wbtsWeightsBase parameter < %f. Clipping\n",
                    ISPC::ControlAWB_Planckian::getMinWeightBase());
                this->WBTSWeightsBase =
                        ISPC::ControlAWB_Planckian::getMinWeightBase();
        }
        if(ISPC::ControlAWB_Planckian::getMaxWeightBase() < this->WBTSWeightsBase)
        {
            LOG_WARNING("-wbtsWeightsBase parameter > %f. Clipping\n",
                    ISPC::ControlAWB_Planckian::getMaxWeightBase());
                this->WBTSWeightsBase =
                        ISPC::ControlAWB_Planckian::getMaxWeightBase();
        }
    }

    algo = 0;
    snprintf(paramName, sizeof(paramName), "-featuresWBTS");
    ret=DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
        "Enable usage of WBTS features - values are mapped ISPC::Smoothing_Features (0=None, 1=FlashFiltering)",
        &(algo));
    if (RET_FOUND != ret)
    {
        LOG_INFO("WBTS disabled\n");
        this->WBTSFeaturesOverride = false;
        if (RET_NOT_FOUND != ret && !printHelp) // if not found use default
        {
            LOG_ERROR("while parsing parameter -featuresWBTS\n");
            missing++;
        }
    }
    else
    {
        LOG_INFO("WBTS enabled\n");
        this->WBTSFeaturesOverride = true;
        this->WBTSFeatures = ISPC::ControlAWB_Planckian::WBTS_FEATURES_NONE;
        if ( algo & ISPC::ControlAWB_Planckian::WBTS_FEATURES_FF)
            this->WBTSFeatures = ISPC::ControlAWB_Planckian::WBTS_FEATURES_FF;
    }

    //
    // CTX options
    //
    for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
    {
        snprintf(paramName, sizeof(paramName), "-CT%d_setupFile", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING,
            "FelixSetupArgs file to use to configure the Pipeline",
            &(this->pszFelixSetupArgsFile[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_imager", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Imager to use as input for that context",
            &(this->uiImager[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_nBuffers", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Number of pending buffers for that context (submitted at once "\
            "- maxed to the size of the pending list in HW)",
            &(this->uiNBuffers[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_HDRInsertion", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING,
            "The filename to use as the input of the HDR Insertion if "\
            "enabled in given setup file - "\
            "if none do not use HDR Insertion, image has to be an FLX "\
            "in RGB of the same size than the one provided to DG but does "\
            "not have to be of the correct bitdepth",
            &(this->pszHDRInsertionFile[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_HDRInsRescale", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Rescale provided HDR Insertion file to 16b per pixels",
            &(this->bRescaleHDRIns[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_updateMaxSize", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Update output size using setEncoderDimensions() and "\
            "setDisplayDimensions() using output size after the scalers",
            &(this->bUpdateMaxSize[ctx]));
        if (ret != RET_FOUND && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_nEncTiled", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Number of encoder buffers to have tiled (maxed to "\
            "corresponding -CTX_nBuffers - if same value than "\
            "-CTX_nBuffers all outputs are tiled)",
            &(this->uiNEncTiled[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_nDispTiled", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Number of display buffers to have tiled (maxed to "\
            "corresponding -CTX_nBuffers - if same value than "\
            "-CTX_nBuffers all outputs are tiled)",
            &(this->uiNDispTiled[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_nHDRExtTiled", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Number of HDR Extraction buffers to have tiled (maxed to "\
            "corresponding -CTX_nBuffers - if same value than "\
            "-CTX_nBuffers all outputs are tiled)",
            &(this->uiNHDRTiled[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_nFrames", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_INT,
            "Number of frames to run for that context. If negative use the "\
            "number of frames from the input file of the associated input.",
            &(this->iNFrames[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_LSHGrid", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_STRING,
            "LSH grid file to load instead of the one in -CTX_setupFile",
            &(this->pszLSHGrid[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_changeLSHPerFrame", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "if more than 1 LSH grid is loaded will try to change the grid "\
            "based on the frame number. This disables ControlLSH module.",
            &(this->bChangeLSHPerFrame[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_saveStats", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable saving of the raw statistics from the HW (contains "\
            "CRCs, timestamps etc) - disabled if -no_write is specified",
            &(this->bSaveStatistics[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_saveTxtStats", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable saving of the statistics from the HW as readbale text "\
            "(contains AWS, timestamps etc but ignores CRCs) - disabled "\
            "if -no_write is specified",
            &(this->bSaveTxtStatistics[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_saveRaw", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable saving of the raw image data for images output (except "\
            "YUV which is always RAW image from HW). Stride is removed.",
            &(this->bWriteRaw[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_saveLSH", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Save the LSH matrices loaded in the test folder. "\
            "If disabled still save the current matrix Id used "\
            "(but the file will not exist - find the matrix Id in the log)",
            &(this->bSaveLSHMatrices[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_TNM", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable usage Global Tone Mapper control algorithm",
            &(this->autoToneMapping[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
        snprintf(paramName, sizeof(paramName), "-CT%d_TNM-local", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable usage Local Tone Mapper control algorithm",
            &(this->localToneMapping[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }
        snprintf(paramName, sizeof(paramName), "-CT%d_TNM-adapt", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable usage Adaptive Tone Mapper control algorithm",
            &(this->adaptiveToneMapping[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_LBC", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable usage Light Based Controls algorithm (and loading of "\
            "the LBC parameters)",
            &(this->autoLBC[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_WBC", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "Enable usage of White Balance Control algorithm and loading of "\
            "the multi-ccm parameters - 0 is disabled, other values are "\
            "mapped ISPC::Correction_Types (1=AC, 2=WP, 3=HLW, 4=COMBINED, "\
            "5=PLANKIAN)",
            &(this->autoWhiteBalanceAlgorithm[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_ControlLSH", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Use Control LSH module instead of direct access to LSH Module. "\
            "Disables -CTx_LSHGrid.",
            &(this->bUseCtrlLSH[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                LOG_WARNING("Error while parsing parameter %s\n", paramName);
            }
        }

        snprintf(paramName, sizeof(paramName), "-CT%d_enableTS", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
            "Enable generation of timestamps - no effect when running "\
            "against CSIM",
            &(this->aEnableTimestamps[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("%s expects a bool but none was found\n", paramName);
            }
        }


        if (ctx > 0)
        {
            snprintf(paramName, sizeof(paramName), "-CT%d_re-use", ctx);
            ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_BOOL8,
                "Enable the re-use of the previously used Camera object "\
                "when running in serial mode (the previously used context "\
                "needs to be configured to use the same DG as the new one!)",
                &(this->bReUse[ctx]));
            if (RET_FOUND != ret && !printHelp)
            {
                if (RET_NOT_FOUND != ret)
                {
                    missing++;
                    LOG_ERROR("Error while parsing parameter %s\n", paramName);
                }
            }
        }

#if defined(FELIX_FAKE) && defined(CI_DEBUG_FCT)

        snprintf(paramName, sizeof(paramName), "-CT%d_regOverride", ctx);
        ret = DYNCMD_RegisterParameter(paramName, DYNCMDTYPE_UINT,
            "[optional] enable register override (0 is disabled, >0 enabled)",
            &(this->uiRegisterOverride[ctx]));
        if (RET_FOUND != ret && !printHelp)
        {
            if (RET_NOT_FOUND != ret)
            {
                missing++;
                LOG_ERROR("Error while parsing parameter %s\n", paramName);
            }
        }

#endif // FELIX_FAKE && CI_DEBUG_FCT
    } // for each context

#ifdef FELIX_FAKE

    ret = DYNCMD_RegisterParameter("-simTransif", DYNCMDTYPE_STRING,
        "Transif library given by the simulator. Also needs -simConfig to "\
        "be used.",
        &(this->pszTransifLib));
    if (RET_NOT_FOUND == ret)
    {
        LOG_INFO("TCP/IP sim connection\n");
    }
    else
    {
        LOG_INFO("Transif sim connection (%s)\n", this->pszTransifLib);
    }

    // only if transif
    ret = DYNCMD_RegisterParameter("-simConfig", DYNCMDTYPE_STRING,
        "Configuration file to give to the simulator when using transif "\
        "(-argfile file parameter of the sim)",
        &(this->pszTransifSimParam));
    if (RET_NOT_FOUND == ret && !printHelp)
    {
        if (this->pszTransifLib)
        {
            missing++;
            LOG_ERROR("-simConfig parameter file not given\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-simIP", DYNCMDTYPE_STRING,
        "Simulator IP address to use when using TCP/IP interface.",
        &(this->pszSimIP));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("-simIP expects a string but none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-simPort", DYNCMDTYPE_INT,
        "Simulator port to use when running TCP/IP interface.",
        &(this->uiSimPort));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("-simPort expects an int but none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-pdumpEnableRDW", DYNCMDTYPE_BOOL8,
        "Enable generation of RDW commands in pdump output (not useful "\
        "for verification)",
        &(this->bEnableRDW));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("-pdumpEnableRDW expects a bool but none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-mmucontrol", DYNCMDTYPE_INT,
        "Flag to indicate how to use the MMU - 0 = bypass, 1 = 32b MMU, "\
        ">1 = extended addressing",
        &(this->uiUseMMU));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("expecting an int for -mmucontrol option\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-tilingScheme", DYNCMDTYPE_INT,
        "Choose tiling scheme (256 = 256Bx16, 512 = 512Bx8)",
        &(this->uiTilingScheme));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            CI_FATAL("-tilingScheme expects an integer and none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-tilingStride", DYNCMDTYPE_INT,
        "If non-0 used for tiling allocations (has to be a power of 2 "\
        "big enough for all tiled allocations)",
        &(this->uiTilingStride));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            CI_FATAL("-tilingStride expects an integer and none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-gammaCurve", DYNCMDTYPE_UINT,
        "Gamma Look Up Curve to use - 0 = BT709, 1 = sRGB (other is invalid)",
        &(this->uiGammaCurve));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("expecting an uint for -gammaCurve option\n");
        }
    }

#ifdef EXT_DATAGEN
    ret = DYNCMD_RegisterArray("-extDGPLL", DYNCMDTYPE_UINT,
        "External Data-generator PLL values (multiplier and divider). "\
        "HAS TO RESPECT HW LIMITATIONS or FPGA may need to be reprogrammed.",
        this->aEDGPLL, 2);
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("expecting 2 uint for -extDGPLL option\n");
            return EXIT_FAILURE;
        }
    }
#endif

    ret = DYNCMD_RegisterParameter("-cpuPageSize", DYNCMDTYPE_UINT,
        "CPU page size in kB (e.g. 4, 16)",
        &(this->uiCPUPageSize));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("expecting an uint for -gammaCurve option\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-no_pdump", DYNCMDTYPE_COMMAND,
        "Do not write pdumps", NULL);
    if (RET_FOUND == ret)
    {
        // global variable from ci_debug.h
        bEnablePdump = IMG_FALSE;
    }

#endif // FELIX_FAKE

    ret = DYNCMD_RegisterParameter("-parallel", DYNCMDTYPE_BOOL8,
        "run multi-context all at once (1) or in serial (0)",
        &(this->bParallel));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("-parallel expects a bool but none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-ignoreDPF", DYNCMDTYPE_BOOL8,
        "Override DPF results in statistics and skip saving of the DPF "\
        "output file (used for verification as CSIM and HW do not match on"\
        "DPF output)", &(this->bIgnoreDPF));
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            missing++;
            LOG_ERROR("-ignoreDPF expects a bool but none was found\n");
        }
    }

    ret = DYNCMD_RegisterParameter("-no_write", DYNCMDTYPE_COMMAND,
        "Do not write output to file (including statistics)", NULL);
    if (RET_FOUND == ret)
    {
        this->bNoWrite = true;
    }

    ret = DYNCMD_RegisterParameter("-wait", DYNCMDTYPE_COMMAND,
        "Wait at the end to allow reading of the console output on windows "\
        "when starting as batch",
        NULL);
    if (RET_FOUND == ret)
    {
        this->waitAtFinish = true;
    }

    ret = DYNCMD_RegisterParameter("-DumpConfigFiles", DYNCMDTYPE_COMMAND,
        "Output configuration files for default, minimum and maximum values "\
        "for all parameters",
        NULL);
    if (RET_FOUND == ret)
    {
        this->bDumpConfigFiles = true;
    }

    ret = DYNCMD_RegisterArray("-encOutCtx", DYNCMDTYPE_UINT,
        "Which context to use for the Encoder pipeline statistics output: "\
        "1st value is context, second value is the pulse"\
        "(see FELIX_CORE:ENC_OUT_CTRL:ENC_OUT_CTRL_SEL)",
        this->aEncOutSelect, 2);
    if (RET_FOUND != ret && !printHelp)
    {
        if (RET_NOT_FOUND != ret)
        {
            LOG_ERROR("-encOutCtx expects 2 int but none were found\n");
            return EXIT_FAILURE;
        }
    }

    //bPrintHWInfo
    ret = DYNCMD_RegisterParameter("-hw_info", DYNCMDTYPE_COMMAND,
        "Prints the HW information from the connection object",
        NULL);
    if (RET_FOUND == ret)
    {
        this->bPrintHWInfo = true;
    }

    /*
     * must be last action
     */
    if (printHelp || missing > 0)
    {
        DYNCMD_PrintUsage();
        return EXIT_FAILURE;
    }

    DYNCMD_HasUnregisteredElements(&unregistered);

    this->printTestParameters();
    /* print if parameters were not used and clean parameter internal
     * structure - we don't need it anymore */
    DYNCMD_ReleaseParameters();

    return missing > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

int TEST_PARAM::populate(TestContext &config, int ctx) const
{
    config.context = ctx;
    config.config = this;
    config.bIgnoreDPF = this->bIgnoreDPF ? true : false;
    config.bStatsTxt = this->bSaveTxtStatistics[ctx] ? true : false;

    config.lshGridFile = this->pszLSHGrid[ctx];

    if (this->iNFrames[ctx] < 0)
    {
        if (config.pCamera == NULL)
        {
            return EXIT_FAILURE;
        }
        IMG_RESULT ret;
        ISPC::DGSensor *sensor =
            static_cast<ISPC::DGSensor*>(config.pCamera->getSensor());

        if (sensor->isInternal)
        {
            ret = IIFDG_ExtendedGetFrameCap(sensor->getHandle(), &config.uiNFrames);
        }
#ifdef EXT_DATAGEN
        else
        {
            ret = DGCam_ExtendedGetFrameCap(sensor->getHandle(), &config.uiNFrames);
        }
#else
        else
        {
            ret = IMG_ERROR_FATAL;
        }
#endif

        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("could not get number of frames from the data-generator\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        config.uiNFrames = this->iNFrames[ctx];
    }

    return EXIT_SUCCESS;
}

TestContext::TestContext()
{
    config = NULL;
    context = 0;

    pCamera = NULL;
    lshGridFile = NULL;

    dispOutput = NULL;
    encOutput = NULL;
    hdrExtOutput = NULL;
    raw2DOutput = NULL;

    statsOutput = NULL;
    bStatsTxt = true;
    bIgnoreDPF = false;
    hdrInsMax = 0;
    uiNFrames = 0;
}

void TestContext::close()
{
    if (dispOutput)
    {
        dispOutput->close();
        delete dispOutput;
        dispOutput = NULL;
    }
    if (encOutput)
    {
        encOutput->close();
        delete encOutput;
        encOutput = NULL;
    }
    if (hdrExtOutput)
    {
        hdrExtOutput->close();
        delete hdrExtOutput;
        hdrExtOutput = NULL;
    }
    if (raw2DOutput)
    {
        raw2DOutput->close();
        delete raw2DOutput;
        raw2DOutput = NULL;
    }
    if (statsOutput)
    {
        statsOutput->close();
        delete statsOutput;
        statsOutput = NULL;
    }
}

void TestContext::printInfo() const
{
    if (pCamera)
    {
        std::cout << "*****" << std::endl;
        PrintGasketInfo(*pCamera, std::cout);
        PrintRTMInfo(*pCamera, std::cout);
        std::cout << "*****" << std::endl;
    }
}

TestContext::~TestContext()
{
    if (pCamera)
    {
        delete pCamera;
        pCamera = NULL;
    }

    close();
}

int TestContext::configureOutput()
{
    if (pCamera)
    {
        ISPC::Pipeline &pipeline = *(pCamera->getPipeline());
        ISPC::ModuleOUT *glb = pCamera->getModule<ISPC::ModuleOUT>();
        ISPC::Global_Setup setup = pipeline.getGlobalSetup();
        const CI_CONNECTION *conn = pCamera->getConnection();
        int ctx = pipeline.getCIPipeline()->config.ui8Context;
        IMG_RESULT ret;
        
#ifdef FELIX_FAKE
        if (config->aEnableTimestamps[ctx])
        {
            LOG_WARNING("Ignoring timestamp option when running "\
                "against CSIM\n");
        }
#else
        pCamera->getPipeline()->getMCPipeline()->bEnableTimestamp =
            config->aEnableTimestamps[ctx];
#endif
        LOG_INFO("Ctx %d timestamps are %s\n",
            ctx, pCamera->getPipeline()->getMCPipeline()->bEnableTimestamp ?
            "ON" : "OFF");

        if (!this->config->bNoWrite)
        {
            int nTiled = 0;  // increment for each output that needs tiling

            if (PXL_NONE != glb->displayType)
            {
                std::string file = ISPC::Save::getDisplayFilename(pipeline);
                dispOutput = new ISPC::Save();
                ret = dispOutput->open(ISPC::Save::Display, pipeline, file);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to open output file '%s'\n", file.c_str());
                    return EXIT_FAILURE;
                }
                if (config->uiNDispTiled[ctx] > 0)
                {
                    nTiled++;
                }
                if (config->bUpdateMaxSize[ctx])
                {
                    pCamera->setDisplayDimensions(setup.ui32DispWidth,
                        setup.ui32DispHeight);
                }
            }
            else if (CI_INOUT_NONE != glb->dataExtractionPoint
                && PXL_NONE != glb->dataExtractionType)
            {
                char filename[64];
                snprintf(filename, sizeof(filename), "dataExtraction%d.flx",
                    ctx);
                dispOutput = new ISPC::Save();
                ret = dispOutput->open(ISPC::Save::Bayer, pipeline, filename);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to open output file '%s'\n", filename);
                    return EXIT_FAILURE;
                }
            }

            if (PXL_NONE != glb->encoderType)
            {
                std::string file = ISPC::Save::getEncoderFilename(pipeline);
                encOutput = new ISPC::Save();

                ret = encOutput->open(ISPC::Save::YUV, pipeline, file);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to open output file '%s'\n",
                        file.c_str());
                    return EXIT_FAILURE;
                }
                if (0 < config->uiNEncTiled[ctx])
                {
                    nTiled++;
                }
                if (config->bUpdateMaxSize[ctx])
                {
                    pCamera->setEncoderDimensions(setup.ui32EncWidth,
                        setup.ui32EncHeight);
                }
            }

            if (PXL_NONE != glb->hdrExtractionType)
            {
                if ((conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_EXT))
                {
                    char filename[64];
                    snprintf(filename, sizeof(filename),
                        "hdrExtraction%d.flx", ctx);
                    hdrExtOutput = new ISPC::Save();
                    ret = hdrExtOutput->open(ISPC::Save::RGB_EXT, pipeline,
                        filename);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to open output file '%s'\n",
                            filename);
                        return EXIT_FAILURE;
                    }
                    if (0 < config->uiNHDRTiled[ctx])
                    {
                        nTiled++;
                    }
                }
                else
                {
                    LOG_WARNING("current HW %d.%d does not support "\
                        "HDR Extraction - output forced to NONE\n",
                        (int)conn->sHWInfo.rev_ui8Major,
                        (int)conn->sHWInfo.rev_ui8Minor);
                    glb->hdrExtractionType = PXL_NONE;
                    glb->requestUpdate();
                }
            }

            if (PXL_NONE != glb->raw2DExtractionType)
            {
                if ((conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_RAW2D_EXT))
                {
                    char filename[64];
                    snprintf(filename, sizeof(filename),
                        "raw2DExtraction%d.flx", ctx);
                    raw2DOutput = new ISPC::Save();
                    ret = raw2DOutput->open(ISPC::Save::Bayer_TIFF, pipeline,
                        filename);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to open output file '%s'\n",
                            filename);
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    LOG_WARNING("current HW %d.%d does not support "\
                        "RAW2D Extraction - output forced to NONE\n",
                        (int)conn->sHWInfo.rev_ui8Major,
                        (int)conn->sHWInfo.rev_ui8Minor);
                    glb->raw2DExtractionType = PXL_NONE;
                    glb->requestUpdate();
                }
            }

            if (this->config->bSaveStatistics[this->context])
            {
                char filename[64];
                snprintf(filename, sizeof(filename), "ctx_%d_stats.dat", ctx);
                statsOutput = new ISPC::Save();
                ret = statsOutput->open(ISPC::Save::Bytes, pipeline, filename);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to open output file '%s'\n", filename);
                    return EXIT_FAILURE;
                }
            }

            // write DPF map handled per shot

            // if available in HW enable tiling if needed
            if (conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_TILING)
            {
                pipeline.setTilingEnabled(0 < nTiled);
            }
        }

        if (PXL_NONE != glb->hdrInsertionType)
        {
            if (0 ==
                (conn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_HDR_INS))
            {
                LOG_WARNING("current HW %d.%d does not support "\
                    "HDR Insertion - input forced to NONE\n",
                    (int)conn->sHWInfo.rev_ui8Major,
                    (int)conn->sHWInfo.rev_ui8Minor);

                glb->hdrInsertionType = PXL_NONE;
            }
            else
            {
                if (NULL != config->pszHDRInsertionFile[ctx])
                {
                    bool isValid;
                    IMG_RESULT ret = IsValidForCapture(*pCamera,
                        config->pszHDRInsertionFile[ctx], isValid, hdrInsMax);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("Failed to verify if provided HDR is "\
                            "not valid for current setup\n");
                        return EXIT_FAILURE;
                    }

                    if (!isValid)
                    {
                        LOG_ERROR("Provided HDR is not valid for "\
                            "current setup\n");
                        return EXIT_FAILURE;
                    }
                }
                else
                {
                    LOG_WARNING("disabling HDR insertion because "\
                        "no file was given as input\n");
                    glb->hdrInsertionType = PXL_NONE;
                }
            }

            if (PXL_NONE == glb->hdrInsertionType)
            {
                glb->requestUpdate();
            }
        }
        else if (NULL != config->pszHDRInsertionFile[ctx])
        {
            LOG_WARNING("HDF insertion format is not enabled but an HDF "\
                "insertion file was provided (HDF will not be enabled)\n");
        }
        // check HDR insertion

        // if enc output is setup for this context:
        if (config->aEncOutSelect[0] == ctx)
        {
            pipeline.getCIPipeline()->config.bUseEncOutLine = IMG_TRUE;
            pipeline.getCIPipeline()->config.ui8EncOutPulse =
                (IMG_UINT8)config->aEncOutSelect[1];
        }
        else
        {
            pipeline.getCIPipeline()->config.bUseEncOutLine = IMG_FALSE;
        }

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
    pCamera->state = ISPC::Camera::CAM_READY;

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

int TestContext::loadLSH(const ISPC::ParameterList &parameters)
{
    ISPC::ModuleLSH *lsh = pCamera->getModule<ISPC::ModuleLSH>();
    IMG_RESULT ret;

    if (config->bUseCtrlLSH[context])
    {
        LOG_DEBUG("Cannot use ModuleLSH directly. "\
                    "ControlLSH is used instead\n");
        return EXIT_FAILURE;
    }

    lshMatrixIds.clear();

    if (!lsh)
    {
        LOG_WARNING("no LSH module registered! Cannot override grid\n");
        return EXIT_FAILURE;
    }

    if (lshGridFile != NULL)
    {
        IMG_UINT32 matrixId;
        LOG_INFO("ignores context %d LSH grids and use '%s' instead\n",
            context, lshGridFile);
        ret = lsh->addMatrix(lshGridFile, matrixId);
        if (ret)
        {
            LOG_ERROR("failed to add LSH matrix!\n");
            return EXIT_FAILURE;
        }
        lshMatrixIds.push_back(matrixId);
        //lsh->bEnableMatrix = true; //FIXME - for test
        if (lsh->bEnableMatrix)
        {
            ret = lsh->configureMatrix(matrixId);
            if (ret)
            {
                LOG_ERROR("failed to configure LSH matrix!\n");
                return EXIT_FAILURE;
            }
        }
    }
    else
    {
        ret = lsh->loadMatrices(parameters, lshMatrixIds);
        // IMG_ERROR_CANCELLED if there were no matrices which is legitimate
        if (ret && IMG_ERROR_CANCELLED != ret)
        {
            LOG_ERROR("failed to add LSH matrices!\n");
            return EXIT_FAILURE;
        }

        if (lshMatrixIds.size() > 0)
        {
            LOG_INFO("Loaded %d LSH matrix from parameters\n",
                lshMatrixIds.size());

            if (lsh->bEnableMatrix)
            {
                /* could also use (*testCtx.lshMatrixIds.begin()) to
                * avoid the search but when we will get a parameter
                * just replace 0 by the index */
                lsh->configureMatrix(lsh->getMatrixId(0));
            }
        }
        else
        {
            LOG_INFO("No LSH matrix found in parameters\n");
        }
    }
    return EXIT_SUCCESS;
}

int TestContext::changeLSH(int frame)
{
    if (config->bUseCtrlLSH[context] && config->bChangeLSHPerFrame[context])
    {
        ISPC::ControlLSH *pLSH = pCamera->getControlModule<ISPC::ControlLSH>();
        if ( pLSH->isEnabled() )
        {
            LOG_WARNING("Disabling ControlLSH because -CTX_changeLSHPerFrame "\
                        "is true\n");
            pLSH->enableControl(false);
        }
    }

    if (config->bChangeLSHPerFrame[context] && lshMatrixIds.size() > 1)
    {
        int m = frame%lshMatrixIds.size();
        ISPC::ModuleLSH *lsh = pCamera->getModule<ISPC::ModuleLSH>();
        IMG_RESULT ret;

        LOG_INFO("frame %d - %d matrices => choosing matrix %d\n",
            frame, lshMatrixIds.size(), m);
        ret = lsh->configureMatrix(lshMatrixIds[m]);
        if (IMG_SUCCESS != ret)
        {
            // we may want to stop the capture and retry to allow loading of
            // matrices with different setup for image quality evaluation
            // but at the moment such test will need to run ISPC_test several
            // times
            LOG_ERROR("failed to change to matrix %d\n", m);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
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
            << "-" << FormatStringForFilename(capturedShot.YUV.pxlFormat)
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
            << "_" << FormatStringForFilename(capturedShot.BAYER.pxlFormat)
            << ".rggb";

        if ( saveRawBuffer(capturedShot.BAYER, stride, filename.str()) != EXIT_SUCCESS )
        {
            LOG_ERROR("failed to open raw DE file: %s\n", filename.str().c_str());
        }
    }
    else if ( capturedShot.DISPLAY.data != NULL )
    {
        bool isYuv444 = PixelFormatIsPackedYcc(capturedShot.DISPLAY.pxlFormat);

        // save as raw only if RGB (tiles or not) or yuv 444 is tiled
        if(!isYuv444 || capturedShot.DISPLAY.isTiled)
        {
            stride = 0;
            filename.str("");
            filename << "display" << this->context << "_" << savedRaw
                << "_" << capturedShot.DISPLAY.width << "x" << capturedShot.DISPLAY.height;
            if ( capturedShot.DISPLAY.isTiled )
            {
                filename << "_tiled" << tileW;
                stride = capturedShot.DISPLAY.stride;
            }
            else
            {
                PIXELTYPE stype;

                if ( PixelTransformDisplay(&stype, capturedShot.DISPLAY.pxlFormat) == IMG_SUCCESS )
                {
                    stride = (capturedShot.DISPLAY.width/stype.ui8PackedElements)*stype.ui8PackedStride;
                    if ( capturedShot.DISPLAY.width%stype.ui8PackedElements != 0 )
                    {
                        stride += stype.ui8PackedStride;
                    }
                }
            }
            filename << "_str" << stride
                << "_" << FormatStringForFilename(capturedShot.DISPLAY.pxlFormat);
            if (IMG_FALSE ==  isYuv444)
            {
                filename << ".rgb";
            } else {
                filename << ".yuv";
            }

            if ( saveRawBuffer(capturedShot.DISPLAY, stride, filename.str()) != EXIT_SUCCESS )
            {
                LOG_ERROR("failed to open raw RGB file: %s\n", filename.str().c_str());
            }
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
            << "_" << FormatStringForFilename(capturedShot.HDREXT.pxlFormat)
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
            << "_" << FormatStringForFilename(capturedShot.RAW2DEXT.pxlFormat)
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
        if (capturedShot.DISPLAY.isTiled)
        {
            static int nTiledDisplay = 0;
            FILE *f = 0;
            if (IMG_FALSE ==
                PixelFormatIsPackedYcc(capturedShot.DISPLAY.pxlFormat) )
            {
                sprintf(name, "tiled%d_%dx%d_(%dx%d-%d)-disp.rgb",
                    nTiledDisplay++,
                    capturedShot.DISPLAY.width, capturedShot.DISPLAY.height,
                    tileW, tileH,
                    capturedShot.DISPLAY.stride);
            } else {
                sprintf(name, "tiled%d_%dx%d_(%dx%d-%d)-disp.yuv",
                    nTiledDisplay++,
                    capturedShot.DISPLAY.width, capturedShot.DISPLAY.height,
                    tileW, tileH,
                    capturedShot.DISPLAY.stride);
            }

            f = fopen(name, "wb");
            if(f)
            {
                fwrite(capturedShot.DISPLAY.data,
                    capturedShot.DISPLAY.stride*capturedShot.DISPLAY.height,
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

            sprintf(name, "tiled%d_%dx%d_(%dx%d-%d)-enc.yuv", nTiled++,
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

    if (bStatsTxt)
    {
        static int nStatsSaved = 0;
        char filename[64];
        snprintf(filename, sizeof(filename),
            "ctx%d-%d-statistics.txt",
            this->context, nStatsSaved++);

        if (ISPC::Save::SingleTxtStats(*pCamera, capturedShot, filename) != IMG_SUCCESS)
        {
            LOG_ERROR("error saving text statistics.\n");
        }
    }

    const ISPC::ModuleDPF *pDPF = pCamera->getModule<ISPC::ModuleDPF>();
    if (pDPF && pDPF->bWrite && !bIgnoreDPF)
    {
        static int nDPFSaved = 0;
        char filename[64];
        snprintf(filename, sizeof(filename),
            "ctx%d-%d-dpf_write_%d.dat",
            this->context, nDPFSaved++,
            capturedShot.metadata.defectiveStats.ui32NOutCorrection);
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
        IMG_RESULT ret;

        ISPC::ParameterList parameters;
        ret = camera.saveParameters(parameters);
        if (IMG_SUCCESS == ret)
        {
            const ISPC::ModuleLSH *pLSH =
                camera.getPipeline()->getModule<ISPC::ModuleLSH>();

            if ( pLSH )
            {
                int i = 0;
                if (0 == nSetupSaved && config->bSaveLSHMatrices[context])
                {
                    std::vector<IMG_UINT32>::const_iterator it;
                    for (it = lshMatrixIds.begin(); it != lshMatrixIds.end(); it++)
                    {
                        filename.str("");
                        filename << "ctx" << this->context
                            << "-lshgrid" << *it << ".lsh";
                        ret = pLSH->saveMatrix(*it, filename.str());
                        if (ret)
                        {
                            LOG_WARNING("Failed to save LSH matrix %d (id=%d)\n",
                                i, *it);
                        }
                        else
                        i++;
                    }
                }
                i = pLSH->getCurrentMatrixId();
                if (0 < i)
                {
                    filename.str("");
                    filename << "ctx" << this->context
                        << "-lshgrid" << i << ".lsh";
                    // no parameter to set default matrix so save 1st one
                    parameters.addParameter(ISPC::Parameter(
                        ISPC::ControlLSH::LSH_FILE_S.name,
                        filename.str()));
                }
            }

            filename.str("");
            filename << "ctx" << this->context << "-" << nSetupSaved
                << "-FelixSetupArgs.txt";
            ret = ISPC::ParameterFileParser::saveGrouped(parameters,
                filename.str());
            if (IMG_SUCCESS != ret)
            {
                LOG_WARNING("failed to save setup parameters to disk.\n");
            }

            nSetupSaved++;
        }
        else
        {
            LOG_WARNING("failed to collect setup parameters for saving.\n");
        }
    }
}

int TestContext::configureAlgorithms(const ISPC::ParameterList &parameters, bool isOnwerOfCamera)
{
    // Create, setup and register control modules-----------------
    ISPC::ControlAWB *pAWB = 0;
    ISPC::ControlLBC *pLBC = new ISPC::ControlLBC();
    ISPC::ControlAE *pAE = 0;;
    ISPC::ControlTNM *pTNM = new ISPC::ControlTNM();
    ISPC::ControlDNS *pDNS = 0;
    ISPC::ControlLSH *pLSH = 0;
    if (config->bUseCtrlLSH[context])
    {
        pLSH = new ISPC::ControlLSH();
    }

    /// @ remove most of the direct set of parameters as setupfile should contain them!

    if(isOnwerOfCamera)
    {
        pAE = new ISPC::ControlAE();
        pDNS = new ISPC::ControlDNS(1.0);

        pCamera->registerControlModule(pAE);
        pCamera->registerControlModule(pDNS);     // not really used because using data generator the gain does not change
    }

    bool usePlanckian = false;
    if (this->config->autoWhiteBalanceAlgorithm[this->context] !=
            ISPC::ControlAWB::WB_NONE)
    {
        ISPC::ControlAWB::Correction_Types type = ISPC::ControlAWB::WB_NONE;
        switch(this->config->autoWhiteBalanceAlgorithm[this->context])
        {
        case TEST_PARAM::WB_AC:
            type = ISPC::ControlAWB::WB_AC;
            LOG_INFO("use WB Average Colour\n");
            break;
        case TEST_PARAM::WB_WP:
            type = ISPC::ControlAWB::WB_WP;
            LOG_INFO("use WB White Patch\n");
            break;
        case TEST_PARAM::WB_HLW:
            type = ISPC::ControlAWB::WB_HLW;
            LOG_INFO("use WB High Luminance White\n");
            break;
        case TEST_PARAM::WB_COMBINED:
            type = ISPC::ControlAWB::WB_COMBINED;
            LOG_INFO("use WB Combined\n");
            break;
        case TEST_PARAM::WB_PLANCKIAN:
        {
            IMG_UINT8 hwMajor = pCamera->getConnection()->sHWInfo.rev_ui8Major;
            IMG_UINT8 hwMinor = pCamera->getConnection()->sHWInfo.rev_ui8Minor;
            if(hwMajor<=2 && hwMinor<6)
            {
                printf("NOTICE: HW version earlier than 2.6: "
                        "Overriding AWB Planckian with AWB PID COMBINED\n");
                type = ISPC::ControlAWB::WB_COMBINED;
            }
            else
            {
                LOG_INFO("Use Planckian Locus\n");
                usePlanckian = true;
            }
        }
            break;
        default:
            LOG_WARNING("given WBC algorithm %d does not exists - force none\n", this->config->autoWhiteBalanceAlgorithm[this->context]);
        }

        if (usePlanckian || type != ISPC::ControlAWB::WB_NONE)
        {
            if(usePlanckian) {
                pAWB = new ISPC::ControlAWB_Planckian();
            } else {
                pAWB = new ISPC::ControlAWB_PID();
            }
            pCamera->registerControlModule(pAWB);

            pAWB->load(parameters);
            pAWB->setCorrectionMode(type);

        }
    }

    pCamera->registerControlModule(pLBC);
    pCamera->registerControlModule(pTNM);

    pCamera->loadControlParameters(parameters);

    if(usePlanckian)
    {
        ISPC::ControlAWB_Planckian *pAWB_Plackian = static_cast<ISPC::ControlAWB_Planckian*>(pAWB);
        bool type =
                this->config->WBTSEnabled;
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
        if(this->config->WBTSEnabledOverride) {
            enabled = this->config->WBTSEnabled;
        }
        if(this->config->WBTSWeightsBaseOverride) {
            weight = this->config->WBTSWeightsBase;
        }
        if(this->config->WBTSTemporalStretchOverride) {
            stretch = this->config->WBTSTemporalStretch;
        }
        if(this->config->WBTSFeaturesOverride) {
            features = this->config->WBTSFeatures;
        }

        pAWB_Plackian->setWBTSAlgorithm(enabled, weight, stretch);
        pAWB_Plackian->setWBTSFeatures(features);
    }
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

    if (pLSH)
    {
        IMG_RESULT ret;
        pCamera->registerControlModule(pLSH);
        if (pAWB)
        {
            pLSH->registerCtrlAWB(pAWB);
            /* Needs to be called before load(params)
             * Otherwise matrix change won't be possible. */
            pLSH->enableControl(true);
        }
        else
        {
            LOG_WARNING("Control LSH enabled without AWB algorithm. "\
                "The Control LSH does not have a source of temperature "\
                "estimation and is therefore not enabled.\n");
        }

        // testCtx.pCamera->loadParameters() was called before CtrlLSH creation
        // Now we need to load params manually
        ret = pLSH->load(parameters);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to load matrices!\n");
            return EXIT_FAILURE;
        }
        pLSH->getMatrixIds(lshMatrixIds);  // for -CTX_changeLSHPerFrame=1

        // to ensure it can change the matrix per frame later
        IMG_INT32 mat = pLSH->configureDefaultMatrix();
        if (mat < 0 && pLSH->isEnabled())
        {
            LOG_ERROR("failed to set the default matrix\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
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
                std::string tag = p->getTag();
                size_t pos = tag.rfind("_0");
                if(pos != std::string::npos)
                {
                    tag.replace(pos, 2, "_X"); // replace '_0' with '_X'
                }

                file << ".. |" << tag << "| replace:: " << tag << std::endl;

                file << ".. |" << tag << "_DEF| replace::";
                for ( unsigned int i = 0 ; i < p->size() ; i++ )
                {
                    data = p->getString(i);
                    file << " " << data;
                }
                file << std::endl;

                if ( !p->getInfo().empty() )
                {
                    comment += p->getInfo();
                }
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

                    file << ".. |" << tag << "_FMT| replace:: " << data << std::endl;

                    comment = comment.substr(pos, comment.size()-pos);

                    pos = comment.find_first_of("["); // search for range=[data]
                    if ( pos != std::string::npos )
                    {
                        size_t end = comment.find_first_of("]", pos);
                        data = comment.substr(pos, end-pos+1);

                        file << ".. |" << tag << "_RANGE| replace:: " << data << std::endl;
                    }
                    else
                    {
                        pos = comment.find_first_of("{"); // search for {data} (boolean or strings)
                        if ( pos != std::string::npos )
                        {
                            size_t end = comment.find_first_of("}", pos);
                            data = comment.substr(pos, end-pos+1);

                            file << ".. |" << tag << "_RANGE| replace:: " << data << std::endl;
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
                std::string tag = paramIt->first;
                size_t pos = tag.rfind("_0");
                if(pos != std::string::npos)
                {
                    tag.replace(pos, 2, "_X"); // replace '_0' with '_X'
                }

                file << ".. |" << tag << "| replace:: " << tag << std::endl;
                //file << paramIt->first;

                file << ".. |" << tag << "_DEF| replace::";
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
            LOG_DEBUG("generating for %s\n", (*mod_it)->getLoggingName());
            (*mod_it)->save(minParam, ISPC::ModuleBase::SAVE_MIN);
            (*mod_it)->save(maxParam, ISPC::ModuleBase::SAVE_MAX);
            (*mod_it)->save(defParam, ISPC::ModuleBase::SAVE_DEF);

            delete *mod_it;
            *mod_it = 0;
        }
    }

    std::list<ISPC::ControlModule*> controls = ISPC::CameraFactory::controlModulesFromHWVersion(FELIX_VERSION_MAJ, FELIX_VERSION_MIN);
    std::list<ISPC::ControlModule*>::iterator ctr_it;

    for ( ctr_it = controls.begin() ; ctr_it != controls.end() ; ctr_it++ )
    {
        if ( *ctr_it )
        {
            LOG_DEBUG("generating for %s\n", (*ctr_it)->getLoggingName());
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
    int ret = EXIT_SUCCESS;
    int ctx;
    TestContext testCtx;

    for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
    {
        int gasket = testparams.uiImager[ctx];

        if (NULL == testparams.pszFelixSetupArgsFile[ctx])
        {
            LOG_INFO("skip context %d: no setup found\n", ctx);
            continue;
        }
        if (CI_N_IMAGERS <= gasket)
        {
            LOG_ERROR("given gasket %d for context %d is not supported "\
                "by driver\n",
                gasket, ctx);
            return EXIT_FAILURE;
        }
        if (testparams.aIsIntDG[gasket])
        {
            if (CI_N_IIF_DATAGEN <= testparams.aIndexDG[gasket])
            {
                LOG_ERROR("given internal data-generator %d for gasket %d "\
                    "is not supported by driver\n",
                    testparams.aIndexDG[gasket], gasket);
                return EXIT_FAILURE;
            }
        }
        else
        {
            if (CI_N_EXT_DATAGEN < gasket)
            {
                LOG_ERROR("given external data-generator %d for gasket %d "\
                    "is not supported by driver\n",
                    gasket, gasket);
                return EXIT_FAILURE;
            }
        }
        if (NULL == testparams.pszInputFile[gasket])
        {
            LOG_INFO("skip context %d because its attached gasket %d "\
                "has no input file\n",
                ctx, gasket);
            continue;
        }

        if (testparams.bReUse[ctx])
        {
            if (ctx > 0 && testparams.uiImager[ctx - 1] == gasket)
            {
                LOG_INFO("context %d re-uses object created for context %d\n",
                    ctx, ctx - 1);

                std::list<IMG_UINT32>::iterator it;
                for (it = testCtx.bufferIds.begin();
                    it != testCtx.bufferIds.end(); it++)
                {
                    ret = testCtx.pCamera->deregisterBuffer(*it);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to deregister a buffer when "\
                            "trying to reuse previous context object\n");
                        return EXIT_FAILURE;
                    }
                }
                testCtx.bufferIds.clear();
                testCtx.pCamera->deleteShots();
            }
            else
            {
                if (ctx > 0)
                {
                    LOG_WARNING("context %d cannot re-use previous context "\
                        "object: context %d uses gasket %d but context %d "\
                        "wants gasket %d\n",
                        ctx, ctx - 1, testparams.uiImager[ctx - 1], ctx,
                        gasket);
                    if (testCtx.pCamera)
                    {
                        /* also deregisters the buffers */
                        delete testCtx.pCamera;
                        testCtx.pCamera = NULL;
                    }
                }
            }
        }
        else if (testCtx.pCamera)  // not reuse and we still have a camera
        {
            delete testCtx.pCamera;
            testCtx.pCamera = NULL;
        }

        if (NULL == testCtx.pCamera)
        {
            ISPC::CI_Connection sConn;
            ISPC::Sensor *sensor = 0;

            testCtx.pCamera = new ISPC::DGCamera(ctx,
                testparams.pszInputFile[gasket], gasket,
                testparams.aIsIntDG[gasket]);  // how to choose the DG context
            sensor = testCtx.pCamera->getSensor();

            ret = ISPC::CameraFactory::populateCameraFromHWVersion(
                *(testCtx.pCamera), sensor);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to setup modules from HW version!\n");
                return EXIT_FAILURE;
            }

            if (testparams.aIsIntDG[gasket])
            {
                ret = IIFDG_ExtendedSetDatagen(sensor->getHandle(),
                    testparams.aIndexDG[gasket]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG datagen\n");
                    return EXIT_FAILURE;
                }
                ret = IIFDG_ExtendedSetNbBuffers(sensor->getHandle(),
                    testparams.uiNBuffers[ctx]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG nBuffers\n");
                    return EXIT_FAILURE;
                }
                ret = IIFDG_ExtendedSetBlanking(sensor->getHandle(),
                    testparams.aBlanking[gasket][0],
                    testparams.aBlanking[gasket][1]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG blanking\n");
                    return EXIT_FAILURE;
                }
                //if (0 < testparams.aNBInputFrames[gasket])
                {
                    IMG_UINT32 cap = testparams.aNBInputFrames[gasket];
                    IMG_UINT32 nFrames = IIFDG_ExtendedGetFrameCount(
                        sensor->getHandle());
                    if (testparams.aNBInputFrames[gasket] < 0)
                    {
                        cap = nFrames;
                    }
                    else
                    {
                        cap = std::min(cap, nFrames);
                    }
                    if (cap != testparams.aNBInputFrames[gasket])
                    {
                        LOG_WARNING("forcing IntDG%d maximum loaded frame "\
                            "to %d as source image does not have more\n",
                            testparams.aIndexDG[gasket], cap);
                    }
                    ret = IIFDG_ExtendedSetFrameCap(sensor->getHandle(), cap);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to set frame cap\n");
                        return EXIT_FAILURE;
                    }
                }
            }
            else
            {
#ifdef EXT_DATAGEN
                ret = DGCam_ExtendedSetNbBuffers(sensor->getHandle(),
                    testparams.uiNBuffers[ctx]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set the number of DG buffers\n");
                    return EXIT_FAILURE;
                }

                ret = DGCam_ExtendedSetBlanking(sensor->getHandle(),
                    testparams.aBlanking[gasket][0],
                    testparams.aBlanking[gasket][1]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set ExtDG blanking\n");
                    return EXIT_FAILURE;
                }

                ret = DGCam_ExtendedSetUseMIPILF(sensor->getHandle(),
                    testparams.bUseMIPILF[gasket]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set Mipi LF\n");
                    return EXIT_FAILURE;
                }

                ret = DGCam_ExtendedSetPreload(sensor->getHandle(),
                    testparams.bPreload[gasket]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set Mipi LF\n");
                    return EXIT_FAILURE;
                }

                ret = DGCam_ExtendedSetMipiLanes(sensor->getHandle(),
                    testparams.uiMipiLanes[gasket]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set Mipi LF\n");
                    return EXIT_FAILURE;
                }

                //if (testparams.aNBInputFrames[gasket] > 0)
                {
                    IMG_UINT32 cap = testparams.aNBInputFrames[gasket];
                    IMG_UINT32 nFrames = 0;
                    DGCam_ExtendedGetFrameCount(sensor->getHandle(), &nFrames);
                    if (testparams.aNBInputFrames[gasket] < 0)
                    {
                        cap = nFrames;
                    }
                    else
                    {
                        cap = std::min(cap, nFrames);
                    }
                    if (cap != testparams.aNBInputFrames[gasket])
                    {
                        LOG_WARNING("forcing DG%d maximum loaded frame to "\
                            "%d as source image does not have more\n",
                            gasket, cap);
                    }

                    ret = DGCam_ExtendedSetFrameCap(sensor->getHandle(), cap);
                    if (IMG_SUCCESS != ret)
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

        ISPC::ParameterList parameters =
            ISPC::ParameterFileParser::parseFile(
            testparams.pszFelixSetupArgsFile[ctx]);


        if (false == parameters.validFlag)
        {
            LOG_ERROR("error while parsing parameter file '%s'\n",
                testparams.pszFelixSetupArgsFile[ctx]);
            return EXIT_FAILURE;
        }

        ret = testCtx.pCamera->loadParameters(parameters);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("error while loading setup parameters from file '%s'\n",
                testparams.pszFelixSetupArgsFile[ctx]);
            return EXIT_FAILURE;
        }

        testparams.populate(testCtx, ctx);
        ret = testCtx.configureOutput();
        if (EXIT_SUCCESS != ret)
        {
            LOG_ERROR("failed to configure output fields\n");
            return EXIT_FAILURE;
        }

        // if using internal DG
        if (testparams.aIsIntDG[gasket])
        {
            LOG_INFO("gasket %d uses Int. Datagen %d\n",
                gasket, testparams.aIndexDG[gasket]);
        }
#ifdef EXT_DATAGEN
        else  // using external DG
        {
            IMG_BOOL bUseMipi;
            bUseMipi = DGCam_ExtendedGetUseMIPI(
                testCtx.pCamera->getSensor()->getHandle());
            LOG_INFO("gasket %d uses Ext. Datagen with mipi=%d\n",
                gasket, (int)bUseMipi);
        }
#endif

        ret = testCtx.pCamera->program();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to program the camera\n");
            return EXIT_FAILURE;
        }

        if (!testparams.bUseCtrlLSH[ctx])
        {
            ret = testCtx.loadLSH(parameters);
            if (EXIT_SUCCESS != ret)
            {
                LOG_ERROR("failed to load LSH matrix\n");
                return EXIT_FAILURE;
            }
        }

        /* allocate buffers */
        ret = testCtx.allocateBuffers();
        if (EXIT_SUCCESS != ret)
        {
            LOG_ERROR("failed to allocate buffers\n");
            return EXIT_FAILURE;
        }

        /* configure algorithms */
        ret = testCtx.configureAlgorithms(parameters);
        if (EXIT_SUCCESS != ret)
        {
            LOG_ERROR("failed to configure the algorithms\n");
            return EXIT_FAILURE;
        }


        ret = testCtx.pCamera->startCapture();
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to start capture\n");
            return EXIT_FAILURE;
        }

        unsigned int frame = 0;
        const ISPC::ModuleOUT *pOut =
            testCtx.pCamera->getModule<const ISPC::ModuleOUT>();
        IMG_ASSERT(pOut);
        ret = EXIT_SUCCESS;
        while (frame < testCtx.uiNFrames && ret == EXIT_SUCCESS)
        {
            int b;
            unsigned pushed = 0;

            for (b = 0; b < (int)testparams.uiNBuffers[ctx]
                && frame + b < testCtx.uiNFrames; b++)
            {
                LOG_INFO("Enqueue frame: %d\n", frame + b);
                testCtx.changeLSH(frame + b);  // change lsh if option is on
                if (PXL_NONE == pOut->hdrInsertionType)
                {
                    ret = testCtx.pCamera->enqueueShot();
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to enqueue shot %d/%u\n",
                            frame, testCtx.uiNFrames);
                        ret = EXIT_FAILURE;
                        break;
                    }
                }
                else
                {
                    int hdrFrame = (frame + b) % testCtx.hdrInsMax;
                    ISPC::DGCamera *pCam = static_cast<ISPC::DGCamera*>(testCtx.pCamera);
                    ret = ProgramShotWithHDRIns(*(pCam),
                        testCtx.config->pszHDRInsertionFile[ctx],
                        testCtx.config->bRescaleHDRIns[ctx] ? true : false,
                        hdrFrame);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to enqueue shot with HDR "\
                            "insertion %d/%u\n",
                            frame, testCtx.uiNFrames);
                        ret = EXIT_FAILURE;
                        break;
                    }
                }
                pushed++;
            }

            while (pushed > 0)
            {
                ISPC::Shot captureBuffer;

                LOG_INFO("Acquire frame: %d\n", frame);

                ret = testCtx.pCamera->acquireShot(captureBuffer);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to acquire processed shot %d/%u from "\
                        "pipeline.\n",
                        frame, testCtx.uiNFrames);
                    ret = EXIT_FAILURE;
                    break;
                }

                testCtx.handleShot(captureBuffer, *(testCtx.pCamera));

                ret = testCtx.pCamera->releaseShot(captureBuffer);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to release shot %d/%u\n",
                        frame, testCtx.uiNFrames);
                    ret = EXIT_FAILURE;
                    break;
                }

                frame++;
                pushed--;
            }
        }  // end while

        // clean the run

        /* done in the if because if ret is already no EXIT_FAILURE we don't
         * want to override it but still want to stop */
        if (IMG_SUCCESS != testCtx.pCamera->stopCapture())
        {
            LOG_ERROR("failed to stop capture.\n");
            ret = EXIT_FAILURE;
        }

        testCtx.close();
    }  // for each context

    if (EXIT_SUCCESS != ret)
    {
        testCtx.printInfo();
    }
    return ret;
}

int runParallel(const struct TEST_PARAM& testparams)
{
    TestContext testCtx[CI_N_CONTEXT];
    int ctx;
    int gasket;
    /* owner of DG, last CTX to use it so that the capture is trigger
     * for both at once */
    int gasketOwner[CI_N_EXT_DATAGEN];
    int ret = EXIT_SUCCESS; // return code
    /* number of buffers to push for the context - has to be modified from
     * input because CSIM cannot allow 1 context to be empty */
    unsigned int nBuffers[CI_N_CONTEXT];
    /* number of frames left to compute on that context */
    unsigned int nToCompute[CI_N_CONTEXT];
    /* number of context still needing processing */
    unsigned int continueRunning = 0;

    for (ctx = 0; ctx < CI_N_EXT_DATAGEN; ctx++)
    {
        gasketOwner[ctx] = -1;  // no owner
    }

    for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
    {
        gasket = testparams.uiImager[ctx];
        /* may get limited later if context share same DG */
        nBuffers[ctx] = testparams.uiNBuffers[ctx];

        if (CI_N_IMAGERS <= gasket)
        {
            LOG_ERROR("given gasket %d for context %d is not supported "\
                "by driver!\n", gasket, ctx);
            return EXIT_FAILURE;
        }
        if (testparams.aIsIntDG[gasket])
        {
            if (testparams.aIndexDG[gasket] >= CI_N_IIF_DATAGEN)
            {
                LOG_ERROR("given internal data-generator %d  for gasket %d "\
                    "is not supported by driver!\n",
                    testparams.aIndexDG[gasket], gasket);
                return EXIT_FAILURE;
            }
        }
        else
        {
            if (CI_N_EXT_DATAGEN < gasket)
            {
                LOG_ERROR("given external data-generator %d  for gasket %d "\
                    "is not supported by driver!\n", gasket, gasket);
                return EXIT_FAILURE;
            }
        }

        /* find which Camera should be a DGCamera and which can be a normal
         * Camera with shared sensor
         * the last context to use the DG is the owner (so that the frame
         * is pushed last and ensure it happens) */
        if (testparams.pszFelixSetupArgsFile[ctx]
            && testparams.pszInputFile[gasket])
        {
            gasketOwner[testparams.uiImager[ctx]] = ctx;
        }
        else
        {
            LOG_INFO("Skipping context %d - setup is %s and input FLX is %s\n",
                ctx, testparams.pszFelixSetupArgsFile[ctx],
                testparams.pszInputFile[gasket]);
        }

        if (testCtx[ctx].pCamera)
        {
            delete testCtx[ctx].pCamera;
            testCtx[ctx].pCamera = NULL;
        }
    }  // for each context

    /* from now on only return should go through goto parallel_exit
     * or be a success! */

    /* the create the DG cameras */
    for (gasket = 0; gasket < CI_N_EXT_DATAGEN; gasket++)
    {
        ctx = gasketOwner[gasket];
        if (0 <= ctx)
        {
            ISPC::Sensor *sensor = 0;

            testCtx[ctx].pCamera = new ISPC::DGCamera(ctx,
                testparams.pszInputFile[gasket], gasket,
                testparams.aIsIntDG[gasket]);
            if (ISPC::Camera::CAM_ERROR == testCtx[ctx].pCamera->state)
            {
                LOG_ERROR("failed to create DGCamera for context %d!\n", ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            sensor = testCtx[ctx].pCamera->getSensor();

            if (testparams.aIsIntDG[gasket])
            {
                IMG_RESULT ret;

                ret = IIFDG_ExtendedSetDatagen(sensor->getHandle(),
                    testparams.aIndexDG[gasket]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG datagen\n");
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }
                ret = IIFDG_ExtendedSetNbBuffers(sensor->getHandle(),
                    testparams.uiNBuffers[ctx]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG nBuffers\n");
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }
                ret = IIFDG_ExtendedSetBlanking(sensor->getHandle(),
                    testparams.aBlanking[gasket][0],
                    testparams.aBlanking[gasket][1]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG blanking\n");
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }
                ret = IIFDG_ExtendedSetPreload(sensor->getHandle(),
                    testparams.bPreload[gasket]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to setup IIFDG preload\n");
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                //if (0 < testparams.aNBInputFrames[gasket])
                {
                    IMG_UINT32 cap = testparams.aNBInputFrames[gasket];
                    IMG_UINT32 nFrames = IIFDG_ExtendedGetFrameCount(
                        sensor->getHandle());
                    if (testparams.aNBInputFrames[gasket] < 0)
                    {
                        cap = nFrames;
                    }
                    else
                    {
                        cap = std::min(cap, nFrames);
                    }
                    if (cap != testparams.aNBInputFrames[gasket])
                    {
                        LOG_WARNING("forcing IntDG%d maximum loaded frame "\
                            "to %d as source image does not have more\n",
                            testparams.aIndexDG[gasket], cap);
                    }
                    ret = IIFDG_ExtendedSetFrameCap(sensor->getHandle(), cap);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to set frame cap\n");
                        ret = EXIT_FAILURE;
                        goto parallel_exit;
                    }
                }

                LOG_INFO("gasket %d uses Int. Datagen %d\n",
                    gasket, testparams.aIndexDG[gasket]);
            }
            else
            {
#ifdef EXT_DATAGEN
                IMG_BOOL bUseMipi = IMG_FALSE;
                IMG_RESULT ret;

                ret = DGCam_ExtendedSetNbBuffers(sensor->getHandle(),
                    testparams.uiNBuffers[ctx]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set the number of DG buffers\n");
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                ret = DGCam_ExtendedSetBlanking(sensor->getHandle(),
                    testparams.aBlanking[gasket][0],
                    testparams.aBlanking[gasket][1]);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("failed to set DG blanking\n");
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                //if (0 < testparams.aNBInputFrames[gasket])
                {
                    IMG_UINT32 cap = testparams.aNBInputFrames[gasket];
                    IMG_UINT32 nFrames = 0;
                    DGCam_ExtendedGetFrameCount(sensor->getHandle(), &nFrames);
                    if (testparams.aNBInputFrames[gasket] < 0)
                    {
                        cap = nFrames;
                    }
                    else
                    {
                        cap = std::min(cap, nFrames);
                    }
                    if (cap != testparams.aNBInputFrames[gasket])
                    {
                        LOG_WARNING("forcing DG%d maximum loaded frame to "\
                            "%d as source image does not have more\n",
                            gasket, cap);
                    }

                    ret = DGCam_ExtendedSetFrameCap(sensor->getHandle(), cap);
                    if (IMG_SUCCESS != ret)
                    {
                        LOG_ERROR("failed to set frame cap\n");
                        ret = EXIT_FAILURE;
                        goto parallel_exit;
                    }
                }

                bUseMipi = DGCam_ExtendedGetUseMIPI(sensor->getHandle());
                LOG_INFO("gasket %d uses Ext. Datagen with mipi=%d\n",
                    gasket, (int)bUseMipi);
#else
                LOG_ERROR("compiled without external data generator support! "\
                    "Cannot use external data generator.\n");
                ret = EXIT_FAILURE;
                goto parallel_exit;
#endif
            }
        }  // if it has a context
    } // for creation of DGCameras

    //
    // limit the number of frames to compute if not owning the gasket AFTER
    // each context is setup
    //
    for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
    {
        // default is compute all the frames
        nToCompute[ctx] = testCtx[ctx].uiNFrames;
        gasket = testparams.uiImager[ctx];
        int dgOwner = gasketOwner[gasket];

        if (testparams.pszFelixSetupArgsFile[ctx] && 0 <= gasket
            && 0 <= dgOwner)
        {
            bool isOwner = false;
            ISPC::Sensor *sensor = 0;
            if (NULL == testCtx[ctx].pCamera)
            {
                sensor = testCtx[dgOwner].pCamera->getSensor();

                /* may need to become DGCamera if we want to be able to
                 * transfert ownership */
                testCtx[ctx].pCamera = new ISPC::Camera(ctx, sensor);

                if (ISPC::Camera::CAM_ERROR == testCtx[ctx].pCamera->state)
                {
                    LOG_ERROR("failed to create DGCamera for context %d!\n",
                        ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }
            }
            else
            {
                isOwner = true;
                sensor = testCtx[ctx].pCamera->getSensor();
            }

            ret = ISPC::CameraFactory::populateCameraFromHWVersion(
                *(testCtx[ctx].pCamera), sensor);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to setup modules from HW version for "\
                    "context %d!\n", ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            ISPC::ParameterList parameters =
                ISPC::ParameterFileParser::parseFile(
                testparams.pszFelixSetupArgsFile[ctx]);

            if (false == parameters.validFlag)
            {
                LOG_ERROR("failed to parse context %d parameter file '%s'\n",
                    ctx, testparams.pszFelixSetupArgsFile[ctx]);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            ret = testCtx[ctx].pCamera->loadParameters(parameters);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to load context %d setup parameters "\
                    "from file: '%s'\n",
                    ctx, testparams.pszFelixSetupArgsFile[ctx]);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            testparams.populate(testCtx[ctx], ctx);
            ret = testCtx[ctx].configureOutput();
            if (EXIT_SUCCESS != ret)
            {
                LOG_ERROR("failed to configure output fields for "\
                    "context %d\n", ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            ret = testCtx[ctx].pCamera->program();
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to program context %d\n",
                    ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            if (!testparams.bUseCtrlLSH[ctx])
            {
                ret = testCtx[ctx].loadLSH(parameters);
                if (EXIT_SUCCESS != ret)
                {
                    LOG_ERROR("failed to load LSH matrix\n");
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }
            }

            ret = testCtx[ctx].allocateBuffers();
            if (EXIT_SUCCESS != ret)
            {
                LOG_ERROR("faield to alloce buffers for context %d\n",
                    ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            /* configure algorithms */
            ret = testCtx[ctx].configureAlgorithms(parameters, isOwner);
            if (EXIT_SUCCESS != ret)
            {
                LOG_ERROR("faield to configured algorithms\n");
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            ret = testCtx[ctx].pCamera->startCapture();
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("failed to start context %d\n", ctx);
                ret = EXIT_FAILURE;
                goto parallel_exit;
            }

            continueRunning++;
        }
    }  // for each context: init

    for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
    {
        // default is compute all the frames
        nToCompute[ctx] = testCtx[ctx].uiNFrames;
        gasket = testparams.uiImager[ctx];
        int dgOwner = gasketOwner[gasket];

        // gasketOwner can be -1 if not used at all
        if (0 > dgOwner)
        {
            nToCompute[ctx] = 0;
        }
        else if (ctx != dgOwner)
        {
            unsigned int ownerBuffers = nBuffers[dgOwner];
            unsigned int ownerFrames = testCtx[dgOwner].uiNFrames;

            /*
             * limit the number of frames to compute if not owning the gasket
             * this has to be done AFTER each context has been setup properly
             * by TEST_PARAM::populate()
             */

            nBuffers[ctx] = ownerBuffers;
            if (testparams.uiNBuffers[ctx] != ownerBuffers)
            {
                LOG_WARNING("change number of buffers for context "\
                    "%d from %u to %u to allow to run with shared "\
                    "data generator\n",
                    ctx, testparams.uiNBuffers[ctx], nBuffers[ctx]);
            }

            nToCompute[ctx] = ownerFrames;
            if (testCtx[ctx].uiNFrames != ownerFrames)
            {
                LOG_WARNING("change number of frames to compute "\
                    "for context %d from %u to %u as context %d "\
                    "owns the Data Generator\n",
                    ctx, testCtx[ctx].uiNFrames,
                    nToCompute[ctx], gasketOwner[gasket]);
            }
        }
    }

    while (continueRunning > 0)
    {
        /* process frames
         * for the moment: assumes that all contexts that share a DG have the
         * same number of buffers and same number of frames to process
         * assumes that the context that owns the DG is the one with the
         * highest CTX number */
        unsigned int nPushed[CI_N_CONTEXT];
        unsigned int f;

        /* first push frames */
        for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
        {
            if (testCtx[ctx].pCamera &&
                ISPC::Camera::CAM_CAPTURING == testCtx[ctx].pCamera->state)
            {
                nPushed[ctx] = IMG_MIN_INT(nBuffers[ctx], nToCompute[ctx]);
                const ISPC::ModuleOUT *pOut = testCtx[ctx].pCamera->\
                    getModule<const ISPC::ModuleOUT>();

                for (f = 0; f < nPushed[ctx]; f++)
                {
                    unsigned frame = testCtx[ctx].uiNFrames - nToCompute[ctx];
                    /* change grid used if option is enabled */
                    testCtx[ctx].changeLSH(frame);
                    if (PXL_NONE == pOut->hdrInsertionType)
                    {
                        ret = testCtx[ctx].pCamera->enqueueShot();
                        if (IMG_SUCCESS != ret)
                        {
                            LOG_ERROR("failed to enqueue shot %u/%u ctx %d\n",
                                frame, testCtx[ctx].uiNFrames, ctx);
                            ret = EXIT_FAILURE;
                            goto parallel_exit;
                        }
                    }
                    else
                    {
                        int hdrFrame = (frame - f) % testCtx[ctx].hdrInsMax;
                        /* if owning the camera we program shot with DG
                         * camera so that it triggers the capture
                         * if not owning the camera the DG should not be
                         * triggered! */
                        //  check that because code is the same!
                        if (testCtx[ctx].pCamera->ownsSensor())
                        {
                            ISPC::DGCamera *pCam =
                                static_cast<ISPC::DGCamera*>(testCtx[ctx].pCamera);
                            /* overloaded for DGCam to trigger shot */
                            ret = ProgramShotWithHDRIns(*(pCam),
                                testCtx[ctx].config->pszHDRInsertionFile[ctx],
                                testCtx[ctx].config->bRescaleHDRIns[ctx] ? true : false,
                                hdrFrame);
                            if (IMG_SUCCESS != ret)
                            {
                                LOG_ERROR("failed to enqueue shot with "\
                                    "HDR insertion %d/%u ctx %d\n",
                                    frame, testCtx[ctx].uiNFrames, ctx);
                                ret = EXIT_FAILURE;
                                goto parallel_exit;
                            }
                        }
                        else
                        {
                            ret = ProgramShotWithHDRIns(
                                *(testCtx[ctx].pCamera),
                                testCtx[ctx].config->pszHDRInsertionFile[ctx],
                                testCtx[ctx].config->bRescaleHDRIns[ctx] ? true : false,
                                hdrFrame);
                            if (IMG_SUCCESS != ret)
                            {
                                LOG_ERROR("failed to enqueue shot with "\
                                    "HDR insertion %d/%d\n",
                                    frame, testCtx[ctx].uiNFrames);
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
        }  // for frame trigger

        /* now acquire pushed frames */
        for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
        {
            for (f = 0; f < nPushed[ctx]; f++)
            {
                ISPC::Shot captureBuffer;

                ret = testCtx[ctx].pCamera->acquireShot(captureBuffer);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("Failed to acquire a frame for context %d\n",
                        ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                testCtx[ctx].handleShot(captureBuffer, *(testCtx[ctx].pCamera));

                ret = testCtx[ctx].pCamera->releaseShot(captureBuffer);
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("Failed to release a frame for context %d\n",
                        ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                IMG_ASSERT(nToCompute[ctx] > 0);
                nToCompute[ctx]--;
            }

            if (0 == nToCompute[ctx] && testCtx[ctx].pCamera)
            {
                LOG_INFO("stopping context %d\n", ctx);
                ret = testCtx[ctx].pCamera->stopCapture();
                if (IMG_SUCCESS != ret)
                {
                    LOG_ERROR("Failed to stop context %d\n", ctx);
                    ret = EXIT_FAILURE;
                    goto parallel_exit;
                }

                IMG_ASSERT(continueRunning > 0);
                continueRunning--;
            }
        }
    }

parallel_exit:
    for (ctx = 0; ctx < CI_N_CONTEXT; ctx++)
    {
        if (EXIT_SUCCESS != ret)
        {
            testCtx[ctx].printInfo();
        }
        if (testCtx[ctx].pCamera)
        {
            delete testCtx[ctx].pCamera;
            testCtx[ctx].pCamera = NULL;
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

    // Load command line parameters
    ret = parameters.load_parameters(argc, argv);
    if (0 < ret)
    {
        return EXIT_FAILURE;
    }

    if (parameters.bDumpConfigFiles)
    {
        return dumpConfigFiles();
    }

#ifdef FELIX_FAKE
    if (parameters.pszSimIP)
    {
        pszTCPSimIp = parameters.pszSimIP;
    }
    if (0 != parameters.uiSimPort)
    {
        ui32TCPPort = parameters.uiSimPort;
    }
    bDisablePdumpRDW = !parameters.bEnableRDW;
    // FAKE driver insmod when running against the sim
    initializeFakeDriver(parameters.uiUseMMU,
        parameters.uiGammaCurve, parameters.uiCPUPageSize << 10,
        parameters.uiTilingScheme, parameters.uiTilingStride,
        parameters.aEDGPLL[0], parameters.aEDGPLL[1],
        parameters.pszTransifLib, parameters.pszTransifSimParam);
#endif /* FELIX_FAKE */

#ifndef EXT_DATAGEN
    {
        /* check that HW supports internal DG otherwise we simply cannot
         * running */
        ISPC::CI_Connection conn;
        const CI_CONNECTION *pConn = conn.getConnection();

        if (0 ==
            (pConn->sHWInfo.eFunctionalities&CI_INFO_SUPPORTED_IIF_DATAGEN))
        {
            LOG_ERROR("compiled without external data generator and HW "\
                "%d.%d does not support internal data generator. "\
                "Cannot run.\n",
                (int)pConn->sHWInfo.rev_ui8Major,
                (int)pConn->sHWInfo.rev_ui8Minor);
            return EXIT_FAILURE;
        }
}
#endif
    if (parameters.bPrintHWInfo)
    {
        ISPC::CI_Connection conn;
        const CI_CONNECTION *pConn = conn.getConnection();

        std::cout << pConn->sHWInfo << std::endl;
    }

    time_t start = time(NULL);

    if (IMG_FALSE == parameters.bParallel)
    {
        LOG_INFO("Running context one after the other\n");
        ret = runSingle(parameters);
    }
    else
    {
        LOG_INFO("Running context in parallel\n");
        ret = runParallel(parameters);
    }

    start = time(NULL) - start;
    LOG_INFO("Test ran in %ld seconds\n", start);

#ifdef FELIX_FAKE
    // FAKE driver rmmod
    finaliseFakeDriver();
#endif

    if (EXIT_SUCCESS == ret)
    {
        LOG_INFO("finished with success\n");
    }
    else
    {
        LOG_INFO("finished with errors\n");
    }

    if (parameters.waitAtFinish)
    {
        LOG_INFO("Press Enter to terminate\n");
        getchar();
    }

    return ret;
}
