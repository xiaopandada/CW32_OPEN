//系统时钟配置为64M,上电，LED灯循环点亮

#include "main.h"

void led_test(void)
{
		 PC13_TOG();
}
	
int main()
{
	//时钟配置64M
	RCC_Configuration(); 
	//LED初始化	
	GPIO_Configuration();
	//Time 初始化
	ATIMER_init();
	//初始化状态机时间片处理
	TimerSlice_Init();
	//
	
	TimerSlice_SetTask(SLICE_ID_LED,LED_DELAY_TIME,led_test,0);
	while(1)
	{
			TimerSlice_Process();
   		
	}
}







