#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "sdheader.h"

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

static int valid_bootscr(unsigned char * addr)
{
	unsigned char header[] = {
		0x0a, 0x2d, 0x2d, 0x5b, 0x5b, 0x75, 0x62, 0x6f,
		0x6f, 0x74, 0x2e, 0x73, 0x63, 0x72, 0x69, 0x70,
		0x74, 0x0a,};
	unsigned char *p = (unsigned char *)addr;

	int i;
	for(i = 0; i < ARRAY_SIZE(header); i++)
	  if(*p++ ^ header[i])
		return 0;

	for(i = 0; i < 0x1000; i++, p++)
	  if(*p == 0xa
		 && *(p + 1) == 0x5d
		 && *(p + 2) == 0x5d)
		return 1;		// valid script 
	return 0;
}

int main(int argc, char *argv[])
{
	char buf[4096];
	char b2[4096];
	int fdi, fdo, ret, i;

	if(argc < 3)
	{
		printf("Usage:\nmkbootsd /dev/dev_node src_file\n");
		exit(0);
	}

confirm:
	printf("All data on SD card will be lost, type 'confirm' to continue: ");
	ret = scanf("%s", buf);
	if(strcmp(buf, "confirm"))
	  goto confirm;

	fdo = open(argv[1], O_RDWR);
	if(fdo < 0)
	{
		printf("Can not open %s\n", argv[1]);
		exit(1);
	}

	/* Write header */
	ret = write(fdo, sd_sector_0, 512);
	if(ret != 512)
	{
		printf("Write SD header failed.\n");
		goto __exit__;
	}

	/* Write Script */
	fdi = open(argv[2], O_RDONLY);
	if(fdo < 0)
	{
		printf("Can not open %s\n", argv[2]);
		exit(1);
	}

	memset(buf, 0, 4096);
	ret = read(fdi, buf, 4096);
	close(fdi);

	if(!valid_bootscr((unsigned char *)buf))
	{
		printf("Boot script is invalid.\n");
		goto __exit__;
	}

	ret = lseek(fdo, 0x80000, SEEK_SET);
	if(ret != 0x80000)
	{
		printf("Seek script location failed.\n");
		goto __exit__;
	}

	ret = write(fdo, buf, 4096);
	if(ret != 4096)
	{
		printf("Write script failed.\n");
		goto __exit__;
	}


	/* here we double check if the script is wrote to SD card */
	fsync(fdo);
	ret = lseek(fdo, 0x80000, SEEK_SET);
	if(ret != 0x80000)
	{
		printf("Seek script checking failed.\n");
		goto __exit__;
	}
	ret = read(fdo, b2, 4096);
	if(ret != 4096)
	{
		printf("Read script failed. ret=%d\n", ret);
		goto __exit__;
	}

	for(i = 0; i < 4096; i++)
	{
		if(buf[i] ^ b2[i]) {
			printf("SD script wrote failed, this may caused by hardware failure.\n");
			goto __exit__;
		}
	}

	printf("Boot SD created successfully!\n");
__exit__:
	close(fdo);
	return 0;
}
