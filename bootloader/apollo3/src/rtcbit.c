
/* simplest memory alloc method, by warits */
#include <linux/stddef.h>
#include <malloc.h>
#include <ius.h>
#include <vstorage.h>
#include <storage.h>
#include <rballoc.h>
#include <asm-arm/arch-apollo3/imapx_base_reg.h>
#include <asm-arm/io.h>

#if defined(CONFIG_PRELOADER)
#include "preloader.h"
#define printf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#define memcpy irf->memcpy
#define malloc irf->malloc
#endif

#define MAX_RTCBITS  64
#define MAX_RTCBITS_PRESERVE  64

/* emulation on PC */
#if 0
uint32_t rtcinfo[8] = {0};
void writel(uint32_t val, int addr) {
	rtcinfo[addr] = val;
}
uint32_t readl(int addr) {
	return rtcinfo[addr];
}
#define RTC_INFO(x) (x)
#define rballoc(a, b) malloc(b)
#endif /* emulation end */

struct rtcbit {
    char name[16];
    int  count;
    int  loc;
    int  preserve; /* 1:  information will be preserved on CPU_RESET, 0: information will be erased on CPU_RESET */
    int  resv[3];
} *rtcbitsp = NULL;

const struct rtcbit gbits[] = {
    {"resetflag", 8, 0, 1},
    {"holdbase", 24, 0, 0},
    {"batterycap", 8, 0, 1},
    {"retry_reboot", 8, 0, 0},
    {"fastboot", 1, 0, 0},
    {"forceshut", 1, 0, 0},
    {"sleeping", 3, 0, 1},
    {NULL, 0, 0, 0},
};
char rtcbit_save[512];

static struct rtcbit * __rtcbit_getid(const char *name)
{
    struct rtcbit *p = rtcbitsp;

    if(!p)
        return NULL;

    for(; p->count; p++) {
        if(!name) {
            //printf("rtcbits: %s: %d@%d\n", p->name, p->count, p->loc);
			continue;
		}

        if(strncmp(p->name, name, 16) == 0)
            return p;
    }

    return NULL;
}

void rtcbit_set_rbmem(void)
{
	char *temp;

	temp = rballoc("rtcbits", 0x200);
	if (!temp)
		return;

	memcpy(temp, rtcbit_save, 0x200);
	rtcbitsp = temp;
}

int rtcbit_init(void)
{
    int i, n = 0, n_preserve = 0;

//#if defined(CONFIG_PRELOADER)
 //   rtcbitsp = malloc(0x200);
//#else
    //rtcbitsp = rballoc("rtcbits", 0x1000);
    //#endif
    rtcbitsp = rtcbit_save;
    if(!rtcbitsp) {
        printf("rtcbits: alloc memory failed.\n");
        return -1;
    }

    //printf("rtcbits_v2: initializing ...\n");
    for(i = 0; gbits[i].count; i++) {
        strncpy(rtcbitsp[i].name, gbits[i].name, 16);
        rtcbitsp[i].count = gbits[i].count;
        rtcbitsp[i].preserve = gbits[i].preserve;
        if(rtcbitsp[i].preserve) {
            rtcbitsp[i].loc = n_preserve;
            n_preserve += gbits[i].count;
        } else {
            rtcbitsp[i].loc = n;
            n += gbits[i].count;
        }

        //printf("rtcbits: %s, %d@%d\n", rtcbitsp[i].name,
                //rtcbitsp[i].count, rtcbitsp[i].loc);

        if(gbits[i].count > 32 || gbits[i].count <= 0 || n > MAX_RTCBITS || n_preserve > MAX_RTCBITS_PRESERVE) {
            printf("rtcbits: exceed max bits. (%d, %d, %d)\n", gbits[i].count, n, n_preserve);
            while(1);
        }
    }

	rtcbitsp[i].count = 0;

    return n + n_preserve;
}

int rtcbit_set(const char *name, uint32_t pattern)
{
    struct rtcbit *p = __rtcbit_getid(name);
    int i, tmp = 0, loc;

    if(!p) {
        printf("rtcbits: can not get id: %s\n", name);
        return -1;
    }

    for(i = 0, loc = p->loc; i < p->count; i++, loc++) {
        if(!i || loc % 8 == 0) {
            tmp = readl(RTC_INFO(loc / 8, p->preserve));
            //printf("rtcbits: get rtcinfo: %d, 0x%02x\n", loc / 8, tmp);
        }

        if(pattern & (1 << i))
            tmp |= (1 << loc % 8);
        else
            tmp &= ~(1 << loc % 8);

        if(loc % 8 == 7 || i == p->count - 1) {
            //printf("rtcbits: update rtcinfo: %d, 0x%02x\n", loc / 8, tmp);
            writel(tmp, RTC_INFO(loc / 8, p->preserve));
        }
    }

    return 0;
}

int rtcbit_clear(const char *name) {
    return rtcbit_set(name, 0);
}

uint32_t rtcbit_get(const char *name)
{
    struct rtcbit *p = __rtcbit_getid(name);
    int i, tmp = 0, pattern = 0, loc;

    if(!p) {
        printf("rtcbits: can not get id: %s\n", name);
        return -1;
    }

    for(i = 0, loc = p->loc; i < p->count; i++, loc++) {
        if(!i || loc % 8 == 0)
            tmp = readl(RTC_INFO(loc / 8, p->preserve));

        pattern |= (!!(tmp & (1 << loc % 8))) << i;
    }

    printf("rtcbits: get bits for %s: 0x%02x\n", p->name, pattern);
    return pattern;
}

void rtcbit_print(void)
{
    __rtcbit_getid(NULL);
}

