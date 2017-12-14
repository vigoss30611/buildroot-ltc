#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>

extern void gpio_reset(void);

int tps65910_init_ready(void)
{
    gpio_reset();
    if(iic_init(0, 1, 0x5a, 0, 1, 0) == 0){
        printf("iic_init() fail!!!\n");
        return 0;
    }else
        printf("iic_init() success!!!\n");

    return 1;
}
