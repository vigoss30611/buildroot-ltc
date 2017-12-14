#ifndef __QMAPI_ALARMPRO_H__
#define __QMAPI_ALARMPRO_H__

#include "QMAPIType.h"
#include "TLNVSDK.h"

//#include <time.h>
//#include "tl_common.h"
//#include "tlsmtp.h"
//#include "QMAPI.h"

///Alarm Message
typedef struct _TL_VIDEO_ALARM_QUEUE
{
    DWORD           index; ///alarm index
    TL_SERVER_MSG       servermsg[10]; ///alarm message 
}TL_VIDEO_ALARM_QUEUE , *PTL_VIDEO_ALARM_QUEUE;

typedef struct _TL_VIDEO_IO_ALARM_DYNAMIC_MANAGE
{
    BOOL AlarmOut[4];
    unsigned int	AlarmOuttimes[4];
    unsigned int	AlarmNotifytimes[4];
}TL_VIDEO_IO_ALARM_DYNAMIC_MANAGE;


void TL_Alarm_Thread();
void TL_Video_Alarm_Thread();

int TL_Video_Motion_Alarm_Initial(int TL_Channel);


#endif
