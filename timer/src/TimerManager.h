#ifndef _TIMERMANAGER_H_
#define _TIMERMANAGER_H_

#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "Timer.h"
#include "debug.h"

#define DEFULT_INTERVAL 1000   
#define DEFAUL_REPAIR 20

typedef enum{
		TIMER_MANAGER_STOP=0,
		TIMER_MANAGER_START
}TimerManagerState;

typedef struct CTimerManager{
	pthread_mutex_t m_mutex;
	struct CTimerManager * m_instance;
	TimerManagerState m_state;
	struct timeval m_interval;
	struct timeval m_repair;
	TAILQ_HEAD(,CTimer) list_;
	unsigned int mark;
}CTimerManager;

CTimerManager * instance();
void add_timer(CTimer * vtimer);
void remove_timer(CTimer * vtimer);
void  start_CTimerManager(unsigned long interval,unsigned long repair);
void stop_CTimerManager();
void dump_CTimerManager();
CTimer* get_timer_( void );
void add_timer_(CTimer * vtimer);
void remove_timer_(CTimer * vtimer);
void Init_CTimerManager();
void *process(void * arg);


#endif
