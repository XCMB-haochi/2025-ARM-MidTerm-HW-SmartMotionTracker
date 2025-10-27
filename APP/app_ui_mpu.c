#include "app_ui_mpu.h"
#include "app_mpu.h"
#include "lv_conf.h"
#include <stdio.h>

// UI objects global variables
static lv_obj_t * label_status;      // MPU status label
static lv_obj_t * label_temp;        // Temperature label
static lv_obj_t * label_pitch;       // Pitch angle label
static lv_obj_t * label_roll;        // Roll angle label
static lv_obj_t * label_yaw;         // Yaw angle label

/**
 * @brief Create MPU6050 UI interface
 */
void App_UI_MPU_Create(void)
{
    // Get current screen
    lv_obj_t * scr = lv_scr_act();

    // ========== MPU6050 Status Card ==========
    lv_obj_t * status_cont = lv_cont_create(scr, NULL);
    lv_obj_set_size(status_cont, 440, 80);
    lv_obj_align(status_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 420);

    static lv_style_t card_style;
    lv_style_copy(&card_style, &lv_style_plain);
    card_style.body.radius = 10;
    card_style.body.border.width = 2;
    card_style.body.border.color = LV_COLOR_RED;
    card_style.body.main_color = LV_COLOR_WHITE;
    card_style.body.grad_color = LV_COLOR_WHITE;
    lv_cont_set_style(status_cont, LV_CONT_STYLE_MAIN, &card_style);

    // MPU status label
    label_status = lv_label_create(status_cont, NULL);
    lv_label_set_text(label_status, "MPU6050: Initializing...");
    lv_obj_align(label_status, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 15);

    // Temperature label
    label_temp = lv_label_create(status_cont, NULL);
    lv_label_set_text(label_temp, "Temp: -- C");
    lv_obj_align(label_temp, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 15, -15);

    // ========== MPU6050 Attitude Data Card ==========
    lv_obj_t * data_cont = lv_cont_create(scr, NULL);
    lv_obj_set_size(data_cont, 440, 180);
    lv_obj_align(data_cont, status_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_cont_set_style(data_cont, LV_CONT_STYLE_MAIN, &card_style);

    // Create data labels
    int y_offset = 15;
    int y_step = 50;

    // Pitch angle
    label_pitch = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_pitch, "Pitch: -- deg");
    lv_obj_align(label_pitch, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);

    // Roll angle
    y_offset += y_step;
    label_roll = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_roll, "Roll:  -- deg");
    lv_obj_align(label_roll, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);

    // Yaw angle
    y_offset += y_step;
    label_yaw = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_yaw, "Yaw:   -- deg");
    lv_obj_align(label_yaw, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);
}

/**
 * @brief Update MPU6050 data display
 * Note: Called periodically in main loop
 */
void App_UI_MPU_Update(void)
{
    char buf[100];

    if(g_mpu_data.valid)
    {
        // Update status
        lv_label_set_text(label_status, "MPU6050: Ready");

        // Update temperature
        sprintf(buf, "Temp: %.1f C", g_mpu_data.temperature);
        lv_label_set_text(label_temp, buf);

        // Update pitch angle
        sprintf(buf, "Pitch: %.2f deg", g_mpu_data.pitch);
        lv_label_set_text(label_pitch, buf);

        // Update roll angle
        sprintf(buf, "Roll:  %.2f deg", g_mpu_data.roll);
        lv_label_set_text(label_roll, buf);

        // Update yaw angle
        sprintf(buf, "Yaw:   %.2f deg", g_mpu_data.yaw);
        lv_label_set_text(label_yaw, buf);
    }
    else
    {
        // MPU6050 not initialized
        lv_label_set_text(label_status, "MPU6050: Error!");
        lv_label_set_text(label_temp, "Temp: -- C");
        lv_label_set_text(label_pitch, "Pitch: -- deg");
        lv_label_set_text(label_roll, "Roll:  -- deg");
        lv_label_set_text(label_yaw, "Yaw:   -- deg");
    }
}
