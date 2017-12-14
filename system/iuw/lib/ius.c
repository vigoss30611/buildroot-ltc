#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <time.h>
#include <types.h>
#include <genisi.h>
#include <listparser.h>
#include <hash.h>
#include <ixconfig.h>
#include <ius.h>

int ius_check_header(struct ius_hdr *hdr)
{
	uint32_t crc_ori, crc_calc;

	/* check magic */
	if(hdr->magic != IUS_MAGIC) {
		printf("magic do not match. 0x%lx\n", hdr->magic);
		return -1;
	}

#if 0
	if(cdbg_verify_burn()) {
		if(!(hdr->flag & IUS_FLAG_ENCRYPT)) {
			printf("untrusted IUS\n");
			return -EUNTRUST;
		}
		ss_aes_init(DECRYPTION, cdbg_verify_burn_key(), __irom_ver);
		ss_aes_data(hdr->swstr, hdr->swstr, sizeof(*hdr) - 16);
	}
#endif

	/* check hcrc */
	crc_ori = hdr->hcrc;
	hdr->hcrc = 0;

	hash_init(IUW_HASH_CRC, sizeof(struct ius_hdr));
	hash_data((void *)hdr, sizeof(struct ius_hdr));
	hash_value((void *)&crc_calc);

	/* restore hcrc */
	hdr->hcrc = crc_ori;

	if(crc_ori != crc_calc) {
		printf("hcrc do not match. ori=0x%lx, calc=0x%lx\n",
					crc_ori, crc_calc);
		return -3;
	}

//	printf("header check passed.\n");
	return 0;
}

