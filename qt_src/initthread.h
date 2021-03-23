#ifndef INITTHREAD_H
#define INITTHREAD_H
#include<QThread>
#include<QString>
#include <QDebug>
#include <detector.h>

class initThread : public QThread
{
    Q_OBJECT
public:
    initThread();
    virtual void run();

};

#endif // INITTHREAD_H
