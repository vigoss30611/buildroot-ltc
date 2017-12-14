#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <qsdk/sys.h>
#include "sysset.h"

#define EXTERNAL_STORAGE_PATH	"/mnt/"
#define SD_DEV_NODE "/dev/mmcblk0"

int storage_test(int argc, char** argv)
{
	int ret = 0;
	char mount_point[128] = {0};
	storage_info_t sinfo;

	realpath(EXTERNAL_STORAGE_PATH, mount_point);

	if (0 == strcmp(argv[1], "status")) {
		disk_info_t disk;
		disk.name = SD_DEV_NODE;
		sys_info("disk name %s\n", disk.name);
		ret = system_get_disk_status(&disk);
		if (!ret) {
			if (disk.mounted)
				sys_info("SD status: mounted\n");
			else if (disk.inserted)
				sys_info("SD status: inserted but umounted\n");
			else 
				sys_info("SD status: umounted\n");
		} else 
			sys_err("Get storage status failed\n");
	} else if (0 == strcmp(argv[1], "info")) {
		system_get_storage_info(mount_point, &sinfo);
		sys_info("SD total storage: %dM\n", sinfo.total/1024);
		sys_info("SD free storagr: %dM\n", sinfo.free/1024);
	} else if (0 == strcmp(argv[1], "format")) {
		if (access(SD_DEV_NODE, 0)) {
			sys_err("sd node not exist, %s\n", SD_DEV_NODE);
			return -1;
		}

		ret = system_format_storage(mount_point);
		if (!ret)
			sys_info("SD format success\n");
		else
			sys_info("SD format failed\n");

		system_get_storage_info(mount_point, &sinfo);
		sys_info("SD total storage: %dM\n", sinfo.total/1024);
		sys_info("SD free storagr: %dM\n", sinfo.free/1024);
	} else {
		sys_err("Segmentation fault\n");
		return -1;
	}

	return 0;
}
