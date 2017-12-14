#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <dss_common.h>

#include "kernel_logo.h"

#define DSS_LAYER   "[display]"
#define DSS_SUB1    "[implementation]"
#define DSS_SUB2    "[logo]"

unsigned int combine_info(int offset, int len)
{
	unsigned int var = 0;
	switch(len) {
		case 4:
			var |= kernel_logo_bmp[offset+3] << 24;
		case 3:
			var |= kernel_logo_bmp[offset+2] << 16;
		case 2:
			var |= kernel_logo_bmp[offset+1] << 8;
		case 1:
			var |= kernel_logo_bmp[offset] << 0;
			break;
		default:
			dss_err("Invalid len = %d.\n", len);
			break;
	}

	return var;
}

unsigned int combine_rgb(unsigned char blue, unsigned char green, unsigned char red)
{
	/* Usually RGB sequence is const */
	unsigned int rgb24 = 0;
	rgb24 |= red << 16;
	rgb24 |= green << 8;
	rgb24 |= blue << 0;
	return rgb24;
}


int fill_framebuffer_logo(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height)
{
	int bf_type, bf_size, bf_offbits, bi_width, bi_height, bi_bitcount, bi_compression, bi_sizeimage, rgb_align; 
	int i, j, seq, pix;
	unsigned char * logo_rgb;
	dma_addr_t tmp;
	unsigned int fb_width;
		
	/* Assume 24bits color depth. No compression. */
	dss_info("Filling framebuffer with kernel booting logo...\n");

	bf_type = combine_info(0x00, 2);	// file type
	bf_size = combine_info(0x02, 4);	// file size
	bf_offbits = combine_info(0x0A, 4);	// rgb data offset
	bi_width = combine_info(0x12, 4);	// image width
	bi_height = combine_info(0x16, 4);	// image height
	bi_bitcount = combine_info(0x1C, 2);	// color depth
	bi_compression = combine_info(0x1E, 4);	// compression
	bi_sizeimage = combine_info(0x22, 4);	// image size, after aligned with 4 per line
	rgb_align = (bi_width * 3 + 3) & (~0x3);	/* align with 4 */

	dss_dbg("bf_type = 0x%04X, bf_size = %d, bf_offbits = %d, bi_width = %d, bi_height = %d\n"
			"bi_bitcount = %d, bi_compression = %d, bi_sizeimage = %d, rgb_align = %d.\n", 
			bf_type, bf_size, bf_offbits, bi_width, bi_height,
			bi_bitcount, bi_compression, bi_sizeimage, rgb_align);

	dss_info("Logo bmp size: %d * %d\n", bi_width, bi_height);

	*bmp_width = bi_width;
	*bmp_height = bi_height;
	fb_width = bi_width;
		
	// Check whether valid
	if (bf_type != 0x4D42) {
		dss_err("\"kernel_logo.bmp\" is not a valid bmp file.\n");
		goto out;
	}
	if (bf_size <= bi_sizeimage) {
		dss_err("bf_size is no bigger than bi_sizeimage.\n");
		goto out;
	}
	if (bf_offbits != 54) {
		dss_info("Special bmp file: rgb data offset = %d\n", bf_offbits);
	}
	if (bi_height < 0) {
		dss_err("bi_height = %d. Dose not support this kind of special bmp file.\n", bi_height);
		goto out;
	}
	if (bi_bitcount != 24) {
		dss_err("bmp color depth = %d. Currently only supprt 24 bits color depth.\n", bi_bitcount);
		goto out;
	}
	if (bi_compression != 0) {
		dss_err("bmp file has compression %d. Currently not supported.\n", bi_compression);
	}
	if (bi_sizeimage < (bi_width * bi_height * bi_bitcount /8)) {
		dss_err("bi_sizeimage is smaller than (bi_width * bi_height * bi_bitcount /8)\n");
		goto out;
	}
	if (rgb_align != (bi_sizeimage/bi_height)) {
		dss_err("rgb_align does not equal to (bi_sizeimage/bi_height).\n");
		goto out;
	}

	dss_dbg("fb_viraddr = 0x%X\n", fb_viraddr);
	
	logo_rgb = &kernel_logo_bmp[bf_offbits];

	for (i  = bi_height-1; i >= 0; i--) {
		for (j = 0; j < bi_width; j++) {
			pix = (bi_height-1-i)  * fb_width + j;			
			seq =  i * rgb_align + j*3;
			*((volatile unsigned int *)fb_viraddr + pix) = 
			//	combine_rgb(logo_rgb[seq], logo_rgb[seq+1], logo_rgb[seq+2]);
				combine_rgb(logo_rgb[seq+2], logo_rgb[seq+1], logo_rgb[seq]);
			wmb();
		}
	}

	/* Write cache to memory */
	tmp = dma_map_single(NULL, (void *)fb_viraddr, bi_width * bi_height * 4, DMA_TO_DEVICE);
	dma_unmap_single(NULL, tmp, bi_width * bi_height * 4, DMA_TO_DEVICE);

	return 0;
	
out:
	return -EINVAL;	
}
EXPORT_SYMBOL(fill_framebuffer_logo);

