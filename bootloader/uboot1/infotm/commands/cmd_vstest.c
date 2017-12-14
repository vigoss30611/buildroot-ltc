#include <common.h>
#include <command.h>
#include <vstorage.h>
#include <malloc.h>
#include <div64.h>
#include <linux/mtd/compat.h>


int change_len = 0;
static int id = 0;
static char *name = NULL;
static char *__rnd_lib = (char *)0x47c00000;

struct number {
    uint32_t len;
    uint32_t old_len;
    volatile uint32_t current;
    int error;
    uint64_t sum_len;
    uint32_t sum;
    uint32_t start;
};

static unsigned char random(void)
{
    unsigned long long rand = get_ticks() * 100000;
    unsigned char a;

    rand = rand * 1664525L + 1013904223L;
    a = rand >> 24;

    if (a < 'A')
	a = a % 10 + 48;
    else if (a < 'F')
	a = a % 6 + 65;
    else if (a < 'a' || a > 'f')
	a = a % 6 + 97;
    return a;

}

static uint32_t get_rand(int length)
{
    unsigned int temp;
    unsigned long tmp;

    temp = random();
    tmp = simple_strtoul((const char*)&temp,NULL,16);
    temp = tmp / 15 * length;

    return temp;
}

static uint32_t writeread_number(struct number *len_num)
{
    len_num->len -= len_num->current;
    if (len_num->len > change_len) {
	len_num->current = change_len;
	return 1;
    } else if (len_num->len > 0) {
	len_num->current = len_num->len;
	return 1;
    } else 
	return 0;
}

static int get_the_coverage(struct number *sun, uint8_t *xbuf)
{
    uint16_t tmp = 0;
    uint16_t temp = 0;
    uint16_t i;

    tmp = (sun->start / vs_align(id)) / 8;	
    temp = (sun->start / vs_align(id)) % 8;

    for (i = 0;i < (sun->old_len / vs_align(id));i++) {
	if (!(xbuf[tmp] & (0x1 << temp)))
	{
	    (sun->sum)++;
	    xbuf[tmp] |= (0x1 << temp);
	}
	temp++;
	if (temp == 8) {
	    temp = 0;
	    tmp += 1;
	}
    }

    return 0;
}


int flags = 0;

int do_vstest(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
    uint32_t length = 0;
    uint32_t capacity, address;
    unsigned long long time;
    uint64_t to,tn,ten,tnto = 0,tentn = 0;
    uint8_t *buf;
    uint8_t *s;
    int i,j;
    int start = 0;
    uint32_t len = 0;
    struct number num;

    buf = malloc(8192);
    s = buf + 4096;

    printf("allocated buffer: %p\n", buf);

    switch (argc) {
	case 3:
	    if (strcmp(argv[1], "speed") == 0) {
		length = simple_strtoul(argv[2], NULL, 16) * 1024;
		if (vs_is_assigned()) {
			memcpy(buf, __rnd_lib, change_len);

#if 0
		    for (i = 0;i<change_len;i++)
			buf[i] = random();
#endif
		    if (flags) {
			vs_erase(start, length);
			printf("please wait until erase finish\n");
		    }
		    to = get_ticks();
		    for (i = 0;i < length / change_len;i++) {
			vs_write(buf, change_len * i, change_len, 0);
			tn = get_ticks();
		    }
		    tn = get_ticks();
		    for (i = 0;i < length / change_len;i++) {
			vs_read(s, change_len * i, change_len, 0);
			ten = get_ticks();
		    }
		    ten = get_ticks();

		    tnto = lldiv((tn - to), 1000000);
		    tentn = lldiv((ten - tn), 1000000);

		    printf("The speed of write is %d KB/s, the speed of read is %d KB/s\n",
			    (change_len * (length / change_len)) / ((uint32_t)(tnto) * 1024),
			    (change_len * (length / change_len)) / ((uint32_t)(tentn) * 1024));
		    free(buf);
		    return 0;
		} else {
		    printf("please input the command -- (vstest assign XXXXX) first\n");
		    free(buf);
		    return -1;
		}

	    } else if (strcmp(argv[1], "assign") == 0) { 
		if (vs_assign_by_name(argv[2], 1) == 0) {
		    name = argv[2];
		    id = vs_device_id(name);
		    if (vs_list[id].flag & DEVICE_ERASABLE)
			flags = 1;
		    if (id == 0 || id == 1 || id == 2)
			change_len = 4096;
		    else
			change_len = 8 * vs_align(id);
		    printf("device init fine\n");
		    free(buf);
		    return 0;
		} else {
		    printf("cannot find the equipment\n");
		    free(buf);
		    return -1;
		}
	    } else if (strcmp(argv[1], "address") == 0) {
		if (vs_is_assigned()) {
		    length = simple_strtoul(argv[2], NULL, 16) * 1024;

		    if (length > change_len) 
			len = change_len;
		    else 
			len = length;

		    memcpy(buf, __rnd_lib, len);
#if 0
		    for (i = 0;i<len;i++)
			buf[i] = random();
#endif
		    if (flags)
			vs_erase(start, len);
		    for (i = 0;i < length / len;i++) {
			vs_read(s,len * i, len,0);
			for (j = 0;j < len;j++)
			    if (s[j] != 0xff) {
				printf("address is fault\n");
				free(buf);
				return -1;
			    }
			vs_write(buf,len * i,len,0);
			vs_read(s,len * i,len,0);

			for (i = 0;i<len;i++)
			    if (buf[i] != s[i]) {
				printf("error: start is %d, len is %d,buf[%d] is %d\n",start,len,i,buf[i]);
				free(buf);
				return -1;
			    }
		    }
		    printf("the test of address is successful\n");
		    free(buf);
		    return 0;
		} else {
		    printf("please input the command -- (vstest assign XXXXX) first\n");
		    free(buf);
		    return -1;
		}
	    } else if (strcmp(argv[1], "restore") == 0) {
		if (vs_is_assigned()) {
		    capacity = simple_strtoul(argv[2], NULL, 16) * 1024;

		    if (capacity > change_len)
			len = change_len;
		    else
			len = length;
		    if (flags)
			vs_erase(start, len);

		    memcpy(buf, __rnd_lib, len);
#if 0
		    for (i = 0;i<len;i++)
			buf[i] = random();
#endif

		    for (i = 0;i < capacity / len;i++) {
			address = len * i;
			memcpy(buf, &address, sizeof(address));
			vs_write(buf, len * i, len, 0);
		    }

		    for (i = 0;i < capacity / len;i++) {
			vs_read(s, len * i, len, 0);
			memcpy(&address, s, sizeof(address));
			if (address != (len * i)) {
			    printf("address is wrong\n");
			    free(buf);
			    return -1;
			}
			for (j = sizeof(address);j < len;j++)
			    if (s[j] != buf[j]) {
				printf("error: start is %d, len is %d,buf[%d] is %d\n",start,len,i,buf[i]);
				free(buf);
				return -1;
			    }
		    }
		    printf("the test of address is successful\n");
		    free(buf);
		    return 0;
		} else {
		    printf("please input the command -- (vstest assign XXXXX) first\n");
		    free(buf);
		    return -1;
		}
	    }
	case 2:
	    if (strcmp(argv[1], "time") == 0) {
		to = get_ticks();
		while (1) {
		    tn = get_ticks();
		    if ((tn-to)>6000000)
			    free(buf);
			return 0;
		}
	    } else if (strcmp(argv[1], "info") == 0) {
		printf("Device list:\n");
		for (i = 0;i < vs_device_count();i++)
		    printf("%-4d%s\n", i, vs_list[i].name);
		printf("\nCurrent device: ");
		if (vs_is_assigned())
		    printf("%d(0x%x)\n", id, vs_align(id));
		else
		    printf("not assigned yet.\n");
		free(buf);
		return 0;
	    }
	case 1:
	case 0:
	    printf("Usage:\n%s\n", cmdtp->help);
	    free(buf);
	    return -1;
	case 4:
	    if (strcmp(argv[1], "stable1") == 0) {
		capacity = simple_strtoul(argv[2], NULL, 16) * 1024;
		time = simple_strtoul(argv[3], NULL, 10) * 60 * 1000000;

		num.error = 0;
		num.sum_len = 0;
		num.sum = 0;
		tnto = 0;

		if (vs_is_assigned()) {
		    uint8_t *cover = kmalloc(capacity / (vs_align(id) * 8), GFP_KERNEL);
		    memset(cover, 0,capacity / (vs_align(id) * 8));

		    while(1) {
BEGIN:
			num.current = 0;
			num.start = start = vs_align(id) * get_rand(capacity / vs_align(id));
			num.old_len = num.len = vs_align(id) * get_rand((capacity - start)/ vs_align(id));
			printf("start: 0x%x, len: 0x%x\n",
				num.start, num.len);	
			if (flags) {
			    printf("please wait until erase finish\n");
			    vs_erase(start,num.old_len);
			}

			to = get_ticks();
			while(writeread_number(&num)) {
				memcpy(buf, __rnd_lib, num.current);
#if 0
			    for (i = 0;i < num.current;i++)
				buf[i] = random();
#endif

			    vs_write(buf,start,num.current,0);
			    tn = get_ticks();
			    vs_read(s,start,num.current,0);
			    tn = get_ticks();
			    for (i = 0;i<(num.current); i += 4) 
				if ((*(uint32_t *)(buf + i)) ^ (*(uint32_t *)(s + i))) {
				    (num.error)++;
				    printf("error: start is %d, len is %d,buf[%d] is %d, %d\n",start,num.current,i,buf[i],s[i]);

				    tn = get_ticks();
				    if ((tn - to) > time) {
					    printf("breaked because of time limit.\n");
//					if (num.error != 0)
					    printf("The total of error is %d\n", num.error);
					free(buf);
					return 0;
				    } else
					goto BEGIN;
				}
			    start += num.current;
			}

			num.sum_len += num.old_len;

			get_the_coverage(&num, cover);

			tn = get_ticks();
			tnto += (tn -to);
			printf("time consumption: %llds\n", lldiv(tnto, 1000000));

			if (tnto > time) {
	//		    if (num.error != 0)
				printf("number of errors: %d\n", num.error);
			    printf("data size: 0x%llx, coverage: %d%%\n",
				    num.sum_len, ((num.sum * 100 * vs_align(id)) / capacity ));
			    free(buf);
			    return 0;
			}
		    }
		} else {
		    printf("please input the command -- (vstest assign XXXXX) first\n");
		    free(buf);
		    return -1;
		}
	    } 
	    else if (strcmp(argv[1], "stable2") == 0) {
		capacity = simple_strtoul(argv[2], NULL, 16) * 1024;
		time = simple_strtoul(argv[3], NULL, 10) * 60 * 100000;
		num.error = 0;
		if (vs_is_assigned()) {
		    char *buf1 = (char*)0x42000000;
		    char *s1 = (char*)0x40000000;		
		    to = get_ticks();
		    if (flags)
			vs_erase(0,0);
		    vs_write((uint8_t *)buf1,0,capacity,0);
		    vs_read((uint8_t *)s1,0,capacity,0);

		    for (i = 0;i<capacity;i++)
			if (buf1[i] != s1[i]) {
			    (num.error)++;
			    printf("error: len is %d,buf[%d] is %d, %d\n",capacity,i,buf1[i],s[1]);
			    free(buf);
			    return -1;
			}

		    printf("read from and write in big file is successful\n");
		    free(buf);
		    return 0;
		} else {
		    printf("please input the command -- (vstest assign XXXXX) first\n");
		    free(buf);
		    return -1;
		}
	    } 
	    else if (strcmp(argv[1], "exception") == 0) {
		capacity = simple_strtoul(argv[2], NULL, 16) * 1024;
		time = simple_strtoul(argv[3], NULL, 10) * 60 * 100000;
		printf(".....capcity is %d, time is %lld\n", capacity, time);
		int xb = 0;
		num.error = 0;
		num.sum_len = 0;
		num.sum = 0;
		tnto = 0;

		if (vs_is_assigned()) { 
		    while(1) {
			if (xb % 2)
			    start = capacity + vs_align(id) * get_rand(change_len / vs_align(id));
			else
			    start = capacity - vs_align(id) * get_rand(change_len / vs_align(id));

			memcpy(buf, __rnd_lib, change_len);
#if 0
			for (i = 0;i < change_len;i++)
			    buf[i] = random();
#endif
			if (flags)
			    vs_erase(start,change_len);

			to = get_ticks();
			vs_write(buf,start,change_len,0);
			tn = get_ticks();
			vs_read(s,start,change_len,0);
			xb++;
			printf("this time can read and write\n");
			tn = get_ticks();
			tnto += (tn - to);
			printf("....tnto is %lld\n", tnto);
			if (tnto > time) {
				free(buf);
			    return 0;
			}
		    }
		} else {
		    printf("please input the command -- (vstest assign XXXXX) first\n");
		    free(buf);
		    return -1;
		}
	    }

	default:
	    printf("Usage:\n%s\n", cmdtp->help);
	    free(buf);
	    return -1;
    }

    free(buf);
    return 0;
}


U_BOOT_CMD(vstest, CONFIG_SYS_MAXARGS, 1, do_vstest,
	"vstest\n",
	"info - show available command\n"
	"vstest stable1 [capicity] [time]\n"
	"vstest stable2 [capicity] [time]\n"
	"vstest exception [capicity] [time]\n"
	"vstest address [capicity]\n"
	"vstest restore [capicity]\n"
	"vstest speed [length]\n"
	"vstest assign number\n"
	"vstest time\n"
	"vstest info"
	);

