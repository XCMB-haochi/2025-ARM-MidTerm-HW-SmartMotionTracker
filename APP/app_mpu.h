#ifndef __APP_MPU_H
#define __APP_MPU_H

#include "sys.h"

// MPU6050 application data structure
typedef struct {
    u8 valid;                // 0: invalid, 1: valid
    float temperature;       // Temperature in Celsius
    short accel_x;          // Accelerometer X-axis raw data
    short accel_y;          // Accelerometer Y-axis raw data
    short accel_z;          // Accelerometer Z-axis raw data
    short gyro_x;           // Gyroscope X-axis raw data
    short gyro_y;           // Gyroscope Y-axis raw data
    short gyro_z;           // Gyroscope Z-axis raw data
    float pitch;            // Pitch angle (degrees)
    float roll;             // Roll angle (degrees)
    float yaw;              // Yaw angle (degrees)
} App_MPU_Data;

extern App_MPU_Data g_mpu_data;

// Function declarations
void App_MPU_Init(void);
void App_MPU_Update(void);

#endif
