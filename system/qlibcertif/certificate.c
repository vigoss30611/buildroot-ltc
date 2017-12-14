#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/statfs.h>
#include <asm/unistd.h>
#include <linux/watchdog.h>
#include <time.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#undef  CERTIF_DEBUG
//#define CERTIF_DEBUG

#undef  SAVE_CERTIF_FILE
//#define SAVE_CERTIF_FILE

#define CHIP_ID_TAG			       ("DEV")
#define ALG_ID_TAG                 ("FUNC")

#define ALG_ID_CHAR_BIAS           (3)
#define ALG_ID_NONE                ("JUST-FOR-TEST")
#define ALG_ID_NONE_BIAS           ("MXVW0IRU0WHVW")
#define ALG_ID_AEC                 ("AEC-TL421-VCP-V1.0")
#define ALG_ID_AEC_BIAS            ("DHF0WO7540YFS0Y413")

#define CERTIF_ENV_NAME_CHAR_BIAS  (2)
#define CERTIF_ENV_NAME            ("alg-certif")
#define CERTIF_ENV_NAME_BIAS       ("cni/egtvkh")
#define CERTIF_ENV_NAME_LEN        (sizeof(CERTIF_ENV_NAME))

#define RETCODE_ENV_NAME_CHAR_BIAS (1)
#define RETCODE_ENV_NAME           ("rcode")
#define RETCODE_ENV_NAME_BIAS      ("sdpef")
#define RETCODE_ENV_NAME_LEN       (sizeof(RETCODE_ENV_NAME))

#define MAC_LEN                    (17+1)
#define ALG_LEN                    (18+1)
#define CERTIF_LEN                 (45+1)
#define ENCRYPTO_CERTIF_LEN        (256)
#define DECRYPTO_CERTIF_LEN        (256)
#define RETCODE_LEN                (5+1)

#define PRIVATE_KEY                (0)
#define PUBLIC_KEY                 (1)

#define RSA_PUB_ENC                (1)
#define RSA_PRV_DEC                (2)
#define RSA_PRV_ENC                (3)
#define RSA_PUB_DEC                (4)


#define ENV_PATH 				   ("/dev/env")
#define ENV_SET_SAVE			   _IOW('U', 213, int)
#define ENV_GET_SAVE			   _IOW('U', 214, int)

#ifdef CERTIF_DEBUG
	#define CERTIF_DBG(x...)       do{printf("CERTIF: " x);}while(0)
#else
	#define CERTIF_DBG(x...)       do{}while(0)
#endif

#define CERTIF_ERR(x...)           do{printf("CERTIF: " x);}while(0)

typedef struct {
	unsigned int  macid_h;
	unsigned int  macid_l;
} soc_info_t;

struct env_item_t {
	char name[128];
	char value[512];
};

const static char alg_id_list[][ALG_LEN] = {
	ALG_ID_NONE_BIAS,
	ALG_ID_AEC_BIAS,
};
#define ALG_ID_LIST_SIZE          (sizeof(alg_id_list)/sizeof(alg_id_list[0]))


#ifdef  SAVE_CERTIF_FILE
#define	CERTIF_FILE_PATH	       "/mnt/sd0/certification.txt"
static int certif_fd = -1;
#endif

/*
 * To create a new private/public key, use command below on your ubuntu:
 * $openssl genrsa -out id_rsa.prv 1024
 * $openssl rsa -in id_rsa.prv -pubout -out id_rsa.pub
 *
 * And copy private key to here:
 * private key should be embedded to code as below, added '\n' for every 64 character.
 */

static char certif_private_key[]="-----BEGIN RSA PRIVATE KEY-----\n"
"MIICXAIBAAKBgQCx3eQVFGSquCnC3GE5a7AhsEEqSqmvec3/ZztQGTGRosajuAuE\n"
"FDyvtWrVq1izh/+8byAOWXfKL4zsYctYKmgPCWPSPMq3S0xMeR9cj8EkmnsVCGD9\n"
"Rtu/kFB6hQzdbl/Ws6JeK+98g79sqB4CCvTKeOABqFw9KB70CYjh2TsFkwIDAQAB\n"
"AoGAQf8ACyZG9+VfcXMODB09/DIG6+dKMNb0LWkjY+QFNXF9XPAELdzIa7XXzzJF\n"
"tk+m/0cIUOq3fOjJ1EexCKSreRJJNtK1TO9pXJVIEbj8+e8jMzBRxoh0pRSItuBV\n"
"FV8kZhJjlCP7Szjhyi9JB3B3pK1zw6WRuoVxgnUnzGmT6HkCQQDgRUYDYy2Rk82+\n"
"kNuJoTlSLVoms3NfFZQ7UUfu72rrw+QX/OkYrrHafej0K/dIGSlXxf0UFFkREoXL\n"
"PTiN7MiFAkEAywfxndN4iXLNYmGKXzEESMlXgYTfQBPuXV5U0lq+AFfHdSEkBGeS\n"
"v0TfQBDWlMGyyHm74e214e0HHkyny5d9NwJAIGA/xDtsF6kDubAF0W+R69gaPJ4J\n"
"WL+vv4RzYv3zLIgCBKiBKgwGJumoWJ+EOkdPnZk6eMybMwi+geGbqXl2VQJBAKAw\n"
"eFZKD4SC68F/rClHB1ZWZZBvQaXRE/TfBJWjut2SZHDp4P0IqyP2Nx7ZKjTcTy8V\n"
"vUOYxjSnS0KAwLNFpykCQDM/MtmKYAS/b6p7UR2Z6VgqJUgUOJjcj/Zlhcy0eQdr\n"
"ieWOOsHaOp1RJLLGq854HbMz02MiIKCCr8rPixYZOk4=\n"
"-----END RSA PRIVATE KEY-----\n";

/**
 * @breif create a RSA instance for encrypt/decrypt
 *
 * @param key -IN: key that will be used by RSA encryption/decryption
 * @param public -IN: is the key public key
 *
 * @return RSA: create success
 * @return NULL: create fail
 */

RSA *createRSA(char *key, int public)
{
    RSA *rsa = NULL;
    BIO *keybio = NULL;

	CERTIF_DBG("%s Entered\n", __func__);
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        CERTIF_DBG( "Failed to create key BIO\n");
        return NULL;
    }
    if(public == PUBLIC_KEY)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, NULL, NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
    if(rsa == NULL)
    {
        CERTIF_DBG( "Failed to create RSA with %s\n", public ? "public key" : "private key");
    }
    BIO_set_close(keybio, BIO_CLOSE);
    BIO_free(keybio);
    return rsa;
}

/**
 * @breif RSA operation for public encrypt & private decrypt or private encrypt & public decrypt
 *
 * @param rsa_ctx -IN: RSA instance created from createRSA()
 * @param in_str  -IN: input string for operation
 * @param in_len  -IN: length of input string for operation
 * @param out_str -IN: output buffer for operation result
 * @param out_len -IN: length of output buffer for operation result
 * @param type    -IN: private key will be used by RSA encryption
 *
 * @return >=0: operation success
 * @return -1:  operation fail
 */

static int rsa_do_operation(RSA* rsa_ctx, char *in_str, int in_len, char* out_str, int out_len, int type)
{
    int rsa_len, num = -1;
	char* out_tmp = NULL;

	CERTIF_DBG("%s Entered\n", __func__);
    if(rsa_ctx == NULL || in_str == NULL || in_len <= 0 || out_str == NULL || out_len <= 0)
    {
		CERTIF_DBG("%s: parameter error: rsa_ctx = %p, in_str = %p, in_len = %d, out_str = %p, out_len = %d\n", __func__, rsa_ctx, in_str, in_len, out_str, out_len);
        return -1;
    }

    rsa_len = RSA_size(rsa_ctx);
    out_tmp = (char *)malloc(rsa_len + 1);
	if(out_tmp == NULL) {
		CERTIF_DBG("%s: get memory for out_tmp failed!\n", __func__);
		return -1;
	}
    memset(out_tmp, 0 ,rsa_len + 1);

    switch(type){

        case RSA_PUB_ENC: //pub enc
			num = RSA_public_encrypt(in_len, (unsigned char *)in_str, (unsigned char*)out_tmp, rsa_ctx, RSA_PKCS1_PADDING);
			break;

        case RSA_PRV_DEC: //prv dec
			num = RSA_private_decrypt(rsa_len, (unsigned char *)in_str, (unsigned char*)out_tmp, rsa_ctx, RSA_PKCS1_PADDING);
			break;

		case RSA_PRV_ENC: //prv enc
			num = RSA_private_encrypt(in_len, (unsigned char *)in_str, (unsigned char*)out_tmp, rsa_ctx, RSA_PKCS1_PADDING);
			break;

		case RSA_PUB_DEC: //pub dec
			num = RSA_public_decrypt(rsa_len, (unsigned char *)in_str, (unsigned char*)out_tmp, rsa_ctx, RSA_PKCS1_PADDING);
			break;

		default:
			break;
    }

    if(num == -1)
    {
        CERTIF_DBG("%s: (%s) on type(%d)!\n", __func__, ERR_error_string(ERR_get_error(), NULL), type);
    }
	else {
		num = (out_len > num ? num : out_len);
		memcpy(out_str, out_tmp, num);
        //CERTIF_DBG("%s: type(%d) operation success! num = %d, out_len = %d, out_tmp = %s, out_str = %s\n", __func__, type, num, out_len, out_tmp, out_str);//some crazy nonsense character, I don't want to printf it
	}

    free(out_tmp);
	out_tmp = NULL;
    return num;
}

#if defined(CERTIF_TEST)
/**
 * @breif use RSA algorithm to encrypt the @in_str with public key of @pub_key, and save it to @out_str
 *
 * @param in_str  -IN: input string for encryption
 * @param in_len  -IN: length of input string for encryption
 * @param out_str -IN: output buffer for encryption result
 * @param out_len -IN: length of output buffer for encryption result
 * @param pub_key -IN: public key will be used by RSA encryption
 *
 * @return >=0: encryption success
 * @return -1:  encryption fail
 */

static int rsa_pub_encrypt(char *in_str, int in_len, char *out_str, int out_len, char *pub_key) {
    RSA *p_rsa;
    int num;
	BIO *b64, *bio;
	BUF_MEM *bptr = NULL;

	CERTIF_DBG("%s Entered\n", __func__);
    if(in_str == NULL || in_len <= 0 || out_str == NULL || out_len <= 0 || pub_key == NULL)
    {
		CERTIF_DBG("%s:parameter error: in_str = %p, in_len = %d, out_str = %p, out_len = %d, pub_key = %p\n", __func__, in_str, in_len, out_str, out_len, pub_key);
        return -1;
    }

	//encypt with private key
	p_rsa = createRSA(pub_key, PUBLIC_KEY);
    if (p_rsa == NULL) {
	   CERTIF_DBG("create rsa error\n");
	   return -1;
	}
    num = rsa_do_operation(p_rsa, in_str, in_len, out_str, out_len, RSA_PUB_ENC);
	if(num < 0) {
		return -1;
	}
    RSA_free(p_rsa);

	//encode to base64
	//CERTIF_DBG("%s: before base64 encoded, in_str = {%s}, in_len = %d\n", __func__, out_str, num);//some crazy nonsense character, I don't want to printf it
	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);
	BIO_write(bio, out_str, num);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bptr);
	memcpy(out_str, bptr->data, bptr->length);
	num = bptr->length;
	out_str[num] = '\0';
	BIO_free_all(bio);
	CERTIF_DBG("%s: after base64 encoded, out_str = {%s}, out_len = %d\n", __func__, out_str, num);


    return num;
}
#endif

/**
 * @breif use RSA algorithm to decrypt the @in_str with private key of @pri_key, and save it to @out_str
 *
 * @param in_str  -IN: input string for decryption
 * @param in_len  -IN: length of input string for decryption
 * @param out_str -IN: output buffer for decryption result
 * @param out_len -IN: length of output buffer for decryption result
 * @param pri_key -IN: private key will be used by RSA decryption
 *
 * @return >=0: encryption success
 * @return -1:  encryption fail
 */

static int rsa_pri_decrypt(char *in_str, int in_len, char *out_str, int out_len, char *pri_key) {
    RSA *p_rsa;
    int num;
	char *tmp_str = NULL;
	BIO *b64, *bio;

	CERTIF_DBG("%s Entered\n", __func__);
    if(in_str == NULL || in_len <= 0 || out_str == NULL || out_len <= 0 || pri_key == NULL)
    {
		CERTIF_DBG("%s:parameter error: in_str = %p, in_len = %d, out_str = %p, out_len = %d, pri_key = %p\n", __func__, in_str, in_len, out_str, out_len, pri_key);
        return -1;
    }

	//decode from base64
	CERTIF_DBG("%s: before base64 decoded, in_str = {%s}, in_len = %d\n", __func__, in_str, in_len);
	tmp_str = (char *)malloc(in_len);
	if(tmp_str == NULL) {
		CERTIF_DBG("%s: malloc for tmp_str fail\n", __func__);
		return -1;
	}
	b64 = BIO_new(BIO_f_base64());
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bio = BIO_new_mem_buf(in_str, in_len);
	bio = BIO_push(b64, bio);
	num = BIO_read(bio, tmp_str, in_len);
	tmp_str[num] = '\0';
	BIO_free_all(bio);
	//CERTIF_DBG("%s: after base64 decoded, out_str = {%s}, out_len = %d\n", __func__, tmp_str, num);//some crazy nonsense character, I don't want to printf it

	//decypt with public key
	p_rsa = createRSA(pri_key, PRIVATE_KEY);
    if (p_rsa == NULL) {
	   CERTIF_DBG("create rsa error\n");
	   num = -1;
	   goto err;
	}
    num = rsa_do_operation(p_rsa, tmp_str, num, out_str, out_len, RSA_PRV_DEC);
    RSA_free(p_rsa);

err:
	free(tmp_str);
	tmp_str = NULL;
    return num;
}

#if 0
/**
 * @breif use RSA algorithm to encrypt(so-called "sign") the @text_data with private key of @pri_key, and save it to @sign_data
 *
 * @param text_data -IN: the buffer where orignal data to be encrypted store
 * @param text_len -IN: the length of orignal data to be encrypted
 * @param sign_len -OUT: the buffer where encrypted data should be put
 * @param pri_key -IN: private key will be used by RSA encryption
 *
 * @return 0: sign success
 * @return -1: sign fail
 */
static int rsa_sign_with_prikey(char *text_data, int text_len, char* sign_data, char * pri_key)
{
    RSA *p_rsa;
    unsigned int out_len = 0;

	CERTIF_DBG("%s\n", __func__);
	if (text_data == NULL || sign_data == NULL || pri_key == NULL) {
		CERTIF_DBG("NULL-pointer input error\n");
		return -1;
	}

	p_rsa = createRSA(pri_key, PRIVATE_KEY);

    if (p_rsa == NULL)
	   return -1;
    if (!RSA_sign(NID_md5, (unsigned char*)text_data, text_len, (unsigned char*)sign_data, &out_len, p_rsa))
    {
        return -1;
    }
    CERTIF_DBG("get sign data len is %d.\n",out_len);
    RSA_free(p_rsa);

    return 0;
}

/**
 * @breif use RSA algorithm to decrypt the @sign_data with public key of @pub_key, and verify it with @text_data
 *
 * @param text_data -IN: the buffer pointer where orignal data to be verified store
 * @param sign_len -IN: the buffer pointer where put encrypted data to be decrypted
 * @param sign_len -IN: the length of encrypted data to be decrypted
 * @param pub_key -IN: public key will be used by RSA decryption
 *
 * @return 0: verify success
 * @return -1: verify fail
 */
static int rsa_verify_with_pubkey(char *text_data, char* sign_data, int sign_len, char *pub_key)
{
    RSA *p_rsa;

	CERTIF_DBG("%s\n", __func__);
	if (text_data == NULL || sign_data == NULL || pub_key == NULL) {
		CERTIF_DBG("NULL-pointer input error\n");
		return -1;
	}

    p_rsa = createRSA(pub_key, PUBLIC_KEY);

    if(p_rsa == NULL)
    {
	   return -1;
    }

    if(!RSA_verify(NID_md5, (unsigned char*)text_data, strlen(text_data), (unsigned char*)sign_data, sign_len, p_rsa))
    {
        return -1;
    }
    RSA_free(p_rsa);
    return 0;
}
#endif

/**
 * @breif get soc id, namely MAC, from efuse
 *
 * @param s_info -OUT: MAC that return from the efuse
 *
 * @return 0: get success
 * @return -1: get fail
 */

static int certif_get_soc_id(soc_info_t *s_info) {

	 FILE *fp;
	 char split[] = ":";
	 char linebuf[512];
	 char *result = NULL;

	 CERTIF_DBG("%s Entered\n", __func__);
	 memset(s_info, 0x0, sizeof (* s_info));
	 fp = fopen("/proc/efuse", "r");
	 if(fp == NULL) {
		 CERTIF_DBG("failed to open soc info node %s \n", strerror(errno));
		 memset(s_info, 0xFF, sizeof (* s_info));
		 return -1;
	 }

	while (fgets(linebuf, 512, fp)) {
		result = strtok(linebuf, split);
		if (!strcmp(result, "MAC")) {

			char mac[2];
			int i;

			result = strtok(NULL, split);

			for(i = 0; i < 6; i++) {
				mac[0] = result[2*i];
				mac[1] = result[2*i + 1];
				if(i < 2)
					s_info->macid_h |= ((strtol(mac, NULL, 16) & 0xff) << ((1 - i) * 8));
				else
					s_info->macid_l |= ((strtol(mac, NULL, 16) & 0xff) << ((5 - i) * 8));
			}
			break;
		}
	}
	fclose(fp);
	return 0;
}

/**
 * @breif get env from Flash
 *
 * @param name -IN: env name
 * @param value -OUT: env value
 * @param count -IN: length of env value
 *
 * @return 0: get success
 * @return -1: get fail
 */

static int certif_get_env(char *name, char *value, int count)
{
	int fd, ret;
	char data[512];

	CERTIF_DBG("%s Entered\n", __func__);
	if ((fd = open(ENV_PATH, (O_RDWR | O_NDELAY))) < 0) {
		CERTIF_DBG("env %s open failed %s \n", ENV_PATH, strerror(errno));
		return fd;
	}

	sprintf(data, name);
	ret = ioctl(fd, ENV_GET_SAVE, (unsigned)data);
	if (ret) {
		close(fd);
		return ret;
	}

	snprintf(value, count, data);
	close(fd);
	return 0;
}

/**
 * @breif set env from Flash
 *
 * @param name -IN: env name
 * @param value -IN: env value
 *
 * @return 0: set success
 * @return -1: set fail
 */

static int certif_set_env(char *name, char *value)
{
	int fd, ret;
	struct env_item_t env_item;

	CERTIF_DBG("%s Entered\n", __func__);
	if ((fd = open(ENV_PATH, (O_RDWR | O_NDELAY))) < 0) {
		CERTIF_DBG("env %s open failed %s \n", ENV_PATH, strerror(errno));
		return fd;
	}

	snprintf(env_item.name, sizeof(env_item.name), name);
	snprintf(env_item.value, sizeof(env_item.value), value);
	ret = ioctl(fd, ENV_SET_SAVE, (unsigned)(&env_item));
	if (ret) {
		close(fd);
		return ret;
	}

	close(fd);
	return 0;
}

/**
 * @breif check whether the certification passed in is legal
 *
 * @param certif -IN: the encrypted string that will be decrypted and verified
 * @param certif_len -IN: lenght of the encrypted string
 * @param alg_id -IN: algorithm id will be checked
 *
 * @return random number matched to env_value of "rcode": legal certification
 * @return -1: illegal certification
 */

int certif_check_certification(char *encrypted_certif, int certif_len, int alg_id)
{
	char certif[CERTIF_LEN] = {0};
	char decrypted_certif[DECRYPTO_CERTIF_LEN] = {0};
	char mac[MAC_LEN] = {0};
	char alg[ALG_LEN] = {0};
	char retcode[RETCODE_LEN] = {0};
	char retcode_env_name[RETCODE_ENV_NAME_LEN] = {0};
	soc_info_t s_info;
	int ret, success_ret = 670;
	int i, j;
	unsigned int macid_h, macid_l;

	CERTIF_DBG("%s Entered\n", __func__);
	srand(time(0));

	//get mac address
	memset(&s_info, 0, sizeof(s_info));
	ret = certif_get_soc_id(&s_info);
	if(ret) {
		CERTIF_DBG("get CHIP-ID failed!\n");
		return -1;
	}
	macid_h = s_info.macid_h;
	macid_l = s_info.macid_l;
	snprintf(mac, MAC_LEN, "%02x-%02x-%02x-%02x-%02x-%02x", (macid_h>>8) & 0xff, macid_h & 0xff, (macid_l>>24) & 0xff, (macid_l>>16) & 0xff, (macid_l>>8) & 0xff, macid_l & 0xff);

	//iterate the whole alg_id_list[]
	for(i = 0; i < ALG_ID_LIST_SIZE; i++) {

		if(!alg_id_list[i]) {
			break;
		}

		//you will get a biased algorithm id from the array of alg_id_list[]
		strncpy(alg, alg_id_list[i], ALG_LEN);
		CERTIF_DBG("alg-bias = %s\n", alg);

		//is that algorithm id user want to check? alg_id = 0 mean user doesn't care about which algorithm it is checked
		if(alg_id && ((alg_id-1) != i)) {
			continue;
		}

		//every char of the biased algorithm id must subtract ALG_ID_CHAR_BIAS from it to get the original algorithm id
		for(j = 0; j < ALG_LEN; j++) {
			if(*(alg+j) == '\0')
				break;
			*(alg+j) -= ALG_ID_CHAR_BIAS;
		}
		CERTIF_DBG("alg-org = %s\n", alg);

		//check the "mac id + algorithm" with the decrypted certification from the flash
		snprintf(certif, CERTIF_LEN, "%s:%s %s:%s", CHIP_ID_TAG, mac, ALG_ID_TAG, alg);
		CERTIF_DBG("for {%s} - certification supposed to be {%s}\n", alg, certif);
		//"verify" only ascertian the certification is not modified illegally
		//ret = rsa_verify_with_pubkey(certif, encrypted_certif, strlen(encrypted_certif), certif_public_key);
		ret = rsa_pri_decrypt(encrypted_certif, strlen(encrypted_certif), decrypted_certif, sizeof(decrypted_certif), certif_private_key);
		//decypt success & compare match
		if((ret > 0) && !strncmp(decrypted_certif, certif, ret)) {
			//use a random number less than 3001 as our return code
			success_ret = (rand()%3000) + 1;
			snprintf(retcode, RETCODE_LEN, "%d", success_ret);

			//every char of the biased env_name of return code must substract RETCODE_ENV_NAME_CHAR_BIAS from it to get the original env_name
			strncpy(retcode_env_name, RETCODE_ENV_NAME_BIAS, RETCODE_ENV_NAME_LEN);
			CERTIF_DBG("retcode-env-name-bias = %s\n", retcode_env_name);
			for(j = 0; j < RETCODE_ENV_NAME_LEN; j++) {
				if(*(retcode_env_name+j) == '\0')
					break;
				*(retcode_env_name+j) -= RETCODE_ENV_NAME_CHAR_BIAS;
			}
			CERTIF_DBG("retcode-env-name-orig = %s\n", retcode_env_name);

			//set the env_value of return code, so return code will be saved to Flash
			ret = certif_set_env(retcode_env_name, retcode);
			if(ret) {
				CERTIF_DBG("set retcode failed!\n");
				return -1;
			}
			CERTIF_DBG("verify certification successed for {%s}, retcode = %d!\n", alg, success_ret);
			return success_ret;
		}
		//decrypt fail
		else {
			CERTIF_DBG("verify certification failed for {%s}!\n", alg);
		}
	}
	CERTIF_DBG("verify certification failed for all alg!\n");
	return -1;
}

/**
 * @breif check whether the algorithm passed in is permited
 *
 * @param alg_id -IN: algorithm id will be checked
 *
 * @return random number matched to env_value of "rcode": algorithm permited
 * @return -1: algorithm no permited 
 */

int certif_check_algorithm_permit(int alg_id)
{
	char encrypted_certif[ENCRYPTO_CERTIF_LEN] = {0};
	char certif_env_name[CERTIF_ENV_NAME_LEN] = {0};
	int ret;
	int j;

	CERTIF_DBG("%s Entered\n", __func__);

#ifdef SAVE_CERTIF_FILE
	certif_fd = open(CERTIF_FILE_PATH, O_RDONLY, 0666);
	if (certif_fd < 0) {
		CERTIF_DBG("create certification file fail in {%s}!\n", CERTIF_FILE_PATH);
	}
	else {
		read(certif_fd, encrypted_certif, ENCRYPTO_CERTIF_LEN);
		close(certif_fd);
		CERTIF_DBG("read from certification file: {%s}!\n", encrypted_certif);
	}
#endif

	//every char of the biased env_name of certification must substract CERTIF_ENV_NAME_CHAR_BIAS from it to get the original env_name
	strncpy(certif_env_name, CERTIF_ENV_NAME_BIAS, CERTIF_ENV_NAME_LEN);
	CERTIF_DBG("certif-env-name-bias = %s\n", certif_env_name);
	for(j = 0; j < CERTIF_ENV_NAME_LEN; j++) {
		if(*(certif_env_name+j) == '\0')
			break;
		*(certif_env_name+j) -= CERTIF_ENV_NAME_CHAR_BIAS;
	}
	CERTIF_DBG("certif-env-name-orig = %s\n", certif_env_name);

	//get certification by env_name
	ret = certif_get_env(certif_env_name, encrypted_certif, ENCRYPTO_CERTIF_LEN);
	if(ret) {
		CERTIF_DBG("get encrypted certification failed!\n");
		return -1;
	}

	//check whether the certification permit the algorithm indexed by alg_id
	CERTIF_DBG("get certification env: {%s}!\n", encrypted_certif);
	ret = certif_check_certification(encrypted_certif, strlen(encrypted_certif), alg_id);

	return ret;
}

#if defined(CERTIF_TEST)
/**
 * @breif just for test, save a artifical certification to Flash to test the reverse flow
 *
 * @param alg_id  -IN: alg_id indicate the algorithm id will be use to make the artifical certification
 * @param pub_key -IN: public key will be used to create the artifical certification
 *
 * @return 0: save success
 * @return -1: save fail
 */

int certif_save_artifical_certification(int alg_id, char *pub_key)
{
	char encrypted_certif[ENCRYPTO_CERTIF_LEN] = {0};
	char certif[CERTIF_LEN] = {0};
	char mac[MAC_LEN] = {0};
	char alg[ALG_LEN] = {0};
	char certif_env_name[CERTIF_ENV_NAME_LEN] = {0};
	soc_info_t s_info;
	int ret;
	int j;
	unsigned int macid_h, macid_l;

	CERTIF_DBG("%s Entered\n", __func__);

	if(alg_id > ALG_ID_LIST_SIZE) {
		CERTIF_DBG("wrong alg_id(%d)!\n", alg_id);
		return -1;
	}
	else if(alg_id == 0) {
		alg_id = 1;
	}

	//get mac address
	memset(&s_info, 0, sizeof(s_info));
	ret = certif_get_soc_id(&s_info);
	if(ret) {
		CERTIF_DBG("get CHIP-ID failed!\n");
		return -1;
	}
	macid_h = s_info.macid_h;
	macid_l = s_info.macid_l;
	snprintf(mac, MAC_LEN, "%02x-%02x-%02x-%02x-%02x-%02x", (macid_h>>8) & 0xff, macid_h & 0xff, (macid_l>>24) & 0xff, (macid_l>>16) & 0xff, (macid_l>>8) & 0xff, macid_l & 0xff);

	//every char of the alg_id_list[alg_id] must substract ALG_ID_CHAR_BIAS from it to get the original algorithm id
	strncpy(alg, alg_id_list[alg_id-1], ALG_LEN);
	CERTIF_DBG("alg-bias = %s\n", alg);
	for(j = 0; j < ALG_LEN; j++) {
		if(*(alg+j) == '\0')
			break;
		*(alg+j) -= ALG_ID_CHAR_BIAS;
	}
	CERTIF_DBG("alg-org = %s\n", alg);

	//encrypt the "mac id + algorithm" so that it become a certification, which is the processdure of so-called sign 
	snprintf(certif, CERTIF_LEN, "%s:%s %s:%s", CHIP_ID_TAG, mac, ALG_ID_TAG, alg);
	CERTIF_DBG("for {%s} - certification supposed to be {%s}\n", alg, certif);
    //"sign" only ascertian the certification is not modified illegally
	//ret = rsa_sign_with_prikey(certif, strlen(certif), encrypted_certif, pri_key);
	ret = rsa_pub_encrypt(certif, strlen(certif), encrypted_certif, sizeof(encrypted_certif), pub_key);
	//encrypt fail
	if(ret <= 0) {
		CERTIF_DBG("create encrypted certification fail for {%s}!\n", alg);
		return -1;
	}

#ifdef CERTIF_DEBUG
	char decrypted_certif[DECRYPTO_CERTIF_LEN] = {0};

	CERTIF_DBG("%s:immediately decrypted: for {%s} - encrypted certification is {%s}\n", __func__, alg, encrypted_certif);
	ret = rsa_pri_decrypt(encrypted_certif, strlen(encrypted_certif), decrypted_certif, sizeof(decrypted_certif), certif_private_key);
	//decrypt fail
	if(ret <= 0) {
		CERTIF_DBG("decrypted certification fail for {%s}!\n", alg);
		return -1;
	}
	CERTIF_DBG("%s:immediately decrypted: for {%s} - certification decrypted to {%s}\n", __func__, alg, decrypted_certif);
#endif

#ifdef SAVE_CERTIF_FILE
	certif_fd = open(CERTIF_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if (certif_fd < 0) {
		CERTIF_DBG("create certification file fail in {%s}!\n", CERTIF_FILE_PATH);
	}
	else {
		CERTIF_DBG("write to certification file: {%s}!\n", encrypted_certif);
		write(certif_fd, encrypted_certif, ENCRYPTO_CERTIF_LEN);
		close(certif_fd);
	}
#endif

	//every char of the biased env_name of certification must substract CERTIF_ENV_NAME_CHAR_BIAS from it to get the original env_name
	strncpy(certif_env_name, CERTIF_ENV_NAME_BIAS, CERTIF_ENV_NAME_LEN);
	CERTIF_DBG("certif-env-name-bias = %s\n", certif_env_name);
	for(j = 0; j < CERTIF_ENV_NAME_LEN; j++) {
		if(*(certif_env_name+j) == '\0')
			break;
		*(certif_env_name+j) -= CERTIF_ENV_NAME_CHAR_BIAS;
	}
	CERTIF_DBG("certif-env-name-orig = %s\n", certif_env_name);

	//set the env_value of certification, so certification will be saved to Flash
	CERTIF_DBG("set certification env: {%s}!\n", encrypted_certif);
	ret = certif_set_env(certif_env_name, encrypted_certif);
	if(ret) {
		CERTIF_DBG("save encrypted certification failed!\n");
		return -1;
	}

	return 0;
}

/**
 * @breif just for test, save a special certification to Flash to test the reverse flow
 *
 * @param certification -IN: the artifical that should be decrypted
 *
 * @return 0: save success
 * @return -1: save fail
 */

int certif_save_special_certification(char *certif)
{
	char certif_env_name[CERTIF_ENV_NAME_LEN] = {0};
	int ret;
	int j;

#ifdef SAVE_CERTIF_FILE
	certif_fd = open(CERTIF_FILE_PATH, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	if (certif_fd < 0) {
		CERTIF_DBG("create certification file fail in {%s}!\n", CERTIF_FILE_PATH);
	}
	else {
		CERTIF_DBG("write to certification file: {%s}!\n", certif);
		write(certif_fd, certif, strlen(certif) + 1);
		close(certif_fd);
	}
#endif

	//every char of the biased env_name of certification must substract CERTIF_ENV_NAME_CHAR_BIAS from it to get the original env_name
	strncpy(certif_env_name, CERTIF_ENV_NAME_BIAS, CERTIF_ENV_NAME_LEN);
	CERTIF_DBG("certif-env-name-bias = %s\n", certif_env_name);
	for(j = 0; j < CERTIF_ENV_NAME_LEN; j++) {
		if(*(certif_env_name+j) == '\0')
			break;
		*(certif_env_name+j) -= CERTIF_ENV_NAME_CHAR_BIAS;
	}
	CERTIF_DBG("certif-env-name-orig = %s\n", certif_env_name);

	//set the env_value of certification, so certification will be saved to Flash
	ret = certif_set_env(certif_env_name, certif);
	if(ret) {
		CERTIF_DBG("save encrypted certification failed!\n");
		return -1;
	}

	return 0;
}
#endif

