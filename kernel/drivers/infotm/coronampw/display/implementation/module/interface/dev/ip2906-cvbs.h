#ifndef _IP2906_H
#define _IP2906_H

#include <mach/ip2906.h>

enum TV_FORMAT
{
	TV_NTSC_M = 0x00, /* 8 */
	TV_NTSC_J = 0x01, /*9 */
	TV_PAL_Nc = 0x02,
	TV_PAL_BGH = 0x03,
	TV_PAL_D= 0x04,
	TV_PAL_I = 0x05,
	TV_PAL_M = 0x06,
	TV_PAL_N = 0x07,
};

int ip2906_cvbs_config(uint8_t colorbar, uint8_t format);
int ip2906_init(void);

#endif