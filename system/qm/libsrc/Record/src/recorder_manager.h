#ifndef __RECORDER_MANAGER_H__
#define __RECORDER_MANAGER_H__
#include "recorder.h"
#include "QMAPIRecord.h" 

int recorder_manager_init(RECORD_ATTR_T *attr, void *callback);
int recorder_manager_deinit(void);
int recorder_manager_start(void);
int recorder_manager_stop(void);

int recorder_manager_update_hd_state(int ready);
int recorder_manager_update_motion_state(int motion);
int recorder_manager_update_time_schedule(int comming);

int recorder_manager_set_attr(RECORD_ATTR_T *attr);
int recorder_manager_get_attr(RECORD_ATTR_T *attr);

int recorder_manager_set_mode(int mode);
int recorder_manager_update_time(void);
int recorder_manager_set_stream(int stream);

int QM_Recorder_Start(void);
int QM_Recorder_Stop(void);

#endif
