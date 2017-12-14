#include <stdio.h>
#include <stdint.h>
#include <crc32.h>
#include <md5.h>
#include <sha1.h>
#include <sha256.h>
#include <hash.h>

static enum hash_type g_type;

static struct hash_func
hash_funcs[] = {
	{
		.init = crc32_init,
		.data = crc32_data,
		.value = crc32_value,
	},

	{
		.init = md5_init,
		.data = md5_data,
		.value = md5_value,
	},
#if 0
	{
		.init = sha1_init,
		.data = sha1_data,
		.value = sha1_value,
	}, {
		.init = sha256_init,
		.data = sha256_data,
		.value = sha256_value,
	}
#endif
};

int hash_init(enum hash_type type, uint32_t len)
{
	if(type > IUW_HASH_SHA256) {
		printf("Unknown hash type: only CRC, MD5, SHA1 and"
					" SHA256 is supported.\n");
		return -1;
	}

	g_type = type;
	return hash_funcs[type].init(len);
}

int hash_data(uint8_t *buf, uint32_t len)
{
	return hash_funcs[g_type].data(buf, len);
}

int hash_value(uint8_t *buf)
{
	return hash_funcs[g_type].value(buf);
}

