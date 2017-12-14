#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <getopt.h>
#include <string.h>
#define __USE_GNU 1
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <qsdk/wifi.h>
#include <qsdk/items.h>
#include <qsdk/sys.h>
#include <qsdk/event.h>

//#define DEBUG

#ifdef DEBUG
#define Dprint(...) printf(__VA_ARGS__)
#else
#define Dprint(...)
#endif

#define VERSION_SD              167
#define VERSION_SDIO            168

typedef unsigned int uint32_t;
struct delayline{
	int read;
	int write;
};
struct delayline delayline_ok;
int save_count;
struct Depar{
	struct delayline values;
	char kind[30];
}Deparameter;

char test_type[10];
int test_begin = 0;

#define MISS_FILE "may be the pre-result file is not exist"
#define DELAYLINE_ERR "the delayline is not fit the controller"

int ioctrl_rdata = 0;
#define Q3F_delayline 1396844298
#define Q3_delayline 1396843530

#define SD_delayline 1
#define SDIO_delayline 2

/*sdio test parameter*/
int ap_enable_g = 0;
int sta_enable_g = 0;
int sta_connect_g = 0;
int ap_enable_ok = 0;
int ap_disable_ok = 0;

void set_delayline(struct delayline arg, int objection_id)
{
	char set[60] = {0};
	memset(set,0,sizeof(set));
	sprintf(set,"echo %02x%02x%02x > /sys/kernel/debug/mmc1/delayline",arg.write
								,arg.read, objection_id);
	system(set);
	system("cat /sys/kernel/debug/mmc1/delayline");
}

void sd_sdio_de(int de, int chan)
{
	char buf[60] = {0};
	sprintf(buf,"echo %d%d > /sys/kernel/debug/mmc1/reset",de,chan);
	system(buf);
}

int analyse_result(void)
{
	FILE *fd = NULL;
	int ret = -1;
	long len = -1;
	char *buf = NULL;
	int i = 0;

	if(test_begin){
		set_delayline(delayline_ok,SD_delayline); 
		sd_sdio_de(0, 1);
		sleep(1);
		sd_sdio_de(1, 1);
		sleep(1);
	}
	if(!access("/dev/mmcblk0p1",F_OK)){
		system("mount -t vfat /dev/mmcblk0p1 /mnt");
		fd = fopen("/mnt/delayline_test_result", "ab+");
		if(!fd){
			printf("open save file err!\n");
			return -1;
		}
		fseek(fd, 0, SEEK_END);
		len = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		buf = (char *)malloc(len + 1);
		if(buf == NULL){
			printf("malloc buf err!\n");
			return -1;
		}
		ret = fread(buf, 1, len, fd);
		if(ret == -1){
			printf("delayline write err!\n");
			return -1;
		}
		printf("---------------------auto test result--------------------\n");
		printf("platform : %s | value \n",
				ioctrl_rdata == Q3_delayline ? "Q3" : "Q3F");
		while(i < len){
			printf("\t\t0x%08x\n",ioctrl_rdata == Q3_delayline ? 
					(buf[i + 4] << 24) + (buf[i] << 16) : 
					(buf[i + 4] << 23) + (buf[i] << 16));
			i += 8;
		}
		fclose(fd);
		system("umount /mnt");
		system("sync");
	}
	sd_sdio_de(0 ,1);
	return 0;
}

int input_parameter()
{
	int inputval = 0;
	char keyboard[30] = {0};
	int fd = 0, ret = 0;
	char *buf = NULL;
	int upper_boundary = 0;

	fd = open("/dev/VerID", O_RDWR);
	if(fd < 0){
		printf("Open /dev/VerID Fail\n");
		return -1;
	}
	ret = ioctl(fd, VERSION_SD, &ioctrl_rdata);
	if(ret < 0){
		printf("ioctl ERR\n");
		return -1;
	}

	close(fd);

	printf("Do you need to dispaly the pre-result?(yes/no)\n");
	scanf("%s",keyboard);
	if(!strcmp("yes",keyboard)){
		if(analyse_result() == -1)
			printf("display the pre-result fail. %s or %s.\n",MISS_FILE,DELAYLINE_ERR);
	}

	printf("Please select the testing object:\n       mmc\n       sd\n       sdio\n");
	scanf("%s",test_type);
	if(ioctrl_rdata == Q3_delayline){
		upper_boundary = 255;
		buf = "0x0~FF";	
	}
	else if(ioctrl_rdata == Q3F_delayline){
		upper_boundary = 127;
		buf = "0x0~0x7F";
	}
delayline:
	memset(keyboard,0,sizeof(keyboard));
	printf("Please input start value of write delayline in hexadecimal format(%s) :\n",buf);
	scanf("%s",keyboard);
	inputval = strtol(keyboard,NULL,16);
	if(inputval < 0 || inputval > upper_boundary){
		printf("Input write delayline value 0x%x overflow ! try again...\n",inputval);
		goto delayline;
	}
	Deparameter.values.write = inputval;
	memset(keyboard,0,sizeof(keyboard));
	printf("Please input start value of read delayline in hexadecimal format(%s) :\n",buf);
	scanf("%s",keyboard);
	inputval = strtol(keyboard,NULL,16);
	if(inputval < 0 || inputval > upper_boundary){
		printf("Input read delayline value 0x%x overflow ! try again...\n",inputval);
		goto delayline;
	}
	Deparameter.values.read = inputval;

step:
	memset(keyboard,0,sizeof(keyboard));
	printf("Please input the kind of delayline test you want(all/write/read) :\n");
	scanf("%s",keyboard);
	if(strcmp("all",keyboard) && strcmp("write",keyboard) && strcmp("read",keyboard)){
		printf("Input err! try again...");
		goto step;
	}
	strcpy(Deparameter.kind, keyboard);
	return 0;
}

void save_delayline(struct delayline value)
{
	FILE *fd = NULL;
	struct delayline gpio = value;
	fd = fopen("/mnt/delayline_test_result", "ab+");
	if(!fd){
		printf("open save file err!\n");
	}
	fseek(fd, 0, SEEK_END);
	fwrite(&gpio, sizeof(int) * 2, 1, fd);
	fclose(fd);
}

int check_file(struct delayline delayline_value)
{
	int fd,savefd;
	int i, count,len;
	char a[1048576]={0},b[1048576]={0};
	savefd = open("/mnt/hello.txt",O_RDWR|O_CREAT|O_DIRECT|O_TRUNC,0644 );
	if(savefd < 0){
		printf("open save file err!\n");
		return -1;
	}
	fd = open("/dev/urandom", O_RDONLY);
	if(fd < 0){
		printf("open source dev fail!\n");
		return -1;
	}
	if ((len = read(fd,a,1048576)) == 1048576)
	{ 
		count = write(savefd, a,len );
		printf("write savefd byte count : %d\n",count);
	}else{
		printf("read data from /dev/urandom err!\n");
		return -1;
	}
	close(fd);
	close(savefd);
	fd = open("/mnt/hello.txt", O_RDONLY,0644);
	if(fd < 0){
		printf("open des dev fail!\n");
		return -1;
	}
    	lseek(fd,0,SEEK_SET);
	len=read( fd,b,1048576 ); 
	if(len != 1048576){
		printf("read from sd card err!\n");
		return -1;
	}
	close(fd);
	for( i = 0;i<1048576;i++){
		if(a[i] != b[i]){
			printf("result is wrong, a[%d]:%d, b[%d]:%d\n",i,a[i],i,b[i]);
			return -1;
		}
	}
    	if(i == 1048576 ){
		printf("write and read test ok!\n");
		if(save_count == 3){
			delayline_ok.read = delayline_value.read;
			delayline_ok.write = delayline_value.write;
		}
		//save_delayline(delayline_value);
		save_count ++;
	}
	return 0;
}

void cal_cycle(int *test_cycle)
{
	uint32_t gpio_value = 0;
	char key = 0;
	if(!strcmp(Deparameter.kind,"all"))
		key = 1;
	if(!strcmp(Deparameter.kind,"write"))
		key = 2;
	if(!strcmp(Deparameter.kind,"read"))
		key = 3;
	switch(key){
		case 1 :
			if(ioctrl_rdata == Q3F_delayline){
				gpio_value = Deparameter.values.write << 7;
				gpio_value += Deparameter.values.read;
				*test_cycle = 0x3FFF - gpio_value;
			}else if(ioctrl_rdata == Q3_delayline){
				gpio_value = Deparameter.values.write << 8;
				gpio_value += Deparameter.values.read;
				*test_cycle = 0xFFFF - gpio_value;
			}
			break;
		case 2 :
			if(ioctrl_rdata == Q3F_delayline)
				*test_cycle = 0x7F - Deparameter.values.write;
			else if(ioctrl_rdata == Q3_delayline)
				*test_cycle = 0xFF - Deparameter.values.write;
			break;
		case 3 :
			if(ioctrl_rdata == Q3F_delayline)
				*test_cycle = 0x7F - Deparameter.values.read;
			else if(ioctrl_rdata == Q3_delayline)
				*test_cycle = 0xFF - Deparameter.values.read;
			break;
	}
}

int get_next_delayline(void)
{
	int value = 0;
	if(ioctrl_rdata == Q3_delayline)
		value = 0xFF;
	else if(ioctrl_rdata == Q3F_delayline)
		value = 0x7F;
	if(!strcmp(Deparameter.kind,"read") && 
			(Deparameter.values.read <= value))
		Deparameter.values.read ++;
	else if(!strcmp(Deparameter.kind,"write") && 
			(Deparameter.values.write <= value))
		Deparameter.values.write ++;
	else if(!strcmp(Deparameter.kind,"all") && 
			(Deparameter.values.write <= value)){
		if(Deparameter.values.read == value){
			Deparameter.values.write++;
			Deparameter.values.read = 0x0;
		}
		Deparameter.values.read ++;
	}
	else
		return -1;
	return 0;
}

/*sdio test func*/
void show_net_info(struct net_info_t *info)
{
    printf("hwaddr is %s\n", info->hwaddr);
    printf("ipaddr is %s\n", info->ipaddr);
    printf("gateway is %s\n", info->gateway);
    printf("mask is %s\n", info->mask);
    printf("dns1 is %s\n", info->dns1);
    printf("dns2 is %s\n", info->dns2);
    printf("server is %s\n", info->server);
    printf("lease is %s\n", info->lease);
}

void show_results(struct wifi_config_t *config)
{
    printf("netword_id is %d\n", config->network_id);
    printf("status is %d \n", config->status);
    printf("list_type is %d \n", config->list_type);
    printf("level is %d \n", config->level);
    printf("ssid is %s \n", config->ssid);
    printf("bssid is %s \n", config->bssid);
    printf("key_management is %s \n", config->key_management);
    printf("pairwise_ciphers is %s \n", config->pairwise_ciphers);
    printf("psk is %s \n", config->psk);
}

void test_wifi_state_handle(char *event, void *arg)
{
    struct wifi_config_t *sta_get_scan_result = NULL, *sta_get_scan_result_prev = NULL, *sta_get_config = NULL;

    printf("func %s, event is %s, arg is %s \n", __func__, event, (char *)arg);

    if (!strncmp(event, WIFI_STATE_EVENT, strlen(WIFI_STATE_EVENT))) {
	if (!strncmp((char *)arg, STATE_STA_ENABLED, strlen(STATE_STA_ENABLED))) {
	    sta_enable_g = 1;
	    printf("sta enabled \n");
	} else if (!strncmp((char *)arg, STATE_STA_DISABLED, strlen(STATE_STA_DISABLED))) {
	    sta_enable_g = 0;
	    printf("sta disabled \n");
	} else if (!strncmp((char *)arg, STATE_SCAN_COMPLETE, strlen(STATE_SCAN_COMPLETE))) {
	    printf("scan complete, now get scan results \n");

	    sta_get_scan_result =  wifi_sta_get_scan_result();
	    if (sta_get_scan_result != NULL) {
		show_results(sta_get_scan_result);
	    }

	    while (sta_get_scan_result != NULL) {
		sta_get_scan_result_prev = sta_get_scan_result;

		sta_get_scan_result = wifi_sta_iterate_config(sta_get_scan_result_prev);
		free(sta_get_scan_result_prev);
		if (sta_get_scan_result != NULL) {
		    show_results(sta_get_scan_result);
		}
	    }
	    
	    if (sta_get_scan_result != NULL) {
		free(sta_get_scan_result);
	    }
	} else if (!strncmp((char *)arg, STATE_STA_CONNECTED, strlen(STATE_STA_CONNECTED))) {
	    sta_connect_g = 1;
	    printf("sta connected \n");
	    sta_get_config = wifi_sta_get_config();
	    if (sta_get_config != NULL) {
		show_results(sta_get_config);
		free(sta_get_config);
	    }
	} else if (!strncmp((char *)arg, STATE_STA_DISCONNECTED, strlen(STATE_STA_DISCONNECTED))) {
	    sta_connect_g = 0;
	    printf("sta disconnected \n");
	} else if (!strncmp((char *)arg, STATE_AP_ENABLED, strlen(STATE_AP_ENABLED))) {
	    ap_enable_g = 1;
	    printf("ap enabled \n");
	} else if (!strncmp((char *)arg, STATE_AP_DISABLED, strlen(STATE_AP_DISABLED))) {
	    ap_enable_g = 0;
	    printf("ap disabled \n");
	} else if (!strncmp((char *)arg, STATE_MONITOR_ENABLED, strlen(STATE_MONITOR_ENABLED))) {
	    printf("monitor enabled");
	} else if (!strncmp((char *)arg, STATE_MONITOR_DISABLED, strlen(STATE_MONITOR_DISABLED))) {
	    printf("monitor disabled");
	}
    } else if (!strncmp(event, STATE_AP_STA_CONNECTED, strlen(STATE_AP_STA_CONNECTED))) {
	printf("ap sta connected info \n");
	show_net_info((struct net_info_t *)arg);
    } else if (!strncmp(event, STATE_AP_STA_DISCONNECTED, strlen(STATE_AP_STA_DISCONNECTED))) {
	printf("ap sta disconected info \n");
	show_net_info((struct net_info_t *)arg);
    }
}

int main(int argc, char **argv) { 
	int i;
	int cycle = 0;
	int ret = 0;

	if(input_parameter() == -1){
		printf("something happened when read Version ID\n");
		return -1;
	}

	if(access("/sys/kernel/debug/mmc1",F_OK)){
		system("mount -t debugfs none /sys/kernel/debug");
	}

	cal_cycle(&cycle);

	if(!strcmp("sd", test_type)){
		test_begin = 1;

		for(i = 0; i <= cycle ; i++)
		{
			printf("---------------------------------------------\n");
			set_delayline(Deparameter.values, SD_delayline);
			if(!access("/dev/mmcblk0p1",F_OK))
			{
				system("mount -t vfat /dev/mmcblk0p1 /mnt");
				if(check_file(Deparameter.values) == -1){
					sd_sdio_de(0, 1);
					printf("ERR happened when check file\n");
					return 0;
				}
				system("umount /mnt");
				system("sync");
				sleep(1);
			}else
				printf("/dev/mmcblk0p1 not esixt!\n");

			if(get_next_delayline() == -1){
				sd_sdio_de(0, 1);
				printf("The sd test delayline value match the biggest\n");
				return 0;
			}
			sd_sdio_de(0, 1);
			sleep(3);
			sd_sdio_de(1, 1);
			sleep(2);
		}
		system("umount /sys/kernel/debug");
	}else if(!strcmp("sdio", test_type)){
		if (argc > 1) {
			printf("more args, usage is: ./wifi_delayline_test \n");
			return 0;
		}else
			printf("start to test\n");
		wifi_start_service(test_wifi_state_handle);
		for(i = 0; i <= cycle ; i++){
			printf("---------------------------------------------------------------------------\n");
			set_delayline(Deparameter.values, SDIO_delayline);
			ap_enable_ok = 0;
			ret = wifi_ap_enable();
			if(ret != 0){
				printf("**enable wifi ap fail!**\n");
				if(get_next_delayline() == -1){
					printf("The sdio test delayline value match the biggest\n");
					return 0;
				}
			}
			while(ap_enable_g != 1){
				usleep(100);
			}
			ap_enable_ok = 1;
			ap_disable_ok = 0;
			ret = wifi_ap_disable();
			if(ret != 0){ 
				printf("**close wifi ap fail!**\n");
				if(get_next_delayline() == -1){
					printf("The sdio test delayline value match the biggest\n");
					return 0;
				}
			}
			while(ap_enable_g != 0){
				usleep(100);
			}
			wifi_stop_service();
			ap_disable_ok = 1;
			if(get_next_delayline() == -1){
				printf("The sdio test delayline value match the biggest\n");
				return 0;
			}
			if(ap_enable_ok && ap_disable_ok)
				save_delayline(Deparameter.values);
			sd_sdio_de(1, 2);
			usleep(100);
		}
	}else if(!strcmp("mmc", test_type)){
		printf("Normally, we haven't surpported mmc delayline testing\n");
	}
	if(analyse_result() == -1)
		printf("something happened when dispaly the test result\n");
	return 0;
}
