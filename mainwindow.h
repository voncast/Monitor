#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <qcustomplot.h>
#include <QList>
#include <QStringList>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString onTime;
    int cpuDataStart[4];
    int cpuDataEnd[4];
    double Buff[3][240];
    double CurrentData[3];
    int ProgressNum;
    int RunPro;
    QString EndPid;
    QList<QStringList> ql;
    void Paint(QCustomPlot*customPlot,int which);
    void StartPer();
private slots:
    void RunningTime();
    void StartTab1();
    void StartTab0();
    void StartTab(int);
    void StartProCount();
    void UpdateCpu();
    void UpdateMemory();
    void UpdateSwap();
    void StartPer1();
    void StartPer2();
    void ShowCurTime();
    void ShowProgress2();
    void SearchProgress();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    unsigned long int flag;
};

#endif // MAINWINDOW_H
