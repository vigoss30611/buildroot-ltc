// Copyright (C) 2016 InfoTM, yong.yan@infotm.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <System.h>
#include "VAQRScanner.h"

int IPU_QRScanner::UnitControl(std::string func, void *arg)
{
    LOGD("%s: %s\n", __func__, func.c_str());

    UCSet(func);

    UCFunction(va_qr_enable) {
        VaqrEnable();
        return 0;
    }

    UCFunction(va_qr_disable) {
        VaqrDisable();
        return 0;
    }

    UCFunction(va_qr_set_density) {
        int ret;

        ret = VaqrSetDensity((density_t *)arg);
        return ret;
    }

    UCFunction(va_qr_get_density) {
        VaqrGetDensity((density_t *)arg);
        return 0;
    }

    LOGE("%s is not support\n", func.c_str());
    return VBENOFUNC;
}
