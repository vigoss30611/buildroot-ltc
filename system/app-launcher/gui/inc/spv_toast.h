/*
** $Id: static_impl.h 13596 2010-11-02 10:00:19Z houhuihua $
**
** static.h: the head file of Static Control module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/5/22
*/

#ifndef GUI_SPVTOASTCTRL_IMPL_H_
#define GUI_SPVTOASTCTRL_IMPL_H_

#include "gui_common.h"

#ifdef  __cplusplus
extern  "C" {
#endif

#define SPV_CTRL_TOAST "spv_toast"

BOOL RegisterSPVToastControl (void);

HWND ShowToast(HWND hWnd, TOAST_TYPE type, int duration);
void HideToast(HWND hWnd);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_SPVTOASTCTRL_IMPL_H_ */

