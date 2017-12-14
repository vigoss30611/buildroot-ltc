#include<stdio.h>
#include<string.h>

#include<minigui/common.h>
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>

//#include "setup_button.h"
#include "spv_language_res.h"
#include "spv_scrollview.h"
#include "spv_common.h"
#include "spv_item.h"
#include "spv_setup_dialog.h"
#include "config_manager.h"
#include "spv_utils.h"
#include "spv_toast.h"
#include "spv_debug.h"

#include "spv_properties.h"
#include "sys_server.h"
#include "gui_icon.h"
#include "gui_common.h"
#include "spv_wifi.h"
#include "spv_config.h"

#define IDC_SPV_SCROLLVIEW 1234

//#define ADVANCED_COUNT 6

static PLOGFONT g_logfont, g_logfont_bold;


static void UpdateItemStatus(DIALOGPARAM *pParam, int itemId, int newStatus, int mask);
void ShowSetupDialogByDialogItem(HWND hWnd, DIALOGITEM *pItem);

static void EndSetupDialog(HWND hDlg, DWORD endCode)
{
#if 0
    HWND hostWnd = GetHosting(hDlg);
    DIALOGPARAM *param = (DIALOGPARAM *) GetWindowAdditionalData(hDlg);
    if(hostWnd != 0 && hostWnd != HWND_INVALID) {
        INFO("current mode: %d\n", GetCurrentMode());
        if(GetCurrentMode() > MODE_PHOTO || param->type == TYPE_SETUP_SETTINGS) {
#if USE_SETTINGS_MODE
            SendMessage(GetHosting(hDlg), MSG_CHANGE_MODE, MODE_VIDEO, 0);
#else
            if(param->type == TYPE_SETUP_SETTINGS) {//In system settings dialog, we need to do some special treatment
                if(endCode != IDCANCEL) {
                    SendMessage(GetHosting(hDlg), MSG_UPDATE_WINDOW, 0, 0);
                } else {
                    INFO("SendNotifyMessage, change mode\n");
                    SendNotifyMessage(GetHosting(hDlg), MSG_CHANGE_MODE, MODE_VIDEO, 0);
                }
            }
#endif
        } else {
            SendMessage(GetHosting(hDlg), MSG_UPDATE_WINDOW, 0, 0);
        }
    }
#else
    SendMessage(GetHosting(hDlg), MSG_UPDATE_WINDOW, 0, 0);
#endif

    EndDialog(hDlg, endCode);
}

/**
 *  * draw bitmap content in button with BS_MORE | BS_SWITCH style
 *   */
static void draw_bitmap_item (HDC hdc, DWORD dwStyle, RECT *prcDraw)
{
    //INFO("dwStyle: 0x%x\n", dwStyle);
    static BITMAP icons[5];
    PBITMAP bmp;
    int bmWidth, bmHeight;

    int x = prcDraw->left;
    int y = prcDraw->top;
    int w = RECTWP (prcDraw);
    int h = RECTHP (prcDraw);
    RECT rcClient = *prcDraw;

    if((dwStyle & ITEM_TYPE_MASK) == ITEM_TYPE_MORE) {
        if((dwStyle & ITEM_STATUS_DISABLE)) { // more disable
            if(icons[1].bmWidth == 0) {
                LoadBitmap(HDC_SCREEN, &icons[1], SPV_ICON[IC_MORE]);
            }
            bmp = &icons[1];
        } else {// more enable
            if(icons[0].bmWidth == 0) {
                LoadBitmap(HDC_SCREEN, &icons[0], SPV_ICON[IC_MORE]);
            }
            bmp = &icons[0];
        }
    }/* else {
        if((dwStyle & ITEM_STATUS_DISABLE)) { //switch disable
            if(icons[4].bmWidth == 0) {
                LoadBitmap(HDC_SCREEN, &icons[4], SPV_ICON[IC_SWITCH_DISABLE]);
            }
            bmp = &icons[4];
        } else if(dwStyle & ITEM_STATUS_OFF) {//switch off 
            if(icons[3].bmWidth == 0) {
                LoadBitmap(HDC_SCREEN, &icons[3], SPV_ICON[IC_SWITCH_OFF]);
            }
            bmp = &icons[3];
        } else {//switch on
            if(icons[2].bmWidth == 0) {
                LoadBitmap(HDC_SCREEN, &icons[2], SPV_ICON[IC_SWITCH_ON]);
            }
            bmp = &icons[2];
        }
    }*/

    bmWidth = bmp->bmWidth;
    bmHeight = bmp->bmHeight;

    bmWidth *= SCALE_FACTOR;
    bmHeight *= SCALE_FACTOR;

    x += (w - bmWidth - 5 * SCALE_FACTOR);
    y += ((h - bmHeight) >> 1);
    w = bmWidth;
    h = bmHeight; 

    if (bmWidth > RECTW(rcClient)){
        x = rcClient.left + BTN_WIDTH_BORDER;
        w = RECTW(rcClient) - 2*BTN_WIDTH_BORDER-1;
    }

    if (bmHeight > RECTH(rcClient)){
        y = rcClient.top + BTN_WIDTH_BORDER;
        h = RECTH(rcClient) - 2*BTN_WIDTH_BORDER-1;
    }

    bmp->bmType = BMP_TYPE_COLORKEY;
    bmp->bmColorKey = PIXEL_lightwhite;

    SetBkMode (hdc, BM_TRANSPARENT);
    FillBoxWithBitmap (hdc, x, y, w, h, bmp);
}



static void paint_content_focus(HWND hWnd, HDC hdc, HSVITEM hsvi, DIALOGITEM *pItem, RECT *rcDraw)
{
    DWORD dt_fmt = 0;
    DWORD fg_color = 0xFFFF;
    DWORD text_pixel;
    BOOL is_get_fg = FALSE;
    BOOL is_get_fmt = FALSE;
    gal_pixel old_pixel;
    RECT focus_rc;
    int status, style;
    const WINDOW_ELEMENT_RENDERER* win_rdr;
    gal_pixel brushColor;

    char caption[128] = {0};

    RECT textRect;
    RECT valueRect;
    RECT bmpRect;
    SIZE size;

    style = pItem->style;
    win_rdr = GetWindowInfo(hWnd)->we_rdr;
    textRect = *rcDraw;
    //textRect.left += 20 * SCALE_FACTOR;

    /*draw button content*/
    switch (style & ITEM_TYPE_MASK)
    {
        case ITEM_TYPE_MORE:
            draw_bitmap_item(hdc, style, rcDraw);
            break;
        case ITEM_TYPE_SWITCH:
            draw_bitmap_item(hdc, style, rcDraw);
            break;
    }

    fg_color = GetWindowElementAttr(hWnd, WE_FGC_THREED_BODY);

    is_get_fg = TRUE;
    text_pixel = RGBA2Pixel(hdc, GetRValue(fg_color),
            GetGValue(fg_color), GetBValue(fg_color),
            GetAValue(fg_color));
    if (scrollview_is_item_hilight(hWnd, hsvi)) {
		text_pixel = RGBA2Pixel(hdc, 0x00, 0x00, 0x00, 0xFF);
		SetBkMode (hdc, BM_OPAQUE);
    	SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));

	} else {
		text_pixel = RGBA2Pixel(hdc, 0xFF, 0xFF, 0xFF, 0xFF);
    	SetBkMode (hdc, BM_TRANSPARENT);
    	SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
	}

    old_pixel = SetTextColor(hdc, text_pixel);
    dt_fmt = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
    is_get_fmt = TRUE;
#ifdef GUI_SPORTDV_ENABLE
    SelectFont(hdc, g_logfont);
#else
    SelectFont(hdc, g_logfont);
#endif
    //ft2SetLcdFilter (g_logfont, MG_SMOOTH_DEFAULT);
    DrawText (hdc, GETSTRING(pItem->caption), -1, &textRect, dt_fmt);

    //draw value
    valueRect = textRect; 
    const char *pValue = NULL;
    char value[VALUE_SIZE] = {0};
    if((pItem->currentValue != 0) && ((style & ITEM_TYPE_MASK) == ITEM_TYPE_MORE)) {
        pValue = GETSTRING(pItem->currentValue);
    //} else if(pItem->itemId == ID_SETUP_PLATE_NUMBER) {
    //    int ret = GetConfigValue(GETKEY(ID_SETUP_PLATE_NUMBER), value);
    //    if(!ret) {
    //        pValue = value;
    //    }
    //} else if(pItem->itemId == ID_SETUP_VERSION) {
    //    pValue =  SPV_VERSION;
    }
    if(pValue != NULL) {
        GetTextExtent(hdc, GETSTRING(pItem->caption), -1, &size);
        valueRect.left += (size.cx + 4 * SCALE_FACTOR);
        SelectFont(hdc, g_logfont_bold);
        DrawText(hdc, pValue, -1, &valueRect, dt_fmt);
    }

    //SetTextColor(hdc, old_pixel);

    /*disable draw text*/
    if ((style & ITEM_STATUS_DISABLE) && ((style & ITEM_TYPE_MASK) == ITEM_TYPE_MORE)) {
        win_rdr->disabled_text_out (hWnd, hdc, GETSTRING(pItem->caption),
                &textRect, dt_fmt);
        if(pItem->currentValue != 0) {
            win_rdr->disabled_text_out (hWnd, hdc, GETSTRING(pItem->currentValue),
                    &valueRect, dt_fmt);
        }
    }
}

static void paint_header_item(HWND hWnd, HDC hdc, DIALOGITEM *pItem, RECT *rcDraw)
{
    static BITMAP s_ExitIcon[2];//0 exit, 1 back
    PBITMAP pExitBmp;
    const char *pExitText;
    BITMAP icon;
    SIZE size;
    int iconId = IC_SETTINGS;
    int x, y, w, h;
    int exitW = 30 * SCALE_FACTOR, exitH = 30 * SCALE_FACTOR;
    int bmWidth, bmHeight;
    RECT headerRect, exitRect;
    DWORD oldBrushColor = GetBrushColor(hdc);
    //brush black bg
    gal_pixel pixel = PIXEL_black;
    if(pItem->style & ITEM_STATUS_SECONDARY) {
        INFO("secondary header\n");
        pixel = RGB2Pixel(hdc, 0x33, 0x33, 0X33);
    }
    SetBrushColor(hdc, pixel);
    FillBox(hdc, rcDraw->left, rcDraw->top, RECTWP(rcDraw), RECTHP(rcDraw));

    //draw header icon
    switch(pItem->itemId)
    {
        //case ID_VIDEO_HEADER:
        //case ID_PHOTO_HEADER:
        //    iconId = IC_SETTINGS;
        //    break;
        //case ID_SETUP_HEADER:
        //    iconId = IC_SETUP;
        //    break;
        //case ID_DISPLAY_HEADER:
        //    iconId = IC_BRIGHTNESS;
        //    break;
        //case ID_DATE_TIME_HEADER:
        //    iconId = IC_DATE_TIME;
        //    break;
    }
    LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[iconId]);
    if(pItem->style & ITEM_STATUS_SECONDARY) {
        icon.bmType = BMP_TYPE_COLORKEY;
        icon.bmColorKey = PIXEL_black;
    }
    bmWidth = icon.bmWidth * SCALE_FACTOR;
    bmHeight = icon.bmHeight * SCALE_FACTOR;
    x = 0;
    y = rcDraw->top + ((RECTHP(rcDraw) - bmHeight) >> 1);
    FillBoxWithBitmap(hdc, x, y, bmWidth, bmHeight, &icon);
    UnloadBitmap(&icon);

    SetBkMode (hdc, BM_TRANSPARENT);
    //draw title text
    headerRect = *rcDraw;
    SetTextColor(hdc, PIXEL_lightwhite);
    SelectFont(hdc, g_logfont_bold);
    headerRect.left = x + bmWidth;
    DrawText(hdc, GETSTRING(pItem->caption), -1, &headerRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

    //draw divider
    SetBrushColor(hdc, PIXEL_red);
    FillBox(hdc, rcDraw->right - SETUP_ITEM_HEIGHT - 2 * SCALE_FACTOR, 
            rcDraw->top + 10 * SCALE_FACTOR, 2 * SCALE_FACTOR, RECTHP(rcDraw) - 20 * SCALE_FACTOR);

    //draw exit icon
    if(pItem->style & ITEM_STATUS_SECONDARY) {
        LoadGlobalIcon(IC_BACK);
        g_icon[IC_BACK].bmType = BMP_TYPE_COLORKEY;
        g_icon[IC_BACK].bmColorKey = PIXEL_black;
        pExitBmp = &g_icon[IC_BACK];
        //pExitText = GETSTRING(STRING_BACK);
    } else {
        LoadGlobalIcon(IC_EXIT);
        pExitBmp = &g_icon[IC_EXIT];
        //pExitText = GETSTRING(STRING_EXIT);
    }
    exitRect = *rcDraw;
    exitRect.left = exitRect.right - RECTH(exitRect);
    SelectFont(hdc, g_logfont);
    GetTextExtent(hdc, pExitText, -1, &size);

    x = exitRect.left + ((RECTW(exitRect) - exitW) >> 1);
    y = exitRect.top + ((RECTH(exitRect) - exitH - size.cy) >> 1);
    FillBoxWithBitmap(hdc, x, y, exitW, exitH, pExitBmp);

    //draw exit text
    SelectFont(hdc, g_logfont_bold);
    SetTextColor(hdc, PIXEL_lightwhite);
    exitRect.top = y + exitH + 2 * SCALE_FACTOR;
    DrawText(hdc, pExitText, -1, &exitRect, DT_TOP | DT_CENTER | DT_SINGLELINE);
}

static void scrollview_set_item_height_by_index(HWND hScrollView, int index, int height)
{
    PSVDATA pData = (PSVDATA) GetWindowAdditionalData2(hScrollView);
    HSVITEM hsvi = scrollview_get_item_by_index(pData, index);
    if(hsvi)
        scrollview_set_item_height(hScrollView, hsvi, height);
}

static void scrollview_refresh_item_by_index(HWND hScrollView, int index)
{
    //INFO("scrollview_refresh_item_by_index, index:%d\n", index);
    /*RECT rect;
    GetWindowRect(hScrollView, &rect);
    rect.right = rect.right - rect.left;
    rect.left = 0;
    PSVDATA pData = (PSVDATA) GetWindowAdditionalData2(hScrollView);
    HSVITEM hsvi = scrollview_get_item_by_index(pData, index);
    if(hsvi) {
        scrollview_get_item_rect(hScrollView, hsvi, &rect, FALSE);
        INFO("rect: (%d, %d, %d, %d)\n", rect.left, rect.top, rect.right, rect.bottom);
        InvalidateRect(hScrollView, &rect, TRUE);
        scrolled_content_to_window();
    }*/
}

static void ChangeToNextValue(HWND hScrollView, DIALOGITEM *pItem, int index)
{
    int i = 0;
    int count = pItem->vc;

    for(i = 0; i < count; i ++)
    {
        //INFO("i: %d, value: %d\n", i, pItem->valuelist[i]);
        if(pItem->currentValue == pItem->valuelist[i])
            break;
    }
    if(i == count) {
        ERROR("[ERROR] value not found in list, value:%d\n", pItem->currentValue);
        return;
    }
    
    int new = (i+1) % count;

    pItem->currentValue = pItem->valuelist[new];

    if(pItem->itemId == ID_SETUP_LANGUAGE) {//special for language
        INFO("set to next language, type: %d\n", new);
        SetLanguageConfig(new, TYPE_FROM_GUI);
        return;
    }
    //INFO("value changed, %s=%s\n", GETKEY(pItem->itemId), GETVALUE(pItem->currentValue));
	if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(pItem->itemId), (long)GETVALUE(pItem->currentValue), TYPE_FROM_GUI)) {
		INFO("SendMsgToServer error!  key:%s, value:%s\n", GETKEY(pItem->itemId), GETVALUE(pItem->currentValue));
	}
    //SetConfigValue(GETKEY(pItem->itemId), GETVALUE(pItem->currentValue), TYPE_FROM_GUI); 
}
/*  
static void RestoreDefaultSettings(HWND hDlg)
{
    int i = 0, j = 0, count = 0;
    const char *key = NULL;
    char value[VALUE_SIZE] = {0};
    void *backupHandle = NULL;
    DIALOGITEM *pItem;
    DIALOGPARAM *param = (DIALOGPARAM *)GetWindowAdditionalData(hDlg);
    if(param == NULL)
         return;

    init(RESOURCE_PATH"./config/libramain.config.bak", &backupHandle);
    if(backupHandle == NULL)
        return;

    for(i = 0; i < param->itemCount; i ++) {
        pItem = param->pItems + i;
        count = pItem->vc;
        memset(value, 0, VALUE_SIZE);
        key = GETKEY(pItem->itemId);
        if(key == NULL || strlen(key) <= 0)
            continue;

        getValue(backupHandle, GETKEY(pItem->itemId), value);
        for(j = 0; j < count; j ++)
        {
            if(!strcasecmp(value, GETVALUE(pItem->valuelist[j])))
                break;
        }
        if(j == count) {
            ERROR("[ERROR] value not found in list, value:%d\n", pItem->currentValue);
            continue;
        }

        pItem->currentValue = pItem->valuelist[j];
        SetConfigValue(GETKEY(pItem->itemId), GETVALUE(pItem->currentValue), TYPE_FROM_GUI); 
    }
#ifdef GUI_CAMERA_REAR_ENABLE
#ifndef GUI_CAMERA_PIP_ENABLE
    if(param->type == TYPE_VIDEO_SETTINGS) {
        SetConfigValue(GETKEY(ID_VIDEO_PIP), "Off", TYPE_FROM_GUI);
    }
#endif
#endif

    releaseHandle(&backupHandle);
    InvalidateRect(hDlg, 0, TRUE);

    SetCameraConfig(FALSE);
}*/

/*
static void OnAdvancedSwitch(HWND hScrollView, DIALOGITEM *pItem, int index)
{
    int i;
    int itemHeight = (pItem->style & ITEM_STATUS_OFF) ? 0 : SETUP_ITEM_HEIGHT;
    for(i = 1; i <= ADVANCED_COUNT; i ++) {
        scrollview_set_item_height_by_index(hScrollView, index + i, itemHeight);
    }
}*/

static BOOL SetOtherItems(HWND hScrollView, DIALOGITEM *pItem, int index)
{
    BOOL otherChanged = FALSE;
    char value[VALUE_SIZE] = {0};

    switch(pItem->itemId) {
#ifdef GUI_CAMERA_REAR_ENABLE
        case ID_VIDEO_REAR_CAMERA:
#ifdef GUI_CAMERA_PIP_ENABLE
            if(pItem->currentValue == STRING_ON && CAMERA_REAR_CONNECTED) {
                scrollview_set_item_height_by_index(hScrollView, index + (ID_VIDEO_PIP - ID_VIDEO_REAR_CAMERA), SETUP_ITEM_HEIGHT);
            } else {
                scrollview_set_item_height_by_index(hScrollView, index + (ID_VIDEO_PIP - ID_VIDEO_REAR_CAMERA), 0);
            }
#else
            scrollview_set_item_height_by_index(hScrollView, index + (ID_VIDEO_PIP - ID_VIDEO_REAR_CAMERA), 0);
#endif
            break;
#endif
		case ID_VIDEO_TIMELAPSED_IMAGE: // 缩时录影
            GetConfigValue(GETKEY(ID_VIDEO_RECORD_MODE), value);
			if(!strcasecmp(GETVALUE(STRING_TIMELAPSED_IMAGE), value)) { //open timelapsed_image mode
				scrollview_set_item_height_by_index(hScrollView, index, SETUP_ITEM_HEIGHT);
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_VIDEO_HIGH_SPEED), (long)GETVALUE(STRING_OFF), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_VIDEO_HIGH_SPEED), GETVALUE(STRING_OFF)); 

			} else if(!strcasecmp(GETVALUE(STRING_NORMAL), value)) { //disable all
				scrollview_set_item_height_by_index(hScrollView, index, 0);
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_VIDEO_TIMELAPSED_IMAGE), (long)GETVALUE(STRING_OFF), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_VIDEO_TIMELAPSED_IMAGE), GETVALUE(STRING_OFF)); 
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_VIDEO_HIGH_SPEED), (long)GETVALUE(STRING_OFF), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_VIDEO_HIGH_SPEED), GETVALUE(STRING_OFF)); 

			} else if(!strcasecmp(GETVALUE(STRING_HIGH_SPEED), value)) { //open high speed mode
				scrollview_set_item_height_by_index(hScrollView, index, 0);
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_VIDEO_TIMELAPSED_IMAGE), (long)GETVALUE(STRING_OFF), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_VIDEO_TIMELAPSED_IMAGE), GETVALUE(STRING_OFF)); 
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(ID_VIDEO_HIGH_SPEED), (long)GETVALUE(STRING_ON), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(ID_VIDEO_HIGH_SPEED), GETVALUE(STRING_ON)); 
			}
            otherChanged = TRUE;
			break;
		case ID_VIDEO_GSENSOR:
		case ID_SETUP_PARKING_GUARD:
			GetConfigValue(GETKEY(ID_SETUP_CARDV_MODE), value);
			if(!strcasecmp(GETVALUE(STRING_ON), value)) {
				scrollview_set_item_height_by_index(hScrollView, index, SETUP_ITEM_HEIGHT);
			} else {
				scrollview_set_item_height_by_index(hScrollView, index, 0);
			}
			break;
		case ID_PHOTO_MODE_TIMER:
		case ID_PHOTO_MODE_AUTO:
		case ID_PHOTO_MODE_SEQUENCE:
            GetConfigValue(GETKEY(ID_PHOTO_CAPTUREMODE), value);
			if((pItem->style & ITEM_TYPE_MASK) == ITEM_TYPE_NORMAL)
				break;
			if(!strcasecmp(GETVALUE(pItem->caption), value)) {
				scrollview_set_item_height_by_index(hScrollView, index, SETUP_ITEM_HEIGHT);
			} else {
				scrollview_set_item_height_by_index(hScrollView, index, 0);
				if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(pItem->itemId), (long)GETVALUE(STRING_OFF), TYPE_FROM_GUI))
					INFO("send config error! key: %s, value: %s\n", GETKEY(pItem->itemId), GETVALUE(STRING_OFF)); 
			}
            otherChanged = TRUE;
			break;
        default:
            break;
    }
    return otherChanged;
}

static void DoConfirmAction(HWND hDlg, HWND hScrollView, DIALOGITEM *pItem, int index)
{
    char value[VALUE_SIZE] = {0};
    switch(pItem->itemId) 
    {
        case ID_VIDEO_RESOLUTION:
        //case ID_SETUP_IMAGE_ROTATION:
            //SetCameraConfig(FALSE);
            break;
#ifdef GUI_ADAS_ENABLE
        case ID_VIDEO_ADAS_CALIBRATE:
            //ShowInfoDialog(hDlg, INFO_DIALOG_ADAS_CALIBRATE);
            break;
#endif
        case ID_VIDEO_RESET:
            ShowInfoDialog(hDlg, INFO_DIALOG_RESET_VIDEO);
            break;
        case ID_PHOTO_RESET:
            ShowInfoDialog(hDlg, INFO_DIALOG_RESET_PHOTO);
            break;
        //case ID_SETUP_VIDEO:
        //    ShowSetupDialog(hDlg, TYPE_VIDEO_SETTINGS);
        //    break;
        //case ID_SETUP_PHOTO:
        //    ShowSetupDialog(hDlg, TYPE_PHOTO_SETTINGS);
        //    break;
#ifdef GUI_WIFI_ENABLE
        case ID_SETUP_WIFI_MODE:
            {
                //GetConfigValue(GETKEY(ID_SETUP_WIFI_MODE), value);
                //UpdateItemStatus((DIALOGPARAM *)GetWindowAdditionalData(hDlg), ID_SETUP_WIFI_MODE, ITEM_STATUS_DISABLE, ITEM_STATUS_DISABLE);
                //PWifiOpt pWifi = (PWifiOpt) malloc(sizeof(WifiOpt));
                //if(pWifi == NULL) {
                //    ERROR("mallock PWifiOpt failed\n");
                //    break;
                //}
                //memset(pWifi, 0, sizeof(WifiOpt));
                //pWifi->operation = strcasecmp(GETVALUE(STRING_ON), value) ? DISABLE_WIFI : ENABLE_AP;
                //if(pWifi->operation == ENABLE_AP) {
                //    GetConfigValue(GETKEY(ID_WIFI_AP_SSID), pWifi->wifiInfo[0]);
                //    GetConfigValue(GETKEY(ID_WIFI_AP_PSWD), pWifi->wifiInfo[1]);
                //    if(!strcmp(pWifi->wifiInfo[0], DEFAULT) || strlen(pWifi->wifiInfo[1]) < 8) {
                //        memset(pWifi->wifiInfo[0], 0, 128);
                //        memset(pWifi->wifiInfo[1], 0, 128);
                //    }
                //}
                //SpvSetWifiStatus(pWifi);
                break;
            }
#endif
        case ID_SETUP_DATE_TIME:
			ShowSetInfoDialog(hDlg, INFO_DIALOG_SET_DATETIME);
            break;
        case ID_SETUP_DATE:
            //ShowSetInfoDialog(hDlg, INFO_DIALOG_SET_DATE);
            break;
        case ID_SETUP_TIME:
            //ShowSetInfoDialog(hDlg, INFO_DIALOG_SET_TIME);
            break;
        /*case ID_SETUP_AUTO_POWER_OFF:
            SetAutoPowerOffStatus();
            break;*/
        //case ID_SETUP_BEEP_SOUND:
        //    GetConfigValue(GETKEY(ID_SETUP_BEEP_SOUND), value);
        //    
        //    //set_volume(CODEC_VOLUME_MAX * atoi(value) / 100);
        //    //g_fucking_volume = atoi(value);
        //    break;
        case ID_SETUP_KEY_TONE:
            //SetKeyTone();
            break;
        case ID_SETUP_DELAYED_SHUTDOWN:
            //SetDelayedShutdwonStatus();
            break;
        //case ID_SETUP_DISPLAY:
        //    ShowSetupDialog(hDlg, TYPE_DISPLAY_SETTINGS);
        //    break;
        //case ID_DISPLAY_SLEEP:
        //    //SetDisplaySleepStatus();
        //    break;
        //case ID_DISPLAY_BRIGHTNESS:
        //    //SpvSetBacklight();
        //    break;
        //case ID_SETUP_PARKING_GUARD:
        //    //SpvSetParkingGuard();
        //    break;
        //case ID_SETUP_IR_LED:
        //    //SetIrLed();
        //    break;
        case ID_SETUP_FORMAT:
            //if(GetSdcardStatus() != SD_STATUS_UMOUNT) {
            ShowInfoDialog(hDlg, INFO_DIALOG_FORMAT);
            //} else {
            //    ShowToast(hDlg, TOAST_SD_NO, 100);
            //}
            break;
        case ID_SETUP_CAMERA_RESET:
            ShowInfoDialog(hDlg, INFO_DIALOG_RESET_CAMERA);
            //might need API to set the camera to default.
            break;
		case ID_SETUP_VERSION:
			ShowInfoDialog(hDlg, INFO_DIALOG_VERSION);
			break;
        //case ID_SETUP_PLATE_NUMBER:
        //    //ShowSetInfoDialog(hDlg, INFO_DIALOG_SET_PLATE);
        //    break;
    }
}

static void UpdateItemStatus(DIALOGPARAM *pParam, int itemId, int newStatus, int mask)
{
    int i = 0;
    DIALOGITEM *pItem;
    for(i = 0; i < pParam->itemCount; i ++) {
        pItem = pParam->pItems + i;
        if(pItem != NULL && pItem->itemId == itemId) {
            pItem->style &= ~mask;
            pItem->style |= newStatus & mask;
            break;
        }
    }
}

static void OnItemConfirmed(HWND hDlg, HWND hScrollView, DIALOGITEM *pItem, int index)
{
    int i;
    int style = pItem->style;
    BOOL refresh = FALSE;
    if(style & ITEM_STATUS_DISABLE) {
        INFO("item disable\n");
        return;
    }

    //general item comfirmed, such as header, more, switch
    //INFO("type: %d\n", (style & ITEM_TYPE_MASK));
    switch(style & ITEM_TYPE_MASK) {
        case ITEM_TYPE_HEADER:
            EndSetupDialog(hDlg, IDOK);
            return;
        case ITEM_TYPE_SWITCH:
        {
            pItem->style ^= ITEM_STATUS_OFF;//switch the status
            const char *value = (pItem->style & ITEM_STATUS_OFF) ? GETVALUE(STRING_OFF) : GETVALUE(STRING_ON);
			if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(pItem->itemId), (long)value, TYPE_FROM_GUI))
				INFO("SendMsgToServer error!  key:%s, value:%s\n", GETKEY(pItem->itemId), value);
            //SetConfigValue(GETKEY(pItem->itemId), value, TYPE_FROM_GUI);
            scrollview_refresh_item_by_index(hScrollView, index);
            refresh = TRUE;
            break;
        }
        case ITEM_TYPE_MORE:
            ChangeToNextValue(hScrollView, pItem, index);
            scrollview_refresh_item_by_index(hScrollView, index);
            refresh = TRUE;
            break;
		case ITEM_TYPE_SETUP_PARAM:
			ShowSetupDialogByDialogItem(hDlg, pItem);
			break;

    }
#ifdef GUI_CAMERA_REAR_ENABLE
    refresh |= SetOtherItems(hScrollView, pItem, index);
#endif
    /*switch(pItem->itemId)
    {
    }*/
    DoConfirmAction(hDlg, hScrollView, pItem, index);
    if(refresh)
        UpdateWindow(hScrollView, TRUE);
}

static void DrawSettingsItem(HWND hWnd, HSVITEM hsvi, HDC hdc, RECT *rcDraw)
{
    const WINDOW_ELEMENT_RENDERER* win_rdr;
    RECT focusRect = *rcDraw;
    DWORD main_color, brushColor;

    DIALOGITEM *pItem = (DIALOGITEM *) scrollview_get_item_adddata (hsvi);

    //INFO("drawitem, id: %d, style: %d, rcDraw(%d, %d)\n", pItem->itemId, pItem->style, RECTWP(rcDraw), RECTHP(rcDraw));

    if((pItem->style & ITEM_TYPE_MASK) == ITEM_TYPE_HEADER)
    {
        paint_header_item(hWnd, hdc, pItem, rcDraw);

        if (scrollview_is_item_hilight(hWnd, hsvi))
        {
            brushColor = GetBrushColor(hdc);
            SetBrushColor(hdc, PIXEL_red);
            FillBox(hdc, focusRect.right - RECTH(focusRect), focusRect.bottom - 6 * SCALE_FACTOR, RECTW(focusRect), 6 * SCALE_FACTOR);
            SetBrushColor(hdc, brushColor);
        }
        return;
    }

    win_rdr = GetWindowInfo(hWnd)->we_rdr;
    main_color = GetWindowElementAttr(hWnd, WE_MAINC_THREED_BODY);//0xD5D2CD;

    //win_rdr->draw_push_button(hWnd, hdc, rcDraw, main_color, 
    //        0xFFFFFFFF, 0);

    paint_content_focus(hWnd, hdc, hsvi, pItem, rcDraw);

    /*draw focus block*/
    //if (scrollview_is_item_hilight(hWnd, hsvi))
    //{
    //    brushColor = GetBrushColor(hdc);
    //    SetBrushColor(hdc, PIXEL_red);
    //    FillBox(hdc, focusRect.left, focusRect.top, 8 * SCALE_FACTOR, RECTH(focusRect));
    //    SetBrushColor(hdc, brushColor);
    //}
}

static int DialogBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    int i = 0;
    switch(message) {
        case MSG_INITDIALOG:
        {
            SVITEMINFO svii;
            DIALOGPARAM *pParam = (DIALOGPARAM *) lParam;
            SetWindowAdditionalData(hDlg, lParam);
            HWND hScrollView = GetDlgItem(hDlg, IDC_SPV_SCROLLVIEW);
#ifdef GUI_SPORTDV_ENABLE
            int fontsize = (LARGE_SCREEN) ? 28 : (16 * SCALE_FACTOR);
#else
            int fontsize = (LARGE_SCREEN) ? 28 : (16 * SCALE_FACTOR);
#endif
            int curSel = 0;
			char value[VALUE_SIZE] = {0};

        g_logfont = CreateLogFont("ttf", "simsun", "UTF-8",
                FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                fontsize, 0); 
        g_logfont_bold = CreateLogFont("ttf", "fb", "UTF-8",
                FONT_WEIGHT_DEMIBOLD, FONT_SLANT_ROMAN,
                FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                fontsize, 0);

            SendMessage(hScrollView, SVM_SETITEMDRAW, 0, (LPARAM)DrawSettingsItem);
			if(pParam->type == TYPE_SETUP_PARAM) {
                GetConfigValue(GETKEY(pParam->pItems[i].itemId), value);
			}

            for(i = 0; i < pParam->itemCount; i ++) {
                svii.nItemHeight = SETUP_ITEM_HEIGHT;
                svii.addData = (DWORD) &(pParam->pItems[i]);
                svii.nItem = i;
                SendMessage(hScrollView, SVM_ADDITEM, 0, (LPARAM)&svii);
                SendMessage(hScrollView, SVM_SETITEMADDDATA, i, (LPARAM)&(pParam->pItems[i]));
				if(!strcasecmp(GETVALUE(pParam->pItems[i].caption), value))
					curSel = i;
            }

				SendMessage(hScrollView, SVM_SETCURSEL, curSel, TRUE);
#ifdef GUI_CAMERA_REAR_ENABLE
            if(pParam->type == TYPE_VIDEO_SETTINGS) {
                UpdateItemStatus(pParam, ID_VIDEO_REAR_CAMERA, CAMERA_REAR_CONNECTED ? ~ITEM_STATUS_DISABLE : ITEM_STATUS_DISABLE, ITEM_STATUS_DISABLE);
                for(i = 0; i < pParam->itemCount; i ++) {//initialize the status
                    SetOtherItems(hScrollView, pParam->pItems + i, i);
                }
            }
#endif
            
            HDC dc = GetClientDC(hScrollView);
            SetWindowBkColor(hScrollView, RGBA2Pixel(dc, 0x00, 0x00, 0x00, 0xFF));
            SetFocusChild(hScrollView);
            ReleaseDC(dc);
            return 0;
        }
        case MSG_SETFOCUS:
            INFO("set focus, wParam: %d, lParam: %ld\n", wParam, lParam);
            break;
        case MSG_KILLFOCUS:
            INFO("kill focus, wParam: %d, lParam: %ld\n", wParam, lParam);
            break;
        case MSG_PAINT:
            INFO("MSG_PAINT\n");
			BOOL refresh = 0;
			DIALOGPARAM *pParam_p = (DIALOGPARAM *)GetWindowAdditionalData(hDlg);
			HWND hScrollView = GetDlgItem(hDlg, IDC_SPV_SCROLLVIEW);
			for(i = 0; i < pParam_p->itemCount; i ++) {
				refresh |= SetOtherItems(hScrollView, pParam_p->pItems + i, i);
			}
			if(refresh)
				UpdateWindow(hScrollView, TRUE);
            break;
        case MSG_MEDIA_UPDATED:
            PostMessage(GetHosting(hDlg), MSG_MEDIA_UPDATED, 0, 0);
            break;
        case MSG_KEYUP:
            INFO("OnKeyUp: %d\n", wParam);
			DIALOGPARAM *pParam = (DIALOGPARAM *)GetWindowAdditionalData(hDlg);
            switch(LOWORD(wParam)) {
                case SCANCODE_INFOTM_OK:
                {
                    HWND hScrollView = GetDlgItem(hDlg, IDC_SPV_SCROLLVIEW);
                    int sel = SendMessage (hScrollView, SVM_GETCURSEL, 0, 0);
                    DIALOGITEM *pItem = (DIALOGITEM * )SendMessage (hScrollView, SVM_GETITEMADDDATA, sel, 0);
            		if(pParam->type == TYPE_SETUP_PARAM) {
						if(pItem->itemId == ID_SETUP_LANGUAGE) {
							if(SetLanguageConfig(pItem->caption - STRING_ENGLISH)) //change STRING_ to LANGUAGE_TYPE
								INFO("set language error\n");
						} else {
							if(SendMsgToServer(MSG_CONFIG_CHANGED, (int)GETKEY(pItem->itemId), (long)GETVALUE(pItem->caption), TYPE_FROM_GUI))
								INFO("SendMsgToServer error!  key:%s, value:%s\n", GETKEY(pItem->itemId), GETVALUE(pItem->caption));
						}
                    	EndSetupDialog(hDlg, IDOK);
						if(pItem->caption == STRING_DIRECT)
							ShowInfoDialog(GetActiveWindow(), INFO_DIALOG_WIFI_ENABLED);
						break;
					}
                    OnItemConfirmed(hDlg, hScrollView, pItem, sel);
                    break;
                }
                case SCANCODE_INFOTM_MODE:
                {
                    EndSetupDialog(hDlg, IDOK);
					if(pParam->type == TYPE_SETUP_SETTINGS) {
						PostMessage(GetActiveWindow(), MSG_KEYUP, SCANCODE_INFOTM_MODE, 0);
						INFO("send keyup message: SCANCODE_INFOTM_MODE\n");
					}
                    break;
                }
				case SCANCODE_INFOTM_MENU:
#if !USE_SETTINGS_MODE
					if(pParam->type == TYPE_VIDEO_SETTINGS || pParam->type == TYPE_PHOTO_SETTINGS) {
						ShowSetupDialogByMode(hDlg, MODE_SETUP);
						INFO("show general setting dialog\n");
						break;
					}
						INFO("setting dialog already open\n");
#endif
						break;
            }
            break;
        case MSG_COMMAND:
        {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);
            switch(id) {
                case IDC_SPV_SCROLLVIEW:
                    if(code == SVN_CLICKED) {
                        HWND hScrollView = GetDlgItem(hDlg, IDC_SPV_SCROLLVIEW);
                        int sel = SendMessage (hScrollView, SVM_GETCURSEL, 0, 0);
                        DIALOGITEM *pItem = (DIALOGITEM * )SendMessage (hScrollView, SVM_GETITEMADDDATA, sel, 0);
                        OnItemConfirmed(hDlg, hScrollView, pItem, sel);
                    }
                    break;
                case IDOK:
                case IDCANCEL:
                    EndSetupDialog(hDlg, wParam);
                    break;
            }
            break;
        }
        case MSG_RESTORE_DEFAULT:
            //RestoreDefaultSettings(hDlg);
            break;
        case MSG_REAR_CAMERA:
        {
#ifdef GUI_CAMERA_REAR_ENABLE
            INFO("MSG_REAR_CAMERA, wParam: %d\n", wParam);
            DIALOGPARAM *pParam = (DIALOGPARAM *)GetWindowAdditionalData(hDlg);
            if(pParam->type == TYPE_VIDEO_SETTINGS) {
                HWND hScrollView = GetDlgItem(hDlg, IDC_SPV_SCROLLVIEW);
                UpdateItemStatus(pParam, ID_VIDEO_REAR_CAMERA, CAMERA_REAR_CONNECTED ? ~ITEM_STATUS_DISABLE : ITEM_STATUS_DISABLE, ITEM_STATUS_DISABLE);
                for(i = 0; i < pParam->itemCount; i ++) {//initialize the status
                    SetOtherItems(hScrollView, pParam->pItems + i, i);
                }
                UpdateWindow(hScrollView, TRUE);
                SendMessage(hScrollView, SVM_SHOWITEM, SendMessage (hScrollView, SVM_GETCURSEL, 0, 0), 0);
            }
#endif
            break;
        }
		case MSG_WIFI_STATUS:
			switch (wParam) {
				case WIFI_OPEN_AP:
					ShowInfoDialog(hDlg, INFO_DIALOG_WIFI_ENABLED);
					break;
				case WIFI_OFF:
					ShowInfoDialog(hDlg, INFO_DIALOG_WIFI_DISABLE);
					break;
				case WIFI_AP_ON:
					ShowInfoDialog(hDlg, INFO_DIALOG_WIFI_INFO);
					break;
				default:
					INFO("wifi status: %d\n", wParam);
					break;
			}
			break;
		case MSG_BATTERY_STATUS:
		case MSG_CHARGE_STATUS:
				ShowInfoDialog(hDlg, INFO_DIALOG_BATTERY_LOW);
			break;
        case MSG_SDCARD_MOUNT:
			INFO("MSG_SDCARD_MOUNT  wParam: %d, lParam: %d\n", wParam, lParam);
			DealSdEvent(hDlg, wParam);
			break;
        case MSG_ACTIVE:
            INFO("MSG_ACTIVE, %d\n", wParam);
            break;
        case MSG_CLOSE:
            INFO("setup dialog, MSG_CLOSE, cancle:%d\n", wParam);
            EndSetupDialog(hDlg, wParam);
            DestroyLogFont(g_logfont);
            DestroyLogFont(g_logfont_bold);
            g_logfont = NULL;
            g_logfont_bold = NULL;
            break;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

static InitDialogItemCurrentValue(DIALOGPARAM *pParam)
{
    if(!pParam || pParam->itemCount <= 0)
        return;

    INFO("InitDialogItemCurrentValue\n");

    int i = 0;
    char *key;
    char value[VALUE_SIZE] = {0};
    int ret;

    for(i = 0; i < pParam->itemCount; i ++) 
    {
        /*if(pParam->pItems[i].style & ITEM_TYPE_MASK != ITEM_TYPE_MORE)
          continue;*/
        switch(pParam->pItems[i].style & ITEM_TYPE_MASK)
        {
            case ITEM_TYPE_SWITCH:
                if(!GetConfigValue(GETKEY(pParam->pItems[i].itemId), value)) {
                    if(!strcasecmp(value, GETVALUE(STRING_OFF)))
                        pParam->pItems[i].style |= ITEM_STATUS_OFF;
                    else 
                        pParam->pItems[i].style &= ~ITEM_STATUS_OFF;
                }
                break;
            case ITEM_TYPE_MORE:
                if(pParam->pItems[i].itemId == ID_SETUP_LANGUAGE) {
                    pParam->pItems[i].currentValue = STRING_ENGLISH + GetLanguageType();
                } else {
                    if(!GetConfigValue(GETKEY(pParam->pItems[i].itemId), value)) {
                        pParam->pItems[i].currentValue = GetStringId(value);
                    }
#ifdef GUI_WIFI_ENABLE
                    if(pParam->pItems[i].itemId == ID_SETUP_WIFI_MODE) {
                        int status = SpvGetWifiStatus();
                        if(status == WIFI_AP_ON || status == WIFI_OFF || status == WIFI_STA_ON) {
                            pParam->pItems[i].style &= ~ITEM_STATUS_DISABLE;
                        } else {
                            pParam->pItems[i].style |= ITEM_STATUS_DISABLE;
                        }
                    }
#endif
                }
                break;
        }
    }
}


void ShowSetupDialog(HWND hWnd, DIALOG_SETUP_TYPE type)
{
    DIALOGPARAM param;
    param.type = type;

    switch (type) {
        case TYPE_VIDEO_SETTINGS:
            param.pItems = g_video_items;
            break;
        case TYPE_PHOTO_SETTINGS:
            param.pItems = g_photo_items;
            break;
        case TYPE_SETUP_SETTINGS:
            param.pItems = g_setup_items;
            break;
        //case TYPE_DISPLAY_SETTINGS:
        //    param.pItems = g_display_items;
        //    break;
        case TYPE_DATE_TIME_SETTINGS:
            param.pItems = g_date_time_items;
            break;
        default:
            break;
    }
    param.itemCount = GetSettingItemsCount(type);//sizeof(g_video_item) / sizeof(DIALOGITEM);

    InitDialogItemCurrentValue(&param);
	CTRLDATA CtrlList[] =
	{
	    {
	        SPV_CTRL_SCROLLVIEW,
	        WS_VISIBLE | WS_CHILD | WS_TABSTOP | SVS_LOOP,
	        0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
	        IDC_SPV_SCROLLVIEW,
	        "spvscrollview", 
	        0, 0
	    },
	};
	
	DLGTEMPLATE DlgInitProgress =
	{
	    WS_NONE,
	#if SECONDARYDC_ENABLED
	        WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT,
	#else
	        WS_EX_TRANSPARENT,
	#endif
	    0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
	    "Setup",
	    0, 0,
	    TABLESIZE(CtrlList), NULL,
	    0
	};

    DlgInitProgress.controls = CtrlList;
    DialogBoxIndirectParam(&DlgInitProgress, hWnd, DialogBoxProc, (DWORD) &param);
}

void ShowSetupDialogByDialogItem(HWND hWnd, DIALOGITEM *pItem)
{
	DIALOGPARAM param;
	DIALOGITEM item[pItem->vc];
	int i = 0;

	param.type = TYPE_SETUP_PARAM;
	for(i = 0; i < pItem->vc; i++) {
		item[i].itemId = pItem->itemId;
		item[i].style = ITEM_TYPE_NORMAL;
		item[i].caption = pItem->valuelist[i];
		item[i].currentValue = 0;
		item[i].vc = 0;
	}
	param.pItems = item;
	param.itemCount = pItem->vc;
CTRLDATA CtrlList[] =
{
    {
        SPV_CTRL_SCROLLVIEW,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | SVS_LOOP,
        0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
        IDC_SPV_SCROLLVIEW,
        "spvscrollview", 
        0, 0
    },
};

DLGTEMPLATE DlgInitProgress =
{
    WS_NONE,
#if SECONDARYDC_ENABLED
        WS_EX_AUTOSECONDARYDC | WS_EX_TRANSPARENT,
#else
        WS_EX_TRANSPARENT,
#endif
    0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
    "Setup",
    0, 0,
    TABLESIZE(CtrlList), NULL,
    0
};

    DlgInitProgress.controls = CtrlList;
    DialogBoxIndirectParam(&DlgInitProgress, hWnd, DialogBoxProc, (DWORD) &param);
}

void ShowSetupDialogByMode(HWND hWnd, SPV_MODE mode)
{
    DIALOG_SETUP_TYPE type;
    switch(mode) {
        case MODE_VIDEO:
            type = TYPE_VIDEO_SETTINGS;//TYPE_VIDEO_SETTINGS;
            break;
        case MODE_PHOTO:
            type = TYPE_PHOTO_SETTINGS;//TYPE_PHOTO_SETTINGS;
            break;
        case MODE_SETUP:
            type = TYPE_SETUP_SETTINGS;
            break;
        default:
            ERROR("mode not found: %d\n", mode);
            return;
    }
    ShowSetupDialog(hWnd, type);
}

