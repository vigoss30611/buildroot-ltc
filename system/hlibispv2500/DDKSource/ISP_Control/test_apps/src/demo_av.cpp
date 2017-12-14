#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "isp_av_binder.h"

int main(int argc, char * argv[])
{
	int msg_snd_id = 0;

	msg_snd_id = msgget(ISP_AVS_BINDER_KEY,0);
	if(msg_snd_id < 0)
	{ 
		msg_snd_id = msgget(ISP_AVS_BINDER_KEY, IPC_EXCL | IPC_CREAT | 0660);
		
		if(msg_snd_id < 0)
		{
			return 0;
		}

	}
	
	ISP_AV_MSG_INFO msg_info;
	memset(&msg_info, 0, sizeof(msg_info));

	msg_info.isp_av_msg_to	 = ISP_PORT;
	msg_info.isp_av_msg_from = ISP_PORT;

	if(argc > 1)
	{
		if(strcmp(argv[1], "0")==0)
		{
			msg_info.isp_av_command = ISP_AV_CMD_ISP_MARK;
			if(argc > 2)
			{
				if(strcmp(argv[2], "0")==0)
					msg_info.isp_mark.value = 0;
				else
					msg_info.isp_mark.value = 1;
			}
			printf("send ISP_AV_CMD_ISP_MARK %d\n", msg_info.isp_mark.value);
		}

		if(strcmp(argv[1], "1")==0)
		{
			msg_info.isp_av_command = ISP_AV_CMD_ISP_RESOLUTION;
			if(argc > 2)
			{
				if(strcmp(argv[2], "0")==0)
					msg_info.isp_resolution = {1280, 720, 30, 0};
				else
					msg_info.isp_resolution = {1920, 1088, 30, 0};
			}
			printf("send ISP_AV_CMD_ISP_RESOLUTION %d\n", msg_info.isp_resolution.width);
		}

		if(strcmp(argv[1], "2")==0)
		{
			msg_info.isp_av_command = ISP_AV_CMD_ISP_ZOOM;
			if(argc > 2)
			{
				msg_info.isp_zoom.zoom = atoi(argv[2]);
			}
			printf("send ISP_AV_CMD_ZOOM %d\n", msg_info.isp_zoom.zoom);
		}
	}
	msgsnd(msg_snd_id, &msg_info, ISP_AV_MSG_SIZE, 0);
    return 0;
}
