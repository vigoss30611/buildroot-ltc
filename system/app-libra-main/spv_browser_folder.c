/*
** $Id: static.c 13736 2011-03-04 06:57:33Z wanzheng $
**
** static.c: the Static Control module.
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** All rights reserved by Feynman Software.
** 
** Current maitainer: Cui Wei
**
** Create date: 1999/5/22
*/

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "ctrl/ctrlhelper.h"
#include "ctrl/static.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "spv_debug.h"
#include "spv_common.h"
#include "spv_browser_folder.h"
#include "spv_language_res.h"

static BITMAP g_default_thumb;
static BITMAP exitBmp;

static void DrawFolder(HWND hWnd, PCONTROL pCtrl, HDC hdc)
{
    DWORD dwStyle;
    RECT rcClient;
    PBITMAP pThumb;
    BOOL focused;
    HDC mem_dc;
    DWORD oldBrushColor;
    int typeIcon = IC_VIDEO_FOLDER;
    int x, y;
    int bmWidth, bmHeight;
    SIZE size;

    dwStyle = pCtrl->dwStyle;//GetWindowStyle(hWnd);
    GetClientRect(hWnd, &rcClient);

    focused = dwStyle & FOLDER_FOCUS ? TRUE : FALSE;
    oldBrushColor = GetBrushColor(hdc);
    SetBkMode(hdc, BM_TRANSPARENT);

    //style
    BITMAP typeBmp;
    switch(dwStyle & FOLDER_TYPE_MASK) {
        case FOLDER_EXIT:
        {
            //draw exit icon
            if(g_icon[IC_EXIT].bmWidth == 0) {
                LoadBitmap(HDC_SCREEN, &g_icon[IC_EXIT], SPV_ICON[IC_EXIT]);
            }
            bmWidth = g_icon[IC_EXIT].bmWidth * SCALE_FACTOR;
            bmHeight = g_icon[IC_EXIT].bmHeight * SCALE_FACTOR;

            SelectFont (hdc, GetWindowFont(hWnd));
            GetTextExtent(hdc, GETSTRING(STRING_EXIT), -1, &size);

            x = rcClient.left + ((RECTW(rcClient) - bmWidth) >> 1);
            y = rcClient.top + ((RECTH(rcClient) - bmHeight - size.cy) >> 1);
            FillBoxWithBitmap(hdc, x, y, bmWidth, bmHeight, &g_icon[IC_EXIT]);

            //draw exit text
            SetTextColor(hdc, PIXEL_lightwhite);
            rcClient.top = y + bmHeight + 2 * SCALE_FACTOR;
            DrawText(hdc, GETSTRING(STRING_EXIT), -1, &rcClient, DT_TOP | DT_CENTER | DT_SINGLELINE);

            //draw focus
            if(focused) {
                SetBrushColor(hdc, PIXEL_red);
                FillBox(hdc, rcClient.left, rcClient.bottom - 6 * SCALE_FACTOR, RECTW(rcClient), 6 * SCALE_FACTOR);
            }
            return;
        }
        case FOLDER_VIDEO:
            typeIcon = IC_VIDEO_FOLDER;
            break;
        case FOLDER_VIDEO_LOCKED:
            typeIcon = IC_VIDEO_LOCKED_FOLDER;
            break;
/*        case FOLDER_TIME_LAPSE:
            typeIcon = IC_TIME_LAPSE_FOLDER;
            break;
        case FOLDER_SLOW_MOTION:
            typeIcon = IC_SLOW_MOTION_FOLDER;
            break;
        case FOLDER_CAR:
            typeIcon = IC_CAR_FOLDER;
            break;*/
        case FOLDER_PHOTO:
            typeIcon = IC_PHOTO_FOLDER;
            break;
/*        case FOLDER_CONTINUOUS:
            typeIcon = IC_PHOTO_CONTINUOUS_FOLDER;
            break;
        case FOLDER_NIGHT:
            typeIcon = IC_PHOTO_NIGHT_FOLDER;
            break;*/
    }
    //draw thumbnail
    if(pCtrl->dwAddData2) {
        pThumb = (PBITMAP) pCtrl->dwAddData2;
    } else {
        if(g_default_thumb.bmWidth == 0) {
            LoadBitmap(hdc, &g_default_thumb, SPV_ICON[IC_DEFAULT_THUMB]);
        }
        pThumb = &g_default_thumb;
    }

#if 0//COLORKEY_ENABLED
    FillBoxWithBitmap(hdc, rcClient.left, rcClient.top, RECTW(rcClient), RECTH(rcClient), pThumb);
#else
    FillBoxWithBitmap(hdc, rcClient.left, rcClient.top, RECTW(rcClient), RECTH(rcClient) - FOLDER_INFO_HEIGHT, pThumb);
#endif

    //draw title
#if 0//COLORKEY_ENABLED
    mem_dc = CreateMemDC (RECTW(rcClient), FOLDER_INFO_HEIGHT, 16, MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA,
            0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F);
    if(focused) {
        SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0xFF, 0x00, 0x00, 0xB0));
    } else {
        SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0x00, 0x00, 0x00, 0xB0));
    }
    FillBox(mem_dc, 0, 0, RECTW(rcClient) - 2, FOLDER_INFO_HEIGHT);
    BitBlt(mem_dc, 0, 0, RECTW(rcClient), FOLDER_INFO_HEIGHT, hdc, 0, RECTH(rcClient) - FOLDER_INFO_HEIGHT, 0); 
    DeleteMemDC(mem_dc);
#else
    if(focused) {
        SetBrushColor(hdc, PIXEL_red);
    } else {
        SetBrushColor(hdc, PIXEL_black);
    }
    FillBox(hdc, rcClient.left, rcClient.top + RECTH(rcClient) - FOLDER_INFO_HEIGHT, RECTW(rcClient) - 2, FOLDER_INFO_HEIGHT);
#endif

    //draw style icon
    if(g_icon[typeIcon].bmWidth == 0) {
        LoadBitmap(hdc, &g_icon[typeIcon], SPV_ICON[typeIcon]);
        g_icon[typeIcon].bmType = BMP_TYPE_COLORKEY;
        g_icon[typeIcon].bmColorKey = PIXEL_black;
    }
    bmWidth = g_icon[typeIcon].bmWidth * SCALE_FACTOR;
    bmHeight = g_icon[typeIcon].bmHeight * SCALE_FACTOR;
    int iconPadding = (FOLDER_INFO_HEIGHT - bmHeight) / 2;
    FillBoxWithBitmap(hdc, rcClient.left, rcClient.top + RECTH(rcClient) - FOLDER_INFO_HEIGHT + iconPadding, bmWidth, bmHeight, &g_icon[typeIcon]);
/*
    if((dwStyle & FOLDER_TYPE_MASK) == FOLDER_PHOTO) {
        typeIcon = IC_PHOTO_NIGHT_FOLDER;
        //draw night icon
        if(g_icon[typeIcon].bmWidth == 0) {
            LoadBitmap(hdc, &g_icon[typeIcon], SPV_ICON[typeIcon]);
            g_icon[typeIcon].bmType = BMP_TYPE_COLORKEY;
            g_icon[typeIcon].bmColorKey = PIXEL_black;
        }
        bmWidth = g_icon[typeIcon].bmWidth * SCALE_FACTOR;
        bmHeight = g_icon[typeIcon].bmHeight * SCALE_FACTOR;
        FillBoxWithBitmap(hdc, rcClient.left + bmWidth, rcClient.top + RECTH(rcClient) - FOLDER_INFO_HEIGHT, bmWidth, bmHeight, &g_icon[typeIcon]);
    }
*/
}

static int StaticControlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    RECT        rcClient;
    HDC         hdc;
    const char* spCaption;
    PCONTROL    pCtrl;
    UINT        uFormat = DT_TOP;
    DWORD       dwStyle;         
    BOOL        needShowCaption = TRUE;
    
    pCtrl = gui_Control (hwnd); 
    switch (message) {
        case MSG_CREATE:
            INFO("create control, %s\n", GetWindowCaption(hwnd));
            pCtrl->dwAddData2 = pCtrl->dwAddData;
            SetWindowBkColor (hwnd, GetWindowElementPixel (hwnd,
                                                WE_MAINC_THREED_BODY));
            pCtrl->iBkColor = PIXEL_lightgray;
            return 0;
            
        case STM_GETIMAGE:
            return (int)(pCtrl->dwAddData2); 
        
        case STM_SETIMAGE:
        {
            int pOldValue;
            
            pOldValue  = (int)(pCtrl->dwAddData2);
            pCtrl->dwAddData2 = (DWORD)wParam;
            InvalidateRect (hwnd, NULL, 
                    (GetWindowStyle (hwnd) & SS_TYPEMASK) == SS_ICON);
            return pOldValue;
        }

        case MSG_KILLFOCUS:
            pCtrl->dwStyle &= (~FOLDER_FOCUS);
            InvalidateRect (hwnd, NULL, TRUE);
            break;

        case MSG_SETFOCUS:
            SendMessage(GetParent(hwnd), MSG_FOCUS_FOLDER, (pCtrl->dwStyle) & FOLDER_TYPE_MASK, 0);

            pCtrl->dwStyle |= FOLDER_FOCUS;
            InvalidateRect (hwnd, NULL, TRUE);
            break;
        case MSG_SET_FOLDER_TYPE:
            pCtrl->dwStyle &= (~FOLDER_TYPE_MASK);
            pCtrl->dwStyle |= wParam;
            break;

        case MSG_GETDLGCODE:
            return DLGC_STATIC;

        case MSG_PAINT:
            INFO("paint control, %s\n", GetWindowCaption(hwnd));
            hdc = BeginPaint(hwnd);
            DrawFolder(hwnd, pCtrl, hdc);
            EndPaint(hwnd, hdc);
            return 0;

        case MSG_LBUTTONDBLCLK:
            if (GetWindowStyle (hwnd) & SS_NOTIFY)
                NotifyParent (hwnd, pCtrl->id, STN_DBLCLK);
            break;

        case MSG_LBUTTONDOWN:
            if (GetWindowStyle (hwnd) & SS_NOTIFY)
                NotifyParent (hwnd, pCtrl->id, STN_CLICKED);
            break;

        case MSG_NCLBUTTONDBLCLK:
            break;

        case MSG_NCLBUTTONDOWN:
            break;

        case MSG_HITTEST:
            dwStyle = GetWindowStyle (hwnd);
            if ((dwStyle & SS_TYPEMASK) == SS_GROUPBOX)
                return HT_TRANSPARENT;

            if (GetWindowStyle (hwnd) & SS_NOTIFY)
                return HT_CLIENT;
            else
                return HT_OUT;
        break;

        case MSG_FONTCHANGED:
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;
            
        case MSG_SETTEXT:
            SetWindowCaption (hwnd, (char*)lParam);
            InvalidateRect (hwnd, NULL, TRUE);
            return 0;
            
        default:
            break;
    }

    return DefaultControlProc (hwnd, message, wParam, lParam);
}

BOOL RegisterSPVFolderControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = SPV_CTRL_FOLDER;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL,WE_MAINC_THREED_BODY);
    WndClass.WinProc     = StaticControlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

BOOL UnregisterSPVFolerControl(void)
{
    return UnregisterWindowClass(SPV_CTRL_FOLDER);
}
