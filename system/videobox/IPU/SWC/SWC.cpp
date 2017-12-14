// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
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
#include "SWC.h"
#include "q3ispost.h"
#include "q3denoise.h"

#define PIXEL_ALIGN(x,pixel) (((x+pixel-1)/pixel)*pixel)

DYNAMIC_IPU(IPU_SWC, "swc");
static const ISPOST_UINT32 g_SwcdwGridSize[] = { 8, 16, 32 };
static struct ipu_swc_3d_dn_param g_SwcdnParamList[] = {
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
static ISPOST_UINT32 SwcMaxLevelCnt = sizeof(g_SwcdnParamList) / sizeof(g_SwcdnParamList[0]);

#define MAX_INPUT 4
#define MASK_INPUT_CHANNEL_0 0x1
#define MASK_INPUT_CHANNEL_1 0x2
#define MASK_INPUT_CHANNEL_2 0x4
#define MASK_INPUT_CHANNEL_3 0x8

#define PP_PIXEL_FORMAT_YUV420_MASK         0x020000U

static int switchFormat(Pixel::Format srcFormat)
{
    if(srcFormat == Pixel::Format::RGBA8888)    //Pixel::Format::RGBA8888
        return PP_PIX_FMT_BGR32; // 0x41001  PP_PIX_FMT_RGB32
    else if(srcFormat == Pixel::Format::NV12) //Pixel::Format::NV12
        return PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;// 0x20001
    else if(srcFormat == Pixel::Format::NV21) //Pixel::Format::NV21
        return PP_PIX_FMT_YCRCB_4_2_0_SEMIPLANAR;  //0x20003
    else if(srcFormat == Pixel::Format::RGB565)//Pixel::Format::RGB565
        return PP_PIX_FMT_RGB16_5_6_5;
    else if(srcFormat == Pixel::Format::YUYV422)
           return PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED;
     else
        throw VBERR_BADPARAM;
}

void IPU_SWC::CalculateDispRect(int index,struct v_pip_info *pipinfo,enum v_nvr_display_mode mode)
{
    if(mode == VNVR_QUARTER_SCREEN_MODE)
    {
        pipinfo->w = pipinfo->pic_w /2;
        pipinfo->h = pipinfo->pic_h /2;

        switch(index)
        {
            case 0:
            {
                pipinfo->x = 0;
                pipinfo->y = 0;
                break;
            }
            case 1:
            {
                pipinfo->x = pipinfo->pic_w /2;
                pipinfo->y = 0;
                break;
            }
            case 2:
            {
                pipinfo->x = 0;
                pipinfo->y = pipinfo->pic_h /2;
                break;
            }
            case 3:
            {
                pipinfo->x = pipinfo->pic_w /2;
                pipinfo->y = pipinfo->pic_h /2;
                break;
            }
            default:
            break;
        }
    }
    else if(mode == VNVR_FULL_SCREEN_MODE)
    {
         pipinfo->x = 0;
         pipinfo->y = 0;
         pipinfo->w = pipinfo->pic_w;
         pipinfo->h = pipinfo->pic_h;
    }
    else if(mode == VNVR_ONE_THREE_MODE)
    {
        //Todo
    }
    pipinfo->x = PIXEL_ALIGN(pipinfo->x,16);
    pipinfo->y = PIXEL_ALIGN(pipinfo->y,16);
    pipinfo->w = PIXEL_ALIGN(pipinfo->w,8);
    pipinfo->h = PIXEL_ALIGN(pipinfo->h,8);
    LOGD("index:%d  In vpinfo x:%d y:%d w:%d h:%d   ori w:%d h:%d\n",index,pipinfo->x,pipinfo->y,pipinfo->w,pipinfo->h,pipinfo->pic_w,pipinfo->pic_h);
}

void IPU_SWC::SetInputParamPp(Port *pt,uint32_t phys_addr,struct v_pip_info *infoin)
{
    pPpConf.ppInImg.width = pt->GetWidth();;
    pPpConf.ppInImg.height = pt->GetHeight();
    pPpConf.ppInImg.videoRange = 1;
    pPpConf.ppInImg.pixFormat = switchFormat(pt->GetPixelFormat());
    pPpConf.ppInImg.bufferBusAddr = phys_addr;
    pPpConf.ppInImg.bufferCbBusAddr = pPpConf.ppInImg.bufferBusAddr + pPpConf.ppInImg.width * pPpConf.ppInImg.height;
    pPpConf.ppInImg.bufferCrBusAddr = pPpConf.ppInImg.bufferCbBusAddr  + (pPpConf.ppInImg.width * pPpConf.ppInImg.height) /4;

    if(infoin != NULL)
    {
        pPpConf.ppInCrop.enable = 1;
        pPpConf.ppInCrop.originX = infoin->x;
        pPpConf.ppInCrop.originY = infoin->y;
        pPpConf.ppInCrop.width = infoin->w;
        pPpConf.ppInCrop.height = infoin->h;
        LOGD("w:%x h:%x crop(%x %x %x %x)\n",pPpConf.ppInImg.width,pPpConf.ppInImg.height,pPpConf.ppInCrop.originX,pPpConf.ppInCrop.originY,pPpConf.ppInCrop.width,pPpConf.ppInCrop.height);
    }
    else
    {
        pPpConf.ppInCrop.enable = 0;
    }
}


void IPU_SWC::SetOutputParamPp(Port *pt,Buffer &buf, struct v_pip_info *info)
{
    pPpConf.ppOutImg.width = info->w;
    pPpConf.ppOutImg.height = info->h;
    pPpConf.ppOutFrmBuffer.enable = 1;
    pPpConf.ppOutFrmBuffer.frameBufferWidth = pt->GetWidth();
    pPpConf.ppOutFrmBuffer.frameBufferHeight= pt->GetHeight();
    pPpConf.ppOutFrmBuffer.writeOriginX = info->x;
    pPpConf.ppOutFrmBuffer.writeOriginY = info->y;
    pPpConf.ppOutImg.pixFormat = switchFormat(pt->GetPixelFormat());
    pPpConf.ppOutImg.bufferBusAddr = buf.fr_buf.phys_addr;
    if(pPpConf.ppOutImg.pixFormat & PP_PIXEL_FORMAT_YUV420_MASK){
        pPpConf.ppOutImg.bufferChromaBusAddr = pPpConf.ppOutImg.bufferBusAddr
            + pPpConf.ppOutFrmBuffer.frameBufferWidth * pPpConf.ppOutFrmBuffer.frameBufferHeight;
    }
    LOGD("framebuffer w:%d h:%d \n",pPpConf.ppOutFrmBuffer.frameBufferWidth,pPpConf.ppOutFrmBuffer.frameBufferHeight);
}
void IPU_SWC::SetInputParamIspost(Port *pin,uint32_t phys_addr,struct v_pip_info *infoin)
{
    if(infoin == NULL)
    {
        m_IspostUser.ui32LcSrcStride = pin->GetWidth();
        m_IspostUser.ui32LcSrcWidth = pin->GetWidth();
        m_IspostUser.ui32LcSrcHeight = pin->GetHeight();

        m_IspostUser.ui32LcSrcBufYaddr = phys_addr;
        m_IspostUser.ui32LcSrcBufUVaddr = m_IspostUser.ui32LcSrcBufYaddr + (m_IspostUser.ui32LcSrcWidth * m_IspostUser.ui32LcSrcHeight);
    }
    else
    {
        //need new grid data to check
        m_IspostUser.ui32LcSrcStride = pin->GetWidth();
        m_IspostUser.ui32LcSrcWidth = infoin->w;
        m_IspostUser.ui32LcSrcHeight = infoin->h;

        m_IspostUser.ui32LcSrcBufYaddr = phys_addr;
        m_IspostUser.ui32LcSrcBufUVaddr = m_IspostUser.ui32LcSrcBufYaddr + (pin->GetWidth() * pin->GetHeight());
        IspostCalcOffsetAddress(infoin->x, infoin->y, m_IspostUser.ui32LcSrcStride,
            m_IspostUser.ui32LcSrcBufYaddr, m_IspostUser.ui32LcSrcBufUVaddr);
        //printf("pip(%d %d %d %d)\n",infoin->x, infoin->y,infoin->w, infoin->h);
    }
}


void IPU_SWC::SetOutputParamIspost(Port *pout,Buffer &buf, struct v_pip_info *info)
{
    m_IspostUser.ui32SS0OutImgBufStride = pout->GetWidth();
    m_IspostUser.ui32SS0OutImgDstWidth = info->w;
    m_IspostUser.ui32SS0OutImgDstHeight = info->h;

    m_IspostUser.ui32SS0OutImgBufYaddr = buf.fr_buf.phys_addr;
    m_IspostUser.ui32SS0OutImgBufUVaddr = m_IspostUser.ui32SS0OutImgBufYaddr + (pout->GetWidth() * pout->GetHeight());

    IspostCalcOffsetAddress(info->x, info->y, m_IspostUser.ui32SS0OutImgBufStride,
                        m_IspostUser.ui32SS0OutImgBufYaddr, m_IspostUser.ui32SS0OutImgBufUVaddr);
}


bool IPU_SWC::IspostJsonGetString(const char* pszItem, char*& pszString,Json *js)
{
    pszString = NULL;

    if (js == NULL)
        return false;

    Json* pSub = js->GetObject(pszItem);
    if (pSub == NULL)
        return false;

    pszString = pSub->valuestring;
    LOGD("GetStr: %s = %s\n", pszItem, pszString);
    return true;
}


bool IPU_SWC::IspostInitial(void)
{
    // DN ----------------------------------------
    m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
    m_IspostUser.ui32LcDstWidth = m_pPortDn->GetWidth();
    m_IspostUser.ui32LcDstHeight = m_pPortDn->GetHeight();

    m_IspostUser.ui32DnOutImgBufStride = m_pPortDn->GetWidth();
    m_IspostUser.ui32DnRefImgBufStride = m_pPortDn->GetWidth();

    LOGD("DN:%d %d %d %d\n",m_IspostUser.ui32LcDstWidth,m_IspostUser.ui32LcDstHeight,m_IspostUser.ui32DnOutImgBufStride,m_IspostUser.ui32DnRefImgBufStride);

     // SS0 ----------------------------------------
    m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_ENABLE;
    m_IspostUser.ui32SS0OutImgBufStride =  m_pPortOut->GetWidth();
    m_IspostUser.ui32SS0OutImgDstWidth = m_pPortOut->GetWidth();//will   change i n trigger
    m_IspostUser.ui32SS0OutImgDstHeight = m_pPortOut->GetHeight();//will   change i n trigger
    LOGD("ss0:%d %d %d\n",m_IspostUser.ui32SS0OutImgBufStride,m_IspostUser.ui32SS0OutImgDstWidth,m_IspostUser.ui32SS0OutImgDstHeight);
    return true;
}


void IPU_SWC::IspostCheckParameter(Port *pin)
{
    if ((pin->GetPipX() < 0) ||
        (pin->GetPipY() < 0) ||
        (pin->GetWidth() < 0) ||
        (pin->GetHeight() < 0) ||
        (pin->GetPipX() % 2 != 0) ||
        (pin->GetPipY() % 2 != 0) ||
        (pin->GetWidth() % 8 != 0) ||
        (pin->GetHeight() % 8 != 0) ||
        (pin->GetPipX() > pin->GetWidth()) ||
        (pin->GetPipY() > pin->GetHeight()) ||
        (pin->GetPipWidth() > pin->GetWidth()) ||
        (pin->GetPipHeight() > pin->GetHeight()) ||
        (pin->GetPipX() + pin->GetPipWidth() > pin->GetWidth()) ||
        (pin->GetPipY() + pin->GetPipHeight() > pin->GetHeight())){
        LOGE("ISPost Crop params error\n");
        throw VBERR_BADPARAM;
    }

    if ((pin->GetPixelFormat() != Pixel::Format::NV12) &&
        (pin->GetPixelFormat() != Pixel::Format::NV21)) {
        LOGE("ISPost PortIn format params error\n");
        throw VBERR_BADPARAM;
    }
}

ISPOST_UINT32 IPU_SWC::IspostGetGridTargetIndexMax(int width, int height, int sz)
{
    ISPOST_UINT32 ui32Stride;
    ISPOST_UINT32 ui32GridSize;
    ISPOST_UINT32 ui32Width;
    ISPOST_UINT32 ui32Height;

    if (GRID_SIZE_MAX <= sz) {
        sz = GRID_SIZE_8_8;
    }

    ui32GridSize = g_SwcdwGridSize[sz];
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

ISPOST_UINT32 IPU_SWC::IspostGetFileSize(FILE* pFileID)
{
    ISPOST_UINT32 pos = ftell(pFileID);
    ISPOST_UINT32 len = 0;
    fseek(pFileID, 0L, SEEK_END);
    len = ftell(pFileID);
    fseek(pFileID, pos, SEEK_SET);
    return len;
}

bool IPU_SWC::IspostLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane)
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

ISPOST_UINT32 IPU_SWC::IspostParseGirdSize(const char* pszFileName)
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


void IPU_SWC::IspostDefaultParameter(void)
{
    memset(&m_IspostUser, 0, sizeof(ISPOST_USER));
    grid_target_index_max = 0;
    memset(&m_Dns3dAttr, 0, sizeof(m_Dns3dAttr));

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

    //3D-denoise information
    m_IspostUser.ui32DnOutImgBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnOutImgBufUVaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnOutImgBufStride = 0;
    m_IspostUser.ui32DnRefImgBufYaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnRefImgBufUVaddr = 0xFFFFFFFF;
    m_IspostUser.ui32DnRefImgBufStride = 0;
    m_IspostUser.ui32DnTargetIdx = 0;
    m_Dns3dAttr.y_threshold = g_SwcdnParamList[0].y_threshold;
    m_Dns3dAttr.u_threshold = g_SwcdnParamList[0].u_threshold;
    m_Dns3dAttr.v_threshold = g_SwcdnParamList[0].v_threshold;
    m_Dns3dAttr.weight = g_SwcdnParamList[0].weight;
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

void IPU_SWC::IspostCalcOffsetAddress(int x, int y, int stride, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr)
{
    Yaddr += ((y * stride) + x);
    UVaddr += (((y / 2) * stride) + x);
}

void IPU_SWC::IspostDumpUserInfo(void)
{
    LOGE("==============================\n");
    LOGE("stCtrl0: %d %d %d %d %d\n",
                    m_IspostUser.stCtrl0.ui1EnLC,
                    m_IspostUser.stCtrl0.ui1EnOV,
                    m_IspostUser.stCtrl0.ui1EnDN,
                    m_IspostUser.stCtrl0.ui1EnSS0,
                    m_IspostUser.stCtrl0.ui1EnSS1);
    LOGE("ui32UpdateFlag: 0x%04X\n", m_IspostUser.ui32UpdateFlag);
    LOGE("ui32PrintFlag: 0x%04X\n", m_IspostUser.ui32PrintFlag);
    LOGE("ui32IspostCount: %d\n", m_IspostUser.ui32IspostCount);

    LOGE("------------------------------\n");
    LOGE("ui32LcSrcBufYaddr: 0x%08X\n", m_IspostUser.ui32LcSrcBufYaddr);
    LOGE("ui32LcSrcBufUVaddr: 0x%08X\n", m_IspostUser.ui32LcSrcBufUVaddr);
    LOGE("ui32LcSrcWidth: %d\n", m_IspostUser.ui32LcSrcWidth);
    LOGE("ui32LcSrcHeight: %d\n", m_IspostUser.ui32LcSrcHeight);
    LOGE("ui32LcSrcStride: %d\n", m_IspostUser.ui32LcSrcStride);
    LOGE("ui32LcDstWidth: %d\n", m_IspostUser.ui32LcDstWidth);
    LOGE("ui32LcDstHeight: %d\n", m_IspostUser.ui32LcDstHeight);
    LOGE("------------------------------\n");

    LOGE("ui32DnOutImgBufYaddr: 0x%08X\n", m_IspostUser.ui32DnOutImgBufYaddr);
    LOGE("ui32DnOutImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32DnOutImgBufUVaddr);
    LOGE("ui32DnOutImgBufStride: %d\n", m_IspostUser.ui32DnOutImgBufStride);
    LOGE("ui32DnRefImgBufYaddr: 0x%08X\n", m_IspostUser.ui32DnRefImgBufYaddr);
    LOGE("ui32DnRefImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32DnRefImgBufUVaddr);
    LOGE("ui32DnRefImgBufStride: %d\n", m_IspostUser.ui32DnRefImgBufStride);
    LOGE("ui32DnTargetIdx: %d\n", m_IspostUser.ui32DnTargetIdx);
    LOGE("------------------------------\n");

    LOGE("ui32SS0OutImgBufYaddr: 0x%08X\n", m_IspostUser.ui32SS0OutImgBufYaddr);
    LOGE("ui32SS0OutImgBufUVaddr: 0x%08X\n", m_IspostUser.ui32SS0OutImgBufUVaddr);
    LOGE("ui32SS0OutImgBufStride: %d\n", m_IspostUser.ui32SS0OutImgBufStride);
    LOGE("ui32SS0OutImgDstWidth: %d\n", m_IspostUser.ui32SS0OutImgDstWidth);
    LOGE("ui32SS0OutImgDstHeight: %d\n", m_IspostUser.ui32SS0OutImgDstHeight);
    LOGE("ui32SS0OutImgMaxWidth: %d\n", m_IspostUser.ui32SS0OutImgMaxWidth);
    LOGE("ui32SS0OutImgMaxHeight: %d\n", m_IspostUser.ui32SS0OutImgMaxHeight);
    LOGE("ui32SS0OutHorzScaling: %d\n", m_IspostUser.ui32SS0OutHorzScaling);
    LOGE("ui32SS0OutVertScaling: %d\n", m_IspostUser.ui32SS0OutVertScaling);
    LOGE("------------------------------\n");
}

int IPU_SWC::IspostProcPrivateData(ipu_v2500_private_data_t *pdata)

{
    int ret = -1;
    cam_3d_dns_attr_t *pattr = NULL;
    cam_common_t *pcmn = NULL;
    ISPOST_UINT32 autoCtrlIdx = 0;
    cam_3d_dns_attr_t autoDnsAttr;

    if(pdata == NULL)
        return 1;

    pattr = &pdata->dns_3d_attr;
    if(pattr == NULL)
        return 1;


    pcmn = (cam_common_t*)&pattr->cmn;

    if (CAM_CMN_AUTO == pcmn->mode)
    {
        memset(&autoDnsAttr, 0, sizeof(autoDnsAttr));

        autoCtrlIdx = pdata->iDnID;
        if ((m_IspostUser.ui32DnTargetIdx != autoCtrlIdx)
            && (autoCtrlIdx < SwcMaxLevelCnt))
        {
            autoDnsAttr.y_threshold = g_SwcdnParamList[autoCtrlIdx].y_threshold;
            autoDnsAttr.u_threshold = g_SwcdnParamList[autoCtrlIdx].u_threshold;
            autoDnsAttr.v_threshold = g_SwcdnParamList[autoCtrlIdx].v_threshold;
            autoDnsAttr.weight = g_SwcdnParamList[autoCtrlIdx].weight;
            pattr = &autoDnsAttr;
            m_IspostUser.ui32DnTargetIdx = autoCtrlIdx;
        }
        else
        {
            pattr = &m_Dns3dAttr;
        }
    }

    ret = IspostSet3dDnsCurve(pattr, 0);

    return ret;
}

int IPU_SWC::IspostSet3dDnsCurve(const cam_3d_dns_attr_t *attr, int is_force)
{
    int ret = -1;
    unsigned char weighting = 1;
    unsigned char threshold[3]={20,20,20};
    Q3_REG_VALUE hermiteTable[Q3_DENOISE_REG_NUM];
    ISPOST_UINT32 i = 0;
    ISPOST_UINT32 j = 0;
    int idx = 0;


    if(attr == NULL)
        return -1;

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
bool IPU_SWC::IspostTrigger(Buffer& BufDn)
{

    int ret = 0;
    static ISPOST_UINT32 ui32DnRefImgBufYaddr;

    if (m_IspostUser.ui32IspostCount == 0)
        ui32DnRefImgBufYaddr =  0xFFFFFFFF;

    //DN
    {
        m_IspostUser.ui32LcDstWidth = m_pPortDn->GetWidth();
        m_IspostUser.ui32LcDstHeight = m_pPortDn->GetHeight();

        m_IspostUser.ui32DnOutImgBufYaddr = BufDn.fr_buf.phys_addr;
        m_IspostUser.ui32DnOutImgBufUVaddr = m_IspostUser.ui32DnOutImgBufYaddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());

        m_IspostUser.ui32DnRefImgBufYaddr = ui32DnRefImgBufYaddr;
        m_IspostUser.ui32DnRefImgBufUVaddr = m_IspostUser.ui32DnRefImgBufYaddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());

        ui32DnRefImgBufYaddr = m_IspostUser.ui32DnOutImgBufYaddr;
    }

    //IspostDumpUserInfo();

    m_IspostUser.ui32IspostCount++;

    ret = ispost_trigger(&m_IspostUser);

    if(ret < 0)
        LOGE("ispost_trigger error ret:%d\n",ret);
    return (ret >= 0) ? true : false;
}

void IPU_SWC::WorkLoop()
{
    PPResult ppRet = PP_OK;
    Buffer bfout,BufDn;
    Buffer bfin[MAX_INPUT];
    Port *pout;
    Port *pin[MAX_INPUT];
    int index = 0;
    struct v_pip_info vpinfoout[MAX_INPUT];
    struct v_pip_info vpinfoin;

    int display_flag = 0;
    bool bfill = false;
    uint32_t phys_addr = 0;
    int framerate = 60;
    int serial[MAX_INPUT] = {0,0,0,0};

    struct timeval tv_begin;
    long StartTime = 0;
    long EndTime = 0;
    long DelayTime = 0;

    //#define TIME_CHECK
    #ifdef TIME_CHECK
    long Time2 = 0;
    long Timeget[MAX_INPUT] = {0,0,0,0};
    #endif
    int rdy = 0;
    enum v_nvr_display_mode nvr_mode = VNVR_INVALID_MODE;

    if(composertool == PP_G1)
    {
        ppRet = PPGetConfig(pPpInst, &(pPpConf));
        if(ppRet != PP_OK) {
            LOGE("PPGetConfig failed while process an image:\n");
            return ;
        }
    }

    pin[0] = GetPort("in0");
    pin[1] = GetPort("in1");
    pin[2] = GetPort("in2");
    pin[3] = GetPort("in3");
    pout = GetPort("out");
    memset(&vpinfoin,0,sizeof(vpinfoin));
    memset(vpinfoout,0,sizeof(vpinfoout));

    while(IsInState(ST::Running)){

        gettimeofday(&tv_begin, NULL);
        StartTime = tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000;

        display_flag = this->channel_flag;
        if(nvr_mode != this->mode)
        {
            nvr_mode = this->mode;
            for(index = 0;index < MAX_INPUT;index++)
            {
                vpinfoout[index].pic_w = pout->GetWidth();
                vpinfoout[index].pic_h = pout->GetHeight();
                CalculateDispRect(index,&(vpinfoout[index]),nvr_mode);
            }
            if(composertool == ISPOST_V1)
            {
                IspostInitial();
            }
        }

        //skip or not decide by Delay time
        if(DelayTime > 1000 /framerate)
        {
            DelayTime -= (1000 /framerate);
            for(index = 0;index < MAX_INPUT;index++)
            {
                if((display_flag &(1<<index)) && (pin[index]->IsEnabled())){
                    try {
                            rdy = pin[index]->CheckBufferRdy(serial[index]);
                            //if only one ready not skip
                            //LOGE("skip index:%d rdy:%d\n",index,rdy);
                            if(rdy > 1)
                            {
                                bfin[index] = pin[index]->GetBuffer(&bfin[index]);
                                serial[index] = bfin[index].fr_buf.serial;
                                pin[index]->PutBuffer(&bfin[index]);
                            }
                    }
                    catch(const char* err){
                    }
                }
            }
        }

        try {
                bfout = pout->GetBuffer();
        }
        catch (const char* err) {
            usleep(10*1000); //10 ms
            continue;
        }

        if(composertool == ISPOST_V1)
        {
            try {
                    BufDn = m_pPortDn->GetBuffer();
            }
            catch (const char* err) {
                usleep(10*1000); //10 ms
                goto PUTOUT;
            }
        }

        #ifdef TIME_CHECK
        gettimeofday(&tv_begin, NULL);
        LOGE("outbuffer:%ld ms\n",(tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000 - StartTime));
        #endif


        for(index = 0;index < MAX_INPUT;index++)
        {
            if((display_flag &(1<<index)) && (pin[index]->IsEnabled())){
                if(pin[index]->GetFPS() >= 5 && (pin[index]->GetFPS() <= 60))
                {
                    if(framerate < pin[index]->GetFPS())
                    {
                        framerate = pin[index]->GetFPS();
                        LOGE("index(%d) fps:%d  [in0]:%d [in1]:%d [in2]:%d [in3]:%d\n",index, framerate,pin[0]->GetFPS(),pin[1]->GetFPS(),pin[2]->GetFPS(),pin[3]->GetFPS());
                    }
                }

                try {

                        #ifdef TIME_CHECK
                        gettimeofday(&tv_begin, NULL);
                        Time2 = tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000;
                        #endif

                        bfill = false;
                        rdy = pin[index]->CheckBufferRdy(serial[index]);

                        //kernel use diff to check
                        if(rdy <= 0)
                        {
                            //LOGE("index(%d) no valid input \n",index);
                            bfill = true;
                        }
                        else
                        {
                            bfin[index] = pin[index]->GetBuffer(&bfin[index]);
                            serial[index] = bfin[index].fr_buf.serial;

#ifdef TIME_CHECK
                        gettimeofday(&tv_begin, NULL);
                        LOGE("(%d)getget:%ld ms\n",index,(tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000 - Timeget[index]));
                        Timeget[index] = tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000;
#endif
                        }

                        if(bfill == true)
                        {
                            if(phys_addr == 0)
                            {
                                //first time skip,no valid previous picture
                                continue;
                            }
                            //use previous out buffer to fill
                            if(composertool == PP_G1)
                                SetInputParamPp(pout,phys_addr,&(vpinfoout[index]));
                            else if(composertool == ISPOST_V1)
                            {
                                //ispost case not work  now need different gird data
                                continue;
                                SetInputParamIspost(pout,phys_addr,&(vpinfoout[index]));
                            }
                        }
                        else
                        {
                            if(composertool == PP_G1)
                                SetInputParamPp(pin[index],bfin[index].fr_buf.phys_addr,NULL);
                            else  if(composertool == ISPOST_V1)
                            {
                                //ispost case
                                SetInputParamIspost(pin[index],bfin[index].fr_buf.phys_addr,NULL);
                            }
                        }
#ifdef TIME_CHECK
                        gettimeofday(&tv_begin, NULL);
                        LOGE("(%d)inbuffer:%ld ms\n",index,(tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000 - Time2));
                        Time2 = tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000;
#endif

                        if(composertool == PP_G1)
                        {
                            //OUT
                            vpinfoin.w = pPpConf.ppInImg.width;
                            vpinfoin.h = pPpConf.ppInImg.height;
                            if(CheckParameterPp(&vpinfoin,&(vpinfoout[index])) == false)
                            {
                                if(bfill == false)
                                    pin[index]->PutBuffer(&bfin[index]);
                                continue;
                            }

                            LOGD("index:%d  In vpinfo x:%d y:%d w:%d h:%d   ori:%d %d\n",index,vpinfoout[index].x,vpinfoout[index].y
                                ,vpinfoout[index].w,vpinfoout.[index].h,vpinfoout[index].pic_w,vpinfoout[index].pic_h);

                            SetOutputParamPp(pout,bfout,&(vpinfoout[index]));
                            ppRet = PPSetConfig(pPpInst, &pPpConf);

                            if(ppRet != PP_OK) {
                                LOGE("PPSetConfig failed while process an image :%d\n", ppRet);
                            }
                            #ifdef TIME_CHECK
                            gettimeofday(&tv_begin, NULL);
                            Time2 = tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000;
                            #endif

                            ppRet = PPGetResult(pPpInst);

                            #ifdef TIME_CHECK
                            gettimeofday(&tv_begin, NULL);
                            LOGE("(%d)PPGetResult:%ldms\n",index,(tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000 - Time2));
                            #endif

                            if(ppRet != PP_OK) {
                                LOGE("PPGetResult failed while process an image :%d\n", ppRet);
                            }
                        }
                        else if(composertool == ISPOST_V1)
                        {
                            //ispost case
                            SetOutputParamIspost(pout,bfout,&(vpinfoout[index]));
                            //IspostProcPrivateData((ipu_v2500_private_data_t*)bfin[index].fr_buf.priv);
                            #ifdef TIME_CHECK
                            gettimeofday(&tv_begin, NULL);
                            Time2 = tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000;
                            #endif
                            IspostTrigger(BufDn);
                            #ifdef TIME_CHECK
                            gettimeofday(&tv_begin, NULL);
                            LOGE("(%d)IspostTrigger:%ld ms\n",index,(tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000 - Time2));
                            #endif

                        }
                        if(bfill == false)
                            pin[index]->PutBuffer(&bfin[index]);
                }
                catch (const char* err) {
                    printf("swc error %s\n",err);
                }
            }
        }

        gettimeofday(&tv_begin, NULL);
        EndTime = tv_begin.tv_sec*1000 + tv_begin.tv_usec/1000;
        #ifdef TIME_CHECK
        LOGE("loop Time:%ld ms\n",EndTime - StartTime);
        #endif

        if(EndTime-StartTime < (1000 /framerate - 3))
        {
            usleep((1000 /framerate - (EndTime-StartTime))*1000);
        }
        else if(EndTime-StartTime > (1000 /framerate))
        {
            DelayTime += (EndTime-StartTime > (1000 /framerate));
        }

        if(composertool == ISPOST_V1)
            m_pPortDn->PutBuffer(&BufDn);
        phys_addr = bfout.fr_buf.phys_addr;
        PUTOUT:
        pout->PutBuffer(&bfout);
    }

}

IPU_SWC::IPU_SWC(std::string name, Json *js)
{

    int display_mode = 0;
    int m_Buffers = 3;
    Name = name;
    this->composertool = ISPOST_V1;

    this->mode = VNVR_QUARTER_SCREEN_MODE;
    this->channel_flag = 0x1 + 0x2 + 0x4 + 0x8;

    if(NULL != js){

         if(0 != js->GetInt("display_mode"))
         {
            display_mode = js->GetInt("display_mode");
            switch (display_mode)
            {
                case 1:
                    this->channel_flag = 0x1;
                    break;
                 case 2:
                    this->channel_flag = 0x2;
                     break;
                 case 3:
                    this->channel_flag = 0x4;
                     break;
                 case 4:
                    this->channel_flag = 0x8;
                     break;
                 case 5:
                 {
                     this->mode = VNVR_FULL_SCREEN_MODE;
                     this->channel_flag = 0x1;
                      break;
                 }
                 case 6:
                 {
                     this->mode = VNVR_FULL_SCREEN_MODE;
                     this->channel_flag = 0x2;
                      break;
                 }
                 case 7:
                 {
                     this->mode = VNVR_FULL_SCREEN_MODE;
                     this->channel_flag = 0x4;
                      break;
                 }
                 case 8:
                 {
                     this->mode = VNVR_FULL_SCREEN_MODE;
                     this->channel_flag = 0x8;
                      break;
                 }
                 case 9:
                    this->channel_flag = 0x1 + 0x2;
                     break;
                 case 10:
                    this->channel_flag = 0x1 + 0x2 + 0x4;
                     break;
                 case 11:
                    this->channel_flag = 0x1 + 0x2 + 0x4 + 0x8;
                     break;
                 default:
                    this->channel_flag = 0x1;
                    break;
             }
         }
    }

    CreatePort("in0", Port::In);
    CreatePort("in1", Port::In);
    CreatePort("in2", Port::In);
    CreatePort("in3", Port::In);

    if(composertool == ISPOST_V1)
    {
        m_pPortDn = CreatePort("dn", Port::Out);
        m_pPortDn->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
        m_pPortDn->SetResolution(1920, 1088);
        m_pPortDn->SetPixelFormat(Pixel::Format::NV12);
        m_pPortDn->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);
        m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
        if(m_IspostUser.stCtrl0.ui1EnDN)
        {
           // export PortDn if dn function is enabled, which will
           // make system allocate buffer for PortDn
           m_pPortDn->Export();
        }
    }

    m_pPortOut = CreatePort("out", Port::Out);
    m_pPortOut->SetBufferType(FRBuffer::Type::FIXED, 3);
    m_pPortOut->SetResolution(1920, 1088);
    m_pPortOut->SetPixelFormat(Pixel::Format::NV12);
    m_pPortOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    m_pPortOut->Enable();

    if(composertool == ISPOST_V1)
    {
        int i;
        char *pszString;
        char szItemName[80];
        bool bdefaultGrid = true;

        for (i = 0; i < IPU_SWC_GRIDDATA_MAX; i++)
        {
            m_pszLcGridFileName[i] = NULL;
            m_pLcGridBuf[i] = NULL;
        }

        IspostDefaultParameter();

        //----------------------------------------------------------------------------------------------
        // Parse IPU parameter
        if (js != NULL)
        {
            //----------------------------------------------------------------------------------------------
            // Parse lc_config parameter

            for (i = 0; i < IPU_SWC_GRIDDATA_MAX; i++)
            {
                sprintf(szItemName, "lc_grid_file_name%d", i+1);
                if (IspostJsonGetString(szItemName, pszString,js))
                {
                    m_pszLcGridFileName[i] = strdup(pszString);
                    bdefaultGrid = false;
                }
            }

        }
        if(bdefaultGrid == true)
        {
            m_pszLcGridFileName[0] = strdup("/root/.ispost/lc_v1_hermite32_1920x1088_scup_0~30.bin");
        }

        ispost_open();

    }
}

IPU_SWC::~IPU_SWC()
{
    int i;
    if(composertool  == ISPOST_V1)
    {
        for (i = 0; i < IPU_SWC_GRIDDATA_MAX; i++)
        {
            if (m_pszLcGridFileName[i] != NULL)
            {
                free(m_pszLcGridFileName[i]);
                m_pszLcGridFileName[i]  = NULL;
            }
            if (m_pLcGridBuf[i] != NULL)
            {
                delete m_pLcGridBuf[i];
                m_pLcGridBuf[i] = NULL;
            }
        }

        ispost_close();
    }
}
void IPU_SWC::Prepare()
{
    int i;
    Buffer buf;
    char BufName[20];
    PPResult  ppRet;
    Port *pin = NULL;

    CheckPortAttribute();

    if(composertool  == PP_G1)
    {
        ppRet = PPInit(&this->pPpInst, 0);
        if(ppRet != PP_OK) {
            LOGE("PPInit failed with %d\n", ppRet);
            throw VBERR_BADPARAM;
        }
    }
    else if(composertool  == ISPOST_V1)
    {
        for(i = 0;i < MAX_INPUT;i++)
        {
            sprintf(BufName, "in%d",i);
            pin = GetPort(BufName);
            if(pin->IsEnabled())
                IspostCheckParameter(pin);
        }

        //Load LcGridBuf
        for (i = 0; i < IPU_SWC_GRIDDATA_MAX; i++){
            if (m_pszLcGridFileName[i] != NULL){
                sprintf(BufName, "LcGridBuf%d", i+1);
                m_IspostUser.stCtrl0.ui1EnLC = ISPOST_ENABLE;
                m_IspostUser.ui32LcGridSize = IspostParseGirdSize(m_pszLcGridFileName[i]);
                LOGD("m_pszLcGridFileName[%d]: %s, GridSize: %d\n", i, m_pszLcGridFileName[i], m_IspostUser.ui32LcGridSize);
                IspostLoadBinaryFile(BufName, &m_pLcGridBuf[i], m_pszLcGridFileName[i]);
            }
        }

        if (m_IspostUser.ui32LcScanModeScanW > m_IspostUser.ui32LcGridSize) {
            m_IspostUser.ui32LcScanModeScanW = m_IspostUser.ui32LcGridSize;
        }
        if (m_IspostUser.ui32LcScanModeScanH > m_IspostUser.ui32LcGridSize) {
            m_IspostUser.ui32LcScanModeScanH = m_IspostUser.ui32LcGridSize;
        }

        buf = m_pLcGridBuf[0]->GetBuffer();
        m_IspostUser.ui32LcGridBufAddr = buf.fr_buf.phys_addr;
        m_IspostUser.ui32LcGridBufSize = buf.fr_buf.size;
        m_pLcGridBuf[0]->PutBuffer(&buf);
    }
}

void IPU_SWC::Unprepare()
{
    Port *pIpuPort = NULL;
    char buf[5] = {0};

    pIpuPort = GetPort("dn");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

    for(int i = 0; i < 4; i++)
    {
        sprintf(buf, "in%d", i);
        pIpuPort = GetPort(buf);
        if (pIpuPort && pIpuPort->IsEnabled())
        {
             pIpuPort->FreeVirtualResource();
        }
    }
    if(composertool  == PP_G1)
        PPRelease(this->pPpInst);
}

bool IPU_SWC::CheckParameterPp(struct v_pip_info *infoin,struct v_pip_info *infoout)
{
    if((infoin->w <16) ||(infoin->w >4096)|| (infoin->h <16) ||(infoin->h <16) ||
        (infoout->w <16) ||(infoout->w >4096)|| (infoout->h <16) ||(infoout->h <16) )
    {
        LOGE("CheckParameterPp fail in(w:%d h:%d),out(w:%d h:%d)\n",infoin->w,infoin->h,infoout->w,infoout->h);
        return false;
    }

    if((infoout->w > (3*infoin->w)) ||(infoout->h>(3*infoin->h - 2)))
    {
        LOGE("CheckParameterPp fail PP limit in(w:%d h:%d),out(w:%d h:%d)\n",infoin->w,infoin->h,infoout->w,infoout->h);
        return false;
    }


    if(((infoin->w > infoout->w) && (infoin->h < infoout->h)) ||
        ((infoin->w < infoout->w) && (infoin->h > infoout->h)))
    {
        // Do Crop with input otherwise pp scale check input fail
        if((infoin->w > infoout->w)){
            pPpConf.ppInCrop.enable = 1;
            pPpConf.ppInCrop.originX = (infoin->w > infoout->w)/2;
            pPpConf.ppInCrop.originY = 0;
            pPpConf.ppInCrop.width = infoout->w;
            pPpConf.ppInCrop.height = infoin->h;
        }
        else if(infoin->w < infoout->w)
        {
            pPpConf.ppInCrop.enable = 1;
            pPpConf.ppInCrop.originX = 0;
            pPpConf.ppInCrop.originY = (infoin->h > infoout->h) /2;
            pPpConf.ppInCrop.width = infoin->w;
            pPpConf.ppInCrop.height = infoout->h;
        }
    }
    return true;
}
bool IPU_SWC::CheckPortAttribute()
{
    Port *pIn0, *pIn1, *pIn2,*pIn3;

    pIn0 = GetPort("in0");
    pIn1 = GetPort("in1");
    pIn2 = GetPort("in2");
    pIn3 = GetPort("in3");

    if((pIn0->IsEnabled() && ((pIn0->GetWidth() < 0) || (pIn0->GetHeight() < 0))) ||
        (pIn1->IsEnabled() && ((pIn1->GetWidth() < 0) || (pIn1->GetHeight() < 0))) ||
        (pIn2->IsEnabled() && ((pIn2->GetWidth() < 0) || (pIn2->GetHeight() < 0))) ||
        (pIn3->IsEnabled() && ((pIn3->GetWidth() < 0) || (pIn3->GetHeight() < 0)))){
        LOGE("Crop params error\n");
        throw VBERR_BADPARAM;
    }

    if((pIn0->IsEnabled() && pIn0->GetPixelFormat() !=Pixel::Format::NV12) ||
        (pIn1->IsEnabled() && pIn1->GetPixelFormat() !=Pixel::Format::NV12) ||
        (pIn2->IsEnabled() && pIn2->GetPixelFormat() !=Pixel::Format::NV12) ||
        (pIn3->IsEnabled() && pIn3->GetPixelFormat() !=Pixel::Format::NV12)){
        LOGE("Input Format params error\n");
        throw VBERR_BADPARAM;
    }

    if(m_pPortOut->GetPixelFormat() !=Pixel::Format::NV12){
        LOGE("Out Format Params error\n");
        throw VBERR_BADPARAM;
    }
    if(composertool  == ISPOST_V1)
    {
        if(m_pPortDn->IsEnabled()){
            if ((m_pPortDn->GetPixelFormat() != Pixel::Format::NV12)
                && (m_pPortDn->GetPixelFormat() != Pixel::Format::NV21)){
                LOGE("ISPost PortDn format params error\n");
                throw VBERR_BADPARAM;
            }

            m_pPortDn->SetPipInfo(0, 0, m_pPortDn->GetWidth(), m_pPortDn->GetHeight());
            if (m_pPortDn->GetPipWidth() < 16 || m_pPortDn->GetPipWidth() > 4096 ||
                m_pPortDn->GetPipHeight() < 16 || m_pPortDn->GetPipHeight() > 4096){
                LOGE("ISPost PortDn 16 > pip > 4096  %d %d\n",m_pPortDn->GetPipWidth(),m_pPortDn->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            LOGE("m_pPortDn->IsEnabled() false\n");
        }

        if(m_pPortOut->IsEnabled()){
            m_pPortOut->SetPipInfo(0, 0, m_pPortOut->GetWidth(), m_pPortOut->GetHeight());
            if (m_pPortOut->GetPipWidth() < 16 || m_pPortOut->GetPipWidth() > 4096 ||
                m_pPortOut->GetPipHeight() < 16 || m_pPortOut->GetPipHeight() > 4096){
                LOGE("ISPost not embezzling PortSS0 16 > pip > 4096\n");
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            LOGE("m_pPortOut->IsEnabled() false\n");
        }
    }
    return true;
}

void IPU_SWC::SetNvrMode(struct v_nvr_info *nvr_mode)
{
    LOGE("nvr  mode%d  [%d %d %d %d]\n",nvr_mode->mode,nvr_mode->channel_enable[0],nvr_mode->channel_enable[1],nvr_mode->channel_enable[2],nvr_mode->channel_enable[3]);

    this->channel_flag = nvr_mode->channel_enable[0] + ((nvr_mode->channel_enable[1]) <<1) + ((nvr_mode->channel_enable[2]) <<2) + ((nvr_mode->channel_enable[3]) <<3);
    this->mode = nvr_mode->mode;
}

int IPU_SWC::UnitControl(std::string func, void *arg)
{

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(video_set_nvr_mode) {
        SetNvrMode((struct v_nvr_info *)arg);
        return 0;
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
