#include "SystemTimer.h"
#include <unordered_set>
//#include <QDebug>
static std::unordered_set<STimer*> timers;
long startTimer(int delta_time,TimerTask toExecute,void* args)
{
    STimer* timer = new STimer{delta_time,toExecute,args};
    timers.insert(timer);
    return (long)timer;
}

void destoryTimer(STimer * timer)
{
    delete timer;
}

void stopTimer(long tid)
{
    STimer* timer = (STimer*)tid;
    if(timers.count(timer)){
        timers.erase(timer);
       destoryTimer(timer);
    }
}


void updateTimer(int interval)
{
    for(auto itr= timers.begin();itr!=timers.end();){
        STimer* timer = *itr++;
        timer->remaind_time-=interval;
        if(timer->remaind_time<=0){
            timers.erase(timer);
            timer->toExcute(timer->args);
            destoryTimer(timer);
        }
    }
}


extern int systemPrintf(const char* format,...);
void excuteTest(void* args)
{
    systemPrintf("Timer Test message \"%s\"\n",(const char*)args);
    startTimer(5000,&excuteTest,args);
}


