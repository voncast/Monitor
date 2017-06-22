#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidget>
#include <QLabel>
#include <QListWidgetItem>
#include <QWidget>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QVector>
#include <QTableWidgetItem>
#include <QDir>
QVector <double> Xvalue(240);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ProgressNum=0;
    EndPid="-1";
    ui->warn->setVisible(false);
    QTimer *Timer = new QTimer(this);
    connect(Timer,SIGNAL(timeout()),this,SLOT(ShowCurTime())); 
    Timer->start(1000);
    QPixmap pix=QPixmap("/home/aliceyang/Documents/OS/osMonitor/images/sys3.jpg");
    ui->curCpuIm->setPixmap(pix);
    ui->curCpuIm->show();
    QPixmap pix2=QPixmap("/home/aliceyang/Documents/OS/osMonitor/images/sys1.jpg");
    ui->curMemIm->setPixmap(pix2);
    ui->curMemIm->show();
    QPixmap pix3=QPixmap("/home/aliceyang/Documents/OS/osMonitor/images/search.jpg");
    ui->question->setPixmap(pix3);
    ui->question->show();
    StartPer();
    StartTab0();
    StartProCount();
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(SearchProgress()));
}
void MainWindow::StartProCount(){
    QTimer *Timer = new QTimer(this);
    connect(Timer,SIGNAL(timeout()),this,SLOT(ShowProgress2()));
    Timer->start(100);
}
void MainWindow::SearchProgress(){
    QString str=ui->lineEdit->text();
    int flag=1,k,i;
    char c;
    for( k = 0;; ++k) {
        c=str[k].toLatin1();
        if( !isdigit(c)) {
            break;
        }
    }
    if(k==str.length())
    {
        flag=0;
    }
    for(i=0;i<ql.length();i++)
    {

        if(QString::compare(ql[i][flag],str)==0||ql[i][flag].indexOf(str)==0)
        {
            ui->tableWidget->selectRow(i);
            break;
        }
    }
    if(i==ql.length())
    {
        ui->warn->setVisible(true);
    }
    else {
        EndPid=ql[i][0];
        ui->warn->setVisible(false);
    }
    ui->lineEdit->clear();
}
void MainWindow::ShowProgress2(){
    ui->tableWidget->setColumnCount(4);
    QStringList header;
    header<<"pid"<<"name"<<"state"<<"ppid";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setColumnWidth(0,150);
    ui->tableWidget->setColumnWidth(1,270);
    ui->tableWidget->setColumnWidth(2,230);
    ui->tableWidget->setColumnWidth(3,171);

    int count=0,pnum=0;
    RunPro=0;
    QStringList filelist;
    QString fname;
    QString pattern("[0-9]*");
    QRegExp rx(pattern);
    QDir dir("/proc");
    dir.setFilter(QDir::Dirs);
    QFileInfoList flist=dir.entryInfoList();
    ql.clear();
    foreach(QFileInfo f,flist){
        fname=f.fileName();
        bool match(rx.exactMatch(fname));
        if(match){
            ++count;

            QString fadd="/proc/"+fname+"/stat";
            QFile file(fadd);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString line = file.readLine();
                QStringList list;
                list = line.split(" ");
                int rows=ProgressNum>pnum?ProgressNum:pnum;
                ui->tableWidget->setRowCount(rows);

                list[1]=list[1].mid(1,list[1].length()-2);
                //removeAll(ql);
                ql.append(QStringList()<<list[0]<<list[1]);
                if(QString::compare(list[2],"R")==0)
                {
                    RunPro++;
                }
                for(int i=0;i<4;i++){
                    ui->tableWidget->setItem(pnum,i,new QTableWidgetItem(list[i]));
                }
                pnum++;
            }

     }

}
    ProgressNum=count;
    QString str=QString::number(count,10);
    str.insert(0,tr("All "));
    ui->RunNum->setText(str);
    ui->progressBar->setValue(RunPro);
}
void MainWindow::ShowCurTime()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss"); //设置显示格式
    ui->curTime->setText(str);
}
void MainWindow::StartPer()
{
    for(int i=0;i<240;i++){
        Buff[0][i] = 0;
        Buff[1][i] = 0;
        Buff[2][i] = 0;
    }
    for(int i=0;i<240;i++){
       Xvalue[i]=i-239;
    }
    CurrentData[0]=0;
    CurrentData[1]=0;
    CurrentData[2]=0;
    QTimer *paintTimer = new QTimer(this);
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(UpdateCpu()));
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(UpdateSwap()));
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(UpdateMemory()));
    paintTimer->start(500);
}
void MainWindow::StartPer1()
{
    for(int i=0;i<240;i++){
        Buff[1][i] = 0;
    }
    for(int i=0;i<240;i++){
       Xvalue[i]=i-239;
    }
    CurrentData[1]=0;
    QTimer *paintTimer = new QTimer(this);
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(UpdateMemory()));
    paintTimer->start(500);
}
void MainWindow::StartPer2()
{
    for(int i=0;i<240;i++){
        Buff[2][i] = 0;
    }
    for(int i=0;i<240;i++){
       Xvalue[i]=i-239;
    }
    CurrentData[2]=0;
    QTimer *paintTimer = new QTimer(this);
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(UpdateSwap()));
    paintTimer->start(500);
}

void MainWindow::UpdateMemory()
{
    QString filename="/proc/meminfo";
    QFile file(filename);
    QString line,mTotal,mAvailable;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mTotal=file.readLine();
        file.readLine();
        mAvailable=file.readLine();
    }
    QString mt=mTotal.split(":")[1].trimmed().split(" kB")[0];
    QString ma=mAvailable.split(":")[1].trimmed().split(" kB")[0];
    CurrentData[1]=100*(1-ma.toDouble()/mt.toDouble());
    ui->mem->setText(QString::number(CurrentData[1], 'f', 2).append("%"));
    ui->curMem->setText(QString::number(CurrentData[1], 'f', 2).append("%"));
    Paint(ui->customPlot_2,1);
}
void MainWindow::UpdateSwap()
{
    QString filename="/proc/meminfo";
    QFile file(filename);
    QString line,sTotal,sFree;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        for(int i =0; i < 14; i++){
            file.readLine();
        }
        sTotal=file.readLine();
        sFree=file.readLine();
    }
    QString st=sTotal.split(":")[1].trimmed().split(" kB")[0];
    QString sf=sFree.split(":")[1].trimmed().split(" kB")[0];
    CurrentData[2]=100*(1-sf.toDouble()/st.toDouble());
    ui->sw->setText(QString::number(CurrentData[2], 'f', 2).append("%"));
    Paint(ui->customPlot_3,2);
}
void MainWindow::StartTab(int i)
{
    if(!i)
    {
        StartTab0();
    }
    else if(i==1)
    {
        StartProCount();
    }
}

void MainWindow::StartTab0(){
    new QListWidgetItem(tr("System"),ui->system);
    QString host=QString("Host%1").arg("\t",10);
    QFile hostfile("/proc/sys/kernel/hostname");
    if(hostfile.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&hostfile);
        host+=textStream.readLine();
    }
    new QListWidgetItem(host,ui->system,2);

    QString type=QString("Type%1").arg("\t",9);
    QFile typefile("/proc/sys/kernel/ostype");
    if(typefile.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&typefile);
        type+=textStream.readLine();
    }
    new QListWidgetItem(type,ui->system,3);

    QString release=QString("Release%1").arg("\t",10);
    QFile releasefile("/proc/sys/kernel/osrelease");
    if(releasefile.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&releasefile);
        release+=textStream.readLine();
    }
    new QListWidgetItem(release,ui->system,4);

    QString cpu=QString("Cpu%1").arg("\t",11);
    QFile cpufile("/proc/cpuinfo");
    int ti=0;
    QString trr;
    if(cpufile.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&cpufile);
        for(;ti<5;ti++){
            trr=textStream.readLine();
        }
    }
    int mi=trr.indexOf(":");
    int mhi=trr.indexOf("@");
    cpu+=trr.mid(mi+1,27);
    QString mhz=trr.mid(mhi+1,trr.length()-1);
    new QListWidgetItem(cpu,ui->system,5);
    mhz.insert(0,QString("Cpu mhz%1").arg("\t",9));
    new QListWidgetItem(mhz,ui->system,6);

    new QListWidgetItem(tr("Time"),ui->runtime);
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(RunningTime()));
    timer->start(500);
}

void MainWindow::StartTab1()
{
    for(int i=0;i<240;i++){
        Buff[0][i] = 0;
    }
    for(int i=0;i<240;i++){
       Xvalue[i]=i-239;
    }
    CurrentData[0]=0;
    QTimer *paintTimer = new QTimer(this);
    connect(paintTimer,SIGNAL(timeout()),this,SLOT(UpdateCpu()));
    paintTimer->start(500);
}

void MainWindow::Paint(QCustomPlot*customPlot,int which){
    QVector<double> Yvalue(240);
    for(int i=0;i<239;i++){
        Buff[which][i]=Buff[which][i+1];
        Yvalue[i]=Buff[which][i];
    }
    Buff[which][239]=CurrentData[which];
    Yvalue[239]=Buff[which][239];
    customPlot->addGraph();
    QPen pen;
    pen.setWidth(1);
    switch (which) {
    case 0:
        pen.setColor(Qt::blue);
        customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
        break;
    case 1:
        pen.setColor(Qt::red);
        customPlot->graph(0)->setBrush(QBrush(QColor(255, 0, 0, 20)));
        break;
    case 2:
        pen.setColor(Qt::green);
        customPlot->graph(0)->setBrush(QBrush(QColor(0, 255, 0, 20)));
        break;
    default:
        break;
    }
    customPlot->graph(0)->setPen(pen);
    customPlot->graph(0)->setData(Xvalue,Yvalue);
    customPlot->xAxis->setLabel("time");
    customPlot->yAxis->setLabel("%");
    customPlot->xAxis->setRange(-240,0);
    customPlot->yAxis->setRange(0,100);
    customPlot->replot();

}
void MainWindow::UpdateCpu()
{
        QFile file("/proc/stat");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QString line = file.readLine();

            QStringList list;
            int current[4], used = 0, total = 0;
            bool ok;

            list = line.split(" ");

            for(int i = 2; i < 6; i++)
            {
                    cpuDataEnd[i-2] = list[i].toInt(&ok, 10);
                    current[i-2] = cpuDataEnd[i-2] - cpuDataStart[i-2];
                    cpuDataStart[i-2] = cpuDataEnd[i-2];
                    total += current[i-2];
            }
            used = total - current[3];
            CurrentData[0] = 100.0 * used / total;
            ui->ult->setText(QString::number(CurrentData[0], 'f', 2).append("%"));
            ui->curCpu->setText(QString::number(CurrentData[0], 'f', 2).append("%"));
            Paint(ui->customPlot,0);
        }
}
void MainWindow::RunningTime(){
    QString stime=QString("On for%1").arg("\t",6);
    QFile timefile("/proc/uptime");
    QString gt;
    if(timefile.open(QIODevice ::ReadOnly | QIODevice ::Text))
    {
        QTextStream textStream(&timefile);
        gt=textStream.readLine();
    }
    onTime=gt.left(7);
    stime+=onTime;
    stime+="s";
    ui->runtime->takeItem(1);
    ui->runtime->takeItem(1);
    new QListWidgetItem(stime,ui->runtime,1);

    QString rtime=QString("Run for%1").arg("\t",6);
    QString offt=gt.right(8);
    float sub1=onTime.toFloat();
    float sub2=offt.toFloat();
    sub2=sub2/4;
    float sub=sub1-sub2;
    QString runt=QString("%1s").arg(sub);
    rtime+=runt;
    //ui->runtime->takeItem(1);
    new QListWidgetItem(rtime,ui->runtime,2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(QString::compare(EndPid,"-1")!=0)
    {
        system("kill "+EndPid.toLatin1());
        //ProgressNum--;
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    system("systemctl poweroff -i");
}
