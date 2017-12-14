#include <cdbg.h>
#include <cpuid.h>

extern struct irom_export *irf;

struct irom_export {
    void        (*printf)(const char *fmt, ...);
    int         (*vs_read)(uint8_t *buf, loff_t start, int length, int extra);
    int         (*isi_check)(void *data);
    int         (*vs_assign_by_id)(int id, int reset);
    int         (*boot_device)(void);
//    int         (*nand_rescan_config)(struct cdbg_cfg_nand t[], int count);
//    void        (*cdbg_shell)(void);
    //void *      (*nand_get_config)(int type);
};

extern struct irom_export irf_2885;
extern struct irom_export irf_2939;
extern struct irom_export irf_2974;

