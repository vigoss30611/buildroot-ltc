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

#ifndef IPU_H
#define IPU_H

#include <thread>
#include <mutex>
#include <Port.h>
#include <Json.h>
#include <Path.h>

#include <qsdk/videobox.h>


#define IPU_THREAD_PRIORITY 90
#define ABS_DIFF(x,y) (((x) > (y)) ? ((x) - (y)) : ((y) - (x)))

// --------------------Code need refine, structure used by ISP & ISPost -----------
enum SHOT_STATE {
    SHOT_INITED = 0,
    SHOT_POSTED,
    SHOT_RETURNED,
};

typedef struct ipu_v2500_private_data {
    // Do NOT define any args before iDnID
    int s32EnEsc;
    int iDnID;
    int iBufID;
    SHOT_STATE state;
    cam_3d_dns_attr_t dns_3d_attr;
    enum cam_mirror_state mirror;
}ipu_v2500_private_data_t;

// ---------------------------------------------------------------------------------

class Path;
struct DynamicIPU;

class IPU {
public:

    enum ST {
        NEW = 0,
        INITED,
        Prepared,
        StartPending,
        Running,
        StopPending,
        Error,
        COUNT,
    };

    static int Enhance;
    static std::vector<struct DynamicIPU *> DIPUs;
    static int SetPriority(int enhancePriority);
    static IPU * DynamicCreate(std::string name, std::string ipu, Json *js);

private:
    volatile int State;
    std::mutex StateMTX;
    std::thread WorkThread;
    std::vector<std::string *> UCF;
    Path *Owner;
    int s32Type;

public:
    std::string Name;
    std::vector<Port*> InPorts, OutPorts;
    EN_IPU_TYPE m_enIPUType;

    IPU();
    virtual ~IPU();

    void SetOwner(Path *);
    int RegisterUCFunction(std::string ucf);
    bool HasUCFunction(std::string ucf);

    bool IsInState(int st);
    void SetState(int state);

    Port* CreatePort(std::string, int);
    Port* GetPort(std::string);
    Port* GetMainInPort();
    void DestroyPort(std::string);
    void InitPorts();
    void DeinitPorts();
    int SetFeatures(std::string);

    virtual void Prepare();
    virtual void Unprepare();
    virtual int Start();
    virtual int Stop();
    virtual void WorkLoop();
    virtual int UnitControl(std::string, void*);
    virtual int GetDebugInfo(void *pInfo, int *ps32Size);
    Path*  GetOwner();

    // On = Prepare + Start
    // Off = Unprepare + Stop
    void RequestOn(int force);
    void RequestOff(int force);
    void ReportError(std::string err);

    // IPUs will use LOGE in class, not the system one
    void LOGE(const char *fmt, ...);
};

struct DynamicIPU {
    std::string Name;
    IPU* (*CreateIPU)(std::string name, Json *para_js);
};

#define DYNAMIC_IPU(u, n)  \
static IPU *Create_D##u(std::string name, Json *para_js) {  \
    IPU *ipu = new u(name, para_js);  \
    return ipu;  \
}  \
struct DynamicIPU D##u {  \
    .Name = n, .CreateIPU = Create_D##u,  \
}

#define REGISTER_IPU(u)  \
extern struct DynamicIPU u;  \
IPU::DIPUs.push_back(&u)

typedef std::vector<DynamicIPU*>::iterator DIpuIterator;

#define UCSet(_f) std::string __f = _f; if(0)
#define UCFunction(_f) else if(__f == #_f)
#define UCFunctionOR(_f, _f1) else if(__f == #_f || __f == #_f1)
#define UCIfNone(_f...) else
#define UCCheckIPU(ipu) else if(ipu == NULL)
#define UCIsFunction(f1) (__f == #f1)


#endif // IPU_H
