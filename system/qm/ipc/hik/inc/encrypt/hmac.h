/*******************************************************************************
       Copyright: 2007-2008 Hangzhou Hikvision Digital Technology Co.,Ltd
        Filename:  hmac.h
     Description:  head file of HMAC algorithm 
         Version:  1.0
         Created:  01/08/2008 01 CST
          Author:  xmq <xiemq@hikvision.com>
******************************************************************************/

#ifndef _HMAC_H
#define _HMAC_H

#ifdef  __cplusplus
extern "C" {
#endif

void encryptHMAC(unsigned char *in, int in_len,     /* input string */
                unsigned char *key, int key_len,    /* input key */
                unsigned char digest[16]);          /* result */

#ifdef  __cplusplus
}
#endif
#endif	/* _HMAC_H */

