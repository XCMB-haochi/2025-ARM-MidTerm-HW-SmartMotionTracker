#include "app_gps.h"
#include "usart3.h"
#include "delay.h"
#include <string.h>

// 全局变量定义
App_GPS_Data g_gps_data;
nmea_msg gpsx;  // 原始GPS数据结构

/**
 * GPS模块初始化
 */
void App_GPS_Init(void)
{
    // 初始化串口3 (GPS通信)
    usart3_init(38400);  // GPS模块默认波特率38400

    // 清空GPS数据
    memset(&g_gps_data, 0, sizeof(App_GPS_Data));
    memset(&gpsx, 0, sizeof(nmea_msg));

    delay_ms(100);
}

/**
 * GPS数据解析 - 从原始nmea_msg转换到App_GPS_Data
 */
void App_GPS_Parse(void)
{
    // 检查GPS定位状态
    if(gpsx.gpssta >= 1)  // 1=非差分定位, 2=差分定位
    {
        g_gps_data.valid = 1;

        // 只有当经纬度不为0时才更新（防止部分解析导致数据清零）
        if(gpsx.latitude != 0)
        {
            // 转换纬度 (原始数据放大了100000倍)
            g_gps_data.latitude = (float)gpsx.latitude / 100000.0f;
            g_gps_data.lat_dir = gpsx.nshemi;
        }

        if(gpsx.longitude != 0)
        {
            // 转换经度
            g_gps_data.longitude = (float)gpsx.longitude / 100000.0f;
            g_gps_data.lon_dir = gpsx.ewhemi;
        }

        // 转换速度 (原始数据放大了1000倍，单位：0.001节/小时)
        // 1节 = 1.852公里/小时
        g_gps_data.speed_kmh = (float)gpsx.speed / 1000.0f * 1.852f;

        // 转换海拔 (原始数据放大了10倍)
        g_gps_data.altitude = (float)gpsx.altitude / 10.0f;

        // 卫星数量
        g_gps_data.satellites = gpsx.posslnum;

        // 水平精度因子
        g_gps_data.hdop = gpsx.hdop;

        // 时间信息
        g_gps_data.year = gpsx.utc.year;
        g_gps_data.month = gpsx.utc.month;
        g_gps_data.day = gpsx.utc.date;
        g_gps_data.hour = gpsx.utc.hour;
        g_gps_data.minute = gpsx.utc.min;
        g_gps_data.second = gpsx.utc.sec;
    }
    else
    {
        g_gps_data.valid = 0;  // GPS未定位
        // 保留卫星数量信息
        g_gps_data.satellites = gpsx.posslnum;
    }
}

/**
 * GPS数据更新 - 在主循环中调用
 * 说明：检查USART3接收缓冲区，如果有数据则解析
 */
void App_GPS_Update(void)
{
    u16 rxlen;
    u16 i;
    static u8 gps_buf[USART3_MAX_RECV_LEN];

    // 检查是否接收到数据
    if(USART3_RX_STA & 0x8000)  // bit15=1 表示接收到完整一帧数据
    {
        rxlen = USART3_RX_STA & 0x7FFF;  // 获取数据长度

        // 复制数据到临时缓冲区
        for(i = 0; i < rxlen; i++)
        {
            gps_buf[i] = USART3_RX_BUF[i];
        }
        gps_buf[i] = 0;  // 添加结束符

        // 清除接收标志
        USART3_RX_STA = 0;

        // 解析GPS数据
        GPS_Analysis(&gpsx, gps_buf);

        // 转换到应用层数据结构
        App_GPS_Parse();
    }
    else if(USART3_RX_STA > 0)  // 有数据但未收完整
    {
        // 如果数据超过一定长度，强制解析（防止数据不完整卡住）
        if(USART3_RX_STA > 100)
        {
            rxlen = USART3_RX_STA;
            for(i = 0; i < rxlen; i++)
            {
                gps_buf[i] = USART3_RX_BUF[i];
            }
            gps_buf[i] = 0;

            USART3_RX_STA = 0;
            GPS_Analysis(&gpsx, gps_buf);
            App_GPS_Parse();
        }
    }
}
