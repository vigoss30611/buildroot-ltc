#include <common.h>
#include <command.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <div64.h>
#include <linux/mtd/compat.h>
#include <bootlist.h>
#include <crypto.h>
#include <hash.h>
#include <tt.h>

#define tt_msg(x...) printf("ttcrypto: " x)
#define ENCRYPT_MAX	0x00800000
#define TROLLING_ERRCOUNT	12

static uint64_t alla, allh;
int tt_crypto_aes(void * mem) {
	int len, ret, es, ds, t;
	uint8_t *data, *cmpbuf = (uint8_t *)TT_CMP_BUFF;

	/* get random data */
	data = tt_rnd_get_data(&len, 0, ENCRYPT_MAX);
	if(!data) {
		tt_msg("get random data failed.\n");
		return -1;
	}

	len &= ~0xf;
	tt_msg("crypto: mem=0x%p(%p) len=0x%x ...", data, mem, len);

	printf("m");
	memcpy(cmpbuf, data, len);

	/* init aes engine */
	printf("i");
	ss_aes_init(ENCRYPTION, NULL, __irom_ver);

	/* encrypt data */
	printf("e");
	if(mem) {
		int _t, _w, _b = 0x8000, _l = len;
		t = 0;
		for(;_l;) {
			_w = (_l < _b) ? _l: _b;
			memcpy(mem, cmpbuf + (len - _l), _w);
			_t = get_timer(0);
			ss_aes_data(mem, mem, (_w > _b / 2) ? (_b / 2): _w);
			if(_w == _b)
			  ss_aes_data(mem + _b / 2, mem + _b / 2, (_w - _b / 2));
			_t = get_timer(_t);
			memcpy(cmpbuf + (len - _l), mem, _w);
			t += _t;
			_l -= _w;
		}
	} else {
		t = get_timer(0);
		ss_aes_data(cmpbuf, cmpbuf, len);
		t = get_timer(t);
	}

	es = (len >> 10) * 1000 / t;

	if(memcmp(cmpbuf, data, 16) == 0) {
		printf("\n");
		tt_msg("data it not encrypted\n");
		return -1;
	}

	/* init aes engine */
	ss_aes_init(DECRYPTION, NULL, __irom_ver);

	/* decrypt data */
	printf("d");
	if(mem) {
		int _t, _w, _b = 0x8000, _l = len;
		t = 0;
		for(;_l;) {
			_w = (_l < _b) ? _l: _b;
			_t = get_timer(0);
			memcpy(mem, cmpbuf + (len - _l), _w);
			ss_aes_data(mem, mem, (_w > _b / 2) ? (_b / 2): _w);
			if(_w == _b)
			  ss_aes_data(mem + _b / 2, mem + _b / 2, (_w - _b / 2));
			_t = get_timer(_t);
			memcpy(cmpbuf + (len - _l), mem, _w);
			t += _t;
			_l -= _w;
		}
	} else {
		t = get_timer(0);
		ss_aes_data(cmpbuf, cmpbuf, len);
		t = get_timer(t);
	}

	ds = (len >> 10) * 1000 / t;

	/* cmp data */
	printf("c\n");
	ret = tt_cmp_buffer(data, cmpbuf, len, TROLLING_ERRCOUNT);
	if(ret)
	  tt_msg("[aes]: FAILED: Ve=%d KB/s, Vd=%d KB/s, errbytes %d\n",
				  es, ds, ret);
	else
	  tt_msg("[aes]: PASSED: Ve=%d KB/s, Vd=%d KB/s\n", es, ds);

	alla += len;
	printf("\n");

	return 0;
}

int tt_crypto_hash(void * mem) {
	int cs, t;
	uint32_t *flags = (uint32_t *)TT_CMP_BUFF;
	uint8_t *data = (uint8_t *)&flags[12], value[32];
	char *names[] = { "crc32", "md5", "sha1", "sha256" };

	/* check memory flag */
	if(flags[0] != 0x99997777) {
		tt_msg("test data not found\n");
		udelay(5000000);
		return -1;
	}

	/* clear memory flag */
	flags[0] = 0;

	/* check type flag */
	if(flags[1] < IUW_HASH_CRC || flags[1] > IUW_HASH_SHA256) {
		tt_msg("unknow type: %d\n", flags[1]);
		return -1;
	}

	tt_msg("[%s]: size=0x%x\n", names[flags[1]], flags[2]);

	/* hash_len have valid value only after init */
	hash_init(flags[1], flags[2]);
	print_buffer((ulong)&flags[4], &flags[4], 1, hash_len(), 32);

	/* calc hash */
	if(mem) {
		int _t, _w, _b = 0x8000, _l = flags[2];
		t = 0;
		for(;_l;) {
			_w = (_l < _b) ? _l: _b;
			memcpy(mem, data + (flags[2] - _l), _w);
			_t = get_timer(0);
			hash_data(mem, (_w > _b / 2) ? (_b / 2): _w);
			if(_w == _b)
			  hash_data(mem + _b / 2, (_w - _b / 2));
			_t = get_timer(_t);
			t += _t;
			_l -= _w;
		}
	} else {
		t = get_timer(0);
		hash_data(data, flags[2]);
		t = get_timer(t);
	}

	hash_value(value);

	cs = (flags[2] >> 10) * 1000 / t;
	print_buffer((ulong)value, value, 1, hash_len(), 32);

	/* check value */
	tt_msg("[%s]: %s: Vc=%d KB/s\n", names[flags[1]],
		memcmp(&flags[4], value, hash_len()) ? "FAILED": "PASS", cs);

	allh += flags[2];
	return 0;
}

int do_tt_crypto(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	uint32_t mem = 0, r, f, i, t;

	if(argc > 1) {
		mem = simple_strtoul(argv[1], NULL, 16);
		tt_msg("focus memory set to 0x%08x\n", mem);
	}

	t = get_timer(0);
	for(i = 0, alla = 0, allh = 0;; i++) {
		r = tt_rnd_get_int(0, 6);
		f = tt_rnd_get_int(0, 2);

		printf("\n\nloop: %d (opt=%d, focus=%d)\n", i, r, f);
		if(r > 3)
		  tt_crypto_aes(f? (void *)mem: NULL);
		else
		  tt_crypto_hash(f? (void *)mem: NULL);

		if(tstc() && (getc() == 0x3)) break;
	}
	t = get_timer(t);

	printf("%d MB aes data, %d MB hash data, %d minutes.\n",
				(int)(alla >> 20), (int)(allh >> 20), t / 60000);

    return 0;
}

U_BOOT_CMD(ttcryloop, CONFIG_SYS_MAXARGS, 1, do_tt_crypto,
	"ttcryloop\n",
	"ttcryloop [focus]\n"
	);

static void __show_strbuf(char *s, uint8_t *buf, int len)
{
	int i;
	
	printf("%s:\n", s);
	for(i = 0; i < len; i++)  printf("%02x", buf[i]);
	printf("\n");
}

int do_crypto_quick(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	uint8_t iv[16] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	}, key[32] = {
		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
		0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
		0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
		0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
	}, plain[64] = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
		0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
		0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
		0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
		0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
		0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,
		0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10
	}, cipher[64] = {
		0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba,
		0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
		0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d,
		0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
		0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf,
		0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
		0xb2, 0xeb, 0x05, 0xe2, 0xc3, 0x9b, 0xe9, 0xfc,
		0xda, 0x6c, 0x19, 0x07, 0x8c, 0x6a, 0x9d, 0x1b,
	}, buf[64];

	__show_strbuf("iv", iv, 16);
	__show_strbuf("key", key, 32);
	__show_strbuf("plain text", plain, 64);
	__show_strbuf("chipher text", cipher, 64);
	
	/* test key enc */
	ss_aes_init(ENCRYPTION, key, iv);
	ss_aes_data(plain, buf, 64);
	__show_strbuf("encrypt result", buf, 64);

	/* test key dec */
	ss_aes_init(DECRYPTION, key, iv);
	ss_aes_data(cipher, buf, 64);
	__show_strbuf("decrypt result", buf, 64);

	/* test ISK enc */
	ss_aes_init(ENCRYPTION, NULL, __irom_ver);
	ss_aes_data(plain, buf, 64);
	__show_strbuf("ISK encrypt result", buf, 64);
	
	/* test ISK dec */
	ss_aes_init(DECRYPTION, NULL, __irom_ver);
	ss_aes_data(buf, buf, 64);
	__show_strbuf("ISK decrypt result", buf, 64);

	/* test md5 */
	hash_init(IUW_HASH_MD5, 64);
	hash_data(plain, 64);
	hash_value(buf);	
	__show_strbuf("md5", buf, hash_len());

	/* test sha1 */
	hash_init(IUW_HASH_SHA1, 64);
	hash_data(plain, 64);
	hash_value(buf);	
	__show_strbuf("sha1", buf, hash_len());

	/* test sha256 */
	hash_init(IUW_HASH_SHA256, 64);
	hash_data(plain, 64);
	hash_value(buf);	
	__show_strbuf("sha256", buf, hash_len());
	
	return 0;
}

U_BOOT_CMD(
	ttcryquick, 3, 1, do_crypto_quick,
	"ttcryquick",
	"test crypto modules quickly\n"
	);


