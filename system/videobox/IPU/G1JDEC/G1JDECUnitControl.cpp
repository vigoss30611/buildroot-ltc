/*
 * Copyright (c) 2016~2021 ShangHai InfoTM Ltd all rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: Jpeg HardWare Decode
 *
 * Author:
 *        devin.zhu <devin.zhu@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/21/2017 init
 */

#include <System.h>
#include "G1JDEC.h"

int IPU_G1JDEC::UnitControl(std::string func, void *pArg)
{
    int s32Ret = 0;

    UCSet(func);

    UCFunction(photo_trigger_decode) {
        LOGE("start tigger!\n");
        return DecodePhoto((bool*)pArg);
    }
    UCFunction(photo_decode_finish) {
        return DecodeFinish((bool*)pArg);
    }
    return s32Ret;
}