
#ifndef _LIB_QMHAL_H_
#define _LIB_QMHAL_H_
#include <stdio.h>
//#include "QMAPI.h"

typedef enum
{
    QMHAL_IRCUT_STATE_UNKNOWN = -1,
    QMHAL_IRCUT_STATE_DAY,
    QMHAL_IRCUT_STATE_NIGHT,
    QMHAL_IRCUT_STATE_BUTT
}QMHAL_IRCUT_STATE_TYPE_E;

typedef enum
{
    QMHAL_LIGHT_STATE_UNKNOWN = -1,
    QMHAL_LIGHT_STATE_OFF,
    QMHAL_LIGHT_STATE_ON,
    QMHAL_LIGHT_STATE_BLINK,
    QMHAL_LIGHT_STATE_BUTT
}QMHAL_LIGHT_STATE_TYPE_E;

typedef enum
{
    QMHAL_LDR_LEVEL_UNKNOWN = -1,
    QMHAL_LDR_LEVEL_LOW,
    QMHAL_LDR_LEVEL_HIGH,
    QMHAL_LDR_LEVEL_BUTT
}QMHAL_LDR_LEVEL_TYPE_E;

typedef enum
{
    QMHAL_GPIO_LEVEL_UNKNOWN = -1,
    QMHAL_GPIO_LEVEL_LOW,
    QMHAL_GPIO_LEVEL_HIGH,
    QMHAL_GPIO_LEVEL_BUTT
}QMHAL_GPIO_LEVEL_TYPE_E;

/*
  Request GPIO pin
  return: 0: success, -1: failed	
*/
int QMHAL_GPIO_Init();

/*
  Set IRCut state.
  state: reference to QMHAL_IRCUT_STATE_TYPE_E
  return: 0: success, -1: failed	
*/
int QMHAL_IRCUT_SetState(QMHAL_IRCUT_STATE_TYPE_E state);

/*
  Get IRCut state.
  return: reference to QMHAL_IRCUT_STATE_TYPE_E
*/
QMHAL_IRCUT_STATE_TYPE_E QMHAL_IRCUT_GetState();

/*
  Set RLight state.
  state: reference to QMHAL_LIGHT_STATE_TYPE_E
  return: 0: success, -1: failed	
*/
int QMHAL_RLight_SetState(QMHAL_LIGHT_STATE_TYPE_E state);

/*
  Get RLight state.
  return: reference to QMHAL_LIGHT_STATE_TYPE_E
*/
QMHAL_LIGHT_STATE_TYPE_E QMHAL_RLight_GetState();

/*
  Get LDR level.
  return: reference to QMHAL_LDR_LEVEL_TYPE_E
*/
QMHAL_LDR_LEVEL_TYPE_E QMHAL_LDR_GetState();

/*
  Get reset key state.
  return: reference to QMHAL_GPIO_LEVEL_TYPE_E
*/
QMHAL_GPIO_LEVEL_TYPE_E QMHAL_Reset_GetState();

/*
  Set alarm led on/off.
  state: reference to QMHAL_LIGHT_STATE_TYPE_E
  return: 0: success, -1: failed	
*/
int QMHAL_AlarmLed_SetState(QMHAL_LIGHT_STATE_TYPE_E state);

/*
  Set alarm led blink.
  light: millisecond of light period in one blink period cycle
  dark: millisecond of dark period in one blink period cycle
  return: 0: success, -1: failed	
*/
int QMHAL_AlarmLed_SetBlink(int light, int dark);

/*
  Get alarm led state.
  return: reference to QMHAL_LIGHT_STATE_TYPE_E
*/
QMHAL_LIGHT_STATE_TYPE_E QMHAL_AlarmLed_GetState();

/*
  Set system led on/off.
  state: reference to QMHAL_LIGHT_STATE_TYPE_E
  return: 0: success, -1: failed	
*/
int QMHAL_SystemLed_SetState(QMHAL_LIGHT_STATE_TYPE_E state);

/*
  Set system led blink.
  light: millisecond of light period in one blink period cycle
  dark: millisecond of dark period in one blink period cycle
  return: 0: success, -1: failed	
*/
int QMHAL_SystemLed_SetBlink(int light, int dark);

/*
  Get system led state.
  return: reference to QMHAL_LIGHT_STATE_TYPE_E
*/
QMHAL_LIGHT_STATE_TYPE_E QMHAL_SystemLed_GetState();

/*
  Set watchdog enable/disable
  enable: 1: enable watchdog, 0: disable watchdog
  return: 0: success, -1: failed	
*/
int QMHAL_WTD_Enable(int enable);

/*
  Set watchdog timeout
  timeout: millisecond to timeout
  return: 0: success, -1: failed	
*/
int QMHAL_WTD_SetTimeout(int timeout);

/*
  Feed watchdog
*/
void QMHAL_WTD_Feed();

/*
  Set speaker enable/disable
  enable: 1: enable speaker, 0: disable speaker
  return: 0: success, -1: failed	
*/
int QMHAL_SPEAKER_Enable(int enable);

#endif

