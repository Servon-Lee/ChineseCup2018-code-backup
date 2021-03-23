#ifndef CONNTHREAD_H
#define CONNTHREAD_H
#include <connection.h>

class ConnThread : public QThread
{
    Q_OBJECT
public:
    ConnThread();
    virtual void run();
};

#endif // CONNTHREAD_H
