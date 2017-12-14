#ifndef __PAD_INFO_H__
#define __PAD_INFO_H__

#include <mach/pad.h>
	//PAD_INIT(_name, _owner, _index, _bank, _mode,  _pull, _pull_en, _level)
struct pad_init_info  rgb0_pads_info[] = {
	PAD_INIT("disp0",	"rgb0",	0,	0, "function",	"down", 1,  0),
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

struct pad_init_info cam2_pads_info[] = {
	PAD_INIT("disp0",	"cam2",	0,	0, "function",	"down", 1, 	0),
	PAD_INIT("disp1",	"cam2",	1,	0, "function",	"down", 1,	0),
	PAD_INIT("disp2",	"cam2",	2,	0, "function",	"down", 1,	0),
	PAD_INIT("disp3",	"cam2",	3,	0, "function",	"down", 1,	0),
	PAD_INIT("disp4",	"cam2",	4,	0, "function",	"down", 1,	0),
	PAD_INIT("disp5",	"cam2",	5,	0, "function",	"down", 1,	0),
	PAD_INIT("disp6",	"cam2",	6,	0, "function",	"down", 1,	0),
	PAD_INIT("disp7",	"cam2",	7,	0, "function",	"down", 1,	0),
	PAD_INIT("disp8",	"cam2",	8,	0, "function",	"down", 1,	0),
	PAD_INIT("disp9",	"cam2",	9,	0, "function",	"down", 1,	0),
	PAD_INIT("disp10",	"cam2",	10,	0, "function",	"down", 1,	0),
	PAD_INIT("disp11",	"cam2",	11,	0, "function",	"down", 1,	0),
	PAD_INIT("disp12",	"cam2",	12,	0, "function",	"down", 1,	0),
	PAD_INIT("disp13",	"cam2",	13,	0, "function",	"down", 1,	0),
};

struct pad_init_info cam3_pads_info[] = {
	PAD_INIT("disp14",	"cam3",	14,	0, "function",	"down", 1,	0),
	PAD_INIT("disp15",	"cam3",	15,	0, "function",	"down", 1,	0),
	PAD_INIT("disp16",	"cam3",	16,	1, "function",	"down", 1,	0),
	PAD_INIT("disp17",	"cam3",	17,	1, "function",	"down", 1,	0),
	PAD_INIT("disp18",	"cam3",	18,	1, "function",	"down", 1,	0),
	PAD_INIT("disp19",	"cam3",	19,	1, "function",	"down", 1,	0),
	PAD_INIT("disp20",	"cam3",	20,	1, "function",	"down", 1,	0),
	PAD_INIT("disp21",	"cam3",	21,	1, "function",	"down", 1,	0),
	PAD_INIT("disp22",	"cam3",	22,	1, "function",	"down", 1,	0),
	PAD_INIT("disp23",	"cam3",	23,	1, "function",	"down", 1,	0),
	PAD_INIT("disp24",	"cam3",	24,	1, "function",	"down", 1,	0),
	PAD_INIT("disp25",	"cam3",	25,	1, "function",	"down", 1,	0),
	PAD_INIT("disp26",	"cam3",	26,	1, "function",	"down", 1,	0),
	PAD_INIT("disp27",	"cam3",	27,	1, "function",	"down", 1,	0),
};

struct pad_init_info  cam_pads_info[] = {
	PAD_INIT("camd0",	"cam",	28,	1, "function",	"up", 1,	0),
	PAD_INIT("camd1",	"cam",	29,	1, "function",	"up", 1,	0),
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
	PAD_INIT("camd16",	"cam",	44,	2, "function",	"down", 1,	0),
	PAD_INIT("camd17",	"cam",	45,	2, "function",	"down", 1,	0),
	PAD_INIT("camd18",	"cam",	46,	2, "function",	"down", 1,	0),
	PAD_INIT("camd19",	"cam",	47,	2, "function",	"down", 1,	0),
	PAD_INIT("camd20",	"cam",	48,	3, "function",	"up", 1,	0),
};

struct pad_init_info iic4_pads_info[] = {
	PAD_INIT("camd0",	"iic4",	28,	1, "function",	"up", 1,	0),
	PAD_INIT("camd1",	"iic4",	29,	1, "function",	"up", 1,	0),
};

struct pad_init_info  iic2_pads_info[] = {
	PAD_INIT("camd12",	"iic2",	40,	2, "function",	"down", 1,	0),
	PAD_INIT("camd13",	"iic2",	41,	2, "function",	"down", 1,	0),
};

struct pad_init_info  iic5_pads_info[] = {
	PAD_INIT("camd14",	"iic5",	42,	2, "function",	"down", 1,	0),
	PAD_INIT("camd15",	"iic5",	43,	2, "function",	"down", 1,	0),
};

struct pad_init_info  iic3_pads_info[] = {
	PAD_INIT("camd21",	"iic3",	49,	3, "function",	"up", 1,	0),
	PAD_INIT("camd22",	"iic3",	50,	3, "function",	"up", 1,	0),
};

struct pad_init_info  iis0_pads_info[] = {
	PAD_INIT("comd0",	"iis0",	51,	3, "function",	"up", 1,	0),
	PAD_INIT("comd1",	"iis0",	52,	3, "function",	"up", 1,	0),
	PAD_INIT("comd2",	"iis0",	53,	3, "function",	"up", 1,	0),
	PAD_INIT("comd3",	"iis0",	54,	3, "function",	"up", 1,	0),
	PAD_INIT("comd4",	"iis0", 55,	3, "function",	"up", 1,	0),
};

struct pad_init_info  pcm0mux_pads_info[] = {
	PAD_INIT("comd0",	"pcm0-mux",	51,	3, "function",	"up", 1,	0),
	PAD_INIT("comd1",	"pcm0-mux",	52,	3, "function",	"up", 1,	0),
	PAD_INIT("comd2",	"pcm0-mux",	53,	3, "function",	"up", 1,	0),
	PAD_INIT("comd3",	"pcm0-mux",	54,	3, "function",	"up", 1,	0),
	PAD_INIT("comd4",	"pcm0-mux", 55,	3, "function",	"up", 1,	0),
};

struct pad_init_info  pcm0_pads_info[] = {
	PAD_INIT("comd5",	"pcm0", 56,	3, "function",	"down", 1,	0),
	PAD_INIT("comd6",	"pcm0", 57,	3, "function",	"down", 1,	0),
	PAD_INIT("comd7",	"pcm0", 58,	3, "function",	"down", 1,	0),
	PAD_INIT("comd8",	"pcm0", 59,	3, "function",	"down", 1,	0),
	PAD_INIT("comd9",	"pcm0", 60,	3, "function",	"down", 1,	0),
};

struct pad_init_info  srgb_pads_info[] = {
	PAD_INIT("comd5",	"srgb", 56,	3, "function",	"down", 1,	0),
	PAD_INIT("comd6",	"srgb", 57,	3, "function",	"down", 1,	0),
	PAD_INIT("comd7",	"srgb", 58,	3, "function",	"down", 1,	0),
	PAD_INIT("comd8",	"srgb", 59,	3, "function",	"down", 1,	0),
	PAD_INIT("comd9",	"srgb", 60,	3, "function",	"down", 1,	0),
	PAD_INIT("comd28",	"srgb", 79,	4, "function",	"up", 1,	0),
	PAD_INIT("comd29",	"srgb", 80,	5, "function",	"up", 1,	0),
	PAD_INIT("comd30",	"srgb", 81,	5, "function",	"up", 1,	0),
	PAD_INIT("comd31",	"srgb", 82,	5, "function",	"up", 1,	0),
	PAD_INIT("comd34",	"srgb", 85,	5, "function",	"up", 1,	0),
	PAD_INIT("comd35",	"srgb", 86,	5, "function",	"up", 1,	0),
	PAD_INIT("comd44",	"srgb",	91,	5, "function",	"down", 1,	0),
};

struct pad_init_info  ssp1_pads_info[] = {
	PAD_INIT("comd10",	"ssp1",	61,	3, "function",	"up", 1,	0),
	PAD_INIT("comd11",	"ssp1",	62,	3, "function",	"up", 1,	0),
	PAD_INIT("comd12",	"ssp1",	63,	3, "function",	"up", 1,	0),
	PAD_INIT("comd13",	"ssp1", 64,	4, "function",	"up", 1,	0),
};

struct pad_init_info  sd0_pads_info[] = {
	PAD_INIT("comd14",	"sd0",	65,	4, "function",	"up", 1,	0),
	PAD_INIT("comd15",	"sd0",	66,	4, "function",	"up", 1,	0),
	PAD_INIT("comd16",	"sd0",	67,	4, "function",	"up", 1,	0),
	PAD_INIT("comd17",	"sd0",	68,	4, "function",	"up", 1,	0),
	PAD_INIT("comd18",	"sd0",	69,	4, "function",	"up", 1,	0),
	PAD_INIT("comd19",	"sd0",	70,	4, "function",	"up", 1,	0),
	PAD_INIT("comd20",	"sd0",	71,	4, "function",	"up", 1,	0),
	PAD_INIT("comd21",	"sd0",	72,	4, "function",	"up", 1,	0),
	PAD_INIT("comd22",	"sd0",	73,	4, "function",	"up", 1,	0),
	PAD_INIT("comd23",	"sd0",	74,	4, "function",	"up", 1,	0),
	PAD_INIT("comd24",	"sd0",	75,	4, "function",	"up", 1,	0),
	PAD_INIT("comd25",	"sd0",	76,	4, "function",	"up", 1,	0),
};

struct pad_init_info  phy_pads_info[] = {
	PAD_INIT("comd15",	"phy",	66,	4, "function",	"up", 1,	0),
	PAD_INIT("comd16",	"phy",	67,	4, "function",	"up", 1,	0),
	PAD_INIT("comd17",	"phy",	68,	4, "function",	"up", 1,	0),
	PAD_INIT("comd18",	"phy",	69,	4, "function",	"up", 1,	0),
	PAD_INIT("comd19",	"phy",	70,	4, "function",	"up", 1,	0),
	PAD_INIT("comd20",	"phy",	71,	4, "function",	"up", 1,	0),
	PAD_INIT("comd21",	"phy",	72,	4, "function",	"up", 1,	0),
	PAD_INIT("comd22",	"phy",	73,	4, "function",	"up", 1,	0),
	PAD_INIT("comd25",	"phy",	76,	4, "function",	"up", 1,	0),
};

struct pad_init_info  sysoclk1mux_pads_info[] = {
        PAD_INIT("comd24",	"sysoclk1-mux",	75,	4, "function",	"up", 1,	0),
};

struct pad_init_info  iic1_pads_info[] = {
	PAD_INIT("comd26",	"iic1",	77,	4, "function",	"up", 1,	0),
	PAD_INIT("comd27",	"iic1",	78,	4, "function",	"up", 1,	0),
};

struct pad_init_info  sd1_pads_info[] = {
};

struct pad_init_info  sd2_pads_info[] = {
};

struct pad_init_info  uart0_pads_info[] = {
	PAD_INIT("comd28",	"uart0",79,	4, "function",	"up", 1,	0),
	PAD_INIT("comd29",	"uart0",80,	5, "function",	"up", 1,	0),
	PAD_INIT("comd30",	"uart0",81,	5, "function",	"up", 1,	0),
	PAD_INIT("comd31",	"uart0",82,	5, "function",	"up", 1,	0),
};

struct pad_init_info  uart3_pads_info[] = {
	PAD_INIT("comd32",	"uart3",83,	5, "function",	"up", 1,	0),
	PAD_INIT("comd33",	"uart3",84,	5, "function",	"up", 1,	0),
};

struct pad_init_info  uart2_pads_info[] = {
	PAD_INIT("comd34",	"uart2",85,	5, "function",	"up", 1,	0),
	PAD_INIT("comd35",	"uart2",86,	5, "function",	"up", 1,	0),
};

struct pad_init_info  pwm0_pads_info[] = {
	PAD_INIT("comd36",	"pwm0",	87,	5, "function",	"down", 1,	0),
};

struct pad_init_info  pwm1_pads_info[] = {
	PAD_INIT("comd37",	"pwm1",	88,	5, "function",	"down", 1,	0),
};

struct pad_init_info  jtag_pads_info[] = {
};

struct pad_init_info  xextclk_pads_info[] = {
	PAD_INIT("comd42",	"xextclk",89,	5, "function",	"up", 1,	0),
};

struct pad_init_info  sysoclk1_pads_info[] = {
	PAD_INIT("comd43",	"sysoclk1",	90,	5, "function",	"down", 1,	0),
};

struct pad_init_info  pwm2_pads_info[] = {
	PAD_INIT("comd44",	"pwm2",	91,	5, "function",	"down", 1,	0),
};

struct pad_init_info  iic0_pads_info[] = {
	PAD_INIT("comd45",	"iic0",	92,	5, "function",	"up", 1,	0),
	PAD_INIT("comd46",	"iic0",	93,	5, "function",	"up", 1,	0),
};

struct pad_init_info  ir_pads_info[] = {
	PAD_INIT("comd47",	"ir",	94,	5, "function",	"up", 1,	0),
};

struct pad_init_info  pwm3_pads_info[] = {
	PAD_INIT("comd48",	"pwm3",	95,	5, "function",	"down", 1,	0),
};

struct pad_init_info cam1_pads_info[] = {
	PAD_INIT("camd23",	"cam1",	96,	  6, "function",	"down", 1,  0),
	PAD_INIT("camd24",	"cam1",	97,	  6, "function",	"down", 1,	0),
	PAD_INIT("camd25",	"cam1",	98,	  6, "function",	"down", 1,	0),
	PAD_INIT("camd26",	"cam1",	99,	  6, "function",	"down", 1,	0),
	PAD_INIT("camd27",	"cam1",	100,  6, "function",	"down", 1,	0),
	PAD_INIT("camd28",	"cam1",	101,  6, "function",	"down", 1,	0),
	PAD_INIT("camd29",	"cam1",	102,  6, "function",	"down", 1,	0),
	PAD_INIT("camd30",	"cam1",	103,  6, "function",	"down", 1,	0),
	PAD_INIT("camd31",	"cam1",	104,  6, "function",	"down", 1,	0),
	PAD_INIT("camd32",	"cam1",	105,  6, "function",	"down", 1,	0),
	PAD_INIT("camd33",	"cam1",	106,  6, "function",	"down", 1,	0),
	PAD_INIT("camd34",	"cam1",	107,  6, "function",	"down", 1,	0),
	PAD_INIT("camd35",	"cam1",	108,  6, "function",	"down", 1,	0),
	PAD_INIT("camd36",	"cam1",	109,  6, "function",	"down", 1,	0),
	PAD_INIT("camd37",	"cam1",	110,  6, "function",	"up", 1,	0),
};

struct pad_init_info  xom_pads_info[] = {
};

struct pad_init_info pwm4_pads_info[] = {
};

struct pad_init_info pwm5_pads_info[] = {
};

struct pad_init_info pwm6_pads_info[] = {
};

struct pad_init_info pwm7_pads_info[] = {
};

struct module_mux_info  imapx_module_mux_info[] = {
#if 0
	MODULE_MUX("rgb0",	3,	0),
	MODULE_MUX("cam2",	0,	0),
	MODULE_MUX("cam3",	0,	0),
	MODULE_MUX("cam",	0,	0),
	MODULE_MUX("iic4",	1,	0),
	MODULE_MUX("iic2",	1,	0),
	MODULE_MUX("iic5",	1,	0),
	MODULE_MUX("iis0",	1,	0),
	MODULE_MUX("pcm0-mux",	1,	0),
	MODULE_MUX("pcm0",	2,	0),
    MODULE_MUX("uart0",	2,	0),
    MODULE_MUX("uart2",	2,	0),
    MODULE_MUX("pwm2",	2,	0),
	MODULE_MUX("srgb",	3,	0),
	MODULE_MUX("sd0",	3,	0),
	MODULE_MUX("phy",	1,	0),
	MODULE_MUX("sysoclk1-mux",	1,	0),
#endif
};


struct module_pads_info imapx_module_pads_info[] = {
	MODULE_PADS("rgb0", 		rgb0_pads_info,		ARRAY_SIZE(rgb0_pads_info)),
	MODULE_PADS("srgb", 		srgb_pads_info,		ARRAY_SIZE(srgb_pads_info)),
	MODULE_PADS("cam", 		cam_pads_info,		ARRAY_SIZE(cam_pads_info)),
	MODULE_PADS("iic0", 		iic0_pads_info,		ARRAY_SIZE(iic0_pads_info)),
	MODULE_PADS("iic1", 		iic1_pads_info,		ARRAY_SIZE(iic1_pads_info)),
	MODULE_PADS("iic2", 		iic2_pads_info,		ARRAY_SIZE(iic2_pads_info)),
	MODULE_PADS("iic3", 		iic3_pads_info,		ARRAY_SIZE(iic3_pads_info)),
	MODULE_PADS("iic4", 		iic4_pads_info,		ARRAY_SIZE(iic4_pads_info)),
	MODULE_PADS("iic5", 		iic5_pads_info,		ARRAY_SIZE(iic5_pads_info)),
	MODULE_PADS("iis0", 		iis0_pads_info,		ARRAY_SIZE(iis0_pads_info)),
	MODULE_PADS("pcm0-mux", 	pcm0mux_pads_info,	ARRAY_SIZE(pcm0mux_pads_info)),
	MODULE_PADS("ssp1", 		ssp1_pads_info,		ARRAY_SIZE(ssp1_pads_info)),
	MODULE_PADS("sd0", 		sd0_pads_info,		ARRAY_SIZE(sd0_pads_info)),
	MODULE_PADS("sd1", 		sd1_pads_info,		ARRAY_SIZE(sd1_pads_info)),
	MODULE_PADS("sd2", 		sd2_pads_info,		ARRAY_SIZE(sd2_pads_info)),
	MODULE_PADS("pwm0", 		pwm0_pads_info,		ARRAY_SIZE(pwm0_pads_info)),
	MODULE_PADS("pwm1", 		pwm1_pads_info,		ARRAY_SIZE(pwm1_pads_info)),
	MODULE_PADS("pwm2", 		pwm2_pads_info,		ARRAY_SIZE(pwm2_pads_info)),
	MODULE_PADS("pwm3", 		pwm3_pads_info,		ARRAY_SIZE(pwm3_pads_info)),
	MODULE_PADS("pwm4", 		pwm4_pads_info,		ARRAY_SIZE(pwm4_pads_info)),
	MODULE_PADS("pwm5", 		pwm5_pads_info,		ARRAY_SIZE(pwm5_pads_info)),
	MODULE_PADS("pwm6", 		pwm6_pads_info,		ARRAY_SIZE(pwm6_pads_info)),
	MODULE_PADS("pwm7", 		pwm7_pads_info,		ARRAY_SIZE(pwm7_pads_info)),
	MODULE_PADS("uart0", 		uart0_pads_info,	ARRAY_SIZE(uart0_pads_info)),
	MODULE_PADS("uart2", 		uart2_pads_info,	ARRAY_SIZE(uart2_pads_info)),
	MODULE_PADS("uart3", 		uart3_pads_info,	ARRAY_SIZE(uart3_pads_info)),
	MODULE_PADS("phy", 		phy_pads_info,		ARRAY_SIZE(phy_pads_info)),
	MODULE_PADS("xextclk", 		xextclk_pads_info,	ARRAY_SIZE(xextclk_pads_info)),
	MODULE_PADS("sysoclk1", 	sysoclk1_pads_info,	ARRAY_SIZE(sysoclk1_pads_info)),
	MODULE_PADS("sysoclk1-mux", 	sysoclk1mux_pads_info,	ARRAY_SIZE(sysoclk1mux_pads_info)),
	MODULE_PADS("xom", 		xom_pads_info,		ARRAY_SIZE(xom_pads_info)),
	MODULE_PADS("ir", 		ir_pads_info,		ARRAY_SIZE(ir_pads_info)),
	MODULE_PADS("jtag", 		jtag_pads_info,		ARRAY_SIZE(jtag_pads_info)),
	MODULE_PADS("pcm0", 		pcm0_pads_info,		ARRAY_SIZE(pcm0_pads_info)),
	MODULE_PADS("cam1", 		cam1_pads_info,		ARRAY_SIZE(cam1_pads_info)),
	MODULE_PADS("cam2", 		cam2_pads_info,		ARRAY_SIZE(cam2_pads_info)),
	MODULE_PADS("cam3", 		cam3_pads_info,		ARRAY_SIZE(cam3_pads_info)),
}; 


static int ngpio = 111;

#endif
