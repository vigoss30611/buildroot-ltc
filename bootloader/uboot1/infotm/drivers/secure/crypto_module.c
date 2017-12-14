#include <common.h>
#include <crypto_module.h>
#include <security_dma.h>

/*
 * here we only ues aes, 128key, 
 * and debug, say, cpu operations directly
 */

/*
 * check R bit
 */
uint32_t check_register_bit(uint32_t value, uint32_t bit)
{
    if(value & (1 << bit))
	    return 1;
	return 0;
}

/*
 * BLKC control
 * 
 * ENABLE or DISABLE
 */
static void block_cipher_control(uint32_t mode)
{
	uint32_t ret = 0;
	
	ret = rSEC_BLKC_CONTROL;
	ret &= ~(1 << BLKC_ENABLE);
	ret |= (mode << BLKC_ENABLE);
	rSEC_BLKC_CONTROL = ret;
}

/*
 * BLCK rst 
 */
uint32_t block_cipher_reset(void)
{
	uint64_t t = 0;
	uint32_t ret = 0;

    rSEC_BLKC_CONTROL = 1 << RC4_RST;
    
	t = get_ticks();
	ret = rSEC_BLKC_CONTROL;
	while(ret & (1 << RC4_RST)){
	    ret = rSEC_BLKC_CONTROL;
		if(get_ticks() > t + 2000)
		    return 1;
	}

	return 0;
}

/*
 * BLKC set key
 *
 * only for aes 256key
 */
static void block_cipher_set_key(uint32_t *key)
{
    uint32_t i = 0;

	for(i = 0; i < 8; i++)
	     rSEC_AES_KEYDATA(i) = *(key + i);
}

/*
 * BLKC set iv
 *
 * only for aes iv
 */
static void block_cipher_set_iv(uint32_t *iv)
{
    uint32_t i = 0;

	for(i = 0; i < 4; i++)
	    rSEC_AES_IVDATA(i) = *(iv + i);
}

/*
 * BLKC lock isk
 *
 * it is used to set the security key
 * if it has been set , you must reset to unlock if you want to use deviceid
 */
void block_cipher_lock_isk(void)
{
    uint32_t ret = 0;

	ret = rSEC_BLKC_CONTROL;
	ret &= ~(0x3 << USE_DEVICEID);
	ret |= SECURITY_KEY << USE_DEVICEID;
	rSEC_BLKC_CONTROL = ret;
}

/*
 * BLKC aes init
 *
 * we default that we use aes 256key CBC mode
 * crypto: ENCRYPTION or DECRYPTION
 * keymode : SECURITY_KEY or CUSTOM_KEY
 * key : 256bit key
 * iv : 128 bit iv
 */
uint32_t block_cipher_aes_init(uint32_t crypto, uint32_t keymode, uint8_t *key, uint8_t *iv)
{
	uint64_t t = 0;

	/* reset */
	if(block_cipher_reset())
	    return 1;
    /* here we use dma , so it should be open */
    rSEC_BLKC_FIFO_MODE_EN = 1;
	if(crypto != ENCRYPTION && crypto != DECRYPTION)
	    return 1;
	if(keymode != SECURITY_KEY && keymode != CUSTOM_KEY && keymode != DEVICEID)
        return 1;
	/* config */
    rSEC_BLKC_CONTROL = (CBC << BLKC_CHAIN_MODE) | (crypto << BLKC_MODE) | (AES << BLKC_ENGINE_SEL) | (AES_KEY_256 << AES_KEYSIZE) | (keymode << USE_DEVICEID);
    /* set key */
	if(keymode == CUSTOM_KEY)
	    block_cipher_set_key((uint32_t *)key);
    /* set iv */
	block_cipher_set_iv((uint32_t *)iv);
	/* check key is done */
	if(keymode == CUSTOM_KEY){
            t = get_ticks();
            while(!check_register_bit(rSEC_BLKC_STATUS, AES_KEYDONE)){
	        if(get_ticks() > t + 1000)
		    return 1;
	    }
	} 
	/* check iv is set up */
	t = get_ticks();
	while(!check_register_bit(rSEC_BLKC_STATUS, AES_IVDATA_SETUP)){
	    if(get_ticks() >t + 1000)
		    return 1;
	}
    /* enable block cipher */
    block_cipher_control(ENABLE);

    return 0;
}

/*
 * block cipher operation
 *
 * indata : indata buff
 * outdata : outdata buff
 * len : data to transfer by byte
 * we use brdam and btdma
 */
uint32_t block_cipher_operation(uint8_t *indata, uint8_t *outdata, uint32_t len)
{
     uint64_t t = 0;
    int width, remain = len;

    while(remain) {
	    width = (remain > 0x8000)? 0x8000: remain;

	    /* init */
	    if(dma_sdma_init(BRDMA, (uint32_t)indata + (len - remain), width))
		    return 1;
	    if(dma_sdma_init(BTDMA, (uint32_t)outdata + (len - remain), width))
		    return 1;

	    /* dma start */
	    dma_start(BRDMA);
	    dma_start(BTDMA);

	    t = get_ticks();
	    while(!dma_get_status(BTDMA, SDMA_FINISH)){
		    if(get_ticks() > t + 1000000)
			    return 1;
	    }
	    if(!dma_get_status(BRDMA, SDMA_FINISH))
		    return 1;

	    remain -= width;
    }

    return 0;
}
