#include "Tasker.h"


/*-------------------------------------ʱ��Ƭ��ʱ�� ���ڹ�������ѭ��������------------------------------------*/

//ʱ��Ƭ����ṹ��
TIME_SLICE_TASK SliceTask[SLICE_ID_MAX];
TIME_SLICE_TASK *pTask = 0;


// ʱ��Ƭ��ʼ��
void TimerSlice_Init(void)
{
		memset(&SliceTask,0,sizeof(TIME_SLICE_TASK) * SLICE_ID_MAX);
}

// ��ʱ��Ƭ�������
uint8_t TimerSlice_SetTask(SLICE_ID id, uint32_t time, void(*action)(uint32_t param), uint32_t param)
{
    
		if((SLICE_ID_NULL == id)||(id >= SLICE_ID_MAX))
		{
			 return SCLIE_NULL;
		}
		pTask = &SliceTask[id];
		pTask->action = action;
		pTask->param = param;
		pTask->Slice = time;
		pTask->TimeCnt = 0;
		pTask->active = 1;
		return 1;
}

// ʱ��Ƭ������ѵ
void TimerSlice_Process(void)
{
    int i = 0;
		for(i = 0; i < SLICE_ID_MAX; i++)
		{
				TIME_SLICE_TASK *pTask = &SliceTask[i];
				if(pTask->TimeCnt >= pTask->Slice && pTask->action && pTask->active)
				{
						(*pTask->action)(pTask->param);
						pTask->TimeCnt = 0;
				}
		}
}


// ����ʱ��Ƭ
uint32_t TIMER_SliceControl(SLICE_ID id, TIMER_CMD_E cmd, uint32_t param)
{
	if((SLICE_ID_NULL == id)||(id >= SLICE_ID_MAX))
	{
		 return SCLIE_NULL;
	}
	pTask = &SliceTask[id];
	switch(cmd)
	{
		// ��ͣ
		case TIMER_CMD_PAUSE:
				pTask->active = 0;
		break;
		// �ָ�
		case TIMER_CMD_RESUME:
				pTask->active = 1;
		break;
		// ɾ������
		case TIMER_CMD_KILL_TASK:
				pTask->Slice = 0;
				pTask->action = 0;
				pTask->param = 0;
		break;
		// ����ʱ��
		case TIMER_CMD_CHANGE_TAIME:
				pTask->Slice = param;
		break;
		// ��λ����ֵ
		case TIMER_CMD_RESET:
				pTask->active = 1;
				pTask->TimeCnt = 0;
		break;
		// �Ƿ񼤻�
		case TIMER_CMD_GET_ACTIVE:
				
		return pTask->active;

		// ��ȡʣ��ʱ��
		case TIMER_CMD_GET_SUR_TIME:
				if(pTask->active)
				{
						if(pTask->TimeCnt > pTask->Slice)
							return 0;
						else
							return pTask->Slice - pTask->TimeCnt;
				}
				
		default: break;
	}
	return 0XFFFFFFFF;
}



