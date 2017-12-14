#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>

#include <Path.h>
#include <ipu_marker.h>
#include <IDS.h>
#include <Log.h>

using namespace std;

int stop_service = 0;
Path p;

#if 0
/* set chinese */
    struct font_attr tutk_attr;
    memset(&tutk_attr, 0, sizeof(struct font_attr));
    sprintf((char *)tutk_attr.ttf_name, "simhei");
    tutk_attr.font_color = 0x00ffffff;
    tutk_attr.back_color = 0x20000000;
    marker_set_mode("marker0", "manual", "%u%Y/%M/%D  %H:%m:%S", &tutk_attr);
    marker_set_string("marker0", "infotm上海盈方微电子");

/* set time marker */
    struct font_attr tutk_attr;
    memset(&tutk_attr, 0, sizeof(struct font_attr));
    sprintf((char *)tutk_attr.ttf_name, "simhei");
    tutk_attr.font_color = 0x00ffffff;
    tutk_attr.back_color = 0x20000000;
    marker_set_mode("marker0", "auto", "%t%Y/%M/%D  %H:%m:%S", &tutk_attr);

/* set utc marker */
    struct font_attr tutk_attr;
    memset(&tutk_attr, 0, sizeof(struct font_attr));
    sprintf((char *)tutk_attr.ttf_name, "simhei");
    tutk_attr.font_color = 0x00ffffff;
    tutk_attr.back_color = 0x20000000;
    marker_set_mode("marker0", "auto", "%u%Y/%M/%D  %H:%m:%S", &tutk_attr);

/* set other number in marker */
    struct font_attr tutk_attr;
    memset(&tutk_attr, 0, sizeof(struct font_attr));
    sprintf((char *)tutk_attr.ttf_name, "simhei");
    tutk_attr.font_color = 0x00ffffff;
    tutk_attr.back_color = 0x20000000;
    marker_set_mode("marker0", "auto", "%c%Y/%M/%D  %H:%m:%S", &tutk_attr);

/* set bmp picture */
    struct font_attr tutk_attr;

    memset(&tutk_attr, 0, sizeof(struct font_attr));
    sprintf((char *)tutk_attr.ttf_name, "arial");
    tutk_attr.font_color = 0x00ffffff;
    tutk_attr.back_color = 0x20000000;
    marker_set_mode("marker0", "manual", "%t%Y/%M/%D  %H:%m:%S", &tutk_attr);
    
    struct fr_buf_info ref;
    fr_INITBUF(&ref, NULL);
    marker_get_frame("marker0", &ref);

    FILE *fpbmp = NULL;
    unsigned int OffSet = 0;              // OffSet from Header part to Data Part
    long width = 0;                       // The Width of the Data Part
    long height = 0;                      // The Height of the Data Part
    fpbmp= fopen("/root/qiwo/wav/1.bmp", "rb");

    if (fpbmp == NULL) {
        printf("Open bmp failed!!!\n");
        goto out;
    }

    fseek(fpbmp, 10L, SEEK_SET);
    fread(&OffSet, sizeof(char), 4, fpbmp);

    fseek(fpbmp, 18L, SEEK_SET);
    fread(&width, sizeof(char), 4, fpbmp);

    fseek(fpbmp, 22L, SEEK_SET);
    fread(&height, sizeof(char), 4, fpbmp);

    fseek(fpbmp, OffSet, SEEK_SET);        //seek set to position of r,g,b data

    fread(ref.virt_addr, 1, ref.size, fpbmp);
    fclose(fpbmp);
    
    marker_put_frame("marker0", &ref);

#endif

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
    struct font_attr attr1;
    IPU_MARKER marker("marker0", NULL);
    
    LOGE("Preparing IPUs ...\n");
    p.Prepare();
    LOGE("Starting IPUs ...\n");
    p.Start();
    LOGE("Install RPC handler ...\n");
    signal(SIGINT, sighandler);
    marker.UpdateString((char*)"marker0", (char*)"/usr/share/fonts/truetype/arial.ttf", &attr1);
    
    LOGE("Main daemon running ...\n");
    
    // deamon for IPC commands
    while(1) {
        usleep(10000000);
    }
}
