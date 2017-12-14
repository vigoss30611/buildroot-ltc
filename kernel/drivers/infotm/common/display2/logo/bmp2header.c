#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char s1[] = "unsigned int ";
char s2[] = "_data[]={";
char s3[] = "};";
char int_type[] = "int ";
char newline[] = "\n";

int main(int argc, char *argv[])
{
	FILE *fb;
	FILE *fh;
	int width=0, rgb_align, height=0;
	unsigned char buf=0;
	int i, j, k, len;
	char header[32] = {0};
	char pre[32] = {0};
	unsigned char rgb[3];
	unsigned int rgb32;
	char rgb_str[16] = {0};
	char width_var[40] = {0};
	char height_var[40] = {0};
	char digit_str[8] = {0};

	//printf("argv[1]=%s\n", argv[1]);

	if (argc != 2) {
		printf("Invalid param number\n");
		goto usage;
	}

	for (i = 0, j = 0; argv[1][i]; i++) {
		if (argv[1][i] == '.')
			break;
		if (!isalnum(argv[1][i]))
			pre[j++] = '_';
		else
			pre[j++] = argv[1][i];
	}
	pre[j] = '\0';
	//printf("pre=%s\n", pre);
	len = strlen(pre);
	//printf("len=%d\n", len);
	strcpy(header, pre);
	header[len++] = '.';
	header[len++] = 'h';
	header[len] = '\0';
	//printf("header=%s\n", header);
	strcpy(width_var, pre);
	strcat(width_var, "_width=");
	strcpy(height_var, pre);
	strcat(height_var, "_height=");
	//printf("width_var=%s, height_var=%s\n", width_var, height_var);

	fb = fopen(argv[1], "rb");
	if (!fb) {
		printf("Failed to open bmp file %s\n", argv[1]);
		goto usage;
	}
	fh = fopen(header, "w");
	if (!fh) {
		printf("Failed to create header file %s\n", argv[2]);
		goto usage;
	}
	/* parse width */
	fseek(fb, 18, SEEK_SET);
	for (i = 0; i < 4; i++) {
		fread(&buf, 1, 1, fb);
		width+=buf<<(i*8);
	}
	rgb_align = width*3+3;
	rgb_align /= 4;
	rgb_align *= 4;

	/* parse height */
	fseek(fb, 22, SEEK_SET);
	for (i = 0; i < 4; i++) {
		fread(&buf, 1, 1, fb);
		height+=buf<<(i*8);
	}

	printf("parse width = %d, height = %d\n", width, height);
	//printf("rgb_align=%d\n", rgb_align);

	fwrite(s1, 1, strlen(s1), fh);
	fwrite(pre, 1, strlen(pre), fh);
	fwrite(s2, 1, strlen(s2), fh);
	fwrite(newline, 1, 1, fh);
	
	for (i = 0, k = 0; i < height; i++) {
		fseek(fb, 54+rgb_align*(height-1-i), SEEK_SET);
		for (j = 0; j < width; j++) {
			fread(rgb, 1, 3, fb);
			rgb32 = (rgb[2]<<16)+(rgb[1]<<8)+(rgb[0]<<0);	// a, r, g, b
			sprintf(rgb_str, "0x%X,", rgb32);
			fwrite(rgb_str, 1, strlen(rgb_str), fh);
			if (++k % 12 == 0)
				fwrite(newline, 1, 1, fh);
		}
	}
	fwrite(newline, 1, 1, fh);
	fwrite(s3, 1, strlen(s3), fh);
	fwrite(newline, 1, 1, fh);

	fwrite(int_type, 1, strlen(int_type), fh);
	fwrite(width_var, 1, strlen(width_var), fh);
	sprintf(digit_str, "%d;", width);
	fwrite(digit_str, 1, strlen(digit_str), fh);
	fwrite(newline, 1, 1, fh);

	fwrite(int_type, 1, strlen(int_type), fh);
	fwrite(height_var, 1, strlen(height_var), fh);
	sprintf(digit_str, "%d;", height);
	fwrite(digit_str, 1, strlen(digit_str), fh);
	fwrite(newline, 1, 1, fh);

	fclose(fb);
	fclose(fh);

	printf("output header file is %s\n", header);

	return 0;

usage:
	printf("Usage: %s <bmp file>\n", argv[0]);
	return -1;
}
