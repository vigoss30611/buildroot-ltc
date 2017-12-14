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


#ifndef __IMAPX200_DENALI_H__
#define __IMAPX200_DENALI_H__

#define DENALI_CTL_PA_09	(EMCPA_BASE_REG_PA + 0x24)
#define DENALI_CTL_PA_62	(EMCPA_BASE_REG_PA + 0xf8)
#define DENALI_CTL_PA_63	(EMCPA_BASE_REG_PA + 0xfc)
#define DENALI_CTL_PA_64	(EMCPA_BASE_REG_PA + 0x100)
#define DENALI_CTL_PA_65	(EMCPA_BASE_REG_PA + 0x104)
#define DENALI_CTL_PA_66	(EMCPA_BASE_REG_PA + 0x108)
#define DENALI_CTL_PA_67	(EMCPA_BASE_REG_PA + 0x10c)
#define DENALI_CTL_PA_68	(EMCPA_BASE_REG_PA + 0x110)
#define DENALI_CTL_PA_69	(EMCPA_BASE_REG_PA + 0x114)

#define DENALI_CTL_PA_18	(EMCPA_BASE_REG_PA + 0x48)
#define DENALI_CTL_PA_46	(EMCPA_BASE_REG_PA + 0xb8)
#define DENALI_CTL_PA_47	(EMCPA_BASE_REG_PA + 0xbc)
#define DENALI_CTL_PA_74	(EMCPA_BASE_REG_PA + 0x128)
#define EMC_PORTA_ADDR          (EMCPA_BASE_REG_PA)

#define DENALI_CTL_R46          0x00460642          
#define DENALI_CTL_R47          0x00000046          
#define DENALI_CTL_R74          0x00a00000                  
#define DENALI_CTL_R18_ODT      0x3

#endif /*__IMAPX200_DENALI_H__*/
