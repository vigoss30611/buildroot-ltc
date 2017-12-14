#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <fr/libfr.h>
#include <qsdk/audiobox.h>
#include <qsdk/event.h>
#include <sys/time.h>
#include <qsdk/sys.h>

int main(int argc, char **argv)
{
		system_set_upgrade("upgrade_flag", "ota");
		system("reboot");
		return 0;
}

