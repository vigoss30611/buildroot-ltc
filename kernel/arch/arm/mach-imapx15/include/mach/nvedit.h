
#ifndef _IMAP_NVEDIT_H_
#define	_IMAP_NVEDIT_H_

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/mutex.h>

#define CONFIG_ENV_SIZE 0x4000
#define ENV_SIZE (CONFIG_ENV_SIZE - 4)

typedef struct environment_s {
    uint32_t        crc;            /* CRC32 over data bytes        */
    unsigned char   data[ENV_SIZE]; /* Environment data             */
} env_t;

struct env_t {
    uint32_t crc;            /* CRC32 over data bytes        */
    unsigned char data[ENV_SIZE]; /* Environment data             */
    struct class cls;
    struct mutex env_lock;
    int (*read_data)(struct env_t *env, loff_t from, size_t len, uint8_t *buf);
    int (*write_data)(struct env_t *env, loff_t from, size_t len, uint8_t *buf);
};


extern int infotm_register_env(struct env_t *env);
extern char *infotm_getenv (char *name);



#ifdef CONFIG_MTD_FTL
extern int initenv(void);
extern int printenv(void);
extern int setenv (const char *name, const char *data);
extern int setenv_compact (const char *desc);
extern char *getenv (char *name);
extern int cleanenv (void);
extern int saveenv (void);
extern uint32_t crc32_uboot (uint32_t crc, const uint8_t *buf, uint32_t len);
#else
#define initenv(x...)		(int)(0)
#define printenv(x...)		(int)(0)
#define setenv(x...)		(int)(0)
#define setenv_compact(x...)	(int)(0)
#define	getenv(x...)		(char*)(0)
#define cleanenv(x...)		(int)(0)
#define saveenv(x...)		(int)(0)
#define crc32_uboot(x...)	(uint32_t)(0)
#endif

#endif /* _IMAP_NVEDIT_H_ */

