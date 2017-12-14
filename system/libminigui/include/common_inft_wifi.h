#ifndef COMMON_INTF_WIFI_H
#define COMMON_INTF_WIFI_H

typedef enum {
    INFT_WIFI_MODE_MIN = 0,
    INFT_WIFI_MODE_STA = INFT_WIFI_MODE_MIN,
    INFT_WIFI_MODE_AP,
    INFT_WIFI_MODE_STA_AP,
    INFT_WIFI_MODE_MAX,

}INFT_WIFI_MODE;

typedef enum
{
    INFT_WIFI_ENCTYPE_INVALID       = 0x00, 
    INFT_WIFI_ENCTYPE_NONE          = 0x01, //
    INFT_WIFI_ENCTYPE_WEP           = 0x02, //WEP, for no password
    INFT_WIFI_ENCTYPE_WPA_TKIP      = 0x03, 
    INFT_WIFI_ENCTYPE_WPA_AES       = 0x04, 
    INFT_WIFI_ENCTYPE_WPA2_TKIP     = 0x05, 
    INFT_WIFI_ENCTYPE_WPA2_AES      = 0x06,
    INFT_WIFI_ENCTYPE_WPA_PSK_TKIP  = 0x07,
    INFT_WIFI_ENCTYPE_WPA_PSK_AES   = 0x08,
    INFT_WIFI_ENCTYPE_WPA2_PSK_TKIP = 0x09,
    INFT_WIFI_ENCTYPE_WPA2_PSK_AES  = 0x0A,

}INFT_WIFI_ENCTYPE;

typedef struct Inft_Wifi_Attr{
    INFT_WIFI_MODE mode;        

    unsigned int enable;

    INFT_WIFI_ENCTYPE enctype;  
    unsigned char ssid[32];      
    unsigned char password[32]; 
    unsigned char address[32];
    unsigned char frequency[32];
    float bitRates;
    int   quality;
    struct Inft_Wifi_Attr* next;

}Inft_Wifi_Attr;

#endif
