/**
  ******************************************************************************
  * @file       qm_event.h
  * @author     InfoTM IPC Team
  * @brief      This file contains top definitions of qm event.
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
  * \brief How to define EVENT MODE
  * - define in submodule header file,exp:
  *   - #define EVENT_ALARM_RESET "alarm_reset"
  *   - #define EVENT_SYSTEM_READY "system_ready"
  *   - #define EVENT_WIFI_STA_OK  "wifi_sta_ok"
  *
  ******************************************************************************
  */

#ifndef __QM_EVENT_H__
#define __QM_EVENT_H__

/**
 * @brief Register Handler for event
 *
 * @details This function register handler for one event, called by consumer 
 *
 * @param event [in] The event mode, string
 *
 * @param priority [in] The handler priority in event handle list, not use now
 *
 * @param handler [in] The handler add into event handle list, will be called when event trigger
 *
 * @return This function return -1 if failed, 0 if successed
 *
 */
int QM_Event_RegisterHandler(const char *event, const int priority, void (* handler)(char* event, void *arg, int size));

/**
 * @brief Unregister Handler for event
 *
 * @details This function unregister handler for one event,called by consumer 
 *
 * @param event [in] The event mode, string
 *
 * @param handler [in] The handler registerd for event,will remove from event handle list
 *
 * @return This function return -1 if failed, 0 if successed
 *
 */
int QM_Event_UnregisterHandler(const char *event, void (* handler)(char* event, void *arg, int size));

/**
 * @brief Send event
 *
 * @details This function Send event,distribute for all handler,called by producter
 *
 * @param event [in] The event mode, string
 *
 * @param arg [in] The point which keep event argument
 *
 * @param size [in] The size of the argument
 *
 * @return This function return -1 if failed, 0 if successed
 *
 */
int QM_Event_Send(const char *event, void *arg, int size);

#endif
