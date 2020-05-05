#include "Interrupter.h"
#include "PCB.h"
#include "SystemPrint.h"
#include <queue>
#include <mutex>
#include <assert.h>
static std::mutex mtx;
static std::queue<Interrupt> sync_interrupts;
typedef int (*interrupt_function) (void *);
//Create Process
static int create_process(void* args)
{
    createProcess((const char*)args);
    systemPrintf("Create Process!\n");
    //std::cout << (char*)args <<std:: endl;
    return 0;
}

//Kill Process
extern void swapProcess();
static int kill(void* args)
{
    PCB* pcb = getPCB((int)args);
    if (pcb) {
        systemPrintf("Kill Process %d",pcb->pid);
        killProcess(pcb);
        swapProcess();
    }
    return 0;
}

static void ls() {
    const char * p_info = getProcessInfo();
    systemPrintf(p_info);
}

//中断表
// static interrupt_function interrupt_table[] = {create_process,kill};

static bool empty()
{
    mtx.lock();
    bool res = sync_interrupts.empty();
    mtx.unlock();
    return res;
}

void addInterrupt(Interrupt& interrupt)
{
    mtx.lock();
    sync_interrupts.push(interrupt);
    mtx.unlock();
}

static Interrupt getInterrupt()
{
    Interrupt interrupt;
    mtx.lock();
    interrupt = sync_interrupts.front();
    sync_interrupts.pop();
    mtx.unlock();
    return interrupt;
}

void handInterrupt()
{
    Interrupt interrupt;
    while (!empty())
    {
        interrupt = getInterrupt();
        trap(interrupt);
    }
}


// 陷阱
int trap(Interrupt& interrupt)
{
#ifdef DEBUG
    assert(interrupt.i_id < (int) sizeof(interrupt_table) / sizeof(interrupt_function));
    assert(interrupt.i_id > 0);
#endif
    // interrupt_table[interrupt.i_id](interrupt.args);
    //也可以采用switch方式实现
    switch (interrupt.i_id)
    {
    case CREATE_PROCESS:
        create_process(interrupt.args);
        break;
    case KILL:
        kill(interrupt.args);
        break;
    case LIST_PROCESS:
        ls();
        break;
    default:
        break;
    }
    return 0;
}
