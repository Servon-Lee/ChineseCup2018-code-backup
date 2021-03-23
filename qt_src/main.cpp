#include "mainwindow.h"
#include "database.h"
#include <QApplication>
#include <QDateTime>
#include <connection.h>

#include <QtCore/QCoreApplication>

QString getQssContent()
{
    QFile styleSheet(":/qss.qss");
    if (!styleSheet.open(QIODevice::ReadOnly))
    {
        qWarning("Can't open the style sheet file.");
        return "";
    }
    return styleSheet.readAll();
}

//bool connStatus;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


//    connStatus =  createConnection();
//    initThread *thread = new initThread;
//    thread->start();

//    ConnThread *connthread = new ConnThread;
//    connthread->start();

//    a.setStyle(QStyleFactory::create("fusion"));
//    a.setWindowIcon(QIcon("src/logo.ico"));

    MainWindow w;
    w.show();

    a.setStyleSheet(getQssContent());

    return a.exec();
}
