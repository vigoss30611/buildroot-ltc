/* Copyright 2012 Google Inc. All Rights Reserved. */

#ifndef CONFIG_H_DEFINED
#define CONFIG_H_DEFINED

#include "basetype.h"

/* tile border coefficients of filter */
#define ASIC_VERT_FILTER_RAM_SIZE 8 /* bytes per pixel row */
/* BSD control data of current picture at tile border
 * 128 bits per 4x4 tile = 128/(8*4) bytes per row */
#define ASIC_BSD_CTRL_RAM_SIZE 4 /* bytes per pixel row */

void SetCommonConfigRegs(u32 *regs);

#endif /* CONFIG_H_DEFINED */
