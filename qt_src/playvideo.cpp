#include "playvideo.h"
#include "ui_playvideo.h"
#include <QPalette>
#include <QDesktopWidget>

bool flag = 1;

playVideo::playVideo(QString username, QWidget *parent) :
    QWidget(parent), name(username),
    ui(new Ui::playVideo)
{
    ui->setupUi(this);
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

    player = new QMediaPlayer;
    videoWidget = new QVideoWidget;

    QPalette pal(videoWidget->palette());
    pal.setColor(QPalette::Background, Qt::black);
    videoWidget->setAutoFillBackground(true);
    videoWidget->setPalette(pal);
    videoWidget->show();

    player->setVideoOutput(videoWidget);
    ui->verticalLayout->addWidget(videoWidget);

    player->setMedia(QUrl::fromLocalFile(name));
    videoWidget->show();
    player->play();
}

playVideo::~playVideo()
{
    delete ui;
}

void playVideo::on_pauseButton_clicked()
{
    if(flag == 1){
        player->pause();
    }else {
        player->play();
    }
    flag = !flag;
}

void playVideo::on_closeButton_clicked()
{
    player->stop();
}

void playVideo::closeEvent( QCloseEvent * event )
{
    player->stop();
    event->accept();
}

bool playVideo::eventFilter(QObject *obj, QEvent *e)
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
void playVideo::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

// 若鼠标左键被按下，移动窗体位置
void playVideo::mouseMoveEvent(QMouseEvent *e)
{
    if (mDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}

// 设置鼠标未被按下
void playVideo::mouseReleaseEvent(QMouseEvent *e)
{
    mDrag = false;
}

void playVideo::on_btnMin_clicked()
{
    showMinimized();
}

void playVideo::on_btnMax_clicked()
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

void playVideo::on_btnExit_clicked()
{
    this->close();
}
