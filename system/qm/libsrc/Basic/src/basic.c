

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <cJSON.h>
#include <qsdk/items.h>
#if 0
#include "basic.h"

#define CTRL_MAGIC	('O')
#define SEQ_NO (1)
#define GETI2CADDR	_IOW(CTRL_MAGIC, SEQ_NO, unsigned long)
#define GETI2CSENSORNAME	_IOW(CTRL_MAGIC, SEQ_NO + 1, char*)

#define ITEM_SENSOR "camera.sensor.model"


int video_stream_resolution_get(char *item_resolution, char *channel_name, int *resolution)
{
    char *buf = NULL;
    char json_name[56];
    int w=0, h=0, fd=0, ret;
    cJSON *c=NULL, *tmp, *tmpp;
    
    do {
        if (!resolution)
        {
            break;
        }
        *resolution = 0;
        
        buf = malloc(20*1024);
        if (!buf) 
        {
            break;
        }

        sprintf(json_name, "/root/.videobox/%s.json",item_resolution);
        fd = open(json_name, O_RDONLY);
        if (fd <= 0)
        {
            break;
        }

        ret = read(fd, buf, 20*1024);
        if (ret <= 0)
        {
            break;
        }

        c = cJSON_Parse((const char *)buf);
        if (!c)
        {
            break;
        }

        tmp = cJSON_GetObjectItem(c, (const char *)"ispost");
        if (!tmp)
        {
            break;
        }

        tmp = cJSON_GetObjectItem(tmp, (const char *)"port");
        if (!tmp)
        {
            break;
        }

        tmp = cJSON_GetObjectItem(tmp, channel_name);
        if (!tmp)
        {
            break;
        }

        tmpp = cJSON_GetObjectItem(tmp, (const char *)"w");
        if (tmpp)
        {
            w = tmpp->valueint;
        }

        tmpp = cJSON_GetObjectItem(tmp, (const char *)"h");
        if (tmpp)
        {
            h = tmpp->valueint;
        }
    }while(0);
    
    printf("%s, %s. channel:%s %d-%d.\n", __FUNCTION__, json_name, channel_name, w, h);
    if (w && h)
    {
        if (w == 1536 && h == 1536)
        {
            *resolution = DEVICE_MEGA_1080P;
        } 
        else if (w == 1920 && h == 1080)
        {
            *resolution = DEVICE_MEGA_1080P;
        }
        else if (w == 1280 && h == 960)
        {
            *resolution = DEVICE_MEGA_960P;
        }
        else if (w == 1280 && h == 720)
        {
            *resolution = DEVICE_MEGA_IPCAM;
        }
        else if (w == 720 && h == 576)
        {
            *resolution = DEVICE_MEGA_D1;
        }
        else if (w == 640 && h == 640)
        {
            *resolution = DEVICE_MEGA_VGA;
        }
        else if (w == 640 && h == 480)
        {
            *resolution = DEVICE_MEGA_VGA;
        }
        else if (w == 640 && h == 360)
        {
            *resolution = DEVICE_MEGA_640x360;
        }
        else if (w == 360 && h == 360)
        {
            *resolution = DEVICE_MEGA_VGA;
        }
        else if (w == 352 && h == 288)
        {
            *resolution = DEVICE_MEGA_CIF;
        } else {
            *resolution = DEVICE_MEGA_1080P;
        } 
    }
    else
    {
        *resolution = DEVICE_MEGA_640x360;
    }
    
    if (c)
    {
        cJSON_Delete(c);
    }
    if (fd > 0)
    {
        close(fd);
    }
    if (buf)
    {
        free(buf);
    }
    return 0;
}

int check_man_resolution(char *item_resolution, int *resolution)
{
    return video_stream_resolution_get(item_resolution, "dn", resolution);
} 

int check_sub_resolution(char *item_resolution, int *resolution)
{
    return video_stream_resolution_get(item_resolution, "ss0", resolution);
}

int check_sensor_resolution(int *sensor, int *manresolution, int *subresolution)
{
	char item_sensor[64] = {0};
	char item_name[64] = {0};
	char item_resolution[8] = {0};

	if (!sensor || !manresolution || !subresolution)
		return -1;
	
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

    {
        *sensor = TL_Q3_SC2135;
    } 

    sprintf(item_name, "sensor.%s.resolution", item_sensor);
    if (!item_exist(item_name)) {
        printf("item %s NOT found!\n", item_name);
        return -1;
    }

    item_string(item_resolution, item_name, 0);

	printf("%s sensor name:%s,max resolution:%s\n",__FUNCTION__, item_sensor, item_resolution);
	
    check_man_resolution(item_resolution, manresolution);
    check_sub_resolution(item_resolution, subresolution);

	return 0;
}
#endif
