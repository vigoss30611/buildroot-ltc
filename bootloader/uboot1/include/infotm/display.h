#ifndef __DISPLAY_H
#define __DISPLAY_H

extern int dlib_get_parameter(uint8_t *buf);
extern int dlib_get_logo(uint8_t *buf);
extern int dlib_mark_initialized(void);
extern void display_logo(int number);

#endif /* __DISPLAY_H */

