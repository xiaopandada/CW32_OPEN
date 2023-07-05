#ifndef __TASKER_H
#define __TASKER_H



#include "main.h"


// 软件定时器周期，即软件定时器最小时间分辨率，单位：ms
#define TIMER_TIME										1
// 无限循环
#define TIMER_LOOP_FOREVER						-1
// 定时器后期处理任务队列
#define TIMER_TASK_QUEUE_SIZE					30
#define TIMER_NULL										0

#define SCLIE_NULL										0
#define LED_DELAY_TIME                100
#define KEY_DELAY_TIME                100

// 时间片结构体
typedef struct 
{
	void(*action)(uint32_t param);
	uint32_t	param;
	uint32_t Slice;        //时间间隔
	uint32_t TimeCnt;      //剩余时间
	uint8_t active;        //运行状态
}TIME_SLICE_TASK;

// 时间片枚举
typedef enum
{
	SLICE_ID_NULL = -1,
	SLICE_ID_KEY_SCAN,									// 按键扫描
	SLICE_ID_LED,								  // 开关设备
  SLICE_ID_KEY,									// 按键扫描
	SLICE_ID_Motor,								//舵机
	SLICE_ID_MAX
}SLICE_ID;


// 操作定时器枚举
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


// 时间片初始化
void TimerSlice_Init(void);
// 向时间片添加任务
uint8_t TimerSlice_SetTask(SLICE_ID id, uint32_t time, void(*action)(uint32_t param), uint32_t param);
//时间片处理
void TimerSlice_Process(void);
// 操作时间片
uint32_t TIMER_SliceControl(SLICE_ID id, TIMER_CMD_E cmd, uint32_t param);





#endif



