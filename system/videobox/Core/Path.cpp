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
#include <sys/shm.h>

#define PATH_TRACE(x) //x
#define SUBTYPE_LIST_LENGTH   3
#define TYPE_LIST_LENGTH      2


static ST_EVENT_TYPE_ITEM s_pstDebugInfoEventList[SUBTYPE_LIST_LENGTH] =
{
    {ISP,    "isp"},
    {ISPOST, "ispost"},
    {VENC,   "venc"}
};

static ST_EVENT_TYPE_ITEM s_pstTypeEventList[TYPE_LIST_LENGTH] =
{
    {IPU_INFO_EVENT,    "ipu_info"},
    {IPU_DATA_EVENT,    "ipu_data"}
};

void Path::SetError(std::string err)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    Error = err;
}

void Path::SetJsonPath(std::string jsonpath)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    JsonPath = jsonpath;
}

const char *Path::GetError()
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    return Error.length()? Error.c_str(): NULL;
}

void Path::AddIPU(IPU *ipu)
{
    PATH_TRACE(printf("trace_path: %s, %d, name: %s\n", __FUNCTION__, __LINE__, ipu->Name.c_str()));
    ipu->SetOwner(this);
    IpuList.push_back(ipu);
}

Path& Path::operator<<(IPU& ipu)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    AddIPU(&ipu);
    return *this;
}

IPU* Path::GetIPU(std::string name)
{
    PATH_TRACE(printf("trace_path: %s, %d, name: %s\n", __FUNCTION__, __LINE__, name.c_str()));
    // For each IPU
    for(IpuListIterator ipu = IpuList.begin();
     ipu != IpuList.end(); ipu++){
        if((*ipu)->Name == name) {
            return *ipu;
        }
    }
    throw VBERR_NOIPU;
}

Port* Path::GetPort(std::string port, IPU** pipu)
{
    Port *p = NULL;
    IPU *ipu = NULL;

    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    LOGD("extract IPU and port in string %s\n", port.c_str());
    try {
        unsigned int sl = port.rfind("-");
        ipu = GetIPU(sl == std::string::npos? port: port.substr(0, sl));
        p = sl == std::string::npos? NULL: ipu->GetPort(port.substr(sl + 1));
    } catch (const char *err) {
        if(IsError(err, VBERR_NOIPU))
            LOGE("No IPU found in string: %s (%s)\n", port.c_str(), err);
        else
            LOGE("No Port found in string: %s (%s)\n", port.c_str(), err);
    }
    if(pipu) *pipu = ipu;
    return p;
}

void Path::On(void)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    // inititialize IPUs in Path
    for(IpuListIterator ipu = IpuList.begin();
            ipu != IpuList.end(); ipu++)
            (*ipu)->InitPorts();

    // Turn on each IPU
    for(IpuListIterator ipu = IpuList.begin();
     ipu != IpuList.end(); ipu++)
     (*ipu)->RequestOn(1);
}

void Path::Off(void)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    // Turn off each IPU
    for(IpuListIteratorReverse ipu = IpuList.rbegin();
     ipu != IpuList.rend(); ipu++)
     (*ipu)->RequestOff(1);

    // for each IPU
    for(IpuListIterator ipu = IpuList.begin();
            ipu != IpuList.end(); ipu++)
            (*ipu)->DeinitPorts();
}

void Path::Trigger(void)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    for(IpuListIteratorReverse ipu = IpuList.rbegin();
     ipu != IpuList.rend(); ipu++)
     (*ipu)->Start();
}

void Path::Suspend(void)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    for(IpuListIteratorReverse ipu = IpuList.rbegin();
     ipu != IpuList.rend(); ipu++)
     (*ipu)->Stop();
}

bool Path::DynamicFindPath(std::list<Port*> &plist, int type)
{
    Port *p = plist.front();

    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    if(p->GetDirection() == Port::In) {
        if(p->GetRuntimePolicy(type) == Port::RPolicy_Forbidden) {
            LOGE("Cannot find path: port %s-%s forbidden\n",
                p->GetOwner()->Name.c_str(), p->GetName().c_str());
            return false;
        } else {
            p = p->GetBindPorts()->front();
            if(p->GetBindPorts()->size() > 1) {
                LOGE("Cannot find path: port %s-%s isn't single bind\n",
                    p->GetOwner()->Name.c_str(), p->GetName().c_str());
                return false;
            }
        }
    } else {
        if(p->GetRuntimePolicy(type) > Port::RPolicy_Forbidden) return true;
        else p = p->GetOwner()->GetMainInPort();
    }

    // find upper port
    if(!p || !p->IsBind()) return false;

    plist.push_front(p);
    return DynamicFindPath(plist, type);
}

void Path::DynamicEnablePath(std::list<Port*> &plist, int type)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    std::list<Port*>::iterator pi;
    // FIXME: related IPU may be paused or stopped two
    // times becase both its input and output ports may
    // be registered in the temporary path.
    for(pi = plist.begin(); pi != plist.end(); pi++)
    {
        LOGE("Enable path port: %s-%s\n",
            (*pi)->GetOwner()->Name.c_str(),
            (*pi)->GetName().c_str());

        if((*pi)->GetRuntimePolicy(type) == Port::RPolicy_Forbidden)
            continue;

        if((*pi)->GetDirection() == Port::Out) {
            (*pi)->AllocateFRBuffer();
            (*pi)->SyncBinding();
        }

        if((*pi)->GetRuntimePolicy(type) < Port::RPolicy_PauseNeeded)
            (*pi)->GetOwner()->Prepare();
        if((*pi)->GetRuntimePolicy(type) < Port::RPolicy_Dynamic)
            (*pi)->GetOwner()->Start();
    }
}

void Path::DynamicDisablePath(std::list<Port*> &plist, int type)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    std::list<Port*>::reverse_iterator pi;

    for(pi = plist.rbegin(); pi != plist.rend(); pi++)
    {
        LOGE("Disable path port: %s-%s\n",
            (*pi)->GetOwner()->Name.c_str(),
            (*pi)->GetName().c_str());

        if((*pi)->GetRuntimePolicy(type) == Port::RPolicy_Forbidden)
            continue;

        if((*pi)->GetRuntimePolicy(type) < Port::RPolicy_Dynamic)
            (*pi)->GetOwner()->Stop();

        if((*pi)->GetRuntimePolicy(type) < Port::RPolicy_PauseNeeded)
            (*pi)->GetOwner()->Unprepare();

        if((*pi)->GetDirection() == Port::Out)
            (*pi)->FreeFRBuffer();
    }
}

void Path::SyncPathFps(int fps)
{
    std::vector<IPU*> List = IpuList;

    for (IpuListIterator ipu = List.begin(); ipu != List.end(); ipu++) {
        std::vector<Port*> outport = (*ipu)->OutPorts;
        for (PortIterator port = outport.begin(); port != outport.end(); port++) {
            PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
            (*port)->SetFPS(fps);
            (*port)->SyncBinding();
        }
    }
}

void Path::Config(IPU *ipu, Json *ports_js){
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    for(Json *port_js = ports_js->Child(); port_js;
        port_js = port_js->Next()) {
        int w = 0, h= 0, pip_x = 0, pip_y = 0,
                pip_w = 0, pip_h = 0, link = 0, lock_ratio = 0, timeout = 0,
                minfps = 0, maxfps = 0;

        for(Json *pset_js = port_js->Child(); pset_js;
            pset_js = pset_js->Next()) {
#define GETJSVAR(v) else if(!strcmp(pset_js->string, #v))  \
    v = pset_js->valueint

            if(0); // Get common values from JSON
            GETJSVAR(w);
            GETJSVAR(h);
            GETJSVAR(pip_x);
            GETJSVAR(pip_y);
            GETJSVAR(pip_w);
            GETJSVAR(pip_h);
            GETJSVAR(link);
            GETJSVAR(lock_ratio);
            GETJSVAR(timeout);
            GETJSVAR(minfps);
            GETJSVAR(maxfps);

            else if(!strcmp(pset_js->string, "pixel"))
                ipu->GetPort(port_js->string)->
                SetPixelFormat(Pixel::ParseString(pset_js->valuestring));
            else if (!strcmp(pset_js->string, "pixel_format"))
            {
                ipu->GetPort(port_js->string)->
                SetPixelFormat(Pixel::ParseString(pset_js->valuestring));
            }
            else if(!strcmp(pset_js->string, "export"))
                ipu->GetPort(port_js->string)->Export();
            else if(!strcmp(pset_js->string, "bind"))
            {
                for(Json *bind_js = pset_js->Child(); bind_js; bind_js = bind_js->Next())
                {
                    ipu->GetPort(port_js->string)->
                    Bind(GetIPU(bind_js->string)->GetPort(bind_js->valuestring));
                }
            }
            else if(!strcmp(pset_js->string, "embezzle"))
            {
                Json *ojs = pset_js->Child();
                ipu->GetPort(port_js->string)->
                Embezzle(GetIPU(ojs->string)->GetPort(ojs->valuestring));
            }
            else {
                LOGE("Unrecognized \"%s\" port \"%s\" attribute: %s\n",\
                        ipu->Name.c_str(), port_js->string, pset_js->string);
                throw VBERR_WRONGDESC;
            }
        }
        if(lock_ratio) ipu->GetPort(port_js->string)->SetLockRatio(lock_ratio);
        if (timeout)
            ipu->GetPort(port_js->string)->SetFRBufferTimeout(timeout);

        if(w || h) ipu->GetPort(port_js->string)->
            SetResolution(w, h);
        if(pip_x || pip_y || pip_w || pip_h) ipu->
            GetPort(port_js->string)->SetPipInfo(pip_x, pip_y, pip_w, pip_h);
        if(minfps || maxfps) ipu->GetPort(port_js->string)->SetFpsRange(minfps,maxfps);
        if(link) ipu->GetPort(port_js->string)->LinkEnable();
    }
}

void Path::Build(Json *js) {

    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    RootJson = js;
    // Check ipu description
    for(Json *ipu_js = js->Child(); ipu_js; ipu_js = ipu_js->Next()) {
        for(Json *attr_js = ipu_js->Child(); attr_js; attr_js = attr_js->Next())
            if(strncmp(attr_js->string, "ipu", 4) &&
                strncmp(attr_js->string, "args", 5) &&
                strncmp(attr_js->string, "port", 5)) {
                LOGE("Wrong IPU attribute: %s", attr_js->string);
                throw VBERR_WRONGDESC;
            }
    }
    IPU::SetPriority(1);
    // Create IPUs
    for(Json *ipu_js = js->Child(); ipu_js; ipu_js = ipu_js->Next()) {
        IPU *ipu = IPU::DynamicCreate(ipu_js->string,
            ipu_js->GetString("ipu"),
            ipu_js->GetObject("args"));
        AddIPU(ipu);
        if (strncmp(ipu_js->GetString("ipu"), "ffvdec", 6) == 0)
        {
            IPU::SetPriority(0);
        }
    }

    // Configurate Ports
    for(Json *ipu_js = js->Child(); ipu_js; ipu_js = ipu_js->Next()) {
        Json *ports_js = ipu_js->GetObject("port");
        if(ports_js) Config(GetIPU(ipu_js->string), ports_js);
    }
}

void Path::Rebuild(Json *js)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    Off();
    while(!IpuList.empty()) {
//        IPU *ipu = IpuList.back();
        IpuList.pop_back();
//        LOGE("remove ipu: %s\n", ipu->Name.c_str());
//        delete ipu;
    }
    Build(js);
    On();
}

void Path::GetVencInfo(void *pArg)
{
    ST_VENC_DBG_INFO *pDbgInfo = NULL;
    ST_VENC_INFO *pVencInfo = NULL;

    if (!pArg)
    {
        LOGE("agument is invalid\n");
        return;
    }
    pDbgInfo = (ST_VENC_DBG_INFO *)pArg;
    pVencInfo = pDbgInfo->stVencInfo;
    pDbgInfo->s8Size = 0;

    for (IpuListIterator ipu = IpuList.begin(); ipu != IpuList.end(); ipu++)
    {
        if ((*ipu)->m_enIPUType == IPU_H264
            || (*ipu)->m_enIPUType == IPU_H265
            || (*ipu)->m_enIPUType == IPU_JPEG)
        {
            if (pDbgInfo->s8Size < pDbgInfo->s8Capcity)
            {
                (*ipu)->GetDebugInfo(pVencInfo, NULL);
                pVencInfo++;
                pDbgInfo->s8Size++;
            }
            else
            {
                LOGE("User memory is small,left channel(%s) info,[Index, Capcity]--[%d, %d]\n",
                    pVencInfo->ps8Channel, pDbgInfo->s8Size, pDbgInfo->s8Capcity);
            }
        }
    }
}

void Path::GetISPInfo(void *pArg)
{
    int s32RetSize = 0;
    int s32ObjSize = 0;

    if (!pArg)
    {
        LOGE("agument is invalid\n");
        return;
    }

    s32ObjSize = sizeof(ST_ISP_DBG_INFO);
    for (IpuListIterator ipu = IpuList.begin(); ipu != IpuList.end(); ipu++)
    {
        if ((*ipu)->m_enIPUType == EN_IPU_ISP)
        {
            (*ipu)->GetDebugInfo(pArg, &s32RetSize);
            if (s32ObjSize != s32RetSize)
            {
                LOGE("get novalid size:%d, need to %d\n", s32RetSize, s32ObjSize);
            }
            break;
        }
    }
}

static int Get_event_type(const char* ps8Event, const ST_EVENT_TYPE_ITEM * pstList, int s32Len)
{
    for (int i = 0; i < s32Len; i++)
    {
        if (strncmp(pstList[i].ps8MetaData, ps8Event, strlen(pstList[i].ps8MetaData)) == 0)
        {
            return pstList[i].s32Type;
        }
    }
    return -1;
}

int Path::VideoExchangeInfo(void *pArg)
{
    ST_SHM_INFO stShmInfo;
    int s32ShmID;
    char *pMapMem = NULL;

    if (!pArg)
    {
        LOGE("argument invalid!\n");
        return -1;
    }

    memcpy((char *)&stShmInfo, (char *)pArg, sizeof(ST_SHM_INFO));

    s32ShmID = shmget(stShmInfo.s32Key, 0, 0666);
    if ((int)s32ShmID == -1)
    {
        LOGE("(%s:L%d) shmget is error (key=0x%x) errno:%s!!!\n", __func__, __LINE__,
            stShmInfo.s32Key, strerror(errno));
        return -1;
    }

    pMapMem = (char*)shmat(s32ShmID, NULL, 0);
    if ((int)pMapMem == -1)
    {
        LOGE("(%s:L%d) pbyMap is NULL (iShmId=%d) errno:%s!!!\n", __func__, __LINE__,
            s32ShmID, strerror(errno));
        return -1;
    }

    switch (Get_event_type(stShmInfo.ps8Type, s_pstTypeEventList, TYPE_LIST_LENGTH))
    {
        case IPU_INFO_EVENT:
            switch (Get_event_type(stShmInfo.ps8SubType, s_pstDebugInfoEventList, SUBTYPE_LIST_LENGTH))
            {
                case ISP:
                    LOGD("[%s]Get ISP INFO\n", stShmInfo.ps8SubType);
                    GetISPInfo(pMapMem);
                    break;
                case ISPOST:
                    LOGD("[%s]Get ISPOST INFO\n", stShmInfo.ps8SubType);
                    break;
                case VENC:
                    LOGD("[%s]Get VENC INFO\n", stShmInfo.ps8SubType);
                    GetVencInfo(pMapMem);
                    break;
                default:
                    LOGE("[%s]Can not get debug info\n", stShmInfo.ps8SubType);
                    break;
            }
            break;
        case IPU_DATA_EVENT:
            {
                const char *ps8Split = "#";
                std::string ps8Channel = strtok(stShmInfo.ps8SubType, ps8Split);
                std::string ps8Fun = strtok(NULL, ps8Split);

                PathControl(ps8Channel, ps8Fun, pMapMem);
            }
            break;
        default:
            LOGE("[%s]event type error\n",stShmInfo.ps8Type);
            break;
    }

    if (shmdt(pMapMem) == -1)
    {
        LOGE("(%s:L%d) shmdt is error (pbyMap=%p) errno:%s!!!\n", __func__, __LINE__,
            pMapMem, strerror(errno));
        return -1;
    }

    return 0;
}

int Path::SetPathPointer(Path *path) {
    if(!RootPath)
        RootPath = path;
    return 0;
}

int Path::SetJsonPointer(Json *js) {
    if(!RootJson)
        RootJson = js;
    return 0;
}

int Path::PathControl(std::string chn, std::string func, void *arg)
{
    PATH_TRACE(printf("trace_path: %s, %d\n", __FUNCTION__, __LINE__));
    IPU *ipu = NULL;
    Port *p = NULL;

    if(!chn.empty() && strncmp(func.c_str(), "videobox_ipu", strlen("videobox_ipu"))) {
        p = GetPort(chn, &ipu);
        if(ipu == NULL) {
            LOGE("PathControl %s:%s IPU not found.\n",
                    func.c_str(), chn.c_str(), GetError());
            return VBEFAILED;
        }
    }

    if(GetError()) {
        LOGE("PathControl %s:%s ignored (error state: %s)\n",
            func.c_str(), chn.c_str(), GetError());
        return VBEFAILED;
    }
    UCSet(func);
    UCFunction(videobox_poll) {
        return getppid();
    }

    UCFunction(videobox_repath) {
        Json js;
        js.Load((char *)arg);

        SetJsonPath((char*)arg);
        Rebuild(&js);
        return 0;
    }

    UCFunction(videobox_get_jsonpath) {
        strcpy((char*)arg, JsonPath.c_str());
        return 0;
    }

    UCFunction(videobox_rebind) {
        Port *p_new = GetPort((char *)arg, NULL);
        if(!p) {
            LOGE("Port is not provided\n");
            return VBEFAILED;
        }

        ipu->RequestOff(0);
        if(!ipu->IsInState(IPU::ST::INITED)) {
            LOGE("IPU: %s cannot be turnned off\n", ipu->Name.c_str());
            return VBEFAILED;
        }

        p->Unbind();
        p_new->Bind(p);
        p_new->AllocateFRBuffer();
        p_new->SyncBinding(p);
        ipu->RequestOn(0);
        return 0;
    }

    UCFunction(video_enable_channel) {
        if(p) p->Enable();
        else if(ipu) ipu->RequestOn(0);
        else return VBEBADPARAM;

        return 0;
    }

    UCFunction(video_disable_channel) {
        std::vector<Port*> outport = ipu->OutPorts;
        for (PortIterator p_c = outport.begin(); p_c != outport.end(); p_c++)
            (*p_c)->Disable();
        if(!p && ipu) ipu->RequestOff(0);
        else return VBEBADPARAM;

        return 0;
    }

    UCFunction(video_get_fps) {
        try {
            return p? p->GetTargetFPS():
                ipu->GetMainInPort()->GetTargetFPS();
        } catch (const char *err) { }

        return VBEFAILED;
    }

    UCFunction(video_set_fps) {
        int fps = *(int *)arg;
        try {
            ipu->GetMainInPort()->SetTargetFPS(fps);
        } catch (const char *err) {
            return VBEFAILED;
        }
        return 0;
    }

    UCFunction(video_get_resolution) {
        vbres_t *res = (vbres_t *)arg;

        if(!p) return VBEBADPARAM;
        if(Pixel::Bits(p->GetPixelFormat()) == 0) {
            res->w = ipu->GetMainInPort()->GetWidth();
            res->h = ipu->GetMainInPort()->GetHeight();
        } else {
            res->w = p->GetWidth();
            res->h = p->GetHeight();
        }
        return 0;
    }

    UCFunction(video_set_resolution) {
        std::list<Port*> plist;

        plist.push_front(p? p: ipu->GetMainInPort());
        if(!DynamicFindPath(plist, Port::RPType_Resolution))
            return VBEFAILED;

        vbres_t *res = (vbres_t *)arg;
        LOGE("%s: %d, %d\n", func.c_str(), res->w, res->h);

        DynamicDisablePath(plist, Port::RPType_Resolution);
        plist.front()->SetResolution(res->w, res->h);
        DynamicEnablePath(plist, Port::RPType_Resolution);

        return 0;
    }

    UCFunction(videobox_mod_offset) {
        std::list<Port*> plist;

        plist.push_front(p? p: ipu->GetMainInPort());
        vbres_t *res = (vbres_t *)arg;
        LOGE("%s: %d, %d\n", func.c_str(), res->w, res->h);

        if((res->w < 0) || (res->w > (ipu->GetMainInPort()->GetWidth() - plist.front()->GetPipWidth()))) {
            return -1;
        }

        if((res->h < 0) || (res->h > (ipu->GetMainInPort()->GetHeight() - plist.front()->GetPipHeight()))) {
            return -1;
        }

        plist.front()->SetPipInfo(res->w, res->h, plist.front()->GetPipWidth(), plist.front()->GetPipHeight());

        return 0;
    }

    UCFunctionOR(videobox_ipu_set_property, videobox_ipu_get_property) {
        if(!RootJson) {
            LOGE("the root json pointer is not ready\n");
            return E_VIDEOBOX_NULL_ARGUMENT;
        }

        const char *const (*ps8ArgsInfo)[2];
        int s32MaxCnt;
        int s32ArrayNum;
        int *value;
        EN_VIDEOBOX_RET enRet = E_VIDEOBOX_OK;
        ST_VIDEOBOX_ARGS_INFO *pstIpuArgs = (ST_VIDEOBOX_ARGS_INFO *)arg;
        ST_JSON_OPERA stJsonOpera;
        ST_VIDEO_RATE_CTRL_INFO stRateCtrl;
        memset(&stJsonOpera, 0, sizeof(ST_JSON_OPERA));
        memset(&stRateCtrl, 0, sizeof(ST_VIDEO_RATE_CTRL_INFO));
        stJsonOpera.bIsArgs = true;
        if(UCIsFunction(videobox_ipu_set_property))
            stJsonOpera.bSet = true;
        else if(UCIsFunction(videobox_ipu_get_property))
            stJsonOpera.bGet = true;

        switch(pstIpuArgs->enProperty / VIDEOBOX_PROPERTY_BASE_OFFSET) {
            case 0: //isp
                ps8ArgsInfo = ps8IspArgsInfo;
                s32ArrayNum = sizeof(ps8IspArgsInfo);
                s32MaxCnt = E_VIDEOBOX_ISP_MAX;
                break;
            case 1: //ispost
                ps8ArgsInfo = ps8IspostArgsInfo;
                s32ArrayNum = sizeof(ps8IspostArgsInfo);
                s32MaxCnt = E_VIDEOBOX_ISPOST_MAX;
                break;
            case 2: //vencoder
                if(pstIpuArgs->enProperty == E_VIDEOBOX_VENCODER_RATECTRL) {
                    ps8ArgsInfo = ps8RateCtrlInfo;
                    s32ArrayNum = sizeof(ps8RateCtrlInfo);
                    s32MaxCnt = E_VIDEOBOX_RATECTRL_END;
                } else {
                    ps8ArgsInfo = ps8VencoderArgsInfo;
                    s32ArrayNum = sizeof(ps8VencoderArgsInfo);
                    s32MaxCnt = E_VIDEOBOX_VENCODER_MAX;
                }
                break;
            case 3: //h1jpeg
                ps8ArgsInfo = ps8H1jpegArgsInfo;
                s32ArrayNum = sizeof(ps8H1jpegArgsInfo);
                s32MaxCnt = E_VIDEOBOX_H1JPEG_MAX;
                break;
            case 4: //jenc
                ps8ArgsInfo = ps8JencArgsInfo;
                s32ArrayNum = sizeof(ps8JencArgsInfo);
                s32MaxCnt = E_VIDEOBOX_JENC_MAX;
                break;
            case 5: //pp
                ps8ArgsInfo = ps8PpArgsInfo;
                s32ArrayNum = sizeof(ps8PpArgsInfo);
                s32MaxCnt = E_VIDEOBOX_PP_MAX;
                break;
            case 6: //marker
                ps8ArgsInfo = ps8MarkerArgsInfo;
                s32ArrayNum = sizeof(ps8MarkerArgsInfo);
                s32MaxCnt = E_VIDEOBOX_MARKER_MAX;
                break;
            case 7: //ids
                ps8ArgsInfo = ps8IdsArgsInfo;
                s32ArrayNum = sizeof(ps8IdsArgsInfo);
                s32MaxCnt = E_VIDEOBOX_IDS_MAX;
                break;
            case 8: //dgframe
                ps8ArgsInfo = ps8DgFrameArgsInfo;
                s32ArrayNum = sizeof(ps8DgFrameArgsInfo);
                s32MaxCnt = E_VIDEOBOX_DGFRAME_MAX;
                break;
            case 9: //fodet
                ps8ArgsInfo = ps8FoDetArgsInfo;
                s32ArrayNum = sizeof(ps8FoDetArgsInfo);
                s32MaxCnt = E_VIDEOBOX_FODETV2_MAX;
                break;
            case 10: //va
                ps8ArgsInfo = ps8VaArgsInfo;
                s32ArrayNum = sizeof(ps8VaArgsInfo);
                s32MaxCnt = E_VIDEOBOX_VA_MAX;
                break;
            case 11: //filesink
                ps8ArgsInfo = ps8FileSinkArgsInfo;
                s32ArrayNum = sizeof(ps8FileSinkArgsInfo);
                s32MaxCnt = E_VIDEOBOX_FILESINK_MAX;
                break;
            case 12: //filesource
                ps8ArgsInfo = ps8FileSrcArgsInfo;
                s32ArrayNum = sizeof(ps8FileSrcArgsInfo);
                s32MaxCnt = E_VIDEOBOX_FILESOURCE_MAX;
                break;
            case 13: //v4l2
                ps8ArgsInfo = ps8V4l2ArgsInfo;
                s32ArrayNum = sizeof(ps8V4l2ArgsInfo);
                s32MaxCnt = E_VIDEOBOX_V4L2_MAX;
                break;
            default:
                break;
        }

        if(s32MaxCnt % VIDEOBOX_PROPERTY_BASE_OFFSET != s32ArrayNum / (int)(sizeof(char*(*)[]) * 2)) {
            LOGE("Enum args parameter is not corresponds with args array\n");
            return VBEBADPARAM;
        }

        if(pstIpuArgs->enProperty == E_VIDEOBOX_VENCODER_RATECTRL) {
            if(stJsonOpera.bSet)
                memcpy(&stRateCtrl, pstIpuArgs->PropertyInfo, sizeof(ST_VIDEO_RATE_CTRL_INFO));
            for(int key = 0; key < E_VIDEOBOX_RATECTRL_END - 1; key++) {
                switch (key) {
                    case E_VIDEOBOX_RATECTRL_RCMODE:
                        value = (int*)&(stRateCtrl.rc_mode);
                        break;
                    case E_VIDEOBOX_RATECTRL_GOP:
                        value = (int*)&(stRateCtrl.gop_length);
                        break;
                    case E_VIDEOBOX_RATECTRL_IDRINTERVAL:
                        value = (int*)&(stRateCtrl.idr_interval);
                        break;
                    case E_VIDEOBOX_RATECTRL_QPMAX:
                    case E_VIDEOBOX_RATECTRL_QPMIN:
                    case E_VIDEOBOX_RATECTRL_BITRATE:
                    case E_VIDEOBOX_RATECTRL_QPDELTA:
                    case E_VIDEOBOX_RATECTRL_HDR:
                    case E_VIDEOBOX_RATECTRL_MAXBITRATE:
                    case E_VIDEOBOX_RATECTRL_THRESHOLD:
                    case E_VIDEOBOX_RATECTRL_QPFIX:
                        switch (stRateCtrl.rc_mode) {
                            case VENC_CBR_MODE:
                                if (key == E_VIDEOBOX_RATECTRL_QPMAX) {
                                    value = (int*)&(stRateCtrl.cbr.qp_max);
                                } else if (key == E_VIDEOBOX_RATECTRL_QPMIN) {
                                    value = (int*)&(stRateCtrl.cbr.qp_min);
                                } else if (key == E_VIDEOBOX_RATECTRL_BITRATE) {
                                    value = (int*)&(stRateCtrl.cbr.bitrate);
                                } else if (key == E_VIDEOBOX_RATECTRL_QPDELTA) {
                                    value = (int*)&(stRateCtrl.cbr.qp_delta);
                                } else if (key == E_VIDEOBOX_RATECTRL_HDR) {
                                    value = (int*)&(stRateCtrl.cbr.qp_hdr);
                                }
                                break;
                            case VENC_VBR_MODE:
                                if (key == E_VIDEOBOX_RATECTRL_QPMAX) {
                                    value = (int*)&(stRateCtrl.vbr.qp_max);
                                } else if (key == E_VIDEOBOX_RATECTRL_QPMIN) {
                                    value = (int*)&(stRateCtrl.vbr.qp_min);
                                } else if (key == E_VIDEOBOX_RATECTRL_MAXBITRATE) {
                                    value = (int*)&(stRateCtrl.vbr.max_bitrate);
                                } else if (key == E_VIDEOBOX_RATECTRL_THRESHOLD) {
                                    value = (int*)&(stRateCtrl.vbr.threshold);
                                } else if (key == E_VIDEOBOX_RATECTRL_QPDELTA) {
                                    value = (int*)&(stRateCtrl.vbr.qp_delta);
                                }
                                break;
                            case VENC_FIXQP_MODE:
                                if (key == E_VIDEOBOX_RATECTRL_QPFIX)
                                    value = (int*)&(stRateCtrl.fixqp.qp_fix);
                                break;
                            default:
                                break;
                        }
                    default:
                        break;
                }
                if(*value || stJsonOpera.bGet) {
                    enRet = (EN_VIDEOBOX_RET)(enRet || RootJson->JOperaCore(&stJsonOpera, (char*)chn.c_str(), NULL,
                                ps8ArgsInfo[key][E_JSON_KEYNAME],
                                value,
                                atoi(ps8ArgsInfo[key][E_JSON_VALUETYPE])));
                }
            }
            if(stJsonOpera.bGet)
                memcpy(pstIpuArgs->PropertyInfo, &stRateCtrl, sizeof(ST_VIDEO_RATE_CTRL_INFO));
        } else {
            enRet = RootJson->JOperaCore(&stJsonOpera, (char*)chn.c_str(), NULL,
                    ps8ArgsInfo[pstIpuArgs->enProperty % VIDEOBOX_PROPERTY_BASE_OFFSET][E_JSON_KEYNAME],
                    &pstIpuArgs->PropertyInfo,
                    atoi(ps8ArgsInfo[pstIpuArgs->enProperty % VIDEOBOX_PROPERTY_BASE_OFFSET][E_JSON_VALUETYPE]));
        }

        return enRet;
    }

    UCFunctionOR(videobox_ipu_set_attribute, videobox_ipu_get_attribute) {
        ST_JSON_OPERA stJsonOpera;
        EN_VIDEOBOX_RET enRet = E_VIDEOBOX_OK;
        ST_VIDEOBOX_PORT_INFO *portargs = (ST_VIDEOBOX_PORT_INFO *)arg;
        ST_VIDEOBOX_PORT_ATTRIBUTE stPort = portargs->stPortAttribute;
        int key;
        int *value = (int*)&stPort;
        memset(&stJsonOpera, 0, sizeof(ST_JSON_OPERA));
        stJsonOpera.bIsPort = true;
        if(UCIsFunction(videobox_ipu_set_attribute))
            stJsonOpera.bSet = true;
        else if(UCIsFunction(videobox_ipu_get_attribute))
            stJsonOpera.bGet = true;
        for(key = 0; key < E_VIDEOBOX_PORT_END - 1; key++, value++) { //process unsigned int
            if(*value || stJsonOpera.bGet) {
                enRet = (EN_VIDEOBOX_RET)(enRet || RootJson->JOperaCore(&stJsonOpera, (char*)chn.c_str(),
                            portargs->s8PortName,
                            (char*)ps8PortInfo[key][E_JSON_KEYNAME],
                            value,
                            atoi((char*)ps8PortInfo[key][E_JSON_VALUETYPE])));
            }
        }
        if(*value || stJsonOpera.bGet) {
            enRet = (EN_VIDEOBOX_RET)(enRet || RootJson->JOperaCore(&stJsonOpera, (char*)chn.c_str(),
                        portargs->s8PortName,  //process string
                        (char*)ps8PortInfo[key][E_JSON_KEYNAME],
                        (char*)value,
                        atoi((char*)ps8PortInfo[key][E_JSON_VALUETYPE])));
        }
        if(stJsonOpera.bGet) {
            portargs->stPortAttribute = stPort;
        }
        return enRet;
    }

    else if(UCIsFunction(videobox_ipu_bind)
            || UCIsFunction(videobox_ipu_unbind)
            || UCIsFunction(videobox_ipu_embezzle)
            || UCIsFunction(videobox_ipu_unembezzle)) {
        ST_VIDEOBOX_BIND_EMBEZZLE_INFO *BEargs = (ST_VIDEOBOX_BIND_EMBEZZLE_INFO *)arg;
        ST_JSON_OPERA stJsonOpera;
        memset(&stJsonOpera, 0, sizeof(ST_JSON_OPERA));
        if(UCIsFunction(videobox_ipu_bind)) {
            stJsonOpera.bIsBind = true;
            stJsonOpera.bIsPort = true;
            stJsonOpera.bSet = true;
        } else if(UCIsFunction(videobox_ipu_unbind)) {
            stJsonOpera.bIsBind = true;
            stJsonOpera.bIsPort = true;
            stJsonOpera.bDelete = true;
        } else if(UCIsFunction(videobox_ipu_embezzle)) {
            stJsonOpera.bIsEmbezzle = true;
            stJsonOpera.bIsPort = true;
            stJsonOpera.bSet = true;
        } else if(UCIsFunction(videobox_ipu_unembezzle)) {
            stJsonOpera.bIsEmbezzle = true;
            stJsonOpera.bIsPort = true;
            stJsonOpera.bDelete = true;
        }
        EN_VIDEOBOX_RET enRet = RootJson->JOperaCore(&stJsonOpera, (char*)chn.c_str(),
                BEargs->s8SrcPortName, BEargs->s8SinkIpuID, BEargs->s8SinkPortName, Json_String);
        return enRet;
    }

    UCFunctionOR(videobox_ipu_create, videobox_ipu_delete) {
        ST_JSON_OPERA stJsonOpera;
        memset(&stJsonOpera, 0, sizeof(ST_JSON_OPERA));
        stJsonOpera.bIsIpu = true;
        if(UCIsFunction(videobox_ipu_create))
            stJsonOpera.bSet = true;
        else if(UCIsFunction(videobox_ipu_delete))
            stJsonOpera.bDelete = true;
        EN_VIDEOBOX_RET enRet = RootJson->JOperaCore(&stJsonOpera, (char*)chn.c_str(), NULL, NULL, NULL, 0);
        return enRet;
    }

    UCFunction(videobox_trigger) {
        Trigger();
        return 0;
    }

    UCFunction(videobox_suspend) {
        Suspend();
        return 0;
    }

    UCFunction(videobox_delete_path) {
        KillHandler(0);
    }

    UCFunction(videobox_start_path) {
        try {
            RootPath->Build(RootJson);
            RootPath->On();
        } catch ( const char* err ) {
            LOGE("ERROR: %s\n", err);
            if(IsError(err, VBERR_WRONGDESC)) LOGE("loc: %s\n",
                    RootJson->EP());
            return -1;
        }
        return 0;
    }

    UCFunction(videobox_stop_path) {
        RootPath->Off();
        return 0;
    }

    UCFunction(videobox_jsonshow) {
        RootJson->JsonShow(NULL);
        return 0;
    }

    UCIfNone() {
        return ipu ? ipu->UnitControl(func, arg) : 0;
    }

    throw VBERR_BADLOGIC;
}
