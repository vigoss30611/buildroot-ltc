#ifndef __PAD_INFO_H__
#define __PAD_INFO_H__

#include <mach/pad.h>
	//PAD_INIT(_name, _owner, _index, _bank, _mode,  _pull, _pull_en, _level)
struct pad_init_info  rgb0_pads_info[] = {
	PAD_INIT("disp0",	"rgb0",	0,	0, "function",	"down", 1,  	0),
	PAD_INIT("disp1",	"rgb0",	1,	0, "function",	"down", 1,	0),
	PAD_INIT("disp2",	"rgb0",	2,	0, "function",	"down", 1,	0),
	PAD_INIT("disp3",	"rgb0",	3,	0, "function",	"down", 1,	0),
	PAD_INIT("disp4",	"rgb0",	4,	0, "function",	"down", 1,	0),
	PAD_INIT("disp5",	"rgb0",	5,	0, "function",	"down", 1,	0),
	PAD_INIT("disp6",	"rgb0",	6,	0, "function",	"down", 1,	0),
	PAD_INIT("disp7",	"rgb0",	7,	0, "function",	"down", 1,	0),
	PAD_INIT("disp8",	"rgb0",	8,	0, "function",	"down", 1,	0),
	PAD_INIT("disp9",	"rgb0",	9,	0, "function",	"down", 1,	0),
	PAD_INIT("disp10",	"rgb0",	10,	0, "function",	"down", 1,	0),
	PAD_INIT("disp11",	"rgb0",	11,	0, "function",	"down", 1,	0),
	PAD_INIT("disp12",	"rgb0",	12,	0, "function",	"down", 1,	0),
	PAD_INIT("disp13",	"rgb0",	13,	0, "function",	"down", 1,	0),
	PAD_INIT("disp14",	"rgb0",	14,	0, "function",	"down", 1,	0),
	PAD_INIT("disp15",	"rgb0",	15,	0, "function",	"down", 1,	0),
	PAD_INIT("disp16",	"rgb0",	16,	1, "function",	"down", 1,	0),
	PAD_INIT("disp17",	"rgb0",	17,	1, "function",	"down", 1,	0),
	PAD_INIT("disp18",	"rgb0",	18,	1, "function",	"down", 1,	0),
	PAD_INIT("disp19",	"rgb0",	19,	1, "function",	"down", 1,	0),
	PAD_INIT("disp20",	"rgb0",	20,	1, "function",	"down", 1,	0),
	PAD_INIT("disp21",	"rgb0",	21,	1, "function",	"down", 1,	0),
	PAD_INIT("disp22",	"rgb0",	22,	1, "function",	"down", 1,	0),
	PAD_INIT("disp23",	"rgb0",	23,	1, "function",	"down", 1,	0),
	PAD_INIT("disp24",	"rgb0",	24,	1, "function",	"down", 1,	0),
	PAD_INIT("disp25",	"rgb0",	25,	1, "function",	"down", 1,	0),
	PAD_INIT("disp26",	"rgb0",	26,	1, "function",	"down", 1,	0),
	PAD_INIT("disp27",	"rgb0",	27,	1, "function",	"down", 1,	0),
};

struct pad_init_info  phymux_pads_info[] = {
	PAD_INIT("disp11",	"phy-mux",	11,	0, "function",	"down", 1,	0),
	PAD_INIT("disp12",	"phy-mux",	12,	0, "function",	"down", 1,	0),
	PAD_INIT("disp13",	"phy-mux",	13,	0, "function",	"down", 1,	0),
	PAD_INIT("disp14",	"phy-mux",	14,	0, "function",	"down", 1,	0),
	PAD_INIT("disp15",	"phy-mux",	15,	0, "function",	"down", 1,	0),
	PAD_INIT("disp16",	"phy-mux",	16,	1, "function",	"down", 1,	0),
	PAD_INIT("disp17",	"phy-mux",	17,	1, "function",	"down", 1,	0),
	PAD_INIT("disp18",	"phy-mux",	18,	1, "function",	"down", 1,	0),
	PAD_INIT("disp19",	"phy-mux",	19,	1, "function",	"down", 1,	0),
};

struct pad_init_info  srgb_pads_info[] = {
	PAD_INIT("disp0",	"srgb",	0,	0, "function",	"down", 1,	0),
	PAD_INIT("disp1",	"srgb",	1,	0, "function",	"down", 1,	0),
	PAD_INIT("disp2",	"srgb",	2,	0, "function",	"down", 1,	0),
	PAD_INIT("disp3",	"srgb",	3,	0, "function",	"down", 1,	0),
	PAD_INIT("disp4",	"srgb",	4,	0, "function",	"down", 1,	0),
	PAD_INIT("disp5",	"srgb",	5,	0, "function",	"down", 1,	0),
	PAD_INIT("disp6",	"srgb",	6,	0, "function",	"down", 1,	0),
	PAD_INIT("disp7",	"srgb",	7,	0, "function",	"down", 1,	0),
	PAD_INIT("disp8",	"srgb",	8,	0, "function",	"down", 1,	0),
	PAD_INIT("disp9",	"srgb",	9,	0, "function",	"down", 1,	0),
	PAD_INIT("disp10",	"srgb",	10,	0, "function",	"down", 1,	0),
	PAD_INIT("disp27",	"srgb",	27,	1, "function",	"down", 1,	0), /*SRGB_CLK*/
};

struct pad_init_info  bt656_pads_info[] = {
	PAD_INIT("disp11",	"bt656",	11,	0, "function",	"down", 1,	0),
	PAD_INIT("disp12",	"bt656",	12,	0, "function",	"down", 1,	0),
	PAD_INIT("disp13",	"bt656",	13,	0, "function",	"down", 1,	0),
	PAD_INIT("disp14",	"bt656",	14,	0, "function",	"down", 1,	0),
	PAD_INIT("disp15",	"bt656",	15,	0, "function",	"down", 1,	0),
	PAD_INIT("disp16",	"bt656",	16,	1, "function",	"down", 1,	0),
	PAD_INIT("disp17",	"bt656",	17,	1, "function",	"down", 1,	0),
	PAD_INIT("disp18",	"bt656",	18,	1, "function",	"down", 1,	0),
	PAD_INIT("disp19",	"bt656",	19,	1, "function",	"down", 1,	0),
};

struct pad_init_info  cam_pads_info[] = {
	PAD_INIT("camd0",	"cam",	28,	1, "function",	"down", 1,	0),
	PAD_INIT("camd1",	"cam",	29,	1, "function",	"down", 1,	0),
	PAD_INIT("camd2",	"cam",	30,	1, "function",	"down", 1,	0),
	PAD_INIT("camd3",	"cam",	31,	1, "function",	"down", 1,	0),
	PAD_INIT("camd4",	"cam",	32,	2, "function",	"down", 1,	0),
	PAD_INIT("camd5",	"cam",	33,	2, "function",	"down", 1,	0),
	PAD_INIT("camd6",	"cam",	34,	2, "function",	"down", 1,	0),
	PAD_INIT("camd7",	"cam",	35,	2, "function",	"down", 1,	0),
	PAD_INIT("camd8",	"cam",	36,	2, "function",	"down", 1,	0),
	PAD_INIT("camd9",	"cam",	37,	2, "function",	"down", 1,	0),
	PAD_INIT("camd10",	"cam",	38,	2, "function",	"down", 1,	0),
	PAD_INIT("camd11",	"cam",	39,	2, "function",	"down", 1,	0),
	PAD_INIT("camd12",	"cam",	40,	2, "function",	"down", 1,	0),
	PAD_INIT("camd13",	"cam",	41,	2, "function",	"down", 1,	0),
	PAD_INIT("camd14",	"cam",	42,	2, "function",	"down", 1,	0),
	PAD_INIT("camd15",	"cam",	43,	2, "function",	"down", 1,	0),
};

struct pad_init_info  camif_pads_info[] = {
	PAD_INIT("camd4",	"camif",	32,	2, "function",	"down", 1,	0),
	PAD_INIT("camd5",	"camif",	33,	2, "function",	"down", 1,	0),
	PAD_INIT("camd6",	"camif",	34,	2, "function",	"down", 1,	0),
	PAD_INIT("camd7",	"camif",	35,	2, "function",	"down", 1,	0),
	PAD_INIT("camd8",	"camif",	36,	2, "function",	"down", 1,	0),
	PAD_INIT("camd9",	"camif",	37,	2, "function",	"down", 1,	0),
	PAD_INIT("camd10",	"camif",	38,	2, "function",	"down", 1,	0),
	PAD_INIT("camd11",	"camif",	39,	2, "function",	"down", 1,	0),
	PAD_INIT("camd12",	"camif",	40,	2, "function",	"down", 1,	0),
	PAD_INIT("camd13",	"camif",	41,	2, "function",	"down", 1,	0),
	PAD_INIT("camd14",	"camif",	42,	2, "function",	"down", 1,	0),
	PAD_INIT("camd15",	"camif",	43,	2, "function",	"down", 1,	0),
};

	//PAD_INIT("camd16",	"cam",	44,	2, "function",	"down", 1,	0),

struct pad_init_info  uart0mux_pads_info[] = {
	PAD_INIT("camd2",	"uart0-mux",	30,	1, "function",	"down", 1,	0),
	PAD_INIT("camd3",	"uart0-mux",	31,	1, "function",	"down", 1,	0),
	PAD_INIT("camd4",	"uart0-mux",	32,	2, "function",	"down", 1,	0),
	PAD_INIT("camd5",	"uart0-mux",	33,	2, "function",	"down", 1,	0),
};

struct pad_init_info  iic3_pads_info[] = {
	PAD_INIT("camd17",	"iic3",	45,	2, "function",	"up", 1,	0),
	PAD_INIT("camd18",	"iic3",	46,	2, "function",	"up", 1,	0),
};

struct pad_init_info  iis0_pads_info[] = {
	PAD_INIT("comd0",	"iis0",	47,	2, "function",	"up", 1,	0),
	PAD_INIT("comd1",	"iis0",	48,	3, "function",	"up", 1,	0),
	PAD_INIT("comd2",	"iis0",	49,	3, "function",	"up", 1,	0),
	PAD_INIT("comd3",	"iis0",	50,	3, "function",	"up", 1,	0),
	PAD_INIT("comd4",	"iis0", 51,	3, "function",	"up", 1,	0),
};

struct pad_init_info  pcm0_pads_info[] = {
	PAD_INIT("comd5",	"pcm0", 52,	3, "function",	"down", 1,	0),
	PAD_INIT("comd6",	"pcm0", 53,	3, "function",	"down", 1,	0),
	PAD_INIT("comd7",	"pcm0", 54,	3, "function",	"down", 1,	0),
	PAD_INIT("comd8",	"pcm0", 55,	3, "function",	"down", 1,	0),
	PAD_INIT("comd9",	"pcm0", 56,	3, "function",	"down", 1,	0),
};

struct pad_init_info  phy_pads_info[] = {
	PAD_INIT("comd10",	"phy",	57,	3, "function",	"down", 1,	0),
	PAD_INIT("comd11",	"phy",	58,	3, "function",	"down", 1,	0),
	PAD_INIT("comd12",	"phy",	59,	3, "function",	"down", 1,	0),
	PAD_INIT("comd13",	"phy",	60,	3, "function",	"down", 1,	0),
	PAD_INIT("comd14",	"phy",	61,	3, "function",	"down", 1,	0),
	PAD_INIT("comd15",	"phy",	62,	3, "function",	"down", 1,	0),
	PAD_INIT("comd16",	"phy",	63,	3, "function",	"down", 1,	0),
	PAD_INIT("comd17",	"phy",	64,	4, "function",	"down", 1,	0),
	PAD_INIT("comd18",	"phy",	65,	4, "function",	"down", 1,	0),
};

struct pad_init_info  sd2mux_pads_info[] = {
	PAD_INIT("comd10",	"sd2-mux",	57,	3, "function",	"down", 1,	0),
	PAD_INIT("comd11",	"sd2-mux",	58,	3, "function",	"down", 1,	0),
	PAD_INIT("comd12",	"sd2-mux",	59,	3, "function",	"down", 1,	0),
	PAD_INIT("comd13",	"sd2-mux",	60,	3, "function",	"down", 1,	0),
	PAD_INIT("comd14",	"sd2-mux",	61,	3, "function",	"down", 1,	0),
	PAD_INIT("comd15",	"sd2-mux",	62,	3, "function",	"down", 1,	0),
};

struct pad_init_info  ssp1_pads_info[] = {
	PAD_INIT("comd19",	"ssp1",	66,	4, "function",	"up", 1,	0),
	PAD_INIT("comd20",	"ssp1",	67,	4, "function",	"up", 1,	0),
	PAD_INIT("comd21",	"ssp1",	68,	4, "function",	"up", 1,	0),
	PAD_INIT("comd22",	"ssp1", 69,	4, "function",	"up", 1,	0),
};

struct pad_init_info  sd0_pads_info[] = {
	PAD_INIT("comd23",	"sd0",	70,	4, "function",	"up", 1,	0),
	PAD_INIT("comd24",	"sd0",	71,	4, "function",	"up", 1,	0),
	PAD_INIT("comd25",	"sd0",	72,	4, "function",	"up", 1,	0),
	PAD_INIT("comd26",	"sd0",	73,	4, "function",	"up", 1,	0),
	PAD_INIT("comd27",	"sd0",	74,	4, "function",	"up", 1,	0),
	PAD_INIT("comd28",	"sd0",	75,	4, "function",	"up", 1,	0),
	PAD_INIT("comd29",	"sd0",	76,	4, "function",	"up", 1,	0),
	PAD_INIT("comd30",	"sd0",	77,	4, "function",	"up", 1,	0),
	PAD_INIT("comd31",	"sd0",	78,	4, "function",	"up", 1,	0),
	PAD_INIT("comd32",	"sd0",	79,	4, "function",	"up", 1,	0),
	PAD_INIT("comd33",	"sd0",	80,	5, "function",	"up", 1,	0),
	PAD_INIT("comd34",	"sd0",	81,	5, "function",	"up", 1,	0),
};

struct pad_init_info  ssp0mux_pads_info[] = {
	PAD_INIT("comd23",	"ssp0",	70,	4, "function",	"up", 1,	0),
	PAD_INIT("comd24",	"ssp0",	71,	4, "function",	"up", 1,	0),
	PAD_INIT("comd25",	"ssp0",	72,	4, "function",	"up", 1,	0),
	PAD_INIT("comd26",	"ssp0",	73,	4, "function",	"up", 1,	0),
	PAD_INIT("comd27",	"ssp0",	74,	4, "function",	"up", 1,	0),
	PAD_INIT("comd33",	"ssp0",	80,	5, "function",	"up", 1,	0),
};

struct pad_init_info  sd1_pads_info[] = {
	PAD_INIT("comd35",	"sd1", 	82,	5, "function",	"up", 1,	0),
	PAD_INIT("comd36",	"sd1",	83,	5, "function",	"up", 1,	0),
	PAD_INIT("comd37",	"sd1",	84,	5, "function",	"up", 1,	0),
	PAD_INIT("comd38",	"sd1",	85,	5, "function",	"up", 1,	0),
	PAD_INIT("comd39",	"sd1",	86,	5, "function",	"up", 1,	0),
	PAD_INIT("comd40",	"sd1",	87,	5, "function",	"up", 1,	0),
	PAD_INIT("comd41",	"sd1",	88,	5, "function",	"up", 1,	0),
};

struct pad_init_info  sd2_pads_info[] = {
	PAD_INIT("comd42",	"sd2",	89,	5, "function",	"up", 1,	0),
	PAD_INIT("comd43",	"sd2",	90,	5, "function",	"up", 1,	0),
	PAD_INIT("comd44",	"sd2",	91,	5, "function",	"up", 1,	0),
	PAD_INIT("comd45",	"sd2",	92,	5, "function",	"up", 1,	0),
	PAD_INIT("comd46",	"sd2",	93,	5, "function",	"up", 1,	0),
	PAD_INIT("comd47",	"sd2",	94,	5, "function",	"up", 1,	0),
};

struct pad_init_info  iic0_pads_info[] = {
	PAD_INIT("comd48",	"iic0",	95,	5, "function",	"up", 1,	0),
	PAD_INIT("comd49",	"iic0",	96,	6, "function",	"up", 1,	0),
};

struct pad_init_info  iic1_pads_info[] = {
	PAD_INIT("comd50",	"iic1",	97,	6, "function",	"up", 1,	0),
	PAD_INIT("comd51",	"iic1",	98,	6, "function",	"up", 1,	0),
};

struct pad_init_info  uart0_pads_info[] = {
	PAD_INIT("comd52",	"uart0",99,	6, "function",	"up", 1,	0),
	PAD_INIT("comd53",	"uart0",100,	6, "function",	"up", 1,	0),
	PAD_INIT("comd54",	"uart0",101,	6, "function",	"up", 1,	0),
	PAD_INIT("comd55",	"uart0",102,	6, "function",	"up", 1,	0),
};

struct pad_init_info  uart3_pads_info[] = {
	PAD_INIT("comd56",	"uart3",103,	6, "function",	"up", 1,	0),
	PAD_INIT("comd57",	"uart3",104,	6, "function",	"up", 1,	0),
};

struct pad_init_info  uart2_pads_info[] = {
	PAD_INIT("comd58",	"uart2",105,	6, "function",	"up", 1,	0),
	PAD_INIT("comd59",	"uart2",106,	6, "function",	"up", 1,	0),
};

struct pad_init_info  pwm0_pads_info[] = {
	PAD_INIT("comd60",	"pwm0",	107,	6, "function",	"down", 1,	0),
};

struct pad_init_info  pwm1_pads_info[] = {
	PAD_INIT("comd61",	"pwm1",	108,	6, "function",	"down", 1,	0),
};

struct pad_init_info  jtag_pads_info[] = {
	PAD_INIT("comd62",	"debug",109,	6, "function",	"up", 1,	0),
	PAD_INIT("comd63",	"debug",110,	6, "function",	"up", 1,	0),
	PAD_INIT("comd64",	"debug",111,	6, "function",	"up", 1,	0),
	PAD_INIT("comd65",	"debug",112,	7, "function",	"up", 1,	0),
};

struct pad_init_info  xextclk_pads_info[] = {
	PAD_INIT("comd66",	"xextclk",113,	7, "function",	"up", 1,	0),
};

struct pad_init_info  sysoclk1_pads_info[] = {
	PAD_INIT("comd67",	"sysoclk1",	114,	7, "function",	"down", 1,	0),
};

struct pad_init_info  xom_pads_info[] = {
	PAD_INIT("comd68",	"xom",	115,	7, "function",	"float",0,	0),
};

struct pad_init_info  iic2_pads_info[] = {
	PAD_INIT("comd69",	"iic2",	116,	7, "function",	"up", 1,	0),
	PAD_INIT("comd70",	"iic2",	117,	7, "function",	"up", 1,	0),
};

struct pad_init_info  ir_pads_info[] = {
	PAD_INIT("comd71",	"ir",	118,	7, "function",	"up", 1,	0),
};

struct pad_init_info  pwm2_pads_info[] = {
	PAD_INIT("comd73",	"pwm2",	120,	7, "function",	"down", 1,	0),
};

struct pad_init_info  pwm3_pads_info[] = {
	PAD_INIT("comd74",	"pwm3",	121,	7, "function",	"down", 1,	0),
};


struct module_mux_info  imapx_module_mux_info[] = {
	MODULE_MUX("cam",	0,	0),
	MODULE_MUX("camif",	0,	0),
	MODULE_MUX("uart0-mux",	0,	1),
	MODULE_MUX("phy",	1,	0),
	MODULE_MUX("sd2-mux",	1,	1),
	MODULE_MUX("sd0",	2,	0),
	MODULE_MUX("ssp0",	2,	1),
	MODULE_MUX("rgb0",	3,	0),
	MODULE_MUX("srgb",	3,	0),
	MODULE_MUX("bt656",	3,	0),
	MODULE_MUX("phy-mux",	3,	1),
};

struct module_pads_info imapx_module_pads_info[] = {
	MODULE_PADS("rgb0", 		rgb0_pads_info,		ARRAY_SIZE(rgb0_pads_info)),
	MODULE_PADS("srgb", 		srgb_pads_info,		ARRAY_SIZE(srgb_pads_info)),
	MODULE_PADS("bt656", 		bt656_pads_info,	ARRAY_SIZE(bt656_pads_info)),
	MODULE_PADS("cam", 		cam_pads_info,		ARRAY_SIZE(cam_pads_info)),
	MODULE_PADS("camif", 		camif_pads_info,	ARRAY_SIZE(camif_pads_info)),
	MODULE_PADS("uart0-mux",	uart0mux_pads_info,	ARRAY_SIZE(uart0mux_pads_info)),
	MODULE_PADS("iic0", 		iic0_pads_info,		ARRAY_SIZE(iic0_pads_info)),
	MODULE_PADS("iic1", 		iic1_pads_info,		ARRAY_SIZE(iic1_pads_info)),
	MODULE_PADS("iic2", 		iic2_pads_info,		ARRAY_SIZE(iic2_pads_info)),
	MODULE_PADS("iic3", 		iic3_pads_info,		ARRAY_SIZE(iic3_pads_info)),
	MODULE_PADS("iis0", 		iis0_pads_info,		ARRAY_SIZE(iis0_pads_info)),
	MODULE_PADS("ssp1", 		ssp1_pads_info,		ARRAY_SIZE(ssp1_pads_info)),
	MODULE_PADS("sd0", 		sd0_pads_info,		ARRAY_SIZE(sd0_pads_info)),
	MODULE_PADS("ssp0", 		ssp0mux_pads_info,	ARRAY_SIZE(ssp0mux_pads_info)),
	MODULE_PADS("sd1", 		sd1_pads_info,		ARRAY_SIZE(sd1_pads_info)),
	MODULE_PADS("sd2", 		sd2_pads_info,		ARRAY_SIZE(sd2_pads_info)),
	MODULE_PADS("pwm0", 		pwm0_pads_info,		ARRAY_SIZE(pwm0_pads_info)),
	MODULE_PADS("pwm1", 		pwm1_pads_info,		ARRAY_SIZE(pwm1_pads_info)),
	MODULE_PADS("pwm2", 		pwm2_pads_info,		ARRAY_SIZE(pwm2_pads_info)),
	MODULE_PADS("pwm3", 		pwm3_pads_info,		ARRAY_SIZE(pwm3_pads_info)),
	MODULE_PADS("uart0", 		uart0_pads_info,	ARRAY_SIZE(uart0_pads_info)),
	MODULE_PADS("uart2", 		uart2_pads_info,	ARRAY_SIZE(uart2_pads_info)),
	MODULE_PADS("uart3", 		uart3_pads_info,	ARRAY_SIZE(uart3_pads_info)),
	MODULE_PADS("phy", 		phy_pads_info,		ARRAY_SIZE(phy_pads_info)),
	MODULE_PADS("phy-mux", 		phymux_pads_info,	ARRAY_SIZE(phymux_pads_info)),
	MODULE_PADS("xextclk", 		xextclk_pads_info,	ARRAY_SIZE(xextclk_pads_info)),
	MODULE_PADS("sysoclk1", 	sysoclk1_pads_info,	ARRAY_SIZE(sysoclk1_pads_info)),
	MODULE_PADS("xom", 		xom_pads_info,		ARRAY_SIZE(xom_pads_info)),
	MODULE_PADS("ir", 		ir_pads_info,		ARRAY_SIZE(ir_pads_info)),
	MODULE_PADS("jtag", 		jtag_pads_info,		ARRAY_SIZE(jtag_pads_info)),
	MODULE_PADS("pcm0", 		pcm0_pads_info,		ARRAY_SIZE(pcm0_pads_info)),
}; 


static int ngpio = 125;

#endif
