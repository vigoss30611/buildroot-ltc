/*
 * Copyright (c) 2013 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * libdemux sample code.
 *
 */

#include <stdio.h>
#include <qsdk/videobox.h>
#include <qsdk/demux.h>

#define DEMUX_ENABLE_DECODE 0
#define MAX_FRM_CNT 100

void dump_data(char* buf, int buf_len, int show_size)
{
    printf("dump data(%p %d %d):", buf, buf_len, show_size);
    for (int cnt = 0; cnt < buf_len && cnt < show_size; cnt ++)
    {
        if (cnt % 8 == 0)
        {
            printf("\n");
        }
        printf("%3x", buf[cnt]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    struct demux_t *dmx;
    struct demux_frame_t frm;
    char header[128];
    int ret = 0;
    char* tmp = NULL;
    int frm_cnt = 0;

    if(argc < 3) {
        printf("player FILE_NAME DEC_PORT FS_PORT\n");
        return 0;
    }

    dmx = demux_init(argv[1]);
    if(!dmx) {
        printf("demux %s failed\n", argv[1]);
        return -1;
    }

    ret = demux_get_head(dmx, header, 128);
    if (ret <= 0)
    {
        printf("get head error, ret:%d!\n", ret);
        return -1;
    }
#if DEMUX_ENABLE_DECODE
    video_set_header(argv[2], header, ret);
#endif

    dump_data(header, 128, ret);
    while(1) {
        ret = demux_get_frame(dmx, &frm);
        printf("codec id:%d ret:%d\n", frm.codec_id, ret);
        if (ret == 0)
            continue;
        if (ret < 0)
            break;
        if(frm.codec_id != DEMUX_VIDEO_H264 && frm.codec_id != DEMUX_VIDEO_H265 )
            goto skip;
        usleep(50000);
#if DEMUX_ENABLE_DECODE
        video_write_frame(argv[3], frm.data, frm.data_size);
#endif
        dump_data((char*)frm.data, frm.data_size, 16);
        frm_cnt ++;
skip:
        demux_put_frame(dmx, &frm);
        if (frm_cnt >= MAX_FRM_CNT)
        {
            printf("frm_cnt reached %d\n", frm_cnt);
            break;
        }
    }

    demux_deinit(dmx);
    return 0;
}


