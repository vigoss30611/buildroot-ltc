#ifndef __CRYPTO_H__
#define __CRYTPO_H__

#define ENCRYPTION           1
#define DECRYPTION           0

extern int ss_aes_init(uint32_t type, uint8_t *key, uint8_t *iv);
extern int ss_aes_data(uint8_t *data, uint8_t *buf, uint32_t len);
extern void ss_lock_isk(void);

#endif /* crypto.h */
