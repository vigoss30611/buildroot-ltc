/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Write MD5 sum for each picture to a file. */
#ifndef __MD5_SINK_H__
#define __MD5_SINK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "software/source/inc/basetype.h"
#include "software/source/inc/decapi.h"

/* MD5 sink functions for calculating sequence checksum. */
const void* Md5sinkOpen(const char* fname);
void Md5sinkClose(const void* inst);
void Md5sinkWritePic(const void* inst, struct DecPicture pic);

/* MD5 sink functions for calculating per picture checksums. */
const void* md5perpicsink_open(const char* fname);
void md5perpicsink_close(const void* inst);
void md5perpicsink_write_pic(const void* inst, struct DecPicture pic);

#ifdef __cplusplus
}
#endif

#endif /* __MD5_SINK_H__ */
