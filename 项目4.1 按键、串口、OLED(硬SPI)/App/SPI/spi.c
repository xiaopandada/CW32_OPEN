#include "Key.h"

KEYMSG_CB keyMsgCB = {0};

// 每个按键的长抬门限 单位ms 用户可以修改
uint16_t keyHoldThreshLift[KEY_NAME_MAX] = {3000,3000,3000,3000};
// 每个按键的长按门限 单位ms 用户可以修改
uint16_t keyHoldThreshHold[KEY_NAME_MAX] = {2000,2000,2000,2000};
// 每个按键抬起门限 单位ms 用户可以修改
uint16_t keyRaiseThreshHold[KEY_NAME_MAX] = {300,300,300,300};
// 每个按键的重复周期 单位ms 用户可以修改
uint16_t keyRepeatTime[KEY_NAME_MAX] = {200,200,200,200};

//配置按键引脚模式
void KEYDRIVER_Init(void)
{    
    GPIO_InitTypeDef GPIO_InitStruct;
    //打开时钟总线使能
    __RCC_GPIOB_CLK_ENABLE();
    //配置IO中断
    GPIO_InitStruct.IT = GPIO_IT_NONE; //key 
    //配置IO输入输出模式
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pins = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
    GPIO_Init( CW_GPIOA, &GPIO_InitStruct);
}
// 结构体初始化接口
void KEYMSG_StructInit(void)
{
	uint8_t i;
	uint8_t cnt;
	// 初始化结构体
	memset(&keyMsgCB,0,sizeof(KEYMSG_CB));
	//	初始化按键为释放状态
	cnt = (KEY_NAME_MAX + 7)/8;
	for(i=0; i < cnt; i++)
	{
		keyMsgCB.newKeyValue[i] = 0xFF;
		keyMsgCB.KeyValue[i] 	= 0xFF;
		keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_UP);
	}
}
//获取键值
void KEYDRIVER_CALLBACK_Scan(uint8_t* p8bitKeyValueArray)
{	
	uint16_t keyValue = 0; 
  uint8_t key_tmp = GPIO_ReadPin(CW_GPIOA, GPIO_PIN_1 );
  keyValue |= (!!key_tmp) << 0;
  key_tmp = GPIO_ReadPin(CW_GPIOA, GPIO_PIN_2 );
  keyValue |= (!!key_tmp) << 1;
	key_tmp = GPIO_ReadPin(CW_GPIOA, GPIO_PIN_3 );
  keyValue |= (!!key_tmp) << 2;
//	key_tmp = GPIO_ReadPin(CW_GPIOA, GPIO_PIN_4 );
//  keyValue |= (!!key_tmp) << 3;
	*((uint8_t *)p8bitKeyValueArray) = (keyValue & 0x00FF);
}

//  注册按键硬件读取接口 需要注意的是：按键按下为0，按键释放为1
void KEYMSG_RegisterKeyScanCallback(void(*fun)(uint8_t* p8bitKeyValueArray))
{
	keyMsgCB.KeyUpdate = fun;
}
// 按键处理回调函数，在定时器控制下，被周期调用，实现按键扫描
void KEYMSG_CALLBACK_Scan(uint32_t param)
{
		keyMsgCB.KeyTimeBase++;
    
//		KEYMSG_ScanProcess();
    
}
// 模块初始化接口
void KEYMSG_Init(void)
{
    KEYDRIVER_Init();
    KEYMSG_StructInit();
    KEYMSG_RegisterKeyScanCallback(KEYDRIVER_CALLBACK_Scan);
    // Kry Scan Slice
    TimerSlice_SetTask(SLICE_ID_KEY,KEY_SCAN_TIME,KEYMSG_CALLBACK_Scan,0);
}
// 按键扫描处理
void KEYMSG_ScanProcess(void)
{
	uint8_t 		i;
	KEYMSG_INFO_CB	*pKeyInfo 	= 0;      // 按键信息
	
	uint8_t			keyCnt		        = 0;			// 按键次数计值
	uint8_t 		lastState 		    = 0;			// 当前按键状态
	uint8_t 		currentState 	    = 0;			// 当前按键状态
	
	// 调用回调函数读取按键值，未注册回调函数时退出
	if(0 == keyMsgCB.KeyUpdate)
		return ;
	// 最新按键值，定时更新，每个bit代表一个按键状态，
	(*keyMsgCB.KeyUpdate)(keyMsgCB.newKeyValue);
	// 对每个按键当前的状态进行处理，将按键状态放入各自的按键信息节点中
	for(i = 0; i < KEY_NAME_MAX; i++)
	{			
		// 获得当前按键状态
		currentState = (keyMsgCB.newKeyValue[i/8] >> ( i % 8 )) & 0x01;	
		// 获得上一次按键状态
		lastState = (keyMsgCB.KeyValue[i/8] >> ( i % 8 )) & 0x01; 
		// 当前按键状态与上一次按键状态不相同时
		if(currentState != lastState)
		{		
			// 获取当前按键信息结构体指针
			pKeyInfo = &keyMsgCB.keyMsgInfo[i];
			// 按键状态为按下
			if(KEY_DOWN == currentState)
			{		
				// 记录按键按下的次数	
				keyCnt = (keyMsgCB.keyCountAndState[i] >> 4) + 1;
				keyMsgCB.keyCountAndState[i] = (keyCnt << 4) | (keyMsgCB.keyCountAndState[i] & 0x0F);
				// 判断是否有注册按下消息类型
				if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_DOWN)))
				{
					// 抛出按下消息
					KEYMSG_ThrowMsg((KEY_NAME_E)i, KEY_MSG_DOWN);				
				}
				// 清除按钮释放状态
				keyMsgCB.keyCountAndState[i] &= ~(1 << KEY_STATE_UP);			
				// 设置按钮状态为按下
				keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_DOWN);				
			}
			
			// 按键状态为释放
			else if (KEY_UP == currentState)
			{	
				// 判断是否有注册抬起消息类型
				if(pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_UP)) 
				{
					// 抛出抬起消息
					KEYMSG_ThrowMsg((KEY_NAME_E)i, KEY_MSG_UP);					
				}
				
				// 清除按钮按下状态
				keyMsgCB.keyCountAndState[i] &= ~(1 << KEY_STATE_DOWN);				
				// 设置按钮状态为释放
				keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_UP);							
			}		
			// 非法状态、不处理
			else
			{
			
			}
			// 记录按键改变时时间
			pKeyInfo->keyStateChangeTime = keyMsgCB.KeyTimeBase;
		}	
	}
	// 更新按键值
	memcpy(keyMsgCB.KeyValue,keyMsgCB.newKeyValue,(KEY_NAME_MAX + 7)/8);
	// 按键消息处理
	KEYMSG_MsgProcess();
}

// 执行回调函数
uint8_t KEYMSG_ThrowMsg(KEY_NAME_E keyName, KEY_MSG_E keyMsg)
{
	KEYMSG_INFO_CB *pKeyInfo = 0;
	if(KEY_NAME_MAX <= keyName)
		return 0;
  pKeyInfo = &keyMsgCB.keyMsgInfo[keyName];	
	if(pKeyInfo->service[keyMsg])
		(*pKeyInfo->service[keyMsg])(pKeyInfo->param[keyMsg]);
	return 1;
}

// 按键消息处理
void KEYMSG_MsgProcess(void)
{
	uint8_t keyCnt = 0;				// 当前按键计数值
	// 对每个按键当前的状态进行处理
  KEY_NAME_E key = 0;
  
	for( key = (KEY_NAME_E)0; key < KEY_NAME_MAX; key++)
	{	
		// 获取当前按键信息结构体指针
		KEYMSG_INFO_CB *pKeyInfo = &keyMsgCB.keyMsgInfo[key];	
		// 获取按键状态变更过去的时间
		uint32_t beforTime = 0;
    beforTime = ((keyMsgCB.KeyTimeBase + 0xFFFF - pKeyInfo->keyStateChangeTime) % 0xFFFF) * KEY_SCAN_TIME; 
		// 默认空闲
		pKeyInfo->FreeTime = KEYMSG_FREE_TIME_MAX;
    
		// 按键有按下计数：计数是从1开始
		if(keyMsgCB.keyCountAndState[key] & 0xF0)
		{	
				// 按下状态
				if(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_DOWN))
				{	
						// 注册了长按消息 长按消息还没触发
						if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_HOLD)) && !(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_HOLD_DOWN)))
						{
							//  时间足够
							if(beforTime >= keyHoldThreshHold[key])
							{ 
//                printf("%u   %u   %u ",keyMsgCB.KeyTimeBase,beforTime,keyHoldThreshHold[key]);
								// 抛出按键长按消息
								KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_HOLD);
								// 设置按钮状态为长按下
								keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_HOLD_DOWN);
							}
							// 剩余时间
							else
								pKeyInfo->FreeTime = keyHoldThreshHold[key] - beforTime;
						}
						// 注册了重复消息 时间也超过了长按
						if(pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_REPEAT) && beforTime >= keyHoldThreshHold[key])
						{		
							// 达到一次重复周期就抛出一次
							if(beforTime >= keyHoldThreshHold[key] + keyRepeatTime[key])
							{
								// 抛出按键重复消息
								KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_REPEAT);
								// 更新时间 为下一次重复消息做准备
								pKeyInfo->keyStateChangeTime = (keyMsgCB.KeyTimeBase  + 0xFFFF - keyHoldThreshHold[key] / KEY_SCAN_TIME) % 0xFFFF;
								// 设置按钮状态为长按下
								keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_HOLD_DOWN);
							}
							// 剩余时间
							else
								pKeyInfo->FreeTime = keyHoldThreshHold[key] + keyRepeatTime[key] - beforTime;
						}
				}	
				// 说明按键安过了已经释放 但是还么有处理
				else 
				{
					// 判断是否超过抬起的门限
					if(beforTime < keyRaiseThreshHold[key])	
						// 记录剩余时间
						pKeyInfo->FreeTime = keyRaiseThreshHold[key] - beforTime;
					else
					{	
						// 之前进入了长按或者重复消息
						if(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_HOLD_DOWN))
						{	
							keyMsgCB.keyCountAndState[key] &= ~(1 << KEY_STATE_HOLD_DOWN);			
						}
						// 点击状态结束
						else 
						{	
							// 按键点击次数是从1开始，故得-1。
							keyCnt = (((keyMsgCB.keyCountAndState[key] & 0xF0) >> 4) - 1) + KEY_MSG_ONE_CLICK;
							
							// 判断点击数
							if(keyCnt >= KEY_MSG_MAX)
							{	
								keyCnt = KEY_MSG_NULL;
							}
							
							// 根据按键计算值查看按键的按键消息类型注册
							if(pKeyInfo->keyMsgSupportType & (1 << keyCnt))
							{						
								// 抛出各点击消息
								KEYMSG_ThrowMsg((KEY_NAME_E)key, (KEY_MSG_E)keyCnt);
							}
						}
						// 清空按键次数
						keyMsgCB.keyCountAndState[key] &= 0x0F;
						// 清空长抬
						keyMsgCB.keyCountAndState[key] &= ~(1 << KEY_STATE_LIFT_UP);
					}			
				}		
		}
		// 之前的按键事件已经处理
		if(!(keyMsgCB.keyCountAndState[key] & 0xF0))
		{
			// 注册了长抬消息 长抬消息还没触发
			if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_LIFT)) && !(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_LIFT_UP)))
			{
					//  时间也还没到
					if(beforTime < keyHoldThreshLift[key])
							// 记录剩余时间
							pKeyInfo->FreeTime = keyHoldThreshLift[key]  - beforTime;
					else
					{
						// 抛出按键长抬消息
						KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_LIFT);
						// 设置按钮状态为长抬
						keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_LIFT_UP);
					}
			}
		}
	}	
}

// 注册按键消息服务
void KEYMSG_RegisterMsgService(KEY_NAME_E keyName, KEY_MSG_E keyMsg, void(*service)(uint32_t param), uint32_t param)
{
	// 参数合法性检验
	if(keyName >= KEY_NAME_MAX)
	{
		return ;
	}
	if(keyMsg >= KEY_MSG_MAX)
	{
		return ;
	}
	if(!service)
	{
		return ;
	}
	// 配置按键服务
	keyMsgCB.keyMsgInfo[keyName].service[keyMsg] = service;
	keyMsgCB.keyMsgInfo[keyName].param[keyMsg] = param;
	keyMsgCB.keyMsgInfo[keyName].keyMsgSupportType |= (1 << keyMsg);
}

