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
#include "app_ui_gps.h"       // 阶段2: GPS UI界面

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
	App_UI_GPS_Create();  // 创建GPS界面

	u16 gps_update_counter = 0;

	while(1)
	{
		tp_dev.scan(0);
		lv_task_handler();

		// GPS数据更新 (每100ms更新一次)
		gps_update_counter++;
		if(gps_update_counter >= 100)
		{
			gps_update_counter = 0;
			App_GPS_Update();     // 更新GPS数据
			App_UI_GPS_Update();  // 更新UI显示
		}

		delay_ms(1);
	}
}
