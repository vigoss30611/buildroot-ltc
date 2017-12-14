/*************************************************************************
    > File Name: gpio_test.c
    > Author: tsico
    > Mail: can.chai@infotm.com 
    > Created Time: Wed 05 Jul 2017 04:16:45 PM CST
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

#define TEST_TEMP_FILE_NAME "test_popen.txt"
#define SET_GPIO123 "echo 123 > sys/class/gpio/export"
#define DIRECTION_IN "echo in > sys/class/gpio/gpio123/direction"
#define DIRECTION_OUT "echo out > sys/class/gpio/gpio123/direction"
#define DIRECTION_CHECK "devmem 0x20f01ec8"
#define VALUE_HIGH "echo 1 > sys/class/gpio/gpio123/value"
#define VALUE_LOW "echo 0 > sys/class/gpio/gpio123/value"
#define IN_VALUE_CHECK "devmem 0x20f01ec0"
#define OUT_VALUE_CHECK "devmem 0x20f01ec4"
#define EDGE_BOTH "echo both > sys/class/gpio/gpio123/edge"
#define EDGE_RISING "echo rising > sys/class/gpio/gpio123/edge"
#define EDGE_FALLING "echo falling > sys/class/gpio/gpio123/edge"
#define EDGE_NONE "echo none > sys/class/gpio/gpio123/edge"
#define EDGE_CHECK "devmem 0x20f01edc"
#define	ACTIVE_LOW "echo 1 > sys/class/gpio/gpio123/active_low" 
#define CAT_VALUE "cat sys/class/gpio/gpio123/value"

unsigned long pipe_out_print(char * content){
	FILE   *stream;  
	FILE   *wstream; 
	char   buf[1024];
	char ctoi;

	/*1.read the output to the FILE   *stream with pipe*/
	stream = popen( content, "r");
	/*2.open a new w+ file*/
	wstream = fopen( TEST_TEMP_FILE_NAME, "w+");
	/*3.read the FILE   *stream data to a buffer*/ 
	fread( buf, sizeof(char), sizeof(buf), stream);
	/*4.check the Corresponding bit*/
	ctoi = strtol(buf, NULL, 16);
	return ctoi;
}

int gpio_direction_test(void){
	int sucess_count = 0;
	char output_data;

	system(DIRECTION_IN);
	output_data = pipe_out_print(DIRECTION_CHECK);
	if(output_data == 0)
		sucess_count++;

	system(DIRECTION_OUT);
	output_data = pipe_out_print(DIRECTION_CHECK);
	if(output_data == 1)
		sucess_count++;
	return sucess_count;
}

int gpio_value_test(void){
	int sucess_count = 0;
	char output_data;

	output_data = pipe_out_print(DIRECTION_CHECK);
	if(output_data == 0){
		system(VALUE_HIGH);
		output_data = pipe_out_print(IN_VALUE_CHECK);
		if(output_data == 1)
			sucess_count++;

		system(VALUE_LOW);
		output_data = pipe_out_print(IN_VALUE_CHECK);
		if(output_data == 0)
			sucess_count++;
	}else{
		system(VALUE_HIGH);
		output_data = pipe_out_print(OUT_VALUE_CHECK);
		if(output_data == 1)
			sucess_count++;

		system(VALUE_LOW);
		output_data = pipe_out_print(OUT_VALUE_CHECK);
		if(output_data == 0)
			sucess_count++;
	}
	return sucess_count;
}

int gpio_edge_test(void){
	int sucess_count = 0;
	char output_data;    

	system(EDGE_BOTH);
	output_data = pipe_out_print(EDGE_CHECK);
	if(output_data == 7 || output_data == 6)
		sucess_count++;

	system(EDGE_RISING);
	output_data = pipe_out_print(EDGE_CHECK);
	if(output_data == 5 || output_data == 4)
		sucess_count++;

	system(EDGE_FALLING);
	output_data = pipe_out_print(EDGE_CHECK);
	if(output_data == 3 || output_data == 2)
		sucess_count++;
}

int gpio_active_low_test(void){
	int sucess_count = 0;
	char output_data;    

	/*set the gpio normal model and output high*/
	system(EDGE_NONE);
	system(DIRECTION_OUT);
	system(VALUE_HIGH);

	output_data = pipe_out_print(CAT_VALUE);
	if(output_data == 1)
		sucess_count++;

	system(ACTIVE_LOW);

	output_data = pipe_out_print(CAT_VALUE);
	if(output_data == 0)
		sucess_count++;
	return sucess_count;
}

int main(void){
	int ret;
	/*enter gpio123 test*/
	system(SET_GPIO123);
	/*direction test*/
	ret = gpio_direction_test();
	if(ret == 2)
		printf("GPIO direction test is SUCCESS\n");
	else{
		printf("GPIO direction test is FAILED\n");
		return -1;
	}
	/*value test*/
	ret = gpio_value_test();
	if(ret == 2)
		printf("GPIO value test is SUCCESS\n");
	else{
		printf("GPIO value test is FAILED\n");
		return -1;
	}
	/*edge test*/
	ret = gpio_edge_test();
	if(ret == 3)
		printf("GPIO edge test is SUCCESS\n");
	else{
		printf("GPIO edge test is FAILED\n");
		return -1;
	}
	/*active low test*/
	ret = gpio_active_low_test();
	if(ret == 2)
		printf("GPIO active low test is SUCCESS\n");
	else{
		printf("GPIO active low test is FAILED\n");
		return -1;
	}
	return 0;
}
