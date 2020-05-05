#include"SystemPrint.h"
#include "Interrupter.h"
#include <iostream>
#include <vector>
#include<string>
#include<iostream>
#define NOSPACE(p) (*p&&*p!=' '&&*p!='\n'&&*p!='\t')
#define SPACE(p) (*p=='  '||*p=='\n'||*p=='\t')
#define ERROR_CODE -1
using namespace std;
static char* tptr,*fptr;
static Interrupr_id i_id;
static void* inter_args;
static const int  command_nums = 2;
static const char* commands_table[] = { "create","kill" ,"ls"};
Interrupr_id getIntID(const char * s) {
    if (strcmp(s, "create") == 0)
        return CREATE_PROCESS;
    else if (strcmp(s, "kill") == 0)
        return KILL;
    else if (strcmp(s, "ls") == 0)
        return LIST_PROCESS;
    return (Interrupr_id)ERROR_CODE;
}
int setopt(char* buffer,int buf_size)
{
    vector<char*> args;
    fptr = tptr = buffer;
    while (*fptr)
    {
        while (NOSPACE(tptr)) tptr++;
        if (*tptr)
        {
            *tptr++ = '\0';
            while (SPACE(tptr)) tptr++;
        }
        args.push_back(fptr);
        //std::cout << fptr << std::endl;
        fptr = tptr;
    }

    if (args.size() <= 0) return -2;


    i_id = getIntID(args[0]);
    char* code,*p;
    int file_size;
    FILE* fp;
    switch (i_id)
    {
    case CREATE_FILE:
        if (args.size() != 2)
        {
            return ERROR_CODE;
        }
        inter_args = (void*)atoi(args[1]);
        break;
    case CREATE_PROCESS:
        if (args.size() != 2)
        {
            return -1;
        }
        //inter_args = (void*)atoi(args[1]);
        fp = fopen(args[1], "r");
        if (fp == NULL)
            return ERROR_CODE;
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        // 此处申请的资源在进程被杀死时释放
        code = new char[file_size+1];
        fseek(fp, 0, SEEK_SET);
        for (p = code; !feof(fp); p++) *p = fgetc(fp);
        *p = '\0';
        inter_args = (void*)code;
        //printf("%s\n%d\n", code,file_size);
        break;
    case KILL:
        if (args.size() != 2)
        {
            return ERROR_CODE;
        }
        inter_args = (void*)atoi(args[1]);
        break;
    case LIST_PROCESS:
        break;
    default:
        break;
    }
    return 0;
}
void command(const char* args_buffer)
{
    Interrupt interrupt;
    char input_buffer[1024];
    strcpy_s(input_buffer,1024,args_buffer);

    int ret = setopt(input_buffer, sizeof(input_buffer));
    if (ret==0)
    {
        interrupt.i_id = i_id;
        interrupt.args = inter_args;
        addInterrupt(interrupt);
    }
    else if(ret==ERROR_CODE){
        systemPrintf("Command not found or command failed!\n");
    }
}
