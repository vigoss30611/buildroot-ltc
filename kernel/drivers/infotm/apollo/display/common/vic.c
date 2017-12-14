/*
 * vic.c - display VIC
 *
 * Copyright (c) 2013 William Smith <terminalnt@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include "dss_common.h"

#define DSS_LAYER	"[common]"
#define DSS_SUB1	"[vic]"
#define DSS_SUB2	""



extern int imap_lcd_timing_fill(dtd_t *dtd, int vic);


int vic_fill(dtd_t *dtd, u32 code, u32 refreshRate)
{
	int ret;
	dss_trace("vic %d\n", code);
	
	dtd->mCode = code;
	dtd->mHBorder = 0;
	dtd->mVBorder = 0;
	dtd->mPixelRepetitionInput = 0;
	dtd->mHImageSize = 16;
	dtd->mVImageSize = 9;

	switch (code)
	{
	case 1: /* 640x480p @ 59.94/60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
		dtd->mHActive = 640;
		dtd->mVActive = 480;
		dtd->mHBlanking = 160;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 10;
		dtd->mHSyncPulseWidth = 96;
		dtd->mVSyncPulseWidth = 2;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0; /* not(progressive_nI) */
		dtd->mPixelClock = (refreshRate == 59940) ? 2517 : 2520;
		break;
	case 2: /* 720x480p @ 59.94/60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 3: /* 720x480p @ 59.94/60Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 480;
		dtd->mHBlanking = 138;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 9;
		dtd->mHSyncPulseWidth = 62;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		//dtd->mPixelClock = (refreshRate == 59940) ? 2700 : 2702;
		dtd->mPixelClock = 2700;
		break;
	case 4: /* 1280x720p @ 59.94/60Hz 16:9 */
		dtd->mHActive = 1280;
		dtd->mVActive = 720;
		dtd->mHBlanking = 370;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 110;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 59940) ? 7417 : 7425;
		break;
	case 5: /* 1920x1080i @ 59.94/60Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 540;
		dtd->mHBlanking = 280;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 88;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = (refreshRate == 59940) ? 7417 : 7425;
		break;
	case 6: /* 720(1440)x480i @ 59.94/60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 7: /* 720(1440)x480i @ 59.94/60Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 240;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 38;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = (refreshRate == 59940) ? 2700 : 2702;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 8: /* 720(1440)x240p @ 59.826/60.054/59.886/60.115Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 9: /* 720(1440)x240p @ 59.826/60.054/59.886/60.115Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 240;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = (refreshRate > 60000) ? 22 : 23;
		dtd->mHSyncOffset = 38;
		dtd->mVSyncOffset = (refreshRate > 60000) ? 4 : 5;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock
				= ((refreshRate == 60054) || refreshRate == 59826) ? 2700
						: 2702; /*  else 60.115/59.886 Hz */
		dtd->mPixelRepetitionInput = 1;
		break;
	case 10: /* 2880x480i @ 59.94/60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 11: /* 2880x480i @ 59.94/60Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 240;
		dtd->mHBlanking = 552;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 76;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 248;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = (refreshRate == 59940) ? 5400 : 5405;
		break;
	case 12: /* 2880x240p @ 59.826/60.054/59.886/60.115Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 13: /* 2880x240p @ 59.826/60.054/59.886/60.115Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 240;
		dtd->mHBlanking = 552;
		dtd->mVBlanking = (refreshRate > 60000) ? 22 : 23;
		dtd->mHSyncOffset = 76;
		dtd->mVSyncOffset = (refreshRate > 60000) ? 4 : 5;
		dtd->mHSyncPulseWidth = 248;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock
				= ((refreshRate == 60054) || refreshRate == 59826) ? 5400
						: 5405; /*  else 60.115/59.886 Hz */
		break;
	case 14: /* 1440x480p @ 59.94/60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 15: /* 1440x480p @ 59.94/60Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 480;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 32;
		dtd->mVSyncOffset = 9;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 59940) ? 5400 : 5405;
		break;
	case 16: /* 1920x1080p @ 59.94/60Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 280;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 88;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 59940) ? 14835 : 14850;
		break;
	case 17: /* 720x576p @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 18: /* 720x576p @ 50Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 576;
		dtd->mHBlanking = 144;
		dtd->mVBlanking = 49;
		dtd->mHSyncOffset = 12;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 64;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 2700;
		break;
	case 19: /* 1280x720p @ 50Hz 16:9 */
		dtd->mHActive = 1280;
		dtd->mVActive = 720;
		dtd->mHBlanking = 700;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 440;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 7425;
		break;
	case 20: /* 1920x1080i @ 50Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 540;
		dtd->mHBlanking = 720;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 528;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 7425;
		break;
	case 21: /* 720(1440)x576i @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 22: /* 720(1440)x576i @ 50Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 288;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 2700;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 23: /* 720(1440)x288p @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 24: /* 720(1440)x288p @ 50Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 288;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = (refreshRate == 50080) ? 24
				: ((refreshRate == 49920) ? 25 : 26);
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = (refreshRate == 50080) ? 2
				: ((refreshRate == 49920) ? 3 : 4);
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 2700;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 25: /* 2880x576i @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 26: /* 2880x576i @ 50Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 288;
		dtd->mHBlanking = 576;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 252;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 5400;
		break;
	case 27: /* 2880x288p @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 28: /* 2880x288p @ 50Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 288;
		dtd->mHBlanking = 576;
		dtd->mVBlanking = (refreshRate == 50080) ? 24
				: ((refreshRate == 49920) ? 25 : 26);
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = (refreshRate == 50080) ? 2
				: ((refreshRate == 49920) ? 3 : 4);
		dtd->mHSyncPulseWidth = 252;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 5400;
		break;
	case 29: /* 1440x576p @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 30: /* 1440x576p @ 50Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 576;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 49;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 128;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 5400;
		break;
	case 31: /* 1920x1080p @ 50Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 720;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 528;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 14850;
		break;
	case 32: /* 1920x1080p @ 23.976/24Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 830;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 638;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 23976) ? 7417 : 7425;
		break;
	case 33: /* 1920x1080p @ 25Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 720;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 528;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 7425;
		break;
	case 34: /* 1920x1080p @ 29.97/30Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 280;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 88;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 29970) ? 7417 : 7425;
		break;
	case 35: /* 2880x480p @ 60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 36: /* 2880x480p @ 60Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 480;
		dtd->mHBlanking = 552;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 9;
		dtd->mHSyncPulseWidth = 248;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 59940) ? 10800 : 10810;
		break;
	case 37: /* 2880x576p @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 38: /* 2880x576p @ 50Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 576;
		dtd->mHBlanking = 576;
		dtd->mVBlanking = 49;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 256;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 10800;
		break;
	case 39: /* 1920x1080i (1250 total) @ 50Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 540;
		dtd->mHBlanking = 384;
		dtd->mVBlanking = 85;
		dtd->mHSyncOffset = 32;
		dtd->mVSyncOffset = 23;
		dtd->mHSyncPulseWidth = 168;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 7200;
		break;
	case 40: /* 1920x1080i @ 100Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 540;
		dtd->mHBlanking = 720;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 528;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 14850;
		break;
	case 41: /* 1280x720p @ 100Hz 16:9 */
		dtd->mHActive = 1280;
		dtd->mVActive = 720;
		dtd->mHBlanking = 700;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 440;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 14850;
		break;
	case 42: /* 720x576p @ 100Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 43: /* 720x576p @ 100Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 576;
		dtd->mHBlanking = 144;
		dtd->mVBlanking = 49;
		dtd->mHSyncOffset = 12;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 64;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 5400;
		break;
	case 44: /* 720(1440)x576i @ 100Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 45: /* 720(1440)x576i @ 100Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 288;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 5400;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 46: /* 1920x1080i @ 119.88/120Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 540;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 88;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = (refreshRate == 119880) ? 14835 : 14850;
		break;
	case 47: /* 1280x720p @ 119.88/120Hz 16:9 */
		dtd->mHActive = 1280;
		dtd->mVActive = 720;
		dtd->mHBlanking = 370;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 110;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 119880) ? 14835 : 14850;
		break;
	case 48: /* 720x480p @ 119.88/120Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 49: /* 720x480p @ 119.88/120Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 480;
		dtd->mHBlanking = 138;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 9;
		dtd->mHSyncPulseWidth = 62;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 119880) ? 5400 : 5405;
		break;
	case 50: /* 720(1440)x480i @ 119.88/120Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 51: /* 720(1440)x480i @ 119.88/120Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 240;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 38;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = (refreshRate == 119880) ? 5400 : 5405;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 52: /* 720X576p @ 200Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 53: /* 720X576p @ 200Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 576;
		dtd->mHBlanking = 144;
		dtd->mVBlanking = 49;
		dtd->mHSyncOffset = 12;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 64;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 10800;
		break;
	case 54: /* 720(1440)x576i @ 200Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 55: /* 720(1440)x576i @ 200Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 288;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 10800;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 56: /* 720x480p @ 239.76/240Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 57: /* 720x480p @ 239.76/240Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 480;
		dtd->mHBlanking = 138;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 9;
		dtd->mHSyncPulseWidth = 62;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = (refreshRate == 239760) ? 10800 : 10810;
		break;
	case 58: /* 720(1440)x480i @ 239.76/240Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 59: /* 720(1440)x480i @ 239.76/240Hz 16:9 */
		dtd->mHActive = 1440;
		dtd->mVActive = 240;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 38;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = (refreshRate == 239760) ? 10800 : 10810;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 60: /* 1280x720p @ 24Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 720;
		dtd->mHBlanking = 2020;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 1760;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 5940;
		dss_err("VIC 60, 720P@24Hz, require 59.40MHz pixel clock. not supported .\n");
		break;
	case 61: /* 1280x720p @ 25Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 720;
		dtd->mHBlanking = 2680;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 2420;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 7425;
		break;
	case 62:	/* 1280x720p @ 30Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 720;
		dtd->mHBlanking = 2020;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 1760;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 7425;
		break;
	/* extened resolution format */
	case 1001:	/* 3840x2160 @30Hz */
		dtd->mHActive = 3840;
		dtd->mVActive = 2160;
		dtd->mHBlanking = 560;
		dtd->mVBlanking = 90;
		dtd->mHSyncOffset = 176;
		dtd->mVSyncOffset = 8;
		dtd->mHSyncPulseWidth = 88;
		dtd->mVSyncPulseWidth = 10;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 29700;
		break;
	case 1002:	/* 3840x2160 @25Hz */
		dtd->mHActive = 3840;
		dtd->mVActive = 2160;
		dtd->mHBlanking = 1440;
		dtd->mVBlanking = 90;
		dtd->mHSyncOffset = 1056;
		dtd->mVSyncOffset = 8;
		dtd->mHSyncPulseWidth = 88;
		dtd->mVSyncPulseWidth = 10;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 29700;
		break;
	case 1003:	/* 3840x2160 @24Hz */
		dtd->mHActive = 3840;
		dtd->mVActive = 2160;
		dtd->mHBlanking = 1660;
		dtd->mVBlanking = 90;
		dtd->mHSyncOffset = 1276;
		dtd->mVSyncOffset = 8;
		dtd->mHSyncPulseWidth = 88;
		dtd->mVSyncPulseWidth = 10;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 29700;
		break;
	case 1004:	/* 4096x2160 @24Hz (SMPTE) */
		dtd->mHActive = 3840;
		dtd->mVActive = 2160;
		dtd->mHBlanking = 1404;
		dtd->mVBlanking = 90;
		dtd->mHSyncOffset = 1020;
		dtd->mVSyncOffset = 8;
		dtd->mHSyncPulseWidth = 88;
		dtd->mVSyncPulseWidth = 10;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 29700;
		break;
    //fengwu . alex advice 20150514
    case 2001: /* 1920x1080p @ 59.94/60Hz 16:9 */
        dtd->mHActive = 1920;
        dtd->mVActive = 1088;
        dtd->mHBlanking = 280;
        dtd->mVBlanking = 45;
        dtd->mHSyncOffset = 88;
        dtd->mVSyncOffset = 4;
        dtd->mHSyncPulseWidth = 44;
        dtd->mVSyncPulseWidth = 5;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = (refreshRate == 59940) ? 14835 : 14850;
        break;
    case 2003: /* 2176*1116 @ 59.94/60Hz 16:9 */
        dtd->mHActive = 2176;//2240;//2176;
        dtd->mVActive = 1116;
        dtd->mHBlanking = 0;
        dtd->mVBlanking = 0;
        dtd->mHSyncOffset = 0;
        dtd->mVSyncOffset = 0;
        dtd->mHSyncPulseWidth = 0;
        dtd->mVSyncPulseWidth = 0;
        dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
        dtd->mInterlaced = 0;
        dtd->mPixelClock = 7554;
        break;    
	case 2002:
		dtd->mHActive = 360;
		dtd->mVActive = 240;
		dtd->mHBlanking = 242;
		dtd->mVBlanking = 26;
		dtd->mHSyncOffset = 3;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 8;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
//		dtd->mVclkInverse = 1;
		dtd->mPixelClock = (refreshRate == 59940) ? 2700 : 2702;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 2004:
		dtd->mHActive = 1664;
		dtd->mVActive = 736;
		dtd->mHBlanking = 0;
		dtd->mVBlanking = 0;
		dtd->mHSyncOffset = 0;
		dtd->mVSyncOffset = 0;
		dtd->mHSyncPulseWidth = 0;
		dtd->mVSyncPulseWidth = 0;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 3675;//8448;
		break;
	case 2005:	//ota5182a
		dtd->mHActive = 320;
		dtd->mVActive = 240;
		dtd->mHBlanking = 280;
		dtd->mVBlanking = 31;
		dtd->mHSyncOffset = 27;
		dtd->mVSyncOffset = 13;
		dtd->mHSyncPulseWidth = 1;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mVclkInverse = 0;
		dtd->mPixelClock = (refreshRate == 59940) ? 2700 : 2702;
		dtd->mPixelRepetitionInput = 1;
		break;
	default:
		ret = imap_lcd_timing_fill(dtd, code);
		if (ret < 0) {
			dtd->mCode = -1;
			dss_err("invalid vic %d", code);
			return -1;
		}
		break;
	}
	return 0;
}

MODULE_DESCRIPTION("InfoTM iMAP display VIC driver");
MODULE_AUTHOR("William Smith, <terminalnt@gmail.com>");
MODULE_LICENSE("GPL v2");
