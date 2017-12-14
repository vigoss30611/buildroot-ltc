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

#ifndef __VB_MARKER_API_H__
#define __VB_MARKER_API_H__

#define TEMP_FMT_LENGTH 128
#define FMT_MAX_LENGTH  64
#define DATA_LENGTH     32
#define FILE_PATH_LENGTH     256

struct marker_data {
    char str[DATA_LENGTH];
};

enum marker_mode{
    MANUAL = 0,
    AUTO
};

enum display_mode{
    LEFT = 0,
    MIDDLE,
    RIGHT
};

struct font_attr {
    char font_size;
    int back_color;
    int font_color;
    char ttf_name[32];
    enum display_mode mode;
};

struct marker_mode_data {
    enum marker_mode mode;
    struct font_attr attr;
    char fmt[FMT_MAX_LENGTH];
};

struct marker_area{
    int width;
    int height;
};

struct file_path{
    char path[FILE_PATH_LENGTH];
};

int marker_disable(const char *marker);

int marker_enable(const char *marker);

int marker_set_mode(const char *marker, const char *mode, const char *fmt, struct font_attr *attr);

int marker_set_picture(const char *marker, char *file_path);

int marker_set_string(const char *marker, const char *str);

int marker_get_frame(const char *marker, struct fr_buf_info *buf);

int marker_put_frame(const char *marker, struct fr_buf_info *buf);


#endif /* __VB_MARKER_API_H__ */
