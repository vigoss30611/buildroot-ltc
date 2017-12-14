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
 * Description: decode & play demo
 *
 * Author:
 *     ranson.ni <ranson.ni@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  09/30/2017 init
 *
 * TODO:
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <fr/libfr.h>
#include <qsdk/audiobox.h>
#include <qsdk/event.h>
#include <qsdk/codecs.h>
#include <qsdk/demux.h>

#define PLAY_CHN_NAME   "default"
#define MP3_FILE_PATH   "/mnt/sd0/test.mp3"//"http://www.djyinyue.com/upload/dance/74F829FD251F5153269B6B522D332165.mp3"
#define MAX_FRM_CNT     100
#define DEMUX_STREAM    0
#define DUMP_PCM_DATA   0
#if DUMP_PCM_DATA
#define PCM_FILE_PATH   "/mnt/sd0/dump.pcm"
#else
#define PCM_FILE_PATH   ""
#endif

#ifndef AUDIO_PLAYBACK
#define AUDIO_PLAYBACK 0
#endif

/*
 * @brief dump buffer data
 * @param ps8Buf -IN: buffer address
 * @param s32BufLen -IN: the length of buffer
 * @param s32ShowSize -IN: the length of buffer for display
 * @return void
 */

void DumpData(char* ps8Buf, int s32BufLen, int s32ShowSize)
{
    printf("dump data(%p %d %d):", ps8Buf, s32BufLen, s32ShowSize);
    for (int s32Cnt = 0; s32Cnt < s32BufLen && s32Cnt < s32ShowSize; s32Cnt++)
    {
        if (s32Cnt % 8 == 0)
        {
            printf("\n");
        }
        printf("%3x", ps8Buf[s32Cnt]);
    }
    printf("\n");
}

/*
 * @brief calculate the frame size from audio format
 * @param pstFmt -IN: audio format that necessary for calculation
 * @return >0: Successed, return frame size.
 * @return -1: Failed.
 */

static int GetFrameSize(audio_fmt_t *pstFmt)
{
    int s32BitWidth;

    if (!pstFmt)
       return -1;

    s32BitWidth = ((pstFmt->bitwidth > 16) ? 32 : pstFmt->bitwidth);
    return (pstFmt->channels * (s32BitWidth >> 3) * pstFmt->sample_size);
}

/*
 * @brief show frames of mp3 file
 * @param ps8Mp3File -IN: mp3 file location
 * @return >=0: Successed
 * @return -1: Failed.
 */

int DemuxStream(char *ps8Mp3File)
{
    struct demux_t *pstDmx;
    struct demux_frame_t stFrm;
    int s32Ret = 0;
    int s32FrmCnt = 0;

    pstDmx = demux_init(ps8Mp3File);
    if(!pstDmx)
    {
        printf("demux %s failed\n", ps8Mp3File);
        return -1;
    }

    while (1)
    {
        s32Ret = demux_get_frame(pstDmx, &stFrm);
        printf("codec id:%d ret:%d\n", stFrm.codec_id, s32Ret);
        if (s32Ret == 0)
            continue;
        if (s32Ret < 0)
            break;
        if(stFrm.codec_id != DEMUX_AUDIO_MP3)
            goto skip;
        usleep(50000);

        DumpData((char*)stFrm.data, stFrm.data_size, 16);
        s32FrmCnt ++;
skip:
        demux_put_frame(pstDmx, &stFrm);
        if (s32FrmCnt >= MAX_FRM_CNT)
        {
            printf("frame count reached %d\n", s32FrmCnt);
            break;
        }
    }

    demux_deinit(pstDmx);
    return 0;
}

/*
 * @brief main function
 * @param s32Argc -IN: args' count
 * @param ps8Argv -IN: args
 * @return 0: Success.
 * @return -1: Failure.
 */

int main(int s32Argc, char** ps8Argv)
{
    int s32Ret = 0;
    int s32AudHandle = -1, s32PcmFd = -1;
    int s32Volume = 100, s32Len = 0;
    const char *ps8ChnName = PLAY_CHN_NAME;
    char ps8Mp3File[] = MP3_FILE_PATH;
    const char* ps8PcmFile = PCM_FILE_PATH;
    // audiobox
    struct fr_buf_info stBuf;
    audio_fmt_t stRealAudFmt;
    audio_fmt_t stAudFmt = {
        .channels = 2,
        .bitwidth = 16,
        .samplingrate = 8000,
        .sample_size = 1024,
    };
    codec_info_t stDecFmt = {
        .in = {
            .codec_type = AUDIO_CODEC_MP3,
            .effect = 0,
            .channel = 2,
            .sample_rate = 16000,
            .bitwidth = 16,
            .bit_rate = 512000,
        },
        .out = {
            .codec_type = AUDIO_CODEC_PCM,
            .effect = 0,
            .channel = 0,
            .sample_rate = 0,
            .bitwidth = 0,
            .bit_rate = 1024000,
        },
    };
    // demux
    struct demux_t *pstDmx = NULL;
    struct demux_frame_t stFrm;
    stream_info_t  *pstStream = NULL;
    // codec
    void *pDecHandle;
    codec_addr_t stDecAddr;

#if DEMUX_STREAM
    DemuxStream(ps8Mp3File);
#endif

    if (ps8PcmFile)
    {
        s32PcmFd = open(ps8PcmFile, O_WRONLY | O_CREAT | O_TRUNC);
        if (s32PcmFd < 0)
        {
            printf("Open pcm file failed: %d, current path: %s\n", s32PcmFd, ps8PcmFile);
        }
    }

    pstDmx = demux_init(ps8Mp3File);
    if (!pstDmx)
    {
        printf("demux %s failed\n", ps8Mp3File);
        return -1;
    }

    printf("stream num:%d\n", pstDmx->stream_num);

    //modify the information of decoder
    pstStream = pstDmx->stream_info;
    stDecFmt.in.channel = pstStream->channels;
    stDecFmt.in.sample_rate = pstStream->sample_rate;
    stDecFmt.in.bitwidth = pstStream->bit_width;
    stDecFmt.in.bit_rate = pstStream->channels * pstStream->sample_rate * pstStream->bit_width;

    // before we do anything involved in audio, daemon "audiobox" must exist in the system. please check it first

    printf("chn_name: %s\n"
           "volume: %d\n"
           "mp3file: %s\n"
           "fmt.in.codec_type: %d\n"
           "fmt.in.bitwidth: %d\n"
           "fmt.in.samplingrate: %d\n"
           "fmt.in.channels: %d\n"
           "fmt.in.bitrate: %d\n"
           "fmt.out.codec_type: %d\n"
           "fmt.out.bitwidth: %d\n"
           "fmt.out.samplingrate: %d\n"
           "fmt.out.channels: %d\n"
           "fmt.out.bitrate: %d\n",
           ps8ChnName, s32Volume, ps8Mp3File,
           stDecFmt.in.codec_type, stDecFmt.in.bitwidth, stDecFmt.in.sample_rate, stDecFmt.in.channel, stDecFmt.in.bit_rate,
           stDecFmt.out.codec_type, stDecFmt.out.bitwidth, stDecFmt.out.sample_rate, stDecFmt.out.channel, stDecFmt.out.bit_rate);

    // get volume of playback physical channel
    s32Ret = audio_get_master_volume(ps8ChnName, AUDIO_PLAYBACK);
    if (s32Ret < 0)
    {
        printf("Get volume err: %d\n", s32Ret);
        goto err_close_fd;
    }

    // set volume of playback physical channel
    printf("Get volume: %d\n", s32Ret);
    if (s32Ret != s32Volume)
    {
        s32Ret = audio_set_master_volume(ps8ChnName, s32Volume, AUDIO_PLAYBACK);
        if (s32Ret < 0) {
            printf("Set volume err: %d\n", s32Ret);
            goto err_close_fd;
        }
        printf("Set volume: %d\n", s32Volume);
    }

    // get playback channel with special audio format and priority
    s32AudHandle = audio_get_channel(ps8ChnName, &stAudFmt, CHANNEL_BACKGROUND);
    if (s32AudHandle < 0)
    {
        printf("Get channel err: %d\n", s32AudHandle);
        goto err_close_fd;
    }

    // get real audio format back
    s32Ret = audio_get_format(ps8ChnName, &stRealAudFmt);
    if (s32Ret < 0)
    {
        printf("Get format err: %d\n", s32Ret);
        audio_put_channel(s32AudHandle);
        goto err_put_chn;
    }

    // open decoder
    stDecFmt.out.bitwidth = stAudFmt.bitwidth;
    stDecFmt.out.channel = stAudFmt.channels;
    stDecFmt.out.sample_rate = stAudFmt.samplingrate;
    pDecHandle = codec_open(&stDecFmt);
    if (!pDecHandle)
    {
        printf("Open encoder err\n");
        goto err_put_chn;
    }

    stDecAddr.len_out = GetFrameSize(&stRealAudFmt) * 2;  //output size: frame size of PCM, get bigger size just in case
    stDecAddr.out = (char *)malloc(stDecAddr.len_out);	    //output buf: allocate a temporary buffer for output
    if (stDecAddr.out == NULL)
    {
        printf("no enough memory for decode output\n");
        goto err_close_codec;
    }

    // read G711A data from wav file, decode it as PCM format, and write it to Audiobox until end of file
    fr_INITBUF(&stBuf, NULL);

    system("date");
    do {
        s32Ret = demux_get_frame(pstDmx, &stFrm);
        if (s32Ret == 0)
        {
            continue;
        }
        if (s32Ret < 0)
        {
            printf("get frame ret error:%d\n", s32Ret);
            break;
        }
        if (stFrm.codec_id != DEMUX_AUDIO_MP3)
        {
            printf("codec id %d error\n", stFrm.codec_id);
            goto err_close_codec;
        }

        stDecAddr.len_in = stFrm.data_size;
        stDecAddr.in = stFrm.data;

        // decode from MP3 to PCM
        s32Ret = codec_decode(pDecHandle, &stDecAddr);
        if (s32Ret < 0)
        {
            break;
        }
        if (s32Ret == 0)
        {
            demux_put_frame(pstDmx, &stFrm);
            continue;
        }

        demux_put_frame(pstDmx, &stFrm);
        if (s32PcmFd > 0)
        {
            if (write(s32PcmFd, stDecAddr.out, s32Ret) != s32Ret)
            {
                printf("Dump audio pcm error\n");
            }
        }

        // write PCM data to Audiobox
        s32Ret = audio_write_frame(s32AudHandle, (char *)stDecAddr.out, s32Ret);
        if (s32Ret < 0)
        {
            break;
        }
    } while (1);

    // close decoder
    do {
        s32Ret = codec_flush(pDecHandle, &stDecAddr, &s32Len);
        if ((s32Ret == CODEC_FLUSH_ERR) || (s32Ret == CODEC_FLUSH_FINISH))
            break;

        if (s32PcmFd > 0)
        {
            if (write(s32PcmFd, stDecAddr.out, s32Ret) != s32Ret)
            {
                printf("Dump audio pcm error\n");
            }
        }
        // write PCM data to Audiobox
        s32Ret = audio_write_frame(s32AudHandle, (char *)stDecAddr.out, s32Ret);
        if (s32Ret < 0)
        {
            break;
        }
    } while (s32Ret == CODEC_FLUSH_AGAIN);
    system("date");
    codec_close(pDecHandle);
    demux_deinit(pstDmx);

    // free temporary buffer for encoder output
    if(stDecAddr.out)
    {
        free(stDecAddr.out);
        stDecAddr.out = NULL;
    }

    // put playback channel back
    audio_put_channel(s32AudHandle);

    // close PCM wav file
    if (s32PcmFd >= 0)
    {
        close(s32PcmFd);
        s32PcmFd = -1;
    }

    return 0;

err_close_codec:
    // close decoder
    if (pDecHandle != NULL)
    {
       codec_close(pDecHandle);
       pDecHandle = NULL;
    }

err_put_chn:
    // put playback channel back
    if (s32AudHandle >= 0)
    {
       audio_put_channel(s32AudHandle);
       s32AudHandle = -1;
    }

    if (pstDmx)
    {
        demux_deinit(pstDmx);
        pstDmx = NULL;
    }

err_close_fd:
    // close PCM file
    if (s32PcmFd >= 0)
    {
       close(s32PcmFd);
       s32PcmFd = -1;
    }

    return -1;
}
