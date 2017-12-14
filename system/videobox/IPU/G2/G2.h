// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
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

#ifndef IPU_G2_H
#define IPU_G2_H
#include <IPU.h>
#include <pthread.h>
#include <semaphore.h>
#include <hlibg2v1/hevc_container.h>
#include <hlibg2v1/hevcdecapi.h>

class IPU_G2 : public IPU {

private:
    static int G2InstanceCnt; //object instance counter
    FRBuffer* decBuf;

public:
    bool Headerflag;
    char *StreamHeader;
    const void *dwl_inst;
    long PicDecodeNum;
    int StreamHeaderLen;
    bool OutputThreadRunFlag;
    pthread_t G2OutputThread;
    HevcDecInst pG2DecInst;
    struct HevcDecInfo pG2DecInfo;
    struct HevcDecInput pG2DecInput;
    struct HevcDecOutput pG2DecOutput;
    struct HevcDecConfig pG2DecCfg;
    struct HevcDecPicture pG2DecPicture;
    IPU_G2(std::string, Json*);
    ~IPU_G2();
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void *);
    bool SetStreamHeader(struct v_header_info *strhdr);
};

#endif

