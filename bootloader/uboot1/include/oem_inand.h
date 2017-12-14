#ifndef _OEM_iNAND
#define _OEM_iNAND



struct part_table{
	char bootableflag;
	char startCHS[3];
	char parttype;
	char endCHS[3];
	unsigned int startLBA;
	unsigned int sizeinsectors;
};

extern int oem_get_imagebase(void);
extern int oem_get_systembase(void);
extern int oem_get_hibernatebase(void);
extern int oem_partition(void);
extern int oem_read_inand(char *membase, int startblk, int cnt);
extern int oem_write_inand(char *membase, int startblk, int cnt);
#endif
