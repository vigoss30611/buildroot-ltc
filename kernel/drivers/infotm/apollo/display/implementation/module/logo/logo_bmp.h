#ifndef __LOGO_BMP_HEADER__
#define __LOGO_BMP_HEADER__

/*
 * Fill the framebuffer with kernel booting logo.
 * Designed for MediaBox.
 * @fb_phyaddr: the virtual address of framebuffer
 */
int fill_framebuffer_logo(unsigned int fb_viraddr, unsigned int * bmp_width, unsigned int * bmp_height);


#endif
