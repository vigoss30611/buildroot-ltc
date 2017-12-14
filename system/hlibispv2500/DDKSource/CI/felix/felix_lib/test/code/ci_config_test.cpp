/**
******************************************************************************
 @file ci_config_test.cpp

 @brief

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
#include <gtest/gtest.h>

#include <cstdlib>
#include <cstdio>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <tal.h>
#include <reg_io2.h>

#include "unit_tests.h"
#include "felix.h"

#include "ci/ci_api.h"
#include "ci/ci_api_internal.h" // access to internal objects
#include "ci_kernel/ci_kernel.h"
#include "registers/context0.h"
#include "felix_hw_info.h" // max DPF output size
#include <hw_struct/ctx_config.h>

TEST(CI_Config, create_destroy)
{
    Felix driver; // init the drivers
    CI_PIPELINE *pConfig = NULL;
    KRN_CI_CONNECTION *pConn = NULL;

    driver.configure();

    pConn = driver.getKrnConnection();

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineCreate(&pConfig, driver.getConnection()));

    EXPECT_EQ (0, pConn->sList_pipeline.ui32Elements) << "no pipeline configuration should be registered yet";

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineDestroy(pConfig)) << "should not fail if not registered";
    pConfig = NULL;

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineCreate(&pConfig, driver.getConnection()));

    // configure it a bit
    pConfig->ui8Context = 0;
    pConfig->sImagerInterface.ui16ImagerSize[1] = (32/CI_CFA_WIDTH)-1;
    pConfig->sImagerInterface.ui16ImagerSize[0] = (32/CI_CFA_HEIGHT)-1;
    pConfig->sEncoderScaler.aOutputSize[0] = (32-2)/2;
    pConfig->sEncoderScaler.aOutputSize[1] = 32-1;
    pConfig->ui16MaxEncOutWidth = 32;
    pConfig->ui16MaxEncOutHeight = 32;
    pConfig->sDisplayScaler.bBypassScaler = IMG_TRUE;
    PixelTransformYUV(&pConfig->eEncType, YVU_422_PL12_8);
    PixelTransformRGB(&pConfig->eDispType, PXL_NONE);
    //pConfig->ui32Stride = 128;
    pConfig->sToneMapping.localColWidth[0] = TNM_MIN_COL_WIDTH;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineRegister(pConfig));

    EXPECT_EQ (1, pConn->sList_pipeline.ui32Elements) << "pipeline configuration should be registered";

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineDestroy(pConfig)) << "works if registered";
    pConfig = NULL;

    EXPECT_EQ (0, pConn->sList_pipeline.ui32Elements) << "no more pipeline configuration should be registered";

    // driver will deinit here
}

TEST(CI_Config, create_destroy_atFinalise_registered)
{
    Felix driver; // init the drivers
    CI_PIPELINE *pConfig = NULL;
    INT_PIPELINE *pIntConfig = NULL;
    KRN_CI_CONNECTION *pConn = NULL;

    driver.configure();

    pConn = driver.getKrnConnection();

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineCreate(&pConfig, driver.getConnection()));

    //pConfig->eType = CI_TYPE_ENCODER;
    pConfig->ui8Context = 0;
    pConfig->sImagerInterface.ui16ImagerSize[1] = (32/CI_CFA_WIDTH)-1;
    pConfig->sImagerInterface.ui16ImagerSize[0] = (32/CI_CFA_HEIGHT)-1;
    pConfig->sEncoderScaler.aOutputSize[0] = (32-2)/2;
    pConfig->sEncoderScaler.aOutputSize[1] = 32-1;
    pConfig->ui16MaxEncOutWidth = 32;
    pConfig->ui16MaxEncOutHeight = 32;
    PixelTransformYUV(&pConfig->eEncType, YVU_420_PL12_8);
    pConfig->sEncoderScaler.eSubsampling = EncOut420_vss;
    //pConfig->ui32Stride = 128;
    pConfig->sToneMapping.localColWidth[0] = TNM_MIN_COL_WIDTH;

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineRegister(pConfig));

    pIntConfig = container_of(pConfig, INT_PIPELINE, publicPipeline);

    EXPECT_EQ(0, pIntConfig->sList_shots.ui32Elements) << "no allocated buffers";
    EXPECT_EQ (1, pConn->sList_pipeline.ui32Elements) << "pipeline configuration should be registered";

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineAddPool(pConfig, 2));
    for ( int i = 0 ; i < 2 ; i++ )
    {
        EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pConfig, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));
    }

    EXPECT_EQ(2, pIntConfig->sList_shots.ui32Elements) << "no allocated shots";
    EXPECT_EQ(2, pIntConfig->sList_buffers.ui32Elements) << "no allocated buffers";

    // driver will deinit here and should destroy the left-over - running test in valgrind will reveal a leak otherwise
}

TEST(CI_Config, create_destroy_atFinalise_notregistered)
{
    Felix driver; // init the drivers
    CI_PIPELINE *pConfig = NULL;
    INT_PIPELINE *pIntConfig = NULL;
    KRN_CI_CONNECTION *pConn = NULL;

    driver.configure();

    pConn = driver.getKrnConnection();

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineCreate(&pConfig, driver.getConnection()));

    // driver will deinit here and should destroy the left-over - running test in valgrind will reveal a leak otherwise
}

// function was removed
/*TEST(CI_Config, add_buffer)
  {
  Felix driver; // init the drivers
  CI_CONFIG *pConfig = NULL;
  IMG_CI_CONFIG *pPrivateConfig = NULL;
  CI_SHOT *pBuffer = NULL;

  driver.configure();

  EXPECT_EQ (IMG_SUCCESS, CI_ConfigCreate(&pConfig));

  pPrivateConfig = container_of(pConfig, IMG_CI_CONFIG*, publicConfig);

//pConfig->eType = CI_TYPE_ENCODER;
pConfig->ui16ImagerSize[1] = (32/CI_CFA_WIDTH) -1;
pConfig->ui16ImagerSize[0] = (32/CI_CFA_HEIGHT) -1;
//pConfig->ui32Stride = 128;

EXPECT_EQ (0, pPrivateConfig->slist_buffer_available->ui32Elements);

EXPECT_EQ (IMG_SUCCESS, CI_ShotCreate(&pBuffer));

//pBuffer->eType = pConfig->eType;
pBuffer->aEncYSize[0] = (pConfig->ui16ImagerSize[1]+1)*CI_CFA_WIDTH;
pBuffer->aEncYSize[1] = (pConfig->ui16ImagerSize[1]+1)*CI_CFA_HEIGHT;
pBuffer->aEncCbCrSize[0] = (pConfig->ui16ImagerSize[0]+1)*CI_CFA_WIDTH/2;
pBuffer->aEncCbCrSize[1] = (pConfig->ui16ImagerSize[1]+1)*CI_CFA_HEIGHT/2;
//pBuffer->ui32Stride = pConfig->ui32Stride;

EXPECT_EQ (IMG_SUCCESS, CI_ShotMallocBuffers(pBuffer));
EXPECT_EQ (IMG_SUCCESS, CI_ConfigAddBuffer(pConfig, pBuffer));

// driver will deinit here
}*/

TEST(CI_Config, create_destroy_invalid)
{
    FullFelix driver; // init the drivers
    CI_PIPELINE config;
    CI_PIPELINE *pConfig = &config;

    driver.configure();

    EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_PipelineCreate(NULL, NULL));
    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_PipelineCreate(&pConfig, NULL));
    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_PipelineCreate(NULL, driver.getConnection()));

    EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_PipelineDestroy(NULL));

    EXPECT_EQ (IMG_ERROR_MEMORY_IN_USE, CI_PipelineCreate(&pConfig, driver.getConnection()));
    pConfig = NULL;

    gui32AllocFails = 1;
    {
        EXPECT_EQ(IMG_ERROR_MALLOC_FAILED, CI_PipelineCreate(&pConfig, driver.getConnection()));
        EXPECT_TRUE ( pConfig == NULL );
    }

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineCreate(&pConfig, driver.getConnection()));

    EXPECT_EQ (IMG_SUCCESS, CI_PipelineDestroy(pConfig));
    pConfig = NULL;

    // driver will deinit here
}

// function was removed
/*TEST(CI_Config, add_buffer_invalid)
  {
  Felix driver; // init the drivers
  CI_CONFIG *pConfig = NULL;
  IMG_CI_CONFIG *pPrivateConfig = NULL;

  EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_ConfigAddBuffer(NULL, NULL));

  EXPECT_EQ (IMG_SUCCESS, CI_ConfigCreate(&pConfig));

  pPrivateConfig = container_of(pConfig, IMG_CI_CONFIG*, publicConfig);

//pConfig->eType = CI_TYPE_ENCODER;
pConfig->ui16ImagerSize[1] = 32/CI_CFA_WIDTH -1;
pConfig->ui16ImagerSize[0] = 32/CI_CFA_HEIGHT -1;
//pConfig->ui32Stride = 128;

EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, CI_ConfigAddBuffer(pConfig, NULL));

EXPECT_EQ (0, pPrivateConfig->slist_buffer_available->ui32Elements);

// driver will deinit here
}*/

#define VERIF_MODULE_UPD(field, block) \
    res = IMG_MEMCMP(&(pUserPipeline->field), &(pKrnPipeline->field), sizeof(CI_MODULE_ ## block)); \
if ( (eUpd&CI_UPD_ ## block ) != 0 && res != 0 || (eUpd&CI_UPD_ ## block ) == 0 && res == 0 ) \
FAIL() << "wrong update for block: " << CI_UPD_ ## block << " upd=" << eUpd;

static void testUpdate(CI_PIPELINE *pUserPipeline, CI_PIPELINE *pKrnPipeline, enum CI_MODULE_UPDATE eUpd)
{
    int res;

    if (pUserPipeline->ui8PrivateValue != pKrnPipeline->ui8PrivateValue)
    {
        FAIL() << "ui8PrivateValue";
    }

    VERIF_MODULE_UPD(sBlackCorrection, BLC);
    VERIF_MODULE_UPD(sDeshading, LSH);
    VERIF_MODULE_UPD(sWhiteBalance, WBC);
    VERIF_MODULE_UPD(sDenoiser, DNS);
    VERIF_MODULE_UPD(sDefectivePixels, DPF);
    VERIF_MODULE_UPD(sChromaAberration, LCA);
    // add DMO
    VERIF_MODULE_UPD(sColourCorrection, CCM);
    VERIF_MODULE_UPD(sMainGamutMapper, MGM);
    VERIF_MODULE_UPD(sGammaCorrection, GMA);
    VERIF_MODULE_UPD(sRGBToYCC, R2Y);
    // add MIE
    // add HSS
    VERIF_MODULE_UPD(sImageEnhancer, MIE);
    VERIF_MODULE_UPD(sToneMapping, TNM);
    VERIF_MODULE_UPD(sSharpening, SHA);
    VERIF_MODULE_UPD(sEncoderScaler, ESC);
    // add VSS
    VERIF_MODULE_UPD(sDisplayScaler, DSC);
    // add HUS
    VERIF_MODULE_UPD(sYCCToRGB, Y2R);
    VERIF_MODULE_UPD(sDisplayGamutMapper, DGM);
    // statistics
    VERIF_MODULE_UPD(sExposureStats, EXS);
    VERIF_MODULE_UPD(sFocusStats, FOS);
    VERIF_MODULE_UPD(sWhiteBalanceStats, WBS);
    VERIF_MODULE_UPD(sHistStats, HIS);
    VERIF_MODULE_UPD(sFlickerStats, FLD);
    //VERIF_MODULE_UPD(sEncStats, ENS);
}

static void testUpdateRegister(IMG_UINT64 regOffset, IMG_UINT32 updRegVal, int updMask, FullFelix &driver)
{
    IMG_UINT32 tmpA = 0, tmpB = 0, tmpC = 0;
    IMG_RESULT retA, retB;
    IMG_BOOL8 shouldFail = false;
    int updated;

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], regOffset, &tmpA);

    shouldFail = CI_PipelineHasPending(driver.pPipeline);

    retA=CI_PipelineUpdate(driver.pPipeline, updMask, &updated);

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], regOffset, &tmpB);

    retB=CI_PipelineUpdate(driver.pPipeline, updMask, NULL); // verify it works without parameter too

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], regOffset, &tmpC);

    if ( shouldFail )
    {
        EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, retA) << "shouldn't have worked with parameter";
        EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, retB) << "shouldn't have worked without parameter";
        EXPECT_NE(0, updated&updMask) << "this module should have failed";
        EXPECT_EQ(tmpA, tmpB) << "the registers should not have been updated!";
        EXPECT_EQ(tmpA, tmpC) << "the registers should not have been updated!";
    }
    else
    {
        EXPECT_EQ(IMG_SUCCESS, retA) << "should have worked with parameter";
        EXPECT_EQ(IMG_SUCCESS, retB) << "should have worked without parameter";
        EXPECT_NE(tmpA, tmpB) << "the registers should have been updated!";
        EXPECT_NE(tmpA, tmpC) << "the registers should have been updated!";
        EXPECT_EQ(updRegVal, tmpB) << "wrong value in register!";
        EXPECT_EQ(updRegVal, tmpC) << "wrong value in register!";
    }
}

/// @ add test for cumulating the flags to update
TEST(CI_Config, update)
{
    FullFelix driver; // init the drivers
    KRN_CI_PIPELINE *pKrnConfig = NULL;
    enum CI_MODULE_UPDATE eUpd = CI_UPD_ALL;
    int updated = CI_UPD_NONE;

    driver.configure();

    pKrnConfig = driver.getKrnPipeline();

    // change one value per module
    driver.pPipeline->sBlackCorrection.ui16PixelMax += 1;
    driver.pPipeline->sDeshading.aGradientsX[0] += 4;
    driver.pPipeline->sWhiteBalance.aGain[2] += 2;
    driver.pPipeline->sDenoiser.bCombineChannels = driver.pPipeline->sDenoiser.bCombineChannels == IMG_FALSE ? IMG_TRUE : IMG_FALSE;
    driver.pPipeline->sDefectivePixels.ui8PositiveThreshold += 3;
    driver.pPipeline->sChromaAberration.aCoeffBlue[2][1] += 3;
    // add DMO
    driver.pPipeline->sColourCorrection.aOffset[2] += 4;
    driver.pPipeline->sMainGamutMapper.aSlope[1] += 5;
    driver.pPipeline->sGammaCorrection.bBypass = IMG_TRUE;
    driver.pPipeline->sRGBToYCC.aCoeff[1][2] += 2;
    driver.pPipeline->sImageEnhancer.aGaussCB[0] += 5;
    // add HSS
    driver.pPipeline->sToneMapping.histFlattenMin += 3;
    driver.pPipeline->sSharpening.ui8Detail += 7;
    driver.pPipeline->sEncoderScaler.aPitch[0] += 5;
    // add VSS
    driver.pPipeline->sDisplayScaler.aOffset[1] += 6;
    // add HUS
    driver.pPipeline->sYCCToRGB.aOffset[1] += 2;
    driver.pPipeline->sDisplayGamutMapper.aCoeff[4] += 7;
    // statistics
    driver.pPipeline->sExposureStats.ui16PixelMax += 2;
    driver.pPipeline->sFocusStats.ui16Left += 3;
    driver.pPipeline->sWhiteBalanceStats.aYMax[0] += 5;
    driver.pPipeline->sHistStats.ui16Height += 4;
    driver.pPipeline->sFlickerStats.ui32SceneChange += 3;
    driver.pPipeline->sEncStats.ui8Log2NCouples += 1;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_BLC;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sBlackCorrection.ui16PixelMax += 1; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_LSH;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sDeshading.aGradientsX[0] += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_WBC;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sWhiteBalance.aGain[2] += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_DNS;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sDenoiser.bCombineChannels = driver.pPipeline->sDenoiser.bCombineChannels == IMG_FALSE ? IMG_TRUE : IMG_FALSE;
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_DPF;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sDefectivePixels.ui8PositiveThreshold += 4; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_LCA;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sChromaAberration.aCoeffBlue[2][1] += 3; // so it won't be equal anymore
    if(HasFailure()) return;

    // add DMO

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_CCM;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sColourCorrection.aOffset[2] += 4; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_MGM;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sMainGamutMapper.aSlope[1] += 5; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_GMA;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sGammaCorrection.bBypass = IMG_FALSE; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_R2Y;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sRGBToYCC.aCoeff[1][2] += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_MIE;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sImageEnhancer.aGaussCB[0] += 3; // so it won't be equal anymore
    if(HasFailure()) return;
    // add HSS

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_TNM;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sToneMapping.histFlattenMin += 3; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_SHA;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sSharpening.ui8Detail += 3; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_ESC;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sEncoderScaler.aPitch[0] += 5; // so it won't be equal anymore
    if(HasFailure()) return;

    // add VSS

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_DSC;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sDisplayScaler.aOffset[1] += 6; // so it won't be equal anymore
    if(HasFailure()) return;

    // add HUS

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_Y2R;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sYCCToRGB.aOffset[1] += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_DGM;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sDisplayGamutMapper.aCoeff[4] += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    // statistics

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_EXS;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sExposureStats.ui16PixelMax += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_FOS;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sFocusStats.ui16Left += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_WBS;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sWhiteBalanceStats.aYMax[0] += 6; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_HIS;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sHistStats.ui16Height += 2; // so it won't be equal anymore
    if(HasFailure()) return;

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_FLD;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
    driver.pPipeline->sFlickerStats.ui32SceneChange += 5; // so it won't be equal anymore
    if(HasFailure()) return;

    /*driver.pPipeline->ui8PrivateValue++;
      eUpd = CI_UPD_ENS;
      updated = CI_UPD_NONE;
      ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
      testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
      EXPECT_EQ(CI_UPD_NONE, updated);
      driver.pPipeline->sEncStats.ui8Log2NCouples += 1; // so it won't be equal anymore
      if(HasFailure()) return;*/

    // ALL

    driver.pPipeline->ui8PrivateValue++;
    eUpd = CI_UPD_ALL;
    updated = CI_UPD_NONE;
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, eUpd, &updated));
    testUpdate(driver.pPipeline, &(pKrnConfig->userPipeline), eUpd);
    EXPECT_EQ(CI_UPD_NONE, updated);
}

TEST(CI_Config, update_register)
{
    FullFelix driver; // init the drivers
    KRN_CI_PIPELINE *pKrnPipeline = NULL;
    int updated = CI_UPD_NONE;
    IMG_UINT32 tmp = 0, field = 0;

    driver.configure();

    pKrnPipeline = driver.getKrnPipeline();

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    // check the registers are updated - should work: no pending frames
    driver.pPipeline->sDeshading.ui16SkipY += 2;
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, &tmp);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y, driver.pPipeline->sDeshading.ui16SkipY);
    testUpdateRegister(FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, tmp, CI_UPD_LSH, driver);
    if ( HasFailure() ) FAIL() << "updating LSH";

    driver.pPipeline->sDefectivePixels.sInput.ui16InternalBufSize += 5;
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_DPF_READ_BUF_SIZE_OFFSET, &tmp);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_READ_BUF_SIZE, DPF_READ_BUF_CONTEXT_SIZE, driver.pPipeline->sDefectivePixels.sInput.ui16InternalBufSize);
    testUpdateRegister(FELIX_CONTEXT0_DPF_READ_BUF_SIZE_OFFSET, tmp, CI_UPD_DPF_INPUT, driver);
    if ( HasFailure() ) FAIL() << "updating DPF_INPUT";

    //
    // the pipeline has a pending buffer now
    //
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

    // check the registers are updated - should not work: pending frames
    driver.pPipeline->sDeshading.ui16SkipY += 2;
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, &tmp);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y, driver.pPipeline->sDeshading.ui16SkipY);
    testUpdateRegister(FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, tmp, CI_UPD_LSH, driver);
    if ( HasFailure() ) FAIL() << "updating LSH";

    driver.pPipeline->sDefectivePixels.sInput.ui16InternalBufSize += 5;
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_DPF_READ_BUF_SIZE_OFFSET, &tmp);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_READ_BUF_SIZE, DPF_READ_BUF_CONTEXT_SIZE, driver.pPipeline->sDefectivePixels.sInput.ui16InternalBufSize);
    testUpdateRegister(FELIX_CONTEXT0_DPF_READ_BUF_SIZE_OFFSET, tmp, CI_UPD_DPF_INPUT, driver);
    if ( HasFailure() ) FAIL() << "updating DPF_INPUT";

    //
    // fake interrupt
    //
    ASSERT_EQ(IMG_SUCCESS, Felix::fakeInterrupt(pKrnPipeline));
    // no more pending!

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, &tmp);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y, driver.pPipeline->sDeshading.ui16SkipY);
    testUpdateRegister(FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, tmp, CI_UPD_LSH, driver);
    if ( HasFailure() ) FAIL() << "updating LSH";

    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_DPF_READ_BUF_SIZE_OFFSET, &tmp);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, DPF_READ_BUF_SIZE, DPF_READ_BUF_CONTEXT_SIZE, driver.pPipeline->sDefectivePixels.sInput.ui16InternalBufSize);
    testUpdateRegister(FELIX_CONTEXT0_DPF_READ_BUF_SIZE_OFFSET, tmp, CI_UPD_DPF_INPUT, driver);
    if ( HasFailure() ) FAIL() << "updating DPF_INPUT";
}

TEST(CI_Config, update_notstarted)
{
    FullFelix driver; // init the drivers
    KRN_CI_PIPELINE *pKrnPipeline = NULL;
    int updated = CI_UPD_NONE;
    IMG_UINT32 tmp = 0, field = 0;

    driver.configure();

    pKrnPipeline = driver.getKrnPipeline();


    // make sure the registers do not contain the step value
    tmp = 0;
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_OFFSET_Y, driver.pPipeline->sDeshading.ui16OffsetY+2);
    REGIO_WRITE_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y, driver.pPipeline->sDeshading.ui16SkipY+2);
    TALREG_WriteWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, tmp);

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, CI_UPD_LSH, &updated));
    EXPECT_EQ(CI_UPD_NONE, updated);

    // registers should not have been updated
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y);
    EXPECT_NE(driver.pPipeline->sDeshading.ui16SkipY, field);

    // starting writes to registers and device memory
    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    // registers should not have been updated
    TALREG_ReadWord32(g_psCIDriver->sTalHandles.hRegFelixContext[0], FELIX_CONTEXT0_LSH_ALIGNMENT_Y_OFFSET, &tmp);
    field = REGIO_READ_FIELD(tmp, FELIX_CONTEXT0, LSH_ALIGNMENT_Y, LSH_SKIP_Y);
    EXPECT_EQ(driver.pPipeline->sDeshading.ui16SkipY, field);
}

/**
 * @brief Tests that submitted frames are updated
 */
TEST(CI_Config, update_asap)
{
    FullFelix driver; // init the drivers
    KRN_CI_PIPELINE *pKrnPipeline = NULL;
    int updated = CI_UPD_NONE;
    IMG_UINT32 tmp = 0, field = 0;

    driver.configure();

    pKrnPipeline = driver.getKrnPipeline();

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

    ASSERT_EQ(CI_UPD_ALL, pKrnPipeline->eLastUpdated) << "when starting the whole pipeline should be loaded";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

    ASSERT_EQ(CI_UPD_NONE, pKrnPipeline->eLastUpdated) << "the 1st frame submission should have cleared the flags";

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

    // now change config
    driver.pPipeline->sBlackCorrection.bRowAverage = IMG_TRUE;
    driver.pPipeline->sBlackCorrection.ui16RowReciprocal += 5;

    // apply on no pending frames
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineUpdate(driver.pPipeline, CI_UPD_BLC, NULL));

    EXPECT_EQ(CI_UPD_BLC, pKrnPipeline->eLastUpdated) << "should have updated the flag until a new frame is submitted";

    // now verify it was not applied
    {
        sCell_T *pCurrentCell = NULL;
        KRN_CI_SHOT *pCurrent = NULL;
        IMG_UINT32 reg = 0, field = 0;

        // not safe without the lock but nothing else is running
        pCurrentCell = List_getHead(&(pKrnPipeline->sList_pending));
        ASSERT_TRUE(pCurrentCell != NULL);

        while ( (pCurrentCell = List_getNext(pCurrentCell)) != NULL )
        {
            pCurrent = (KRN_CI_SHOT*)pCurrentCell->object;

            SYS_MemReadWord(&(pCurrent->hwLoadStructure), FELIX_LOAD_STRUCTURE_BLACK_ANALYSIS_OFFSET, &reg);
            field = REGIO_READ_FIELD(reg, FELIX_LOAD_STRUCTURE, BLACK_ANALYSIS, BLACK_ROW_RECIPROCAL);

            EXPECT_NE(driver.pPipeline->sBlackCorrection.ui16RowReciprocal, field);
        }
    }

    // apply a second change
    driver.pPipeline->sChromaAberration.aDec[0] += 2;

    // apply on all submitted frames but the 1st one
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineUpdateASAP(driver.pPipeline, CI_UPD_LCA, NULL));

    EXPECT_EQ(CI_UPD_BLC|CI_UPD_LCA, pKrnPipeline->eLastUpdated) << "should have cummulated the flag until a new frame is submitted";

    // now verify it was applied on all submitted frames but the 1st one
    {
        sCell_T *pCurrentCell = NULL;
        KRN_CI_SHOT *pCurrent = NULL;
        IMG_UINT32 reg, field;

        // not safe without the lock but nothing else is running
        pCurrentCell = List_getHead(&(pKrnPipeline->sList_pending));
        ASSERT_TRUE(pCurrentCell != NULL);
        pCurrent = (KRN_CI_SHOT*)pCurrentCell->object;

        SYS_MemReadWord(&(pCurrent->hwLoadStructure), FELIX_LOAD_STRUCTURE_BLACK_ANALYSIS_OFFSET, &reg);
        field = REGIO_READ_FIELD(reg, FELIX_LOAD_STRUCTURE, BLACK_ANALYSIS, BLACK_ROW_RECIPROCAL);

        EXPECT_NE(driver.pPipeline->sBlackCorrection.ui16RowReciprocal, field);

        while ( (pCurrentCell = List_getNext(pCurrentCell)) != NULL )
        {
            pCurrent = (KRN_CI_SHOT*)pCurrentCell->object;

            SYS_MemReadWord(&(pCurrent->hwLoadStructure), FELIX_LOAD_STRUCTURE_BLACK_ANALYSIS_OFFSET, &reg);
            field = REGIO_READ_FIELD(reg, FELIX_LOAD_STRUCTURE, BLACK_ANALYSIS, BLACK_ROW_RECIPROCAL);

            EXPECT_EQ(driver.pPipeline->sBlackCorrection.ui16RowReciprocal, field);
        }
    }
}

/// @ add invalid test CI_PipelineRegister
/// @ add test CI_PipelineVerify and its invalid one

TEST(CI_Config, dpf_write)
{
    FullFelix driver;
    CI_PIPELINE *pConfig = NULL;
    CI_SHOT *pShot = NULL;

    driver.configure();

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pConfig, driver.getConnection()));
    EXPECT_EQ(0, driver.pPipeline->ui32DPFWriteMapSize) << "default dpf output size should be 0";

    memcpy(pConfig, driver.pPipeline, sizeof(CI_PIPELINE)); // get correct configuration

    pConfig->ui32DPFWriteMapSize = DPF_MAP_MAX_PER_LINE*pConfig->sImagerInterface.ui16ImagerSize[1]*DPF_MAP_OUTPUT_SIZE;

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineRegister(pConfig));

    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAddPool(pConfig, 1));
    EXPECT_EQ(IMG_SUCCESS, CI_PipelineAllocateBuffer(pConfig, CI_TYPE_ENCODER, 0, IMG_FALSE, NULL));
    // driver.pPipeline already have a few buffers

    { // run the pipeline that does not have DPF output
        EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(driver.pPipeline));

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(driver.pPipeline));

        EXPECT_EQ(IMG_SUCCESS, Felix::fakeInterrupt(driver.getKrnPipeline()));

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(driver.pPipeline, &pShot));

        EXPECT_TRUE(pShot->pDPFMap == NULL) << "no dpf by default";

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(driver.pPipeline, pShot));
        pShot = NULL;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(driver.pPipeline));
    }
    { // run the pipeline that has DPF output
        EXPECT_EQ(IMG_SUCCESS, CI_PipelineStartCapture(pConfig));

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineTriggerShoot(pConfig));

        EXPECT_EQ(IMG_SUCCESS, Felix::fakeInterrupt(driver.getKrnPipeline(pConfig)));

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineAcquireShot(pConfig, &pShot));

        EXPECT_TRUE(pShot->pDPFMap != NULL) << "should have dpf output";

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineReleaseShot(pConfig, pShot));
        pShot = NULL;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineStopCapture(pConfig));
    }
}

TEST(CI_Config, computeLinestore_invalid)
{
    {
        Felix driver;

        driver.configure();

        EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, CI_PipelineComputeLinestore(NULL)) << "NULL param";
        // @ add one with IIF not configured
    }
}

TEST(CI_Config, computeLinestore)
{
    Felix driver;
    CI_CONNECTION *pConnection = NULL;
    CI_PIPELINE *pPipeline = NULL;
    CI_LINESTORE sOriginalLinestore;
    IMG_SIZE imageWidht = 0;

    driver.configure();

    pConnection = driver.getConnection();

    IMG_MEMCPY(&sOriginalLinestore, &(g_psCIDriver->sLinestoreStatus), sizeof(CI_LINESTORE));

    ASSERT_EQ(IMG_SUCCESS, CI_PipelineCreate(&pPipeline, driver.getConnection()));

    ASSERT_TRUE(pConnection->sHWInfo.ui32MaxLineStore > 1920) << "max linestore too small for 1080!";
    ASSERT_TRUE(pConnection->sHWInfo.context_aMaxWidthMult[1] + 1920u < pConnection->sHWInfo.ui32MaxLineStore) << "ctx1 + 1280 should be possible!";

    {
        // fake a started context from another app that uses bottom of the linestore (but has a gap big enough for 720p)
        g_psCIDriver->sLinestoreStatus.aStart[1] = 0;
        g_psCIDriver->sLinestoreStatus.aSize[1] = pConnection->sHWInfo.context_aMaxWidthMult[1];
        g_psCIDriver->sLinestoreStatus.aActive[1] = IMG_TRUE;

        // try 720p
        imageWidht = 1280;
        pPipeline->ui8Context = 0; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 720;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineComputeLinestore(pPipeline));

        EXPECT_EQ(g_psCIDriver->sLinestoreStatus.aStart[0], g_psCIDriver->sLinestoreStatus.aStart[1] + g_psCIDriver->sLinestoreStatus.aSize[1]);
        EXPECT_GE(g_psCIDriver->sLinestoreStatus.aSize[0], imageWidht);

        // now try again with a bigger image that will not fit (1080p)
        imageWidht = pConnection->sHWInfo.ui32MaxLineStore - g_psCIDriver->sLinestoreStatus.aSize[1] + 2; // +2 so that it does not fit!
        pPipeline->ui8Context = 0; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 1080;

        EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_PipelineComputeLinestore(pPipeline));

        // try to activate the already active context! - should not even check if it fits!
        pPipeline->ui8Context = 1; 
        EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_PipelineComputeLinestore(pPipeline));
    }
    IMG_MEMCPY(&(g_psCIDriver->sLinestoreStatus), &sOriginalLinestore, sizeof(CI_LINESTORE)); // restore original

    {
        // fake a started context from another app that uses top of the linestore (but has a gap big enough for 720p)
        unsigned size = pConnection->sHWInfo.context_aMaxWidthMult[0];
        if (pConnection->sHWInfo.ui32MaxLineStore-size < 1280)
        {
            size = pConnection->sHWInfo.ui32MaxLineStore-1280;
        }
        g_psCIDriver->sLinestoreStatus.aStart[0] = pConnection->sHWInfo.ui32MaxLineStore - size;
        g_psCIDriver->sLinestoreStatus.aSize[0] = size;
        g_psCIDriver->sLinestoreStatus.aActive[0] = IMG_TRUE;
        
        // try 720p
        imageWidht = 1280;
        pPipeline->ui8Context = 1; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 720;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineComputeLinestore(pPipeline));

        EXPECT_EQ(g_psCIDriver->sLinestoreStatus.aStart[1], 0);
        EXPECT_GE(g_psCIDriver->sLinestoreStatus.aSize[1], imageWidht);

        // now try again with a bigger image that will not fit
        imageWidht = pConnection->sHWInfo.ui32MaxLineStore-size +10;
        EXPECT_GE(g_psCIDriver->sHWInfo.context_aMaxWidthMult[1], imageWidht) << "linestore config cannot allow this test"; // if fail modify the linestore default values for NULL interface
        pPipeline->ui8Context = 1; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 1080;

        EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_PipelineComputeLinestore(pPipeline));
    }
    IMG_MEMCPY(&(g_psCIDriver->sLinestoreStatus), &sOriginalLinestore, sizeof(CI_LINESTORE)); // restore original

    {
        // fake a started context from another app in the middle (but has a gap big enough for 1080p at the end and vga at the beginning)
        g_psCIDriver->sLinestoreStatus.aStart[1] = 640;
        g_psCIDriver->sLinestoreStatus.aSize[1] = pConnection->sHWInfo.context_aMaxWidthMult[1]-640;
        g_psCIDriver->sLinestoreStatus.aActive[1] = IMG_TRUE;

        // try 720p
        imageWidht = 1280;
        pPipeline->ui8Context = 0; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 720;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineComputeLinestore(pPipeline));

        EXPECT_EQ(g_psCIDriver->sLinestoreStatus.aStart[0], g_psCIDriver->sLinestoreStatus.aStart[1] + g_psCIDriver->sLinestoreStatus.aSize[1]);
        EXPECT_GE(g_psCIDriver->sLinestoreStatus.aSize[0], imageWidht);

        // now try again with a bigger image that will fit at the beginning (vga)
        imageWidht = 640;
        pPipeline->ui8Context = 0; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 480;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineComputeLinestore(pPipeline));

        EXPECT_EQ(g_psCIDriver->sLinestoreStatus.aStart[0], 0);
        EXPECT_GE(g_psCIDriver->sLinestoreStatus.aSize[0], imageWidht);
    }
    IMG_MEMCPY(&(g_psCIDriver->sLinestoreStatus), &sOriginalLinestore, sizeof(CI_LINESTORE)); // restore original

    {
        // try 720p in context 0
        imageWidht = 1280;
        pPipeline->ui8Context = 0; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 720;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineComputeLinestore(pPipeline));

        EXPECT_EQ(g_psCIDriver->sLinestoreStatus.aStart[0], 0);
        EXPECT_GE(g_psCIDriver->sLinestoreStatus.aSize[0], imageWidht);
    }
    IMG_MEMCPY(&(g_psCIDriver->sLinestoreStatus), &sOriginalLinestore, sizeof(CI_LINESTORE)); // restore original

    {
        // try 720p in context 1
        imageWidht = 1280;
        pPipeline->ui8Context = 1; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 720;

        EXPECT_EQ(IMG_SUCCESS, CI_PipelineComputeLinestore(pPipeline));

        EXPECT_EQ(g_psCIDriver->sLinestoreStatus.aStart[1], 0);
        EXPECT_GE(g_psCIDriver->sLinestoreStatus.aSize[1], imageWidht);
    }
    IMG_MEMCPY(&(g_psCIDriver->sLinestoreStatus), &sOriginalLinestore, sizeof(CI_LINESTORE)); // restore original

#if FELIX_VERSION_MAJ >= 2
    // because HW v1 does not have a difference between aMaxWidthSingle and aMaxWidthMult
    {
        // start context 0 with its maximum size and ensure context cannot start because it would break context 0
        EXPECT_GT(pConnection->sHWInfo.context_aMaxWidthSingle[0], pConnection->sHWInfo.context_aMaxWidthMult[0]) << "linestore config is wrong!"; // change linestore config of the NULL interface if that fails
        g_psCIDriver->sLinestoreStatus.aStart[0] = 0;
        g_psCIDriver->sLinestoreStatus.aSize[0] = pConnection->sHWInfo.context_aMaxWidthSingle[0];
        g_psCIDriver->sLinestoreStatus.aActive[0] = IMG_TRUE;

        imageWidht = 1280;
        pPipeline->ui8Context = 1; // the linestore needs the IIF to be configured
        pPipeline->sImagerInterface.ui16ImagerSize[0] = (imageWidht)/2 -1; // half the linestore converted in CFA (and -1 for the register)
        pPipeline->sImagerInterface.ui16ImagerSize[1] = 720;

        EXPECT_EQ(IMG_ERROR_NOT_SUPPORTED, CI_PipelineComputeLinestore(pPipeline));

        EXPECT_EQ(g_psCIDriver->sLinestoreStatus.aStart[1], sOriginalLinestore.aStart[1]);
        EXPECT_GE(g_psCIDriver->sLinestoreStatus.aSize[1], sOriginalLinestore.aSize[1]);
    }
    IMG_MEMCPY(&(g_psCIDriver->sLinestoreStatus), &sOriginalLinestore, sizeof(CI_LINESTORE)); // restore original
#endif // FELIX_VERSION_MAJ
}
