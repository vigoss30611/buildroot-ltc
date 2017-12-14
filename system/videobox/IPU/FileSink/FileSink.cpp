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
#include "FileSink.h"

DYNAMIC_IPU(IPU_FileSink, "filesink");

IPU_FileSink::IPU_FileSink(std::string name, Json *js) {
    Name = name;

    if(js) Path = js->GetString("data_path");
    else {
        LOGE("file path is not provided for filesink\n");
        throw VBERR_WRONGDESC;
    }

    s32MaxSaveCnt = js->GetInt("max_save_cnt");

    Count = 0;
    Port *p = CreatePort("in", Port::In);
    p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_StopNeeded);
}

void IPU_FileSink::Prepare()
{
    fd = open(Path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd < 0) {
        LOGE("failed to created file %s\n", Path.c_str());
        //throw VBERR_BADPARAM;
    }
}

void IPU_FileSink::Unprepare()
{
    Port *pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    if (fd >= 0)
        close(fd);
}

void IPU_FileSink::WorkLoop()
{
    Port *p = GetPort("in");
    Buffer buf;
    int mode = s32MaxSaveCnt;

    while(IsInState(ST::Running)) {
        try {
            buf = p->GetBuffer(&buf);
            if(buf.fr_buf.priv == VIDEO_FRAME_DUMMY)
            {
                LOGE("resolution change\n");
                p->PutBuffer(&buf);
                continue;
            }
            if (fd >= 0) {
                if (!mode) {
                    LOGD("%3d.%dKB >> %s, %d\n", buf.fr_buf.size >> 10,
                        (buf.fr_buf.size & 0x3ff) * 10 / 0x400,
                    Path.c_str(), Count++);
                    write(fd, buf.fr_buf.virt_addr, buf.fr_buf.size);
                } else {
                    if (s32MaxSaveCnt > 0) {
                        LOGE("# %3d.%dKB >> %s, %d\n", buf.fr_buf.size >> 10,
                            (buf.fr_buf.size & 0x3ff) * 10 / 0x400,
                        Path.c_str(), Count++);
                        write(fd, buf.fr_buf.virt_addr, buf.fr_buf.size);
                        s32MaxSaveCnt --;
                    }
                }
            }
            p->PutBuffer(&buf);
        }
        catch (const char* error) {
            usleep(10000);
        }
    }
}

int IPU_FileSink::UnitControl(std::string func, void *arg)
{
    return 0;
}
