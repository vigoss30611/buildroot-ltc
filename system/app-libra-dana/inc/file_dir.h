#ifndef _FILE_DIR_H
#define _FILE_DIR_H

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>


//#define NULL 0
#define VIDEO_DIR_NAME  "/mnt/mmc/DCIM/100CVR"
#define JPEG_DIR_NAME  "/mnt/mmc/DCIM/100CVR"
#define MMC_PATH "/mnt/mmc"
#define SD_FOLDER "DCIM"
#define FILE_NAME_TYPE "CVR"

enum {
	SDCARD_FREE_SPACE_IS_SAFE,		
	SDCARD_FREE_SPACE_MIN,
	SDCARD_FREE_SPACE_IS_NOT_ENOUGH,	
	SDCARD_FREE_SPACE_ZERO,                 //maybe read only
	SDCARD_FREE_SPACE_NUM
};

#define KSIZE(size) (size * 1024L)
#define MSIZE(size) (size * KSIZE(1024)) 

#define SDCARD_FREE_SPACE_MIN_VALUE              200
#define SDCARD_FREE_SPACE_NOT_ENOUGH_VALUE       100

#define SDCARD_FREE_SPACE_RESERVE                50
#define SDCARD_FREE_SPACE_PHOTO_NOT_ENOUGH_VALUE 10
#define SDCARD_FREE_SPACE_ZERO_VALUE             0


#ifdef __cplusplus 
extern "C" {
#endif

int file_getDirFileCnt(char *dirName);
int file_getDirFileNameByIdx(char *dirName, int idx, char *fname_out);


#ifdef __cplusplus 
}
#endif

#endif
