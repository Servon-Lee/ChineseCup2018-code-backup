#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"
#include <main.cpp>

#include <initthread.h>
#include <QDesktopWidget>
#include <QIcon>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#include <Windowsx.h>
#endif

//bool connStatus;
using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize( this->width (),this->height ());
    this->setWindowTitle("智能安全帽监测系统！");
    this->setWindowIcon(QIcon(":/png/src/logo.png"));

    // 设置无边框
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;

    ui->widgetTitle->installEventFilter(this);
    ui->btnMin->setIcon(QIcon(":/png/src/min.png"));
    ui->btnMax->setIcon(QIcon(":/png/src/max1.png"));
    ui->btnExit->setIcon(QIcon(":/png/src/exit.png"));



    // 检查目录是否存在，若不存在则新建
    QString dir_str = "C:/safehatvideo";
    QDir dir;
    if (!dir.exists(dir_str)){
        bool res = dir.mkpath(dir_str);
        qDebug() << "新建video目录成功:" << res;
    }

//    dir_str = "C:/safehatinfo";
//    if (!dir.exists(dir_str)){
//        bool res = dir.mkpath(dir_str);
//        qDebug() << "新建info目录成功:" << res;
//    }

    connect(ui->videoButton, SIGNAL(clicked()), this, SLOT(openVideoAalysis()));
    connect(ui->realtimeButton, SIGNAL(clicked()), this, SLOT(openRealtimeAnalysis()));
    connect(ui->exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    connthread = new ConnThread;
    connthread->start();

    thread = new initThread;
    thread->start();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openVideoAalysis()
{
    v = new VideoAnalysis;
    v->show();
}

void MainWindow::openRealtimeAnalysis()
{
    r = new RealtimeAnalysis;
    r->show();
}

void MainWindow::on_QueryButton_clicked()
{
//    connStatus = createConnection();
    d = new Database;
    d->show();
}

void MainWindow::closeEvent( QCloseEvent * event )
{
   switch( QMessageBox::information( this, tr("智能安全监控系统"),
                                     tr("您确定要退出吗?"),
                                     tr("Yes"), tr("No"),
                                     0, 1 ))
   {
   case 0:
       event->accept();
       break;
   case 1:
   default:
       event->ignore();
       break;
   }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widgetTitle)
    {
        if(e->type() == QEvent::MouseButtonDblClick)
        {
            on_btnMax_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}

// 鼠标相对于窗体的位置 event->globalPos() - this->pos()
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

// 若鼠标左键被按下，移动窗体位置
void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (mDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}

// 设置鼠标未被按下
void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    mDrag = false;
}

void MainWindow::on_btnMin_clicked()
{
    showMinimized();
}

void MainWindow::on_btnMax_clicked()
{
    if (mIsMax)
    {
        setGeometry(mLocation);
        ui->btnMax->setIcon(QIcon(":/png/src/max1.png"));
    }
    else
    {
        mLocation = geometry();
        setGeometry(qApp->desktop()->availableGeometry());
        ui->btnMax->setIcon(QIcon(":/png/src/max2.png"));
    }
    mIsMax = !mIsMax;
}

void MainWindow::on_btnExit_clicked()
{
    qApp->exit();
}
