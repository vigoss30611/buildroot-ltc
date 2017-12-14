#include <common.h>
#include <command.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <div64.h>
#include <linux/mtd/compat.h>
#include <bootlist.h>
#include <tt.h>

#define tt_msg(x...) printf("ttff: " x)
#define TT_FF_STEP	0x00400000


int tt_ff_trolling(const char *dev, uint64_t size) {
	int len, id, i, ret;
	uint8_t *data, *cmpbuf = (uint8_t *)TT_CMP_BUFF;
	loff_t offset;

	ret = vs_assign_by_name(dev, 1);
	if(ret) {
		tt_msg("assign dev %s failed.\n", dev);
		return -1;
	}

	len = vs_align(DEV_CURRENT);

	data = tt_rnd_get_data(&len, len, len);
	if(!data) {
		tt_msg("get random data failed.\n");
		return -1;
	}

	/* rnd data */
	memcpy(cmpbuf, data, len);

	tt_msg("set mark every 0x%x ...", TT_FF_STEP);
	id = vs_device_id(dev);
	for(i = 0, offset = 0; offset < size;
			i++, offset += TT_FF_STEP) {
		if(vs_device_erasable(id)) {
			vs_erase(offset, TT_FF_STEP);
		}

		if(vs_is_device(id, "bnd") || vs_is_device(id, "fnd") ||
				vs_is_device(id, "nnd")) {
			if(nnd_vs_isbad(offset)) {
				tt_msg("skip bad block @ 0x%llx\n", offset);
				continue;
			}
		}

		*(uint32_t *)cmpbuf = i;
		ret = vs_write(cmpbuf, offset, len, 0);
		if(ret < 0) {
			tt_msg("FAILED at 0x%llx\n", offset);
			break;
		}

		if(!(i & 0x1f)) printf(":%d M:", i * (TT_FF_STEP >> 20));
	}
	if(offset >= size) printf("DONE.\n");



	tt_msg("check mark every 0x%x ...", TT_FF_STEP);
	for(i = 0, offset = 0; offset < size;
			i++, offset += TT_FF_STEP) {

		if(vs_is_device(id, "bnd") || vs_is_device(id, "fnd") ||
				vs_is_device(id, "nnd")) {
			if(nnd_vs_isbad(offset)) {
				tt_msg("skip bad block @ 0x%llx\n", offset);
				continue;
			}
		}

		vs_read(cmpbuf, offset, len, 0);
		if(!(i & 0x1f)) printf(":%d M:", i * (TT_FF_STEP >> 20));
		if(*(uint32_t *)cmpbuf != i) {
			printf("FAIL at 0x%llx\n", offset);
			break;
		}

		if(tt_cmp_buffer(data + 4, cmpbuf + 4,
					len - 4, 10))
		{
			printf("FAIL at 0x%llx\n", offset);
			break;
		}
	}

	if(offset >= size) {
		printf("DONE.\n");
		tt_msg("ff check passed.\n");
	}

	return 0;
}

int do_tt_ff(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	uint32_t size = 0;

	if(argc < 2) {
		tt_msg("please provide the device name you want to test:\n"
					"ttblk devname\n");
		return -1;
	}

	if(argc > 2) {
		size = simple_strtoul(argv[2], NULL, 10);
		printf("device size is set to %dMB\n", size);
	}

	tt_ff_trolling(argv[1], (uint64_t)size << 20);

    return 0;
}

U_BOOT_CMD(ttff, CONFIG_SYS_MAXARGS, 1, do_tt_ff,
	"ttff\n",
	"info - show available command\n"
	);

