#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <md5.h>

static uint64_t msg_offset = 0;
static uint64_t msg_len = 0;
static uint8_t *data_buf = NULL;

int md5_init(uint32_t len)
{	
	msg_len = (uint64_t)len;
	msg_offset = 0;
	data_buf = malloc(len);
	if (!data_buf)
		return -1;
	return 0;
}

int md5_data(uint8_t *buf, uint32_t len)
{
	if (data_buf)
		memcpy(data_buf + msg_offset, buf, len);
	msg_offset += (uint64_t)len;

	return 0;
}

int md5_value(uint8_t *buf)
{
	uint8_t tmp[32];
	uint8_t i = 0;

	/* check the len */
	if(!msg_len)
	    return -1;
	if (data_buf) {
		memset(tmp, 0, 32);
    	MD5((const unsigned char *)data_buf, msg_len, (unsigned char *)tmp);
    	free(data_buf);
    }
	for(i = 0; i < 16; i++)
	    buf[i] = tmp[i];

	return 0;
}

