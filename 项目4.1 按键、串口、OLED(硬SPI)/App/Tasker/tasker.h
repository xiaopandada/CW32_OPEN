#ifndef __TASKER_H
#define __TASKER_H



#include "main.h"


// �����ʱ�����ڣ��������ʱ����Сʱ��ֱ��ʣ���λ��ms
#define TIMER_TIME										1
// ����ѭ��
#define TIMER_LOOP_FOREVER						-1
// ��ʱ�����ڴ����������
#define TIMER_TASK_QUEUE_SIZE					30
#define TIMER_NULL										0

#define SCLIE_NULL										0
#define LED_DELAY_TIME                100
#define KEY_DELAY_TIME                100

// ʱ��Ƭ�ṹ��
typedef struct 
{
	void(*action)(uint32_t param);
	uint32_t	param;
	uint32_t Slice;        //ʱ����
	uint32_t TimeCnt;      //ʣ��ʱ��
	uint8_t active;        //����״̬
}TIME_SLICE_TASK;

// ʱ��Ƭö��
typedef enum
{
	SLICE_ID_NULL = -1,
	SLICE_ID_KEY_SCAN,									// ����ɨ��
	SLICE_ID_LED,								  // �����豸
  SLICE_ID_KEY,									// ����ɨ��
	SLICE_ID_Motor,								//���
	SLICE_ID_MAX
}SLICE_ID;


// ������ʱ��ö��
typedef enum
{
	TIMER_CMD_NULL = 0,
	TIMER_CMD_PAUSE,
	TIMER_CMD_RESUME,
	TIMER_CMD_KILL_TASK,
	TIMER_CMD_CHANGE_TAIME,
	TIMER_CMD_RESET,
	TIMER_CMD_GET_ACTIVE,
	TIMER_CMD_GET_SUR_TIME,
}TIMER_CMD_E;


// ʱ��Ƭ��ʼ��
void TimerSlice_Init(void);
// ��ʱ��Ƭ�������
uint8_t TimerSlice_SetTask(SLICE_ID id, uint32_t time, void(*action)(uint32_t param), uint32_t param);
//ʱ��Ƭ����
void TimerSlice_Process(void);
// ����ʱ��Ƭ
uint32_t TIMER_SliceControl(SLICE_ID id, TIMER_CMD_E cmd, uint32_t param);





#endif



