#include <stdio.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>
#include "debug.h"
#include "TimerManager.h"
#include "queue.h"
#include "Timer.h"

pthread_mutex_t m_mutex= PTHREAD_MUTEX_INITIALIZER;
static CTimerManager * m_instance=NULL;
unsigned int mark=0;


void Init_CTimerManager()
{
	m_instance->m_state = TIMER_MANAGER_STOP;
	TAILQ_INIT(&(m_instance->list_));//队列初始化
}

CTimerManager * instance()
{
	if(m_instance==NULL)
	{
		pthread_mutex_lock(&m_mutex);
		if(m_instance==NULL)
		{
			m_instance= malloc(sizeof(CTimerManager));
            memset(m_instance,0,sizeof(CTimerManager));
            pthread_mutex_init(&(m_instance->m_mutex),NULL);
		}
		pthread_mutex_unlock(&m_mutex);
	}
	return m_instance;
}

void add_timer_(CTimer * vtimer)
{	
	if(vtimer->m_state==TIMER_ALIVE)
		return;
	CTimer *item;
	struct timeval now,interval;
	
	vtimer->m_state=TIMER_ALIVE;

    #if 0
	//interval.tv_sec=vtimer->m_interval/1000;
	//interval.tv_usec=(vtimer->m_interval%1000)*1000;
	interval.tv_sec=vtimer->m_interval;
	interval.tv_usec=0;
	gettimeofday(&now,0);
	
	timeradd(&now,&interval,&vtimer->m_endtime);
	TAILQ_FOREACH(item, &(m_instance->list_), entry_)
	{
	
		if(timercmp(&item->m_endtime,&vtimer->m_endtime,>=))	
		{
			TAILQ_INSERT_BEFORE(item, vtimer, entry_);
			break;
		}
	}
    #endif
	//if(item==TAILQ_END(&(m_instance->list_)))
	//{
		TAILQ_INSERT_TAIL(&(m_instance->list_), vtimer,entry_);
	//}
}

CTimer* get_timer_( void )
{
	return TAILQ_FIRST( &(m_instance->list_) );
}

void remove_timer_(CTimer * vtimer)
{
	if ( vtimer->m_state!=TIMER_ALIVE )
	{
		DEBUG_INFO("============== remove_timer-- ================\n");
		return;
	}
	TAILQ_REMOVE(&(m_instance->list_),vtimer,entry_);
	vtimer->m_state=TIMER_IDLE;
}

void add_timer(CTimer * vtimer)
{
	pthread_mutex_lock(&m_mutex);
	add_timer_(vtimer);
	vtimer->id_=++mark;
	pthread_mutex_unlock(&m_mutex);
}

void remove_timer(CTimer * vtimer)
{
	pthread_mutex_lock(&m_mutex);
	remove_timer_(vtimer);
	vtimer->m_state=TIMER_IDLE;
	pthread_mutex_unlock(&m_mutex);
}

void  start_CTimerManager(unsigned long interval,unsigned long repair)
{
	instance();
	Init_CTimerManager();
	m_instance->m_interval.tv_sec=interval/1000;//1
	m_instance->m_interval.tv_usec=(interval%1000)*1000;//0
	m_instance->m_repair.tv_sec=repair/1000;//0s
	m_instance->m_repair.tv_usec=(repair%1000)*1000;//10ms

    DEBUG_INFO("%d,%d",m_instance->m_interval.tv_sec,m_instance->m_interval.tv_usec);
    DEBUG_INFO("%d,%d",m_instance->m_repair.tv_sec,m_instance->m_repair.tv_usec);
	if(m_instance->m_state==TIMER_MANAGER_STOP)
	{
		m_instance->m_state=TIMER_MANAGER_START;
		pthread_t pid;
		int ret =pthread_create(&pid,0,process,m_instance);
	}
}

void  stop_CTimerManager()
{
	m_instance->m_state=TIMER_MANAGER_STOP;
}

void dump_CTimerManager()
{
	CTimer *item;
	struct timeval now;
	pthread_mutex_lock(&m_mutex);
	gettimeofday(&now,0);
	struct timeval subTimer;
	unsigned int nTimeToEnd;
	TAILQ_FOREACH(item, &(m_instance->list_), entry_)
	{
		timersub(&item->m_endtime,&now,&subTimer);
		nTimeToEnd=subTimer.tv_sec*1000+subTimer.tv_usec/1000;
		DEBUG_INFO("      %d    %d    %d    %d    %d\n",
			item->id_,item->m_interval,nTimeToEnd,item->m_type,item->m_state);
	}
	pthread_mutex_unlock(&m_mutex);
}

void *process(void * arg)
{
	pthread_detach(pthread_self());

	CTimerManager *manager=(CTimerManager *)arg;

	CTimer *item;
	struct timeval now,stand;
	unsigned int delay;
	struct timeval tm;
	CTimer tmpTimer;
	
	while(manager->m_state==TIMER_MANAGER_START)
	{
		tm.tv_sec=manager->m_interval.tv_sec;
		tm.tv_usec=manager->m_interval.tv_usec;
		while(select(0,0,0,0,&tm)<0&&errno==EINTR);
		gettimeofday(&now,0);
		timeradd(&now,&manager->m_repair,&stand);

		int ret = pthread_mutex_lock(&manager->m_mutex); 
		TAILQ_FOREACH(item, &(manager->list_), entry_)
		{
            //比较时间段,只要满足在这个时间段，都会去执行,不在这个时间段则不执行
			if(timercmp(&item->m_starttime,&stand,<) && timercmp(&item->m_endtime,&stand,>))
			{
				if(item->m_func)
				{
					item->m_func(item,item->m_data);
				}

				if(item->m_type==TIMER_ONCE)
				{
					//DEBUG_INFO("remove timer TIMER_ONCE ++\n");
					remove_timer_(item);
					item->m_state=TIMER_TIMEOUT;
					free(item);  //Add for release memonry
				}
                #if 0
				else if(item->m_type==TIMER_CIRCLE)  //
				{
					if(item->m_state==TIMER_ALIVE) //Timer 没有在执行 m_func 被删除 ，还有效应该重载。
					{
						tmpTimer.entry_=item->entry_;
						remove_timer_(item);
						add_timer_(item);
						item=&tmpTimer;
					}else //在执行 m_func 时候已经 remove_timer ,应该释放timer
					{
						free( item );  //Add for release memonry
					}
				}
                #endif
			}
			else
			{
				break;
			}
		}
		ret = pthread_mutex_unlock(&manager->m_mutex);
	}
}


