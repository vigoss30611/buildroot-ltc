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

#ifndef __VB_VA_QR_H__
#define __VB_VA_QR_H__

#ifndef EVENT_VAQR
#define EVENT_VAQR "vaqrscanner"
#endif

#define DATALEN 128
#define TYPELEN 32

typedef struct {
    int x;
    int y;
} density_t;

struct event_vaqrscanner {
    char type[TYPELEN];
    char data[DATALEN];
};

int va_qr_enable(const char *chn);
int va_qr_disable(const char *chn);
int va_qr_set_density(const char *chn, int x, int y);
int va_qr_get_density(const char *chn, int *x, int *y);

#endif /* __VB_VA_MOVE_H__ */
