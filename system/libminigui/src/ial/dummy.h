/*
** $Id: dummy.h 6566 201-06-04 jeason.zhao $
**
** dummy.h:. the head file for Custom IAL Engine.
**
** Copyright (C) 2015 Infotmic Software.
**
** Created by jeason.zhao, 2015/06/04
*/

#ifndef GUI_IAL_DUMMY_H
#define GUI_IAL_DUMMY_H

/* bit 7 = state bits0-3 = key number */
#define KEY_RELEASED    0x80
#define KEY_NUM         0x0f

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
BOOL InitDummyInput (INPUT* input, const char* mdev, const char* mtype);
void TermDummyInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_DUMMY__H */
