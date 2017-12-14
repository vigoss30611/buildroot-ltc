
#include <common.h>
#include <isi.h>

int isi_check_header(void *data)
{
	uint32_t crc_ori, crc_calc;
	struct isi_hdr *hdr = data;
	/* check magic */
	if(hdr->magic != ISI_MAGIC) {
//		printf("magic do not match. 0x%x\n", hdr->magic);
		return -1;
	}

	/* check hcrc */
	crc_ori = hdr->hcrc;
	hdr->hcrc = 0;

	crc_calc = crc32(0, hdr, ISI_SIZE);

	/* restore hcrc */
	hdr->hcrc = crc_ori;

	if(crc_ori != crc_calc) {
//		printf("hcrc do not match. ori=0x%x, calc=0x%x\n",
//					crc_ori, crc_calc);
		return -2;
	}

	printf("i: type (%d)\n", isi_get_type_image(hdr));
	printf("i: signature (%d)\n", isi_get_type_sig(hdr));
//	printf("header check passed.\n");
	return 0;
}


