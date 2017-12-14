#ifndef __SPV_LDWS_FCWS_H__
#define __SPV_LDWS_FCWS_H__

#include "adas_gui_binder.h"

enum {
    ADAS_DETECTION_DEINIT,
    ADAS_DETECTION_INIT,
    ADAS_DETECTION_STOPPED,
    ADAS_DETECTION_RUNNING,
};

int GetLDDetectionState();

int ConfigLDDetection(ADAS_LDWS_CONFIG *ldConfig, ADAS_FCWS_CONFIG *fcConfig);

int StartLFDetection();

int StopLFDetection();

int InitLFDetection();

int DeinitLFDetection();

#endif
