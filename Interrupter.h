#pragma once
typedef enum {CREATE_PROCESS,KILL,CREATE_FILE,ECHO,LIST_PROCESS} Interrupr_id;
typedef struct
{
    Interrupr_id i_id;
    void* args;
}Interrupt;

void handInterrupt();
int trap(Interrupt& interrupt);
void addInterrupt(Interrupt& interrupt);
