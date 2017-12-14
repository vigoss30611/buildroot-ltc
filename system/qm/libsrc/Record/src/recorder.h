#ifndef __INFOTM_RECORDER_H__
#define __INFOTM_RECORDER_H__
#include "QMAPIRecord.h" 

/* !< CFG_FEATURE_RECORD */
#define CFG_RECORD_STORE_MOUNTPOINT         "/mnt/sd0"
#define CFG_RECORD_STORE_PATH               "/mnt/sd0/"
#define CFG_RECORD_CONTAINER_TYPE           "mkv"   /* !< mkv */
#define CFG_RECORD_SLICE_DURATION           120/* !< second */
#define CFG_RECORD_AUDIO_FORMAT              MEDIA_REC_AUDIO_FORMAT_G711A;
#define CFG_RECORD_AV_FORMAT                      MEDIA_REC_AV_FORMAT_MKV;
#define CFG_RECORD_STORE_LOW_THRESHOLD      (128 * 1024 * 1024)

typedef void (*infotm_record_callback)(int event, const char *filename);

int infotm_record_init(RECORD_ATTR_T *attr, infotm_record_callback callback);
int infotm_record_deinit(void);

int infotm_record_start(void);
int infotm_record_stop(void);

int infotm_record_section(void);
#endif
