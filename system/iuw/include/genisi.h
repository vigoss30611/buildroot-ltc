#ifndef __GENISI_H__
#define __GENISI_H__


struct genisi_image_header {
	uint32_t magic;
    uint32_t type;
    uint32_t flag;
    uint32_t size;
    uint32_t load;
    uint32_t entry;
    uint32_t time;
	uint32_t hcrc;
	uint8_t  dcheck[32];
	char	name[192];
	uint8_t  extra[256];
};

struct image_header {
	uint32_t    ih_magic;
   	uint32_t    ih_hcrc;
   	uint32_t    ih_time;
   	uint32_t    ih_size;
    uint32_t    ih_load;
    uint32_t    ih_ep;
    uint32_t    ih_dcrc;
    uint8_t     ih_os;
    uint8_t     ih_arch;
    uint8_t     ih_type;
    uint8_t     ih_comp;
    uint8_t     ih_name[32];
};


struct file_header {
    uint32_t type_sector;
    uint32_t addr0;
    uint32_t addr1;
    uint32_t length;
};

struct wrap_header {
    uint32_t ius_magic;
    uint32_t flag;
    uint32_t hw_version0;
    uint32_t hw_version1;
    uint32_t sw_version0;
    uint32_t sw_version1;
    uint32_t hcrc;
    uint32_t count;
	struct file_header file[30];
};

struct hd_geometry {
    unsigned char heads;
    unsigned char sectors;
    unsigned short cylinders;
    unsigned long start;
};




/*
 * The function can be extract the ius package, or with the
 * help of header, you can know any information about the ius.
 * @header: the struct of the header about  the ius package.
 * @fd: the descriptor of the file.
 * @extract_path: the path you extract the ius package to
 *	if you want to extract, you need to put the is_write = 1;
 *	else you can input extract = NULL.
 * @is_write: flag, to ensure you want to extract.
 * return 0, if success.
 * return -1, if failed.
 */

extern int check_ius(struct wrap_header *header, int fd, char *extract_path, int is_write);
extern int genisi(int argc, char * argv[]);

		
#endif


