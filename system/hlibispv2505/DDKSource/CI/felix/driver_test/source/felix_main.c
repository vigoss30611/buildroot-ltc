/**
*******************************************************************************
 @file felix_main.c

 @brief Command line parsing for the driver_test (test application of the CI
 layer)

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
#include <img_types.h>
#include <img_errors.h>

#include "felix.h"

#include <ci/ci_version.h> // CI_CHANGELIST CI_DATE
// used to setup black level and other registers
#include <ctx_reg_precisions.h> 

#include <dyncmd/commandline.h>
#include <mc/module_config.h>  // to initialise AWS
#ifdef FELIX_FAKE
#include <ci_kernel/ci_debug.h>

#undef CI_FATAL
#undef CI_WARNING
#undef CI_INFO
#undef CI_DEBUG
#endif /* FELIX_FAKE */

#include <time.h>
#define LOG_TAG "driver_test"
#include <felixcommon/userlog.h>

// to check at compile time what we verify in the loop below
IMG_STATIC_ASSERT(CI_N_IIF_DATAGEN == 1, moreThanOneIIFDG); 
#ifdef FELIX_FAKE
IMG_STATIC_ASSERT(CI_N_HEAPS == N_HEAPS, ciNHeapsChanged);
#endif /* FELIX_FAKE */

int main(int argc,char * argv[])
{
    IMG_BOOL8 bForceQuit = IMG_FALSE, bDumpParams = IMG_FALSE,
        bPrintHelp = IMG_FALSE;
    IMG_UINT32 iRet, i;
    struct CMD_PARAM parameters;
    time_t runtime;
    char param_name[64];

    init_Param(&parameters);

    LOG_INFO("CHANGELIST %s - DATE %s\n", CI_CHANGELIST_STR, CI_DATE_STR);
    LOG_INFO("Compiled for %d contexts, %d internal datagen\n",
        CI_N_CONTEXT, CI_N_IIF_DATAGEN);
#ifdef CI_EXT_DATAGEN
    LOG_INFO("Compiled for %d ext datagen\n", CI_N_EXT_DATAGEN);
#else
    LOG_INFO("No support for ext datagen\n");
#endif
#ifdef CI_DEBUG_FCT
    LOG_INFO ("INFO: Debug functions available (register override, register "\
        "test)\n");
#endif

    /*
     * generic parameters
     */

    iRet = DYNCMD_AddCommandLine(argc, argv, "-source");
    if (IMG_SUCCESS != iRet)
    {
        LOG_ERROR("unable to parse command line\n");
        return EXIT_FAILURE;
    }

    /*
     * find help first to avoid printing error messages if parameters are not
     * found
     */
    iRet = DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND,
        "Usage information",
        NULL);
    if (RET_FOUND == iRet)
    {
        bPrintHelp = IMG_TRUE;
    }

    iRet = DYNCMD_RegisterParameter("-dump_params", DYNCMDTYPE_COMMAND,
        "Dump the defaults and generated parameters for the driver test",
        NULL);
    if (RET_FOUND == iRet)
    {
        dumpParameters(&parameters, "defaults_params.txt");
        bDumpParams = IMG_TRUE;
    }

    iRet = DYNCMD_RegisterParameter("-no_write", DYNCMDTYPE_COMMAND,
        "Do not write output to file (including statistics)",
        NULL);
    if (RET_FOUND == iRet)
    {
        parameters.bNowrite = IMG_TRUE;
    }

    iRet = DYNCMD_RegisterParameter("-rough_exit", DYNCMDTYPE_COMMAND,
        "When returning only close connection to CI/DG and rely on them "\
        "to clean objects. Expect error/warnings when doing so.",
        NULL);
    if (RET_FOUND == iRet)
    {
        parameters.bRoughExit = IMG_TRUE;
    }

#ifdef FELIX_FAKE

    iRet = DYNCMD_RegisterParameter("-no_pdump", DYNCMDTYPE_COMMAND,
        "Do not write pdumps",
        NULL);
    if (RET_FOUND == iRet)
    {
        // global variable from ci_debug.h
        bEnablePdump = IMG_FALSE;
    }

    iRet = DYNCMD_RegisterParameter("-simIP", DYNCMDTYPE_STRING,
        "Simulator IP address to use when using TCP/IP interface.",
        &(parameters.pszSimIP));
    if (RET_NOT_FOUND == iRet)
    {
        // nothing to really say...
    }

    iRet = DYNCMD_RegisterParameter("-simPort", DYNCMDTYPE_INT,
        "Simulator port to use when running TCP/IP interface.",
        &(parameters.uiSimPort));
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("-simPort expetcs an int but none was found\n");
            return EXIT_FAILURE;
        }
    }

    iRet = DYNCMD_RegisterParameter("-simTransif", DYNCMDTYPE_STRING,
        "Transif library given by the simulator. Also needs -simconfig to "\
        "be used.",
        &(parameters.pszTransifLib));
    if (RET_NOT_FOUND == iRet && !bPrintHelp)
    {
        LOG_INFO("INFO: TCP/IP sim connection\n");
    }

    iRet = DYNCMD_RegisterParameter("-simConfig", DYNCMDTYPE_STRING,
        "Configuration file to give to the simulator when using transif "\
        "(-argfile file parameter of the sim)",
        &(parameters.pszTransifSimParam));
    if (RET_NOT_FOUND == iRet && !bPrintHelp)
    {
        if (parameters.pszTransifLib != NULL)
        {
            LOG_ERROR("simulator parameter file not given\n");
            return EXIT_FAILURE;
        }
        LOG_INFO("INFO: Transif sim connection (%s)\n",
            parameters.pszTransifLib);
    }

    iRet = DYNCMD_RegisterParameter("-simTransifTimed", DYNCMDTYPE_BOOL8,
        "Use timed model for transif.",
        &(parameters.bTransifUseTimedModel));
    if (RET_NOT_FOUND ==  iRet)
    {
        parameters.bTransifUseTimedModel = IMG_FALSE;
    }
    else
    {
        if (parameters.bTransifUseTimedModel)
            LOG_INFO("INFO: Transif use timed model\n");
        else
            LOG_INFO("INFO: Transif use untimed model\n");
    }

    iRet = DYNCMD_RegisterParameter("-simTransifDebug", DYNCMDTYPE_BOOL8,
        "Debug transif operations.",
        &(parameters.bTransifDebug));
    if (RET_NOT_FOUND == iRet)
    {
        parameters.bTransifDebug = IMG_FALSE;
    }
    else
    {
        if (parameters.bTransifDebug)
            LOG_INFO("INFO: Transif debugging enabled\n");
        else
            LOG_INFO("INFO: Transif debugging disabled\n");
    }

    iRet = DYNCMD_RegisterParameter("-pdumpEnableRDW", DYNCMDTYPE_BOOL8,
        "Enable generation of RDW commands in pdump output (not useful for "\
        "verification)",
        &(parameters.bEnableRDW));
    if (RET_FOUND !=  iRet)
    {
        if (iRet != RET_NOT_FOUND )
        {
            LOG_ERROR("-pdumpDisableRDW expetcs a bool but none was found\n");
            return EXIT_FAILURE;
        }
    }

    iRet = DYNCMD_RegisterParameter("-mmucontrol", DYNCMDTYPE_INT,
        "Flag to indicate how to use the MMU - 0 = bypass, 1 = 32b MMU, "\
        ">1 = extended addressing",
        &(parameters.uiUseMMU));
    if (RET_NOT_FOUND == iRet && !bPrintHelp)
    {
        LOG_INFO("INFO: using default MMU config: %s\n",
            parameters.uiUseMMU>0? "on" : "off");
    }
    else if (RET_INCORRECT == iRet && !bPrintHelp)
    {
        LOG_ERROR("-mmucontrol expetcs an int but none was found\n");
        return EXIT_FAILURE;
    }

    iRet = DYNCMD_RegisterParameter("-tilingScheme", DYNCMDTYPE_INT,
        "Choose tiling scheme (256 = 256Bx16, 512 = 512Bx8)",
        &(parameters.uiTilingScheme));
    if (RET_INCORRECT == iRet && !bPrintHelp)
    {
        LOG_ERROR("-tilingScheme expects an integer and none was found\n");
        return EXIT_FAILURE;
    }

    iRet = DYNCMD_RegisterParameter("-tilingStride", DYNCMDTYPE_INT,
        "If non-0 will be used as the tiling stride (needs to be a power of "\
        "2 big enough for all allocations)",
        &(parameters.uiTilingStride));
    if (RET_INCORRECT == iRet && !bPrintHelp)
    {
        LOG_ERROR("-tilingStride expects an integer and none was found\n");
        return EXIT_FAILURE;
    }

    iRet = DYNCMD_RegisterParameter("-gammaCurve", DYNCMDTYPE_UINT,
        "Gamma Look Up Curve to use - 0 = BT709, 1 = sRGB (other is invalid)",
        &(parameters.uiGammaCurve));
    if (RET_INCORRECT == iRet && !bPrintHelp)
    {
        LOG_ERROR("expecting an uint for -gammaCurve option\n");
        return EXIT_FAILURE;
    }

#ifdef CI_EXT_DATAGEN
    iRet = DYNCMD_RegisterArray("-extDGPLL", DYNCMDTYPE_UINT,
        "External Data-generator PLL values (multiplier and divider). "\
        "HAS TO RESPECT HW LIMITATIONS or FPGA may need to be reprogrammed.",
        parameters.aEDGPLL, 2);
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("expecting 2 uint for -extDGPLL option\n");
            return EXIT_FAILURE;
        }
    }
#endif

    iRet = DYNCMD_RegisterParameter("-cpuPageSize", DYNCMDTYPE_INT,
        "CPU page size in kB (e.g. 4, 16)",
        &(parameters.uiCPUPageSize));
    if (RET_NOT_FOUND != iRet && !bPrintHelp)
    {
        if (iRet == RET_INCORRECT)
        {
            LOG_ERROR("-cpuPageSize expects an int but none was found\n");
            return EXIT_FAILURE;
        }
    }
    if (!bPrintHelp)
    {
        LOG_INFO("INFO: using CPU page size: %dkB\n", parameters.uiCPUPageSize);
    }

    iRet=DYNCMD_RegisterArray("-virtHeapOffset", DYNCMDTYPE_UINT,
        "Virtual address heaps offsets in bytes - will be rounded up to "\
        "MMU page size",
        parameters.aHeapOffsets, N_HEAPS);
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_INCOMPLETE == iRet)
        {
            char number[10];
            int i;

            sprintf(param_name, "");
            for(i = 0 ; i < N_HEAPS ; i++)
            {
                sprintf(number, " %d", parameters.aHeapOffsets[i]);
                strcat(param_name, number);
            }

            LOG_WARNING("not enough values found when parsing "\
                "-virtHeapOffset, using%s\n", param_name);
        }
        else if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("failure when parsing %s\n", param_name);
            bForceQuit = IMG_TRUE;
        }
    }

#endif // FELIX_FAKE

    iRet = DYNCMD_RegisterArray("-encOutCtx", DYNCMDTYPE_UINT,
        "Which context to use for the Encoder pipeline statistics output: "\
        "1st value is context, second value is the pulse"\
        "(see FELIX_CORE:ENC_OUT_CTRL:ENC_OUT_CTRL_SEL)",
        parameters.aEncOutSelect, 2);
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("-encOutCtx expects 2 int but none were found\n");
            return EXIT_FAILURE;
        }
    }

    iRet = DYNCMD_RegisterParameter("-data_extraction", DYNCMDTYPE_INT,
        "Choose global Data Extraction point [1..2]",
        &(parameters.ui8DataExtraction));
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("-data_extraction expects an integer and none "\
                "was found\n");
            return EXIT_FAILURE;
        }
    }
    if (!bPrintHelp)
    {
        LOG_INFO("INFO: using data extraction point %d\n",
            parameters.ui8DataExtraction);
    }

    iRet = DYNCMD_RegisterParameter("-parallel", DYNCMDTYPE_COMMAND,
        "Run the shoots in 'parallel' - i.e. submit them at the same time",
        NULL);
    if (RET_FOUND == iRet)
    {
        parameters.bParallel = IMG_TRUE;
    }

    iRet = DYNCMD_RegisterParameter("-register_test", DYNCMDTYPE_UINT,
        "Run the register test (0 none - 1 read/write all registers instead "\
        "of capture - 2 verify load structure and GMA Lut after capture)",
        &(parameters.uiRegisterTest));
    if (RET_INCORRECT == iRet && !bPrintHelp)
    {
        LOG_ERROR("-register_test expects an integer and none was found\n");
        return EXIT_FAILURE;
    }

    iRet = DYNCMD_RegisterParameter("-hw_info", DYNCMDTYPE_COMMAND,
        "Prints the HW information from the connection object",
        NULL);
    if (RET_FOUND == iRet)
    {
        parameters.bPrintHWInfo = IMG_TRUE;
    }

    iRet = DYNCMD_RegisterParameter("-ignoreDPF", DYNCMDTYPE_BOOL8,
        "Override DPF results in statistics and skip saving of the DPF "\
        "output file (used for verification as CSIM and HW do not match on"\
        "DPF output)",
        &(parameters.bIgnoreDPF));
    if (RET_FOUND != iRet)
    {
        if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("-ignoreDPF expects a boolean and none "\
                "was found\n");
            return EXIT_FAILURE;
        }
    }
    
    iRet = DYNCMD_RegisterParameter("-writeDGFrame", DYNCMDTYPE_BOOL8,
        "Write converted DG frame to text file",
        &(parameters.bWriteDGFrame));
    if (RET_FOUND != iRet)
    {
        if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("-writeDGFrame expects a boolean and none "\
                "was found\n");
            return EXIT_FAILURE;
        }
    }

    /*
     * pipeline
     */
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        snprintf(param_name, sizeof(param_name), "-CT%d_useLSH", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_INT,
            "Use the deshading grid module (LSH): 0 disables, 1 generate "\
            "linear grid from 4 corners, 2 generate grid from 5 corners "\
            "(4 corners + center)",
            &(parameters.sCapture[i].iUseDeshading));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_fixLSH", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_BOOL8,
            "[0] Fix the LSH grid if the differences are too big, "\
            "[1] allow modification of tile size to support HW max line size "\
            "- may not generate a nice grid but should run, if only "\
            "1 parameter is given it will be duplicated to the 2nd parameter",
            parameters.sCapture[i].bFixLSH, 2);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                parameters.sCapture[i].bFixLSH[1] =
                    parameters.sCapture[i].bFixLSH[0];
                LOG_WARNING("only 1 value specified for %s, assumes second "\
                    "value is %u\n",
                    param_name, parameters.sCapture[i].bFixLSH[1]);
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_LSHTile", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_INT,
            "The deshading tile size in CFA (between LSH_TILE_MIN and "\
            "LSH_TILE_MAX and has to be a power of 2) - used to compute "\
            "deshading grid size using input FLX size and corner values",
            &(parameters.sCapture[i].uiLshTile));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_LSHCorners", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
            "The corner values used to generate the matrix (TL, TR, BL, "\
            "BR, CE) per channel - center is only used if useLSH is 2 but "\
            "has to be specified",
            parameters.sCapture[i].lshCorners, LSH_GRADS_NO*LSH_N_CORNERS);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                LOG_ERROR("not enough values found when parsing %s\n",
                    param_name);
                bForceQuit = IMG_TRUE;
            }
            else if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_LSHGrads", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
            "The gradients values {X,Y} per channel",
            parameters.sCapture[i].lshGradients, 2 * LSH_GRADS_NO);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                LOG_ERROR("not enough values found when parsing %s\n",
                    param_name);
                bForceQuit = IMG_TRUE;
            }
            else if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_encoder", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "The filename to use as the output of the encoder - if none do "\
            "not use encoder, if the name does not end by '.yuv' the the "\
            "size and format will be added",
            &(parameters.sCapture[i].pszEncoderOutput));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_rawYUV", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Enable output of the raw YUV output from HW (i.e. do not "\
            "remove the strides - adds an additional output file)",
            &(parameters.sCapture[i].bRawYUV));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_YUV422", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Encoder outputs 422 instead of 420",
            &(parameters.sCapture[i].bEncOut422));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_YUV10b", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Encoder outputs 10b (packed) instead of 8b",
            &(parameters.sCapture[i].bEncOut10b));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_YUVAlloc", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Size to allocate for the YUV buffer in bytes "\
            "(multiple of 64 Bytes required) "\
            "- 0 to let driver compute the size "\
            "- ignored if buffer is tiled",
            &(parameters.sCapture[i].uiYUVAlloc));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_YUVStrides", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_UINT,
            "Strides for the Y and CbCr (multiple of 64 Bytes required) "\
            "- 0 means computed by the driver "\
            "- ignored if buffer is tiled "\
            "- if only one is given it is assumed to be luma stride and is "\
            "duplicated to the chroma stride.",
            (parameters.sCapture[i].uiYUVStrides), 2);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                parameters.sCapture[i].uiYUVStrides[1] =
                    parameters.sCapture[i].uiYUVStrides[0];
                LOG_WARNING("only 1 stride with given to %s - duplicating "\
                    "luma stride %u to chroma %u\n",
                    param_name, parameters.sCapture[i].uiYUVStrides[0],
                    parameters.sCapture[i].uiYUVStrides[1]);
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_YUVOffsets", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_UINT,
            "Offset for Y and CbCr buffers in bytes "\
            "(multiple of 64 Bytes required) "\
            "- CbCr offset of 0 means computed by the driver "\
            "- ignored if buffer is tiled "\
            "- if only one is given it is assumed to be the luma offset, "\
            "and chroma offset will be set to 0.",
            (parameters.sCapture[i].uiYUVOffsets), 2);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                parameters.sCapture[i].uiYUVOffsets[1] = 0;
                LOG_WARNING("only 1 offset with given to %s - duplicating "\
                    "luma offset is %u (chroma is %u)\n",
                    param_name, parameters.sCapture[i].uiYUVOffsets[0],
                    parameters.sCapture[i].uiYUVOffsets[1]);
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_display", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "The filename to use as the output of the display - if none "\
            "display pipeline not used",
            &(parameters.sCapture[i].pszDisplayOutput));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_RGB24", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Display outputs RGB888 in 24b instead of RGB888 in 32b - "\
            "RGB10 takes precedence over RGB24 option",
            &(parameters.sCapture[i].bDispOut24));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_RGB10", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Display outputs RGB101010 in 32b instead of RGB888 in 32b - "\
            "takes precedence over RGB24 option",
            &(parameters.sCapture[i].bDispOut10));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_DispAlloc", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Display output (or DE output) allocation size in bytes "\
            "(multiple of 64 Bytes required) "\
            "- 0 to let driver compute the size "\
            "- ignored if buffer is tiled",
            &(parameters.sCapture[i].uiDispAlloc));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_DispStride", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Display stride (or DE stride) in bytes "\
            "(multiple of 64 Bytes required) "\
            "- 0 to let driver compute the stride "\
            "- ignored if buffer is tiled",
            &(parameters.sCapture[i].uiDispStride));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_DispOffset", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Display offset (or DE) in bytes (multiple of 64 Bytes required) "\
            "- ignored if buffer is tiled",
            &(parameters.sCapture[i].uiDispOffset));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_dataExtraction", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "Enable the Data Extraction for this context - see "\
            "data_extraction to choose DE point - cannot be used with "\
            "display enabled, by default disabled",
            &(parameters.sCapture[i].pszDataExtOutput));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_DEBitDepth", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Data Extraction Bayer output bitdepth, 0 for pipeline "\
            "bitdepth (default), <10 for 8b, >10 for 12b and 10 for 10b",
            &(parameters.sCapture[i].uiDEBitDepth));
        if (RET_FOUND != iRet)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_rawDisp", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "save raw RGB or Bayer data (no stride removal)",
            &(parameters.sCapture[i].bRawDisp));
        if (RET_FOUND != iRet)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_raw2DExtraction", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "Enable the Raw 2D Extraction for this context and provides "\
            "filename to use for output (raw)",
            &(parameters.sCapture[i].pszRaw2ExtDOutput));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_raw2DFormat", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Raw 2D format bitdepth (can be 10 or 12), default 10",
            &(parameters.sCapture[i].uiRaw2DBitDepth));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_raw2DAlloc", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Raw 2D allocation size in bytes "\
            "(multiple of 64 Bytes required) "\
            "- 0 to let driver compute the size",
            &(parameters.sCapture[i].uiRaw2DAlloc));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_raw2DStride", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Raw 2D stride in bytes (does NOT need to be a multiple of "\
            "64 Bytes) "\
            "- 0 to let driver compute the stride",
            &(parameters.sCapture[i].uiRaw2DStride));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_raw2DOffset", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Raw2D offset in bytes (does NOT need to be a multiple of "\
            "64 Bytes)",
            &(parameters.sCapture[i].uiRaw2DOffset));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_rawTiff", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Additional output of the raw TIFF format from RAW 2D output",
            &(parameters.sCapture[i].brawTiff));
        if (RET_FOUND != iRet)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }


        if ( parameters.sCapture[i].pszDataExtOutput != NULL
            && parameters.sCapture[i].pszDisplayOutput != NULL 
            && !bPrintHelp)
        {
            LOG_ERROR("Both data extraction and display pipeline cannot "\
                "be enabled!\n");
            bForceQuit = IMG_TRUE;
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_HDRExtraction", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "The filename to use as the output of the HDR Extraction - "\
            "if none do not use HDR Extraction (format is RGB10 32b with "\
            "R,G,B order)",
            &(parameters.sCapture[i].pszHDRextOutput));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_HDRExtAlloc", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "HDR Extraction allocation size in bytes "\
            "(multiple of 64 Bytes required) "\
            "- 0 to let driver compute the size "\
            "- ignored if buffer is tiled",
            &(parameters.sCapture[i].uiHDRExtAlloc));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_HDRExtStride", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "HDR Extraction stride in bytes "\
            "(multiple of 64 Bytes required) "\
            "- 0 to let driver compute the stride "\
            "- ignored if buffer is tiled",
            &(parameters.sCapture[i].uiHDRExtStride));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_HDRExtOffset", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "HDR Extraction offset in bytes (multiple of 64 Bytes required) "\
            "- ignored if buffer is tiled",
            &(parameters.sCapture[i].uiHDRExtOffset));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_rawHDR", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Additional output of the raw HDR Extraction buffer that "\
            "HW provided",
            &(parameters.sCapture[i].bAddRawHDR));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_HDRInsertion", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "The filename to use as the input of the HDR Insertion - "\
            "if none do not use HDR Insertion, image has to be an FLX "\
            "in RGB of the same size than the one provided to DG but does "\
            "not have to be of the correct bitdepth",
            &(parameters.sCapture[i].pszHDRInsInput));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_HDRInsRescale", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Rescale provided HDR Insertion file to 16b per pixels",
            &(parameters.sCapture[i].bRescaleHDRIns));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_nBuffers", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Give a number of buffers to have ready for capture",
            &(parameters.sCapture[i].ui32ConfigBuffers));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing the %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_nEncTiled", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Give a number of Encoder buffers to have tiled (if same than"\
            "-CTX_nBuffers then all are tiled) - mmucontrol must be enabled!",
            &(parameters.sCapture[i].ui32NEncTiled));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing the %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_nDispTiled", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Give a number of Display buffers to have tiled (if same than "\
            "-CTX_nBuffers then all are tiled) - mmucontrol must be enabled!",
            &(parameters.sCapture[i].ui32NDispTiled));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing the %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_nHDRExtTiled", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Give a number of HDR Extraction buffers to have tiled (if "\
            "same than -CTX_nBuffers then all are tiled) - mmucontrol must "\
            "be enabled!",
            &(parameters.sCapture[i].ui32NHDRExtTiled));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing the %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_nFrames", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_INT,
            "Number of frames to run for. Negative value and it will use "\
            "all frames from its associated input.",
            &(parameters.sCapture[i].i32RunForFrames));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing the %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_imager", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "The imager to use",
            &(parameters.sCapture[i].ui8imager));
        if (RET_FOUND != iRet)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_stats", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_INT,
            "If -1 given all the statistics are enabled for this context "\
            "(use the same value as the save flags in linked list to enable "\
            "stats one by one).",
            &(parameters.sCapture[i].iStats));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_enableTS", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Enable timestamps generation - no effect when running "\
            "Fake driver",
            &(parameters.sCapture[i].bEnableTimestamps));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("Failure when parsing %s, expecting a bool an "\
                    "none was found.\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_updateMaxSize", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Update maxEncOutWidth maxEncOutHeight maxDispOutWidth "\
            "and maxDispOutHeight using pitch computed output size",
            &(parameters.sCapture[i].bUpdateMaxSize));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("Failure when parsing %s, expecting a bool an "\
                    "none was found.\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_allocateLate", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "If false allocate the buffer just after configuring the "\
            "pipeline, if enabled will allocate the buffers after starting",
            &(parameters.sCapture[i].bAllocateLate));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("Failure when parsing %s, expecting a bool an "\
                    "none was found.\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_ENSNLines", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Then Encoder Statistics parameters: number of lines for a "\
            "region (change output size of ENS - see register for minimum "\
            "and maximum, should be power of 2).",
            &(parameters.sCapture[i].uiENSNLines));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_escPitch", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
            "Encoder scaler horizontal and vertical pitch (ratio "\
            "input/output) - second value is optional and horizontal pitch "\
            "is duplicated",
            parameters.sCapture[i].escPitch, 2);
        if (RET_FOUND != iRet  && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                parameters.sCapture[i].escPitch[1] =
                    parameters.sCapture[i].escPitch[0];
                LOG_WARNING("only 1 value specified for %s, assumes "\
                    "second value is the same %f\n",
                    param_name, parameters.sCapture[i].escPitch[1]);
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_dscPitch", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
            "Encoder scaler horizontal and vertical pitch (ratio "\
            "input/output) - second value is optional and horizontal pitch "\
            "is duplicated",
            parameters.sCapture[i].dscPitch, 2);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                parameters.sCapture[i].dscPitch[1] =
                    parameters.sCapture[i].dscPitch[0];
                LOG_WARNING("only 1 value specified for %s, assumes "\
                    "second value is the same %f\n",
                    param_name, parameters.sCapture[i].dscPitch[1]);
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_black", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Aimed REGISTER black level value for the context "\
            "(default is equivalent to 64)",
            &(parameters.sCapture[i].uiSysBlack));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_DPF", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_INT,
            "Enable defective pixel detection (0 disabled, 1 detect, "\
            ">1 detect+write result)",
            &(parameters.sCapture[i].iDPFEnabled));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_DPFReadMap", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "DPF read map input file (binary - following DPF read map format)",
            &(parameters.sCapture[i].pszDPFRead));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

#ifndef RLT_NOT_AVAILABLE
        snprintf(param_name, sizeof(param_name), "-CT%d_RLTMode", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "RLT mode selection (0=disabled, 1=linear, 2=cubic) - "\
            "changes meaning of -CTX_RLTSlopes",
            &(parameters.sCapture[i].ui32RLTMode));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_RLTSlopes", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
            "RLT slopes values - if linear values for each section of the "\
            "curve, if cubic slope value per channel in R,G1,G2,B order",
            parameters.sCapture[i].aRLTSlopes, RLT_SLICE_N);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                LOG_ERROR("%s needs %d floats!\n", param_name, RLT_SLICE_N);
                bForceQuit = IMG_TRUE;
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_RLTKnee", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_UINT,
            "RLT knee point values - if linear knee points for the curve "\
            "(0 is implicitly the 1st value) - between 1 and 65535",
            parameters.sCapture[i].aRLTKnee, RLT_SLICE_N - 1);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                LOG_ERROR("%s needs %d uints!\n", param_name, RLT_SLICE_N-1);
                bForceQuit = IMG_TRUE;
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }
#endif /* RLT_NOT_AVAILABLE */

#ifdef AWS_LINE_SEG_AVAILABLE
    snprintf(param_name, sizeof(param_name), "-CT%d_AWSCoeffX", i);
    iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
        "AWS line segment: horizontal coefficients",
        parameters.sCapture[i].aCurveCoeffX, AWS_LINE_SEG_N);
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_INCOMPLETE == iRet)
        {
            LOG_ERROR("%s needs %d floats!\n", param_name, AWS_LINE_SEG_N);
            bForceQuit = IMG_TRUE;
        }
        else if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("failure when parsing %s\n", param_name);
            bForceQuit = IMG_TRUE;
        }
    }

    snprintf(param_name, sizeof(param_name), "-CT%d_AWSCoeffY", i);
    iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
        "AWS line segment: vertical coefficients",
        parameters.sCapture[i].aCurveCoeffY, AWS_LINE_SEG_N);
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_INCOMPLETE == iRet)
        {
            LOG_ERROR("%s needs %d floats!\n", param_name, AWS_LINE_SEG_N);
            bForceQuit = IMG_TRUE;
        }
        else if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("failure when parsing %s\n", param_name);
            bForceQuit = IMG_TRUE;
        }
    }

    snprintf(param_name, sizeof(param_name), "-CT%d_AWSOffset", i);
    iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
        "AWS line segment: offsets",
        parameters.sCapture[i].aCurveOffset, AWS_LINE_SEG_N);
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_INCOMPLETE == iRet)
        {
            LOG_ERROR("%s needs %d floats!\n", param_name, AWS_LINE_SEG_N);
            bForceQuit = IMG_TRUE;
        }
        else if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("failure when parsing %s\n", param_name);
            bForceQuit = IMG_TRUE;
        }
    }

    snprintf(param_name, sizeof(param_name), "-CT%d_AWSBoundary", i);
    iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_FLOAT,
        "AWS line segment: boundaries (0, 0-1, 1-2, 2-3, 3-4)",
        parameters.sCapture[i].aCurveBoundary, AWS_LINE_SEG_N);
    if (RET_FOUND != iRet && !bPrintHelp)
    {
        if (RET_INCOMPLETE == iRet)
        {
            LOG_ERROR("%s needs %d floats!\n", param_name, AWS_LINE_SEG_N);
            bForceQuit = IMG_TRUE;
        }
        else if (RET_NOT_FOUND != iRet)
        {
            LOG_ERROR("failure when parsing %s\n", param_name);
            bForceQuit = IMG_TRUE;
        }
    }
#endif /* AWS_LINE_SEG_AVAILABLE */

#ifdef FELIX_FAKE
        snprintf(param_name, sizeof(param_name), "-CT%d_regOverride", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Allow register override if a file is present (file name "\
            "format: drv_regovr_ctx<context number>_frm<global frame "\
            "number>.txt). Available only if debug functions are present. "\
            "0 disables register override even if file is present, 1 "\
            "enables it (loading 1 file per frame), >1 is used as a module "\
            "(loop on the files using the global frame counter).",
            &(parameters.sCapture[i].uiRegOverride));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-CT%d_checkCRC", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Enable the generation of CRCs for HW testing (fake interface)",
            &(parameters.sCapture[i].bCRC));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s - expecting a boolean\n",
                    param_name);
                bForceQuit = IMG_TRUE;
            }
        }
#endif

    } // for every context

#ifdef CI_EXT_DATAGEN
    /*
     * data gen parameters
     */
    for ( i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        snprintf(param_name, sizeof(param_name), "-DG%d_inputFLX", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "Input FLX file for data generator.",
            &(parameters.sDataGen[i].pszInputFLX));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND == iRet)
            {
                LOG_INFO("INFO: DG %d not used (no input file found)\n", i);
            }
            else
            {
                LOG_ERROR("Failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-DG%d_inputFrames", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_INT,
            "Number of frames to use from the DG input file. If negative "\
            "uses all frames from the input file.",
            &(parameters.sDataGen[i].iFrames));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("Failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-DG%d_blanking", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_UINT,
            "Horizontal and vertical blanking to use",
            parameters.sDataGen[i].aBlanking, 2);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                LOG_WARNING("Found only horizontal blanking! Leaving "\
                    "vertical blanking to %u!\n",
                    parameters.sDataGen[i].aBlanking[1]);
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }
        
        snprintf(param_name, sizeof(param_name), "-DG%d_MIPI_LF", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Flag to indicate we should use MIPI Line Flags for the "\
            "data generator", 
            &(parameters.sDataGen[i].bMIPIUsesLineFlag));
        if (RET_FOUND != iRet  && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-DG%d_preload", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Enable the DG line preload feature",
            &(parameters.sDataGen[i].bPreload));
        if (RET_FOUND != iRet  && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-DG%d_mipiLanes", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Number of MIPI lanes to use (1, 2 or 4) - no effect if gasket "\
            "is parallel",
            &(parameters.sDataGen[i].uiMipiLanes));
        if (RET_FOUND != iRet  && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

    } // for DG
#endif // CI_EXT_DATAGEN

    // for internal DG
    for ( i = 0 ; i < CI_N_IIF_DATAGEN ; i++ )
    {
        snprintf(param_name, sizeof(param_name), "-IntDG%d_inputFLX", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_STRING,
            "Input FLX file for internal data generator.",
            &(parameters.sIIFDG[i].pszInputFLX));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_NOT_FOUND == iRet)
            {
                LOG_INFO("INFO: IntDG %d not used (no input file found)\n", i);
            }
            else
            {
                LOG_ERROR("Failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-IntDG%d_inputFrames", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_INT,
            "Number of frames to use from the IntDG input file. If "\
            "negative loads all frames from the input file.",
            &(parameters.sIIFDG[i].iFrames));
        if (RET_FOUND != iRet)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("Failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-IntDG%d_gasket", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_UINT,
            "Gasket to replace data from with IntDG",
            &(parameters.sIIFDG[i].uiGasket));
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if ( iRet != RET_NOT_FOUND )
            {
                LOG_ERROR("Failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-IntDG%d_blanking", i);
        iRet = DYNCMD_RegisterArray(param_name, DYNCMDTYPE_UINT,
            "Horizontal and vertical blanking to use",
            parameters.sIIFDG[i].aBlanking, 2);
        if (RET_FOUND != iRet && !bPrintHelp)
        {
            if (RET_INCOMPLETE == iRet)
            {
                LOG_WARNING("Found only horizontal blanking! Leaving "\
                    "vertical blanking to %u!\n",
                    parameters.sIIFDG[i].aBlanking[1]);
            }
            else if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }

        snprintf(param_name, sizeof(param_name), "-IntDG%d_preload", i);
        iRet = DYNCMD_RegisterParameter(param_name, DYNCMDTYPE_BOOL8,
            "Enable the internal DG line preload feature",
            &(parameters.sIIFDG[i].bPreload));
        if (RET_FOUND != iRet  && !bPrintHelp)
        {
            if (RET_NOT_FOUND != iRet)
            {
                LOG_ERROR("failure when parsing %s\n", param_name);
                bForceQuit = IMG_TRUE;
            }
        }
    }

    /*
    * must be last action
    */
    if (bPrintHelp || bForceQuit)
    {
        int i;
        printf("> ");
        for (i = 0; i < argc; i++)
        {
            printf("%s ", argv[i]);
        }
        printf("\n");
        DYNCMD_PrintUsage();
        DYNCMD_ReleaseParameters();
        return bForceQuit == IMG_TRUE ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    // now verify that at least 1 ctx and 1 dg are enabled
    iRet = 0;

    // disables the DG that are using the same gasket than internal DG
    for ( i = 0 ; i < CI_N_IIF_DATAGEN ; i++ )
    {
        if ( parameters.sIIFDG[i].pszInputFLX != NULL )
        {
            unsigned int gasket = parameters.sIIFDG[i].uiGasket;
            if (gasket < CI_N_EXT_DATAGEN &&
                parameters.sDataGen[gasket].pszInputFLX)
            {
                LOG_WARNING("External DG %d enabled but internal DG will "\
                    "override its gasket - force DG to disabled\n",
                    parameters.sIIFDG[i].uiGasket);
                parameters.sDataGen[gasket].pszInputFLX = NULL;
            }
        }
    }
    /* because there is only 1 internal DG we do not have to check that 
     * several internal DG override the same gasket */
    IMG_ASSERT(CI_N_IIF_DATAGEN == 1);

    for ( i = 0 ; i < CI_N_CONTEXT && parameters.uiRegisterTest != 1 ; i++ )
    {
        if ( parameters.sCapture[i].pszEncoderOutput != NULL ||
            parameters.sCapture[i].pszDisplayOutput != NULL ||
            parameters.sCapture[i].pszDataExtOutput != NULL ||
            parameters.sCapture[i].pszHDRextOutput != NULL ||
            parameters.sCapture[i].pszRaw2ExtDOutput != NULL ||
            parameters.sCapture[i].iStats != 0 ||
            parameters.sCapture[i].iDPFEnabled > 1 )
        {
            unsigned int imager = parameters.sCapture[i].ui8imager;
            if (imager < CI_N_EXT_DATAGEN &&
                parameters.sDataGen[imager].pszInputFLX != NULL )
            {
                LOG_INFO("INFO: CTX %d using DG %d - encoder %s, "\
                    "display %s, data-extraction %s, RAW 2D ext %s, "\
                    "HDR ext %s, HDR ins %s, stats %s\n",
                    i, imager, parameters.sCapture[i].pszEncoderOutput ?
                    "enabled" : "disabled",
                    parameters.sCapture[i].pszDisplayOutput ?
                    "enabled" : "disabled",
                    parameters.sCapture[i].pszDataExtOutput ?
                    "enabled" : "disabled",
                    parameters.sCapture[i].pszRaw2ExtDOutput ?
                    "enabled" : "disabled",
                    parameters.sCapture[i].pszHDRextOutput ?
                    "enabled" : "disabled",
                    parameters.sCapture[i].pszHDRInsInput ?
                    "enabled" : "disabled",
                    parameters.sCapture[i].iStats != 0 ||
                    parameters.sCapture[i].iDPFEnabled > 1 ?
                    "enabled" : "disabled"
                );
                if ( parameters.sCapture[i].i32RunForFrames < 0 )
                {
                    parameters.sCapture[i].i32RunForFrames =
                        parameters.sDataGen[imager].iFrames;
                }
                iRet++;
            }
            else
            {
                int j = 0;
                for ( j = 0 ; j < CI_N_IIF_DATAGEN ; j++ )
                {
                    if ( parameters.sIIFDG[j].uiGasket == 
                        parameters.sCapture[i].ui8imager 
                        && parameters.sIIFDG[j].pszInputFLX != NULL )
                    {
                        parameters.sCapture[i].iIIFDG = j;
                        LOG_INFO("INFO: CTX %d using IntDG %d on gasket %d "\
                            "- encoder %s, display %s, data-extraction %s, "\
                            "RAW 2D ext %s, HDR ext %s, HDR ins %s, stats %s\n",
                            i, j, parameters.sCapture[i].ui8imager,
                            parameters.sCapture[i].pszEncoderOutput ?
                            "enabled" : "disabled",
                            parameters.sCapture[i].pszDisplayOutput ?
                            "enabled" : "disabled",
                            parameters.sCapture[i].pszDataExtOutput ?
                            "enabled" : "disabled",
                            parameters.sCapture[i].pszRaw2ExtDOutput ?
                            "enabled" : "disabled",
                            parameters.sCapture[i].pszHDRextOutput ?
                            "enabled" : "disabled",
                            parameters.sCapture[i].pszHDRInsInput ?
                            "enabled" : "disabled",
                            parameters.sCapture[i].iStats != 0 ||
                            parameters.sCapture[i].iDPFEnabled > 1 ?
                            "enabled" : "disabled"
                        );
                        if ( parameters.sCapture[i].i32RunForFrames < 0 )
                        {
                            parameters.sCapture[i].i32RunForFrames =
                                parameters.sIIFDG[ j ].iFrames;
                        }
                        iRet++;
                        break;
                    }
                    
                }

                // if it is not using an internal DG
                if ( parameters.sCapture[i].iIIFDG == -1 )
                {
                    LOG_ERROR("CTX %d is trying use disabled DG %d\n", i,
                        parameters.sCapture[i].ui8imager);
                    bForceQuit = IMG_TRUE;
                }
            }

        }
    }
    if ( iRet == 0 && parameters.uiRegisterTest != 1 )
    {
        LOG_ERROR("No CTX enabled\n");
        bForceQuit = IMG_TRUE;
    }
    
    DYNCMD_HasUnregisteredElements(&iRet);
    DYNCMD_ReleaseParameters();

#ifdef FELIX_FAKE
    if ( parameters.uiUseMMU > 0 )
    {
        LOG_INFO("Using MMU (extended address %d) tiling scheme %d\n", 
            parameters.uiUseMMU > 1, parameters.uiTilingScheme);
    }
    else
    {
        LOG_INFO("Using direct allocation\n");
    }
#endif

    if (bDumpParams)
    {
        dumpParameters(&parameters, "used_params.txt");
    }
    
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        IMG_UINT enc = parameters.sCapture[i].ui32NEncTiled, disp =
            parameters.sCapture[i].ui32NDispTiled;

        parameters.sCapture[i].ui32NEncTiled = 
            IMG_MIN_INT(enc, parameters.sCapture[i].ui32ConfigBuffers);
        parameters.sCapture[i].ui32NDispTiled = 
            IMG_MIN_INT(disp, parameters.sCapture[i].ui32ConfigBuffers);

        if ( enc != parameters.sCapture[i].ui32NEncTiled )
        {
            printf("INFO: reduced number of encoder tiled buffers to fit in "\
                "the number of config buffers from %d to %d\n",
                enc, parameters.sCapture[i].ui32NEncTiled);
        }
        if ( disp != parameters.sCapture[i].ui32NDispTiled )
        {
            printf("INFO: reduced number of display tiled buffers to fit in "\
                "the number of config buffers from %d to %d\n", 
                disp, parameters.sCapture[i].ui32NDispTiled);
        }
    }

    runtime = time(NULL);

    iRet = init_drivers(&parameters);
    if (EXIT_SUCCESS == iRet)
    {
        iRet = run_drivers(&parameters);
        finalise_drivers();
    }

    runtime = time(NULL)-runtime;

    printf("driver terminated returning %d in %ld sec\n", iRet, runtime);

    clean_Param(&parameters);

    printf("main returns %d\n", iRet);
    return iRet;
}

#define GET_OFFSET(container, element) \
    ( ((IMG_UINTPTR)(&((container)->element)))-(IMG_UINTPTR)container )

#define PRINT_ELEM(file, container, element, print) fprintf(file, \
    "   %s [0x%"IMG_PTRDPR"x] = %" print "\n", #element, \
    GET_OFFSET(container, element), (container)->element);

static void printDG_Param(const struct DG_PARAM *param, FILE *file)
{
    int i;
    fprintf(file, "struct DG_PARAM 0x%p (sizeof 0x%"IMG_SIZEPR"x)\n",
        param, sizeof(struct DG_PARAM));
    PRINT_ELEM(file, param, pszInputFLX, "s");
    PRINT_ELEM(file, param, iFrames, "d");
    for ( i = 0 ; i < 2 ; i++ )
    {
        PRINT_ELEM(file, param, aBlanking[i], "u");
    }
    PRINT_ELEM(file, param, bMIPIUsesLineFlag, "d");
    PRINT_ELEM(file, param, bPreload, "d");
    PRINT_ELEM(file, param, uiMipiLanes, "u");
}

static void printIIFDG_Param(const struct IIFDG_PARAM *param, FILE *file)
{
    int i;
    fprintf(file, "struct IIFDG_PARAM 0x%p (sizeof 0x%"IMG_SIZEPR"x)\n",
        param, sizeof(struct IIFDG_PARAM));
    PRINT_ELEM(file, param, pszInputFLX, "s");
    PRINT_ELEM(file, param, iFrames, "d");
    for ( i = 0 ; i < 2 ; i++ )
    {
        PRINT_ELEM(file, param, aBlanking[i], "u");
    }
    PRINT_ELEM(file, param, bPreload, "d");
    PRINT_ELEM(file, param, uiGasket, "u");
}

static void printCapture_Param(const struct CAPTURE_PARAM *param, FILE *file)
{
    int i;
    fprintf(file, "struct CAPTURE_PARAM 0x%p (sizeof 0x%"IMG_SIZEPR"x)\n",
        param, sizeof(struct CAPTURE_PARAM));
    PRINT_ELEM(file, param, ui8imager, "u");
    PRINT_ELEM(file, param, iStats, "d");
    PRINT_ELEM(file, param, bCRC, "d");
    PRINT_ELEM(file, param, bEnableTimestamps, "d");
    PRINT_ELEM(file, param, bUpdateMaxSize, "d");
    PRINT_ELEM(file, param, bAllocateLate, "d");

    PRINT_ELEM(file, param, uiSysBlack, "u");
    for (i = 0; i < 2; i++)
    {
        PRINT_ELEM(file, param, bFixLSH[i], "d");
    }
    PRINT_ELEM(file, param, iUseDeshading, "d");
    PRINT_ELEM(file, param, uiLshTile, "u");
    for ( i = 0; i < LSH_GRADS_NO*LSH_N_CORNERS ; i++ )
    {
        PRINT_ELEM(file, param, lshCorners[i], "f");
    }
    for ( i = 0; i < 2*LSH_GRADS_NO ; i++ )
    {
        PRINT_ELEM(file, param, lshGradients[i], "f");
    }
    PRINT_ELEM(file, param, ui32RLTMode, "u");
    for ( i = 0 ; i < RLT_SLICE_N ; i++ )
    {
        PRINT_ELEM(file, param, aRLTSlopes[i], "f");
    }
    for ( i = 0 ; i < RLT_SLICE_N-1 ; i++ )
    {
        PRINT_ELEM(file, param, aRLTKnee[i], "d");
    }
    for (i = 0; i < AWS_LINE_SEG_N; i++)
    {
        PRINT_ELEM(file, param, aCurveCoeffX[i], "f");
        PRINT_ELEM(file, param, aCurveCoeffY[i], "f");
        PRINT_ELEM(file, param, aCurveOffset[i], "f");
        PRINT_ELEM(file, param, aCurveBoundary[i], "f");
    }
    PRINT_ELEM(file, param, iDPFEnabled, "d");

    PRINT_ELEM(file, param, ui32ConfigBuffers, "u");
    PRINT_ELEM(file, param, i32RunForFrames, "d");

    PRINT_ELEM(file, param, pszEncoderOutput, "s");
    PRINT_ELEM(file, param, pszDisplayOutput, "s");
    PRINT_ELEM(file, param, pszDataExtOutput, "s");
    PRINT_ELEM(file, param, pszHDRextOutput, "s");
    PRINT_ELEM(file, param, pszHDRInsInput, "s");
    PRINT_ELEM(file, param, pszRaw2ExtDOutput, "s");

    PRINT_ELEM(file, param, bRawYUV, "d");
    PRINT_ELEM(file, param, bEncOut422, "d");
    PRINT_ELEM(file, param, bEncOut10b, "d");
    PRINT_ELEM(file, param, uiYUVAlloc, "u");
    PRINT_ELEM(file, param, uiYUVStrides[0], "u");
    PRINT_ELEM(file, param, uiYUVStrides[1], "u");
    PRINT_ELEM(file, param, uiYUVOffsets[0], "u");
    PRINT_ELEM(file, param, uiYUVOffsets[1], "u");

    PRINT_ELEM(file, param, bDispOut24, "d");
    PRINT_ELEM(file, param, bDispOut10, "d");
    PRINT_ELEM(file, param, uiDEBitDepth, "u");
    PRINT_ELEM(file, param, bRawDisp, "d");
    PRINT_ELEM(file, param, uiDispAlloc, "u");
    PRINT_ELEM(file, param, uiDispStride, "u");

    PRINT_ELEM(file, param, uiRaw2DBitDepth, "u");
    PRINT_ELEM(file, param, brawTiff, "d");
    PRINT_ELEM(file, param, uiRaw2DAlloc, "u");
    PRINT_ELEM(file, param, uiRaw2DStride, "u");

    PRINT_ELEM(file, param, uiHDRExtAlloc, "u");
    PRINT_ELEM(file, param, uiHDRExtStride, "u");

    PRINT_ELEM(file, param, bRescaleHDRIns, "d");

    PRINT_ELEM(file, param, escPitch[0], "f");
    PRINT_ELEM(file, param, escPitch[1], "f");
    PRINT_ELEM(file, param, dscPitch[0], "f");
    PRINT_ELEM(file, param, dscPitch[1], "f");

    PRINT_ELEM(file, param, uiENSNLines, "u");

    PRINT_ELEM(file, param, uiRegOverride, "u");

    PRINT_ELEM(file, param, iIIFDG, "d");
}

static void printCMD_Param(const struct CMD_PARAM *param, FILE *file)
{
    int i;
    fprintf(file, "struct CMD_PARAM 0x%p (sizeof 0x%"IMG_SIZEPR"x)\n",
        param, sizeof(struct CMD_PARAM));
    PRINT_ELEM(file, param, pszTransifLib, "s");
    PRINT_ELEM(file, param, pszTransifSimParam, "s");
    PRINT_ELEM(file, param, bTransifUseTimedModel, "d");
    PRINT_ELEM(file, param, bTransifDebug, "d");
    PRINT_ELEM(file, param, pszSimIP, "s");
    PRINT_ELEM(file, param, uiSimPort, "d");
    PRINT_ELEM(file, param, bEnableRDW, "d");
    PRINT_ELEM(file, param, uiCPUPageSize, "u");

    PRINT_ELEM(file, param, bParallel, "d");
    PRINT_ELEM(file, param, bIgnoreDPF, "d");
    PRINT_ELEM(file, param, uiUseMMU, "u");
    for (i = 0; i < 2; i++)
    {
        PRINT_ELEM(file, param, aEDGPLL[i], "u");
    }
    PRINT_ELEM(file, param, bNowrite, "d");
    PRINT_ELEM(file, param, uiRegisterTest, "d");
    PRINT_ELEM(file, param, bPrintHWInfo, "d");
    PRINT_ELEM(file, param, uiTilingScheme, "u");
    PRINT_ELEM(file, param, uiTilingStride, "u");
    PRINT_ELEM(file, param, bWriteDGFrame, "d");
    PRINT_ELEM(file, param, ui8DataExtraction, "u");
    for ( i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        fprintf(file, "   sDataGen %d [0x%lx]\n", i, GET_OFFSET(param,
            sDataGen));
        printDG_Param(&(param->sDataGen[i]), file);
    }
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        fprintf(file, "   sCapture %d [0x%lx]\n", i, GET_OFFSET(param,
            sCapture));
        printCapture_Param(&(param->sCapture[i]), file);
    }
    for ( i = 0 ; i < CI_N_IIF_DATAGEN ; i++ )
    {
        fprintf(file, "   sIIFDG %d [0x%lx]\n", i, GET_OFFSET(param, sIIFDG));
        printCapture_Param(&(param->sCapture[i]), file);
    }
}

void init_Param(struct CMD_PARAM *param)
{
    int i,j;
#if defined(AWS_LINE_SEG_AVAILABLE)
    MC_AWS sAWS;
    MC_IIF sIIF;
    // fake a size - we only care about the line segment part of the AWS
    sIIF.ui16ImagerSize[0] = 30;
    sIIF.ui16ImagerSize[1] = 30;

    MC_AWSInit(&sAWS, &sIIF);
#endif
    // setup any default states in the variables which will be handling
    // commandline parameters
    IMG_MEMSET(param, 0, sizeof(struct CMD_PARAM));

    // global
    param->uiTilingScheme = 256;
    //param->uiTilingStride = 0;
    // otherwise value is not used and CI_DEF_GMACURVE is not defined
#ifdef FELIX_FAKE 
    param->uiGammaCurve = CI_DEF_GMACURVE;
#endif
    param->pszTransifLib = NULL;
    param->pszSimIP = NULL; // use default
    //param->uiSimPort = 0; // invalid port: use default
    param->bEnableRDW = IMG_FALSE;
    //param->uiRegisterTest = 0;
    param->bPrintHWInfo = IMG_FALSE;
    param->aEncOutSelect[0] = CI_N_CONTEXT;
    param->aEncOutSelect[1] = 1;
    param->uiUseMMU = 2;
    for (i = 0; i < 2; i++)
    {
        param->aEDGPLL[i] = 0;  // so that driver computes default
    }
    param->uiCPUPageSize = 4;
    param->ui8DataExtraction = 1;

    // capture
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
#ifndef FELIX_FAKE
        param->sCapture[i].bEnableTimestamps = IMG_TRUE;
#endif // otherwise false

        param->sCapture[i].ui8imager = i;
        param->sCapture[i].bUpdateMaxSize = IMG_FALSE;
        param->sCapture[i].bAllocateLate = IMG_FALSE;
        param->sCapture[i].ui32ConfigBuffers = 1;
        param->sCapture[i].i32RunForFrames = -1; // run for all associated DG frames
        // all strides default to 0 so that driver computes them
        // all offsets default to 0
        param->sCapture[i].escPitch[0] = 1.0;
        param->sCapture[i].escPitch[1] = 1.0;
        param->sCapture[i].dscPitch[0] = 1.0;
        param->sCapture[i].dscPitch[1] = 1.0;
        param->sCapture[i].bRescaleHDRIns = IMG_TRUE;
        // others is 0 or NULL or IMG_FALSE
        param->sCapture[i].uiSysBlack = IMG_MIN_INT(64<<SYS_BLACK_LEVEL_FRAC,
            (1<<(SYS_BLACK_LEVEL_FRAC+SYS_BLACK_LEVEL_INT))-1);
        param->sCapture[i].iUseDeshading = 0;
        param->sCapture[i].uiLshTile = LSH_TILE_MIN;
        for ( j = 0 ; j < 2*LSH_GRADS_NO ; j++ )
        {
            param->sCapture[i].lshGradients[j] = 0.0f;
        }
        for ( j = 0 ; j < LSH_GRADS_NO*LSH_N_CORNERS ; j++ )
        {
            param->sCapture[i].lshCorners[j] = 1.0f;
        }
        for ( j = 0 ; j < RLT_SLICE_N ; j++ )
        {
            param->sCapture[i].aRLTSlopes[j] = 1.0f;
        }
        for ( j = 0 ; j < RLT_SLICE_N-1 ; j++ )
        {
            IMG_UINT32 multiplier = 
                (1<<(RLT_LUT_0_POINTS_ODD_1_TO_15_INT
                +RLT_LUT_0_POINTS_ODD_1_TO_15_FRAC));
            param->sCapture[i].aRLTKnee[j] = (j+1)*(multiplier/RLT_SLICE_N);
        }
#if defined(AWS_LINE_SEG_AVAILABLE)
        for (j = 0; j < AWS_LINE_SEG_N; j++)
        {
            param->sCapture[i].aCurveCoeffX[j] = (float)sAWS.aCurveCoeffX[j];
            param->sCapture[i].aCurveCoeffY[j] = (float)sAWS.aCurveCoeffY[j];
            param->sCapture[i].aCurveOffset[j] = (float)sAWS.aCurveOffset[j];
            param->sCapture[i].aCurveBoundary[j] =
                (float)sAWS.aCurveBoundary[j];
        }
#endif
        param->sCapture[i].uiENSNLines = ENS_NLINES_MIN;
        param->sCapture[i].uiRaw2DBitDepth = 10;
        param->sCapture[i].iIIFDG = -1; // not using internal DG
    }

    // data generator
    for ( i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        //param->sDataGen[i].bUsesMIPI = IMG_TRUE;
        param->sDataGen[i].iFrames = -1;
        param->sDataGen[i].aBlanking[0] = FELIX_MIN_H_BLANKING;
        param->sDataGen[i].aBlanking[1] = FELIX_MIN_V_BLANKING;
        // others are 0 or NULL or IMG_FALSE
        param->sDataGen[i].uiMipiLanes = 1;
    }

    // internal data generator
    for ( i = 0 ; i < CI_N_IIF_DATAGEN ; i++ )
    {
        param->sIIFDG[i].iFrames = -1;
        param->sIIFDG[i].uiGasket = i;
        param->sIIFDG[i].aBlanking[0] = FELIX_MIN_H_BLANKING;
        param->sIIFDG[i].aBlanking[1] = FELIX_MIN_V_BLANKING;
    }
}

void dumpParameters(const struct CMD_PARAM *parameters, 
    const char *pszFilename)
{
    FILE *file = NULL;

    printf("INFO: dumping parameters into '%s'\n", pszFilename);
    if ( (file = fopen(pszFilename, "w")) == NULL )
    {
        LOG_ERROR("failed to dump the parameters\n");
        return;
    }

    printCMD_Param(parameters, file);

    fclose(file);
}

void printConnection(const CI_CONNECTION *pConn, FILE *file)
{
    const CI_HWINFO *pHWInfo = &pConn->sHWInfo;
    const CI_LINESTORE *pLine = &(pConn->sLinestoreStatus);
    int i;

    fprintf(file, "HWInfo: version %d.%d.%d", pHWInfo->rev_ui8Major,
        pHWInfo->rev_ui8Minor, pHWInfo->rev_ui8Maint);
    fprintf(file, "Linestore info:\n");
    for ( i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        fprintf(file, "   CT%d active=%d start=%d size=%d\n", i,
            pLine->aActive[i], pLine->aStart[i], pLine->aSize[i]);
    }
}

void clean_Param(struct CMD_PARAM *param)
{
    int i;
#ifdef FELIX_FAKE
    if (param->pszSimIP != NULL)
    {
        IMG_FREE(param->pszSimIP);
        param->pszSimIP = NULL;
    }
    if (param->pszTransifLib != NULL)
    {
        IMG_FREE(param->pszTransifLib);
        param->pszTransifLib = NULL;
    }
    if (param->pszTransifSimParam != NULL)
    {
        IMG_FREE(param->pszTransifSimParam);
        param->pszTransifSimParam = NULL;
    }
#endif /* FELIX_FAKE */

    for (i = 0 ; i < CI_N_CONTEXT ; i++)
    {
        if (param->sCapture[i].pszEncoderOutput != NULL)
        {
            IMG_FREE(param->sCapture[i].pszEncoderOutput);
            param->sCapture[i].pszEncoderOutput = NULL;
        }
        if (param->sCapture[i].pszDisplayOutput != NULL)
        {
            IMG_FREE(param->sCapture[i].pszDisplayOutput);
            param->sCapture[i].pszDisplayOutput = NULL;
        }
        if (param->sCapture[i].pszDataExtOutput != NULL)
        {
            IMG_FREE(param->sCapture[i].pszDataExtOutput);
            param->sCapture[i].pszDataExtOutput = NULL;
        }
        if (param->sCapture[i].pszHDRextOutput != NULL)
        {
            IMG_FREE(param->sCapture[i].pszHDRextOutput);
            param->sCapture[i].pszHDRextOutput = NULL;
        }
        if (param->sCapture[i].pszHDRInsInput != NULL)
        {
            IMG_FREE(param->sCapture[i].pszHDRInsInput);
            param->sCapture[i].pszHDRInsInput = NULL;
        }
        if (param->sCapture[i].pszRaw2ExtDOutput != NULL)
        {
            IMG_FREE(param->sCapture[i].pszRaw2ExtDOutput);
            param->sCapture[i].pszRaw2ExtDOutput = NULL;
        }
        if (param->sCapture[i].pszDPFRead != NULL)
        {
            IMG_FREE(param->sCapture[i].pszDPFRead);
            param->sCapture[i].pszDPFRead = NULL;
        }
    }
    for (i = 0 ; i < CI_N_EXT_DATAGEN ; i++)
    {
        if (param->sDataGen[i].pszInputFLX != NULL)
        {
          IMG_FREE(param->sDataGen[i].pszInputFLX);
          param->sDataGen[i].pszInputFLX = NULL;
        }
    }
    for (i = 0; i < CI_N_IIF_DATAGEN ; i++)
    {
        if (param->sIIFDG[i].pszInputFLX != NULL)
        {
            IMG_FREE(param->sIIFDG[i].pszInputFLX);
            param->sIIFDG[i].pszInputFLX = NULL;
        }
    }
}
