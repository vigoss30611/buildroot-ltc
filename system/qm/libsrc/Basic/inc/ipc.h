/**
  ******************************************************************************
  * @file       ipc.h
  * @author     InfoTM IPC Team
  * @brief      This file contains top definitions of ipc.
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
  *
  ******************************************************************************
  *
  * Revision History:
  * ----------------------------------------------------------------------------
  * v0.0.1  2016/06/17  leo.zhang   First created.
  * v1.0.0  2016/06/30  leo.zhang   -
  * v1.0.3  2016/07/06  leo.zhang   Add media-jpeg interface.
  * v1.0.4  2016/07/12  leo.zhang   Add ms_motdect_blind() interface.
  *
  ******************************************************************************
  */


#ifndef __IPC_H__
#define __IPC_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>

/**
 *
 */
#define SETTING_PREFIX      "ipc"

/**
 * @brief   debug related.
 */
#include <assert.h>

enum {
    L_DEBUG,
    L_INFO,
    L_WARN,
    L_ERROR,
    L_ALERT,
    L_OOO
};
void ipclog_log(char *tag, int level, char *fmt, ...);

#ifndef __IPC_RELEASE__
    #define ipclog_debug(fmt, args...)  ipclog_log(LOGTAG, L_DEBUG, fmt, ##args)
    #define ipclog_info(fmt, args...)   ipclog_log(LOGTAG, L_INFO, fmt, ##args)
    #define ipclog_warn(fmt, args...)   ipclog_log(LOGTAG, L_WARN, fmt, ##args)
    #define ipclog_error(fmt, args...)  ipclog_log(LOGTAG, L_ERROR, fmt, ##args)
    #define ipclog_alert(fmt, args...)  ipclog_log(LOGTAG, L_ALERT, fmt, ##args)
    #define ipclog_ooo(fmt, args...)    ipclog_log(LOGTAG, L_OOO, fmt, ##args)
#else
    #define ipclog_debug(fmt, args...) 
    #define ipclog_info(fmt, args...)
    #define ipclog_warn(fmt, args...)
    #define ipclog_error(fmt, args...)
    #define ipclog_alert(fmt, args...)
    #define ipclog_ooo(fmt, args...)
#endif

void ipc_panic(char const *what);

#endif  // __IPC_H__

