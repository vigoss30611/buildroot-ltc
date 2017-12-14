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
#include "VAQRScanner.h"

DYNAMIC_IPU(IPU_QRScanner, "vaqrscanner");

void IPU_QRScanner::WorkLoop()
{
    int w, h;
    Buffer buf;
    char *raw = NULL;
    Port *p = GetPort("in");

    w = p->GetWidth();
    h = p->GetHeight();

    raw = (char *)malloc(w*h);

    Image = zbar_image_create();
    while(IsInState(ST::Running)) {
        if(zbarEnable){
            try{
                buf = p->GetBuffer(&buf);
            }catch (const char *err){
                usleep(200000);
                LOGE("wait 200ms...\n");
                continue;
            }

            memcpy(raw, buf.fr_buf.virt_addr, w*h);

            zbar_image_set_format(Image, zbar_fourcc('Y','8','0','0'));
            zbar_image_set_size(Image, w, h);
            zbar_image_set_data(Image, raw, w * h, NULL);
            zbar_scan_image(Scanner, Image);

            const zbar_symbol_t *symbol = zbar_image_first_symbol(Image);
            for(; symbol; symbol = zbar_symbol_next(symbol)) {
                /* do something useful with results */
                zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
                const char *data = zbar_symbol_get_data(symbol);
                LOGE("decoded %s symbol \"%s\"\n", zbar_get_symbol_name(typ), data);

                struct event_vaqrscanner ev_qr;
                memcpy(ev_qr.data, data, strlen(data));
                memcpy(ev_qr.type, zbar_get_symbol_name(typ), strlen(zbar_get_symbol_name(typ)));

                event_send(EVENT_VAQR, (char *)&ev_qr, sizeof(ev_qr));
            }
            p->PutBuffer(&buf);
        }
    }
    free(raw);
    zbar_image_destroy(Image);
}

IPU_QRScanner::IPU_QRScanner(std::string name, Json *js) {
     Port *pin = NULL;
    Name = name;

    pin = CreatePort("in", Port::In);
    pin->SetPixelFormat(Pixel::Format::NV12);
    MLock =  new std::mutex();

    zbarEnable = false;
}

IPU_QRScanner::~IPU_QRScanner()
{
    delete MLock;
}

void IPU_QRScanner::Prepare()
{
    int h;
    Port *p = GetPort("in");
    Pixel::Format enPixelFormat = p->GetPixelFormat();

    if(enPixelFormat != Pixel::Format::NV12 && enPixelFormat != Pixel::Format::NV21)
    {
        LOGE("in Port Pixel Format Params Error Not Support\n");
        throw VBERR_BADPARAM;
    }
    h = p->GetHeight();

    density_y = density_x = (h>=720)?(h/180 + 1)/2 : 2;

    Scanner = zbar_image_scanner_create();
    zbar_image_scanner_set_config(Scanner, ZBAR_QRCODE,    ZBAR_CFG_ENABLE, 1);
    zbar_image_scanner_set_config(Scanner, ZBAR_NONE,    ZBAR_CFG_X_DENSITY, density_x);
    zbar_image_scanner_set_config(Scanner, ZBAR_NONE,    ZBAR_CFG_Y_DENSITY, density_y);
}

void IPU_QRScanner::Unprepare()
{
    Port *pIpuPort = NULL;

    pIpuPort = GetPort("in");
    if (pIpuPort->IsEnabled())
    {
         pIpuPort->FreeVirtualResource();
    }

    zbar_image_scanner_destroy(Scanner);
}

void IPU_QRScanner::VaqrEnable()
{
    MLock->lock();
    zbarEnable = true;
    MLock->unlock();
}

void IPU_QRScanner::VaqrDisable()
{
    MLock->lock();
    zbarEnable = false;
    MLock->unlock();
}

int IPU_QRScanner::VaqrSetDensity(density_t *dsty)
{
    if(dsty->x<0 || dsty->y < 0)
        return -1;

    density_x = dsty->x; density_y = dsty->y;
    zbar_image_scanner_set_config(Scanner, ZBAR_NONE, ZBAR_CFG_X_DENSITY, density_x);
    zbar_image_scanner_set_config(Scanner, ZBAR_NONE, ZBAR_CFG_Y_DENSITY, density_y);

    return 0;
}

int IPU_QRScanner::VaqrGetDensity(density_t *dsty)
{
    dsty->x = density_x;
    dsty->y = density_y;

    return 0;
}
