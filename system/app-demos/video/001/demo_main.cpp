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
 * Description: Take photo demo for H1Jpeg
 *
 * Author:
 *     devin.zhu <devin.zhu@infotm.com>
 *
 * Revision History:
 * -----------------
 * 1.1  08/14/2017 init
 */


#include <stdio.h>
#include <unistd.h>
#include <qsdk/videobox.h>

#define PATH "/mnt/sd0/photo.jpg"
#define PHOTOCHANNEL "jpeg-out"
#define ISPCHANNEL "isp"
#define ERROR(args...) printf(args)

/*
 * @brief Take and save photo
 * @param  pFilename -IN: path to save photo
 * @param  pChannel  -IN: encoded data channel
 * @return 0: Success
 * @return -1: Failure
 */
int TakePhoto(const char *pFilename, const char *pChannel)
{
    struct fr_buf_info stBuf;
    FILE  *pFileHandler = NULL;
    int32_t s32Ret = -1;
    int32_t s32FileSize = 0;

    if (pFilename == NULL || pChannel == NULL)
    {
        ERROR("Error:argument is null!\n");
        return s32Ret;
    }

    pFileHandler = fopen(pFilename, "wb+");
    if (pFileHandler == NULL)
    {
        ERROR("Error:open %s fail!\n",pFilename);
        goto err;
    }

    s32Ret = camera_snap_one_shot(ISPCHANNEL);
    if (s32Ret < 0)
    {
        ERROR("Error:trigger isp %s fail!\n",ISPCHANNEL);
        goto err;
    }

    s32Ret = video_get_snap(pChannel, &stBuf);
    if (s32Ret < 0)
    {
        ERROR("Error:take photo fail!\n");
        goto err;
    }

    s32FileSize = fwrite(stBuf.virt_addr, stBuf.size, 1, pFileHandler);

    if (s32FileSize != 1)
    {
        ERROR("Error:save %s fail!\n",pFilename);
        video_put_snap(pChannel, &stBuf);
        goto err;
    }

    s32Ret = video_put_snap(pChannel, &stBuf);
    if (s32Ret < 0)
    {
        ERROR("Error:take photo fail!\n");
        goto err;
    }

    s32Ret = 0;
    fclose(pFileHandler);
    camera_snap_exit(ISPCHANNEL);
    return s32Ret;
err:
    if (pFileHandler)
    {
        fclose(pFileHandler);
        pFileHandler = NULL;
    }
    camera_snap_exit(ISPCHANNEL);
    return s32Ret;
}

/*
 * @brief main function
 * @param  void
 * @return 0: Success
 * @return -1: Failure
 */
int main()
{
    int32_t s32Ret = 0;

    videobox_repath("./path.json");

    s32Ret = TakePhoto(PATH, PHOTOCHANNEL);

    videobox_stop();

    return s32Ret;
}

