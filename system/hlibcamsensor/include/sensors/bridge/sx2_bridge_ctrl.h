#ifndef SX2_BRIDGE_CTRL_H
#define SX2_BRIDGE_CTRL_H
#include <stdint.h>
#include <linux/ioctl.h>

enum SX2_MODE {
    SX2_MANUAL_MODE = 0x00,
    SX2_AUTO_MODE = 0x01,
};

struct _sx2_in_fmt_t{
    uint16_t h_size;
    uint16_t v_size;
    uint16_t h_blk;
    uint16_t v_blk;
};

struct sx2_init_config {
    uint32_t mode;
    uint32_t freq;
    struct _sx2_in_fmt_t in_fmt;
};

#define SX2_CTRL_MAGIC    ('S')

#define SX2_G_CFG    _IOW(CTRL_MAGIC, 0x32, struct sx2_init_config)
#define SX2_S_CFG    _IOW(CTRL_MAGIC, 0x33, struct sx2_init_config)

#define SX2_BRIDGE_NODE "/dev/sx2_bridge"

#endif
