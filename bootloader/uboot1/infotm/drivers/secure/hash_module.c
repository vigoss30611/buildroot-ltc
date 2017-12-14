#include <common.h>
#include <hash_module.h>
#include <security_dma.h>

/*
 * it is mini interface of hash module
 * you could use md5, sha1 or sha256
 * we use the hrdma sdma to transfer data, and should be sure that the len is less than 16bit
 */

/*
 * hash start
 */
static void hash_module_start(void)
{
    rSEC_HASH_CONTROL |= 1 << HASH_START_BIT;
}

/*
 * hash clear
 */
uint32_t hash_module_clear(void)
{
	uint32_t ret = 0;
	uint64_t t = 0;

    rSEC_HASH_CONTROL = 1 << HASH_CLEAR_BIT;

	t = get_ticks();
	ret = rSEC_HASH_CONTROL;
	while((ret & (1 << HASH_CLEAR_BIT)) == (1 << HASH_CLEAR_BIT)){
	    ret = rSEC_HASH_CONTROL;
		if(get_ticks() > t + 2000)
		    return 1;
	}

	return 0;
}

/*
 * hash control
 * 
 * mode : ENABLE or DISABLE
 */
static void hash_module_control(uint32_t mode)
{
	uint32_t ret = 0;
	
	ret = rSEC_HASH_CONTROL;
	ret &= ~(1 << HASH_ENALBE_BIT);
	ret |= (mode << HASH_ENALBE_BIT);
	rSEC_HASH_CONTROL = ret;
}

/*
 * hash data get
 * 
 * each time read 256bit , 
 * so make sure output be "256bit"
 */
static uint32_t hash_module_get_outdata(uint32_t *outdata)
{
    uint32_t i = 0;
	uint64_t t = 0;
    
	/* check the msg is done */
	t = get_ticks();
	i = rSEC_HASH_STATUS;
        while((i & (1 << HASH_MSG_DONE)) != (1 << HASH_MSG_DONE))
        {
    	    i = rSEC_HASH_STATUS;	
	    if(get_ticks() > t + 2000)
	       return 1;
        }
    
	for(i = 0; i < 8; i++)
            *(outdata + i) = rSEC_HASH_RESULT(i);

        rSEC_HASH_STATUS = 1 << HASH_MSG_DONE;

	return 0;
}

/*
 * hash module get result
 */
uint32_t hash_module_get_result(uint8_t *buf)
{
    if(hash_module_get_outdata((uint32_t *)buf))
        return 1;
	 
	hash_module_control(DISABLE);
	return 0;
}

/*
 * hash init
 * 
 * mode : MD5, SHA1, SHA256
 */
uint32_t hash_module_init(uint32_t mode, uint32_t msg_high, uint32_t msg_low)
{
	/* disable hash */
	hash_module_control(DISABLE);
	/* hash clear */
	if(hash_module_clear())
	    return 1;
	/* hash fifo set */
    rSEC_HASH_FIFO_MODE_EN = (ENABLE << HASH_FIFO_MODE_EN) | (SRC_INDEPENDENDT_SOURCE << HASH_HASH_SRC_SEL);
	/* hash init */
    rSEC_HASH_CONTROL = mode << HASH_HASH_ENGINE_SEL;
	/* set msg */
	rSEC_HASH_MSG_SIZE_HIGH = msg_high;
	rSEC_HASH_MSG_SIZE_LOW = msg_low;
	// enable hash
	hash_module_control(ENABLE);
	// hash start
	hash_module_start();

	return 0;
}

/*
 * hash dma transfer
 *
 * here we use HRDMA
 */
uint32_t hash_dma_transfer(uint8_t *buf, uint32_t len)
{
    uint64_t t = 0;
    int width, remain = len;

    while(remain) {
	    width = (remain > 0x8000)? 0x8000: remain;

	    /* init */
	    if(dma_sdma_init(HRDMA, (uint32_t)buf + (len - remain), width))
		    return 1;

	    /* dma start */
	    dma_start(HRDMA);

	    t = get_ticks();
	    while(!dma_get_status(HRDMA, SDMA_FINISH)){
		    if(get_ticks() > t + 1000000)
			    return 1;
	    }

	    remain -= width;
    }

    return 0;
}

