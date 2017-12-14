#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>

#include <Path.h>
//#include <PP.h>
#include <DGPixel.h>
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
    //FIXME:: data
    IPU_DGPixel tp("d0", NULL);
    IPU_IDS ids("d0", NULL);

    tp.GetPort("out")->Bind(ids.GetPort("osd0"));

    // fill all the units into the path
    p << tp << ids;

    LOGE("Preparing IPUs ...\n");
    p.Prepare();
    LOGE("Starting IPUs ...\n");
    p.Start();
    LOGE("Main daemon running ...\n");
    signal(SIGINT, sighandler);

    // deamon for IPC commands
    while(1) {
        usleep(10000000);
    }
}
