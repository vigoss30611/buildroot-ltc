#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include <Path.h>
#include <DGFrame.h>
#include <DGPixel.h>
#include <H2.h>
#include <FileSink.h>
#include <Log.h>
#include <qsdk/video.h>

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
void show_help(void){
    LOGE("app case  :\n");
    LOGE("\tcase :\n");
    LOGE("\t\tdouble : double dg generate\n");
    LOGE("\t\tsingle : single dg generate\n");

}
void double_dg_encode(void) {

#if 1
    IPU_DGFrame pal0("d0", NULL);
    //IPU_DGPixel pal0("/nfs/test0.txt");
    IPU_DGPixel pal1("d1", NULL); //FIXME
#else
    IPU_DGFrame pal0;
    IPU_DGFrame pal1;
#endif

    IPU_H2 hevc0("enc0", NULL);
    IPU_H2 hevc1("enc1", NULL);
    hevc0.SetIndex(0);
    hevc1.SetIndex(1);

    // FIXME:: json path
    IPU_FileSink fileSink0("fs0", NULL);
    IPU_FileSink fileSink1("fs1", NULL);

    pal0.GetPort("out")->SetPixelFormat(EPixelFormat::NV12);
    pal0.GetPort("out")->Bind(hevc0.GetPort("frame"));
    hevc0.GetPort("stream")->Bind(fileSink0.GetPort("in"));

    pal1.GetPort("out")->SetPixelFormat(EPixelFormat::NV12);
    pal1.GetPort("out")->Bind(hevc1.GetPort("frame"));
    hevc1.GetPort("stream")->Bind(fileSink1.GetPort("in"));

    // fill all the units into the path
    p << fileSink0 << fileSink1 <<pal0 <<pal1<< hevc0 << hevc1  ;

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
void single_dg_encode(void) {
    //IPU_DGPixel pal0("/nfs/test1.txt");
    IPU_DGFrame pal0("d0", NULL);
    IPU_H2 hevc0("enc0", NULL);
    IPU_FileSink fileSink0("fs0", NULL); //FIXME: path

    pal0.GetPort("out")->SetPixelFormat(EPixelFormat::NV12);
    pal0.GetPort("out")->Bind(hevc0.GetPort("frame"));
    hevc0.GetPort("stream")->Bind(fileSink0.GetPort("in"));
    // fill all the units into the path
    p << fileSink0  <<pal0 << hevc0   ;

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
static void set_encode_config_nals(void) {
    IPU_DGPixel pal0("d0", NULL);
    IPU_H2 hevc0("enc0", NULL);
    IPU_FileSink fileSink0("fs0", NULL);

//    VENC_ENCODE_CONFIG_T encode_config ;
//    encode_config.frame_num =  15;

    //encode_config.stream_type = VENC_STREAM_NALSTREAM ;

    //hevc0.SetBasicInfo(encode_config);


    pal0.GetPort("out")->SetPixelFormat(EPixelFormat::NV12);
    pal0.GetPort("out")->Bind(hevc0.GetPort("frame"));
    hevc0.GetPort("stream")->Bind(fileSink0.GetPort("in"));
    // fill all the units into the path
    p << fileSink0  <<pal0 << hevc0   ;

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
static void set_encode_roi_area(void) {
    IPU_DGPixel pal0("d0", NULL);
    IPU_H2 hevc0("enc0", NULL);
    IPU_FileSink fileSink0("fs0", NULL);

    struct v_basic_info encode_config ;
    encode_config.p_frame =  15;

    encode_config.stream_type = VENC_NALSTREAM ;

    hevc0.SetBasicInfo(&encode_config);


    pal0.GetPort("out")->SetPixelFormat(EPixelFormat::NV12);
    pal0.GetPort("out")->Bind(hevc0.GetPort("frame"));
    hevc0.GetPort("stream")->Bind(fileSink0.GetPort("in"));
    // fill all the units into the path
    p << fileSink0  <<pal0 << hevc0   ;

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
static void set_encode_rate_ctrl(void) {
    IPU_DGPixel pal0("d0", NULL); //FIXME
    IPU_H2 hevc0("enc0", NULL);
    IPU_FileSink fileSink0("fs0", NULL); //FIXME

    v_basic_info encode_config ;
    encode_config.p_frame =  15;
    encode_config.stream_type = VENC_BYTESTREAM ;

    hevc0.SetBasicInfo(&encode_config);


    pal0.GetPort("out")->SetPixelFormat(EPixelFormat::NV12);
    pal0.GetPort("out")->Bind(hevc0.GetPort("frame"));
    hevc0.GetPort("stream")->Bind(fileSink0.GetPort("in"));
    // fill all the units into the path
    p << fileSink0  <<pal0 << hevc0   ;

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
int main(int argc ,char **argv)
{
#if 0
    if (argc <  2 ){
        show_help();
        exit(0);
    }
    char *p = argv[1];
#else
    char p[] = "nals";
#endif
    if (strcmp("double" ,p )  == 0 ){
        double_dg_encode();
    }
    else if (strcmp("single" ,p )  == 0 ){
        single_dg_encode();
    }
    else if (strcmp("nals" , p )  == 0 ){
        set_encode_config_nals();
    }
    else if (strcmp("roiarea" , p )  == 0 ){
        set_encode_roi_area();
    }
    else if (strcmp("ratectrl" , p )  == 0 ){
        set_encode_rate_ctrl();
    }
    else {
        show_help();
    }
}
