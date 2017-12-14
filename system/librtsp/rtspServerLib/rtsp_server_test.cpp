#include <unistd.h>
#include "rtsp_server.h"
#include <qsdk/videobox.h>
#include <stdio.h>
#include <string.h>
#include <qsdk/audiobox.h>

int main(int argc, char** argv) {
	int w,h;
	video_get_resolution("isp-out",&w,&h);
	printf("w:%d,h:%d\n",w,h);
	struct font_attr tutk_attr;	
	memset(&tutk_attr, 0, sizeof(struct font_attr));	
	sprintf((char *)tutk_attr.ttf_name, "songti");	
	tutk_attr.font_color = 0x00ffffff;	
	tutk_attr.back_color = 0x20000000;	
	
	//marker_set_mode("marker0", "manual", "%t%Y/%M/%D  %H:%m:%S", &tutk_attr);
	marker_set_mode("marker0", "auto", "%t%Y/%M/%D  %H:%m:%S", &tutk_attr);
	//marker_set_string("marker0","‰∏≠Êñá");
	//marker_set_string("marker0","123456faehueyhdw‰∏≠ÂõΩÊ∑±Âú≥");
	//marker_set_string("marker0","13579pqwtumaqb…Ó€⁄”Ø∑ΩŒ¢");

	//marker_set_mode("marker1", "manual", "%t%Y/%M/%D  %H:%m:%S", &tutk_attr);
	//marker_set_string("marker0","‰∏≠Êñá");
	//marker_set_string("marker1","135apagfkv…Ó€⁄”Ø∑ΩŒ¢");
	rtsp_server_init(0,NULL,NULL);
	rtsp_server_start();
	while(1)
	{
	#if 0
		 struct font_attr fontAttr;
		 memset(&fontAttr,0,sizeof(fontAttr));
		 sprintf((char *)fontAttr.ttf_name,"arial");
		 fontAttr.font_size = 0;
		 fontAttr.font_color = 0x00ffffff;
		 //fontAttr.back_color = 0x80CCCCCC;
		 fontAttr.back_color = 0x20000000;
		 fontAttr.mode = MIDDLE;
		 printf("********ZZY marker_set_mode start\n");
		 marker_set_mode("marker0", "auto", "%t%Y/%M/%D %H:%m:%S", &fontAttr);
		 printf("*********ZZY marker_set_mode over\n");
	#endif
		 usleep(200*1000);
	}
	rtsp_server_stop();
}

