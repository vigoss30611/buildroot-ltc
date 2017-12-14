#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <fstream>

#include <Path.h>
#include <V2500.h>
#include <FileSink.h>
#include <Log.h>


using namespace std;

int stop_service = 0;
//Path p;

void sighandler(int signo)
{
    LOGE("Signal INT: %d\n", signo);
    stop_service = 1;
    //p.Stop();
    //p.Unprepare();

    exit(0);
}

int main(int argc ,char **argv)
{
    ofstream file;
    IPU_ISPC::IPU_V2500 v2500("d0", NULL); //FIXME
    v2500.Start();

    //file.open("/nfs/capture_camera_frame.flx", ios::out | ios::binary);
    LOGE("Main daemon running ...\n");
    signal(SIGINT, sighandler);
    //int nsize = 1920*1088*3/2;

    IMG_RESULT ret;
    // deamon for IPC commands
    //ret = v2500.pCamera->enqueueShot();
    std::cout << "camera status:" <<std::dec<<v2500.pCamera->state<<endl;
    //ISPC::ModuleOUT *pModule = v2500.pCamera->getPipeline()->getModule<ISPC::ModuleOUT>();
    //std::cout<< "module out data:" <<pModule->encoderType <<endl;

    int i = 0;
    for(i = 0; i < v2500.nBuffers; i++)
    {
        ret = v2500.pCamera->enqueueShot();
        if (IMG_SUCCESS != ret)
        {
            LOGE("enqueue shot in v2500 ipu thread failed.\n");
            return -1 ;
        }
        LOGE("enqueue %d===\n", i);
    }


    while(1){

        if(stop_service)
        {
            //break;
        }
//        ret = v2500.pCamera->enqueueShot();
//        if (IMG_SUCCESS != ret)
//        {
//            std::cout << "enque shot in v2500 ipu thread failed.\n";
//            return -1;
//        }
        LOGE("loop====0\n");
        //may be blocking until the frame is received
        ret = v2500.pCamera->acquireShot(v2500.frame);
        if (IMG_SUCCESS != ret) {
            std::cout << "failed to get shot\n";
            return -1 ;
        }
        //TODO
        std::cout << "frame YUV buffer address = 0x" << std::hex << (int *)v2500.frame.YUV.data<<endl;
        //file.write((char *)v2500.frame.YUV.data, nsize);
        //WriteFile((char *)v2500.frame.YUV.data, nsize, "/nfs/yuv.%d", 0);
        //std::cout << "frame YUV buffer address = 0x" << std::hex << v2500.frame.YUV.phyAddr<<endl;
        LOGE("v2500.frame.YUV.height =%d\n",v2500.frame.YUV.height );
        usleep(100000);
        ret = v2500.pCamera->releaseShot(v2500.frame);
        if (IMG_SUCCESS != ret) {
            std::cout << "failed to release shot\n";
            return -1 ;
        }
        LOGE("loop====1\n");
        ret = v2500.pCamera->enqueueShot();
        if (IMG_SUCCESS != ret)
        {
            std::cout << "enque shot in v2500 ipu thread failed.\n";
            return -1;
        }
        LOGE("loop====2\n");
    }
    file.close();
    v2500.Stop();
}
