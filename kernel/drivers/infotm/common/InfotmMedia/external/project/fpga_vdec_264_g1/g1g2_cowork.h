#ifndef _G1G2_COWORK_
#define _G1G2_COWORK_

enum {
    USER_NONE,
    USER_G1,
    USER_G2,
};

#define USER_UNCHANGE  0
#define USER_CHANGED 1

/* init g1g2 cowork module */
int g1g2_cowork_init(void);

/* deinit g1g2 cowork module */
int g1g2_cowork_deinit(void);

/* reserve cowork module */
int g1g2_cowork_reserve(int user);

/* release cowork module */
int g1g2_cowork_release(int user);


/* get cowork user */
int g1g2_cowork_get_user(void);

#endif 
