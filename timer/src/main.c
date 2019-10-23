#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "TimerManager.h"

static int a = 0;

void runTimeBucketTask(CTimer *timer,void *data)
{
   int *p = data;
   *p +=1;
   DEBUG_INFO("%d",*(int*)data);
}

void caculate_time(void)
{
    struct timeval now,interval;
    interval.tv_sec=0;
	interval.tv_usec=0;
    gettimeofday(&now,0);
    CTimer * timer1= malloc(sizeof(CTimer));
    timeradd(&now,&interval,&timer1->m_starttime);
        
    interval.tv_sec=6;//结束时间比开始时间多3min，满足这个时间段就执行
    timeradd(&now,&interval,&timer1->m_endtime);
    Init_Timer(timer1,0,runTimeBucketTask,(void *)&a,TIMER_CIRCLE);
    start_Timer(timer1);
}

int main(void)
{
    start_CTimerManager(100,10);
    sleep(3);
    caculate_time();
    while(1)
    {
        sleep(1);
    }
    return 0;
}

