#include "PCB.h"
#include<list>
#include<string>
#include<vector>
#include<algorithm>
static std::list<PCB*> runing_processes;
static int pcb_count=0;

// 根据进程指令新建进程，传入参数为进程指令
int createProcess(const char * code)
{
    PCB* pcb = new PCB;
    if (pcb == NULL)
        return -1;
    pcb->code = code;
    pcb->pc = 0;
    // 此处调用内存管理API申请资源，申请失败则创建进程失败，失败时务必释放资源
    pcb->memory = NULL;
    pcb->pid = pcb_count;
    pcb->time_cost = 0;
    pcb->total_run_time = 0;
    runing_processes.push_back(pcb);
    pcb_count++;
    return 0;
}

// 销毁PCB
void destroyPCB(PCB* pcb)
{
    // 此处增加释放其它资源的操作，比如内存
    delete pcb->code;
    delete pcb;
}

// 从进程id返回PCB
PCB* getPCB(int pid)
{
    // 没有检查设备管理队列，进程可能处于等待状态，应增加对应操作
    for (std::list<PCB*>::iterator itr = runing_processes.begin(); itr != runing_processes.end(); itr++)
    {
        if ((*itr)->pid == pid)
        {
            return *itr;
        }
    }
    return NULL;
}

// 杀死pcb指向进程
int killProcess(PCB* pcb)
{
        // 没有检查设备管理队列，进程可能处于等待状态，应增加
    if (pcb == NULL) return 0;
        runing_processes.remove(pcb);
        destroyPCB(pcb);
        pcb_count--;
        return 0;
}

// 将进程从运行队列移除
void removePCB(PCB* pcb)
{
    runing_processes.remove(pcb);
}

// 将进程插入运行队列尾部
void pushBackPCB(PCB* pcb)
{
    runing_processes.push_back(pcb);
}

// 根据调度算法，取出下一个待运行进程
PCB* nextProcess()
{
    if (runing_processes.size() > 0)
    {
        // 简单时间片轮转算法
        PCB* pcb = runing_processes.front();
        runing_processes.pop_front();
        return pcb;
    }
    else
    {
        return NULL;
    }
}

const char* getProcessInfo() {
    static std::string  process_ui_info;
    std::vector<std::pair<int,std::string>> processes_info;
    char buff[1024];
    process_ui_info.clear();
    sprintf(buff, "\nPID\tMemory\tS\tRuningTime(ms)\n");
    process_ui_info += buff;
    for (PCB* p:runing_processes) {
        sprintf(buff, "%d\t%-4d\tr\t%d\n", p->pid, 0, p->total_run_time);
        std::pair<int,std::string> item(p->pid,buff);
        processes_info.push_back(item);
    }
    std::sort(processes_info.begin(),processes_info.end());
    for(auto item:processes_info)
    {
        process_ui_info+=item.second;
    }
    return process_ui_info.c_str();
}

//const char * getProcessInfoWithMutex() {
//    global_mutex.lock();
//    const char * res = getProcessInfo();
//    global_mutex.unlock();
//    return res;
//}


