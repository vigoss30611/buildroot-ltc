#include <common.h>                                              
#include <command.h>                                                   
#include <asm/io.h>                                              
#include <bootl.h>                                      
int do_bootl (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{                                                                
	return  bootl(); 
}
U_BOOT_CMD(                                                     
		bootl, CONFIG_SYS_MAXARGS, 1,   do_bootl,               
		"start uImage at address 'addr'",                       
		"zImage is unsafe, using of this is not recommended.\n" 
		);                                                      

