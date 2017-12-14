
#ifndef __VIDEO_MARKER_H__
#define __VIDEO_MARKER_H__

#include "QMAPINetSdk.h"

int QMAPI_Video_MarkerInit(void);
void QMAPI_Video_MarkerUninit(void);

void QMAPI_Video_MarkerStart(void);
void QMAPI_Video_MarkerStop(void);
int  QMAPI_Video_Marker_IOCtrl(int Handle, int nCmd, int nChannel, void *lpInParam, int nInSize);

#endif
