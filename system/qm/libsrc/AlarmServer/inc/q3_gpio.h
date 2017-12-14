#ifndef __Q3_GPIO_H__
#define __Q3_GPIO_H__

typedef enum __LED_STATE
{
    LED_STATE_OFF = 0,
    LED_STATE_ON,
    LED_STATE_SLOW_BLINK,
    LED_STATE_FAST_BLINK,
    LED_STATE_BUTT,
}LED_STATE_E;


int DMS_DEV_GPIO_Init_Ext(int IRcutMode);

//0---day   1----night
int DMS_DEV_GPIO_SetIRCutState(int nDay);
int DMS_DEV_GPIO_GetResetState(DWORD *lpdwState);
int DMS_DEV_GPIO_SetWifiLed(LED_STATE_E eState);
int DMS_DEV_GPIO_SetSpeakerOnOff(int nOn);

void detectIOThread(void);

#endif

