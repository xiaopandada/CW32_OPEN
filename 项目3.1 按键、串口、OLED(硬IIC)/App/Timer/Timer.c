#include "Timer.h"
extern TIME_SLICE_TASK SliceTask[SLICE_ID_MAX];

void ATIMER_init(void)
{
	ATIM_InitTypeDef ATIM_InitStruct;
	
	__RCC_ATIM_CLK_ENABLE();
	
	__disable_irq(); 
  NVIC_EnableIRQ(ATIM_IRQn); 
  __enable_irq();
	
	ATIM_InitStruct.BufferState = ENABLE;                               //使能缓存寄存器   
  ATIM_InitStruct.ClockSelect = ATIM_CLOCK_PCLK;                      //选择PCLK时钟计数
  ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_EDGE_ALIGN;    //边沿对齐
  ATIM_InitStruct.CounterDirection = ATIM_COUNTING_UP;                //向上计数；
  ATIM_InitStruct.CounterOPMode = ATIM_OP_MODE_REPETITIVE;            //连续运行模式
  ATIM_InitStruct.OverFlowMask = DISABLE;                             //重复计数器上溢出不屏蔽
  ATIM_InitStruct.Prescaler = ATIM_Prescaler_DIV8;                    // 8分频,8MHZ
  ATIM_InitStruct.ReloadValue = 8000;                                 // 重载周期 1MS TIMER
  ATIM_InitStruct.RepetitionCounter = 0;                              // 重复周期0
  ATIM_InitStruct.UnderFlowMask = DISABLE;                            // 重复计数下溢出不屏蔽
  ATIM_Init(&ATIM_InitStruct);
  ATIM_ITConfig(ATIM_CR_IT_OVE, ENABLE);                              // 有重复计数器溢出产生进入中断
  ATIM_Cmd(ENABLE); 	  
}

void ATIM_IRQHandler(void)
{

  if (ATIM_GetITStatus(ATIM_IT_OVF))
  {
    ATIM_ClearITPendingBit(ATIM_IT_OVF);
    for( int i = 0; i < SLICE_ID_MAX; i++)
		{
				TIME_SLICE_TASK *pTask = &SliceTask[i];
				if(!pTask->active)
					continue;
				if(pTask->action)
						pTask->TimeCnt += TIMER_TIME;			
		}
  }
}





