#ifndef _ALARM_DB_COMMON_H_
#define _ALARM_DB_COMMON_H_

#include "alarm_common.h"

#define MAX_ALARM_DB_REC_CNT    (200)    // TODO: use config instead
#define ALARM_DB_HEADER_SIZE   (sizeof(T_AlarmDBHeader))
#define ALARM_DB_REC_SIZE   (sizeof(T_AlarmDBRecord) * (MAX_ALARM_DB_REC_CNT))
#define MAX_ALARM_DB_SIZE   ((ALARM_DB_HEADER_SIZE) + (ALARM_DB_REC_SIZE))

typedef struct {
    unsigned int recCount;
    unsigned int recCursor;
} T_AlarmDBHeader;

typedef struct {
    T_AlarmTimestamp occurTime;
    T_ALARM_FILE_TYPE fileType;
    T_AlarmFileKey fileKey;
    T_ALARM_EVENT_TYPE eventType;
    unsigned int deleted;
} T_AlarmDBRecord;

typedef struct {
    T_AlarmDBHeader header;
    T_AlarmDBRecord rec; // the first record
} T_AlarmDBTable;


/*
    @return: (==0)equal; (>0)greater; (<1)less
*/
typedef int (*ALARM_DB_REC_CMP)(T_AlarmDBRecord *recA, T_AlarmDBRecord *recB);


int alarm_db_checkStorage();
int alarm_db_openStorage(unsigned int writable);

/* basic cursor operations */
int alarm_db_getRecCnt();
int alarm_db_getFirstRec(T_AlarmDBRecord **rec_out);
int alarm_db_getLastRec(T_AlarmDBRecord **rec_out);
T_AlarmDBRecord* alarm_db_getRecAt(int pos);
int alarm_db_nextRec(T_AlarmDBRecord *curRec, T_AlarmDBRecord **nextRec_out);
int alarm_db_preRec(T_AlarmDBRecord *curRec, T_AlarmDBRecord **preRec_out);

/* db search algorithm */
int alarm_db_bsearchForCircularAry(T_AlarmDBRecord *rec, ALARM_DB_REC_CMP cmpFunc);

/* compare function for query */
int alarm_db_cmpOccurTime(T_AlarmDBRecord *recA, T_AlarmDBRecord *recB);
int alarm_db_cmpFileKey(T_AlarmDBRecord *recA, T_AlarmDBRecord *recB);

/* debug */
void alarm_db_printRec(T_AlarmDBRecord *rec);
void alarm_db_printAllRec();


#endif

