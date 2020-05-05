#include "SystemPrint.h"
#include <stdarg.h>
#ifndef QT_UI
#include <stdio.h>
#include<string>
#include <windows.h>
#endif
#include <mutex>
// 与UI线程的互斥锁，每个10ms周期系统内部状态刷新完毕前，不容许UI读取系统信息
std::mutex global_mutex;
std::string output_buffer;

void bufferLock()
{
    global_mutex.lock();
}
void bufferUnlock()
{
    global_mutex.unlock();
}

static char buffer[1024];
int systemPrintf(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = vsprintf(buffer, format, args);
    va_end(args, format);
    output_buffer += buffer;
    return ret;
}

const char* getBufferInfo(int * size)
{
    *size = output_buffer.size();
    return output_buffer.c_str();
}

void bufferClear()
{
    output_buffer.clear();
}

#ifndef QT_UI
void flushPerFiftyMs()
{
    int size;
    const char* cptr;
    while (true)
    {
        bufferLock();
        cptr = getBufferInfo(&size);
        if (size)
        {
            printf("%s", cptr);
            bufferClear();
        }
        bufferUnlock();
        Sleep(50);
    }
}
#endif
