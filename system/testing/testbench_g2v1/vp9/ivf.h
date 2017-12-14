/****************************************************************************
 *
 *   Module Title : ivf.h
 *
 *   Description  : On2 Intermediate video file format header
 *
 *   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
 *
 ***************************************************************************/

#ifndef __INC_IVF_H
#define __INC_IVF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32
#define __int64 long long
#endif

typedef struct {
  unsigned char signature[4];  //='DKIF';
  unsigned short version;      //= 0;
  unsigned short headersize;   //= 32;
  unsigned int FourCC;
  unsigned short width;
  unsigned short height;
  unsigned int rate;
  unsigned int scale;
  unsigned int length;
  unsigned char unused[4];
} IVF_HEADER;

#pragma pack(4)
typedef struct {

  unsigned int frame_size;
  unsigned __int64 time_stamp;

} IVF_FRAME_HEADER;
#pragma pack()

extern void InitIVFHeader(IVF_HEADER *ivf);

#ifdef __cplusplus
}
#endif

#endif

