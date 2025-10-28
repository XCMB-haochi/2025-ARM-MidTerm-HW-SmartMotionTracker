#include "app_fusion.h"
#include <math.h>

#define DELTA_T     0.001f  // 1ms update rate
#define M_PI        3.14159265f
#define ALPHA       0.3f    // Low-pass filter coefficient

// Global variables
float fusion_Kp = 0.17f;   // Proportional gain
float fusion_Ki = 0.004f;  // Integral gain

static Quaternion q = {1, 0, 0, 0};  // Global quaternion
static float I_ex = 0, I_ey = 0, I_ez = 0;  // Integral error
static GyroOffset gyro_offset = {0, 0, 0, 0};
static u16 calibration_count = 0;

/**
 * @brief Fast inverse square root (Quake III algorithm)
 */
float App_Fusion_InvSqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/**
 * @brief Initialize fusion algorithm
 */
void App_Fusion_Init(void)
{
    q.q0 = 1.0f;
    q.q1 = 0.0f;
    q.q2 = 0.0f;
    q.q3 = 0.0f;

    I_ex = I_ey = I_ez = 0.0f;

    gyro_offset.x = 0.0f;
    gyro_offset.y = 0.0f;
    gyro_offset.z = 0.0f;
    gyro_offset.calibrated = 0;
    calibration_count = 0;
}

/**
 * @brief Calibrate gyroscope offset (call multiple times during startup)
 * @param gx, gy, gz: Raw gyroscope data
 */
void App_Fusion_CalibrateGyro(short gx, short gy, short gz)
{
    if(calibration_count < 100)
    {
        gyro_offset.x += (float)gx;
        gyro_offset.y += (float)gy;
        gyro_offset.z += (float)gz;
        calibration_count++;

        if(calibration_count == 100)
        {
            gyro_offset.x /= 100.0f;
            gyro_offset.y /= 100.0f;
            gyro_offset.z /= 100.0f;
            gyro_offset.calibrated = 1;
        }
    }
}

/**
 * @brief Complementary filter for IMU data fusion
 * @param ax, ay, az: Accelerometer raw data
 * @param gx, gy, gz: Gyroscope raw data
 * @param angles: Output Euler angles
 */
void App_Fusion_UpdateIMU(short ax, short ay, short az, short gx, short gy, short gz, EulerAngles *angles)
{
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float halfT = 0.5f * DELTA_T;

    // Convert gyroscope to rad/s (MPU6050: 16.4 LSB/(°/s) at ±2000°/s)
    float gx_rad = ((float)gx - gyro_offset.x) * M_PI / 180.0f / 16.4f;
    float gy_rad = ((float)gy - gyro_offset.y) * M_PI / 180.0f / 16.4f;
    float gz_rad = ((float)gz - gyro_offset.z) * M_PI / 180.0f / 16.4f;

    // Convert accelerometer to g (MPU6050: ±2g range, 16384 LSB/g)
    float ax_g = (float)ax / 16384.0f;
    float ay_g = (float)ay / 16384.0f;
    float az_g = (float)az / 16384.0f;

    // Normalize accelerometer
    norm = App_Fusion_InvSqrt(ax_g * ax_g + ay_g * ay_g + az_g * az_g);
    ax_g *= norm;
    ay_g *= norm;
    az_g *= norm;

    // Estimated gravity direction from quaternion
    vx = 2.0f * (q.q1 * q.q3 - q.q0 * q.q2);
    vy = 2.0f * (q.q0 * q.q1 + q.q2 * q.q3);
    vz = q.q0 * q.q0 - q.q1 * q.q1 - q.q2 * q.q2 + q.q3 * q.q3;

    // Error is cross product between estimated and measured gravity
    ex = ay_g * vz - az_g * vy;
    ey = az_g * vx - ax_g * vz;
    ez = ax_g * vy - ay_g * vx;

    // Integral error
    I_ex += halfT * ex;
    I_ey += halfT * ey;
    I_ez += halfT * ez;

    // Apply PI feedback to gyroscope
    gx_rad += fusion_Kp * ex + fusion_Ki * I_ex;
    gy_rad += fusion_Kp * ey + fusion_Ki * I_ey;
    gz_rad += fusion_Kp * ez + fusion_Ki * I_ez;

    // Integrate quaternion rate (first-order Runge-Kutta)
    q.q0 += (-q.q1 * gx_rad - q.q2 * gy_rad - q.q3 * gz_rad) * halfT;
    q.q1 += ( q.q0 * gx_rad + q.q2 * gz_rad - q.q3 * gy_rad) * halfT;
    q.q2 += ( q.q0 * gy_rad - q.q1 * gz_rad + q.q3 * gx_rad) * halfT;
    q.q3 += ( q.q0 * gz_rad + q.q1 * gy_rad - q.q2 * gx_rad) * halfT;

    // Normalize quaternion
    norm = App_Fusion_InvSqrt(q.q0 * q.q0 + q.q1 * q.q1 + q.q2 * q.q2 + q.q3 * q.q3);
    q.q0 *= norm;
    q.q1 *= norm;
    q.q2 *= norm;
    q.q3 *= norm;

    // Convert quaternion to Euler angles
    angles->pitch = asinf(-2.0f * q.q1 * q.q3 + 2.0f * q.q0 * q.q2) * 180.0f / M_PI;
    angles->roll = atan2f(2.0f * q.q2 * q.q3 + 2.0f * q.q0 * q.q1,
                          -2.0f * q.q1 * q.q1 - 2.0f * q.q2 * q.q2 + 1.0f) * 180.0f / M_PI;
    angles->yaw = atan2f(2.0f * q.q1 * q.q2 + 2.0f * q.q0 * q.q3,
                         -2.0f * q.q2 * q.q2 - 2.0f * q.q3 * q.q3 + 1.0f) * 180.0f / M_PI;

    // Attitude limits
    if(angles->roll > 90.0f || angles->roll < -90.0f)
    {
        if(angles->pitch > 0)
            angles->pitch = 180.0f - angles->pitch;
        else
            angles->pitch = -(180.0f + angles->pitch);
    }

    // Normalize yaw to 0-360
    if(angles->yaw > 360.0f)
        angles->yaw -= 360.0f;
    else if(angles->yaw < 0.0f)
        angles->yaw += 360.0f;
}
