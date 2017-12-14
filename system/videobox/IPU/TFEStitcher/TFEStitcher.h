// Copyright (C) 2016 InfoTM, XXX
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

#ifndef IPU_TFEStitcher_H
#define IPU_TFEStitcher_H
#include <IPU.h>

class IPU_TFEStitcher: public IPU {

private:
public:

    IPU_TFEStitcher(std::string, Json *);
    void Prepare();
    void Unprepare();
    void WorkLoop();
    int UnitControl(std::string, void*);
    void Stitch(char *b0, char *b1, char *out, int w, int h);
};

#endif // IPU_TFEStitcher_H
