#ifndef _CTIMER_H_
#define _CTIMER_H_
#include <sys/time.h>
#include "queue.h"

typedef enum{
		TIMER_IDLE=0,
		TIMER_ALIVE,
		TIMER_TIMEOUT
}TimerState;//定时器状态

typedef enum{
		TIMER_ONCE=0,
		TIMER_CIRCLE
}TimerType;//定时器类型

typedef struct CTimer{
	unsigned int id_;
    unsigned long m_interval;//定时时间
    unsigned int m_counter;//count
	struct timeval m_endtime;
    struct timeval m_starttime;
	TimerState m_state;//定时器状态
	TimerType m_type;//定时器类型
	void (*m_func)(struct CTimer *,void *);//回调函数
	void * m_data;//定时器数据
	TAILQ_ENTRY(CTimer) entry_;
}CTimer;//定时器


void Init_Timer(CTimer * Timer,unsigned int vinterval,void (*vfunc)(CTimer *,void *),void *vdata,TimerType vtype);
void start_Timer(CTimer * Timer);
void stop_Timer(CTimer * Timer);
void reset_Timer(CTimer * Timer,unsigned int vinterval);
void Release_Timer(CTimer * Timer);
#endif

