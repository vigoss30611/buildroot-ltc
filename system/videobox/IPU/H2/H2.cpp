// Copyright (C) 2016 InfoTM, bright.jiang@infotm.com
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
#include "H2.h"

#define HEVC_PERFORMANCE_TEST  0

#define  HEVC_SHOW_DEBUG_PARA  0
#define BITRATE_MIN (10*1000)
#define BITRATE_MAX (60000*1000)
#define MIN_QUALITY (double)2.0/52.0
#define AVG_FRAME_FACTOR    (double)4.0
#define SEQ_QUALITY_FACTOR  (double)100.0
#define DEVIATION_FACTOR    (double)10.0
#define THRESHOLD_FACTOR    (double)4.0
#define FPS_MONITOR 0
#define CTB_SIZE   64

DYNAMIC_IPU(IPU_H2, "h2");
std::mutex *IPU_H2::MutexHwLock = NULL; //init class static various
int IPU_H2::InstanceCounter = 0 ;  //init  class instance counter
bool IPU_H2::bSmartrcStarted = false;

static HEVCEncPictureType HevcSwitchFormat(Pixel::Format srcFormat)
{
    if(srcFormat == Pixel::Format::RGBA8888)
        return HEVCENC_BGR888;// encode need set BGR888 other than RGB888
    else if(srcFormat == Pixel::Format::NV12)
        return HEVCENC_YUV420_SEMIPLANAR;
    else if(srcFormat == Pixel::Format::NV21)
        return HEVCENC_YUV420_SEMIPLANAR_VU;
    else if(srcFormat == Pixel::Format::RGB565)
        return HEVCENC_RGB565;
    else if(srcFormat == Pixel::Format::YUYV422)
        return HEVCENC_YUV422_INTERLEAVED_YUYV;
    else
    {
        LOGE("input src type not support -> %d \n",srcFormat);
        throw VBERR_BADPARAM;
    }
}
void IPU_H2::WorkLoop() {
    int hRet;
    Buffer bfin, bfout;
    Port *pin, *pout;

    pin = GetPort("frame");
    pout = GetPort("stream");
    int fpsRatio = 1;

    bool needNewInFrFlag = true;
    bool needNewOutFrFlag = true;
    int outFrSize;
    this->Infps = pin->GetFPS();
    if (this->IsHevcReady() == false) {
        if (this->HevcInstanceInit() != H2_RET_OK) {
            LOGE("init hevc error\n");
            throw VBERR_BADPARAM;
        } else {
            LOGD("init hevc inst ok\n");
        }
    }

    if (this->HevcEncodeSetPreProcessCfg() != H2_RET_OK) {
        LOGE("encode start set pre process cfg error \n");
        throw VBERR_BADPARAM;
    }
    if (this->HevcEncodeSetCodingCfg() != H2_RET_OK) {
        LOGE("encode start set coding cfg error \n");
        throw VBERR_BADPARAM;
    }
    if( this->HevcEncodeSetRateCtrl() != H2_RET_OK ){
        LOGE("encode start set rc error \n");
        throw VBERR_BADPARAM;
    }

    bfout = pout->GetBuffer();
    if (this->HevcEncodeStartStream(bfout) != H2_RET_OK) {
        LOGE("encode start steam error \n");
        throw VBERR_RUNTIME;
    } else {
        IsHeaderReady = true;
    }

    this->SaveHeader((char *)bfout.fr_buf.virt_addr, bfout.fr_buf.size);
    pout->PutBuffer(&bfout);

    LOGE("stream start ok ->>>>>>>>>>>>\n");

    this->FrameCounter = 0;
    this->TotolFrameCounter++;
    this->EncIn.poc = 0;

    while(IsInState(ST::Running)) {
        this->m_stVencInfo.enState = RUN;
        try {
#if FPS_MONITOR
            if(this->IsFPSChange && this->CurFps && needNewOutFrFlag && needNewInFrFlag) {
                this->MutexSettingLock->lock();
                this->IsFPSChange = false;
                this->HevcEncodeUpdateFPS();
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
                    hRet =  HevcEncodePictureStream(bfin,bfout) ;
                    if(hRet != H2_RET_OK){
                        bfout.fr_buf.size = outFrSize;
                        needNewOutFrFlag = false;
                        this->m_stVencInfo.u32FrameDrop++;
                        break;
                    }
                    bfout.fr_buf.timestamp = bfin.fr_buf.timestamp + this->FrameDuration * i;
                    if(this->VencRateCtrl.rc_mode == VENC_VBR_MODE)
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

                        outFrSize = bfout.fr_buf.size;
                        hRet =  HevcEncodePictureStream(bfin,bfout) ;
                        if(hRet != H2_RET_OK){
                            LOGE("ExH264EncStrmEncode failed.\n");
                            needNewOutFrFlag = false;
                            pin->PutBuffer(&bfin);
                            needNewInFrFlag = true;
                            bfout.fr_buf.size = outFrSize;
                            this->m_stVencInfo.u32FrameDrop++;
                            break;
                        }

                        bfout.SyncStamp(&bfin);
                        if(this->VencRateCtrl.rc_mode == VENC_VBR_MODE)
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
        UpdateDebugInfo(1);
        //When threads use SCHED_RR to schedule, need sleep to let other encode thread to get mutex lock.
        //Otherwise, other encode thread will block by cann't get lock.
        usleep(1000);
    }
    this->m_stVencInfo.enState = STOP;
    UpdateDebugInfo(0);
    //exit and return all buffer
    if(needNewInFrFlag == false){
        pin->PutBuffer(&bfin);
        needNewInFrFlag = true;
    }
    if (needNewOutFrFlag == false)
    {
        //last error frame ,use dummy mark it
        LOGE("put dummy frame for last error\n");
        bfout.fr_buf.size  = 4;
        bfout.fr_buf.priv  = VIDEO_FRAME_DUMMY;
        this->m_stVencInfo.u64OutCount++;
        this->m_stVencInfo.u32FrameDrop++;
        pout->PutBuffer(&bfout);
    }
    needNewInFrFlag = true;
    needNewOutFrFlag = true;
    if (this->HevcInstanceRelease() != H2_RET_OK) {
        LOGE("release hevc instance error\n");
        throw VBERR_RUNTIME;
    }
}

void Show_Hevc_Rate_Control_Info( HEVCEncRateCtrl *rc ) {
#if HEVC_SHOW_DEBUG_PARA != 1
    return ;
#else

    LOGE("rc info ---------->\n");
    LOGE("\tpictureRc ---------->%d\n",rc->pictureRc);
    LOGE("\tpictureSkip ---------->%d\n",rc->pictureSkip);
    LOGE("\tqpHdr ---------->%d\n",rc->qpHdr);
    LOGE("\tqpMin ---------->%d\n",rc->qpMin);
    LOGE("\tqpMax ---------->%d\n",rc->qpMax);
    LOGE("\tbitPerSecond ---------->%d\n",rc->bitPerSecond);
    LOGE("\thrd ---------->%d\n",rc->hrd);
    LOGE("\thrd_cpbsize ---------->%d\n",rc->hrd_cpbsize);
    LOGE("\tgopLen ---------->%d\n",rc->gopLen);
    LOGE("\tintraQpDelta ---------->%d\n",rc->intraQpDelta);
    LOGE("\tfixedIntraQp ---------->%d\n",rc->fixedIntraQp);
    return ;
#endif
}

void Show_Hevc_Coding_Control_Info( HEVCEncCodingCtrl *cc ) {
#if HEVC_SHOW_DEBUG_PARA != 1
    return ;
#else
    LOGE("coding controlc info ---------->\n");
    //LOGE("\t --->%d\n",);
    //LOGE("\t --->%d\n",);
#endif
}
void Show_Hevc_Encoding_Info( HEVCEncConfig *cfg ) {
#if HEVC_SHOW_DEBUG_PARA != 1
    return ;
#else
    LOGE("encoding  info ---------->\n");
    LOGE("\t streamType --->%d\n",cfg->streamType);
    LOGE("\t level --->%d\n",cfg->level);
    LOGE("\t width --->%d\n",cfg->width);
    LOGE("\t heigh --->%d\n",cfg->height);
    LOGE("\t frameRateNum--->%d\n",cfg->frameRateNum);
    LOGE("\t frameRateDenom --->%d\n",cfg->frameRateDenom);
    LOGE("\t refFrameAmount --->%d\n",cfg->refFrameAmount);
    LOGE("\t constrainedIntraPrediction --->%d\n",cfg->constrainedIntraPrediction);
    LOGE("\t strongIntraSmoothing --->%d\n",cfg->strongIntraSmoothing);
    LOGE("\t sliceSize --->%d\n",cfg->sliceSize);
    LOGE("\t compressor --->%d\n",cfg->compressor);
    LOGE("\t interlacedFrame --->%d\n",cfg->interlacedFrame);
//    LOGE("\t  --->%d\n",cfg->);
#endif
}
void Show_Basic_Info( struct v_basic_info *cfg) {
#if HEVC_SHOW_DEBUG_PARA != 1
    return ;
#else
    LOGE("basic config  info ---------->\n");
    LOGE("\t media_type --->%d\n",cfg->media_type);
    LOGE("\t profile --->%d\n",cfg->profile);
    LOGE("\t stream_type --->%d\n",cfg->stream_type);
    LOGE("\t p_frame --->%d\n",cfg->p_frame);
#endif
}
void Show_Bitrate_Info( struct v_bitrate_info *cfg ) {
#if HEVC_SHOW_DEBUG_PARA != 1
    return ;
#else
    LOGE("bitrate  info ---------->\n");
    LOGE("\t bitrate  --->%d\n",cfg->bitrate);
    LOGE("\t qp_max --->%d\n",cfg->qp_max);
    LOGE("\t qp_min --->%d\n",cfg->qp_min);
    LOGE("\t qp_delta --->%d\n",cfg->qp_delta);
    LOGE("\t qp_hdr --->%d\n",cfg->qp_hdr);
    LOGE("\t gop_length --->%d\n",cfg->gop_length);
    LOGE("\t picture_skip --->%d\n",cfg->picture_skip);
#endif
    //LOGE("\t  --->%d\n",cfg->);
    //LOGE("\t  --->%d\n",cfg->);
}
void Show_Roi_Info( HEVCEncConfig *cfg ) {
#if HEVC_SHOW_DEBUG_PARA != 1
    return ;
#else
    LOGE("encoding  info ---------->\n");
#endif
}
IPU_H2::IPU_H2(std::string name, Json *js) {
    Port *p;
    int bufferSize = 0x300000;
    int min_buffernum = 3;
    int value = 0;
    bool ratectrset = false;
    char s8Rotate[8];

    memset(&this->rate_ctrl_info, 0, sizeof(struct v_rate_ctrl_info));
    memset(&this->VencPpCfg,0,sizeof(hevc_preprocess_t));
    this->chroma_qp_offset = 0;
    if(NULL != js)
    {
        if (NULL != js->GetString("rotate"))
        {
            strncpy(s8Rotate, js->GetString("rotate"), 8);
            if (NULL != s8Rotate)
            {
                if (!strncmp(s8Rotate, "90R", 3))
                {
                    this->VencPpCfg.rotation = HEVCENC_ROTATE_90R;
                }
                else if (!strncmp(s8Rotate, "90L", 3))
                {
                    this->VencPpCfg.rotation = HEVCENC_ROTATE_90L;
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
        if(js->GetInt("min_buffernum") >= 3)
            min_buffernum = js->GetInt("min_buffernum");
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
            this->rate_ctrl_info.gop_length = value > 0 ? value : 15;

            value = js->GetInt("idr_interval");
            this->rate_ctrl_info.idr_interval = value > 0 ? value : 15;

            if(VENC_CBR_MODE == this->rate_ctrl_info.rc_mode)
            {
                value =  js->GetInt("qp_max");
                this->rate_ctrl_info.cbr.qp_max = value > 0 ? value : 42;
                value = js->GetInt("qp_min");
                this->rate_ctrl_info.cbr.qp_min = value > 0 ? value : 26;
                value = js->GetInt("bitrate");
                this->rate_ctrl_info.cbr.bitrate = value > 0 ? value : 600*1024;
                value = js->GetInt("qp_delta");
                this->rate_ctrl_info.cbr.qp_delta = value != 0 ? value : -1;
                value = js->GetInt("qp_hdr");
                this->rate_ctrl_info.cbr.qp_hdr = value != 0 ? value : -1;
            }
            else if(VENC_VBR_MODE == this->rate_ctrl_info.rc_mode)
            {
                value = js->GetInt("qp_max");
                this->rate_ctrl_info.vbr.qp_max = value > 0 ? value : 42;
                value = js->GetInt("qp_min");
                this->rate_ctrl_info.vbr.qp_min =  value > 0 ? value : 26;
                value =  js->GetInt("max_bitrate");
                this->rate_ctrl_info.vbr.max_bitrate = value > 0 ? value : 2 * 1024 * 1024;
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
        this->smartrc = 0;
    }

    if(false == ratectrset)
    {
        this->rate_ctrl_info.rc_mode = VENC_CBR_MODE;
        this->rate_ctrl_info.gop_length = 15;
        this->rate_ctrl_info.idr_interval = 15;
        this->rate_ctrl_info.cbr.qp_max = 42;
        this->rate_ctrl_info.cbr.qp_min = 26;
        this->rate_ctrl_info.cbr.bitrate =  600*1024;
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

    this->Name = name;
    if (this->MutexHwLock ==  NULL ){
        this->MutexHwLock =  new std::mutex();
    }
    this->MutexSettingLock =  new std::mutex();
    this->InstanceCounter++;
    Header = (char *)malloc(256);
    memset(Header ,0,256);

    p = CreatePort("frame", Port::In);
    p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
    p->SetPixelFormat(Pixel::Format::NV12);

    p = CreatePort("stream", Port::Out);

    // This port is most likely not to be bind,
    // So export it here as to enable buffer allocation
    p->Export();

    //StreamOut->SetPixelFormat(Pixel::Format::YCbCr420p);
    p->SetBufferType(FRBuffer::Type::FLOAT, bufferSize, bufferSize / min_buffernum - 0x4000);
    p->SetPixelFormat(Pixel::Format::H265ES);

    this->MaxBitrate = 2 * 1024 * 1024;
    this->VBRThreshHold = 80;
    this->vbr_pic_skip = 0;

    this->HevcBasicParaInit();
    this->CurFps = 0;
    TriggerKeyFrameFlag = false;

    this->HeaderReadyFlag =  false ;

    this->PrepareCounter = 0 ;
    this->UnPrepareCounter = 0 ;

    memset(&this->VideoScenarioBuffer ,0 ,sizeof(struct fr_buf_info));
    this->VideoScenarioMode = VIDEO_DAY_MODE ;
    this->m_bSEIUserDataSetFlag = false;
    this->m_enSEITriggerMode = EN_SEI_TRIGGER_ONCE;
}

void IPU_H2::Prepare() {
#if 0
    Show_Hevc_Encoding_Info(&this->EncConfig);
    Show_Hevc_Rate_Control_Info(&this->EncRateCtrl);
    Show_Hevc_Coding_Control_Info(&this->EncCodingCtrl);
    Show_Basic_Info(&this->VencEncodeConfig);
#endif
    Show_Bitrate_Info(&this->VencRateCtrl);
    Port *portIn =  this->GetPort("frame") ;
    Buffer src, dst;
    Pixel::Format enPixelFormat = portIn->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888 && enPixelFormat != Pixel::Format::RGB565
        && enPixelFormat != Pixel::Format::YUYV422)
    {
        LOGE("frame Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    if(GetPort("stream")->GetPixelFormat() != Pixel::Format::H265ES)
    {
        LOGE("stream Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }


    this->VencPpCfg.originW = portIn->GetWidth() &(~15);
    this->VencPpCfg.originH = portIn->GetHeight() &(~7);
    this->Infps = portIn->GetFPS();
    this->EncRateCtrl.gopLen = this->VencRateCtrl.gop_length;
    this->IdrInterval = this->VencRateCtrl.p_frame ;
    this->EncConfig.frameRateNum = this->Infps;
    this->VencPpCfg.startX = portIn->GetPipX();
    this->VencPpCfg.startY = portIn->GetPipY();
    this->VencPpCfg.offsetW = portIn->GetPipWidth();
    this->VencPpCfg.offsetH = portIn->GetPipHeight();
    this->VencPpCfg.srcFrameType = portIn->GetPixelFormat();
    this->counter = 0;
    this->sequence_quality = -1.0;
    this->total_size = 0;
    this->total_sizes = 0;
    this->total_frames = 0;
    this->pre_rtn_quant = -1;
    this->avg_framesize = 0;
    this->t_cnt = 0;
    this->t_total_size = 0;
    this->rtBps = 0;
    this->IsHeaderReady = false;
    this->m_enIPUType = IPU_H265;
    this->m_stVencInfo.enType = IPU_H265;
    this->m_stVencInfo.enState = INIT;
    this->m_stVencInfo.enState = STATENONE;
    this->m_stVencInfo.stRcInfo.enRCMode = RCMODNONE;
    this->m_stVencInfo.s32Width = portIn->GetWidth();
    this->m_stVencInfo.s32Height = portIn->GetHeight();
    snprintf(this->m_stVencInfo.ps8Channel, CHANNEL_LENGTH, "%s-%s", Name.c_str(), "stream");
    UpdateDebugInfo(0);

    //this->VencPpCfg.rotation = ;
    LOGD("origin port info->%4d %4d\n",this->VencPpCfg.originW,this->VencPpCfg.originH);
    if(this->VencPpCfg.startX > this->VencPpCfg.originW  ){
        LOGE("pip start x error  ->%d %d\n",this->VencPpCfg.startX,this->VencPpCfg.originW);
        throw VBERR_BADPARAM;
    }
    else if(this->VencPpCfg.startY > this->VencPpCfg.originH ){
        LOGE("pip start y error  ->%d %d\n",this->VencPpCfg.startY,this->VencPpCfg.originH);
        throw VBERR_BADPARAM;
    }
    else  if( ( this->VencPpCfg.startX +this->VencPpCfg.offsetW )> this->VencPpCfg.originW){
        LOGE("pip value x and width  error->%d %d\n",this->VencPpCfg.startX,this->VencPpCfg.offsetW);
        throw VBERR_BADPARAM;
    }
    else if( (this->VencPpCfg.startY +this->VencPpCfg.offsetH) > this->VencPpCfg.originH ){
        LOGE("pip value y and height error ->%4d %4d\n",this->VencPpCfg.startY,this->VencPpCfg.offsetH);
        throw VBERR_BADPARAM;
    }
    else {
//        this->EncConfig.width = this->VencPpCfg.offsetW & (~15);
//        this->EncConfig.height = this->VencPpCfg.offsetH & (~7);

    }
    if(this->VencPpCfg.offsetW == 0){
        this->VencPpCfg.offsetW = this->VencPpCfg.originW ;
    }
    if(this->VencPpCfg.offsetH == 0){
        this->VencPpCfg.offsetH = this->VencPpCfg.originH ;
    }
    LOGD("+++++++ pip info ->%2X %3d %3d %4d %4d %4d %4d\n",
            this->VencPpCfg.srcFrameType,
            this->VencPpCfg.startX,
            this->VencPpCfg.startY,
            this->VencPpCfg.offsetW,
            this->VencPpCfg.offsetH,
            this->VencPpCfg.originW ,
            this->VencPpCfg.originH
            );


    LOGD("frame info -> %X %d %d %d\n",this->VencPpCfg.srcFrameType,
            this->EncConfig.width,
            this->EncConfig.height,
            this->Infps);
    this->PrepareCounter++;
    //LOGE("%s $$$$$$$$$  init -> %d %d\n",this->Name.c_str() ,this->PrepareCounter,this->UnPrepareCounter);
    memset(&this->VideoScenarioBuffer ,0 ,sizeof(struct fr_buf_info));
    this->VideoScenarioMode = VIDEO_DAY_MODE;
    this->m_pstSei = NULL;
    this->m_enSEITriggerMode = EN_SEI_TRIGGER_ONCE;
    this->m_bSEIUserDataSetFlag = false;
}

void IPU_H2::Unprepare() {
    Port *pIpuPort;

    if (m_pstSei != NULL)
    {
        free(m_pstSei);
        m_pstSei = NULL;
    }

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

    this->UnPrepareCounter++;
    if( this->VideoScenarioBuffer.virt_addr != NULL){
        fr_free_single(this->VideoScenarioBuffer.virt_addr,this->VideoScenarioBuffer.phys_addr);
        memset(&this->VideoScenarioBuffer,0,sizeof(struct fr_buf_info ));
    }
    //LOGE("%s ^^^^^^^^^  unit -> %d %d\n",this->Name.c_str() , this->PrepareCounter,this->UnPrepareCounter);
}

void sliceReadyCbFunc(HEVCEncSliceReady *Slice) {
    // TODO : Support slice encoded
    LOGE("slice callback\n");
    return;
}

void IPU_H2::HevcEncodeUpdateFPS()
{
    int hRet;
    Buffer bfin, bfout;
    Port *pin, *pout;

    pin = GetPort("frame");
    pout = GetPort("stream");
    bfin = pin->GetBuffer(&bfin);
    bfout = pout->GetBuffer();

    hRet = HevcEncodeEndStream(&bfin, &bfout);
    if(hRet != H2_RET_OK){
        LOGE("HevcEncodeEndStream failed.\n");
        throw VBERR_BADPARAM;
    }
    pout->PutBuffer(&bfout);
    if (this->HevcInstanceRelease() != H2_RET_OK) {
        LOGE("release hevc instance error\n");
        throw VBERR_RUNTIME;
    }

    while(this->EncInst) {
        LOGE("EncInst is not NULL\n");
        usleep(20000);
    }

    this->EncConfig.frameRateDenom = 1;
    this->EncConfig.frameRateNum = this->CurFps;
    this->MutexHwLock->lock();
    HEVCEncInitOn();
    LOGE("##########h265 init fps ->%d\n", this->EncConfig.frameRateNum);
    hRet = HEVCEncInit(&this->EncConfig, &this->EncInst);
    this->MutexHwLock->unlock();
    if(hRet != HEVCENC_OK) {
        LOGE("HEVC Init failed. err code:%i\n", hRet);
        throw VBERR_BADPARAM;
    }

    if (this->HevcEncodeSetPreProcessCfg() != H2_RET_OK) {
        LOGE("encode start set pre process cfg error \n");
        throw VBERR_BADPARAM;
    }
    if (this->HevcEncodeSetCodingCfg() != H2_RET_OK) {
        LOGE("encode start set coding cfg error \n");
        throw VBERR_BADPARAM;
    }
    if( this->HevcEncodeSetRateCtrl() != H2_RET_OK ){
        LOGE("encode start set rc error \n");
        throw VBERR_BADPARAM;
    }
    bfout = pout->GetBuffer();
    if (this->HevcEncodeStartStream(bfout) != H2_RET_OK) {
        LOGE("encode start steam error \n");
        throw VBERR_RUNTIME;
    }
    pin->PutBuffer(&bfin);
    pout->PutBuffer(&bfout);
    this->EncIn.poc = 0;
}

void IPU_H2::HevcBasicParaInit(void) {
    EncInst = NULL;
    this->frame_mode = VENC_HEADER_NO_IN_FRAME_MODE;

    memset(&this->VencEncodeConfig , 0 ,sizeof( struct v_basic_info)  );
    memset(&this->VencRateCtrl, 0 ,sizeof( struct v_bitrate_info)  );
    memset(&this->VencRoiAreaInfo , 0 ,sizeof(struct v_roi_info )  );
    this->VencRoiAreaInfo.roi[0].enable = 1;
    this->VencRoiAreaInfo.roi[1].enable = 1;

    this->VencEncodeConfigUpdateFlag =  true ;
    this->VencRateCtrlUpateFlag =  true ;
    this->VencCodingCtrlUpdateFlag =  true ;
    // this->FrameCounter = 0;
    this->TotolFrameCounter = 0;
    //get system default setting
    //init IPU_H2 unit control setting date ;
    //this->VencEncodeConfig.profile      = VENC_PROFILE_MAIN ;
    this->VencEncodeConfig.stream_type  = VENC_BYTESTREAM ;
//    this->InitDefaultParameter();

//    this->EncNalMaxSize = IPY_H2_MAX_NAL_SIZE;

    this->VencPpCfg.srcFrameType = Pixel::Format::NV12;

    //init  basic enc config
    this->EncConfig.width =  1920  ;
    this->EncConfig.height =  1080 ;

    this->EncConfig.frameRateDenom =  1 ;
    this->EncConfig.frameRateNum =  30;
    this->CurFps = this->EncConfig.frameRateNum /  this->EncConfig.frameRateDenom;
    m_s32RateCtrlDistance = 0;
    m_u8RateCtrlReset = 0;
    this->VencRateCtrl.qp_hdr = -1;

    if(SetRateCtrl(&this->rate_ctrl_info)  != 0)
    {
        LOGE("ratectrl parameters is not right,please check json arg reset to default\n");
        this->VencRateCtrl.qp_max =  42;
        this->VencRateCtrl.qp_min =  26;
        this->VencRateCtrl.qp_hdr =  -1;
        this->VencRateCtrl.qp_delta = -1;
        this->VencRateCtrl.bitrate =  600*1024;
        this->VencRateCtrl.picture_skip =  0;
        this->VencRateCtrl.gop_length = 15;
        this->VencRateCtrl.p_frame    = 15;
    }
    this->IdrInterval =  this->VencRateCtrl.p_frame;

    this->EncConfig.sliceSize = 0 ;
    this->EncConfig.constrainedIntraPrediction =  0;
    this->EncConfig.strongIntraSmoothing =  1;
    this->EncConfig.streamType = this->VencEncodeConfig.stream_type == VENC_BYTESTREAM ?
        HEVCENC_BYTE_STREAM : HEVCENC_NAL_UNIT_STREAM ;

    this->EncConfig.level = HEVCENC_LEVEL_6 ;
    this->EncConfig.interlacedFrame  = 0;
    this->EncConfig.refFrameAmount =   1 + this->EncConfig.interlacedFrame ;
    this->EncConfig.compressor =  1 ;

#if 0
    this->EncConfig.level =(HEVCEncLevel) cml->level  ;
    this->EncConfig.refFrameAmount =  cml->interlacedFrame + 1 ;
    this->EncConfig.compressor =  cml->compressor ;
    this->EncConfig.interlacedFrame  =  cml->interlacedFrame;
#else
    //this->EncConfig.compressor =  1 ;


#endif
}
int IPU_H2::HevcEncodeSetCodingCfg(void){
    HEVCEncRet ret ;
    u32 u32CtbPerCol = 0,u32CtbPerRow = 0;

    ret = HEVCEncGetCodingCtrl(this->EncInst ,&this->EncCodingCtrl );
    if (ret != HEVCENC_OK){
        LOGE("hevc get coding ctrl error\n");
        return H2_RET_ERROR ;
    }

    u32CtbPerRow = (EncConfig.width + CTB_SIZE - 1) / CTB_SIZE;
    u32CtbPerCol = (EncConfig.height + CTB_SIZE - 1) / CTB_SIZE;

//    COMMOND_LINE_H2 *cml = &this->CommondLineH2 ;
    this->EncCodingCtrl.sliceSize = 0 ;
    this->EncCodingCtrl.disableDeblockingFilter =  0 ;
    this->EncCodingCtrl.tc_Offset   =  -2 ;
    this->EncCodingCtrl.beta_Offset =  5 ;
    this->EncCodingCtrl.enableSao = 1 ;
    this->EncCodingCtrl.enableDeblockOverride =  0 ; //fix from 1 to 0
    this->EncCodingCtrl.deblockOverride =  0 ;    //fix from 1 to 0
    this->EncCodingCtrl.cabacInitFlag =  0 ;
//

    this->EncCodingCtrl.cirStart = 0 ;
    this->EncCodingCtrl.cirInterval = 0 ;

    //fill intraArea
    this->EncCodingCtrl.intraArea.top =  -1  ;
    this->EncCodingCtrl.intraArea.left =  -1  ;
    this->EncCodingCtrl.intraArea.bottom =  -1  ;
    this->EncCodingCtrl.intraArea.right =  -1  ;
    this->EncCodingCtrl.intraArea.enable =  0  ;

    this->EncCodingCtrl.insertrecoverypointmessage=0;

    this->EncCodingCtrl.roi1Area.top = (this->VencRoiAreaInfo.roi[0].y) / CTB_SIZE;
    this->EncCodingCtrl.roi1Area.left = (this->VencRoiAreaInfo.roi[0].x) / CTB_SIZE;
    this->EncCodingCtrl.roi1Area.right = (this->VencRoiAreaInfo.roi[0].w + this->VencRoiAreaInfo.roi[0].x) / CTB_SIZE;
    this->EncCodingCtrl.roi1Area.bottom = (this->VencRoiAreaInfo.roi[0].h + this->VencRoiAreaInfo.roi[0].y) / CTB_SIZE;
    this->EncCodingCtrl.roi1Area.enable = this->VencRoiAreaInfo.roi[0].enable;
    EncCodingCtrl.roi1Area.right = EncCodingCtrl.roi1Area.right >= u32CtbPerRow ? u32CtbPerRow - 1 : EncCodingCtrl.roi1Area.right;
    EncCodingCtrl.roi1Area.bottom = EncCodingCtrl.roi1Area.bottom >= u32CtbPerCol ? u32CtbPerCol - 1 : EncCodingCtrl.roi1Area.bottom;


    this->EncCodingCtrl.roi2Area.top = (this->VencRoiAreaInfo.roi[1].y) / CTB_SIZE;
    this->EncCodingCtrl.roi2Area.left = (this->VencRoiAreaInfo.roi[1].x) / CTB_SIZE;
    this->EncCodingCtrl.roi2Area.right = (this->VencRoiAreaInfo.roi[1].w + this->VencRoiAreaInfo.roi[1].x) / CTB_SIZE;
    this->EncCodingCtrl.roi2Area.bottom = (this->VencRoiAreaInfo.roi[1].h + this->VencRoiAreaInfo.roi[1].y) / CTB_SIZE;
    this->EncCodingCtrl.roi2Area.enable = this->VencRoiAreaInfo.roi[1].enable;
    EncCodingCtrl.roi2Area.right = EncCodingCtrl.roi2Area.right >= u32CtbPerRow ? u32CtbPerRow - 1 : EncCodingCtrl.roi2Area.right;
    EncCodingCtrl.roi2Area.bottom = EncCodingCtrl.roi2Area.bottom >= u32CtbPerCol ? u32CtbPerCol - 1 : EncCodingCtrl.roi2Area.bottom;

    this->EncCodingCtrl.roi1DeltaQp = this->VencRoiAreaInfo.roi[0].qp_offset;
    this->EncCodingCtrl.roi2DeltaQp = this->VencRoiAreaInfo.roi[1].qp_offset;

#if 0
    if(cml->sei || cml->enableGDR || cml->dynamicenableGDR )
        this->EncCodingCtrl.seiMessages = 1;
    else
        this->EncCodingCtrl.seiMessages = 0;
#else
        this->EncCodingCtrl.seiMessages = 0;
#endif

    this->EncCodingCtrl.intraArea.enable  =  0;

    this->EncCodingCtrl.max_cu_size = 64 ;
    this->EncCodingCtrl.min_cu_size = 8 ;
    this->EncCodingCtrl.max_tr_size = 16 ;
    this->EncCodingCtrl.min_tr_size = 4 ;

    this->EncCodingCtrl.tr_depth_intra =  2 ;
    this->EncCodingCtrl.tr_depth_inter =  this->EncCodingCtrl.max_cu_size == 64 ?  4:3;

    this->EncCodingCtrl.videoFullRange  = 1  ;//fix from 0 to 1
    this->EncCodingCtrl.enableScalingList = 0 ;
    this->EncCodingCtrl.chroma_qp_offset =  this->chroma_qp_offset;

//    this->EncCodingCtrl.fieldOrder = 0 ;

    ret = HEVCEncSetCodingCtrl(this->EncInst ,&this->EncCodingCtrl );
    if (ret != HEVCENC_OK){
        LOGE("hevc set coding ctrl error ->%d\n",ret);
        return H2_RET_ERROR ;
    }
    LOGD("set coding type success\n");
    return H2_RET_OK;
}
int IPU_H2::HevcEncodeSetRateCtrl(void){
    HEVCEncRet ret ;
    int retValue = H2_RET_OK ;
    ret = HEVCEncGetRateCtrl(this->EncInst ,&this->EncRateCtrl );
    Show_Hevc_Rate_Control_Info(&this->EncRateCtrl);
    if (ret != HEVCENC_OK){
        LOGE("hevc get rate ctrl error\n");
        return H2_RET_ERROR ;
    }

    this->EncRateCtrl.hrd = this->VencRateCtrl.hrd;
    this->EncRateCtrl.hrdCpbSize = this->VencRateCtrl.hrd_cpbsize;
    this->IdrInterval = this->VencRateCtrl.p_frame;
    this->EncRateCtrl.pictureSkip = this->VencRateCtrl.picture_skip;
    this->EncRateCtrl.gopLen = this->VencRateCtrl.gop_length;
    this->EncRateCtrl.fixedIntraQp = 0;

    /* patch for reset ratectrl window to accelate smooth qp only user setting available
        qp_max threshold 42 and qp_max diff threshold 3 are pre-set value that can not cover all cases
    */
    if((VencRateCtrl.qp_max >= 42 && (u32)VencRateCtrl.qp_max > EncRateCtrl.qpMax
        && (u32)VencRateCtrl.qp_max - EncRateCtrl.qpMax >= 3
        && m_s32RateCtrlDistance > EncRateCtrl.gopLen && m_u8RateCtrlReset == 1)
        ||(m_u8RateCtrlReset == 2))
    {
        EncRateCtrl.u32NewStream = 1;
    }
    else
    {
        EncRateCtrl.u32NewStream = 0;
    }
    m_u8RateCtrlReset = 0;
    m_s32RateCtrlDistance = 0;

    if(this->VencRateCtrl.rc_mode == VENC_CBR_MODE ||
            this->VencRateCtrl.rc_mode == VENC_FIXQP_MODE) {
        LOGD("h265 enc cbr mode\n");
        if ( this->EncRateCtrl.bitPerSecond ==  (uint32_t)this->VencRateCtrl.bitrate  ) {
            this->EncRateCtrl.bitPerSecond = this->VencRateCtrl.bitrate +100;
        } else {
            this->EncRateCtrl.bitPerSecond = this->VencRateCtrl.bitrate;
        }
        this->EncRateCtrl.qpMax = this->VencRateCtrl.qp_max;
        this->EncRateCtrl.qpMin = this->VencRateCtrl.qp_min;
        this->EncRateCtrl.qpHdr =  this->VencRateCtrl.qp_hdr;
        this->EncRateCtrl.intraQpDelta =  this->VencRateCtrl.qp_delta;

        this->EncRateCtrl.pictureRc =  1;
    } else if(this->VencRateCtrl.rc_mode == VENC_VBR_MODE) {
        LOGD("h265 enc vbr mode\n");
        if(this->VencRateCtrl.qp_hdr == -1)
        {
            HEVCEncCalculateInitialQp(this->EncInst, &this->VencRateCtrl.qp_hdr, this->MaxBitrate * (this->VBRThreshHold + 100)/2/100);
            if(this->VencRateCtrl.qp_hdr > this->VencRateCtrl.qp_max)
            {
                this->VencRateCtrl.qp_hdr = this->VencRateCtrl.qp_max;
            }
            else if(this->VencRateCtrl.qp_hdr < this->VencRateCtrl.qp_min)
            {
                this->VencRateCtrl.qp_hdr = this->VencRateCtrl.qp_min;
            }
        }

        if(this->sequence_quality == -1.0)
            this->sequence_quality = 2.0 / this->VencRateCtrl.qp_hdr;
        if(this->pre_rtn_quant == -1)
            this->pre_rtn_quant = this->VencRateCtrl.qp_hdr;

        this->EncRateCtrl.qpMax = this->VencRateCtrl.qp_max ;
        this->EncRateCtrl.qpMin = this->VencRateCtrl.qp_min ;
        this->EncRateCtrl.qpHdr = this->VencRateCtrl.qp_hdr;
        this->EncRateCtrl.intraQpDelta = this->VencRateCtrl.qp_delta;
        this->EncRateCtrl.pictureRc =  0;
        if(this->vbr_pic_skip)
            this->EncRateCtrl.bitPerSecond = this->MaxBitrate;
    } else {
        LOGD("h265 enc rc mode error\n");
        return false;
    }

    if(this->EncRateCtrl.bitPerSecond < BITRATE_MIN)
        this->EncRateCtrl.bitPerSecond = BITRATE_MIN;
    if(this->EncRateCtrl.bitPerSecond > BITRATE_MAX)
        this->EncRateCtrl.bitPerSecond = BITRATE_MAX;

    Show_Hevc_Rate_Control_Info(&this->EncRateCtrl);
    ret = HEVCEncSetRateCtrl(this->EncInst ,&this->EncRateCtrl );
    Show_Hevc_Rate_Control_Info(&this->EncRateCtrl);
    if (ret != HEVCENC_OK){
        LOGE("hevc set rate ctrl error:%d\n", ret);
        retValue =  H2_RET_ERROR ;
    }

    return retValue;
}
int IPU_H2::HevcInstanceInit(void) {
    int ret = H2_RET_OK ;
    // init
    if(this->TryLockSettingResource() ){
        //get config data Mutex lock ok
        if(this->VencEncodeConfigUpdateFlag ){
            this->EncConfig.streamType = \
            this->VencEncodeConfig.stream_type == VENC_BYTESTREAM  ? HEVCENC_BYTE_STREAM :HEVCENC_NAL_UNIT_STREAM ;
            this->EncConfig.frameRateNum =   this->Infps;
        }
        else {

        }
        //free setting mutex resource
        this->UnlockSettingResource();
    }
    switch(this->VencPpCfg.rotation){
        case 0 :
            this->EncConfig.width = this->VencPpCfg.offsetW ;
            this->EncConfig.height = this->VencPpCfg.offsetH ;
            break;
        case 1 :
        case 2 :
            this->EncConfig.width = this->VencPpCfg.offsetH ;
            this->EncConfig.height = this->VencPpCfg.offsetW ;
            break;
        default :
            LOGD("hevc rotaton only ->0:0,1:90R,2:90L\n",this->VencPpCfg.rotation );
            throw VBERR_BADPARAM;
    }
    this->FrameSize = this->VencPpCfg.originW * this->VencPpCfg.originH;
    this->MutexHwLock->lock();
    HEVCEncInitOn();
    LOGE("##########h265 init fps ->%d\n", this->EncConfig.frameRateNum);
    if (HEVCEncInit(&this->EncConfig, &this->EncInst) != HEVCENC_OK) {
        ret = H2_RET_ERROR ;
    }
    this->MutexHwLock->unlock();
    return ret;
}


int IPU_H2::HevcEncodeStartStream(Buffer &OutBuf) {
    HEVCEncRet ret ;
    this->EncIn.pOutBuf = (uint32_t *)(OutBuf.fr_buf.virt_addr);

    this->EncIn.busOutBuf = OutBuf.fr_buf.phys_addr ;
    this->EncIn.outBufSize =  OutBuf.fr_buf.size ;

    LOGD("start stream -> v: %p b: %X  %d\n",OutBuf.fr_buf.virt_addr,OutBuf.fr_buf.phys_addr,OutBuf.fr_buf.size);
    this->MutexHwLock->lock();
    HEVCEncOn(this->EncInst);
    ret = HEVCEncStrmStart(this->EncInst, &this->EncIn, &this->EncOut);
    this->MutexHwLock->unlock();
    if (ret != HEVCENC_OK) {
        LOGE("stream start error -> %d %d\n",ret ,this->EncOut.streamSize);
        return H2_RET_ERROR;
    }
    OutBuf.fr_buf.size = this->EncOut.streamSize;
    OutBuf.fr_buf.priv = VIDEO_FRAME_HEADER;
    this->SaveHeader((char *) OutBuf.fr_buf.virt_addr, OutBuf.fr_buf.size);
    LOGD("stream start -> %d\n",this->EncOut.streamSize);
    return H2_RET_OK;
}
int IPU_H2::HevcEncodePictureStream(Buffer &InBuf, Buffer &OutBuf) {
    int AlignSize = 0, offset = 0;
    HEVCEncRet ret;
    bool bSetSEI = false;
    bool bIsIntraFrame = false;

    this->EncIn.busLuma = InBuf.fr_buf.phys_addr;
    this->EncIn.busChromaU = InBuf.fr_buf.phys_addr+this->FrameSize;
    this->EncIn.busChromaV = this->EncIn.busChromaU + this->FrameSize/4;

    this->EncIn.timeIncrement = 1;

    this->EncIn.pOutBuf = (uint32_t *)OutBuf.fr_buf.virt_addr;
    this->EncIn.busOutBuf = OutBuf.fr_buf.phys_addr;
    this->EncIn.outBufSize = OutBuf.fr_buf.size;

    // this->EncIn.codingType = HEVCENC_PREDICTED_FRAME;
    // this->EncIn.codingType = HEVCENC_NO_REFERENCE_NO_REFRESH;

    //
    if(this->TryLockSettingResource()) {
        //printf("encode ->%3d %2d\n",this->EncIn.poc,this->IdrInterval);
        this->IdrInterval = this->VencRateCtrl.p_frame;
        if (this->VencCodingCtrlUpdateFlag ){
            //update coding control
            this->HevcEncodeSetCodingCfg();
            this->VencCodingCtrlUpdateFlag =  false ;
        }
        if (this->VencRateCtrlUpateFlag ){
            //update rate control
            this->HevcEncodeSetRateCtrl();
            this->VencRateCtrlUpateFlag  = false;
        }

        if ((( this->EncIn.poc % this->IdrInterval ) == 0 )  || this->TriggerKeyFrameFlag ) {
            this->EncIn.codingType = HEVCENC_INTRA_FRAME;
            this->EncIn.poc = 0;
            OutBuf.fr_buf.priv = VIDEO_FRAME_I;
            this->TriggerKeyFrameFlag =  false ;

            if(frame_mode == VENC_HEADER_IN_FRAME_MODE) {
                AlignSize = (HeaderLength + 0x7) & (~0x7);
                offset = AlignSize - HeaderLength;
                memcpy(((char *)OutBuf.fr_buf.virt_addr + offset), Header, HeaderLength);
                memset((char *)OutBuf.fr_buf.virt_addr, 0, offset);
                this->EncIn.pOutBuf = (u32 *)((unsigned int)OutBuf.fr_buf.virt_addr + AlignSize);
                this->EncIn.busOutBuf = OutBuf.fr_buf.phys_addr + AlignSize;
                this->EncIn.outBufSize = OutBuf.fr_buf.size - AlignSize;
            }
        } else {
            this->EncIn.codingType = HEVCENC_PREDICTED_FRAME;
            OutBuf.fr_buf.priv = VIDEO_FRAME_P;
        }
        //fake uv data
        if(this->VideoScenarioMode == VIDEO_NIGHT_MODE){
            int uvSize = 0 ;
            switch(this->VencPpCfg.srcFrameType){
                case Pixel::Format::NV12 :
                case Pixel::Format::NV21 :
                    LOGD("video scenario nv12/nv21 \n");
                    uvSize =  this->VencPpCfg.originW * this->VencPpCfg.originH / 2;
                    break;
                case Pixel::Format::YUYV422 :
                    LOGD("video scenario yuyv422 \n");
                    uvSize =  this->VencPpCfg.originW * this->VencPpCfg.originH;
                    break;
                default :
                    LOGE("video scenario  not support -> %d \n",this->VencPpCfg.srcFrameType);
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
            this->EncIn.busChromaU = this->VideoScenarioBuffer.phys_addr;
            this->EncIn.busChromaV = this->VideoScenarioBuffer.phys_addr + uvSize ;
        }

        if (this->EncIn.codingType == HEVCENC_INTRA_FRAME)
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
            if (HEVCENC_OK != (HEVCEncSetSeiUserData(this->EncInst,
                (const u8 *)m_pstSei->seiUserData, m_pstSei->length)))
            {
                LOGE("set sei user data error\n");
            }
            m_bSEIUserDataSetFlag = true;
            if (m_enSEITriggerMode == EN_SEI_TRIGGER_ONCE)
            {
                m_pstSei->length = 0;
            }
        }
        this->UnlockSettingResource();
    }

//    this->HevcEncodeSetRateCtrl();

    this->LockHwResource(); //may be just support basic encode ,not support slice
#if HEVC_PERFORMANCE_TEST == 1
    TimePin tp;
#endif
    HEVCEncOn(this->EncInst);
    ret = HEVCEncStrmEncode(this->EncInst, &this->EncIn, &this->EncOut,
            sliceReadyCbFunc, NULL);
    HEVCEncOff(this->EncInst);
#if HEVC_PERFORMANCE_TEST == 1
    LOGE("HEVCEncStrmEncode: %d -> %d, time:%dms\n",this->TotolFrameCounter, ret, tp.Peek()/10);
#endif
    this->UnlockHwResource();
    switch (ret) {
        case HEVCENC_FRAME_READY:
            if (this->TryLockSettingResource())
            {
                // Clear SEI setting by set length to zero
                if (bSetSEI)
                {
                    if (m_enSEITriggerMode != EN_SEI_TRIGGER_EVERY_FRAME)
                    {
                        if (HEVCENC_OK != (HEVCEncSetSeiUserData(this->EncInst,\
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
                this->UnlockSettingResource();
            }
            if((this->EncIn.codingType == HEVCENC_INTRA_FRAME) && (frame_mode == VENC_HEADER_IN_FRAME_MODE)) {
                OutBuf.fr_buf.size = this->EncOut.streamSize + AlignSize;
            } else
                OutBuf.fr_buf.size = this->EncOut.streamSize;

            if (this->EncConfig.streamType == HEVCENC_BYTE_STREAM) {
                // save byte stream
            } else if (this->EncConfig.streamType == HEVCENC_NAL_UNIT_STREAM) {
                // save nal stream
            } else {
                LOGE("wrong stream type \n");
                return H2_RET_ERROR;
            }
            this->TotolFrameCounter++;
            this->FrameCounter++;
            if(!this->EncOut.streamSize)
                break;
            this->EncIn.poc++;
            break;
        case HEVCENC_OK:
            // do nothing
            break;
        case HEVCENC_OUTPUT_BUFFER_OVERFLOW:
            LOGE("memory over flow  -8\n");
            this->EncIn.poc  = 0;
            return H2_RET_ERROR;
        case HEVCENC_HW_RESET:
            LOGE("HW reset, retry  -16\n");
            this->EncIn.poc  = 0;
            return H2_RET_ERROR ;
        default:
            this->EncIn.poc  = 0;
            LOGE("encode error ->%d\n", ret);
            throw H2_RET_OK;
    }

    return H2_RET_OK;
}
int IPU_H2::HevcEncodeEndStream(Buffer *InBuf, Buffer *OutBuf) {

    HEVCEncRet ret =  HEVCENC_OK ;
    this->EncIn.busLuma = InBuf->fr_buf.phys_addr;
    this->EncIn.busChromaU = InBuf->fr_buf.phys_addr+ this->FrameSize;
    this->EncIn.busChromaV = 0;
    this->EncIn.timeIncrement = 0;
    this->EncIn.pOutBuf = (uint32_t *)(OutBuf->fr_buf.virt_addr);

    this->EncIn.busOutBuf = OutBuf->fr_buf.phys_addr;
    this->EncOut.pNaluSizeBuf = (uint32_t *)OutBuf->fr_buf.virt_addr;
    this->MutexHwLock->lock();
    ret = HEVCEncStrmEnd(this->EncInst, &this->EncIn, &this->EncOut);
    this->MutexHwLock->unlock();
    if ( ret != HEVCENC_OK) {
        return H2_RET_ERROR;
    }
    OutBuf->fr_buf.size = this->EncOut.streamSize;
    OutBuf->fr_buf.priv = VIDEO_FRAME_I;
    return H2_RET_OK;
}

int IPU_H2::HevcInstanceRelease(void) {
    HEVCEncRet hret;
    this->MutexHwLock->lock();
    hret = HEVCEncRelease(this->EncInst);
    this->MutexHwLock->unlock();
    if (hret != HEVCENC_OK) {
        return H2_RET_ERROR;
    }

    this->EncInst = NULL;
    return H2_RET_OK;
}

bool IPU_H2::IsHevcReady(void) {
    return this->EncInst != NULL ? true : false;
}

int IPU_H2::HevcEncodeSetPreProcessCfg(void){
    HEVCEncRet ret ;

    this->MutexHwLock->lock();
    HEVCEncOn(this->EncInst);
    ret = HEVCEncGetPreProcessing(this->EncInst,&this->EncPreProcessingCfg);
    this->MutexHwLock->unlock();

    if (ret != HEVCENC_OK ){
        LOGE("hevc get pre processing error -> %d \n",ret);
        return H2_RET_ERROR ;
    }

    this->EncPreProcessingCfg.inputType = HevcSwitchFormat(this->VencPpCfg.srcFrameType);
    this->EncPreProcessingCfg.origWidth = this->VencPpCfg.originW ;
    this->EncPreProcessingCfg.origHeight = this->VencPpCfg.originH ;
    this->EncPreProcessingCfg.xOffset = this->VencPpCfg.startX;
    this->EncPreProcessingCfg.yOffset = this->VencPpCfg.startY;
    this->EncPreProcessingCfg.scaledOutput =  0 ;
    LOGD("rotation->%d\n",this->VencPpCfg.rotation);
    switch(this->VencPpCfg.rotation){
        case 0:
            this->EncPreProcessingCfg.rotation = HEVCENC_ROTATE_0;
            break;
        case 1:
            this->EncPreProcessingCfg.rotation = HEVCENC_ROTATE_90R;
            break;
        case 2:
            this->EncPreProcessingCfg.rotation = HEVCENC_ROTATE_90L;
            break;
        default :
            LOGE("hevc pp rotation error ->%d\n",this->VencPpCfg.rotation);
    }
    this->EncPreProcessingCfg.videoStabilization = 0;
    this->EncPreProcessingCfg.interlacedFrame = 0;

    this->EncPreProcessingCfg.colorConversion.type  = HEVCENC_RGBTOYUV_BT601;
    this->MutexHwLock->lock();
    HEVCEncOn(this->EncInst);
    ret = HEVCEncSetPreProcessing(this->EncInst,&this->EncPreProcessingCfg);
    this->MutexHwLock->unlock();
    if (ret != HEVCENC_OK ){
        LOGE("hevc set pre processing error -> %d \n",ret);
        return H2_RET_ERROR ;
    }
    LOGD("hevc set pre processing success\n");
    return H2_RET_OK ;

}
IPU_H2::~IPU_H2() {
    delete this->MutexSettingLock ;
    if (this->InstanceCounter <= 0 )
        return ;
    this->InstanceCounter--;
    if(this->InstanceCounter ==  0 ){
        delete this->MutexHwLock ;
    }
    if(this->Header != NULL ){
        free(this->Header);
        this->Header = NULL;
    }

    if (this->m_pstSei != NULL)
    {
        free(this->m_pstSei);
        this->m_pstSei = NULL;
    }

//free other resource

}
bool IPU_H2::VbrProcess(Buffer *dst)
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

    if ((this->pre_rtn_quant == this->VencRateCtrl.qp_min) && (frame_size < target_framesize_min))
        goto skip_integrate_err;
    else if ((this->pre_rtn_quant == this->VencRateCtrl.qp_max) && (frame_size > target_framesize_max))
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

    if (rtn_quant > this->VencRateCtrl.qp_max) {
        rtn_quant = this->VencRateCtrl.qp_max;
        if(this->vbr_pic_skip && this->VencRateCtrl.picture_skip == 0) {
            this->LockSettingResource();
            this->VencRateCtrlUpateFlag =  false ;
            this->VencRateCtrl.picture_skip = 1;
            this->VencRateCtrlUpateFlag =  true ;
            this->UnlockSettingResource();
        }
    } else if (rtn_quant < this->VencRateCtrl.qp_min) {
        rtn_quant = this->VencRateCtrl.qp_min;
    } else {
        if(this->vbr_pic_skip && this->VencRateCtrl.picture_skip == 1) {
            this->LockSettingResource();
            this->VencRateCtrlUpateFlag =  false ;
            this->VencRateCtrl.picture_skip = 0;
            this->VencRateCtrlUpateFlag =  true ;
            this->UnlockSettingResource();
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
        this->LockSettingResource();
        this->VencRateCtrlUpateFlag =  false ;
        this->VencRateCtrl.qp_hdr = rtn_quant;
        this->VencRateCtrlUpateFlag =  true ;
        this->UnlockSettingResource();
        this->pre_rtn_quant = rtn_quant;
    }
    return true;
}

bool IPU_H2::VbrAvgProcess(Buffer *dst)
{
    uint64_t out_bitrate;
    if(this->counter == this->VencRateCtrl.gop_length) {
        this->total_sizes += dst->fr_buf.size;
        this->total_time += dst->fr_buf.timestamp - this->last_frame_time;
        this->total_sizes -= this->aver_size;
        this->total_time -= this->aver_time;
        out_bitrate = this->total_sizes * 8 * 1000/ this->total_time;
        this->last_frame_time = dst->fr_buf.timestamp;
        this->aver_time = this->total_time/this->counter;
        this->aver_size = this->total_sizes/this->counter;
        this->VencRateCtrl.bitrate = out_bitrate;
        if(out_bitrate > this->MaxBitrate) {
            if(this->VencRateCtrl.qp_hdr < this->VencRateCtrl.qp_max) {
                this->LockSettingResource();
                this->VencRateCtrl.qp_hdr++;
                this->VencRateCtrlUpateFlag =  true ;
                this->UnlockSettingResource();
            }
        } else if (out_bitrate < this->MaxBitrate * this->VBRThreshHold / 100) {
            if(this->VencRateCtrl.qp_hdr > this->VencRateCtrl.qp_min) {
                this->LockSettingResource();
                this->VencRateCtrl.qp_hdr--;
                this->VencRateCtrlUpateFlag =  true ;
                this->UnlockSettingResource();
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
bool IPU_H2::TrylockHwResource(void){
    return this->MutexHwLock->try_lock();
}
bool IPU_H2::LockHwResource(void){
    this->MutexHwLock->lock();
    return true;
}
bool IPU_H2::UnlockHwResource(void){
    this->MutexHwLock->unlock();
    return true;
}
void IPU_H2::SetIndex(int Index){
    this->Index = Index;
}
bool IPU_H2::TryLockSettingResource(void){
    return this->MutexSettingLock->try_lock();
}
bool IPU_H2::LockSettingResource(void){
    this->MutexSettingLock->lock();
    return true ;
}
bool IPU_H2::UnlockSettingResource(void){
    this->MutexSettingLock->unlock();
    return true ;
}
bool IPU_H2::GetLevelFromResolution(uint32_t x,uint32_t  y ,int *level) {
    bool ret =  true;
    HEVCEncLevel temp_level =  HEVCENC_LEVEL_6 ;
#define HEVCENC_MAX_FRAME_SIZE_NUM 7
    uint32_t width[HEVCENC_MAX_FRAME_SIZE_NUM]  =  {  176,352,640,960,1920,4096,8192 };
    uint32_t height[HEVCENC_MAX_FRAME_SIZE_NUM] =  {  144,288,360,540,1080,2160,4320 } ;
    int i ;
    int j ;
    for (i =1 ; i< HEVCENC_MAX_FRAME_SIZE_NUM ; i++ ){
        if(x <  width[i] )
            break;
        if (x > width[i] && x <= width[i+1] )
        {
            break;
        }
        else{
            continue ;
        }
    }
    for(j = 1  ;j < HEVCENC_MAX_FRAME_SIZE_NUM ; j++ ){
        if (y <  height[j] ) {
            break;
        }
        if (y > height[j] && y<=height[j+1] ) {
            break;
        }
        else {
            continue ;
        }
    }
    LOGD("target level range -> %d %d\n",i,j);
    i = i>j  ? i:j ;

    switch (i ){
        case 1:
            temp_level = HEVCENC_LEVEL_2 ;
            break ;
        case 2:
            temp_level = HEVCENC_LEVEL_2_1 ;
            break ;
        case 3:
            temp_level = HEVCENC_LEVEL_3 ;
            break ;
        case 4:
            temp_level = HEVCENC_LEVEL_1 ;
            break ;
        case 5:
            temp_level = HEVCENC_LEVEL_1 ;
            break ;
        case 6:
            temp_level = HEVCENC_LEVEL_1 ;
            break ;
        case 7:
            temp_level = HEVCENC_LEVEL_1 ;
            break ;
        default :
            ret = false ;
            break;
    }
    if(ret == true )
        *level = (int) temp_level ;
    return ret ;
}
bool IPU_H2::CheckAreaValid(struct v_roi_area *cfg){
#if 1
    if ( cfg->x >= this->EncConfig.width  ){
        LOGD("roi offset x error ->%d \n",cfg->x);
        return false ;
    }
    if ( cfg->y >= this->EncConfig.height  ){
        LOGD("roi offset Y error ->%d \n",cfg->y);
        return false ;
    }
    if (  ( cfg->w + cfg->x) > this->EncConfig.width ){
        LOGD("roi width error ->%d \n",cfg->w);
        return false ;
    }
    if ( ( cfg->h + cfg->y) > this->EncConfig.height ){
        LOGD("roi height error ->%d \n",cfg->h);
        return false ;
    }
#endif
    return true ;
}

int IPU_H2::SaveHeader(char *buf, int len)
{
    LOGD("H2 header saved %d bytes\n", len);

    memcpy(Header, buf, len);
    HeaderLength = len;
    return 0;
}

int IPU_H2::GetBasicInfo(struct v_basic_info *cfg) {

    this->VencEncodeConfig.media_type = VIDEO_MEDIA_HEVC;
    *cfg =  this->VencEncodeConfig;
    return 0;
}

int IPU_H2::SetBasicInfo(struct v_basic_info *cfg) {
    this->LockSettingResource();
    this->VencEncodeConfig = *cfg ;
    this->VencEncodeConfig.media_type = VIDEO_MEDIA_HEVC;
    this->VencEncodeConfigUpdateFlag =  true;
    this->UnlockSettingResource();
    return 0;
}

int IPU_H2::GetBitrate(struct v_bitrate_info *cfg) {
    *cfg =  this->VencRateCtrl ;
    if(cfg->rc_mode == VENC_VBR_MODE){
        LOGE("vbr mode ,set bitrate = 0,will add auto cal later");
        cfg->bitrate = 0;
    }
    return 0;
}

/* not maintain from 10.23.2017 */
int IPU_H2::SetBitrate(struct v_bitrate_info *cfg) {
    this->LockSettingResource();
    this->VencRateCtrlUpateFlag =  false ;
    //basic rate ctrl data check
    if(this->VencRateCtrl.rc_mode != cfg->rc_mode)
    {
        m_u8RateCtrlReset = 2;
    }
    this->VencRateCtrl = *cfg;
    if(this->VencRateCtrl.p_frame == 0){
        this->VencRateCtrl.p_frame = this->Infps == 0 ? 10 : this->Infps ;
        LOGE("Please fix your p_frame value ,current use default value :%d\n",
                this->VencRateCtrl.p_frame);
    }
    if(this->VencRateCtrl.gop_length == 0 ){
        this->VencRateCtrl.gop_length =  this->VencRateCtrl.p_frame;
        LOGE("Please fix your gop_length value ,current use default value:%d\n",
                this->VencRateCtrl.gop_length);
    }
    if(this->VencRateCtrl.qp_min < 0 || this->VencRateCtrl.qp_min > 51){
        this->VencRateCtrl.qp_min =  10;
        LOGE("Please fix your qp_min value ,current use default value %d\n",
                this->VencRateCtrl.qp_min);
    }
    if(this->VencRateCtrl.qp_max < 0 || this->VencRateCtrl.qp_max > 51){
        this->VencRateCtrl.qp_max =  51;
        LOGE("Please fix your qp_max value ,current use default value %d\n",
                this->VencRateCtrl.qp_max);
    }
    if(this->VencRateCtrl.qp_hdr < -1 || this->VencRateCtrl.qp_hdr > 51){
        this->VencRateCtrl.qp_hdr =  -1;
        LOGE("Please fix your qp_hdr value ,current use default value %d\n",
                this->VencRateCtrl.qp_hdr);
    }
    if(cfg->rc_mode != VENC_CBR_MODE && cfg->rc_mode != VENC_VBR_MODE){
        LOGE("rc_mode set error, VENC_CBR_MODE or VENC_VBR_MODE\n");
        return VBEBADPARAM;
    }
    this->IdrInterval = this->VencRateCtrl.p_frame;
    this->counter = 0;
    this->VencRateCtrlUpateFlag =  true ;
    this->UnlockSettingResource();
    return 0;
}

int IPU_H2::GetRateCtrl(struct v_rate_ctrl_info *cfg)
{
    if(cfg == NULL) {
        LOGE("invalid param\n");
        return VBEBADPARAM;
    } else {
        memset(cfg, 0, sizeof(struct v_rate_ctrl_info));
    }
    this->LockSettingResource();
    switch (this->VencRateCtrl.rc_mode) {
        case VENC_CBR_MODE:
            cfg->cbr.qp_max = this->VencRateCtrl.qp_max;
            cfg->cbr.qp_min = this->VencRateCtrl.qp_min;
            cfg->cbr.qp_delta = this->VencRateCtrl.qp_delta;
            cfg->cbr.qp_hdr = this->VencRateCtrl.qp_hdr;
            cfg->cbr.bitrate = this->VencRateCtrl.bitrate;
            cfg->picture_skip = this->VencRateCtrl.picture_skip;
            break;
        case VENC_VBR_MODE:
            cfg->vbr.qp_max = this->VencRateCtrl.qp_max;
            cfg->vbr.qp_min = this->VencRateCtrl.qp_min;
            cfg->vbr.qp_delta = this->VencRateCtrl.qp_delta;
            cfg->vbr.max_bitrate = this->MaxBitrate;
            cfg->vbr.threshold = this->VBRThreshHold;
            cfg->picture_skip = this->vbr_pic_skip;
            break;
        case VENC_FIXQP_MODE:
            cfg->fixqp.qp_fix = this->VencRateCtrl.qp_hdr;
            cfg->picture_skip = this->VencRateCtrl.picture_skip;
            break;
        default:
            LOGE("invalid rc mode\n");
            break;
    }
    cfg->rc_mode = this->VencRateCtrl.rc_mode;
    cfg->gop_length = this->VencRateCtrl.gop_length;
    cfg->idr_interval = this->VencRateCtrl.p_frame;
    cfg->hrd = this->VencRateCtrl.hrd;
    cfg->hrd_cpbsize = this->VencRateCtrl.hrd_cpbsize;
    cfg->refresh_interval = this->VencRateCtrl.refresh_interval;
    this->UnlockSettingResource();
    return 0;
}

int IPU_H2::SetRateCtrl(struct v_rate_ctrl_info *cfg)
{
    if(!cfg || cfg->rc_mode < VENC_CBR_MODE || cfg->rc_mode > VENC_FIXQP_MODE || cfg->idr_interval <= 0 ||
        cfg->gop_length < 1 || cfg->gop_length > 150 || cfg->picture_skip < 0 || cfg->picture_skip > 1 ||
        cfg->hrd < 0 || cfg->hrd > 1 || cfg->hrd_cpbsize < 0) {
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

    this->LockSettingResource();
    this->VencRateCtrlUpateFlag =  false ;

    switch (cfg->rc_mode) {
        case VENC_CBR_MODE:
            if(this->VencRateCtrl.bitrate != cfg->cbr.bitrate)
            {
                m_u8RateCtrlReset = 2;
            }
            this->VencRateCtrl.qp_max = cfg->cbr.qp_max;
            this->VencRateCtrl.qp_min = cfg->cbr.qp_min;
            this->VencRateCtrl.qp_delta = cfg->cbr.qp_delta;
            this->VencRateCtrl.qp_hdr = cfg->cbr.qp_hdr;
            this->VencRateCtrl.bitrate = cfg->cbr.bitrate;
            this->VencRateCtrl.picture_skip = cfg->picture_skip;
            this->vbr_pic_skip = 0;
            break;
        case VENC_VBR_MODE:
            if(this->MaxBitrate != (uint64_t)cfg->vbr.max_bitrate || this->VBRThreshHold != (uint32_t)cfg->vbr.threshold)
            {
                m_u8RateCtrlReset = 2;
            }
            this->VencRateCtrl.qp_min = cfg->vbr.qp_min;
            this->VencRateCtrl.qp_max = cfg->vbr.qp_max;
            this->VencRateCtrl.qp_delta = cfg->vbr.qp_delta;
            if(this->VencRateCtrl.rc_mode != VENC_VBR_MODE)
            {
                this->VencRateCtrl.qp_hdr = -1;
            }
            this->MaxBitrate = cfg->vbr.max_bitrate;
            this->VBRThreshHold = cfg->vbr.threshold;
            this->total_frames = 0;
            this->total_size = 0;
            this->counter = 0;
            this->avg_framesize = (double)this->MaxBitrate * (this->VBRThreshHold + 100) / 200 / 8.0 / this->CurFps;
            this->vbr_pic_skip = cfg->picture_skip;
            if(!this->vbr_pic_skip)
                this->VencRateCtrl.picture_skip = 0;
            break;
        case VENC_FIXQP_MODE:
            this->VencRateCtrl.qp_hdr = cfg->fixqp.qp_fix;
            this->VencRateCtrl.qp_max = cfg->fixqp.qp_fix;
            this->VencRateCtrl.qp_min = cfg->fixqp.qp_fix;
            this->VencRateCtrl.picture_skip = cfg->picture_skip;
            this->vbr_pic_skip = 0;
            break;
        default:
            LOGE("invalid rc mode\n");
            break;
    }
    if(this->VencRateCtrl.rc_mode != cfg->rc_mode)
    {
        m_u8RateCtrlReset = 2;
    }
    this->VencRateCtrl.rc_mode = cfg->rc_mode;
    this->VencRateCtrl.gop_length = cfg->gop_length;
    this->VencRateCtrl.p_frame = cfg->idr_interval;
    this->VencRateCtrl.hrd = cfg->hrd;
    this->VencRateCtrl.hrd_cpbsize = cfg->hrd_cpbsize;
    this->VencRateCtrlUpateFlag =  true ;
    this->UnlockSettingResource();
    return 0;
}

int IPU_H2::GetROI(struct v_roi_info *cfg) {
    *cfg =  this->VencRoiAreaInfo;
    return 0;
}
int IPU_H2::SetROI(struct v_roi_info *cfg) {
    int ret  =  0;
    this->LockSettingResource();
    this->VencCodingCtrlUpdateFlag =  false ;
    bool validFlag = true ;
    for (int i = 0 ; i < VENC_H265_ROI_AREA_MAX_NUM ;i++ ){
        if (!cfg || cfg->roi[i].qp_offset > 0 || cfg->roi[i].qp_offset < -15)
        {
            validFlag = false;
            break;
        }
        if (this->CheckAreaValid((cfg->roi) +i )  == true) {
            validFlag = true ;
        }
        else {
            validFlag = false ;
            break;
        }
    }
    if(validFlag){
        this->VencRoiAreaInfo =  *cfg ;
        this->VencCodingCtrlUpdateFlag =  true;
    }
    else {
        ret = VBEBADPARAM;
    }

    this->UnlockSettingResource();
    return ret ;
}
int IPU_H2::TriggerKeyFrame(void){
    this->LockSettingResource();
    this->TriggerKeyFrameFlag = true ;
    this->UnlockSettingResource();
    return 1;
}
int IPU_H2::SetScenario(enum video_scenario mode){
    LOGE("%s->line:%d ->mode:%d\n",__FUNCTION__,__LINE__,mode);
    switch(this->VencPpCfg.srcFrameType){

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
            LOGE("input src type -> %d ,not support scenario set ,defualt day mode \n", this->VencPpCfg.srcFrameType);
            return VBEBADPARAM;
    }
    this->LockSettingResource();
    this->VideoScenarioMode = mode;
    this->UnlockSettingResource();
    return 1;
}
enum video_scenario IPU_H2::GetScenario(void){
    return this->VideoScenarioMode;
}

int IPU_H2::SetFrameMode(enum v_enc_frame_type arg)
{
    frame_mode = arg;
    return 1;
}

int IPU_H2::GetFrameMode()
{
    return frame_mode;
}

int IPU_H2::SetFrc(v_frc_info *info){
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
int IPU_H2::GetFrc(v_frc_info *info){
    this->MutexSettingLock->lock();
    *info = this->FrcInfo;
    this->MutexSettingLock->unlock();
    return 1;
}

int IPU_H2::SetPipInfo(struct v_pip_info *vpinfo)
{
    if(strncmp(vpinfo->portname, "frame", 5) == 0)
    {
        GetPort("frame")->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
    }
    return 0;
}

int IPU_H2::GetPipInfo(struct v_pip_info *vpinfo)
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

bool IPU_H2::CheckFrcUpdate(void){
    if(this->InfpsRatio ==  -1){
        if(this->FrcInfo.framerate == 0){
            this->InfpsRatio = 1;
            this->FrcInfo.framebase = 1;
            this->FrcInfo.framerate = this->Infps;
        }
        int targetInfps = this->FrcInfo.framerate / this->FrcInfo.framebase;
        int quotient = 0;
        int remainder = 0;
        if(targetInfps == this->Infps || (0)){ //TODO: add some more check for little error
            this->InfpsRatio = 1;
        }
        else if(targetInfps > this->Infps){
            //fill frame
            quotient = targetInfps / this->Infps;
            remainder = targetInfps % this->Infps;
            if(remainder > (this->Infps / 2))
            {
                quotient++;
            }
            if(remainder != 0)
            {
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

        LOGE("target fps ************->%2d %3d %8d\n", targetInfps, this->InfpsRatio, this->FrameDuration);
        return true;
    }
    else{
        return false;
    }
}

int IPU_H2::GetRTBps()
{
    return this->rtBps;
}

void IPU_H2::VideoInfo(Buffer *dst)
{
    if(this->t_cnt >= this->VencRateCtrl.gop_length*5) {
        if(dst->fr_buf.timestamp == this->t_last_frame_time)
        {
            rtBps = (this->VencRateCtrl.rc_mode == VENC_CBR_MODE) ? VencRateCtrl.bitrate : (MaxBitrate * (100 + VBRThreshHold) / 100);
            return;
        }
        this->AvgFps = this->t_fps_cnt * 1000 / (dst->fr_buf.timestamp - this->t_last_frame_time);
        if(this->CurFps == 0)
        {
            this->CurFps = this->AvgFps;
        }
        if(abs(this->CurFps - this->AvgFps) > 1)
        {
            this->CurFps = this->AvgFps;
            this->IsFPSChange = true;
        }
        this->t_cnt = 0;
        this->t_fps_cnt = 0;
        this->rtBps = this->t_total_size * 8 * 1000 / (dst->fr_buf.timestamp - this->t_last_frame_time);
#if 0
        LOGE("avgfps: %d\n", this->AvgFps);
        LOGE("t_out_bitrate: \t\t\t%d\n", this->t_total_size * 8 * 1000 / (dst->fr_buf.timestamp - this->t_last_frame_time));
        LOGE("t_fps:\t\t\t%d\n", this->CurFps);
        LOGE("rc_mode:\t\t\t%d\n", this->VencRateCtrl.rc_mode);
#endif
        this->MutexSettingLock->lock();
        if(((this->VencRateCtrl.rc_mode == VENC_CBR_MODE && rtBps > (uint64_t) this->VencRateCtrl.bitrate * 9 / 10) /* Just Reserve a Gap */
            || (this->VencRateCtrl.rc_mode == VENC_VBR_MODE && rtBps > MaxBitrate))
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

void IPU_H2::UpdateDebugInfo(int s32Update)
{
    if(s32Update)
    {
        m_stVencInfo.stRcInfo.s8MaxQP = VencRateCtrl.qp_max;
        m_stVencInfo.stRcInfo.s8MinQP = VencRateCtrl.qp_min;
        m_stVencInfo.stRcInfo.s32QPDelta = VencRateCtrl.qp_delta;

        m_stVencInfo.stRcInfo.s32Gop = VencRateCtrl.gop_length;
        m_stVencInfo.stRcInfo.s32IInterval = VencRateCtrl.p_frame;
        m_stVencInfo.stRcInfo.s32Mbrc = VencRateCtrl.mbrc;
        m_stVencInfo.f32BitRate = rtBps/(1024.0);
        m_stVencInfo.f32FPS = CurFps;

        if (VencRateCtrl.rc_mode == VENC_CBR_MODE)
        {
            m_stVencInfo.stRcInfo.enRCMode = CBR;
            m_stVencInfo.stRcInfo.s32PictureSkip = VencRateCtrl.picture_skip;
            HEVCEncGetRealQp(this->EncInst,&m_stVencInfo.stRcInfo.s32RealQP);
            m_stVencInfo.stRcInfo.f32ConfigBitRate = VencRateCtrl.bitrate/(1024.0);
            m_stVencInfo.stRcInfo.u32ThreshHold = 0;
        }
        else if (VencRateCtrl.rc_mode == VENC_VBR_MODE)
        {
            m_stVencInfo.stRcInfo.enRCMode = VBR;
            m_stVencInfo.stRcInfo.s32PictureSkip = vbr_pic_skip;
            m_stVencInfo.stRcInfo.s32RealQP = VencRateCtrl.qp_hdr;
            m_stVencInfo.stRcInfo.f32ConfigBitRate = MaxBitrate/(1024.0);
            m_stVencInfo.stRcInfo.u32ThreshHold = VBRThreshHold;
        }
        else
        {
            m_stVencInfo.stRcInfo.enRCMode = FIXQP;
            m_stVencInfo.stRcInfo.s32PictureSkip = VencRateCtrl.picture_skip;
            m_stVencInfo.stRcInfo.s8MaxQP = m_stVencInfo.stRcInfo.s8MinQP = VencRateCtrl.qp_hdr;
            m_stVencInfo.stRcInfo.s32RealQP = VencRateCtrl.qp_hdr;
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
        m_stVencInfo.u32FrameDrop= 0;
    }

}

int IPU_H2::SetSeiUserData(ST_VIDEO_SEI_USER_DATA_INFO *pstSei)
{
    if ((pstSei == NULL) || (pstSei->length > VIDEO_SEI_USER_DATA_MAXLEN)\
           || (pstSei->length <= 0))
    {
        LOGE("sei param error\n");
        return VBEBADPARAM;
    }

    if (this->TryLockSettingResource())
    {
        if (!this->m_bSEIUserDataSetFlag)
        {
            if (m_pstSei == NULL)
            {
                this->m_pstSei = (ST_VIDEO_SEI_USER_DATA_INFO*)malloc(sizeof(ST_VIDEO_SEI_USER_DATA_INFO));
            }
            if (this->m_pstSei == NULL)
            {
                LOGE("malloc sei info error\n");
                this->UnlockSettingResource();
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
            LOGE("seting last sei info, try later.\n");
            this->UnlockSettingResource();
            return VBEFAILED;
        }
        this->UnlockSettingResource();
        return VBOK;
    }
    else
    {
        return VBEFAILED;
    }
}

int IPU_H2::GetDebugInfo(void *pInfo, int *ps32Size)
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
