#include "iic.h"
#include "oledfont.h" 

#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
#define COM				0x00  // OLED 指令（禁止修改）  写命令
#define DAT 			0x40  // OLED 数据（禁止修改）  写数据


void I2C_Configuration(void)
{
	
	  GPIO_InitTypeDef GPIO_InitStructure;
		I2C_InitTypeDef I2C_InitStruct;
	
		__RCC_I2C1_CLK_ENABLE();
		__RCC_GPIOB_CLK_ENABLE();	
    
	  PB06_AFx_I2C1SCL();
	  PB07_AFx_I2C1SDA(); 

		GPIO_InitStructure.Pins = GPIO_PIN_6 | GPIO_PIN_7;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;   ////I2C必须开漏输出
		GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
		GPIO_Init(CW_GPIOB, &GPIO_InitStructure);
	
		I2C_InitStruct.I2C_BaudEn = ENABLE;
		I2C_InitStruct.I2C_Baud = 0x02;
    I2C_InitStruct.I2C_FLT = DISABLE;
    I2C_InitStruct.I2C_AA = DISABLE;  //DISABLE ENABLE
    
		I2C1_DeInit(); 
    I2C_Master_Init(CW_I2C1,&I2C_InitStruct);//初始化模块
		I2C_Cmd(CW_I2C1,ENABLE);  //模块使能
}

void I2C_MasterWriteEepromData1(I2C_TypeDef *I2Cx,uint8_t u8Addr,uint8_t *pu8Data,uint32_t u32Len)
{
    uint8_t u8i=0,u8State;
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(1)
	{
		while(0 == I2C_GetIrq(I2Cx))
		{;}
		u8State = I2C_GetState(I2Cx);
		switch(u8State)
		{
			case 0x08:   //发送完START信号
				I2C_GenerateSTART(I2Cx, DISABLE);
				I2C_Send7bitAddress(I2Cx, OLED_ADDRESS,0X00);  //从设备地址发送
				break;
			case 0x18:   //发送完SLA+W信号,ACK已收到
				I2C_SendData(I2Cx,u8Addr);//从设备内存地址发送
				break;
			case 0x28:   //发送完1字节数据：发送EEPROM中memory地址也会产生，发送后面的数据也会产生	
				I2C_SendData(I2Cx,pu8Data[u8i++]);
				break;
			case 0x20:   //发送完SLA+W后从机返回NACK
			case 0x38:    //主机在发送 SLA+W 阶段或者发送数据阶段丢失仲裁  或者  主机在发送 SLA+R 阶段或者回应 NACK 阶段丢失仲裁
				I2C_GenerateSTART(I2Cx, ENABLE);
				break;
			case 0x30:   //发送完一个数据字节后从机返回NACK
				I2C_GenerateSTOP(I2Cx, ENABLE);
				break;
			default:
				break;
		}			
		if(u8i>u32Len)
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);//此顺序不能调换，出停止条件
			I2C_ClearIrq(I2Cx);
			break;
		}
		I2C_ClearIrq(I2Cx);			
	}
}

void WriteCmd(unsigned char I2C_Command)//写命令
{
	I2C_MasterWriteEepromData1(CW_I2C1,COM,&I2C_Command,1);
}

void WriteDat(unsigned char I2C_Data)//写数据
{
	I2C_MasterWriteEepromData1(CW_I2C1,DAT,&I2C_Data,1);
}

//初始化SSD1306					    
void OLED_Init(void)
{ 	
 
	WriteCmd(0xAE); //display off
	
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //亮度调节 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel

}  
void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
}

void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
//void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
void OLED_ShowStr(unsigned char x, unsigned char y, char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}


void OLED_ShowStrM2(uint8_t Line, uint8_t Column, char *String)
{
	OLED_ShowStr(Line,Column,String,2);
}
//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}

