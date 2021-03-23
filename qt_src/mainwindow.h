#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUrl>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

#include <QMessageBox>
#include <QCloseEvent>
#include <QDir>

#include "videoanalysis.h"
#include "realtimeanalysis.h"
#include "database.h"
#include "initthread.h"
#include "connthread.h"


namespace Ui {
class MainWindow;
}

class QCamera;
class QCameraViewfinder;
class QCameraImageCapture;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    initThread *thread;
    ConnThread *connthread;

private:
    Ui::MainWindow *ui;
    VideoAnalysis *v;
    RealtimeAnalysis *r;
    Database *d;

    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;

    bool eventFilter(QObject *obj, QEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    bool mDrag;             //是否在拖动
    QPoint mDragPos;        //拖动起始点
    bool mIsMax;            //当前是否最大化
    QRect mLocation;        //最大化后恢复时的位置

public slots:
    void openVideoAalysis();
    void openRealtimeAnalysis();
    void on_QueryButton_clicked();

    void closeEvent(QCloseEvent *);
private slots:
    void on_btnMin_clicked();
    void on_btnMax_clicked();
    void on_btnExit_clicked();
};

#endif // MAINWINDOW_H
