#ifndef _SHM_H_
#define _SHM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info_log.h"
#include "osa_ipc.h"
#include "av_osa_ipc.h"
#include "common_inft_data.h"
#include <sys/time.h>

#define __D(fmt, args...) printf( fmt, ## args)
#define __E(fmt, args...) printf( fmt, ## args)


#ifndef NULL
#define NULL 0
#endif
// at most we support 3 video input

#ifdef SPORTDV_PROJECT
#define SHM_AV_BLK_CNT  (150+25)
#define SHM_JPEG_BLK_SIZE  1024*10
#else
#define SHM_AV_BLK_CNT  (150+75)
#define SHM_JPEG_BLK_SIZE  1024*20
#endif
#define SHM_AV_BLK_SIZE_DIV_PARAM  40
#define SHM_AUDIO_BLK_CNT  512
#define SHM_AUDIO_BLK_SIZE 1024*2
#define SHM_EXTRA_SIZE  256
#define SHM_INVALID_SERIAL -1
#define ENTRY_VIDOE_INFO_CNT  (ENTRY_VIDOE_INFO_LOW+1)


typedef enum {
	DUMMY_FRAME = -2,
	EMPTY_FRAME = -1,
	AUDIO_FRAME = 0,
	JPEG_FRAME,
	I_FRAME,
	P_FRAME,
	B_FRAME,
	SPS_FRAME,
	END_FRAME_TYPE
} SHM_FRAME_TYPE;

typedef struct _VIDEO_FRAME {
	int serial;
	SHM_FRAME_TYPE fram_type;
	int blkindex;
	int blks;
	int realsize;
	int flag;
	struct timeval timestamp_frm;
	int ref_serial[ENTRY_INFO_CNT];
} VIDEO_FRAME;

typedef struct _VIDEO_GOP
{
	int				last_Start;
	int				last_Start_serial;
	int				last_End;
	int				last_End_serial;
	int				lastest_I;
	int				lastest_I_serial;
}VIDEO_GOP;


typedef struct _VIDEO_BLK_INFO {
	int video_type;
	int width;
	int height;
	int extrasize;
	int blk_sz;
	int blk_num;
	int blk_free;
	int frame_num;
	int idle_frame_cnt; //fengwu add
	int cur_frame;
	int cur_serial;
	int del_serial;
	int cur_blk;
	VIDEO_GOP	gop;
	struct timeval timestamp_blk; //fengwu this time is for last frame save time 0401
	unsigned char extradata[SHM_EXTRA_SIZE];
	VIDEO_FRAME frame[SHM_AUDIO_BLK_CNT];
	long dataAddrsss; // real data, here only show 4 byte
} VIDEO_BLK_INFO;

typedef struct _MEM_INFO {
	int video_info_nums;
	unsigned long video_info_size[ENTRY_INFO_CNT];
} MEM_MNG_INFO;



#ifdef __cplusplus
extern "C" {
#endif

/* extern functions */
MEM_MNG_INFO* MMng_Init(); 
int MMng_GetShmEntryType(unsigned int streamIndex);
unsigned long MMng_GetMemSize();
int MMng_GetSerial_new(SHM_ENTRY_TYPE idx, int **ppRefSial);
int MMng_GetSerial_I(SHM_ENTRY_TYPE idx, int **ppRefSial);
int MMng_GetAudioSerialByTimestamp(SHM_ENTRY_TYPE idx, struct timeval timestamp);
int MMng_IsValidSerial(SHM_ENTRY_TYPE idx, int serial);
void MMng_Read(SHM_ENTRY_TYPE idx, int serialNO, unsigned char** ppData, int *sz, struct timeval *ptime, int *pIsKey);
int MMng_Write(SHM_ENTRY_TYPE idx, int frame_type,unsigned char *pData, int size  );
int MMng_Put_Extra(SHM_ENTRY_TYPE idx, unsigned char *pData, int size);
int MMng_Get_Extra(SHM_ENTRY_TYPE idx, unsigned char **ppData, int *sz);
int MMng_Set_timestamp(SHM_ENTRY_TYPE idx, struct timeval timestamp);



/* internal functions, don't use external */
void*  MMng_AllocShMem(int size);
MEM_MNG_INFO *MMng_GetShMem();
void  MMng_FreeShMem() ;
void MMng_dumpVInfo(SHM_ENTRY_TYPE idx);
void MMng_dumpMemInfo(MEM_MNG_INFO * pMemInfo);
void MMng_dumpFrmInfo(SHM_ENTRY_TYPE idx, int serialNO, const char* str);

void MMng_dumpBusyFrame(VIDEO_BLK_INFO *pVInfo);
void MMng_dumpIdleCnt(VIDEO_BLK_INFO *pVInfo);
void MMng_reset(VIDEO_FRAME * pframe);

int MMng_Config_All(MEM_MNG_INFO * pMemInfo);
int MMng_Video_Jpeg_Reset(SHM_ENTRY_TYPE shmType);
int MMng_caculate_all(MEM_MNG_INFO *pMemInfo);
int MMng_Free(VIDEO_BLK_INFO * pVidInfo, int frame_id);
int MMng_Insert(void *pData, int size, int blks,
               int frame_type, SHM_ENTRY_TYPE idx);

VIDEO_BLK_INFO *MMng_GetBlkInfo(SHM_ENTRY_TYPE idx);
int MMng_GetSerial_Del(SHM_ENTRY_TYPE idx);


VIDEO_FRAME *MMng_GetFrame_current(VIDEO_BLK_INFO * video_info);
VIDEO_FRAME *MMng_GetFrame_BySerial(int serial, SHM_ENTRY_TYPE idx);
VIDEO_FRAME *MMng_GetFrame_LastI(VIDEO_BLK_INFO * video_info);
SHM_ENTRY_TYPE MMng_Get_Entry_Type(int width);


#ifdef __cplusplus
}
#endif
#endif
