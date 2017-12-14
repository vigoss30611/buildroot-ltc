#include <unistd.h>
#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <qsdk/sys.h>

#include "upgrade.h"

#define MMC_IUS_PATH "/mnt/mmc/update.ius"

static void upgrade_state_callback_qiwo(void *arg, int image_num, int state, int state_arg)
{
	struct upgrade_t *upgrade = (struct upgrade_t *)arg;
	int cost_time;

	switch (state) {
		case UPGRADE_START:
			system_set_led("red", 1000, 800, 1);
			gettimeofday(&upgrade->start_time, NULL);
			printf("QIWO upgrade start \n");
			break;
		case UPGRADE_COMPLETE:
			gettimeofday(&upgrade->end_time, NULL);
			cost_time = (upgrade->end_time.tv_sec - upgrade->start_time.tv_sec);
			system_set_led("red", 1, 0, 0);
			if (system_update_check_flag() == UPGRADE_OTA)
				system_update_clear_flag(UPGRADE_FLAGS_OTA);
	        printf("\n QIWO upgrade sucess time is %ds \n", cost_time);
			break;
		case UPGRADE_WRITE_STEP:
			printf("\r QIWO %s image -- %d %% complete", image_name[image_num], state_arg);
			fflush(stdout);
			break;
		case UPGRADE_VERIFY_STEP:
			if (state_arg)
				printf("\n QIWO %s verify failed %x \n", image_name[image_num], state_arg);
			else
				printf("\n QIWO %s verify success\n", image_name[image_num]);
			break;
		case UPGRADE_FAILED:
			system_set_led("red", 300, 200, 1);
			printf("QIWO upgrade %s failed %d \n", image_name[image_num], state_arg);
			break;
	}

	return;
}

int main(int argc, char** argv)
{
	int flag;
	char *path = NULL;
	char cmd[256];
	
	if (argc > 1) {
		path = argv[1];
	} else {
		flag = system_update_check_flag();
		if (flag == UPGRADE_IUS) {
			path = NULL;
		} else if (flag == UPGRADE_OTA) {
			if (item_exist("board.disk") && item_equal("board.disk", "emmc", 0)) {
				sprintf(cmd, "mount -t vfat /dev/mmcblk1p1 /mnt/mmc");
			} else {
				sprintf(cmd, "mount -t vfat /dev/mmcblk0p1 /mnt/mmc");
			}
			system(cmd);
			if(access(MMC_IUS_PATH, F_OK) == 0)
				path = MMC_IUS_PATH;
		} else {
			printf("nothing upgrade image \n");
			return SUCCESS;
		}
	}

	if (item_exist("board.string.device") && item_equal("board.string.device", "qiwo304", 0))
		system_update_upgrade(path, upgrade_state_callback_qiwo, NULL);
	else
		system_update_upgrade(path, NULL, NULL);

	if (item_exist("board.string.device") && item_equal("board.string.device", "qiwo304", 0)) {
		if (flag == UPGRADE_OTA)
			printf("\n Remove mmc card to enter the new system.\n");
		else
			printf("\n Remove ius card to enter the new system.\n");

		if ((access("/dev/mmcblk0", F_OK) == 0) && (access("/dev/mmcblk1", F_OK) == 0))
			while(!access("/dev/mmcblk1", F_OK));
		else
			while(!access("/dev/mmcblk0", F_OK));
	}

	reboot(LINUX_REBOOT_CMD_RESTART);

	return SUCCESS;
}

