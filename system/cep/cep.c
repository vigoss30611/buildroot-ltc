/*****************************************
 *
 *cep means: common-event-provider
 *
 ****************************************/

#include <qsdk/event.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cep.h>
#include <qsdk/sys.h>

static int event_boot = 0;
struct event_key key_cp;
struct event_lightsence lightsence_cp;
struct event_gsensor  g_sensor_cp;
struct event_battery_st battery_st_cp;
struct event_battery_cap battery_cap_cp;
struct event_charge charge_cp;
struct event_linein linein_cp;
struct event_hdmi_hpd hdmi_hpd_cp;
char *array_cp[20];
#define NETLINK_USER 31

int get_filename_array(const char *path)
{
    DIR *pDir;
    int count = 0;
    struct dirent* ent = NULL;
    char *dir = NULL;
    struct stat statbuf;

    if ((pDir = opendir(path)) == NULL) {
	printf("Cannot open directory:%s\n", path);
	return -1;
    }

    while ((ent = readdir(pDir)) != NULL) {
	dir = malloc(512);
	snprintf(dir, 512, "%s/%s", path, ent->d_name);
	array_cp[count] = dir;
	lstat(dir, &statbuf);
	if (!S_ISDIR(statbuf.st_mode)){
	    count++;
	    pc_debug("dir is %s, count %d\n", dir, count);
	}
    }

    if (dir != NULL)
	free(dir);

    pc_debug("All %d files\n", count);
    return count;
}

static void cep_boot_handle(char *event, void *arg)
{
    if (!strncmp(event, EVENT_BOOT_COMPLETE, strlen(EVENT_BOOT_COMPLETE))) {
	if (lightsence_cp.trigger_current <= LS_L2_BRIGHT) {
	    // printf("LIGNT SENCE: dark before %d, current %d\n",
		//    lightsence_cp.trigger_before, lightsence_cp.trigger_current);
	    event_send(EVENT_LIGHTSENCE_D, (char *)&lightsence_cp, sizeof(struct event_lightsence));
	} else if( lightsence_cp.trigger_current > LS_L2_BRIGHT) {
	    // printf("LIGNT SENCE: bright before %d, current %d\n",
		//   lightsence_cp.trigger_before, lightsence_cp.trigger_current);
	    event_send(EVENT_LIGHTSENCE_B, (char *)&lightsence_cp, sizeof(struct event_lightsence));
	}
    }
}

void clean_struct(void)
{
    battery_cap_cp.battery_cap_cur = 0;
    battery_cap_cp.battery_cap_bef = 0;
    charge_cp.charge_st_cur = 0;
    charge_cp.charge_st_bef = 0;
}

void check_event_key(int keys_fd)
{
    struct input_event t;
    struct timeval tv;
    struct timezone tz;

    if (read(keys_fd, &t, sizeof (t)) == sizeof (t) 
	    && t.type == EV_KEY && (t.value == 0 || t.value == 1)) {
	if(t.code == KEY_SPORT) {
	    g_sensor_cp.collision = 1;
	    /*it's defined a G-sensor detected crush event*/
	    event_send(EVENT_ACCELEROMETER, (char *)&g_sensor_cp, sizeof(struct event_gsensor));
	} else {
	    /*it's a key press event*/
	    printf ("key %d %s\n", t.code,(t.value) ? "down" : "up");
	    key_cp.key_code = t.code;
	    key_cp.down = t.value;
	    gettimeofday(&tv, &tz);
	    key_cp.cur_time_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	    //printf ("cur_time_ms is %ld ms\n", key_cp.cur_time_ms);
	    event_send(EVENT_KEY, (char *)&key_cp, sizeof(struct event_key));
	}
    }
}

void check_light_sence(int keys_fd, char *tmp_buf)
{
    lightsence_cp.trigger_before = lightsence_cp.trigger_current;
    lightsence_cp.trigger_current = atoi(tmp_buf);
    if(lightsence_cp.trigger_current <= LS_L1_DARK 
	    && lightsence_cp.trigger_before > LS_L1_DARK){
	// printf("LIGNT SENCE: dark before %d, current %d\n",
		//lightsence_cp.trigger_before, lightsence_cp.trigger_current);
	event_send(EVENT_LIGHTSENCE_D, (char *)&lightsence_cp, sizeof(struct event_lightsence));
    }
    if(lightsence_cp.trigger_current >= LS_L2_BRIGHT 
	    && lightsence_cp.trigger_before < LS_L2_BRIGHT){
	// printf("LIGNT SENCE: bright before %d, current %d\n",
		//lightsence_cp.trigger_before, lightsence_cp.trigger_current);
	event_send(EVENT_LIGHTSENCE_B, (char *)&lightsence_cp, sizeof(struct event_lightsence));
    }
    if (!event_boot) {
	event_register_handler(EVENT_BOOT_COMPLETE, 0, cep_boot_handle);
	event_boot = 1;
    }
}

void check_battery_status(int keys_fd, char *tmp_buf, char *pri_buf) 
{
    if(strncmp(pri_buf, tmp_buf, 4) != 0){
	strcpy(battery_st_cp.battery_st, tmp_buf);
	event_send(EVENT_BATTERY_ST, (char *)&battery_st_cp, sizeof(struct event_battery_st));
	strcpy(pri_buf, tmp_buf);
    }
}

void check_battery_capacity(int keys_fd, char *tmp_buf) 
{
    battery_cap_cp.battery_cap_cur = atoi(tmp_buf);
    if( battery_cap_cp.battery_cap_cur != battery_cap_cp.battery_cap_bef){
	event_send(EVENT_BATTERY_CAP, (char *)&battery_cap_cp, sizeof(struct event_battery_cap));
    }
    battery_cap_cp.battery_cap_bef = battery_cap_cp.battery_cap_cur;
}

void check_charge_status(int keys_fd, char *tmp_buf) 
{
    charge_cp.charge_st_cur = atoi(tmp_buf);
    if( charge_cp.charge_st_cur != charge_cp.charge_st_bef){
	event_send(EVENT_CHARGING, (char *)&charge_cp, sizeof(struct event_charge));
    }
    charge_cp.charge_st_bef = charge_cp.charge_st_cur;
}

void check_linein_status(int keys_fd, char *tmp_buf)
{
	linein_cp.linein_st_cur = atoi(tmp_buf);
	if(linein_cp.linein_st_cur != linein_cp.linein_st_bef){
		event_send(EVENT_LINEIN, (char *)&linein_cp, sizeof(struct event_linein));
	}
	linein_cp.linein_st_bef = linein_cp.linein_st_cur;
}

void check_hdmi_hpd(int keys_fd, char *tmp_buf)
{
	hdmi_hpd_cp.hdmi_hpd_cur = atoi(tmp_buf);
	if(hdmi_hpd_cp.hdmi_hpd_cur != hdmi_hpd_cp.hdmi_hpd_bef){
		event_send(EVENT_LINEIN, (char *)&hdmi_hpd_cp, sizeof(struct event_hdmi_hpd));
		//printf("cep: hdmi hpd event: hpd_cur=%d, hpd_bef=%d\n", 
		//		hdmi_hpd_cp.hdmi_hpd_cur, hdmi_hpd_cp.hdmi_hpd_bef);
	}
	hdmi_hpd_cp.hdmi_hpd_bef = hdmi_hpd_cp.hdmi_hpd_cur;
}

void cp_thread_fun(void){
    int file_num = 0, i = 0, retval = 0, keys_fd[20];
    fd_set readfds;
    struct timeval tv, tv1;
    char tmp_buf[16]={0}, pri_buf[16]={0};

    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;
    int msglen, fd_socket = 0;
    struct event_process *info;

	while (system_check_process("eventhub") < 0)
		usleep(30000);
    file_num = get_filename_array(EVENT_CP_NAME);
    for(i=0; i< file_num; i++) {
	keys_fd[i] = open(array_cp[i], O_RDONLY);
	pc_debug("file %s, fd %d\n", array_cp[i], keys_fd[i]);
    }

    keys_fd[file_num] = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    /*light sence fd*/
    keys_fd[file_num+1] = open(EVENT_LSENCE_NAME, O_RDONLY);
    /*battery status fd*/
    keys_fd[file_num+2] = open(EVENT_BAT_ST_NAME, O_RDONLY);
    /*battery capacity fd*/
    keys_fd[file_num+3] = open(EVENT_BAT_CAP_NAME, O_RDONLY);
    /*battery capacity fd*/
    keys_fd[file_num+4] = open(EVENT_CAHRGE_NAME, O_RDONLY);
	/*line in state*/
	keys_fd[file_num+5] = open(EVENT_LINEIN_NAME, O_RDONLY);
	/*hdmi hot plug detect */
	keys_fd[file_num+6] = open(EVENT_HDMI_HPD_NAME, O_RDONLY);
    clean_struct();

    if (keys_fd[file_num] < 0) {
	pc_debug("socket file  err, fd %d\n", keys_fd[file_num]);
	return ;
    } else {
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = PF_NETLINK;
	src_addr.nl_pid = getpid(); /* self pid */
	src_addr.nl_groups = 1 << 4; /*  interested in group 1<<0 */

	bind(keys_fd[file_num], (struct sockaddr *)&src_addr, sizeof(src_addr));

	msglen = sizeof(struct event_process);
	memset(&dest_addr, 0, sizeof (dest_addr));
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(msglen));
	memset(nlh, 0, NLMSG_SPACE(msglen));

	iov.iov_base = (void *)nlh;
	iov.iov_len = NLMSG_SPACE(msglen);
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1 ;
    }

    while(1) {
	usleep(30000);
	FD_ZERO(&readfds);

	for(i=0; i<= (file_num+6); i++) {
	    if (keys_fd[i] > 0) {
		fd_socket = keys_fd[i];
		FD_SET(keys_fd[i], &readfds);
	    }
	}

	tv.tv_sec = 30;
	tv.tv_usec = 0;
	retval = select(fd_socket+1, &readfds, NULL, NULL, &tv);
	if (retval < 0) {
	    pc_debug("select() failed\n");
	    break;
	}
	if (retval == 0) {
	    pc_debug("select() timeout, couninue\n");
	    continue;
	}

	for(i=0; i<=(file_num+6); i++) {
	    if (!(keys_fd[i]>0 && FD_ISSET(keys_fd[i], &readfds)))
		    continue;

	    if(i<file_num){
		check_event_key(keys_fd[i]);
	    } else if(i==file_num) {
		/* Read message from kernel */
		recvmsg(keys_fd[file_num], &msg, 0);
		info = (struct event_process *)(NLMSG_DATA(nlh));
		event_send(EVENT_PROCESS_END, (char *)info, sizeof(struct event_process));
	    }else if(i==(file_num+1)){
		lseek(keys_fd[i], 0, SEEK_SET);
		read(keys_fd[i], tmp_buf, 16);
		check_light_sence(keys_fd[i], tmp_buf);
	    } else if (i == (file_num+2)){
		lseek(keys_fd[i], 0, SEEK_SET);
		read(keys_fd[i], tmp_buf, 16);
		check_battery_status(keys_fd[i], tmp_buf, pri_buf);
	    } else if (i == (file_num+3)){
		lseek(keys_fd[i], 0, SEEK_SET);
		read(keys_fd[i], tmp_buf, 16);
		check_battery_capacity(keys_fd[i], tmp_buf);
	    } else if (i == (file_num+4)){
		lseek(keys_fd[i], 0, SEEK_SET);
		read(keys_fd[i], tmp_buf, 16);
		check_charge_status(keys_fd[i], tmp_buf);
	    } else if (i == (file_num+5)){
		lseek(keys_fd[i], 0, SEEK_SET);
		read(keys_fd[i], tmp_buf, 16);
		check_linein_status(keys_fd[i], tmp_buf);
		} else if (i == (file_num+6)){
		lseek(keys_fd[i], 0, SEEK_SET);
		read(keys_fd[i], tmp_buf, 16);
		check_hdmi_hpd(keys_fd[i], tmp_buf);
		}
	}
    }
}

void main(int argc, char *argv[])
{
    pc_debug("======new %s[%d],time %s\n", __func__, __LINE__, __TIME__);
    cp_thread_fun();
}
