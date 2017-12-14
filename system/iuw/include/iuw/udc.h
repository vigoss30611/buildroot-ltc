
#ifndef __IUW_UDC_H__
#define __IUW_UDC_H__

/* TODO: add headings here */
extern int udc_register_callback(struct sv_callback *cb);
extern int udc_start(void);
extern int udc_stop(void);
extern int udc_read(int cd, uint8_t *buf, int len);
extern int udc_write(int cd, uint8_t *buf, int len);
extern int udc_put_cd(int cd);

#define MAX_MONITOR_COUNT	10
#define UDC_DEVICE_BASE		"/dev/secbulk%d"

enum udc_stat {
	UDC_IDLE = 0,
	UDC_READY,
	UDC_BUSY,
};

struct udc_node {
	int	fd;
	int stat;
};

struct udc_dev {
	struct sv_callback cb;
	pthread_t pid;
	struct udc_node nodes[MAX_MONITOR_COUNT];
};

#endif /* __IUW_UDC_H__ */

