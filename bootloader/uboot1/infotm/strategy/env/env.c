/*
 * (C) Copyright 2008
 * Stuart Wood, Lab X Technologies <stuart.wood@labxtechnologies.com>
 *
 * (C) Copyright 2004
 * Jian Zhang, Texas Instruments, jzhang@ti.com.

 * (C) Copyright 2000-2006
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* #define DEBUG */

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

/* references to names in env_common.c */
extern uchar default_environment[];

char * env_name_spec = "INFOTM";

env_t *env_ptr = 0;

/* local functions */
static void use_default(void);

DECLARE_GLOBAL_DATA_PTR;

uchar env_get_char_spec (int index)
{
	return ( *((uchar *)(gd->env_addr + index)) );
}


/* this is called before nand_init()
 * so we can't read Nand to validate env data.
 * Mark it OK for now. env_relocate() in env_common.c
 * will call our relocate function which does the real
 * validation.
 *
 * When using a NAND boot image (like sequoia_nand), the environment
 * can be embedded or attached to the U-Boot image in NAND flash. This way
 * the SPL loads not only the U-Boot image from NAND but also the
 * environment.
 */
int env_init(void)
{
        env_ptr = (env_t *)(rballoc("ubootenv", CONFIG_ENV_SIZE));
        memset(env_ptr, 0, CONFIG_ENV_SIZE);

        gd->env_addr  = 0;
        gd->env_valid = 0;

        return 0;
}

/*
 * The legacy NAND code saved the environment in the first NAND device i.e.,
 * nand_dev_desc + 0. This is also the behaviour using the new NAND code.
 */
int saveenv(void)
{
    int ret = 0;
    loff_t offs;
    uint64_t size = 0;

    offs = storage_offset("reserved");
    size = storage_size("reserved");

	vs_erase(offs, size);
	ret = vs_write(env_ptr, offs, CONFIG_ENV_SIZE, 0);

    return ret;
}

int readenv (void)
{
    int ret, dev_id;

    dev_id = storage_device();
	vs_assign_by_id(dev_id, 0);
    ret = vs_read((void *)env_ptr, storage_offset("reserved"), CONFIG_ENV_SIZE, 0);

    return ret < CONFIG_ENV_SIZE;
}

/*
 * The legacy NAND code saved the environment in the first NAND device i.e.,
 * nand_dev_desc + 0. This is also the behaviour using the new NAND code.
 */
void env_relocate_spec (void)
{
    //int ret;

    if(readenv())
        use_default();

    if (crc32(0, env_ptr->data, ENV_SIZE) != env_ptr->crc)
        use_default();
    else
        gd->env_valid = 1;

    gd->env_addr = (ulong)&(env_ptr->data);
    if(!getenv("serialex"))
        setenv("serialex", efuse_mac2serial());

    if(!getenv("macex"))
        setenv("macex", "0");
}

static void use_default()
{
	puts ("*** Warning - bad CRC or NAND, using default environment\n\n");
	set_default_env();
	saveenv();
}

