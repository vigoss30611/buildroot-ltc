/****************************************************************************
 *
 *   Module Title : ivf.c
 *
 *   Description  : On2 Intermediate video file format header
 *
 *   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
 *
 ***************************************************************************/

#include "ivf.h"
#include <memory.h>
#include <string.h>

void InitIVFHeader(IVF_HEADER *ivf) {
  memset(ivf, 0, sizeof(IVF_HEADER));
  strncpy((char *)(ivf->signature), "DKIF", 4);
  ivf->version = 0;
  ivf->headersize = 32;
}