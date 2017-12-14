#include<stdio.h>
#include<string.h>

#include<minigui/common.h>
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>

#include "spv_common.h"
#include "spv_static.h"
#include "spv_browser_folder.h"
#include "spv_folder_dialog.h"
#include "spv_setup_dialog.h"
#include "spv_utils.h"
#include "spv_language_res.h"
#include "spv_playback_dialog.h"
#include "spv_toast.h"
#include "spv_debug.h"

#define IDC_FILE_COUNT      9527 
#define IDC_FOLDER_EXIT     (IDC_FILE_COUNT + 1)
#define IDC_VIDEO           (IDC_FOLDER_EXIT + 1)
#define IDC_VIDEO_LOCKED    (IDC_VIDEO + 1)
#define IDC_PHOTO           (IDC_VIDEO_LOCKED + 1)


typedef struct {
    int cursel;
    int folderCount;
    BOOL destroyed;
} FolderDialogInfo;

typedef FolderDialogInfo* PFolderDialogInfo;

static PLOGFONT g_title_info_font;

static BITMAP g_default_thumb;

static FileList local_media_list[MEDIA_COUNT];

static HWND g_toast_wnd = HWND_INVALID;

static void InitPlayFolder(HWND hDlg, BOOL needSync);


static MEDIA_TYPE GetFolderType(int index) {
    int i;
    int ctrlcount = 0;

    if(index <= 0) {
        return MEDIA_UNKNOWN;
    }

    for(i = 0; i < MEDIA_COUNT; i ++) {
        if(local_media_list[i].fileCount > 0) {
            ctrlcount ++;
        }
        if(ctrlcount == index) {
            return i;
        }
    }
    return MEDIA_UNKNOWN;
}

static int GetFocusInfo(int curSel, char *focusInfo) {
    int type = GetFolderType(curSel);

    if(type > MEDIA_UNKNOWN && type < MEDIA_COUNT) {
        int stringId = local_media_list[type].fileCount > 1 ? STRING_FILES : STRING_FILE;
        sprintf(focusInfo, "%d %s", local_media_list[type].fileCount, GETSTRING(stringId));
        return 0;
    }

    strcpy(focusInfo, GETSTRING(STRING_PLAYBACK));
    return 0;
}

static void FocusNextChild(HWND hDlg, PFolderDialogInfo pInfo, BOOL bPrevious)
{
    pInfo->cursel += bPrevious ? -1 : 1;
    pInfo->cursel = (pInfo->cursel + pInfo->folderCount + 1) % (pInfo->folderCount + 1);
    InvalidateRect(hDlg, 0, TRUE);
}

static void EndFolderDialog(HWND hDlg, DWORD endCode)
{
    int i = 0;
    BOOL fileDeleted = FALSE;
    for(i = 0; i < MEDIA_COUNT; i ++) {
        if(local_media_list[i].fileCount != g_media_files[i].fileCount)
            fileDeleted = TRUE;

        FreeFileList(&local_media_list[i]);
    }
    if(fileDeleted) {
        INFO("file has been changed, rescan media files\n");
        GoingToScanMedias(GetMainWindow(), 100);
    }
    
    PFolderDialogInfo info = (PFolderDialogInfo) GetWindowAdditionalData(hDlg);
    info->destroyed = TRUE;

    if(IDCANCEL != endCode) {// here we want to change to setup mode
#if USE_SETTINGS_MODE
        SendMessage(GetHosting(hDlg), MSG_CHANGE_MODE, MODE_SETUP, 0);
#else
        SendMessage(GetMainWindow(), MSG_CHANGE_MODE, MODE_VIDEO, (GetMainWindow() == GetHosting(hDlg)));
#endif
    }

    UnregisterSPVFolerControl();
    EndDialog(hDlg, endCode);
}

static DrawFolderItem(HWND hDlg, HDC hdc, PFolderDialogInfo pInfo, int index)
{
    int x, y, bmWidth, bmHeight;
    x = ((index - 1) % FOLDER_COLUMN) * FOLDER_WIDTH;
    y = FOLDER_DIALOG_TITLE_HEIGHT + (index - 1) / FOLDER_COLUMN * FOLDER_HEIGHT;

    //draw bg photo
    if(g_default_thumb.bmWidth == 0) {
        LoadBitmap(hdc, &g_default_thumb, SPV_ICON[IC_DEFAULT_THUMB]);
    }
    FillBoxWithBitmap(hdc, x, y, FOLDER_WIDTH, FOLDER_HEIGHT - FOLDER_INFO_HEIGHT, &g_default_thumb);

    y += FOLDER_HEIGHT - FOLDER_INFO_HEIGHT;
    if(pInfo->cursel == index) {
        SetBrushColor(hdc, PIXEL_red);
    } else {
        SetBrushColor(hdc, PIXEL_black);
    }
    FillBox(hdc, x, y, FOLDER_WIDTH - 2, FOLDER_INFO_HEIGHT);
    SetBrushColor(hdc, PIXEL_lightwhite);
    FillBox(hdc, x + FOLDER_WIDTH - 2, y, 2, FOLDER_INFO_HEIGHT);

    MEDIA_TYPE type = GetFolderType(index);
    int typeIcon = IC_VIDEO_FOLDER;
    switch(type) {
        case MEDIA_VIDEO:
            typeIcon = IC_VIDEO_FOLDER;
            break;
        case MEDIA_VIDEO_LOCKED:
            typeIcon = IC_VIDEO_LOCKED_FOLDER;
            break;
        case MEDIA_PHOTO:
            typeIcon = IC_PHOTO_FOLDER;
            break;
    }

    //draw style icon
    if(g_icon[typeIcon].bmWidth == 0) {
        LoadGlobalIcon(typeIcon);
    }   
    bmWidth = g_icon[typeIcon].bmWidth;
    bmHeight = g_icon[typeIcon].bmHeight;
    int iconPadding = (FOLDER_INFO_HEIGHT - bmHeight) / 2;
    FillBoxWithBitmap(hdc, x, y + iconPadding, bmWidth, bmHeight, &g_icon[typeIcon]);
}

static DrawFolderDialog(HWND hDlg, HDC hdc, PFolderDialogInfo pInfo)
{
    SIZE size;
    RECT rect;
    char focusString[64] = {0};
    int bmWidth, bmHeight;
    int width, height;
    int x, y, i;

    //draw playback icon
    if(g_icon[IC_PLAYBACK_FOLDER].bmWidth == 0) {
        LoadBitmap(HDC_SCREEN, &g_icon[IC_PLAYBACK_FOLDER], SPV_ICON[IC_PLAYBACK_FOLDER]);
    }
    x = 0;
    y = (FOLDER_DIALOG_TITLE_HEIGHT - g_icon[IC_PLAYBACK_FOLDER].bmHeight) >> 1;
    FillBoxWithBitmap(hdc, x, y, g_icon[IC_PLAYBACK_FOLDER].bmWidth, g_icon[IC_PLAYBACK_FOLDER].bmHeight, &g_icon[IC_PLAYBACK_FOLDER]);

    SelectFont (hdc, g_title_info_font);
    SetBkColor(hdc, PIXEL_black);
    SetTextColor(hdc, PIXEL_lightwhite);
    
    //draw focus string
    GetFocusInfo(pInfo->cursel, focusString);
    rect.left = x + g_icon[IC_PLAYBACK_FOLDER].bmWidth + 3;
    rect.top = 0;
    rect.right = rect.left + 200;
    rect.bottom = FOLDER_DIALOG_TITLE_HEIGHT;
    DrawText(hdc, focusString, -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

    //draw divider
    x = VIEWPORT_WIDTH - FOLDER_DIALOG_TITLE_HEIGHT - 2;
    y = 10;
    SetBrushColor(hdc, PIXEL_red);
    FillBox(hdc, x, y, 2, FOLDER_DIALOG_TITLE_HEIGHT - 20);

    //draw exit icon
    if(g_icon[IC_EXIT].bmWidth == 0) {
        LoadBitmap(HDC_SCREEN, &g_icon[IC_EXIT], SPV_ICON[IC_EXIT]);
    }   
    bmWidth = g_icon[IC_EXIT].bmWidth;
    bmHeight = g_icon[IC_EXIT].bmHeight;

    GetTextExtent(hdc, GETSTRING(STRING_EXIT), -1, &size);

    x = VIEWPORT_WIDTH - FOLDER_DIALOG_TITLE_HEIGHT + ((FOLDER_DIALOG_TITLE_HEIGHT - bmWidth) >> 1); 
    y = (FOLDER_DIALOG_TITLE_HEIGHT - bmHeight - size.cy / SCALE_FACTOR) >> 1; 
    FillBoxWithBitmap(hdc, x, y, bmWidth, bmHeight, &g_icon[IC_EXIT]);

    //draw exit text
    rect.left = VIEWPORT_WIDTH - FOLDER_DIALOG_TITLE_HEIGHT;
    rect.top = y + bmHeight + 2;
    rect.right = VIEWPORT_WIDTH;
    rect.bottom = FOLDER_DIALOG_TITLE_HEIGHT;
    DrawText(hdc, GETSTRING(STRING_EXIT), -1, &rect, DT_TOP | DT_CENTER | DT_SINGLELINE);

    //draw focus
    if(pInfo->cursel == 0) {
        SetBrushColor(hdc, PIXEL_red);
        FillBox(hdc, rect.left, rect.bottom - 6, FOLDER_DIALOG_TITLE_HEIGHT, 6);
    }

    //draw folder items
    for(i = 1; i <= pInfo->folderCount; i ++) {
        DrawFolderItem(hDlg, hdc, pInfo, i);
    }

}

static int FolderDialogBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    PFolderDialogInfo pInfo = (PFolderDialogInfo) GetWindowAdditionalData(hDlg);
    if(pInfo != NULL && pInfo->destroyed) {//add this to ignore the message while EndFolderDialog has been called
        return DefaultDialogProc(hDlg, message, wParam, lParam);
    }

    switch(message) {
        case MSG_INITDIALOG:
            {
                int fontsize = LARGE_SCREEN ? 28 : (16 * SCALE_FACTOR);
                g_title_info_font = CreateLogFont("ttf", "fb", "UTF-8",
                    FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                    FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    fontsize, 0);
            int i;
            SetWindowAdditionalData(hDlg, lParam);
            SetWindowFont(hDlg, g_title_info_font);
            SetWindowBkColor(hDlg, COLOR_black);
            InitPlayFolder(hDlg, TRUE);
            return 0;
            }
        case MSG_INIT_FOLDERS:
            if(!IsSdcardMounted()) {
                g_toast_wnd = ShowSDErrorToast(hDlg);
            }
            InitPlayFolder(hDlg, (BOOL)wParam);
            InvalidateRect(hDlg, 0, TRUE);
            return 0;
        case MSG_PAINT:
            {
                INFO("MSG_PAINT\n");
                HDC hdc = BeginSpvPaint(hDlg);
                DrawFolderDialog(hDlg, hdc, pInfo);
                EndPaint(hDlg, hdc);
            break;
            }
        case MSG_COMMAND:
            switch(wParam) {
                case IDOK:
                case IDCANCEL:
                    EndFolderDialog(hDlg, wParam);
                    break;
            }
            break;
        case MSG_MEDIA_SCAN_FINISHED:
            if(!IsSdcardMounted()) {
                int i = 0, count = 0;
                for(i = 0; i < MEDIA_COUNT; i ++) {
                    count += local_media_list[i].fileCount;
                }
                if(count == 0)// here we do not need to resync the media files
                    return 0;
            }
            SendNotifyMessage(hDlg, MSG_INIT_FOLDERS, TRUE, 0);
            return 0;
        case MSG_FOCUS_FOLDER:
            {
                char fileInfo[32] = {0};
                if(wParam == FOLDER_EXIT) {
                    strcpy(fileInfo, GETSTRING(STRING_PLAYBACK));
                } else {
                    int fileCount = local_media_list[wParam - FOLDER_VIDEO].fileCount;
                    int stringId = fileCount > 1 ? STRING_FILES : STRING_FILE;
                    sprintf(fileInfo, "%d %s", fileCount, GETSTRING(stringId));
                }
                SetWindowText(GetDlgItem(hDlg, IDC_FILE_COUNT), fileInfo); 
            break;
            }
        case MSG_CHANGE_MODE:
            EndFolderDialog(hDlg, IDOK);
            break;
        case MSG_ACTIVE:
            INFO("MSG_ACTIVE, %d\n",wParam);
            //if(wParam == 1) {
            //    PostMessage(hDlg, MSG_INIT_FOLDERS, FALSE, 0);
            //}
            break;
        case MSG_KEYUP:
            INFO("OnKeyUp: %d\n", wParam);
            switch(wParam) {
                case SCANCODE_SPV_MENU:
#if !USE_SETTINGS_MODE
                    //ShowSetupDialog(hDlg, TYPE_SETUP_SETTINGS);
#endif
                    break;
                case SCANCODE_SPV_DOWN:
                    FocusNextChild(hDlg, pInfo, FALSE);
                    break;
                case SCANCODE_SPV_UP:
                    FocusNextChild(hDlg, pInfo, TRUE);
                    break;
                case SCANCODE_SPV_MODE:
                    EndFolderDialog(hDlg, IDOK);
                    break;
                case SCANCODE_SPV_OK:
                    {
                        if(pInfo->cursel == 0) {
                            EndFolderDialog(hDlg, IDOK);
                        } else {
                            int type = GetFolderType(pInfo->cursel);
                            if(type > MEDIA_UNKNOWN && type < MEDIA_COUNT)
                                ShowPlaybackDialog(hDlg, &local_media_list[type]);
                        }
                        break;
                    }
            }
            break;
        case MSG_CLOSE:
            INFO("MSG_CLOSE\n");
            DestroyLogFont(g_title_info_font);
            EndFolderDialog(hDlg, wParam);
            UnloadBitmap(&g_default_thumb);
            g_default_thumb.bmWidth = 0;
            break;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

static void InitPlayFolder(HWND hDlg, BOOL needSync)
{
    int i;
    int ctrlCount = 0;
    int filecount = 0;

    PFolderDialogInfo pInfo = (PFolderDialogInfo) GetWindowAdditionalData(hDlg);
    if(pInfo == NULL)
        return;

    if(needSync) {
        if(!IsSdcardMounted()) {
            for(i = 0; i < MEDIA_COUNT; i ++) {
                local_media_list[i].fileType = (MEDIA_TYPE)i;
                FreeFileList(&local_media_list[i]);
            }
        } else {
            SyncMediaList(local_media_list);
        }
    }

    for(i = 0; i < MEDIA_COUNT; i ++)
    {
        if(local_media_list[i].fileCount > 0) {//only show the folder that contains media files
            INFO("file found: %d, type: %d\n", local_media_list[i].fileCount, local_media_list[i].fileType);
            ctrlCount ++;
        }
        filecount += local_media_list[i].fileCount;
    }

    pInfo->folderCount = ctrlCount;

    if(filecount > 0) {
        if(g_toast_wnd != HWND_INVALID) {
            HideToast(g_toast_wnd);
            g_toast_wnd = HWND_INVALID;
        }
        if(pInfo->cursel > ctrlCount) {
            pInfo->cursel = ctrlCount;
        } else if(pInfo->cursel < 1) {
            pInfo->cursel = 1;
        }
    } else {
        pInfo->cursel = 0;
        if(IsSdcardMounted() && ImmediatlyMediaFilesCount() <= 0) {
            g_toast_wnd = ShowToast(hDlg, TOAST_SD_EMPTY, 160);
        }
    }
}

int ShowFolderDialog(HWND hWnd, DWORD dwAddData)
{
    int i = 0;
    INFO("Folder dialog begin\n");
    for(i = 0; i < MEDIA_COUNT; i ++) {
        local_media_list[i].fileType = (MEDIA_TYPE)i;
    }
    //RegisterSPVFolderControl();

    DLGTEMPLATE FolderDlg =
    {
        WS_NONE,
#if SECONDARYDC_ENABLED
        WS_EX_AUTOSECONDARYDC,
#else
        WS_EX_NONE,
#endif
        0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
        "Folder",
        0, 0,
        0, NULL,
        0
    };

    CTRLDATA FolderCtrl[] = 
    {
        /*{
            SPV_CTRL_STATIC,
            WS_VISIBLE | WS_CHILD | SS_CENTER,
            MW_ICON_WIDTH * SCALE_FACTOR, 0, 200 * SCALE_FACTOR, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR,
            IDC_FILE_COUNT,
            "Playback",
            0, WS_EX_TRANSPARENT
        },
        {
            SPV_CTRL_FOLDER,
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | FOLDER_EXIT,
            DEVICE_WIDTH - FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR, 0, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR,
            IDC_FOLDER_EXIT,
            "Exit",
            0, WS_EX_TRANSPARENT
        },
        {
            SPV_CTRL_FOLDER,
            WS_CHILD | WS_TABSTOP | FOLDER_VIDEO,
            0, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR, FOLDER_WIDTH, FOLDER_HEIGHT,
            IDC_VIDEO,
            "Video",
            0, 0
        },
        {
            SPV_CTRL_FOLDER,
            WS_CHILD | WS_TABSTOP | FOLDER_VIDEO_LOCKED,
            FOLDER_WIDTH, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR, FOLDER_WIDTH, FOLDER_HEIGHT,
            IDC_VIDEO_LOCKED,
            "Video Locked",
            0, 0
        },
        {
            SPV_CTRL_FOLDER,
            WS_CHILD | WS_TABSTOP | FOLDER_PHOTO,
            FOLDER_WIDTH*2, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR, FOLDER_WIDTH, FOLDER_HEIGHT,
            IDC_PHOTO,
            "Photo",
            0, 0
        },
            {
              SPV_CTRL_FOLDER,
              WS_VISIBLE | WS_CHILD | WS_TABSTOP | FOLDER_SINGLE,
              0, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR + FOLDER_HEIGHT, FOLDER_WIDTH, FOLDER_HEIGHT,
              IDC_SINGLE,
              "Single",
              0, 0
              },
              {
              SPV_CTRL_FOLDER,
              WS_VISIBLE | WS_CHILD | WS_TABSTOP | FOLDER_CONTINUOUS,
              FOLDER_WIDTH * 1, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR + FOLDER_HEIGHT, FOLDER_WIDTH, FOLDER_HEIGHT,
              IDC_CONTINUOUS,
              "Continuous",
              0, 0
              },
              {
              SPV_CTRL_FOLDER,
              WS_VISIBLE | WS_CHILD | WS_TABSTOP | FOLDER_NIGHT,
        //FOLDER_WIDTH*2, FOLDER_DIALOG_TITLE_HEIGHT + FOLDER_HEIGHT, FOLDER_WIDTH, FOLDER_HEIGHT,
        0, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR + FOLDER_HEIGHT*2, FOLDER_WIDTH, FOLDER_HEIGHT,
        IDC_NIGHT,
        "Night",
        0, 0
        },
        {
        SPV_CTRL_FOLDER,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | FOLDER_CAR,
        //FOLDER_WIDTH*3, FOLDER_DIALOG_TITLE_HEIGHT, FOLDER_WIDTH, FOLDER_HEIGHT,
        FOLDER_WIDTH * 2, FOLDER_DIALOG_TITLE_HEIGHT * SCALE_FACTOR + FOLDER_HEIGHT, FOLDER_WIDTH, FOLDER_HEIGHT,
        IDC_CAR,
        "Car Video",
        0, 0
        },*/
    };

    PFolderDialogInfo pInfo = (PFolderDialogInfo) malloc(sizeof(FolderDialogInfo));
    if(pInfo == NULL) {
        ERROR("malloc PFolderDialogInfo failed\n");
        return -1;
    }
    memset(pInfo, 0, sizeof(FolderDialogInfo));

    FolderDlg.controls = FolderCtrl;
    FolderDlg.controlnr = GET_ARRAY_LENGTH(FolderCtrl);
    DialogBoxIndirectParam(&FolderDlg, hWnd, FolderDialogBoxProc, (LPARAM)pInfo);

    free(pInfo);
    pInfo = NULL;
    INFO("Folder dialog end\n");
    return 0;
}

