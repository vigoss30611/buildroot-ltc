#ifndef __MIX_H__
#define __MIX_H__

//#define Mix_dbg(x...) printf("Mix: " x)
#define Mix_dbg(x...)
#define Mix_err(x...) printf("Mix: " x)

#define Max_file        0x40000000
#define RND_FILE_PATH       "mix.rnd"
#define ICG_FILE_PATH       "mix.icg"
#define DEV_FILE_PATH       "mix.dev"
#define ENC_FILE_PATH       "mix.enc"
#define DEC_FILE_PATH       "mix.dec"
#define PNC_FILE_PATH       "mix.pnc"
#define PEC_FILE_PATH       "mix.pec"
#define RED_FILE_PATH       "mix.red"

struct Mixlist {
    char    name[36];
    int     data[3];
};


extern int Mix_init(char *path, struct Mixlist *list);

#endif
