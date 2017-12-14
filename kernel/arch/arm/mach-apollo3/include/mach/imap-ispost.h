#ifndef __IMAPX800_ISPOST_H__
#define __IMAPX800_ISPOST_H__


#if defined(__cplusplus)
extern "C" {
#endif


// ISPOST base address
#if defined(CONFIG_COMPILE_RTL) || defined(CONFIG_COMPILE_FPGA)
#define ISPOST_PERI_BASE_ADDR                  IMAP_ISPOST_BASE
#else
#define ISPOST_PERI_BASE_ADDR                  0x00000000
#endif



#if defined(__cplusplus)
}
#endif


#endif //__IMAPX800_ISPOST_H__
