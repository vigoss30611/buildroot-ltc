#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <qsdk/sys.h>
#include "libQMHAL.h"

QMHAL_IRCUT_STATE_TYPE_E g_IRCut_state = QMHAL_IRCUT_STATE_UNKNOWN;

int QMHAL_GPIO_Init()
{
    return 0;
}

int QMHAL_IRCUT_SetState(QMHAL_IRCUT_STATE_TYPE_E state)
{
    int ret = -1;

    printf("%s  %d, state = %d\n", __FUNCTION__, __LINE__, state);
    return ret;
}

QMHAL_IRCUT_STATE_TYPE_E QMHAL_IRCUT_GetState()
{
    QMHAL_IRCUT_STATE_TYPE_E state = QMHAL_IRCUT_STATE_UNKNOWN;

    //printf("%s  %d\n", __FUNCTION__, __LINE__);

    return state;
}

int QMHAL_RLight_SetState(QMHAL_LIGHT_STATE_TYPE_E state)
{
    int ret = -1;

    printf("%s  %d, state = %d\n", __FUNCTION__, __LINE__, state);

    return ret;
}

QMHAL_LIGHT_STATE_TYPE_E QMHAL_RLight_GetState()
{
    QMHAL_LIGHT_STATE_TYPE_E state = QMHAL_LIGHT_STATE_UNKNOWN;

    return state;
}

QMHAL_LDR_LEVEL_TYPE_E QMHAL_LDR_GetState()
{
    QMHAL_LDR_LEVEL_TYPE_E state = QMHAL_LDR_LEVEL_UNKNOWN;

    return state;
}

QMHAL_GPIO_LEVEL_TYPE_E QMHAL_Reset_GetState()
{
    QMHAL_GPIO_LEVEL_TYPE_E state = QMHAL_GPIO_LEVEL_UNKNOWN;

    return state;
}

int QMHAL_AlarmLed_SetState(QMHAL_LIGHT_STATE_TYPE_E state)
{
    int ret = -1;

    printf("%s  %d, state = %d\n", __FUNCTION__, __LINE__, state);

    return ret;
}

int QMHAL_AlarmLed_SetBlink(int light, int dark)
{
    int ret = -1;

    printf("%s  %d, light=%d, dark=%d\n", __FUNCTION__, __LINE__, light, dark);

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

    return ret;
}

int QMHAL_SystemLed_SetBlink(int light, int dark)
{
    int ret = -1;

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

    return ret;
}

int QMHAL_WTD_SetTimeout(int timeout)
{
    return 0;
}

void QMHAL_WTD_Feed()
{
    return;
}

int QMHAL_SPEAKER_Enable(int enable)
{
    return 0;
}

