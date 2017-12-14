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

#ifndef FRBuffer_H
#define FRBuffer_H

#include <vector>
#include <string>
#include <functional>
#include <mutex>

#include <fr/libfr.h>

#define MAX_MAP_BUFFER_NUMER 16

class Buffer {
public:
    struct fr_buf_info fr_buf;
    Buffer() {
        fr_buf.buf = 0;
    }
    void Stamp();
    void Stamp(uint64_t timestamp);
    void SyncStamp(Buffer *from);
};

class FRBuffer {
public:
    enum Type {
        FIXED,
        FIXED_NODROP,
        FLOAT,
        FLOAT_NODROP,
        VACANT,
    };

private:
    struct fr_info fr;
    int m_s32Flag;
    int m_s32Size;
    uint32_t m_u32PhyBassAddr;
    int m_s32BufferMapNumber;
    ST_BUFFER_MAP_INFO m_stBufferMapInfo[MAX_MAP_BUFFER_NUMER];//float buffer use index 0
    std::mutex *m_pMutexLock;
    int GetVirtualAddr(struct fr_buf_info *p_stFrBufferInfo, bool bReferenceBuffer);

public:

/*
 * @constructor of class FRBuffer
 * @param name -IN: buffer name that used for ID.
 * @param type -IN: buffer type such as float. vancant or fixed.
 * @param para0 -IN: total buffer size when type is float, buffer number when type is fixed or vancant
 * @param para1 -IN: max buffer size of each one(float_max)when type is float, buffer size of each one when type is fixed or vancant
 * @param para2 -IN: enable new mmap/unmap scheme or not  1:enable 0:disable default disable
 * @return NONE
*/
    FRBuffer(std::string name, FRBuffer::Type type, int para0, int para1, int para2 = 0);
    ~FRBuffer();
    int CheckBufferRdy(int serial);
    Buffer GetBuffer();
    void PutBuffer(Buffer*);
    void FlushBuffer();
    void InvBuffer();
    Buffer GetReferenceBuffer(Buffer*) __attribute__((warn_unused_result));
    void PutReferenceBuffer(Buffer*);
    int SetFloatMax(int max);
    void FreeVirtualResource(int s32Num = 0);
    void SetFRTimeout(int timeout_in_ms);
    std::function<int (Buffer*)> m_releaseBufferCB;
    void RegCbFunc(std::function<int (Buffer*)> pFunc);
};

#endif // FRBuffer_H
