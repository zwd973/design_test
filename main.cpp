#include "mainwindow.h"
#include "SystemPrint.h"
#include<stdio.h>
#include <QApplication>
#include <iostream>       // std::cout
#include <thread>         // std::thread
using namespace std;
extern void VM();
extern void command();
extern bool not_exit;
int main(int argc, char *argv[])
{
    std::thread virtual_machine_th(VM);
#ifndef QT_UI
    std::thread command_thd(command);
    std::thread flush_thd(flushPerTenMs);
#endif // !QT_UI
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int ret = a.exec();
    not_exit=false;
    virtual_machine_th.join();
#ifndef QT_UI
    command_thd.join();
    flush_thd.join();
#endif // !QT_UI
    return ret;
}
