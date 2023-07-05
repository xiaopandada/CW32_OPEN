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


void KEYDRIVER_Init(void)
{    
    GPIO_InitTypeDef GPIO_InitStruct;

    __RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.IT = GPIO_IT_NONE; //key 

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pins = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
    GPIO_Init( CW_GPIOA, &GPIO_InitStruct);
}

void KEYMSG_StructInit(void)
{
	uint8_t i;
	uint8_t cnt;

	memset(&keyMsgCB,0,sizeof(KEYMSG_CB));

	cnt = (KEY_NAME_MAX + 7)/8;
	for(i=0; i < cnt; i++)
	{
		keyMsgCB.newKeyValue[i] = 0xFF;
		keyMsgCB.KeyValue[i] 	= 0xFF;
		keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_UP);
	}
}

void KEYDRIVER_CALLBACK_Scan(uint8_t* p8bitKeyValueArray)
{	
	uint16_t keyValue = 0; 
  uint8_t key_tmp = GPIO_ReadPin(CW_GPIOA, GPIO_PIN_1 );
  keyValue |= (!!key_tmp) << 0;
  key_tmp = GPIO_ReadPin(CW_GPIOA, GPIO_PIN_2 );
  keyValue |= (!!key_tmp) << 1;
	key_tmp = GPIO_ReadPin(CW_GPIOA, GPIO_PIN_3 );
  keyValue |= (!!key_tmp) << 2;
	*((uint8_t *)p8bitKeyValueArray) = (keyValue & 0x00FF);
}

void KEYMSG_RegisterKeyScanCallback(void(*fun)(uint8_t* p8bitKeyValueArray))
{
	keyMsgCB.KeyUpdate = fun;
}

void KEYMSG_CALLBACK_Scan(uint32_t param)
{
		keyMsgCB.KeyTimeBase++;    
}
void KEYMSG_Init(void)
{
    KEYDRIVER_Init();
    KEYMSG_StructInit();
    KEYMSG_RegisterKeyScanCallback(KEYDRIVER_CALLBACK_Scan);
    TimerSlice_SetTask(SLICE_ID_KEY,KEY_SCAN_TIME,KEYMSG_CALLBACK_Scan,0);
}
void KEYMSG_ScanProcess(void)
{
	uint8_t 		i;
	KEYMSG_INFO_CB	*pKeyInfo 	= 0;      
	
	uint8_t			keyCnt		        = 0;			
	uint8_t 		lastState 		    = 0;	
	uint8_t 		currentState 	    = 0;	
	

	if(0 == keyMsgCB.KeyUpdate)
		return ;

	(*keyMsgCB.KeyUpdate)(keyMsgCB.newKeyValue);

	for(i = 0; i < KEY_NAME_MAX; i++)
	{			
		currentState = (keyMsgCB.newKeyValue[i/8] >> ( i % 8 )) & 0x01;	
		lastState = (keyMsgCB.KeyValue[i/8] >> ( i % 8 )) & 0x01; 
		if(currentState != lastState)
		{		
			pKeyInfo = &keyMsgCB.keyMsgInfo[i];
			if(KEY_DOWN == currentState)
			{		
				keyCnt = (keyMsgCB.keyCountAndState[i] >> 4) + 1;
				keyMsgCB.keyCountAndState[i] = (keyCnt << 4) | (keyMsgCB.keyCountAndState[i] & 0x0F);
				if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_DOWN)))
				{
					KEYMSG_ThrowMsg((KEY_NAME_E)i, KEY_MSG_DOWN);				
				}
				keyMsgCB.keyCountAndState[i] &= ~(1 << KEY_STATE_UP);			
				keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_DOWN);				
			}
			else if (KEY_UP == currentState)
			{	
				if(pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_UP)) 
				{
					KEYMSG_ThrowMsg((KEY_NAME_E)i, KEY_MSG_UP);					
				}
				keyMsgCB.keyCountAndState[i] &= ~(1 << KEY_STATE_DOWN);				
				keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_UP);							
			}		
			else
			{
			
			}
			pKeyInfo->keyStateChangeTime = keyMsgCB.KeyTimeBase;
		}	
	}
	memcpy(keyMsgCB.KeyValue,keyMsgCB.newKeyValue,(KEY_NAME_MAX + 7)/8);
	KEYMSG_MsgProcess();
}

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


void KEYMSG_MsgProcess(void)
{
	uint8_t keyCnt = 0;	
  KEY_NAME_E key = 0;
  
	for( key = (KEY_NAME_E)0; key < KEY_NAME_MAX; key++)
	{	
		KEYMSG_INFO_CB *pKeyInfo = &keyMsgCB.keyMsgInfo[key];	
		uint32_t beforTime = 0;
    beforTime = ((keyMsgCB.KeyTimeBase + 0xFFFF - pKeyInfo->keyStateChangeTime) % 0xFFFF) * KEY_SCAN_TIME; 
		pKeyInfo->FreeTime = KEYMSG_FREE_TIME_MAX;
		if(keyMsgCB.keyCountAndState[key] & 0xF0)
		{	
				if(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_DOWN))
				{	
						if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_HOLD)) && !(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_HOLD_DOWN)))
						{
							if(beforTime >= keyHoldThreshHold[key])
							{ 
								KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_HOLD);
								keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_HOLD_DOWN);
							}
							else
								pKeyInfo->FreeTime = keyHoldThreshHold[key] - beforTime;
						}
						if(pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_REPEAT) && beforTime >= keyHoldThreshHold[key])
						{		
							if(beforTime >= keyHoldThreshHold[key] + keyRepeatTime[key])
							{
								KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_REPEAT);
								pKeyInfo->keyStateChangeTime = (keyMsgCB.KeyTimeBase  + 0xFFFF - keyHoldThreshHold[key] / KEY_SCAN_TIME) % 0xFFFF;
								keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_HOLD_DOWN);
							}
							else
								pKeyInfo->FreeTime = keyHoldThreshHold[key] + keyRepeatTime[key] - beforTime;
						}
				}	
				else 
				{
					if(beforTime < keyRaiseThreshHold[key])	
						pKeyInfo->FreeTime = keyRaiseThreshHold[key] - beforTime;
					else
					{	
						if(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_HOLD_DOWN))
						{	
							keyMsgCB.keyCountAndState[key] &= ~(1 << KEY_STATE_HOLD_DOWN);			
						}
						else 
						{	
							keyCnt = (((keyMsgCB.keyCountAndState[key] & 0xF0) >> 4) - 1) + KEY_MSG_ONE_CLICK;
							if(keyCnt >= KEY_MSG_MAX)
							{	
								keyCnt = KEY_MSG_NULL;
							}
							if(pKeyInfo->keyMsgSupportType & (1 << keyCnt))
							{						
								KEYMSG_ThrowMsg((KEY_NAME_E)key, (KEY_MSG_E)keyCnt);
							}
						}
						keyMsgCB.keyCountAndState[key] &= 0x0F;
						keyMsgCB.keyCountAndState[key] &= ~(1 << KEY_STATE_LIFT_UP);
					}			
				}		
		}
		if(!(keyMsgCB.keyCountAndState[key] & 0xF0))
		{
			if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_LIFT)) && !(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_LIFT_UP)))
			{
					if(beforTime < keyHoldThreshLift[key])
							pKeyInfo->FreeTime = keyHoldThreshLift[key]  - beforTime;
					else
					{
						KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_LIFT);
						keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_LIFT_UP);
					}
			}
		}
	}	
}

void KEYMSG_RegisterMsgService(KEY_NAME_E keyName, KEY_MSG_E keyMsg, void(*service)(uint32_t param), uint32_t param)
{
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
	keyMsgCB.keyMsgInfo[keyName].service[keyMsg] = service;
	keyMsgCB.keyMsgInfo[keyName].param[keyMsg] = param;
	keyMsgCB.keyMsgInfo[keyName].keyMsgSupportType |= (1 << keyMsg);
}

