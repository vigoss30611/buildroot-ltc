/*
 * this filter logic works for Q3F scaler yuv input width,
 * such as 467*280.
 * useless on Q3 and Apollo3.
*/

#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[])
{
	int width, height;
	int s_width, scaler_w;
	int i, j;

	if (argc != 3) {
		printf("Invalid param number.\n");
		goto usage;
	}

	width = strtoul(argv[1], NULL, 10);
	height = strtoul(argv[2], NULL, 10);
	s_width = width/8;
	s_width *= 8;

	if (width != s_width)
		printf("width should be align by 8, the aligned width is %d\n", s_width);

	for (i = 0; i < 4; i++) {
		scaler_w = s_width-(i*8);
		for(j = 1 ; j <=  height; j++) {
			if((j*scaler_w/8)%256 == 255) {
				break;
			}
		}
		if (j > height)
			break;
	}

	if (i >= 4) {
		printf("width %d NOT supported, choose another value\n", width);
		return 0;
	}

	if (scaler_w == width)
		printf("width %d is supported.\n", width);
	else
		printf("adjusted width %d is supported, two sides may lose when display.\n", scaler_w);

	return 0;

usage:
	printf("Usage: %s <width> <height>\n", argv[0]);
	return 0;
}
