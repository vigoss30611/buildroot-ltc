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

#include <System.h>
#include <Path.h>

using namespace std;

Path VideoBox;

void RegisterAllIPU()
{
#ifdef COMPILE_IPU_DGFRAME
    REGISTER_IPU(DIPU_DGFrame);
#endif
#ifdef COMPILE_IPU_DGMATH
    REGISTER_IPU(DIPU_DGMath);
#endif
#ifdef COMPILE_IPU_DGPIXEL
    REGISTER_IPU(DIPU_DGPixel);
#endif
#ifdef COMPILE_IPU_DGOV
    REGISTER_IPU(DIPU_DGOverlay);
#endif
#ifdef COMPILE_IPU_V2500
    REGISTER_IPU(DIPU_V2500);
#endif
#ifdef COMPILE_IPU_V2505
    REGISTER_IPU(DIPU_V2500);
#endif
#ifdef COMPILE_IPU_ISP
    REGISTER_IPU(DIPU_ISP);
#endif
#ifdef COMPILE_IPU_V4L2
    REGISTER_IPU(DIPU_V4L2);
#endif
#ifdef COMPILE_IPU_FILESINK
    REGISTER_IPU(DIPU_FileSink);
#endif
#ifdef COMPILE_IPU_FILESOURCE
    REGISTER_IPU(DIPU_FileSource);
#endif
#ifdef COMPILE_IPU_H2
    REGISTER_IPU(DIPU_H2);
#endif
#ifdef COMPILE_IPU_H2V4
    REGISTER_IPU(DIPU_H2);
#endif
#ifdef COMPILE_IPU_VENCODER
    REGISTER_IPU(DIPU_VENCODER);
#endif
#ifdef COMPILE_IPU_G1264
        REGISTER_IPU(DIPU_G1264);
#endif
#ifdef COMPILE_IPU_G1JDEC
        REGISTER_IPU(DIPU_G1JDEC);
#endif
#ifdef COMPILE_IPU_G2
    REGISTER_IPU(DIPU_G2);
#endif
#if defined(COMPILE_IPU_IDS) || defined(COMPILE_IPU_IDS2)
    REGISTER_IPU(DIPU_IDS);
#endif
#ifdef COMPILE_IPU_ISPOSTV1
    REGISTER_IPU(DIPU_ISPOSTV1);
#endif
#ifdef COMPILE_IPU_ISPOSTV2
    REGISTER_IPU(DIPU_ISPOSTV2);
#endif
#ifdef COMPILE_IPU_FODETV2
    REGISTER_IPU(DIPU_FODETV2);
#endif
#ifdef COMPILE_IPU_VAMOVEMENT
    REGISTER_IPU(DIPU_VAMovement);
#endif
#ifdef COMPILE_IPU_MVMOVEMENT
    REGISTER_IPU(DIPU_MVMovement);
#endif
#ifdef COMPILE_IPU_PP
    REGISTER_IPU(DIPU_PP);
#endif
#ifdef COMPILE_IPU_H1JPEG
    REGISTER_IPU(DIPU_H1JPEG);
#endif
#ifdef COMPILE_IPU_H1264
    REGISTER_IPU(DIPU_H1264);
#endif
#ifdef COMPILE_IPU_FFVDEC
    REGISTER_IPU(DIPU_FFVDEC);
#endif
#ifdef COMPILE_IPU_VAQRSCANNER
    REGISTER_IPU(DIPU_QRScanner);
#endif
#ifdef COMPILE_IPU_MARKER
    REGISTER_IPU(DIPU_MARKER);
#endif
#ifdef COMPILE_IPU_SOFTLAYER
    REGISTER_IPU(DIPU_SoftLayer);
#endif
#ifdef COMPILE_IPU_TFESTITCHER
    REGISTER_IPU(DIPU_TFEStitcher);
#endif
#ifdef COMPILE_IPU_BADENCODER
    REGISTER_IPU(DIPU_BadEncoder);
#endif
#ifdef COMPILE_IPU_FFPHOTO
    REGISTER_IPU(DIPU_FFPHOTO);
#endif
#ifdef COMPILE_IPU_JENC
    REGISTER_IPU(DIPU_JENC);
#endif
#ifdef COMPILE_IPU_SWC
    REGISTER_IPU(DIPU_SWC);
#endif
#ifdef COMPILE_IPU_ISPLUS
    REGISTER_IPU(DIPU_ISPLUS);
#endif
#ifdef COMPILE_IPU_BUFSYNC
    REGISTER_IPU(DIPU_BUFSYNC);
#endif
}

void EventListener(char *event, void *arg)
{
    if (strncmp(VB_RPC_EVENT, event, strlen(VB_RPC_EVENT)) == 0)
    {
        PathRPC *t = NULL;
        t = (PathRPC *)arg;
        LOGD("%s: %s: %s\n", __func__, t->chn, t->func);
        t->ret = VideoBox.PathControl(t->chn, t->func, t->arg);
    }
    else if (strncmp(VB_RPC_INFO, event, strlen(VB_RPC_INFO)) == 0)
    {
        VideoBox.VideoExchangeInfo(arg);
    }
}

void KillHandler(int signo) {
    static int in = 0;
    if(in++) return ;
    const char *msg_ex = "exiting";
    event_send(EVENT_VIDEOBOX, (char *)msg_ex, strlen(msg_ex) + 1);

    LOGE("Exiting from videobox\n");
    VideoBox.Off();

    const char *msg = signo? "force_close": "exit";
    event_send(EVENT_VIDEOBOX, (char *)msg, strlen(msg) + 1);
    LOGE("Exited from videobox\n");
    exit(0);
}

void Started(int signo) {
    LOGE("Videobox %s!\n", signo == SIGINT?
        "started": "terminated");
    exit(0);
}

int videobox_poll(void)
{
    videobox_launch_rpc_l("", NULL, 0);
    return _rpc.ret;
}

#define PATHFILE "/root/.videobox/path.json"
int main(int argc, char *argv[]) {
    Json path_js;
    int ret;
    pid_t pid;

    if((ret = videobox_poll()) > 0) {
        LOGE("videobox is already running! (pid = %d)\n", ret);
        return VBEALREADYUP;
    }

    pid = fork();
    if(pid > 0) {
        signal(SIGINT, Started);
        signal(SIGTERM, Started);
        // Hold until routine started in child process
        for(;; usleep(20000));
    }

    VideoBox.SetJsonPointer(&path_js);
    VideoBox.SetPathPointer(&VideoBox);
    try {
        path_js.Load(argc > 1? argv[1]: PATHFILE);
        VideoBox.SetJsonPath((char*)(argc > 1? argv[1]: PATHFILE));
        RegisterAllIPU();
        VideoBox.Build(&path_js);
        VideoBox.On();
    } catch ( const char* err ) {
        LOGE("ERROR: %s\n", err);
        if(IsError(err, VBERR_WRONGDESC)) LOGE("loc: %s\n",
            path_js.EP());
        kill(getppid(), SIGTERM);
        return -1;
    }

    // Setup kill handler
    signal(SIGINT, KillHandler); // ctrl-c
    signal(SIGTERM, KillHandler); // kill
    signal(SIGQUIT, KillHandler); // ctrl-slash

    // Setup RPC handler
    ret = event_register_handler(VB_RPC_EVENT, EVENT_RPC, EventListener);
    if(ret < 0) {
        LOGE("Warning: register RPC handler failed, cannot provide API service\n");
        kill(getppid(), SIGTERM);
        return -1;
    }

    ret = event_register_handler(VB_RPC_INFO, EVENT_RPC, EventListener);
    if(ret < 0) {
        LOGE("Warning: register RPC handler failed, cannot provide debug info service\n");
        kill(getppid(), SIGTERM);
        return -1;
    }
    // Kill Parent
    kill(getppid(), SIGINT);

    // Hold the process
    for(; !VideoBox.GetError(); usleep(100000));
    KillHandler(0);
    return 0;
}
