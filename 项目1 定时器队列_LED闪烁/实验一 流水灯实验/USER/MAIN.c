//ϵͳʱ������Ϊ64M,�ϵ磬LED��ѭ������

#include "main.h"

void led_test(void)
{
		 PC13_TOG();
}
	
int main()
{
	//ʱ������64M
	RCC_Configuration(); 
	//LED��ʼ��	
	GPIO_Configuration();
	//Time ��ʼ��
	ATIMER_init();
	//��ʼ��״̬��ʱ��Ƭ����
	TimerSlice_Init();
	//
	
	TimerSlice_SetTask(SLICE_ID_LED,LED_DELAY_TIME,led_test,0);
	while(1)
	{
			TimerSlice_Process();
   		
	}
}







