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

#ifndef System_H
#define System_H
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define B(n) L2B(0x##n##ull)
#define L2B(n) (int)(((n>>45) & 0x8000) | \
            ((n>>42) & 0x4000) | ((n>>39) & 0x2000) | ((n>>36) & 0x1000) |  \
            ((n>>33) & 0x800) | ((n>>30) & 0x400) | ((n>>27) & 0x200) |  \
            ((n>>24) & 0x100) | ((n>>21) & 0x80) | ((n>>18) & 0x40) |  \
            ((n>>15) & 0x20) | ((n>>12) & 0x10) | ((n>>9) & 0x08) |  \
            ((n>>6) & 0x04) | ((n>>3) & 0x02) | ((n) & 0x01))

#define VBERR_FR_ALLOC "cannot allocate fr buffer"
#define VBERR_FR_NOBUFFER "fr buffer unavailable"
#define VBERR_FR_WIPED "fr buffer is wiped"
#define VBERR_FR_INTERRUPTED "get fr buffer interrupted"
#define VBERR_FR_INVAL "illeagal fr operation"
#define VBERR_FR_HANDLE "wrong fr handle"
#define VBERR_FR_UNKNOWN "unknown fr error"
#define VBERR_FR_MAPERROR "fr map error"

#define VBERR_BADPARAM "invalid parameter"
#define VBERR_WRONGDESC "wrong videobox description file"
#define VBERR_NOIPU "cannot find IPU"
#define VBERR_NOPORT "cannot find Port"
#define VBERR_EVENT "fail with libevent"
#define VBERR_BADLOGIC "the code is implemented in bad logic"
#define VBERR_RUNTIME "runtime error"
#define VBERR_WRONGSTATE "wrong state change"

#define IsError(e, s) (!strncmp(e, s, 128))

class TimePin {
  struct timespec ts;

public:
  TimePin();
  void Poke();
  int Peek();
};

extern void LOGE(const char *fmt, ...);
#define LOGDEBUG LOGE("%s%d\n",__func__,__LINE__)
// if debug, define LOGD(x...) as LOGE(x)
//#define LOGD(x...) LOGE(x)
#define LOGD(x...)

extern int WriteFile(void *buf, int len, const char *fmt, ...);
#endif // System_H
