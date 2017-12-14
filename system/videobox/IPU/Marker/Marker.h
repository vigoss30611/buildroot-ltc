// Copyright (C) 2016 InfoTM, jinhua.chen@infotm.com
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

#ifndef IPU_MARKER_H
#define IPU_MARKER_H
#include <IPU.h>
#include <fr/libfr.h>
#ifdef COMPILE_LIB_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype2/freetype/freetype.h>
#include <freetype2/freetype/ftglyph.h>
#endif
extern int PixFormatBits(Pixel::Format pf);
extern Pixel::Format PixFormatEnum(std::string str);

struct YUV {
    int y;
    int u;
    int v;
};

typedef struct _overlay_info {
    char *pAddr; // YUV buffer address
    unsigned int iPaletteTable[4];
    int width;
    int height;
    int disable;
}ST_OVERLAY_INFO;

class IPU_MARKER: public IPU {

    private:
        char *bitmap_buf = NULL;
        char *data_buf = NULL;
        volatile char disable_flag = 0;
        struct font_attr ft;
        int path_mode = 0;
        int md;
        int time_count;
        char temp[TEMP_FMT_LENGTH];
        char y = 0;
        char u = 0;
        char v = 0;
        char alpha = 0;
        char alpha_y = 0;
        char alpha_u = 0;
        char alpha_v = 0;
        char mark[128];
        char time_order[6] = {0};
        int offset = 0;
        char set_mode_sucess = 0;
        int total_width_size = 0;
        int total_height_size = 0;
        int m_num_chars;
        struct fr_buf_info ref;
        struct fr_info fr;
        FRBuffer *marker = NULL;
        int           target_height;
#ifdef COMPILE_LIB_FREETYPE
        FT_Library    library = NULL;                /* handle to library */
        FT_Face       face = NULL;                   /* handle to face object */
        FT_Matrix     matrix;                 /* transformation matrix */
        int Get_Fonts_offset(wchar_t *in_text);
        int Get_Fonts_Bitmap_Freetype(char *in_text);
        int draw_bitmap(FT_Bitmap*  bitmap, FT_Int x, FT_Int y, int offset);
#else
        char *lib_chinese;
        char *lib_asc;
        int m_lib_chinese_len;
        int m_lib_asc_len;
        int  font_library_init(char *font_file);
        int Get_Fonts_Bitmap(char *in_text);
#endif
        ST_OVERLAY_INFO bpp2_overlay_info;
        int NewMarker(std::string name, int w, int h, Pixel::Format pfmt);
        int DeleteMarker(void);
        int show_image(void);
        int Bitmap_To_Rgb(char *m_pImgDataOut, char *m_pImgData);

        int Bitmap_To_Rgb8888(char *m_pImgData);
        int Bitmap_To_Rgb565(char *m_pImgData);
        int Bitmap_To_NV12(char *m_pImgData);
        int Bitmap_To_NV21(char *m_pImgData);
        int Bitmap_To_YUV422(char *m_pImgData);
        int Bitmap_To_BPP2(char *m_pImgData);
/* marker API */
        int marker_disable(void);
        int marker_enable(void);
        int marker_set_mode(int mode, char *fmt, struct font_attr attr);
        int UpdateImage(char *path);
        int UpdateString(char *str);
        int marker_put_frame(void *arg);
        int Compose(char data, struct tm *mark_tm);
        int process_time(char *mark, int *last_sec);
        int process_utc(char *mark);
        int generate_time_string(char *fmt);
        unsigned int Get_Unicode(char *in_text, wchar_t *unicode_text, unsigned int out_buf_len);
        int GB2312_To_UTF8(char *inbuf, unsigned int inlen, char *outbuf, unsigned int outlen);
        int UTF8_To_GB2312(char *inbuf, unsigned int inlen, char *outbuf, unsigned int outlen);
        int UTF8_To_UNICODE(char *inbuf, unsigned int inlen, char *outbuf, unsigned int outlen);
        int code_convert(char *from_charset,char *to_charset,const char *inbuf,unsigned int inlen,
        char *outbuf,unsigned int outlen);
        /* bmp 图片相关的处理函数 32位RGB转YUV420SP */
        int marker_set_bmp(unsigned char *src, int size);
        struct YUV rgbToyuv(char R, char G, char B);
        void rgbToyuv(unsigned char R, unsigned char G , unsigned char B,
        unsigned char *Y, unsigned char * Cb, unsigned char *Cr);
        void ARGBtoAYCbCr(unsigned int ARGB, unsigned int* AYCbCr);

    public:
        IPU_MARKER(std::string name, Json *js);
        ~IPU_MARKER();
        void Prepare();
        void Unprepare();
#if 1
        void WorkLoop();
#endif
        int UnitControl(std::string func, void *arg);

        enum path{
            ISPPOST = 0,
            SOFTLAYER,
            ISPPOST2
        };
};

#endif
