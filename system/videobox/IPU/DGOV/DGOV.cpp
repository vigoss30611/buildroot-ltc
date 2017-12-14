// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
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

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include <System.h>
#include "DGOV.h"

DYNAMIC_IPU(IPU_DGOverlay, "dg-ov");

static unsigned long mystrtoul(char* str, int base)
{
   char *endptr;
   unsigned long val;

   errno = 0;    /* To distinguish success/failure after call */
   val = strtoul(str, &endptr, base);

   /* Check for various possible errors */
   if (endptr == str) {
       fprintf(stderr, "No digits were found\n");
       return 0;
   }

   /* If we got here, strtol() successfully parsed a number */

   LOGD("strtol() returned %ld\n", val);

   if (*endptr != '\0')        /* Not necessarily an error... */
       LOGE("Further characters after number: %s\n", endptr);

   return val;
}

static void RGB2YCbCr(unsigned char R, unsigned char G , unsigned char B,
        unsigned char *Y, unsigned char * Cb, unsigned char *Cr)
{
    *Y = 0.257*R  + 0.504*G  + 0.098*B + 16;
    *Cb = -0.148*R - 0.291*G + 0.439*B  + 128;
    *Cr = 0.439*R - 0.368*G - 0.071*B + 128;
}

static void ARGBtoAYCbCr(unsigned int ARGB, unsigned int* AYCbCr)
{
    unsigned char Y = 0, Cb = 0, Cr = 0;

    RGB2YCbCr((ARGB >> 16) & 0xff, (ARGB >> 8) & 0xff, ARGB & 0xff, &Y, &Cb, &Cr);
    *AYCbCr = ((ARGB >> 24)&0xff) << 24 | Y << 16 | Cb << 8 | Cr;
    LOGD("Y = 0x%x Cb = 0x%x Cr = 0x%x A = 0x%x\n", Y, Cb, Cr, *AYCbCr);
}

IPU_DGOverlay::IPU_DGOverlay(std::string name, Json *js) {
    Name = name;
    Mode = IPU_DGOverlay::PALETTE;
    Frames = 6;
    unsigned int A0 = 0, A1 = 0, A2 = 0, A3 = 0;

    if(NULL != js){
        if(NULL != js->GetString("mode")){
            if(strncmp(js->GetString("mode"), "vacant", 6) == 0){
                Mode = IPU_DGOverlay::VACANT;
                Frames = js->GetInt("frames");
                if(0 == Frames)
                    Frames = 8;
            }else if(strncmp(js->GetString("mode"), "picture", 7) == 0){
                Mode = IPU_DGOverlay::PICTRUE;
                if(js->GetString("data")){
                    Path = js->GetString("data");
                }else{
                    LOGE("DGFrame no image path specified\n");
                    throw VBERR_BADPARAM;
                }
                Frames = js->GetInt("frames");
                if(0 == Frames)
                    Frames = 2;
            }else{
                Mode = IPU_DGOverlay::PALETTE;
                Frames = js->GetInt("frames");
                if(0 == Frames)
                    Frames = 4;
            }
        }

    if(NULL != js->GetString("format")){
        if(strncmp(js->GetString("format"), "bpp2", 4) == 0){
            Format = IPU_DGOverlay::OV_BPP2;
        }else if(strncmp(js->GetString("format"), "nv12", 4) == 0){
            Format = IPU_DGOverlay::OV_NV12;
        } else {
            Format = IPU_DGOverlay::OV_BPP2;
            LOGE("pixel_format in json is not match, force BPP2\n");
        }
    }else {
        Format = IPU_DGOverlay::OV_BPP2;
    }

    if(NULL != js->GetString("colorbar-mode")){
        ColorbarMode= js->GetInt("colorbar-mode");
    } else
        ColorbarMode = 0;

    if(NULL != js->GetString("alpha0")){
        A0 = (unsigned int)mystrtoul((char*)js->GetString("alpha0"), 16);
        if (A0 == 0)
            A0 = 0x7fff0000; /*RED*/
    } else
        A0 = 0x7fff0000; /*RED*/

    if(NULL != js->GetString("alpha1")){
        A1 = (unsigned int)mystrtoul((char*)js->GetString("alpha1"), 16);
        if (A1 == 0)
            A1 = 0x7f00ff00; /*GREEN*/
    } else
        A1 = 0x7f00ff00; /*GREEN*/

    if(NULL != js->GetString("alpha2")){
        A2 = (unsigned int)mystrtoul((char*)js->GetString("alpha2"), 16);
        if (A2 == 0)
            A2 = 0x7f0000ff; /*BLUE*/
    } else
        A2 = 0x7f0000ff; /*BLUE*/

    if(NULL != js->GetString("alpha3")){
        A3 = (unsigned int)mystrtoul((char*)js->GetString("alpha3"), 16);
        if (A3 == 0)
            A3 = 0x7fffff00; /*YELLOW*/
    } else
        A3 = 0x7fffff00; /*YELLOW*/
    }

    Port *p = CreatePort("out", Port::Out);
    p->SetBufferType(FRBuffer::Type::FIXED, Frames);
    p->SetResolution(320, 320);

    if (Format == OV_BPP2)
        p->SetPixelFormat(Pixel::Format::BPP2);
    else
        p->SetPixelFormat(Pixel::Format::NV12);

    if(p->GetFPS() == 0){
        p->SetFPS(15);
    }
    p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);

    Color[0].y = 178;
    Color[0].u = 135;
    Color[0].v = 169;

    Color[1].y = 76;
    Color[1].u = 111;
    Color[1].v = 119;

    Color[2].y = 184;
    Color[2].u = 145;
    Color[2].v = 118;

    Color[3].y = 128;
    Color[3].u = 182;
    Color[3].v = 47;

    Color[4].y = 199;
    Color[4].u = 109;
    Color[4].v = 149;

    Color[5].y = 178;
    Color[5].u = 105;
    Color[5].v = 116;

    ARGBtoAYCbCr(A0, &ov_info.iPaletteTable[0]);
    ARGBtoAYCbCr(A1, &ov_info.iPaletteTable[1]);
    ARGBtoAYCbCr(A2, &ov_info.iPaletteTable[2]);
    ARGBtoAYCbCr(A3, &ov_info.iPaletteTable[3]);

    LOGE("A0 = 0x%x A1 = 0x%x A2 = 0x%x A3 = 0x%x",
            ov_info.iPaletteTable[0] , ov_info.iPaletteTable[1],
            ov_info.iPaletteTable[2], ov_info.iPaletteTable[3]);
    ov_info.width = 0;
    ov_info.height= 0;
}

int inline IPU_DGOverlay::Palette(Port *p, int i, int f_index) {
    i += f_index * 96;
    return ((i % p->GetWidth()) / 96 + ((i / p->GetWidth()
            / 135) & 1) * 3) % 6;
}

int IPU_DGOverlay::Fill_Buffer(char *buf, int index)
{
    LOGD("%s\n", __func__);
    Port *p = GetPort("out");
    int i, j, pixels = p->GetWidth() * p->GetHeight();
    char *y = buf;
    short *uv = (short *)(buf + pixels);

    for(i = 0; i < pixels; i++)
        y[i] = Color[Palette(p, i, index)].y;

    for(i = 0; i < pixels >> 2; i++) {
        j = 4 * i / p->GetWidth() * p->GetWidth()
            + i % (p->GetWidth() / 2) * 2;
        uv[i] = Color[Palette(p, j, index)].v;
        uv[i] <<= 8;
        uv[i] |= Color[Palette(p, j, index)].u;
    }

    return 0;
}

static void gen_bpp2_colorbar(int width, int height,
    char* data, char c1, int c2, int c3, int c4)
{
    int barwidth;
    int i=0,j=0;
    int barnum = 0;
    //Check
    if(width<=0||height<=0){
        LOGE("Error: Width, Height cannot be 0 or negative number!\n");
        LOGE("Default Param is used.\n");
        width=640;
        height=480;
    }
    if(width%4!=0) {
        LOGE("Warning: Width cannot be divided by Bar Number without remainder!\n");
        width -= width%4;
        LOGE("W = %d H= %d\n", width, height);
    }

    barwidth=width/4;
    LOGD("W = %d H= %d\n", width, height);
    for(j=0;j<height;j++){
        for(i=0;i<width;i++){
            barnum= i/barwidth;
            data[(j*width+i)] = 0;
            switch(barnum){
                case 0:{
                    data[(j*width+i)] = (c1 & 0x3) | (c1 & 0x3) << 2 | (c1 & 0x3) << 4 | (c1 & 0x3) << 6;
                    break;
                }

                case 1:{
                    data[(j*width+i)] = (c2 & 0x3) | (c2 & 0x3) << 2 | (c2 & 0x3) << 4 | (c2 & 0x3) << 6;
                    break;
                }

                case 2:{
                    data[(j*width+i)] = (c3 & 0x3) | (c3 & 0x3) << 2 | (c3 & 0x3) << 4 | (c3 & 0x3) << 6;
                    break;
                }

                case 3:{
                    data[(j*width+i)] = (c4 & 0x3) | (c4 & 0x3) << 2 | (c4 & 0x3) << 4 | (c4 & 0x3) << 6;
                    break;
                }
            }
            //printf("data[%d*%d +%d] = 0x%x\n", j, width, i, data[(j*width+i)]);
        }
    }
}

int IPU_DGOverlay::Fill_BPP2_Buffer(char *buf, int index)
{
    LOGD("%s\n", __func__);
    Port *p = GetPort("out");

    if (!ColorbarMode)
        gen_bpp2_colorbar(p->GetWidth()/4, p->GetHeight(), buf,0, 1, 2, 3);
    else
    {
        if (index%Frames == 0)
            gen_bpp2_colorbar(p->GetWidth()/4, p->GetHeight(), buf,0, 1, 2, 3);
        else if (index%Frames == 1)
            gen_bpp2_colorbar(p->GetWidth()/4, p->GetHeight(), buf,3, 0,1, 2);
        else if (index%Frames == 2)
            gen_bpp2_colorbar(p->GetWidth()/4, p->GetHeight(), buf,2, 3, 0, 1);
        else if (index%Frames == 3)
            gen_bpp2_colorbar(p->GetWidth()/4, p->GetHeight(), buf,1, 2, 3, 0);
        else
            gen_bpp2_colorbar(p->GetWidth()/4, p->GetHeight(), buf,0, 1, 2, 3);
    }
    return 0;
}

void IPU_DGOverlay::Prepare()
{
    int ret, i;
    Port *p = GetPort("out");
    FILE *fp = NULL;
    Pixel::Format enPixelFormat = GetPort("out")->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::BPP2)
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    Delay = 1000000 / p->GetFPS();

    if(Mode == IPU_DGOverlay::PICTRUE){
        fp = fopen(Path.c_str(), "r");
        if(fp==NULL){
            LOGE("DGFrame path fopen failed\n");
            throw "fopen failed";
        }
    }
    LOGE("total frames ->%3d %3d %4d\n", p->GetFPS(), Frames, Delay);

    // ov width and height must conform to the rules at blow
    ov_info.width = p->GetWidth() -p->GetWidth()%32;
    ov_info.height = p->GetHeight() - p->GetHeight()%32;

    if (ov_info.width < 160)
        ov_info.width = 160;

    if (ov_info.height < 32)
        ov_info.height = 32;

    LOGE("DGOV W=%d H=%d\n", ov_info.width, ov_info.height);
    if(p->GetDirection() == Port::Out && ov_info.width != p->GetWidth())
    {
        LOGE("DGOV Re-Allocate Buffer\n");
        p->FreeFRBuffer();
        p->SetResolution(ov_info.width,ov_info.height);
        p->AllocateFRBuffer();
        p->SyncBinding();
    }

    for(i=0; i<Frames; i++){
        Buffer buf = p->GetBuffer();
        buf.fr_buf.priv = ((int)&ov_info);
        LOGD("fr_buf.priv = 0x%x\n", buf.fr_buf.priv);
        if(Mode == IPU_DGOverlay::PICTRUE){
            //fseek(fp, 0L, SEEK_SET);
            ret = fread(buf.fr_buf.virt_addr, buf.fr_buf.size, 1, fp);
            LOGE("%3d len:%5d, ret:%d %5ld\n", i+1, buf.fr_buf.size, ret, ftell(fp));
            if(ret < 0){
                LOGE("DGFrame path fread failed\n");
                throw "fread failed";
            }
        }else if(Mode == IPU_DGOverlay::PALETTE){

            LOGE("paletting frame %d, %p(v), %08x(p)...", i,
                buf.fr_buf.virt_addr, buf.fr_buf.phys_addr);
            if (Format == OV_NV12)
                Fill_Buffer((char *)buf.fr_buf.virt_addr, i);
            else
                Fill_BPP2_Buffer((char *)buf.fr_buf.virt_addr, i);
            LOGE("done\n");

        }else if(Mode == IPU_DGOverlay::VACANT){
            memset(buf.fr_buf.virt_addr, 0, buf.fr_buf.size);
        }
        p->PutBuffer(&buf);
    }

    if(Mode == IPU_DGOverlay::PICTRUE)
        fclose(fp);
}

void IPU_DGOverlay::Unprepare()
{
    Port *pIpuPort;

    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

}

void IPU_DGOverlay::WorkLoop()
{
    Buffer buf;
    Port *p = GetPort("out");

    while(IsInState(ST::Running)) {
        buf = p->GetBuffer();

        usleep(Delay);
        buf.Stamp();
        p->PutBuffer(&buf);
    }
}

int IPU_DGOverlay::UnitControl(std::string func, void *arg)
{
    return 0;
}
