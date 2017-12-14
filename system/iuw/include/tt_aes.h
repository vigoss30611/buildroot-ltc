#ifndef __TT_AES_H__
#define __TT_AES_H__

struct conclusion {
    uint64_t data;
    int transfor_number;
    int enc_error;
    int dec_error;
    double dax;
    uint8_t unit[8];
};

struct tt_aes_file {
    char name[32];
    char path[128];
};

#endif

