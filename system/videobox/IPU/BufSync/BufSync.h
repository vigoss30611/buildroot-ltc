// Copyright (C) 2016 InfoTM
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

#ifndef IPU_BUFSYNC_H
#define IPU_BUFSYNC_H


#include <list>
#include <IPU.h>


enum
{
    RETURN_FLAG_0,
    RETURN_FLAG_1,
    RETURN_FLAG_MAX
};

typedef struct
{
    struct fr_buf_info stFrBuf;
    int refCount;
    int flag0;
    int flag1;
}LOOP_BUF_INFO;

class IPU_BUFSYNC: public IPU
{
private:
    Port *m_pPortIn;
    Port *m_pPortOut0, *m_pPortOut1;
    Buffer m_BufIn, m_BufOut0, m_BufOut1;
    int m_Buffers;
    std::function<int (Buffer*)> m_pBufFuncOut0;
    std::function<int (Buffer*)> m_pBufFuncOut1;
    std::list<LOOP_BUF_INFO> m_stInBufList;
    pthread_mutex_t m_BufLock;
    int m_RefOutPorts;
    int m_RefSerial;


    int ReturnBuf0(Buffer *pstBuf);
    int ReturnBuf1(Buffer *pstBuf);
    int PutInBuf(Buffer *pstBuf, int flag);
public:
    IPU_BUFSYNC(std::string name, Json* js);
    ~IPU_BUFSYNC();
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string func, void* arg);
};

#endif
