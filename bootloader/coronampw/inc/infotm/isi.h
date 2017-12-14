
#ifndef _IROM_ISI_H_
#define _IROM_ISI_H_
#include <linux/types.h>
struct isi_hdr {
	uint32_t magic;
	uint32_t type;
	uint32_t flag;
	uint32_t size;
	uint32_t load;
	uint32_t entry;
	uint32_t time;
	uint32_t hcrc;
	uint8_t  dcheck[32];
	uint8_t  name[192];
	uint8_t  extra[256];
};

#define ISI_SIZE	0x200
#define ISI_MAGIC	0x6973692b

#define ISI_TYPE_IMAGE	0x0
#define ISI_TYPE_EXEC	0x1
#define ISI_TYPE_CFG	0x2
#define ISI_TYPE_BL		0x3

extern int isi_check_header(void *hdr);
extern int isi_check_data(void *data);
extern int isi_check_data_separate(struct isi_hdr *hdr, void *data);
extern int isi_check(void *data);
#define isi_is_secure(x)    !!((((struct isi_hdr *)x)->type >> 28) & 0xf)
#define isi_get_type_image(x)  (((struct isi_hdr *)x)->type &  0xffffff )
#define isi_get_type_sig(x)   ((((struct isi_hdr *)x)->type >> 24) & 0xf)
#define isi_get_load(x)        (((struct isi_hdr *)x)->load  )
#define isi_get_flag(x)        (((struct isi_hdr *)x)->flag  )
#define isi_get_entry(x)       (((struct isi_hdr *)x)->entry )
#define isi_get_size(x)        (((struct isi_hdr *)x)->size  )
#define isi_get_sig(x)         (((struct isi_hdr *)x)->dcheck)
#define isi_get_extra(x)       (((struct isi_hdr *)x)->extra )
#define isi_get_data(x)        ((uint8_t *)(((uint32_t)x) + ISI_SIZE))


#endif /* _IROM_ISI_H_ */

