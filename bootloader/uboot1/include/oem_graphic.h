/*****************************************************************************
** oem_graphic.h
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** Use of Infotm's code is governed by terms and conditions 
** stated in the accompanying licensing statement. 
** 
** Description:	E-boot image relative structures.
**
** Author:
**     warits    <warits@infotm.com>
**      
** Revision History: 
** ----------------- 
** 0.1  01/29/2010     warits
*****************************************************************************/

#ifndef __OEM_GRAPHIC_H
#define __OEM_GRAPHIC_H
#include <lcd.h>

#define _sc_w	(panel_info.vl_col)
#define _sc_h	(panel_info.vl_row)
#define _v_bp	(NBITS(panel_info.vl_bpix)/8)

struct oem_picture {

	u_short	x;
	u_short y;
	u_short	width;
	u_short	height;
	u_char	desc[32];

	u_char	*pic;
};

extern void oem_fb_switch(u_short win);
extern void oem_show_window(u_short win);
extern void oem_hide_window(u_short win);
extern void oem_clear_screen(u_short c);
extern void oem_set_fg(u_short x, u_short y, u_short w, u_short h, u_short c);
extern void oem_set_color(u_short x, u_short y, u_short w, u_short h, u_short c);
extern void oem_set_pic_raw(u_short x, u_short y, u_short w, u_short h, const u_char *src);
extern void oem_set_pic_raw_offset(u_short x, u_short y, u_short w, u_short h,
   u_short ox, u_short oy, u_short ow, u_short oh, const u_char *src);
extern void oem_set_pic(short x, short y, struct oem_picture *picture);
extern int oem_draw_box(u_short x1, u_short y1, u_short x2, u_short y2);
extern void oem_lcd_msg(char *s);
extern void oem_mid(char * s);
extern void oem_mid2(char * s);
extern void oem_below(char * s);
extern void oem_below2(char * s);
extern void oem_p_ef(char * s);
extern void oem_p_done(void);
extern void oem_p_pro(void);
extern int oem_p_spark(int a);
extern void oem_p_bg(void);
#endif /* __OEM_GRAPHIC_H */
