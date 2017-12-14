/**
******************************************************************************
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

void LOG_Error(const char *function, unsigned int line, const char *log_tag, const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsprintf(_message_, format, args);

    va_end(args);

    fprintf(stderr, "ERROR [%s]: %s():%u %s", log_tag, function, line, _message_);

    va_end(args);
}

void LOG_Warning(const char *function, unsigned int line, const char *log_tag, const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsprintf(_message_, format, args);

    va_end(args);

    fprintf(stderr, "WARNING [%s]: %s() %s", log_tag, function, _message_);
}

void LOG_Info(const char *function, unsigned int line, const char *log_tag, const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsprintf(_message_, format, args);

    va_end(args);

    fprintf(stdout, "INFO [%s]: %s() %s", log_tag, function, _message_);
}

void LOG_Debug(const char *function, unsigned int line, const char *log_tag, const char* format, ...)
{
    char _message_[CI_LOG_TMP];
    va_list args;

    va_start(args, format);

    vsprintf(_message_, format, args);

    va_end(args);

    fprintf(stderr, "DEBUG [%s]: %s():%u %s", log_tag, function, line, _message_);
}
#endif /* __ANDROID__ */
