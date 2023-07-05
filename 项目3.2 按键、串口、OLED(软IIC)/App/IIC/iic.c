#include "iic.h"
#include "oledfont.h" 

#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78
#define COM				0x00  // OLED 指令（禁止修改）  写命令
#define DAT 			0x40  // OLED 数据（禁止修改）  写数据

//设置SDA输入模式
void SDA_IN(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    //根据GPIO组初始化GPIO时钟
    RCC_AHBPeriphClk_Enable(SCL_GPIO_CRM_CLK, ENABLE);

    gpio_init_struct.IT = GPIO_IT_NONE;
    gpio_init_struct.Mode = GPIO_MODE_INPUT_PULLUP;
    gpio_init_struct.Pins = SDA_PIN;
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(SDA_GPIO, &gpio_init_struct);
}

//设置SDA为输出模式
void SDA_OUT(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    //根据GPIO组初始化GPIO时钟
    RCC_AHBPeriphClk_Enable(SDA_GPIO_CRM_CLK, ENABLE);

    gpio_init_struct.IT = GPIO_IT_NONE;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pins = SDA_PIN;
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(SDA_GPIO, &gpio_init_struct);
}

//设置SCL电平
void I2C_SCL(int n)
{
    if(n == 1)
    {
        GPIO_WritePin(SCL_GPIO, SCL_PIN, GPIO_Pin_SET); //设置SCL为高电平
    }
    else
    {
        GPIO_WritePin(SCL_GPIO, SCL_PIN, GPIO_Pin_RESET); //设置SCL为低电平
    }
}

//设置SDA电平
void I2C_SDA(int n)
{
    if(n == 1)
    {
        GPIO_WritePin(SDA_GPIO, SDA_PIN, GPIO_Pin_SET); //设置SDA为高电平
    }
    else
    {
        GPIO_WritePin(SDA_GPIO, SDA_PIN, GPIO_Pin_RESET); //设置SDA为低电平
    }
}

//读取SDA电平
unsigned char READ_SDA(void)
{
    return GPIO_ReadPin(SDA_GPIO, SDA_PIN); //读取SDA电平
}

//I2C初始化
void I2C_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    //根据GPIO组初始化GPIO时钟
    RCC_AHBPeriphClk_Enable(SCL_GPIO_CRM_CLK | SDA_GPIO_CRM_CLK, ENABLE);

    //GPIO_SCL初始化设置
    gpio_init_struct.IT = GPIO_IT_NONE;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pins = SCL_PIN;
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(SCL_GPIO, &gpio_init_struct);
    //GPIO_SDA初始化设置
    gpio_init_struct.IT = GPIO_IT_NONE;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pins = SDA_PIN;
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;
    GPIO_Init(SDA_GPIO, &gpio_init_struct);
    //SCL、SDA的初始化均为高电平
    I2C_SCL(1);
    I2C_SDA(1);

}

//I2C Start
void I2C_Start(void)
{
    SDA_OUT();
    I2C_SDA(1);
    I2C_SCL(1);
    //Delay_Us(1);//4
    I2C_SDA(0); //START:when CLK is high,DATA change form high to low
    //Delay_Us(1);//4
    I2C_SCL(0); //钳住I2C总线,准备发送或接收数据
}

//I2C Stop
void I2C_Stop(void)
{
    SDA_OUT();
    I2C_SCL(0);
    I2C_SDA(0); //STOP:when CLK is high DATA change form low to high
    //Delay_Us(1);//4
    I2C_SCL(1);
    I2C_SDA(1); //发送I2C总线结束信号
    //Delay_Us(1);//4
}

//I2C_Wait_ack 返回HAL_OK表示wait成功,返回HAL_ERROR表示wait失败
unsigned char I2C_Wait_Ack(void) //IIC_Wait_ack,返回wait失败或是成功
{
    unsigned char ucErrTime = 0;

    SDA_IN();
    I2C_SDA(1);
    //Delay_Us(1);
    I2C_SCL(1);
    //Delay_Us(1);

    while(READ_SDA())
    {
        ucErrTime++;

        if(ucErrTime > 250)
        {
            I2C_Stop();
            return HAL_ERROR;
        }
    }

    I2C_SCL(0);
    return HAL_OK;
}

//产生ACK应答
void I2C_Ack(void)
{
    I2C_SCL(0);
    SDA_OUT();
    I2C_SDA(0);
    //Delay_Us(1);//2
    I2C_SCL(1);
    //Delay_Us(1);//2
    I2C_SCL(0);
}

//产生NACK应答
void I2C_NAck(void)
{
    I2C_SCL(0);
    SDA_OUT();
    I2C_SDA(1);
    //Delay_Us(1);//2
    I2C_SCL(1);
    //Delay_Us(1);//2
    I2C_SCL(0);
}

//I2C_Send_Byte,入口参数为要发送的字节
void I2C_Send_Byte(unsigned char txd)
{
    unsigned char cnt = 0;

    SDA_OUT();
    I2C_SCL(0);

    for(cnt = 0; cnt < 8; cnt++)
    {
        I2C_SDA((txd & 0x80) >> 7);
        txd <<= 1;
        I2C_SCL(1);
        I2C_SCL(0);
    }
}

//I2C_Read_Byte,入口参数为是否要发送ACK信号
unsigned char I2C_Read_Byte(unsigned char ack)
{
    unsigned char cnt, rec = 0;

    SDA_IN();

    for(cnt = 0; cnt < 8; cnt++)
    {
        I2C_SCL(0);
        //Delay_Us(2);
        I2C_SCL(1);
        rec <<= 1;

        if(READ_SDA())
        {
            rec++;
        }

        //Delay_Us(1);
    }

    if(!ack)
    {
        I2C_NAck();
    }
    else
    {
        I2C_Ack();
    }

    return rec;
}

void WriteCmd(unsigned char I2C_Command)//写命令
{
		I2C_Start();
    I2C_Send_Byte(OLED_ADDRESS);
    I2C_Wait_Ack();
    I2C_Send_Byte(0x00);
    I2C_Wait_Ack();
    I2C_Send_Byte(I2C_Command);
    I2C_Wait_Ack();
    I2C_Stop();
}

void WriteDat(unsigned char I2C_Data)//写数据
{
		I2C_Start();
    I2C_Send_Byte(OLED_ADDRESS);
    I2C_Wait_Ack();
    I2C_Send_Byte(0x40);
    I2C_Wait_Ack();
    I2C_Send_Byte(I2C_Data);
    I2C_Wait_Ack();
    I2C_Stop();
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




