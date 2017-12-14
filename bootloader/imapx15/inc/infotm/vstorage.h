
// InfoTMIC iROM, all right reserved.
// File: VirtualStorage.h
// Desc: Provide an abstracted layed for storage devices accessing.
// Author: Warits
// Date: Jun. 11, 2011

#ifndef __IROM_VS_H__
#define __IROM_VS_H__

#include <bootlist.h>

/*
 * @flag: bit (0, 1, 2, 3, 4), indicates (bootable, burnsource, burndst,
 *        storefrombegining, erasable)
 */
struct vstorage {
	char		name[8];
	uint32_t	flag;
	int			(* align)(void);
	int			(* read)(uint8_t *buf, loff_t start, int length, int extra);
	int			(* write)(uint8_t *buf, loff_t start, int length, int extra);
//	int			(* w2)(uint8_t *buf, loff_t start, int length, int extra);
	int			(* reset)(void);
	int			(* erase)(loff_t start, uint64_t size);
	int			(* scrub)(loff_t start, uint64_t size);
	int			(* isbad)(loff_t offset);
	int			(* special)(void *arg);
	loff_t      (*capacity)(void);
};

enum vs_options {
	VS_OPT_ALIGN = 0,
	VS_OPT_READ,
	VS_OPT_WRITE,
	VS_OPT_RESET,
	VS_OPT_ERASE,
	VS_OPT_SCRUB,
};

extern int vs_reset(void);
extern int vs_read (uint8_t *buf, loff_t start, size_t length, int extra);
extern int vs_write(uint8_t *buf, loff_t start, size_t length, int extra);
extern int vs_erase(loff_t start, uint64_t size);
extern int vs_isbad(loff_t start);
//extern int vs_w2(uint8_t *buf, loff_t start, int length, int extra);
extern int vs_scrub(loff_t start, uint64_t size);
extern int vs_special(void *arg);

extern int vs_assign_by_name(const char *name, int reset);
extern int vs_assign_by_id(int id, int reset);
extern int vs_device_id(const char *name);
extern int vs_is_assigned(void);
extern int vs_align(int id);
extern loff_t vs_capacity(int id);
extern int vs_is_device(int id, const char *name);
extern int vs_is_opt_available(int id, int opt);

#define	DEVICE_BOOTABLE		(1 << 0)
#define	DEVICE_BURNSRC		(1 << 1)
#define	DEVICE_BURNDST		(1 << 2)
#define DEVICE_BOOT0		(1 << 3)
#define DEVICE_ERASABLE		(1 << 4)
#define DEVICE_ID(x)		((x & 0xff) << 24)
extern int vs_device_bootable(int id);
extern int vs_device_burnsrc(int id);
extern int vs_device_burndst(int id);
extern int vs_device_boot0(int id);
extern int vs_device_erasable(int id);
extern int vs_device_count(void);
extern const char * vs_device_name(int id);

extern const struct vstorage vs_list[];

#define list_for_each_vs(pos)		\
	for (pos = vs_list;		 \
		 pos <= &vs_list[ARRAY_SIZE(vs_list) - 1];	 \
		 pos++)
#define INFO_MAX_PARTITIONS	8
#endif /* __IROM_VS_H__ */
