
#ifndef __IUW_SERVER_H__
#define __IUW_SERVER_H__
#include <list.h>

#define MAX_ETH_COUNT 20
int pthread[MAX_ETH_COUNT];

struct sv_callback {
	void	(* connect)(int cd);
	void	(* disconnect)(int cd);
};

struct svline_node {
	pthread_t	pid;
	int			cd;
	uint8_t		*buffer;
	struct list_head list;
};

struct sv_transfer_api {
	int (*regist)(struct sv_callback *cb);
	int (*start)(void);
	int (*stop)(void);
	int (*read)(int cd, uint8_t *buf, int len);
	int (*write)(int cd, uint8_t *buf, int len);
	int (*put)(int cd);
};

extern int iuw_get_connect_device(char *buf);

extern struct svline_node * svline_self(void);
extern struct svline_node * pid_to_svline(pthread_t *pid);
extern struct svline_node * cd_to_svline(int cd);
extern int svline_read(struct svline_node *svl, int len);
extern int svline_write(struct svline_node *svl, int len);
extern int svline_release(struct svline_node *svl);
extern void server_clean(void);
extern int server_setdev(const char *dev);
extern int server_setfunc(void * (*func)(void *));
extern int server_setbsize(int size);
extern int server_start(void);
extern void server_stop(int a);
extern int server_islocked(void);
extern void svline_print_buffer(struct svline_node *svl, int len);

#if 0
static int svline_pend_set(struct svline_node *svl);
static int svline_pend_clear(struct svline_node *svl);
static void svline_wait(struct svline_node *svl);
//static int svline_wait_timeout(struct svline_node *svl, int seconds);
static int svline_init(void);
static int svline_finish(struct svline_node *svl);
static struct svline_node * svline_create(int cd);
/* return 0 if successful, -1 if failed. */
static struct svline_node * cd_to_svline(int cd);
static struct svline_node * pid_to_svline(pthread_t *pid);
static struct svline_node * svline_self(void);

static int svline_read(struct svline_node *svl, int len);
static int svline_write(struct svline_node *svl, int len);
#endif

#define SV_PACKAGE_BUFFER_SIZE		(0x100000)

/* IUW svline command */
#define IUW_SVL_VSWR		0x10
#define IUW_SVL_VSRD		0x11
#define IUW_SVL_NBWR		0x12
#define IUW_SVL_NBRD		0x13
#define IUW_SVL_NRWR		0x14
#define IUW_SVL_NRRD		0x15
#define IUW_SVL_NFWR		0x16
#define IUW_SVL_NFRD        0x17

#define IUW_SVL_MMWR		0x20
#define IUW_SVL_MMRD		0x21

#define IUW_SVL_CONFIG		0x30
#define IUW_SVL_JUMP		0x31
#define IUW_SVL_IDENTIFY	0x33

#define IUW_SVL_ENC			0x42
#define IUW_SVL_DEC			0x44
#define IUW_SVL_HASH		0x46

#define IUW_SVL_READY		0x02
#define IUW_SVL_FINISH		0xe0
#define IUW_SVL_ERROR		0xf0

#define IUW_SUB_STEP		0x40
#define IUW_SUB_MAC			0x41

#define IUW_SVL_MAGIC		0xaa
#define IUW_SVL_LEN			0x20

#define opt_is_read(x)		(x & 0x1)
#define opt_is_write(x)		(!(x & 0x1))

//#define sv_dbg(x...) printf("svline: " x)
#define sv_dbg(x...)
#define sv_err(x...) printf("svline: " x)

#define IUW_LOCK_FILE		"/var/lock/iuw"


#endif /* __IUW_SERVER_H__ */

