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

int overlay_disable_region(const char *marker)
{
    videobox_launch_rpc_l(marker, NULL, 0);
    return _rpc.ret;
}

int overlay_enable_region(const char *marker)
{
    videobox_launch_rpc_l(marker, NULL, 0);
    return _rpc.ret;
}

int overlay_set_picture(const char *marker, char *buf)
{
    videobox_launch_rpc(marker, buf);
    return _rpc.ret;
}

int overlay_set_string(const char *marker, const char *str, struct font_attr *attr)
{
    struct overlay_data overlay;
    memcpy(overlay.str, str, strlen(str)+1);
    overlay.attr = *attr;
    videobox_launch_rpc(marker, &overlay);
    return _rpc.ret;
}
