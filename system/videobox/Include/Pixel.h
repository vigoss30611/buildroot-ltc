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

#ifndef Pixel_H
#define Pixel_H
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <Json.h>

class Pixel {
public:
  char r, g, b, y, u, v, a;

  enum Format {
#define bpp_category(b) (b << 8)
    NotPixel = 0,
    H264ES,
    H265ES,
    MJPEG,
    JPEG,
    VAM, //vamovement in type
    BPP2 = bpp_category(2),
    MV, //mvmovement in type should be 1.5,  x * y * MV / 8 = x / 16 * y / 16 * 48
    NV12 = bpp_category(12),
    NV21,
    YUV420P,
    FODET,//fodetv2 out type
    YUYV422 = bpp_category(16),
    RGB565,
    YUV422SP,
    RGB888 = bpp_category(24),
    RGBA8888 = bpp_category(32),
  };

  static int Bits(int pf) {
    return pf >> 8;
  };

  static Pixel::Format ParseString(std::string str) {
    if(str == "bpp2" || str == "BPP2")
        return BPP2;
    if(str == "nv12" || str == "NV12")
      return NV12;
    else if(str == "nv21" || str == "NV21")
      return NV21;
    else if(str == "rgb565" || str == "RGB565")
      return RGB565;
    else if(str == "yuyv422" || str == "YUYV422")
      return YUYV422;
    else if(str == "rgb888" || str == "RGB888")
      return RGB888;
    else if(str == "rgba8888" || str == "RGBA8888")
      return RGBA8888;
    else if(str == "yuv422sp" || str == "YUV422SP")
      return YUV422SP;
    else if(str == "mjpeg" || str == "MJPEG")
        return MJPEG;
    else if(str == "jpeg" || str == "JPEG")
        return JPEG;
    else if(str == "h264" || str == "H264")
        return H264ES;
    else if(str == "h265" || str == "H265")
        return H265ES;
    return NotPixel;
  };

  void SetRGB(char _r, char _g, char _b, char _a) {
    r = _r; g = _g; b = _b; a = _a;
  }

  void SyncRGB() {
    r = (int)(1.164 * (y - 16) + 1.596 * (v - 128));
    g = (int)(1.164 * (y - 16) - 0.813 * (v - 128) - 0.392 * (u - 128));
    b = (int)(1.164 * (y - 16) + 2.017 * (u - 128));
    a = 0xff;
  }

  void SetYUV(char _y, char _u, char _v) {
    y = _y; u = _u; v = _v;
  }

  void SyncYUV() {
    y = (int)(0.257 * r + 0.504 * g + 0.098 * b) + 16,
    u = (int)(-0.148 * r - 0.291 * g + 0.439 * b) + 128,
    v = (int)(0.439 * r - 0.368 * g - 0.071 * b) + 128;
  }

  Pixel(char _r, char _g, char _b, char _a) {
    SetRGB(_r, _g, _b, _a);
    SyncYUV();
  }

  Pixel(char _y, char _u, char _v) {
    SetYUV(_y, _u, _v);
    SyncRGB();
  }

  Pixel() {}

  uint32_t GetRGBA() {
    uint32_t c = 0x0;

    c |= r;
    c <<= 8;
    c |= g;
    c <<= 8;
    c |= b;
    c <<= 8;
    c |= a;

    return c;
  }
};

#endif // Pixel_H
