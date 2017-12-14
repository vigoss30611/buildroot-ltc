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

#include <System.h>

TimePin::TimePin()
{
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
}

void TimePin::Poke()
{
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
}

int TimePin::Peek()
{
  struct timespec t2;
  clock_gettime(CLOCK_MONOTONIC_RAW, &t2);

  return t2.tv_sec * 10000 + t2.tv_nsec / 100000 -
    ts.tv_sec * 10000 - ts.tv_nsec / 100000;
}

void LOGE(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    fflush(stdout);
}

int WriteFile(void *buf, int len, const char *fmt, ...)
{
    char path[256];
    va_list ap;
    int fd;

    va_start(ap, fmt);
    vsnprintf(path, 256, fmt, ap);
    va_end(ap);

    fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd < 0) {
        LOGE("Cannot create file %s\n", path);
        return -1;
    }

    write(fd, buf, len);
    close(fd);
    return 0;
}
