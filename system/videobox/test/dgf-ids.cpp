#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <qsdk/event.h>

#include <Path.h>
//#include <PP.h>
#include <DGFrame.h>
#include <IDS.h>
#include <Log.h>


using namespace std;

int stop_service = 0;
Path p;

void sighandler(int signo)
{
    LOGE("Signal INT: %d\n", signo);
    stop_service = 1;
    p.Stop();
    p.Unprepare();

    exit(0);
}

int main()
{
    IPU_DGFrame pal("d0", NULL);
    IPU_IDS ids("display", NULL);


    pal.GetPort("out")->SetPixelFormat(EPixelFormat::NV12);
    pal.GetPort("out")->Bind(ids.GetPort("osd0"));

    // fill all the units into the path
    p << pal << ids;

    LOGE("Preparing IPUs ...\n");
    p.Prepare();
    LOGE("Starting IPUs ...\n");
    p.Start();
    LOGE("Install RPC handler ...\n");
    signal(SIGINT, sighandler);

    LOGE("Main daemon running ...\n");
    // deamon for IPC commands
    while(1) {
        usleep(10000000);
    }
}
