/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Tracing macros to wrap around decoder functions.
   Warning! Tested with GNU C only, *NOT* ISO C compatible! */

#ifndef DECAPI_TRACE_H
#define DECAPI_TRACE_H

#if 0
/* TODO(vmr): Make wrappers for other functions as well. */
#define HevcDecDecode(inst, input, output)                                  \
  ({                                                                        \
    static const char* codes[] = {                                          \
        "HEVCDEC_OK",                  "HEVCDEC_STRM_PROCESSED",            \
        "HEVCDEC_PIC_RDY",             "HEVCDEC_PIC_DECODED",               \
        "HEVCDEC_HDRS_RDY",            "HEVCDEC_ADVANCED_TOOLS",            \
        "HEVCDEC_PENDING_FLUSH",       "HEVCDEC_NONREF_PIC_SKIPPED",        \
        "HEVCDEC_END_OF_STREAM",       "HEVCDEC_PARAM_ERROR",               \
        "HEVCDEC_STRM_ERROR",          "HEVCDEC_NOT_INITIALIZED",           \
        "HEVCDEC_MEMFAIL",             "HEVCDEC_INITFAIL",                  \
        "HEVCDEC_HDRS_NOT_RDY",        "HEVCDEC_STREAM_NOT_SUPPORTED",      \
        "HEVCDEC_HW_RESERVED",         "HEVCDEC_HW_TIMEOUT",                \
        "HEVCDEC_HW_BUS_ERROR",        "HEVCDEC_SYSTEM_ERROR",              \
        "HEVCDEC_DWL_ERROR",           "HEVCDEC_EVALUATION_LIMIT_EXCEEDED", \
        "HEVCDEC_FORMAT_NOT_SUPPORTED"};                                    \
    static i32 i = 0, count = 0, sum = 0;                                   \
    for (i = 0; i < (input)->data_len; i++) sum += (input)->stream[i];      \
    printf("HevcDecDecode #%i: len=%u, sum=%i\n", ++count, i, sum);         \
    enum HevcDecRet ret = HevcDecDecode(inst, input, output);               \
    printf("HevcDecDecode returned %s: data_left=%u\n", codes[ret],         \
           (output)->data_left);                                            \
    ret;                                                                    \
  })
#endif

#endif /* DECAPI_TRACE_H */
