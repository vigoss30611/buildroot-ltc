/*
*/

#ifndef __SPV_BROWSER_FOLDER_H__
#define __SPV_BROWSER_FOLDER_H__

#define SPV_CTRL_FOLDER "spv_folder"

typedef enum {
    FOLDER_VIDEO = 1,
    FOLDER_VIDEO_LOCKED,
    FOLDER_PHOTO,
/*    FOLDER_SLOW_MOTION,
    FOLDER_CONTINUOUS,
    FOLDER_CAR,
    FOLDER_NIGHT,*/
    FOLDER_EXIT
} FOLDER_TYPE;

#define FOLDER_TYPE_MASK 0x0000000FL

#define FOLDER_FOCUS 0x00000010L


BOOL RegisterSPVFolderControl (void);

BOOL UnregisterSPVFolerControl(void);

#endif  /* __SPV_BROWSER_FOLDER_H__ */

