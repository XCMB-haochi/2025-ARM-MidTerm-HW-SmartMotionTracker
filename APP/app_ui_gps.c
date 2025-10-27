#include "app_ui_gps.h"
#include "app_gps.h"
#include "lv_conf.h"
#include <stdio.h>

// UI对象全局变量
static lv_obj_t * label_status;      // GPS状态标签
static lv_obj_t * label_lat;         // 纬度标签
static lv_obj_t * label_lon;         // 经度标签
static lv_obj_t * label_speed;       // 速度标签
static lv_obj_t * label_altitude;    // 海拔标签
static lv_obj_t * label_satellites;  // 卫星数标签
static lv_obj_t * label_time;        // 时间标签

/**
 * 创建GPS测试界面
 */
void App_UI_GPS_Create(void)
{
    // ========== 创建主容器 ==========
    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    // ========== 标题 ==========
    lv_obj_t * title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "GPS Module Test - Stage 2");

    static lv_style_t title_style;
    lv_style_copy(&title_style, &lv_style_plain);
    title_style.text.font = &lv_font_roboto_16;
    title_style.text.color = LV_COLOR_BLUE;
    lv_label_set_style(title, LV_LABEL_STYLE_MAIN, &title_style);
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    // ========== GPS状态卡片 ==========
    lv_obj_t * status_cont = lv_cont_create(scr, NULL);
    lv_obj_set_size(status_cont, 440, 80);
    lv_obj_align(status_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 60);

    static lv_style_t card_style;
    lv_style_copy(&card_style, &lv_style_plain);
    card_style.body.radius = 10;
    card_style.body.border.width = 2;
    card_style.body.border.color = LV_COLOR_BLUE;
    card_style.body.main_color = LV_COLOR_WHITE;
    card_style.body.grad_color = LV_COLOR_WHITE;
    lv_cont_set_style(status_cont, LV_CONT_STYLE_MAIN, &card_style);

    // GPS状态标签
    label_status = lv_label_create(status_cont, NULL);
    lv_label_set_text(label_status, "GPS Status: Waiting...");
    lv_obj_align(label_status, NULL, LV_ALIGN_IN_TOP_LEFT, 15, 15);

    // 卫星数标签
    label_satellites = lv_label_create(status_cont, NULL);
    lv_label_set_text(label_satellites, "Satellites: 0");
    lv_obj_align(label_satellites, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 15, -15);

    // ========== GPS数据卡片 ==========
    lv_obj_t * data_cont = lv_cont_create(scr, NULL);
    lv_obj_set_size(data_cont, 440, 300);
    lv_obj_align(data_cont, status_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_cont_set_style(data_cont, LV_CONT_STYLE_MAIN, &card_style);

    // 创建数据标签
    int y_offset = 15;
    int y_step = 45;

    // 纬度
    label_lat = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_lat, "Latitude:  --");
    lv_obj_align(label_lat, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);

    // 经度
    y_offset += y_step;
    label_lon = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_lon, "Longitude: --");
    lv_obj_align(label_lon, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);

    // 速度
    y_offset += y_step;
    label_speed = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_speed, "Speed:     -- km/h");
    lv_obj_align(label_speed, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);

    // 海拔
    y_offset += y_step;
    label_altitude = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_altitude, "Altitude:  -- m");
    lv_obj_align(label_altitude, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);

    // 时间
    y_offset += y_step;
    label_time = lv_label_create(data_cont, NULL);
    lv_label_set_text(label_time, "UTC Time:  --:--:--");
    lv_obj_align(label_time, NULL, LV_ALIGN_IN_TOP_LEFT, 15, y_offset);

    // ========== 底部状态提示 ==========
    lv_obj_t * hint = lv_label_create(scr, NULL);
    lv_label_set_text(hint, "Stage 2: GPS module working!");

    static lv_style_t hint_style;
    lv_style_copy(&hint_style, &lv_style_plain);
    hint_style.text.color = LV_COLOR_GREEN;
    lv_label_set_style(hint, LV_LABEL_STYLE_MAIN, &hint_style);
    lv_obj_align(hint, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
}

/**
 * 更新GPS数据显示
 * 说明：在主循环中周期性调用
 */
void App_UI_GPS_Update(void)
{
    char buf[120];

    // 更新卫星数（总是显示）
    sprintf(buf, "Satellites: %d", g_gps_data.satellites);
    lv_label_set_text(label_satellites, buf);

    if(g_gps_data.valid)
    {
        // 更新状态
        lv_label_set_text(label_status, "GPS Status: Fixed");

        // 更新纬度
        sprintf(buf, "Lat: %.6f %c", g_gps_data.latitude, g_gps_data.lat_dir);
        lv_label_set_text(label_lat, buf);

        // 更新经度
        sprintf(buf, "Lon: %.6f %c", g_gps_data.longitude, g_gps_data.lon_dir);
        lv_label_set_text(label_lon, buf);

        // 更新速度
        sprintf(buf, "Speed: %.2f km/h", g_gps_data.speed_kmh);
        lv_label_set_text(label_speed, buf);

        // 更新海拔
        sprintf(buf, "Alt: %.1f m", g_gps_data.altitude);
        lv_label_set_text(label_altitude, buf);

        // 更新时间 (UTC时间+8小时=北京时间)
        u8 beijing_hour = (g_gps_data.hour + 8) % 24;
        sprintf(buf, "UTC: %02d:%02d:%02d BJ:%02d:%02d",
                g_gps_data.hour, g_gps_data.minute, g_gps_data.second,
                beijing_hour, g_gps_data.minute);
        lv_label_set_text(label_time, buf);
    }
    else
    {
        // GPS未定位
        lv_label_set_text(label_status, "GPS Status: Searching...");
    }
}
