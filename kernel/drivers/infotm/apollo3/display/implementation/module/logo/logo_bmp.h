#ifndef __LOGO_BMP_HEADER__
#define __LOGO_BMP_HEADER__

#define DATA_FROM_FILE 1
#define DATA_FROM_ONE_COLOR 0

enum __COLOR_BAR_TEST_CASE {
	COLOR_BAR_OSD0_RGB = 0,
	COLOR_BAR_OSD0_NV12 =1,
	COLOR_BAR_ODS1_RGB,
};

struct yuv_pixel {
	char y;
	char u;
	char v;
};

struct _bmp_logo{
	int type;
	int width;
	int height;
	u8* logo_base;
	u32 size;
};


void bmp_logo_init(int type);

/*
 * Fill the framebuffer with kernel booting logo.
 * Designed for MediaBox.
 * @fb_phyaddr: the virtual address of framebuffer
 */
int fill_framebuffer_logo(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height);
 int colorbar_fill_fb(unsigned char* fb0_viraddr, unsigned char* fb1_viraddr,int width, int height, int source);
 int ids_fill_dummy_yuv(unsigned int fb_viraddr, unsigned int  width, unsigned int	height, int idx);

#endif
