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

#ifndef IPU_FFPHOTO_H
#define IPU_FFPHOTO_H

#include <IPU.h>
#include <semaphore.h>
extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/frame.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libavutil/opt.h>

#include <libavformat/avio.h>
#include <libavutil/file.h>

}

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef int int32_t;

struct buffer_data {
    uint8_t *ptr;
    size_t size; ///< size left in the buffer
};

typedef struct{
    int width;
    int height;
    int fmt;
}photo_info_t;

class IPU_FFPHOTO : public IPU{
    private:
        FRBuffer* decBuf;
        bool bEnableDecode;
        bool bDecodeFinish;
        photo_info_t info;
        int  ImageDecode(Buffer* pbfin);
    public:
        IPU_FFPHOTO(std::string name, Json *js);
        ~IPU_FFPHOTO();
        void Prepare();
        void Unprepare();
        void WorkLoop();
        bool DecodePhoto(bool* bEnable);
        bool DecodeFinish(bool* bFinish);
        int UnitControl(std::string, void *);
};
#endif

