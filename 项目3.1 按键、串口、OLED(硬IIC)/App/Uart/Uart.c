#include "Uart.h"


void UART_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	

    RCC_APBPeriphClk_Enable2(RCC_APB2_PERIPH_UART1, ENABLE);
    RCC_AHBPeriphClk_Enable( RCC_AHB_PERIPH_GPIOA, ENABLE);  
    
	
    GPIO_InitStructure.Pins = GPIO_PIN_8; //PA8
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(CW_GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pins = GPIO_PIN_9; //PA9
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
    GPIO_Init(CW_GPIOA, &GPIO_InitStructure);
		
		//重定向
    PA08_AFx_UART1TXD();
    PA09_AFx_UART1RXD();
  
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_Over = USART_Over_16;
    USART_InitStructure.USART_Source = USART_Source_PCLK;
    USART_InitStructure.USART_UclkFreq = 64000000;
    USART_InitStructure.USART_StartBit = USART_StartBit_FE;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(CW_UART1, &USART_InitStructure);
	
	
    //使能UARTx RC中断
    USART_ITConfig(CW_UART1, USART_IT_RC, ENABLE);
    //优先级，无优先级分组
    NVIC_SetPriority(UART1_IRQn, 0);
    //UARTx中断使能
    NVIC_EnableIRQ(UART1_IRQn);

}

 
void UART1_IRQHandler(void)
{
  /* USER CODE BEGIN */

  if(USART_GetITStatus(CW_UART1, USART_IT_RC) != RESET)
  { 
		
  }
  /* USER CODE END */
}


