/* display register access driver */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/display_device.h>
#include <linux/io.h>


#define DISPLAY_MODULE_LOG		"logo"


#ifdef CONFIG_DISPLAY2_LOGO1
#include "logo1.h"
#endif
#ifdef CONFIG_DISPLAY2_LOGO2
#include "logo2.h"
#endif
#ifdef CONFIG_DISPLAY2_LOGO3
#include "logo3.h"
#endif
#ifdef CONFIG_DISPLAY2_LOGO4
#include "logo4.h"
#endif
#ifdef CONFIG_DISPLAY2_LOGO5
#include "logo5.h"
#endif

static unsigned int *__get_logo_data(int *width, int *height)
{
#ifdef CONFIG_DISPLAY2_LOGO1
	*width = logo1_width;
	*height = logo1_height;
	return logo1_data;
#endif
#ifdef CONFIG_DISPLAY2_LOGO2
	*width = logo2_width;
	*height = logo2_height;
	return logo2_data;
#endif
#ifdef CONFIG_DISPLAY2_LOGO3
	*width = logo3_width;
	*height = logo3_height;
	return logo3_data;
#endif
#ifdef CONFIG_DISPLAY2_LOGO4
	*width = logo4_width;
	*height = logo4_height;
	return logo4_data;
#endif
#ifdef CONFIG_DISPLAY2_LOGO5
	*width = logo5_width;
	*height = logo5_height;
	return logo5_data;
#endif
	*width = 0;
	*height = 0;
	return NULL;
}

int kernel_logo(void)
{
#ifdef CONFIG_DISPLAY2_NOLOGO
	return 0;
#else
	return 1;
#endif
}

int logo_get_width(void)
{
	int width, height;
	__get_logo_data(&width, &height);
	return width;
}

int logo_get_height(void)
{
	int width, height;
	__get_logo_data(&width, &height);
	return height;
}

int fill_logo_buffer(unsigned int buffer_addr)
{
	int i, j, pix;
	unsigned int * logo_data;
	int width, height;

	logo_data = __get_logo_data(&width, &height);
	if (!logo_data) {
		dlog_err("failed to get logo data\n");
		return -EINVAL;
	}

	dlog_dbg("buffer_addr = 0x%X, phys_addr = 0x%X, width = %d, height = %d\n",
						buffer_addr, virt_to_phys((void *)buffer_addr), width, height);

	for (i  = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			pix =  i * width + j;
			*((volatile unsigned int *)buffer_addr+pix) = logo_data[pix];
		}
		wmb();
	}

	return 0;
}


MODULE_AUTHOR("feng.qu@infotm.com");
MODULE_DESCRIPTION("display logo driver");
MODULE_LICENSE("GPL");
