// Copyright (C) 2016 InfoTM, peter.fu@infotm.com
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

#ifndef IPU_V4L2_H
#define IPU_V4L2_H
#include <IPU.h>
#include "videodev2.h"

#define V4L2_MAX_CAM_NUM    4
#define V4L2_MAX_BUF_NUM     6

struct V4l2Buffer {
    void      *virt_addr;
    uint32_t  phys_addr;
    int size;
};

typedef enum _v4l2_cam_run_status {
    V4L2_CAM_RUN_IDLE,
    V4L2_CAM_RUN_CAPTURING,
    V4L2_CAM_RUN_PREVIEWING,
    V4L2_CAM_RUN_CAPTURED,
    V4L2_CAM_RUN_MAX
}E_V4L2_CAM_RUN_STATUS;

class IPU_V4L2:public IPU {

public:
    int nCamfd;
    unsigned int nBuffers;
    unsigned int nMaxFrameBuffers;
    FRBuffer *Vbuffer;
    Buffer cBuf;
    volatile int RunSt;
    unsigned int nAllocBufST;
    struct V4l2Buffer sBuffer[V4L2_MAX_BUF_NUM];
    IPU_V4L2(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int SetResolution(int nimgH, int imgW);
    int UnitControl(std::string, void *);
    virtual ~IPU_V4L2();
private:
    const char *pDrvName;
    const char *pModelName;
    const char *pPixelFmt;
    char *pDevPath;
    //std::string DevPath;
    Pixel::Format ePixFmt;
    unsigned int nImgWidth;
    unsigned int nImgHeight;
    int nFrameSize;
    unsigned int nMaxImgWidth;
    unsigned int nMaxImgHeight;
    int nMaxFrameSize;
    unsigned int GetV4l2PixFmt(Pixel::Format pixf);
    int V4l2InitDevice(void);
    int V4l2InitFmt(void);
    int V4l2InitBuffer(void);
    int V4l2Deinit(void);
    int V4l2Start(void);
    int V4l2Stop(void);
    int V4L2QueryCtrl(struct v4l2_queryctrl *queryctrl);
    int V4L2GetCtrl(struct v4l2_control *ctrl);
    int V4L2SetCtrl(struct v4l2_control *ctrl);
    int V4L2GetBrightness(void);
    int V4L2SetBrightness(int brightness);
    int V4L2TakePicture();
    int V4L2StopPicture();
};

#endif
