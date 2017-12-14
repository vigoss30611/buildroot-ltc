/*
 * =====================================================================================
 *
 *       Filename:  gui.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2016年08月04日 13时53分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "spv_common.h"
#include "spv_static.h"
#include "spv_setup_dialog.h"
#include "spv_utils.h"
#include "config_manager.h"
#include "spv_item.h"
#include "spv_language_res.h"
#include "spv_folder_dialog.h"
#include "file_dir.h"
#include "spv_debug.h"
#include "gps.h"
#include "spv_adas.h"
#include "gui_icon.h"
#include "gui_common.h"
#include "sys_server.h"
#include "qsdk/event.h"


#define FLAG_POWERED_BY_GSENSOR 8424
#define FLAG_POWERED_BY_OTHER 8425

#define TIMER_CLEANUP_SECONDARYDC 123
#define TIMER_PHOTO_EFFECT (TIMER_CLEANUP_SECONDARYDC + 1)
#define TIMER_BATTERY_FLICK (TIMER_PHOTO_EFFECT + 1)
#define TIMER_AUTO_SHUTDOWN (TIMER_BATTERY_FLICK + 1)
#define TIMER_LONGPRESS_DOWN (TIMER_AUTO_SHUTDOWN + 1)
#define TIMER_LONGPRESS_UP (TIMER_LONGPRESS_DOWN + 1)
#define TIMER_TIRED_ALARM (TIMER_LONGPRESS_UP + 1)

#define MIDDLE_FONT_SIZE 14
#define LARGE_FONT_SIZE 18
#define SMALL_FONT_SIZE 12


extern void sysserver_proc();

LANGUAGE_TYPE GetLanguageType()
{
    char value[VALUE_SIZE] = {0};
    LANGUAGE_TYPE type = LANG_EN;
    if(!GetConfigValue(GETKEY(ID_SETUP_LANGUAGE), value)) {
        if(!strcasecmp("EN", value)) {
            type = LANG_EN;
        } else if(!strcasecmp("ZH", value)) {
            type = LANG_ZH;
        } else if(!strcasecmp("TW", value)) {
            type = LANG_TW;
        } else if(!strcasecmp("FR", value)) {
            type = LANG_FR;
        } else if(!strcasecmp("ES", value)) {
            type = LANG_ES;
        } else if(!strcasecmp("IT", value)) {
            type = LANG_IT;
        } else if(!strcasecmp("PT", value)) {
            type = LANG_PT;
        } else if(!strcasecmp("DE", value)) {
            type = LANG_DE;
        } else if(!strcasecmp("RU", value)) {
            type = LANG_RU;
        } else if(!strcasecmp("KR", value)) {
            type = LANG_KR;
        } else if(!strcasecmp("JP", value)) {
            type = LANG_JP;
        }
    }

    return type;
}

int SetLanguageConfig(LANGUAGE_TYPE lang)
{
    char value[16] = {0};
    switch(lang) {
        case LANG_EN:
			strcpy(value, "EN");
            g_language = language_en;
            break;
        case LANG_ZH:
			strcpy(value, "ZH");
            g_language = language_zh;
            break;
        case LANG_TW:
			strcpy(value, "TW");
            g_language = language_tw;
            break;
        case LANG_FR:
			strcpy(value, "FR");
            g_language = language_fr;
            break;
        case LANG_ES:
			strcpy(value, "ES");
            g_language = language_es;
            break;
        case LANG_IT:
			strcpy(value, "IT");
            g_language = language_it;
            break;
        case LANG_PT:
			strcpy(value, "PT");
            g_language = language_pt;
            break;
        case LANG_DE:
			strcpy(value, "DE");
            g_language = language_de;
            break;
        case LANG_RU:
			strcpy(value, "RU");
            g_language = language_ru;
            break;
        case LANG_KR:
			strcpy(value, "KR");
            g_language = language_kr;
            break;
        case LANG_JP:
			strcpy(value, "JP");
            g_language = language_jp;
            break;
        default:
			strcpy(value, "ZH");
            g_language = language_zh;
            break;
    }
	return SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_SETUP_LANGUAGE), (long)value, TYPE_FROM_GUI);
}

static SPVSTATE* g_spv_camera_state = NULL;
void UpdateModeItemsContent(SPV_MODE currentMode);
void UpdateMainWindow(HWND hWnd);
void InvalidateItemRect(HWND hWnd, MWI_TYPE itemId, BOOL bErase);
void UpdateBatteryIcon();


typedef struct {
    MW_STYLE style;
    RECT rect;
    BOOL visibility;
    char text[VALUE_SIZE];
    int iconId;
} MWITEM;
typedef MWITEM* PMWITEM;
MWITEM g_items[MWI_MAX + 1];

BITMAP *g_icon;
extern PSleepStatus g_status;
static HWND g_main_window;
static BOOL g_recompute_locations = FALSE;
static BOOL g_force_cleanup_secondarydc = FALSE;
static PLOGFONT g_logfont_middle;
static PLOGFONT g_logfont_large;
static PLOGFONT g_logfont_small;

int g_photo_count = 0;
int g_photo_count_remain = 0;

int DEVICE_WIDTH = 128;
int DEVICE_HEIGHT = 64;
int LARGE_SCREEN = 0;

int SCALE_FACTOR = 1;
int VIEWPORT_WIDTH = 128;
int VIEWPORT_HEIGHT = 64;

// main window
int MW_BAR_HEIGHT = 16;
int MW_ICON_WIDTH = 24;
int MW_ICON_HEIGHT = 14;
int MW_SMALL_ICON_WIDTH;
int MW_ICON_PADDING;
int MW_ITEM_PADDING = 2;
int MW_INTERNAL_HEIGHT = 26;
int MW_BAR_PADDING = 3;

//setup window 
int SETUP_ITEM_HEIGHT;

//folder window
int FOLDER_DIALOG_TITLE_HEIGHT;
int FOLDER_WIDTH;
int FOLDER_HEIGHT;
int FOLDER_INFO_HEIGHT;

//playback window
int PB_BAR_HEIGHT;
int PB_PROGRESS_HEIGHT;

int PB_ICON_WIDTH;
int PB_ICON_HEIGHT;
int PB_ICON_PADDING;

int PB_FOCUS_WIDTH;
int PB_FOCUS_HEIGHT;


static void GetLapseString(char *lstring)
{
    if(lstring == NULL)
        return;

    int timeRemain;
    int timeLapse;
    int countLapse;
    char value[VALUE_SIZE] = {0};
    
    if(g_spv_camera_state->mode <= MODE_VIDEO) {
        //if(SpvGetSdcardStatus() == SD_STATUS_UMOUNT) {
        //    strcpy(lstring, "00:00:00");
        //    return;
        //}
        if(g_spv_camera_state->isRecording) {
            GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
            int loopTime = atoi(value) * 60;
            if(loopTime == 0) {//here if the loop recording is off, change to 5 minutes
                loopTime = 5 * 60;
            }
            timeLapse = g_spv_camera_state->timeLapse;
            if(loopTime != 0) {
                if(timeLapse != 0 && (timeLapse % loopTime == 0)) {
                    //g_spv_camera_state->videoCount += 1;
                    if(g_spv_camera_state->isLocked) {//loop record next video, kill the lock icon
                        INFO("reset isLocked\n");
                        g_items[MWV_LOCKED].visibility = FALSE;
                        InvalidateItemRect(g_main_window, MWV_LOCKED, TRUE);
                    }
                }
                //timeLapse = timeLapse % loopTime;
            }
            TimeToString(timeLapse, lstring);
        } else {
            GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), value);
            timeRemain = GetVideoTimeRemain(g_spv_camera_state->availableSpace, value);
			memset(value, 0, VALUE_SIZE);
            TimeToString(timeRemain, value);
			if(strlen(value) > 9) //9 refer: "000:00:00"
				strncpy(lstring, value + strlen(value) - 9, 9);
			else
				strcpy(lstring, value);
        }
    } else if(g_spv_camera_state->mode == MODE_PHOTO) {
        //if(SpvGetSdcardStatus() == SD_STATUS_UMOUNT) {
        //    strcpy(lstring, "000000");
        //    return;
        //}
        sprintf(lstring, "%06d", g_photo_count_remain - g_spv_camera_state->photoCount + g_photo_count);
    }
	
}

int ResetConfig(char *filepath, char *resetkey)
{
	FILE *fp = NULL;
	char linebuf[LINE_BUF_SIZE] = { 0 };
    char keybuff[VALUE_SIZE] = { 0 };
    char valuebuff[VALUE_SIZE] = { 0 };
	char tempbuff[VALUE_SIZE] = { 0 };
	char *pComment = NULL;
	char *pResetKey = NULL;
	char *pNextLine = NULL;
	char *pEqual = NULL;

	if((filepath == NULL) || (resetkey == NULL)) {
		ERROR("filepath or resetkey is NULL\n");
		return -1;
	}

	fp = fopen(filepath, "r");
	if(!fp) {
		ERROR("open %s error\n", filepath);
		return -1;
	}

	while(!feof(fp)) {
		pResetKey = NULL;

        if(fgets(linebuf, LINE_BUF_SIZE, fp) == NULL)
            break;

		if((pResetKey = strstr(linebuf, resetkey)) == NULL)
			continue;

		//test if reset key is at the beginning
		if(pResetKey != linebuf)
			continue;

        pComment = strchr(linebuf, '#');
        if(pComment != NULL) {
            memset(pComment, 0, strlen(pComment));
        }
        pComment = NULL;

        pNextLine = strchr(linebuf, '\n');
        if(pNextLine != NULL) {
            memset(pNextLine, 0, strlen(pNextLine));
        }
        pNextLine = NULL;

        if ((pEqual = strstr(linebuf, "=")) == NULL)
            continue;

		memcpy(tempbuff, linebuf, pEqual - linebuf);
		trimeSpace(tempbuff, keybuff);
		trimeSpace(++pEqual, valuebuff);

		if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)keybuff, (long)valuebuff, TYPE_FROM_GUI))
			INFO("send config error! key: %s, value: %s\n", keybuff, valuebuff);

		memset(keybuff, 0, VALUE_SIZE);
		memset(valuebuff, 0, VALUE_SIZE);
		memset(tempbuff, 0, VALUE_SIZE);
	}
	fclose(fp);
	return 0;
}

void SpvStateInit(SPVSTATE* state)
{
    state->mode = MODE_VIDEO;
    state->isRecording = FALSE;
    state->isPicturing = FALSE;
    state->beginTime.tv_sec = 0; 
    state->beginTime.tv_usec = 0; 
    state->timeLapse = 0;
    state->battery = 100;
    state->charging = FALSE;
    state->wifiStatus = 0;
    state->availableSpace = 0;
    state->isDaytime = TRUE;
    state->sdStatus = 0;
    state->isKeyTone = TRUE;
    state->videoCount = 0;
    state->photoCount = 0;
    state->isPhotoEffect = FALSE;
    state->isLocked = FALSE;
    state->rearCamInserted = FALSE;
    state->reverseImage = FALSE;
}

static void GetDateTimeString(char *dtString)
{
    if(dtString == NULL)
        return;

    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    int year, month, day, hour, minute;
    year = timeinfo->tm_year + 1900;
    month = timeinfo->tm_mon + 1;
    day = timeinfo->tm_mday;
    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;

    sprintf(dtString, "%02d%02d%02d %02d:%02d", year%2000, month, day, hour, minute);
}

void LoadGlobalIcon(int iconId)
{
    if(iconId < 0 || iconId > IC_MAX)
        return;
    if(g_icon[iconId].bmWidth == 0)
    {
        LoadBitmap(HDC_SCREEN, &g_icon[iconId], SPV_ICON[iconId]);
        switch(iconId) {
            case IC_VIDEO:
            case IC_PHOTO:
            case IC_BATTERY_LOW:
            case IC_BATTERY_LOW_CHARGING:
            case IC_BATTERY_MEDIUM:
            case IC_BATTERY_MEDIUM_CHARGING:
            case IC_BATTERY_HIGH:
            case IC_BATTERY_HIGH_CHARGING:
            case IC_BATTERY_FULL:
            case IC_BATTERY_FULL_CHARGING:
                g_icon[iconId].bmType = BMP_TYPE_COLORKEY;
                g_icon[iconId].bmColorKey = RGB2Pixel(HDC_SCREEN, 0xB8, 0x40, 0x40);
                break;
            default:
                g_icon[iconId].bmType = BMP_TYPE_COLORKEY;
                g_icon[iconId].bmColorKey = PIXEL_black;
                break;
        }
    }
}

int GetTextWidth(HDC hdc, const char *string, GUI_FONTSIZE fontsize)
{
    if(string == NULL || strlen(string) <= 0)
        return 0;
    SIZE size = {0, 0};
	switch (fontsize) {
		case GUI_FONTSIZE_SMALL:
			SelectFont(hdc, g_logfont_small);
			break;
		case GUI_FONTSIZE_MIDDLE:
			SelectFont(hdc, g_logfont_middle);
			break;
		case GUI_FONTSIZE_LARGE:
			SelectFont(hdc, g_logfont_large);
			break;
		default:
			break;
	}
    GetTextExtent(hdc, string, -1, &size);
    return size.cx / SCALE_FACTOR;
}

int GetItemWidth(HDC hdc, MWI_TYPE type)
{
    char value[VALUE_SIZE] = {0};
	int IsOn = 0;
    int width = 0;
    BOOL widthSet = FALSE;
    switch (type) {
        case MWV_MODE:
        case MWV_BATTERY:
        case MWP_MODE:
        case MWP_BATTERY:
            widthSet = TRUE;
            width = MW_ICON_WIDTH;
            break;
		case MWV_RESOLUTION:
            widthSet = TRUE;
			strcpy(value, "1088");
			width = GetTextWidth(hdc, value, GUI_FONTSIZE_SMALL) + MW_ITEM_PADDING;
            break;
		case MWP_RESOLUTION:
            widthSet = TRUE;
			strcpy(value, "1088");
			width = GetTextWidth(hdc, value, GUI_FONTSIZE_SMALL) + MW_ITEM_PADDING;
            break;
		case MWV_PREVIEW_MODE:
		case MWP_PREVIEW_MODE:
            break;
        case MWV_LOOP: //loop recording
            widthSet = TRUE;
            GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
            width = atoi(value) ? MW_ICON_WIDTH + MW_ITEM_PADDING : 0;
            break;
		case MWV_LOCKED:
			widthSet = TRUE;
			width = g_spv_camera_state->isLocked ? MW_ICON_WIDTH + MW_ITEM_PADDING : 0;
			break;
        case MWP_SEQUENCE: 
            widthSet = TRUE;
            GetConfigValue(GETKEY(ID_PHOTO_MODE_SEQUENCE), value);
            width = atoi(value) ? MW_ICON_WIDTH + MW_ITEM_PADDING : 0;
            break;
        case MWV_WIFI:
        case MWP_WIFI:
            widthSet = TRUE;
            width = (g_spv_camera_state->wifiStatus == WIFI_AP_ON || g_spv_camera_state->wifiStatus == WIFI_STA_ON) ? MW_ICON_WIDTH: 0;
            break;
		case MWV_VIDEO_MODE:
            widthSet = TRUE;
            GetConfigValue(GETKEY(ID_VIDEO_TIMELAPSED_IMAGE), value);
			IsOn = strcasecmp(GETVALUE(STRING_OFF), value);
            GetConfigValue(GETKEY(ID_VIDEO_HIGH_SPEED), value);
            width = (strcasecmp(GETVALUE(STRING_OFF), value) || IsOn) ? MW_SMALL_ICON_WIDTH : 0;
			break;
		case MWP_PHOTO_MODE:
            widthSet = TRUE;
            GetConfigValue(GETKEY(ID_PHOTO_MODE_TIMER), value);
			IsOn = strcasecmp(GETVALUE(STRING_OFF), value);
            GetConfigValue(GETKEY(ID_PHOTO_MODE_AUTO), value);
            width = (strcasecmp(GETVALUE(STRING_OFF), value) || IsOn) ? MW_SMALL_ICON_WIDTH : 0;
			break;
		case MWV_LAPSE:
            widthSet = TRUE;
            //GetLapseString(value);
            strcpy(value, "000:00:00");
			width = GetTextWidth(hdc, value, GUI_FONTSIZE_LARGE);
			break;
		case MWP_COUNT_LAPSE:
            widthSet = TRUE;
			//Get_photo_count(value);
            strcpy(value, "000000");
			width = GetTextWidth(hdc, value, GUI_FONTSIZE_LARGE);
			break;
		case MWV_DATETIME:
		case MWP_DATETIME:
            strcpy(value, "000000 00:00");
            break;
        default:
            widthSet = TRUE;
            width = 0;
            break;
    }

    if(!widthSet) {
        width = GetTextWidth(hdc, value, GUI_FONTSIZE_MIDDLE) + MW_ITEM_PADDING;
    }

    return width;
}

static void ComputeItemsLocation(HDC hdc, BOOL isVideo)
{
    INFO("ComputeItemsLocation\n");
    int startx;
    BOOL alignLeft = TRUE;
    BOOL alignTop = TRUE;
	BOOL alignInter = FALSE;

    int i = 0, begin, end;
    if(isVideo) {
        begin = MWI_VIDEO_BEGIN;
        end = MWI_VIDEO_END;
    } else {
        begin = MWI_PHOTO_BEGIN;
        end = MWI_PHOTO_END;
    }

    for(i = begin; i < end + 1; i ++)
    {
        switch(i) {
            case MWV_MODE://left top corner
            case MWP_MODE:
                startx = 0;
                alignLeft = TRUE;
                alignTop = TRUE;
				alignInter = FALSE;
                break;
            case MWV_WIFI://right top corner
			case MWP_WIFI:
                startx = VIEWPORT_WIDTH;
                alignLeft = FALSE;
                alignTop = TRUE;
				alignInter = FALSE;
                break;
            case MWV_DATETIME://left bottom corner
            case MWP_DATETIME:
                startx = 0;
                alignLeft = TRUE;
                alignTop = FALSE;
				alignInter = FALSE;
                break;
            case MWV_VIDEO_MODE://right bottom corner
            case MWP_PHOTO_MODE:
                startx = VIEWPORT_WIDTH;
                alignLeft = FALSE;
                alignTop = FALSE;
				alignInter = FALSE;
                break;
			case MWP_COUNT_LAPSE:
			case MWV_LAPSE:
				startx = 0;
				alignLeft = TRUE;
				alignTop = FALSE;
				alignInter = TRUE;
				break;
			case MWP_BATTERY:
			case MWV_BATTERY:
				startx = VIEWPORT_WIDTH;
				alignLeft = FALSE;
				alignTop = FALSE;
				alignInter = TRUE;
				break;
        }

        //mwv_mode icon
        int width = GetItemWidth(hdc, i);
        if(alignLeft) {
            g_items[i].rect.left = startx;
            g_items[i].rect.right = startx + width;
            startx += width;
        } else {
            g_items[i].rect.left = startx - width;
            g_items[i].rect.right = startx;
            startx -= width;
        }

        if(alignTop) {
            g_items[i].rect.top = 0;
            g_items[i].rect.bottom = MW_BAR_HEIGHT;
        } else {
            g_items[i].rect.top = VIEWPORT_HEIGHT - MW_BAR_HEIGHT; 
            g_items[i].rect.bottom = VIEWPORT_HEIGHT;
		}
        if(alignInter) {
            g_items[i].rect.top = MW_BAR_HEIGHT + MW_BAR_PADDING; 
            g_items[i].rect.bottom = MW_BAR_HEIGHT + MW_BAR_PADDING + MW_INTERNAL_HEIGHT;
		}
        g_items[i].visibility = width > 0 ? TRUE : FALSE;
    }
}

void FreeGlobalIcon(int iconId)
{
    if(iconId < 0 || iconId > IC_MAX)
        return;

    if(g_icon[iconId].bmWidth != 0)
    {
        UnloadBitmap(&g_icon[iconId]);
        memset(&g_icon[iconId], 0, sizeof(BITMAP));
    }
}

void LoadMainWindowIcon(int iconId)
{
    if(iconId < 0 || iconId > IC_MAX)
        return;

    if(g_icon[iconId].bmWidth == 0)
    {
        LoadBitmap(HDC_SCREEN, &g_icon[iconId], SPV_ICON[iconId]);
        g_icon[iconId].bmType = BMP_TYPE_COLORKEY;
        g_icon[iconId].bmColorKey = RGB2Pixel(HDC_SCREEN, 0xB8, 0x40, 0x40);//SPV_COLOR_TRANSPARENT;
    }
}

static void DrawMainWindow(HWND hWnd, HDC hdc)
{

    int x, y, w, h;
    int i = 0, begin, end;
#if !COLORKEY_ENABLED && !defined(UBUNTU) //only do in arm with no colorkey 
    SetBrushColor (hdc, RGBA2Pixel (hdc, 0x01, 0x01, 0x01, 0x0));
    FillBox (hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
    SetMemDCAlpha (hdc, MEMDC_FLAG_SRCPIXELALPHA,0);
    SetBkMode (hdc, BM_TRANSPARENT);
#endif

    if(g_spv_camera_state->isPhotoEffect) {
        return;
    }

//#define GET_BMP
#ifdef GET_BMP
    int width = 300;//200
    int height = 128;//72
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, (VIEWPORT_WIDTH - width) >> 1, (VIEWPORT_HEIGHT - height)>>1, width, height);
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, (VIEWPORT_WIDTH - width )/2 + 4, (VIEWPORT_HEIGHT - height )/2 + 4, width - 8, height - 8);
    RECT r = {(VIEWPORT_WIDTH - width) >> 1, (VIEWPORT_HEIGHT - height)>>1, VIEWPORT_WIDTH / 2 + width / 2, VIEWPORT_HEIGHT / 2 + height / 2};
    SetTextColor(hdc, PIXEL_lightwhite);
    SetBkColor(hdc, PIXEL_transparent);
    DrawText(hdc, "准备存储卡中", -1, &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    SetBrushColor(hdc, PIXEL_black);
    Rectangle(hdc, (VIEWPORT_WIDTH - width) >> 1, (VIEWPORT_HEIGHT - height)>>1, (VIEWPORT_WIDTH + width) / 2 - 1, (VIEWPORT_HEIGHT + height) / 2 - 1);
#endif

    if(g_recompute_locations) {
        g_recompute_locations = FALSE;
        ComputeItemsLocation(hdc, g_spv_camera_state->mode == MODE_VIDEO);
    }   

    if((g_spv_camera_state->mode == MODE_VIDEO)) {
        begin = MWI_VIDEO_BEGIN;
        end = MWI_VIDEO_END;
    } else {
        begin = MWI_PHOTO_BEGIN;
        end = MWI_PHOTO_END;
    }

    for(i = begin; i < end + 1; i ++)
    {
        PMWITEM pItem = &g_items[i];
        if(!(pItem->visibility))
            continue;

        switch(pItem->style) {
            case MW_STYLE_ICON:
                {
                    LoadMainWindowIcon(pItem->iconId);
                    w = g_icon[pItem->iconId].bmWidth;
                    h = g_icon[pItem->iconId].bmHeight;
                    x = (pItem->rect).left + (RECTW(pItem->rect) - w) / 2;
                    y = (pItem->rect).top + (RECTH(pItem->rect) - h) / 2;
                    FillBoxWithBitmap(hdc, x, y, w, h, &g_icon[pItem->iconId]);
                    break;
                }
            case MW_STYLE_TEXT:
				switch (i) {
					case MWP_COUNT_LAPSE:
					case MWV_LAPSE:
						SelectFont(hdc, g_logfont_large);
						break;
					case MWV_RESOLUTION:
					case MWP_RESOLUTION:
						SelectFont(hdc, g_logfont_small);
						break;
					default:
						SelectFont(hdc, g_logfont_middle);
						break;
				}
                SetTextColor(hdc, PIXEL_lightwhite);
                SetBkMode(hdc, BM_TRANSPARENT);
                SetBkColor(hdc, PIXEL_lightwhite);
                DrawText(hdc, pItem->text, -1, &(pItem->rect), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                break;
        }
    }
    if(!g_spv_camera_state->isRecording)
        INFO("DrawMainWindow\n");
}

HDC BeginSpvPaint(HWND hWnd)
{
    HDC hdc = BeginPaint(hWnd);
    if(SCALE_FACTOR == 1) {
        SetMapMode(hdc, MM_TEXT);
    } else {
        SetMapMode(hdc, MM_ANISOTROPIC);
        POINT p;
        p.x = VIEWPORT_WIDTH; 
        p.y = VIEWPORT_HEIGHT; 
        SetWindowExt(hdc, &p); 
        p.x = DEVICE_WIDTH; 
        p.y = DEVICE_HEIGHT; 
        SetViewportExt(hdc, &p); 
    }

    return hdc;
}

void ChangeToMode(HWND hWnd, SPV_MODE mode)
{
    int i = 0;
    int winCount = 0;
    char value[VALUE_SIZE] = {0};

    if(g_spv_camera_state->mode == (mode % MODE_COUNT))
        return;

	if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_CURRENT_MODE), (long)g_mode_name[mode % MODE_COUNT], TYPE_FROM_GUI))
		INFO("send config error! key: %s, value: %s\n", GETKEY(ID_CURRENT_MODE), g_mode_name[mode % MODE_COUNT]);

	if(MODE_PHOTO == g_spv_camera_state->mode) {
		g_photo_count = g_spv_camera_state->photoCount;
		GetConfigValue(GETKEY(ID_PHOTO_RESOLUTION), value);
		g_photo_count_remain = GetPictureRemain(g_spv_camera_state->availableSpace, value);
	}

    //camera
    switch(g_spv_camera_state->mode) {
        case MODE_VIDEO:
        case MODE_PHOTO:
			UpdateModeItemsContent(g_spv_camera_state->mode);
			g_recompute_locations = TRUE;
			INFO("set camera config\n");
			UpdateMainWindow(hWnd);
            break;
        case MODE_SETUP:
            ShowSetupDialogByMode(hWnd, MODE_SETUP);
			INFO("show setup dialog\n");
            break;
    }
}

static BOOL IsTimeLapseUpdated()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, 0);
    int timeLapse = ((currentTime.tv_sec- g_spv_camera_state->beginTime.tv_sec) * 10 + 
            (currentTime.tv_usec - g_spv_camera_state->beginTime.tv_usec) / 100000) / 10;
    if(timeLapse != g_spv_camera_state->timeLapse) {
        g_spv_camera_state->timeLapse = timeLapse;
        return TRUE;
    }
    
    return FALSE;
}

static int DealClockTimer(HWND hWnd)
{
    char lapseString[VALUE_SIZE]  = {0};
    if(!g_spv_camera_state->isRecording)
        return 0;

    if(IsTimeLapseUpdated()) {
        GetLapseString(g_items[MWV_LAPSE].text);
        InvalidateItemRect(hWnd, MWV_LAPSE, TRUE);
    }
    return 0;
}

static int DealCountdownTimer(HWND hWnd)
{
    struct timeval currentTime;
    int timeLapse, timeCountdown;
    char value[VALUE_SIZE] = {0};
	int countTime = 0;
	int ret = 0;
	BOOL isTimerPhoto = FALSE;

    if(!g_spv_camera_state->isPicturing || !IsTimeLapseUpdated()) {
        return 0;
    }

    GetConfigValue(GETKEY(ID_PHOTO_MODE_TIMER), value);
    countTime = atoi(value);
	isTimerPhoto = countTime ? TRUE : FALSE;

	if(!countTime) {
		GetConfigValue(GETKEY(ID_PHOTO_MODE_AUTO), value);
		countTime = atoi(value);
	}
    if(g_spv_camera_state->timeLapse >= countTime) {
        INFO("time countdown ready, take photo\n");
		if(isTimerPhoto) {
        	KillTimer(hWnd, TIMER_COUNTDOWN);
        	g_spv_camera_state->isPicturing = FALSE;
		} else {
			//auto photo mode to clear diaplay timer count
			g_spv_camera_state->timeLapse = 0;
    	    gettimeofday(&g_spv_camera_state->beginTime, 0);
		}
        g_status->idle = 1;
        gettimeofday((struct timeval *)&(g_status->idleTime), 0);
        g_items[MWP_PHOTO_MODE].style = MW_STYLE_ICON;
        g_items[MWP_MODE].iconId = IC_PHOTO;
        ret = SendMsgToServer(MSG_SHUTTER_ACTION, 0, 0, TYPE_FROM_GUI);
        UpdateMainWindow(hWnd);
        return ret;
    } else {
        SpvPlayAudio(SPV_WAV[WAV_KEY]);
        sprintf(g_items[MWP_PHOTO_MODE].text, "%d", (atoi(value) - g_spv_camera_state->timeLapse));
        InvalidateItemRect(hWnd, MWP_PHOTO_MODE, TRUE);
    }

    return 0;
}

static BOOL BatteryFlickTimer(HWND hWnd, int id, DWORD time)
{
    static BOOL iconVisible;
    g_items[MWV_BATTERY].visibility = g_items[MWP_BATTERY].visibility = iconVisible = !iconVisible;
    if(g_spv_camera_state->mode == MODE_VIDEO) {
        InvalidateItemRect(hWnd, MWV_BATTERY, TRUE);
    } else {
        InvalidateItemRect(hWnd, MWP_BATTERY, TRUE);
    }
    return TRUE;
}

void GUIDoShutterAction(HWND hWnd)
{
	if(g_spv_camera_state->sdStatus == SD_STATUS_UMOUNT) {
		ShowInfoDialog(hWnd, INFO_DIALOG_SD_NO);
		return;
	}
	if(g_spv_camera_state->mode == MODE_VIDEO) {
		if(SendMsgToServer(MSG_SHUTTER_ACTION, 0, 0, TYPE_FROM_GUI))
			INFO("DoShutterAction error\n");
		if(g_spv_camera_state->isRecording) {
    		GetLapseString(g_items[MWV_LAPSE].text);
        	SetTimer(hWnd, TIMER_CLOCK, 10);
		} else {
    		KillTimer(hWnd, TIMER_CLOCK);
    		GetLapseString(g_items[MWV_LAPSE].text);
			if(!g_spv_camera_state->isLocked) {
				g_items[MWV_LOCKED].visibility = FALSE;
				InvalidateItemRect(hWnd, MWV_LOCKED, TRUE);
			}
    		InvalidateItemRect(hWnd, MWV_LAPSE, TRUE);
		}
	} else if(g_spv_camera_state->mode == MODE_PHOTO) {
    	char value[VALUE_SIZE] = {0};
		int photo_auto_value = 0;
    	GetConfigValue(GETKEY(ID_PHOTO_MODE_AUTO), value);
		photo_auto_value = atoi(value);
		if(g_spv_camera_state->isPicturing) {
			//to stop auto photo mode
			if(photo_auto_value) {
				KillTimer(hWnd, TIMER_COUNTDOWN);
        		g_spv_camera_state->isPicturing = FALSE;
        		UpdateMainWindow(hWnd);
				return;
			} else {
				return;
			}
		}

    	GetConfigValue(GETKEY(ID_PHOTO_MODE_TIMER), value);
    	if(atoi(value) || photo_auto_value) {
    	    g_status->idle = 0;
    	    g_spv_camera_state->isPicturing = TRUE;
    	    SetTimer(hWnd, TIMER_COUNTDOWN, 10);
    	    sprintf(g_items[MWP_PHOTO_MODE].text, "%d", atoi(value));
    	    g_items[MWP_PHOTO_MODE].style = MW_STYLE_TEXT;
    	    gettimeofday(&g_spv_camera_state->beginTime, 0);
    	    g_spv_camera_state->timeLapse = 0;
    	    g_items[MWP_MODE].iconId = IC_PHOTO_RECORDING;
		} else {
			if(SendMsgToServer(MSG_SHUTTER_ACTION, 0, 0, TYPE_FROM_GUI))
				INFO("DoShutterAction error\n");
		}
    	GetLapseString(g_items[MWP_COUNT_LAPSE].text);
    	InvalidateItemRect(hWnd, MWP_COUNT_LAPSE, TRUE);
	}
}

void UpdateModeItemsContent(SPV_MODE currentMode)
{
    char value[VALUE_SIZE] = {0};
    if(currentMode > MODE_PHOTO)
        return;

    INFO("UpdateModeItemsContent\n");
    if((currentMode == MODE_VIDEO)) {

		//video mode
        g_items[MWV_MODE].style = MW_STYLE_ICON;
		if(currentMode == MODE_VIDEO)
        g_items[MWV_MODE].iconId = g_spv_camera_state->isRecording ? IC_VIDEO_RECORDING: IC_VIDEO;
		else
        g_items[MWV_MODE].iconId = g_spv_camera_state->isRecording ? IC_CARDV_RECORDING: IC_CARDV;

		//video resolution
        g_items[MWV_RESOLUTION].style = MW_STYLE_TEXT;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_VIDEO_RESOLUTION), value);
        if(!strcasecmp(GETVALUE(STRING_2448_30FPS), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "2448@30");
        } else if(!strcasecmp(GETVALUE(STRING_1080P_60FPS), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "1080@60");
        } else if(!strcasecmp(GETVALUE(STRING_1080P_30FPS), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "1080@30");
        } else if(!strcasecmp(GETVALUE(STRING_720P_30FPS), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "720@30");
        } else if(!strcasecmp(GETVALUE(STRING_3264), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "3264");
        } else if(!strcasecmp(GETVALUE(STRING_2880), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "2880");
        } else if(!strcasecmp(GETVALUE(STRING_2048), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "2048");
        } else if(!strcasecmp(GETVALUE(STRING_1920), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "1920");
        } else if(!strcasecmp(GETVALUE(STRING_1472), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "1472");
        } else if(!strcasecmp(GETVALUE(STRING_1088), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "1088");
        } else if(!strcasecmp(GETVALUE(STRING_768), value)) {
			sprintf(g_items[MWV_RESOLUTION].text, "%s", "768");
        }
		
		//video preview mode
		g_items[MWV_PREVIEW_MODE].style = MW_STYLE_ICON;
        //memset(value, 0, sizeof(value));
        //GetConfigValue(GETKEY(ID_VIDEO_PREVIEW), value);
        //if(!strcasecmp(GETVALUE(STRING_PANORAMA_ROUND), value)) {
        //    g_items[MWV_PREVIEW_MODE].iconId = IC_VIDEO_PANORAMA_ROUND;
        //} else if(!strcasecmp(GETVALUE(STRING_PANORAMA_EXPAND), value)) {
        //    g_items[MWV_PREVIEW_MODE].iconId = IC_VIDEO_PANORAMA_EXPAND;
        //} else if(!strcasecmp(GETVALUE(STRING_TWO_SEGMENT), value)) {
        //    g_items[MWV_PREVIEW_MODE].iconId = IC_VIDEO_TWO_SEGMENT;
        //} else if(!strcasecmp(GETVALUE(STRING_THREE_SEGMENT), value)) {
        //    g_items[MWV_PREVIEW_MODE].iconId = IC_VIDEO_THREE_SEGMENT;
        //} else if(!strcasecmp(GETVALUE(STRING_FOUR_SEGMENT), value)) {
        //    g_items[MWV_PREVIEW_MODE].iconId = IC_VIDEO_FOUR_SEGMENT;
        //} else if(!strcasecmp(GETVALUE(STRING_ROUND), value)) {
        //    g_items[MWV_PREVIEW_MODE].iconId = IC_VIDEO_ROUND;
        //} else if(!strcasecmp(GETVALUE(STRING_VR), value)) {
        //    g_items[MWV_PREVIEW_MODE].iconId = IC_VIDEO_VR;
		//}

		//video loop recording
        g_items[MWV_LOOP].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_VIDEO_LOOP_RECORDING), value);
        if(!strcasecmp(GETVALUE(STRING_1_MINUTE), value)) {
            g_items[MWV_LOOP].iconId = IC_VIDEO_1MIN;
        } else if(!strcasecmp(GETVALUE(STRING_3_MINUTES), value)) {
            g_items[MWV_LOOP].iconId = IC_VIDEO_3MIN;
        } else if(!strcasecmp(GETVALUE(STRING_5_MINUTES), value)) {
            g_items[MWV_LOOP].iconId = IC_VIDEO_5MIN;
        }

		g_items[MWV_LOCKED].style = MW_STYLE_ICON;
		g_items[MWV_LOCKED].iconId = IC_LOCKED;

		//video record time
        g_items[MWV_LAPSE].style = MW_STYLE_TEXT;
        memset(g_items[MWV_LAPSE].text, 0, sizeof(g_items[MWV_LAPSE].text));
        GetLapseString(g_items[MWV_LAPSE].text);
        
		//date and time
        g_items[MWV_DATETIME].style = MW_STYLE_TEXT;
        GetDateTimeString(g_items[MWV_DATETIME].text);
        
		//video record mode :timelapse image
        g_items[MWV_VIDEO_MODE].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_VIDEO_TIMELAPSED_IMAGE), value);
		if(!strcasecmp(GETVALUE(STRING_OFF), value)) {
			INFO("timelapsed image off\n");
		} else {
        	if(!strcasecmp(GETVALUE(STRING_1_SECONDS), value)) {
        	    g_items[MWV_VIDEO_MODE].iconId = IC_VIDEO_TIMELAPSED_1S;
			} else if(!strcasecmp(GETVALUE(STRING_2_SECONDS), value)) {
        	    g_items[MWV_VIDEO_MODE].iconId = IC_VIDEO_TIMELAPSED_2S;
			} else if(!strcasecmp(GETVALUE(STRING_3_SECONDS), value)) {
        	    g_items[MWV_VIDEO_MODE].iconId = IC_VIDEO_TIMELAPSED_3S;
			} else if(!strcasecmp(GETVALUE(STRING_5_SECONDS), value)) {
        	    g_items[MWV_VIDEO_MODE].iconId = IC_VIDEO_TIMELAPSED_5S;
			} else if(!strcasecmp(GETVALUE(STRING_10_SECONDS), value)) {
        	    g_items[MWV_VIDEO_MODE].iconId = IC_VIDEO_TIMELAPSED_10S;
			}
		}

		//video record mode :high speed record
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_VIDEO_HIGH_SPEED), value);
		if(!strcasecmp(GETVALUE(STRING_OFF), value)) {
			INFO("high speed record off\n");
		} else {
        	g_items[MWV_VIDEO_MODE].iconId = IC_VIDEO_HIGH_SPEED;
		}

    } else if(currentMode == MODE_PHOTO){
		//photo mode
        g_items[MWP_MODE].style = MW_STYLE_ICON;
        g_items[MWP_MODE].iconId = g_spv_camera_state->isPicturing ? IC_PHOTO_RECORDING: IC_PHOTO;

		//photo resolution
        g_items[MWP_RESOLUTION].style = MW_STYLE_TEXT;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_RESOLUTION), value);
        if(!strcasecmp(GETVALUE(STRING_16M), value)) {
			sprintf(g_items[MWP_RESOLUTION].text, "%s", "16M");
        } else if(!strcasecmp(GETVALUE(STRING_10M), value)) {
			sprintf(g_items[MWP_RESOLUTION].text, "%s", "10M");
        } else if(!strcasecmp(GETVALUE(STRING_8M), value)) {
			sprintf(g_items[MWP_RESOLUTION].text, "%s", "8M");
        } else if(!strcasecmp(GETVALUE(STRING_4M), value)) {
			sprintf(g_items[MWP_RESOLUTION].text, "%s", "4M");
        } else if(!strcasecmp(GETVALUE(STRING_1088), value)) {
			sprintf(g_items[MWP_RESOLUTION].text, "%s", "1088");
        }
		
		//photo preview mode
		g_items[MWP_PREVIEW_MODE].style = MW_STYLE_ICON;
        //memset(value, 0, sizeof(value));
        //GetConfigValue(GETKEY(ID_PHOTO_PREVIEW), value);
        //if(!strcasecmp(GETVALUE(STRING_PANORAMA_ROUND), value)) {
        //    g_items[MWP_PREVIEW_MODE].iconId = IC_PHOTO_PANORAMA_ROUND;
        //} else if(!strcasecmp(GETVALUE(STRING_PANORAMA_EXPAND), value)) {
        //    g_items[MWP_PREVIEW_MODE].iconId = IC_PHOTO_PANORAMA_EXPAND;
        //} else if(!strcasecmp(GETVALUE(STRING_TWO_SEGMENT), value)) {
        //    g_items[MWP_PREVIEW_MODE].iconId = IC_PHOTO_TWO_SEGMENT;
        //} else if(!strcasecmp(GETVALUE(STRING_THREE_SEGMENT), value)) {
        //    g_items[MWP_PREVIEW_MODE].iconId = IC_PHOTO_THREE_SEGMENT;
        //} else if(!strcasecmp(GETVALUE(STRING_FOUR_SEGMENT), value)) {
        //    g_items[MWP_PREVIEW_MODE].iconId = IC_PHOTO_FOUR_SEGMENT;
        //} else if(!strcasecmp(GETVALUE(STRING_ROUND), value)) {
        //    g_items[MWP_PREVIEW_MODE].iconId = IC_PHOTO_ROUND;
        //} else if(!strcasecmp(GETVALUE(STRING_VR), value)) {
        //    g_items[MWP_PREVIEW_MODE].iconId = IC_PHOTO_VR;
		//}

		//photo sequence mode
        g_items[MWP_SEQUENCE].style = MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_MODE_SEQUENCE), value);
        if(!strcasecmp(GETVALUE(STRING_3PS), value)) {
            g_items[MWP_SEQUENCE].iconId = IC_PHOTO_SEQUENCE;
        } else if(!strcasecmp(GETVALUE(STRING_5PS), value)) {
            g_items[MWP_SEQUENCE].iconId = IC_PHOTO_SEQUENCE;
        } else if(!strcasecmp(GETVALUE(STRING_7PS), value)) {
            g_items[MWP_SEQUENCE].iconId = IC_PHOTO_SEQUENCE;
        } else if(!strcasecmp(GETVALUE(STRING_10PS), value)) {
            g_items[MWP_SEQUENCE].iconId = IC_PHOTO_SEQUENCE;
        }

        //photo count
        g_items[MWP_COUNT_LAPSE].style = MW_STYLE_TEXT;
        GetLapseString(g_items[MWP_COUNT_LAPSE].text);
		
		//date and time
        g_items[MWP_DATETIME].style = MW_STYLE_TEXT;
        GetDateTimeString(g_items[MWP_DATETIME].text);

		//photo mode : capture by timer
        g_items[MWP_PHOTO_MODE].style = g_spv_camera_state->isPicturing ? MW_STYLE_TEXT : MW_STYLE_ICON;
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_MODE_TIMER), value);
		if(!strcasecmp(GETVALUE(STRING_OFF), value)) {
			INFO("timelapsed image off\n");
		} else {
        	if(!strcasecmp(GETVALUE(STRING_2_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_TIMER_2S;
			} else if(!strcasecmp(GETVALUE(STRING_5_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_TIMER_5S;
			} else if(!strcasecmp(GETVALUE(STRING_10_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_TIMER_10S;
			}
		}
        int timeRemain = atoi(value) - g_spv_camera_state->timeLapse;
        timeRemain = timeRemain < 0 ? 0 : timeRemain;
        sprintf(g_items[MWP_PHOTO_MODE].text, "%d", timeRemain);

		//photo mode : auto capture
        memset(value, 0, sizeof(value));
        GetConfigValue(GETKEY(ID_PHOTO_MODE_AUTO), value);
		if(!strcasecmp(GETVALUE(STRING_OFF), value)) {
			INFO("timelapsed image off\n");
		} else {
        	if(!strcasecmp(GETVALUE(STRING_3_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_AUTO_3S;
			} else if(!strcasecmp(GETVALUE(STRING_10_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_AUTO_10S;
			} else if(!strcasecmp(GETVALUE(STRING_15_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_AUTO_15S;
			} else if(!strcasecmp(GETVALUE(STRING_20_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_AUTO_20S;
			} else if(!strcasecmp(GETVALUE(STRING_30_SECONDS), value)) {
        	    g_items[MWP_PHOTO_MODE].iconId = IC_PHOTO_AUTO_30S;
			}
		}
    }
    //mwv_battery, mwp_battery
    g_items[MWP_BATTERY].style = g_items[MWV_BATTERY].style = MW_STYLE_ICON;
    UpdateBatteryIcon();
	
	//wifi icon
	g_items[MWP_WIFI].style = g_items[MWV_WIFI].style = MW_STYLE_ICON;
	g_items[MWP_WIFI].iconId = g_items[MWV_WIFI].iconId = IC_WIFI;
}

void UpdateMainWindow(HWND hWnd)
{
    UpdateModeItemsContent(g_spv_camera_state->mode);
    g_recompute_locations = TRUE;
    g_force_cleanup_secondarydc = TRUE;
    UpdateWindow(hWnd, TRUE);
}

void InvalidateItemRect(HWND hWnd, MWI_TYPE itemId, BOOL bErase)
{
    RECT rect = g_items[itemId].rect;

    rect.left *= SCALE_FACTOR;
    rect.top *= SCALE_FACTOR;
    rect.right *= SCALE_FACTOR;
    rect.bottom *= SCALE_FACTOR;

    g_force_cleanup_secondarydc = TRUE;
    InvalidateRect(hWnd, &rect, bErase);
}

BOOL IsMainWindowActive()
{
    return g_main_window == GetActiveWindow();
}

int GetBatteryIcon()
{
    int batteryIcon = IC_BATTERY_FULL;
    int bt = g_spv_camera_state->battery;
    if(bt > 99) bt = 99;
    if(bt < 0) bt = 0;
    if(g_spv_camera_state->charging) {
        batteryIcon = bt / BATTERY_DIV + IC_BATTERY_LOW_CHARGING;
    } else {
        batteryIcon = bt / BATTERY_DIV + IC_BATTERY_LOW;
    }

    return batteryIcon;
}

void UpdateBatteryIcon()
{
    //battery
    int batteryIcon = GetBatteryIcon();
    g_items[MWV_BATTERY].iconId = batteryIcon;
    g_items[MWP_BATTERY].iconId = batteryIcon;
}

HWND GetMainWindow()
{
    return g_main_window;
}

void DealSdEvent(HWND hWnd, WPARAM wParam)
{
	if(hWnd == NULL) {
		INFO("hWnd is NULL\n");
		return;
	}

	switch (wParam) {
		case SD_STATUS_MOUNTED:
			INFO("sd mounted\n");
			GetLapseString(g_items[MWV_LAPSE].text);
			InvalidateItemRect(g_main_window, MWV_LAPSE, TRUE);
			break;
		case SD_STATUS_UMOUNT:
			INFO("sd umounted\n");
			KillTimer(g_main_window, TIMER_CLOCK);
			KillTimer(g_main_window, TIMER_COUNTDOWN);
			GetLapseString(g_items[MWV_LAPSE].text);
			if(!g_spv_camera_state->isLocked) {
				g_items[MWV_LOCKED].visibility = FALSE;
				InvalidateItemRect(g_main_window, MWV_LOCKED, TRUE);
			}
			InvalidateItemRect(g_main_window, MWV_LAPSE, TRUE);
			ShowInfoDialog(hWnd, INFO_DIALOG_SD_NO);
			break;
		case SD_STATUS_NEED_FORMAT:
			INFO("show sd format\n");
			break;
		case SD_STATUS_ERROR:
			INFO("sd error\n");
			ShowInfoDialog(hWnd, INFO_DIALOG_SD_ERROR);
			break;
		default:
			break;
	}

}

static void MeasureWindowSize()
{
    //DEVICE_WIDTH = VIEWPORT_WIDTH = g_rcDesktop.right;
    //DEVICE_HEIGHT = VIEWPORT_HEIGHT = g_rcDesktop.bottom;
    DEVICE_WIDTH = VIEWPORT_WIDTH = 128;
    DEVICE_HEIGHT = VIEWPORT_HEIGHT = 64;

    SCALE_FACTOR = 1;
#if SCALE_ENABLED
    while(VIEWPORT_HEIGHT > 300) {
        SCALE_FACTOR ++;
        VIEWPORT_WIDTH = DEVICE_WIDTH / SCALE_FACTOR;
        VIEWPORT_HEIGHT = DEVICE_HEIGHT / SCALE_FACTOR;
    }
#endif
    LARGE_SCREEN = VIEWPORT_HEIGHT >= 400;
    INFO("window infomation, w: %d, h: %d, vw: %d, vh: %d, sf: %d, large: %d\n", DEVICE_WIDTH, DEVICE_HEIGHT, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, SCALE_FACTOR, LARGE_SCREEN);
    // main window
    MW_BAR_HEIGHT = 20;
    MW_ICON_WIDTH = 24;
    MW_ICON_HEIGHT = 20;
    MW_SMALL_ICON_WIDTH = MW_ICON_HEIGHT;
    MW_ICON_PADDING = ((MW_BAR_HEIGHT - MW_ICON_HEIGHT)>>1);
    MW_ITEM_PADDING = 2;
	MW_INTERNAL_HEIGHT = 20;
	MW_BAR_PADDING = 2;
    //setup window 
    SETUP_ITEM_HEIGHT = DEVICE_HEIGHT / (LARGE_SCREEN ? 6 : 3);
}

int key_press_hook(void *context, HWND dstWnd, int message, WPARAM wParam, LPARAM lParam)
{
    gettimeofday((struct timeval *)&(g_status->idleTime), 0);
    switch(message) {
        case MSG_KEYDOWN:
            if(IsTimerInstalled(g_main_window, TIMER_AUTO_SHUTDOWN)) {
                KillTimer(g_main_window, TIMER_AUTO_SHUTDOWN);
            }
			INFO("key down play key auido\n");
            //if(!g_spv_camera_state->isKeyTone || g_fucking_volume == 0)
             //   return;
            //SpvPlayAudio(SPV_WAV[WAV_KEY]);
            break;
        case MSG_KEYUP:
            if(SCANCODE_SPV_SHUTDOWN == LOWORD(wParam)) {
				INFO("shutdown process\n");
                //ShutdownProcess();
            }
            break;
        default:
            break;
    }
    return HOOK_GOON;
}
void CleanupSecondaryDC(HWND hWnd)
{
    if(GetActiveWindow() != hWnd)
        return;
#if !COLORKEY_ENABLED
    HDC hdc;
    hdc = GetSecondaryDC(hWnd);
    if(hdc == 0)
        return;
    SetMemDCAlpha (hdc, 0, 0);
    BitBlt(hdc, 0, 0, DEVICE_WIDTH, DEVICE_HEIGHT, HDC_SCREEN, 0, 0, 0);
#endif
}

static void OnKeyUp(HWND hWnd, int keyCode, LPARAM lParam) 
{
    INFO("OnKeyUp, keyCode: %d, lParam: %ld\n", keyCode, lParam);
    char value[VALUE_SIZE] = {0};
    switch(keyCode) {
		case SCANCODE_INFOTM_MODE:
			INFO("scancode mode\n");
            if(g_spv_camera_state->mode == MODE_PHOTO && g_spv_camera_state->isPicturing)
                return;
            if(g_spv_camera_state->mode == MODE_VIDEO && g_spv_camera_state->isRecording) {
				INFO("to do take pic\n");
				if(SendMsgToServer(MSG_TAKE_SNAPSHOT, 0, 0, TYPE_FROM_GUI))
					INFO("take photo error\n");
				break;
			}
            ChangeToMode(hWnd, (g_spv_camera_state->mode + 1) % MODE_COUNT);
			break;
		case SCANCODE_INFOTM_POWER:
			break;
		case SCANCODE_INFOTM_SHUTDOWN:
            INFO("scancode shutdown\n");
            ShowInfoDialog(hWnd, INFO_DIALOG_SHUTDOWN);
			break;
		case SCANCODE_INFOTM_OK:
			INFO("scancode ok\n");
			GUIDoShutterAction(hWnd);
			break;
		case SCANCODE_INFOTM_UP:
			INFO("scancode up\n");
			INFO("switch wifi\n");
			GetConfigValue(GETKEY(ID_SETUP_WIFI_MODE), value);
			if(!strcasecmp(GETVALUE(STRING_OFF), value)) {
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_SETUP_WIFI_MODE), (long)GETVALUE(STRING_DIRECT), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_SETUP_WIFI_MODE), GETVALUE(STRING_DIRECT)); 
			} else if(!strcasecmp(GETVALUE(STRING_DIRECT), value)) {
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_SETUP_WIFI_MODE), (long)GETVALUE(STRING_OFF), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_SETUP_WIFI_MODE), GETVALUE(STRING_OFF)); 
			}
			break;
		case SCANCODE_INFOTM_MENU:
			INFO("scancode menu\n");
            if(g_spv_camera_state->mode == MODE_PHOTO && g_spv_camera_state->isPicturing)
                return;
            if(g_spv_camera_state->mode == MODE_VIDEO && g_spv_camera_state->isRecording)
                return;
            ShowSetupDialogByMode(hWnd, g_spv_camera_state->mode);
			break;
		case SCANCODE_INFOTM_DOWN:
			INFO("scancode down\n");
			INFO("change preview mode\n");
			break;
        case SCANCODE_INFOTM_LOCK:
			INFO("scancode lock\n");
            if(g_spv_camera_state->mode == MODE_VIDEO && g_spv_camera_state->isRecording) {
				INFO("detected collision video lock\n");
                //SpvSetCollision(hWnd, g_camera);
            }
            break;
        default:
            break;
    }
}

static int MainWindowProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc, mem_dc;
    static HWND textHandle = 0;
    RECT rect = {60, 40, 300, 200};
    unsigned int startTick, endTick;
    char mode[VALUE_SIZE] = {0};
    int ret = -1;
    //printf("MainWindowProc, MSG: %d\n", message);

    switch(message) {
        case MSG_CREATE:
        {
            DEVFONT *devfontBold = LoadDevFontFromFile("ttf-fb-rrncnn-0-0-UTF-8", RESOURCE_PATH"./res/SpvfontBold.ttf");
            DEVFONT *devfont = LoadDevFontFromFile("ttf-simsun-rrncnn-0-0-UTF-8", RESOURCE_PATH"./res/Spvfont.ttf");
            g_recompute_locations = TRUE;
            g_logfont_middle= CreateLogFont("ttf", "fb", "UTF-8",
                    FONT_WEIGHT_LIGHT, FONT_SLANT_ROMAN,
                    FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    MIDDLE_FONT_SIZE, 0);
            g_logfont_large = CreateLogFont("ttf", "fb", "UTF-8",
                    FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                    FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    LARGE_FONT_SIZE, 0);
            g_logfont_small= CreateLogFont("ttf", "simsun", "UTF-8",
                    FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                    FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    SMALL_FONT_SIZE, 0);
            SetWindowFont(hWnd, g_logfont_middle);
            SetTimer(hWnd, TIMER_DATETIME, 100);
    		SetTimer(hWnd, TIMER_CLOCK, 10);
			SetLanguageConfig(GetLanguageType());
            //SetTimer(hWnd, TIMER_TIRED_ALARM,  50 * 60 * 100);//60 minutes alarm
            //if(!IsSdcardMounted()) {
            //    ShowSDErrorToast(hWnd);
            //}
            //SendNotifyMessage(hWnd, MSG_BATTERY_STATUS, 0, 0);
            UpdateMainWindow(hWnd);
#ifdef GUI_ADAS_ENABLE
            InitAdasDetection();
#endif
            break;
        }
        case MSG_ACTIVE:
            INFO("MSG_ACTIVE, wparam:%d, lparam:%ld\n", wParam, lParam);
            if(wParam) {
                CleanupSecondaryDC(hWnd);
            }
            if(wParam && g_spv_camera_state->mode < MODE_COUNT) {
                //if(!SetCameraConfig(TRUE))
                    //g_camera->start_preview();
					INFO("setup config\n");
					INFO("start_preview\n");
            } else {
                //g_camera->stop_preview();
				INFO("stop_preview\n");
            }
            break;
        case MSG_BATTERY_STATUS:
        case MSG_CHARGE_STATUS:
        {
            INFO("MSG_BATTERY_STATUS, MSG_CHARGE_STATUS, msg: %d, wParam:%d, lParam:%ld\n", message, wParam, lParam);
            if(g_spv_camera_state->battery <= BATTERY_DIV && !g_spv_camera_state->charging && !IsTimerInstalled(hWnd, TIMER_BATTERY_FLICK)) {
                SetTimerEx(hWnd, TIMER_BATTERY_FLICK, 50, BatteryFlickTimer);
				if(IsMainWindowActive()) {
					ShowInfoDialog(hWnd, INFO_DIALOG_BATTERY_LOW);
				} else {
					SendNotifyMessage(GetActiveWindow(), message, wParam, lParam);
				}
            } else if(g_spv_camera_state->battery > BATTERY_DIV || g_spv_camera_state->charging) {
                KillTimer(hWnd, TIMER_BATTERY_FLICK);
                g_items[MWV_BATTERY].visibility = g_items[MWP_BATTERY].visibility = TRUE;
            }
            UpdateBatteryIcon();
			InvalidateItemRect(hWnd, MWV_BATTERY, TRUE);
            return 0;
        }
        case MSG_HDMI_HOTPLUG:
            INFO("MSG_HDMI_HOTPLUG, wParam:%d, lParam:%ld\n", wParam, lParam);
            return 0;
        case MSG_SDCARD_MOUNT:
			INFO("MSG_SDCARD_MOUNT  wParam: %d, lParam: %d\n", wParam, lParam);
			DealSdEvent(hWnd, wParam);
			break;
        case MSG_MEDIA_UPDATED:
            {
                int vc, pc;
                INFO("MSG_MEDIA_UPDATED\n");
                //GetMediaFileCount(&vc, &pc);
                //UpdatePhotoCount(hWnd, pc);
#ifdef GUI_WIFI_ENABLE
                //UpdateCapacityStatus();
#endif
                return 0;
            }
        case MSG_CHANGE_MODE:
            ChangeToMode(hWnd, wParam);
            if(!lParam && !IsMainWindowActive()) {
                do {
                    INFO("current active window: %d, g_main_window: %d\n", GetActiveWindow(), g_main_window);
                    SendMessage(GetActiveWindow(), MSG_CLOSE, IDCANCEL, 0);
                } while(!IsMainWindowActive());
            }
            break;
        case MSG_SHUTTER_PRESSED:
            {
                if(lParam == 1 && g_spv_camera_state->sdStatus != SD_STATUS_MOUNTED)
                    return 0;
                //int ret = DoShutterAction(hWnd);
                if(g_spv_camera_state->mode != MODE_VIDEO || g_spv_camera_state->isRecording != TRUE) {
                    ERROR("MSG_SHUTTER_PRESSED, start shutter pressed\n");
                    return ret;
                }
                if(FLAG_POWERED_BY_GSENSOR != wParam)
                    return ret;
                INFO("FLAG_POWERED_BY_GSENSOR detected\n");
				//SpvSetCollision(hWnd, g_camera);
				INFO("set Collision\n");
                //if(!SpvAdapterConnected()) {
                //    //SetTimerEx(hWnd, TIMER_AUTO_SHUTDOWN, 10, AutoShutdownTimerProc);
                //}
                return ret;
            }
        case MSG_GET_UI_STATUS:
            return (int)g_status;
        case MSG_IR_LED:
            INFO("MSG_IR_LED: wParam: %d\n", wParam);
            return 0;
        case MSG_COLLISION:
            INFO("MSG_COLLISION\n");
			if(g_spv_camera_state->mode == MODE_VIDEO) {
				if(g_spv_camera_state->isRecording) {
					GetLapseString(g_items[MWV_LAPSE].text);
					SetTimer(hWnd, TIMER_CLOCK, 10);
                    g_items[MWV_LOCKED].visibility = TRUE;
					UpdateMainWindow(hWnd);
				}
			}
			break;
        case MSG_REAR_CAMERA:
#ifdef GUI_CAMERA_REAR_ENABLE
            INFO("MSG_REAR_CAMERA, device inserted:%d\n", wParam);
            UpdateRearCameraStatus(wParam);
            if(IsMainWindowActive()) {
                //SetConfigValue(GETKEY(ID_VIDEO_FRONTBIG), GETVALUE(STRING_ON), TYPE_FROM_GUI);
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_VIDEO_FRONTBIG), (long)GETVALUE(STRING_ON), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_VIDEO_FRONTBIG), GETVALUE(STRING_ON)); 
                //if(!SetCameraConfig(TRUE))
                    //g_camera->start_preview();
					INFO("start_preview\n");
                if(g_spv_camera_state->isRecording && wParam) {
                    //g_camera->start_video();
					INFO("start video\n");
                }
            } else {
                PostMessage(GetActiveWindow(), MSG_REAR_CAMERA, wParam, lParam);
            }
#endif
            break;
        case MSG_REVERSE_IMAGE:
        {
#ifdef GUI_CAMERA_REAR_ENABLE
            INFO("MSG_REVERSE_IMAGE, wParam: %d\n", wParam);
            if(g_spv_camera_state->reverseImage == (BOOL)wParam) 
                return 0;

            REVERSE_IMAGE = g_spv_camera_state->reverseImage = (BOOL)wParam;

            if(wParam == 1) {
                if(!IsMainWindowActive()) {
                    INFO("close the current non-main active window: \n");
                    do {
                        INFO("\tcurrent active window: %d, g_main_window: %d\n", GetActiveWindow(), g_main_window);
                        SendMessage(GetActiveWindow(), MSG_CLOSE, IDCANCEL, 0);
                    } while(!IsMainWindowActive());
                }
            }

            if(IsMainWindowActive()) {
				//g_camera->start_preview();
				INFO("start preview\n");
                UpdateMainWindow(hWnd);
            }
#endif
            return 0;
        }
        case MSG_ACC_CONNECT:
        {
            INFO("MSG_ACC_CONNECT, wParam: %d\n", wParam);
            //SpvDelayedShutdown(wParam ? -1 : 8000);
            break;
        }
#ifdef GUI_ADAS_ENABLE
#if 0
        case MSG_LDWS:
            gettimeofday(&g_spv_camera_state->ldTime, NULL);
            memcpy(&(g_spv_camera_state->ldwsResult), (ADAS_LDWS_RESULT *)lParam, sizeof(ADAS_LDWS_RESULT));
            InvalidateRect(hWnd, 0, TRUE);
            break;
        case MSG_FCWS:
            gettimeofday(&g_spv_camera_state->fcTime, NULL);
            memcpy(&(g_spv_camera_state->fcwsResult), (ADAS_FCWS_RESULT *)lParam, sizeof(ADAS_FCWS_RESULT));
            InvalidateRect(hWnd, 0, TRUE);
            break;
#else
        case MSG_ADAS:
        {
            INFO("MSG_ADAS: lParam: %ld\n", lParam);
            struct timeval ct;
            gettimeofday(&ct, 0);
            static long last_update_time = 0;
            long current = ct.tv_sec * 1000 + ct.tv_usec / 1000;
            int warn = !ParseAdasAndWarning((ADAS_PROC_RESULT*)lParam);
            if(warn || (current - last_update_time) > 300) {
                InvalidateRect(hWnd, 0, TRUE);
                last_update_time = current;
            }
            memcpy(&(g_spv_camera_state->adasResult), (ADAS_PROC_RESULT*)lParam, sizeof(ADAS_PROC_RESULT));
            break;
        }
#endif
#endif
        case MSG_GPS:
        {
#ifdef GUI_GPS_ENABLE
            INFO("MSG_GPS: lParam: %ld, g_sync: %d\n", lParam, g_sync_gps_time);
            gps_data *gd = (gps_data *) lParam;
            if(lParam == 0)
                return;

            GPS_Data data;
            data.latitude = gd->latitude;
            data.longitude = gd->longitude;
            data.speed = gd->speed * 1.852f;
            if(data.latitude == 0.0f || data.longitude == 0.0f) {
                data.latitude = 0xFF;
                data.longitude = 0xFF;
                data.speed = 0xFF;
            }
            SpvUpdateGpsData(&data);
            memcpy(&(g_spv_camera_state->gpsData), &data, sizeof(GPS_Data));

            static BOOL time_updated = FALSE;
            if(!g_spv_camera_state->isRecording && g_sync_gps_time && !time_updated) {
                if(!SetTimeByData(gd->gut))
                    time_updated = TRUE;
            }
            free(gd);
#endif
            break;
        }
        case MSG_UPDATE_WINDOW:
            UpdateMainWindow(hWnd);
            break;
        case MSG_CAMERA_CALLBACK:
            INFO("MSG_CAMERA_CALLBACK, wParam: %d, isRecording: %d\n", wParam, g_spv_camera_state->isRecording);
			KillTimer(hWnd, TIMER_CLOCK);
            UpdateMainWindow(hWnd);
            switch(wParam) {
				case CAMERA_ERROR_INTERNAL:
					INFO("camera inter error\n");
					break;
				case CAMERA_ERROR_DISKIO:
					INFO("camera error: diskio error\n");
					break;
                case CAMERA_ERROR_SD_FULL:
					INFO("camera error: sdcard full\n");
                    break;
				default:
					break;
            }
            break;
        case MSG_CLEAN_SECONDARY_DC:
            if(IsMainWindowActive()) {
                g_force_cleanup_secondarydc = TRUE;
                InvalidateRect(hWnd, 0, TRUE);
            }
            break;
        case MSG_TIMER:
            switch(wParam)
            {
                case TIMER_CLOCK:
                    DealClockTimer(hWnd);
                    break;
                case TIMER_COUNTDOWN:
                    DealCountdownTimer(hWnd);
                    break;
                case TIMER_DATETIME:
                    if(!IsMainWindowActive())
                        break;
                    char timestring[14] = {0};
                    GetDateTimeString(timestring);
					if(g_spv_camera_state->mode <= MODE_VIDEO) {
						if(strcasecmp(timestring, g_items[MWV_DATETIME].text)) {
							strcpy(g_items[MWV_DATETIME].text, timestring);
							InvalidateItemRect(hWnd, MWV_DATETIME, TRUE);
						}
					} else if(g_spv_camera_state->mode == MODE_PHOTO) {
						if(strcasecmp(timestring, g_items[MWP_DATETIME].text)) {
							strcpy(g_items[MWP_DATETIME].text, timestring);
							InvalidateItemRect(hWnd, MWP_DATETIME, TRUE);
						}
					}
                    break;
                /*case TIMER_TIRED_ALARM:
                    {
                        static int count = 1;
                        if(count < 2)
                            count ++;
                        if(count == 2)
                            ResetTimer(hWnd, TIMER_TIRED_ALARM, 25 * 60 * 100);//8 hours later, alarm every 20 minutes
                        INFO("TIMER_TIRED_ALARM\n");
                        SpvTiredAlarm(SPV_WAV[WAV_KEY]);
                        break;
                    }*/
                default:
                    break;
            }
            break;
        case MSG_PAINT:
            hdc = BeginSpvPaint(hWnd);
            if(g_spv_camera_state->mode > MODE_PHOTO) {
                //DrawPlaybackOrSetup(hWnd, hdc);
            } else {
                DrawMainWindow(hWnd, hdc);
            }
            EndPaint(hWnd, hdc);

            if(g_force_cleanup_secondarydc) {
                //INFO("msg_paint, g_force_cleanup_secondarydc\n");
                CleanupSecondaryDC(hWnd);
                g_force_cleanup_secondarydc = FALSE;
            }
            return 0;
        case MSG_KEYUP:
            OnKeyUp(hWnd, LOWORD(wParam), lParam);
            break;
		case MSG_WIFI_STATUS:
			switch (wParam) {
				case WIFI_OPEN_AP:
					ShowInfoDialog(hWnd, INFO_DIALOG_WIFI_ENABLED);
					break;
				case WIFI_OFF:
					ShowInfoDialog(hWnd, INFO_DIALOG_WIFI_DISABLE);
					break;
				case WIFI_AP_ON:
					ShowInfoDialog(hWnd, INFO_DIALOG_WIFI_INFO);
					break;
				default:
					INFO("wifi status: %d\n", wParam);
					break;
			}
			break;
        case MSG_CLOSE:
            INFO("MSG_CLOSE, spv_main!\n");
            DestroyWindow(hWnd);
            DestroyLogFont(g_logfont_middle);
            DestroyLogFont(g_logfont_large);
            DestroyLogFont(g_logfont_small);
            PostQuitMessage(hWnd);
            break;
        default:
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int ServerMsgListener(int message, int wParam, long lParam)
{
	switch (message) {
		case MSG_USER_KEYDOWN:
			INFO("----->rec key down message: %d\n", wParam);
			PostMessage(GetActiveWindow(), MSG_KEYDOWN, wParam, lParam);
			break;
		case MSG_USER_KEYUP:
			INFO("--->rec key up message: %d\n", wParam);
			PostMessage(GetActiveWindow(), MSG_KEYUP, wParam, lParam);
			if(wParam == SCANCODE_INFOTM_SHUTDOWN) {
				INFO("power off\n");
				SendMsgToServer(MSG_SHUTDOWN, 0, 0, TYPE_FROM_GUI);
			}
			break;
		case MSG_WIFI_STATUS:
			INFO("--->rec wifi status: %d\n", wParam);
			switch (wParam) {
				case WIFI_OPEN_AP:
					PostMessage(GetActiveWindow(), MSG_WIFI_STATUS, wParam, 0);
					break;
				case WIFI_AP_ON:
					PostMessage(GetActiveWindow(), MSG_WIFI_STATUS, wParam, lParam);
					break;
				case WIFI_OFF:
					PostMessage(GetActiveWindow(), MSG_WIFI_STATUS, wParam, 0);
					break;
			}
			UpdateMainWindow(g_main_window);
			break;
		case MSG_WIFI_CONNECTION:
			INFO("--->rec wifi connection : %d\n", wParam);
			switch (wParam) {
				case WIFI_AP_CONNECTED:
					PostMessage(GetActiveWindow(), MSG_WIFI_CONNECTION, WIFI_AP_CONNECTED, 0); //close 'wifi connecting ...' window
					break;
				default:
					break;
			}
		case MSG_BATTERY_STATUS:
			INFO("rec MSG_BATTERY_STATUS  message: %d\n", wParam);
			PostMessage(g_main_window, MSG_BATTERY_STATUS, wParam, 0);
			break;
		case MSG_CHARGE_STATUS:
			INFO("rec MSG_CHARGE_STATUS message: %d\n", wParam);
			PostMessage(g_main_window, MSG_CHARGE_STATUS, wParam, 0);
			break;
		case MSG_MEDIA_UPDATED:
			INFO("rec MSG_MEDIA_UPDATED message: %d\n", wParam);
			break;
		case MSG_COLLISION:
			INFO("rec MSG_COLLISION message: %d\n", wParam);
			PostMessage(g_main_window, MSG_COLLISION, wParam, 0);
			break;
		case MSG_REAR_CAMERA:
			INFO("rec MSG_REAR_CAMERA message: %d\n", wParam);
			break;
		case MSG_ACC_CONNECT:
			INFO("rec MSG_ACC_CONNECT message: %d\n", wParam);
			break;
		case MSG_GPS:
			INFO("rec MSG_GPS message: %d\n", wParam);
			break;
		case MSG_CAMERA_CALLBACK:
			if(!g_spv_camera_state->isRecording)
				break;
			PostMessage(g_main_window, MSG_CAMERA_CALLBACK, wParam, lParam);
			INFO("rec MSG_CAMERA_CALLBACK message: %d\n", wParam);
			break;
		case MSG_SDCARD_MOUNT:
			PostMessage(GetActiveWindow(), MSG_SDCARD_MOUNT, wParam, lParam); 
			INFO("rec MSG_SDCARD_MOUNT message: %d\n", wParam);
			break;
		case MSG_CONFIG_CHANGED:
            if(wParam != 0 && GetKeyId((char *)wParam) >= 0) {
                if(IsMainWindowActive()) {
                    UpdateMainWindow(g_main_window);
                } else {
                    InvalidateRect(GetActiveWindow(), 0, TRUE);
                }
            }
			INFO("rec MSG_CONFIG_CHANGED message: %s, id: %d\n", wParam, GetKeyId((char *)wParam));
			break;
		default:
			INFO("----->rec message: %d\n", message);
			break;
	}
}

void GUIKeyEventTest(char *name, void* args)
{
	INFO("GUIKeyTest name : %s\n", name);
	char *keytest = NULL;
	if((NULL == name) || (NULL == args)) {
		INFO("invalid parameter!\n");
		return;
	}

	if(!strcasecmp(name, "keytest")) 
	{
		keytest = args;
		INFO("args: %s, keytest: %s\n", args, keytest);
		if(!strcasecmp(keytest, "ms")) {
			INFO("----->key mode short up\n");
			PostMessage(GetActiveWindow(), MSG_KEYUP, SCANCODE_INFOTM_MODE, 0);
		} else if(!strcasecmp(keytest, "ml")) {
			INFO("----->key mode long up\n");
			PostMessage(GetActiveWindow(), MSG_KEYUP, SCANCODE_INFOTM_POWER, 0);
		} else if(!strcasecmp(keytest, "us")) {
			INFO("----->key up short up\n");
			PostMessage(GetActiveWindow(), MSG_KEYUP, SCANCODE_INFOTM_UP, 0);
		} else if(!strcasecmp(keytest, "ul")) {
			INFO("----->key up long up\n");
		} else if(!strcasecmp(keytest, "ds")) {
			INFO("----->key down short up\n");
			PostMessage(GetActiveWindow(), MSG_KEYUP, SCANCODE_INFOTM_DOWN, 0);
		} else if(!strcasecmp(keytest, "dl")) {
			INFO("----->key down long up\n");
			PostMessage(GetActiveWindow(), MSG_KEYUP, SCANCODE_INFOTM_MENU, 0);
		} else if(!strcasecmp(keytest, "os")) {
			INFO("----->key ok short up\n");
			PostMessage(GetActiveWindow(), MSG_KEYUP, SCANCODE_INFOTM_OK, 0);
		} else if(!strcasecmp(keytest, "ol")) {
			INFO("----->key ok long up\n");
		} else if(!strcasecmp(keytest, "dos")) {
			INFO("----->key ok short down\n");
			PostMessage(GetActiveWindow(), MSG_KEYDOWN, SCANCODE_INFOTM_OK, 0);
		}

	}

}

int MiniGUIMain(int argc, const char* argv[])
{
    MSG msg;
    MAINWINCREATE CreateInfo;

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER, "SportDV GUI", 0, 0);
#endif

    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    INFO("sysserver_proc pthread Created\n");
    pthread_create(&t, &attr, (void *)sysserver_proc, NULL);
    pthread_attr_destroy (&attr);

    g_icon = (BITMAP *)malloc(sizeof(BITMAP) * IC_MAX);
    memset(g_icon, 0, sizeof(BITMAP) * IC_MAX);
	
    RegisterSPVStaticControl();
    MeasureWindowSize();

    CreateInfo.dwStyle = WS_VISIBLE;// | WS_EX_TRANSPARENT;
#if SECONDARYDC_ENABLED
    CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT; //WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT;
#else
    CreateInfo.dwExStyle = WS_EX_TRANSPARENT; //WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT;
#endif
    CreateInfo.spCaption = "SportDV GUI";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = 0;
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = MainWindowProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = DEVICE_WIDTH;
    CreateInfo.by = DEVICE_HEIGHT;
#if COLORKEY_ENABLED || defined(UBUNTU)
    CreateInfo.iBkColor = SPV_COLOR_TRANSPARENT;
#else
    CreateInfo.iBkColor = COLOR_transparent;
#endif
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
	
	RegisterSpvClient(TYPE_FROM_GUI, ServerMsgListener);

	while(1) {
		usleep(20000);
		g_spv_camera_state = GetSpvState();
        if(g_spv_camera_state != NULL) {
        if(g_spv_camera_state->initialized == 1) {
            break;
        }
        }
	}

	event_register_handler("keytest", 0, GUIKeyEventTest);

    g_main_window = CreateMainWindow(&CreateInfo);
    if(g_main_window == HWND_INVALID) {
        ERROR("MiniGUI MAIN window create failed\n");
        return -1;
    }
    ShowWindow(g_main_window, SW_SHOWNORMAL);
    ShowCursor(FALSE);

	

    while(GetMessage(&msg, g_main_window)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    MainWindowCleanup(g_main_window);
    INFO("main window exit!\n");

    return 0;
}
