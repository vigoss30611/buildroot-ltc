#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <dss_common.h>

#include "logo_bmp.h"

#ifdef TEST_LOGO_360_240_BMP
#include "kernel_logo_360_240.h"
#endif

#ifdef TEST_LOGO_720_576_BMP
#include "kernel_logo_720_576.h"
#endif

#ifdef TEST_LOGO_800_480_BMP
#include "kernel_logo_800_480.h"
#endif

#ifdef TEST_LOGO_1024_600_BMP
#include "kernel_logo_1024_576.h"
#endif

#define DSS_LAYER   "[display]"
#define DSS_SUB1    "[dss]"
#define DSS_SUB2    "[logo]"

static struct yuv_pixel color[6] = {
	{178, 135, 169},
	{76,111,119},
	{184,145,18},
	{128,182,47},
	{199,109,149},
	{178,105,116}
};

static struct _bmp_logo logo;
static int init = 0;

void bmp_logo_init(int type)
{
	logo.type = type;

	switch(logo.type) {
	case LOGO_360_240:
#ifdef TEST_LOGO_360_240_BMP
		logo.logo_base = __360_240_bmp;
		logo.width = 360;
		logo.height = 240;
		logo.size = __360_240_bmp_len;
		init = 1;
#else
		logo.logo_base = NULL;
		logo.width = 360;
		logo.height = 240;
		logo.size = (360*240*4);
		init = 1;
#endif
		break;

	case LOGO_720_576:
#ifdef TEST_LOGO_720_576_BMP
		dss_info("init logo bmp 720 * 576\n");
		logo.logo_base = __720_576_bmp;
		logo.width = 720;
		logo.height = 576;
		logo.size = __720_576_bmp_len;
		init = 1;
#else
		dss_info("init logo bmp 720 * 576\n");
		logo.logo_base = NULL;
		logo.width = 720;
		logo.height = 576;
		logo.size = (720*576*4);
		init = 1;
#endif
		break;

	case LOGO_800_480:
#ifdef TEST_LOGO_800_480_BMP
		dss_info("init logo bmp 800 * 480\n");
		logo.logo_base = __800_480_bmp;
		logo.width = 800;
		logo.height = 480;
		logo.size = __800_480_bmp_len;
		init = 1;
#else
		dss_info("init logo bmp 800 * 480\n");
		logo.logo_base = NULL;
		logo.width = 800;
		logo.height = 480;
		logo.size = (800*480*4);
		init = 1;
#endif
		break;
#if 0
	case LOGO_1024_600:
		logo.logo_base = kernel_logo_1024_576_bmp;
		logo.width = 1024;
		logo.height = 600;
		logo.size = kernel_logo_1024_576_bmp_len;
		init = 1;
		break;
#endif
	default:
		break;
	}
}
EXPORT_SYMBOL(bmp_logo_init);

static unsigned int combine_info(const unsigned char* logo ,int offset, int len)
{
	unsigned int var = 0;
	switch(len) {
		case 4:
			var |= logo[offset+3] << 24;
		case 3:
			var |= logo[offset+2] << 16;
		case 2:
			var |= logo[offset+1] << 8;
		case 1:
			var |= logo[offset] << 0;
			break;
		default:
			dss_err("Invalid len = %d.\n", len);
			break;
	}

	return var;
}

static unsigned int combine_rgb(unsigned char blue, unsigned char green, unsigned char red)
{
	/* Usually RGB sequence is const */
	unsigned int rgb24 = 0;
	rgb24 |= red << 16;
	rgb24 |= green << 8;
	rgb24 |= blue << 0;
	return rgb24;
}

#if 0
static void fill_one_color_to_raw(u8*addr, int w, int h, int a, u8 r, u8 g, u8 b)
{
	int i, j;
	for(i= 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			*addr++ = b;
			*addr++ = g;
			*addr++ = r;
			*addr++ = a;
			wmb();
		}
	}
}

static int fill_framebuffer_logo_raw(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height, int source)
{
	dma_addr_t tmp;

	if (!init) {
		dss_err("filling framebuffer without init !\n");
		return -1;
	}

	/* Assume 24bits color depth. No compression. */
	dss_info("Filling framebuffer with kernel booting logo...\n");

	dss_dbg("fb_viraddr = 0x%X\n", fb_viraddr);
	if (source) {
		if (!logo.logo_base)
			goto err;

		memcpy((void*)fb_viraddr, (void*)logo.logo_base, logo.size);
		wmb();

	} else {
		dss_info("fb_viradrr = 0x%x, width = %d, height = %d\n", (u32)fb_viraddr, logo.width, logo.height);
		fill_one_color_to_raw((u8*)fb_viraddr, logo.width, logo.height, 0xff, 0xff, 0x00, 0x00);
		//memset((u8*)fb_viraddr, 0xff, logo.width*logo.height*4);
		wmb();
	}

	/* Write cache to memory */
	tmp = dma_map_single(NULL, (void *)fb_viraddr, logo.size, DMA_TO_DEVICE);
	dma_unmap_single(NULL, tmp, logo.size, DMA_TO_DEVICE);

	return 0;
err:
	return -1;
}
#endif

static int fill_framebuffer_logo_bmp(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height)
{
	int bf_type, bf_size, bf_offbits, bi_width, bi_height, bi_bitcount, bi_compression, bi_sizeimage, rgb_align; 
	int i, j, seq, pix;
	const unsigned char * logo_rgb;
	dma_addr_t tmp;
	unsigned int fb_width;

	if (!init) {
		dss_err("filling framebuffer without init !\n");
		goto out;
	}

	/* Assume 24bits color depth. No compression. */
	dss_info("Filling framebuffer with kernel booting logo...\n");

	if (!logo.logo_base)
		goto out;

	logo_rgb = logo.logo_base;
	dss_dbg("logo_rgb = 0x%x...\n", (u32)logo_rgb);

	bf_type = combine_info(logo_rgb, 0x00, 2);	// file type
	bf_size = combine_info(logo_rgb, 0x02, 4);	// file size
	bf_offbits = combine_info(logo_rgb, 0x0A, 4);	// rgb data offset
	bi_width = combine_info(logo_rgb, 0x12, 4);	// image width
	bi_height = combine_info(logo_rgb, 0x16, 4);	// image height
	bi_bitcount = combine_info(logo_rgb, 0x1C, 2);	// color depth
	bi_compression = combine_info(logo_rgb, 0x1E, 4);	// compression
	bi_sizeimage = combine_info(logo_rgb, 0x22, 4);	// image size, after aligned with 4 per line
	rgb_align = (bi_width * 3 + 3) & (~0x3);	/* align with 4 */

	dss_dbg("bf_type = 0x%04X, bf_size = %d, bf_offbits = %d, bi_width = %d, bi_height = %d\n"
			"bi_bitcount = %d, bi_compression = %d, bi_sizeimage = %d, rgb_align = %d.\n", 
			bf_type, bf_size, bf_offbits, bi_width, bi_height,
			bi_bitcount, bi_compression, bi_sizeimage, rgb_align);

	dss_info(" Logo bmp size: %d * %d\n", bi_width, bi_height);

	//*bmp_width = bi_width;
	//*bmp_height = bi_height;
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

	/* logo_rgb = &kernel_logo_bmp[bf_offbits]; */
	logo_rgb = logo_rgb + bf_offbits;

	for (i  = bi_height-1; i >= 0; i--) {
		for (j = 0; j < bi_width; j++) {
			pix = (bi_height-1-i)  * fb_width + j;
			seq =  i * rgb_align + j*3;
			*((volatile unsigned int *)fb_viraddr + pix) = 
			//combine_rgb(logo_rgb[seq], logo_rgb[seq+1], logo_rgb[seq+2]); /* BGR*/
			combine_rgb(logo_rgb[seq+2], logo_rgb[seq+1], logo_rgb[seq]); /*RGB*/
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

static int inline ids_palette(int i, int w,int f_index) 
{
	i += f_index * 96;
	return ((i % w) / 96 + ((i / w / 135) & 1) * 3) % 6;
}

static int ids_fill_buffer(char *buf, int w, int h,int index)
{
	int i, j, pixels =w * h;
	char *y = buf;
	short *uv = (short *)(buf + pixels);

	for(i = 0; i < pixels; i++)
		y[i] = color[ids_palette(i, w, index)].y;

	for(i = 0; i < pixels >> 2; i++) {
		j = 4 * i / w * w+ i % (w/ 2) * 2;
		uv[i] = color[ids_palette( j,w,  index)].v;
		uv[i] <<= 8;
		uv[i] |= color[ids_palette( j, w, index)].u;
	}

	return 0;
}

int ids_fill_dummy_yuv(unsigned int fb_viraddr, unsigned int  width, unsigned int  height, int idx)
{
	dma_addr_t tmp;
	/* Assume 24bits color depth. No compression. */

	static char yuv_buffer[1024*600*2] = {0}; // TODO

	dss_info("Logo yuv size: %d * %d\n", width, height);

	ids_fill_buffer(yuv_buffer, width, height,idx);
	memcpy((void* )fb_viraddr, (void*)yuv_buffer, width*height*2);
	wmb();

	/* Write cache to memory */
	tmp = dma_map_single(NULL, (void *)fb_viraddr, width * height * 2, DMA_TO_DEVICE);
	dma_unmap_single(NULL, tmp, width * height * 2, DMA_TO_DEVICE);

	return 0;
}

static int rgb24_to_yuv420sp(int w,int h,unsigned char *rgb,unsigned char *yuv)
{
	unsigned char *u,*v,*y,*uu,*vv;
	unsigned char *pu1,*pu2,*pu3,*pu4;
	unsigned char *pv1,*pv2,*pv3,*pv4;
	unsigned char *r,*g,*b;
	unsigned char *uv;
	int i,j;

	y=yuv;
	u= uu = yuv + 2* w*h;
	v = vv=yuv + 3* w*h;

	// Get r,g,b pointers from bmp image data....
	r=rgb;
	g=rgb+1;
	b=rgb+2;

	//Get YUV values for rgb values
	for(i=0;i<h;i++) {
		for(j=0;j<w;j++) {
			*y = (unsigned char)( ( 66 * (*r) + 129 * (*g) + 25 * (*b) + 128) >> 8) + 16 ;
			*y = (unsigned char)( (*y<0) ? 0 : ((*y>255) ? 255 :* y) );
			y++;
			*u++ = (unsigned char)( ( -38 * (*r) - 74 * (*g) + 112 * (*b) + 128) >> 8) + 128 ;
			*v++ = (unsigned char)( ( 112 * (*r) - 94 * (*g) - 18 * (*b) + 128) >> 8) + 128 ;
			r+=3;
			g+=3;
			b+=3;
		}
	}

	// Now sample the U & V to obtain YUV 4:2:0 format
	// Sampling mechanism...
	/* @ -> Y
	# -> U or V
	@ @ @ @
	# #
	@ @ @ @
	@ @ @ @
	# #
	@ @ @ @
	*/

	// Get the right pointers
	uv=yuv+w*h;

	// For U
	pu1=uu;
	pu2=pu1+1;
	pu3=pu1+w;
	pu4=pu3+1;

	// For V
	pv1=vv;
	pv2=pv1+1;
	pv3=pv1+w;
	pv4=pv3+1;

	// Do sampling
	for(i=0;i<h;i+=2) {
		for(j=0;j<w;j+=2) {
			*uv++=(*pu1+*pu2+*pu3+*pu4)>>2;
			*uv++=(*pv1+*pv2+*pv3+*pv4)>>2;
			pu1+=2;
			pu2+=2;
			pu3+=2;
			pu4+=2;

			pv1+=2;
			pv2+=2;
			pv3+=2;
			pv4+=2;
		}
		pu1+=w;
		pu2+=w;
		pu3+=w;
		pu4+=w;
		pv1+=w;
		pv2+=w;
		pv3+=w;
		pv4+=w;
	}

	return 0;
}

static void gen_rgb24_colorbar(int width, int height, unsigned char* data)
{
	int barwidth;
	int i=0,j=0;

	//Check
	if(width<=0||height<=0){
		dss_err("Error: Width, Height cannot be 0 or negative number!\n");
		dss_err("Default Param is used.\n");
		width=640;
		height=480;
	}
	if(width%8!=0)
		dss_err("Warning: Width cannot be divided by Bar Number without remainder!\n");

	barwidth=width/8;

	for(j=0;j<height;j++){
		for(i=0;i<width;i++){
			int barnum=i/barwidth;
			switch(barnum){
			case 0:{
				data[(j*width+i)*3+0]=255; /*R*/
				data[(j*width+i)*3+1]=255; /*G*/
				data[(j*width+i)*3+2]=255;/*B*/
				break;
			}
			case 1:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=0;
				break;
			}
			case 2:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=255;
				break;
			}
			case 3:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=255;
				data[(j*width+i)*3+2]=0;
				break;
			}
			case 4:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=255;
				break;
			}
			case 5:{
				data[(j*width+i)*3+0]=255;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=0;
				break;
			}
			case 6:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=255;
				break;
			}
			case 7:{
				data[(j*width+i)*3+0]=0;
				data[(j*width+i)*3+1]=0;
				data[(j*width+i)*3+2]=0;
				break;
			}
		}
		}
	}
}

static void gen_argb32_colorbar(int width, int height, unsigned char* data)
{
	int barwidth;
	int i=0,j=0;

	//Check
	if(width<=0||height<=0){
		dss_err("Error: Width, Height cannot be 0 or negative number!\n");
		dss_err("Default Param is used.\n");
		width=640;
		height=480;
	}
	if(width%8!=0)
		dss_err("Warning: Width cannot be divided by Bar Number without remainder!\n");

	barwidth=width/8;

	for(j=0;j<height;j++){
		for(i=0;i<width;i++){
			int barnum=i/barwidth;
			switch(barnum){
			case 0:{
				data[(j*width+i)*4+0]=255;
				data[(j*width+i)*4+1]=255;
				data[(j*width+i)*4+2]=255;
				data[(j*width+i)*4+3]=255;
				break;
			}
			case 1:{
				data[(j*width+i)*4+0]=255;
				data[(j*width+i)*4+1]=255;
				data[(j*width+i)*4+2]=0;
				data[(j*width+i)*4+3]=255;
				break;
			}
			case 2:{
				data[(j*width+i)*4+0]=0;
				data[(j*width+i)*4+1]=255;
				data[(j*width+i)*4+2]=255;
				data[(j*width+i)*4+3]=255;
				break;
			}
			case 3:{
				data[(j*width+i)*4+0]=0;
				data[(j*width+i)*4+1]=255;
				data[(j*width+i)*4+2]=0;
				data[(j*width+i)*4+3]=255;
				break;
			}
			case 4:{
				data[(j*width+i)*4+0]=255;
				data[(j*width+i)*4+1]=0;
				data[(j*width+i)*4+2]=255;
				data[(j*width+i)*4+3]=255;
				break;
			}
			case 5:{
				data[(j*width+i)*4+0]=255;
				data[(j*width+i)*4+1]=0;
				data[(j*width+i)*4+2]=0;
				data[(j*width+i)*4+3]=255;
				break;
			}
			case 6:{
				data[(j*width+i)*4+0]=0;
				data[(j*width+i)*4+1]=0;
				data[(j*width+i)*4+2]=255;
				data[(j*width+i)*4+3]=255;
				break;
			}
			case 7:{
				data[(j*width+i)*4+0]=0;
				data[(j*width+i)*4+1]=0;
				data[(j*width+i)*4+2]=0;
				data[(j*width+i)*4+3]=255;
				break;
			}
			}
		}
	}
}

static void gen_yuv420sp_colorbar(int width, int height, unsigned char* yuv, unsigned char* rgb)
{
	//Check
	if(width<=0||height<=0){
		dss_err("Error: Width, Height cannot be 0 or negative number!\n");
		dss_err("Default Param is used.\n");
		width=640;
		height=480;
	}
	if(width%8!=0)
		dss_err("Warning: Width cannot be divided by Bar Number without remainder!\n");

	gen_rgb24_colorbar(width, height, rgb);
	rgb24_to_yuv420sp(width,height,rgb, yuv);
}

int colorbar_fill_fb(unsigned char* fb0_viraddr, unsigned char* fb1_viraddr,
			int width, int height, int source)
{
	dma_addr_t da;

	/* Assume 24bits color depth. No compression. */
	dss_info("Filling framebuffer with kernel\n"); 
	if (source == COLOR_BAR_OSD0_NV12) {
		if (!fb0_viraddr || !fb0_viraddr)
			return -1;

		gen_yuv420sp_colorbar(width, height, fb0_viraddr, fb1_viraddr);
		wmb();
	} else {
		gen_argb32_colorbar(width, height, fb0_viraddr);
		wmb();
	}

	/* Write cache to memory */
	da = dma_map_single(NULL, (void *)fb0_viraddr, width * height * 4, DMA_TO_DEVICE);
	dma_unmap_single(NULL, da, width * height * 4, DMA_TO_DEVICE);

	return 0; 
}

int fill_framebuffer_logo(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height)
{
	if (logo.type == LOGO_360_240) {
#if defined(TEST_LOGO_360_240_BMP)
	return fill_framebuffer_logo_bmp(fb_viraddr, bmp_width, bmp_height);
#else
	return colorbar_fill_fb((unsigned char*)fb_viraddr, NULL,logo.width, logo.height, 0);
#endif
	}

	if (logo.type == LOGO_720_576) {
#if defined(TEST_LOGO_720_576_BMP)
	return fill_framebuffer_logo_bmp(fb_viraddr, bmp_width, bmp_height);
#else
	return colorbar_fill_fb((unsigned char*)fb_viraddr, NULL,logo.width, logo.height, 0);
#endif
	}

	if (logo.type == LOGO_800_480) {
#if defined(TEST_LOGO_800_480_BMP)
	return fill_framebuffer_logo_bmp(fb_viraddr, bmp_width, bmp_height);
#else
	return colorbar_fill_fb((unsigned char*)fb_viraddr, NULL,logo.width, logo.height, 0);
#endif
	}

	return 0;
}
EXPORT_SYMBOL(fill_framebuffer_logo);
