/*****************************************************************************
** bootpic.h
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

#ifndef __EBOOT_BOOTPIC_H
#define __EBOOT_BOOTPIC_H
#include <lcd.h>

#define _sc_w	(panel_info.vl_col)
#define _sc_h	(panel_info.vl_row)
#define _v_bp	(NBITS(panel_info.vl_bpix)/8)

enum oem_image_type {
	OEM_IMAGE_RAW = 0,
	OEM_IMAGE_RAW_H,
	OEM_IMAGE_YAFFS,
};
enum Loadimageerrortype{
	Erro_NON=0,     //read image1 successfully,and checked ok.
	Erro_READ,  //it means something error occoured when read inand, a rerset is needed.
	Erro_CHECKSUM, //it means read image1 success, but checked error.
	Erro_AllCORRUPT//it means succcessfully to read both image1 and image2, but both checked error.
};
enum inandimage{
	UIMAGE=0,
	RAMDISK,
	RAMDRE,
	UPDATER,
	UBOOT
};
struct part_table{
	char bootableflag;
	char startCHS[3];
	char parttype;
	char endCHS[3];
	unsigned int startLBA;
	unsigned int sizeinsectors;
};
struct boottype_info {
	char command[32];
	char status[32];
	char recovery[1024];
};
#define IMAGE_ERROR 0xE7A10000

//extern  int oem_get_imagebase(void);

extern int oem_partition(void);
extern int oem_get_systembase(void);
extern int oem_get_base(int partnum);
extern int oem_check_part_state(void);
extern int oem_get_system_length(void) ;
extern int oem_set_system_length(int length);
extern int oem_read_sys_disk(uint8_t *membase, int start, int length);
extern int oem_write_sys_disk(uint8_t *membase, int start, int length , int max_len);

extern void oem_show_logo(void);
extern void oem_show_starting(void);
extern  void oem_show_batt(void);
extern void power_off(void);
extern void oem_opt_key_toggle(int flag);
extern void oem_load_back_kernel(void);
extern void oem_progress_init(void);
extern void oem_progress_finish(void);
extern void oem_choose_kernel(void);
extern int oem_mmc_init(int channel);
extern void uboot_basic_boot(void);
extern void oem_bootl(char *args);
extern void oem_bootw(void);
extern void oem_choose_kernel(void);
extern void * oem_check_img(uint32_t addr);
extern int oem_maintain_image(void);
extern int oem_set_maintain(void);
extern int oem_clear_maintain(void);
extern void (* oem_progress_update)(ushort val);
extern int oem_load_NK(void);
extern int oem_load_LK(void);
extern int oem_load_RD(void);
extern int oem_load_RD_(void);
extern int oem_burn_uboot(uint8_t * data);
extern int oem_burn_NK(uint8_t * data);
extern int oem_burn_LK(uint8_t * data);
extern int oem_burn_SYS(uint8_t *data, uint32_t size);
extern int oem_burn_UDAT(uint8_t *data, uint32_t size);
extern int oem_burn_NDISK(uint8_t *data, uint32_t size);
extern int oem_burn_RD(uint8_t * data);
extern int oem_burn_RD_(uint8_t * data);
extern int oem_burn_zAS(uint8_t * data);
extern int oem_start_updater(int type);
extern int oem_load_updater(void);
#ifndef CONFIG_SYS_DISK_iNAND
extern int oem_load_img(uint32_t ram_base, uint32_t nand_addr, uint32_t back_addr, uint32_t recv_len);
extern int iuw_upfrom_nand(void);
#else
extern int oem_burn_UPDT(uint8_t *data);
extern int oem_load_img(uint32_t ram_base, char image);
extern int iuw_upfrom_inand(void);
#ifdef CONFIG_HIBERNATE
extern int oem_hibernate_manage(void);
#endif
#endif

extern int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int gunzip(void *dst, int dstlen, unsigned char *src, unsigned long *lenp);
extern void oem_revert_pic(uint8_t *buf, short w, short h);
extern int oem_set_boot_type(int type);
extern int oem_got_boot_info(void);
extern int oem_got_boot_type(void);
extern int burn_file_inand(char *filename,char * addr ,char imgnum);
extern void oem_bootw(void);
extern void oem_uboot_maintain(void);
extern void oem_update_wince(void);
extern void oem_consume_keypress(void);
extern uint8_t oem_getc(void);
extern void oem_getc_exec(uint8_t a);
extern void imapx200_cache_flush(uint32_t start, uint32_t end);
extern void imapx200_cache_inv(uint32_t start, uint32_t end);
extern void imapx200_cache_clean(uint32_t start, uint32_t end);
extern void oem_mmu_enable(void);
extern int oem_erase_markbad(uint32_t start, uint32_t len);
extern int oem_read_markbad(uint8_t *buf, uint32_t start, uint32_t length);
extern int oem_write_markbad(uint8_t *data, uint32_t start, uint32_t length, uint32_t max_len);
extern int oem_write_yaffs(uint8_t *img, uint32_t size, uint32_t start, uint32_t max_len);
extern void oem_update_clean_hive(void);
extern int oem_otg_check(void);
extern int oem_otg_server(void);
extern uint32_t oem_factory_check(void);
extern int oem_disk_clear(void);
extern uint32_t get_PCLK(void);
extern void oem_basic_up_uimage(void);
extern int imapx200_kbd_init (void);
extern int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int oem_load_nand_LK(uint32_t nand_addr);
extern int do_fat_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int do_bootd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
extern int oem_load_nand_img(uint32_t ram_base, uint32_t nand_addr);
extern int oem_scrub(uint32_t start, uint32_t len);
extern int oem_clear_badblock(void);
extern int oem_burn_U0(uint8_t * data);
extern int oem_clear_env(void);
extern int oem_boot_sd(void);
extern int oem_simpro_init(char *title, uint32_t size);
extern int oem_simpro_finish(char *title);
extern int oem_simpro_update(uint32_t n);
extern int android_check_recovery(void);
extern int android_do_recovery(void);
extern int nor_program(uint32_t, uint8_t *, uint32_t);
extern int nor_id(int entry);
extern uint16_t nor_get(uint32_t addr);
extern int nor_erase_chip(void);
extern int nor_erase_block(uint32_t addr);
extern void nor_hw_init(void);
extern void oem_print_eye(char *s);
extern void oem_draw_box_color(u_short x1, u_short y1, u_short x2, u_short y2, u_short color);
extern void oem_check_denali(void);
extern void backlight_power_off(void);
extern void oem_test_denali(void);
extern int iuw_update(uint8_t *wrap);
extern int iuw_confirm(uint8_t *wrap);
extern uint8_t * iuw_img_data(uint8_t * wrap, int num);
extern  int oem_maitain_image(void);
extern void oem_getc_exec_gpio(uint32_t gpio, int num);
extern int iuw_check_update(void);
extern int burn_simage(int  simage ,int index, int to );
struct oem_kernel {
	char name[16];
	struct oem_picture *logo;
	void (* bootfunc)(void);
};
#endif /* __EBOOT_BOOTPIC_H */
