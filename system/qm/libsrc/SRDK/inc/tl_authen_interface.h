/******************************************************************************
******************************************************************************/
#ifndef _TL_AUTHEN_INTERFACE_H
#define _TL_AUTHEN_INTERFACE_H

int encrypt_info_write(char *data, int len);
int encrypt_info_read(char *data, int len);
int encrypt_info_check(char *data, int len, unsigned short checksum);
#endif
