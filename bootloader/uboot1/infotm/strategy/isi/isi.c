
#include <common.h>
#include <isi.h>
#include <hash.h>
#include <irerrno.h>

int isi_check_header(void *data)
{
	uint32_t crc_ori, crc_calc;
	struct isi_hdr *hdr = data;
	/* check magic */
	if(hdr->magic != ISI_MAGIC) {
//		printf("magic do not match. 0x%x\n", hdr->magic);
		return -EBADMAGIC;
	}

	/* check hcrc */
	crc_ori = hdr->hcrc;
	hdr->hcrc = 0;

	hash_init(IUW_HASH_CRC, ISI_SIZE);
	hash_data(hdr, ISI_SIZE);
	hash_value(&crc_calc);

	/* restore hcrc */
	hdr->hcrc = crc_ori;

	if(crc_ori != crc_calc) {
//		printf("hcrc do not match. ori=0x%x, calc=0x%x\n",
//					crc_ori, crc_calc);
		return -EBADCRC;
	}

	printf("i: type (%d)\n", isi_get_type_image(hdr));
	printf("i: signature (%d)\n", isi_get_type_sig(hdr));
//	printf("header check passed.\n");
	return 0;
}

int isi_check_data_separate(struct isi_hdr *hdr, void * data)
{
	uint8_t sig[32];

	hash_init(isi_get_type_sig(hdr), isi_get_size(hdr));
	hash_data(data, isi_get_size(hdr));
	hash_value(sig);

	if(memcmp(sig, isi_get_sig(hdr), hash_len())) {
		printf("signature do not match, size: 0x%x\n", isi_get_size(hdr));
		print_buffer((ulong)isi_get_sig(hdr), isi_get_sig(hdr), 1, hash_len(), 32);
		print_buffer((ulong)sig, sig, 1, hash_len(), 32);
		return -EBADSIG;
	}

	return 0;
}

int isi_check_data(void *data) {
	struct isi_hdr *hdr = data;
	return isi_check_data_separate(hdr, isi_get_data(hdr));
}

int isi_check(void *data) {
	int ret;

	ret = isi_check_header(data);
	if(ret) return ret;

	return isi_check_data(data);
}

