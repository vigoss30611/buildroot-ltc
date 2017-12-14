/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include "QMAPIType.h"
#include "QMAPIErrno.h"

#include <qsdk/videobox.h>
#include <qsdk/camera.h>

#include "tlmisc.h"
#include "q3_gpio.h"
#include "basic.h"
#include "libCamera.h"

extern int DMS_ov10630_DNCheck();
/* video input */
static int  g_tl_video_input_type = TL_Q3_SC2135;
static int  g_tl_video_input_num = 1;

static DEVICE_TYPE_E    g_enDeviceType = DEVICE_MEGA_IPCAM;


/*
if board_supprot_vi_num > 0, we will use board_supprot_vi_num to get g_tl_video_input_type
else we will use video_input_type directly.
*/
static int Camera_Set_Video_Input_Type(int video_input_type)
{
    g_tl_video_input_type = video_input_type;

    return 0;
}

static int Camera_Get_Video_Input_Type( void )
{
    return g_tl_video_input_type;
}


int Camera_Set_Video_Input_Num(int viChannelNum)
{
    g_tl_video_input_num =  viChannelNum;

    return 0;
}

int Camera_Get_Video_Input_Num( void )
{
    return g_tl_video_input_num;
}


int Camera_Vadc_SetColor(int nChannel, unsigned int cmd, unsigned int val)
{
	int nRes = -2;
	
	switch(cmd)
	{
		case QMAPI_COLOR_SET_BRIGHTNESS:
		{
			if(val>255)
				val = 255;
			if (val > 0)
				nRes = camera_set_brightness("isp", val);
			break;
		}
		case QMAPI_COLOR_SET_HUE:
		{
			if(val>255)
				val = 255;
			if (val > 0)
				nRes = camera_set_hue("isp", val);
			break;
		}
		case QMAPI_COLOR_SET_SATURATION:
		{
			if(val>255)
				val = 255;
			if (val > 0)
				nRes = camera_set_saturation("isp", val);
			break;
		}
		case QMAPI_COLOR_SET_CONTRAST:
		{
			if(val > 255)
				val = 255;
			if (val > 0)
				nRes = camera_set_contrast("isp", val);
			break;
		}
		case QMAPI_COLOR_SET_DEFINITION:
		{
			if(val > 255)
				val = 255;
			if (val > 0)
				nRes = camera_set_sharpness("isp", val);
			break;
		}
		case QMAPI_COLOR_SET_SCENE:
		case QMAPI_COLOR_SET_IRISBASIC:
		case QMAPI_COLOR_SET_ROTATE:
		case QMAPI_COLOR_SET_WD:
		case QMAPI_COLOR_SET_EWD:
		case QMAPI_COLOR_SET_BAW:
		case QMAPI_COLOR_SET_GC:
		break;
		case QMAPI_COLOR_SET_MIRROR: /* 0:²»¾µÏñ, 1: ÉÏÏÂ¾µÏñ, 2: ×óÓÒ¾µÏñ  3: ÉÏÏÂ×óÓÒ¾µÏñ */
		{
			if (val == 0)
				nRes = camera_set_mirror("isp", CAM_MIRROR_NONE);
			else if (val == 1)
				nRes = camera_set_mirror("isp", CAM_MIRROR_V);
			else if (val == 2)
				nRes = camera_set_mirror("isp", CAM_MIRROR_H);
			else if (val == 3)
				nRes = camera_set_mirror("isp", CAM_MIRROR_HV);
			break;
		}

        case QMAPI_COLOR_SET_ANTIFLICKER:
        {
            int ret = 0;
            int freq = 50;
            
            if (val == 0) {
                freq = 60;
            } else {
                freq = 50;
            }
            
            ret = camera_set_antiflicker("isp", freq);
            if (ret < 0) {
                printf("camera_set_antiflicker:%d failed\n", freq);
            }
            nRes = ret;
            printf("camera_set_antiflicker:%d\n", freq);

            break;
        }

        case QMAPI_COLOR_SET_RED:
        case QMAPI_COLOR_SET_BLUE:
        case QMAPI_COLOR_SET_GREEN:
        case QMAPI_COLOR_SET_GAMMA:
        default:
        printf("DMS_ov10630_SetColor not support cmd:%d\n", cmd);
			break;
	}
	printf("%s cmd:%x. val:%u.. nRes:%d. \n", __FUNCTION__, cmd, val, nRes);
	return nRes;
}


int Camera_Get_Default_Color(QMAPI_NET_CHANNEL_COLOR  *lpColor)
{
    int nRet = -1;
    switch(g_tl_video_input_type)
    {
		case TL_Q3_SC1135:          
            nRet = Get_Q3_SC1135_DefaultColor(lpColor);
        	break;
		case TL_Q3_SC2135:          
            nRet = Get_Q3_SC2135_DefaultColor(lpColor);
        	break;
		default:
        	break;
    }
	printf("GetVadcDefaultColor.. type:%d.. nRet:%d. \n", g_tl_video_input_type, nRet);
    return nRet;
}


int Camera_Get_Default_Enhanced_Color(QMAPI_NET_CHANNEL_ENHANCED_COLOR  *lpEnhancedColor)
{
    int nRet = -1;
    switch(g_tl_video_input_type)
    {
    	case TL_Q3_SC1135: 
		case TL_Q3_SC2135:
		default:
        	break;
    }
    return nRet;
}

int Camera_Get_Color_Support(QMAPI_NET_COLOR_SUPPORT  *lpColorSupport)
{
    int nRet = -1;

    switch(g_tl_video_input_type)
    {
        case TL_Q3_SC1135: 
		case TL_Q3_SC2135:
			nRet = Get_Q3_ColorSupport(lpColorSupport);
			break;
		default:
        	break;
    }

    return nRet;
}

int Camera_Get_Enhanced_Color_Support(QMAPI_NET_ENHANCED_COLOR_SUPPORT  *lpEnhancedColorSupport)
{
    int nRet = -1;
    memset(lpEnhancedColorSupport, 0x00, sizeof(QMAPI_NET_ENHANCED_COLOR_SUPPORT));
    lpEnhancedColorSupport->dwSize = sizeof(QMAPI_NET_ENHANCED_COLOR_SUPPORT);
    
    switch(g_tl_video_input_type)
    {
    	default:
	    	break;
    }

    switch( g_enDeviceType )
    {
	    case DEVICE_MEGA_D1:
    	case DEVICE_MEGA_IPCAM:
    	case DEVICE_MEGA_1080P:
    	case DEVICE_MEGA_300M:
    	case DEVICE_MEGA_960P:
    	case DEVICE_IPCAM_AUTO:
    	{
			// need fixme yi.zhang
    		//if(g_tl_video_input_type != TL_HI3516_LG1220 
    		//	&& g_tl_video_input_type != TL_SONY_6300
    		//	&& g_tl_video_input_type != TL_HITACHI_DISC110 
    		//	&& 	 g_tl_video_input_type != TL_SUNELL_CAMERA)
    		{
    			lpEnhancedColorSupport->dwMask |= QMAPI_COLOR_SET_IRISBASIC;
    			lpEnhancedColorSupport->dwMask |= QMAPI_COLOR_SET_AIRIS;		
    		}
    	}
    	break;
    	default:
    	break;
    }
    return nRet;
}


int Camera_Set_Video_Color(QMAPI_NET_CHANNEL_COLOR_SINGLE *lpstColorSet,  QMAPI_NET_CHANNEL_COLOR *lpColorParam, QMAPI_NET_CHANNEL_ENHANCED_COLOR *lpEnancedColor)
{
    //int nRes = 0;
    BOOL bSetFlag = 1;
 
	printf("%s  %d, 11111111111111111111,channel:%lu,cmd:%lu,value:%d\n",__FUNCTION__, __LINE__,lpstColorSet->dwChannel,lpstColorSet->dwSetFlag,lpstColorSet->nValue);
    switch(lpstColorSet->dwSetFlag)
    {
        case QMAPI_COLOR_SET_BRIGHTNESS:
        {
            lpColorParam->nBrightness = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_HUE:
        {
            lpColorParam->nHue = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_SATURATION:
        {
            lpColorParam->nSaturation = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_CONTRAST:
        {
            lpColorParam->nContrast = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_DEFINITION:
        {
            
            lpColorParam->nDefinition = lpstColorSet->nValue;
        }
        break;
        ///////////////////////////////////
        case  QMAPI_COLOR_SET_IRISBASIC:
        {
            lpEnancedColor->nIrisBasic = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_RED:
        {
            lpEnancedColor->nRed = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_BLUE:
        {
            lpEnancedColor->nBlue = lpstColorSet->nValue;
        }
        break;
        case QMAPI_COLOR_SET_GREEN:
        {
            lpEnancedColor->nGreen = lpstColorSet->nValue;
        }
        break;
        case QMAPI_COLOR_SET_GAMMA:
        {
            lpEnancedColor->nGamma = lpstColorSet->nValue;
        //  DMS_DEV_VENC_SetRc(lpstColorSet->dwChannel, QMAPI_MAIN_STREAM, lpstColorSet->nValue);
        //  return 0;
        }
        break;
        case QMAPI_COLOR_SET_ADNSWITCH:
        {
            lpEnancedColor->bEnableAutoDenoise = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_DN:
        {
            lpEnancedColor->nDenoise = lpstColorSet->nValue;
        }
        break;
        case QMAPI_COLOR_SET_AWBSWITCH:
        {
            lpEnancedColor->bEnableAWB = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_AECSWITCH:
        {
            lpEnancedColor->bEnableAEC = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_EC:
        {
            lpEnancedColor->nEC = lpstColorSet->nValue;
            if(0 == lpstColorSet->nValue)
            {
                lpEnancedColor->bEnableAEC = TRUE;
            }
            else
            {
                lpEnancedColor->bEnableAEC = FALSE;
            }
        }
        break;
        case  QMAPI_COLOR_SET_AGCSWITCH:
        {
            lpEnancedColor->bEnableAGC = lpstColorSet->nValue;

	     Camera_Vadc_SetColor(lpstColorSet->dwChannel, lpstColorSet->dwSetFlag, lpstColorSet->nValue);
	     lpstColorSet->dwSetFlag = QMAPI_COLOR_SET_GC;
	     lpstColorSet->nValue = lpEnancedColor->GC;
        }
        break;
        case QMAPI_COLOR_SET_GC:
        {
            lpEnancedColor->GC = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_MIRROR:
        {
            lpEnancedColor->nMirror = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_ROTATE:
		{
			lpEnancedColor->nRotate = lpstColorSet->nValue;
		}
		break;
        case QMAPI_COLOR_SET_BAW:
        {
            if(lpstColorSet->nValue<0 || lpstColorSet->nValue>2)
            {
                printf("%s  %d, Invalid bEnableBAW param:%d\n",__FUNCTION__, __LINE__,lpstColorSet->nValue);
                return 0;
            }
            lpEnancedColor->bEnableBAW = lpstColorSet->nValue;
            DMS_DEV_GPIO_SetIRCutState(lpEnancedColor->bEnableBAW);
           // g_tl_globel_param.TL_Channel_EnhancedColor[0].bEnableBAW = lpEnancedColor->bEnableBAW;
        }
        break;
        case QMAPI_COLOR_SET_EWD:
        {
            lpEnancedColor->bWideDynamic = lpstColorSet->nValue;
	     
        }
        break;
        case QMAPI_COLOR_SET_WD:
        {
            lpEnancedColor->nWDLevel = lpstColorSet->nValue;
        }
        break;
        case  QMAPI_COLOR_SET_SCENE:
        {
            lpEnancedColor->nSceneMode = lpstColorSet->nValue;
        }
        break;
        case QMAPI_COLOR_SET_AIRIS:
        {
            lpEnancedColor->bEnableAIris = lpstColorSet->nValue;
        }
        break;
        case QMAPI_COLOR_SET_BLC:
        {
            lpEnancedColor->bEnableBLC = lpstColorSet->nValue;
        }
        break;

        case QMAPI_COLOR_SET_ANTIFLICKER:
        {
            if(lpstColorSet->nValue<0 || lpstColorSet->nValue>1)
            {
                printf("%s Invalid antiflicker param:%d\n",__FUNCTION__, lpstColorSet->nValue);
                return 0;
            }
        }
        break;

        default:
            bSetFlag  = 0;
        break;
    }

	if(bSetFlag == 1)
	{
		Camera_Vadc_SetColor(lpstColorSet->dwChannel, lpstColorSet->dwSetFlag, lpstColorSet->nValue);
	}
    return 0;
}

