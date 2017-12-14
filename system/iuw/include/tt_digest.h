#ifndef __TT_DIGEST_H__
#define __TT_DIGEST_H__

#define HASH_NUM 4

struct tt_digest_dev {
    uint8_t name[16];
    uint8_t unit[8];
    double dax;
    int transfor_number;
    int error;
    uint64_t data;
};


#endif
