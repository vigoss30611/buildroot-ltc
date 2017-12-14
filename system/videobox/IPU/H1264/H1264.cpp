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
#include "H1264.h"
#include <assert.h>

#define BITRATE_MIN 10000
#define BITRATE_MAX (50000*1200)
#define MIN_QUALITY (double)2.0/52.0
#define AVG_FRAME_FACTOR    (double)4.0
#define SEQ_QUALITY_FACTOR  (double)100.0
#define DEVIATION_FACTOR    (double)10.0
#define THRESHOLD_FACTOR    (double)4.0
#define FPS_MONITOR 0

DYNAMIC_IPU(IPU_H1264, "h1264");
#define CALYUVSIZE(w,h) ((((w) + 15)&(~0x0f)) * (((h) + 3)&(~0x03)))
int IPU_H1264::InstanceCnt = 0;
bool IPU_H1264::bSmartrcStarted = false;

static int SwitchFormat(Pixel::Format srcFormat)
{
    if(srcFormat == Pixel::Format::RGBA8888)
        return H264ENC_BGR888;// encode need set BGR888 other than RGB888
    else if(srcFormat == Pixel::Format::NV12)
        return H264ENC_YUV420_SEMIPLANAR;
    else if(srcFormat == Pixel::Format::NV21)
        return H264ENC_YUV420_SEMIPLANAR_VU;
    else if(srcFormat == Pixel::Format::RGB565)
        return H264ENC_RGB565;
    else if(srcFormat == Pixel::Format::YUYV422)
        return H264ENC_YUV422_INTERLEAVED_YUYV;
    else
        throw VBERR_BADPARAM;
}

void IPU_H1264::WorkLoop()
{
    int hRet;
    Buffer bfin, bfout;
    Port *pin, *pout;

    pin = GetPort("frame");
    pout = GetPort("stream");
    int fpsRatio = 1;

    if (this->IsH264InstReady() != true) {
        if (this->ExH264EncInit() != H264RET_OK) {
            LOGE("init H264 error\n");
            throw VBERR_BADPARAM;
        }
    }
    hRet = this->ExH264EncSetCodingCtrl();
    if(hRet != H264RET_OK){
        LOGE("ExH264EncSetCodingCtrl failed.\n");
        throw VBERR_BADPARAM;
    }

    hRet = this->ExH264EncSetRateCtrl();
    if(hRet != H264RET_OK){
        LOGE("ExH264EncSetRateCtrl failed.\n");
        throw VBERR_BADPARAM;
    }

    hRet = this->ExH264EncSetPreProcessing();
    if(hRet != H264RET_OK){
        LOGE("ExH264EncSetPreProcessing failed.\n");
        throw VBERR_BADPARAM;
    }

    bfout = pout->GetBuffer();

    hRet = this->ExH264EncStrmStart(&bfout);
    if(hRet != H264RET_OK){
        LOGE("ExH264EncStrmStart failed.\n");
        throw VBERR_BADPARAM;
    } else {
        IsHeaderReady = true;
    }

    pout->PutBuffer(&bfout);
    this->FrameCnt = 0;
    this->refresh_cnt = 0;
    bool needNewInFrFlag = true;
    bool needNewOutFrFlag = true;
    int outFrSize;

    while(IsInState(ST::Running)) {
        this->m_stVencInfo.enState = RUN;
        try {
#if FPS_MONITOR
            if(this->IsFPSChange && this->CurFps && needNewOutFrFlag && needNewInFrFlag) {
                this->MutexSettingLock->lock();
                this->IsFPSChange = false;
                this->ExH264EncUpdateFPS();
                this->MutexSettingLock->unlock();
            }
#endif
            this->MutexSettingLock->lock();
            if(this->InfpsRatio == -1){
                this->CheckFrcUpdate();
            }
            fpsRatio =  this->InfpsRatio;
            this->MutexSettingLock->unlock();
            if(fpsRatio > 0){
                bfin = pin->GetBuffer(&bfin);
                this->m_stVencInfo.u64InCount++;
                needNewInFrFlag = false;
                for(int i = 0; i < fpsRatio; i++){
                    if(IsInState(ST::Running) == false) {
                        LOGE("frc %d exit\n", fpsRatio);
                        break;
                    }
                    if(needNewOutFrFlag){
                        bfout = pout->GetBuffer(&bfout);
                        needNewOutFrFlag = false;
                    }
                    pin->LinkReady();
                    outFrSize = bfout.fr_buf.size;
                    hRet = this->ExH264EncStrmEncode(&bfin, &bfout);
                    if(hRet < H264RET_OK){
                        bfout.fr_buf.size = outFrSize;
                        needNewOutFrFlag = false;
                        this->m_stVencInfo.u32FrameDrop++;
                        break;
                    }
                    bfout.fr_buf.timestamp = bfin.fr_buf.timestamp + this->FrameDuration * i;
                    if(this->pH264EncLocBRCfg.rc_mode == VENC_VBR_MODE)
                        this->VbrProcess(&bfout);
                    this->VideoInfo(&bfout);
                    if(bfout.fr_buf.size  <= 0){
                        needNewOutFrFlag = false;
                        bfout.fr_buf.size = outFrSize;
                        LOGE("encoder may skip & output size 0\n");
                    }
                    else{
                        pout->PutBuffer(&bfout);
                        this->m_stVencInfo.u64OutCount++;
                        needNewOutFrFlag = true;
                    }
                }
                pin->PutBuffer(&bfin);
                needNewInFrFlag = true;
            }
            else{
                fpsRatio = 0 - fpsRatio;
                for(int i = 0; i < fpsRatio; i++){
                    if(IsInState(ST::Running) == false) {
                        LOGE("frc -%d exit\n", fpsRatio);
                        break;
                    }
                    this->MutexSettingLock->lock();
                    if(this->InfpsRatio == -1){
                        this->MutexSettingLock->unlock();
                        LOGE("FPS change, break now.\n");
                        break;
                    }
                    this->MutexSettingLock->unlock();
                    bfin = pin->GetBuffer(&bfin);
                    this->m_stVencInfo.u64InCount++;
                    needNewInFrFlag = false;
                    if(i == 0){
                        if(needNewOutFrFlag){
                            bfout = pout->GetBuffer(&bfout);
                            needNewOutFrFlag = false;
                        }
                        pin->LinkReady();
                        outFrSize = bfout.fr_buf.size;
                        hRet = this->ExH264EncStrmEncode(&bfin, &bfout);
                        if(hRet < H264RET_OK){
                            LOGE("ExH264EncStrmEncode failed.\n");
                            needNewOutFrFlag = false;
                            pin->PutBuffer(&bfin);
                            needNewInFrFlag = true;
                            bfout.fr_buf.size = outFrSize;
                            this->m_stVencInfo.u32FrameDrop++;
                            break;
                        }

                        bfout.SyncStamp(&bfin);
                        if(this->pH264EncLocBRCfg.rc_mode == VENC_VBR_MODE)
                            this->VbrProcess(&bfout);
                        this->VideoInfo(&bfout);
                        if(bfout.fr_buf.size  <= 0){
                            needNewOutFrFlag = false;
                            bfout.fr_buf.size = outFrSize;
                            LOGE("encoder may skip & output size 0\n");
                        }
                        else{
                            pout->PutBuffer(&bfout);
                            this->m_stVencInfo.u64OutCount++;
                            needNewOutFrFlag = true;
                        }
                        pin->PutBuffer(&bfin);
                        needNewInFrFlag = true;
                    }
                    else{
                        if (this->TriggerKeyFrameFlag)
                        {
                            LOGE("Begin encode key frame\n");
                            pout->PutBuffer(&bfout);
                            this->m_stVencInfo.u64OutCount++;
                            if(!needNewInFrFlag)
                                pin->PutBuffer(&bfin);
                            needNewInFrFlag = true;
                            break;
                        }
                        pin->PutBuffer(&bfin);
                        needNewInFrFlag = true;
                    }
                }
            }
        }catch(const char* err){
            //LOGE("exception buf state -> in:%d out:%d\n", needNewInFrFlag, needNewOutFrFlag);
            this->m_stVencInfo.enState = IDLE;
            if(IsError(err, VBERR_FR_INTERRUPTED)) {
                LOGE("videobox interrupted out 1\n");
                break;
            }
            else if (IsError(err, VBERR_FR_NOBUFFER)){
                //LOGE("wait for buffer\n");
                usleep(5000);
            }
            else{
                //LOGE("exception -> %s\n", err);
            }
            if(needNewInFrFlag == false){
                pin->PutBuffer(&bfin);
                needNewInFrFlag = true;
            }
        }
        //When threads use SCHED_RR to schedule, need sleep to let other encode thread to get mutex lock.
        //Otherwise, other encode thread will block by cann't get lock.
        UpdateDebugInfo(1);
        usleep(1000);
    }
    this->m_stVencInfo.enState = STOP;
    UpdateDebugInfo(0);
#if 1
    //exit and return all buffer
    if(needNewInFrFlag == false){
        pin->PutBuffer(&bfin);
        needNewInFrFlag = true;
    }
#endif
    if(needNewOutFrFlag == false){
        //last error frame ,use dummy mark it
        LOGE("put dummy frame for last error\n");
        bfout.fr_buf.size  = 4;
        bfout.fr_buf.priv  = VIDEO_FRAME_DUMMY;
        pout->PutBuffer(&bfout);
        this->m_stVencInfo.u64OutCount++;
        this->m_stVencInfo.u32FrameDrop++;
    }

    needNewInFrFlag = true;
    needNewOutFrFlag = true;
}

IPU_H1264::IPU_H1264(std::string name, Json *js)
{
    this->Name = name;
    Port *pout, *pin, *pmv;
    char s8Rotate[8];
    int bufferSize = 0x240000;
    int min_buffernum = 3;
    uint32_t mvw, mvh;
    int value = 0;
    bool ratectrset = false;

    memset(&pH264LocPpCfg, 0, sizeof(h264LocPpCfg));
    memset(&this->rate_ctrl_info, 0, sizeof(struct v_rate_ctrl_info));

    this->Frc = 0;
    this->chroma_qp_offset = 2;

    if(NULL != js)
    {
        if (NULL != js->GetString("rotate"))
        {
            strncpy(s8Rotate, js->GetString("rotate"), 8);
            if (NULL != s8Rotate)
            {
                if (!strncmp(s8Rotate, "90R", 3))
                {
                    pH264LocPpCfg.rotate = H264ENC_ROTATE_90R;
                }
                else if(!strncmp(s8Rotate, "90L", 3))
                {
                    pH264LocPpCfg.rotate = H264ENC_ROTATE_90L;
                }
                else
                {
                    LOGE("Rotate Param Error Reset Default No Rotate\n");
                }
            }
        }
        if(js->GetObject("chroma_qp_offset"))
            this->chroma_qp_offset = js->GetInt("chroma_qp_offset");
        if(0 != js->GetInt("buffer_size"))
            bufferSize = js->GetInt("buffer_size");
        if(js->GetInt("min_buffernum") >=3 )
            min_buffernum = js->GetInt("min_buffernum");
        mvw = js->GetInt("mvw");
        mvh = js->GetInt("mvh");
        if(!mvw) mvw = 1920;
        if(!mvh) mvh = 1088;
        int framerate = js->GetInt("framerate");
        int framebase = js->GetInt("framebase");
        if(framerate < 0 || framebase < 0){
                LOGE("please check framerate and framebase in ipu arg,can not be negative\n");
                assert(1);
        }
        else if(framerate * framebase == 0 ){
            if( framerate > 0 || framebase > 0){//set and
                LOGE("please set framerate and framebase in ipu arg\n");
                assert(1);
            }
            else{
                LOGE("ipu arg no frc data, use default value\n");
                this->FrcInfo.framerate = 0;
                this->FrcInfo.framebase = 1;
                this->InfpsRatio = -1;
            }
        }
        else{
            this->FrcInfo.framerate = framerate;
            this->FrcInfo.framebase = framebase;
            this->InfpsRatio = -1;
        }
        this->enable_longterm = js->GetInt("enable_longterm");
        if (this->enable_longterm == 0)
        {
            // refmode is conflict with SmaptP, But SmartP with Higher Priority
            this->s32RefMode = js->GetInt("refmode");
        }
        else
        {
            this->s32RefMode = 0;
        }

        LOGE("frc value value ->%d %d\n", this->FrcInfo.framerate, this->FrcInfo.framebase);

        this->smartrc = 0;
        if ( 1 == js->GetInt("smartrc") )
        {
            if ( 0 == access("/root/.videobox/rc.json", F_OK) )
            {
                this->smartrc = 1;
                LOGE("smartrc:%d\n", this->smartrc);
                if ( !this->bSmartrcStarted )
                {
                    system("smartrc &");
                    this->bSmartrcStarted = true;
                }
            }
            else
            {
                LOGE("smartrc enabled, but no rc.json file, please add\n");
            }
        }
        LOGE("smartrc:%d \n", this->smartrc);

        if(js->GetObject("rc_mode"))
        {
            ratectrset = true;
            this->rate_ctrl_info.rc_mode = (enum v_rate_ctrl_type)js->GetInt("rc_mode");
            value = js->GetInt("gop_length");
            if(value > 0)
                this->rate_ctrl_info.gop_length = value;
            else
                this->rate_ctrl_info.gop_length = this->enable_longterm != 0 ? 150 : 15;

            value = js->GetInt("idr_interval");
            if(value > 0)
                this->rate_ctrl_info.idr_interval = value > 0 ? value : 15;
            else
                this->rate_ctrl_info.idr_interval = this->enable_longterm != 0 ? 150 : 15;

            if(VENC_CBR_MODE == this->rate_ctrl_info.rc_mode)
            {
                value =  js->GetInt("qp_max");
                this->rate_ctrl_info.cbr.qp_max = value > 0 ? value : 47;
                value = js->GetInt("qp_min");
                this->rate_ctrl_info.cbr.qp_min = value > 0 ? value : 26;
                value = js->GetInt("bitrate");
                this->rate_ctrl_info.cbr.bitrate = value > 0 ? value : 1024*1024;
                value = js->GetInt("qp_delta");
                this->rate_ctrl_info.cbr.qp_delta = value != 0 ? value : -1;
                value = js->GetInt("qp_hdr");
                this->rate_ctrl_info.cbr.qp_hdr = value != 0 ? value : -1;
            }
            else if(VENC_VBR_MODE == rate_ctrl_info.rc_mode)
            {
                value = js->GetInt("qp_max");
                this->rate_ctrl_info.vbr.qp_max = value > 0 ? value : 47;
                value = js->GetInt("qp_min");
                this->rate_ctrl_info.vbr.qp_min =  value > 0 ? value : 26;
                value =  js->GetInt("max_bitrate");
                this->rate_ctrl_info.vbr.max_bitrate = value > 0 ? value : 2*1024*1024;
                value = js->GetInt("qp_delta");
                this->rate_ctrl_info.vbr.qp_delta =  value != 0 ? value : -1;
                value =  js->GetInt("threshold");
                this->rate_ctrl_info.vbr.threshold = value > 0 ? value : 80;
            }
            else if(VENC_FIXQP_MODE == this->rate_ctrl_info.rc_mode)
            {
                value = js->GetInt("qp_fix");
                this->rate_ctrl_info.fixqp.qp_fix = value > 0 ? value : 35;
            }
        }
    }
    else{
        LOGE("ipu arg no frc data, use default value\n");
        this->FrcInfo.framerate = 0;
        this->FrcInfo.framebase = 1;
        this->InfpsRatio = -1;
        this->enable_longterm = 0;
        this->smartrc = 0;
        this->s32RefMode = 0;
    }
    this->rate_ctrl_info.mb_qp_adjustment = -1;
    if(false == ratectrset)
    {
        this->rate_ctrl_info.rc_mode = VENC_CBR_MODE;
        this->rate_ctrl_info.gop_length = this->enable_longterm != 0 ? 150 : 15;
        this->rate_ctrl_info.idr_interval = this->enable_longterm != 0 ? 150 : 15;
        this->rate_ctrl_info.cbr.qp_max = 47;
        this->rate_ctrl_info.cbr.qp_min = 26;
        this->rate_ctrl_info.cbr.bitrate =  1024*1024;
        this->rate_ctrl_info.cbr.qp_delta =  -1;
        this->rate_ctrl_info.cbr.qp_hdr = -1;
    }

    LOGE("RateCtrl rc_mode:%d gop_length:%d  idr_interval:%d qp_max:%d qp_min:%d bitrate:%d qp_delta:%d qp_hdr:%d\n",
        this->rate_ctrl_info.rc_mode ,
        this->rate_ctrl_info.gop_length,
        this->rate_ctrl_info.idr_interval,
        this->rate_ctrl_info.cbr.qp_max,
        this->rate_ctrl_info.cbr.qp_min,
        this->rate_ctrl_info.cbr.bitrate,
        this->rate_ctrl_info.cbr.qp_delta,
        this->rate_ctrl_info.cbr.qp_hdr
        );

    this->MutexSettingLock =  new std::mutex();
    pin = CreatePort("frame", Port::In);
    pin->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
    pin->SetPixelFormat(Pixel::Format::NV12);

    pout = CreatePort("stream", Port::Out);
    pout->SetBufferType(FRBuffer::Type::FLOAT, bufferSize, bufferSize / min_buffernum - 0x4000);
    pout->SetPixelFormat(Pixel::Format::H264ES);
    pout->Export();

    this->MaxBitrate = 2 * 1024 * 1024;
    this->VBRThreshHold = 80;
    this->vbr_pic_skip = 0;

    this->H264EncParamsInit();

    pmv = CreatePort("mv", Port::Out);
    pmv->SetBufferType(FRBuffer::Type::VACANT, 3);
    pmv->SetResolution(mvw, mvh);
    pmv->SetPixelFormat(Pixel::Format::MV);

    this->pH264Header = (char *)malloc(256);
    this->pH264HeaderLen = 0;
    this->InstanceCnt++;
    this->TriggerKeyFrameFlag = true;
    memset(&this->VideoScenarioBuffer, 0, sizeof(struct fr_buf_info));
    this->VideoScenarioMode = VIDEO_DAY_MODE;
    this->m_bSEIUserDataSetFlag = false;
    this->m_enSEITriggerMode = EN_SEI_TRIGGER_ONCE;
}

IPU_H1264::~IPU_H1264()
{
    delete this->MutexSettingLock;
    if(this->InstanceCnt < 0)
        return ;
    this->InstanceCnt--;

    if(this->pH264Header != NULL ){
        free(this->pH264Header);
        this->pH264Header = NULL;
    }

    if (this->m_pstSei != NULL)
    {
        free(this->m_pstSei);
        this->m_pstSei = NULL;
    }
}

void IPU_H1264::Prepare()
{
    Port *pIn = GetPort("frame");
    Buffer bfout;
    this->FrameDuration = 0;
    this->Infps = pIn->GetFPS();
    this->CurFps = pIn->GetFPS();
    this->AvgFps = pIn->GetFPS();
    this->CheckFrcUpdate();
    ParamCheck();

    pH264LocPpCfg.originW = pIn->GetWidth();
    pH264LocPpCfg.originH = pIn->GetHeight();
    pH264LocPpCfg.offsetW = pIn->GetPipWidth();
    pH264LocPpCfg.offsetH = pIn->GetPipHeight();
    pH264LocPpCfg.startX = pIn->GetPipX();
    pH264LocPpCfg.startY = pIn->GetPipY();
    if(pH264LocPpCfg.offsetW == 0)
        pH264LocPpCfg.offsetW = pH264LocPpCfg.originW;
    if(pH264LocPpCfg.offsetH == 0)
        pH264LocPpCfg.offsetH = pH264LocPpCfg.originH;
    this->sequence_quality = -1.0;
    this->total_size = 0;
    this->total_sizes = 0;
    this->total_frames = 0;
    this->pre_rtn_quant = -1;
    this->avg_framesize = 0;
    this->t_cnt = 0;
    this->t_total_size = 0;
    this->rtBps = 0;
    this->counter = 0;
    this->IsFPSChange = false;
    this->IsHeaderReady = false;
    memset(&this->VideoScenarioBuffer, 0, sizeof(struct fr_buf_info));
    this->VideoScenarioMode = VIDEO_DAY_MODE;
    this->m_enIPUType = IPU_H264;
    this->m_stVencInfo.enType = IPU_H264;
    this->m_stVencInfo.enState = INIT;
    this->m_stVencInfo.enState = STATENONE;
    this->m_stVencInfo.stRcInfo.enRCMode = RCMODNONE;
    this->m_stVencInfo.s32Width = pIn->GetWidth();
    this->m_stVencInfo.s32Height = pIn->GetHeight();
    UpdateDebugInfo(0);
    this->m_pstSei = NULL;
    this->m_enSEITriggerMode = EN_SEI_TRIGGER_ONCE;
    this->m_bSEIUserDataSetFlag = false;
    snprintf(this->m_stVencInfo.ps8Channel, CHANNEL_LENGTH, "%s-%s", Name.c_str(), "stream");
}

void IPU_H1264::Unprepare()
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
    pIpuPort = GetPort("mv");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if( this->VideoScenarioBuffer.virt_addr != NULL){
        fr_free_single(this->VideoScenarioBuffer.virt_addr,this->VideoScenarioBuffer.phys_addr);
        memset(&this->VideoScenarioBuffer,0,sizeof(struct fr_buf_info ));
    }
    if (!this->ExH264EncRelease()) {
        LOGE("release hevc instance error\n");
        throw VBERR_RUNTIME;
    }
    if (m_pstSei != NULL)
    {
        free(m_pstSei);
        m_pstSei = NULL;
    }
}

void IPU_H1264::ParamCheck()
{
    Port *pIn = GetPort("frame");
    Pixel::Format enPixelFormat = pIn->GetPixelFormat();

    if((pIn->GetWidth()%4 != 0) ||
        (pIn->GetHeight()%2 != 0) ||
        (pIn->GetPipWidth()%4 != 0) ||
        (pIn->GetPipHeight()%2 != 0) ||
        (pIn->GetWidth()<144 || pIn->GetHeight()<96) ||
        (pIn->GetWidth()>4080 || pIn->GetHeight()>4080) ||
        (pIn->GetPipX()<0 || pIn->GetPipY()<0) ||
        (pIn->GetPipWidth()<0 || pIn->GetPipHeight()<0) ||
        (pIn->GetPipX() + pIn->GetPipWidth() > pIn->GetWidth()) ||
        (pIn->GetPipY() + pIn->GetPipHeight() > pIn->GetHeight())
    ){
        LOGE("H1264 param error\n");
        throw VBERR_BADPARAM;
    }

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888 && enPixelFormat != Pixel::Format::RGB565
        && enPixelFormat != Pixel::Format::YUYV422)
    {
        LOGE("frame Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    if(GetPort("stream")->GetPixelFormat() != Pixel::Format::H264ES
        || GetPort("mv")->GetPixelFormat() != Pixel::Format::MV)
    {
        LOGE("stream or mv Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
}

void IPU_H1264::H264EncParamsInit()
{
    this->pH264EncInst = NULL;
    this->frame_mode = VENC_HEADER_NO_IN_FRAME_MODE;

    memset(&this->pH264EncCfg, 0, sizeof(H264EncConfig));
    memset(&this->pH264EncRateCtrl, 0, sizeof(H264EncRateCtrl));
    memset(&this->pH264EncPreProcCfg, 0, sizeof(H264EncPreProcessingCfg));
    memset(&this->pH264EncCodingCfg, 0, sizeof(H264EncCodingCtrl));
    memset(&this->pH264EncLocBasicCfg, 0, sizeof(v_basic_info));
    memset(&this->pH264EncLocBRCfg, 0, sizeof(v_bitrate_info));
    memset(&this->pH264EncLocRoiAreaInfo, 0, sizeof(v_roi_info));

    this->pH264EncLocBasicCfg.stream_type = VENC_BYTESTREAM;
    this->pH264InFrameType = Pixel::Format::NV12;

    this->pH264EncCfg.width = 1920;
    this->pH264EncCfg.height = 1080;
    this->pH264EncCfg.frameRateDenom = 1;
    this->pH264EncCfg.frameRateNum = 30;
    this->pH264EncCfg.streamType = H264ENC_BYTE_STREAM;
    this->pH264EncCfg.level = H264ENC_LEVEL_5_1;
    this->pH264EncLocBRCfg.qp_hdr = -1;
    m_s32RateCtrlDistance = 0;
    m_u8RateCtrlReset = 0;
    this->CurFps = this->pH264EncCfg.frameRateNum / this->pH264EncCfg.frameRateDenom;

    if(SetRateCtrl(&rate_ctrl_info)  != 0)
    {
        LOGE("ratectrl parameters is not right,please check json arg reset to default\n");
        this->pH264EncLocBRCfg.qp_max = 42;
        this->pH264EncLocBRCfg.qp_min = 26;
        this->pH264EncLocBRCfg.qp_hdr = -1;
        this->pH264EncLocBRCfg.qp_delta = -1;
        this->pH264EncLocBRCfg.bitrate = 1024*1024;//*1024;
        this->pH264EncLocBRCfg.picture_skip = 0;
        this->pH264EncLocBRCfg.gop_length = this->enable_longterm != 0 ? 150 : 15;;
        this->pH264EncLocBRCfg.p_frame = this->enable_longterm != 0 ? 150 : 15;;
        this->pH264EncLocBRCfg.rc_mode = VENC_CBR_MODE;
    }
    this->pIdrInterval = this->pH264EncLocBRCfg.p_frame;
    if(this->enable_longterm == 1){
        this->pH264EncCfg.refFrameAmount = 2;
        this->refresh_interval = 15;
        this->pH264EncCfg.viewMode = H264ENC_BASE_VIEW_MULTI_BUFFER;
    }
    else{
        this->pH264EncCfg.refFrameAmount = 1;
        this->refresh_interval = 0;
        this->pH264EncCfg.viewMode = H264ENC_BASE_VIEW_DOUBLE_BUFFER;
    }
    this->H264EncCfgUpdateFlag = true;
    this->H264EncRateCtrlUpateFlag = true;
    this->H264EncCodingCtrlUpdateFlag = true;
}

int IPU_H1264::ExH264EncInit()
{
    H264EncRet hRet;
    Port *pin = NULL;

    pin = GetPort("frame");

    this->Infps = pin->GetFPS();
    this->pH264EncRateCtrl.gopLen = this->pH264EncLocBRCfg.gop_length;
    this->pH264EncCfg.frameRateNum = this->Infps;
    this->pH264EncCfg.frameRateDenom = 1;
    this->pH264InFrameType = pin->GetPixelFormat();
    this->pH264EncCfg.width = pH264LocPpCfg.offsetW;
    this->pH264EncCfg.height = pH264LocPpCfg.offsetH;
    if(pH264LocPpCfg.rotate != 0){
        this->pH264EncCfg.width = this->pH264LocPpCfg.offsetH;
        this->pH264EncCfg.height = this->pH264LocPpCfg.offsetW;
    }
    this->pH264EncCfg.linkMode = pin->LinkEnabled();
    this->MutexSettingLock->lock();
    if(this->H264EncCfgUpdateFlag){
        this->pH264EncCfg.streamType = (H264EncStreamType)this->pH264EncLocBasicCfg.stream_type;
        this->pH264EncCfg.frameRateNum = this->Infps;
        this->pH264EncCfg.frameRateDenom = 1;
        LOGE("set h264 inst to %d %d\n", this->pH264EncCfg.frameRateNum, this->InfpsRatio);
    }
    this->MutexSettingLock->unlock();
    this->pH264EncCfg.chromaQpIndexOffset = this->chroma_qp_offset;
    hRet = H264EncInitSyn(&this->pH264EncCfg, &this->pH264EncInst);
    if(hRet != H264ENC_OK){
        LOGE("H264Enc Init failed. err code:%i\n", hRet);
        return hRet;
    }
    return hRet;
}

void IPU_H1264::ExH264EncUpdateFPS()
{
    int hRet;
    Buffer bfin, bfout;
    Port *pin, *pout;

    pin = GetPort("frame");
    pout = GetPort("stream");

    hRet = ExH264EncStrmEnd(pin, pout);
    if(hRet != H264RET_OK){
        LOGE("ExH264EncStrmEnd failed.\n");
        throw VBERR_BADPARAM;
    }

    if(!this->ExH264EncRelease()) {
        LOGE("ExH264EncRelease error\n");
    }

    while(this->pH264EncInst) {
        LOGE("pH264EncInst is not NULL\n");
        usleep(20000);
    }

    this->pH264EncCfg.frameRateDenom = 1;
    this->pH264EncCfg.frameRateNum = this->CurFps;
    hRet = H264EncInitSyn(&this->pH264EncCfg, &this->pH264EncInst);
    if(hRet != H264ENC_OK){
        LOGE("H264Enc Init failed. err code:%i\n", hRet);
        throw VBERR_BADPARAM;
    }

    hRet = this->ExH264EncSetCodingCtrl();
    if(hRet != H264RET_OK){
        LOGE("ExH264EncSetCodingCtrl failed.\n");
        throw VBERR_BADPARAM;
    }

    hRet = this->ExH264EncSetRateCtrl();
    if(hRet != H264RET_OK){
        LOGE("ExH264EncSetRateCtrl failed.\n");
        throw VBERR_BADPARAM;
    }

    hRet = this->ExH264EncSetPreProcessing();
    if(hRet != H264RET_OK){
        LOGE("ExH264EncSetPreProcessing failed.\n");
        throw VBERR_BADPARAM;
    }

    bfout = pout->GetBuffer();
    hRet = this->ExH264EncStrmStart(&bfout);
    if(hRet != H264RET_OK){
        LOGE("ExH264EncStrmStart failed.\n");
        throw VBERR_BADPARAM;
    }
    pout->PutBuffer(&bfout);
}

int IPU_H1264::ExH264EncSetRateCtrl()
{
    H264EncRet hRet;

    hRet = H264EncGetRateCtrl(this->pH264EncInst, &this->pH264EncRateCtrl);
    if(hRet != H264ENC_OK){
        LOGE("H264Enc GetRateCtrl failed, err code:%i", hRet);
        return hRet;
    }

    this->pH264EncRateCtrl.mbRc = 0; //h1264 not support mbRc
    this->pH264EncRateCtrl.hrd = this->pH264EncLocBRCfg.hrd;
    this->pH264EncRateCtrl.hrdCpbSize = this->pH264EncLocBRCfg.hrd_cpbsize;
    this->pH264EncRateCtrl.mbQpAdjustment = this->pH264EncLocBRCfg.mb_qp_adjustment;
    this->pIdrInterval = this->pH264EncLocBRCfg.p_frame;
    this->pH264EncRateCtrl.pictureSkip = this->pH264EncLocBRCfg.picture_skip;
    this->pH264EncRateCtrl.gopLen = this->pH264EncLocBRCfg.gop_length;
    if (this->pH264EncCfg.refFrameAmount == 2)
    {
        this->pH264EncRateCtrl.longTermPicRate = 1;
    }
    else
    {
        this->pH264EncRateCtrl.longTermPicRate = 0;
    }

    /* patch for reset ratectrl window to accelate smooth qp only user setting available
        qp_max threshold 42 and qp_max diff threshold 3 are pre-set value that can not cover all cases
    */
    if((pH264EncLocBRCfg.qp_max >= 42 && (u32)pH264EncLocBRCfg.qp_max > pH264EncRateCtrl.qpMax
        && (u32)pH264EncLocBRCfg.qp_max - pH264EncRateCtrl.qpMax >= 3
        && m_s32RateCtrlDistance > pH264EncRateCtrl.gopLen && m_u8RateCtrlReset == 1)
        ||(m_u8RateCtrlReset == 2))
    {
        pH264EncRateCtrl.u32NewStream = 1;
    }
    else
    {
        pH264EncRateCtrl.u32NewStream = 0;
    }
    m_u8RateCtrlReset = 0;
    m_s32RateCtrlDistance = 0;

    if(this->pH264EncLocBRCfg.rc_mode == VENC_CBR_MODE ||
            this->pH264EncLocBRCfg.rc_mode == VENC_FIXQP_MODE) {
        this->pH264EncRateCtrl.pictureRc = 1;
        this->pH264EncRateCtrl.qpMax= this->pH264EncLocBRCfg.qp_max;
        this->pH264EncRateCtrl.qpMin = this->pH264EncLocBRCfg.qp_min;
        this->pH264EncRateCtrl.qpHdr = this->pH264EncLocBRCfg.qp_hdr;
        this->pH264EncRateCtrl.intraQpDelta = this->pH264EncLocBRCfg.qp_delta;
        this->pH264EncRateCtrl.fixedIntraQp = 0;
        this->pH264EncRateCtrl.bitPerSecond = this->pH264EncLocBRCfg.bitrate;
    }else if(this->pH264EncLocBRCfg.rc_mode == VENC_VBR_MODE) {
        this->pH264EncRateCtrl.pictureRc = 0;
        this->pH264EncRateCtrl.qpMax = this->pH264EncLocBRCfg.qp_max;
        this->pH264EncRateCtrl.qpMin = this->pH264EncLocBRCfg.qp_min;
        if(this->pH264EncLocBRCfg.qp_hdr == -1)
        {
            H264EncCalculateInitialQp(this->pH264EncInst, &this->pH264EncLocBRCfg.qp_hdr, this->MaxBitrate * (this->VBRThreshHold + 100)/2/100);
            if(this->pH264EncLocBRCfg.qp_hdr > this->pH264EncLocBRCfg.qp_max)
            {
                this->pH264EncLocBRCfg.qp_hdr = this->pH264EncLocBRCfg.qp_max;
            }
            else if(this->pH264EncLocBRCfg.qp_hdr < this->pH264EncLocBRCfg.qp_min)
            {
                this->pH264EncLocBRCfg.qp_hdr = this->pH264EncLocBRCfg.qp_min;
            }
        }
        if(this->sequence_quality == -1.0)
            this->sequence_quality = 2.0 / this->pH264EncLocBRCfg.qp_hdr;
        if(this->pre_rtn_quant == -1)
            this->pre_rtn_quant = this->pH264EncLocBRCfg.qp_hdr;
        this->pH264EncRateCtrl.qpHdr = this->pH264EncLocBRCfg.qp_hdr;
        this->pH264EncRateCtrl.intraQpDelta = this->pH264EncLocBRCfg.qp_delta;
        this->pH264EncRateCtrl.fixedIntraQp = 0;
        if(this->vbr_pic_skip)
            this->pH264EncRateCtrl.bitPerSecond = this->MaxBitrate;
    }

    if(this->pH264EncRateCtrl.bitPerSecond < BITRATE_MIN)
        this->pH264EncRateCtrl.bitPerSecond = BITRATE_MIN;
    if(this->pH264EncRateCtrl.bitPerSecond > BITRATE_MAX)
        this->pH264EncRateCtrl.bitPerSecond = BITRATE_MAX;

    hRet = H264EncSetRateCtrl(this->pH264EncInst, &this->pH264EncRateCtrl);
    if(hRet != H264ENC_OK){
        LOGE("H264Enc SetRateCtrl failed, err code:%i\n", hRet);
        return hRet;
    }

    return hRet;
}

int IPU_H1264::ExH264EncSetCodingCtrl()
{
    H264EncRet hRet;
    u32 u32MbPerCol = 0, u32MbPerRow = 0;

    hRet = H264EncGetCodingCtrl(this->pH264EncInst, &this->pH264EncCodingCfg);
    if(hRet != H264ENC_OK){
        LOGE("H264EncGetRateCtrl failed. err Code:%i\n", hRet);
        return hRet;
    }

    u32MbPerRow = (this->pH264EncCfg.width + 15) / 16;
    u32MbPerCol = (this->pH264EncCfg.height + 15) / 16;

    this->pH264EncCodingCfg.sliceSize = 0 ;
    this->pH264EncCodingCfg.disableDeblockingFilter = 0;
    this->pH264EncCodingCfg.videoFullRange = 0;
    this->pH264EncCodingCfg.fieldOrder = 0;

    this->pH264EncCodingCfg.cirStart = 0;
    this->pH264EncCodingCfg.cirInterval = 0;

    this->pH264EncCodingCfg.intraArea.enable = 0;
    this->pH264EncCodingCfg.intraArea.top = -1;
    this->pH264EncCodingCfg.intraArea.left = -1;
    this->pH264EncCodingCfg.intraArea.bottom = -1;
    this->pH264EncCodingCfg.intraArea.right = -1;

    this->pH264EncCodingCfg.roi1Area.top = (this->pH264EncLocRoiAreaInfo.roi[0].y) >> 4;
    this->pH264EncCodingCfg.roi1Area.left = (this->pH264EncLocRoiAreaInfo.roi[0].x) >> 4;
    this->pH264EncCodingCfg.roi1Area.right = (this->pH264EncLocRoiAreaInfo.roi[0].w + this->pH264EncLocRoiAreaInfo.roi[0].x) >> 4;
    this->pH264EncCodingCfg.roi1Area.bottom = (this->pH264EncLocRoiAreaInfo.roi[0].h + this->pH264EncLocRoiAreaInfo.roi[0].y) >> 4;
    this->pH264EncCodingCfg.roi1Area.enable = this->pH264EncLocRoiAreaInfo.roi[0].enable;
    pH264EncCodingCfg.roi1Area.right = pH264EncCodingCfg.roi1Area.right >= u32MbPerRow ?
        u32MbPerRow - 1 : pH264EncCodingCfg.roi1Area.right;
    pH264EncCodingCfg.roi1Area.bottom = pH264EncCodingCfg.roi1Area.bottom >= u32MbPerCol ?
        u32MbPerCol - 1 : pH264EncCodingCfg.roi1Area.bottom;

    this->pH264EncCodingCfg.roi2Area.top = (this->pH264EncLocRoiAreaInfo.roi[1].y) >> 4;
    this->pH264EncCodingCfg.roi2Area.left = (this->pH264EncLocRoiAreaInfo.roi[1].x) >> 4;
    this->pH264EncCodingCfg.roi2Area.right = (this->pH264EncLocRoiAreaInfo.roi[1].w + this->pH264EncLocRoiAreaInfo.roi[1].x) >> 4;
    this->pH264EncCodingCfg.roi2Area.bottom = (this->pH264EncLocRoiAreaInfo.roi[1].h + this->pH264EncLocRoiAreaInfo.roi[1].y) >> 4;
    this->pH264EncCodingCfg.roi2Area.enable = this->pH264EncLocRoiAreaInfo.roi[1].enable;
    pH264EncCodingCfg.roi2Area.right = pH264EncCodingCfg.roi2Area.right >= u32MbPerRow ?
        u32MbPerRow - 1 : pH264EncCodingCfg.roi2Area.right;
    pH264EncCodingCfg.roi2Area.bottom= pH264EncCodingCfg.roi2Area.bottom >= u32MbPerCol ?
        u32MbPerCol - 1 : pH264EncCodingCfg.roi2Area.bottom;

    this->pH264EncCodingCfg.roi1DeltaQp = this->pH264EncLocRoiAreaInfo.roi[0].qp_offset;
    this->pH264EncCodingCfg.roi2DeltaQp = this->pH264EncLocRoiAreaInfo.roi[1].qp_offset;

    this->pH264EncCodingCfg.seiMessages = 0;

    this->pH264EncCodingCfg.videoFullRange = 1;

    hRet = H264EncSetCodingCtrl(this->pH264EncInst, &this->pH264EncCodingCfg);
    if(hRet != H264ENC_OK){
        LOGE("H264EncSetCodingCtrl failed. err Code:%i\n", hRet);
        return hRet;
    }

    return hRet;
}

int IPU_H1264::ExH264EncSetPreProcessing()
{
    H264EncRet hRet;
    hRet = H264EncGetPreProcessingSyn(this->pH264EncInst, &pH264EncPreProcCfg);
    if(hRet != H264ENC_OK){
        LOGE("H264EncGetPreProcessing failed. Error code: %8i\n", hRet);
        return hRet;
    }

    this->pH264EncPreProcCfg.inputType = (H264EncPictureType)SwitchFormat(GetPort("frame")->GetPixelFormat());
    this->pH264EncPreProcCfg.origWidth = pH264LocPpCfg.originW;
    this->pH264EncPreProcCfg.origHeight = pH264LocPpCfg.originH;
    this->pH264EncPreProcCfg.xOffset = pH264LocPpCfg.startX;
    this->pH264EncPreProcCfg.yOffset = pH264LocPpCfg.startY;
    this->pH264EncPreProcCfg.scaledOutput = 0;
    this->pH264EncPreProcCfg.rotation = (H264EncPictureRotation)pH264LocPpCfg.rotate;
    this->pH264EncPreProcCfg.videoStabilization = 0;
    this->pH264EncPreProcCfg.interlacedFrame = 0;
    this->pH264EncPreProcCfg.colorConversion.type = H264ENC_RGBTOYUV_BT601;
    hRet = H264EncSetPreProcessingSyn(this->pH264EncInst, &pH264EncPreProcCfg);
    if(hRet != H264ENC_OK){
        LOGE("H264EncSetPreProcessing failed. Error code: %8i\n", hRet);
        return hRet;
    }

    return hRet;
}

int IPU_H1264::ExH264EncStrmStart(Buffer *bfout)
{
    H264EncRet hRet;

    this->pH264EncIn.pOutBuf = (u32 *)bfout->fr_buf.virt_addr;
    this->pH264EncIn.busOutBuf = bfout->fr_buf.phys_addr;
    this->pH264EncIn.outBufSize = bfout->fr_buf.size;


    hRet = H264EncStrmStartSyn(this->pH264EncInst, &this->pH264EncIn, &this->pH264EncOut);
    if(hRet != H264ENC_OK){
        LOGE("H264EncStrmStart failed. Error code: %8i\n", hRet);
        return hRet;
    }

    bfout->fr_buf.size = this->pH264EncOut.streamSize;
    this->H264SaveHeader((char *)bfout->fr_buf.virt_addr, bfout->fr_buf.size);

    return hRet;
}

int IPU_H1264::ExH264EncStrmEncode(Buffer *bfIn, Buffer *bfOut)
{
    int picSize = 0, AlignSize = 0, offset = 0;
    H264EncRet hRet;
    Port *pin;
    Port *pmv;
    Buffer bmv;
    bool bSetSEI = false;
    bool bIsIntraFrame = false;

    pin = GetPort("frame");
    pmv = GetPort("mv");
    picSize = CALYUVSIZE(pin->GetWidth(), pin->GetHeight());
    this->pH264EncIn.busLuma = bfIn->fr_buf.phys_addr;
    this->pH264EncIn.busChromaU = this->pH264EncIn.busLuma + picSize;
    this->pH264EncIn.busChromaV = this->pH264EncIn.busChromaU + picSize / 4;
    this->pH264EncIn.pOutBuf = (u32 *)bfOut->fr_buf.virt_addr;
    this->pH264EncIn.busOutBuf = bfOut->fr_buf.phys_addr;
    this->pH264EncIn.outBufSize = bfOut->fr_buf.size;

    if(this->FrameCnt != 0)
        this->pH264EncIn.timeIncrement = this->pH264EncCfg.frameRateDenom;
    else
        this->pH264EncIn.timeIncrement = 0;

    this->MutexSettingLock->lock();
    H264LinkMode(this->pH264EncInst, this->pH264EncCfg.linkMode);
    if(this->H264EncCodingCtrlUpdateFlag){
        this->ExH264EncSetCodingCtrl();
        this->H264EncCodingCtrlUpdateFlag = false;
    }

    if(this->H264EncRateCtrlUpateFlag){
        this->ExH264EncSetRateCtrl();
        this->H264EncRateCtrlUpateFlag = false;
    }

    this->MutexSettingLock->unlock();

    //longterm reference case use this->TriggerKeyFrameFlag dynamic set IDR
    //update IDR and longterm reference every time  with pIdrInterval temprary,
    //future may be base on background update algorithm
    if((this->FrameCnt % this->pIdrInterval )== 0 && this->pH264EncCfg.refFrameAmount > 1)
        this->TriggerKeyFrameFlag = true;

    if (((this->FrameCnt % this->pIdrInterval == 0 &&  this->pH264EncCfg.refFrameAmount == 1) || this->TriggerKeyFrameFlag )) {
        this->pH264EncIn.codingType = H264ENC_INTRA_FRAME;
        bfOut->fr_buf.priv = VIDEO_FRAME_I;
        this->pH264EncIn.ipf = H264ENC_NO_REFERENCE_NO_REFRESH;
        this->pH264EncIn.ltrf = H264ENC_NO_REFERENCE_NO_REFRESH;
        this->TriggerKeyFrameFlag = false;

        if(frame_mode == VENC_HEADER_IN_FRAME_MODE){
            AlignSize = (pH264HeaderLen + 0x7) & (~0x7);
            offset = AlignSize - pH264HeaderLen;
            memcpy(((char *)bfOut->fr_buf.virt_addr + offset), pH264Header, pH264HeaderLen);
            memset((char *)bfOut->fr_buf.virt_addr, 0, offset);
            this->pH264EncIn.pOutBuf = (u32 *)((unsigned int)bfOut->fr_buf.virt_addr + AlignSize);
            this->pH264EncIn.busOutBuf = bfOut->fr_buf.phys_addr + AlignSize;
            this->pH264EncIn.outBufSize = bfOut->fr_buf.size - AlignSize;
        }
    }
    else if(this->refresh_interval != 0 && this->refresh_cnt % this->refresh_interval == 0 &&
            this->pH264EncCfg.refFrameAmount > 1)
    {
        this->pH264EncIn.codingType = H264ENC_PREDICTED_FRAME;
        bfOut->fr_buf.priv = VIDEO_FRAME_P;
        this->pH264EncIn.ipf = H264ENC_REFRESH;
        this->pH264EncIn.ltrf = H264ENC_REFERENCE;
    }
    else if(this->FrameCnt % 2 == 1 && this->s32RefMode == 1)
    {
        this->pH264EncIn.codingType = H264ENC_PREDICTED_FRAME;
        bfOut->fr_buf.priv = VIDEO_FRAME_P;
        this->pH264EncIn.ipf = H264ENC_REFERENCE;
        this->pH264EncIn.ltrf = H264ENC_NO_REFERENCE_NO_REFRESH;
    }
    else {
        this->pH264EncIn.codingType = H264ENC_PREDICTED_FRAME;
        bfOut->fr_buf.priv = VIDEO_FRAME_P;
        this->pH264EncIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
        this->pH264EncIn.ltrf = H264ENC_REFERENCE;
    }

    //fake uv data
    if(this->VideoScenarioMode == VIDEO_NIGHT_MODE){
        int uvSize = 0 ;
        switch( this->pH264InFrameType){
            case Pixel::Format::NV12 :
            case Pixel::Format::NV21 :
                LOGD("video scenario nv12/nv21 \n");
                uvSize =  this->pH264LocPpCfg.originW * this->pH264LocPpCfg.originH / 2;
                break;
            case Pixel::Format::YUYV422 :
                LOGD("video scenario yuyv422 \n");
                uvSize =  this->pH264LocPpCfg.originW * this->pH264LocPpCfg.originH;
                break;
            default :
                LOGE("video scenario  not support -> %d \n", this->pH264InFrameType);
                throw VBERR_BADPARAM;
        }
        if(this->VideoScenarioBuffer.virt_addr == NULL){
            char buf[48];
            sprintf(buf,"%s_scenario",this->Name.c_str());
            LOGE("video scenario->%s %d\n",buf,uvSize);
            this->VideoScenarioBuffer.virt_addr =  fr_alloc_single(buf,uvSize, &this->VideoScenarioBuffer.phys_addr);
            memset(this->VideoScenarioBuffer.virt_addr,0x80,uvSize);
            if(this->VideoScenarioBuffer.virt_addr == NULL){
                LOGE("malloc linear scenario buffer error\n");
                throw VBERR_RUNTIME;
            }
            this->VideoScenarioBuffer.size = uvSize ;
        }
        uvSize =  uvSize / 2;
        this->pH264EncIn.busChromaU = this->VideoScenarioBuffer.phys_addr;
        this->pH264EncIn.busChromaV = this->VideoScenarioBuffer.phys_addr + uvSize ;
    }

    this->MutexSettingLock->lock();
    if (this->pH264EncIn.codingType == H264ENC_INTRA_FRAME)
    {
        bIsIntraFrame = true;
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
        if (H264ENC_OK != (H264EncSetSeiUserData(this->pH264EncInst, (const u8 *)m_pstSei->seiUserData, m_pstSei->length)))
        {
            LOGE("set sei user data error\n");
        }
        m_bSEIUserDataSetFlag = true;
        if (m_enSEITriggerMode == EN_SEI_TRIGGER_ONCE)
        {
            m_pstSei->length = 0;
        }
    }
    this->MutexSettingLock->unlock();


    hRet = H264EncStrmEncodeSyn(this->pH264EncInst, &this->pH264EncIn, &this->pH264EncOut, NULL, NULL);
    if(hRet < H264ENC_OK){
            LOGE("h264 error %d in  bus L:0x%8X U:%X V:%5d\n",
                    hRet,
                this->pH264EncIn.busLuma,
                this->pH264EncIn.busChromaU,
                this->pH264EncIn.busChromaV);
            LOGE("h264 error %d out bus:0x%8X vir:%X size:%5d\n",
                    hRet,
                this->pH264EncIn.busOutBuf,
                this->pH264EncIn.pOutBuf,
                this->pH264EncIn.outBufSize);
            this->FrameCnt = 0;
        return hRet;
    }
    switch (hRet) {
        case H264ENC_FRAME_READY:
            this->MutexSettingLock->lock();
            // Clear SEI setting by set length to zero
            if (bSetSEI)
            {
                if (m_enSEITriggerMode != EN_SEI_TRIGGER_EVERY_FRAME)
                {
                    if (H264ENC_OK != (H264EncSetSeiUserData(this->pH264EncInst,\
                            (const u8 *)m_pstSei->seiUserData, 0)))
                    {
                        LOGE("clear sei user data error\n");
                    }
                }
                this->m_bSEIUserDataSetFlag = false;
                if ((m_pstSei) && (m_pstSei->length == 0))
                {
                    free(this->m_pstSei);
                    this->m_pstSei = NULL;
                }
            }
            m_s32RateCtrlDistance++;
            this->MutexSettingLock->unlock();
            this->FrameCnt++;
            if((this->pH264EncIn.codingType == H264ENC_INTRA_FRAME) && (frame_mode == VENC_HEADER_IN_FRAME_MODE)) {
                bfOut->fr_buf.size = this->pH264EncOut.streamSize + AlignSize;
            } else
                bfOut->fr_buf.size = this->pH264EncOut.streamSize;
            if(this->pH264EncIn.codingType == H264ENC_INTRA_FRAME)
                this->refresh_cnt = 1;
            else
                this->refresh_cnt++;

            if(pmv->IsEnabled()) {
                bmv = pmv->GetBuffer();
                bmv.fr_buf.phys_addr = (uint32_t)this->pH264EncOut.motionVectors;
                bmv.SyncStamp(bfIn);
                pmv->PutBuffer(&bmv);
            }

            // save steam
            //if (this->EncConfig.streamType == HEVCENC_BYTE_STREAM) {
                // save byte stream
            //} else if (this->EncConfig.streamType == HEVCENC_NAL_UNIT_STREAM) {
                // save nal stream
            //} else {
            //    Log_debug("wrong stream type\n");
            //    return VIDEOBOX_RET_ERROR;
            //}
            break;
        case H264ENC_OK:
            // do nothing
            break;
        default:
            LOGE("h264 encode warn ->%d\n", hRet);
            break;
    }

    return hRet;
}

int IPU_H1264::ExH264EncStrmEnd(Port *pin, Port *pout)
{
    H264EncRet hRet;
    Buffer bfin, bfout;

    bfin = pin->GetBuffer(&bfin);
    bfout = pout->GetBuffer();

    this->pH264EncIn.busLuma = bfin.fr_buf.phys_addr;
    this->pH264EncIn.busChromaU = bfin.fr_buf.phys_addr + (pin->GetWidth()*pin->GetHeight());
    this->pH264EncIn.busChromaV = 0;
    this->pH264EncIn.timeIncrement = 0;
    this->pH264EncIn.pOutBuf = (u32 *)bfout.fr_buf.virt_addr;
    this->pH264EncIn.busOutBuf = bfout.fr_buf.phys_addr;

    this->pH264EncOut.pNaluSizeBuf = (u32 *)bfout.fr_buf.virt_addr;
    hRet = H264EncStrmEndSyn(this->pH264EncInst, &this->pH264EncIn, &this->pH264EncOut);
    if(hRet != H264ENC_OK){
        LOGE("H264EncStrmStart failed. err Code:%i\n", hRet);
        return hRet;
    }
    bfout.fr_buf.size = this->pH264EncOut.streamSize;

    pout->PutBuffer(&bfout);
    pin->PutBuffer(&bfin);

    return hRet;
}

bool IPU_H1264::ExH264EncRelease()
{
    H264EncRet hRet;

    hRet = H264EncRelease(this->pH264EncInst);
    if(hRet != H264ENC_OK){
        LOGE("H264EncRelease failed. err Code:%i\n", hRet);
        return false;
    }

    this->pH264EncInst = NULL;
    return true;
}

bool IPU_H1264::H264SaveHeader(char *buf, int len)
{
    LOGD("H1 header saved %d bytes\n", len);

    memcpy(this->pH264Header, buf, len);
    this->pH264HeaderLen = len;

    return true;
}

int IPU_H1264::GetBasicInfo(struct v_basic_info *cfg)
{
    this->MutexSettingLock->lock();
    this->pH264EncLocBasicCfg.media_type = VIDEO_MEDIA_H264;
    *cfg = this->pH264EncLocBasicCfg;
    this->MutexSettingLock->unlock();

    return 0;
}


int IPU_H1264::SetScenario(enum video_scenario mode){
    LOGE("%s->line:%d ->mode:%d\n",__FUNCTION__,__LINE__,mode);
    switch( this->pH264InFrameType){
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
            LOGE("input src type -> %d ,not support scenario set ,defualt day mode \n",  this->pH264InFrameType);
            return VBEBADPARAM;
    }
    this->MutexSettingLock->lock();
    this->VideoScenarioMode = mode;
    this->MutexSettingLock->unlock();
    return 1;
}

enum video_scenario IPU_H1264::GetScenario(void){
    return this->VideoScenarioMode;
}

int IPU_H1264::SetBasicInfo(struct v_basic_info *cfg)
{

    this->MutexSettingLock->lock();
    this->H264EncCfgUpdateFlag = false;
    this->pH264EncLocBasicCfg = *cfg;
    this->pH264EncLocBasicCfg.media_type = VIDEO_MEDIA_H264;
    this->H264EncCfgUpdateFlag = true;
    this->MutexSettingLock->unlock();

    return 0;
}

int IPU_H1264::GetBitrate(struct v_bitrate_info *cfg)
{
    this->MutexSettingLock->lock();
    *cfg = this->pH264EncLocBRCfg;
    if(cfg->rc_mode == VENC_VBR_MODE){
        LOGE("vbr mode ,set bitrate = 0,will add auto cal later\n");
        cfg->bitrate = 0;
    }
    this->MutexSettingLock->unlock();

    return 0;
}

int IPU_H1264::GetRTBps()
{
    return this->rtBps;
}

/* not maintain from 10.23.2017 */
int IPU_H1264::SetBitrate(struct v_bitrate_info *cfg)
{
    if(!cfg->rc_mode && (cfg->bitrate < BITRATE_MIN || cfg->bitrate > BITRATE_MAX)){
        LOGE("bitrate set error,range[%d, %d]\n", BITRATE_MIN, BITRATE_MAX);
        return VBEBADPARAM;
    }
    if(cfg->rc_mode != VENC_CBR_MODE && cfg->rc_mode != VENC_VBR_MODE){
        LOGE("rc_mode set error, VENC_CBR_MODE or VENC_VBR_MODE\n");
        return VBEBADPARAM;
    }
    if(cfg->gop_length < 1 || cfg->gop_length > 150){
        LOGE("gop_length set error,range[1, 150]\n");
        return VBEBADPARAM;
    }
    if(cfg->qp_delta < -12 || cfg->qp_delta > 12){
        LOGE("qp_delta set error, range[-12, 12]\n");
        return VBEBADPARAM;
    }
    if(cfg->qp_max > 51 || cfg->qp_max < 0 ||
        cfg->qp_min > 51 || cfg->qp_min < 0){
        LOGE("qp value set error, range[0,51]\n");
        return VBEBADPARAM;
    }
    if(cfg->p_frame == 0){
        cfg->p_frame = this->pH264EncLocBRCfg.p_frame;
    }

    if(this->enable_longterm == 1){
        this->refresh_interval = cfg->refresh_interval;
    }
    else{
        this->refresh_interval = 0;
    }

    this->MutexSettingLock->lock();
    this->H264EncRateCtrlUpateFlag = false;
    if(this->pH264EncLocBRCfg.rc_mode != cfg->rc_mode)
    {
        m_u8RateCtrlReset = 2;
    }
    this->pH264EncLocBRCfg = *cfg;
    this->counter = 0;
    this->H264EncRateCtrlUpateFlag = true;

    this->MutexSettingLock->unlock();

    return 0;
}

int IPU_H1264::GetRateCtrl(struct v_rate_ctrl_info *cfg)
{
    if(cfg == NULL) {
        LOGE("invalid param\n");
        return VBEBADPARAM;
    } else {
        memset(cfg, 0, sizeof(struct v_rate_ctrl_info));
    }
    this->MutexSettingLock->lock();
    switch (this->pH264EncLocBRCfg.rc_mode) {
        case VENC_CBR_MODE:
            cfg->cbr.qp_max = this->pH264EncLocBRCfg.qp_max;
            cfg->cbr.qp_min = this->pH264EncLocBRCfg.qp_min;
            cfg->cbr.qp_delta = this->pH264EncLocBRCfg.qp_delta;
            cfg->cbr.qp_hdr = this->pH264EncLocBRCfg.qp_hdr;
            cfg->cbr.bitrate = this->pH264EncLocBRCfg.bitrate;
            cfg->picture_skip = this->pH264EncLocBRCfg.picture_skip;
            break;
        case VENC_VBR_MODE:
            cfg->vbr.qp_max = this->pH264EncLocBRCfg.qp_max;
            cfg->vbr.qp_min = this->pH264EncLocBRCfg.qp_min;
            cfg->vbr.qp_delta = this->pH264EncLocBRCfg.qp_delta;
            cfg->vbr.max_bitrate = this->MaxBitrate;
            cfg->vbr.threshold = this->VBRThreshHold;
            cfg->picture_skip = this->vbr_pic_skip;
            break;
        case VENC_FIXQP_MODE:
            cfg->fixqp.qp_fix = this->pH264EncLocBRCfg.qp_hdr;
            cfg->picture_skip = this->pH264EncLocBRCfg.picture_skip;
            break;
        default:
            LOGE("invalid rc mode\n");
            break;
    }
    cfg->rc_mode = this->pH264EncLocBRCfg.rc_mode;
    cfg->gop_length = this->pH264EncLocBRCfg.gop_length;
    cfg->idr_interval = this->pH264EncLocBRCfg.p_frame;
    cfg->hrd = this->pH264EncLocBRCfg.hrd;
    cfg->hrd_cpbsize = this->pH264EncLocBRCfg.hrd_cpbsize;
    cfg->refresh_interval = this->refresh_interval;
    cfg->mbrc = this->pH264EncLocBRCfg.mbrc;
    cfg->mb_qp_adjustment = this->pH264EncLocBRCfg.mb_qp_adjustment;
    this->MutexSettingLock->unlock();
    return 0;
}

int IPU_H1264::SetRateCtrl(struct v_rate_ctrl_info *cfg)
{
    if(!cfg || cfg->rc_mode < VENC_CBR_MODE || cfg->rc_mode > VENC_FIXQP_MODE || cfg->idr_interval <= 0 ||
        cfg->gop_length < 1 || cfg->gop_length > 150 || cfg->picture_skip < 0 || cfg->picture_skip > 1 ||
        cfg->hrd < 0 || cfg->hrd > 1 || cfg->hrd_cpbsize < 0 || cfg->mbrc != 0 ||
        cfg->mb_qp_adjustment < -8 || cfg->mb_qp_adjustment > 7) {
        LOGE("invalid rate ctrl param\n");
        return VBEBADPARAM;
    }
    if(cfg->rc_mode == VENC_CBR_MODE) {
        if(cfg->cbr.qp_max > 51 || cfg->cbr.qp_min < 0 || cfg->cbr.qp_min > cfg->cbr.qp_max ||
           cfg->cbr.bitrate < BITRATE_MIN || cfg->cbr.bitrate > BITRATE_MAX ||
           cfg->cbr.qp_delta < -12 || cfg->cbr.qp_delta > 12 ||
           (cfg->cbr.qp_hdr != -1 && (cfg->cbr.qp_hdr > cfg->cbr.qp_max ||cfg->cbr.qp_hdr < cfg->cbr.qp_min ))) {
           LOGE("invalid cbr param\n");
           return VBEBADPARAM;
        }
    } else if(cfg->rc_mode == VENC_VBR_MODE) {
        if(cfg->vbr.qp_max > 51 || cfg->vbr.qp_min < 0 || cfg->vbr.qp_min > cfg->vbr.qp_max ||
           cfg->vbr.max_bitrate < BITRATE_MIN || cfg->vbr.max_bitrate > BITRATE_MAX ||
           cfg->vbr.threshold < 0 || cfg->vbr.threshold > 100) {
           LOGE("invalid vbr param\n");
           return VBEBADPARAM;
        }
    } else if(cfg->rc_mode == VENC_FIXQP_MODE) {
        if(cfg->fixqp.qp_fix > 51 || cfg->fixqp.qp_fix < 0) {
           LOGE("invalid fixqp param\n");
           return VBEBADPARAM;
        }
    }

    this->MutexSettingLock->lock();
    this->H264EncRateCtrlUpateFlag = false;

    switch (cfg->rc_mode) {
        case VENC_CBR_MODE:
            if(pH264EncLocBRCfg.bitrate != cfg->cbr.bitrate)
            {
                m_u8RateCtrlReset = 2;
            }
            this->pH264EncLocBRCfg.qp_max = cfg->cbr.qp_max;
            this->pH264EncLocBRCfg.qp_min = cfg->cbr.qp_min;
            this->pH264EncLocBRCfg.qp_delta = cfg->cbr.qp_delta;
            this->pH264EncLocBRCfg.qp_hdr = cfg->cbr.qp_hdr;
            this->pH264EncLocBRCfg.bitrate = cfg->cbr.bitrate;
            this->pH264EncLocBRCfg.picture_skip = cfg->picture_skip;
            this->vbr_pic_skip = 0;
            break;
        case VENC_VBR_MODE:
            if((uint64_t)cfg->vbr.max_bitrate != MaxBitrate || VBRThreshHold != (uint32_t)cfg->vbr.threshold)
            {
                m_u8RateCtrlReset = 2;
            }
            this->pH264EncLocBRCfg.qp_min = cfg->vbr.qp_min;
            this->pH264EncLocBRCfg.qp_max = cfg->vbr.qp_max;
            this->pH264EncLocBRCfg.qp_delta = cfg->vbr.qp_delta;
            if(this->pH264EncLocBRCfg.rc_mode != VENC_VBR_MODE)
            {
                this->pH264EncLocBRCfg.qp_hdr = -1;
            }
            this->MaxBitrate = cfg->vbr.max_bitrate;
            this->VBRThreshHold = cfg->vbr.threshold;
            this->total_frames = 0;
            this->total_size = 0;
            this->counter = 0;
            this->avg_framesize = (double)this->MaxBitrate * (this->VBRThreshHold + 100) / 200 / 8.0 / this->CurFps;
            this->vbr_pic_skip = cfg->picture_skip;
            if(!this->vbr_pic_skip)
                this->pH264EncLocBRCfg.picture_skip = 0;
            break;
        case VENC_FIXQP_MODE:
            this->pH264EncLocBRCfg.qp_hdr = cfg->fixqp.qp_fix;
            this->pH264EncLocBRCfg.qp_max = cfg->fixqp.qp_fix;
            this->pH264EncLocBRCfg.qp_min = cfg->fixqp.qp_fix;
            this->pH264EncLocBRCfg.picture_skip = cfg->picture_skip;
            this->vbr_pic_skip = 0;
            break;
        default:
            LOGE("invalid rc mode\n");
            break;
    }
    if(this->pH264EncLocBRCfg.rc_mode != cfg->rc_mode)
    {
        m_u8RateCtrlReset = 2;
    }
    this->pH264EncLocBRCfg.rc_mode = cfg->rc_mode;
    this->pH264EncLocBRCfg.gop_length = cfg->gop_length;
    this->pH264EncLocBRCfg.p_frame = cfg->idr_interval;
    this->pH264EncLocBRCfg.hrd = cfg->hrd;
    this->pH264EncLocBRCfg.hrd_cpbsize = cfg->hrd_cpbsize;
    this->pH264EncLocBRCfg.mb_qp_adjustment = cfg->mb_qp_adjustment;
    if(this->enable_longterm == 1){
        this->refresh_interval = cfg->refresh_interval;
    }
    else{
        this->refresh_interval = 0;
    }
    this->H264EncRateCtrlUpateFlag = true;
    this->MutexSettingLock->unlock();
    return 0;
}

int IPU_H1264::GetROI(struct v_roi_info *cfg)
{
    this->MutexSettingLock->lock();
    *cfg = this->pH264EncLocRoiAreaInfo;
    this->MutexSettingLock->unlock();

    return 0;
}

int IPU_H1264::SetROI(struct v_roi_info *cfg)
{
    int i = 0;
    for(i = 0; i < 2; i++) {
        if(!cfg || cfg->roi[i].qp_offset > 0 || cfg->roi[i].qp_offset < -15) {
            LOGE("invalid roi qp_offset param\n");
            return VBEBADPARAM;
        }
        if(cfg->roi[i].x >= this->pH264EncCfg.width || cfg->roi[i].y >= this->pH264EncCfg.height
                || (cfg->roi[i].x + cfg->roi[i].w) > this->pH264EncCfg.width
                || (cfg->roi[i].y + cfg->roi[i].h) > this->pH264EncCfg.height) {
            LOGE("invalid roi param\n");
            return VBEBADPARAM;
        }
    }

    this->MutexSettingLock->lock();
    this->H264EncCodingCtrlUpdateFlag = false;
    this->pH264EncLocRoiAreaInfo = *cfg;
    this->H264EncCodingCtrlUpdateFlag = true;
    this->MutexSettingLock->unlock();

    return 0;
}

int IPU_H1264::SetFrc(v_frc_info *info){
    if(info->framebase == 0){
        LOGE("frc framebase can not be zero\n");
        return VBEBADPARAM;
    }
    LOGE("###########frc info -> %d :%d \n", info->framerate, info->framebase);
    if(info->framerate <= 0 || info->framebase <= 0){
        LOGE("target frc can be negative\n");
        return VBEBADPARAM;
    }
    this->MutexSettingLock->lock();
    this->FrcInfo = *info;
    this->InfpsRatio = -1;
    this->MutexSettingLock->unlock();
    return 1;
}
int IPU_H1264::GetFrc(v_frc_info *info){
    this->MutexSettingLock->lock();
    *info = this->FrcInfo;
    this->MutexSettingLock->unlock();
    return 1;
}

int IPU_H1264::SetPipInfo(struct v_pip_info *vpinfo)
{
    if(strncmp(vpinfo->portname, "frame", 5) == 0)
    {
        GetPort("frame")->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
    }
    return 0;
}

int IPU_H1264::GetPipInfo(struct v_pip_info *vpinfo)
{
    Port *pin;

    if(strncmp(vpinfo->portname, "frame", 5) == 0)
    {
        pin = GetPort("frame");
        vpinfo->x = pin->GetPipX();
        vpinfo->y = pin->GetPipY();
        vpinfo->w = pin->GetPipWidth();
        vpinfo->h = pin->GetPipHeight();
    }
    return 0;
}

bool IPU_H1264::CheckFrcUpdate(void){
    if(this->InfpsRatio ==  -1){
        if(this->FrcInfo.framerate == 0){
            this->InfpsRatio = 1;
            this->FrcInfo.framebase = 1;
            this->FrcInfo.framerate = this->Infps;
        }
        int targetFps = this->FrcInfo.framerate / this->FrcInfo.framebase;
        int quotient = 0;
        int remainder = 0;
        if(targetFps == this->Infps || (0)){ //TODO: add some more check for little error
            this->InfpsRatio = 1;
        }
        else if(targetFps > this->Infps){
            //fill frame
            quotient = targetFps / this->Infps;
            remainder = targetFps % this->Infps;
            if(remainder > (this->Infps / 2))
            {
                quotient++;
            }
            if(remainder != 0){
                LOGE("WARNING: frc repeat may not support ,just set to %d times of origin fps ->%d:%d\n",
                        quotient, this->Infps, this->Infps * quotient);
            }
            this->InfpsRatio = quotient;
            this->FrameDuration = 1000/(this->InfpsRatio * this->Infps);
        }
        else{
            //skip frame
            quotient = this->Infps *this->FrcInfo.framebase / this->FrcInfo.framerate;
            remainder = this->Infps *this->FrcInfo.framebase % this->FrcInfo.framerate;
            if(remainder > (this->FrcInfo.framerate / this->FrcInfo.framebase /2))
            {
                quotient++;
            }
            if(remainder != 0)
            {
                LOGE("WARNING: frc skip may not support ,just set to 1/%d times of origin fps ->%d:%d\n",
                        quotient, this->Infps, this->Infps / quotient);
            }
            if (quotient == 1)
            {
                this->InfpsRatio = quotient;
            }
            else
            {
                this->InfpsRatio = 0 - quotient;
            }
            this->FrameDuration = 1000 * this->InfpsRatio / this->Infps;
        }

        //LOGE("target fps ************->%2d %3d %8d\n", targetFps, this->InfpsRatio, this->FrameDuration);
        return true;
    }
    else{
        return false;
    }
}

void IPU_H1264::VideoInfo(Buffer *dst)
{
    if ((this->t_cnt >= this->pH264EncLocBRCfg.gop_length * 5 && this->enable_longterm == 0)
        ||(this->t_cnt >= this->pH264EncLocBRCfg.gop_length && this->enable_longterm == 1))
    {
        if(dst->fr_buf.timestamp == this->t_last_frame_time)
        {
            rtBps = (this->pH264EncLocBRCfg.rc_mode == VENC_CBR_MODE) ? pH264EncLocBRCfg.bitrate : (MaxBitrate * (100 + VBRThreshHold) / 100);
            return;
        }

        this->AvgFps = this->t_fps_cnt * 1000 / (dst->fr_buf.timestamp - this->t_last_frame_time);
        if(abs(this->CurFps - this->AvgFps) > 1) {
            this->CurFps = this->AvgFps;
            this->IsFPSChange = true;
        }
        this->t_cnt = 0;
        this->t_fps_cnt = 0;
        this->rtBps =  this->t_total_size * 8 * 1000 / (dst->fr_buf.timestamp - this->t_last_frame_time);
#if 0
        LOGE("avgfps: %d\n", this->AvgFps);
        LOGE("t_out_bitrate: \t\t\t%d\n", this->t_total_size * 8 * 1000 / (dst->fr_buf.timestamp - this->t_last_frame_time));
        LOGE("t_fps:\t\t\t%d\n", this->CurFps);
        LOGE("rc_mode:\t\t\t%d\n", this->pH264EncLocBRCfg.rc_mode);
#endif
    this->MutexSettingLock->lock();
    if(((this->pH264EncLocBRCfg.rc_mode == VENC_CBR_MODE && rtBps > (uint64_t) this->pH264EncLocBRCfg.bitrate * 9 / 10) /* Just Reserve a Gap */
        || (this->pH264EncLocBRCfg.rc_mode == VENC_VBR_MODE && rtBps > MaxBitrate))
        && (m_u8RateCtrlReset != 2))
     {
        m_u8RateCtrlReset = 1;
     }
     this->MutexSettingLock->unlock();
    } else {
        if(!this->t_cnt) {
            this->t_total_size = 0;
            this->t_last_frame_time = dst->fr_buf.timestamp;
        }

        this->t_total_size += dst->fr_buf.size;
        this->t_cnt++;
        if(dst->fr_buf.size)
            this->t_fps_cnt++;
#if 0
        if(dst->fr_buf.priv == VIDEO_FRAME_I) {
            LOGE("I: %d\n", dst->fr_buf.size);
        } else {
            LOGE("P: %d\n", dst->fr_buf.size);
        }
#endif
    }
}

bool IPU_H1264::VbrProcess(Buffer *dst)
{
    int     rtn_quant;
    int64_t deviation;
    uint32_t frame_size;
    uint32_t cur_fps;
    double target_framesize_min, target_framesize_max;
    double quality_scale, base_quality, target_quality;
    double overflow;

    if(this->pre_rtn_quant == -1) {
        LOGE("wait qp_hdr init\n");
        return false;
    }

    cur_fps = this->CurFps;
    frame_size = dst->fr_buf.size;
    target_framesize_max = ((double)this->MaxBitrate * (THRESHOLD_FACTOR - 1) / THRESHOLD_FACTOR +
        (double)this->MaxBitrate * this->VBRThreshHold / 100 / THRESHOLD_FACTOR) / 8.0 / cur_fps;
    target_framesize_min = ((double)this->MaxBitrate / THRESHOLD_FACTOR +
        (double)this->MaxBitrate * this->VBRThreshHold / 100 * (THRESHOLD_FACTOR - 1) / THRESHOLD_FACTOR) / 8.0 / cur_fps;

    if ((this->pre_rtn_quant == this->pH264EncLocBRCfg.qp_min) && (frame_size < target_framesize_min))
        goto skip_integrate_err;
    else if ((this->pre_rtn_quant == this->pH264EncLocBRCfg.qp_max) && (frame_size > target_framesize_max))
        goto skip_integrate_err;

    this->total_frames++;
    this->total_size += frame_size;

skip_integrate_err:
    if(this->total_size > target_framesize_max * this->total_frames) {
        deviation = this->total_size - target_framesize_max * this->total_frames;
    } else if(this->total_size < target_framesize_min * this->total_frames) {
        deviation = this->total_size - target_framesize_min * this->total_frames;
    } else {
        deviation = 0;
    }

    if (this->pre_rtn_quant >= 2) {
        this->sequence_quality -= this->sequence_quality / SEQ_QUALITY_FACTOR;
        this->sequence_quality += 2.0 / (double) this->pre_rtn_quant / SEQ_QUALITY_FACTOR;
        if (dst->fr_buf.priv == VIDEO_FRAME_P) {
            this->avg_framesize -= this->avg_framesize / AVG_FRAME_FACTOR;
            this->avg_framesize += frame_size / AVG_FRAME_FACTOR;
        }
    }

    if(this->avg_framesize > target_framesize_max) {
        quality_scale = target_framesize_max / this->avg_framesize * target_framesize_max / this->avg_framesize;
    } else if(this->avg_framesize < target_framesize_min) {
        quality_scale = target_framesize_min / this->avg_framesize * target_framesize_min / this->avg_framesize;
    } else {
        quality_scale = 1.0;
    }

    base_quality = this->sequence_quality;
    if (quality_scale >= 1.0) {
        base_quality = 1.0 - (1.0 - base_quality) / quality_scale;
    } else {
        base_quality = MIN_QUALITY + (base_quality - MIN_QUALITY) * quality_scale;
    }

    overflow = -((double) deviation / DEVIATION_FACTOR);

    if(quality_scale > 1.0) {
        target_quality = base_quality + (base_quality - MIN_QUALITY) * overflow / target_framesize_min;
    } else if(quality_scale < 1.0) {
        target_quality = base_quality + (base_quality - MIN_QUALITY) * overflow / target_framesize_max;
    } else {
        target_quality = (double)2.0 / this->pre_rtn_quant;
    }

    if (target_quality > 2.0)
        target_quality = 2.0;
    if (target_quality < MIN_QUALITY)
        target_quality = MIN_QUALITY;

    rtn_quant = (int) (2.0 / target_quality);
    if (rtn_quant < 52) {
        this->quant_error[rtn_quant] += 2.0 / target_quality - rtn_quant;
        if (this->quant_error[rtn_quant] >= 1.0) {
            this->quant_error[rtn_quant] -= 1.0;
            rtn_quant++;
        }
    }
    if (cur_fps <= 10) {
        if (rtn_quant > this->pre_rtn_quant + 3) {
            rtn_quant = this->pre_rtn_quant + 3;
        } else if(rtn_quant < this->pre_rtn_quant - 3) {
            rtn_quant = this->pre_rtn_quant - 3;
        }
    } else {
        if (rtn_quant > this->pre_rtn_quant + 1) {
            rtn_quant = this->pre_rtn_quant + 1;
        } else if (rtn_quant < this->pre_rtn_quant - 1) {
            rtn_quant = this->pre_rtn_quant - 1;
        }
    }

    if (rtn_quant > this->pH264EncLocBRCfg.qp_max) {
        rtn_quant = this->pH264EncLocBRCfg.qp_max;
        if(this->vbr_pic_skip && this->pH264EncLocBRCfg.picture_skip == 0) {
            this->MutexSettingLock->lock();
            this->H264EncRateCtrlUpateFlag = false;
            this->pH264EncLocBRCfg.picture_skip = 1;
            this->H264EncRateCtrlUpateFlag = true;
            this->MutexSettingLock->unlock();
        }
    } else if (rtn_quant < this->pH264EncLocBRCfg.qp_min) {
        rtn_quant = this->pH264EncLocBRCfg.qp_min;
    } else {
        if(this->vbr_pic_skip && this->pH264EncLocBRCfg.picture_skip == 1) {
            this->MutexSettingLock->lock();
            this->H264EncRateCtrlUpateFlag = false;
            this->pH264EncLocBRCfg.picture_skip = 0;
            this->H264EncRateCtrlUpateFlag = true;
            this->MutexSettingLock->unlock();
        }
    }
    if (cur_fps <= 10 && this->MaxBitrate * this->VBRThreshHold / 100 <= 128000) {
        if (dst->fr_buf.priv == VIDEO_FRAME_I) {
            rtn_quant -= 5;
            if (rtn_quant <= 25){
                rtn_quant = 25;
            }
        }
}

#if 0
    LOGE("\t Infps\t\t%d\n", this->Infps);
    LOGE("\t curfps\t\t%d\n", cur_fps);
    LOGE("\t this->total_frames\t\t%ld\n", this->total_frames);
    LOGE("\t this->total_size\t\t%lf\n", this->total_size);
    LOGE("\t target_frame_size_min\t\t%lf\n", target_framesize_min);
    LOGE("\t target_frame_size_max\t\t%lf\n", target_framesize_max);
    LOGE("\t this->sequence_quality\t\t%lf\n", this->sequence_quality);
    LOGE("\t this->avg_framesize\t\t%lf\n", this->avg_framesize);
    LOGE("\t frame_size\t\t%d\n", frame_size);
    LOGE("\t quality_scale\t\t%lf\n", quality_scale);
    LOGE("\t deviation\t\t%ld\n", deviation);
    LOGE("\t overflow\t\t%lf\n", overflow);
    LOGE("\t base_quality\t\t%lf\n", base_quality);
    LOGE("\t target_quality\t\t%lf\n", target_quality);
    LOGE("\t pre_rtn_quant: \t\t%d\n", this->pre_rtn_quant);
    LOGE("\t rtn_quant: \t\t%d\n", rtn_quant);
#endif
    if(rtn_quant != this->pre_rtn_quant) {
        this->MutexSettingLock->lock();
        this->H264EncRateCtrlUpateFlag = false;
        this->pH264EncLocBRCfg.qp_hdr = rtn_quant;
        this->H264EncRateCtrlUpateFlag = true;
        this->MutexSettingLock->unlock();
        this->pre_rtn_quant = rtn_quant;
    }
    return true;
}

bool IPU_H1264::VbrAvgProcess(Buffer *dst)
{
    uint64_t out_bitrate;
    if(this->counter == this->pH264EncLocBRCfg.gop_length) {
        this->total_sizes += dst->fr_buf.size;
        this->total_time += dst->fr_buf.timestamp - this->last_frame_time;
        this->total_sizes -= this->aver_size;
        this->total_time -= this->aver_time;
        out_bitrate = this->total_sizes * 8 * 1000/ this->total_time;
        this->last_frame_time = dst->fr_buf.timestamp;
        this->aver_time = this->total_time/this->counter;
        this->aver_size = this->total_sizes/this->counter;
        this->pH264EncLocBRCfg.bitrate = out_bitrate;
        if(out_bitrate > this->MaxBitrate) {
            if(this->pH264EncLocBRCfg.qp_hdr < this->pH264EncLocBRCfg.qp_max) {
                this->MutexSettingLock->lock();
                this->H264EncRateCtrlUpateFlag = false;
                this->pH264EncLocBRCfg.qp_hdr++;
                this->H264EncRateCtrlUpateFlag = true;
                this->MutexSettingLock->unlock();
            }
        } else if (out_bitrate < this->MaxBitrate * this->VBRThreshHold / 100) {
            if(this->pH264EncLocBRCfg.qp_hdr > this->pH264EncLocBRCfg.qp_min) {
                this->MutexSettingLock->lock();
                this->H264EncRateCtrlUpateFlag = false;
                this->pH264EncLocBRCfg.qp_hdr--;
                this->H264EncRateCtrlUpateFlag = true;
                this->MutexSettingLock->unlock();
            }
        }
    } else {
        if(!this->counter) {
            this->last_frame_time = dst->fr_buf.timestamp;
            this->total_time = this->total_sizes = 0;
            this->aver_time = this->aver_size = 0;
        }
        this->counter++;
        this->total_sizes += dst->fr_buf.size;
    }
    return true;
}

bool IPU_H1264::IsH264InstReady()
{
    return (this->pH264EncInst != NULL ? true : false);
}

int IPU_H1264::TriggerKeyFrame(void)
{
    this->TriggerKeyFrameFlag = true ;
    return 1;
}

int IPU_H1264::SetFrameMode(enum v_enc_frame_type arg)
{
    frame_mode = arg;
    return 1;
}

int IPU_H1264::GetFrameMode()
{
    return frame_mode;
}

void IPU_H1264::UpdateDebugInfo(int s32Update)
{
    if(s32Update)
    {
        m_stVencInfo.stRcInfo.s8MaxQP = pH264EncLocBRCfg.qp_max;
        m_stVencInfo.stRcInfo.s8MinQP = pH264EncLocBRCfg.qp_min;
        m_stVencInfo.stRcInfo.s32PictureSkip = pH264EncLocBRCfg.picture_skip;
        m_stVencInfo.stRcInfo.s32Gop = pH264EncLocBRCfg.gop_length;
        m_stVencInfo.stRcInfo.s32IInterval = pH264EncLocBRCfg.p_frame;
        m_stVencInfo.stRcInfo.s32Mbrc = pH264EncRateCtrl.mbQpAdjustment;
        m_stVencInfo.f32BitRate = rtBps/(1024.0);
        m_stVencInfo.f32FPS = CurFps;

        if (pH264EncLocBRCfg.rc_mode == VENC_CBR_MODE)
        {
            m_stVencInfo.stRcInfo.enRCMode = CBR;
            m_stVencInfo.stRcInfo.s32QPDelta = pH264EncLocBRCfg.qp_delta;
            m_stVencInfo.stRcInfo.f32ConfigBitRate = pH264EncLocBRCfg.bitrate/(1024.0);
            m_stVencInfo.stRcInfo.u32ThreshHold = 0;
            H264EncGetRealQp(this->pH264EncInst,&m_stVencInfo.stRcInfo.s32RealQP);
        }
        else if (pH264EncLocBRCfg.rc_mode == VENC_VBR_MODE)
        {
            m_stVencInfo.stRcInfo.enRCMode = VBR;
            m_stVencInfo.stRcInfo.s32QPDelta = pH264EncLocBRCfg.qp_delta;
            m_stVencInfo.stRcInfo.f32ConfigBitRate = MaxBitrate/(1024.0);
            m_stVencInfo.stRcInfo.u32ThreshHold = VBRThreshHold;
            m_stVencInfo.stRcInfo.s32RealQP = pH264EncLocBRCfg.qp_hdr;
        }
        else
        {
            m_stVencInfo.stRcInfo.enRCMode = FIXQP;
            m_stVencInfo.stRcInfo.s8MaxQP = m_stVencInfo.stRcInfo.s8MinQP = pH264EncLocBRCfg.qp_hdr;
            m_stVencInfo.stRcInfo.s32QPDelta = 0;
            m_stVencInfo.stRcInfo.f32ConfigBitRate = 0;
            m_stVencInfo.stRcInfo.u32ThreshHold = 0;
            m_stVencInfo.stRcInfo.s32RealQP = pH264EncLocBRCfg.qp_hdr;
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

int IPU_H1264::SetSeiUserData(ST_VIDEO_SEI_USER_DATA_INFO *pstSei)
{
    if ((pstSei == NULL) || (pstSei->length > VIDEO_SEI_USER_DATA_MAXLEN)\
           || (pstSei->length <= 0))
    {
        LOGE("sei param error\n");
        return VBEBADPARAM;
    }

    this->MutexSettingLock->lock();
    if (!this->m_bSEIUserDataSetFlag)
    {
        if (m_pstSei == NULL)
        {
            this->m_pstSei = (ST_VIDEO_SEI_USER_DATA_INFO*)malloc(sizeof(ST_VIDEO_SEI_USER_DATA_INFO));
        }
        if (this->m_pstSei == NULL)
        {
            LOGE("malloc sei info error\n");
            this->MutexSettingLock->unlock();
            return VBEMEMERROR;
        }
        memcpy(this->m_pstSei, pstSei, sizeof(ST_VIDEO_SEI_USER_DATA_INFO));
        this->m_bSEIUserDataSetFlag = true;
        m_enSEITriggerMode = m_pstSei->enMode;
        LOGE("seting sei info ok, mode:%d, size:%d, data:%s\n",
             m_enSEITriggerMode, m_pstSei->length, m_pstSei->seiUserData);
    }
    else
    {
        this->MutexSettingLock->unlock();
        LOGE("seting last sei info, try later.\n");
        return VBEFAILED;
    }
    this->MutexSettingLock->unlock();
    return VBOK;
}

int IPU_H1264::GetDebugInfo(void *pInfo, int *ps32Size)
{
    if (pInfo == NULL)
    {
        LOGE("[%s]Get debug info fail\n",__func__);
        return -1;
    }

    memcpy((ST_VENC_INFO *)pInfo, &m_stVencInfo, sizeof(ST_VENC_INFO));
    if (ps32Size != NULL)
    {
        *ps32Size = sizeof(ST_VENC_INFO);
    }


    return 0;
}
