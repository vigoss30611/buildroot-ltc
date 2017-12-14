#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "crypto.h" 

#define MAXBUF 1024
/************关于本文档********************************************
*filename: ssl-server.c
*purpose: 演示利用 OpenSSL 库进行基于 IP层的 SSL 加密通讯的方法，这是服务器端例子
*wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
*date time:2007-02-02 19:40
*Note: 任何人可以任意复制代码并运用这些文档，当然包括你的商业用途
* 但请遵循GPL
*Thanks to:Google
*Hope:希望越来越多的人贡献自己的力量，为科学技术发展出力
* 科技站在巨人的肩膀上进步更快！感谢有开源前辈的贡献！
*********************************************************************/
int main(int argc, char **argv)
{
    int sockfd, new_fd;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    unsigned int myport, lisnum;
 //   char buf[MAXBUF + 1];
    SSL_CTX *ctx;
    char text[MAXBUF];
    char sign[MAXBUF];//

    memset((char*)text, 0 ,MAXBUF);
    memset((char*)sign, 0 ,MAXBUF);
	
    if (argv[1])
        myport = atoi(argv[1]);
    else
        myport = 7838;

    if (argv[2])
        lisnum = atoi(argv[2]);
    else
        lisnum = 2;

    /* SSL 库初始化 */
    SSL_library_init();
    /* 载入所有 SSL 算法 */
    OpenSSL_add_all_algorithms();
    /* 载入所有 SSL 错误消息 */
    SSL_load_error_strings();
    /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
    ctx = SSL_CTX_new(SSLv23_server_method());
    /* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
    if (ctx == NULL) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 载入用户的数字证书， 此证书用来发送给客户端。 证书里包含有公钥 */
    if (SSL_CTX_use_certificate_file(ctx, argv[4], SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 载入用户私钥 */
    if (SSL_CTX_use_PrivateKey_file(ctx, argv[5], SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }
    /* 检查用户私钥是否正确 */
    if (!SSL_CTX_check_private_key(ctx)) {
        ERR_print_errors_fp(stdout);
        exit(1);
    }

    /* 开启一个 socket 监听 */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    } else
        printf("[server]socket created\n");

    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = PF_INET;
    my_addr.sin_port = htons(myport);
    if (argv[3])
        my_addr.sin_addr.s_addr = inet_addr(argv[3]);
    else
        my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))== -1) {
        perror("bind");
        exit(1);
    } else
        printf("[server]binded\n");

    if (listen(sockfd, lisnum) == -1) {
        perror("listen");
        exit(1);
    } else
        printf("[server]begin listen\n");

    while (1) {
        SSL *ssl;
        len = sizeof(struct sockaddr);
        /* 等待客户端连上来 */
        if ((new_fd =accept(sockfd, (struct sockaddr *) &their_addr,&len)) == -1) {
            perror("[server]accept");
            exit(errno);
        } else
            printf("[server]server: got connection from %s, port %d, socket %d\n",
                   inet_ntoa(their_addr.sin_addr),ntohs(their_addr.sin_port), new_fd);

        /* 基于 ctx 产生一个新的 SSL */
        ssl = SSL_new(ctx);
        /* 将连接用户的 socket 加入到 SSL */
        SSL_set_fd(ssl, new_fd);
        /* 建立 SSL 连接 */
        if (SSL_accept(ssl) == -1) {
            perror("accept");
            close(new_fd);
            break;
        }
		

        /* 接收客户端的消息 */
        len = SSL_read(ssl, text, MAXBUF);
        if (len > 0)
            printf("[server]receive message success:'%s',%d bytes data.\n",
                   text, len);
        else
            printf("[server]message receive fail,error message is %d,error info is '%s'\n",errno, strerror(errno));

	  if(crypto_sign_data(sign,text,len)!=0)
	    {
	    	printf("[server]sign error\n");
	    	return -1;
	    }
	    len =	strlen(sign);
	    printf("[server]sign number is:\n");	
	    if(len > 0)	
	    {
		    hexprint(sign,len);
			
	        /* 开始处理每个新连接上的数据收发 */
	      //  bzero(buf, MAXBUF + 1);
	       // strcpy(buf, "server->client");
	        /* 发消息给客户端 */
	        len = SSL_write(ssl, sign, strlen(sign));

	        if (len <= 0) {
	            printf ("[server]message '%s' send fail!error code is %d,error message is '%s'\n", sign, errno, strerror(errno));
	            goto finish;
	        } 
		else
		{
	            printf("[server]message '%s' send success, send %d bytes\n",sign, len);
		}
	}
       
        /* 处理每个新连接上的数据收发结束 */
      finish:
        /* 关闭 SSL 连接 */
        SSL_shutdown(ssl);
        /* 释放 SSL */
        SSL_free(ssl);
        /* 关闭 socket */
        close(new_fd);
    }

    /* 关闭监听的 socket */
    close(sockfd);
    /* 释放 CTX */
    SSL_CTX_free(ctx);
    return 0;
}

