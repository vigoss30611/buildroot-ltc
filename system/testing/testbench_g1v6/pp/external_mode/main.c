/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : post-processor external mode testbench
--
------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pptestbench.h"
#include "ppapi.h"
#include "tb_cfg.h"

#if defined (PP_EVALUATION)
extern u32 gHwVer;
#endif

TBCfg tbCfg;

extern u32 forceTiledInput;
    
int main(int argc, char **argv)
{
    int ret = 0, frame, i = 1;
    char *cfgFile = "pp.cfg";
    FILE *fTBCfg;

#if defined(PP_EVALUATION_8170)
    gHwVer = 8170;
#elif defined(PP_EVALUATION_8190)
    gHwVer = 8190;
#elif defined(PP_EVALUATION_9170)
    gHwVer = 9170;
#elif defined(PP_EVALUATION_9190)
    gHwVer = 9190;
#elif defined(PP_EVALUATION_G1)
    gHwVer = 10000;
#endif

#ifndef EXPIRY_DATE
#define EXPIRY_DATE (u32)0xFFFFFFFF
#endif /* EXPIRY_DATE */

    /* expiry stuff */
    {
        u8 tmBuf[7];
        time_t sysTime;
        struct tm * tm;
        u32 tmp1;

        /* Check expiry date */
        time(&sysTime);
        tm = localtime(&sysTime);
        strftime(tmBuf, sizeof(tmBuf), "%y%m%d", tm);
        tmp1 = 1000000+atoi(tmBuf);
        if (tmp1 > (EXPIRY_DATE) && (EXPIRY_DATE) > 1 )
        {
            fprintf(stderr,
                "EVALUATION PERIOD EXPIRED.\n"
                "Please contact On2 Sales.\n");
            return -1;
        }
    }

    {
        PPApiVersion ppApi;
        PPBuild ppBuild;

        /* Print API version number */
        ppApi = PPGetAPIVersion();
        ppBuild = PPGetBuild();

        printf("\nX170 PostProcessor API v%d.%d - SW build: %d\n",
                     ppApi.major, ppApi.minor, ppBuild.swBuild);
        printf("X170 ASIC build: %x\n\n", ppBuild.hwBuild);

    }


    /* Parse cmdline arguments */
    if(argc == 1)
    {
        printf("Usage: %s [<config-file>]\n", argv[0]);
        return 1;
    }

    for (i = 1; i < (u32)(argc-1); i++)
    {
        if (strncmp(argv[i], "-E", 2) == 0)
            forceTiledInput = 1;
        else
        {
            printf("Unknown option: %s\n", argv[i]);
            return 2;
        }
    }

    cfgFile = argv[argc-1];
    
    /* set test bench configuration */
    TBSetDefaultCfg(&tbCfg);
    fTBCfg = fopen("tb.cfg", "r");
    if (fTBCfg == NULL)
    {
        printf("UNABLE TO OPEN INPUT FILE: \"tb.cfg\"\n");
        printf("USING DEFAULT CONFIGURATION\n");
    }
    else
    {
        fclose(fTBCfg);
        if (TBParseConfig("tb.cfg", TBReadParam, &tbCfg) == TB_FALSE)
            return -1;
        if (TBCheckCfg(&tbCfg) != 0)
            return -1;
    }
    /*TBPrintCfg(&tbCfg);*/

    ret = pp_external_run(cfgFile, &tbCfg);

    return ret;
}
