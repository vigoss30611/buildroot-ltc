#ifndef _AXP_COMMON_H_
#define _AXP_COMMON_H_

#include "axp-struct.h"

extern void axp_mode_scan(struct device *);
extern void axp_drvbus_ctrl(int);
extern void axp_vbusen_ctrl(int);
extern int  axp_irq_used(void);
extern void axp_irq_init(uint32_t *, uint32_t *);
extern int  axp_pwrkey_used(void);
extern void axp_mfd_minit(struct device *);
extern void axp_mfd_msuspend(struct device *);
extern void axp_mfd_mresume(struct device *);
extern void axp_batt_minit(struct axp_charger *);
extern void axp_batt_msuspend(struct axp_charger *);
extern void axp_batt_mresume(struct axp_charger *);

#endif
