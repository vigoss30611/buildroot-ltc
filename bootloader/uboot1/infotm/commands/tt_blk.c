#include <common.h>
#include <command.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <div64.h>
#include <linux/mtd/compat.h>
#include <bootlist.h>
#include <tt.h>

#define tt_msg(x...) printf("ttblk: " x)
#define REDUNDANT_TEST
#define TROLLING_MAX	0x00800000
#define TROLLING_MIN	0x00100000
#define TROLLING_SIZE	0x00800000
#define TROLLING_ERRCOUNT	12



static uint64_t all;
int tt_blk_trolling(const char *dev, uint64_t size, void *mem) {
	int len, ret, ws, rs = 0, cnt, id;
	uint32_t tr_max = TROLLING_MAX, tr_min = TROLLING_MIN, t;
	uint64_t tr_size = TROLLING_SIZE;
	uint8_t *data, *cmpbuf = (uint8_t *)TT_CMP_BUFF;
	loff_t offset;

	ret = vs_assign_by_name(dev, 1);
	if(ret) {
		tt_msg("assign dev %s failed.\n", dev);
		return -1;
	}

	if(size) {
		tr_size = size;
		if(size < tr_max) {
			tr_max = size;
			tr_min = vs_align(DEV_CURRENT);
		}
	}

	/* get random data */
	cnt  = tt_rnd_get_int(0, tr_max / vs_align(DEV_CURRENT));
	ret = cnt * vs_align(DEV_CURRENT);

	data = tt_rnd_get_data(&len, ret, ret);
	if(!data) {
		tt_msg("get random data failed.\n");
		return -1;
	}

	/* get a random page */
	ret = tt_rnd_get_int(0,
		lldiv((tr_size - len), vs_align(DEV_CURRENT)));
	offset = (uint64_t)ret * (uint64_t)vs_align(DEV_CURRENT);


	tt_msg("trolling on %s: mem=0x%p (0x%x @ 0x%llx) ...",
			dev, data, len, offset);

	/* erase nand device */
	id = vs_device_id(dev);
	if(vs_device_erasable(id)) {
		printf("e");
		vs_erase(offset, len);
	}

	/* write data */
	printf("w");
	if(mem) {
		int _t, _l = len, _w, _b = 0x8000;
		t = 0;
		for(;_l;) {
			_w =  (_l < _b) ? _l: _b;
			memcpy(mem, data + (len - _l), _w);
			_t = get_timer(0);
			vs_write(mem, offset + (len - _l), _w, 0);
			_t = get_timer(_t);
			t += _t;
			_l -= _w;
		}
	} else {
		t = get_timer(0);
		vs_write(data, offset, len, 0);
		t = get_timer(t);
	}
	ws = (len >> 10) * 1000 / t;

	/* read data:
	 * if test case is TROLL_NND or TROLL_FND, we
	 * should read the test area with both "nnd" and "fnd"
	 */
	printf("r");
	if(mem) {
		int _t, _l = len, _w, _b = 0x8000;
		t = 0;
		for(;_l;) {
			_w =  (_l < _b) ? _l: _b;
			_t = get_timer(0);
			vs_read(mem, offset + (len - _l), _w, 0);
			_t = get_timer(_t);
			memcpy(cmpbuf + (len - _l), mem, _w);
			t += _t;
			_l -= _w;
		}
	} else {
		t = get_timer(0);
		vs_read(cmpbuf, offset, len, 0);
		t = get_timer(t);
	}
	rs = (len >> 10) * 1000 / t;

	/* cmp data */
	printf("c\n");
	ret = tt_cmp_buffer(data, cmpbuf, len, TROLLING_ERRCOUNT);
	if(ret)
	  tt_msg("[%s]: FAILED, Vw=%d KB/s, Vr=%d KB/s, errbytes %d\n",
				  dev, ws, rs, ret);
	else
	  tt_msg("[%s]: PASSED: Vw=%d KB/s, Vr=%d KB/s\n",
				  dev, ws, rs);

	all += len;
	printf("\n");

	return 0;
}

int do_tt_blk(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	uint32_t i, size = 0, t, mem = 0;

	if(argc < 2) {
		tt_msg("please provide the device name you want to test:\n"
					"ttblk devname\n");
		return -1;
	}

	if(argc > 2) {
		size = simple_strtoul(argv[2], NULL, 10);
		if(strncmp(argv[1], "flash", 5) == 0)
		    tt_msg("device size is set to %d KB\n", size);
		else if(strncmp(argv[1], "eep", 3) == 0)
		    tt_msg("device size is set to %d KB\n", size);
		else
		    tt_msg("device size is set to %d MB\n", size);

		if(argc > 3) {
			mem = simple_strtoul(argv[3], NULL, 16);
			tt_msg("focus memory set to 0x%08x\n", mem);
		}
	}

	t = get_timer(0);
	for(i = 0, all = 0;; i++) {
		printf("loop: %d\n", i);
		if(strncmp(argv[1], "flash", 5) == 0)
		    tt_blk_trolling(argv[1], (uint64_t)size << 10, (void *)mem);
		else if(strncmp(argv[1], "eep", 3) == 0)
		    tt_blk_trolling(argv[1], (uint64_t)size << 10, (void *)mem);
		else
		    tt_blk_trolling(argv[1], (uint64_t)size << 20, (void *)mem);
		if(tstc() && (getc() == 0x3)) break;
	}
	t = get_timer(t);

	printf("%d MB data tested in %d minutes.\n", (int)(all >> 20), t / 60000);

    return 0;
}

U_BOOT_CMD(ttblk, CONFIG_SYS_MAXARGS, 1, do_tt_blk,
	"ttblk\n",
	"info - show available command\n"
	);

