#include <common.h>
#include <hash_module.h>

static uint64_t msg_len = 0;

/*
 * md5 init
 *
 * len : length of data to be calculated
 * return : 0  ok      -1 failed
 */
int md5_init(uint32_t len)
{	
	msg_len = (uint64_t)len;
        msg_len <<= 3;

	if(hash_module_init(MD5, (uint32_t)(msg_len >> 32), (uint32_t)msg_len))
	    return -1;

	msg_len >>= 3;
	return 0;
}

/*
 * md5 data
 *
 * buf : buff holding data
 * len : length of data to be calculated
 * return : 0  accepted
 *         -1  exceed the value set when initialized
 *         -2  other error
 */
int md5_data(uint8_t *buf, uint32_t len)
{
	/* exceed the len */
    if(msg_len < (uint64_t)len)
	    return -1;
    msg_len -= (uint64_t)len;
	/* transfer the data */
	if(hash_dma_transfer(buf, len))
	    return -2;

	return 0;
}

/*
 * md5 value
 *
 * buf: buff to hold the result
 * return : 0  successful
 *         -1  data is not enough
 *         -2  other errors
 */
int md5_value(uint8_t *buf)
{
	uint8_t tmp[32];
	uint8_t i = 0;

	/* check the len */
	if(msg_len)
	    return -1;
    /* get result */
	if(hash_module_get_result(tmp))
	    return -2;
	for(i = 0; i < 16; i++)
	    buf[i] = tmp[i];

	return 0;
}

