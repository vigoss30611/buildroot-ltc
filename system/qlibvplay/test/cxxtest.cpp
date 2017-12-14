#include <iostream>
#include <mediamuxer.h>
#include <apicalleraudio.h>
#include <apicallervideo.h>
#include <apicallerextra.h>
#include <qrecorder.h>
#include <unistd.h>
#include <define.h>
#include <stdio.h>
using namespace std;




int main(int argc,char *argv[] ){

    VRecorderInfo recorderInfo  ;
    printf("start %s\n",argv[0]);
    memset(&recorderInfo ,0,sizeof(VRecorderInfo));

    char temp[] =  "REC_VIDEO_%ts-%te_%c" ;
    if (argc > 1) {
        int ch = 0 ;
        strcat(recorderInfo.av_format,argv[1]);
        strcat(recorderInfo.dir_prefix,"/nfs/");
        printf("use cmd para\n");
#if 0
        char *av_format = NULL;
        char *audio_format = NULL;
        char *suffix = NULL;
#endif
        while(( ch=getopt(argc,argv,"a::m::s::") ) != -1 ){
            switch (ch ) {
                case 'a':
                    printf("audio type-> %s\n",optarg);
                    break ;
                case 'm':
                    printf("muxer type-> %s\n",optarg);
                    break ;
                default :
                    printf("not support options\n");
                    break;
            }
        }
    }
    else {
        strcat(recorderInfo.av_format,"mkv");
        strcat(recorderInfo.suffix,temp);
        strcat(recorderInfo.dir_prefix,"/nfs/");
        printf("use local para\n");
    }
    recorderInfo.enable_gps = 0 ;
    recorderInfo.audio_format.type = AUDIO_CODEC_TYPE_G711A ;
//    recorderInfo.audio_format.type = AUDIO_CODEC_PCM ;

    recorderInfo.audio_format.sample_rate = 8000;
    recorderInfo.audio_format.sample_fmt   = AUDIO_SAMPLE_FMT_S16 ;
    recorderInfo.audio_format.channels    = 2;

    recorderInfo.time_segment = 30;
    recorderInfo.time_backward = 30;


    recorderInfo.enable_gps =  0;




    //char temp[] =  "REC_VIDEO_%Y_%y_%d_%H_%M_%S_%s_%c" ;
    printf("-->%s<--\n",recorderInfo.suffix );
    printf("-->%s<--\n",recorderInfo.av_format);
    sprintf(recorderInfo.video_channel,"enc1080p-stream" );
    sprintf(recorderInfo.audio_channel,"default_mic" );
    strcat(recorderInfo.time_format , "%m_%d_%H_%M_%S");
    //strcat(recorderInfo.time_format , "%Y_%y_%m_%d_%H_%M_%S_%s");



    char c ;
    VRecorder *recorder =  vplay_new_recorder(&recorderInfo);
    if(recorder == NULL){
        printf("init recorder error\n");
        exit(0);
    }
    int ret = 0 ;
    printf("input control char :\n\ts，start;  \n\tc: continue \n\tq:stop and quit ;\n\tp pause ;\n");
    while(1){
        c =  getchar();
        switch(c){
            case 's':
                ret = vplay_start_recorder2(recorder);
                printf("start recorder ->%d\n",ret);
                break;
            case 'q' :
                ret = vplay_delete_recorder(recorder);
                printf("stop recorder ->%d\n",ret);
                break;
            case 'p':
                ret = vplay_stop_recorder2(recorder);
                printf("stop recorder ->%d\n",ret);
                break;
            case 'c':
                ret = vplay_start_recorder2(recorder);
                printf("start recorder ->%d\n",ret);
                break;
            case 'h':
                printf("input control char :\n\ts，start;  \n\tc: continue \n\tq:stop and quit ;\n\tp pause ;\n");
                break;
            default :
                break ;
        }
        if(c == 'q'){
            break;
        }
        if(c == '\n'){
            continue ;
        }
        //printf("muxer state ->%d\n",recorder->GetWorkingState());

    }
//    vplay_delete_recorder(recorder);

    return 1;
}
