#include "Sys.h"

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	__RCC_GPIOB_CLK_ENABLE();
	__RCC_GPIOC_CLK_ENABLE();
	__RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.IT = GPIO_IT_NONE; //LED 
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pins = GPIO_PIN_13;	// LED4
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);
	GPIO_WritePin(CW_GPIOC,GPIO_PIN_13,GPIO_Pin_RESET);
}

void RCC_Configuration(void)
{
  /* 0. HSIʹ�ܲ�У׼ */
  RCC_HSI_Enable(RCC_HSIOSC_DIV6);

  /* 1. ����HCLK��PCLK�ķ�Ƶϵ����*/
  RCC_HCLKPRS_Config(RCC_HCLK_DIV1);
  RCC_PCLKPRS_Config(RCC_PCLK_DIV1);
  
  /* 2. ʹ��PLL��ͨ��PLL��Ƶ��64MHz */
  RCC_PLL_Enable(RCC_PLLSOURCE_HSI, 8000000, 8);     // HSI Ĭ�����Ƶ��8MHz
 // RCC_PLL_OUT();  //PC13�����PLLʱ��
  
  ///< ��ʹ�õ�ʱ��ԴHCLK����24M,С�ڵ���48MHz������FLASH ���ȴ�����Ϊ2 cycle
  ///< ��ʹ�õ�ʱ��ԴHCLK����48MHz������FLASH ���ȴ�����Ϊ3 cycle
  __RCC_FLASH_CLK_ENABLE();
  FLASH_SetLatency(FLASH_Latency_3);   
    
  /* 3. ʱ���л���PLL */
  RCC_SysClk_Switch(RCC_SYSCLKSRC_PLL);
  RCC_SystemCoreClockUpdate(64000000);	
}

static unsigned int fac_us = 0; //us��ʱ������
static unsigned int fac_ms = 0; //ms��ʱ������


//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8(M3��M4);SYSTICK=HCLK(M0+)
//SYSCLK:ϵͳʱ��
void Delay_Init(void)
{
    fac_us = SystemCoreClock / (1000000U); //systickʱ��Ƶ��ΪHCLKʱ��Ƶ��,�ݼ�һ������,��ʱ1/SystemCoreClock��,��λ����΢��Ϊ1000000/SystemCoreClock΢��,
    //1us��Ҫ�ļ���ֵ:1us/(1000000/SystemCoreClock)us=SystemCoreClock/1000000=fac_us
    fac_ms = (unsigned short)fac_us * (1000U); //ÿ1ms��Ҫ��systickʱ����,��ÿ1ms systick��VAL������Ŀ
}

//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,�����ʱΪ:
//nms<=0xffffff/fac_ms=0xffffff/(SystemCoreClock/1000000*1000)=0xffffff*1000/SystemCoreClock
//SystemCoreClock��λΪHz,nms��λΪms
//��SystemCoreClock=16M,nms<=0xffffff*1000/16000000=1048;
void Delay_Ms(unsigned short nms)
{
    unsigned int temp;

    SysTick->LOAD = (unsigned int)(nms * fac_ms); //ʱ�����(SysTick->LOADΪ24bit)
    SysTick->VAL = 0x00; //��ռ�����
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk; //��ʼ����,ʱ��ԴΪ��ʱ��(0Ϊ��ʱ��/2)

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16))); //�ȴ�ʱ�䵽��,�鿴CTRL�ĵ�16λ��COUNTFLAG���Ƿ�Ϊ1,��0λ��ENABLE���Ƿ�Ϊ1

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //�رռ�����
    SysTick->VAL = 0x00; //��ռ�����
}
void Delay_Us(unsigned int nus)
{
    unsigned int temp;

    SysTick->LOAD = (unsigned int)(nus * fac_us); //ʱ�����
    SysTick->VAL = 0x00; //��ռ�����
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk; //��ʼ����,ʱ��ԴΪ��ʱ��(0Ϊ��ʱ��/2)

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16))); //�ȴ�ʱ�䵽��

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //�رռ�����
    SysTick->VAL = 0x00; //��ռ�����
}
