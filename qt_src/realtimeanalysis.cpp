#include "realtimeanalysis.h"
#include "ui_realtimeanalysis.h"
#include <Windows.h>
#include <QTimer>
#include <QDateTime>
#include <QDesktopWidget>
#include <initthread.cpp>

#include <videoanalysis.h>


using namespace cv;

std::vector<bbox_t> vector_bbox_t2;
Mat frame;
QImage image;
extern Detector *detector;
VideoWriter writer2;

RealtimeAnalysis::RealtimeAnalysis(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RealtimeAnalysis)
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
    ui->btnMin->setIcon(QIcon(":/png/src/min.png"));
    ui->btnMax->setIcon(QIcon(":/png/src/max1.png"));
    ui->btnExit->setIcon(QIcon(":/png/src/exit.png"));

    // 初始化
    capture = NULL;
    timer = new QTimer(this);
    imag = new QImage();

//    on_OpenCameraBtn_clicked();

    connect(timer, SIGNAL(timeout()), this, SLOT(getFrame()));//超时就读取当前摄像头信息

}

RealtimeAnalysis::~RealtimeAnalysis()
{
    delete ui;
}


QImage  Mat2QImage(cv::Mat cvImg)
{
    QImage qImg;
    if (cvImg.channels() == 3)                             //3 channels color image
    {

        cv::cvtColor(cvImg, cvImg, CV_BGR2RGB);
        qImg = QImage((const unsigned char*)(cvImg.data),
            cvImg.cols, cvImg.rows,
            cvImg.cols*cvImg.channels(),
            QImage::Format_RGB888);
    }
    else if (cvImg.channels() == 1)                    //grayscale image
    {
        qImg = QImage((const unsigned char*)(cvImg.data),
            cvImg.cols, cvImg.rows,
            cvImg.cols*cvImg.channels(),
            QImage::Format_Indexed8);
    }
    else
    {
        qImg = QImage((const unsigned char*)(cvImg.data),
            cvImg.cols, cvImg.rows,
            cvImg.cols*cvImg.channels(),
            QImage::Format_RGB888);
    }

    return qImg;

}

void RealtimeAnalysis::on_OpenCameraBtn_clicked()
{
    capture.open(0);//打开摄像头，从摄像头中获取视频
    if(!capture.isOpened())
    {
        qDebug()<<"open error!";
    }

    QDateTime current_date_time =QDateTime::currentDateTime();
    QString curtime = current_date_time.toString("yyyy-MM-dd_hh-mm-ss");

    //获取相关信息
    int width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    int FPS = capture.get(CV_CAP_PROP_FPS);
    qDebug()<<"FPS:"<<FPS<<endl;

    QString videoname = "C:/safehatvideo/"+curtime+".mp4";
    cv::String cv_videoname = videoname.toUtf8().constData();

    writer2.open(cv_videoname, CV_FOURCC('X','V','I','D'), 15, Size(width, height));
    if(writer2.isOpened()){
       qDebug()<<"writer2 isOpened()"<<endl;
    }

    timer->start(50);//开始计时，超时则发出timeout()信号,1000为1秒，50毫秒取一帧
}

struct myBox{
    CvPoint leftup, rightbot;
    int perOrHat;
};

float IOU2(int left1,int right1,int top1,int bot1,int left2,int right2,int top2,int bot2)
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

void RealtimeAnalysis::getFrame()
{

    if(!capture.read(frame)){
        std::cout<<"frame empty"<<endl;
        return;
    }

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

    vector_bbox_t2 = detector->detect(frame);

    struct myBox vper[100], vhat[100];
    int daicnt = 0;	//戴了安全帽的人数
    int width = (int)capture.get(CAP_PROP_FRAME_WIDTH) * .006;

    int perBoxlen = 0;
    int hatBoxlen = 0;

    for(std::vector<bbox_t>::iterator it=vector_bbox_t2.begin(); it!=vector_bbox_t2.end(); it++){
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
            if(IOU2(vper[i].leftup.x, vper[i].rightbot.x, vper[i].leftup.y, vper[i].leftup.y + (vper[i].rightbot.y-vper[i].leftup.y)/2,
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

    if(writer2.isOpened()){
        qDebug()<<"writer2 isOpened!"<<endl;
        writer2.write(frame);
    }else {
        qDebug()<<"writer2 is NOT Opened!";
    }

    ui->people1->setText(QString::number(perBoxlen, 10));
    ui->people2->setText(QString::number(perBoxlen - daicnt, 10));

    image = Mat2QImage(frame);
    ui->hint->setPixmap(QPixmap::fromImage(image));
}

void RealtimeAnalysis::on_TakePicBtn_clicked()
{
    ui->statusBar->showMessage(tr("capturing..."), 1000);
    ui->ImageCapture->setPixmap(QPixmap::fromImage(image));//将截取的图片显示到ImageCapture上
    ui->statusBar->showMessage(tr("capture OK!"), 5000);
}

void RealtimeAnalysis::on_CloseCameraBtn_clicked()
{
    timer->stop(); //停止取帧
    if(writer2.isOpened()){
        writer2.release();
    }
    if(capture.isOpened()){
        capture.release();//释放资源
    }
}

void RealtimeAnalysis::on_saveButton_clicked()
{
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString curtime = current_date_time.toString("yyyyMMddhhmmss");

    QString fileName=QFileDialog::getSaveFileName(this, tr("save file"), "savePic"+curtime, tr("JPEG(*.jpg;*.jpeg)"));
    if(fileName.isEmpty()){
        ui->statusBar->showMessage(tr("save cancel"), 5000);
        return;
    }
    const QPixmap* pixmap=ui->ImageCapture->pixmap();
    if(pixmap){
        pixmap->save(fileName);
        ui->statusBar->showMessage(tr("save OK"), 5000);
    }
}

void RealtimeAnalysis::on_backButton_clicked()
{
    timer->stop(); //停止取帧
    if(writer2.isOpened()){
        writer2.release();
    }
    if(capture.isOpened()){
        capture.release();//释放资源
    }
    this->close();

}

bool RealtimeAnalysis::eventFilter(QObject *obj, QEvent *e)
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
void RealtimeAnalysis::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

// 若鼠标左键被按下，移动窗体位置
void RealtimeAnalysis::mouseMoveEvent(QMouseEvent *e)
{
    if (mDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}

// 设置鼠标未被按下
void RealtimeAnalysis::mouseReleaseEvent(QMouseEvent *e)
{
    mDrag = false;
}

void RealtimeAnalysis::on_btnMin_clicked()
{
    showMinimized();
}

void RealtimeAnalysis::on_btnMax_clicked()
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

void RealtimeAnalysis::on_btnExit_clicked()
{
    if(writer2.isOpened()){
        writer2.release();
    }
    if(capture.isOpened()){
        capture.release();
    }
    this->close();
}
