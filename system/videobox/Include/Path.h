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

#ifndef Path_H
#define Path_H

#include <vector>
#include <list>
#include <Json.h>
#include <qsdk/videobox.h>
#include <IPU.h>

typedef std::vector<IPU*>::iterator IpuListIterator;
typedef std::vector<IPU*>::reverse_iterator IpuListIteratorReverse;
extern void KillHandler(int signo);

class Path {

private:
    std::vector<IPU*> IpuList;
    std::string Error;
    std::string JsonPath;
    Json *RootJson;
    Path *RootPath;

    void GetVencInfo(void *pArg);
    void GetISPInfo(void *pArg);
    void Trigger();
    void Suspend();

public:
    void SetError(std::string);
    void SetJsonPath(std::string);
    const char* GetError();
    void Config(IPU *ipu, Json *ports_js);
    void Build(Json *js);
    void Rebuild(Json *js);
    void On();
    void Off();
    int SetPathPointer(Path *path);
    int SetJsonPointer(Json *js);
    int PathControl(std::string, std::string, void *);

    void AddIPU(IPU *ipu);
    IPU* GetIPU(std::string name);
    Port* GetPort(std::string port, IPU** pipu);
    Path& operator<<(IPU&);

    bool DynamicFindPath(std::list<Port*> &plist, int);
    void DynamicEnablePath(std::list<Port*> &plist, int);
    void DynamicDisablePath(std::list<Port*> &plist, int);
    void SyncPathFps(int fps);
    int VideoExchangeInfo(void *pArg);
};

typedef struct {
    char func[VB_CHN_LEN];
    char chn[VB_CHN_LEN];
    int ret;
    char arg[4];
} PathRPC;

#endif // Path_H
