#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <System.h>
#include <Path.h>
#include <V2505.h>
#include <IDS.h>

#include "Configuration.h"
#include <string.h>
#include <stdio.h>
using namespace std;

volatile int stop_service = 0;
Path p;

static void GetParameterThread(void *arg)
{
    string cmd;
    IPU_V2500 *v2500 = (IPU_V2500 *)arg;
    Configuration config;
    const char * split = " ";
    char *p  =  NULL;
    char *argv[50];

    while(getline(cin, cmd))
    {
        int i = 0;
        p = strtok((char *)cmd.c_str(), split);
        std::cout<< cmd.c_str()<<endl;
        while(p != NULL)
        {
            argv[i] =(char *) malloc((unsigned int )strlen(p) + 1);

            strcpy(argv[i], p);
            LOGE("i = %d ==arv[%d]= %s\n", i, i, argv[i]);
            p = strtok(NULL, split);
            i++;
        }

        LOGE("i = %d\n", i);
        config.getParameters(i, argv, *v2500);
        //config.parseParameter(*v2500);
        config.releaseParameter();
    }
}

void sighandler(int signo)
{
    LOGE("Signal INT: %d\n", signo);
    stop_service = 1;
    p.Off();

    exit(0);
}

int main(int argc ,char **argv)
{

    //std::cout <<"debug0" <<endl;
    IPU_V2500 v2500("isp", NULL); //FIXME
    if (v2500.currentStatus != SENSOR_CREATE_SUCESS)
    {
        LOGE("IPU v2500 create failed\n");
        exit(1);
    }

    //std::cout <<"debug1" <<endl;
    //v2500.SetResolution(1280, 720);
    IPU_IDS ids("display", NULL);
    //std::cout <<"debug2" <<endl;
    std::cout << "camera status:" <<std::dec<<v2500.pCamera->state<<endl;
    v2500.GetPort("out")->SetPixelFormat(Pixel::NV12);
    v2500.GetPort("out")->Bind(ids.GetPort("osd0"));
    //fill all elements to path
    p << v2500 << ids;
    p.On();

    signal(SIGINT, sighandler);

    LOGE("Main daemon running ...\n");
    //TODO receive parameters
    thread *pthread = new thread(GetParameterThread, &v2500);
    pthread->join();
    while(1)
    {
        usleep(10000000);
    }

}
