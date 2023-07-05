#ifndef __IIC_H
#define __IIC_H



#include "main.h"


typedef struct
{
	char string1_new[20];	 	// OLED第1行新数据
	char string2_new[20];	 	// OLED第2行新数据
	char string3_new[20];	 	// OLED第3行新数据
	char string4_new[20];	 	// OLED第4行新数据
	
	char string1_oled[20];	 	// OLED第1行旧数据
	char string2_oled[20];	 	// OLED第2行旧数据
	char string3_oled[20];	 	// OLED第3行旧数据
	char string4_oled[20];	 	// OLED第4行旧数据
}OLED_CB;



void I2C_Configuration(void);
void OLED_Init(void);
void OLED_CLS(void);


void OLED_ShowStrM2(uint8_t Line, uint8_t Column, char *String);

void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);

void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);

void OLED_ShowStr(unsigned char x, unsigned char y, char ch[], unsigned char TextSize);

void OLED_OFF(void);

void OLED_ON(void);



#endif



