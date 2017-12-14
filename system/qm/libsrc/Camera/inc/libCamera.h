/******************************************************************************
******************************************************************************/

#ifndef __LIBCAMERA_H__
#define __LIBCAMERA_H__

#include "QMAPIType.h"
#include "QMAPINetSdk.h"


//int Camera_Set_Video_Input_Type(int video_input_type);
//int Camera_Get_Video_Input_Type( void );
int Camera_Get_Video_Input_Num( void );

int Camera_Vadc_SetColor(int nChannel, unsigned int cmd, unsigned int val);
int Camera_Get_Default_Color(QMAPI_NET_CHANNEL_COLOR  *lpColor);
int Camera_Get_Default_Enhanced_Color(QMAPI_NET_CHANNEL_ENHANCED_COLOR  *lpEnhancedColor);
int Camera_Get_Color_Support(QMAPI_NET_COLOR_SUPPORT  *lpColorSupport);
int Camera_Get_Enhanced_Color_Support(QMAPI_NET_ENHANCED_COLOR_SUPPORT  *lpEnhancedColorSupport);


int Camera_Set_Video_Color(QMAPI_NET_CHANNEL_COLOR_SINGLE *lpstColorSet,  QMAPI_NET_CHANNEL_COLOR *lpColorParam, QMAPI_NET_CHANNEL_ENHANCED_COLOR *lpEnancedColor);

#endif
