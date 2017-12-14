#ifndef __ISPOST_H__
#define __ISPOST_H__

#define TAG "Ispost: "

#ifdef ISPOST_DEBUG
#define ISPOST_DBG(fmt...)     printk(KERN_ERR TAG""fmt)
#else
#define ISPOST_DBG(...)        do {} while (0)
#endif

#define ISPOST_ERR(fmt...)     printk(KERN_ERR TAG""fmt)
#define ISPOST_INFO(fmt...)    printk(KERN_ERR TAG""fmt)


#define ISPOST_MAGIC	'b'
#define ISPOST_IOCMAX	10
#define ISPOST_NUM		8
#define ISPOST_NAME		"ispost"

#define ISPOST_ENV		  		_IOW(ISPOST_MAGIC, 1, unsigned long)
#define ISPOST_FULL_TRIGGER   _IOW(ISPOST_MAGIC, 2, unsigned long)

#endif
