//系统时钟配置为64M,上电，LED灯循环点亮

#include "main.h"

void led_test(void)
{
		 PC13_TOG();
}
void key1(uint32_t param)
{
	printf("key1:%d \r\n",param);
	
}
void key2(uint32_t param)
{
	printf("key2:%d \r\n",param);
	
}
void key3(uint32_t param)
{
	printf("key3:%d \r\n",param);
	
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
	//串口初始化
	UART_init();
	//按键初始化
	KEYMSG_Init();
	printf("cw32 \r\n");
	KEYMSG_RegisterMsgService(KEY_A1_KEY,KEY_MSG_ONE_CLICK,key1,1);
	KEYMSG_RegisterMsgService(KEY_A1_KEY,KEY_MSG_HOLD,key1,255);
	KEYMSG_RegisterMsgService(KEY_A2_KEY,KEY_MSG_ONE_CLICK,key2,1);
	KEYMSG_RegisterMsgService(KEY_A3_KEY,KEY_MSG_ONE_CLICK,key3,1);
	TimerSlice_SetTask(SLICE_ID_LED,LED_DELAY_TIME,led_test,0);
	while(1)
	{
			TimerSlice_Process();
   		KEYMSG_ScanProcess();
	}
}




#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/**
 * @brief Retargets the C library printf function to the USART.
 * 
 */
PUTCHAR_PROTOTYPE
{
    USART_SendData_8bit(CW_UART1, (uint8_t)ch);

    while (USART_GetFlagStatus(CW_UART1, USART_FLAG_TXE) == RESET);

    return ch;
}




