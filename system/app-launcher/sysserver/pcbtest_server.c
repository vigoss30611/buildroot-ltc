#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include<fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <qsdk/sys.h>
#include <qsdk/event.h>
#include <qsdk/cep.h>
#include <qsdk/videobox.h>
#include <qsdk/vplay.h>
#include <qsdk/codecs.h>
#include <qsdk/event.h>


#define PORT 8888
#if defined(PRODUCT_LESHI_C23)
#define KEY_OK 	(0x01<<0)
//#define KEY_RESET 		(0x01<<1)
#define KEY_WIFI 		(0x01<<2)
#define KEY_ALL		(KEY_OK|KEY_WIFI) //KEY_RESET
#else
#define KEY_DOWN 	(0x01<<0)
#define KEY_UP 		(0x01<<1)
#define KEY_OK 		(0x01<<2)
#define KEY_MODE 	(0x01<<3)
#define KEY_ALL		(KEY_DOWN|KEY_UP|KEY_OK|KEY_MODE)
#endif

static const char *usb_host_file = "/sys/devices/platform/imap-ehci/usb2/2-1/2-1:1.0/net/usb1";
static int client_sock;
static pthread_mutex_t p_lock;
static pthread_t tid_sd, tid_gsensor, tid_key, tid_usb_host, tid_video, tid_battery;
static int key_mask = 0;

void led_test(void)
{
    printf("enter %s \n", __func__);
    system_set_led("led_work", 1000, 800, 1);
}

void battery_test(void *arg)
{
    int ret = 0;
    char client_message[128] = {0};
    battery_info_t info= {0};
    system_get_battry_info(&info);
    if(info.capacity > 0) {
	    sprintf(client_message, "battery_test success, capacity: %d", info.capacity);
    } else {
	    sprintf(client_message, "battery_test fail");
    }
    pthread_mutex_lock(&p_lock);
    printf("client_message is %s \n", client_message);
    send(client_sock, client_message, strlen(client_message)+1, 0);
    if (recv(client_sock, client_message, 1024, 0) > 0)
        pthread_mutex_unlock(&p_lock);
}

void *sd_test(void *arg)
{
    char client_message[128];
    int ret = 0;
    disk_info_t info;
    FILE *fp;

    printf("enter %s \n", __func__);
    info.name = "/dev/mmcblk0";
    while (1) {
	ret = system_get_disk_status(&info);
	if (ret) {
	    printf("get disk %s status failed.\n", info.name);
	} else if (info.inserted && info.mounted) {
	    if ((fp = fopen("/mnt/sd0/sdtest", "w+")) == NULL) {
		sprintf(client_message, "sd_test fail");
	    } else {
		sprintf(client_message, "sd_test success");
		fclose(fp);
		remove("/mnt/sd0/sdtest");
	    }

	    pthread_mutex_lock(&p_lock);
	    printf("client_message is %s \n", client_message);
	    send(client_sock, client_message, strlen(client_message)+1, 0);
	    if (recv(client_sock, client_message, 1024, 0) > 0)
		pthread_mutex_unlock(&p_lock);
	    break;
	}

	usleep(500*1000);
    }

    return((void *)1);
}

void gsensor_handler(char *name, void *args)
{
    char client_message[128];

    printf("enter %s \n", __func__);
    if ((NULL == name) || (NULL == args)) {
	printf("invalid parameter!\n");
	return;
    }

    if (!strcasecmp(name, EVENT_ACCELEROMETER)) {
	struct event_gsensor *g_sensor_cp = args;
	if (g_sensor_cp->collision) {
	    sprintf(client_message, "gsensor_test success");
	    pthread_mutex_lock(&p_lock);
	    printf("client_message is %s \n", client_message);
	    send(client_sock, client_message, strlen(client_message)+1, 0);
	    if (recv(client_sock, client_message, 1024, 0) > 0)
		pthread_mutex_unlock(&p_lock);

	    pthread_cancel(tid_gsensor);
	}
    } 
}

void *gsensor_test(void *arg)
{
    pthread_mutex_lock(&p_lock);
    event_register_handler(EVENT_ACCELEROMETER, 0, gsensor_handler);
    pthread_mutex_unlock(&p_lock);

    printf("enter %s \n", __func__);
    while (1) {
	usleep(100*1000);
    }

    return((void *)1);
}

void key_handler(char *name, void *args)
{
    char client_message[128];

    printf("enter %s \n", __func__);
    if ((NULL == name) || (NULL == args)) {
	printf("invalid parameter!\n");
	return;
    }

    if (!strcasecmp(name, EVENT_KEY)) {
	struct event_key *key = args;
#if defined(PRODUCT_LESHI_C23)
	if (key->key_code == 116)
	    key_mask |= KEY_OK;
	//else if (key->key_code == 158)
	//    key_mask |= KEY_RESET;
	else if (key->key_code == 352)
	    key_mask |= KEY_WIFI;
	
#else
	if (key->key_code == 108)
	    key_mask |= KEY_DOWN;
	else if (key->key_code == 103)
	    key_mask |= KEY_UP;
	else if (key->key_code == 352)
	    key_mask |= KEY_OK;
	else if (key->key_code == 116)
	    key_mask |= KEY_MODE;
#endif

	if (key_mask == KEY_ALL) {
	    sprintf(client_message, "key_test success");
	    pthread_mutex_lock(&p_lock);
	    printf("client_message is %s \n", client_message);
	    send(client_sock, client_message, strlen(client_message)+1, 0);
	    if (recv(client_sock, client_message, 1024, 0) > 0)
		pthread_mutex_unlock(&p_lock);

	    pthread_cancel(tid_key);
	}
    }
}

void *key_test(void *arg)
{
    pthread_mutex_lock(&p_lock);
    event_register_handler(EVENT_KEY, 0, key_handler);
    pthread_mutex_unlock(&p_lock);

    printf("enter %s \n", __func__);
    while (1) {
	usleep(100*1000);
    }

    return((void *)1);
}

void *usb_host_test(int pass)
{
    char client_message[128] = {0};
    printf("enter %s \n", __func__);
    sprintf(client_message, "usb_test %s", pass ? "success" : "fail");
    pthread_mutex_lock(&p_lock);
    printf("client_message is %s \n", client_message);
    send(client_sock, client_message, strlen(client_message)+1, 0);
    if (recv(client_sock, client_message, 1024, 0) > 0)
        pthread_mutex_unlock(&p_lock);


    /*while (1) {
        if (!access(usb_host_file, F_OK)) {
            system_set_led("led_work", 300, 200, 1);
            break;
        }
        usleep(100*1000);
    }*/

    return((void *)1);
}


#define PWM_IOCTL_SET_FREQ 1
#define PWM_IOCTL_STOP 0
int beep(int freq, int duration, int interval, int times)
{
    int m_fd = open("/dev/beep", O_RDONLY);
    if(m_fd < 0) {
        printf("open /dev/beep failed\n");
        return -1;
    }
    int i = 0;

    ioctl(m_fd, PWM_IOCTL_STOP);
    for(i = 0; i < times; i ++) {
        ioctl(m_fd, PWM_IOCTL_SET_FREQ, freq);
        usleep(duration * 1000);
        ioctl(m_fd, PWM_IOCTL_STOP);
        usleep(interval * 1000);
    }

    return 0;
}

#define VCHN "encavc0-stream"  //video local save channel
void *video_test(void *arg)
{
    video_enable_channel(VCHN);
    //set bitrate control
    struct v_bitrate_info LOCAL_MEDIA_BITRATE_INFO = {
        .bitrate = 6*1024*1024,
        .qp_max = 41,
        .qp_min = 10,
        .qp_delta = -3,
        .qp_hdr = 30,
        .gop_length = 60,
        .picture_skip = 0,
        .p_frame = 15,
    };
    video_set_bitrate(VCHN, &LOCAL_MEDIA_BITRATE_INFO);

    VRecorderInfo recorderInfo = {0};
    memset(&recorderInfo ,0,sizeof(VRecorderInfo));
    recorderInfo.enable_gps = 0 ;
    recorderInfo.keep_temp = 1;
    recorderInfo.audio_format.type = AUDIO_CODEC_TYPE_AAC;

	recorderInfo.audio_format.sample_rate = 16000;
	recorderInfo.audio_format.sample_fmt  = AUDIO_SAMPLE_FMT_S16 ;
	recorderInfo.audio_format.channels    = 2;

    recorderInfo.time_segment = 60 * 30;
	recorderInfo.time_backward = 0;
	
	strcpy(recorderInfo.av_format, "mp4");
	strcpy(recorderInfo.suffix, "test");
	strcpy(recorderInfo.time_format , "%Y_%m%d_%H%M%S");
	strcpy(recorderInfo.dir_prefix,"/mnt/http/mmc/");

	sprintf(recorderInfo.audio_channel,"default_mic" );
	sprintf(recorderInfo.video_channel, "%s", VCHN );

    VRecorder *recorder = vplay_new_recorder(&recorderInfo);
    if(recorder == NULL){
        printf("create recorder handler error\n");
        return;
    }
	vplay_start_recorder(recorder);

    int ret = 0;
    int count = 8;
    while(count > 0) {
        ret = beep(1, 300, 200, 2);
        if(!access("/usr/share/launcher/res/key.wav", F_OK)) {
            system("abctrl play -d /usr/share/launcher/res/key.wav -w 32 -s 16000 -n 2 &");
        }
        if(ret)
            usleep(1000000);
        count--;
    }
    vplay_stop_recorder(recorder);
    char client_message[128] = {0};
    if(!access("/mnt/http/mmc/test.mp4", F_OK)) {
	    sprintf(client_message, "video_test finished, video url:http://172.3.0.2/mmc/test.mp4");
    } else {
	    sprintf(client_message, "video_test fail");
    }

    pthread_mutex_lock(&p_lock);
    printf("client_message is %s \n", client_message);
    send(client_sock, client_message, strlen(client_message)+1, 0);
    if (recv(client_sock, client_message, 1024, 0) > 0)
        pthread_mutex_unlock(&p_lock);

    return;

}

int main(int argc, char *argv[])
{
    int socket_desc, c, read_size, err, reuse = 1;
    struct sockaddr_in server, client;
    char client_message[128];
    void *tret;

    if (pthread_mutex_init(&p_lock, NULL) != 0) {
        puts("pthread_mutext_init fail \n");
	return -1;
    }

    /*err = pthread_create(&tid_usb_host, NULL, usb_host_test, NULL);
    if (err != 0) {
	puts("can't create tid_usb_host");
	return -1;
    }*/

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
	printf("Could not create socket");
    }
    puts("Socket created");

    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &reuse,
		sizeof(int)) < 0)
    {
	printf("setsockopt failed \n");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    //Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
	//print the error message
	perror("bind failed. Error");
	return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc, 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
	perror("accept failed");
	return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    while ((read_size = recv(client_sock, client_message, 1024, 0)) > 0)
    {
	//Send the message back to client
	client_message[read_size] = 0;
	puts(client_message);
	if (!strncasecmp(client_message, "start", 5)) {
	    sprintf(client_message, "start success");
	    send(client_sock, client_message, strlen(client_message)+1, 0);

	    led_test();

        usb_host_test(atoi(argv[1]));

	    err = pthread_create(&tid_battery, NULL, battery_test, NULL);
	    if (err != 0) {
		puts("can't create battery_test");
		return -1;
	    }

	    err = pthread_create(&tid_sd, NULL, sd_test, NULL);
	    if (err != 0) {
		puts("can't create tid_sd");
		return -1;
	    }

	    err = pthread_create(&tid_gsensor, NULL, gsensor_test, NULL);
	    if (err != 0) {
		puts("can't create tid_gsensor");
		return -1;
	    }

	    err = pthread_create(&tid_key, NULL, key_test, NULL);
	    if (err != 0) {
		puts("can't create tid_key");
		return -1;
	    }

	    err = pthread_create(&tid_video, NULL, video_test, NULL);
	    if (err != 0) {
		puts("can't create tid_video");
		return -1;
	    }

	    err = pthread_join(tid_battery, &tret);
	    if (err != 0) {
		puts("can't join with tid_battery");
		return -1;
	    }

	    err = pthread_join(tid_sd, &tret);
	    if (err != 0) {
		puts("can't join with tid_sd");
		return -1;
	    }

	    err = pthread_join(tid_gsensor, &tret);
	    if (err != 0) {
		puts("can't join with tid_gsensor");
		return -1;
	    }

	    err = pthread_join(tid_key, &tret);
	    if (err != 0) {
		puts("can't join with tid_key");
		return -1;
	    }

	    err = pthread_join(tid_video, &tret);
	    if (err != 0) {
		puts("can't join with tid_video");
		return -1;
	    }


	    break;
	} else {
	    strcat(client_message, " is an invalid request.");
	    send(client_sock, client_message, strlen(client_message)+1, 0);
	}
    }

    if(read_size == 0)
    {
	puts("Client disconnected");
	fflush(stdout);
    }
    else if(read_size == -1)
    {
	perror("recv failed");
    }

    err = pthread_join(tid_usb_host, &tret);
    if (err != 0) {
	puts("can't join with tid_usb_host");
	return -1;
    }

    pthread_mutex_destroy(&p_lock);
    return 0;
}
