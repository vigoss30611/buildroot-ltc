/**
******************************************************************************
 @file info_helper.cpp

 @brief implementation of information gathering

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
#include "ispctest/info_helper.h"

#include "ci/ci_api.h"
#include "ci/ci_api_structs.h"

#define LOG_TAG "INFO_HELPER"
#include <felixcommon/userlog.h>


void PrintGasketInfo(ISPC::Camera &camera, std::ostream &os)
{
    CI_GASKET_INFO sGasketInfo;
    CI_CONNECTION *pConn = camera.getConnection();
    IMG_RESULT ret;
    IMG_UINT8 uiGasket = camera.getSensor()->uiImager;

    ret = CI_GasketGetInfo(&sGasketInfo, uiGasket, pConn);

    if (IMG_SUCCESS == ret)
    {
        os << "Gasket " << (unsigned)uiGasket << " ";
        if (sGasketInfo.eType&CI_GASKET_PARALLEL)
        {
            os << "is enabled (Parallel) - frame count "
                << sGasketInfo.ui32FrameCount << std::endl;
        }
        else if (sGasketInfo.eType&CI_GASKET_MIPI)
        {
            // to write hex because it's simpler than changing the stream bits
            char tmp[16];
            os << "is enabled (MIPI) - frame count "
                << sGasketInfo.ui32FrameCount << std::endl;

            os << "Gasket MIPI FIFO " << (int)sGasketInfo.ui8MipiFifoFull
                << " - Enabled lanes "
                << (int)sGasketInfo.ui8MipiEnabledLanes << std::endl;

            sprintf(tmp, "0x%x", (int)sGasketInfo.ui8MipiCrcError);
            os << "Gasket MIPI CRC Error " << tmp << std::endl;
            sprintf(tmp, "0x%x", (int)sGasketInfo.ui8MipiHdrError);
            os << "Gasket MIPI Header Error " << tmp << std::endl;
            sprintf(tmp, "0x%x", (int)sGasketInfo.ui8MipiEccError);
            os << "Gasket MIPI ECC Error " << tmp << std::endl;
            sprintf(tmp, "0x%x", (int)sGasketInfo.ui8MipiEccCorrected);
            os << "Gasket MIPI ECC Correted " << tmp << std::endl;
        }
        else
        {
            os << "is disabled" << std::endl;
        }
    }
    else
    {
        LOG_ERROR("failed to get information about gasket %u\n",
            (int)uiGasket);
    }
}

void PrintRTMInfo(ISPC::Camera &camera, std::ostream &os)
{
    CI_RTM_INFO sRTMInfo;
    CI_CONNECTION *pConn = camera.getConnection();
    IMG_RESULT ret;
    IMG_UINT8 c, i;

    ret = CI_DriverGetRTMInfo(pConn, &sRTMInfo);

    if (IMG_SUCCESS == ret)
    {
        // to write hex because it's simpler than changing the stream bits
        char tmp[16];
        for (i = 0; i < pConn->sHWInfo.config_ui8NRTMRegisters; i++)
        {
            sprintf(tmp, "0x%x", sRTMInfo.aRTMEntries[i]);
            os << "RTM Core " << (int)i << " " << tmp << std::endl;
        }
        for (c = 0; c < pConn->sHWInfo.config_ui8NContexts; c++)
        {
            sprintf(tmp, "0x%x", sRTMInfo.context_status[c]);
            os << "RTM Context " << (int)c << " status " << tmp << std::endl;
            sprintf(tmp, "0x%x", sRTMInfo.context_linkEmptyness[c]);
            os << "RTM Context " << (int)c << " linked list emptiness "
                << tmp << std::endl;
            for (i = 0; i < pConn->sHWInfo.config_ui8NRTMRegisters; i++)
            {
                sprintf(tmp, "0x%x", sRTMInfo.context_position[c][i]);
                os << "RTM Context " << (int)c << " position " << (int)i
                    << " " << tmp << std::endl;
            }
        }
    }
    else
    {
        LOG_ERROR("failed to get RTM information\n");
    }
}

