
#ifndef __IROM_RAMDISK_H__
#define __IROM_RAMDISK_H__

int ram_vs_read(uint8_t *buf, loff_t offs, int len, int extra);
int ram_vs_write(uint8_t *buf, loff_t offs, int len, int extra);
int ram_vs_reset(void);
int ram_vs_align(void);
loff_t ram_addr2offs(uint32_t addr);
uint32_t ram_offs2addr(loff_t offs);

#endif /* __IROM_RAMDISK_H__ */
