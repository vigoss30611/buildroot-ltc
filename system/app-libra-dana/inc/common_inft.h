#ifndef COMMON_INTF_H
#define COMMON_INTF_H

#include "common_inft_cfg.h"
#include "common_inft_action.h"
#include "common_inft_wifi.h"
#include "common_inft_data.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* data API to get audio/video data */
int Inft_data_getVideoFrame(SHM_ENTRY_TYPE entryId, char** ppData, int *pDataLen, 
    int *pSerialNo, int *pAudioRefSerial, struct timeval * pTime, int *pIsKey); 
int Inft_data_getAudioFrame(SHM_ENTRY_TYPE entryType, char** ppData, int *pDataLen, 
    int *pSerialNo, struct timeval * pTime);

/* get Thumbnail api */
int inft_data_getThumbnail(char* filename, int res_wid_in, int res_hei_in, char **ppbuff_out, int *plen_out); 

/* config api */
int Inft_cfg_setResolution(const int width, const int height);
int Inft_cfg_getVideoAttr(const Inft_Video_Mode_Attr *pstVideoAttr,ENUM_INFT_CAMERA camera);
int Inft_cfg_setVideoAttr(const Inft_Video_Mode_Attr *pstVideoAttr,ENUM_INFT_CAMERA camera);
int Inft_cfg_getPhotoAttr(const Inft_PHOTO_Mode_Attr *pstPhotoAttr,ENUM_INFT_CAMERA camera);
int Inft_cfg_setPhotoAttr(const Inft_PHOTO_Mode_Attr *pstPhotoAttr,ENUM_INFT_CAMERA camera);
int Inft_cfg_getConfigAttr(const Inft_CONFIG_Mode_Attr *pstConfigAttr);
int Inft_cfg_setConfigAttr(const Inft_CONFIG_Mode_Attr *pstConfigAttr);

int Inft_cfg_reset(); 

/*action operation*/
int Inft_getFileDuration(char*filename);//full name
int Inft_getPlayAttr(const Inft_Play_Attr *pstPlayAttr);
int Inft_setPlayAttr(const Inft_Play_Attr *pstPlayAttr);
int Inft_action_getAttr(Inft_Action_Attr *pstActionAttr);
int Inft_action_setAttr(Inft_Action_Attr *pstActionAttr);

/*wifi operation*/
int Inft_wifi_getAttr(Inft_Wifi_Attr *pstWifiAttr);
int Inft_wifi_setAttr(Inft_Wifi_Attr *pstWifiAttr);
int Inft_wifi_scanAps(Inft_Wifi_Attr *pstWifiAttr);

/*gps operation*/
int Inft_gps_getData(GPS_Data *data);
int Inft_gps_setData(GPS_Data *data);


#ifdef __cplusplus
}
#endif

#endif

