#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <listparser.h>
#include <ius.h>
#include <sys/types.h>
#include <sys/stat.h>

static struct list_desc images[32];
static char base_dir[1024];
static int uc = 0;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
static int list_absolute_dir(const char *buf)
{
	const char *p = buf;
	while(*p == ' ' || *p == '\t')
	  p++;
	if(*p == '/')
	  return 1;

	return 0;
}

static int desc_is_empty(const char *desc)
{
	const char *p;
	int i;

	for(p = desc, i = 0; (*p != '\0')
				&& (*p != '\r')
				&& (*p != '\n')
				&& (i < 1024); p++, i++)
	  if(*p != ' ' && *p != '\t') {
		if(*p == '#')
		  return 1;
		return 0;
	  }

	return 1;
}

#define iswhite(x) (x == ' ' || x == '\t')

static int desc_para(char * buf, const char *desc, int para)
{
	const char *p;
	char *q;
	int i, n = -1;

	for(p = desc, i = 0, q = buf;
				(*p != '\0') && (i < 1024);
				p++, i++) {

		if((p != desc && iswhite(*(p - 1)) && !iswhite(*p))
			|| (p == desc && !iswhite(*p)))
		  n++;

		if(n == para) {
			*q++ = *p;
			if(iswhite(*(p+1)) || *(p+1) == '\r' || *(p+1) == '\n') {
				*q = 0;
//				printf("got para %d: %s\n", para, buf);
				return 0;
			}
		}
	}

	printf("failed to get parameter %d\n", para);
	return -1;
}

static int
desc_mask(char *buf, const char *desc, int para)
{
	int ret;
	ret = desc_para(buf, desc, para);

	if(ret)
	  return -1;

	if(strncmp(buf, "i", 4) == 0)
	  return 0;
	else if(strncmp(buf, "u", 4) == 0)
	  return 1;
	else if(strncmp(buf, "r", 4) == 0)
	  return 2;

	return -1;
}

static int
desc_type(char *buf, const char *desc, int para)
{
	int ret, type;
	ret = desc_para(buf, desc, para);

	if(ret)
	  return -1;

	type = strtol(buf, NULL, 16);

	/* exclude "bnd" & "fnd" from the possibilities */
	if(!type || type == 0xb || type == 0xf) {
		if(strncmp(buf, "mmc", 3) == 0
					|| strncmp(buf, "eeprom", 6) == 0
					|| strncmp(buf, "flash", 5) == 0
					|| strncmp(buf, "hdisk", 5) == 0
					|| strncmp(buf, "udisk", 5) == 0
					|| strncmp(buf, "ram", 3) == 0)
		  return IUS_DESC_STRW;
		else if(strncmp(buf, "bnd", 3) == 0)
		  return IUS_DESC_STNB;
		else if(strncmp(buf, "nnd", 3) == 0)
		  return IUS_DESC_STNR;
		else if(strncmp(buf, "fnd", 3) == 0)
		  return IUS_DESC_STNF;
		else if(strncmp(buf, "run", 3) == 0)
		  return IUS_DESC_EXEC;
		else if(strncmp(buf, "ins", 3) == 0)
		  return IUS_DESC_CFG;
		else {
			printf("unknow action type: %s\n", buf);
			return -2;
		}
	}

	return type;
};

static int
desc_file(char *buf, const char *desc, int para)
{
	int ret;
	ret = desc_para(buf, desc, para);
	if(ret)
	  return -1;
	if(strnlen(buf, 1024) == 0
		|| strnlen(buf, 1024) == 1024)
	{
		printf("file string too long.\n");
		return -1;
	}

	if(!list_absolute_dir(buf))
	{
		char tmp[1024];
		memcpy(tmp, buf, 1024);
		memcpy(buf, base_dir, 1024);
		strncat(buf, tmp, 1024 - strnlen(buf, 1024));
	}

//	printf("file: %s\n", buf);

	return 0;
}

static uint32_t
desc_addr(char *buf, const char *desc, int para)
{
	int ret;
	ret = desc_para(buf, desc, para);
	if(ret)
	  return -1;
	return strtoul(buf, NULL, 16);
}

static uint64_t
desc_offs(char *buf, const char *desc, int para)
{
	int ret = desc_para(buf, desc, para);
	if(ret)
	  return -1;
	return strtoull(buf, NULL, 16);
}

static int
desc_dev(char *buf, const char *desc, int para)
{
	int ret, i;
	char *devs[] = {
		"bnd", "nnd", "fnd", "eeprom",
		"flash", "hdisk", "mmc0",
		"mmc1", "mmc2", "udisk0",
		"udisk1", "udc", "eth", "ram"};

	ret = desc_para(buf, desc, para);

	if(ret)
	  return -1;

	for(i = 0; i < ARRAY_SIZE(devs); i++)
	  if(strncmp(buf, devs[i], 10) == 0)
		return i;

	return -1;
}

static int desc_to_struct(const char *desc, struct list_desc *t)
{
	char buf[1024];
	uint64_t tmp0, tmp1;
	int ret, type, mask;

	ret = desc_type(buf, desc, 1);
//	printf("got type: %d\n", ret);
	if(ret < 0) /* get type failed */
	  return ret;

	type = ret;
	mask = desc_mask(buf, desc, 0);
	t->type = type | (mask << 6);
	t->info0 = t->info1 = t->length = 0;
	memset(t->path, 0, 1024);
	switch(type)
	{
		case IUS_DESC_CFG:
			ret = desc_file(t->path, desc, 2);
			if(ret)
			  return ret;
			break;
		case IUS_DESC_EXEC:
			t->info0 = desc_addr(buf, desc, 2);
			t->info1 = desc_addr(buf, desc, 3);
			ret = desc_file(t->path, desc, 4);
			if(ret)
			  return ret;
			break;
		case IUS_DESC_STRW:
			ret = desc_file(t->path, desc, 3);
			if(ret)
			  return ret;

			tmp0 = desc_dev (buf, desc, 1);
			tmp1 = desc_offs(buf, desc, 2);
			if(!tmp1) {
				/* no offset provided, may be a system.img */
				if(strncmp(buf, "system", 6) == 0)
				  t->type = IUS_DESC_SYSM | (mask << 6);
				else if(strncmp(buf, "misc", 4) == 0)
				  t->type = IUS_DESC_MISC | (mask << 6);
				else if(strncmp(buf, "cache", 5) == 0)
				  t->type = IUS_DESC_CACHE | (mask << 6);
				else if(strncmp(buf, "user", 4) == 0)
				  t->type = IUS_DESC_USER | (mask << 6);

				t->info1 = (uint32_t)(tmp0 << 16);
			} else {
				if(tmp1 >> 48) {
					printf("offset two long.\n");
					return -1;
				}
				t->info0 = (uint32_t)(tmp1 & 0xffffffffull);
				t->info1 = (uint32_t)((tmp0 << 16) |
							((tmp1 >> 32) & 0xffffull));
			}
			break;
		case IUS_DESC_STNB:
		case IUS_DESC_STNR:
		case IUS_DESC_STNF:
			tmp1 = desc_offs(buf, desc, 3); // offset
			tmp0 = desc_offs(buf, desc, 2); // part size
			if(!tmp0) {
				/* no offset provided, may be a system.img */
				if(strncmp(buf, "system", 6) == 0)
				  t->type = IUS_DESC_SYSM | (mask << 6);
				else if(strncmp(buf, "misc", 4) == 0)
				  t->type = IUS_DESC_MISC | (mask << 6);

				ret = desc_file(t->path, desc, 3);
				if(ret)
				  return ret;
			} else {
				if(tmp1 >> 48) {
					printf("offset two long.\n");
					return -1;
				}
				t->info0 = (uint32_t)(tmp1 & 0xffffffffull);
				t->info1 = (uint32_t)((tmp0 & ~0xffff) |
							((tmp1 >> 32) & 0xffffull));

				ret = desc_file(t->path, desc, 4);
				if(ret)
				  return ret;
			}
			break;

		case IUS_DESC_SYSM:
		case IUS_DESC_MISC:
			ret = desc_file(t->path, desc,2);
			if(ret) return ret;
		default:
//			printf("type: %02x\n", type);
		;
	}

#if 0
	/* finally check the file existence */
	fd = open(t->path, O_RDONLY);
	if(fd < 0) {
		printf("image file do not exsit.\n");
		return -1;
	} else {
		t->length = lseek(fd, 0, SEEK_END);
		close(fd);
	}
#endif

#if 0
	printf("parse line successfully:\n");
	printf("info0: 0x%x, info1: 0x%x, len: 0x%x "
				"type: %d, path: %s\n",
				t->info0, t->info1, t->length,
				t->type, t->path);
#endif
	return 0;
}

static int list_base_dir(char *buf, const char *lst)
{
	int i;

	if(list_absolute_dir(lst))
	  strncpy(buf, lst, 1024);
	else {

		if(!getcwd(buf, 1024))
		{
			printf("you are working in a deep path.\n");
			return -1;
		}
		strncat(buf, "/", 1024 - strnlen(buf, 1024));
		strncat(buf, lst, 1024 - strnlen(buf, 1024));
	}

	for(i = strnlen(buf, 1024); i >= 0; i--)
	  if(buf[i] == '/') {
		  buf[i + 1] = '\0';
		  break;
	  }

//	printf("base dir: %s\n", buf);
	return 0;
}

int list_init(const char *lst)
{
	FILE *fp;
	char desc[1024], *ret;
	int count = 0, res, i;
	struct list_desc *p = images;

	fp = fopen(lst, "r");
	if(!fp) {
		printf("list %s not found.\n", lst);
		return -1;
	}

	list_base_dir(base_dir, lst);

	for(uc = 0, count = 0;;++count)
	{
		if(uc == 32) {
		  printf("a maximum of 32 images are supported.\n");
		  break;
		}

		ret = fgets(desc, 1024, fp);
		if(!ret) {
//			printf("file ends.\n");
			break;
		} else {
			if(desc_is_empty(desc))
			  continue;
//			printf("got(%p): %s", ret, desc);
			res = desc_to_struct(desc, p);

			if(res) {
				printf("line %d parsing failed.\n", count);
				break;
			} else {
				p++;
				uc++;
			}
		}
	}

	if(res)
	  return -1;
//	printf("%d images listed:\n", uc);
//	printf("type   len          info0        info1        file\n");
	printf("--------------------------------------------------\n");
	for(i = 0, p = images; i < uc; i++, p++) {
		printf("0x%02x   0x%08x"
					"   0x%08x   0x%08x   %s\n", p->type,
					p->length, p->info0, p->info1,
					p->path);
	}

	fclose(fp);
	return 0;
}

/* return type of the 'n'th image */
int list_get_info(struct list_desc *t, int n)
{
	if(n < uc) {
		memcpy(t, images + n, sizeof(*t));
		return 0;
	}

	return -1;
}
