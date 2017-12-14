/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: VEncode IPU
 *
 * Author:
 *     beca.zhang <beca.zhang@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/12/2017 init
 */
#include <System.h>
#include "VEncoder.h"

#define BITRATE_MIN 10000
#define BITRATE_MAX (60000000)

//vbr sensitivity factor
#define MIN_QUALITY         (double)2.0/52.0
#define AVG_FRAME_FACTOR    (double)4.0
#define SEQ_QUALITY_FACTOR  (double)100.0
#define DEVIATION_FACTOR    (double)10.0
#define THRESHOLD_FACTOR    (double)4.0

#define H2_CTB_SIZE   64
#define H1_CTB_SIZE   16
#define HEVCENC_MAX_FRAME_SIZE_NUM 7

DYNAMIC_IPU(IPU_VENCODER, "vencoder");

std::mutex *IPU_VENCODER::ms_pMutexHwLock = NULL;
int IPU_VENCODER::ms_s32InstanceCounter = 0;
bool IPU_VENCODER::ms_bSmartrcStarted = false;

#ifdef VENCODER_DEPEND_H2V4
//FrameN Type POC QPoffset QPfactor TemporalID num_ref_pics ref_pics used_by_cur
const char *VGopCfgDefault_GOPSize_1[] =
{
    "Frame1:  P    1   0        0.8       0     1       -1      1",
    NULL,
};

const char *VGopCfgDefault_GOPSize_2[] =
{
    "Frame1:  P    1   0        0.4624    1     1       -1      1",
    "Frame2:  P    2   0        0.4624    0     1       -2      1",
    NULL,
};

const char *VGopCfgDefault_GOPSize_4[] =
{
    "Frame1:  P    1   0        0.4624    2     1       -1          1",
    "Frame2:  P    2   0        0.4624    1     1       -2          1",
    "Frame3:  P    3   0        0.4624    2     2       -1 -3       1 0",
    "Frame4:  P    4   0        0.578     0     1       -4          1",
    NULL,
};
#endif

void IPU_VENCODER::WorkLoop()
{
    Buffer bfin, bfout;
    Port *pin, *pout;

    pin = GetPort("frame");
    pout = GetPort("stream");

    bool bNeedNewInFrFlag = true;
    bool bNeedNewOutFrFlag = true;
    int s32FpsRatio = 1;
    int s32Ret;
    int s32OutFrSize;
#ifdef VENCODER_DEPEND_H1V6
    m_s32FrameCnt = 0;
    m_s32RefreshCnt = 0;
#endif

    m_s32Infps = pin->GetFPS();
    if (m_pEncode->IsInstReady(&m_unEncode) == false)
    {
        if (EncodeInstanceInit() != VENCODER_RET_OK)
        {
            LOGE("init vencoder error\n");
            throw VBERR_BADPARAM;
        }
        else
        {
            LOGD("init vencoder inst ok\n");
        }
    }

    if (EncodeSetPreProcessCfg() != VENCODER_RET_OK)
    {
        LOGE("encode start set pre process cfg error \n");
        throw VBERR_BADPARAM;
    }
    if (EncodeSetCodingCfg() != VENCODER_RET_OK)
    {
        LOGE("encode start set coding cfg error \n");
        throw VBERR_BADPARAM;
    }
    if (EncodeSetRateCtrl() != VENCODER_RET_OK)
    {
        LOGE("encode start set rc error \n");
        throw VBERR_BADPARAM;
    }

    bfout = pout->GetBuffer();
    if (EncodeStartStream(bfout) != VENCODER_RET_OK)
    {
        LOGE("encode start steam error \n");
        throw VBERR_RUNTIME;
    }
    else
    {
        m_bIsHeaderReady = true;
    }

    SaveHeader((char *)bfout.fr_buf.virt_addr, bfout.fr_buf.size);
    pout->PutBuffer(&bfout);
    m_stEncIn.poc = 0;

    while (IsInState(ST::Running))
    {
        m_stVencInfo.enState = RUN;
        try
        {
            m_pMutexSettingLock->lock();
            if (m_s32InFpsRatio == -1)
            {
                CheckFrcUpdate();
            }
            s32FpsRatio = m_s32InFpsRatio;
            m_pMutexSettingLock->unlock();
#ifdef VENCODER_DEPEND_H2V4
            if (!m_bIsH264Lib)
            {
                CheckCtbRcUpdate();
            }
#endif
            if (s32FpsRatio > 0)
            {
                bfin = pin->GetBuffer(&bfin);
                m_stVencInfo.u64InCount++;
                bNeedNewInFrFlag = false;
                for (int i = 0; i < s32FpsRatio; i++)
                {
                    if (IsInState(ST::Running) == false)
                    {
                        LOGD("frc %d exit\n", s32FpsRatio);
                        break;
                    }
                    if (bNeedNewOutFrFlag)
                    {
                        bfout = pout->GetBuffer(&bfout);
                        bNeedNewOutFrFlag = false;
                    }
                    pin->LinkReady();
                    s32OutFrSize = bfout.fr_buf.size;
                    s32Ret = EncodeStrmEncode(bfin, bfout);
                    if (s32Ret != VENCODER_RET_OK)
                    {
                        bfout.fr_buf.size = s32OutFrSize;
                        bNeedNewOutFrFlag = false;
                        m_stVencInfo.u32FrameDrop++;
                        break;
                    }
                    bfout.fr_buf.timestamp = bfin.fr_buf.timestamp
                        + m_s32FrameDuration * i;
                    if (m_stVencRateCtrl.rc_mode == VENC_VBR_MODE)
                    {
                        VbrProcess(&bfout);
                    }
                    VideoInfo(&bfout);
                    if (bfout.fr_buf.size <= 0)
                    {
                        bNeedNewOutFrFlag = false;
                        bfout.fr_buf.size = s32OutFrSize;
                        m_stVencInfo.u32FrameDrop++;
                        LOGD("encoder may skip & output size 0\n");
                    }
                    else
                    {
                        pout->PutBuffer(&bfout);
                        m_stVencInfo.u64OutCount++;
                        bNeedNewOutFrFlag = true;
                    }
                }
                pin->PutBuffer(&bfin);
                bNeedNewInFrFlag = true;
            }
            else
            {
                s32FpsRatio = 0 - s32FpsRatio;
                for (int i = 0; i < s32FpsRatio; i++)
                {
                    if (IsInState(ST::Running) == false)
                    {
                        LOGD("frc -%d exit\n", s32FpsRatio);
                        break;
                    }
                    m_pMutexSettingLock->lock();
                    if (m_s32InFpsRatio == -1)
                    {
                        m_pMutexSettingLock->unlock();
                        LOGD("FPS change, break now.\n");
                        break;
                    }
                    m_pMutexSettingLock->unlock();
                    bfin = pin->GetBuffer(&bfin);
                    m_stVencInfo.u64InCount++;
                    bNeedNewInFrFlag = false;
                    if (i == 0)
                    {
                        if (bNeedNewOutFrFlag)
                        {
                            bfout = pout->GetBuffer(&bfout);
                            bNeedNewOutFrFlag = false;
                        }

                        s32OutFrSize = bfout.fr_buf.size;
                        s32Ret = EncodeStrmEncode(bfin, bfout);
                        if (s32Ret != VENCODER_RET_OK)
                        {
                            LOGE("EncodeStrmEncode failed.\n");
                            bNeedNewOutFrFlag = false;
                            pin->PutBuffer(&bfin);
                            bNeedNewInFrFlag = true;
                            bfout.fr_buf.size = s32OutFrSize;
                            m_stVencInfo.u32FrameDrop++;
                            break;
                        }

                        bfout.SyncStamp(&bfin);
                        if (m_stVencRateCtrl.rc_mode == VENC_VBR_MODE)
                        {
                            VbrProcess(&bfout);
                        }
                        VideoInfo(&bfout);
                        if (bfout.fr_buf.size <= 0)
                        {
                            bNeedNewOutFrFlag = false;
                            bfout.fr_buf.size = s32OutFrSize;
                            m_stVencInfo.u32FrameDrop++;
                            LOGD("encoder may skip & output size 0\n");
                        }
                        else
                        {
                            pout->PutBuffer(&bfout);
                            m_stVencInfo.u64OutCount++;
                            bNeedNewOutFrFlag = true;
                        }
                        pin->PutBuffer(&bfin);
                        bNeedNewInFrFlag = true;
                    }
                    else
                    {
                        if (m_bTriggerKeyFrameFlag)
                        {
                            LOGE("Begin encode key frame\n");
                            pout->PutBuffer(&bfout);
                            m_stVencInfo.u64OutCount++;
                            if (!bNeedNewInFrFlag)
                            {
                                pin->PutBuffer(&bfin);
                            }
                            bNeedNewInFrFlag = true;
                            break;
                        }
                        pin->PutBuffer(&bfin);
                        bNeedNewInFrFlag = true;
                    }
                }
            }
        }
        catch (const char* err)
        {
            //LOGE("exception buf state -> in:%d out:%d\n", bNeedNewInFrFlag,bNeedNewOutFrFlag);
            m_stVencInfo.enState = IDLE;
            if (IsError(err, VBERR_FR_INTERRUPTED))
            {
                LOGE("videobox interrupted out 1\n");
                break;
            }
            else if (IsError(err, VBERR_FR_NOBUFFER))
            {
                //LOGE("wait for buffer\n");
                usleep(5000);
            }
            else
            {
                //LOGE("exception -> %s\n", err);
            }
            if (bNeedNewInFrFlag == false)
            {
                pin->PutBuffer(&bfin);
                bNeedNewInFrFlag = true;
            }
        }
        //When threads use SCHED_RR to schedule, need sleep to let other encode
        //thread to get mutex lock. Otherwise, other encode thread will block by
        //cann't get lock.
        UpdateDebugInfo(1);
        usleep(1000);
    }
    m_stVencInfo.enState = STOP;
    UpdateDebugInfo(0);
    if (bNeedNewInFrFlag == false)
    {
        pin->PutBuffer(&bfin);
        bNeedNewInFrFlag = true;
    }
    if (bNeedNewOutFrFlag == false)
    {
        LOGE("put dummy frame for last error\n");
        bfout.fr_buf.size = 4;
        bfout.fr_buf.priv = VIDEO_FRAME_DUMMY;
        pout->PutBuffer(&bfout);
        m_stVencInfo.u64OutCount++;
        m_stVencInfo.u32FrameDrop++;

    }
    bNeedNewInFrFlag = true;
    bNeedNewOutFrFlag = true;
    if (EncodeInstanceRelease() != VENCODER_RET_OK)
    {
        LOGE("release vencoder instance error\n");
        throw VBERR_RUNTIME;
    }
}

IPU_VENCODER::IPU_VENCODER(std::string name, Json *js)
{
    Name = name;
    Port *pout, *pin;
    int s32MinBufNum = 3;
    char s8Rotate[8];
    int s32BufSize;
#ifdef VENCODER_DEPEND_H1V6
    Port *pmv;
    uint32_t u32MvWidth, u32MvHeight;
#endif
    int s32Value = 0;
    bool s32RateCtrlSet = false;
    memset(&m_stRateCtrlInfo, 0, sizeof(ST_VIDEO_RATE_CTRL_INFO));
    memset(&m_unEncode, 0, sizeof(U_ENCODE));
    memset(&m_stVencInfo, 0, sizeof(ST_VENC_INFO));
    memset(&m_stVencPpCfg, 0, sizeof(ST_PREPROCESS));

    if (js != NULL)
    {
        if (!strcmp(js->GetString("encode_type"), "h264"))
        {
#ifndef VENCODER_DEPEND_H1V6
            LOGE("func:%s line:%d IPU vencoder enable h264  but There is no H264 lib!\n",__FUNCTION__,__LINE__);
            usleep(100*1000);
            throw VBERR_BADPARAM;
#endif
            m_pEncode = new dH264();
            m_bIsH264Lib = true;
        }
        else if (!strcmp(js->GetString("encode_type"), "h265"))
        {
#if !(defined VENCODER_DEPEND_H2V1) && !(defined VENCODER_DEPEND_H2V4)
            LOGE("func:%s line:%d IPU vencoder enable h265  but There is no H265 lib!\n",__FUNCTION__,__LINE__);
            usleep(100*1000);
            throw VBERR_BADPARAM;
#endif
            m_pEncode = new dH265();
            m_bIsH264Lib = false;
        }
        else
        {
            LOGE("func:%s line:%d IPU vencoder must set  encode_type(h264 or h265) in json file!\n",__FUNCTION__,__LINE__);
            usleep(100*1000);
            throw VBERR_BADPARAM;
        }
    }
    else
    {
        LOGE("func:%s line:%d IPU vencoder must set  encode_type(h264 or h265) in json file!\n",__FUNCTION__,__LINE__);
        usleep(100*1000);
        throw VBERR_BADPARAM;
    }

    if (m_bIsH264Lib)
    {
        m_s32ChromaQpOffset = 2;
        s32BufSize = 0x240000;
    }
    else
    {
        s32BufSize = 0x300000;
        m_s32ChromaQpOffset = 0;
    }
    m_s32RefMode = E_GOPCFG1;
    if (NULL != js)
    {
        if (NULL != js->GetString("rotate"))
        {
            strncpy(s8Rotate, js->GetString("rotate"), 8);
            if (NULL != s8Rotate)
            {
                if (!strncmp(s8Rotate, "90R", 3))
                {
                    m_stVencPpCfg.u32Rotation = 1; //90R
                }
                else if (!strncmp(s8Rotate, "90L", 3))
                {
                    m_stVencPpCfg.u32Rotation = 2; //90L
                }
                else
                {
                    LOGE("Rotate Param Error Reset Default No Rotate\n");
                }
            }
        }
        if (js->GetObject("chroma_qp_offset"))
        {
            m_s32ChromaQpOffset = js->GetInt("chroma_qp_offset");
        }
        if (js->GetInt("min_buffernum") >=3 ) //h2v4 also support
        {
            s32MinBufNum = js->GetInt("min_buffernum");
        }
#ifdef VENCODER_DEPEND_H1V6
        if (m_bIsH264Lib)
        {
            u32MvWidth = js->GetInt("mvw");
            u32MvHeight = js->GetInt("mvh");
            if (!u32MvWidth)
            {
                u32MvWidth = 1920;
            }
            if (!u32MvHeight)
            {
                u32MvHeight = 1088;
            }
        }
#endif
        if (0 != js->GetInt("buffer_size"))
        {
            s32BufSize = js->GetInt("buffer_size");
        }
        int s32FrameRate = js->GetInt("framerate");
        int s32FrameBase = js->GetInt("framebase");
        if (s32FrameRate < 0 || s32FrameBase < 0)
        {
            LOGE("please check framerate and framebase in ipu arg, can not be\
                    negative\n");
            assert(1);
        }
        else if (s32FrameRate * s32FrameBase == 0)
        {
            if (s32FrameRate > 0 || s32FrameBase > 0)
            {
                LOGE("please set framerate and framebase in ipu arg\n");
                assert(1);
            }
            else
            {
                LOGE("ipu arg no frc data, use default value\n");
                m_stFrcInfo.framerate = 0;
                m_stFrcInfo.framebase = 1;
                m_s32InFpsRatio = -1;
            }
        }
        else
        {
            m_stFrcInfo.framerate = s32FrameRate;
            m_stFrcInfo.framebase = s32FrameBase;
            m_s32InFpsRatio = -1;
        }
#ifdef VENCODER_DEPEND_H1V6
        if (m_bIsH264Lib)
        {
            m_s32EnableLongterm = js->GetInt("enable_longterm");
            // refmode is conflict with SmaptP, But SmartP with Higher Priority
            if (m_bIsH264Lib)
            {
                m_s32RefMode = m_s32EnableLongterm ? 0 : js->GetInt("refmode");
            }
        }
#endif
#ifdef VENCODER_DEPEND_H2V4
        if (!m_bIsH264Lib)
        {
            m_s32RefMode = js->GetInt("refmode");
        }

        m_stRateCtrlInfo.mbrc = 1;
#endif

        LOGE("frc value value ->%d %d\n", m_stFrcInfo.framerate,
                m_stFrcInfo.framebase);
        m_s32Smartrc = 0;
        if ( 1 == js->GetInt("smartrc") )
        {
            if ( 0 == access("/root/.videobox/rc.json", F_OK) )
            {
                m_s32Smartrc = 1;
                LOGE("Smartrc:%d\n", m_s32Smartrc);
                if (!ms_bSmartrcStarted)
                {
                    system("smartrc &");
                    ms_bSmartrcStarted = true;
                }
            }
            else
            {
                LOGE("Smartrc enabled, but no rc.json file, please add\n");
            }
        }
        LOGE("Smartrc:%d \n", m_s32Smartrc);

        if (js->GetObject("rc_mode"))
        {
            s32RateCtrlSet = true;
            m_stRateCtrlInfo.rc_mode =
                (enum v_rate_ctrl_type)js->GetInt("rc_mode");
            s32Value = js->GetInt("gop_length");
            if (s32Value > 0)
            {
                m_stRateCtrlInfo.gop_length = s32Value;
            }
            else
            {
#ifdef VENCODER_DEPEND_H1V6
                if (m_bIsH264Lib)
                {
                     m_stRateCtrlInfo.gop_length = m_s32EnableLongterm != 0 ? 150 : 15;
                }
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
                if (!m_bIsH264Lib)
                {
                    m_stRateCtrlInfo.gop_length = 15;
                }
#endif
            }
            s32Value = js->GetInt("idr_interval");
            if (s32Value > 0)
            {
                m_stRateCtrlInfo.idr_interval = s32Value;
            }
            else
            {
#ifdef VENCODER_DEPEND_H1V6
                if (m_bIsH264Lib)
                {
                    m_stRateCtrlInfo.idr_interval =
                        m_s32EnableLongterm != 0 ? 150 : 15;
                }
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
                if (!m_bIsH264Lib)
                {
                    m_stRateCtrlInfo.idr_interval = 15;
                }
#endif
            }

            if (VENC_CBR_MODE == m_stRateCtrlInfo.rc_mode)
            {
                s32Value = js->GetInt("qp_max");
                m_stRateCtrlInfo.cbr.qp_max = s32Value > 0 ? s32Value : 42;
                s32Value = js->GetInt("qp_min");
                m_stRateCtrlInfo.cbr.qp_min = s32Value > 0 ? s32Value : 26;
                s32Value = js->GetInt("bitrate");
                m_stRateCtrlInfo.cbr.bitrate = s32Value > 0 ? s32Value
                    : 600 * 1024;
                s32Value = js->GetInt("qp_delta");
                m_stRateCtrlInfo.cbr.qp_delta = s32Value != 0 ? s32Value : -1;
                s32Value = js->GetInt("qp_hdr");
                m_stRateCtrlInfo.cbr.qp_hdr = s32Value != 0 ? s32Value : -1;
            }
            else if (VENC_VBR_MODE == m_stRateCtrlInfo.rc_mode)
            {
                s32Value = js->GetInt("qp_max");
                m_stRateCtrlInfo.vbr.qp_max = s32Value > 0 ? s32Value : 42;
                s32Value = js->GetInt("qp_min");
                m_stRateCtrlInfo.vbr.qp_min = s32Value > 0 ? s32Value : 26;
                s32Value = js->GetInt("max_bitrate");
                m_stRateCtrlInfo.vbr.max_bitrate = s32Value > 0 ? s32Value
                    : 2 * 1024 * 1024;
                s32Value = js->GetInt("qp_delta");
                m_stRateCtrlInfo.vbr.qp_delta = s32Value != 0 ? s32Value : -1;
                s32Value = js->GetInt("threshold");
                m_stRateCtrlInfo.vbr.threshold = s32Value > 0 ? s32Value : 80;
            }
            else if (VENC_FIXQP_MODE == m_stRateCtrlInfo.rc_mode)
            {
                s32Value = js->GetInt("qp_fix");
                m_stRateCtrlInfo.fixqp.qp_fix = s32Value > 0 ? s32Value : 35;
            }
        }
    }
    else
    {
        LOGE("ipu arg no frc data, use default value\n");
        m_stFrcInfo.framerate = 0;
        m_stFrcInfo.framebase = 1;
        m_s32InFpsRatio = -1;
#ifdef VENCODER_DEPEND_H1V6
        m_s32EnableLongterm = 0;
#endif
        m_s32Smartrc = 0;
    }

    m_stRateCtrlInfo.mb_qp_adjustment = -1;
    if (false == s32RateCtrlSet)
    {
        m_stRateCtrlInfo.rc_mode = VENC_CBR_MODE;
        m_stRateCtrlInfo.gop_length = 15;
        m_stRateCtrlInfo.idr_interval = 15;
#ifdef VENCODER_DEPEND_H1V6
        if (m_bIsH264Lib)
        {
            m_stRateCtrlInfo.gop_length = m_s32EnableLongterm != 0 ? 150 : 15;
            m_stRateCtrlInfo.idr_interval = m_s32EnableLongterm != 0 ? 150 : 15;
        }
#endif
        m_stRateCtrlInfo.cbr.qp_max = 42;
        m_stRateCtrlInfo.cbr.qp_min = 26;
        m_stRateCtrlInfo.cbr.bitrate = 600 * 1024;
        m_stRateCtrlInfo.cbr.qp_delta = -1;
        m_stRateCtrlInfo.cbr.qp_hdr = -1;
    }

    LOGE("RateCtrl rc_mode:%d gop_length:%d idr_interval:%d qp_max:%d qp_min:%d\
            bitrate:%d qp_delta:%d qp_hdr:%d\n",
            m_stRateCtrlInfo.rc_mode,
            m_stRateCtrlInfo.gop_length,
            m_stRateCtrlInfo.idr_interval,
            m_stRateCtrlInfo.cbr.qp_max,
            m_stRateCtrlInfo.cbr.qp_min,
            m_stRateCtrlInfo.cbr.bitrate,
            m_stRateCtrlInfo.cbr.qp_delta,
            m_stRateCtrlInfo.cbr.qp_hdr
        );

    if (ms_pMutexHwLock == NULL)
    {
        ms_pMutexHwLock = new std::mutex();
    }
    m_pMutexSettingLock = new std::mutex();
    ms_s32InstanceCounter++;
    m_ps8Header = NULL;
    m_s32HeaderLength = 0;

    pin = CreatePort("frame", Port::In);
    pin->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
    pin->SetPixelFormat(Pixel::Format::NV12);

    pout = CreatePort("stream", Port::Out);
    pout->SetBufferType(FRBuffer::Type::FLOAT, s32BufSize,
            s32BufSize / s32MinBufNum - 0x4000);
    if(m_bIsH264Lib)
    {
        pout->SetPixelFormat(Pixel::Format::H264ES);
    }
    else
    {
        pout->SetPixelFormat(Pixel::Format::H265ES);
    }

    // This port is most likely not to be bind,
    // So export it here as to enable buffer allocation
    pout->Export();

    m_u64MaxBitRate = 2 * 1024 * 1024;
    m_u32VbrThreshold = 80;
    m_u32VbrPicSkip = 0;
    BasicParaInit();
    m_s32CurFps = 0;
    m_bTriggerKeyFrameFlag = true;

#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        pmv = CreatePort("mv", Port::Out);
        pmv->SetBufferType(FRBuffer::Type::VACANT, 3);
        pmv->SetResolution(u32MvWidth, u32MvHeight);
        pmv->SetPixelFormat(Pixel::Format::MV);
    }
#endif
    memset(&m_stVideoScenarioBuffer, 0, sizeof(struct fr_buf_info));
    m_enVideoScenarioMode = VIDEO_DAY_MODE;
    m_bSEIUserDataSetFlag = false;
    m_enSEITriggerMode = EN_SEI_TRIGGER_ONCE;
}

IPU_VENCODER::~IPU_VENCODER()
{
    delete m_pMutexSettingLock;
    if (m_pstSei != NULL)
    {
        free(m_pstSei);
        m_pstSei = NULL;
    }
    if (ms_s32InstanceCounter <= 0)
    {
        return;
    }
    ms_s32InstanceCounter--;
    if (ms_s32InstanceCounter == 0)
    {
        delete ms_pMutexHwLock;
    }
}

void IPU_VENCODER::Prepare()
{
    Port *portIn = GetPort("frame");
    Pixel::Format enPixelFormat = portIn->GetPixelFormat();
    Buffer src, dst;

    if(enPixelFormat != Pixel::Format::NV12
            && enPixelFormat != Pixel::Format::NV21
            && enPixelFormat != Pixel::Format::RGBA8888
            && enPixelFormat != Pixel::Format::RGB565
            && enPixelFormat != Pixel::Format::YUYV422)
    {
        LOGE("input format param error\n");
        throw VBERR_BADPARAM;
    }
    if((m_bIsH264Lib && GetPort("stream")->GetPixelFormat() != Pixel::Format::H264ES)
            || (!m_bIsH264Lib && GetPort("stream")->GetPixelFormat() != Pixel::Format::H265ES)
#ifdef VENCODER_DEPEND_H1V6
        || (m_bIsH264Lib && GetPort("mv")->GetPixelFormat() != Pixel::Format::MV)
#endif
        )
    {
        LOGE("output format param error\n");
        throw VBERR_BADPARAM;
    }

    if (m_ps8Header == NULL)
    {
        m_ps8Header = (char *)calloc(256, 1);
    }
    m_s32CurFps = m_s32AvgFps = m_s32Infps = portIn->GetFPS();
    m_s32FrameDuration = 0;
    CheckFrcUpdate();
    m_stVencPpCfg.u32OriginW = portIn->GetWidth() & (~15);
    m_stVencPpCfg.u32OriginH = portIn->GetHeight() & (~7);
    m_stEncRateCtrl.gopLen = m_stVencRateCtrl.gop_length;
    m_s32IdrInterval = m_stVencRateCtrl.p_frame;
    m_stEncConfig.frameRateNum = m_s32Infps;
    m_stVencPpCfg.u32StartX = portIn->GetPipX();
    m_stVencPpCfg.u32StartY = portIn->GetPipY();
    m_stVencPpCfg.u32OffsetW = portIn->GetPipWidth();
    m_stVencPpCfg.u32OffsetH = portIn->GetPipHeight();
    m_stVencPpCfg.enSrcFrameType = portIn->GetPixelFormat();
    m_dVbrSeqQuality = -1.0;
    m_dVbrTotalSize = 0;
    m_u64VbrTotalFrame = 0;
    m_s32VbrPreRtnQuant = -1;
    m_dVbrAvgFrameSize = 0;
    m_s32DebugCnt = 0;
    m_u64DebugTotalSize = 0;
    m_u64RtBps = 0;
    m_bIsHeaderReady = false;

    m_stVencInfo.enState = STATENONE;
    m_stVencInfo.stRcInfo.enRCMode = RCMODNONE;
    UpdateDebugInfo(0);
    m_pstSei = NULL;

#ifdef VENCODER_DEPEND_H2V4
    m_enCtbRcChange = E_STATUS_INIT;
#endif

    if(m_bIsH264Lib)
    {
        m_stVencInfo.enType = IPU_H264;
        m_enIPUType = IPU_H264;
    }
    else
    {
        m_stVencInfo.enType = IPU_H265;
        m_enIPUType = IPU_H265;
    }
    m_stVencInfo.s32Width = portIn->GetWidth();
    m_stVencInfo.s32Height = portIn->GetHeight();
    snprintf(m_stVencInfo.ps8Channel, CHANNEL_LENGTH, "%s-%s", Name.c_str(),
            "stream");

    if (m_stVencPpCfg.u32StartX > m_stVencPpCfg.u32OriginW)
    {
        LOGE("pip start x error  ->%d %d\n", m_stVencPpCfg.u32StartX,
                m_stVencPpCfg.u32OriginW);
        throw VBERR_BADPARAM;
    }
    else if (m_stVencPpCfg.u32StartY > m_stVencPpCfg.u32OriginH)
    {
        LOGE("pip start y error  ->%d %d\n", m_stVencPpCfg.u32StartY,
                m_stVencPpCfg.u32OriginH);
        throw VBERR_BADPARAM;
    }
    else if ((m_stVencPpCfg.u32StartX + m_stVencPpCfg.u32OffsetW)
            > m_stVencPpCfg.u32OriginW)
    {
        LOGE("pip value x and width error->%d %d\n", m_stVencPpCfg.u32StartX,
                m_stVencPpCfg.u32OffsetW);
        throw VBERR_BADPARAM;
    }
    else if ((m_stVencPpCfg.u32StartY + m_stVencPpCfg.u32OffsetH)
            > m_stVencPpCfg.u32OriginH)
    {
        LOGE("pip value y and height error ->%4d %4d\n", m_stVencPpCfg.u32StartY,
                m_stVencPpCfg.u32OffsetH);
        throw VBERR_BADPARAM;
    }
    if (m_stVencPpCfg.u32OffsetW == 0)
    {
        m_stVencPpCfg.u32OffsetW = m_stVencPpCfg.u32OriginW;
    }
    if (m_stVencPpCfg.u32OffsetH == 0)
    {
        m_stVencPpCfg.u32OffsetH = m_stVencPpCfg.u32OriginH;
    }
#ifdef VENCODER_DEPEND_H2V4
    if (!m_bIsH264Lib)
    {
        m_stEncIn.gopConfig.size = 0;
        m_stEncIn.gopConfig.id = 0;

        if (SetGopConfigMode(m_s32RefMode))
        {
            LOGE("set gop config mode error\n");
            throw VBERR_BADPARAM;
        }
    }
#endif
    m_SliceSize = 0;
    memset(&m_stVideoScenarioBuffer, 0, sizeof(struct fr_buf_info));
    m_enVideoScenarioMode = VIDEO_DAY_MODE;
    m_pEncode->SetEncRateCtrl(&m_unEncode, &m_stEncRateCtrl);
    m_pEncode->SetEncIn(&m_unEncode, &m_stEncIn);
    m_pEncode->SetEncConfig(&m_unEncode, &m_stEncConfig);
    m_enSEITriggerMode = EN_SEI_TRIGGER_ONCE;
    m_bSEIUserDataSetFlag = false;
}

void IPU_VENCODER::Unprepare()
{
    Port *pIpuPort;

    pIpuPort = GetPort("frame");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("stream");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        pIpuPort = GetPort("mv");
        if (pIpuPort->IsEnabled())
        {
             pIpuPort->FreeVirtualResource();
        }
    }

#endif

    if ( m_stVideoScenarioBuffer.virt_addr != NULL)
    {
        fr_free_single(m_stVideoScenarioBuffer.virt_addr,
                m_stVideoScenarioBuffer.phys_addr);
        memset(&m_stVideoScenarioBuffer,0,sizeof(struct fr_buf_info ));
    }

    if (m_ps8Header != NULL)
    {
        free(m_ps8Header);
        m_ps8Header = NULL;
    }
    if(ms_s32InstanceCounter > 0)
    {
        ms_s32InstanceCounter--;
    }
    else
    {
        system("pidof smartrc | xargs kill -9");
    }
    if (m_pstSei != NULL)
    {
        free(m_pstSei);
        m_pstSei = NULL;
    }
}

void IPU_VENCODER::BasicParaInit(void)
{
    m_enFrameMode = VENC_HEADER_NO_IN_FRAME_MODE;

    memset(&m_stVencEncConfig, 0, sizeof(ST_VIDEO_BASIC_INFO));
    memset(&m_stVencRateCtrl, 0, sizeof(ST_VIDEO_BITRATE_INFO));
    memset(&m_stVencRoiAreaInfo, 0, sizeof(ST_VIDEO_ROI_INFO));
    memset(&m_stEncCodingCtrl, 0 ,sizeof(ST_EncCodingCtrl));
    memset(&m_stEncPreProCfg, 0 ,sizeof(ST_EncPreProcessingCfg));
    memset(&m_stEncRateCtrl, 0 ,sizeof(ST_EncRateCtrl));
    memset(&m_stEncConfig, 0 ,sizeof(ST_EncConfig));

    //for h2v1, roi should be enabled at init phase
    //Otherwise roi couldn't set anymore.
    m_stVencRoiAreaInfo.roi[0].enable = 1;
    m_stVencRoiAreaInfo.roi[1].enable = 1;

    m_bVencEncConfigUpdateFlag = true;
    m_bVencRateCtrlUpdateFlag = true;
    m_bVencCodingCtrlUpdateFlag = true;
    m_u8RateCtrlReset = 0;
    m_s32RateCtrlDistance = 0;
    m_stVencEncConfig.stream_type = VENC_BYTESTREAM;
    m_stVencPpCfg.enSrcFrameType = Pixel::Format::NV12;

    m_stEncConfig.width = 1920;
    m_stEncConfig.height = 1080;
    m_stEncConfig.frameRateDenom = 1;
    m_stEncConfig.frameRateNum = 30;
    m_stVencRateCtrl.qp_hdr = -1;

    m_s32CurFps = m_stEncConfig.frameRateNum /  m_stEncConfig.frameRateDenom;

    if (SetRateCtrl(&m_stRateCtrlInfo)  != 0)
    {
        LOGE("ratectrl parameters is not right, please check json arg reset to\
                default\n");
        m_stVencRateCtrl.qp_max = 42;
        m_stVencRateCtrl.qp_min = 26;
        m_stVencRateCtrl.qp_hdr = -1;
        m_stVencRateCtrl.qp_delta = -1;
        m_stVencRateCtrl.bitrate = 600 * 1024;
        m_stVencRateCtrl.picture_skip = 0;
        m_stVencRateCtrl.rc_mode = VENC_CBR_MODE;
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
        if (!m_bIsH264Lib)
        {
            m_stVencRateCtrl.gop_length = 15;
            m_stVencRateCtrl.p_frame  = 15;
        }
#endif
#ifdef VENCODER_DEPEND_H1V6
        if (m_bIsH264Lib)
        {
            m_stVencRateCtrl.gop_length = m_s32EnableLongterm != 0 ? 150 : 15;
            m_stVencRateCtrl.p_frame = m_s32EnableLongterm != 0 ? 150 : 15;
        }
#endif
    }

#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        if (m_s32EnableLongterm == 1)
        {
            m_stEncConfig.refFrameAmount = 2;
            m_s32RefreshInterval = 15;
            m_stEncConfig.viewMode = H264ENC_BASE_VIEW_MULTI_BUFFER;
        }
        else
        {
            m_stEncConfig.refFrameAmount = 1;
            m_s32RefreshInterval = 0;
            m_stEncConfig.viewMode = H264ENC_BASE_VIEW_DOUBLE_BUFFER;
        }
        m_stEncConfig.H264StreamType = m_stVencEncConfig.stream_type
            == VENC_BYTESTREAM ? H264ENC_BYTE_STREAM : H264ENC_NAL_UNIT_STREAM;
        m_stEncConfig.H264level = H264ENC_LEVEL_5_1;
    }
#endif

#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    if (!m_bIsH264Lib)
    {
        m_stEncConfig.refFrameAmount =  1 + m_stEncConfig.interlacedFrame;
        m_stEncConfig.HEVCStreamType = m_stVencEncConfig.stream_type
            == VENC_BYTESTREAM ? HEVCENC_BYTE_STREAM : HEVCENC_NAL_UNIT_STREAM;
        m_stEncConfig.HEVClevel = HEVCENC_LEVEL_6;
    }
#endif
    m_s32IdrInterval = m_stVencRateCtrl.p_frame;
#ifdef VENCODER_DEPEND_H2V1
    m_stEncConfig.sliceSize = 0;
    m_stEncConfig.constrainedIntraPrediction = 0;
#endif
    m_stEncConfig.strongIntraSmoothing = 1;
    m_stEncConfig.interlacedFrame = 0;
    m_stEncConfig.compressor = 0;
    m_pEncode->SetEncConfig(&m_unEncode, &m_stEncConfig);
    m_pEncode->SetPreProcessingCfg(&m_unEncode, &m_stEncPreProCfg);
}

int IPU_VENCODER::EncodeInstanceInit(void)
{
    int s32Ret = VENCODER_RET_OK;

    if (TryLockSettingResource())
    {
        if (m_bVencEncConfigUpdateFlag)
        {
            m_stEncConfig.frameRateNum =  m_s32Infps;
            m_stEncConfig.frameRateDenom = 1;
        }
        UnlockSettingResource();
    }

    switch(m_stVencPpCfg.u32Rotation)
    {
        case 0 :
            m_stEncConfig.width = m_stVencPpCfg.u32OffsetW;
            m_stEncConfig.height = m_stVencPpCfg.u32OffsetH;
            break;
        case 1 :
        case 2 :
            m_stEncConfig.width = m_stVencPpCfg.u32OffsetH;
            m_stEncConfig.height = m_stVencPpCfg.u32OffsetW;
            break;
        default :
            LOGD("vencoder rotaton only ->0:0, 1:90R, 2:90L\n",
                    m_stVencPpCfg.u32Rotation);
            throw VBERR_BADPARAM;
    }
    m_s32SrcFrameSize = m_stVencPpCfg.u32OriginW * m_stVencPpCfg.u32OriginH;
    ms_pMutexHwLock->lock();
#ifdef VENCODER_DEPEND_H2V4
    if (!m_bIsH264Lib)
    {
        m_stEncConfig.profile = HEVCENC_MAIN_PROFILE;
        m_stEncConfig.bitDepthLuma = 8;
        m_stEncConfig.bitDepthChroma = 8;
        m_stEncConfig.interlacedFrame = 0;

        uint32_t u32Index, u32MaxRefPics = 0;
        int maxTemporalId = 0;;
        for (u32Index = 0; u32Index < m_stEncIn.gopConfig.size; u32Index++)
        {
            HEVCGopPicConfig *cfg = &(m_stEncIn.gopConfig.pGopPicCfg[u32Index]);
            if (cfg->codingType != HEVCENC_INTRA_FRAME)
            {
                if (u32MaxRefPics < cfg->numRefPics)
                {
                    u32MaxRefPics = cfg->numRefPics;
                }

                if (maxTemporalId < cfg->temporalId)
                {
                    maxTemporalId = cfg->temporalId;
                }
            }
        }
        m_stEncConfig.refFrameAmount = u32MaxRefPics
            + m_stEncConfig.interlacedFrame;
        m_stEncConfig.maxTLayers = maxTemporalId + 1;
        m_stEncConfig.frameRateNum = m_s32Infps;
    }
#endif
    m_stEncConfig.chromaQpIndexOffset = m_s32ChromaQpOffset;
    m_pEncode->SetEncConfig(&m_unEncode, &m_stEncConfig);
    VEncRet encRet = m_pEncode->VEncInit(&m_unEncode);
    if (encRet != VENC_OK)
    {
        LOGE("init vencoder error ->%d\n", encRet);
        s32Ret = VENCODER_RET_ERROR;
    }
    ms_pMutexHwLock->unlock();
    return s32Ret;
}

int IPU_VENCODER::EncodeStartStream(Buffer &OutBuf)
{
    VEncRet ret;

    m_stEncIn.pOutBuf = (uint32_t *)(OutBuf.fr_buf.virt_addr);
    m_stEncIn.busOutBuf = OutBuf.fr_buf.phys_addr;
    m_stEncIn.outBufSize = OutBuf.fr_buf.size;

    LOGD("start stream -> v: %p b: %X  %d\n", OutBuf.fr_buf.virt_addr,
            OutBuf.fr_buf.phys_addr, OutBuf.fr_buf.size);
    ms_pMutexHwLock->lock();
    m_pEncode->SetEncIn(&m_unEncode, &m_stEncIn);
    ret = m_pEncode->VEncStrmStart(&m_unEncode, &m_stEncOut);
    ms_pMutexHwLock->unlock();
    if (ret != VENC_OK)
    {
        LOGE("stream start error -> %d %d\n", ret, m_stEncOut.streamSize);
        return VENCODER_RET_ERROR;
    }
    OutBuf.fr_buf.size = m_stEncOut.streamSize;
    OutBuf.fr_buf.priv = VIDEO_FRAME_HEADER;
    SaveHeader((char *) OutBuf.fr_buf.virt_addr, OutBuf.fr_buf.size);
    return VENCODER_RET_OK;
}

int IPU_VENCODER::EncodeStrmEncode(Buffer &InBuf, Buffer &OutBuf)
{
    int s32AlignSize = 0, s32Offset = 0;
    bool bIsIntraFrame = false;
    bool bSetSEI = false;
#ifdef VENCODER_DEPEND_H1V6
    Port *pmv;
    Buffer bmv;
    if (m_bIsH264Lib)
    {
        pmv = GetPort("mv");
        m_pEncode->VEncLinkMode(&m_unEncode, &m_stEncConfig);
    }
#endif

    if (m_bIsH264Lib)
    {
        if (m_s32FrameCnt != 0)
        {
            m_stEncIn.timeIncrement = m_stEncConfig.frameRateDenom;
        }
        else
        {
            m_stEncIn.timeIncrement = 0;
        }
    }
    else
    {
        m_stEncIn.timeIncrement = 1;
    }

    m_pMutexSettingLock->lock();
    if (m_bVencCodingCtrlUpdateFlag)
    {
        EncodeSetCodingCfg();
        m_bVencCodingCtrlUpdateFlag = false;
    }
    if (m_bVencRateCtrlUpdateFlag
#ifdef VENCODER_DEPEND_H2V4
            && m_enCtbRcChange == E_STATUS_INIT
#endif
       )
    {
        EncodeSetRateCtrl();
        m_bVencRateCtrlUpdateFlag = false;
    }

    m_pMutexSettingLock->unlock();

    m_stEncIn.busLuma = InBuf.fr_buf.phys_addr;
    m_stEncIn.busChromaU = InBuf.fr_buf.phys_addr + m_s32SrcFrameSize;
    m_stEncIn.busChromaV = m_stEncIn.busChromaU + m_s32SrcFrameSize/4;
    m_stEncIn.pOutBuf = (uint32_t *)OutBuf.fr_buf.virt_addr;
    m_stEncIn.busOutBuf = OutBuf.fr_buf.phys_addr;
    m_stEncIn.outBufSize = OutBuf.fr_buf.size;


    if (TryLockSettingResource())
    {

#ifdef VENCODER_DEPEND_H1V6
        //longterm reference case use m_bTriggerKeyFrameFlag dynamic set IDR
        //update IDR and longterm reference every time with pm_s32IdrInterval
        //temprary, future may be base on background update algorithm
        if (m_bIsH264Lib)
        {
            if ((m_s32FrameCnt % m_s32IdrInterval)== 0
                    && m_stEncConfig.refFrameAmount > 1)
            {
                m_bTriggerKeyFrameFlag = true;
            }

            if (((m_s32FrameCnt % m_s32IdrInterval == 0
                            && m_stEncConfig.refFrameAmount == 1)
                        || m_bTriggerKeyFrameFlag ))
            {
                bIsIntraFrame = true;
            }
        }
#endif

#if (defined VENCODER_DEPEND_H2V4) || (defined VENCODER_DEPEND_H2V1)
        if (!m_bIsH264Lib)
        {
            m_s32IdrInterval = m_stVencRateCtrl.p_frame;
            if (((m_stEncIn.poc % m_s32IdrInterval) == 0)
                    || m_bTriggerKeyFrameFlag)
            {
                bIsIntraFrame = true;
            }
        }
#endif
        if (bIsIntraFrame)
        {
#ifdef VENCODER_DEPEND_H1V6
            if (m_bIsH264Lib)
            {
                m_stEncIn.H264CodingType = H264ENC_INTRA_FRAME;
                m_stEncIn.ipf = H264ENC_NO_REFERENCE_NO_REFRESH;
                m_stEncIn.ltrf = H264ENC_NO_REFERENCE_NO_REFRESH;
            }
#endif
#if (defined VENCODER_DEPEND_H2V4) || (defined VENCODER_DEPEND_H2V1)
            if (!m_bIsH264Lib)
            {
                m_stEncIn.HEVCCodingType = HEVCENC_INTRA_FRAME;
                m_stEncIn.poc = 0;
            }
#endif
            OutBuf.fr_buf.priv = VIDEO_FRAME_I;
            m_bTriggerKeyFrameFlag = false;

            if (m_enFrameMode == VENC_HEADER_IN_FRAME_MODE)
            {
                s32AlignSize = (m_s32HeaderLength + 0x7) & (~0x7);
                s32Offset = s32AlignSize - m_s32HeaderLength;
                memcpy(((char *)OutBuf.fr_buf.virt_addr + s32Offset),
                        m_ps8Header, m_s32HeaderLength);
                memset((char *)OutBuf.fr_buf.virt_addr, 0, s32Offset);
                m_stEncIn.pOutBuf = (u32 *)((unsigned int)OutBuf.fr_buf.virt_addr
                        + s32AlignSize);
                m_stEncIn.busOutBuf = OutBuf.fr_buf.phys_addr + s32AlignSize;
                m_stEncIn.outBufSize = OutBuf.fr_buf.size - s32AlignSize;
            }
        }
#ifdef VENCODER_DEPEND_H1V6
        else if (m_s32RefreshInterval != 0
                && m_s32RefreshCnt % m_s32RefreshInterval == 0
                && m_stEncConfig.refFrameAmount > 1)
        {
            if (m_bIsH264Lib)
            {
                m_stEncIn.H264CodingType = H264ENC_PREDICTED_FRAME;
                OutBuf.fr_buf.priv = VIDEO_FRAME_P;
                m_stEncIn.ipf = H264ENC_REFRESH;
                m_stEncIn.ltrf = H264ENC_REFERENCE;
            }
        }
        else if (m_s32FrameCnt % 2 == 1 && m_s32RefMode == E_GOPCFG2 && m_bIsH264Lib)
        {
            m_stEncIn.H264CodingType = H264ENC_PREDICTED_FRAME;
            OutBuf.fr_buf.priv = VIDEO_FRAME_P;
            m_stEncIn.ipf = H264ENC_REFERENCE;
            m_stEncIn.ltrf = H264ENC_NO_REFERENCE_NO_REFRESH;
        }
#endif
        else {
#ifdef VENCODER_DEPEND_H1V6
            if (m_bIsH264Lib)
            {
                m_stEncIn.H264CodingType = H264ENC_PREDICTED_FRAME;
                m_stEncIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
                m_stEncIn.ltrf = H264ENC_REFERENCE;
            }
#endif
#if (defined VENCODER_DEPEND_H2V4) || (defined VENCODER_DEPEND_H2V1)
            if (!m_bIsH264Lib)
            {
                m_stEncIn.HEVCCodingType = HEVCENC_PREDICTED_FRAME;
            }
#endif
            OutBuf.fr_buf.priv = VIDEO_FRAME_P;
        }

        //fake uv data
        if (m_enVideoScenarioMode == VIDEO_NIGHT_MODE)
        {
            int s32UvSize = 0;
            switch(m_stVencPpCfg.enSrcFrameType)
            {
                case Pixel::Format::NV12 :
                case Pixel::Format::NV21 :
                    LOGD("video scenario nv12/nv21 \n");
                    s32UvSize = m_stVencPpCfg.u32OriginW
                        * m_stVencPpCfg.u32OriginH / 2;
                    break;
                case Pixel::Format::YUYV422 :
                    LOGD("video scenario yuyv422 \n");
                    s32UvSize = m_stVencPpCfg.u32OriginW
                        * m_stVencPpCfg.u32OriginH;
                    break;
                default :
                    LOGE("video scenario not support -> %d \n",
                            m_stVencPpCfg.enSrcFrameType);
                    throw VBERR_BADPARAM;
            }
            if (m_stVideoScenarioBuffer.virt_addr == NULL)
            {
                char s8Buf[48];
                sprintf(s8Buf,"%s_scenario", Name.c_str());
                LOGE("video scenario->%s %d\n", s8Buf, s32UvSize);
                m_stVideoScenarioBuffer.virt_addr = fr_alloc_single(s8Buf,
                        s32UvSize, &m_stVideoScenarioBuffer.phys_addr);
                memset(m_stVideoScenarioBuffer.virt_addr, 0x80, s32UvSize);
                if (m_stVideoScenarioBuffer.virt_addr == NULL)
                {
                    LOGE("malloc linear scenario buffer error\n");
                    throw VBERR_RUNTIME;
                }
                m_stVideoScenarioBuffer.size = s32UvSize;
            }
            s32UvSize = s32UvSize / 2;
            m_stEncIn.busChromaU = m_stVideoScenarioBuffer.phys_addr;
            m_stEncIn.busChromaV = m_stVideoScenarioBuffer.phys_addr + s32UvSize;
        }

        if (m_bSEIUserDataSetFlag)
        {
            if (m_enSEITriggerMode != EN_SEI_TRIGGER_IFRAME)
            {
                bSetSEI = true;
            }
        }
        if (m_enSEITriggerMode == EN_SEI_TRIGGER_IFRAME && bIsIntraFrame)
        {
            bSetSEI = true;
        }
        if (bSetSEI)
        {
            m_bSEIUserDataSetFlag = true;
            if (VENC_OK != (VEncRet)(m_pEncode->VEncSetSeiUserData(
                            &m_unEncode, (const u8*)m_pstSei->seiUserData,
                            m_pstSei->length)))
            {
                LOGE("set sei user data error\n");
            }
            if (m_enSEITriggerMode == EN_SEI_TRIGGER_ONCE)
            {
                m_pstSei->length = 0;
            }
        }
        UnlockSettingResource();
    }

    LockHwResource();
    m_pEncode->SetEncIn(&m_unEncode, &m_stEncIn);
    VEncRet ret = m_pEncode->VEncStrmEncode(&m_unEncode, &m_stEncOut);

    UnlockHwResource();
    bIsIntraFrame = false;
    switch (ret)
    {
        case VENC_FRAME_READY:

            m_pMutexSettingLock->lock();
            // Clear SEI setting by set length to zero
            if (bSetSEI)
            {
                if (m_enSEITriggerMode != EN_SEI_TRIGGER_EVERY_FRAME)
                {
                    if (VENC_OK != (VEncRet)(m_pEncode->VEncSetSeiUserData(
                                    &m_unEncode, (const u8*)m_pstSei->seiUserData,
                                    0)))
                    {
                        LOGE("set sei user data error\n");
                    }
                }
                m_bSEIUserDataSetFlag = false;
                if ((m_pstSei) && (m_pstSei->length == 0))
                {
                    free(m_pstSei);
                    m_pstSei = NULL;
                }
            }
            m_s32RateCtrlDistance++;
            m_pMutexSettingLock->unlock();

#ifdef VENCODER_DEPEND_H1V6
            if (m_bIsH264Lib
                    && (m_stEncIn.H264CodingType == H264ENC_INTRA_FRAME))
            {
                bIsIntraFrame = true;
            }
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
            if (!m_bIsH264Lib
                    && (m_stEncIn.HEVCCodingType == HEVCENC_INTRA_FRAME))
            {
                bIsIntraFrame = true;
            }
#endif
            OutBuf.fr_buf.size = m_stEncOut.streamSize;
            if (bIsIntraFrame)
            {
                if(m_enFrameMode == VENC_HEADER_IN_FRAME_MODE)
                {
                    OutBuf.fr_buf.size += s32AlignSize;
                }
#ifdef VENCODER_DEPEND_H2V4
                if (m_enCtbRcChange == E_STATUS_UPDATING)
                {
                    m_enCtbRcChange = E_STATUS_INIT;
                }
#endif
            }
            m_s32FrameCnt++;
            if (!m_stEncOut.streamSize)
            {
                break;
            }
#ifdef VENCODER_DEPEND_H1V6
            if (m_bIsH264Lib)
            {
                if (m_stEncIn.H264CodingType == H264ENC_INTRA_FRAME)
                {
                    m_s32RefreshCnt = 1;
                }
                else
                {
                    m_s32RefreshCnt++;
                }
                if (pmv->IsEnabled())
                {
                    bmv = pmv->GetBuffer();
                    bmv.fr_buf.phys_addr = (uint32_t)m_stEncOut.motionVectors;
                    bmv.SyncStamp(&InBuf);
                    pmv->PutBuffer(&bmv);
                }
            }
#endif
#ifdef VENCODER_DEPEND_H2V4
            if (!m_bIsH264Lib)
            {
                int s32CurPoc, s32DeltaPocToNext;
                if (m_stEncIn.gopSize > 1)
                {
                    if (m_stEncIn.HEVCCodingType == HEVCENC_INTRA_FRAME)
                    {
                        s32CurPoc = 0;
                        m_stEncIn.gopPicIdx = 0;
                    }
                    else
                    {
                        s32CurPoc = m_stEncIn.gopConfig.pGopPicCfg[m_stEncIn.gopPicIdx].poc;
                        m_stEncIn.gopPicIdx =
                            (m_stEncIn.gopPicIdx + 1) % m_stEncIn.gopSize;
                        if (m_stEncIn.gopPicIdx == 0)
                        {
                            s32CurPoc -= m_stEncIn.gopSize;
                        }
                    }
                    s32DeltaPocToNext =
                        m_stEncIn.gopConfig.pGopPicCfg[m_stEncIn.gopPicIdx].poc
                        - s32CurPoc;
                    m_stEncIn.poc += s32DeltaPocToNext;
                    m_stEncIn.gopConfig.id = m_stEncIn.gopPicIdx;
                }
                else
                {
                    m_stEncIn.poc++;
                }
            }
#endif
#ifdef VENCODER_DEPEND_H2V1
            if (!m_bIsH264Lib)
            {
                m_stEncIn.poc++;
            }
#endif
            break;
        case VENC_OK:
            break;
        case VENC_OUTPUT_BUFFER_OVERFLOW:
            LOGE("memory over flow  -8\n");
            m_stEncIn.poc = 0;
            return VENCODER_RET_ERROR;
        case VENC_HW_RESET:
            LOGE("HW reset, retry  -16\n");
            m_stEncIn.poc = 0;
            return VENCODER_RET_ERROR;
        default:
            m_stEncIn.poc = 0;
            LOGE("encode error ->%d\n", ret);
            throw VENCODER_RET_OK;
    }

    return VENCODER_RET_OK;
}

int IPU_VENCODER::EncodeEndStream(Buffer *InBuf, Buffer *OutBuf)
{
    VEncRet ret = VENC_OK;
    m_stEncIn.busLuma = InBuf->fr_buf.phys_addr;
    m_stEncIn.busChromaU = InBuf->fr_buf.phys_addr + m_s32SrcFrameSize;
    m_stEncIn.busChromaV = 0;
    m_stEncIn.timeIncrement = 0;
    m_stEncIn.pOutBuf = (uint32_t *)(OutBuf->fr_buf.virt_addr);
    m_stEncIn.busOutBuf = OutBuf->fr_buf.phys_addr;
    m_stEncOut.pNaluSizeBuf = (uint32_t *)OutBuf->fr_buf.virt_addr;
    ms_pMutexHwLock->lock();
    m_pEncode->SetEncIn(&m_unEncode, &m_stEncIn);
    ret = m_pEncode->VEncStrmEnd(&m_unEncode);
    ms_pMutexHwLock->unlock();
    if (ret != VENC_OK)
    {
        return VENCODER_RET_ERROR;
    }
    OutBuf->fr_buf.size = m_stEncOut.streamSize;
    OutBuf->fr_buf.priv = VIDEO_FRAME_I;
    return VENCODER_RET_OK;
}

int IPU_VENCODER::EncodeSetRateCtrl(void)
{
    VEncRet ret;
    int s32Ret = VENCODER_RET_OK;

    ret = m_pEncode->VEncGetRateCtrl(&m_unEncode, &m_stEncRateCtrl);
    if (ret != VENC_OK)
    {
        LOGE("vencoder get rate ctrl error\n");
        return VENCODER_RET_ERROR;
    }

    m_stEncRateCtrl.mbRc = 0;
#ifdef VENCODER_DEPEND_H2V4
    if (!m_bIsH264Lib)
    {
        m_stEncRateCtrl.blockRCSize = 2;
        m_stEncRateCtrl.mbRc = m_stVencRateCtrl.mbrc;
        m_stEncRateCtrl.monitorFrames = 120;  //[10, 120] not use
    }
#endif
    m_stEncRateCtrl.hrd = m_stVencRateCtrl.hrd;
    m_stEncRateCtrl.hrdCpbSize = m_stVencRateCtrl.hrd_cpbsize;

#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        m_stEncRateCtrl.mbQpAdjustment = m_stVencRateCtrl.mb_qp_adjustment;
        if (m_stEncConfig.refFrameAmount == 2)
        {
            m_stEncRateCtrl.longTermPicRate = 1;
        }
        else
        {
            m_stEncRateCtrl.longTermPicRate = 0;
        }
    }
#endif
    m_s32IdrInterval = m_stVencRateCtrl.p_frame;
    m_stEncRateCtrl.pictureSkip = m_stVencRateCtrl.picture_skip;
    m_stEncRateCtrl.gopLen = m_stVencRateCtrl.gop_length;
    m_stEncRateCtrl.fixedIntraQp = 0;

    /* patch for reset ratectrl window to accelate smooth qp only user setting available
        qp_max threshold 42 and qp_max diff threshold 3 are pre-set value that can not cover all cases
    */
    if((m_stVencRateCtrl.qp_max >= 42 && (u32)m_stVencRateCtrl.qp_max > m_stEncRateCtrl.qpMax
        && (u32)m_stVencRateCtrl.qp_max - m_stEncRateCtrl.qpMax >= 3
        && m_s32RateCtrlDistance > m_stEncRateCtrl.gopLen && m_u8RateCtrlReset == 1)
        || (m_u8RateCtrlReset == 2))
    {
        m_stEncRateCtrl.u32NewStream = 1;
    }
    else
    {
        m_stEncRateCtrl.u32NewStream = 0;
    }
    m_s32RateCtrlDistance = 0;
    m_u8RateCtrlReset = 0;

    if (m_stVencRateCtrl.rc_mode == VENC_CBR_MODE ||
            m_stVencRateCtrl.rc_mode == VENC_FIXQP_MODE)
    {
        LOGD("enc cbr mode\n");
        m_stEncRateCtrl.bitPerSecond = m_stVencRateCtrl.bitrate;
        m_stEncRateCtrl.qpMax = m_stVencRateCtrl.qp_max;
        m_stEncRateCtrl.qpMin = m_stVencRateCtrl.qp_min;
        m_stEncRateCtrl.qpHdr = m_stVencRateCtrl.qp_hdr;
        m_stEncRateCtrl.intraQpDelta = m_stVencRateCtrl.qp_delta;
        m_stEncRateCtrl.pictureRc = 1;
    } else if (m_stVencRateCtrl.rc_mode == VENC_VBR_MODE)
    {
        LOGD("enc vbr mode\n");

        if (m_stVencRateCtrl.qp_hdr == -1)
        {
            m_pEncode->VEncCalculateInitialQp(&m_unEncode,&m_stVencRateCtrl.qp_hdr,
                m_u64MaxBitRate * (m_u32VbrThreshold + 100)/2/100);
            if(m_stVencRateCtrl.qp_hdr > m_stVencRateCtrl.qp_max)
            {
                m_stVencRateCtrl.qp_hdr = m_stVencRateCtrl.qp_max;
            }
            else if(m_stVencRateCtrl.qp_hdr < m_stVencRateCtrl.qp_min)
            {
                m_stVencRateCtrl.qp_hdr = m_stVencRateCtrl.qp_min;
            }
        }
        if (m_dVbrSeqQuality == -1.0)
        {
            m_dVbrSeqQuality = 2.0 / m_stVencRateCtrl.qp_hdr;
        }
        if (m_s32VbrPreRtnQuant == -1)
        {
            m_s32VbrPreRtnQuant = m_stVencRateCtrl.qp_hdr;
        }

        m_stEncRateCtrl.qpMax = m_stVencRateCtrl.qp_max;
        m_stEncRateCtrl.qpMin = m_stVencRateCtrl.qp_min;
        m_stEncRateCtrl.qpHdr = m_stVencRateCtrl.qp_hdr;
        m_stEncRateCtrl.intraQpDelta = m_stVencRateCtrl.qp_delta;
        m_stEncRateCtrl.pictureRc = 0;
        if (m_u32VbrPicSkip)
        {
            m_stEncRateCtrl.bitPerSecond = m_u64MaxBitRate;
        }
    }
    else
    {
        LOGD("enc rc mode error\n");
        return false;
    }

    if (m_stEncRateCtrl.bitPerSecond < BITRATE_MIN)
    {
        m_stEncRateCtrl.bitPerSecond = BITRATE_MIN;
    }
    if (m_stEncRateCtrl.bitPerSecond > BITRATE_MAX)
    {
        m_stEncRateCtrl.bitPerSecond = BITRATE_MAX;
    }
#ifdef VENCODER_DEPEND_H2V4
    m_stEncRateCtrl.bitVarRangeI = 2000;
    m_stEncRateCtrl.bitVarRangeP = 2000;
    m_stEncRateCtrl.bitVarRangeB = 2000;
#endif
    m_pEncode->SetEncRateCtrl(&m_unEncode, &m_stEncRateCtrl);
    ret = m_pEncode->VEncSetRateCtrl(&m_unEncode);
    if (ret != VENC_OK)
    {
        LOGE("vencoder set rate ctrl error:%d\n", ret);
        s32Ret = VENCODER_RET_ERROR;
    }
    return s32Ret;
}

int IPU_VENCODER::EncodeSetCodingCfg(void)
{
    VEncRet ret;
    u32 u32CtbPerCol = 0, u32CtbPerRow = 0;

    ret = m_pEncode->VEncGetCodingCtrl(&m_unEncode, &m_stEncCodingCtrl);
    if (ret != VENC_OK)
    {
        LOGE("vencoder get coding ctrl error\n");
        return VENCODER_RET_ERROR;
    }

    m_stEncCodingCtrl.sliceSize = m_SliceSize;
    m_stEncCodingCtrl.disableDeblockingFilter = 0;
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    if (!m_bIsH264Lib)
    {
        m_stEncCodingCtrl.tc_Offset = -2;
        m_stEncCodingCtrl.beta_Offset = 5;
        m_stEncCodingCtrl.enableSao = 1;
        m_stEncCodingCtrl.enableDeblockOverride = 0;
        m_stEncCodingCtrl.deblockOverride = 0;
        m_stEncCodingCtrl.cabacInitFlag = 0;
        m_stEncCodingCtrl.enableScalingList = 0;
        m_stEncCodingCtrl.m_s32ChromaQpOffset =  m_s32ChromaQpOffset;
        m_stEncCodingCtrl.HEVCIntraArea.top = -1;
        m_stEncCodingCtrl.HEVCIntraArea.left = -1;
        m_stEncCodingCtrl.HEVCIntraArea.bottom = -1;
        m_stEncCodingCtrl.HEVCIntraArea.right = -1;
        m_stEncCodingCtrl.HEVCIntraArea.enable = 0;
        m_stEncCodingCtrl.insertrecoverypointmessage = 0;
        u32CtbPerRow = (m_stEncConfig.width + H2_CTB_SIZE - 1) / H2_CTB_SIZE;
        u32CtbPerCol = (m_stEncConfig.height + H2_CTB_SIZE - 1) / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi1Area.top = (m_stVencRoiAreaInfo.roi[0].y)
            / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi1Area.left = (m_stVencRoiAreaInfo.roi[0].x)
            / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi1Area.right = (m_stVencRoiAreaInfo.roi[0].w
                + m_stVencRoiAreaInfo.roi[0].x) / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi1Area.bottom = (m_stVencRoiAreaInfo.roi[0].h
                + m_stVencRoiAreaInfo.roi[0].y) / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi1Area.enable = m_stVencRoiAreaInfo.roi[0].enable;
        m_stEncCodingCtrl.HEVCRoi1Area.right = m_stEncCodingCtrl.HEVCRoi1Area.right >= u32CtbPerRow ? \
            u32CtbPerRow -1 : m_stEncCodingCtrl.HEVCRoi1Area.right;
        m_stEncCodingCtrl.HEVCRoi1Area.bottom = m_stEncCodingCtrl.HEVCRoi1Area.bottom >= u32CtbPerCol ?  \
            u32CtbPerCol -1 : m_stEncCodingCtrl.HEVCRoi1Area.bottom;

        m_stEncCodingCtrl.HEVCRoi2Area.top = (m_stVencRoiAreaInfo.roi[1].y)
            / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi2Area.left = (m_stVencRoiAreaInfo.roi[1].x)
            / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi2Area.right = (m_stVencRoiAreaInfo.roi[1].w
                + m_stVencRoiAreaInfo.roi[1].x) / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi2Area.bottom = (m_stVencRoiAreaInfo.roi[1].h
                + m_stVencRoiAreaInfo.roi[1].y) / H2_CTB_SIZE;
        m_stEncCodingCtrl.HEVCRoi2Area.enable = m_stVencRoiAreaInfo.roi[1].enable;
        m_stEncCodingCtrl.HEVCRoi2Area.right = m_stEncCodingCtrl.HEVCRoi2Area.right >= u32CtbPerRow ? \
            u32CtbPerRow -1 : m_stEncCodingCtrl.HEVCRoi2Area.right;
        m_stEncCodingCtrl.HEVCRoi2Area.bottom = m_stEncCodingCtrl.HEVCRoi2Area.bottom >= u32CtbPerCol ?  \
            u32CtbPerCol -1 : m_stEncCodingCtrl.HEVCRoi2Area.bottom;
    }
#endif
#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        m_stEncCodingCtrl.fieldOrder = 0;
        m_stEncCodingCtrl.H264IntraArea.top = -1;
        m_stEncCodingCtrl.H264IntraArea.left = -1;
        m_stEncCodingCtrl.H264IntraArea.bottom = -1;
        m_stEncCodingCtrl.H264IntraArea.right = -1;
        m_stEncCodingCtrl.H264IntraArea.enable = 0;
        u32CtbPerRow = (m_stEncConfig.width + H1_CTB_SIZE - 1) / H1_CTB_SIZE;
        u32CtbPerCol = (m_stEncConfig.height + H1_CTB_SIZE - 1) / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi1Area.top = (m_stVencRoiAreaInfo.roi[0].y)
            / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi1Area.left = (m_stVencRoiAreaInfo.roi[0].x)
            / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi1Area.right = (m_stVencRoiAreaInfo.roi[0].w
                + m_stVencRoiAreaInfo.roi[0].x) / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi1Area.bottom = (m_stVencRoiAreaInfo.roi[0].h
                + m_stVencRoiAreaInfo.roi[0].y) / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi1Area.enable = m_stVencRoiAreaInfo.roi[0].enable;
        m_stEncCodingCtrl.H264Roi1Area.right = m_stEncCodingCtrl.H264Roi1Area.right >= u32CtbPerRow ? \
            u32CtbPerRow -1 : m_stEncCodingCtrl.H264Roi1Area.right;
        m_stEncCodingCtrl.H264Roi1Area.bottom = m_stEncCodingCtrl.H264Roi1Area.bottom >= u32CtbPerCol ?  \
            u32CtbPerCol -1 : m_stEncCodingCtrl.H264Roi1Area.bottom;

        m_stEncCodingCtrl.H264Roi2Area.top = (m_stVencRoiAreaInfo.roi[1].y)
            / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi2Area.left = (m_stVencRoiAreaInfo.roi[1].x)
            / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi2Area.right = (m_stVencRoiAreaInfo.roi[1].w
                + m_stVencRoiAreaInfo.roi[1].x) / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi2Area.bottom = (m_stVencRoiAreaInfo.roi[1].h
                + m_stVencRoiAreaInfo.roi[1].y) / H1_CTB_SIZE;
        m_stEncCodingCtrl.H264Roi2Area.enable = m_stVencRoiAreaInfo.roi[1].enable;
        m_stEncCodingCtrl.H264Roi2Area.right = m_stEncCodingCtrl.H264Roi2Area.right >= u32CtbPerRow ? \
            u32CtbPerRow -1 : m_stEncCodingCtrl.H264Roi2Area.right;
        m_stEncCodingCtrl.H264Roi2Area.bottom = m_stEncCodingCtrl.H264Roi2Area.bottom >= u32CtbPerCol ?  \
            u32CtbPerCol -1 : m_stEncCodingCtrl.H264Roi2Area.bottom;
    }
#endif
    m_stEncCodingCtrl.cirStart = 0;
    m_stEncCodingCtrl.cirInterval = 0;
    m_stEncCodingCtrl.roi1DeltaQp = m_stVencRoiAreaInfo.roi[0].qp_offset;
    m_stEncCodingCtrl.roi2DeltaQp = m_stVencRoiAreaInfo.roi[1].qp_offset;
    m_stEncCodingCtrl.seiMessages = 0;

#ifdef VENCODER_DEPEND_H2V1
    if (!m_bIsH264Lib)
    {
        m_stEncCodingCtrl.max_cu_size = 64;
        m_stEncCodingCtrl.min_cu_size = 8;
        m_stEncCodingCtrl.max_tr_size = 16;
        m_stEncCodingCtrl.min_tr_size = 4;

        m_stEncCodingCtrl.tr_depth_intra = 2;
        m_stEncCodingCtrl.tr_depth_inter =
        m_stEncCodingCtrl.max_cu_size == 64 ?  4:3;
    }
#endif
    m_stEncCodingCtrl.videoFullRange = 1;
    m_pEncode->SetEncCodingCtrl(&m_unEncode, &m_stEncCodingCtrl);
    ret = m_pEncode->VEncSetCodingCtrl(&m_unEncode);
    if (ret != VENC_OK)
    {
        LOGE("vencoder set coding ctrl error ->%d\n", ret);
        return VENCODER_RET_ERROR;
    }
    LOGD("set coding type success\n");
    return VENCODER_RET_OK;
}

int IPU_VENCODER::EncodeSetPreProcessCfg()
{
    VEncRet ret;
    ms_pMutexHwLock->lock();
    ret = m_pEncode->VEncGetPreProcessing(&m_unEncode, &m_stEncPreProCfg);
    ms_pMutexHwLock->unlock();
    if (ret != VENC_OK)
    {
        LOGE("vencoder get pre processing error -> %d \n", ret);
        return VENCODER_RET_ERROR;
    }
#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        m_stEncPreProCfg.H264InputType =
            (H264EncPictureType)SwitchFormat(GetPort("frame")->GetPixelFormat());
        m_stEncPreProCfg.H264Rotation =
            (H264EncPictureRotation)m_stVencPpCfg.u32Rotation;
        m_stEncPreProCfg.H264colorConversion.type = H264ENC_RGBTOYUV_BT601;
    }
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    if (!m_bIsH264Lib)
    {
        m_stEncPreProCfg.HEVCInputType =
            (HEVCEncPictureType)SwitchFormat(GetPort("frame")->GetPixelFormat());
        m_stEncPreProCfg.HEVCRotation =
            (HEVCEncPictureRotation)m_stVencPpCfg.u32Rotation;
        m_stEncPreProCfg.HEVCcolorConversion.type = HEVCENC_RGBTOYUV_BT601;
    }
#endif
    m_stEncPreProCfg.origWidth = m_stVencPpCfg.u32OriginW;
    m_stEncPreProCfg.origHeight = m_stVencPpCfg.u32OriginH;
    m_stEncPreProCfg.xOffset = m_stVencPpCfg.u32StartX;
    m_stEncPreProCfg.yOffset = m_stVencPpCfg.u32StartY;
    m_stEncPreProCfg.scaledOutput = 0;
    m_stEncPreProCfg.videoStabilization = 0;
    m_stEncPreProCfg.interlacedFrame = 0;

    ms_pMutexHwLock->lock();
    m_pEncode->SetPreProcessingCfg(&m_unEncode, &m_stEncPreProCfg);
    ret = m_pEncode->VEncSetPreProcessing(&m_unEncode);
    ms_pMutexHwLock->unlock();
    if (ret != VENC_OK)
    {
        LOGE("vencoder set pre processing error -> %d \n", ret);
        return VENCODER_RET_ERROR;
    }
    LOGD("vencoder set pre processing success\n");
    return VENCODER_RET_OK;

}

int IPU_VENCODER::EncodeInstanceRelease(void)
{
    VEncRet hret;
    ms_pMutexHwLock->lock();
    hret = m_pEncode->VEncRelease(&m_unEncode);
    ms_pMutexHwLock->unlock();
    if (hret != VENC_OK)
    {
        return VENCODER_RET_ERROR;
    }
    return VENCODER_RET_OK;
}

#if (defined VENCODER_DEPEND_H2V4) || (defined VENCODER_DEPEND_H2V1)
bool IPU_VENCODER::GetLevelFromResolution(uint32_t s32X, uint32_t s32Y, int *level)
{
    bool bRet = true;
    HEVCEncLevel enTempLevel = HEVCENC_LEVEL_6;
    uint32_t u32Width[HEVCENC_MAX_FRAME_SIZE_NUM] =
    {  176, 352, 640, 960, 1920, 4096, 8192 };
    uint32_t u32Height[HEVCENC_MAX_FRAME_SIZE_NUM] =
    {  144, 288, 360, 540, 1080, 2160, 4320 };
    int i;
    int j;
    for (i =1; i< HEVCENC_MAX_FRAME_SIZE_NUM; i++)
    {
        if (s32X <  u32Width[i])
        {
            break;
        }
        if (s32X > u32Width[i] && s32X <= u32Width[i + 1])
        {
            break;
        }
        else
        {
            continue;
        }
    }
    for (j = 1;j < HEVCENC_MAX_FRAME_SIZE_NUM; j++)
    {
        if (s32Y <  u32Height[j])
        {
            break;
        }
        if (s32Y > u32Height[j] && s32Y <= u32Height[j + 1])
        {
            break;
        }
        else {
            continue;
        }
    }
    LOGD("target level range -> %d %d\n", i, j);
    i = i>j  ? i:j;

    switch (i)
    {
        case 1:
            enTempLevel = HEVCENC_LEVEL_2;
            break;
        case 2:
            enTempLevel = HEVCENC_LEVEL_2_1;
            break;
        case 3:
            enTempLevel = HEVCENC_LEVEL_3;
            break;
        case 4:
            enTempLevel = HEVCENC_LEVEL_1;
            break;
        case 5:
            enTempLevel = HEVCENC_LEVEL_1;
            break;
        case 6:
            enTempLevel = HEVCENC_LEVEL_1;
            break;
        case 7:
            enTempLevel = HEVCENC_LEVEL_1;
            break;
        default :
            bRet = false;
            break;
    }
    if (bRet == true)
    {
        *level = (int) enTempLevel;
    }
    return bRet;
}
#endif

int IPU_VENCODER::SwitchFormat(Pixel::Format enSrcFormat)
{
    if (enSrcFormat == Pixel::Format::RGBA8888)
    {
        return VENC_BGR888; //encode need set BGR888 other than RGB888
    }
    else if (enSrcFormat == Pixel::Format::NV12)
    {
        return VENC_YUV420_SEMIPLANAR;
    }
    else if (enSrcFormat == Pixel::Format::NV21)
    {
        return VENC_YUV420_SEMIPLANAR_VU;
    }
    else if (enSrcFormat == Pixel::Format::RGB565)
    {
        return VENC_RGB565;
    }
    else if (enSrcFormat == Pixel::Format::YUYV422)
    {
        return VENC_YUV422_INTERLEAVED_YUYV;
    }
    else
    {
        throw VBERR_BADPARAM;
    }
}

int IPU_VENCODER::SaveHeader(char *ps8Buf, int s32Len)
{
    LOGD("H2 header saved %d bytes\n", s32Len);

    memcpy(m_ps8Header, ps8Buf, s32Len);
    m_s32HeaderLength = s32Len;
    return 0;
}
bool IPU_VENCODER::CheckFrcUpdate(void)
{
    if (m_s32InFpsRatio == -1)
    {
        if (m_stFrcInfo.framerate == 0)
        {
            m_s32InFpsRatio = 1;
            m_stFrcInfo.framebase = 1;
            m_stFrcInfo.framerate = m_s32Infps;
        }
        int s32TargetInFps = m_stFrcInfo.framerate / m_stFrcInfo.framebase;
        int s32Quotient = 0;
        int s32Remainder = 0;
        if (s32TargetInFps == m_s32Infps)
        {
            m_s32InFpsRatio = 1;
        }
        else if (s32TargetInFps > m_s32Infps)
        {
            s32Quotient = s32TargetInFps / m_s32Infps;
            s32Remainder = s32TargetInFps % m_s32Infps;
            if (s32Remainder > (m_s32Infps / 2))
            {
                s32Quotient++;
            }
            if (s32Remainder != 0)
            {
                LOGE("WARNING: frc repeat may not support ,just set to %d times\
                        of origin fps ->%d:%d\n",
                        s32Quotient, m_s32Infps, m_s32Infps * s32Quotient);
            }
            m_s32InFpsRatio = s32Quotient;
            m_s32FrameDuration = 1000/(m_s32InFpsRatio * m_s32Infps);
        }
        else
        {
            s32Quotient = m_s32Infps * m_stFrcInfo.framebase
                / m_stFrcInfo.framerate;
            s32Remainder = m_s32Infps * m_stFrcInfo.framebase
                % m_stFrcInfo.framerate;
            if (s32Remainder > (m_stFrcInfo.framerate / m_stFrcInfo.framebase / 2))
            {
                s32Quotient++;
            }
            if (s32Remainder != 0)
            {
                LOGE("WARNING: frc skip may not support ,just set to 1/%d times\
                        of origin fps ->%d:%d\n",
                        s32Quotient, m_s32Infps, m_s32Infps / s32Quotient);
            }
            if (s32Quotient == 1)
            {
                m_s32InFpsRatio = s32Quotient;
            }
            else
            {
                m_s32InFpsRatio = 0 - s32Quotient;
            }
            m_s32FrameDuration = 1000 * m_s32InFpsRatio / m_s32Infps;
        }

        return true;
    }
    else
    {
        return false;
    }
}

#ifdef VENCODER_DEPEND_H2V4
const char * IPU_VENCODER::nextToken (const char *ps8Str)
{
    char *ps8Pointer = strchr(ps8Str, ' ');
    if (ps8Pointer)
    {
        while (*ps8Pointer == ' ') ps8Pointer++;
        if (*ps8Pointer == '\0')
        {
            ps8Pointer = NULL;
        }
    }
    return ps8Pointer;
}

int IPU_VENCODER::ParseGopConfigString (const char *ps8Line,
        HEVCGopConfig *pstGopCfg, int s32FrameIndex, int s32GopSize)
{
    if (!ps8Line)
    {
        return -1;
    }

    //format: FrameN Type POC QPoffset QPfactor TemporalID num_ref_pics
    //ref_pics used_by_cur
    int s32FrameN, s32Poc, s32NumRefPics, i;
    char s8Type;
    HEVCGopPicConfig *cfg = &(pstGopCfg->pGopPicCfg[pstGopCfg->size++]);

    sscanf (ps8Line, "Frame%d", &s32FrameN);
    if (s32FrameN != (s32FrameIndex + 1)) return -1;

    ps8Line = nextToken(ps8Line);
    if (!ps8Line)
    {
        return -1;
    }
    sscanf (ps8Line, "%c", &s8Type);
    if (s8Type == 'P' || s8Type == 'p')
    {
        cfg->codingType = HEVCENC_PREDICTED_FRAME;
    }
    else if (s8Type == 'B' || s8Type == 'b')
    {
        cfg->codingType = HEVCENC_BIDIR_PREDICTED_FRAME;
    }
    else
    {
        return -1;
    }

    ps8Line = nextToken(ps8Line);
    if (!ps8Line)
    {
        return -1;
    }
    sscanf (ps8Line, "%d", &s32Poc);
    if (s32Poc < 1 || s32Poc > s32GopSize)
    {
        return -1;
    }
    cfg->poc = s32Poc;

    ps8Line = nextToken(ps8Line);
    if (!ps8Line)
    {
        return -1;
    }
    sscanf (ps8Line, "%d", &(cfg->QpOffset));

    ps8Line = nextToken(ps8Line);
    if (!ps8Line)
    {
        return -1;
    }
    sscanf (ps8Line, "%lf", &(cfg->QpFactor));

    ps8Line = nextToken(ps8Line);
    if (!ps8Line)
    {
        return -1;
    }
    sscanf (ps8Line, "%d", &(cfg->temporalId));

    ps8Line = nextToken(ps8Line);
    if (!ps8Line)
    {
        return -1;
    }
    sscanf (ps8Line, "%d", &s32NumRefPics);
    if (s32NumRefPics < 0 || s32NumRefPics > HEVCENC_MAX_REF_FRAMES)
    {
        LOGE("GOP Config: Error, num_ref_pic can not be more than %d \n",
                HEVCENC_MAX_REF_FRAMES);
        return -1;
    }
    cfg->numRefPics = s32NumRefPics;

    for (i = 0; i < s32NumRefPics; i++)
    {
        ps8Line = nextToken(ps8Line);
        if (!ps8Line)
        {
            return -1;
        }
        sscanf (ps8Line, "%d", &(cfg->refPics[i].ref_pic));

        if (cfg->refPics[i].ref_pic < (-s32GopSize-1))
        {
            LOGE("GOP Config: Error, Reference picture %d for GOP size = %d is\
                    not supported\n", cfg->refPics[i].ref_pic, s32GopSize);
            return -1;
        }
    }
    if (i < s32NumRefPics)
    {
        return -1;
    }

    for (i = 0; i < s32NumRefPics; i++)
    {
        ps8Line = nextToken(ps8Line);
        if (!ps8Line)
        {
            return -1;
        }
        sscanf (ps8Line, "%u", &(cfg->refPics[i].used_by_cur));
    }
    if (i < s32NumRefPics)
    {
        return -1;
    }
    return 0;
}

int IPU_VENCODER::SetGopConfigMode(uint32_t u32Mode)
{
    if (u32Mode < 0 || u32Mode > 2)
    {
        LOGE("invalid gopcfg mode parameter\n");
        return -1;
    }

    const char **ps8pConfig;
    int s32Id = 0;
    int s32GopSize = 0;

    switch (u32Mode)
    {
        case E_GOPCFG1:
            ps8pConfig = VGopCfgDefault_GOPSize_1;
            s32GopSize = 1;
            break;
        case E_GOPCFG2:
            ps8pConfig = VGopCfgDefault_GOPSize_2;
            s32GopSize = 2;
            break;
        case E_GOPCFG4:
            ps8pConfig = VGopCfgDefault_GOPSize_4;
            s32GopSize = 4;
            break;
        default:
            break;
    }

    m_stEncIn.gopSize = s32GopSize;
    m_stEncIn.gopPicIdx = 0;

    //why put here , not put at the init place?
    m_stEncIn.gopConfig.pGopPicCfg = (HEVCGopPicConfig*)calloc(s32GopSize,
            sizeof(HEVCGopPicConfig));
    while (ps8pConfig[s32Id])
    {
        if (ParseGopConfigString (ps8pConfig[s32Id], &(m_stEncIn.gopConfig),
                    s32Id, s32GopSize))
        {
            LOGE("ParseGopConfigString error\n");
            return -1;
        }
        s32Id++;
    }
    m_pEncode->SetEncIn(&m_unEncode, &m_stEncIn);
    return 0;
}
#endif //VENCODER_DEPEND_H2V4
void IPU_VENCODER::UpdateDebugInfo(int s32Update)
{
    if (s32Update)
    {
        m_stVencInfo.stRcInfo.s8MaxQP = m_stVencRateCtrl.qp_max;
        m_stVencInfo.stRcInfo.s8MinQP = m_stVencRateCtrl.qp_min;
        m_stVencInfo.stRcInfo.s32QPDelta = m_stVencRateCtrl.qp_delta;
        m_stVencInfo.stRcInfo.s32Gop = m_stVencRateCtrl.gop_length;
        m_stVencInfo.stRcInfo.s32IInterval = m_stVencRateCtrl.p_frame;
        if (m_bIsH264Lib)
        {
            m_stVencInfo.stRcInfo.s32Mbrc = m_stEncRateCtrl.mbQpAdjustment;
        }
        else
        {
            m_stVencInfo.stRcInfo.s32Mbrc = m_stEncRateCtrl.mbRc;
        }
        m_stVencInfo.f32BitRate = m_u64RtBps/(1024.0);
        m_stVencInfo.f32FPS = m_s32CurFps;
        m_stVencInfo.stRcInfo.s32PictureSkip = m_stVencRateCtrl.picture_skip;

        if (m_stVencRateCtrl.rc_mode == VENC_CBR_MODE)
        {
            m_stVencInfo.stRcInfo.enRCMode = CBR;
            m_pEncode->VEncGetRealQp(&m_unEncode,&m_stVencInfo.stRcInfo.s32RealQP);
            m_stVencInfo.stRcInfo.f32ConfigBitRate = m_stVencRateCtrl.bitrate
                / (1024.0);
            m_stVencInfo.stRcInfo.u32ThreshHold = 0;
        }
        else if (m_stVencRateCtrl.rc_mode == VENC_VBR_MODE)
        {
            m_stVencInfo.stRcInfo.enRCMode = VBR;
            m_stVencInfo.stRcInfo.s32RealQP = m_stVencRateCtrl.qp_hdr;
            m_stVencInfo.stRcInfo.f32ConfigBitRate = m_u64MaxBitRate / (1024.0);
            m_stVencInfo.stRcInfo.u32ThreshHold = m_u32VbrThreshold;
        }
        else
        {
            m_stVencInfo.stRcInfo.enRCMode = FIXQP;
            m_stVencInfo.stRcInfo.s8MaxQP = m_stVencInfo.stRcInfo.s8MinQP
                = m_stVencRateCtrl.qp_hdr;
            m_stVencInfo.stRcInfo.s32RealQP = m_stVencRateCtrl.qp_hdr;
            m_stVencInfo.stRcInfo.s32QPDelta = 0;
            m_stVencInfo.stRcInfo.f32ConfigBitRate = 0;
            m_stVencInfo.stRcInfo.u32ThreshHold = 0;
        }
    }
    else
    {
        m_stVencInfo.f32BitRate = 0.0;
        m_stVencInfo.f32FPS = 0.0;
        m_stVencInfo.u64InCount = 0;
        m_stVencInfo.u64OutCount = 0;
        m_stVencInfo.u32FrameDrop = 0;
    }
}

void IPU_VENCODER::CheckCtbRcUpdate(void)
{
    Buffer bfin, bfout;
    Port *pin, *pout;
    bool bReset = false;

    bReset = m_bVencRateCtrlUpdateFlag
#ifdef VENCODER_DEPEND_H2V4
        && (m_enCtbRcChange == E_STATUS_SETTING)
#endif
        ;
    if (bReset)
    {
        pin = GetPort("frame");
        pout = GetPort("stream");
        bfin = pin->GetBuffer(&bfin);
        bfout = pout->GetBuffer(&bfout);
        if (EncodeEndStream(&bfin, &bfout) != VENCODER_RET_OK)
        {
            LOGE("EncodeEndStream failed.\n");
            throw VBERR_BADPARAM;
        }
        pin->PutBuffer(&bfin);
        pout->PutBuffer(&bfout);

        EncodeSetRateCtrl();

        m_bIsHeaderReady = false;
        bfout = pout->GetBuffer(&bfout);
        if (EncodeStartStream(bfout) != VENCODER_RET_OK)
        {
            LOGE("encode start stream error \n");
            throw VBERR_RUNTIME;
        }
        else
        {
            m_bIsHeaderReady = true;
        }
        pout->PutBuffer(&bfout);
        m_bVencRateCtrlUpdateFlag = false;
#ifdef VENCODER_DEPEND_H2V4
        m_enCtbRcChange = E_STATUS_UPDATING;
#endif
        m_bTriggerKeyFrameFlag =  true;
    }
}

int IPU_VENCODER::SetSeiUserData(ST_VIDEO_SEI_USER_DATA_INFO *pstSei)
{
    if ((pstSei == NULL) || (pstSei->length > VIDEO_SEI_USER_DATA_MAXLEN)
           || (pstSei->length <= 0))
    {
        LOGE("sei param error\n");
        return VBEBADPARAM;
    }

    m_pMutexSettingLock->lock();
    if (!m_bSEIUserDataSetFlag)
    {
        if (m_pstSei == NULL)
        {
            m_pstSei = (ST_VIDEO_SEI_USER_DATA_INFO*)malloc(sizeof(ST_VIDEO_SEI_USER_DATA_INFO));
        }
        if (m_pstSei == NULL)
        {
            LOGE("malloc sei info error\n");
            m_pMutexSettingLock->unlock();
            return VBEMEMERROR;
        }
        memcpy(m_pstSei, pstSei, sizeof(ST_VIDEO_SEI_USER_DATA_INFO));
        m_bSEIUserDataSetFlag = true;
        m_enSEITriggerMode = m_pstSei->enMode;
        LOGE("seting sei info ok, mode:%d, size:%d, data:%s\n",
            m_enSEITriggerMode, m_pstSei->length, m_pstSei->seiUserData);
    }
    else
    {
        m_pMutexSettingLock->unlock();
        LOGE("seting last sei info, try later.\n");
        return VBEFAILED;
    }
    m_pMutexSettingLock->unlock();
    return VBOK;
}

int IPU_VENCODER::SetSliceHeight(int s32SliceHeight)
{
    if (s32SliceHeight < 0 || (uint32_t)s32SliceHeight > m_stVencPpCfg.u32OffsetH)
    {
        LOGE("SetSliceSize param error: s32SliceHeight:%d Input Height:%d\n",s32SliceHeight, m_stVencPpCfg.u32OffsetH);
        return VBEBADPARAM;
    }
    if(m_bIsH264Lib)
    {
        s32SliceHeight = (s32SliceHeight + H1_CTB_SIZE -1)/ H1_CTB_SIZE;
    }
    else
    {
        s32SliceHeight = (s32SliceHeight + H2_CTB_SIZE -1)/ H2_CTB_SIZE;
    }
    m_pMutexSettingLock->lock();
    m_bVencCodingCtrlUpdateFlag = false;
    m_SliceSize = s32SliceHeight;
    m_bVencCodingCtrlUpdateFlag = true;
    m_pMutexSettingLock->unlock();
    return VBOK;
}
int IPU_VENCODER::GetSliceHeight(int *ps32SliceHeight)
{
    if (ps32SliceHeight == NULL)
    {
        LOGE("GetSliceSize Input NULL\n");
        return VBEBADPARAM;
    }

    m_pMutexSettingLock->lock();
    if(m_bIsH264Lib)
    {
        *ps32SliceHeight = m_SliceSize * H1_CTB_SIZE;
    }
    else
    {
        *ps32SliceHeight = m_SliceSize * H2_CTB_SIZE;
    }
    m_pMutexSettingLock->unlock();
    return VBOK;
}
int IPU_VENCODER::GetDebugInfo(void *pInfo, int *ps32Size)
{
    if (pInfo == NULL)
    {
        LOGE("[%s]Get debug info fail\n",__func__);
        return VBEFAILED;
    }

    memcpy((ST_VENC_INFO *)pInfo, &m_stVencInfo, sizeof(ST_VENC_INFO));
    if (ps32Size != NULL)
    {
        *ps32Size = sizeof(ST_VENC_INFO);
    }
    return 0;
}

int IPU_VENCODER::SetBasicInfo(ST_VIDEO_BASIC_INFO *pstCfg)
{
    LockSettingResource();
    m_bVencEncConfigUpdateFlag = false;
    m_stVencEncConfig = *pstCfg;
#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        m_stVencEncConfig.media_type = VIDEO_MEDIA_H264;
    }
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
    if (!m_bIsH264Lib)
    {
        m_stVencEncConfig.media_type = VIDEO_MEDIA_HEVC;
    }
#endif
    m_bVencEncConfigUpdateFlag = true;
    UnlockSettingResource();
    return 0;
}

int IPU_VENCODER::GetBasicInfo(ST_VIDEO_BASIC_INFO *pstCfg)
{
    LockSettingResource();
#ifdef VENCODER_DEPEND_H1V6
        if (m_bIsH264Lib)
        {
            m_stVencEncConfig.media_type = VIDEO_MEDIA_H264;
        }
#endif
#if (defined VENCODER_DEPEND_H2V1) || (defined VENCODER_DEPEND_H2V4)
        if (!m_bIsH264Lib)
        {
            m_stVencEncConfig.media_type = VIDEO_MEDIA_HEVC;
        }
#endif

    *pstCfg= m_stVencEncConfig;
    UnlockSettingResource();
    return 0;
}

/* not maintain from 10.23.2017 */
int IPU_VENCODER::SetBitrate(ST_VIDEO_BITRATE_INFO *pstCfg)
{
    if (pstCfg->rc_mode != VENC_CBR_MODE && pstCfg->rc_mode != VENC_VBR_MODE)
    {
        LOGE("rc_mode set error, only could be set VENC_CBR_MODE or VENC_VBR_MODE\n");
        return VBEBADPARAM;
    }
    if (pstCfg->gop_length < 1 || pstCfg->gop_length > 150)
    {
        LOGE("Please fix your gop_length value:%d\n", pstCfg->gop_length);
    }
    if (pstCfg->qp_delta < -12 || pstCfg->qp_delta > 12)
    {
        LOGE("Please fix qp_delta value:\n", pstCfg->qp_delta);
        return VBEBADPARAM;
    }
    if (pstCfg->qp_max > 51 || pstCfg->qp_max < 0 ||
            pstCfg->qp_min > 51 || pstCfg->qp_min < 0)
    {
        LOGE("Please fix qp_max or qp_min value: qp_max %d, qp_min: %d\n",
                pstCfg->qp_max, pstCfg->qp_min);
        return VBEBADPARAM;
    }
    if (pstCfg->qp_hdr < -1 || pstCfg->qp_hdr > 51)
    {
        LOGE("Please fix your qp_hdr value:%d\n",
                pstCfg->qp_hdr);
    }
    if (pstCfg->p_frame == 0)
    {
        pstCfg->p_frame = m_stVencRateCtrl.p_frame;
    }

    m_s32IdrInterval = m_stVencRateCtrl.p_frame;
#ifdef VENCODER_DEPEND_H1V6
    m_s32RefreshInterval = m_s32EnableLongterm == 1 ? pstCfg->refresh_interval: 0;
#endif

    LockSettingResource();
    m_bVencRateCtrlUpdateFlag = false;
    if(m_stVencRateCtrl.rc_mode != pstCfg->rc_mode)
    {
        m_u8RateCtrlReset = 2;
    }
    m_stVencRateCtrl = *pstCfg;
    m_bVencRateCtrlUpdateFlag = true;
    UnlockSettingResource();
    return 0;
}

int IPU_VENCODER::GetBitrate(ST_VIDEO_BITRATE_INFO *pstCfg)
{
    LockSettingResource();
    *pstCfg = m_stVencRateCtrl;
    if (pstCfg->rc_mode == VENC_VBR_MODE)
    {
        LOGE("vbr mode, set bitrate = 0, will add auto cal later");
        pstCfg->bitrate = 0;
    }
    UnlockSettingResource();

    return 0;
}

int IPU_VENCODER::SetRateCtrl(ST_VIDEO_RATE_CTRL_INFO *pstCfg)
{
    if (!pstCfg
            || pstCfg->rc_mode < VENC_CBR_MODE
            || pstCfg->rc_mode > VENC_FIXQP_MODE
            || pstCfg->idr_interval <= 0
            || pstCfg->gop_length < 1
            || pstCfg->gop_length > 150
            || pstCfg->picture_skip < 0
            || pstCfg->picture_skip > 1
            || pstCfg->hrd < 0
            || pstCfg->hrd > 1
            || pstCfg->hrd_cpbsize < 0
            || pstCfg->mbrc < 0
            || pstCfg->mbrc > 1
            || pstCfg->mb_qp_adjustment < -8
            || pstCfg->mb_qp_adjustment > 7)
    {
        LOGE("invalid rate ctrl param\n");
        return VBEBADPARAM;
    }
    if (pstCfg->rc_mode == VENC_CBR_MODE)
    {
        if (pstCfg->cbr.qp_max > 51
                || pstCfg->cbr.qp_min < 0
                || pstCfg->cbr.qp_min > pstCfg->cbr.qp_max
                || pstCfg->cbr.bitrate < BITRATE_MIN
                || pstCfg->cbr.bitrate > BITRATE_MAX
                || pstCfg->cbr.qp_delta < -12
                || pstCfg->cbr.qp_delta > 12
                || (pstCfg->cbr.qp_hdr != -1
                    && (pstCfg->cbr.qp_hdr > pstCfg->cbr.qp_max
                        ||pstCfg->cbr.qp_hdr < pstCfg->cbr.qp_min)))
        {
            LOGE("invalid cbr param\n");
            return VBEBADPARAM;
        }
    } else if (pstCfg->rc_mode == VENC_VBR_MODE)
    {
        if (pstCfg->vbr.qp_max > 51
                || pstCfg->vbr.qp_min < 0
                || pstCfg->vbr.qp_min > pstCfg->vbr.qp_max
                || pstCfg->vbr.max_bitrate < BITRATE_MIN
                || pstCfg->vbr.max_bitrate > BITRATE_MAX
                || pstCfg->vbr.threshold < 0
                || pstCfg->vbr.threshold > 100)
        {
            LOGE("invalid vbr param\n");
            return VBEBADPARAM;
        }
    } else if (pstCfg->rc_mode == VENC_FIXQP_MODE)
    {
        if (pstCfg->fixqp.qp_fix > 51 || pstCfg->fixqp.qp_fix < 0)
        {
            LOGE("invalid fixqp param\n");
            return VBEBADPARAM;
        }
    }

    LockSettingResource();
    m_bVencRateCtrlUpdateFlag = false;

    switch (pstCfg->rc_mode)
    {
        case VENC_CBR_MODE:
            if(m_stVencRateCtrl.bitrate != pstCfg->cbr.bitrate)
            {
                m_u8RateCtrlReset = 2;
            }
            m_stVencRateCtrl.qp_max = pstCfg->cbr.qp_max;
            m_stVencRateCtrl.qp_min = pstCfg->cbr.qp_min;
            m_stVencRateCtrl.qp_delta = pstCfg->cbr.qp_delta;
            m_stVencRateCtrl.qp_hdr = pstCfg->cbr.qp_hdr;
            m_stVencRateCtrl.bitrate = pstCfg->cbr.bitrate;
            m_stVencRateCtrl.picture_skip = pstCfg->picture_skip;
            m_u32VbrPicSkip = 0;
            break;
        case VENC_VBR_MODE:
            if(m_u64MaxBitRate != (uint64_t)pstCfg->vbr.max_bitrate
                || m_u32VbrThreshold != (uint32_t)pstCfg->vbr.threshold)
            {
                m_u8RateCtrlReset = 2;
            }
            m_stVencRateCtrl.qp_min = pstCfg->vbr.qp_min;
            m_stVencRateCtrl.qp_max = pstCfg->vbr.qp_max;
            m_stVencRateCtrl.qp_delta = pstCfg->vbr.qp_delta;
            if(m_stVencRateCtrl.rc_mode != VENC_VBR_MODE)
            {
                m_stVencRateCtrl.qp_hdr = -1;
            }
            m_u64MaxBitRate = pstCfg->vbr.max_bitrate;
            m_u32VbrThreshold = pstCfg->vbr.threshold;
            m_u64VbrTotalFrame = 0;
            m_dVbrTotalSize = 0;
            m_dVbrAvgFrameSize = (double)m_u64MaxBitRate
                * (m_u32VbrThreshold + 100) / 200 / 8.0 / m_s32CurFps;
            m_u32VbrPicSkip = pstCfg->picture_skip;
            if (!m_u32VbrPicSkip)
            {
                m_stVencRateCtrl.picture_skip = 0;
            }
            break;
        case VENC_FIXQP_MODE:
            m_stVencRateCtrl.qp_hdr = pstCfg->fixqp.qp_fix;
            m_stVencRateCtrl.qp_max = pstCfg->fixqp.qp_fix;
            m_stVencRateCtrl.qp_min = pstCfg->fixqp.qp_fix;
            m_stVencRateCtrl.picture_skip = pstCfg->picture_skip;
            m_u32VbrPicSkip = 0;
            break;
        default:
            LOGE("invalid rc mode\n");
            break;
    }
    if(m_stVencRateCtrl.rc_mode != pstCfg->rc_mode)
    {
        m_u8RateCtrlReset = 2;
    }
    m_stVencRateCtrl.rc_mode = pstCfg->rc_mode;
    m_stVencRateCtrl.gop_length = pstCfg->gop_length;
    m_stVencRateCtrl.p_frame = pstCfg->idr_interval;
    m_stVencRateCtrl.hrd = pstCfg->hrd;
    m_stVencRateCtrl.hrd_cpbsize = pstCfg->hrd_cpbsize;
#ifdef VENCODER_DEPEND_H2V4
    if (!m_bIsH264Lib)
    {
        m_enCtbRcChange = m_stVencRateCtrl.mbrc == pstCfg->mbrc
            ? E_STATUS_INIT: E_STATUS_SETTING;
        m_stVencRateCtrl.mbrc = pstCfg->mbrc;
    }
#endif
#ifdef VENCODER_DEPEND_H1V6
    if (m_bIsH264Lib)
    {
        m_stVencRateCtrl.mb_qp_adjustment = pstCfg->mb_qp_adjustment;
        if (m_s32EnableLongterm == 1)
        {
            m_s32RefreshInterval = pstCfg->refresh_interval;
        }
        else
        {
            m_s32RefreshInterval = 0;
        }
    }
#endif
    m_bVencRateCtrlUpdateFlag = true;
    UnlockSettingResource();
    return 0;
}

int IPU_VENCODER::GetRateCtrl(ST_VIDEO_RATE_CTRL_INFO *pstCfg)
{
    if (pstCfg == NULL)
    {
        LOGE("invalid param\n");
        return -1;
    }
    else
    {
        memset(pstCfg, 0, sizeof(ST_VIDEO_RATE_CTRL_INFO));
    }
    LockSettingResource();
    switch (m_stVencRateCtrl.rc_mode)
    {
        case VENC_CBR_MODE:
            pstCfg->cbr.qp_max = m_stVencRateCtrl.qp_max;
            pstCfg->cbr.qp_min = m_stVencRateCtrl.qp_min;
            pstCfg->cbr.qp_delta = m_stVencRateCtrl.qp_delta;
            pstCfg->cbr.qp_hdr = m_stVencRateCtrl.qp_hdr;
            pstCfg->cbr.bitrate = m_stVencRateCtrl.bitrate;
            pstCfg->picture_skip = m_stVencRateCtrl.picture_skip;
            break;
        case VENC_VBR_MODE:
            pstCfg->vbr.qp_max = m_stVencRateCtrl.qp_max;
            pstCfg->vbr.qp_min = m_stVencRateCtrl.qp_min;
            pstCfg->vbr.qp_delta = m_stVencRateCtrl.qp_delta;
            pstCfg->vbr.max_bitrate = m_u64MaxBitRate;
            pstCfg->vbr.threshold = m_u32VbrThreshold;
            pstCfg->picture_skip = m_u32VbrPicSkip;
            break;
        case VENC_FIXQP_MODE:
            pstCfg->fixqp.qp_fix = m_stVencRateCtrl.qp_hdr;
            pstCfg->picture_skip = m_stVencRateCtrl.picture_skip;
            break;
        default:
            LOGE("invalid rc mode\n");
            break;
    }
    pstCfg->rc_mode = m_stVencRateCtrl.rc_mode;
    pstCfg->gop_length = m_stVencRateCtrl.gop_length;
    pstCfg->idr_interval = m_stVencRateCtrl.p_frame;
    pstCfg->hrd = m_stVencRateCtrl.hrd;
    pstCfg->hrd_cpbsize = m_stVencRateCtrl.hrd_cpbsize;
    pstCfg->refresh_interval = m_s32RefreshInterval;
#ifdef VENCODER_DEPEND_H2V4
    if (!m_bIsH264Lib)
    {
        pstCfg->mbrc = m_stVencRateCtrl.mbrc;
    }
#endif
#ifdef VENCODER_DEPEND_H1V6
    pstCfg->mb_qp_adjustment = m_stVencRateCtrl.mb_qp_adjustment;
#endif
    UnlockSettingResource();
    return 0;
}

int IPU_VENCODER::SetROI(ST_VIDEO_ROI_INFO *pstCfg)
{
    for (int i = 0; i < VENC_H265_ROI_AREA_MAX_NUM;i++)
    {
        if (!pstCfg || pstCfg->roi[i].qp_offset > 0
                || pstCfg->roi[i].qp_offset < -15)
        {
            LOGE("invalid roi qp_offset param\n");
            return VBEBADPARAM;
        }
        if (pstCfg->roi[i].x >= m_stEncConfig.width
                || pstCfg->roi[i].y >= m_stEncConfig.height
                || (pstCfg->roi[i].x + pstCfg->roi[i].w) > m_stEncConfig.width
                || (pstCfg->roi[i].y + pstCfg->roi[i].h) > m_stEncConfig.height)
        {
            LOGE("invalid roi param\n");
            return VBEBADPARAM;
        }
    }

    LockSettingResource();
    m_bVencCodingCtrlUpdateFlag = false;
    m_stVencRoiAreaInfo = *pstCfg;
    m_bVencCodingCtrlUpdateFlag = true;
    UnlockSettingResource();
    return 0;
}

int IPU_VENCODER::GetROI(ST_VIDEO_ROI_INFO *pstCfg)
{
    LockSettingResource();
    *pstCfg = m_stVencRoiAreaInfo;
    UnlockSettingResource();
    return 0;
}

int IPU_VENCODER::SetFrameMode(EN_VIDEO_ENCODE_FRAME_TYPE enArg)
{
    m_enFrameMode = enArg;
    return 1;
}

int IPU_VENCODER::GetFrameMode()
{
    return m_enFrameMode;
}

int IPU_VENCODER::SetFrc(ST_VIDEO_FRC_INFO *pstInfo)
{
    if (pstInfo->framebase == 0)
    {
        LOGE("frc framebase can not be zero\n");
        return VBEBADPARAM;
    }
    if (pstInfo->framerate <= 0 || pstInfo->framebase <= 0)
    {
        LOGE("target frc can be negative\n");
        return VBEBADPARAM;
    }
    m_pMutexSettingLock->lock();
    m_stFrcInfo = *pstInfo;
    m_s32InFpsRatio = -1;
    m_pMutexSettingLock->unlock();
    return 1;
}

int IPU_VENCODER::GetFrc(ST_VIDEO_FRC_INFO *pstInfo)
{
    m_pMutexSettingLock->lock();
    *pstInfo = m_stFrcInfo;
    m_pMutexSettingLock->unlock();
    return 1;
}

int IPU_VENCODER::SetScenario(EN_VIDEO_SCENARIO stMode)
{
    LOGE("%s->line:%d ->stMode:%d\n", __FUNCTION__, __LINE__, stMode);
    if(m_enVideoScenarioMode == stMode)
    {
        return 1;
    }
    switch(m_stVencPpCfg.enSrcFrameType)
    {

        case Pixel::Format::NV12 :
            LOGD("input src type -> NV12 \n");
            break;
        case Pixel::Format::NV21 :
            LOGD("input src type -> NV21 \n");
            break;
        case Pixel::Format::YUYV422 :
            LOGD("input src type -> YUYV422 \n");
            break;
        default :
            LOGE("input src type -> %d, not support scenario set, defualt day\
                    stMode \n", m_stVencPpCfg.enSrcFrameType);
            return VBEBADPARAM;
    }
    LockSettingResource();
    m_enVideoScenarioMode = stMode;
    UnlockSettingResource();
    return 1;
}

int IPU_VENCODER::SetPipInfo(ST_VIDEO_PIP_INFO *pstPipInfo)
{
    if (strncmp(pstPipInfo->portname, "frame", 5) == 0)
    {
        GetPort("frame")->SetPipInfo(pstPipInfo->x, pstPipInfo->y,
                pstPipInfo->w, pstPipInfo->h);
    }

    return 0;
}

int IPU_VENCODER::GetPipInfo(ST_VIDEO_PIP_INFO *pstPipInfo)
{
    Port *pin;

    if (strncmp(pstPipInfo->portname, "frame", 5) == 0)
    {
        pin = GetPort("frame");
        pstPipInfo->x = pin->GetPipX();
        pstPipInfo->y = pin->GetPipY();
        pstPipInfo->w = pin->GetPipWidth();
        pstPipInfo->h = pin->GetPipHeight();
    }

    return 0;
}

EN_VIDEO_SCENARIO IPU_VENCODER::GetScenario(void)
{
    return m_enVideoScenarioMode;
}

int IPU_VENCODER::GetRTBps()
{
    return m_u64RtBps;
}

int IPU_VENCODER::TriggerKeyFrame(void)
{
    LockSettingResource();
    m_bTriggerKeyFrameFlag = true;
    UnlockSettingResource();
    return 1;
}

void IPU_VENCODER::VideoInfo(Buffer *dst)
{

    int s32Ratio = 1;
#ifdef VENCODER_DEPEND_H1V6
    s32Ratio = (m_s32EnableLongterm ? 1 : 5);
#endif

    if (m_s32DebugCnt >= m_stVencRateCtrl.gop_length * s32Ratio)
    {
        if(dst->fr_buf.timestamp == m_u64DebugLastFrameTime)
        {
            m_u64RtBps = (m_stVencRateCtrl.rc_mode == VENC_CBR_MODE) ? m_stVencRateCtrl.bitrate : (m_u64MaxBitRate * (100 + m_u32VbrThreshold) / 100);
            return;
        }
        m_s32AvgFps = m_s32DebugFpsCnt * 1000
            / (dst->fr_buf.timestamp - m_u64DebugLastFrameTime);
        if (abs(m_s32CurFps - m_s32AvgFps) > 1)
        {
            m_s32CurFps = m_s32AvgFps;
        }
        m_s32DebugCnt = 0;
        m_s32DebugFpsCnt = 0;
        m_u64RtBps = m_u64DebugTotalSize * 8 * 1000
            / (dst->fr_buf.timestamp - m_u64DebugLastFrameTime);
#if 0
        LOGE("avgfps: %d\n", m_s32AvgFps);
        LOGE("t_out_bitrate: \t\t\t%d\n", m_u64DebugTotalSize * 8 * 1000
                / (dst->fr_buf.timestamp - m_u64DebugLastFrameTime));
        LOGE("t_fps:\t\t\t%d\n", m_s32CurFps);
        LOGE("rc_mode:\t\t\t%d\n", m_stVencRateCtrl.rc_mode);
#endif
       m_pMutexSettingLock->lock();
       if(((m_stVencRateCtrl.rc_mode == VENC_CBR_MODE && m_u64RtBps > (uint64_t)m_stVencRateCtrl.bitrate * 9 / 10) /* Just Reserve a Gap */
           || (m_stVencRateCtrl.rc_mode == VENC_VBR_MODE && m_u64RtBps > m_u64MaxBitRate))
           && (m_u8RateCtrlReset != 2))
        {
           m_u8RateCtrlReset = 1;
        }
        m_pMutexSettingLock->unlock();
    }
    else
    {
        if (!m_s32DebugCnt)
        {
            m_u64DebugTotalSize = 0;
            m_u64DebugLastFrameTime = dst->fr_buf.timestamp;
        }
        m_u64DebugTotalSize += dst->fr_buf.size;
        m_s32DebugCnt++;
        if (dst->fr_buf.size)
        {
            m_s32DebugFpsCnt++;
        }
#if 0
        if (dst->fr_buf.priv == VIDEO_FRAME_I)
        {
            LOGE("I: %d\n", dst->fr_buf.size);
        }
        else
        {
            LOGE("P: %d\n", dst->fr_buf.size);
        }
#endif
    }
}
bool IPU_VENCODER::VbrProcess(Buffer *dst)
{
    int     s32RtnQuant;
    int64_t s64Deviation;
    uint32_t u32VbrFrameSize;
    uint32_t u32VbrCurFps;
    double dTargetFrameSizeMin, dTargetFrameSizeMax;
    double dQualityScale, dBaseQuality, dTargetQuality;
    double dOverFlow;

    if (m_s32VbrPreRtnQuant == -1)
    {
        LOGE("wait qp_hdr init\n");
        return false;
    }

    u32VbrCurFps = m_s32CurFps;
    u32VbrFrameSize = dst->fr_buf.size;
    dTargetFrameSizeMax = ((double)m_u64MaxBitRate * (THRESHOLD_FACTOR - 1)
            / THRESHOLD_FACTOR + (double)m_u64MaxBitRate * m_u32VbrThreshold
            / 100 / THRESHOLD_FACTOR) / 8.0 / u32VbrCurFps;
    dTargetFrameSizeMin = ((double)m_u64MaxBitRate / THRESHOLD_FACTOR
            + (double)m_u64MaxBitRate * m_u32VbrThreshold / 100
            * (THRESHOLD_FACTOR - 1) / THRESHOLD_FACTOR) / 8.0 / u32VbrCurFps;

    if ((m_s32VbrPreRtnQuant == m_stVencRateCtrl.qp_min)
            && (u32VbrFrameSize < dTargetFrameSizeMin))
    {
        goto skip_integrate_err;
    }
    else if ((m_s32VbrPreRtnQuant == m_stVencRateCtrl.qp_max)
            && (u32VbrFrameSize > dTargetFrameSizeMax))
    {
        goto skip_integrate_err;
    }

    m_u64VbrTotalFrame++;
    m_dVbrTotalSize += u32VbrFrameSize;

skip_integrate_err:
    if (m_dVbrTotalSize > dTargetFrameSizeMax * m_u64VbrTotalFrame)
    {
        s64Deviation = m_dVbrTotalSize - dTargetFrameSizeMax * m_u64VbrTotalFrame;
    } else if (m_dVbrTotalSize < dTargetFrameSizeMin * m_u64VbrTotalFrame)
    {
        s64Deviation = m_dVbrTotalSize - dTargetFrameSizeMin * m_u64VbrTotalFrame;
    }
    else
    {
        s64Deviation = 0;
    }

    if (m_s32VbrPreRtnQuant >= 2)
    {
        m_dVbrSeqQuality -= m_dVbrSeqQuality / SEQ_QUALITY_FACTOR;
        m_dVbrSeqQuality += 2.0 / (double) m_s32VbrPreRtnQuant
            / SEQ_QUALITY_FACTOR;
        if (dst->fr_buf.priv == VIDEO_FRAME_P)
        {
            m_dVbrAvgFrameSize -= m_dVbrAvgFrameSize / AVG_FRAME_FACTOR;
            m_dVbrAvgFrameSize += u32VbrFrameSize / AVG_FRAME_FACTOR;
        }
    }

    if (m_dVbrAvgFrameSize > dTargetFrameSizeMax)
    {
        dQualityScale = dTargetFrameSizeMax / m_dVbrAvgFrameSize
            * dTargetFrameSizeMax / m_dVbrAvgFrameSize;
    } else if (m_dVbrAvgFrameSize < dTargetFrameSizeMin)
    {
        dQualityScale = dTargetFrameSizeMin / m_dVbrAvgFrameSize
            * dTargetFrameSizeMin / m_dVbrAvgFrameSize;
    }
    else
    {
        dQualityScale = 1.0;
    }

    dBaseQuality = m_dVbrSeqQuality;
    if (dQualityScale >= 1.0)
    {
        dBaseQuality = 1.0 - (1.0 - dBaseQuality) / dQualityScale;
    }
    else
    {
        dBaseQuality = MIN_QUALITY + (dBaseQuality - MIN_QUALITY) * dQualityScale;
    }

    dOverFlow = -((double) s64Deviation / DEVIATION_FACTOR);

    if (dQualityScale > 1.0)
    {
        dTargetQuality = dBaseQuality + (dBaseQuality - MIN_QUALITY)
            * dOverFlow / dTargetFrameSizeMin;
    } else if (dQualityScale < 1.0)
    {
        dTargetQuality = dBaseQuality + (dBaseQuality - MIN_QUALITY)
            * dOverFlow / dTargetFrameSizeMax;
    }
    else
    {
        dTargetQuality = (double)2.0 / m_s32VbrPreRtnQuant;
    }

    if (dTargetQuality > 2.0)
    {
        dTargetQuality = 2.0;
    }
    if (dTargetQuality < MIN_QUALITY)
    {
        dTargetQuality = MIN_QUALITY;
    }

    s32RtnQuant = (int) (2.0 / dTargetQuality);
    if (s32RtnQuant < 52)
    {
        m_dpVbrQuantError[s32RtnQuant] += 2.0 / dTargetQuality - s32RtnQuant;
        if (m_dpVbrQuantError[s32RtnQuant] >= 1.0)
        {
            m_dpVbrQuantError[s32RtnQuant] -= 1.0;
            s32RtnQuant++;
        }
    }
    if (u32VbrCurFps <= 10)
    {
        if (s32RtnQuant > m_s32VbrPreRtnQuant + 3)
        {
            s32RtnQuant = m_s32VbrPreRtnQuant + 3;
        } else if (s32RtnQuant < m_s32VbrPreRtnQuant - 3)
        {
            s32RtnQuant = m_s32VbrPreRtnQuant - 3;
        }
    }
    else
    {
        if (s32RtnQuant > m_s32VbrPreRtnQuant + 1)
        {
            s32RtnQuant = m_s32VbrPreRtnQuant + 1;
        } else if (s32RtnQuant < m_s32VbrPreRtnQuant - 1)
        {
            s32RtnQuant = m_s32VbrPreRtnQuant - 1;
        }
    }

    if (s32RtnQuant > m_stVencRateCtrl.qp_max)
    {
        s32RtnQuant = m_stVencRateCtrl.qp_max;
        if (m_u32VbrPicSkip && m_stVencRateCtrl.picture_skip == 0)
        {
            LockSettingResource();
            m_bVencRateCtrlUpdateFlag = false;
            m_stVencRateCtrl.picture_skip = 1;
            m_bVencRateCtrlUpdateFlag = true;
            UnlockSettingResource();
        }
    } else if (s32RtnQuant < m_stVencRateCtrl.qp_min)
    {
        s32RtnQuant = m_stVencRateCtrl.qp_min;
    }
    else
    {
        if (m_u32VbrPicSkip && m_stVencRateCtrl.picture_skip == 1)
        {
            LockSettingResource();
            m_bVencRateCtrlUpdateFlag = false;
            m_stVencRateCtrl.picture_skip = 0;
            m_bVencRateCtrlUpdateFlag = true;
            UnlockSettingResource();
        }
    }
    if (u32VbrCurFps <= 10 && m_u64MaxBitRate * m_u32VbrThreshold / 100 <= 128000)
    {
        if (dst->fr_buf.priv == VIDEO_FRAME_I)
        {
            s32RtnQuant -= 5;
            if (s32RtnQuant <= 25)
            {
                s32RtnQuant = 25;
            }
        }
    }

#if 0
    LOGE("\t m_s32Infps\t\t%d\n", m_s32Infps);
    LOGE("\t curfps\t\t%d\n", u32VbrCurFps);
    LOGE("\t m_u64VbrTotalFrame\t\t%ld\n", m_u64VbrTotalFrame);
    LOGE("\t m_dVbrTotalSize\t\t%lf\n", m_dVbrTotalSize);
    LOGE("\t target_u32VbrFrameSize_min\t\t%lf\n", dTargetFrameSizeMin);
    LOGE("\t target_u32VbrFrameSize_max\t\t%lf\n", dTargetFrameSizeMax);
    LOGE("\t m_dVbrSeqQuality\t\t%lf\n", m_dVbrSeqQuality);
    LOGE("\t m_dVbrAvgFrameSize\t\t%lf\n", m_dVbrAvgFrameSize);
    LOGE("\t u32VbrFrameSize\t\t%d\n", u32VbrFrameSize);
    LOGE("\t dQualityScale\t\t%lf\n", dQualityScale);
    LOGE("\t s64Deviation\t\t%ld\n", s64Deviation);
    LOGE("\t dOverFlow\t\t%lf\n", dOverFlow);
    LOGE("\t dBaseQuality\t\t%lf\n", dBaseQuality);
    LOGE("\t dTargetQuality\t\t%lf\n", dTargetQuality);
    LOGE("\t m_s32VbrPreRtnQuant: \t\t%d\n", m_s32VbrPreRtnQuant);
    LOGE("\t s32RtnQuant: \t\t%d\n", s32RtnQuant);
#endif
    if (s32RtnQuant != m_s32VbrPreRtnQuant)
    {
        LockSettingResource();
        m_bVencRateCtrlUpdateFlag = false;
        m_stVencRateCtrl.qp_hdr = s32RtnQuant;
        m_bVencRateCtrlUpdateFlag = true;
        UnlockSettingResource();
        m_s32VbrPreRtnQuant = s32RtnQuant;
    }
    return true;
}

bool IPU_VENCODER::TryLockSettingResource(void)
{
    return m_pMutexSettingLock->try_lock();
}

bool IPU_VENCODER::LockSettingResource(void)
{
    m_pMutexSettingLock->lock();
    return true;
}

bool IPU_VENCODER::UnlockSettingResource(void)
{
    m_pMutexSettingLock->unlock();
    return true;
}

bool IPU_VENCODER::TrylockHwResource(void)
{
    return ms_pMutexHwLock->try_lock();
}

bool IPU_VENCODER::LockHwResource(void)
{
    ms_pMutexHwLock->lock();
    return true;
}

bool IPU_VENCODER::UnlockHwResource(void)
{
    ms_pMutexHwLock->unlock();
    return true;
}

