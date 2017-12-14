#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <qsdk/sys.h>
//#include <qsdk/sensors.h>
#include "libQMHAL.h"

#define QMHAL_CFG_IRCUT_DNP_PIN  122
#define QMHAL_CFG_IRCUT_DNN_PIN  123
#define QMHAL_CFG_IRLED_PIN  112
#define QMHAL_CFG_LIGHT_SENSOR 110
#define QMHAL_CFG_LDR_SENSE_THRESHOLD  12
#define QMHAL_CFG_RESET_KEY_PIN   119
//#define QMHAL_CFG_ALRAM_LED_NAME  "red"
#define QMHAL_CFG_SYSTEM_LED_NAME "green"
#define QMHAL_CFG_SPEAKER_PIN  121

QMHAL_IRCUT_STATE_TYPE_E g_IRCut_state = QMHAL_IRCUT_STATE_UNKNOWN;

int QMHAL_GPIO_Init()
{
    if(system_request_pin(QMHAL_CFG_IRCUT_DNP_PIN))
    {
        printf("%s  %d, request gpio %d err !!\n", __FUNCTION__, __LINE__, QMHAL_CFG_IRCUT_DNP_PIN);
        //return -1;
    }

    if(system_request_pin(QMHAL_CFG_IRCUT_DNN_PIN))
    {
        printf("%s  %d, request gpio %d err !!\n", __FUNCTION__, __LINE__, QMHAL_CFG_IRCUT_DNN_PIN);
        //return -1;
    }

    if(system_request_pin(QMHAL_CFG_IRLED_PIN))
    {
        printf("%s  %d, request gpio %d err !!\n", __FUNCTION__, __LINE__, QMHAL_CFG_IRLED_PIN);
        //return -1;
    }

    if(system_request_pin(QMHAL_CFG_LIGHT_SENSOR))
    {
        printf("%s  %d, request gpio %d err !!\n", __FUNCTION__, __LINE__, QMHAL_CFG_LIGHT_SENSOR);
        //return -1;
    }

    if(system_request_pin(QMHAL_CFG_RESET_KEY_PIN))
    {
        printf("%s  %d, request gpio %d err !!\n", __FUNCTION__, __LINE__, QMHAL_CFG_RESET_KEY_PIN);
        //return -1;
    }

    if(system_request_pin(QMHAL_CFG_SPEAKER_PIN))
    {
        printf("%s  %d, request gpio %d err !!\n", __FUNCTION__, __LINE__, QMHAL_CFG_SPEAKER_PIN);
        //return -1;
    }

    return 0;
}

int QMHAL_IRCUT_SetState(QMHAL_IRCUT_STATE_TYPE_E state)
{
    int ret = -1;

    printf("%s  %d, state = %d\n", __FUNCTION__, __LINE__, state);

    switch (state)
    {
        case QMHAL_IRCUT_STATE_DAY:
            if (g_IRCut_state != QMHAL_IRCUT_STATE_DAY)
            {
                system_set_pin_value(QMHAL_CFG_IRCUT_DNP_PIN, 1);
                system_set_pin_value(QMHAL_CFG_IRCUT_DNN_PIN, 0);
                usleep(200 * 1000);
                system_set_pin_value(QMHAL_CFG_IRCUT_DNP_PIN, 0);
                system_set_pin_value(QMHAL_CFG_IRCUT_DNN_PIN, 0);
                g_IRCut_state = state;
            }
            ret = 0;
            break;
        case QMHAL_IRCUT_STATE_NIGHT:
            if (g_IRCut_state != QMHAL_IRCUT_STATE_NIGHT)
            {
                system_set_pin_value(QMHAL_CFG_IRCUT_DNP_PIN, 0);
                system_set_pin_value(QMHAL_CFG_IRCUT_DNN_PIN, 1);
                usleep(200 * 1000);
                system_set_pin_value(QMHAL_CFG_IRCUT_DNP_PIN, 0);
                system_set_pin_value(QMHAL_CFG_IRCUT_DNN_PIN, 0);
                g_IRCut_state = state;
            }
            ret = 0;
            break;
        default:
            printf("%s  %d, state(%d) out of range!!\n", __FUNCTION__, __LINE__, state);
            ret = -1;
    }

    return ret;
}

QMHAL_IRCUT_STATE_TYPE_E QMHAL_IRCUT_GetState()
{
    QMHAL_IRCUT_STATE_TYPE_E state = QMHAL_IRCUT_STATE_UNKNOWN;

    //printf("%s  %d\n", __FUNCTION__, __LINE__);

    state = g_IRCut_state;

    return state;
}

int QMHAL_RLight_SetState(QMHAL_LIGHT_STATE_TYPE_E state)
{
    int ret = -1;

    printf("%s  %d, state = %d\n", __FUNCTION__, __LINE__, state);

    switch (state)
    {
        case QMHAL_LIGHT_STATE_ON:
            //system_set_pin_value(QMHAL_CFG_IRLED_PIN, 1);
            ret = 0;
            break;
        case QMHAL_LIGHT_STATE_OFF:
            //system_set_pin_value(QMHAL_CFG_IRLED_PIN, 0);
            ret = 0;
            break;
        default:
            printf("%s  %d, invalid parameter state = %d!!\n", __FUNCTION__, __LINE__, state);
            ret = -1;
    }

    return ret;
}

QMHAL_LIGHT_STATE_TYPE_E QMHAL_RLight_GetState()
{
    QMHAL_LIGHT_STATE_TYPE_E state = QMHAL_LIGHT_STATE_UNKNOWN;
    int val;

    //printf("%s  %d\n", __FUNCTION__, __LINE__);

    val = system_get_pin_value(QMHAL_CFG_IRLED_PIN);
    if (val>=0)
    {
        state = (val==1) ? QMHAL_LIGHT_STATE_ON : QMHAL_LIGHT_STATE_OFF;
    }
    else
    {
        printf("%s get IRLed pin err: %d\n", __FUNCTION__, val);
    }

    return state;
}

QMHAL_LDR_LEVEL_TYPE_E QMHAL_LDR_GetState()
{
    QMHAL_LDR_LEVEL_TYPE_E state = QMHAL_LDR_LEVEL_UNKNOWN;
    int val;

    //printf("%s  %d\n", __FUNCTION__, __LINE__);
#if 0
    val = sensors_get_luminous();
    if (val != -1)
    {
        state = (val>QMHAL_CFG_LDR_SENSE_THRESHOLD) ? QMHAL_LDR_LEVEL_HIGH : QMHAL_LDR_LEVEL_LOW;
    }
    else
    {
        state = QMHAL_LDR_LEVEL_UNKNOWN;
    }
#endif

    val = system_get_pin_value(QMHAL_CFG_LIGHT_SENSOR);
    if (val>=0)
    {
        state = (val==1) ? QMHAL_LDR_LEVEL_HIGH : QMHAL_LDR_LEVEL_LOW;
    }
    else
    {
        printf("%s get light sensor pin err: %d\n", __FUNCTION__, val);
    }

    return state;
}

QMHAL_GPIO_LEVEL_TYPE_E QMHAL_Reset_GetState()
{
    QMHAL_GPIO_LEVEL_TYPE_E state = QMHAL_GPIO_LEVEL_UNKNOWN;
    int val;

    //printf("%s  %d\n", __FUNCTION__, __LINE__);

    val = system_get_pin_value(QMHAL_CFG_RESET_KEY_PIN);
    if (val>=0)
    {
        state = (val==1) ? QMHAL_GPIO_LEVEL_HIGH : QMHAL_GPIO_LEVEL_LOW;
    }
    else
    {
        printf("%s get reset key pin err: %d\n", __FUNCTION__, val);
    }

    return state;
}

int QMHAL_AlarmLed_SetState(QMHAL_LIGHT_STATE_TYPE_E state)
{
    int ret = -1;

    printf("%s  %d, state = %d\n", __FUNCTION__, __LINE__, state);

#ifdef QMHAL_CFG_ALRAM_LED_NAME
    switch (state)
    {
        case QMHAL_LIGHT_STATE_ON:
            ret = system_set_led(QMHAL_CFG_ALRAM_LED_NAME, 1, 0, 0);
            break;
        case QMHAL_LIGHT_STATE_OFF:
            ret = system_set_led(QMHAL_CFG_ALRAM_LED_NAME, 0, 0, 0);
            break;
        default:
            printf("%s  %d, invalid parameter state = %d!!\n", __FUNCTION__, __LINE__, state);
            ret = -1;
    }
#endif
            
    return ret;
}

int QMHAL_AlarmLed_SetBlink(int light, int dark)
{
    int ret = -1;

    printf("%s  %d, light=%d, dark=%d\n", __FUNCTION__, __LINE__, light, dark);

#ifdef QMHAL_CFG_ALRAM_LED_NAME
    ret = system_set_led(QMHAL_CFG_ALRAM_LED_NAME, light, dark, 1);
#endif

    return ret;
}

QMHAL_LIGHT_STATE_TYPE_E QMHAL_AlarmLed_GetState()
{
    QMHAL_LIGHT_STATE_TYPE_E state = QMHAL_LIGHT_STATE_UNKNOWN;

    //TODO:

    return state;
}

int QMHAL_SystemLed_SetState(QMHAL_LIGHT_STATE_TYPE_E state)
{
    int ret = -1;

    printf("%s  %d, state = %d\n", __FUNCTION__, __LINE__, state);

    switch (state)
    {
        case QMHAL_LIGHT_STATE_ON:
            ret = system_set_led(QMHAL_CFG_SYSTEM_LED_NAME, 1, 0, 0);
            break;
        case QMHAL_LIGHT_STATE_OFF:
            ret = system_set_led(QMHAL_CFG_SYSTEM_LED_NAME, 0, 0, 0);
            break;
        default:
            printf("%s  %d, invalid parameter state = %d!!\n", __FUNCTION__, __LINE__, state);
            ret = -1;
    }
            
    return ret;
}

int QMHAL_SystemLed_SetBlink(int light, int dark)
{
    int ret = -1;

    printf("%s  %d, light=%d, dark=%d\n", __FUNCTION__, __LINE__, light, dark);
    ret = system_set_led(QMHAL_CFG_SYSTEM_LED_NAME, light, dark, 1);

    return ret;
}

QMHAL_LIGHT_STATE_TYPE_E QMHAL_SystemLed_GetState()
{
    QMHAL_LIGHT_STATE_TYPE_E state = QMHAL_LIGHT_STATE_UNKNOWN;

    //TODO:

    return state;
}

int QMHAL_WTD_Enable(int enable)
{
    int ret = -1;

    printf("%s  %d, enable = %d\n", __FUNCTION__, __LINE__, enable);

    if (enable)
    {
        ret = system_enable_watchdog();
    }
    else
    {
        ret = system_disable_watchdog();
    }

    return ret;
}

int QMHAL_WTD_SetTimeout(int timeout)
{
    int ret = -1;

    printf("%s  %d, timeout = %d\n", __FUNCTION__, __LINE__, timeout);

    if (timeout>0)
    {
        ret = system_set_watchdog(timeout);
    }

    return ret;
}

void QMHAL_WTD_Feed()
{
    system_feed_watchdog();

    return;
}

int QMHAL_SPEAKER_Enable(int enable)
{
    int ret = -1;

    printf("%s  %d, enable = %d\n", __FUNCTION__, __LINE__, enable);

    if (enable)
    {
        ret = system_set_pin_value(QMHAL_CFG_SPEAKER_PIN, 1);
    }
    else
    {
        ret = system_set_pin_value(QMHAL_CFG_SPEAKER_PIN, 0);
    }

    return ret;
}

