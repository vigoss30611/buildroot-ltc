/*************************************************************************
    > File Name: ccf_test.c
    > Author: tsico
    > Mail: can.chai@infotm.com 
    > Created Time: Tue 04 Jul 2017 04:56:57 PM CST
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>    
#include <string.h>    
#include <errno.h>     
#include <malloc.h>    
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h> 

#define ENABLE_CHECK "devmem 0x2D00A25C"
#define FREQ_CHECK "devmem 0x2D00A254"
#define TEST_TEMP_FILE_NAME "test_popen.txt"
#define CLK_DEBUGFS "/sys/kernel/debug/clk/clk_summary"
#define MOUNT_CLK_DEBUGFS "mount -t debugfs none /sys/kernel/debug"
#define ECHO_MMC1_DEV_NAME "echo sd-mmc1 > /sys/kernel/debug/clk/configure/name"
#define ECHO_DISABLE "echo disable > /sys/kernel/debug/clk/configure/enable"
#define ECHO_ENABLE "echo enable > /sys/kernel/debug/clk/configure/enable"
#define ECHO_27M_CLK "echo 27000000 > /sys/kernel/debug/clk/configure/freq"
#define ECHO_DEFAULT_CLK "echo 39600000 > /sys/kernel/debug/clk/configure/freq"
#define EPLL_FREQ 594

int CCF_enable_test(void){
	FILE   *stream;
	FILE   *wstream;
	char   buf[1024]; 
	unsigned long ul;
	int success_count = 0;

	memset( buf, '\0', sizeof(buf) );

	if(access(CLK_DEBUGFS, F_OK)){
		system(MOUNT_CLK_DEBUGFS);
	}

	/*enable mmc1 clk*/
	system(ECHO_MMC1_DEV_NAME);
	system(ECHO_DISABLE);
	/*check if the disable bit is set or not*/
	/*1.read the output of devmem to the FILE   *stream with pipe*/
	stream = popen( ENABLE_CHECK, "r");
	/*2.open a new w+ file*/
	wstream = fopen( TEST_TEMP_FILE_NAME, "w+");
	/*3.read the FILE   *stream data to a buffer*/
	fread( buf, sizeof(char), sizeof(buf), stream);
	/*4.check the disable bit*/
	ul = strtol(buf, NULL, 16);
	if(ul & 0x1)
		success_count++;
	else
		printf("disable sd-mmc1 controller failed\n");
	pclose( stream );
	fclose( wstream );
	memset( buf, '\0', sizeof(buf) );
	/*enable mmc1 clk*/
	system(ECHO_ENABLE);
	/*check if the enable bit is set or not*/
	/*1.read the output of devmem to the FILE   *stream with pipe*/
	stream = popen( ENABLE_CHECK, "r");
	/*2.open a new w+ file*/
	wstream = fopen( TEST_TEMP_FILE_NAME, "w+");
	/*3.read the FILE   *stream data to a buffer*/
	fread( buf, sizeof(char), sizeof(buf), stream);
	/*4.check the enable bit*/
	ul = strtol(buf, NULL, 16);
	if(ul & 0x1)
		printf("enable sd-mmc1 controller failed\n");
	else
		success_count++;
	pclose( stream );
	fclose( wstream );
	return success_count;
}

int CCF_Freq_test(void){
	FILE   *stream;
	FILE   *wstream;
	char   buf[1024]; 
	char ctoi, div;
	int success_count = 0;

	memset( buf, '\0', sizeof(buf) );

	if(access(CLK_DEBUGFS, F_OK)){
		system(MOUNT_CLK_DEBUGFS);
	}

	/*enable mmc1 clk*/
	system(ECHO_MMC1_DEV_NAME);
	system(ECHO_27M_CLK);
	/*check if the div bits are right or not*/
	/*1.read the output of devmem to the FILE   *stream with pipe*/
	stream = popen( FREQ_CHECK, "r");
	/*2.open a new w+ file*/
	wstream = fopen( TEST_TEMP_FILE_NAME, "w+");
	/*3.read the FILE   *stream data to a buffer*/
	fread( buf, sizeof(char), sizeof(buf), stream);
	/*4.check the div bits*/
	div = (EPLL_FREQ / 27) - 1;
	ctoi = strtol(buf, NULL, 16);
	if(ctoi == div)
		success_count++;
	else
		printf("set sd-mmc1 27M clk failed\n");
	pclose( stream );
	fclose( wstream );
	memset( buf, '\0', sizeof(buf) );
	/*enable mmc1 clk*/
	system(ECHO_DEFAULT_CLK);
	/*check if the div bits are right or not*/
	/*1.read the output of devmem to the FILE   *stream with pipe*/
	stream = popen( FREQ_CHECK, "r");
	/*2.open a new w+ file*/
	wstream = fopen( TEST_TEMP_FILE_NAME, "w+");
	/*3.read the FILE   *stream data to a buffer*/
	fread( buf, sizeof(char), sizeof(buf), stream);
	/*4.check the div bits*/
	div = 14;
	ctoi = strtol(buf, NULL, 16);
	if(ctoi == div)
		success_count++;
	else
		printf("set sd-mmc1 39.6M clk failed\n");
	pclose( stream );
	fclose( wstream );
	return success_count;
}

int main(void){
	int ret;
	ret = CCF_enable_test();
	if(ret == 2)
		printf("The enable test of CCF is SUCCESS\n");
	else
		printf("The enable test of CCF is FAILED\n");
	ret = 0;
	ret = CCF_Freq_test();
	if(ret == 2)
		printf("The Freq test of CCF is SUCCESS\n");
	else
		printf("The Freq test of CCF is FAILED\n");
	return 0;
}
