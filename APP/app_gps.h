#ifndef __APP_GPS_H
#define __APP_GPS_H

#include "sys.h"
#include "gps.h"

// GPS数据结构(简化版，便于UI显示)
typedef struct
{
    u8 valid;                // GPS数据有效标志 (1=有效, 0=无效)
    float latitude;          // 纬度 (度)
    char lat_dir;           // 纬度方向 ('N'=北, 'S'=南)
    float longitude;         // 经度 (度)
    char lon_dir;           // 经度方向 ('E'=东, 'W'=西)
    float speed_kmh;        // 速度 (km/h)
    float altitude;         // 海拔 (米)
    u8 satellites;          // 卫星数量
    u16 hdop;               // 水平精度因子

    // 时间信息
    u8 hour;
    u8 minute;
    u8 second;
    u16 year;
    u8 month;
    u8 day;
} App_GPS_Data;

// 全局GPS数据
extern App_GPS_Data g_gps_data;
extern nmea_msg gpsx;  // 原始GPS数据

// 函数声明
void App_GPS_Init(void);           // GPS初始化
void App_GPS_Update(void);         // GPS数据更新(在主循环中调用)
void App_GPS_Parse(void);          // GPS数据解析

#endif
