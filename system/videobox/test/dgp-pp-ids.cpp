#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <Path.h>
#include <DGPixel.h>
#include <IDS.h>
#include <PP.h>
#include <Log.h>
#include <DGMath.h>
#include <DGFrame.h>
#include <Common.h>

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
    IPU_DGMath mp("d0", NULL);
    IPU_DGMath mp1("d1", NULL);
    IPU_DGFrame dgf("d2", NULL);
    IPU_IDS ids("display", NULL);
    IPU_PP pp("pp0", NULL);
    IPU_DGPixel dp("d3", NULL); //FIXME::data path
    IPU_PP pp_f("pp1", NULL);

    mp.GetPort("out")->SetResolution(640, 360);
    dgf.GetPort("out")->SetResolution(640, 360);

    pp_f.SetOffset("ol0", 0, 0);
    pp.SetOffset("fb", 1280, 720);

    dp.GetPort("out")->Bind(pp.GetPort("fb"));
    dgf.GetPort("out")->Bind(pp.GetPort("in"));
    mp.GetPort("out")->Bind(pp_f.GetPort("ol0"));
    pp.GetPort("out")->Bind(pp_f.GetPort("in"));
    pp_f.GetPort("out")->Bind(ids.GetPort("osd0"));

    p << dgf << mp << pp << dp << pp_f << ids;

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
