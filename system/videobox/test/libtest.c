#include <stdio.h>
#include <qsdk/videobox.h>

#define CHN1 "enc1080p-stream"
#define CHN2 "enc720p-stream"
#define CHN3 "encvga-stream"

int main()
{
    char *chn[] = {
        "enc1080p-stream",
        "enc720p-stream",
        "encvga-stream",
        NULL
    };
    int i, j;
    struct fr_buf_info buf;

again:
    /* disable */
    for(i = 0; chn[i]; i++)
        video_disable_channel(chn[i]);

    /* enable */
    for(i = 0; chn[i]; i++)
        video_enable_channel(chn[i]);

    for(i = 0; chn[i]; i++)
        for(j = 0, fr_INITBUF(&buf, NULL); j < 3; j++) {
            video_get_frame(chn[i], &buf);
            video_put_frame(chn[i], &buf);
        }

    return 0;
}
