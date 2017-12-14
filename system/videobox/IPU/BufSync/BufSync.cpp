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
#include "BufSync.h"


DYNAMIC_IPU(IPU_BUFSYNC, "bufsync");

void IPU_BUFSYNC::WorkLoop()
{
    LOOP_BUF_INFO stLoopBuf;
    typedef struct _LOOP_LIST
    {
        Port *pPort;
        Buffer *pBuf;
    }LOOP_LIST;

    LOOP_LIST stList[] =
    {
        {m_pPortOut0, &m_BufOut0},
        {m_pPortOut1, &m_BufOut1},
        {NULL, NULL}
    };
    int count = sizeof(stList) / sizeof(stList[0]) - 1;
    int i = 0;


    while(IsInState(ST::Running))
    {
        try
        {
            m_BufIn = m_pPortIn->GetBuffer(&m_BufIn);
        }
        catch (const char* err)
        {
            usleep(20000);
            continue;
        }

        for (i = 0; i < count; ++i)
        {
            if (stList[i].pPort->IsEnabled())
                *(stList[i].pBuf) = stList[i].pPort->GetBuffer();
        }

        for (i = 0; i < count; ++i)
        {
            if (stList[i].pPort->IsEnabled())
                (*(stList[i].pBuf)).fr_buf.phys_addr = m_BufIn.fr_buf.phys_addr;
        }

        pthread_mutex_lock(&m_BufLock);
        stLoopBuf.stFrBuf = m_BufIn.fr_buf;
        stLoopBuf.refCount = 0;
        stLoopBuf.flag0 = 0;
        stLoopBuf.flag1 = 0;
        m_stInBufList.push_back(stLoopBuf);
        pthread_mutex_unlock(&m_BufLock);

        for (i = 0; i < count; ++i)
        {
            if (stList[i].pPort->IsEnabled())
            {
                (*(stList[i].pBuf)).SyncStamp(&m_BufIn);
                stList[i].pPort->PutBuffer(stList[i].pBuf);
            }
        }
    }

    LOGD("(%s:L%d) thread exit \n", __func__, __LINE__);
}

IPU_BUFSYNC::IPU_BUFSYNC(std::string name, Json *js)
{
    Name = name;
    m_Buffers = 3;


    if(NULL != js)
    {
        if (js->GetObject("nbuffers"))
            m_Buffers = js->GetInt("nbuffers");
    }

    m_pPortIn = CreatePort("in", Port::In);
    m_pPortIn->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    m_pPortOut0 = CreatePort("out0", Port::Out);
    m_pPortOut0->SetBufferType(FRBuffer::Type::VACANT, m_Buffers);
    m_pPortOut0->SetResolution(640, 480);
    m_pPortOut0->SetPixelFormat(Pixel::Format::NV12);
    m_pPortOut0->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);


    m_pPortOut1 = CreatePort("out1", Port::Out);
    m_pPortOut1->SetBufferType(FRBuffer::Type::VACANT, m_Buffers);
    m_pPortOut1->SetResolution(640, 480);
    m_pPortOut1->SetPixelFormat(Pixel::Format::NV12);
    m_pPortOut1->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);

    pthread_mutex_init(&m_BufLock, NULL);
    m_RefOutPorts = 0;
    m_RefSerial = 0;
}

IPU_BUFSYNC::~IPU_BUFSYNC()
{
    pthread_mutex_destroy(&m_BufLock);
}

void IPU_BUFSYNC::Prepare()
{
    if (m_pPortOut0->IsEnabled())
    {
        if(m_pPortOut0->GetPixelFormat() != m_pPortIn->GetPixelFormat())
        {
            LOGE("In and Out0 Port Pixel Format Params not Match\n");
            throw VBERR_BADPARAM;
        }
        m_pBufFuncOut0 = std::bind(&IPU_BUFSYNC::ReturnBuf0,this,std::placeholders::_1);
        m_pPortOut0->frBuffer->RegCbFunc(m_pBufFuncOut0);
        ++m_RefOutPorts;
    }
    if (m_pPortOut1->IsEnabled())
    {
        if(m_pPortOut1->GetPixelFormat() != m_pPortIn->GetPixelFormat())
        {
             LOGE("In and Out1 Port Pixel Format Params not Match\n");
            throw VBERR_BADPARAM;
        }
        m_pBufFuncOut1 = std::bind(&IPU_BUFSYNC::ReturnBuf1,this,std::placeholders::_1);
        m_pPortOut1->frBuffer->RegCbFunc(m_pBufFuncOut1);
        ++m_RefOutPorts;
    }
    m_stInBufList.clear();
}

void IPU_BUFSYNC::Unprepare()
{
    Port *pIpuPort;

    pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("out0");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("out1");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }
}

int IPU_BUFSYNC::UnitControl(std::string func, void *arg)
{
    return 0;
}

int IPU_BUFSYNC::ReturnBuf0(Buffer *pstBuf)
{
    return PutInBuf(pstBuf, RETURN_FLAG_0);
}

int IPU_BUFSYNC::ReturnBuf1(Buffer *pstBuf)
{
    return PutInBuf(pstBuf, RETURN_FLAG_1);
}

int IPU_BUFSYNC::PutInBuf(Buffer *pstBuf, int flag)
{
    Buffer stBuf;
    std::list<LOOP_BUF_INFO>::reverse_iterator rit;


    if (NULL == pstBuf)
    {
        LOGE("(%s:L%d) param is NULL !!!\n", __func__, __LINE__);
        return -1;
    }
    pthread_mutex_lock(&m_BufLock);
    if (m_stInBufList.empty())
    {
        pthread_mutex_unlock(&m_BufLock);
        return 0;
    }

    LOGD("(%s:L%d)  pstBuf serial=%d, phys=%p \n", __func__, __LINE__,\
        pstBuf->fr_buf.serial, pstBuf->fr_buf.phys_addr);

    for (rit = m_stInBufList.rbegin(); rit != m_stInBufList.rend(); )
    {
        LOGD("(%s:L%d)  stBuf serial=%d, phys=%p refCnt=%d m_Serial=%d \n", __func__, __LINE__,\
            rit->stFrBuf.serial, rit->stFrBuf.phys_addr, rit->refCount, m_RefSerial);

        if (rit->stFrBuf.phys_addr == pstBuf->fr_buf.phys_addr)
        {
            if (0 == rit->flag0 && flag == RETURN_FLAG_0)
            {
                rit->flag0 = 1;
                ++(rit->refCount);
            }
            if (0 == rit->flag1 && flag == RETURN_FLAG_1)
            {
                rit->flag1 = 1;
                ++(rit->refCount);
            }
            if (m_RefOutPorts == rit->refCount)
            {
                stBuf.fr_buf = rit->stFrBuf;
                m_pPortIn->PutBuffer(&stBuf);
                m_RefSerial = rit->stFrBuf.serial;
                rit = std::list<LOOP_BUF_INFO>::reverse_iterator(m_stInBufList.erase((++rit).base()));
            }
            else
            {
                ++rit;
            }
        }
        else if (rit->stFrBuf.serial < m_RefSerial)
        {
            LOGD("(%s:L%d) delete serial=%d, phys=%p, m_RefSerial=%d \n", __func__, __LINE__,\
                    rit->stFrBuf.serial, rit->stFrBuf.phys_addr, m_RefSerial);
            stBuf.fr_buf = rit->stFrBuf;
            m_pPortIn->PutBuffer(&stBuf);
            rit = std::list<LOOP_BUF_INFO>::reverse_iterator(m_stInBufList.erase((++rit).base()));
        }
        else
        {
            ++rit;
        }
    }
    pthread_mutex_unlock(&m_BufLock);

    return 0;
}
