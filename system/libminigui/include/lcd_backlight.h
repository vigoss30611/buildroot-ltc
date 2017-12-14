#ifndef LCD_BACKLIGHT_H
#define LCD_BACKLIGHT_H

#include <stdint.h>
//#include <sys/cdef.h>


//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_OUT_OF_RANGE	-1

#define LCD_BACKLIGHT_MAX 255

void lcd_blank(int blank);

int set_backlight(int brightness);
int get_backlight(void);

#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif
