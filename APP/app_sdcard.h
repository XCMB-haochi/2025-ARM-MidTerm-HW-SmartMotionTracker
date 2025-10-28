#ifndef __APP_SDCARD_H
#define __APP_SDCARD_H

#include "sys.h"

// SD card status
typedef struct {
    u8 initialized;     // 0: not ready, 1: ready
    u8 logging;         // 0: not logging, 1: logging
    u32 record_count;   // Total number of records written
} App_SDCard_Status;

extern App_SDCard_Status g_sdcard_status;

// Function declarations
void App_SDCard_Init(void);
void App_SDCard_StartLog(void);
void App_SDCard_StopLog(void);
void App_SDCard_WriteData(void);

#endif
