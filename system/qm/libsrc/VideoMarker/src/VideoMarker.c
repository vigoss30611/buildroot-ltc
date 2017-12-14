
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <qsdk/videobox.h>
#include <qsdk/marker.h>

#include "QMAPIType.h"
#include "QMAPIErrno.h"
#include "VideoMarker.h"
#include "OSD.h"
#include "video.h"

enum {
    OSD_MARKER_CHN_TIME = 0,
    OSD_MARKER_CHN_1,
    OSD_MARKER_CHN_2,
    OSD_MARKER_CHN_MAX,
} OSD_CHANNEL_E;

enum {
    SHELTER_MARKER_CHN_1 = OSD_MARKER_CHN_MAX,
    SHELTER_MARKER_CHN_2,
    SHELTER_MARKER_CHN_3,
    SHELTER_MARKER_CHN_4,
    SHELTER_MARKER_CHN_MAX,
} SHELTER_CHANNEL_E;

int QMAPI_Video_MarkerInit(void)
{
    return QMAPI_OSD_Init();
}

void QMAPI_Video_MarkerUninit(void)
{
    QMAPI_OSD_Uninit();
}

void QMAPI_Video_MarkerStart(void)
{
    int nChannel;

    for (nChannel=0; nChannel<QMAPI_MAX_CHANNUM; nChannel++) {
        QMAPI_NET_CHANNEL_OSDINFO stChnOsd;
        QMapi_sys_ioctrl(nChannel, QMAPI_SYSCFG_GET_OSDCFG, 0, &stChnOsd, sizeof(QMAPI_NET_CHANNEL_OSDINFO));
        if (stChnOsd.bShowTime == QMAPI_TRUE) {
            QMAPI_OSD_SetFontSize(OSD_MARKER_CHN_TIME, OSD_MARKER_FONT_SIZE);
            QMAPI_OSD_Start(OSD_MARKER_CHN_TIME);
            QMAPI_OSD_SetTimeFormat(OSD_MARKER_CHN_TIME, stChnOsd.byReserve1);
        }

        if (stChnOsd.byShowChanName && QMAPI_OSD_GetConfigured(OSD_MARKER_CHN_1)==TRUE) {
            QMAPI_OSD_SetFontSize(OSD_MARKER_CHN_1, OSD_MARKER_FONT_SIZE);
            QMAPI_OSD_SetColor(OSD_MARKER_CHN_1, BACK_SIDE, OSD_MARKER_BACK_COLOR);
            QMAPI_OSD_SetFontType(OSD_MARKER_CHN_1, OSD_MARKER_FONT_TTF_NAME);
            QMAPI_OSD_SetUpdateMode(OSD_MARKER_CHN_1, MANUAL_MODE);
            QMAPI_OSD_SetString(OSD_MARKER_CHN_1, stChnOsd.csChannelName);
            QMAPI_OSD_Start(OSD_MARKER_CHN_1);
            QMAPI_OSD_SetLocation(OSD_MARKER_CHN_1, stChnOsd.dwShowNameTopLeftX, stChnOsd.dwShowNameTopLeftY);
        }
    }
}

void QMAPI_Video_MarkerStop(void)
{
    QMAPI_OSD_Stop(OSD_MARKER_CHN_TIME);
}

int QMAPI_Video_Marker_SetCfg(QMAPI_NET_CHANNEL_OSDINFO *pstChnOsdInfo)
{
    printf("#### %s %d\n", __FUNCTION__, __LINE__);
    int ret;

    if(NULL == pstChnOsdInfo)
    {
        return QMAPI_FAILURE;
    }

    if(QMAPI_FALSE == pstChnOsdInfo->bShowTime)
    {
        QMAPI_OSD_Stop(OSD_MARKER_CHN_TIME);
    }
    if(QMAPI_TRUE == pstChnOsdInfo->bShowTime)
    {
        QMAPI_OSD_Start(OSD_MARKER_CHN_TIME);
    }

    QMAPI_OSD_SetTimeFormat(OSD_MARKER_CHN_TIME, pstChnOsdInfo->byReserve1);

    if (QMAPI_OSD_GetConfigured(OSD_MARKER_CHN_1)==TRUE) {
        if(pstChnOsdInfo->byShowChanName)
        {
            QMAPI_OSD_SetFontSize(OSD_MARKER_CHN_1, OSD_MARKER_FONT_SIZE);
            QMAPI_OSD_SetColor(OSD_MARKER_CHN_1, BACK_SIDE, OSD_MARKER_BACK_COLOR);
            QMAPI_OSD_SetFontType(OSD_MARKER_CHN_1, OSD_MARKER_FONT_TTF_NAME);
            QMAPI_OSD_SetUpdateMode(OSD_MARKER_CHN_1, MANUAL_MODE);
            QMAPI_OSD_SetString(OSD_MARKER_CHN_1, pstChnOsdInfo->csChannelName);
            QMAPI_OSD_Start(OSD_MARKER_CHN_1);
            QMAPI_OSD_SetLocation(OSD_MARKER_CHN_1, pstChnOsdInfo->dwShowNameTopLeftX, pstChnOsdInfo->dwShowNameTopLeftY);
        }
        else
        {
            QMAPI_OSD_Stop(OSD_MARKER_CHN_1);
        }
    }

    return 0;
}

int QMAPI_Video_Marker_SetShelter(QMAPI_NET_CHANNEL_SHELTER *shelter)
{
    printf("%s \n", __FUNCTION__); 
    int ret = QMAPI_SUCCESS;
    if(NULL == shelter || shelter->dwSize != sizeof(QMAPI_NET_CHANNEL_SHELTER))
        return QMAPI_FAILURE;

    //TODO: set each configured marker channel a usage
    //if (QMAPI_OSD_GetChannelNum() > SHELTER_MARKER_CHN_1) {
    {
        int i = 0;
        for (i = 0; i < SHELTER_MARKER_CHN_MAX - SHELTER_MARKER_CHN_1; i++ ) {
            int shelterMarkerIndex = SHELTER_MARKER_CHN_1 + i;
            int enable   = shelter->bEnable;
            int x = shelter->strcShelter[i].wLeft;
            int y = shelter->strcShelter[i].wTop;
            int w = shelter->strcShelter[i].wWidth;
            int h = shelter->strcShelter[i].wHeight;

            printf("channel:%d enable:%d %d:%d %d*%d \n", shelterMarkerIndex, enable, 
                    x, y, w, h); 

            ven_info_t * info = infotm_video_info_get(0);
            if (info == NULL) {
                w = 0;
                h = 0;
                enable = 0;
            } else {
                if (x >= info->width) {
                    x = 0;
                }

                if (y >= info->height) {
                    y = 0;
                }

                if ((x+w) >= info->width) {
                    w = info->width - x;
                }

                if ((y+h) >= info->height) {
                    h = info->height - y;
                }
            }

            if (w == 0 || h == 0) {
                enable = 0;
            }

            printf("channel:%d adjust enable:%d %d:%d %d*%d \n", shelterMarkerIndex, enable, 
                    x, y, w, h); 


            if (enable) {
                QMAPI_Shelter_Start(shelterMarkerIndex);
                QMAPI_OSD_SetFontSize(shelterMarkerIndex,  SHELTER_MARKER_FONT_SIZE);
                QMAPI_Shelter_SetColor(shelterMarkerIndex, BACK_SIDE, SHELTER_MARKER_BACK_COLOR);
                QMAPI_Shelter_SetArea(shelterMarkerIndex,  x, y, w, h);
            } else {
                QMAPI_Shelter_Stop(shelterMarkerIndex);
            }
        }
    }

    printf("%s done\n", __FUNCTION__); 

    return ret;
}


#if 1
/*********************************************************************
Function:   QMAPI_Video_Marker_IOCtrl
Description:
Calls:
Called By:
parameter:
[in] Handle
[in] nCmd
[in/out] lpInParam 
[in/out] nSize
Return: QMAPI_SUCCESS/QMAPI_FAILURE/QMAPI_ERR_UNKNOW_COMMNAD
 ********************************************************************/
int QMAPI_Video_Marker_IOCtrl(int Handle, int nCmd, int nChannel, void *lpInParam, int nInSize)
{
    int ret = QMAPI_FAILURE;

    if(QMAPI_SYSCFG_SET_OSDCFG != nCmd 
       && QMAPI_SYSCFG_GET_OSDCFG != nCmd
        && QMAPI_SYSCFG_SET_SHELTERCFG != nCmd
            && QMAPI_SYSCFG_GET_SHELTERCFG != nCmd)
    {
        printf("<fun:%s>-<line:%d>, unspport cmd!\n", __FUNCTION__, __LINE__);
        return QMAPI_ERR_UNKNOW_COMMNAD;
    }

    switch(nCmd)
    {
		case QMAPI_SYSCFG_SET_OSDCFG:
		{
			ret = QMAPI_Video_Marker_SetCfg(lpInParam);
            break;
		}

		case QMAPI_SYSCFG_GET_OSDCFG:
		{
			break;
		}

        case QMAPI_SYSCFG_SET_SHELTERCFG:
        {
			ret = QMAPI_Video_Marker_SetShelter(lpInParam);
            break;
        }
        case QMAPI_SYSCFG_GET_SHELTERCFG:
        {
            break;
        }

        default:
        break;
	}

    return ret;
}
#endif


