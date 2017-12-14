
/* simplest memory alloc method, by warits */

#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <nand.h>
#include <ius.h>
#include <vstorage.h>
#include <storage.h>
#include <burn.h>
#include <rballoc.h>

#define RB_BLOCK 0x8000
/*
 * |<--- 32K --->|<--- else --->|
 *    head array    buffer repo
 */

#if !defined(CONFIG_PRELOADER)
#define rbprintf printf
#else
#include "preloader.h"
#define rbprintf irf->printf
#define strncmp irf->strncmp
#define strncpy irf->strncpy
#define memset irf->memset
#endif

void rbinit(void) {
    memset((void *)___RBASE, 0, RB_BLOCK);
}

void * rballoc(const char * owner, uint32_t len)
{
    struct reserved_buffer *p = (void *)___RBASE;
    uint32_t used = 0;

    if(!owner || !len)
        return NULL;

    for(; *p->owner; p++) {
        if(strncmp(p->owner, owner, 32) == 0) {
            rbprintf("rballoc: shared owner (%s) 0x%x@0x%08x\n",
                    owner, p->length, p->start);
            return (void *)p->start;
        }

        used += p->length;
    }

    if(RB_BLOCK + used + len > ___RLENGTH) {
        rbprintf("rballoc: OOM, 0x%x bytes denied.\n", len);
        return NULL;
    }

    strncpy(p->owner, owner, 32);
    p->start = ___RBASE + RB_BLOCK + used;
    p->length = (len + 0xfff) & ~0xfff;

    rbprintf("rballoc: 0x%x@0x%08x allocated for %s\n", p->length,
            p->start, p->owner);

    return (void *)p->start;
}

void * rbget(const char *owner)
{
    struct reserved_buffer *p = (void *)___RBASE;

    if(!owner)
        return NULL;

    for(; *p->owner; p++)
        if(strncmp(p->owner, owner, 32) == 0)
            return (void *)p->start;

    return NULL;
}

void rbsetint(const char *owner, uint32_t val)
{
    uint8_t *p = rballoc(owner, 0x1000);

    if(p)
        *(uint32_t *)p = val;
}

uint32_t rbgetint(const char *owner)
{
    uint8_t *p = rbget(owner);

    return p? *(uint32_t *)p: 0;
}

void rblist(void)
{
    struct reserved_buffer *p = (void *)___RBASE;

    for(; *p->owner; p++) {
        rbprintf("%16s 0x%08x ", p->owner, p->start);
#if !defined(CONFIG_PRELOADER)
        print_size(p->length, "\n");
#endif
    }
}

