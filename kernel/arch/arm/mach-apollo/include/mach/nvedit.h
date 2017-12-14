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

#endif /* _IMAP_NVEDIT_H_ */

