#ifndef DATABASE_H
#define DATABASE_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QTableView>

#include <QMessageBox>
#include <QCloseEvent>

#include "playvideo.h"

namespace Ui {
class Database;
}

class QSqlTableModel;

class Database : public QMainWindow
{
    Q_OBJECT

public:
    explicit Database(QWidget *parent = 0);
    ~Database();

private slots:
    void on_submitButton_clicked();

    void on_backButton_clicked();

    void on_queryButton_clicked();

    void on_queryAllButton_clicked();

    void on_deleteButton_clicked();

    void on_addButton_clicked();

    void on_playButton_clicked();

    void on_queryInfoButton_clicked();

    void on_btnMin_clicked();

    void on_btnMax_clicked();

    void on_btnExit_clicked();

private:
    Ui::Database *ui;
    QSqlTableModel *model;
    playVideo *p;

    bool eventFilter(QObject *obj, QEvent *e);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    bool mDrag;             //是否在拖动
    QPoint mDragPos;        //拖动起始点
    bool mIsMax;            //当前是否最大化
    QRect mLocation;        //最大化后恢复时的位置
};

#endif // DATABASE_H
