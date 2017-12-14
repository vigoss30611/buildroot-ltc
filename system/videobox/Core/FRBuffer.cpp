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
#include <memory.h>
#include <FRBuffer.h>

FRBuffer::FRBuffer(std::string name, FRBuffer::Type type,
    int para0, int para1, int para2) {

    // for FIXED and VACANT para0 = BufferCount, para1 = size
    // for FLOAT para0 = size, para1 = max
    int ret, size = para1, flag = 0;

    switch(type) {
        case Type::FLOAT_NODROP:
            flag |= FR_FLAG_NODROP;
        case Type::FLOAT:
            size = para0;
            flag |= FR_FLAG_FLOAT | FR_FLAG_MAX(para1);
            break;
        case Type::VACANT:
            flag |= FR_FLAG_VACANT;
            flag |= FR_FLAG_PROTECTED;
            flag |= FR_FLAG_RING(para0);
            break;
        case Type::FIXED_NODROP:
            flag |= FR_FLAG_NODROP;
        default:
            flag |= FR_FLAG_RING(para0);
            flag |= FR_FLAG_PROTECTED;
    }
    m_releaseBufferCB = NULL;


    LOGD("allocate: %x, %x, %x, %x\n", size, flag, para0, para1);
    ret = fr_alloc(&fr, name.c_str(), size, flag);
    if(ret < 0) throw VBERR_FR_ALLOC;
    m_s32Flag = flag;
    m_pMutexLock = NULL;
    memset(m_stBufferMapInfo, 0, sizeof(m_stBufferMapInfo));
    if(para2)
    {
        m_s32Flag |= FR_FLAG_NEWMAPVIRTUAL;
        m_s32Size = size;
        m_u32PhyBassAddr = fr.u32BasePhyAddr;
        m_s32BufferMapNumber = 0;
        m_pMutexLock =  new std::mutex();
    }
}

FRBuffer::~FRBuffer() {
    if(m_s32Flag & FR_FLAG_NEWMAPVIRTUAL)
    {
        FreeVirtualResource();
    }
    m_s32Flag = 0;
    if(m_pMutexLock)
       delete m_pMutexLock;
    memset(m_stBufferMapInfo, 0, sizeof(m_stBufferMapInfo));
    fr_free(&fr);
}

int FRBuffer::SetFloatMax(int max){
    return fr_float_setmax(&fr, max);
}

static void throw_fr_exception(int ret)
{
    switch(ret) {
        case FR_EHANDLE:
            throw VBERR_FR_HANDLE;
        case FR_EWIPED:
            throw VBERR_FR_WIPED;
        case FR_ENOBUFFER:
            throw VBERR_FR_NOBUFFER;
        case FR_ERESTARTSYS:
            throw VBERR_FR_INTERRUPTED;
        case FR_EINVOPR:
            throw VBERR_FR_INVAL;
        case FR_TIMEOUT:
            LOGE("Interuppted by timeout\n");
            throw VBERR_FR_INTERRUPTED;
        case FR_MAPERROR:
            throw VBERR_FR_MAPERROR;
        default:
            throw VBERR_FR_UNKNOWN;
    }
}

int FRBuffer::CheckBufferRdy(int serial)
{
    Buffer buf;
    int ret = 0;
    fr_INITBUF(&buf.fr_buf, &fr);
    buf.fr_buf.serial = serial;
    ret = fr_test_new(&buf.fr_buf);
    return ret;
}

int FRBuffer::GetVirtualAddr(struct fr_buf_info *p_stFrBufferInfo, bool bReferenceBuffer)
{
    if(m_s32Flag & FR_FLAG_NEWMAPVIRTUAL)
    {
        if(m_s32Flag & FR_FLAG_FLOAT)
        {
            if(p_stFrBufferInfo->phys_addr < m_u32PhyBassAddr || p_stFrBufferInfo->phys_addr > (m_u32PhyBassAddr +  m_s32Size))
            {
                if(p_stFrBufferInfo->u32BasePhyAddr == m_u32PhyBassAddr &&  p_stFrBufferInfo->s32TotalSize == m_s32Size)
                {
                    LOGE("fatal error: float buffer phy address overflow phy addr:%x %x size:%x)\n", p_stFrBufferInfo->phys_addr, m_u32PhyBassAddr, m_s32Size);
                    p_stFrBufferInfo->virt_addr = NULL;
                    return -1;
                }
                else if(m_stBufferMapInfo[0].u8status == 1)
                {
                    LOGE("float buffer is change remap float buffer (phy size)(0x%x 0x%x)->(0x%x 0x%x) \n",
                    m_u32PhyBassAddr, m_s32Size, p_stFrBufferInfo->u32BasePhyAddr,p_stFrBufferInfo->s32TotalSize);
                    m_u32PhyBassAddr = p_stFrBufferInfo->u32BasePhyAddr;
                    m_s32Size = p_stFrBufferInfo->s32TotalSize;
                    m_stBufferMapInfo[0].u32PhyAddr = 0;
                }
            }

            //map base other than the phy addr that first time get because of page offset
            if(m_stBufferMapInfo[0].u32PhyAddr <  m_u32PhyBassAddr)
            {
                m_pMutexLock->lock();
                if(m_stBufferMapInfo[0].u8status == 1)
                {
                    if(fr_munmap(m_stBufferMapInfo[0].pVirtualAddr, m_stBufferMapInfo[0].s32size) != 0)
                    {
                        LOGE("error: float buffer  fr_munmap virtual:0x%x size:%d\n", m_stBufferMapInfo[0].pVirtualAddr, m_stBufferMapInfo[0].s32size);
                    }
                    LOGD("func:%s line:%d fr_munmap stats:%d size:0x%x, VirtualAddr:%p PhyAddr:0x%x\n", __FUNCTION__, __LINE__, m_stBufferMapInfo[0].u8status,
                        m_stBufferMapInfo[0].s32size, m_stBufferMapInfo[0].pVirtualAddr, m_stBufferMapInfo[0].u32PhyAddr);

                    m_stBufferMapInfo[0].u8status = 0;
                    m_stBufferMapInfo[0].pVirtualAddr = NULL;
                    m_stBufferMapInfo[0].s32size = 0;
                }
                m_stBufferMapInfo[0].u32PhyAddr =  m_u32PhyBassAddr;
                m_stBufferMapInfo[0].s32size =  m_s32Size;
                m_stBufferMapInfo[0].pVirtualAddr =  fr_mmap(m_stBufferMapInfo[0].u32PhyAddr,m_s32Size);
                if (m_stBufferMapInfo[0].pVirtualAddr == (void *)-1)
                {
                    LOGE("fatal error: float buffer  fr_mmap error\n");
                    m_stBufferMapInfo[0].pVirtualAddr = NULL;
                    m_pMutexLock->unlock();
                    return -1;
                }
                m_stBufferMapInfo[0].u8status = 1;
                m_s32BufferMapNumber = 1;
                p_stFrBufferInfo->virt_addr = m_stBufferMapInfo[0].pVirtualAddr;
                LOGD("func:%s line:%d num:%d fr_mmap stats:%d size:0x%x, VirtualAddr:%p PhyAddr:0x%x\n", __FUNCTION__, __LINE__, m_s32BufferMapNumber,
                    m_stBufferMapInfo[0].u8status, m_stBufferMapInfo[0].s32size, m_stBufferMapInfo[0].pVirtualAddr, m_stBufferMapInfo[0].u32PhyAddr);
                m_pMutexLock->unlock();
            }
            else
            {
                p_stFrBufferInfo->virt_addr = (void *)((uint32_t)m_stBufferMapInfo[0].pVirtualAddr + p_stFrBufferInfo->phys_addr - m_stBufferMapInfo[0].u32PhyAddr);
            }
        }
        else if((m_s32Flag & FR_FLAG_VACANT) == 0 ||((m_s32Flag & FR_FLAG_VACANT) != 0 && bReferenceBuffer == true))
        {
             bool bNeedMap = true;
             if(p_stFrBufferInfo->phys_addr == 0)
             {
                 //special case may be send ctrl info other than data buffer, so return 0 other than -1;
                 p_stFrBufferInfo->virt_addr = NULL;
                 return 0;
             }
             for(int i = 0; i < m_s32BufferMapNumber; i++)
             {
                 if(p_stFrBufferInfo->phys_addr == m_stBufferMapInfo[i].u32PhyAddr)
                 {
                    if(p_stFrBufferInfo->map_size == m_stBufferMapInfo[i].s32size)
                    {
                        p_stFrBufferInfo->virt_addr = m_stBufferMapInfo[i].pVirtualAddr;
                        return 0;
                    }
                    else
                    {
                        //error handle or size change
                        LOGE("fixed buffer is change remap fixed buffer (phy size)(0x%x 0x%x)->(0x%x 0x%x) \n",
                            m_stBufferMapInfo[i].u32PhyAddr, m_stBufferMapInfo[i].s32size, p_stFrBufferInfo->phys_addr,p_stFrBufferInfo->map_size);
                        m_pMutexLock->lock();
                        if(fr_munmap(m_stBufferMapInfo[i].pVirtualAddr, m_stBufferMapInfo[i].s32size) != 0)
                        {
                             LOGE("error: fixed buffer fr_munmap error\n");
                        }
                        //put new mapped  at the end
                        for(int j  = i ; j < m_s32BufferMapNumber - 1; j++)
                        {
                            memcpy(&m_stBufferMapInfo[j], &m_stBufferMapInfo[j + 1], sizeof(ST_BUFFER_MAP_INFO));
                        }
                        i = m_s32BufferMapNumber - 1;
                        memset(&m_stBufferMapInfo[i], 0, sizeof(ST_BUFFER_MAP_INFO));
                        m_stBufferMapInfo[i].pVirtualAddr = fr_mmap(p_stFrBufferInfo->phys_addr, p_stFrBufferInfo->map_size);
                        if (m_stBufferMapInfo[i].pVirtualAddr == (void *)-1)
                        {
                            LOGE("fatal error: fixed buffer fr_mmap error\n");
                            m_s32BufferMapNumber--;
                            p_stFrBufferInfo->virt_addr = NULL;
                            m_pMutexLock->unlock();
                            return 0;
                        }
                        m_stBufferMapInfo[i].u32PhyAddr = p_stFrBufferInfo->phys_addr;
                        m_stBufferMapInfo[i].s32size = p_stFrBufferInfo->map_size;
                        m_stBufferMapInfo[i].u8status = 1;
                        p_stFrBufferInfo->virt_addr= m_stBufferMapInfo[i].pVirtualAddr;
                        m_pMutexLock->unlock();
                        return 0;
                    }
                }
             }
             if(bNeedMap == true)
             {
                 m_pMutexLock->lock();
                 if(m_s32BufferMapNumber >= MAX_MAP_BUFFER_NUMER)
                 {
                     /*
                         if overflow free pre-half becauseof we pue new mapped at the end
                         MAX_MAP_BUFFER_NUMER should be equal with actual used buffer number *2 at least when buffer change
                     */
                     LOGE("Warning: buffer overflow FreeVirtualResource/Reset First\n");
                     m_pMutexLock->unlock();
                     FreeVirtualResource(MAX_MAP_BUFFER_NUMER/2);
                     m_pMutexLock->lock();
                 }
                 m_stBufferMapInfo[m_s32BufferMapNumber].pVirtualAddr = fr_mmap(p_stFrBufferInfo->phys_addr,p_stFrBufferInfo->map_size);
                 if (m_stBufferMapInfo[m_s32BufferMapNumber].pVirtualAddr == (void *)-1)
                 {
                     LOGE("fatal error: fixed buffer fr_mmap error\n");
                     p_stFrBufferInfo->virt_addr = NULL;
                     m_pMutexLock->unlock();
                     return 0;
                 }
                 m_stBufferMapInfo[m_s32BufferMapNumber].u32PhyAddr = p_stFrBufferInfo->phys_addr;
                 m_stBufferMapInfo[m_s32BufferMapNumber].s32size = p_stFrBufferInfo->map_size;
                 m_stBufferMapInfo[m_s32BufferMapNumber].u8status = 1;
                 p_stFrBufferInfo->virt_addr = m_stBufferMapInfo[m_s32BufferMapNumber].pVirtualAddr;
                 LOGD("func:%s line:%d num:%d fr_mmap stats:%d size:0x%x, VirtualAddr:%p PhyAddr:0x%x\n", __FUNCTION__, __LINE__, m_s32BufferMapNumber + 1,
                    m_stBufferMapInfo[m_s32BufferMapNumber].u8status, m_stBufferMapInfo[m_s32BufferMapNumber].s32size,
                    m_stBufferMapInfo[m_s32BufferMapNumber].pVirtualAddr, m_stBufferMapInfo[m_s32BufferMapNumber].u32PhyAddr);

                 m_s32BufferMapNumber++;
                 m_pMutexLock->unlock();
             }
        }
    }
    return 0;
}

Buffer FRBuffer::GetBuffer()
{
    Buffer buf;
    int ret;
    int s32NewMapFlag = m_s32Flag & FR_FLAG_NEWMAPVIRTUAL;

    fr_INITBUF(&buf.fr_buf, &fr);
    if(s32NewMapFlag)
    {
        buf.fr_buf.flag |= FR_FLAG_NEWMAPVIRTUAL;
    }
    ret = fr_get_buf(&buf.fr_buf);
    if(ret == 0)
    {
        if(s32NewMapFlag && GetVirtualAddr(&buf.fr_buf,false) == -1)
        {
            ret = FR_MAPERROR;
        }
        else
        {
            return buf;
        }
    }
    throw_fr_exception(ret);
    return buf; // never reached
}

void FRBuffer::PutBuffer(Buffer *buf)
{
    if(m_s32Flag & FR_FLAG_NEWMAPVIRTUAL)
    {
        buf->fr_buf.flag |= FR_FLAG_NEWMAPVIRTUAL;
    }
    fr_put_buf(&buf->fr_buf);
}

/*
 * @constructor of class FRBuffer
 * @param s32Num -IN:  if s32Num = 0  or s32Num bigger than all mapped, free all
                                            0  < s32Num < mapped num free s32Num start from index 0
 * @return NONE
*/
void FRBuffer::FreeVirtualResource(int s32Num)
{
    int i = 0;

    if(m_s32Flag & FR_FLAG_NEWMAPVIRTUAL)
    {
        LOGD("FRBuffer::FreeVirtualResource() m_s32BufferMapNumber:%d\n", m_s32BufferMapNumber);
        m_pMutexLock->lock();

        if(s32Num == 0)
        {
            s32Num = m_s32BufferMapNumber;
        }
        else
        {
            s32Num = s32Num < m_s32BufferMapNumber ? s32Num : m_s32BufferMapNumber;
        }
        for(i = 0; i < s32Num; i++)
        {
            LOGD("func:%s line:%d num:%d fr_unmap stats:%d size:0x%x, VirtualAddr:%p PhyAddr:0x%x\n", __FUNCTION__, __LINE__, m_s32BufferMapNumber,
                m_stBufferMapInfo[i].u8status, m_stBufferMapInfo[i].s32size, m_stBufferMapInfo[i].pVirtualAddr, m_stBufferMapInfo[i].u32PhyAddr);
            if(m_stBufferMapInfo[i].u8status == 1 && m_stBufferMapInfo[i].pVirtualAddr != NULL && m_stBufferMapInfo[i].s32size > 0)
            {
                if(fr_munmap(m_stBufferMapInfo[i].pVirtualAddr,  m_stBufferMapInfo[i].s32size) != 0)
                {
                    LOGE("error: float buffer  fr_munmap virtual:0x%x size:%d\n", m_stBufferMapInfo[0].pVirtualAddr, m_stBufferMapInfo[0].s32size);
                }
                m_stBufferMapInfo[i].u8status = 0;
                m_stBufferMapInfo[i].pVirtualAddr = NULL;
                m_stBufferMapInfo[i].s32size = 0;
                m_stBufferMapInfo[i].u32PhyAddr = 0;
            }
        }
        m_s32BufferMapNumber -= s32Num;
        if(m_s32BufferMapNumber > 0)
        {
            for(i = 0; i < m_s32BufferMapNumber; i++)
                memcpy(&m_stBufferMapInfo[i], &m_stBufferMapInfo[s32Num + i], sizeof(ST_BUFFER_MAP_INFO));
        }
        m_pMutexLock->unlock();
    }
}

void FRBuffer::FlushBuffer(void)
{
    Buffer buf;
    fr_INITBUF(&buf.fr_buf, &fr);
    fr_flush_buf(&buf.fr_buf);
}

void FRBuffer::InvBuffer(void)
{
    Buffer buf;
    fr_INITBUF(&buf.fr_buf, &fr);
    fr_inv_buf(&buf.fr_buf);
}

Buffer __attribute__((warn_unused_result))
FRBuffer::GetReferenceBuffer(Buffer *ref)
{
    Buffer buf;
    int ret;
    int s32NewMapFlag = m_s32Flag & FR_FLAG_NEWMAPVIRTUAL;

    if(!ref || !ref->fr_buf.buf)
        fr_INITBUF(&buf.fr_buf, &fr);
    else
        buf = *ref;
    if(s32NewMapFlag)
    {
        buf.fr_buf.flag |= FR_FLAG_NEWMAPVIRTUAL;
    }
    ret = fr_get_ref(&buf.fr_buf);
    if(ret == 0)
    {
        if(s32NewMapFlag/*  && (m_s32Flag & FR_FLAG_FLOAT) == 0 */&& GetVirtualAddr(&buf.fr_buf, true) == -1)
        {
            ret = FR_MAPERROR;
        }
        else
        {
            return buf;
        }

    }

    throw_fr_exception(ret);
    return buf; // never reached
}

void FRBuffer::PutReferenceBuffer(Buffer *ref)
{
    if (m_releaseBufferCB != NULL)
        m_releaseBufferCB(ref);
    if(m_s32Flag & FR_FLAG_NEWMAPVIRTUAL)
    {
        ref->fr_buf.flag |= FR_FLAG_NEWMAPVIRTUAL;
    }
    fr_put_ref(&ref->fr_buf);
}

void FRBuffer::RegCbFunc(std::function<int (Buffer*)> pFunc)
{
    m_releaseBufferCB = pFunc;
}

void FRBuffer::SetFRTimeout(int timeout_in_ms)
{
    LOGE("set timeout: %d\n", timeout_in_ms);
    fr_set_timeout(&fr, timeout_in_ms);
}

void Buffer::Stamp()
{
    fr_STAMPBUF(&fr_buf);
}

void Buffer::Stamp(uint64_t timestamp)
{
    fr_buf.timestamp = timestamp;
}

void Buffer::SyncStamp(Buffer *from)
{
    fr_buf.timestamp = from->fr_buf.timestamp;
}

