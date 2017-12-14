/*
 * =====================================================================================
 *
 *       Filename:  net_ota.c
 *
 *    Description:  The implementation of net ota functions.
 *
 *        Version:  1.0
 *        Created:  2015年11月06日 15时40分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Helix , helix.he@infotmic.com.cn
 *        Company:  infoTM
 *
 * =====================================================================================
 */

#include <qsdk/upgrade.h>
#include "ota.h"
#include "net_ota.h"

//#include "net_utils.h"
#include <curl/curl.h>
//#include "md5.h"
#include <openssl/md5.h>
#include <time.h>
#include <sys/time.h>

#define MD5_SIZE 16
#define MD5_PROC_BLOCK 1024
#define HEADER_MAX 1024

#define CA_BUNDLE "/etc/certs/ca-bundle.crt"
#define CA_CERT   "/etc/certs/ca.cer"

static size_t header_offset;
static size_t body_offset;

size_t header_callback( void *ptr, size_t size, size_t nmemb, void *stream) {
    char* buffer = (char*)stream;
    memcpy(buffer+header_offset, ptr, size * nmemb);
    header_offset += size * nmemb;
    
#ifdef DEBUG
    struct timeval tv;
    gettimeofday(&tv, NULL);
    LOGI("%lld sec %lld usec, curl Read %d.\n", tv.tv_sec, tv.tv_usec, size*nmemb);
#endif

    return size * nmemb;
}
size_t body_callback( void *ptr, size_t size, size_t nmemb, void *stream) {
    char* buffer = (char*)stream;
    memcpy(buffer+body_offset, ptr, size * nmemb);
    body_offset += size * nmemb;

#ifdef DEBUG
    struct timeval tv;
    gettimeofday(&tv, NULL);
    LOGI("%lld sec %lld usec, curl Read %d.\n", tv.tv_sec, tv.tv_usec, size*nmemb);
#endif

    return size * nmemb;
}
int net_ota_update(char* data, char* value, char* api, char* url) 
{
    int ret;
    printf("net_ota_update value %s api %s url %s\n", value, api, url);

    ssize_t length = 0;
    ssize_t tmpres;
    char* pt = data;
    char* header_pt;
    char* data_pt;
    int i;
    header_offset = 0;
    body_offset = 0;
    CURL *curl;
    CURLcode curl_ret;
    curl_off_t fsize;
    // We close the socket but skip the check proccess.

    // Init curl
    struct curl_slist *headers=NULL; 
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");
    curl = curl_easy_init();
    if(!curl) {
        LOGE("Initialize curl failed.\n");
        return FAILED;
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE);
    curl_easy_setopt(curl, CURLOPT_SSLCERT, CA_CERT);
   
    snprintf(pt, 300, "%s%s", url, api);
    
    LOGI("URL: %s\n", pt);
    curl_easy_setopt(curl, CURLOPT_URL, pt);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 90);
    // Should we skip the header checking here? 
    header_pt = data;
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, header_pt);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    data_pt = data + 1000;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_pt);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, body_callback); 
    // We don't need to recerve the header here.
    // Perform the action
    ret = curl_easy_perform(curl);
    LOGI("curl easy perform return: %d\n", ret);
    if (ret == 0) {
        LOGI("Perform network get OTA information success!\n");
        // Since we receive data directly from curl we don't need to skip to the data part.
        //while(data[i++] != ' ');
        //if (data[i] != '2') {
        //    LOGE("Query OTA server receive '%c%c%c' error code.\n", data[i], data[i+1], data[i+2]);
        //    ret = FAILED;
        //    goto err_handle;
        //}
        LOGI("header of https = %s\n", header_pt);
        if (strncmp(header_pt, "HTTP/1.1 2", 10)) {
            LOGE("Get version infor header check failed.\n");
            ret = FAILED;
            goto err_handle;
        }
        ret = SUCCESS;
    } else {
        switch(ret) {
        case CURLE_COULDNT_CONNECT:
            LOGE("TimeOut can't connect to the host.\n");
            break;
        case CURLE_HTTP_RETURNED_ERROR:
            LOGE("HTTP return false.");
            break;
        case CURLE_SSL_ENGINE_INITFAILED:
            LOGE("SSL can't be initialized.");
            break;
        case CURLE_COULDNT_RESOLVE_HOST:
            LOGE("SSL can't create ssl connection.");
            break;
        default:
            LOGE("correspond failed with %d.", ret);
            break;
        }
        ret = FAILED;
    }
err_handle:
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return ret;
}

int net_ota_get(void* ctx, char* data, char* value, char* api, char* url) 
{

    assert(ctx != NULL);

    struct net_ota_qiwo* net_ctx = (struct net_ota_qiwo*)ctx;
    int ret;

    ssize_t length = 0;
    ssize_t tmpres;
    char* pt;
    char* header_pt;
    char* data_pt;
    int i;
    header_offset = 0;
    body_offset = 0;
    CURL *curl;
    CURLcode curl_ret;
    curl_off_t fsize;
    // We close the socket but skip the check proccess.
    pt = data;

    // Init curl
    struct curl_slist *headers=NULL; 
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");

    curl = curl_easy_init();
    if(!curl) {
        LOGE("Initialize curl failed.\n");
        return FAILED;
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(curl, CURLOPT_CAINFO, CA_BUNDLE);
    curl_easy_setopt(curl, CURLOPT_SSLCERT, CA_CERT);
    
    snprintf(pt, 300, "%s%s", url, api);
    
    LOGI("URL: %s\n", pt);
    curl_easy_setopt(curl, CURLOPT_URL, pt);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 90);
    // Should we skip the header checking here? 
    header_pt = data;
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, header_pt);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    data_pt = data + 1000;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_pt);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, body_callback); 
    // We don't need to recerve the header here.
    // Perform the action
    ret = curl_easy_perform(curl);
    i = 0;
    if (ret == 0) {
        LOGI("Perform network get OTA information success!\n");
        
        LOGI("header of https = %s\n", header_pt);
        if (strncmp(header_pt, "HTTP/1.1 2", 10)) {
            LOGE("Get version infor header check failed.\n");
            ret = FAILED;
            goto err_handle;
        }

        //
        while(data_pt[i]!='\0') {
            if (data_pt[i] == '_') {
                if (!strncmp(data_pt+i, "_code", 5)) {
                    i += 6;
                    while(data_pt[i++] != ':');
                    if (data_pt[i] == ' ') ++i;
                    LOGD("We get code data as %s.\n", data_pt+i);
                    if (!strncmp(data_pt+i, "200", 3)) {
                        break;
                    } else {
                        LOGE("Get abnormal status code as %c%c%c.\n", data_pt[i], data_pt[i+1], data_pt[i+2]);
                        ret = FAILED;
                        goto err_handle;
                    }
                }
            }
            ++i;
        }

        int filded = 0;
        while(data_pt[i++]!='{');
        LOGI("real data: %s\n", data_pt + i);
        while(data_pt[i]!='\0') {
            while(data_pt[i++]!='"');
            switch(data_pt[i]) {
                case 'n':
                    if (data_pt[i-1] == '"' && data_pt[i+4] == 'v') {
                        while(data_pt[i++] != ':');
                        if (data_pt[i] == '0') {
                            net_ctx->new_version = 0;
                        } else {
                            net_ctx->new_version = 1;
                        }
                        ++filded;
                    }
                    break;
                case 's':
                    if (data_pt[i-1] == '"' && (data_pt[i+11] == ':' || data_pt[i+16] == ':')) {
                        if (data_pt[i+7] == 'm') {
                            while(data_pt[i++] != ':');
                            while(data_pt[i++] != '"');
                            net_ctx->md5[1] = data_pt+i;
                            while(data_pt[i++] != '"');
                            data_pt[i-1] = '\0';
                            LOGI("SYSTEM MD5: %s.\n", net_ctx->md5[1]);
                            ++filded;
                        } else if (data_pt[i+7] == 'd') {
                            while(data_pt[i++] != ':');
                            while(data_pt[i++] != '"');
                            net_ctx->url[1] = data_pt+i;
                            while(data_pt[i++] != '"');
                            data_pt[i-1] = '\0';
                            LOGI("SYSTEM URL: %s.\n", net_ctx->url[1]);
                            ++filded;
                        }
                    }
                    break;
                case 'k':
                    if (data_pt[i-1] == '"' && (data_pt[i+11] == ':' || data_pt[i+16] == ':')) {
                        if (data_pt[i+7] == 'm') {
                            while(data_pt[i++] != ':');
                            while(data_pt[i++] != '"');
                            net_ctx->md5[0] = data_pt+i;
                            while(data_pt[i++] != '"');
                            data_pt[i-1] = '\0';
                            LOGI("KERNEL MD5: %s.\n", net_ctx->md5[0]);
                            ++filded;
                        } else if (data_pt[i+7] == 'd') {
                            while(data_pt[i++] != ':');
                            while(data_pt[i++] != '"');
                            net_ctx->url[0] = data_pt+i;
                            while(data_pt[i++] != '"');
                            data_pt[i-1] = '\0';
                            LOGI("SYSTEM URL: %s.\n", net_ctx->url[0]);
                            ++filded;
                        }
                    }
                    break;
            }
            if (filded >= 5) {
                net_ctx->offset = i+1;
                break;
            }
            while(data_pt[i] != ',' && data_pt[i] != '}' && data_pt[i] != '\0') {
                ++i;
            }
        }
        if (filded < 5) {
            LOGE("The check of integrity of ota information failed.");
            ret = FAILED;
            goto err_handle;
        }
        ret = SUCCESS;
    } else {
        switch(ret) {
        case CURLE_COULDNT_CONNECT:
            LOGE("TimeOut can't connect to the host.\n");
            break;
        case CURLE_HTTP_RETURNED_ERROR:
            LOGE("HTTP return false.");
            break;
        case CURLE_SSL_ENGINE_INITFAILED:
            LOGE("SSL can't be initialized.");
            break;
        case CURLE_COULDNT_RESOLVE_HOST:
            LOGE("SSL can't create ssl connection.");
            break;
        default:
            LOGE("No %d correspond failed.", ret);
            break;
        }
        ret = FAILED;
    }
err_handle:
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return ret;
}


int check_md5sum(char* file, ssize_t length, char* sum) 
{
    ssize_t offset = 0;
    ssize_t msize = MD5_PROC_BLOCK;
    int i;
    MD5_CTX md5;
    unsigned char md5sum[MD5_SIZE];
    unsigned char md5str[MD5_SIZE*2+1];
    MD5_Init(&md5);
    while(msize > 0) {
        MD5_Update(&md5, file+offset, msize);
        offset += MD5_PROC_BLOCK;
        msize = length - offset > MD5_PROC_BLOCK ? MD5_PROC_BLOCK : (length - offset);
    //    LOGI("msize = %d, length = %d, offset = %d\n", msize, length, offset);
    }
    MD5_Final(md5sum, &md5);
    for(i = 0; i < MD5_SIZE; i++)
    {
        printf("i = %d, %2x\n", i, md5sum[i]);
        snprintf(md5str + i*2, 3, "%02x", md5sum[i]);
    }
    md5str[MD5_SIZE*2] = '\0';
    LOGI("my sum = %s, his sum = %s\n", md5str, sum);
    if(strncmp(sum, md5str, MD5_SIZE*2)) {
        LOGE("Compare checksum failed.\n");
        return FAILED;
    }
    return SUCCESS;
}

ssize_t net_ota_read(char* url, char* md5, char* data, char** file) 
{
    assert(data != NULL);
    char* header_pt = data;         // Use data for temp storage for now. we asume that host wouldn't overseed 256 char.
    char* data_pt = data + HEADER_MAX;
    int ret;
    ssize_t length;
    char* pt = header_pt;
    char* tmppt;
    int i;
    //FILE* data_file = fopen("./data_file", "w");

    header_offset = 0;
    body_offset = 0;

    CURL *curl;
    curl_off_t fsize;

    curl = curl_easy_init();
    if(!curl) {
        LOGE("Initialize curl failed.\n");
        return FAILED;
    }
    LOGI("net ota read url: %s, md5: %s\n", url, md5);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 840);
    // Should we skip the header checking here? 
    header_pt = data;
    curl_easy_setopt(curl, CURLOPT_WRITEHEADER, header_pt);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, data_pt);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, body_callback); 
    // We don't need to recerve the header here.
    // Perform the action
    ret = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    //fclose(data_file);
    if (ret != 0) {
        LOGI("curl_easy_perform return %d\n", ret);
        switch(ret) {
        case CURLE_COULDNT_CONNECT:
            LOGE("TimeOut can't connect to the host.\n");
            break;
        case CURLE_HTTP_RETURNED_ERROR:
            LOGE("HTTP return false.");
            break;
        case CURLE_SSL_ENGINE_INITFAILED:
            LOGE("SSL can't be initialized.");
            break;
        case CURLE_COULDNT_RESOLVE_HOST:
            LOGE("SSL can't create ssl connection.");
            break;
        default:
            LOGE("correspond failed.");
            break;
        }
        ret = FAILED;
        goto err_handle;
    }

    LOGI("Perform net ota download success!\n");
    LOGI("header = %s\n", header_pt);
    
    LOGI("body_offset = %d\n", body_offset);
    *file = data_pt;

    if (SUCCESS == check_md5sum(data_pt, body_offset, md5)) {
        return body_offset;
    } else {
        return FAILED;
    }

err_handle:
    return ret;
}
