#ifndef __IIC_H
#define __IIC_H



#include "main.h"

		#define HAL_ERROR 0
    #define HAL_OK 1
		
    #define SCL_PIN            GPIO_PIN_6
    #define SCL_GPIO           CW_GPIOB
    #define SCL_GPIO_CRM_CLK   RCC_AHB_PERIPH_GPIOB

    #define SDA_PIN            GPIO_PIN_7
    #define SDA_GPIO           CW_GPIOB
    #define SDA_GPIO_CRM_CLK   RCC_AHB_PERIPH_GPIOB

typedef struct
{
	char string1_new[20];	 	// OLED��1��������
	char string2_new[20];	 	// OLED��2��������
	char string3_new[20];	 	// OLED��3��������
	char string4_new[20];	 	// OLED��4��������
	
	char string1_oled[20];	 	// OLED��1�о�����
	char string2_oled[20];	 	// OLED��2�о�����
	char string3_oled[20];	 	// OLED��3�о�����
	char string4_oled[20];	 	// OLED��4�о�����
}OLED_CB;

//I2C��ʼ��
void I2C_Init(void);

void OLED_Init(void);
void OLED_CLS(void);

//����SCL��ƽ
void I2C_SCL(int n);

void OLED_ShowStrM2(uint8_t Line, uint8_t Column, char *String);

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);

void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);

void OLED_ShowStr(unsigned char x, unsigned char y, char ch[], unsigned char TextSize);

void OLED_OFF(void);

void OLED_ON(void);



#endif



