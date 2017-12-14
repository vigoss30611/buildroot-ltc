#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "crypto.h" 
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#if 0
//#define PUBENC_PRVDEC 1
#define PRVENC_PUBDEC 2
int main(int argc,char** argv){ 

    char *ptr_en,*ptr_de;
    int len;

    if (argc != 3) {
	printf("more args, usage is: ./crypto_test \n");
	return 0;
    } else {
	printf("start to crypto_test\n");
	
    }
    printf("random numbe is :%s\n",argv[1]);
    printf("encrypt file is :%s\n",argv[2]);
    printf("decrypt file is :%s\n",argv[3]);
	
#ifdef  PUBENC_PRVDEC 
    len=rsa_pub_encrypt(argv[1],argv[2],&ptr_en);
    printf("pubkey encrypt:\n");
    hexprint(ptr_en,len);
    
    rsa_prv_decrypt(ptr_en,argv[3],len,&ptr_de);
    printf("prvkey decrypt:%s\n",ptr_de==NULL?"NULL":ptr_de);

    if(!strcmp(ptr_de,argv[1]))
	    printf("\nauthentication is ok!\n");
    else
	    printf("\nauthentication is fail!\n");
#endif

#ifdef  PRVENC_PUBDEC 
    len=rsa_prv_encrypt(argv[1],argv[2],&ptr_en);
    printf("pubkey encrypt:\n");
    hexprint(ptr_en,len);
    
    rsa_pub_decrypt(ptr_en,argv[3],len,&ptr_de);
    printf("prvkey decrypt:%s\n",ptr_de==NULL?"NULL":ptr_de);

  if(!strcmp(ptr_de,argv[1]))
	    printf("\nauthentication is ok!\n");
    else
	    printf("\nauthentication is fail!\n");	
#endif
	
    
    if(ptr_en!=NULL){
        free(ptr_en);
    } 
    if(ptr_de!=NULL){
        free(ptr_de);
    } 
    
    return 0;
} 
#else

//能用公钥解密，就一定是私钥加密的
#define MSG_LEN (128+1)
int main(int argc,char**argv)
{
    char text[MSG_LEN];
    char sign[MSG_LEN];//
    int len=0;

    memset((char*)text, 0 ,MSG_LEN);
    memset((char*)sign, 0 ,MSG_LEN);
	

    strcpy((char*)text, "123456789 123456789 123456789 12a");
    len= strlen(text);
    printf("rand number is %s,len=%d\n",text,len); 	
	
    if(crypto_sign_data(sign,text,len)!=0)
    {
    	printf("sign error\n");
    	return -1;
    }
    len =	strlen(sign);
    printf("sign number is:\n");	
    hexprint(sign,len);
	
    if(crypto_verify_data(text,sign)!=0)
    {
 //   	printf("verify error\n");
    	return -1;
    }
//    printf("verify ok\n");
    return 0;
}
#endif
