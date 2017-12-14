/****************************************************************************
*              Copyright 2007 - 2011, Hisilicon Tech. Co., Ltd.
*                           ALL RIGHTS RESERVED
* FileName: hi_mtrans_err.h
* Description: mtrans error code list
*
* History:
* Version   Date         Author     DefectNum    Description
* 1.1       2008-02-18   w54723     NULL         Create this file.
*****************************************************************************/

#ifndef __HI_MTRANS_ERR_H__
#define __HI_MTRANS_ERR_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
//#include "hi_type.h"
//#include "hi_log.h"
#include "hi_log_def.h"

#define HI_ERR_MTRANS_ILLEGAL_PARAM             HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x01)
#define HI_ERR_MTRANS_CALLOC_FAILED             HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x02)
#define HI_ERR_MTRANS_OVER_TASK_NUM             HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x03)
#define HI_ERR_MTRANS_ILLEGALE_TASK_NUM         HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x04)
#define HI_ERR_MTRANS_NOT_SUPPORT_TRANSMODE     HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x05)
#define HI_ERR_MTRANS_ILLEGALE_TASK             HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x06)
#define HI_ERR_MTRANS_NOT_ALLOWED_CREAT_TASK    HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x07)
#define HI_ERR_MTRANS_NOT_ALLOWED_START_TASK    HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x08)
#define HI_ERR_MTRANS_NOT_ALLOWED_STOP_TASK     HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x09)
#define HI_ERR_MTRANS_NO_DATA                   HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x0A)
#define HI_ERR_MTRANS_SEND                      HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x0B)
#define HI_ERR_MTRANS_UNRECOGNIZED_TRANS_MODE   HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x0C)
#define HI_ERR_MTRANS_UNSUPPORT_PACKET_TYPE     HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x0D)
#define HI_ERR_MTRANS_ALREADY_INIT              HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x0E)
#define HI_ERR_MTRANS_ALREADY_DEINIT            HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x0F)
#define HI_ERR_MTRANS_UNSUPPORT_PACKET_MEDIA_TYPE HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x10)
#define HI_ERR_MTRANS_CREATE_THREAD             HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x11)
#define HI_ERR_MTRANS_UNSUPPORT_GETPTS             HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x12)
#define HI_ERR_MTRANS_SOCKET_ERR             HI_APP_DEF_ERR(HI_APPID_MTRANS, LOG_LEVEL_ERROR, 0x13)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
