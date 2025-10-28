#ifndef __APP_FUSION_H
#define __APP_FUSION_H

#include "sys.h"

// Quaternion structure
typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion;

// Euler angles structure
typedef struct {
    float pitch;
    float roll;
    float yaw;
} EulerAngles;

// Gyroscope offset calibration
typedef struct {
    float x;
    float y;
    float z;
    u8 calibrated;
} GyroOffset;

// Fusion algorithm parameters
extern float fusion_Kp;  // Proportional gain
extern float fusion_Ki;  // Integral gain

// Function declarations
void App_Fusion_Init(void);
void App_Fusion_CalibrateGyro(short gx, short gy, short gz);
void App_Fusion_UpdateIMU(short ax, short ay, short az, short gx, short gy, short gz, EulerAngles *angles);
float App_Fusion_InvSqrt(float x);

#endif
