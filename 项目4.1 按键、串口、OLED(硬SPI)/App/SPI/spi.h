#ifndef __KEY_H
#define __KEY_H



#include "main.h"



/******************************************************************************
* ���û����Ʋ�������
* ����ϵͳ��Ҫ�޸ģ�����ϸ���ע���������ò���
// ����������,�û�������Ҫ������Ӱ�������
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



// ������Ҫ��ģ��ͷ�ļ�

#define	KEY_SCAN_TIME					50  					// ����ɨ�����ڣ���λ��ms������Ϊ���ⶨʱ���ķֱ��ʵ�N��(N>=1)
#define	KEY_MSG_QUEUE_SIZE		10						// ������Ϣ�������ߴ磬�����ɱ��ֶ��ٸ���Ϣ��ע��ʵ���Ͽ���Ԫ��Ϊ��ֵ-1
#define KEYMSG_FREE_TIME_MAX	(0xFFFFFFFF - 100)

// ��������״̬
typedef enum
{
	KEY_DOWN = 0,												// ��������
	KEY_UP,															// �����ͷ�
}KEY_E;

// ����״̬��Ӧ
typedef enum
{
	KEY_STATE_DOWN = 0,										// ��������
	KEY_STATE_UP,													// ����̧��
	KEY_STATE_LIFT_UP,										// ������̧
	KEY_STATE_HOLD_DOWN,									// ��������
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
	uint16_t 	keyStateChangeTime;												// ��ť�ı�ʱ��ʱ��
	uint32_t 	FreeTime;																	// ����ʱ��
	uint16_t	keyMsgSupportType;												// �ð�����֧�ֵ���Ϣ���ͣ�bit��ӦKEY_MSG_E��֧��Ϊ1��֧��Ϊ0
	void	(*service[KEY_MSG_MAX])(uint32_t param);			// ������Ӧ
	uint32_t	param[KEY_MSG_MAX];												// ������Ӧ����Ĳ���
}KEYMSG_INFO_CB;	
// �������ƽṹ��

typedef struct
{
	uint32_t		  KeyTimeBase;								      		// ����ʱ��׼
	uint8_t				newKeyValue[(KEY_NAME_MAX + 7)/8];		// ���°���ֵ����ʱ���£�ÿ��bit����һ������״̬��
	uint8_t				KeyValue[(KEY_NAME_MAX + 7)/8];				// ����İ���ֵ״̬ ÿ�ζ�ʱ���µĽ�������ֵ���Ƚϣ�����������а��������仯						
	uint8_t				keyCountAndState[KEY_NAME_MAX];				// ���������Լ�������ǰ״̬ 
	KEYMSG_INFO_CB		keyMsgInfo[KEY_NAME_MAX];					// ������Ϣ�ڵ�
	void(*KeyUpdate)	(uint8_t *p8bitKeyValueArray);		// ����ˢ�º�������Ҫʹ�����ṩ�ص������ӿڣ��ڻص�������Ϊ������ֵ
}KEYMSG_CB;


// ģ���ʼ���ӿ�
void KEYMSG_Init(void);
// ��������ص��������ڶ�ʱ�������£������ڵ��ã�ʵ�ְ���ɨ��
void KEYMSG_CALLBACK_Scan(uint32_t param);
// ����ɨ�账��
void KEYMSG_ScanProcess(void);
// ִ�лص�����
uint8_t KEYMSG_ThrowMsg(KEY_NAME_E keyName, KEY_MSG_E keyMsg);
// ������Ϣ����
void KEYMSG_MsgProcess(void);
// ע�ᰴ����Ϣ����
void KEYMSG_RegisterMsgService(KEY_NAME_E keyName, KEY_MSG_E keyMsg, void(*service)(uint32_t param), uint32_t param);





#endif



