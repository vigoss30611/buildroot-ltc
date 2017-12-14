/*
 *  font.h -- `Soft' font definitions
 *
 *  Created 1995 by Geert Uytterhoeven
 */

#ifndef _VIDEO_FONT_H
#define _VIDEO_FONT_H

struct font_desc {
    int idx;
    const char *name;
    int width, height;
    const char *data;
    int pref;
};

/* Max. length for the name of a predefined font */
#define MAX_FONT_NAME    32

extern const struct font_desc font_7x14;

#endif /* _VIDEO_FONT_H */
