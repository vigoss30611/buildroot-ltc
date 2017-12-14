
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <linux/crc32.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/poll.h>
#include <linux/vmalloc.h>
#include <mach/items.h>
#include <mach/rballoc.h>
#include <mach/nvedit.h>
#include <mach/mem-reserve.h>

#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

#define nvprintf(x...) printk(KERN_ERR x)
#define ENV_OFFSET	0x3c00000

env_t *env_ptr = NULL;
extern int nand_env_save(uint8_t *data, int len);
extern int infotm_nftl_part_write_data(loff_t offset, size_t size, unsigned char *buf);

static struct delayed_work __save_work;
static void __env_save_func(struct work_struct *work)
{
#define __ENV_NODE "/dev/block/nandblk0"
    struct file *fp;
    mm_segment_t old_fs;
    loff_t pos = ENV_OFFSET;
    static int ret;

    fp = filp_open(__ENV_NODE, O_WRONLY, 0);
    if(IS_ERR(fp)) {
        printk(KERN_ERR "%s: open %s failed. save delayed\n",
                __func__, __ENV_NODE); 
	schedule_delayed_work(&__save_work, HZ * 6);
        return ;
    }

    old_fs = get_fs();
    set_fs(KERNEL_DS);
    ret = vfs_write(fp, (uint8_t *)env_ptr, CONFIG_ENV_SIZE, &pos);
    set_fs(old_fs);

    filp_close(fp, 0);
    printk(KERN_ERR "%d bytes wrote to %s\n", 
            ret, __ENV_NODE);

    return ;
}

static void __env_save(void) {
    __env_save_func(NULL);
}

static void nftl_env_save(void) {
	infotm_nftl_part_write_data(ENV_OFFSET, CONFIG_ENV_SIZE, (uint8_t *)env_ptr);
}

int __init initenv(void)
{
    uint8_t *p = rbget("ubootenv");

    if(!p) {
        nvprintf("ubootenv: not available.\n");
        return -1;
    }

    env_ptr = alloc_bootmem(CONFIG_ENV_SIZE);
    if(!env_ptr) {
        nvprintf("ubootenv: alloc memory failed\n");
        return -1;
    }

    memcpy((uint8_t *)env_ptr, p, CONFIG_ENV_SIZE);

#if 1
    if(crc32_uboot(0, env_ptr->data, ENV_SIZE) != env_ptr->crc) {
        nvprintf("ubootenv crc32 mismatch. new: 0x%08x, store: 0x%08x\n",
                crc32_uboot(0, env_ptr->data, ENV_SIZE), env_ptr->crc);
        env_ptr = NULL;
        return -2;
    }
#endif

    //INIT_DELAYED_WORK(&__save_work, __env_save_func);
    nvprintf("ubootenv found.\n");
    printenv();

    return 0;
}

char env_get_char(int index)
{
    return env_ptr->data[index];
}

uint8_t *env_get_addr(int index)
{
    return &env_ptr->data[index];
}

void env_crc_update (void)
{
    env_ptr->crc = crc32_uboot(0, env_ptr->data, ENV_SIZE);
}

/************************************************************************
 * Match a name / name=value pair
 *
 * s1 is either a simple 'name', or a 'name=value' pair.
 * i2 is the environment index for a 'name2=value2' pair.
 * If the names match, return the index for the value2, else NULL.
 */

int envmatch (uint8_t *s1, int i2)
{

    while (*s1 == env_get_char(i2++))
        if (*s1++ == '=')
            return(i2);
    if (*s1 == '\0' && env_get_char(i2-1) == '=')
        return(i2);
    return(-1);
}

/*
 * state 0: finish printing this string and return (matched!)
 * state 1: no matching to be done; print everything
 * state 2: continue searching for matched name
 */
int printenv(void)
{
    int i, j;
    char c, buf[256];

    if(!env_ptr) 
        return -1;

    i = 0;
    buf[255] = '\0';

    while (env_get_char(i) != '\0') {
        j = 0;
        do {
            buf[j++] = c = env_get_char(i++);
            if (j == sizeof(buf) - 1) {
                nvprintf("%s\n", buf);
                j = 0;
            }
        } while (c != '\0');

        if (j)
            nvprintf("%s\n", buf);
    }

    return i;
}

/************************************************************************
 * Set a new environment variable,
 * or replace or delete an existing one.
 *
 * This function will ONLY work with a in-RAM copy of the environment
 */
int setenv (const char *name, const char *data)
{
    int   len, oldval;
    uint8_t *env, *nxt = NULL;

    uint8_t *env_data = env_get_addr(0);

    printk(KERN_ERR "setenv: %s: %s\n", name, data);
    if(!env_ptr) 
        return -1;

    if (!env_data)	/* need copy in RAM */
        return 1;

    if (strchr(name, '=')) {
        nvprintf ("## Error: illegal character '=' in variable name \"%s\"\n", name);
        return 1;
    }

    /*
     * search if variable with this name already exists
     */
    oldval = -1;
    for (env=env_data; *env; env=nxt+1) {
        for (nxt=env; *nxt; ++nxt)
            ;
        if ((oldval = envmatch((uint8_t *)name, env-env_data)) >= 0)
            break;
    }

    /*
     * Delete any existing definition
     */
    if (oldval >= 0) {
	/* Should not cover the read-only env */
	if(!strncmp("ro.", name, 3))
		return -EPERM;
        if (*++nxt == '\0') {
            if (env > env_data) {
                env--;
            } else {
                *env = '\0';
            }
        } else {
            for (;;) {
                *env = *nxt++;
                if ((*env == '\0') && (*nxt == '\0'))
                    break;
                ++env;
            }
        }
        *++env = '\0';
    }

    /* Delete only ? */
    if (!data) {
        env_crc_update ();
        return 0;
    }

    /*
     * Append new definition at the end
     */
    for (env=env_data; *env || *(env+1); ++env)
        ;
    if (env > env_data)
        ++env;
    /*
     * Overflow when:
     * "name" + "=" + "val" +"\0\0"  > ENV_SIZE - (env-env_data)
     */
    len = strlen(name) + 2;

    len += strlen(data) + 1;
    if (len > (&env_data[ENV_SIZE]-env)) {
        nvprintf ("## Error: environment overflow, \"%s\" deleted\n", name);
        return 1;
    }
    while ((*env = *name++) != '\0')
        env++;

    /* append = after name */
    *env = '=';
    while ((*++env = *data++) != '\0');

    /* end is marked with double '\0' */
    *++env = '\0';

    /* Update CRC */
    env_crc_update ();
    return 0;
}

#define iswhite(x) (x == ' ' || x == '\t')
static int get_para(char * buf, const char *desc, int para, int all)
{
    const char *p;
    char *q;
    int i, n = -1;

    for(p = desc, i = 0, q = buf;
            (*p != '\0') && (i < 1024);
            p++, i++) {

        if((p != desc && iswhite(*(p - 1)) && !iswhite(*p))
                || (p == desc && !iswhite(*p)))
            n += (all && (n == para))? 0: 1;

        if(n == para) {
            *q++ = *p;
//            printk(KERN_ERR "n: %d, c=%c\n", n, *p);
            if((!all && iswhite(*(p+1)))
                    || *(p+1) == '\n' || !*(p+1)) {
//                printk(KERN_ERR "%d: %d: %d\n", (!all && iswhite(*(p+1))),
//                        *(p+1) == '\n', !*(p+1));
                *q = 0;
                return 0;
            }
        }
    }

    *buf = 0;
    return -1;
}



int setenv_compact(const char *desc)
{
    char buf[256], name[256], s[256];

    get_para(buf, desc, 1, 0);

    if(strncmp(buf, "put", 3) == 0) {
        get_para(name, desc, 2, 0);
        get_para(s,    desc, 3, 1);

//        printk(KERN_ERR "put: name: %s, string: %s\n", name, s);
        if(name[0])
            setenv(name, s[0]?s: NULL);
    } else if(strncmp(buf, "clear", 5) == 0) {
	if(!strncmp("ro.", name, 3))
		return -EPERM;
        get_para(name, desc, 2, 0);
        if(name[0])
            setenv(name, NULL);
    } else if(strncmp(buf, "show", 4) == 0) {
        printenv();
    } else if(strncmp(buf, "clean", 5) == 0) {
        cleanenv();
    } else if(strncmp(buf, "commit", 6) == 0) {
        saveenv();
    }

    return 0;
}

/************************************************************************
 * Look up variable from environment,
 * return address of storage for that variable,
 * or NULL if not found
 */

char *getenv (char *name)
{
    int i, nxt;

    if(!env_ptr) 
        return NULL;

    for (i=0; env_get_char(i) != '\0'; i=nxt+1) {
        int val;

        for (nxt=i; env_get_char(nxt) != '\0'; ++nxt) {
            if (nxt >= CONFIG_ENV_SIZE) {
                return (NULL);
            }
        }
        if ((val=envmatch((uint8_t *)name, i)) < 0)
            continue;
        return ((char *)env_get_addr(val));
    }

    return (NULL);
}

int saveenv (void)
{
    if(!env_ptr) 
        return -1;

    nvprintf ("storing environment ...\n");
    //__env_save();
    //nand_env_save((uint8_t *)env_ptr, CONFIG_ENV_SIZE);
    nftl_env_save();
    return 0;
}

int cleanenv()
{
	int ret;
	char *buff;
	char *env_data;
	char *p;
	char *n;

	buff = vmalloc(CONFIG_ENV_SIZE);
	if(buff == NULL)
		return -ENOMEM;
	p = buff;

	env_data = env_get_addr(0);
	for (n = env_data; n < (env_data + CONFIG_ENV_SIZE); ++n) {
		if (*n == '\0') {
			break;
		} else {
			if (!strncmp(n, "rm.", 3)) {
				while(*n)
					++n;
				continue;
			}
			ret = sprintf(p, n, 256);
			p += ret;
			*p++ = '\0';
			n += ret;
		}
	}
	memset(env_data, 0, CONFIG_ENV_SIZE);
	memcpy(env_data, buff, p-buff);
	vfree(buff);
        env_crc_update();
	return 0;
}
