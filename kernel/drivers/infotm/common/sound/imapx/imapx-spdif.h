#ifndef __IMAPX800_SPDIF__
#define __IMAPX800_SPDIF__

#define SP_CTRL         0x00
#define SP_TXCONFIG     0x04
#define SP_RXCONFIG     0x08
#define SP_TXCHST       0x0c
#define SP_RXCHST       0x10
#define SP_FIFOSTAT     0x14
#define SP_INTMASK      0x18
#define SP_INTSTAT      0x1c
#define SP_TXFIFO       0x20
#define SP_RXFIFO       0x24
#define SP_TXCHST_A0    0x30
#define SP_TXCHST_A1    0x34
#define SP_TXCHST_A2    0x38
#define SP_TXCHST_A3    0x3c
#define SP_TXCHST_A4    0x40
#define SP_TXCHST_A5    0x44
#define SP_TXCHST_B0    0x50
#define SP_TXCHST_B1    0x54
#define SP_TXCHST_B2    0x58
#define SP_TXCHST_B3    0x5c
#define SP_TXCHST_B4    0x60
#define SP_TXCHST_B5    0x64
#define SP_TXUDAT_A0    0x70
#define SP_TXUDAT_A1    0x74
#define SP_TXUDAT_A2    0x78
#define SP_TXUDAT_A3    0x7c
#define SP_TXUDAT_A4    0x80
#define SP_TXUDAT_A5    0x84
#define SP_TXUDAT_B0    0x90
#define SP_TXUDAT_B1    0x94
#define SP_TXUDAT_B2    0x98
#define SP_TXUDAT_B3    0x9c
#define SP_TXUDAT_B4    0xa0
#define SP_TXUDAT_B5    0xa4
#define SP_RXCHST_A0    0xb0
#define SP_RXCHST_A1    0xb4
#define SP_RXCHST_A2    0xb8
#define SP_RXCHST_A3    0xbc
#define SP_RXCHST_A4    0xc0
#define SP_RXCHST_A5    0xc4
#define SP_RXCHST_B0    0xd0
#define SP_RXCHST_B1    0xd4
#define SP_RXCHST_B2    0xd8
#define SP_RXCHST_B3    0xdc
#define SP_RXCHST_B4    0xe0
#define SP_RXCHST_B5    0xe4
#define SP_RXUDAT_A0    0xf0
#define SP_RXUDAT_A1    0xf4
#define SP_RXUDAT_A2    0xf8
#define SP_RXUDAT_A3    0xfc
#define SP_RXUDAT_A4    0x100
#define SP_RXUDAT_A5    0x104
#define SP_RXUDAT_B0    0x110
#define SP_RXUDAT_B1    0x114
#define SP_RXUDAT_B2    0x118
#define SP_RXUDAT_B3    0x11c
#define SP_RXUDAT_B4    0x120
#define SP_RXUDAT_B5    0x124

struct imapx_spdif_info {
    struct device   *dev;
    spinlock_t      lock;
    void __iomem    *regs;
    struct clk  *extclk;
    unsigned int    iis_clock_rate;
    int chan_num;
};

#endif
