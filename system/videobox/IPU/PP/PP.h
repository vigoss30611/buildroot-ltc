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

#ifndef IPU_PP_H
#define IPU_PP_H
#include <IPU.h>
#include <ppapi.h>

typedef enum{
    STPRM_OLMASK0 = 1,
    STPRM_OLMASK1,
    STPRM_FRMBUF
}VDEC_ENUM_PP_MASKSET_TYPE_T;

class IPU_PP : public IPU {

private:

public:
    int Rotate;
    PPInst pPpInst;
    PPConfig pPpConf;
    static int InstanceCnt;

    IPU_PP(std::string, Json*);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    bool CheckPPAttribute();
    bool IsRotateEnabled();
    int GetRotateValue();
    void SetOutputParam(Port *pt, int type, int enc);
    int SetPipInfo(struct v_pip_info *vpinfo);
    int GetPipInfo(struct v_pip_info *vpinfo);
    int UnitControl(std::string func, void *arg);
};


#endif // IPU_PP_H
