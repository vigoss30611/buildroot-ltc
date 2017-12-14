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
/*
* note:
*      v1.0 marker的基本框架      2016/04/20
*      v1.1 marker与softlayer IPU功能的连接  2016/04/22
*      v1.2 marker与softlayer关于加快图像合成速度算法的优化，牺牲空间，传递给softlayer的fr buffer 是malloc的。 2016/04/25
*      v1.3 marker与isppost IPU功能的连接    2016/04/26
*      v1.4 marker中新增marker_set_mode API的实现 2016/04/29
*      v1.5 marker_set_mode中字体格式，字体颜色，字体大小等功能的增加 2016/05/08
*      v1.6 marker中自动手动打印功能的区分，以及时间格式化算法的处理  2016/05/10
*      V1.7 marker中中文字符的处理 ，GB2312转UTF-8  2016/08/10
*      v1.8 marker显示位置新增API的实现  2016/08/15
*      v1.9 marker中marker_disable以及marker_enable相关bug的解决  2016/09/01
*            由于port的disable不能够传递，需要在marker_disable时把buffer清空，注意softlayer的处理，容易产生野指针
*      v1.10 marker中支持marker_set_picture API,此API支持输入的图片格式为bmp，需要注意的是isp的nv12格式为NV12 sp,
*           也就是u,v交替存储。 BMP 是标准的32BMP格式   2016/09/18
*/

#include <System.h>
#include <sys/time.h>
#include "Marker.h"
#include <iconv.h>


#define DPI_SIZE 300
#define ALPHA_MAX 15

#define FONT_FILE_PATH "/usr/share/fonts/truetype/"

#define TEST 0
DYNAMIC_IPU(IPU_MARKER, "marker");
#define MARKER_VERSION   V1.10

int IPU_MARKER::NewMarker(std::string name, int w, int h, Pixel::Format pfmt)
{
    Port *p = CreatePort(name.c_str(), Port::Out);
    if(NULL == p) {
        return -1;
    }
    p->SetResolution(w, h);
    p->SetPixelFormat(pfmt);
    p->SetBufferType(FRBuffer::Type::FIXED, 2);

    p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
    if(NULL == bitmap_buf) {
        bitmap_buf = (char*)malloc(w * h);
        if(NULL == bitmap_buf) {
            LOGE("%s can not get buffer\n",Name.c_str());
        }
    }

#ifdef COMPILE_LIB_FREETYPE
    FT_Error error = FT_Init_FreeType(&library);  /* init freetype library */
    if(error) {  /* init freetype library error */
        library = NULL;
        LOGE("FT_Init_FreeType failed\n");
    }
#endif

    return 0;
}

int IPU_MARKER::DeleteMarker(void)
{
    Port *p = GetPort("out");

    if(NULL != p)
    {
        if (p->IsEnabled())
        {
             p->FreeVirtualResource();
        }
        delete p;
    }

    if(NULL != bitmap_buf) {
        free(bitmap_buf);
        bitmap_buf = NULL;
    }
#ifdef COMPILE_LIB_FREETYPE
    if(face != NULL)
    {
        FT_Done_Face(face);
        face = NULL;
    }

    if(library != NULL)
    {
        FT_Done_FreeType(library);
        library = NULL;
    }
#endif

    return 0;
}

IPU_MARKER::IPU_MARKER(std::string name, Json *js)
{
#ifndef COMPILE_LIB_FREETYPE
    lib_chinese = NULL;
    lib_asc = NULL;
    m_lib_chinese_len = m_lib_asc_len = 0;
#endif

    data_buf = NULL;
    Name = name;
    std::string port_name = "out";

    if (NULL != js) {
        if(0 == strcmp(js->GetString("mode"), "normal")) {
            path_mode = SOFTLAYER;
        } else if(0 == strcmp(js->GetString("mode"), "nv12")) {
            path_mode = ISPPOST;
        } else if(0 == strcmp(js->GetString("mode"), "bit2")) {
            path_mode = ISPPOST2;
        } else {
            path_mode = ISPPOST;
        }
    }

    int w = 120;
    int h = 20;
    Pixel::Format pfmt = Pixel::Format::RGBA8888;
    NewMarker(port_name, w, h, pfmt);
    disable_flag = 0;

    memset(&bpp2_overlay_info,0,sizeof(ST_OVERLAY_INFO));
    bpp2_overlay_info.iPaletteTable[0] = 0xffff0000;
    bpp2_overlay_info.iPaletteTable[1] = 0x20008080;
    bpp2_overlay_info.iPaletteTable[2] = 0xffff0000;
    bpp2_overlay_info.iPaletteTable[3] = 0x20008080;
}

IPU_MARKER::~IPU_MARKER()
{
#ifndef COMPILE_LIB_FREETYPE
    if(lib_chinese!= NULL)
        free(lib_chinese);
    if(lib_asc!= NULL)
        free(lib_asc);
#endif
}

int IPU_MARKER::marker_disable(void)
{
    Port *p = GetPort("out");
    Buffer buf1;
    size_t size;
    Pixel::Format pfmt;
    if(NULL == p) {
        return -1;
    }

    if(1 == disable_flag) {
        return 0;
    }
    disable_flag = 1;

    pfmt = p->GetPixelFormat();

    if(1 == path_mode) {    //use softlayer
        switch(pfmt) {
            case Pixel::Format::RGBA8888:
                size = (p->GetWidth() * p->GetHeight() * 5);
                if(NULL != data_buf) {
                    memset(data_buf, 0, size);
                }
                break;
            case Pixel::Format::NV12:
                size = (p->GetWidth() * p->GetHeight() * 5) >> 1;
                if(NULL != data_buf) {
                    memset(data_buf, 0, size);
                }
                break;
            default:
                break;
        }
    } else{  //use isppost
        size_t w = p->GetWidth();
        size_t h = p->GetHeight();
        size = w * h * (Pixel::Bits(p->GetPixelFormat()) >> 3);

retry:
        try {
            buf1 = p->GetBuffer();
        } catch (const char* err) {
            usleep(2000);
            goto retry;
        }

        memset(buf1.fr_buf.virt_addr,0,size);
        bpp2_overlay_info.disable = 1;

        p->PutBuffer(&buf1);
    }

    UpdateString((char *)" ");
    usleep(500*1000);

    RequestOff(1);
    return 0;
}

int IPU_MARKER::marker_enable(void)
{
    if(0 == disable_flag) {
        return 0;
    }
    disable_flag = 0;
    RequestOn(1);
    bpp2_overlay_info.disable = 0;
    return 0;
}

int IPU_MARKER::generate_time_string(char *fmt)
{
        int i, j, k = 0;
        int len = strlen(fmt);
        char data[8] = {0};
        int offset = 0;

        for(i = 0; i < len; i++) {
            if( fmt[i] == '%' ) {
                if( i >= (len - 1) ) {
                    LOGE("%s------->%d    \n", __FUNCTION__, __LINE__);
                    return -1;
                } else {
                    i++;
                    switch(fmt[i]) {
                        case 'c' :
                            strcat(temp, data);
                            time_count = 0;
                            LOGE("%s------->%d    \n", __FUNCTION__, __LINE__);
                            return 0;
                        case 'u' :
                        case 'U' :
                            strcat(temp, data);
                            time_count = 2;
                            LOGE("%s------->%d    \n", __FUNCTION__, __LINE__);
                            return 0;
                        case 't' :
                            time_count = 1;
                            i++;
                            for(j = i; j < len; j++, i++) {
                                if('%' == fmt[j]){
                                    j++;
                                    switch(fmt[j]) {
                                        case 'Y':
                                        case 'y':
                                            offset += 4;
                                            time_order[k] = 'Y';
                                            k++;
                                            strcat(temp, (char *)"%04d");
                                            break;
                                        case 'M':
                                            offset += 4;
                                            time_order[k] = 'M';
                                            k++;
                                            strcat(temp, (char *)"%02d");
                                            break;
                                        case 'D':
                                        case 'd':
                                            offset += 4;
                                            time_order[k] = 'D';
                                            k++;
                                            strcat(temp, (char *)"%02d");
                                            break;
                                        case 'H':
                                        case 'h':
                                            offset += 4;
                                            time_order[k] = 'H';
                                            k++;
                                            strcat(temp, (char *)"%02d");
                                            break;
                                        case 'm':
                                            offset += 4;
                                            time_order[k] = 'm';
                                            k++;
                                            strcat(temp, (char *)"%02d");
                                            break;
                                        case 'S':
                                        case 's':
                                            offset += 4;
                                            time_order[k] = 'S';
                                            k++;
                                            strcat(temp, (char *)"%02d");
                                            break;
                                        default:
                                            LOGE("%s------->%d    \n", __FUNCTION__, __LINE__);
                                            return -1;
                                    }
                                } else {
                                    temp[offset] = fmt[j];
                                    offset++;
                                }
                            }
                            LOGE("%s------->%s    \n", __FUNCTION__, temp);
                            goto out;
                        default :
                            LOGE("%s------->%d    \n", __FUNCTION__, __LINE__);
                            return -1 ;
                    }
                }
            } else {
                temp[offset] = fmt[i];
                offset++;
            }
        }
    out:
        struct tm *mark_tm;
        time_t local_time;
        int data1[6] = {0};

        local_time = time(NULL);
        mark_tm = localtime(&local_time);
        for(i = 0; i < 6; i++) {
            data[i] = Compose(time_order[i], mark_tm);
        }

        for(i = 0; i < 6; i++) {
            if(-1 == data1[i]) {
                for(j = i; j < 6; j++) {
                    if(-1 != data1[j]) {
                        data1[i] = data1[j];
                        data1[j] = -1;
                        break;
                    }
                }
            }
        }

        for(i = 0; i < 6; i++) {
            if( -1 == data1[i]) {
                 break;
            }
        }
        switch(i) {
            case 0:
                 break;
            case 1:
                sprintf(mark, temp, data1[0]);
                break;
            case 2:
                sprintf(mark, temp, data1[0], data1[1]);
                break;
            case 3:
                sprintf(mark, temp, data1[0], data1[1], data1[2]);
                break;
            case 4:
                sprintf(mark, temp, data1[0], data1[1], data1[2], data1[3]);
                break;
            case 5:
                sprintf(mark, temp, data1[0], data1[1], data1[2], data1[3], data1[4]);
                break;
            case 6:
                sprintf(mark, temp, data1[0], data1[1], data1[2], data1[3], data1[4], data1[5]);
                break;
        }

    return 0;
}


int IPU_MARKER::marker_put_frame(void *arg)
{
    Port *p = GetPort("out");
    Buffer buf2;
    if(NULL == p) {
        return -1;
    }
    buf2 = marker->GetReferenceBuffer(&buf2);
    marker_set_bmp((unsigned char*)buf2.fr_buf.virt_addr, buf2.fr_buf.size);
    marker->PutReferenceBuffer(&buf2);
    return 0;
}

int IPU_MARKER::marker_set_mode(int mode, char *fmt, struct font_attr attr)
{
    md = mode;
    char ttf_name[64] = {0};
    FILE *fp;
    char r, g, b;
    memset(temp, 0, TEMP_FMT_LENGTH);

    Port *p = GetPort("out");
    if(NULL == p) {
        return -1;
    }

    int img_height = p->GetHeight();
    int img_width = p->GetWidth();

    set_mode_sucess = 0;

    if(NULL == fmt) {
        return -1;
    }

    memcpy(ft.ttf_name, attr.ttf_name, 32);
    ft.font_size = attr.font_size;
    ft.font_color = attr.font_color;
    ft.back_color = attr.back_color;
    ft.mode = attr.mode;

    ARGBtoAYCbCr(attr.font_color,bpp2_overlay_info.iPaletteTable);
    ARGBtoAYCbCr(attr.back_color,bpp2_overlay_info.iPaletteTable + 1);

    if((ft.mode > RIGHT) || (ft.mode < LEFT)) {
        return -1;
    }

    if(ft.font_size < 0) {
        ft.font_size = 0;
    }

    r = (attr.font_color >> 16);
    g = (attr.font_color >> 8);
    b = (attr.font_color);

    y = (int)(0.257 * r + 0.504 * g + 0.098 * b) + 16;
    u = (int)(-0.148 * r - 0.291 * g + 0.439 * b) + 128;
    v = (int)(0.439 * r - 0.368 * g - 0.071 * b) + 128;

    r = (attr.back_color >> 16);
    g = (attr.back_color >> 8);
    b = (attr.back_color);

    alpha_y = (int)(0.257 * r + 0.504 * g + 0.098 * b) + 16;
    alpha_u = (int)(-0.148 * r - 0.291 * g + 0.439 * b) + 128;
    alpha_v = (int)(0.439 * r - 0.368 * g - 0.071 * b) + 128;

    alpha = (attr.back_color >> 24) * ALPHA_MAX / 255;

#ifdef COMPILE_LIB_FREETYPE
    sprintf(ttf_name, "%s%s.ttf", FONT_FILE_PATH,(char *)ft.ttf_name);
#else
    sprintf(ttf_name, "%s%s", FONT_FILE_PATH,(char *)ft.ttf_name);
#endif
    fp = fopen(ttf_name, "r");//open
    if(NULL == fp){
        LOGE("%s------->%d    \n", __FUNCTION__, __LINE__);
        return -1;
    }
    fclose(fp);

#ifndef COMPILE_LIB_FREETYPE
    font_library_init(ttf_name);
#endif

    generate_time_string(fmt);

    if(AUTO == md) {
        if(0 == ft.font_size) {
            set_mode_sucess = 1;
            return 0;
        }

        int font_pixel_size = ft.font_size * DPI_SIZE / 72;
        if((font_pixel_size > img_height) || (total_width_size > img_width)) {
            LOGE("%s------->please check your font size     \n", __FUNCTION__);
            return -1;
        }

        set_mode_sucess = 1;
    }

    return 0;
}

int IPU_MARKER::UpdateImage(char *path)
{
    Port *p = GetPort("out");
    if(NULL == p) {
        return -1;
    }
    Buffer buf1;
    FILE * nv12_pic = fopen(path,"rb");
    if(NULL == nv12_pic) {
        LOGE("open file(%s) failed\n",path);
        return -1;
    }
    size_t w = p->GetWidth();
    size_t h = p->GetHeight();
    size_t size = w * h * Pixel::Bits(p->GetPixelFormat()) / 8;

    buf1 = p->GetBuffer();

    fseek(nv12_pic, 0, SEEK_END);
    int lenth = ftell(nv12_pic);
    fseek(nv12_pic, 0, SEEK_SET);

    memset(buf1.fr_buf.virt_addr, 0, size);
    fread(buf1.fr_buf.virt_addr,lenth,1,nv12_pic);
    fclose(nv12_pic);

    p->PutBuffer(&buf1);

    return 0;
}

int IPU_MARKER::UpdateString(char *str)
{
    Port *p = GetPort("out");
    Buffer buf;
    if(NULL == p) {
        return -1;
    }
    size_t w = p->GetWidth();
    size_t h = p->GetHeight();

    if((NULL == str) || (NULL == ft.ttf_name)) {
        return -1;
    }

    memset(bitmap_buf, 0, w * h);

#ifdef COMPILE_LIB_FREETYPE
    Get_Fonts_Bitmap_Freetype(str);
#else
    Get_Fonts_Bitmap(str);
#endif

    return 0;
};

void IPU_MARKER::Prepare()
{
    Pixel::Format pfmt;
    Port *p = GetPort("out");
    int size;
    Buffer buf;
    if(NULL == p) {
        return ;
    }

    pfmt = p->GetPixelFormat();
    if ((Pixel::Format::NV21 != pfmt) && (Pixel::Format::NV12 != pfmt) && (Pixel::Format::RGBA8888 != pfmt)
        && (Pixel::Format::BPP2 != pfmt && SOFTLAYER != path_mode))
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        return;
    } // ov width and height must conform to the rules at blow
    bpp2_overlay_info.width = p->GetWidth() -p->GetWidth()%32;
    bpp2_overlay_info.height = p->GetHeight() - p->GetHeight()%32;

    if (bpp2_overlay_info.width < 160)
        bpp2_overlay_info.width = 160;

    if (bpp2_overlay_info.height < 32)
        bpp2_overlay_info.height = 32;

    LOGE("Marker W=%d H=%d\n", bpp2_overlay_info.width, bpp2_overlay_info.height);
    if((p->GetDirection() == Port::Out) && (bpp2_overlay_info.width != p->GetWidth()))
    {
        LOGE("Marker Re-Allocate Buffer\n");
        p->FreeFRBuffer();
        p->SetResolution(bpp2_overlay_info.width,bpp2_overlay_info.height);
        p->AllocateFRBuffer();
        p->SyncBinding();
    }

    if(bitmap_buf != NULL)
    {
        free(bitmap_buf);
        bitmap_buf = (char*)malloc(p->GetWidth()* p->GetHeight());
        if(NULL == bitmap_buf) {
            LOGE("%s can not get buffer\n",Name.c_str());
        }
    }

    if(1 == path_mode) {    //use softlayer
        switch(pfmt) {
            case Pixel::Format::RGBA8888:
                size = (p->GetWidth() * p->GetHeight() * 5);

                if(NULL == data_buf) {
                    data_buf = (char *)malloc(size);
                    if(NULL == data_buf) {
                        return ;
                    }
                    memset(data_buf, 0, size);
                    size = (p->GetWidth() * p->GetHeight() * 4);
                    marker = new FRBuffer(Name.c_str(), FRBuffer::Type::FIXED, 1, size);
                }
                break;
            case Pixel::Format::NV12:
                size = (p->GetWidth() * p->GetHeight() * 5) >> 1;

                if(NULL == data_buf) {
                    data_buf = (char *)malloc(size);
                    if(NULL == data_buf) {
                        return ;
                    }

                    memset(data_buf, 0, size);
                    size = (p->GetWidth() * p->GetHeight() * 4);
                    marker = new FRBuffer(Name.c_str(), FRBuffer::Type::FIXED, 1, size);
                }
                break;
            default:
                return ;
        }
    } else {  //use isp post or isppost2
        if((Pixel::Format::RGBA8888 != pfmt) && (Pixel::Format::NV12 != pfmt) && (Pixel::Format::BPP2 != pfmt)) {
            return ;
        }
        switch(pfmt) {
            case Pixel::Format::RGBA8888:
                size = (p->GetWidth() * p->GetHeight() * 4);
                if(marker == NULL)
                    marker = new FRBuffer(Name.c_str(), FRBuffer::Type::FIXED, 1, size);
                break;
            case Pixel::Format::NV12:
                size = (p->GetWidth() * p->GetHeight() * 4);
                if(marker == NULL)
                    marker = new FRBuffer(Name.c_str(), FRBuffer::Type::FIXED, 1, size);
                break;
            default:
                break;
        }
    }

    return ;
}

/*
*marker_disable take Unprepare
*if free data_buf, frbuf in softlayer will be 野指针
*frbuf指向的内存会被别的程序malloc , 这个 区域的内容将会不确定
*所以在UNprepare中不能释放data_buf
*/
void IPU_MARKER::Unprepare()
{
    Port *pIpuPort = NULL;

    if(marker != NULL)
    {
        delete marker;
        marker = NULL;
    }

    if(NULL != data_buf) {
        free(data_buf);
        data_buf = NULL;
    }

#ifdef COMPILE_LIB_FREETYPE
    if(face != NULL)
    {
        FT_Done_Face(face);
        face = NULL;
    }
#endif

    pIpuPort = GetPort("out");
    if (pIpuPort != NULL && pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    return ;
}

int IPU_MARKER::Compose(char data, struct tm *mark_tm)
{
    int time;
    switch(data) {
        case 'Y':
            time = mark_tm->tm_year + 1900;
            break;
        case 'M':
            time = mark_tm->tm_mon + 1;
            break;
        case 'D':
            time = mark_tm->tm_mday;
            break;
        case 'H':
            time = mark_tm->tm_hour;
            break;
        case 'm':
            time = mark_tm->tm_min;
            break;
        case 'S':
            time = mark_tm->tm_sec;
            break;
        default:
            time = -1;
            break;
    }
    return time;
}

int IPU_MARKER::process_time(char *mark, int *last_sec)
{
    struct tm *mark_tm;
    time_t local_time;
    int i = 0, j = 0;
    int data[6] = {0};

    local_time = time(NULL);
    mark_tm = localtime(&local_time);
    if(*last_sec == mark_tm->tm_sec) {
        return -1;
    } else {
        *last_sec = mark_tm->tm_sec;
    }
    for(i = 0; i < 6; i++){
        data[i] = Compose(time_order[i], mark_tm);
    }

    for(i = 0; i < 6; i++) {
        if(-1 == data[i]) {
            for(j = i; j < 6; j++) {
                if(-1 != data[j]) {
                    data[i] = data[j];
                    data[j] = -1;
                    break;
                }
            }
        }
    }

    for(i = 0; i < 6; i++){
        if( -1 == data[i]){
            break;
        }
    }
    switch(i) {
        case 0:
            break;
        case 1:
            sprintf(mark, temp, data[0]);
            break;
        case 2:
            sprintf(mark, temp, data[0], data[1]);
            break;
        case 3:
            sprintf(mark, temp, data[0], data[1], data[2]);
            break;
        case 4:
            sprintf(mark, temp, data[0], data[1], data[2], data[3]);
            break;
        case 5:
            sprintf(mark, temp, data[0], data[1], data[2], data[3], data[4]);
            break;
        case 6:
            sprintf(mark, temp, data[0], data[1], data[2], data[3], data[4], data[5]);
            break;
    }
    return 0;
}

int IPU_MARKER::process_utc(char *mark)
{
    long seconds= time((time_t*)NULL);
    sprintf(mark, "%ld", seconds);
    return 0;
}

#if 1
void IPU_MARKER::WorkLoop()
{
    char mark[128];
    int last_sec = 0;

    while(IsInState(ST::Running)){

        if((AUTO == md) && (0 == disable_flag)) {
            if(1 == time_count) {
                if(0 == process_time(mark, &last_sec)) {
                    if(1 == set_mode_sucess) {
                        UpdateString((char*)mark);
                    }
                }
            } else if(2 == time_count) {
                if(0 == process_utc(mark)) {
                    UpdateString((char*)mark);
                }
            }
        }

        usleep(600000);
    }

    return ;
}
#endif

int IPU_MARKER::UnitControl(std::string func, void *arg)
{
    UCSet(func);

    UCFunction(marker_disable) {
        return marker_disable();
    }

    UCFunction(marker_enable) {
        return marker_enable();
    }

    UCFunction(marker_set_mode) {
        struct marker_mode_data *data = (struct marker_mode_data *)arg;
        return marker_set_mode((int)(data->mode), data->fmt, data->attr);
    }

    UCFunction(marker_set_picture) {
        struct file_path *data = (struct file_path *)arg;
        return UpdateImage(data->path);
    }

    UCFunction(marker_set_string) {
        struct marker_data *data = (struct marker_data *)arg;
        return UpdateString(data->str);
    }

    UCFunction(marker_put_frame) {
        return marker_put_frame(arg);
    }

    return 0;
}

int IPU_MARKER::Bitmap_To_Rgb8888(char *m_pImgData)
{
    Port *p = GetPort("out");
    Buffer buf1;
    if(NULL == p) {
        return -1;
    }
    int i, j;
    int m_imgHeight = p->GetHeight();
    int m_imgWidth = p->GetWidth();
    int BytesPerLineOut = 4 * m_imgWidth;
    int BytesPerLineIn = m_imgWidth;

    buf1 = p->GetBuffer();
    char *buf = (char*)buf1.fr_buf.virt_addr;

    if(NULL == m_pImgData) {
        p->PutBuffer(&buf1);
        return -1;
    }

    char data;
    int size = (m_imgHeight * m_imgWidth *4);

    for(i = 0; i < m_imgHeight; i++) {
        for(j = 0; j < m_imgWidth; j++) {
            data = *(m_pImgData + i*BytesPerLineIn + j);
            if(1 == path_mode) {
                if(0 == data) {
                    *(data_buf + size + i*BytesPerLineOut+ j) = 0xff;
                } else {
                    *(data_buf + size + i*BytesPerLineOut+ j) = 0;
                }
            }
#if 0
            for(k = 0; k < 4; k++) {
                if(0 == data){
                    *(buf + i*BytesPerLineOut+ j*4 + k) = 0;
                } else {
                    *(buf + i*BytesPerLineOut+ j*4 + k) = ft.font_color;
                }
                if(1 == path_mode) {
                    if(0 == data){
                        *(data_buf + i*BytesPerLineOut+ j*4 + k) = 0;
                    } else {
                        *(data_buf + i*BytesPerLineOut+ j*4 + k) = ft.font_color;
                    }
                }
            }
#else
            if(0 == data){
                *(buf + i*BytesPerLineOut+ j) = ft.back_color;
            } else {
                *(buf + i*BytesPerLineOut+ j) = ft.font_color;
            }
            if(1 == path_mode) {
                if(0 == data){
                    *((int *)data_buf + i*BytesPerLineOut+ j) = ft.back_color;
                } else {
                    *((int *)data_buf + i*BytesPerLineOut+ j) = ft.font_color;
                }
            }
#endif
        }
    }
    p->PutBuffer(&buf1);

    return 0;
}

int IPU_MARKER::Bitmap_To_NV12(char *m_pImgData)
{
    Port *p = GetPort("out");
    Buffer buf1;
    if(NULL == p) {
        return -1;
    }
    int i, j;
    int m_imgHeight = p->GetHeight();
    int m_imgWidth = p->GetWidth();
    int BytesPerLineOut = m_imgWidth;
    int BytesPerLineIn = m_imgWidth;

    buf1 = p->GetBuffer();
    char *buf = (char*)buf1.fr_buf.virt_addr;

    if(NULL == m_pImgData) {
        p->PutBuffer(&buf1);
        return -1;
    }

    int size = (m_imgHeight * m_imgWidth *3) >> 1;
    for(i = 0; i < m_imgHeight; i++) {
        for(j = 0; j < m_imgWidth; j++) {
            char data = *(m_pImgData + i * BytesPerLineIn + j);
            if(1 == path_mode) {
                if(0 == data) {
                    *(data_buf + size + i * BytesPerLineOut + j) = 0xff;
                    *(data_buf + i * BytesPerLineOut + j) = 0;
                } else {
                    *(data_buf + size + i * BytesPerLineOut + j) = 0;
                    *(data_buf + i * BytesPerLineOut + j) = y;
                }
            }

            if(0 == data) {
                if(0 == path_mode) {
                    *(buf + i * BytesPerLineOut + j) = ((alpha_y & 0xFC) | (alpha >> 2));
                } else {
                    *(buf + i * BytesPerLineOut + j) = 0;
                }
            } else {
                if(0 == path_mode) {
                    *(buf + i * BytesPerLineOut + j) = (y | 0x03);
                } else {
                    *(buf + i * BytesPerLineOut + j) = y;
                }
            }
        }
    }
#if 0
    memset(buf + m_imgWidth * m_imgHeight, 0, (m_imgWidth * m_imgHeight) >> 1);
    if(1 == path_mode) {
        memset(data_buf + m_imgWidth * m_imgHeight, u, (m_imgWidth * m_imgHeight) >> 2);
        memset(data_buf + m_imgWidth * m_imgHeight + ((m_imgWidth * m_imgHeight) >> 2), v, (m_imgWidth * m_imgHeight) >> 2);
        buf1.fr_buf.priv = (int)data_buf;
    }
    memset(buf + m_imgWidth * m_imgHeight, u, (m_imgWidth * m_imgHeight) >> 2);
    memset(buf + m_imgWidth * m_imgHeight + ((m_imgWidth * m_imgHeight) >> 2), v, (m_imgWidth * m_imgHeight) >> 2);
#else
    int offset_uv = BytesPerLineIn * m_imgHeight;
    for(i = 0; i < (m_imgHeight >> 1); i++) {
        for(j = 0; j < m_imgWidth; j += 2) {
            char data1 = *(m_pImgData + (i << 1) * BytesPerLineIn + j);
            if(1 == path_mode) {
                *(buf + (i * BytesPerLineOut + j) + offset_uv) = u;
                *(data_buf + (i * BytesPerLineOut + j) + offset_uv) = u;
            } else {
                if(0 == data1) {
                    if((alpha & 0x02) == 0) {
                        *(buf + (i * BytesPerLineOut + j) + offset_uv) = (alpha_u & 0xFE);
                    } else {
                        *(buf + (i * BytesPerLineOut + j) + offset_uv) = (alpha_u | 0x01);
                    }
                } else {
                    if(0 == path_mode) {
                        *(buf + (i * BytesPerLineOut + j) + offset_uv) = (u | 0x01);
                    } else {
                        *(buf + (i * BytesPerLineOut + j) + offset_uv) = u;
                    }
                }
            }
            if(1 == path_mode) {
                *(buf + (i * BytesPerLineOut + j + 1) + offset_uv) = v;
                *(data_buf + (i * BytesPerLineOut + j + 1) + offset_uv) = v;
            } else {
                if(0 == data1) {
                    if((alpha & 0x01) == 0) {
                        *(buf + (i * BytesPerLineOut + j + 1) + offset_uv) = (alpha_v & 0xfe);
                    } else {
                        *(buf + (i * BytesPerLineOut + j + 1) + offset_uv) = (alpha_v | 0x01);
                    }
                    } else {
                        if(0 == path_mode) {
                            *(buf + (i * BytesPerLineOut + j + 1) + offset_uv) = (v | 0x01);
                        } else {
                            *(buf + (i * BytesPerLineOut + j + 1) + offset_uv) = v;
                        }
                    }
            }

            if((0x0 == *(m_pImgData + (i << 1) * BytesPerLineOut + j))
                &&(((((i << 1) - 2) >=0) && (0x0 != *(m_pImgData + ((i << 1) - 2) * BytesPerLineOut + j)))
                ||(((j-2) >=0) && (0x0 != *(m_pImgData + (i << 1) * BytesPerLineOut + j - 2)))
                ||((((i << 1) + 2) < m_imgHeight) && (0x0 != *(m_pImgData + ((i << 1) + 2) * BytesPerLineOut + j)))
                ||(((j+2) < m_imgWidth) && (0x0 != *(m_pImgData + (i << 1) * BytesPerLineOut + j + 2)))))
            {
                *(buf + (i << 1) * BytesPerLineOut + j) = 16 | 0x03;
                *(buf + (i) * BytesPerLineOut + j + offset_uv) = 128 | 0x01;
                *(buf + (i) * BytesPerLineOut + j + 1 + offset_uv) = 128 | 0x01;
            }

        }
    }
    if(1 == path_mode) {
        buf1.fr_buf.priv = (int)data_buf;
    }
    else
    {
        buf1.fr_buf.priv = (int)&bpp2_overlay_info;
    }
#endif

    p->PutBuffer(&buf1);

    return 0;
}

int IPU_MARKER::Bitmap_To_BPP2(char *m_pImgData)
{
    char bpp2_data = 0;
    int count = 0;
    int cur_pos = 0;
    Port *p = GetPort("out");
    Buffer buf1;
    if(NULL == p) {
        return -1;
    }
    int i, j;
    int m_imgHeight = p->GetHeight();
    int m_imgWidth = p->GetWidth();
    int BytesPerLineOut = m_imgWidth;

    buf1 = p->GetBuffer();
    char *buf = (char*)buf1.fr_buf.virt_addr;

    if(NULL == m_pImgData) {
        p->PutBuffer(&buf1);
        return -1;
    }

    for(i = 0; i < m_imgHeight; i++) {
        for(j = 0; j < m_imgWidth; j++) {
            char data = *(m_pImgData + i * BytesPerLineOut+ j);
            if(data != 0)
            {
                bpp2_data |= 0 << count * 2;
            }
            else
            {
                bpp2_data |= 1 << count * 2;
            }
            count++;

            if(count == 4)
            {
                cur_pos = (i * BytesPerLineOut+ j) / 4;
                buf[cur_pos] = bpp2_data;
                count = 0;
                bpp2_data = 0;
            }
        }
    }

    buf1.fr_buf.priv = (int)&bpp2_overlay_info;
    p->PutBuffer(&buf1);

    return 0;
}


int IPU_MARKER::show_image(void)
{
    int  i, j;
    Port *p = GetPort("out");
    if(NULL == p) {
        return -1;
    }
    int m_imgHeight = p->GetHeight();
    int m_imgWidth = p->GetWidth();

    for ( i = 0; i < (signed int)m_imgHeight; i++ ) {
        for ( j = 0; j < (signed int)m_imgWidth; j++ ) {
            putchar(*(bitmap_buf + i * m_imgWidth + j) == 0 ? ' ': *(bitmap_buf + i * m_imgWidth + j) < 128 ? '+': '*');
        }
        putchar( '\n' );
      }
      return 0;
}

/*encode change:from one to another*/
int IPU_MARKER::code_convert(char *from_charset,char *to_charset,const char *inbuf,unsigned int inlen,char *outbuf,unsigned int outlen)
{
    unsigned int outlen_bak = outlen;

    iconv_t cd;
    const char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset, from_charset);
    if (0 == cd) {
        printf("iconv_open error:%s\n",strerror(errno));
        return -1;
    }
    memset(outbuf,0,outlen);
    if (-1 == (signed int)iconv(cd, (char**)pin, &inlen, (char**)pout, &outlen)) {
        LOGE("iconv error:%s\n",strerror(errno));
    }
    iconv_close(cd);
    return (outlen_bak - outlen);
}

/*GB2312 -> utf-8*/
int IPU_MARKER::GB2312_To_UTF8(char *inbuf, unsigned int inlen, char *outbuf, unsigned int outlen)
{
    return code_convert((char *)"GBK", (char *)"UTF-8", inbuf, inlen, outbuf, outlen);
}

/*GB2312 <- utf-8*/
int IPU_MARKER::UTF8_To_GB2312(char *inbuf, unsigned int inlen, char *outbuf, unsigned int outlen)
{
    return code_convert((char *)"UTF-8", (char *)"GBK", inbuf, inlen, outbuf, outlen);
}

int IPU_MARKER::UTF8_To_UNICODE(char *inbuf, unsigned int inlen, char *outbuf, unsigned int outlen)
{
    return code_convert((char *)"UTF-8", (char *)"UCS-4LE", inbuf, inlen, outbuf, outlen);
}


/*assume in_text gb2312*/
unsigned int IPU_MARKER::Get_Unicode(char *in_text, wchar_t *unicode_text, unsigned int out_buf_len)
{
    if(NULL == unicode_text)
    {
        LOGE("Get_Unicode out param unicode_text is NULL\n");
        return -1;
    }

    unsigned int unicode_num;

    char utf_8[256] = {0};
    char in_text_mirror[256] = {0};

    GB2312_To_UTF8(in_text,strlen(in_text),utf_8,256);
    UTF8_To_GB2312(utf_8,strlen(utf_8),in_text_mirror,256);

    if(strcmp(in_text,in_text_mirror) == 0)//gb2312
        unicode_num = UTF8_To_UNICODE(utf_8,strlen(utf_8),(char *)unicode_text,out_buf_len);
    else//utf-8
        unicode_num = UTF8_To_UNICODE(in_text,strlen(in_text),(char *)unicode_text,out_buf_len);

    return unicode_num / 4;
}

#ifdef COMPILE_LIB_FREETYPE
/* Replace this function with something useful. */
int IPU_MARKER::draw_bitmap(FT_Bitmap*  bitmap, FT_Int x, FT_Int y, int offset)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;
    Port *port = GetPort("out");

    if(NULL == port) {
        return -1;
    }
    int m_imgHeight = port->GetHeight();
    int m_imgWidth = port->GetWidth();
#if TEST
    Buffer buf1;
    buf1 = port->GetBuffer();
    char *buf = (char*)buf1.fr_buf.virt_addr;
#endif

    for (i = x, p = 0; i < x_max; i++, p++ ) {
        for (j = y, q = 0; j < y_max; j++, q++ ) {
            if (i < 0 || j < 0 || i >= (signed int)m_imgWidth || j >= (signed int)m_imgHeight ) {
                continue;
            }
#if TEST
            *(buf + j * m_imgWidth + i + offset)  = bitmap->buffer[q * bitmap->width + p];
#endif
            *(bitmap_buf + j * m_imgWidth + i + offset)  = bitmap->buffer[q * bitmap->width + p];
        }
    }
#if TEST
    port->PutBuffer(&buf1);
#endif
    return 0;
}

int IPU_MARKER::Get_Fonts_offset(wchar_t *in_text)
{
    FT_GlyphSlot  slot;
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;

    int           n;

    Port *p = GetPort("out");
    if(NULL == p) {
        return -1;
    }

    int m_imgWidth = p->GetWidth();

    slot = face->glyph;

    pen.x = 0 * 64;
    pen.y = 0 * 64;

    offset = 0;

    for ( n = 0; n < m_num_chars; n++ ) {
        /* set transformation */
        FT_Set_Transform( face, &matrix, &pen );

        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char( face, in_text[n], FT_LOAD_RENDER );
        if ( error ) {
            LOGE("%s-->load char fail \n", __FUNCTION__);
              continue;/* ignore errors */
        }

        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
      }

      if(0 == ft.font_size){
          offset = (m_imgWidth - slot->bitmap_left - slot->bitmap.width) >> 1;
          total_width_size =  slot->bitmap_left + slot->bitmap.width;
      } else {
          switch(ft.mode) {
              case LEFT:
                  offset = 0;
                  break;
              case MIDDLE:
                  offset = (m_imgWidth - slot->bitmap_left - slot->bitmap.width) >> 1;
                  break;
              case RIGHT:
                  offset =  m_imgWidth - slot->bitmap_left - slot->bitmap.width;
                  break;
              default:
                  offset = 0;
                  break;
          }

          total_width_size =  slot->bitmap_left + slot->bitmap.width;
    }

    LOGD("%s------>%d   %d offset:%d\n", __FUNCTION__, slot->bitmap_left, slot->bitmap.width,offset);

    return 0;
}

int IPU_MARKER::Get_Fonts_Bitmap_Freetype(char *in_text)
{
    FT_GlyphSlot  slot;
    FT_Vector     pen;                    /* untransformed origin  */
    FT_Error      error;

    unsigned int           n;
    unsigned int           num;

    int           num_chars;

    Port *p = GetPort("out");
    if(NULL == p) {
        return -1;
    }

    int img_height = p->GetHeight();
    int img_width = p->GetWidth();

    double  angle = ( 0.0 / 360 ) * 3.14159 * 2;/* use 0 degrees     */

    /* set up matrix */
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

    if(face != NULL)
    {
        FT_Done_Face(face);
        face = NULL;
    }

    char ttf_name[64] = {0};
    sprintf(ttf_name, "/usr/share/fonts/truetype/%s.ttf", (char *)ft.ttf_name);

    /* get font from the font file */
    error = FT_New_Face(library, ttf_name, 0 , &face);
    if( error == FT_Err_Unknown_File_Format ) {
        face = NULL;
        LOGE("%s->unknow file format    \n", __FUNCTION__); /* unkonw file format */
        return -1;
    } else if(error) {
        face = NULL;
        LOGE("%s->open file font file fail   \n", __FUNCTION__);/* open error */
        return -1;
    }

#if 0
    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size(     /* set character size */
          face,          /* handle to face object            */
          10 * 64,         /* char_width in 1/64th of points    */
          0,             /* char_height in 1/64th of points */
          160,             /* horizontal device resolution    */
          0 );             /* vertical device resolution        */
    /* error handling omitted */
#else
    num_chars = strlen(mark);

    target_height = img_width / num_chars;
    if(target_height > (int)img_height) {
        target_height = img_height;
    }

    target_height = img_height;

    //set font size
    if(0 == ft.font_size){
        error = FT_Set_Pixel_Sizes(face,((img_width << 1) / (num_chars + 1)), target_height);
    } else {
        int font_pixel_size = ft.font_size * DPI_SIZE / 72;
        error = FT_Set_Pixel_Sizes(face, font_pixel_size, font_pixel_size);
    }
#endif
    wchar_t *unicode_text = NULL;
    unsigned out_buf_len = sizeof(wchar_t)*(strlen(in_text)*2);
    unicode_text = (wchar_t*)calloc(1,out_buf_len);
    num = Get_Unicode(in_text,unicode_text,out_buf_len);
    m_num_chars = num;

    FT_Select_Charmap(face, FT_ENCODING_UNICODE);   /* set UNICODE */

    Get_Fonts_offset(unicode_text);

    slot = face->glyph;

    /* the pen position in 26.6 cartesian space coordinates; */
    /* start at (30,20) relative to the upper left corner  */
    pen.x = 0 * 64;
    pen.y = 0 * 64;

    FT_Glyph   glyph;

    for ( n = 0; n < num; n++ ) {
        /* set transformation */
        FT_Set_Transform( face, &matrix, &pen );

        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char( face, unicode_text[n], FT_LOAD_DEFAULT | FT_LOAD_RENDER );
        if ( error ) {
            LOGE("%s-->load char fail \n", __FUNCTION__);
              continue;                 /* ignore errors */
          }
         FT_Get_Glyph(face -> glyph, &glyph);
         FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0 ,1);

        /* now, draw to our target surface (convert position) */
        draw_bitmap(&slot->bitmap, slot->bitmap_left, (target_height - slot->bitmap_top)-target_height*2/9, offset);

        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
        FT_Done_Glyph(glyph);
        glyph = NULL;
      }
#if TEST
      show_image();
#endif

#if 1
    Pixel::Format pfmt = p->GetPixelFormat();
    if(Pixel::Format::RGBA8888 == pfmt) {
        Bitmap_To_Rgb8888(bitmap_buf);
    } else if(Pixel::Format::NV12 == pfmt){
        Bitmap_To_NV12(bitmap_buf);
    } else if(Pixel::Format::BPP2 == pfmt){
        Bitmap_To_BPP2(bitmap_buf);
    }
#endif
    if(NULL != unicode_text) {
        free(unicode_text);
    }

    FT_Done_Face(face);
    face = NULL;

    return 0;
}

#else

//初始化字库
int IPU_MARKER::font_library_init(char *font_file)
{
    int retval = 0;
    FILE *font_fd = NULL;

    //chinese
    if((font_fd = fopen(font_file, "rb")) == NULL)
    {
        LOGE("Open %s failed !", font_file);
        return -1;
    }

    fseek(font_fd, 0, SEEK_END);
    m_lib_chinese_len= ftell(font_fd);
    rewind(font_fd);

    if(lib_chinese != NULL)
        free(lib_chinese);

    lib_chinese = (char*)malloc(m_lib_chinese_len);
    if(lib_chinese == NULL)
    {
        LOGE("Couldn't allocate memory");
        fclose(font_fd);
        return -1;
    }

    retval = fread(lib_chinese, 1, m_lib_chinese_len,font_fd);
    if(retval != m_lib_chinese_len)
    {
        LOGE("fread failed retval = %d, lSize = %d",retval, m_lib_chinese_len);
        free(lib_chinese);
        lib_chinese = NULL;
        fclose(font_fd);
        return -1;
    }
    fclose(font_fd);

    char asc_file_path[128]={0};
    sprintf(asc_file_path, "%s%s", FONT_FILE_PATH,"asc16");
    //ascii
    if((font_fd = fopen(asc_file_path, "rb")) == NULL)
    {
        LOGE("Open %s failed !", asc_file_path);
        return -1;
    }

    fseek(font_fd, 0, SEEK_END);
    m_lib_asc_len= ftell(font_fd);
    rewind(font_fd);

    if(lib_asc!= NULL)
        free(lib_asc);

    lib_asc = (char*)malloc(m_lib_asc_len);
    if(lib_asc == NULL)
    {
        LOGE("Couldn't allocate memory");
        fclose(font_fd);
        return -1;
    }

    retval = fread(lib_asc, 1, m_lib_asc_len,font_fd);
    if(retval != m_lib_asc_len)
    {
        LOGE("fread failed retval = %d, lSize = %d",retval, m_lib_asc_len);
        free(lib_asc);
        lib_asc = NULL;
        fclose(font_fd);
        return -1;
    }
    fclose(font_fd);

    return 0;
}


int IPU_MARKER::Get_Fonts_Bitmap(char *in_text)
{
    char *in_text_gb2312 = NULL;
    char utf_8[256] = {0};
    char in_text_mirror[256] = {0};
    int offset_y = 0;
    int i = 0;
    char ch0;
    char ch1;
    unsigned char qH;//gb2312 qu hao
    unsigned char wH;//gb2312 wei hao
    char font_width_bytes;
    char *font = NULL;
    int pos = 0;
    int scale=2;

    if(ft.font_size != 0)
        scale = ft.font_size;

    Port *p = GetPort("out");
    if(NULL == p) {
        return -1;
    }

    int img_width = p->GetWidth();
    int img_height = p->GetHeight();

    GB2312_To_UTF8(in_text,strlen(in_text),utf_8,256);
    UTF8_To_GB2312(utf_8,strlen(utf_8),in_text_mirror,256);
    if(strcmp(in_text,in_text_mirror) == 0)//gb2312
        in_text_gb2312 = in_text;

    else//utf-8
    {
        memset(in_text_mirror,0,sizeof(in_text_mirror));
        UTF8_To_GB2312(in_text,strlen(in_text),in_text_mirror,256);
        in_text_gb2312 = in_text_mirror;
    }

    int font_width = 16;
    char tmp[32];
    sscanf(ft.ttf_name,"%s%d",tmp,&font_width);
    int in_text_len = strlen(in_text_gb2312);
    total_width_size = font_width / 2 * in_text_len * scale;
    total_height_size = font_width*scale;
    if((total_width_size > img_width) ||( total_height_size >img_height))
    {
        LOGE("marker text len(%d) is big than port width(%d)\n",total_width_size,img_width);
        return -1;
    }

    if(0 == ft.font_size){
          offset = (img_width - total_width_size) >> 1;
      } else {
          switch(ft.mode) {
              case LEFT:
                  offset = 0;
                  break;
              case MIDDLE:
                  offset = (img_width - total_width_size) >> 1;
                  break;
              case RIGHT:
                  offset =  img_width - total_width_size;
                  break;
              default:
                  offset = 0;
                  break;
          }
    }

    offset_y = (img_height - total_height_size) >> 1;



    while(i<in_text_len)
    {
        /*ascii*/
        if(in_text_gb2312[i] < 0xa0)
        {
            ch0 = in_text_gb2312[i];
            font_width_bytes = 1;
            pos = ch0 * font_width * font_width_bytes;
            if(pos >= m_lib_asc_len)
            {
                LOGE("char is out of library\n");
                return -1;
            }
            font = &lib_asc[pos];
        }
        /*chinese*/
        else
        {
            ch0 = in_text_gb2312[i];
            ch1 = in_text_gb2312[i+1];
            qH= ch0 - 0xa0;
            wH= ch1 - 0xa0;
            font_width_bytes = 2;
            pos = (94*(qH-1)+(wH-1))*font_width*font_width_bytes;
            if(pos >= m_lib_chinese_len)
            {
                LOGE("char is out of library\n");
                return -1;
            }
            font = &lib_chinese[pos];//notice yi chu
        }

        for(int j=0;j<font_width;j++)//font size lines
        {
            for(int m=0;m<font_width_bytes;m++)
            {
                for(int k=0;k<8;k++)
                {
                    if(((font[j*font_width_bytes+m]>>(8-k-1)) & 0x1) != 0)
                    {
                        //scale
                        for(int l=0;l<scale;l++)
                        {
                            for(int n=0;n<scale;n++)
                            {
                                *(bitmap_buf + (offset_y + scale*j +n) * img_width + offset + (m*8 + k)*scale + l) = 1;
                            }
                        }
                    }
                }
            }
        }

        offset += font_width_bytes * 8 * scale;
        i+=font_width_bytes;
    }

#if TEST
      show_image();
#endif

    Pixel::Format pfmt = p->GetPixelFormat();
    if(Pixel::Format::RGBA8888 == pfmt) {
        Bitmap_To_Rgb8888(bitmap_buf);
    } else if(Pixel::Format::NV12 == pfmt){
        Bitmap_To_NV12(bitmap_buf);
    } else if(Pixel::Format::BPP2 == pfmt){
        Bitmap_To_BPP2(bitmap_buf);
    }

    return 0;
}

#endif

/* r,g,b data to y,u,v data */
struct YUV IPU_MARKER::rgbToyuv(char R, char G, char B)
{
    struct YUV yuv;
    yuv.y = (int)(0.257 * R + 0.504 * G + 0.098 * B) + 16;
    yuv.u = (int)(-0.148 * R - 0.291 * G + 0.439 * B) + 128;
    yuv.v = (int)(0.439 * R - 0.368 * G - 0.071 * B) + 128;
    return yuv;
}

void IPU_MARKER::rgbToyuv(unsigned char R, unsigned char G , unsigned char B,
        unsigned char *Y, unsigned char * Cb, unsigned char *Cr)
{
    *Y = 0.257*R  + 0.504*G  + 0.098*B + 16;
    *Cb = -0.148*R - 0.291*G + 0.439*B  + 128;
    *Cr = 0.439*R - 0.368*G - 0.071*B + 128;
}


void IPU_MARKER::ARGBtoAYCbCr(unsigned int ARGB, unsigned int* AYCbCr)
{
    unsigned char Y = 0, Cb = 0, Cr = 0;

    rgbToyuv((unsigned char)((ARGB >> 16) & 0xff),(unsigned char)((ARGB >> 8) & 0xff),(unsigned char)(ARGB & 0xff),&Y, &Cb, &Cr);
    *AYCbCr = ((ARGB >> 24)&0xff) << 24 | Y << 16 | Cb << 8 | Cr;
    LOGE("Y = 0x%x Cb = 0x%x Cr = 0x%x AYCbCr = 0x%x\n", Y, Cb, Cr, *AYCbCr);
}


/* 将bmp图片的内容转换为nv12 sp格式的数据填充到fr buffer中 */
int IPU_MARKER::marker_set_bmp(unsigned char *src, int bmp_size)
{
    long width = 0;                       // The Width of the Data Part
    long height = 0;                      // The Height of the Data Part
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    int stride;
    struct YUV data;
    Buffer dest;
    char *buf_dest = NULL;
    long uv_offset = 0;
    Pixel::Format pfmt;
    size_t size;
    int i,j;

    Port *p = GetPort("out");
    if(NULL == p) {
        return -1;
    }

    width = p->GetWidth();
    height = p->GetHeight();
retry:
    try {
        dest = p->GetBuffer();
    } catch (const char* err) {
        usleep(20000);
        goto retry;
    }
    buf_dest = (char*)dest.fr_buf.virt_addr;
    uv_offset = width * height;
    pfmt = p->GetPixelFormat();
    stride = width * 4;
    int data_size = (p->GetWidth() * p->GetHeight() * 3) >> 1;

    if(1 == path_mode) {    //use softlayer
        switch(pfmt) {
            case Pixel::Format::RGBA8888:
                size = (p->GetWidth() * p->GetHeight() * 5);
                if(NULL != data_buf) {
                    for(j = 0; j < height; j++) {
                           memcpy(data_buf + j * stride, src + j * stride, stride);
                    }
                    memset(data_buf + data_size, 0x0, size - data_size);
                }
                break;
            case Pixel::Format::NV12:
                size = (p->GetWidth() * p->GetHeight() * 5) >> 1;
                if(NULL != data_buf) {
                    for(j = 0; j < height; j++) {
                           for(i = 0; i < width; i++) {
                            r = *(src + j * stride + i * 4 + 2);
                            g = *(src + j * stride + i * 4 + 1);
                            b = *(src + j * stride + i * 4 + 0);
                            data = rgbToyuv(r, g, b);
                            int k = height - j;
                            *(data_buf + k * width + i) = data.y;

                            if((1 == (i & 0x01 )) && (1 == (k & 0x01))) {
                                *(data_buf + uv_offset + ((k >> 1) * width) + 2 * (i >> 1)) = data.u;
                                *(data_buf + uv_offset + ((k >> 1) * width) + 2 * (i >> 1) + 1) = data.v;
                            }
                        }
                    }
                    memset(data_buf + data_size, 0x0, size - data_size);
                }
                break;
            default:
                return 0;
        }
    } else {     //use isppost or isppost2
        switch(pfmt) {
            case Pixel::Format::RGBA8888:
                for(j = 0; j < height; j++) {
                       memcpy(buf_dest + j * stride, src + j * stride, stride);
                }
                break;
            case Pixel::Format::NV12:
                for(j = 0; j < height; j++) {
                       for(i = 0; i < width; i++) {
                        r = *(src + j * stride + i * 4 + 2);
                        g = *(src + j * stride + i * 4 + 1);
                        b = *(src + j * stride + i * 4 + 0);

                        data = rgbToyuv(r, g, b);
                        int k = height - j;
                        *(buf_dest + k * width + i) = data.y;

                        if((1 == (i & 0x01 )) && (1 == (k & 0x01))) {
                            *(buf_dest + uv_offset + ((k >> 1) * width) + 2 * (i >> 1)) = data.u;
                            *(buf_dest + uv_offset + ((k >> 1) * width) + 2 * (i >> 1) + 1) = data.v;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    if(1 == path_mode) {
        dest.fr_buf.priv = (int)data_buf;
    }

    p->PutBuffer(&dest);
    return 0;
}
