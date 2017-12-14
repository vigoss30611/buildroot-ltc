/******************************************************************************/
/* Standard C Includes.                                                       */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

/******************************************************************************/
/* Include header files for TCPServer and StreamerThread classes.             */
/******************************************************************************/
#include "deamon.h"
#include "main.h"
#include <qsdk/rtsp_server.h>

/******************************************************************************/
/* Default value for the filename containing the configuration information.   */
/******************************************************************************/
#define CONFIG_FILE             "deamon.config"


int main(int iArgC, char *ppcArgV[])
{
    //openlog("my_deamon", LOG_PID, LOG_DAEMON);
	int iListenPort = 0;

    printf("===================================\n");
    printf("        Infotm ITTD Server         \n");
    printf("===================================\n");
    printf(" Version      : 1.0.0              \n");
    printf(" Developed By : Jazz Chang         \n");
    printf(" Copyright (c) 2017                \n");
    printf("===================================\n");

    if (iArgC > 1)
    {
    	iListenPort = atoi(ppcArgV[1]);
    }

    Deamon cDeamon(CONFIG_FILE, iListenPort);

    printf("RTSP server start ...\n");
    rtsp_server_init(0, NULL, NULL);
    rtsp_server_start();
    printf("\n");

    try
    {
        if (iArgC > 2)
        {
            printf("Error: Illegal number of arguments !\n");
            printf("Usage:\n\tittd [port_number]\n");
            printf("===================================\n");
            throw 1;
        }

        cDeamon.RunDeamon();
    }
    catch (...)
    {
    	printf("ERROR: Abnormal Infotm ITTD Server Termination\n");
    }

	printf("RTSP server stop !!!\n");
	rtsp_server_stop();

	//closelog();
    printf("###################################\n");
    printf("        Infotm ITTD Server         \n");
    printf("             Bye Bye               \n");
    printf("###################################\n");
    return 0;
}

