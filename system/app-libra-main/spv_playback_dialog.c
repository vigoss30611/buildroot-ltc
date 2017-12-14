#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<math.h>

#include<minigui/common.h>
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>

#include "spv_common.h"
#include "spv_toast.h"
#include "spv_static.h"
#include "spv_info_dialog.h"
#include "spv_app.h"
#include "spv_utils.h"
#include "media_play.h"
#include "spv_playback_dialog.h"
#include "spv_debug.h"

#define TIMER_HIDE_STATUSBAR 300
#define TIMER_VIDEO_TIME     TIMER_HIDE_STATUSBAR + 1
#define TIMER_BATTERY  TIMER_VIDEO_TIME + 1

#define MSG_PLAYBACK_COMPLETION MSG_USER+201
#define MSG_PLAYBACK_ERROR MSG_USER+202
#define MSG_SHOW_PREVIEW MSG_USER+203

#define LOOP_BROWSE   1
#define LOOP_PLAYBACK 0

//#define FLAG_COMPLITION 1

static BOOL g_force_clean_dc;

static HWND g_playback_window = HWND_INVALID;

static PLOGFONT g_info_font;

typedef struct {
    media_play* player;
    FileList *pFileList;
    unsigned int fileIndex;
    FileNode *pNode;
    unsigned int focusIndex;
    BOOL isBarHiding;
    BOOL isPlaying;
    BOOL isStarted;
    unsigned int duration; //video time duration
    unsigned int position; //video playing position
    unsigned int progressWidth;
    float speed;
    unsigned int battery;
    BOOL isCharging;
    BOOL batteryVisible;
    BOOL shadowEffect;//use black background to cover the fb0
} PlaybackInfo;

typedef PlaybackInfo* PPlaybackInfo;


const static int g_video_view_icons[] = {IC_PREV, IC_PLAY, IC_NEXT, IC_DELETE, IC_GRID};
const static int g_video_playing_icons[] = {IC_REWIND, IC_PAUSE, IC_FAST_FORWARD, IC_VOLUME};
const static int g_photo_view_icons[] = {IC_PREV, IC_NEXT, IC_DELETE, IC_GRID};

static RECT g_time_rect;
static RECT g_progress_rect;
static RECT g_focus_rect;
static RECT g_bottom_rect;
static RECT g_battery_rect;
static RECT g_speed_rect;

static CTRLDATA PlaybackCtrl[] = 
{

};


static int ChangeToNextMedia(HWND hDlg, PPlaybackInfo pInfo, BOOL showThumb, int manual);
static void DeleteCurrentMedia(HWND hDlg, PPlaybackInfo pInfo);
static int PlayVideo(HWND hDlg, PPlaybackInfo pInfo, float speed);
static int PauseVideo(HWND hDlg, PPlaybackInfo pInfo);
static int StopVideo(HWND hDlg, PPlaybackInfo pInfo);
static int UpdateVideoSpeed(HWND hDlg, PPlaybackInfo pInfo, float speed);
static void ShowStatusbar(HWND hDlg, PPlaybackInfo pInfo);
static void GoingToHideStatusbar(HWND hDlg, PPlaybackInfo pInfo, BOOL reset);

static void EndPlaybackDialog(HWND hDlg, DWORD endCode)
{
    PPlaybackInfo pInfo = (PPlaybackInfo)GetWindowAdditionalData(hDlg);
    if(pInfo != NULL && pInfo->player != NULL) {
        StopVideo(hDlg, pInfo);
        //if(pInfo->isPlaying) {
        //    pInfo->player->end_video();
        //}
        media_play_destroy(pInfo->player);
    }
    KillTimer(hDlg, TIMER_BATTERY);
    KillTimer(hDlg, TIMER_VIDEO_TIME);
    KillTimer(hDlg, TIMER_HIDE_STATUSBAR);
    SendNotifyMessage(GetHosting(hDlg), MSG_INIT_FOLDERS, FALSE, 0);
    EndDialog(hDlg, endCode);
}

static int IsPhotoView(PPlaybackInfo pInfo)
{
    if(pInfo != NULL && pInfo->pFileList != NULL)
        return MEDIA_PHOTO == pInfo->pFileList->fileType;// || type == MEDIA_CONTINUOUS || type == MEDIA_NIGHT;

    return FALSE;//default video
}

static int GetThumbnailPath(PFileNode pNode, char *thumbpath)
{
    char *suffix = NULL;
    char fileName[128] = {0};

    if(thumbpath == NULL || pNode == NULL || (suffix = strstr(pNode->fileName, ".mkv")) == NULL) {
        ERROR("error path to get thumbpath\n");
        return -1;
    }

    if(!strncasecmp(pNode->fileName, "LOCK_", 5)) {
        strncpy(fileName, pNode->fileName + 5, suffix - pNode->fileName - 5);
    } else {
        strncpy(fileName, pNode->fileName, suffix - pNode->fileName);
    }
    int nameLength = strlen(fileName);
    /*if(fileName[nameLength - 1] == 's' || fileName[nameLength - 1] == 'S') {//for locked video
        fileName[nameLength - 1] = 0;
    }*/
    sprintf(thumbpath, "%s/%s/.%s.jpg", EXTERNAL_MEDIA_DIR, pNode->folderName, fileName);
    
    INFO("video thumbnail path---: %s\n", thumbpath);

    if(access(thumbpath, F_OK) != 0) {
        ERROR("No thumb found, %s\n", thumbpath);
        return -1;
    }

    return 0;
}

static int ShowVideoThumbnail(PPlaybackInfo pInfo, char *path)
{
    char thumbpath[128] = {0};

    if(GetThumbnailPath(pInfo->pNode, thumbpath))
        return -1;

    pInfo->player->set_file_path(thumbpath);

    return pInfo->player->display_photo();
}

static int ShowPreviewInfo(HWND hDlg, PPlaybackInfo pInfo, BOOL showThumb)
{
    int ret = 0;
    if(pInfo == NULL || pInfo->pNode == NULL || pInfo->player == NULL) {
        ERROR("ShowPreviewInfo, pInfo == NULL\n");
        return -1;
    }

    char filePath[256] = {0};

    sprintf(filePath, "%s/%s/%s", EXTERNAL_MEDIA_DIR, pInfo->pNode->folderName, pInfo->pNode->fileName);
    INFO("filepath: %s\n", filePath);

    if(access(filePath, F_OK)) {
        ERROR("File not exist, show next media info\n");
        DeleteCurrentMedia(hDlg, pInfo);
        return;
    }

    if(IsPhotoView(pInfo)) {
        pInfo->player->set_file_path(filePath);
        ret = pInfo->player->display_photo();
        pInfo->shadowEffect = FALSE;
        InvalidateRect(hDlg, 0, TRUE);
    } else {
        StopVideo(hDlg, pInfo);
        if(showThumb) {
            if(ShowVideoThumbnail(pInfo, filePath)) {
                pInfo->player->set_file_path(filePath);
                pInfo->player->prepare_video();
            }
            pInfo->shadowEffect = FALSE;
            InvalidateRect(hDlg, 0, TRUE);
        }
        pInfo->player->set_file_path(filePath);
        pInfo->position = 0;
        int duration;
#if USE_THIRD_LIB_CALCULATE_DUEATION
        duration = SpvGetVideoDuration(filePath);
#else
        duration = pInfo->player->get_video_duration();
#endif
        if(duration <= 0) duration = 0;

#if IGNORE_INVALID_VIDEO
        if(pInfo->pFileList->fileCount > 1 && duration == 0) {
            DeleteCurrentMedia(hDlg, pInfo);
            //ChangeToNextMedia(hDlg, pInfo, TRUE);
            return 0;
        }
#endif
        //ret = pInfo->player->get_video_thumbnail();
        pInfo->duration = duration;
        INFO("prepare video, duration: %d\n", pInfo->duration);
        if(pInfo->duration <= 0) {
            SendNotifyMessage(hDlg, MSG_PLAYBACK_ERROR, 0, 0);
            return -1;
        }
    }

    return ret;
}

static int OnVideoCallback(int flag)
{
    INFO("OnVideoCallback, flag: %d\n", flag);
    switch(flag) {
        case MSG_PLAY_STATE_END:
            if(g_playback_window != HWND_INVALID) {
                SendNotifyMessage(g_playback_window, MSG_PLAYBACK_COMPLETION, 0, 0);
            }
            break;
        case MSG_PLAY_STATE_ERROR:
            if(g_playback_window != HWND_INVALID) {
                SendNotifyMessage(g_playback_window, MSG_PLAYBACK_ERROR, 0, 0);
            }
            break;
    }
    return 0;
}

static int GetInfoString(PPlaybackInfo pInfo, char *info)
{
    if(info == NULL)
        return -1;

    if(IsPhotoView(pInfo)) {
        sprintf(info, "%d / %d", pInfo->fileIndex, pInfo->pFileList->fileCount);
    } else {
        char durationString[32] = {0};
        char progressString[32] = {0};

        TimeToString(pInfo->position, progressString);
        TimeToString(pInfo->duration, durationString);
        sprintf(info, "%s|%s", progressString, durationString);
    }

    return 0;
}

static void DrawTopPannel(HWND hDlg, HDC hdc, PPlaybackInfo pInfo)
{
    int x, y;
    RECT nameRect, infoRect, speedRect;
    HDC mem_dc;
    char infoString[64] = {0}, speedString[16] = {0};

    SetBkMode(hdc, BM_TRANSPARENT);
#if 0
    /*mem_dc = CreateMemDC (VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 16, MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA,
            0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F);
    SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0x00, 0x00, 0x00, 0x60));
    FillBox(mem_dc, 0, 0, VIEWPORT_WIDTH, PB_BAR_HEIGHT * SCALE_FACTOR);
    BitBlt(mem_dc, 0, 0, VIEWPORT_WIDTH, PB_BAR_HEIGHT * SCALE_FACTOR, hdc, 0, 0, 0);
    DeleteMemDC(mem_dc);*/
#else
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, PB_BAR_HEIGHT);
#endif

    //draw type icon
    int typeIcon = IC_VIDEO;
    MEDIA_TYPE type = pInfo->pFileList->fileType;
    switch(type) {
        case MEDIA_VIDEO:
        case MEDIA_VIDEO_LOCKED:
            typeIcon = IC_VIDEO;
            break;
/*        case MEDIA_TIME_LAPSE:
            typeIcon = IC_TIME_LAPSE;
            break;
        case MEDIA_SLOW_MOTION:
            typeIcon = IC_SLOW_MOTION;
            break;
        case MEDIA_CAR:
            typeIcon = IC_CAR;
            break;*/
        case MEDIA_PHOTO:
            typeIcon = IC_PHOTO;
            break;
/*        case MEDIA_CONTINUOUS:
            typeIcon = IC_PHOTO_CONTINUOUS;
            break;*/
    }
    LoadGlobalIcon(typeIcon);
    x = 0;
    y = PB_ICON_PADDING;
    FillBoxWithBitmap(hdc, x, y, PB_ICON_WIDTH, PB_ICON_HEIGHT, &g_icon[typeIcon]);

    SelectFont(hdc, g_info_font);
    SetBkColor(hdc, PIXEL_black);
    SetTextColor(hdc, PIXEL_lightwhite);
    //draw file name
    nameRect.left = PB_ICON_WIDTH;
    nameRect.top = 0;
    nameRect.right = VIEWPORT_WIDTH - PB_ICON_WIDTH;
    nameRect.bottom = (PB_BAR_HEIGHT - PB_PROGRESS_HEIGHT) / 2;
    DrawText(hdc, pInfo->pNode->fileName, -1, &nameRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

    //draw info text
    infoRect.left = PB_ICON_WIDTH;
    infoRect.top = PB_BAR_HEIGHT / 2;
    infoRect.right = VIEWPORT_WIDTH - PB_ICON_WIDTH;
    infoRect.bottom = PB_BAR_HEIGHT - PB_PROGRESS_HEIGHT;
    GetInfoString(pInfo, infoString);
    DrawText(hdc, infoString, -1, &infoRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

    if(pInfo->speed != 1.0f && !IsPhotoView(pInfo)) {
        //draw speed text
        speedRect.left = g_speed_rect.left / SCALE_FACTOR;
        speedRect.top = g_speed_rect.top / SCALE_FACTOR;
        speedRect.right = g_speed_rect.right / SCALE_FACTOR;
        speedRect.bottom = g_speed_rect.bottom / SCALE_FACTOR;
        sprintf(speedString, "%dX", (int)(pInfo->speed));
        DrawText(hdc, speedString, -1, &speedRect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
    }

    if(pInfo->batteryVisible) {
        //draw battery
        int btIcon = GetBatteryIcon();
        LoadGlobalIcon(btIcon);
        FillBoxWithBitmap(hdc, VIEWPORT_WIDTH - PB_ICON_WIDTH, y, PB_ICON_WIDTH, PB_ICON_HEIGHT, &g_icon[btIcon]);
    }

    //draw progress
    if(pInfo->progressWidth > 0 && !IsPhotoView(pInfo)) {
        SetBrushColor(hdc, PIXEL_red);
        FillBox(hdc, 0, PB_BAR_HEIGHT - PB_PROGRESS_HEIGHT, pInfo->progressWidth, PB_PROGRESS_HEIGHT);
    }
}

static const int *GetBottomItems(PPlaybackInfo pInfo, int *count)
{
    const int *pItems = NULL;
    if(IsPhotoView(pInfo)) { // photo view 4 icons
        *count = GET_ARRAY_LENGTH(g_photo_view_icons);
        pItems = g_photo_view_icons;
    } else if(pInfo->isPlaying) {// video playing 4 icons
        *count = GET_ARRAY_LENGTH(g_video_playing_icons) - 1;
        pItems = g_video_playing_icons;
    } else {// video view 5 icons
        *count = GET_ARRAY_LENGTH(g_video_view_icons);
        pItems = g_video_view_icons;
    }

    return pItems;
}

static void DrawBottomPannel(HWND hDlg, HDC hdc, PPlaybackInfo pInfo)
{
    int i = 0;
    int iconGap = 0;
    int iconCount = 0;
    const int *pIcons = NULL;
    int x = 0, y = 0;
    
#if 0//COLORKEY_ENABLED1
/*    mem_dc = CreateMemDC (VIEWPORT_WIDTH, VIEWPORT_HEIGHT, 16, MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA,
            0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F);
    SetBrushColor (mem_dc, RGBA2Pixel (mem_dc, 0x00, 0x00, 0x00, 0x60));
    FillBox(mem_dc, 0, 0, VIEWPORT_WIDTH, PB_BAR_HEIGHT);
    BitBlt(mem_dc, 0, 0, VIEWPORT_WIDTH, PB_BAR_HEIGHT, hdc, 0, VIEWPORT_HEIGHT - PB_BAR_HEIGHT, 0);
    DeleteMemDC(mem_dc);*/
#else
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, VIEWPORT_HEIGHT - PB_BAR_HEIGHT, VIEWPORT_WIDTH, PB_BAR_HEIGHT);
#endif

    pIcons = GetBottomItems(pInfo, &iconCount);
    if(!IsPhotoView(pInfo) && pInfo->isPlaying) {
        iconGap = (VIEWPORT_WIDTH - (iconCount + 1) * PB_ICON_WIDTH) / (iconCount + 1);
    } else {
        iconGap = (VIEWPORT_WIDTH - iconCount * PB_ICON_WIDTH) / iconCount;
    }

    x = iconGap >> 1;
    y = VIEWPORT_HEIGHT - (PB_BAR_HEIGHT - PB_ICON_PADDING);
    for(i = 0; i < iconCount; i ++) //draw bottom panel icons
    {
        LoadGlobalIcon(*pIcons);
        FillBoxWithBitmap(hdc, x + i * (PB_ICON_WIDTH + iconGap), y, PB_ICON_WIDTH, PB_ICON_HEIGHT, &g_icon[*pIcons]);
        pIcons ++;
    }

    //draw focus
    pInfo->focusIndex = (pInfo->focusIndex % iconCount + iconCount) % iconCount;
    SetBrushColor(hdc, PIXEL_red);
    FillBox(hdc, x + (pInfo->focusIndex) * (PB_ICON_WIDTH + iconGap) + (PB_ICON_WIDTH - PB_FOCUS_WIDTH) / 2, VIEWPORT_HEIGHT - PB_FOCUS_HEIGHT, PB_FOCUS_WIDTH, PB_FOCUS_HEIGHT);
}

static void DrawPlaybackDialog(HWND hDlg, HDC hdc, PPlaybackInfo pInfo)
{
    if(pInfo == NULL || pInfo->pFileList == NULL) {
        ERROR("pInfo == NULL\n");
        return;
    }
#if COLORKEY_ENABLED || defined(UBUNTU)
    if(pInfo->shadowEffect) {
        SetBrushColor(hdc, PIXEL_black);
    } else {
        SetBrushColor(hdc, SPV_COLOR_TRANSPARENT);
    }
    FillBox (hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
#else
    SetBrushColor (hdc, RGBA2Pixel (hdc, 0x01, 0x01, 0x01, 0x0));
    FillBox (hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
    SetMemDCAlpha (hdc, MEMDC_FLAG_SRCPIXELALPHA,0);
    SetBkMode (hdc, BM_TRANSPARENT);
#endif

    if(pInfo->isBarHiding)
    {//hiding status bar
        return;
    }

    DrawTopPannel(hDlg, hdc, pInfo);
    DrawBottomPannel(hDlg, hdc, pInfo);
}

static void TimerProc(HWND hDlg, PPlaybackInfo pInfo, int timerId)
{
    switch(timerId) {
        case TIMER_HIDE_STATUSBAR:
            if(pInfo->isBarHiding)
                return;
            INFO("TIMER_HIDE_STATUSBAR\n");
            g_force_clean_dc = TRUE;
            pInfo->isBarHiding = TRUE;
            InvalidateRect(hDlg, 0, TRUE);
            break;
        case TIMER_VIDEO_TIME:
            {
                int oldposition = pInfo->position;
                pInfo->position = (unsigned int)(pInfo->player->get_video_position());
                if(pInfo->duration == 0) {
                    INFO("duration is 0\n");
                    InvalidateRect(hDlg, &g_time_rect, TRUE);
                    return;
                }
                if(pInfo->position == pInfo->duration) {
                    INFO("position == duration\n");
#ifdef UBUNTU
                    SendNotifyMessage(hDlg, MSG_PLAYBACK_COMPLETION, 0, 0);
                    return;
#endif
                }
                int width = pInfo->position * VIEWPORT_WIDTH / pInfo->duration;
                if(width != pInfo->progressWidth) {
                    pInfo->progressWidth = width;
                    InvalidateRect(hDlg, &g_progress_rect, TRUE);
                }
                if(oldposition != pInfo->position) {
                    INFO("position: %d\n", pInfo->position);
                    InvalidateRect(hDlg, &g_time_rect, TRUE);
                }
                break;
            }
    }
}

PB_BUTTON GetButtonID(PPlaybackInfo pInfo)
{
//    INFO("getbuttonid:%s\n", pInfo->pNode->fileName);
    if(IsPhotoView(pInfo)) {
        return pInfo->focusIndex + BTP_PREV;
    } else if(pInfo->isPlaying) {
        return pInfo->focusIndex + BTV_REWIND;
    } else {
        return pInfo->focusIndex + BTV_PREV;
    }
}

static PFileNode GetPrevFileNode(PPlaybackInfo pInfo)
{
    int i = 0;
    PFileNode pCur = pInfo->pFileList->pHead;
    PFileNode pPrev = pInfo->pFileList->pHead;

    if(pCur == pInfo->pNode) {
        while(pCur->next != NULL) {
            pCur = pCur->next;
        }
        pPrev = pCur;
    } else {
        while(pCur != pInfo->pNode) {
            pPrev = pCur;
            pCur = pCur->next;
            if(pCur == NULL)
                break;
        }
    }

    return pPrev;
}

static int ChangeToPrevMedia(HWND hDlg, PPlaybackInfo pInfo, BOOL showThumb, BOOL manual)
{
    //INFO("change to prev media: %s\n", pInfo->pNode->fileName);
    if(pInfo->pFileList->fileCount <= 1)
        return -1;

    int index = pInfo->fileIndex - 1;
    if(index < 1)
        index = pInfo->pFileList->fileCount;
    if(pInfo->pNode->prev != NULL) {
        pInfo->pNode = pInfo->pNode->prev;//GetPrevFileNode(pInfo);
    } else {
#if LOOP_PLAYBACK || LOOP_BROWSE
        if(manual) {
            PFileNode ptmp = pInfo->pFileList->pHead;
            while(ptmp->next) {
                ptmp = ptmp->next;
            }//find tail
            pInfo->pNode = ptmp;
        } else {
            return -1;
        }
#else
        return -1;
#endif
    }
    pInfo->fileIndex = index;

    pInfo->position = 0;
    pInfo->duration = 0;
    pInfo->progressWidth = 0;

    ShowPreviewInfo(hDlg, pInfo, showThumb);
    //InvalidateRect(hDlg, &g_time_rect, TRUE);
    //InvalidateRect(hDlg, &g_progress_rect, TRUE);
    InvalidateRect(hDlg, 0, TRUE);
    return 0;
}

static int ChangeToNextMedia(HWND hDlg, PPlaybackInfo pInfo, BOOL showThumb, BOOL manual)
{
    //INFO("change to next media: %s\n", pInfo->pNode->fileName);
    if(pInfo->pFileList->fileCount <= 1)
        return -1;

    int index = pInfo->fileIndex + 1;
    if(index > pInfo->pFileList->fileCount)
        index = 1;

    if(pInfo->pNode->next != NULL) {
        pInfo->pNode = pInfo->pNode->next;
    } else {
#if LOOP_PLAYBACK || LOOP_BROWSE
        if(manual) {
            pInfo->pNode = pInfo->pFileList->pHead;
        } else {
            return -1;
        }
#else
        return -1;
#endif
    }

    pInfo->position = 0;
    pInfo->duration = 0;
    pInfo->progressWidth = 0;

    pInfo->fileIndex = index;

    ShowPreviewInfo(hDlg, pInfo, showThumb);
    //InvalidateRect(hDlg, &g_time_rect, TRUE);
    //InvalidateRect(hDlg, &g_progress_rect, TRUE);
    InvalidateRect(hDlg, 0, TRUE);

    return 0;
}

static int DeleteMediaFile(HWND hDlg, PPlaybackInfo pInfo)
{
    if(pInfo == NULL || pInfo->pNode == NULL)
        return -1;

    PFileNode pNode = pInfo->pNode;
    char filepath[128] = {0};
    sprintf(filepath, "%s/%s/%s", EXTERNAL_MEDIA_DIR, pNode->folderName, pNode->fileName);

    int ret = unlink(filepath);
    INFO("unlink file: %s, ret: %d\n", filepath, ret);
    if(ret) {
        perror("unlink file failed, ERRNO");
        if(!access(filepath, F_OK))
            return -1;
    } else {
        if(!access(filepath, F_OK)) {
            ERROR("unlink success, but file is still exist\n");
            return -1;
        } else {
            INFO("unlink media file succeed, file path: %s\n", filepath);
        }
    }
    
    if(!IsPhotoView(pInfo)) {
        //del the thumbnail
        char thumbpath[128] = {0};
        if(!GetThumbnailPath(pNode, thumbpath)) {
            if(!unlink(thumbpath)) {
                INFO("unlink thumbnail succeed,  thumbnail path: %s\n", thumbpath);
            }
        }
    }
    
    return ret;
}

static void DeleteCurrentMedia(HWND hDlg, PPlaybackInfo pInfo)
{
    if(pInfo == NULL)
        return;

    PFileNode pCur = pInfo->pNode;
    if(pCur == NULL || pInfo->pFileList == NULL)
        return;

    PFileNode pPrev = NULL;

    DeleteMediaFile(hDlg, pInfo);

    if(pInfo->pFileList->fileCount <= 1) {
        pInfo->pFileList->pHead = NULL;
        pInfo->pFileList->fileCount = 0;
        pInfo->pNode = NULL;
        free(pCur);
        EndPlaybackDialog(hDlg, IDOK);
        return;
    }

    //pFileList->fileCount --;
    if(pInfo->pNode->prev == NULL || pInfo->fileIndex == 1) {//head
        pInfo->pNode = pInfo->pNode->next;
        pInfo->pNode->prev = NULL;
        pInfo->pFileList->pHead = pInfo->pNode;
    } else if(pInfo->pNode->next == NULL || pInfo->fileIndex == pInfo->pFileList->fileCount) {//tail
        pPrev = pInfo->pNode->prev;//GetPrevFileNode(pInfo);
        pPrev->next = NULL;
        pInfo->fileIndex --;
        pInfo->pNode = pPrev;//pInfo->pFileList->pHead;
    } else {
        pPrev = pInfo->pNode->prev;//GetPrevFileNode(pInfo);
        pInfo->pNode = pInfo->pNode->next;
        pPrev->next = pInfo->pNode;
        pInfo->pNode->prev = pPrev;
    }
    free(pCur);
    pCur = NULL;
    pInfo->pFileList->fileCount --;

    pInfo->position = 0;
    pInfo->duration = 0;
    pInfo->progressWidth = 0;

    ShowPreviewInfo(hDlg, pInfo, TRUE);
    //InvalidateRect(hDlg, &g_time_rect, TRUE);
    //InvalidateRect(hDlg, &g_progress_rect, TRUE);
    InvalidateRect(hDlg, 0, TRUE);

    INFO("delete current media end\n");
}

static int UpdateVideoSpeed(HWND hDlg, PPlaybackInfo pInfo, float speed)
{
    BOOL changed = FALSE;
    if(pInfo->speed == speed)
        return -1;
    INFO("UpdateVideoSpeed, old: %f, new:%f\n", pInfo->speed, speed);
    pInfo->speed = speed;
    pInfo->player->video_speedup(pInfo->speed);
    InvalidateRect(hDlg, &g_speed_rect, TRUE);
    return 0;
}

static int PlayVideo(HWND hDlg, PPlaybackInfo pInfo, float speed)
{
    INFO("PlayVideo, isPlaying: %d, isStarted: %d\n", pInfo->isPlaying, pInfo->isStarted);
    if(pInfo->isPlaying)
        return 0;

    if(pInfo->duration == 0) {
        INFO("duration == 0, file cannot be decode\n");
        return -1;
    }
    if(pInfo->shadowEffect) {
        pInfo->shadowEffect = FALSE;
        InvalidateRect(hDlg, 0, TRUE);
    }
    

    GoingToHideStatusbar(hDlg, pInfo, FALSE);
    

    if(!pInfo->isStarted) {
        if(!(pInfo->player->begin_video())) {
            SetHighLoadStatus(1);
            //use the last file's playback speed if is auto playing
            UpdateVideoSpeed(hDlg, pInfo, speed);
            pInfo->isStarted = TRUE;
            pInfo->isPlaying = TRUE;
            SetTimer(hDlg, TIMER_VIDEO_TIME, 10);
            InvalidateRect(hDlg, &g_bottom_rect, TRUE);
        } else {
            ERROR("begin_video error\n");
            return -1;
        }
    } else {
        if(!(pInfo->player->resume_video())) {
            SetHighLoadStatus(1);
            UpdateVideoSpeed(hDlg, pInfo, 1.0f);
            pInfo->isPlaying = TRUE;
            SetTimer(hDlg, TIMER_VIDEO_TIME, 10);
            InvalidateRect(hDlg, &g_bottom_rect, TRUE);
        } else {
            ERROR("resume_video error\n");
            return -1;
        }
    }
    return 0;
}

static int PauseVideo(HWND hDlg, PPlaybackInfo pInfo)
{
    INFO("PauseVideo, isPlaying: %d, isStarted: %d\n", pInfo->isPlaying, pInfo->isStarted);
    if(!pInfo->isPlaying)
        return 0;

    ShowStatusbar(hDlg, pInfo);

    UpdateVideoSpeed(hDlg, pInfo, 1.0f);
    if(!(pInfo->player->pause_video())) {
        SetHighLoadStatus(0);
        pInfo->isPlaying = FALSE;
        KillTimer(hDlg, TIMER_VIDEO_TIME);
        InvalidateRect(hDlg, &g_bottom_rect, TRUE);
    } else {
        ERROR("pause_video error\n");
        return -1;
    }
    return 0;
}

static int StopVideo(HWND hDlg, PPlaybackInfo pInfo)
{
    INFO("StopVideo, isPlaying: %d, isStarted: %d\n", pInfo->isPlaying, pInfo->isStarted);
    int ret = 0;
    if(!pInfo->isStarted && !pInfo->isPlaying)
        return ret;

    ShowStatusbar(hDlg, pInfo);

    UpdateVideoSpeed(hDlg, pInfo, 1.0f);
    SetHighLoadStatus(0);
    if(!(pInfo->player->end_video())) {
        KillTimer(hDlg, TIMER_VIDEO_TIME);
    } else {
        ERROR("end_video error\n");
        ret = -1;
    }

    pInfo->isPlaying = FALSE;
    pInfo->isStarted = FALSE;
    InvalidateRect(hDlg, &g_bottom_rect, TRUE);
    return ret;
}

static void SetNextSpeed(HWND hDlg, PPlaybackInfo pInfo, BOOL fast)
{
    float power = log10(abs(pInfo->speed)) / log10(2);
    power *= abs(pInfo->speed) / pInfo->speed;
    BOOL changed = FALSE;
    float speed = 1.0f;

    power += fast ? 1.0f : -1.0f;
    if(power != 0) {
        speed = abs(power) / power * (1 << (int)abs(power));
    } else {
        speed = 1.0f;
    }

    if(speed > 32.0f)
        speed = 32.0f;
    if(speed < -32.0f)
        speed = -32.0f;

    if(speed != pInfo->speed)
        UpdateVideoSpeed(hDlg, pInfo, speed);
}

static void OnButtonConfirmed(HWND hDlg, PPlaybackInfo pInfo)
{
    switch(GetButtonID(pInfo)) {
        case BTV_PREV:
        case BTP_PREV:
            ChangeToPrevMedia(hDlg, pInfo, TRUE, TRUE);
            break;
        case BTV_PLAY:
            if(PlayVideo(hDlg, pInfo, 1.0f)) {
                ShowToast(hDlg, TOAST_PLAYBACK_ERROR, 100);
            }
            break;
        case BTV_PAUSE:
            PauseVideo(hDlg, pInfo);
            break;
        case BTV_NEXT:
        case BTP_NEXT:
            ChangeToNextMedia(hDlg, pInfo, TRUE, TRUE);
            break;
        case BTV_DELETE:
        case BTP_DELETE:
            DeleteCurrentMedia(hDlg, pInfo);
            break;
        case BTV_GRID:
        case BTP_GRID:
            EndPlaybackDialog(hDlg, IDOK);
            break;
        case BTV_VOLUME:
            ShowInfoDialog(hDlg, INFO_DIALOG_VOLUME);
            break;
        case BTV_REWIND:
            SetNextSpeed(hDlg, pInfo, FALSE);
            break;
        case BTV_FAST_FORWORD:
            SetNextSpeed(hDlg, pInfo, TRUE);
            break;
    }
}

static void OnKeyUp(HWND hDlg, PPlaybackInfo pInfo, int keyCode)
{
    INFO("OnKeyUp, %d\n", keyCode);
    int iconCount = 0;
    switch(keyCode) {
        case SCANCODE_SPV_MENU:
            //EndPlaybackDialog(hDlg, IDOK);
            break;
        case SCANCODE_SPV_OK:
            OnButtonConfirmed(hDlg, pInfo);
            break;
        case SCANCODE_SPV_UP:
            GetBottomItems(pInfo, &iconCount);
            pInfo->focusIndex = (pInfo->focusIndex - 1 + iconCount) % iconCount;
            InvalidateRect(hDlg, &g_focus_rect, TRUE);
            break;
        case SCANCODE_SPV_DOWN:
            GetBottomItems(pInfo, &iconCount);
            pInfo->focusIndex = (pInfo->focusIndex + 1) % iconCount;
            InvalidateRect(hDlg, &g_focus_rect, TRUE);
            break;
    }
}

static BOOL BatteryFlickTimer(HWND hDlg, int id, DWORD time)
{
    static BOOL iconVisible;
    PPlaybackInfo pInfo = (PPlaybackInfo)GetWindowAdditionalData(hDlg);
    pInfo->batteryVisible = iconVisible = !iconVisible;
    InvalidateRect(hDlg, &g_battery_rect, TRUE);
    return TRUE;    
}

static void ShowStatusbar(HWND hDlg, PPlaybackInfo pInfo)
{
    KillTimer(hDlg, TIMER_HIDE_STATUSBAR);
    pInfo->isBarHiding = FALSE;
    InvalidateRect(hDlg, 0, TRUE);
}

static void GoingToHideStatusbar(HWND hDlg, PPlaybackInfo pInfo, BOOL reset)
{
    INFO("GoingToHideStatusbar\n");
    if(reset) {
        ResetTimer(hDlg, TIMER_HIDE_STATUSBAR, 100 * 3);
    } else {
        SetTimer(hDlg, TIMER_HIDE_STATUSBAR, 100 * 3);
    }
    g_force_clean_dc = TRUE;
    pInfo->isBarHiding = FALSE;
    InvalidateRect(hDlg, 0, TRUE);
}

static int PlaybackDialogBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    PPlaybackInfo pInfo;
    pInfo = (PPlaybackInfo)GetWindowAdditionalData(hDlg);

    switch(message) {
        case MSG_INITDIALOG:
            {
                g_playback_window = hDlg;
                SetWindowAdditionalData(hDlg, lParam);
                g_info_font = CreateLogFont("ttf", "simsun", "UTF-8",
                    FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                    FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                    FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                    (VIEWPORT_WIDTH >= 400 ? 16: 14) * SCALE_FACTOR, 0);
                SetWindowBkColor(hDlg, SPV_COLOR_TRANSPARENT);
                //SendNotifyMessage(hDlg, MSG_BATTERY_STATUS, 0, 0);

                pInfo = (PPlaybackInfo)lParam;
                pInfo->player = media_play_create();
                register_video_callback(pInfo->player, OnVideoCallback);
                if(IsPhotoView(pInfo)) {
                    GoingToHideStatusbar(hDlg, pInfo, FALSE);
                }
                PostMessage(hDlg, MSG_SHOW_PREVIEW, 0, 0);
                break;
            }
        case MSG_SHOW_PREVIEW:
            ShowPreviewInfo(hDlg, pInfo, TRUE);
            break;
        case MSG_PAINT:
            hdc = BeginSpvPaint(hDlg);
            DrawPlaybackDialog(hDlg, hdc, pInfo);
            EndPaint(hDlg, hdc);
            if(g_force_clean_dc) {
                g_force_clean_dc = FALSE;
#if !COLORKEY_ENABLED
                if(GetActiveWindow() == hDlg) {
                    hdc = GetSecondaryDC(hDlg);
                    SetMemDCAlpha (hdc, 0, 0);
                    BitBlt(hdc, 0, 0, DEVICE_WIDTH, DEVICE_HEIGHT, HDC_SCREEN, 0, 0, 0);
                    ReleaseSecondaryDC(hDlg, hdc);
                }
#endif
            }
            //INFO("playback, msg_paint end\n");
            break;
        case MSG_COMMAND:
            switch(wParam) {
                case IDOK:
                case IDCANCEL:
                    EndPlaybackDialog(hDlg, wParam);
                    break;
            }
            break;
        case MSG_ACTIVE:
            if(wParam) {
                //ShowStatusbar(hDlg, pInfo);
                g_force_clean_dc = TRUE;
            }
            break;
        case MSG_CLEAN_SECONDARY_DC:
            g_force_clean_dc = TRUE;
            InvalidateRect(hDlg, 0, TRUE);
            break;
        case MSG_KEYUP:
            //INFO("infodlg, keydown: %d\n", wParam);
            if(IsPhotoView(pInfo) || pInfo->isPlaying || pInfo->isBarHiding) {
                GoingToHideStatusbar(hDlg, pInfo, TRUE);
                if(pInfo->isBarHiding && GetButtonID(pInfo) != BTV_PAUSE) {
                    return 0;
                }
            }
            OnKeyUp(hDlg, pInfo, wParam);
            break;
        case MSG_SDCARD_MOUNT:
            if(!wParam) {
                StopVideo(hDlg, pInfo);
                SendNotifyMessage(GetHosting(hDlg), MSG_SDCARD_MOUNT, wParam, 0);
                EndPlaybackDialog(hDlg, IDCANCEL);
            }
            return 0;
        case MSG_SHUTDOWN:
            StopVideo(hDlg, pInfo);
            return 0;
        case MSG_BATTERY_STATUS:
        case MSG_CHARGE_STATUS:
            INFO("MSG_CHARGE_STATUS/MSG_BATTERY_STATUS: wparam: %d, lParam: %ld\n", wParam, lParam);
            /*if(message == MSG_BATTERY_STATUS) {
                pInfo->battery = wParam;//SpvGetBattery();
            } else {
                pInfo->isCharging = (wParam == BATTERY_CHARGING);//SpvIsCharging();
            }
            if(pInfo->battery <= 10 && !pInfo->isCharging && !IsTimerInstalled(hDlg, TIMER_BATTERY)) {
                SetTimerEx(hDlg, TIMER_BATTERY, 50, BatteryFlickTimer);
            } else if(pInfo->battery > 10 || pInfo->isCharging) {
                KillTimer(hDlg, TIMER_BATTERY);
                pInfo->batteryVisible = TRUE;
            }*/

            InvalidateRect(hDlg, &g_battery_rect, TRUE);
            return 0;
        case MSG_TIMER:
            TimerProc(hDlg, pInfo, wParam);
            break;
        case MSG_PLAYBACK_COMPLETION:
            INFO("MSG_PLAYBACK_COMPLETION\n");
            if(pInfo->isPlaying || pInfo->isStarted) {
                float lastSpeed = pInfo->speed;
                StopVideo(hDlg, pInfo);
                pInfo->position = 0;
                pInfo->progressWidth = 0;
                ShowStatusbar(hDlg, pInfo);
#ifdef GUI_AUTO_PLAY
#if LOOP_PLAYBACK
                if(lastSpeed == 1.0f && !ChangeToNextMedia(hDlg, pInfo, FALSE, TRUE)) {
#else
                if(lastSpeed == 1.0f && !ChangeToNextMedia(hDlg, pInfo, FALSE, FALSE)) {
#endif
                    sleep(1);
                    //pInfo->speed = lastSpeed;
                    if(PlayVideo(hDlg, pInfo, lastSpeed)) {
                        ShowToast(hDlg, TOAST_PLAYBACK_ERROR, 100);
                    }
                } else 
#endif
                {
                    ShowPreviewInfo(hDlg, pInfo, TRUE);
                }
            }
            break;
        case MSG_PLAYBACK_ERROR:
            ERROR("MSG_PLAYBACK_ERROR\n");
            if(pInfo->isPlaying) {
                StopVideo(hDlg, pInfo);
            }
            ShowToast(hDlg, TOAST_PLAYBACK_ERROR, 100);
            ShowStatusbar(hDlg, pInfo);
            break;
        case MSG_MEDIA_SCAN_FINISHED:
            INFO("MSG_MEDIA_SCAN_FINISHED\n");
            //ResetMediaFiles();
            break;
        case MSG_CLOSE:
            EndPlaybackDialog(hDlg, IDOK);
            break;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

static void MeasureWindowSize()
{
    g_time_rect.left = PB_ICON_WIDTH * SCALE_FACTOR;
    g_time_rect.top = 0;
    g_time_rect.right = (VIEWPORT_WIDTH - PB_ICON_WIDTH) * SCALE_FACTOR;
    g_time_rect.bottom = (PB_BAR_HEIGHT - PB_PROGRESS_HEIGHT) * SCALE_FACTOR;

    g_progress_rect.left = 0;
    g_progress_rect.top = (PB_BAR_HEIGHT - PB_PROGRESS_HEIGHT) * SCALE_FACTOR;
    g_progress_rect.right = VIEWPORT_WIDTH * SCALE_FACTOR;
    g_progress_rect.bottom = PB_BAR_HEIGHT * SCALE_FACTOR;

    g_focus_rect.left = 0;
    g_focus_rect.top = (VIEWPORT_HEIGHT - PB_FOCUS_HEIGHT) * SCALE_FACTOR;
    g_focus_rect.right = VIEWPORT_WIDTH * SCALE_FACTOR;
    g_focus_rect.bottom = VIEWPORT_HEIGHT * SCALE_FACTOR;

    g_bottom_rect.left = 0;
    g_bottom_rect.top = (VIEWPORT_HEIGHT - PB_BAR_HEIGHT) * SCALE_FACTOR;
    g_bottom_rect.right = VIEWPORT_WIDTH * SCALE_FACTOR;
    g_bottom_rect.bottom = VIEWPORT_HEIGHT * SCALE_FACTOR;

    g_battery_rect.left = (VIEWPORT_WIDTH - PB_ICON_WIDTH) * SCALE_FACTOR;
    g_battery_rect.top = 0;
    g_battery_rect.right = VIEWPORT_WIDTH * SCALE_FACTOR;
    g_battery_rect.bottom = PB_ICON_HEIGHT * SCALE_FACTOR;

    g_speed_rect.left = (VIEWPORT_WIDTH - PB_ICON_WIDTH - 80) * SCALE_FACTOR;
    g_speed_rect.top = PB_BAR_HEIGHT / 2 * SCALE_FACTOR;
    g_speed_rect.right = (VIEWPORT_WIDTH - PB_ICON_WIDTH - 6) * SCALE_FACTOR;
    g_speed_rect.bottom = (PB_BAR_HEIGHT - PB_PROGRESS_HEIGHT) * SCALE_FACTOR;
}

void ShowPlaybackDialog(HWND hWnd, FileList *pFileList)
{
    if(pFileList == NULL || pFileList->fileCount <= 0) {
        ERROR("Filelist is empty!\n");
        return;
    }
    PPlaybackInfo info = (PPlaybackInfo) malloc(sizeof(PlaybackInfo));
    if(info == NULL) {
        ERROR("ERROR: malloc failed!!!!!!!!!!!\n");
        return;
    }
    memset(info, 0, sizeof(PlaybackInfo));
    info->pFileList = pFileList;
    info->pNode = pFileList->pHead;
    info->fileIndex = 1;
    //info->battery = SpvGetBattery();
    //info->isCharging = SpvIsCharging();
    info->batteryVisible = TRUE;
    info->speed = 1.0f;
    info->shadowEffect = TRUE;

    DLGTEMPLATE PlaybackDlg =
    {
        WS_NONE,
#if SECONDARYDC_ENABLED
        WS_EX_AUTOSECONDARYDC,
#else
        WS_EX_NONE,
#endif
        0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
        "PlaybackDialog",
        0, 0,
        0, NULL,
        0
    };
    MeasureWindowSize();

    DialogBoxIndirectParam(&PlaybackDlg, hWnd, PlaybackDialogBoxProc, (LPARAM)info);

    g_playback_window = HWND_INVALID;

    free(info);
    INFO("ShowPlaybackDialog end\n");
}

