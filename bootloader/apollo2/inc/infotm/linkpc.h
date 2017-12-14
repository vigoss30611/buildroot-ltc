#ifndef __IROM_LINKPC_H__
#define __IROM_LINKPC_H__

extern int linkpc_loop(void);
extern void linkpc_set_stage(const char * stage);
extern int linkpc_hook(void);
extern int linkpc_necessary(void);
extern int linkpc_manual_forced(unsigned int gpio);

#endif /* __IROM_LINKPC_H__ */

