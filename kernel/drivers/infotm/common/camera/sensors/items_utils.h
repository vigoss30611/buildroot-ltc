/*
 * items_utils.h
 *
 *  Created on: Apr 20, 2017
 *      Author: zhaomh
 */

#ifndef ITEMS_UTILS_H_
#define ITEMS_UTILS_H_

#include <linux/fs.h>
#include <mach/items.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>


#define parse_db(x...)

#define DEFAULT_PATH ("/tmp/video_path.items")

#define SETUP_PATH   ("/root/.ispddk/")
#define PARSE_LIMIT 256
#define SENSOR_ITEM_MAX_COUNT 40

#define MAX_SENSORS_NUM  (2)
#define MAX_BRIDGE_SENSORS_NUM (2)
#define PATH_ITEM_MAX_LEN  128

static struct _sensor_sttributes {
     char sensor_item_path[ITEM_MAX_LEN];
     char sensor_config_path[ITEM_MAX_LEN];
     char isp_config_path[ITEM_MAX_LEN];
     char lsh_config_path[ITEM_MAX_LEN];
}sensor_attr_info[MAX_SENSORS_NUM];

char *ddk_get_sensor_path(int index);
char *ddk_get_isp_path(int index);
int ddk_parse_ext_item(void);

#endif

