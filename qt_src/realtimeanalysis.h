#ifndef REALTIMEANALYSIS_H
#define REALTIMEANALYSIS_H

#include <QMainWindow>
#include <QFileDialog>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

#include <QMessageBox>
#include <QCloseEvent>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <detector.h>

#include <QImage>
#include <QTimer>
#include <initthread.h>
#include <connthread.h>

namespace Ui {
class RealtimeAnalysis;
}

class QCamera;
class QCameraViewfinder;
class QCameraImageCapture;

class RealtimeAnalysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit RealtimeAnalysis(QWidget *parent = 0);
    ~RealtimeAnalysis();

private:
    Ui::RealtimeAnalysis *ui;
    QImage *imag;
    cv::VideoCapture capture;
    QTimer *timer;//定时器用于定时取帧

    bool eventFilter(QObject *obj, QEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    bool mDrag;             //是否在拖动
    QPoint mDragPos;        //拖动起始点
    bool mIsMax;            //当前是否最大化
    QRect mLocation;        //最大化后恢复时的位置

public slots:
//    void closeEvent(QCloseEvent *);

private slots:
    void on_OpenCameraBtn_clicked();
    void on_CloseCameraBtn_clicked();
    void on_TakePicBtn_clicked();
    void getFrame();
    void on_saveButton_clicked();
    void on_backButton_clicked();
    void on_btnMin_clicked();
    void on_btnMax_clicked();
    void on_btnExit_clicked();
};

#endif // REALTIMEANALYSIS_H
