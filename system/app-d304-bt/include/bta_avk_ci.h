/*****************************************************************************
**
**  Name:           bta_avk_ci.h
**
**  Description:    This is the interface file for advanced audio/video call-in
**                  functions.
**
**  Copyright (c) 2005, Widcomm Inc., All Rights Reserved.
**  Widcomm Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_AVK_CI_H
#define BTA_AVK_CI_H

#include "bta_avk_api.h"

/*****************************************************************************
**  Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         bta_avk_ci_setconfig
**
** Description      This function must be called in response to function
**                  bta_avk_co_audio_setconfig() or bta_avk_co_video_setconfig.
**                  Parameter err_code is set to an AVDTP status value;
**                  AVDT_SUCCESS if the codec configuration is ok,
**                  otherwise error.
**
** Returns          void 
**
*******************************************************************************/
BTA_API extern void bta_avk_ci_setconfig(tBTA_AVK_CHNL chnl, UINT8 err_code, UINT8 category);

#ifdef __cplusplus
}
#endif

#endif /* BTA_AVK_CI_H */

