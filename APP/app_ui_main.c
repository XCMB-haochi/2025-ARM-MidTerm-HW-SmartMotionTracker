#include "app_ui_main.h"
#include "app_gps.h"
#include "app_mpu.h"
#include "app_sdcard.h"
#include "ff.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

// UI objects
static lv_obj_t * tabview;
static lv_obj_t * tab_home;
static lv_obj_t * tab_gps;
static lv_obj_t * tab_imu;
static lv_obj_t * tab_files;  // Changed from tab_data to tab_files
static lv_obj_t * main_screen;
static lv_obj_t * status_bar;

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
static lv_obj_t * btn_record_toggle;  // Combined Start/Stop button
static lv_obj_t * btn_dark_mode;
static lv_obj_t * btn_browse_data;
static lv_obj_t * btn_clear_data;
static lv_obj_t * label_record_count;
static lv_obj_t * label_motion_status;
static lv_obj_t * label_track_info;
static lv_obj_t * label_last_record;  // Preview last recorded data
static lv_obj_t * label_statistics;    // Statistics display
static lv_obj_t * label_browse_data;   // Browse data display
static lv_obj_t * label_record_btn;    // Label for record toggle button

// Dark mode button labels (for updating text)
static lv_obj_t * label_dark_home;
static lv_obj_t * label_dark_gps;
static lv_obj_t * label_dark_imu;
static lv_obj_t * label_dark_files;

// Button styles
static lv_style_t style_btn_green;
static lv_style_t style_btn_red;
static lv_style_t style_btn_dark;
static lv_style_t style_dark_bg;
static lv_style_t style_dark_statusbar;
static lv_style_t style_dark_tab;
static lv_style_t style_light_tab;

// Dark mode flag
static u8 dark_mode_enabled = 0;

// Statistics tracking
static float max_speed = 0.0f;
static float max_altitude = 0.0f;
static float total_distance = 0.0f;
static float last_lat = 0.0f;
static float last_lon = 0.0f;
static u8 stats_initialized = 0;

// Button event handler
static void btn_record_toggle_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        if(g_sdcard_status.logging)
        {
            // Currently logging, stop it
            App_SDCard_StopLog();
            lv_label_set_text(label_record_btn, "Start Log");
            lv_btn_set_style(btn_record_toggle, LV_BTN_STYLE_REL, &style_btn_green);
        }
        else
        {
            // Not logging, start it
            App_SDCard_StartLog();
            lv_label_set_text(label_record_btn, "Stop Log");
            lv_btn_set_style(btn_record_toggle, LV_BTN_STYLE_REL, &style_btn_red);

            // Reset statistics when starting new log
            max_speed = 0.0f;
            max_altitude = 0.0f;
            total_distance = 0.0f;
            stats_initialized = 0;
        }
    }
}

static void btn_dark_mode_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        dark_mode_enabled = !dark_mode_enabled;

        if(dark_mode_enabled)
        {
            // ===== Dark Mode =====
            // Background: Pure black
            lv_style_copy(&style_dark_bg, &lv_style_plain);
            style_dark_bg.body.main_color = LV_COLOR_MAKE(0, 0, 0);
            style_dark_bg.body.grad_color = LV_COLOR_MAKE(0, 0, 0);
            style_dark_bg.text.color = LV_COLOR_WHITE;

            // Status bar: Dark gray
            lv_style_copy(&style_dark_statusbar, &lv_style_plain);
            style_dark_statusbar.body.main_color = LV_COLOR_MAKE(32, 32, 32);
            style_dark_statusbar.body.grad_color = LV_COLOR_MAKE(32, 32, 32);
            style_dark_statusbar.text.color = LV_COLOR_WHITE;

            // Tabs: Dark gray background with white text
            lv_style_copy(&style_dark_tab, &lv_style_plain);
            style_dark_tab.body.main_color = LV_COLOR_MAKE(18, 18, 18);
            style_dark_tab.body.grad_color = LV_COLOR_MAKE(18, 18, 18);
            style_dark_tab.text.color = LV_COLOR_MAKE(230, 230, 230);
            style_dark_tab.body.padding.top = 10;
            style_dark_tab.body.padding.bottom = 10;
            style_dark_tab.body.padding.left = 10;
            style_dark_tab.body.padding.right = 10;
            style_dark_tab.body.padding.inner = 10;

            lv_obj_set_style(main_screen, &style_dark_bg);
            lv_obj_set_style(status_bar, &style_dark_statusbar);
            lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BG, &style_dark_tab);
            lv_obj_set_style(tab_home, &style_dark_tab);
            lv_obj_set_style(tab_gps, &style_dark_tab);
            lv_obj_set_style(tab_imu, &style_dark_tab);
            lv_obj_set_style(tab_files, &style_dark_tab);

            // Update button labels
            lv_label_set_text(label_dark_home, "Light");
            lv_label_set_text(label_dark_gps, "Light");
            lv_label_set_text(label_dark_imu, "Light");
            lv_label_set_text(label_dark_files, "Light");
        }
        else
        {
            // ===== Light Mode =====
            lv_obj_set_style(main_screen, &lv_style_scr);

            // Status bar: Blue
            static lv_style_t light_status;
            lv_style_copy(&light_status, &lv_style_plain);
            light_status.body.main_color = LV_COLOR_BLUE;
            light_status.body.grad_color = LV_COLOR_BLUE;
            light_status.text.color = LV_COLOR_WHITE;
            lv_obj_set_style(status_bar, &light_status);

            // Tabs: Light background
            lv_style_copy(&style_light_tab, &lv_style_plain);
            style_light_tab.body.main_color = LV_COLOR_WHITE;
            style_light_tab.body.grad_color = LV_COLOR_WHITE;
            style_light_tab.text.color = LV_COLOR_BLACK;
            style_light_tab.body.padding.top = 10;
            style_light_tab.body.padding.bottom = 10;
            style_light_tab.body.padding.left = 10;
            style_light_tab.body.padding.right = 10;
            style_light_tab.body.padding.inner = 10;

            lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BG, &style_light_tab);
            lv_obj_set_style(tab_home, &style_light_tab);
            lv_obj_set_style(tab_gps, &style_light_tab);
            lv_obj_set_style(tab_imu, &style_light_tab);
            lv_obj_set_style(tab_files, &style_light_tab);

            // Update button labels
            lv_label_set_text(label_dark_home, "Dark");
            lv_label_set_text(label_dark_gps, "Dark");
            lv_label_set_text(label_dark_imu, "Dark");
            lv_label_set_text(label_dark_files, "Dark");
        }
    }
}

static void btn_browse_data_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        FIL test_file;
        FRESULT fres;
        char filename[32];
        char display[800];
        u16 file_count = 0;

        // Check if SD card is initialized
        if(g_sdcard_status.initialized == 0)
        {
            lv_label_set_text(label_browse_data, "SD Card Error!\nCannot access files");
            return;
        }

        // Scan for existing files
        strcpy(display, "Files on SD Card:\n\n");

        for(u16 i = 0; i < 100; i++)
        {
            sprintf(filename, "0:/data_%03d.csv", i);
            fres = f_open(&test_file, filename, FA_READ);
            if(fres == FR_OK)
            {
                // File exists, get size
                DWORD file_size = f_size(&test_file);
                f_close(&test_file);

                char line[80];
                sprintf(line, "data_%03d.csv  (%lu bytes)\n", i, file_size);
                strcat(display, line);
                file_count++;

                // Limit display to 10 files to avoid overflow
                if(file_count >= 10)
                {
                    strcat(display, "\n... (more files exist)");
                    break;
                }
            }
        }

        // Show current recording status
        if(g_sdcard_status.logging)
        {
            char status[80];
            sprintf(status, "\n[Recording Now]\nCurrent: %d records", g_sdcard_status.record_count);
            strcat(display, status);
        }

        if(file_count == 0)
        {
            strcpy(display, "No files found\n\nPress 'Start Log' to begin\nrecording GPS+IMU data");
        }

        lv_label_set_text(label_browse_data, display);
    }
}

static void btn_clear_data_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        if(App_SDCard_ClearAllData() == 0)
        {
            lv_label_set_text(label_browse_data, "All data cleared!");
            // Reset statistics
            max_speed = 0.0f;
            max_altitude = 0.0f;
            total_distance = 0.0f;
            stats_initialized = 0;
        }
        else
        {
            lv_label_set_text(label_browse_data, "Failed to clear data");
        }
    }
}

/**
 * @brief Create main UI with tabview
 */
void App_UI_Main_Create(void)
{
    // Create screen
    main_screen = lv_obj_create(NULL, NULL);
    lv_scr_load(main_screen);

    // ========== Status Bar (Top) ==========
    status_bar = lv_cont_create(main_screen, NULL);
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
    tabview = lv_tabview_create(main_screen, NULL);
    lv_obj_set_size(tabview, 480, 760);
    lv_obj_align(tabview, status_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    // Create tabs
    tab_home = lv_tabview_add_tab(tabview, "Home");
    tab_gps = lv_tabview_add_tab(tabview, "GPS");
    tab_imu = lv_tabview_add_tab(tabview, "IMU");
    tab_files = lv_tabview_add_tab(tabview, "Files");

    // Initialize button styles (before creating any buttons)
    lv_style_copy(&style_btn_green, &lv_style_plain);
    style_btn_green.body.main_color = LV_COLOR_GREEN;
    style_btn_green.body.grad_color = LV_COLOR_GREEN;
    style_btn_green.body.radius = 5;
    style_btn_green.text.color = LV_COLOR_WHITE;

    lv_style_copy(&style_btn_red, &lv_style_plain);
    style_btn_red.body.main_color = LV_COLOR_RED;
    style_btn_red.body.grad_color = LV_COLOR_RED;
    style_btn_red.body.radius = 5;
    style_btn_red.text.color = LV_COLOR_WHITE;

    lv_style_copy(&style_btn_dark, &lv_style_plain);
    style_btn_dark.body.main_color = LV_COLOR_MAKE(50, 50, 50);
    style_btn_dark.body.grad_color = LV_COLOR_MAKE(30, 30, 30);
    style_btn_dark.body.radius = 5;
    style_btn_dark.text.color = LV_COLOR_WHITE;

    // ========== Home Tab ==========
    // Title with style
    static lv_style_t style_title;
    lv_style_copy(&style_title, &lv_style_plain);
    style_title.text.font = &lv_font_roboto_28;
    style_title.text.color = LV_COLOR_MAKE(0, 120, 215);  // Blue

    lv_obj_t * home_title = lv_label_create(tab_home, NULL);
    lv_label_set_text(home_title, "Smart Motion Tracker");
    lv_label_set_style(home_title, LV_LABEL_STYLE_MAIN, &style_title);
    lv_obj_align(home_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    // Product description
    static lv_style_t style_desc;
    lv_style_copy(&style_desc, &lv_style_plain);
    style_desc.text.font = &lv_font_roboto_16;
    style_desc.text.color = LV_COLOR_MAKE(60, 60, 60);

    lv_obj_t * home_desc = lv_label_create(tab_home, NULL);
    lv_label_set_text(home_desc, "Vehicle Motion Data Logger");
    lv_label_set_style(home_desc, LV_LABEL_STYLE_MAIN, &style_desc);
    lv_obj_align(home_desc, home_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // Feature list with icons (using text symbols)
    lv_obj_t * feature_title = lv_label_create(tab_home, NULL);
    lv_label_set_text(feature_title, "Features:");
    lv_label_set_style(feature_title, LV_LABEL_STYLE_MAIN, &style_title);
    lv_obj_set_pos(feature_title, 30, 100);

    lv_obj_t * features = lv_label_create(tab_home, NULL);
    lv_label_set_text(features,
        "> GPS Position Tracking\n"
        "  Real-time latitude, longitude, speed\n\n"
        "> Motion Sensing (MPU6050)\n"
        "  Pitch, Roll, Yaw angles\n\n"
        "> Data Recording to SD Card\n"
        "  CSV format, easy to analyze\n\n"
        "> On-Screen Data Browsing\n"
        "  No card reader needed!"
    );
    lv_obj_set_pos(features, 40, 135);

    // Quick start guide
    lv_obj_t * guide_title = lv_label_create(tab_home, NULL);
    lv_label_set_text(guide_title, "Quick Start:");
    lv_label_set_style(guide_title, LV_LABEL_STYLE_MAIN, &style_title);
    lv_obj_set_pos(guide_title, 30, 380);

    lv_obj_t * guide = lv_label_create(tab_home, NULL);
    lv_label_set_text(guide,
        "1. Check GPS status (top-right)\n"
        "2. Go to 'Data' tab\n"
        "3. Press 'Start Log' button\n"
        "4. Move/Drive around\n"
        "5. Press 'Browse' to view data"
    );
    lv_obj_set_pos(guide, 40, 415);

    // Project info footer
    static lv_style_t style_footer;
    lv_style_copy(&style_footer, &lv_style_plain);
    style_footer.text.color = LV_COLOR_MAKE(120, 120, 120);

    lv_obj_t * footer = lv_label_create(tab_home, NULL);
    lv_label_set_text(footer, "UJS Embedded Dev - Fall 2025");
    lv_label_set_style(footer, LV_LABEL_STYLE_MAIN, &style_footer);
    lv_label_set_align(footer, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(footer, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -20);

    // Dark mode button for Home tab (bottom right corner)
    lv_obj_t * btn_dark_home = lv_btn_create(tab_home, NULL);
    lv_obj_set_size(btn_dark_home, 70, 40);
    lv_obj_align(btn_dark_home, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -20);
    lv_btn_set_style(btn_dark_home, LV_BTN_STYLE_REL, &style_btn_dark);
    lv_obj_set_event_cb(btn_dark_home, btn_dark_mode_event);

    label_dark_home = lv_label_create(btn_dark_home, NULL);
    lv_label_set_text(label_dark_home, "Dark");

    // ========== GPS Tab ==========

    lv_obj_t * gps_title = lv_label_create(tab_gps, NULL);
    lv_label_set_text(gps_title, "GPS Navigation");
    lv_label_set_style(gps_title, LV_LABEL_STYLE_MAIN, &style_title);
    lv_obj_align(gps_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 15);

    // Position section header
    static lv_style_t style_header;
    lv_style_copy(&style_header, &lv_style_plain);
    style_header.text.color = LV_COLOR_MAKE(80, 80, 80);

    lv_obj_t * pos_header = lv_label_create(tab_gps, NULL);
    lv_label_set_text(pos_header, "[ Position ]");
    lv_label_set_style(pos_header, LV_LABEL_STYLE_MAIN, &style_header);
    lv_obj_set_pos(pos_header, 30, 70);

    label_lat = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_lat, "Latitude:  --");
    lv_obj_set_pos(label_lat, 50, 100);

    label_lon = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_lon, "Longitude: --");
    lv_obj_set_pos(label_lon, 50, 125);

    // Motion section
    lv_obj_t * motion_header = lv_label_create(tab_gps, NULL);
    lv_label_set_text(motion_header, "[ Motion ]");
    lv_label_set_style(motion_header, LV_LABEL_STYLE_MAIN, &style_header);
    lv_obj_set_pos(motion_header, 30, 170);

    label_speed = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_speed, "Speed:     -- km/h");
    lv_obj_set_pos(label_speed, 50, 200);

    label_alt = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_alt, "Altitude:  -- m");
    lv_obj_set_pos(label_alt, 50, 225);

    // Signal section
    lv_obj_t * signal_header = lv_label_create(tab_gps, NULL);
    lv_label_set_text(signal_header, "[ Signal ]");
    lv_label_set_style(signal_header, LV_LABEL_STYLE_MAIN, &style_header);
    lv_obj_set_pos(signal_header, 30, 270);

    label_sats = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_sats, "Satellites: 0");
    lv_obj_set_pos(label_sats, 50, 300);

    // Time section
    lv_obj_t * time_header = lv_label_create(tab_gps, NULL);
    lv_label_set_text(time_header, "[ Time ]");
    lv_label_set_style(time_header, LV_LABEL_STYLE_MAIN, &style_header);
    lv_obj_set_pos(time_header, 30, 345);

    label_time = lv_label_create(tab_gps, NULL);
    lv_label_set_text(label_time, "UTC Time:  --:--:--");
    lv_obj_set_pos(label_time, 50, 375);

    // GPS info footer
    lv_obj_t * gps_info = lv_label_create(tab_gps, NULL);
    lv_label_set_text(gps_info, "Tip: Need 4+ satellites for fix\nWorks best in open outdoor areas");
    lv_label_set_align(gps_info, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(gps_info, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

    // Dark mode button for GPS tab (bottom right corner)
    lv_obj_t * btn_dark_gps = lv_btn_create(tab_gps, NULL);
    lv_obj_set_size(btn_dark_gps, 70, 40);
    lv_obj_align(btn_dark_gps, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -20);
    lv_btn_set_style(btn_dark_gps, LV_BTN_STYLE_REL, &style_btn_dark);
    lv_obj_set_event_cb(btn_dark_gps, btn_dark_mode_event);

    label_dark_gps = lv_label_create(btn_dark_gps, NULL);
    lv_label_set_text(label_dark_gps, "Dark");

    // ========== IMU Tab ==========
    // Title (reuse style_title)
    lv_obj_t * imu_title = lv_label_create(tab_imu, NULL);
    lv_label_set_text(imu_title, "Motion Sensor");
    lv_label_set_style(imu_title, LV_LABEL_STYLE_MAIN, &style_title);
    lv_obj_align(imu_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 15);

    // Subtitle
    lv_obj_t * imu_subtitle = lv_label_create(tab_imu, NULL);
    lv_label_set_text(imu_subtitle, "MPU6050 with DMP");
    lv_label_set_style(imu_subtitle, LV_LABEL_STYLE_MAIN, &style_header);
    lv_obj_align(imu_subtitle, imu_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    // Orientation section
    lv_obj_t * orient_header = lv_label_create(tab_imu, NULL);
    lv_label_set_text(orient_header, "[ Orientation ]");
    lv_label_set_style(orient_header, LV_LABEL_STYLE_MAIN, &style_header);
    lv_obj_set_pos(orient_header, 30, 90);

    label_pitch = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_pitch, "Pitch: -- deg");
    lv_obj_set_pos(label_pitch, 50, 120);

    label_roll = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_roll, "Roll:  -- deg");
    lv_obj_set_pos(label_roll, 50, 145);

    label_yaw = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_yaw, "Yaw:   -- deg");
    lv_obj_set_pos(label_yaw, 50, 170);

    // Temperature section
    lv_obj_t * temp_header = lv_label_create(tab_imu, NULL);
    lv_label_set_text(temp_header, "[ Temperature ]");
    lv_label_set_style(temp_header, LV_LABEL_STYLE_MAIN, &style_header);
    lv_obj_set_pos(temp_header, 30, 215);

    label_temp = lv_label_create(tab_imu, NULL);
    lv_label_set_text(label_temp, "Temp:  -- C");
    lv_obj_set_pos(label_temp, 50, 245);

    // IMU info footer
    lv_obj_t * imu_info = lv_label_create(tab_imu, NULL);
    lv_label_set_text(imu_info, "Pitch: Forward/Backward tilt\nRoll: Left/Right tilt\nYaw: Rotation around vertical\n\nUsing Digital Motion Processor");
    lv_label_set_align(imu_info, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(imu_info, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

    // Dark mode button for IMU tab (bottom right corner)
    lv_obj_t * btn_dark_imu = lv_btn_create(tab_imu, NULL);
    lv_obj_set_size(btn_dark_imu, 70, 40);
    lv_obj_align(btn_dark_imu, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -20);
    lv_btn_set_style(btn_dark_imu, LV_BTN_STYLE_REL, &style_btn_dark);
    lv_obj_set_event_cb(btn_dark_imu, btn_dark_mode_event);

    label_dark_imu = lv_label_create(btn_dark_imu, NULL);
    lv_label_set_text(label_dark_imu, "Dark");

    // ========== Files Tab (File Manager Style) ==========
    // Title
    lv_obj_t * files_title = lv_label_create(tab_files, NULL);
    lv_label_set_text(files_title, "SD Card Files");
    lv_label_set_style(files_title, LV_LABEL_STYLE_MAIN, &style_title);
    lv_obj_align(files_title, NULL, LV_ALIGN_IN_TOP_MID, 0, 15);

    // File list area header
    lv_obj_t * filelist_header = lv_label_create(tab_files, NULL);
    lv_label_set_text(filelist_header, "Recorded Files:");
    lv_obj_set_pos(filelist_header, 30, 70);

    // File list display area (like file explorer)
    label_browse_data = lv_label_create(tab_files, NULL);
    lv_label_set_text(label_browse_data, "No files found\nPress 'Start Log' to record data");
    lv_label_set_long_mode(label_browse_data, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label_browse_data, 420);
    lv_obj_set_pos(label_browse_data, 40, 100);

    // Recording control buttons
    lv_obj_t * control_header = lv_label_create(tab_files, NULL);
    lv_label_set_text(control_header, "Recording Control:");
    lv_obj_set_pos(control_header, 30, 280);

    // Record toggle button (Start/Stop combined, centered)
    btn_record_toggle = lv_btn_create(tab_files, NULL);
    lv_obj_set_size(btn_record_toggle, 380, 50);
    lv_obj_set_pos(btn_record_toggle, 50, 315);
    lv_btn_set_style(btn_record_toggle, LV_BTN_STYLE_REL, &style_btn_green);
    lv_obj_set_event_cb(btn_record_toggle, btn_record_toggle_event);

    label_record_btn = lv_label_create(btn_record_toggle, NULL);
    lv_label_set_text(label_record_btn, "Start Log");

    // File management buttons
    lv_obj_t * manage_header = lv_label_create(tab_files, NULL);
    lv_label_set_text(manage_header, "File Management:");
    lv_obj_set_pos(manage_header, 30, 385);

    // Browse/Refresh button
    btn_browse_data = lv_btn_create(tab_files, NULL);
    lv_obj_set_size(btn_browse_data, 180, 50);
    lv_obj_set_pos(btn_browse_data, 30, 420);
    lv_btn_set_style(btn_browse_data, LV_BTN_STYLE_REL, &style_btn_dark);
    lv_obj_set_event_cb(btn_browse_data, btn_browse_data_event);

    lv_obj_t * label_browse = lv_label_create(btn_browse_data, NULL);
    lv_label_set_text(label_browse, "Refresh");

    // Delete all button (red)
    btn_clear_data = lv_btn_create(tab_files, NULL);
    lv_obj_set_size(btn_clear_data, 180, 50);
    lv_obj_set_pos(btn_clear_data, 230, 420);
    lv_btn_set_style(btn_clear_data, LV_BTN_STYLE_REL, &style_btn_red);
    lv_obj_set_event_cb(btn_clear_data, btn_clear_data_event);

    lv_obj_t * label_clear = lv_label_create(btn_clear_data, NULL);
    lv_label_set_text(label_clear, "Delete All");

    // Statistics display
    label_statistics = lv_label_create(tab_files, NULL);
    lv_label_set_text(label_statistics, "Max Speed: 0 km/h | Distance: 0 km");
    lv_label_set_long_mode(label_statistics, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label_statistics, 420);
    lv_obj_set_pos(label_statistics, 30, 490);

    // Record count and status
    label_motion_status = lv_label_create(tab_files, NULL);
    lv_label_set_text(label_motion_status, "Status: Stationary");
    lv_obj_set_pos(label_motion_status, 30, 515);

    label_record_count = lv_label_create(tab_files, NULL);
    lv_label_set_text(label_record_count, "Records: 0");
    lv_obj_set_pos(label_record_count, 250, 515);

    // Last record preview
    label_last_record = lv_label_create(tab_files, NULL);
    lv_label_set_text(label_last_record, "Last: --");
    lv_label_set_long_mode(label_last_record, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label_last_record, 420);
    lv_obj_set_pos(label_last_record, 30, 540);

    // Dark mode button (bottom right corner)
    btn_dark_mode = lv_btn_create(tab_files, NULL);
    lv_obj_set_size(btn_dark_mode, 70, 40);
    lv_obj_align(btn_dark_mode, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -20);
    lv_btn_set_style(btn_dark_mode, LV_BTN_STYLE_REL, &style_btn_dark);
    lv_obj_set_event_cb(btn_dark_mode, btn_dark_mode_event);

    label_dark_files = lv_label_create(btn_dark_mode, NULL);
    lv_label_set_text(label_dark_files, "Dark");
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
        sprintf(buf, "Latitude:  %.6f%c", g_gps_data.latitude, g_gps_data.lat_dir);
        lv_label_set_text(label_lat, buf);

        sprintf(buf, "Longitude: %.6f%c", g_gps_data.longitude, g_gps_data.lon_dir);
        lv_label_set_text(label_lon, buf);

        sprintf(buf, "Speed:     %.2f km/h", g_gps_data.speed_kmh);
        lv_label_set_text(label_speed, buf);

        sprintf(buf, "Altitude:  %.1f m", g_gps_data.altitude);
        lv_label_set_text(label_alt, buf);

        sprintf(buf, "Satellites: %d (Fixed)", g_gps_data.satellites);
        lv_label_set_text(label_sats, buf);

        u8 bj_hour = (g_gps_data.hour + 8) % 24;
        sprintf(buf, "UTC: %02d:%02d:%02d  Beijing: %02d:%02d:%02d",
                g_gps_data.hour, g_gps_data.minute, g_gps_data.second,
                bj_hour, g_gps_data.minute, g_gps_data.second);
        lv_label_set_text(label_time, buf);
    }
    else
    {
        lv_label_set_text(label_lat, "Latitude:  Searching...");
        lv_label_set_text(label_lon, "Longitude: Searching...");
        lv_label_set_text(label_speed, "Speed:     --");
        lv_label_set_text(label_alt, "Altitude:  --");
        sprintf(buf, "Satellites: %d (Searching)", g_gps_data.satellites);
        lv_label_set_text(label_sats, buf);
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
    // Detect motion based on speed and gyroscope
    if(g_gps_data.speed_kmh > 1.0f ||
       (g_mpu_data.gyro_x > 500 || g_mpu_data.gyro_x < -500) ||
       (g_mpu_data.gyro_y > 500 || g_mpu_data.gyro_y < -500) ||
       (g_mpu_data.gyro_z > 500 || g_mpu_data.gyro_z < -500))
    {
        lv_label_set_text(label_motion_status, "Status: MOVING");
    }
    else
    {
        lv_label_set_text(label_motion_status, "Status: Stationary");
    }

    sprintf(buf, "Records: %d", g_sdcard_status.record_count);
    lv_label_set_text(label_record_count, buf);

    // Update record toggle button state
    if(g_sdcard_status.logging)
    {
        lv_label_set_text(label_record_btn, "Stop Log");
        lv_btn_set_style(btn_record_toggle, LV_BTN_STYLE_REL, &style_btn_red);
    }
    else
    {
        lv_label_set_text(label_record_btn, "Start Log");
        lv_btn_set_style(btn_record_toggle, LV_BTN_STYLE_REL, &style_btn_green);
    }

    // Update statistics (only when GPS is valid and logging)
    if(g_gps_data.valid && g_sdcard_status.logging)
    {
        // Track max speed
        if(g_gps_data.speed_kmh > max_speed)
            max_speed = g_gps_data.speed_kmh;

        // Track max altitude
        if(g_gps_data.altitude > max_altitude)
            max_altitude = g_gps_data.altitude;

        // Calculate distance traveled (simple approximation)
        if(stats_initialized)
        {
            // Simple distance calculation (lat/lon difference in km)
            // Approximate: 1 degree latitude = 111 km
            float lat_diff = (g_gps_data.latitude - last_lat) * 111.0f;
            float lon_diff = (g_gps_data.longitude - last_lon) * 111.0f * 0.707f; // cos(45deg) rough estimate
            float dist = sqrtf(lat_diff * lat_diff + lon_diff * lon_diff);

            // Only add if reasonable movement (< 1km per update to filter GPS noise)
            if(dist < 1.0f && dist > 0.0f)
                total_distance += dist;
        }

        last_lat = g_gps_data.latitude;
        last_lon = g_gps_data.longitude;
        stats_initialized = 1;
    }

    // Display statistics (single line to avoid overlap)
    sprintf(buf, "Max Speed: %.2f km/h  Distance: %.2f km",
            max_speed, total_distance);
    lv_label_set_text(label_statistics, buf);

    // Update last record preview (simplified)
    if(g_sdcard_status.record_count > 0)
    {
        sprintf(buf, "Last: %s Lat:%.4f Lon:%.4f %.1fkm/h",
                g_sdcard_status.last_record.time_str,
                g_sdcard_status.last_record.latitude,
                g_sdcard_status.last_record.longitude,
                g_sdcard_status.last_record.speed);
        lv_label_set_text(label_last_record, buf);
    }
    else
    {
        lv_label_set_text(label_last_record, "Last: No data yet");
    }
}

/**
 * @brief Switch to specific page
 */
void App_UI_Main_SwitchPage(UI_Page page)
{
    lv_tabview_set_tab_act(tabview, page, true);
}
