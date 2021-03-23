#ifndef VIDEOANALYSIS_H
#define VIDEOANALYSIS_H

#include <QMainWindow>
#include <QUrl>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFileDialog>

#include <QMessageBox>
#include <QCloseEvent>
#include <detector.h>

namespace Ui {
class VideoAnalysis;
}

class VideoAnalysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoAnalysis(QWidget *parent = 0);
    ~VideoAnalysis();
    void VideoAnalysis::showImage(QImage image);

private:
    Ui::VideoAnalysis *ui;
    cv::VideoCapture capture;

    bool eventFilter(QObject *obj, QEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    bool mDrag;             //是否在拖动
    QPoint mDragPos;        //拖动起始点
    bool mIsMax;            //当前是否最大化
    QRect mLocation;        //最大化后恢复时的位置


public slots:
    void closeEvent(QCloseEvent *);
    int Video_to_Image(QString qs);
//    void Image_to_Video(long totalFrameNumber);


    void on_backButton_clicked();
private slots:
    void on_videoButton_clicked();
    void on_pauseButton_clicked();
    void on_closeButton_clicked();
    void on_btnMin0_clicked();
    void on_btnMax0_clicked();
    void on_btnExit0_clicked();
};

#endif // VIDEOANALYSIS_H
