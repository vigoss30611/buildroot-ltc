/**
******************************************************************************
 @file felix.cpp

 @brief Unit test helpers implementation

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
#include "felix.h"
#include "unit_tests.h"

IMG_UINT32 g_context_max_width = CONTEXT_MAX_WIDTH_DEF;

void InitTalReset();
// this is used to setup the TAL default values when using the NULL interface
void (*gpfnInitTal)(void) = &InitTalReset;
IMG_BOOL8 gbUseTalNULL = IMG_FALSE;

#include "ci/ci_api.h"
#include "ci_kernel/ci_kernel.h"
#include "ci/ci_api_internal.h"
#include "felixcommon/lshgrid.h"
#include "mc/module_config.h"
#include "mmulib/heap.h"
#include "mmulib/mmu.h"

#include <cstdio>
#include <cmath>

#include <tal.h>
#include <target.h>
#include <reg_io2.h>
#include <registers/core.h>
#include <registers/context0.h>
#include <registers/mmu.h>
#include <registers/data_generator.h>
#ifdef FELIX_HAS_DG
#include <dg/dg_api.h>
#include <dg_kernel/dg_camera.h>
#include <registers/ext_data_generator.h>
#endif

#ifdef WIN32
#define log2(n) (log10(n) / log10(2.0))
#endif

static bool gTalWasInit = false;

TAL_HANDLES::TAL_HANDLES()
{
    int i;
    char name[64];

    driver.hRegFelixCore = TAL_GetMemSpaceHandle("REG_FELIX_CORE");
    hRegMMU = TAL_GetMemSpaceHandle("REG_FELIX_MMU");
    //driver.hRegFelixI2C = NULL;
    driver.hRegGammaLut = TAL_GetMemSpaceHandle("REG_FELIX_GMA_LUT");
    driver.hMemHandle = NULL;

    for (i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        snprintf(name, sizeof(name), "REG_FELIX_CONTEXT_%d", i);
        driver.hRegFelixContext[i] = TAL_GetMemSpaceHandle(name);
    }
    for (i = 0; i < CI_N_IMAGERS; i++)
    {
        snprintf(name, sizeof(name), "REG_FELIX_GASKET_%d", i);
        driver.hRegGaskets[i] = TAL_GetMemSpaceHandle(name);
    }
    for (i = 0; i < CI_N_IIF_DATAGEN; i++)
    {
        snprintf(name, sizeof(name), "REG_FELIX_DG_IIF_%d", i);
        driver.hRegIIFDataGen[i] = TAL_GetMemSpaceHandle(name);
    }

    datagen.hMemHandle = NULL;
    datagen.hRegMMU = TAL_GetMemSpaceHandle("REG_TEST_MMU");
    for (i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        snprintf(name, sizeof(name), "REG_TEST_DG_%d", i);
        datagen.hRegFelixDG[i] = TAL_GetMemSpaceHandle(name);
    }
}

void InitTalReset()
{
    IMG_UINT32 value;
    TAL_HANDLES talHandles;

    CI_INFO ("setting up register reset values\n");

    /*
     * Felix Core configuration
     */

    /*deviceCore = TAL_GetMemSpaceHandle("REG_FELIX_CORE");
    context[0] = TAL_GetMemSpaceHandle("REG_FELIX_CONTEXT_0");

    for ( int i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        char name[64];

        sprintf(name, "REG_FELIX_CONTEXT_%d", i);
        context[i] = TAL_GetMemSpaceHandle(name);
    }
    for ( int i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        char name[64];

        sprintf(name, "REG_FELIX_TEST_DG_%d", i);
        datagen[i] = TAL_GetMemSpaceHandle(name);
    }*/


#if FELIX_VERSION_MAJ == 1
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, GROUP_ID, CORE_GROUP_RESET);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, ALLOCATION_ID, CORE_ALLOCATION_RESET);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, FELIX_BIT_DEPTH, CORE_BIT_DEPTH-1); // register stores the size -1
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, FELIX_PARALLELISM, CORE_PARALLELISM-1);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, FELIX_NUM_IMAGERS, CORE_NIMAGER-1);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, FELIX_NUM_CONTEXT, CI_N_CONTEXT-1);
    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_ID_OFFSET, value);
#else
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, GROUP_ID, CORE_GROUP_RESET);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID, ALLOCATION_ID, CORE_ALLOCATION_RESET);
    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_ID_OFFSET, value);

    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID2, FELIX_BIT_DEPTH, CORE_BIT_DEPTH-1); // register stores the size -1
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID2, FELIX_PARALLELISM, CORE_PARALLELISM-1);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID2, FELIX_NUM_IMAGERS, CORE_NIMAGER-1);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_ID2, FELIX_NUM_CONTEXT, CI_N_CONTEXT-1);
    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_ID2_OFFSET, value);
#endif

    // revision
    value = 0;
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_REVISION, CORE_DESIGNER, FELIX_DESIGNER);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_REVISION, CORE_MAJOR_REV, FELIX_VERSION_MAJ);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_REVISION, CORE_MINOR_REV, FELIX_VERSION_MIN);
    REGIO_WRITE_FIELD(value, FELIX_CORE, CORE_REVISION, CORE_MAINT_REV, FELIX_VERSION_MNT);
    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_CORE_REVISION_OFFSET, value);

    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_DESIGNER_REV_FIELD1_OFFSET, CORE_CONFIG1_RESET);
    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_DESIGNER_REV_FIELD2_OFFSET, CORE_CONFIG2_RESET);
    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_DESIGNER_REV_FIELD3_OFFSET, CORE_CONFIG3_RESET);
    TALREG_WriteWord32 (talHandles.driver.hRegFelixCore, FELIX_CORE_DESIGNER_REV_FIELD4_OFFSET, CORE_CONFIG4_RESET);

    value = 0;
    // encoder scaler info
    REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD3, ENC_SCALER_H_LUMA_TAPS, ESC_H_LUMA_TAPS-1);
    REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD3, ENC_SCALER_V_LUMA_TAPS, ESC_V_LUMA_TAPS-1);
    // ESC_H_CHORMA_TAPS
    REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD3, ENC_SCALER_V_CHROMA_TAPS, ESC_V_CHROMA_TAPS-1);
    // display scaler info
    REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD3, DISP_SCALER_H_LUMA_TAPS, DSC_H_LUMA_TAPS-1);
    REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD3, DISP_SCALER_V_LUMA_TAPS, DSC_V_LUMA_TAPS-1);
    // DSC_H_CHROMA_TAPS
    // DSC_V_CHROMA_TAPS
    TALREG_WriteWord32(talHandles.driver.hRegFelixCore, FELIX_CORE_DESIGNER_REV_FIELD3_OFFSET, value);

    for ( int i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        IMG_UINT32 line = g_context_max_width;
        if ( i > 0 ) line >>= i;

#if FELIX_VERSION_MAJ == 1
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONFIG_2, MAX_CONTEXT_WIDTH, line-1); // max line store value is stored in ctx 0
        TALREG_WriteWord32(talHandles.driver.hRegFelixContext[i], FELIX_CONTEXT0_CONTEXT_CONFIG_2_OFFSET, value);
#else
        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONFIG, MAX_CONTEXT_WIDTH_SGL, line-1);
        // register def says -1 but it is -9 in practice
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONFIG, CONTEXT_BIT_DEPTH, CORE_BIT_DEPTH-9);
        TALREG_WriteWord32(talHandles.driver.hRegFelixContext[i], FELIX_CONTEXT0_CONTEXT_CONFIG_OFFSET, value);

        if(i==0)
        {
            line -= line/4;
            // when multiple are enabled only context 0 can be max size
            // as in HW config
        }

        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_CONFIG_2, MAX_CONTEXT_WIDTH_MULT, line-1); // max line store value is stored in ctx 0
        TALREG_WriteWord32(talHandles.driver.hRegFelixContext[i], FELIX_CONTEXT0_CONTEXT_CONFIG_2_OFFSET, value);
#endif

        value = 0;
        REGIO_WRITE_FIELD(value, FELIX_CONTEXT0, CONTEXT_LINK_EMPTYNESS, CONTEXT_LINK_EMPTYNESS, CONTEXT_MAX_QUEUE);
        TALREG_WriteWord32(talHandles.driver.hRegFelixContext[i], FELIX_CONTEXT0_CONTEXT_LINK_EMPTYNESS_OFFSET, value);
    }

#if FELIX_VERSION_MAJ == 1

    for ( int i = 0 ; i < CI_N_IMAGERS ; i++ )
    {
        value = 0;
        // IMGR_PORT_WIDTH
        REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD4, GASKET_BIT_DEPTH, CORE_GASKETBITDEPTH);
        if ( i%CORE_N_DIFF_GASKETS == 0 )
        {
            REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD4, GASKET_TYPE, CORE_GASKETTYPE_0);
        }
        else
        {
            REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD4, GASKET_TYPE, CORE_GASKETTYPE_1);
        }
        // GASKET_MAX_FRAME_WIDTH
        TALREG_WriteWord32(talHandles.driver.hRegFelixCore, REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD4, i), value);
    }
#else
    value = g_context_max_width;
    TALREG_WriteWord32(talHandles.driver.hRegFelixCore, FELIX_CORE_MAX_ACTIVE_WIDTH_OFFSET, value);

    for ( int i = 0 ; i < CI_N_IMAGERS ; i++ )
    {
        value = 0;
        // IMGR_PORT_WIDTH
        REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD5, GASKET_BIT_DEPTH, CORE_GASKETBITDEPTH);
        if ( i%CORE_N_DIFF_GASKETS == 0 )
        {
            REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD5, GASKET_TYPE, CORE_GASKETTYPE_0);
        }
        else
        {
            REGIO_WRITE_FIELD(value, FELIX_CORE, DESIGNER_REV_FIELD5, GASKET_TYPE, CORE_GASKETTYPE_1);
        }
        // GASKET_MAX_FRAME_WIDTH
        TALREG_WriteWord32(talHandles.driver.hRegFelixCore, REGIO_TABLE_OFF(FELIX_CORE, DESIGNER_REV_FIELD5, i), value);
    }
#endif // FELIX_VERSION == 1

    /*
     * other registers:
     DESIGNER
     INTERRUPT_STATUS
     INTERRUPT_ENABLE
     INTERRUPT_CLEAR
     FELIX_CLOCK_CONTROL
     CORE_RESET
     FELIX_STATUS
     DE_CTRL // data extraction
     DI_CTRL // data insertion
     DI_SIZE
     DI_PIXEL_RATE
     DI_LINK_ADDR
     DI_TAG
     DI_ADDR_Y
     DI_ADDR_Y_STRIDE
     DI_ADDR_C
     DI_ADDR_C_STRIDE
     */

    // MMU setup
    value = 0;
    REGIO_WRITE_FIELD(value, IMG_VIDEO_BUS4_MMU, MMU_CONFIG0, EXTENDED_ADDR_RANGE, MMU_BITDEPTH);
    REGIO_WRITE_FIELD(value, IMG_VIDEO_BUS4_MMU, MMU_CONFIG0, MMU_SUPPORTED, MMU_ENABLED);
    TALREG_WriteWord32(talHandles.hRegMMU, IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_OFFSET, value);

#ifdef FELIX_HAS_DG
    TALREG_WriteWord32(talHandles.datagen.hRegMMU, IMG_VIDEO_BUS4_MMU_MMU_CONFIG0_OFFSET, value);
    for ( int i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        TALREG_WriteWord32(talHandles.datagen.hRegFelixDG[i], FELIX_TEST_DG_DG_CONFIG_OFFSET, CI_N_EXT_DATAGEN);
    }
#endif

#if FELIX_VERSION_MAJ > 1
    for (int i = 0; i < CI_N_IIF_DATAGEN; i++)
    {
        TALREG_WriteWord32(talHandles.driver.hRegIIFDataGen[i],
            FELIX_DG_IIF_DG_CONFIG_OFFSET, FELIX_DG_IIF_DG_CONFIG_DG_ENABLE_STATUS_MASK);
    }
#endif

    gTalWasInit = true;
}

Felix::Felix()
{
  pKrnConnection = NULL;
  pUserConnection = NULL;
}

IMG_RESULT Felix::configure(enum FelixRegInit regInit, IMG_BOOL8 bMMUEnabled)
{
    IMG_RESULT ret;

    if ( g_psCIDriver != NULL )
    {
        KRN_CI_DriverDestroy(g_psCIDriver);
        SYS_DevDeregister(&sDevice);
        CI_FATAL("driver already initiliased! Previous test must have failed without deinitialising!");
        return IMG_ERROR_ALREADY_INITIALISED;
    }

    gTalWasInit = false;
    sDevice.probeSuccess = NULL;
    SYS_DevRegister(&sDevice);

#ifdef IMG_MALLOC_CHECK
    printf ("IMG:alloc %u - free %u\n", gui32Alloc, gui32Free);
#endif

    /*
     * in order to allow the setup of the default register the unit test need to init the tal first
     */

    switch(regInit)
    {
    case DEFAULT_VAL:
        gpfnInitTal = &InitTalReset;
        gbUseTalNULL = IMG_TRUE;
        ret = KRN_CI_DriverCreate(&sCIDriver, bMMUEnabled, 256, 0, CI_DEF_GMACURVE, &sDevice);
        break;

    case NONE:
    default:
        gpfnInitTal = NULL;
        ret = KRN_CI_DriverCreate(&sCIDriver, bMMUEnabled, 256, 0, CI_DEF_GMACURVE, &sDevice);
    }
    // driver already init the way we want, we can call normal CI function

    if ( ret != IMG_SUCCESS )
    {
        CI_FATAL("failed to init the driver\n");
        return ret;
    }

    // if talinit != NULL and the gTalWasInit is still false
    if ( (gpfnInitTal != NULL) != gTalWasInit )
    {
        CI_FATAL("the TAL init function was not called when initialising the driver\n");
        return IMG_ERROR_FATAL;
    }

    if ( (ret=CI_DriverInit(&(this->pUserConnection))) != IMG_SUCCESS )
    {
        CI_FATAL("failed to get the connection handle\n");
        return ret;
    }

    //this->pConnection = container_of(givenConnection, KRN_CI_CONNECTION*, publicConnection);
    {
        sCell_T *pCell = NULL;
        KRN_CI_CONNECTION *pConnection = NULL;

        assert(g_psCIDriver!=NULL);
        pCell = List_getHead(&(g_psCIDriver->sList_connection));
        assert(pCell!=NULL);
        this->pKrnConnection = (KRN_CI_CONNECTION*)pCell->object;
    }

    return IMG_SUCCESS;
}

Felix::~Felix()
{
  clean();
}

void Felix::clean()
{
    if (pUserConnection)
    {
        CI_DriverFinalise(pUserConnection);
        pUserConnection = NULL;
    }

  if (g_psCIDriver)
  {
    KRN_CI_DriverDestroy(&sCIDriver);
    SYS_DevDeregister(&sDevice);
#ifdef IMG_MALLOC_CHECK
    printf ("IMG:alloc %u - free %u\n", gui32Alloc, gui32Free);
#endif
  }
}

KRN_CI_DRIVER* Felix::getKrnDriver()
{
  return &sCIDriver;
}

CI_CONNECTION* Felix::getConnection()
{
    return this->pUserConnection;
}

KRN_CI_CONNECTION* Felix::getKrnConnection()
{
    return this->pKrnConnection;
}

IMG_RESULT Felix::fakeInterrupt(KRN_CI_PIPELINE* pKrnPipeline)
{
    sCell_T *pHead = NULL;
    KRN_CI_SHOT *pToBeShot = NULL;
    int ctx = pKrnPipeline->pipelineConfig.ui8Context;

    pToBeShot = KRN_CI_PipelineShotCompleted(pKrnPipeline);
    if ( pToBeShot != NULL )
    {
        pToBeShot->userShot.i32MissedFrames = 0; // we didn't miss a frame
        return IMG_SUCCESS;
    }
    return IMG_ERROR_FATAL;
}


FullFelix::FullFelix(): Felix()
{
    pPipeline = NULL;
}

FullFelix::~FullFelix()
{
    clean();
}

void FullFelix::clean()
{
    if (pPipeline)
    {
        // stops the pipelin if it is started
        CI_PipelineDestroy(pPipeline);
        pPipeline = NULL;
    }
    Felix::clean();
}

void FullFelix::configure(int width, int height, ePxlFormat yuvOut, ePxlFormat rgbOut, bool genCRC, int nBuffers, IMG_BOOL8 bMMU)
{
    IMG_RESULT ret = IMG_SUCCESS;

    Felix::configure(DEFAULT_VAL, bMMU);

    ret = CI_PipelineCreate(&pPipeline, pUserConnection);
    if (IMG_SUCCESS != ret)
    {
        FAIL() << "failed to init pPipeline (returned " << (Test_IMG_RESULT)ret << ")";
    }

    pPipeline->config.ui8Context = 0;
    pPipeline->sImagerInterface.ui8Imager = 0;
    pPipeline->sImagerInterface.eBayerFormat = MOSAIC_RGGB;

    pPipeline->sImagerInterface.ui16ImagerSize[0] = (width/CI_CFA_WIDTH)-1;
    pPipeline->sImagerInterface.ui16ImagerSize[1] = (height/CI_CFA_HEIGHT)-1;
    if ( yuvOut != PXL_NONE )
    {
        pPipeline->config.ui16MaxEncOutWidth = width;
        pPipeline->config.ui16MaxEncOutHeight = height;
        pPipeline->sEncoderScaler.aOutputSize[0] = (width-2)/2;
        pPipeline->sEncoderScaler.aOutputSize[1] = height-1;
    }
    else
    {
        pPipeline->sEncoderScaler.bBypassScaler = IMG_TRUE;
    }
    if ( rgbOut != PXL_NONE )
    {
        pPipeline->config.ui16MaxDispOutWidth = width;
        pPipeline->config.ui16MaxDispOutHeight = height;
        pPipeline->sDisplayScaler.aOutputSize[0] = (width-2)/2;
        pPipeline->sDisplayScaler.aOutputSize[1] = height-1;
    }
    else
    {
        pPipeline->sDisplayScaler.bBypassScaler = IMG_TRUE;
    }
    PixelTransformYUV(&pPipeline->config.eEncType, yuvOut);
    PixelTransformRGB(&pPipeline->config.eDispType, rgbOut);

    pPipeline->sEncoderScaler.bOutput422 =
        pPipeline->config.eEncType.ui8HSubsampling == 1;  // 422

    if ( genCRC )
    {
        pPipeline->config.eOutputConfig = (enum CI_SAVE_CONFIG_FLAGS )(pPipeline->config.eOutputConfig | CI_SAVE_CRC);
    }
    else
    {
        pPipeline->config.eOutputConfig = (enum CI_SAVE_CONFIG_FLAGS )(pPipeline->config.eOutputConfig & ~CI_SAVE_CRC);
    }

    // to be sure the minimum column size is met (although it should be computed from the number of columns)
    pPipeline->sToneMapping.localColWidth[0] = TNM_MIN_COL_WIDTH;

    pPipeline->stats.sExposureStats.ui16Width = EXS_MIN_WIDTH;
    pPipeline->stats.sExposureStats.ui16Height = EXS_MIN_HEIGHT;

    ret=CI_PipelineRegister(pPipeline);
    if (ret)
    {
        FAIL() << "failed to register the pConfig (returned " << (Test_IMG_RESULT)ret << ")";
    }

    ret=CI_PipelineAddPool(pPipeline, nBuffers);
    if (ret)
    {
        FAIL() << "failed to add pool to pConfig (returned " << (Test_IMG_RESULT)ret << ")";
    }
    for ( int b = 0 ; b < nBuffers ; b++ )
    {
        if ( yuvOut != PXL_NONE )
        {
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL);
            if (ret)
            {
                FAIL() <<"failed to allocate YUV buffer " << b+1 << "/" << nBuffers << " (returned " << (Test_IMG_RESULT)ret << ")";
                break;
            }
        }
        if ( rgbOut != PXL_NONE )
        {
            ret = CI_PipelineAllocateBuffer(pPipeline, CI_TYPE_DISPLAY, 0, IMG_FALSE, NULL);
            if (ret)
            {
                FAIL() <<"failed to allocate RGB buffer " << b+1 << "/" << nBuffers << " (returned " << (Test_IMG_RESULT)ret << ")";
                break;
            }
        }
    }

    // initialise gasket to 0
    writeGasketCount(pPipeline->sImagerInterface.ui8Imager, 0);
    // pPipeline is ready to be started
}

#include "ci_kernel/ci_hwstruct.h"
#include "reg_io2.h"
#if FELIX_VERSION_MAJ == 1
#include <registers/gaskets.h>
#else
#include <registers/gasket_mipi.h>
#include <registers/gasket_parallel.h>
#endif

void FullFelix::writeGasketCount(IMG_UINT32 ui32Gasket, IMG_UINT32 gasketCount)
{
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(ui32Gasket < CI_N_IMAGERS);

#if FELIX_VERSION_MAJ == 1
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[0],
        REGIO_TABLE_OFF(FELIX_GASKET, GASKETS_FRAME_CNT, ui32Gasket),
        gasketCount
    );
#elif FELIX_VERSION_MAJ >= 2
    if ( g_psCIDriver->sHWInfo.gasket_aType[ui32Gasket] == CI_GASKET_MIPI )
    {
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[ui32Gasket],
            FELIX_GAS_MIPI_GASKET_FRAME_CNT_OFFSET,
            gasketCount);
    }
    else if ( g_psCIDriver->sHWInfo.gasket_aType[ui32Gasket] == CI_GASKET_PARALLEL )
    {
        TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegGaskets[ui32Gasket],
            FELIX_GAS_PARA_GASKET_FRAME_CNT_OFFSET,
            gasketCount);
    }
#endif
}

IMG_RESULT FullFelix::fakeCtxInterrupt(IMG_UINT32 ctx, bool updateGasket, bool callHandler)
{
    IMG_UINT32 enabled = 0;
    IMG_UINT32 expectedEnabled = FELIX_CONTEXT0_INTERRUPT_ENABLE_INT_EN_START_OF_FRAME_RECEIVED_MASK|FELIX_CONTEXT0_INTERRUPT_ENABLE_INT_EN_FRAME_DONE_ALL_MASK;
    IMG_UINT32 currentId;
    int ret;

    //int ctx = pKrnPipeline->pipelineConfig.ui8Context;
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[ctx],
        FELIX_CONTEXT0_INTERRUPT_ENABLE_OFFSET, &enabled);

    if ( g_psCIDriver->aActiveCapture[ctx] == NULL )
    {
        //FAIL() << "no enabled pipeline for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if( (enabled&expectedEnabled) == 0 )
    {
        //FAIL() << "interrupts are not enabled for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if ( g_psCIDriver->aActiveCapture[ctx]->sList_pending.ui32Elements == 0 )
    {
        //FAIL() << "no frame was triggered before the interrupt";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // fake we received an interrupt
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[ctx],
        FELIX_CONTEXT0_INTERRUPT_STATUS_OFFSET,
        expectedEnabled);

    sCell_T *phead = List_getHead(&(g_psCIDriver->aActiveCapture[ctx]->sList_pending));
    currentId = 0;

    // fake last loaded ID
    REGIO_WRITE_FIELD(currentId, FELIX_CONTEXT0, LAST_FRAME_INFO, LAST_CONTEXT_TAG, ((KRN_CI_SHOT*)phead->object)->iIdentifier);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[ctx],
        FELIX_CONTEXT0_LAST_FRAME_INFO_OFFSET,
        currentId);

    if (updateGasket)
    {
        // fake that gasket frame counter went up
        enabled = HW_CI_GasketFrameCount(g_psCIDriver->aActiveCapture[ctx]->iifConfig.ui8Imager, 0);
        writeGasketCount(g_psCIDriver->aActiveCapture[ctx]->iifConfig.ui8Imager, enabled+1);
    }

    if (callHandler)
    {
        // fake the whole interrupt
        // 5 is an arbitrary number
        ret = g_psCIDriver->pDevice->irqHardHandle(5, NULL);
        if (IRQ_WAKE_THREAD == ret)
        {
            ret = g_psCIDriver->pDevice->irqThreadHandle(5, NULL);
            if (IRQ_NONE == ret)
            {
                std::cout << "ERROR: no interrupt to handled in threaded IRQ!"
                    << std::endl;
                return IMG_ERROR_FATAL;
            }
        }
        else
        {
            std::cout << "WARNING: no interrupt to handled in hard IRQ!"
                << std::endl;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT FullFelix::fakeIIFDgInterrupt(IMG_UINT32 dgx, bool callhandler)
{
    IMG_UINT32 enabled = 0;
    IMG_UINT32 expectedEnabled = FELIX_DG_IIF_DG_INTER_ENABLE_DG_INT_END_OF_FRAME_EN_MASK;
    IMG_RESULT ret;

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegIIFDataGen[dgx],
        FELIX_DG_IIF_DG_INTER_ENABLE_OFFSET, &enabled);

    if (g_psCIDriver->pActiveDatagen == NULL)
    {
        //FAIL() << "no enabled pipeline for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if ((enabled&expectedEnabled) == 0)
    {
        //FAIL() << "interrupts are not enabled for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (g_psCIDriver->pActiveDatagen->sList_busy.ui32Elements == 0)
    {
        //FAIL() << "no frame was triggered before the interrupt";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // fake we received an interrupt
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegIIFDataGen[dgx],
        FELIX_DG_IIF_DG_INTER_STATUS_OFFSET,
        expectedEnabled);

    if (callhandler)
    {
        // fake the whole interrupt
        // 5 is an arbitrary number
        ret = g_psCIDriver->pDevice->irqHardHandle(5, NULL);
        if (IRQ_WAKE_THREAD == ret)
        {
            ret = g_psCIDriver->pDevice->irqThreadHandle(5, NULL);
            if (IRQ_NONE == ret)
            {
                std::cout << "ERROR: no interrupt to handled in threaded IRQ!"
                    << std::endl;
                return IMG_ERROR_FATAL;
            }
        }
        else
        {
            std::cout << "WARNING: no interrupt to handled in hard IRQ!"
                << std::endl;
        }
    }
    return IMG_SUCCESS;
}

bool CheckMapped::memMapped(const SYS_MEM *pMem) const
{
    IMG_ASSERT(pMem);
    IMG_ASSERT(pMMUDir);

    if (pMem->pDevVirtAddr)
    {
        const IMG_UINTPTR virt = pMem->pDevVirtAddr->uiVirtualAddress;
        const IMG_UINTPTR lastVirt = virt + pMem->pDevVirtAddr->uiAllocSize;
        const IMG_UINTPTR pagesize = IMGMMU_GetPageSize();

        for (IMG_UINTPTR p = virt; p < lastVirt; p += pagesize)
        {
            IMG_UINT32 entry = IMGMMU_DirectoryGetPageTableEntry(pMMUDir, p);

            if (entry == (IMG_UINT32)-1 || (entry & 0x1) == 0)
            {
                if (expected)
                {
                    ADD_FAILURE() << " page " << std::hex << p
                        << " was expected to be mapped";
                }
                return false;  // not mapped
            }
        }
        if (!expected)
        {
            ADD_FAILURE() << "pages from " << std::hex << virt
                << " was not expected to be mapped";
        }
        return true;  // mapped
    }
    return false;  // not mapped
}

bool CheckMapped::bufferMapped(const KRN_CI_BUFFER *pBuff) const
{
    IMG_ASSERT(pBuff);
    IMG_ASSERT(pMMUDir);
    return memMapped(&(pBuff->sMemory));
}

bool CheckMapped::pipelineBuffers(KRN_CI_PIPELINE *pPipeline, bool expected)
{
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pPipeline->pipelineConfig.ui8Context < CI_N_CONTEXT);

    CheckMapped checker;
    sCell_T *found = NULL;

    checker.expected = expected;
    checker.pMMUDir = g_psCIDriver->sMMU.apDirectory[pPipeline->pipelineConfig.ui8Context];
    IMG_ASSERT(checker.pMMUDir != NULL);

    found = List_visitor(&(pPipeline->sList_availableBuffers), &checker,
        &Visitor_checkBufferMapped);
    if (found) return false;
    found = List_visitor(&(pPipeline->sList_matrixBuffers), &checker,
        &Visitor_checkBufferMapped);
    if (found) return false;

    found = List_visitor(&(pPipeline->sList_available), &checker,
        &Visitor_checkShotMapped);
    if (found) return false;
    found = List_visitor(&(pPipeline->sList_sent), &checker,
        &Visitor_checkShotMapped);
    if (found) return false;
    found = List_visitor(&(pPipeline->sList_processed), &checker,
        &Visitor_checkShotMapped);
    if (found) return false;
    found = List_visitor(&(pPipeline->sList_pending), &checker,
        &Visitor_checkShotMapped);
    if (found) return false;

    return true;
}

bool CheckMapped::pipelineLSHMatrix(KRN_CI_PIPELINE *pPipeline, bool expected)
{
    IMG_ASSERT(g_psCIDriver);
    IMG_ASSERT(pPipeline);
    IMG_ASSERT(pPipeline->pipelineConfig.ui8Context < CI_N_CONTEXT);

    CheckMapped checker;
    sCell_T *found = NULL;

    checker.expected = expected;
    checker.pMMUDir = g_psCIDriver->sMMU.apDirectory[pPipeline->pipelineConfig.ui8Context];
    IMG_ASSERT(checker.pMMUDir != NULL);

    found = List_visitor(&(pPipeline->sList_matrixBuffers), &checker,
        &Visitor_checkBufferMapped);
    if (found) return false;
    return true;
}

IMG_BOOL8 Visitor_checkShotMapped(void *elem, void *param)
{
    CheckMapped *info = static_cast<CheckMapped*>(param);
    KRN_CI_SHOT *pShot = static_cast<KRN_CI_SHOT *>(elem);
    bool ret;

    ret = info->memMapped(&(pShot->hwLinkedList));
    if (ret != info->expected) return IMG_FALSE;  // failed
    ret = info->memMapped(&(pShot->hwLoadStructure.sMemory));
    if (ret != info->expected) return IMG_FALSE;  // failed
    if (pShot->phwEncoderOutput)
    {
        ret = info->bufferMapped(pShot->phwEncoderOutput);
        if (ret != info->expected) return IMG_FALSE;  // failed
    }
    if (pShot->phwDisplayOutput)
    {
        ret = info->bufferMapped(pShot->phwDisplayOutput);
        if (ret != info->expected) return IMG_FALSE;  // failed
    }
    if (pShot->phwHDRExtOutput)
    {
        ret = info->bufferMapped(pShot->phwHDRExtOutput);
        if (ret != info->expected) return IMG_FALSE;  // failed
    }
    if (pShot->phwHDRInsertion)
    {
        ret = info->bufferMapped(pShot->phwHDRInsertion);
        if (ret != info->expected) return IMG_FALSE;  // failed
    }
    if (pShot->phwRaw2DExtOutput)
    {
        ret = info->bufferMapped(pShot->phwRaw2DExtOutput);
        if (ret != info->expected) return IMG_FALSE;  // failed
    }
    ret = info->bufferMapped(&(pShot->hwSave));
    if (ret != info->expected) return IMG_FALSE;  // failed
    if (pShot->sDPFWrite.sMemory.uiAllocated > 0)
    {
        ret = info->bufferMapped(&(pShot->sDPFWrite));
        if (ret != info->expected) return IMG_FALSE;  // failed
    }
    if (pShot->sENSOutput.sMemory.uiAllocated > 0)
    {
        ret = info->bufferMapped(&(pShot->sENSOutput));
        if (ret != info->expected) return IMG_FALSE;  // failed
    }

    return IMG_TRUE;
}

// return IMG_TRUE to continue going through the list
IMG_BOOL8 Visitor_checkBufferMapped(void* elem, void *param)
{
    CheckMapped *info = static_cast<CheckMapped*>(param);
    KRN_CI_BUFFER *pBuff = static_cast<KRN_CI_BUFFER *>(elem);
    bool ret;

    ret = info->bufferMapped(pBuff);
    // if expected && !ret || !expected && ret
    return info->expected != ret ? IMG_FALSE : IMG_TRUE;
}

IMG_RESULT FullFelix::fakeInterrupt(IMG_UINT32 ctx)
{
    IMG_UINT32 enabled = 0;
    IMG_UINT32 expectedEnabled = FELIX_CONTEXT0_INTERRUPT_ENABLE_INT_EN_START_OF_FRAME_RECEIVED_MASK|FELIX_CONTEXT0_INTERRUPT_ENABLE_INT_EN_FRAME_DONE_ALL_MASK;
    IMG_UINT32 currentId;
    int ret;

    //int ctx = pKrnPipeline->pipelineConfig.ui8Context;
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[ctx],
        FELIX_CONTEXT0_INTERRUPT_ENABLE_OFFSET, &enabled);

    if ( g_psCIDriver->aActiveCapture[ctx] == NULL )
    {
        //FAIL() << "no enabled pipeline for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if( (enabled&expectedEnabled) == 0 )
    {
        //FAIL() << "interrupts are not enabled for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if ( g_psCIDriver->aActiveCapture[ctx]->sList_pending.ui32Elements == 0 )
    {
        //FAIL() << "no frame was triggered before the interrupt";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // fake we received an interrupt
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[ctx],
        FELIX_CONTEXT0_INTERRUPT_STATUS_OFFSET,
        expectedEnabled);

    sCell_T *phead = List_getHead(&(g_psCIDriver->aActiveCapture[ctx]->sList_pending));
    currentId = 0;

    // fake last loaded ID
    REGIO_WRITE_FIELD(currentId, FELIX_CONTEXT0, LAST_FRAME_INFO, LAST_CONTEXT_TAG, ((KRN_CI_SHOT*)phead->object)->iIdentifier);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[ctx],
        FELIX_CONTEXT0_LAST_FRAME_INFO_OFFSET,
        currentId);

    // fake that gasket frame counter went up
    //  eagle use the virtual channel
    enabled = HW_CI_GasketFrameCount(
        g_psCIDriver->aActiveCapture[ctx]->iifConfig.ui8Imager, 0);
    writeGasketCount(g_psCIDriver->aActiveCapture[ctx]->iifConfig.ui8Imager,
        enabled+1);

#if 0
    // fake interrupt source
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
        FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET, ctx + 1);

    // use the interrupt thread
    currentId = 0;
    while(currentId == 0)
    {
        sched_yield();  // give a change to the interrupt thread

        TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixCore,
            FELIX_CORE_FELIX_INTERRUPT_SOURCE_OFFSET, &currentId);
    }
#else
    // fake the whole interrupt
    // 5 is an arbitrary number
    ret = g_psCIDriver->pDevice->irqHardHandle(5, NULL);
    if (IRQ_WAKE_THREAD == ret)
    {
        ret = g_psCIDriver->pDevice->irqThreadHandle(5, NULL);
        if (IRQ_NONE == ret)
        {
            std::cout << "ERROR: no interrupt to handled in threaded IRQ!"
                << std::endl;
            return IMG_ERROR_FATAL;
        }
    }
    else
    {
        std::cout << "WARNING: no interrupt to handled in hard IRQ!"
            << std::endl;
    }
#endif

    return IMG_SUCCESS;
}

/*
 * stolen from ci_connection.c
 */
// in user mode debuge copy to and from "kernel" does not make a difference
// copy_to_user and copy_from_user return the amount of memory still to be copied: i.e. 0 is the success result
#define copy_from_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define copy_to_user(to, from, size) ( IMG_MEMCPY(to, from, size) == NULL ? size : 0 )
#define __user

// returns true if visiter should continue visiting the list
static IMG_BOOL8 ListVisitor_findPipeline(void* listElem, void* lookingFor)
{
    IMG_UINT32 *lookingForId = (IMG_UINT32 *)lookingFor;
    KRN_CI_PIPELINE *pConfig = (KRN_CI_PIPELINE*)listElem;

    return !(pConfig->ui32Identifier == *lookingForId);
}

static sCell_T* FindPipeline(KRN_CI_CONNECTION *pConnection, IMG_UINT32 captureId)
{
    sCell_T *pFound = NULL;
    IMG_RESULT ret = IMG_SUCCESS;

    if ( captureId == 0 )
    {
        CI_FATAL("Capture not registered\n");
        return NULL;
    }

    SYS_LockAcquire(&(pConnection->sLock));
    {
        pFound = List_visitor(&(pConnection->sList_pipeline), (void*)&captureId, &ListVisitor_findPipeline);
    }
    SYS_LockRelease(&(pConnection->sLock));

    return pFound;
}

/*
 * end of stolen
 */

KRN_CI_PIPELINE* FullFelix::getKrnPipeline(CI_PIPELINE *pUsePipeline)
{
    INT_PIPELINE *pIntConfig = NULL;
    if (pUsePipeline == NULL )
    {
        pUsePipeline = pPipeline;
        if ( pPipeline == NULL )
        {
            return NULL;
        }
    }

    pIntConfig = container_of(pUsePipeline, INT_PIPELINE, publicPipeline);

    sCell_T *pFound = FindPipeline(getKrnConnection(), pIntConfig->ui32Identifier);
    if ( pFound == NULL ) return NULL;
    return (KRN_CI_PIPELINE*)pFound->object;
}

#ifdef FELIX_HAS_DG

Datagen::Datagen() : Felix()
{
    pDGConnection = NULL;
}

Datagen::~Datagen()
{
    clean();
}

void Datagen::clean()
{
    if (pDGConnection)
    {
        DG_DriverFinalise(pDGConnection);
        pDGConnection = NULL;
    }
    KRN_DG_DriverDestroy(&sDGDriver);

    Felix::clean();
}

DG_CONNECTION *Datagen::getDGConnection()
{
    return pDGConnection;
}

IMG_RESULT Datagen::configure(enum FelixRegInit regInit, IMG_UINT8 uiMMU)
{
    IMG_RESULT ret;

    if (g_pDGDriver)
    {
        CI_FATAL("dg already initiliased! Previous test must have "\
            "failed without deinitialising!");
        KRN_DG_DriverDestroy(g_pDGDriver);
    }

    ret = Felix::configure(regInit, uiMMU);
    if (!ret)
    {
        // default PLL
        ret = KRN_DG_DriverCreate(&sDGDriver, 0, 0, uiMMU);
        if (!ret)
        {
            ret = DG_DriverInit(&pDGConnection);
            if (ret)
            {
                CI_FATAL("Failed to init DG driver\n");
            }
        }
        else
        {
            CI_FATAL("Failed to create DG driver\n");
        }
    }
    return ret;
}

IMG_RESULT Datagen::fakeDgxInterrupt(IMG_UINT32 dgx, bool updateGasket, bool callHandler)
{
    IMG_UINT32 enabled = 0;
    IMG_UINT32 expectedEnabled = FELIX_TEST_DG_DG_INTER_ENABLE_DG_INT_END_OF_FRAME_EN_MASK;
    int ret;

    //int ctx = pKrnPipeline->pipelineConfig.ui8Context;
    TALREG_ReadWord32(g_pDGDriver->hRegFelixDG[dgx],
        FELIX_TEST_DG_DG_INTER_ENABLE_OFFSET, &enabled);

    if (g_pDGDriver->aActiveCamera[dgx] == NULL)
    {
        //FAIL() << "no enabled pipeline for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if ((enabled&expectedEnabled) == 0)
    {
        //FAIL() << "interrupts are not enabled for this context!";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    if (g_pDGDriver->aActiveCamera[dgx]->sList_pending.ui32Elements == 0)
    {
        //FAIL() << "no frame was triggered before the interrupt";
        return IMG_ERROR_UNEXPECTED_STATE;
    }

    // fake we received an interrupt
    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dgx],
        FELIX_TEST_DG_DG_INTER_STATUS_OFFSET,
        expectedEnabled);

    // fake FELIX_TEST_DG_DG_STATUS_OFFSET to increase number of captured frames
    IMG_UINT32 reg = 0;
    TALREG_ReadWord32(g_pDGDriver->hRegFelixDG[dgx], FELIX_TEST_DG_DG_STATUS_OFFSET, &reg);
    enabled = REGIO_READ_FIELD(reg, FELIX_TEST_DG, DG_STATUS, DG_FRAMES_SENT);
    REGIO_WRITE_FIELD(reg, FELIX_TEST_DG, DG_STATUS, DG_FRAMES_SENT, enabled + 1);
    TALREG_WriteWord32(g_pDGDriver->hRegFelixDG[dgx], FELIX_TEST_DG_DG_STATUS_OFFSET, reg);

    if (updateGasket)
    {
        // fake that gasket frame counter went up
        enabled = HW_CI_GasketFrameCount(g_pDGDriver->aActiveCamera[dgx]->publicCamera.ui8Gasket, 0);
        FullFelix::writeGasketCount(g_pDGDriver->aActiveCamera[dgx]->publicCamera.ui8Gasket, enabled + 1);
    }

    if (callHandler)
    {
        // fake the whole interrupt
        // 5 is an arbitrary number
        ret = g_psCIDriver->pDevice->irqHardHandle(5, NULL);
        if (IRQ_WAKE_THREAD == ret)
        {
            ret = g_psCIDriver->pDevice->irqThreadHandle(5, NULL);
            if (IRQ_NONE == ret)
            {
                std::cout << "ERROR: no interrupt to handled in threaded IRQ!"
                    << std::endl;
                return IMG_ERROR_FATAL;
            }
        }
        else
        {
            std::cout << "WARNING: no interrupt to handled in hard IRQ!"
                << std::endl;
        }
    }

    return IMG_SUCCESS;
}

#endif /* FELIX_HAS_DG */

std::ostream& operator<< (std::ostream& os, const Test_IMG_RESULT& e)
{
    os << (int)e.ret << " (";

    switch(e.ret)
    {
        case    IMG_SUCCESS:
        os << "IMG_SUCCESS";
    break;
        case IMG_ERROR_TIMEOUT:
            os << "IMG_ERROR_TIMEOUT";
    break;
        case IMG_ERROR_MALLOC_FAILED:
            os << "IMG_ERROR_MALLOC_FAILED";
    break;
        case IMG_ERROR_FATAL:
            os << "IMG_ERROR_FATAL";
    break;
        case IMG_ERROR_OUT_OF_MEMORY:
            os << "IMG_ERROR_OUT_OF_MEMORY";
    break;
        case IMG_ERROR_DEVICE_NOT_FOUND:
            os << "IMG_ERROR_DEVICE_NOT_FOUND";
    break;
        case IMG_ERROR_DEVICE_UNAVAILABLE:
            os << "IMG_ERROR_DEVICE_UNAVAILABLE";
    break;
        case IMG_ERROR_GENERIC_FAILURE:
            os << "IMG_ERROR_GENERIC_FAILURE";
    break;
        case IMG_ERROR_INTERRUPTED:
            os << "IMG_ERROR_INTERRUPTED";
    break;
        case IMG_ERROR_INVALID_ID:
            os << "IMG_ERROR_INVALID_ID";
    break;
        case IMG_ERROR_SIGNATURE_INCORRECT:
            os << "IMG_ERROR_SIGNATURE_INCORRECT";
    break;
        case IMG_ERROR_INVALID_PARAMETERS:
            os << "IMG_ERROR_INVALID_PARAMETERS";
    break;
        case IMG_ERROR_STORAGE_TYPE_EMPTY:
            os << "IMG_ERROR_STORAGE_TYPE_EMPTY";
    break;
        case IMG_ERROR_STORAGE_TYPE_FULL:
            os << "IMG_ERROR_STORAGE_TYPE_FULL";
    break;
        case IMG_ERROR_ALREADY_COMPLETE:
            os << "IMG_ERROR_ALREADY_COMPLETE";
    break;
        case IMG_ERROR_UNEXPECTED_STATE:
            os << "IMG_ERROR_UNEXPECTED_STATE";
    break;
        case IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE:
            os << "IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE";
    break;
        case IMG_ERROR_NOT_INITIALISED:
            os << "IMG_ERROR_NOT_INITIALISED";
    break;
        case IMG_ERROR_ALREADY_INITIALISED:
            os << "IMG_ERROR_ALREADY_INITIALISED";
    break;
        case IMG_ERROR_VALUE_OUT_OF_RANGE:
            os << "IMG_ERROR_VALUE_OUT_OF_RANGE";
    break;
        case IMG_ERROR_CANCELLED:
            os << "IMG_ERROR_CANCELLED";
    break;
        case IMG_ERROR_MINIMUM_LIMIT_NOT_MET:
            os << "IMG_ERROR_MINIMUM_LIMIT_NOT_MET";
    break;
        case IMG_ERROR_NOT_SUPPORTED:
            os << "IMG_ERROR_NOT_SUPPORETED";
    break;
        case IMG_ERROR_IDLE:
            os << "IMG_ERROR_IDLE";
    break;
        case IMG_ERROR_BUSY:
            os << "IMG_ERROR_BUSY";
    break;
        case IMG_ERROR_DISABLED:
            os << "IMG_ERROR_DISABLED";
    break;
        case IMG_ERROR_OPERATION_PROHIBITED:
            os << "IMG_ERROR_OPERATION_PROHIBITED";
    break;
        case IMG_ERROR_MMU_PAGE_DIRECTORY_FAULT:
            os << "IMG_ERROR_MMU_PAGE_DIRECTORY_FAULT";
    break;
        case IMG_ERROR_MMU_PAGE_TABLE_FAULT:
            os << "IMG_ERROR_MMU_PAGE_TABLE_FAULT";
    break;
        case IMG_ERROR_MMU_PAGE_CATALOGUE_FAULT:
            os << "IMG_ERROR_MMU_PAGE_CATALOGUE_FAULT";
    break;
        case IMG_ERROR_MEMORY_IN_USE:
            os << "IMG_ERROR_MEMORY_IN_USE";
    break;
        default:
            os << "???";
    }

    os << ")";
    return os;
}

void CreateLSHMatrix(CI_PIPELINE *pPipeline, const IMG_UINT32 ui32Width,
    const IMG_UINT32 ui32Height, const IMG_UINT32 ui32TileSize,
    const IMG_UINT8 bitsPerDiff, IMG_UINT32 &matrixId,
    const float *lshCorners)
{
    LSH_GRID sGrid = LSH_GRID();
    CI_LSHMAT sMat;
    int c;
    IMG_UINT32 uiLineSize = 0, uiStride = 0, uiAllocation = 0;
    IMG_UINT8 ui8BitsPerDiff = 0;

    ASSERT_EQ(ui32TileSize, 1 << static_cast<int>(log2(ui32TileSize)))
        << "test is erroneous";
    ASSERT_GE(ui32TileSize, (unsigned)LSH_TILE_MIN) << "test is erroneous";
    ASSERT_LE(ui32TileSize, (unsigned)LSH_TILE_MAX) << "test is erroneous";
    ASSERT_GE(bitsPerDiff, LSH_DELTA_BITS_MIN) << "test is erroneous";
    ASSERT_LE(bitsPerDiff, LSH_DELTA_BITS_MAX) << "test is erroneous";

    ASSERT_EQ(IMG_SUCCESS,
        LSH_CreateMatrix(&sGrid, ui32Width, ui32Height, ui32TileSize));
    for (c = 0; c < LSH_GRADS_NO; c++)
    {
        ASSERT_EQ(IMG_SUCCESS, LSH_FillBowl(&sGrid, c, lshCorners));
    }

    ui8BitsPerDiff = MC_LSHComputeMinBitdiff(&sGrid, NULL);
    EXPECT_GE(ui8BitsPerDiff, LSH_DELTA_BITS_MIN);
    EXPECT_LE(ui8BitsPerDiff, LSH_DELTA_BITS_MAX);
    EXPECT_GE(bitsPerDiff, ui8BitsPerDiff) << "computed wrong bits per diff!";
    ui8BitsPerDiff = bitsPerDiff;

    /*
    * line size = 2B + round_up_to_byte((width - 1) * bit_per_diff)
    * but line size has to be a multiple of 16B (HW requirement)
    *
    * stride is line size rounded up to multiple of 64B
    *
    * allocation is stride * height * 4 channels
    *
    * allocation in pages is allocation rounded up to page size (4KB or 16KB)
    */
    uiAllocation = MC_LSHGetSizes(&sGrid, ui8BitsPerDiff,
        &uiLineSize, &uiStride);

    ASSERT_EQ(IMG_SUCCESS,
        CI_PipelineAllocateLSHMatrix(pPipeline, uiAllocation, &matrixId))
        << "failed to allocate matrix!";
    EXPECT_GT(matrixId, 0u) << "matrixId is invalid";

    EXPECT_EQ(IMG_SUCCESS,
        CI_PipelineAcquireLSHMatrix(pPipeline, matrixId, &sMat));

    EXPECT_EQ(uiAllocation, sMat.ui32Size)
        << "allocation size rounded to page size is wrong";

    EXPECT_EQ(IMG_SUCCESS, MC_LSHConvertGrid(&sGrid, ui8BitsPerDiff, &sMat));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseLSHMatrix(pPipeline, &sMat));

    LSH_Free(&sGrid);
}
