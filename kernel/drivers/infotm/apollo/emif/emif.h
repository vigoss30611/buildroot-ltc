#ifndef __EMIF_H__
#define __EMIF_H__

#include <linux/io.h>

struct imapx_emif;

struct imapx_emif{
    struct device    *dev;
    void __iomem     *io_base;
    struct clk       *clk;
    uint32_t         port[8];
    uint32_t         count[8];
    uint32_t         flag[8];
    resource_size_t  rsrc_start;
    resource_size_t  rsrc_len;
    spinlock_t       lock;
};

#define DEFAULT_PORT_V     0x00100130

#define IDS1_ENTRY         0x11
#define IDS1_EXIT          0x1f
#define IDS0_ENTRY         0x21
#define IDS0_EXIT          0x2f

extern uint32_t emif_control(uint32_t mode);

#endif /* emif.h */

