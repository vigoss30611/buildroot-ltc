#ifndef _APPRO_SHM_H
#define _APPRO_SHM_H

typedef struct
{
    char filename[256];
    int streamId;
    int duration;
    int frameRate;
    int  needStop;
    int  isAviSaveRunning;
}T_ishm_file_save;

typedef struct {
    unsigned int sdinserted;
   unsigned int freeSpace;    //MB
   unsigned int totalSpace;   //MB
} T_ishm_sdcard;

#endif
