#include "Timer.h"
extern TIME_SLICE_TASK SliceTask[SLICE_ID_MAX];

void ATIMER_init(void)
{
	ATIM_InitTypeDef ATIM_InitStruct;
	
	__RCC_ATIM_CLK_ENABLE();
	
	__disable_irq(); 
  NVIC_EnableIRQ(ATIM_IRQn); 
  __enable_irq();
	
	ATIM_InitStruct.BufferState = ENABLE;                               //ʹ�ܻ���Ĵ���   
  ATIM_InitStruct.ClockSelect = ATIM_CLOCK_PCLK;                      //ѡ��PCLKʱ�Ӽ���
  ATIM_InitStruct.CounterAlignedMode = ATIM_COUNT_MODE_EDGE_ALIGN;    //���ض���
  ATIM_InitStruct.CounterDirection = ATIM_COUNTING_UP;                //���ϼ�����
  ATIM_InitStruct.CounterOPMode = ATIM_OP_MODE_REPETITIVE;            //��������ģʽ
  ATIM_InitStruct.OverFlowMask = DISABLE;                             //�ظ������������������
  ATIM_InitStruct.Prescaler = ATIM_Prescaler_DIV8;                    // 8��Ƶ,8MHZ
  ATIM_InitStruct.ReloadValue = 8000;                                 // �������� 1MS TIMER
  ATIM_InitStruct.RepetitionCounter = 0;                              // �ظ�����0
  ATIM_InitStruct.UnderFlowMask = DISABLE;                            // �ظ����������������
  ATIM_Init(&ATIM_InitStruct);
  ATIM_ITConfig(ATIM_CR_IT_OVE, ENABLE);                              // ���ظ�������������������ж�
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





