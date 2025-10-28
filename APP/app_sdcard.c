#include "app_sdcard.h"
#include "app_gps.h"
#include "app_mpu.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "exfuns.h"
#include "malloc.h"
#include <stdio.h>
#include <string.h>

App_SDCard_Status g_sdcard_status = {0};

static FIL log_file;
static char log_filename[32];

/**
 * @brief Initialize SD card and FATFS
 */
void App_SDCard_Init(void)
{
    u8 res;

    g_sdcard_status.initialized = 0;
    g_sdcard_status.logging = 0;
    g_sdcard_status.record_count = 0;

    // Initialize SD card
    res = SD_Init();
    if(res != 0)
    {
        return;  // SD card init failed
    }

    // Initialize memory for FATFS
    exfuns_init();

    // Mount SD card (fs[0] is for SD card)
    res = f_mount(fs[0], "0:", 1);
    if(res != FR_OK)
    {
        return;  // Mount failed
    }

    g_sdcard_status.initialized = 1;
}

/**
 * @brief Start data logging
 */
void App_SDCard_StartLog(void)
{
    FRESULT res;
    UINT bw;
    char header[200];

    if(g_sdcard_status.initialized == 0)
        return;

    // Generate log filename with timestamp
    sprintf(log_filename, "0:/data_%03d.csv", g_sdcard_status.record_count / 1000);

    // Open file for append
    res = f_open(&log_file, log_filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(res != FR_OK)
    {
        return;  // Failed to create file
    }

    // Write CSV header
    sprintf(header, "Time,Lat,LatDir,Lon,LonDir,Speed,Alt,Sats,Pitch,Roll,Yaw,Temp\r\n");
    f_write(&log_file, header, strlen(header), &bw);
    f_sync(&log_file);  // Flush to SD card

    g_sdcard_status.logging = 1;
    g_sdcard_status.record_count = 0;
}

/**
 * @brief Stop data logging
 */
void App_SDCard_StopLog(void)
{
    if(g_sdcard_status.logging == 0)
        return;

    f_close(&log_file);
    g_sdcard_status.logging = 0;
}

/**
 * @brief Write one record to SD card
 */
void App_SDCard_WriteData(void)
{
    FRESULT res;
    UINT bw;
    char buffer[200];

    if(g_sdcard_status.logging == 0)
        return;

    // Format data as CSV
    sprintf(buffer, "%02d:%02d:%02d,%.6f,%c,%.6f,%c,%.2f,%.1f,%d,%.2f,%.2f,%.2f,%.1f\r\n",
            g_gps_data.hour, g_gps_data.minute, g_gps_data.second,
            g_gps_data.latitude, g_gps_data.lat_dir,
            g_gps_data.longitude, g_gps_data.lon_dir,
            g_gps_data.speed_kmh, g_gps_data.altitude, g_gps_data.satellites,
            g_mpu_data.pitch, g_mpu_data.roll, g_mpu_data.yaw,
            g_mpu_data.temperature);

    // Write to file
    res = f_write(&log_file, buffer, strlen(buffer), &bw);
    if(res == FR_OK)
    {
        g_sdcard_status.record_count++;

        // Sync every 10 records to prevent data loss
        if(g_sdcard_status.record_count % 10 == 0)
        {
            f_sync(&log_file);
        }
    }
}
