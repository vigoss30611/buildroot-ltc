
#ifndef __IROM_FLASH_H__
#define __IROM_FLASH_H__

int flash_vs_read(uint8_t *buf, loff_t offs, int len, int extra);
int flash_vs_write(uint8_t *buf, loff_t offs, int len, int extra);
int flash_vs_erase(loff_t offs, uint32_t len);
int flash_vs_reset(void);
int flash_vs_align(void);
void spi_set_bytes(int);

#endif /* __IROM_FLASH_H__ */
