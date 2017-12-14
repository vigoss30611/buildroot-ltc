#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fr/libfr.h>

struct fr_info fr;

#define msleep(n) usleep(n * 1000)

void get_random_bytes(char *buf, int len)
{
	int fd = open("/dev/urandom", O_RDONLY);
	read(fd, buf, len);
	close(fd);
}

int __frt_thread_on = 0;
static void* feeder(void *arg)
{
	struct fr_buf_info buf;

	printf( "%s:%s started (%d)\n", __func__, fr.name,
		__frt_thread_on);
	fr_INITBUF(&buf, &fr);
	while(__frt_thread_on) {
		/* feed buf every second */
		fr_get_buf(&buf);
//		printf( "%s -> %s: get %08x, p%08x\n", __func__,
//		 fr.name, buf.virt_addr, buf.phys_addr);
		msleep(10);
		fr_STAMPBUF(&buf);
		get_random_bytes((char *)&buf.size, 4);
		buf.size &= 0xfffff;
		buf.size += 1;
		fr_put_buf(&buf);
		printf( "%s -> put %08x(%p), size %x\n", __func__,
		 buf.phys_addr, buf.virt_addr, buf.size);
	}
	return 0;
}

static int frt_buf_processer(int speed, const char *name)
{
	struct fr_buf_info ref;
	int ret;

	printf( "%s:%s started (%d)\n", name, fr.name,
		__frt_thread_on);
	fr_INITBUF(&ref, &fr);
	while(__frt_thread_on) {
		printf( "%s || %d\n", name, fr_test_new(&ref));
		ret = fr_get_ref(&ref);
		if(ret < 0) {
			printf( "%s << no ref (%d)\n", name, ret);
			msleep(5);
			continue;
		}
		printf( "%s << got %08x(%p), size %x\n",
			 name, ref.phys_addr, ref.virt_addr, ref.size);
		msleep(speed); /* pretend to process */
		fr_put_ref(&ref);
	}
	return 0;
}

static void * profast(void *arg)
{
	frt_buf_processer(10, "prfast");
	return NULL;
}

static void * proslow(void *arg)
{
	frt_buf_processer(100, "prslow");
	return NULL;
}

static void* nfeeder(void *arg)
{
	struct fr_buf_info buf;
	int ret;

	printf( "%s:%s started (%d)\n", __func__, fr.name,
		__frt_thread_on);
	fr_INITBUF(&buf, &fr);
	while(__frt_thread_on) {
		/* feed buf every second */
		ret = fr_get_buf_by_name(fr.name, &buf);
		if(ret < 0) {
			printf("can not get buf from frlib-ntst\n");
			return NULL;
		}

//		printf( "%s -> %s: get %08x, p%08x\n", __func__,
//		 fr.name, buf.virt_addr, buf.phys_addr);
		msleep(10);
		fr_STAMPBUF(&buf);
		get_random_bytes((char *)&buf.size, 4);
		buf.size &= 0xfffff;
		buf.size += 1;
		fr_put_buf(&buf);
		printf( "%s -> put %08x(%p), size %x\n", __func__,
		 buf.phys_addr, buf.virt_addr, buf.size);
	}
	return 0;
}

static int frt_buf_nprocesser(int speed, const char *name)
{
	struct fr_buf_info ref;
	int ret;

	printf( "%s:%s started (%d)\n", name, fr.name,
		__frt_thread_on);
	fr_INITBUF(&ref, NULL);
	while(__frt_thread_on) {
		printf( "%s || %d\n", name, fr_test_new_by_name(fr.name, &ref));
		ret = fr_get_ref_by_name(fr.name, &ref);
		if(ret < 0) {
			printf( "%s << no ref\n", name);
			msleep(5);
			continue;
		}
		printf( "%s << got %08x(%p), size %x\n",
			 name, ref.phys_addr, ref.virt_addr, ref.size);
		msleep(speed); /* pretend to process */
		fr_put_ref(&ref);
	}
	return 0;
}

static void * nprofast(void *arg)
{
	frt_buf_nprocesser(10, "prfast");
	return NULL;
}

static void * nproslow(void *arg)
{
	frt_buf_nprocesser(100, "prslow");
	return NULL;
}

static void __test_alloc(void)
{
	int ret;

	// alloc a 3.5M buffer
	ret = fr_alloc(&fr, "frlib-test", 0x371234, FR_FLAG_RING(1));
	if(ret < 0) {
		printf("alloc fr %s failed.\n", fr.name);
		exit(1);
	}

	// free it
	fr_free(&fr);
}

static void __test_getput(void)
{
	struct fr_buf_info buf1;
	int i, ret;

	// alloc a 1.3M x 5 ring buffer
	ret = fr_alloc(&fr, "frlib-ring", 0x153397, FR_FLAG_RING(5));
	if(ret < 0) {
		printf("alloc fr %s failed.\n", fr.name);
		exit(1);
	}

	fr_INITBUF(&buf1, &fr);
	for(i = 0; i < 100; i++) {
		fr_get_buf(&buf1);
		printf("got buffer: v%p, p%08x\n", buf1.virt_addr, buf1.phys_addr);
		fr_put_buf(&buf1);
	}

	fr_free(&fr);
}

static void __test_single(void)
{
	void *virt;
	int i, ret;
	uint32_t phys[100];

	// test single
	for(i = 0; i < 100; i++) {
		virt = fr_alloc_single("sinb", (i+1) * 15000, &phys[i]);
		if(!virt) {
			printf("unable to allocat memory anymore, %dMB allocated\n",
			(i * (i + 1) / 2 * 15000) >> 20);
			i--;
			break;
		} else
		printf("%d: v%p, p%08x\n", i, virt, phys[i]);
	}

	sleep(5);
	for(; i > 50; i--)
		fr_free_single(NULL, phys[i]);
	for(i = 0; i < 51; i++)
		fr_free_single(NULL, phys[i]);
}

static void __test_float(void)
{
	pthread_t f, p1, p2;
	int i, ret;

	printf("\n\n\ntest float\n\n\n");

	ret = fr_alloc(&fr, "frlib-float", 0x404000, FR_FLAG_FLOAT
				| FR_FLAG_MAX(0x100000));

	if(ret < 0) {
		printf("failed to alloc buffer %d\n", ret);
		return ;
	}

	__frt_thread_on = 1;
	pthread_create(&f, NULL, feeder, NULL);
	pthread_create(&p1, NULL, proslow, NULL);
	pthread_create(&p2, NULL, profast, NULL);

	for(i = 10; i--; sleep(1));
	__frt_thread_on = 0;
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(f, NULL);

	fr_free(&fr);
}

static void __test_name(void)
{
	pthread_t f, p1, p2;
	int i, ret;

	printf("\n\n\ntest name\n\n\n");
	ret = fr_alloc(&fr, "frlib-ntst", 0xa00000, FR_FLAG_FLOAT
				| FR_FLAG_MAX(0x150000));

	__frt_thread_on = 1;
	usleep(1000000);
	pthread_create(&f, NULL, nfeeder, NULL);
//	pthread_create(&p1, NULL, nproslow, NULL);
	pthread_create(&p2, NULL, nprofast, NULL);

	for(i = 10; i--; sleep(1));
	__frt_thread_on = 0;
	pthread_join(f, NULL);
//	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	fr_free(&fr);
}

static void __test_nodrop(void)
{
	pthread_t f, p1, p2;
	int i, ret;

	printf("\n\n\ntest nodrop\n\n\n");
	ret = fr_alloc(&fr, "frlib-nodrop", 0x155555,
				FR_FLAG_RING(4) | FR_FLAG_NODROP);

	__frt_thread_on = 1;
	pthread_create(&f, NULL, nfeeder, NULL);
	usleep(1000000);
	pthread_create(&p1, NULL, nproslow, NULL);
	for(i = 10; i--; sleep(1));
	__frt_thread_on = 0;
	pthread_join(f, NULL);
	pthread_join(p1, NULL);
	fr_free(&fr);
}

static void __test_float_nodrop(void)
{
	pthread_t f, p1, p2;
	int i, ret;

	printf("\n\n\ntest nodrop_float\n\n\n");
	ret = fr_alloc(&fr, "frlib-nodrop", 0xa00000,
				FR_FLAG_FLOAT | FR_FLAG_NODROP
				| FR_FLAG_MAX(0x150000));

	__frt_thread_on = 1;
	pthread_create(&f, NULL, feeder, NULL);
	usleep(1000000);
	pthread_create(&p1, NULL, proslow, NULL);
	for(i = 10; i--; sleep(1));
	__frt_thread_on = 0;
	pthread_join(f, NULL);
	pthread_join(p1, NULL);
	fr_free(&fr);
}

static void __test_protected(void)
{
	pthread_t f, p1, p2;
	int i, ret;

	printf("\n\n\ntest protected\n\n\n");
	ret = fr_alloc(&fr, "frlib-protected", 0x155555,
				FR_FLAG_RING(4) | FR_FLAG_PROTECTED);

	__frt_thread_on = 1;
	pthread_create(&f, NULL, nfeeder, NULL);
	usleep(1000000);
	pthread_create(&p1, NULL, nproslow, NULL);
	for(i = 10; i--; sleep(1));
	__frt_thread_on = 0;
	pthread_join(f, NULL);
	pthread_join(p1, NULL);

	printf("finished\n");
	usleep(5000000);
	fr_free(&fr);
}

static void __test_float_protected(void)
{
	pthread_t f, p1, p2;
	int i, ret;

	printf("\n\n\ntest protected_float\n\n\n");
	ret = fr_alloc(&fr, "frlib-protected", 0xa00000,
				FR_FLAG_FLOAT | FR_FLAG_PROTECTED
				| FR_FLAG_MAX(0x150000));

	__frt_thread_on = 1;
	pthread_create(&f, NULL, feeder, NULL);
	usleep(1000000);
	pthread_create(&p1, NULL, proslow, NULL);
	for(i = 10; i--; sleep(1));
	__frt_thread_on = 0;
	pthread_join(f, NULL);
	pthread_join(p1, NULL);
	fr_free(&fr);
}

static void __test_ctrl_c_sig(int nr) {
	printf("singal caught: %d\n", nr);
}

static void __test_ctrl_c(void)
{
	pthread_t f, p1, p2;
	int i, ret;
	struct fr_buf_info buf, ref = FR_BUFINITIALIZER;

	printf("\n\n\ntest ctrl-c\n\n\n");
	ret = fr_alloc(&fr, "frlib-ctrlc", 0x1004, FR_FLAG_RING(3));
	if(ret < 0) {
		printf("alloc fr failed: %d\n", ret);
		return ;
	}

	signal(SIGALRM, __test_ctrl_c_sig);
	fr_INITBUF(&buf, &fr);
	fr_get_buf(&buf);
	fr_put_buf(&buf);

	fr_INITBUF(&ref, &fr);
	printf("get first: ...\n");
	fr_get_ref(&ref);
	fr_put_ref(&ref);
	printf("ok\n");
	while(1) {
		printf("get second: ...\n");
		ret = fr_get_ref(&ref);
		if(ret < 0) continue;
		fr_put_ref(&ref);
		printf("ok\n");
		break;
	}

	fr_free(&fr);
}

int main(int argc, char *argv[])
{
	if(argc == 1 || !strcmp(argv[1], "all")) {
		__test_alloc();
		__test_getput();
		__test_single();
		__test_float();
		__test_name();
		__test_nodrop();
		__test_float_nodrop();
		__test_protected();
		__test_float_protected();
		__test_ctrl_c();
	}
	else if (!strcmp(argv[1], "float"))
	{
		__test_float();
	}
	else if (!strcmp(argv[1], "nodrop"))
	{
		__test_nodrop();
	}
	else if (!strcmp(argv[1], "fnodrop"))
	{
		__test_float_nodrop();
	}
	else if (!strcmp(argv[1], "protect"))
	{
		__test_protected();
	}
	else if (!strcmp(argv[1], "fprotect"))
	{
		__test_float_protected();
	}
	else if (!strcmp(argv[1], "getput"))
	{
		__test_getput();
	}
	else if (!strcmp(argv[1], "single"))
	{
		__test_single();
	}
	else if (!strcmp(argv[1], "name"))
	{
		__test_name();
	}
	else if (!strcmp(argv[1], "ctrl-c"))
	{
		__test_ctrl_c();
	}


	return 0;
}
