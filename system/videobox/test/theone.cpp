#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <fstream>

#include <Path.h>
#include <V2500.h>
#include <IDS.h>
#include <H2.h>
#include <FileSink.h>
#include <Log.h>

using namespace std;

Path p;

void sighandler(int signo)
{
    LOGE("Signal INT: %d\n", signo);
    p.Stop();
    p.Unprepare();

    exit(0);
}

void listener(char *event, void *arg)
{
    PathRPC *t = (PathRPC *)arg;

    LOGE("videobox listener: got event: %s\n", event);
    t->ret = p.PathControl(t->chn, t->func, t->arg);
}

int main(int argc ,char **argv)
{
    IPU_V2500 v2500("d0", NULL); //FIXME
    IPU_H2 hevc0("enc0", NULL);
    IPU_IDS ids("display", NULL);
    IPU_FileSink fs("fs0", NULL);  //FIXME: path

    v2500.SetResolution(1920, 1088);
    v2500.GetPort("isp")->SetPixelFormat(EPixelFormat::NV21);
    v2500.GetPort("isp")->Bind(ids.GetPort("osd0"));
    v2500.GetPort("isp")->Bind(hevc0.GetPort("frame"));
    hevc0.GetPort("stream")->Bind(fs.GetPort("in"));

    //fill all elements to path
    p << v2500 << hevc0 << ids;

    p.Prepare();
    p.Start();

    signal(SIGINT, sighandler);
    int ret;
    ret = event_register_handler(VB_RPC_EVENT, EVENT_RPC, listener);

    LOGE("register handler got result: %d\n", ret);
    LOGE("Main daemon running ...\n");

    while(1)
    {
        usleep(10000000);
    }

}
