
#ifndef __IROM_SATA_H__
#define __IROM_SATA_H__

int sata_vs_read(uint8_t *buf, loff_t offs, int len, int extra);
int sata_vs_write(uint8_t *buf, loff_t offs, int len, int extra);
int sata_vs_erase(loff_t offs, uint32_t len);
int sata_vs_reset(void);
int sata_vs_align(void);

#endif /* __IROM_SATA_H__ */
