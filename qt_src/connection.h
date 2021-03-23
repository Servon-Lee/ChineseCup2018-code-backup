#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QtSql>
#include <QStyleFactory>

/**
 * @brief createConnection
 * 创建与数据库的连接
 */
static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("120.79.152.38");
    db.setDatabaseName("ChineseCup");
    db.setUserName("root");
    db.setPort(3306);
    db.setPassword("root");
    if (db.open())
    {
        qDebug()<<"suceess Connect!";
        return true;
    }
    else
    {
        qDebug()<<"error Connect!";
        return false;
    }

}

#endif // CONNECTION_H
