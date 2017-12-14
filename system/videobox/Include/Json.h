/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

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

#ifndef Json_H
#define Json_H
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <qsdk/videobox.h>

/* Json Types: */
#define Json_False 0
#define Json_True 1
#define Json_NULL 2
#define Json_Number 3
#define Json_String 4
#define Json_Array 5
#define Json_Object 6

#define Json_IsReference 256
#define SJson_Number "3"
#define SJson_String "4"

#define JSDG(fp, fmt, x...) do { if(fp) fprintf(fp, fmt, ##x);\
    else printf(fmt, ##x);} while (0)

typedef enum {
    E_JSON_LAYER_ROOT = 0,
    E_JSON_LAYER_IPU,
    E_JSON_LAYER_ARGS_PORT,
    E_JSON_LAYER_PORT,
    E_JSON_LAYER_BIND_EMBEZZLE,
} EN_JSON_LAYER;

typedef enum {
    E_JSON_KEYNAME = 0,
    E_JSON_VALUETYPE,
} EN_JSON_KEYVALUE;

typedef struct {
    bool bIsIpu;
    bool bIsArgs;
    bool bIsPort;
    bool bIsBind;
    bool bIsEmbezzle;
    bool bSet;
    bool bGet;
    bool bDelete;
} ST_JSON_OPERA;

const char *const ps8IspArgsInfo[][2] =
{
    //key name,                     key type
    { "nbuffers",               SJson_Number },
    { "skipframe",              SJson_Number },
    { "flip",                   SJson_Number }
};

const char *const ps8IspostArgsInfo[][2] =
{
    { "nbuffers",               SJson_Number },
    { "lc_grid_file_name1",     SJson_String },
    { "lc_enable",              SJson_Number },
    { "lc_grid_target_index",   SJson_Number },
    { "dn_enable",              SJson_Number },
    { "fcefile",                SJson_String },
    { "fisheye",                SJson_Number },
    { "lc_geometry_file_name1", SJson_String },
    { "lc_grid_line_buf_enable",SJson_Number },
    { "dn_target_index",        SJson_Number },
    { "linkmode",               SJson_Number },
    { "linkbuffers",            SJson_Number }
};

const char *const ps8VencoderArgsInfo[][2] =
{
    { "encode_type",            SJson_String },
    { "buffer_size",            SJson_Number },
    { "min_buffernum",          SJson_Number },
    { "framerate",              SJson_Number },
    { "framebase",              SJson_Number },
    { "rotate",                 SJson_String },
    { "smartrc",                SJson_Number },
    { "refmode",                SJson_Number },
    { "mv_w",                   SJson_Number },
    { "mv_h",                   SJson_Number },
    { "enable_longterm",        SJson_Number },
    { "chroma_qp_offset",       SJson_Number },
    { "rate_ctrl",              "NULL"       } //It's value is a struct
};

const char *const ps8H1jpegArgsInfo[][2] =
{
    { "mode",                   SJson_String },
    { "rotate",                 SJson_String },
    { "buffer_size",            SJson_Number },
    { "use_backup_buf",         SJson_Number },
    { "thumbnail_mode",         SJson_String },
    { "thumbnail_width",        SJson_Number },
    { "thumbnail_height",       SJson_Number }
};

const char *const ps8JencArgsInfo[][2] =
{
    { "mode",                   SJson_String },
    { "buffer_size",            SJson_Number }
};

const char *const ps8PpArgsInfo[][2] =
{
    { "rotate",                 SJson_String }
};

const char *const ps8MarkerArgsInfo[][2] =
{
    { "mode",                   SJson_String }
};

const char *const ps8IdsArgsInfo[][2] =
{
    { "no_osd1",                SJson_Number }
};

const char *const ps8DgFrameArgsInfo[][2] =
{
    { "nbuffers",               SJson_Number },
    { "data",                   SJson_String },
    { "mode",                   SJson_String }
};

const char *const ps8FoDetArgsInfo[][2] =
{
    { "src_img_wd",             SJson_Number },
    { "src_img_ht",             SJson_Number },
    { "src_img_st",             SJson_Number },
    { "crop_x_ost",             SJson_Number },
    { "crop_y_ost",             SJson_Number },
    { "crop_wd",                SJson_Number },
    { "crop_ht",                SJson_Number },
    { "fd_db_file_name1",       SJson_String }
};

const char *const ps8VaArgsInfo[][2] =
{
    { "send_blk_info",          SJson_Number }
};

const char *const ps8FileSinkArgsInfo[][2] =
{
    { "data_path",              SJson_String },
    { "max_save_cnt",           SJson_Number }
};

const char *const ps8FileSrcArgsInfo[][2] =
{
    { "data_path",              SJson_String },
    { "frame_size",             SJson_Number }
};

const char *const ps8V4l2ArgsInfo[][2] =
{
    { "driver",                 SJson_String },
    { "string",                 SJson_String },
    { "pixfmt",                 SJson_String }
};

typedef enum {
    E_VIDEOBOX_PORT_W = 0,
    E_VIDEOBOX_PORT_H,
    E_VIDEOBOX_PORT_PX,
    E_VIDEOBOX_PORT_PY,
    E_VIDEOBOX_PORT_PW,
    E_VIDEOBOX_PORT_PH,
    E_VIDEOBOX_PORT_LINKMODE,
    E_VIDEOBOX_PORT_LOCKRATIO,
    E_VIDEOBOX_PORT_TIMEOUT,
    E_VIDEOBOX_PORT_MINFPS,
    E_VIDEOBOX_PORT_MAXFPS,
    E_VIDEOBOX_PORT_EXPORT,
    E_VIDEOBOX_PORT_PIXELFORMAT,
    E_VIDEOBOX_PORT_END
} E_VIDEOBOX_PORT_ATTRIBUTE;

const char *const ps8PortInfo[][2] =
{
    { "w",                      SJson_Number },
    { "h",                      SJson_Number },
    { "pip_x",                  SJson_Number },
    { "pip_y",                  SJson_Number },
    { "pip_w",                  SJson_Number },
    { "pip_h",                  SJson_Number },
    { "link",                   SJson_Number },
    { "lock_ratio",             SJson_Number },
    { "timeout",                SJson_Number },
    { "minfps",                 SJson_Number },
    { "maxfps",                 SJson_Number },
    { "export",                 SJson_Number },
    { "pixel",                  SJson_String }
};

typedef enum {
    E_VIDEOBOX_RATECTRL_RCMODE = 0,
    E_VIDEOBOX_RATECTRL_GOP,
    E_VIDEOBOX_RATECTRL_IDRINTERVAL,
    E_VIDEOBOX_RATECTRL_QPMAX,
    E_VIDEOBOX_RATECTRL_QPMIN,
    E_VIDEOBOX_RATECTRL_BITRATE,
    E_VIDEOBOX_RATECTRL_QPDELTA,
    E_VIDEOBOX_RATECTRL_HDR,
    E_VIDEOBOX_RATECTRL_MAXBITRATE,
    E_VIDEOBOX_RATECTRL_THRESHOLD,
    E_VIDEOBOX_RATECTRL_QPFIX,
    E_VIDEOBOX_RATECTRL_END
} E_VIDEOBOX_RATECTRL;

const char *const ps8RateCtrlInfo[][2] =
{
    { "rc_mode",                SJson_Number },
    { "gop_length",             SJson_Number },
    { "idr_interval",           SJson_Number },
    { "qp_max",                 SJson_Number },
    { "qp_min",                 SJson_Number },
    { "bitrate",                SJson_Number },
    { "qp_delta",               SJson_Number },
    { "qp_hdr",                 SJson_Number },
    { "max_bitrate",            SJson_Number },
    { "threshold",              SJson_Number },
    { "qp_fix",                 SJson_Number }
};

/*
typedef struct {
    int rc_mode;
    int gop_length;
    int idr_interval;
    int qp_max;
    int qp_min;
    int bitrate;
    int qp_delta;
    int qp_hdr;
    int max_bitrate;
    int threshold;
    int qp_fix;
} ST_VIDEOBOX_VENCODER_RATECTRL;
*/

class Json {
private:
    class Json *next,*prev;
    class Json *child;
    class Json *father;
    const char *ep;

    int strcasecmp(const char *s1,const char *s2);
    const char *skip(const char *in); /* Utility to jump whitespace and cr/lf */
    const char *parse_number(Json *item,const char *num);
    unsigned parse_hex4(const char *str);
    static const unsigned char firstByteMark[7];
    const char *parse_string(Json *item,const char *str);
    const char *parse_array(Json *item,const char *value);
    const char *parse_object(Json *item,const char *value);
    const char *parse_value(Json *item,const char *value);

public:
    int type;
    char *valuestring;
    int valueint;
    double valuedouble;
    bool bDKeepNext;
    char *string;
    EN_JSON_LAYER enLayer;

    Json();
    ~Json();
    Json(std::string jss);
    void Load(std::string fjs);
    const char *EP();
    Json *Next();
    Json *Child();
    Json *GetObject(const char* s);
    const char * GetString(const char *s);
    int GetInt(const char* s);
    double GetDouble(const char * s);
    int ParseIpuName(char *name, char* ipuname);
    EN_VIDEOBOX_RET JOperaCore(ST_JSON_OPERA *pstOpera, char* ps8IpuName, char *ps8PortName,
            const char *ps8Key, const void *ps8Value, int s32ValueType);
    void JsonShow(FILE *fp);
};
#endif // Json_H
