#ifndef AV_VIDEO_BURST_H
#define AV_VIDEO_BURST_H


typedef enum {
    E_BURST_OFF,
    E_BURST_3,
    E_BURST_5,
    E_BURST_10,
} T_BURST_TYPE;


#ifdef __cplusplus 
extern "C" {
#endif


void burst_encode_init(int tskId, int bufferId);

void burst_encode_setPicCnt(unsigned int cnt,unsigned int bfilename,char*filename);


#ifdef __cplusplus 
}
#endif

#endif

