#include<stdio.h>
#include<time.h>
#include<string.h>
#include<sys/time.h>

#include<minigui/common.h>
#include<minigui/minigui.h>
#include<minigui/gdi.h>
#include<minigui/window.h>
#include<minigui/control.h>

#include "config_manager.h"
#include "spv_language_res.h"
#include "spv_item.h"
#include "spv_common.h"
#include "spv_static.h"
#include "spv_utils.h"
#include "spv_debug.h"
#include "gui_icon.h"
#include "gui_common.h"


#define TIMER_DATE 201
#define TIMER_TIME 202
#define TIMER_PLATE 203

static int DIALOG_BIG_WIDTH;
static int DIALOG_BIG_HEIGHT;

static int DIALOG_NORMAL_WIDTH;
static int DIALOG_NORMAL_HEIGHT;

static int DIALOG_SMALL_WIDTH;
static int DIALOG_SMALL_HEIGHT;

static PLOGFONT g_logfont, g_logfont_bold;
static PLOGFONT g_content_font;
static BITMAP s_ExitIcon, icon;

typedef struct{
	int min;
	int hour;
	int meridiem;
	int sel;

    RECT scttrect[3];
    RECT cttrect;
    RECT trect;
}InfotmTime;

typedef InfotmTime* PInfotmTime;

typedef struct{
	int day;
	int month;
	int year;
	int sel;

    RECT scttrect[3];
    RECT cttrect;
    RECT trect;
}InfotmDate;

typedef InfotmDate* PInfotmDate;

typedef enum
{
	INFO_DATETIME_NONE,
	INFO_DATETIME_MON,
	INFO_DATETIME_DAY,
	INFO_DATETIME_YEAR,
	INFO_DATETIME_HOUR,
	INFO_DATETIME_MIN,
	INFO_DATETIME_MERIDIEM,
} INFO_DATETIME_TYPE;

typedef struct {
	int month;
	int day;
	int year;
	int hour;
	int min;
	int meridiem;
	INFO_DATETIME_TYPE sel;

	RECT scttrect[6];
}InfotmDateTime;

typedef InfotmDateTime* PInfotmDateTime;

typedef struct{
    
    int plateNo[7];    
    int sel;

    RECT scttrect[7];
    RECT cttrect;
    RECT trect;
    
}InfotmPlate;

typedef InfotmPlate* PInfotmPlate;


typedef struct {
    int type;
    DWORD infoData;
} DialogInfo;

typedef DialogInfo * PDialogInfo;



static CTRLDATA AdvInfoSettingCtrl[] = 
{

};

static void setDate(PInfotmDate pDate){

	PInfotmDate ptempDate = (PInfotmDate) pDate;
	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

    timeinfo->tm_year = ptempDate->year+100;
    timeinfo->tm_mon = ptempDate->month-1;
    timeinfo->tm_mday = ptempDate->day;
	
	rawtime = mktime(timeinfo);

    struct timeval tv;
    struct timezone tz;
    
    gettimeofday(&tv, &tz);

    tv.tv_sec = rawtime;
    tv.tv_usec = 0;

    //int result = SetTimeToSystem(tv, tz);
}

static void setTime(PInfotmTime pTime){

	PInfotmTime ptempTime = (PInfotmTime) pTime;
	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
    if(ptempTime->meridiem == 0)
        timeinfo->tm_hour = ptempTime->hour;
    else
        timeinfo->tm_hour = ptempTime->hour + 12;
        timeinfo->tm_min = ptempTime->min;
   	
	rawtime = mktime(timeinfo);

    struct timeval tv;
    struct timezone tz;
    
    gettimeofday(&tv, &tz);
    
    tv.tv_sec = rawtime;
    tv.tv_usec = 0;

    //int result = SetTimeToSystem(tv, tz);
}

static int setDateTime(PInfotmDateTime pDateTime)
{
	PInfotmDateTime ptempDateTime = (PInfotmDateTime) pDateTime;
	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);

    timeinfo->tm_year = ptempDateTime->year+100;
    timeinfo->tm_mon = ptempDateTime->month-1;
    timeinfo->tm_mday = ptempDateTime->day;
    if(ptempDateTime->meridiem == 0)
        timeinfo->tm_hour = ptempDateTime->hour;
    else
        timeinfo->tm_hour = ptempDateTime->hour + 12;
        timeinfo->tm_min = ptempDateTime->min;

	rawtime = mktime(timeinfo);

    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    tv.tv_sec = rawtime;
    tv.tv_usec = 0;
    return SetTimeToSystem(tv, tz);
}

static void setPlate(PInfotmPlate pPlate){
    
    PInfotmPlate ptempPlate = (PInfotmPlate)pPlate;
    char value[VALUE_SIZE] = {0};
    char prvStr[] = {"川鄂甘赣桂贵黑沪吉冀津晋京辽鲁蒙闽宁青琼陕苏皖湘新渝豫粤云藏浙"};
    char areaStr[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    char noStr[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    int i = 0;
    char stringV[10];

    while(i<7){
        
        if(0 == i){
            int chNo = (ptempPlate->plateNo[i]) * 3;
            sprintf(stringV,"%c%c%c", prvStr[chNo], prvStr[chNo+1], prvStr[chNo+2]);
        }
        else if( 1 == i){
            sprintf(stringV,"%c", areaStr[ptempPlate->plateNo[i]]);
        }
        else{
            sprintf(stringV,"%c", noStr[ptempPlate->plateNo[i]]);
        }
        
        strcat(value, stringV);
        i++;

    }

    //SetConfigValue(GETKEY(ID_SETUP_PLATE_NUMBER),value, FROM_USER);

}

static void SetFocusColor(HDC hdc, BOOL isFocus)
{
	DWORD text_pixel;
	if(isFocus) {
		text_pixel = RGBA2Pixel(hdc, 0x00, 0x00, 0x00, 0xFF);
		SetBkMode (hdc, BM_OPAQUE);
		SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
		SetTextColor(hdc, text_pixel);
	} else {
		text_pixel = RGBA2Pixel(hdc, 0xFF, 0xFF, 0xFF, 0xFF);
		SetBkMode (hdc, BM_TRANSPARENT);
		SetBkColor(hdc, RGB2Pixel(hdc, 0xFF, 0xFF, 0xFF));
		SetTextColor(hdc, text_pixel);
	}
}

static void EndAdvInfoSettingDialog(HWND hDlg, DWORD endCode)
{
    INFO_DIALOG_TYPE type = ((PDialogInfo)GetWindowAdditionalData(hDlg))->type;
	if(INFO_DIALOG_SET_DATE == type)
		    setDate((PInfotmDate)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData));
	else if(INFO_DIALOG_SET_TIME == type)
		    setTime((PInfotmTime)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData));
    else if(INFO_DIALOG_SET_PLATE == type)
            setPlate((PInfotmPlate)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData));
	else if(INFO_DIALOG_SET_DATETIME == type) {
			if(setDateTime((PInfotmDateTime)(((PDialogInfo)GetWindowAdditionalData(hDlg))->infoData)))
				INFO("set data time error\n");
	}

    UnloadBitmap(&s_ExitIcon);
    UnloadBitmap(&icon);
    DestroyLogFont(g_logfont_bold);
    DestroyLogFont(g_logfont);
    DestroyLogFont(g_content_font);
    EndDialog(hDlg, endCode);
}

static void getTime(PInfotmTime pTime){	

	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
    PInfotmTime ptempTime = pTime; 
    if(timeinfo->tm_hour > 11){
        ptempTime->hour = timeinfo->tm_hour -12;
        ptempTime->meridiem = 1;
    }
    else{
        ptempTime->hour = timeinfo->tm_hour;
        ptempTime->meridiem = 0;
    }
    ptempTime->min = timeinfo->tm_min;

}

static void getDate(PInfotmDate pDate){
	
	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	PInfotmDate ptempDate = pDate;
	ptempDate->day = timeinfo->tm_mday;
    ptempDate->month = 1+timeinfo->tm_mon;
    ptempDate->year = 1900+timeinfo->tm_year - 2000;
    if(ptempDate->year < 15) {
        ptempDate->year = 15;
    } else if(ptempDate->year > 30) {
        ptempDate->year = 30;
    }

}

static void getDateTime(PInfotmDateTime pDateTime)
{
	static time_t rawtime;
	static struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	PInfotmDateTime ptempDateTime = pDateTime;

    if(timeinfo->tm_hour > 11){
        ptempDateTime->hour = timeinfo->tm_hour -12;
        ptempDateTime->meridiem = 1;
    }
    else{
        ptempDateTime->hour = timeinfo->tm_hour;
        ptempDateTime->meridiem = 0;
    }
    ptempDateTime->min = timeinfo->tm_min;
	ptempDateTime->day = timeinfo->tm_mday;
    ptempDateTime->month = 1+timeinfo->tm_mon;
    ptempDateTime->year = 1900+timeinfo->tm_year - 2000;
    if(ptempDateTime->year < 15) {
        ptempDateTime->year = 15;
    } else if(ptempDateTime->year > 30) {
        ptempDateTime->year = 30;
    }
}

static void getPlate(PInfotmPlate pPlate){
    char value[VALUE_SIZE] = {0};    
    char prvStr[] = {"川鄂甘赣桂贵黑沪吉冀津晋京辽鲁蒙闽宁青琼陕苏皖湘新渝豫粤云藏浙"};
    char areaStr[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    char noStr[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    //GetConfigValue(GETKEY(ID_SETUP_PLATE_NUMBER), value);
    int i = 0;
    char StringV1[10];
    char StringV2[10];
    while(i< 7){
        if(i == 0){
            sprintf(StringV2, "%c%c%c", value[0],value[1],value[2]);
            int y = 0;
            while( y < sizeof(prvStr))
            {

                sprintf(StringV1, "%c%c%c", prvStr[y],prvStr[y+1],prvStr[y+2]);
                if(strcmp(StringV1, StringV2)== 0){
                    pPlate->plateNo[i] = y/3;
                    break;
                }
                y = y+3;
            } 
        }
        else if(i == 1){
            int y = 0;
            while(y < sizeof(areaStr))
            {
                if(areaStr[y] == value[i+2]){
                    pPlate->plateNo[i] = y;
                    break;
                }
                y++;
            }
        }else{
            int y = 0;
            while(y < sizeof(noStr))
            {
                if(noStr[y] == value[i+2]){
                    pPlate->plateNo[i] = y;
                    break;
                }
                y++;
            }
        }

       i++;
    }

}
static void ChangeValue(HWND hDlg, PDialogInfo tempinfo){
    INFO_DIALOG_TYPE type = tempinfo->type;

    if(INFO_DIALOG_SET_DATE == type){
        PInfotmDate pdate =(PInfotmDate)tempinfo->infoData;
        if(1 == pdate->sel){
            pdate->month = pdate->month % 12 + 1;
        }
        else if(2 == pdate->sel){
            if(pdate->month == 2){
                if((pdate->year)%4 == 0){
                    pdate->day = pdate->day % 29 + 1;
                }
                else
                    pdate->day = pdate->day % 28 + 1;
            }
            else if(pdate->month == 4 || pdate->month == 6 || pdate->month == 9 || pdate->month == 11){
                pdate->day = pdate->day % 30 + 1;
            }
            else{
                    pdate->day = pdate->day % 31 + 1;
            }
        }
        else if(3 == pdate->sel){
            (pdate->year)++;
            if(pdate->year > 30)
                pdate->year = 15;
            if( ((pdate->year)%4 != 0) && pdate->month == 2 && pdate->day == 29 ){
                pdate->day = 28;
                InvalidateRect(hDlg, &(pdate->scttrect[pdate->sel-2]), TRUE);
            }
        }
        if((0 != pdate->sel) && !IsTimerInstalled(hDlg, TIMER_DATE))
	        SetTimer(hDlg, TIMER_DATE, 50);
        
        InvalidateRect(hDlg, &(pdate->scttrect[pdate->sel-1]), TRUE);

    }
    else if (INFO_DIALOG_SET_TIME == type){
	    PInfotmTime ptime = (PInfotmTime)tempinfo->infoData;
        if(1 == ptime->sel){
            (ptime->hour)++;
            ptime->hour = ptime->hour % 12;
        }
        else if(2 == ptime->sel){
            (ptime->min)++;
            ptime->min = ptime->min % 60;
        }
        else if(3 == ptime->sel){
            ptime->meridiem = (ptime->meridiem + 1)%2;
        }
        if((0 != ptime->sel) && !IsTimerInstalled(hDlg, TIMER_TIME))
	        SetTimer(hDlg, TIMER_TIME, 50);
        
        InvalidateRect(hDlg, &(ptime->scttrect[ptime->sel-1]), TRUE);
    }
    else if(INFO_DIALOG_SET_PLATE == type){
	    PInfotmPlate pplate = (PInfotmPlate)tempinfo->infoData;
        (pplate->plateNo[pplate->sel-1])++;
        if( 1 == pplate->sel){
            pplate->plateNo[pplate->sel-1] = pplate->plateNo[pplate->sel-1]%31;
            
        }
        else if( 2 == pplate->sel){
            pplate->plateNo[pplate->sel-1] = pplate->plateNo[pplate->sel-1]%26;

        }
        else{
            pplate->plateNo[pplate->sel-1] = pplate->plateNo[pplate->sel-1]%36;
        }

        if((0 != pplate->sel) && !IsTimerInstalled(hDlg, TIMER_PLATE))
	        SetTimer(hDlg, TIMER_PLATE, 50);
        InvalidateRect(hDlg, &(pplate->scttrect[pplate->sel-1]), TRUE);
       // InvalidateRect(hDlg, &(pplate->cttrect), TRUE);

    } else if(INFO_DIALOG_SET_DATETIME == type) {
	    PInfotmDateTime pdateTime = (PInfotmDateTime)tempinfo->infoData;
		switch (pdateTime->sel) {
			case INFO_DATETIME_MON:
				pdateTime->month = pdateTime->month % 12 + 1;
				break;
			case INFO_DATETIME_DAY:
				if(pdateTime->month == 2){
				    if((pdateTime->year)%4 == 0){
				        pdateTime->day = pdateTime->day % 29 + 1;
				    }
				    else
				        pdateTime->day = pdateTime->day % 28 + 1;
				}
				else if(pdateTime->month == 4 || pdateTime->month == 6 || pdateTime->month == 9 || pdateTime->month == 11){
				    pdateTime->day = pdateTime->day % 30 + 1;
				}
				else{
				        pdateTime->day = pdateTime->day % 31 + 1;
				}
				break;
			case INFO_DATETIME_YEAR:
				(pdateTime->year)++;
				if(pdateTime->year > 30)
				    pdateTime->year = 15;
				if( ((pdateTime->year)%4 != 0) && pdateTime->month == 2 && pdateTime->day == 29 ){
				    pdateTime->day = 28;
				    InvalidateRect(hDlg, &(pdateTime->scttrect[pdateTime->sel-2]), TRUE);
				}
				break;
			case INFO_DATETIME_HOUR:
				(pdateTime->hour)++;
				pdateTime->hour = pdateTime->hour % 12;
				break;
			case INFO_DATETIME_MIN:
				(pdateTime->min)++;
				pdateTime->min = pdateTime->min % 60;
				break;
			case INFO_DATETIME_MERIDIEM:
				pdateTime->meridiem = (pdateTime->meridiem + 1)%2;
				break;
			default:
				break;
		}
        if((0 != pdateTime->sel) && !IsTimerInstalled(hDlg, TIMER_DATETIME))
	        SetTimer(hDlg, TIMER_DATETIME, 50);
        UpdateWindow(hDlg, TRUE);
	}
    else
        ERROR("THis mode does not exist\n");

}

static void goNext(HWND hDlg, PDialogInfo tempinfo){
	INFO_DIALOG_TYPE type = tempinfo->type;
	if (type == INFO_DIALOG_SET_TIME){
    	PInfotmTime ptime = (PInfotmTime)tempinfo->infoData;
		ptime->sel = (ptime->sel+1)%4;
        if(0 == ptime->sel || 1 == ptime->sel){
            InvalidateRect(hDlg, &(ptime->cttrect), TRUE);
            InvalidateRect(hDlg, &(ptime->trect), TRUE); 
        }
        else{
            InvalidateRect(hDlg, &(ptime->cttrect), TRUE);
        }
	}
	else if(type == INFO_DIALOG_SET_DATE){
		PInfotmDate pdate = (PInfotmDate)tempinfo->infoData;
        pdate->sel = (pdate->sel+1)%4;
        if(0 == pdate->sel || 1 == pdate->sel){
            InvalidateRect(hDlg, &(pdate->cttrect), TRUE);
            InvalidateRect(hDlg, &(pdate->trect), TRUE); 
        }
        else{
            InvalidateRect(hDlg, &(pdate->cttrect), TRUE);
        }
	} else if(INFO_DIALOG_SET_DATETIME) {
		PInfotmDateTime pdateTime = (PInfotmDateTime)tempinfo->infoData;
        pdateTime->sel = (pdateTime->sel+1)%7;
		if(pdateTime->sel == 0)
			pdateTime->sel = 1;
	}
    else if(type == INFO_DIALOG_SET_PLATE){
		PInfotmPlate pplate = (PInfotmPlate)tempinfo->infoData;
        pplate->sel = (pplate->sel+1)%8;
        if(0 == pplate->sel || 1 == pplate->sel){
            InvalidateRect(hDlg, &(pplate->cttrect), TRUE);
            InvalidateRect(hDlg, &(pplate->trect), TRUE); 
        }
        else{
            InvalidateRect(hDlg, &(pplate->cttrect), TRUE);
        }
        
    }
	else{
		ERROR("error, in goNext, this type does not exist!\n");
    }

}

static void goPre(HWND hDlg, PDialogInfo tempinfo){
	INFO_DIALOG_TYPE type = tempinfo->type;
	if (type == INFO_DIALOG_SET_TIME){
    	PInfotmTime ptime = (PInfotmTime)tempinfo->infoData;
		ptime->sel = (ptime->sel+3)%4;
        if(3 == ptime->sel || 0 == ptime->sel){
            InvalidateRect(hDlg, &(ptime->cttrect), TRUE);
            InvalidateRect(hDlg, &(ptime->trect), TRUE); 
        }
        else{
            InvalidateRect(hDlg, &(ptime->cttrect), TRUE);
        }
	}
	else if(type == INFO_DIALOG_SET_DATE){
		PInfotmDate pdate = (PInfotmDate)tempinfo->infoData;
		pdate->sel = (pdate->sel+3)%4;
        if(3 == pdate->sel || 0 == pdate->sel){
            InvalidateRect(hDlg, &(pdate->cttrect), TRUE);
            InvalidateRect(hDlg, &(pdate->trect), TRUE); 
        }
        else{
            InvalidateRect(hDlg, &(pdate->cttrect), TRUE);
        }
	} else if(INFO_DIALOG_SET_DATETIME) {
		PInfotmDateTime pdateTime = (PInfotmDateTime)tempinfo->infoData;
        pdateTime->sel = (pdateTime->sel+6)%7;
		if(pdateTime->sel == 0)
			pdateTime->sel = 6;
	}
    else if(type == INFO_DIALOG_SET_PLATE){
		PInfotmPlate pplate = (PInfotmPlate)tempinfo->infoData;
		pplate->sel = (pplate->sel+7)%8;
        if(7 == pplate->sel || 0 == pplate->sel){
            InvalidateRect(hDlg, &(pplate->cttrect), TRUE);
            InvalidateRect(hDlg, &(pplate->trect), TRUE); 
        }
        else{
            InvalidateRect(hDlg, &(pplate->cttrect), TRUE);
        }
    }
	else{
		ERROR("error, in goPre, this type does not exist!\n");
	}	
}

static void DrawPlateContent(HWND hDlg, HDC hdc, PDialogInfo tempInfo){
    DWORD brushColor;
    brushColor = GetBrushColor(hdc);
    PInfotmPlate pplate;
    char prvStr[] = {"川鄂甘赣桂贵黑沪吉冀津晋京辽鲁蒙闽宁青琼陕苏皖湘新渝豫粤云藏浙"};
    char areaStr[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    char noStr[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

    int sel;

    int itemHeight = (LARGE_SCREEN ? 2.0f : 1.5f) * FOLDER_DIALOG_TITLE_HEIGHT;

    int itemTop = (VIEWPORT_HEIGHT + FOLDER_DIALOG_TITLE_HEIGHT - itemHeight) / 2;
    int itemBottom = itemTop + itemHeight;

	pplate = (PInfotmPlate)tempInfo->infoData;
    sel = pplate->sel;
    
    //draw content divider	
    int totalDvr = 7;
    int dvrWidth = (VIEWPORT_WIDTH*2)/19;
    int dvrHeight = LARGE_SCREEN ? 5 : 3;
    int hzDist2 = VIEWPORT_WIDTH/50; //dist from edge to the nearest divider
    int hzDist = (VIEWPORT_WIDTH - dvrWidth*totalDvr - hzDist2*2)/(totalDvr - 1);//VIEWPORT_WIDTH/30; //dist between each divider
    int hzTDist = hzDist + dvrWidth;
    int x = 0;

    while(x<totalDvr){
        
        if((x+1) == sel)
	        SetBrushColor(hdc, PIXEL_red);
        else	
	        SetBrushColor(hdc, PIXEL_lightwhite);

        FillBox(hdc, hzDist2 + x*hzTDist, itemTop, dvrWidth, dvrHeight);
        FillBox(hdc, hzDist2 + x*hzTDist, itemBottom - dvrHeight, dvrWidth, dvrHeight);

        x++;
    }

    SetBrushColor(hdc, PIXEL_lightwhite);

    //draw content
    RECT cttRect;
    cttRect.top = itemTop;
    cttRect.bottom = itemBottom;
    SelectFont(hdc, g_content_font);
    char stringV[10];

    int y = 0;
    
    while(y<totalDvr){
        cttRect.left = hzDist2 + y*hzTDist;
        cttRect.right = cttRect.left + dvrWidth;
        
        if(0 == y){
            int chNo = (pplate->plateNo[y]) * 3;
            sprintf(stringV,"%c%c%c", prvStr[chNo], prvStr[chNo+1], prvStr[chNo+2]);
        }
        else if( 1 == y){
            sprintf(stringV,"%c", areaStr[pplate->plateNo[y]]);
        }
        else{
            sprintf(stringV,"%c", noStr[pplate->plateNo[y]]);
        }

        cttRect.left = cttRect.left - hzTDist;
        cttRect.right = cttRect.right + hzTDist;
        if(RECTWP(&(pplate->scttrect[y])) < 1){
            pplate->scttrect[y].left = cttRect.left * SCALE_FACTOR;
            pplate->scttrect[y].right = cttRect.right * SCALE_FACTOR;
            pplate->scttrect[y].top = cttRect.top * SCALE_FACTOR;
            pplate->scttrect[y].bottom = cttRect.bottom * SCALE_FACTOR;
        }

        DrawText(hdc, stringV, -1, &cttRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_SINGLELINE);
        
        y++;

    }
    if(RECTWP(&(pplate->cttrect)) < 1){ 
        pplate->cttrect.left = 0;//x * SCALE_FACTOR;
        pplate->cttrect.right = VIEWPORT_WIDTH * SCALE_FACTOR; 
        pplate->cttrect.top = itemTop * SCALE_FACTOR;
        pplate->cttrect.bottom = itemBottom * SCALE_FACTOR;
    }
    SetBrushColor(hdc, brushColor);
}

static void DrawDateTimeAllContent(HWND hDlg, HDC hdc, PDialogInfo tempInfo){
    PInfotmDate pdate;
    PInfotmTime ptime;
	PInfotmDateTime pdateTime = (PInfotmDateTime)tempInfo->infoData;
    DWORD oldBrushColor, oldTextColor;

    int temptype = tempInfo->type;
    int sel = pdateTime->sel;
    int totalDvr = 11;
    int x = 0;
    char stringV[3];

	SIZE DateTextSize, TextSize;
    RECT scttRect;
	RECT DateTitleRect, TimeTitleRect;

    oldBrushColor = GetBrushColor(hdc);
    oldTextColor = GetTextColor(hdc);
    SelectFont(hdc, g_content_font);
    SetBrushColor(hdc, PIXEL_black);
    FillBox(hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	SetFocusColor(hdc, FALSE);
    GetTextExtent(hdc, GETSTRING(STRING_DATE_SETTING), -1, &DateTextSize);

    int itemHeight = DateTextSize.cy;
	int itemInternal = 4;
    int itemTop = (VIEWPORT_HEIGHT - itemHeight*2 - itemInternal) >> 1;
    int itemBottom = itemTop + itemHeight;
	int itemPadding = 1;

	DateTitleRect.top = itemTop;
	DateTitleRect.bottom = itemBottom;
	DateTitleRect.left = 0;
	DateTitleRect.right = DateTitleRect.left + DateTextSize.cx;

	TimeTitleRect.top = DateTitleRect.bottom + itemInternal;
	TimeTitleRect.bottom = TimeTitleRect.top + itemHeight;
	TimeTitleRect.left = 0;
	TimeTitleRect.right = DateTitleRect.left + DateTextSize.cx;

    scttRect.top = DateTitleRect.top;
    scttRect.bottom = DateTitleRect.bottom;
	scttRect.left = DateTitleRect.left;
	scttRect.right = DateTitleRect.right;

	DrawText(hdc, GETSTRING(STRING_DATE_SETTING), -1, &DateTitleRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	DrawText(hdc, GETSTRING(STRING_TIME_SETTING), -1, &TimeTitleRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    GetTextExtent(hdc, "AM", -1, &DateTextSize);
    GetTextExtent(hdc, "/", -1, &TextSize);

    while(x < totalDvr) {
        if(x%2 == 0) {
			switch (x/2 + 1) {
				case INFO_DATETIME_MON:
                    sprintf(stringV, "%02d", pdateTime->month);
					break;
				case INFO_DATETIME_DAY:
                    sprintf(stringV, "%02d", pdateTime->day);
					break;
				case INFO_DATETIME_YEAR:
					sprintf(stringV, "%02d", pdateTime->year);
					break;
				case INFO_DATETIME_HOUR:
					sprintf(stringV, "%02d", pdateTime->hour);
					break;
				case INFO_DATETIME_MIN:
					sprintf(stringV, "%02d", pdateTime->min);
					break;
				case INFO_DATETIME_MERIDIEM:
					if(0 == pdateTime->meridiem)
					    sprintf(stringV, "AM");
					else
					    sprintf(stringV, "PM");
					break;
				default:
					break;
			}

			if(x/2 + 1 == INFO_DATETIME_HOUR) {
				scttRect.left = TimeTitleRect.right + itemPadding;
				scttRect.right = scttRect.left + DateTextSize.cx;
				scttRect.top = TimeTitleRect.top;
				scttRect.bottom = TimeTitleRect.bottom;
			} else {
				scttRect.left = scttRect.right + itemPadding;
				scttRect.right = scttRect.left + DateTextSize.cx;
			}
            if(RECTWP(&(pdateTime->scttrect[x/2])) < 1){
                pdateTime->scttrect[x/2].left = scttRect.left * SCALE_FACTOR;
                pdateTime->scttrect[x/2].right = scttRect.right * SCALE_FACTOR;
                pdateTime->scttrect[x/2].top = scttRect.top * SCALE_FACTOR;
                pdateTime->scttrect[x/2].bottom = scttRect.bottom * SCALE_FACTOR;
            }
			if(x/2 + 1 == sel) {
				SetFocusColor(hdc, TRUE);
			} else {
				SetFocusColor(hdc, FALSE);
			}
            DrawText(hdc, stringV, -1, &(pdateTime->scttrect[x/2]), DT_VCENTER | DT_CENTER | DT_SINGLELINE);
        } else {
            scttRect.left = scttRect.right + itemPadding;
            scttRect.right = scttRect.left + TextSize.cx;
			SetFocusColor(hdc, FALSE);
			if(x/2 + 1 < INFO_DATETIME_HOUR) {
				if(x/2 + 1 != INFO_DATETIME_YEAR)
				DrawText(hdc, "/", -1, &scttRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			} else {
				if((x/2 + 1 != INFO_DATETIME_MIN) && (x/2 + 1 != INFO_DATETIME_MERIDIEM))
				DrawText(hdc, ":", -1, &scttRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			}
        }
        x++;
    }
    SetBrushColor(hdc, oldBrushColor);
    SetTextColor(hdc, oldTextColor);
}

static void DrawDateTimeContent(HWND hDlg, HDC hdc, PDialogInfo tempInfo){
    DWORD brushColor;
    brushColor = GetBrushColor(hdc);
    int temptype = tempInfo->type;
    PInfotmDate pdate;
    PInfotmTime ptime;
    int sel;

    int itemHeight = (LARGE_SCREEN ? 2.0f : 1.5f) * FOLDER_DIALOG_TITLE_HEIGHT;

    int itemTop = (VIEWPORT_HEIGHT + FOLDER_DIALOG_TITLE_HEIGHT - itemHeight) / 2;
    int itemBottom = itemTop + itemHeight;

    if(INFO_DIALOG_SET_DATE == temptype){
	    pdate = (PInfotmDate)tempInfo->infoData;
    	sel = pdate->sel;
    }
    else if(INFO_DIALOG_SET_TIME == temptype){
	    ptime = (PInfotmTime)tempInfo->infoData;
	    sel = ptime->sel;
	}
    //draw content divider
    int realDvr = 3;
    int totalDvr = 5;	
    int dvrWidth = (VIEWPORT_WIDTH*2)/9;
    int dvrHeight = LARGE_SCREEN ? 5 : 3;
    int hzDist2 = VIEWPORT_WIDTH/18; //dist from edge to the nearest divider
    int hzDist = (VIEWPORT_WIDTH - dvrWidth*realDvr - hzDist2*2) / (realDvr - 1); // dist between each divider
    int hzTDist = hzDist + dvrWidth;
    int x = 0;
    char stringV[3];

    while(x < realDvr){
        if((x+1) == sel)
	        SetBrushColor(hdc, PIXEL_red);
        else
            SetBrushColor(hdc, PIXEL_lightwhite);
        FillBox(hdc, hzDist2 + x*hzTDist, itemTop, dvrWidth, dvrHeight);
        FillBox(hdc, hzDist2 + x*hzTDist, itemBottom - dvrHeight, dvrWidth, dvrHeight);
        
        x++;
    }

    SetBrushColor(hdc, PIXEL_lightwhite);
    RECT scttRect;
    scttRect.top = itemTop;
    scttRect.bottom = itemBottom;
    SelectFont(hdc, g_content_font);
    SetTextColor(hdc, PIXEL_lightwhite);
    x = 0;

    if(INFO_DIALOG_SET_DATE == temptype){
        while(x < totalDvr){
            if(x%2 == 0){
                scttRect.left = hzDist2 + (x/2)*hzTDist; 
                scttRect.right = scttRect.left + dvrWidth;
                if(x/2 == 0){
                    if(pdate->month > 9)
                        sprintf(stringV, "%d", pdate->month);
                    else
                        sprintf(stringV, "0%d", pdate->month);
                
                }else if (x/2 ==1){
                    if(pdate->day > 9)
                        sprintf(stringV, "%d", pdate->day);
                    else
                        sprintf(stringV, "0%d", pdate->day);
                }else if(x /2 == 2){
                    sprintf(stringV, "%d", pdate->year);
                }
                if(RECTWP(&(pdate->scttrect[x/2])) < 1){
            
                    pdate->scttrect[x/2].left = scttRect.left * SCALE_FACTOR;
                    pdate->scttrect[x/2].right = scttRect.right * SCALE_FACTOR;
                    pdate->scttrect[x/2].top = scttRect.top * SCALE_FACTOR;
                    pdate->scttrect[x/2].bottom = scttRect.bottom * SCALE_FACTOR;
            
                }
                DrawText(hdc, stringV, -1, &scttRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
            }
            else{
                scttRect.left = scttRect.right;
                scttRect.right = scttRect.left + hzDist;

                DrawText(hdc, "/", -1, &scttRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
            }
    
            x++; 
        }

        if(RECTWP(&(pdate->cttrect)) < 1){ 
            pdate->cttrect.left = hzDist2 * SCALE_FACTOR;
            pdate->cttrect.right = scttRect.right * SCALE_FACTOR;
            pdate->cttrect.top = itemTop * SCALE_FACTOR;
            pdate->cttrect.bottom = itemBottom * SCALE_FACTOR;
        }
    }else if(INFO_DIALOG_SET_TIME == temptype){
        while(x < totalDvr){
            if(x%2 == 0){
                scttRect.left = hzDist2 + (x/2)*hzTDist; 
                scttRect.right = scttRect.left + dvrWidth;
                if(x/2 == 0){
                    if(ptime->hour > 9)
                        sprintf(stringV, "%d", ptime->hour);
                    else
                        sprintf(stringV, "0%d", ptime->hour);
                
                }else if (x/2 ==1){
                    if(ptime->min > 9)
                        sprintf(stringV, "%d", ptime->min);
                    else
                        sprintf(stringV, "0%d", ptime->min);
                }else if(x /2 == 2){
                    if(0 == ptime->meridiem)
                        sprintf(stringV, "AM");
                    else
                        sprintf(stringV, "PM");
                }
                if(RECTWP(&(ptime->scttrect[x/2])) < 1){
            
                    ptime->scttrect[x/2].left = scttRect.left * SCALE_FACTOR;
                    ptime->scttrect[x/2].right = scttRect.right * SCALE_FACTOR;
                    ptime->scttrect[x/2].top = scttRect.top * SCALE_FACTOR;
                    ptime->scttrect[x/2].bottom = scttRect.bottom * SCALE_FACTOR;
            
                }
                DrawText(hdc, stringV, -1, &scttRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
            }
            else{
                if(x/2 == 0){
                    scttRect.left = scttRect.right;
                    scttRect.right = scttRect.left + hzDist;

                    DrawText(hdc, ":", -1, &scttRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
                }
            }
    
            x++; 
        }

        if(RECTWP(&(ptime->cttrect)) < 1){ 
            ptime->cttrect.left = hzDist2 * SCALE_FACTOR;
            ptime->cttrect.right = scttRect.right * SCALE_FACTOR;
            ptime->cttrect.top = itemTop * SCALE_FACTOR;
            ptime->cttrect.bottom = itemBottom * SCALE_FACTOR;
        }

    }
 
   SetBrushColor(hdc, brushColor);
}

static void DrawAdvInfoSettingDialog(HWND hDlg, HDC hdc,  PDialogInfo tempInfo)
{
    int temptype = tempInfo->type;
    const char *pExitText;
    RECT headerRect, exitRect;
    headerRect.left = 0;
    headerRect.right = VIEWPORT_WIDTH;
    headerRect.top = 0;
    headerRect.bottom = FOLDER_DIALOG_TITLE_HEIGHT;	
    SIZE size;
    int iconId = IC_DATE_TIME;
    int x, y, w, h;
    int exitW = 30, exitH = 30;
    DWORD oldBrushColor = GetBrushColor(hdc);
    //brush black bg
    SetBrushColor(hdc,RGB2Pixel(hdc,0x33,0x33,0X33));
    FillBox(hdc, 0, 0, headerRect.right, headerRect.bottom);

    LoadBitmap(HDC_SCREEN, &icon, SPV_ICON[iconId]);
    icon.bmType = BMP_TYPE_COLORKEY;
    icon.bmColorKey = PIXEL_black;

    x = 0;
    y = headerRect.top + ((RECTHP(&headerRect) - icon.bmHeight) >> 1);
    //y = ((FOLDER_DIALOG_TITLE_HEIGHT - icon.bmHeight) >> 1);
    FillBoxWithBitmap(hdc, x, y, icon.bmWidth, icon.bmHeight, &icon);

    SetBkMode (hdc, BM_TRANSPARENT);

    //draw title text
    SetTextColor(hdc, PIXEL_lightwhite);
    SelectFont(hdc, g_logfont_bold);

    headerRect.left = x + icon.bmWidth;
    if(INFO_DIALOG_SET_DATE == temptype){
        DrawText(hdc, GETSTRING(STRING_DATE_SETTING), -1, &headerRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    }
    else if (INFO_DIALOG_SET_TIME == temptype){
	
        DrawText(hdc, GETSTRING(STRING_TIME_SETTING), -1, &headerRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    }else if(INFO_DIALOG_SET_PLATE == temptype){
        //DrawText(hdc, GETSTRING(STRING_PLATE_NUMBER), -1, &headerRect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
    }

    //draw divider
    SetBrushColor(hdc, PIXEL_red);
    FillBox(hdc, headerRect.right - FOLDER_DIALOG_TITLE_HEIGHT - 2, headerRect.top + 10, 2, RECTHP(&headerRect) - 20);

    //draw exit icon
   // if(s_ExitIcon.bmWidth == 0) {
    LoadBitmap(HDC_SCREEN, &s_ExitIcon, SPV_ICON[IC_BACK]);
    s_ExitIcon.bmType = BMP_TYPE_COLORKEY;
    s_ExitIcon.bmColorKey = PIXEL_black;
   // }
    //pExitText = GETSTRING(STRING_BACK);
    exitRect = headerRect;
    exitRect.left = exitRect.right - RECTH(exitRect);
    SelectFont(hdc, g_logfont);
    GetTextExtent(hdc, pExitText, -1, &size);

    size.cx /= SCALE_FACTOR;
    size.cy /= SCALE_FACTOR;

    x = exitRect.left + ((RECTW(exitRect) - exitW) >> 1);
    y = exitRect.top + ((RECTH(exitRect) - exitH - size.cy) >> 1);
    FillBoxWithBitmap(hdc, x, y, exitW, exitH, &s_ExitIcon);

    //draw exit text
    SetTextColor(hdc, PIXEL_lightwhite);
    exitRect.top = y + exitH;
    DrawText(hdc, pExitText, -1, &exitRect, DT_TOP | DT_CENTER | DT_SINGLELINE);

	if(INFO_DIALOG_SET_DATE == temptype){
    	PInfotmDate pdate = (PInfotmDate)tempInfo->infoData;

        if(RECTWP(&(pdate->trect)) < 1){
            pdate->trect.left = (headerRect.right - RECTH(headerRect)) * SCALE_FACTOR;
            pdate->trect.right = (headerRect.right - RECTH(headerRect) + RECTW(headerRect)) * SCALE_FACTOR;
            pdate->trect.top = (headerRect.bottom - 5) * SCALE_FACTOR;
            pdate->trect.bottom = headerRect.bottom * SCALE_FACTOR;
        }
        //draw focus divider
	    if(pdate->sel == 0){
    	    SetBrushColor(hdc, PIXEL_red);
            FillBox(hdc, headerRect.right - RECTH(headerRect), headerRect.bottom - 5, RECTW(headerRect), 5);
	    }
    }
    else if (INFO_DIALOG_SET_TIME == temptype)
    {
    	PInfotmTime ptime = (PInfotmTime)tempInfo->infoData;
       
        if(RECTWP(&(ptime->trect)) < 1){
            ptime->trect.left = (headerRect.right - RECTH(headerRect)) * SCALE_FACTOR;
            ptime->trect.right = (headerRect.right - RECTH(headerRect) + RECTW(headerRect)) * SCALE_FACTOR;
            ptime->trect.top = (headerRect.bottom - 5) * SCALE_FACTOR;
            ptime->trect.bottom = headerRect.bottom * SCALE_FACTOR;
        }
        //draw focus divider
	    if(ptime->sel == 0){
    	    SetBrushColor(hdc, PIXEL_red);
            FillBox(hdc, headerRect.right - RECTH(headerRect), headerRect.bottom - 5, RECTW(headerRect), 5);
	    }
    }
    else if(INFO_DIALOG_SET_PLATE == temptype)
    {
    	PInfotmPlate pplate = (PInfotmPlate)tempInfo->infoData;
       
        if(RECTWP(&(pplate->trect)) < 1){
            pplate->trect.left = (headerRect.right - RECTH(headerRect)) * SCALE_FACTOR;
            pplate->trect.right = (headerRect.right - RECTH(headerRect) + RECTW(headerRect)) * SCALE_FACTOR;
            pplate->trect.top = (headerRect.bottom - 5) * SCALE_FACTOR;
            pplate->trect.bottom = headerRect.bottom * SCALE_FACTOR;
        }
        //draw focus divider
	    if(pplate->sel == 0){
    	    SetBrushColor(hdc, PIXEL_red);
            FillBox(hdc, headerRect.right - RECTH(headerRect), headerRect.bottom - 5, RECTW(headerRect), 5);
	    }

    }

    SetBrushColor(hdc, PIXEL_lightwhite);
    if(INFO_DIALOG_SET_PLATE != temptype)
        DrawDateTimeContent(hDlg, hdc, tempInfo);
    else
        DrawPlateContent(hDlg, hdc, tempInfo);
   
    //DrawFocus(hDlg,hdc, &headerRect);

}


static int AdvInfoSettingDialogBoxProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static int keydown = 0;
	static PInfotmTime pTime = NULL;
	static PInfotmDate pDate = NULL;
	static PInfotmPlate pPlate = NULL;
	static PInfotmDateTime pDateTime = NULL;
	static INFO_DIALOG_TYPE type;
    switch(message) {
        case MSG_INITDIALOG:
        {

            DEVFONT *datetimefont = LoadDevFontFromFile("ttf-dt-rrncnn-0-0-UTF-8", "./res/arial-bold.ttf");
            type = ((PDialogInfo)lParam)->type;
            //INFO("lParam: %x, type: %x\n", lParam,type);

            switch(type){
                case INFO_DIALOG_SET_TIME:
		        {
                    pTime= (PInfotmTime)(((PDialogInfo)lParam)->infoData);
			        pTime->sel = 1;
                    /*pTime->cttrect.left = 0;
                    pTime->cttrect.right = 0;
                    pTime->cttrect.top = 0;
                    pTime->cttrect.bottom = 0;
                    pTime->trect.left = 0;
                    pTime->trect.right = 0;
                    pTime->trect.top = 0;
                    pTime->trect.bottom = 0;*/
                    break;
		        }
                case INFO_DIALOG_SET_DATE:
                { 
					pDate= (PInfotmDate)(((PDialogInfo)lParam)->infoData);
			        pDate->sel = 1;
                    /*pDate->cttrect.left = 0;
                    pDate->cttrect.right = 0;
                    pDate->cttrect.top = 0;
                    pDate->cttrect.bottom = 0;
                    pDate->trect.left = 0;
                    pDate->trect.right = 0;
                    pDate->trect.top = 0;
                    pDate->trect.bottom = 0;*/
                    break;
		        }
                case INFO_DIALOG_SET_PLATE:
                {
					pPlate= (PInfotmPlate)(((PDialogInfo)lParam)->infoData);
			        pPlate->sel = 1;
                    break;
                }
				case INFO_DIALOG_SET_DATETIME:
				{
					pDateTime = (PInfotmDateTime)(((PDialogInfo)lParam)->infoData);
					pDateTime->sel = 1;
					break;
				}

            }

            int fontsize = LARGE_SCREEN ? 22 : (16 * SCALE_FACTOR);

            SetWindowAdditionalData(hDlg, lParam);
            g_content_font = CreateLogFont("ttf", "fb", "UTF-8",
                FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                14, 0);
                
            g_logfont = CreateLogFont("ttf", "simsun", "UTF-8",
                FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
                FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                fontsize, 0); 
        //int ret = ft2SetLcdFilter(g_logfont, MG_SMOOTH_LEGACY);
        //INFO("ret:%d\n", ret);
            g_logfont_bold = CreateLogFont("ttf", "fb", "UTF-8",
                FONT_WEIGHT_DEMIBOLD, FONT_SLANT_ROMAN,
                FONT_FLIP_NIL, FONT_OTHER_AUTOSCALE,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                fontsize, 0);
        //ft2SetLcdFilter (g_logfont_bold, MG_SMOOTH_LIGHT);
        //SetWindowElementAttr(control, WE_FGC_WINDOW, 0xFFFFFFFF);
        //SetWindowFont(hDlg, g_title_info_font);
#if 0 
        SetWindowBkColor(hDlg, COLOR_transparent);
#else 
            SetWindowBkColor(hDlg, COLOR_black);
#endif
            break;
        }
        case MSG_PAINT:
            INFO("infodialog, msg_paint\n");
            hdc = BeginSpvPaint(hDlg);
#if 0
            SetBrushColor (hdc, RGBA2Pixel (hdc, 0x00, 0x00, 0x00, 0x0));
            FillBox (hdc, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); 
            SetMemDCAlpha (hdc, MEMDC_FLAG_SRCPIXELALPHA, 0);
            SetBkMode (hdc, BM_TRANSPARENT);
#endif
            PDialogInfo pInfo = (PDialogInfo)GetWindowAdditionalData(hDlg);
            //INFO("pInfo: %x, type: %d\n", pInfo, pInfo->type);
			if(INFO_DIALOG_SET_DATETIME == pInfo->type)
				DrawDateTimeAllContent(hDlg, hdc, pInfo);
			else
				DrawAdvInfoSettingDialog(hDlg, hdc, pInfo);
            EndPaint(hDlg, hdc);
            break;
        case MSG_COMMAND:
            switch(wParam) {
                case IDOK:
                case IDCANCEL:
                    EndAdvInfoSettingDialog(hDlg, wParam);
                    break;
            }
            break;
        case MSG_KEYDOWN:
        {
            INFO("infodlg, keydown: %d\n", wParam);
	
            switch(wParam) {
                case SCANCODE_SPV_OK:
	            {
	    	        int sel = -1;
					switch (type) {
						case INFO_DIALOG_SET_DATE:
							sel = pDate->sel;
							break;
						case INFO_DIALOG_SET_TIME:
							sel = pTime->sel;
							break;
						case INFO_DIALOG_SET_DATETIME:
							sel = pDateTime->sel;
							break;
						case INFO_DIALOG_SET_PLATE:
							sel = pPlate->sel;
							break;
						default:
							break;
					}

		            if(-1 != sel){
                        ChangeValue(hDlg, ((PDialogInfo)GetWindowAdditionalData(hDlg)));
		            }
                    break;
	            }
            }
	        break;
	    }
        case MSG_KEYUP:
        {
            INFO("OnKeyUp: %d\n", wParam);
            switch(wParam){
                case SCANCODE_INFOTM_MODE:
		        {
                    EndAdvInfoSettingDialog(hDlg, IDOK);
                    break;
		        }
                case SCANCODE_INFOTM_DOWN:
	                goNext(hDlg, ((PDialogInfo)GetWindowAdditionalData(hDlg)));
                    UpdateWindow(hDlg, TRUE);
                    break;
                case SCANCODE_INFOTM_UP:
	                goPre(hDlg, ((PDialogInfo)GetWindowAdditionalData(hDlg)));
                    UpdateWindow(hDlg, TRUE);
                    break;
	            case SCANCODE_INFOTM_OK:
	            {
	                int sel = -1;
		            if(INFO_DIALOG_SET_DATE == type){
						sel = pDate->sel;
	        	    }
	                else if(INFO_DIALOG_SET_TIME == type){
		                sel = pTime->sel;
    	    	    }else if(INFO_DIALOG_SET_PLATE == type){
		                sel = pPlate->sel;
                    } else if(INFO_DIALOG_SET_DATETIME == type) {
			            sel = pDateTime->sel;
					}
	   	
	                if(-1 != sel){
		                if((INFO_DIALOG_SET_DATE == type) && IsTimerInstalled(hDlg, TIMER_DATE)){
			                KillTimer(hDlg, TIMER_DATE);
			                keydown = 0;
	    	            }
    	    	        else if(INFO_DIALOG_SET_TIME == type && IsTimerInstalled(hDlg, TIMER_TIME)){
				            KillTimer(hDlg, TIMER_TIME);
				            keydown = 0;
		                }else if(INFO_DIALOG_SET_PLATE == type && IsTimerInstalled(hDlg, TIMER_PLATE)){
				            KillTimer(hDlg, TIMER_PLATE);
				            keydown = 0;
		                }else if(INFO_DIALOG_SET_DATETIME == type && IsTimerInstalled(hDlg, TIMER_DATETIME)){
				            KillTimer(hDlg, TIMER_DATETIME);
				            keydown = 0;
                        }
		            }
	                else{
	    	            ERROR("Error, get value of sel failed in MSG_KEYUP\n");
		            }
		        }
	        }
	        break;
        }
        case MSG_TIMER:
        {   
	        INFO_DIALOG_TYPE type = ((PDialogInfo)GetWindowAdditionalData(hDlg))->type;
	        if(0 == keydown){
	            keydown = 1;
		        if(INFO_DIALOG_SET_DATE == type)
		            ResetTimer(hDlg, TIMER_DATE, 10);
	            else if(INFO_DIALOG_SET_TIME == type)
		            ResetTimer(hDlg, TIMER_TIME, 10);
                else if(INFO_DIALOG_SET_PLATE == type)
                    ResetTimer(hDlg, TIMER_PLATE, 10);
                else if(INFO_DIALOG_SET_DATETIME == type)
                    ResetTimer(hDlg, TIMER_DATETIME, 10);
	        }
            ChangeValue(hDlg, ((PDialogInfo)GetWindowAdditionalData(hDlg)));
            break;
	    }
        case MSG_CLOSE:
            EndAdvInfoSettingDialog(hDlg, IDOK);
            break;
    }
        return DefaultDialogProc(hDlg, message, wParam, lParam);
}

static void MeasureWindowSize()
{
    DIALOG_BIG_WIDTH = (VIEWPORT_WIDTH - 120);
    DIALOG_BIG_HEIGHT = (VIEWPORT_HEIGHT - 48);

    DIALOG_NORMAL_WIDTH = (VIEWPORT_WIDTH - 120);
    DIALOG_NORMAL_HEIGHT = (VIEWPORT_HEIGHT - 168);

    DIALOG_SMALL_WIDTH = (VIEWPORT_WIDTH - 120);
    DIALOG_SMALL_HEIGHT = (VIEWPORT_HEIGHT - 208);
}


void ShowSetInfoDialog(HWND hWnd, INFO_DIALOG_TYPE type)
{
    DLGTEMPLATE InfoDlg =
    {
        WS_NONE,
#if SECONDARYDC_ENABLED
        WS_EX_AUTOSECONDARYDC,
#else
        WS_EX_NONE,
#endif
        0, 0, DEVICE_WIDTH, DEVICE_HEIGHT,
        "InfoDialog",
        0, 0,
        0, NULL,
        0
    };

    MeasureWindowSize();

    PDialogInfo info = malloc(sizeof(DialogInfo));
    memset(info, 0, sizeof(DialogInfo));

    info->type = type;
    switch(type) {
        case INFO_DIALOG_SET_DATE:
            InfoDlg.controls = AdvInfoSettingCtrl;
	        info->infoData = (DWORD)malloc(sizeof(InfotmDate));
            memset((void *)(info->infoData), 0, sizeof(InfotmDate));

	        getDate((PInfotmDate)info->infoData);
            //gettimeofday(&(((PPairingInfo)info->infoData)->beginTime), 0);
            break;
        case INFO_DIALOG_SET_TIME:
            InfoDlg.controls = AdvInfoSettingCtrl;
	        info->infoData = (DWORD)malloc(sizeof(InfotmTime));
            memset((void *)(info->infoData), 0, sizeof(InfotmTime));

	        getTime((PInfotmTime)info->infoData);
            break;
		case INFO_DIALOG_SET_DATETIME:
            InfoDlg.controls = AdvInfoSettingCtrl;
	        info->infoData = (DWORD)malloc(sizeof(InfotmDateTime));
            memset((void *)(info->infoData), 0, sizeof(InfotmDateTime));

	        getDateTime((PInfotmDateTime)info->infoData);
			break;
        case INFO_DIALOG_SET_PLATE:
            InfoDlg.controls = AdvInfoSettingCtrl;
	        info->infoData = (DWORD)malloc(sizeof(InfotmPlate));
            memset((void *)(info->infoData), 0, sizeof(InfotmPlate));

            getPlate((PInfotmPlate)info->infoData);
            break;
        default:
            ERROR("not support yet!\n");
            return;
    }

    DialogBoxIndirectParam(&InfoDlg, hWnd, AdvInfoSettingDialogBoxProc, (LPARAM)info);
    if(info->infoData != 0)
        free((void *)(info->infoData));
    free(info);
    info = NULL;
    INFO("ShowInfoDialog end\n");

}

