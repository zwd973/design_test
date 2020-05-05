#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QTimer;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btnOK_clicked();

    void on_lineEdit_editingFinished();

    void updateText();

    void on_bntCancel_clicked();

private:
    Ui::MainWindow *ui;
    QTimer* timer_output_flush;
    void updateSystemInfo();
};
#endif // MAINWINDOW_H
