#include "app_ui_main.h"
#include "app_gps.h"
#include "app_mpu.h"
#include "app_sdcard.h"
#include <stdio.h>

// UI objects
static lv_obj_t * tabview;
static lv_obj_t * tab_gps;
static lv_obj_t * tab_imu;
static lv_obj_t * tab_data;

// Status bar labels
static lv_obj_t * label_sd_status;
static lv_obj_t * label_gps_status;

// GPS page labels
static lv_obj_t * label_lat;
static lv_obj_t * label_lon;
static lv_obj_t * label_speed;
static lv_obj_t * label_alt;
static lv_obj_t * label_sats;
static lv_obj_t * label_time;

// IMU page labels
static lv_obj_t * label_pitch;
static lv_obj_t * label_roll;
static lv_obj_t * label_yaw;
static lv_obj_t * label_temp;

// Data page
static lv_obj_t * btn_start_log;
static lv_obj_t * btn_stop_log;
static lv_obj_t * label_record_count;

// Button event handler
static void btn_start_log_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        App_SDCard_StartLog();
    }
}

static void btn_stop_log_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        App_SDCard_StopLog();
    }
}

/**
 * @brief Create main UI with tabview
 */
void App_UI_Main_Create(void)
{
    // Create screen
    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    // ========== Status Bar (Top) ==========
    lv_obj_t * status_bar = lv_cont_create(scr, NULL);
    lv_obj_set_size(status_bar, 480, 40);
    lv_obj_align(status_bar, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    static lv_style_t status_style;
    lv_style_copy(&status_style, &lv_style_plain);
    status_style.body.main_color = LV_COLOR_BLUE;
    status_style.body.grad_color = LV_COLOR_BLUE;
    status_style.text.color = LV_COLOR_WHITE;
    lv_cont_set_style(status_bar, LV_CONT_STYLE_MAIN, &status_style);

    // SD status
    label_sd_status = lv_label_create(status_bar, NULL);
    lv_label_set_text(label_sd_status, "SD: --");
    lv_obj_align(label_sd_status, NULL, LV_ALIGN_IN_LEFT_MID, 10, 0);

    // GPS status
    label_gps_status = lv_label_create(status_bar, NULL);
    lv_label_set_text(label_gps_status, "GPS: Search");
    lv_label_set_long_mode(label_gps_status, LV_LABEL_LONG_EXPAND);  // Auto expand width
    lv_obj_align(label_gps_status, NULL, LV_ALIGN_IN_RIGHT_MID, -10, 0);

    // ========== Tabview ==========
    tabview = lv_tabview_create(scr, NULL);
    lv_obj_set_size(tabview, 480, 760);
    lv_obj_align(tabview, status_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    // Create tabs
    tab_gps = lv_tabview_add_tab(tabview, "GPS");
    tab_imu = lv_tabview_add_tab(tabview, "IMU");
    tab_data = lv_tabview_add_tab(tabview, "Data");

    // ========== GPS Tab ==========
    label_lat = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_lat, "Latitude:  --");
    lv_obj_align(label_lat, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);

    label_lon = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_lon, "Longitude: --");
    lv_obj_align(label_lon, label_lat, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    label_speed = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_speed, "Speed:     -- km/h");
    lv_obj_align(label_speed, label_lon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    label_alt = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_alt, "Altitude:  -- m");
    lv_obj_align(label_alt, label_speed, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    label_sats = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_sats, "Satellites: 0");
    lv_obj_align(label_sats, label_alt, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    label_time = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_time, "UTC Time:  --:--:--");
    lv_obj_align(label_time, label_sats, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    // ========== IMU Tab ==========
    label_pitch = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_pitch, "Pitch: -- deg");
    lv_obj_align(label_pitch, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);

    label_roll = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_roll, "Roll:  -- deg");
    lv_obj_align(label_roll, label_pitch, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    label_yaw = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_yaw, "Yaw:   -- deg");
    lv_obj_align(label_yaw, label_roll, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    label_temp = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_temp, "Temp:  -- C");
    lv_obj_align(label_temp, label_yaw, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);

    // ========== Data Tab ==========
    label_record_count = lv_label_create(tab_data, NULL);
    lv_label_set_text(label_record_count, "Records: 0");
    lv_obj_align(label_record_count, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);

    // Start button
    btn_start_log = lv_btn_create(tab_data, NULL);
    lv_obj_set_size(btn_start_log, 200, 60);
    lv_obj_align(btn_start_log, label_record_count, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    lv_obj_set_event_cb(btn_start_log, btn_start_log_event);

    lv_obj_t * label_start = lv_label_create(btn_start_log, NULL);
    lv_label_set_text(label_start, "Start Logging");

    // Stop button
    btn_stop_log = lv_btn_create(tab_data, NULL);
    lv_obj_set_size(btn_stop_log, 200, 60);
    lv_obj_align(btn_stop_log, btn_start_log, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_event_cb(btn_stop_log, btn_stop_log_event);

    lv_obj_t * label_stop = lv_label_create(btn_stop_log, NULL);
    lv_label_set_text(label_stop, "Stop Logging");
}

/**
 * @brief Update UI data
 */
void App_UI_Main_Update(void)
{
    char buf[100];

    // ========== Status Bar ==========
    if(g_sdcard_status.initialized == 0)
    {
        lv_label_set_text(label_sd_status, "SD: Error");
    }
    else if(g_sdcard_status.logging)
    {
        sprintf(buf, "SD: Log %d", g_sdcard_status.record_count);
        lv_label_set_text(label_sd_status, buf);
    }
    else
    {
        lv_label_set_text(label_sd_status, "SD: Ready");
    }

    if(g_gps_data.valid)
    {
        sprintf(buf, "GPS: %d sats", g_gps_data.satellites);
        lv_label_set_text(label_gps_status, buf);
    }
    else
    {
        lv_label_set_text(label_gps_status, "GPS: Search");
    }

    // ========== GPS Tab ==========
    if(g_gps_data.valid)
    {
        sprintf(buf, "Lat: %.6f %c", g_gps_data.latitude, g_gps_data.lat_dir);
        lv_label_set_text(label_lat, buf);

        sprintf(buf, "Lon: %.6f %c", g_gps_data.longitude, g_gps_data.lon_dir);
        lv_label_set_text(label_lon, buf);

        sprintf(buf, "Speed: %.2f km/h", g_gps_data.speed_kmh);
        lv_label_set_text(label_speed, buf);

        sprintf(buf, "Alt: %.1f m", g_gps_data.altitude);
        lv_label_set_text(label_alt, buf);

        sprintf(buf, "Sats: %d", g_gps_data.satellites);
        lv_label_set_text(label_sats, buf);

        u8 bj_hour = (g_gps_data.hour + 8) % 24;
        sprintf(buf, "UTC: %02d:%02d:%02d BJ:%02d:%02d",
                g_gps_data.hour, g_gps_data.minute, g_gps_data.second,
                bj_hour, g_gps_data.minute);
        lv_label_set_text(label_time, buf);
    }

    // ========== IMU Tab ==========
    if(g_mpu_data.valid)
    {
        sprintf(buf, "Pitch: %.2f deg", g_mpu_data.pitch);
        lv_label_set_text(label_pitch, buf);

        sprintf(buf, "Roll:  %.2f deg", g_mpu_data.roll);
        lv_label_set_text(label_roll, buf);

        sprintf(buf, "Yaw:   %.2f deg", g_mpu_data.yaw);
        lv_label_set_text(label_yaw, buf);

        sprintf(buf, "Temp:  %.1f C", g_mpu_data.temperature);
        lv_label_set_text(label_temp, buf);
    }

    // ========== Data Tab ==========
    sprintf(buf, "Records: %d", g_sdcard_status.record_count);
    lv_label_set_text(label_record_count, buf);
}

/**
 * @brief Switch to specific page
 */
void App_UI_Main_SwitchPage(UI_Page page)
{
    lv_tabview_set_tab_act(tabview, page, true);
}
