/**
 ******************************************************************************
 @file ci_debug.c

 @brief DebugFs functions

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
#include <img_defs.h>
#include <img_errors.h>

#include "ci_kernel/ci_debug.h"
#include "ci_kernel/ci_kernel.h" // access to g_psCIDriver
#include "ci_kernel/ci_hwstruct.h"

#include "ci_kernel/ci_connection.h"
#include "ci_kernel/ci_ioctrl.h" // for struct CI_POOL_PARAM
// to know if we are using tcp when running fake interface
#include "ci_kernel/ci_debug.h"

#include "ci/ci_api_structs.h"
#include "ci/ci_version.h"
#include "felix_hw_info.h" // CI_FATAL

#include "linkedlist.h"
#include "mmulib/mmu.h" // to clean the pages when last connection is removed

#include <tal.h>

#include <registers/fields_core.h>
#include <registers/fields_context0.h>
#include <registers/context0.h> // to clear interrupts before load test
#include <registers/fields_gammalut.h>
#include <registers/test_io.h>
#include <hw_struct/fields_ctx_pointers.h>
#include <hw_struct/fields_save.h>
#include <hw_struct/fields_ctx_config.h>
#include <hw_struct/save.h>
#include <hw_struct/ctx_pointers.h>

#include <registers/core.h>
#include <reg_io2.h>

#ifdef FELIX_HAS_DG
#include <dg_kernel/dg_camera.h>
#include <dg_kernel/dg_debug.h>
#include <registers/ext_data_generator.h> // to check version when starting pdump
#endif

#include <asm/uaccess.h> // copy_to_user and copy_from_user
#include <linux/compiler.h> // for the __user macro
#include <linux/kernel.h> // container_of
#include <linux/errno.h>
#include <linux/ctype.h>
#include <linux/debugfs.h>

#define DEBUGFS_MOD 0555

#ifdef INFOTM_ISP

#ifdef CI_DEBUGFS
typedef struct CI_HW_LIST_ELEMENTS_INFO
{
    int available_elements;
    int pending_elements;
    int processed_elements;
    int send_elements;
} CI_HW_LIST_ELEMENTS_INFO;

typedef struct CI_LAST_FRAME_INFO
{
    IMG_UINT8 lastConfigTag[CI_N_CONTEXT];
    IMG_UINT8 lastContextTag[CI_N_CONTEXT];
    IMG_UINT8 lastContextErros[CI_N_CONTEXT];
    IMG_UINT8 lastContextFrame[CI_N_CONTEXT];

    IMG_UINT32 context_status[CI_N_CONTEXT];
}CI_LAST_FRAME_INFO;

typedef struct CI_DEBUG_FS_ST {
    struct dentry *pDir;

    // RTM info
    struct dentry *pRTMInfo;
#if 0
    struct dentry* pARTMEntries[FELIX_MAX_RTM_VALUES];
    struct dentry* pContextStatus[CI_N_CONTEXT];
    struct dentry* pContextLinkEmptyness[CI_N_CONTEXT];
    struct dentry* pContextPosition[CI_N_CONTEXT][FELIX_MAX_RTM_VALUES];
#endif
    // Interrupt per second
    struct dentry* pINTPS;

    //gasket info
    struct dentry *pActiveGasketInfo;
    struct dentry *pHwListElem;
    struct dentry *pIIFInfo;
    struct dentry *pEncScalerInfo;
    struct dentry *pLastFrameInfo;
} CI_DEBUG_FS_ST;

#define DEBUGFS_RTMINFO "RTMInfo"

#define DEBUGFS_NARTMNENTRIES_S "aRTMEntries%d"
#define DEBUGFS_NCTXSTATUS_S "CTX%dStatus"
#define DEBUGFS_NCTXLINKEMPTYNESS_S "CTX%dLinkEmptyness"
#define DEBUGFS_NCTXNPOSITION_S "CTX%dPosition%d"

#define DEBUGFS_INTERRUPTPERSEC_S "IntPerSec"

#define DEBUGFD_ACTIVEGASKETINFO_S "ActiveGasketInfo"
#define DEBUGFD_HWLISTELEM_S "HwListElements"
#define DEBUGFD_IIFINFO_S "IIFInfo"
#define DEBUGFD_ENCSCALERINFO_S "EncScalerInfo"
#define DEBUGFD_LASTFAMEINFO_S "LastFrameInfo"

typedef enum CI_DEBUG_FS_EN {
    CI_ENUM_RTM_INFO = 0x0,
    CI_ENUM_INT_FPS,
    CI_ENUM_GASKET_INFO,
    CI_ENUM_HWLIST_ELEM,
    CI_ENUM_IFF,
    CI_ENUM_ENC_SCALER,
    CI_ENUM_LAST_FRAEM_INFO
} CI_DEBUG_FS_EN;

/* Show Interrupt per second */
IMG_UINT32 g_u32LastTimestamp[CI_N_CONTEXT]; /* us */
IMG_UINT32 g_u32CurrentTimestamp[CI_N_CONTEXT]; /* us */

IMG_UINT32 g_u32LastIntCnt[CI_N_CONTEXT];
IMG_UINT32 g_u32CurIntCnt[CI_N_CONTEXT];

IMG_UINT32 g_u32INTPS[CI_N_CONTEXT];

CI_RTM_INFO g_stRTMInfo;

CI_HW_LIST_ELEMENTS_INFO g_stHWListElem[CI_N_CONTEXT];

struct CI_GASKET_PARAM g_stGasket[CI_N_IMAGERS];
IMG_UINT8 g_u8ActiveGasketNum = 0;

struct CI_MODULE_IIF g_stIIF[CI_N_CONTEXT];
struct CI_MODULE_SCALER g_stEncScaler[CI_N_CONTEXT];
CI_LAST_FRAME_INFO g_stLastFrameInfo;

CI_DEBUG_FS_ST g_stCIDebugFs;

IMG_RESULT KERN_CI_GetRTMInfo(CI_RTM_INFO *pRTMInfo)
{
    CI_RTM_INFO sRTM;
    IMG_UINT8 i;

    IMG_ASSERT(g_psCIDriver != NULL);
    IMG_ASSERT(pRTMInfo != NULL);

    if (g_psCIDriver->sHWInfo.config_ui8NRTMRegisters > FELIX_MAX_RTM_VALUES)
    {
        // should not happen
        CI_FATAL("not enough values in FELIX_MAX_RTM_VALUES!!!");
        return IMG_ERROR_NOT_SUPPORTED;
    }

    IMG_MEMSET(&sRTM, 0, sizeof(CI_RTM_INFO));

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters; i++)
    {
        sRTM.aRTMEntries[i] = HW_CI_ReadCoreRTM(i);
    }
    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        HW_CI_ReadContextRTM(i, &sRTM);
    }

    IMG_MEMCPY(pRTMInfo, &sRTM, sizeof(sRTM));

    return IMG_SUCCESS;
}

int KERN_CI_GetActiveGasket(int *pActiveGasket)
{
    int i =0;
    int n = 0;
    IMG_ASSERT(pActiveGasket != NULL);

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++,n++)
    {
        if (g_psCIDriver->aActiveCapture[i])
            pActiveGasket[n] = g_psCIDriver->aActiveCapture[i]->iifConfig.ui8Imager;
    }
    return n;
}

IMG_RESULT KERN_CI_GetGasketInfo(
        struct CI_GASKET_PARAM *pGasket, int gasketNum)
{
    struct CI_GASKET_PARAM gasket;

    gasket.uiGasket = gasketNum;
    if (gasket.uiGasket >= g_psCIDriver->sHWInfo.config_ui8NImagers)
    {
        CI_FATAL("gasket %d not available (NGaskets=NImages=%d)\n",
            gasket.uiGasket, g_psCIDriver->sHWInfo.config_ui8NImagers);
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    IMG_MEMSET(&(gasket.sGasketInfo), 0, sizeof(CI_GASKET_INFO));

    HW_CI_ReadGasket(&(gasket.sGasketInfo), gasket.uiGasket);

    IMG_MEMCPY(pGasket, &gasket, sizeof(gasket));

    return IMG_SUCCESS;
}

void KERN_CI_GetHWListElements(IMG_UINT8 context,
        CI_HW_LIST_ELEMENTS_INFO* hwListElem)
{
    KRN_CI_PIPELINE *pPipeline = g_psCIDriver->aActiveCapture[context];
    hwListElem->available_elements = pPipeline->sList_available.ui32Elements;
    hwListElem->pending_elements = pPipeline->sList_pending.ui32Elements;
    hwListElem->processed_elements = pPipeline->sList_processed.ui32Elements;
    hwListElem->send_elements = pPipeline->sList_sent.ui32Elements;
}

void KERN_CI_ReadRegIFF(IMG_UINT8 context, struct CI_MODULE_IIF *pIIF)
{
    IMG_UINT32 value;
    IMG_UINT8 field;
    IMG_HANDLE contextHandle;

    contextHandle = g_psCIDriver->sTalHandles.hRegFelixContext[context];

    //read imagers information
    TALREG_ReadWord32(contextHandle, FELIX_CONTEXT0_IMGR_CTRL_OFFSET, &value);
    field = REGIO_READ_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL, IMGR_BAYER_FORMAT);
    switch(field)
    {
        case FELIX_CONTEXT0_IMGR_BAYER_FORMAT_RGGB:
            pIIF->eBayerFormat = MOSAIC_RGGB;
            break;
        case FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GRBG:
            pIIF->eBayerFormat = MOSAIC_GRBG;
            break;
        case FELIX_CONTEXT0_IMGR_BAYER_FORMAT_GBRG :
            pIIF->eBayerFormat = MOSAIC_GBRG;
            break;
        case FELIX_CONTEXT0_IMGR_BAYER_FORMAT_BGGR :
            pIIF->eBayerFormat = MOSAIC_BGGR;
            break;
        default:
            CI_FATAL("kernel-side mosaic is unknown \n");
    }

    pIIF->ui8Imager = REGIO_READ_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL,
        IMGR_INPUT_SEL);
    pIIF->ui16ScalerBottomBorder = REGIO_READ_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL,
        IMGR_SCALER_BOTTOM_BORDER);
    pIIF->ui16BuffThreshold = REGIO_READ_FIELD(value, FELIX_CONTEXT0, IMGR_CTRL,
        IMGR_BUF_THRESH);

    //read clipping rectangle
    value = 0;
    TALREG_ReadWord32(contextHandle, FELIX_CONTEXT0_IMGR_CAP_OFFSET_OFFSET,
        &value);
    pIIF->ui16ImagerOffset[0] = REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        IMGR_CAP_OFFSET,IMGR_CAP_OFFSET_X);
    pIIF->ui16ImagerOffset[1] = REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        IMGR_CAP_OFFSET,IMGR_CAP_OFFSET_Y);

    //read output size
    value = 0;
    TALREG_ReadWord32(contextHandle, FELIX_CONTEXT0_IMGR_OUT_SIZE_OFFSET,
        &value);
    // warning: number of CFA rows (half nb captured -1)
    pIIF->ui16ImagerSize[1] = REGIO_READ_FIELD(value, FELIX_CONTEXT0, IMGR_OUT_SIZE,
        IMGR_OUT_ROWS_CFA);
    // warning: number of CFA columns (half nb captured -1)
    pIIF->ui16ImagerSize[0] = REGIO_READ_FIELD(value, FELIX_CONTEXT0, IMGR_OUT_SIZE,
        IMGR_OUT_COLS_CFA);

    //read decimation
    value = 0;
    TALREG_ReadWord32(contextHandle,
        FELIX_CONTEXT0_IMGR_IMAGE_DECIMATION_OFFSET, &value);
     pIIF->ui8BlackBorderOffset = REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        IMGR_IMAGE_DECIMATION,BLACK_BORDER_OFFSET);
    pIIF->ui16ImagerDecimation[0] = REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        IMGR_IMAGE_DECIMATION,IMGR_COMP_SKIP_X);
    pIIF->ui16ImagerDecimation[1] = REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        IMGR_IMAGE_DECIMATION,IMGR_COMP_SKIP_Y);

}

void KERN_CI_ReadRegEncScaler(IMG_UINT8 context,
    struct CI_MODULE_SCALER *pENC)
{
    IMG_UINT32 value;
    IMG_HANDLE contextHandle;

    contextHandle = g_psCIDriver->sTalHandles.hRegFelixContext[context];

    //read enc scaler out height
    TALREG_ReadWord32(contextHandle,
        FELIX_CONTEXT0_ENC_SCAL_V_SETUP_OFFSET, &value);
    pENC->aOutputSize[1]= REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        ENC_SCAL_V_SETUP, ENC_SCAL_OUTPUT_ROWS);
    pENC->aOffset[1]= REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        ENC_SCAL_V_SETUP, ENC_SCAL_V_OFFSET);

    //read enc scaler out weight
    TALREG_ReadWord32(contextHandle,
        FELIX_CONTEXT0_ENC_SCAL_H_SETUP_OFFSET, &value);
    pENC->aOutputSize[0]= REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        ENC_SCAL_H_SETUP, ENC_SCAL_OUTPUT_COLUMNS);
    pENC->aOffset[0]= REGIO_READ_FIELD(value, FELIX_CONTEXT0,
        ENC_SCAL_H_SETUP, ENC_SCAL_H_OFFSET);

}

void KERN_CI_ReadRegLastFrameInfo(CI_LAST_FRAME_INFO *frameInfo)
{
    int i;
    IMG_UINT32 thisCTXVal, thisCtxLast;
    IMG_HANDLE contextHandle;

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        contextHandle = g_psCIDriver->sTalHandles.hRegFelixContext[i];

        thisCTXVal = 0;
        TALREG_ReadWord32(contextHandle,
            FELIX_CONTEXT0_CONTEXT_STATUS_OFFSET, &thisCTXVal);
        frameInfo->context_status[i] = thisCTXVal;

        thisCtxLast = 0;
        TALREG_ReadWord32(contextHandle,
            FELIX_CONTEXT0_LAST_FRAME_INFO_OFFSET, &thisCtxLast);

        frameInfo->lastContextErros[i] = REGIO_READ_FIELD(thisCtxLast, FELIX_CONTEXT0,
            LAST_FRAME_INFO, LAST_CONTEXT_ERRORS);
        frameInfo->lastContextFrame[i] = REGIO_READ_FIELD(thisCtxLast, FELIX_CONTEXT0,
            LAST_FRAME_INFO, LAST_CONTEXT_FRAMES);
        frameInfo->lastConfigTag[i] = REGIO_READ_FIELD(thisCtxLast, FELIX_CONTEXT0,
            LAST_FRAME_INFO, LAST_CONFIG_TAG);
        frameInfo->lastContextTag[i] = REGIO_READ_FIELD(thisCtxLast, FELIX_CONTEXT0,
            LAST_FRAME_INFO, LAST_CONTEXT_TAG);

    }
}

void PrintRTMInfoToBuf(char *RTMInfoBuf, IMG_UINT32* sz)
{
    IMG_RESULT ret;
    IMG_UINT8 c, i;
    char *pBuf = RTMInfoBuf;

    *sz = 0;

    ret = KERN_CI_GetRTMInfo(&g_stRTMInfo);
    if (IMG_SUCCESS == ret)
    {
        // config_ui8NRTMRegisters = 8
        for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters; i++)
        {
            sprintf(pBuf+strlen(pBuf), "RTMCore%d:0x%x\n",
                i, g_stRTMInfo.aRTMEntries[i]);
        }

        for (c = 0; c < g_psCIDriver->sHWInfo.config_ui8NContexts; c++)
        {
            sprintf(pBuf+strlen(pBuf), "RTMCtx%dStatus:0x%x\n", c,
                    g_stRTMInfo.context_status[c]);
            sprintf(pBuf+strlen(pBuf), "RTMCtx%dLinkedListEmptyness:0x%x\n",
                    c, g_stRTMInfo.context_linkEmptyness[c]);

            for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters; i++)
            {
                sprintf(pBuf+strlen(pBuf), "RTMCtx%dPos%d:0x%x\n",
                        c, i,g_stRTMInfo.context_position[c][i]);
            }
        }
    }
    else
    {
        CI_FATAL("failed to get RTM information\n");
    }

    *sz = strlen(pBuf);
    //printk("sz=%d, %s\n", *sz, pBuf);
}

void PrintGasketInfoToBuf(char *GasketInfoBuf,
    IMG_UINT32* sz)
{
    IMG_RESULT ret;
    IMG_UINT8 i;
    char *pBuf = GasketInfoBuf;

    int ActiveGasketIndex[CI_N_IMAGERS] = {0};
    IMG_UINT8 gasketNum;

    *sz = 0;
    gasketNum =KERN_CI_GetActiveGasket(ActiveGasketIndex);
    g_u8ActiveGasketNum = gasketNum;

    for (i = 0; i < gasketNum; i++)
    {
        ret = KERN_CI_GetGasketInfo(&g_stGasket[i], ActiveGasketIndex[i]);
        if (IMG_SUCCESS == ret)
        {
            //sprintf(pBuf+strlen(pBuf), "Gasket:%d\n", g_stGasket[i].uiGasket);

            if (g_stGasket[i].sGasketInfo.eType&CI_GASKET_PARALLEL)
            {
                sprintf(pBuf+strlen(pBuf),
                    "Gasket%dStatus:1\n"
                    "Gasket%dType:DVP\n"
                    "Gasket%dFrameCount:%d\n",
                    i, i, i, g_stGasket[i].sGasketInfo.ui32FrameCount);
            }
            else if (g_stGasket[i].sGasketInfo.eType&CI_GASKET_MIPI)
            {
                sprintf(pBuf+strlen(pBuf), "Gasket%dStatus:1\n"
                    "Gasket%dType:MIPI\n"
                    "Gasket%dFrameCount:%d\n",
                    i, i, i, g_stGasket[i].sGasketInfo.ui32FrameCount);

                sprintf(pBuf+strlen(pBuf), "Gasket%dMipiFifoFull:%d\n"
                    "Gasket%dEnabledLanes:%d\n",
                    i, g_stGasket[i].sGasketInfo.ui8MipiFifoFull,
                    i, g_stGasket[i].sGasketInfo.ui8MipiEnabledLanes);

                sprintf(pBuf+strlen(pBuf), "Gasket%dMipiCrcError:%d\n"
                    "Gasket%dMipiHdrError:%d\n",
                    i, g_stGasket[i].sGasketInfo.ui8MipiCrcError,
                    i, g_stGasket[i].sGasketInfo.ui8MipiHdrError);

                sprintf(pBuf+strlen(pBuf), "Gasket%dMipiEccError:%d\n"
                    "Gasket%dMipiEccCorrected:%d\n",
                    i, g_stGasket[i].sGasketInfo.ui8MipiEccError,
                    i, g_stGasket[i].sGasketInfo.ui8MipiEccCorrected);
            }
            else
            {
                sprintf(pBuf+strlen(pBuf), "Gasket%dStatus:0\n", i);
            }

        }
        else
        {
             CI_FATAL("Failed to get Gasket%d information\n", g_stGasket[i].uiGasket);
        }
    }

    *sz = strlen(pBuf);
    //printk("sz=%d, %s\n", *sz, pBuf);
}

void PrintHwListElem(char *HwListElemBuf, IMG_UINT32* sz)
{
    int i =0;
    char *pBuf = HwListElemBuf;
   *sz = 0;

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        KERN_CI_GetHWListElements(i, &g_stHWListElem[i]);

         sprintf(pBuf+strlen(pBuf), "Ctx%dAvailable:%d,Ctx%dPending:%d,"
            "Ctx%dProcessed:%d,Ctx%dSend:%d\n",
            i, g_stHWListElem[i].available_elements,
            i, g_stHWListElem[i].pending_elements,
            i, g_stHWListElem[i].processed_elements,
            i, g_stHWListElem[i].send_elements);
    }

    *sz = strlen(pBuf);
    //printk("sz=%d, %s\n", *sz, pBuf);
}

const IMG_CHAR* eBayerFormat2String(enum MOSAICType eBayerFormat)
{
    switch(eBayerFormat)
    {
    case MOSAIC_RGGB:
        return "RGGB";
    case MOSAIC_GRBG:
        return "GRBG";
    case MOSAIC_GBRG:
        return "GBRG";
    case MOSAIC_BGGR:
       return "BGGR";
    default:
        return "None";
    }
}

void PrintIIFInfoToBuf(char* pIIFInfoBuf, IMG_UINT32* sz)
{
    int i =0;
    char *pBuf = pIIFInfoBuf;
    *sz = 0;

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        KERN_CI_ReadRegIFF(i, &g_stIIF[i]);

        sprintf(pBuf+strlen(pBuf),
            "Ctx%dImager:%d\n"
            "Ctx%deBayerFormat:%s\n"
            "Ctx%dScalerBottomBorder:%d\n"
            "Ctx%dBuffThreshold:%d\n",
            i, g_stIIF[i].ui8Imager,
            i, eBayerFormat2String(g_stIIF[i].eBayerFormat),
            i, g_stIIF[i].ui16ScalerBottomBorder,
            i, g_stIIF[i].ui16BuffThreshold);

        sprintf(pBuf+strlen(pBuf),
            "Ctx%dImagerOffsetX:%d\n"
            "Ctx%dImagerOffsetY:%d\n"
            "Ctx%dImagerSizeRow:%d\n"
            "Ctx%dImagerSizeCol:%d\n",
            i, g_stIIF[i].ui16ImagerOffset[0],
            i, g_stIIF[i].ui16ImagerOffset[1],
            i, 2*g_stIIF[i].ui16ImagerSize[0]+1,
            i, 2*g_stIIF[i].ui16ImagerSize[1]+1);

        sprintf(pBuf+strlen(pBuf),
            "Ctx%dBlackBorderOffset:%d\n"
            "Ctx%dImagerDecimationX:%d\n"
            "Ctx%dImagerDecimationY:%d\n",
            i, g_stIIF[i].ui8BlackBorderOffset,
            i, g_stIIF[i].ui16ImagerDecimation[0],
            i, g_stIIF[i].ui16ImagerDecimation[1]);
    }

    *sz = strlen(pBuf);
    //printk("sz=%d, %s\n", *sz, pBuf);
}

void PrintEncScalerInfoToBuf(char* pIIFInfoBuf, IMG_UINT32* sz)
{
    int i =0;
    char *pBuf = pIIFInfoBuf;
    *sz = 0;

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        KERN_CI_ReadRegEncScaler(i, &g_stEncScaler[i]);

        sprintf(pBuf+strlen(pBuf),
            "Ctx%dOutputSizeH:%d\n"
            "Ctx%dOutputSizeV:%d\n"
            "Ctx%dOffsetH:%d\n"
            "Ctx%dOffsetV:%d\n",
            i, 2*g_stEncScaler[i].aOutputSize[0]+2,
            i, g_stEncScaler[i].aOutputSize[1]+1,
            i, g_stEncScaler[i].aOffset[0],
            i, g_stEncScaler[i].aOffset[1]);
    }

    *sz = strlen(pBuf);
    //printk("sz=%d, %s\n", *sz, pBuf);
}

void PrintLastFrameInfoToBuf(char* pLastFrameInfoBuf,
    IMG_UINT32* sz)
{
    int i =0;
    char *pBuf = pLastFrameInfoBuf;
    *sz = 0;

    KERN_CI_ReadRegLastFrameInfo(&g_stLastFrameInfo);

    for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NContexts; i++)
    {
        sprintf(pBuf+strlen(pBuf),
            "Ctx%dStatus:%d\n"
            "Ctx%dLastConfigTag:%d\n"
            "Ctx%dLastContextTag:%d\n"
            "Ctx%dLastContextErros:%d\n"
            "Ctx%dLastContextFrame:%d\n",
            i, g_stLastFrameInfo.context_status[i],
            i,g_stLastFrameInfo.lastConfigTag[i],
            i,g_stLastFrameInfo.lastContextTag[i],
            i,g_stLastFrameInfo.lastContextErros[i],
            i,g_stLastFrameInfo.lastContextFrame[i]);
    }

    *sz = strlen(pBuf);
    //printk("sz=%d, %s\n", *sz, pBuf);
}

void PrintIntPerSec(char* pIntPerSecBuf, IMG_UINT32 *sz)
{
    IMG_UINT8 c = 0;
    IMG_UINT32 timeDiff;
    char *pBuf = pIntPerSecBuf;

    *sz = 0;

    for (c = 0; c < g_psCIDriver->sHWInfo.config_ui8NContexts; c++)
    {
        if (g_u32LastTimestamp[c] == 0)
        {
           g_u32LastTimestamp[c] = jiffies_to_usecs(jiffies);
           g_u32LastIntCnt[c] = g_ui32NCTXInt[c];
        }

        g_u32CurrentTimestamp[c] = jiffies_to_usecs(jiffies);

        if (g_u32LastTimestamp[c] != g_u32CurrentTimestamp[c])
        {
            if (g_u32CurrentTimestamp[c] > g_u32LastTimestamp[c])
                timeDiff = (g_u32CurrentTimestamp[c] - g_u32LastTimestamp[c]);
            else
                timeDiff = (g_u32CurrentTimestamp[c] + (0xffffffff - g_u32LastTimestamp[c]));
            g_u32INTPS[c] =((g_ui32NCTXInt[c] - g_u32LastIntCnt[c]) * 1000 *1000)/timeDiff;

            g_u32LastTimestamp[c] = g_u32CurrentTimestamp[c];
            g_u32LastIntCnt[c] = g_ui32NCTXInt[c];
        }
        else
        {
            g_u32INTPS[c] = 0;
        }

        sprintf(pBuf+strlen(pBuf),"Ctx%dInt:%d\nCtx%dIps:%d\n",
            c, g_ui32NCTXInt[c], c,g_u32INTPS[c]);
    }

    *sz = strlen(pBuf);
    //printk("sz=%d, %s\n", *sz, pBuf);
}

static ssize_t DebugFs_RTMInfoRead(struct file *,
    char __user *, size_t , loff_t *);

static ssize_t DebugFs_RTMInfoWrite(struct file *,
    const char __user *, size_t , loff_t *);

static ssize_t DebugFs_IntPSRead(struct file *,
    char __user *, size_t , loff_t *);

static ssize_t DebugFs_IntPSWrite(struct file *,
    const char __user *, size_t , loff_t *);

static ssize_t DebugFs_ActiveGasketInfoRead(struct file *,
    char __user *, size_t , loff_t *);

static ssize_t DebugFs_ActiveGasketInfoWrite(struct file *,
    const char __user *, size_t , loff_t *);

static ssize_t DebugFs_HwListElemRead(struct file *,
    char __user *, size_t , loff_t *);

static ssize_t DebugFs_HwListElemWrite(struct file *,
    const char __user *, size_t , loff_t *);

static ssize_t DebugFs_IIFInfoRead(struct file *,
    char __user *, size_t , loff_t *);

static ssize_t DebugFs_IIFInfoWrite(struct file *,
    const char __user *, size_t , loff_t *);

static ssize_t DebugFs_EncScalerInfoRead(struct file *,
    char __user *, size_t , loff_t *);

static ssize_t DebugFs_EncScalerInfoWrite(struct file *,
    const char __user *, size_t , loff_t *);

static ssize_t DebugFs_LastFrameInfoRead(struct file *,
    char __user *, size_t , loff_t *);

static ssize_t DebugFs_LastFrameInfoWrite(struct file *,
    const char __user *, size_t , loff_t *);

static const struct file_operations DebugFs_RTMInfoOps = {
    .open        = nonseekable_open,
    .read        = DebugFs_RTMInfoRead,
    .write        = DebugFs_RTMInfoWrite,
    .llseek        = no_llseek,
};

static const struct file_operations DebugFs_IntPSOps = {
    .open        = nonseekable_open,
    .read        = DebugFs_IntPSRead,
    .write        = DebugFs_IntPSWrite,
    .llseek        = no_llseek,
};

static const struct file_operations DebugFs_ActiveGasketInfoOps = {
    .open        = nonseekable_open,
    .read        = DebugFs_ActiveGasketInfoRead,
    .write        = DebugFs_ActiveGasketInfoWrite,
    .llseek        = no_llseek,
};

const struct file_operations DebugFs_HwListElemOps = {
    .open        = nonseekable_open,
    .read        = DebugFs_HwListElemRead,
    .write        = DebugFs_HwListElemWrite,
    .llseek        = no_llseek,
};

static const struct file_operations DebugFs_IIFInfoOps = {
    .open        = nonseekable_open,
    .read        = DebugFs_IIFInfoRead,
    .write        = DebugFs_IIFInfoWrite,
    .llseek        = no_llseek,
};

static const struct file_operations DebugFs_EncScalerInfoOps = {
    .open        = nonseekable_open,
    .read        = DebugFs_EncScalerInfoRead,
    .write        = DebugFs_EncScalerInfoWrite,
    .llseek        = no_llseek,
};

static const struct file_operations DebugFs_LastFrameInfoOps = {
    .open        = nonseekable_open,
    .read        = DebugFs_LastFrameInfoRead,
    .write        = DebugFs_LastFrameInfoWrite,
    .llseek        = no_llseek,
};

/* atomic operations */
static ssize_t DebugFS_Read(char __user *buff,
    size_t count, loff_t *ppos, CI_DEBUG_FS_EN type)
{
    char *str;
    ssize_t len = 0;

    //printk("%s buf count=%d\n", __func__, count);

    str = kzalloc(count, GFP_KERNEL);
    if (!str)
        return -ENOMEM;

    switch (type) {
    case CI_ENUM_RTM_INFO:
        PrintRTMInfoToBuf(str, &len);
        break;

    case CI_ENUM_GASKET_INFO:
        PrintGasketInfoToBuf(str,&len);
        break;

    case CI_ENUM_IFF:
        PrintIIFInfoToBuf(str, &len);
        break;

    case CI_ENUM_ENC_SCALER:
        PrintEncScalerInfoToBuf(str, &len);
        break;

    case CI_ENUM_LAST_FRAEM_INFO:
        PrintLastFrameInfoToBuf(str, &len);
        break;

    case CI_ENUM_HWLIST_ELEM:
        PrintHwListElem(str,&len);
        break;

    case CI_ENUM_INT_FPS:
        PrintIntPerSec(str, &len);
        break;

    default:
        break;
    }

    if (len < 0)
        CI_FATAL("Can't read data\n");
    else
        len = simple_read_from_buffer(buff, count, ppos, str, len);

    kfree(str);

    return len;
}

static int DebugFS_Write(const char __user *buff,
     size_t count, CI_DEBUG_FS_EN type)
{
    int err = 0, i = 0, ret = 0;
    char *start, *str;

    str = kzalloc(count, GFP_KERNEL);
    if (!str)
        return -ENOMEM;

    if (copy_from_user(str, buff, count)) {
        ret = -EFAULT;
        goto exit_dbgfs_write;
    }

    start = str;

    while (*start == ' ')
        start++;

    /* strip ending whitespace */
    for (i = count - 1; i > 0 && isspace(str[i]); i--)
        str[i] = 0;

    switch (type) {
    case CI_ENUM_RTM_INFO:
    case CI_ENUM_INT_FPS:
    case CI_ENUM_GASKET_INFO:
    case CI_ENUM_HWLIST_ELEM:
    case CI_ENUM_IFF:
    case CI_ENUM_ENC_SCALER:
    case CI_ENUM_LAST_FRAEM_INFO:
    default:
        break;
    }

    if (err) {
        CI_FATAL("error\n");
        ret = -1;
    }

exit_dbgfs_write:
    kfree(str);
    return ret;
}

static ssize_t DebugFs_RTMInfoRead(struct file *file, char __user *buff,
                size_t count, loff_t *ppos)
{
    ssize_t len = 0;

    if (*ppos < 0 || !count)
        return -EINVAL;

    len = DebugFS_Read(buff, count, ppos, CI_ENUM_RTM_INFO);
    return len;
}

static ssize_t DebugFs_RTMInfoWrite(struct file *file, const char __user *buff,
                size_t count, loff_t *ppos)
{
    ssize_t ret = 0;
    int err = 0;

    ret = count;
    if (*ppos < 0 || !count)
        return -EINVAL;

    err = DebugFS_Write(buff, count, CI_ENUM_RTM_INFO);
    if (err < 0)
        return err;

    *ppos += ret;
    return ret;
}

static ssize_t DebugFs_IntPSRead(struct file *file, char __user *buff,
                size_t count, loff_t *ppos)
{
    ssize_t len = 0;

    if (*ppos < 0 || !count)
        return -EINVAL;

    len = DebugFS_Read(buff, count, ppos, CI_ENUM_INT_FPS);
    return len;
}

static ssize_t DebugFs_IntPSWrite(struct file *file, const char __user *buff,
                size_t count, loff_t *ppos)
{
    ssize_t ret = 0;
    int err = 0;

    ret = count;
    if (*ppos < 0 || !count)
        return -EINVAL;

    err = DebugFS_Write(buff, count, CI_ENUM_INT_FPS);
    if (err < 0)
        return err;

    *ppos += ret;
    return ret;
}

static ssize_t DebugFs_ActiveGasketInfoRead(struct file * file,
    char __user * buff,size_t count,loff_t * ppos)
{
    ssize_t len = 0;

    if (*ppos < 0 || !count)
        return -EINVAL;

    len = DebugFS_Read(buff, count, ppos, CI_ENUM_GASKET_INFO);
    return len;
}

static ssize_t DebugFs_ActiveGasketInfoWrite(struct file *file,
    const char __user *buff, size_t count, loff_t *ppos)
{
    ssize_t ret = 0;
    int err = 0;

    ret = count;
    if (*ppos < 0 || !count)
        return -EINVAL;

    err = DebugFS_Write(buff, count, CI_ENUM_GASKET_INFO);
    if (err < 0)
        return err;

    *ppos += ret;
    return ret;
}

static ssize_t DebugFs_HwListElemRead(struct file * file,
    char __user * buff,size_t count,loff_t * ppos)
{
    ssize_t len = 0;

    if (*ppos < 0 || !count)
        return -EINVAL;

    len = DebugFS_Read(buff, count, ppos, CI_ENUM_HWLIST_ELEM);
    return len;
}

static ssize_t DebugFs_HwListElemWrite(struct file *file,
    const char __user *buff, size_t count, loff_t *ppos)
{
    ssize_t ret = 0;
    int err = 0;

    ret = count;
    if (*ppos < 0 || !count)
        return -EINVAL;

    err = DebugFS_Write(buff, count, CI_ENUM_HWLIST_ELEM);
    if (err < 0)
        return err;

    *ppos += ret;
    return ret;
}

static ssize_t DebugFs_IIFInfoRead(struct file * file,
    char __user * buff,size_t count,loff_t * ppos)
{
    ssize_t len = 0;

    if (*ppos < 0 || !count)
        return -EINVAL;

    len = DebugFS_Read(buff, count, ppos, CI_ENUM_IFF);
    return len;
}

static ssize_t DebugFs_IIFInfoWrite(struct file *file,
    const char __user *buff, size_t count, loff_t *ppos)
{
    ssize_t ret = 0;
    int err = 0;

    ret = count;
    if (*ppos < 0 || !count)
        return -EINVAL;

    err = DebugFS_Write(buff, count, CI_ENUM_IFF);
    if (err < 0)
        return err;

    *ppos += ret;
    return ret;
}

static ssize_t DebugFs_EncScalerInfoRead(struct file * file,
    char __user * buff,size_t count,loff_t * ppos)
{
    ssize_t len = 0;

    if (*ppos < 0 || !count)
        return -EINVAL;

    len = DebugFS_Read(buff, count, ppos, CI_ENUM_ENC_SCALER);
    return len;
}

static ssize_t DebugFs_EncScalerInfoWrite(struct file *file,
    const char __user *buff, size_t count, loff_t *ppos)
{
    ssize_t ret = 0;
    int err = 0;

    ret = count;
    if (*ppos < 0 || !count)
        return -EINVAL;

    err = DebugFS_Write(buff, count, CI_ENUM_ENC_SCALER);
    if (err < 0)
        return err;

    *ppos += ret;
    return ret;
}

static ssize_t DebugFs_LastFrameInfoRead(struct file * file,
    char __user * buff,size_t count,loff_t * ppos)
{
    ssize_t len = 0;

    if (*ppos < 0 || !count)
        return -EINVAL;

    len = DebugFS_Read(buff, count, ppos, CI_ENUM_LAST_FRAEM_INFO);
    return len;
}

static ssize_t DebugFs_LastFrameInfoWrite(struct file *file,
    const char __user *buff, size_t count, loff_t *ppos)
{
    ssize_t ret = 0;
    int err = 0;

    ret = count;
    if (*ppos < 0 || !count)
        return -EINVAL;

    err = DebugFS_Write(buff, count, CI_ENUM_LAST_FRAEM_INFO);
    if (err < 0)
        return err;

    *ppos += ret;
    return ret;
}

void ExtDebugFS_Create(struct dentry * pRoot)
{
#if 0
	IMG_UINT8 c, i;
	char name[64];
#endif

    if (!pRoot)
    return ;

    g_stCIDebugFs.pDir = pRoot;

    g_stCIDebugFs.pRTMInfo = debugfs_create_file(DEBUGFS_RTMINFO,
               S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
               g_stCIDebugFs.pDir, NULL,
               &DebugFs_RTMInfoOps);
    if (g_stCIDebugFs.pRTMInfo == NULL)
        CI_FATAL("cannot create debugfs %s\n", DEBUGFS_RTMINFO);

    g_stCIDebugFs.pINTPS= debugfs_create_file(DEBUGFS_INTERRUPTPERSEC_S,
               S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
               g_stCIDebugFs.pDir, NULL,
               &DebugFs_IntPSOps);
    if (g_stCIDebugFs.pINTPS == NULL)
        CI_FATAL("cannot create debugfs %s\n", DEBUGFS_INTERRUPTPERSEC_S);

    g_stCIDebugFs.pActiveGasketInfo= debugfs_create_file(
                DEBUGFD_ACTIVEGASKETINFO_S,
                S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
                g_stCIDebugFs.pDir, NULL,
                &DebugFs_ActiveGasketInfoOps);
    if (g_stCIDebugFs.pActiveGasketInfo == NULL)
        CI_FATAL("cannot create debugfs %s\n", DEBUGFD_ACTIVEGASKETINFO_S);

    g_stCIDebugFs.pHwListElem = debugfs_create_file(
                DEBUGFD_HWLISTELEM_S,
                S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
                g_stCIDebugFs.pDir, NULL,
                &DebugFs_HwListElemOps);
    if (g_stCIDebugFs.pHwListElem == NULL)
        CI_FATAL("cannot create debugfs %s\n", DEBUGFD_HWLISTELEM_S);

    g_stCIDebugFs.pIIFInfo= debugfs_create_file(
                DEBUGFD_IIFINFO_S,
                S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
                g_stCIDebugFs.pDir, NULL,
                &DebugFs_IIFInfoOps);
    if (g_stCIDebugFs.pIIFInfo== NULL)
        CI_FATAL("cannot create debugfs %s\n", DEBUGFD_IIFINFO_S);

    g_stCIDebugFs.pEncScalerInfo= debugfs_create_file(
               DEBUGFD_ENCSCALERINFO_S,
               S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
               g_stCIDebugFs.pDir, NULL,
                &DebugFs_EncScalerInfoOps);
    if (g_stCIDebugFs.pEncScalerInfo == NULL)
        CI_FATAL("cannot create debugfs %s\n", DEBUGFD_ENCSCALERINFO_S);

    g_stCIDebugFs.pLastFrameInfo= debugfs_create_file(
                DEBUGFD_LASTFAMEINFO_S,
                S_IFREG | S_IRUSR | S_IRGRP | S_IWUSR | S_IWGRP,
                g_stCIDebugFs.pDir, NULL,
                &DebugFs_LastFrameInfoOps);
    if (g_stCIDebugFs.pLastFrameInfo == NULL)
        CI_FATAL("cannot create debugfs %s\n", DEBUGFD_LASTFAMEINFO_S);

#if 0
	for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters; i++)
	{
		sprintf(name, DEBUGFS_NARTMNENTRIES_S, i);
		g_stCIDebugFs.pARTMEntries[i] =
			debugfs_create_u32(name, DEBUGFS_MOD,
				g_stCIDebugFs.pDir, &g_stRTMInfo.aRTMEntries[i]);
	}

	for (c = 0; c < g_psCIDriver->sHWInfo.config_ui8NContexts; c++)
	{
		/*1. RTM_INFO*/
		sprintf(name, DEBUGFS_NCTXSTATUS_S, i);
		g_stCIDebugFs.pContextStatus[i] =
			debugfs_create_u32(name, DEBUGFS_MOD,
				g_stCIDebugFs.pDir, &g_stRTMInfo.context_status[c]);

		sprintf(name, DEBUGFS_NCTXLINKEMPTYNESS_S, i);
		g_stCIDebugFs.pContextLinkEmptyness[i] =
			debugfs_create_u32(name, DEBUGFS_MOD,
				g_stCIDebugFs.pDir, &g_stRTMInfo.context_linkEmptyness[c]);

		for (i = 0; i < g_psCIDriver->sHWInfo.config_ui8NRTMRegisters; i++)
		{
			sprintf(name, DEBUGFS_NCTXNPOSITION_S, c, i);
			g_stCIDebugFs.pContextPosition[c][i] =
				debugfs_create_u32(name, DEBUGFS_MOD,
					g_stCIDebugFs.pDir, &g_stRTMInfo.context_position[c][i]);
		}
	}
#endif
}

#endif /* End of CI_DEBUGFS */

#endif /* End for INFOTM */
