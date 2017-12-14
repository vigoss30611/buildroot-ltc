/* display vic_fill */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/display_device.h>

#include "display_register.h"

#define DISPLAY_MODULE_LOG		"vic"


/* dtd_buffer and dtd_overlay param should be update simultaneously */
static dtd_t dtd_buffer[IDS_PATH_NUM][IDS_OVERLAY_NUM] = {{{0}}};
static dtd_t dtd_overlay[IDS_PATH_NUM][IDS_OVERLAY_NUM] = {{{0}}};
static dtd_t dtd_osd[IDS_PATH_NUM] = {{0}};
static dtd_t dtd_peripheral[IDS_PATH_NUM] = {{0}};


/*
  * Notice: mPixelClock unit is KHz which is different from dtd_fill.
  *				All Interlace modes' mVActive have changed into actual picture height.
  *				All Pixel Repetition Input modes' mHActive have changed into actual picture width.
  *				vic 7 adjusted slightly to fit NTSC timing.
  */
/* @path param is only useful for internal vic */
int vic_fill(int path, dtd_t *dtd, unsigned int code)
{
	//dlog_dbg("code = %d\n", code);

	dtd->mCode = code;
	dtd->mHBorder = 0;
	dtd->mVBorder = 0;
	dtd->mPixelRepetitionInput = 0;
	dtd->mHImageSize = 16;
	dtd->mVImageSize = 9;

	switch (code)
	{
/* CEA-861-E standard timing */
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
		dtd->mPixelClock = 25175;
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
		dtd->mPixelClock = 27000;
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
		dtd->mPixelClock = 74250;
		break;
	case 5: /* 1920x1080i @ 59.94/60Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 280;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 88;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 74250;
		break;
	case 6: /* 720(1440)x480i @ 59.94/60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 7: /* 720(1440)x480i @ 59.94/60Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 480;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 32;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 27000;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 8: /* 720(1440)x240p @ 59.826/60.054/59.886/60.115Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 9: /* 720(1440)x240p @ 59.826/60.054/59.886/60.115Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 480;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 23;
		dtd->mHSyncOffset = 38;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 27000;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 10: /* 2880x480i @ 59.94/60Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 11: /* 2880x480i @ 59.94/60Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 480;
		dtd->mHBlanking = 552;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 76;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 248;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 54000;
		break;
	case 12: /* 2880x240p @ 59.826/60.054/59.886/60.115Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 13: /* 2880x240p @ 59.826/60.054/59.886/60.115Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 240;
		dtd->mHBlanking = 552;
		dtd->mVBlanking = 23;
		dtd->mHSyncOffset = 76;
		dtd->mVSyncOffset = 5;
		dtd->mHSyncPulseWidth = 248;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 54000;
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
		dtd->mPixelClock = 54000;
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
		dtd->mPixelClock = 148500;
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
		dtd->mPixelClock = 27000;
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
		dtd->mPixelClock = 74250;
		break;
	case 20: /* 1920x1080i @ 50Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 720;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 528;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 74250;
		break;
	case 21: /* 720(1440)x576i @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 22: /* 720(1440)x576i @ 50Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 576;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 27000;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 23: /* 720(1440)x288p @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 24: /* 720(1440)x288p @ 50Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 288;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 25;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 27000;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 25: /* 2880x576i @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 26: /* 2880x576i @ 50Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 576;
		dtd->mHBlanking = 576;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 252;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 54000;
		break;
	case 27: /* 2880x288p @ 50Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 28: /* 2880x288p @ 50Hz 16:9 */
		dtd->mHActive = 2880;
		dtd->mVActive = 288;
		dtd->mHBlanking = 576;
		dtd->mVBlanking = 25;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 252;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 54000;
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
		dtd->mPixelClock = 54000;
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
		dtd->mPixelClock = 148500;
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
		dtd->mPixelClock = 74250;
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
		dtd->mPixelClock = 74250;
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
		dtd->mPixelClock = 74250;
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
		dtd->mPixelClock = 108000;
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
		dtd->mPixelClock = 108000;
		break;
	case 39: /* 1920x1080i (1250 total) @ 50Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 384;
		dtd->mVBlanking = 85;
		dtd->mHSyncOffset = 32;
		dtd->mVSyncOffset = 23;
		dtd->mHSyncPulseWidth = 168;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 72000;
		break;
	case 40: /* 1920x1080i @ 100Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 720;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 528;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 148500;
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
		dtd->mPixelClock = 148500;
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
		dtd->mPixelClock = 54000;
		break;
	case 44: /* 720(1440)x576i @ 100Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 45: /* 720(1440)x576i @ 100Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 576;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 54000;
		dtd->mPixelRepetitionInput = 1;
		break;
	case 46: /* 1920x1080i @ 119.88/120Hz 16:9 */
		dtd->mHActive = 1920;
		dtd->mVActive = 1080;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 88;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 44;
		dtd->mVSyncPulseWidth = 5;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 148500;
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
		dtd->mPixelClock = 148500;
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
		dtd->mPixelClock = 54000;
		break;
	case 50: /* 720(1440)x480i @ 119.88/120Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 51: /* 720(1440)x480i @ 119.88/120Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 480;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 38;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 54000;
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
		dtd->mPixelClock = 108000;
		break;
	case 54: /* 720(1440)x576i @ 200Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 55: /* 720(1440)x576i @ 200Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 576;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 126;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 108000;
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
		dtd->mPixelClock = 108000;
		break;
	case 58: /* 720(1440)x480i @ 239.76/240Hz 4:3 */
		dtd->mHImageSize = 4;
		dtd->mVImageSize = 3;
	case 59: /* 720(1440)x480i @ 239.76/240Hz 16:9 */
		dtd->mHActive = 720;
		dtd->mVActive = 480;
		dtd->mHBlanking = 276;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 38;
		dtd->mVSyncOffset = 4;
		dtd->mHSyncPulseWidth = 124;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 108000;
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
		dtd->mPixelClock = 59400;
		dlog_err("VIC 60, 720P@24Hz, require 59.40MHz pixel clock. not supported .\n");
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
		dtd->mPixelClock = 74250;
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
		dtd->mPixelClock = 74250;
		break;

/* HDMI extended 4K*2K resolution. */
/* HDMI_VIC:1~4, Equivalent CEA-861-F VIC 95,94,93,98 */
	case 95:	/* 3840x2160 @30Hz */
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
		dtd->mPixelClock = 297000;
		break;
	case 94:	/* 3840x2160 @25Hz */
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
		dtd->mPixelClock = 297000;
		break;
	case 93:	/* 3840x2160 @24Hz */
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
		dtd->mPixelClock = 297000;
		break;
	case 98:	/* 4096x2160 @24Hz (SMPTE) */
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
		dtd->mPixelClock = 297000;
		break;

/* VESA DMT timing standard */
	case 2000:	/* 640x350@85Hz */
		dtd->mHActive = 640;
		dtd->mVActive = 350;
		dtd->mHBlanking = 192;
		dtd->mVBlanking = 95;
		dtd->mHSyncOffset = 32;
		dtd->mVSyncOffset = 32;
		dtd->mHSyncPulseWidth = 64;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 31500;
		break;
	case 2001:	/* 640x400@85Hz */
		dtd->mHActive = 640;
		dtd->mVActive = 400;
		dtd->mHBlanking = 192;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 32;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 64;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 31500;
		break;
	case 2002:	/* 720x400@85Hz */
		dtd->mHActive = 720;
		dtd->mVActive = 400;
		dtd->mHBlanking = 216;
		dtd->mVBlanking = 46;
		dtd->mHSyncOffset = 36;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 72;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 35500;
		break;
	case 2003:	/* 640x480@60Hz */
		dtd->mHActive = 640;
		dtd->mVActive = 480;
		dtd->mHBlanking = 144;
		dtd->mVBlanking = 29;
		dtd->mHSyncOffset = 8;
		dtd->mVSyncOffset = 2;
		dtd->mHSyncPulseWidth = 96;
		dtd->mVSyncPulseWidth = 2;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 25175;
		break;
	case 2004:	/* 640x480@72Hz */
		dtd->mHActive = 640;
		dtd->mVActive = 480;
		dtd->mHBlanking = 176;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 40;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 31500;
		break;
	case 2005:	/* 640x480@75Hz */
		dtd->mHActive = 640;
		dtd->mVActive = 480;
		dtd->mHBlanking = 200;
		dtd->mVBlanking = 20;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 64;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 31500;
		break;
	case 2006:	/* 640x480@85Hz */
		dtd->mHActive = 640;
		dtd->mVActive = 480;
		dtd->mHBlanking = 192;
		dtd->mVBlanking = 29;
		dtd->mHSyncOffset = 56;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 56;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 36000;
		break;
	case 2007:	/* 800x600@56Hz */
		dtd->mHActive = 800;
		dtd->mVActive = 600;
		dtd->mHBlanking = 224;
		dtd->mVBlanking = 25;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 72;
		dtd->mVSyncPulseWidth = 2;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 36000;
		break;
	case 2008:	/* 800x600@60Hz */
		dtd->mHActive = 800;
		dtd->mVActive = 600;
		dtd->mHBlanking = 256;
		dtd->mVBlanking = 28;
		dtd->mHSyncOffset = 40;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 128;
		dtd->mVSyncPulseWidth = 4;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 40000;
		break;
	case 2009:	/* 800x600@72Hz */
		dtd->mHActive = 800;
		dtd->mVActive = 600;
		dtd->mHBlanking = 240;
		dtd->mVBlanking = 66;
		dtd->mHSyncOffset = 56;
		dtd->mVSyncOffset = 37;
		dtd->mHSyncPulseWidth = 120;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 50000;
		break;
	case 2010:	/* 800x600@75Hz */
		dtd->mHActive = 800;
		dtd->mVActive = 600;
		dtd->mHBlanking = 256;
		dtd->mVBlanking = 25;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 80;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 49500;
		break;
	case 2011:	/* 800x600@85Hz */
		dtd->mHActive = 800;
		dtd->mVActive = 600;
		dtd->mHBlanking = 248;
		dtd->mVBlanking = 31;
		dtd->mHSyncOffset = 32;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 64;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 56250;
		break;
	case 2012:	/* 848x480@60Hz */
		dtd->mHActive = 848;
		dtd->mVActive = 480;
		dtd->mHBlanking = 240;
		dtd->mVBlanking = 37;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 6;
		dtd->mHSyncPulseWidth = 112;
		dtd->mVSyncPulseWidth = 8;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 33750;
		break;
	case 2013:	/* 1024x768@43Hz - interlaced */
		dtd->mHActive = 1024;
		dtd->mVActive = 768;
		dtd->mHBlanking = 240;
		dtd->mVBlanking = 24;
		dtd->mHSyncOffset = 8;
		dtd->mVSyncOffset = 0;
		dtd->mHSyncPulseWidth = 176;
		dtd->mVSyncPulseWidth = 4;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 1;
		dtd->mPixelClock = 44900;
		break;
	case 2014:	/* 1024x768@60Hz */
		dtd->mHActive = 1024;
		dtd->mVActive = 768;
		dtd->mHBlanking = 320;
		dtd->mVBlanking = 38;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 136;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 65000;
		break;
	case 2015:	/* 1024x768@70Hz */
		dtd->mHActive = 1024;
		dtd->mVActive = 768;
		dtd->mHBlanking = 304;
		dtd->mVBlanking = 38;
		dtd->mHSyncOffset = 24;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 136;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 75000;
		break;
	case 2016:	/* 1024x768@75Hz */
		dtd->mHActive = 1024;
		dtd->mVActive = 768;
		dtd->mHBlanking = 288;
		dtd->mVBlanking = 32;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 96;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 78750;
		break;
	case 2017:	/* 1024x768@85Hz */
		dtd->mHActive = 1024;
		dtd->mVActive = 768;
		dtd->mHBlanking = 352;
		dtd->mVBlanking = 40;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 96;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 94500;
		break;
	case 2018:	/* 1152x864@75Hz */
		dtd->mHActive = 1152;
		dtd->mVActive = 864;
		dtd->mHBlanking = 448;
		dtd->mVBlanking = 36;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 128;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 108000;
		break;
	case 2019:	/* 1280x768@60Hz - Reduced Blanking */
		dtd->mHActive = 1280;
		dtd->mVActive = 768;
		dtd->mHBlanking = 160;
		dtd->mVBlanking = 22;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 32;
		dtd->mVSyncPulseWidth = 7;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 68250;
		break;
	case 2020:	/* 1280x768@60Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 768;
		dtd->mHBlanking = 384;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 128;
		dtd->mVSyncPulseWidth = 7;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 79500;
		break;
	case 2021:	/* 1280x768@75Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 768;
		dtd->mHBlanking = 416;
		dtd->mVBlanking = 37;
		dtd->mHSyncOffset = 80;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 128;
		dtd->mVSyncPulseWidth = 7;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 102250;
		break;
	case 2022:	/* 1280x768@85Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 768;
		dtd->mHBlanking = 432;
		dtd->mVBlanking = 41;
		dtd->mHSyncOffset = 80;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 136;
		dtd->mVSyncPulseWidth = 7;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 117500;
		break;
	case 2023:	/* 1280x960@60Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 960;
		dtd->mHBlanking = 520;
		dtd->mVBlanking = 40;
		dtd->mHSyncOffset = 96;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 112;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 108000;
		break;
	case 2024:	/* 1280x960@85Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 960;
		dtd->mHBlanking = 448;
		dtd->mVBlanking = 51;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 160;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 148500;
		break;
	case 2025:	/* 1280x1024@60Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 1024;
		dtd->mHBlanking = 408;
		dtd->mVBlanking = 42;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 112;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 108000;
		break;
	case 2026:	/* 1280x1024@75Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 1024;
		dtd->mHBlanking = 408;
		dtd->mVBlanking = 42;
		dtd->mHSyncOffset = 16;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 144;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 135000;
		break;
	case 2027:	/* 1280x1024@85Hz */
		dtd->mHActive = 1280;
		dtd->mVActive = 1024;
		dtd->mHBlanking = 448;
		dtd->mVBlanking = 48;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 160;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 157500;
		break;
	case 2028:	/* 1360x768@60Hz */
		dtd->mHActive = 1360;
		dtd->mVActive = 768;
		dtd->mHBlanking = 432;
		dtd->mVBlanking = 27;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 112;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 85500;
		break;
	case 2029:	/* 1400x1050@60Hz - Reduced Blanking */
		dtd->mHActive = 1400;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 160;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 32;
		dtd->mVSyncPulseWidth = 4;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 101000;
		break;
	case 2030:	/* 1400x1050@60Hz */
		dtd->mHActive = 1400;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 464;
		dtd->mVBlanking = 39;
		dtd->mHSyncOffset = 88;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 144;
		dtd->mVSyncPulseWidth = 4;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 121750;
		break;
	case 2031:	/* 1400x1050@75Hz */
		dtd->mHActive = 1400;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 496;
		dtd->mVBlanking = 49;
		dtd->mHSyncOffset = 104;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 144;
		dtd->mVSyncPulseWidth = 4;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 156000;
		break;
	case 2032:	/* 1400x1050@85Hz */
		dtd->mHActive = 1400;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 512;
		dtd->mVBlanking = 55;
		dtd->mHSyncOffset = 104;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 152;
		dtd->mVSyncPulseWidth = 4;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 179500;
		break;
	case 2033:	/* 1440x900@60Hz - Reduced Blanking */
		dtd->mHActive = 1440;
		dtd->mVActive = 900;
		dtd->mHBlanking = 160;
		dtd->mVBlanking = 26;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 32;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 88750;
		break;
	case 2034:	/* 1440x900@60Hz */
		dtd->mHActive = 1440;
		dtd->mVActive = 900;
		dtd->mHBlanking = 464;
		dtd->mVBlanking = 34;
		dtd->mHSyncOffset = 80;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 152;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 106500;
		break;
	case 2035:	/* 1440x900@75Hz */
		dtd->mHActive = 1440;
		dtd->mVActive = 900;
		dtd->mHBlanking = 496;
		dtd->mVBlanking = 42;
		dtd->mHSyncOffset = 96;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 152;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 136750;
		break;
	case 2036:	/* 1440x900@85Hz */
		dtd->mHActive = 1440;
		dtd->mVActive = 900;
		dtd->mHBlanking = 512;
		dtd->mVBlanking = 48;
		dtd->mHSyncOffset = 104;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 152;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 157000;
		break;
	case 2037:	/* 1600x1200@60Hz */
		dtd->mHActive = 1600;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 560;
		dtd->mVBlanking = 50;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 192;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 162000;
		break;
	case 2038:	/* 1600x1200@65Hz */
		dtd->mHActive = 1600;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 560;
		dtd->mVBlanking = 50;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 192;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 175500;
		break;
	case 2039:	/* 1600x1200@70Hz */
		dtd->mHActive = 1600;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 560;
		dtd->mVBlanking = 50;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 192;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 189000;
		break;
	case 2040:	/* 1600x1200@75Hz */
		dtd->mHActive = 1600;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 560;
		dtd->mVBlanking = 50;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 192;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 202500;
		break;
	case 2041:	/* 1600x1200@85Hz */
		dtd->mHActive = 1600;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 560;
		dtd->mVBlanking = 50;
		dtd->mHSyncOffset = 64;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 192;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 229500;
		break;
	case 2042:	/* 1680x1050@60Hz - Reduced Blanking */
		dtd->mHActive = 1680;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 160;
		dtd->mVBlanking = 30;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 32;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 119000;
		break;
	case 2043:	/* 1680x1050@60Hz */
		dtd->mHActive = 1680;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 560;
		dtd->mVBlanking = 39;
		dtd->mHSyncOffset = 104;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 176;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 146250;
		break;
	case 2044:	/* 1680x1050@75Hz */
		dtd->mHActive = 1680;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 592;
		dtd->mVBlanking = 49;
		dtd->mHSyncOffset = 120;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 176;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 187000;
		break;
	case 2045:	/* 1680x1050@85Hz */
		dtd->mHActive = 1680;
		dtd->mVActive = 1050;
		dtd->mHBlanking = 608;
		dtd->mVBlanking = 55;
		dtd->mHSyncOffset = 128;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 176;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 214750;
		break;
	case 2046:	/* 1792x1344@60Hz */
		dtd->mHActive = 1792;
		dtd->mVActive = 1344;
		dtd->mHBlanking = 656;
		dtd->mVBlanking = 50;
		dtd->mHSyncOffset = 128;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 200;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 204750;
		break;
	case 2047:	/* 1792x1344@75Hz */
		dtd->mHActive = 1792;
		dtd->mVActive = 1344;
		dtd->mHBlanking = 664;
		dtd->mVBlanking = 73;
		dtd->mHSyncOffset = 96;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 216;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 261000;
		break;
	case 2048:	/* 1856x1392@60Hz */
		dtd->mHActive = 1856;
		dtd->mVActive = 1392;
		dtd->mHBlanking = 672;
		dtd->mVBlanking = 47;
		dtd->mHSyncOffset = 96;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 224;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 218250;
		break;
	case 2049:	/* 1856x1392@75Hz */
		dtd->mHActive = 1856;
		dtd->mVActive = 1392;
		dtd->mHBlanking = 704;
		dtd->mVBlanking = 108;
		dtd->mHSyncOffset = 128;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 224;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 288000;
		break;
	case 2050:	/* 1920x1200@60Hz - Reduced Blanking */
		dtd->mHActive = 1920;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 160;
		dtd->mVBlanking = 35;
		dtd->mHSyncOffset = 48;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 32;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 1;
		dtd->mVSyncPolarity = 0;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 154000;
		break;
	case 2051:	/* 1920x1200@60Hz */
		dtd->mHActive = 1920;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 672;
		dtd->mVBlanking = 45;
		dtd->mHSyncOffset = 136;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 200;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 193250;
		break;
	case 2052:	/* 1920x1200@75Hz */
		dtd->mHActive = 1920;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 688;
		dtd->mVBlanking = 55;
		dtd->mHSyncOffset = 136;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 208;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 245250;
		break;
	case 2053:	/* 1920x1200@85Hz */
		dtd->mHActive = 1920;
		dtd->mVActive = 1200;
		dtd->mHBlanking = 704;
		dtd->mVBlanking = 62;
		dtd->mHSyncOffset = 144;
		dtd->mVSyncOffset = 3;
		dtd->mHSyncPulseWidth = 208;
		dtd->mVSyncPulseWidth = 6;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 281250;
		break;
	case 2054:	/* 1920x1440@60Hz */
		dtd->mHActive = 1920;
		dtd->mVActive = 1440;
		dtd->mHBlanking = 680;
		dtd->mVBlanking = 60;
		dtd->mHSyncOffset = 128;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 208;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 234000;
		break;
	case 2055:	/* 1920x1440@75Hz */
		dtd->mHActive = 1920;
		dtd->mVActive = 1440;
		dtd->mHBlanking = 720;
		dtd->mVBlanking = 60;
		dtd->mHSyncOffset = 144;
		dtd->mVSyncOffset = 1;
		dtd->mHSyncPulseWidth = 224;
		dtd->mVSyncPulseWidth = 3;
		dtd->mHSyncPolarity = 0;
		dtd->mVSyncPolarity = 1;
		dtd->mInterlaced = 0;
		dtd->mPixelClock = 297000;
		break;

/* Internal vic, not exposure to userspace */
	case DISPLAY_SPECIFIC_VIC_PERIPHERAL:
		*dtd = dtd_peripheral[path];
		break;

	case DISPLAY_SPECIFIC_VIC_OSD:
		*dtd = dtd_osd[path];
		break;

	default:
		if (code >= DISPLAY_SPECIFIC_VIC_BUFFER_START
				&& code < DISPLAY_SPECIFIC_VIC_BUFFER_START + IDS_OVERLAY_NUM) {
			*dtd = dtd_buffer[path][code-DISPLAY_SPECIFIC_VIC_BUFFER_START];
		}
		else if (code >= DISPLAY_SPECIFIC_VIC_OVERLAY_START
				&& code < DISPLAY_SPECIFIC_VIC_OVERLAY_START + IDS_OVERLAY_NUM) {
			*dtd = dtd_overlay[path][code-DISPLAY_SPECIFIC_VIC_OVERLAY_START];
		}
		else {
			dlog_dbg("Unrecognized vic code %d\n", code);
			return -EINVAL;
		}
		break;
	}
	return 0;
}

static int vic_set_dtd(struct display_device *pdev, int vic, dtd_t *dtd)
{
	int path = pdev->path;
	
	if (vic == DISPLAY_SPECIFIC_VIC_PERIPHERAL) {
		dtd_peripheral[path] = *dtd;
	}
	else if (vic == DISPLAY_SPECIFIC_VIC_OSD) {
		dtd_osd[path] = *dtd;
	}
	else if (vic >= DISPLAY_SPECIFIC_VIC_BUFFER_START
				&& vic < DISPLAY_SPECIFIC_VIC_BUFFER_START + IDS_OVERLAY_NUM) {
		dtd_buffer[path][vic-DISPLAY_SPECIFIC_VIC_BUFFER_START] = *dtd;
	}
	else if (vic >= DISPLAY_SPECIFIC_VIC_OVERLAY_START
				&& vic < DISPLAY_SPECIFIC_VIC_OVERLAY_START + IDS_OVERLAY_NUM) {
		dtd_overlay[path][vic-DISPLAY_SPECIFIC_VIC_OVERLAY_START] = *dtd;
	}
	else {
		dlog_err("Invalid vic %d\n", vic);
		return -EINVAL;
	}

	dlog_dbg("path%d: vic=%d, mHActive=%d, mVActive=%d, mInterlaced=%d\n",
					path, vic, dtd->mHActive, dtd->mVActive, dtd->mInterlaced);

	return 0;
}

static int vic_probe(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static int vic_remove(struct display_device *pdev)
{
	dlog_trace();

	return 0;
}

static struct display_driver vic_driver = {
	.probe  = vic_probe,
	.remove = vic_remove,
	.driver = {
		.name = "vic",
		.owner = THIS_MODULE,
	},
	.module_type = DISPLAY_MODULE_TYPE_VIC,
	.function = {
		.set_dtd = vic_set_dtd, 
	}
};

module_display_driver(vic_driver);
MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display vic driver");
MODULE_LICENSE("GPL");
