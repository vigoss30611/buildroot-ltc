
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <version.h>
#include <asm/io.h>
#include <vstorage.h>
#include <bootlist.h>
#include <isi.h>
#include <hash.h>
#include <cdbg.h>
#include <efuse.h>
#include <lowlevel_api.h>

struct efuse_paras efuse_para_table;
static uint64_t efuse_mac_address = 0;

void init_efuse(void)
{
	int i, a = ECFG_PARA_OFFSET;
	uint8_t *d = (uint8_t *)&efuse_para_table;

	/* make sure e-fuse info update is ready
	 * the start read command is send by sysmgr before
	 * iROM was running
	 */
	while(readl(EFUSE_SYSM_ADDR) & 1);

	/* read e-fuse data into table */
	for(i = 0; i < sizeof(struct efuse_paras); i++, a += 4)
	  d[i] = readl(EFUSE_DATA_ADDR + a);

	/* read e-fuse data into mac */
	a = ECFG_MAC_OFFSET;
	d = (uint8_t *)&efuse_mac_address;
	for(i = 0; i < 6; i++, a += 4)
	  d[i] = readl(EFUSE_DATA_ADDR + a);
}

int inline ecfg_check_flag(uint32_t flag) {
	if(efuse_para_table.trust == ECFG_TRUST_NUM)
	  return !(efuse_para_table.e_config & flag);
	else
	  return 1;
}

struct efuse_paras * ecfg_paras(void) {
	return &efuse_para_table;
}

uint64_t efuse_mac(void) {
	return efuse_mac_address;
}

char __emac_serial[16];
char * efuse_mac2serial(void) {
	uint32_t crc;

	hash_init(IUW_HASH_CRC, 6);
	hash_data(&efuse_mac_address, 6);
	hash_value(&crc);

	sprintf(__emac_serial, "iMAP%08x", crc);
	return __emac_serial;
}

void efuse_info(int human) {
	int i, a = ECFG_PARA_OFFSET;

	printf("e-fuse information:\n");
	printf("---------------------\n");
	if(human) {
		printf("config:  0x%x\n", efuse_para_table.e_config);
		printf("mmc mux: 0x%x\n", efuse_para_table.mux_mmc);
		printf("default mux: 0x%x\n", efuse_para_table.mux_default);
		printf("crystal: 0x%x\n", efuse_para_table.crystal);
		printf("toggle timing: 0x%x\n", efuse_para_table.tg_timing);
		printf("trust: 0x%x\n", efuse_para_table.trust);
		printf("phy read: 0x%x\n", efuse_para_table.phy_read);
		printf("phy delay: 0x%x\n", efuse_para_table.phy_delay);
		printf("polynomial: 0x%x\n", efuse_para_table.polynomial);
		for(i = 0; i < 4; i++)
		  printf("seed(%d): 0x%x\n", i, efuse_para_table.seed[i]);
		for(i = 0; i < 12; i++)
		  printf("binfo(%d): 0x%x\n", i + 16, efuse_para_table.binfo[i]);
	} else {
		/* read e-fuse data into table */
		for(i = 0; i < sizeof(struct efuse_paras); i++, a += 4)
		  printf("0x%08x: 0x%02x\n", EFUSE_DATA_ADDR + a,
					  readl(EFUSE_DATA_ADDR + a));
	}
}

