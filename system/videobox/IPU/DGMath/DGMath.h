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

#ifndef IPU_DGM_H
#define IPU_DGM_H
#include <IPU.h>

struct Drawing {
    const char *Name;
    int (*GetOffset)(int t, int h);
    void (*Draw)(char *buf, int w, int h, int i, int j);
};

class IPU_DGMath: public IPU {

private:

public:
    int Delay;

    IPU_DGMath(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);

    FRBuffer *Canvas;
    struct Drawing *D;

    int GetDrawingCount();
    Drawing *GetDrawingByID(int n);
    Drawing *GetDrawingByName(std::string name);
};

#endif // IPU_DGM_H
