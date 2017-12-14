#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "crypto.h" 

#define MAXBUF 1024

void ShowCerts(SSL * ssl)
{
    X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);
    if (cert != NULL) {
        printf("[client]subject info:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("[client]certs is :%s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("[client]wrote by%s\n", line);//颁发者: 
        free(line);
        X509_free(cert);
    } else
        printf("[client]no certs infomation\n");
}
/************关于本文档********************************************
*filename: ssl-client.c
*purpose: 演示利用 OpenSSL 库进行基于 IP层的 SSL 加密通讯的方法，这是客户端例子
*wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
*date time:2007-02-02 20:10
*Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
* 但请遵循GPL
*Thanks to:Google
*Hope:希望越来越多的人贡献自己的力量，为科学技术发展出力
* 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
*********************************************************************/
int main(int argc, char **argv)
{
    int sockfd, len;
    struct sockaddr_in dest;
   // char buffer[MAXBUF + 1];
    SSL_CTX *ctx;
    SSL *ssl;
    char text[MAXBUF];
    char sign[MAXBUF];//


    if (argc != 3) {
        printf("[client]paramter is error,\n\t\t%s IP port\n\t example\t%s 127.0.0.1 80\n此程序用来从某个 IP 地址的服务器某个端口接收最多 MAXBUF 个字节的消息",
             argv[0], argv[0]);
        exit(0);
    }
    memset((char*)text, 0 ,MAXBUF);
    memset((char*)sign, 0 ,MAXBUF);
	
    /* SSL 库初始化，参看 ssl-server.c 代码 */
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }

    /* 创建一个 socket 用于 tcp 通信 */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(errno);
    }
    printf("[client]socket created\n");

    /* 初始化服务器端（对方）的地址和端口信息 */
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(argv[2]));
    if (inet_aton(argv[1], (struct in_addr *) &dest.sin_addr.s_addr) == 0) {
        perror(argv[1]);
        exit(errno);
    }
    printf("[client]address created\n");

    /* 连接服务器 */
    if (connect(sockfd, (struct sockaddr *) &dest, sizeof(dest)) != 0) {
        perror("Connect ");
        exit(errno);
    }
    printf("[client]server connected\n");

    /* 基于 ctx 产生一个新的 SSL */
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);
    /* 建立 SSL 连接 */
    if (SSL_connect(ssl) == -1)
        ERR_print_errors_fp(stderr);
    else {
        printf("[client]Connected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);
    }

    strcpy(text, "0123456789876543210");
    /* 发消息给服务器 */
    len = SSL_write(ssl, text, strlen(text));
    if (len < 0)
    {
        printf("[client]message '%s' send fail!error code is %d,error message is '%s'\n",text, errno, strerror(errno));
    }
    else
    {
        printf("[client]message '%s' send success, send %d bytes\n", text, len);

	    /* 接收对方发过来的消息，最多接收 MAXBUF 个字节 */
	   // bzero(sign, MAXBUF + 1);
	    /* 接收服务器来的消息 */
	    len = SSL_read(ssl, sign, MAXBUF);
	    if (len > 0)
	    {
	        printf("[client]receive message success:'%s',%d bytes data.\n",sign, len);
		    if(crypto_verify_data(text,sign)!=0)
		    {
		 //   	printf("verify error\n");
		    	return -1;
		    }	
	    }
	    else {
	        printf("[client]message receive fail,error message is %d,error info is '%s'\n\n",errno, strerror(errno));
	        goto finish;
	    }

    }
  finish:
    /* 关闭连接 */
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    return 0;
}
 

