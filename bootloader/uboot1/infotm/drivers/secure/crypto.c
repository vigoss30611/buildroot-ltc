#include <common.h>
#include <crypto_module.h>
#include <efuse.h>

/*
 * aes 128key encryption or decryption
 * you`re supposed to reset all the security module if you want to use
 * custom key after using the security key
 *
 * by Larry
 */

/*
 * ss aes init
 *
 * type : ENCRYPTION or DECRYPTION
 * key : 256 bit AES key, if NULL use ISK
 * iv  : 128 bit initial vector
 * return : 0 successful    -1 failed
 */
int ss_aes_init(uint32_t type, uint8_t *key, uint8_t *iv)
{
	if(!ecfg_check_flag(ECFG_ENABLE_CRYPTO)) {
		printf("noss\n");
		return 0;
	}

	/* init */
	if(key == NULL){
        if(block_cipher_aes_init(type, DEVICEID, key, iv))
		    return -1;
	}else{
	    if(block_cipher_aes_init(type, CUSTOM_KEY, key, iv))
		    return -1;
	}

	return 0;
}

/*
 * ss aes data
 *
 * data : indata buff
 * buf : outdata buff
 * len :  data length to transfer and 16 bytes aligned
 * return : 0 seccessful    -1 failed
 */
int ss_aes_data(uint8_t *data, uint8_t *buf, uint32_t len)
{
	if(!ecfg_check_flag(ECFG_ENABLE_CRYPTO)) {
		printf("noss\n");
		return 0;
	}

	/* test lenth should be "16" */
	len = (len + 0xf) & ~0xf;

	/* operation */
    if(block_cipher_operation(data, buf, len))
	    return -1;

	return 0;
}

#if 0
int ss_decrypt(void *data, uint32_t len, void *key)
{
	uint8_t buf[16];
	uint32_t offs = 0;
    
	printf("decrypting data: 0x%x\n", len);
	while(offs < len)
	{
//		if(ss_aes_decrypt(data + offs, buf, key))
    	{
			printf("decrypt failed.\n");
			return -1;
		}

		memcpy(data + offs, buf, min(len - offs, 16));
		offs += 16;
	}

	return 0;
}

int ss_encrypt(void *data, uint32_t len, void *key)
{
	uint8_t buf[16];
	uint32_t offs = 0;


	printf("encrypting data: 0x%x\n", len);
	while(offs < len)
	{
//		if(ss_aes_encrypt(data + offs, buf, key))
		{
			printf("encrypt failed.\n");
			return -1;
		}

		memcpy(data + offs, buf, min(len - offs, 16));
		offs += 16;
	}

	return 0;
}
#endif

void ss_lock_isk(void)
{
	if(!ecfg_check_flag(ECFG_ENABLE_CRYPTO)) {
		printf("noss\n");
		return ;
	}

	block_cipher_lock_isk();
}


