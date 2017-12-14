
#ifndef __OSD_H__
#define __OSD_H__

#include <qsdk/marker.h>

#define OSD_MARKER_BACK_COLOR    0x00FFFFFF  /*!< 32bit A/R/G/B */
#define OSD_MARKER_FONT_SIZE     10
#define OSD_MARKER_FONT_TTF_NAME "songti"

#define SHELTER_MARKER_BACK_COLOR    0xFF000000  /*!< 32bit A/R/G/B */
#define SHELTER_MARKER_FONT_SIZE     10
#define SHELTER_MARKER_FONT_TTF_NAME "songti"


#define FRONT_SIDE 0
#define BACK_SIDE 1

#define AUTO_MODE 0
#define MANUAL_MODE 1

int QMAPI_OSD_Configure(const char *json_name);
int QMAPI_OSD_GetChannelNum();
int QMAPI_OSD_Init(void);
void QMAPI_OSD_Uninit(void);
void QMAPI_OSD_Start(int channel);
void QMAPI_OSD_Stop(int channel);
void QMAPI_OSD_Reset(const char *json_name);
int QMAPI_OSD_GetState(int channel);
BOOL QMAPI_OSD_GetConfigured(int channel);
void QMAPI_OSD_SetTimeFormat(int channel, int format);
int QMAPI_OSD_GetTimeFormat(int channel);
void QMAPI_OSD_SetString(int channel, const char *str);
int QMAPI_OSD_GetString(int channel, char *str);
void QMAPI_OSD_SetLocation(int channel, int x, int y);
int QMAPI_OSD_GetLocation(int channel, int *x, int *y);
void QMAPI_OSD_SetSize(int channel, int width, int height);
int QMAPI_OSD_GetSize(int channel, int *width, int *height);
void QMAPI_OSD_SetFontSize(int channel, char font_size);
void QMAPI_OSD_SetFontType(int channel, const char *ttf_name);
void QMAPI_OSD_SetDisplayMode(int channel, enum display_mode mode);
void QMAPI_OSD_SetColor(int channel, int side, int color);
int QMAPI_OSD_GetFontAttr(int channel, struct font_attr *fontAttr);



void QMAPI_Shelter_Start(int channel);
void QMAPI_Shelter_Stop(int channel);
int  QMAPI_Shelter_GetState(int channel); 
int  QMAPI_Shelter_SetArea(int channel, int x, int y, int w, int h); 
void QMAPI_Shelter_SetColor(int channel, int side, int color);

#endif
