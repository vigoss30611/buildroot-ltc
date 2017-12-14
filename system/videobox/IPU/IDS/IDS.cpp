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

#include <System.h>
#include "IDS.h"


DYNAMIC_IPU(IPU_IDS, "ids");

void IPU_IDS::set_osd0(int fd, uint32_t addr, uint32_t w, uint32_t h,
        struct crop_info *crop, Pixel::Format pixf)
{
    struct imapfb_info iinfo;

    if(crop->w && crop->h && (crop->x + crop->w <= w)
            && (crop->y + crop->h <= h)) {
        iinfo.win_width = crop->w;
        iinfo.win_height = crop->h;
        iinfo.crop_x = crop->x;
        iinfo.crop_y = crop->y;
    } else {
        LOGE("no crop info or crop info invalid\n");
        iinfo.win_width = w;
        iinfo.win_height = h;
        iinfo.crop_x = 0;
        iinfo.crop_y = 0;
    }

    iinfo.width = w;
    iinfo.height = h;
    iinfo.scaler_buff = addr;
    iinfo.format   = (pixf == Pixel::Format::NV21)? 2:
                    ((pixf == Pixel::Format::NV12)? 1: 0);

    if (ioctl(fd, IMAPFB_CONFIG_VIDEO_SIZE, &iinfo) == -1) {
        LOGE("set osd0 size failed\n");
        return ;
    }
    suspend_info = iinfo;
}

IPU_IDS::IPU_IDS(std::string name, Json *js) {
    Name = name;
    no_osd1 = true;

    if((NULL != js) && js->GetObject("no_osd1"))
        no_osd1 = js->GetInt("no_osd1") ? true : false;

    Port *p = CreatePort("osd0", Port::In);
    CreatePort("osd1", Port::In);
    p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    p->SetPixelFormat(Pixel::Format::NV12);
    memset(&m_PreBuf, 0, sizeof(Buffer));
}

static void disable_color_key(void)
{
    int fd_osd1 = open("/dev/fb1", O_RDWR), size;
    struct fb_var_screeninfo vinfo;

    ioctl(fd_osd1, FBIOGET_VSCREENINFO, &vinfo);
    LOGD("osd1 info: bpp%d, std%d, x%d, y%d, vx%d, vy%d\n",
     vinfo.bits_per_pixel, vinfo.nonstd, vinfo.xres, vinfo.yres,
     vinfo.xres_virtual, vinfo.yres_virtual);

    size = vinfo.xres * vinfo.yres * 4 * 2;
    void *buf = malloc(size);
    memset(buf, 0x01, size);
    write(fd_osd1, buf, size);
    free(buf);
    close(fd_osd1);
}

void IPU_IDS::Prepare()
{
    Pixel::Format enPixelFormat = GetPort("osd0")->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21
        && enPixelFormat != Pixel::Format::RGBA8888)
    {
        LOGE("osd0 Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    fd_osd0 = open("/dev/fb0", O_RDWR);
    if(fd_osd0 < 0) {
        LOGE("failed to open fb0\n");
        throw VBERR_BADPARAM;
    }
}

void IPU_IDS::Unprepare()
{
    Port *pIpuPort = GetPort("osd0");

    if(pIpuPort->IsEnabled())
    {
        pIpuPort->FreeVirtualResource();
    }
    pIpuPort = GetPort("osd1");
    if(pIpuPort && pIpuPort->IsEnabled())
    {
        pIpuPort->FreeVirtualResource();
    }
    ioctl(fd_osd0, IMAPFB_LCD_SUSPEND, this->suspend_info);
    close(fd_osd0);
    if (m_PreBuf.fr_buf.phys_addr != 0)
    {
        GetPort("osd0")->PutBuffer(&m_PreBuf);
        m_PreBuf.fr_buf.phys_addr = 0;
    }
}

void IPU_IDS::WorkLoop()
{
    struct fb_var_screeninfo vinfo;
    struct crop_info cinfo;
    Port *p = GetPort("osd0");
    Buffer buf;
    bool ids_init_flag = false;
    int s32ResUpdate = 0;
    uint32_t u32Width = 0, u32Height = 0;

    u32Width = p->GetWidth();
    u32Height = p->GetHeight();
    while(IsInState(ST::Running)) {
        if(!ids_init_flag) {
            try {
                buf = p->GetBuffer(&buf);
            } catch (const char* err) {
                usleep(5*1000);
                continue;
            }
            ioctl(fd_osd0, FBIOGET_VSCREENINFO, &vinfo);
            if (buf.fr_buf.priv == VIDEO_FRAME_DUMMY)
            {
                u32Width = buf.fr_buf.stAttr.s32Width;
                u32Height = buf.fr_buf.stAttr.s32Height;
                p->PutBuffer(&buf);
                continue;
            } else {
                LOGE("buf.fr_buf.priv 1:0x%x\n", buf.fr_buf.priv);
                cinfo.x = p->GetPipX();
                cinfo.y = p->GetPipY();
                cinfo.w = p->GetPipWidth();
                cinfo.h = p->GetPipHeight();
            }

            set_osd0(fd_osd0, buf.fr_buf.phys_addr, u32Width,u32Height,
                    &cinfo, p->GetPixelFormat());

            if(no_osd1)
                disable_color_key();
            ioctl(fd_osd0, IMAPFB_LCD_UNSUSPEND, NULL);

            p->PutBuffer(&buf);
            ids_init_flag = true;
        }
        try {
            buf = p->GetBuffer(&buf);
            if (buf.fr_buf.priv == VIDEO_FRAME_DUMMY)
            {
                ioctl(fd_osd0, IMAPFB_LCD_SUSPEND, NULL);
                u32Width = buf.fr_buf.stAttr.s32Width;
                u32Height = buf.fr_buf.stAttr.s32Height;
                p->PutBuffer(&buf);
                s32ResUpdate = 1;
                continue;
            }
            LOGD("ids got buffer: %08x\n", buf.fr_buf.phys_addr);

            if (s32ResUpdate)
            {
                ioctl(fd_osd0, FBIOGET_VSCREENINFO, &vinfo);
                set_osd0(fd_osd0, buf.fr_buf.phys_addr, u32Width, u32Height,
                    &cinfo, p->GetPixelFormat());
                vinfo.reserved[0] = buf.fr_buf.phys_addr;
                ioctl(fd_osd0, IMAPFB_LCD_UNSUSPEND, NULL);
                s32ResUpdate = 0;
            }
            else
            {
                vinfo.reserved[0] = buf.fr_buf.phys_addr;
            }
            ioctl(fd_osd0, FBIOPAN_DISPLAY, &vinfo);
            if (m_PreBuf.fr_buf.phys_addr != 0)
            {
                p->PutBuffer(&m_PreBuf);
                m_PreBuf.fr_buf.phys_addr = 0;
            }
            memcpy(&m_PreBuf, &buf, sizeof(Buffer));

        } catch (const char* err) {
            LOGE("err:%s\n",err);
            usleep(200000);
        }
    }
}

int IPU_IDS::UnitControl(std::string func, void *arg)
{
    return 0;
}
