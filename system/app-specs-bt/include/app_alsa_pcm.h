/*****************************************************************************
 **
 **  Name:           app_alsa.h
 **
 **  Description:    Linux ALSA utility functions (playback/recorde)
 **
 **  Copyright (c) 2011, Broadcom Corp., All Rights Reserved.
 **  Broadcom Bluetooth Core. Proprietary and confidential.
 **
 *****************************************************************************/

#ifndef APP_ALSA_PCM_H
#define APP_ALSA_PCM_H

/*******************************************************************************
 **
 ** Function        app_alsa_play_ble_voice
 **
 ** Description     ALSA play ble voice
 **
 ** Parameters      p_fname:path and name of wav file
 **
 ** Returns         0 for success;other for fail
 **
 *******************************************************************************/
int app_alsa_play_ble_voice(const char *p_fname);
#endif
