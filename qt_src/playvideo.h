#ifndef PLAYVIDEO_H
#define PLAYVIDEO_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QCloseEvent>
#include <QString>
#include <QFileDialog>

#include <QMessageBox>
#include <QCloseEvent>
#include <QCloseEvent>

namespace Ui {
class playVideo;
}

class playVideo : public QWidget
{
    Q_OBJECT

public:
    explicit playVideo(QWidget *parent = 0);
    explicit playVideo(QString username, QWidget *parent = 0);
    ~playVideo();

private slots:
    void on_pauseButton_clicked();

    void on_closeButton_clicked();

    void closeEvent(QCloseEvent *);

    void on_btnMin_clicked();

    void on_btnMax_clicked();

    void on_btnExit_clicked();

private:
    QString name;
    Ui::playVideo *ui;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;

    bool eventFilter(QObject *obj, QEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    bool mDrag;             //是否在拖动
    QPoint mDragPos;        //拖动起始点
    bool mIsMax;            //当前是否最大化
    QRect mLocation;        //最大化后恢复时的位置
};

#endif // PLAYVIDEO_H
