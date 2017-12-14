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

#include <stdio.h>
#include <string.h>
#include <qsdk/videobox.h>

int va_move_set_sensitivity(const char *va, int level) {
    videobox_launch_rpc(va, &level);
    return _rpc.ret;
}

int va_move_get_sensitivity(const char *va) {
    videobox_launch_rpc_l(va, NULL, 0);
    return _rpc.ret;
}

int va_move_get_status(const char *va, int row, int column) {
    int cr = (row << 16) | column;
    videobox_launch_rpc(va, &cr);
    return _rpc.ret;
}
int va_move_set_sample_fps(const char *va, int target_fps) {
    videobox_launch_rpc(va, &target_fps);
    return _rpc.ret;
}
int va_move_get_sample_fps(const char *va) {
    videobox_launch_rpc_l(va, NULL, 0);
    return _rpc.ret;
}

int va_mv_set_blkinfo(const char *va, struct blkInfo_t *st) {
    videobox_launch_rpc(va, st);
    return _rpc.ret;
}

int va_mv_get_blkinfo(const char *va, struct blkInfo_t *st) {
    videobox_launch_rpc(va, st);
    return _rpc.ret;
}

int va_mv_set_roi(const char *va, struct roiInfo_t *st) {
    videobox_launch_rpc(va, st);
    return _rpc.ret;
}

int va_mv_get_roi(const char *va, struct roiInfo_t *st) {
    videobox_launch_rpc(va, st);
    return _rpc.ret;
}

int va_mv_set_sensitivity(const char *va, struct MBThreshold *st) {
    videobox_launch_rpc(va, st);
    return _rpc.ret;
}

int va_mv_get_sensitivity(const char *va, struct MBThreshold *st) {
    videobox_launch_rpc(va, st);
    return _rpc.ret;
}

int va_mv_set_senlevel(const char *va, int level) {
    videobox_launch_rpc(va, &level);
    return _rpc.ret;
}

int va_mv_get_senlevel(const char *va) {
    videobox_launch_rpc_l(va, NULL, 0);
    return _rpc.ret;
}

int va_mv_set_sample_fps(const char *va, int target_fps) {
    videobox_launch_rpc(va, &target_fps);
    return _rpc.ret;
}
int va_mv_get_sample_fps(const char *va) {
    videobox_launch_rpc_l(va, NULL, 0);
    return _rpc.ret;
}
