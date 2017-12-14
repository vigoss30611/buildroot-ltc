// Copyright (C) 2016 InfoTM, jinhua.chen@infotm.com
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

#ifndef IPU_SOFTLAYER_H
#define IPU_SOFTLAYER_H
#include <IPU.h>

#define MAX_OL_NUM  4
#define DEFAULT_W  32
#define DEFAULT_H  32

struct RGB {
    int r;
    int g;
    int b;
};

struct YUV {
    int y;
    int u;
    int v;
};

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X)

// RGB -> YUV
#define RGB2Y(R, G, B) CLIP(( (  66 * (R) + 129 * (G) +  25 * (B) + 128) >> 8) +  16)
#define RGB2U(R, G, B) CLIP(( ( -38 * (R) -  74 * (G) + 112 * (B) + 128) >> 8) + 128)
#define RGB2V(R, G, B) CLIP(( ( 112 * (R) -  94 * (G) -  18 * (B) + 128) >> 8) + 128)

// YUV -> RGB
#define C(Y) ( (Y) - 16  )
#define D(U) ( (U) - 128 )
#define E(V) ( (V) - 128 )

#define YUV2R(Y, U, V) CLIP(( 298 * C(Y)              + 409 * E(V) + 128) >> 8)
#define YUV2G(Y, U, V) CLIP(( 298 * C(Y) - 100 * D(U) - 208 * E(V) + 128) >> 8)
#define YUV2B(Y, U, V) CLIP(( 298 * C(Y) + 516 * D(U)              + 128) >> 8)

// RGB -> YCbCr
#define CRGB2Y(R, G, B) CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16)
#define CRGB2Cb(R, G, B) CLIP((36962 * (B - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)
#define CRGB2Cr(R, G, B) CLIP((46727 * (R - CLIP((19595 * R + 38470 * G + 7471 * B ) >> 16) ) >> 16) + 128)

// YCbCr -> RGB
#define CYCbCr2R(Y, Cb, Cr) CLIP( Y + ( 91881 * Cr >> 16 ) - 179 )
#define CYCbCr2G(Y, Cb, Cr) CLIP( Y - (( 22544 * Cb + 46793 * Cr ) >> 16) + 135)
#define CYCbCr2B(Y, Cb, Cr) CLIP( Y + (116129 * Cb >> 16 ) - 226 )

class IPU_SoftLayer: public IPU {

    private:
        int R = 0;
        int G = 1;
        int B = 2;
        pthread_mutex_t m_PipMutex;
        int Img_Compose_RGBA8888(Buffer *dst, Port *out, int num);
        int Img_Compose_NV12(Buffer *dst, Port *out, int num);
        bool CheckPPAttribute();
        struct RGB yuvTorgb(char Y, char U, char V);
        char *NV12ToARGB(char *src, int width, int height);
        int ARGBToNV12(Buffer *dst,Port *out, Port *pOl, int width, int height, int offset_x, int offset_y);
        struct YUV rgbToyuv(char R, char G, char B);


    public:
        IPU_SoftLayer(std::string name, Json *js);
        ~IPU_SoftLayer();
        void WorkLoop();
        void Prepare();
        void Unprepare();
        int Img_Compose(Buffer *dst);
        void SoftlayerAdjustOvPos(int ov_idx);
        int SoftlayerFormatPipInfo(struct v_pip_info *vpinfo);
        int SoftlayerSetPipInfo(struct v_pip_info *vpinfo);
        int SoftlayerGetPipInfo(struct v_pip_info *vpinfo);
        int UnitControl(std::string func, void* arg);
};

#endif
