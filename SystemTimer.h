#ifndef SYSTEMTIMER_H
#define SYSTEMTIMER_H
typedef void (*TimerTask)(void*);
typedef struct STimer{
    int remaind_time;
    TimerTask toExcute;
    void* args;
} STimer;
void updateTimer(int interval);
void stopTimer(long tid);
long startTimer(int delta_time,TimerTask toExecute,void* args);
void destoryTimer(STimer * timer);
void excuteTest(void* args);
#endif // SYSTEMTIMER_H
