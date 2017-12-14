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
#include "ISPOSTv2.h"
#include "q3ispost.h"
#include "q3denoise.h"
#include <qsdk/sys.h>

typedef struct
{
    Port *p_port;
    ISPOST_UINT32 flag;
    Buffer *p_buf;
}local_port_t;

DYNAMIC_IPU(IPU_ISPOSTV2, "ispostv2");
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

void IPU_ISPOSTV2::SetOvPaletteTable(
        ST_OVERLAY_INFO* ov_info, ISPOST_UINT32 flag)
{
    int i = 1;
    if (ov_info == NULL)
        throw VBERR_FR_HANDLE;

    switch(flag)
    {
        case ISPOST_PT_OV1:
            i = 1;
            break;

        case ISPOST_PT_OV2:
            i = 2;
            break;

        case ISPOST_PT_OV3:
            i = 3;
            break;

        case ISPOST_PT_OV4:
            i = 4;
            break;

        case ISPOST_PT_OV5:
            i = 5;
            break;

        case ISPOST_PT_OV6:
            i = 6;
            break;

        case ISPOST_PT_OV7:
            i = 7;
            break;

        default:
            LOGE("The PT[0x%x] can not set ov palette\n", flag);
            i = 0;
            break;
    }

    if (i > 0)
    {
        LOGD("Ov%d:A[1]: 0x%x A[2]:0x%x A[3]:0x%x [4]:0x%x\n", i,
            ov_info->iPaletteTable[0], ov_info->iPaletteTable[1],
            ov_info->iPaletteTable[2], ov_info->iPaletteTable[3]);

        m_IspostUser.stOvInfo.stOvAlpha[i].stValue0.ui32Alpha = ov_info->iPaletteTable[0];
        m_IspostUser.stOvInfo.stOvAlpha[i].stValue1.ui32Alpha = ov_info->iPaletteTable[1];
        m_IspostUser.stOvInfo.stOvAlpha[i].stValue2.ui32Alpha = ov_info->iPaletteTable[2];
        m_IspostUser.stOvInfo.stOvAlpha[i].stValue3.ui32Alpha = ov_info->iPaletteTable[3];
    }

}

void IPU_ISPOSTV2::WorkLoop()
{
    static int ErrorCount = 0;
    int StepCount;
    ISPOST_UINT32 LastPortEnable = 0;
    int HasOneOut = 0;
    int HasOneIn = 0;
    int i = 0;
    const int max_port_set = 13;
    local_port_t PortSet[max_port_set] =
    {
        {m_pPortDn, ISPOST_PT_DN, &m_BufDn},
        {m_pPortOv[0], ISPOST_PT_OV0, &m_BufOv[0]},
        {m_pPortOv[1], ISPOST_PT_OV1, &m_BufOv[1]},
        {m_pPortOv[2], ISPOST_PT_OV2, &m_BufOv[2]},
        {m_pPortOv[3], ISPOST_PT_OV3, &m_BufOv[3]},
        {m_pPortOv[4], ISPOST_PT_OV4, &m_BufOv[4]},
        {m_pPortOv[5], ISPOST_PT_OV5, &m_BufOv[5]},
        {m_pPortOv[6], ISPOST_PT_OV6, &m_BufOv[6]},
        {m_pPortOv[7], ISPOST_PT_OV7, &m_BufOv[7]},
        {m_pPortUo, ISPOST_PT_UO, &m_BufUo},
        {m_pPortSs0, ISPOST_PT_SS0, &m_BufSs0},
        {m_pPortSs1, ISPOST_PT_SS1, &m_BufSs1},
        {NULL, 0}
    };
    local_port_t *pLocalPort = NULL;

    while (IsInState(ST::Running))
    {
        LOGD("Port IN:%d DN:%d, UO:%d SS0:%d SS1:%d OV:%d \n",
            m_pPortIn->IsEnabled(), m_pPortDn->IsEnabled(),
            m_pPortUo->IsEnabled(), m_pPortSs0->IsEnabled(),
            m_pPortSs1->IsEnabled(), m_pPortOv[0]->IsEnabled());
        LOGD("IsLcManual:%d LcManualVal:%d \n", m_IsLcManual, m_LcManualVal);
        LOGD("IsDnManual:%d DnManualVal:%d \n", m_IsDnManual, m_DnManualVal);
        LOGD("IsOvManual:%d OvManualVal:%d \n", m_IsOvManual, m_OvManualVal);

        HasOneOut = 0;
        HasOneIn = 0;
        StepCount = 0;
        m_PortEnable = m_PortGetBuf = 0;
        m_PortEnable |= (m_pPortIn->IsEnabled()) ? ISPOST_PT_IN : 0;
        //force lc working, otherwise error.
        if (m_IsLcManual)
            m_PortEnable |= (m_LcManualVal) ? ISPOST_PT_LC : 0;
        else
            m_PortEnable |= (m_pPortIn->IsEnabled()) ? ISPOST_PT_LC : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal & 0x1) && m_pPortOv[0]->IsEnabled()) ? ISPOST_PT_OV0 : 0;
        else
            m_PortEnable |= (m_pPortOv[0]->IsEnabled()) ? ISPOST_PT_OV0 : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal>>1 & 0x1) && m_pPortOv[1]->IsEnabled()) ? ISPOST_PT_OV1 : 0;
        else
            m_PortEnable |= (m_pPortOv[1]->IsEnabled()) ? ISPOST_PT_OV1 : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal>>2 & 0x1) && m_pPortOv[2]->IsEnabled()) ? ISPOST_PT_OV2 : 0;
        else
            m_PortEnable |= (m_pPortOv[2]->IsEnabled()) ? ISPOST_PT_OV2 : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal>>3 & 0x1) && m_pPortOv[3]->IsEnabled()) ? ISPOST_PT_OV3 : 0;
        else
            m_PortEnable |= (m_pPortOv[3]->IsEnabled()) ? ISPOST_PT_OV3 : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal>>4 & 0x1) && m_pPortOv[4]->IsEnabled()) ? ISPOST_PT_OV4 : 0;
        else
            m_PortEnable |= (m_pPortOv[4]->IsEnabled()) ? ISPOST_PT_OV4 : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal>>5 & 0x1) && m_pPortOv[5]->IsEnabled()) ? ISPOST_PT_OV5 : 0;
        else
            m_PortEnable |= (m_pPortOv[5]->IsEnabled()) ? ISPOST_PT_OV5 : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal>>6 & 0x1) && m_pPortOv[6]->IsEnabled()) ? ISPOST_PT_OV6 : 0;
        else
            m_PortEnable |= (m_pPortOv[6]->IsEnabled()) ? ISPOST_PT_OV6 : 0;

        if (m_IsOvManual)
            m_PortEnable |= ((m_OvManualVal>>7 & 0x1) && m_pPortOv[7]->IsEnabled()) ? ISPOST_PT_OV7 : 0;
        else
            m_PortEnable |= (m_pPortOv[7]->IsEnabled()) ? ISPOST_PT_OV7 : 0;

        if (m_IsDnManual)
            m_PortEnable |= (m_DnManualVal && m_pPortDn->IsEnabled()) ? ISPOST_PT_DN : 0;
        else
            m_PortEnable |= (m_pPortDn->IsEnabled()) ? ISPOST_PT_DN : 0;
        m_PortEnable |= (m_pPortUo->IsEnabled()) ? ISPOST_PT_UO : 0;
        m_PortEnable |= (m_pPortSs0->IsEnabled()) ? ISPOST_PT_SS0 : 0;
        m_PortEnable |= (m_pPortSs1->IsEnabled()) ? ISPOST_PT_SS1 : 0;

        // update hw from port settings
        if (LastPortEnable != m_PortEnable)
        {
            if (!IspostInitial())
            {
                LOGE("IspostInitial Fail!\n");
                break;
            }
            LastPortEnable = m_PortEnable;
        }

        LOGD("m_PortEnable 0x%lx \n", m_PortEnable);

        try
        {
            StepCount++;
            if (m_PortEnable & ISPOST_PT_IN)
            {
                if (m_pPortDn->IsEmbezzling() || m_pPortSs0->IsEmbezzling() || m_pPortSs1->IsEmbezzling()
                    || m_pPortUo->IsEmbezzling())
                    m_BufIn = m_pPortIn->GetBuffer();
                else
                    m_BufIn = m_pPortIn->GetBuffer(&m_BufIn);
                m_PortGetBuf |= ISPOST_PT_IN;
                HasOneIn = 1;
                if (m_PortEnable & ISPOST_PT_LC)
                    m_PortGetBuf |= ISPOST_PT_LC;

                if (m_PortEnable & ISPOST_PT_DN)
                {
                    IspostProcPrivateData((ipu_v2500_private_data_t*)m_BufIn.fr_buf.priv);
                }
            }
            for (i = 0; i < max_port_set - 1; ++i)
            {
                pLocalPort = &PortSet[i];
                if (pLocalPort
                    && pLocalPort->p_port
                    && pLocalPort->p_buf)
                {
                    try
                    {
                        if (m_PortEnable & pLocalPort->flag)
                        {
                            if (pLocalPort->p_port->IsEmbezzling())
                                (*pLocalPort->p_buf) = pLocalPort->p_port->GetBuffer(pLocalPort->p_buf);
                            else
                                (*pLocalPort->p_buf) = pLocalPort->p_port->GetBuffer();
                            m_PortGetBuf |= pLocalPort->flag;

                           if (pLocalPort->flag & ISPOST_PT_OV1_7)
                                SetOvPaletteTable((ST_OVERLAY_INFO*)((*pLocalPort->p_buf).fr_buf.priv), pLocalPort->flag);

                            if (Port::In == pLocalPort->p_port->GetDirection())
                                HasOneIn = 1;
                            else
                                HasOneOut = 1;
                        }
                    }
                    catch(const char* err)
                    {
                        if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                            LOGE("videobox interrupted out 1.\n");
                            PutAllBuffer();
                            return;
                        }
                        if (m_IspostUser.ui32IspostCount%60000 == 0)
                            LOGE("Waring: %s cannot get buffer\n", pLocalPort->p_port->GetName().c_str());
                        usleep(100);
                    }
                }
            }
        }
        catch (const char* err)
        {
            usleep(20000);
            ErrorCount++;
            if (!strncmp(err, VBERR_FR_INTERRUPTED, strlen(VBERR_FR_INTERRUPTED) - 1)) {
                LOGE("videobox interrupted out 2.\n");
                PutAllBuffer();
                return;
            }
            if (StepCount == 1 && (ErrorCount % 500 == 0))
                LOGD("Warning: Thread Port In cannot get buffer, ErrorCnt: %d. StepCount: %d\n", ErrorCount, StepCount);
            PutAllBuffer();
            continue;
        }

        LOGD("m_PortGetBuf=0x%lx HasOneIn=%d, HasOneOut=%d \n",\
                m_PortGetBuf, HasOneIn, HasOneOut);

        if (HasOneIn && HasOneOut && m_PortGetBuf != 0)
        {
            if (m_LinkMode)
            {
                if (m_PortGetBuf & ISPOST_PT_UO)
                {
                    m_BufUo.fr_buf.phys_addr = m_BufIn.fr_buf.phys_addr;
                    pthread_mutex_lock(&m_BufLock);
                    m_stInBufList.push_back(m_BufIn.fr_buf);
                    pthread_mutex_unlock(&m_BufLock);
                }
            }
            else if (m_SnapMode && m_SnapBufMode == 1)
            {
                pthread_mutex_lock(&m_SnapBufLock);
                m_pSnapBufFunc = std::bind(&IPU_ISPOSTV2::ReturnSnapBuf,this,std::placeholders::_1);
                if (m_pPortUo->IsEnabled())
                {
                    m_pPortUo->frBuffer->RegCbFunc(m_pSnapBufFunc);
                    m_pPortUo->SyncBinding();
                }

                if (m_PortGetBuf & ISPOST_PT_UO
                     &&m_pPortUo->GetFrameSize() > 0
                     &&m_SnapUoBuf.fr_buf.phys_addr == 0)
                {
                    LOGE("-------fr_alloc_single-----\n");
                    m_SnapUoBuf.fr_buf.virt_addr = fr_alloc_single("ISPostSnapUoBuf",
                        m_pPortUo->GetFrameSize(), (uint32_t*)&m_SnapUoBuf.fr_buf.phys_addr);

                    LOGE("Uo Frame Size=%d phy_addr=0x%x vir_addr=%p\n",
                        m_pPortUo->GetFrameSize(),
                        m_SnapUoBuf.fr_buf.phys_addr,
                        m_SnapUoBuf.fr_buf.virt_addr);

                    if (NULL == m_SnapUoBuf.fr_buf.virt_addr)
                    {
                        LOGE("(%s:L%d) failed to alloc SnapUoBuf!\n", __func__, __LINE__);
                        pthread_mutex_unlock(&m_SnapBufLock);
                        continue;
                    }

                    m_SnapUoBuf.fr_buf.size = m_pPortUo->GetFrameSize();
                    m_SnapUoBuf.fr_buf.map_size = m_pPortUo->GetFrameSize();
                }
                pthread_mutex_unlock(&m_SnapBufLock);

                m_BufUo.fr_buf.virt_addr = m_SnapUoBuf.fr_buf.virt_addr;
                m_BufUo.fr_buf.phys_addr = m_SnapUoBuf.fr_buf.phys_addr;
                m_BufUo.fr_buf.size = m_pPortUo->GetFrameSize();
                m_BufUo.fr_buf.map_size = m_pPortUo->GetFrameSize();
                LOGE("#Uo Frame Size=%d phy_addr=0x%x vir_addr=%p pix=%d\n",
                        m_BufUo.fr_buf.size,
                        m_BufUo.fr_buf.phys_addr,
                        m_BufUo.fr_buf.virt_addr,
                        Pixel::Bits(m_pPortUo->GetPixelFormat()));
            }
            else if (m_SnapMode && m_SnapBufMode == 2)
            {
                int maxSize = 0;
                int modeID = 0;
                if (m_PortGetBuf & ISPOST_PT_UO &&
                     m_SnapUoBuf.fr_buf.phys_addr == 0)
                {
                    pthread_mutex_lock(&m_SnapBufLock);
                    IspostGetSnapMaxSize(maxSize, modeID);
                    maxSize = maxSize * Pixel::Bits(m_pPortUo->GetPixelFormat())/8;
                    LOGE("-------fr_alloc_single-----\n");
                    m_SnapUoBuf.fr_buf.virt_addr = fr_alloc_single("ISPostSnapUoBuf",
                        maxSize, (uint32_t*)&m_SnapUoBuf.fr_buf.phys_addr);

                    LOGE("Snap Uo Frame Size=%d phy_addr=0x%x vir_addr=%p pix=%d\n",
                                    maxSize, m_SnapUoBuf.fr_buf.phys_addr,
                                    m_SnapUoBuf.fr_buf.virt_addr, Pixel::Bits(m_pPortUo->GetPixelFormat()));

                    if (NULL == m_SnapUoBuf.fr_buf.virt_addr)
                    {
                        LOGE("(%s:L%d) failed to alloc SnapUoBuf!\n", __func__, __LINE__);
                        pthread_mutex_unlock(&m_SnapBufLock);
                        continue;
                    }

                    m_SnapUoBuf.fr_buf.size = maxSize;
                    m_SnapUoBuf.fr_buf.map_size =maxSize;
                    pthread_mutex_unlock(&m_SnapBufLock);
                }

                m_BufUo.fr_buf.virt_addr = m_SnapUoBuf.fr_buf.virt_addr;
                m_BufUo.fr_buf.phys_addr = m_SnapUoBuf.fr_buf.phys_addr;
                m_BufUo.fr_buf.size = m_pPortUo->GetFrameSize();
                m_BufUo.fr_buf.map_size = m_pPortUo->GetFrameSize();
                LOGE("*Uo Frame Size=%d phy_addr=0x%x vir_addr=%p pix=%d\n",
                        m_BufUo.fr_buf.size,
                        m_BufUo.fr_buf.phys_addr,
                        m_BufUo.fr_buf.virt_addr,
                        Pixel::Bits(m_pPortUo->GetPixelFormat()));

            }

            if (m_SnapMode)
            {
                if (m_pPortIn->GetWidth() != m_SnapInW ||
                    m_pPortIn->GetHeight() != m_SnapInH ||
                    m_pPortUo->GetWidth() != m_SnapOutW ||
                    m_pPortUo->GetHeight() != m_SnapOutH)
                {
                    IspostSetFceModebyResolution(
                        m_pPortIn->GetWidth(), m_pPortIn->GetHeight(),
                        m_pPortUo->GetWidth(), m_pPortUo->GetHeight());

                    m_SnapInW = m_pPortIn->GetWidth();
                    m_SnapInH = m_pPortIn->GetHeight();
                    m_SnapOutW = m_pPortUo->GetWidth();
                    m_SnapOutH = m_pPortUo->GetHeight();
                }
            }

            if (!IspostTrigger())
            {
                LOGE("Trigger fial.\n");
                PutAllBuffer();
                break;
            }
        }
        PutAllBuffer();
    } //end of while (IsInState(ST::Running))
}

void IPU_ISPOSTV2::PutAllBuffer(void)
{
    if (m_PortGetBuf & ISPOST_PT_SS1)
    {
        m_BufSs1.SyncStamp(&m_BufIn);
        m_pPortSs1->PutBuffer(&m_BufSs1);
    }
    if (m_PortGetBuf & ISPOST_PT_SS0)
    {
        m_BufSs0.SyncStamp(&m_BufIn);
        m_pPortSs0->PutBuffer(&m_BufSs0);
    }
    if (m_PortGetBuf & ISPOST_PT_UO)
    {
        m_BufUo.SyncStamp(&m_BufIn);
        m_pPortUo->PutBuffer(&m_BufUo);
    }
    if (m_PortGetBuf & ISPOST_PT_DN)
    {
        m_BufDn.SyncStamp(&m_BufIn);
        m_pPortDn->PutBuffer(&m_BufDn);
    }
    if (m_PortGetBuf & ISPOST_PT_OV0)
    {
        m_pPortOv[0]->PutBuffer(&m_BufOv[0]);
    }
    if (m_PortGetBuf & ISPOST_PT_OV1)
    {
        m_pPortOv[1]->PutBuffer(&m_BufOv[1]);
    }
    if (m_PortGetBuf & ISPOST_PT_OV2)
    {
        m_pPortOv[2]->PutBuffer(&m_BufOv[2]);
    }
    if (m_PortGetBuf & ISPOST_PT_OV3)
    {
        m_pPortOv[3]->PutBuffer(&m_BufOv[3]);
    }
    if (m_PortGetBuf & ISPOST_PT_OV4)
    {
        m_pPortOv[4]->PutBuffer(&m_BufOv[4]);
    }
    if (m_PortGetBuf & ISPOST_PT_OV5)
    {
        m_pPortOv[5]->PutBuffer(&m_BufOv[5]);
    }
    if (m_PortGetBuf & ISPOST_PT_OV6)
    {
        m_pPortOv[6]->PutBuffer(&m_BufOv[6]);
    }
    if (m_PortGetBuf & ISPOST_PT_OV7)
    {
        m_pPortOv[7]->PutBuffer(&m_BufOv[7]);
    }
    if (!m_LinkMode && (m_PortGetBuf & ISPOST_PT_IN))
    {
        m_pPortIn->PutBuffer(&m_BufIn);
    }
}

IPU_ISPOSTV2::IPU_ISPOSTV2(std::string name, Json* js)
{
    int i, Value;
    char* pszString;
    char szItemName[80];
    int hasGridFile = 0;
    soc_info_t s_info;

    m_pJsonMain = js;
    Name = name;//"ispostv2";
    for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
    {
        m_pszLcGridFileName[i] = NULL;
        m_pszLcGeometryFileName[i] = NULL;
        m_pLcGridBuf[i] = NULL;
        m_pLcGeometryBuf[i] = NULL;
    }
    for (i = 0; i < OVERLAY_CHNL_MAX; i++)
    {
        m_pszOvImgFileName[i] = NULL;
        m_pOvImgBuf[i] = NULL;
    }
    m_Buffers = 3;
    m_Rotate = 0;
    grid_target_index_max = 1;
    m_IsFisheye = 0;
    m_IsBlendScale = 0;
    m_IsLineBufEn =0;
    m_IsDnManual = 0;
    m_DnManualVal = 0;
    m_IsLcManual = 0;
    m_LcManualVal = 0;
    m_IsOvManual = 0;
    m_OvManualVal = 0;
    memset(&m_Dns3dAttr, 0, sizeof(m_Dns3dAttr));
    m_Dns3dAttr.y_threshold = g_dnParamList[0].y_threshold;
    m_Dns3dAttr.u_threshold = g_dnParamList[0].u_threshold;
    m_Dns3dAttr.v_threshold = g_dnParamList[0].v_threshold;
    m_Dns3dAttr.weight = g_dnParamList[0].weight;
    m_eDnsModePrev = CAM_CMN_AUTO;
    m_ui32DnRefImgBufYaddr = 0xFFFFFFFF;

    pthread_mutex_init( &m_PipMutex, NULL);
    IspostDefaultParameter();
    pthread_mutex_init(&m_FceLock, NULL);
    m_LinkMode = 0;
    m_LinkBuffers = 0;
    pthread_mutex_init(&m_BufLock, NULL);
    m_RefBufSerial = 0;
    memset(&m_stFceJsonParam, 0, sizeof(m_stFceJsonParam));

    m_s32LcOutImgWidth = m_s32LcOutImgHeight = 0;
    m_SnapMode = 0;
    m_SnapBufMode = 0;
    m_SnapInW = m_SnapInH = m_SnapOutW = m_SnapOutH = 0;
    pthread_mutex_init(&m_SnapBufLock, NULL);

#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
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
            m_Rotate = Value;
        if (IspostJsonGetInt("fisheye", Value)) {
            m_IsFisheye = (1 == Value) ? (1) : (0);
            if (m_IsFisheye) {
                m_IspostUser.stLcInfo.stPxlCacheMode.ui1BurstLenSet = (BURST_LEN_4QW_FISHEYE);
                m_IspostUser.stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = (CCH_DAT_SHAPE_3);
            } else {
                m_IspostUser.stLcInfo.stPxlCacheMode.ui1BurstLenSet = (BURST_LEN_8QW_BARREL);
                m_IspostUser.stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = (CCH_DAT_SHAPE_1);
            }
        }
        if (IspostJsonGetInt("blendscale", Value)) {
            m_IsBlendScale = Value;
        }
        //----------------------------------------------------------------------------------------------
        // Parse lc_config parameter
        for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
        {
            sprintf(szItemName, "lc_grid_file_name%d", i + 1);
            if (IspostJsonGetString(szItemName, pszString))
            {
                m_pszLcGridFileName[i] = strdup(pszString);
#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
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
                hasGridFile = 1;
            }
            sprintf(szItemName, "lc_geometry_file_name%d", i + 1);
            if (IspostJsonGetString(szItemName, pszString))
                m_pszLcGeometryFileName[i] = strdup(pszString);
        }
        if (IspostJsonGetInt("lc_enable", Value))
        {
            m_IsLcManual = 1;
            m_LcManualVal = Value;
            if (m_LcManualVal && !hasGridFile)
                LOGE("Error: No grid files \n");
        }
        if(system_get_soc_type(&s_info) == 0)
        {
            if(s_info.chipid == 0x3420010 || s_info.chipid == 0x3420310)//disable lc function for Q3420P and C20.
            {
                m_IsLcManual = 1;
                m_LcManualVal = 0;
                LOGE("force disable lc function.\n");
            }
        }
        if (IspostJsonGetInt("lc_grid_target_index", Value))
            m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx = Value;

        if (IspostJsonGetInt("lc_grid_line_buf_enable", Value))
        {
            m_IsLineBufEn = (1 == Value) ? (1) : (0);
            m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable = m_IsLineBufEn;
        }
        if (IspostJsonGetInt("lc_cbcr_swap", Value))
            m_IspostUser.stLcInfo.stPxlCacheMode.ui1CBCRSwap = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_w", Value))
            m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_h", Value))
            m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH = Value;
        if (IspostJsonGetInt("lc_scan_mode_scan_m", Value))
            m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanM = Value;
        if (IspostJsonGetInt("lc_fill_y", Value))
            m_IspostUser.stLcInfo.stBackFillColor.ui8Y = Value;
        if (IspostJsonGetInt("lc_fill_cb", Value))
            m_IspostUser.stLcInfo.stBackFillColor.ui8CB = Value;
        if (IspostJsonGetInt("lc_fill_cr", Value))
            m_IspostUser.stLcInfo.stBackFillColor.ui8CR = Value;
        if (IspostJsonGetInt("lc_out_width", Value))
            m_s32LcOutImgWidth = Value;
        if (IspostJsonGetInt("lc_out_height", Value))
            m_s32LcOutImgHeight = Value;

        //----------------------------------------------------------------------------------------------
        // Parse ov_config parameter
        if (IspostJsonGetInt("ov_enable", Value))
        {
            m_IsOvManual = 1;
            m_OvManualVal = Value;//ov enable bitmap [7][6][5][4][3][2][1][0]
        }
        if (IspostJsonGetInt("ov_mode", Value))
            m_IspostUser.stOvInfo.stOverlayMode.ui32OvMode = Value;
        if (IspostJsonGetInt("ov_img_x", Value))
            m_IspostUser.stOvInfo.stOvOffset[0].ui16X = Value;
        if (IspostJsonGetInt("ov_img_y", Value))
            m_IspostUser.stOvInfo.stOvOffset[0].ui16Y = Value;
        if (IspostJsonGetInt("ov_img_w", Value))
            m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width = Value;
        if (IspostJsonGetInt("ov_img_h", Value))
            m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height = Value;
        if (IspostJsonGetInt("ov_img_s", Value))
            m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride = Value;
        for (i = 1; i < OVERLAY_CHNL_MAX; i++)
        {
            sprintf(szItemName, "ov%d_img_x", i );
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvOffset[i].ui16X = Value;

            sprintf(szItemName, "ov%d_img_y", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = Value;

            sprintf(szItemName, "ov%d_img_w", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Width = Value;

            sprintf(szItemName, "ov%d_img_h", i);
            if (IspostJsonGetInt(szItemName, Value))
            m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Height = Value;

            sprintf(szItemName, "ov%d_img_s", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = Value;

            sprintf(szItemName, "ov%d_alpha0", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue0.ui32Alpha = Value;

            sprintf(szItemName, "ov%d_alpha1", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue1.ui32Alpha = Value;

            sprintf(szItemName, "ov%d_alpha2", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue2.ui32Alpha = Value;

            sprintf(szItemName, "ov%d_alpha3", i);
            if (IspostJsonGetInt(szItemName, Value))
                m_IspostUser.stOvInfo.stOvAlpha[i].stValue3.ui32Alpha = Value;

            sprintf(szItemName, "ov_img_file_name%d", i);
            if (IspostJsonGetString(szItemName, pszString))
                m_pszOvImgFileName[i] = strdup(pszString);
        }

        //----------------------------------------------------------------------------------------------
        // Parse dn_config parameter
        if (IspostJsonGetInt("dn_enable", Value))
        {
            m_IsDnManual = 1;
            m_DnManualVal = Value;
        }
        if (IspostJsonGetInt("dn_target_index", Value))
            m_IspostUser.stDnInfo.ui32DnTargetIdx = Value;
        if (IspostJsonGetInt("linkmode", Value))
            m_LinkMode = Value;
        if (IspostJsonGetInt("linkbuffers", Value))
            m_LinkBuffers = Value;

        sprintf(szItemName, "fcefile");
        if (IspostJsonGetString(szItemName, pszString))
        {
            ParseFceFileJson(pszString, &m_stFceJsonParam);
        }

        if (IspostJsonGetInt("snapmode", Value))
           m_SnapMode = Value;

        if (IspostJsonGetInt("snapbufmode", Value))
            m_SnapBufMode = Value;

    }

    if (m_Buffers <= 0 || m_Buffers > 20)
    {
        LOGE("the buffers number is over.\n");
        throw VBERR_BADPARAM;
    }

    //----------------------------------------------------------------------------------------------
    // Create ISPOST support port
    m_pPortIn = CreatePort("in", Port::In);
    m_pPortOv[0] = CreatePort("ov0", Port::In);
    m_pPortOv[1] = CreatePort("ov1", Port::In);
    m_pPortOv[2] = CreatePort("ov2", Port::In);
    m_pPortOv[3] = CreatePort("ov3", Port::In);
    m_pPortOv[4] = CreatePort("ov4", Port::In);
    m_pPortOv[5] = CreatePort("ov5", Port::In);
    m_pPortOv[6] = CreatePort("ov6", Port::In);
    m_pPortOv[7] = CreatePort("ov7", Port::In);
    m_pPortUo = CreatePort("uo", Port::Out);
    m_pPortDn = CreatePort("dn", Port::Out);
    m_pPortSs0 = CreatePort("ss0", Port::Out);
    m_pPortSs1 = CreatePort("ss1", Port::Out);
    for (i = 0; i < 8; i++)
    {
        m_pPortOv[i]->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
        if(i == 0)
        {
            m_pPortOv[i]->SetPixelFormat(Pixel::Format::NV12);
        }
        else
        {
            m_pPortOv[i]->SetPixelFormat(Pixel::Format::BPP2);
        }
    }

    LOGD("Crop Info: %d, %d, %d, %d\n", m_pPortIn->GetPipX(), m_pPortIn->GetPipY(), m_pPortIn->GetPipWidth(), m_pPortIn->GetPipHeight());

    //m_pPortDn->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortDn->SetBufferType(FRBuffer::Type::FIXED, 1);
    m_pPortDn->SetResolution(640, 480);
    m_pPortDn->SetPixelFormat(Pixel::Format::NV12);
    m_pPortDn->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);

    if (m_DnManualVal)
    {
        // export PortDn if dn function is enabled, which will
        // make system allocate buffer for PortDn
        m_pPortDn->Export();
    }

    if (m_LinkMode)
        m_pPortUo->SetBufferType(FRBuffer::Type::VACANT, m_LinkBuffers);
    else if (m_SnapMode && m_SnapBufMode)
        m_pPortUo->SetBufferType(FRBuffer::Type::VACANT, 1);
    else
        m_pPortUo->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);

    m_pPortUo->SetResolution(1920, 1088);
    m_pPortUo->SetPixelFormat(Pixel::Format::NV12);
    m_pPortUo->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    m_pPortSs0->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortSs0->SetResolution(1280, 720);
    m_pPortSs0->SetPixelFormat(Pixel::Format::NV12);
    m_pPortSs0->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    m_pPortSs1->SetBufferType(FRBuffer::Type::FIXED, m_Buffers);
    m_pPortSs1->SetResolution(640, 480);
    m_pPortSs1->SetPixelFormat(Pixel::Format::NV12);
    m_pPortSs1->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    ispost_open();

    m_pJsonMain = NULL;

    LOGE("IPU_ISPOSTV2() end\n");
}

IPU_ISPOSTV2::~IPU_ISPOSTV2()
{
    int i;


    ispost_close();
    for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
    {
        if (m_pszLcGridFileName[i] != NULL)
            free(m_pszLcGridFileName[i]);
        if (m_pszLcGeometryFileName[i] != NULL)
            free(m_pszLcGeometryFileName[i]);
        if (m_pLcGridBuf[i] != NULL)
            delete m_pLcGridBuf[i];
        if (m_pLcGeometryBuf[i] != NULL)
            delete m_pLcGeometryBuf[i];
    }
    for (i = 0; i < OVERLAY_CHNL_MAX; i++)
    {
        if (m_pszOvImgFileName[i] != NULL)
            free(m_pszOvImgFileName[i]);
        if (m_pOvImgBuf[i] != NULL)
            delete m_pOvImgBuf[i];
    }
    pthread_mutex_destroy( &m_PipMutex);
    pthread_mutex_destroy(&m_FceLock);
    pthread_mutex_destroy(&m_BufLock);
    pthread_mutex_destroy(&m_SnapBufLock);
}

int IPU_ISPOSTV2::IspostFormatPipInfo(struct v_pip_info *vpinfo)
{
    int idx = 0;
    int dn_w, dn_h;
    int ov_w, ov_h;
    dn_w = m_pPortDn->GetWidth() - 10;
    dn_h = m_pPortDn->GetHeight() - 10;

    if(strncmp(vpinfo->portname, "ov0", 3) == 0)
        idx = 0;
    else if(strncmp(vpinfo->portname, "ov1", 3) == 0)
        idx = 1;
    else if(strncmp(vpinfo->portname, "ov2", 3) == 0)
        idx = 2;
    else if(strncmp(vpinfo->portname, "ov3", 3) == 0)
        idx = 3;
    else if(strncmp(vpinfo->portname, "ov4", 3) == 0)
        idx = 4;
    else if(strncmp(vpinfo->portname, "ov5", 3) == 0)
        idx = 5;
    else if(strncmp(vpinfo->portname, "ov6", 3) == 0)
        idx = 6;
    else if(strncmp(vpinfo->portname, "ov7", 3) == 0)
        idx = 7;
    else
        return 0;

    ov_w = m_pPortOv[idx]->GetWidth();
    ov_h = m_pPortOv[idx]->GetHeight();

    if (ov_w + vpinfo->x >= dn_w)
        vpinfo->x = dn_w - ov_w ;

    if (ov_h + vpinfo->y >= dn_h)
        vpinfo->y = dn_h - ov_h;

    if (vpinfo->x < 0)
        vpinfo->x = 0;

    if (vpinfo->y < 0)
        vpinfo->y = 0;

    if (idx == 0) {
        vpinfo->x -= vpinfo->x%4;
        vpinfo->y -= vpinfo->y%4;
    }

    LOGD("FormatPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

return 0;
}

void IPU_ISPOSTV2::IspostAdjustOvPos(int ov_idx)
{
    int dn_w, dn_h;
    int ov_w, ov_h;
    int ov_x, ov_y;

    dn_w = m_pPortDn->GetWidth() - 10;
    dn_h = m_pPortDn->GetHeight() - 10;

    ov_w = m_pPortOv[ov_idx]->GetWidth();
    ov_h = m_pPortOv[ov_idx]->GetHeight();

    ov_x = m_pPortOv[ov_idx]->GetPipX();
    ov_y = m_pPortOv[ov_idx]->GetPipY();

    if (ov_w + ov_x >= dn_w)
        ov_x = dn_w - ov_w ;

    if (ov_h + ov_y >= dn_h)
        ov_y = dn_h - ov_h;

    if (ov_x < 0)
        ov_x = 0;

    if (ov_y < 0)
        ov_y = 0;

    if (ov_idx == 0) {
        ov_x -= ov_x%4;
        ov_y -= ov_y%4;
    }

    m_pPortOv[ov_idx]->SetPipInfo(ov_x, ov_y, 0, 0);
}

int IPU_ISPOSTV2::IspostSetPipInfo(struct v_pip_info *vpinfo)
{
    IspostFormatPipInfo(vpinfo);
    pthread_mutex_lock( &m_PipMutex);
    {
        if(strncmp(vpinfo->portname, "dn", 2) == 0)
            m_pPortDn->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "uo", 2) == 0)
            m_pPortUo->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ss0", 3) == 0)
            m_pPortSs0->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ss1", 3) == 0)
            m_pPortSs1->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "in", 2) == 0)
            m_pPortIn->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov0", 3) == 0)
            m_pPortOv[0]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov1", 3) == 0)
            m_pPortOv[1]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov2", 3) == 0)
            m_pPortOv[2]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov3", 3) == 0)
            m_pPortOv[3]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov4", 3) == 0)
            m_pPortOv[4]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov5", 3) == 0)
            m_pPortOv[5]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov6", 3) == 0)
            m_pPortOv[6]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        else if(strncmp(vpinfo->portname, "ov7", 3) == 0)
            m_pPortOv[7]->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
    }
    pthread_mutex_unlock( &m_PipMutex);

    LOGD("SetPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

    return 0;
}

int IPU_ISPOSTV2::IspostGetPipInfo(struct v_pip_info *vpinfo)
{
    if(strncmp(vpinfo->portname, "dn", 2) == 0) {
        vpinfo->x = m_pPortDn->GetPipX();
        vpinfo->y = m_pPortDn->GetPipY();
        vpinfo->w = m_pPortDn->GetPipWidth();
        vpinfo->h = m_pPortDn->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "uo", 2) == 0) {
        vpinfo->x = m_pPortUo->GetPipX();
        vpinfo->y = m_pPortUo->GetPipY();
        vpinfo->w = m_pPortUo->GetPipWidth();
        vpinfo->h = m_pPortUo->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "ss0", 3) == 0) {
        vpinfo->x = m_pPortSs0->GetPipX();
        vpinfo->y = m_pPortSs0->GetPipY();
        vpinfo->w = m_pPortSs0->GetPipWidth();
        vpinfo->h = m_pPortSs0->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "ss1", 3) == 0) {
        vpinfo->x = m_pPortSs1->GetPipX();
        vpinfo->y = m_pPortSs1->GetPipY();
        vpinfo->w = m_pPortSs1->GetPipWidth();
        vpinfo->h = m_pPortSs1->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "in", 2) == 0) {
        vpinfo->x = m_pPortIn->GetPipX();
        vpinfo->y = m_pPortIn->GetPipY();
        vpinfo->w = m_pPortIn->GetPipWidth();
        vpinfo->h = m_pPortIn->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "ov0", 3) == 0) {
        vpinfo->x = m_pPortOv[0]->GetPipX();
        vpinfo->y = m_pPortOv[0]->GetPipY();
        vpinfo->w = m_pPortOv[0]->GetPipWidth();
        vpinfo->h = m_pPortOv[0]->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "ov1", 3) == 0) {
        vpinfo->x = m_pPortOv[1]->GetPipX();
        vpinfo->y = m_pPortOv[1]->GetPipY();
        vpinfo->w = m_pPortOv[1]->GetPipWidth();
        vpinfo->h = m_pPortOv[1]->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "ov2", 3) == 0) {
        vpinfo->x = m_pPortOv[2]->GetPipX();
        vpinfo->y = m_pPortOv[2]->GetPipY();
        vpinfo->w = m_pPortOv[2]->GetPipWidth();
        vpinfo->h = m_pPortOv[2]->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "ov3", 3) == 0) {
        vpinfo->x = m_pPortOv[3]->GetPipX();
        vpinfo->y = m_pPortOv[3]->GetPipY();
        vpinfo->w = m_pPortOv[3]->GetPipWidth();
        vpinfo->h = m_pPortOv[3]->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "ov4", 3) == 0) {
        vpinfo->x = m_pPortOv[4]->GetPipX();
        vpinfo->y = m_pPortOv[4]->GetPipY();
        vpinfo->w = m_pPortOv[4]->GetPipWidth();
        vpinfo->h = m_pPortOv[4]->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "ov5", 3) == 0) {
        vpinfo->x = m_pPortOv[5]->GetPipX();
        vpinfo->y = m_pPortOv[5]->GetPipY();
        vpinfo->w = m_pPortOv[5]->GetPipWidth();
        vpinfo->h = m_pPortOv[5]->GetPipHeight();
    } else if(strncmp(vpinfo->portname, "ov6", 3) == 0) {
        vpinfo->x = m_pPortOv[6]->GetPipX();
        vpinfo->y = m_pPortOv[6]->GetPipY();
        vpinfo->w = m_pPortOv[6]->GetPipWidth();
        vpinfo->h = m_pPortOv[6]->GetPipHeight();
    }else if(strncmp(vpinfo->portname, "ov7", 3) == 0) {
        vpinfo->x = m_pPortOv[7]->GetPipX();
        vpinfo->y = m_pPortOv[7]->GetPipY();
        vpinfo->w = m_pPortOv[7]->GetPipWidth();
        vpinfo->h = m_pPortOv[7]->GetPipHeight();
    }

    LOGD("GetPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname ,
             vpinfo->x,
             vpinfo->y,
             vpinfo->w,
             vpinfo->h);

    return 0;
}

#if defined(INFOTM_HW_BLENDING)

#ifdef INFOTM_BLEND_8STEP_ALPHA
void IPU_ISPOSTV2::IspostAdjustOvAlphaValue(ISPOST_UINT8 ui8Side, void* pVirtAddr, ISPOST_UINT16 ui16Stride, ISPOST_UINT16 ui16Height)
{
    long long * ptr_y;
    long long * ptr_uv;

    if (pVirtAddr == NULL) {
        LOGE("ERROR : Virture address not correct. \n");
        return;
    }

    ptr_y = (long long*)pVirtAddr;
    ptr_uv = ptr_y + (ui16Stride * ui16Height / 8);

    if (BLENDING_SIDE_LEFT == ui8Side) {
        for (int i = 0; i < ui16Height / 2; i++) {
            // y line 1
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 33221100, Pixel 07~00,
            ptr_y++;
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 77665544, Pixel 15~08,
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha BBAA9988, Pixel 23~16,
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha FFEEDDCC, Pixel 31~24,
            ptr_y += (((ui16Stride - 32) / 8) + 1);

            // y line 2
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 33221100, Pixel 07~00,
            ptr_y++;
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 77665544, Pixel 15~08,
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha BBAA9988, Pixel 23~16,
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha FFEEDDCC, Pixel 31~24,
            ptr_y += (((ui16Stride - 32) / 8) + 1);

            // uv line
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha 11110000, Pixel 07~00,
            *(ptr_uv) |= 0x0001000100000000;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha 33332222, Pixel 15~08,
            *(ptr_uv) |= 0x0101010101000100;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha 55554444, Pixel 23~16,
            *(ptr_uv) |= 0x0001000100000000;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha 77776666, Pixel 31~24,
            *(ptr_uv) |= 0x0101010101000100;

            ptr_uv += (((ui16Stride - 32) / 8) + 1);
        }
    } else {
        for (int i = 0; i < ui16Height / 2; i++) {
            // y line 1
            ptr_y++;
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha CCDDEEFF, Pixel 16~23,
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha 8899AABB, Pixel 24~31,
            ptr_y++;
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 44556677, Pixel 32~39,
            ptr_y++;
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 00112233, Pixel 40~47,
            ptr_y += (((ui16Stride - 32 - 16) / 8) + 1);

            // y line 2
            ptr_y++;
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha CCDDEEFF, Pixel 16~23,
            ptr_y++;
            *(ptr_y) |= 0x0101010101010101; // set [1:0] = 1, Alpha 8899AABB, Pixel 24~31,
            ptr_y++;
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 44556677, Pixel 32~39,
            ptr_y++;
            *(ptr_y) &= 0xFEFEFEFEFEFEFEFE; // set [1:0] = 0, Alpha 00112233, Pixel 40~47,
            ptr_y += (((ui16Stride - 32 - 16) / 8) + 1);

            // uv line
            ptr_uv++;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha 66667777,
            *(ptr_uv) |= 0x0100010001010101;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha 44445555
            *(ptr_uv) |= 0x0000000000010001;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha 22223333
            *(ptr_uv) |= 0x0100010001010101;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha 00001111
            *(ptr_uv) |= 0x0000000000010001;
            ptr_uv += (((ui16Stride - 32 - 16) / 8) + 1);
        }
    }
}
#else
void IPU_ISPOSTV2::IspostAdjustOvAlphaValue(ISPOST_UINT8 ui8Side, void* pVirtAddr, ISPOST_UINT16 ui16Stride, ISPOST_UINT16 ui16Height)
{
    long long * ptr_y;
    long long * ptr_uv;

    if (pVirtAddr == NULL) {
        LOGE("ERROR : Virture address not correct. \n");
        return;
    }

    ptr_y = (long long*)pVirtAddr;
    ptr_uv = ptr_y + (ui16Stride * ui16Height / 8);

    if (BLENDING_SIDE_LEFT == ui8Side) {
        for (int i = 0; i < ui16Height / 2; i++) {
            // y line 1
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 0, Alpha 33221100, Pixel 07~00,
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 1, Alpha 77665544, Pixel 15~08,
            *(ptr_y) |= 0x0101010101010101;
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 2, Alpha BBAA9988, Pixel 23~16,
            *(ptr_y) |= 0x0202020202020202;
            ptr_y++;
            *(ptr_y) |= 0x0303030303030303; // set [1:0] = 3, Alpha FFEEDDCC, Pixel 31~24,
            ptr_y += (((ui16Stride - 32) / 8) + 1);

            // y line 2
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 0, Alpha 33221100, Pixel 07~00,
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 1, Alpha 77665544, Pixel 15~08,
            *(ptr_y) |= 0x0101010101010101;
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 2, Alpha BBAA9988, Pixel 23~16,
            *(ptr_y) |= 0x0202020202020202;
            ptr_y++;
            *(ptr_y) |= 0x0303030303030303; // set [1:0] = 3, Alpha FFEEDDCC, Pixel 31~24,
            ptr_y += (((ui16Stride - 32) / 8) + 1);

            // uv line
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha 33221100, Pixel 07~00,
            *(ptr_uv) |= 0x0101010000010000;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha 77665544, Pixel 15~08,
            *(ptr_uv) |= 0x0101010000010000;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha BBAA9988, Pixel 23~16,
            *(ptr_uv) |= 0x0101010000010000;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 3~0, Alpha 77665544, Pixel 31~24,
            *(ptr_uv) |= 0x0101010000010000;
            ptr_uv += (((ui16Stride - 32) / 8) + 1);
        }
    } else {
        for (int i = 0; i < ui16Height / 2; i++) {
            // y line 1
            ptr_y++;
            ptr_y++;
            *(ptr_y) |= 0x0303030303030303; // set [1:0] = 3, Alpha CCDDEEFF, Pixel 16~23,
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 2, Alpha 8899AABB, Pixel 24~31,
            *(ptr_y) |= 0x0202020202020202; //
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 1, Alpha 44556677, Pixel 32~39,
            *(ptr_y) |= 0x0101010101010101;
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 0, Alpha 00112233, Pixel 40~47,
            ptr_y += (((ui16Stride - 32 - 16) / 8) + 1);

            // y line 2
            ptr_y++;
            ptr_y++;
            *(ptr_y) |= 0x0303030303030303; // set [1:0] = 3, Alpha CCDDEEFF, Pixel 16~23,
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 2, Alpha 8899AABB, Pixel 24~31,
            *(ptr_y) |= 0x0202020202020202; //
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 1, Alpha 44556677, Pixel 32~39,
            *(ptr_y) |= 0x0101010101010101;
            ptr_y++;
            *(ptr_y) &= 0xFCFCFCFCFCFCFCFC; // set [1:0] = 0, Alpha 00112233, Pixel 40~47,
            ptr_y += (((ui16Stride - 32 - 16) / 8) + 1);

            // uv line
            ptr_uv++;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha CCDDEEFF,
            *(ptr_uv) |= 0x0000000101000101;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha 8899AABB
            *(ptr_uv) |= 0x0000000101000101;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha 44556677
            *(ptr_uv) |= 0x0000000101000101;
            ptr_uv++;
            *(ptr_uv) &= 0xFEFEFEFEFEFEFEFE; // set V[0]U[0] = 0~3, Alpha 00112233
            *(ptr_uv) |= 0x0000000101000101;
            ptr_uv += (((ui16Stride - 32 - 16) / 8) + 1);
        }
    }
}
#endif

void IPU_ISPOSTV2::IspostExtBufForHWBlending(ISPOST_UINT8 ui8location, ST_EXT_BUF_INFO *stExtBufInfo)
{
    if (m_PortGetBuf & ISPOST_PT_UO) {
        IspostAdjustOvAlphaValue(
            ui8location,
            (void *)stExtBufInfo->stExtBuf.virt_addr,
            stExtBufInfo->stride,
            stExtBufInfo->height
            );

        m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
        m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = ISPOST_ENABLE;
        m_IspostUser.stOvInfo.stOvOffset[0].ui16X = 0;
        m_IspostUser.stOvInfo.stOvOffset[0].ui16Y = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride = stExtBufInfo->stride;
        m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width = stExtBufInfo->width;
        m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height = stExtBufInfo->height;
        m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr = stExtBufInfo->stExtBuf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[0].ui32UVAddr = stExtBufInfo->stExtBuf.phys_addr +
        stExtBufInfo->stride * stExtBufInfo->height;
    }
}
#endif //#if defined(INFOTM_HW_BLENDING)

#if defined(INFOTM_SW_BLENDING)
void IPU_ISPOSTV2::IspostSwOverlay(ISPOST_UINT8 ui8Side, BUF_INFO stImgBufInfo, BUF_INFO stOvBufInfo)
{
    long long * pImgY = NULL;
    long long * pImgUV = NULL;
    long long * pOvImgY = NULL;
    long long * pOvImgUV = NULL;
    ISPOST_ULONGLONG stImgValue;
    ISPOST_ULONGLONG stOvImgValue;
    ISPOST_UINT16 ui16OffsetX;
    ISPOST_UINT16 ui16OffsetY;
    int iImgValue;
    int iOvValue;
    int iH;
    int i;

    ui16OffsetX = stImgBufInfo.ui16OffsetX;
    ui16OffsetY = stImgBufInfo.ui16OffsetY;
    pImgY = (long long *)(stImgBufInfo.ui32YAddr + ((ui16OffsetY * stImgBufInfo.ui16Stride) + ui16OffsetX));
    pImgUV = (long long *)(stImgBufInfo.ui32UVAddr + (((ui16OffsetY / 2) * stImgBufInfo.ui16Stride) + ui16OffsetX));

    ui16OffsetX = stOvBufInfo.ui16OffsetX;
    ui16OffsetY = stOvBufInfo.ui16OffsetY;
    pOvImgY = (long long *)(stOvBufInfo.ui32YAddr + ((ui16OffsetY * stOvBufInfo.ui16Stride) + ui16OffsetX));
    pOvImgUV = (long long *)(stOvBufInfo.ui32UVAddr + (((ui16OffsetY / 2) * stOvBufInfo.ui16Stride) + ui16OffsetX));

    if (BLENDING_SIDE_LEFT == ui8Side) {
        for (iH = 0; iH < (INFOTM_OV_HEIGHT >> 1); iH++) {
            //=================================================================
            // Process Y plane.
            //=================================================================
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - (i * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#if INFOTM_OV_WIDTH > 8
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 16
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - ((i + 16) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 24
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - ((i + 24) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif

            pImgY += ((stImgBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
            pOvImgY += ((stOvBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;

            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - (i * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#if INFOTM_OV_WIDTH > 8
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 16
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - ((i + 16) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 24
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_Y_ALPHA_PWR) - ((i + 24) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif

            pImgY += ((stImgBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
            pOvImgY += ((stOvBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;

            //=================================================================
            // Process UV plane.
            //=================================================================
            stImgValue.ullValue = *pImgUV;
            stOvImgValue.ullValue = *pOvImgUV;

            for (i = 0; i < 4; i++) {
                iImgValue = stImgValue.byValue[2*i];
                iOvValue = stOvImgValue.byValue[2*i];
                stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - (i * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
                iImgValue = stImgValue.byValue[2*i + 1];
                iOvValue = stOvImgValue.byValue[2*i + 1];
                stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - (i * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            }
            *pImgUV = stImgValue.ullValue;
#if INFOTM_OV_WIDTH > 8
            pImgUV++;
            pOvImgUV++;

            stImgValue.ullValue = *pImgUV;
            stOvImgValue.ullValue = *pOvImgUV;

            for (i = 0; i < 4; i++) {
                iImgValue = stImgValue.byValue[2*i];
                iOvValue = stOvImgValue.byValue[2*i];
                stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - ((i + 4) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
                iImgValue = stImgValue.byValue[2*i + 1];
                iOvValue = stOvImgValue.byValue[2*i + 1];
                stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - ((i + 4) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            }
            *pImgUV = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 16
            pImgUV++;
            pOvImgUV++;

            stImgValue.ullValue = *pImgUV;
            stOvImgValue.ullValue = *pOvImgUV;

            for (i = 0; i < 4; i++) {
                iImgValue = stImgValue.byValue[2*i];
                iOvValue = stOvImgValue.byValue[2*i];
                stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
                iImgValue = stImgValue.byValue[2*i + 1];
                iOvValue = stOvImgValue.byValue[2*i + 1];
                stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            }
            *pImgUV = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 24
            pImgUV++;
            pOvImgUV++;

            stImgValue.ullValue = *pImgUV;
            stOvImgValue.ullValue = *pOvImgUV;

            for (i = 0; i < 4; i++) {
                iImgValue = stImgValue.byValue[2*i];
                iOvValue = stOvImgValue.byValue[2*i];
                stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - ((i + 12) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
                iImgValue = stImgValue.byValue[2*i + 1];
                iOvValue = stOvImgValue.byValue[2*i + 1];
                stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iImgValue << INFOTM_OV_UV_ALPHA_PWR) - ((i + 12) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            }
            *pImgUV = stImgValue.ullValue;
#endif
            pImgUV += ((stImgBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
            pOvImgUV += ((stOvBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
        }
    }else {
        for (iH = 0; iH < (INFOTM_OV_HEIGHT >> 1); iH++) {
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + (i * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#if INFOTM_OV_WIDTH > 8
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 16
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + ((i + 16) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 24
            pImgY++;
            pOvImgY++;
            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + ((i + 24) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#endif
            pImgY += ((stImgBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
            pOvImgY += ((stOvBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;

            stImgValue.ullValue = *pImgY;
            stOvImgValue.ullValue = *pOvImgY;
            for (i = 0; i < 8; i++) {
                iImgValue = stImgValue.byValue[i];
                iOvValue = stOvImgValue.byValue[i];
                stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + (i * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
            }
            *pImgY = stImgValue.ullValue;
#if INFOTM_OV_WIDTH > 8
           pImgY++;
           pOvImgY++;
           stImgValue.ullValue = *pImgY;
           stOvImgValue.ullValue = *pOvImgY;
           for (i = 0; i < 8; i++) {
               iImgValue = stImgValue.byValue[i];
               iOvValue = stOvImgValue.byValue[i];
               stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
           }
           *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 16
           pImgY++;
           pOvImgY++;
           stImgValue.ullValue = *pImgY;
           stOvImgValue.ullValue = *pOvImgY;
           for (i = 0; i < 8; i++) {
               iImgValue = stImgValue.byValue[i];
               iOvValue = stOvImgValue.byValue[i];
               stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + ((i + 16) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
           }
           *pImgY = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 24
           pImgY++;
           pOvImgY++;
           stImgValue.ullValue = *pImgY;
           stOvImgValue.ullValue = *pOvImgY;
           for (i = 0; i < 8; i++) {
               iImgValue = stImgValue.byValue[i];
               iOvValue = stOvImgValue.byValue[i];
               stImgValue.byValue[i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_Y_ALPHA_PWR) + ((i + 24) * (iImgValue - iOvValue))) >> INFOTM_OV_Y_ALPHA_PWR);
           }
           *pImgY = stImgValue.ullValue;
#endif
            pImgY += ((stImgBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
            pOvImgY += ((stOvBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;

            //=================================================================
            // Process UV plane.
            //=================================================================
            stImgValue.ullValue = *pImgUV;
            stOvImgValue.ullValue = *pOvImgUV;

            for (i = 0; i < 4; i++) {
                iImgValue = stImgValue.byValue[2*i];
                iOvValue = stOvImgValue.byValue[2*i];
                stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + (i * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
                iImgValue = stImgValue.byValue[2*i + 1];
                iOvValue = stOvImgValue.byValue[2*i + 1];
                stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + (i * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            }
            *pImgUV = stImgValue.ullValue;
#if INFOTM_OV_WIDTH > 8
            pImgUV++;
            pOvImgUV++;

            stImgValue.ullValue = *pImgUV;
            stOvImgValue.ullValue = *pOvImgUV;

            for (i = 0; i < 4; i++) {
            iImgValue = stImgValue.byValue[2*i];
            iOvValue = stOvImgValue.byValue[2*i];
            stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + ((i + 4) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            iImgValue = stImgValue.byValue[2*i + 1];
            iOvValue = stOvImgValue.byValue[2*i + 1];
            stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + ((i + 4) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            }

            *pImgUV = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 16
            pImgUV++;
            pOvImgUV++;

            stImgValue.ullValue = *pImgUV;
            stOvImgValue.ullValue = *pOvImgUV;

            for (i = 0; i < 4; i++) {
                iImgValue = stImgValue.byValue[2*i];
                iOvValue = stOvImgValue.byValue[2*i];
                stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
                iImgValue = stImgValue.byValue[2*i + 1];
                iOvValue = stOvImgValue.byValue[2*i + 1];
                stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + ((i + 8) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
            }

            *pImgUV = stImgValue.ullValue;
#endif
#if INFOTM_OV_WIDTH > 24
             pImgUV++;
             pOvImgUV++;

             stImgValue.ullValue = *pImgUV;
             stOvImgValue.ullValue = *pOvImgUV;

             for (i = 0; i < 4; i++) {
                 iImgValue = stImgValue.byValue[2*i];
                 iOvValue = stOvImgValue.byValue[2*i];
                 stImgValue.byValue[2*i] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + ((i + 12) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
                 iImgValue = stImgValue.byValue[2*i + 1];
                 iOvValue = stOvImgValue.byValue[2*i + 1];
                 stImgValue.byValue[2*i + 1] = (ISPOST_UINT8)(((iOvValue << INFOTM_OV_UV_ALPHA_PWR) + ((i + 12) * (iImgValue - iOvValue))) >> INFOTM_OV_UV_ALPHA_PWR);
             }

             *pImgUV = stImgValue.ullValue;
#endif
        pImgUV += ((stImgBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
        pOvImgUV += ((stOvBufInfo.ui16Stride - INFOTM_OV_WIDTH) / 8) + 1;
        }
    }
}

void IPU_ISPOSTV2::IspostExtBufForSWBlending(ISPOST_UINT8 ui8location, ST_EXT_BUF_INFO *stExtBufInfo)
{
    BUF_INFO stImgBuf;
    BUF_INFO stOvImgBuf;

    stImgBuf.ui32YAddr = (ISPOST_UINT32)m_BufUo.fr_buf.virt_addr;
    stImgBuf.ui32UVAddr = stImgBuf.ui32YAddr +
        (m_pPortUo->GetWidth() * m_pPortUo->GetHeight());
    stImgBuf.ui16Stride = m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride;
    stImgBuf.ui16OffsetX = stExtBufInfo->x;
    stImgBuf.ui16OffsetY = stExtBufInfo->y;
    stOvImgBuf.ui32YAddr = (ISPOST_UINT32)stExtBufInfo->stExtBuf.virt_addr;
    stOvImgBuf.ui32UVAddr = stOvImgBuf.ui32YAddr +
        (stExtBufInfo->stride * stExtBufInfo->height);
    stOvImgBuf.ui16Stride = stExtBufInfo->stride;
    stOvImgBuf.ui16OffsetX = 0;
    stOvImgBuf.ui16OffsetY = 0;

    IspostSwOverlay(
        ui8location,
        stImgBuf,
        stOvImgBuf
    );
}

#endif //#if defined(INFOTM_SW_BLENDING)

void IPU_ISPOSTV2:: IspostSaveDataFromUO(IMG_INFO *stImgInfo, ST_EXT_BUF_INFO *stExtBufInfo)
{
    if (m_PortGetBuf & ISPOST_PT_UO) {
        memcpy(stImgInfo, &m_IspostUser.stUoInfo.stOutImgInfo, sizeof(IMG_INFO));
        if (ISPOST_ENABLE == m_IspostUser.stCtrl0.ui1EnDN) {
            m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
    }
        m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr = stExtBufInfo->stExtBuf.phys_addr;
        m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr = m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr +
            (stExtBufInfo->stride * stExtBufInfo->height);
        m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride = stExtBufInfo->stride;
    }
}

void IPU_ISPOSTV2::Prepare()
{
    int i;
    Buffer buf;
    char szBufName[20];
    bool Ret;


    IspostCheckParameter();
    if (0 == m_stFceJsonParam.mode_number)
    {
        //Load LcGridBuf
        for (i = 0; i < IPU_ISPOST_GRIDDATA_MAX; i++)
        {
            if (m_pszLcGridFileName[i] != NULL)
            {
                sprintf(szBufName, "LcGridBuf%d", i + 1);
                m_IspostUser.stLcInfo.stGridBufInfo.ui8Size = IspostParseGirdSize(m_pszLcGridFileName[i]);
                Ret = IspostLoadBinaryFile(szBufName, &m_pLcGridBuf[i], m_pszLcGridFileName[i]);
                LOGE("m_pszLcGridFileName[%d]: %s, GridSize: %d, Ret: %d\n", i, m_pszLcGridFileName[i], m_IspostUser.stLcInfo.stGridBufInfo.ui8Size, Ret);
            }
            if (m_IsLineBufEn)
            {
                if (m_pszLcGeometryFileName[i] != NULL)
                {
                    sprintf(szBufName, "LcGeometryBuf%d", i + 1);
                    Ret = IspostLoadBinaryFile(szBufName, &m_pLcGeometryBuf[i], m_pszLcGeometryFileName[i]);
                    LOGE("m_pszLcGeometryFileName[%d]: %s, Ret: %d\n", i, m_pszLcGeometryFileName[i], Ret);
                }
                if (m_pszLcGeometryFileName[0] == NULL)
                {
                    LOGE("(%s:L%d) geo file does not exist.\n", __func__, __LINE__);
                    throw VBERR_BADPARAM;
                }
            }
        }
    }
    if (m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW > m_IspostUser.stLcInfo.stGridBufInfo.ui8Size) {
        m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW = m_IspostUser.stLcInfo.stGridBufInfo.ui8Size;
    }
    if (m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH > m_IspostUser.stLcInfo.stGridBufInfo.ui8Size) {
        m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH = m_IspostUser.stLcInfo.stGridBufInfo.ui8Size;
    }

#if IPU_ISPOST_LC_ROTATE_DEBUG
    //=============================================================
    // Remember you have put normal grid data into array index 0,
    // and put rotate 90 degree grid data into array index 1,
    // finally put rotate 270 degree grid data into array index 2.
    //=============================================================
    if (90 == m_Rotate) {
        i = 1;
    } else if (270 == m_Rotate){
        i = 2;
    } else {
        i = 0;
    }
#else
    i = 0;
#endif

    if(m_pLcGridBuf[i] != NULL)
    {
        buf = m_pLcGridBuf[i]->GetBuffer();
        m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr = buf.fr_buf.phys_addr;
        m_IspostUser.stLcInfo.stGridBufInfo.ui32BufLen = buf.fr_buf.size;
        m_pLcGridBuf[i]->PutBuffer(&buf);
    }

    if(m_pLcGeometryBuf[i] != NULL)
    {
        buf = m_pLcGeometryBuf[i]->GetBuffer();
        m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufAddr = (ISPOST_UINT32)buf.fr_buf.phys_addr;
        m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufLen = buf.fr_buf.size;
        m_pLcGeometryBuf[i]->PutBuffer(&buf);
    }
    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV1 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV2 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV3 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV4 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV5 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV6 = 0;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV7 = 0;

#ifdef IPU_ISPOST_OVERLAY_TEST
    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
    m_IspostUser.stOvInfo.stOverlayMode.ui2OVM = OVERLAY_MODE_YUV;
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = 1;
    m_IspostUser.stOvInfo.stOvOffset[0].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[0].ui16Y = 64;
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width = 128;
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height = 64;
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride = 128;

    Ret = IspostLoadBinaryFile("OvImgBuf0", &m_pOvImgBuf[0], "/root/.ispost/ov_fpga_128x64.bin");
    buf = m_pOvImgBuf[0]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr = buf.fr_buf.phys_addr;
    IspostCalcPlanarAddress(m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height,
                        m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr,  m_IspostUser.stOvInfo.stOvImgInfo[0].ui32UVAddr);
    m_pOvImgBuf[0]->PutBuffer(&buf);

    Ret = IspostLoadBinaryFile("OvImgBuf1", &m_pOvImgBuf[1], "/root/.ispost/Overlay_1-7_Yuv_Alpha_1920x1080.bin");
    // Overlay channel 1.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV1 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[1].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[1].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[1].ui16Y = 32;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue0.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue1.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue2.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[1].stValue3.ui8Alpha0 = 0xcc;
    // Overlay channel 2.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV2 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[2].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[2].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[2].ui16Y = 140;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue0.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue1.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue2.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha1 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[2].stValue3.ui8Alpha0 = 0x80;
    // Overlay channel 3.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV3 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[3].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[3].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[3].ui16Y = 140;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue0.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue1.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue2.ui8Alpha0 = 0xcc;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[3].stValue3.ui8Alpha1 = 0x80;
    // Overlay channel 4.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV4 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[4].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[4].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[4].ui16Y = 256;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue0.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue1.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue2.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[4].stValue3.ui8Alpha0 = 0x20;
    // Overlay channel 5.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV5 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[5].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[5].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[5].ui16Y = 256;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue0.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue1.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue2.ui8Alpha0 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[5].stValue3.ui8Alpha0 = 0x20;
    // Overlay channel 6.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV6 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[6].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[6].ui16X = 64;
    m_IspostUser.stOvInfo.stOvOffset[6].ui16Y = 360;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha3 = 0x33;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue0.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha3 = 0x77;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue1.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha3 = 0xbb;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue2.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha3 = 0xff;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha1 = 0x20;
    m_IspostUser.stOvInfo.stOvAlpha[6].stValue3.ui8Alpha0 = 0x80;
    // Overlay channel 7.
    m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV7 = 0;
    buf = m_pOvImgBuf[1]->GetBuffer();
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui32YAddr = buf.fr_buf.phys_addr;
    m_pOvImgBuf[1]->PutBuffer(&buf);
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Stride = 1920 / 4;
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Width = 192;
    m_IspostUser.stOvInfo.stOvImgInfo[7].ui16Height = 108;
    m_IspostUser.stOvInfo.stOvOffset[7].ui16X = 320;
    m_IspostUser.stOvInfo.stOvOffset[7].ui16Y = 360;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha3 = 0xaf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha2 = 0x00;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue0.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha3 = 0xbf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha2 = 0x40;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue1.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha3 = 0xcf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha2 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue2.ui8Alpha0 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha3 = 0xdf;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha2 = 0xc0;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha1 = 0x80;
    m_IspostUser.stOvInfo.stOvAlpha[7].stValue3.ui8Alpha0 = 0x80;
#endif //IPU_ISPOST_OVERLAY_TEST


    m_stFce.maxModeGroupNb = 0;
    m_stFce.hasFce = false;
    m_stFce.stModeGroupAllList.clear();
    m_stFce.modeGroupActiveList.clear();
    m_stFce.modeGroupPendingList.clear();
    m_stFce.modeGroupIdleList.clear();

    m_HasFceData = false;
    m_HasFceFile = false;

    if (m_LinkMode)
    {
        m_pBufFunc = std::bind(&IPU_ISPOSTV2::ReturnBuf,this,std::placeholders::_1);
        if (m_pPortUo->IsEnabled())
            m_pPortUo->frBuffer->RegCbFunc(m_pBufFunc);
    }
    else if (m_SnapMode && m_SnapBufMode == 1)
    {
        m_pSnapBufFunc = std::bind(&IPU_ISPOSTV2::ReturnSnapBuf,this,std::placeholders::_1);
        if (m_pPortUo->IsEnabled())
            m_pPortUo->frBuffer->RegCbFunc(m_pSnapBufFunc);
    }

    m_stInBufList.clear();

    if (m_stFceJsonParam.mode_number)
    {
        IspostLoadAndFireFceData(&m_stFceJsonParam);
        m_IspostUser.stLcInfo.stGridBufInfo.ui8Size = \
            IspostParseGirdSize(m_stFceJsonParam.mode_list[0].file_list[0].file_grid);
        if (m_IsLineBufEn)
        {
            if (access(m_stFceJsonParam.mode_list[0].file_list[0].file_geo, R_OK))
            {
                LOGE("(%s:L%d) failed to read geo file %s \n", __func__, __LINE__,
                    m_stFceJsonParam.mode_list[0].file_list[0].file_geo);
                throw VBERR_BADPARAM;
            }
        }
    }
}

void IPU_ISPOSTV2::Unprepare()
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
    if (m_pPortUo->IsEnabled())
    {
         m_pPortUo->FreeVirtualResource();
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
    for (int i = 0; i < OVERLAY_CHNL_MAX; i++)
    {
        if (m_pPortOv[i]->IsEnabled())
        {
             m_pPortOv[i]->FreeVirtualResource();
        }
    }

    ReturnSnapBuf(&m_SnapUoBuf);
}

bool IPU_ISPOSTV2::IspostInitial(void)
{
    int i = 0;
    // IN ----------------------------------------
    if (m_pPortIn->IsEnabled())
    {
        if (m_IsLcManual && m_LcManualVal == 0) {
            m_IspostUser.stVsInfo.stSrcImgInfo.ui16Width = m_pPortIn->GetWidth();
            m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height = m_pPortIn->GetHeight();
            m_IspostUser.stVsInfo.stSrcImgInfo.ui16Stride = m_pPortIn->GetWidth();
        }
    }
    // OV ----------------------------------------
    for (i = 0; i < OVERLAY_CHNL_MAX; i++)
    {
        if (m_pPortOv[i]->IsEnabled())
        {
            m_IspostUser.stOvInfo.stOverlayMode.ui2OVM = OVERLAY_MODE_YUV;
            m_IspostUser.stOvInfo.stOvOffset[i].ui16X = m_pPortOv[i]->GetPipX();
            m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = m_pPortOv[i]->GetPipY();
            m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Width = m_pPortOv[i]->GetWidth();
            m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Height = m_pPortOv[i]->GetHeight();
            if (i > 0)
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = m_pPortOv[i]->GetWidth()/4;
            else
                m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = m_pPortOv[i]->GetWidth();

            LOGE("OV%d X=%d Y=%d W=%d H=%d S=%d\n",i ,
                        m_pPortOv[i]->GetPipX(),
                        m_pPortOv[i]->GetPipY(),
                        m_pPortOv[i]->GetWidth(),
                        m_pPortOv[i]->GetHeight(),
                        m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride);
        }
    }

    // DN ----------------------------------------
    if (m_pPortDn->IsEnabled() && m_DnManualVal)
    {
        //m_IspostUser.stCtrl0.ui1EnDN = ISPOST_ENABLE;
        m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride = m_pPortDn->GetWidth();
        m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride = m_pPortDn->GetWidth();
        if (m_Rotate) {
            m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride = m_pPortDn->GetHeight();
            m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride = m_pPortDn->GetHeight();
        }
    }
    else
    {
        //m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
        m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr = 0x0;
        m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr = 0x0;
        m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr = 0x0;
        m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr = 0x0;
    }

    // UO ----------------------------------------
    if (m_pPortUo->IsEnabled())
    {
        m_IspostUser.stUoInfo.ui8ScanH = SCAN_HEIGHT_16;
        if (m_pPortUo->IsEmbezzling())
        {
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_pPortUo->GetPipWidth();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = m_pPortUo->GetPipHeight();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride =  m_pPortUo->GetWidth();
            LOGD("SS0 PipW: %d, PipH: %d, Stride: %d", m_pPortUo->GetPipWidth(), m_pPortUo->GetPipHeight(), m_pPortUo->GetWidth());
        }
        else
        {
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_pPortUo->GetWidth();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = m_pPortUo->GetHeight();
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width;
        }
        if (m_Rotate) {
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stUoInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stUoInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stUoInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride = m_IspostUser.stUoInfo.stOutImgInfo.ui16Width;
        }
    }
    else
    {
        m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr = 0xFFFFFFFF;
        m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr = 0xFFFFFFFF;
    }

    if(SetupLcInfo() < 0)
    {
        return false;
    }

    // SS0 ----------------------------------------
    if (m_pPortSs0->IsEnabled())
    {
        if (m_pPortSs0->IsEmbezzling())
        {
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = m_pPortSs0->GetPipWidth();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = m_pPortSs0->GetPipHeight();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride =  m_pPortSs0->GetWidth();
            LOGD("SS0 PipW: %d, PipH: %d, Stride: %d", m_pPortSs0->GetPipWidth(), m_pPortSs0->GetPipHeight(), m_pPortSs0->GetWidth());
        }
        else
        {
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = m_pPortSs0->GetWidth();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = m_pPortSs0->GetHeight();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride = m_IspostUser.stSS0Info.stOutImgInfo.ui16Width;
        }
    }
    else
    {
        m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr = 0xFFFFFFFF;
        m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr = 0xFFFFFFFF;
    }

    // SS1 ----------------------------------------
    if (m_pPortSs1->IsEnabled())
    {
        if (m_pPortSs1->IsEmbezzling())
        {
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = m_pPortSs1->GetPipWidth();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = m_pPortSs1->GetPipHeight();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride =  m_pPortSs1->GetWidth();
        }
        else
        {
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = m_pPortSs1->GetWidth();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = m_pPortSs1->GetHeight();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride = m_IspostUser.stSS1Info.stOutImgInfo.ui16Width;
        }
    }
    else
    {
        m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr = 0xFFFFFFFF;
        m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr = 0xFFFFFFFF;
    }

    m_IspostUser.ui32IspostCount = 0;

    return true;
}

int IPU_ISPOSTV2::UpdateDnUserInfo(bool hasFce, cam_position_t *pstPos, ISPOST_UINT32 *pRefBufYAddr )
{
    int pipX = 0;
    int pipY = 0;


    if (NULL == pRefBufYAddr)
    {
        LOGE("(%s:L%d) param is NULL !!!\n", __func__, __LINE__);
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
    m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr = m_BufDn.fr_buf.phys_addr;
    m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr = m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());

    m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr = *pRefBufYAddr;
    m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr = m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr + (m_pPortDn->GetWidth() * m_pPortDn->GetHeight());
    if (hasFce)
        *pRefBufYAddr = m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr;
    if (m_pPortDn->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride,
                                m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr, m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr);

        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride,
                               m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr, m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr);
    }
    if (m_pPortIn->GetPipWidth() != 0 && m_pPortIn->GetPipHeight() != 0)
    {
        IspostCalcOffsetAddress(m_pPortIn->GetPipX(), m_pPortIn->GetPipY(), m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride,
                                m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr, m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr);
    }
    if (!hasFce)
        *pRefBufYAddr = m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr;

    return 0;
}

int IPU_ISPOSTV2::UpdateUoUserInfo(bool hasFce, cam_position_t *pstPos)
{
    int pipX = 0;
    int pipY = 0;


    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.stUoInfo.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stUoInfo.stOutImgInfo.ui16Height = pstPos->height;

        LOGD("(%s:%d) UO pipX=%d, pipY=%d %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortUo->IsEmbezzling())
    {
        pipX = m_pPortUo->GetPipX();
        pipY = m_pPortUo->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnUO = ISPOST_ENABLE;
    m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr = m_BufUo.fr_buf.phys_addr;
    m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr = m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr + (m_pPortUo->GetWidth() * m_pPortUo->GetHeight());
    if (m_pPortUo->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride,
                            m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr, m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr);
    }

    return 0;
}

int IPU_ISPOSTV2::UpdateLcUserInfo(bool hasFce, cam_position_t *pstPos)
{
    if (hasFce && pstPos)
    {
        m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = pstPos->height;
    }

    return 0;
}

int IPU_ISPOSTV2::UpdateSs0UserInfo(bool hasFce, cam_position_t *pstPos)
{
    int pipX = 0;
    int pipY = 0;


    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = pstPos->height;
        LOGD("(%s:%d) SS0 pipX=%d, pipY=%d, %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortSs0->IsEmbezzling())
    {
        pipX = m_pPortSs0->GetPipX();
        pipY = m_pPortSs0->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_ENABLE;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr = m_BufSs0.fr_buf.phys_addr;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr = m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr + (m_pPortSs0->GetWidth() * m_pPortSs0->GetHeight());
    if (m_pPortSs0->IsEmbezzling() || hasFce)
    {
            IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride,
                                m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr, m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr);
    }

    return 0;
}
int IPU_ISPOSTV2::UpdateSs1UserInfo(bool hasFce, cam_position_t *pstPos)
{
    int pipX = 0;
    int pipY = 0;


    if (hasFce && pstPos)
    {
        pipX = pstPos->x;
        pipY = pstPos->y;
        m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = pstPos->width;
        m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = pstPos->height;
        LOGD("(%s:%d) SS1 pipX=%d, pipY=%d, %dx%d \n", __func__, __LINE__, pipX, pipY, pstPos->width, pstPos->height);
    }
    else if (m_pPortSs1->IsEmbezzling())
    {
        pipX = m_pPortSs1->GetPipX();
        pipY = m_pPortSs1->GetPipY();
    }
    m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_ENABLE;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr = m_BufSs1.fr_buf.phys_addr;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr = m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr + (m_pPortSs1->GetWidth() * m_pPortSs1->GetHeight());
    if (m_pPortSs1->IsEmbezzling() || hasFce)
    {
        IspostCalcOffsetAddress(pipX, pipY, m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride,
                            m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr, m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr);
    }

    return 0;
}
bool IPU_ISPOSTV2::IspostTrigger(void)
{
    int ret = 0;
    int fileNumber = 1;
    int extBufNum = 0;
    bool hasOv = false;

    if (m_IspostUser.ui32IspostCount == 0)
        m_ui32DnRefImgBufYaddr =  0xFFFFFFFF;

    if (m_PortGetBuf & ISPOST_PT_LC)
    {
        m_IspostUser.stCtrl0.ui1EnLC = ISPOST_ENABLE;
        m_IspostUser.stCtrl0.ui1EnVSR = ISPOST_DISABLE;
        m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr = m_BufIn.fr_buf.phys_addr;
        m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr = m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr + (m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride * m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height);
    }
    else
    {
        m_IspostUser.stCtrl0.ui1EnLC = ISPOST_DISABLE;
        if (m_IsLcManual && m_LcManualVal == 0) {
            m_IspostUser.stCtrl0.ui1EnVSR = ISPOST_ENABLE;
            m_IspostUser.stVsInfo.stSrcImgInfo.ui32YAddr = m_BufIn.fr_buf.phys_addr;
            m_IspostUser.stVsInfo.stSrcImgInfo.ui32UVAddr = m_IspostUser.stVsInfo.stSrcImgInfo.ui32YAddr + (m_IspostUser.stVsInfo.stSrcImgInfo.ui16Stride * m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height);
        }
    }

    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
    if (m_PortGetBuf & ISPOST_PT_OV0_7)
    {
        for(int i = 0;i < OVERLAY_CHNL_MAX; i++)
        {
            if (m_PortGetBuf & (1 << (3 + i)))
            {
                if (m_BufOv[i].fr_buf.phys_addr != 0)
                {
                    hasOv = true;
                    m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
                    ISPOST_UINT32 *ov_enable = &m_IspostUser.stOvInfo.stOverlayMode.ui32OvMode;
                    *ov_enable |= 1 << (8 + i);

                    ST_OVERLAY_INFO* ov_info = NULL;
                    ov_info = (ST_OVERLAY_INFO*)(m_BufOv[i].fr_buf.priv);
                    if (ov_info)
                    {
                        if (ov_info->height > 0 && ov_info->width > 0)
                        {
                            if (ov_info->width != m_pPortOv[i]->GetWidth() ||ov_info->height != m_pPortOv[i]->GetHeight())
                                LOGE("\nOV7 [BUF] W=%d H=%d\n", ov_info->width,ov_info->height);
                            m_pPortOv[i]->SetResolution(ov_info->width,ov_info->height);
                        }

                        if (ov_info->disable == 1)
                        {
                            ISPOST_UINT32 *ov_enable = &m_IspostUser.stOvInfo.stOverlayMode.ui32OvMode;
                            *ov_enable &= ~ (1 << (8 + i));
                        }
                    }
                    m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Width = m_pPortOv[i]->GetWidth() ;
                    m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Height =m_pPortOv[i]->GetHeight();
                    if(i == 0)
                        m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Width;
                    else
                        m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Stride = m_IspostUser.stOvInfo.stOvImgInfo[i].ui16Width>>2;

                    pthread_mutex_lock( &m_PipMutex);
                    IspostAdjustOvPos(i);
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16X = m_pPortOv[i]->GetPipX();
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = m_pPortOv[i]->GetPipY();
                    pthread_mutex_unlock( &m_PipMutex);

                    m_IspostUser.stOvInfo.stOvImgInfo[i].ui32YAddr = m_BufOv[i].fr_buf.phys_addr;
                    if(i == 0)
                        IspostCalcPlanarAddress(m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height,
                            m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr,  m_IspostUser.stOvInfo.stOvImgInfo[0].ui32UVAddr);
                }
                else
                {
                    ISPOST_UINT32 *ov_enable = &m_IspostUser.stOvInfo.stOverlayMode.ui32OvMode;
                    *ov_enable &= ~ (1 << (8 + i));
                }
            }
        }
    }
#ifdef IPU_ISPOST_OVERLAY_TEST
    else
    {
        m_BufOv[0] = m_pOvImgBuf[0]->GetBuffer();
        m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr = m_BufOv[0].fr_buf.phys_addr;
        IspostCalcPlanarAddress(m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Stride, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height,
                                m_IspostUser.stOvInfo.stOvImgInfo[0].ui32YAddr,  m_IspostUser.stOvInfo.stOvImgInfo[0].ui32UVAddr);
        m_pOvImgBuf[0]->PutBuffer(&m_BufOv[0]);
        m_BufOv[1] = m_pOvImgBuf[1]->GetBuffer();
        m_IspostUser.stOvInfo.stOvImgInfo[1].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[2].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[3].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[4].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[5].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[6].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_IspostUser.stOvInfo.stOvImgInfo[7].ui32YAddr = m_BufOv[1].fr_buf.phys_addr;
        m_pOvImgBuf[1]->PutBuffer(&m_BufOv[1]);
    }
#endif //IPU_ISPOST_OVERLAY_TEST

    ipu_v2500_private_data_t *priv = (ipu_v2500_private_data_t*)m_BufIn.fr_buf.priv;
    if(priv)
    {
        enum cam_mirror_state mirror = priv->mirror;
        int in_w,in_h;
        in_w = m_pPortIn->GetWidth();
        in_h = m_pPortIn->GetHeight();
        for (int i = 0; i < OVERLAY_CHNL_MAX; i++)
        {
            if (m_pPortOv[i]->IsEnabled())
            {
                if(mirror == CAM_MIRROR_H)
                {
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16X = in_w - m_pPortOv[i]->GetPipX() - m_pPortOv[i]->GetWidth();
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = m_pPortOv[i]->GetPipY();
                }
                else if(mirror == CAM_MIRROR_V)
                {
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16X = m_pPortOv[i]->GetPipX();
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = in_h - m_pPortOv[i]->GetPipY()- m_pPortOv[i]->GetHeight();
                }
                else if(mirror == CAM_MIRROR_HV)
                {
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16X = in_w - m_pPortOv[i]->GetPipX() - m_pPortOv[i]->GetWidth();
                    m_IspostUser.stOvInfo.stOvOffset[i].ui16Y = in_h - m_pPortOv[i]->GetPipY()- m_pPortOv[i]->GetHeight();
                }
                else
                {

                }
            }
        }
    }

    bool hasFce = false;
    cam_position_t stDnPos;
    cam_position_t stUoPos;
    cam_position_t stSs0Pos;
    cam_position_t stSs1Pos;
    ST_FCE_MODE stActiveMode;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    ST_EXT_BUF_INFO stExtBufInfo;
#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)
    IMG_INFO stImgInfo;
#endif //#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)


    pthread_mutex_lock(&m_FceLock);
    memset(&stBufInfo, 0, sizeof(stBufInfo));
    if (m_stFce.hasFce)
    {
        ret = GetFceActiveMode(&stActiveMode);
        if (0 == ret)
        {
            hasFce = true;
            fileNumber = stActiveMode.stBufList.size();
            extBufNum = stActiveMode.stExtBufList.size();
            if (extBufNum == 0)
            {
                stExtBufInfo.modeID = 0xff;
            }
        }
    }
    if (hasFce)
        UpdateFcePendingStatus();
    pthread_mutex_unlock(&m_FceLock);


    int i = 0;
    int j = 0;

    do
    {
        if (hasFce)
        {
            stBufInfo = stActiveMode.stBufList.at(i);

            if (!stActiveMode.stPosDnList.empty())
                stDnPos = stActiveMode.stPosDnList.at(i);
            if (!stActiveMode.stPosUoList.empty())
                stUoPos = stActiveMode.stPosUoList.at(i);
            if (!stActiveMode.stPosSs0List.empty())
                stSs0Pos = stActiveMode.stPosSs0List.at(i);
            if (!stActiveMode.stPosSs1List.empty())
                stSs1Pos = stActiveMode.stPosSs1List.at(i);
        }

        if (stBufInfo.stBufGrid.phys_addr)
        {
            m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr = stBufInfo.stBufGrid.phys_addr;
            m_IspostUser.stLcInfo.stGridBufInfo.ui32BufLen = stBufInfo.stBufGrid.size;
            if (m_IsLineBufEn)
            {
                if (stBufInfo.stBufGeo.phys_addr)
                {
                    m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufAddr = stBufInfo.stBufGeo.phys_addr;
                    m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufLen = stBufInfo.stBufGeo.size;
                }
                else
                {
                    LOGE("(%s:L%d) geo file %d does not exist.\n", __func__, __LINE__, i);
                    return false;
                }
            }
        }
        if (hasOv)
        {
            if (i == (fileNumber - 1))
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_ENABLE;
            else
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
        }
        if (m_PortGetBuf & ISPOST_PT_DN)
        {
            UpdateDnUserInfo(hasFce, &stDnPos, &m_ui32DnRefImgBufYaddr);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
        }

        if (m_PortGetBuf & ISPOST_PT_UO)
        {
            UpdateUoUserInfo(hasFce, &stUoPos);
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnUO = ISPOST_DISABLE;
        }

        if(m_PortGetBuf & ISPOST_PT_LC)
        {
            if ( m_PortGetBuf & ISPOST_PT_DN)
            {
                UpdateLcUserInfo(hasFce, &stDnPos);
            }
            else if(m_PortGetBuf & ISPOST_PT_UO)
            {
                UpdateLcUserInfo(hasFce, &stUoPos);
            }
        }

        if (m_PortGetBuf & ISPOST_PT_SS0)
        {
            UpdateSs0UserInfo(hasFce, &stSs0Pos);
#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)
            if (m_IsBlendScale) {
                m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_DISABLE;
            }
#endif
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_DISABLE;
        }
        if (m_PortGetBuf & ISPOST_PT_SS1)
        {
            UpdateSs1UserInfo(hasFce, &stSs1Pos);
#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)
            if (m_IsBlendScale) {
                m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_DISABLE;
            }
#endif
        }
        else
        {
            m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_DISABLE;
        }
#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)
        if (4 < fileNumber) {
            switch (i) {
            case 0:
                for (j = 0; j < extBufNum; j++) {
                    stExtBufInfo = stActiveMode.stExtBufList.at(j);
                    if (stExtBufInfo.modeID == i)
                    break;
                }

                IspostSaveDataFromUO(&stImgInfo, &stExtBufInfo);
            break;

            case 1:
#if defined(INFOTM_HW_BLENDING)
//                if (m_IspostUser.ui32IspostCount == 10)// && m_IspostUser.ui32IspostCount == 60)test
//                    WriteFile(stExtBufInfo.stExtBuf.virt_addr, stExtBufInfo.stExtBuf.size, "/mnt/sd0/stExtBufInfo_case1.yuv");

                for (j = 0; j < extBufNum; j++) {
                    stExtBufInfo = stActiveMode.stExtBufList.at(j);
                    if (stExtBufInfo.overlayTiming == 1)
                        break;
                }

                IspostExtBufForHWBlending(BLENDING_SIDE_LEFT, &stExtBufInfo);
#endif
            break;

            case 2:
                for (j = 0; j < extBufNum; j++) {
                    stExtBufInfo = stActiveMode.stExtBufList.at(j);
                    if (stExtBufInfo.modeID == i)
                    break;
                }

                IspostSaveDataFromUO(&stImgInfo, &stExtBufInfo);
            break;

            case 3:
#if defined(INFOTM_HW_BLENDING)
//                if (m_IspostUser.ui32IspostCount == 10)// && m_IspostUser.ui32IspostCount == 60)test
//                    WriteFile(stExtBufInfo.stExtBuf.virt_addr, stExtBufInfo.stExtBuf.size, "/mnt/sd0/stExtBufInfo_case3.yuv");

                for (j = 0; j < extBufNum; j++) {
                    stExtBufInfo = stActiveMode.stExtBufList.at(j);
                    if (stExtBufInfo.overlayTiming == 3)
                    break;
                }

                IspostExtBufForHWBlending(BLENDING_SIDE_RIGHT, &stExtBufInfo);
#endif
            break;

            case 4:
            break;
            }
        }
#endif //#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)

        //IspostDumpUserInfo();
        ret = ispost_full_trigger(&m_IspostUser);

#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)
        if (4 < fileNumber) {
            switch (i) {
            case 0:
                if (m_PortGetBuf & ISPOST_PT_UO) {
                    memcpy(&m_IspostUser.stUoInfo.stOutImgInfo, &stImgInfo, sizeof(IMG_INFO));
                }
            break;

            case 1:
                if (m_PortGetBuf & ISPOST_PT_UO) {
#if defined(INFOTM_HW_BLENDING)
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = ISPOST_DISABLE;
#elif defined(INFOTM_SW_BLENDING)
                for (j = 0; j < extBufNum; j++) {
                    stExtBufInfo = stActiveMode.stExtBufList.at(j);
                    if (stExtBufInfo.overlayTiming == 1)
                    break;
                }

                IspostExtBufForSWBlending(BLENDING_SIDE_LEFT, &stExtBufInfo);
#endif
            }
            break;

            case 2:
                if (m_PortGetBuf & ISPOST_PT_UO) {
                    memcpy(&m_IspostUser.stUoInfo.stOutImgInfo, &stImgInfo, sizeof(IMG_INFO));
                }
            break;

            case 3:
                if (m_PortGetBuf & ISPOST_PT_UO) {
#if defined(INFOTM_HW_BLENDING)
                m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;
                m_IspostUser.stOvInfo.stOverlayMode.ui1ENOV0 = ISPOST_DISABLE;
#elif defined(INFOTM_SW_BLENDING)
                for (j = 0; j < extBufNum; j++) {
                    stExtBufInfo = stActiveMode.stExtBufList.at(j);
                    if (stExtBufInfo.overlayTiming == 3)
                    break;
                }

                IspostExtBufForSWBlending(BLENDING_SIDE_RIGHT, &stExtBufInfo);
#endif
            }
            break;

            case 4:
            break;
            }
        }
#endif //#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)

    }while(++i < fileNumber);

#if defined(INFOTM_HW_BLENDING) || defined(INFOTM_SW_BLENDING)
    if (m_IsBlendScale) {
        m_IspostUser.stCtrl0.ui1EnLC = ISPOST_DISABLE;
        m_IspostUser.stCtrl0.ui1EnVSR = ISPOST_ENABLE;
        m_IspostUser.stVsInfo.stSrcImgInfo.ui16Stride = m_pPortUo->GetWidth();
        m_IspostUser.stVsInfo.stSrcImgInfo.ui16Width = m_pPortUo->GetWidth();
        m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height = m_pPortUo->GetHeight();
        m_IspostUser.stVsInfo.stSrcImgInfo.ui32YAddr = m_BufUo.fr_buf.phys_addr;
        m_IspostUser.stVsInfo.stSrcImgInfo.ui32UVAddr = m_IspostUser.stVsInfo.stSrcImgInfo.ui32YAddr + (m_IspostUser.stVsInfo.stSrcImgInfo.ui16Stride * m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height);

        m_IspostUser.stCtrl0.ui1EnDN = ISPOST_DISABLE;
        m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride = m_pPortDn->GetWidth();

        m_IspostUser.stCtrl0.ui1EnUO = ISPOST_DISABLE;
        m_IspostUser.stCtrl0.ui1EnOV = ISPOST_DISABLE;

        if (m_PortGetBuf & ISPOST_PT_SS1) {
            m_IspostUser.stCtrl0.ui1EnSS1 = ISPOST_ENABLE;
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = m_pPortSs1->GetWidth();
            m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = m_pPortSs1->GetHeight();
            m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr = m_BufSs1.fr_buf.phys_addr;
            m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr = m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr + (m_pPortSs1->GetWidth() * m_pPortSs1->GetHeight());
        }

        if (m_PortGetBuf & ISPOST_PT_SS0) {
            m_IspostUser.stCtrl0.ui1EnSS0 = ISPOST_ENABLE;
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = m_pPortSs0->GetWidth();
            m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = m_pPortSs0->GetHeight();
            m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr = m_BufSs0.fr_buf.phys_addr;
            m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr = m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr + (m_pPortSs0->GetWidth() * m_pPortSs0->GetHeight());
        }

        ret = ispost_full_trigger(&m_IspostUser);
    }
#endif

    m_IspostUser.ui32IspostCount++;
    LOGD("Trigger ctrl0 LC:%d, OV:%d, DN:%d, SS0:%d, SS1:%d, UO:%d  \n",
                m_IspostUser.stCtrl0.ui1EnLC,
                m_IspostUser.stCtrl0.ui1EnOV,
                m_IspostUser.stCtrl0.ui1EnDN,
                m_IspostUser.stCtrl0.ui1EnSS0,
                m_IspostUser.stCtrl0.ui1EnSS1,
                m_IspostUser.stCtrl0.ui1EnUO
            );
#ifdef IPU_ISPOST_LC_DEBUG
    if (m_IsFisheye) {
        if (0 == (m_IspostUser.ui32IspostCount % 5)) {
            m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx++;
            if (m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx >= (ISPOST_UINT32)grid_target_index_max)
                m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx = 0;
            m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx++;
        }
    }
#endif

#if 0  //for test trigger after ispost image
    if (m_PortEnable & ISPOST_PT_SS1 && m_IspostUser.ui32IspostCount == 20)
        WriteFile(m_BufSs1.fr_buf.virt_addr, m_BufSs1.fr_buf.size, "/nfs/ispost_ss1_%d.yuv", m_IspostUser.ui32IspostCount);
    if (m_PortEnable & ISPOST_PT_SS0 && m_IspostUser.ui32IspostCount == 30)
        WriteFile(m_BufSs0.fr_buf.virt_addr, m_BufSs0.fr_buf.size, "/nfs/ispost_ss0_%d.yuv", m_IspostUser.ui32IspostCount);
    if (m_PortEnable & ISPOST_PT_UO && m_IspostUser.ui32IspostCount == 40)
        WriteFile(m_BufUo.fr_buf.virt_addr, m_BufUo.fr_buf.size, "/nfs/ispost_uo_%d.yuv", m_IspostUser.ui32IspostCount);
    if (m_PortEnable & ISPOST_PT_DN && m_IspostUser.ui32IspostCount == 50)
        WriteFile(m_BufDn.fr_buf.virt_addr, m_BufDn.fr_buf.size, "/nfs/ispost_dn_%d.yuv", m_IspostUser.ui32IspostCount);
    //if (m_PortEnable & ISPOST_PT_OV0 && m_IspostUser.ui32IspostCount == 60)
    //  WriteFile(m_BufOv[0].fr_buf.virt_addr, m_BufOv[0].fr_buf.size, "/nfs/ispost_ol_%d.yuv", m_IspostUser.ui32IspostCount);
    //if (m_PortEnable & ISPOST_PT_IN && m_IspostUser.ui32IspostCount % 100 == 0)
    //  WriteFile(m_BufIn.fr_buf.virt_addr, m_BufIn.fr_buf.size, "/nfs/ispost_in_%d.yuv", m_IspostUser.ui32IspostCount);
#endif //0

    return (ret >= 0) ? true : false;
}

void IPU_ISPOSTV2::IspostDefaultParameter(void)
{
    ISPOST_UINT8 ui8Idx;

    memset(&m_IspostUser, 0, sizeof(ISPOST_USER));

    m_IspostUser.ui32UpdateFlag = 0;
    m_IspostUser.ui32PrintFlag = 0; //ISPOST_PRN_ALL;
    m_IspostUser.ui32PrintStep = 1000;
    m_IspostUser.ui32IspostCount = 0;

    //Ispost control
    m_IspostUser.stCtrl0.ui1EnLC = 0;
    m_IspostUser.stCtrl0.ui1EnVSR = 0;
    m_IspostUser.stCtrl0.ui1EnOV = 0;
    m_IspostUser.stCtrl0.ui1EnDN = 0;
    m_IspostUser.stCtrl0.ui1EnSS0 = 0;
    m_IspostUser.stCtrl0.ui1EnSS1 = 0;
    m_IspostUser.stCtrl0.ui1EnUO = 0;

    //-----------------------------------------------------------------
    //Lens correction information
    m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width = 0;
    m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height = 0;

    m_IspostUser.stLcInfo.stBackFillColor.ui8Y = LC_BACK_FILL_Y;
    m_IspostUser.stLcInfo.stBackFillColor.ui8CB = LC_BACK_FILL_CB;
    m_IspostUser.stLcInfo.stBackFillColor.ui8CR = LC_BACK_FILL_CR;

    m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32BufLen = 0; //No use
    m_IspostUser.stLcInfo.stGridBufInfo.ui16Stride = 0; //driver setting
    m_IspostUser.stLcInfo.stGridBufInfo.ui8Size = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufAddr = 0;
    m_IspostUser.stLcInfo.stGridBufInfo.ui32GeoBufLen = 0; //No use

    m_IspostUser.stLcInfo.stPxlCacheMode.ui1CBCRSwap = CBCR_SWAP_MODE_SWAP;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui2PreFetchCtrl = LC_CACHE_MODE_CTRL_CFG;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui1BurstLenSet = LC_CACHE_MODE_BURST_LEN;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui2CacheDataShapeSel = LC_CACHE_MODE_DAT_SHAPE;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui2LineBufFormat = LC_CACHE_MODE_LBFMT;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui1LineBufFifoType = LC_CACHE_MODE_FIFO_TYPE;
    m_IspostUser.stLcInfo.stPxlCacheMode.ui1WaitLineBufEnd = LC_CACHE_MODE_WLBE;

    m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW = SCAN_WIDTH_32;
    m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH = SCAN_HEIGHT_32;
    m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanM = SCAN_MODE_LR_TB;

    m_IspostUser.stLcInfo.stOutImgInfo.ui32YAddr = 0; //No use
    m_IspostUser.stLcInfo.stOutImgInfo.ui32UVAddr = 0; //No use
    m_IspostUser.stLcInfo.stOutImgInfo.ui16Stride = 0; //No use
    m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = 0;
    m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = 0;

    m_IspostUser.stLcInfo.stAxiId.ui8SrcReqLmt = 4;
    m_IspostUser.stLcInfo.stAxiId.ui8GBID = 1;
    m_IspostUser.stLcInfo.stAxiId.ui8SRCID = 0;

    //-----------------------------------------------------------------
    //Video stabilization information
    m_IspostUser.stVsInfo.stSrcImgInfo.ui32YAddr = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui32UVAddr = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui16Stride = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui16Width = 0;
    m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height = 0;
    m_IspostUser.stVsInfo.stRegOffset.ui16Roll = 0;
    m_IspostUser.stVsInfo.stRegOffset.ui16HOff = 0;
    m_IspostUser.stVsInfo.stRegOffset.ui8VOff = 0;
    m_IspostUser.stVsInfo.stAxiId.ui8FWID = 0;
    m_IspostUser.stVsInfo.stAxiId.ui8RDID = 2;
    m_IspostUser.stVsInfo.stCtrl0.ui32Ctrl = 0;

    //-----------------------------------------------------------------
    //Overlay image information
    m_IspostUser.stOvInfo.stOverlayMode.ui2OVM = OVERLAY_MODE_YUV;
    for (ui8Idx = 0; ui8Idx < OVERLAY_CHNL_MAX; ui8Idx++)
    {
        m_IspostUser.stOvInfo.ui8OvIdx[ui8Idx] = 0;

        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Stride = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Width = 0;
        m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Height = 0;

        m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16X = 0;
        m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16Y = 0;

#ifdef INFOTM_BLEND_8STEP_ALPHA
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha = 0x66442200; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha = 0xEECCAA88; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha = 0x66442200; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha = 0xEECCAA88; //0xFFFFFFFF
#else
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha = 0x33221100; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha = 0x77665544; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha = 0xBBAA9988; //0xFFFFFFFF
        m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha = 0xFFEEDDCC; //0xFFFFFFFF
#endif

        m_IspostUser.stOvInfo.stAxiId[ui8Idx].ui8OVID = 8;
    }

    //-----------------------------------------------------------------
    //3D-denoise information
    m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr = 0;
    m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr = 0;
    m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride = 0;
    m_IspostUser.stDnInfo.ui32DnTargetIdx = 0;
    IspostSet3dDnsCurve(&m_Dns3dAttr, 1);
    m_IspostUser.stDnInfo.stAxiId.ui8RefWrID = 3;
    m_IspostUser.stDnInfo.stAxiId.ui8RefRdID = 3;

    //-----------------------------------------------------------------
    //Un-Scaled output information
    m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stUoInfo.ui8ScanH = SCAN_HEIGHT_16;
    m_IspostUser.stUoInfo.stAxiId.ui8RefWrID = 4;

    //-----------------------------------------------------------------
    //Scaling stream 0 information
    m_IspostUser.stSS0Info.ui8StreamSel = SCALING_STREAM_0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui16Width = 0;
    m_IspostUser.stSS0Info.stOutImgInfo.ui16Height = 0;
    m_IspostUser.stSS0Info.stHSF.ui16SF = 0;
    m_IspostUser.stSS0Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS0Info.stVSF.ui16SF = 0;
    m_IspostUser.stSS0Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS0Info.stAxiId.ui8SSWID = 1;

    //-----------------------------------------------------------------
    //Scaling stream 1 information
    m_IspostUser.stSS1Info.ui8StreamSel = SCALING_STREAM_1;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui16Width = 0;
    m_IspostUser.stSS1Info.stOutImgInfo.ui16Height = 0;
    m_IspostUser.stSS1Info.stHSF.ui16SF = 0;
    m_IspostUser.stSS1Info.stHSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS1Info.stVSF.ui16SF = 0;
    m_IspostUser.stSS1Info.stVSF.ui8SM = SCALING_MODE_SCALING_DOWN;
    m_IspostUser.stSS1Info.stAxiId.ui8SSWID = 2;
}

void IPU_ISPOSTV2::IspostCheckParameter()
{

    if (((m_pPortIn->GetPipX() < 0) || (m_pPortIn->GetPipX()> 8192) || (m_pPortIn->GetPipX() % 2 != 0)) ||
        (m_pPortIn->GetPipY() < 0) || (m_pPortIn->GetPipY() > 8192) || (m_pPortIn->GetPipY() % 2 != 0) ||
        (m_pPortIn->GetPipWidth()< 0) || (m_pPortIn->GetPipWidth() > 8192) || (m_pPortIn->GetPipWidth() % 8 != 0) ||
        (m_pPortIn->GetPipHeight() < 0) || (m_pPortIn->GetPipHeight() > 8192) || (m_pPortIn->GetPipHeight() % 8 != 0))
    {
        LOGE("the Crop(x,y,w,h) has to be a 8 multiple value.\n");
        throw VBERR_BADPARAM;
    }

    if (m_pPortIn->GetPipX() > m_pPortIn->GetWidth() ||
        m_pPortIn->GetPipY() > m_pPortIn->GetHeight() ||
        m_pPortIn->GetPipWidth() > m_pPortIn->GetWidth() ||
        m_pPortIn->GetPipHeight() > m_pPortIn->GetHeight() ||
        m_pPortIn->GetPipX() + m_pPortIn->GetPipWidth() > m_pPortIn->GetWidth() ||
        m_pPortIn->GetPipY() + m_pPortIn->GetPipHeight() > m_pPortIn->GetHeight())
    {
        LOGE("ISPost Crop params error\n");
        throw VBERR_BADPARAM;
    }

    if ((m_pPortIn->GetPixelFormat() != Pixel::Format::NV12)
        && (m_pPortIn->GetPixelFormat() != Pixel::Format::NV21))
    {
        LOGE("ISPost PortIn format params error\n");
        throw VBERR_BADPARAM;
    }

    if (m_pPortOv[0]->IsEnabled())
    {
        if ((m_pPortOv[0]->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortOv[0]->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortOv0 format params error\n");
            throw VBERR_BADPARAM;
        }
    }
    for(int i = 1; i < OVERLAY_CHNL_MAX; i++)
    {
        if (m_pPortOv[i]->IsEnabled())
        {
            if (m_pPortOv[i]->GetPixelFormat() != Pixel::Format::BPP2)
            {
                LOGE("Ov%d Port Pixel Format Params Error Not Support\n", i);
                throw VBERR_BADPARAM;
            }
        }
    }

    if (m_pPortDn->IsEnabled())
    {
        if ((m_pPortDn->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortDn->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortDn format params error\n");
            throw VBERR_BADPARAM;
        }
        if (m_pPortDn->IsEmbezzling())
        {
            if (m_pPortDn->GetPipX() < 0 ||
                m_pPortDn->GetPipY() < 0 ||
                (m_pPortDn->GetPipX() + m_pPortDn->GetPipWidth()) > m_pPortDn->GetWidth() ||
                (m_pPortDn->GetPipY() + m_pPortDn->GetPipHeight()) > m_pPortDn->GetHeight())
            {
                LOGE("ISPost PortDn Embezzling params error\n");
                LOGE("PortDn(W:%d, H:%d), PortDn(X:%d, Y:%d, W:%d, H%d)\n",
                    m_pPortDn->GetWidth(), m_pPortDn->GetHeight(),
                    m_pPortDn->GetPipX(), m_pPortDn->GetPipY(),
                    m_pPortDn->GetPipWidth(), m_pPortDn->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            m_pPortDn->SetPipInfo(0, 0, m_pPortDn->GetWidth(), m_pPortDn->GetHeight());
        }
        if (m_pPortDn->GetPipWidth() < 16 || m_pPortDn->GetPipWidth() > 4096 ||
            m_pPortDn->GetPipHeight() < 16 || m_pPortDn->GetPipHeight() > 4096)
        {
            LOGE("ISPost not embezzling PortDn 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

    if (m_pPortSs0->IsEnabled())
    {
        if ((m_pPortSs0->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortSs0->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortSs0 format params error\n");
            throw VBERR_BADPARAM;
        }
        if (m_pPortSs0->IsEmbezzling())
        {
            if (m_pPortSs0->GetPipX() < 0 ||
                m_pPortSs0->GetPipY() < 0 ||
                (m_pPortSs0->GetPipX() + m_pPortSs0->GetPipWidth()) > m_pPortSs0->GetWidth() ||
                (m_pPortSs0->GetPipY() + m_pPortSs0->GetPipHeight()) > m_pPortSs0->GetHeight())
            {
                LOGE("ISPost PortSs0 Embezzling params error\n");
                LOGE("PortSs0(W:%d, H:%d), Pip(X:%d, Y:%d, W:%d, H%d)\n",
                     m_pPortSs0->GetWidth(), m_pPortSs0->GetHeight(),
                    m_pPortSs0->GetPipX(), m_pPortSs0->GetPipY(),
                    m_pPortSs0->GetPipWidth(), m_pPortSs0->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            m_pPortSs0->SetPipInfo(0, 0, m_pPortSs0->GetWidth(), m_pPortSs0->GetHeight());
        }
        if (m_pPortSs0->GetPipWidth() < 16 || m_pPortSs0->GetPipWidth() > 4096 ||
            m_pPortSs0->GetPipHeight() < 16 || m_pPortSs0->GetPipHeight() > 4096)
        {
            LOGE("ISPost not embezzling PortSS0 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

    if (m_pPortSs1->IsEnabled())
    {
        if ((m_pPortSs1->GetPixelFormat() != Pixel::Format::NV12)
            && (m_pPortSs1->GetPixelFormat() != Pixel::Format::NV21))
        {
            LOGE("ISPost PortSs1 format params error\n");
            throw VBERR_BADPARAM;
        }
        if (m_pPortSs1->IsEmbezzling())
        {
            if (m_pPortSs1->GetPipX() < 0 ||
                m_pPortSs1->GetPipY() < 0 ||
                (m_pPortSs1->GetPipX() + m_pPortSs1->GetPipWidth()) > m_pPortSs1->GetWidth() ||
                (m_pPortSs1->GetPipY() + m_pPortSs1->GetPipHeight()) > m_pPortSs1->GetHeight())
            {
                LOGE("ISPost PortSs1 Embezzling params error\n");
                LOGE("PortSs1(W:%d, H:%d), Pip(X:%d, Y:%d, W:%d, H%d)\n",
                     m_pPortSs1->GetWidth(), m_pPortSs1->GetHeight(),
                    m_pPortSs1->GetPipX(), m_pPortSs1->GetPipY(),
                    m_pPortSs1->GetPipWidth(), m_pPortSs1->GetPipHeight());
                throw VBERR_BADPARAM;
            }
        }
        else
        {
            m_pPortSs1->SetPipInfo(0, 0, m_pPortSs1->GetWidth(), m_pPortSs1->GetHeight());
        }
        if (m_pPortSs1->GetPipWidth() < 16 || m_pPortSs1->GetPipWidth() > 4096 ||
            m_pPortSs1->GetPipHeight() < 16 || m_pPortSs1->GetPipHeight() > 4096)
        {
            LOGE("ISPost not embezzling PortSs1 16 > pip > 4096\n");
            throw VBERR_BADPARAM;
        }
    }

}

bool IPU_ISPOSTV2::IspostJsonGetInt(const char* pszItem, int& Value)
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

bool IPU_ISPOSTV2::IspostJsonGetString(const char* pszItem, char*& pszString)
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


ISPOST_UINT32 IPU_ISPOSTV2::IspostGetGridTargetIndexMax(int width, int height, ISPOST_UINT8 sz, ISPOST_UINT32 nload)
{
    ISPOST_UINT32 ui32Stride;
    ISPOST_UINT32 ui32GridSize;
    ISPOST_UINT32 ui32Width;
    ISPOST_UINT32 ui32Height;
    ISPOST_UINT32 multi;

    if (GRID_SIZE_MAX <= sz)
    {
        sz = GRID_SIZE_8_8;
    }

    ui32GridSize = g_dwGridSize[sz];
    ui32Width = (ISPOST_UINT32)width / ui32GridSize;
    if ((ISPOST_UINT32)width > (ui32Width * ui32GridSize))
    {
        ui32Width += 2;
    }
    else
    {
        ui32Width++;
    }
    ui32Stride = ui32Width * 16;

    ui32Height = (ISPOST_UINT32)height / ui32GridSize;
    if ((ISPOST_UINT32)height > (ui32Height * ui32GridSize))
    {
        ui32Height += 2;
    }
    else
    {
        ui32Height++;
    }

    multi = ui32Height * ui32Stride;
    if(multi >= nload)
        return 1;

    return (multi + 63) & 0xFFFFFFC0;
}


ISPOST_UINT32 IPU_ISPOSTV2::IspostGetFileSize(FILE* pFileID)
{
    ISPOST_UINT32 pos = ftell(pFileID);
    ISPOST_UINT32 len = 0;
    fseek(pFileID, 0L, SEEK_END);
    len = ftell(pFileID);
    fseek(pFileID, pos, SEEK_SET);
    return len;
}

bool IPU_ISPOSTV2::IspostLoadBinaryFile(const char* pszBufName, FRBuffer** ppBuffer, const char* pszFilenane)
{
    ISPOST_UINT32 nLoaded = 0, temp = 0;
    FILE* f = NULL;
    Buffer buf;

    if (*ppBuffer != NULL)
    {
        delete(*ppBuffer);
        *ppBuffer = NULL;
    }

    if ((f = fopen(pszFilenane, "rb")) == NULL){
        LOGE("failed to open %s!\n", pszFilenane);
        return false;
    }

    nLoaded = IspostGetFileSize(f);
    if(strncmp(pszBufName, "LcGridBuf", 9) == 0){
        temp = IspostGetGridTargetIndexMax(m_pPortUo->GetWidth(), m_pPortUo->GetHeight(), m_IspostUser.stLcInfo.stGridBufInfo.ui8Size, nLoaded);
        grid_target_index_max = (temp == 1) ? temp:(nLoaded / temp);

        LOGE("nLoaded:%d, temp:%d, sz:%d, width:%d, height:%d, grid_target_index_max:%d\n", nLoaded, temp, m_IspostUser.stLcInfo.stGridBufInfo.ui8Size, m_pPortDn->GetWidth(), m_pPortDn->GetHeight(), grid_target_index_max);
    }

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

ISPOST_UINT32 IPU_ISPOSTV2::IspostParseGirdSize(const char* pszFileName)
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

void IPU_ISPOSTV2::IspostCalcPlanarAddress(int w, int h, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr)
{
    UVaddr = Yaddr + (w * h);
}

void IPU_ISPOSTV2::IspostCalcOffsetAddress(int x, int y, int Stride, ISPOST_UINT32& Yaddr, ISPOST_UINT32& UVaddr)
{
    Yaddr += ((y * Stride) + x);
    UVaddr += (((y / 2) * Stride) + x);
}

void IPU_ISPOSTV2::IspostDumpUserInfo(void)
{
    ISPOST_UINT8 ui8Idx;

    LOGE("--------------------------------------------------------\n");
    LOGE("ui32UpdateFlag: 0x%04X\n", m_IspostUser.ui32UpdateFlag);
    LOGE("ui32PrintFlag: 0x%04X\n", m_IspostUser.ui32PrintFlag);
    LOGE("ui32IspostCount: %d\n", m_IspostUser.ui32IspostCount);

    LOGE("--------------------------------------------------------\n");
    LOGE("LC(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnLC, m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width, m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height);
    LOGE("OV0(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnOV, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Width, m_IspostUser.stOvInfo.stOvImgInfo[0].ui16Height);
    LOGE("DN(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnDN, m_IspostUser.stLcInfo.stOutImgInfo.ui16Width, m_IspostUser.stLcInfo.stOutImgInfo.ui16Height);
    LOGE("UO(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnUO, m_IspostUser.stLcInfo.stOutImgInfo.ui16Width, m_IspostUser.stLcInfo.stOutImgInfo.ui16Height);
    LOGE("SS0(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnSS0, m_IspostUser.stSS0Info.stOutImgInfo.ui16Width, m_IspostUser.stSS0Info.stOutImgInfo.ui16Height);
    LOGE("SS1(%d), W(%d) x H(%d)\n", m_IspostUser.stCtrl0.ui1EnSS1, m_IspostUser.stSS1Info.stOutImgInfo.ui16Width, m_IspostUser.stSS1Info.stOutImgInfo.ui16Height);

    LOGE("--------------------------------------------------------\n");
    LOGE("stLcInfo.stSrcImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui32YAddr);
    LOGE("stLcInfo.stSrcImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui32UVAddr);
    LOGE("stLcInfo.stSrcImgInfo.ui16Width: %d\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width);
    LOGE("stLcInfo.stSrcImgInfo.ui16Height: %d\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height);
    LOGE("stLcInfo.stSrcImgInfo.ui16Stride: %d\n", m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride);
    LOGE("stLcInfo.stBackFillColor.ui8Y: %d\n", m_IspostUser.stLcInfo.stBackFillColor.ui8Y);
    LOGE("stLcInfo.stBackFillColor.ui8CB: %d\n", m_IspostUser.stLcInfo.stBackFillColor.ui8CB);
    LOGE("stLcInfo.stBackFillColor.ui8CR: %d\n", m_IspostUser.stLcInfo.stBackFillColor.ui8CR);
    LOGE("stLcInfo.stGridBufInfo.ui32BufAddr: 0x%X\n", m_IspostUser.stLcInfo.stGridBufInfo.ui32BufAddr);
    LOGE("stLcInfo.stGridBufInfo.ui16Stride: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui16Stride);
    LOGE("stLcInfo.stGridBufInfo.ui8LineBufEnable: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui8LineBufEnable);
    LOGE("stLcInfo.stGridBufInfo.ui8Size: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui8Size);
    LOGE("stLcInfo.stGridBufInfo.ui32GridTargetIdx: %d\n", m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx);
    LOGE("stLcInfo.stPxlCacheMode.ui32PxlCacheMode: %d\n", m_IspostUser.stLcInfo.stPxlCacheMode.ui32PxlCacheMode);
    LOGE("stLcInfo.stPxlScanMode.ui8ScanW: %d\n", m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanW);
    LOGE("stLcInfo.stPxlScanMode.ui8ScanH: %d\n", m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanH);
    LOGE("stLcInfo.stPxlScanMode.ui8ScanM: %d\n", m_IspostUser.stLcInfo.stPxlScanMode.ui8ScanM);
    LOGE("stLcInfo.stOutImgInfo.ui16Width: %d\n", m_IspostUser.stLcInfo.stOutImgInfo.ui16Width);
    LOGE("stLcInfo.stOutImgInfo.ui16Height: %d\n", m_IspostUser.stLcInfo.stOutImgInfo.ui16Height);
    LOGE("stLcInfo.stAxiId.ui8SrcReqLmt: %d\n", m_IspostUser.stLcInfo.stAxiId.ui8SrcReqLmt);
    LOGE("stLcInfo.stAxiId.ui8GBID: %d\n", m_IspostUser.stLcInfo.stAxiId.ui8GBID);
    LOGE("stLcInfo.stAxiId.ui8SRCID: %d\n", m_IspostUser.stLcInfo.stAxiId.ui8SRCID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stOvInfo.ui8OverlayMode: %d\n", m_IspostUser.stOvInfo.stOverlayMode.ui32OvMode);
    for (ui8Idx = 0; ui8Idx < OVERLAY_CHNL_MAX; ui8Idx++)
    {
        LOGE("--------------------------------------------------------\n");
        LOGE("stOvInfo.ui8OvIdx[0]: %d\n", ui8Idx);
        LOGE("stOvInfo.stOvImgInfo[%d].ui32YAddr: 0x%X\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32YAddr);
        LOGE("stOvInfo.stOvImgInfo[%d].ui32UVAddr: 0x%X\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui32UVAddr);
        LOGE("stOvInfo.stOvImgInfo[%d].ui16Stride: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Stride);
        LOGE("stOvInfo.stOvImgInfo[%d].ui16Width: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Width);
        LOGE("stOvInfo.stOvImgInfo[%d].ui16Height: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvImgInfo[ui8Idx].ui16Height);
        LOGE("stOvInfo.stOvOffset[%d].ui16X: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16X);
        LOGE("stOvInfo.stOvOffset[%d].ui16Y: %d\n", ui8Idx, m_IspostUser.stOvInfo.stOvOffset[ui8Idx].ui16Y);
        LOGE("stOvInfo.stOvAlpha[%d].stValue0: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue0.ui32Alpha);
        LOGE("stOvInfo.stOvAlpha[%d].stValue1: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue1.ui32Alpha);
        LOGE("stOvInfo.stOvAlpha[%d].stValue2: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue2.ui32Alpha);
        LOGE("stOvInfo.stOvAlpha[%d].stValue3: 0x%08X\n", ui8Idx, m_IspostUser.stOvInfo.stOvAlpha[ui8Idx].stValue3.ui32Alpha);
        LOGE("stOvInfo.stAxiId[%d].ui8OVID: %d\n", ui8Idx, m_IspostUser.stOvInfo.stAxiId[ui8Idx].ui8OVID);
    }


    LOGE("--------------------------------------------------------\n");
    LOGE("stDnInfo.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stDnInfo.stOutImgInfo.ui32YAddr);
    LOGE("stDnInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stDnInfo.stOutImgInfo.ui32UVAddr);
    LOGE("stDnInfo.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stDnInfo.stOutImgInfo.ui16Stride);
    LOGE("stDnInfo.stRefImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stDnInfo.stRefImgInfo.ui32YAddr);
    LOGE("stDnInfo.stRefImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stDnInfo.stRefImgInfo.ui32UVAddr);
    LOGE("stDnInfo.stRefImgInfo.ui16Stride: %d\n", m_IspostUser.stDnInfo.stRefImgInfo.ui16Stride);
    LOGE("stDnInfo.ui32DnTargetIdx: %d\n", m_IspostUser.stDnInfo.ui32DnTargetIdx);
    LOGE("stDnInfo.stAxiId.ui8RefWrID: %d\n", m_IspostUser.stDnInfo.stAxiId.ui8RefWrID);
    LOGE("stDnInfo.stAxiId.ui8RefRdID: %d\n", m_IspostUser.stDnInfo.stAxiId.ui8RefRdID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stUoInfo.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stUoInfo.stOutImgInfo.ui32YAddr);
    LOGE("stUoInfo.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stUoInfo.stOutImgInfo.ui32UVAddr);
    LOGE("stUoInfo.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stUoInfo.stOutImgInfo.ui16Stride);
    LOGE("stUoInfo.ui8ScanH: %d\n", m_IspostUser.stUoInfo.ui8ScanH);
    LOGE("stUoInfo.stAxiId.ui8RefWrID: %d\n", m_IspostUser.stUoInfo.stAxiId.ui8RefWrID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stSS0Info.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stSS0Info.stOutImgInfo.ui32YAddr);
    LOGE("stSS0Info.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stSS0Info.stOutImgInfo.ui32UVAddr);
    LOGE("stSS0Info.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stSS0Info.stOutImgInfo.ui16Stride);
    LOGE("stSS0Info.stOutImgInfo.ui16Width: %d\n", m_IspostUser.stSS0Info.stOutImgInfo.ui16Width);
    LOGE("stSS0Info.stOutImgInfo.ui16Height: %d\n", m_IspostUser.stSS0Info.stOutImgInfo.ui16Height);
    LOGE("stSS0Info.stHSF.ui16SF: %d\n", m_IspostUser.stSS0Info.stHSF.ui16SF);
    LOGE("stSS0Info.stHSF.ui8SM: %d\n", m_IspostUser.stSS0Info.stHSF.ui8SM);
    LOGE("stSS0Info.stVSF.ui16SF: %d\n", m_IspostUser.stSS0Info.stVSF.ui16SF);
    LOGE("stSS0Info.stVSF.ui8SM: %d\n", m_IspostUser.stSS0Info.stVSF.ui8SM);
    LOGE("stSS0Info.stAxiId.ui8SSWID: %d\n", m_IspostUser.stSS0Info.stAxiId.ui8SSWID);

    LOGE("--------------------------------------------------------\n");
    LOGE("stSS1Info.stOutImgInfo.ui32YAddr: 0x%X\n", m_IspostUser.stSS1Info.stOutImgInfo.ui32YAddr);
    LOGE("stSS1Info.stOutImgInfo.ui32UVAddr: 0x%X\n", m_IspostUser.stSS1Info.stOutImgInfo.ui32UVAddr);
    LOGE("stSS1Info.stOutImgInfo.ui16Stride: %d\n", m_IspostUser.stSS1Info.stOutImgInfo.ui16Stride);
    LOGE("stSS1Info.stOutImgInfo.ui16Width: %d\n", m_IspostUser.stSS1Info.stOutImgInfo.ui16Width);
    LOGE("stSS1Info.stOutImgInfo.ui16Height: %d\n", m_IspostUser.stSS1Info.stOutImgInfo.ui16Height);
    LOGE("stSS1Info.stHSF.ui16SF: %d\n", m_IspostUser.stSS1Info.stHSF.ui16SF);
    LOGE("stSS1Info.stHSF.ui8SM: %d\n", m_IspostUser.stSS1Info.stHSF.ui8SM);
    LOGE("stSS1Info.stVSF.ui16SF: %d\n", m_IspostUser.stSS1Info.stVSF.ui16SF);
    LOGE("stSS1Info.stVSF.ui8SM: %d\n", m_IspostUser.stSS1Info.stVSF.ui8SM);
    LOGE("stSS1Info.stAxiId.ui8SSWID: %d\n", m_IspostUser.stSS1Info.stAxiId.ui8SSWID);
    LOGE("--------------------------------------------------------\n");
}

int IPU_ISPOSTV2::IspostSetLcGridTargetIndex(int Index)
{
    if (Index < 0)
    {
        return -1;
    }
    if (Index >= grid_target_index_max)
    {
        return -1;
    }
    return m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx
            = Index % grid_target_index_max;
}

int IPU_ISPOSTV2::IspostGetLcGridTargetIndex()
{
    return m_IspostUser.stLcInfo.stGridBufInfo.ui32GridTargetIdx;
}

int IPU_ISPOSTV2::IspostGetLcGridTargetCount()
{
    return grid_target_index_max;
}

int IPU_ISPOSTV2::IspostSet3dDnsCurve(const cam_3d_dns_attr_t *attr, int is_force)
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
            m_IspostUser.stDnInfo.stDnMskCrvInfo[i].ui8MskCrvIdx = DN_MSK_CRV_IDX_Y + i;
            for (j = 0; j < DN_MSK_CRV_DNMC_MAX; j++)
            {
                idx = 1 + i + i * DN_MSK_CRV_DNMC_MAX + j;
                m_IspostUser.stDnInfo.stDnMskCrvInfo[i].stMskCrv[j].ui32MskCrv = hermiteTable[idx].value;
            }
        }
        m_Dns3dAttr = *attr;
    }

    return ret;
}

int IPU_ISPOSTV2::IspostProcPrivateData(ipu_v2500_private_data_t *pdata)
{
    int ret = -1;
    cam_3d_dns_attr_t *pattr = NULL;
    cam_common_t *pcmn = NULL;
    ISPOST_UINT32 autoCtrlIdx = 0;
    ISPOST_UINT32 maxDnCnt = 0;
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
        maxDnCnt = sizeof(g_dnParamList) / sizeof(g_dnParamList[0]);
        memset(&autoDnsAttr, 0, sizeof(autoDnsAttr));

        autoCtrlIdx = pdata->iDnID;
        if (((m_IspostUser.stDnInfo.ui32DnTargetIdx != autoCtrlIdx) && (autoCtrlIdx < maxDnCnt)) ||
            (m_eDnsModePrev != pcmn->mode))
        {
            autoDnsAttr.y_threshold = g_dnParamList[autoCtrlIdx].y_threshold;
            autoDnsAttr.u_threshold = g_dnParamList[autoCtrlIdx].u_threshold;
            autoDnsAttr.v_threshold = g_dnParamList[autoCtrlIdx].v_threshold;
            autoDnsAttr.weight = g_dnParamList[autoCtrlIdx].weight;
            pattr = &autoDnsAttr;

            m_IspostUser.stDnInfo.ui32DnTargetIdx = autoCtrlIdx;
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

#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
int IPU_ISPOSTV2::IspostGetDnParamCount(void)
{
    return (int)(sizeof(g_dnParamList)/sizeof(ipu_ispost_3d_dn_param));
}

ipu_ispost_3d_dn_param* IPU_ISPOSTV2::IspostGetDnParamList(void)
{
    return g_dnParamList;
}

#endif //#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
#ifdef MACRO_IPU_ISPOSTV2_FCE
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

void IPU_ISPOSTV2::ParseFceDataParam(cam_fisheye_correction_t *pstFrom, TFceGenGridDataParam *pstTo)
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

int IPU_ISPOSTV2::IspostCreateAndFireFceData(cam_fcedata_param_t *pstParam)
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
        if (m_pPortUo && m_pPortUo->IsEnabled())
        {
            ret = SetFceDefaultPos(m_pPortUo, 1, &stMode.stPosUoList);
            if (ret)
            {
                LOGE("(%s:L%d) failed to set uo default pos !\n", __func__, __LINE__);
            }
        }
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

#endif //MACRO_IPU_ISPOSTV2_FCE

int IPU_ISPOSTV2::IspostLoadAndFireFceData(cam_fcefile_param_t *pstParam)
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

int IPU_ISPOSTV2::IspostSetFceMode(int stMode)
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

int IPU_ISPOSTV2::IspostGetSnapMaxSize(int &maxSize, int &modeID)
{
    int ret = -1;
    int size = 0;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;
    std::vector<ST_FCE_MODE>::iterator mit;

    maxSize = 0;
    modeID = 0;

    pthread_mutex_lock(&m_FceLock);
    it = m_stFce.stModeGroupAllList.begin();
    while (it != m_stFce.stModeGroupAllList.end())
    {
        mit = it->stModeAllList.begin();
        while(mit != it->stModeAllList.end())
        {
            size = mit->outW*mit->outH;
            LOGE("ModeID = %d intW=%d intH=%d outW=%d outW=%d OutSize=%d\n",
                mit->modeID, mit->inW, mit->inH, mit->outW, mit->outH, size);

            if (maxSize < size)
            {
                maxSize = size;
                modeID = mit->modeID;
            }
            mit++;
        }
        it++;
    }
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}


int IPU_ISPOSTV2::IspostSetFceModebyResolution(int inW, int inH, int outW, int outH)
{
    int ret = -1;
    int hasWork = 0;
    int maxMode = 0;
    int stMode = 0;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;
    std::vector<ST_FCE_MODE>::iterator mit;
    bool found = false;

    LOGE("(%s:L%d) intW=%d  intH=%d outW=%d outW=%d  \n",
        __func__, __LINE__, inW, inH, outW, outH);

    pthread_mutex_lock(&m_FceLock);
    it = m_stFce.stModeGroupAllList.begin();
    while (it != m_stFce.stModeGroupAllList.end())
    {
        mit = it->stModeAllList.begin();
        while(mit != it->stModeAllList.end())
        {
            LOGE("ModeID = %d intW=%d intH=%d outW=%d outW=%d  \n",
            mit->modeID, mit->inW, mit->inH, mit->outW, mit->outH);

            if (mit->inW == inW && mit->inH == inH &&
            mit->outW == outW && mit->outH == outH)
            {
                stMode = mit->modeID;
                found = true;
                break;
            }
            mit++;
        }

        if (found)
            break;
        it++;
    }

    hasWork = !m_stFce.modeGroupPendingList.empty();
    if (!hasWork && it != m_stFce.stModeGroupAllList.end())
    {
        it->modeActiveList.clear();
    }

    if (found)
    {
        maxMode = it->stModeAllList.size();
        if (stMode < maxMode)
        {
            LOGE("** ModeID = %d intW=%d intH=%d outW=%d outW=%d  \n",
                    mit->modeID, mit->inW, mit->inH, mit->outW, mit->outH);
            it->modeActiveList.push_back(stMode);
            ret = 0;
        }
    }
    else
    {
        LOGE("(%s:L%d) Can't find Mode (intW=%d intH=%d outW=%d outW=%d)\n",
            __func__, __LINE__, inW, inH, outW, outH);
    }
    pthread_mutex_unlock(&m_FceLock);

    return ret;
}


int IPU_ISPOSTV2::IspostGetFceMode(int *pMode)
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

int IPU_ISPOSTV2::IspostGetFceTotalModes(int *pModeCount)
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

int IPU_ISPOSTV2::IspostSaveFceData(cam_save_fcedata_param_t *pstParam)
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

int IPU_ISPOSTV2::UpdateFceModeGroup(ST_FCE *pstFce, ST_FCE_MODE_GROUP *pstGroup)
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

int IPU_ISPOSTV2::GetFceActiveMode(ST_FCE_MODE *pstMode)
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

int IPU_ISPOSTV2::UpdateFcePendingStatus()
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

int IPU_ISPOSTV2::ClearFceModeGroup(ST_FCE_MODE_GROUP *pstModeGroup)
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
        ST_EXT_BUF_INFO stExtBufInfo;


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

            nbBuf = modeIt->stExtBufList.size();
            LOGD("(%s:L%d)stExtBufList nbBuf=%d \n", __func__, __LINE__, nbBuf);
            for (i = 0; i < nbBuf; ++i)
            {
                stExtBufInfo = modeIt->stExtBufList.at(i);
                if (stExtBufInfo.stExtBuf.virt_addr)
                {
                    fr_free_single(stExtBufInfo.stExtBuf.virt_addr, stExtBufInfo.stExtBuf.phys_addr);
                    LOGD("(%s:L%d) free stExtBufInfo phys=0x%X \n", __func__, __LINE__, stExtBufInfo.stExtBuf.phys_addr);
                }
            }

            modeIt->stBufList.clear();
            modeIt->stExtBufList.clear();
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

int IPU_ISPOSTV2::ParseFceFileParam(ST_FCE_MODE_GROUP *pstModeGroup, int groupID, cam_fcefile_param_t *pstParam)
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
    ST_EXT_BUF_INFO stExtBufInfo;
    cam_position_t stPos;
    struct stat stStatBuf;
    char szFrBufName[50];
    char ExtBufName[50];
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
        stMode.stExtBufList.clear();
        stMode.stPosDnList.clear();
        stMode.stPosUoList.clear();
        stMode.stPosSs0List.clear();
        stMode.stPosSs1List.clear();

        pstParamMode = &pstParam->mode_list[i];
        nbFile = pstParamMode->file_number;

        LOGD("(%s:L%d) inW=%d inH=%d  outW=%d  outH=%d \n", __func__, __LINE__,
                pstParamMode->in_w , pstParamMode->in_h,
                pstParamMode->out_w, pstParamMode->out_h);

        stMode.inH = pstParamMode->in_h;
        stMode.inW = pstParamMode->in_w;
        stMode.outH = pstParamMode->out_h;
        stMode.outW = pstParamMode->out_w;

        LOGD("(%s:L%d) inW=%d inH=%d  outW=%d  outH=%d \n", __func__, __LINE__,
                stMode.inW , stMode.inH,  stMode.outW,  stMode.outH);

        LOGD("(%s:L%d) nbFile %d \n", __func__, __LINE__, nbFile);
        for (j = 0; j < nbFile; ++j)
        {
            memset(&stBufInfo, 0, sizeof(stBufInfo));
            pstParamFile = &pstParamMode->file_list[j];

#if defined(COMPILE_ISPOSTV2_IQ_TUNING_TOOL_APIS)
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
            if (pstParamFile->need_alloc_buf == 1) {
                memset(&stExtBufInfo, 0, sizeof(stExtBufInfo));
                stExtBufInfo.modeID = j;
                stExtBufInfo.x = pstParamFile->ov0_x;
                stExtBufInfo.y = pstParamFile->ov0_y;
                stExtBufInfo.width = pstParamFile->ov0_width;
                stExtBufInfo.height = pstParamFile->ov0_height;

                if (stExtBufInfo.width <= 64) {
                    stExtBufInfo.stride = 64;
                } else if (stExtBufInfo.width > 64) {
                    stExtBufInfo.stride = (stExtBufInfo.width + 31) / 32 * 32;
                }

                if (stExtBufInfo.modeID == 0) {
                    stExtBufInfo.overlayTiming = 1;
                } else if (stExtBufInfo.modeID == 2) {
                    stExtBufInfo.overlayTiming = 3;
                }

                stExtBufInfo.stExtBuf.size = stExtBufInfo.stride * stExtBufInfo.height * 3 / 2;
                stExtBufInfo.stExtBuf.map_size = stExtBufInfo.stride * stExtBufInfo.height * 3 / 2;
                sprintf(ExtBufName, "ExtBuf%d_%d_%d", groupID, i, j);
                stExtBufInfo.stExtBuf.virt_addr = fr_alloc_single(ExtBufName, stExtBufInfo.stExtBuf.size, &stExtBufInfo.stExtBuf.phys_addr);
                if (!stExtBufInfo.stExtBuf.virt_addr) {
                    LOGE("(%s:L%d) buf_name=%s \n", __func__, __LINE__, ExtBufName);
                    goto allocFail;
                }

                stMode.stExtBufList.push_back(stExtBufInfo);
            }

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
            if (m_pPortUo && m_pPortUo->IsEnabled())
            {
                ret = SetFceDefaultPos(m_pPortUo, nbFile, &stMode.stPosUoList);
                if (ret)
                {
                    LOGE("(%s:L%d) failed to set default pos(nbFile=%d) !\n", __func__, __LINE__, nbFile);
                    goto defaultPosFail;
                }
            }
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

void IPU_ISPOSTV2::FreeFce()
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

void IPU_ISPOSTV2::PrintAllFce(ST_FCE *pstFce, const char *pszFileName, int line)
{
    int nbBuf = 0;
    int nbPort = 0;
    int i = 0;
    int j = 0;
    std::vector<ST_FCE_MODE_GROUP>::iterator it;
    std::vector<ST_FCE_MODE>::iterator modeIt;
    std::list<int>::iterator intIt;
    ST_GRIDDATA_BUF_INFO stBufInfo;
    ST_EXT_BUF_INFO stExtBufInfo;
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
            nbBuf = modeIt->stExtBufList.size();
            LOGE("modeID=%d file:%d =======\n", modeIt->modeID, nbBuf);
            for (i = 0; i < nbBuf; ++i)
            {
                stExtBufInfo = modeIt->stExtBufList.at(i);
                LOGE("modeID=%d, extbuf phys_addr=0x%X \n",
                stExtBufInfo.modeID, stExtBufInfo.stExtBuf.phys_addr);
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

int IPU_ISPOSTV2::SetFceDefaultPos(Port *pPort, int fileNumber, std::vector<cam_position_t> *pPosList)
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

int IPU_ISPOSTV2::ReturnBuf(Buffer *pstBuf)
{
    Buffer stBuf;
    std::list<struct fr_buf_info>::reverse_iterator rit;


    if (NULL == pstBuf)
    {
        LOGE("(%s:L%d) param is NULL !!!\n", __func__, __LINE__);
        return -1;
    }
    pthread_mutex_lock(&m_BufLock);
    if (m_stInBufList.empty())
    {
        pthread_mutex_unlock(&m_BufLock);
        return 0;
    }


    for (rit = m_stInBufList.rbegin(); rit != m_stInBufList.rend(); )
    {
        stBuf.fr_buf = *rit;
        if (rit->phys_addr == pstBuf->fr_buf.phys_addr)
        {
            m_RefBufSerial = rit->serial;
            m_pPortIn->PutBuffer(&stBuf);
            rit = std::list<struct fr_buf_info>::reverse_iterator(m_stInBufList.erase((++rit).base()));
        }
        else if (rit->serial < m_RefBufSerial)
        {
            m_pPortIn->PutBuffer(&stBuf);
            rit = std::list<struct fr_buf_info>::reverse_iterator(m_stInBufList.erase((++rit).base()));
        }
        else
        {
            ++rit;
        }
    }
    pthread_mutex_unlock(&m_BufLock);

    return 0;
}

int IPU_ISPOSTV2::ReturnSnapBuf(Buffer *pstBuf)
{
    LOGD("-------%s-----\n", __func__);
    if (NULL == pstBuf)
    {
        LOGE("(%s:L%d) param is NULL !!!\n", __func__, __LINE__);
        return -1;
    }

    if (m_SnapMode && m_SnapBufMode)
    {
        pthread_mutex_lock(&m_SnapBufLock);
        if (pstBuf->fr_buf.phys_addr == m_SnapUoBuf.fr_buf.phys_addr)
        {
            LOGE("#fr_free_single vir_addr = %p phy_addr =0x%x\n",
                m_SnapUoBuf.fr_buf.virt_addr, m_SnapUoBuf.fr_buf.phys_addr);
            fr_free_single(m_SnapUoBuf.fr_buf.virt_addr, m_SnapUoBuf.fr_buf.phys_addr);

            m_SnapUoBuf.fr_buf.virt_addr = 0;
            m_SnapUoBuf.fr_buf.phys_addr = 0;
            m_SnapUoBuf.fr_buf.size = 0;
            m_SnapUoBuf.fr_buf.map_size = 0;
        }
        pthread_mutex_unlock(&m_SnapBufLock);
    }

    return 0;
}

int IPU_ISPOSTV2::ParseFceFileJson(char *fileName, cam_fcefile_param_t *pstParam)
{
    Json js;
    int i = 0;
    int j = 0;
    int k = 0;
    char strTag[50];
    int modeNumber = 0;
    int fileNumber = 0;
    int portNumber = 0;
    int need_alloc_buf = 0;
    const char *pszString = NULL;
    cam_fcefile_mode_t *pstMode = NULL;
    cam_fcefile_file_t *pstFile = NULL;
    cam_fcefile_port_t *pstPort = NULL;

    char strRes[32] = {0};
    const char delim = 'x';
    char *str, *token, *saveptr;

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

            sprintf(strTag, "in_res");
            pszString = mode_js->GetString(strTag);
            if (pszString)
            {
                snprintf(strRes, sizeof(strRes), "%s",pszString);
                LOGD("(%s:L%d) %s=%s\n", __func__, __LINE__, strTag, strRes);

                str = strRes;
                saveptr = NULL;
                token = strtok_r(str, &delim, &saveptr);
                pstMode->in_w = atoi(token);

                token = strtok_r(NULL, &delim, &saveptr);
                pstMode->in_h = atoi(token);
                LOGD("(%s:L%d) %s=%s in %dx%d\n", __func__, __LINE__,
                    strTag, strRes, pstMode->in_w, pstMode->in_h);
            }

            sprintf(strTag, "out_res");
            pszString = mode_js->GetString(strTag);
            if (pszString)
            {
                snprintf(strRes, sizeof(strRes), "%s",pszString);
                LOGD("(%s:L%d) %s=%s\n", __func__, __LINE__, strTag, strRes);
                str = strRes;
                saveptr = NULL;
                token = strtok_r(str, &delim, &saveptr);
                pstMode->out_w = atoi(token);

                token = strtok_r(NULL, &delim, &saveptr);
                pstMode->out_h = atoi(token);
                LOGD("(%s:L%d) %s=%s out %dx%d\n", __func__, __LINE__,
                    strTag, strRes, pstMode->out_w, pstMode->out_h);
            }

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

                    sprintf(strTag, "need_alloc_buf");
                    need_alloc_buf = file_js->GetObject(strTag) ? file_js->GetInt(strTag):0;
                    pstFile->need_alloc_buf = need_alloc_buf;

                    sprintf(strTag, "ov_stitch");
                    pstFile->ov_stitch = file_js->GetObject(strTag) ? file_js->GetInt(strTag):0;

                    sprintf(strTag, "ov0_x");
                    pstFile->ov0_x = file_js->GetObject(strTag) ? file_js->GetInt(strTag):0;

                    sprintf(strTag, "ov0_y");
                    pstFile->ov0_y = file_js->GetObject(strTag) ? file_js->GetInt(strTag):0;

                    sprintf(strTag, "ov0_width");
                    pstFile->ov0_width = file_js->GetObject(strTag) ? file_js->GetInt(strTag):0;

                    sprintf(strTag, "ov0_height");
                    pstFile->ov0_height = file_js->GetObject(strTag) ? file_js->GetInt(strTag):0;

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

int IPU_ISPOSTV2::SetupLcInfo()
{
    // IN ----------------------------------------
    if (m_pPortIn->IsEnabled())
    {
        if (m_IsLcManual && m_LcManualVal == 0) {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stVsInfo.stSrcImgInfo.ui16Width;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stVsInfo.stSrcImgInfo.ui16Height;
        } else {
            m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width = m_pPortIn->GetWidth();
            m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height = m_pPortIn->GetHeight();
            m_IspostUser.stLcInfo.stSrcImgInfo.ui16Stride = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Width;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stSrcImgInfo.ui16Height;
            if (m_Rotate) {
                m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
                m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
                m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            }
        }
    }


    if (m_pPortDn->IsEnabled() && (m_DnManualVal || !m_IsDnManual))
    {
        if (m_pPortDn->IsEmbezzling())
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_pPortDn->GetPipWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_pPortDn->GetPipHeight();
        }
        else
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_pPortDn->GetWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_pPortDn->GetHeight();
        }

        if (m_Rotate)
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
        }
    }
    // UO ----------------------------------------
    else if (m_pPortUo->IsEnabled())
    {
        if (m_pPortUo->IsEmbezzling())
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_pPortUo->GetPipWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_pPortUo->GetPipHeight();
        }
        else
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_pPortUo->GetWidth();
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_pPortUo->GetHeight();
        }

        if (m_Rotate) {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
        }
    }
    else if ((m_s32LcOutImgWidth != 0) && (m_s32LcOutImgHeight != 0))
    {
        m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_s32LcOutImgWidth;
        m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_s32LcOutImgHeight;

        if (m_Rotate)
        {
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Height = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
            m_IspostUser.stLcInfo.stOutImgInfo.ui16Width = m_IspostUser.stLcInfo.stOutImgInfo.ui16Width ^ m_IspostUser.stLcInfo.stOutImgInfo.ui16Height;
        }
    }
    else
    {
        LOGE("Error:no Lc out image params");
        return -1;
    }


    return 0;
}
