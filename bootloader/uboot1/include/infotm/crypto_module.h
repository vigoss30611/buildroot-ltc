#ifndef __CRYPTO_MODULE_H__
#define __CRYPTO_MODULE_H__
/*
 * block cipher
 */

/* blkc_control R */
#define BLKC_ENABLE                  0      /* 1:enable   0:disable */
#define BLKC_CHAIN_MODE              1      /* block cipher chain mode  [5:1]  1:ECB 2:CBC 3:CFB 4:OFB 5:CTR */
#define BLKC_MODE                    6      /* 1: encryption   0: decryption */
#define BLKC_ENGINE_SEL              7      /* [8:7] 00:AES  01:DES  10:TDES  11:RC4 */
#define BLKC_STREAM_MODE             9      /* [12:9] 0000: 8bit  0001: 16bit  .. */
#define AES_KEYSIZE                  13     /* [14:13] 00: 128   01: 192   10: 256 */
#define TDES_MODE                    15     /* [17:15] 15bit: DES1  16bit: DES2  17bit: DES3 */
#define USE_CUSTOM_SBOX              18     /* 0: default   1: custom */
#define USE_DEVICEID                 19     /* 0: custom key 1: device ID */
#define RC4_KEYSIZE                  21     /* [24:20] byte number: 00000: 1  00001: 2 ... */
#define RC4_USE_SW_KSA               26      
#define RC4_SW_KSA_DONE              29     /* aftern sw set the sw , it should be written to 1 */
#define RC4_RST                      31     /* when you set sw and re-init, you need to reset the rc4 */

#define ECB                          0x1
#define CBC                          0x2
#define CFB                          0x4
#define OFB                          0x8
#define CTR                          0x10

#define ENABLE                       1
#define DISABLE                      0

#define ENCRYPTION                   1
#define DECRYPTION                   0

#define AES                          0
#define DES                          1
#define TDES                         2
#define RC4                          3

#define AES_KEY_128                  0
#define AES_KEY_192                  1
#define AES_KEY_256                  2     

#define CUSTOM_KEY                   0
#define DEVICEID                     1
#define SECURITY_KEY                 3

/* blkc fifo mode */
#define BLKC_FIFO_MODE_EN            (1 << 0)     /* enalbe fifo mode */

/* blkc status */
#define AES_CNTDATA_SETUP            0     /* setup done */
#define AES_CNTDATA_ERROR            1      
#define AES_KEYDATA_SETUP            2     
#define AES_KEYDONE                  3     /* key expansion */
#define AES_IVDATA_SETUP             4      
#define AES_IVDATA_ERROR             5      
#define AES_BUSY                     6     
#define AES_IN_READY                 7     
#define AES_OUT_READY                8        

/* function */
extern uint32_t block_cipher_reset(void);
extern uint32_t block_cipher_aes_init(uint32_t crypto, uint32_t keymode, uint8_t *key, uint8_t *iv);	
extern uint32_t block_cipher_operation(uint8_t *indata, uint8_t *outdata, uint32_t len);
extern void block_cipher_lock_isk(void);

#endif /* crypto.h */
