#ifndef __IUS_H__
#define __IUS_H__

#include <stdint.h>
#include <unistd.h>
#include "upgrade.h"

#define IUS_MAGIC   0x7375692b
#define IUS_OFFSET  0x1000000

struct ius_desc {
    uint32_t    sector;
    uint32_t    info0;
    uint32_t    info1;
    uint32_t    length;
};

struct ius_hdr {
    uint32_t        magic;
    uint32_t        flag;
    uint8_t         hwstr[8];
    uint8_t         swstr[8];
    uint32_t        hcrc;
    uint32_t        count;
    struct ius_desc data[480];
};

#define ius_get_count(x)    ((x)->count)
#define ius_get_desc(x, b)  (struct ius_desc *)(((x)->data + b * sizeof(struct ius_desc)))

#define ius_desc_offi(x)    (((uint64_t)((x)->sector & 0xffffff) << 9)   \
            + BL_DEFAULT_LOCATION)
#define ius_desc_offo(x)    ((uint64_t)((x)->info0) | ((uint64_t)((x)->info1 & 0xffff) << 32))
#define ius_desc_type(x)    (((x)->sector >> 24) & 0x3f)
#define ius_desc_length(x)  ((x)->length)
#define ius_desc_id(x)      (((x)->info1 >> 16) & 0xffff)
/*  max block number and device share the same field */
#define ius_desc_maxsize(x)     ((x)->info1 & ~0xffff)
#define ius_desc_load(x)        ((x)->info1)
#define ius_desc_entry(x)       ((x)->info0)
#define ius_desc_sector(x)  ((x)->sector & 0xffffff)
#define ius_desc_mask(x)        ((x)->sector >> 30)

#define ius_desc_start(x) (((x)->sector&0xffffff)<<9)

struct ius_t {
    int fd;
    int inited;
    int is_file;
    char dev_path[32];
    struct ius_hdr header;
};

extern int ius_init(struct upgrade_t *upgrade, char *ius_path);
extern void ius_deinit(struct upgrade_t *upgrade);
extern int ius_get_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t *offset, uint64_t *size);
extern void ius_release_image(struct upgrade_t *upgrade, image_type_t image_num);
extern int ius_check_image(struct upgrade_t *upgrade, image_type_t image_type);
extern int ius_read_image(struct upgrade_t *upgrade, image_type_t image_num, uint64_t offset, uint64_t size, char *data);

#endif
