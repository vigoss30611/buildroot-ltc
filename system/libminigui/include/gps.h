#ifndef GPS_H
#define GPS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double second;
} gps_utc_time;

typedef struct {
    gps_utc_time gut;
    double latitude;
    double longitude;
//    double altitude;
    double speed;
} gps_data;

//获取包括时间，位置，速度所有数据
int gps_request(gps_data *gd);

//获取当前utc时间，包括yymmdd hhmmss
//int gps_request_utc_time(gps_utc_time *gut);

//获取当前位置，包括纬度(degrees)，经度(degrees)和海拔高度(meters)
//int gps_request_location(double *latitude, double *longitude, double *altitude);

//获取当前速度(meters per second)
//int gps_request_speed(float *speed);

//通过几次location（经度和纬度）的变化来得到平均方位(degrees)
//int gps_request_bearing(float *bearing);

#ifdef __cplusplus
}
#endif

#endif

