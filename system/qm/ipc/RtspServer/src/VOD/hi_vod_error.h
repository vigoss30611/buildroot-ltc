
/****************************************************************************
*              Copyright 2007 - 2011, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_vod_err.h
* Description: vod error code list
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2008-02-18   w54723     NULL         Create this file.
*****************************************************************************/

#ifndef __HI_VOD_ERR_H__
#define __HI_VOD_ERR_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include "hi_type.h"
#include "hi_log_def.h"

#define HI_ERR_VOD_OVER_CONN_NUM                HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x01)
#define HI_ERR_VOD_ILLEGAL_PARAM                HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x02)
#define HI_ERR_VOD_DESCRIBE_FAILED              HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x03)
#define HI_ERR_VOD_SETUP_FAILED                 HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x04)
#define HI_ERR_VOD_PLAY_FAILED                  HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x05)
#define HI_ERR_VOD_PAUSE_FAILED                 HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x06)
#define HI_ERR_VOD_TEARDOWN_FAILED              HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x07)
#define HI_ERR_VOD_ILLEGAL_USER                 HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x08)
#define HI_ERR_VOD_INVALID_SESSION              HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x09)
#define HI_ERR_VOD_GET_PIC_SIZE_FAILED          HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x0A)
#define HI_ERR_VOD_UNRECOGNIZED_TRANS_MODE      HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x0B)
#define HI_ERR_VOD_PLAY_METHOD_MEDIA_NOTINPLAYSTAE  HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x0C)
#define HI_ERR_VOD_PLAY_METHOD_START_MTRANS_FAILED  HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x0D)
#define HI_ERR_VOD_CALLOC_FAILED                  HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x0E)
#define HI_ERR_VOD_ALREADY_DEINIT                 HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x0F)
#define HI_ERR_VOD_ALREADY_INIT                   HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x10)
#define HI_ERR_VOD_ILLEGAL_CB                   HI_APP_DEF_ERR(HI_APPID_VOD,LOG_LEVEL_ERROR,0x11)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

