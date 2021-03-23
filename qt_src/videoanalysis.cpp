#include "videoanalysis.h"
#include "ui_videoanalysis.h"
#include <Windows.h>
#include <iostream>
#include <QDebug>
#include <vector>
#include <QDesktopWidget>
#include <time.h>
#include <QDateTime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <detector.h>
#include <initthread.h>

using namespace cv;
using namespace std;

extern Detector *detector;
std::vector<bbox_t> vector_bbox_t;
VideoWriter writer;

VideoAnalysis::VideoAnalysis(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VideoAnalysis)
{
    if(detector == NULL){
        initThread *thread = new initThread;
        thread->start();
    }

    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/png/src/logo.png"));

    // 设置无边框
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);

    mLocation = this->geometry();
    mIsMax = false;
    mDrag = false;

    ui->widgetTitle->installEventFilter(this);
    ui->btnMin0->setIcon(QIcon(":/png/src/min.png"));
    ui->btnMax0->setIcon(QIcon(":/png/src/max1.png"));
    ui->btnExit0->setIcon(QIcon(":/png/src/exit.png"));

    setAttribute(Qt::WA_DeleteOnClose, true);

//    on_videoButton_clicked();
}

QImage Mat2QImage(cv::Mat &image)
{
    QImage img;

    if(image.channels()==3)
    {
        //cvt Mat BGR 2 QImage RGB
        cv::cvtColor(image,image,CV_BGR2RGB);
        img =QImage((const unsigned char*)(image.data),
                    image.cols,image.rows,
                    image.cols*image.channels(),
                    QImage::Format_RGB888);
    }
    else
    {
        img =QImage((const unsigned char*)(image.data),
                    image.cols,image.rows,
                    image.cols*image.channels(),
                    QImage::Format_RGB888);
    }

    return img;
}

struct myBox{
    CvPoint leftup, rightbot;
    int perOrHat;
};

float IOU(int left1,int right1,int top1,int bot1,int left2,int right2,int top2,int bot2)
{
    float garea=(right2-left2)*(bot2-top2);
    float xL=max(left1,left2);
    float yL=max(top1,top2);
    float xR=min(right1,right2);
    float yR=min(bot1,bot2);
    float w=max(0,xR-xL);
    float h=max(0,yR-yL);
    float area=w*h;
    float iou=area/garea;

    return iou;
}


int VideoAnalysis::Video_to_Image(QString qs)
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString curtime = current_date_time.toString("yyyy-MM-dd_hh-mm-ss");

    cv::String filename = qs.toUtf8().constData();
    capture.open(filename);//获取VideoCapture 对象
    if(!capture.isOpened())
        cout<<"fail to open!"<<endl;

    //获取相关信息
    long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
    int width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    int FPS = capture.get(CV_CAP_PROP_FPS);

    QString videoname = "C:/safehatvideo/"+curtime+".mp4";
    cv::String cv_videoname = videoname.toUtf8().constData();

    writer.open(cv_videoname, CV_FOURCC('X','V','I','D'), FPS, Size(width, height));

    //设置开始帧()
    long frameToStart = 1;
    capture.set( CV_CAP_PROP_POS_FRAMES,frameToStart);

    //设置结束帧
    int frameToStop = totalFrameNumber;

    if(frameToStop < frameToStart)
    {
        cout<<"frameToStop < frameToStart"<<endl;
        return -1;
    }
    else
    {
        cout<<"overed at:"<<frameToStop<<"frame"<<endl;
    }

    //定义一个用来控制读取视频循环结束的变量
    bool stop = false;

    //承载每一帧的图像
    Mat frame;

    cvNamedWindow("raw");
//    HWND hWnd = (HWND)cvGetWindowHandle("raw");
//    HWND hRawWnd = ::GetParent(hWnd); //获得父窗口句柄
//    ShowWindow(hWnd, 0);     //0：的时候表示隐藏子窗口
//    ShowWindow(hRawWnd, 0);  //0：的时候表示隐藏父窗口

    //currentFrame是在循环体中控制读取到指定的帧后循环结束的变量
    long currentFrame = frameToStart;

    CvScalar color_green;
    color_green.val[0] = 0;
    color_green.val[1] = 255;
    color_green.val[2] = 0;

    CvScalar color_red;
    color_red.val[0] = 0;
    color_red.val[1] = 0;
    color_red.val[2] = 255;

    //人与安全帽交集的阈值
    double threshold = 0.2;

    int i, j;

    while(!stop)
    {
        //读取下一帧
        if(!capture.read(frame))
        {
            cout<<"Over!"<<endl;
            break;
        }

        //识别到的所有框
        vector_bbox_t = detector->detect(frame);

        struct myBox vper[100], vhat[100];
        int daicnt = 0;	//戴了安全帽的人数
        int width = (int)capture.get(CAP_PROP_FRAME_WIDTH) * .006;

        int perBoxlen = 0;
        int hatBoxlen = 0;

        for(std::vector<bbox_t>::iterator it=vector_bbox_t.begin(); it!=vector_bbox_t.end(); it++){
            std::cout <<"("<< it->x<<","<<it->y<<","<<it->w<<","<<it->h<<")"<<endl;
            std::cout <<it->prob<<endl;
            std::cout <<it->obj_id<<endl;

            CvPoint pt1, pt2;//leftup、rightbot
            pt1.x = it->x;
            pt1.y = it->y;
            pt2.x = it->x + it->w;
            pt2.y = it->y + it->h;

            struct myBox mb;
            mb.leftup = pt1;
            mb.rightbot = pt2;

            if(it->obj_id == 0){ //识别的对象是person
                mb.perOrHat = 0;
                vper[perBoxlen++] = mb;
            }else if(it->obj_id == 1){ //识别的对象是hat
                mb.perOrHat = 1;
                vhat[hatBoxlen++] = mb;
            }

        }

        for(j=0; j<hatBoxlen; j++){ // 为每个hat画框
            cv::rectangle(frame, vhat[j].leftup, vhat[j].rightbot, color_green, width, 8, 0);
        }

        for(i=0; i<perBoxlen; i++){ // 遍历每个person的框
            int flag = 0;
            for(j=0; j<hatBoxlen; j++){ // 遍历每个hat的框
                // 戴了安全帽
                if(IOU(vper[i].leftup.x, vper[i].rightbot.x, vper[i].leftup.y, vper[i].leftup.y + (vper[i].rightbot.y-vper[i].leftup.y)/2,
                    vhat[j].leftup.x, vhat[j].rightbot.x, vhat[j].leftup.y, vhat[j].rightbot.y) > threshold){
                    flag = 1;
                    daicnt++;
                    break;
                }
            }
            if (flag == 1) {//为戴了安全帽的人画框
                cv::rectangle(frame, vper[i].leftup, vper[i].rightbot, color_green, width, 8, 0);
            }
            else {//为未戴安全帽的人画框
                cv::rectangle(frame, vper[i].leftup, vper[i].rightbot, color_red, width, 8, 0);
            }
        }

        ui->people1->setText(QString::number(perBoxlen, 10));
        ui->people2->setText(QString::number(perBoxlen - daicnt, 10));

        if(writer.isOpened()){
            qDebug()<<"writer isOpened!"<<endl;
            writer.write(frame);
        }else {
            qDebug()<<"writer is NOT Opened!";
        }


        QImage image = Mat2QImage(frame);
        ui->hint->setPixmap(QPixmap::fromImage(image));

        cout<<"reading"<<currentFrame<<"frame"<<endl;

        //waitKey(int delay=0)当delay ≤ 0时会永远等待；当delay>0时会等待delay毫秒
        //当时间结束前没有按键按下时，返回值为-1；否则返回按键
        int c = waitKey(1);
        //按下ESC或者到达指定的结束帧后退出读取视频
        if((char) c == 27 || currentFrame > frameToStop)
        {
            stop = true;
        }
        //按下按键后会停留在当前帧，等待下一次按键
        if( c >= 0 )
        {
            waitKey(0);
        }
        currentFrame++;

    }
    //关闭视频文件
//    cvDestroyWindow("raw");


    frame.release();

    if(writer.isOpened()){
        writer.release();
    }
    if(capture.isOpened()){
        capture.release();
    }

    return 0;
}

VideoAnalysis::~VideoAnalysis()
{
    delete ui;
}

void VideoAnalysis::closeEvent( QCloseEvent * event )
{
    if(writer.isOpened()){
        writer.release();
    }
    if(capture.isOpened()){
        capture.release();
    }
    cvDestroyAllWindows();
    event->accept();
}

void VideoAnalysis::on_backButton_clicked()
{
    if(writer.isOpened()){
        writer.release();
    }
    if(capture.isOpened()){
        capture.release();
    }
    cvDestroyAllWindows();
    this->close();
}

void VideoAnalysis::on_videoButton_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("选择视频"));
    fileDialog->setDirectory(".");
    fileDialog->setNameFilter(tr("Videos(*.mp4 *.mkv *.mov *.avi *.flv)"));
    fileDialog->setViewMode(QFileDialog::Detail);

    QString fileName;
    if(fileDialog->exec())
    {
        //获取所要播放的视频名称
        fileName = fileDialog->selectedFiles()[0];

        Video_to_Image(fileName);
    }
}

void VideoAnalysis::on_pauseButton_clicked()
{

}

void VideoAnalysis::on_closeButton_clicked()
{
    if(writer.isOpened()){
        writer.release();
    }
    if(capture.isOpened()){
        capture.release();
    }
}

bool VideoAnalysis::eventFilter(QObject *obj, QEvent *e)
{
    if (obj == ui->widgetTitle)
    {
        if(e->type() == QEvent::MouseButtonDblClick)
        {
            on_btnMax0_clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj, e);
}

// 鼠标相对于窗体的位置 event->globalPos() - this->pos()
void VideoAnalysis::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

// 若鼠标左键被按下，移动窗体位置
void VideoAnalysis::mouseMoveEvent(QMouseEvent *e)
{
    if (mDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}

// 设置鼠标未被按下
void VideoAnalysis::mouseReleaseEvent(QMouseEvent *e)
{
    mDrag = false;
}

void VideoAnalysis::on_btnMin0_clicked()
{
    showMinimized();
}

void VideoAnalysis::on_btnMax0_clicked()
{
    if (mIsMax)
    {
        setGeometry(mLocation);
        ui->btnMax0->setIcon(QIcon(":/png/src/max1.png"));
    }
    else
    {
        mLocation = geometry();
        setGeometry(qApp->desktop()->availableGeometry());
        ui->btnMax0->setIcon(QIcon(":/png/src/max2.png"));
    }
    mIsMax = !mIsMax;
}

void VideoAnalysis::on_btnExit0_clicked()
{
    if(writer.isOpened()){
        writer.release();
    }
    if(capture.isOpened()){
        capture.release();
    }
    cvDestroyAllWindows();
    this->close();
}
