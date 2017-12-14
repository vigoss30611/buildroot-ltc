// Copyright (C) 2016 InfoTM, devin.zhu@infotm.com
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

#ifndef IPU_G1264_H
#define IPU_G1264_H
#include <IPU.h>
#include <pthread.h>
#include <semaphore.h>
#include <hlibg1v6/h264decapi.h>

class IPU_G1264:public IPU {

    private:
        static int G1264InstanceCount;
        FRBuffer* decBuf;

    public:
        bool Headerflag;
        char *StreamHeader;
        int  StreamHeaderLen;
        long PicDecodeNum;

        H264DecInst pG1264DecInst;

        

        H264DecInput        G1264DecInput;
        H264DecOutput       G1264DecOutput;
        H264DecInfo         G1264DecInfo;
        H264DecPicture      G1264DecPicture;

        IPU_G1264(std::string, Json *);
        ~IPU_G1264();
        void Prepare();
        void Unprepare();
        void WorkLoop();
        bool SetStreamHeader(struct v_header_info *pheader);
        int  UnitControl(std::string, void *);


};
#endif
