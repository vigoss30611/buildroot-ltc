/**
  ******************************************************************************
  * @file       encoder_control.c
  * @author     InfoTM IPC Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 InfoTM</center></h2>
  *
  * This software is confidential and proprietary and may be used only as expressly
  * authorized by a licensing agreement from InfoTM, and ALL RIGHTS RESERVED.
  * 
  * The entire notice above must be reproduced on all copies and should not be
  * removed.
  ******************************************************************************
  */
#ifndef __ENCODE_CONTROLLER_H__
#define __ENCODE_CONTROLLER_H__

enum {
    MODE_DAY,
    MODE_NIGHT,
    MODE_DARK,
    MODE_LOWLUX,
};

enum {
    MOTION_NONE,
    MOTION_TRIGGER
};

enum {
    ENCODE_BITRATE_LOW = -1,
    ENCODE_BITRATE_NORMAL = 0,
    ENCODE_BITRATE_HIGH = 1
};

enum {
    ENCODE_CONTROLLER_NONE = 0, 
    ENCODE_CONTROLLER_RECOVERY = 1,
    ENCODE_CONTROLLER_CONTROL = 2,
};

int encode_controller_init(void);
int encode_controller_deinit(void);
int encode_controller_set_encode_param(int id, int bitrate, int rc_mode, int p_frame, int quality);
int encode_controller_set_bitrate(int id, int bitrate);
int encode_controller_refresh_bitrate(int id);
int encode_controller_set_motion(int motion);
int encode_controller_set_night(int mode);

int encode_controller_count_bitrate(int id, int frcnt, int priv, int size);
int encode_controller_check_bitrate(int id);

int encode_controller_check_control();
int encode_controller_set_control(int control);

#endif
