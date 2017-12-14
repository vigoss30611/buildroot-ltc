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

#include <stdio.h>
#include <string.h>
#include <qsdk/videobox.h>
#include <signal.h>
#include <errno.h>
#include <sys/shm.h>
#include <errno.h>

#if 0
#include <sys/wait.h>
#endif


typedef void (*sighandler_t)(int);
static int vb_system(char *cmdstring)
{
    int status = 0;
    sighandler_t old_handler = NULL;


    if (NULL == cmdstring)
    {
        return -1;
    }

    old_handler = signal(SIGCHLD, SIG_DFL);
    status = system(cmdstring);
    signal(SIGCHLD, old_handler);

//  printf("cmd: status=%d  old_handler=%p  error: %s \n", status, old_handler,  strerror(errno));

    if (status < 0)
    {
        printf("(%s:L%d) cmd:%s error:%s\n", __func__, __LINE__, cmdstring, strerror(errno));
        return status;
    }

#if 0
    if(WIFEXITED(status))
    {
        printf("normal termination, exit status = %d\n", WEXITSTATUS(status));
    }
    else if(WIFSIGNALED(status))
    {
        printf("abnormal termination,signal number =%d\n", WTERMSIG(status));
    }
    else if(WIFSTOPPED(status))
    {
        printf("process stopped, signal number =%d\n", WSTOPSIG(status));
    }
#endif

    return 0;
}

int videobox_poll(void) {
    videobox_launch_rpc_l("none", NULL, 0);
    return _rpc.ret;
}

int videobox_start(const char *js)
{
    char cmd[256];

    sprintf(cmd, "videoboxd %s", js? js: " ");
    vb_system(cmd);

    return 0;
}

int videobox_stop(void)
{
    printf("stopping videobox ...\n");
    vb_system("killall videoboxd");

    while(system("killall -q -0 videoboxd") == 0);
    printf("stopped\n");

    return 0;
}

int videobox_rebind(const char *p0, const char *p2)
{
    char buf[32];
    strncpy(buf, p2, 32);
    buf[31] = 0;

    printf("%s: %s -> %s\n", __func__, p0, buf);
    videobox_launch_rpc_l(p0, buf, 32);
    return _rpc.ret;
}

int videobox_repath(const char *js)
{
    char buf[128];
#if 0
    strncpy(buf, js, 128);
    buf[127] = 0;

    videobox_launch_rpc_l("none", buf, 128);
    return _rpc.ret;
#endif
    /* FIXME: this is a workaround */
    videobox_stop();
    usleep(30000);
    sprintf(buf, "videoboxd %s", js);

    return vb_system(buf);
}

int videobox_control(const char *ipu, int code, int para)
{
    vbctrl_t c = {
        .code = code,
        .para = para,
    };
    videobox_launch_rpc(ipu, &c);
    return _rpc.ret;
}

int videobox_mod_offset(const char *ipu, int x, int y)
{
    vbres_t offset = {
        .w = x,
        .h = y,
    };
    videobox_launch_rpc(ipu, &offset);
    return _rpc.ret;
}

int videobox_get_jsonpath(void *buf, int len)
{
    videobox_launch_rpc_l("none", buf, len);
    return _rpc.ret;
}

int videobox_get_debug_info(const char *ps8SubType, void *pInfo, const int s32MemSize)
{
    return videobox_exchange_data("ipu_info", ps8SubType, pInfo, s32MemSize);
}

int videobox_exchange_data(const char *ps8Type, const char *ps8SubType, void *pInfo, const int s32MemSize)
{
    ST_SHM_INFO stShmInfo;
    struct event_scatter stEventScatter;
    int s32Ret = 0;
    int s32Key = 0;
    int s32ShmId = 0;
    int s32ShmSize = 0;
    char *pMap = NULL;
    char ps8ShmPath[] = "/usr/bin";

    strncpy(stShmInfo.ps8Type, ps8Type, TYPE_LENGTH);
    strncpy(stShmInfo.ps8SubType, ps8SubType, SUB_TYPE_LENGTH);
    s32ShmSize = s32MemSize;

    s32Key = ftok(ps8ShmPath, 0x05);

    if (s32Key == -1)
    {
        printf("(%s:L%d) ftok is error (path_name=%s) errono:%s!!!\n",
            __func__, __LINE__, ps8ShmPath, strerror(errno)) ;
        return -1;
    }

    s32ShmId = shmget(s32Key, s32ShmSize, IPC_CREAT | IPC_EXCL | 0666);
    if (s32ShmId == -1)
    {
        s32ShmId = shmget(s32Key, s32ShmSize, 0666);
        if (s32ShmId == -1)
        {
            printf("(%s:L%d) shmget is error (key=%d, size=%d) errono:%s!!!\n",
                __func__, __LINE__, s32Key, s32ShmSize, strerror(errno)) ;
            return -1;
        }
    }

    pMap = shmat(s32ShmId, NULL, 0);
    if ((int)pMap == -1)
    {
        printf("(%s:L%d) p_map is NULL (shm_id=%d) errono:%s !!!\n",
            __func__, __LINE__, s32ShmId, strerror(errno));
        return -1;
    }
    else
    {
        memcpy(pMap, (char *)pInfo, s32ShmSize);
    }

    stShmInfo.s32Key = s32Key;
    stShmInfo.s32Size = s32ShmSize;

    stEventScatter.buf = &stShmInfo;
    stEventScatter.len = sizeof(stShmInfo);
    s32Ret = event_rpc_call_scatter(VB_RPC_INFO, &stEventScatter, 1);
    if (s32Ret < 0)
    {
        printf("(%s:L%d) get debug info fail\n", __func__, __LINE__);
        s32Ret = -1;
    }
    else
    {
        memcpy(pInfo, pMap, s32ShmSize);
    }


    if (shmdt(pMap) == -1)
    {
        printf("(%s:L%d) shmdt error (p_map=%p) !!!\n", __func__, __LINE__, pMap);
        s32Ret = -1;
    }

    if (shmctl(s32ShmId, IPC_RMID, NULL) == -1)
    {
        printf("(%s:L%d) shmctl error (shm_id=%d) !!!\n", __func__, __LINE__, s32ShmId);
        s32Ret = -1;
    }

    return s32Ret;
}

