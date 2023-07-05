#ifndef __SYS_H
#define __SYS_H



#include "main.h"


void GPIO_Configuration(void);
void RCC_Configuration(void);

//SYSCLK:œµÕ≥ ±÷”
void Delay_Init(void);
void Delay_Ms(unsigned short nms);
void Delay_Us(unsigned int nus);
#endif



