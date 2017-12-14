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
#include <Port.h>
#include <IPU.h>

#define PORT_TRACE(x) //x

Port::Port(std::string name, int dir, IPU *owner) {
    PORT_TRACE(printf("trace_port: %s, %d, own:%s\n", __FUNCTION__, __LINE__, owner->Name.c_str()));
    Name = name;
    Direction = dir;
    Owner = owner;
    frBuffer = NULL;
    EmbezzledPort = NULL;
    EnableCount = 0;
    LinkMode = 0;
    PipX = PipY = PipWidth = PipHeight = 0;
    MinFps = MaxFps = 0;
    LockRatio = 0;
    for(int i = 0; i < RPType_COUNT; RuntimePolicy[i++]
        = RPolicy_Forbidden);
    SkipKeep = 0;
    SkipTable = SkipTable_30;
    framecnt = 0;
    FpsRangeMode = false;
    Exported = false;
    LinkMTX.lock();
}

std::string Port::GetName() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return Name;
}

int Port::GetDirection() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return Direction;
}

void Port::SetResolution(int w, int h) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    LOGD("set resolution: %d %d\n", w, h);
    Width = w;
    Height = h;
}
void Port::SetLockRatio(int ratio) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    this->LockRatio = ratio;
}
int Port::GetWidth() { return Width;}
int Port::GetHeight() { return Height;}

void Port::SetPipInfo(int x, int y, int w, int h) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    PipX = x; PipY = y; PipWidth = w; PipHeight = h;
}
int Port::GetPipX() { return PipX;}
int Port::GetPipY() { return PipY;}
int Port::GetPipWidth() { return PipWidth;}
int Port::GetPipHeight() { return PipHeight;}
int Port::GetLockRatio() {return this->LockRatio;}
void Port::SetPixelFormat(Pixel::Format pf) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    LOGD("set pixel format: %d\n", pf);
    PixelFormat = pf;
}

Pixel::Format Port::GetPixelFormat() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return PixelFormat;
}

void Port::ResetSkipper()
{
    if(FPS == 25 || FPS == 30) {
        switch (FPS) {
            case 25:
                SkipTable = SkipTable_25;
                break;
            case 30:
                SkipTable = SkipTable_30;
                break;
            default:
                SkipTable = SkipTable_30;
                break;
        }
    } else {
        PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
                    __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
        SkipKeep = FPS - TargetFPS;
        SkipDrop = FPS / TargetFPS - 1;
        if(SkipKeep) {
            SkipKeep = FPS / SkipKeep - 1;
            if(!SkipKeep) SkipKeep = 1;
        }
        if(!SkipDrop) SkipDrop = 1;
        LOGD("reset skipper: %d, %d, keep&drop: %d/%d\n",
                FPS, TargetFPS, SkipKeep, SkipDrop);
        SkipCounter = 0;
    }
}

bool Port::CheckSkipper()
{
    if(FpsRangeMode)
        return 0;

    if(FPS == 25 || FPS == 30) {
        framecnt = framecnt % FPS;
        return SkipTable[TargetFPS][framecnt++] ? 0 : 1;
    } else {
        PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
                    __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
        if(!SkipKeep) return false;

        bool ret = false;
        if(SkipCounter >= SkipKeep &&
                SkipCounter < SkipKeep + SkipDrop)
            ret = true;

        if(++SkipCounter >= SkipKeep + SkipDrop)
            SkipCounter = 0;

        return ret;
    }
}

int Port::_SetFPS(int fps, int sk_fps) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));

    if((fps & 0x80) && !FpsRangeMode) {
        FpsRangeMode = true;
        FPS = fps;
    }

    if(!(fps & 0x80) && FpsRangeMode) {
        FpsRangeMode = false;
        FPS = fps;
    }

    if(FpsRangeMode)
        return 0;

    if(fps <= 0) return 0;
    if(sk_fps > fps) throw VBERR_BADPARAM;

    FPS = fps;
    TargetFPS = sk_fps;

    try {
        if(GetOwner()->GetMainInPort() == this) {
            for(PortIterator p = GetOwner()->OutPorts.begin();
                p != GetOwner()->OutPorts.end(); p++) {
                (*p)->SetFPS(TargetFPS);
            }
        }
    } catch (const char *err) {} // do nothing if no main-in-port

    ResetSkipper();
    return sk_fps;
}

int Port::SetTargetFPS(int sk_fps) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return _SetFPS(FPS, sk_fps);
}

int Port::GetTargetFPS() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return TargetFPS;
}

void Port::SetFPS(int fps) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    _SetFPS(fps, fps);
}

int Port::GetFPS() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return FPS & 0x7f;
}

void Port::SetFpsRange(int x, int y) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));

    MinFps = x; MaxFps = y;

    if (MinFps != 0 && MaxFps == 0) {
        _SetFPS(MinFps, MinFps);
    } else {
        _SetFPS(MaxFps, MaxFps);
    }
}

int Port::GetMinFps() { return MinFps;}
int Port::GetMaxFps() { return MaxFps;}

int Port::GetFrameSize() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(BufferType == FRBuffer::Type::VACANT
        || BufferType == FRBuffer::Type::FIXED) {
        return Width * Height * Pixel::Bits(PixelFormat) / 8;
    }
    return 0;
}

void Port::SetRuntimePolicy(int type, int policy)
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    RuntimePolicy[type] = policy;
}

int Port::GetRuntimePolicy(int type) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return RuntimePolicy[type];
}

void Port::SetBufferType(FRBuffer::Type type, int count)
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(type == FRBuffer::Type::FLOAT ||
        type == FRBuffer::Type::FLOAT_NODROP)
        throw VBERR_BADPARAM;

    if(EmbezzledPort && type != FRBuffer::Type::VACANT) {
        LOGE("embezzling another port(%s-%s), attempt"
            " to set buffer type as %d is dennied\n",
             EmbezzledPort->Owner->Name.c_str(),
            EmbezzledPort->Name.c_str(), type);
        throw VBERR_BADPARAM;
    }

    BufferType = type;
    BufferCount = count;
}

void Port::SetBufferType(FRBuffer::Type type, int float_size, int float_max)
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(type == FRBuffer::Type::VACANT || type == FRBuffer::Type::FIXED)
        throw "port type with wrong parameters";

    if(EmbezzledPort) {
        LOGE("embezzling another port(%s-%s), attempt"
            " to set buffer type as %d is dennied\n",
            EmbezzledPort->Owner->Name.c_str(),
            EmbezzledPort->Name.c_str(), type);
        throw VBERR_BADPARAM;
    }

    BufferType = type;
    FloatSize = float_size;
    FloatMax = float_max;
}

FRBuffer::Type Port::GetBufferType() { return BufferType; }
int Port::GetBufferCount() { return BufferCount; }
int Port::GetFloatSize() { return FloatSize; }
int Port::GetFloatMax() { return FloatMax; }

int Port::CheckBufferRdy(int serial)
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(!frBuffer) {
        LOGE("no buffer\n");
        return -1;
    }
    if(Direction == Port::In)
    {
        return frBuffer->CheckBufferRdy(serial);
    }
    else
        return 1;
}


void Port::AllocateFRBuffer() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    std::string name = Owner->Name + "-" + Name;
    int size = 0;

    if(frBuffer) {
        LOGD("frBuffer is already allocated\n");
        return ;
    }

    if(BufferType == FRBuffer::Type::FLOAT ||
        BufferType == FRBuffer::Type::FLOAT_NODROP)
        frBuffer = new FRBuffer(name, BufferType, FloatSize, FloatMax,1);
    else {
        size = Width * Height * Pixel::Bits(PixelFormat) / 8;
        frBuffer = new FRBuffer(name, BufferType, BufferCount, size,1);
    }
    LOGD("alloc fr: type: %d, fix: %d * %d, float: 0x%x/0x%x\n",
        BufferType, size, BufferCount, FloatSize, FloatMax);

    if(LinkMode) {
        Buffer b = frBuffer->GetBuffer();
        b.fr_buf.phys_addr = Port::LinkGenPhys();
        frBuffer->PutBuffer(&b);
        LOGD("set link phys to: %08x\n", b.fr_buf.phys_addr);
    }
    if (m_iTimeoutInMs)
    {
        frBuffer->SetFRTimeout(m_iTimeoutInMs);
    }
}


void Port::FreeVirtualResource()
{
    if(frBuffer)
    {
        frBuffer->FreeVirtualResource();
    }
}

void Port::FreeFRBuffer() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(frBuffer) delete frBuffer;
    frBuffer = NULL;
}

Buffer Port::GetBuffer(Buffer *buf) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    Buffer rbuf = *buf;
    if(!frBuffer) {
        LOGE("no buffer\n");
        throw VBERR_FR_HANDLE;
    }

again:
    if(EmbezzledPort) {
        rbuf = EmbezzledPort->frBuffer->GetReferenceBuffer(&rbuf);
    } else if(Direction == Port::Out) {
        return frBuffer->GetBuffer();
    } else
        rbuf = frBuffer->GetReferenceBuffer(&rbuf);

    if(CheckSkipper()) {
        PutBuffer(&rbuf);
        goto again;
    }

    return rbuf;
}

Buffer Port::GetBuffer() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(!frBuffer) {
        LOGE("no buffer\n");
        throw VBERR_FR_HANDLE;
    }
    if(EmbezzledPort)
        throw VBERR_FR_HANDLE;

    if(LinkMode) {
        // if LinkMode, put buffer back imediately
        // to enable the next module to get buffer
        Buffer b = frBuffer->GetBuffer();
        frBuffer->PutBuffer(&b);
        return b;
    }

    return Direction == Port::In?
        frBuffer->GetReferenceBuffer(NULL):
        frBuffer->GetBuffer();
}

void Port::FlushBuffer() {
	frBuffer->FlushBuffer();
}

void Port::InvBuffer() {
	frBuffer->InvBuffer();
}

void Port::PutBuffer(Buffer *buf) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(!frBuffer) {
        LOGE("no buffer\n");
        throw VBERR_FR_HANDLE;
    }
    if(EmbezzledPort) {
        Buffer out = frBuffer->GetBuffer();
        out.SyncStamp(buf);
        out.fr_buf.phys_addr = buf->fr_buf.phys_addr;
        out.fr_buf.priv = buf->fr_buf.priv;
        frBuffer->PutBuffer(&out);
        EmbezzledPort->frBuffer->PutReferenceBuffer(buf);
    } else if(Direction == Port::Out) {
        frBuffer->PutBuffer(buf);
    } else
        frBuffer->PutReferenceBuffer(buf);
}

void Port::SetFRBufferTimeout(int iTimeoutInMs)
{
    m_iTimeoutInMs = iTimeoutInMs;
}

void Port::_Bind(Port *p)
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    BindPorts.push_back(p);
}

void Port::Export()
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    Exported = true;
    LOGD("exported\n");
}

bool Port::IsExported()
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return Exported;
}

void Port::Bind(Port *out)
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    LOGD("bind with %s-%s\n", out->Owner->Name.c_str(),
        out->Name.c_str());

    // Check direction
    if(this->Direction != Port::Out
            || out->Direction != Port::In) {
        LOGE("binding with wrong direction: %d->%d\n",
            this->Direction, out->Direction);
      throw VBERR_BADPARAM;
    }

    _Bind(out);
    out->_Bind(this);
}

void Port::Unbind()
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(this->Direction != Port::In)
        throw "Undind only works for In port";

    Port *out = BindPorts.front();
    BindPorts.clear();
    for(PortIterator bp = out->BindPorts.begin();
        bp != out->BindPorts.end();)
            if((*bp) == this)
                bp = out->BindPorts.erase(bp);
            else bp++;
}

int Port::IsBind()
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return BindPorts.size();
}

std::vector<Port*>* Port::GetBindPorts(){
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return &BindPorts;
}

std::vector<Port*>* Port::GetEmbezzlePorts(){
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return &EmbezzlePorts;
}

void Port::Embezzle(Port *port) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(Direction == Port::In)
        throw VBERR_BADPARAM;

    if(frBuffer) {
        LOGE("Cannot embezzle a port after frBuffer allocated\n");
        throw VBERR_BADPARAM;
    }
    EmbezzledPort = port;
    port->EmbezzlePorts.push_back(this);
    BufferType = FRBuffer::Type::VACANT;
}

bool Port::IsEmbezzling() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return !!EmbezzledPort;
}

bool Port::IsEmbezzled() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return !EmbezzlePorts.empty();
}

bool Port::Compatible(Port *port) {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return ((Width == port->GetWidth()) &&
                    (Height == port->GetHeight()) &&
                    (PixelFormat == port->GetPixelFormat()));
}

void Port::SyncEmbezzlement() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    for(PortIterator bp = EmbezzlePorts.begin();
        bp != EmbezzlePorts.end(); bp++) {
            (*bp)->SetResolution(Width, Height);
            (*bp)->SetPixelFormat(PixelFormat);
            (*bp)->SetFPS(FPS);
    }
}

void Port::SyncBinding(Port *p)
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    p->SetResolution(Width, Height);
    p->SetPixelFormat(PixelFormat);
    p->SetFPS(FPS);
    p->frBuffer = frBuffer;
}

void Port::SyncBinding() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    for(PortIterator bp = BindPorts.begin();
        bp != BindPorts.end(); bp++)
            SyncBinding(*bp);
}

IPU* Port::GetOwner() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return Owner;
}

void Port::_Enable() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    EnableCount = 1;
}

void Port::Enable() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(EnableCount++) {
        LOGD("not enabled EC:%d\n", EnableCount);
        return ;
    }

    if(Direction == Port::Out)
        Owner->RequestOn(0);
    else if(IsBind()) BindPorts.front()->Enable();
}

int Port::GetEnableCount()
{
    int s32EnCount = 0;
    for(PortIterator bp = BindPorts.begin();
                bp != BindPorts.end(); bp++)
        if((*bp)->IsEnabled())
        {
            s32EnCount++;
        }
    return s32EnCount;
}

void Port::Disable() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    if(EnableCount != 1) {
        EnableCount -= !!EnableCount;
        LOGD("not disabled EC:%d\n", EnableCount);
        return;
    }

    if(Direction == Port::Out) {
        for(PortIterator bp = BindPorts.begin();
            bp != BindPorts.end(); bp++)
            if((*bp)->IsEnabled()) {
                LOGE("remains enabled (%s-%s is working)\n",
                (*bp)->GetOwner()->Name.c_str(), (*bp)->GetName().c_str());
                return ;
            }
        EnableCount -= !!EnableCount;
        LOGE("disabled (EC:%d)\n", EnableCount);
        try {    Owner->RequestOff(0);
        } catch (const char *err) {}
    } else {
        EnableCount -= !!EnableCount;
        LOGE("disabled (EC:%d)\n", EnableCount);
        if(IsBind()) BindPorts.front()->Disable();
    }
}

bool Port::IsEnabled() { return !!EnableCount;}

void Port::LinkRequest()
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    LinkMTX.lock();
}

void Port::LinkRelease() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
}
void Port::LinkReady()
{
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    LinkMTX.unlock();
}

//void Port::LinkDisable() { LinkMode = 0;}
int Port::LinkEnabled() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    return !!LinkMode;
}
void Port::LinkEnable() {
    PORT_TRACE(printf("trace_port: %s, %d, own: %s, port: %s\n",
        __FUNCTION__, __LINE__, Owner->Name.c_str(), Name.c_str()));
    LinkMode = 1;
    SetBufferType(FRBuffer::Type::VACANT, 1);
    LOGD("link mode enabled\n");
}

// This is an lazy work around for link mode.
// Link mode takes space above 0x80000000 for physical addresses,
// so here we implement an easy algorithm to allocate this kind
// of physical addresses for link mode. Each address allocated increases
// by 16MB globally.
// As there are not two many link requirements, so this work around should
// not do any harm.
uint32_t Port::LinkPhys = 0x94270000;
uint32_t Port::LinkGenPhys(void) {
    LinkPhys += 0x1000000;
    if(LinkPhys < 0x80000000) {
        ::LOGE("LinkPhys wraps to below 0x80000000: (%08x)\n", LinkPhys);
        throw VBERR_BADLOGIC;
    }
    return LinkPhys;
}

void Port::LOGE(const char *fmt, ...)
{    ::LOGE("<%s-%s>: ", Owner? Owner->Name.c_str(): "|", Name.c_str());

    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    fflush(stdout);
}
