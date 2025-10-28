#ifndef __APP_UI_MAIN_H
#define __APP_UI_MAIN_H

#include "lvgl.h"

// Page index
typedef enum {
    PAGE_GPS = 0,
    PAGE_IMU = 1,
    PAGE_DATA = 2
} UI_Page;

void App_UI_Main_Create(void);
void App_UI_Main_Update(void);
void App_UI_Main_SwitchPage(UI_Page page);

#endif
