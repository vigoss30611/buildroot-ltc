#ifndef _CRYPTOTEST_H_  
#define _CRYPTOTEST_H_  
  
#include <stddef.h>
#include <string.h>
#include <unistd.h>
  
extern int rsa_prv_decrypt(char *str,char *path_key,int inlen,char** outstr);  
extern int rsa_pub_encrypt(char *str,char *path_key,char** outstr);
extern void hexprint(char *str,int len);
extern int create_rsa_pem(char *rsa_pub_file,char *rsa_prv_file);
extern int crypto_sign_data(char *out, char *in, int len);
extern int crypto_verify_data(char *text, char *sign);
#define  RSA_PUB_ENC 1
#define  RSA_PRV_DEC 2
#define  RSA_PRV_ENC 3
#define  RSA_PUB_DEC 4

#define  PRIVATE_KEY   0
#define  PUBLIC_KEY     1


#endif //_CRYPTOTEST_H_  