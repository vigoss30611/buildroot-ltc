
#include <common.h>
#include <cdbg.h>
#include <isi.h>
#include <ius.h>
#include <hash.h>
#include <crypto.h>
#include <ius.h>
#include <bootlist.h>
#include <vstorage.h>
#include <malloc.h>
#include <nand.h>
#include <led.h>
#include <irerrno.h>
#include <items.h>
#include <sparse.h>
#include <storage.h>
#include <burn.h>
#include <bootl.h>
#include <yaffs.h>

struct part_ius {
	char name[16];
	int  iustype;
};

struct part_ius p_ius[] = {
	{ "misc", IUS_DESC_MISC },
	{ "system", IUS_DESC_SYSM },
	{ "cache", IUS_DESC_CACH },
	{ "user", IUS_DESC_USER },
};

int ius_id = -1;
int ius_remain = 0;
loff_t ius_offs = 0;

const char *ius2part(int d) {
	int i;

	for (i = 0; i < ARRAY_SIZE(p_ius); i++)
	  if(p_ius[i].iustype == d)
		return p_ius[i].name;

	return NULL;
}

#define EXTRA_BUFFER_SIZE 0x40000
#if 0
static int ius_sparse_data(void *buf)
{
	int step = EXTRA_BUFFER_SIZE, ret;

	if(!ius_remain) {
		printf("no data for sparse.\n");
		return 0;
	}

	vs_assign_by_id(ius_id, 0);
	if(step>ius_remain){
		step=ius_remain;
		if(6<=ius_id&&ius_id<=7){
			step=(step&(~(vs_align(ius_id)-1)))+vs_align(ius_id);
		}
	}
	ret = vs_read(buf, ius_offs, step, 0);
//		ret = vs_read(buf, ius_offs, step, 0);
//	printf("read sparse data by vs_read 0x%x bytes,from  device:%d, wanted 0x%x data[0]:0x%8x\n", ret ,ius_id,min(step, ius_remain),*((int*)buf));
	ius_remain -= ret;
	ius_offs   += ret;
//	printf("----remain:0x%8x\n",ius_remain);
	return ret;
}
#endif
int ius_get_page(uint8_t *buf)
{
	if(!ius_remain) {
		printf("page data end.\n");
		return -1;
	}

	vs_assign_by_id(ius_id, 0);
	vs_read(buf, ius_offs, 2064, 0);

	ius_remain -= 2064;
	ius_offs   += 2064;
	return 0;
}

int burn_extra(struct ius_desc *desc, int id_i)
{
	uint8_t *buf;
	int id_o, size;
	int ret = 0, type = ius_desc_type(desc);
	loff_t offs;

	switch(type)
	{
		case IUS_DESC_BOOT:
			printf("IUS boot normal.\n");
			/* never reached */
			return 0;
		case IUS_DESC_BOOTR:
		case IUS_DESC_REBOOT:
			printf("IUS boot recovery.\n");

			/* update config */
			init_config();

			printf("part---\n");
			/* update partition */

			printf("bootre---\n");
			/* TODO: boot the recovery*/
			set_boot_type(BOOT_FACTORY_INIT);
			printf("bootre1---\n");
			if (type == IUS_DESC_REBOOT) {
                            printf("boot to recovery directly.\n");
                            do_boot((void *)CONFIG_SYS_PHY_LK_BASE,
                                    (void *)CONFIG_SYS_PHY_RD_BASE);
                        } else
                            bootl();

			/* never reached */
			return 0;
		default:
			printf("extra burns.\n");
	}

	buf = malloc(EXTRA_BUFFER_SIZE);
	if(!buf) {
		printf("burn extra alloc buffer failed.\n");
		return -1;
	}

	id_o = storage_device();
	offs = storage_offset(ius2part(type));
	size = storage_size(ius2part(type));

	if(!offs) {
		printf("%s: get offs.\n",
					__func__);
		ret = -2;
		goto __out;
	}
	if(!size){
		 printf("%s: get size.\n",__func__);
		 ret = -2;
		 goto __out;
	}

	ius_offs = ius_desc_offi(desc);
	ius_remain = ius_desc_length(desc);
	ius_id = id_i;

__out:
	free(buf);
	return ret;
}

