#pragma once
#ifndef PCB_H
#define PCB_H
#define ERROR_CODE -1
#include<string>
typedef struct PCB
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

int useDevice(PCB* pcb,const char* device_name, int time_cost);

int createProcess(const char* code);

PCB* getPCB(int pid);

PCB*  nextProcess();

void pushBackPCB(PCB* pcb);

int killProcess(PCB* pcb);

const char* getProcessInfo();

// 使用设备
int useDevice(PCB* pcb,const char* device_name, int time_cost);
#endif // PCB_H
