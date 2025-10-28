#include "app_ui_splash.h"
#include "app_ui_main.h"
#include <stdio.h>

static lv_obj_t * splash_screen;

/**
 * @brief Create splash screen with project info
 */
void App_UI_Splash_Create(void)
{
    // Create splash screen
    splash_screen = lv_obj_create(NULL, NULL);
    lv_scr_load(splash_screen);

    // Background color - Professional blue gradient
    static lv_style_t style_bg;
    lv_style_copy(&style_bg, &lv_style_plain);
    style_bg.body.main_color = LV_COLOR_MAKE(0, 80, 150);
    style_bg.body.grad_color = LV_COLOR_MAKE(0, 40, 100);
    lv_obj_set_style(splash_screen, &style_bg);

    // University Info
    static lv_style_t style_university;
    lv_style_copy(&style_university, &lv_style_plain);
    style_university.text.color = LV_COLOR_WHITE;
    style_university.text.font = &lv_font_roboto_22;

    lv_obj_t * label_university = lv_label_create(splash_screen, NULL);
    lv_label_set_text(label_university, "Jiangsu University");
    lv_label_set_style(label_university, LV_LABEL_STYLE_MAIN, &style_university);
    lv_obj_align(label_university, NULL, LV_ALIGN_IN_TOP_MID, 0, 60);

    // Project Title
    static lv_style_t style_title;
    lv_style_copy(&style_title, &lv_style_plain);
    style_title.text.color = LV_COLOR_WHITE;
    style_title.text.font = &lv_font_roboto_28;

    lv_obj_t * label_title = lv_label_create(splash_screen, NULL);
    lv_label_set_text(label_title, "Smart Motion Tracker");
    lv_label_set_style(label_title, LV_LABEL_STYLE_MAIN, &style_title);
    lv_obj_align(label_title, NULL, LV_ALIGN_CENTER, 0, -40);

    // Subtitle
    static lv_style_t style_subtitle;
    lv_style_copy(&style_subtitle, &lv_style_plain);
    style_subtitle.text.color = LV_COLOR_MAKE(180, 220, 255);
    style_subtitle.text.font = &lv_font_roboto_16;

    lv_obj_t * label_subtitle = lv_label_create(splash_screen, NULL);
    lv_label_set_text(label_subtitle, "GPS + IMU Data Logger");
    lv_label_set_style(label_subtitle, LV_LABEL_STYLE_MAIN, &style_subtitle);
    lv_obj_align(label_subtitle, NULL, LV_ALIGN_CENTER, 0, 0);

    // Course Info
    lv_obj_t * label_course = lv_label_create(splash_screen, NULL);
    lv_label_set_text(label_course, "Embedded Application Development\nFall 2025 - Midterm Project");
    lv_label_set_style(label_course, LV_LABEL_STYLE_MAIN, &style_subtitle);
    lv_label_set_align(label_course, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label_course, NULL, LV_ALIGN_CENTER, 0, 50);

    // Student Info
    static lv_style_t style_info;
    lv_style_copy(&style_info, &lv_style_plain);
    style_info.text.color = LV_COLOR_MAKE(200, 230, 255);

    lv_obj_t * label_student = lv_label_create(splash_screen, NULL);
    lv_label_set_text(label_student, "Class: Communication 2402\nGrade: 2024");
    lv_label_set_style(label_student, LV_LABEL_STYLE_MAIN, &style_info);
    lv_label_set_align(label_student, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(label_student, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -40);

    // Loading indicator
    lv_obj_t * label_loading = lv_label_create(splash_screen, NULL);
    lv_label_set_text(label_loading, "Loading...");
    lv_label_set_style(label_loading, LV_LABEL_STYLE_MAIN, &style_info);
    lv_obj_align(label_loading, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
}

/**
 * @brief Close splash screen and switch to main UI
 */
void App_UI_Splash_Close(void)
{
    // Delete splash screen
    lv_obj_del(splash_screen);

    // Create main UI
    App_UI_Main_Create();
}
