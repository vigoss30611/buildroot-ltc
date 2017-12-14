#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <time.h>
#include <arpa/inet.h>
#include <hash.h>
#include <genisi.h>

const char *list_type[] = {
    "raw",
    "rawx",
    "config",
    "boot",
};          

const char *list_check[] = {
    "crc",
    "md5",
    "sha1",
    "sha256"
};          


int image_check_u_hcrc(struct image_header header)
{
	struct image_header check;
	uint32_t len = sizeof(header);
	uint8_t a[len];
	uint8_t dcheck[32];
	uint32_t hcrc;

	memcpy(&check, &header,len);
	check.ih_hcrc = 0;
	hash_init(0, len);
	memcpy(a, &check, len);
	hash_data(a, len);
	hash_value(dcheck);
	memcpy(&hcrc, dcheck, 4); 

	return (ntohl(header.ih_hcrc) == hcrc);
}

int image_check_isi_hcrc(struct genisi_image_header header)
{
    struct genisi_image_header check;
    uint32_t len = sizeof(header);
    uint8_t a[len];
    uint8_t dcheck[32];
    uint32_t hcrc;

    memcpy(&check, &header,len);
    check.hcrc = 0;
    hash_init(0, len);
    memcpy(a, &check, len);
    hash_data(a, len);
    hash_value(dcheck);
    memcpy(&hcrc, dcheck, 4);

    return (header.hcrc == hcrc);
}


int image_dcrc(int fd, int fd_isi, struct genisi_image_header *header)
{
	uint8_t buf[4096];
	off_t size;
	uint32_t i;
	uint32_t length;
	uint32_t len;

	if (header->size > 0) {
		if (fd_isi < 0) {
			size = header->size;
			lseek(fd, 512, SEEK_SET);
		} else {
	    	size = header->size;
	    	lseek(fd, 64, SEEK_SET);
	    	lseek(fd_isi, 512, SEEK_SET);
		}
	} else {
	    size = lseek(fd, 0, SEEK_END);
	    lseek(fd, 0, SEEK_SET);
	    header->size += size;
	    lseek(fd_isi, 512, SEEK_SET);
	}

	hash_init((header->type >> 24) & 0x3, size);
	for (i = 0;i < (size / 4096) + 1;i++) {
	    memset(buf, 0,4096);
	    if ((len = read(fd, buf, 4096)) < 0) {
	        printf("read failed\n");
	        return -1;
	    }
	    hash_data(buf, len);
		if (fd_isi >= 0)
	    	if ((length = write(fd_isi, buf, len)) != len) {
	        	printf("write wrong\n");
	        	return -1;
	    	}
	}
	hash_value(header->dcheck);
	return 0;
}


int change_header(int fd, int fd_isi, struct genisi_image_header *header, uint8_t *sb)
{
	struct image_header head;
	uint32_t dcheck;
	
	if (read(fd, &head, sizeof(head)) < 0) {
	    printf("read failed\n");
	    return -1;
	}
	if (ntohl(head.ih_magic) == 0x27051956) {
	    if (!image_check_u_hcrc(head))
	    {
	        printf("Header CRC32 do not match,the image is broken\n");
	        return -1;
	    }
		header->size += ntohl(head.ih_size);
		image_dcrc(fd, fd_isi, header);
		
		memcpy(&dcheck, header->dcheck, sizeof(dcheck));
		if(dcheck != ntohl(head.ih_dcrc))
		{
		    printf("Data CRC32 do not match!!\n");
		    return -1;
		}

	    if (!sb[3])
	        memcpy(header->name, head.ih_name, sizeof(head.ih_name));
	    if (!sb[2])
	        header->entry = ntohl(head.ih_ep);
	    if (!sb[1])
	        header->load = ntohl(head.ih_load);
	}
	else {
		printf("the image have not a U header\n");
		image_dcrc(fd, fd_isi, header);
	}
	return 0;
}

int check_header(struct genisi_image_header *header, char *path)
{
	int fd;
	uint32_t i;
	struct genisi_image_header check;
	
	if ((fd = open(path, O_RDWR, 0666)) < 0) {
		printf("open the image failed, the path is %s\n",path);
		return -1;
	}
	if (read(fd, header, sizeof(*header)) < 0) {
		printf("read failed");
		return -1;
	}
	memcpy(&check, header, sizeof(*header));
	if (header->magic != 0x6973692b) {
		printf("head magic is wrong\n");
		return -1;
	}
	if (!image_check_isi_hcrc(*header)) {
	    printf("The crc about head is wrong\n");
	    return -1;
	}
	image_dcrc(fd, -1, header);
	for (i = 0;i < 4;i++)
		if (header->dcheck[i] != check.dcheck[i]) {
			printf("Date crc is wrong\n");
			return -1;
		}
	close(fd);
	return 0;
}


void header_init(struct genisi_image_header *header)
{
	header->magic = 0x6973692b;
	header->type = 0;
	header->flag = 0;
	header->size = 0;
	header->load = 0;
	header->entry = 0;
	header->time = 0;
	header->hcrc = 0;
	/* deprecated, by warits Sat Sep 10, 2011 */
#if 0
	header->iusmsk = 0;
	memset(header->iuskey, 0, 32);
#endif
	memset(header->dcheck, 0, 32);
	memset(header->name, 0, 192);
	memset(header->extra, 0, 256);
}

int genisi(int argc, char * argv[])
{
	printf("::%s\n", __func__);
	
	struct genisi_image_header header, info;
	int fd,fd_isi, i;
	char *outname = "a.isi";
	char *path = NULL;
	uint8_t s[512];
	uint8_t crc[32];
	uint32_t flags = 1, length;
	uint8_t sb[4];
	struct tm *tblock;
	time_t b;

	header_init(&header);
	
	memset(sb, 0, 4);
	memset(s, 0, 512);
	
	for (;;) {
		int option_index = 0;
		static const char *short_options = "o:d:n:g:t:a:e:f:i:rs";
		static const struct option long_options[] = {
			{"out", required_argument, 0, 'o'},
			{"data", required_argument, 0, 'd'},
			{"name", required_argument, 0, 'n'},
			/* accept: crc, md5, sha1, sha256 */
			{"signature", required_argument, 0, 'g'},
			/* TODO: currently supported types are:
			 * 0: legacy boot loader (lboot)
			 * 1: config boot loader (cboot)
			 * 2: raw executable (rawx)
			 * 3: raw image (raw)
			 * 4: NANDFLASH image (nand)
			 * 
			 * accept: lboot, cboot, rawx, raw, nand
			 */
			{"type", required_argument, 0, 't'},
			{"load", required_argument, 0, 'a'},
			{"entry", required_argument, 0, 'e'},
			{"flag", required_argument, 0, 'f'},
			{"information", required_argument, 0, 'i'},
			{"force-raw", no_argument, 0, 'r'},
			{"secure", no_argument, 0, 's'},
			{0, 0, 0, 0},
		};

		int c = getopt_long(argc, argv, short_options,
				long_options, &option_index);
		
		if (c == EOF) {
			if (path == NULL) {
				printf("you must define the path of image\n");
				return -1;
			}
			break;
		}

		switch (c) {
			case 'o':
				printf("the name of the file is %s\n",optarg);
				outname = optarg;
				sb[0] = 1;
				break;
			case 'r':
				printf("__force_raw\n");
				flags = 0;
				break;
			case 's':
				printf("__secure\n");
				header.type |= (0x1 << 28);
				break;
			case 'g':
				printf("signature type: %s\n", optarg);
				for (i = 0;i < sizeof(list_check) / sizeof(list_check[0]);i++)
					if (strcmp(optarg, list_check[i]) == 0)
						header.type |= (i << 24);
				if (i == (sizeof(list_check) / sizeof(list_check[0]) + 1)) {
					printf("error: wrong crc\n");
					return -1;
				}
				break;
			case 't':
				printf("image type: %s\n", optarg);
				for (i = 0;i < sizeof(list_type) / sizeof(list_type[0]);i++)
					if (strcmp(optarg, list_type[i]) == 0) {
						header.type |= i;
						break;
					}
				if (i == (sizeof(list_type) / sizeof(list_type[0]) + 1)) {
					printf("error: wrong type\n");
					return -1;
				}
				break;
			case 'a':
				printf("load address: 0x%lx\n",
							strtol(optarg, NULL, 0));
				header.load = strtoul(optarg, NULL, 0);
				sb[1] = 1;
				break;
			case 'e':
				printf("entry address: 0x%lx\n",
							strtol(optarg, NULL, 0));
				header.entry = strtoul(optarg, NULL, 0);
				sb[2] = 1;
				break;
			case 'n':
				/* FIXME: this is a BUG */
				printf("image name: %s\n", optarg);
//				memcpy(header.name, optarg, sizeof(optarg));
				strncpy(header.name, optarg, 192);
				sb[3] = 1;
				break;
			case 'd':
				printf("the path of image is %s\n", optarg);
				path = optarg;
				break;
			case 'f':
				printf("the flag is %lx\n",
							strtoul(optarg, NULL, 0));
				header.flag = strtoul(optarg, NULL, 0);
				break;
			case 'i':
				printf("the path of .isi is %s\n", optarg);
				char image_type[5][50] = {"Legacy boot loader", "Easy boot loader", "Raw Executable Image",
					"Raw Image", "NANDFLASH image (including OOB data)"};
				char check[4][10] = {"CRC32", "MD5", "SHA-1", "SHA-256"};
				char ensure_secure[3][15] = {"Not secure", "Secure", "Reserved"};
				
				if (check_header(&info, optarg) == 0){
					b = (time_t)info.time;
					tblock = localtime(&b);
					printf("Image Name: %s\n"
						   "Created: %s"
						   "Flag: %x\n"
						   "Data Size: %d Bytes = %0.2f KB = %0.2f MB\n"
						   "Image Type: %s\n"
						   "Verification Method: %s\n"
						   "Secure Image: %s\n"
						   "Load Address: %x\n"
						   "Entry Point: %x\n", info.name, asctime(tblock),
						   info.flag, info.size, (double)info.size / 1024, 
						   (double)info.size / (1024 * 1024), image_type[info.type & 0xf], 
						   check[(info.type >> 24) & 0xf], ensure_secure[info.type >> 28], info.load, info.entry);
				}
				return 0;
		}
	}
	if ((fd = open(path, O_RDWR, 0666)) < 0) {
		printf("open the image failed, the path is %s\n",path);
		return -1;
	}
	if ((fd_isi = open(outname, O_TRUNC | O_CREAT | O_WRONLY, 0666)) < 0) {
		printf("cannot creat the target file\n");
		return -1;
	}
	
	if (flags) 
		change_header(fd, fd_isi, &header, sb);
	else
		image_dcrc(fd, fd_isi, &header);
	
	b = (time_t)header.time;	
	time(&b);
 	
   	memcpy(s, &header, sizeof(header));	

	hash_init(0,512);
	hash_data(s,512);
	hash_value(crc);
	memcpy(&header.hcrc, crc, 4);

	lseek(fd_isi, 0,SEEK_SET);
	if ((length = write(fd_isi, &header, sizeof(header))) != sizeof(header)) {
	    printf("write wrong\n");
	    return -1;
	}

	close(fd);
	close(fd_isi);
	
	return 0;
}

