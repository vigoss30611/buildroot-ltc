#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <qsdk/videobox.h>

#define CHANNELNAME "enc0-stream"
int main(int argc, char *argv[])
{
	int fd, len, ret, loc = 0;
	struct fr_buf_info buf = FR_BUFINITIALIZER;
	char header[256];

	fd = open(argc > 1? argv[1]:"/nfs/rec.265", O_CREAT | O_TRUNC | O_WRONLY, 0666);

	len = video_get_header(CHANNELNAME, header, 128);

	printf("got header: %d bytes\n", len);

	write(fd, header, len);
	for(; ;) {
		video_get_frame(CHANNELNAME, &buf);
		printf("rec << %d KB (%s:%lld)\n", buf.size >> 10,
		 buf.priv == VIDEO_FRAME_I? "i": "p", buf.timestamp);
		write(fd, buf.virt_addr, buf.size);
		video_put_frame(CHANNELNAME, &buf);
	}

	return 0;
}

