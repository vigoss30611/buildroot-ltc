#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <qsdk/certif.h>
#include <qsdk/sys.h>

#if defined(CERTIF_TEST)

#define RETCODE_ENV_NAME           ("rcode")
#define RETCODE_LEN                (5)

/*
 * To create a new private/public key, use command below on your ubuntu:
 * $openssl genrsa -out id_rsa.prv 1024
 * $openssl rsa -in id_rsa.prv -pubout -out id_rsa.pub
 *
 * And copy public key to here:
 * public key should be embedded to code as below, added '\n' for every 64 character.
 */

static char certif_public_key[]="-----BEGIN PUBLIC KEY-----\n"
"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCx3eQVFGSquCnC3GE5a7AhsEEq\n"
"Sqmvec3/ZztQGTGRosajuAuEFDyvtWrVq1izh/+8byAOWXfKL4zsYctYKmgPCWPS\n"
"PMq3S0xMeR9cj8EkmnsVCGD9Rtu/kFB6hQzdbl/Ws6JeK+98g79sqB4CCvTKeOAB\n"
"qFw9KB70CYjh2TsFkwIDAQAB\n"
"-----END PUBLIC KEY-----\n";

// encrypted certification for "DEV:00-00-00-00-00-00 FUNC:JUST-FOR-TEST"
static char certif[] = "TlfBgujf7QLyhbUG3Cm62WwJLHu7NLxV2vQQD+BiP2j6J0cwjB71tBVeHHZhrZ41QYKVgv2SEGhIPClfNlgMKKmH7RCwla5jxAaA3Nq4Ed4yZ/PxCPPyZPito8OwtWd7T4jD+evnEQvgVcuQihB/qTQOKcYzpa8e6boxcMaTiJs=";

// wrong encrypted certification for nothing
static char wrong_certif[] = "wrong_certif_62WwJLHu7NLxV2vQQD+BiP2j6J0cwjB71tBVeHHZhrZ41QYKVgv2SEGhIPClfNlgMKKmH7RCwla5jxAaA3Nq4Ed4yZ/PxCPPyZPito8OwtWd7T4jD+evnEQvgVcuQihB/qTQOKcYzpa8e6boxcMaTiJs=";


extern int certif_save_artifical_certification(int alg_id, char *pub_key);
extern int certif_save_special_certification(char *certif);

int main(int argc, char *argv[])
{
	int ret, ret_code;
	char retcode[RETCODE_LEN] = {0};

	/* Write wrong certification directly to Flash and read it from Flash and check it */
	printf("TEST1: Write wrong certification directly to Flash and read it from Flash and check it. Suppose to fail here\n");
	sleep(1);
	ret = certif_save_special_certification(wrong_certif);
	if(ret) {
		printf("save certification failed\n");
		return -1;
	}

	ret_code = certif_check_algorithm_permit(0); //0 for automatic searching

	ret = system_get_env(RETCODE_ENV_NAME, retcode, RETCODE_LEN);
	if(ret) {
		printf("get retcode failed!\n");
		return -1;
	}

	ret = atoi(retcode);
	if(ret == ret_code) {
		printf("check certification permit successed\n");
	}
	else {
		printf("check certification permit failed, ret_code = %d, ret = %d\n", ret_code, ret);
	}
	printf("\n\n\n");

	/* Write special certification directly to Flash and read it from Flash and check it */
	printf("TEST2: Write special certification directly to Flash and read it from Flash and check it\n");
	sleep(1);
	ret = certif_save_special_certification(certif);
	if(ret) {
		printf("save certification failed\n");
		return -1;
	}

	ret_code = certif_check_algorithm_permit(0); //0 for automatic searching

	ret = system_get_env(RETCODE_ENV_NAME, retcode, RETCODE_LEN);
	if(ret) {
		printf("get retcode failed!\n");
		return -1;
	}

	ret = atoi(retcode);
	if(ret == ret_code) {
		printf("check certification permit successed\n");
	}
	else {
		printf("check certification permit failed, ret_code = %d, ret = %d\n", ret_code, ret);
		return -1;
	}
	printf("\n\n\n");

	/* Create a certification from ALG_ID_NONE, then write it to Flash. Read it from Flash and check it */
	printf("TEST3: Create a certification from ALG_ID_NONE, then write it to Flash. Read it from Flash and check it\n");
	sleep(1);
	ret = certif_save_artifical_certification(1, certif_public_key); //1 for ALG_ID_NONE
	if(ret) {
		printf("create certification & save it to Flash failed\n");
		return -1;
	}

	ret_code = certif_check_algorithm_permit(1); //1 for ALG_ID_NONE

	ret = system_get_env(RETCODE_ENV_NAME, retcode, RETCODE_LEN);
	if(ret) {
		printf("get retcode failed!\n");
		return -1;
	}

	ret = atoi(retcode);
	if(ret == ret_code) {
		printf("check ALG_ID_NONE permit successed\n");
	}
	else {
		printf("check ALG_ID_NONE permit failed, ret_code = %d, ret = %d\n", ret_code, ret);
		return -1;
	}
	printf("\n\n\n");

	/* Create a certification from ALG_ID_AEC, then write it to Flash. Read it from Flash and check it */
	printf("TEST4: Create a certification from ALG_ID_AEC, then write it to Flash. Read it from Flash and check it\n");
	sleep(1);
	ret = certif_save_artifical_certification(2, certif_public_key); //2 for ALG_ID_AEC
	if(ret) {
		printf("create certification & save it to Flash failed\n");
		return -1;
	}

	ret_code = certif_check_algorithm_permit(2); //2 for ALG_ID_AEC

	ret = system_get_env(RETCODE_ENV_NAME, retcode, RETCODE_LEN);
	if(ret) {
		printf("get retcode failed!\n");
		return -1;
	}

	ret = atoi(retcode);
	if(ret == ret_code) {
		printf("check ALG_ID_AEC permit successed\n");
	}
	else {
		printf("check ALG_ID_AEC permit failed, ret_code = %d, ret = %d\n", ret_code, ret);
		return -1;
	}
	printf("\n\n\n");

	/* Create a certification from ALG_ID_NONE, then write it to Flash. Read it from Flash and check it with ALG_ID_AEC */
	printf("TEST5: Create a certification from ALG_ID_NONE, then write it to Flash. Read it from Flash and check it with ALG_ID_AEC. Suppose to fail here\n");
	sleep(1);
	ret = certif_save_artifical_certification(1, certif_public_key); //1 for ALG_ID_NONE
	if(ret) {
		printf("create certification & save it to Flash failed\n");
		return -1;
	}

	ret_code = certif_check_algorithm_permit(2); //2 for ALG_ID_AEC

	ret = system_get_env(RETCODE_ENV_NAME, retcode, RETCODE_LEN);
	if(ret) {
		printf("get retcode failed!\n");
		return -1;
	}

	ret = atoi(retcode);
	if(ret == ret_code) {
		printf("check ALG_ID_NONE permit successed\n");
	}
	else {
		printf("check ALG_ID_NONE permit failed, ret_code = %d, ret = %d\n", ret_code, ret);
	}
	printf("\n\n\n");

	/* Create a certification from ALG_ID_AEC, then write it to Flash. Read it from Flash and check it with ALG_ID_NONE */
	printf("TEST6: Create a certification from ALG_ID_AEC, then write it to Flash. Read it from Flash and check it with ALG_ID_NONE. Suppose to fail here\n");
	sleep(1);
	ret = certif_save_artifical_certification(2, certif_public_key); //2 for ALG_ID_AEC
	if(ret) {
		printf("create certification & save it to Flash failed\n");
		return -1;
	}

	ret_code = certif_check_algorithm_permit(1); //1 for ALG_ID_AEC

	ret = system_get_env(RETCODE_ENV_NAME, retcode, RETCODE_LEN);
	if(ret) {
		printf("get retcode failed!\n");
		return -1;
	}

	ret = atoi(retcode);
	if(ret == ret_code) {
		printf("check ALG_ID_AEC permit successed\n");
	}
	else {
		printf("check ALG_ID_AEC permit failed, ret_code = %d, ret = %d\n", ret_code, ret);
		return -1;
	}
	printf("\n\n\n");

	return 0;
}

#endif
