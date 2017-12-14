#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<minigui/common.h>
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>

/*#include "ctrl/ctrlhelper.h"
#include "ctrl/static.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"*/

#include"spv_common.h"
#include"spv_static.h"
#include"spv_language_res.h"
#include"spv_toast.h"
#include"spv_utils.h"
#include"spv_debug.h"

#define TIMER_DESTROY 201
#define IDC_BEGIN 200

static int TOAST_WIDTH;
static int TOAST_HEIGHT;
static int TOAST_PADDING;
static int BORDER_WIDTH;

static PLOGFONT g_info_font = NULL;

static HWND oldToastWnd = HWND_INVALID;

typedef struct {
    TOAST_TYPE type;
    int showTime;
    DWORD infoData;
} ToastInfo;

typedef ToastInfo * PToastInfo;

static void DrawToastContent(HWND hwnd, HDC hdc, PToastInfo info){
    if(info == NULL) {
        ERROR("info == NULL\n");
        return;
    }

    char cttStr[128] = {0};
    RECT windowRect;
    SIZE strSize;
    RECT strRect;
    int hasIcon = -1; 

    BITMAP icon;
    int x = 0, y = 0, w = 0, h = 0;
    int bmWidth, bmHeight;
    
    int type  = info->type;

    GetClientRect(hwnd, &windowRect);
    strRect = windowRect;

    switch(type){
        case TOAST_SD_ERROR:
            strcpy(cttStr, GETSTRING(STRING_SD_ERROR));
            hasIcon = 1;
            break;
        case TOAST_SD_EMPTY:
            strcpy(cttStr, GETSTRING(STRING_SD_EMPTY));
            hasIcon = 1;
            break;
        case TOAST_SD_FULL:
            strcpy(cttStr, GETSTRING(STRING_SD_FULL));
            hasIcon = 1;
            break;
        case TOAST_SD_NO:
            strcpy(cttStr, GETSTRING(STRING_SD_NO));
            hasIcon = 1;
            break;
        case TOAST_SD_FORMAT:
            strcpy(cttStr, GETSTRING(STRING_SD_ERROR_FORMAT));
            hasIcon = 1;
            break;
        case TOAST_SD_PREPARE:
            strcpy(cttStr, GETSTRING(STRING_SD_PREPARE));
            hasIcon = 1;
            break;
        case TOAST_CAMERA_BUSY:
            hasIcon = 1;
            strcpy(cttStr, GETSTRING(STRING_CAMERA_BUSY));
            break;
        case TOAST_PLAYBACK_ERROR:
            hasIcon = 0;
            strcpy(cttStr, GETSTRING(STRING_PLAYBACK_ERROR));
            break;
    }

    //draw retangle
    SetBrushColor(hdc, PIXEL_black);
    Rectangle(hdc, windowRect.left, windowRect.top, windowRect.right - 1, windowRect.bottom - 1);
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, windowRect.left + BORDER_WIDTH, windowRect.top + BORDER_WIDTH, 
            windowRect.right - 2 * BORDER_WIDTH, windowRect.bottom - 2 * BORDER_WIDTH);

    SelectFont(hdc, g_info_font);
    GetTextExtent(hdc, cttStr, -1, &strSize);
    
    if(1 == hasIcon)
    {
        // define bitmap
        if(type == TOAST_CAMERA_BUSY) {
            LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_CAMERA_BUSY]);
        } else {
            LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[IC_CARD]);
        }

        icon.bmType = BMP_TYPE_COLORKEY;
        icon.bmColorKey = PIXEL_black;

        bmWidth = icon.bmWidth * SCALE_FACTOR;
        bmHeight = icon.bmHeight * SCALE_FACTOR;

        x = (RECTW(windowRect) - (bmWidth + strSize.cx + TOAST_PADDING)) >> 1;
        y = (RECTH(windowRect) - bmHeight) >> 1;
        if(x < 0) {
            x = 0;
        }
        if(y < 0) {
            y = 0;
        }

        SetTextColor(hdc, PIXEL_lightwhite);
        SetBkColor(hdc, PIXEL_black);
        SetBkMode (hdc, BM_TRANSPARENT);

        //draw icon
        FillBoxWithBitmap(hdc, x, y, bmWidth, bmHeight, &icon);

        //draw string
        strRect.left = x + bmWidth + TOAST_PADDING;
        DrawText(hdc, cttStr, -1, &strRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        UnloadBitmap(&icon);
    }
    else if(0 == hasIcon){
        SetTextColor(hdc, PIXEL_lightwhite);
        SetBkMode (hdc, BM_TRANSPARENT);
        DrawText(hdc, cttStr, -1, &strRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    else {
        ERROR("error, no such type!\n");
    }
}

static void OnKeyUp(HWND hwnd, int keyCode, PToastInfo pInfo)
{
    switch(pInfo->type){
        case TOAST_SD_NO:
        case TOAST_SD_FULL:
        case TOAST_SD_EMPTY:
        case TOAST_SD_ERROR:
        {
            switch(keyCode) {
                case SCANCODE_SPV_MODE:
                case SCANCODE_SPV_OK:
//                    EndToast(hwnd, IDOK);
                    break;
            }
        }
    }
}

static int 
ToastControlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    RECT        rcClient;
    HDC         hdc;
    const char* spCaption;
    //PCONTROL    pCtrl;
    UINT        uFormat = DT_TOP;
    DWORD       dwStyle;         
    BOOL        needShowCaption = TRUE;

    PToastInfo pInfo = (PToastInfo)GetWindowAdditionalData(hwnd);
    
    //pCtrl = gui_Control (hwnd); 
    switch (message) {
        case MSG_CREATE:
            if(pInfo != NULL && pInfo->showTime > 0) {
                SetTimer(hwnd, TIMER_DESTROY, pInfo->showTime);
            }
            //pCtrl->dwAddData2 = pCtrl->dwAddData;
            SetWindowBkColor (hwnd, PIXEL_lightwhite);
            return 0;
            
        case MSG_GETDLGCODE:
            return DLGC_STATIC;

        case MSG_PAINT:
            hdc = BeginPaint(hwnd);
            GetClientRect(hwnd, &rcClient);
#if 0
            SetBrushColor (hdc, RGBA2Pixel (hdc, 0x00, 0x00, 0x00, 0x0));
            FillBox (hdc, 0, 0, , VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
            SetMemDCAlpha (hdc, MEMDC_FLAG_SRCPIXELALPHA, 0);
            SetBkMode (hdc, BM_TRANSPARENT);
#endif
            DrawToastContent(hwnd, hdc, pInfo);

            EndPaint (hwnd, hdc);
            return 0;
        case MSG_TIMER:
            if(wParam == TIMER_DESTROY) {
                KillTimer(hwnd, TIMER_DESTROY);
                HWND parent = GetParent(hwnd);
                HideToast(hwnd);
                INFO("timer_destroy\n");
                SendMessage(parent, MSG_CLEAN_SECONDARY_DC, 0, 0);
            }
            return 0;
        case MSG_FONTCHANGED:
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;
        case MSG_DESTROY:
            INFO("toat window destroy, windowId: 0x%x\n", hwnd);
            if(pInfo != NULL) {
                free(pInfo);
                pInfo = NULL;
                SetWindowAdditionalData(hwnd, 0);
            }
            if(oldToastWnd == hwnd) {
                oldToastWnd = HWND_INVALID;
            }
            break;
        default:
            break;
    }

    return DefaultControlProc (hwnd, message, wParam, lParam);
}

BOOL RegisterToastControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = SPV_CTRL_TOAST;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = PIXEL_black;
    WndClass.WinProc     = ToastControlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

static void MeasureWindowSize()
{
    TOAST_WIDTH = 300 * SCALE_FACTOR;
    TOAST_HEIGHT = 96 * SCALE_FACTOR;
    TOAST_PADDING = 10 * SCALE_FACTOR;
    BORDER_WIDTH = 4 * SCALE_FACTOR;
}


HWND ShowToast(HWND hWnd, TOAST_TYPE type, int duration)
{
    static BOOL sToastRegistered;
    if(!sToastRegistered) {
        RegisterToastControl();
        sToastRegistered = TRUE;
    }

    INFO("ShowToast, type: %d\n", type);

    PToastInfo info = (PToastInfo)malloc(sizeof(ToastInfo));
    memset(info, 0, sizeof(ToastInfo));
    info->type = type;
    info->showTime = duration;

    if(g_info_font == NULL) {
        g_info_font = CreateLogFont("ttf", "simsun", "UTF-8",
                FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                LARGE_SCREEN ? 24 : 18 * SCALE_FACTOR, 0);
    }

    MeasureWindowSize();

    HWND toastWindow = CreateWindowEx(SPV_CTRL_TOAST, "Toast",
            WS_NONE, WS_EX_NONE, IDC_BEGIN + type,
            (DEVICE_WIDTH - TOAST_WIDTH) >> 1, (DEVICE_HEIGHT - TOAST_HEIGHT) >> 1,
            TOAST_WIDTH, TOAST_HEIGHT, hWnd, (DWORD)info);

    ShowWindow(toastWindow, SW_SHOWNORMAL);
    //makesure there is only one toast in window
    if(oldToastWnd == toastWindow) {
        INFO("the new created window's address is same as the old window, why?\n");
    } else {
        if(IsWindow(oldToastWnd)) {
            const char *caption = GetWindowCaption(oldToastWnd);
            if(caption != NULL && !strcmp(caption, "Toast")) {
                INFO("DestroyWindow, 0x%x\n", oldToastWnd);
                DestroyWindow(oldToastWnd);
            }
        }
        oldToastWnd = toastWindow;
    }
    LightLcdStatus();
    SpvPlayAudio(SPV_WAV[WAV_ERROR]);
    return toastWindow;
}

void HideToast(HWND hWnd) {
    PToastInfo info = (PToastInfo)GetWindowAdditionalData(hWnd);
    if(info != NULL)
        free(info);
    info = NULL;

    SetWindowAdditionalData(hWnd, 0);

    INFO("DestroyWindow, 0x%x\n", hWnd);
    if(hWnd == oldToastWnd) {
        oldToastWnd = HWND_INVALID;
    }
    DestroyWindow(hWnd);
}
