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
 * Description: PCM record demo
 *
 * Author:
 *     ben.xiao <ben.xiao@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/25/2017 init
 *
 * TODO:
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <fr/libfr.h>
#include <qsdk/audiobox.h>
#include <qsdk/event.h>

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

#define RECORD_CHN_NAME "default_mic"
#define WAV_FILE_PATH   "/mnt/sd0/record_pcm.wav"

#define FORMAT_PCM 1

struct wav_header {
    uint32_t u32RiffId;
    uint32_t u32RiffSz;
    uint32_t u32RiffFmt;
    uint32_t u32FmtId;
    uint32_t u32FmtSz;
    uint16_t u16AudioFormat;
    uint16_t u16NumChannels;
    uint32_t u32SampleRate;
    uint32_t u32ByteRate;
    uint16_t u16BlockAlign;
    uint16_t u16BitsPerSample;
    uint32_t u32DataId;
    uint32_t u32DataSz;
};

/*
 * @brief main function
 * @param s32Argc -IN: args' count
 * @param ps8Argv -IN: args
 * @return 0: Success.
 * @return -1: Failure.
 */

int main(int s32Argc, char** ps8Argv)
{
    int s32Ret;
    int s32Handle = -1, s32Fd = -1;
    int s32Volume = 100, s32Time = 10; // record for 10s
    int s32Len, s32TotalLen, s32NeededLen;
    struct wav_header stHeader;
    const char *ps8ChnName = RECORD_CHN_NAME;
    const char *ps8WavFile = WAV_FILE_PATH;
    struct fr_buf_info stBuf;
    audio_fmt_t stFmt = {
        .channels = 2,
        .bitwidth = 32,
        .samplingrate = 48000,
        .sample_size = 1024,
    };
    audio_fmt_t stRealFmt;

    // open PCM wav file
    if (ps8WavFile) {
        s32Fd = open(ps8WavFile, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (s32Fd < 0) {
            printf("Open wav file failed: %d! Please input right path! current path: %s\n", s32Fd, ps8WavFile);
            return -1;
        }
    }

    // add the head for wav
    stHeader.u32RiffId = ID_RIFF;
    stHeader.u32RiffSz = 0;
    stHeader.u32RiffFmt = ID_WAVE;
    stHeader.u32FmtId = ID_FMT;
    stHeader.u32FmtSz = 16;
    stHeader.u16AudioFormat = FORMAT_PCM;
    stHeader.u16NumChannels = stFmt.channels;
    stHeader.u32SampleRate = stFmt.samplingrate;
    stHeader.u16BitsPerSample = (stFmt.bitwidth == 16) ? 16 : 32;
    stHeader.u32ByteRate = (stHeader.u16BitsPerSample / 8) * stFmt.channels * stFmt.samplingrate;
    stHeader.u16BlockAlign = stFmt.channels * (stHeader.u16BitsPerSample / 8);
    stHeader.u32DataId = ID_DATA;
    stHeader.u32DataSz = stHeader.u32SampleRate * stHeader.u16BlockAlign * s32Time;
    stHeader.u32RiffSz = stHeader.u32DataSz + sizeof(stHeader) - 8;

    s32Ret = write(s32Fd, &stHeader, sizeof(struct wav_header));

    s32NeededLen = stHeader.u32DataSz;

    // before we do anything involved in audio, daemon "audiobox" must exist in the system. please check it first

    printf("pcm_name: %s\n"
           "volume: %d\n"
           "wavfile: %s\n"
           "filesize: %d\n"
           "fmt.bitwidth: %d\n"
           "fmt.samplingrate: %d\n"
           "fmt.channels: %d\n"
           "time: %ds\n",
           ps8ChnName, s32Volume, ps8WavFile, s32NeededLen, stFmt.bitwidth, 
           stFmt.samplingrate, stFmt.channels, s32Time);

    // get volume of capture physical channel
    s32Ret = audio_get_master_volume(ps8ChnName, AUDIO_CAPTURE);
    if (s32Ret < 0) {
        printf("Get volume err: %d\n", s32Ret);
        goto err_close_fd;
    }

    // set volume of capture physical channel
    printf("Get volume: %d\n", s32Ret);
    if (s32Ret != s32Volume) {
        s32Ret = audio_set_master_volume(ps8ChnName, s32Volume, AUDIO_CAPTURE);
        if (s32Ret < 0) {
            printf("Set volume err: %d\n", s32Ret);
            goto err_close_fd;
        }
        printf("Set volume: %d\n", s32Volume);
    }

    // get capture channel with special audio format and priority
    s32Handle = audio_get_channel(ps8ChnName, &stFmt, CHANNEL_BACKGROUND);
    if (s32Handle < 0) {
        printf("Get channel err: %d\n", s32Handle);
        goto err_close_fd;
    }

    // get real audio format back
    s32Ret = audio_get_format(ps8ChnName, &stRealFmt);
    if (s32Ret < 0) {
        printf("Get format err: %d\n", s32Ret);
        goto err_put_chn;
    }

    // read from Audiobox and write it to PCM wav file until record timeout(10s)
    s32TotalLen = 0;
    fr_INITBUF(&stBuf, NULL);
    do {
        // get buf reference with PCM data from Audiobox
        s32Ret = audio_get_frame(s32Handle, &stBuf);
        if (s32Ret < 0)
            continue;

        // write PCM data to file
        s32Len = write(s32Fd, stBuf.virt_addr, stBuf.size);
        if (s32Len < 0) {
            audio_put_frame(s32Handle, &stBuf);
            break;
        }

        // put buf reference back to Audiobox
        audio_put_frame(s32Handle, &stBuf);

        s32TotalLen += s32Len;
    } while (s32TotalLen < s32NeededLen);

    // put capture channel back
    audio_put_channel(s32Handle);

    // close PCM wav file
    if (s32Fd >= 0)
        close(s32Fd);

    return 0;

err_put_chn:
    // put playback channel back
    if (s32Handle >= 0)
        audio_put_channel(s32Handle);

err_close_fd:
    // close PCM wav file
    if (s32Fd >= 0)
        close(s32Fd);

    return -1;
}
