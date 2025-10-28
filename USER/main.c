#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "timer.h"
#include "touch.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
// #include "app_test.h"      // 阶段1测试界面
#include "app_gps.h"          // 阶段2: GPS模块
// #include "app_ui_gps.h"    // 阶段2: GPS UI界面 (replaced by app_ui_main)
#include "app_mpu.h"          // 阶段3: MPU6050模块
// #include "app_ui_mpu.h"    // 阶段3: MPU6050 UI界面 (replaced by app_ui_main)
#include "app_sdcard.h"       // 阶段4: SD卡数据记录
#include "app_ui_main.h"      // 阶段6: 新UI界面

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	
	LED_Init();					//��ʼ��LED
 	LCD_Init();					//LCD��ʼ��
	KEY_Init(); 				//������ʼ��
	TIM3_Int_Init(999,83);	//��ʱ����ʼ��(1ms�ж�),���ڸ�lvgl�ṩ1ms����������
	tp_dev.init();			//��������ʼ��
	
	lv_init();						//lvglϵͳ��ʼ��
	lv_port_disp_init();	//lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
	lv_port_indev_init();	//lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���

	// ========== 阶段2: GPS模块测试 ==========
	App_GPS_Init();       // 初始化GPS模块

	// ========== 阶段3: MPU6050模块测试 ==========
	App_MPU_Init();       // 初始化MPU6050模块

	// ========== 阶段4: SD卡数据记录 ==========
	App_SDCard_Init();    // 初始化SD卡
	delay_ms(500);        // 等待SD卡初始化

	// ========== 阶段6: 创建新UI界面 ==========
	App_UI_Main_Create(); // 创建带选项卡的UI

	u16 ui_update_counter = 0;
	u16 log_counter = 0;

	while(1)
	{
		tp_dev.scan(0);
		lv_task_handler();

		// GPS数据持续解析（每次循环都检查）
		App_GPS_Update();

		// MPU6050数据持续读取（每次循环都检查）
		App_MPU_Update();

		// UI更新频率控制 (每200次循环更新一次UI，约200ms)
		ui_update_counter++;
		if(ui_update_counter >= 200)
		{
			ui_update_counter = 0;
			App_UI_Main_Update();  // 更新新UI显示
			LED0 = !LED0;         // LED闪烁指示运行
		}

		// SD卡数据记录（每1000次循环记录一次，约1秒）
		log_counter++;
		if(log_counter >= 1000)
		{
			log_counter = 0;
			App_SDCard_WriteData();  // 写入数据到SD卡
		}

		delay_ms(1);
	}
}
