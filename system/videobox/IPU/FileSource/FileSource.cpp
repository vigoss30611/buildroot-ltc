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
#include "FileSource.h"

DYNAMIC_IPU(IPU_FileSource, "filesource");

IPU_FileSource::IPU_FileSource(std::string name, Json *js) {
    Name = name;

    if(js && js->GetString("data_path")) {
        DataPath = js->GetString("data_path");
        FrameSize = js->GetInt("frame_size");
        if(!FrameSize) throw VBERR_BADPARAM;

        Mode = js->GetString("mode")? js->GetString("mode"):
            "reload";
    }    else {
        Mode = "idle";
    }

    Count = 0;
    Port *p = CreatePort("out", Port::Out);
    p->SetBufferType(FRBuffer::Type::FLOAT, 0x200000, 0x70000);
    p->SetPixelFormat(Pixel::Format::NotPixel);
}

void IPU_FileSource::Prepare()
{
    if(Mode == "idle") return;
    fd = open(DataPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd < 0) {
        LOGE("failed to created file %s\n", DataPath.c_str());
        throw VBERR_BADPARAM;
    }
}

void IPU_FileSource::Unprepare()
{
    Port *pIpuPort = GetPort("out");

    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if(Mode == "idle") return;
    close(fd);
}

void IPU_FileSource::WorkLoop()
{
    Port *p = GetPort("out");
    Buffer buf;
    int ret;

    if(Mode == "idle") return;
    while(IsInState(ST::Running)) {
        try {
            buf = p->GetBuffer();
retry:
            ret = read(fd, buf.fr_buf.virt_addr, FrameSize);
            LOGE("%3d.%dKB << %s, %d\n", buf.fr_buf.size >> 10,
                    (buf.fr_buf.size & 0x3ff) * 10 / 0x400,
                    DataPath.c_str(), Count++);
            if(ret < FrameSize) {
                lseek(fd, 0, SEEK_SET);
                goto retry;
            }

            p->PutBuffer(&buf);
        }
        catch (const char* error) {
            usleep(10000);
        }
    }
}

int IPU_FileSource::UnitControl(std::string func, void *arg)
{
    return 0;
}
