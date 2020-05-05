#pragma once
#ifndef SYSTEMPRINT_H
#define SYSTEMPRINT_H
#define QT_UI
// printf的替代
int systemPrintf(const char* format,...);
const char* getBufferInfo(int * size);
void bufferLock();
void bufferUnlock();
void bufferClear();
#ifdef QT_UI

#else
void flushPerTenMs();
#endif


#endif // SYSTEMPRINT_H
