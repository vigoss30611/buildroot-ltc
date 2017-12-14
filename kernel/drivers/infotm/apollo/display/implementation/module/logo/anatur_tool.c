#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
unsigned char buf1[1920* 1080* 3], buf2[128], *p;
int main(int argc, char *argv[])
{
	int fs = 0;
	int i,j;
	int width, height;
	char file_name[128];
	int off_bits = 0;
	// unsigned char *buf1;

	if (argc < 3) {
		printf("nedd input both width and height\n");
		exit;
	}
	width = atoi(argv[1]);
	height = atoi(argv[2]);
	if (width <= 0 || height  <= 0) {
		printf("input correct width and height\n");
		exit;
	}

	sprintf(file_name, "kernel_logo_%d_%d.bmp", width, height);
	if ((access(file_name, 0) == -1)) {
		printf("No logo bmp file found\n");
		exit;
	}
//	fs = open ("kernel_logo.bmp", O_RDONLY );
	fs = open (file_name, O_RDONLY );
	read(fs, buf1, 0x32);
	off_bits = (buf1[0xA+3] << 24) | (buf1[0xA+2] << 16) | (buf1[0xA+1] << 8) | (buf1[0xA] << 0);
	lseek ( fs, off_bits, SEEK_SET ) ;
	// read ( fs, buf1, sizeof ( buf1 ) );
	read ( fs, buf1, width*height*3);
	close ( fs );
	fs = open ( "antaur.h", O_RDWR | O_CREAT | O_TRUNC );
	// p = buf1 + sizeof ( buf1 );
//	p = buf1 + width*(height-1)*3;
	// for ( i = 0; i < 800 * 480; i++ ) {
	//  if ( ( i % 800 ) == 0 ) {
	for (i = height - 1; i >= 0; i--) {
	//	sprintf ( buf2, "//line %3d\n", height - i - 1);
	//	write ( fs, buf2, strlen(buf2));
		for (j = 0; j < width; j++) {
			p = buf1 + i*width*3 + j*3;
			/*
			if ( ( j % width ) == 0 ) {
				sprintf ( buf2, "//line %3d\n", j / width);
				write ( fs, buf2, strlen ( buf2 ) );
			}
			*/
			//if ((*(p + 2) == 0x01) && (*(p + 1) == 0x01) && (*p == 0x01))
			//	*p = 0x02;
			if (*(p + 2) == 0x01)
				*(p + 2) = 0x02;
			if (*(p + 1) == 0x01)
				*(p + 1) = 0x02;
			if (*p == 0x01)
				*p = 0x02;
			sprintf ( buf2, " 0x%02x, 0x%02x, 0x%02x, 00,", *p, *(p + 1), *(p + 2));
			//		p += 3;
			write ( fs, buf2, strlen ( buf2 ) );
			/*
			if ( ( j & 3 ) == 3 ) {
				sprintf ( buf2, "\n" );
				write ( fs, buf2, strlen ( buf2 ) );
			}
			*/
		}
	}
	close ( fs );
}

