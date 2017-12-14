#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <linux/types.h>
#include <configs/imapx800.h>

typedef enum {
	IMAGE_UBOOT = 0,
	IMAGE_ITEM = 1,
	IMAGE_RAMDISK = 2,
	IMAGE_KERNEL = 3,
	IMAGE_KERNEL_UPGRADE = 4,
	IMAGE_SYSTEM = 5,
	IMAGE_UBOOT1 = 16,
	IMAGE_RESERVED = 17,
	IMAGE_NONE = 18,
}image_type_t;

typedef struct image_header {
  uint32_t ih_magic;     // Image Header Magic Number
  uint32_t ih_hcrc;      // Image Header CRC Checksum
  uint32_t ih_time;      // Image Creation Timestamp
  uint32_t ih_size;      // Image Data Size
  uint32_t ih_load;      // Data  Load Address
  uint32_t ih_ep;        // Entry Point Address
  uint32_t ih_dcrc;      // Image Data CRC Checksum
  uint8_t  ih_os;        // Operating System
  uint8_t  ih_arch;      // CPU architecture
  uint8_t  ih_type;      // Image Type
  uint8_t  ih_comp;      // Compression Type
  uint8_t  ih_name[32];  // Image Name
} image_header_t;


#define UPGRADE_PARA_SIZE	0x400
#define RESERVED_ENV_SIZE	0xfc00
#define EMMC_IMAGE_OFFSET	0x200000
#define DEFAULT_ITEM_OFFSET 0xC000
#define DEFAULT_UBOOT1_OFFSET 0x20000
#define KERNEL_MAGIC	0x56190527
#define KERNEL_INNER_OFFS	64
#define FLAG_ADDR 0x21e09e30

#define TAG_START (CONFIG_SYS_SDRAM_BASE+0x100)
#define RAMDISK_START (CONFIG_SYS_SDRAM_BASE+(dram_size <<20)-0x400000)

#define CMDLINE_RECOVERY "console=ttyAMA3,115200 lpj=%d mem=%dM mode=%s"
#define CMDLINE_RECOVERY_LINKPC "console=ttyAMA3,115200 lpj=%d mem=%dM mode=%s cma=%s"
#define CMDLINE_SPI_FLASH "console=ttyAMA3,115200 lpj=%d mem=%dM rootfstype=%s root=/dev/spiblock1 rw"
//#define CMDLINE_SPI_FLASH "console=ttyAMA3,115200 lpi=%d mem=%dM mode=%s"
#define CMDLINE_SPI_NAND_FLASH "console=ttyAMA3,115200 lpj=%d mem=%dM rootfstype=ext4 root=/dev/spiblock1p1 rw"
#define CMDLINE_TF_CARD "console=ttyAMA3,115200 lpj=%d mem=%dM rootfstype=ext4 root=/dev/mmcblk0p2 rw init=/init"
#define CMDLINE_TF_CARD_EMMC "console=ttyAMA3,115200 lpj=%d mem=%dM rootfstype=ext4 root=/dev/mmcblk1p2 rw init=/init"
#define CMDLINE_EMMC "console=ttyAMA3,115200 lpj=%d mem=%dM rootfstype=ext4 root=/dev/mmcblk0p1 rw"

extern void int_endian_change(unsigned int *out, unsigned int in);
extern void setup_start_tag(void);
extern void setup_commandline_tag(char *commandline);
extern void setup_initrd_tag(ulong initrd_start, ulong initrd_end);
extern void setup_end_tag(void);

#endif
