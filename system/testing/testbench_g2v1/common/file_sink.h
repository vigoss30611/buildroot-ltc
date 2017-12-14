/* Copyright 2013 Google Inc. All Rights Reserved. */

/* Write output to file. */
#ifndef __WRITE_PICTURE_H__
#define __WRITE_PICTURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "software/source/inc/basetype.h"
#include "software/source/inc/decapi.h"

const void* FilesinkOpen(const char* fname);
void FilesinkClose(const void* inst);
void FilesinkWritePic(const void* inst, struct DecPicture pic);
void FilesinkWriteSinglePic(const void* inst, struct DecPicture pic);

#ifdef __cplusplus
}
#endif

#endif /* __WRITE_PICTURE_H__ */
