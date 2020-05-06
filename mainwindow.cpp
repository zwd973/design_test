#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"SystemPrint.h"
#include<stdio.h>
#include<mutex>
#include<QTimer>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui_rest = ui;
    timer_output_flush = new QTimer(this);
    connect(timer_output_flush,SIGNAL(timeout()),SLOT(updateText()));
    timer_output_flush->start(50);


    //connect();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_btnOK_clicked()
{

}

void MainWindow::updateText()
{
    int size;
    bufferLock();
    QString output_buffer(getBufferInfo(&size));
    if(output_buffer.size()){
        if(output_buffer.at(output_buffer.size()-1)=='\n')
            output_buffer.remove(output_buffer.size()-1,1);
        ui->textCommand->append(output_buffer);
        output_buffer.clear();
        bufferClear();
    }
    updateSystemInfo();
    bufferUnlock();
    timer_output_flush->start(50);
}

extern const char* getProcessInfo();
void MainWindow::updateSystemInfo()
{
    ui->systemInfo->setText(getProcessInfo());
}

extern void command(const char* args_buffer);
void MainWindow::on_lineEdit_editingFinished()
{
   QString args = ui->lineEdit->text();
   args.remove("root#:");
   command(args.toStdString().c_str());
   ui->lineEdit->setText("root#:");
}

void MainWindow::on_bntCancel_clicked()
{
    ui->textCommand->clear();
}
