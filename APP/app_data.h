#ifndef __APP_DATA_H
#define __APP_DATA_H

#include "sys.h"

// ========== 数据结构定义 ==========

// GPS数据结构
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
    float hdop;             // 水平精度因子

    // 时间信息
    u8 hour;
    u8 minute;
    u8 second;
    u16 year;
    u8 month;
    u8 day;
} GPS_Data;

// MPU6050数据结构
typedef struct
{
    short accel_x;          // X轴加速度 (原始值)
    short accel_y;          // Y轴加速度 (原始值)
    short accel_z;          // Z轴加速度 (原始值)

    short gyro_x;           // X轴角速度 (原始值)
    short gyro_y;           // Y轴角速度 (原始值)
    short gyro_z;           // Z轴角速度 (原始值)

    short temperature;      // 温度 (原始值)

    float accel_x_g;        // X轴加速度 (g)
    float accel_y_g;        // Y轴加速度 (g)
    float accel_z_g;        // Z轴加速度 (g)

    float pitch;            // 俯仰角 (度)
    float roll;             // 横滚角 (度)
    float yaw;              // 航向角 (度)
} MPU_Data;

// 运动状态枚举
typedef enum
{
    MOTION_IDLE = 0,        // 静止
    MOTION_WALKING,         // 行走
    MOTION_RUNNING,         // 跑步
    MOTION_CYCLING,         // 骑行
    MOTION_TURNING          // 转弯
} Motion_State;

// 轨迹点数据结构
typedef struct
{
    u32 timestamp;          // 时间戳 (秒)
    float latitude;         // 纬度
    float longitude;        // 经度
    float speed;            // 速度 (km/h)
    float altitude;         // 海拔 (m)
    Motion_State state;     // 运动状态
} Track_Point;

// 统计数据结构
typedef struct
{
    u8 is_recording;        // 是否正在记录 (1=记录中, 0=停止)
    u32 record_time;        // 记录时间 (秒)
    float total_distance;   // 总里程 (km)
    float avg_speed;        // 平均速度 (km/h)
    float max_speed;        // 最大速度 (km/h)
    u32 point_count;        // 记录点数量
    Motion_State current_state;  // 当前运动状态
} Statistics_Data;

// 全局数据变量声明
extern GPS_Data g_gps_data;
extern MPU_Data g_mpu_data;
extern Statistics_Data g_statistics;

// ========== 函数声明 ==========

void App_Data_Init(void);           // 数据初始化
Motion_State Motion_State_Detect(void);  // 运动状态检测

#endif
