#ifndef __FODET_H__
#define __FODET_H__

#define TAG "[Drv_Fodet]: "

#ifdef FODET_DEBUG
#define FODET_DBG(fmt...)     printk(KERN_ERR TAG""fmt)
#else
#define FODET_DBG(...)        do {} while (0)
#endif

#define FODET_ERR(fmt...)     printk(KERN_ERR TAG"ERROR: "fmt)
#define FODET_INFO(fmt...)    printk(KERN_INFO TAG"INFO: "fmt)
#define FODET_LOG(fmt...)    printk(KERN_DEBUG TAG"LOG: "fmt)
//#define FODET_DBG(fmt...)    printk(KERN_DEBUG __VA_ARGS__ TAG""fmt)

//#define FODET_MAGIC	'b'
#define FODET_MAGIC	'k'
#define FODET_IOCMAX	10
#define FODET_NUM	8
#define FODET_NAME	"fodetv2"


#define FODET_ENV		  	_IOW(FODET_MAGIC, 1, unsigned long)
#define FODET_INITIAL	  		_IOW(FODET_MAGIC, 2, unsigned long)
#define FODET_OBJECT_DETECT  	_IOW(FODET_MAGIC, 3, unsigned long)
#define FODET_GET_COORDINATE _IOR(FODET_MAGIC, 4, unsigned long)
#endif
