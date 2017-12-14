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

#include <System.h>
#include "DGPixel.h"

DYNAMIC_IPU(IPU_DGPixel, "dg-pixel");

void IPU_DGPixel::BlackScreen(char *scr, Port *p)
{
    int loc = p->GetWidth() * p->GetHeight();
    memset(scr, 16, loc);
    memset(scr + loc    , 128, p->GetWidth() * p->GetHeight() / 2);
}

void IPU_DGPixel::DrawFont(char *scr, char c,
                                                            Port *p, Pixel color)
{
    const char *b = &ft->data[(int)c * ft->height];
    int i, j, l, loc, x, y;
    int w = p->GetWidth(), h = p->GetHeight();

//    LOGE("drawing: %p, [%c] (%d, %d) ", scr, c, loc_x, loc_y);
//    return ;

    for(i = 0; i < ft->height; i++) {
        for(l = b[i], j = 7; l; l >>= 1, j--)
        {
            if(l & 1) {
                x = loc_x * ft->width + j;
                y = loc_y * ft->height + i;

//                LOGE("draw pix @(%d, %d)\n", x, y);
                // draw pixel
                loc = x + y * w;
//                LOGE("draw y @%p\n", *(scr + loc));
// LOGE("-->y:(%d, %d)", x, y);
                *(scr + loc) = color.y;
//                if (loc_y == 77) LOGE("^ ");
                if(!((y | x) & 1)) {
                    if (loc_y == 77) LOGD("draw uv @%x <= %x\n", w    * h
                        + y * w / 2 + x, ((short)color.v << 8) | color.u);
                    *(short *)(scr + w    * h    + ((y * w / 2 + x) & ~1))
                         = ((short)color.v << 8) | color.u;

                }
            }
        }
    }
//    LOGE("OK\n");
}

IPU_DGPixel::IPU_DGPixel(std::string name, Json *js) {
    Name = name;

    if(js) Path = js->GetString("path");
    else {
        LOGE("txt file path is not provided for dgp\n");
        throw VBERR_WRONGDESC;
    }

    Port *p = CreatePort("out", Port::Out);
    p->SetBufferType(FRBuffer::Type::FIXED, 2);
    p->SetResolution(1920, 1080);
    p->SetPixelFormat(Pixel::Format::NV12);
    p->SetFPS(15);
    p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);

    loc_x = loc_y = 0;
    ft = &font_7x14;

    Color[0].SetYUV(178, 135, 169);
    Color[1].SetYUV(128, 182, 47);
    Color[2].SetYUV(199, 109, 149);
}

void IPU_DGPixel::Prepare()
{
    Port *p = GetPort("out");
    Buffer buf;
    Pixel::Format enPixelFormat = p->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21)
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    Delay = 1000000 / p->GetFPS();

    fd = open(Path.c_str(), O_RDONLY);
    if(fd < 0) {
        LOGE("Unable to open: %s\n", Path.c_str());
        throw VBERR_BADPARAM;
    }

    for(int i = 0; i < p->GetBufferCount(); i++) {
            buf = p->GetBuffer();
            BlackScreen((char *)buf.fr_buf.virt_addr, p);
            p->PutBuffer(&buf);
        }
}

void IPU_DGPixel::Unprepare()
{
    Port *pIpuPort;

    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

    close(fd);
}

void IPU_DGPixel::WorkLoop()
{
#define ONCE_MAX 20
    Buffer buf;
    Port *p = GetPort("out");
    char cs[4096];
    int count = 0, c1 = 0, c2 = 0, xslots = p->GetWidth()
            / ft->width - 1, yslots = p->GetHeight()
            / ft->height - 1;
    static int color_index = 0;

    while(IsInState(ST::Running)) {

        if(c2 >= count) {
begin:
            count = read(fd, cs, 4096);
            if(count <= 0) {
                lseek(fd, 0, SEEK_SET);
                goto begin;
            }

            c1 = c2 = 0;
//            LOGE("%d bytes read from %s", count, Path.c_str());
        }

        buf = p->GetBuffer();
        for(c2 = c1; c2 - c1 < ONCE_MAX && c2 < count; c2++) {

//                LOGE("char::%d\n", cs[c2]);
                if(cs[c2] > 127) {
                    if(cs[c2] == '\t') cs[c2] = ' ';
                    else {
    //                    LOGE("ignore >127\n");
                        continue;
                    }
                }

                if(cs[c2] == '\n') {
//                    LOGE("newline\n");
                    loc_y++;
                    continue;
                }

                if(cs[c2] == ' ') {
    //                LOGE("space\n");
                    color_index = (color_index + 1) % 4;
                    loc_x++;
                    continue;
                }

                if(loc_x > xslots) {
    //                LOGE("newline_x\n");
                    loc_y ++;
                    loc_x = 0;
                }

                if(loc_y > yslots) {
//                        LOGE("y>, black screen\n");
                        BlackScreen((char *)buf.fr_buf.virt_addr, p);
                        loc_x = 0;
                        loc_y = 0;
                }

                DrawFont((char *)buf.fr_buf.virt_addr,
                                        cs[c2], p, Color[color_index]);

                loc_x++;
        }

        c1 = c2 + 1;

        usleep(Delay);
        buf.Stamp();
    //    WriteFile(buf.fr_buf.virt_addr, buf.fr_buf.size, "/nfs/yuv.%d", c1);
        p->PutBuffer(&buf);
    }
}

int IPU_DGPixel::UnitControl(std::string func, void *arg)
{
    return 0;
}
