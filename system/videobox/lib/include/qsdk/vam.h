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

#ifndef __VIDEOBOX_API_H__
#error YOU SHOULD NOT INCLUDE THIS FILE DIRECTLY, INCLUDE <videobox.h> INSTEAD
#endif

#ifndef __VB_VA_MOVE_H__
#define __VB_VA_MOVE_H__

#ifndef EVENT_VAMOVE
#define EVENT_VAMOVE "vamovement"
#endif

typedef struct event_vamovement {
    int x, y; //blk cordinate
    int block_count;
    int timestamp;
} ST_VA_EVENT;

typedef struct MBInfo {
    uint32_t lmv;
    uint32_t amv; //keep quadrant info
    uint32_t cmv;
    uint32_t mbNumRef;
} ST_MB_INFO;

typedef struct MBThreshold {
    uint32_t tlmv;
    uint32_t tdlmv;
    uint32_t tcmv;
} ST_MB_THRESHOLD;

typedef struct evd {
    uint32_t mx;
    uint32_t my;
    uint32_t blk_x;
    uint32_t blk_y;
    uint32_t mbNum;
    uint32_t timestamp;
} ST_EVENT_DATA;

typedef struct mvRoi_t {
    uint32_t x, y;
    uint32_t w, h;
    uint32_t mv_mb_num;
} ST_MV_ROI;

typedef struct roiInfo_t {
    struct mvRoi_t mv_roi;
    int roi_num;
} ST_ROI_INFO;

typedef struct blkInfo_t {
    uint32_t blk_x, blk_y;
} ST_BLK_INFO;

int va_move_set_sensitivity(const char *, int level);
int va_move_get_sensitivity(const char *);
int va_move_get_status(const char *, int row, int column);
int va_move_set_sample_fps(const char *, int target_fps);
int va_move_get_sample_fps(const char *);

int va_mv_set_blkinfo(const char *va, struct blkInfo_t *st);
int va_mv_get_blkinfo(const char *va, struct blkInfo_t *st);
int va_mv_set_roi(const char *va, struct roiInfo_t *st);
int va_mv_get_roi(const char *va, struct roiInfo_t *st);
int va_mv_set_sensitivity(const char *, struct MBThreshold *st);
int va_mv_get_sensitivity(const char *va, struct MBThreshold *st);
int va_mv_set_senlevel(const char *va, int level);
int va_mv_get_senlevel(const char *va);
int va_mv_set_sample_fps(const char *, int target_fps);
int va_mv_get_sample_fps(const char *);

#endif /* __VB_VA_MOVE_H__ */
