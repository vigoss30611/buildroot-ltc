#ifndef __SPV_ITEM_H__
#define __SPV_ITEM_H__

#include "spv_config_keys.h"
#define GETVALUE(id) language_en[id - 1]

#define MAX_VALUE_COUNT 12

#define ITEM_TYPE_NORMAL 0x00000001L
#define ITEM_TYPE_HEADER 0x000000002L
#define ITEM_TYPE_MORE 0x00000003L
#define ITEM_TYPE_SWITCH 0x00000004L
#define ITEM_TYPE_SETUP_PARAM 0x00000005L //for q360 display setup param on third window

#define ITEM_TYPE_MASK 0x00000007L

#define ITEM_STATUS_SECONDARY 0x00000010L
#define ITEM_STATUS_DISABLE   0x00000020L
#define ITEM_STATUS_INVISIBLE   0x00000040L
#define ITEM_STATUS_OFF        0x00000080L

typedef struct _DIALOGITEM{
    int itemId;
    int style;
    int caption;
    int currentValue;
    int valuelist[MAX_VALUE_COUNT];
    int vc;
} DIALOGITEM;

typedef struct {
    int type;
    DIALOGITEM *pItems;
    int itemCount;
} DIALOGPARAM;

extern const int g_video_1080_fps_value[MAX_VALUE_COUNT];
extern const int g_video_720_fps_value[MAX_VALUE_COUNT];
extern const int g_video_360_fps_value[MAX_VALUE_COUNT];

extern DIALOGITEM g_video_items[];
extern DIALOGITEM g_photo_items[];
extern DIALOGITEM g_setup_items[];
extern DIALOGITEM g_display_items[];
extern DIALOGITEM g_date_time_items[];
//const extern DIALOGITEM *g_all_item[];

void UpdateSetupDialogItemValue(const char *key, const char *value);

int GetSettingItemsCount(int type);

#endif //__SPV_ITEM_H__
