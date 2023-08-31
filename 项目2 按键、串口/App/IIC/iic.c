#include "Key.h"

KEYMSG_CB keyMsgCB = {0};

// ÿ�������ĳ�̧���� ��λms �û������޸�
uint16_t keyHoldThreshLift[KEY_NAME_MAX] = {3000,3000,3000,3000};
// ÿ�������ĳ������� ��λms �û������޸�
uint16_t keyHoldThreshHold[KEY_NAME_MAX] = {2000,2000,2000,2000};
// ÿ������̧������ ��λms �û������޸�
uint16_t keyRaiseThreshHold[KEY_NAME_MAX] = {300,300,300,300};
// ÿ���������ظ����� ��λms �û������޸�
uint16_t keyRepeatTime[KEY_NAME_MAX] = {200,200,200,200};

//���ð�������ģʽ
void KEYDRIVER_Init(void)
{    
    GPIO_InitTypeDef GPIO_InitStruct;
    //��ʱ������ʹ��
    __RCC_GPIOB_CLK_ENABLE();
    //����IO�ж�
    GPIO_InitStruct.IT = GPIO_IT_NONE; //key 
    //����IO�������ģʽ
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Pins = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
    GPIO_Init( CW_GPIOA, &GPIO_InitStruct);
}
// �ṹ���ʼ���ӿ�
void KEYMSG_StructInit(void)
{
	uint8_t i;
	uint8_t cnt;
	// ��ʼ���ṹ��
	memset(&keyMsgCB,0,sizeof(KEYMSG_CB));
	//	��ʼ������Ϊ�ͷ�״̬
	cnt = (KEY_NAME_MAX + 7)/8;
	for(i=0; i < cnt; i++)
	{
		keyMsgCB.newKeyValue[i] = 0xFF;
		keyMsgCB.KeyValue[i] 	= 0xFF;
		keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_UP);
	}
}
//��ȡ��ֵ
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

//  ע�ᰴ��Ӳ����ȡ�ӿ� ��Ҫע����ǣ���������Ϊ0�������ͷ�Ϊ1
void KEYMSG_RegisterKeyScanCallback(void(*fun)(uint8_t* p8bitKeyValueArray))
{
	keyMsgCB.KeyUpdate = fun;
}
// ��������ص��������ڶ�ʱ�������£������ڵ��ã�ʵ�ְ���ɨ��
void KEYMSG_CALLBACK_Scan(uint32_t param)
{
		keyMsgCB.KeyTimeBase++;
    
//		KEYMSG_ScanProcess();
    
}
// ģ���ʼ���ӿ�
void KEYMSG_Init(void)
{
    KEYDRIVER_Init();
    KEYMSG_StructInit();
    KEYMSG_RegisterKeyScanCallback(KEYDRIVER_CALLBACK_Scan);
    // Kry Scan Slice
    TimerSlice_SetTask(SLICE_ID_KEY,KEY_SCAN_TIME,KEYMSG_CALLBACK_Scan,0);
}
// ����ɨ�账��
void KEYMSG_ScanProcess(void)
{
	uint8_t 		i;
	KEYMSG_INFO_CB	*pKeyInfo 	= 0;      // ������Ϣ
	
	uint8_t			keyCnt		        = 0;			// ����������ֵ
	uint8_t 		lastState 		    = 0;			// ��ǰ����״̬
	uint8_t 		currentState 	    = 0;			// ��ǰ����״̬
	
	// ���ûص�������ȡ����ֵ��δע��ص�����ʱ�˳�
	if(0 == keyMsgCB.KeyUpdate)
		return ;
	// ���°���ֵ����ʱ���£�ÿ��bit����һ������״̬��
	(*keyMsgCB.KeyUpdate)(keyMsgCB.newKeyValue);
	// ��ÿ��������ǰ��״̬���д���������״̬������Եİ�����Ϣ�ڵ���
	for(i = 0; i < KEY_NAME_MAX; i++)
	{			
		// ��õ�ǰ����״̬
		currentState = (keyMsgCB.newKeyValue[i/8] >> ( i % 8 )) & 0x01;	
		// �����һ�ΰ���״̬
		lastState = (keyMsgCB.KeyValue[i/8] >> ( i % 8 )) & 0x01; 
		// ��ǰ����״̬����һ�ΰ���״̬����ͬʱ
		if(currentState != lastState)
		{		
			// ��ȡ��ǰ������Ϣ�ṹ��ָ��
			pKeyInfo = &keyMsgCB.keyMsgInfo[i];
			// ����״̬Ϊ����
			if(KEY_DOWN == currentState)
			{		
				// ��¼�������µĴ���	
				keyCnt = (keyMsgCB.keyCountAndState[i] >> 4) + 1;
				keyMsgCB.keyCountAndState[i] = (keyCnt << 4) | (keyMsgCB.keyCountAndState[i] & 0x0F);
				// �ж��Ƿ���ע�ᰴ����Ϣ����
				if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_DOWN)))
				{
					// �׳�������Ϣ
					KEYMSG_ThrowMsg((KEY_NAME_E)i, KEY_MSG_DOWN);				
				}
				// �����ť�ͷ�״̬
				keyMsgCB.keyCountAndState[i] &= ~(1 << KEY_STATE_UP);			
				// ���ð�ť״̬Ϊ����
				keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_DOWN);				
			}
			
			// ����״̬Ϊ�ͷ�
			else if (KEY_UP == currentState)
			{	
				// �ж��Ƿ���ע��̧����Ϣ����
				if(pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_UP)) 
				{
					// �׳�̧����Ϣ
					KEYMSG_ThrowMsg((KEY_NAME_E)i, KEY_MSG_UP);					
				}
				
				// �����ť����״̬
				keyMsgCB.keyCountAndState[i] &= ~(1 << KEY_STATE_DOWN);				
				// ���ð�ť״̬Ϊ�ͷ�
				keyMsgCB.keyCountAndState[i] |= (1 << KEY_STATE_UP);							
			}		
			// �Ƿ�״̬��������
			else
			{
			
			}
			// ��¼�����ı�ʱʱ��
			pKeyInfo->keyStateChangeTime = keyMsgCB.KeyTimeBase;
		}	
	}
	// ���°���ֵ
	memcpy(keyMsgCB.KeyValue,keyMsgCB.newKeyValue,(KEY_NAME_MAX + 7)/8);
	// ������Ϣ����
	KEYMSG_MsgProcess();
}

// ִ�лص�����
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

// ������Ϣ����
void KEYMSG_MsgProcess(void)
{
	uint8_t keyCnt = 0;				// ��ǰ��������ֵ
	// ��ÿ��������ǰ��״̬���д���
  KEY_NAME_E key = 0;
  
	for( key = (KEY_NAME_E)0; key < KEY_NAME_MAX; key++)
	{	
		// ��ȡ��ǰ������Ϣ�ṹ��ָ��
		KEYMSG_INFO_CB *pKeyInfo = &keyMsgCB.keyMsgInfo[key];	
		// ��ȡ����״̬�����ȥ��ʱ��
		uint32_t beforTime = 0;
    beforTime = ((keyMsgCB.KeyTimeBase + 0xFFFF - pKeyInfo->keyStateChangeTime) % 0xFFFF) * KEY_SCAN_TIME; 
		// Ĭ�Ͽ���
		pKeyInfo->FreeTime = KEYMSG_FREE_TIME_MAX;
    
		// �����а��¼����������Ǵ�1��ʼ
		if(keyMsgCB.keyCountAndState[key] & 0xF0)
		{	
				// ����״̬
				if(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_DOWN))
				{	
						// ע���˳�����Ϣ ������Ϣ��û����
						if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_HOLD)) && !(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_HOLD_DOWN)))
						{
							//  ʱ���㹻
							if(beforTime >= keyHoldThreshHold[key])
							{ 
//                printf("%u   %u   %u ",keyMsgCB.KeyTimeBase,beforTime,keyHoldThreshHold[key]);
								// �׳�����������Ϣ
								KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_HOLD);
								// ���ð�ť״̬Ϊ������
								keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_HOLD_DOWN);
							}
							// ʣ��ʱ��
							else
								pKeyInfo->FreeTime = keyHoldThreshHold[key] - beforTime;
						}
						// ע�����ظ���Ϣ ʱ��Ҳ�����˳���
						if(pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_REPEAT) && beforTime >= keyHoldThreshHold[key])
						{		
							// �ﵽһ���ظ����ھ��׳�һ��
							if(beforTime >= keyHoldThreshHold[key] + keyRepeatTime[key])
							{
								// �׳������ظ���Ϣ
								KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_REPEAT);
								// ����ʱ�� Ϊ��һ���ظ���Ϣ��׼��
								pKeyInfo->keyStateChangeTime = (keyMsgCB.KeyTimeBase  + 0xFFFF - keyHoldThreshHold[key] / KEY_SCAN_TIME) % 0xFFFF;
								// ���ð�ť״̬Ϊ������
								keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_HOLD_DOWN);
							}
							// ʣ��ʱ��
							else
								pKeyInfo->FreeTime = keyHoldThreshHold[key] + keyRepeatTime[key] - beforTime;
						}
				}	
				// ˵�������������Ѿ��ͷ� ���ǻ�ô�д���
				else 
				{
					// �ж��Ƿ񳬹�̧�������
					if(beforTime < keyRaiseThreshHold[key])	
						// ��¼ʣ��ʱ��
						pKeyInfo->FreeTime = keyRaiseThreshHold[key] - beforTime;
					else
					{	
						// ֮ǰ�����˳��������ظ���Ϣ
						if(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_HOLD_DOWN))
						{	
							keyMsgCB.keyCountAndState[key] &= ~(1 << KEY_STATE_HOLD_DOWN);			
						}
						// ���״̬����
						else 
						{	
							// ������������Ǵ�1��ʼ���ʵ�-1��
							keyCnt = (((keyMsgCB.keyCountAndState[key] & 0xF0) >> 4) - 1) + KEY_MSG_ONE_CLICK;
							
							// �жϵ����
							if(keyCnt >= KEY_MSG_MAX)
							{	
								keyCnt = KEY_MSG_NULL;
							}
							
							// ���ݰ�������ֵ�鿴�����İ�����Ϣ����ע��
							if(pKeyInfo->keyMsgSupportType & (1 << keyCnt))
							{						
								// �׳��������Ϣ
								KEYMSG_ThrowMsg((KEY_NAME_E)key, (KEY_MSG_E)keyCnt);
							}
						}
						// ��հ�������
						keyMsgCB.keyCountAndState[key] &= 0x0F;
						// ��ճ�̧
						keyMsgCB.keyCountAndState[key] &= ~(1 << KEY_STATE_LIFT_UP);
					}			
				}		
		}
		// ֮ǰ�İ����¼��Ѿ�����
		if(!(keyMsgCB.keyCountAndState[key] & 0xF0))
		{
			// ע���˳�̧��Ϣ ��̧��Ϣ��û����
			if((pKeyInfo->keyMsgSupportType & (1 << KEY_MSG_LIFT)) && !(keyMsgCB.keyCountAndState[key] & (1 << KEY_STATE_LIFT_UP)))
			{
					//  ʱ��Ҳ��û��
					if(beforTime < keyHoldThreshLift[key])
							// ��¼ʣ��ʱ��
							pKeyInfo->FreeTime = keyHoldThreshLift[key]  - beforTime;
					else
					{
						// �׳�������̧��Ϣ
						KEYMSG_ThrowMsg((KEY_NAME_E)key, KEY_MSG_LIFT);
						// ���ð�ť״̬Ϊ��̧
						keyMsgCB.keyCountAndState[key] |= (1 << KEY_STATE_LIFT_UP);
					}
			}
		}
	}	
}

// ע�ᰴ����Ϣ����
void KEYMSG_RegisterMsgService(KEY_NAME_E keyName, KEY_MSG_E keyMsg, void(*service)(uint32_t param), uint32_t param)
{
	// �����Ϸ��Լ���
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
	// ���ð�������
	keyMsgCB.keyMsgInfo[keyName].service[keyMsg] = service;
	keyMsgCB.keyMsgInfo[keyName].param[keyMsg] = param;
	keyMsgCB.keyMsgInfo[keyName].keyMsgSupportType |= (1 << keyMsg);
}

