#ifndef __KEY_H
#define __KEY_H



#include "main.h"



/******************************************************************************
* 【用户定制参数区】
* 根据系统需要修改，务必严格按照注释内容设置参数
// 按键名定义,用户根据需要自行添加按键个数
******************************************************************************/
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

#define	KEY_SCAN_TIME					50  					// 按键扫描周期，单位：ms，必须为虚拟定时器的分辨率的N倍(N>=1)
#define	KEY_MSG_QUEUE_SIZE		10						// 按键消息缓冲区尺寸，即最多可保持多少个消息，注意实际上可用元素为该值-1
#define KEYMSG_FREE_TIME_MAX	(0xFFFFFFFF - 100)

// 按键物理状态
typedef enum
{
	KEY_DOWN = 0,												// 按键按下
	KEY_UP,															// 按键释放
}KEY_E;

// 按键状态对应
typedef enum
{
	KEY_STATE_DOWN = 0,										// 按键按下
	KEY_STATE_UP,													// 按键抬起
	KEY_STATE_LIFT_UP,										// 按键长抬
	KEY_STATE_HOLD_DOWN,									// 按键长按
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
	uint16_t 	keyStateChangeTime;												// 按钮改变时刻时间
	uint32_t 	FreeTime;																	// 空闲时间
	uint16_t	keyMsgSupportType;												// 该按键所支持的消息类型，bit对应KEY_MSG_E，支持为1不支持为0
	void	(*service[KEY_MSG_MAX])(uint32_t param);			// 按键响应
	uint32_t	param[KEY_MSG_MAX];												// 按键响应服务的参数
}KEYMSG_INFO_CB;	
// 按键控制结构体

typedef struct
{
	uint32_t		  KeyTimeBase;								      		// 按键时基准
	uint8_t				newKeyValue[(KEY_NAME_MAX + 7)/8];		// 最新按键值，定时更新，每个bit代表一个按键状态，
	uint8_t				KeyValue[(KEY_NAME_MAX + 7)/8];				// 保存的按键值状态 每次定时更新的结果会与该值做比较，若不相等则有按键发生变化						
	uint8_t				keyCountAndState[KEY_NAME_MAX];				// 按键计数以及按键当前状态 
	KEYMSG_INFO_CB		keyMsgInfo[KEY_NAME_MAX];					// 按键信息节点
	void(*KeyUpdate)	(uint8_t *p8bitKeyValueArray);		// 按键刷新函数，需要使用者提供回调函数接口，在回调函数中为按键赋值
}KEYMSG_CB;


// 模块初始化接口
void KEYMSG_Init(void);
// 按键处理回调函数，在定时器控制下，被周期调用，实现按键扫描
void KEYMSG_CALLBACK_Scan(uint32_t param);
// 按键扫描处理
void KEYMSG_ScanProcess(void);
// 执行回调函数
uint8_t KEYMSG_ThrowMsg(KEY_NAME_E keyName, KEY_MSG_E keyMsg);
// 按键消息处理
void KEYMSG_MsgProcess(void);
// 注册按键消息服务
void KEYMSG_RegisterMsgService(KEY_NAME_E keyName, KEY_MSG_E keyMsg, void(*service)(uint32_t param), uint32_t param);





#endif



