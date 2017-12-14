#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>

#include <Path.h>
//#include <PP.h>
#include <DGMath.h>
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
    try {
        IPU_DGMath m("d0", NULL);
        IPU_IDS ids("display", NULL);

        m.GetPort("out")->Bind(ids.GetPort("osd0"));

        // fill all the units into the path
        p << m << ids;

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
    } catch (const char *s) {
        LOGE("construct IPU failed: %s\n", s);
        return -1;
    }
}
