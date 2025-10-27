#include "app_test.h"
#include "lv_conf.h"
#include <string.h>

// 按钮点击事件回调
static void btn_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED)
    {
        // 获取标签对象
        lv_obj_t * label = lv_obj_get_child(obj, NULL);

        // 切换按钮文本
        const char * txt = lv_label_get_text(label);
        if(strcmp(txt, "Test OK!") != 0)
        {
            lv_label_set_text(label, "Test OK!");
        }
        else
        {
            lv_label_set_text(label, "Click Test");
        }
    }
}

/**
 * 创建阶段1测试界面
 * 功能：测试LVGL基本显示和触摸功能
 */
void App_Test_Create(void)
{
    // ========== 创建主容器 ==========
    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    // ========== 标题标签 ==========
    lv_obj_t * title = lv_label_create(scr, NULL);
    lv_label_set_text(title, "Smart Motion Tracker");

    // 标题样式 - 使用默认字体
    static lv_style_t title_style;
    lv_style_copy(&title_style, &lv_style_plain);
    title_style.text.font = &lv_font_roboto_16;  // 使用已启用的字体
    title_style.text.color = LV_COLOR_BLUE;
    lv_label_set_style(title, LV_LABEL_STYLE_MAIN, &title_style);

    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 30);

    // ========== 版本信息 ==========
    lv_obj_t * version = lv_label_create(scr, NULL);
    lv_label_set_text(version, "Version 1.0 - Stage 1");

    static lv_style_t ver_style;
    lv_style_copy(&ver_style, &lv_style_plain);
    ver_style.text.color = LV_COLOR_GRAY;
    lv_label_set_style(version, LV_LABEL_STYLE_MAIN, &ver_style);

    lv_obj_align(version, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // ========== 测试信息卡片 ==========
    lv_obj_t * info_cont = lv_cont_create(scr, NULL);
    lv_obj_set_size(info_cont, 400, 280);
    lv_obj_align(info_cont, NULL, LV_ALIGN_CENTER, 0, -20);

    // 卡片样式
    static lv_style_t card_style;
    lv_style_copy(&card_style, &lv_style_plain);
    card_style.body.radius = 10;
    card_style.body.border.width = 2;
    card_style.body.border.color = LV_COLOR_BLUE;
    card_style.body.main_color = LV_COLOR_WHITE;
    card_style.body.grad_color = LV_COLOR_WHITE;
    lv_cont_set_style(info_cont, LV_CONT_STYLE_MAIN, &card_style);

    // 卡片内容 - 使用英文避免编码问题
    lv_obj_t * info_label = lv_label_create(info_cont, NULL);
    lv_label_set_text(info_label,
        "Stage 1 Test Items:\n\n"
        "[OK] LCD Display\n"
        "[OK] LVGL Graphics\n"
        "[OK] Touch Screen\n"
        "[OK] 480x800 Resolution\n\n"
        "Hardware Modules:\n"
        "- GPS Module Ready\n"
        "- MPU6050 Ready\n"
        "- FATFS Ready"
    );
    lv_obj_align(info_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);

    // ========== 测试按钮 ==========
    lv_obj_t * btn = lv_btn_create(scr, NULL);
    lv_obj_set_size(btn, 200, 60);
    lv_obj_align(btn, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -40);
    lv_obj_set_event_cb(btn, btn_event_handler);

    // 按钮样式
    static lv_style_t btn_style;
    lv_style_copy(&btn_style, &lv_style_plain);
    btn_style.body.radius = 30;
    btn_style.body.main_color = LV_COLOR_BLUE;
    btn_style.body.grad_color = LV_COLOR_NAVY;
    btn_style.body.border.width = 2;
    btn_style.body.border.color = LV_COLOR_WHITE;
    lv_btn_set_style(btn, LV_BTN_STYLE_REL, &btn_style);

    // 按钮文本
    lv_obj_t * btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Click Test");

    static lv_style_t btn_label_style;
    lv_style_copy(&btn_label_style, &lv_style_plain);
    btn_label_style.text.color = LV_COLOR_WHITE;
    btn_label_style.text.font = &lv_font_roboto_16;  // 使用已启用的字体
    lv_label_set_style(btn_label, LV_LABEL_STYLE_MAIN, &btn_label_style);

    // ========== 底部提示 ==========
    lv_obj_t * hint = lv_label_create(scr, NULL);
    lv_label_set_text(hint, "Touch the button to test");

    static lv_style_t hint_style;
    lv_style_copy(&hint_style, &lv_style_plain);
    hint_style.text.color = LV_COLOR_GRAY;
    lv_label_set_style(hint, LV_LABEL_STYLE_MAIN, &hint_style);

    lv_obj_align(hint, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
}
