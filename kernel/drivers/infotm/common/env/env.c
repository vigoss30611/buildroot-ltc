
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/bootmem.h>
#include <linux/crc32.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/ioport.h>
#include <linux/poll.h>
#include <linux/vmalloc.h>
#include <mach/items.h>
#include <mach/rballoc.h>
#include <mach/nvedit.h>
#include <mach/mem-reserve.h>

#define ENV_MAJOR	213
#define ENV_NAME	"env"
#define ENV_SET_SAVE		_IOW('U', 213, int)
#define ENV_GET_SAVE		_IOW('U', 214, int)
#define ENV_CLN_SAVE		_IOW('U', 215, int)

#define INFOTM_ENV_MAGIC		"infotm_env"
#define infotm_env_dbg			printk
#define XMK_STR(x)	#x
#define MK_STR(x)	XMK_STR(x)

struct env_item_t {
	char name[128];
	char value[512];
};

static struct env_t *infotm_env_ptr = NULL;
static int env_boot_err_stat = 0;
static void infotm_printenv(void);

static int infotm_envmatch(uint8_t *s1, int i2)
{
    while (*s1 == infotm_env_ptr->data[i2++])
        if (*s1++ == '=')
            return(i2);
    if (*s1 == '\0' && infotm_env_ptr->data[i2-1] == '=')
        return(i2);
    return(-1);
}

char *infotm_getenv(char *name)
{
    int i, nxt;

    if(!infotm_env_ptr) 
        return NULL;

	mutex_lock(&infotm_env_ptr->env_lock);
    for (i=0; infotm_env_ptr->data[i] != '\0'; i=nxt+1) {
        int val;

        for (nxt=i; infotm_env_ptr->data[nxt] != '\0'; ++nxt) {
            if (nxt >= CONFIG_ENV_SIZE) {
            	mutex_unlock(&infotm_env_ptr->env_lock);
                return (NULL);
            }
        }
        if ((val=infotm_envmatch((uint8_t *)name, i)) < 0)
            continue;
		mutex_unlock(&infotm_env_ptr->env_lock);
        return ((char *)&infotm_env_ptr->data[val]);
    }
    mutex_unlock(&infotm_env_ptr->env_lock);

    return (NULL);
}

int infotm_setenv(char *name, char *data)
{
    int   len, oldval;
    uint8_t *env, *nxt = NULL;
    uint8_t *env_data = NULL;

    printk(KERN_ERR "setenv: %s: %s\n", name, data);
    if(!infotm_env_ptr) 
        return -1;

	env_data = (uint8_t *)(&infotm_env_ptr->data[0]);
    if (!env_data)	/* need copy in RAM */
        return 1;

    if (strchr(name, '=')) {
        infotm_env_dbg ("## Error: illegal character '=' in variable name \"%s\"\n", name);
        return 1;
    }

    /*
     * search if variable with this name already exists
     */
	mutex_lock(&infotm_env_ptr->env_lock);
    oldval = -1;
    for (env=env_data; *env; env=nxt+1) {
        for (nxt=env; *nxt; ++nxt)
            ;
        if ((oldval = infotm_envmatch((uint8_t *)name, env-env_data)) >= 0)
            break;
    }

    /*
     * Delete any existing definition
     */
    if (oldval >= 0) {
	/* Should not cover the read-only env */
		if(!strncmp("ro.", name, 3)) {
			mutex_unlock(&infotm_env_ptr->env_lock);
			return -EPERM;
		}
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
        infotm_env_ptr->crc = (crc32((0 ^ 0xffffffffL), infotm_env_ptr->data, ENV_SIZE) ^ 0xffffffffL);
        infotm_env_ptr->write_data(infotm_env_ptr, 0, CONFIG_ENV_SIZE, (uint8_t *)infotm_env_ptr);
        mutex_unlock(&infotm_env_ptr->env_lock);
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
        infotm_env_dbg ("## Error: environment overflow, \"%s\" deleted\n", name);
        mutex_unlock(&infotm_env_ptr->env_lock);
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
    infotm_env_ptr->crc = (crc32((0 ^ 0xffffffffL), infotm_env_ptr->data, ENV_SIZE) ^ 0xffffffffL);
	infotm_env_ptr->write_data(infotm_env_ptr, 0, CONFIG_ENV_SIZE, (uint8_t *)infotm_env_ptr);
	mutex_unlock(&infotm_env_ptr->env_lock);

    return 0;
}

int infotm_cleanenv(void)
{
	int ret;
	char *buff;
	char *env_data;
	char *p;
	char *n;

    if(!infotm_env_ptr) 
        return -1;

	buff = vmalloc(CONFIG_ENV_SIZE);
	if(buff == NULL)
		return -ENOMEM;
	p = buff;

	mutex_lock(&infotm_env_ptr->env_lock);
	env_data = (uint8_t *)(&infotm_env_ptr->data[0]);
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
	infotm_env_ptr->crc = (crc32((0 ^ 0xffffffffL), infotm_env_ptr->data, ENV_SIZE) ^ 0xffffffffL);
	infotm_env_ptr->write_data(infotm_env_ptr, 0, CONFIG_ENV_SIZE, (uint8_t *)infotm_env_ptr);
	mutex_unlock(&infotm_env_ptr->env_lock);
	return 0;
}

int infotm_cleanallenv(void)
{
	if (!infotm_env_ptr)
		return -1;
	
	mutex_lock(&infotm_env_ptr->env_lock);

	memset(infotm_env_ptr->data, 0, CONFIG_ENV_SIZE);
	infotm_env_ptr->crc = (crc32((0 ^ 0xffffffffL), infotm_env_ptr->data, ENV_SIZE) ^ 0xffffffffL);
	infotm_env_ptr->write_data(infotm_env_ptr, 0, CONFIG_ENV_SIZE, (uint8_t *)infotm_env_ptr);

	mutex_unlock(&infotm_env_ptr->env_lock);

	return 0;
}

/*
  warning: not strip space for simplicity
  .e.g: set platform value to bsp
  $echo "platform:bsp" > /sys/class/infotm_env/env_operation
*/
static ssize_t infotm_set_env(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	char *name = NULL;
	char *value = NULL;
	char *pstr = NULL;

	if (!buf) {
		printk("invalid args, empty \n");
		return count;
	}

	if (!strncmp("clearall", buf, strlen("clearall"))) {
		infotm_cleanallenv();
		return count;
	}

	pstr = strchr(buf, ':');
	if (!pstr) {
		printk("useage echo \"name:value\" > /sys/class/infotm_env/env_operation \n");
		return count;
	}
	*pstr = 0;
	pstr++;

	name = buf;
	value = pstr;
	printk("env_name:%s   env_value: %s \n", name, value);
    infotm_setenv(name, value);
	return count;
}

static ssize_t infotm_get_env(struct class *class, struct class_attribute *attr, char *buf)
{
    printk("bootup env_crc check sta: %s \n", env_boot_err_stat ? "error":"OK");

    if (infotm_env_ptr != NULL) {
        infotm_printenv();
    } else {
        printk("ENV: env-variable is not avail \n");
    }
	return 0;
}

static CLASS_ATTR(env_operation, 0666, infotm_get_env, infotm_set_env);

static int env_dev_open(struct inode *fi, struct file *fp)
{
	return 0;
}

static int env_dev_read(struct file *filp, char __user *buf, size_t length, loff_t *offset)
{
	return 0;
}

static long env_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)                       
{
	struct env_item_t *env_item = NULL;
	char buf[512], *data;

	switch (cmd) {
		case ENV_GET_SAVE:
			if (copy_from_user(buf, (char *)arg, 512))
				return -EFAULT;
			data = infotm_getenv(buf);
			//infotm_printenv();
			if (data) {
				snprintf(buf, 512, "%s", data);
				if (copy_to_user((char *)arg, buf, 512))
					return -EFAULT;
			}
			break;
		case ENV_SET_SAVE:
			env_item = kzalloc(sizeof(struct env_item_t), GFP_KERNEL);
			if (env_item == NULL)
				return -EFAULT;
			if (copy_from_user(env_item, (char *)arg, sizeof(struct env_item_t))) {
				kfree(env_item);
				return -EFAULT;
			}
			if (infotm_setenv(env_item->name, env_item->value)) {
				kfree(env_item);
				return -EFAULT;
			}
			//infotm_printenv();
			kfree(env_item);
			break;
		case ENV_CLN_SAVE:
			infotm_cleanallenv();
			break;
		default:
			break;
	}

	return 0;
}

static ssize_t env_dev_write(struct file *filp, const char __user *buf, size_t length, loff_t *offset)
{
	return 0;
}

static const struct file_operations env_dev_fops = {
	.read  = env_dev_read,
	.write = env_dev_write,
	.open  = env_dev_open,
	.unlocked_ioctl = env_dev_ioctl,
};

void infotm_printenv(void)
{
    int i, j;
    char c, buf[256];

    if(!infotm_env_ptr) 
        return;

    i = 0;
    buf[255] = '\0';

    while (infotm_env_ptr->data[i] != '\0') {
        j = 0;
        do {
            buf[j++] = c = infotm_env_ptr->data[i++];
            if (j == sizeof(buf) - 1) {
                infotm_env_dbg("%s\n", buf);
                j = 0;
            }
        } while (c != '\0');

        if (j)
            infotm_env_dbg("%s\n", buf);
    }

    return;
}

int infotm_register_env(struct env_t *env)
{
	int error = 0, cal_crc = 0, save_crc = 0;
	uint8_t *env_data = NULL;

	if ((!env->read_data) || (!env->write_data))
		return -1;

	env_data = kzalloc(CONFIG_ENV_SIZE, GFP_KERNEL);
	if (!env_data)
		return -1;

	memset(env->data, 0, ENV_SIZE);
	error = env->read_data(env, 0, CONFIG_ENV_SIZE, env_data);
	if (error) {
		kfree(env_data);
		return error;
	}

	save_crc = *(uint32_t *)env_data;
	cal_crc = (crc32((0 ^ 0xffffffffL), env_data + sizeof(uint32_t), ENV_SIZE) ^ 0xffffffffL);
	if (save_crc != cal_crc) {
        env_boot_err_stat = 1;
        memset(env->data, 0 , ENV_SIZE); /* clear all environment in ram */
		infotm_env_dbg("env crc error save:%x calc:%x!\n", save_crc, cal_crc);
	} else {
	    env->crc = save_crc;
        memcpy(env->data, env_data + sizeof(uint32_t), ENV_SIZE);
    }
    kfree(env_data);
    env_data = NULL;

	mutex_init(&env->env_lock);
	env->cls.name = INFOTM_ENV_MAGIC;
   	error = class_register(&env->cls);
	if(error) {
		infotm_env_dbg(" class register env fail!\n");
		return error;
	}
	if( class_create_file(&env->cls, &class_attr_env_operation) < 0 )
		infotm_env_dbg("Error in Creating 'env_operation' sys mode\n");

	error = register_chrdev(ENV_MAJOR, ENV_NAME, &env_dev_fops);
	if (error < 0){
		infotm_env_dbg("Register char device for env failed\n");
		return -EFAULT;
	}
	device_create(&env->cls, NULL, MKDEV(ENV_MAJOR, 0), NULL, ENV_NAME);
	infotm_env_ptr = env;

    if (cal_crc == save_crc) { /* Only display env when CRC pass  */
        infotm_printenv();
    }
    return 0;
}
