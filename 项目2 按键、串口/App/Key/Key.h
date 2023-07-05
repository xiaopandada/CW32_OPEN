#ifndef __KEY_H
#define __KEY_H



#include "main.h"


typedef enum
{
	KEY_A1_KEY = 0,
	KEY_A2_KEY,
  KEY_A3_KEY,
	KEY_A4_KEY,
	KEY_NAME_MAX,
	KEY_NAME_NULL,
}KEY_NAME_E;



// 包含必要的模块头文件

#define	KEY_SCAN_TIME					50  					// 按键消息缓冲区尺寸，即最多可保持多少个消息，注意实际上可用元素为该值-1
#define KEYMSG_FREE_TIME_MAX	(0xFFFFFFFF - 100)

// 按键物理状态
typedef enum
{
	KEY_DOWN = 0,											
	KEY_UP,														
}KEY_E;

// 按键状态对应
typedef enum
{
	KEY_STATE_DOWN = 0,										
	KEY_STATE_UP,												
	KEY_STATE_LIFT_UP,									
	KEY_STATE_HOLD_DOWN,								
}KEY_STATE_E;

// 按键消息定义，此部分固定 ,用户不可修改
typedef enum
{
	KEY_MSG_NULL = 0,										// 非法按键消息
	KEY_MSG_DOWN,												// 按键按下消息
	KEY_MSG_UP,													// 按键弹起消息
	KEY_MSG_HOLD,												// 按键长按消息
	KEY_MSG_LIFT,												// 按键长抬消息
	KEY_MSG_REPEAT,											// 按键重复消息
	KEY_MSG_ONE_CLICK,											// 单击消息
	KEY_MSG_MAX,												// 按键消息最大数
}KEY_MSG_E;	

// 按键信息节点	
typedef struct	
{	
	uint16_t 	keyStateChangeTime;									
	uint32_t 	FreeTime;																
	uint16_t	keyMsgSupportType;												
	void	(*service[KEY_MSG_MAX])(uint32_t param);			
	uint32_t	param[KEY_MSG_MAX];										
}KEYMSG_INFO_CB;	
// 按键控制结构体

typedef struct
{
	uint32_t		  KeyTimeBase;								      		
	uint8_t				newKeyValue[(KEY_NAME_MAX + 7)/8];		
	uint8_t				KeyValue[(KEY_NAME_MAX + 7)/8];					
	uint8_t				keyCountAndState[KEY_NAME_MAX];			
	KEYMSG_INFO_CB		keyMsgInfo[KEY_NAME_MAX];				
	void(*KeyUpdate)	(uint8_t *p8bitKeyValueArray);		
}KEYMSG_CB;


// 模块初始化接口
void KEYMSG_Init(void);

void KEYMSG_CALLBACK_Scan(uint32_t param);

void KEYMSG_ScanProcess(void);

uint8_t KEYMSG_ThrowMsg(KEY_NAME_E keyName, KEY_MSG_E keyMsg);

void KEYMSG_MsgProcess(void);
// 注册按键消息服务
void KEYMSG_RegisterMsgService(KEY_NAME_E keyName, KEY_MSG_E keyMsg, void(*service)(uint32_t param), uint32_t param);





#endif



