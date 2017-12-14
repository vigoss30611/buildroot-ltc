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
#include "FFVDEC.h"
#include "table.h"

DYNAMIC_IPU(IPU_FFVDEC, "ffvdec");
#define ALLOCBUFSIZE 0x80000
#define BUFFER_SIZE_THREADHOLD 0x100000

void IPU_FFVDEC::YUV420p_to_RGBA8888(uint8_t **src, uint8_t *dst, int width, int height)
{
    uint32_t *tables = yuv2rgb565_table;
    uint32_t *dst_ptr = (uint32_t *)(void *)dst;
    int32_t   y_span = width;
    int32_t   uv_span = width>>1;
    int32_t   dst_span = width<<2;

    dst_span >>= 2;
    height -= 1;

    while (height > 0){
        height -= width<<16;
        height += 1<<16;
        while (height < 0){
            /* Do 2 column pairs */
            uint32_t uv, y0, y1;

            uv    = READUV(*src[2]++,*src[1]++);
            y1    = uv + tables[src[0][y_span]];
            y0    = uv + tables[*src[0]++];
            FIXUP(y1);
            FIXUP(y0);
            STORE(y1, dst_ptr[dst_span]);
            STORE(y0, *dst_ptr++);
            y1    = uv + tables[src[0][y_span]];
            y0    = uv + tables[*src[0]++];
            FIXUP(y1);
            FIXUP(y0);
            STORE(y1, dst_ptr[dst_span]);
            STORE(y0, *dst_ptr++);
            height += (2<<16);
        }
        if ((height>>16) == 0){
            /* Trailing column pair */
            uint32_t uv, y0, y1;

            uv = READUV(*src[2],*src[1]);
            y1 = uv + tables[src[0][y_span]];
            y0 = uv + tables[*src[0]++];
            FIXUP(y1);
            FIXUP(y0);
            STORE(y0, dst_ptr[dst_span]);
            STORE(y1, *dst_ptr++);
        }
        dst_ptr += dst_span*2-width;
        src[0]    += y_span*2-width;
        src[2]    += uv_span-(width>>1);
        src[1]    += uv_span-(width>>1);
        height = (height<<16)>>16;
        height -= 2;
    }
    if (height == 0){
        /* Trail row */
        height -= width<<16;
        height += 1<<16;
        while (height < 0){
            /* Do a row pair */
            uint32_t uv, y0, y1;

            uv    = READUV(*src[2]++,*src[1]++);
            y1    = uv + tables[*src[0]++];
            y0    = uv + tables[*src[0]++];
            FIXUP(y1);
            FIXUP(y0);
            STORE(y1, *dst_ptr++);
            STORE(y0, *dst_ptr++);
            height += (2<<16);
        }
        if ((height>>16) == 0){
            /* Trailing pix */
            uint32_t uv, y0;

            uv = READUV(*src[2]++,*src[1]++);
            y0 = uv + tables[*src[0]++];
            FIXUP(y0);
            STORE(y0, *dst_ptr++);
        }
    }
}

void IPU_FFVDEC::WorkLoop()
{
    int len, got_frame;
    Port *pIn, *pOut;
    Buffer bfin, bfout;
    bool HeaderUserflag = false;
    int width, height;
    int s32PortWidth, s32PortHeight, s32Row;
    int idx = 0;

    FfvdecFrameCnt = 0;
    pIn = GetPort("stream");
    pOut = GetPort("frame");
    width = s32PortWidth = pOut->GetWidth();
    height = s32PortHeight = pOut->GetHeight();
    while (IsInState(ST::Running)){
        if (Headerflag == false && HeaderUserflag == false){
            usleep(20000);
            continue;
        } else if(Headerflag == true)
        {
            //high priority to decode header
        }else if(HeaderUserflag) {
            try{
                if(pIn->IsBind())
                    bfin = pIn->GetBuffer(&bfin);
                else
                    bfin = decBuf->GetReferenceBuffer(&bfin);
            }catch(const char *err){
                usleep(20000);
                continue;
            }
        }
DECODE_NEXT:
        av_init_packet(&FfvdecAvpkt);
        if(Headerflag){
            FfvdecAvpkt.data = (uint8_t *)StreamHeader;
            FfvdecAvpkt.size = StreamHeaderLen;
        }else if(HeaderUserflag){
            FfvdecAvpkt.data = (uint8_t *)bfin.fr_buf.virt_addr;
            FfvdecAvpkt.size = bfin.fr_buf.size;
        }else{
            LOGE("ERROR should never be here\n");
            continue;
        }
        while (FfvdecAvpkt.size > 0 && IsInState(ST::Running))
        {
            len = avcodec_decode_video2(FfvdecAvContext, FfvdecAvframe, &got_frame, &FfvdecAvpkt);
            if (len < 0) {
                    //LOGE("Error while decoding frame %d, len:%d\n", FfvdecFrameCnt, len);
                    break;
            }
            if(got_frame){
                if (width != FfvdecAvframe->linesize[0] || ABS_DIFF(height, FfvdecAvframe->height) >= 16)
                {
                    LOGE("!!!FFVDEC SEND VIDEO_FRAME_DUMMY becauseof resolution change next IPU must handle!!!\n");
                    LOGE("video resolution change (%d,%d)->(%d,%d)\n",width, height,FfvdecAvframe->linesize[0],FfvdecAvframe->height);
                    bfout = pOut->GetBuffer();
                    bfout.fr_buf.priv = VIDEO_FRAME_DUMMY;
                    bfout.fr_buf.stAttr.s32Width = FfvdecAvframe->linesize[0];
                    bfout.fr_buf.stAttr.s32Height = FfvdecAvframe->height;
                    width = FfvdecAvframe->linesize[0];
                    height = FfvdecAvframe->height;
                    pOut->PutBuffer(&bfout);
                    usleep(100*1000);
                    if(width * height != s32PortWidth * s32PortHeight)
                    {
                        DeinitBuffer();
                        if(InitBuffer(width, height) < 0)
                        {
                            throw VBERR_BADPARAM;
                        }
                        s32PortWidth = width;
                        s32PortHeight = height;
                    }
                }
                width = FfvdecAvframe->linesize[0];
                height = FfvdecAvframe->height;
                try{
                    bfout = pOut->GetBuffer();
                }catch(const char *err){
                    usleep(20000);
                    continue;
                }
                idx = FfvdecFrameCnt % m_s32OutBufNum;
                if(pOut->GetPixelFormat() == Pixel::Format::NV12){
                    int i, j, lsize1,offset;
                    char *Vaddr = (char *)m_pOutBuf[idx].fr_buf.virt_addr;

                    lsize1 = FfvdecAvframe->linesize[1];
                    /*Luma offset and Chroma Height use small one to avoid buffer overflow*/
                    offset = s32PortWidth * s32PortHeight;
                    if(offset >  width * height)
                        offset = width * height;
                    memcpy(Vaddr, FfvdecAvframe->data[0], offset);
                    Vaddr+=(s32PortWidth * s32PortHeight);
                    s32Row = (s32PortHeight > height) ? height/2 : s32PortHeight/2;
                    for(i = 0; i < s32Row; i++){
                        for(j = 0; j < lsize1; j++){
                            *Vaddr++ = *(FfvdecAvframe->data[1] + i*lsize1 + j);
                            *Vaddr++ = *(FfvdecAvframe->data[2] + i*lsize1 + j);
                        }
                    }
                    bfout.fr_buf.size = (s32PortWidth * s32PortHeight * 3) >> 1;
                }else if(pOut->GetPixelFormat() == Pixel::Format::RGBA8888){
                    s32Row = s32PortHeight > height ? height : s32PortHeight;
                    YUV420p_to_RGBA8888(FfvdecAvframe->data, (uint8_t *)m_pOutBuf[idx].fr_buf.virt_addr, width, s32Row);
                    bfout.fr_buf.size = (s32PortWidth * s32PortHeight) << 2;
                }
                else
                {
                    LOGE("output pixel format not support\n");
                }
                bfout.fr_buf.virt_addr = m_pOutBuf[idx].fr_buf.virt_addr;
                bfout.fr_buf.phys_addr = m_pOutBuf[idx].fr_buf.phys_addr;
                bfout.fr_buf.map_size = m_pOutBuf[idx].fr_buf.map_size;
                bfout.fr_buf.priv = 0;
                pOut->PutBuffer(&bfout);
                FfvdecFrameCnt++;
            }
            if (FfvdecAvpkt.data) {
                FfvdecAvpkt.size -= len;
                FfvdecAvpkt.data += len;
            }
        }
        if (Headerflag && HeaderUserflag)
        {
            //handle play next stream, header and first I frame
            //may be receive at the same loop becuase pIn->getbuffer is block
            Headerflag = false;
            goto DECODE_NEXT;
        }
        if (Headerflag) {
            HeaderUserflag = true;
            Headerflag = false;
        }
        else if (HeaderUserflag)
        {
           if(pIn->IsBind())
                pIn->PutBuffer(&bfin);
            else
                decBuf->PutReferenceBuffer(&bfin);
        }
    }
    Headerflag = false;
    HeaderUserflag = false;
}

IPU_FFVDEC::IPU_FFVDEC(std::string name, Json *js)
{
    Port *pOut = NULL, *pIn = NULL;
    Name = name;
    StreamHeader = NULL;

    pIn = CreatePort("stream", Port::In);
    pIn->SetPixelFormat(Pixel::Format::H264ES);

    pOut = CreatePort("frame", Port::Out);
    pOut->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    pOut->Enable();
    pOut->SetResolution(1920, 1080);
    pOut->SetBufferType(FRBuffer::Type::VACANT, MAX_BUFFERS_COUNT);
    pOut->SetPixelFormat(Pixel::Format::NV12);
    pOut->Export();

    m_bH265 = false;
    if(js != NULL)
    {
        m_bH265 = js->GetInt("h265");
    }
    avcodec_register_all();
    m_s32OutBufNum = 0;
    decBuf = NULL;
    memset(m_pOutFrBuf, 0, sizeof(m_pOutFrBuf));
}

IPU_FFVDEC::~IPU_FFVDEC()
{
}

void IPU_FFVDEC::Prepare()
{
    char dec_name[24] = {0};
    Port* pOut = GetPort("frame");
    Port* pIn = GetPort("stream");
    Pixel::Format enPixelFormat = pIn->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::H264ES && enPixelFormat != Pixel::Format::H265ES)
    {
        LOGE("stream Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    enPixelFormat = pOut->GetPixelFormat();
    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888 && enPixelFormat != Pixel::Format::YUV420P)
    {
        LOGE("frame Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    if(!pIn->IsBind()){
        LOGE("ffvdec decBuf malloc buffer\n");
        sprintf(dec_name, "%s-stream", Name.c_str());
        if(decBuf == NULL){
            decBuf = new FRBuffer(dec_name, FRBuffer::Type::FLOAT_NODROP,  0x244000, ALLOCBUFSIZE);
            if (decBuf ==  NULL){
                LOGE("failed to allocate decBuf.\n");
                return;
            }
        }
    }
    if(InitBuffer(pOut->GetWidth(),pOut->GetHeight()) < 0)
    {
           throw VBERR_BADPARAM;
    }
    StreamHeader = (char*)malloc(128);
    memset(StreamHeader, 0, 128);
    InitFfVdecParam();
    av_init_packet(&FfvdecAvpkt);
    if(m_bH265 == false)
        FfvdecAvcodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    else
        FfvdecAvcodec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
    if (!FfvdecAvcodec) {
        LOGE("Ffvdec Codec not found decoder\n");
           throw VBERR_BADPARAM;
    }

    FfvdecAvContext = avcodec_alloc_context3(FfvdecAvcodec);
    if(!FfvdecAvContext){
        LOGE("Ffvdec could not alloc Avcodec\n");
        throw VBERR_BADPARAM;
    }

    if(m_bH265 == false)
    {
        FfvdecAvContext->extradata = extra;
        FfvdecAvContext->extradata_size = 32;
    }
    else
    {
        FfvdecAvContext->extradata = NULL;
        FfvdecAvContext->extradata_size = 0;
    }

    if (avcodec_open2(FfvdecAvContext, FfvdecAvcodec, NULL) < 0){
        LOGE("Ffvdec could not open codec\n");
        throw VBERR_BADPARAM;
    }

    FfvdecAvframe = av_frame_alloc();
    if(!FfvdecAvframe){
        LOGE("Ffvdev could not alloc frame\n");
        throw VBERR_BADPARAM;
    }
}

void IPU_FFVDEC::Unprepare()
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
    DeinitBuffer();
    if(StreamHeader != NULL)
    {
        free(StreamHeader);
        StreamHeader = NULL;
    }
    avcodec_close(FfvdecAvContext);
    av_free(FfvdecAvContext);
    av_frame_free(&FfvdecAvframe);
}

void IPU_FFVDEC::InitFfVdecParam()
{
    Headerflag = false;
    StreamHeaderLen = 0;
    FfvdecAvcodec = NULL;
    FfvdecAvframe = NULL;
    FfvdecAvContext = NULL;
    memset(extra, 0, 32);
    memset(&FfvdecAvpkt, 0, sizeof(AVPacket));

    extra[0] = 0x00;
    extra[1] = 0x00;
    extra[2] = 0x00;
    extra[3] = 0x01;

    extra[4] = 0x67;
    extra[5] = 0x42;
    extra[6] = 0x80;
    extra[7] = 0x1e;

    extra[8] = 0x88;
    extra[9] = 0x8b;
    extra[10] = 0x40;
    extra[11] = 0x50;

    extra[12] = 0x1e;
    extra[13] = 0xd0;
    extra[14] = 0x80;
    extra[15] = 0x00;

    extra[16] = 0x03;
    extra[17] = 0x84;
    extra[18] = 0x00;
    extra[19] = 0x00;

    extra[20] = 0xaf;
    extra[21] = 0xc8;
    extra[22] = 0x02;
    extra[23] = 0x00;

    extra[24] = 0x00;
    extra[25] = 0x00;
    extra[26] = 0x00;
    extra[27] = 0x01;

    extra[28] = 0x68;
    extra[29] = 0xce;
    extra[30] = 0x38;
    extra[31] = 0x80;
}

bool IPU_FFVDEC::SetStreamHeader(struct v_header_info *strhdr)
{
    if(strhdr->header){
        memcpy(StreamHeader, strhdr->header, strhdr->headerLen);
        Headerflag = true;
        StreamHeaderLen = strhdr->headerLen;
        LOGE("headerLen:%d\n", strhdr->headerLen);
        return true;
    }
    return false;
}

int IPU_FFVDEC::InitBuffer(int s32Width, int s32Height)
{
    int idx = 0, s32BufferSize = 0;
    Port* pOut = GetPort("frame");

    if(pOut->GetPixelFormat() == Pixel::Format::RGBA8888)
        s32BufferSize = s32Width * s32Height *4;
    else
        s32BufferSize = s32Width * s32Height *3/2;

    if ((s32Width * s32Height<= 640  *512))
    {
       //buffer more
       m_s32OutBufNum = MAX_BUFFERS_COUNT;
    }
    else
    {
       //at least 3
       m_s32OutBufNum = 3;
    }

    for(idx = 0 ; idx < m_s32OutBufNum; idx++)
    {
        m_pOutFrBuf[idx] = new FRBuffer(Name.c_str(), FRBuffer::Type::FIXED, 1,s32BufferSize);
        if(m_pOutFrBuf[idx] == NULL)
        {
            if(idx < 3)
            {
                 LOGE("Ffvdec could not new FRBuffer\n");
                 DeinitBuffer();
                 return -1;;
            }
            else
            {
                m_s32OutBufNum = idx;
                break;
            }
        }
        m_pOutBuf[idx] = m_pOutFrBuf[idx]->GetBuffer();
    }
    return 0;
}
void IPU_FFVDEC::DeinitBuffer()
{
    int idx = 0;

    for(idx = 0 ; idx < m_s32OutBufNum; idx++)
    {
        if(m_pOutFrBuf[idx])
        {
            m_pOutFrBuf[idx]->PutBuffer(&m_pOutBuf[idx]);
            delete m_pOutFrBuf[idx];
            m_pOutFrBuf[idx] = NULL;
        }
    }
}

