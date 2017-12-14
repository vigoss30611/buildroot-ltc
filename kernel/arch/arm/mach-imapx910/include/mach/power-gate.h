#ifndef __POWER_GATE_H__
#define __POWER_GATE_H__

#define ENABLE     1
#define DISABLE    0

#define SYS_SOFT_RESET(x)            (x + 0x00)     /* soft reset register */
#define SYS_CLOCK_GATE_EN(x)         (x + 0x04)     /* clock gated enable register */
#define SYS_POWER_CONTROL(x)         (x + 0x08)     /* power control register */
#define SYS_BUS_MANAGER(x)           (x + 0x0c)     /* bus manager register */
#define SYS_BUS_QOS_MANAGER0(x)      (x + 0x10)     /* bus QOS manager register */
#define SYS_BUS_QOS_MANAGER1(x)      (x + 0x14)     /* bus QOS manager register */
#define SYS_BUS_ISOLATION_R(x)       (x + 0x18)     /* bus signal isolation */

/* power control */
#define MODULE_POWERON            0x11
#define MODULE_ISO_CLOSE          0x1
#define MODULE_POWERON_ACK        1
#define MODULE_POWERDOWN          0x10

#define MODULE_BUS_ENABLE         0x3

int module_reset(uint32_t module_addr, uint32_t reset_sel);
int module_power_on(uint32_t module_addr);
int module_power_on_lite(uint32_t module_addr);
int module_power_down(uint32_t module_addr);
void imapx910_reset_core(int index, uint32_t hold_base, uint32_t virt_base);
void imapx910_reset_module(void);

#endif /* power-gate.h */
