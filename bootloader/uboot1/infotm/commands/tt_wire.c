#include <common.h>
#include <command.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <div64.h>
#include <linux/mtd/compat.h>
#include <bootlist.h>
#include <tt.h>
#include <net.h>
#include <udc.h>
#include <iuw.h>
#include <hash.h>

#define tt_msg(x...) printf("ttwire: " x)
#define TT_WIRE_MAX	0x8000
#define TT_WIRE_MIN	0x00000020

#define TT_WIRE_S2C		0
#define TT_WIRE_C2S		1
#define TT_WIRE_CON		2
#define TT_WIRE_END		3
#define TT_MAGIC		0x7766dd80

static uint64_t allw = 0, allr = 0;
const static struct trans_dev tdev[] = {
	{
//		.connect = udc_connect,
//		.disconnect = udc_disconnect,
//		.read    = udc_read,
//		.write   = udc_write,
//		.name    = "usb",
	},
#if defined(CONFIG_ENABLE_ETH)
	{
		.connect = eth_connect,
		.read    = eth_read,
		.write   = eth_write,
		.name    = "ethernet",
	}
#endif
}, *ptdev;

int tt_wire_check_valid(uint8_t * buf, int len) {

	uint32_t tmp,
			 mag = *(uint32_t *)(buf + 0),
			 crc = *(uint32_t *)(buf + 4);

	if(mag != TT_MAGIC) 
	{
		tt_msg("mag do not match. 0x%x <=> 0x%x\n", mag, TT_MAGIC);
		return 0;
	}

	*(uint32_t *)(buf + 4) = 0;

	hash_init(IUW_HASH_CRC, len);
	hash_data(buf, len);
	hash_value(&tmp);
	if(tmp != crc)
	{ 
		tt_msg("crc do not match. 0x%x <=> 0x%x\n", tmp, crc);
		return 0;
	}

	return 1;
}

int tt_wire_get_opt(uint8_t * buf) {
	return *(uint32_t *)(buf + 0x8);
}

int tt_wire_get_len(uint8_t * buf) {
	return *(uint32_t *)(buf + 0xc);
}

int tt_wire_make_data(uint8_t * buf, uint8_t * data, int len, int opt, int nxlen) {

	memcpy(buf, data, len);
	*(uint32_t *)(buf + 0x0) = TT_MAGIC;
	*(uint32_t *)(buf + 0x4) = 0;
	*(uint32_t *)(buf + 0x8) = opt;
	*(uint32_t *)(buf + 0xc) = nxlen;

	hash_init(IUW_HASH_CRC, len);
	hash_data(buf, len);
	hash_value(buf + 0x4);
	return 0;
}

#if 0
int crc_test(void)
{
	uint8_t buf[32] = {
		0x80, 0xdd, 0x66, 0x77, 0x65, 0x2d, 0xaa, 0xfb,
		0x01, 0x00, 0x00, 0x00, 0x43, 0x7b, 0x32, 0x00,
		0x85, 0x40, 0xfe, 0xd7, 0x1d, 0x3a, 0x81, 0x7d,
		0x77, 0xd2, 0xb3, 0xbb, 0x82, 0xa2, 0x2d, 0x41};
	uint32_t tmp;

	hash_init(IUW_HASH_CRC, 32);
	hash_data(buf, 32);
	hash_value((uint8_t *)&tmp);

	printf("PC crc: 0x%x\n", tmp);
	while(1);
	return 0;
}
#endif

int do_tt_wire(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int ret, opt, len, ct, t;
	uint8_t *data, *cmpbuf = (uint8_t *)TT_CMP_BUFF;

//	crc_test();
	/* argv[1] = eth or udc */
	if(argc < 2) {
		tt_msg("please provide the device name you want to test:\n"
					"ttwire devname\n");
		return -1;
	}

	if(strncmp(argv[1], "udc", 3) == 0)
	  ptdev = &tdev[0];
	else if(strncmp(argv[1], "eth", 3) == 0)
	  ptdev = &tdev[1];
	else {
		tt_msg("device unavailible: %s\n", argv[1]);
		return -1;
	}

	t = get_timer(0);

__restart:
	udelay(2000000);
	tt_msg("connecting to server ...\n");
//	ret = udc_connect();
	ret = ptdev->connect();
	if(ret < 0) {
		tt_msg("connect to server failed.\n");
		return -1;
	}

	opt = TT_WIRE_S2C;
	len = TT_WIRE_MIN;

__start:
	if(opt == TT_WIRE_S2C) {

		tt_msg("opt: StoC, 0x%x :: ", len);
		ct = get_timer(0);
		ret = ptdev->read(cmpbuf, len);
//		print_buffer(cmpbuf, cmpbuf, 1, len, 16);
		ct = get_timer(ct);
		if(ret < 0) {
			printf("Fail transport\n");
			goto __restart;
		}

		if(!tt_wire_check_valid(cmpbuf, len)) {
			printf("FAIL: %d KB/s\n", (len >> 10) * 1000 / ct);
			goto __restart;
		}

		allr += len;
		printf("PASS: %d KB/s\n", (len >> 10) * 1000 / ct);
		len = tt_wire_get_len(cmpbuf);
		opt = tt_wire_get_opt(cmpbuf);
	} else if(opt == TT_WIRE_C2S) {
		int nextlen;

		tt_msg("opt: CtoS, 0x%x :: ", len);
		opt = 0;
		tt_rnd_get_int(0, 2);
		data = tt_rnd_get_data(&nextlen, TT_WIRE_MIN, TT_WIRE_MAX);

		tt_wire_make_data(cmpbuf, data, len, opt, nextlen);
		ct = get_timer(0);
		ret = ptdev->write(cmpbuf, len);
		ct = get_timer(ct);
		if(ret < 0) {
			printf("Fail transport\n");
			udelay(0x200);
			goto __restart;
		}

		allw += len;
		len = nextlen;
		printf("DONE: %d KB/s\n", (len >> 10) * 1000 / ct);
	} else if(opt == TT_WIRE_END) {

		tt_msg("opt: end\n");
		goto __end;

	} else if(opt == TT_WIRE_CON) {

		tt_msg("opt: reconnect\n\n");
		goto __restart;
	} else {
		tt_msg("unknown opt, should not reach here.\n");
		for(;;);
	}

	/* check breaks */
	if(tstc() && (getc() == 0x3)) goto __end;
	goto __start;

__end:
	t = get_timer(t);
	tt_msg("%d MB data sent.\n", (int)(allw >> 20));
	tt_msg("%d MB data received.\n", (int)(allr >> 20));
	tt_msg("%d minutes used.\n", t / 60000);

    return 0;
}

U_BOOT_CMD(ttwire, CONFIG_SYS_MAXARGS, 1, do_tt_wire,
	"ttwire\n",
	"info - show available command\n"
	);

