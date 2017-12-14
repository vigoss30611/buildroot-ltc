#include <cdbg.h>
#include <cpuid.h>
#include <ius.h>

#if defined(MODE) && (MODE == SIMPLE)
#define BL_LOC_UBOOT		0x8000
#define BL_LOC_CONFIG		0x7c000
#else
#define BL_LOC_UBOOT		0x800000
#define BL_LOC_CONFIG		0x1000000
#endif

extern struct irom_export *irf;
#define spl_printf(x...)		irf->printf("spl: " x)

extern void do_hang(void);
extern void do_burn(uint8_t *mem);
extern void do_boot(void);

struct irom_export {

	int         (*init_timer )(void);
	int         (*init_serial)(void);

	void        (*printf)(const char *fmt, ...);

	void        (*udelay)(unsigned long us);

	void *      (*memcpy)(void * dest,const void *src,size_t count);
	size_t      (*strnlen)(const char * s, size_t count);


	int         (*vs_reset)(void);
	int         (*vs_read)(uint8_t *buf, loff_t start, int length, int extra);
	int         (*vs_write)(uint8_t *buf, loff_t start, int length, int extra);
	int         (*vs_erase)(loff_t start, uint32_t size);
	int         (*vs_isbad)(loff_t start);
	int         (*vs_scrub)(loff_t start, uint32_t size);
	int         (*vs_special)(void *arg);

	int         (*vs_assign_by_name)(const char *name, int reset);
	int         (*vs_assign_by_id)(int id, int reset);
	int         (*vs_device_id)(const char *name);
	int         (*vs_is_assigned)(void);
	int         (*vs_align)(int id);
	int         (*vs_is_device)(int id, const char *name);
	int         (*vs_is_opt_available)(int id, int opt);
	const char *(*vs_device_name)(int id);
	int         (*boot_device)(void);
	int         (*nand_rescan_config)(struct cdbg_cfg_nand t[], int count);
	int         (*isi_check_header)(void *data);
	void        (*cdbg_shell)(void);
	int         (*ius_check_header)(struct ius_hdr *hdr);
	long        (*simple_strtol)(const char *cp,char **endp,unsigned int base);
	int         (*strncmp)(const char * cs,const char * ct,size_t count);
	char *      (*strncpy)(char * dest,const char *src,size_t count);
	void        (*cdbg_log_toggle)(int en);
	void		(*module_set_clock)(uint32_t base, int pll, int radio);
	uint32_t	(*module_get_clock)(uint32_t base);
	void		(*module_enable)(uint32_t base);
	void 		(*set_pll)(int pll, uint16_t value);
	void *      (*memset)(void * s,int c,size_t count);
	void        (*ss_jtag_en)(int en);
	void *      (*malloc)(size_t bytes);
	void        (*free)(void * mem);
	void *      (*nand_get_config)(int type);
	void        (*pads_chmod)(int index, int mode, int val);
        void        (*pads_pull)(int index, int en, int high);
	int        (*get_xom)(int mode);
	int	    (*hash_init)(int type, uint32_t len); 
	int	    (*hash_data)(void *buf, uint32_t len);           
	int	    (*hash_value)(void *buf);                        

};

extern struct irom_export irf_2885;
extern struct irom_export irf_2939;
extern struct irom_export irf_2974;
extern struct irom_export irf_cde7869;
extern struct irom_export irf_c31c431;
extern struct irom_export irf_4eadb12;
extern struct irom_export irf_876543;
extern struct irom_export irf_4e5a75;

