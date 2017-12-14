#include <dyncmd/commandline.h>

#include <iostream>
#include <termios.h>
#include <sstream>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include <ispc/integral.h>
#include <ispc/ldws.h>
#include <ispc/fcws.h>

#include "adas_gui_binder.h"

#define ADAS_WIDTH  640//720
#define ADAS_HEIGHT 360//480
#define LDWS_DEPTH  1

#define SCREEN_WIDTH	800
#define SCREEN_HEIGHT	480

typedef enum
{
	ADAS_ALGO_DISABLE 	= 0,
	ADAS_ALGO_ENABLE	= 1
}ADAS_ALGO_STATUS;

typedef enum 
{
	STATUS_IDLE 			= 0,
	STATUS_LOCKED			= 1,
	STATUS_PREPARED			= 2,
	STATUS_FINISHED			= 3,
	STATUS_DISTROYED		= 4
}ADAS_STATUS;


typedef struct
{
	bool loopActive_status;
	bool buffer_status;
	unsigned char *img_original;
	unsigned char *data_proc;
}T_THREAD_STRUCT;

typedef struct
{
	int veh_lane_status;
	int left_lane_x1;
	int left_lane_y1;
	int left_lane_x2;
	int left_lane_y2;
	int right_lane_x1;
	int right_lane_y1;
	int right_lane_x2;
	int right_lane_y2;
}T_LDWS_RESULT;

typedef struct
{
	int target_status;
	int target_distance;
	int target_left;
	int target_top;
	int target_right;
	int target_bottom;
}T_FCWS_RESULT;

typedef struct
{
	ADAS_STATUS img_status;
	ADAS_STATUS adas_status;
	int adas_ldws_enable;
	int adas_fcws_enable;
	unsigned int img_count;
	unsigned int img_width;
	unsigned int img_height;
	unsigned int img_stride;
	unsigned int proc_width;
	unsigned int proc_height;
	unsigned int screen_width;
	unsigned int screen_height;
	pthread_t adws_proc_id;
	pthread_t msg_proc_id;
	pthread_t scale_proc_id;
	int adas_gui_msg_id;
	unsigned long long *integral_image;
}T_ADAS_GBL_CTRL;

typedef struct
{
	T_ADAS_GBL_CTRL tAdasGblCtrl;
	T_THREAD_STRUCT tThreadStruct;
	ldws_params_t	*ptLdwsParaCtrl;
	ldws_info_t 	*ptLdwsInfo;
	T_LDWS_RESULT	tLdwsResult;
	fcws_params_t 	*ptFcwsParaCtrl;
	fcws_info_t 	*ptFcwsInfo;
	T_FCWS_RESULT	atFcwsResult[3];
}T_ADAS_PROC_CTRL;

static T_ADAS_PROC_CTRL *gptAdasProcCtrl = NULL;

#ifdef ADAS_ADD_TO_ISP_MODE
void AdasSetLdwsStatusToImage(int status, int image_width, void *img_src)
{
	int i, j;
	int startX, startY, endX, endY;
	int statusValue = status;
	unsigned char *imgBuf = (unsigned char *)img_src;

	startY = 200;
	endY = 250;
	if(LDWS_STATE_NORMAL == statusValue)
	{
		startX = (image_width >> 1) - 20;
		endX = startX + 50;
	}
	if(LDWS_STATE_DEPARTURE_LEFT == statusValue)
	{
		startX = 100;
		endX = 150;
	}
	if(LDWS_STATE_DEPARTURE_RIGHT == statusValue)
	{
		startX = image_width - 150;
		endX = image_width - 100;
	}

	for(i = startY; i < endY; i++)
	{
		for(j = startX; j < endX; j++)
		{
			*(imgBuf + i * image_width + j) = 250;
		}
	}
		
}

void AdasSetLdwsLineToImage(int px, int py, int qx, int qy, int image_width, int image_height, void *img_src)
{
	int i ,j;
	int curX, curY;
	int startX, startY, endX, endY;
	unsigned char *imgBuf = (unsigned char *)img_src;

	if(py == qy)
	{
		qy++;
	}
	
	if(py > qy)
	{
		startX = qx;
		startY = qy;
		endX = px;
		endY = py;
	}
	else
	{
		startX = px;
		startY = py;
		endX = qx;
		endY = qy;
	}
	
	for(j = startY; j < endY; j++)
	{
		curY = j;
		curX = startX + (curY - startY)*(endX - startX)/(endY - startY);
		curY = curY * image_height / ADAS_HEIGHT;
		curX = curX * image_width / ADAS_WIDTH;
		*(imgBuf + curY * image_width + curX ) = 250;
		*(imgBuf + curY * image_width + curX + 1 ) = 250;
		*(imgBuf + curY * image_width + curX + 2) = 250;
	}
}

void AdasSetFcwsTrectToImage(int px, int py, int qx, int qy, int image_width, int image_height, void *img_src)
{
	int i , j;
	int left, top, right, bottom;
	unsigned char *imgBuf = (unsigned char *)img_src;

	if(px == 0 && py == 0 && qx == 0 && qy == 0)
	{
		return;
	}
	left = px * image_width / ADAS_WIDTH;
	top = py * image_height / ADAS_HEIGHT;
	right = qx * image_width / ADAS_WIDTH;
	bottom = qy * image_height / ADAS_HEIGHT;
	for( i = top; i < bottom; i++)
	{
		//top line
		if((i == top) || (i == top + 1) || (i == top + 2))
		{
			for(j = left; j < right; j++)
			{
				*(imgBuf + i * image_width + j) = 250;
			}
		}
		//bottom line
		if((i == bottom) || (i == bottom - 1) ||(i == bottom - 2))
		{
			for(j = left; j < right; j++)
			{
				*(imgBuf + i * image_width + j) = 250;
			}
		}

		for(j = left; j < right; j++)
		{
			//left line
			if((j == left) || (j == left + 1) || (j == left + 2))
			{
				*(imgBuf + i * image_width + j) = 250;
			}
			//right line
			if((j == right) || (j == right - 1) || (j == right - 2))
			{
				*(imgBuf + i * image_width + j) = 250;
			}
		}
	}
}

int AdasGetoverlayIndex(char* pChar)
{
	int Index = 0xFFFFFFFF;
	
    if (*pChar >= '0' && *pChar <= '9')
    {
        Index = *pChar - '0' + 1;
    }
	else if(*pChar >= 65 && *pChar <= 90)
	{
		Index = *pChar - 52;
	}
	else if(*pChar >= 128 && *pChar <= 174)
	{
		Index = *pChar - 89;
	}
    else if (*pChar == '/')
    {
    	Index = 11;
    }
    else if (*pChar == ':')
    {
        Index = 12;
    }
	else if (*pChar == '.')
    {
        Index = 89;
    }
	else if (*pChar == '`')
    {
        Index = 90;
    }
	else if (*pChar == ',')
    {
        Index = 88;
    }
	else if (*pChar == 'k')
    {
        Index = 86;
    }
	else if (*pChar == 'm')
    {
        Index = 87;
    }
	else if (*pChar == 'h')
    {
        Index = 85;
    }
    else if(*pChar == 32)
    {
		Index = 0;
	}

	return Index;
}

void AdasSetImgOverlay(void* ImgAddr, int ImgWidth, int ImgHeight, char overlayString[], int x, int y)
{
	static int flag = 1;
	FILE *fp;
	int i, j;
	int count, Index, startx, starty;
	char* pChar;
	static IMG_UINT8 imgOverlayBuf[176640];
	static IMG_UINT32 pBufSize;

	char* imgBuf = (char*)ImgAddr;
	if(flag)
	{
		fp = fopen("/etc/ovimg_timestamp.bin", "rb");
		if(NULL != fp)
		{
			pBufSize = fread(imgOverlayBuf, 1, 176640, fp);
			fclose(fp);
			flag = 0;
		}
	}
	if(0 != flag)
	{
		return;
	}

	count = 0;
	startx = x * ImgWidth / ADAS_WIDTH;
	starty = y * ImgHeight / ADAS_HEIGHT;
	pChar = overlayString;
	while(*pChar != '\0')
	{
		Index = AdasGetoverlayIndex(pChar);
		for(i = 0; i < 40; i++)
		{
			for(j = 0; j < 32; j++)
			{	
				
				if(imgOverlayBuf[32*60*Index + i*32 + j] > 200)
				{
					*(imgBuf + (i + starty) * ImgWidth+ count * 32 + j + startx) = 250;//imgOverlayBuf[32*60 + i*32 + j];
				}
			}
		}

		count++;
		pChar++;
	}
}

void AdasSetFcwsDistanceToImage(int distance, int startX, int startY, void *img_src)
{
	int i;
	int dis = distance;
	char disString[8] = {0};
	int image_width = 1920;
	int image_height = 1088;

	disString[2] = dis % 10 + 48;
	dis /= 10;
	disString[1] = dis % 10 + 48;
	dis /= 10;
	disString[0] = dis % 10 + 48;
	disString[3] = 'm';
	AdasSetImgOverlay(img_src, image_width, image_height, disString, startX, startY);
}
#endif

void AdasResultProc(void *img_Buf, int img_width, int img_height)
{
	int i;
	
#ifdef SUPPORT_ADAS_MODE
#ifdef ADAS_ADD_TO_ISP_MODE
	if(ADAS_ALGO_ENABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_ldws_enable)
	{
		AdasSetLdwsLineToImage(gptAdasProcCtrl->tLdwsResult.left_lane_x1, 
								gptAdasProcCtrl->tLdwsResult.left_lane_y1, 
								gptAdasProcCtrl->tLdwsResult.left_lane_x2, 
								gptAdasProcCtrl->tLdwsResult.left_lane_y2, 
								img_width, img_height, img_Buf);
		AdasSetLdwsLineToImage(gptAdasProcCtrl->tLdwsResult.right_lane_x1, 
								gptAdasProcCtrl->tLdwsResult.right_lane_y1, 
								gptAdasProcCtrl->tLdwsResult.right_lane_x2, 
								gptAdasProcCtrl->tLdwsResult.right_lane_y2, 
								img_width, img_height, img_Buf);
		AdasSetLdwsStatusToImage(gptAdasProcCtrl->tLdwsResult.veh_lane_status, img_width, img_Buf);
	}
	if(ADAS_ALGO_ENABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_fcws_enable)
	{
		for(i = 0; i < 3; i++)
		{
			if(gptAdasProcCtrl->atFcwsResult[i].target_status == FCWS_STATE_FIND)
			{
				AdasSetFcwsTrectToImage(gptAdasProcCtrl->atFcwsResult[i].target_left,
										gptAdasProcCtrl->atFcwsResult[i].target_top,
										gptAdasProcCtrl->atFcwsResult[i].target_right,
										gptAdasProcCtrl->atFcwsResult[i].target_bottom,
										img_width, img_height, img_Buf);
				AdasSetFcwsDistanceToImage(gptAdasProcCtrl->atFcwsResult[i].target_distance,
											gptAdasProcCtrl->atFcwsResult[i].target_left,
											gptAdasProcCtrl->atFcwsResult[i].target_bottom,
											img_Buf);
			}
		}
	}
	
#endif
#endif
	
}

void AdasPrepareData(void *img_buf, int src_width, int src_height, int src_stride)
{
	if(NULL == gptAdasProcCtrl)
	{
		return;
	}

#ifdef SUPPORT_ADAS_MODE
	gptAdasProcCtrl->tThreadStruct.img_original = (unsigned char *)img_buf;
	gptAdasProcCtrl->tAdasGblCtrl.img_width = (unsigned int)src_width;
	gptAdasProcCtrl->tAdasGblCtrl.img_height = (unsigned int)src_height;
	gptAdasProcCtrl->tAdasGblCtrl.img_stride = (unsigned int)src_stride;
	gptAdasProcCtrl->tAdasGblCtrl.img_count++;
#endif
}

int AdasDataScaleproc(unsigned char *img_src, unsigned char *img_dst, 
								int src_width, int src_height, int src_stride)
{
	int i, j;
	int stride_step = 1;

	if(NULL == img_src || NULL == img_dst)
	{
		return -1;
	}

	if(1920 == src_width)
	{
		stride_step = 3;
	}
	if(1280 == src_width)
	{
		stride_step = 2;
	}

	for(i = 0; i < 360; i++)
	{
		for(j = 0; j < 640;j++)
		{
			*(img_dst + i * 640 + j) = *(img_src + i * stride_step * src_stride + j * stride_step);
		}
	}
	img_dst += 360 * 640;
	img_src += src_stride * src_height;
	for(i = 0; i < 180; i++)
	{
		for(j = 0; j < 640; j+=2)
		{
			*(img_dst + i * 640 + j) = *(img_src + i * stride_step * src_stride + j * stride_step);
			*(img_dst + i * 640 + j + 1) = *(img_src + i * stride_step * src_stride + j * stride_step + 1);
		}
	}
	return 0;

}

ldws_err AdasLdwsResetRoi(int x_camera_position, int y_upper, int y_lower)
{
	ldws_err ret;
	unsigned int proc_width = gptAdasProcCtrl->tAdasGblCtrl.proc_width;
	unsigned int proc_height = gptAdasProcCtrl->tAdasGblCtrl.proc_height;
	ldws_params_t *ptldws_params = gptAdasProcCtrl->ptLdwsParaCtrl;

	if(NULL == ptldws_params)
	{
		return LDWS_ERR_NULL_PARAMETER;
	}

	ptldws_params->image_width      = proc_width;
   	ptldws_params->image_height     = proc_height;
    ptldws_params->image_rowsize    = proc_width;

    ptldws_params->camera_position             = x_camera_position;
    ptldws_params->detect_upper                = y_upper;
    ptldws_params->detect_lower                = y_lower;
    ptldws_params->min_detect_lane_width       =  300;
    ptldws_params->max_detect_lane_width       = 1000;
    ptldws_params->min_detect_lane_mark_length = 10;

    ptldws_params->alarm_time_sec = 3;
    ptldws_params->sensitivity    = 0.25;  

	ldws_init(ptldws_params);
	
	ret = ldws_set_roi(0,(y_upper+((y_lower-y_upper)/3)+1), proc_width, (2*(y_lower-y_upper)/3)+1);
	
	return ret;
}

fcws_err AdasFcwsResetRoi(int x_camera_position, int y_upper, int y_lower)
{
	fcws_err ret;
	double camera_height,camera_focal,width_rate;
	int roi_x,roi_y,roi_w,roi_h;
	unsigned int proc_width = gptAdasProcCtrl->tAdasGblCtrl.proc_width;
	unsigned int proc_height = gptAdasProcCtrl->tAdasGblCtrl.proc_height;
	fcws_params_t *fcws_params = gptAdasProcCtrl->ptFcwsParaCtrl;

	camera_height = 1.2;
	camera_focal  = 300;
	width_rate    = 0.85;

    fcws_params->image_width      =  proc_width;
    fcws_params->image_height     =  proc_height;
    fcws_params->image_rowsize    =  proc_width;

    fcws_params->detect_upper                =  y_upper;
    fcws_params->detect_lower                =  y_lower;
    fcws_params->width_rate                  =  width_rate;
    fcws_params->camera_position             =  x_camera_position;

    fcws_params->camera_focal                = camera_focal;
    fcws_params->camera_height               = camera_height;

    fcws_init(fcws_params);
    roi_x =0;
    roi_y =0;
    roi_w =proc_width;
    roi_h =proc_height;
    ret = fcws_set_roi(roi_x,roi_y,roi_w,roi_h);
	
	return ret;
}

int AdasGuiInitMsg()
{
	int msg_id;
	
    msg_id = msgget(ADAS_GUI_BINDER_KEY, IPC_EXCL | IPC_CREAT | 0660);
    if(msg_id < 0)
    {
        msg_id = msgget(ADAS_GUI_BINDER_KEY, 0);
		if(msg_id < 0)
		{
			return -1;
		}
    }
	gptAdasProcCtrl->tAdasGblCtrl.adas_gui_msg_id = msg_id;
	printf("%s msg_id[%d]\n", __FUNCTION__, gptAdasProcCtrl->tAdasGblCtrl.adas_gui_msg_id);

	return 0;
}


void *AdasGuiRcvMsgCallback(void *args)
{
    int ret;
	int msg_id;
    ADAS_GUI_MSG_INFO msg_info;
    msg_info.adas_gui_msg_to = ADAS_PORT;
	msg_id = gptAdasProcCtrl->tAdasGblCtrl.adas_gui_msg_id;

	printf("%s start msg_id[%d]!\n", __FUNCTION__, msg_id);
    while(1)
    {
		ret = msgrcv(msg_id, &msg_info, ADAS_GUI_MSG_SIZE, msg_info.adas_gui_msg_to, 0);
        if(ret < 0)
        {
            usleep(1000);
            continue;
        }
		
        switch(msg_info.adas_gui_command) 
        {
        	case ADAS_GUI_CMD_LDWS_START:
				{
					gptAdasProcCtrl->tAdasGblCtrl.adas_ldws_enable = ADAS_ALGO_ENABLE;
					printf("%s command ADAS_GUI_CMD_LDWS_START\n", __FUNCTION__);
				}
				break;
			case ADAS_GUI_CMD_LDWS_STOP:
				{
					gptAdasProcCtrl->tAdasGblCtrl.adas_ldws_enable = ADAS_ALGO_DISABLE;
					printf("%s command ADAS_GUI_CMD_LDWS_STOP\n", __FUNCTION__);
				}
				break;
            case ADAS_GUI_CMD_LDWS_CONFIG:
                {
					unsigned int camera_position_x = msg_info.adas_ldws_config.camera_position_x;
					unsigned int camera_position_y = msg_info.adas_ldws_config.camera_position_y;
					unsigned int veh_hood_top_y = msg_info.adas_ldws_config.veh_hood_top_y;
                    AdasLdwsResetRoi(camera_position_x, camera_position_y, veh_hood_top_y);
					printf("%s command ADAS_GUI_CMD_LDWS_CONFIG[%d][%d][%d]\n", __FUNCTION__,
							camera_position_x, camera_position_y, veh_hood_top_y);
                }
                break;
			case ADAS_GUI_CMD_FCWS_START:
				{
					gptAdasProcCtrl->tAdasGblCtrl.adas_fcws_enable = ADAS_ALGO_ENABLE;
					printf("%s command ADAS_GUI_CMD_FCWS_START\n", __FUNCTION__);
				}
				break;
			case ADAS_GUI_CMD_FCWS_STOP:
				{
					gptAdasProcCtrl->tAdasGblCtrl.adas_fcws_enable = ADAS_ALGO_DISABLE;
					printf("%s command ADAS_GUI_CMD_FCWS_STOP\n", __FUNCTION__);
				}
				break;
			case ADAS_GUI_CMD_FCWS_CONFIG:
				{
					unsigned int camera_position_x = msg_info.adas_fcws_config.camera_position_x;
					unsigned int camera_position_y = msg_info.adas_fcws_config.camera_position_y;
					unsigned int veh_hood_top_y = msg_info.adas_fcws_config.veh_hood_top_y;
					AdasFcwsResetRoi(camera_position_x, camera_position_y, veh_hood_top_y);
					printf("%s command ADAS_GUI_CMD_LDWS_CONFIG[%d][%d][%d]\n", __FUNCTION__,
								camera_position_x, camera_position_y, veh_hood_top_y);
				}
				break;
			case ADAS_GUI_CMD_SHOW_RESOLUTION:
				{
					gptAdasProcCtrl->tAdasGblCtrl.screen_width = msg_info.adas_show_resolution.screen_width;
					gptAdasProcCtrl->tAdasGblCtrl.screen_height = msg_info.adas_show_resolution.screen_height;
					printf("%s command ADAS_GUI_CMD_SHOW_RESOLUTION[%d][%d]\n", __FUNCTION__,
								gptAdasProcCtrl->tAdasGblCtrl.screen_width,
								gptAdasProcCtrl->tAdasGblCtrl.screen_height);
				}
				break;
            default:
                printf("%s does not deal this command(%d) \n", __FUNCTION__, msg_info.adas_gui_command);
               	break;
        }
    }
}

void *AdasDataScaleThread(void *args)
{
	static int LastCount = 0;
	
	while(1)
	{
		usleep(100);

		if(NULL == gptAdasProcCtrl)
		{
			continue;
		}
		
		if(ADAS_ALGO_DISABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_ldws_enable
			&& ADAS_ALGO_DISABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_fcws_enable)
		{
			continue;
		}
			
		if(gptAdasProcCtrl->tAdasGblCtrl.adas_status != STATUS_IDLE
			&& gptAdasProcCtrl->tAdasGblCtrl.adas_status != STATUS_FINISHED)
		{
			continue;
		}

		
		if(LastCount == gptAdasProcCtrl->tAdasGblCtrl.img_count)
		{
			continue;
		}

		gptAdasProcCtrl->tAdasGblCtrl.img_status = STATUS_LOCKED;
		AdasDataScaleproc(gptAdasProcCtrl->tThreadStruct.img_original,
							gptAdasProcCtrl->tThreadStruct.data_proc,
							gptAdasProcCtrl->tAdasGblCtrl.img_width,
							gptAdasProcCtrl->tAdasGblCtrl.img_height,
							gptAdasProcCtrl->tAdasGblCtrl.img_stride);
		gptAdasProcCtrl->tAdasGblCtrl.img_status = STATUS_FINISHED;
		LastCount = gptAdasProcCtrl->tAdasGblCtrl.img_count;
	}
}

void AdasGuiSendProcRsult()
{
	int i;
	int tmp;
	static int count = 0;
	static int eCount = 0;
    ADAS_GUI_MSG_INFO msg_info;
	fcws_info_t *ptFcwsInfo = gptAdasProcCtrl->ptFcwsInfo;
	ldws_info_t *ptLdwsInfo = gptAdasProcCtrl->ptLdwsInfo;
	int msg_id = gptAdasProcCtrl->tAdasGblCtrl.adas_gui_msg_id;
	int screen_width = gptAdasProcCtrl->tAdasGblCtrl.screen_width;
	int screen_height = gptAdasProcCtrl->tAdasGblCtrl.screen_height;
	unsigned int proc_width = gptAdasProcCtrl->tAdasGblCtrl.proc_width;
	unsigned int proc_height = gptAdasProcCtrl->tAdasGblCtrl.proc_height;

	if(msg_id < 0)
    {
        printf("%s msg_id  error [%d]\n", __FUNCTION__, msg_id);
		return;
    }
	
	if(0 == proc_width || 0 == proc_height)
	{
		printf("%s error proc_width[%d], proc_height[%d]\n", __FUNCTION__, proc_width, proc_height);
		return;
	}

	msg_info.adas_proc_result.adas_ldws_result.left_lane_x1 = ptLdwsInfo->left_lane_x1 * screen_width / proc_width;
	msg_info.adas_proc_result.adas_ldws_result.left_lane_y1 = ptLdwsInfo->left_lane_y1 * screen_height / proc_height;
	msg_info.adas_proc_result.adas_ldws_result.left_lane_x2 = ptLdwsInfo->left_lane_x2 * screen_width / proc_width;
	msg_info.adas_proc_result.adas_ldws_result.left_lane_y2 = ptLdwsInfo->left_lane_y2 * screen_height / proc_height;
	msg_info.adas_proc_result.adas_ldws_result.right_lane_x1 = ptLdwsInfo->right_lane_x1 * screen_width / proc_width;
	msg_info.adas_proc_result.adas_ldws_result.right_lane_y1 = ptLdwsInfo->right_lane_y1 * screen_height / proc_height;
	msg_info.adas_proc_result.adas_ldws_result.right_lane_x2 = ptLdwsInfo->right_lane_x2 * screen_width / proc_width;
	msg_info.adas_proc_result.adas_ldws_result.right_lane_y2 = ptLdwsInfo->right_lane_y2 * screen_height / proc_height;
	tmp = (int)ptLdwsInfo->ldws_state;
	msg_info.adas_proc_result.adas_ldws_result.veh_ldws_state = (ADAS_LDWS_STATE)tmp;
	tmp = (int)ptLdwsInfo->flag;
	msg_info.adas_proc_result.adas_ldws_result.ldws_lanemark = (ADAS_LDWS_LANEMARK)tmp;

	msg_info.adas_proc_result.adas_fcws_result.target_num = 0;
	for(i = 0; i < ADAS_FCWS_TARGET_MAX; i++)
	{
		if(ptFcwsInfo->state[i] == FCWS_STATE_FIND)
		{
			msg_info.adas_proc_result.adas_fcws_result.target_info[msg_info.adas_proc_result.adas_fcws_result.target_num].target_distance
				= ptFcwsInfo->distance[i];
			msg_info.adas_proc_result.adas_fcws_result.target_info[msg_info.adas_proc_result.adas_fcws_result.target_num].target_left
				= ptFcwsInfo->car_x[i] * screen_width / proc_width;
			msg_info.adas_proc_result.adas_fcws_result.target_info[msg_info.adas_proc_result.adas_fcws_result.target_num].target_top
				= ptFcwsInfo->car_y[i] * screen_height / proc_height;
			msg_info.adas_proc_result.adas_fcws_result.target_info[msg_info.adas_proc_result.adas_fcws_result.target_num].target_right
				= (ptFcwsInfo->car_x[i] + ptFcwsInfo->car_width[i]) * screen_width / proc_width;
			msg_info.adas_proc_result.adas_fcws_result.target_info[msg_info.adas_proc_result.adas_fcws_result.target_num].target_bottom
				= (ptFcwsInfo->car_y[i] + ptFcwsInfo->car_height[i]) * screen_height / proc_height;
			msg_info.adas_proc_result.adas_fcws_result.target_num++;
		}
	}
	
	msg_info.adas_gui_command = ADAS_GUI_CMD_ADAS_RESULT;
    msg_info.adas_gui_msg_to   = GUI_PORT;
    msg_info.adas_gui_msg_from = ADAS_PORT;
    tmp = msgsnd(msg_id, &msg_info, ADAS_GUI_MSG_SIZE, IPC_NOWAIT);
	if(tmp == -1)
	{
		msqid_ds msgBuf;
		msgctl( msg_id, IPC_STAT, &msgBuf);
		printf("%s msgBuf msg_qnum[%d]\n", __FUNCTION__, msgBuf.msg_qnum);
		gptAdasProcCtrl->tAdasGblCtrl.adas_ldws_enable = ADAS_ALGO_DISABLE;
		gptAdasProcCtrl->tAdasGblCtrl.adas_fcws_enable = ADAS_ALGO_DISABLE;
		printf("%s disable ldws proc & fcws proc\n", __FUNCTION__);
		
	}

	if(count++%200==0)
    {
        printf("%s veh_ldws_state %d, veh_fcws_num:%d\n", __FUNCTION__, 
				ptLdwsInfo->ldws_state, msg_info.adas_proc_result.adas_fcws_result.target_num);
    }

	
}

void AdasResourceRelease()
{
	if(NULL == gptAdasProcCtrl)
	{
		return;
	}
	
	if(gptAdasProcCtrl->ptFcwsInfo)
	{
		free(gptAdasProcCtrl->ptFcwsInfo);
	}

	if(gptAdasProcCtrl->ptFcwsParaCtrl)
	{
		free(gptAdasProcCtrl->ptFcwsParaCtrl);
	}

	if(gptAdasProcCtrl->ptLdwsInfo)
	{
		free(gptAdasProcCtrl->ptLdwsInfo);
	}

	if(gptAdasProcCtrl->ptLdwsParaCtrl)
	{
		free(gptAdasProcCtrl->ptLdwsParaCtrl);
	}

	if(gptAdasProcCtrl->tThreadStruct.data_proc)
	{
		free(gptAdasProcCtrl->tThreadStruct.data_proc);
	}

	if(gptAdasProcCtrl->tAdasGblCtrl.integral_image)
	{
		free(gptAdasProcCtrl->tAdasGblCtrl.integral_image);
	}

	free(gptAdasProcCtrl);
	
}

void *AdasProcMainThread(void *args)
{
	int i;
	int y_start,y_end;
	integral_roi i_roi;
	
	int ret;
	int adas_widthstep;

	int roi_x,roi_y,roi_w,roi_h;
	unsigned int proc_width, proc_height;
	double camera_height, camera_focal, width_rate;	
	int lane_width , lane_mark;

	unsigned char *pucImgBuf = NULL;
	unsigned long long *integral_image = NULL;
	ldws_params_t *ptldws_params = NULL;
	ldws_info_t *ldws_info = NULL;
	T_LDWS_RESULT *ptLdwsResult = NULL;
	
	fcws_params_t *fcws_params = NULL;
	fcws_info_t *fcws_info = NULL;
	T_FCWS_RESULT *ptFcws_result = NULL;

	float fpsDeltaTime = 0;
	static int procCount = 0;
	static struct timeval timeStart, timeEnd;
	int deltaX, deltaY;

	proc_width = gptAdasProcCtrl->tAdasGblCtrl.proc_width;
	proc_height = gptAdasProcCtrl->tAdasGblCtrl.proc_height;
	//***initial  LDWS***//
	y_start=(int)(13.5*proc_height/32);
	y_end  =(int)(26.5*proc_height/32);
	
	ptldws_params = gptAdasProcCtrl->ptLdwsParaCtrl;

	ptldws_params->image_width      			= proc_width;
	ptldws_params->image_height     			= proc_height;
	ptldws_params->image_rowsize    			= proc_width;

	ptldws_params->camera_position             	= 0;
	ptldws_params->detect_upper                	= y_start;
	ptldws_params->detect_lower                	= y_end;
	ptldws_params->min_detect_lane_width       	= 300;
	ptldws_params->max_detect_lane_width       	= 1000;
	ptldws_params->min_detect_lane_mark_length 	= 10;

	ptldws_params->alarm_time_sec = 3;
	ptldws_params->sensitivity    = 0.25;  

	ret = ldws_init(ptldws_params);
	if(ret)
	{
		printf("%s ldws_init error[%d]\n", __FUNCTION__, ret);
	}
	ret = ldws_set_roi(0,(y_start+((y_end-y_start)/3)+1), proc_width, (2*(y_end-y_start)/3)+1);
	if(ret)
	{
		printf("%s ldws_set_roi error[%d]\n", __FUNCTION__, ret);
	}

	//***initial  FCWS***//
	y_start=(int)(13.5*proc_height/32);
	y_end  =(int)(26.5*proc_height/32);
	
	camera_height = 1.2;
	camera_focal  = 300;
	width_rate	  = 0.85;
	lane_width	  = 300;
	lane_mark	  = 10;

	fcws_params = gptAdasProcCtrl->ptFcwsParaCtrl;
	fcws_params->image_width	  	= proc_width;
	fcws_params->image_height	  	= proc_height;
	fcws_params->image_rowsize	  	= proc_width;

	fcws_params->detect_upper		= y_start;
	fcws_params->detect_lower		= y_end;
	fcws_params->width_rate 		= width_rate;
	fcws_params->camera_position	= 0;

	fcws_params->camera_focal		= camera_focal;
	fcws_params->camera_height		= camera_height;

	ret = fcws_init(fcws_params);
	if(ret)
	{
		printf("%s fcws_init error[%d]\n", __FUNCTION__, ret);
	}
	roi_x = 0;
	roi_y = 0;
	roi_w = proc_width;
	roi_h = proc_height;
	ret = fcws_set_roi(roi_x,roi_y,roi_w,roi_h);
	if(ret)
	{
		printf("%s fcws_set_roi error[%d]\n", __FUNCTION__, ret);
	}

	i_roi.x = 0;
	i_roi.y = 0;
	i_roi.width = proc_width;
	i_roi.height = proc_height;
	
	pucImgBuf = gptAdasProcCtrl->tThreadStruct.data_proc;
	integral_image = gptAdasProcCtrl->tAdasGblCtrl.integral_image;

	adas_widthstep = (proc_width*LDWS_DEPTH+3)&(~3); //align 4 bytes
	ldws_info = gptAdasProcCtrl->ptLdwsInfo;
	fcws_info = gptAdasProcCtrl->ptFcwsInfo;

	gettimeofday(&timeStart, 0);
	gptAdasProcCtrl->tAdasGblCtrl.adas_status = STATUS_IDLE;
	while (gptAdasProcCtrl->tThreadStruct.loopActive_status)
	{
		usleep(100);
		if(gptAdasProcCtrl->tAdasGblCtrl.img_status == STATUS_FINISHED)
		{
			if(ADAS_ALGO_DISABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_ldws_enable 
				&& ADAS_ALGO_DISABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_fcws_enable)
			{
				continue;
			}
            /*1 integral*/
			gptAdasProcCtrl->tAdasGblCtrl.adas_status = STATUS_LOCKED;
            ret = integral_process(pucImgBuf, adas_widthstep, integral_image, proc_width, &i_roi);
			if(ret)
			{
				printf("%s integral_process error[%d]\n", __FUNCTION__, ret);
			}
			
			gptAdasProcCtrl->tAdasGblCtrl.adas_status = STATUS_PREPARED;
			if(ADAS_ALGO_ENABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_ldws_enable)
			{
	            /*2 ldws_core*/
	            ret = ldws_process(pucImgBuf, (const void *)integral_image);
				if(ret)
				{
					printf("%s ldws_process error[%d]\n", __FUNCTION__, ret);
				}

	            /*3 get result*/
	            ret = ldws_get_result(ldws_info);
				if(ret)
				{
					printf("%s ldws_get_result error[%d]\n", __FUNCTION__, ret);
				}
				//AdasGuiSendLdwsResult();

				#ifdef ADAS_ADD_TO_ISP_MODE
				ptLdwsResult = (T_LDWS_RESULT *)&gptAdasProcCtrl->tLdwsResult;
				ptLdwsResult->veh_lane_status = ldws_info->ldws_state;
				if(ldws_info->ldws_state == 	LDWS_STATE_NORMAL ||
					ldws_info->ldws_state == LDWS_STATE_DEPARTURE_LEFT ||
					ldws_info->ldws_state == LDWS_STATE_DEPARTURE_RIGHT)
				{
					deltaX = (ldws_info->left_lane_x1 - ldws_info->left_lane_x2)/4;
					deltaY = (ldws_info->left_lane_y1 - ldws_info->left_lane_y2)/4;
					ptLdwsResult->left_lane_x1 = ldws_info->left_lane_x2+ deltaX;
					ptLdwsResult->left_lane_y1 = ldws_info->left_lane_y2+ deltaY;
					ptLdwsResult->left_lane_x2 = ldws_info->left_lane_x2 + deltaX * 3;
					ptLdwsResult->left_lane_y2 = ldws_info->left_lane_y2 + deltaY * 3;
					deltaX = (ldws_info->right_lane_x1 - ldws_info->right_lane_x2)/4;
					deltaY = (ldws_info->right_lane_y1 - ldws_info->right_lane_y2)/4;
					ptLdwsResult->right_lane_x1 = ldws_info->right_lane_x2 + deltaX;
					ptLdwsResult->right_lane_y1 = ldws_info->right_lane_y2 + deltaY;
					ptLdwsResult->right_lane_x2 = ldws_info->right_lane_x2 + deltaX * 3;
					ptLdwsResult->right_lane_y2 = ldws_info->right_lane_y2 + deltaY * 3;
				}
				#endif
			}
			if(ADAS_ALGO_ENABLE == gptAdasProcCtrl->tAdasGblCtrl.adas_fcws_enable)
			{
				/*4 ldws_core*/
				ret = fcws_process(pucImgBuf, (const void *)integral_image, NULL);
				if(ret)
				{
					printf("%s fcws_process error[%d]\n", __FUNCTION__, ret);
				}
				
				/*5 get result*/
				
				memset(fcws_info, 0, sizeof(fcws_info_t));
				ret = fcws_get_result(fcws_info);
				if(ret)
				{
					printf("%s fcws_get_result error[%d]\n", __FUNCTION__, ret);
				}
				
				//AdasGuiSendFcwsResult();
				
				#ifdef ADAS_ADD_TO_ISP_MODE
				ptFcws_result = (T_FCWS_RESULT *)gptAdasProcCtrl->atFcwsResult;
				memset(ptFcws_result, 0, sizeof(T_FCWS_RESULT) * 3);
				for(i = 0; i < 3; i++)
				{
					if(fcws_info->state[i] == FCWS_STATE_FIND)
					{
						ptFcws_result = (T_FCWS_RESULT *)&gptAdasProcCtrl->atFcwsResult[i];
						ptFcws_result->target_status = fcws_info->state[i];
						ptFcws_result->target_distance = (int)fcws_info->distance[i];
						ptFcws_result->target_left = fcws_info->car_x[i];
						ptFcws_result->target_top = fcws_info->car_y[i];
						ptFcws_result->target_right = fcws_info->car_x[i] + fcws_info->car_width[i];
						ptFcws_result->target_bottom = fcws_info->car_y[i] + fcws_info->car_height[i];
					}
				}
				#endif
			}
			AdasGuiSendProcRsult();
			gptAdasProcCtrl->tAdasGblCtrl.adas_status = STATUS_FINISHED;

			procCount++;
			if(procCount % 1000 == 0)
			{
				gettimeofday(&timeEnd, 0);
				fpsDeltaTime = timeEnd.tv_sec - timeStart.tv_sec + (timeEnd.tv_usec - timeStart.tv_usec)/1000000;
				printf("adas_process fps [%.2f]\n", 1000.00/fpsDeltaTime);
				timeStart = timeEnd;
			}
		}
	}
	gptAdasProcCtrl->tAdasGblCtrl.adas_status = STATUS_DISTROYED;
    ldws_deinit();
	fcws_deinit();
	AdasResourceRelease();
	
	printf("thread exit loop.\n");
	pthread_exit(0);
}

int AdasProcInit()
{
	unsigned char *pucBuf = NULL;

#ifdef SUPPORT_ADAS_MODE

	pucBuf = (unsigned char *)malloc(sizeof(T_ADAS_PROC_CTRL));
	if(NULL == pucBuf)
	{
		return -1;
	}
	memset(pucBuf, 0, sizeof(T_ADAS_PROC_CTRL));
	gptAdasProcCtrl = (T_ADAS_PROC_CTRL *)pucBuf;

	pucBuf = (unsigned char *)malloc(ADAS_WIDTH*ADAS_HEIGHT*sizeof(unsigned long long));
	if(NULL == pucBuf)
	{
		return -1;
	}
	memset(pucBuf, 0, ADAS_WIDTH*ADAS_HEIGHT*sizeof(unsigned long long));
	gptAdasProcCtrl->tAdasGblCtrl.integral_image = (unsigned long long *)pucBuf;

	pucBuf = (unsigned char*)malloc(sizeof(unsigned char)*345600);
	if(NULL == pucBuf)
	{
		return -1;
	}
	memset(pucBuf, 0, sizeof(unsigned char)*345600);
	gptAdasProcCtrl->tThreadStruct.data_proc = (unsigned char *)pucBuf;

	pucBuf = (unsigned char*)malloc(sizeof(ldws_params_t));
	if(NULL == pucBuf)
	{
		return -1;
	}
	memset(pucBuf, 0, sizeof(ldws_params_t));
	gptAdasProcCtrl->ptLdwsParaCtrl = (ldws_params_t *)pucBuf;

	pucBuf = (unsigned char*)malloc(sizeof(ldws_info_t));
	if(NULL == pucBuf)
	{
		return -1;
	}
	memset(pucBuf, 0, sizeof(ldws_info_t));
	gptAdasProcCtrl->ptLdwsInfo = (ldws_info_t *)pucBuf;

	pucBuf = (unsigned char*)malloc(sizeof(fcws_params_t));
	if(NULL == pucBuf)
	{
		return -1;
	}
	memset(pucBuf, 0, sizeof(fcws_params_t));
	gptAdasProcCtrl->ptFcwsParaCtrl = (fcws_params_t *)pucBuf;

	pucBuf = (unsigned char*)malloc(sizeof(fcws_info_t));
	if(NULL == pucBuf)
	{
		return -1;
	}
	memset(pucBuf, 0, sizeof(fcws_info_t));
	gptAdasProcCtrl->ptFcwsInfo = (fcws_info_t *)pucBuf;
	gptAdasProcCtrl->tAdasGblCtrl.img_count = 0;
	gptAdasProcCtrl->tAdasGblCtrl.proc_width = ADAS_WIDTH;
	gptAdasProcCtrl->tAdasGblCtrl.proc_height = ADAS_HEIGHT;
	gptAdasProcCtrl->tAdasGblCtrl.screen_width = SCREEN_WIDTH;
	gptAdasProcCtrl->tAdasGblCtrl.screen_height = SCREEN_HEIGHT;

	gptAdasProcCtrl->tThreadStruct.loopActive_status = ADAS_ALGO_ENABLE;

	AdasGuiInitMsg();

	pthread_create(&gptAdasProcCtrl->tAdasGblCtrl.msg_proc_id, NULL, AdasGuiRcvMsgCallback, NULL);

	pthread_create(&gptAdasProcCtrl->tAdasGblCtrl.scale_proc_id, NULL, AdasDataScaleThread, NULL);

	pthread_create(&gptAdasProcCtrl->tAdasGblCtrl.adws_proc_id, NULL, AdasProcMainThread, NULL);	

#endif
}

void AdasProcUninit()
{
	gptAdasProcCtrl->tThreadStruct.loopActive_status = ADAS_ALGO_DISABLE;
}


