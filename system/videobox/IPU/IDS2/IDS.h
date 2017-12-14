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


/*  Four-character-code (FOURCC) */
#define v4l2_fourcc(a, b, c, d)\
	((__u32)(a) | ((__u32)(b) << 8) | ((__u32)(c) << 16) | ((__u32)(d) << 24))

#define V4L2_PIX_FMT_RGB565  v4l2_fourcc('R', 'G', 'B', 'P') /* 16  RGB-5-6-5     */
#define V4L2_PIX_FMT_BGR24   v4l2_fourcc('B', 'G', 'R', '3') /* 24  BGR-8-8-8     */
#define V4L2_PIX_FMT_RGB24   v4l2_fourcc('R', 'G', 'B', '3') /* 24  RGB-8-8-8     */
#define V4L2_PIX_FMT_BGR32   v4l2_fourcc('B', 'G', 'R', '4') /* 32  BGR-8-8-8-8   */
#define V4L2_PIX_FMT_RGB32   v4l2_fourcc('R', 'G', 'B', '4') /* 32  RGB-8-8-8-8   */
#define V4L2_PIX_FMT_NV12    v4l2_fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
#define V4L2_PIX_FMT_NV21    v4l2_fourcc('N', 'V', '2', '1') /* 12  Y/CrCb 4:2:0  */

#define OVERLAY_NUM         2


struct overlay_info {
    uint32_t w;
    uint32_t h;
    uint32_t x;
    uint32_t y;
    uint32_t chroma_key_enable;      // chroma key is mutual exclusion with alpha
    uint32_t chroma_key_value;
    uint32_t chroma_alpha_nonkey_area;
    uint32_t chroma_alpha_key_area;
    uint32_t alpha_type;
    uint32_t alpha_value;
    int fd;
    Buffer buffer;
};

class IPU_IDS: public IPU {

private:

    struct overlay_info overlay[2];
    uint32_t osd_w;
    uint32_t osd_h;
    uint32_t background;
    uint32_t mapcolor;
    uint32_t mapcolor_enable;
    int enfb;
    int blank(int);

public:

    IPU_IDS(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);
};

#endif // IPU_IDS_H
