#include "app_mpu.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "delay.h"

// Global MPU data
App_MPU_Data g_mpu_data = {0};

/**
 * @brief Initialize MPU6050 module
 */
void App_MPU_Init(void)
{
    u8 res;

    // Initialize MPU6050
    res = MPU_Init();
    if(res != 0)
    {
        g_mpu_data.valid = 0;
        return;
    }

    // Initialize DMP
    res = mpu_dmp_init();
    if(res != 0)
    {
        g_mpu_data.valid = 0;
        return;
    }

    g_mpu_data.valid = 1;
}

/**
 * @brief Update MPU6050 data
 */
void App_MPU_Update(void)
{
    float pitch, roll, yaw;
    short temp;

    if(g_mpu_data.valid == 0)
        return;

    // Get DMP data (pitch, roll, yaw)
    if(mpu_dmp_get_data(&pitch, &roll, &yaw) == 0)
    {
        // Get temperature
        temp = MPU_Get_Temperature();
        g_mpu_data.temperature = (float)temp / 100.0f;

        // Get accelerometer data
        MPU_Get_Accelerometer(&g_mpu_data.accel_x, &g_mpu_data.accel_y, &g_mpu_data.accel_z);

        // Get gyroscope data
        MPU_Get_Gyroscope(&g_mpu_data.gyro_x, &g_mpu_data.gyro_y, &g_mpu_data.gyro_z);

        // Store attitude angles
        g_mpu_data.pitch = pitch;
        g_mpu_data.roll = roll;
        g_mpu_data.yaw = yaw;
    }
}
