

#ifndef __IROM_EEPROM_H__
#define __IROM_EEPROM_H__

int e2p_vs_read(uint8_t *buf, loff_t offs, int len, int extra);
int e2p_vs_write(uint8_t *buf, loff_t offs, int len, int extra);
int e2p_vs_reset(void);
int e2p_vs_align(void);

#endif /* __IROM_EEPROM_H__ */
