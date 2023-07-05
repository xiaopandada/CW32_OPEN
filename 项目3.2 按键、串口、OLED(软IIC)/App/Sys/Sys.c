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
  /* 0. HSI使能并校准 */
  RCC_HSI_Enable(RCC_HSIOSC_DIV6);

  /* 1. 设置HCLK和PCLK的分频系数　*/
  RCC_HCLKPRS_Config(RCC_HCLK_DIV1);
  RCC_PCLKPRS_Config(RCC_PCLK_DIV1);
  
  /* 2. 使能PLL，通过PLL倍频到64MHz */
  RCC_PLL_Enable(RCC_PLLSOURCE_HSI, 8000000, 8);     // HSI 默认输出频率8MHz
 // RCC_PLL_OUT();  //PC13脚输出PLL时钟
  
  ///< 当使用的时钟源HCLK大于24M,小于等于48MHz：设置FLASH 读等待周期为2 cycle
  ///< 当使用的时钟源HCLK大于48MHz：设置FLASH 读等待周期为3 cycle
  __RCC_FLASH_CLK_ENABLE();
  FLASH_SetLatency(FLASH_Latency_3);   
    
  /* 3. 时钟切换到PLL */
  RCC_SysClk_Switch(RCC_SYSCLKSRC_PLL);
  RCC_SystemCoreClockUpdate(64000000);	
}

static unsigned int fac_us = 0; //us延时倍乘数
static unsigned int fac_ms = 0; //ms延时倍乘数


//初始化延迟函数
//SYSTICK的时钟固定为HCLK时钟的1/8(M3、M4);SYSTICK=HCLK(M0+)
//SYSCLK:系统时钟
void Delay_Init(void)
{
    fac_us = SystemCoreClock / (1000000U); //systick时钟频率为HCLK时钟频率,递减一个计数,耗时1/SystemCoreClock秒,单位换成微妙为1000000/SystemCoreClock微秒,
    //1us需要的计数值:1us/(1000000/SystemCoreClock)us=SystemCoreClock/1000000=fac_us
    fac_ms = (unsigned short)fac_us * (1000U); //每1ms需要的systick时钟数,即每1ms systick的VAL减的数目
}

//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,最大延时为:
//nms<=0xffffff/fac_ms=0xffffff/(SystemCoreClock/1000000*1000)=0xffffff*1000/SystemCoreClock
//SystemCoreClock单位为Hz,nms单位为ms
//在SystemCoreClock=16M,nms<=0xffffff*1000/16000000=1048;
void Delay_Ms(unsigned short nms)
{
    unsigned int temp;

    SysTick->LOAD = (unsigned int)(nms * fac_ms); //时间加载(SysTick->LOAD为24bit)
    SysTick->VAL = 0x00; //清空计数器
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk; //开始倒数,时钟源为主时钟(0为主时钟/2)

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16))); //等待时间到达,查看CTRL的第16位（COUNTFLAG）是否为1,第0位（ENABLE）是否为1

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //关闭计数器
    SysTick->VAL = 0x00; //清空计数器
}
void Delay_Us(unsigned int nus)
{
    unsigned int temp;

    SysTick->LOAD = (unsigned int)(nus * fac_us); //时间加载
    SysTick->VAL = 0x00; //清空计数器
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk; //开始倒数,时钟源为主时钟(0为主时钟/2)

    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & 0x01) && !(temp & (1 << 16))); //等待时间到达

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; //关闭计数器
    SysTick->VAL = 0x00; //清空计数器
}
