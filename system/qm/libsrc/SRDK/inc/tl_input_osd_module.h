/******************************************************************************
******************************************************************************/

#ifndef _TL_INPUT_OSD_MODULE_H
#define _TL_INPUT_OSD_MODULE_H

#define Input_OSD_SetChar     	1
#define Input_OSD_SetString    	2
#define Input_OSD_SetPos    	3
#define Input_OSD_Enable    	4
#define Input_OSD_Disable    	5


typedef struct
{	
	unsigned int 	Input_OSD_Font[32];	
	char 		*msg;
	int  		HPos;
	int 		VPos;		
}tl_osdreq;


#endif
