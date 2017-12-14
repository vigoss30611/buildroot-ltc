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

#include <stdio.h>
#include <string.h>
#include <qsdk/videobox.h>

int marker_disable(const char *marker)
{
    videobox_launch_rpc_l(marker, NULL, 0);
    return _rpc.ret;
}

int marker_enable(const char *marker)
{
    videobox_launch_rpc_l(marker, NULL, 0);
    return _rpc.ret;
}

int marker_set_mode(const char *marker, const char *mode, const char *fmt, struct font_attr *attr)
{
    struct marker_mode_data marker_mode;
    if(strlen(fmt) >= FMT_MAX_LENGTH) {
        return -1;
    }
    if(0 == strcmp(mode, "auto")) {
        marker_mode.mode = AUTO;
    } else if(0 == strcmp(mode, "manual")) {
        marker_mode.mode = MANUAL;
    } else {
        return -1;
    }
    marker_mode.attr = *attr;
    memcpy(marker_mode.fmt, fmt, strlen(fmt)+1);
    videobox_launch_rpc(marker, &marker_mode);
    return _rpc.ret;
}

int marker_set_picture(const char *marker, char *file_path)
{
    struct file_path file;
    if(strlen(file_path) >= FILE_PATH_LENGTH) {
        return -1;
    }
    memcpy(file.path, file_path, strlen(file_path)+1);
    videobox_launch_rpc(marker, &file);
    return _rpc.ret;
}

int marker_set_string(const char *marker, const char *str)
{
    struct marker_data data;
    if(strlen(str) >= DATA_LENGTH){
        return -1;
    }
    memcpy(data.str, str, strlen(str)+1);
    videobox_launch_rpc(marker, &data);
    return _rpc.ret;
}

int marker_get_frame(const char *marker, struct fr_buf_info *buf)
{
    fr_get_buf_by_name(marker, buf);
}

int marker_put_frame(const char *marker, struct fr_buf_info *buf)
{
    fr_put_buf(buf);
    videobox_launch_rpc_l(marker, NULL, 0);
}

