/* frt is short for frame-ring test */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/printk.h>
#include <linux/hrtimer.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/random.h>
#include <linux/fr.h>

#include <asm/ioctl.h>
#include <asm/uaccess.h>

static long long __getns(void)
{
	ktime_t a;
	a = ktime_get_boottime();
	return a.tv64;
}

void frt_allocandfree(void)
{
	struct fr* fr;

	fr = __fr_alloc("ktst_1", SZ_1M, FR_FLAG_RING(5));

	__fr_free(fr);
}

void frt_getputbuf(void)
{
	struct fr *fr1 = __fr_alloc("ktst_2", SZ_1M, FR_FLAG_RING(7)),
		*fr2 = __fr_alloc("ktst_3", 666, FR_FLAG_RING(3));
	struct fr_buf *buf;
	int i, ret;

	printk(KERN_ERR "\ntest fr1\n");
	for(i = 0; i < 100; i++) {
		ret = fr_get_buf(fr1, &buf);
		buf->timestamp = __getns();
		fr_put_buf(fr1, buf);
		fr_buf_print(buf);
	}

	printk(KERN_ERR "\ntest fr2\n");
	for(i = 0; i < 100; i++) {
		ret = fr_get_buf(fr2, &buf);
		buf->timestamp = __getns();
		fr_put_buf(fr2, buf);
		fr_buf_print(buf);
	}

	__fr_free(fr1);
	__fr_free(fr2);

}

static int __frt_thread_on = 0;
static int frt_buf_feeder(void *arg)
{
	struct fr *fr = arg;
	struct fr_buf *buf;
	int ret;

	printk(KERN_ERR "%s:%s started (%d)\n", __func__, fr->name,
		__frt_thread_on);
	while(__frt_thread_on) {
		/* feed buf every second */
		ret = fr_get_buf(fr, &buf);
//		printk(KERN_ERR "%s -> %s: get %p\n", current->comm,
//		 fr->name, buf->virt_addr);
		msleep(1000);
		buf->timestamp = __getns();
		fr_put_buf(fr, buf);
		printk(KERN_ERR "%s -> %s: put %p\n", current->comm,
		 fr->name, buf->virt_addr);
	}
	return 0;
}

static int frt_float_feeder(void *arg)
{
	struct fr *fr = arg;
	struct fr_buf *buf;
	int ret;

	printk(KERN_ERR "%s:%s started (%d)\n", __func__, fr->name,
		__frt_thread_on);
	while(__frt_thread_on) {
		/* feed buf every second */
		ret = fr_get_buf(fr, &buf);
//		printk(KERN_ERR "%s -> %s: get %p\n", current->comm,
//		 fr->name, buf->virt_addr);
		msleep(10);
		buf->timestamp = __getns();
		get_random_bytes(&buf->size, 4);
		buf->size &= 0xfffff;
		buf->size += 1;
		fr_put_buf(fr, buf);
		printk(KERN_ERR "%s -> %s: put %p, size %x\n", current->comm,
		 fr->name, buf->virt_addr, buf->size);
	}
	return 0;
}

static int frt_buf_processer(struct fr *fr, int speed, struct fr_buf *self_ref)
{
	struct fr_buf *ref = self_ref;
	int ret;

	while(__frt_thread_on) {
		ret = fr_get_ref(fr, &ref);
		if(!ref) {
			printk(KERN_ERR "%s << %s: no ref\n",
		  current->comm, fr->name);
			msleep(5);
			continue;
		}
		printk(KERN_ERR "%s << %s: got %p, size %x\n",
			 current->comm, fr->name,
			ref->virt_addr, ref->size);
		msleep(speed); /* pretend to process */
		fr_put_ref(fr, ref);
	}
	return 0;
}

static int frt_buf_profast(void *arg)
{
	static struct fr_buf *ref = NULL;
	struct fr *fr = arg;
	frt_buf_processer(fr, 10, ref);
	return 0;
}

static int frt_buf_proslow(void *arg)
{
	static struct fr_buf *ref = NULL;
	struct fr *fr = arg;
	frt_buf_processer(fr, 100, ref);
	return 0;
}

static int frt_buf_consulter(void *arg)
{
	struct fr *fr = arg;
	struct fr_buf *ref = NULL;
	int ret;

	while(__frt_thread_on) {
		ret = fr_get_ref(fr, &ref);
		if(!ref) {
			printk(KERN_ERR "<< %s: no ref\n", fr->name);
			msleep(5);
			continue;
		}
		printk(KERN_ERR "%s << %s: got %p, stamp %llx\n",
			current->comm, fr->name,
			ref->virt_addr, ref->timestamp);
		msleep(200); /* pretend to consult */
		fr_put_ref(fr, ref);
	}
	return 0;
}

void frt_refandbuf(void)
{
	struct fr *fr1 = __fr_alloc("ktst:p", SZ_1M, FR_FLAG_RING(7)),
		*fr2 = __fr_alloc("ktst:c", SZ_1M + 382, FR_FLAG_RING(3));

	struct task_struct *f1, *f2, *p, *c;

	__frt_thread_on = 1;
	/* two feeder feed buffer every 1s */
	f1 = kthread_create(frt_buf_feeder, fr1, "fr_f1");
	f2 = kthread_create(frt_buf_feeder, fr2, "fr_f2");

	/* buffer post processer, process buffer sequentially,
	 * the processing speed is fast (30ms)
	 */
	p = kthread_create(frt_buf_profast, fr1, "fr_p1");

	/* buffer consulter, always consult the latest buffer,
	 * the consulting speed is 200ms
	 */
	c = kthread_create(frt_buf_consulter, fr2, "fr_c1");

	wake_up_process(f1);
	wake_up_process(f2);
	wake_up_process(p);
	wake_up_process(c);
}

void frt_refandbuf_end(void)
{
	__frt_thread_on = 0;
}

void frt_refandbuf_float(void)
{
	struct fr *fr1 = __fr_alloc("ktst:float", SZ_16M,
				 FR_FLAG_MAX(SZ_1M) | FR_FLAG_FLOAT);
	struct task_struct *f1, *p1, *p2;

	__frt_thread_on = 1;

	f1 = kthread_create(frt_float_feeder, fr1, "feeder");
	p1 = kthread_create(frt_buf_profast, fr1, "prfast");
	p2 = kthread_create(frt_buf_proslow, fr1, "prslow");

	wake_up_process(f1);
	wake_up_process(p1);
	wake_up_process(p2);
}

void frt_refandbuf_nodrop(void)
{
	struct fr *fr1 = __fr_alloc("ktst:float", SZ_1M,
				 FR_FLAG_RING(5) | FR_FLAG_NODROP);
	struct task_struct *f1, *p2;

	if(!fr1) {
		printk("allocated fr1 failed\n");
		return ;
	}
	__frt_thread_on = 1;

	f1 = kthread_create(frt_float_feeder, fr1, "feeder");
	p2 = kthread_create(frt_buf_proslow, fr1, "prslow");

	wake_up_process(f1);
	msleep(1000);
	wake_up_process(p2);
}

void frt_refandbuf_nodrop_float(void)
{
	struct fr *fr1 = __fr_alloc("ktst:float", SZ_16M,
				 FR_FLAG_MAX(SZ_1M) | FR_FLAG_FLOAT | FR_FLAG_NODROP);
	struct task_struct *f1, *p2;

	if(!fr1) {
		printk("allocated fr1 failed\n");
		return ;
	}

	__frt_thread_on = 1;

	f1 = kthread_create(frt_float_feeder, fr1, "feeder");
	p2 = kthread_create(frt_buf_proslow, fr1, "prslow");

	wake_up_process(f1);
	msleep(1000);
	wake_up_process(p2);
}

