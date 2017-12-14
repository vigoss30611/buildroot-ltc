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

void InitTalReset();
void (*gpfnInitTal)(void) = &InitTalReset; // this is used to setup the TAL default values when using the NULL interface
IMG_BOOL8 gbUseTalNULL = IMG_FALSE;

#include "ci/ci_api.h"
#include "ci_kernel/ci_kernel.h"
#include "ci/ci_api_internal.h"

#include <cstdio>

#include <tal.h>
#include <target.h>
#include <reg_io2.h>
#include <registers/core.h>
#include <registers/context0.h>
#include <registers/mmu.h>
#ifdef FELIX_HAS_DG
#include <dg/dg_api.h>
#include <dg_kernel/dg_camera.h>
#include <registers/ext_data_generator.h>
#endif

static bool gTalWasInit = false;

TAL_HANDLES::TAL_HANDLES()
{
    int i;
    char name[64];

    driver.hRegFelixCore = TAL_GetMemSpaceHandle("REG_FELIX_CORE");
    driver.hRegFelixContext[0] = TAL_GetMemSpaceHandle("REG_FELIX_CONTEXT_0");
    hRegMMU = TAL_GetMemSpaceHandle("REG_FELIX_MMU");
    //driver.hRegFelixI2C = NULL;
    driver.hMemHandle = NULL;

    for (i = 0 ; i < CI_N_CONTEXT ; i++ )
    {
        sprintf(name, "REG_FELIX_CONTEXT_%d", i);
        driver.hRegFelixContext[i] = TAL_GetMemSpaceHandle(name);
    }

    datagen.hMemHandle = NULL;
    datagen.hRegMMU = TAL_GetMemSpaceHandle("REG_TEST_MMU");
    for (i = 0 ; i < CI_N_EXT_DATAGEN ; i++ )
    {
        sprintf(name, "REG_TEST_DG_%d", i);
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
        IMG_UINT32 line = CONTEXT_MAX_WIDTH;
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
    value = CONTEXT_MAX_WIDTH;
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
        ret = KRN_CI_DriverCreate(&sCIDriver, bMMUEnabled, 256, CI_DEF_GMACURVE, &sDevice);
        break;

    case NONE:
    default:
        gpfnInitTal = NULL;
        ret = KRN_CI_DriverCreate(&sCIDriver, bMMUEnabled, 256, CI_DEF_GMACURVE, &sDevice);
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
    if ( pUserConnection )
    {
        CI_DriverFinalise(pUserConnection);
        pUserConnection = NULL;
    }

  if ( g_psCIDriver != NULL )
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
    int ctx = pKrnPipeline->userPipeline.ui8Context;

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
    if ( pPipeline != NULL )
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

    if ( (ret=CI_PipelineCreate(&pPipeline, pUserConnection)) != IMG_SUCCESS )
    {
        FAIL() << "failed to init pPipeline (returned " << (Test_IMG_RESULT)ret << ")";
    }

    pPipeline->ui8Context = 0;
    pPipeline->sImagerInterface.ui8Imager = 0;
    pPipeline->sImagerInterface.eBayerFormat = MOSAIC_RGGB;

    pPipeline->sImagerInterface.ui16ImagerSize[0] = (width/CI_CFA_WIDTH)-1;
    pPipeline->sImagerInterface.ui16ImagerSize[1] = (height/CI_CFA_HEIGHT)-1;
    if ( yuvOut != PXL_NONE )
    {
        pPipeline->ui16MaxEncOutWidth = width;
        pPipeline->ui16MaxEncOutHeight = height;
        pPipeline->sEncoderScaler.aOutputSize[0] = (width-2)/2;
        pPipeline->sEncoderScaler.aOutputSize[1] = height-1;
    }
    else
    {
        pPipeline->sEncoderScaler.bBypassScaler = IMG_TRUE;
    }
    if ( rgbOut != PXL_NONE )
    {
        pPipeline->ui16MaxDispOutWidth = width;
        pPipeline->ui16MaxDispOutHeight = height;
        pPipeline->sDisplayScaler.aOutputSize[0] = (width-2)/2;
        pPipeline->sDisplayScaler.aOutputSize[1] = height-1;
    }
    else
    {
        pPipeline->sDisplayScaler.bBypassScaler = IMG_TRUE;
    }
    PixelTransformYUV(&pPipeline->eEncType, yuvOut);
    PixelTransformRGB(&pPipeline->eDispType, rgbOut);

    if ( pPipeline->eEncType.ui8HSubsampling == 2 ) // 420
    {
        pPipeline->sEncoderScaler.eSubsampling = EncOut420_scaler;
    }

    if ( genCRC )
    {
        pPipeline->eOutputConfig = (enum CI_SAVE_CONFIG_FLAGS )(pPipeline->eOutputConfig | CI_SAVE_CRC);
    }
    else
    {
        pPipeline->eOutputConfig = (enum CI_SAVE_CONFIG_FLAGS )(pPipeline->eOutputConfig & ~CI_SAVE_CRC);
    }

    // to be sure the minimum column size is met (although it should be computed from the number of columns)
    pPipeline->sToneMapping.localColWidth[0] = TNM_MIN_COL_WIDTH;

    pPipeline->sExposureStats.ui16Width = EXS_MIN_WIDTH;
    pPipeline->sExposureStats.ui16Height = EXS_MIN_HEIGHT;

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

IMG_RESULT FullFelix::fakeInterrupt(IMG_UINT32 ctx)
{
    IMG_UINT32 enabled = 0;
    IMG_UINT32 expectedEnabled = FELIX_CONTEXT0_INTERRUPT_ENABLE_INT_EN_START_OF_FRAME_RECEIVED_MASK|FELIX_CONTEXT0_INTERRUPT_ENABLE_INT_EN_FRAME_DONE_ALL_MASK;
    IMG_UINT32 currentId;
    int ret;

    //int ctx = pKrnPipeline->userPipeline.ui8Context;
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
    enabled = HW_CI_GasketFrameCount(g_psCIDriver->aActiveCapture[ctx]->userPipeline.sImagerInterface.ui8Imager);
    writeGasketCount(g_psCIDriver->aActiveCapture[ctx]->userPipeline.sImagerInterface.ui8Imager, enabled+1);

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

Datagen::~Datagen()
{
    DG_DriverFinalise();
    KRN_DG_DriverDestroy(&sDGDriver);
}

IMG_RESULT Datagen::configure(enum FelixRegInit regInit, IMG_UINT8 uiMMU)
{
    IMG_RESULT ret;

    if ( g_pDGDriver != NULL )
    {
        CI_FATAL("dg already initiliased! Previous test must have failed without deinitialising!");
        KRN_DG_DriverDestroy(g_pDGDriver);
    }

    if ( (ret=Felix::configure(regInit, uiMMU)) == IMG_SUCCESS )
    {
        if ( (ret=KRN_DG_DriverCreate(&sDGDriver, uiMMU)) == IMG_SUCCESS )
        {
            if ( (ret=DG_DriverInit()) != IMG_SUCCESS )
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

#endif // FELIX_HAS_DG

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
