#ifndef _ALARM_COMMON_H_
#define _ALARM_COMMON_H_

#define INVALID_ALARM_TIMESTAMP   (0)
#define ALARM_FILE_NAME_LEN   (128)

#define SYS_ALARM_FILE   "/mnt/mmc/alarm_db"


typedef unsigned int  T_AlarmTimestamp;
typedef T_AlarmTimestamp T_AlarmFileKey;

typedef enum {
    E_ALARM_EVENT_MOTION_DETECT = 0,
    E_ALARM_EVENT_SOUND_DETECT,
    E_ALARM_EVENT_TEMPERATURE,
    E_ALARM_EVENT_CNT
} T_ALARM_EVENT_TYPE;

typedef enum {
    E_ALARM_FILE_NA = 0,  // no file output for alarm
    E_ALARM_FILE_VIDEO = 0x01,
    E_ALARM_FILE_PIC = 0x02,
    E_ALARM_FILE_CLOUD_VIDEO = 0x04,
    E_ALARM_FILE_CLOUD_PIC = 0x08,
} T_ALARM_FILE_TYPE;

#ifdef  __cplusplus
	extern "C" {
#endif

int alarm_nameAlarmFile
    (T_ALARM_FILE_TYPE fileType, T_AlarmTimestamp occurTime, T_AlarmFileKey fileKey, char *fileName_out);

/*
    if more than one file are found, put all file names into one string with ',' as separator
*/
int alarm_getRelatedAlarmFile
    (T_ALARM_FILE_TYPE fileType, T_AlarmFileKey fileKey, char *fileList_out, unsigned int *fileCnt_out);

T_AlarmTimestamp alarm_genCurTimestamp();
int alarm_diffTimestamp(T_AlarmTimestamp start, T_AlarmTimestamp end);
T_AlarmTimestamp alarm_convertTimestamp(
        unsigned int year, unsigned int month, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec);
void alarm_timestamp2str(T_AlarmTimestamp timestamp, char *tstr_out, int size_in);
#ifdef  __cplusplus
}
#endif

#endif

