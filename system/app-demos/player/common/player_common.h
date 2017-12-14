/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: Player common functions
 *
 * Author:
 *     ranson.ni <ranson.ni@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/25/2017 init
 */
#include <qsdk/vplay.h>

#ifndef __PLAYER_COMMON_H__
#define __PLAYER_COMMON_H__

/*
 * @brief player event handler
 * @param ps8Name        -IN: event name
 * @param pArgs        -IN: event parameter
 * @return void
 */
void VplayEventHandler(char *ps8Name, void *pArgs);

/*
 * @brief player signal handler
 * @param s32Sig        -IN: signal id
 * @return void
 */
void VplayerSignalHandler(int s32Sig);

/*
 * @brief player init signal handler
 * @param void
 * @return void
 */
void VplayerInitSignal(void);

/*
 * @brief init player info
 * @param pstInfo        -IN: player info
 * @param s32Cnt         -IN: player cnt
 * @return void
 */
void InitPlayerInfo(VPlayerInfo *pstInfo, int s32Cnt);

/*
 * @brief show player info
 * @param pstInfo        -IN: player info
 * @return void
 */
void ShowPlayerInfo(VPlayerInfo *pstInfo);
#endif
