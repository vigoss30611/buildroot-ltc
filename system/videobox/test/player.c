#include <stdio.h>
#include <qsdk/videobox.h>
#include <qsdk/demux.h>

int main(int argc, char *argv[])
{
    struct demux_t *dmx;
    struct demux_frame_t frm;
    char header[128];
    int ret;

    if(argc < 3) {
        printf("player FILE_NAME DEC_PORT FS_PORT\n");
        return 0;
    }

    dmx = demux_init(argv[1]);
    if(!dmx) {
        printf("demux %s failed\n", argv[1]);
        return -1;
    }

    ret = demux_get_head(dmx, header, 128);
    video_set_header(argv[2], header, ret);
    while((ret = demux_get_frame(dmx, &frm)) == 0) {
        if(frm.codec_id != DEMUX_VIDEO_H265) goto skip;
        usleep(50000);
        video_write_frame(argv[3], frm.data, frm.data_size);
skip:
        demux_put_frame(dmx, &frm);
    }

    demux_deinit(dmx);
    return 0;
}

/*
pkg_check_modules(DMX REQUIRED demux)
include_directories(${DMX_INCLUDE_DIRS})
add_definitions(${DMX_CFLAGS})
link_libraries(${DMX_LIBRARIES})
add_executable(tplayer lib/video.c test/player.c)
*/
