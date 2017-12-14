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
#include "BadEncoder.h"

DYNAMIC_IPU(IPU_BadEncoder, "badencoder");

void IPU_BadEncoder::WorkLoop()
{
    Buffer bfin, bfout;
    Port *pin, *pout;
    int ret;

    pin = GetPort("in");
    pout = GetPort("out");

    while(IsInState(ST::Running)) {
        try{
            bfin = pin->GetBuffer(&bfin);
        }catch(const char* err){
            usleep(20000);
            continue;
        }

        bfout = pout->GetBuffer();
        ret = Encode((char *)bfin.fr_buf.virt_addr, (char *)bfout.fr_buf.virt_addr);
        if(ret < 0) {
            LOGE("encode failed\n");
            throw VBERR_RUNTIME;
        }

//        WriteFile(bfin.fr_buf.virt_addr, bfin.fr_buf.size, "/tmp/d.yuv");
        WriteFile(bfout.fr_buf.virt_addr, ret, "/tmp/d.enc");
        LOGE("%3d.%dKB >> %s\n", ret >> 10,
            (ret & 0x3ff) * 10 / 0x400,    "/tmp/d.enc");

        bfout.fr_buf.size = ret;
        bfout.SyncStamp(&bfin);
        pout->PutBuffer(&bfout);
        pin->PutBuffer(&bfin);
    }
}

IPU_BadEncoder::IPU_BadEncoder(std::string name, Json *js)
{
    Name = name;
    Port *pout, *pin;

    pin = CreatePort("in", Port::In);
    pin->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);

    pout = CreatePort("out", Port::Out);
    pout->SetBufferType(FRBuffer::Type::FLOAT, 0x1400000, 0x100000);
    pout->SetPixelFormat(Pixel::Format::H265ES);
    pout->Export();

    Header = (char *)malloc(256);
    HeaderLength = 0;
}

void IPU_BadEncoder::Prepare()
{
    Pixel::Format enPixelFormat = GetPort("in")->GetPixelFormat();
    if ((enPixelFormat != Pixel::Format::NV12) && (enPixelFormat != Pixel::Format::NV21)
        && (enPixelFormat != Pixel::Format::RGBA8888))
    {
        LOGE("In Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    if ((GetPort("out")->GetPixelFormat() != Pixel::Format::H265ES))
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }

    GenHeader();
}

void IPU_BadEncoder::Unprepare()
{
    Port *pIpuPort;

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

void IPU_BadEncoder::GenHeader()
{
    // TODO:
    // 1. fill header info into Header
    // 2. set HeaderLength
}

int IPU_BadEncoder::Encode(char *in, char *out)
{
    // TODO:
    // 1. encode
    // 2. return frame length

    return 0x3491;
}

int IPU_BadEncoder::UnitControl(std::string func, void *arg) {

    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(video_get_header) {
        if(HeaderLength <= VIDEO_HEADER_MAXLEN) {
            memcpy(arg, Header, HeaderLength);
            LOGD("copy %d bytes header to RPC call\n", HeaderLength);
            return HeaderLength;
        }
        return -2;
    }

    LOGE("%s is not support\n", func.c_str());
    return -2;
}
