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
 * Description: isp Panorama expansion function
 *
 * Author:
 *     minghui.zhao <minghui.zhao@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/24/2017 init
 *
 * TODO:
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qsdk/videobox.h>

#define VIDEOBOX_PATH_JSON "./path.json"

/*
* @brief main function
* @param  void
* @return 0: Success
* @return -1: Failure
*/
int main(void)
{
    int s32Retval = 0;

    s32Retval = videobox_repath(VIDEOBOX_PATH_JSON);
    return s32Retval;
}
