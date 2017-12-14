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

#ifndef IPU_IDS_H
#define IPU_IDS_H
#include <IPU.h>
#include "fb.h"

#define IMAPFB_CONFIG_VIDEO_SIZE   20005
#define IMAPFB_LCD_SUSPEND         20006
#define IMAPFB_LCD_UNSUSPEND       20007
struct imapfb_info {
    uint32_t width;
    uint32_t height;
    uint32_t win_width;
    uint32_t win_height;
    uint32_t crop_x;
    uint32_t crop_y;
    uint32_t scaler_buff;
    int format;
};

struct crop_info {
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
};

class IPU_IDS: public IPU {

private:
    struct imapfb_info suspend_info;
    void set_osd0(int fd, uint32_t addr, uint32_t w, uint32_t h,
        struct crop_info *crop, Pixel::Format pixf);

public:
    int fd_osd0;
    int fd_osd1;
    bool no_osd1;

    IPU_IDS(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);
    Buffer m_PreBuf;
};

#endif // IPU_IDS_H
