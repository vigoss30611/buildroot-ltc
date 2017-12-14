#ifndef __SPV_PlAYBACK_DIALOG_H__
#define __SPV_PlAYBACK_DIALOG_H__ 

typedef enum {
    BTV_PREV,
    BTV_PLAY,
    BTV_NEXT,
    BTV_DELETE,
    BTV_GRID,
    BTV_REWIND,
    BTV_PAUSE,
    BTV_FAST_FORWORD,
    BTV_VOLUME,
    BTP_PREV,
    BTP_NEXT,
    BTP_DELETE,
    BTP_GRID,
} PB_BUTTON;

void ShowPlaybackDialog(HWND hWnd, FileList *pFileList);

#endif //__SPV_PlAYBACK_DIALOG_H__
