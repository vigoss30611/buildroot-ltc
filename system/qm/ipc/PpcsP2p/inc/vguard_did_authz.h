#ifndef __VGUARD_DID_AUTHZ_H__
#define __VGUARD_DID_AUTHZ_H__


#define ENC_CHIP_H 1 //海思
#define ENC_CHIP_Z 2 //中星微
#define ENC_CHIP_R 3 //新的加密ROM

#define VG_UPDATE_TIME (300 * 1000)

#define SOCK_STREAM_PORT		30001				/*通信的端口*/
#define VG_P2P_DID_PFEFIX_MAX_LEN		8				/*DID前缀的最长取值*/

#define VGUARDP2P_AESKEY		"VGUARDP2P_AESKEY"	/*VGUARD数据通信加密秘钥*/

#define PACKED __attribute__ ((__packed__))			/*按字节对齐*/

#define P2P_DID_LEN				30					/*长度须为32n - 2, n > 0, n为整数*/
#define P2P_AUTHZ_ONLY				1
#define P2P_AUTHZ_AND_RECYCLE		2
#define P2P_DID_SOURCE_NOT_EXIT		-1
#define P2P_DID_ANALYSYS_ERROR		-2
#define P2P_DID_SYSTEM_ERROR		-3
#define P2P_DID_PARAMETER_ERROR		-4
#define P2P_DID_NO_ONE_LEFT			-5
#define P2P_DID_WRITE_BACK_ERROR	-6
#define P2P_DID_READ_ERROR			-7
#define P2P_DID_SPACE_TOO_SMALL		-8
#define P2P_DID_OPEN_FILE_ERROR		-9


typedef struct
{
	unsigned char enc_chip_type; //海思，中星微等
	unsigned char res0;          //保留，置0
	unsigned char res1;          //保留，置0
	unsigned char res2;          //保留，置0
	unsigned int  seq;           //加密芯片序号
} seq_t;

typedef struct
{
	unsigned char old_did[P2P_DID_LEN];            
	//旧DID，前缀加后缀，'0'结尾字符串，当旧DID不存在时，该字段填'0'结尾的随机字符串。
	unsigned char res0;                   
	//保留字段，现为0
	unsigned char checksum0;              
	//校验字段，除checksum之外的所有字节以unsigned char类型相加，取最低字节
	
	seq_t    new_seq;                     
	//8字节顺序号
	seq_t    old_seq;                     
	//8字节顺序号，释放授权时候需要传入
	unsigned char new_did_prefix[9];      
	//新DID前缀，'0'结尾字符串
	unsigned char res1[5];                
	//保留字段，现为0
	unsigned char req;                    
	//1表示申请新的DID，2表示释放老的DID并申请新的。
	unsigned char checksum1;              
	//校验字段
}PACKED dev_pack_t;

typedef struct
{
	unsigned char new_did[P2P_DID_LEN];            
	//新DID，前缀加后缀，'0'结尾字符串，当ack不等于1或2时，该字段填'0'结尾的随机字符串。
	unsigned char ack;                    
	//1表示新的授权，2表示回收旧的并授权新的，其他表示失败。
	unsigned char checksum0;              
	//校验字段	
} PACKED server_pack_t;

#ifndef bool
#define bool int
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define VG_AUTHZ_ERROR_NO_ERROR		0
#define VG_AUTHZ_ERROR_PARAM		-1
#define VG_AUTHZ_ERROR_OPEN_FILE	-2
#define VG_AUTHZ_ERROR_CONN_FAIL	-3
#define VG_AUTHZ_ERROR_SEND_FAIL	-4
#define VG_AUTHZ_ERROR_RECV_FAIL	-5
#define VG_AUTHZ_ERROR_ACK			-6
#define VG_AUTHZ_ERROR_CHECKSUM		-7
#define VG_AUTHZ_ERROR_FORMAT		-8
#define VG_AUTHZ_ERROR_LENGTH		-9
#define VG_AUTHZ_ERROR_HOSTNAME		-10
#define VG_AUTHZ_ERROR_UPDATE_FILE	-11
#define VG_AUTHZ_ERROR_NO_FILE		-12
#define VG_AUTHZ_ERROR_PARAM_NOT_EXIST	-13

//进行网络授权操作
//pdidPrefix ----- 从bin读取的DID前缀，传入参数只读
//pCrcKey ----- 从bin读取的CRCKey，传入参数只读
//encrypType ----- 从bin读取的加密芯片类型，传入参数只读
//encrypSeq ----- 从bin读取的加密芯片流水号，传入参数只读
//pAuthzAddr ----- 从bin读取的授权服务器地址，传入参数只读
//pAuthzHost ----- 从bin读取的授权服务器域名，传入参数只读
//pDidString ----- 授权以后的DID，传入传出参数
//didStringLen ----- 传入DID的buff长度
//pApiLicString ----- 授权以后的APILICENSE + CRCKEY，传入传出参数
//apiLicStringLen ----- 传入APILICENSE+CRCKEY的buff长度

int vg_conn_authz_server(const char *pAuthzAddr, 
	const unsigned int encrypType, const unsigned int encrypSeq, char *crckey, char *p2pconfdid, char *p2pconfapil);

#endif

