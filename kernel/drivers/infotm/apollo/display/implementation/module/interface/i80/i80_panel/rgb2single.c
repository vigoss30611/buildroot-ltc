#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
unsigned char buf1[1920* 1080* 3]= {0}, buf2[128] = {0}, *p;
int main(int argc, char *argv[])
{
	int fs = 0, newfs = 0;
	int i,j;
	int width, height;
	char file_name[128];
	int off_bits = 0;
	// unsigned char *buf1;

	if (argc < 3) {
		printf("nedd input both width and height\n");
		exit;
	}
	width = atoi(argv[2]);
	height = atoi(argv[3]);
	if (width <= 0 || height  <= 0) {
		printf("input correct width and height\n");
		exit;
	}

	if ((access(argv[1], 0) == -1)) {
		printf("No logo bmp file found\n");
		exit;
	}
//	fs = open ("kernel_logo.bmp", O_RDONLY );
	fs = open (argv[1], O_RDONLY );
	read(fs, buf1, 0x32);
	off_bits = (buf1[0xA+3] << 24) | (buf1[0xA+2] << 16) | (buf1[0xA+1] << 8) | (buf1[0xA] << 0);
	lseek ( fs, off_bits, SEEK_SET ) ;
	// read ( fs, buf1, sizeof ( buf1 ) );
	read ( fs, buf1, width*height*3);
	close ( fs );
	newfs = open ( "kernel_logo.h", O_RDWR | O_CREAT | O_TRUNC, 0666);
    int xxfd = open("style.h", O_RDWR | O_CREAT  | O_TRUNC, 0666);
    printf("newfs: %d, xxfd: %d\n", newfs, xxfd);
#if 0
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
			if (*(p + 2) == 0x00)
				*(p + 2) = 0x02;
			if (*(p + 1) == 0x00)
				*(p + 1) = 0x02;
			if (*p == 0x00)
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
#else
    write(newfs, "const unsigned char antaur_h[] = {\n", strlen("const unsigned char antaur_h[] = {\n"));
    int bc = 8;
    unsigned char value = 0;
    int count = 0;
    char s2[1];
    int s_count = 0;
	for (i = height - 1; i >= 0; i--) {
		for (j = width - 1; j >= 0; j--) {
            bc--;
			p = buf1 + i*width*3 + j*3;
            int c;
            int color = ((*p)*38 + (*(p+1))*75 + (*(p+2))*15) >> 7;
			if (color < 128) {
				c = 0x00;
                s2[0] = '0';
            } else {
                c = 0x01;
                s2[0] = '1';
            }
            //printf("s2: %c\n", s2[0]);
            value += c << bc;
            if(bc == 0) {
                memset(buf2, 0, 128);
			    sprintf ( buf2, " 0x%02x,", value);
                bc = 8;
			    int ret = write ( newfs, buf2, strlen (buf2) );
                value = 0;
                count++;
                if(count % 12 == 0) {
			        write ( newfs, "\n", 1);
                }
            }
            s_count++;
            write(xxfd, s2, 1);
            if(s_count%width == 0) {
			    write (xxfd, "\n", 1);
            }
		}
	}
    write(newfs, "\n};\n", 3);
    printf("s_count: %d, count : %d\n", s_count, count);
	//write ( newfs, buf1, width*height*3);
#endif
	close ( newfs );
    close(xxfd);
}

