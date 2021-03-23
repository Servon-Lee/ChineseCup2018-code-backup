#include "database.h"
#include "ui_database.h"
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QProcess>
#include <qtextcodec.h>
#include <QDesktopWidget>

extern bool connStatus;

Database::Database(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Database)
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

    QDir dir("C:/safehatvideo");
    QStringList nameFilters;
    nameFilters << "*.mp4";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

    if(connStatus == true)
    {
        QSqlQuery query("videotable");
        query.exec("SELECT name from videotable");
        //用dbList保存查询到的所有视频名称，便于后面使用
        QStringList dbList;
        while(query.next()){
            dbList.append(query.value(0).toString());
        }

        //查询文件夹中的视频是否在数据库中，如果不在则插入
        for(QString file:files){
            if(!dbList.contains(file)){
                query.prepare("INSERT INTO videotable(name, address) values(:vname, :vaddr)");
                query.bindValue(":vname", file);
                query.bindValue(":vaddr", "C:/safehatvideo/"+file);
                query.exec();
            }
        }

        model = new QSqlTableModel(this);
        model->setTable("videotable");
        model->select();

        // 设置编辑策略
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        ui->tableView->setModel(model);

        ui->tableView->setColumnWidth(0, 320);
        ui->tableView->setColumnWidth(1, 427);

    }

}

Database::~Database()
{
    delete ui;
}

void Database::on_submitButton_clicked()
{
    // 开始事务操作
    model->database().transaction();
    if(model->submitAll()){
        model->database().commit(); //提交
    }else{
        model->database().rollback(); //回滚
        QMessageBox::warning(this, tr("tableModel"),
        tr("数据库错误:%1").arg(model->lastError().text()));
    }
}

void Database::on_backButton_clicked()
{
    model->revertAll();
}

void Database::on_queryButton_clicked()
{
    QString name = ui->lineEdit->text();
    if(name.length()){
    //根据姓名进行筛选,一定要使用单引号
    model->setFilter(QString("name = '%1'").arg(name));
    model->select();
    }else{
        QMessageBox::warning(this, tr("Warning"),tr("名称不能为空") );
    }
}

void Database::on_queryAllButton_clicked()
{
    model->setTable("videotable");
    model->select();

    ui->tableView->setColumnWidth(0, 320);
    ui->tableView->setColumnWidth(1, 427);
}



void Database::on_deleteButton_clicked()
{
    // 获取选中的行
    int curRow = ui->tableView->currentIndex().row();
    // 删除该行
    model->removeRow(curRow);
    int ok = QMessageBox::warning(this,tr("删除当前行！"),
                                  tr("确定删除当前行吗?"),
                                  QMessageBox::Yes, QMessageBox::No);
    if(ok == QMessageBox::No)
    { // 如果不删除,则撤销
        model->revertAll();
    } else { // 否则提交,在数据库中删除该行，并且删除该文件
        model->submitAll();

        QModelIndex index = model->index(curRow,1);//获取视频地址的下标
        QString fileAddr = model->data(index).toString();//获取视频地址
        qDebug()<<"row:"<<curRow<<endl;
        qDebug()<<"index:"<<index<<endl;
        qDebug()<<"fileAddr:"<<fileAddr<<endl;
        QFile f(fileAddr);
        if(f.exists()){
            qDebug()<<"存在";
            f.remove();
        }else {
            qDebug()<<"NO";
        }
    }

}

void Database::on_addButton_clicked()
{
    // 获得表的行数
    int rowNum = model->rowCount();
    // 添加一行
    model->insertRow(rowNum);
    model->setData(model->index(rowNum,0),rowNum+1);
}

void Database::on_playButton_clicked()
{
    int curRow = ui-> tableView ->currentIndex().row();//获取鼠标所选中的行号
    QModelIndex index = model->index(curRow,1);//获取视频地址的下标
    QString fileAddr = model->data(index).toString();//获取视频地址
    //播放视频
    p = new playVideo(fileAddr);
    p->show();
}

void Database::on_queryInfoButton_clicked()
{
//    int curRow = ui-> tableView ->currentIndex().row();//获取鼠标所选中的行号
//    QString info = "notepad.exe D:/info/output.txt";
//    QProcess* process = new QProcess();
//    process->start(info);
}

bool Database::eventFilter(QObject *obj, QEvent *e)
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
void Database::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        mDrag = true;
        mDragPos = e->globalPos() - pos();
        e->accept();
    }
}

// 若鼠标左键被按下，移动窗体位置
void Database::mouseMoveEvent(QMouseEvent *e)
{
    if (mDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - mDragPos);
        e->accept();
    }
}

// 设置鼠标未被按下
void Database::mouseReleaseEvent(QMouseEvent *e)
{
    mDrag = false;
}

void Database::on_btnMin_clicked()
{
    showMinimized();
}

void Database::on_btnMax_clicked()
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

void Database::on_btnExit_clicked()
{
    this->close();
}
