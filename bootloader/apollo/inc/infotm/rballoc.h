
#ifndef _RB_H_
#define _RB_H_

struct reserved_buffer {
    char owner[32];
    uint32_t start;
    uint32_t length;
};

extern void rbinit(void);
extern void * rballoc(const char * owner, uint32_t len);
extern void * rbget(const char *owner);
extern uint32_t rbgetint(const char *owner);
extern void rbsetint(const char *owner, uint32_t);
extern void rblist(void);

#endif

