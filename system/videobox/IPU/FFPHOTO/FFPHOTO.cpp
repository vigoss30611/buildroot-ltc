// Copyright (C) 2016 InfoTM, ranson.ni@infotm.com
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
#include "FFPHOTO.h"

DYNAMIC_IPU(IPU_FFPHOTO, "ffphoto");
#define ALLOCBUFSIZE 0x300000
#define DUMP_YUV_DATA 0
#define TIMEUSAGE 0

#ifdef FFPHOTO_SWSCALE_ENABLE
    #pragma message "photo swscale on"
    #define ENABLE_SCALE 1
#else
    #pragma message "photo swscale off"
#endif

static int read_bs(void *opaque, uint8_t *buf, int buf_size)
{
    struct buffer_data *bd = (struct buffer_data *)opaque;
    buf_size = FFMIN((size_t)buf_size, bd->size);

    /* copy internal buffer data to buf */
    memcpy(buf, bd->ptr, buf_size);
    bd->ptr  += buf_size;
    bd->size -= buf_size;

    return buf_size;
}

int IPU_FFPHOTO::ImageDecode(Buffer* pbfin)
{
    uint8_t *avio_ctx_buffer = NULL;
    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVFrame *pFrame = NULL;
    AVPacket packet;
    AVIOContext *avio = NULL;
    struct buffer_data bd = { 0 };
    size_t  avio_ctx_buffer_size = 4096;
    Buffer bfout;
    Port *pOut = NULL;
    unsigned char *out_buffer = NULL;
    int videoindex = -1, ret = 0, got_picture = 0, total_size=0;
    unsigned int i = 0;
#if ENABLE_SCALE
    int scWidth = 0, scHeight = 0;
    struct SwsContext *img_convert_ctx = NULL;
    AVFrame* pFrameYUV = NULL;
    AVPixelFormat fmt = AV_PIX_FMT_NV12;
#else
    int y_size = 0;
#endif

#if DUMP_YUV_DATA
    static int cnt = 0;
    char scaled[100] = {0};
    char output[100] = {0};
    sprintf(scaled,"/mnt/sd0/DCIM/100CVR/Photo/scaled_output_%d.yuv",cnt);
    sprintf(output,"/mnt/sd0/DCIM/100CVR/Photo/dec_output_%d.yuv", cnt);
    LOGE("scled:%s\n",scaled);
    LOGE("output:%s\n", output);
    FILE *fp_scaled_yuv = fopen(scaled,"wb+");
    FILE *fp_dec_yuv = fopen(output,"wb+");
    cnt++;
#endif

#if TIMEUSAGE
    struct timeval tmStart, tmEnd, tmScale, tmDec;
    struct timeval tmScale_S, tmScale_E, tmDec_S, tmDec_E;
    tmDec.tv_sec = tmDec.tv_usec = 0;
    tmScale.tv_sec = tmScale.tv_usec = 0;
    gettimeofday(&tmStart, NULL);
#endif
    pOut = GetPort("frame");

    pFormatCtx = avformat_alloc_context();
    if(!pFormatCtx)
    {
        ret = AVERROR(ENOMEM);
        return ret;
    }

    /* fill opaque structure used by the AVIOContext read callback */
    bd.ptr  = (uint8_t*)pbfin->fr_buf.virt_addr;
    bd.size = pbfin->fr_buf.size;

    avio_ctx_buffer = (uint8_t*)av_malloc(avio_ctx_buffer_size);
    if(!avio_ctx_buffer) {
        ret = AVERROR(ENOMEM);
        return ret;
    }
    avio = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
                                  0, &bd, &read_bs, NULL, NULL);
    if(!avio) {
        ret = AVERROR(ENOMEM);
        return ret;
    }
    pFormatCtx->pb = avio;

    ret = avformat_open_input(&pFormatCtx, NULL, NULL, NULL);
    if(ret != 0)
    {
        LOGE("Couldn't open input stream,error:%d.\n", ret);
        return -1;
    }

    if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        LOGE("Couldn't find stream information.\n");
        return -1;
    }
    videoindex = -1;
    for(i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoindex = i;
            LOGE("stream id:%d,codectype:%d\n", i,
                pFormatCtx->streams[i]->codec->codec_type);
            break;
        }
    }

    if(videoindex == -1)
    {
        LOGE("Didn't find a video stream.\n");
        return -1;
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec == NULL)
    {
        LOGE("Codec not found.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        LOGE("Could not open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    if(!pFrame)
    {
        ret = AVERROR(ENOMEM);
        return ret;
    }
#if ENABLE_SCALE
    pFrameYUV = av_frame_alloc();
    if(!pFrameYUV)
    {
        ret = AVERROR(ENOMEM);
        return ret;
    }
    //FIXME: need to realloc out port buffer by real picture size.
    scWidth = pCodecCtx->width;
    scHeight = pCodecCtx->height;
#endif
    if((pCodecCtx->width > pOut->GetWidth()) || (pCodecCtx->height > pOut->GetHeight()))
    {
        LOGE("picture size(%d %d) beyond buffer size(%d %d)!\n", pCodecCtx->width, pCodecCtx->height,
            pOut->GetWidth(), pOut->GetHeight());
#if ENABLE_SCALE
        scWidth = pOut->GetWidth();
        scHeight = pOut->GetHeight();
#endif
    }
    bool bGetBuffer = false;
    for (int i = 0; (i < 10) && (!bGetBuffer); i++)
    {
        try{
            bfout = pOut->GetBuffer();
        }catch(const char *err){
            usleep(20*1000);
            LOGE("out port cann't get buffer,retry again!\n");
            continue;
        }
        bGetBuffer = true;
    }

    if (!bGetBuffer)
    {
        LOGE("%s:%d:out port get buffer fail!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    LOGD("out port fmt:%d, but we only support yuv420p output.\n", pOut->GetPixelFormat());
    av_init_packet(&packet);

    out_buffer = (unsigned char *)bfout.fr_buf.virt_addr;
#if ENABLE_SCALE
    if (pOut->GetPixelFormat() == Pixel::Format::NV12)
    {
        fmt = AV_PIX_FMT_NV12;
    }
    else if(pOut->GetPixelFormat() == Pixel::Format::RGBA8888)
    {
        fmt = AV_PIX_FMT_RGBA;
    }

    total_size = av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
        fmt, scWidth, scHeight, 1);


    //SWS_FAST_BILINEAR SWS_BICUBIC
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        scWidth, scHeight, fmt, SWS_FAST_BILINEAR, NULL, NULL, NULL);
#endif

    while(av_read_frame(pFormatCtx, &packet) >= 0)
    {
        if(packet.stream_index == videoindex)
        {
#if TIMEUSAGE
            gettimeofday(&tmDec_S, NULL);
#endif
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
#if TIMEUSAGE
            gettimeofday(&tmDec_E, NULL);
            tmDec.tv_sec =  tmDec_E.tv_sec - tmDec_S.tv_sec;
            tmDec.tv_usec = tmDec_E.tv_usec - tmDec_S.tv_usec;
#endif
            if(ret < 0)
            {
                LOGE("Decode Error.\n");
                return -1;
            }

            if(got_picture)
            {
#if TIMEUSAGE
                gettimeofday(&tmScale_S, NULL);
#endif
#if DUMP_YUV_DATA

                int data_size = pCodecCtx->width * pCodecCtx->height;
                if((fp_dec_yuv) && (pCodecCtx->pix_fmt == AV_PIX_FMT_YUVJ420P))
                {
                    fwrite(pFrame->data[0], 1, data_size, fp_dec_yuv);    //Y
                    fwrite(pFrame->data[1], 1, data_size/4, fp_dec_yuv);  //U
                    fwrite(pFrame->data[2], 1, data_size/4, fp_dec_yuv);  //V
                }
                if (fp_dec_yuv)
                    fclose(fp_dec_yuv);
#endif
#if ENABLE_SCALE
                sws_scale(img_convert_ctx, (const unsigned char* const*)pFrame->data,
                    pFrame->linesize, 0, pCodecCtx->height,
                    pFrameYUV->data, pFrameYUV->linesize);
#else
                y_size = pCodecCtx->width * pCodecCtx->height;
                if ((pCodecCtx->pix_fmt == AV_PIX_FMT_YUVJ420P) ||
                     (pCodecCtx->pix_fmt == AV_PIX_FMT_YUV420P))
                {
                    memcpy(out_buffer, pFrame->data[0], y_size);//Y
                    out_buffer += y_size;
                    memcpy(out_buffer, pFrame->data[1], y_size/4);//U
                    out_buffer += y_size/4;
                    memcpy(out_buffer, pFrame->data[2], y_size/4);//V
                    total_size = y_size * 3 / 2;
                }
                else
                {
                    LOGE("Unsupport frame format:%d\n", pCodecCtx->pix_fmt);
                }
#endif
#if TIMEUSAGE
             gettimeofday(&tmScale_E, NULL);
             tmScale.tv_sec =  tmScale_E.tv_sec - tmScale_S.tv_sec;
             tmScale.tv_usec = tmScale_E.tv_usec - tmScale_S.tv_usec;
#endif

#if DUMP_YUV_DATA
                if(fp_scaled_yuv)
                {
                    if (total_size > 0)
                        fwrite((unsigned char *)bfout.fr_buf.virt_addr, 1, total_size, fp_scaled_yuv);  //UV
                    LOGE("codec width: %d, height: %d, scaled width: %d, scaled height: %d, total size: %d\n",
                        pCodecCtx->width, pCodecCtx->height, scWidth, scHeight, total_size);
                }
#endif
                bfout.fr_buf.size = total_size;
                pOut->PutBuffer(&bfout);
                info.height = pCodecCtx->height;
                info.width  = pCodecCtx->width;
                LOGE("Succeed to decode 1 frame!\n");
            }
        }
        av_packet_unref(&packet);
        av_init_packet(&packet);
    }

    packet.data = NULL;
    packet.size = 0;
    av_packet_unref(&packet);

    /* note: the internal buffer could have changed, and be != avio_ctx_buffer */
    if (avio)
    {
        av_freep(&avio->buffer);
        av_freep(&avio);
    }

#if ENABLE_SCALE
    sws_freeContext(img_convert_ctx);
    av_frame_free(&pFrameYUV);
#endif

    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

#if DUMP_YUV_DATA
    if (fp_scaled_yuv)
        fclose(fp_scaled_yuv);
#endif

#if TIMEUSAGE
    gettimeofday(&tmEnd, NULL);
    float total = (tmEnd.tv_sec - tmStart.tv_sec) * 1000 + (tmEnd.tv_usec - tmStart.tv_usec) / (1000.0);
    float swScale = tmScale.tv_sec*1000 + tmScale.tv_usec/ (1000.0);
    float swDec = tmDec.tv_sec*1000 + tmDec.tv_usec / (1000.0);

    LOGE("Slice Scale time usage:\n");
    LOGE("total:%f swScale:%f swDec:%f\n", total, swScale, swDec);
    LOGE("swScale percent:%f swDec percent:%f other:%f\n", swScale/total, swDec/total,
        (total - swScale - swDec) / total);
#endif
    return 0;
}

void IPU_FFPHOTO::WorkLoop()
{
    Port *pIn;
    Buffer bfin;
    int ret = 0;

    pIn = GetPort("stream");

    while (IsInState(ST::Running))
    {
        if(!bEnableDecode)
        {
            LOGD("block here! wait for trigger!\n");
            usleep(10*1000);
            continue;
        }
        if (bEnableDecode)
        {
            bDecodeFinish = false;
            memset(&info, 0, sizeof(info));
            try{
                if(pIn->IsBind())
                {
                    bfin = pIn->GetBuffer(&bfin);
                }
                else
                {
                    bfin = decBuf->GetReferenceBuffer(&bfin);
                }
            }catch(const char *err){
                usleep(20000);
                continue;
            }
        }
        else
        {
            LOGE("flag error, continue ...\n");
            continue;
        }

        //do decode
        ret = ImageDecode(&bfin);
        if(ret < 0)
        {
            LOGE("Image decode error(%d)!\n", ret);
        }
        if(pIn->IsBind())
        {
            pIn->PutBuffer(&bfin);
        }
        else
        {
            decBuf->PutReferenceBuffer(&bfin);
        }
        bDecodeFinish = true;
        bEnableDecode = false;
    }
    bEnableDecode = false;
}

IPU_FFPHOTO::IPU_FFPHOTO(std::string name, Json *js)
{
    Port *pOut = NULL, *pIn= NULL;

    Name = name;

    pIn = CreatePort("stream", Port::In);
    pIn->SetPixelFormat(Pixel::Format::JPEG);
    pOut = CreatePort("frame", Port::Out);
    pOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    //pOut->Enable();
    pOut->SetResolution(1920, 1080);
    pOut->SetBufferType(FRBuffer::Type::FIXED, 1);
    pOut->SetPixelFormat(Pixel::Format::NV12);
    pOut->Export();

    avcodec_register_all();
    decBuf = NULL;
}

IPU_FFPHOTO::~IPU_FFPHOTO()
{

}

void IPU_FFPHOTO::Prepare()
{
    char dec_name[24] = {0};
    Port *pIn = NULL;
    Pixel::Format enPixelFormat = GetPort("frame")->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::RGBA8888)
    {
        LOGE("frame Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    pIn = GetPort("stream");
    if(pIn->GetPixelFormat() != Pixel::Format::JPEG)
    {
        LOGE("stream Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    if(!pIn->IsBind())
    {
        LOGE("ffvdec decBuf malloc buffer\n");
        sprintf(dec_name, "%s-stream", Name.c_str());
        if(decBuf == NULL)
        {
            decBuf = new FRBuffer(dec_name, FRBuffer::Type::FIXED_NODROP, 1, ALLOCBUFSIZE);
            if (decBuf ==  NULL){
                LOGE("failed to allocate decBuf.\n");
                return;
            }
        }
    }
    bDecodeFinish = false;
    memset(&info, 0, sizeof(info));
    av_register_all();
}

void IPU_FFPHOTO::Unprepare()
{
    Port *pIpuPort = NULL;

    pIpuPort = GetPort("frame");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("stream");
    if (pIpuPort->IsBind() && pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if(decBuf)
    {
        delete decBuf;
        decBuf = NULL;
    }

}

bool IPU_FFPHOTO::DecodePhoto(bool* bEnable)
{
    LOGE("Begin to decode:%d\n", *bEnable);
    bEnableDecode = *bEnable;
    return true;
}

bool IPU_FFPHOTO::DecodeFinish(bool* bFinish)
{
    *bFinish = bDecodeFinish;
    if (bDecodeFinish)
    {
        bDecodeFinish = false;
    }
    return true;
}
