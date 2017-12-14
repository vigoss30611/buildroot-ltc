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

#ifndef IPU_DOV_H
#define IPU_DOV_H
#include <IPU.h>

typedef struct _dg_overlay_info {
    char *pAddr; // YUV buffer address
    unsigned int iPaletteTable[4];
    int width;
    int height;
    int disable;
}DG_OVERLAY_INFO;

class IPU_DGOverlay: public IPU {

private:
    int Mode, Frames;
    int Palette(Port *, int, int);
    int Fill_Buffer(char *buf, int index);
    int Fill_BPP2_Buffer(char *buf, int index);
    DG_OVERLAY_INFO ov_info;
public:

    enum {
        PALETTE = 0,
        PICTRUE,
        VACANT
    };

    enum {
        OV_BPP2 = 0,
        OV_NV12,
    };

    int FPS;
    int Delay;
    Pixel Color[6];
    std::string Path;
    int Format;
    int ColorbarMode;

    IPU_DGOverlay(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);
};

#endif // IPU_DOV_H