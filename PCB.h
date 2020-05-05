#pragma once
#ifndef PCB_H
#define PCB_H
#include<string>
typedef struct
{
    int pid;
    void* memory;
    const char* code;
    int pc;
    int total_run_time;
    int time_cost;
    // 8个运算寄存器
    long reg[8];
}PCB;

int createProcess(const char* code);
PCB* getPCB(int pid);
PCB*  nextProcess();
void pushBackPCB(PCB* pcb);
int killProcess(PCB* pcb);
const char* getProcessInfo();
#endif // PCB_H
