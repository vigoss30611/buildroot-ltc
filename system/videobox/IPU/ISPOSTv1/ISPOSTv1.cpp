// Copyright (C) 2016 InfoTM, jazz.chang@infotm.com,
//    yong.yan@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <System.h>
#include "ISPOSTv1.h"
#include "q3ispost.h"
#include "q3denoise.h"

DYNAMIC_IPU(IPU_ISPOSTV1, "ispost");


const ISPOST_UINT32 g_dwGridSize[] = { 8, 16, 32 };
struct ipu_ispost_3d_dn_param g_dnParamList[] = {
    {10, 10, 10, 5},
    {10, 10, 10, 4},
    {10, 10, 10, 3},
    {20, 20, 20, 5},
    {20, 20, 20, 4},
    {20, 20, 20, 3},
    {30, 30, 30, 5},
    {30, 30, 30, 4},
    {30, 30, 30, 3},
    {20, 10, 10, 3},
    {30, 20, 20, 3},
    {20, 20, 20, 2}
};
static ISPOST_UINT32 sMaxLevelCnt = sizeof(g_dnParamList) / sizeof(g_dnParamList[0]);

void IPU_ISPOSTV1::WorkLoop()
{
    Buffer BufIn, BufOl, BufDn, BufSs0, BufSs1;
    int ss0 = -1, ss1 = -1, dn = -1;


    while (IsInState(ST::Running)){

        InEnableFlag = m_pPortIn->IsEnabled();
        DnEnableFlag = m_pPortDn->IsEnabled() || m_IspostUser.stCtrl0.ui1EnDN;
        SS0EnableFlag = m_pPortSs0->IsEnabled();
        SS1EnableFlag = m_pPortSs1->IsEnabled();
        OlEnableFlag = m_pPortOl->IsEnabled() && (m_IspostUser.ui32IspostCount > 0);
        OlAvailable = false;

        // update hw from port settings
        if(ss0 != SS0EnableFlag || ss1 != SS1EnableFlag || dn != DnEnableFlag)
        {
            if (!IspostInitial()){
                LOGE("IspostInitial FAIL!\n");
                throw VBERR_BADPARAM;
            }
            ss0 = SS0EnableFlag;
            ss1 = SS1EnableFlag;
            dn = DnEnableFlag;
        }

        try{
            if (OlEnableFlag){
                BufOl = m_pPortOl->GetBuffer();
                OlAvailable = true;
            }
        }catch(const char *err){
            if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                LOGE("videobox interrupted out 2.\n");
                return;
            }
        }

        try{
            if (InEnableFlag){
                if (m_pPortDn->IsEmbezzling() || m_pPortSs0->IsEmbezzling() || m_pPortSs1->IsEmbezzling())
                    BufIn = m_pPortIn->GetBuffer();
                else
                    BufIn = m_pPortIn->GetBuffer(&BufIn);
            }
        }catch(const char *err){
            if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                LOGE("videobox interrupted out 2.\n");
                return;
            }
            usleep(20000);
            goto PutOl;
        }

        try{
            if (DnEnableFlag){
                if (m_pPortDn->IsEmbezzling())
                    BufDn = m_pPortDn->GetBuffer(&BufDn);
                else
                    BufDn = m_pPortDn->GetBuffer();
            }
        }catch (const char *err){
            if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                LOGE("videobox interrupted out 2.\n");
                return;
            }
            usleep(20000);
            goto PutIn;
        }

        try{
            if (SS0EnableFlag){
                if (m_pPortSs0->IsEmbezzling())
                    BufSs0 = m_pPortSs0->GetBuffer(&BufSs0);
                else
                    BufSs0 = m_pPortSs0->GetBuffer();
            }
        }catch (const char *err){
            if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                LOGE("videobox interrupted out 2.\n");
                return;
            }
            usleep(20000);
            goto PutDn;
        }

        try{
            if (SS1EnableFlag){
                if (m_pPortSs1->IsEmbezzling())
                    BufSs1 = m_pPortSs1->GetBuffer(&BufSs1);
                else
                    BufSs1 = m_pPortSs1->GetBuffer();
            }
        }catch (const char *err){
            if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                LOGE("videobox interrupted out 2.\n");
                return;
            }
            usleep(20000);
            goto PutSS0;
        }

        if (DnEnableFlag)
            IspostProcPrivateData((ipu_v2500_private_data_t*)BufIn.fr_buf.priv);

        if (!IspostTrigger(BufIn, BufOl, BufDn, BufSs0, BufSs1)){
            LOGE("Trigger fial.\n");
            break;
        }

        if (SS1EnableFlag){
            BufSs1.SyncStamp(&BufIn);
            m_pPortSs1->PutBuffer(&BufSs1);
        }

PutSS0:
        if (SS0EnableFlag){
            BufSs0.SyncStamp(&BufIn);
            m_pPortSs0->PutBuffer(&BufSs0);
        }
PutDn:
        if (DnEnableFlag){
            BufDn.SyncStamp(&BufIn);
            m_pPortDn->PutBuffer(&BufDn);
        }
PutIn:
        if (InEnableFlag)
            m_pPortIn->PutBuffer(&BufIn);
PutOl:
        if (OlEnableFlag && OlAvailable)
            m_pPortOl->PutBuffer(&BufOl);
    }
}

IPU_ISPOSTV1::IPU_ISPOSTV1(std::string name, Json *js)
{
    int i, Value;
    char *pszString;
    char szItemName[80];

    m_pJsonMain = js;
    Name = name;//"ispostv1";
    m_pOvImageBuf = NULL;
    m_pOvFontBuf = NULL;
    for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
    {
        m_pszLcGridFileName[i] = NULL;
        m_pLcGridBuf[i] = NULL;
    }
    m_pszOvImgFileName = NULL;
    m_pszOvFontFileName = NULL;
    m_Buffers = 3;
    m_Rotate = grid_target_index_max = 0;
    memset(&m_Dns3dAttr, 0, sizeof(m_Dns3dAttr));
    m_Dns3dAttr.y_threshold = g_dnParamList[0].y_threshold;
    m_Dns3dAttr.u_threshold = g_dnParamList[0].u_threshold;
    m_Dns3dAttr.v_threshold = g_dnParamList[0].v_threshold;
    m_Dns3dAttr.weight = g_dnParamList[0].weight;
    m_eDnsModePrev = CAM_CMN_AUTO;
    m_ui32DnRefImgBufYaddr = 0xFFFFFFFF;
    pthread_mutex_init(&m_PipMutex, NULL);


    IspostDefaultParameter();
    pthread_mutex_init(&m_FceLock, NULL);
    memset(&m_stFceJsonParam, 0, sizeof(m_stFceJsonParam));

    m_s32LcOutImgWidth = m_s32LcOutImgHeight = 0;

#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
    m_bIqTuningDebugInfoEnable = false;
    m_szGridFilePath[0][0] = '\0';
    m_szGridFilePath[1][0] = '\0';

#endif
    //----------------------------------------------------------------------------------------------
    // Parse IPU parameter
    if (m_pJsonMain != NULL)
    {
        if (IspostJsonGetInt("buffers", Value) || IspostJsonGetInt("nbuffers", Value))
            m_Buffers = Value;
        if (IspostJsonGetInt("rotate", Value))
            m_Rotate = 0;

        //----------------------------------------------------------------------------------------------
        // Parse lc_config parameter
        for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
        {
            sprintf(szItemName, "lc_grid_file_name%d", i+1);
            if (IspostJsonGetString(szItemName, pszString)) {
                m_pszLcGridFileName[i] = strdup(pszString);
#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
                if ('\0' == m_szGridFilePath[0][0]) {
                    int iIdx = 0;
                    char *pszTmp;

                    pszTmp = strrchr(m_pszLcGridFileName[i], '/');
                    if (NULL != pszTmp) {
                        iIdx = &pszTmp[0] - &m_pszLcGridFileName[i][0];
                        iIdx++;
                        strncpy(m_szGridFilePath[0], m_pszLcGridFileName[i], (unsigned int)iIdx);
                        m_szGridFilePath[0][iIdx] = '\0';
                    }
                }
#endif
            }
        }

        if (IspostJsonGetInt("lc_enable", Value))
            m_IspostUser.stCtrl0.ui1EnLC = Value;
        if(IspostJsonGetInt("lc_grid_target_index", Value))
            m_IspostUser.ui32LcGridTargetIdx = Value;
        if (IspostJsonGetInt("lc_grid_line_buf_enable", Value))
            m_IspostUser.ui32LcGridLineBufEn = Value;
        if (IspostJsonGetInt("lc_cache_mode_ctrl_config", Value))
            m_IspostUser.ui32LcCacheModeCtrlCfg = Value;
        if (IspostJsonGetInt("lc_cbcr_swap", Value))
            m_IspostUser.ui32LcCBCRSwap = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_w", Value))
            m_IspostUser.ui32LcScanModeScanW = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_h", Value))
            m_IspostUser.ui32LcScanModeScanH = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_m", Value))
            m_IspostUser.ui32LcScanModeScanM = Value;
        if (IspostJsonGetInt("lc_fill_y", Value))
            m_IspostUser.ui32LcFillY = Value;
        if (IspostJsonGetInt("lc_fill_cb", Value))
            m_IspostUser.ui32LcFillCB = Value;
        if (IspostJsonGetInt("lc_fill_cr", Value))
            m_IspostUser.ui32LcFillCR = Value;
        if (IspostJsonGetInt("lc_out_width", Value))
            m_s32LcOutImgWidth = Value;
        if (IspostJsonGetInt("lc_out_height", Value))
            m_s32LcOutImgHeight = Value;

        //----------------------------------------------------------------------------------------------
        // Parse ov_config parameter
        if (IspostJsonGetInt("ov_enable", Value))
            m_IspostUser.stCtrl0.ui1EnOV = Value;
        if (IspostJsonGetInt("ov_mode", Value))
            m_IspostUser.ui32OvMode = Value;
        if (IspostJsonGetInt("ov_img_x", Value))
            m_IspostUser.ui32OvImgOffsetX = Value;
        if (IspostJsonGetInt("ov_img_y", Value))
            m_IspostUser.ui32OvImgOffsetY = Value;
        if (IspostJsonGetInt("ov_img_w", Value))
            m_IspostUser.ui32OvImgWidth = Value;
        if (IspostJsonGetInt("ov_img_h", Value))
            m_IspostUser.ui32OvImgHeight = Value;
        if (IspostJsonGetInt("ov_img_s", Value))
            m_IspostUser.ui32OvImgStride = Value;
        if (IspostJsonGetString("ov_img_file_name", pszString))
            m_pszOvImgFileName = strdup(pszString);
        if (IspostJsonGetString("ov_font_file_name", pszString))
            m_pszOvFontFileName = strdup(pszString);

        //----------------------------------------------------------------------------------------------
        // Parse dn_config parameter
        if (IspostJsonGetInt("dn_enable", Value))
            m_IspostUser.stCtrl0.ui1EnDN = Value;
        if (IspostJsonGetInt("dn_target_index", Value))
            m_IspostUser.ui32DnTargetIdx = Value;
        sprintf(szItemName, "fcefile");
        if (IspostJsonGetString(szItemName, pszString))
        {
            ParseFceFileJson(pszString, &m_stFceJsonParam);
        }
    }

    if (m_Buffers <= 0 || m_Buffers > 20)
    {
        LOGE("the buffers number is over.\n");
        throw VBERR_BADPARAM;
    }

    //----------------------------------------------------------------------------------------------
    // Create ISPOST support port
    m_pPortIn = CreatePort("in", Port::In);
    m_pPortOl = CreatePort("ol", Port::In);
    m_pPortDn = CreatePort("dn", Port::Out);
    m_pPortSs0 = CreatePort("ss0", Port::Out);
    m_pPortSs1 = CreatePort("ss1", Port::Out);

    m_pPortDn->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortDn->SetResolution(1920, 1088);
    m_pPortDn->SetPixelFormat(Pixel::Format::NV12);
    m_pPortDn->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);
    if(m_IspostUser.stCtrl0.ui1EnDN)
        // export PortDn if dn function is enabled, which will
        // make system allocate buffer for PortDn
        m_pPortDn->Export();

    m_pPortSs0->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortSs0->SetResolution(1280, 720);
    m_pPortSs0->SetPixelFormat(Pixel::Format::NV12);
    m_pPortSs0->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    m_pPortSs1->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortSs1->SetResolution(640, 480);
    m_pPortSs1->SetPixelFormat(Pixel::Format::NV12);
    m_pPortSs1->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    m_pPortOl->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    m_pPortOl->SetPixelFormat(Pixel::Format::NV12);

    ispost_open();

    m_pJsonMain = NULL;
}

IPU_ISPOSTV1::~IPU_ISPOSTV1()
{
    int i;

    ispost_close();
    for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
    {
        if (m_pszLcGridFileName[i] != NULL)
            free(m_pszLcGridFileName[i]);
        if (m_pLcGridBuf[i] != NULL)
            delete m_pLcGridBuf[i];
    }
    if (m_pszOvImgFileName != NULL)
        free(m_pszOvImgFileName);
    if (m_pszOvFontFileName != NULL)
        free(m_pszOvFontFileName);

    if (m_pOvFontBuf != NULL)
        delete m_pOvFontBuf;

    if (m_pOvImageBuf != NULL)
        delete m_pOvImageBuf;

    pthread_mutex_destroy(&m_PipMutex);
    pthread_mutex_destroy(&m_FceLock);

}

int IPU_ISPOSTV1::IspostFormatPipInfo(struct v_pip_info *vpinfo)
{
    int dn_w, dn_h;
    int ov_w, ov_h;
    if(strncmp(vpinfo->portname, "ol", 2) != 0)
        return 0;

    dn_w = m_pPortDn->GetWidth() - 10;
    dn_h = m_pPortDn->GetHeight() - 10;

    ov_w = m_pPortOl->GetWidth();
    ov_h = m_pPortOl->GetHeight();

    if (ov_w + vpinfo->x >= dn_w)
        vpinfo->x = dn_w - ov_w ;

    if (ov_h + vpinfo->y >= dn_h)
        vpinfo->y = dn_h - ov_h;

    if (vpinfo->x < 0)
        vpinfo->x = 0;

    if (vpinfo->y < 0)
        vpinfo->y = 0;

    vpinfo->x -= vpinfo->x%4;
    vpinfo->y -= vpinfo->y%4;

    LOGD("FormatPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

    return 0;
}

void IPU_ISPOSTV1::Prepare()
{
    int i;
    Buffer buf;
    char szBufName[20];

    IspostCheckParameter();

    //Load LcGridBuf
    if (0 == m_stFceJsonParam.mode_number)
    {
        for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
        {
            if (m_pszLcGridFileName[i] != NULL)
            {
                sprintf(szBufName, "LcGridBuf%d", i+1);
                m_IspostUser.stCtrl0.ui1EnLC = ISPOST_ENABLE;
                m_IspostUser.ui32LcGridSize = IspostParseGirdSize(m_pszLcGridFileName[i]);
                LOGD("m_pszLcGridFileName[%d]: %s, GridSize: %d\n", i, m_pszLcGridFileName[i], m_IspostUser.ui32LcGridSize);
                IspostLoadBinaryFile(szBufName, &m_pLcGridBuf[i], m_pszLcGridFileName[i]);
            }
        }
    }

    if (m_IspostUser.ui32LcScanModeScanW > m_IspostUser.ui32LcGridSize) {
        m_IspostUser.ui32LcScanModeScanW = m_IspostUser.ui32LcGridSize;
    }
    if (m_IspostUser.ui32LcScanModeScanH > m_IspostUser.ui32LcGridSize) {
        m_IspostUser.ui32LcScanModeScanH = m_IspostUser.ui32LcGridSize;
    }

    if (m_pLcGridBuf[0])
    {
        buf = m_pLcGridBuf[0]->GetBuffer();
        m_IspostUser.ui32LcGridBufAddr = buf.fr_buf.phys_addr;
        m_IspostUser.ui32LcGridBufSize = buf.fr_buf.size;
        m_pLcGridBuf[0]->PutBuffer(&buf);
    }

#ifdef IPU_ISPOST_OVERLAY_TEST
    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
    m_IspostUser.ui32OvMode = OVERLAY_MODE_YUV;
    m_IspostUser.ui32OvImgOffsetX = 0;
    m_IspostUser.ui32OvImgOffsetY = 0;
    m_IspostUser.ui32OvImgWidth = 240;
    m_IspostUser.ui32OvImgHeight = 60;
    m_IspostUser.ui32OvImgStride = 240;

    IspostLoadBinaryFile("OvImageBuf", &m_pOvImageBuf, "/root/.ispost/ov_infotm_240x60.bin");
    buf = m_pOvImageBuf->GetBuffer();
    m_IspostUser.ui32OvImgBufYaddr = buf.fr_buf.phys_addr;
    m_pOvImageBuf->PutBuffer(&buf);
#endif


    m_stFce.maxModeGroupNb = 0;
    m_stFce.hasFce = false;
    m_stFce.stModeGroupAllList.clear();
    m_stFce.modeGroupActiveList.clear();
    m_stFce.modeGroupPendingList.clear();
    m_stFce.modeGroupIdleList.clear();

    m_HasFceData = false;
    m_HasFceFile = false;

    if (m_stFceJsonParam.mode_number)
    {
        m_IspostUser.stCtrl0.ui1EnLC = ISPOST_ENABLE;
        IspostLoadAndFireFceData(&m_stFceJsonParam);
        m_IspostUser.ui32LcGridSize = \
            IspostParseGirdSize(m_stFceJsonParam.mode_list[0].file_list[0].file_grid);
    }
}

void IPU_ISPOSTV1::Unprepare()
{
    if (m_stFce.hasFce)
    {
        FreeFce();
        m_stFce.hasFce = false;
    }
    m_HasFceData = false;
    m_HasFceFile = false;
    if (m_pPortIn->IsEnabled())
    {
         m_pPortIn->FreeVirtualResource();
    }
    if (m_pPortOl->IsEnabled())
    {
         m_pPortOl->FreeVirtualResource();
    }
    if (m_pPortDn->IsEnabled())
    {
         m_pPortDn->FreeVirtualResource();
    }
    if (m_pPortSs0->IsEnabled())
    {
         m_pPortSs0->FreeVirtualResource();
    }
    if (m_pPortSs1->IsEnabled())
    {
         m_pPortSs1->FreeVirtualResource();
    }
}

bool IPU_ISPOSTV1::IspostInitial(void)
{
    // OV ----------------------------------------
    if (m_pPortOl->IsEnabled()){
        m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
        m_IspostUser.ui32OvMode = OVERLAY_MODE_YUV;
        m_IspostUser.ui32OvImgOffsetX = m_pPortOl->GetPipX();
        m_IspostUser.ui32OvImgOffsetY = m_pPortOl->GetPipY();
        m_IspostUser.ui32OvImgWidth = m_pPortOl->GetWidth();
        m_IspostUser.ui32OvImgHeight = m_pPortOl->GetHeight();
        m_IspostUser.ui32OvImgStride = m_pPortOl->GetWidth();
    }

    // DN ----------------------------------------
    if (m_pPortDn->IsEnabled() || m_IspostUser.stCtrl0.ui1EnDN){
        //m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
        m_IspostUser.ui32DnOutImgBufStride = m_pPortDn->GetWidth();
        m_IspostUser.ui32DnRefImgBufStride = m_pPortDn->GetWidth();
    }else{
        //m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
        m_IspostUser.ui32DnOutImgBufYaddr = 0x0;
        m_IspostUser.ui32DnOutImgBufUVaddr = 0x0;
        m_IspostUser.ui32DnRefImgBufYaddr = 0x0;
        m_IspostUser.ui32DnRefImgBufUVaddr = 0x0;
    }

    if(SetupLcInfo() < 0)
    {
        return false;
    }

    // SS0 ----------------------------------------
    if (m_pPortSs0->IsEnabled()){
        m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_ENABLE;
        if (m_pPortSs0->IsEmbezzling()){
            m_IspostUser.ui32SS0OutImgDstWidth = m_pPortSs0->GetPipWidth();
            m_IspostUser.ui32SS0OutImgDstHeight = m_pPortSs0->GetPipHeight();
            m_IspostUser.ui32SS0OutImgBufStride =  m_pPortSs0->GetWidth();
            LOGD("SS0 PipW: %d, PipH: %d, Stride: %d", m_pPortSs0->GetPipWidth(), m_pPortSs0->GetPipHeight(), m_pPortSs0->GetWidth());
        }else{
            m_IspostUser.ui32SS0OutImgDstWidth = m_pPortSs0->GetWidth();
            m_IspostUser.ui32SS0OutImgDstHeight = m_pPortSs0->GetHeight();
            m_IspostUser.ui32SS0OutImgBufStride = m_IspostUser.ui32SS0OutImgDstWidth;
        }
    }else{
        m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_DISABLE;
        m_IspostUser.ui32SS0OutImgBufYaddr = 0xFFFFFFFF;
        m_IspostUser.ui32SS0OutImgBufUVaddr = 0xFFFFFFFF;
    }

    if (m_pPortSs1->IsEnabled()){
        m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_ENABLE;
        if (m_pPortSs1->IsEmbezzling()){
            m_IspostUser.ui32SS1OutImgDstWidth = m_pPortSs1->GetPipWidth();
            m_IspostUser.ui32SS1OutImgDstHeight = m_pPortSs1->GetPipHeight();
            m_IspostUser.ui32SS1OutImgBufStride =  m_pPortSs1->GetWidth();
        }else{
            m_IspostUser.ui32SS1OutImgDstWidth = m_pPortSs1->GetWidth();
            m_IspostUser.ui32SS1OutImgDstHeight = m_pPortSs1->GetHeight();
            m_IspostUser.ui32SS1OutImgBufStride = m_IspostUser.ui32SS1OutImgDstWidth;
        }
    }else{
        m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_DISABLE;
        m_IspostUser.ui32SS1OutImgBufYaddr = 0xFFFFFFFF;
        m_IspostUser.ui32SS1OutImgBufUVaddr = 0xFFFFFFFF;
    }

    m_IspostUser.ui32IspostCount = 0;

    return true;
}


int IPU_ISPOSTV1::UpdateDnUserInfo(bool hasFce, cam_position_t *pstPos, ISPOST_UINT32 *pRefBufYAddr, Buffer *pPortBuf)
{
    int pipX = 0;
    int pipY = 0;


    if (NULL == pRefBufYAddr || NULL == pPortBuf)
    {
        LOGE("(%s:L%d) params %p %p has NULL !!!\n", __func__, __LINE__, pRefBufYAddr, pPortBuf);
        return -1;
    }
    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        LOGD("(%s:%d) DN pipX=%d, pipY=%d %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortDn->IsEmbezzling())
    {
        pipX = m_pPortDn->GetPipX();
        pipY = m_pPortDn->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
    m_IspostUser.ui32DnOutImgBufYaddr = pPortBuf->fr_buf.phys_addr;
    m_IspostUser.ui32DnOutImgBufUVaddr = m_IspostUser.ui32DnOutImgBufYaddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());

    m_IspostUser.ui32DnRefImgBufYaddr = *pRefBufYAddr;
    m_IspostUser.ui32DnRefImgBufUVaddr = m_IspostUser.ui32DnRefImgBufYaddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());
    if (hasFce)
        *pRefBufYAddr = m_IspostUser.ui32DnOutImgBufYaddr;
    if (m_pPortDn->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.ui32DnOutImgBufStride,
                                m_IspostUser.ui32DnOutImgBufYaddr, m_IspostUser.ui32DnOutImgBufUVaddr);

        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.ui32DnRefImgBufStride,
                               m_IspostUser.ui32DnRefImgBufYaddr, m_IspostUser.ui32DnRefImgBufUVaddr);
    }
    if (m_pPortIn->GetPipWidth() != 0 && m_pPortIn->GetPipHeight() != 0)
    {
        IspostCalcOffsetAddress(m_pPortIn->GetPipX(), m_pPortIn->GetPipY(), m_IspostUser.ui32LcSrcStride,
                        m_IspostUser.ui32LcSrcBufYaddr, m_IspostUser.ui32LcSrcBufUVaddr);
    }
    if (!hasFce)
        *pRefBufYAddr = m_IspostUser.ui32DnOutImgBufYaddr;

    return 0;
}

int IPU_ISPOSTV1::UpdateLcUserInfo(bool hasFce, cam_position_t *pstPos)
{
    if (hasFce && pstPos)
    {
        m_IspostUser.ui32LcDstWidth = pstPos->width;
        m_IspostUser.ui32LcDstHeight = pstPos->height;
    }

    return 0;
}

int IPU_ISPOSTV1::UpdateSs0UserInfo(bool hasFce, cam_position_t *pstPos, Buffer *pPortBuf)
{
    int pipX = 0;
    int pipY = 0;


    if (NULL == pPortBuf)
    {
        LOGE("(%s:L%d) param %p is NULL !!!\n", __func__, __LINE__, pPortBuf);
        return -1;
    }
    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.ui32SS0OutImgDstWidth = pstPos->width;
        m_IspostUser.ui32SS0OutImgDstHeight = pstPos->height;
        LOGD("(%s:%d) SS0 pipX=%d, pipY=%d, %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortSs0->IsEmbezzling())
    {
        pipX = m_pPortSs0->GetPipX();
        pipY = m_pPortSs0->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_ENABLE;
    m_IspostUser.ui32SS0OutImgBufYaddr = pPortBuf->fr_buf.phys_addr;
    m_IspostUser.ui32SS0OutImgBufUVaddr = m_IspostUser.ui32SS0OutImgBufYaddr + (m_pPortSs0->GetWidth() * m_pPortSs0->GetHeight());
    if (m_pPortSs0->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.ui32SS0OutImgBufStride,
                        m_IspostUser.ui32SS0OutImgBufYaddr, m_IspostUser.ui32SS0OutImgBufUVaddr);
    }

    return 0;
}

int IPU_ISPOSTV1::UpdateSs1UserInfo(bool hasFce, cam_position_t *pstPos, Buffer *pPortBuf)
{
    int pipX = 0;
    int pipY = 0;


    if (NULL == pPortBuf)
    {
        LOGE("(%s:L%d) param %p is NULL !!!\n", __func__, __LINE__, pPortBuf);
        return -1;
    }
    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.ui32SS1OutImgDstWidth = pstPos->width;
        m_IspostUser.ui32SS1OutImgDstHeight = pstPos->height;
        LOGD("(%s:%d) SS1 pipX=%d, pipY=%d, %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortSs1->IsEmbezzling())
    {
        pipX = m_pPortSs1->GetPipX();
        pipY = m_pPortSs1->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_ENABLE;
    m_IspostUser.ui32SS1OutImgBufYaddr = pPortBuf->fr_buf.phys_addr;
    m_IspostUser.ui32SS1OutImgBufUVaddr = m_IspostUser.ui32SS1OutImgBufYaddr + (m_pPortSs1->GetWidth() * m_pPortSs1->GetHeight());
    if (m_pPortSs1->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.ui32SS1OutImgBufStride,
                        m_IspostUser.ui32SS1OutImgBufYaddr, m_IspostUser.ui32SS1OutImgBufUVaddr);
    }

    return 0;
}

bool IPU_ISPOSTV1::IspostTrigger(Buffer& BufIn, Buffer& BufOl, Buffer& BufDn, Buffer& BufSs0, Buffer& BufSs1)
{
    int ret = 0;
    int fileNumber = 1;
    int hasOv = false;


    if (m_IspostUser.ui32IspostCount == 0)
        m_ui32DnRefImgBufYaddr =  0xFFFFFFFF;

    if (InEnableFlag){
        m_IspostUser.ui32LcSrcBufYaddr = BufIn.fr_buf.phys_addr;
        m_IspostUser.ui32LcSrcBufUVaddr = m_IspostUser.ui32LcSrcBufYaddr + (m_IspostUser.ui32LcSrcWidth * m_IspostUser.ui32LcSrcHeight);
    }

    if (OlEnableFlag){
        hasOv = true;
        m_IspostUser.ui32OvImgWidth = m_pPortOl->GetWidth();
        m_IspostUser.ui32OvImgHeight = m_pPortOl->GetHeight();
        m_IspostUser.ui32OvImgStride = m_pPortOl->GetWidth();
        pthread_mutex_lock(&m_PipMutex);
        IspostAdjustOvPos();
        m_IspostUser.ui32OvImgOffsetX = m_pPortOl->GetPipX();
        m_IspostUser.ui32OvImgOffsetY = m_pPortOl->GetPipY();
        pthread_mutex_unlock(&m_PipMutex);
        m_IspostUser.ui32OvImgBufYaddr = BufOl.fr_buf.phys_addr;
    }

    bool hasFce = false;
    cam_position_t stDnPos;
#if 0
    cam_position_t stUoPos;
#endif
    cam_position_t stSs0Pos;
    cam_position_t stSs1Pos;
    ST_FCE_MODE stActiveMode;
    ST_GRIDDATA_BUF_INFO stBufInfo;


    pthread_mutex_lock(&m_FceLock);
    memset(&stBufInfo, 0, sizeof(stBufInfo));
    if (m_stFce.hasFce)
    {
        ret = GetFceActiveMode(&stActiveMode);
        if (0 == ret)
        {
            hasFce = true;
            fileNumber = stActiveMode.stBufList.size();
        }
    }
    if (hasFce)
        UpdateFcePendingStatus();
    pthread_mutex_unlock(&m_FceLock);


    int i = 0;
    do
    {
        if (hasFce)
        {
            stBufInfo = stActiveMode.stBufList.at(i);

            if (!stActiveMode.stPosDnList.empty())
                stDnPos = stActiveMode.stPosDnList.at(i);
#if 0
            if (!stActiveMode.stPosUoList.empty())
                stUoPos = stActiveMode.stPosUoList.at(i);
#endif
            if (!stActiveMode.stPosSs0List.empty())
                stSs0Pos = stActiveMode.stPosSs0List.at(i);
            if (!stActiveMode.stPosSs1List.empty())
                stSs1Pos = stActiveMode.stPosSs1List.at(i);
        }

        if (stBufInfo.stBufGrid.phys_addr)
        {
            m_IspostUser.ui32LcGridBufAddr = stBufInfo.stBufGrid.phys_addr;
            m_IspostUser.ui32LcGridBufSize = stBufInfo.stBufGrid.size;
        }
        if (hasOv)
        {
            if (i == (fileNumber - 1))
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
            else
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
        }
        if (DnEnableFlag)
        {
            UpdateDnUserInfo(hasFce, &stDnPos, &m_ui32DnRefImgBufYaddr, &BufDn);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
        }

        if (m_IspostUser.stCtrl0.ui1EnLC && DnEnableFlag)
        {
            UpdateLcUserInfo(hasFce, &stDnPos);
        }

        if (SS0EnableFlag)
        {
            UpdateSs0UserInfo(hasFce, &stSs0Pos, &BufSs0);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_DISABLE;
        }
        if (SS1EnableFlag)
        {
            UpdateSs1UserInfo(hasFce, &stSs1Pos, &BufSs1);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_DISABLE;
        }

        //IspostDumpUserInfo();
        ret = ispost_trigger(&m_IspostUser);
    }while(++i < fileNumber);


    m_IspostUser.ui32IspostCount++;

    return (ret >= 0) ? true : false;
}

void IPU_ISPOSTV1::IspostDefaultParameter(void)
{
    memset(&m_IspostUser, 0, sizeof(ISPOST_USER));

    //Ispost control
    m_IspostUser.stCtrl0.ui1EnLC = ISPOST_DISABLE;
    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
    m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
    m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_DISABLE;
    m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_DISABLE;
    m_IspostUser.ui32UpdateFlag = 0;
    m_IspostUser.ui32PrintFlag = 0; //ISPOST_PRN_ALL;
    m_IspostUser.ui32IspostCount = 0;

    //Lens correction information
    m_IspostUser.ui32LcSrcBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32LcSrcBufUVaddr = 0;
    m_IspostUser.ui32LcSrcWidth = 0;
    m_IspostUser.ui32LcSrcHeight = 0;
    m_IspostUser.ui32LcSrcStride = 0;
    m_IspostUser.ui32LcDstWidth = 0;
    m_IspostUser.ui32LcDstHeight = 0;
    m_IspostUser.ui32LcGridBufAddr = 0xFFFFFFFF;
    m_IspostUser.ui32LcGridBufSize = 0;
    m_IspostUser.ui32LcGridTargetIdx = 0;
    m_IspostUser.ui32LcGridLineBufEn = 0;
    m_IspostUser.ui32LcGridSize = GRID_SIZE_32_32;
    m_IspostUser.ui32LcCacheModeCtrlCfg = CACHE_MODE_4;
    m_IspostUser.ui32LcCBCRSwap = CBCR_SWAP_MODE_SWAP;
    m_IspostUser.ui32LcScanModeScanW = SCAN_WIDTH_32;
    m_IspostUser.ui32LcScanModeScanH = SCAN_HEIGHT_32;
    m_IspostUser.ui32LcScanModeScanM = SCAN_MODE_LR_TB;
    m_IspostUser.ui32LcFillY = LC_BACK_FILL_Y;
    m_IspostUser.ui32LcFillCB = LC_BACK_FILL_CB;
    m_IspostUser.ui32LcFillCR = LC_BACK_FILL_CR;

    //Overlay image information
    m_IspostUser.ui32OvMode = OVERLAY_MODE_YUV;
    m_IspostUser.ui32OvImgBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32OvImgBufUVaddr = 0;
    m_IspostUser.ui32OvImgWidth = 0;
    m_IspostUser.ui32OvImgHeight = 0;
    m_IspostUser.ui32OvImgStride = 0;
    m_IspostUser.ui32OvImgOffsetX = 0;
    m_IspostUser.ui32OvImgOffsetY = 0;
    m_IspostUser.ui32OvAlphaValue0 = 0x33221100;
    m_IspostUser.ui32OvAlphaValue1 = 0x77665544;
    m_IspostUser.ui32OvAlphaValue2 = 0xBBAA9988;
    m_IspostUser.ui32OvAlphaValue3 = 0xFFEEDDCC;
//  m_IspostUser.ui32OvAlphaValue0 = 0xFFFFFFFF;
//  m_IspostUser.ui32OvAlphaValue1 = 0xFFFFFFFF;
//  m_IspostUser.ui32OvAlphaValue2 = 0xFFFFFFFF;
//  m_IspostUser.ui32OvAlphaValue3 = 0xFFFFFFFF;

    //3D-denoise information
    m_IspostUser.ui32DnOutImgBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnOutImgBufUVaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnOutImgBufStride = 0;
    m_IspostUser.ui32DnRefImgBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnRefImgBufUVaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnRefImgBufStride = 0;
    m_IspostUser.ui32DnTargetIdx = 0;
    IspostSet3dDnsCurve(&m_Dns3dAttr, true);

    //Scaling stream 0 information
    m_IspostUser.ui32SS0OutImgBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32SS0OutImgBufUVaddr = 0;
    m_IspostUser.ui32SS0OutImgBufStride = 0;
    m_IspostUser.ui32SS0OutImgDstWidth = 0;
    m_IspostUser.ui32SS0OutImgDstHeight = 0;
    m_IspostUser.ui32SS0OutImgMaxWidth = 1920;
    m_IspostUser.ui32SS0OutImgMaxHeight = 1088;
	m_IspostUser.ui32SS0OutHorzScaling = SCALING_MODE_SCALING_DOWN;
	m_IspostUser.ui32SS0OutVertScaling = SCALING_MODE_SCALING_DOWN;

    //Scaling stream 1 information
    m_IspostUser.ui32SS1OutImgBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32SS1OutImgBufUVaddr = 0;
    m_IspostUser.ui32SS1OutImgBufStride = 0;
    m_IspostUser.ui32SS1OutImgDstWidth = 0;
    m_IspostUser.ui32SS1OutImgDstHeight = 0;
    m_IspostUser.ui32SS1OutImgMaxWidth = 1920;
    m_IspostUser.ui32SS1OutImgMaxHeight = 1088;
	m_IspostUser.ui32SS1OutHorzScaling = SCALING_MODE_SCALING_DOWN;
	m_IspostUser.ui32SS1OutVertScaling = SCALING_MODE_SCALING_DOWN;
}

void IPU_ISPOSTV1::IspostCheckParameter()
{
    if ((m_pPortIn->GetPipX() < 0) ||
        (m_pPortIn->GetPipY() < 0) ||
        (m_pPortIn->GetWidth() < 0) ||
        (m_pPortIn->GetHeight() < 0) ||
        (m_pPortIn->GetPipX() % 2 != 0) ||
        (m_pPortIn->GetPipY() % 2 != 0) ||
        (m_pPortIn->GetWidth() % 8 != 0) ||
        (m_pPortIn->GetHeight() % 8 != 0) ||
        (m_pPortIn->GetPipX() > m_pPortIn->GetWidth()) ||
        (m_pPortIn->GetPipY() > m_pPortIn->GetHeight()) ||
        (m_pPortIn->GetPipWidth() > m_pPortIn->GetWidth()) ||
        (m_pPortIn->GetPipHeight() > m_pPortIn->GetHeight()) ||
        (m_pPortIn->GetPipX() + m_pPortIn->GetPipWidth() > m_pPortIn->GetWidth()) ||
        (m_pPortIn->GetPipY() + m_pPortIn->GetPipHeight() > m_pPortIn->GetHeight())){
        LOGE("ISPost Crop params error\n");
        throw VBERR_BADPARAM;
    }

    if ((m_pPortIn->GetPixelFormat() != Pixel::Format::NV12) &&
        (m_pPortIn->GetPixelFormat() != Pixel::Format::NV21)) {
        LOGE("ISPost PortIn format params error\n");
        throw VBERR_BADPARAM;
    }


    if (m_pPortOl->IsEnabled()){
        if ((m_pPortOl->GetPixelFormat() != Pixel::Format::NV12) &&
           (m_pPortOl->GetPixelFormat() != Pixel::Format::NV21)){
            LOGE("ISPost PortOl format params error\n");
            throw VBERR_BADPARAM;
        }
    }

    if(m_pPortDn->IsEnabled()){
        if ((m_pPortDn->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortDn->GetPixelFormat() != Pixel::Format::NV21)){
            LOGE("ISPost PortDn format params error\n");
            throw VBERR_BADPARAM;
        }
        if (m_pPortDn->IsEmbezzling()){
            if (m_pPortDn->GetPipX() < 0 ||
                m_pPortDn->GetPipY() < 0 ||
                (m_pPortDn->GetPipX() + m_pPortDn->GetPipWidth()) > m_pPortDn->GetWidth() ||
                (m_pPortDn->GetPipY() + m_pPortDn->GetPipHeight()) > m_pPortDn->GetHeight()){
                LOGE("ISPost PortDn Embezzling params error\n");
                LOGE("PortDn(W:%d, H:%d), PortDn(X:%d, Y:%d, W:%d, H%d)\n",
                    m_pPortDn->GetWidth(), m_pPortDn->GetHeight(),
                    m_pPortDn->GetPipX(), m_pPortDn->GetPipY(),
                    m_pPortDn->GetPipWidth(), m_pPortDn->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }else{
            m_pPortDn->SetPipInfo(0, 0, m_pPortDn->GetWidth(), m_pPortDn->GetHeight());
        }
        if (m_pPortDn->GetPipWidth() < 16 || m_pPortDn->GetPipWidth() > 4096 ||
            m_pPortDn->GetPipHeight() < 16 || m_pPortDn->GetPipHeight() > 4096){
            LOGE("ISPost PortDn 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

    if(m_pPortSs0->IsEnabled()){
        if ((m_pPortSs0->GetPixelFormat() != Pixel::Format::NV12) &&
           (m_pPortSs0->GetPixelFormat() != Pixel::Format::NV21)){
            LOGE("ISPost PortSs0 format params error\n");
            throw VBERR_BADPARAM;
        }
        if(m_pPortSs0->IsEmbezzling()){
            if (m_pPortSs0->GetPipX() < 0 ||
                m_pPortSs0->GetPipY() < 0 ||
                (m_pPortSs0->GetPipX() + m_pPortSs0->GetPipWidth()) > m_pPortSs0->GetWidth() ||
                (m_pPortSs0->GetPipY() + m_pPortSs0->GetPipHeight()) > m_pPortSs0->GetHeight()){
                LOGE("ISPost PortSs0 Embezzling params error\n");
                LOGE("PortSs0(W:%d, H:%d), Pip(X:%d, Y:%d, W:%d, H%d)\n",
                    m_pPortSs0->GetWidth(), m_pPortSs0->GetHeight(),
                    m_pPortSs0->GetPipX(), m_pPortSs0->GetPipY(),
                    m_pPortSs0->GetPipWidth(), m_pPortSs0->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }else{
            m_pPortSs0->SetPipInfo(0, 0, m_pPortSs0->GetWidth(), m_pPortSs0->GetHeight());
        }
        if (m_pPortSs0->GetPipWidth() < 16 || m_pPortSs0->GetPipWidth() > 4096 ||
            m_pPortSs0->GetPipHeight() < 16 || m_pPortSs0->GetPipHeight() > 4096){
            LOGE("ISPost not embezzling PortSS0 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

    if(m_pPortSs1->IsEnabled()){

        if((m_pPortSs1->GetPixelFormat() != Pixel::Format::NV12) &&
          (m_pPortSs1->GetPixelFormat() != Pixel::Format::NV21)){
            LOGE("ISPost PortSs1 format params error\n");
            throw VBERR_BADPARAM;
        }
        if(m_pPortSs1->IsEmbezzling()){
            if (m_pPortSs1->GetPipX() < 0 ||
                m_pPortSs1->GetPipY() < 0 ||
                (m_pPortSs1->GetPipX() + m_pPortSs1->GetPipWidth()) > m_pPortSs1->GetWidth() ||
                (m_pPortSs1->GetPipY() + m_pPortSs1->GetPipHeight()) > m_pPortSs1->GetHeight()){
                LOGE("ISPost PortSs1 Embezzling params error\n");
                LOGE("PortSs1(W:%d, H:%d), Pip(X:%d, Y:%d, W:%d, H%d)\n",
                    m_pPortSs1->GetWidth(), m_pPortSs1->GetHeight(),
                    m_pPortSs1->GetPipX(), m_pPortSs1->GetPipY(),
                    m_pPortSs1->GetPipWidth(), m_pPortSs1->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }else{
            m_pPortSs1->SetPipInfo(0, 0, m_pPortSs1->GetWidth(), m_pPortSs1->GetHeight());
        }
        if (m_pPortSs1->GetPipWidth() < 16 || m_pPortSs1->GetPipWidth() > 4096 ||
            m_pPortSs1->GetPipHeight() < 16 || m_pPortSs1->GetPipHeight() > 4096){
            LOGE("ISPost not embezzling PortSs1 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

}

bool IPU_ISPOSTV1::IspostJsonGetInt(const char* pszItem, int& Value)
{
    Value = 0;

    if (m_pJsonMain == NULL)
        return false;

    Json* pSub = m_pJsonMain->GetObject(pszItem);
    if (pSub == NULL)
        return false;

    Value = pSub->valueint;
    LOGD("GetInt: %s = %d\n", pszItem, Value);
    return true;
}

bool IPU_ISPOSTV1::IspostJsonGetString(const char* pszItem, char*& pszString)
{
    pszString = NULL;

    if (m_pJsonMain == NULL)
        return false;

    Json* pSub = m_pJsonMain->GetObject(pszItem);
    if (pSub == NULL)
        return false;

    pszString = pSub->valuestring;
    LOGD("GetStr: %s = %s\n", pszItem, pszString);
    return true;
}

ISPOST_UINT32 IPU_ISPOSTV1::IspostGetGridTargetIndexMax(int width, int height, int sz)
{
    ISPOST_UINT32 ui32Stride;
    ISPOST_UINT32 ui32GridSize;
    ISPOST_UINT32 ui32Width;
    ISPOST_UINT32 ui32Height;

    if (GRID_SIZE_MAX <= sz) {
        sz = GRID_SIZE_8_8;
    }

    ui32GridSize = g_dwGridSize[sz];
    ui32Width = (ISPOST_UINT32)width / ui32GridSize;
    if ((ISPOST_UINT32)width > (ui32Width * ui32GridSize)) {
        ui32Width += 2;
    } else {
        ui32Width++;
    }
    ui32Stride = ui32Width * 16;

    ui32Height = (ISPOST_UINT32)height / ui32GridSize;
    if ((ISPOST_UINT32)height > (ui32Height * ui32GridSize)) {
        ui32Height += 2;
    } else {
        ui32Height++;
    }

    return ((ui32Height * ui32Stride)+63)&0xFFFFFFC0;
}

ISPOST_UINT32 IPU_ISPOSTV1::IspostGetFileSize(FILE* pFileID)
{
    ISPOST_UINT32 pos = ftell(pFileID);
    ISPOST_UINT32 len = 0;
    fseek(pFileID, 0L, SEEK_END);
    len = ftell(pFileID);
    fseek(pFileID, pos, SEEK_SET);
    return len;
}

bool IPU_ISPOSTV1::IspostLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane)
{
    ISPOST_UINT32 nLoaded = 0, temp = 0;
    FILE* f = NULL;
    Buffer buf;

    if (*ppBuffer != NULL){
        delete (*ppBuffer);
        *ppBuffer = NULL;
    }

    if ( (f = fopen(pszFilenane, "rb")) == NULL ){
        LOGE("failed to open %s!\n", pszFilenane);
        return false;
    }

    nLoaded = IspostGetFileSize(f);
    temp = IspostGetGridTargetIndexMax(m_pPortDn->GetWidth(), m_pPortDn->GetHeight(), m_IspostUser.ui32LcGridSize);
    grid_target_index_max = nLoaded / temp;
    LOGE("grid_target_index_max:%d\n", grid_target_index_max);

    *ppBuffer = new FRBuffer(pszBufName, FRBuffer::Type::FIXED, 1, nLoaded);
    if (*ppBuffer ==  NULL){
        fclose(f);
        LOGE("failed to allocate buffer.\n");
        return false;
    }

    buf = (*ppBuffer)->GetBuffer();
    fread(buf.fr_buf.virt_addr, nLoaded, 1, f);
    (*ppBuffer)->PutBuffer(&buf);

    fclose(f);

    LOGD("success load file %s with %d size.\n", pszFilenane, nLoaded);

    return true;
}

ISPOST_UINT32 IPU_ISPOSTV1::IspostParseGirdSize(const char* pszFileName)
{
    char* pszHermite;
    pszHermite = strstr(pszFileName, "hermite32");
    if (pszHermite != NULL)
        return GRID_SIZE_32_32;
    pszHermite = strstr(pszFileName, "hermite16");
    if (pszHermite != NULL)
        return GRID_SIZE_16_16;
    pszHermite = strstr(pszFileName, "hermite8");
    if (pszHermite != NULL)
        return GRID_SIZE_8_8;
    return GRID_SIZE_8_8;
}

void IPU_ISPOSTV1::IspostCalcPlanarAddress(int w, int h, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr)
{
    UVaddr = Yaddr + (w * h);
}

void IPU_ISPOSTV1::IspostCalcOffsetAddress(int x, int y, int stride, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr)
{
    Yaddr += ((y * stride) + x);
    UVaddr += (((y / 2) * stride) + x);
}

void IPU_ISPOSTV1::IspostDumpUserInfo(void)
{
    LOGD("==============================\n");
    LOGD("stCtrl0: %d %d %d %d %d\n",
                    m_IspostUser.stCtrl0.ui1EnLC,
                    m_IspostUser.stCtrl0.ui1EnOV,
                    m_IspostUser.stCtrl0.ui1EnDN,
                    m_IspostUser.stCtrl0.ui1EnSS0,
                    m_IspostUser.stCtrl0.ui1EnSS1);
    LOGD("ui32UpdateFlag: 0x%04X\n", m_IspostUser.ui32UpdateFlag);
    LOGD("ui32PrintFlag: 0x%04X\n", m_IspostUser.ui32PrintFlag);
    LOGD("ui32IspostCount: %d\n", m_IspostUser.ui32IspostCount);

    LOGD("------------------------------\n");
    LOGD("ui32LcSrcBufYaddr: 0x%08X\n", m_IspostUser.ui32LcSrcBufYaddr);
    LOGD("ui32LcSrcBufUVaddr: 0x%08X\n", m_IspostUser.ui32LcSrcBufUVaddr);
    LOGD("ui32LcSrcWidth: %d\n", m_IspostUser.ui32LcSrcWidth);
    LOGD("ui32LcSrcHeight: %d\n", m_IspostUser.ui32LcSrcHeight);
    LOGD("ui32LcSrcStride: %d\n", m_IspostUser.ui32LcSrcStride);
    LOGD("ui32LcDstWidth: %d\n", m_IspostUser.ui32LcDstWidth);
    LOGD("ui32LcDstHeight: %d\n", m_IspostUser.ui32LcDstHeight);
    LOGD("ui32LcGridBufAddr: 0x%08X\n", m_IspostUser.ui32LcGridBufAddr);
    LOGD("ui32LcGridBufSize: %d\n", m_IspostUser.ui32LcGridBufSize);
    LOGD("ui32LcGridTargetIdx: %d\n", m_IspostUser.ui32LcGridTargetIdx);
    LOGD("ui32LcGridLineBufEn: %d\n", m_IspostUser.ui32LcGridLineBufEn);
    LOGD("ui32LcGridSize: %d\n", m_IspostUser.ui32LcGridSize);
    LOGD("ui32LcCacheModeCtrlCfg: %d\n", m_IspostUser.ui32LcCacheModeCtrlCfg);
    LOGD("ui32LcCBCRSwap: %d\n", m_IspostUser.ui32LcCBCRSwap);
    LOGD("ui32LcScanModeScanW: %d\n", m_IspostUser.ui32LcScanModeScanW);
    LOGD("ui32LcScanModeScanH: %d\n", m_IspostUser.ui32LcScanModeScanH);
    LOGD("ui32LcScanModeScanM: %d\n", m_IspostUser.ui32LcScanModeScanM);
    LOGD("ui32LcFillY: %d\n", m_IspostUser.ui32LcFillY);
    LOGD("ui32LcFillCB: %d\n", m_IspostUser.ui32LcFillCB);
    LOGD("ui32LcFillCR: %d\n", m_IspostUser.ui32LcFillCR);
    LOGD("------------------------------\n");

    LOGD("ui32OvMode: %d\n", m_IspostUser.ui32OvMode);
    LOGD("ui32OvImgBufYaddr: 0x%08X\n", m_IspostUser.ui32OvImgBufYaddr);
    LOGD("ui32OvImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32OvImgBufUVaddr);
    LOGD("ui32OvImgWidth: %d\n", m_IspostUser.ui32OvImgWidth);
    LOGD("ui32OvImgHeight: %d\n", m_IspostUser.ui32OvImgHeight);
    LOGD("ui32OvImgStride: %d\n", m_IspostUser.ui32OvImgStride);
    LOGD("ui32OvImgOffsetX: %d\n", m_IspostUser.ui32OvImgOffsetX);
    LOGD("ui32OvImgOffsetY: %d\n", m_IspostUser.ui32OvImgOffsetY);
    LOGD("ui32OvAlphaValue0: 0x%08X\n", m_IspostUser.ui32OvAlphaValue0);
    LOGD("ui32OvAlphaValue1: 0x%08X\n", m_IspostUser.ui32OvAlphaValue1);
    LOGD("ui32OvAlphaValue2: 0x%08X\n", m_IspostUser.ui32OvAlphaValue2);
    LOGD("ui32OvAlphaValue3: 0x%08X\n", m_IspostUser.ui32OvAlphaValue3);
    LOGD("------------------------------\n");

    LOGD("ui32DnOutImgBufYaddr: 0x%08X\n", m_IspostUser.ui32DnOutImgBufYaddr);
    LOGD("ui32DnOutImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32DnOutImgBufUVaddr);
    LOGD("ui32DnOutImgBufStride: %d\n", m_IspostUser.ui32DnOutImgBufStride);
    LOGD("ui32DnRefImgBufYaddr: 0x%08X\n", m_IspostUser.ui32DnRefImgBufYaddr);
    LOGD("ui32DnRefImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32DnRefImgBufUVaddr);
    LOGD("ui32DnRefImgBufStride: %d\n", m_IspostUser.ui32DnRefImgBufStride);
    LOGD("ui32DnTargetIdx: %d\n", m_IspostUser.ui32DnTargetIdx);
    LOGD("------------------------------\n");

    LOGD("ui32SS0OutImgBufYaddr: 0x%08X\n", m_IspostUser.ui32SS0OutImgBufYaddr);
    LOGD("ui32SS0OutImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32SS0OutImgBufUVaddr);
    LOGD("ui32SS0OutImgBufStride: %d\n", m_IspostUser.ui32SS0OutImgBufStride);
    LOGD("ui32SS0OutImgDstWidth: %d\n", m_IspostUser.ui32SS0OutImgDstWidth);
    LOGD("ui32SS0OutImgDstHeight: %d\n", m_IspostUser.ui32SS0OutImgDstHeight);
    LOGD("ui32SS0OutImgMaxWidth: %d\n", m_IspostUser.ui32SS0OutImgMaxWidth);
    LOGD("ui32SS0OutImgMaxHeight: %d\n", m_IspostUser.ui32SS0OutImgMaxHeight);
    LOGD("ui32SS0OutHorzScaling: %d\n", m_IspostUser.ui32SS0OutHorzScaling);
    LOGD("ui32SS0OutVertScaling: %d\n", m_IspostUser.ui32SS0OutVertScaling);
    LOGD("------------------------------\n");


    LOGD("ui32SS1OutImgBufYaddr: 0x%08X\n", m_IspostUser.ui32SS1OutImgBufYaddr);
    LOGD("ui32SS1OutImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32SS1OutImgBufUVaddr);
    LOGD("ui32SS1OutImgBufStride: %d\n", m_IspostUser.ui32SS1OutImgBufStride);
    LOGD("ui32SS1OutImgDstWidth: %d\n", m_IspostUser.ui32SS1OutImgDstWidth);
    LOGD("ui32SS1OutImgDstHeight: %d\n", m_IspostUser.ui32SS1OutImgDstHeight);
    LOGD("ui32SS1OutImgMaxWidth: %d\n", m_IspostUser.ui32SS1OutImgMaxWidth);
    LOGD("ui32SS1OutImgMaxHeight: %d\n", m_IspostUser.ui32SS1OutImgMaxHeight);
    LOGD("ui32SS1OutHorzScaling: %d\n", m_IspostUser.ui32SS1OutHorzScaling);
    LOGD("ui32SS1OutVertScaling: %d\n", m_IspostUser.ui32SS1OutVertScaling);
    LOGD("==============================\n");
}
int IPU_ISPOSTV1::IspostSetLcTargetIndex(int index)
{
    if (index < 0)
    {
        return -1;
    }
    if (index >= grid_target_index_max)
    {
        return -1;
    }
    m_IspostUser.ui32LcGridTargetIdx = index % grid_target_index_max;
    return 0;
}
int IPU_ISPOSTV1::IspostGetLcTargetIndex()
{
    return m_IspostUser.ui32LcGridTargetIdx;
}
int IPU_ISPOSTV1::IspostGetLcTargetCount()
{
    return grid_target_index_max;
}

void IPU_ISPOSTV1::IspostAdjustOvPos()
{
    int dn_w, dn_h;
    int ov_w, ov_h;
    int ov_x, ov_y;

    dn_w = m_pPortDn->GetWidth() - 10;
    dn_h = m_pPortDn->GetHeight() - 10;

    ov_w = m_pPortOl->GetWidth();
    ov_h = m_pPortOl->GetHeight();

    ov_x = m_pPortOl->GetPipX();
    ov_y = m_pPortOl->GetPipY();

    if (ov_w + ov_x >= dn_w)
        ov_x = dn_w - ov_w ;

    if (ov_h + ov_y >= dn_h)
        ov_y = dn_h - ov_h;

    if (ov_x < 0)
        ov_x = 0;

    if (ov_y < 0)
        ov_y = 0;

    ov_x -= ov_x%4;
    ov_y -= ov_y%4;

    m_pPortOl->SetPipInfo(ov_x, ov_y, 0, 0);
}

int IPU_ISPOSTV1::IspostSetPipInfo(struct v_pip_info *vpinfo)
{
    IspostFormatPipInfo(vpinfo);
    pthread_mutex_lock(&m_PipMutex);
    {
        if(strncmp(vpinfo->portname, "dn", 2) == 0){
            m_pPortDn->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        }else if(strncmp(vpinfo->portname, "ss0", 3) == 0){
            m_pPortSs0->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        }else if(strncmp(vpinfo->portname, "ss1", 3) == 0){
            m_pPortSs1->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        }else if(strncmp(vpinfo->portname, "in", 2) == 0){
            m_pPortIn->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        }else if(strncmp(vpinfo->portname, "ol", 2) == 0){
            m_pPortOl->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        }
    }
    pthread_mutex_unlock(&m_PipMutex);

    LOGE("SetPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

    return 0;
}

int IPU_ISPOSTV1::IspostGetPipInfo(struct v_pip_info *vpinfo)
{
    if(strncmp(vpinfo->portname, "dn", 2) == 0){
        vpinfo->x = m_pPortDn->GetPipX();
        vpinfo->y = m_pPortDn->GetPipY();
        vpinfo->w = m_pPortDn->GetPipWidth();
        vpinfo->h = m_pPortDn->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "ss0", 3) == 0){
        vpinfo->x = m_pPortSs0->GetPipX();
        vpinfo->y = m_pPortSs0->GetPipY();
        vpinfo->w = m_pPortSs0->GetPipWidth();
        vpinfo->h = m_pPortSs0->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "ss1", 3) == 0){
        vpinfo->x = m_pPortSs1->GetPipX();
        vpinfo->y = m_pPortSs1->GetPipY();
        vpinfo->w = m_pPortSs1->GetPipWidth();
        vpinfo->h = m_pPortSs1->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "in", 2) == 0){
        vpinfo->x = m_pPortIn->GetPipX();
        vpinfo->y = m_pPortIn->GetPipY();
        vpinfo->w = m_pPortIn->GetPipWidth();
        vpinfo->h = m_pPortIn->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "ol", 2) == 0){
        vpinfo->x = m_pPortOl->GetPipX();
        vpinfo->y = m_pPortOl->GetPipY();
        vpinfo->w = m_pPortOl->GetPipWidth();
        vpinfo->h = m_pPortOl->GetPipHeight();
    }

    return 0;
}

int IPU_ISPOSTV1::IspostSet3dDnsCurve(const cam_3d_dns_attr_t *attr, int is_force)
{
    int ret = -1;
    unsigned char weighting = 1;
    unsigned char threshold[3]={20,20,20};
    Q3_REG_VALUE hermiteTable[Q3_DENOISE_REG_NUM];
    ISPOST_UINT32 i = 0;
    ISPOST_UINT32 j = 0;
    int idx = 0;


    ret = IspostCheckParam(attr);
    if (ret)
        return ret;

    if (is_force || (m_Dns3dAttr.y_threshold != attr->y_threshold)
        || (m_Dns3dAttr.u_threshold != attr->u_threshold)
        || (m_Dns3dAttr.v_threshold != attr->v_threshold)
        || (m_Dns3dAttr.weight != attr->weight))
    {
        memset(hermiteTable, 0, sizeof(hermiteTable));

        threshold[0] = attr->y_threshold;
        threshold[1] = attr->u_threshold;
        threshold[2] = attr->v_threshold;
        weighting = attr->weight;

        ret = DN_HermiteGenerator(threshold, weighting, hermiteTable);
        if (ret)
        {
            LOGE("(%s:%d) Error: HermiteGenerator ret =%d !!!\n", __func__, __LINE__, ret);
            return ret;
        }


        LOGD("(%s:%d) thr0=%d thr1=%d thr2=%d w=%d \n",__func__, __LINE__,
                threshold[0], threshold[1], threshold[2], weighting);

        for (i = 0; i < DN_MSK_CRV_IDX_MAX; i++)
        {
            m_IspostUser.stDnMskCrvInfo[i].ui8MskCrvIdx = DN_MSK_CRV_IDX_Y + i;
            for (j = 0; j < DN_MSK_CRV_MAX; j++)
            {
                idx = 1 + i + i * DN_MSK_CRV_MAX + j;
                m_IspostUser.stDnMskCrvInfo[i].stMskCrv[j].ui32MskCrv = hermiteTable[idx].value;
            }
        }
        m_Dns3dAttr = *attr;
    }

    return ret;
}

int IPU_ISPOSTV1::IspostProcPrivateData(ipu_v2500_private_data_t *pdata)
{
    int ret = -1;
    cam_3d_dns_attr_t *pattr = NULL;
    cam_common_t *pcmn = NULL;
    ISPOST_UINT32 autoCtrlIdx = 0;
    cam_3d_dns_attr_t autoDnsAttr;


    if (NULL == pdata)
        return ret;

    pattr = &pdata->dns_3d_attr;
    ret = IspostCheckParam(pattr);
    if (ret)
        return ret;
    pcmn = (cam_common_t*)&pattr->cmn;

    if (CAM_CMN_AUTO == pcmn->mode)
    {
        memset(&autoDnsAttr, 0, sizeof(autoDnsAttr));

        autoCtrlIdx = pdata->iDnID;
        if (((m_IspostUser.ui32DnTargetIdx != autoCtrlIdx) && (autoCtrlIdx < sMaxLevelCnt)) ||
            (m_eDnsModePrev != pcmn->mode))
        {
            autoDnsAttr.y_threshold = g_dnParamList[autoCtrlIdx].y_threshold;
            autoDnsAttr.u_threshold = g_dnParamList[autoCtrlIdx].u_threshold;
            autoDnsAttr.v_threshold = g_dnParamList[autoCtrlIdx].v_threshold;
            autoDnsAttr.weight = g_dnParamList[autoCtrlIdx].weight;
            pattr = &autoDnsAttr;

            m_IspostUser.ui32DnTargetIdx = autoCtrlIdx;
        }
        else
        {
            pattr = &m_Dns3dAttr;
        }
        m_eDnsModePrev = CAM_CMN_AUTO;
    }
    else
    {
        m_eDnsModePrev = CAM_CMN_MANUAL;
    }

    ret = IspostSet3dDnsCurve(pattr, 0);

    return ret;
}

#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
int IPU_ISPOSTV1::IspostGetDnParamCount(void)
{
    return (int)(sizeof(g_dnParamList)/sizeof(ipu_ispost_3d_dn_param));
}

ipu_ispost_3d_dn_param* IPU_ISPOSTV1::IspostGetDnParamList(void)
{
    return g_dnParamList;
}

#endif //#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
#ifdef MACRO_IPU_ISPOSTV1_FCE
int CalcGridDataSize(TFceCalcGDSizeParam *pstDataSize)
{
    int ret = 0;


    if (NULL == pstDataSize)
    {
        LOGE("(%s:L%d) param is NULL !\n", __func__, __LINE__);
        return -1;
    }
    ret = FceCalGDSize(pstDataSize);
    if (ret)
    {
        LOGE("(%s:L%d) FceCalGDSize() error!\n", __func__, __LINE__);
        goto toOut;
    }
    pstDataSize->gridBufLen = (pstDataSize->gridBufLen + GRIDDATA_ALIGN_SIZE - 1) / GRIDDATA_ALIGN_SIZE * GRIDDATA_ALIGN_SIZE;
    pstDataSize->geoBufLen = (pstDataSize->geoBufLen + GRIDDATA_ALIGN_SIZE - 1) / GRIDDATA_ALIGN_SIZE * GRIDDATA_ALIGN_SIZE;
toOut:
    return ret;
}

int GenGridData(TFceGenGridDataParam *stGridParam)
{
    int ret = 0;

    if (NULL == stGridParam)
    {
        LOGE("(%s:L%d) param is NULL !\n", __func__, __LINE__);
        return -1;
    }
    ret = FceGenGridData(stGridParam);
    if (ret)
    {
        LOGE("(%s:L%d) Genrating grid data error !\n", __func__, __LINE__);
    }

    return ret;
}

void IPU_ISPOSTV1::ParseFceDataParam(cam_fisheye_correction_t *pstFrom, TFceGenGridDataParam *pstTo)
{
    if (NULL == pstFrom || NULL == pstTo)
        return;
    pstTo->fisheyeMode = pstFrom->fisheye_mode;

    pstTo->fisheyeStartTheta = pstFrom->fisheye_start_theta;
    pstTo->fisheyeEndTheta = pstFrom->fisheye_end_theta;
    pstTo->fisheyeStartPhi = pstFrom->fisheye_start_phi;
    pstTo->fisheyeEndPhi = pstFrom->fisheye_end_phi;

    pstTo->fisheyeRadius = pstFrom->fisheye_radius;

    pstTo->rectCenterX = pstFrom->rect_center_x;
    pstTo->rectCenterY = pstFrom->rect_center_y;

    pstTo->fisheyeFlipMirror = pstFrom->fisheye_flip_mirror;
    pstTo->scalingWidth = pstFrom->scaling_width;
    pstTo->scalingHeight = pstFrom->scaling_height;
    pstTo->fisheyeRotateAngle = pstFrom->fisheye_rotate_angle;
    pstTo->fisheyeRotateScale = pstFrom->fisheye_rotate_scale;

    pstTo->fisheyeHeadingAngle = pstFrom->fisheye_heading_angle;
    pstTo->fisheyePitchAngle = pstFrom->fisheye_pitch_angle;
    pstTo->fisheyeFovAngle = pstFrom->fisheye_fov_angle;

    pstTo->coefficientHorizontalCurve = pstFrom->coefficient_horizontal_curve;
    pstTo->coefficientVerticalCurve = pstFrom->coefficient_vertical_curve;

    pstTo->fisheyeTheta1 = pstFrom->fisheye_theta1;
    pstTo->fisheyeTheta2 = pstFrom->fisheye_theta2;
    pstTo->fisheyeTranslation1 = pstFrom->fisheye_translation1;
    pstTo->fisheyeTranslation2 = pstFrom->fisheye_translation2;
    pstTo->fisheyeTranslation3 = pstFrom->fisheye_translation3;
    pstTo->fisheyeCenterX2 = pstFrom->fisheye_center_x2;
    pstTo->fisheyeCenterY2 = pstFrom->fisheye_center_y2;
    pstTo->fisheyeRotateAngle2 = pstFrom->fisheye_rotate_angle2;

    pstTo->debugInfo = pstFrom->debug_info;
}

int IPU_ISPOSTV1::IspostCreateAndFireFceData(cam_fcedata_param_t *pstParam)
{
    int ret = -1;
    TFceCalcGDSizeParam stDataSize;
    TFceGenGridDataParam stGridParam;
    int gridSize = 0;
    int inWidth = 0;
    int inHeight = 0;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    int fisheyeCenterX = 0;
    int fisheyeCenterY = 0;
    int listSize = 0;
    char szFrBufName[50];
    bool hasNewBuf = false;
    ST_FCE_MODE_GROUP stGroup;
    std::vector<ST_FCE_MODE_GROUP>::iterator idleIt;


    pthread_mutex_lock(&m_FceLock);
    if (m_HasFceFile)
    {
        LOGE("(%s:L%d) FCE had existed !\n", __func__, __LINE__);
        goto toOut;
    }
    inWidth = m_pPortIn->GetWidth();
    inHeight = m_pPortIn->GetHeight();
    switch (pstParam->common.grid_size)
    {
    case CAM_GRID_SIZE_32:
        gridSize = 32;
        break;
    case CAM_GRID_SIZE_16:
        gridSize = 16;
        break;
    case CAM_GRID_SIZE_8:
        gridSize = 8;
        break;
    default:
        gridSize = 32;
        break;
    }
    if (0 == pstParam->common.fisheye_center_x
        && 0 == pstParam->common.fisheye_center_y)
    {
        fisheyeCenterX = inWidth / 2;
        fisheyeCenterY = inHeight / 2;
    }
    else
    {
        fisheyeCenterX = pstParam->common.fisheye_center_x;
        fisheyeCenterY = pstParam->common.fisheye_center_y;
    }

    stDataSize.gridSize = gridSize;
    stDataSize.inWidth = inWidth;
    stDataSize.inHeight = inHeight;
    stDataSize.outWidth = pstParam->common.out_width;
    stDataSize.outHeight = pstParam->common.out_height;
    ret = CalcGridDataSize(&stDataSize);
    if (ret)
        goto calcSizeFail;

    memset(&stBufInfo, 0, sizeof(stBufInfo));
    m_stFce.maxModeGroupNb = CREATEFCEDATA_MAX;
    listSize = m_stFce.stModeGroupAllList.size();
    if (listSize < m_stFce.maxModeGroupNb)
    {
        sprintf(szFrBufName, "CreateGDGrid%d", listSize);
        stBufInfo.stBufGrid.virt_addr = fr_alloc_single(szFrBufName, stDataSize.gridBufLen, &stBufInfo.stBufGrid.phys_addr);
        if (NULL == stBufInfo.stBufGrid.virt_addr)
        {
            LOGE("(%s:L%d) failed to alloc grid!\n", __func__, __LINE__);
            goto allocFail;
        }
        stBufInfo.stBufGrid.size = stDataSize.gridBufLen;
        stBufInfo.stBufGrid.map_size = stDataSize.gridBufLen;
#if 0
        sprintf(szFrBufName, "CreateGDGeo%d", listSize);
        stBufInfo.stBufGeo.virt_addr = fr_alloc_single(szFrBufName, stDataSize.geoBufLen, &stBufInfo.stBufGeo.phys_addr);
        if (NULL == stBufInfo.stBufGeo.virt_addr)
        {
            LOGE("(%s:L%d) failed to alloc geo!\n", __func__, __LINE__);
            goto allocFail;
        }
        stBufInfo.stBufGeo.size = stDataSize.geoBufLen;
        stBufInfo.stBufGeo.map_size = stDataSize.geoBufLen;
#endif
        hasNewBuf = true;
        ret = 0;
    }
    else
    {
        if (!m_stFce.modeGroupIdleList.empty())
        {
            int idleIndex = 0;
            int foundGroup = false;

            idleIndex = m_stFce.modeGroupIdleList.front();
            m_stFce.modeGroupIdleList.pop_front();

            idleIt = m_stFce.stModeGroupAllList.begin();
            while (idleIt != m_stFce.stModeGroupAllList.end())
            {
                if (idleIt->groupID == idleIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++idleIt;
            }
            if (foundGroup)
            {
                stBufInfo = idleIt->stModeAllList.at(0).stBufList.at(0);
            }
            if (NULL == stBufInfo.stBufGrid.virt_addr)
            {
                LOGE("(%s:L%d) grid buf is NULL !\n", __func__, __LINE__);
                goto toOut;
            }
            if (stDataSize.gridBufLen > stBufInfo.stBufGrid.size)
            {
                LOGE("(%s:%d) Error new grid size %d > old size %d \n", __func__, __LINE__,
                        stDataSize.gridBufLen, stBufInfo.stBufGrid.size);
                goto toOut;
            }
            if (stBufInfo.stBufGeo.virt_addr)
            {
                if (stDataSize.geoBufLen > stBufInfo.stBufGeo.size)
                {
                    LOGE("(%s:%d) Error new geo size %d > old size %d \n", __func__, __LINE__,
                            stDataSize.geoBufLen, stBufInfo.stBufGeo.size);
                    goto toOut;
                }
            }
            ret = 0;
        }
        else
        {
            LOGE("(%s:%d) Warning NO idle buf \n", __func__, __LINE__);
            goto toOut;
        }
    }
    ParseFceDataParam(&pstParam->fec, &stGridParam);
    stGridParam.fisheyeGridSize = stDataSize.gridSize;
    stGridParam.fisheyeImageWidth = stDataSize.inWidth;
    stGridParam.fisheyeImageHeight = stDataSize.inHeight;;
    stGridParam.fisheyeCenterX = fisheyeCenterX;
    stGridParam.fisheyeCenterY = fisheyeCenterY;
    stGridParam.rectImageWidth = stDataSize.outWidth;
    stGridParam.rectImageHeight = stDataSize.outHeight;
    stGridParam.gridBufLen = stDataSize.gridBufLen;
    stGridParam.geoBufLen = stDataSize.geoBufLen;
    stGridParam.fisheyeGridbuf = (unsigned char*)stBufInfo.stBufGrid.virt_addr;
    stGridParam.fisheyeGeobuf = (unsigned char*)stBufInfo.stBufGeo.virt_addr;
    ret = GenGridData(&stGridParam);
    if (ret)
        goto genFail;

    if (hasNewBuf)
    {
        ST_FCE_MODE stMode;

        stGroup.groupID = listSize;
        stGroup.stModeAllList.clear();
        stGroup.modePendingList.clear();
        stGroup.modeActiveList.clear();

        stMode.modeID = 0;
        stMode.stBufList.clear();
        stMode.stPosDnList.clear();
        stMode.stPosUoList.clear();
        stMode.stPosSs0List.clear();
        stMode.stPosSs1List.clear();

        stMode.stBufList.push_back(stBufInfo);
#if 0
        if (m_pPortUo && m_pPortUo->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortUo, 1, &stMode.stPosUoList);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set uo default pos !\n", __func__, __LINE__);
            }
        }
#endif
        if (m_pPortDn && m_pPortDn->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortDn, 1, &stMode.stPosDnList);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set dn default pos !\n", __func__, __LINE__);
            }
        }
        if (m_pPortSs0 && m_pPortSs0->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortSs0, 1, &stMode.stPosSs0List);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set ss0 default pos !\n", __func__, __LINE__);
            }
        }
        if (m_pPortSs1 && m_pPortSs1->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortSs1, 1, &stMode.stPosSs1List);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set ss1 default pos !\n", __func__, __LINE__);
            }
        }
        stGroup.stModeAllList.push_back(stMode);
        stGroup.modeActiveList.push_back(0);

        m_stFce.stModeGroupAllList.push_back(stGroup);
        m_stFce.modeGroupActiveList.push_back(stGroup.groupID);
        m_stFce.hasFce = true;
        m_HasFceData = true;
    }
    else
    {
        idleIt->modeActiveList.push_back(0);
        idleIt->modePendingList.clear();
        m_stFce.modeGroupActiveList.push_back(idleIt->groupID);
    }
    pthread_mutex_unlock(&m_FceLock);
    return 0;

allocFail:
genFail:
    if (hasNewBuf)
    {
        if (stBufInfo.stBufGrid.virt_addr)
            fr_free_single(stBufInfo.stBufGrid.virt_addr, stBufInfo.stBufGrid.phys_addr);
        if (stBufInfo.stBufGeo.virt_addr)
            fr_free_single(stBufInfo.stBufGeo.virt_addr, stBufInfo.stBufGeo.phys_addr);
    }
calcSizeFail:
toOut:
    pthread_mutex_unlock(&m_FceLock);

    LOGE("(%s:%d) ret=%d\n", __func__, __LINE__, ret);
    return ret;
}

#endif //MACRO_IPU_ISPOSTV1_FCE

int IPU_ISPOSTV1::IspostLoadAndFireFceData(cam_fcefile_param_t *pstParam)
{
    int ret = 0;
    int modeGroupSize = 0;


    pthread_mutex_lock(&m_FceLock);
    if (m_HasFceData)
    {
        ret = -1;
        LOGE("(%s:L%d) FCE had existed !\n", __func__, __LINE__);
        goto toOut;
    }
    m_stFce.maxModeGroupNb = LOADFCEDATA_MAX;
    modeGroupSize = m_stFce.stModeGroupAllList.size();
    if (modeGroupSize < m_stFce.maxModeGroupNb)
    {
        ST_FCE_MODE_GROUP stGroup;

        stGroup.groupID = modeGroupSize;
        stGroup.stModeAllList.clear();
        stGroup.modePendingList.clear();
        stGroup.modeActiveList.clear();

        ret = ParseFceFileParam(&stGroup, stGroup.groupID, pstParam);
        if (ret)
            goto toOut;
        m_stFce.stModeGroupAllList.push_back(stGroup);
        m_stFce.modeGroupActiveList.push_back(modeGroupSize);
        m_stFce.hasFce = true;
        m_HasFceFile = true;
    }
    else
    {
        if (!m_stFce.modeGroupIdleList.empty())
        {
            int idleIndex = 0;
            ST_FCE_MODE_GROUP stIdleGroup;
            std::vector<ST_FCE_MODE_GROUP>::iterator idleIt;
            int foundGroup = false;

            idleIndex = m_stFce.modeGroupIdleList.front();
            m_stFce.modeGroupIdleList.pop_front();
            stIdleGroup = m_stFce.stModeGroupAllList.at(idleIndex);

            idleIt = m_stFce.stModeGroupAllList.begin();
            while (idleIt != m_stFce.stModeGroupAllList.end())
            {
                if (idleIt->groupID == idleIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++idleIt;
            }
            if (foundGroup)
            {
                ret = ClearFceModeGroup(&(*idleIt));
                if (ret)
                    goto toOut;
            }
            stIdleGroup.modePendingList.clear();
            stIdleGroup.modeActiveList.clear();
            stIdleGroup.stModeAllList.clear();
            ret = ParseFceFileParam(&stIdleGroup, stIdleGroup.groupID, pstParam);
            if (ret)
                goto toOut;
            ret = UpdateFceModeGroup(&m_stFce, &stIdleGroup);
            if (ret)
                goto toOut;
            m_stFce.modeGroupActiveList.push_back(stIdleGroup.groupID);
        }
        else
        {
            LOGE("%s:%d Warning NO idle buf !!!\n", __func__, __LINE__);
            ret = -1;
            goto toOut;
        }
    }
    pthread_mutex_unlock(&m_FceLock);
    return 0;

toOut:
    pthread_mutex_unlock(&m_FceLock);
    LOGE("(%s:%d) ret=%d \n", __func__, __LINE__, ret);

    return ret;
}

int IPU_ISPOSTV1::IspostSetFceMode(int stMode)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;
    int maxMode = 0;


    LOGD("(%s:L%d) stMode=%d \n", __func__, __LINE__, stMode);
    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        std::vector<ST_FCE_MODE_GROUP>::iterator it;
        bool found = false;

        groupIndex = m_stFce.modeGroupPendingList.back();

        it = m_stFce.stModeGroupAllList.begin();
        while (it != m_stFce.stModeGroupAllList.end())
        {
            if (it->groupID == groupIndex)
            {
                found = true;
                break;
            }
            it++;
        }
        if (found)
        {
            maxMode = it->stModeAllList.size();
            if (stMode < maxMode)
            {
                it->modeActiveList.push_back(stMode);
                ret = 0;
            }
        }
    }
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}


int IPU_ISPOSTV1::IspostGetFceMode(int *pMode)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;


    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        groupIndex = m_stFce.modeGroupPendingList.back();
        *pMode = (m_stFce.stModeGroupAllList.at(groupIndex)).modePendingList.back();
        ret = 0;
    }
    pthread_mutex_unlock(&m_FceLock);

    LOGD("(%s:L%d) ret=%d stMode=%d\n", __func__, __LINE__, ret, *pMode);

    return ret;
}

int IPU_ISPOSTV1::IspostGetFceTotalModes(int *pModeCount)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;


    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        groupIndex = m_stFce.modeGroupPendingList.back();
        *pModeCount = (m_stFce.stModeGroupAllList.at(groupIndex)).stModeAllList.size();
        ret = 0;
    }
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}

int IPU_ISPOSTV1::IspostSaveFceData(cam_save_fcedata_param_t *pstParam)
{
    int ret = -1;
    int groupIndex = 0;
    int hasWork = 0;
    int modeIndex = 0;
    char szFileName[200];
    ST_FCE_MODE stMode;
    int nbBuf = 0;
    int i = 0;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    FILE *pFd = NULL;
    ST_FCE_MODE_GROUP stModeGroup;


    pthread_mutex_lock(&m_FceLock);
    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (hasWork)
    {
        groupIndex = m_stFce.modeGroupPendingList.back();
        stModeGroup = m_stFce.stModeGroupAllList.at(groupIndex);

        if (stModeGroup.modePendingList.empty())
            goto toOut;
        modeIndex = stModeGroup.modePendingList.back();

        stMode = m_stFce.stModeGroupAllList.at(groupIndex).stModeAllList.at(modeIndex);
        nbBuf = stMode.stBufList.size();
        for (i = 0; i < nbBuf; ++i)
        {
            stBufInfo = stMode.stBufList.at(i);
            if (stBufInfo.stBufGrid.size > 0)
            {
                snprintf(szFileName, sizeof(szFileName), "%s_grid%d.bin", pstParam->file_name, i);
                pFd = fopen(szFileName, "wb");
                if (pFd)
                {
                    fwrite(stBufInfo.stBufGrid.virt_addr, stBufInfo.stBufGrid.size, 1, pFd);
                    fclose(pFd);
                }
                else
                {
                    ret = -1;
                    LOGE("(%s:L%d) failed to open file %s !!!\n", __func__, __LINE__, szFileName);
                    break;
                }
            }
            if (stBufInfo.stBufGeo.size > 0)
            {
                snprintf(szFileName, sizeof(szFileName), "%s_geo%d.bin", pstParam->file_name, i);
                pFd = fopen(szFileName, "wb");
                if (pFd)
                {
                    fwrite(stBufInfo.stBufGeo.virt_addr, stBufInfo.stBufGrid.size, 1, pFd);
                    fclose(pFd);
                }
                else
                {
                    ret = -1;
                    LOGE("(%s:L%d) failed to open file %s !!!\n", __func__, __LINE__, szFileName);
                    break;
                }
            }
        }
        ret = 0;
    }
toOut:
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}

int IPU_ISPOSTV1::UpdateFceModeGroup(ST_FCE *pstFce, ST_FCE_MODE_GROUP *pstGroup)
{
    int ret = -1;
    int found = false;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;


    if (NULL == pstFce || NULL == pstGroup)
    {
        LOGE("(%s:L%d) param %p %p is NULL !!!\n", __func__, __LINE__, pstFce, pstGroup);
        return -1;
    }
    it = pstFce->stModeGroupAllList.begin();
    while (it != pstFce->stModeGroupAllList.end())
    {
        if (it->groupID == pstGroup->groupID)
        {
            found = true;
            break;
        }
        ++it;
    }
    if (found)
    {
        m_stFce.stModeGroupAllList.erase(it);
        m_stFce.stModeGroupAllList.insert(it, *pstGroup);
        ret = 0;
    }

    return ret;
}

int IPU_ISPOSTV1::GetFceActiveMode(ST_FCE_MODE *pstMode)
{
    int ret = -1;
    int groupIndex = 0;
    int modeIndex = 0;
    ST_FCE_MODE_GROUP stActiveGroup;
    int foundGroup = false;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;


    if (!m_stFce.modeGroupActiveList.empty())
    {
        LOGD("(%s:L%d) active size=%d \n", __func__, __LINE__, m_stFce.modeGroupActiveList.size());

        groupIndex = m_stFce.modeGroupActiveList.front();
        m_stFce.modeGroupActiveList.pop_front();

        it = m_stFce.stModeGroupAllList.begin();
        while (it != m_stFce.stModeGroupAllList.end())
        {
            if(it->groupID == groupIndex)
            {
                foundGroup = true;
                break;
            }
            ++it;
        }
        if (foundGroup)
        {
            if (!it->modeActiveList.empty())
            {
                modeIndex = it->modeActiveList.front();
                it->modeActiveList.pop_front();
                *pstMode = it->stModeAllList.at(modeIndex);

                LOGD("(%s:L%d) active modeIndex=%d \n", __func__, __LINE__, modeIndex);

                it->modePendingList.push_back(modeIndex);
                m_stFce.modeGroupPendingList.push_back(groupIndex);
                if (!m_HasFceData)
                    m_ui32DnRefImgBufYaddr = 0xFFFFFFFF;
                ret = 0;
            }
            else
            {
                LOGE("(%s:L%d) failed to get active stMode! \n", __func__, __LINE__);
            }
        }

    }
    else
    {
        if (!m_stFce.modeGroupPendingList.empty())
        {
            groupIndex = m_stFce.modeGroupPendingList.back();

            it = m_stFce.stModeGroupAllList.begin();
            while (it != m_stFce.stModeGroupAllList.end())
            {
                if (it->groupID == groupIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++it;
            }
            if (foundGroup)
            {
                if (!it->modeActiveList.empty())
                {
                    modeIndex = it->modeActiveList.front();
                    it->modeActiveList.pop_front();

                    it->modePendingList.push_back(modeIndex);
                    *pstMode = it->stModeAllList.at(modeIndex);
                    if (!m_HasFceData)
                        m_ui32DnRefImgBufYaddr =  0xFFFFFFFF;
                    ret = 0;
                    LOGD("(%s:L%d) new active index=%d \n", __func__, __LINE__, modeIndex);
                }
                else if (!it->modePendingList.empty())
                {
                    modeIndex = it->modePendingList.back();
                    *pstMode = it->stModeAllList.at(modeIndex);
                    ret = 0;
                    LOGD("(%s:L%d) exist active index=%d \n", __func__, __LINE__, modeIndex);
                }
                else
                {
                    LOGE("(%s:L%d) failed to get active stMode !\n", __func__, __LINE__);
                }
            }
        }
        else
        {
            LOGE("(%s:L%d) Warning NO Worked stMode group \n", __func__, __LINE__);
        }
    }

    LOGD("(%s:L%d) ret=%d\n", __func__, __LINE__, ret);

    return ret;
}

int IPU_ISPOSTV1::UpdateFcePendingStatus()
{
    int ret = 0;
    int groupIndex = 0;


    if (!m_stFce.modeGroupPendingList.empty())
    {
        if (m_stFce.modeGroupPendingList.size() > 1)
        {
            groupIndex = m_stFce.modeGroupPendingList.front();
            m_stFce.modeGroupPendingList.pop_front();

            m_stFce.modeGroupIdleList.push_back(groupIndex);
            LOGD("(%s:L%d) NEW pop group index=%d \n", __func__, __LINE__, groupIndex);
        }
        else
        {
            int groupIndex = 0;
            int foundGroup = false;
            std::vector<ST_FCE_MODE_GROUP>::iterator it;


            groupIndex = m_stFce.modeGroupPendingList.back();
            it = m_stFce.stModeGroupAllList.begin();
            while (it != m_stFce.stModeGroupAllList.end())
            {
                if (it->groupID == groupIndex)
                {
                    foundGroup = true;
                    break;
                }
                ++it;
            }
            if (foundGroup)
            {
                if (it->modePendingList.size() > 1)
                {
                    LOGD("(%s:L%d) NEW pop mode=%d \n", __func__, __LINE__, it->modePendingList.front());
                    it->modePendingList.pop_front();
                }
            }
        }
    }

    return ret;
}

int IPU_ISPOSTV1::ClearFceModeGroup(ST_FCE_MODE_GROUP *pstModeGroup)
{
    if (NULL == pstModeGroup)
    {
        LOGE("(%s:L%d) param is NULL !!!\n", __func__, __LINE__);
        return -1;
    }
    if (!pstModeGroup->stModeAllList.empty())
    {
        int nbBuf = 0;
        int i = 0;
        std::vector<ST_FCE_MODE>::iterator modeIt;
        ST_GRIDDATA_BUF_INFO stBufInfo;


        LOGD("(%s:L%d) groupID=%d stMode size=%d \n", __func__, __LINE__,
                pstModeGroup->groupID, pstModeGroup->stModeAllList.size());
        modeIt = pstModeGroup->stModeAllList.begin();
        while (modeIt != pstModeGroup->stModeAllList.end())
        {
            nbBuf = modeIt->stBufList.size();
            LOGD("(%s:L%d) nbBuf=%d \n", __func__, __LINE__, nbBuf);
            for (i = 0; i < nbBuf; ++i)
            {
                stBufInfo = modeIt->stBufList.at(i);
                if (stBufInfo.stBufGrid.virt_addr)
                {
                    fr_free_single(stBufInfo.stBufGrid.virt_addr, stBufInfo.stBufGrid.phys_addr);
                    LOGD("(%s:L%d) free grid phys=0x%X \n", __func__, __LINE__, stBufInfo.stBufGrid.phys_addr);
                }
                if (stBufInfo.stBufGeo.virt_addr)
                {
                    fr_free_single(stBufInfo.stBufGeo.virt_addr, stBufInfo.stBufGeo.phys_addr);
                }
            }
            modeIt->stBufList.clear();
            modeIt->stPosDnList.clear();
            modeIt->stPosUoList.clear();
            modeIt->stPosSs0List.clear();
            modeIt->stPosSs1List.clear();

            pstModeGroup->stModeAllList.erase(modeIt);
        }
        pstModeGroup->modeActiveList.clear();
        pstModeGroup->modePendingList.clear();
    }

    return 0;
}

int IPU_ISPOSTV1::ParseFceFileParam(ST_FCE_MODE_GROUP *pstModeGroup, int groupID, cam_fcefile_param_t *pstParam)
{
    int ret = -1;
    cam_fcefile_mode_t *pstParamMode = NULL;
    cam_fcefile_file_t *pstParamFile = NULL;
    cam_fce_port_type_e portType = CAM_FCE_PORT_NONE;

    int nbMode = 0;
    int nbFile = 0;
    int nbPort = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    ST_FCE_MODE stMode;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    cam_position_t stPos;
    struct stat stStatBuf;
    char szFrBufName[50];
    FILE *pFd = NULL;
    char *pszFileGrid = NULL;
    char *pszFileGeo = NULL;
    int nbNoPos = 0;


    if (NULL == pstModeGroup || NULL == pstParam)
    {
        LOGE("(%s:L%d) params %p %p are NULL !!!\n", __func__, __LINE__, pstModeGroup, pstParam);
        return -1;
    }
    pstModeGroup->groupID = groupID;
    pstModeGroup->stModeAllList.clear();
    pstModeGroup->modePendingList.clear();
    pstModeGroup->modeActiveList.clear();

    nbMode = pstParam->mode_number;
    if (0 == nbMode)
        goto toOut;
    LOGD("(%s:L%d) nbMode %d \n", __func__, __LINE__, nbMode);
    for (i = 0; i < nbMode; ++i)
    {
        nbNoPos = 0;
        stMode.modeID = i;
        stMode.stBufList.clear();
        stMode.stPosDnList.clear();
        stMode.stPosUoList.clear();
        stMode.stPosSs0List.clear();
        stMode.stPosSs1List.clear();

        pstParamMode = &pstParam->mode_list[i];
        nbFile = pstParamMode->file_number;
        LOGD("(%s:L%d) nbFile %d \n", __func__, __LINE__, nbFile);
        for (j = 0; j < nbFile; ++j)
        {
            memset(&stBufInfo, 0, sizeof(stBufInfo));
            pstParamFile = &pstParamMode->file_list[j];

#if defined(COMPILE_ISPOSTV1_IQ_TUNING_TOOL_APIS)
            if ('\0' == m_szGridFilePath[1][0]) {
                int iIdx = 0;
                char *pszTmp;

                pszTmp = strrchr(pstParamFile->file_grid, '/');
                if (NULL != pszTmp) {
                    iIdx = &pszTmp[0] - &pstParamFile->file_grid[0];
                    iIdx++;
                    strncpy(m_szGridFilePath[1], pstParamFile->file_grid, (unsigned int)iIdx);
                    m_szGridFilePath[1][iIdx] = '\0';
                }
            }

#endif
            pszFileGrid = pstParamFile->file_grid;
            pFd = fopen(pszFileGrid, "rb");
            if (NULL == pFd)
            {
                LOGE("(%s:L%d) Failed to open %s !!!\n", __func__, __LINE__, pszFileGrid);
                goto openFail;
            }
            stat(pszFileGrid, &stStatBuf);
            sprintf(szFrBufName, "GDfile%dgrid%d_%d", groupID, i, j);
            stBufInfo.stBufGrid.virt_addr = fr_alloc_single(szFrBufName, stStatBuf.st_size, &stBufInfo.stBufGrid.phys_addr);
            if (!stBufInfo.stBufGrid.virt_addr)
            {
                LOGE("(%s:L%d) buf_name=%s \n", __func__, __LINE__, szFrBufName);
                goto allocFail;
            }
            stBufInfo.stBufGrid.size = stStatBuf.st_size;
            stBufInfo.stBufGrid.map_size = stStatBuf.st_size;
            fread(stBufInfo.stBufGrid.virt_addr, stStatBuf.st_size, 1, pFd);
            fclose(pFd);

            pszFileGeo = pstParamFile->file_geo;
            if (strlen(pszFileGeo) > 0)
            {
                pFd = fopen(pszFileGeo, "rb");
                if (NULL == pFd)
                {
                    LOGE("(%s:L%d) Failed to open %s !!!\n", __func__, __LINE__, pszFileGeo);
                    goto openFail;
                }
                stat(pszFileGeo, &stStatBuf);
                sprintf(szFrBufName, "GDfilegeo%d_%d_%d", groupID, i, j);
                stBufInfo.stBufGeo.virt_addr = fr_alloc_single(szFrBufName, stStatBuf.st_size, &stBufInfo.stBufGeo.phys_addr);
                if (!stBufInfo.stBufGeo.virt_addr)
                {
                    LOGE("(%s:L%d) buf_name=%s \n", __func__, __LINE__, szFrBufName);
                    goto allocFail;
                }
                stBufInfo.stBufGeo.size = stStatBuf.st_size;
                stBufInfo.stBufGeo.map_size = stStatBuf.st_size;
                fread(stBufInfo.stBufGeo.virt_addr, stStatBuf.st_size, 1, pFd);
                fclose(pFd);
            }
            nbPort = pstParamFile->outport_number;
            LOGD("(%s:L%d) nbPort=%d \n", __func__, __LINE__, nbPort);
            if (nbPort)
            {
                for (k = 0; k < nbPort; ++k)
                {
                    portType = pstParamFile->outport_list[k].type;
                    stPos = pstParamFile->outport_list[k].pos;
                    switch (portType)
                    {
                    case CAM_FCE_PORT_UO:
                        stMode.stPosUoList.push_back(stPos);
                        break;
                    case CAM_FCE_PORT_DN:
                        stMode.stPosDnList.push_back(stPos);
                        break;
                    case CAM_FCE_PORT_SS0:
                        stMode.stPosSs0List.push_back(stPos);
                        break;
                    case CAM_FCE_PORT_SS1:
                        stMode.stPosSs1List.push_back(stPos);
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                ++nbNoPos;
            }
            stMode.stBufList.push_back(stBufInfo);
        }
        if (nbNoPos == nbFile)
        {
            LOGD("(%s:L%d) nbNoPos %d \n", __func__, __LINE__, nbNoPos);
#if 0
            if (m_pPortUo && m_pPortUo->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortUo, nbFile, &stMode.stPosUoList);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
#endif
            if (m_pPortDn && m_pPortDn->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortDn, nbFile, &stMode.stPosDnList);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
            if (m_pPortSs0 && m_pPortSs0->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortSs0, nbFile, &stMode.stPosSs0List);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
            if (m_pPortSs1 && m_pPortSs1->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortSs1, nbFile, &stMode.stPosSs1List);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
        }
        pstModeGroup->stModeAllList.push_back(stMode);
        if (i == 0)
        {
            pstModeGroup->modeActiveList.push_back(0);
        }
    }
    return 0;

defaultPosFail:
allocFail:
openFail:
    ClearFceModeGroup(pstModeGroup);
toOut:

    LOGE("(%s:L%d) ret=%d\n", __func__, __LINE__, ret);
    return ret;
}

void IPU_ISPOSTV1::FreeFce()
{
    if (!m_stFce.stModeGroupAllList.empty())
    {
        std::vector<ST_FCE_MODE_GROUP>::iterator it;

        it = m_stFce.stModeGroupAllList.begin();
        while(it != m_stFce.stModeGroupAllList.end())
        {
            ClearFceModeGroup(&(*it));
            it++;
        }
        m_stFce.stModeGroupAllList.clear();
        m_stFce.modeGroupIdleList.clear();
        m_stFce.modeGroupActiveList.clear();
        m_stFce.modeGroupPendingList.clear();
    }
}

void IPU_ISPOSTV1::PrintAllFce(ST_FCE *pstFce, const char *pszFileName, int line)
{
    int nbBuf = 0;
    int nbPort = 0;
    int i = 0;
    int j = 0;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;
    std::vector<ST_FCE_MODE>::iterator modeIt;
    std::list<int>::iterator intIt;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    int listSize = 0;
    cam_position_t stPos;


    listSize = pstFce->stModeGroupAllList.size();
    LOGE("(%s:L%d) listSize=%d all start---------\n", pszFileName, line, listSize);

    it = pstFce->stModeGroupAllList.begin();
    for (; it != pstFce->stModeGroupAllList.end(); ++it)
    {
        LOGE("groupID=%d Mode:%d all +++++++++\n", it->groupID, it->stModeAllList.size());
        modeIt = it->stModeAllList.begin();
        for(; modeIt != it->stModeAllList.end(); ++modeIt)
        {
            nbBuf = modeIt->stBufList.size();
            LOGE("modeID=%d file:%d =======\n", modeIt->modeID, nbBuf);
            for (i = 0; i < nbBuf; ++i)
            {
                stBufInfo = modeIt->stBufList.at(i);
                LOGE("grid phys=0x%X, geo phys=0x%X \n",
                                stBufInfo.stBufGrid.phys_addr, stBufInfo.stBufGeo.phys_addr);
            }
            nbPort = modeIt->stPosDnList.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosDnList.at(j);
                LOGE("DN_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
            nbPort = modeIt->stPosUoList.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosUoList.at(j);
                LOGE("UO_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
            nbPort = modeIt->stPosSs0List.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosSs0List.at(j);
                LOGE("SS0_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
            nbPort = modeIt->stPosSs1List.size();
            for (j = 0; j < nbPort; ++j)
            {
                stPos = modeIt->stPosSs1List.at(j);
                LOGE("SS1_%d (%d,%d,%d,%d) \n", j, stPos.x, stPos.y, stPos.width, stPos.height);
            }
        }
        LOGE("group=%d modePending:%d  --------\n", it->groupID, it->modePendingList.size());
        intIt = it->modePendingList.begin();
        for(; intIt != it->modePendingList.end(); ++intIt)
        {
            LOGE("modeID=%d \n", *intIt);
        }
        LOGE("group=%d modeActive:%d  --------\n", it->groupID, it->modeActiveList.size());
        intIt = it->modeActiveList.begin();
        for(; intIt != it->modeActiveList.end(); ++intIt)
        {
            LOGE("modeID=%d \n", *intIt);
        }
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);

    listSize = pstFce->modeGroupPendingList.size();
    LOGE("(%s:L%d) listSize=%d pending start---------\n", pszFileName, line, listSize);
    intIt = pstFce->modeGroupPendingList.begin();
    for (; intIt != pstFce->modeGroupPendingList.end(); ++intIt)
    {
        LOGE("groupID=%d \n", *intIt);
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);

    listSize = pstFce->modeGroupIdleList.size();
    LOGE("(%s:L%d) listSize=%d idle start---------\n", pszFileName, line, listSize);
    intIt = pstFce->modeGroupIdleList.begin();
    for (; intIt != pstFce->modeGroupIdleList.end(); ++intIt)
    {
        LOGE("groupID=%d \n", *intIt);
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);

    listSize = pstFce->modeGroupActiveList.size();
    LOGE("(%s:L%d) listSize=%d active start---------\n", pszFileName, line, listSize);
    intIt = pstFce->modeGroupActiveList.begin();
    for (; intIt != pstFce->modeGroupActiveList.end(); ++intIt)
    {
        LOGE("groupID=%d \n", *intIt);
    }
    LOGE("(%s:L%d) end----------\n", pszFileName, line);
}

int IPU_ISPOSTV1::SetFceDefaultPos(Port *pPort, int fileNumber, std::vector<cam_position_t> *pPosList)
{
    int ret = -1;
    cam_position_t stPos;
    int i = 0;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;


    if (NULL == pPort || 0 == fileNumber || NULL == pPosList)
    {
        LOGE("(%s:L%d) param %p %d %p has error !\n", __func__, __LINE__, pPort, fileNumber, pPosList);
        return -1;
    }
    memset(&stPos, 0, sizeof(stPos));
    width = pPort->GetWidth();
    height = pPort->GetHeight();
    switch (fileNumber)
    {
    case 1:
        stPos.x = 0;
        stPos.y = 0;
        stPos.width = width;
        stPos.height = height;
        pPosList->push_back(stPos);
        ret = 0;
        break;
    case 2:
        height /= 2;
        stPos.width = width;
        stPos.height = height;
        for (i = 0; i < fileNumber; ++i, pPosList->push_back(stPos))
        {
            switch (i)
            {
            case 0:
                x = 0;
                y = 0;
                ret = 0;
                break;
            case 1:
                x = 0;
                y = height;
                break;
            default:
                break;
            }
            stPos.x = x;
            stPos.y = y;
        }
        ret = 0;
        break;
    case 3:
    case 4:
        width /= 2;
        height /= 2;
        stPos.width = width;
        stPos.height = height;
        for (i = 0; i < fileNumber; ++i, pPosList->push_back(stPos))
        {
            switch (i)
            {
            case 0:
                x = 0;
                y = 0;
                ret = 0;
                break;
            case 1:
                x = width;
                y = 0;
                break;
            case 2:
                x = 0;
                y = height;
                break;
            case 3:
                x = width;
                y = height;
                break;
            default:
                break;
            }
            stPos.x = x;
            stPos.y = y;
            LOGD("(%s:L%d) pos (%d,%d,%d,%d) \n", __func__, __LINE__, x, y, width, height);
        }
        ret = 0;
        break;
    default:
        ret = -1;
        LOGE("(%s:L%d) NOT support file number %d \n", __func__, __LINE__, fileNumber);
        break;
    }

    return ret;
}

int IPU_ISPOSTV1::ParseFceFileJson(char *fileName, cam_fcefile_param_t *pstParam)
{
    Json js;
    int i = 0;
    int j = 0;
    int k = 0;
    char strTag[50];
    int modeNumber = 0;
    int fileNumber = 0;
    int portNumber = 0;
    const char *pszString = NULL;
    cam_fcefile_mode_t *pstMode = NULL;
    cam_fcefile_file_t *pstFile = NULL;
    cam_fcefile_port_t *pstPort = NULL;


    LOGD("(%s:L%d) fileName=%s \n", __func__, __LINE__, fileName);
    js.Load(fileName);

    sprintf(strTag, "mode_number");
    modeNumber = js.GetObject(strTag) ? js.GetInt(strTag):0;
    LOGD("(%s:L%d) mode_number=%d \n", __func__, __LINE__, modeNumber);


    pstParam->mode_number = modeNumber;
    for (i = 0; i < modeNumber; ++i)
    {
        sprintf(strTag, "mode_list%d", i);
        Json *mode_js = js.GetObject(strTag);
        if (mode_js)
        {
            pstMode = &pstParam->mode_list[i];

            sprintf(strTag, "file_number");
            fileNumber = mode_js->GetObject(strTag) ? mode_js->GetInt(strTag):0;
            LOGD("(%s:L%d) file_number=%d \n", __func__, __LINE__, fileNumber);


            pstMode->file_number = fileNumber;
            for (j = 0; j < fileNumber; ++j)
            {
                sprintf(strTag, "file_list%d", j);
                Json *file_js = mode_js->GetObject(strTag);
                if (file_js)
                {
                    pstFile = &pstMode->file_list[j];

                    sprintf(strTag, "file_grid");
                    pszString = file_js->GetString(strTag);
                    if (pszString)
                    {
                        snprintf(pstFile->file_grid, sizeof(pstFile->file_grid),
                                    "%s",pszString);
                        LOGD("(%s:L%d) %s=%s\n", __func__, __LINE__, strTag, pstFile->file_grid);
                    }
                    sprintf(strTag, "file_geo");
                    pszString = file_js->GetString(strTag);
                    if (pszString)
                    {
                        snprintf(pstFile->file_geo, sizeof(pstFile->file_geo),
                                    "%s",pszString);
                        LOGD("(%s:L%d) %s=%s\n", __func__, __LINE__, strTag, pstFile->file_geo);
                    }

                    sprintf(strTag, "outport_number");
                    portNumber = file_js->GetObject(strTag) ? file_js->GetInt(strTag):0;
                    pstFile->outport_number = portNumber;
                    for (k = 0; k < portNumber; ++k)
                    {
                        sprintf(strTag, "outport_list%d", k);
                        Json *port_js = file_js->GetObject(strTag);
                        if (port_js)
                        {
                            pstPort = &pstFile->outport_list[k];

                            sprintf(strTag, "port");
                            pszString = port_js->GetString(strTag);
                            if (pszString)
                            {
                                if (0 == strncmp("uo", pszString, 2))
                                    pstPort->type = CAM_FCE_PORT_UO;
                                if (0 == strncmp("ss0", pszString, 3))
                                    pstPort->type = CAM_FCE_PORT_SS0;
                                if (0 == strncmp("ss1", pszString, 3))
                                    pstPort->type = CAM_FCE_PORT_SS1;
                                if (0 == strncmp("dn", pszString, 2))
                                    pstPort->type = CAM_FCE_PORT_DN;
                            }

                            sprintf(strTag, "x");
                            pstPort->pos.x = port_js->GetObject(strTag) ? port_js->GetInt(strTag):0;
                            sprintf(strTag, "y");
                            pstPort->pos.y = port_js->GetObject(strTag) ? port_js->GetInt(strTag):0;
                            sprintf(strTag, "w");
                            pstPort->pos.width = port_js->GetObject(strTag) ? port_js->GetInt(strTag):0;
                            sprintf(strTag, "h");
                            pstPort->pos.height = port_js->GetObject(strTag) ? port_js->GetInt(strTag):0;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int IPU_ISPOSTV1::SetupLcInfo()
{
    // IN ----------------------------------------
    if (m_pPortIn->IsEnabled()){
        m_IspostUser.ui32LcSrcWidth = m_pPortIn->GetWidth();
        m_IspostUser.ui32LcSrcHeight = m_pPortIn->GetHeight();
        m_IspostUser.ui32LcSrcStride = m_IspostUser.ui32LcSrcWidth;
    }

    if (m_pPortDn->IsEnabled() || m_IspostUser.stCtrl0.ui1EnDN)
    {
        if (m_pPortDn->IsEmbezzling())
        {
            m_IspostUser.ui32LcDstWidth = m_pPortDn->GetPipWidth();
            m_IspostUser.ui32LcDstHeight = m_pPortDn->GetPipHeight();
        }
        else
        {
            m_IspostUser.ui32LcDstWidth = m_pPortDn->GetWidth();
            m_IspostUser.ui32LcDstHeight = m_pPortDn->GetHeight();
        }
    }
    else if ((m_s32LcOutImgWidth != 0) && (m_s32LcOutImgHeight != 0))
    {
        m_IspostUser.ui32LcDstWidth = m_s32LcOutImgWidth;
        m_IspostUser.ui32LcDstHeight = m_s32LcOutImgHeight;
    }
    else
    {
        LOGE("Error:no Lc out image params");
        return -1;
    }

    return 0;
}
