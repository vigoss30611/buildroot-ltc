#include <common.h>
#include <crc32.h>
#include <md5.h>
/* the infotm sha1&256 conflics with the u-boot original ones */
#include <infotm/sha1.h>
#include <infotm/sha256.h>
#include <hash.h>
#include <efuse.h>

static enum hash_type g_type;

static struct hash_func
hash_funcs[] = {
	{
		.len   = 4,
		.init  = crc32_init,
		.data  = crc32_data,
		.value = crc32_value,
	}, {
		.len   = 16,
		.init  = md5_init,
		.data  = md5_data,
		.value = md5_value,
	}, {
		.len   = 20,
		.init  = sha1_init,
		.data  = sha1_data,
		.value = sha1_value,
	}, {
		.len   = 32,
		.init  = sha256_init,
		.data  = sha256_data,
		.value = sha256_value,
	}
};

int hash_init(enum hash_type type, uint32_t len)
{
	if(type > IUW_HASH_SHA256) {
		printf("Unknown hash type: only CRC, MD5, SHA1 and"
					" SHA256 is supported.\n");
		return -1;
	}

	g_type = type;

	if(g_type && !ecfg_check_flag(ECFG_ENABLE_CRYPTO)) {
		printf("noss\n");
		return 0;
	}

	return hash_funcs[type].init(len);
}

int hash_data(void *buf, uint32_t len)
{
	if(g_type && !ecfg_check_flag(ECFG_ENABLE_CRYPTO)) {
		printf("noss\n");
		return 0;
	}

	return hash_funcs[g_type].data(buf, len);
}

int hash_value(void *buf)
{
	if(g_type && !ecfg_check_flag(ECFG_ENABLE_CRYPTO)) {
		printf("noss\n");
		return 0;
	}

	return hash_funcs[g_type].value(buf);
}

int inline hash_len(void)
{
	return hash_funcs[g_type].len;
}

