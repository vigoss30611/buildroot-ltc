#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fr/libfr.h>

static int debug = 0;
#define SAVE_YCBCR	0

#define dlog_dbg(format, arg...)	\
({if(debug) printf("[bmp2fb] dbg: " format, ## arg);0;})

#define dlog_info(format, arg...)	\
({printf("[bmp2fb] info: " format, ## arg);0;})

#define dlog_err(format, arg...)	\
({printf("[bmp2fb] error: " format, ## arg);0;})

static char cmdbuf[128] = {0};

static int div_round(int n, int base)
{
	int div, mod;

	div = n/base;
	mod = n%base;
	if (mod*2>=base)
		div++;
	return div;
}

/* formula: BT601 full range */
static int __rgb2y(int r, int g, int b)
{
	int y, tmp;

	tmp = 257*r+504*g+98*b+16000;
	y = div_round(tmp, 1000);
	if (y > 255) {
		dlog_err("y=%d is larger than 255\n", y);
		return -EINVAL;
	}

	return y;
}

static int __rgb2cbcr(int r, int g, int b, int *cbp, int *crp)
{
	int cb, cr, tmp;

	tmp = 128000-148*r-291*g+439*b;
	cb = div_round(tmp, 1000);
	tmp = 128000+439*r-368*g-71*b;
	cr = div_round(tmp, 1000); 

	if (cb > 255 || cr > 255) {
		dlog_err("cb=%d, cr=%d is larger than 255\n", cb, cr);
		return -EINVAL;
	}

	*cbp = cb;
	*crp = cr;
	return 0;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int overlay;
	int scale;
	unsigned int alpha=0xFFFFFF;
	char * buffer;
	int format = 0;
	char * line;
	unsigned int phys_addr;
	unsigned char buf=0;
	int width=0, height=0, rgb_align;
	int i, j;
	int r, g, b, cb=0, cr=0;
	int bytes_per_pixel;

	if (argc != 5 && argc != 6 && argc != 7) {
		goto usage;
	}

	overlay = strtoul(argv[2], NULL, 10);

	scale = strtoul(argv[3], NULL, 0);
	if (scale < 0 || scale > 4) {
		dlog_err("Invalid scale param %d\n", scale);
		return -EINVAL;
	}

	format = strtoul(argv[4], NULL, 10);

	if (argc >= 6)
		alpha = strtoul(argv[5], NULL, 16);

	if (argc >= 7)
		debug = strtoul(argv[6], NULL, 10);

	fp = fopen(argv[1], "rb");
	if (!fp) {
		dlog_err("can NOT open file %s\n", argv[1]);
		return -ENOENT;
	}

	/* parse width */
	fseek(fp, 18, SEEK_SET);
	for (i = 0; i < 4; i++) {
		fread(&buf, 1, 1, fp);
		width+=buf<<(i*8);
	}
	rgb_align = width*3+3;
	rgb_align /= 4;
	rgb_align *= 4;

	dlog_dbg("rgb_align=%d\n", rgb_align);

	/* parse height */
	fseek(fp, 22, SEEK_SET);
	for (i = 0; i < 4; i++) {
		fread(&buf, 1, 1, fp);
		height+=buf<<(i*8);
	}

	line = malloc(rgb_align*2);
	if (!line) {
		dlog_err("No memory for line\n");
		return -ENOMEM;
	}

	dlog_dbg("filling logo buffer...\n");

	if (format == 0 || format == 1 || format == 2) {
		if (format == 0 || format == 1)
			bytes_per_pixel = 4;
		else
			bytes_per_pixel = 2;
		buffer = fr_alloc_single("bmp2fb", width*height*bytes_per_pixel, &phys_addr);
		if (!buffer) {
			dlog_err("fr_alloc_single failed\n");
			return -ENOMEM;
		}
		for (i = 0; i < height; i++) {
			fseek(fp, 54+rgb_align*(height-1-i), SEEK_SET);
			fread(line, 1, rgb_align, fp);
			for (j = 0; j < width; j++) {
				if (format == 0) {
					buffer[(width*4*i)+j*4+3] = 0;				// a
					buffer[(width*4*i)+j*4+2] = line[j*3+2];	// r
					buffer[(width*4*i)+j*4+1] = line[j*3+1];	// g
					buffer[(width*4*i)+j*4+0] = line[j*3+0];	// b
				}
				else if (format == 1) {
					buffer[(width*4*i)+j*4+3] = 0;				// a
					buffer[(width*4*i)+j*4+2] = line[j*3+0];	// b
					buffer[(width*4*i)+j*4+1] = line[j*3+1];	// g
					buffer[(width*4*i)+j*4+0] = line[j*3+2];	// r
				}
				else {
					r = line[j*3+2] >> 3;
					g = line[j*3+1] >> 2;
					b = line[j*3+0] >> 3;
					/* for better accuracy */
					if ((line[j*3+2]>>2)&0x01)
						r=(r>=31?31:r+1);
					if ((line[j*3+1]>>1)&0x01)
						g=(g>=63?63:g+1);
					if ((line[j*3+0]>>2)&0x01)
						b=(b>=31?31:b+1);
					buffer[(width*2*i)+j*2+1] = (r << 3) | (g >> 3);
					buffer[(width*2*i)+j*2+0] = ((g << 5) & 0xE0) | b;
				}
			}
		}
	}
	else if (format == 3 || format == 4) {
		if (height%2) {
			dlog_err("suggest even line number for NV12 or NV21 format\n");
			return -EINVAL;
		}
		buffer = fr_alloc_single("bmp2fb", width*height*3/2, &phys_addr);
		if (!buffer) {
			dlog_err("fr_alloc_single failed\n");
			return -ENOMEM;
		}
		/* y */
		for (i = 0; i < height; i++) {
			fseek(fp, 54+rgb_align*(height-1-i), SEEK_SET);
			fread(line, 1, rgb_align, fp);
			for (j = 0; j < width; j++)
				buffer[width*i+j] = __rgb2y(line[j*3+2], line[j*3+1], line[j*3+0]);
		}
		/* cbcr */
		for (i = 0; i < height; i+=2) {
			fseek(fp, 54+rgb_align*(height-2-i), SEEK_SET);
			fread(line, 1, rgb_align*2, fp);
			for (j = 0; j < width; j+=2) {
				r = line[j*3+2] + line[(j+1)*3+2] + line[rgb_align+j*3+2] + line[rgb_align+(j+1)*3+2];
				g = line[j*3+1] + line[(j+1)*3+1] + line[rgb_align+j*3+1] + line[rgb_align+(j+1)*3+1];
				b = line[j*3+0] + line[(j+1)*3+0] + line[rgb_align+j*3+0] + line[rgb_align+(j+1)*3+0];
				__rgb2cbcr(div_round(r,4), div_round(g,4), div_round(b,4), &cb, &cr);
				if (format == 3) {
					buffer[width*height+width*i/2+j] = cb;
					buffer[width*height+width*i/2+j+1] = cr;
				}
				else {
					buffer[width*height+width*i/2+j] = cr;
					buffer[width*height+width*i/2+j+1] = cb;
				}
			}
		}
	}
	else {
		dlog_err("format %d not support\n", format);
		return -EINVAL;
	}

	fclose(fp);
	free(line);

	if (SAVE_YCBCR && (format == 3 || format == 4)) {
		fp = fopen("/tmp/ycbcr_buf", "wb");
		if (!fp) {
			dlog_err("can NOT open file %s\n", argv[1]);
			return -ENOENT;
		}
		fwrite(buffer, 1, width*height*3/2, fp);
		fclose(fp);
		dlog_info("the ycbcr buffer is saved into /tmp/ycbcr_buf file in bin format\n");
	}
	
	dlog_info("bmp width %d, height %d, format %d\n", width, height, format);

	sprintf(cmdbuf, "buf2fb %d 0x%X %d %d %d %d 0x%X %d", overlay, phys_addr, width, height, scale, format, alpha, debug);

	system(cmdbuf);	

	//for (;;)
	//	sleep(1);

	fr_free_single(buffer, phys_addr);

	return 0;

usage:
	printf("Usage:\n\tbmp2fb <bmp_file> <overlay> <scale> <format> [alpha] [debug]\n"
			"\t@overlay: which overlay to use\n"
			"\t@scale: 0: Not scale. 1: Fit screen & keep ratio. 2: Fit & Full screen, not keep ratio.\n"
			"\t\t3: Postscaler - Fit screen & keep ratio. 4: Postscaler - Fit & Full screen, not keep ratio.\n"
			"\t@format: 0: RGB32. 1: BGR32. 2: RGB565 3: NV12. 4: NV21.\n"
			"\t@alpha: plane blending mode, alpha value in RGB888. Default 0xFFFFFF. Not valid for overlay0.\n"
			"\t@debug: 1: print debug log. 0 or none: disable debug log\n\n");
	return 0;
}
