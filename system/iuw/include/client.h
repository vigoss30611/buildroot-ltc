#ifndef __CLIENT_H__
#define __CLIENT_H__

#define MAX_DEVICE 11

struct device_id {
	char      name[16];
	int 	  id;
	int       cmd;
};

struct device_desr {
	char      name[16];
	uint64_t  size;
	int       align;
	int       offset;
};

struct test_conclusion {
    uint64_t data;
    int sum;    
    int transfor_number;
    int error;
    uint8_t unit[8];
    double dax;
    uint8_t *buf;
};

struct device_id client_id[MAX_DEVICE] = {
	{
		.name = "e2p",
		.id   = 0x3,
		.cmd  = IUW_SVL_VSWR,
	},
	{
		.name = "bnd",
		.id   = 0x0,
		.cmd  = IUW_SVL_NBWR,
	},
	{
		.name = "nand",
		.id   = 0x1,
		.cmd  = IUW_SVL_NRWR,
	},
	{
		.name = "fnd",
		.id   = 0x2,
		.cmd  = IUW_SVL_NFWR,
	},
	{
		.name = "mmc0",
		.id   = 0x6,
		.cmd  = IUW_SVL_VSWR,
	},
	{
		.name = "mmc1",
		.id   = 0x7,
		.cmd  = IUW_SVL_VSWR,
	},
	{
		.name = "mmc2",
		.id   = 0x8,
		.cmd  = IUW_SVL_VSWR,
	},
	{
		.name = "udisk0",
		.id   = 0x9,
		.cmd  = IUW_SVL_VSWR,
	},
	{
		.name = "udisk1",
		.id   = 0xa,
		.cmd  = IUW_SVL_VSWR,
	},
	{
		.name = "sata",
		.id   = 0x5,
		.cmd  = IUW_SVL_VSWR,
	},
	{
		.name = "flash",
		.id   = 0x4,
		.cmd  = IUW_SVL_VSWR,
	},
};

#endif
