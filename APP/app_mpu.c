#include "app_mpu.h"
#include "app_fusion.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "delay.h"

// Use fusion algorithm instead of DMP (0: DMP, 1: Fusion)
#define USE_FUSION_ALGORITHM  0

// Global MPU data
App_MPU_Data g_mpu_data = {0};
static u16 calibration_timer = 0;

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

#if USE_FUSION_ALGORITHM
    // Initialize fusion algorithm
    App_Fusion_Init();
    calibration_timer = 0;
    g_mpu_data.valid = 1;
#else
    // Initialize DMP
    res = mpu_dmp_init();
    if(res != 0)
    {
        g_mpu_data.valid = 0;
        return;
    }
    g_mpu_data.valid = 1;
#endif
}

/**
 * @brief Update MPU6050 data
 */
void App_MPU_Update(void)
{
    short temp;

    if(g_mpu_data.valid == 0)
        return;

    // Get temperature
    temp = MPU_Get_Temperature();
    g_mpu_data.temperature = (float)temp / 100.0f;

    // Get accelerometer data
    MPU_Get_Accelerometer(&g_mpu_data.accel_x, &g_mpu_data.accel_y, &g_mpu_data.accel_z);

    // Get gyroscope data
    MPU_Get_Gyroscope(&g_mpu_data.gyro_x, &g_mpu_data.gyro_y, &g_mpu_data.gyro_z);

#if USE_FUSION_ALGORITHM
    // Calibrate gyroscope during first 100 samples
    if(calibration_timer < 100)
    {
        App_Fusion_CalibrateGyro(g_mpu_data.gyro_x, g_mpu_data.gyro_y, g_mpu_data.gyro_z);
        calibration_timer++;

        // Set default angles during calibration
        g_mpu_data.pitch = 0;
        g_mpu_data.roll = 0;
        g_mpu_data.yaw = 0;
    }
    else
    {
        // Use fusion algorithm
        EulerAngles angles;
        App_Fusion_UpdateIMU(g_mpu_data.accel_x, g_mpu_data.accel_y, g_mpu_data.accel_z,
                             g_mpu_data.gyro_x, g_mpu_data.gyro_y, g_mpu_data.gyro_z,
                             &angles);

        g_mpu_data.pitch = angles.pitch;
        g_mpu_data.roll = angles.roll;
        g_mpu_data.yaw = angles.yaw;
    }
#else
    // Use DMP
    float pitch, roll, yaw;
    if(mpu_dmp_get_data(&pitch, &roll, &yaw) == 0)
    {
        g_mpu_data.pitch = pitch;
        g_mpu_data.roll = roll;
        g_mpu_data.yaw = yaw;
    }
#endif
}
