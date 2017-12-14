#include <unistd.h>
#include <sys/reboot.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <qsdk/upgrade.h>

#define PORT 8899
#define MMC_IUS_PATH "/mnt/mmc/update.ius"

static int client_sock = -1;
static pthread_t tid_socket;

void thd_socket(void *arg)
{
    int socket_desc, c, reuse = 1;
    struct sockaddr_in server, client;

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

    pthread_detach(pthread_self());
}

static void upgrade_state_callback_q360(void *arg, int image_num, int state, int state_arg)
{
    struct upgrade_t *upgrade = (struct upgrade_t *)arg;
    int cost_time;
    char client_message[128] = {0};

    switch (state) {
	case UPGRADE_START:
	    system_set_led("led_work", 1000, 800, 1);
	    gettimeofday(&upgrade->start_time, NULL);
	    sprintf(client_message, "upgrade start \n");
	    break;
	case UPGRADE_COMPLETE:
	    gettimeofday(&upgrade->end_time, NULL);
	    cost_time = (upgrade->end_time.tv_sec - upgrade->start_time.tv_sec);
	    system_set_led("led_work", 1, 0, 0);
	    if (system_update_check_flag() == UPGRADE_OTA)
				system_update_clear_flag(UPGRADE_FLAGS_OTA);
	    sprintf(client_message, "upgrade sucess time is %ds", cost_time);

	    reboot(LINUX_REBOOT_CMD_RESTART);

	    break;
	case UPGRADE_WRITE_STEP:
	    sprintf(client_message, "%s image -- %d %% complete", image_name[image_num], state_arg);
	    fflush(stdout);
	    break;
	case UPGRADE_VERIFY_STEP:
	    if (state_arg)
		sprintf(client_message, "%s verify failed %x", image_name[image_num], state_arg);
	    else
		sprintf(client_message, "%s verify success", image_name[image_num]);
	    break;
	case UPGRADE_FAILED:
	    system_set_led("led_work", 300, 200, 1);
	    sprintf(client_message, "upgrade %s failed %d", image_name[image_num], state_arg);
	    break;
    }

    printf("client_message is %s \n", client_message);
    if (client_sock >= 0)
	send(client_sock, client_message, strlen(client_message)+1, 0);

    return;
}

int main(int argc, char** argv)
{
    char *path = NULL;
    int err;

    err = pthread_create(&tid_socket, NULL, thd_socket, NULL);
    if (err != 0) {
	puts("can't create tid_socket");
	return -1;
    }

    if (argc > 1) {
	path = argv[1];
    } else {
	printf("nothing upgrade image \n");
	return SUCCESS;
    }

    system_update_upgrade(path, upgrade_state_callback_q360, NULL);

    while (1) {
	sleep(1);
    }

    return SUCCESS;
}

