/**
*******************************************************************************
 @file userlog.c

 @brief Implementation of the log functions for android and normal user-mode

 @copyright Imagination Technologies Ltd. All Rights Reserved.

 @license Strictly Confidential.
   No part of this software, either material or conceptual may be copied or
   distributed, transmitted, transcribed, stored in a retrieval system or
   translated into any human or computer language in any form by any means,
   electronic, mechanical, manual or other-wise, or disclosed to third
   parties without the express written permission of
   Imagination Technologies Limited,
   Unit 8, HomePark Industrial Estate,
   King's Langley, Hertfordshire,
   WD4 8LZ, U.K.

******************************************************************************/
#ifndef __ANDROID__
// not used in android builds
#include <stdio.h>
#include <stdarg.h>

/// @brief size of the tmp message
#define CI_LOG_TMP 512

#include <img_defs.h>  // IMG_I64_PR
#ifndef WIN32
#include <stdint.h>
#include <time.h>
#else
#include <stdint.h>
#include <Windows.h>
#define FILETIME_EPOCH (116444736000000000i64)
#define FILETIME_TO_S_DIV (10000000)  // 10^(9-2) ns because of interval
#define FILETIME_TO_US_MULT 10
#define FILETIME_TO_US_MOD (1000000)  // 10^6 us in a second
#define vsnprintf _vsnprintf
#endif

static void getTime_us(int64_t *tsec, int64_t *tusec)
{
#ifdef WIN32
    FILETIME t;
    ULARGE_INTEGER d;
    uint64_t v;

    GetSystemTimeAsFileTime(&t);

    d.LowPart = t.dwLowDateTime;
    d.HighPart = t.dwHighDateTime;

    v = d.QuadPart;  // in 100 ns interval
    v -= FILETIME_EPOCH;

    *tsec = (int64_t)(v / FILETIME_TO_S_DIV);
    *tusec = (int64_t)((v * FILETIME_TO_US_MULT) % FILETIME_TO_US_MOD);
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    *tsec = t.tv_sec;
    *tusec = t.tv_usec;
#endif
}

void LOG_Error(const char *function, unsigned int line, const char *log_tag,
    const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsnprintf(_message_, CI_LOG_TMP, format, args);

    va_end(args);

    fprintf(stderr, "ERROR [%s]: %s():%u %s", log_tag, function, line,
        _message_);

    va_end(args);
}

void LOG_Warning(const char *function, unsigned int line, const char *log_tag,
    const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsnprintf(_message_, CI_LOG_TMP, format, args);

    va_end(args);

    fprintf(stderr, "WARNING [%s]: %s() %s", log_tag, function, _message_);
}

void LOG_Perf(const char *function, unsigned int line, const char *log_tag,
    const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    int64_t ts, tus;
    va_list args;

    getTime_us(&ts, &tus);

    va_start(args, format);

    vsnprintf(_message_, CI_LOG_TMP, format, args);

    va_end(args);

    fprintf(stdout, "PERF [%s]: %s() %"IMG_I64PR"u:%06"IMG_I64PR"u %s",
        log_tag, function, ts, tus, _message_);
}

void LOG_Info(const char *function, unsigned int line, const char *log_tag,
    const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsnprintf(_message_, CI_LOG_TMP, format, args);

    va_end(args);

    fprintf(stdout, "INFO [%s]: %s() %s", log_tag, function, _message_);
}

void LOG_Debug(const char *function, unsigned int line, const char *log_tag,
    const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsnprintf(_message_, CI_LOG_TMP, format, args);

    va_end(args);

    fprintf(stderr, "DEBUG [%s]: %s():%u %s", log_tag, function, line,
        _message_);
}
#endif /* __ANDROID__ */
