#include "Timer.h"
#include "TimerManager.h"
#include "debug.h"
#include "queue.h"

void Init_Timer(CTimer * Timer,unsigned int vinterval,void (*vfunc)(CTimer *,void *),void *vdata,TimerType vtype)
{
	Timer->m_interval	=	(vinterval);
	Timer->m_state		=	(TIMER_IDLE);
	Timer->m_type		=	(vtype);
	Timer->m_func		=	(vfunc);
	Timer->m_data		=	(vdata);
}

void start_Timer(CTimer * Timer)
{
	//DEBUG_INFO("enter start_Timer Timer->:%p\n" , Timer);
	add_timer(Timer);
}

void stop_Timer(CTimer * Timer)
{
	remove_timer(Timer);
}

void reset_Timer(CTimer * Timer,unsigned int vinterval)
{
	remove_timer(Timer);
	Timer->m_interval=vinterval;
	add_timer(Timer);
}

void Release_Timer(CTimer * Timer)
{
	if(Timer->m_state==TIMER_ALIVE)
		stop_CTimerManager();
}

