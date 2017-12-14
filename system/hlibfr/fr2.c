#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fr-user.h>
#include <fr-lib.h>
#include <list.h>

#define log(x...) printf("frlib: " x)

static int fd = -1;
static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct fr_buf_info list_of_bufs;
#define assert_fd() do { if(fd < 0) return 0;} while(0)

void *fr_alloc_single(const char *name, int size, uint32_t *phys)
{
	struct fr_info fr = {
		.count = count,
		.size = size,
	};
	int ret;

	assert_fd();
	strncpy(fr.name, name, FR_INFO_SZ_NAME);

	ret = ioctl(fd, FRING_ALLOC, &fr);
	if(ret < 0) {
		log("failed to alloc buffer\n");
		return NULL;
	}

	struct fr_buf_info *buf = malloc(sizeof(struct fr_buf_info));
	if(!buf) {
		log("failed to alloc fr_buf_info node\n");
		return NULL;
	}

	fr_INITBUF(buf, &fr);
	ret = fr_get_buf(buf);
	if(ret < 0) {
		log("failed to get buffer from fr %s\n", fr.name);
		free(buf);
		return NULL;
	}

	*phys = buf->phys_addr;
	pthread_mutex_lock(&list_mutex);
	list_add_tail(buf, &list_of_bufs);
	pthread_mutex_unlock(&list_mutex);

	return buf->virt_addr;
}

void fr_free_single(void *virt, uint32_t phys)
{
	struct fr_buf_info *buf;

	assert_fd();

	pthread_mutex_lock(&list_mutex);
	list_for_each(buf, list_of_bufs) {
		if((phys && (phys == buf->phys_addr)) ||
			(virt && (virt == buf->virt_addr))) {
			fr_put_buf(buf);
			ioctl(fd, FRING_FREE, buf->fr);
			break;
		}
	}
	list_del(buf);
	free(buf);
	pthread_mutex_unlock(&list_mutex);
}


