#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#include <time.h>
#include <genisi.h>
#include <listparser.h>
#include <hash.h>
#include <ixconfig.h>
#include <ius.h>

// mkius .../mmc.ixl -o a.ius -h "p7901a" -s "1.0.2.3"
// mkius -l a.ius
// mkius -x a.ius -C path

#define OPT_M 0
#define OPT_L 1
#define OPT_X 2

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
int _get_name(char *path, char *name)
{
	int i = strnlen(path, 1024);

	for (; i >= 0; i--)
	  if (path[i] == '/')
		break;

	strncpy(name, &path[i + 1], 45);
	return 0;
}

int _check_image(int fd, struct ius_desc *d)
{
	int l, ret, r;
	uint8_t *buf;

	buf = malloc(0x100000);
	if(!buf) {
		printf("no buffer.\n");
		return -1;
	}

	l = lseek(fd, (ius_desc_sector(d) << 9), SEEK_SET);
	if(l < 0) {
		printf("  #corrupted");
		free(buf);
		return -3;
	}

	hash_init(IUW_HASH_CRC, ius_desc_length(d));
	for(l = ius_desc_length(d);;) {

		r = (l < 0x100000)? l: 0x100000;
		ret = read(fd, buf, r);
		if(ret > 0) {
			hash_data(buf, ret);
		}

		if(l < 0x100000)
		  break;

		l -= r;
	}
	hash_value(buf);
	ret = read(fd, buf + 16, 16 + 45); // md5 + name

	printf("%s ", buf + 32);
	if(memcmp(buf, buf + 16, 16))
	  printf("  # (corrupted!!!)");

	free(buf);
	return 0;
}

int mkius_make(const char *il, const char *out, const char *hws, const char *sws)
{
	int fo, l, fd, loc, ret, a, rc;
	struct list_desc t;
	struct ius_hdr hdr;
	struct ius_desc *is;
	uint8_t *buf;

	rc = 0;
	buf = malloc(0x100000);
	if(!buf) {
		printf("no buffer.\n");
		return -1;
	}

	if(!out)
	  out = "a.ius";

	fo = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if(fo < 0) {
		printf("create IUS failed: %s\n", out);
		return -1;
	}

	printf("\nGenerating %s ...\n", out);
	memset(&hdr, 0, sizeof(struct ius_hdr));
	hdr.magic = IUS_MAGIC;
	hdr.flag  = 0x2;
	if(hws) strncpy((char *)hdr.hwstr, hws, 8);
	if(sws) strncpy((char *)hdr.swstr, sws, 8);
	hdr.hcrc  = 0;
	hdr.count = 0;
	is = (struct ius_desc *)hdr.data;

	list_init(il);

	for(loc = 512;; ) {
		if(list_get_info(&t, hdr.count) < 0)
		  break ;

		is[hdr.count].sector = (((uint32_t)t.type) << 24) | (loc >> 9);
		is[hdr.count].info0  = t.info0;
		is[hdr.count].info1  = t.info1;
		is[hdr.count].length = 0;

		l = 0;
		if(t.path[0]) {
			fd = open(t.path, O_RDONLY);

			if(fd < 0) {
				printf("open image file failed: %s\n", t.path);
				rc = -1;
				goto __out;
			}

			l = lseek(fd, 0, SEEK_END);
			lseek(fd, 0  , SEEK_SET);
			lseek(fo, loc, SEEK_SET);

			is[hdr.count].length = l;

			hash_init(IUW_HASH_MD5, l);
			for(;;) {
				ret = read(fd, buf, 0x100000);
				if(ret > 0) {
					hash_data(buf, ret);
					a = write(fo, buf, ret);
				}

				if(ret != 0x100000)
				  break;
			}

			/* write hash */
			hash_value(buf);
			a = write(fo, buf, 16);

			/* write image name */
			_get_name(t.path, (char *)buf);
			a = write(fo, buf, 48);
			l += 64;
			memset(buf, 0, 0x400);
			write(fo, buf, 0x400 - (l & 0x3ff));
			loc += (l + 0x3ff) & ~0x3ff;
		} // file write

		hdr.count++ ;

	} // image list

	/* calc header CRC32 */
	hash_init(IUW_HASH_CRC, 512);
	hash_data((uint8_t *)&hdr, 512);
	hash_value(buf);
	hdr.hcrc = *(uint32_t *)buf;

	lseek (fo, 0, SEEK_SET);
	ret = write(fo, &hdr, 512);

__out:
	/* done */
	close(fo);
	free(buf);
	printf("generated!\n");
	return rc;
}

int mkius_list(const char *ius)
{
	int fd, i, ret;
	struct ius_hdr hdr;
	struct ius_desc *is;

	if(!ius) {
		printf("no ius provided.\n");
		return -1;
	}

	fd = open(ius, O_RDONLY);
	if(fd < 0) {
		printf("open ius failed: %s\n", ius);
		return -1;
	}

	ret = read(fd, &hdr, 512);
	is = (struct ius_desc *)hdr.data;

	if(hdr.magic == IUS_MAGIC && hdr.flag == 1) {
		hdr.hwstr[7] = '\0';
		printf("encrypted image for: %s\n", hdr.hwstr);
		return 0;
	}

	if(ius_check_header(&hdr)) {
		printf("image is corrupted.\n");
		return 0;
	}

	printf("\n#image list: \n");
	for(i = 0; i < hdr.count; i++) {
		int type = ius_desc_type(&is[i]);
		char *devs[] = {
			"bnd", "nnd", "fnd", "eeprom",
			"flash", "hdisk", "mmc0",
			"mmc1", "mmc2", "udisk0",
			"udisk1", "udc", "eth", "ram"};

//		printf("got type: %x\n", type);
		printf("\n%c ", ius_desc_mask(&is[i])?
					(ius_desc_mask(&is[i]) == 1? 'u': 'r'): 'i');

		switch(type) {
			case IUS_DESC_STRW:
				if(ius_desc_id(&is[i]) < ARRAY_SIZE(devs)) {
					printf("%s ", devs[ius_desc_id(&is[i])]);
					printf("0x%08llx ", ius_desc_offo(&is[i]));
					_check_image(fd, &is[i]);
				}
				break ;
			case IUS_DESC_STNB:
			case IUS_DESC_STNR:
			case IUS_DESC_STNF:
				printf("%s ", (type == IUS_DESC_STNB)? "bnd" :
							(type == IUS_DESC_STNR)? "nnd" : "fnd");
				printf("0x%08lx ", ius_desc_maxsize(&is[i]));
				printf("0x%08llx ", ius_desc_offo(&is[i]));
				_check_image(fd, &is[i]);
				break ;
			case IUS_DESC_CFG:
				printf("ins ");
				_check_image(fd, &is[i]);
				break;
			case IUS_DESC_EXEC:
				printf("run 0x%08lx 0x%08lx ", ius_desc_entry(&is[i]),
							ius_desc_load(&is[i]));
				_check_image(fd, &is[i]);
				break;
			case IUS_DESC_SYSM:
				if(ius_desc_id(&is[i]) < ARRAY_SIZE(devs)) {
					printf("%s system ", devs[ius_desc_id(&is[i])]);
					_check_image(fd, &is[i]);
				}
				break ;
			case IUS_DESC_MISC:
				if(ius_desc_id(&is[i]) < ARRAY_SIZE(devs)) {
					printf("%s misc ", devs[ius_desc_id(&is[i])]);
					_check_image(fd, &is[i]);
				}
				break ;
			default:
				printf("0x%02x  ", type);
		}
	}
	printf("\n\n");

	close(fd);

	return 0;
}

int mkius_extract(const char *ius, const char *path)
{
	printf("not implemented yet.\n");
	return 0;
}

int mkius(int argc, char * argv[])
{
	char *il = NULL, *out = NULL, *hws = NULL, *sws = NULL,
		 *ius = NULL, *path = NULL;
	int opt = -1;

	if(!il && argv[1] && argv[1][0] != '-') {
//		printf("image list: %s\n", argv[1]);
		il = argv[1];
		opt = OPT_M;
	}

	for(;;)
	{
		int option_index = 0;
		static const char *short_options = "o:h:s:l:x:C";
		static const struct option long_options[] = {
			{"out", required_argument, 0, 'o'},
			{"hwstring", required_argument, 0, 'h'},
			{"swstring", required_argument, 0, 's'},
			{"list", required_argument, 0, 'l'},
			{"extract", required_argument, 0, 'x'},
			{"path", required_argument, 0, 'C'},
			{0,0,0,0},
		};

		int c = getopt_long(argc, argv, short_options,
					        long_options, &option_index);

		if (c == EOF) {
	    	break;
		}

		switch (c) {
			case 'o':
				if(!out) {
//					printf("output image: %s\n", optarg);
					out = optarg;
				} else
				  printf("output image already specified: %s\n", out);
				break;
			case 'h':
				if(!hws) {
					printf("hardware string: %s\n", optarg);
					hws = optarg;
				} else
				  printf("hwstring already specified: %s\n", hws);
				break;
			case 's':
				if(!sws) {
					printf("software string: %s\n", optarg);
					sws = optarg;
				} else
				  printf("swstring already specified: %s\n", hws);
				break;
			case 'l':
				if(!ius) {
					printf("ius package: %s\n", optarg);
					ius = optarg;
					opt = OPT_L;
				} else
				  printf("ius already specified: %s\n", ius);
				break;
			case 'x':
				if(!ius) {
					printf("ius package: %s\n", optarg);
					ius = optarg;
					opt = OPT_X;
				} else
				  printf("ius already specified: %s\n", ius);
				break;
			case 'C':
				if(!path) {
					printf("out path: %s\n", optarg);
					path = optarg;
				} else
				  printf("out path already specified: %s\n", path);
				break;
			default:
				break;
		}
	}

	if(opt == OPT_M)
	  return mkius_make(il, out, hws, sws);
	else if(opt == OPT_L)
	  return mkius_list(ius);
	else if(opt == OPT_X)
	  return mkius_extract(ius, path);
	else
	  printf("unknown option.\n");

	return 0;
}
