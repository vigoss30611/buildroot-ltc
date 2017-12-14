/***************************************************************************** 
** imapx200.h 
** 
** Copyright (c) 2009~2014 ShangHai Infotm Ltd all rights reserved. 
** 
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** Description: All registers used in U-BOOT on imapx200.
**
** Author:
**     Warits   <warits.wang@infotm.com>
**      
** Revision History: 
** ----------------- 
** 1.1  XXX 12/16/2009 XXX	Initialized by warits
*****************************************************************************/


#ifndef __IMAPX200_WINCE_H__
#define __IMAPX200_WINCE_H__

#define DRAM_BASE_PA_START	(0x40000000)
#define DRAM_BASE_CA_START	(0x82000000)
#define DRAM_BASE_UA_START	(0xa2000000)

/* Sleep Data Area */
#define IMAGE_SLEEP_DATA_OFFSET			(0x001BB000)	// must same with config.bib 
#define IMAGE_SLEEP_DATA_PA_START		(DRAM_BASE_PA_START+IMAGE_SLEEP_DATA_OFFSET)
#define IMAGE_SLEEP_DATA_CA_START		(DRAM_BASE_CA_START+IMAGE_SLEEP_DATA_OFFSET)
#define IMAGE_SLEEP_DATA_UA_START		(DRAM_BASE_UA_START+IMAGE_SLEEP_DATA_OFFSET)
#define IMAGE_SLEEP_DATA_SIZE			(0x00002000)

/*------------------------------------------------------------------------------
	Sleep Data Layout
------------------------------------------------------------------------------*/
#define SleepState_Data_Start			(0)
#define SleepState_WakeAddr			(SleepState_Data_Start)
#define SleepState_SYSCTL			(SleepState_WakeAddr	+ 4 )
#define SleepState_MMUTTB0			(SleepState_SYSCTL	+ 4 )
#define SleepState_MMUTTB1			(SleepState_MMUTTB0	+ 4 )
#define SleepState_MMUTTBCTL			(SleepState_MMUTTB1	+ 4 )
#define SleepState_MMUDOMAIN			(SleepState_MMUTTBCTL	+ 4 )
#define SleepState_SVC_SP			(SleepState_MMUDOMAIN	+ 4 )
#define SleepState_SVC_SPSR			(SleepState_SVC_SP	+ 4 )
#define SleepState_FIQ_SPSR			(SleepState_SVC_SPSR	+ 4 )
#define SleepState_FIQ_R8			(SleepState_FIQ_SPSR	+ 4 )
#define SleepState_FIQ_R9			(SleepState_FIQ_R8	+ 4 )
#define SleepState_FIQ_R10			(SleepState_FIQ_R9	+ 4 )
#define SleepState_FIQ_R11			(SleepState_FIQ_R10	+ 4 )
#define SleepState_FIQ_R12			(SleepState_FIQ_R11	+ 4 )
#define SleepState_FIQ_SP			(SleepState_FIQ_R12	+ 4 )
#define SleepState_FIQ_LR			(SleepState_FIQ_SP	+ 4 )
#define SleepState_ABT_SPSR			(SleepState_FIQ_LR	+ 4 )
#define SleepState_ABT_SP			(SleepState_ABT_SPSR	+ 4 )
#define SleepState_ABT_LR			(SleepState_ABT_SP	+ 4 )
#define SleepState_IRQ_SPSR			(SleepState_ABT_LR	+ 4 )
#define SleepState_IRQ_SP			(SleepState_IRQ_SPSR	+ 4 )
#define SleepState_IRQ_LR			(SleepState_IRQ_SP	+ 4 )
#define SleepState_UND_SPSR			(SleepState_IRQ_LR	+ 4 )
#define SleepState_UND_SP			(SleepState_UND_SPSR	+ 4 )
#define SleepState_UND_LR			(SleepState_UND_SP	+ 4 )
#define SleepState_SYS_SP			(SleepState_UND_LR	+ 4 )
#define SleepState_SYS_LR			(SleepState_SYS_SP	+ 4 )
#define SleepState_Data_End     		(SleepState_SYS_LR	+ 4 )
#define SLEEPDATA_SIZE		    		((SleepState_Data_End - SleepState_Data_Start) / 4)

#endif /*__IMAPX200_WINCE_H__*/
