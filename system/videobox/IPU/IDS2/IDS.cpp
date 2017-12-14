// Copyright (C) 2016 InfoTM, feng.qu@infotm.com
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
#include <iostream>
#include "fb.h"
#include "display_fb.h"
#include "IDS.h"


DYNAMIC_IPU(IPU_IDS, "ids");




IPU_IDS::IPU_IDS(std::string name, Json *js) {
    Name = name;
    char argstr[64] = {0};
    int i;
    const char *strp;

    for (i = 0; i < OVERLAY_NUM; i++) {
        memset(&overlay[i], 0, sizeof(struct overlay_info));
        overlay[i].alpha_value = 0xFFFFFF;
        sprintf(argstr, "overlay%d", i);
        LOGD("create in port: %s\n", argstr);
        Port *p = CreatePort(argstr, Port::In);
        p->SetRuntimePolicy(Port::RPType_Resolution, Port::RPolicy_PauseNeeded);
    }
    osd_w = 0;
    osd_h = 0;
    background = 0;
    mapcolor = 0;
    enfb = -1;

    if (js != NULL) {
        sprintf(argstr, "osd_w");
        if (js->GetObject(argstr))
            osd_w = js->GetInt(argstr);
        sprintf(argstr, "osd_h");
        if (js->GetObject(argstr))
            osd_h = js->GetInt(argstr);
        sprintf(argstr, "background");
        if (js->GetObject(argstr)) {
            strp = js->GetString(argstr);
            background = strtoul(strp, NULL, 16);
            LOGE("background=0x%X\n", background);
        }
        sprintf(argstr, "mapcolor");
        if (js->GetObject(argstr)) {
            strp = js->GetString(argstr);
            mapcolor = strtoul(strp, NULL, 16);
            LOGE("mapcolor=0x%X\n", mapcolor);
            mapcolor_enable = 1;
        }

        for (i = 0; i < OVERLAY_NUM; i++) {
            sprintf(argstr, "ov%d_w", i);
            if (js->GetObject(argstr))
                overlay[i].w = js->GetInt(argstr);
            sprintf(argstr, "ov%d_h", i);
            if (js->GetObject(argstr))
                overlay[i].h = js->GetInt(argstr);
            sprintf(argstr, "ov%d_x", i);
            if (js->GetObject(argstr))
                overlay[i].x = js->GetInt(argstr);
            sprintf(argstr, "ov%d_y", i);
            if (js->GetObject(argstr))
                overlay[i].y = js->GetInt(argstr);
            if (i == 0)
                continue;
            sprintf(argstr, "ov%d_chroma_key_enable", i);
            if (js->GetObject(argstr))
                overlay[i].chroma_key_enable = js->GetInt(argstr);
            sprintf(argstr, "ov%d_chroma_key_value", i);
            if (js->GetObject(argstr)) {
                strp = js->GetString(argstr);
                overlay[i].chroma_key_value = strtoul(strp, NULL, 16);
                LOGE("overlay%d chroma_key_value=0x%X\n", i, overlay[i].chroma_key_value);
            }
            sprintf(argstr, "ov%d_chroma_alpha_nonkey_area", i);
            if (js->GetObject(argstr)) {
                strp = js->GetString(argstr);
                overlay[i].chroma_alpha_nonkey_area = strtoul(strp, NULL, 16);
                LOGE("overlay%d chroma_alpha_nonkey_area=0x%X\n", i, overlay[i].chroma_alpha_nonkey_area);
            }
            sprintf(argstr, "ov%d_chroma_alpha_key_area", i);
            if (js->GetObject(argstr)) {
                strp = js->GetString(argstr);
                overlay[i].chroma_alpha_key_area = strtoul(strp, NULL, 16);
                LOGE("overlay%d chroma_alpha_key_area=0x%X\n", i, overlay[i].chroma_alpha_key_area);
            }
            sprintf(argstr, "ov%d_alpha_type", i);
            if (js->GetObject(argstr))
                overlay[i].alpha_type = js->GetInt(argstr);
            sprintf(argstr, "ov%d_alpha_value", i);
            if (js->GetObject(argstr)) {
                strp = js->GetString(argstr);
                overlay[i].alpha_value = strtoul(strp, NULL, 16);
                LOGE("overlay%d alpha_value=0x%X\n", i, overlay[i].alpha_value);
            }
        }
    }
}

void IPU_IDS::Prepare()
{
    int i;
    char str[64] = {0};
    Port *p;

    for (i = 0; i < OVERLAY_NUM; i++) {
        sprintf(str, "overlay%d", i);
        p = GetPort(str);
        if (p->IsEnabled()) {
            sprintf(str, "/dev/fb%d", i);
            overlay[i].fd = open(str, O_RDWR);
            if(overlay[i].fd < 0) {
                LOGE("failed to open %s\n", str);
                throw VBERR_BADPARAM;
            }
        }
    }
}

void IPU_IDS::Unprepare()
{
    int i;
    int ret;
    char str[64] = {0};
    Port *p;

    if (enfb >= 0) {
        ret = ioctl(overlay[enfb].fd, FBIOBLANK, FB_BLANK_POWERDOWN);
        if (ret) {
            LOGE("ioctl FBIOBLANK failed: %s\n", strerror(errno));
            throw VBERR_RUNTIME;
            return;
        }
    }

    for (i = 0; i < OVERLAY_NUM; i++) {
        sprintf(str, "overlay%d", i);
        p = GetPort(str);
        if (p->IsEnabled()) {
            p->FreeVirtualResource();
            close(overlay[i].fd);
            p->PutBuffer(&(overlay[i].buffer));
        }
    }
}

void IPU_IDS::WorkLoop()
{
    struct fb_var_screeninfo vinfo;
    char portstr[64] = {0};
    Port *p;
    Port *port[2];
    int i = 0;
    int ret;
    struct display_fb_win_size win_size;
    int peripheral_w, peripheral_h;

    for (i = 0; i < OVERLAY_NUM; i++) {
        sprintf(portstr, "overlay%d", i);
        p = GetPort(portstr);
        if (p->IsEnabled()) {
            enfb = i;
            break;
        }
    }

    ret = ioctl(overlay[enfb].fd, FBIO_GET_PERIPHERAL_SIZE, &win_size);
    if (ret) {
        LOGE("fb%d ioctl FBIO_GET_PERIPHERAL_SIZE failed: %s\n", i, strerror(errno));
        throw VBERR_RUNTIME;
        return;
    }
    peripheral_w = win_size.width;
    peripheral_h = win_size.height;
    if (!osd_w || !osd_h) {
        osd_w = peripheral_w;
        osd_h = peripheral_h;
    }
    win_size.width = osd_w;
    win_size.height = osd_h;
    ret = ioctl(overlay[enfb].fd, FBIO_SET_OSD_SIZE, &win_size);
    if (ret) {
        LOGE("fb%d ioctl FBIO_SET_OSD_SIZE failed: %s\n", i, strerror(errno));
        throw VBERR_RUNTIME;
        return;
    }

    for (i = 0; i < OVERLAY_NUM; i++) {
        sprintf(portstr, "overlay%d", i);
        port[i] = GetPort(portstr);
        p = port[i];
        if (p->IsEnabled()) {
            while(IsInState(ST::Running)) {
                try {
                    overlay[i].buffer = p->GetBuffer(&(overlay[i].buffer));
                } catch (const char* err) {
                    usleep(20000);
                    continue;
                }
               break;
            }
            LOGD("port %s is enabled\n", portstr);
            ret = ioctl(overlay[i].fd, FBIOGET_VSCREENINFO, &vinfo);
            if (ret) {
                LOGE("fb%d ioctl FBIOGET_VSCREENINFO failed: %s\n", i, strerror(errno));
                throw VBERR_RUNTIME;
                return;
            }
            if (!p->GetPipWidth() || !p->GetPipHeight()) {
                p->SetPipInfo(0, 0, 0, 0);
            }
            if (!overlay[i].w || !overlay[i].h) {
                overlay[i].x = 0;
                overlay[i].y = 0;
            }

            vinfo.xres = p->GetPipWidth() ? p->GetPipWidth() : p->GetWidth();
            vinfo.yres = p->GetPipHeight() ? p->GetPipHeight() : p->GetHeight();
            vinfo.xres_virtual = p->GetWidth();
            vinfo.yres_virtual = p->GetHeight();
            vinfo.xoffset = p->GetPipX();
            vinfo.yoffset = p->GetPipY();
            vinfo.width = overlay[i].w ? overlay[i].w : osd_w;
            vinfo.height = overlay[i].h ? overlay[i].h : osd_h;
            vinfo.reserved[0] = overlay[i].x;
            vinfo.reserved[1] = overlay[i].y;
            vinfo.activate = FB_ACTIVATE_FORCE;
            switch (p->GetPixelFormat()) {
                case Pixel::Format::RGBA8888:
                    vinfo.grayscale = V4L2_PIX_FMT_RGB32;
                    break;
                case Pixel::Format::RGB565:
                    vinfo.grayscale = V4L2_PIX_FMT_RGB565;
                    break;
                case Pixel::Format::NV12:
                    vinfo.grayscale = V4L2_PIX_FMT_NV12;
                    break;
                case Pixel::Format::NV21:
                    vinfo.grayscale = V4L2_PIX_FMT_NV21;
                    break;
                default:
                    LOGE("fb%d Invalid Pixel Format %d\n", i, p->GetPixelFormat());
                    throw VBERR_BADPARAM;
                    return;
                    break;
            }
            LOGD("overlay%d info: w %d, h %d, pip_w %d, pip_h %d, pip_x %d, pip_y %d, overlay_w %d, overlay_h %d\n"
                "overlay_x %d, overlay_y %d, osd_w %d, osd_h %d, peripheral_w %d, peripheral_h %d, pixel format 0x%X\n",
                i, vinfo.xres_virtual, vinfo.yres_virtual, vinfo.xres, vinfo.yres, vinfo.xoffset, vinfo.yoffset, vinfo.width, vinfo.height,
                vinfo.reserved[0], vinfo.reserved[1], osd_w, osd_h, peripheral_w, peripheral_h, p->GetPixelFormat());
            ioctl(overlay[i].fd, FBIOPUT_VSCREENINFO, &vinfo);
            if (ret) {
                LOGE("fb%d ioctl FBIOPUT_VSCREENINFO failed: %s\n", i, strerror(errno));
                throw VBERR_RUNTIME;
                return;
            }
            ret = ioctl(overlay[i].fd, FBIO_SET_BUFFER_ADDR, overlay[i].buffer.fr_buf.phys_addr);
            if (ret) {
                LOGE("fb%d ioctl FBIO_SET_BUFFER_ADDR failed: %s\n", i, strerror(errno));
                throw VBERR_RUNTIME;
                return;
            }
            if (i > 0) {
                if (overlay[i].chroma_key_enable) {
                    ret = ioctl(overlay[i].fd, FBIO_SET_CHROMA_KEY_VALUE, overlay[i].chroma_key_value);
                    if (ret) {
                        LOGE("ioctl FBIO_SET_CHROMA_KEY_VALUE failed: %s\n", strerror(errno));
                        throw VBERR_RUNTIME;
                        return;
                    }
                    ret = ioctl(overlay[i].fd, FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA, overlay[i].chroma_alpha_nonkey_area);
                    if (ret) {
                        LOGE("ioctl FBIO_SET_CHROMA_KEY_ALPHA_NONKEY_AREA failed: %s\n", strerror(errno));
                        throw VBERR_RUNTIME;
                        return;
                    }
                    ret = ioctl(overlay[i].fd, FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA, overlay[i].chroma_alpha_key_area);
                    if (ret) {
                        LOGE("ioctl FBIO_SET_CHROMA_KEY_ALPHA_KEY_AREA failed: %s\n", strerror(errno));
                        throw VBERR_RUNTIME;
                        return;
                    }
                    ret = ioctl(overlay[i].fd, FBIO_SET_CHROMA_KEY_ENABLE, overlay[i].chroma_key_enable);
                    if (ret) {
                        LOGE("ioctl FBIO_SET_CHROMA_KEY_ENABLE failed: %s\n", strerror(errno));
                        throw VBERR_RUNTIME;
                        return;
                    }
                }
                else if (overlay[i].alpha_value != 0xFFFFFF || overlay[i].alpha_type) {
                    ret = ioctl(overlay[i].fd, FBIO_SET_ALPHA_TYPE, overlay[i].alpha_type);
                    if (ret) {
                        LOGE("ioctl FBIO_SET_BUFFER_ADDR failed: %s\n", strerror(errno));
                        throw VBERR_RUNTIME;
                        return;
                    }
                    ret = ioctl(overlay[i].fd, FBIO_SET_ALPHA_VALUE, overlay[i].alpha_value);
                    if (ret) {
                        LOGE("ioctl FBIO_SET_ALPHA_VALUE failed: %s\n", strerror(errno));
                        throw VBERR_RUNTIME;
                        return;
                    }
                }
            }
        }
    }

    if (background) {
        ret = ioctl(overlay[enfb].fd, FBIO_SET_OSD_BACKGROUND, background);
        if (ret) {
            LOGE("fb%d ioctl FBIO_SET_OSD_BACKGROUND failed: %s\n", i, strerror(errno));
            throw VBERR_RUNTIME;
            return;
        }
    }

    if (mapcolor_enable) {
        ret = ioctl(overlay[enfb].fd, FBIO_SET_MAPCOLOR, mapcolor);
        if (ret) {
            LOGE("fb%d ioctl FBIO_SET_MAPCOLOR failed: %s\n", i, strerror(errno));
            throw VBERR_RUNTIME;
            return;
        }
        ret = ioctl(overlay[enfb].fd, FBIO_SET_MAPCOLOR_ENABLE, mapcolor_enable);
        if (ret) {
            LOGE("fb%d ioctl FBIO_SET_MAPCOLOR_ENABLE failed: %s\n", i, strerror(errno));
            throw VBERR_RUNTIME;
            return;
        }
    }

    ret = ioctl(overlay[enfb].fd, FBIOBLANK, FB_BLANK_UNBLANK);
    if (ret) {
        LOGE("ioctl FBIOBLANK failed: %s\n", strerror(errno));
        throw VBERR_RUNTIME;
        return;
    }

    while(IsInState(ST::Running)) {
        for (i = 0; i < OVERLAY_NUM; i++) {
            p = port[i];
            if (p->IsEnabled()) {
                try {
                    p->PutBuffer(&(overlay[i].buffer));
                    overlay[i].buffer = p->GetBuffer(&(overlay[i].buffer));
                    ret = ioctl(overlay[i].fd, FBIO_SET_BUFFER_ADDR, overlay[i].buffer.fr_buf.phys_addr);
                    if (ret) {
                        LOGE("fb%d ioctl FBIO_SET_BUFFER_ADDR failed: %s\n", i, strerror(errno));
                        throw VBERR_RUNTIME;
                        return;
                    }
                } catch (const char* err) {
                    LOGD("fb%d get buffer failed\n");
                }
            }
        }
        ret = ioctl(overlay[enfb].fd, FBIO_WAITFORVSYNC, 1000);
        if (ret) {
            LOGE("ioctl FBIO_WAITFORVSYNC failed: %s\n", strerror(errno));
            throw VBERR_RUNTIME;
            return;
        }
    }
}

int IPU_IDS::blank(int blank)
{
    int ret;
    int eblank;

    if (enfb < 0)
        return 0;

    if (blank)
        eblank = FB_BLANK_POWERDOWN;
    else
        eblank = FB_BLANK_UNBLANK;

    ret = ioctl(overlay[enfb].fd, FBIOBLANK, eblank);
    if (ret) {
        LOGE("ioctl FBIOBLANK failed: %s\n", strerror(errno));
        return ret;
    }

    return 0;
}

int IPU_IDS::UnitControl(std::string func, void *arg)
{
    UCSet(func);

    UCFunction(ids_blank) {
        return blank(*(int *)arg);
    }

    return 0;
}
