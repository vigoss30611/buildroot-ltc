/* by warits, Tue Mar 26 */

#include <common.h>
#include <asm/io.h>
#include <lowlevel_api.h>
#include <efuse.h>
#include <preloader.h>
#include <items.h>
#include <bootlist.h>

int dlib_get_parameter(uint8_t *buf)
{
    return 0;
}

int dlib_get_logo(uint8_t *buf)
{
    uint8_t *p = NULL;
    int ret;	

    ret = vs_assign_by_id(DEV_FND, 0);
    if(ret)
	    printf("Erro: Can not get the system disk ready, for display_log\n");
 
    p = load_image("logo", NULL, buf, storage_device());

    return !p;
}

int dlib_mark_initialized(void)
{

    return 0;
}

