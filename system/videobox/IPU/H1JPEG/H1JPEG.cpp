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
#include "H1JPEG.h"
#include <sys/time.h>

#define CALYUVSIZE(w,h) ((((w) + 15)&(~0x0f)) * (((h) + 3)&(~0x03)))
#define DUMP 0
#define APP1_XMP_ID "http://ns.adobe.com/xap/1.0/"
// #define DEBUG
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

DYNAMIC_IPU(IPU_H1JPEG, "h1jpeg");

static int switchFormat(Pixel::Format srcFormat)
{
    if(srcFormat == Pixel::Format::RGBA8888)
        return JPEGENC_BGR888;// encode need set BGR888 other than RGB888
    else if(srcFormat == Pixel::Format::NV12)
        return JPEGENC_YUV420_SEMIPLANAR;
    else if(srcFormat == Pixel::Format::NV21)
        return JPEGENC_YUV420_SEMIPLANAR_VU;
    else if(srcFormat == Pixel::Format::RGB565)
        return JPEGENC_RGB565;
    else if(srcFormat == Pixel::Format::YUYV422)
           return JPEGENC_YUV422_INTERLEAVED_YUYV;
     else
        throw VBERR_BADPARAM;
}

void IPU_H1JPEG::WorkLoop()
{
    Buffer bfin, bfJpout;
    Port *pin, *pJpout;
    int skip = 0;
    u32 idx = 0, u32StrLen = 0;
    u8 *u8XmpData = NULL;
    #if TIMEUSAGE
    float timeUsage, timeScale, timeCopy,timeEncode;
    struct timeval tmFrameStart_tmp, tmFrameEnd_tmp;
    #endif
    Buffer bfJpIn;

    pin = GetPort("in");
    pJpout = GetPort("out");

    if ((enJencRet = JpegEncInitSyn(&this->pJpegEncCfg, &this->pJpegEncInst)) != JPEGENC_OK)
    {
        LOGE("failed to initialize the h1 encoder. Error code: %i\n", enJencRet);
        throw VBERR_BADPARAM;
    }

    while (IsInState(ST::Running))
    {
        try {
            if (Mode == IPU_H1JPEG::Trigger)
            {
                if (!GetTrigger())
                {
                    throw "out";
                }
                else
                {
                    bfin = pin->GetBuffer();
                    if (bfin.fr_buf.serial <= SnapSerial)
                    {
                        skip ++;
                        if (skip % 40 == 0)
                        LOGE("no new frames, %d, %d\n", bfin.fr_buf.serial, SnapSerial);
                        pin->PutBuffer(&bfin);
                        throw "out";
                    }
                    SnapSerial = bfin.fr_buf.serial;
                    LOGD("got frame: %d\n", SnapSerial);
                }
            }
            else
            {
                bfin = pin->GetBuffer(&bfin);
            }
        }
        catch(const char* err)
        {
            usleep(20000);
            this->m_stVencInfo.enState = IDLE;
            continue;
        }

        m_stVencInfo.u64InCount++;
        this->m_stVencInfo.enState = RUN;
        pthread_mutex_lock(&updateMut);
        if (updateSetting() < 0)
        {
            pthread_mutex_unlock(&updateMut);
            pin->PutBuffer(&bfin);
            LOGE("update config args fail\n");
            continue;
        }
        pthread_mutex_unlock(&updateMut);

        if (thumbnailEnable)
        {
            enJencRet = EncThumnail(&bfin);
            if (enJencRet != JPEGENC_OK)
            {
                LOGE("Encode thumbnail failed!\n");
            }
        }

        if (encFrameMode == JPEGENC_SLICED_FRAME && hasBackupBuf)
        {
            AllocBackupBuf();
            CopyToBackupBuf(&bfin, &bfJpIn);
        }
        else
        {
            bfJpIn.fr_buf = bfin.fr_buf;
        }

        bfJpout = pJpout->GetBuffer();

        if ((enJencRet = JpegEncSetPictureSize(pJpegEncInst, &pJpegEncCfg)) != JPEGENC_OK)
        {
            CheckEncCfg(&pJpegEncCfg);
            LOGE("error:failed to JpegEncSetPictureSize. Error code: %i\n", enJencRet);
            m_stVencInfo.u32FrameDrop++;
            goto EXIT;
        }

        if (qualityFlag)
        {
            if (JpegEncSetQuality(pJpegEncInst, &pJpegEncCfg) != JPEGENC_OK)
            {
                LOGE("warning:failed to JpegEncSetQuality. Error code: %i\n", enJencRet);
            }
            else
            {
                LOG_DBG("JpegEncSetQuality set quality to %d \n", pJpegEncCfg.qLevel);
            }
        }

        #if TIMEUSAGE
        gettimeofday(&tmFrameStart_tmp, NULL);
        #endif
        pthread_mutex_lock(&updateMut);
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
                            sprintf((char *)u8XmpData,"%d", pJpegEncCfg.codingHeight);
                        else
                            sprintf((char *)u8XmpData,"%d", pJpegEncCfg.codingWidth);
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
        pthread_mutex_unlock(&updateMut);

        if (encFrameMode == JPEGENC_SLICED_FRAME)
        {
            enJencRet = (JpegEncRet)StartJpegSliceEncode(&bfJpIn, &bfJpout);
        }
        else
        {
            enJencRet = (JpegEncRet)StartJpegEncode(&bfJpIn, &bfJpout);
        }

        if (enJencRet != JPEGENC_OK)
        {
            m_stVencInfo.u32FrameDrop++;
        }

        if (encFrameMode == JPEGENC_SLICED_FRAME && hasBackupBuf)
        {
            FreeBackupBuf();
        }


        #if TIMEUSAGE
        gettimeofday(&tmFrameEnd_tmp, NULL);
        timeUsage = (float)(tmFrameEnd_tmp.tv_sec - tmFrameStart_tmp.tv_sec) + (tmFrameEnd_tmp.tv_usec - tmFrameStart_tmp.tv_usec)/(1000000.0);
        timeScale = (float)(tmInterpolation.tv_sec + tmInterpolation.tv_usec/(1000000.0));
        timeCopy = (float)(tmCopy.tv_sec + tmCopy.tv_usec/(1000000.0));
        timeEncode = (float)(tmEncoded.tv_sec + tmEncoded.tv_usec/(1000000.0));

        LOGE("time usage percent:\n");
        LOGE("                   timeUsage:%f timeScale:%f timeEncode:%f timeCopy:%f other:%f\n",timeUsage, timeScale, timeCopy, timeEncode,timeUsage - timeScale - timeEncode - timeCopy);
        LOGE("                                timeScale:%f timeEncode:%f timeCopy:%f other:%f\n",(timeScale/timeUsage)*100, (timeEncode/timeUsage)*100,(timeCopy/timeUsage)*100,  ((timeUsage - timeScale - timeEncode)/timeUsage)*100);

        LOGE("encode one frame time usage:%f tmFrameEnd.tv_sec - tmFrameStart.tv_sec:%ld\n",timeUsage,tmFrameEnd.tv_sec - tmFrameStart.tv_sec);
        #endif

EXIT:
        pJpout->PutBuffer(&bfJpout);
        m_stVencInfo.u64OutCount++;
        pin->PutBuffer(&bfin);
        qualityFlag = false;

        if (Mode == IPU_H1JPEG::Trigger)
        {
            ClearTrigger();
        }
    }
    this->m_stVencInfo.enState = STOP;
    m_stVencInfo.u64InCount = 0;
    m_stVencInfo.u64OutCount = 0;
    SnapSerial = -1;
}

IPU_H1JPEG::IPU_H1JPEG(std::string name, Json *js)
{
    this->Name = name;
    Port *pjp_out, *pjp_in;
    char pRotate[8];
    Rotate = JPEGENC_ROTATE_0;
    Mode = IPU_H1JPEG::Trigger;
    int bufferSize = 0x200000;
    u32 idx = 0;
    const char *ps8ThumbnailMode = NULL;

    this->hasBackupBuf = 0;
    if (NULL != js)
    {
        if (strncmp(js->GetString("mode"), "trigger", 8) == 0)
        {
            Mode = IPU_H1JPEG::Trigger;
        }
        else
        {
            Mode = IPU_H1JPEG::Auto;
        }
        if (NULL != js->GetString("rotate"))
        {
            strncpy(pRotate, js->GetString("rotate"), 8);
            if (NULL != pRotate)
            {
                if (!strncmp(pRotate, "90R", 3))
                {
                    Rotate = JPEGENC_ROTATE_90R;
                }
                else if (!strncmp(pRotate, "90L", 3))
                {
                    Rotate = JPEGENC_ROTATE_90L;
                }
                else
                {
                    LOGE("Rotate Param Error\n");
                    throw VBERR_BADPARAM;
                }
            }
        }
        if (0 != js->GetInt("buffer_size"))
        {
            bufferSize = js->GetInt("buffer_size");
        }
        if (js->GetObject("use_backup_buf"))
        {
            this->hasBackupBuf = js->GetInt("use_backup_buf");
        }
        else
        {
            this->hasBackupBuf = js->GetInt("usebackupbuf");
        }
        thumbnailWidth = 128;
        thumbnailHeight = 96;
        thumbnailMode = 0;
        thumbnailEnable = false;
        if (NULL != js->GetObject("thumbnail_mode"))
        {
            ps8ThumbnailMode = js->GetString("thumbnail_mode");
        }
        else
        {
            ps8ThumbnailMode = js->GetString("thumbnailMode");
        }
        if (NULL != ps8ThumbnailMode)
        {
            #ifdef COMPILE_OPT_JPEG_SWSCALE
            if (strncmp(ps8ThumbnailMode, "intra", 8) == 0)
            {
                LOG_DBG("set thumbnailMode:intra\n");
                thumbnailMode = INTRA;
            }
            else if (strncmp(ps8ThumbnailMode, "extra", 8) == 0)
            {
                LOG_DBG("set thumbnailMode:extra\n");
                thumbnailMode = EXTRA;
            }

            if (thumbnailMode != 0)
            {
                thumbnailEnable = true;
                thumbnailWidth = js->GetInt("thumbnail_width");
                thumbnailHeight = js->GetInt("thumbnail_height");
                if (thumbnailWidth <= 0 || thumbnailHeight <= 0)
                {
                    thumbnailWidth = js->GetInt("thumbnailWidth");
                    thumbnailHeight = js->GetInt("thumbnailHeight");
                }

                if (thumbnailWidth > 0 && thumbnailHeight > 0)
                {
                    thumbnailWidth = (thumbnailWidth < 96)?96:thumbnailWidth;
                    thumbnailHeight = (thumbnailHeight < 32)?32:thumbnailHeight;
                    thumbnailWidth = (thumbnailWidth%16)?((thumbnailWidth + 15)&(~0x0f)):thumbnailWidth;
                    thumbnailHeight = (thumbnailHeight%2)?((thumbnailHeight + 1)&(~0x01)):thumbnailHeight;
                    if (thumbnailMode == INTRA)
                    {
                        if (thumbnailWidth > 255 || thumbnailHeight > 255)
                        {
                            LOGE("Json file set thumbnail resolution fail,thumbWidth:%d thumbHeight:%d, width and height should be [96, 255)\n",
                                thumbnailWidth, thumbnailHeight);
                            thumbnailWidth = 128;
                            thumbnailHeight = 96;
                        }
                    }
                }
                else
                {
                    thumbnailWidth = 128;
                    thumbnailHeight = 96;
                }
            }
            #else
            LOGE("Warning:Jpeg thumbnail need scale encoding support,please open h1jpeg option macro!\n");
            #endif
        }
        LOG_DBG("thumbnailMode:%d thumbnailWidth:%d thumbnailHeight:%d\n",thumbnailMode, thumbnailWidth, thumbnailHeight);
    }

    pjp_in = CreatePort("in", Port::In);
    pjp_out = CreatePort("out", Port::Out);
    if (Mode == IPU_H1JPEG::Trigger)
    {
        pjp_out->SetPixelFormat(Pixel::Format::NV12);
        pjp_out->SetResolution(100, 100);//really realloc SetSnapParams funciton
        pjp_out->SetBufferType(FRBuffer::Type::FIXED, 1);
    }
    else
    {
        pjp_out->SetBufferType(FRBuffer::Type::FLOAT, bufferSize, bufferSize / 4 - 0x4000);
        pjp_out->SetPixelFormat(Pixel::Format::JPEG);
    }

    memset(&pJpegEncCfg, 0, sizeof(JpegEncCfg));

    pjp_out->Export();
    pjp_in->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
    SnapSerial = -1;
    memset(&backupBuf, 0, sizeof(backupBuf));
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

void IPU_H1JPEG::Prepare()
{
    ParamCheck();
    this->pJpegEncCfg.frameType = (JpegEncFrameType)switchFormat(GetPort("in")->GetPixelFormat());

    /* default values middle */
    this->pJpegEncCfg.qLevel = 6;
    this->pJpegEncCfg.markerType = JPEGENC_SINGLE_MARKER;
    this->pJpegEncCfg.unitsType = JPEGENC_NO_UNITS;
    this->pJpegEncCfg.xDensity = 72;
    this->pJpegEncCfg.yDensity = 72;
    this->scaledSliceBuffer.virt_addr = NULL;
    this->xOffset = 0;
    this->yOffset = 0;
    this->cropWidth = 0;
    this->cropHeight = 0;
    this->frameSerial = 0;
    this->encodedSize = 0;
    this->encodedPicBuffer = NULL;
    this->qualityFlag = false;
    this->Triggerred = 0;
    this->configData.flag = UPDATE_NONE;
    this->outBufferRes = 0;
    this->thumbCropBuffer = NULL;
    this->updateMut = PTHREAD_MUTEX_INITIALIZER;
    this->m_s32PrePost = ACTION_NONE;
    this->m_ps8MainCropBuffer = NULL;
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
    this->s32ThumbEncSize = 0;
    this->enJencRet = JPEGENC_OK;
    snprintf(this->m_stVencInfo.ps8Channel, CHANNEL_LENGTH, "%s-%s", Name.c_str(), "out");

    SetJpegEncDefaultCfgParams();
}

void IPU_H1JPEG::Unprepare()
{
    JpegEncRet JpRet;
    Port *pIpuPort = NULL;

    if ((JpRet = JpegEncReleaseSyn(pJpegEncInst)) != JPEGENC_OK)
    {
        LOGE("failed to JpegEncRelease. Error code: %i\n", JpRet);
    }

    if (scaledSliceBuffer.virt_addr)
    {
        fr_free_single(scaledSliceBuffer.virt_addr, scaledSliceBuffer.phys_addr);
        scaledSliceBuffer.virt_addr = NULL;
    }

    if (thumbnailInputBuffer.virt_addr)
    {
        fr_free_single(thumbnailInputBuffer.virt_addr, thumbnailInputBuffer.phys_addr);
        thumbnailInputBuffer.virt_addr = NULL;
    }

    if (thumbnailEncodedBuffer.virt_addr)
    {
        fr_free_single(thumbnailEncodedBuffer.virt_addr, thumbnailEncodedBuffer.phys_addr);
        thumbnailEncodedBuffer.virt_addr = NULL;
    }

    if(encodedPicBuffer)
    {
        free(encodedPicBuffer);
        encodedPicBuffer = NULL;
    }

    if(thumbCropBuffer)
    {
        free(thumbCropBuffer);
        thumbCropBuffer=NULL;
    }

    if (m_ps8MainCropBuffer)
    {
        free(m_ps8MainCropBuffer);
        m_ps8MainCropBuffer = NULL;
    }
    if(pJpegEncIn.xmpData)
    {
        free(pJpegEncIn.xmpData);
        pJpegEncIn.xmpData = NULL;
    }
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

}

void IPU_H1JPEG::ParamCheck()
{
    Port *pIn = GetPort("in");
    Pixel::Format enPixelFormat = pIn->GetPixelFormat();

    if (enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888 && enPixelFormat != Pixel::Format::RGB565
        && enPixelFormat != Pixel::Format::YUYV422)
    {
        LOGE("in Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = GetPort("out")->GetPixelFormat();
    if ((enPixelFormat != Pixel::Format::JPEG && Mode != IPU_H1JPEG::Trigger)
        ||(enPixelFormat != Pixel::Format::NV12 && Mode == IPU_H1JPEG::Trigger))
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    LOGD("(pIn->GetPipX() = %d, pIn->GetPipY() = %d, pIn->GetPipWidth()=%d, \
        pIn->GetPipHeight() =%d, pIn->GetWidth() = %d, pIn->GetHeight() = %d\n", \
        pIn->GetPipX(), pIn->GetPipY() ,  pIn->GetPipWidth(), \
        pIn->GetPipHeight(), pIn->GetWidth(), pIn->GetHeight() );
    if (pIn->GetWidth()%16 != 0 || pIn->GetHeight()%2 != 0 ||
        pIn->GetWidth() < 96 || pIn->GetHeight() < 32 ||
        pIn->GetWidth() > 8176 || pIn->GetHeight() > 8176)
    {
        LOGE("H1Jpeg param error0\n");
        throw VBERR_BADPARAM;
    }
    if ((pIn->GetPipX() < 0 || pIn->GetPipY() < 0) ||
        (pIn->GetPipWidth() < 0 || pIn->GetPipHeight() < 0) ||
        (pIn->GetPipWidth()%16 != 0 || pIn->GetPipHeight()%2 != 0) ||
        (pIn->GetPipX() + pIn->GetPipWidth() > pIn->GetWidth()) ||
        (pIn->GetPipY() + pIn->GetPipHeight() > pIn->GetHeight()))
    {
        LOGE("H1Jpeg param error1\n");
        throw VBERR_BADPARAM;
    }
}

int IPU_H1JPEG::SetQuality(int Qlevel)
{
    if (Qlevel < 0 ||  Qlevel > 9)
    {
        return -1;
    }
    else
    {
        this->pJpegEncCfg.qLevel = Qlevel;
        qualityFlag = true;
    }
    return 0;
}

void IPU_H1JPEG::SetTrigger()
{
    Triggerred = 1;
}
int IPU_H1JPEG::GetTrigger()
{
    return Triggerred;
}
void IPU_H1JPEG::ClearTrigger()
{
    Triggerred = 0;
}

void IPU_H1JPEG::TriggerSnap()
{
    LOGE("set trigger\n");
    for(SetTrigger(); GetTrigger(); usleep(1000));
    LOGE("out of trigger\n");
}

int IPU_H1JPEG::GetThumbnail()
{
    Port *pJOut = NULL;
    Buffer bfJpout;

    if (s32ThumbEncSize <= 0)
    {
        LOGE("error:need trigger photo first!\n");
        return VBEFAILED;
    }
    if (thumbnailEncodedBuffer.virt_addr == NULL)
    {
        LOGE("error:get thumbnail fail!\n");
        return VBEFAILED;
    }
    pJOut = GetPort("out");
    bfJpout = pJOut->GetBuffer();
    memcpy((char *)bfJpout.fr_buf.virt_addr, (char *)thumbnailEncodedBuffer.virt_addr, s32ThumbEncSize);
    bfJpout.fr_buf.size = s32ThumbEncSize;

    pJOut->PutBuffer(&bfJpout);
    s32ThumbEncSize = 0;
    return VBOK;
}

int IPU_H1JPEG::SetSnapParams(struct v_pip_info * arg)
{

    Port *pin = NULL;
    Port *pout = NULL;
    int oriw = 0, orih = 0;
    int realloc_flag = 0;

    if (arg)
    {
        LOGE("IPU_H1JPEG::UnitControl video_set_snap!arg:pic_w:%d pic_h:%d xOffset:%d yOffset:%d cropW:%d cropH:%d\n",
            arg->pic_w, arg->pic_h, arg->x, arg->y, arg->w, arg->h);
        if (arg->w <= 0 || arg->h <= 0 || arg->pic_w <= 0 || arg->pic_h <= 0)
        {
            return -1;
        }
    }

    pin = this->GetPort("in");
    pout = this->GetPort("out");
    oriw = pin->GetWidth();
    orih = pin->GetHeight();
    xOffset = pin->GetPipX();
    yOffset = pin->GetPipY();
    cropWidth = pin->GetPipWidth();
    cropHeight = pin->GetPipHeight();

    m_s32PrePost = ACTION_NONE;
    if (arg == NULL)
    {
        this->encFrameMode = JPEGENC_WHOLE_FRAME;
        realloc_flag = 1;
        if (cropWidth <= 0)
            cropWidth = oriw;
        if(cropHeight <= 0)
            cropHeight = orih;
        LOGE("Jpeg default encode type-->JPEGENC_WHOLE_FRAME\n");
        this->m_stVencInfo.s32Width = oriw;
        this->m_stVencInfo.s32Height = orih;
    }
    else if (arg->w == arg->pic_w && arg->h == arg->pic_h)
    {
        this->encFrameMode = JPEGENC_WHOLE_FRAME;
        LOGE("Jpeg set encode type-->JPEGENC_WHOLE_FRAME\n");

        xOffset = arg->x;
        yOffset = arg->y;
        cropWidth = arg->pic_w;
        cropHeight = arg->pic_h;

        if (arg->w < oriw || arg->h < orih)
        {
            m_s32PrePost |= HARDWRE_CROP;
        }
        this->m_stVencInfo.s32Width = arg->w;
        this->m_stVencInfo.s32Height = arg->h;
    }
    else
    {
        #ifdef COMPILE_OPT_JPEG_SWSCALE
        this->encFrameMode = JPEGENC_SLICED_FRAME;
        m_s32PrePost |= SOFTWARE_SCALE;
        if (arg->w < oriw || arg->h < orih)
        {
            m_s32PrePost |= SOFTWARE_CROP;
        }

        swScaledWidth = arg->pic_w;
        swScaledHeight = arg->pic_h;
        xOffset = arg->x;
        yOffset = arg->y;
        cropWidth = arg->w;
        cropHeight = arg->h;

        this->m_stVencInfo.s32Width = arg->w;
        this->m_stVencInfo.s32Height = arg->h;
        LOGE("Jpeg set encode type-->JPEGENC_SLICED_FRAME\n");
        #else
        LOGE("Jpeg scale encoding not support,please open h1jpeg option macro!\n");
        return -1;
        #endif
    }

    if (arg)
    {
        (arg->pic_w*arg->pic_h > outBufferRes)?(realloc_flag = 1):(realloc_flag = 0);
    }

    if (encFrameMode == JPEGENC_SLICED_FRAME)
    {
        this->pJpegEncCfg.inputWidth = swScaledWidth;
        this->pJpegEncCfg.inputHeight = swScaledHeight;
        this->pJpegEncCfg.xOffset = 0;
        this->pJpegEncCfg.yOffset = 0;
        this->pJpegEncCfg.codingWidth = swScaledWidth;
        this->pJpegEncCfg.codingHeight = swScaledHeight;

        if (swScaledHeight%cropHeight == 0)
        {
            sliceCount = swScaledHeight/cropHeight;
            sliceCount = sliceCount < 6?6:sliceCount;
            LOG_DBG("Jpeg sliceCount:%d\n",sliceCount);
        }
        else
        {
            sliceCount = swScaledHeight/cropHeight + 1;
            LOG_DBG("Jpeg sliceCount:%d\n",sliceCount);
        }

        if (scaledSliceBuffer.virt_addr)
        {
            fr_free_single(scaledSliceBuffer.virt_addr, scaledSliceBuffer.phys_addr);
            scaledSliceBuffer.virt_addr = NULL;
        }

        this->pJpegEncCfg.restartInterval = swScaledHeight/(16*sliceCount);
        this->pJpegEncCfg.codingType = JPEGENC_SLICED_FRAME;

        scaledSliceYUVSize = swScaledWidth*swScaledHeight*3/(2*sliceCount);
        scaledSliceHeight = this->pJpegEncCfg.restartInterval << 4;
        scaledlumSliceRowsSize = scaledSliceHeight*swScaledWidth;
        heightRatio = (float)cropHeight/swScaledHeight;
        srcSliceHeight = (int)(scaledSliceHeight*heightRatio);

        if (srcSliceHeight%2 != 0)
        {
            srcSliceHeight += 1;
        }

        srclumSliceRowsSize = cropWidth*srcSliceHeight;
        srcSliceYUVSize = srclumSliceRowsSize*3/2;
        inputPicSize = cropWidth*cropHeight;

    }
    else
    {
        this->pJpegEncCfg.rotation = (JpegEncPictureRotation)Rotate;

        this->pJpegEncCfg.inputWidth = oriw;
        this->pJpegEncCfg.inputHeight = orih;
        if (this->pJpegEncCfg.rotation != 0)// spec limitation, slice encode not support rotation
        {
            this->pJpegEncCfg.codingWidth = cropHeight;
            this->pJpegEncCfg.codingHeight = cropWidth;
        }
        else
        {
            this->pJpegEncCfg.codingWidth = cropWidth;
            this->pJpegEncCfg.codingHeight = cropHeight;
        }
        this->pJpegEncCfg.xOffset = xOffset;
        this->pJpegEncCfg.yOffset = yOffset;

        this->pJpegEncCfg.restartInterval = 5;
        this->pJpegEncCfg.codingType = JPEGENC_WHOLE_FRAME;
    }

    // realloc out buffer
    if (realloc_flag)
    {
        pout->FreeFRBuffer();
        pout->SetResolution(pJpegEncCfg.inputWidth, pJpegEncCfg.inputHeight*3/4);
        pout->AllocateFRBuffer();
        outBufferRes = pJpegEncCfg.inputWidth*pJpegEncCfg.inputHeight;
    }

    if (m_ps8MainCropBuffer)
    {
        free(m_ps8MainCropBuffer);
        m_ps8MainCropBuffer = NULL;
    }

    encodedSize = pJpegEncCfg.codingWidth*pJpegEncCfg.codingHeight*3/4;

    if (scaledSliceBuffer.virt_addr)
    {
        fr_free_single(scaledSliceBuffer.virt_addr, scaledSliceBuffer.phys_addr);
        scaledSliceBuffer.virt_addr = NULL;
    }

    if (encodedPicBuffer)
    {
        free(encodedPicBuffer);
        encodedPicBuffer = NULL;
    }

    return CheckEncCfg(&pJpegEncCfg);
}

int IPU_H1JPEG::SetJpegEncDefaultCfgParams()
{
    return SetSnapParams(NULL);
}

int IPU_H1JPEG:: CheckEncCfg(JpegEncCfg *pJpegEncCfg)
{
    /*check input/coding width and height followed by the spec limitation
      JPEGENC_SLICED_FRAME support 4K(memory limitation), note h1v6 support 8k*/
    u32 width,height;
    if ((pJpegEncCfg->inputWidth & 0xF) != 0)
    {
        width = (pJpegEncCfg->inputWidth + 15)&(~0xF);
        height = (pJpegEncCfg->inputHeight + 1)&(~0x1);
        LOGE("error: Input width must be multiple of 16! suggestion-->(width ,height):(%d, %d)\n",(width > 8176)?8176:width, (height > 8176)?8176:height);
        return -1;
    }

    if ((pJpegEncCfg->inputHeight & 0x1) != 0)
    {
        width = (pJpegEncCfg->inputWidth + 15)&(~0xF);
        height = (pJpegEncCfg->inputHeight + 1)&(~0x1);
        LOGE("error: Input height must be multiple of 2! suggestion-->(width ,height):(%d, %d)\n",(width > 8176)?8176:width, (height > 8176)?8176:height);
        return -1;
    }

    if (pJpegEncCfg->inputWidth < 96 || pJpegEncCfg->inputWidth > 8176 || pJpegEncCfg->inputHeight < 32 || pJpegEncCfg->inputHeight > 8176)
    {
        LOGE("error:Input Width out of range [96, 8176] or Input Height out of range [32, 8176]\n");
        return -1;
    }

    if (pJpegEncCfg->codingWidth & 0x3)
    {
        LOGE("error: Cropping width must be multiple of 4!\n");
        return -1;
    }

    if (pJpegEncCfg->codingHeight & 0x1)
    {
        LOGE("error: Cropping height must be multiple of 2!");
        return -1;
    }

    if (pJpegEncCfg->inputWidth < (pJpegEncCfg->xOffset + pJpegEncCfg->codingWidth) || pJpegEncCfg->inputHeight < (pJpegEncCfg->yOffset + pJpegEncCfg->codingHeight))
    {
        LOGE("error:crop args(xOffset, yOffset, codingWidth, codingHeight)-->(%d, %d, %d, %d) out of input picture area(0,0,%d,%d)\n",pJpegEncCfg->xOffset, pJpegEncCfg->yOffset, pJpegEncCfg->codingWidth, pJpegEncCfg->codingHeight, pJpegEncCfg->inputWidth, pJpegEncCfg->inputHeight);
        return -1;
    }

    return 0;
}

int IPU_H1JPEG::StartJpegEncode(Buffer *InBuf, Buffer *OutBuf)
{
    int PicSize = 0;
    Port *pin;

    pin = this->GetPort("in");
    PicSize = CALYUVSIZE(pin->GetWidth(), pin->GetHeight());

    this->pJpegEncIn.busLum = InBuf->fr_buf.phys_addr;
    this->pJpegEncIn.busCb = this->pJpegEncIn.busLum + PicSize;
    this->pJpegEncIn.busCr = this->pJpegEncIn.busCb + (PicSize >> 2);

    this->pJpegEncIn.pOutBuf = (u8 *)OutBuf->fr_buf.virt_addr;
    this->pJpegEncIn.busOutBuf = OutBuf->fr_buf.phys_addr;
    this->pJpegEncIn.outBufSize = OutBuf->fr_buf.map_size;
    this->pJpegEncIn.frameHeader = 1;

    do
    {
        enJencRet = JpegEncEncodeSyn(this->pJpegEncInst, &this->pJpegEncIn, &this->pJpegEncOut);
        if (enJencRet < JPEGENC_OK)
        {
            LOGE("failed to h1 JpegEncode. Error code: %i\n", enJencRet);
            return (int)enJencRet;
        }
    } while (enJencRet != JPEGENC_FRAME_READY);

    OutBuf->fr_buf.size = this->pJpegEncOut.jfifSize;
    return (int)JPEGENC_OK;
}

int IPU_H1JPEG::StartJpegSliceEncode(Buffer *InBuf, Buffer *OutBuf)
{
#ifdef COMPILE_OPT_JPEG_SWSCALE
    Port *pin = NULL;
    int s32PicBytes = 0;
    int s32Slice = 0;
    int s32InWidth = 0, s32InHeight = 0;
    bool bLast = false;
    char *ps8InputData = NULL;
    int s32LumOffset = 0;
    int s32CbOffset = 0;
    int s32AccumSrcHeight = 0;
    int s32AccumScaledHeight = 0;

    pin = GetPort("in");
    s32InWidth = pin->GetWidth();
    s32InHeight = pin->GetHeight();
    srclumSliceRowsSize = cropWidth*srcSliceHeight;

    if (scaledSliceBuffer.virt_addr == NULL)
    {
        if (scaledSliceYUVSize > srcSliceYUVSize)
        {
            scaledSliceBuffer.virt_addr = fr_alloc_single("scaledSliceBuf", scaledSliceYUVSize, &scaledSliceBuffer.phys_addr);
        }
        else
        {
            scaledSliceBuffer.virt_addr = fr_alloc_single("scaledSliceBuf", srcSliceYUVSize, &scaledSliceBuffer.phys_addr);
        }
        if (scaledSliceBuffer.virt_addr == NULL || scaledSliceBuffer.virt_addr == (void *)-1)
        {
            LOGE("Error:fr_alloc_single failed!\n");
            return JPEGENC_MEMORY_ERROR;
        }
    }

    if (encodedPicBuffer == NULL)
    {
        encodedPicBuffer = (char *)malloc(encodedSize);
        if (encodedPicBuffer == NULL)
        {
            LOGE("Error:malloc buffer fail!\n");
            return JPEGENC_MEMORY_ERROR;
        }
    }

    ps8InputData = (char *)InBuf->fr_buf.virt_addr;

    if (m_s32PrePost & SOFTWARE_CROP)
    {
        if (m_ps8MainCropBuffer == NULL)
        {
            m_ps8MainCropBuffer = (char *)malloc(cropWidth*cropHeight*3<<1);
        }
        if (m_ps8MainCropBuffer == NULL)
        {
            LOGE("Error:malloc buffer fail!\n");
            return JPEGENC_MEMORY_ERROR;
        }
        doCrop((char *)InBuf->fr_buf.virt_addr, s32InWidth, s32InHeight, 0, m_ps8MainCropBuffer, xOffset, yOffset, cropWidth, cropHeight, 0);
        ps8InputData = m_ps8MainCropBuffer;
    }

    pJpegEncIn.busLum = scaledSliceBuffer.phys_addr;
    pJpegEncIn.busCb = pJpegEncIn.busLum + scaledlumSliceRowsSize;
    // pJpegEncIn.busCr = pJpegEncIn.busCb +  lumSiceRowsSize/4; //using for YUV420 PLANAR

    pJpegEncIn.pOutBuf = (u8 *)OutBuf->fr_buf.virt_addr;
    pJpegEncIn.busOutBuf = OutBuf->fr_buf.phys_addr;
    pJpegEncIn.outBufSize = OutBuf->fr_buf.map_size;
    this->pJpegEncIn.frameHeader = 1;

    #if TIMEUSAGE
    tmInterpolation.tv_sec = 0;
    tmInterpolation.tv_usec = 0;
    tmEncoded.tv_sec = 0;
    tmEncoded.tv_usec = 0;
    tmCopy.tv_sec = 0;
    tmCopy.tv_usec = 0;
    #endif

    #if DUMP
    WriteFile(ps8InputData, cropWidth*cropHeight*3/2,"/mnt/Crop%d_%d_%d.yuv",s32Slice, cropWidth, cropHeight);
    #endif

    do
    {
        s32LumOffset = srclumSliceRowsSize*s32Slice;
        s32CbOffset = inputPicSize + (srclumSliceRowsSize >> 1)*s32Slice;

        #if TIMEUSAGE
        gettimeofday(&tmFrameStart, NULL);
        #endif

        // input lum slice
        if (s32AccumSrcHeight + srcSliceHeight >= cropHeight)
        {
            srclumSliceRowsSize = cropWidth*(cropHeight - s32AccumSrcHeight);
            LOG_DBG("final lum input!slice:%d w:%d h:%d lumOffset:%d srclumSliceRowsSize:%d\n",s32Slice, cropWidth, (cropHeight - s32AccumSrcHeight), s32LumOffset, srclumSliceRowsSize);
            memcpy((char *)scaledSliceBuffer.virt_addr, ps8InputData + s32LumOffset, srclumSliceRowsSize);
        }
        else
        {
            LOG_DBG("normal lum input!slice:%d w:%d h:%d lumOffset:%d srclumSliceRowsSize:%d\n",s32Slice, cropWidth, srcSliceHeight, s32LumOffset, srclumSliceRowsSize);
            memcpy((char *)scaledSliceBuffer.virt_addr, ps8InputData + s32LumOffset, srclumSliceRowsSize);
        }

        // input cb&cr slice
        if (s32AccumSrcHeight + srcSliceHeight >= cropHeight)
        {
            LOG_DBG("final cb input!slice:%d w:%d h:%d cbOffset:%d srclumSliceRowsSize/2:%d\n",s32Slice, cropWidth, (cropHeight - s32AccumSrcHeight), s32CbOffset, srclumSliceRowsSize/2);
            memcpy((char *)scaledSliceBuffer.virt_addr + srclumSliceRowsSize, ps8InputData + s32CbOffset , cropWidth*(cropHeight - s32AccumSrcHeight) >> 1);
            bLast = true;

            #if DUMP
            WriteFile((char *)scaledSliceBuffer.virt_addr, cropWidth*(cropHeight - s32AccumSrcHeight)*3/2,"/mnt/srcPic%d_%d_%d.yuv",s32Slice, cropWidth, cropHeight - s32AccumSrcHeight);
            #endif
        }
        else
        {
            LOG_DBG("normal copy srclumSliceRowsSize:%d cbOffset:%d size:%d\n",srclumSliceRowsSize, s32CbOffset, (srclumSliceRowsSize >> 1));
            memcpy((char *)scaledSliceBuffer.virt_addr + srclumSliceRowsSize, ps8InputData + s32CbOffset , (srclumSliceRowsSize >> 1));
            bLast = false;

            #if DUMP
            WriteFile((char *)scaledSliceBuffer.virt_addr, cropWidth*srcSliceHeight*3/2,"/mnt/srcPic%d_%d_%d.yuv",s32Slice, cropWidth, srcSliceHeight);
            #endif
        }

        #if TIMEUSAGE
        gettimeofday(&tmFrameEnd, NULL);
        tmCopy.tv_sec += tmFrameEnd.tv_sec - tmFrameStart.tv_sec;
        tmCopy.tv_usec += tmFrameEnd.tv_usec - tmFrameStart.tv_usec;
        #endif

        #if TIMEUSAGE
        gettimeofday(&tmFrameStart, NULL);
        #endif

        if (bLast)
        {
            doSliceScale((char *)scaledSliceBuffer.virt_addr, cropWidth, cropHeight - s32AccumSrcHeight, 0, (char *)scaledSliceBuffer.virt_addr, swScaledWidth, swScaledHeight - s32AccumScaledHeight, 0, NULL);
            pJpegEncIn.busLum = scaledSliceBuffer.phys_addr;
            pJpegEncIn.busCb = pJpegEncIn.busLum + swScaledWidth*(swScaledHeight - s32AccumScaledHeight);
            s32AccumSrcHeight = cropHeight;
            #if DUMP
            WriteFile((char *)scaledSliceBuffer.virt_addr, swScaledWidth*(swScaledHeight - s32AccumScaledHeight)*3/2,"/mnt/scaledPic%d_%d_%d.yuv",s32Slice, swScaledWidth, swScaledHeight - s32AccumScaledHeight);
            #endif
            s32AccumScaledHeight = swScaledHeight;
        }
        else
        {
            doSliceScale((char *)scaledSliceBuffer.virt_addr, cropWidth, srcSliceHeight, 0, (char *)scaledSliceBuffer.virt_addr, swScaledWidth, scaledSliceHeight, 0, NULL);
            pJpegEncIn.busLum = scaledSliceBuffer.phys_addr;
            pJpegEncIn.busCb = pJpegEncIn.busLum + swScaledWidth*scaledSliceHeight;
            s32AccumSrcHeight += srcSliceHeight;
            s32AccumScaledHeight += scaledSliceHeight;
            #if DUMP
            WriteFile((char *)scaledSliceBuffer.virt_addr, swScaledWidth*scaledSliceHeight*3/2,"/mnt/scaledPic%d_%d_%d.yuv",s32Slice, swScaledWidth, scaledSliceHeight);
            #endif
        }

        #if TIMEUSAGE
        gettimeofday(&tmFrameEnd, NULL);
        tmInterpolation.tv_sec += tmFrameEnd.tv_sec - tmFrameStart.tv_sec;
        tmInterpolation.tv_usec += tmFrameEnd.tv_usec - tmFrameStart.tv_usec;
        #endif

        #if TIMEUSAGE
        gettimeofday(&tmFrameStart, NULL);
        #endif
        enJencRet = JpegEncEncodeSyn(pJpegEncInst, &pJpegEncIn, &pJpegEncOut);

        switch (enJencRet)
        {
            case JPEGENC_RESTART_INTERVAL:
                s32Slice++;
                memcpy(encodedPicBuffer + s32PicBytes, pJpegEncIn.pOutBuf, pJpegEncOut.jfifSize);
                s32PicBytes += pJpegEncOut.jfifSize;
                break;
            case JPEGENC_FRAME_READY:
                memcpy(encodedPicBuffer + s32PicBytes, pJpegEncIn.pOutBuf, pJpegEncOut.jfifSize);
                s32PicBytes += pJpegEncOut.jfifSize;
                frameSerial++;
                break;
            case JPEGENC_OUTPUT_BUFFER_OVERFLOW:
                LOGE("frameSerial:%d silce:%d pJpegEncOut.jfifSize:%d  overflow!\n",frameSerial, s32Slice, pJpegEncOut.jfifSize);
                frameSerial++;
                break;
            default:
                LOGE("error:frame:%d slice:%d jpRet:%d\n",frameSerial, s32Slice, enJencRet);
                break;
        }
    }while(enJencRet == JPEGENC_RESTART_INTERVAL);

    memcpy((char *)OutBuf->fr_buf.virt_addr, encodedPicBuffer, s32PicBytes);
    #if DUMP
    WriteFile(encodedPicBuffer, s32PicBytes,"/mnt/EncPic%d_%d_%d.jpeg",s32Slice, swScaledWidth, swScaledHeight);
    #endif
    OutBuf->fr_buf.size = s32PicBytes;
    #if TIMEUSAGE
    gettimeofday(&tmFrameEnd, NULL);
    tmEncoded.tv_sec += tmFrameEnd.tv_sec - tmFrameStart.tv_sec;
    tmEncoded.tv_usec += tmFrameEnd.tv_usec - tmFrameStart.tv_usec;
    #endif

    return (int)JPEGENC_OK;
#else
    LOGE("Jpeg scale encoding not support,please open h1jpeg option macro!\n");
    return (int)JPEGENC_ERROR;
#endif
}

int IPU_H1JPEG::AllocBackupBuf()
{
    int ret = 0;
    Port *pin = NULL;
    int size = 0;


    pin = this->GetPort("in");
    size = pin->GetWidth() * pin->GetHeight() * 3 / 2;
    if (NULL == backupBuf.virt_addr)
    {
        backupBuf.virt_addr = fr_alloc_single("JpegBackupBuf", size, &backupBuf.phys_addr);
        if (backupBuf.virt_addr)
        {
            backupBuf.size = size;
            backupBuf.map_size = size;
        }
    }
    if (NULL == backupBuf.virt_addr)
    {
        LOGE("(%s:L%d) WARNING alloc size %d failed!\n", __func__, __LINE__, size);
        ret = -1;
    }

    return ret;
}

int IPU_H1JPEG::FreeBackupBuf()
{
    if (backupBuf.virt_addr)
    {
        fr_free_single(backupBuf.virt_addr, backupBuf.phys_addr);
        backupBuf.virt_addr = NULL;
    }

    return 0;
}

int IPU_H1JPEG::CopyToBackupBuf(Buffer *InBuf, Buffer *OutBuf)
{
    if (backupBuf.virt_addr
        && backupBuf.size == InBuf->fr_buf.size)
    {
        memcpy((char *)backupBuf.virt_addr, InBuf->fr_buf.virt_addr, backupBuf.size);
        OutBuf->fr_buf = backupBuf;
    }
    else
    {
        OutBuf->fr_buf = InBuf->fr_buf;
    }

    return 0;
}

JpegEncRet IPU_H1JPEG::EncThumnail(Buffer *InBuf)
{
    int thumbnail_size = thumbnailWidth*thumbnailHeight;
    #ifdef COMPILE_OPT_JPEG_SWSCALE
    Port *pin;

    pin = GetPort("in");
    #endif
    thumbEncBufSize = ((thumbnail_size*3>>1)*3>>2) + 630;// %75 picdata + 630bytes container
    if (thumbnailInputBuffer.virt_addr == NULL)
    {
        LOGD("alloc thumbnailInputBuffer!\n");
        thumbnailInputBuffer.virt_addr = fr_alloc_single("thumbInBuffer", thumbnail_size*3>>1, &thumbnailInputBuffer.phys_addr);
    }
    if (thumbnailEncodedBuffer.virt_addr == NULL)
    {
        LOGD("alloc thumbnailEncodedBuffer!\n");
        thumbnailEncodedBuffer.virt_addr = fr_alloc_single("thumbEncBuffer", thumbEncBufSize, &thumbnailEncodedBuffer.phys_addr);
    }
    s32ThumbEncSize = 0;
    #ifdef COMPILE_OPT_JPEG_SWSCALE
    int inWidth, inHeight;

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
    inWidth = pin->GetWidth();
    inHeight = pin->GetHeight();
    if(pJpegEncCfg.codingWidth != (u32)inWidth || pJpegEncCfg.codingHeight != (u32)inHeight)
    {
        if (thumbCropBuffer == NULL)
        {
            thumbCropBuffer = (char *)malloc(pJpegEncCfg.codingWidth*pJpegEncCfg.codingHeight*3/2);
            if (thumbCropBuffer == NULL)
            {
                LOGE("Error:can not malloc crop buffer!\n");
                return JPEGENC_MEMORY_ERROR;
            }
        }
        doCrop((char *)InBuf->fr_buf.virt_addr, inWidth, inHeight, 0, thumbCropBuffer, pJpegEncCfg.xOffset, pJpegEncCfg.yOffset, pJpegEncCfg.codingWidth, pJpegEncCfg.codingHeight, 0);
        if (doSliceScale(thumbCropBuffer, pJpegEncCfg.codingWidth, pJpegEncCfg.codingHeight, 0, (char *)thumbnailInputBuffer.virt_addr, thumbnailWidth, thumbnailHeight, 0, NULL) < 0)
        {
            LOGE("Error:scale fails!\n");
            return JPEGENC_ERROR;
        }
    }
    else
    {
        if (doSliceScale((char *)InBuf->fr_buf.virt_addr, inWidth, inHeight, 0, (char *)thumbnailInputBuffer.virt_addr, thumbnailWidth, thumbnailHeight, 0, NULL) < 0)
        {
            LOGE("Error:scale fails!\n");
            return JPEGENC_ERROR;
        }
    }
    #endif
    memcpy(&thumbJpegEncCfg, &pJpegEncCfg, sizeof(JpegEncCfg));
    thumbJpegEncCfg.inputWidth = thumbnailWidth;
    thumbJpegEncCfg.inputHeight = thumbnailHeight;
    thumbJpegEncCfg.codingWidth = thumbnailWidth;
    thumbJpegEncCfg.codingHeight = thumbnailHeight;
    thumbJpegEncCfg.xOffset = 0;
    thumbJpegEncCfg.yOffset = 0;
    thumbJpegEncCfg.codingType = JPEGENC_WHOLE_FRAME;
    thumbJpegEncCfg.restartInterval = 2;

    pJpegEncIn.busLum = thumbnailInputBuffer.phys_addr;
    pJpegEncIn.busCb = this->pJpegEncIn.busLum + thumbnail_size;
    pJpegEncIn.busCr = this->pJpegEncIn.busCb + (thumbnail_size >> 2);
    pJpegEncIn.pOutBuf = (u8 *)thumbnailEncodedBuffer.virt_addr;
    pJpegEncIn.busOutBuf = thumbnailEncodedBuffer.phys_addr;
    pJpegEncIn.outBufSize = thumbEncBufSize;
    pJpegEncIn.frameHeader = 1;
    pJpegEncIn.app1 = 0;

    if ((enJencRet = JpegEncSetPictureSize(pJpegEncInst, &thumbJpegEncCfg)) != JPEGENC_OK)
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

    s32ThumbEncSize = pJpegEncOut.jfifSize;

    if (thumbnailMode == INTRA)
    {
        jpegThumb.data = thumbnailEncodedBuffer.virt_addr;
        jpegThumb.format = JPEGENC_THUMB_JPEG;
        jpegThumb.width = thumbnailWidth;
        jpegThumb.height = thumbnailHeight;
        jpegThumb.dataLength = s32ThumbEncSize;
        if ((enJencRet = JpegEncSetThumbnail(pJpegEncInst, &jpegThumb)) != JPEGENC_OK)
        {
            LOGE("Set Thumbnail Failed! ret:%d\n",enJencRet);
            return enJencRet;
        }
    }

   return JPEGENC_OK;
}

int IPU_H1JPEG::SetPipInfo(struct v_pip_info *vpinfo)
{
    if (strncmp(vpinfo->portname, "out", 3) == 0)
    {
        GetPort("out")->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        return 0;
    }
    else if (strncmp(vpinfo->portname, "in", 2) == 0)
    {
        GetPort("in")->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        return 0;
    }
    else
    {
        LOGE("donot support the port:%s\n",vpinfo->portname);
        return -1;
    }
}

int IPU_H1JPEG::SetThumbnailRes(vbres_t *res) {

    int old_space = 0;
    int new_space = 0;

    old_space = thumbnailWidth*thumbnailHeight;
    thumbnailWidth = (res->w < 96)?96:res->w;
    thumbnailHeight = (res->h < 32)?32:res->h;
    thumbnailWidth = (res->w % 16)?((res->w + 15)&(~0x0f)):res->w;
    thumbnailHeight = (res->h % 2)?((res->h + 1)&(~0x01)):res->h;

    if (thumbnailMode == INTRA)
    {
        if (thumbnailWidth > 255 || thumbnailHeight > 255)
        {
            LOGE("Set thumbnail resolution fail,thumbWidth:%d thumbHeight:%d, width and height should be [96, 255)\n",thumbnailWidth, thumbnailHeight);
            return -1;
        }
    }

    new_space = thumbnailWidth*thumbnailHeight;
    if ( new_space > old_space)
    {
        if (thumbCropBuffer)
        {
            free(thumbCropBuffer);
            thumbCropBuffer=NULL;
        }
        if (thumbnailInputBuffer.virt_addr)
        {
            fr_free_single(thumbnailInputBuffer.virt_addr, thumbnailInputBuffer.phys_addr);
            thumbnailInputBuffer.virt_addr = NULL;
        }
        if (thumbnailEncodedBuffer.virt_addr)
        {
            fr_free_single(thumbnailEncodedBuffer.virt_addr, thumbnailEncodedBuffer.phys_addr);
            thumbnailEncodedBuffer.virt_addr = NULL;
        }
    }
    return 0;
}

int IPU_H1JPEG::updateSetting()
{
    int ret = 0;
    if (configData.flag == UPDATE_NONE)
    {
        return ret;
    }
    if (configData.flag & UPDATE_THUMBRES)
    {
        ret = SetThumbnailRes(&configData.thumbRes);

        if  (ret < 0)
        {
            configData.flag = UPDATE_NONE;
            return ret;
        }
    }
    if (configData.flag & UPDATE_CROPINFO)
    {
        ret = SetSnapParams(&configData.pipInfo);
        if (ret < 0)
        {
            configData.flag = UPDATE_NONE;
            return ret;
        }
    }
    if (configData.flag & UPDATE_QUALITY)
    {
        SetQuality(configData.qLevel);
    }
    configData.flag = UPDATE_NONE;
    return ret;
}

int IPU_H1JPEG::GetDebugInfo(void *pInfo, int *ps32Size)
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


