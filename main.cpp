#include "mainwindow.h"
#include "SystemPrint.h"
#include<stdio.h>
#include <QApplication>
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include<QDebug>
using namespace std;
extern void VM();
extern void command();
extern bool not_exit;
int main(int argc, char *argv[])
{
    std::thread virtual_machine_th(VM);
    //virtual_machine_th.detach();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int ret = a.exec();
    qDebug()<<"\n";
    not_exit=false;
    virtual_machine_th.join();
    return ret;

}
