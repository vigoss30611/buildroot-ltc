#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "crypto.h" 

//能用公钥解密，就一定是私钥加密的
#define MSG_LEN (128+1)
int main(int argc,char**argv)
{
    char pubkey[]="/usr/share/crypto/id_rsa.pub";
    char prikey[]="/usr/share/crypto/id_rsa.prv";
    char cmd[256];
	
    if(access(pubkey, F_OK)||access(prikey, F_OK))
    {
    	 create_rsa_pem(pubkey,prikey);
	 sprintf(cmd, "ls -l %s", pubkey);
	 system(cmd);	 
	 sprintf(cmd, "ls -l %s", prikey);
	 system(cmd);	 
    }
    else
    {
	 printf("id_rsa.pub and id_rsa.prv have exist!\n");
    }
    return 0;	
}	