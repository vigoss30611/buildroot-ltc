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

#ifndef IPU_DGP_H
#define IPU_DGP_H
#include <IPU.h>
#include "font.h"
class IPU_DGPixel: public IPU {

private:

public:
    int FPS;
    int Delay;

    int fd;
    int loc_x;
    int loc_y;
    std::string Path;

    const struct font_desc *ft;
    Pixel Color[3];

    IPU_DGPixel(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);

    int SetFPS(int fps);
    void BlackScreen(char *, Port*);
    void DrawFont(char *, char, Port*, Pixel);
};

#endif // IPU_DGP_H
