/* 
 * Allocate memory blocks
 *
 * Copyright (C) 2012 Google Finland Oy.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
--------------------------------------------------------------------------------
--
--  Abstract : Allocate memory blocks
--
------------------------------------------------------------------------------*/

#include <linux/kernel.h>
#include <linux/module.h>
/* needed for __init,__exit directives */
#include <linux/init.h>
/* needed for remap_page_range */
#include <linux/mm.h>
/* obviously, for kmalloc */
#include <linux/slab.h>
/* for struct file_operations, register_chrdev() */
#include <linux/fs.h>
/* standard error codes */
#include <linux/errno.h>
/* this header files wraps some common module-space operations ...
   here we use mem_map_reserve() macro */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/list.h>
/* for current pid */
#include <linux/sched.h>

/* Our header */
#include "memalloc.h"

/* module description */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Google Finland Oy");
MODULE_DESCRIPTION("RAM allocation");

/* start of reserved linear memory area */
#ifndef HLINA_START_ADDRESS
#define HLINA_START_ADDRESS 0x02000000  /* 32MB */
#endif

/* end of reserved linear memory area */
#ifndef HLINA_END_ADDRESS
#define HLINA_END_ADDRESS   0x08000000  /* 128MB */
#endif

#define MEMALLOC_SW_MINOR 0
#define MEMALLOC_SW_MAJOR 1
#define MEMALLOC_SW_BUILD ((MEMALLOC_SW_MAJOR * 1000) + MEMALLOC_SW_MINOR)

#define MAX_OPEN 32
#define ID_UNUSED 0xFF
#define MEMALLOC_BASIC 0
#define MEMALLOC_MAX_OUTPUT 1
#define MEMALLOC_X280 2

/* selects the memory allocation method, i.e. which allocation scheme table is used */
unsigned int alloc_method = MEMALLOC_BASIC;

static int memalloc_major = 0;  /* dynamic */

int id[MAX_OPEN] = { ID_UNUSED };

/* module_param(name, type, perm) */
module_param(alloc_method, uint, 0);

/* here's all the must remember stuff */
struct allocation
{
    struct list_head list;
    void *buffer;
    unsigned int order;
    int fid;
};

struct list_head heap_list;

DEFINE_SPINLOCK(mem_lock);

typedef struct hlinc
{
    unsigned int bus_address;
    unsigned int used;
    unsigned int size;
    int file_id;
} hlina_chunk;

unsigned int *size_table;

/* MEMALLOC_BASIC, 24500 chunks, roughly 96MB */
unsigned int size_table_0[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1,
    4, 4, 4, 4, 4, 4, 4, 4,
    10, 10, 10, 10,
    22, 22, 22, 22,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    50, 50, 50, 50, 50, 50, 50,
    75, 75, 75, 75, 75,
    86, 86, 86, 86, 86,
    113, 113,
    152, 152,
    162, 162, 162,
    270, 270, 270,
    400, 400, 400,
    450, 450,
    769, 769,
    2000, 2000, 2000,
    3072,
    8192
};

/* MEMALLOC_MAX_OUTPUT, 65532 chunks, 256MB */
unsigned int size_table_1[] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    16, 16, 16, 16, 16, 16, 16, 16,
    64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64,
    512, 512, 512, 512, 512, 512, 512, 512,
    2675,
    2675,
    21575,
    32960
};

/* MEMALLOC_X280, 48092 chunks, 188MB */
unsigned int size_table_2[] = {
    /* Table calculated for 155x255 max frame size */
    1, 1, 1, 1,
    4, 4, 4, 4,
    16, 16, 16, 16,
    64, 64, 64, 64,
    512,
    2048,
    /* Reference frame buffers, 3x luma + 4x chroma */
    1250, 1250, 1250, 1250,
    2500, 2500, 2500,
    /* Input frame buffer + stab */
    4096, 4096,
    /* 96MB used so far, if more is available we can allocate big chunk for
     * maximum input resolution */
    24500
};

/* PC PCI test */
unsigned int size_table_3[] = {
    1, 1,
    57, 57, 57, 57, 57,
    225, 225, 225, 225, 225,
    384, 384,
    1013, 1013, 1013, 1013, 1013, 1013,
    16000
};

static int max_chunks;

static hlina_chunk hlina_chunks[256];

static int AllocMemory(unsigned *busaddr, unsigned int size, struct file *filp);
static int FreeMemory(unsigned long busaddr);
static void ResetMems(void);

static long memalloc_ioctl(struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
    int err = 0;
    int ret;

    PDEBUG("ioctl cmd 0x%08x\n", cmd);

    if(filp == NULL || arg == 0)
    {
        return -EFAULT;
    }
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if(_IOC_TYPE(cmd) != MEMALLOC_IOC_MAGIC)
        return -ENOTTY;
    if(_IOC_NR(cmd) > MEMALLOC_IOC_MAXNR)
        return -ENOTTY;

    if(_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd));
    else if(_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void *) arg, _IOC_SIZE(cmd));
    if(err)
        return -EFAULT;

    switch (cmd)
    {
    case MEMALLOC_IOCHARDRESET:

        PDEBUG("HARDRESET\n");
        ResetMems();

        break;

    case MEMALLOC_IOCXGETBUFFER:
        {
            int result;
            MemallocParams memparams;

            PDEBUG("GETBUFFER\n");
            spin_lock(&mem_lock);

            if(__copy_from_user
               (&memparams, (const void *) arg, sizeof(memparams)))
            {
                result = -EFAULT;
            }
            else
            {
                result =
                    AllocMemory(&memparams.busAddress, memparams.size, filp);

                if(__copy_to_user((void *) arg, &memparams, sizeof(memparams)))
                {
                    result = -EFAULT;
                }
            }

            spin_unlock(&mem_lock);

            return result;
        }
    case MEMALLOC_IOCSFREEBUFFER:
        {

            unsigned long busaddr;

            PDEBUG("FREEBUFFER\n");
            spin_lock(&mem_lock);
            __get_user(busaddr, (unsigned long *) arg);
            ret = FreeMemory(busaddr);

            spin_unlock(&mem_lock);
            return ret;
        }
    }
    return 0;
}

static int memalloc_open(struct inode *inode, struct file *filp)
{
    int i = 0;

    for(i = 0; i < MAX_OPEN + 1; i++)
    {

        if(i == MAX_OPEN)
            return -1;
        if(id[i] == ID_UNUSED)
        {
            id[i] = i;
            filp->private_data = id + i;
            break;
        }
    }
    PDEBUG("dev opened, instance=%d\n", i);
    return 0;

}

static int memalloc_release(struct inode *inode, struct file *filp)
{

    int i = 0;

    for(i = 0; i < max_chunks; i++)
    {
        if(hlina_chunks[i].file_id == *((int *) (filp->private_data)))
        {
            hlina_chunks[i].used = 0;
            hlina_chunks[i].file_id = ID_UNUSED;
        }
    }
    *((int *) filp->private_data) = ID_UNUSED;
    PDEBUG("dev closed\n");
    return 0;
}

/* VFS methods */
static struct file_operations memalloc_fops = {
  open:memalloc_open,
  release:memalloc_release,
  unlocked_ioctl:memalloc_ioctl,
};

int __init memalloc_init(void)
{
    int result;
    int i = 0;

#if (HLINA_END_ADDRESS <= HLINA_START_ADDRESS)
#error HLINA_END_ADDRESS and/or HLINA_START_ADDRESS not valid!
#endif

    PDEBUG("module init\n");
    printk("memalloc: H1 build %d \n", MEMALLOC_SW_BUILD);
    printk("memalloc: linear memory base = 0x%08x \n", HLINA_START_ADDRESS);
    printk("memalloc: linear memory end  = 0x%08x \n", HLINA_END_ADDRESS);
    printk("memalloc: linear memory size =   %8d KB \n",
           (HLINA_END_ADDRESS - HLINA_START_ADDRESS) / 1024);

    switch (alloc_method)
    {

    case MEMALLOC_MAX_OUTPUT:
        size_table = size_table_1;
        max_chunks = (sizeof(size_table_1) / sizeof(*size_table_1));
        printk(KERN_INFO "memalloc: allocation method: MEMALLOC_MAX_OUTPUT\n");
        break;
    case MEMALLOC_X280:
        size_table = size_table_2;
        max_chunks = (sizeof(size_table_2) / sizeof(*size_table_2));
        printk(KERN_INFO "memalloc: allocation method: MEMALLOC_X280\n");
        break;
    case 3:
        size_table = size_table_3;
        max_chunks = (sizeof(size_table_3) / sizeof(*size_table_3));
        printk(KERN_INFO "memalloc: allocation method: PC PCI test\n");
        break;
    default:
        size_table = size_table_0;
        max_chunks = (sizeof(size_table_0) / sizeof(*size_table_0));
        printk(KERN_INFO "memalloc: allocation method: MEMALLOC_BASIC\n");
        break;
    }

    result = register_chrdev(memalloc_major, "memalloc", &memalloc_fops);
    if(result < 0)
    {
        PDEBUG("memalloc: unable to get major %d\n", memalloc_major);
        goto err;
    }
    else if(result != 0)    /* this is for dynamic major */
    {
        memalloc_major = result;
    }

    ResetMems();

    /* We keep a register of out customers, reset it */
    for(i = 0; i < MAX_OPEN; i++)
    {
        id[i] = ID_UNUSED;
    }

    return 0;

  err:
    PDEBUG("memalloc: module not inserted\n");
    unregister_chrdev(memalloc_major, "memalloc");
    return result;
}

void __exit memalloc_cleanup(void)
{

    PDEBUG("clenup called\n");

    unregister_chrdev(memalloc_major, "memalloc");

    printk(KERN_INFO "memalloc: module removed\n");
    return;
}

module_init(memalloc_init);
module_exit(memalloc_cleanup);

/* Cycle through the buffers we have, give the first free one */
static int AllocMemory(unsigned *busaddr, unsigned int size, struct file *filp)
{

    int i = 0;

    *busaddr = 0;

    for(i = 0; i < max_chunks; i++)
    {
        if(!hlina_chunks[i].used && (hlina_chunks[i].size >= size))
        {
            *busaddr = hlina_chunks[i].bus_address;
            hlina_chunks[i].used = 1;
            hlina_chunks[i].file_id = *((int *) (filp->private_data));
            break;
        }
    }

    if((*busaddr + hlina_chunks[i].size) > HLINA_END_ADDRESS)
    {
        printk("memalloc: FAILED allocation, linear memory overflow\n");
        *busaddr = 0;
    }
    else if(*busaddr == 0)
    {
        printk("memalloc: FAILED allocation, no free chunck of size = %d\n",
               size);
    }
    else
    {
        PDEBUG("memalloc: Allocated chunck; size requested %d, reserved: %d\n",
               size, hlina_chunks[i].size);
    }

    return 0;
}

/* Free a buffer based on bus address */
static int FreeMemory(unsigned long busaddr)
{
    int i = 0;

    for(i = 0; i < max_chunks; i++)
    {
        if(hlina_chunks[i].bus_address == busaddr)
        {
            hlina_chunks[i].used = 0;
            hlina_chunks[i].file_id = ID_UNUSED;
        }
    }

    return 0;
}

/* Reset "used" status */
void ResetMems(void)
{
    int i = 0;
    unsigned int ba = HLINA_START_ADDRESS;

    for(i = 0; i < max_chunks; i++)
    {
        hlina_chunks[i].bus_address = ba;
        hlina_chunks[i].used = 0;
        hlina_chunks[i].file_id = ID_UNUSED;
        hlina_chunks[i].size = 4096 * size_table[i];

        ba += hlina_chunks[i].size;
    }

    printk("memalloc: %d bytes (%d KB) configured\n",
           ba - HLINA_START_ADDRESS, (ba - HLINA_START_ADDRESS) / 1024);

    if(ba > HLINA_END_ADDRESS)
    {
        printk("memalloc: WARNING! Linear memory top limit exceeded!\n");
    }

}
