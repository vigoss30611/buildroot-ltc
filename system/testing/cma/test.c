#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int opt;
	int blksize = -1;
	int mblknum = -1;
	int fstep = -1; 
	char echar;
	int loopcnt = -1;
	int curnum = 0;
	char str[20];
	void * ptr;
	int total_malloc;
	int total_free;
	int total_fail;
	int mblknum_loc;
	int wait_time = 2000;//seconds
	
	while((opt = getopt(argc, argv, "s:m:f:l:w:"))!=-1) {
		switch (opt) {
		case 's':
			blksize = atoi(optarg);
			break;
		case 'm': 
			mblknum = atoi(optarg);
			break;
		case 'f':
			fstep = atoi(optarg);
			break;
		case 'l':
			loopcnt = atoi(optarg);
			break;
		case 'w':
			wait_time = atoi(optarg);
			break;
		case '?': 
			echar = (char)optopt;
			printf("argument error, opt char is \' %c \'!\n", echar);
			break;
		}
	}
	
	if (blksize <= 0 || mblknum <=0 || fstep <0 || loopcnt<=0) {
		printf("error para value. %d, %d, %d, %d\n", blksize, mblknum, fstep, loopcnt);
		return -1;
	}
	printf("size=%08x, mblknum=%08x, fstep=%08x, loopcnt=%d, waittime=%d\n", blksize, mblknum, fstep, loopcnt, wait_time);
	
#if 0
	while(1) {
		printf("please enter test loopcnt\n");
		gets(str);
		if (atoi(str) <= 0) {
			printf("finish test loopcnt=%d\n", loopcnt);
			return -1;
		}
		loopcnt = atoi(str);
#endif
		printf("start test loop %d\n", loopcnt);
		while(loopcnt-- > 0) {
			curnum = 0;
			mblknum_loc = mblknum;
			while (mblknum_loc-- > 0) {
				ptr = malloc(blksize);
				*(unsigned char*) ptr = 0xd;
				if (ptr <= 0) {
					total_fail++;
					printf("malloc(%d) failed %d, total: malloc=%d free=%d, fail=%d\n", blksize, ptr, total_malloc, total_free, total_fail);
					
				}
				total_malloc++;
				total_free++;

				if (fstep!=0 && curnum%fstep==0 && ptr>0) {
					free(ptr);
				}
				curnum++;
			}
		}

		sleep(500);
#if 0
	}
#endif


	return 0;
}

