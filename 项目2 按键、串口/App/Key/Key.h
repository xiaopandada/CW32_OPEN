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



// ������Ҫ��ģ��ͷ�ļ�

#define	KEY_SCAN_TIME					50  					// ������Ϣ�������ߴ磬�����ɱ��ֶ��ٸ���Ϣ��ע��ʵ���Ͽ���Ԫ��Ϊ��ֵ-1
#define KEYMSG_FREE_TIME_MAX	(0xFFFFFFFF - 100)

// ��������״̬
typedef enum
{
	KEY_DOWN = 0,											
	KEY_UP,														
}KEY_E;

// ����״̬��Ӧ
typedef enum
{
	KEY_STATE_DOWN = 0,										
	KEY_STATE_UP,												
	KEY_STATE_LIFT_UP,									
	KEY_STATE_HOLD_DOWN,								
}KEY_STATE_E;

// ������Ϣ���壬�˲��̶ֹ� ,�û������޸�
typedef enum
{
	KEY_MSG_NULL = 0,										// �Ƿ�������Ϣ
	KEY_MSG_DOWN,												// ����������Ϣ
	KEY_MSG_UP,													// ����������Ϣ
	KEY_MSG_HOLD,												// ����������Ϣ
	KEY_MSG_LIFT,												// ������̧��Ϣ
	KEY_MSG_REPEAT,											// �����ظ���Ϣ
	KEY_MSG_ONE_CLICK,											// ������Ϣ
	KEY_MSG_MAX,												// ������Ϣ�����
}KEY_MSG_E;	

// ������Ϣ�ڵ�	
typedef struct	
{	
	uint16_t 	keyStateChangeTime;									
	uint32_t 	FreeTime;																
	uint16_t	keyMsgSupportType;												
	void	(*service[KEY_MSG_MAX])(uint32_t param);			
	uint32_t	param[KEY_MSG_MAX];										
}KEYMSG_INFO_CB;	
// �������ƽṹ��

typedef struct
{
	uint32_t		  KeyTimeBase;								      		
	uint8_t				newKeyValue[(KEY_NAME_MAX + 7)/8];		
	uint8_t				KeyValue[(KEY_NAME_MAX + 7)/8];					
	uint8_t				keyCountAndState[KEY_NAME_MAX];			
	KEYMSG_INFO_CB		keyMsgInfo[KEY_NAME_MAX];				
	void(*KeyUpdate)	(uint8_t *p8bitKeyValueArray);		
}KEYMSG_CB;


// ģ���ʼ���ӿ�
void KEYMSG_Init(void);

void KEYMSG_CALLBACK_Scan(uint32_t param);

void KEYMSG_ScanProcess(void);

uint8_t KEYMSG_ThrowMsg(KEY_NAME_E keyName, KEY_MSG_E keyMsg);

void KEYMSG_MsgProcess(void);
// ע�ᰴ����Ϣ����
void KEYMSG_RegisterMsgService(KEY_NAME_E keyName, KEY_MSG_E keyMsg, void(*service)(uint32_t param), uint32_t param);





#endif



