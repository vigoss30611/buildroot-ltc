#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <asm/ioctl.h> 
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/prctl.h>

//#include "aes.h"
#include "vguard_did_authz.h"

#define BYTE unsigned char       /* 8 bits  */
#define WORD unsigned long       /* 32 bits */

/* rotates x one bit to the left */

#define ROTL(x) (((x)>>7)|((x)<<1))

/* Rotates 32-bit word left by 1, 2 or 3 byte  */

#define ROTL8(x) (((x)<<8)|((x)>>24))
#define ROTL16(x) (((x)<<16)|((x)>>16))
#define ROTL24(x) (((x)<<24)|((x)>>8))

/* Fixed Data */

static BYTE InCo[4]={0xB,0xD,0x9,0xE};  /* Inverse Coefficients */

BYTE fbsub[256];
BYTE rbsub[256];
BYTE ptab[256],ltab[256];
WORD ftable[256];
WORD rtable[256];
WORD rco[30];


/* Parameter-dependent data */

int Nk;
BYTE fi[24],ri[24];
int Nb;
WORD fkey[120];
WORD rkey[120];
int Nr;

#define MY_NOOP \
	__asm("nop;");

static inline WORD pack(BYTE *b)
{ /* pack bytes into a 32-bit Word */
	MY_NOOP
    return ((WORD)b[3]<<24)|((WORD)b[2]<<16)|((WORD)b[1]<<8)|(WORD)b[0];
}

static inline void unpack(WORD a,BYTE *b)
{ /* unpack bytes from a word */
    b[0]=(BYTE)a;
	MY_NOOP
    b[1]=(BYTE)(a>>8);
	MY_NOOP
    b[2]=(BYTE)(a>>16);
	MY_NOOP
    b[3]=(BYTE)(a>>24);
}

//关于模多项式0x011b的乘10b运算
static inline BYTE xtime(BYTE a)
{
    BYTE b;
    if (a&0x80) b=0x1B;
    else        b=0;
	MY_NOOP
    a<<=1;
	MY_NOOP
    a^=b;
	MY_NOOP
    return a;
}

static inline BYTE bmul(BYTE x,BYTE y)
{ /* x.y= AntiLog(Log(x) + Log(y)) */
	MY_NOOP
    if (x && y)
	{
		MY_NOOP
		return ptab[(ltab[x]+ltab[y])%255];
    }
    else return 0;
}

static inline WORD SubByte(WORD a)
{
    BYTE b[4];
	MY_NOOP
    unpack(a,b);
	MY_NOOP
    b[0]=fbsub[b[0]];
	MY_NOOP
    b[1]=fbsub[b[1]];
	MY_NOOP
    b[2]=fbsub[b[2]];
	MY_NOOP
    b[3]=fbsub[b[3]];
	MY_NOOP
    return pack(b);    
}

static inline BYTE product(WORD x,WORD y)
{ /* dot product of two 4-byte arrays */
    BYTE xb[4],yb[4];
	MY_NOOP
    unpack(x,xb);
	MY_NOOP
    unpack(y,yb); 
	MY_NOOP
    return bmul(xb[0],yb[0])^bmul(xb[1],yb[1])^bmul(xb[2],yb[2])^bmul(xb[3],yb[3]);
}

static inline WORD InvMixCol(WORD x)
{ /* matrix Multiplication */
    WORD y,m;
    BYTE b[4];
	MY_NOOP
    m=pack(InCo);
	MY_NOOP
    b[3]=product(m,x);
	MY_NOOP
    m=ROTL24(m);
	MY_NOOP
    b[2]=product(m,x);
	MY_NOOP
    m=ROTL24(m);
	MY_NOOP
    b[1]=product(m,x);
	MY_NOOP
    m=ROTL24(m);
	MY_NOOP
    b[0]=product(m,x);
	MY_NOOP
    y=pack(b);
	MY_NOOP
    return y;
}

BYTE ByteSub(BYTE x)
{
    BYTE y=ptab[255-ltab[x]];  /* multiplicative inverse */
	MY_NOOP
    x=y;  x=ROTL(x);
	MY_NOOP
    y^=x; x=ROTL(x);
	MY_NOOP
    y^=x; x=ROTL(x);
	MY_NOOP
    y^=x; x=ROTL(x);
	MY_NOOP
    y^=x; y^=0x63;
	MY_NOOP
    return y;
}

static inline void gentables(void)
{ /* generate tables */
    int i;
    BYTE y,b[4];

  /* use 3 as primitive root to generate power and log tables */

    ltab[0]=0;
  MY_NOOP
    ptab[0]=1;  ltab[1]=0;
  MY_NOOP
    ptab[1]=3;  ltab[3]=1; 
  MY_NOOP
    for (i=2;i<256;i++)
    {
    	MY_NOOP
        ptab[i]=ptab[i-1]^xtime(ptab[i-1]);
		MY_NOOP
        ltab[ptab[i]]=i;
    }
    
  /* affine transformation:- each bit is xored with itself shifted one bit 
	仿射变换
	*/

    fbsub[0]=0x63;
  MY_NOOP
    rbsub[0x63]=0;
  MY_NOOP
    for (i=1;i<256;i++)
    {
    MY_NOOP
        y=ByteSub((BYTE)i);
	MY_NOOP
        fbsub[i]=y; 
	MY_NOOP
		rbsub[y]=i;
	MY_NOOP
    }

    for (i=0,y=1;i<30;i++)
    {
    	MY_NOOP
        rco[i]=y;
		MY_NOOP
        y=xtime(y);
		MY_NOOP
    }

  /* calculate forward and reverse tables */
    for (i=0;i<256;i++)
    {
    	MY_NOOP
        y=fbsub[i];
		MY_NOOP
        b[3]=y^xtime(y); 
		MY_NOOP
		b[2]=y;
		MY_NOOP
        b[1]=y;
		MY_NOOP
		b[0]=xtime(y);
		MY_NOOP
        ftable[i]=pack(b);
		MY_NOOP
        y=rbsub[i];
		MY_NOOP
        b[3]=bmul(InCo[0],y); 
		MY_NOOP
		b[2]=bmul(InCo[1],y);
		MY_NOOP
        b[1]=bmul(InCo[2],y); 
		MY_NOOP
		b[0]=bmul(InCo[3],y);
		MY_NOOP
        rtable[i]=pack(b);
		MY_NOOP
    }
}

static inline void gkey(int nb,int nk,BYTE *key)
{ /* blocksize=32*nb bits. Key=32*nk bits */
  /* currently nb,bk = 4, 6 or 8          */
  /* key comes as 4*Nk bytes              */
  /* Key Scheduler. Create expanded encryption key */
    int i,j,k,m,N;
    int C1,C2,C3;
    WORD CipherKey[8];
    
    Nb=nb; Nk=nk;

  /* Nr is number of rounds */
    if (Nb>=Nk) Nr=6+Nb;
    else        Nr=6+Nk;

    C1=1;
    if (Nb<8) { C2=2; C3=3; MY_NOOP}
    else      { C2=3; C3=4; MY_NOOP}

  /* pre-calculate forward and reverse increments */
    for (m=j=0;j<nb;j++,m+=3)
    {
    	MY_NOOP
        fi[m]=(j+C1)%nb;
		MY_NOOP
        fi[m+1]=(j+C2)%nb;
		MY_NOOP
        fi[m+2]=(j+C3)%nb;
		MY_NOOP
        ri[m]=(nb+j-C1)%nb;
		MY_NOOP
        ri[m+1]=(nb+j-C2)%nb;
		MY_NOOP
        ri[m+2]=(nb+j-C3)%nb;
    }

    N=Nb*(Nr+1);
    
    for (i=j=0;i<Nk;i++,j+=4)
    {
    	MY_NOOP
        CipherKey[i]=pack((BYTE *)&key[j]);
		MY_NOOP
    }
    for (i=0;i<Nk;i++) 
	{
		fkey[i]=CipherKey[i];
		MY_NOOP
    }
    for (j=Nk,k=0;j<N;j+=Nk,k++)
    {
        fkey[j]=fkey[j-Nk]^SubByte(ROTL24(fkey[j-1]))^rco[k];
		MY_NOOP
        if (Nk<=6)
        {
            for (i=1;i<Nk && (i+j)<N;i++)
            {
                fkey[i+j]=fkey[i+j-Nk]^fkey[i+j-1];
				MY_NOOP
            }
        }
        else
        {
            for (i=1;i<4 &&(i+j)<N;i++)
            {
                fkey[i+j]=fkey[i+j-Nk]^fkey[i+j-1];
				MY_NOOP
            }
            if ((j+4)<N)
			{
				fkey[j+4]=fkey[j+4-Nk]^SubByte(fkey[j+3]);
				MY_NOOP
            }
            for (i=5;i<Nk && (i+j)<N;i++)
            {
                fkey[i+j]=fkey[i+j-Nk]^fkey[i+j-1];
				MY_NOOP
            }
        }

    }

 /* now for the expanded decrypt key in reverse order */

    for (j=0;j<Nb;j++)
	{
		rkey[j+N-Nb]=fkey[j]; 
		MY_NOOP
    }
    for (i=Nb;i<N-Nb;i+=Nb)
    {
        k=N-Nb-i;
		MY_NOOP
        for (j=0;j<Nb;j++)
		{
			rkey[k+j]=InvMixCol(fkey[i+j]);
			MY_NOOP
        }
    }
    for (j=N-Nb;j<N;j++)
	{
		rkey[j-N+Nb]=fkey[j];
		MY_NOOP
    }
}


/* There is an obvious time/space trade-off possible here.     *
 * Instead of just one ftable[], I could have 4, the other     *
 * 3 pre-rotated to save the ROTL8, ROTL16 and ROTL24 overhead */ 

static inline void encrypt(BYTE *buff)
{
    int i,j,k,m;
    WORD a[8],b[8],*x,*y,*t;

    for (i=j=0;i<Nb;i++,j+=4)
    {
        a[i]=pack((BYTE *)&buff[j]);
        a[i]^=fkey[i];
    }
    k=Nb;
    x=a; y=b;

/* State alternates between a and b */
    for (i=1;i<Nr;i++)
    { /* Nr is number of rounds. May be odd. */

/* if Nb is fixed - unroll this next 
   loop and hard-code in the values of fi[]  */

        for (m=j=0;j<Nb;j++,m+=3)
        { /* deal with each 32-bit element of the State */
          /* This is the time-critical bit */
            y[j]=fkey[k++]^ftable[(BYTE)x[j]]^
                 ROTL8(ftable[(BYTE)(x[fi[m]]>>8)])^
                 ROTL16(ftable[(BYTE)(x[fi[m+1]]>>16)])^
                 ROTL24(ftable[x[fi[m+2]]>>24]);
        }
        t=x; x=y; y=t;      /* swap pointers */
    }

/* Last Round - unroll if possible */ 
    for (m=j=0;j<Nb;j++,m+=3)
    {
        y[j]=fkey[k++]^(WORD)fbsub[(BYTE)x[j]]^
             ROTL8((WORD)fbsub[(BYTE)(x[fi[m]]>>8)])^
             ROTL16((WORD)fbsub[(BYTE)(x[fi[m+1]]>>16)])^
             ROTL24((WORD)fbsub[x[fi[m+2]]>>24]);
    }   
    for (i=j=0;i<Nb;i++,j+=4)
    {
        unpack(y[i],(BYTE *)&buff[j]);
        x[i]=y[i]=0;   /* clean up stack */
    }
    return;
}

static inline void decrypt(BYTE *buff)
{
    int i,j,k,m;
    WORD a[8],b[8],*x,*y,*t;

    for (i=j=0;i<Nb;i++,j+=4)
    {
        a[i]=pack((BYTE *)&buff[j]);
        a[i]^=rkey[i];
    }
    k=Nb;
    x=a; y=b;

/* State alternates between a and b */
    for (i=1;i<Nr;i++)
    { /* Nr is number of rounds. May be odd. */

/* if Nb is fixed - unroll this next 
   loop and hard-code in the values of ri[]  */

        for (m=j=0;j<Nb;j++,m+=3)
        { /* This is the time-critical bit */
            y[j]=rkey[k++]^rtable[(BYTE)x[j]]^
                 ROTL8(rtable[(BYTE)(x[ri[m]]>>8)])^
                 ROTL16(rtable[(BYTE)(x[ri[m+1]]>>16)])^
                 ROTL24(rtable[x[ri[m+2]]>>24]);
        }
        t=x; x=y; y=t;      /* swap pointers */
    }

/* Last Round - unroll if possible */ 
    for (m=j=0;j<Nb;j++,m+=3)
    {
        y[j]=rkey[k++]^(WORD)rbsub[(BYTE)x[j]]^
             ROTL8((WORD)rbsub[(BYTE)(x[ri[m]]>>8)])^
             ROTL16((WORD)rbsub[(BYTE)(x[ri[m+1]]>>16)])^
             ROTL24((WORD)rbsub[x[ri[m+2]]>>24]);
    }        
    for (i=j=0;i<Nb;i++,j+=4)
    {
        unpack(y[i],(BYTE *)&buff[j]);
        x[i]=y[i]=0;   /* clean up stack */
    }
    return;
}

static inline int get_key(unsigned char * key)
{
	key[16] = 0x2;
	key[17] = 0x0;
	key[18] = 0xe3;
	key[19] = 0x55;
	key[20] = 0x38;
	key[21] = 0x92;
	key[22] = 0x9c;
	key[23] = 0xb1;
	key[24] = 0x48;
	key[25] = 0x34;
	key[26] = 0x96;
	key[27] = 0x77;
	key[28] = 0x3c;
	key[29] = 0xd0;
	key[30] = 0xf2;
	key[31] = 0x5f;
	
	key[0] = 0x5;
	key[1] = 0x90;
	key[2] = 0x23;
	key[3] = 0xe4;
	key[4] = 0x65;
	key[5] = 0x78;
	key[6] = 0x9c;
	key[7] = 0xb5;
	key[8] = 0x44;
	key[9] = 0x14;
	key[10] = 0x0a;
	key[11] = 0x61;
	key[12] = 0x4c;
	key[13] = 0xd;
	key[14] = 0x33;
	key[15] = 0x56;//0x56

	return 0;
}

int aes_enc(unsigned char block[])
{ 
    int nb,nk;

//	char key[32] = {
//		0x5 ,0x90 ,0x23 ,0xe4 ,0x65 ,0x78 ,0x9c ,0xb5 ,0x44 ,0x14 ,0x0a ,0x61 ,0x4c ,0x0d  ,0x33  ,0x55, 
//			0x2 ,0x0  ,0xe3 ,0x55 ,0x38 ,0x92 ,0x9c ,0xb1 ,0x48 ,0x34 ,0x96 ,0x77 ,0x3c ,0xd0 ,0xf2  ,0x5f
//	};
	unsigned char key[32];

	get_key(key);

	gentables();

	nb = 4;
	nk = 8;
		
    gkey(nb,nk,(unsigned char*)key);
	
    encrypt((unsigned char*)block);
		
    return 0;
}

void aes_dec(unsigned char block[])
{
    int nb,nk;

//	char key[32] = {
//		0x5 ,0x90 ,0x23 ,0xe4 ,0x65 ,0x78 ,0x9c ,0xb5 ,0x44 ,0x14 ,0x0a ,0x61 ,0x4c ,0x0d  ,0x33  ,0x55, 
//			0x2 ,0x0  ,0xe3 ,0x55 ,0x38 ,0x92 ,0x9c ,0xb1 ,0x48 ,0x34 ,0x96 ,0x77 ,0x3c ,0xd0 ,0xf2  ,0x5f
//	};

	unsigned char key[32];

	get_key(key);
	
	gentables();

	nb = 4;
	nk = 8;
		
    gkey(nb,nk,(unsigned char*)key);

    decrypt((unsigned char*)block);
}

void aes_enc16(BYTE *block, BYTE *key)
{
    if (block && key)
    {
        gentables();
        gkey(4, 4, key);
        encrypt(block);
    }
}

void aes_dec16(BYTE *block, BYTE *key)
{
    if (block && key)
    {
        gentables();
        gkey(4, 4, key);
        decrypt(block);
    }
}

void aes_enc32(BYTE *block, BYTE *key)
{
    if (block && key)
    {
        gentables();
        gkey(8, 4, key);
        encrypt(block);
    }
}

void aes_dec32(BYTE *block, BYTE *key)
{
    if (block && key)
    {
        gentables();
        gkey(8, 4, key);
        decrypt(block);
    }
}

static char *vg_chomp(char *buf)
{
	char *p;
	
	if (buf == NULL)
		return buf;
	for (p = buf; *p != '\0'; p++)
		;
	if (p != buf)
		p--;
	while (p >= buf && (*p == '\n' || *p == '\r'))
	{
		*p-- = '\0';
	}

	return(buf);
}

int vg_conn_authz_server(const char *pAuthzAddr, 
	const unsigned int encrypType, const unsigned int encrypSeq, char *crckey, char *p2pconfdid, char *p2pconfapil)
{
	int sockfd, tmp;
	struct sockaddr_in servaddr;
	dev_pack_t sendPack;
	unsigned char count;
	server_pack_t recvPack;

	printf("vg_conn_authz_server server ip = %s\n", pAuthzAddr);
	while((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("vg_conn_authz_server socket apply error\n");
		sleep(5);
	}
	memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(pAuthzAddr, &servaddr.sin_addr);
    servaddr.sin_port = htons(SOCK_STREAM_PORT);
//    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
//    {
//    	printf("vg_conn_authz_server conn error, %s\n", strerror(errno));
//		close(sockfd);
//		return VG_AUTHZ_ERROR_CONN_FAIL;
   // }
	int error=-1, len;
	len = sizeof(int);
	
	fd_set set;
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags|O_NONBLOCK); /*设置为非阻塞模式*/
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		struct timeval tm = {3,0};
		FD_ZERO(&set);
		FD_SET(sockfd, &set);
		do {
			if( select(sockfd+1, NULL, &set, NULL, &tm) > 0)
			{
				getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
				if (error == 0) break;
			}
			printf("vg_conn_authz_server conn error, %s\n", strerror(errno));
			close(sockfd);
			return VG_AUTHZ_ERROR_CONN_FAIL;
		}while(0);
	}
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags&(~O_NONBLOCK)); /*设置为阻塞模式*/
	//发送数据处理
	memset(&sendPack, 0, sizeof(sendPack));
	for (tmp = 0, count = 0; tmp < 31; tmp++)
	{
		count += ((unsigned char *)&sendPack)[tmp];
	}
	sendPack.checksum0 = count;
	aes_enc32((BYTE *)&sendPack, (BYTE *)VGUARDP2P_AESKEY);
	sendPack.new_seq.enc_chip_type = encrypType;
	sendPack.new_seq.seq = encrypSeq;
	strcpy((char *)(sendPack.new_did_prefix), "IVGS");
	sendPack.req = P2P_AUTHZ_ONLY;
	for (tmp = 32, count = 0; tmp < 63; tmp++)
	{
		count += ((unsigned char *)&sendPack)[tmp];
	}
	sendPack.checksum1 = count;
	aes_enc32((BYTE *)&sendPack + 32, (BYTE *)VGUARDP2P_AESKEY);
	if (send(sockfd, (char *)&sendPack, sizeof(sendPack), 0) < 0)
	{
		printf("vg_conn_authz_server send error, %s\n", strerror(errno));
		close(sockfd);
		return VG_AUTHZ_ERROR_SEND_FAIL;
	}
	if (recv(sockfd, &recvPack, sizeof(recvPack), 0) != sizeof(recvPack))
	{
		printf("vg_conn_authz_server recv error, %s\n", strerror(errno));
		close(sockfd);
		return VG_AUTHZ_ERROR_RECV_FAIL;
	}
	//接收数据处理
	aes_dec32((BYTE *)&recvPack,(BYTE *)VGUARDP2P_AESKEY);
	if (recvPack.ack != P2P_AUTHZ_ONLY)
	{
		printf("vg_conn_authz_server recv msg, ack error = %d\n", (char)(recvPack.ack));
		close(sockfd);
		return VG_AUTHZ_ERROR_ACK;
	}
	for (tmp = 0, count = 0; tmp < 31; tmp++)
	{
		count += ((unsigned char *)&recvPack)[tmp];
	}
	if (count != recvPack.checksum0)
	{
		printf("vg_conn_authz_server recv msg, checksum error\n");
		close(sockfd);
		return VG_AUTHZ_ERROR_CHECKSUM;
	}
	vg_chomp((char *)(recvPack.new_did));
	/*
	if (recvPack.new_did[strlen(recvPack.new_did) - 1] == '\r')
	{
		printf("vg_conn_authz_server == '\r'\n");
		recvPack.new_did[strlen(recvPack.new_did) - 1] = '\0';
	}
	*/
	//从接收的DID中分离正确的DID和CRCKey
	for (count = 0; count < strlen((char *)(recvPack.new_did)); count++)
	{
		if (recvPack.new_did[count] == ',')
			break;
	}
	if (count >= strlen((char *)(recvPack.new_did)))
	{
		printf("vg_conn_authz_server recv msg, format error\n");
		close(sockfd);
		return VG_AUTHZ_ERROR_FORMAT;
	}

	memcpy(p2pconfdid, recvPack.new_did, count);
	p2pconfdid[count] = '\0';
	sprintf(p2pconfapil, "%s:%s%c", recvPack.new_did + count + 1, crckey, '\0');

	printf("p2pconfdid: %s. \n",p2pconfdid);
	printf("p2pconfapil: %s. \n",p2pconfapil);
	close(sockfd);
	return VG_AUTHZ_ERROR_NO_ERROR;
}


