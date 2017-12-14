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
#include "DGFrame.h"

DYNAMIC_IPU(IPU_DGFrame, "dg-frame");

IPU_DGFrame::IPU_DGFrame(std::string name, Json *js) {
    Name = name;
    Mode = IPU_DGFrame::PALETTE;
    Frames = 6;

    if(NULL != js){
        Mode = IPU_DGFrame::PALETTE;
        (Frames = js->GetInt("frames")) ? : (Frames = js->GetInt("nbuffers"));
        if(0 == Frames)
            Frames = 6;

        if(NULL != js->GetString("mode")){
            if(strncmp(js->GetString("mode"), "vacant", 6) == 0){
                Mode = IPU_DGFrame::VACANT;
                (Frames = js->GetInt("frames")) ? : (Frames = js->GetInt("nbuffers"));
                if(0 == Frames)
                    Frames = 8;
            }else if(strncmp(js->GetString("mode"), "picture", 7) == 0){
                Mode = IPU_DGFrame::PICTRUE;
                if(js->GetString("data")){
                    Path = js->GetString("data");
                }else{
                    LOGE("DGFrame no image path specified\n");
                    throw VBERR_BADPARAM;
                }
                (Frames = js->GetInt("frames")) ? : (Frames = js->GetInt("nbuffers"));
                if(0 == Frames)
                    Frames = 2;
            }else if(strncmp(js->GetString("mode"), "yuvfile", 7) == 0){
                Mode = IPU_DGFrame::YUVFILE;
                if(js->GetString("data")){
                    Path = js->GetString("data");
                }else{
                    LOGE("DGFrame no image path specified\n");
                    throw VBERR_BADPARAM;
                }
                (Frames = js->GetInt("frames")) ? : (Frames = js->GetInt("nbuffers"));
                if(0 == Frames)
                    Frames = 2;
            }
        }
    }

    Port *p = CreatePort("out", Port::Out);
    p->SetBufferType(FRBuffer::Type::FIXED, Frames);
    p->SetResolution(1920, 1080);
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

}

int inline IPU_DGFrame::Palette(Port *p, int i, int f_index) {
    i += f_index * 96;
    return ((i % p->GetWidth()) / 96 + ((i / p->GetWidth()
            / 135) & 1) * 3) % 6;
}

int IPU_DGFrame::Fill_Buffer(char *buf, int index)
{
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

void IPU_DGFrame::Prepare()
{
    int ret, i;
    Port *p = GetPort("out");

    fp = NULL;
    Delay = 1000000 / p->GetFPS();

    if(Mode == IPU_DGFrame::PICTRUE || Mode == IPU_DGFrame::YUVFILE){
        fp = fopen(Path.c_str(), "r");
        if(fp==NULL){
            LOGE("DGFrame path fopen failed\n");
            throw "fopen failed";
        }
        fseek(fp, 0L, SEEK_END);
        file_size = ftell(fp);
        rewind(fp);
    }
    LOGE("total frames ->%3d %3d %4d\n", p->GetFPS(), Frames, Delay);
    for(i=0; i<Frames; i++){
        Buffer buf = p->GetBuffer();
        if(Mode == IPU_DGFrame::PICTRUE){
            fseek(fp, 0L, SEEK_SET);
            ret = fread(buf.fr_buf.virt_addr, buf.fr_buf.size, 1, fp);
            LOGE("%3d len:%5d, ret:%d %5ld\n", i+1, buf.fr_buf.size, ret, ftell(fp));
            if(ret < 0){
                LOGE("DGFrame path fread failed\n");
                throw "fread failed";
            }
        }else if(Mode == IPU_DGFrame::PALETTE){
            LOGE("paletting frame %d, %p(v), %08x(p)...", i,
                buf.fr_buf.virt_addr, buf.fr_buf.phys_addr);
            Fill_Buffer((char *)buf.fr_buf.virt_addr, i);
            LOGE("done\n");
        }else if(Mode == IPU_DGFrame::VACANT){
            memset(buf.fr_buf.virt_addr, 0, buf.fr_buf.size);
        }
        p->PutBuffer(&buf);
    }

    if(Mode == IPU_DGFrame::PICTRUE)
        fclose(fp);
}

void IPU_DGFrame::Unprepare()
{
    Port *pIpuPort = GetPort("out");
    if(pIpuPort->IsEnabled())
    {
        pIpuPort->FreeVirtualResource();
    }
    if((Mode == IPU_DGFrame::PICTRUE || Mode == IPU_DGFrame::YUVFILE) && fp)
        fclose(fp);
}

void IPU_DGFrame::WorkLoop()
{
    Buffer buf;
    Port *p = GetPort("out");
    int ret;

    while(IsInState(ST::Running)) {
        buf = p->GetBuffer();
        if(Mode == IPU_DGFrame::YUVFILE) {
            ret = fread(buf.fr_buf.virt_addr, buf.fr_buf.size, 1, fp);
            if(ftell(fp) >= file_size) {
                rewind(fp);
            }
            if(ret < 0){
                LOGE("DGFrame path fread failed\n");
                throw "fread failed";
            }
        }
        usleep(Delay);
        buf.Stamp();
        p->PutBuffer(&buf);
    }
}

int IPU_DGFrame::UnitControl(std::string func, void *arg)
{
    return 0;
}
