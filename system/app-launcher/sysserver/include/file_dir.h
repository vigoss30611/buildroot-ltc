#ifndef _FILE_DIR_H
#define _FILE_DIR_H

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>


#define VIDEO_DIR "Video"
#define PHOTO_DIR "Photo"
#define LOCK_DIR  "Locked"

enum {
	SDCARD_FREE_SPACE_IS_SAFE,		
	SDCARD_FREE_SPACE_MIN,
	SDCARD_FREE_SPACE_IS_NOT_ENOUGH,	
	SDCARD_FREE_SPACE_ZERO,                 //maybe read only
	SDCARD_FREE_SPACE_NUM
};

#define KSIZE(size) (size * 1024L)
#define MSIZE(size) (size * KSIZE(1024)) 

#define SDCARD_FREE_SPACE_MIN_VALUE              400
#define SDCARD_FREE_SPACE_NOT_ENOUGH_VALUE       100

#define SDCARD_FREE_SPACE_RESERVE                50
#define SDCARD_FREE_SPACE_PHOTO_NOT_ENOUGH_VALUE 10
#define SDCARD_FREE_SPACE_ZERO_VALUE             0

#endif
