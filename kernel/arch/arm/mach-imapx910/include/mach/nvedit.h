
#ifndef _IMAP_NVEDIT_H_
#define	_IMAP_NVEDIT_H_

#define CONFIG_ENV_SIZE 0x4000
#define ENV_SIZE (CONFIG_ENV_SIZE - 4)

typedef struct environment_s {
    uint32_t        crc;            /* CRC32 over data bytes        */
    unsigned char   data[ENV_SIZE]; /* Environment data             */
} env_t;

extern int initenv(void);
extern int printenv(void);
extern int setenv (const char *name, const char *data);
extern int setenv_compact (const char *desc);
extern char *getenv (char *name);
extern int cleanenv (void);
extern int saveenv (void);
extern uint32_t crc32_uboot (uint32_t crc, const uint8_t *buf, uint32_t len);

#endif /* _IMAP_NVEDIT_H_ */

