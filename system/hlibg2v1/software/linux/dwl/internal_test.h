/* Copyright 2012 Google Inc. All Rights Reserved. */
/* Author: mkarki@google.com (Matti Kärki) */

#ifndef SOFTWARE_LINUX_DWL_INTERNAL_TEST_H_
#define SOFTWARE_LINUX_DWL_INTERNAL_TEST_H_

#include "basetype.h"

extern void InternalTestInit();
extern void InternalTestFinalize();
extern void InternalTestDumpReadSwReg(i32 core_id, u32 sw_reg, u32 value,
                                      u32 *sw_regs);
extern void InternalTestDumpWriteSwReg(i32 core_id, u32 sw_reg, u32 value,
                                       u32 *sw_regs);

#endif /* SOFTWARE_LINUX_DWL_INTERNAL_TEST_H_ */
