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
 * Description: record & encode demo
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
#include <qsdk/codecs.h>

#define ID_RIFF 0x46464952 //"RIFF"
#define ID_WAVE 0x45564157 //"WAVE"
#define ID_FMT  0x20746d66 //"fmt"
#define ID_DATA 0x61746164 //"data"

#define RECORD_CHN_NAME "default_mic"
#define WAV_FILE_PATH   "/mnt/sd0/record_g711a.wav"

#define FORMAT_G711A 6

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
 * @brief calculate the frame size from audio format
 * @param pstFmt -IN: audio format that necessary for calculation
 * @return >0: Successed, return frame size.
 * @return -1: Failed.
 */

static int get_frame_size(audio_fmt_t *pstFmt)
{
    int s32BitWidth;

    if (!pstFmt)
       return -1;

    s32BitWidth = ((pstFmt->bitwidth > 16) ? 32 : pstFmt->bitwidth);
    return (pstFmt->channels * (s32BitWidth >> 3) * pstFmt->sample_size);
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
    int s32Ret;
    int s32AudHandle = -1, s32Fd = -1;
    int s32Volume = 100, s32Time = 10; // record for 10s
    int s32Len, s32TotalLen, s32NeededLen;
    struct wav_header stHeader;
    const char *ps8ChnName = RECORD_CHN_NAME;
    const char *ps8WavFile = WAV_FILE_PATH;
    struct fr_buf_info stBuf;
    audio_fmt_t stAudFmt = {
        .channels = 2,
        .bitwidth = 32,
        .samplingrate = 16000,
        .sample_size = 1024,
    };
    audio_fmt_t stRealAudFmt;

    void *pEncHandle;
    codec_addr_t stEncAddr;
    codec_info_t stEncFmt = {
        .in = {
            .codec_type = AUDIO_CODEC_PCM,
            .effect = 0,
            .channel = 2,
            .sample_rate = 16000,
            .bitwidth = 32,
            .bit_rate = 1024000,
        },
        .out = {
            .codec_type = AUDIO_CODEC_G711A,
            .effect = 0,
            .channel = 2,
            .sample_rate = 16000,
            .bitwidth = 16,
            .bit_rate = 512000,
        },
    };

    // open G711A wav file
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
    stHeader.u16AudioFormat = FORMAT_G711A;
    stHeader.u16NumChannels = stEncFmt.out.channel;
    stHeader.u32SampleRate = stEncFmt.out.sample_rate;
    stHeader.u16BitsPerSample = (stEncFmt.out.bitwidth == 16) ? 16 : 32;
    stHeader.u32ByteRate = (stHeader.u16BitsPerSample / 8) * stEncFmt.out.channel * stEncFmt.out.sample_rate;
    stHeader.u16BlockAlign = stEncFmt.out.channel * (stHeader.u16BitsPerSample / 8);
    stHeader.u32DataId = ID_DATA;
    stHeader.u32DataSz = stHeader.u32SampleRate * stHeader.u16BlockAlign * s32Time / 2; //for G711A, ratio is 0.5, half of the data size will be spared
    stHeader.u32RiffSz = stHeader.u32DataSz + sizeof(stHeader) - 8;

    s32Ret = write(s32Fd, &stHeader, sizeof(struct wav_header));

    s32NeededLen = stHeader.u32DataSz;

    // before we do anything involved in audio, daemon "audiobox" must exist in the system. please check it first

    printf("chn_name: %s\n"
           "volume: %d\n"
           "time: %ds\n"
           "wavfile: %s\n"
           "filesize: %d\n"
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
           ps8ChnName, s32Volume, s32Time, ps8WavFile, s32NeededLen,
           stEncFmt.in.codec_type, stEncFmt.in.bitwidth, stEncFmt.in.sample_rate, stEncFmt.in.channel, stEncFmt.in.bit_rate,
           stEncFmt.out.codec_type, stEncFmt.out.bitwidth, stEncFmt.out.sample_rate, stEncFmt.out.channel, stEncFmt.out.bit_rate);

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
    s32AudHandle = audio_get_channel(ps8ChnName, &stAudFmt, CHANNEL_BACKGROUND);
    if (s32AudHandle < 0) {
        printf("Get channel err: %d\n", s32AudHandle);
        return -1;
    }

    // get real audio format back
    s32Ret = audio_get_format(ps8ChnName, &stRealAudFmt);
    if (s32Ret < 0) {
        printf("Get format err: %d\n", s32Ret);
        goto err_put_chn;
    }

    // open encoder
    pEncHandle = codec_open(&stEncFmt);
    if (!pEncHandle) {
        printf("Open encoder err\n");
        goto err_put_chn;
    }

    // encoder input & output information
    stEncAddr.len_in = get_frame_size(&stRealAudFmt);   //input size: frame size of PCM
    stEncAddr.in = NULL;								//input buf: will be stuffed when reading PCM audio data
    stEncAddr.len_out = stEncAddr.len_in;               //output size: for G711A, ratio is 0.5, so (len_in / 2) is enough. But use (len_in) just in case.
    stEncAddr.out = (char *)malloc(stEncAddr.len_out);	//output buf: allocate a temporary buffer for output
    if (stEncAddr.out == NULL) {
        printf("no enough memory for encode output\n");
        goto err_close_codec;
    }

    // read PCM data from Audiobox, encode it as G711A format, and write it to G711A wav file until record timeout(10s)
    s32TotalLen = 0;
    fr_INITBUF(&stBuf, NULL);
    do {
        // get buf reference with PCM data from Audiobox
        s32Ret = audio_get_frame(s32AudHandle, &stBuf);
        if (s32Ret < 0)
            continue;

        // encode from PCM to G711A
        stEncAddr.in = stBuf.virt_addr;
        stEncAddr.len_in = stBuf.size;
        s32Ret = codec_encode(pEncHandle, &stEncAddr);
        if (s32Ret < 0)
            break;

        // write G711A data to file
        s32Len = write(s32Fd, stEncAddr.out, s32Ret);
        if (s32Len < 0)
            break;

        // put buf reference back to Audiobox
        audio_put_frame(s32AudHandle, &stBuf);

        s32TotalLen += s32Len;
    } while (s32TotalLen < s32NeededLen);

    // close encoder
    do {
        s32Ret = codec_flush(pEncHandle, &stEncAddr, &s32Len);
        if (s32Ret == CODEC_FLUSH_ERR)
            break;

        if (write(s32Fd, stEncAddr.out, s32Len) != s32Len)
            break;
    } while (s32Ret == CODEC_FLUSH_AGAIN);
    codec_close(pEncHandle);

    // free temporary buffer for encoder output
    if(stEncAddr.out) {
        free(stEncAddr.out);
    }

    // close PCM wav file
    if (s32Fd >= 0)
        close(s32Fd);

    // put capture channel back
    audio_put_channel(s32AudHandle);
    return 0;

err_close_codec:
    // close decoder
    if (pEncHandle != NULL)
        codec_close(pEncHandle);

err_put_chn:
    // put playback channel back
    if (s32AudHandle >= 0)
       audio_put_channel(s32AudHandle);

err_close_fd:
    // close PCM wav file
    if (s32Fd >= 0)
       close(s32Fd);

    return -1;
}
