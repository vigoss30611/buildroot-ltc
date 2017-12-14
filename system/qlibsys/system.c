#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/statfs.h>
#include <sys/reboot.h>
#include <sys/mount.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <linux/watchdog.h>
#include <linux/reboot.h>
#include <err.h>
#include <qsdk/items.h>
#include "sys.h"
#include "fb.h"

#define SZ_1K					0x400
#define UPGRADE_INNER_SIZE		128
#define	EMMC_IMAGE_OFFSET		0x200000
#define ENV_PATH 				"/dev/env"
#define SPI_BLK 				"/dev/spiblock0"
#define EMMC_BLK 				"/dev/mmcblk0"
#define WDT_DEV_NAME     		"/dev/watchdog"
#define LED_SYS_PATH	 		"/sys/class/leds"
#define GPIO_EREQUEST_PATH		"/sys/class/gpio/export"
#define GPIO_DIRETION_PATH		"/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE_PATH			"/sys/class/gpio/gpio%d/value"
#define SYSTEM_VERSION_PATH		"/root/version"
#define GSENSOR_ATTR_ENABLE_PATH	"/sys/devices/virtual/misc/%s/attribute/enable"
#define ENV_SET_SAVE			_IOW('U', 213, int)
#define ENV_GET_SAVE			_IOW('U', 214, int)
#define ENV_CLN_SAVE			_IOW('U', 215, int)
#define LED_ON		1
#define LED_OFF		0

struct env_item_t {
	char name[128];
	char value[512];
};

char leds_name[512];

static int system_get_soc_info(char *type, soc_info_t *s_info) {
	 FILE *fp;
	 char split[] = ":";
	 char linebuf[512];
	 char *result = NULL;

	 memset(s_info, 0x0, sizeof (* s_info));
	 fp = fopen("/proc/efuse", "r");
	 if(fp == NULL) {
		 printf("failed to open soc info node %s \n", strerror(errno));
		 memset(s_info, 0xFF, sizeof (* s_info));
		 return -1;
	 }

	while (fgets(linebuf, 512, fp)) {
		result = strtok(linebuf, split);
		if (!strcmp(result, type)) {
			result = strtok(NULL, split);
			if(!strncmp(type, "TYPE", 4)) {
				s_info->chipid = strtol(result, NULL ,16);
			}
			if(!strncmp(type, "MAC", 3)) {
				char mac[2];
				int i;
				for(i = 0; i < 6; i++) {
					mac[0] = result[2*i];
					mac[1] = result[2*i + 1];
					if(i  < 2) 
						s_info->macid_h |= ((strtol(mac, NULL, 16) & 0xff) << ((1 - i) * 8));
					else
						s_info->macid_l |= ((strtol(mac, NULL, 16) & 0xff) << ((5 - i) * 8));
				}
			}
			break;
		}
	}
	fclose(fp);
	return 0;
}

int system_get_soc_type(soc_info_t *s_info) {
	return system_get_soc_info("TYPE", s_info);
}

int system_get_soc_mac(soc_info_t *s_info) {
	return system_get_soc_info("MAC", s_info);
}

void system_get_storage_info(char *mount_point, storage_info_t *si)
{
	struct statfs disk_info;
	int blksize;
	FILE *fp;
	char split[] = " ";
	char linebuf[512];
	char *result = NULL;
	int mounted = 0;

	fp = fopen("/proc/mounts", "r");
	if (fp == NULL) {
		printf("open error mount proc %s \n", strerror(errno));
		si->total = 0;
		si->free = 0;
		return ;
	}
	while (fgets(linebuf, 512, fp)) {
		result = strtok(linebuf, split);
		result = strtok(NULL, split);
		if (!strncmp(result, mount_point, strlen(mount_point))) {
			mounted = 1;
			break;
		}
	}
	fclose(fp);
	if (mounted ==  0) {
		si->total = 0;
		si->free = 0;
		return;
	}

	memset(&disk_info, 0, sizeof(struct statfs));
	if(statfs(mount_point, &disk_info) < 0) return ;
	if (disk_info.f_bsize >= (1 << 10)) {
		si->total = ((unsigned int)(disk_info.f_bsize >> 10) * disk_info.f_blocks);
		si->free = ((unsigned int)(disk_info.f_bsize >> 10) * disk_info.f_bfree);
	} else if (disk_info.f_bsize > 0) {
		blksize = ((1 << 10) / disk_info.f_bsize);
		si->total = (disk_info.f_blocks / blksize);
		si->free = (disk_info.f_bfree / blksize);
	}
}

int system_get_disk_status(disk_info_t *info) {
	FILE *fp;
	int node_len;
	char linebuf[512];
	char *result;
	char split[] = " ";

	if(strncmp("/dev/sda", info->name, 8) &&
			strncmp("/dev/mmcblk0", info->name, 12) &&
			strncmp("/dev/mmcblk1", info->name, 12)) {
		printf("unsurported disk %s\n", info->name);
		return -1;
	}

	node_len = strlen(info->name);

	if(access(info->name , 0)) {
		info->inserted = 0;
		info->mounted = 0;
	} else {
		info->mounted = 0;
		info->inserted = 1;

		fp = fopen("/proc/mounts", "r");
		if (fp == NULL) {
			printf("open error mount proc %s \n", strerror(errno));
			return -1;
		}
		while (fgets(linebuf, 512, fp)) {
			result = strtok(linebuf, split);
			if(!strncmp(result, info->name, node_len)) {
				info->mounted = 1;
				//sprintf(blkname, result);
				//result = strtok(NULL, split);
				//sprintf(mountpoint, result);
				break;
			}
		}
		fclose(fp);
	}
	return 0;
}

int system_format_storage(char *mount_point)
{
	FILE *fp;
	char split[] = " ";
	char linebuf[512];
	char cmd[128];
	char dev_path[128];
	char *result = NULL;
	int ret;

	fp = fopen("/proc/mounts", "r");
	if (fp == NULL) {
		printf("open error mount proc %s \n", strerror(errno));
		return -1;
	}

	memset(dev_path, '\0', 128);
	while (fgets(linebuf, 512, fp)) {
		result = strtok(linebuf, split);
		while (result != NULL) {
			if (!strncmp(result, mount_point, strlen(mount_point)))
				break;
			sprintf(dev_path, result);
			result = strtok(NULL, split);
		}
	}
	fclose(fp);
	if (dev_path[0] == '\0')
		return -2;

	ret = umount(mount_point);
	if(ret < 0) {
		printf("umount %s failed,error:%s\n", mount_point,strerror(errno));
		return ret;
	}
	
	sprintf(cmd, "mkfs.vfat %s", dev_path);
	system(cmd);

	ret = mount(dev_path, mount_point, "vfat", 0, NULL);
	if(ret < 0) {
		printf("mount %s to %s failed,error:%s\n", dev_path, mount_point,strerror(errno));
		return ret;
	}

	return 0;
}

int system_get_env(char *name, char *value, int count)
{
	int fd, ret;
	char data[512];

	if ((fd = open(ENV_PATH, (O_RDWR | O_NDELAY))) < 0) {
		printf("env %s open failed %s \n", ENV_PATH, strerror(errno));
		return fd;
	}

	sprintf(data, name);
	ret = ioctl(fd, ENV_GET_SAVE, (unsigned)data);
	if (ret) {
		close(fd);
		return ret;
	}

	snprintf(value, count, data);
	close(fd);
	return 0;
}

int system_set_env(char *name, char *value)
{
	int fd, ret;
	struct env_item_t env_item;

	if ((fd = open(ENV_PATH, (O_RDWR | O_NDELAY))) < 0) {
		printf("env %s open failed %s \n", ENV_PATH, strerror(errno));
		return fd;
	}

	snprintf(env_item.name, sizeof(env_item.name), name);
	snprintf(env_item.value, sizeof(env_item.value), value);
	ret = ioctl(fd, ENV_SET_SAVE, (unsigned)(&env_item));
	if (ret) {
		close(fd);
		return ret;
	}

	close(fd);
	return 0;
}

int system_clear_env(void)
{
	int fd, ret;
	unsigned data = 0;

	if ((fd = open(ENV_PATH, (O_RDWR | O_NDELAY))) < 0) {
		printf("env %s open failed %s \n", ENV_PATH, strerror(errno));
		return fd;
	}

	ret = ioctl(fd, ENV_CLN_SAVE, data);
	if (ret) {
		close(fd);
		return ret;
	}

	close(fd);
	return 0;
}

int system_enable_watchdog(void)
{
	int fd = -1, args = WDIOS_ENABLECARD;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_SETOPTIONS, &args);
	close(fd);

	return 0;
}

int system_disable_watchdog(void)
{
	int fd = -1, args = WDIOS_DISABLECARD;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_SETOPTIONS, &args);
	close(fd);

	return 0;
}

int system_feed_watchdog(void)
{
	int fd = -1, args = 0;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_KEEPALIVE, &args);
	close(fd);

	return 0;
}

int system_set_watchdog(int interval)
{
	int fd = -1, args = interval;;

	if ((fd = open(WDT_DEV_NAME, O_RDONLY)) < 0) {
		printf("watchdog open failed \n");
		return fd;
	}

	ioctl(fd, WDIOC_SETTIMEOUT, &args);
	close(fd);

	return 0;
}

char *system_get_ledlist(void)
{
	DIR *dir;
	struct dirent *ptr;

    if ((dir = opendir(LED_SYS_PATH)) == NULL) {
        printf("no led node \n");
        return NULL;
    }

	memset(leds_name, '\0', 512);
    while ((ptr = readdir(dir)) != NULL) {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
            continue;

		strcat(leds_name, ptr->d_name);
		strcat(leds_name, "/");
    }
    closedir(dir);

	return leds_name;
}

int system_get_led(char *led_name)
{
	int ret = -1;
	DIR *dir;
	struct dirent *ptr;

    if ((dir = opendir(LED_SYS_PATH)) == NULL) {
        printf("no led node \n");
        return -1;
    }

    while ((ptr = readdir(dir)) != NULL) {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
            continue;

		if (strcmp(ptr->d_name, led_name)==0 ) {
			ret = 0;
			break;
		}
    }
    closedir(dir);

	return ret;
}

int system_set_led(const char *led, int time_on, int time_off, int blink_on)
{
	int fd;
	char cmd[8];
	char path[128];

	sprintf(path, "%s/%s/trigger", LED_SYS_PATH, led);
	if ((fd = open(path, O_RDWR)) < 0) {
		printf("led %s open failed %s \n", path, strerror(errno));
		return fd;
	}
	if (!blink_on)
		sprintf(cmd, "%s", "none");
	else
		sprintf(cmd, "%s", "timer");

	if (write(fd, cmd, strlen(cmd)) != strlen(cmd)) {
		printf("Failed to write led (%s) \n", strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);

	if (!blink_on) {
		sprintf(path, "%s/%s/brightness", LED_SYS_PATH, led);
		if ((fd = open(path, O_RDWR)) < 0) {
			printf("led %s open failed %s \n", path, strerror(errno));
			return fd;
		}
		if (time_on)
			sprintf(cmd, "%d", time_on);
		else
			sprintf(cmd, "%d", time_off);
	    if (write(fd, cmd, sizeof(int)) != sizeof(int)) {
	        printf("Failed to write led (%s) \n", strerror(errno));
	        close(fd);
	        return -1;
	    }
	    close(fd);
		return 0;
	}

	if ((time_on <= 0) || (time_off <= 0)) {
		time_on = 500;
		time_off = 500;
	}

	sprintf(path, "%s/%s/delay_on", LED_SYS_PATH, led);
	if ((fd = open(path, O_RDWR)) < 0) {
		printf("led %s open failed %s \n", path, strerror(errno));
		return fd;
	}

	sprintf(cmd, "%d", time_on);
    if (write(fd, cmd, sizeof(int)) != sizeof(int)) {
        printf("Failed to write led (%s) \n", strerror(errno));
        close(fd);
        return -1;
    }
    close(fd);

	sprintf(path, "%s/%s/delay_off", LED_SYS_PATH, led);
	if ((fd = open(path, O_RDWR)) < 0) {
		printf("led %s open failed %s \n", path, strerror(errno));
		return fd;
	}

	sprintf(cmd, "%d", time_off);
    if (write(fd, cmd, sizeof(int)) != sizeof(int)) {
        printf("Failed to write led (%s) \n", strerror(errno));
        close(fd);
        return -1;
    }
	close(fd);

	return 0;
}

int system_get_boot_reason() {
	char rdbuf[8];
	int reason = 0 ;
	int ret;
	int fd = open("/sys/class/pmu-debug/on", O_RDONLY);
	if(fd < 0) 
		return -1;
	ret = read(fd, rdbuf, 12);
	if (ret < 0) {
		printf("Cannot read to boot reason: %s \n", strerror(errno));
		close(fd);
		return -2;
	}
	close(fd);

	if(!strncmp("ACIN", rdbuf, 4)) {
		reason = BOOT_REASON_ACIN;
	} else if (!strncmp("EXTINT", rdbuf, 6)) {
		reason = BOOT_REASON_EXTINT;
	} else if(!strncmp("POWERKEY", rdbuf, 8)) {
		reason = BOOT_REASON_POWERKEY;
	} else {
		reason = BOOT_REASON_UNKNOWN;
	}
	return reason;
}

int system_get_backlight()
{
	FILE *fp = fopen("/sys/class/backlight/lcd-backlight/brightness", "r");
	if(!fp) return -1;

	int b = -1;
	fscanf(fp, "%d", &b);
	fclose(fp);
	return b;
}

int system_set_backlight(int brightness)
{
	FILE *fp = fopen("/sys/class/backlight/lcd-backlight/brightness", "w");
	if(!fp) return -1;

	fprintf(fp, "%d", brightness);
	fscanf(fp, "%d", &brightness);
	fclose(fp);
	return brightness;
}

int system_enable_backlight(int enable) {
	int fd;
	fd = open("/dev/fb0", O_RDWR);
	if(fd < 0) 
		return -1;

	if(!enable)
		ioctl(fd, FBIOBLANK, FB_BLANK_POWERDOWN);
	else
		ioctl(fd, FBIOBLANK, FB_BLANK_UNBLANK);

	close(fd);
	return 0;
}

int system_request_pin(int index)
{
	int fd;
	char fbuf[64];

	sprintf(fbuf, GPIO_DIRETION_PATH, index);
	if (access(fbuf, F_OK) == 0) {
		printf("gpio  %d already request\n", index);
		return -1;
	}

	fd = open(GPIO_EREQUEST_PATH, O_WRONLY);
    if (fd < 0) {
        printf("Cannot open %s: %s \n", GPIO_EREQUEST_PATH, strerror(errno));
        return -1;
    }

	sprintf(fbuf, "%d", index);
	if (write(fd, fbuf, strlen(fbuf)) < 0) {
		printf("Cannot write to %s: %s \n", GPIO_EREQUEST_PATH, strerror(errno));
		close(fd);
		return -2;
	}

	close(fd);
	return 0;
}

int system_get_pin_value(int index)
{
	int fd, ret;
	char fbuf[64], value[16];

	sprintf(fbuf, GPIO_DIRETION_PATH, index);
	fd = open(fbuf, O_RDWR);
	if (fd < 0) {
		printf("Cannot open %s: %s \n", fbuf, strerror(errno));
		return -1;
	}

	if (write(fd, "in", strlen("in")) < 0) {
	    printf("Cannot write to %s: %s \n", fbuf, strerror(errno));
	  close(fd);
	    return -2;
	}
	close(fd);

	sprintf(fbuf, GPIO_VALUE_PATH, index);
	fd = open(fbuf, O_RDWR);
	if (fd < 0) {
		printf("Cannot open %s: %s \n", fbuf, strerror(errno));
		return -1;
	}

	ret = lseek(fd, 0, SEEK_SET);
	if (ret < 0) {
	    printf("lseek failed with %s.\n", strerror(errno));
	    return -1;
	}

	memset(value, 0, 16*sizeof(char));
	ret = read(fd, value, 1);
	if (ret < 0) {
		printf("Cannot read to %s: %s \n", fbuf, strerror(errno));
		close(fd);
		return -2;
	}

	return atoi(value);
}

int system_set_pin_value(int index, int value)
{
	int fd;
	char fbuf[64];

	sprintf(fbuf, GPIO_DIRETION_PATH, index);
	fd = open(fbuf, O_RDWR);
	if (fd < 0) {
		printf("Cannot open %s: %s \n", fbuf, strerror(errno));
		return -1;
	}

  if (write(fd, "out", strlen("out")) < 0) {
      printf("Cannot write to %s: %s \n", fbuf, strerror(errno));
    close(fd);
      return -2;
  }
  close(fd);

	sprintf(fbuf, GPIO_VALUE_PATH, index);
	fd = open(fbuf, O_RDWR);
	if (fd < 0) {
		printf("Cannot open %s: %s \n", fbuf, strerror(errno));
		return -1;
	}

	sprintf(fbuf, "%d", value);
  if (write(fd, fbuf, 1) < 0) {
      printf("Cannot write to %s: %s \n", fbuf, strerror(errno));
    close(fd);
      return -2;
  }
  close(fd);

	return 0;
}

#define GET_FILE_INT(c, f) do {  \
	fp = fopen("/sys/class/power_supply/" #c "/" #f, "r");	c->f = 0;  \
	if(fp) { fscanf(fp, "%d", &c->f); fclose(fp); }} while (0)
void system_get_ac_info(ac_info_t *ac)
{
	FILE *fp;
	GET_FILE_INT(ac, current_now);
	GET_FILE_INT(ac, online);
	GET_FILE_INT(ac, present);
	GET_FILE_INT(ac, voltage_now);
}

void system_get_battry_info(battery_info_t *battery)
{
	FILE *fp;
	GET_FILE_INT(battery, capacity);
	GET_FILE_INT(battery, online);
	GET_FILE_INT(battery, voltage_now);

	fp = fopen("/sys/class/power_supply/battery/status", "r");
	memset(battery->status, 0, sizeof(battery->status));
	if(fp) { fscanf(fp, "%s", battery->status); fclose(fp); }
}

int system_get_version(char *version)
{
	int fd, ret;

	fd = open(SYSTEM_VERSION_PATH, O_RDONLY);
	if (fd < 0) {
		printf("Cannot open %s: %s \n", SYSTEM_VERSION_PATH, strerror(errno));
		return -1;
	}

    ret = lseek(fd, 0, SEEK_SET);
    if (ret < 0) {
        printf("lseek failed with %s.\n", strerror(errno));
        return -1;
    }

	ret = read(fd, version, 16);
	if (ret < 0) {
		printf("Cannot read to %s: %s \n", SYSTEM_VERSION_PATH, strerror(errno));
		close(fd);
		return -2;
	}

	close(fd);
	return 0;
}

int system_check_process(char *pid_name)
{
	int pid = -1, temp, offset;
	DIR *dir;
	struct dirent *ptr;
	FILE *fp;
	char value[256], path[128];

	dir = opendir("/proc");
	if (dir != NULL) {
		while ((ptr = readdir(dir)) != NULL) {
			sprintf(path, "/proc/");
			strcat(path, ptr->d_name);
			strcat(path, "/cmdline");
			if (access(path, F_OK) != 0)
				continue;

			if ((fp = fopen(path,"r")) == NULL)
        		continue;

			memset(value, 0, 256);
			temp = 0;
			while (!feof(fp)) {
				value[temp++] = fgetc(fp);
				if (temp >= 255)
					break;
			}

			offset = 0;
			while (offset < temp) {
				if (!(value[offset] || value[offset+1]))
					break;
				if ((value[offset] == 0) && value[offset+1])
					value[offset] = 0x20;
				else if (value[offset] && (value[offset+1] == 0) && value[offset+2])
					value[offset+1] = 0x20;
				offset += 2;
			}

			if (!strstr(value, pid_name)) {
				fclose(fp);
				continue;
			}
			pid = atoi(ptr->d_name);
			fclose(fp);
			break;
		}
		closedir(dir);
	}

	return pid;
}

void system_kill_process(char *pid_name)
{
	int pid, temp, offset;
	DIR *dir;
	struct dirent *ptr;
	FILE *fp;
	char value[256], path[128], cmd[32];

	dir = opendir("/proc");
	if (dir != NULL) {
		while ((ptr = readdir(dir)) != NULL) {
			sprintf(path, "/proc/");
			strcat(path, ptr->d_name);
			strcat(path, "/cmdline");
			if (access(path, F_OK) != 0)
				continue;

			if ((fp = fopen(path,"r")) == NULL)
        		continue;

			memset(value, 0, 256);
			temp = 0;
			while (!feof(fp)) {
				value[temp++] = fgetc(fp);
				if (temp >= 255)
					break;
			}

			offset = 0;
			while (offset < temp) {
				if (!(value[offset] || value[offset+1]))
					break;
				if ((value[offset] == 0) && value[offset+1])
					value[offset] = 0x20;
				else if (value[offset] && (value[offset+1] == 0) && value[offset+2])
					value[offset+1] = 0x20;
				offset += 2;
			}

			if (!strstr(value, pid_name)) {
				fclose(fp);
				continue;
			}
			pid = atoi(ptr->d_name);
			sprintf(cmd, "kill %d", pid);
			system(cmd);
			fclose(fp);
			break;
		}
		closedir(dir);
	}

	return;
}

static int system_get_upgrade_dev(char *buf)                     
{                                                                          
	int fd, offset = 0, ret;                                                 

	if (!buf)                                                               
		return -1;                                                             

	if (item_exist("part0"))                                                 
		offset += item_integer("part0", 1);                                    
	if (item_exist("part1"))                                                 
		offset += item_integer("part1", 1);                                    
	offset *= SZ_1K;                                                         
	offset += (EMMC_IMAGE_OFFSET + UPGRADE_PARA_SIZE);                                           
	
	fd = open(EMMC_BLK, (O_RDWR | O_SYNC));                         
	if (fd < 0) {                                                            
		printf("Cannot open %s \n", strerror(errno));   
		return -1;                                                             
	}                                                                        

	ret = lseek(fd, offset, SEEK_SET);                                       
	if (ret < 0) {                                                           
		printf("lseek failed with %s.\n", strerror(errno));                    
		close(fd);                                                             
		return -1;                                                             
	}                                                                        

	if (read(fd, buf, RESERVED_ENV_UBOOT_SIZE) < 0) {                         
		printf("Cannot read: %s \n", strerror(errno));
		close(fd);                                                             
		return -2;                                                             
	}                                                                        
	close(fd);                                                               

	sync();                                                                  

	return 0;                                                                
}                                                                          


static int system_set_upgrade_dev(char *buf)
{
	int fd, offset = 0, ret;

	if (!buf)                           
		return -1;                         

	if (item_exist("part0"))             
		offset += item_integer("part0", 1);
	if (item_exist("part1"))             
		offset += item_integer("part1", 1);
	offset *= SZ_1K;                     
	offset += (EMMC_IMAGE_OFFSET + UPGRADE_PARA_SIZE);

	fd = open(EMMC_BLK, (O_RDWR | O_SYNC));
	if (fd < 0) {
		printf("Cannot open %s: %s \n", EMMC_BLK, strerror(errno));
		return -1;                                                          
	}

	ret = lseek(fd, offset, SEEK_SET);
	if (ret < 0) {                                       
		printf("lseek failed with %s.\n", strerror(errno));
		close(fd);                                         
		return -1;                                         
	}                                                    

	if (write(fd, buf, RESERVED_ENV_UBOOT_SIZE) < 0) {                         
		printf("Cannot write to %s \n", strerror(errno));
		close(fd);                                                              
		return -2;                                                              
	}                                                                         
	close(fd);                                                                
	
	sync();
	return 0;
}

typedef struct {
	char name[32];
	char val[32];
} system_parse_t;

#define ITEMS_MAX		32

int system_parse_data(char *data, system_parse_t *items)                                                                                  
{
	int i = 0, j, k = 0;                                                                                                          
	char *p = data + 4;
	char *token, *d;
	char c, buf[256];
	
	if (!data || !items)
		return -1;

	while (p[i] != '\0') {                                                                                               
		j = 0;                                                                                                           
		do {                                                                                                             
			if (i >= RESERVED_ENV_UBOOT_SIZE) // do not find "str"                                                         
				break;                                                                                                   

			buf[j++] = c = p[i++];                                                                                         
			if (j == sizeof(buf) - 1) {                                                                                    
				printf("%s\n", __func__);                                                                                
				j = 0;                                                                                                       
			}                                                                                                              
		} while (c != '\0');                                                                                             

		if (j > 1) { 
			d = buf;
		//	printf("d %s\n", d);
			token = strsep(&d, "=");
			if (!token)
				continue;

			memcpy(items[k].name, token, strlen(token));
			items[k].name[strlen(token)] = '\0';
			//printf("items[%d].name %s\n", k, items[k].name);
			token = strsep(&d, "=");
			if (!token)
				continue;

			memcpy(items[k].val, token, strlen(token));
			items[k].val[strlen(token)] = '\0';
			//printf("items[%d].val %s\n", k, items[k].val);
			k++;
		}                                                                                                                
	}                                                                                                                  

	return k;                                                                                                         
}                                                                                                                    


int system_check_upgrade(char *name, char *value)
{
	int ret;
	char result[64];
	char buf[RESERVED_ENV_UBOOT_SIZE];
	system_parse_t data[ITEMS_MAX];
	int i;
	
	if (!name || !value)
		return -1;
	
	if (item_exist("board.disk") && item_equal("board.disk", "flash", 0)) {
		ret = system_get_env(name, result, 64);
		if (ret < 0)
			return ret;

		if (strncmp(result, value, strlen(value)))
			return -1;
	} else {
		ret = system_get_upgrade_dev(buf);
		if (ret < 0)
			return -1;

		ret = system_parse_data(buf, data);
		if (ret < 0)
			return -1;
	
		for (i = 0; i < ret; i++) {
			if (!strncmp(name, data[i].name, strlen(name))) {
				if (!strncmp(value, data[i].val, strlen(value)))
					return 0;
			}
		}
		return -1;
	}

	return 0;
}

int system_set_upgrade(char *name, char *value)
{
	int ret;
	char buf[RESERVED_ENV_UBOOT_SIZE];
	system_parse_t data[ITEMS_MAX];
	int i;
	int offset = 4;


	if (!name || !value)
		return -1;
	
	if (item_exist("board.disk") && item_equal("board.disk", "flash", 0))
		return system_set_env(name, value);
	else {
		ret = system_get_upgrade_dev(buf);
		if (ret < 0)
			return -1;

		ret = system_parse_data(buf, data);
		if (ret < 0)
			return -1;

		for (i = 0; i < ret; i++) {
			if (!strncmp(name, data[i].name, strlen(name))) {
				memcpy(data[i].val, value, strlen(value));
				data[i].val[strlen(value)] = '\0';
				break;
			}
		}
		if (i == ret) {
			memcpy(data[i].name, name, strlen(name));
			data[i].name[strlen(name)] = '\0';
			memcpy(data[i].val, value, strlen(value));
			data[i].val[strlen(value)] = '\0';
			ret++;
		}

		for (i = 0; i < ret; i++) {
			sprintf(buf + offset, "%s=%s", data[i].name, data[i].val);
			//printf("buf %s\n", buf + offset);
			offset += strlen(data[i].name) + strlen(data[i].val) + 1;	
			buf[offset] = '\0';
			offset += 1;
		}
		buf[offset + 1] = '\0';
		system_set_upgrade_dev(buf);

		return 0;
	}
}

int system_clearall_upgrade(void)
{
	char buf[RESERVED_ENV_UBOOT_SIZE];

	if (item_exist("board.disk") && item_equal("board.disk", "flash", 0))
		return system_clear_env();
	else {
		memset(buf, 0, RESERVED_ENV_UBOOT_SIZE);
		return system_set_upgrade_dev(buf);
	}
}

/* 
 * now. it's used to enable/disable extern power up caused by g-sensor 
 * dev_name    externel device  driver name  
 * enbale      1: enable externel externel power up
 *	       0: disable externel externel power up
 */
int system_enable_externel_powerup(enum externel_device_type dev_type, char *dev_name, int enable) {
	int ret, fd;
	char path[128];
	char cmd[1];
	if(!dev_name)
		return -1;

	switch (dev_type) {
		case  EXT_DEVICE_GSENSOR:
			sprintf(path, GSENSOR_ATTR_ENABLE_PATH, dev_name);
			break;
		default:
			printf("dev_type %d not support now\n", dev_type);
			return -1;
	}
	printf("[system] enable_externel_powerup path- %s\n", path);

	fd = open(path, O_RDWR);
	if(fd < 0) 
		return -1;
	sprintf(cmd, "%d", !!enable);
	ret = write(fd, cmd, 1);
	if(ret < 0)
		return -1;
	close(fd);
	return 0;
}

