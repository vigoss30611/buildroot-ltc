#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<minigui/common.h>
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>
#include<pthread.h>

#include "spv_common.h"
#include "spv_static.h"
#include "spv_info_dialog.h"
#include "spv_app.h"
#include "spv_language_res.h"
#include "codec_volume.h"
#include "spv_item.h"
#include "config_manager.h"
#include "spv_utils.h"
#include "spv_debug.h"
#include "gui_icon.h"
#include "gui_common.h"
#include "sys_server.h"

#define TIMER_PAIRING 200
#define TIMER_COMMON 201
#define TIMER_DELETING 202

#define PAIRING_TIMEOUT 180 //seconds

static int DIALOG_BIG_WIDTH;
static int DIALOG_BIG_HEIGHT;

static int DIALOG_NORMAL_WIDTH;
static int DIALOG_NORMAL_HEIGHT;

static int DIALOG_SMALL_WIDTH;
static int DIALOG_SMALL_HEIGHT;

static PLOGFONT g_info_font;
static PLOGFONT g_normal_font;
static PLOGFONT g_pin_font;
static BITMAP icon;
static BITMAP iconVg, iconVr;

typedef struct {
    int type;
    DWORD infoData;
} DialogInfo;

typedef DialogInfo * PDialogInfo;

typedef struct {
    char pin[6];
    struct timeval beginTime;
    int timeRemain;
    RECT tRect;
} PairingInfo;

typedef PairingInfo * PPairingInfo;

typedef struct{
    int isSelectable;
    int sel;
    RECT tRect;
} CommonInfo;

typedef CommonInfo * PCommonInfo;

typedef struct{
    int volume;
    int sel;
    RECT tRect;
}VolumeInfo;

typedef VolumeInfo * PVolumeInfo;

static CTRLDATA PairingCtrl[] = 
{

};

static CTRLDATA CommonDialogCtrl[] =
{

};

static CTRLDATA VolumeDialogCtrl[] =
{

};

static void DeleteSdcard(HWND hDlg)
{
    INFO("DeteleSdcard\n");
    sleep(5);
    SendNotifyMessage(hDlg, MSG_CLOSE, IDOK, 0);
}

static void FormatSdcard(HWND hDlg)
{
	int ret = SendMsgToServer(MSG_FORMAT_SDCARD, 0, 0, TYPE_FROM_GUI);
    if(!ret) {
		SendNotifyMessage(hDlg, MSG_CLOSE, IDOK, 0);
    } else {
		SendNotifyMessage(hDlg, MSG_FORMAT_ERROR, IDOK, 0);
    }
}

static void CreateDeleteThread(HWND hDlg)
{
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr, (void*)DeleteSdcard, (void *)hDlg);
    pthread_attr_destroy (&attr);
}

static void CreateFormatThread(HWND hDlg)
{
    pthread_attr_t attr;
    pthread_t t;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&t, &attr, (void*)FormatSdcard, (void *)hDlg);
    pthread_attr_destroy (&attr);
} 


void EndInfoDialog(HWND hDlg, DWORD endCode)
{
/*
    PDialogInfo pInfo;
    pInfo = (PDialogInfo)GetWindowAdditionalData(hDlg);
    switch(pInfo->type){
        case INFO_DIALOG_SD_NO:
        case INFO_DIALOG_SD_ERROR:
        case INFO_DIALOG_SD_EMPTY:
        case INFO_DIALOG_SD_FULL:
    }
*/
    UnloadBitmap(&icon);
    UnloadBitmap(&iconVg);
    UnloadBitmap(&iconVr);
	DestroyLogFont(g_info_font);
	DestroyLogFont(g_pin_font);
	DestroyLogFont(g_normal_font);

    EndDialog(hDlg, endCode);
}

static void SetFocusColor(HDC hdc, BOOL isFocus)
{
	DWORD text_pixel;
	if(isFocus) {
		text_pixel = RGBA2Pixel(hdc, 0x00, 0x00, 0x00, 0xFF);
		SetBkMode (hdc, BM_OPAQUE);
		SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
		SetTextColor(hdc, text_pixel);
	} else {
		text_pixel = RGBA2Pixel(hdc, 0xFF, 0xFF, 0xFF, 0xFF);
		SetBkMode (hdc, BM_TRANSPARENT);
		SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
		SetTextColor(hdc, text_pixel);
	}
}

static void DrawAdasCalibrateDialog(HWND hDlg, HDC hdc)
{
#ifdef GUI_ADAS_ENABLE
#if COLORKEY_ENABLED
    SetBrushColor(hdc, SPV_COLOR_TRANSPARENT);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
#else
    SetBrushColor (hdc, RGBA2Pixel (hdc, 0x01, 0x01, 0x01, 0x0));
    FillBox (hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
    SetMemDCAlpha (hdc, MEMDC_FLAG_SRCPIXELALPHA,0);
    SetBkMode (hdc, BM_TRANSPARENT);
#endif
    //draw the hood top
    int x1 = VIEWPORT_WIDTH / 4;
    int y1 = 29 * VIEWPORT_HEIGHT / 32;
    int x2 = VIEWPORT_WIDTH * 3 / 4;
    int y2 = y1;
    SetPenWidth(hdc, 5);
    SetPenColor(hdc, PIXEL_yellow);
    LineEx(hdc, x1, y1, x2, y2);
    //SetBrushColor(hdc, PIXEL_lightwhite);
    SelectFont(hdc, g_info_font);
    SetTextColor(hdc, PIXEL_lightwhite);
    SetBkMode(hdc, BM_TRANSPARENT);
    RECT rect = {x1 - 30, y1 - 40, x2 + 30, y2 - 4};
    DrawText(hdc, GETSTRING(STRING_ADAS_HOOD_LINE), -1, &rect, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);

    //draw the road middle
    x1 = VIEWPORT_WIDTH / 2;
    y1 = VIEWPORT_HEIGHT / 4;
    x2 = x1;
    LineEx(hdc, x1, y1, x2, y2);
    rect.top = y1 - 40;
    rect.bottom = y1;
    DrawText(hdc, GETSTRING(STRING_ADAS_MIDDLE_LINE), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_TOP);

/*    SelectFont(hdc, g_pin_font);
    SetTextColor(hdc, PIXEL_lightwhite);
    SetBkColor(hdc, PIXEL_black);
    DrawText(hdc, "Goodbye!!!", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);*/
#endif
}

static void DrawShutDownDialog(HWND hDlg, HDC hdc)
{
#if defined(SHUTDOWN_LOGO)
    BITMAP shutdownBmp;

    LoadBitmap(hdc, &shutdownBmp, SPV_ICON[IC_SHUTDOWN]);
    FillBoxWithBitmap(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, &shutdownBmp);
    UnloadBitmap(&shutdownBmp);
#else
    RECT rect = {0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT};
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    SelectFont(hdc, g_pin_font);
    SetTextColor(hdc, PIXEL_lightwhite);
    SetBkColor(hdc, PIXEL_black);
    DrawText(hdc, GETVALUE(STRING_GOODBYE), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
#endif
}

static void DrawThreeLine(HWND hDlg, HDC hdc)
{
	RECT infoRect;
	SIZE infoSize;
	char infoString[3][64] = {0};
	char value[VALUE_SIZE] = {0};
	char ssid[VALUE_SIZE] = {0};
	char *pSsid = NULL;
	int i = 0;
	BOOL firstLineInter = FALSE;
    DWORD oldBrushColor, oldTextColor;
    DWORD text_pixel;

    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);

    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

	text_pixel = RGBA2Pixel(hdc, 0xFF, 0xFF, 0xFF, 0xFF);
    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
    SetTextColor(hdc, text_pixel);
    SelectFont(hdc, g_normal_font);

    int type  = (int)(((PDialogInfo)GetWindowAdditionalData(hDlg))->type);
	switch (type) {
		case INFO_DIALOG_WIFI_INFO:
			firstLineInter = TRUE;
			GetConfigValue(GETKEY(ID_SETUP_WIFI_MODE), value);
			if(!strcasecmp(value, GETVALUE(STRING_DIRECT)))
				sprintf(infoString[0], "%s", GETSTRING(STRING_DIRECT));
			else
				sprintf(infoString[0], "%s", GETSTRING(STRING_ROUTER));

			GetConfigValue(GETKEY(ID_WIFI_AP_SSID), value);
			if(!strncasecmp(value, "default", 7)) {
				if(SpvGetWifiSSID(value)) {
					INFO("get ssid error\n");
					strcpy(value, "default");
				}
			}
			if((pSsid = strstr(value, "q360")) == NULL)
				strcpy(ssid, value);
			else
				strcpy(ssid, pSsid);
			sprintf(infoString[1], "ssid: %s", ssid);
			GetConfigValue(GETKEY(ID_WIFI_AP_PSWD), value);
			sprintf(infoString[2], "ps: %s", value);
			break;
		case INFO_DIALOG_VERSION:
			firstLineInter = FALSE;
            {
                char version[128] = {0};
                SpvGetVersion(version);
			    sprintf(infoString[0], "%s: %s", GETSTRING(STRING_VERSION), version);
            }
			sprintf(infoString[1], "%s %s", GETSTRING(STRING_PRODUCT), SPV_PRODUCT);
			sprintf(infoString[2], "%s %s", GETSTRING(STRING_INC), SPV_INC);
			break;
		default:
			break;
	}

	for(i = 0; i < 3; i++) {
		GetTextExtent(hdc, infoString[i], -1, &infoSize);
		infoRect.top = VIEWPORT_HEIGHT/3 * i;
		if(firstLineInter && i == 0) {
			infoRect.left = (VIEWPORT_WIDTH - infoSize.cx) >> 1;
		} else {
			infoRect.left = 0;
		}
		infoRect.bottom = infoRect.top + infoSize.cy;
		infoRect.right = infoRect.left + infoSize.cx;
		INFO("--->infoString[%d]:\t%s\n", i, infoString[i]);
		DrawText(hdc, infoString[i], -1, &infoRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);
}

static void DrawInterText(HWND hDlg, HDC hdc, const char* interString, BOOL isSingleLine)
{
	RECT interRect;
	SIZE interSize;
    DWORD oldBrushColor, oldTextColor;
    DWORD text_pixel;

    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);

    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    SelectFont(hdc, g_info_font);
	GetTextExtent(hdc, interString, -1, &interSize);
	interRect.top = (VIEWPORT_HEIGHT - interSize.cy*2) >> 1;
	interRect.left = 0;
	interRect.bottom = interRect.top + interSize.cy*2;
	interRect.right = VIEWPORT_WIDTH;

	text_pixel = RGBA2Pixel(hdc, 0xFF, 0xFF, 0xFF, 0xFF);
    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
    SetTextColor(hdc, text_pixel);

    DrawText(hdc, interString, -1, &interRect, DT_CENTER | DT_VCENTER | (isSingleLine ? DT_SINGLELINE : DT_WORDBREAK));

    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);
}

static void DrawPairingDialog(HWND hDlg, HDC hdc)
{
    DWORD oldBrushColor, oldTextColor;
    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);

    RECT timeRect, pinRect, otherRect;
    char timeString[128], pinString[64],otherString[256], buttonString[64];
    SIZE timeSize, pinSize, otherSize;
    RECT cButton;

    int x = (VIEWPORT_WIDTH - DIALOG_BIG_WIDTH) >> 1;
    int y = (VIEWPORT_HEIGHT - DIALOG_BIG_HEIGHT) >> 1;

    PPairingInfo info = (PPairingInfo)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData);

    //INFO("add data, pin: %s, time: %d\n", info->pin, info->beginTime.tv_sec);
    //sprintf(timeString, GETSTRING(STRING_WIFI_PIN_TIME), info->timeRemain / 60, info->timeRemain % 60);
    //sprintf(pinString, GETSTRING(STRING_WIFI_PIN), info->pin);
    //strcpy(otherString, GETSTRING(STRING_WIFI_PIN_DES));
    strcpy(buttonString, GETSTRING(STRING_CANCLE));
    
    SelectFont(hdc, g_info_font);
    GetTextExtent(hdc, timeString, -1, &timeSize);
    GetTextExtent(hdc, otherString, -1, &otherSize);
    SelectFont(hdc, g_pin_font);
    GetTextExtent(hdc, pinString, -1, &pinSize);

    timeSize.cx /= SCALE_FACTOR;
    timeSize.cy /= SCALE_FACTOR;
    pinSize.cx /= SCALE_FACTOR;
    pinSize.cy /= SCALE_FACTOR;
    otherSize.cx /= SCALE_FACTOR;
    otherSize.cy /= SCALE_FACTOR;

    int textWidth = DIALOG_BIG_WIDTH;
    int timeHeight = ((timeSize.cx / textWidth) + 1) * timeSize.cy;
    int pinHeight = ((pinSize.cx / textWidth) + 1) * pinSize.cy;
    int otherHeight = ((otherSize.cx / textWidth) + 1) * otherSize.cy;

    //define Cancel Button
    cButton.left = x + DIALOG_BIG_WIDTH*3/56;
    cButton.right = x + DIALOG_BIG_WIDTH - DIALOG_BIG_WIDTH*3/56;   
    int buttonHeight = DIALOG_BIG_HEIGHT*9/40;

    int textHeight  = timeHeight + pinHeight + otherHeight + 2 * 3 + buttonHeight + buttonHeight/2;

    int topPadding = (DIALOG_BIG_HEIGHT - textHeight) >> 1;
    //INFO("toppadding: %d, textHeight: %d\n", topPadding, textHeight);
    topPadding = topPadding < 0 ? 0 : topPadding;

    timeRect.left = pinRect.left = otherRect.left = x;
    timeRect.right = pinRect.right = otherRect.right = x + textWidth;

    SetTextColor(hdc, PIXEL_lightwhite);
    SetBkMode (hdc, BM_TRANSPARENT);

    //draw time string
    SelectFont(hdc, g_info_font);
    timeRect.top = y + topPadding;
    timeRect.bottom = timeRect.top + timeHeight;
    DrawText(hdc, timeString, -1, &timeRect, DT_CENTER | DT_TOP | DT_WORDBREAK);

    info->tRect.left = timeRect.left * SCALE_FACTOR;
    info->tRect.top = timeRect.top * SCALE_FACTOR;
    info->tRect.right = timeRect.right * SCALE_FACTOR;
    info->tRect.bottom = timeRect.bottom * SCALE_FACTOR;
    
    //draw pin string
    SelectFont(hdc, g_pin_font);
    pinRect.top = timeRect.bottom + 3;
    pinRect.bottom = pinRect.top + pinHeight;
    DrawText(hdc, pinString, -1, &pinRect, DT_CENTER | DT_TOP | DT_WORDBREAK);

    //draw other info string
    SelectFont(hdc, g_info_font);
    otherRect.top = pinRect.bottom + 3;
    otherRect.bottom = otherRect.top + otherHeight;
    DrawText(hdc, otherString, -1, &otherRect, DT_CENTER | DT_TOP | DT_WORDBREAK);

    //draw button
    cButton.top = otherRect.bottom + buttonHeight/2;
    cButton.bottom = cButton.top + buttonHeight;
    SetBrushColor(hdc, RGB2Pixel(hdc, 0xD5, 0xD2, 0xCD));
    FillBox(hdc, cButton.left, cButton.top, RECTW(cButton), RECTH(cButton));
    SetBrushColor(hdc, PIXEL_red);
    FillBox(hdc, cButton.left, cButton.top, 8, RECTH(cButton));
    SetTextColor(hdc, PIXEL_black);
    DrawText(hdc, buttonString, -1, &cButton, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);
}

static void DrawVolumeContent(HWND hDlg, HDC hdc){
    
    DWORD oldBrushColor, oldTextColor;
    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);


    int x = (VIEWPORT_WIDTH - DIALOG_BIG_WIDTH) >> 1;
    int y = (VIEWPORT_HEIGHT - DIALOG_BIG_HEIGHT) >> 1;
    
    PVolumeInfo info = (PVolumeInfo)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData);

    SetTextColor(hdc, PIXEL_black);
    SetBkMode (hdc, BM_TRANSPARENT);

    

    //draw icon
    LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_VOLUME]);
    icon.bmType = BMP_TYPE_COLORKEY;
    icon.bmColorKey = PIXEL_black;
    int iconHeight = DIALOG_BIG_HEIGHT/5;
    int iconWidth = icon.bmWidth * iconHeight / icon.bmHeight;
    int iconX = x + (DIALOG_BIG_WIDTH - iconWidth)/2; //(x + DIALOG_BIG_WIDTH)/2;
    int iconY = y+DIALOG_BIG_HEIGHT/15;
    FillBoxWithBitmap(hdc, iconX, iconY, iconWidth, iconHeight, &icon);
    
    
    //draw volume
    LoadBitmap(HDC_SCREEN, &iconVg, SPV_ICON[IC_VOLUME_MUTE]);
    iconVg.bmType = BMP_TYPE_COLORKEY;
    iconVg.bmColorKey = PIXEL_black;
    LoadBitmap(HDC_SCREEN, &iconVr, SPV_ICON[IC_VOLUME_MAX]);
    iconVr.bmType = BMP_TYPE_COLORKEY;
    iconVr.bmColorKey = PIXEL_black;

    int iconVHeight = iconVg.bmHeight;//DIALOG_BIG_HEIGHT*3/10;
    int iconVWidth = iconVg.bmWidth;//((iconVg.bmWidth * iconVHeight / iconVg.bmHeight)/10) * 10 + 10;

    int iconVFactor = iconVWidth/10;

    //draw red volume
    
   // float tempV = (info->volume/CODEC_VOLUME_MAX) * 10;
    int tempVR = info->volume;
//  INFO("@@@@the value of tempVR: %d, info->volume: %f\n", tempVR, info->volume);

    int iconVRX = x + (DIALOG_BIG_WIDTH - iconVWidth)/2;
    int iconVRY = iconY + iconHeight + DIALOG_BIG_HEIGHT/20;
    int iconVRW = iconVFactor * tempVR;
    int iconVRH = iconVHeight;
    int iconVRBW = iconVWidth * SCALE_FACTOR;
    int iconVRBH = iconVHeight * SCALE_FACTOR;
    int iconVRX0 = 0;
    int iconVRY0 = 0;

    FillBoxWithBitmapPart(hdc, iconVRX, iconVRY, iconVRW, iconVRH, iconVRBW, iconVRBH, &iconVr, iconVRX0, iconVRY0);
    
    // draw grey voluem
    int tempVG = 10 - tempVR;

    int iconVGX = x + (DIALOG_BIG_WIDTH - iconVWidth)/2 + iconVFactor * tempVR;
    int iconVGY = iconY + iconHeight + DIALOG_BIG_HEIGHT/20;
    int iconVGW = iconVFactor * tempVG;
    int iconVGH = iconVHeight;
    int iconVGBW = iconVWidth * SCALE_FACTOR;
    int iconVGBH = iconVHeight * SCALE_FACTOR;
    int iconVGX0 = iconVWidth * SCALE_FACTOR * tempVR / 10;
    int iconVGY0 = 0;


    FillBoxWithBitmapPart(hdc, iconVGX, iconVGY, iconVGW, iconVGH, iconVGBW, iconVGBH, &iconVg, iconVGX0, iconVGY0);




    //draw button
    RECT bRect1, bRect2;
    bRect1.top = bRect2.top = iconVGY + iconVGH + DIALOG_BIG_HEIGHT/8;
    bRect1.bottom = bRect2.bottom = bRect2.top + DIALOG_BIG_HEIGHT/5;
    bRect1.left = x + DIALOG_BIG_WIDTH/9;//iconVRX;
    bRect1.right = bRect1.left + DIALOG_BIG_WIDTH/3;

    bRect2.right = x + DIALOG_BIG_WIDTH*8/9;
    bRect2.left = bRect2.right - DIALOG_BIG_WIDTH/3;

    SetBrushColor(hdc, RGB2Pixel(hdc, 0xD5, 0xD2, 0xCD));

    FillBox(hdc, bRect1.left, bRect1.top, RECTW(bRect1), RECTH(bRect1));
    FillBox(hdc, bRect2.left, bRect2.top, RECTW(bRect2), RECTH(bRect2));
      
    //draw button text
    SelectFont(hdc, g_info_font);
    SetTextColor(hdc, PIXEL_black);
    DrawText(hdc, "-", -1, &bRect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DrawText(hdc, "+", -1, &bRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SetBrushColor(hdc, PIXEL_red);
    if(0 == info->sel){
       
        FillBox(hdc, bRect1.left, bRect1.top, 8, RECTH(bRect1));

    }
    else if(1 == info->sel){
       
        FillBox(hdc, bRect2.left, bRect2.top, 8, RECTH(bRect2));
        
    }


    info->tRect.left = x * SCALE_FACTOR;
    info->tRect.right = (x+DIALOG_BIG_WIDTH) * SCALE_FACTOR;
    info->tRect.top = iconY * SCALE_FACTOR;
    info->tRect.bottom = bRect2.bottom * SCALE_FACTOR;
    
    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);

}



static void DrawBigContent(HWND hDlg, HDC hdc){

    char cttStr[128] = {0};
    DWORD oldBrushColor, oldTextColor;
    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);
    SIZE strSize;
    RECT strRect;

    int x = (VIEWPORT_WIDTH - DIALOG_BIG_WIDTH) >> 1;
    int y = (VIEWPORT_HEIGHT - DIALOG_BIG_HEIGHT) >> 1;
    
    PCommonInfo info = (PCommonInfo)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData);
    int type  = (int)(((PDialogInfo)GetWindowAdditionalData(hDlg))->type);

    switch(type){
        case INFO_DIALOG_DELETE_LAST:
            //strcpy(cttStr, GETSTRING(STRING_DEL_LAST_Q));
            LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_DELETE]);
            break;
        case INFO_DIALOG_DELETE_ALL:
            //strcpy(cttStr, GETSTRING(STRING_DEL_ALL_Q));
            LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_DELETE]);
            break;
        case INFO_DIALOG_DELETE_PROG:
            //strcpy(cttStr, GETSTRING(STRING_DEL_PROG_S));
            if(0 == info->sel)
                LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_DELETED]);
            else if(1 == info->sel)
                LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_DELETING]); 
            break;
        case INFO_DIALOG_FORMAT_PROG:
            if(info->sel == 0) {
                strcpy(cttStr, GETSTRING(STRING_FORMATE_ERROR));
            } else {
                strcpy(cttStr, GETSTRING(STRING_FORMATE_PROG_S));
            }
            LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_SETUP]);
            break;
    }

    icon.bmType = BMP_TYPE_COLORKEY;
    icon.bmColorKey = PIXEL_black;

    SelectFont(hdc, g_info_font);
    GetTextExtent(hdc, cttStr, -1, &strSize);
    
    strSize.cx /= SCALE_FACTOR;
    strSize.cy /= SCALE_FACTOR;

    if(info->isSelectable)
    {

        int textWidth = DIALOG_BIG_WIDTH;
        int strHeight = ((strSize.cx / textWidth) + 1) * strSize.cy + 4;
        
        int textHeight  = strHeight;
        
        int topPadding = (DIALOG_BIG_HEIGHT - textHeight) >> 1;
        //INFO("toppadding: %d, textHeight: %d\n", topPadding, textHeight);
        topPadding = topPadding < 0 ? 0 : topPadding;

        SetTextColor(hdc, PIXEL_lightwhite);
        SetBkMode (hdc, BM_TRANSPARENT);

        //draw icon
        int iconHeight = icon.bmHeight;//DIALOG_BIG_HEIGHT/5;
        int iconWidth = icon.bmWidth * iconHeight / icon.bmHeight;
        FillBoxWithBitmap(hdc, (x + DIALOG_BIG_WIDTH)/2 - iconWidth/8, y+DIALOG_BIG_HEIGHT/6, iconWidth, iconHeight, &icon);



        //draw string
        strRect.left = x;
        strRect.right = x + textWidth;
        strRect.top = y + DIALOG_BIG_HEIGHT/6 + iconHeight + topPadding/10;
        strRect.bottom = strRect.top + strHeight;
        DrawText(hdc, cttStr, -1, &strRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        //draw button
        RECT bRect1, bRect2;
        bRect1.top = bRect2.top = strRect.bottom + DIALOG_BIG_HEIGHT/6;
        bRect1.bottom = bRect2.bottom = bRect2.top + DIALOG_BIG_HEIGHT/5;
        bRect1.left = strRect.left + DIALOG_BIG_WIDTH/9;
        bRect1.right = bRect1.left + DIALOG_BIG_WIDTH/3;

        bRect2.right = strRect.left + DIALOG_BIG_WIDTH*8/9;
        bRect2.left = bRect2.right - DIALOG_BIG_WIDTH/3;

        SetBrushColor(hdc, RGB2Pixel(hdc, 0xD5, 0xD2, 0xCD));

        FillBox(hdc, bRect1.left, bRect1.top, RECTW(bRect1), RECTH(bRect1));
        FillBox(hdc, bRect2.left, bRect2.top, RECTW(bRect2), RECTH(bRect2));
      
        //draw button text
        SetTextColor(hdc, PIXEL_black);
        DrawText(hdc, GETSTRING(STRING_CANCLE), -1, &bRect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        //DrawText(hdc, GETSTRING(STRING_DELETE), -1, &bRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


        SetBrushColor(hdc, PIXEL_red);
        
        if(0 == info->sel){
       
            FillBox(hdc, bRect1.left, bRect1.top, 8, RECTH(bRect1));

        }
        else if(1 == info->sel){
       
            FillBox(hdc, bRect2.left, bRect2.top, 8, RECTH(bRect2));
        
        }


        info->tRect.left = x * SCALE_FACTOR;
        info->tRect.right = strRect.right * SCALE_FACTOR;
        info->tRect.top = (strRect.bottom) * SCALE_FACTOR;
        info->tRect.bottom = bRect2.bottom * SCALE_FACTOR;

    }
    else
    {
        int textWidth = DIALOG_BIG_WIDTH;
        int strHeight = ((strSize.cx / textWidth) + 1) * strSize.cy + 4;
        
        int textHeight  = strHeight;
        
        int topPadding = (DIALOG_BIG_HEIGHT - textHeight) >> 1;
        //INFO("toppadding: %d, textHeight: %d\n", topPadding, textHeight);
        topPadding = topPadding < 0 ? 0 : topPadding;

        SetTextColor(hdc, PIXEL_lightwhite);
        SetBkMode (hdc, BM_TRANSPARENT);

        //draw icon
        int iconHeight = icon.bmHeight;//DIALOG_BIG_HEIGHT/5;
        int iconWidth = icon.bmWidth * iconHeight / icon.bmHeight;
        int iconX =  x  + (DIALOG_BIG_WIDTH - iconWidth)/2;
        int iconY = y + DIALOG_BIG_HEIGHT/4;
        FillBoxWithBitmap(hdc, iconX, iconY, iconWidth, iconHeight, &icon);

        //draw string
        strRect.left = x;
        strRect.right = x + textWidth;
        strRect.top = iconY + iconHeight + topPadding/3;
        strRect.bottom = strRect.top + strHeight;
        DrawText(hdc, cttStr, -1, &strRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        info->tRect.left =  iconX * SCALE_FACTOR;
        info->tRect.right = (iconX * iconWidth) * SCALE_FACTOR;
        info->tRect.top = iconY * SCALE_FACTOR;
        info->tRect.bottom = (iconY + iconHeight) * SCALE_FACTOR;
    }


    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);
}

static void DrawNormalContent(HWND hDlg, HDC hdc){

    DWORD oldBrushColor, oldTextColor;
    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);
    SIZE strSize;
    RECT strRect;
    char cttStr[32] = {0};
	int i;

    PCommonInfo info = (PCommonInfo)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData);
    int type  = (int)(((PDialogInfo)GetWindowAdditionalData(hDlg))->type);

    switch(type){
        case INFO_DIALOG_RESET_VIDEO:
        case INFO_DIALOG_RESET_PHOTO:
            strcpy(cttStr, GETSTRING(STRING_RESTORE_DEFAULT_SETTINGS));
            break;
        case INFO_DIALOG_RESET_CAMERA:
            strcpy(cttStr, GETSTRING(STRING_RESET_CAMER_Q));
            break;
        case INFO_DIALOG_FORMAT:
            strcpy(cttStr, GETSTRING(STRING_FORMAT_Q));
			break;
		default:
			break;
    }

    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    SelectFont(hdc, g_normal_font);
    GetTextExtent(hdc, cttStr, -1, &strSize);

    if(info->isSelectable){

		int itemInternal = 4;
		int itemPadding = 10;

        //draw string
        strRect.left = 0;
        strRect.right = VIEWPORT_WIDTH;
        strRect.top = (VIEWPORT_HEIGHT - strSize.cy*3 - itemInternal) >> 1;
        strRect.bottom = strRect.top + strSize.cy*2;
		SetFocusColor(hdc, FALSE);
        DrawText(hdc, cttStr, -1, &strRect, DT_CENTER | DT_VCENTER | (strSize.cx > VIEWPORT_WIDTH ? DT_WORDBREAK : DT_SINGLELINE));
        
        //draw button
        RECT bRect[2];
		GetTextExtent(hdc, GETSTRING(STRING_FORMAT), -1, &strSize);
        bRect[0].top = bRect[1].top = strRect.bottom + itemInternal;
        bRect[0].bottom = bRect[1].bottom = bRect[1].top + strSize.cy;
        bRect[0].left = (VIEWPORT_WIDTH - strSize.cx*2 - itemPadding) >> 1;
        bRect[0].right = bRect[0].left + strSize.cx;

        bRect[1].left = bRect[0].right + itemPadding;
        bRect[1].right = bRect[1].left + strSize.cx;

        //draw button text
		for(i = 0; i < 2; i++) {
			if(!i) {
				strcpy(cttStr, GETSTRING(STRING_CANCLE));
			} else if(type == INFO_DIALOG_FORMAT) {
				strcpy(cttStr, GETSTRING(STRING_FORMAT));
			} else {
				strcpy(cttStr, GETSTRING(STRING_RESET));
			}
			if(i == info->sel)
				SetFocusColor(hdc, TRUE);
			else
				SetFocusColor(hdc, FALSE);
			DrawText(hdc, cttStr, -1, &bRect[i], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
    } else {
        icon.bmType = BMP_TYPE_COLORKEY;
        icon.bmColorKey = PIXEL_black;

		int x = (VIEWPORT_WIDTH - DIALOG_NORMAL_WIDTH) >> 1;
		int y = (VIEWPORT_HEIGHT - DIALOG_NORMAL_HEIGHT) >> 1;

        int textWidth = DIALOG_NORMAL_WIDTH;
        int strHeight = ((strSize.cx / textWidth) + 1) * strSize.cy + 4;
        
        int textHeight  = strHeight;
        
        int topPadding = (DIALOG_NORMAL_HEIGHT - textHeight) >> 1;
        //INFO("toppadding: %d, textHeight: %d\n", topPadding, textHeight);
        topPadding = topPadding < 0 ? 0 : topPadding;

        SetTextColor(hdc, PIXEL_lightwhite);
        SetBkMode (hdc, BM_TRANSPARENT);


        //draw icon
        int iconHeight = icon.bmHeight;//DIALOG_NORMAL_HEIGHT/3;
        int iconWidth = icon.bmWidth * iconHeight / icon.bmHeight;
        FillBoxWithBitmap(hdc, x + (DIALOG_NORMAL_WIDTH - iconWidth)/2, y+DIALOG_NORMAL_HEIGHT/5, iconWidth, iconHeight, &icon);

        //draw string
        strRect.left = x;
        strRect.right = x + textWidth;
        strRect.top = y + iconHeight + 4*topPadding/5 ;
        strRect.bottom = strRect.top + strHeight;
        DrawText(hdc, cttStr, -1, &strRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        info->tRect.left = x * SCALE_FACTOR;
        info->tRect.right = strRect.right * SCALE_FACTOR;
        info->tRect.top = y * SCALE_FACTOR;
        info->tRect.bottom = strRect.bottom * SCALE_FACTOR;

    }
    
    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);

}
/*
static void DrawStaticInfo(HWND hDlg, HDC hdc, INFO_DIALOG_TYPE type)
{
    int x, y;
    HDC hdc = BeginSpvPaint(hDlg);
    if(g_icon[IC_PLAYBACK_FOLDER].bmWidth == 0) {
        LoadBitmap(HDC_SCREEN, &g_icon[IC_PLAYBACK_FOLDER], SPV_ICON[IC_PLAYBACK_FOLDER]);
    }

    //draw icon
    x = 0;
    y = (FOLDER_DIALOG_TITLE_HEIGHT - g_icon[IC_PLAYBACK_FOLDER].bmHeight) >> 1;
    FillBoxWithBitmap(hdc, x, y, g_icon[IC_PLAYBACK_FOLDER].bmWidth, g_icon[IC_PLAYBACK_FOLDER].bmHeight, &g_icon[IC_PLAYBACK_FOLDER]);
    //draw divider
    x = VIEWPORT_WIDTH - FOLDER_DIALOG_TITLE_HEIGHT - 2;
    y = 10;
    SetBrushColor(hdc, PIXEL_red);
    FillBox(hdc, x, y, 2, FOLDER_DIALOG_TITLE_HEIGHT - 20);
    EndPaint(hDlg, hdc);
}*/

static void DrawSmallContent(HWND hDlg, HDC hdc){
    char cttStr[128] = {0};
    DWORD oldBrushColor, oldTextColor;
    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);
    SIZE strSize;
    RECT strRect;
    int hasIcon = -1; 

    int x = (VIEWPORT_WIDTH - DIALOG_SMALL_WIDTH) >> 1;
    int y = (VIEWPORT_HEIGHT - DIALOG_SMALL_HEIGHT) >> 1;
    
    PCommonInfo info = (PCommonInfo)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData);
    int type  = (int)(((PDialogInfo)GetWindowAdditionalData(hDlg))->type);


    switch(type){
        case INFO_DIALOG_SD_ERROR:
            strcpy(cttStr, GETSTRING(STRING_SD_ERROR));
            hasIcon = 1;
            break;
        case INFO_DIALOG_SD_EMPTY:
            strcpy(cttStr, GETSTRING(STRING_SD_EMPTY));
            hasIcon = 1;
            break;
        case INFO_DIALOG_SD_FULL:
            strcpy(cttStr, GETSTRING(STRING_SD_FULL));
            hasIcon = 1;
            break;
        case INFO_DIALOG_SD_NO:
            strcpy(cttStr, GETSTRING(STRING_SD_NO));
            hasIcon = 1;
            break;
        case INFO_DIALOG_CAMERA_BUSY:
            hasIcon = 1;
            strcpy(cttStr, GETSTRING(STRING_CAMERA_BUSY));
            break;

        case INFO_DIALOG_RESET_WIFI_PRGO:
            hasIcon = 0;
            //strcpy(cttStr, GETSTRING(STRING_RESET_WIFI_S));
            break;
    }

    SelectFont(hdc, g_info_font);
    GetTextExtent(hdc, cttStr, -1, &strSize);
    
    strSize.cx /= SCALE_FACTOR;
    strSize.cy /= SCALE_FACTOR;
   
    if(1 == hasIcon)
    {
        // define bitmap
        if(type == INFO_DIALOG_CAMERA_BUSY)
            LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_CAMERA_BUSY]);
        else
            LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_CARD]);
        icon.bmType = BMP_TYPE_COLORKEY;
        icon.bmColorKey = PIXEL_black;

        int textWidth = DIALOG_SMALL_WIDTH;
        int strHeight = ((strSize.cx / textWidth) + 1) * strSize.cy + 4;
        
        int textHeight  = strHeight;
        
        int topPadding = (DIALOG_SMALL_HEIGHT - textHeight) >> 1;
        //INFO("toppadding: %d, textHeight: %d\n", topPadding, textHeight);
        topPadding = topPadding < 0 ? 0 : topPadding;

        SetTextColor(hdc, PIXEL_lightwhite);
        SetBkMode (hdc, BM_TRANSPARENT);


        //draw icon
        int iconHeight = icon.bmHeight;//DIALOG_SMALL_HEIGHT;
        int iconWidth = icon.bmWidth * iconHeight / icon.bmHeight;
        FillBoxWithBitmap(hdc, x + DIALOG_SMALL_WIDTH/15, y + (DIALOG_SMALL_HEIGHT - iconHeight)/2, iconWidth, iconHeight, &icon);

        //draw string
        strRect.left = x + DIALOG_SMALL_WIDTH*3/28 + iconWidth;
        strRect.right = x + textWidth;
        strRect.top = y + topPadding;
        strRect.bottom = strRect.top + strHeight;
        DrawText(hdc, cttStr, -1, &strRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    }
    else if(0 == hasIcon){

        int textWidth = DIALOG_SMALL_WIDTH;
        int strHeight = ((strSize.cx / textWidth) + 1) * strSize.cy + 4;
    
        int textHeight  = strHeight;

        int topPadding = (DIALOG_SMALL_HEIGHT - textHeight) >> 1;
        //INFO("toppadding: %d, textHeight: %d\n", topPadding, textHeight);
        topPadding = topPadding < 0 ? 0 : topPadding;

        SetTextColor(hdc, PIXEL_lightwhite);
        SetBkMode (hdc, BM_TRANSPARENT);

        //draw string
        strRect.left = x;
        strRect.right = x + textWidth;
        strRect.top = y + topPadding;
        strRect.bottom = strRect.top + strHeight;
        DrawText(hdc, cttStr, -1, &strRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else
    {
        ERROR("error, no such type!\n");
    }

    if( -1 != hasIcon){
        info->tRect.left = x * SCALE_FACTOR;
        info->tRect.right = strRect.right * SCALE_FACTOR;
        info->tRect.top = strRect.top * SCALE_FACTOR;
        info->tRect.bottom = strRect.bottom * SCALE_FACTOR;
    }

    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);

}


static void DrawSmallDialog(HDC hdc){

    int x = (VIEWPORT_WIDTH - DIALOG_SMALL_WIDTH) >> 1;
    int y = (VIEWPORT_HEIGHT - DIALOG_SMALL_HEIGHT) >> 1;

    //draw retangle
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, x - 3, y - 3, DIALOG_SMALL_WIDTH + 6, DIALOG_SMALL_HEIGHT + 6);
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, x, y, DIALOG_SMALL_WIDTH, DIALOG_SMALL_HEIGHT);

}

static void DrawNormalDialog(HDC hdc){

    int x = (VIEWPORT_WIDTH - DIALOG_NORMAL_WIDTH) >> 1;
    int y = (VIEWPORT_HEIGHT - DIALOG_NORMAL_HEIGHT) >> 1;
 
    //draw retangle
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, x - 3, y - 3, DIALOG_NORMAL_WIDTH + 6, DIALOG_NORMAL_HEIGHT + 6);
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, x, y, DIALOG_NORMAL_WIDTH, DIALOG_NORMAL_HEIGHT);
}

static void DrawBigDialog(HDC hdc){

    int x = (VIEWPORT_WIDTH - DIALOG_BIG_WIDTH) >> 1;
    int y = (VIEWPORT_HEIGHT - DIALOG_BIG_HEIGHT) >> 1;
 
    //draw retangle
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, x - 3, y - 3, DIALOG_BIG_WIDTH + 6, DIALOG_BIG_HEIGHT + 6);
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, x, y, DIALOG_BIG_WIDTH, DIALOG_BIG_HEIGHT);
}

static void DoConfirm(HWND hDlg, int type)
{
    switch(type) {
        case INFO_DIALOG_RESET_VIDEO:
            INFO("to reset video setting\n");
			if(ResetConfig(RESOURCE_PATH"./config/spv_gui.config.bak", "video"))
				INFO("reset video config error\n");
			EndInfoDialog(hDlg, IDOK);
            //SendMessage(hWnd, MSG_RESTORE_DEFAULT, INFO_DIALOG_RESET_VIDEO, 0);
            break;
        case INFO_DIALOG_RESET_PHOTO:
            INFO("to reset photo setting\n");
			if(ResetConfig(RESOURCE_PATH"./config/spv_gui.config.bak", "photo"))
				INFO("reset photo config error\n");
			EndInfoDialog(hDlg, IDOK);
            //SendMessage(hWnd, MSG_RESTORE_DEFAULT, INFO_DIALOG_RESET_PHOTO, 0);
            break;
        case INFO_DIALOG_FORMAT:
			INFO("do format\n");
			EndInfoDialog(hDlg, IDOK);
            ShowInfoDialog(GetActiveWindow(), INFO_DIALOG_FORMAT_PROG);
            break;
        case INFO_DIALOG_RESET_CAMERA:
			if(SendMsgToServer(MSG_RESTORE_SETTINGS, 0, 0, TYPE_FROM_GUI))
				INFO("reset camera error\n");
			EndInfoDialog(hDlg, IDOK);
			SendNotifyMessage(GetActiveWindow(), MSG_CLOSE, IDOK, 0);
            break;
		default:
			break;
    }

}
static void DrawInfoContent(HWND hDlg, HDC hdc, PDialogInfo pInfo)
{
    if(pInfo == NULL)
        return;

    PCommonInfo info = (PCommonInfo)(pInfo->infoData);

    //draw content
    switch(pInfo->type) {
        case INFO_DIALOG_PAIRING:
            DrawBigDialog(hdc);
            DrawPairingDialog(hDlg, hdc);
            break;

        case INFO_DIALOG_VOLUME:
            DrawBigDialog(hdc);
            DrawVolumeContent(hDlg, hdc);
            break;

        case INFO_DIALOG_WIFI_ENABLED:
			DrawInterText(hDlg, hdc, GETSTRING(STRING_WIFI_DEPLOYING), TRUE);
			break;

        case INFO_DIALOG_BATTERY_LOW:
			DrawInterText(hDlg, hdc, GETSTRING(STRING_BATTERY_LOW), FALSE);
			break;

		case INFO_DIALOG_WIFI_DISABLE:
			DrawInterText(hDlg, hdc, GETSTRING(STRING_WIFI_OFF), TRUE);
			break;

        case INFO_DIALOG_FORMAT_PROG:
			if(info->sel)
				DrawInterText(hDlg, hdc, GETSTRING(STRING_FORMATE_PROG_S), TRUE);
			else
				DrawInterText(hDlg, hdc, GETSTRING(STRING_FORMATE_ERROR), TRUE);
			break;

        case INFO_DIALOG_SD_NO:
			DrawInterText(hDlg, hdc, GETSTRING(STRING_SD_NO), TRUE);
			break;

		case INFO_DIALOG_WIFI_INFO:
			INFO("show wifi info\n");
			DrawThreeLine(hDlg, hdc);
			break;

		case INFO_DIALOG_VERSION:
			INFO("show version info\n");
			DrawThreeLine(hDlg, hdc);
			break;

        case INFO_DIALOG_DELETE_LAST:
        case INFO_DIALOG_DELETE_ALL:
        case INFO_DIALOG_DELETE_PROG:
            DrawBigDialog(hdc);
            DrawBigContent(hDlg, hdc);
            break;

        case INFO_DIALOG_SD_ERROR:
        case INFO_DIALOG_SD_EMPTY:
        case INFO_DIALOG_SD_FULL:
        case INFO_DIALOG_CAMERA_BUSY:
        case INFO_DIALOG_RESET_WIFI_PRGO:
            DrawSmallDialog(hdc);
            DrawSmallContent(hDlg, hdc);
            break;

        case INFO_DIALOG_RESET_VIDEO:
        case INFO_DIALOG_RESET_PHOTO:
        case INFO_DIALOG_PAIRING_STOPPED:
        case INFO_DIALOG_RESET_CAMERA:
        case INFO_DIALOG_RESET_WIFI:
        case INFO_DIALOG_FORMAT:
            DrawNormalContent(hDlg, hdc);
            break;
        case INFO_DIALOG_SHUTDOWN:
            DrawShutDownDialog(hDlg, hdc);
            break;
        case INFO_DIALOG_ADAS_CALIBRATE:
            DrawAdasCalibrateDialog(hDlg, hdc);
            break;
        default:
            break;
    }
}


static void OnKeyUp(HWND hDlg, int keyCode, PDialogInfo pInfo)
{
    INFO("OnKeyUp: %d\n", keyCode);
    switch(pInfo->type){
        case INFO_DIALOG_PAIRING:
        {
            switch(keyCode) {
                case SCANCODE_SPV_MENU:
                    EndInfoDialog(hDlg, IDOK);
                    break;
                case SCANCODE_SPV_DOWN:
                    break;
                case SCANCODE_SPV_MODE:
                    break;
                case SCANCODE_SPV_OK:
                    EndInfoDialog(hDlg, IDOK);
                    break;
            }
            break;
        }
		case INFO_DIALOG_WIFI_INFO:
		case INFO_DIALOG_VERSION:
		case INFO_DIALOG_WIFI_ENABLED:
		case INFO_DIALOG_WIFI_DISABLE:
		{
			switch(keyCode) {
				case SCANCODE_INFOTM_MODE:
				case SCANCODE_INFOTM_OK:
					EndInfoDialog(hDlg,IDOK);
					break;
				default:
					break;
			}
		}
        case INFO_DIALOG_ADAS_CALIBRATE:
        {
            switch(keyCode) {
                case SCANCODE_SPV_MENU:
                    EndInfoDialog(hDlg, IDOK);
                    break;
            }
            break;
        }
        case INFO_DIALOG_VOLUME:
        {
            PVolumeInfo pVInfo = (PVolumeInfo) pInfo->infoData;
            switch(keyCode) {
                case SCANCODE_SPV_MENU:
                    EndInfoDialog(hDlg, IDOK);
                    break;
                case SCANCODE_SPV_DOWN:
                case SCANCODE_SPV_UP:
                    pVInfo->sel = (pVInfo->sel+1) % 2;
                    InvalidateRect(hDlg, &(pVInfo->tRect), TRUE);
                    break;
                case SCANCODE_SPV_OK:
                {
                    if(0 == pVInfo->sel){
                        pVInfo->volume--;
                        if(pVInfo->volume < 0 )
                            pVInfo->volume = 0;
                    }else if(1 == pVInfo->sel)
                    {
                        pVInfo->volume++;
                        if(pVInfo->volume > 10 )
                            pVInfo->volume = 10;
                    }  
                    //g_fucking_volume = pVInfo->volume * 10;
 
                    InvalidateRect(hDlg, &(pVInfo->tRect), TRUE);
                    break;
                }
            }
            //ResetTimer(hDlg, TIMER_COMMON, 300);
            break;
        }
        case INFO_DIALOG_RESET_CAMERA:
        case INFO_DIALOG_RESET_WIFI:
        case INFO_DIALOG_RESET_VIDEO:
        case INFO_DIALOG_RESET_PHOTO:
        case INFO_DIALOG_DELETE_LAST:
        case INFO_DIALOG_DELETE_ALL:
        case INFO_DIALOG_FORMAT:
        { 
            PCommonInfo pCInfo = (PCommonInfo) pInfo->infoData;
            switch(keyCode) {
                case SCANCODE_SPV_MENU:
                    break;
                case SCANCODE_SPV_DOWN:
                case SCANCODE_SPV_UP:
                    pCInfo->sel = !(pCInfo->sel);
                    UpdateWindow(hDlg, TRUE);
                    break;
                case SCANCODE_SPV_OK:
					if(!(pCInfo->sel)) {
						EndInfoDialog(hDlg, IDOK);
					} else {
						DoConfirm(hDlg, pInfo->type);
					}
                    break;
            }
            break;
        }
        case INFO_DIALOG_SD_NO:
        case INFO_DIALOG_SD_FULL:
        case INFO_DIALOG_SD_EMPTY:
        case INFO_DIALOG_SD_ERROR:
        {
            switch(keyCode) {
                case SCANCODE_SPV_MODE:
                case SCANCODE_SPV_OK:
                    EndInfoDialog(hDlg, IDOK);
                    break;
            }
        }
    }
}

static int FolderDialogBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
	PDialogInfo pInfo;
	pInfo = (PDialogInfo)GetWindowAdditionalData(hDlg);

    switch(message) {
        case MSG_INITDIALOG:
            {
                INFO_DIALOG_TYPE type = ((PDialogInfo)lParam)->type;
                SetWindowAdditionalData(hDlg, lParam);
                switch(type){
                    case INFO_DIALOG_PAIRING: 
                        SetTimer(hDlg, TIMER_PAIRING, 10);
                        break;
                    case INFO_DIALOG_CAMERA_BUSY:
                    case INFO_DIALOG_WIFI_ENABLED:
                        break;
                    case INFO_DIALOG_VOLUME:
                    {   PVolumeInfo pVInfo = (PVolumeInfo)(((PDialogInfo)lParam)->infoData);
                         
                        //float initV =  ((float)get_volume()/CODEC_VOLUME_MAX) * 10 + 0.5;
                        float initV =  0.5;
                        pVInfo->volume = initV;//(get_volume()/CODEC_VOLUME_MAX) * 10;
                        SetTimer(hDlg, TIMER_COMMON, 300);
                        break;
                    }
                    case INFO_DIALOG_SD_ERROR:
                    case INFO_DIALOG_SD_FULL:
                    case INFO_DIALOG_SD_EMPTY:
                    case INFO_DIALOG_SD_NO:
                    case INFO_DIALOG_RESET_WIFI_PRGO:
                    case INFO_DIALOG_PAIRING_STOPPED:
					case INFO_DIALOG_WIFI_INFO:
					case INFO_DIALOG_WIFI_DISABLE:
                    case INFO_DIALOG_SHUTDOWN:
					case INFO_DIALOG_VERSION:
					case INFO_DIALOG_BATTERY_LOW:
						INFO("set timer\n");
                        SetTimer(hDlg, TIMER_COMMON, 300);
                        break;
                    case INFO_DIALOG_RESET_VIDEO:
                    case INFO_DIALOG_RESET_PHOTO:
                    case INFO_DIALOG_RESET_CAMERA:
                    case INFO_DIALOG_RESET_WIFI:
                    case INFO_DIALOG_DELETE_LAST:
                    case INFO_DIALOG_DELETE_ALL:
                    case INFO_DIALOG_FORMAT:
                    {
                  
                        PCommonInfo pCInfo = (PCommonInfo)(((PDialogInfo)lParam)->infoData);
                        pCInfo->isSelectable = 1;
                        break;
                    }

                    case INFO_DIALOG_DELETE_PROG:
                    {
                        CreateDeleteThread(hDlg);
                        SetTimer(hDlg, TIMER_DELETING, 50);
                        break;
                    }
                    case INFO_DIALOG_FORMAT_PROG:
                    {
                        PCommonInfo pCInfo = (PCommonInfo)(((PDialogInfo)lParam)->infoData);
						pCInfo->sel = 1; //show "formatting..." in INFO_DIALOG_FORMAT_PROG
                        CreateFormatThread(hDlg);
                        break;
                    }

                }
                int fontsize = LARGE_SCREEN ? 28 : 18 * SCALE_FACTOR;
                g_normal_font = CreateLogFont("ttf", "simsun", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                        FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                        FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        14, 0);
                g_info_font = CreateLogFont("ttf", "simsun", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                        FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                        FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        fontsize, 0);
                g_pin_font = CreateLogFont("ttf", "fb", "UTF-8",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                        FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                        FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                        fontsize + 7, 0);

                //SetWindowElementAttr(control, WE_FGC_WINDOW, 0xFFFFFFFF);
                //SetWindowFont(hDlg, g_title_info_font);
                SetWindowBkColor(hDlg, COLOR_lightwhite);
                break;
            }
        case MSG_PAINT:
    		pInfo = (PDialogInfo)GetWindowAdditionalData(hDlg);
            hdc = BeginSpvPaint(hDlg);
            DrawInfoContent(hDlg, hdc, pInfo);

            EndPaint(hDlg, hdc);
            break;
        case MSG_COMMAND:
            switch(wParam) {
                case IDOK:
                case IDCANCEL:
                    EndInfoDialog(hDlg, wParam);
                    break;
            }
            break;
        case MSG_PAIRING_SUCCEED:
            INFO("pairing succeed, connecting to app mode\n");
            KillTimer(hDlg, TIMER_PAIRING);
            EndInfoDialog(hDlg, IDOK);
            break;
		case MSG_WIFI_CONNECTION:
			INFO("MSG_WIFI_CONNECTION: %d\n", wParam);
			if(wParam == WIFI_AP_CONNECTED)
				EndInfoDialog(hDlg, IDOK); //end 'wifi connecting ...' dialog
			break;
		case MSG_WIFI_STATUS:
			if(wParam == WIFI_AP_ON)
				EndInfoDialog(hDlg, IDOK); //end 'wifi connecting ...' dialog
			ShowInfoDialog(GetActiveWindow(), INFO_DIALOG_WIFI_INFO);
			break;
		case MSG_FORMAT_ERROR:
			pInfo = (PDialogInfo)GetWindowAdditionalData(hDlg);
			PCommonInfo pCInfo = (PCommonInfo)(pInfo->infoData);
			pCInfo->sel = 0; //to show "format error" in INFO_DIALOG_FORMAT_PROG
			InvalidateRect(hDlg, 0, TRUE);
			SetTimer(hDlg, TIMER_COMMON, 200);
			break;
        case MSG_KEYUP:
            INFO("spv_info_dialog, keydown: %d\n", wParam);
            OnKeyUp(hDlg, wParam, pInfo);
            break;
        case MSG_SDCARD_MOUNT:
			INFO("MSG_SDCARD_MOUNT  wParam: %d, lParam: %d\n", wParam, lParam);
			DealSdEvent(hDlg, wParam);
			break;
        case MSG_TIMER:
            switch(wParam) {
                case TIMER_PAIRING:
                {
                    PPairingInfo pPairing = (PPairingInfo)(pInfo->infoData);
                    PRECT rect = &(pPairing->tRect);
                    struct timeval curTime;
                    gettimeofday(&curTime, 0);
                        int remain = PAIRING_TIMEOUT - ((curTime.tv_sec - pPairing->beginTime.tv_sec) * 10 + 
                                (curTime.tv_usec - pPairing->beginTime.tv_usec) / 100000) / 10;
                    if(remain != pPairing->timeRemain)  {
                        pPairing->timeRemain = remain;
                        InvalidateRect(hDlg, rect, TRUE);
                    }
                    if(remain <= 0) {
                        INFO("pairing timeout, retry again\n");
                        KillTimer(hDlg, TIMER_PAIRING);
                        EndInfoDialog(hDlg, IDCANCEL);
                        return;
                    }
                    break;
                }
                case TIMER_COMMON:
                {
                    switch(pInfo->type){
                        case INFO_DIALOG_VOLUME:
                        {
                            PVolumeInfo pVInfo = (PVolumeInfo)(pInfo->infoData);
                            int finalV = (float)pVInfo->volume/10 * CODEC_VOLUME_MAX;
                            //set_volume(finalV);
                            char value[VALUE_SIZE];
                            int finalVI = pVInfo->volume;
                            //if(pVInfo->volume == 0)
                            //    SetConfigValue(GETKEY(ID_SETUP_BEEP_SOUND), GETVALUE(STRING_OFF), FROM_USER);
                            //else
                            //    SetConfigValue(GETKEY(ID_SETUP_BEEP_SOUND), GETVALUE(STRING_10P+finalVI-1), FROM_USER);
                            KillTimer(hDlg, TIMER_COMMON);
                            EndInfoDialog(hDlg,IDCANCEL);
                            break;
                        }
                        case INFO_DIALOG_SD_ERROR:
                        case INFO_DIALOG_SD_EMPTY:
                        case INFO_DIALOG_SD_FULL:
                        case INFO_DIALOG_SD_NO:
                        case INFO_DIALOG_RESET_WIFI_PRGO:
                        case INFO_DIALOG_WIFI_ENABLED:
						case INFO_DIALOG_WIFI_DISABLE:
						case INFO_DIALOG_WIFI_INFO:
						case INFO_DIALOG_BATTERY_LOW:
						case INFO_DIALOG_VERSION:
                        case INFO_DIALOG_PAIRING_STOPPED:
						case INFO_DIALOG_FORMAT_PROG:
                            KillTimer(hDlg, TIMER_COMMON);
                            EndInfoDialog(hDlg,IDCANCEL);
                            break;
                        case INFO_DIALOG_SHUTDOWN:
							break;
                        default:
                            break;
                    }
                }
                case TIMER_DELETING:
                {
                    PCommonInfo pCInfo = (PCommonInfo)(pInfo->infoData);
                    pCInfo->sel = (pCInfo->sel + 1)%2;
                    InvalidateRect(hDlg, &(pCInfo->tRect), TRUE);
                }
            }
            break;
        case MSG_CLOSE:
            EndInfoDialog(hDlg, IDOK);
            break;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}


static void MeasureWindowSize()
{
    if(!LARGE_SCREEN) {
        DIALOG_BIG_WIDTH = (VIEWPORT_WIDTH - 120);
        DIALOG_BIG_HEIGHT = (VIEWPORT_HEIGHT - 48);

        DIALOG_NORMAL_WIDTH = (VIEWPORT_WIDTH - 120);
        DIALOG_NORMAL_HEIGHT = (VIEWPORT_HEIGHT - 96);

        DIALOG_SMALL_WIDTH = (VIEWPORT_WIDTH - 120);
        DIALOG_SMALL_HEIGHT = (VIEWPORT_HEIGHT - 168);
    } else {
        DIALOG_BIG_WIDTH = 400;
        DIALOG_BIG_HEIGHT = 300;

        DIALOG_NORMAL_WIDTH = 400;
        DIALOG_NORMAL_HEIGHT = 240;

        DIALOG_SMALL_WIDTH = 400;
        DIALOG_SMALL_HEIGHT = 100;
    }
}



void ShowInfoDialog(HWND hWnd, INFO_DIALOG_TYPE type)
{
    PDialogInfo info = (PDialogInfo)malloc(sizeof(DialogInfo));
    memset(info, 0, sizeof(DialogInfo));
    info->type = type;

    DLGTEMPLATE InfoDlg =
    {
        WS_NONE,
#if SECONDARYDC_ENABLED
        WS_EX_AUTOSECONDARYDC,
#else
        WS_EX_NONE,
#endif
        0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
        "InfoDialog",
        0, 0,
        0, NULL,
        0
    };
    MeasureWindowSize();

    InfoDlg.controls = CommonDialogCtrl;
	//info->infoData = NULL;
    info->infoData = (DWORD)malloc(sizeof(CommonInfo));
    memset((void *)(info->infoData), 0, sizeof(CommonInfo));

    DialogBoxIndirectParam(&InfoDlg, hWnd, FolderDialogBoxProc, (LPARAM)info);


	free((void *)(info->infoData));
    free(info);
    INFO("ShowInfoDialog end, type: %d\n", type);
}

