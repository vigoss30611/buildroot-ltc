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
#include <Path.h>
#include <IPU.h>


#define IPU_TRACE(x) //x
#define IPU_INFO(x)   x

int IPU::Enhance = 1;

IPU::~IPU() {}
IPU::IPU()
{
    IPU_TRACE(printf("trace_ipu: %s, %d\n", __FUNCTION__, __LINE__));
    State = ST::NEW;
    Owner = NULL;
    m_enIPUType = IPU_NONE;
}

std::vector<struct DynamicIPU *> IPU::DIPUs;
IPU * IPU::DynamicCreate(std::string name, std::string ipu, Json *js)
{
//    LOGD("Creating IPU %s with name: %s\n", ipu.c_str(), name.c_str());
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, name.c_str()));
    for(DIpuIterator dipu = IPU::DIPUs.begin(); dipu != IPU::DIPUs.end(); dipu++)
    {
        if((*dipu)->Name == ipu)
            return (*dipu)->CreateIPU(name, js);
    }

    ::LOGE("Cannot find IPU with name: %s\n", ipu.c_str());
    throw VBERR_NOIPU;
}

void IPU::SetOwner(Path *p)
{
    IPU_TRACE(printf("trace_ipu: %s, %d\n", __FUNCTION__, __LINE__));
    Owner = p;
}

int IPU::RegisterUCFunction(std::string ucf)
{
    IPU_TRACE(printf("trace_ipu: %s, %d\n", __FUNCTION__, __LINE__));
    UCF.push_back(new std::string(ucf));
    return 0;
}

bool IPU::HasUCFunction(std::string ucf)
{
    IPU_TRACE(printf("trace_ipu: %s, %d\n", __FUNCTION__, __LINE__));
    std::vector<std::string*>::iterator ucfs;
    for(ucfs = UCF.begin(); ucfs != UCF.end(); ucfs++)
        if(*(*ucfs) == ucf) return true;
    return false;
}

bool IPU::IsInState(int st)
{
    IPU_TRACE(printf("trace_ipu: %s, %d, state:%d\n", __FUNCTION__, __LINE__, st));
    StateMTX.lock();
    bool ret = (st == State);
    StateMTX.unlock();
    return ret;
}

void IPU::SetState(int state)
{
    // IPU work state
    // NEW <==> INITED <==> Prepared -> StartPending
    //                        /|\             |
    //                         |             _|/
    //                      StopPending <- Running
    //
    // States changes in the above routine strictly,
    // Except for Error state, which can be accessed
    // from every state.

    // The route table is in Little Endine
    IPU_TRACE(printf("trace_ipu: %s, %d, %d\n", __FUNCTION__, __LINE__, state));
    int wrong = 0, route[] = {
        B(1000010),
        B(1000101),
        B(1001010),
        B(1010000),
        B(1100000),
        B(1100100),
        B(0000001),
    };

    StateMTX.lock();
    if(route[State] & (1 << state))
        State = state;
    else {
        LOGE("wrong state change: %d -> %d\n", State, state);
        wrong = 1;
    }
    StateMTX.unlock();
    if(wrong) throw VBERR_WRONGSTATE;
}

static void IPUException(int signo) {
    char name[32];
    prctl(PR_GET_NAME, name);
    LOGE("%s get %s(%d)!\n", name, signo == SIGSEGV?
        "Segmentation Fault": "Signal", signo);
    if(signo ==SIGHUP) return;
    exit(1);
}

static void IPUWorkThread(void *arg) {
    IPU_TRACE(printf("trace_ipu: %s, %d\n", __FUNCTION__, __LINE__));
    IPU *ipu = (IPU *)arg;
    prctl(PR_SET_NAME, ipu->Name.c_str());
    signal(SIGSEGV, IPUException);
    signal(SIGHUP, IPUException);
    ipu->SetState(IPU::ST::Running);
    try    {
        ipu->WorkLoop();
        ipu->SetState(IPU::ST::StopPending);
    }    catch (const char *err) {
        LOGE("encountered an error: %s\n", err);
        ipu->SetState(IPU::ST::Error);
        ipu->ReportError(err);
        return ;
    }
    ipu->SetState(IPU::ST::Prepared);
}

void IPU::InitPorts()
{
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    // for each port
    for(PortIterator port = OutPorts.begin();
    port != OutPorts.end(); port++) {
        // 1. embezzlement must be treated first
        // 2. allocate frBuffer
        // 3. sync data to bindports
        (*port)->SyncEmbezzlement();
        if((*port)->IsBind() || (*port)->IsExported() || (*port)->IsEmbezzled())
            (*port)->AllocateFRBuffer();
        (*port)->SyncBinding();
    }
    SetState(IPU::ST::INITED);
}

void IPU::DeinitPorts()
{
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    for(PortIterator port = OutPorts.begin();
        port != OutPorts.end(); port++) {
        // allocate frBuffer
        (*port)->FreeFRBuffer();
    }
    SetState(IPU::ST::NEW);
}

int IPU::SetPriority(int enhancePriority)
{
    IPU::Enhance = enhancePriority;
    IPU_INFO(printf("Set priority:%d\n", IPU::Enhance));
    return 0;
}

int IPU::Start() {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s %d\n", __FUNCTION__, __LINE__, Name.c_str(), IPU::Enhance));

    SetState(ST::StartPending);
    WorkThread = std::thread(IPUWorkThread, this);
    if (IPU::Enhance)
    {
        struct sched_param sched;
        sched.sched_priority = IPU_THREAD_PRIORITY;
        pthread_setschedparam(WorkThread.native_handle(), SCHED_RR, &sched);
    }

    return 0;
}

int IPU::Stop() {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    if(IsInState(ST::Running)) {
        SetState(ST::StopPending);
        for(TimePin tp; !IsInState(ST::Prepared);) {
            if(tp.Peek() > 1000) {
                pthread_kill(WorkThread.native_handle(), SIGHUP);
                LOGE("force terminated.\n");
                break;
            }
        }
        WorkThread.join();
    }
    return 0;
}

void IPU::Prepare() {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    LOGE("using default IPU::Prepare()\n");
}

void IPU::Unprepare() {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    LOGE("using default IPU::Unprepare()\n");
}

void IPU::WorkLoop() {
    LOGE("using default IPU::WorkLoop()\n");
}

int IPU::UnitControl(std::string func, void *arg) {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    LOGE("using default IPU::UnitControl()\n");
    return 0;
}

int IPU::GetDebugInfo(void *pInfo, int *ps32Size){
    LOGE("using default IPU::GetDebugInfo()\n");
    *ps32Size = 0;
    return 0;
}

void IPU::RequestOn(int force) {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    if(!IsInState(ST::INITED))
        return ;

    // Enable all its input ports
    for(PortIterator p = InPorts.begin();
        p != InPorts.end(); p++) {
            if((*p)->IsBind()) {
                if(force) (*p)->_Enable();
                else (*p)->Enable();
            }
    }

    // Enable all its output ports
    for(PortIterator p = OutPorts.begin();
        force && p != OutPorts.end(); p++) {
            if((*p)->IsBind() || (*p)->IsEmbezzled()
                || (*p)->IsExported())
                (*p)->_Enable();
    }

    // Start this IPU
    TimePin tp;
    Prepare();
    SetState(IPU::ST::Prepared);
    Start();
    LOGE("on (%dms)\n", tp.Peek() / 10);
}

void IPU::RequestOff(int force) {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    if(!IsInState(ST::Running)) {
        if(!force) LOGE("isn't running (state %d)\n", State);
        return ;
    }

    // Check if any port is still working
    for(PortIterator p = OutPorts.begin();
        !force && p != OutPorts.end(); p++) {
            if((*p)->IsEnabled()) {
                LOGE("remains on (%s-%s is working)\n",
                    (*p)->GetOwner()->Name.c_str(),
                    (*p)->GetName().c_str());
                return;
            }
    }

    // If none is working, stop the IPU
    Stop();
    Unprepare();
    SetState(IPU::ST::INITED);
    LOGE("off\n");

    // Disable all its input ports
    for(PortIterator p = InPorts.begin();
        !force && p != InPorts.end(); p++)
            (*p)->Disable();
}

Port* IPU::CreatePort(std::string name, int d)
{
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s, port_name: %s\n",
        __FUNCTION__, __LINE__, Name.c_str(), name.c_str()));
    Port *p = new Port(name, d, this);

    if(d == Port::In)
        InPorts.push_back(p);
    else
        OutPorts.push_back(p);
    return p;
}

void IPU::DestroyPort(std::string pname)
{
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    Port *p = GetPort(pname);
    if(p) { delete p;}
}

Port* IPU::GetPort(std::string name) {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    for(PortIterator p = InPorts.begin();
        p != InPorts.end(); p++) {
            if((*p)->GetName() == name)
                return (*p);
    }

    for(PortIterator p = OutPorts.begin();
        p != OutPorts.end(); p++) {
            if((*p)->GetName() == name)
                return (*p);
    }

    LOGE("cannot find port %s\n", name.c_str());
    throw VBERR_NOPORT;
}

Path* IPU::GetOwner()
{
    return Owner;
}

Port* IPU::GetMainInPort() {
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    if(!InPorts.empty()) return InPorts.front();

    LOGD("cannot find main-in-port for IPU %s\n", Name.c_str());
    throw VBERR_NOPORT;
}

void IPU::ReportError(std::string err)
{
    IPU_TRACE(printf("trace_ipu: %s, %d, name: %s\n", __FUNCTION__, __LINE__, Name.c_str()));
    if(Owner) Owner->SetError(err);
    LOGE("error: %s\n", err.c_str());
}

void IPU::LOGE(const char *fmt, ...)
{    ::LOGE("[%s]: ", Name.c_str());

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    fflush(stdout);
}
