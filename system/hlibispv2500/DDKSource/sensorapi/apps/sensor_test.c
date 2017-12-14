/**
******************************************************************************
@file sensor_test.c

@brief Start and run a sensor (not data-generators)

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

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include <img_errors.h>

#include <dyncmd/commandline.h>

#include <sensorapi/sensorapi.h>
#include <ci/ci_api.h> // to get gasket info

#include <felixcommon/userlog.h>
#define LOG_TAG "sensor_test"

// include this if special AR330 modes need to be supported
#include <sensors/ar330.h>
//#define SPECIAL_AR330

static void printInfo(const SENSOR_INFO *info)
{
    printf("Sensor %s mode %u (%dx%d @%.2lf) %ubit\n",
        info->pszSensorName, info->sStatus.ui16CurrentMode,
        (int)info->sMode.ui16Width, (int)info->sMode.ui16Height,
        info->sMode.flFrameRate, (unsigned)info->sMode.ui8BitDepth);
    printf("   imager %u exposure (%u..%u) v-total %d mipi-lanes %u\n",
        (unsigned)info->ui8Imager,
        info->sMode.ui32ExposureMin, info->sMode.ui32ExposureMax,
        info->sMode.ui16VerticalTotal,
        (unsigned)info->sMode.ui8MipiLanes);
}

static void printGasketInfo(CI_CONNECTION *pConn, unsigned int uiGasket)
{
    CI_GASKET_INFO sGasketInfo;
    IMG_RESULT ret;
    
    ret = CI_GasketGetInfo(&sGasketInfo, uiGasket, pConn);

    if (IMG_SUCCESS == ret)
    {
        printf("*****\n");
        printf("Gasket %u\n", (unsigned)uiGasket);
        if (pConn->sHWInfo.gasket_aType[uiGasket]&CI_GASKET_PARALLEL)
        {
            printf("%s (Parallel) - frame count %u\n",
                sGasketInfo.eType&CI_GASKET_PARALLEL ? "enabled" : "disabled",
                sGasketInfo.ui32FrameCount);
        }
        else if (pConn->sHWInfo.gasket_aType[uiGasket] & CI_GASKET_MIPI)
        {
            printf("%s (MIPI) - frame count %u\n",
                sGasketInfo.eType&CI_GASKET_MIPI ? "enabled" : "disabled",
                sGasketInfo.ui32FrameCount);
            printf("Gasket MIPI FIFO %u - Enabled lanes %u\n",
                (unsigned)sGasketInfo.ui8MipiFifoFull,
                (unsigned)sGasketInfo.ui8MipiEnabledLanes);
            printf("Gasket CRC Error 0x%x\n"\
                "       HDR Error 0x%x\n"\
                "       ECC Error 0x%x\n"\
                "       ECC Correted 0x%x\n",
                (int)sGasketInfo.ui8MipiCrcError,
                (int)sGasketInfo.ui8MipiHdrError,
                (int)sGasketInfo.ui8MipiEccError,
                (int)sGasketInfo.ui8MipiEccCorrected);
        }
        printf("*****\n");
    }
    else
    {
        LOG_ERROR("failed to get information about gasket %u\n",
            (int)uiGasket);
    }
}

/**
 * @brief Enable mode to allow proper key capture in the application
 */
void enableRawMode(struct termios *orig_term_attr)
{
    struct termios new_term_attr;

    LOG_DEBUG("Enable Console RAW mode\n");

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), orig_term_attr);
    memcpy(&new_term_attr, orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO | ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;

    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
}

/**
* @brief Disable mode to allow proper key capture in the application
*/
void disableRawMode(struct termios *orig_term_attr)
{
    LOG_DEBUG("Disable Console RAW mode\n");
    tcsetattr(fileno(stdin), TCSANOW, orig_term_attr);
}

/**
 * @brief read key without blocking
 *
 * Could use fgetc but does not work with some versions of uclibc
 */
char getch(void)
{
    static char line[2];
    if (read(0, line, 1))
    {
        return line[0];
    }
    return -1;
}

static char *pszSensor = NULL;
static SENSOR_HANDLE sensor = NULL;
static CI_CONNECTION *pConn = NULL;
struct termios term_attr;
#ifdef SPECIAL_AR330
/* load registers from there */
static char *specialAR330Registers = NULL;
#endif

void cleanHeap()
{
    if (pszSensor) free(pszSensor);
#ifdef SPECIAL_AR330
    if (specialAR330Registers) free(specialAR330Registers);
#endif
    if (sensor) Sensor_Destroy(sensor);
    if (pConn) CI_DriverFinalise(pConn);
    disableRawMode(&term_attr);
}

void ctrlcHandler(sig_atomic_t sig)
{
    cleanHeap();
    exit(1);
}

int main(int argc, char *argv[])
{
    IMG_UINT uiSensorMode = 0;
    int ret;
    /*  missing parameters - if != 0 at the end of loading exit */
    int missing = 0;
    int unregistered;

    int continue_running = 1;
    IMG_UINT usleeptime = 100000;
    IMG_BOOL8 bPrintHelp = IMG_FALSE;

    enableRawMode(&term_attr);
    
    //
    //command line parameter loading:
    //

    ret = DYNCMD_AddCommandLine(argc, argv, "-source");
    if (IMG_SUCCESS != ret)
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
        bPrintHelp = IMG_TRUE;
    }

    ret = DYNCMD_RegisterParameter("-sensor", DYNCMDTYPE_STRING,
        "Sensor to use",
        &(pszSensor));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        pszSensor = NULL;
        LOG_ERROR("-sensor option not found!\n");
        missing++;
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

    ret = DYNCMD_RegisterParameter("-usleeptime", DYNCMDTYPE_UINT,
        "[optional] time to sleep in us between two prints of the gasket status",
        &(usleeptime));
    if (RET_FOUND != ret && !bPrintHelp)
    {
        if (RET_NOT_FOUND != ret) // if not found use default
        {
            LOG_ERROR("while parsing parameter -usleeptime\n");
            missing++;
        }
        // parameter is optional
    }
    
#ifdef SPECIAL_AR330
    ret = DYNCMD_RegisterParameter("-ar330_registers", DYNCMDTYPE_STRING,
        "[optional] if using AR330 and specified will override the mode to "\
        "be the special mode and load the registers from this file",
        &(specialAR330Registers));
    if (!bPrintHelp)
    {
        if (RET_FOUND != ret)
        {
            specialAR330Registers = NULL;
            if (RET_NOT_FOUND != ret)
            {
                LOG_ERROR("expecting a string after -ar330_registers\n");
                missing++;
            }
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
        return EXIT_FAILURE;
    }

    {
        //
        // Create a sensor
        //
        int sensorId = 0;
        SENSOR_INFO info;

        signal(SIGINT, ctrlcHandler);

        ret = CI_DriverInit(&pConn);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to connect to ISP driver\n");
            cleanHeap();
            return EXIT_FAILURE;
        }

        // search sensor ID from name
        {
            const int nSensors = Sensor_NumberSensors();
            const char ** sensorNames = Sensor_ListAll();

            for (sensorId = 0; sensorId < nSensors; sensorId++)
            {
                if (0 == strncmp(pszSensor, sensorNames[sensorId],
                    strlen(sensorNames[sensorId])))
                {
                    LOG_DEBUG("Found sensor Id %d for sensor %s\n",
                        sensorId, pszSensor);
                    break;
                }
            }
            if (nSensors == sensorId)
            {
                LOG_ERROR("Failed to find sensor Id for sensor %s\n",
                    pszSensor);
                cleanHeap();
                return EXIT_FAILURE;
            }
        }
#ifdef INFOTM_ISP
        ret = Sensor_Initialise(sensorId, &sensor, 0);
#else
        ret = Sensor_Initialise(sensorId, &sensor);
#endif
        if (IMG_SUCCESS != ret || !sensor)
        {
            LOG_ERROR("Failed to initialise sensor Id %d:s\n",
                sensorId, pszSensor);
            cleanHeap();
            return EXIT_FAILURE;
        }
        
#ifdef SPECIAL_AR330
        if (specialAR330Registers)
        {
            IMG_ASSERT(uiSensorMode == AR330_SPECIAL_MODE);
            ret = AR330_LoadSpecialModeRegisters(sensor,
                specialAR330Registers);
            if (IMG_SUCCESS != ret)
            {
                LOG_ERROR("Failed to load '%s' as registers for %s\n",
                    specialAR330Registers, pszSensor);
                cleanHeap();
                return EXIT_FAILURE;
            }
        }
#endif

        ret = Sensor_SetMode(sensor, uiSensorMode, SENSOR_FLIP_NONE);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to set mode %u\n", uiSensorMode);
            cleanHeap();
            return EXIT_FAILURE;
        }

        ret = Sensor_GetInfo(sensor, &info);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to get sensor's information\n");
            cleanHeap();
            return EXIT_FAILURE;
        }

        printInfo(&info);

        printf("Before staring:\n");
        printGasketInfo(pConn, info.ui8Imager);

        ret = Sensor_Enable(sensor);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to enable sensor\n");
            cleanHeap();
            return EXIT_FAILURE;
        }

        while (continue_running)
        {
            char c = getch();

            printf("press x to stop\n");
            printInfo(&info);
            printGasketInfo(pConn, info.ui8Imager);

            if (c == 'x')
            {
                continue_running = 0;
            }

            usleep(usleeptime);
        }


        ret = Sensor_Disable(sensor);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("Failed to disable sensor\n");
            cleanHeap();
            return EXIT_FAILURE;
        }

        printf("After stopping:\n");
        printGasketInfo(pConn, info.ui8Imager);

        // sensor will be destroyed when returning
        // connection will be closed when returning
    }

    cleanHeap();
    return EXIT_SUCCESS;
}
