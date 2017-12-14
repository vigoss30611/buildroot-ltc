#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <qsdk/videobox.h>

#define CHANNELNAMEENC "enc1080p-stream"
#define CHANNELNAMEG2 "g2dec-frame"

int main(void)
{
    char* header;
    int len = 0;

    header = (char *)malloc(128);
    len = video_get_header(CHANNELNAMEENC, header, 128);
    if(len < 0){
        printf("video_get_header failed\n");
        free(header);
        return -1;
    }

    len = video_set_header(CHANNELNAMEG2, header, len);
    if(len < 0){
        printf("video_set_header failed\n");
        free(header);
        return -1;
    }

    free(header);
    return 0;
}
