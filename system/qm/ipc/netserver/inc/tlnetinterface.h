/******************************************************************************
******************************************************************************/


#include "QMAPIType.h"

#ifndef _TL_VIDEO_INTERFACE_
#define _TL_VIDEO_INTERFACE_

/*
set send buffer's size
if (DEVICE_MEGA_IPCAM) || (DEVICE_MEGA_1080P) || (DEVICE_MEGA_960P))
	 nBufNum = (1024*30);
else
	nBufNum = (300);
*/

int TL_net_server_init(int serverMediaPort, int nBufNum);

int TL_net_server_start();

#endif
