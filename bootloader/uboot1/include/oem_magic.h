
#ifndef __OEM_MAGIC_H__
#define __OEM_MAGIC_H__

struct oem_board_para {

	char		maf[16];
	uint32_t	magic;
};

struct oem_board_para infotm_boards[] = {

	{"benign",		0xbb220000},
	{"elonix",		0xeb00c000},
	{"jingdian",	0x11dd0000},
	{"sy0702",		0xeeaa0702},
	{"sy7901l",		0xeeaa7901},
	{"7901i",		0xeef07901},
	{"sy7901w",		0xeeaa7902},
	{"sy1011",		0xeeaa1011},
	{"develop",		0xeef00000},
	{"coby",		0xccbb0000},
	{"zenithink",	0xbeadff00},
	{"wwe8b",		0xdb6db608},
	{"sy9901",		0xeeaa9901},
	{"wwe10",		0xdb6db510},
	{"wwe8",		0xdb6db508},
	{"wwe10b",		0xdb6db610},
};

#endif /* __OEM_MAGIC_H__ */
