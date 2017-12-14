#ifndef _IROM_LOWL_H_
#define _IROM_LOWL_H_

extern void init_lowlevel(void);
extern void init_mmu(void);
extern void set_mux(uint32_t);
extern void init_mmu_table(void);
extern void module_set_clock(uint32_t base, int srctype, int divtype, uint8_t divvalue);
extern int module_get_clock(uint32_t base);
extern void module_enable(uint32_t base);
extern void module_reset(uint32_t base);

extern void pads_chmod(int index, int mode, int val);
extern void pads_pull(int index, int en, int high);
extern void pads_setpin(int index, int val);
extern int pads_getpin(int index);
extern int pads_states(int index);
extern void set_xom(int xom);
extern void init_igps_ram(void);
void reset_core(int index, uint32_t hold_base);
void disable_core(int index);
extern void set_mux(uint32_t mux);
extern void init_mux(int high);
#endif /* _IROM_LOWL_H_ */

