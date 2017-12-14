#ifndef _ALARM_DB_CLIENT_H_
#define _ALARM_DB_CLIENT_H_

#include "alarm_db_common.h"

/* please use curRec for each loop */
#define F_ALARM_DB_TRAVERSE(_start, _cnt) \
    T_AlarmDBRecord *preRec; \
    T_AlarmDBRecord *curRec; \
    unsigned int _i; \
    int _ret; \
    for (curRec = (_start), preRec = NULL, _i = 0, _ret = 0; \
           (_i < (_cnt)) && (_ret == 0); \
           preRec = curRec, _ret = alarm_db_nextRec(preRec, &curRec), _i++)


#ifdef  __cplusplus
    extern "C" {
#endif


/* basic query operations */
int alarm_db_queryRec(T_AlarmDBRecord *rec, ALARM_DB_REC_CMP cmp, T_AlarmDBRecord *rec_out);
int alarm_db_queryRecs
        (T_AlarmDBRecord *fromRec, T_AlarmDBRecord *toRec, ALARM_DB_REC_CMP cmp, T_AlarmDBRecord *firstRec_out, unsigned int size_in, unsigned int *cnt_out);

/* extension query operations */
int alarm_db_queryRec_byOccurTime(T_AlarmTimestamp occurTime, T_AlarmDBRecord *rec_out);
int alarm_db_queryRecs_byOccurTime(T_AlarmTimestamp from, T_AlarmTimestamp to, T_AlarmDBRecord *rec_out, unsigned int size_in, unsigned int *cnt_out);
int alarm_db_queryRec_byFileKey(T_AlarmFileKey fileKey, T_AlarmDBRecord *rec_out);
int alarm_db_queryRecsWithUniqKey_byOccurTime
    (T_AlarmTimestamp from, T_AlarmTimestamp to, T_AlarmDBRecord *rec_out, unsigned int size_in, unsigned int *cnt_out);

#ifdef  __cplusplus
}
#endif

#endif


