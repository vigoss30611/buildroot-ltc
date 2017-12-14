#ifndef __HASH_MODULE_H__
#define __HASH_MODULE_H__

/*
 * HASH
 */

/* rSEC_HASH_CONTROL */
#define HASH_PAUSE_BIT               0     
#define HASH_CLEAR_BIT               1     
#define HASH_START_BIT               2     
#define HASH_HASH_ENGINE_SEL         3     /* MD5 SHA1 SHA224 SHA256 MD5-HMAC SHA1-HMAC SHA224-HMAC SHA256-HMAC */
#define HASH_USE_CUSTOM_VECTOR       6     
#define HASH_ENALBE_BIT              7     

#define MD5                          0
#define SHA1                         1
#define SHA224                       2
#define SHA256                       3
#define MD5_HMAC                     4
#define SHA1_HMAC                    5s
#define SHA224_HMAC                  6
#define SHA256_HMAC                  7

/* rSEC_HASH_FIFO_MODE_EN */
#define HASH_FIFO_MODE_EN            0
#define HASH_HASH_SRC_SEL            1

#define SRC_INDEPENDENDT_SOURCE      0
#define SRC_BLOCK_CIPHER_INPUT       1
#define SRC_BLOCK_CIPHER_OUTPUT      2

/* hash status */
#define HASH_MSG_DONE                0
#define HASH_BUF_READY               1
#define HASH_CV_SETUP                2
#define HASH_CV_ERROR                3
#define HASH_KEY_SETUP               4
#define HASH_KEY_ERROR               5

#define ENABLE                       1
#define DISABLE                      0

/* function */
extern uint32_t hash_module_clear(void);
extern uint32_t hash_module_get_result(uint8_t *buf);
extern uint32_t hash_module_init(uint32_t mode, uint32_t msg_high, uint32_t msg_low);
extern uint32_t dma_reset(uint32_t dma);	
extern uint32_t hash_dma_transfer(uint8_t *buf, uint32_t len);


#endif /* hash_module.h */

