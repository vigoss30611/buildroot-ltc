
#ifndef _IROM_IUS_H_
#define _IROM_IUS_H_

struct ius_desc {
	uint32_t	sector;
	uint32_t	info0;
	uint32_t	info1;
	uint32_t	length;
};

struct ius_hdr {
	uint32_t	magic;
	uint32_t	flag;
	uint8_t		hwstr[8];
	uint8_t		swstr[8];
	uint32_t	hcrc;
	uint32_t	count;
	uint8_t		data[480];
};

extern int ius_check_header(struct ius_hdr *hdr);
extern int ius_count_size  (struct ius_hdr *hdr);

#define IUS_DEFAULT_LOCATION  0x1000000
#define IUS_MAGIC			0x7375692b
#define ius_get_count(x)	((x)->count)
#define ius_get_desc(x, b)	(struct ius_desc *)(((x)->data + b * sizeof(struct ius_desc)))

#define ius_desc_offi(x)	(((uint64_t)((x)->sector & 0xffffff) << 9)	 \
			+ IUS_DEFAULT_LOCATION)
#define ius_desc_offo(x)	((uint64_t)((x)->info0) | ((uint64_t)((x)->info1 & 0xffff) << 32))
#define ius_desc_type(x)	(((x)->sector >> 24) & 0x3f)
#define ius_desc_length(x)	((x)->length)
#define ius_desc_id(x)		(((x)->info1 >> 16) & 0xffff)
/* max block number and device share the same field */
#define ius_desc_maxsize(x)		((x)->info1 & ~0xffff)
#define ius_desc_load(x)		((x)->info1)
#define ius_desc_entry(x)		((x)->info0)
#define ius_desc_sector(x)	((x)->sector & 0xffffff)
#define ius_desc_mask(x)		((x)->sector >> 30)

#define IUS_DESC_CFG	0
#define IUS_DESC_EXEC	1
#define IUS_DESC_STRW	2
#define IUS_DESC_STNB	3
#define IUS_DESC_STNR	4
#define IUS_DESC_STNF	5

#define IUS_DESC_REBOOT	0x30
#define IUS_DESC_BOOT	0x31
#define IUS_DESC_BOOTR	0x32

#define IUS_DESC_SYSM	0x20
#define IUS_DESC_MISC	0x21
#define IUS_DESC_CACH	0x22
#define IUS_DESC_USER	0x23

#define IUS_FLAG_COMPRESS	0x1
#define IUS_FLAG_ENCRYPT	0x2

#define IR_MAX_BUFFER 0x10000
#endif /* _IROM_IUS_H_ */

