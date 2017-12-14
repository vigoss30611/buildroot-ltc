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
#include "DGMath.h"

DYNAMIC_IPU(IPU_DGMath, "dg-math");

IPU_DGMath::IPU_DGMath(std::string name, Json *js) {
    Name = name;

    if(js) {
        std::string drawing = js->GetString("drawing");
        if(!drawing.length()) throw VBERR_BADPARAM;
        D = GetDrawingByName(drawing);
    } else {
        srand((int)time(NULL));
        int n = rand() % GetDrawingCount();
        D = GetDrawingByID(n);
    }

    Port *p = CreatePort("out", Port::Out);
    p->SetBufferType(FRBuffer::Type::VACANT, 3);
    p->SetResolution(1920, 1080);
    p->SetPixelFormat(Pixel::Format::RGBA8888);
    p->SetFPS(60);
    p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
}

void IPU_DGMath::Prepare()
{
    int i, j, w, h, size;
    Port *p = GetPort("out");
    w = p->GetWidth();
    h = p->GetHeight();
    Delay = 1000000 / p->GetFPS();

    if(p->GetPixelFormat() != Pixel::Format::RGBA8888) {
        LOGE("only RGBA8888 is supported");
        throw VBERR_BADPARAM;
    }

    size = w * h * Pixel::Bits(p->GetPixelFormat())    / 8 * 2;
//    LOGE("dgm intbuffer size: 0x%x\n", w * h * p->Pixel::Bits()
//                / 8 * 2);
    // two buffers
    Canvas = new FRBuffer(Name.c_str(), FRBuffer::Type::FIXED, 1, size);

    Buffer buf = Canvas->GetBuffer();

    LOGE("drawing %s ...", D->Name);
    memset(buf.fr_buf.virt_addr, 0, size);
    for(j = 0; j < 2 * h; j++)
        for(i = 0; i < w; i++) {
//            LOGE("virt: %p, w: %d, i: %d j: %d\n",
//            (char *)buf.fr_buf.virt_addr,
//            w, i, j);

            D->Draw((char *)buf.fr_buf.virt_addr,
            w, h, i, j);
//            Log_debug();

        }
    LOGE("done\n");

    Canvas->PutBuffer(&buf);
}

void IPU_DGMath::Unprepare()
{
    Port *pIpuPort;

    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if(Canvas)
    {
        delete Canvas;
        Canvas = NULL;
    }
}

void IPU_DGMath::WorkLoop()
{
    Buffer buf, cvs;
    Port *p = GetPort("out");

    TimePin tp;
    while(IsInState(ST::Running)) {
        buf = p->GetBuffer();
        usleep(Delay);

        cvs = Canvas->GetBuffer();
        buf.fr_buf.phys_addr = cvs.fr_buf.phys_addr
                            + D->GetOffset(tp.Peek(), p->GetHeight())
                            * p->GetWidth() * Pixel::Bits(p->GetPixelFormat()) / 8;
        Canvas->PutBuffer(&cvs);
        buf.Stamp();
        p->PutBuffer(&buf);
    }
}

int IPU_DGMath::UnitControl(std::string func, void *arg)
{
    return 0;
}

#include "palette.cpp"
#include "water.cpp"
#include "cloth.cpp"
struct Drawing Drawings[] = {
    {
        .Name = "palette",
        .GetOffset = palette_get_offset,
        .Draw = palette_draw,
    }, {
            .Name = "cloth",
            .GetOffset = cloth_get_offset,
            .Draw = cloth_draw,
    }, {
            .Name = "water",
            .GetOffset = water_get_offset,
            .Draw = water_draw,
    },
};

int IPU_DGMath::GetDrawingCount()
{
    return 3;
}

Drawing *
IPU_DGMath::GetDrawingByID(int n)
{
    if (n < GetDrawingCount())
        return &Drawings[n];
    throw "get drawing id exceed limit";
}

Drawing *
IPU_DGMath::GetDrawingByName(std::string name)
{
    for(int i = 0; i < GetDrawingCount(); i++) {
        LOGD("checking dgm %s->%s\n", Drawings[i].Name, name.c_str());
        if(strcmp(Drawings[i].Name, name.c_str()) == 0)
            return &Drawings[i];
        }
    throw "get drawing w/ invalid name" + name;
}
