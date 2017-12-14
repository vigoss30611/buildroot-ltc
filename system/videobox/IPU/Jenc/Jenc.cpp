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
#include "Jenc.h"
#include <sys/time.h>

#define CALYUVSIZE(w,h) ((((w) + 15)&(~0x0f)) * (((h) + 3)&(~0x03)))
#define DUMP 0
#define APP1_XMP_ID "http://ns.adobe.com/xap/1.0/"
//#define DEBUG
#ifdef DEBUG
    #define LOG_DBG(args...) LOGE(args)
#else
    #define LOG_DBG(args...)
#endif

const char *g_google_panorama_xmp[] =
{
"<?xpacket begin='\xEF\xBB\xBF' id='W5M0MpCehiHzreSzNTczkc9d'?>\n\
<x:xmpmeta xmlns:x='adobe:ns:meta/' x:xmptk='Image::ExifTool 9.46'>\n\
<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>\n\
<rdf:Description rdf:about=''\n\
xmlns:GPano='http://ns.google.com/photos/1.0/panorama/'>\n\
<GPano:CroppedAreaImageHeightPixels>",\
/*Insert Height*/\
"</GPano:CroppedAreaImageHeightPixels>\n\
<GPano:CroppedAreaImageWidthPixels>",\
/*Insert Width*/\
"</GPano:CroppedAreaImageWidthPixels>\n\
<GPano:CroppedAreaLeftPixels>0</GPano:CroppedAreaLeftPixels>\n\
<GPano:CroppedAreaTopPixels>0</GPano:CroppedAreaTopPixels>\n\
<GPano:FullPanoHeightPixels>",\
/*Insert Height*/\
"</GPano:FullPanoHeightPixels>\n\
<GPano:FullPanoWidthPixels>",\
/*Insert Width*/\
"</GPano:FullPanoWidthPixels>\n\
<GPano:ProjectionType>equirectangular</GPano:ProjectionType>\n\
</rdf:Description>\n\
</rdf:RDF>\n\
</x:xmpmeta>\n\
<?xpacket end='w'?>"
};

DYNAMIC_IPU(IPU_JENC, "jenc");

static int switchFormat(Pixel::Format srcFormat)
{
    if(srcFormat == Pixel::Format::NV12) //Pixel::Format::NV12
        return JPEGENC_NV12;// 0x20001
    else if(srcFormat == Pixel::Format::NV21) //Pixel::Format::NV21
        return JPEGENC_NV21;  //0x20003
    else
        throw VBERR_BADPARAM;
}

void IPU_JENC::WorkLoop()
{
    Buffer bfin, bfJpout;
    Port *pin, *pJpout;
    int skip = 0;
    u32 idx = 0, u32StrLen = 0;
    u8 *u8XmpData = NULL;
    #if TIMEUSAGE
    float timeUsage;
    struct timeval tmFrameStart_tmp, tmFrameEnd_tmp;
    #endif

    pin = GetPort("in");
    pJpout = GetPort("out");
    if ((enJencRet = JpegEncInitSyn(&this->pJpegEncCfg, &this->pJpegEncInst)) != JPEGENC_OK){
        LOGE("failed to initialize the jenc encoder. Error code: %i\n", enJencRet);
        throw VBERR_BADPARAM;
     }

    while(IsInState(ST::Running)){
        try{
            if(Mode == IPU_JENC::Trigger) {
                if(!GetTrigger()) throw "out";
                else { // Triggered
                    #if TIMEUSAGE
                    gettimeofday(&tmFrameStart_tmp, NULL);
                    #endif
                    bfin = pin->GetBuffer();
                    if(bfin.fr_buf.serial <= SnapSerial) {
                        skip ++;
                        if (skip % 40 == 0)
                        LOGD("no new frames, %d, %d\n", bfin.fr_buf.serial, SnapSerial);
                        pin->PutBuffer(&bfin);
                        throw "out";
                    }
                    SnapSerial = bfin.fr_buf.serial;
                    LOGD("got frame: %d\n", SnapSerial);
                }
            } else
                bfin = pin->GetBuffer(&bfin);
        } catch(const char* err) {
            usleep(20000);
            this->m_stVencInfo.enState = IDLE;
            continue;
        }

        this->m_stVencInfo.enState = RUN;

        if (m_s32ThumbnailEnable)
        {
            pthread_mutex_lock(&m_stUpdateMut);
            m_s32ThumbnailWidth = m_stThumbResolution.w;
            m_s32ThumbnailHeight = m_stThumbResolution.h;
            CheckThumbnailRes();
            pthread_mutex_unlock(&m_stUpdateMut);

            enJencRet = EncThumnail(&bfin);
            if (enJencRet != JPEGENC_OK)
            {
                LOGE("Encode thumbnail failed!\n");
            }
        }
        m_stVencInfo.u64InCount++;
        bfJpout = pJpout->GetBuffer();
        //WriteFile((char *)(bfin.fr_buf.virt_addr), (pin->GetWidth())*(pin->GetHeight())*3/2,"/Input_%d_%d.yuv",pin->GetWidth(), pin->GetHeight());
        if ((enJencRet = JpegEncSetPictureSize(pJpegEncInst, &pJpegEncCfg)) != JPEGENC_OK) {
            CheckEncCfg(&pJpegEncCfg);
            LOGE("error:failed to JpegEncSetPictureSize. Error code: %i\n", enJencRet);
            m_stVencInfo.u32FrameDrop++;
            goto EXIT;
        }

        if (qualityFlag) {
            if ((enJencRet = JpegEncSetQuality(pJpegEncInst, &pJpegEncCfg)) != JPEGENC_OK) {
                LOGE("warning:failed to set quality. Error code: %i\n", enJencRet);
            } else {
                LOGE("set quality to %d \n", pJpegEncCfg.qLevel);
            }
            qualityFlag = false;
        }
        pthread_mutex_lock(&m_stUpdateMut);
        if(true == m_bApp1Enable)
        {
            pJpegEncIn.app1 = 1;
            pJpegEncIn.xmpId = (u8 *)APP1_XMP_ID;
            if(pJpegEncIn.xmpData == NULL)
            {
                pJpegEncIn.xmpData = (u8 *)malloc(m_u32XmpDataMaxLength);
            }
            if(pJpegEncIn.xmpData != NULL)
            {
                memset(pJpegEncIn.xmpData, 0 ,m_u32XmpDataMaxLength);
                u8XmpData = pJpegEncIn.xmpData;
                for(idx = 0; idx < sizeof(g_google_panorama_xmp)/sizeof(*g_google_panorama_xmp); idx++)
                {
                    u32StrLen = strlen(g_google_panorama_xmp[idx]);
                    memcpy(u8XmpData,g_google_panorama_xmp[idx],u32StrLen);
                    u8XmpData = u8XmpData + u32StrLen;
                    //match g_google_panorama_xmp insert skip last
                    if(idx < sizeof(g_google_panorama_xmp)/sizeof(*g_google_panorama_xmp) - 1)
                    {
                        if(idx%2 == 0)
                            sprintf((char *)u8XmpData,"%d", pJpegEncCfg.inputHeight);
                        else
                            sprintf((char *)u8XmpData,"%d", pJpegEncCfg.inputWidth);
                    }
                    u8XmpData = pJpegEncIn.xmpData + strlen((const char *)pJpegEncIn.xmpData);
                }
            }
            else
            {
                pJpegEncIn.app1 = 0;
            }
            //LOGE("m_u32XmpDataMaxLength:%d pJpegEncIn.xmpData:%s\n",m_u32XmpDataMaxLength,pJpegEncIn.xmpData);
        }
        else
        {
            pJpegEncIn.app1 = 0;
        }
        pthread_mutex_unlock(&m_stUpdateMut);

        enJencRet = (JpegEncRet)StartJpegEncode(&bfin, &bfJpout);
        if (enJencRet != JPEGENC_OK)
        {
            m_stVencInfo.u32FrameDrop++;
        }
        #if TIMEUSAGE
        gettimeofday(&tmFrameEnd_tmp, NULL);
        timeUsage = (float)(tmFrameEnd_tmp.tv_sec - tmFrameStart_tmp.tv_sec) + (tmFrameEnd_tmp.tv_usec - tmFrameStart_tmp.tv_usec)/(1000000.0);
        printf("encode one frame time usage:%f tmFrameEnd.tv_sec - tmFrameStart.tv_sec:%ld\n",timeUsage,tmFrameEnd.tv_sec - tmFrameStart.tv_sec);
        #endif

EXIT:
        pJpout->PutBuffer(&bfJpout);
        m_stVencInfo.u64OutCount++;
        pin->PutBuffer(&bfin);

        if (Mode == IPU_JENC::Trigger) {
            ClearTrigger();
        }
    }
    LOGE("out workloop\n");
    this->m_stVencInfo.enState = STOP;
    m_stVencInfo.u64InCount = 0;
    m_stVencInfo.u64OutCount = 0;
    SnapSerial = -1;
}

IPU_JENC::IPU_JENC(std::string name, Json *js)
{
    this->Name = name;
    Port *pjp_out, *pjp_in;//, *h264_out;
    Mode = IPU_JENC::Trigger;
    int bufferSize = 0x200000;
    const char *ps8ThumbnailMode = NULL;
    u32 idx = 0;

    if(NULL != js){
        if(strncmp(js->GetString("mode"), "trigger", 8) == 0)
            Mode = IPU_JENC::Trigger;
        else
            Mode = IPU_JENC::Auto;
        if(0 != js->GetInt("buffer_size"))
            bufferSize = js->GetInt("buffer_size");
    }

    pjp_in = CreatePort("in", Port::In);
    pjp_out = CreatePort("out", Port::Out);

    if(Mode == IPU_JENC::Trigger){
        pjp_out->SetPixelFormat(Pixel::Format::NV12);
        pjp_out->SetResolution(100, 100);//really realloc SetSnapParams funciton
        pjp_out->SetBufferType(FRBuffer::Type::FIXED, 1);

    }else{
        pjp_out->SetBufferType(FRBuffer::Type::FLOAT, bufferSize, bufferSize / 4 - 0x4000);
        pjp_out->SetPixelFormat(Pixel::Format::JPEG);
    }

    m_s32ThumbnailWidth = 128;
    m_s32ThumbnailHeight = 96;
    m_s32ThumbnailMode = 0;
    m_s32ThumbnailEnable = false;
    if (NULL != js->GetObject("thumbnail_mode"))
    {
        ps8ThumbnailMode = js->GetString("thumbnail_mode");
    }
    else
    {
        ps8ThumbnailMode = js->GetString("thumbnailMode");
    }
    LOGE("thumbnailMode:%s\n", ps8ThumbnailMode);
    if (NULL != ps8ThumbnailMode)
    {
        #ifdef COMPILE_OPT_JPEG_SWSCALE
        if (strncmp(ps8ThumbnailMode, "intra", 8) == 0)
        {
            LOG_DBG("set thumbnailMode:intra\n");
            m_s32ThumbnailMode = INTRA;
        }
        else if (strncmp(ps8ThumbnailMode, "extra", 8) == 0)
        {
            LOG_DBG("set thumbnailMode:extra\n");
            m_s32ThumbnailMode = EXTRA;
        }

        if (m_s32ThumbnailMode != 0)
        {
            m_s32ThumbnailEnable = true;
            m_s32ThumbnailWidth = js->GetInt("thumbnail_width");
            m_s32ThumbnailHeight = js->GetInt("thumbnail_height");
            if (m_s32ThumbnailWidth <= 0 || m_s32ThumbnailHeight <= 0)
            {
                m_s32ThumbnailWidth = js->GetInt("thumbnailWidth");
                m_s32ThumbnailHeight = js->GetInt("thumbnailHeight");
            }
            CheckThumbnailRes();
        }
    #else
        LOGE("Warning:Jpeg thumbnail need scale encoding support,please open jenc option macro!\n");
    #endif
    }
    LOG_DBG("thumbnailMode:%d thumbnailWidth:%d thumbnailHeight:%d\n",m_s32ThumbnailMode, m_s32ThumbnailWidth, m_s32ThumbnailHeight);
    memset(&pJpegEncCfg, 0, sizeof(JpegEncCfg));
    memset(&m_stThumbJpegEncCfg, 0, sizeof(JpegEncCfg));
    memset(&m_stThumbResolution, 0, sizeof(vbres_t));

    pjp_out->Export();
    pjp_in->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
    SnapSerial = -1;
    memset(&pJpegEncIn, 0, sizeof(pJpegEncIn));
    m_bApp1Enable= false;
    m_u32XmpDataMaxLength = 0;
    for(idx = 0; idx < sizeof(g_google_panorama_xmp)/sizeof(*g_google_panorama_xmp); idx++)
    {
        m_u32XmpDataMaxLength += strlen(g_google_panorama_xmp[idx]);
    }
    //reserver 32 to write value
    m_u32XmpDataMaxLength += 32;
}

void IPU_JENC::Prepare()
{
    ParamCheck();
    this->pJpegEncCfg.frameType = (JpegEncFrameType)switchFormat(GetPort("in")->GetPixelFormat());

    /* default values middle */
    this->pJpegEncCfg.qLevel = 6;
    this->pJpegEncCfg.markerType = JPEGENC_SINGLE_MARKER;
    this->pJpegEncCfg.unitsType = JPEGENC_NO_UNITS;
    this->pJpegEncCfg.xDensity = 72;
    this->pJpegEncCfg.yDensity = 72;
    this->frameSerial = 0;
    this->qualityFlag = false;
    this->Triggerred = 0;
    this->m_enIPUType = IPU_JPEG;
    this->m_stVencInfo.enType = IPU_JPEG;
    this->m_stVencInfo.enState = INIT;
    this->m_stVencInfo.enState = STATENONE;
    this->m_stVencInfo.stRcInfo.enRCMode = RCMODNONE;
    this->m_stVencInfo.f32BitRate = 0.0;
    this->m_stVencInfo.f32FPS = 0.0;
    this->m_stVencInfo.u64InCount = 0;
    this->m_stVencInfo.u64OutCount = 0;
    this->m_stVencInfo.u32FrameDrop= 0;
    this->m_stUpdateMut = PTHREAD_MUTEX_INITIALIZER;
    this->enJencRet = JPEGENC_OK;
    snprintf(this->m_stVencInfo.ps8Channel, CHANNEL_LENGTH, "%s-%s", Name.c_str(), "out");

    SetJpegEncDefaultCfgParams();
}

void IPU_JENC::Unprepare()
{
    Port *pIpuPort = NULL;

    pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if (m_stThumbnailInputBuffer.virt_addr)
    {
        fr_free_single(m_stThumbnailInputBuffer.virt_addr, m_stThumbnailInputBuffer.phys_addr);
        m_stThumbnailInputBuffer.virt_addr = NULL;
    }
    if (m_stThumbnailEncodedBuffer.virt_addr)
    {
        fr_free_single(m_stThumbnailEncodedBuffer.virt_addr, m_stThumbnailEncodedBuffer.phys_addr);
        m_stThumbnailEncodedBuffer.virt_addr = NULL;
    }
    if(pJpegEncIn.xmpData)
    {
        free(pJpegEncIn.xmpData);
        pJpegEncIn.xmpData = NULL;
    }
    if ((enJencRet = JpegEncReleaseSyn(pJpegEncInst)) != JPEGENC_OK) {
        LOGE("failed to JpegEncRelease. Error code: %i\n", enJencRet);
    }

}

void IPU_JENC::ParamCheck()
{
    Port *pIn = GetPort("in");
    Pixel::Format enPixelFormat = pIn->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21)
    {
        LOGE("in Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    enPixelFormat = GetPort("out")->GetPixelFormat();
    if((enPixelFormat != Pixel::Format::JPEG && Mode != IPU_JENC::Trigger)
        || (enPixelFormat != Pixel::Format::NV12 && Mode == IPU_JENC::Trigger))
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    if(pIn->GetWidth()%16 != 0 || pIn->GetHeight()%2 != 0 ||
        pIn->GetWidth() < 96 || pIn->GetHeight() < 32 ||
        pIn->GetWidth() > 8176 || pIn->GetHeight() > 8176){
        LOGE("Jenc param error width:%d height:%d\n",pIn->GetWidth(), pIn->GetHeight());
        throw VBERR_BADPARAM;
    }
}

bool IPU_JENC::SetQuality(int Qlevel)
{
    this->pJpegEncCfg.qLevel = Qlevel;
    qualityFlag = true;
    return true;
}

void IPU_JENC::SetTrigger() { Triggerred = 1; }
int IPU_JENC::GetTrigger() { return Triggerred; }
void IPU_JENC::ClearTrigger() { Triggerred = 0; }

void IPU_JENC::TriggerSnap()
{
    LOGE("set trigger\n");
    for(SetTrigger(); GetTrigger(); usleep(1000));
    LOGE("out of trigger\n");
}

int IPU_JENC::SetSnapParams(struct v_pip_info * arg) {
    if (arg != NULL)
        printf("IPU_H1JPEG::UnitControl video_set_snap!arg:pic_w:%d pic_h:%d xOffset:%d yOffset:%d cropW:%d cropH:%d\n",arg->pic_w, arg->pic_h, arg->x, arg->y, arg->w, arg->h);

    Port *pin = NULL;
    Port *pout = NULL;
    int oriw = 0, orih = 0;

    pin = this->GetPort("in");
    pout = this->GetPort("out");
    oriw = pin->GetWidth();
    orih = pin->GetHeight();

    if(arg != NULL) {
        LOGE("chage resolution from (%d, %d) to (%d, %d)\n",
            pJpegEncCfg.inputWidth, pJpegEncCfg.inputHeight, arg->w, pJpegEncCfg.inputHeight);
        this->pJpegEncCfg.inputWidth = arg->w;
        this->pJpegEncCfg.inputHeight = orih;
        this->pJpegEncCfg.stride = oriw;
    } else {
        this->pJpegEncCfg.inputWidth = oriw;
        this->pJpegEncCfg.inputHeight = orih;
        this->pJpegEncCfg.stride = oriw;
    }

    pout->FreeFRBuffer();
    pout->SetResolution(pJpegEncCfg.inputWidth, pJpegEncCfg.inputHeight*3/4);
    pout->AllocateFRBuffer();

    this->m_stVencInfo.s32Width = oriw;
    this->m_stVencInfo.s32Height = orih;
    return CheckEncCfg(&pJpegEncCfg);
}

int IPU_JENC::SetJpegEncDefaultCfgParams()
{
    return SetSnapParams(NULL);
}

int IPU_JENC:: CheckEncCfg(JpegEncCfg *pJpegEncCfg) {
    //check input/coding width and height followed by the spec limitation
    u32 width,height;
    if ((pJpegEncCfg->inputWidth & 0xF) != 0) {
        width = (pJpegEncCfg->inputWidth + 15)&(~0xF);
        height = (pJpegEncCfg->inputHeight + 1)&(~0x1);
        LOGE("error: Input width must be multiple of 16! suggestion-->(width ,height):(%d, %d)\n",(width > 8176)?8176:width, (height > 8176)?8176:height);
        return -1;
    }

    if ((pJpegEncCfg->inputHeight & 0x1) != 0) {
        width = (pJpegEncCfg->inputWidth + 15)&(~0xF);
        height = (pJpegEncCfg->inputHeight + 1)&(~0x1);
        LOGE("error: Input height must be multiple of 2! suggestion-->(width ,height):(%d, %d)\n",
            (width > 8176)?8176:width, (height > 8176)?8176:height);
        return -1;
    }

    if (pJpegEncCfg->inputWidth < 96 || pJpegEncCfg->inputWidth > 8176 || pJpegEncCfg->inputHeight < 32 || pJpegEncCfg->inputHeight > 8176) {
        LOGE("error:Input Width out of range [96, 8176] or Input Height out of range [32, 8176]\n");
        return -1;
    }

    if (this->pJpegEncCfg.stride < 0 || this->pJpegEncCfg.stride < pJpegEncCfg->inputWidth || (this->pJpegEncCfg.stride & 0xF) != 0)
    {
        LOGE("error:Stride(%d) is invalid\n", this->pJpegEncCfg.stride);
        return -1;
    }

    return 0;
}

int IPU_JENC::StartJpegEncode(Buffer *InBuf, Buffer *OutBuf)
{
    int PicSize = 0;
    Port *pin;

    pin = this->GetPort("in");
    PicSize = CALYUVSIZE(pin->GetWidth(), pin->GetHeight());

    //set input
    this->pJpegEncIn.busLum = InBuf->fr_buf.phys_addr;
    this->pJpegEncIn.busCb = this->pJpegEncIn.busLum + PicSize;
    this->pJpegEncIn.busCr = this->pJpegEncIn.busCb + (PicSize >> 2);

    //set output buffer
    this->pJpegEncIn.pOutBuf = (u8 *)OutBuf->fr_buf.virt_addr;
    this->pJpegEncIn.busOutBuf = OutBuf->fr_buf.phys_addr;
    this->pJpegEncIn.outBufSize = OutBuf->fr_buf.map_size;
    this->pJpegEncIn.frameHeader = 1;

    do{
        enJencRet = JpegEncEncodeSyn(this->pJpegEncInst, &this->pJpegEncIn, &this->pJpegEncOut);
        if (enJencRet < JPEGENC_OK) {
            LOGE("failed to h1 JpegEncode. Error code: %i\n", enJencRet);
            return (int)enJencRet;
        }
    }while (enJencRet != JPEGENC_FRAME_READY);

    OutBuf->fr_buf.size = this->pJpegEncOut.jfifSize;
    return (int)JPEGENC_OK;
}

int IPU_JENC::GetDebugInfo(void *pInfo, int *ps32Size)
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

JpegEncRet IPU_JENC::EncThumnail(Buffer *pstInBuf)
{
    #ifdef COMPILE_OPT_JPEG_SWSCALE
    int s32InWidth = 0, s32InHeight = 0;
    Port *pin;

    pin = GetPort("in");
    #endif
    int s32_Thumbnail_size = m_s32ThumbnailWidth*m_s32ThumbnailHeight;
    m_s32ThumbEncBufSize = ((s32_Thumbnail_size*3>>1)*3>>2) + 630;// %75 picdata + 630bytes container
    if (m_stThumbnailInputBuffer.virt_addr == NULL)
    {
        LOGD("alloc thumbnailInputBuffer!\n");
        m_stThumbnailInputBuffer.virt_addr = fr_alloc_single("thumbInBuffer", s32_Thumbnail_size*3>>1, &m_stThumbnailInputBuffer.phys_addr);
    }
    if (m_stThumbnailEncodedBuffer.virt_addr == NULL)
    {
        LOGD("alloc thumbnailEncodedBuffer!\n");
        m_stThumbnailEncodedBuffer.virt_addr = fr_alloc_single("thumbEncBuffer", m_s32ThumbEncBufSize, &m_stThumbnailEncodedBuffer.phys_addr);
    }

    #ifdef COMPILE_OPT_JPEG_SWSCALE
    s32InWidth = pin->GetWidth();
    s32InHeight = pin->GetHeight();
    if (s32InWidth != m_s32ThumbnailWidth || s32InHeight != m_s32ThumbnailHeight)
    {
        if (doSliceScale((char *)pstInBuf->fr_buf.virt_addr, s32InWidth, s32InHeight, 0, (char *)m_stThumbnailInputBuffer.virt_addr, m_s32ThumbnailWidth, m_s32ThumbnailHeight, 0, NULL) < 0)
        {
            LOGE("Error:scale fails!\n");
            return JPEGENC_ERROR;
        }
    }
    //WriteFile((char *)m_stThumbnailInputBuffer.virt_addr, m_s32ThumbnailWidth*m_s32ThumbnailHeight*3/2,"/Input_%d_%d.yuv",m_s32ThumbnailWidth, m_s32ThumbnailHeight);
    #endif
    if (qualityFlag)
    {
        if ((enJencRet = JpegEncSetQuality(pJpegEncInst, &pJpegEncCfg)) != JPEGENC_OK)
        {
            LOGE("warning:failed to Set Thumbnail Quality. Error code: %i\n", enJencRet);
        }
        else
        {
            LOGD("Thumbnail set quality to %d \n", pJpegEncCfg.qLevel);
        }
    }
    memcpy(&m_stThumbJpegEncCfg, &pJpegEncCfg, sizeof(JpegEncCfg));
    m_stThumbJpegEncCfg.inputWidth = m_s32ThumbnailWidth;
    m_stThumbJpegEncCfg.inputHeight = m_s32ThumbnailHeight;
    m_stThumbJpegEncCfg.stride = m_s32ThumbnailWidth;

    pJpegEncIn.busLum = m_stThumbnailInputBuffer.phys_addr;
    pJpegEncIn.busCb = this->pJpegEncIn.busLum + s32_Thumbnail_size;
    pJpegEncIn.busCr = this->pJpegEncIn.busCb + (s32_Thumbnail_size >> 2);
    pJpegEncIn.pOutBuf = (u8 *)m_stThumbnailEncodedBuffer.virt_addr;
    pJpegEncIn.busOutBuf = m_stThumbnailEncodedBuffer.phys_addr;
    pJpegEncIn.outBufSize = m_s32ThumbEncBufSize;
    pJpegEncIn.frameHeader = 1;
    pJpegEncIn.app1 = 0;

    if ((enJencRet = JpegEncSetPictureSize(pJpegEncInst, &m_stThumbJpegEncCfg)) != JPEGENC_OK)
    {
        CheckEncCfg(&pJpegEncCfg);
        LOGE("error:Failed to config thumbnail. Error code: %i\n", enJencRet);
        return enJencRet;
    }

    do
    {
        enJencRet = JpegEncEncodeSyn(this->pJpegEncInst, &this->pJpegEncIn, &this->pJpegEncOut);
        if (enJencRet < JPEGENC_OK)
        {
            LOGE("failed to h1 JpegEncode. Error code: %i\n", enJencRet);
            return enJencRet;
        }
    } while (enJencRet != JPEGENC_FRAME_READY);

    m_s32ThumbEncSize = pJpegEncOut.jfifSize;
    if (m_s32ThumbnailMode == INTRA)
    {
        m_stJpegThumb.data = m_stThumbnailEncodedBuffer.virt_addr;
        m_stJpegThumb.format = JPEGENC_THUMB_JPEG;
        m_stJpegThumb.width = m_s32ThumbnailWidth;
        m_stJpegThumb.height = m_s32ThumbnailHeight;
        m_stJpegThumb.dataLength = m_s32ThumbEncSize;
        if ((enJencRet = JpegEncSetThumbnail(pJpegEncInst, &m_stJpegThumb)) != JPEGENC_OK)
        {
            LOGE("Set Thumbnail Failed! ret:%d\n",enJencRet);
            return enJencRet;
        }
    }

   return JPEGENC_OK;
}

int IPU_JENC::GetThumbnail()
{
    Port *pJOut = NULL;
    Buffer bfJpout;


    if (this->m_s32ThumbnailMode != EXTRA)
    {
        LOGE("error:thumbnail mode is not extra mode(%d)!\n", this->m_s32ThumbnailMode);
        return VBEFAILED;
    }

    if (m_s32ThumbEncSize <= 0)
    {
        LOGE("error:need trigger photo first!\n");
        return VBEFAILED;
    }
    if (m_stThumbnailEncodedBuffer.virt_addr == NULL)
    {
        LOGE("error:get thumbnail fail!\n");
        return VBEFAILED;
    }
    pJOut = GetPort("out");
    bfJpout = pJOut->GetBuffer();
    memcpy((char *)bfJpout.fr_buf.virt_addr, (char *)m_stThumbnailEncodedBuffer.virt_addr, m_s32ThumbEncSize);
    bfJpout.fr_buf.size = m_s32ThumbEncSize;

    pJOut->PutBuffer(&bfJpout);
    m_s32ThumbEncSize = 0;
    return VBOK;
}


void IPU_JENC::CheckThumbnailRes()
{
    if (m_s32ThumbnailWidth > 0 && m_s32ThumbnailHeight > 0)
    {
        m_s32ThumbnailWidth = (m_s32ThumbnailWidth < 96)?96:m_s32ThumbnailWidth;
        m_s32ThumbnailHeight = (m_s32ThumbnailHeight < 32)?32:m_s32ThumbnailHeight;
        m_s32ThumbnailWidth = (m_s32ThumbnailWidth%16)?((m_s32ThumbnailWidth + 15)&(~0x0f)):m_s32ThumbnailWidth;
        m_s32ThumbnailHeight = (m_s32ThumbnailHeight%2)?((m_s32ThumbnailHeight + 1)&(~0x01)):m_s32ThumbnailHeight;
        if (m_s32ThumbnailMode == INTRA)
        {
            if (m_s32ThumbnailWidth > 255 || m_s32ThumbnailHeight > 255)
            {
                LOGE("Json file set thumbnail resolution fail,thumbWidth:%d thumbHeight:%d, width and height should be [96, 255)\n",
                    m_s32ThumbnailWidth, m_s32ThumbnailHeight);
                m_s32ThumbnailWidth = 128;
                m_s32ThumbnailHeight = 96;
            }
        }
    }
    else
    {
        m_s32ThumbnailWidth = 128;
        m_s32ThumbnailHeight = 96;
    }

    m_stThumbResolution.w = m_s32ThumbnailWidth;
    m_stThumbResolution.h = m_s32ThumbnailHeight;

}

