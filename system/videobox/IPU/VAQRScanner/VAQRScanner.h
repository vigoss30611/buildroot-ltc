// Copyright (C) 2016 InfoTM, warits.wang@infotm.com
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

#ifndef IPU_QRScanner_H
#define IPU_QRScanner_H
#include <IPU.h>
#include <zbar.h>

using namespace zbar;
class IPU_QRScanner: public IPU {

private:
    bool zbarEnable;
    int density_x, density_y;
    std::mutex *MLock;
public:
    zbar_image_scanner_t *Scanner;
    zbar_image_t *Image;
    IPU_QRScanner(std::string, Json *);
    ~IPU_QRScanner();
    void Prepare();
    void Unprepare();
    void WorkLoop();
    void VaqrEnable();
    void VaqrDisable();
    int VaqrSetDensity(density_t *dsty);
    int VaqrGetDensity(density_t *dsty);
    int UnitControl(std::string, void*);
};

#endif // IPU_QRScanner_H
