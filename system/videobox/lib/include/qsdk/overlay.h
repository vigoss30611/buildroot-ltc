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

#ifndef __VIDEOBOX_API_H__
#error YOU SHOULD NOT INCLUDE THIS FILE DIRECTLY, INCLUDE <videobox.h> INSTEAD
#endif

#ifndef __VB_OVERLAY_API_H__
#define __VB_OVERLAY_API_H__

struct font_attr {
    int ttf_name[32];
    int font_size;
    int font_color;
};

struct overlay_data {
    char str[32];
    struct font_attr attr;
};

int overlay_disable_region(const char *marker);

int overlay_enable_region(const char *marker);

int overlay_set_picture(const char *marker, char *buf);

int overlay_set_string(const char *marker, const char *str, struct font_attr *attr);


#endif /* __VB_OVERLAY_API_H__ */
