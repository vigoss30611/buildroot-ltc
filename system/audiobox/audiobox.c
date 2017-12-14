#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <log.h>
#include <audiobox_listen.h>
#include <fr/libfr.h>
#include <audiobox.h>

static int audiobox_stop = 0;

static void audiobox_exit(void)
{
	audiobox_listener_deinit();
}

void audiobox_signal_handler(int sig)
{
	AUD_DBG("AudioBox server caught signal: %d\n", sig);
	audiobox_stop = 1;
}

static void audiobox_signal_init(void)            
{                                            
	signal(SIGTERM, audiobox_signal_handler);
	signal(SIGQUIT, audiobox_signal_handler);
	signal(SIGINT, audiobox_signal_handler); 
	signal(SIGTSTP, audiobox_signal_handler);
}                                            

int main(int argc, char **argv)
{
	struct timeval timetv;
	int err;
	
	gettimeofday(&timetv, 0);
	AUD_DBG("AudioBox Server start. time: %s\n", asctime(localtime(&timetv.tv_sec)));
	
	/* set signal init */
	audiobox_signal_init();
	/* set audiobox listener */
	err = audiobox_listener_init();
	if (err < 0) {
		audiobox_exit();
		return -1;
	}

	while (1) {                
		sleep(1); 
		if (audiobox_stop) {
			AUD_DBG("audiobox_exit\n");
			audio_stop_service();
			audiobox_exit();
			break;
		}
	}
	return 0;
}
