// Copyright (C) 2016 InfoTM, jinhua.chen@infotm.com
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
#include "SoftLayer.h"

DYNAMIC_IPU(IPU_SoftLayer, "softlayer");

IPU_SoftLayer::IPU_SoftLayer(std::string name, Json *js)
{
    Name = name;
    int i = 0;
    char buf[5] = {0};
    Port *out;

    out = CreatePort("out", Port::Out);
    if(NULL == out) {
        return;
    }
    out->SetResolution(DEFAULT_W, DEFAULT_H);
    out->SetPixelFormat(Pixel::Format::RGBA8888);
    out->SetBufferType(FRBuffer::Type::FIXED, 2);

    for(i = 0; i < MAX_OL_NUM; i++) {
        Port *ol;
        sprintf(buf, "ol%d", i);
        ol = CreatePort(buf, Port::In);
        if(ol == NULL) {
            return;
        }
        ol->SetResolution(DEFAULT_W, DEFAULT_H);
        ol->SetPixelFormat(Pixel::Format::RGBA8888);
        ol->SetBufferType(FRBuffer::Type::FIXED, 2);
        ol->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);
    }

    pthread_mutex_init(&m_PipMutex, NULL);

    return;
}

IPU_SoftLayer::~IPU_SoftLayer()
{
    pthread_mutex_destroy(&m_PipMutex);
}

void IPU_SoftLayer::Prepare()
{
    try {
        CheckPPAttribute();
    }catch ( const char* err ) {
       LOGE("ERROR: %s\n", err);
       return ;
    }

    return ;
}

void IPU_SoftLayer::Unprepare()
{
    Port *pIpuPort = NULL;
    char buf[5] = {0};

    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    for(int i = 0; i < MAX_OL_NUM; i++)
    {
        sprintf(buf, "ol%d", i);
        pIpuPort = GetPort(buf);
        if (pIpuPort && pIpuPort->IsEnabled())
        {
             pIpuPort->FreeVirtualResource();
        }

    }
    return ;
}

bool IPU_SoftLayer::CheckPPAttribute()
{
    Port *pOut, *pOl;
    char buf[5] = {0};
    int i ;

    pOut = GetPort("out");

    if((Pixel::Format::NV12 != pOut->GetPixelFormat()) &&
        (Pixel::Format::RGBA8888 != pOut->GetPixelFormat())) {
        LOGE("Out Format Params error\n");
        throw VBERR_BADPARAM;
    }

    for(i = 0; i < MAX_OL_NUM; i++) {
        sprintf(buf, "ol%d", i);
        pOl = GetPort(buf);
        if(pOl->IsEnabled()){
            if((Pixel::Format::RGBA8888 != pOl->GetPixelFormat()) &&
                (Pixel::Format::NV12 != pOl->GetPixelFormat())) {
                LOGE("ol Format Params error\n");
                throw VBERR_BADPARAM;
            }

            if(pOut->GetPixelFormat() != pOl->GetPixelFormat()) {
                LOGE("out ol Format Params error\n");
                throw VBERR_BADPARAM;
            }

            if((pOl->GetPipX()<0) || (pOl->GetPipX() > pOut->GetWidth()) ||
                (pOl->GetPipY()<0) || (pOl->GetPipY() > pOut->GetHeight()) ||
                ((pOl->GetPipX() + pOl->GetWidth()) > pOut->GetWidth()) ||
                ((pOl->GetPipY() + pOl->GetHeight()) > pOut->GetHeight())) {
                LOGE("ol params error\n");
                throw VBERR_BADPARAM;
            }
            pOl->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
        }
    }

    if(!pOut->IsEmbezzling()){
        LOGE("port out params error must be embezzle\n");
        throw VBERR_BADPARAM;
    }

    return true;
}

void IPU_SoftLayer::WorkLoop()
{
    int skip = 0;
    Buffer dst;
    Port *out;
    out = GetPort("out");

    while(IsInState(ST::Running)){
        try {
            dst = out->GetBuffer(&dst);
        } catch (const char* err) {
            usleep(2000);
            continue;
        }
        if(skip) {
            out->PutBuffer(&dst);
        } else {
            Img_Compose(&dst);
            out->PutBuffer(&dst);
        }
    }
}

struct YUV IPU_SoftLayer::rgbToyuv(char R, char G, char B)
{
    struct YUV yuv;
    yuv.y = 0.299 * R + 0.587 * G + 0.114 * B;
    yuv.u = -0.147 * R - 0.289 * G + 0.436 * B;
    yuv.v = 0.615 * R - 0.515 * G - 0.100 * B;
    return yuv;
}

struct RGB IPU_SoftLayer::yuvTorgb(char Y, char U, char V)
{
    struct RGB rgb;
    rgb.r = (int)((Y & 0xff) + 1.4075 * ((V & 0xff) - 128));
    rgb.g = (int)((Y & 0xff) - 0.3455 * ((U & 0xff) - 128) - 0.7169 * ((V & 0xff) - 128));
    rgb.b = (int)((Y & 0xff) + 1.779 * ((U & 0xff) - 128));
    rgb.r =(rgb.r < 0? 0: rgb.r > 255? 255 : rgb.r);
    rgb.g =(rgb.g < 0? 0: rgb.g > 255? 255 : rgb.g);
    rgb.b =(rgb.b < 0? 0: rgb.b > 255? 255 : rgb.b);
    return rgb;
}

char* IPU_SoftLayer::NV12ToARGB(char *src, int width, int height)
{
    int numOfPixel = width * height;
    int positionOfU = numOfPixel;
    char *rgb = (char*)malloc(numOfPixel * 4);

    if(NULL == rgb) {
        return NULL;
    }

    for(int i = 0; i < height; i++) {
        int startY = i * width;
        int step = i/2 * width;
        int startU = positionOfU + step;
        for(int j = 0; j < width; j++) {
            int Y = startY + j;
            int U = startU + j/2;
            int V = U + 1;
            int index = Y*4;
            RGB tmp = yuvTorgb(src[Y], src[U], src[V]);
            rgb[index + R] = tmp.r;
            rgb[index + G] = tmp.g;
            rgb[index + B] = tmp.b;
        }
    }
    return rgb;
}

int IPU_SoftLayer::ARGBToNV12(Buffer *dst, Port *out, Port *pOl, int width, int height, int offset_x, int offset_y)
{
    int numOfPixel = width * height;
    char *y = (char*)malloc(numOfPixel);
    char *u = (char*)malloc(numOfPixel >> 2);
    char *v = (char*)malloc(numOfPixel >> 2);
    memset(y, 0, numOfPixel);
    memset(u, 0, numOfPixel >> 2);
    memset(v, 0, numOfPixel >> 2);
    int nYoutPos    = 0;
    int nUVPos = 0;
    int u_data = 0;
    int v_data = 0;
    Buffer src;
    char *buf_src = NULL;
    char *buf_dest = NULL;

    if((NULL == y) || (NULL == u) || (NULL == v)) {
        return -1;
    }

    try {
        src = pOl->GetBuffer();
    } catch (const char* err) {
        usleep(20000);
        return 0;
    }
    buf_src = (char*)src.fr_buf.virt_addr;

    for( int nCol = 0; nCol < height; nCol++ ) {
        for( int nWid = 0; nWid < ( width * 4 ); nWid += 4 ) {
            char *data = buf_src + ( nCol * width * 4 ) + nWid;
            int nR = *(data + 2);
             int nG = *(data + 1);
            int nB = *data;

            y[nYoutPos] = RGB2Y(nR, nG, nB);
            if(1 == (nCol % 2)) {
                if(1 != (nWid % 2)) {
                    u_data = RGB2U(nR, nG, nB);
                    v_data = RGB2V(nR, nG, nB);
                } else {
                    u[nUVPos] = (RGB2U(nR, nG, nB) + u_data) >> 1;
                    v[nUVPos] = (RGB2V(nR, nG, nB) + v_data) >> 1;

                    u_data = 0;
                    v_data = 0;
                    nUVPos++;
                }
            }
            nYoutPos++;
        }
    }

    pOl->PutBuffer(&src);

    int out_w = out->GetWidth();

    buf_dest = (char*)dst->fr_buf.virt_addr;

    for( int nCol = 0; nCol < height; nCol++ ) {
        memcpy(buf_dest + (nCol + offset_y) * out_w + offset_x, y, width);
    }

#if 1
    int out_h = out->GetHeight();
    for( int nCol = 0; nCol < height; nCol += 2 ) {
        memcpy(buf_dest + out_w * out_h + (((nCol + offset_y) * out_w) >> 1) + (offset_x), u, width);
        memcpy(buf_dest + out_w * out_h + ((out_w * out_h) >> 1) + (((nCol + offset_y) * out_w) >> 1) + (offset_x), v, width);
    }
#endif

    if(NULL != y) {
        free(y);
        y = NULL;
    }

    if(NULL != u) {
        free(u);
        y = NULL;
    }

    if(NULL != v) {
        free(v);
        y = NULL;
    }
    return 0;
}


int IPU_SoftLayer::Img_Compose_RGBA8888(Buffer *dst, Port *out, int num)
{
    char buf[5] = {0};
    int i, j;
    int w, h;
    int offset_x, offset_y;
    Buffer src;
    char *buf_src = NULL;
    int *buf_dest = NULL;
    size_t size_perline;

    sprintf(buf, "ol%d", num);
    Port *pOl = GetPort(buf);
    if(pOl->IsEnabled()) {
        w = pOl->GetWidth();
        h = pOl->GetHeight();
        pthread_mutex_lock(&m_PipMutex);
        offset_x = pOl->GetPipX();
        offset_y = pOl->GetPipY();
        pthread_mutex_unlock(&m_PipMutex);
        if(Pixel::Format::RGBA8888 == pOl->GetPixelFormat()) {
            try {
                src = pOl->GetBuffer();
            } catch (const char* err) {
                usleep(2000);
                return 0;
            }
            buf_src = (char*)src.fr_buf.priv;
            if(NULL != buf_src) {
                buf_dest = (int*)dst->fr_buf.virt_addr;
                size_perline = out->GetWidth() << 2;
                int size = (w * h) << 2;

                for(i = 0; i < h; i++) {
                    for(j = 0; j < w; j++) {
                        int data = *((int *)buf_src + (w * i + j));
                        if(0 != data) {
                            int data_mask = *((int *)buf_src + ((size + w * i + j) >> 2));
                            int data_dest = *(buf_dest + ((size_perline * (offset_y + i) + ((offset_x + j) << 2)) >> 2)) ;
                            data_dest &= data_mask;
                            data_dest |= data;
                            *(buf_dest + ((size_perline * (offset_y + i) + ((offset_x + j) << 2)) >> 2)) = data_dest;
                        }
                    }
                }
            }
            pOl->PutBuffer(&src);
        } else {
            return -1;
        }
    }
    return 0;
}

int IPU_SoftLayer::Img_Compose_NV12(Buffer *dst, Port *out, int num)
{
    char port_name[5] = {0};
    int i, j;
    int w, h;
    int offset_x, offset_y;
    Buffer src;
    char *buf_src = NULL;
    int *buf_dest = NULL;
    size_t size_perline;
#if 1
    size_t dest_height;
#endif
    sprintf(port_name, "ol%d", num);
    Port *pOl = GetPort(port_name);
    if(pOl->IsEnabled()) {
        w = pOl->GetWidth();
        h = pOl->GetHeight();
        pthread_mutex_lock( &m_PipMutex);
        offset_x = pOl->GetPipX();
        offset_y = pOl->GetPipY();
        pthread_mutex_unlock(&m_PipMutex);
        size_perline = out->GetWidth();
#if 1
        dest_height = out->GetHeight();
#endif
        if(Pixel::Format::NV12 == pOl->GetPixelFormat()) {
#if 1
            try {
                src = pOl->GetBuffer();
            } catch (const char* err) {
                usleep(2000);
                return 0;
            }
            buf_src = (char*)src.fr_buf.priv;
#endif
            if(NULL != buf_src) {
                buf_dest = (int*)dst->fr_buf.virt_addr;
                int size = (w * h * 3) >> 1;

                for(i = 0; i < h; i++) {
                    for(j = 0; j < w; j +=4) {
                        int data = *((int *)buf_src + ((w * i + j) >> 2));

                        if(0 != data) {
                            int data_dest = *(buf_dest + ((size_perline * (offset_y + i) + offset_x + j) >> 2)) ;
                            int data_mask = *((int *)buf_src + ((size + w * i + j) >> 2));
                            data_dest &= data_mask;
                            data_dest |= data;
                            *(buf_dest + ((size_perline * (offset_y + i) + offset_x + j) >> 2)) = data_dest;
                            if((i % 2) == 0)
                            {
                                int uv_off = size_perline * dest_height;
                                int off = ((size_perline * ((offset_y + i) >> 1)) + (offset_x + j));
                                int src_uv_off = w * h;
                                int src_off = (((w * i) >> 1) + j);
                                *(buf_dest + ((uv_off + off) >> 2)) = *((int *)buf_src + ((src_uv_off + src_off) >> 2));
                            }
                        }
                    }
                }
            }
            pOl->PutBuffer(&src);
        } else {
            ARGBToNV12(dst, out, pOl, w, h, offset_x, offset_y);
        }
    }
    return 0;
}

int IPU_SoftLayer::Img_Compose(Buffer *dst)
{
    int i = 0;
    Pixel::Format in_pfmt;
    Port *out = GetPort("out");
    if(NULL == out) {
        return -1;
    }
    in_pfmt = out->GetPixelFormat();

    for(i = 0; i < MAX_OL_NUM; i++) {
        pthread_mutex_lock(&m_PipMutex);
        SoftlayerAdjustOvPos(i);
        pthread_mutex_unlock( &m_PipMutex);
        switch(in_pfmt) {
            case Pixel::Format::RGBA8888:
                Img_Compose_RGBA8888(dst, out, i);
                break;
            case Pixel::Format::NV12:
                Img_Compose_NV12(dst, out, i);
                break;
            default:
                break;
        }
    }

    return 0;
}

void IPU_SoftLayer::SoftlayerAdjustOvPos(int ov_idx)
{
    int out_w, out_h;
    int ov_w, ov_h;
    int ov_x, ov_y;

    Port *out;
    out = GetPort("out");
    out_w = out->GetWidth();
    out_h = out->GetHeight();

    char buf[5] = {0};
    sprintf(buf, "ol%d", ov_idx);
    Port *pOl = GetPort(buf);

    out_w = out->GetWidth();
    out_h = out->GetHeight();

    ov_w = pOl->GetWidth();
    ov_h = pOl->GetHeight();

    ov_x = pOl->GetPipX();
    ov_y = pOl->GetPipY();

    if (ov_w + ov_x >= out_w)
        ov_x = out_w - ov_w ;

    if (ov_h + ov_y >= out_h)
        ov_y = out_h - ov_h;

    if (ov_x < 0)
        ov_x = 0;

    if (ov_y < 0)
        ov_y = 0;

    pOl->SetPipInfo(ov_x, ov_y, 0, 0);
}


int IPU_SoftLayer::SoftlayerFormatPipInfo(struct v_pip_info *vpinfo)
{
    int idx = 0;
    int out_w, out_h;
    int ov_w, ov_h;
    Port *out;
    out = GetPort("out");
    out_w = out->GetWidth();
    out_h = out->GetHeight();

    if(strncmp(vpinfo->portname, "ol0", 3) == 0)
        idx = 0;
    else if(strncmp(vpinfo->portname, "ol1", 3) == 0)
        idx = 1;
    else if(strncmp(vpinfo->portname, "ol2", 3) == 0)
        idx = 2;
    else if(strncmp(vpinfo->portname, "ol3", 3) == 0)
        idx = 3;
    else
        return 0;

    char buf[5] = {0};
    sprintf(buf, "ol%d", idx);
    Port *pOl = GetPort(buf);

    ov_w = pOl->GetWidth();
    ov_h = pOl->GetHeight();

    if (ov_w + vpinfo->x >= out_w)
        vpinfo->x = out_w - ov_w ;

    if (ov_h + vpinfo->y >= out_h)
        vpinfo->y = out_h - ov_h;

    if (vpinfo->x < 0)
        vpinfo->x = 0;

    if (vpinfo->y < 0)
        vpinfo->y = 0;

    LOGD("FormatPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

    return 0;
}


int IPU_SoftLayer::SoftlayerSetPipInfo(struct v_pip_info *vpinfo)
{

    SoftlayerFormatPipInfo(vpinfo);
    pthread_mutex_lock(&m_PipMutex);
    {
        if((strncmp(vpinfo->portname, "ol0", 3) == 0) ||
            (strncmp(vpinfo->portname, "ol1", 3) == 0) ||
            (strncmp(vpinfo->portname, "ol2", 3) == 0) ||
            (strncmp(vpinfo->portname, "ol3", 3) == 0))
        {
            Port *pOl = GetPort(vpinfo->portname);
            pOl->SetPipInfo(vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);
        }
    }
    pthread_mutex_unlock(&m_PipMutex);

    LOGE("SetPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname , vpinfo->x, vpinfo->y, vpinfo->w, vpinfo->h);

    return 0;
}

int IPU_SoftLayer::SoftlayerGetPipInfo(struct v_pip_info * vpinfo)
{
   if((strncmp(vpinfo->portname, "ol0", 3) == 0) ||
        (strncmp(vpinfo->portname, "ol1", 3) == 0) ||
        (strncmp(vpinfo->portname, "ol2", 3) == 0) ||
        (strncmp(vpinfo->portname, "ol3", 3) == 0) ||
        (strncmp(vpinfo->portname, "out", 3) == 0))
    {
        Port *pPort = GetPort(vpinfo->portname);
        vpinfo->x = pPort->GetPipX();
        vpinfo->y = pPort->GetPipY();
        vpinfo->w = pPort->GetWidth();
        vpinfo->h = pPort->GetHeight();
    }

    LOGD("GetPipInfo %s X=%d Y=%d W=%d H=%d\n",vpinfo->portname ,
             vpinfo->x,
             vpinfo->y,
             vpinfo->w,
             vpinfo->h);

    return 0;
}

int IPU_SoftLayer::UnitControl(std::string func, void *arg)
{

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(video_set_pip_info) {
        SoftlayerSetPipInfo((struct v_pip_info *)arg);
        return 0;
    }

    UCFunction(video_get_pip_info) {
        SoftlayerGetPipInfo((struct v_pip_info *)arg);
        return 0;
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}

