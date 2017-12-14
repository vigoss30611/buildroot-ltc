/* apollo3 display registers */

#ifndef __DISPLAY_REGISTER_RELATION_HEADER__
#define __DISPLAY_REGISTER_RELATION_HEADER__

#ifdef CONFIG_ARCH_APOLLO3
#include "register_apollo3.h"
#endif
#ifdef CONFIG_ARCH_Q3F
#include "register_q3f.h"
#endif
#ifdef CONFIG_ARCH_APOLLO
#include "register_apollo.h"
#endif

#include "register_apollo_i80.h"


#ifdef CONFIG_ARCH_APOLLO
#define IDS_BUS_CLK_SRC			"imap-ids.1"
#define IDS_OSD_CLK_SRC			"imap-ids1-ods"
#define IDS_OSD_CLK_FREQ_L		148500000
#define IDS_OSD_CLK_FREQ_M		198000000
#define IDS_OSD_CLK_FREQ_H		297000000
#define IDS_EITF_CLK_SRC			"imap-ids1-eitf"
#define IDS_TVIF_CLK_SRC			"imap-ids1-tvif"
#define IDS_CLK_SRC_PARENT		"imap-ids1"

#define HDMI_BUS_CLK_SRC		"imap-hdmi-tx"
#define HDMI_BUS_CLK_SRC_PARENT	"imap-hdmi-tx"
#define HDMI_CLK_SRC					"imap-hdmi"
#define HDMI_CLK_FREQ				24000000		// SFR clock. must <= 25MHz, best =25MHz
#define HDMI_CLK_SRC_PARENT		"imap-hdmi"
#else
#define IDS_BUS_CLK_SRC			"imap-ids.0"
#define IDS_OSD_CLK_SRC			"imap-ids0-osd"
#define IDS_OSD_CLK_FREQ_L		148500000
#define IDS_OSD_CLK_FREQ_M		198000000
#define IDS_OSD_CLK_FREQ_H		297000000
#define IDS_EITF_CLK_SRC			"imap-ids0-eitf"
#define IDS_CLK_SRC_PARENT		"imap-ids0"
#endif

enum overlay_registers {
	EOVCWxCR = 0,		// Control register
	EOVCWxPCAR,		// LeftTopX/Y
	EOVCWxPCBR,		// RightBotX/Y
	EOVCWxPCCR,		// alpha r,g,b
	EOVCWxVSSR,		// vw_width
	EOVCWxCKCR,		// chroma key control
	EOVCWxCKR,			// chroma key value
	EOVCWxCMR,			// map color
	EOVCWxB0SAR,		// buffer 0 addr
	EOVCWxB1SAR,		// buffer 1 addr
	EOVCWxB2SAR,		// buffer 2 addr
	EOVCWxB3SAR,		// buffer 3 addr
};

extern int __overlay_OVCWx(int overlay, enum overlay_registers enum_reg);

extern int __ids_sysmgr(int path);

extern int ids_access_init(int path, struct resource *res);

extern unsigned int ids_readword(int path, unsigned int addr);

extern int ids_writeword(int path, unsigned int addr, unsigned int val);

extern int ids_write(int path, unsigned int addr, int bit, int width, unsigned int val);

static inline void osd_unload_param(struct display_device *pdev)
{
	ids_write(pdev->path, OVCDCR, OVCDCR_UpdateReg, 1, 0);
}

static inline void osd_load_param(struct display_device *pdev)
{
	ids_write(pdev->path, OVCDCR, OVCDCR_UpdateReg, 1, 1);
}

#endif
