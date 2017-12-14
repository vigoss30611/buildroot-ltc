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

#include <System.h>
#include <assert.h>
#include "V4L2.h"
#include <fstream>
DYNAMIC_IPU(IPU_V4L2, "v4l2");

#define V4L2_SKIP_FRAME (3)

pthread_mutex_t v4l2_mutex = PTHREAD_MUTEX_INITIALIZER;
using namespace std;
void IPU_V4L2::WorkLoop() {
    Buffer dst;
    Buffer BufDst;
    struct v4l2_buffer buf;
    int ret;
    int val = 0;
    int preSt = RunSt;
    unsigned int i, skip_fr = 0;
    Port *p = GetPort("out");
    Port *pPortCap = GetPort("cap");

    LOGD("WorkLoop() in.\n");
    //ofstream file;
    //file.open("/mnt/sd0/camera.yuv", ios::out | ios::binary);
    while(IsInState(ST::Running)) {
        fd_set fds;
        struct timeval tv;

        FD_ZERO(&fds);
        FD_SET(nCamfd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = (1000*1000); //worst case: FPS > 2 500ms

        if (RunSt == V4L2_CAM_RUN_CAPTURING) {
            if (preSt == V4L2_CAM_RUN_PREVIEWING) {
                this->V4l2Stop();

                if (pPortCap) {
                    this->nImgWidth = pPortCap->GetWidth();
                    this->nImgHeight = pPortCap->GetHeight();
                    this->ePixFmt = pPortCap->GetPixelFormat();
                }

                this->nBuffers = 4; //set default
                val = 0;
                val += this->V4l2InitFmt();
                val += this->V4l2InitBuffer();
                val += this->V4l2Start();
                if (val != 0) {
                    this->V4l2Deinit();
                    throw VBERR_RUNTIME;
                }
            }

            ret = select(nCamfd + 1, &fds, NULL, NULL, &tv);
            if (-1 == ret) {
                LOGE("select poll error\n");
                break;
            } else if(0 == ret) {
                LOGE("select poll timeout\n");
                break;
            }

            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            ret = ioctl (nCamfd, VIDIOC_DQBUF, &buf);
            if(ret < 0) {
                LOGE("VIDIOC_DQBUF failed.\n");
                break;
                /* TODO need do exceptional handle*/
            }

            for(i = 0; i < nBuffers; i++)
                if (buf.m.userptr == (unsigned long)sBuffer[i].virt_addr)
                    break;
            assert(i < nBuffers);

            if (skip_fr == V4L2_SKIP_FRAME) {
                BufDst = pPortCap->GetBuffer();
                BufDst.Stamp();
                BufDst.fr_buf.phys_addr = sBuffer[i].phys_addr;
                BufDst.fr_buf.map_size = sBuffer[i].size;
                BufDst.fr_buf.size = buf.bytesused;

                BufDst.fr_buf.virt_addr = (void *)buf.m.userptr;
                LOGE("%d dst buffer size:%d phy:0x%x.\n", __LINE__, BufDst.fr_buf.size, sBuffer[i].phys_addr);
                //file.write((char *)dst.fr_buf.virt_addr, dst.fr_buf.size);

                pPortCap->PutBuffer(&BufDst);
            }

            ret = ioctl (nCamfd, VIDIOC_QBUF, &buf);
            if(ret < 0) {
                LOGE("VIDIOC_QBUF failed.\n");
                break;
            }

            if (skip_fr < V4L2_SKIP_FRAME) {
                skip_fr++;
                preSt = RunSt;
            } else {
                this->V4l2Stop();
                preSt = RunSt;
                skip_fr = 0;
                RunSt = V4L2_CAM_RUN_CAPTURED;
            }
        } else if (RunSt == V4L2_CAM_RUN_CAPTURED) {
            usleep(16670);
        } else if (RunSt == V4L2_CAM_RUN_PREVIEWING) {
            if (preSt == V4L2_CAM_RUN_CAPTURING) {
                if (p) {
                    this -> nImgWidth = p->GetWidth();
                    this -> nImgHeight = p->GetHeight();
                    this->ePixFmt = p->GetPixelFormat();
                }

                this->nBuffers = 4; //set default
                val = 0;
                val += this->V4l2InitFmt();
                val += this->V4l2InitBuffer();
                val += this->V4l2Start();
                if (val != 0) {
                    this->V4l2Deinit();
                    throw VBERR_RUNTIME;
                }
                preSt = RunSt;
            }

            ret = select(nCamfd + 1, &fds, NULL, NULL, &tv);
            if (-1 == ret) {
                LOGE("select poll error\n");
                break;
            } else if(0 == ret) {
                LOGE("select poll timeout\n");
                break;
            }

            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_USERPTR;
            ret = ioctl (nCamfd, VIDIOC_DQBUF, &buf);
            if(ret < 0) {
                LOGE("VIDIOC_DQBUF failed.\n");
                break;
                /* TODO need do exceptional handle*/
            }

            for(i = 0; i < nBuffers; i++)
                if (buf.m.userptr == (unsigned long)sBuffer[i].virt_addr)
                    break;
            assert(i < nBuffers);

            dst = p->GetBuffer();
            dst.Stamp();
            dst.fr_buf.phys_addr = sBuffer[i].phys_addr;
            dst.fr_buf.map_size = sBuffer[i].size;
            dst.fr_buf.size = buf.bytesused;

            dst.fr_buf.virt_addr = (void *)buf.m.userptr;
            LOGD("%d dst buffer size:%d phy:0x%x.\n", __LINE__, dst.fr_buf.size, sBuffer[i].phys_addr);
            //file.write((char *)dst.fr_buf.virt_addr, dst.fr_buf.size);
            p->PutBuffer(&dst);
            ret = ioctl (nCamfd, VIDIOC_QBUF, &buf);
            if(ret < 0) {
                LOGE("VIDIOC_QBUF failed.\n");
                break;
                /* TODO need do exceptional handle*/
            }
        }
    }

    LOGD("V4L2 WorlLoop exit\n");
    return ;
}


IPU_V4L2::IPU_V4L2(std::string name, Json *js) {
    Name = name;
    LOGD("IPU_V4L2()++.\n");
    this->nBuffers = 4;
    this->nCamfd = 0;
    this->nImgWidth = 0;
    this->nImgHeight = 0;
    this->nFrameSize = 0;
    this->nMaxFrameBuffers = 3;
    this->nMaxImgWidth = 0;
    this->nMaxImgHeight = 0;
    this->nMaxFrameSize = 0;
    this->nAllocBufST = 0;
    RunSt = V4L2_CAM_RUN_PREVIEWING; //default
    this->ePixFmt = Pixel::Format::YUYV422;

    if(js != NULL) {
        pDrvName = js->GetString("driver");
        pModelName = js->GetString("model");
        pPixelFmt = js->GetString("pixfmt");
        if(pPixelFmt != NULL)
        {
            this->ePixFmt = Pixel::ParseString(this->pPixelFmt);
        }
    }

    Port *p = NULL, *pPortCap = NULL;
    p = CreatePort("out", Port::Out);
    if(NULL != p) {
        p->SetBufferType(FRBuffer::Type::VACANT, this->nBuffers);
        p->SetPixelFormat(this->ePixFmt);
    }

    pPortCap = CreatePort("cap", Port::Out);
    if (NULL != pPortCap)
    {
        pPortCap->SetBufferType(FRBuffer::Type::VACANT, this->nBuffers);
        pPortCap->SetPixelFormat(Pixel::Format::NV12);
        pPortCap->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_Dynamic);
    }
}

unsigned int IPU_V4L2::GetV4l2PixFmt(Pixel::Format pixf) {

    if(pixf == Pixel::Format::NV12)
        return V4L2_PIX_FMT_NV12;
    else if(pixf == Pixel::Format::NV21)
        return V4L2_PIX_FMT_NV21;
    else if(pixf == Pixel::Format::YUYV422)
        return V4L2_PIX_FMT_YUYV;
    else if(pixf == Pixel::Format::MJPEG)
        return V4L2_PIX_FMT_MJPEG;
    else if(pixf == Pixel::Format::H264ES)
        return V4L2_PIX_FMT_H264;
    else
        return V4L2_PIX_FMT_YUYV;
}


int IPU_V4L2::V4l2InitDevice() {
    struct v4l2_capability cap;
    unsigned int i;
    char devPath[16];
    int fd;
    int ret;


    LOGD("V4l2InitDevice() in.\n");
    /*lookup for a valid camera sensor*/
    for(i = 0; i < V4L2_MAX_CAM_NUM; i++) {
        sprintf(devPath, "/dev/video%d", i);
        fd = open(devPath, O_RDWR | O_NONBLOCK, 0);
        if(fd < 0 ) {
            LOGE("open %s failed\n", devPath);
            continue;
        }

        memset(&cap, 0, sizeof(cap));
        ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
        if(ret < 0) {
            LOGE("io-cmd VIDIOC_QUERYCAP failed\n");
            close(fd);
            continue;
        }


        if(strcmp((const char *)cap.card, this->pModelName)) {
            LOGE("[%s] drv:%s. card:%s. version-%d. capability-%8x\n",
                    devPath, cap.driver, cap.card, cap.version, cap.capabilities);
            LOGE("Path need: driver:%s. card:%s. Not match!!\n",
                    this->pDrvName, this->pModelName);
            close(fd);
            continue;
        }

#if 0
        if(strcmp((const char *)cap.driver, this->pDrvName)) {
            LOGE("[**IPU_V4L2**] dis-match a exist camera.\n");
            close(fd);
            continue;
        }
#endif
        break;
    }

    if (i >= V4L2_MAX_CAM_NUM ) {
        LOGE("Open device failed\n");
        return -1;
    }

    this->nCamfd = fd;
    LOGE("Camera found driver:%s. card:%s.\n",
            this->pDrvName, this->pModelName);

    return 0;
}

int IPU_V4L2::V4l2InitFmt() {
    struct v4l2_format fmt;
    struct v4l2_fmtdesc fmtdesc;
    int ret;
    int nCamFmtFound = 0;
    int page_size = getpagesize();

    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while ((ioctl(this->nCamfd, VIDIOC_ENUM_FMT, &fmtdesc)) == 0) {
        LOGD("%d.\n{\n\tpixelformat = '%c%c%c%c',\n\tdescription = '%s'\n }\n",
            fmtdesc.index+1, fmtdesc.pixelformat & 0xFF, (fmtdesc.pixelformat >> 8) & 0xFF,
            (fmtdesc.pixelformat >> 16) & 0xFF, (fmtdesc.pixelformat >> 24) & 0xFF,
            fmtdesc.description);
        fmtdesc.index++;
        if(fmtdesc.pixelformat == this->GetV4l2PixFmt(this->ePixFmt)) {
            struct v4l2_frmsizeenum fsize;
            fsize.index = 0;
            fsize.type = V4L2_FRMSIZE_TYPE_DISCRETE;
            fsize.pixel_format = fmtdesc.pixelformat;
            while((ioctl(this->nCamfd, VIDIOC_ENUM_FRAMESIZES, &fsize))==0){
                LOGD("\t %d.%d*%d\n",fsize.index+1,fsize.discrete.width,
                        fsize.discrete.height);
                fsize.index++;
                if(fsize.discrete.width == this->nImgWidth &&
                    fsize.discrete.height == this->nImgHeight) {
                        nCamFmtFound = 1;
                }

                if (fsize.discrete.width >= this->nMaxImgWidth &&
                        fsize.discrete.height >= this->nMaxImgHeight ) {
                    this->nMaxImgWidth = fsize.discrete.width;
                    this->nMaxImgHeight = fsize.discrete.height;
                }
            }
            break;
        }
    }

    if(nCamFmtFound != 1) {
        LOGE("V4l2InitFmt failed.\n");
        return -1;
    }
    /* set fmt */

    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = this->nImgWidth;
    fmt.fmt.pix.height = this->nImgHeight;
    fmt.fmt.pix.pixelformat = this->GetV4l2PixFmt(this->ePixFmt);
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    ret = ioctl (this->nCamfd, VIDIOC_S_FMT, &fmt);
    if(ret < 0) {
        LOGE("VIDIOC_S_FMT falied.\n");
        return -1;
    }
    ret = ioctl (this->nCamfd, VIDIOC_G_FMT, &fmt);
    if(ret < 0) {
        LOGE("VIDIOC_G_FMT falied.\n");
        return -1;
    }
    LOGD("width=%d, heigth=%d, format=%x sizeimage:%d \n",  fmt.fmt.pix.width,
        fmt.fmt.pix.height,fmt.fmt.pix.pixelformat, fmt.fmt.pix.sizeimage);

    this->nFrameSize = (fmt.fmt.pix.sizeimage + page_size -1) & ~(page_size - 1);
    this->nMaxFrameSize = (this->nMaxImgWidth * this->nMaxImgHeight * 3 / 2 + page_size - 1) & ~(page_size - 1);

    return 0;
}

int IPU_V4L2::V4l2InitBuffer() {
    struct v4l2_requestbuffers req;
    unsigned int i;
    int ret;
    /*
     * We use V4L2_MEMORY_USERPTR instead of V4L2_MEMORY_DMABUF.
     * So we need alloc buffer in the userspace
     */

    LOGD("V4l2InitBuffer nAllocBufST %d\n",nAllocBufST);
    if (nAllocBufST == 0) {
        Vbuffer = new FRBuffer(Name.c_str(), FRBuffer::Type::FIXED, 1,this->nMaxFrameBuffers * this->nMaxFrameSize);
        if (Vbuffer) {
            nAllocBufST = 1;
        }

        cBuf = Vbuffer->GetBuffer();
    }

    if ((RunSt == V4L2_CAM_RUN_CAPTURING) || (RunSt == V4L2_CAM_RUN_PREVIEWING)) {
        while (this->nBuffers * this->nFrameSize > this->nMaxFrameBuffers * this->nMaxFrameSize) {
            this->nBuffers--;
        }

        if (this->nBuffers < 2) {
            LOGE("V4l2InitBuffer please check your param.\n");
            throw VBERR_RUNTIME;
        }
    }

    LOGD("RunSt %d, nBuffers %d, nFrameSize %d, nMaxFrameSize %d\n", RunSt,this->nBuffers,this->nFrameSize,this->nMaxFrameSize);
    memset (&(req), 0, sizeof (req));
    req.count = this->nBuffers;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;
    ret = ioctl(this->nCamfd, VIDIOC_REQBUFS, &req);
    if(ret < 0 || req.count < this->nBuffers) {
        LOGE("VIDIOC_REQBUFS failed. get %d/%d buffers.\n",
             req.count, this->nBuffers);
        return -1;
    }

    for(i = 0; i < this->nBuffers; i++) {
        struct v4l2_buffer buf;
        memset (&(buf), 0, sizeof (buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_USERPTR;
        buf.index = i;
        buf.m.userptr = (unsigned long)cBuf.fr_buf.virt_addr + i *this->nFrameSize ;
        buf.length = this->nFrameSize;
        this->sBuffer[i].virt_addr = (void *)buf.m.userptr;
        this->sBuffer[i].phys_addr = cBuf.fr_buf.phys_addr + i * this->nFrameSize ;
        this->sBuffer[i].size = buf.length;
        //memset((void *)buf.m.userptr, 0, buf.length);
        LOGD("VIDIOC_QBUFS index-%d.virt_phys-0x%x phy_addr:0x%x. length-%d\n",
                i, this->sBuffer[i].virt_addr, this->sBuffer[i].phys_addr, buf.length);

        if(-1  ==  ioctl (this->nCamfd, VIDIOC_QBUF, &buf)) {
            LOGE("VIDIOC_QBUFS faild.\n");
            goto BUFF_ERROR;
        }
    }
    return 0;

BUFF_ERROR:
    Vbuffer->PutBuffer(&cBuf);
    delete Vbuffer;
    return -1;
}

int IPU_V4L2::V4l2Deinit() {
    int ret = 0;
    LOGD("V4l2Deinit.\n");
    if(this->Vbuffer)  {
        Vbuffer->PutBuffer(&cBuf);
        delete Vbuffer;
    }

    if(this->nCamfd > 0) {
        close(this->nCamfd);
        this->nCamfd = 0;
    }
    return ret;
}

int IPU_V4L2::V4l2Start() {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    LOGD("V4l2Start().\n");
    if(ioctl(this->nCamfd, VIDIOC_STREAMON, &type) < 0) {
        LOGE("VIDIOC_STREAMON failed\n");
        return -1;
    }
    return 0;
}

int IPU_V4L2::V4l2Stop() {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    LOGD("V4l2Stop().\n");
    if(ioctl (this->nCamfd, VIDIOC_STREAMOFF, &type) < 0 ) {
        LOGE("VIDIOC_STREAMOFF failed\n");
        return -1;
    }
    return 0;
}


void IPU_V4L2::Prepare() {
    Port *p = this->GetPort("out");
    /*
     * require pixel args from port
     */
    LOGD("Prepare().\n");
    this->nImgWidth = p->GetWidth();
    this->nImgHeight = p->GetHeight();
    this->ePixFmt = p->GetPixelFormat();
    //this->ePixFmt = Pixel::Format::NotPixel;
/*
    if(this->V4l2Init())
        ret = -1;
*/
    // current support pixel_format, may be more in future
    if(this->ePixFmt != Pixel::Format::H264ES && this->ePixFmt != Pixel::Format::MJPEG
        && this->ePixFmt != Pixel::Format::NV12 && this->ePixFmt != Pixel::Format::NV21
        && this->ePixFmt != Pixel::Format::YUYV422)
    {
        LOGE("Out Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    if(this->V4l2InitDevice())
        goto START_ERROR;
    if(this->V4l2InitFmt())
        goto START_ERROR;
    if(this->V4l2InitBuffer())
        goto START_ERROR;
    if(this->V4l2Start())
        goto START_ERROR;
    return ;

START_ERROR:
    this->V4l2Deinit();
    throw VBERR_RUNTIME;
}

void IPU_V4L2::Unprepare() {
    Port *pIpuPort = NULL;
    Port *pPortCap = NULL;

    LOGD("Unprepare().\n");
    pIpuPort = GetPort("out");
    pPortCap = GetPort("cap");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

    if (pPortCap->IsEnabled())
    {
         pPortCap->FreeVirtualResource();
    }

    this->V4l2Stop();
    this->V4l2Deinit();
}

int IPU_V4L2::V4L2QueryCtrl(struct v4l2_queryctrl *queryctrl) {
    if(ioctl(this->nCamfd, VIDIOC_QUERYCTRL, queryctrl) < 0) {
        LOGE("VIDIOC_QUERYCTRL Failed  ID:%x\n", queryctrl->id);
        return -1;
    }
    LOGE("VIDIOC_QUERYCTRL Name--%s,  ID-%x, Type-%d, Step--%d, Min--%d, Max--%d, default--%d\n",
            queryctrl->name,  queryctrl->id, queryctrl->type, queryctrl->step,
            queryctrl->minimum, queryctrl->maximum,  queryctrl->default_value);
    return 0;
}

int IPU_V4L2::V4L2GetCtrl(struct v4l2_control *ctrl) {
    if(ioctl(this->nCamfd, VIDIOC_G_CTRL, ctrl) < 0) {
        LOGE("VIDIOC_G_CTRL Failed  ID:%x\n", ctrl->id);
        return -1;
    }
    LOGE("VIDIOC_G_CTRL ID-%x, Value-%d\n", ctrl->id, ctrl->value);
    return 0;
}

int IPU_V4L2::V4L2SetCtrl(struct v4l2_control *ctrl) {
    if(ioctl(this->nCamfd, VIDIOC_S_CTRL, ctrl) < 0) {
        LOGE("VIDIOC_S_CTRL Failed ID-%x, Value-%d\n", ctrl->id, ctrl->value);
        return -1;
    }
    LOGE("VIDIOC_S_CTRL ID-%x, Value-%d\n", ctrl->id, ctrl->value);
    return 0;
}

int IPU_V4L2::V4L2GetBrightness() {
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control ctrl;

    queryctrl.id = V4L2_CID_BRIGHTNESS;
    if(this->V4L2QueryCtrl(&queryctrl))
        return VBENOFUNC;

    ctrl.id = V4L2_CID_BRIGHTNESS;
    if(this->V4L2GetCtrl(&ctrl))
        return -1;

    /* TODO The value maybe need some transport*/
    return ctrl.value;
}

int IPU_V4L2::V4L2SetBrightness(int brightness) {
    struct v4l2_queryctrl queryctrl;
    struct v4l2_control ctrl;

    queryctrl.id = V4L2_CID_BRIGHTNESS;
    if(this->V4L2QueryCtrl(&queryctrl))
        return VBENOFUNC;

    ctrl.value = brightness;
    ctrl.id = V4L2_CID_BRIGHTNESS;
    if(this->V4L2SetCtrl(&ctrl))
        return -1;

    /* TODO The value maybe need some transport*/
    return 0;
}


int IPU_V4L2::UnitControl(std::string func, void *arg)
{
    LOGD("%s(): %s\n", __func__, func.c_str());
    UCSet(func);
#if 1
    UCFunction(camera_load_parameters) {
        LOGE("[IPU_V4L2]%s is not implemented\n", func.c_str());
        return VBENOFUNC;
    }
    UCFunction(camera_get_brightness) {
        return this->V4L2GetBrightness();
    }

    UCFunction(camera_get_brightness) {
        return this->V4L2SetBrightness(*(int*)arg);
    }
    UCFunction(camera_v4l2_snap_one_shot) {
        return V4L2TakePicture();
    }

    UCFunction(camera_v4l2_snap_exit) {
        return V4L2StopPicture();
    }
#endif
    LOGE("[IPU_V4L2]%s is not support\n", func.c_str());
    return VBENOFUNC;
}


IPU_V4L2::~IPU_V4L2() {
    LOGD("~IPU_V4L2().\n");
    this->DestroyPort("out");
}

int IPU_V4L2::V4L2TakePicture()
{
    pthread_mutex_lock(&v4l2_mutex);
    RunSt = V4L2_CAM_RUN_CAPTURING;
    pthread_mutex_unlock(&v4l2_mutex);
    return 0;
}

int IPU_V4L2::V4L2StopPicture()
{
    pthread_mutex_lock(&v4l2_mutex);
    RunSt = V4L2_CAM_RUN_PREVIEWING;
    pthread_mutex_unlock(&v4l2_mutex);
    return 0;
}
