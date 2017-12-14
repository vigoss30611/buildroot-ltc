
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <cJSON.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <qsdk/items.h>

#include <qsdk/videobox.h>
#include <qsdk/marker.h>

#include "QMAPIType.h"
#include "QMAPIErrno.h"
#include "OSD.h"

typedef struct {
    pthread_mutex_t mutex;
    char portname[8];
    char channelname[16];
    BOOL configured; /*!< configured flag to device */
    struct font_attr fontAttr;
    int enaRefcnt;
    BOOL started;   /*!< FIXME: */
    int width;
    int height;
    int x;
    int y;
    int mode;   /*!< 0:auto, 1:manual */
    int format; /*!< Time string format for auto mode */
    char string[128];   /*!< String for manual mode */
} marker_t;

#define CFG_MARKER_FONT_COLOR           0xFFFFFFFF  /*!< 32bit 0/R/G/B */
#define CFG_MARKER_BACK_COLOR           0x80CCCCCC  /*!< 32bit A/R/G/B */

#define CFG_MARKER_CHANNEL_NUM_MAX 8

static char TimeStringFormat[6][32] = {
    "%t%Y-%M-%D  %H:%m:%S",
    "%t%M-%D-%Y  %H:%m:%S",
    "%t%Y/%M/%D  %H:%m:%S",
    "%t%M/%D/%Y  %H:%m:%S",
    "%t%D-%M-%Y  %H:%m:%S",
    "%t%D/%M/%Y  %H:%m:%S"
};

static marker_t *sMarker = NULL;
static int sConfiguredChannelNum = 0;

int QMAPI_OSD_Configure(const char *json_name)
{
    char *buf = NULL;
    int fd=0, ret;
    cJSON *c=NULL, *tmp, *tmpp;
    int i;
    marker_t *mkr;
    int configured_osd_num = 0;

    do {
        buf = malloc(20*1024);
        if (!buf) {
            break;
        }

        fd = open(json_name, O_RDONLY);
        if (fd <= 0) {
            break;
        }

        ret = read(fd, buf, 20*1024);
        if (ret<=0) {
            break;
        }

        c = cJSON_Parse((const char *)buf);
        if (!c) {
            break;
        }

        tmp = cJSON_GetObjectItem(c, (const char *)"ispost");
        if (!tmp) {
            // for ISPOSTv1, support one 'ol' only
            printf("#%s %d\n",__FUNCTION__,__LINE__);
            tmp = cJSON_GetObjectItem(c, (const char *)"ispost0");
            if (!tmp) {
                break;
            }

            tmp = cJSON_GetObjectItem(tmp, (const char *)"port");
            if (!tmp) {
                break;
            }

            tmpp = cJSON_GetObjectItem(tmp, (const char *)"ol");
            if (!tmpp) {
                break;
            }

            mkr = sMarker;

            pthread_mutex_lock(&mkr->mutex);
            configured_osd_num++;
            mkr->configured = TRUE;

            sprintf((char *)mkr->portname, "ol");
            sprintf((char *)mkr->channelname, "marker0"); //FIXME: should check which ipu bind to the port and get its name

            sprintf((char *)mkr->fontAttr.ttf_name, OSD_MARKER_FONT_TTF_NAME);
            mkr->fontAttr.font_size = 0;
            mkr->fontAttr.font_color = CFG_MARKER_FONT_COLOR;
            mkr->fontAttr.back_color = CFG_MARKER_BACK_COLOR;
            mkr->fontAttr.mode = LEFT;
            mkr->started = FALSE;
            mkr->mode = AUTO_MODE;
            pthread_mutex_unlock(&mkr->mutex);

            printf("#%s: port %s, channel %s configured.\n", __FUNCTION__, mkr->portname, mkr->channelname);
            break;
        }

        printf("#%s %d\n",__FUNCTION__,__LINE__);
        // for ISPOSTv2, support many 'ov'
        tmp = cJSON_GetObjectItem(tmp, (const char *)"port");
        if (!tmp) {
            break;
        }

        for (i=0,mkr=sMarker; i<CFG_MARKER_CHANNEL_NUM_MAX; i++,mkr++) {
            char portname[8];
            sprintf(portname, "ov%d", i);

            tmpp = cJSON_GetObjectItem(tmp, portname);
            if (!tmpp) {
                continue;
            }

            pthread_mutex_lock(&mkr->mutex);
            configured_osd_num++;
            mkr->configured = TRUE;

            sprintf((char *)mkr->portname, "%s", portname);
            sprintf((char *)mkr->channelname, "marker%d", i); //FIXME: should check which ipu bind to the port and get its name

            sprintf((char *)mkr->fontAttr.ttf_name, OSD_MARKER_FONT_TTF_NAME);
            mkr->fontAttr.font_size = 0;
            mkr->fontAttr.font_color = CFG_MARKER_FONT_COLOR;
            mkr->fontAttr.back_color = CFG_MARKER_BACK_COLOR;
            mkr->fontAttr.mode = LEFT;
            mkr->started = FALSE;
            mkr->mode = AUTO_MODE;
            pthread_mutex_unlock(&mkr->mutex);

            printf("#%s: port %s, channel %s configured.\n", __FUNCTION__, mkr->portname, mkr->channelname);
        }

    }while(0);

    if (c) {
        cJSON_Delete(c);
    }
    if (fd>0) {
        close(fd);
    }
    if (buf) {
        free(buf);
    }

    return configured_osd_num;
}

int QMAPI_OSD_GetChannelNum()
{
    return sConfiguredChannelNum;
}

#define CTRL_MAGIC	('O')
#define SEQ_NO (1)
#define GETI2CSENSORNAME    _IOW(CTRL_MAGIC, SEQ_NO + 1, char*)
#define ITEM_SENSOR "camera.sensor.model"
static int get_videobox_json_name(char *json_name)
{
    int ret = -1;
    char item_sensor[64] = {0};
    char item_name[64] = {0};
    char item_resolution[64] = {0};

    if (!json_name)
        return -1;
#if 0
	if((item_exist("camera.sensor.autodetect")) && item_integer("camera.sensor.autodetect", 0)){
        int fd = -1;
        fd = open("/dev/ddk_sensor", O_RDWR);
        if(fd <0)
        {
            printf("open /dev/ddk_sensor error\n");
            close(fd);
			return -1;
        }

        ioctl(fd, GETI2CSENSORNAME, item_sensor);
        close(fd);

    }else{
        if (!item_exist(ITEM_SENSOR)) {
            printf("item %s NOT found!\n", ITEM_SENSOR);
            return -1;
        }
        item_string(item_sensor, ITEM_SENSOR, 0);
    }

	sprintf(item_name, "sensor.%s.resolution", item_sensor);
	if (!item_exist(item_name)) {
        printf("item %s NOT found!\n", item_name);
        return -1;
    }

	item_string(item_resolution, item_name, 0);
#endif

    sprintf(item_resolution, "%s", "path"); 
    sprintf(json_name, "/root/.videobox/%s.json", item_resolution);
	printf("%s  %s\n",__FUNCTION__, json_name);
    return 0;
}

int QMAPI_OSD_Init(void)
{
    int i;
    int ret;

    printf("## %s %d\n",__FUNCTION__,__LINE__);

    if (sMarker==NULL) {
        marker_t *mkr;
        int channel_num = CFG_MARKER_CHANNEL_NUM_MAX;
        char item_sensor[64] = {0};
        char item_name[64] = {0};
        char item_resolution[64] = {0};
        char json_name[64] = {0};

        if (channel_num>0) {
            sMarker = (marker_t *)malloc(sizeof(marker_t)*channel_num);

            if (sMarker==NULL) {
                printf("Failed to alloc memory for %s\n", __FUNCTION__);
                return -1;
            }

            memset((void *)sMarker, 0, sizeof(marker_t)*channel_num);
            for (i=0,mkr=sMarker; i<channel_num; i++,mkr++) {
                pthread_mutex_init(&mkr->mutex, NULL);
                mkr->configured = FALSE;
            }

            ret = get_videobox_json_name(json_name);
            if (ret==0) {
                sConfiguredChannelNum = QMAPI_OSD_Configure(json_name);
            }
        }
    }

    return 0;
}

void QMAPI_OSD_Uninit(void)
{
    int i;

    printf("## %s %d\n",__FUNCTION__,__LINE__);

    if (sMarker) {
        int channel_num = CFG_MARKER_CHANNEL_NUM_MAX;
        for (i=0; i<channel_num; i++) {
            QMAPI_OSD_Stop(i);
        }

        free(sMarker);
    }
}

void QMAPI_OSD_Start(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            if (!mkr->started) {
                if (mkr->mode == AUTO_MODE) {
                    marker_set_mode(mkr->channelname, "auto", TimeStringFormat[mkr->format], &mkr->fontAttr);
                } else {
                    marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
                }
                marker_enable(mkr->channelname);
                mkr->started = TRUE;
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

void QMAPI_OSD_Stop(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            if (mkr->started == TRUE) {
                marker_disable(mkr->channelname);
                mkr->started = FALSE;
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

void QMAPI_OSD_Reset(const char *json_name)
{
    int i;
    marker_t *mkr;

    printf("## %s %d\n",__FUNCTION__,__LINE__);

    for (i=0,mkr=sMarker; i<CFG_MARKER_CHANNEL_NUM_MAX; i++,mkr++) {
        QMAPI_OSD_Stop(i);

        pthread_mutex_lock(&mkr->mutex);
        mkr->configured = FALSE;
        pthread_mutex_unlock(&mkr->mutex);
    }

    sConfiguredChannelNum = QMAPI_OSD_Configure(json_name);
}

/* 0:stoped, 1:started -1:error */
int QMAPI_OSD_GetState(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        return mkr->started;
    }

    return -1;
}

BOOL QMAPI_OSD_GetConfigured(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        return mkr->configured;
    }

    return FALSE;
}

void QMAPI_OSD_SetTimeFormat(int channel, int format)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        if (mkr->configured==TRUE) {

            pthread_mutex_lock(&mkr->mutex);
            if (format != mkr->format) {
                mkr->mode = AUTO_MODE;
                mkr->format = format;
                if (mkr->started) {
                    marker_set_mode(mkr->channelname, "auto", TimeStringFormat[mkr->format], &mkr->fontAttr);
                }
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

int QMAPI_OSD_GetTimeFormat(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        return mkr->format;
    }

    return -1;
}

void QMAPI_OSD_SetUpdateMode(int channel, int mode)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        if (mkr->configured==TRUE) {

            pthread_mutex_lock(&mkr->mutex);
            mkr->mode = mode;
            if (mkr->mode == AUTO_MODE) {
                marker_set_mode(mkr->channelname, "auto", TimeStringFormat[mkr->format], &mkr->fontAttr);
            } else {
                marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

int QMAPI_OSD_GetUpdateMode(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        return mkr->mode;
    }

    return -1;
}

void QMAPI_OSD_SetString(int channel, const char *str)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        if (mkr->configured==TRUE) {
        
        pthread_mutex_lock(&mkr->mutex);
        mkr->mode = MANUAL_MODE;
        memcpy(mkr->string, str, strlen(str)+1);
        //if (mkr->started) {
            marker_set_mode(mkr->channelname, "manual", TimeStringFormat[mkr->format], &mkr->fontAttr);
            marker_set_string(mkr->channelname, mkr->string);
        //}
        pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

int QMAPI_OSD_GetString(int channel, char *str)
{
    int ret = -1;

    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker && str) {
        marker_t *mkr = sMarker;
        mkr += channel;

        memcpy(str, mkr->string, strlen(mkr->string));
        ret = 0;
    }

    return ret;
}

void QMAPI_OSD_SetLocation(int channel, int x, int y)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        if (mkr->configured==TRUE) {

            pthread_mutex_lock(&mkr->mutex);
            mkr->x = x;
            mkr->y = y;
            if (mkr->started) {
                struct v_pip_info pip_info;
                int ret;

                strncpy(pip_info.portname, mkr->portname, strlen(mkr->portname));
                pip_info.x = mkr->x;
                pip_info.y = mkr->y;
                //pip_info.w = 800;
                //pip_info.h = 64;

                ret = video_set_pip_info("ispost", &pip_info);
                printf("#### %s %d, ret=%d\n", __FUNCTION__, __LINE__, ret);
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

int QMAPI_OSD_GetLocation(int channel, int *x, int *y)
{
    int ret = -1;
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker && x && y) {
        marker_t *mkr = sMarker;
        mkr += channel;

        *x = mkr->x;
        *y = mkr->y;
        ret = 0;
    }
    return ret;
}

void QMAPI_OSD_SetSize(int channel, int width, int height)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        if (mkr->configured==TRUE) {
            char portname[16];

            sprintf(portname, "ispost-%s", mkr->portname);
            pthread_mutex_lock(&mkr->mutex);
            mkr->width = width;
            mkr->height = height;
            video_set_resolution(portname, mkr->width, mkr->height);
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

int QMAPI_OSD_GetSize(int channel, int *width, int *height)
{
    int ret = -1;
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker && width && height) {
        marker_t *mkr = sMarker;
        mkr += channel;

        width = mkr->width;
        height = mkr->height;
        ret = 0;
    }
    return ret;
}

void QMAPI_OSD_SetFontSize(int channel, char font_size)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        
        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            if (mkr->fontAttr.font_size != font_size) {
                mkr->fontAttr.font_size = font_size;
                if (mkr->started) {
                    if (mkr->mode == AUTO_MODE) {
                        marker_set_mode(mkr->channelname, "auto", TimeStringFormat[mkr->format], &mkr->fontAttr);
                    } else {
                        marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
                    }
                }
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

void QMAPI_OSD_SetFontType(int channel, const char *ttf_name)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            sprintf((char *)mkr->fontAttr.ttf_name, ttf_name);
            if (mkr->started) {
                if (mkr->mode == AUTO_MODE) {
                    marker_set_mode(mkr->channelname, "auto", TimeStringFormat[mkr->format], &mkr->fontAttr);
                } else {
                    marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
                }
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

void QMAPI_OSD_SetDisplayMode(int channel, enum display_mode mode)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            if (mkr->fontAttr.mode != mode) {
                mkr->fontAttr.mode = mode;
                if (mkr->started) {
                    if (mkr->mode == AUTO_MODE) {
                        marker_set_mode(mkr->channelname, "auto", TimeStringFormat[mkr->format], &mkr->fontAttr);
                    } else {
                        marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
                    }
                }
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

void QMAPI_OSD_SetColor(int channel, int side, int color)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            if (side == FRONT_SIDE)
                mkr->fontAttr.font_color = color;
            if (side == BACK_SIDE)
                mkr->fontAttr.back_color = color;

            if (mkr->started) {
                if (mkr->mode == AUTO_MODE) {
                    marker_set_mode(mkr->channelname, "auto", TimeStringFormat[mkr->format], &mkr->fontAttr);
                } else {
                    marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
                }
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

int QMAPI_OSD_GetFontAttr(int channel, struct font_attr *fontAttr)
{
    int ret = -1;
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX && sMarker && fontAttr) {
        marker_t *mkr = sMarker;
        mkr += channel;

        memcpy(fontAttr, &mkr->fontAttr, sizeof(struct font_attr));
        ret = 0;
    }
    return ret;
}

void QMAPI_Shelter_Start(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);
    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX  && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;
        
        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            mkr->mode = 1;
            printf("%s channel:%d channelName:%s \n", __FUNCTION__, channel, mkr->channelname);
            if (!mkr->started) {
                printf("marker_enable channel:%d channelName:%s \n", channel, mkr->channelname);
                marker_enable(mkr->channelname);

                printf("marker_set_mode channel:%d channelName:%s \n", channel, mkr->channelname);
                marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
                mkr->started = TRUE;
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

void QMAPI_Shelter_Stop(int channel)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX  && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            if (mkr->started == TRUE) {
                marker_disable(mkr->channelname);
                mkr->started = FALSE;
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

/* 0:stoped, 1:started -1:error */

int QMAPI_Shelter_GetState(int channel) 
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    return QMAPI_OSD_GetState(channel);
}

int QMAPI_Shelter_SetArea(int channel, int x, int y, int w, int h) 
{

    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX  && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            mkr->x = x;
            mkr->y = y;
            mkr->width = w;
            mkr->height = h;
            if (mkr->started) {
                struct v_pip_info pip_info;
                char portname[16];
                int ret;

                strncpy(pip_info.portname, mkr->portname, strlen(mkr->portname));
                pip_info.x = mkr->x;
                pip_info.y = mkr->y;
                //pip_info.w = 800;
                //pip_info.h = 64;
                ret = video_set_pip_info("ispost", &pip_info);

                sprintf(portname, "ispost-%s", mkr->portname);
                ret = video_set_resolution(portname, mkr->width, mkr->height);
                memcpy(mkr->string, " ", strlen(" ")+1);
                marker_set_string(mkr->channelname, mkr->string);
            }

            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

void QMAPI_Shelter_SetColor(int channel, int side, int color)
{
    printf("## %s %d: %d\n",__FUNCTION__,__LINE__,channel);

    if (channel>=0 && channel<CFG_MARKER_CHANNEL_NUM_MAX  && sMarker) {
        marker_t *mkr = sMarker;
        mkr += channel;

        if (mkr->configured==TRUE) {
            pthread_mutex_lock(&mkr->mutex);
            if (side == FRONT_SIDE)
                mkr->fontAttr.font_color = color;
            if (side == BACK_SIDE)
                mkr->fontAttr.back_color = color;

            if (mkr->started) {
                marker_set_mode(mkr->channelname, "manual", "%t%Y-%M-%D  %H:%m:%S", &mkr->fontAttr);
            }
            pthread_mutex_unlock(&mkr->mutex);
        }
    }
}

