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

#ifndef IPU_DGF_H
#define IPU_DGF_H
#include <IPU.h>

class IPU_DGFrame: public IPU {

private:
    int Mode, Frames;
    int Palette(Port *, int, int);
    int Fill_Buffer(char *buf, int index);
    long file_size;
    FILE *fp;

public:

    enum {
        PALETTE = 0,
        PICTRUE,
        YUVFILE,
        VACANT
    };
    int FPS;
    int Delay;
    Pixel Color[6];
    std::string Path;

    IPU_DGFrame(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);
};

#endif // IPU_DGF_H
