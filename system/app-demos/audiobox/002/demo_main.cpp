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
 * Description: PCM play demo
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

#define PLAY_CHN_NAME   "default"
#define WAV_FILE_PATH   "/mnt/sd0/play_pcm.wav"

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
    int s32Handle = -1, s32Fd = -1;
    int s32Volume = 100;
    int s32Len, s32FileLen, s32TotalLen;
    struct wav_header stHeader;
    const char *ps8ChnName = PLAY_CHN_NAME;
    const char *ps8WavFile = WAV_FILE_PATH;
    int s32AudBufSize = 0;
    char * pAudBuf = NULL;
    audio_fmt_t stFmt = {
        .channels = 2,
        .bitwidth = 32,
        .samplingrate = 48000,
        .sample_size = 1024,
    };
    audio_fmt_t stRealFmt;

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

    stFmt.channels = stHeader.u16NumChannels;
    stFmt.samplingrate = stHeader.u32SampleRate;
    stFmt.bitwidth = stHeader.u16BitsPerSample;

    s32FileLen = stHeader.u32DataSz;

    // before we do anything involved in audio, daemon "audiobox" must exist in the system. please check it first

    printf("chn_name: %s\n"
           "volume: %d\n"
           "wavfile: %s\n"
           "filesize: %d\n"
           "fmt.bitwidth: %d\n"
           "fmt.samplingrate: %d\n"
           "fmt.channels: %d\n"
           "fmt.sample_size: %d\n",
           ps8ChnName, s32Volume, ps8WavFile, s32FileLen, stFmt.bitwidth, stFmt.samplingrate, stFmt.channels, stFmt.sample_size);

    // get volume of playback physical channel
    s32Ret = audio_get_master_volume(ps8ChnName, AUDIO_PLAYBACK);
    if (s32Ret < 0) {
        printf("Get volume err: %d\n", s32Ret);
        goto err_close_fd;
    }
    printf("Get volume: %d\n", s32Ret);

    // set volume of playback physical channel
    if (s32Ret != s32Volume) {
        s32Ret = audio_set_master_volume(ps8ChnName, s32Volume, AUDIO_PLAYBACK);
        if (s32Ret < 0) {
           printf("Set volume err: %d\n", s32Ret);
           goto err_close_fd;
        }
        printf("Set volume: %d\n", s32Volume);
    }

    // get playback channel with special audio format and priority
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

    // allocate temporary memory for PCM data
    s32AudBufSize = get_frame_size(&stRealFmt);
    pAudBuf = (char *)malloc(s32AudBufSize);
    if (pAudBuf == NULL) {
        printf("no enough memory for PCM data\n");
        goto err_put_chn;
    }

    // read from PCM wav file and write it to Audiobox until end of file
    s32TotalLen = 0;
    do {
        // read PCM data from file
        s32Len = safe_read(s32Fd, pAudBuf, s32AudBufSize);
        if (s32Len < 0) {
            break;
        }

        // write PCM data to Audiobox
        s32Ret = audio_write_frame(s32Handle, pAudBuf, s32Len);
        if (s32Ret < 0) {
            break;
        }

        s32TotalLen += s32Len;
    } while (s32TotalLen < s32FileLen);

    // put playback channel back
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
