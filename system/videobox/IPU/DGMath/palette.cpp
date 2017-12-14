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

#define BLOCK_W 40
#define BLOCK_H 64

static Pixel color[] = {
    Pixel(178, 135, 169),
    Pixel(76, 111, 119),
    Pixel(184, 145, 118),
    Pixel(128, 182, 47),
    Pixel(199, 109, 149),
    Pixel(178, 105, 116),
};

int palette_get_offset(int t, int h)
{
//    LOGE("%d, %d\n", t / 10, (t / 10) % (BLOCK_H * 6));
    return (t >> 4) % (BLOCK_H * 8);
}

void palette_draw(char *buf, int w, int h, int i, int j)
{
    int n;

//    LOGE("buf: %p, w: %d, i: %d, j: %d\n", buf, w, i, j);
//    LOGE("in draw: %d, %d\n", i, j);

    n = (i / BLOCK_W + ((j / BLOCK_H) & 1) * 3) % 6;
//    LOGE("draw pixel: 0x%08p ", ((uint32_t *)buf + j * w + i));
    *((uint32_t *)buf + j * w + i) = color[n].GetRGBA();
//    LOGE("OK\n");
}
