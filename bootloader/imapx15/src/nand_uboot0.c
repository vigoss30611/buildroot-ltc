#include <nand.h>
#include <asm-generic/errno.h>
#include <asm-arm/io.h>
#include <lowlevel_api.h>
#include <preloader.h>
#include <rballoc.h>
#include <hash.h>
#include <bootlist.h>
#include <rtcbits.h>
#include <items.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <asm-arm/arch-apollo3/nand.h>
#include <asm-arm/arch-apollo3/imapx_nand.h>

extern struct irom_export *irf;
extern int boot_dev_id;
static uint8_t *rrtb;
static uint8_t nand_param[IMAPX800_PARA_SAVE_SIZE];
static int uboot0_unit, vitual_fifo_offset, vitual_fifo_start, fpga_test = 0;

#if 1
unsigned uboot0_data_seed_1k[][8] = {
	{0x0be3, 0x2c65, 0x3fa6, 0x1fd3, 0x267d, 0x3aaa, 0x1d55, 0x273e},
	{0x3fff, 0x366b, 0x32a1, 0x30c4, 0x1862, 0x0c31, 0x2f8c, 0x17c6},
	{0x139f, 0x205b, 0x39b9, 0x3548, 0x1aa4, 0x0d52, 0x06a9, 0x2ac0},
	{0x1560, 0x0ab0, 0x0558, 0x02ac, 0x0156, 0x00ab, 0x29c1, 0x3d74},
	{0x1eba, 0x0f5d, 0x2e3a, 0x171d, 0x221a, 0x110d, 0x2112, 0x1089},
	{0x21d0, 0x10e8, 0x0874, 0x043a, 0x021d, 0x289a, 0x144d, 0x23b2},
	{0x11d9, 0x2178, 0x10bc, 0x085e, 0x042f, 0x2b83, 0x3c55, 0x37be},
	{0x1bdf, 0x247b, 0x3ba9, 0x3440, 0x1a20, 0x0d10, 0x0688, 0x0344},
	{0x01a2, 0x00d1, 0x29fc, 0x14fe, 0x0a7f, 0x2cab, 0x3fc1, 0x3674},
	{0x1b3a, 0x0d9d, 0x2f5a, 0x17ad, 0x2242, 0x1121, 0x2104, 0x1082},
	{0x0841, 0x2db4, 0x16da, 0x0b6d, 0x2c22, 0x1611, 0x229c, 0x114e},
	{0x08a7, 0x2dc7, 0x3f77, 0x362f, 0x3283, 0x30d5, 0x31fe, 0x18ff},
	{0x25eb, 0x3b61, 0x3424, 0x1a12, 0x0d09, 0x2f10, 0x1788, 0x0bc4},
	{0x05e2, 0x02f1, 0x28ec, 0x1476, 0x0a3b, 0x2c89, 0x3fd0, 0x1fe8},
	{0x0ff4, 0x07fa, 0x03fd, 0x286a, 0x1435, 0x238e, 0x11c7, 0x2177},
	{0x392f, 0x3503, 0x3315, 0x301e, 0x180f, 0x2593, 0x3b5d, 0x343a},
	{0x1a1d, 0x249a, 0x124d, 0x20b2, 0x1059, 0x21b8, 0x10dc, 0x086e},
	{0x0437, 0x2b8f, 0x3c53, 0x37bd, 0x324a, 0x1925, 0x2506, 0x1283},
	{0x20d5, 0x39fe, 0x1cff, 0x27eb, 0x3a61, 0x34a4, 0x1a52, 0x0d29},
	{0x2f00, 0x1780, 0x0bc0, 0x05e0, 0x02f0, 0x0178, 0x00bc, 0x005e},
	{0x002f, 0x2983, 0x3d55, 0x373e, 0x1b9f, 0x245b, 0x3bb9, 0x3448},
	{0x1a24, 0x0d12, 0x0689, 0x2ad0, 0x1568, 0x0ab4, 0x055a, 0x02ad},
	{0x28c2, 0x1461, 0x23a4, 0x11d2, 0x08e9, 0x2de0, 0x16f0, 0x0b78},
	{0x05bc, 0x02de, 0x016f, 0x2923, 0x3d05, 0x3716, 0x1b8b, 0x2451},
	{0x3bbc, 0x1dde, 0x0eef, 0x2ee3, 0x3ee5, 0x36e6, 0x1b73, 0x242d},
	{0x3b82, 0x1dc1, 0x2774, 0x13ba, 0x09dd, 0x2d7a, 0x16bd, 0x22ca},
	{0x1165, 0x2126, 0x1093, 0x21dd, 0x397a, 0x1cbd, 0x27ca, 0x13e5},
	{0x2066, 0x1033, 0x218d, 0x3952, 0x1ca9, 0x27c0, 0x13e0, 0x09f0},
	{0x04f8, 0x027c, 0x013e, 0x009f, 0x29db, 0x3d79, 0x3728, 0x1b94},
	{0x0dca, 0x06e5, 0x2ae6, 0x1573, 0x232d, 0x3802, 0x1c01, 0x2794},
	{0x13ca, 0x09e5, 0x2d66, 0x16b3, 0x22cd, 0x38f2, 0x1c79, 0x27a8},
	{0x13d4, 0x09ea, 0x04f5, 0x2bee, 0x15f7, 0x236f, 0x3823, 0x3585},
	{0x3356, 0x19ab, 0x2541, 0x3b34, 0x1d9a, 0x0ecd, 0x2ef2, 0x1779},
	{0x2228, 0x1114, 0x088a, 0x0445, 0x2bb6, 0x15db, 0x2379, 0x3828},
	{0x1c14, 0x0e0a, 0x0705, 0x2a16, 0x150b, 0x2311, 0x381c, 0x1c0e},
	{0x0e07, 0x2e97, 0x3edf, 0x36fb, 0x32e9, 0x30e0, 0x1870, 0x0c38},
	{0x061c, 0x030e, 0x0187, 0x2957, 0x3d3f, 0x370b, 0x3211, 0x309c},
	{0x184e, 0x0c27, 0x2f87, 0x3e57, 0x36bf, 0x32cb, 0x30f1, 0x31ec},
	{0x18f6, 0x0c7b, 0x2fa9, 0x3e40, 0x1f20, 0x0f90, 0x07c8, 0x03e4},
	{0x01f2, 0x00f9, 0x29e8, 0x14f4, 0x0a7a, 0x053d, 0x2b0a, 0x1585},
	{0x2356, 0x11ab, 0x2141, 0x3934, 0x1c9a, 0x0e4d, 0x2eb2, 0x1759}
};

unsigned uboot0_ecc_seed_1k[][8] = {
	{0x0a72, 0x0539, 0x6b08, 0x1584, 0x0ac2, 0x0561, 0x6b24, 0x1592},
	{0x1720, 0x0b90, 0x05c8, 0x02e4, 0x0172, 0x00b9, 0x69c8, 0x14e4},
	{0x0ac9, 0x6cf0, 0x1678, 0x0b3c, 0x059e, 0x02cf, 0x68f3, 0x7ded},
	{0x7762, 0x1bb1, 0x644c, 0x1226, 0x0913, 0x6d1d, 0x7f1a, 0x1f8d},
	{0x6652, 0x1329, 0x6000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100},
	{0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001},
	{0x6994, 0x14ca, 0x0a65, 0x6ca6, 0x1653, 0x62bd, 0x78ca, 0x1c65},
	{0x67a6, 0x13d3, 0x607d, 0x79aa, 0x1cd5, 0x67fe, 0x13ff, 0x606b},
	{0x79a1, 0x7544, 0x1aa2, 0x0d51, 0x6f3c, 0x179e, 0x0bcf, 0x6c73},
	{0x7fad, 0x7642, 0x1b21, 0x6404, 0x1202, 0x0901, 0x6d14, 0x168a},
	{0x0b45, 0x6c36, 0x161b, 0x6299, 0x78d8, 0x1c6c, 0x0e36, 0x071b},
	{0x6a19, 0x7c98, 0x1e4c, 0x0f26, 0x0793, 0x6a5d, 0x7cba, 0x1e5d},
	{0x66ba, 0x135d, 0x603a, 0x101d, 0x619a, 0x10cd, 0x61f2, 0x10f9},
	{0x61e8, 0x10f4, 0x087a, 0x043d, 0x6b8a, 0x15c5, 0x6376, 0x11bb},
	{0x6149, 0x7930, 0x1c98, 0x0e4c, 0x0726, 0x0393, 0x685d, 0x7dba},
	{0x1edd, 0x66fa, 0x137d, 0x602a, 0x1015, 0x619e, 0x10cf, 0x61f3},
	{0x796d, 0x7522, 0x1a91, 0x64dc, 0x126e, 0x0937, 0x6d0f, 0x7f13},
	{0x761d, 0x729a, 0x194d, 0x6532, 0x1299, 0x60d8, 0x106c, 0x0836},
	{0x041b, 0x6b99, 0x7c58, 0x1e2c, 0x0f16, 0x078b, 0x6a51, 0x7cbc},
	{0x1e5e, 0x0f2f, 0x6e03, 0x7e95, 0x76de, 0x1b6f, 0x6423, 0x7b85},
	{0x7456, 0x1a2b, 0x6481, 0x7bd4, 0x1dea, 0x0ef5, 0x6eee, 0x1777},
	{0x622f, 0x7883, 0x75d5, 0x737e, 0x19bf, 0x654b, 0x7b31, 0x740c},
	{0x1a06, 0x0d03, 0x6f15, 0x7e1e, 0x1f0f, 0x6613, 0x7a9d, 0x74da},
	{0x1a6d, 0x64a2, 0x1251, 0x60bc, 0x105e, 0x082f, 0x6d83, 0x7f55},
	{0x763e, 0x1b1f, 0x641b, 0x7b99, 0x7458, 0x1a2c, 0x0d16, 0x068b},
	{0x6ad1, 0x7cfc, 0x1e7e, 0x0f3f, 0x6e0b, 0x7e91, 0x76dc, 0x1b6e},
	{0x0db7, 0x6f4f, 0x7e33, 0x768d, 0x72d2, 0x1969, 0x6520, 0x1290},
	{0x0948, 0x04a4, 0x0252, 0x0129, 0x6900, 0x1480, 0x0a40, 0x0520},
	{0x0290, 0x0148, 0x00a4, 0x0052, 0x0029, 0x6980, 0x14c0, 0x0a60},
	{0x0530, 0x0298, 0x014c, 0x00a6, 0x0053, 0x69bd, 0x7d4a, 0x1ea5},
	{0x66c6, 0x1363, 0x6025, 0x7986, 0x1cc3, 0x67f5, 0x7a6e, 0x1d37},
	{0x670f, 0x7a13, 0x749d, 0x73da, 0x19ed, 0x6562, 0x12b1, 0x60cc},
	{0x1066, 0x0833, 0x6d8d, 0x7f52, 0x1fa9, 0x6640, 0x1320, 0x0990},
	{0x04c8, 0x0264, 0x0132, 0x0099, 0x69d8, 0x14ec, 0x0a76, 0x053b},
	{0x6b09, 0x7c10, 0x1e08, 0x0f04, 0x0782, 0x03c1, 0x6874, 0x143a},
	{0x0a1d, 0x6c9a, 0x164d, 0x62b2, 0x1159, 0x6138, 0x109c, 0x084e},
	{0x0427, 0x6b87, 0x7c57, 0x77bf, 0x724b, 0x70b1, 0x71cc, 0x18e6},
	{0x0c73, 0x6fad, 0x7e42, 0x1f21, 0x6604, 0x1302, 0x0981, 0x6d54}
};
unsigned uboot0_ecc_seed_1k_16bit[][8] = {
	{0x11bd, 0x614a, 0x10a5, 0x61c6, 0x10e3, 0x61e5, 0x7966, 0x1cb3},
	{0x16f9, 0x62e8, 0x1174, 0x08ba, 0x045d, 0x6bba, 0x15dd, 0x637a},
	{0x67cd, 0x7a72, 0x1d39, 0x6708, 0x1384, 0x09c2, 0x04e1, 0x6be4},
	{0x15f2, 0x0af9, 0x6ce8, 0x1674, 0x0b3a, 0x059d, 0x6b5a, 0x15ad},
	{0x6342, 0x11a1, 0x6144, 0x10a2, 0x0851, 0x6dbc, 0x16de, 0x0b6f},
	{0x6c23, 0x7f85, 0x7656, 0x1b2b, 0x6401, 0x7b94, 0x1dca, 0x0ee5},
	{0x6ee6, 0x1773, 0x622d, 0x7882, 0x1c41, 0x67b4, 0x13da, 0x09ed},
	{0x6d62, 0x16b1, 0x62cc, 0x1166, 0x08b3, 0x6dcd, 0x7f72, 0x1fb9},
	{0x6648, 0x1324, 0x0992, 0x04c9, 0x6bf0, 0x15f8, 0x0afc, 0x057e},
	{0x02bf, 0x68cb, 0x7df1, 0x776c, 0x1bb6, 0x0ddb, 0x6f79, 0x7e28},
	{0x1f14, 0x0f8a, 0x07c5, 0x6a76, 0x153b, 0x6309, 0x7810, 0x1c08},
	{0x0e04, 0x0702, 0x0381, 0x6854, 0x142a, 0x0a15, 0x6c9e, 0x164f},
	{0x62b3, 0x78cd, 0x75f2, 0x1af9, 0x64e8, 0x1274, 0x093a, 0x049d},
	{0x6bda, 0x15ed, 0x6362, 0x11b1, 0x614c, 0x10a6, 0x0853, 0x6dbd},
	{0x7f4a, 0x1fa5, 0x6646, 0x1323, 0x6005, 0x7996, 0x1ccb, 0x67f1},
	{0x7a6c, 0x1d36, 0x0e9b, 0x6ed9, 0x7ef8, 0x1f7c, 0x0fbe, 0x07df},
	{0x6a7b, 0x7ca9, 0x77c0, 0x1be0, 0x0df0, 0x06f8, 0x037c, 0x01be},
	{0x00df, 0x69fb, 0x7d69, 0x7720, 0x1b90, 0x0dc8, 0x06e4, 0x0372},
	{0x01b9, 0x6948, 0x14a4, 0x0a52, 0x0529, 0x6b00, 0x1580, 0x0ac0},
	{0x0560, 0x02b0, 0x0158, 0x00ac, 0x0056, 0x002b, 0x6981, 0x7d54},
	{0x1eaa, 0x0f55, 0x6e3e, 0x171f, 0x621b, 0x7899, 0x75d8, 0x1aec},
	{0x0d76, 0x06bb, 0x6ac9, 0x7cf0, 0x1e78, 0x0f3c, 0x079e, 0x03cf},
	{0x6873, 0x7dad, 0x7742, 0x1ba1, 0x6444, 0x1222, 0x0911, 0x6d1c},
	{0x168e, 0x0b47, 0x6c37, 0x7f8f, 0x7653, 0x72bd, 0x70ca, 0x1865},
	{0x65a6, 0x12d3, 0x60fd, 0x79ea, 0x1cf5, 0x67ee, 0x13f7, 0x606f},
	{0x79a3, 0x7545, 0x7336, 0x199b, 0x6559, 0x7b38, 0x1d9c, 0x0ece},
	{0x0767, 0x6a27, 0x7c87, 0x77d7, 0x727f, 0x70ab, 0x71c1, 0x7174},
	{0x18ba, 0x0c5d, 0x6fba, 0x17dd, 0x627a, 0x113d, 0x610a, 0x1085},
	{0x61d6, 0x10eb, 0x61e1, 0x7964, 0x1cb2, 0x0e59, 0x6eb8, 0x175c},
	{0x0bae, 0x05d7, 0x6b7f, 0x7c2b, 0x7781, 0x7254, 0x192a, 0x0c95},
	{0x6fde, 0x17ef, 0x6263, 0x78a5, 0x75c6, 0x1ae3, 0x64e5, 0x7be6},
	{0x1df3, 0x676d, 0x7a22, 0x1d11, 0x671c, 0x138e, 0x09c7, 0x6d77},
	{0x7f2f, 0x7603, 0x7295, 0x70de, 0x186f, 0x65a3, 0x7b45, 0x7436},
	{0x1a1b, 0x6499, 0x7bd8, 0x1dec, 0x0ef6, 0x077b, 0x6a29, 0x7c80},
	{0x1e40, 0x0f20, 0x0790, 0x03c8, 0x01e4, 0x00f2, 0x0079, 0x69a8},
	{0x14d4, 0x0a6a, 0x0535, 0x6b0e, 0x1587, 0x6357, 0x783f, 0x758b},
	{0x7351, 0x703c, 0x181e, 0x0c0f, 0x6f93, 0x7e5d, 0x76ba, 0x1b5d}
};
#endif

static void trx_afifo_nop (unsigned int flag, unsigned int nop_cnt)
{
	unsigned int value = 0;

	value = ((flag & 0xf)<<10) | (nop_cnt & 0x3ff);
	if (vitual_fifo_start) {
		*(uint16_t *)(nand_param + vitual_fifo_offset) = value;
		vitual_fifo_offset += sizeof(uint16_t);
	} else {
		writel(value, NF2TFPT);
	}
}

static void trx_afifo_ncmd (unsigned int flag, unsigned int type, unsigned int wdat)
{
	unsigned int value = 0;

	value = (0x1<<14) | ((flag & 0xf) <<10) | (type<<8) | wdat;
	if (vitual_fifo_start) {
		*(uint16_t *)(nand_param + vitual_fifo_offset) = value;
		vitual_fifo_offset += sizeof(uint16_t);
	} else {
		writel(value, NF2TFPT);
	}
}

static void  trx_afifo_manual_cmd(int data)
{
	writel((data & 0xffff), NF2TFPT);
}

static int nf2_soft_reset(int num)
{
	volatile unsigned int tmp = 0;
	int ret = 0;

	writel(num, NF2SFTR);
	while(1) {
		tmp = readl(NF2SFTR) & 0x1f;
		if(tmp == 0)break;
	}

	return ret;
}
static void nf2_timing_init(int interface, int timing, int rnbtimeout, int phyread, int phydelay, int busw)
{
	unsigned int pclk_cfg = 0;
	writel((busw == 16)? 0x1: 0, NF2BSWMODE);
	pclk_cfg = readl(NF2PCLKM);
	pclk_cfg &= ~(0x7<<4);
	pclk_cfg |= (0x4<<4);
	writel(pclk_cfg, NF2PCLKM);
	if(interface == ASYNC_INTERFACE) {
		writel(0x0, NF2PSYNC);
		pclk_cfg = readl(NF2PCLKM);
		pclk_cfg |= (0x1);
		writel(pclk_cfg, NF2PCLKM);
	} else if(interface == ONFI_SYNC_INTERFACE) {
		writel(0x1, NF2PSYNC);
		pclk_cfg = readl(NF2PCLKM);
		pclk_cfg &= ~(0x1);
		writel(pclk_cfg, NF2PCLKM);
	} else {
		writel(0x2, NF2PSYNC);
		pclk_cfg = readl(NF2PCLKM);
		pclk_cfg &= ~(0x1); //sync clock pass
		writel(pclk_cfg, NF2PCLKM);
	}
	writel((timing & 0x1fffff), NF2AFTM);
	writel(((timing & 0xff000000) >> 24), NF2SFTM);
	writel(0xe001, NF2STSC);
	writel(phyread, NF2PRDC);
	writel(rnbtimeout, NF2TOUT);
	writel(phydelay, NF2PDLY);
	return;
}

static void trx_afifo_reg (unsigned int flag, unsigned int addr, unsigned int data)
{
	unsigned int value = 0;

	if (flag==0){
		value = (0xf<<12) | (flag<<8) | addr;
		writel(value, NF2TFPT);
		value = data & 0xffff;
		writel(value, NF2TFPT);
		value = (data>>16) & 0xffff;
		writel(value, NF2TFPT);
	} else if (flag==1) {
		value = (0xf<<12) | (flag<<8) | addr;
		writel(value, NF2TFPT);
		value = data & 0xffff;
		writel(value, NF2TFPT);
	} else {
		value = (0xf<<12) | (flag<<8) | addr;
		writel(value, NF2TFPT);
	}
}

static int imap_read_page_raw(int dev_id, int dma_unit_offset, int page, unsigned char *buf, int len)
{
	int tmp;
	nf2_soft_reset(1);
	writel(0x1, NF2FFCE);
	trx_afifo_nop(0x7, 100);
	trx_afifo_ncmd(0x0, 0x0, 0x00);
	trx_afifo_ncmd(0x0, 0x1, (0 & 0xff));
	trx_afifo_ncmd(0x0, 0x1, ((0 >> 8) & 0xff));
	trx_afifo_ncmd(0x0, 0x1, (page & 0xff));
	trx_afifo_ncmd(0x0, 0x1, ((page >> 8) & 0xff));
	trx_afifo_ncmd(0x0, 0x1, ((page >> 16) & 0xff));
	trx_afifo_ncmd(0x0, 0x0, 0x30);
	trx_afifo_nop(0x7, 100);
	trx_afifo_nop(0x5, 100);
	writel(0, NF2RANDME);
	writel(0, NF2RADR0);
	writel(0, NF2RADR1);
	writel(0, NF2RADR2);
	writel(0, NF2RADR3);
	writel(0, NF2CADR);
	writel(((1024 << 16) | 1), NF2SBLKN);
	writel((3 << 8), NF2ECCC);
	writel(1024, NF2PGEC);
	writel(0, NF2ECCLEVEL);
	writel(1024, NF2SBLKS);
	writel(1, NF2DMAC);
	writel((int)buf, NF2SADR0);
	trx_afifo_reg(0x8, 1, 0);
	writel(0x0, NF2FFCE);
	writel(0xe, NF2CSR);
	writel(0x5, NF2STRR);
	while(1) {
		tmp = (readl(NF2INTR) & (1 << 5));
		if(tmp == (1 << 5))
			break;
	}
	writel(0xf, NF2CSR);
	writel(0xffffff, NF2INTR);
	writel(0, NF2RANDME);
	return 0;
}

struct nand_config old_nc;
struct nand_config * get_nand_config(void)
{
	struct nand_config *nc;
	struct nand_config_new *new_nc;

	if(IROM_IDENTITY != IX_CPUID_X15_NEW)
	{
		nc = irf->nand_get_config(NAND_CFG_BOOT);
		return nc;
	} else	{        /* new irom */
	//	irf->printf("===&new_nc 0x%x badpagemajor 0x%x minor 0x%x \n", &new_nc, &new_nc->badpagemajor, &new_nc->badpageminor);
		new_nc = irf->nand_get_config(NAND_CFG_BOOT);
		irf->memcpy(&old_nc, new_nc, 8);
		irf->memcpy((((uint8_t *)&old_nc) + 8), ((uint8_t *)new_nc) + 12, 148);

		return &old_nc;
	}

}

static int imap_read_page(int dev_id, int dma_unit_offset, int page, unsigned char *buf, int len)
{
	struct nand_config *nc = get_nand_config();
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)nand_param;
	int col, col_offset, tmp, ecc_info = 0, seed_pattern = 0, busw;
	unsigned *data_seed_1k, *ecc_seed_1k;
	char oob_buf[20];

	busw = (nc->busw == 16) ? 1 : 0;
	if (dev_id == DEV_UBOOT0) {
		if (IROM_IDENTITY == IX_CPUID_X15_NEW){
			col = ((1024 + 4) * (dma_unit_offset + 1) + (NAND_BCH80_ECC_SIZE * 2) * dma_unit_offset);
			col_offset = (1024 + 4 + NAND_BCH80_ECC_SIZE * 2) * dma_unit_offset;
		} else {
			col = ((uboot0_unit + 4) * (dma_unit_offset + 1) + (NAND_BCH127_ECC_SIZE + NAND_BCH8_SECC_SIZE) * dma_unit_offset);
			col_offset = (uboot0_unit + 4 + NAND_BCH127_ECC_SIZE + NAND_BCH8_SECC_SIZE) * dma_unit_offset;
		}
	} else {
		col = ((1 << infotm_nand_save->page_shift) + 20);
		col_offset = 0;
	}
	nf2_soft_reset(1);

	writel(0x1, NF2FFCE);
	trx_afifo_nop(0x7, 100);
	trx_afifo_ncmd(0x0, 0x0, 0x00);
	trx_afifo_ncmd(0x0, 0x1, ((col >> busw) & 0xff));
	trx_afifo_ncmd(0x0, 0x1, (((col >> 8) >> busw) & 0xff));
	trx_afifo_ncmd(0x0, 0x1, (page & 0xff));
	trx_afifo_ncmd(0x0, 0x1, ((page >> 8) & 0xff));
	trx_afifo_ncmd(0x0, 0x1, ((page >> 16) & 0xff));
	trx_afifo_ncmd(0x0, 0x0, 0x30);
	trx_afifo_nop(0x7, 100);
	trx_afifo_nop(0x5, 100);

	writel(1, NF2RANDME);
	if (dev_id == DEV_UBOOT0) {
		if (fpga_test == 0) {
			if (IROM_IDENTITY == IX_CPUID_X15_NEW) {
				data_seed_1k = (unsigned *)uboot0_data_seed_1k;
				ecc_seed_1k = (unsigned *)uboot0_ecc_seed_1k;
				//user irom seed OK
				//data_seed_1k = (unsigned *)nc->data_seed_1k;
				//ecc_seed_1k = (unsigned *)nc->ecc_seed_1k;
				tmp = (page & 0xff);
			} else {
				data_seed_1k = (unsigned *)nc->seed;
				ecc_seed_1k = (unsigned *)nc->ecc_seed;
				tmp = 0;
			}
		} else {
			data_seed_1k = (unsigned *)nc->seed;
			ecc_seed_1k = (unsigned *)nc->ecc_seed;
			tmp = 0;
		}
		seed_pattern = ((*(unsigned *)((unsigned *)data_seed_1k + tmp * 8 + 1) << 16) | *(unsigned *)((unsigned *)data_seed_1k + tmp * 8));
		writel(seed_pattern, NF2DATSEED0);
		seed_pattern = ((*(unsigned *)((unsigned *)data_seed_1k + tmp * 8 + 3) << 16) | *(unsigned *)((unsigned *)data_seed_1k + tmp * 8 + 2));
		writel(seed_pattern, NF2DATSEED1);
		seed_pattern = ((*(unsigned *)((unsigned *)data_seed_1k + tmp * 8 + 5) << 16) | *(unsigned *)((unsigned *)data_seed_1k + tmp * 8 + 4));
		writel(seed_pattern, NF2DATSEED2);
		seed_pattern = ((*(unsigned *)((unsigned *)data_seed_1k + tmp * 8 + 7) << 16) | *(unsigned *)((unsigned *)data_seed_1k + tmp * 8 + 6));
		writel(seed_pattern, NF2DATSEED3);
		seed_pattern = ((*(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8 + 1) << 16) | *(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8));
		writel(seed_pattern, NF2ECCSEED0);
		seed_pattern = ((*(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8 + 3) << 16) | *(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8 + 2));
		writel(seed_pattern, NF2ECCSEED1);
		seed_pattern = ((*(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8 + 5) << 16) | *(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8 + 4));
		writel(seed_pattern, NF2ECCSEED2);
		seed_pattern = ((*(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8 + 7) << 16) | *(unsigned *)((unsigned *)ecc_seed_1k + tmp * 8 + 6));
		writel(seed_pattern, NF2ECCSEED3);
		if ((IROM_IDENTITY == IX_CPUID_X15) || (IROM_IDENTITY == IX_CPUID_820_0) || (IROM_IDENTITY == IX_CPUID_820_1)) {
			writel(nc->seed[0], NF2DATSEED0);
			writel(nc->seed[1], NF2DATSEED1);
			writel(nc->seed[2], NF2DATSEED2);
			writel(nc->seed[3], NF2DATSEED3);
			writel((nc->ecc_seed[1]<<16 | nc->ecc_seed[0]), NF2ECCSEED0);
			writel((nc->ecc_seed[3]<<16 | nc->ecc_seed[2]), NF2ECCSEED1);
			writel((nc->ecc_seed[5]<<16 | nc->ecc_seed[4]), NF2ECCSEED2);
			writel((nc->ecc_seed[7]<<16 | nc->ecc_seed[6]), NF2ECCSEED3);
		}
	} else {
		seed_pattern = (page & (infotm_nand_save->seed_cycle - 1));
		writel((infotm_nand_save->data_seed[seed_pattern][1]<<16 | infotm_nand_save->data_seed[seed_pattern][0]), NF2DATSEED0);
		writel((infotm_nand_save->data_seed[seed_pattern][3]<<16 | infotm_nand_save->data_seed[seed_pattern][2]), NF2DATSEED1);
		writel((infotm_nand_save->data_seed[seed_pattern][5]<<16 | infotm_nand_save->data_seed[seed_pattern][4]), NF2DATSEED2);
		writel((infotm_nand_save->data_seed[seed_pattern][7]<<16 | infotm_nand_save->data_seed[seed_pattern][6]), NF2DATSEED3);
		writel((infotm_nand_save->ecc_seed[seed_pattern][1]<<16 | infotm_nand_save->ecc_seed[seed_pattern][0]), NF2ECCSEED0);
		writel((infotm_nand_save->ecc_seed[seed_pattern][3]<<16 | infotm_nand_save->ecc_seed[seed_pattern][2]), NF2ECCSEED1);
		writel((infotm_nand_save->ecc_seed[seed_pattern][5]<<16 | infotm_nand_save->ecc_seed[seed_pattern][4]), NF2ECCSEED2);
		writel((infotm_nand_save->ecc_seed[seed_pattern][7]<<16 | infotm_nand_save->ecc_seed[seed_pattern][6]), NF2ECCSEED3);
	}
	writel(0, NF2RADR0);
	writel(0, NF2RADR1);
	writel(0, NF2RADR2);
	writel(0, NF2RADR3);
	writel((((col >> busw) << 16) | (col_offset >> busw)), NF2CADR);
	writel(((1024 << 16) | 1), NF2SBLKN);
	if (dev_id == DEV_UBOOT0) {
		//new x15 irom 80bit ecc, half mode, no secc
		if (IROM_IDENTITY == IX_CPUID_X15_NEW){
			writel(((1 << 18) | (3 << 8) | (12 << 4) | (1 << 3) | 0x1), NF2ECCC);
			writel((((140 * 2) << 16) | (1024 + 4)), NF2PGEC);
			writel(((80 << 8) | (0 << 16)), NF2ECCLEVEL);
			writel(((4 << 16) | 1024), NF2SBLKS);
		} else {
			writel(((1 << 16) | (3 << 8) | (15 << 4) | (1 << 3) | 0x1), NF2ECCC);
			writel((((224 * (uboot0_unit >> 10) + 12) << 16) | (uboot0_unit + 4)), NF2PGEC);
			writel(((127 << 8) | (4 << 16)), NF2ECCLEVEL);
			writel(((4 << 16) | uboot0_unit), NF2SBLKS);
		}
	} else {
		writel(((infotm_nand_save->secc_mode << 17) | (1 << 16) | (3 << 8) | (infotm_nand_save->ecc_mode << 4) | (1 << 3) | 0x1), NF2ECCC);
		writel((((infotm_nand_save->ecc_bytes * (1 << (infotm_nand_save->page_shift - 10)) + infotm_nand_save->secc_bytes) << 16) | col), NF2PGEC);
		writel(((infotm_nand_save->ecc_bits << 8) | (infotm_nand_save->secc_bits << 16)), NF2ECCLEVEL);
		writel(((20 << 16) | (1 << infotm_nand_save->page_shift)), NF2SBLKS);
	}
	writel(((1 << 2) | 1), NF2DMAC);
	writel((int)buf, NF2SADR0);
	writel((int)oob_buf, NF2SADR1);

	trx_afifo_reg(0x8, 1, 0);
	writel(0x0, NF2FFCE);
	writel(0xe, NF2CSR);
	writel(0x7, NF2STRR);
	while(1) {
		tmp = (readl(NF2INTR) & (1 << 5));
		if(tmp == (1 << 5))
			break;
	}
	ecc_info = readl(NF2ECCINFO8);

	writel(0xf, NF2CSR);
	writel(0xffffff, NF2INTR);
	writel(0, NF2RANDME);

	if (dev_id == DEV_UBOOT0) {
		if(ecc_info & 0x1)
			return -1;
	} else {
		if(ecc_info & ((1 << (1 << (infotm_nand_save->page_shift - 10))) - 1))
			return -1;
	}

	return 0;
}

static void nand_uboot0_set_retry_level(int retry_level)
{
	int i, tmp = 0, buf_offset = 0, retry_count = 0, retry_cmd_cnt;

	buf_offset = sizeof(struct infotm_nand_para_save);
	do {
		if ((*(uint32_t *)(nand_param + buf_offset)) == RETRY_PARAMETER_CMD_MAGIC) {
			if (retry_count == retry_level)
				break;
			retry_count++;
		}
		buf_offset += sizeof(uint32_t);
	} while (buf_offset < IMAPX800_PARA_SAVE_SIZE);
	if (buf_offset >= IMAPX800_PARA_SAVE_SIZE)
		return;
	retry_cmd_cnt = *(uint16_t *)(nand_param + buf_offset + sizeof(uint32_t));

	nf2_soft_reset(1);
	writel(0x1, NF2FFCE);
	for (i=0; i<retry_cmd_cnt; i++)
		trx_afifo_manual_cmd(*(uint16_t *)(nand_param + buf_offset + sizeof(uint32_t) + sizeof(uint16_t) + i * sizeof(uint16_t)));

	writel(0x0, NF2FFCE);
	writel(0xe, NF2CSR);
	writel(0x1, NF2STRR);
	while(1) {
		tmp = readl(NF2INTR);
		if(tmp & 1)
			break;
	}

	writel(0xf, NF2CSR);
	writel(0xffffff, NF2INTR);
	return;
}

int nand_uboot0_read(unsigned char *buf, int dev_id, int offset, int len)
{
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)nand_param;
	int error = 0, i = 0, j, k, page_cnt, page_shift, page_addr, retry_level_support = 0, retry_level, retry_count = 0;

	if (infotm_nand_save->head_magic != IMAPX800_UBOOT_PARA_MAGIC)
		return -1;

	if (infotm_nand_save->retry_para_magic == IMAPX800_RETRY_PARA_MAGIC)
		retry_level_support = infotm_nand_save->retry_level_support;

	if (dev_id == DEV_UBOOT0)
		page_shift = ((IROM_IDENTITY == IX_CPUID_X15 || IROM_IDENTITY == IX_CPUID_X15_NEW) ? 10 : 11);
	else
		page_shift = infotm_nand_save->page_shift;
	page_cnt = ((len + ((1 << page_shift) - 1)) >> page_shift);

	for (j=0; j<page_cnt; j++) {
		for (i=0; i<IMAPX800_BOOT_COPY_NUM; i++) {
			retry_count = 0;
			do {
				if (dev_id == DEV_ITEM)
					page_addr = (infotm_nand_save->item_blk[i] << infotm_nand_save->pages_per_blk_shift) + (offset >> page_shift);
				else if (dev_id == DEV_UBOOT1)
					page_addr = (infotm_nand_save->uboot1_blk[i] << infotm_nand_save->pages_per_blk_shift) + (offset >> page_shift);
				else
					page_addr = (infotm_nand_save->uboot0_blk[i] << infotm_nand_save->pages_per_blk_shift) + (offset >> page_shift);

				error = imap_read_page(dev_id, 0, page_addr + j, buf + (j << page_shift), len);
				//spl_printf("read %d %x %d \n", dev_id, page_addr + j, error);
				if (error && (retry_count == 0)) {
					imap_read_page_raw(dev_id, 0, page_addr + j, buf + (j << page_shift), 1024);
					for (k=0; k<1024; k++) {
						if (buf[(j << page_shift) + k] != 0xff)
							break;
					}
					if (k >= 1024)
						error = 0;
				}
				if (error) {
					if (retry_count > retry_level_support)
						break;
					retry_level = infotm_nand_save->retry_level[0] + 1;
					if (retry_level > retry_level_support)
						retry_level = 0;
					nand_uboot0_set_retry_level(retry_level);
					infotm_nand_save->retry_level[0] = retry_level;
					if (retry_count == retry_level_support)
						break;
					retry_count++;
				}
			} while (retry_level_support && error);
			if ((retry_count > 0) &&  (infotm_nand_save->retry_level_circulate == 0)) {
				retry_level = 0;
				nand_uboot0_set_retry_level(retry_level);
				infotm_nand_save->retry_level[0] = retry_level;
			}
			if (!error) {
				if (retry_count > 0)
					spl_printf("retry success %d %x %d \n", dev_id, page_addr + j, retry_count);
				break;
			} else {
				spl_printf("read failed %d %x %d \n", dev_id, page_addr + j, error);
			}
		}
	}

	return error;
}

int imap_get_para_from_page(unsigned char *buf, int len)
{
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)nand_param;
	int ret = 0, page, i, read_page_num, read_page_once = 1, buf_offset = 0;

	read_page_num = 0;
	if (sizeof(struct infotm_nand_para_save) > IMAPX800_PARA_SAVE_SIZE)
		return -1;
	read_page_once = (IROM_IDENTITY == IX_CPUID_X15 || IROM_IDENTITY == IX_CPUID_X15_NEW) ? (IMAPX800_PARA_SAVE_SIZE >> 10) : (IMAPX800_PARA_SAVE_SIZE >> 11) ;

	do {
		if (IROM_IDENTITY == IX_CPUID_X15_NEW)
			page = 0;
		else
			page = (IMAPX800_BOOT_PAGES_PER_COPY >> 1);

		do {
			ret = imap_read_page(DEV_UBOOT0, read_page_num, page, buf + buf_offset, len);
			if (ret == 0)
				break;
			imap_read_page_raw(DEV_UBOOT0, read_page_num, page, buf + buf_offset, 1024);
			for (i=0; i<1024; i++) {
				if (buf[buf_offset + i] != 0xff)
					break;
			}
			if (i >= 1024) {
				spl_printf("para page is all ff data %d\n", page);
				page+=128;
			} else {
				page++;
				if ((page & ((IMAPX800_BOOT_PAGES_PER_COPY >> 1) - 1)) >= IMAPX800_PARA_ONESEED_SAVE_PAGES)
					page += ((IMAPX800_BOOT_PAGES_PER_COPY >> 1) - IMAPX800_PARA_ONESEED_SAVE_PAGES);
				if (((page & ((IMAPX800_BOOT_PAGES_PER_COPY >> 1) - 1)) == 0))
					page += (IMAPX800_BOOT_PAGES_PER_COPY >> 1);
			}
		} while (page < IMAPX800_PARA_SAVE_PAGES);
		if (page >= IMAPX800_PARA_SAVE_PAGES)
			return -1;
		read_page_num++;
		buf_offset += uboot0_unit;
		if (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC) {
			if (infotm_nand_save->retry_para_magic != IMAPX800_RETRY_PARA_MAGIC)
				break;
		}
		if (buf_offset >= IMAPX800_PARA_SAVE_SIZE)
			break;
	} while (read_page_num < read_page_once);

	return ret;
}

void param_transmit_to_uboot1(void)
{
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)nand_param;
	if (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC) {
		if((boot_dev_id == DEV_MMC(1)) && item_exist("nand.erase") && (item_integer("nand.erase", 0) == 1)) {
			spl_printf("need to scrub all nand\n");
		} else {
			rrtb = rballoc("nandrrtb", RESERVED_RRTB_SIZE);
			irf->memcpy(rrtb, nand_param, IMAPX800_PARA_SAVE_SIZE);
		}
	}
	return;
}

int nand_uboot0_init(void)
{
	//struct nand_config *nc = (struct nand_config *)irf->nand_get_config(NAND_CFG_BOOT);
	struct infotm_nand_para_save *infotm_nand_save = (struct infotm_nand_para_save *)nand_param;
	int ret, retry_level_saved;

#if defined(CONFIG_IMAPX_FPGATEST)
	fpga_test = 1;
#endif
	if (fpga_test == 0) {
		nf2_timing_init(ASYNC_INTERFACE, DEFAULT_TIMING, DEFAULT_RNB_TIMEOUT, DEFAULT_PHY_READ, DEFAULT_PHY_DELAY, BUSW_8BIT);
	}
	uboot0_unit = (IROM_IDENTITY == IX_CPUID_X15 || IROM_IDENTITY == IX_CPUID_X15_NEW) ? 1024 : 2048;
	ret = imap_get_para_from_page(nand_param, IMAPX800_PARA_SAVE_SIZE);
	if (ret)
		return ret;

	spl_printf("uboot0_inited %x \n", infotm_nand_save->head_magic);
	if (infotm_nand_save->retry_para_magic == IMAPX800_RETRY_PARA_MAGIC) {
		if (fpga_test == 1) {
			infotm_nand_save->retry_level[1] = rtcbit_get("retry_reboot");
			if (infotm_nand_save->retry_level[1])
				rtcbit_set("retry_reboot", 0);
		} else {
			retry_level_saved = rtcbit_get("retry_reboot");
			infotm_nand_save->retry_level[0] = (retry_level_saved & 0xf);
			infotm_nand_save->retry_level[1] = ((retry_level_saved >> 4) & 0xf);
			rtcbit_set("retry_reboot", 0);
		}
		spl_printf("retrylevel = %d %d \n", infotm_nand_save->retry_level[0], infotm_nand_save->retry_level[1]);
		//spl_printf("level = %d %d \n", infotm_nand_save->retry_level[0], infotm_nand_save->retry_level[1]);
	}
	return (infotm_nand_save->head_magic == IMAPX800_UBOOT_PARA_MAGIC) ? 0 : 1;
}