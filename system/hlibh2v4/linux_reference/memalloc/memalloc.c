/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------
--
--  Abstract : Allocate memory blocks
--
------------------------------------------------------------------------------*/

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

/* Our header */
#include "memalloc.h"

#ifndef HLINA_START_ADDRESS
#define HLINA_START_ADDRESS             0x02000000
#endif

#ifndef HLINA_SIZE
#define HLINA_SIZE                      96
#endif

#ifndef HLINA_TRANSL_OFFSET
#define HLINA_TRANSL_OFFSET             0x0
#endif

/* the size of chunk in MEMALLOC_DYNAMIC */
#define CHUNK_SIZE                      (PAGE_SIZE * 4)

/* memory size in MBs for MEMALLOC_DYNAMIC */
unsigned int alloc_size = HLINA_SIZE;
unsigned int alloc_base = HLINA_START_ADDRESS;

/* user space SW will substract HLINA_TRANSL_OFFSET from the bus address
 * and decoder HW will use the result as the address translated base
 * address. The SW needs the original host memory bus address for memory
 * mapping to virtual address. */
unsigned int addr_transl = HLINA_TRANSL_OFFSET;

static int memalloc_major = 0;  /* dynamic */

/* module_param(name, type, perm) */
module_param(alloc_size, uint, 0);
module_param(alloc_base, uint, 0);
module_param(addr_transl, uint, 0);

static DEFINE_SPINLOCK(mem_lock);

typedef struct hlinc {
        u32 bus_address;
        u16 chunks_reserved;
        const struct file *filp; /* Client that allocated this chunk */
} hlina_chunk;

static hlina_chunk *hlina_chunks = NULL;
static size_t chunks = 0;

static int AllocMemory(unsigned *busaddr, unsigned int size,
                       const struct file *filp);
static int FreeMemory(unsigned long busaddr, const struct file *filp);
static void ResetMems(void);

static long memalloc_ioctl(struct file *filp, unsigned int cmd,
                           unsigned long arg)
{
        int ret = 0;
        MemallocParams memparams;
        unsigned long busaddr;

        PDEBUG("ioctl cmd 0x%08x\n", cmd);

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if(_IOC_TYPE(cmd) != MEMALLOC_IOC_MAGIC)
                return -ENOTTY;
        if(_IOC_NR(cmd) > MEMALLOC_IOC_MAXNR)
                return -ENOTTY;

        if(_IOC_DIR(cmd) & _IOC_READ)
                ret = !access_ok(VERIFY_WRITE, arg, _IOC_SIZE(cmd));
        else if(_IOC_DIR(cmd) & _IOC_WRITE)
                ret = !access_ok(VERIFY_READ, arg, _IOC_SIZE(cmd));
        if(ret)
                return -EFAULT;

        spin_lock(&mem_lock);

        switch (cmd) {
	case MEMALLOC_IOCGMEMBASE:
		__put_user(alloc_base, (unsigned long *) arg);
		break;
        case MEMALLOC_IOCHARDRESET:
                PDEBUG("HARDRESET\n");
                ResetMems();
                break;
        case MEMALLOC_IOCXGETBUFFER:
                PDEBUG("GETBUFFER");

                ret = copy_from_user(&memparams, (MemallocParams*)arg,
                                     sizeof(MemallocParams));
                if(ret) break;

                ret = AllocMemory(&memparams.busAddress, memparams.size, filp);

                memparams.translationOffset = addr_transl;

                ret |= copy_to_user((MemallocParams*)arg, &memparams,
                                    sizeof(MemallocParams));

                break;
        case MEMALLOC_IOCSFREEBUFFER:
                PDEBUG("FREEBUFFER\n");

                __get_user(busaddr, (unsigned long *) arg);
                ret = FreeMemory(busaddr, filp);
                break;
        }

        spin_unlock(&mem_lock);

        return ret ? -EFAULT: 0;
}

static int memalloc_open(struct inode *inode, struct file *filp)
{

        PDEBUG("dev opened\n");
        return 0;
}

static int memalloc_release(struct inode *inode, struct file *filp)
{
        int i = 0;

        for(i = 0; i < chunks; i++) {
                spin_lock(&mem_lock);
                if(hlina_chunks[i].filp == filp) {
                        printk(KERN_WARNING "memalloc: Found unfreed memory at release time!\n");

                        hlina_chunks[i].filp = NULL;
                        hlina_chunks[i].chunks_reserved = 0;
                }
                spin_unlock(&mem_lock);
        }
        PDEBUG("dev closed\n");
        return 0;
}

void __exit memalloc_cleanup(void)
{
        if(hlina_chunks != NULL)
                vfree(hlina_chunks);

        unregister_chrdev(memalloc_major, "memalloc");

        PDEBUG("module removed\n");
        return;
}

/* VFS methods */
static struct file_operations memalloc_fops = {
        .owner = THIS_MODULE,
        .open = memalloc_open,
        .release = memalloc_release,
        .unlocked_ioctl = memalloc_ioctl
};

int __init memalloc_init(void)
{
       int result;
		
        PDEBUG("module init\n");
		
		printk("memalloc: Linear Memory Allocator\n");
        printk("memalloc: Linear memory base = 0x%08x\n", alloc_base);

        chunks = (alloc_size * 1024 * 1024) / CHUNK_SIZE;

        printk(KERN_INFO "memalloc: Total size %d MB; %d chunks"
               " of size %lu\n", alloc_size, chunks, CHUNK_SIZE);

        hlina_chunks = (hlina_chunk *) vmalloc(chunks * sizeof(hlina_chunk));
        if (hlina_chunks == NULL) {
                printk(KERN_ERR "memalloc: cannot allocate hlina_chunks\n");
                result = -ENOMEM;
                goto err;
        }

        result = register_chrdev(memalloc_major, "memalloc", &memalloc_fops);
        if(result < 0) {
                PDEBUG("memalloc: unable to get major %d\n", memalloc_major);
                goto err;
        } else if(result != 0) { /* this is for dynamic major */
                memalloc_major = result;
        }

        ResetMems();

        return 0;

err:
        if(hlina_chunks != NULL)
                vfree(hlina_chunks);

        return result;
}

/* Cycle through the buffers we have, give the first free one */
static int AllocMemory(unsigned *busaddr, unsigned int size,
                       const struct file *filp)
{

        int i = 0;
        int j = 0;
        unsigned int skip_chunks = 0;

        /* calculate how many chunks we need; round up to chunk boundary */
        unsigned int alloc_chunks = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;

        *busaddr = 0;

        /* run through the chunk table */
        for(i = 0; i < chunks;) {
                skip_chunks = 0;
                /* if this chunk is available */
                if(!hlina_chunks[i].chunks_reserved) {
                        /* check that there is enough memory left */
                        if (i + alloc_chunks > chunks)
                                break;

                        /* check that there is enough consecutive chunks available */
                        for (j = i; j < i + alloc_chunks; j++) {
                                if (hlina_chunks[j].chunks_reserved) {
                                        skip_chunks = 1;
                                        /* skip the used chunks */
                                        i = j + hlina_chunks[j].chunks_reserved;
                                        break;
                                }
                        }

                        /* if enough free memory found */
                        if (!skip_chunks) {
                                *busaddr = hlina_chunks[i].bus_address;
                                hlina_chunks[i].filp = filp;
                                hlina_chunks[i].chunks_reserved = alloc_chunks;
                                break;
                        }
                } else {
                        /* skip the used chunks */
                        i += hlina_chunks[i].chunks_reserved;
                }
        }

        if(*busaddr == 0) {
                printk("memalloc: Allocation FAILED: size = %d\n", size);
                return -EFAULT;
        } else {
                PDEBUG("MEMALLOC OK: size: %d, reserved: %ld\n", size,
                       alloc_chunks * CHUNK_SIZE);
        }

        return 0;
}

/* Free a buffer based on bus address */
static int FreeMemory(unsigned long busaddr, const struct file *filp)
{
        int i = 0;


        for(i = 0; i < chunks; i++) {
                /* user space SW has stored the translated bus address, add addr_transl to
                 * translate back to our address space */
                if(hlina_chunks[i].bus_address == busaddr + addr_transl) {
                        if(hlina_chunks[i].filp == filp) {
                                hlina_chunks[i].filp = NULL;
                                hlina_chunks[i].chunks_reserved = 0;
                        } else {
                                printk(KERN_WARNING "memalloc: Owner mismatch while freeing memory!\n");
                        }
                        break;
                }
        }
        return 0;
}

/* Reset "used" status */
static void ResetMems(void)
{
        int i = 0;
        unsigned int ba = alloc_base;

        for(i = 0; i < chunks; i++) {
                hlina_chunks[i].bus_address = ba;
                hlina_chunks[i].filp = NULL;
                hlina_chunks[i].chunks_reserved = 0;

                ba += CHUNK_SIZE;
        }
}

module_init(memalloc_init);
module_exit(memalloc_cleanup);

/* module description */
MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("Google Finland Oy");
MODULE_DESCRIPTION("Linear RAM allocation");
