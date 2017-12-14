
#include <common.h>
#include <ius.h>
#include <hash.h>
#include <crypto.h>
#include <cdbg.h>
#include <irerrno.h>

int ius_check_header(struct ius_hdr *hdr)
{
	uint32_t crc_ori, crc_calc;

	/* check magic */
	if(hdr->magic != IUS_MAGIC) {
		printf("magic do not match5. 0x%x\n", hdr->magic);
		return -1;
	}

	if(cdbg_verify_burn()) {
		if(!(hdr->flag & IUS_FLAG_ENCRYPT)) {
			printf("untrusted IUS\n");
			return -EUNTRUST;
		}
		ss_aes_init(DECRYPTION, cdbg_verify_burn_key(), __irom_ver);
		ss_aes_data(hdr->swstr, hdr->swstr, sizeof(*hdr) - 16);
	}

	/* check hcrc */
	crc_ori = hdr->hcrc;
	hdr->hcrc = 0;

	hash_init(IUW_HASH_CRC, sizeof(struct ius_hdr));
	hash_data(hdr, sizeof(struct ius_hdr));
	hash_value(&crc_calc);

	/* restore hcrc */
	hdr->hcrc = crc_ori;

	if(crc_ori != crc_calc) {
		printf("hcrc do not match. ori=0x%x, calc=0x%x\n",
					crc_ori, crc_calc);
		return -EBADCRC;
	}

	printf("header check passed.\n");
	return 0;
}

int ius_count_size(struct ius_hdr *hdr)
{
    int last = ius_get_count(hdr) - 1;
    struct ius_desc *desc = ius_get_desc(hdr, last);

    return ((desc->sector & 0xffffff) << 9)
        + ius_desc_length(desc);
}

