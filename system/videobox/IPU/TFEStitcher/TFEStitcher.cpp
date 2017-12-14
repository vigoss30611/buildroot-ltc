// Copyright (C) 2016 InfoTM, XXX
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
#include "TFEStitcher.h"

DYNAMIC_IPU(IPU_TFEStitcher, "tfestitcher");

void IPU_TFEStitcher::WorkLoop()
{
    Port *in0 = GetPort("in0"), *in1 = GetPort("in1"),
        *out = GetPort("out");
    Buffer b0, b1, buf;

    while(IsInState(ST::Running)) {
        try {
            b0 = in0->GetBuffer(&b0);
            b1 = in1->GetBuffer(&b1);
            buf = out->GetBuffer();

            // Stitcher pictures
            Stitch((char *)b0.fr_buf.virt_addr,
                (char *)b1.fr_buf.virt_addr,
                (char *)buf.fr_buf.virt_addr,
                in0->GetWidth(), in0->GetHeight());

            in0->PutBuffer(&b0);
            in1->PutBuffer(&b1);
            out->PutBuffer(&buf);
        } catch (const char* err) {
            usleep(200000);
        }
    }
}

IPU_TFEStitcher::IPU_TFEStitcher(std::string name, Json *js) {
    Name = name;

    CreatePort("in0", Port::In);
    CreatePort("in1", Port::In);
    Port *out = CreatePort("out", Port::Out);
    out->SetBufferType(FRBuffer::Type::FIXED, 3);
    out->SetPixelFormat(Pixel::Format::NV12);
    out->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Forbidden);

}

void IPU_TFEStitcher::Prepare()
{
    Port *in0 = GetPort("in0"), *in1 = GetPort("in1"),
        *out = GetPort("out");

    if(in0->GetWidth() != in1->GetWidth()
    || in0->GetHeight() != in1->GetHeight()
    || (in0->IsEnabled() && in1->IsEnabled() && in0->GetPixelFormat() != in1->GetPixelFormat())
    || (out->GetWidth() != in0->GetWidth() + in1->GetWidth())
    || (out->GetHeight() != in0->GetHeight())) {
        LOGE("different picture size or format\n");
        throw VBERR_BADPARAM;
    }
}

void IPU_TFEStitcher::Unprepare()
{
    Port *pIpuPort = NULL;
    char buf[5] = {0};

    pIpuPort = GetPort("out");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    for(int i = 0; i < 2; i++)
    {
        sprintf(buf, "in%d", i);
        pIpuPort = GetPort(buf);
        if (pIpuPort && pIpuPort->IsEnabled())
        {
             pIpuPort->FreeVirtualResource();
        }
    }

}

int IPU_TFEStitcher::UnitControl(std::string func, void *arg)
{
    return 0;
}

void IPU_TFEStitcher::Stitch(char *b0, char *b1, char *out, int w, int h)
{
    // FIXME with the correct algorithm
    for(h = h * 3 / 2; h--; b0 += w, b1 += w) {
        memcpy(out, b0, w); out += w;
        memcpy(out, b1, w);    out += w;
    }
}
