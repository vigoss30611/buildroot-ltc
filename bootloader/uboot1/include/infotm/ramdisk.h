
#ifndef __IROM_RAMDISK_H__
#define __IROM_RAMDISK_H__

int ram_vs_read(uint8_t *buf, loff_t offs, int len, int extra);
int ram_vs_write(uint8_t *buf, loff_t offs, int len, int extra);
int ram_vs_reset(void);
int ram_vs_align(void);

#endif /* __IROM_RAMDISK_H__ */
