#ifndef __IUW_HASH_H__
#define __IUW_HASH_H__

enum hash_type {
	IUW_HASH_CRC = 0,
	IUW_HASH_MD5,
	IUW_HASH_SHA1,
	IUW_HASH_SHA256,
};

struct hash_func {
	int len;
	int (*init) (uint32_t len);
	int (*data) (uint8_t *buf, uint32_t len);
	int (*value) (uint8_t *buf);
};

extern int hash_init(enum hash_type type, uint32_t len);
extern int hash_data(void *buf, uint32_t len);
extern int hash_value(void *buf);
extern int inline hash_len(void);

#endif /* __IUW_HASH_H__ */
