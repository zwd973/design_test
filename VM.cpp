#include "Interrupter.h"
#include "PCB.h"
#include "SystemPrint.h"
#include <iostream>
#include <windows.h>
using namespace std;

static PCB* cur_pcb=NULL;
static int time_slot = 100;
bool not_exit=true;

void setProcess(PCB* next_pcb)
{
    cur_pcb = next_pcb;
}

PCB* getProcess()
{
    return cur_pcb;
}

void swapProcess()
{
    // 取出队首进程PCB,空则返回NULL
    PCB* pcb = nextProcess();
    // 重置时间片
    time_slot = 100;
    // 设置VM当前进程
    setProcess(pcb);
    // 将进程置于队列尾部(即等待下轮循环)
    if (pcb)
        pushBackPCB(pcb);
}

static char reg_info[1024];
static const char* get_reg_info() {
    char* p = reg_info;
    for (int i = 0; i < 8; i++) {
        int l = sprintf(p, "r%d:%ld\n", i,cur_pcb->reg[i]);
        p += l;
    }
    return reg_info;
}

// 执行进程的下一条指令，修改进程状态
#define SPACE(x) (x==' '||x=='\t'||x=='\n')
enum INSTRUCTION_TYPE { OPERATION='C',IO='B',HALT='H',GOTO='G',PRINT='P' ,LOAD='L',
                                            ADD='+',SUB='-',MUL='*',DIV='/',JG='>',JL='<',JE='=' ,PRINT_REG='R'} ;
#define MOVE_PC() while (*pc_ptr&&*pc_ptr != '\n') pc_ptr++;\
                                if (!*pc_ptr) goto code_error;\
                                cur_pcb->pc = pc_ptr - cur_pcb->code
#define GOTO_LINE(_line_) pc_ptr = cur_pcb->code;\
                                            for (int i = 0; i < (_line_) - 1; i++) {\
                                                while (*pc_ptr&&*pc_ptr != '\n') pc_ptr++;\
                                            if (!*pc_ptr) goto code_error;\
                                            }\
                                        cur_pcb->pc = pc_ptr - cur_pcb->code
static void execute()
{
    /*
        进程有五种特殊指令，C,B,H,G，P分别对应计算，设备占
        用,停机,无条件转移和打印,例：
        C 1000
        B 1 1000
        C 2000
        G 1
        H
        进程先运算1000ms,然后占用设备号为1的设备1000ms,接
        着再运算2000ms,最终停机，每个周期10ms,因此一条P指
        令可能执行多个周期，当前P指令已经花费的时间，记录在
        PCB的time_cost字段,G 1指令转移到第1行。

        四种运算指令+，-，*，/：
        +,-,*,/ src1 src2 dest

        三种条件转移指令，<,>,=
        < r l line_id
        如果寄存器r<l则转移到line_id

        最后一种加载立即数指令L
        L dest number
    */
    cur_pcb->total_run_time += 10;
    const char* pc_ptr = cur_pcb->code + cur_pcb->pc;
    // 跳过空字段
    while (*pc_ptr&&SPACE(*pc_ptr)) pc_ptr++;
    if (!*pc_ptr) goto code_error;
    int time_cost, device_id,line_id,src1,src2,dest;
    long number;
    char op, buff[1024];
    switch (*pc_ptr)
    {
    case OPERATION:
        sscanf(pc_ptr, "%c %d", &op, &time_cost);
        cur_pcb->time_cost += 10;
        if (cur_pcb->time_cost < time_cost)
            return;
        cur_pcb->time_cost = 0;
        // 移动pc
        MOVE_PC();
        break;
    case IO:
        sscanf(pc_ptr, "%c %d %d", &op, &device_id,&time_cost);
        // 移动pc
        MOVE_PC();
        // 将进程移入等待队列
        // 交互进程
        swapProcess();
        break;
    case GOTO:
        sscanf(pc_ptr, "%c %d", &op, &line_id);
        // 行号转移
        GOTO_LINE(line_id);
        break;
    case PRINT:
        pc_ptr++;
        while (*pc_ptr&&SPACE(*pc_ptr)) pc_ptr++;
        if (!*pc_ptr) goto code_error;
        for (int k = 0; pc_ptr[k] != '\n'&&pc_ptr[k]; k++) {
            buff[k] = pc_ptr[k];
            buff[k + 1] = '\0';
        }
        systemPrintf("PrintMessage \"%s\" from process with pid %d\n",buff,cur_pcb->pid);
        // 移动pc
        MOVE_PC();
        break;
    case HALT:
        systemPrintf("Process %d exit\n", cur_pcb->pid);
        killProcess(cur_pcb);
        swapProcess();
        return;
        break;
    case ADD:
        sscanf(pc_ptr, "%c %d %d %d", &op, &src1,&src2,&dest);
        cur_pcb->reg[dest] = cur_pcb->reg[src1] + cur_pcb->reg[src2];
        MOVE_PC();
        break;
    case SUB:
        sscanf(pc_ptr, "%c %d %d %d", &op, &src1, &src2, &dest);
        cur_pcb->reg[dest] = cur_pcb->reg[src1] - cur_pcb->reg[src2];
        MOVE_PC();
        break;
    case MUL:
        sscanf(pc_ptr, "%c %d %d %d", &op, &src1, &src2, &dest);
        cur_pcb->reg[dest] = cur_pcb->reg[src1] * cur_pcb->reg[src2];
        MOVE_PC();
        break;
    case DIV:
        sscanf(pc_ptr, "%c %d %d %d", &op, &src1, &src2, &dest);
        cur_pcb->reg[dest] = cur_pcb->reg[src1] / cur_pcb->reg[src2];
        MOVE_PC();
        break;
    case JG:
        sscanf(pc_ptr, "%c %d %d %d", &op, &src1, &src2, &line_id);
        if (cur_pcb->reg[src1] > cur_pcb->reg[src2]){
            GOTO_LINE(line_id);
        }else {
            MOVE_PC();
        }
        break;
    case JL:
        sscanf(pc_ptr, "%c %d %d %d", &op, &src1, &src2, &line_id);
        if (cur_pcb->reg[src1] < cur_pcb->reg[src2]) {
            GOTO_LINE(line_id);
        }
        else {
            MOVE_PC();
        }
        break;
    case JE:
        sscanf(pc_ptr, "%c %d %d %d", &op, &src1, &src2, &line_id);
        if (cur_pcb->reg[src1] == cur_pcb->reg[src2]) {
            GOTO_LINE(line_id);
        }
        else {
            MOVE_PC();
        }
        break;
    case LOAD:
        sscanf(pc_ptr, "%c %d %ld", &op, &src1, &number);
        cur_pcb->reg[src1] = number;
        MOVE_PC();
        break;
    case PRINT_REG:
        systemPrintf("Process %d register value as follows:\n", cur_pcb->pid);
        systemPrintf(get_reg_info());
        MOVE_PC();
        break;
    default:
        // 出错，未定义指令
        goto code_error;
        break;
    }
    return;
    // 指令格式出错
code_error:
    systemPrintf("Process %d  crashed for unparing instruction\n",cur_pcb->pid);
    killProcess(cur_pcb);
    swapProcess();
    return;
}

void VM()
{
    while (not_exit)
    {
        bufferLock();
        time_slot -= 10;
        if (cur_pcb)
        {
            execute();
        }
        // 时间片到期则切换
        if (time_slot <= 0||cur_pcb==NULL)
        {
            swapProcess();
        }
        // 中断处理
        handInterrupt();
        bufferUnlock();
        Sleep(10);
    }
}
