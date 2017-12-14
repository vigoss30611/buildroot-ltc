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
#include <memory.h>
#include <fr/libfr.h>
#include <qsdk/audiobox.h>
#include <qsdk/event.h>
#include <qsdk/codecs.h>

#define ID_RIFF 0x46464952 //"RIFF"
#define ID_WAVE 0x45564157 //"WAVE"
#define ID_FMT  0x20746d66 //"fmt"
#define ID_DATA 0x61746164 //"data"

#define PLAY_CHN_NAME   "default"
#define WAV_FILE_PATH   "/mnt/sd0/play_g711a.wav"

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
 * @brief read data from file safely
 * @param s32Fd    -IN: file description index to the file should be read
 * @param pvoidBuf -IN: pointer of buffer that data will put into
 * @param s32Count -IN: bytes of data will be read from file 
 * @return >0: Successed, return bytes of data read from file.
 * @return =0: Failed, no data is read from file.
 */

static int safe_read(int s32Fd, void *pBuf, int s32Count)
{
    int s32Result = 0, s32Res = 0;

    memset(pBuf, 0, s32Count);
    while (s32Count > 0) {
        if ((s32Res = read(s32Fd, pBuf, s32Count)) == 0)
            break;
        if (s32Res < 0)
            return s32Result > 0 ? s32Result : s32Res;
        s32Count -= s32Res;
        s32Result += s32Res;
        pBuf = (char *)pBuf + s32Res;
    }

    return s32Result;
}

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
    int s32Volume = 100;
    int s32Len, s32TotalLen, s32FileLen;
    struct wav_header stHeader;
    const char *ps8ChnName = PLAY_CHN_NAME;
    const char *ps8WavFile = WAV_FILE_PATH;
    struct fr_buf_info stBuf;
    audio_fmt_t stAudFmt = {
        .channels = 2,
        .bitwidth = 32,
        .samplingrate = 16000,
        .sample_size = 1024,
    };
    audio_fmt_t stRealAudFmt;

    void *pDecHandle;
    codec_addr_t stDecAddr;
    codec_info_t stDecFmt = {
        .in = {
            .codec_type = AUDIO_CODEC_G711A,
            .effect = 0,
            .channel = 2,
            .sample_rate = 16000,
            .bitwidth = 16,
            .bit_rate = 512000,
        },
        .out = {
            .codec_type = AUDIO_CODEC_PCM,
            .effect = 0,
            .channel = 2,
            .sample_rate = 16000,
            .bitwidth = 32,
            .bit_rate = 1024000,
        },
    };

    // open PCM wav file
    if (ps8WavFile) {
        s32Fd = open(ps8WavFile, O_RDONLY);
        if (s32Fd < 0) {
            printf("Open wav file failed: %d! Please input right path! current path: %s\n", s32Fd, ps8WavFile);
            return -1;
        }
    }

    // get the head for wav
    s32Ret = read(s32Fd, &stHeader, sizeof(struct wav_header));

    //for this demo, only support G711A
    if(stHeader.u16AudioFormat != FORMAT_G711A) {
        printf("no a G711A wav file! Please input right path! current path: %s\n", ps8WavFile);
        goto err_close_fd;
    }

    //modify the information of decoder
    stDecFmt.in.channel = stHeader.u16NumChannels;
    stDecFmt.in.sample_rate = stHeader.u32SampleRate;
    stDecFmt.in.bitwidth = stHeader.u16BitsPerSample;
    stDecFmt.in.bit_rate = stHeader.u16NumChannels * stHeader.u32SampleRate * stHeader.u16BitsPerSample;
    s32FileLen = stHeader.u32DataSz;

    // before we do anything involved in audio, daemon "audiobox" must exist in the system. please check it first

    printf("chn_name: %s\n"
           "volume: %d\n"
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
           ps8ChnName, s32Volume, ps8WavFile, s32FileLen,
           stDecFmt.in.codec_type, stDecFmt.in.bitwidth, stDecFmt.in.sample_rate, stDecFmt.in.channel, stDecFmt.in.bit_rate,
           stDecFmt.out.codec_type, stDecFmt.out.bitwidth, stDecFmt.out.sample_rate, stDecFmt.out.channel, stDecFmt.out.bit_rate);

    // get volume of playback physical channel
    s32Ret = audio_get_master_volume(ps8ChnName, AUDIO_PLAYBACK);
    if (s32Ret < 0) {
        printf("Get volume err: %d\n", s32Ret);
        audio_put_channel(s32AudHandle);
        goto err_close_fd;
    }

    // set volume of playback physical channel
    printf("Get volume: %d\n", s32Ret);
    if (s32Ret != s32Volume) {
        s32Ret = audio_set_master_volume(ps8ChnName, s32Volume, AUDIO_PLAYBACK);
        if (s32Ret < 0) {
            printf("Set volume err: %d\n", s32Ret);
            audio_put_channel(s32AudHandle);
            goto err_close_fd;
        }
        printf("Set volume: %d\n", s32Volume);
    }

    // get playback channel with special audio format and priority
    s32AudHandle = audio_get_channel(ps8ChnName, &stAudFmt, CHANNEL_BACKGROUND);
    if (s32AudHandle < 0) {
        printf("Get channel err: %d\n", s32AudHandle);
        goto err_close_fd;
    }

    // get real audio format back
    s32Ret = audio_get_format(ps8ChnName, &stRealAudFmt);
    if (s32Ret < 0) {
        printf("Get format err: %d\n", s32Ret);
        audio_put_channel(s32AudHandle);
        goto err_put_chn;
    }

    // open decoder
    pDecHandle = codec_open(&stDecFmt);
    if (!pDecHandle) {
        printf("Open encoder err\n");
        goto err_put_chn;
    }

    // decoder input & output information
    stDecAddr.len_in = get_frame_size(&stRealAudFmt) / 2;   //input size: for G711A, ratio is 0.5, so (len_out / 2) is enough
    stDecAddr.in = (char *)malloc(stDecAddr.len_in);	    //input buf: allocate a temporary buffer for input
    if (stDecAddr.in == NULL) {
        printf("no enough memory for decode input\n");
        goto err_close_codec;
    }

    stDecAddr.len_out = get_frame_size(&stRealAudFmt) * 2;  //output size: frame size of PCM, get bigger size just in case
    stDecAddr.out = (char *)malloc(stDecAddr.len_out);	    //output buf: allocate a temporary buffer for output
    if (stDecAddr.out == NULL) {
        printf("no enough memory for decode output\n");
        goto err_close_codec;
    }

    // read G711A data from wav file, decode it as PCM format, and write it to Audiobox until end of file
    s32TotalLen = 0;
    fr_INITBUF(&stBuf, NULL);
    do {
        // read G711A data from file
        s32Len = safe_read(s32Fd, stDecAddr.in, stDecAddr.len_in);
        if (s32Len < 0) {
            break;
        }

        // decode from G711A to PCM
        s32Ret = codec_decode(pDecHandle, &stDecAddr);
        if (s32Ret < 0) {
            break;
        }

        // write PCM data to Audiobox
        s32Ret = audio_write_frame(s32AudHandle, (char *)stDecAddr.out, s32Ret);
        if (s32Ret < 0) {
            break;
        }

        s32TotalLen += s32Len;
    } while (s32TotalLen < s32FileLen);

    // close decoder
    do {
        s32Ret = codec_flush(pDecHandle, &stDecAddr, &s32Len);
        if (s32Ret == CODEC_FLUSH_ERR)
            break;

        if (write(s32Fd, stDecAddr.out, s32Len) != s32Len)
            break;
    } while (s32Ret == CODEC_FLUSH_AGAIN);
    codec_close(pDecHandle);

    // free temporary buffer for encoder output
    if(stDecAddr.in) {
        free(stDecAddr.in);
    }

    // put playback channel back
    audio_put_channel(s32AudHandle);

    // close PCM wav file
    if (s32Fd >= 0)
        close(s32Fd);

    return 0;

err_close_codec:
    // close decoder
    if (pDecHandle != NULL)
       codec_close(pDecHandle);

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
