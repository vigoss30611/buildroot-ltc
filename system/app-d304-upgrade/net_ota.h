/*
 * =====================================================================================
 *
 *       Filename:  net_ota.h
 *
 *    Description:  This header provide functions and neccessary definition for download
 *                  ota packages via net and read the images from it.
 *
 *        Version:  1.0
 *        Created:  2015年11月06日 11时09分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Helix (), helix.he@infotmic.com.cn
 *        Company:  infoTM
 *
 * =====================================================================================
 */

#ifndef __NET_OTA_H__
#define __NET_OTA_H__

#define QIWO_CLOUD_ENV_DEV  0
#define QIWO_CLOUD_ENV_TEST 0
#define QIWO_CLOUD_ENV_PUB  1

#define BASE_PORT 443
//#define SU_QUERY_FORMAT "POST %s HTTP/1.1\r\nHost: %s\r\n\
//Content-Type: application/json\r\nCache-Control: no-cache\r\n\r\n\
//{\"did\":\"%s\",\"step\":\"%d\"}"
//#define CU_QUERY_FORMAT "POST %s HTTP/1.1\r\nHost: %s\r\n\
//Content-Type: application/json\r\nCache-Control: no-cache\r\n\r\n\
//{\"did\":\"%s\"}"
//#define GU_QUERY_FORMAT "POST %s HTTP/1.1\r\nHost: %s\r\n\
//Content-Type: application/json\r\nCache-Control: no-cache\r\n\r\n\
//{\"did\":\"%s\",\"version\":\"%s\"}"
#define SU_QUERY_FORMAT "{\"did\":\"%s\",\"step\":\"%d\"}"
#define CU_QUERY_FORMAT "{\"did\":\"%s\"}"
#define GU_QUERY_FORMAT "{\"did\":\"%s\",\"version\":\"%s\"}"
#define START_UPDATE 1
#define FINISH_DOWNLOAD 2
#define FINISH_UPDATE 3

#define BODY_DATA "{\"version\":\"%s\",\"productType\":\"%s\",\"productId\":\"%s\"}"
#define DOWNLOAD_FORMAT "GET %s HTTP/1.1\r\nUser-Agent: curl/7.35.0\r\nHost: %s\r\nAccept: */*\r\n\r\n"
#define EXTENSION "&currentVersion=%s"
#define PRODUCT_PREFIX "ipcam-304-"
#define OPT_SIZE 1024

struct net_ota_qiwo {
    int new_version;
    char* url[2];
    char* md5[2];
    int length[2];
    int offset;
};

int net_ota_update(char* data, char* value, char* api, char* url);

/* 
 * Get the net ota download context.
 * param:
 *  ctx: The context memory for parsed data.
 *  data: The memory to contain all received datas.
 *  version: The parameter for the version string.
 *  pd_type: The parameter for product type string.
 *  api: The parameter for the using api.
 * return:
 *  FAILED or SUCCESS
 */
int net_ota_get(void* ctx, char* data, char* value, char* api, char *url);

/* 
 * Read data from network and check the data integrity.
 * param:
 *  ctx: The context of net ota.
 *  data: The memory for receiving image.
 * return:
 *  FAILED or SUCCESS
 */
ssize_t net_ota_read(char* url, char* md5, char* data, char** file);

#endif
